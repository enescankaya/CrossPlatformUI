#ifndef CAMERASCREEN_H
#define CAMERASCREEN_H
#include "ui_CameraScreen.h"
#include <QWidget>
#include <QMediaPlayer>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsVideoItem>

namespace Ui {
class CameraScreen;
}

class CameraScreen : public QWidget
{
    Q_OBJECT

public:
    explicit CameraScreen(QWidget *parent = nullptr);
    ~CameraScreen();


private:
    Ui::showscreen2 *ui;
    QMediaPlayer *player;
    QGraphicsScene *graphicsScene;
    QGraphicsVideoItem *videoItem;
    void resizeEvent(QResizeEvent *event);
};

#endif // CAMERASCREEN_H
