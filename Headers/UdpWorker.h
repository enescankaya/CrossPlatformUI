#ifndef UDPWORKER_H
#define UDPWORKER_H

#include "Library/Mavlink2/mavlink_types.h"
#include <mavlink.h>
#include <Library/Mavlink2/mavlink_helpers.h>
#include <Library/Mavlink2/mavlink_conversions.h>
#include <QObject>
#include <QUdpSocket>
#include <QTimer>
#include <QMutex>

class UdpWorker : public QObject {
    Q_OBJECT

public:
    explicit UdpWorker(QObject *parent = nullptr);
    ~UdpWorker();
    void ReadData();

public slots:
    void initialize();
    void handleConnect(const QString &ip, int port);
    void handleDisconnect();
    void sendMavlinkMessage(const mavlink_message_t& msg);

signals:
    void connectionStateChanged(bool connected);
    void showMessage(const QString& title, const QString& message,
                     const QString& color, int duration);
    void processMAVLinkMessage(const mavlink_message_t& msg);

private slots:
    void handleSocketError(QAbstractSocket::SocketError socketError);
    void handleStateChange(QAbstractSocket::SocketState socketState);
    void handleConnectionTimeout();

private:
    void cleanup();
    QString getErrorMessage(QAbstractSocket::SocketError socketError);
    void startConnectionTimer();
    void stopConnectionTimer();

    QUdpSocket* udpSocket{nullptr};
    QTimer* connectionTimer{nullptr};
    static constexpr int CONNECTION_TIMEOUT = 3000;
    QMutex mutex;
    uint8_t buffer[MAVLINK_MAX_PACKET_LEN];
    mavlink_message_t msg;
    mavlink_status_t status = {};
    QHostAddress remoteAddress;
    quint16 remotePort{0};
};

#endif // UDPWORKER_H
