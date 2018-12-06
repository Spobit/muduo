// Copyright 2010, Shuo Chen.  All rights reserved.
// http://code.google.com/p/muduo/
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

// Author: Shuo Chen (chenshuo at chenshuo dot com)
//
// This is a public header file, it must only include public header files.

#ifndef MUDUO_NET_EVENTLOOP_H
#define MUDUO_NET_EVENTLOOP_H

#include <atomic>
#include <functional>
#include <vector>

#include <boost/any.hpp>

#include <muduo/base/Mutex.h>
#include <muduo/base/CurrentThread.h>
#include <muduo/base/Timestamp.h>
#include <muduo/net/Callbacks.h>
#include <muduo/net/TimerId.h>

namespace muduo
{
namespace net
{

class Channel;
class Poller;
class TimerQueue;

///
/// Reactor, at most one per thread.
///
/// This is an interface class, so don't expose too much details.

/** eventfd:
 * - int eventfd(unsigned int initval, int flags);
 *   - flags:
 *     - EFD_CLOEXEC
 *     - EFD_NONBLOCK
 *     - EFD_SEMAPHORE
 * - ssize_t read(int fd, void *buf, size_t count);
 * - ssize_t write(int fd, const void *buf, size_t count);
 * - int close(int fd);
 */

/** class EventLoop
 * - Brief:
 *    EventLoop indirectly manage socketfd & eventfd & timerfd, and it has
 *    poll function.
 *    so, to be understand it.
 *    pendingFunctors_ and runInLoop (eventfd evnets) make it can do anyway
 *    task we specify, and keep run in thread of owner self.
 *    so, EventLoop can handle
 *      1) socketfd evnets (poll notify, channel-read invoke, do tasks )
 *      2) eventfd evnets  (poll notify, channel-read invoke, do tasks in pendingFunctors_)
 *      3) timerfd evnets  (poll notify, channel-read invoke, do exparition tasks )
 *
 *    Channel is also a important class. because of it can determine owner
 *    indirectly (socket evnet is belong who), and then dispath these events.
 */

class EventLoop : noncopyable
{
public:
  typedef std::function<void()> Functor;
private:
  typedef std::vector<Channel*> ChannelList;

private:
  bool looping_; /* atomic */ ///< if self start event loop.
  std::atomic<bool> quit_; ///< if self will exit event loop.
  bool eventHandling_; /* atomic */ ///< if it will handle event
  ///> if it will call doPendingFunctors().
  bool callingPendingFunctors_; /* atomic */
  int64_t iteration_; ///< loop counter of loop()`s while content
  const pid_t threadId_; ///<  id of thread that EventLoop object is birth.
  ///> the timestamp of ::epoll_wait() returned.
  Timestamp pollReturnTime_;
  std::unique_ptr<Poller> poller_;

  std::unique_ptr<TimerQueue> timerQueue_;

  ///> channels actived by epoll, then will call suitable handler.
  ChannelList activeChannels_;
  Channel* currentActiveChannel_; ///< channel who is handling event.

  ///> wakeupFd_ and pendingFunctors_ are combo to do task, they ensure
  ///  doing task in object thread (IO thread).
  int wakeupFd_; ///< eventfd file descriptor.
  std::unique_ptr<Channel> wakeupChannel_; ///< eventfd chanel.
  ///> pend event function pointers. see EventLoop::runInLoop().
  ///> these tasks will be invoke in the thead of owner object. do not care
  ///  invoke runInLoop() in which thread.
  std::vector<Functor> pendingFunctors_ /*GUARDED_BY(mutex_)*/;

  boost::any context_; ///> custom data.

  mutable MutexLock mutex_;

public:
  EventLoop();
  ~EventLoop();  // force out-line dtor, for std::unique_ptr members.

  ///
  /// Loops forever.
  ///
  /// Must be called in the same thread as creation of the object.
  ///
  void loop();

  /// Quits loop.
  ///
  /// This is not 100% thread safe, if you call through a raw pointer,
  /// better to call through shared_ptr<EventLoop> for 100% safety.
  void quit();

  ///
  /// Time when poll returns, usually means data arrival.
  ///
  Timestamp pollReturnTime() const { return pollReturnTime_; }

  int64_t iteration() const { return iteration_; }

  /// Runs callback immediately in the loop thread.
  /// It wakes up the loop, and run the cb.
  /// If in the same loop thread, cb is run within the function.
  /// Safe to call from other threads.
  void runInLoop(Functor cb);
  /// Queues callback in the loop thread.
  /// Runs after finish pooling.
  /// Safe to call from other threads.
  void queueInLoop(Functor cb);

  size_t queueSize() const;

  // timers

  ///
  /// Runs callback at 'time'.
  /// Safe to call from other threads.
  ///
  TimerId runAt(Timestamp time, TimerCallback cb);
  ///
  /// Runs callback after @c delay seconds.
  /// Safe to call from other threads.
  ///
  TimerId runAfter(double delay, TimerCallback cb);
  ///
  /// Runs callback every @c interval seconds.
  /// Safe to call from other threads.
  ///
  TimerId runEvery(double interval, TimerCallback cb);
  ///
  /// Cancels the timer.
  /// Safe to call from other threads.
  ///
  void cancel(TimerId timerId);

  // internal usage
  ///> activate eventfd, let ::epoll_wait() retruned, then continua
  ///> Eventloop::loop() code.
  void wakeup();
  void updateChannel(Channel* channel); ///< EPollPoller::updateChannel()
  void removeChannel(Channel* channel); ///< EPollPoller::removeChannel()
  bool hasChannel(Channel* channel); ///< Poller::hasChannel()

  // pid_t threadId() const { return threadId_; }
  ///> birth thread and loop thread must be same one.
  void assertInLoopThread()
  {
    if (!isInLoopThread()) { abortNotInLoopThread(); }
  }
  bool isInLoopThread() const { return threadId_ == CurrentThread::tid(); }
  // bool callingPendingFunctors() const { return callingPendingFunctors_; }
  ///< if it is in handling event critical section.
  bool eventHandling() const { return eventHandling_; }

  void setContext(const boost::any& context)
  { context_ = context; }

  const boost::any& getContext() const
  { return context_; }

  boost::any* getMutableContext()
  { return &context_; }

  ///< one loop one thread
  static EventLoop* getEventLoopOfCurrentThread();

private:
  void abortNotInLoopThread();
  void handleRead();  /* waked up */ ///< eventfd waked up
  void doPendingFunctors();

  void printActiveChannels() const; // DEBUG
};

}  // namespace net
}  // namespace muduo

#endif  // MUDUO_NET_EVENTLOOP_H
