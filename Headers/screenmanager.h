#pragma once
#include <QVBoxLayout>
#include <MapScreen.h>
#include "CameraScreen.h"
#include "qlabel.h"
#include <GlobalParams.h>
class ScreenManager {
public:
    ScreenManager(QWidget* smallFrame, QWidget* mainFrame);
    void swapScreens();
    void adjustVisibility(QWidget* videoSettings, QWidget* hud, QWidget* compass, QLabel* warningLabel);

private:
    QVBoxLayout* frameLayout1;
    QVBoxLayout* frameLayout2;
    CameraScreen* cameraScreen;
    QWidget* smallFrame;
    QWidget* mainFrame;

    void setupLayouts();
};
