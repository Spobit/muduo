QT += core
QT -= gui

CONFIG += console
CONFIG -= app_bundle
QMAKE_CXXFLAGS += -std=c++0x

TEMPLATE = app
TARGET = echo_server
INCLUDEPATH += ../../../.. \
    $$PWD/../../../../bin
DESTDIR = $$PWD/../../../../bin

LIBS += -lmuduo_base -lmuduo_net -L$$PWD/../../../../bin

HEADERS += echo.h

SOURCES += echo.cc main.cc
