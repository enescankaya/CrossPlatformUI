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
    int throttle_Min=1000;
    int throttle_Max=2000;
    int32_t rpm_value=0;
    enum Mode {
        Manual = 0,
        Circle = 1,
        Auto = 10,
        Guided = 15,
        Taxi = -1,
        RTL = 11
    }Modes;
    Mode currentMode=Mode::Manual;
    Mode indexToMode(int value) {
        switch (value) {
        case 0: return Mode::Manual;
        case 1: return Mode::Circle;
        case 2: return Mode::Auto;
        case 3: return Mode::Guided;
        case 4: return Mode::Taxi;
        case 5: return Mode::RTL;
        default:
            return Mode::Manual; //  varsayılan
        }
    }
    int getModeIndex(int value) {
        switch (value) {
        case 0: return value;
        case 1: return value;
        case 10: return 2;
        case 15: return 3;
        case -1: return 4;
        case 11: return 5;
        default:
            return 0; //  manuel
        }
    }
    double altitude=100;//default
    double latitude=39.9334;//Ankara
    double longitude=32.8597;
    float ThrottleValue=30;
    double vertical_speed=0;
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
