#ifndef MAVLINKCOMMUNICATION_H
#define MAVLINKCOMMUNICATION_H

#include "qobject.h"
#include <mavlink.h>
#include <QtConcurrent>
#include <Headers/GlobalParams.h>
class MavlinkCommunication: public QObject
{
    Q_OBJECT

public:
    explicit MavlinkCommunication(QObject *parent = nullptr);
    ~MavlinkCommunication();
    void Arm();
    void disArm();


public slots:
    void processMAVLinkMessage(const mavlink_message_t& msg);
    void changeMode(GlobalParams::Mode currentMode);
    void SetAltitude(float altitude);
    void Go_Coordinate(double lat, double lng);
    void Remove_Coordinate();
    void SetThrottle(double throttlePercent);

signals:
    void sendMessage(const mavlink_message_t& msg);
    void setMap(QVariant,QVariant,QVariant);
    void updateHeading(QVariant heading);
    void updateHUD(QVariant newAirspeed, QVariant newAltitude, QVariant newPitch, QVariant newRoll);
    void updateInfoHud(uint16_t throttle,float airspeed,float groundspeed,int32_t rpm_value);
    void setFuelValue(QVariant value);
    void setBatteryLevel(QVariant level);
    void setSignalStrength(QVariant strength);
    void setArmState(QVariant state);
    void setEngineState(QVariant state);
    void setMode(QVariant mode);

private:
    QMutex mutex;
    QAtomicInteger<bool> isSending=false;  // Verilerin gönderilip gönderilmeyeceğini kontrol etmek için
    void handleGlobalPosition(const mavlink_message_t& msg);
    void handleAttitude(const mavlink_message_t& msg);
    void handleRPM(const mavlink_message_t& msg);
    void handleRSSI(const mavlink_message_t& msg);
    void handleInfos(const mavlink_message_t& msg);
    void handleFuelAndBatteryStatus(const mavlink_message_t& msg);
    void handleHeartbeat(const mavlink_message_t& msg);
signals:

};
#endif // MAVLINK_COMMUNICATION_H
