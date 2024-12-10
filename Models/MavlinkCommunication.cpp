#include "MavlinkCommunication.h"
#include <QMessageBox>
#include <QMutexLocker>

MavlinkCommunication::MavlinkCommunication(QObject *parent):
    QObject(parent)
{

}


void MavlinkCommunication::processMAVLinkMessage(const mavlink_message_t& msg) {
    QFuture<void> future = QtConcurrent::run([&msg]() {
        switch (msg.msgid) {
        case MAVLINK_MSG_ID_GLOBAL_POSITION_INT:
            //handleGlobalPosition(msg);
            break;
        case MAVLINK_MSG_ID_ATTITUDE:
            //handleAttitude(msg);
            break;
        case MAVLINK_MSG_ID_VFR_HUD:
            //handleAirspeed(msg);
            break;
        case MAVLINK_MSG_ID_SCALED_PRESSURE:
            //handlePressure(msg);
            break;
        case MAVLINK_MSG_ID_NAV_CONTROLLER_OUTPUT:
            //handleNavigation(msg);
            break;
        default:
            break;
        }
    });

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

MavlinkCommunication::~MavlinkCommunication() = default;
