#include "Headers/MavlinkManager.h"

MavlinkManager::MavlinkManager(QObject *parent)
    : QObject(parent) {
    initializeWorker();
}

MavlinkManager::~MavlinkManager() {
    cleanupWorker();
}

void MavlinkManager::initializeWorker() {
    workerThread = new QThread(this);
    worker = new Mavlink_Communication();
    worker->moveToThread(workerThread);

    // Çalışan başlatma sinyali bağlantısı
    connect(workerThread, &QThread::started,
            worker, &Mavlink_Communication::initialize);

    // Durum sinyalleri bağlantıları

    connect(worker, &Mavlink_Communication::messageBox_Show,
            this, &MavlinkManager::messageBox_Show);


    // Araç durum sinyalleri bağlantıları
    connect(worker, &Mavlink_Communication::setAirSpeed,
            this, &MavlinkManager::setAirSpeed);
    connect(worker, &Mavlink_Communication::setRoll,
            this, &MavlinkManager::setRoll);
    connect(worker, &Mavlink_Communication::setPitch,
            this, &MavlinkManager::setPitch);
    connect(worker, &Mavlink_Communication::setYaw,
            this, &MavlinkManager::setYaw);
    connect(worker, &Mavlink_Communication::setAltitude,
            this, &MavlinkManager::setAltitude);
    connect(worker, &Mavlink_Communication::setPressure,
            this, &MavlinkManager::setPressure);
    connect(worker, &Mavlink_Communication::setVerticleSpeed,
            this, &MavlinkManager::setVerticleSpeed);
    connect(worker, &Mavlink_Communication::setCourse,
            this, &MavlinkManager::setCourse);
    connect(worker, &Mavlink_Communication::setNavigation,
            this, &MavlinkManager::setNavigation);
    connect(worker, &Mavlink_Communication::setMap,
            this, &MavlinkManager::setMap);


    // Komut istek sinyalleri bağlantıları
    connect(this, &MavlinkManager::goCoordinateRequested,
            worker, &Mavlink_Communication::Go_Coordinate);
    connect(this, &MavlinkManager::removeCoordinateRequested,
            worker, &Mavlink_Communication::Remove_Coordinate);
    connect(this, &MavlinkManager::ArmStateChangedSignal,
            worker, &Mavlink_Communication::ArmStateChanged);
    connect(this, &MavlinkManager::modeChangeRequested,
            worker, &Mavlink_Communication::change_Mode);

    connect(this, &MavlinkManager::sendTCPCommand,
            worker, &Mavlink_Communication::sendTCPCommand);

    workerThread->start();
}

void MavlinkManager::cleanupWorker() {
    if (workerThread) {
        worker->shutdown();
        workerThread->quit();
        workerThread->wait();
        delete worker;
        worker = nullptr;
    }
}

void MavlinkManager::handleIncomingData() {
    if (worker) {
        worker->processIncomingData();
    }
}

// Araç kontrol istek fonksiyonları
void MavlinkManager::requestGoCoordinate(double lat, double lng) {
    if (isProcessing.loadAcquire()) {
        emit messageBox_Show("Komut Durumu", "Komut işleniyor");
        return;
    }
    isProcessing.storeRelease(true);
    try {
        emit goCoordinateRequested(lat, lng);
    }
    catch (const std::exception &ex) {
        emit messageBox_Show("Sistem Hatası",
                             QString("Komut hatası: %1").arg(ex.what()));
    }
    isProcessing.storeRelease(false);
}

void MavlinkManager::requestRemoveCoordinate() {
    if (isProcessing.loadAcquire()) {
        emit messageBox_Show("Komut Durumu", "Komut işleniyor");
        return;
    }
    isProcessing.storeRelease(true);
    try {
        emit removeCoordinateRequested();
    }
    catch (const std::exception &ex) {
        emit messageBox_Show("Sistem Hatası",
                             QString("Komut hatası: %1").arg(ex.what()));
    }
    isProcessing.storeRelease(false);
}

void MavlinkManager::ArmStateChanged(bool isArmed) {
    if (isProcessing.loadAcquire()) {
        emit messageBox_Show("Komut Durumu", "Komut işleniyor");
        return;
    }
    isProcessing.storeRelease(true);
    try {
        emit ArmStateChangedSignal(isArmed);
    }
    catch (const std::exception &ex) {
        emit messageBox_Show("Sistem Hatası",
                             QString("Komut hatası: %1").arg(ex.what()));
    }
    isProcessing.storeRelease(false);
}


void MavlinkManager::requestModeChange(int mode) {
    if (isProcessing.loadAcquire()) {
        emit messageBox_Show("Komut Durumu", "Komut işleniyor");
        return;
    }
    isProcessing.storeRelease(true);
    try {
        emit modeChangeRequested(mode);
    }
    catch (const std::exception &ex) {
        emit messageBox_Show("Sistem Hatası",
                             QString("Komut hatası: %1").arg(ex.what()));
    }
    isProcessing.storeRelease(false);
}
