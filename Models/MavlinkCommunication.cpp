#include "MavlinkCommunication.h"
#include <QMessageBox>
#include <QMutexLocker>
#include <algorithm> // std::clamp için gerekli
MavlinkCommunication::MavlinkCommunication(QObject *parent):
    QObject(parent)
{
    m_threadPool = new QThreadPool(this);
    m_threadPool->setMaxThreadCount(QThread::idealThreadCount());
    // Timer kurulumu
    m_signalCheckTimer.setInterval(500);
    connect(&m_signalCheckTimer, &QTimer::timeout, this, [this]() {
        m_threadPool->start([this]() {
            updateMavlinkSignalStrength();
        });
    });
    m_signalCheckTimer.start();

    // Throttle timer kurulumu
    m_throttleTimer.setInterval(100);
    connect(&m_throttleTimer, &QTimer::timeout, this, [this]() {
        if (m_isThrottleActive) {
            sendThrottleCommand();
        }
    });
}
void MavlinkCommunication::updateMavlinkSignalStrength() {
    m_threadPool->start([=]() {
        static bool sendHeartBeat = false;
        sendHeartBeat = !sendHeartBeat;
        if (sendHeartBeat) {
            sendHeartbeat();  // Heartbeat gönder
        }
    qint64 timeSinceLastHeartbeat = m_lastHeartbeat.elapsed();

    // Calculate signal strength (0-100)
    int signalStrength;
    if (timeSinceLastHeartbeat <= 1000) { // Good signal: less than 1 second
        signalStrength = 100;
    } else if (timeSinceLastHeartbeat >= MAX_HEARTBEAT_INTERVAL) { // No signal
        signalStrength = 0;
    }
    else { // Linear interpolation between 1000ms and MAX_HEARTBEAT_INTERVAL
        signalStrength = static_cast<int>(100 * (1.0 - (timeSinceLastHeartbeat - 1000.0) / (MAX_HEARTBEAT_INTERVAL - 1000.0)));
    }
    GlobalParams::getInstance().setMavlinkSignalStrength(signalStrength);
    emit setMavlinkSignalValue(signalStrength);
    // Get system armed state from base_mode field
    bool isArmed = (heartbeat.base_mode & MAV_MODE_FLAG_SAFETY_ARMED) != 0;

    // Sadece heart beat'ten gelen arm durumu true ise güncelle
    if (GlobalParams::getInstance().getArmState() != isArmed) {
        GlobalParams::getInstance().setArmState(isArmed);
        emit setArmState(isArmed);
    }

    // Convert custom mode to our internal mode enum
    auto currentMode = convertMavlinkMode(heartbeat.custom_mode);

    // Update mode if changed
    if (GlobalParams::getInstance().getCurrentMode() != currentMode) {
        GlobalParams::getInstance().setCurrentMode(currentMode);
        emit setMode(GlobalParams::getInstance().getModeIndex(static_cast<int>(currentMode)));
    }

    // STATUSTEXT zamanını kontrol edin
    if (m_lastStatusMessage.isValid() && m_lastStatusMessage.elapsed() > 5000) {
        emit showWarningMessage(" ");
        m_lastStatusMessage.invalidate(); // Tekrar tetiklenmeyi önlemek için geçersiz yap
    }
    emit updateClock(isArmed);
    });

}
void MavlinkCommunication::sendHeartbeat() {
    if (GlobalParams::getInstance().getConnectionState() == true) {
        mavlink_message_t msg;
        mavlink_heartbeat_t hb;

        hb.type = MAV_TYPE_GCS; // Yer kontrol istasyonu
        hb.autopilot = MAV_AUTOPILOT_INVALID; // GCS için
        hb.base_mode = MAV_MODE_FLAG_CUSTOM_MODE_ENABLED; // Özel mod aktif
        hb.custom_mode = 0; // Özel mod için varsayılan değer
        hb.system_status = MAV_STATE_ACTIVE; // Sistem aktif
        hb.mavlink_version = 3; // MAVLink 2.0

        mavlink_msg_heartbeat_encode(255, 190, &msg, &hb);
        emit sendMessage(msg);  // Mesaj gönderimi
    }
}


