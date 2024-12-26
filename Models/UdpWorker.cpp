#include "Headers/UdpWorker.h"
#include "Headers/GlobalParams.h"
#include "Library/Mavlink2/mavlink_helpers.h"

UdpWorker::UdpWorker(QObject *parent)
    : QObject(parent) {
}

UdpWorker::~UdpWorker() {
    cleanup();
}

void UdpWorker::initialize() {
    if (!udpSocket) {
        udpSocket = new QUdpSocket(this);
        connect(udpSocket, &QUdpSocket::errorOccurred,
                this, &UdpWorker::handleSocketError);
        connect(udpSocket, &QUdpSocket::stateChanged,
                this, &UdpWorker::handleStateChange);
        connect(udpSocket, &QUdpSocket::readyRead,
                this, &UdpWorker::ReadData);
    }

    if (!connectionTimer) {
        connectionTimer = new QTimer(this);
        connectionTimer->setSingleShot(true);
        connect(connectionTimer, &QTimer::timeout,
                this, &UdpWorker::handleConnectionTimeout);
    }
    if (!signalCheckTimer) {
        signalCheckTimer = new QTimer(this);
        signalCheckTimer->setInterval(1000); // Check every second
        connect(signalCheckTimer, &QTimer::timeout,
                this, &UdpWorker::checkSignalStrength);
    }
}

void UdpWorker::checkSignalStrength() {
    auto &globals = GlobalParams::getInstance();
    signalCheckAttempts++;

    // 5 saniye içinde (5 deneme x 1 saniye interval) heartbeat gelmezse bağlantıyı kes
    if (signalCheckAttempts >= MAX_SIGNAL_CHECK_ATTEMPTS) {
        if (globals.getMavlinkSignalStrength() > MIN_SIGNAL_STRENGTH) {
            // Signal strength is good, stop checking
            signalCheckTimer->stop();
            signalCheckAttempts = 0;
            emit showMessage("Connection Status", "Connection established successfully", "green", 3000);
            emit connectionStateChanged(true);
        } else {
            // 5 saniye boyunca heartbeat gelmedi
            signalCheckTimer->stop();
            signalCheckAttempts = 0;
            handleDisconnect();
            emit showMessage("Connection Error", "No Mavlink Connection", "red", 3000);
            emit connectionStateChanged(false);
        }
    }
}
void UdpWorker::startConnectionTimer() {
    if (connectionTimer) {
        connectionTimer->start(CONNECTION_TIMEOUT);
    }
}

void UdpWorker::stopConnectionTimer() {
    if (connectionTimer) {
        connectionTimer->stop();
    }
}


void UdpWorker::handleConnect(const QString &ip, int port) {
    Q_UNUSED(ip);
    QMutexLocker locker(&mutex);
    auto &globals = GlobalParams::getInstance();

    if (port <= 0 || port > 65535) {
        stopConnectionTimer();
        emit showMessage("Error", "Invalid port number", "red", 3000);
        return;
    }

    if (udpSocket->state() == QAbstractSocket::BoundState) {
        handleDisconnect();
    }

    startConnectionTimer();
    emit showMessage("Connection Status", "Waiting for MAVLink Connection...", "blue", 5000);

    // Only bind to the local port, don't set remote endpoint yet
    if (!udpSocket->bind(QHostAddress::Any, port, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint)) {
        emit showMessage("Error", "Failed to bind UDP socket", "red", 3000);
        return;
    }

    bindPort = port;
    remoteEndpointEstablished = false; // Reset remote endpoint flag

    globals.setUdpPort(port);
    globals.setUdpConnectionState(true);
    GlobalParams::getInstance().setActiveConnectionType(GlobalParams::ConnectionType::UDP);

    signalCheckAttempts = 0;
    signalCheckTimer->start();
}


void UdpWorker::handleDisconnect() {
    QMutexLocker locker(&mutex);
    auto &globals = GlobalParams::getInstance();

    startConnectionTimer();
    emit showMessage("Connection Status", "Disconnecting...", "blue", 3000);

    udpSocket->close();
    globals.setUdpConnectionState(false);
    emit connectionStateChanged(false);
    GlobalParams::getInstance().setActiveConnectionType(GlobalParams::ConnectionType::None);
    stopConnectionTimer();
}

