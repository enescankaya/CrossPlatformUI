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
    emit showMessage("Connection Status", "Attempting connection...", "blue", 3000);

    remoteAddress = QHostAddress(ip);
    remotePort = port;

    if (udpSocket->bind(QHostAddress::Any, port)) {
        stopConnectionTimer();
        globals.setUdpIp(ip);
        globals.setUdpPort(port);
        globals.setUdpConnectionState(true);
        emit connectionStateChanged(true);
        GlobalParams::getInstance().setActiveConnectionType(GlobalParams::ConnectionType::UDP);
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
    emit showMessage("Connection Error", getErrorMessage(socketError), "red", 5000);
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

void UdpWorker::cleanup() {
    stopConnectionTimer();
    if (udpSocket && udpSocket->state() != QAbstractSocket::UnconnectedState) {
        handleDisconnect();
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
    if (!udpSocket || udpSocket->state() != QAbstractSocket::BoundState) {
        qDebug() << "UDP socket is not bound or invalid.";
        return;
    }

    if (remoteAddress.isNull() || remotePort == 0) {
        qDebug() << "Remote address or port is invalid.";
        return;
    }

    uint8_t buffer[MAVLINK_MAX_PACKET_LEN];
    uint16_t len = mavlink_msg_to_send_buffer(buffer, &msg);

    qDebug() << "Sending MAVLink message of length:" << len
             << "to" << remoteAddress.toString() << ":" << remotePort;

    qint64 bytesSent = udpSocket->writeDatagram(reinterpret_cast<const char*>(buffer), len, remoteAddress, remotePort);

    if (bytesSent != len) {
        qDebug() << "Failed to send MAVLink message. Sent bytes:" << bytesSent;
    } else {
        qDebug() << "MAVLink message sent successfully.";
    }
}


