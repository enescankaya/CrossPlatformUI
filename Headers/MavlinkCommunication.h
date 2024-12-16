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
    void setMavlinkSignalValue(int value);

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
    QElapsedTimer m_lastHeartbeat;
    static constexpr int MAX_HEARTBEAT_INTERVAL = 3000; // 3 seconds maximum interval
    void updateMavlinkSignalStrength();
    std::atomic<bool> m_isArmed=true;

    GlobalParams::Mode convertMavlinkMode(uint32_t custom_mode);

    static const uint32_t MANUAL_MODE = 0;
    static const uint32_t CIRCLE_MODE = 1;
    static const uint32_t AUTO_MODE = 10;
    static const uint32_t GUIDED_MODE = 15;
    static const uint32_t TAXI_MODE = static_cast<uint32_t>(-1);
    static const uint32_t RTL_MODE = 11;

    mavlink_heartbeat_t heartbeat;

signals:

};
#endif // MAVLINK_COMMUNICATION_H
