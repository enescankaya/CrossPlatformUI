#include "MapScreen.h"
#include "ui_MapScreen.h"
#include <QQuickItem>
#include <QVariant>
MapScreen::MapScreen(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ShowScrenn)
{
    ui->setupUi(this);
    ui->quickWidget->setSource(QUrl(QStringLiteral("qrc:/Views/qmlFiles/map.qml")));
    ui->quickWidget->show();
    qmlRootObject = ui->quickWidget->rootObject();
    connect(ui->Zoom_Out_Button, &QPushButton::clicked, this, [this]() {
        QMetaObject::invokeMethod(qmlRootObject, "zoomOut");
    });
    connect(ui->Zoom_In_Button, &QPushButton::clicked, this, [this]() {
        QMetaObject::invokeMethod(qmlRootObject, "zoomIn");
    });
    connect(ui->Default_Location_Button, &QPushButton::clicked, this, [this]() {
        QMetaObject::invokeMethod(qmlRootObject, "setDefaultLocation");
    });
}

MapScreen::~MapScreen()
{
    delete ui;
}

void MapScreen::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    ui->quickWidget->resize(event->size());
    QWidget::resizeEvent(event);
    ui->quickWidget->resize(event->size());

    if (ui->verticalLayoutWidget) {
        QWidget *layoutWidget = ui->verticalLayoutWidget;
        int parentWidth = this->width();
        int parentHeight = this->height();
        int widgetWidth = layoutWidget->width();
        int widgetHeight = layoutWidget->height();
        int x = parentWidth - widgetWidth;
        int y = (parentHeight - widgetHeight) / 2;
        layoutWidget->move(x, y);
    }
}

