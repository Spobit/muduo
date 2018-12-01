QT += core
QT -= gui

CONFIG += console
CONFIG -= app_bundle
QMAKE_CXXFLAGS += -std=c++0x

TEMPLATE = app
TARGET = simple_discard
INCLUDEPATH += ../../.. \
    $$PWD/../../../bin
DESTDIR = $$PWD/../../../bin

LIBS += -lmuduo_base -lmuduo_net -L$$PWD/../../../bin

HEADERS += discard.h

SOURCES += discard.cc main.cc


