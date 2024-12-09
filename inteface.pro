QT       += core gui
QT       += core gui svg
QT       += core gui network
QT       += core
QT += svg
QT += svgwidgets
QT += core gui concurrent
QT += widgets quickwidgets
QT += widgets
QT += quick
QT += core gui multimedia multimediawidgets
 QT += multimediawidgets
 QT += multimedia multimediawidgets
 QT += quick quickcontrols2
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Delegate/AnimationManager.cpp \
    Delegate/CameraScreen.cpp \
    Models/GlobalParams.cpp \
    Delegate/MapScreen.cpp \
    Delegate/connectionmanager.cpp \
    Models/TcpManager.cpp \
    Models/TcpWorker.cpp \
    Models/errormanager.cpp \
    Delegate/screenmanager.cpp \
    Delegate/uistatemanager.cpp \
    Delegate/widgetmanager.cpp \
    main.cpp \
    Delegate/maininterface.cpp\

HEADERS += \
    Headers/AnimationManager.h \
    Headers/CameraScreen.h \
    Headers/GlobalParams.h \
    Headers/MapScreen.h \
    Headers/TcpManager.h \
    Headers/TcpWorker.h \
    Headers/maininterface.h \
    Headers/errormanager.h \
    Headers/screenmanager.h \
    Headers/uistatemanager.h \
    Headers/widgetmanager.h \
    Headers/connectionmanager.h \

FORMS += \
    Views/CameraScreen.ui \
    Views/MapScreen.ui \
    Views/maininterface.ui

INCLUDEPATH += $$PWD/Headers

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Resource.qrc
