#ifndef TCPWORKER_H
#define TCPWORKER_H
#include "Library/Mavlink2/mavlink_types.h"
#include <mavlink.h>
#include <Library/Mavlink2/mavlink_helpers.h>
#include <Library/Mavlink2/mavlink_conversions.h>
#include <QObject>
#include <QTcpSocket>
#include <QTimer>
#include <QMutex>
class TcpWorker : public QObject {
    Q_OBJECT
public:
    explicit TcpWorker(QObject *parent = nullptr);
    ~TcpWorker();
    void ReadData();

public slots:
    void initialize();
    void handleConnect(const QString &ip, int port);
    void handleDisconnect();

signals:
    void connectionStateChanged(bool connected);
    void showMessage(const QString& title, const QString& message,
                     const QString& color, int duration);

private slots:
    void handleSocketError(QAbstractSocket::SocketError socketError);
    void handleStateChange(QAbstractSocket::SocketState socketState);
    void handleConnectionTimeout();


private:
    void cleanup();
    QString getErrorMessage(QAbstractSocket::SocketError socketError);
    void startConnectionTimer();
    void stopConnectionTimer();

    QTcpSocket* tcpSocket{nullptr};
    QTimer* connectionTimer{nullptr};  // Şimdi pointer olarak tutuyoruz
    static constexpr int CONNECTION_TIMEOUT = 3000;
    QMutex mutex;
    uint8_t buffer[MAVLINK_MAX_PACKET_LEN];
    mavlink_message_t msg;
    mavlink_status_t status = {};
};
#endif // TCPWORKER_H