QString UdpWorker::getErrorMessage(QAbstractSocket::SocketError socketError) {
    switch (socketError) {
    case QAbstractSocket::ConnectionRefusedError:
        return "Connection refused by host";
    case QAbstractSocket::SocketAccessError:
        return "Socket access error";
    case QAbstractSocket::SocketResourceError:
        return "Socket resource error";
    case QAbstractSocket::SocketTimeoutError:
        return "Connection attempt timed out";
    case QAbstractSocket::NetworkError:
        return "Network error occurred";
    default:
        return udpSocket->errorString();
    }
}

void UdpWorker::handleSocketError(QAbstractSocket::SocketError socketError) {
    auto &globals = GlobalParams::getInstance();
    emit showMessage("Connection Error", getErrorMessage(socketError), "red", 4700);
    globals.setUdpConnectionState(false);
    emit connectionStateChanged(false);

    udpSocket->close();
}

void UdpWorker::handleStateChange(QAbstractSocket::SocketState socketState) {
    auto &globals = GlobalParams::getInstance();

    switch (socketState) {
    case QAbstractSocket::UnconnectedState:
        globals.setUdpConnectionState(false);
        emit connectionStateChanged(false);
        break;
    case QAbstractSocket::BoundState:
        globals.setUdpConnectionState(true);
        emit connectionStateChanged(true);
        break;
    default:
        break;
    }
}

void UdpWorker::handleConnectionTimeout() {
    auto &globals = GlobalParams::getInstance();

    if (udpSocket->state() != QAbstractSocket::BoundState) {
        udpSocket->close();
        globals.setUdpConnectionState(false);
        emit connectionStateChanged(false);
        emit showMessage("Error", "Connection attempt timed out", "red", 3000);
    }
}



void UdpWorker::ReadData() {
    while (udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;

        udpSocket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);

        // If this is the first packet, establish the remote endpoint
        if (!remoteEndpointEstablished) {
            remoteAddress = sender;
            remotePort = senderPort;
            remoteEndpointEstablished = true;

            auto &globals = GlobalParams::getInstance();
            globals.setUdpIp(sender.toString());

            emit showMessage("Connection Status",
                             "Remote endpoint established: " + sender.toString() + ":" + QString::number(senderPort),
                             "green", 3000);
        }

        // Process the MAVLink message
        for (int i = 0; i < datagram.size(); i++) {
            if (mavlink_parse_char(MAVLINK_COMM_0, static_cast<uint8_t>(datagram[i]), &msg, &status)) {
                emit processMAVLinkMessage(msg);
            }
        }
    }
}
void UdpWorker::sendMavlinkMessage(const mavlink_message_t& msg) {
    QMutexLocker locker(&mutex);

    if (!udpSocket || udpSocket->state() != QAbstractSocket::BoundState) {
        emit showMessage("Error", "UDP socket not bound!", "red", 3000);
        return;
    }

    if (!remoteEndpointEstablished) {
        return;
    }

    uint16_t len = mavlink_msg_to_send_buffer(m_sendBuffer, &msg);
    QByteArray datagram(reinterpret_cast<const char*>(m_sendBuffer), len);

    qint64 bytesSent = udpSocket->writeDatagram(datagram, remoteAddress, remotePort);
    if (bytesSent == -1) {
        emit showMessage("Error", "UDP send failed: " + udpSocket->errorString(), "red", 3000);
    }
}

void UdpWorker::cleanup() {
    if (connectionTimer) {
        connectionTimer->stop();
        delete connectionTimer;
        connectionTimer = nullptr;
    }
    if (signalCheckTimer) {
        signalCheckTimer->stop();
        delete signalCheckTimer;
        signalCheckTimer = nullptr;
    }
    if (udpSocket) {
        udpSocket->close();
        delete udpSocket;
        udpSocket = nullptr;
    }
}
