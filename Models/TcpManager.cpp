#include "Headers/TcpManager.h"
#include "Headers/TcpWorker.h"

TcpManager::TcpManager(QObject *parent)
    : QObject(parent) {
    initializeWorker();
}

TcpManager::~TcpManager() {
    cleanupWorker();
}

void TcpManager::initializeWorker() {
    workerThread = new QThread(this);
    worker = new TcpWorker();
    worker->moveToThread(workerThread);

    // Worker initialize sinyalini bağla
    connect(workerThread, &QThread::started,
            worker, &TcpWorker::initialize);

    // Diğer sinyalleri bağla
    connect(worker, &TcpWorker::connectionStateChanged,
            this, &TcpManager::connectionStateChanged);
    connect(worker, &TcpWorker::showMessage,
            this, &TcpManager::showMessage);

    connect(this, &TcpManager::connectRequested,
            worker, &TcpWorker::handleConnect);
    connect(this, &TcpManager::disconnectRequested,
            worker, &TcpWorker::handleDisconnect);

    // Worker'ı başlat
    workerThread->start();
}


void TcpManager::cleanupWorker() {
    if (workerThread) {
        workerThread->quit();
        workerThread->wait();
        delete worker;
        worker = nullptr;
    }
}

void TcpManager::requestConnection(bool connect, const QString &ip, int port) {
    if (isProcessing.loadAcquire()) {
        emit showMessage("Connection Status", "Connection process in progress", "yellow", 3000);
        return;
    }

    isProcessing.storeRelease(true);

    try {
        if (connect) {
            emit connectRequested(ip, port);
        } else {
            emit disconnectRequested();
        }
    }
    catch (const std::exception &ex) {
        emit showMessage("System Error",
                         QString("Connection error: %1").arg(ex.what()), "red", 5000);
    }

    isProcessing.storeRelease(false);
}
