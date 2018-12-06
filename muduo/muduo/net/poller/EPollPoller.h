// Copyright 2010, Shuo Chen.  All rights reserved.
// http://code.google.com/p/muduo/
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

// Author: Shuo Chen (chenshuo at chenshuo dot com)
//
// This is an internal header file, you should not include this.

#ifndef MUDUO_NET_POLLER_EPOLLPOLLER_H
#define MUDUO_NET_POLLER_EPOLLPOLLER_H

#include <muduo/net/Poller.h>

#include <vector>

struct epoll_event;

namespace muduo
{
namespace net
{

///
/// IO Multiplexing with epoll(4).
///
class EPollPoller : public Poller
{
private:
  int epollfd_; // socketfd
  typedef std::vector<struct epoll_event> EventList;
  static const int kInitEventListSize = 16;
  EventList events_;

public:
  EPollPoller(EventLoop* loop);
  ~EPollPoller() override;

  Timestamp poll(int timeoutMs, ChannelList* activeChannels) override;
  void updateChannel(Channel* channel) override;
  void removeChannel(Channel* channel) override;

private:
  ///> get channel pointer from @a struct epoll_event object
  void fillActiveChannels(int numEvents, ChannelList* activeChannels) const;
  ///> add del mod
  void update(int operation, Channel* channel);
  ///> print operation,  add or del or mod or unknown?
  static const char* operationToString(int op);
};

}  // namespace net
}  // namespace muduo
#endif  // MUDUO_NET_POLLER_EPOLLPOLLER_H
