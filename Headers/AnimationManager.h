#ifndef ANIMATIONMANAGER_H
#define ANIMATIONMANAGER_H

#include <QObject>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QPushButton>
#include <QLabel>

class AnimationManager : public QObject {
    Q_OBJECT

public:
    explicit AnimationManager(QObject *parent = nullptr);
    ~AnimationManager();

    void startRecordingAnimation(QLabel* recordingLabel);
    void stopRecordingAnimation(QLabel* recordingLabel);
    void createPhotoButtonAnimation(QPushButton* button);
    bool isRecordingAnimationActive() const;

private:
    QPropertyAnimation* blinkAnimation;
    bool isRecording;

    void setupBlinkAnimation(QLabel* recordingLabel);
    void cleanupBlinkAnimation(QLabel* recordingLabel);
};
#endif // ANIMATIONMANAGER_H
