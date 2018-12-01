// Copyright 2010, Shuo Chen.  All rights reserved.
// http://code.google.com/p/muduo/
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

// Author: Shuo Chen (chenshuo at chenshuo dot com)
//
// This is a public header file, it must only include public header files.

#ifndef MUDUO_NET_TCPSERVER_H
#define MUDUO_NET_TCPSERVER_H

#include <muduo/base/Atomic.h>
#include <muduo/base/Types.h>
#include <muduo/net/TcpConnection.h>

#include <map>

namespace muduo
{
namespace net
{

class Acceptor;
class EventLoop;
class EventLoopThreadPool;

///
/// TCP server, supports single-threaded and thread-pool models.
///
/// This is an interface class, so don't expose too much details.

/** class TcpServer
 * - Brief:
 *    TcpServer is the core of a service, it represent a service and bind the
 *    life cycle.
 * - Analysis:
 *    1) EventLoop object:
 *        poll, handle event.
 *    2) Acceptor object
 *        listen socket class.
 *    3) TcpConnection objects
 *        connection socket class.
 *    4) connectionCallback_ messageCallback_ writeCompleteCallback_
 *        Every TcpConnection object will invoke when evnet occured.
 *    5) EventLoopThreadPool
 */
class TcpServer : noncopyable
{
 public:
  typedef std::function<void(EventLoop*)> ThreadInitCallback;
  enum Option
  {
    kNoReusePort,
    kReusePort,
  };

  //TcpServer(EventLoop* loop, const InetAddress& listenAddr);
  TcpServer(EventLoop* loop,
            const InetAddress& listenAddr,
            const string& nameArg,
            Option option = kNoReusePort);
  ~TcpServer();  // force out-line dtor, for std::unique_ptr members.

  const string& ipPort() const { return ipPort_; }
  const string& name() const { return name_; }
  EventLoop* getLoop() const { return loop_; }

  /// Set the number of threads for handling input.
  ///
  /// Always accepts new connection in loop's thread.
  /// Must be called before @c start
  /// @param numThreads
  /// - 0 means all I/O in loop's thread, no thread will created.
  ///   this is the default value.
  /// - 1 means all I/O in another thread.
  /// - N means a thread pool with N threads, new connections
  ///   are assigned on a round-robin basis.
  void setThreadNum(int numThreads);
  void setThreadInitCallback(const ThreadInitCallback& cb)
  { threadInitCallback_ = cb; }
  /// valid after calling start()
  std::shared_ptr<EventLoopThreadPool> threadPool()
  { return threadPool_; }

  /// Starts the server if it's not listenning.
  ///
  /// It's harmless to call it multiple times.
  /// Thread safe.
  void start();

  /// Set connection callback.
  /// Not thread safe.
  void setConnectionCallback(const ConnectionCallback& cb)
  { connectionCallback_ = cb; }

  /// Set message callback.
  /// Not thread safe.
  void setMessageCallback(const MessageCallback& cb)
  { messageCallback_ = cb; }

  /// Set write complete callback.
  /// Not thread safe.
  void setWriteCompleteCallback(const WriteCompleteCallback& cb)
  { writeCompleteCallback_ = cb; }

 private:
  /// Not thread safe, but in loop
  void newConnection(int sockfd, const InetAddress& peerAddr);
  /// Thread safe.
  void removeConnection(const TcpConnectionPtr& conn);
  /// Not thread safe, but in loop
  void removeConnectionInLoop(const TcpConnectionPtr& conn);


  EventLoop* loop_;  // the acceptor loop
  ///> ip:port string of server.
  const string ipPort_;
  ///> server name string
  const string name_;
  ///> acceptor
  std::unique_ptr<Acceptor> acceptor_; // avoid revealing Acceptor
  ///> thread pool
  std::shared_ptr<EventLoopThreadPool> threadPool_;
  ///> TcpConnection object will invoke it. see TcpServer::newConnection().
  ///> callback will be invoked when Acceptor has a new socket connetion.
  ///>  1) EventLoop::loop()
  ///>  2) Channel::handleEvent()
  ///>  3) Channel::handleEventWithGuard()
  ///>  4) Acceptor::handleRead()
  ///>  5) void TcpServer::newConnection()
  ///>  6) void TcpConnection::connectEstablished()
  ///>  7) connectionCallback_()
  ConnectionCallback connectionCallback_;
  ///> TcpConnection object will invoke it. see TcpServer::newConnection().
  ///> callback will be invoked when TcpConnection recv a new message.
  ///>  1) EventLoop::loop()
  ///>  2) Channel::handleEvent()
  ///>  3) Channel::handleEventWithGuard()
  ///>  4) TcpConnection::handleRead()
  ///>  5) messageCallback_()
  MessageCallback messageCallback_;
  ///> TcpConnection object will invoke it. see TcpServer::newConnection().
  ///> callback will be invoked when TcpConnection completely send a new message.
  WriteCompleteCallback writeCompleteCallback_;
  ///> threads of threadPool_ object will invoke it. see TcpServer::start().
  ThreadInitCallback threadInitCallback_;
  ///> if server is started. see TcpServer::start().
  AtomicInt32 started_;
  // always in loop thread
  ///> next connection id, it`s as part of thread name string.
  int nextConnId_;
  typedef std::map<string, TcpConnectionPtr> ConnectionMap;
  ///> TcpConnection class map
  ConnectionMap connections_;
};

}  // namespace net
}  // namespace muduo

#endif  // MUDUO_NET_TCPSERVER_H
