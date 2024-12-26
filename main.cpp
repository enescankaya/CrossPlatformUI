#include "maininterface.h"
#include <QApplication>
#include <QIcon>
#include <QPixmap>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainInterface w;
    // Simgeyi daha büyük bir boyutta ayarlama
    QPixmap iconPixmap(":/Resources/icon2.png");
    QIcon icon(iconPixmap.scaled(128, 128, Qt::KeepAspectRatio, Qt::SmoothTransformation)); // 128x128 olarak ölçeklendirme
    w.setWindowIcon(icon);
    w.setWindowFlags(Qt::FramelessWindowHint);
    w.show();
    return a.exec();
}
