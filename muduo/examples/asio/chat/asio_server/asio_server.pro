QT += core
QT -= gui

CONFIG += console
CONFIG -= app_bundle
QMAKE_CXXFLAGS += -std=c++0x

TEMPLATE = app
TARGET = asio_server
DESTDIR = $$PWD/../../../../bin
INCLUDEPATH += $$PWD/../../../..

LIBS += -lmuduo_base -lmuduo_net -L$$PWD/../../../../bin

HEADERS += codec.h

SOURCES += server.cc



