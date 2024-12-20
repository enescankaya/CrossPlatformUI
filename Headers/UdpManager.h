#ifndef UDPMANAGER_H
#define UDPMANAGER_H

#include "Library/Mavlink2/mavlink_types.h"
#include <QObject>
#include <QThread>
#include <QAtomicInteger>

class UdpWorker;

class UdpManager : public QObject {
    Q_OBJECT

public:
    explicit UdpManager(QObject *parent = nullptr);
    ~UdpManager();
    void requestConnection(bool connect, const QString &ip = QString(), int port = 0);

signals:
    void connectionStateChanged(bool connected);
    void showMessage(const QString& title, const QString& message,
                     const QString& color, int duration);
    void connectRequested(const QString &ip, int port);
    void disconnectRequested();
    void processMAVLinkMessage(const mavlink_message_t& msg);
    void sendMavlinkMessage(const mavlink_message_t& msg);

private:
    void initializeWorker();
    void cleanupWorker();

    QThread* workerThread{nullptr};
    UdpWorker* worker{nullptr};
    QAtomicInteger<bool> isProcessing{false};
};

#endif // UDPMANAGER_H
