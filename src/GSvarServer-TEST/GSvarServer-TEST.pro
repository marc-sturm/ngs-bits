#c++11 support
CONFIG += c++11

#base settings
QT += core
QT += network
QT -= gui
QT += sql

QTPLUGIN += QSQLMYSQL

CONFIG   += console
CONFIG   -= app_bundle
TEMPLATE = app
DESTDIR = ../../bin/

include("../app_cli.pri")

#include NGSD library
INCLUDEPATH += $$PWD/../cppNGSD
LIBS += -L$$PWD/../bin -lcppNGSD

#the server itself
INCLUDEPATH += $$PWD/../GSvarServer

#include cppTFW library
INCLUDEPATH += $$PWD/../cppTFW

HEADERS += \
    Helper-Test.h \
    SessionManager-Test.h \
    WebEntity-Test.h

SOURCES += \
        main.cpp
