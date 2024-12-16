#include "Headers/screenmanager.h"

ScreenManager::ScreenManager(QWidget* smallFrame, QWidget* mainFrame)
    : smallFrame(smallFrame), mainFrame(mainFrame) {

    GlobalParams::getInstance().setMapScreen(new MapScreen(mainFrame));
    cameraScreen = new CameraScreen(smallFrame);
    setupLayouts();
}

void ScreenManager::setupLayouts() {
    frameLayout1 = new QVBoxLayout(smallFrame);
    frameLayout2 = new QVBoxLayout(mainFrame);
    frameLayout1->addWidget(cameraScreen);
    frameLayout2->addWidget(GlobalParams::getInstance().getMapScreen());
}

void ScreenManager::swapScreens() {
    QWidget* widget1 = frameLayout1->itemAt(0)->widget();
    QWidget* widget2 = frameLayout2->itemAt(0)->widget();

    frameLayout1->removeWidget(widget1);
    frameLayout2->removeWidget(widget2);

    frameLayout2->addWidget(widget1);
    frameLayout1->addWidget(widget2);
}

void ScreenManager::adjustVisibility(QWidget* videoSettings, QWidget* hud, QWidget* compass) {
    if (frameLayout2->itemAt(0)->widget() == cameraScreen) {
        videoSettings->raise();
        hud->show();
        compass->show();
    } else {
        videoSettings->stackUnder(smallFrame);
        hud->hide();
        compass->hide();
    }
}
