#ifndef GLOBALPARAMS_H
#define GLOBALPARAMS_H

#include <QObject>
#include <QMutex>
#include <QDebug>
#include <QString>
#include <QTcpSocket>

class GlobalParams : public QObject {
    Q_OBJECT

public:
    static GlobalParams& getInstance() {
        static GlobalParams instance;
        return instance;
    }

    bool TCP_CONNECTION_STATE = false;
    int TCP_Current_port = -1;
    QString TCP_Current_ip;

    bool allPanelsAvailable=true;
signals:
    void connectionStatusChanged(bool connected);
    void showMessage(const QString& title, const QString& message,
                     const QString& color, int duration);

private:
    explicit GlobalParams(QObject *parent = nullptr);
    ~GlobalParams();


    Q_DISABLE_COPY(GlobalParams)
};

#endif // GLOBALPARAMS_H
