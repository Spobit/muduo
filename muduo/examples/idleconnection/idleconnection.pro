QT += core
QT -= gui

CONFIG += console
CONFIG -= app_bundle
QMAKE_CXXFLAGS += -std=c++0x

TEMPLATE = app
TARGET = idle_connection
DESTDIR = $$PWD/../../bin

LIBS += -lmuduo_base -lmuduo_net -L$$PWD/../../bin

SOURCES += sortedlist.cc
