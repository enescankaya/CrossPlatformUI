#include "connectionmanager.h"
#include "maininterface.h"
#include "ui_maininterface.h"

ConnectionManager::ConnectionManager(QObject* parent) :
    QObject(parent),
    tcpManager(new TcpManager(this)),
    udpManager(new UdpManager(this)),
    mavlink_Class(new MavlinkCommunication(this))
{

}
void ConnectionManager::setupConnections(MainInterface* mainInterface) {

    //MavlinkSignal Qml
    auto rootObjectMavlink = mainInterface->ui->Connection_States_Widget_4->rootObject();
    connect(mavlink_Class, SIGNAL(setMavlinkSignalValue(int)), rootObjectMavlink, SLOT(setSignalValue(int)));

    // Mode connections
    auto rootObjectMode = mainInterface->ui->change_Mode->rootObject();
    connect(rootObjectMode, SIGNAL(modeChanged(QString,int)), mainInterface, SLOT(handleModeChange(QString,int)));
    connect(mavlink_Class, SIGNAL(setMode(QVariant)), rootObjectMode, SLOT(setMode(QVariant)));
    connect(rootObjectMode, SIGNAL(setGuidedMode(int)), mainInterface, SLOT(handleGuidedMode(int)));

    // Throttle
    auto rootObjectThrottle = mainInterface->ui->ThrottleWidget->rootObject();
    connect(rootObjectThrottle, SIGNAL(throttleValueChanged(double)), mavlink_Class, SLOT(SetThrottle(double)));

    // Connection widget
    auto rootObjectConnection = mainInterface->ui->Connection_Widget->rootObject();
    connect(rootObjectConnection, SIGNAL(connectionSignal(bool,QString,int)),
            mainInterface, SLOT(handleConnectionSignal(bool,QString,int)));
    connect(rootObjectConnection, SIGNAL(udpConnectionSignal(bool,QString,int)),
            mainInterface, SLOT(handleUDPConnectionSignal(bool,QString,int)));
    connect(mainInterface, SIGNAL(updateConnectionState(QVariant)),
            rootObjectConnection, SLOT(onExternalConnectionSignal(QVariant)));

    // Engine state
    auto rootObjectEngine = mainInterface->ui->Engine_Widget->rootObject();
    connect(rootObjectEngine, SIGNAL(engineStateChanged(bool)),
            mainInterface, SLOT(handleEngineStateChanged(bool)));
    connect(mavlink_Class, SIGNAL(setEngineState(QVariant)),
            rootObjectEngine, SLOT(setMode(QVariant)));

    // Arm state
    auto rootObjectArm = mainInterface->ui->Arm_Widget->rootObject();
    connect(rootObjectArm, SIGNAL(securityStateChanged(bool)),
            mainInterface, SLOT(handleSecurityStateChanged(bool)));
    connect(mavlink_Class, SIGNAL(setArmState(QVariant)),
            rootObjectArm, SLOT(setMode(QVariant)));

    // HUD and telemetry
    auto rootObjectHUD = mainInterface->ui->HUD_Widget->rootObject();
    connect(mavlink_Class, SIGNAL(updateHUD(QVariant,QVariant,QVariant,QVariant)),
            rootObjectHUD, SLOT(updateValues(QVariant,QVariant,QVariant,QVariant)));

    auto rootObjectCompass = mainInterface->ui->Compass_Widget->rootObject();
    connect(mainInterface, SIGNAL(updateHeading(QVariant)),
            rootObjectCompass, SLOT(updateHeading(QVariant)));

    // Altitude
    auto rootObjectAltitude = mainInterface->ui->CHANGE_ALTITUDE_WIDGET->rootObject();
    connect(mainInterface, SIGNAL(setAltitude(QVariant)),
            rootObjectAltitude, SLOT(setAltitude(QVariant)));
    connect(rootObjectAltitude, SIGNAL(altitudeChanged(int)),
            mainInterface, SLOT(handleAltitudeChanged(int)));

    // Error widgets
    auto rootObjectError1 = mainInterface->ui->Error_Widget->rootObject();
    connect(mainInterface, SIGNAL(showError(QVariant,QVariant,QVariant,QVariant)),
            rootObjectError1, SLOT(showError(QVariant,QVariant,QVariant,QVariant)));
    connect(rootObjectError1, SIGNAL(errorDismissed()),
            mainInterface, SLOT(handleErrorDismissed()));

    auto rootObjectError2 = mainInterface->ui->Error_Widget_2->rootObject();
    connect(mainInterface, SIGNAL(showError2(QVariant,QVariant,QVariant,QVariant)),
            rootObjectError2, SLOT(showError(QVariant,QVariant,QVariant,QVariant)));
    connect(rootObjectError2, SIGNAL(errorDismissed()),
            mainInterface, SLOT(handleErrorDismissed()));

    auto rootObjectError3 = mainInterface->ui->Error_Widget_3->rootObject();
    connect(mainInterface, SIGNAL(showError3(QVariant,QVariant,QVariant,QVariant)),
            rootObjectError3, SLOT(showError(QVariant,QVariant,QVariant,QVariant)));
    connect(rootObjectError3, SIGNAL(errorDismissed()),
            mainInterface, SLOT(handleErrorDismissed()));

    // Status widgets

    auto rootObjectSignal = mainInterface->ui->Signal_Widget->rootObject();
    connect(mavlink_Class, SIGNAL(setSignalStrength(QVariant)),
            rootObjectSignal, SLOT(setValue(QVariant)));

    auto rootObjectFuel = mainInterface->ui->fuel_state_Widget->rootObject();
    connect(mavlink_Class, SIGNAL(setFuelValue(QVariant)),
            rootObjectFuel, SLOT(setValue(QVariant)));

    auto rootObjectBattery = mainInterface->ui->Battery_Status_Widget->rootObject();
    connect(mavlink_Class, SIGNAL(setBatteryLevel(QVariant)),
            rootObjectBattery, SLOT(setValue(QVariant)));

    //Singleton
    connect(&GlobalParams::getInstance(), &GlobalParams::showMessage,mainInterface, &MainInterface::showMessage);
    connect(&GlobalParams::getInstance(), &GlobalParams::setTCPButton,mainInterface, &MainInterface::setTCPButton);
    connect(&GlobalParams::getInstance(), &GlobalParams::setUDPButton,mainInterface, &MainInterface::setUDPButton);

    //Mavlink Connections
    connect(mainInterface,&MainInterface::Arm,mavlink_Class,&MavlinkCommunication::Arm,Qt::QueuedConnection);
    connect(mainInterface,&MainInterface::disArm,mavlink_Class,&MavlinkCommunication::disArm,Qt::QueuedConnection);
    connect(mainInterface,&MainInterface::changeMode,mavlink_Class,&MavlinkCommunication::changeMode,Qt::QueuedConnection);
    connect(mainInterface,&MainInterface::AltitudeChanged,mavlink_Class,&MavlinkCommunication::SetAltitude,Qt::QueuedConnection);
    connect(mavlink_Class,&MavlinkCommunication::updateHeading,mainInterface,&MainInterface::updateHeading,Qt::QueuedConnection);
    connect(mavlink_Class,&MavlinkCommunication::updateInfoHud,mainInterface,&MainInterface::UpdateInfos,Qt::QueuedConnection);
    connect(mavlink_Class,&MavlinkCommunication::updateClock,mainInterface,&MainInterface::showTime,Qt::QueuedConnection);
    connect(mavlink_Class, &MavlinkCommunication::showWarningMessage, mainInterface, &MainInterface::showWarningMessage);
    //Map
    connect(mavlink_Class, SIGNAL(setMap(QVariant,QVariant,QVariant)), GlobalParams::getInstance().getMapScreen()->qmlRootObject, SLOT(addMarker(QVariant,QVariant,QVariant)),Qt::QueuedConnection);
    connect(GlobalParams::getInstance().getMapScreen()->qmlRootObject, SIGNAL(rightClickSignal(double,double)), mavlink_Class, SLOT(Go_Coordinate(double,double)),Qt::QueuedConnection);
    connect(GlobalParams::getInstance().getMapScreen()->qmlRootObject, SIGNAL(removerightClickSignal()), mavlink_Class, SLOT(Remove_Coordinate()),Qt::QueuedConnection);
    connect(mainInterface, SIGNAL(isGuidedModeOn(QVariant)), GlobalParams::getInstance().getMapScreen()->qmlRootObject, SLOT(changeMode(QVariant)));
    //Connection TYPES
    setUpConnections_For_Connections(mainInterface);
}
void ConnectionManager::setUpConnections_For_Connections(MainInterface* mainInterface){
     if(GlobalParams::getInstance().getActiveConnectionType()==GlobalParams::ConnectionType::TCP && CurrentConnection!=GlobalParams::ConnectionType::TCP){
         //Disconnect UDP Connections
        disconnect(mainInterface, &MainInterface::UDP_Connection_State,udpManager, &UdpManager::requestConnection);
        disconnect(udpManager, &UdpManager::showMessage,mainInterface, &MainInterface::showMessage);
        disconnect(udpManager, &UdpManager::connectionStateChanged,mainInterface, &MainInterface::updateConnectionState);
        disconnect(udpManager,&UdpManager::processMAVLinkMessage,mavlink_Class,&MavlinkCommunication::processMAVLinkMessage);
        disconnect(mavlink_Class,&MavlinkCommunication::sendMessage,udpManager,&UdpManager::sendMavlinkMessage);
         //TCP Connections
        connect(mainInterface,&MainInterface::TCP_Connection_State,tcpManager, &TcpManager::requestConnection);
        connect(tcpManager, &TcpManager::showMessage,mainInterface, &MainInterface::showMessage);
        connect(tcpManager, &TcpManager::connectionStateChanged,mainInterface, &MainInterface::updateConnectionState);
        connect(tcpManager,&TcpManager::processMAVLinkMessage,mavlink_Class,&MavlinkCommunication::processMAVLinkMessage,Qt::QueuedConnection);
        connect(mavlink_Class,&MavlinkCommunication::sendMessage,tcpManager,&TcpManager::sendMavlinkMessage,Qt::QueuedConnection);
        CurrentConnection=GlobalParams::ConnectionType::TCP;
        return;
     }
     if(GlobalParams::getInstance().getActiveConnectionType()==GlobalParams::ConnectionType::UDP && CurrentConnection!=GlobalParams::ConnectionType::UDP){
         // Disconnect TCP Connections
        disconnect(mainInterface, &MainInterface::TCP_Connection_State,tcpManager, &TcpManager::requestConnection);
        disconnect(tcpManager, &TcpManager::showMessage,mainInterface, &MainInterface::showMessage);
        disconnect(tcpManager, &TcpManager::connectionStateChanged,mainInterface, &MainInterface::updateConnectionState);
        disconnect(tcpManager, &TcpManager::processMAVLinkMessage,mavlink_Class,&MavlinkCommunication::processMAVLinkMessage);
        disconnect(mavlink_Class, &MavlinkCommunication::sendMessage,tcpManager,&TcpManager::sendMavlinkMessage);
         //UDP Connections
        connect(mainInterface,&MainInterface::UDP_Connection_State,udpManager, &UdpManager::requestConnection);
        connect(udpManager, &UdpManager::showMessage,mainInterface, &MainInterface::showMessage);
        connect(udpManager, &UdpManager::connectionStateChanged,mainInterface, &MainInterface::updateConnectionState);
        connect(udpManager,&UdpManager::processMAVLinkMessage,mavlink_Class,&MavlinkCommunication::processMAVLinkMessage,Qt::QueuedConnection);
        connect(mavlink_Class,&MavlinkCommunication::sendMessage,udpManager,&UdpManager::sendMavlinkMessage,Qt::QueuedConnection);
        CurrentConnection=GlobalParams::ConnectionType::UDP;
        return;
     }
}
