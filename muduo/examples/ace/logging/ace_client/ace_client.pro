QT += core
QT -= gui

CONFIG += console
CONFIG -= app_bundle
QMAKE_CXXFLAGS += -std=c++0x

TEMPLATE = app
TARGET = ace_client
INCLUDEPATH += ../../../..
DESTDIR = $$PWD/../../../../bin

LIBS += -lmuduo_base -lmuduo_net -L$$PWD/../../../../bin

SOURCES += client.cc



