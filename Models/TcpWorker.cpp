#include "Headers/TcpWorker.h"
#include "Headers/GlobalParams.h"

TcpWorker::TcpWorker(QObject *parent)
    : QObject(parent) {
}

TcpWorker::~TcpWorker() {
    cleanup();
}

void TcpWorker::initialize() {
    if (!tcpSocket) {
        tcpSocket = new QTcpSocket(this);
        connect(tcpSocket, &QTcpSocket::errorOccurred,
                this, &TcpWorker::handleSocketError);
        connect(tcpSocket, &QTcpSocket::stateChanged,
                this, &TcpWorker::handleStateChange);
    }

    if (!connectionTimer) {
        connectionTimer = new QTimer(this);
        connectionTimer->setSingleShot(true);
        connect(connectionTimer, &QTimer::timeout,
                this, &TcpWorker::handleConnectionTimeout);
    }
}
void TcpWorker::startConnectionTimer() {
    if (connectionTimer) {
        connectionTimer->start(CONNECTION_TIMEOUT);
    }
}

void TcpWorker::stopConnectionTimer() {
    if (connectionTimer) {
        connectionTimer->stop();
    }
}


void TcpWorker::handleConnect(const QString &ip, int port) {
    QMutexLocker locker(&mutex);
    auto &globals = GlobalParams::getInstance();

    if (tcpSocket->state() == QAbstractSocket::ConnectedState) {
        if (globals.TCP_Current_ip == ip && globals.TCP_Current_port == port) {
            emit showMessage("Connection Status", "Already connected to this address", "blue", 3000);
            return;
        }
        handleDisconnect();
    }

    startConnectionTimer();
    emit showMessage("Connection Status", "Attempting connection...", "blue", 3000);

    tcpSocket->connectToHost(ip, port);

    if (tcpSocket->waitForConnected(CONNECTION_TIMEOUT)) {
        stopConnectionTimer();
        globals.TCP_Current_ip = ip;
        globals.TCP_Current_port = port;
        globals.TCP_CONNECTION_STATE = true;
        emit connectionStateChanged(true);
    } else {
        stopConnectionTimer();
        emit showMessage("Error", "Connection failed", "red", 3000);
    }
}

void TcpWorker::handleDisconnect() {
    QMutexLocker locker(&mutex);
    auto &globals = GlobalParams::getInstance();

    startConnectionTimer();
    emit showMessage("Connection Status", "Disconnecting...", "blue", 3000);

    tcpSocket->disconnectFromHost();

    if (tcpSocket->state() == QAbstractSocket::UnconnectedState ||
        tcpSocket->waitForDisconnected(CONNECTION_TIMEOUT)) {
        globals.TCP_CONNECTION_STATE = false;
        emit connectionStateChanged(false);
    } else {
        emit showMessage("Error", "Failed to disconnect", "red", 3000);
    }

    stopConnectionTimer();
}

QString TcpWorker::getErrorMessage(QAbstractSocket::SocketError socketError) {
    switch (socketError) {
    case QAbstractSocket::ConnectionRefusedError:
        return "Connection refused by host";
    case QAbstractSocket::RemoteHostClosedError:
        return "Connection closed by remote host";
    case QAbstractSocket::HostNotFoundError:
        return "Host address not found";
    case QAbstractSocket::SocketTimeoutError:
        return "Connection attempt timed out";
    case QAbstractSocket::NetworkError:
        return "Network error occurred";
    default:
        return tcpSocket->errorString();
    }
}

void TcpWorker::handleSocketError(QAbstractSocket::SocketError socketError) {
    auto &globals = GlobalParams::getInstance();
    emit showMessage("Connection Error", getErrorMessage(socketError), "red", 5000);
    globals.TCP_CONNECTION_STATE = false;
    emit connectionStateChanged(false);

    tcpSocket->abort();
}

void TcpWorker::handleStateChange(QAbstractSocket::SocketState socketState) {
    auto &globals = GlobalParams::getInstance();

    switch (socketState) {
    case QAbstractSocket::UnconnectedState:
        globals.TCP_CONNECTION_STATE = false;
        emit connectionStateChanged(false);
        break;
    case QAbstractSocket::ConnectedState:
        globals.TCP_CONNECTION_STATE = true;
        emit connectionStateChanged(true);
        break;
    default:
        break;
    }
}

void TcpWorker::handleConnectionTimeout() {
    auto &globals = GlobalParams::getInstance();

    if (tcpSocket->state() != QAbstractSocket::ConnectedState) {
        tcpSocket->abort();
        globals.TCP_CONNECTION_STATE = false;
        emit connectionStateChanged(false);
        emit showMessage("Error", "Connection attempt timed out", "red", 3000);
    }
}

void TcpWorker::cleanup() {
    stopConnectionTimer();
    if (tcpSocket && tcpSocket->state() != QAbstractSocket::UnconnectedState) {
        handleDisconnect();
    }
}
