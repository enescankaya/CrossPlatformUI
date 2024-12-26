#include "MapScreen.h"
#include "ui_MapScreen.h"
#include <QQuickItem>
#include <QVariant>
#include <GlobalParams.h>
#include <QQmlContext>  // Add this for QML context
#include <QDir>

MapScreen::MapScreen(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ShowScrenn)
{
    // Performance optimizations
    qputenv("QML_DISK_CACHE_ENABLE", "true");

    // Set up paths before loading QML
    QString appPath = QCoreApplication::applicationDirPath();
    QString tilesPath = appPath + "/tiles";
    QString cachePath = appPath + "/cache";

    // Ensure directories exist
    QDir().mkpath(tilesPath);
    QDir().mkpath(cachePath);

    GlobalParams::getInstance().setTilePath(tilesPath);

    // Set up UI
    ui->setupUi(this);

    // Set initial context properties for QML
    ui->quickWidget->rootContext()->setContextProperty("initialTilePath", tilesPath);
    ui->quickWidget->rootContext()->setContextProperty("initialCachePath", cachePath);

    // Configure QQuickWidget
    ui->quickWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);
    ui->quickWidget->setSource(QUrl(QStringLiteral("qrc:/Views/qmlFiles/map.qml")));
    ui->quickWidget->show();

    qmlRootObject = ui->quickWidget->rootObject();

    // Set up button connections
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
