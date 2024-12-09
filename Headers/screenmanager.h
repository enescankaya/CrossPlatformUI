#pragma once
#include <QVBoxLayout>
#include "MapScreen.h"
#include "CameraScreen.h"

class ScreenManager {
public:
    ScreenManager(QWidget* smallFrame, QWidget* mainFrame);
    void swapScreens();
    void adjustVisibility(QWidget* videoSettings, QWidget* hud, QWidget* compass);

private:
    QVBoxLayout* frameLayout1;
    QVBoxLayout* frameLayout2;
    MapScreen* mapScreen;
    CameraScreen* cameraScreen;
    QWidget* smallFrame;
    QWidget* mainFrame;

    void setupLayouts();
};
