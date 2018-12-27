QT += core
QT -= gui

CONFIG += console
CONFIG -= app_bundle
QMAKE_CXXFLAGS += -std=c++0x

TEMPLATE = mytest
TARGET = asio_server
DESTDIR = $$PWD/../../bin
INCLUDEPATH += $$PWD/../..

LIBS += -lmuduo_base -lmuduo_net -L$$PWD/../../bin

TEMPLATE = app

SOURCES += main.cpp

