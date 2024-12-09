#include "Headers/AnimationManager.h"

AnimationManager::AnimationManager(QObject *parent)
    : QObject(parent), blinkAnimation(nullptr), isRecording(false) {
}

AnimationManager::~AnimationManager() {
    if (blinkAnimation) {
        delete blinkAnimation;
    }
}

void AnimationManager::setupBlinkAnimation(QLabel* recordingLabel) {
    QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect(recordingLabel);
    recordingLabel->setGraphicsEffect(effect);

    blinkAnimation = new QPropertyAnimation(effect, "opacity");
    blinkAnimation->setDuration(1000);
    blinkAnimation->setStartValue(1.0);
    blinkAnimation->setEndValue(0.0);
    blinkAnimation->setEasingCurve(QEasingCurve::InOutSine);
    blinkAnimation->setLoopCount(-1);
}

void AnimationManager::cleanupBlinkAnimation(QLabel* recordingLabel) {
    if (blinkAnimation) {
        blinkAnimation->stop();
        delete blinkAnimation;
        blinkAnimation = nullptr;
        recordingLabel->setGraphicsEffect(nullptr);
    }
}

void AnimationManager::startRecordingAnimation(QLabel* recordingLabel) {
    if (!blinkAnimation) {
        setupBlinkAnimation(recordingLabel);
    }
    blinkAnimation->start();
    recordingLabel->raise();
    recordingLabel->setVisible(true);
    isRecording = true;
}

void AnimationManager::stopRecordingAnimation(QLabel* recordingLabel) {
    cleanupBlinkAnimation(recordingLabel);
    recordingLabel->stackUnder(recordingLabel->parentWidget());
    recordingLabel->setVisible(false);
    isRecording = false;
}

void AnimationManager::createPhotoButtonAnimation(QPushButton* button) {
    QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect(button);
    button->setGraphicsEffect(effect);

    QPropertyAnimation *animation = new QPropertyAnimation(effect, "opacity");
    animation->setDuration(500);
    animation->setStartValue(1.0);
    animation->setKeyValueAt(0.5, 0.3);
    animation->setEndValue(1.0);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

bool AnimationManager::isRecordingAnimationActive() const {
    return isRecording;
}
