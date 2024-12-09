#include "MavlinkCommunication.h"
#include <QMessageBox>

MavlinkCommunication::MavlinkCommunication(QObject *parent):
    QObject(parent)
{

}


void MavlinkCommunication::processMAVLinkMessage(const mavlink_message_t& msg) {
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
}
MavlinkCommunication::~MavlinkCommunication() = default;
