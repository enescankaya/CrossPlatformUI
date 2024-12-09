#pragma once
#include <QQuickWidget>
#include <QList>
#include <QString>

class WidgetManager {
public:
    void addWidget(QQuickWidget* widget);
    void setupWidget(QQuickWidget* widget, const QString& sourceUrl);
    void closeAllExcept(QQuickWidget* widget);
    void setVisibility(QWidget* widget, QWidget* stackTarget, bool isVisible);

private:
    QList<QQuickWidget*> widgetList;
};
