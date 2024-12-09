#include "maininterface.h"
#include "qprocess.h"
#include "ui_maininterface.h"
#include <utility>
#include <vector>
#include <functional>
#include <array>
auto& globals = GlobalParams::getInstance();
MainInterface::MainInterface(QWidget *parent)
    : QMainWindow(parent)
    , ui(std::make_unique<Ui::BAYKARHP>())
    , currentImageIndex(6) {

    ui->setupUi(this);

    // Initialize all subsystems
    setupQmlWidgets();
    setupManagers();
    setupErrorHandling();
    // Perform initial UI setup
    on_uavIcons_Button_clicked();
    on_Swapping_Button_clicked();
    ui->Recording_Label->setVisible(false);


}
void MainInterface::set_SITL(){
    QString batFilePath ="C:/Users/PC_6270/Desktop/Enes/QtAndroidInterfaceProject/inteface/SITL.bat";
    QProcess *process = new QProcess(this);
    process->start(batFilePath);
    if (!process->waitForStarted()) {
        emit showError("Hata", "SITL.bat başlatılamadı!", "red", 2000);
            } else {
        emit showError("Hata", "SITL.bat başarıyla başlatıldı!", "", 2000);
    }
    // BAT dosyasını çalıştır
}
void MainInterface::setupManagers() {
    // Initialize all managers with smart pointers
    screenManager = std::make_unique<ScreenManager>(ui->small_Frame, ui->main_Frame);
    errorManager = std::make_unique<ErrorManager>();
    widgetManager = std::make_unique<WidgetManager>();
    uiStateManager = std::make_unique<UIStateManager>();
    connectionManager = std::make_unique<ConnectionManager>(this);
    animationManager = std::make_unique<AnimationManager>(this);

    // Configure connection manager
    connectionManager->setupConnections(this);

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
}

void MainInterface::setupQmlWidgets() {
    const std::vector<std::pair<QQuickWidget*, QString>> widgets = {
        {ui->Battery_Status_Widget, "BatteryIcon.qml"},
        {ui->Connection_States_Widget_4, "Connections.qml"},
        {ui->zoomingWidget, "Zooming.qml"},
        {ui->fuel_state_Widget, "fuelicon.qml"},
        {ui->change_Mode, "ChangeMode.qml"},
        {ui->ThrottleWidget, "Throttle.qml"},
        {ui->Connection_Widget, "Tcp_Connection.qml"},
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
}

void MainInterface::handleGuidedMode(int altitude) {
    emit setAltitude(altitude);
}

void MainInterface::handleEngineStateChanged(bool isEngineOn) {
    uiStateManager->handleEngineState(isEngineOn, ui->Motor_Button);
}

void MainInterface::handleSecurityStateChanged(bool isArmed) {
    uiStateManager->handleSecurityState(isArmed, ui->Arm_Button);
}

void MainInterface::handleConnectionSignal(bool connectionState, const QString &ip, int port) {
    auto &globals = GlobalParams::getInstance();
    set_SITL();
    // Only proceed if there's an actual state change
    if (globals.TCP_CONNECTION_STATE != connectionState) {
        emit TCP_Connection_State(connectionState, ip, port);
    }

    // Update UI only if the desired state was achieved
    if (globals.TCP_CONNECTION_STATE == connectionState) {
        setTCPButton(connectionState);
        if (connectionState) {
            globals.TCP_Current_port = port;
            globals.TCP_Current_ip = ip;
        }
    }

    emit updateConnectionState(globals.TCP_CONNECTION_STATE);
}
void MainInterface::setTCPButton(bool TCP_CONNECTION_STATE){
    uiStateManager->handleConnectionState(TCP_CONNECTION_STATE, ui->Tcp_Button);
    setUI();
}

void MainInterface::setUI(){
    widgetManager->closeAllExcept(nullptr);
    widgetManager->setVisibility(ui->ThrottleWidget, ui->main_Frame,
                                 globals.TCP_CONNECTION_STATE && ui->ThrottleWidget->isVisible());
    widgetManager->setVisibility(ui->CHANGE_ALTITUDE_WIDGET, ui->main_Frame,
                                 globals.TCP_CONNECTION_STATE && ui->CHANGE_ALTITUDE_WIDGET->isVisible());
    ui->Arm_Button->setEnabled(globals.TCP_CONNECTION_STATE);
    ui->Motor_Button->setEnabled(globals.TCP_CONNECTION_STATE);
    ui->Mode_Button->setEnabled(globals.TCP_CONNECTION_STATE);
}
void MainInterface::handleAltitudeChanged(int value) {
    qDebug() << "Altitude changed to:" << value;
}

void MainInterface::handleErrorDismissed() {
    errorManager->handleDismissal();
    if(!globals.allPanelsAvailable){
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
    screenManager->adjustVisibility(ui->videoSettings_Frame, ui->HUD_Widget, ui->Compass_Widget);
    // Update UI element stacking order
    const std::vector<QWidget*> stackingOrder = {
        ui->BaykarLabel,
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
    errorManager->handleError("merhaba", "hayır", "green", 5000);
    emit updateConnectionState(true);
    emit setMode(0);

    // Update HUD values
    static int value = 0;
    emit updateHUD(value, value, value, value);
    emit updateHeading(value);
    emit setAltitude(value);
    value += 1;

    // Handle recording animation
    if (!animationManager->isRecordingAnimationActive()) {
        animationManager->startRecordingAnimation(ui->Recording_Label);
    } else {
        animationManager->stopRecordingAnimation(ui->Recording_Label);
    }

    uiStateManager->setButtonImage(ui->takeVideo_Button,
                                   animationManager->isRecordingAnimationActive());
}

void MainInterface::on_takePhoto_Button_clicked() {
    // Update system state
    errorManager->handleError("fs", "ew", "red", 3000);
    emit setArmState(true);
    emit setEngineState(true);
    emit setFuelValue(12);
    emit setBatteryLevel(88);
    emit setSignalStrength(94);
    emit updateConnectionState(false);

    // Create photo animation
    animationManager->createPhotoButtonAnimation(ui->takePhoto_Button);

    // Update HUD values
    static int value = 0;
    emit updateHUD(value, value, value, value);
    emit updateHeading(value);
    value -= 1;
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
    errorManager->handleError("OCIS", "okay", "", 5000);
}

void MainInterface::showMessage(const QString& title, const QString& message,//burayı düzelt
                                const QString& color, int duration) {
    errorManager->handleError(title, message, color, duration);
    if(globals.allPanelsAvailable){
        ui->Errors_Frame->raise();
    }
}

MainInterface::~MainInterface() = default;
