#ifndef TCPWORKER_H
#define TCPWORKER_H
#include <QObject>
#include <QTcpSocket>
#include <QTimer>
#include <QMutex>
class TcpWorker : public QObject {
    Q_OBJECT
public:
    explicit TcpWorker(QObject *parent = nullptr);
    ~TcpWorker();

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
};
#endif // TCPWORKER_H
