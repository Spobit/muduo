QT += core
QT -= gui

CONFIG += console
CONFIG -= app_bundle
QMAKE_CXXFLAGS += -std=c++0x

TEMPLATE = app
TARGET = pingpong_client
DESTDIR = $$PWD/../../../bin
INCLUDEPATH += $$PWD/../../..

LIBS += -lmuduo_base -lmuduo_net -L$$PWD/../../../bin

SOURCES += client.cc
