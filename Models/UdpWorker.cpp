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
    int currentSignalStrength = globals.getMavlinkSignalStrength();

    signalCheckAttempts++;

    if (currentSignalStrength >= MIN_SIGNAL_STRENGTH) {
        // Signal strength is good, stop checking
        signalCheckTimer->stop();
        signalCheckAttempts = 0;
        emit showMessage("Connection Status", "Connection established successfully", "green", 3000);
    } else if (signalCheckAttempts >= MAX_SIGNAL_CHECK_ATTEMPTS) {
        // Signal strength remained low for 5 seconds
        signalCheckTimer->stop();
        signalCheckAttempts = 0;
        handleDisconnect();
        emit showMessage("Connection Error", "No MAVLink signal", "red", 3000);
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
    QMutexLocker locker(&mutex);
    auto &globals = GlobalParams::getInstance();

    if (udpSocket->state() == QAbstractSocket::BoundState) {
        if (globals.getUdpIp() == ip && globals.getUdpPort() == port) {
            emit showMessage("Connection Status", "Already connected to this address", "blue", 3000);
            return;
        }
        handleDisconnect();
    }

    startConnectionTimer();
    emit showMessage("Connection Status", "Attempting connection...", "blue", 5000);

    remoteAddress = QHostAddress(ip);
    remotePort = port;

    if (udpSocket->bind(QHostAddress::Any, port)) {
        stopConnectionTimer();
        globals.setUdpIp(ip);
        globals.setUdpPort(port);
        globals.setUdpConnectionState(true);
        emit connectionStateChanged(true);
        GlobalParams::getInstance().setActiveConnectionType(GlobalParams::ConnectionType::UDP);

        // Start monitoring signal strength
        signalCheckAttempts = 0;
        signalCheckTimer->start();
    } else {
        stopConnectionTimer();
        emit showMessage("Error", "Connection failed", "red", 3000);
    }
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

        // Sadece beklenen adresten gelen paketleri işle
            for (int i = 0; i < datagram.size(); i++) {
                if (mavlink_parse_char(MAVLINK_COMM_0, static_cast<uint8_t>(datagram[i]), &msg, &status)) {
                    emit processMAVLinkMessage(msg);                }
            }
    }
}

void UdpWorker::sendMavlinkMessage(const mavlink_message_t& msg) {
    if (!udpSocket) {
        emit showMessage("Error", "UDP socket is null!", "red", 3000);
        return;
    }

    if (udpSocket->state() != QAbstractSocket::BoundState) {
        emit showMessage("Error", QString("Socket not bound! Current state: %1").arg(udpSocket->state()), "red", 3000);
        return;
    }

    if (remoteAddress.isNull() || remotePort == 0) {
        emit showMessage("Error", QString("Invalid remote address or port! Address: %1, Port: %2")
                             .arg(remoteAddress.toString())
                             .arg(remotePort), "red", 3000);
        return;
    }

    uint8_t buffer[MAVLINK_MAX_PACKET_LEN];
    qint64 len = mavlink_msg_to_send_buffer(buffer, &msg);
    QByteArray datagram(reinterpret_cast<const char*>(buffer), len);
    udpSocket->writeDatagram(datagram, remoteAddress, remotePort);
}


void UdpWorker::cleanup() {
    stopConnectionTimer();
    if (signalCheckTimer) {
        signalCheckTimer->stop();
    }
    if (udpSocket && udpSocket->state() != QAbstractSocket::UnconnectedState) {
        handleDisconnect();
    }
}
