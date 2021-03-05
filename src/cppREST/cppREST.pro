#c++11 and c++14 support
CONFIG += c++11 

#base settings
QT       -= gui
QT       += sql
QT       += xml xmlpatterns
QTPLUGIN += QSQLMYSQL
TEMPLATE = lib
TARGET = cppREST
DEFINES += CPPREST_LIBRARY

#enable O3 optimization
QMAKE_CXXFLAGS_RELEASE -= -O
QMAKE_CXXFLAGS_RELEASE -= -O1
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE *= -O3

#copy DLL to bin folder
DESTDIR = ../../bin/

#include cppCORE library
INCLUDEPATH += $$PWD/../cppCORE
LIBS += -L$$PWD/../../bin -lcppCORE

#include cppNGS library
INCLUDEPATH += $$PWD/../cppNGS
LIBS += -L$$PWD/../../bin -lcppNGS

#include cppNGSD library
INCLUDEPATH += $$PWD/../cppNGSD
LIBS += -L$$PWD/../../bin -lcppNGSD

#include cppXML library
INCLUDEPATH += $$PWD/../cppXML
LIBS += -L$$PWD/../../bin -lcppXML

#include htslib library
INCLUDEPATH += $$PWD/../../htslib/include/
LIBS += -L$$PWD/../../htslib/lib/ -lhts

#include zlib library
LIBS += -lz

#make the executable search for .so-files in the same folder under linux
QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN\'"

SOURCES += \
    EndpointHelper.cpp \
    EndpointManager.cpp \
    FileCache.cpp \
    HttpsServer.cpp \
    RequestHandler.cpp \
    ServerHelper.cpp \
    SessionManager.cpp \
    SslServer.cpp \
    WebEntity.cpp \
    WebExceptions.cpp \
    WorkerThread.cpp

HEADERS += \
    EndpointHelper.h \
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

RESOURCES += \
    cppREST.qrc

DISTFILES +=
