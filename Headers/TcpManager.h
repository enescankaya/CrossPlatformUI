// TcpManager.h
#ifndef TCPMANAGER_H
#define TCPMANAGER_H

#include "Library/Mavlink2/mavlink_types.h"
#include <QObject>
#include <QTcpSocket>
#include <QThread>
#include <QAtomicInteger>
#include <QTimer>
#include <QMutex>

class TcpWorker;

class TcpManager : public QObject {
    Q_OBJECT
public:
    explicit TcpManager(QObject *parent = nullptr);
    ~TcpManager();

    // Public interface
    void requestConnection(bool connect, const QString &ip = QString(), int port = 0);

signals:
    // Signals for UI updates
    void connectionStateChanged(bool connected);
    void showMessage(const QString& title, const QString& message,
                     const QString& color, int duration);

    // Internal signals for worker communication
    void connectRequested(const QString &ip, int port);
    void disconnectRequested();
    void processMAVLinkMessage(const mavlink_message_t& msg);
    void sendMavlinkMessage(const mavlink_message_t& msg);

private:
    void initializeWorker();
    void cleanupWorker();

    QThread* workerThread{nullptr};
    TcpWorker* worker{nullptr};
    QAtomicInteger<bool> isProcessing{false};
};

#endif // TCPMANAGER_H
