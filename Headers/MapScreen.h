#ifndef MAPSCREEN_H
#define MAPSCREEN_H

#include "ui_MapScreen.h"
#include <QWidget>

namespace Ui {
class MapScreen;
}

class MapScreen : public QWidget
{
    Q_OBJECT

public:
    explicit MapScreen(QWidget *parent = nullptr);
    ~MapScreen();
    QObject *qmlRootObject;
private slots:

private:
    Ui::ShowScrenn *ui;
    void resizeEvent(QResizeEvent *event);
};

#endif // MAPSCREEN_H
