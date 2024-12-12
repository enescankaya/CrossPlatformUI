#include "MavlinkCommunication.h"
#include <QMessageBox>
#include <QMutexLocker>

MavlinkCommunication::MavlinkCommunication(QObject *parent):
    QObject(parent)
{
    m_lastHeartbeat.start();

    // Create timer to continuously check heartbeat status
    QTimer* signalCheckTimer = new QTimer(this);
    connect(signalCheckTimer, &QTimer::timeout, this, &MavlinkCommunication::updateMavlinkSignalStrength);
    signalCheckTimer->start(500); // Check every 500ms
}
void MavlinkCommunication::updateMavlinkSignalStrength() {
    qint64 timeSinceLastHeartbeat = m_lastHeartbeat.elapsed();

    // Calculate signal strength (0-100)
    int signalStrength;
    if (timeSinceLastHeartbeat <= 1000) { // Good signal: less than 1 second
        signalStrength = 100;
    } else if (timeSinceLastHeartbeat >= MAX_HEARTBEAT_INTERVAL) { // No signal
        signalStrength = 0;
    } else { // Linear interpolation between 1000ms and MAX_HEARTBEAT_INTERVAL
        signalStrength = static_cast<int>(100 * (1.0 - (timeSinceLastHeartbeat - 1000.0) / (MAX_HEARTBEAT_INTERVAL - 1000.0)));
    }

    emit setMavlinkSignalValue(signalStrength);
}

void MavlinkCommunication::processMAVLinkMessage(const mavlink_message_t& msg) {
    QFuture<void> future = QtConcurrent::run([&msg, this]() {
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
        case MAVLINK_MSG_ID_NAV_CONTROLLER_OUTPUT:
            //handleNavigation(msg);
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
        default:
            break;
        }
    });
}

void MavlinkCommunication::handleInfos(const mavlink_message_t& msg) {
    mavlink_vfr_hud_t hud;
    mavlink_msg_vfr_hud_decode(&msg, &hud);
    emit updateInfoHud(hud.throttle,hud.airspeed,hud.groundspeed,GlobalParams::getInstance().rpm_value);
}
void MavlinkCommunication::handleRPM(const mavlink_message_t& msg) {
    mavlink_esc_status_t esc_status;
    mavlink_msg_esc_status_decode(&msg, &esc_status);
    GlobalParams::getInstance().rpm_value = esc_status.rpm[0];  // Assuming first ESC is main
}
void MavlinkCommunication::handleRSSI(const mavlink_message_t& msg) {
    mavlink_rc_channels_t rc_channels;
    mavlink_msg_rc_channels_decode(&msg, &rc_channels);
    emit setSignalStrength(static_cast<int>((rc_channels.rssi / 255.0) * 100.0));
}

void MavlinkCommunication::handleFuelAndBatteryStatus(const mavlink_message_t& msg) {
    mavlink_sys_status_t sys_status;
    mavlink_msg_sys_status_decode(&msg, &sys_status);
    emit setFuelValue(sys_status.load / 10.0f);
    emit setBatteryLevel(sys_status.battery_remaining);
}

