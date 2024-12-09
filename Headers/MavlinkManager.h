#ifndef MAVLINKMANAGER_H
#define MAVLINKMANAGER_H

#include <QObject>
#include <QThread>
#include <QAtomicInteger>
#include "MavlinkWorker.h"

class MavlinkManager : public QObject {
    Q_OBJECT

public:
    explicit MavlinkManager(QObject *parent = nullptr);
    ~MavlinkManager();

    // Araç kontrol fonksiyonları
    void requestGoCoordinate(double lat, double lng);
    void requestRemoveCoordinate();
    void requestArm();
    void requestDisarm();
    void requestModeChange(int mode);

signals:
    // Arayüz güncelleme sinyalleri
    void updateLabelBackground(bool state, uint32_t time);
    void updateConnectionStatus(bool connected);
    void messageBox_Show(QString title, QString message);
    void connecting_tcp();

    // Araç durum sinyalleri
    void setAirSpeed(double value);
    void setRoll(double value);
    void setPitch(double value);
    void setYaw(double value);
    void setAltitude(double value);
    void setPressure(double value);
    void setVerticleSpeed(double value);
    void setCourse(double value);
    void setNavigation(double bearing, double deviation, double distance);
    void setMap(QVariant lat, QVariant lon, QVariant heading);
    void addMessageToLabel(QString message);
    void sendTCPCommand(uint8_t buf[],uint16_t len);

    // İç haberleşme sinyalleri
    void goCoordinateRequested(double lat, double lng);
    void removeCoordinateRequested();
    void ArmStateChangedSignal(bool isArmed);
    void modeChangeRequested(int mode);

public slots:
    void handleIncomingData();
    void ArmStateChanged(bool isArmed);

private:
    void initializeWorker();
    void cleanupWorker();

    QThread* workerThread{nullptr};
    Mavlink_Communication* worker{nullptr};
    QAtomicInteger<bool> isProcessing{false};
};

#endif // MAVLINKMANAGER_H