void MavlinkCommunication::processMAVLinkMessage(const mavlink_message_t& msg) {
    m_threadPool->start([&msg, this]() {
        switch (msg.msgid) {
        case MAVLINK_MSG_ID_GLOBAL_POSITION_INT:
            handleGlobalPosition(msg);
            break;
        case MAVLINK_MSG_ID_ATTITUDE:
            handleAttitude(msg);
            break;
        case MAVLINK_MSG_ID_VFR_HUD:
            handleInfos(msg);
            break;
        case MAVLINK_MSG_ID_ESC_STATUS:
            handleRPM(msg);
            break;
        case MAVLINK_MSG_ID_RC_CHANNELS:
            handleRSSI(msg);
            break;
        case MAVLINK_MSG_ID_SYS_STATUS:
            handleFuelAndBatteryStatus(msg);
            break;
        case MAVLINK_MSG_ID_HEARTBEAT:
            handleHeartbeat(msg);
            break;
        case MAVLINK_MSG_ID_STATUSTEXT:
            handleStatusText(msg);
            break;
        default:
            break;
        }
    });
}

void MavlinkCommunication::handleStatusText(const mavlink_message_t& msg) {
    static QString previousMessage = "firstMessage";

    mavlink_statustext_t statustext;
    mavlink_msg_statustext_decode(&msg, &statustext);
    QString currentMessage = QString::fromLatin1(statustext.text);

    // Kritik mesaj kontrolü
    if ((statustext.severity <= MAV_SEVERITY_WARNING || currentMessage.contains("SIM Hit ground at")) &&
        !currentMessage.contains(previousMessage)) {
        emit showWarningMessage(currentMessage);
        previousMessage = currentMessage;
        // Yeni mesaj alındığı için zamanlayıcıyı sıfırla
        m_lastStatusMessage.restart();
    }
}

void MavlinkCommunication::handleInfos(const mavlink_message_t& msg) {
    mavlink_vfr_hud_t hud;
    mavlink_msg_vfr_hud_decode(&msg, &hud);
    emit updateInfoHud(hud.throttle,hud.airspeed,hud.groundspeed,GlobalParams::getInstance().getRpm());
}
void MavlinkCommunication::handleRPM(const mavlink_message_t& msg) {
    mavlink_esc_status_t esc_status;
    mavlink_msg_esc_status_decode(&msg, &esc_status);
    GlobalParams::getInstance().setRpm(esc_status.rpm[0]);   // Assuming first ESC is main
}
void MavlinkCommunication::handleRSSI(const mavlink_message_t& msg) {
    mavlink_rc_channels_t rc_channels;
    mavlink_msg_rc_channels_decode(&msg, &rc_channels);
    emit setSignalStrength(static_cast<int>((rc_channels.rssi / 255.0) * 100.0));
}

