#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

#include <QObject>
#include <QQuickWidget>
#include <QQuickItem>
#include <TcpManager.h>
#include <Headers/GlobalParams.h>
#include <MavlinkCommunication.h>
#include <UdpManager.h>
class MainInterface;
class TcpManager;
class ConnectionManager : public QObject {
    Q_OBJECT
public:
    explicit ConnectionManager(QObject* parent = nullptr);

    void setupConnections(MainInterface* mainInterface);
    void setUpConnections_For_Connections(MainInterface *mainInterface);
private:
    TcpManager *tcpManager;
    UdpManager *udpManager;
    MavlinkCommunication *mavlink_Class;
    GlobalParams::ConnectionType CurrentConnection=GlobalParams::ConnectionType::None;
};

#endif // CONNECTIONMANAGER_H
