#pragma once

#include "qmutex.h"
#include <QString>
#include <array>
#include <queue>
#include <functional>


class ErrorManager {

public:

    ErrorManager();

    void initialize(const std::array<std::function<void(QString, QString, QString, int)>, 3>& handlers);

    void handleError(const QString& title, const QString& message, const QString& color, int duration);

    void handleDismissal();


private:

    std::array<bool, 3> panelsAvailability;

    std::array<std::function<void(QString, QString, QString, int)>, 3> signalHandlers;

    std::queue<std::tuple<QString, QString, QString, int>> errorQueue;

    size_t currentPanelIndex = 0;

    QMutex mutex;
    void processQueue();

};