void MavlinkCommunication::handleFuelAndBatteryStatus(const mavlink_message_t& msg) {
    mavlink_sys_status_t sys_status;
    mavlink_msg_sys_status_decode(&msg, &sys_status);
    //emit setFuelValue(sys_status.load / 10.0f);
    emit setBatteryLevel(sys_status.battery_remaining);
}
void MavlinkCommunication::handleHeartbeat(const mavlink_message_t& msg) {
    mavlink_msg_heartbeat_decode(&msg, &heartbeat);
    m_lastHeartbeat.restart();
}
GlobalParams::Mode MavlinkCommunication::convertMavlinkMode(uint32_t custom_mode) {
    // Map MAVLink custom modes to our internal Mode enum
    switch (custom_mode) {
    case MANUAL_MODE:
        return GlobalParams::Mode::Manual;
    case CIRCLE_MODE:
        return GlobalParams::Mode::Circle;
    case AUTO_MODE:
        return GlobalParams::Mode::Auto;
    case GUIDED_MODE:
        return GlobalParams::Mode::Guided;
    case TAXI_MODE:
        return GlobalParams::Mode::Taxi;
    case RTL_MODE:
        return GlobalParams::Mode::RTL;
    default:
        return GlobalParams::Mode::Manual;
    }
}
void MavlinkCommunication::handleGlobalPosition(const mavlink_message_t& msg) {
    mavlink_global_position_int_t global_position;
    mavlink_msg_global_position_int_decode(&msg, &global_position);
    GlobalParams::getInstance().setVerticalSpeed(global_position.vz / 100.0f);
    GlobalParams::getInstance().setAltitude(global_position.relative_alt / 1000.0);
    emit setMap(global_position.lat / 1e7,global_position.lon / 1e7,global_position.hdg/100.0);
    emit updateHeading(global_position.hdg / 100.0);
}
void MavlinkCommunication::handleAttitude(const mavlink_message_t& msg) {
    mavlink_attitude_t attitude;
    mavlink_msg_attitude_decode(&msg, &attitude);
    emit updateHUD(GlobalParams::getInstance().getVerticalSpeed(),GlobalParams::getInstance().getAltitude(),attitude.pitch * 180 / M_PI,attitude.roll * 180 / M_PI);
    int rollValue = int(attitude.roll * 180 / M_PI);
    rollValue = std::clamp(rollValue, -50, 50);
    emit setFuelRoll(rollValue);
}
void MavlinkCommunication::disArm(){
    m_threadPool->start([=]() {
    if (GlobalParams::getInstance().getConnectionState() == true) {
        mavlink_message_t msg;
        mavlink_msg_command_long_pack(255, 190, &msg, 1, 1, MAV_CMD_COMPONENT_ARM_DISARM, 1, 0, 21196, 0, 0, 0, 0, 0);
        emit sendMessage(msg);
    } else {
        emit GlobalParams::getInstance().showMessage("Not Connected", "Please connect to the server first.","",2000);
    }
    });
}
void MavlinkCommunication::Arm(){
    m_threadPool->start([=]() {
    if (GlobalParams::getInstance().getConnectionState() == true) {
        mavlink_message_t msg;
        mavlink_msg_command_long_pack(255, 190, &msg, 1, 1, MAV_CMD_COMPONENT_ARM_DISARM, 1, 1, 2989, 0, 0, 0, 0, 0);
        emit sendMessage(msg);
    }
    });
}
void MavlinkCommunication::changeMode(GlobalParams::Mode currentMode) {
    m_threadPool->start([=]() {
    if (GlobalParams::getInstance().getConnectionState() == true) {
    mavlink_message_t msg;
    mavlink_msg_command_long_pack(255, 190, &msg,
                                  1, 1, // Target system and component
                                  MAV_CMD_DO_SET_MODE, 0, // Command and confirmation
                                  217, // Custom mode (arbitrary value)
                                  static_cast<int>(currentMode), // Mode as int
                                  0, 0, 0, 0, 0); // Other parameters (unused)
    emit sendMessage(msg);
    GlobalParams::getInstance().setCurrentMode(currentMode);
    }
    });
}
void MavlinkCommunication::SetAltitude(float altitude) {
    GlobalParams::getInstance().setAltitude(altitude);
    m_threadPool->start([=]() {
    if (GlobalParams::getInstance().getConnectionState() == true) {
        mavlink_message_t msg;

        // Position mask - only enable altitude change
        uint16_t mask = POSITION_TARGET_TYPEMASK_X_IGNORE |
                        POSITION_TARGET_TYPEMASK_Y_IGNORE |
                        POSITION_TARGET_TYPEMASK_VX_IGNORE |
                        POSITION_TARGET_TYPEMASK_VY_IGNORE |
                        POSITION_TARGET_TYPEMASK_VZ_IGNORE |
                        POSITION_TARGET_TYPEMASK_AX_IGNORE |
                        POSITION_TARGET_TYPEMASK_AY_IGNORE |
                        POSITION_TARGET_TYPEMASK_AZ_IGNORE |
                        POSITION_TARGET_TYPEMASK_YAW_IGNORE |
                        POSITION_TARGET_TYPEMASK_YAW_RATE_IGNORE;

        mavlink_msg_set_position_target_global_int_pack(
            255,           // Sender system type
            190,  // Sender autopilot type
            &msg,
            0,                      // Time boot ms
            1,                      // Target system
            1,                      // Target component
            MAV_FRAME_GLOBAL_RELATIVE_ALT_INT,  // Frame
            mask,                   // Type mask
            0,                      // Latitude (not used)
            0,                      // Longitude (not used)
            altitude,               // Altitude
            0,                      // X velocity
            0,                      // Y velocity
            0,                      // Z velocity
            0,                      // X acceleration
            0,                      // Y acceleration
            0,                      // Z acceleration
            0,                      // Yaw
            0                       // Yaw rate
            );

        emit sendMessage(msg);
    }
    });
}
void MavlinkCommunication::Go_Coordinate(double lat, double lng) {
    m_threadPool->start([=]() {
    if (GlobalParams::getInstance().getConnectionState() == true) {
    // Convert latitude and longitude to MAVLink's expected format
    int32_t lat_mav = static_cast<int32_t>(lat * 1e7);
    int32_t lng_mav = static_cast<int32_t>(lng * 1e7);

        mavlink_message_t msg;

        mavlink_msg_mission_item_int_pack(
            255, 190, &msg,
            1,  // System ID
            1,  // Component ID
            0,  // Sequence (you may want to adjust this if you're sending multiple waypoints)
            MAV_FRAME_GLOBAL_RELATIVE_ALT,
            MAV_CMD_NAV_WAYPOINT,
            2,  // Current (set to 2 for guided mode)
            1,  // Auto-continue
            0, 0, 0, 0,  // Params 1 to 4 (unused)
            lat_mav,
            lng_mav,
            GlobalParams::getInstance().getAltitude(),  // Altitude in meters
            MAV_MISSION_TYPE_MISSION
            );
        emit sendMessage(msg);
    }
    });
}
void MavlinkCommunication::Remove_Coordinate() {
    m_threadPool->start([=]() {
    if (GlobalParams::getInstance().getConnectionState() == true) {
        mavlink_message_t msg;
        mavlink_msg_command_long_pack(255, 190, &msg, 1, 1,
                                      MAV_CMD_DO_SET_MODE, 0, 217, static_cast<int>(GlobalParams::Mode::Circle), 0, 0, 0, 0, 0);//12=> loiter mode
        emit sendMessage(msg);
    }
    });
}
void MavlinkCommunication::SetThrottle(double throttlePercent) {
    GlobalParams::getInstance().setThrottleValue(static_cast<float>(throttlePercent));

    if (GlobalParams::getInstance().getConnectionState() &&
        GlobalParams::getInstance().getCurrentMode() == GlobalParams::Mode::Manual) {
        m_isThrottleActive = true;
        m_throttleTimer.start();
    }
}