void MavlinkCommunication::handleHeartbeat(const mavlink_message_t& msg) {
    mavlink_heartbeat_t heartbeat;
    mavlink_msg_heartbeat_decode(&msg, &heartbeat);
    m_lastHeartbeat.restart();
    // Extract arm state and current mode
    bool armed = (heartbeat.base_mode & MAV_MODE_FLAG_SAFETY_ARMED);
    if(armed != GlobalParams::getInstance().isArmed){
        emit setArmState(armed);
        emit GlobalParams::getInstance().showMessage(armed ? "Armed": "Disarmed","State Changed", armed ?  "green":"red",2000);
    }
    if(static_cast<uint8_t>(GlobalParams::getInstance().currentMode)!=heartbeat.custom_mode){
        emit setMode(GlobalParams::getInstance().getModeIndex(heartbeat.custom_mode));
        emit GlobalParams::getInstance().showMessage("Mode Changed", "Mode Has Been Changed","",2000);
    }
}
void MavlinkCommunication::handleGlobalPosition(const mavlink_message_t& msg) {
    mavlink_global_position_int_t global_position;
    mavlink_msg_global_position_int_decode(&msg, &global_position);
    GlobalParams::getInstance().vertical_speed=global_position.vz / 100.0f;
    GlobalParams::getInstance().altitude=global_position.relative_alt / 1000.0;
    emit setMap(global_position.lat / 1e7,global_position.lon / 1e7,global_position.hdg/100.0);
    emit updateHeading(global_position.hdg / 100.0);
}
void MavlinkCommunication::handleAttitude(const mavlink_message_t& msg) {
    mavlink_attitude_t attitude;
    mavlink_msg_attitude_decode(&msg, &attitude);
    emit updateHUD(GlobalParams::getInstance().vertical_speed,GlobalParams::getInstance().altitude,attitude.pitch * 180 / M_PI,attitude.roll * 180 / M_PI);
}
void MavlinkCommunication::disArm(){
    QFuture<void> future = QtConcurrent::run([=]() {
    if (GlobalParams::getInstance().TCP_CONNECTION_STATE == true) {
        mavlink_message_t msg;
        mavlink_msg_command_long_pack(MAV_TYPE_GCS, MAV_AUTOPILOT_GENERIC, &msg, 1, 1, MAV_CMD_COMPONENT_ARM_DISARM, 1, 0, 21196, 0, 0, 0, 0, 0);
        emit sendMessage(msg);
    } else {
        emit GlobalParams::getInstance().showMessage("Not Connected", "Please connect to the server first.","",2000);
    }
    });
}
void MavlinkCommunication::Arm(){
    QFuture<void> future = QtConcurrent::run([=]() {
    if (GlobalParams::getInstance().TCP_CONNECTION_STATE == true) {
        mavlink_message_t msg;
        mavlink_msg_command_long_pack(MAV_TYPE_GCS, MAV_AUTOPILOT_GENERIC, &msg, 1, 1, MAV_CMD_COMPONENT_ARM_DISARM, 1, 1, 2989, 0, 0, 0, 0, 0);
        emit sendMessage(msg);
    }
    });
}
void MavlinkCommunication::changeMode(GlobalParams::Mode currentMode) {
    QFuture<void> future = QtConcurrent::run([=]() {
    mavlink_message_t msg;
    mavlink_msg_command_long_pack(MAV_TYPE_GCS, MAV_AUTOPILOT_GENERIC, &msg,
                                  1, 1, // Target system and component
                                  MAV_CMD_DO_SET_MODE, 0, // Command and confirmation
                                  217, // Custom mode (arbitrary value)
                                  static_cast<int>(currentMode), // Mode as int
                                  0, 0, 0, 0, 0); // Other parameters (unused)
    emit sendMessage(msg);
    });
}
void MavlinkCommunication::SetAltitude(float altitude) {
    GlobalParams::getInstance().altitude = altitude;
    QFuture<void> future = QtConcurrent::run([=]() {
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
            MAV_TYPE_GCS,           // Sender system type
            MAV_AUTOPILOT_GENERIC,  // Sender autopilot type
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
    });
}
void MavlinkCommunication::Go_Coordinate(double lat, double lng) {
    QFuture<void> future = QtConcurrent::run([=]() {
    // Convert latitude and longitude to MAVLink's expected format
    int32_t lat_mav = static_cast<int32_t>(lat * 1e7);
    int32_t lng_mav = static_cast<int32_t>(lng * 1e7);

        mavlink_message_t msg;

        mavlink_msg_mission_item_int_pack(
            MAV_TYPE_GCS, MAV_AUTOPILOT_GENERIC, &msg,
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
            GlobalParams::getInstance().altitude,  // Altitude in meters
            MAV_MISSION_TYPE_MISSION
            );
        emit sendMessage(msg);
    });
}
void MavlinkCommunication::Remove_Coordinate() {
    QFuture<void> future = QtConcurrent::run([=]() {
        mavlink_message_t msg;
        mavlink_msg_command_long_pack(MAV_TYPE_GCS, MAV_AUTOPILOT_GENERIC, &msg, 1, 1,
                                      MAV_CMD_DO_SET_MODE, 0, 217, static_cast<int>(GlobalParams::Mode::Circle), 0, 0, 0, 0, 0);//12=> loiter mode
        emit sendMessage(msg);
    });
}
// throttlePercent: 0-100 arasında bir değer
void MavlinkCommunication::SetThrottle(double throttlePercent) {
    GlobalParams::getInstance().ThrottleValue = static_cast<float>(throttlePercent);
    QFuture<void> future = QtConcurrent::run([=]() {
        while(GlobalParams::getInstance().currentMode == GlobalParams::Manual && GlobalParams::getInstance().TCP_CONNECTION_STATE) {
            // Yüzdelik değeri PWM değerine çevirme (1000-2000 PWM aralığı)
            uint16_t pwmValue = static_cast<uint16_t>(1000 + (static_cast<float>(GlobalParams::getInstance().ThrottleValue) * 10));
            // PWM değerini sınırla
            pwmValue = std::min(std::max(pwmValue, static_cast<uint16_t>(1000)), static_cast<uint16_t>(2000));
            mavlink_message_t msg;

            mavlink_msg_rc_channels_override_pack(
                255,           // Sender system type
                190,           // Sender autopilot type
                &msg,
                1,             // Target system
                1,             // Target component
                UINT16_MAX,    // RC Channel 1 (Roll) - ignore
                UINT16_MAX,    // RC Channel 2 (Pitch) - ignore
                pwmValue,      // RC Channel 3 (Throttle)
                UINT16_MAX,    // RC Channel 4 (Yaw) - ignore
                UINT16_MAX,    // RC Channel 5 - ignore
                UINT16_MAX,    // RC Channel 6 - ignore
                UINT16_MAX,    // RC Channel 7 - ignore
                UINT16_MAX,    // RC Channel 8 - ignore
                UINT16_MAX,    // RC Channel 9 - ignore
                UINT16_MAX,    // RC Channel 10 - ignore
                UINT16_MAX,    // RC Channel 11 - ignore
                UINT16_MAX,    // RC Channel 12 - ignore
                UINT16_MAX,    // RC Channel 13 - ignore
                UINT16_MAX,    // RC Channel 14 - ignore
                UINT16_MAX,    // RC Channel 15 - ignore
                UINT16_MAX,    // RC Channel 16 - ignore
                UINT16_MAX,    // RC Channel 17 - ignore
                0              // Sign/Flags field for MAVLink 2
                );
            emit sendMessage(msg);

            // 2 HZ FREKANS(MISSION PLANNER ILE ESITLENDI)
            QThread::msleep(500);
        }
    });
}
MavlinkCommunication::~MavlinkCommunication() = default;
