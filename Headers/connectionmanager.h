#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

#include <QObject>
#include <QQuickWidget>
#include <QQuickItem>
#include <TcpManager.h>
#include <Headers/GlobalParams.h>
class MainInterface; // MainInterface sınıfını önceden bildirelim
class TcpManager;
class ConnectionManager : public QObject {
    Q_OBJECT
public:
    explicit ConnectionManager(QObject* parent = nullptr);

    void setupConnections(MainInterface* mainInterface);

private:
    TcpManager *tcpManager;


};

#endif // CONNECTIONMANAGER_H
