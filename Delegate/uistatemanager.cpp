#include "Headers/uistatemanager.h"

UIStateManager::UIStateManager() : engineOn(false), armed(false) {}

void UIStateManager::handleModeChange(int modeIndex,
                                      QWidget* throttleWidget, QWidget* altitudeWidget,
                                      QWidget* mainFrame) {
    if (modeIndex == 0) {  // Manual mode
        throttleWidget->raise();
        throttleWidget->setVisible(true);
        altitudeWidget->stackUnder(mainFrame);
        altitudeWidget->setVisible(false);
    } else if (modeIndex == 3) {  // Guided mode
        altitudeWidget->raise();
        altitudeWidget->setVisible(true);
        throttleWidget->stackUnder(mainFrame);
        throttleWidget->setVisible(false);
    } else {
        throttleWidget->stackUnder(mainFrame);
        throttleWidget->setVisible(false);
        altitudeWidget->stackUnder(mainFrame);
        altitudeWidget->setVisible(false);
    }
}

void UIStateManager::handleEngineState(bool isEngineOn, QPushButton* motorButton) {
    engineOn = isEngineOn;
    QString iconPath = engineOn ? ":/Resources/plane-engine-ON.png" : ":/Resources/plane-engine-OFF.png";
    motorButton->setIcon(QIcon(iconPath));
}

void UIStateManager::handleSecurityState(bool isArmed, QPushButton* armButton) {
    armed = isArmed;
    QString iconPath = armed ? ":/Resources/unlock.png" : ":/Resources/lock.png";
    armButton->setIcon(QIcon(iconPath));
}

void UIStateManager::handleConnectionState(bool state, QPushButton* tcpButton) {
    QString iconPath = state ? ":/Resources/TCP_ON.png" : ":/Resources/TCP_OFF.png";
    tcpButton->setIcon(QIcon(iconPath));
}

void UIStateManager::setButtonImage(QPushButton* button, bool isRecording) {
    QPixmap pixmap;
    QSize buttonSize(40, 40);
    button->setFixedSize(buttonSize);

    if (button->objectName() == "takePhoto_Button") {
        pixmap = QPixmap(":/Resources/take_photo.png");
    } else if (button->objectName() == "takeVideo_Button") {
        pixmap = isRecording ? QPixmap(":/Resources/stop_video.png") : QPixmap(":/Resources/start_video.png");
    } else if (button->objectName() == "zoom_Button") {
        pixmap = QPixmap(":/Resources/zoom.png");
    }

    QPixmap scaledPixmap = pixmap.scaled(buttonSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    button->setIcon(QIcon(scaledPixmap));
    button->setIconSize(buttonSize);
}
