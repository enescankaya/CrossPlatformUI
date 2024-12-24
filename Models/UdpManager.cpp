#include "UdpManager.h"
#include "UdpWorker.h"

UdpManager::UdpManager(QObject *parent) : QObject(parent) {
    initializeWorker();
}

UdpManager::~UdpManager() {
    cleanupWorker();
}

void UdpManager::initializeWorker() {
    workerThread = new QThread(this);
    worker = new UdpWorker();
    worker->moveToThread(workerThread);

    // Connect signals/slots
    connect(workerThread, &QThread::started, worker, &UdpWorker::initialize);
    connect(worker, &UdpWorker::connectionStateChanged,
            this, &UdpManager::connectionStateChanged, Qt::QueuedConnection);
    connect(worker, &UdpWorker::showMessage,
            this, &UdpManager::showMessage, Qt::QueuedConnection);
    connect(worker, &UdpWorker::processMAVLinkMessage,
            this, &UdpManager::processMAVLinkMessage, Qt::QueuedConnection);
    connect(this, &UdpManager::connectRequested,
            worker, &UdpWorker::handleConnect, Qt::QueuedConnection);
    connect(this, &UdpManager::disconnectRequested,
            worker, &UdpWorker::handleDisconnect, Qt::QueuedConnection);
    connect(this, &UdpManager::sendMavlinkMessage,
            worker, &UdpWorker::sendMavlinkMessage, Qt::QueuedConnection);

    workerThread->start();
}

void UdpManager::cleanupWorker() {
    if (workerThread && worker) {
        worker->cleanup();
        workerThread->quit();

        if (!workerThread->wait(3000)) {
            workerThread->terminate();
            workerThread->wait();
        }

        delete worker;
        delete workerThread;
        worker = nullptr;
        workerThread = nullptr;
    }
}

void UdpManager::requestConnection(bool connect, const QString &ip, int port) {
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
                         QString("Connection error: %1").arg(ex.what()),
                         "red", 5000);
    }

    isProcessing.storeRelease(false);
}