void MavlinkCommunication::sendThrottleCommand() {
    if (!GlobalParams::getInstance().getConnectionState() ||
        GlobalParams::getInstance().getCurrentMode() != GlobalParams::Mode::Manual) {
        m_isThrottleActive = false;
        m_throttleTimer.stop();
        return;
    }

    uint16_t pwmValue = static_cast<uint16_t>(1000 + (static_cast<float>(GlobalParams::getInstance().getThrottleValue()) * 10));
    pwmValue = std::min(std::max(pwmValue, static_cast<uint16_t>(1000)), static_cast<uint16_t>(2000));

    mavlink_message_t msg;
    mavlink_msg_rc_channels_override_pack(
        255, 190, &msg,
        1, 1,
        UINT16_MAX, UINT16_MAX,
        pwmValue,
        UINT16_MAX, UINT16_MAX, UINT16_MAX,
        UINT16_MAX, UINT16_MAX, UINT16_MAX,
        UINT16_MAX, UINT16_MAX, UINT16_MAX,
        UINT16_MAX, UINT16_MAX, UINT16_MAX,
        UINT16_MAX, UINT16_MAX, 0
        );

    emit sendMessage(msg);
}

MavlinkCommunication::~MavlinkCommunication() {
    m_isThrottleActive = false;
    m_signalCheckTimer.stop();
    m_throttleTimer.stop();
    m_threadPool->waitForDone();
}
