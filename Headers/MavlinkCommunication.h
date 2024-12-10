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
private:
    QMutex mutex;
    QAtomicInteger<bool> isSending=false;  // Verilerin gönderilip gönderilmeyeceğini kontrol etmek için



signals:

};
#endif // MAVLINK_COMMUNICATION_H
