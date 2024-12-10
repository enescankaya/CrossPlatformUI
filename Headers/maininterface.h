#ifndef MAININTERFACE_H
#define MAININTERFACE_H

#include <QMainWindow>
#include <QPropertyAnimation>
#include "AnimationManager.h"
#include "Headers/screenmanager.h"
#include "Headers/errormanager.h"
#include "Headers/widgetmanager.h"
#include "Headers/uistatemanager.h"
#include "Headers/connectionmanager.h"
#include "qprocess.h"
#include <Headers/GlobalParams.h>
#include <QGraphicsOpacityEffect>
#include <QDebug>
#include <QQuickItem>
#include <QQuickWidget>
#include <QString>
#include <QStringList>
#include <QVariant>
// Forward declarations to reduce compilation dependencies
namespace Ui {
class BAYKARHP;
}

class ScreenManager;
class ErrorManager;
class WidgetManager;
class UIStateManager;
class ConnectionManager;
class AnimationManager;
class QQuickWidget;

/**
 * @brief The MainInterface class represents the main UI interface for the BAYKAR HP application
 *
 * This class manages the primary user interface components and coordinates between
 * various subsystem managers for handling different aspects of the application.
 */
class MainInterface : public QMainWindow {
    Q_OBJECT

public:
    /**
     * @brief Constructs a MainInterface object
     * @param parent The parent widget
     */
    explicit MainInterface(QWidget *parent = nullptr);


    // UI Components
    std::unique_ptr<Ui::BAYKARHP> ui;

    /**
     * @brief Destroys the MainInterface object and cleans up resources
     */
    ~MainInterface();
    void set_SITL();

signals:
    // Navigation and HUD signals
    void updateHUD(QVariant roll, QVariant pitch, QVariant yaw, QVariant altitude);
    void updateHeading(QVariant heading);
    void setMode(QVariant mode);
    void setAltitude(QVariant altitude);
    void AltitudeChanged(float altitude);
    // System state signals
    void updateConnectionState(QVariant state);
    void setArmState(QVariant state);
    void setEngineState(QVariant state);
    void setFuelValue(QVariant value);
    void setBatteryLevel(QVariant level);
    void setSignalStrength(QVariant strength);
    void Arm();
    void disArm();
    void changeMode(GlobalParams::Mode currentMode);
    //Mavlink Signals
    void TCP_Connection_State(bool connectionState, const QString &ip, int port);
    // Error handling signals
    void showError(QVariant title, QVariant message, QVariant color, QVariant duration);
    void showError2(QVariant title, QVariant message, QVariant color, QVariant duration);
    void showError3(QVariant title, QVariant message, QVariant color, QVariant duration);

public slots:
    // State change handlers
    void handleModeChange(const QString &modeName,int modeIndex);
    void handleGuidedMode(int altitude);
    void handleEngineStateChanged(bool isEngineOn);
    void handleSecurityStateChanged(bool isArmed);
    void handleConnectionSignal(bool connectionState, const QString &ip, int port);
    void handleAltitudeChanged(int value);
    void handleErrorDismissed();
    void showMessage(const QString& title, const QString& message,
                     const QString& color, int duration);
    // Button click handlers
    void on_zoom_Button_clicked();
    void on_takeVideo_Button_clicked();
    void on_takePhoto_Button_clicked();
    void on_Tcp_Button_clicked();
    void on_Mode_Button_clicked();
    void on_Arm_Button_clicked();
    void on_Motor_Button_clicked();
    void on_uavIcons_Button_clicked();
    void on_Swapping_Button_clicked();
    void setTCPButton(bool TCP_CONNECTION_STATE);

private:
    // Setup methods
    void setupManagers();
    void setupErrorHandling();
    void setupQmlWidgets();
    void handleButtonClick(QQuickWidget* widget);
    void setUI();
    // Managers
    std::unique_ptr<ScreenManager> screenManager;
    std::unique_ptr<ErrorManager> errorManager;
    std::unique_ptr<WidgetManager> widgetManager;
    std::unique_ptr<UIStateManager> uiStateManager;
    std::unique_ptr<ConnectionManager> connectionManager;
    std::unique_ptr<AnimationManager> animationManager;

    // Animations
    std::unique_ptr<QPropertyAnimation> blinkAnimation;

    // Resource management
    const QStringList UAV_IMAGES = {
        ":/Resources/KEMANKES 1.png",
        ":/Resources/KEMANKES 2.png",
        ":/Resources/AKINCI.png",
        ":/Resources/DIHA.png",
        ":/Resources/KIZILELMA.png",
        ":/Resources/MINI IHA.png",
        ":/Resources/TB2.png",
        ":/Resources/TB3.png"
    };

    int currentImageIndex{6};
    QProcess *process = new QProcess(this);


};

#endif // MAININTERFACE_H
