QT += core
QT += network
QT -= gui
QT += sql

QTPLUGIN += QSQLMYSQL

CONFIG += c++11 console
CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        EndpointHandler.cpp \
        EndpointManager.cpp \
        FileCache.cpp \
        HttpsServer.cpp \
        RequestHandler.cpp \
        ServerHelper.cpp \
        SessionManager.cpp \
        SslServer.cpp \
        WebEntity.cpp \
        WebExceptions.cpp \
        WorkerThread.cpp \
        main.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    EndpointHandler.h \
    EndpointManager.h \
    FileCache.h \
    HttpsServer.h \
    RequestHandler.h \
    ServerHelper.h \
    SessionManager.h \
    SslServer.h \
    WebEntity.h \
    WebExceptions.h \
    WorkerThread.h

include("../app_cli.pri")

#include NGSD library
INCLUDEPATH += $$PWD/../cppNGSD
LIBS += -L$$PWD/../bin -lcppNGSD

RESOURCES += \
    GSvarServer.qrc
