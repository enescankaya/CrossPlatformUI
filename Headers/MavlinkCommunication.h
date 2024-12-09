#ifndef MAVLINKCOMMUNICATION_H
#define MAVLINKCOMMUNICATION_H

#include "qobject.h"
#include <mavlink.h>
#include <QtConcurrent>
class MavlinkCommunication: public QObject
{
    Q_OBJECT

public:
    explicit MavlinkCommunication(QObject *parent = nullptr);

public slots:
    void processMAVLinkMessage(const mavlink_message_t& msg);
private:
    ~MavlinkCommunication();

signals:

};
#endif // MAVLINK_COMMUNICATION_H
