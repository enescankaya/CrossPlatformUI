#include "maininterface.h"
#include "qprocess.h"
#include "ui_maininterface.h"
#include <utility>
#include <vector>
#include <functional>
#include <array>
MainInterface::MainInterface(QWidget *parent)
    : QMainWindow(parent)
    , ui(std::make_unique<Ui::BAYKARHP>())
    , currentImageIndex(6),
    process(new QProcess(this))
    {

    ui->setupUi(this);

    // Initialize all subsystems
    setupQmlWidgets();
    setupManagers();
    setupErrorHandling();
    // Perform initial UI setup
    on_uavIcons_Button_clicked();
    on_Swapping_Button_clicked();
    ui->Recording_Label->setVisible(false);
    setUI();
}
void MainInterface::set_SITL()
{
    QString batFilePath = "C:/Users/PC_6270/Desktop/Enes/QtAndroidInterfaceProject/inteface/SITL.bat";
    process->start(batFilePath);
    // Çalışmanın başlatıldığını kontrol et
    if (!process->waitForStarted()) {
        emit showError("Hata", "SITL.bat başlatılamadı!", "red", 2000);
    } else {
        emit showError("Bilgi", "SITL.bat başarıyla başlatıldı!", "", 2000);
    }
}

void MainInterface::setupManagers() {
    // Initialize all managers with smart pointers
    screenManager = std::make_unique<ScreenManager>(ui->small_Frame, ui->main_Frame);
    errorManager = std::make_unique<ErrorManager>();
    widgetManager = std::make_unique<WidgetManager>();
    uiStateManager = std::make_unique<UIStateManager>();
    connectionManager = std::make_unique<ConnectionManager>(this);
    animationManager = std::make_unique<AnimationManager>(this);



    // Register widgets with widget manager
    const std::vector<QQuickWidget*> managedWidgets = {
        ui->Engine_Widget,
        ui->Arm_Widget,
        ui->change_Mode,
        ui->Connection_Widget,
        ui->zoomingWidget
    };

    for (auto* widget : managedWidgets) {
        widgetManager->addWidget(widget);
    }
    // Configure connection manager
    connectionManager->setupConnections(this);
}

void MainInterface::showWarningMessage(QString warningMessage){
    ui->Warning_Label->setText(warningMessage);
}

void MainInterface::setupQmlWidgets() {
    const std::vector<std::pair<QQuickWidget*, QString>> widgets = {
        {ui->Battery_Status_Widget, "BatteryIcon.qml"},
        {ui->Connection_States_Widget_4, "Connections.qml"},
        {ui->zoomingWidget, "Zooming.qml"},
        {ui->fuel_state_Widget, "fuelicon.qml"},
        {ui->change_Mode, "ChangeMode.qml"},
        {ui->ThrottleWidget, "Throttle.qml"},
        {ui->Connection_Widget, "TCP_UDP_Connection.qml"},
        {ui->HUD_Widget, "HUD.qml"},
        {ui->Signal_Widget, "Signal.qml"},
        {ui->Arm_Widget, "ArmingState.qml"},
        {ui->Engine_Widget, "EngineState.qml"},
        {ui->Compass_Widget, "Compass.qml"},
        {ui->CHANGE_ALTITUDE_WIDGET, "Change_Altitude.qml"},
        {ui->Error_Widget, "Errors.qml"},
        {ui->Error_Widget_2, "Errors.qml"},
        {ui->Error_Widget_3, "Errors.qml"}
    };

    for (const auto& [widget, source] : widgets) {
        widgetManager->setupWidget(widget, source);
    }
}

void MainInterface::setupErrorHandling() {
    std::array<std::function<void(QString, QString, QString, int)>, 3> errorHandlers = {
        [this](QString title, QString msg, QString color, int duration) {
            emit showError(title, msg, color, duration);
        },
        [this](QString title, QString msg, QString color, int duration) {
            emit showError2(title, msg, color, duration);
        },
        [this](QString title, QString msg, QString color, int duration) {
            emit showError3(title, msg, color, duration);
        }
    };

    errorManager->initialize(errorHandlers);
}


void MainInterface::handleButtonClick(QQuickWidget* widget) {
    if (!widget->isVisible()) {
        widgetManager->closeAllExcept(widget);
    } else {
        widgetManager->closeAllExcept(nullptr);
    }
}
// Event handlers
void MainInterface::handleModeChange(const QString &modeName, int modeIndex) {
    ui->Mode_Button->setText(modeName);
    uiStateManager->handleModeChange(modeIndex, ui->ThrottleWidget,
                                     ui->CHANGE_ALTITUDE_WIDGET, ui->main_Frame);
    emit changeMode(GlobalParams::getInstance().indexToMode(modeIndex));
    GlobalParams::getInstance().setCurrentMode(GlobalParams::getInstance().indexToMode(modeIndex));
    if(modeIndex==3){//guided mode ise
        emit isGuidedModeOn(true);
    }else{
        emit isGuidedModeOn(false);
    }
}

