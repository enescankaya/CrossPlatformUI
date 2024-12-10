#ifndef GLOBALPARAMS_H
#define GLOBALPARAMS_H

#include <QObject>
#include <QMutex>
#include <QDebug>
#include <QString>
#include <QTcpSocket>
#include "MapScreen.h"

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
    bool Mavlink_Available=false;
    bool allPanelsAvailable=true;
    bool isArmed=false;

    enum Mode {
        Manual = 0,
        Circle = 1,
        Auto = 10,
        Guided = 15,
        Taxi = -1,
        RTL = 11
    }Modes;
    Mode indexToMode(int value) {
        switch (value) {
        case 0: return GlobalParams::Manual;
        case 1: return GlobalParams::Circle;
        case 2: return GlobalParams::Auto;
        case 3: return GlobalParams::Guided;
        case 4: return GlobalParams::Taxi;
        case 5: return GlobalParams::RTL;
        default:
            return GlobalParams::Manual; //  varsayılan
        }
    }
    float altitude;
    double latitude;
    double longitude;
    MapScreen* mapScreen;

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
