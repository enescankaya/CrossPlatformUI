
#include "CameraScreen.h"
#include <QMediaPlayer>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsVideoItem>
#include <QPushButton>
#include <QVBoxLayout>
#include <QResizeEvent>  // Bu başlık dosyasını ekle
CameraScreen::CameraScreen(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::showscreen2)
    , player(new QMediaPlayer(this))
    , graphicsScene(new QGraphicsScene(this))
    , videoItem(new QGraphicsVideoItem())
{
    ui->setupUi(this);

    // QGraphicsView ayarları
    ui->graphicsView->setScene(graphicsScene);

    // Video öğesi ve player bağlantısı
    graphicsScene->addItem(videoItem);
    player->setVideoOutput(videoItem);

    // Video dosyasını yükleyin
    player->setSource(QUrl(QStringLiteral("qrc:/Resources/video.mp4")));

    // Videoyu döngüye al ve başlat
    connect(player, &QMediaPlayer::mediaStatusChanged, this, [=](QMediaPlayer::MediaStatus status) {
        if (status == QMediaPlayer::EndOfMedia)
            player->play();
    });
    player->play();
}

// Pencere boyutu değiştiğinde grafik view boyutunu ayarla
void CameraScreen::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    ui->graphicsView->resize(event->size()); // graphicsView boyutunu pencere boyutuna göre ayarlayın
    videoItem->setSize(event->size()); // videoItem'ın boyutunu da güncelleyin
}

CameraScreen::~CameraScreen()
{
    delete ui;
    delete player;
    delete graphicsScene;
    delete videoItem;

}