void MainInterface::handleGuidedMode(int altitude) {
    emit setAltitude(altitude);
    emit AltitudeChanged(static_cast<float>(altitude));

}

void MainInterface::handleEngineStateChanged(bool isEngineOn) {
    uiStateManager->handleEngineState(isEngineOn, ui->Motor_Button);
}

void MainInterface::handleSecurityStateChanged(bool isArmed) {
    GlobalParams::getInstance().setArmState(isArmed);
        uiStateManager->handleSecurityState(isArmed, ui->Arm_Button);
        if(isArmed){
            emit Arm();
        }else{
            emit disArm();
        }
        setUI();
}

void MainInterface::handleConnectionSignal(bool connectionState, const QString &ip, int port) {
    GlobalParams::getInstance().setActiveConnectionType(GlobalParams::ConnectionType::TCP);
    connectionManager->setUpConnections_For_Connections(this);
    if(port==5760){
        //set_SITL();
    }    // Only proceed if there's an actual state change
    if (GlobalParams::getInstance().getTcpConnectionState() != connectionState) {
        GlobalParams::getInstance().setTcpPort(port);
        GlobalParams::getInstance().setTcpIp(ip);
        emit TCP_Connection_State(connectionState, ip, port);
    }

    // Update UI only if the desired state was achieved
    if (GlobalParams::getInstance().getTcpConnectionState() == connectionState) {
        if (connectionState) {
            return;
        }
    }
    //setTCPButton(connectionState);
    //emit updateConnectionState(GlobalParams::getInstance().getTcpConnectionState());
}
void MainInterface::handleUDPConnectionSignal(bool connectionState, const QString &ip, int port) {
    GlobalParams::getInstance().setActiveConnectionType(GlobalParams::ConnectionType::UDP);
    connectionManager->setUpConnections_For_Connections(this);
    if(port==5760){
        //set_SITL();
    }    // Only proceed if there's an actual state change
    if (GlobalParams::getInstance().getUdpConnectionState() != connectionState) {
        GlobalParams::getInstance().setUdpPort(port);
        GlobalParams::getInstance().setUdpIp(ip);
        emit UDP_Connection_State(connectionState, ip, port);
    }

    // Update UI only if the desired state was achieved
    if (GlobalParams::getInstance().getUdpConnectionState() == connectionState) {
        if (connectionState) {
            return;
        }
    }
    //setUDPButton(connectionState);
    //emit updateConnectionState(GlobalParams::getInstance().getUdpConnectionState());
}
void MainInterface::setTCPButton(bool TCP_CONNECTION_STATE){
    uiStateManager->handleConnectionState(TCP_CONNECTION_STATE, ui->Tcp_Button);
    ui->Tcp_Button->setText("TCP");
    qDebug()<<"TCPayarlandı";
    setUI();
}
void MainInterface::setUDPButton(bool UDP_CONNECTION_STATE){
    uiStateManager->handleConnectionState(UDP_CONNECTION_STATE, ui->Tcp_Button);
    ui->Tcp_Button->setText("UDP");
    qDebug()<<"UDPayarlandı";
    setUI();
}
void MainInterface::setUI(){
    widgetManager->closeAllExcept(nullptr);
    widgetManager->setVisibility(ui->ThrottleWidget, ui->main_Frame,
                                 GlobalParams::getInstance().getConnectionState() && ui->ThrottleWidget->isVisible() && GlobalParams::getInstance().getArmState());
    widgetManager->setVisibility(ui->CHANGE_ALTITUDE_WIDGET, ui->main_Frame,
                                 GlobalParams::getInstance().getConnectionState() && ui->CHANGE_ALTITUDE_WIDGET->isVisible() && GlobalParams::getInstance().getArmState());
    ui->Arm_Button->setEnabled(GlobalParams::getInstance().getConnectionState());
    ui->Motor_Button->setEnabled(GlobalParams::getInstance().getConnectionState() && GlobalParams::getInstance().getArmState());
    ui->Mode_Button->setEnabled(GlobalParams::getInstance().getConnectionState() && GlobalParams::getInstance().getArmState());
    qDebug() <<GlobalParams::getInstance().getConnectionState();
}
void MainInterface::handleAltitudeChanged(int value) {
    emit AltitudeChanged(static_cast<float>(value));
}

void MainInterface::handleErrorDismissed() {
    errorManager->handleDismissal();
    if(!GlobalParams::getInstance().getisAllPanelsAvailable()){
        ui->Errors_Frame->stackUnder(ui->main_Frame);
    }
}

