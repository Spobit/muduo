// Copyright 2010, Shuo Chen.  All rights reserved.
// http://code.google.com/p/muduo/
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

// Author: Shuo Chen (chenshuo at chenshuo dot com)
//
// This is an internal header file, you should not include this.

#ifndef MUDUO_NET_CONNECTOR_H
#define MUDUO_NET_CONNECTOR_H

#include <muduo/base/noncopyable.h>
#include <muduo/net/InetAddress.h>

#include <functional>
#include <memory>

namespace muduo
{
namespace net
{

class Channel;
class EventLoop;

/** class Connector
 * - Brief:
 *    Connector is used when is connecting, if connection is complete, it is not
 *    useful. it created and managed by TcpClient.
 *
 * - Think this:
 *    Connector is used when is connecting, represent socketfd, has callback that it need.
 *    TcpConnection is used atfer connected, represent socketfd, has anothor callback it need.
 */
class Connector : noncopyable,
    public std::enable_shared_from_this<Connector>
{
public:
  typedef std::function<void (int sockfd)> NewConnectionCallback;

private:
  enum States { kDisconnected, kConnecting, kConnected };
  EventLoop* loop_;
  InetAddress serverAddr_;
  bool connect_; // atomic
  ///> connection status.
  States state_;  // FIXME: use atomic variable
  ///> see connecting().
  std::unique_ptr<Channel> channel_;
  ///> see start() --> startInLoop() --> connect() --> connecting() -->
  ///  handleWrite() --> newConnectionCallback_()
  NewConnectionCallback newConnectionCallback_;
  ///> micro seconds of retry connection.
  int retryDelayMs_;

public:
  Connector(EventLoop* loop, const InetAddress& serverAddr);
  ~Connector();

  void setNewConnectionCallback(const NewConnectionCallback& cb)
  { newConnectionCallback_ = cb; }

  void start();  // can be called in any thread
  void restart();  // must be called in loop thread
  void stop();  // can be called in any thread

  const InetAddress& serverAddress() const { return serverAddr_; }

private:
  static const int kMaxRetryDelayMs = 30*1000;
  static const int kInitRetryDelayMs = 500;

  void setState(States s) { state_ = s; }
  void startInLoop();
  void stopInLoop();
  void connect();
  void connecting(int sockfd);
  void handleWrite();
  void handleError();
  void retry(int sockfd);
  int removeAndResetChannel();
  void resetChannel();
};

}  // namespace net
}  // namespace muduo

#endif  // MUDUO_NET_CONNECTOR_H
