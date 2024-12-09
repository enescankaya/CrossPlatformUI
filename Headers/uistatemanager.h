#pragma once
#include <QString>
#include <QPushButton>

class UIStateManager {
public:
    UIStateManager();
    void handleModeChange(int modeIndex, QWidget* throttleWidget,
                          QWidget* altitudeWidget, QWidget* mainFrame);
    void handleEngineState(bool isEngineOn, QPushButton* motorButton);
    void handleSecurityState(bool isArmed, QPushButton* armButton);
    void handleConnectionState(bool state, QPushButton* tcpButton);
    void setButtonImage(QPushButton* button, bool isRecording = false);

private:
    bool engineOn = false;
    bool armed = false;
};