// Button click handlers
void MainInterface::on_Mode_Button_clicked() {
    handleButtonClick(ui->change_Mode);//hello
}

void MainInterface::on_Tcp_Button_clicked() {
    handleButtonClick(ui->Connection_Widget);
}

void MainInterface::on_Motor_Button_clicked() {
    handleButtonClick(ui->Engine_Widget);
}

void MainInterface::on_Arm_Button_clicked() {
    handleButtonClick(ui->Arm_Widget);
}

void MainInterface::on_Swapping_Button_clicked() {
    widgetManager->closeAllExcept(nullptr);
    screenManager->swapScreens();
    screenManager->adjustVisibility(ui->videoSettings_Frame, ui->HUD_Widget, ui->Compass_Widget,ui->Warning_Label);
    // Update UI element stacking order
    const std::vector<QWidget*> stackingOrder = {
        ui->UAV_State_Label,
        ui->Swapping_Button,
        ui->Top_Frame,
        ui->infos_Frame,
        ui->small_Frame,
        ui->uavIcons_Button,
        ui->Recording_Label,
        ui->Connection_Widget,
        ui->Signal_Widget,
        ui->Error_Widget,
        ui->Error_Widget_2,
        ui->Error_Widget_3
    };

    for (auto* widget : stackingOrder) {
        widget->raise();
    }

    // Update widget visibility
    widgetManager->setVisibility(ui->ThrottleWidget, ui->main_Frame,
                                 ui->ThrottleWidget->isVisible());
    widgetManager->setVisibility(ui->CHANGE_ALTITUDE_WIDGET, ui->main_Frame,
                                 ui->CHANGE_ALTITUDE_WIDGET->isVisible());
}

void MainInterface::on_takeVideo_Button_clicked() {
    // Update system state
    if (!animationManager->isRecordingAnimationActive()) {
        animationManager->startRecordingAnimation(ui->Recording_Label);
    } else {
        animationManager->stopRecordingAnimation(ui->Recording_Label);
    }

    uiStateManager->setButtonImage(ui->takeVideo_Button,
                                   animationManager->isRecordingAnimationActive());
}

void MainInterface::on_takePhoto_Button_clicked() {
    // Create photo animation
    animationManager->createPhotoButtonAnimation(ui->takePhoto_Button);
}

void MainInterface::on_uavIcons_Button_clicked() {
    QPixmap pixmap(UAV_IMAGES[currentImageIndex]);
    QPixmap scaledPixmap = pixmap.scaled(ui->uavIcons_Button->size(),
                                         Qt::KeepAspectRatio,
                                         Qt::SmoothTransformation);

    ui->uavIcons_Button->setIcon(QIcon(scaledPixmap));
    ui->uavIcons_Button->setIconSize(ui->uavIcons_Button->size());
    currentImageIndex = (currentImageIndex + 1) %7;
}

void MainInterface::on_zoom_Button_clicked() {
    ui->zoomingWidget->setVisible(!ui->zoomingWidget->isVisible());
}

void MainInterface::showMessage(const QString& title, const QString& message,//burayı düzelt
                                const QString& color, int duration) {
    errorManager->handleError(title, message, color, duration);
    if(GlobalParams::getInstance().getisAllPanelsAvailable()){
        ui->Errors_Frame->raise();
    }
}
void MainInterface::UpdateInfos(uint16_t throttle,float airspeed,float groundspeed,int32_t rpm_value){
    ui->THROTTLE_VALUE->setText(QString::number(throttle));
    ui->AIR_SPEED_VALUE->setText(QString::number(airspeed, 'f', 1));
    ui->GROUND_SPEED_VALUE->setText(QString::number(groundspeed, 'f', 1));
    ui->RPM_VALUE->setText(QString::number(rpm_value));
}
void MainInterface::showTime(bool isActive) {
    static QTime elapsedTime(0, 0, 0); // Statik sayaç başlangıcı (00:00:00)
    static bool wasActive = false;    // Önceki durumun takibi

    if (isActive) {
        if (!wasActive) {
            elapsedTime = QTime(0, 0, 0);
            ui->UAV_State_Label->setText("FLYING");
        }

        elapsedTime = elapsedTime.addMSecs(500);

        QString timeString = elapsedTime.toString("HH:mm:ss");
        ui->Counter->display(timeString);

        wasActive = true;
    } else {
        if (wasActive) {
            ui->Counter->display("00:00:00");
            ui->UAV_State_Label->setText("READY TO FLY");
        }

        wasActive = false;
    }
}

MainInterface::~MainInterface(){
    if (process) {
        process->kill();
        process->waitForFinished();
        delete process;
    }
}

