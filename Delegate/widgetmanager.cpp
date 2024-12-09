#include "Headers/widgetmanager.h"

void WidgetManager::addWidget(QQuickWidget* widget) {
    widgetList.append(widget);
}

void WidgetManager::setupWidget(QQuickWidget* widget, const QString& sourceUrl) {
    widget->setResizeMode(QQuickWidget::SizeRootObjectToView);
    widget->setAttribute(Qt::WA_AlwaysStackOnTop);
    widget->setClearColor(Qt::transparent);
    widget->setSource(QUrl(QStringLiteral("qrc:/Views/qmlFiles/") + sourceUrl));
    widget->show();
}

void WidgetManager::closeAllExcept(QQuickWidget* widget) {
    for (auto w : widgetList) {
        if (w == widget) {
            w->setVisible(true);
            w->raise();
        } else {
            w->stackUnder(w->parentWidget());
            w->setVisible(false);
        }
    }
}

void WidgetManager::setVisibility(QWidget* widget, QWidget* stackTarget, bool isVisible) {
    if (isVisible) {
        widget->raise();
        widget->setVisible(true);
    } else {
        widget->stackUnder(stackTarget);
        widget->setVisible(false);
    }
}
