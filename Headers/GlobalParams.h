#ifndef GLOBALPARAMS_H
#define GLOBALPARAMS_H

#include <QObject>
#include <QMutex>
#include <QReadWriteLock>
#include <QString>
#include <QTcpSocket>
#include <atomic>
#include <memory>
#include "MapScreen.h"

class GlobalParams : public QObject {
    Q_OBJECT

public:
    static GlobalParams& getInstance() {
        static GlobalParams instance;
        return instance;
    }

    // Thread-safe getters/setters
    bool getTcpConnectionState() const {
        QReadLocker lock(&m_lock);
        return m_TCP_CONNECTION_STATE;
    }

    void setTcpConnectionState(bool state) {
        QWriteLocker lock(&m_lock);
        m_TCP_CONNECTION_STATE = state;
        emit connectionStatusChanged(state);
    }

    int getTcpPort() const {
        QReadLocker lock(&m_lock);
        return m_TCP_Current_port;
    }

    void setTcpPort(int port) {
        QWriteLocker lock(&m_lock);
        m_TCP_Current_port = port;
    }

    QString getTcpIp() const {
        QReadLocker lock(&m_lock);
        return m_TCP_Current_ip;
    }

    void setTcpIp(const QString& ip) {
        QWriteLocker lock(&m_lock);
        m_TCP_Current_ip = ip;
    }

    bool getArmState() const {
        QReadLocker lock(&m_lock);
        return m_isArmed;
    }

    void setArmState(bool state) {
        QWriteLocker lock(&m_lock);
        m_isArmed = state;
    }
    bool getisAllPanelsAvailable() const {
        QReadLocker lock(&m_lock);
        return m_allPanelsAvailable;
    }

    void setisAllPanelsAvailable(bool state) {
        QWriteLocker lock(&m_lock);
        m_allPanelsAvailable = state;
    }
    bool getRpm() const {
        QReadLocker lock(&m_lock);
        return m_rpm;
    }

    void setRpm(int32_t value) {
        QWriteLocker lock(&m_lock);
        m_rpm = value;
    }
    enum class Mode {
        Manual = 0,
        Circle = 1,
        Auto = 10,
        Guided = 15,
        Taxi = -1,
        RTL = 11
    };

    Mode getCurrentMode() const {
        QReadLocker lock(&m_lock);
        return m_currentMode;
    }

    void setCurrentMode(Mode mode) {
        QWriteLocker lock(&m_lock);
        m_currentMode = mode;
    }

    Mode indexToMode(int value) {
        switch (value) {
        case 0: return Mode::Manual;
        case 1: return Mode::Circle;
        case 2: return Mode::Auto;
        case 3: return Mode::Guided;
        case 4: return Mode::Taxi;
        case 5: return Mode::RTL;
        default:
            return Mode::Manual;
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
            return 0;
        }
    }

    // Thread-safe altitude handling
    double getAltitude() const {
        return m_altitude.load();
    }

    void setAltitude(double alt) {
        m_altitude.store(alt);
    }

    // Coordinate validation and handling
    void setCoordinates(double lat, double lon) {
        QWriteLocker lock(&m_lock);
        m_latitude = lat;
        m_longitude = lon;
    }

    double getLatitude() const {
        QReadLocker lock(&m_lock);
        return m_latitude;
    }

    double getLongitude() const {
        QReadLocker lock(&m_lock);
        return m_longitude;
    }

    // Throttle handling with bounds checking
    void setThrottleValue(float value) {
        QWriteLocker lock(&m_lock);
        m_ThrottleValue = value;
    }

    float getThrottleValue() const {
        QReadLocker lock(&m_lock);
        return m_ThrottleValue;
    }

    void setVerticalSpeed(double speed) {
        QWriteLocker lock(&m_lock);
        m_vertical_speed = speed;
    }

    double getVerticalSpeed() const {
        QReadLocker lock(&m_lock);
        return m_vertical_speed;
    }

    // Safe MapScreen handling
    void setMapScreen(MapScreen* screen) {
        QWriteLocker lock(&m_lock);
        m_mapScreen.reset(screen);
    }

    MapScreen* getMapScreen() const {
        QReadLocker lock(&m_lock);
        return m_mapScreen.get();
    }

signals:
    void connectionStatusChanged(bool connected);
    void showMessage(const QString& title, const QString& message,
                     const QString& color, int duration);

protected:
    explicit GlobalParams(QObject *parent = nullptr) : QObject(parent) {
        initializeDefaultValues();
    }

private:
    void initializeDefaultValues() {
        m_TCP_CONNECTION_STATE = false;
        m_TCP_Current_port = -1;
        m_isArmed = true;
        m_currentMode = Mode::Manual;
        m_altitude.store(100.0);
        m_latitude = 39.9334;
        m_longitude = 32.8597;
        m_ThrottleValue = 30.0f;
        m_vertical_speed = 0.0;
        m_allPanelsAvailable=true;
        m_rpm=0;
    }

    // Prevent copying
    GlobalParams(const GlobalParams&) = delete;
    GlobalParams& operator=(const GlobalParams&) = delete;
    ~GlobalParams() {}

    // Thread-safe member variables
    mutable QReadWriteLock m_lock;
    std::atomic<bool> m_TCP_CONNECTION_STATE;
    int m_TCP_Current_port;
    QString m_TCP_Current_ip;
    std::atomic<bool> m_isArmed;
    Mode m_currentMode;
    std::atomic<double> m_altitude;
    std::atomic<bool> m_allPanelsAvailable;
    double m_latitude;
    double m_longitude;
    float m_ThrottleValue;
    double m_vertical_speed;
    std::unique_ptr<MapScreen> m_mapScreen;
    int32_t m_rpm;
};

#endif // GLOBALPARAMS_H
