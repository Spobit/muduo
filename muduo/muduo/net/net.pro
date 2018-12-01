QMAKE_CXXFLAGS += -std=c++0x
CONFIG += staticlib

TEMPLATE = lib
TARGET = muduo_net
INCLUDEPATH += ../..
DESTDIR = $$PWD/../../bin

DISTFILES += \
    net.pro.user \
    CMakeLists.txt \
    premake4.lua

HEADERS += \
    Acceptor.h \
    BoilerPlate.h \
    Buffer.h \
    Callbacks.h \
    Channel.h \
    Connector.h \
    Endian.h \
    EventLoop.h \
    EventLoopThread.h \
    EventLoopThreadPool.h \
    InetAddress.h \
    Poller.h \
    Socket.h \
    SocketsOps.h \
    TcpClient.h \
    TcpConnection.h \
    TcpServer.h \
    Timer.h \
    TimerId.h \
    TimerQueue.h \
    ZlibStream.h \
    poller/EPollPoller.h \
    poller/PollPoller.h

SOURCES += \
    Acceptor.cc \
    BoilerPlate.cc \
    Buffer.cc \
    Channel.cc \
    Connector.cc \
    EventLoop.cc \
    EventLoopThread.cc \
    EventLoopThreadPool.cc \
    InetAddress.cc \
    Poller.cc \
    Socket.cc \
    SocketsOps.cc \
    TcpClient.cc \
    TcpConnection.cc \
    TcpServer.cc \
    Timer.cc \
    TimerQueue.cc \
    poller/DefaultPoller.cc \
    poller/EPollPoller.cc \
    poller/PollPoller.cc
