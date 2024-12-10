#include "errormanager.h"
#include "qtconcurrentrun.h"
#include <QMutexLocker>
#include <Headers/GlobalParams.h>
ErrorManager::ErrorManager() {
    // Tüm paneller başlangıçta kullanılabilir olarak ayarlanır
    panelsAvailability.fill(true);
}

void ErrorManager::initialize(const std::array<std::function<void(QString, QString, QString, int)>, 3>& handlers) {
    // Hata sinyal işleyicilerini başlatır
    signalHandlers = handlers;
}

void ErrorManager::handleError(const QString& title, const QString& message, const QString& color, int duration) {
    QMutexLocker locker(&mutex); // Mutex kilitlenir
    // Gelen hatayı kuyruğa ekler
    errorQueue.push(std::make_tuple(title, message, color, duration));
    processQueue();  // Kuyruğu işlemeye çalışır
    locker.unlock(); // Kilit açılır

}

void ErrorManager::handleDismissal() {
    QMutexLocker locker(&mutex); // Mutex kilitlenir
    // Hata paneli serbest bırakıldığında çağrılır
    if (currentPanelIndex < panelsAvailability.size()) {
        panelsAvailability[currentPanelIndex] = true; // Paneli tekrar kullanılabilir yap
        currentPanelIndex = (currentPanelIndex + 1) % panelsAvailability.size(); // Sonraki paneli seç
    }
    locker.unlock(); // Kilit açılır
    processQueue();  // Kuyruğu işlemeye çalışır
}

void ErrorManager::processQueue() {
    // QtConcurrent ile arka planda kuyruk işlemesini gerçekleştirir
    QFuture<void> future = QtConcurrent::run([this]() {

        if (errorQueue.empty()) return; // Kuyruk boşsa çık
        // Tüm panellerin boş olup olmadığını kontrol et
        GlobalParams::getInstance().allPanelsAvailable = std::all_of(panelsAvailability.begin(), panelsAvailability.end(), [](bool available) {
            return available;
        });
        size_t checkedPanels = 0;
        size_t index = currentPanelIndex;

        // Tüm panelleri döngüsel olarak kontrol eder
        while (checkedPanels < panelsAvailability.size() && !errorQueue.empty()) {
            if (panelsAvailability[index]) { // Uygun bir panel bulunursa
                auto [title, message, color, duration] = errorQueue.front(); // Kuyruktan hata bilgisini al
                signalHandlers[index](title, message, color, duration);     // Sinyali tetikle
                panelsAvailability[index] = false;                         // Paneli meşgul olarak işaretle
                errorQueue.pop();                                          // Kuyruktan hata bilgisini çıkar
            }
            index = (index + 1) % panelsAvailability.size(); // Bir sonraki paneli seç
            checkedPanels++;
        }

        // Sıradaki panel indeksini güncelle
        currentPanelIndex = index;
    });
}
