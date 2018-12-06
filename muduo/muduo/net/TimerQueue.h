// Copyright 2010, Shuo Chen.  All rights reserved.
// http://code.google.com/p/muduo/
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

// Author: Shuo Chen (chenshuo at chenshuo dot com)
//
// This is an internal header file, you should not include this.

#ifndef MUDUO_NET_TIMERQUEUE_H
#define MUDUO_NET_TIMERQUEUE_H

#include <set>
#include <vector>

#include <muduo/base/Mutex.h>
#include <muduo/base/Timestamp.h>
#include <muduo/net/Callbacks.h>
#include <muduo/net/Channel.h>

namespace muduo
{
namespace net
{

class EventLoop;
class Timer;
class TimerId;

///
/// A best efforts timer queue.
/// No guarantee that the callback will be on time.

/** timerfd
 * - int timerfd_create(int clockid, int flags);
 *    - clockid:
 *      - CLOCK_REALTIME
 *        系统实时时间,随系统实时时间改变而改变,即从UTC1970-1-1 0:0:0开始计时,
 *        中间时刻如果系统时间被用户改成其他,则对应的时间相应改变.
 *      - CLOCK_MONOTONIC
 *        从系统启动这一刻起开始计时,不受系统时间被用户改变的影响.
 *    - flags:
 *      - TFD_NONBLOCK
 *      - TFD_CLOEXEC
 *        子进程不会得到该fd.
 *    - return:
 *      timer file descriptor.
 * - int timerfd_gettime(int fd, struct itimerspec *curr_value);
 * - int timerfd_settime(int fd, int flags,
 *                       const struct itimerspec *new_value,
 *                       struct itimerspec *old_value);
 *
 *          struct timespec {
 *              time_t tv_sec;                // Seconds
 *              long   tv_nsec;               // Nanoseconds
 *          };
 *          struct itimerspec {
 *              struct timespec it_interval;  // Interval for periodic timer
 *              struct timespec it_value;     // Initial expiration
 *          };
 *
 *    - flags:
 *      0: relative timer.
 *      TFD_TIMER_ABSTIME: absolute timer
 *    - new_value:
 *      new_value.it_value specifies the initial expiration of the timer,
 *      in seconds and nanoseconds:
 *        1) Setting either field of new_value.it_value to a nonzero value
 *           arms the timer.
 *        2) Setting both fields of new_value.it_value to zero disarms the timer.
 *        3) If both fields of new_value.it_interval are zero, the timer expires
 *           just once, at the time specified by new_value.it_value.
 *    - old_value:
 *      If the old_value argument is not NULL, then the itimerspec structure
 *      that it points to is used to return the setting of the timer that was
 *      current at the time of the call;
 */

/** class TimerQueue
 * - Brief:
 *    TimerQueue manage a timerfd, so, in fact, it is manager of timed task.
 *    1) timerfd_ make it can set careful events flags.
 *    2) timerfdChannel_ make it can invoke handleRead(), then invoke task.
 *    3) timers_ and activeTimers_ are valid timers, cancelingTimers_ is
 *       canceled timers.
 */
class TimerQueue : noncopyable
{
private:
  // FIXME: use unique_ptr<Timer> instead of raw pointers.
  // This requires heterogeneous comparison lookup (N3465) from C++14
  // so that we can find an T* in a set<unique_ptr<T>>.
  // expiration
  typedef std::pair<Timestamp, Timer*> Entry;
  typedef std::set<Entry> TimerList;
  // activation
  typedef std::pair<Timer*, int64_t> ActiveTimer;
  typedef std::set<ActiveTimer> ActiveTimerSet;

  EventLoop* loop_;
  const int timerfd_;
  Channel timerfdChannel_;

  ///> do task critical section.
  bool callingExpiredTimers_; /* atomic */
  // Timer list sorted by expiration
  ///> ?????? active timer set, pair data is diff of activeTimers_.
  TimerList timers_;
  ///> active timer set
  ActiveTimerSet activeTimers_;
  ///> cancel timer set
  ActiveTimerSet cancelingTimers_;

public:
  explicit TimerQueue(EventLoop* loop);
  ~TimerQueue();

  ///
  /// Schedules the callback to be run at given time,
  /// repeats if @c interval > 0.0.
  ///
  /// Must be thread safe. Usually be called from other threads.
  ///> see addTimerInLoop().
  TimerId addTimer(TimerCallback cb, Timestamp when, double interval);
  ///> see cancelInLoop().
  void cancel(TimerId timerId);

private:
  ///> 1) insert timer
  ///  2) if timer is earliest, reset experation timer.
  void addTimerInLoop(Timer* timer);
  ///> 1) if timer is in activeTimers_, erase from activeTimers_ and timers_.
  ///  2) if that has expiration timer, add into cancelingTimers_.
  void cancelInLoop(TimerId timerId);
  ///> invoke callback to do task.
  void handleRead();
  // move out all expired timers
  ///> move out all expired timers from activeTimers_ and timers_.
  std::vector<Entry> getExpired(Timestamp now);
  ///> set repeat timer
  ///> remove non repeat timer
  ///> reset earliest timer
  void reset(const std::vector<Entry>& expired, Timestamp now);
  ///> insert timer into timers_ and activeTimers_, return result that whether
  ///  the timer is earliest.
  bool insert(Timer* timer);
};

}  // namespace net
}  // namespace muduo
#endif  // MUDUO_NET_TIMERQUEUE_H
