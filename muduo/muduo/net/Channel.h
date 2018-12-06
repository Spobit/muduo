// Copyright 2010, Shuo Chen.  All rights reserved.
// http://code.google.com/p/muduo/
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

// Author: Shuo Chen (chenshuo at chenshuo dot com)
//
// This is an internal header file, you should not include this.

#ifndef MUDUO_NET_CHANNEL_H
#define MUDUO_NET_CHANNEL_H

#include <muduo/base/noncopyable.h>
#include <muduo/base/Timestamp.h>

#include <functional>
#include <memory>

namespace muduo
{
namespace net
{

class EventLoop;

///
/// A selectable I/O channel.
///
/// This class doesn't own the file descriptor.
/// The file descriptor could be a socket,
/// an eventfd, a timerfd, or a signalfd

/** class Channel
 * - Brief:
 *    Create by TcpConnection.
 *
 *    This class is a important:
 *    1) every Channel class manage a socketfd.
 *    2) can update and delete socketfd evnets flags from epoll_wait().
 *    3) hold every socketfd`s event handlers that it care.
 *    4) hols every socketfd`s event handlers that it was occured.
 *    5) Channel object will be known when epoll_wait() return,
 *       see EPollPoller::update(). when we get Channel object,
 *       then invoke handlers.
 */
class Channel : noncopyable
{
public:
  typedef std::function<void()> EventCallback;
  typedef std::function<void(Timestamp)> ReadEventCallback;

private:
  static const int kNoneEvent/* = 0*/;
  static const int kReadEvent/* = POLLIN | POLLPRI*/;
  static const int kWriteEvent/* = POLLOUT*/;

  ///> which EventLoop object hold the object.
  EventLoop* loop_;
  ///> socketfd
  const int fd_;
  ///> event flags which socketfd care
  int events_;
  ///> event flags returnd which socketfd events occur.
  int revents_; // it's the received event types of epoll or poll
  ///> index_ is used as record socketfd status.
  /// const int kNew = -1;      will be add into epoll_wait()
  /// const int kAdded = 1;     has been added into epoll_wait()
  /// const int kDeleted = 2;   has been deleted in epoll_wait()
  int index_; // used by Poller.
  ///> ??? log POLLHUP event.
  bool logHup_;

  ///> Think from point of server:
  ///   invoke at TcpConnection::connectEstablished(), tie_ holds the
  ///   TcpConnection object who create the Channel objects.
  std::weak_ptr<void> tie_;
  ///> true when tie_ holds TcpConnection object.
  bool tied_;
  ///> handle event critital section flag.
  bool eventHandling_;
  ///> if socketfd is added to epoll for monitor evnets.
  bool addedToLoop_;

  ///> Aligned by TcpConnection::handleRead, see TcpConnection::TcpConnection().
  ReadEventCallback readCallback_;
  ///> Aligned by TcpConnection::handleWrite, see TcpConnection::TcpConnection().
  EventCallback writeCallback_;
  ///> Aligned by TcpConnection::handleClose, see TcpConnection::TcpConnection().
  EventCallback closeCallback_;
  ///> Aligned by TcpConnection::handleError, see TcpConnection::TcpConnection().
  EventCallback errorCallback_;

public:
  Channel(EventLoop* loop, int fd);
  ~Channel();

  void handleEvent(Timestamp receiveTime);
  void setReadCallback(ReadEventCallback cb)
  { readCallback_ = std::move(cb); }
  void setWriteCallback(EventCallback cb)
  { writeCallback_ = std::move(cb); }
  void setCloseCallback(EventCallback cb)
  { closeCallback_ = std::move(cb); }
  void setErrorCallback(EventCallback cb)
  { errorCallback_ = std::move(cb); }

  /// Tie this channel to the owner object managed by shared_ptr,
  /// prevent the owner object being destroyed in handleEvent.
  void tie(const std::shared_ptr<void>& obj);

  int fd() const { return fd_; }
  int events() const { return events_; }
  void set_revents(int revt) { revents_ = revt; } // used by pollers

  void enableReading() { events_ |= kReadEvent; update(); }
  void disableReading() { events_ &= ~kReadEvent; update(); }
  void enableWriting() { events_ |= kWriteEvent; update(); }
  void disableWriting() { events_ &= ~kWriteEvent; update(); }
  void disableAll() { events_ = kNoneEvent; update(); }
  ///> if socketfd is monitored ???
  bool isWriting() const { return events_ & kWriteEvent; }
  bool isReading() const { return events_ & kReadEvent; }

  // for Poller
  int index() { return index_; }
  void set_index(int idx) { index_ = idx; }

  // for debug
  string reventsToString() const;
  string eventsToString() const;

  void doNotLogHup() { logHup_ = false; }

  EventLoop* ownerLoop() { return loop_; }
  ///> del socketfd monitor from epoll_wait().
  void remove();
  bool isNoneEvent() const { return events_ == kNoneEvent; }


private:
  static string eventsToString(int fd, int ev);

  ///> add or mod socketfd event flags.
  void update();
  void handleEventWithGuard(Timestamp receiveTime);
};

}  // namespace net
}  // namespace muduo

#endif  // MUDUO_NET_CHANNEL_H
