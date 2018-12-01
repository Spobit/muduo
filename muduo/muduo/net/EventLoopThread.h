// Copyright 2010, Shuo Chen.  All rights reserved.
// http://code.google.com/p/muduo/
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

// Author: Shuo Chen (chenshuo at chenshuo dot com)
//
// This is a public header file, it must only include public header files.

#ifndef MUDUO_NET_EVENTLOOPTHREAD_H
#define MUDUO_NET_EVENTLOOPTHREAD_H

#include <muduo/base/Condition.h>
#include <muduo/base/Mutex.h>
#include <muduo/base/Thread.h>

namespace muduo
{
namespace net
{

class EventLoop;

/** class EventLoopThread
 * - Brief:
 *    this class make EventLoop into a new thread.
 */
class EventLoopThread : noncopyable
{
public:
  typedef std::function<void(EventLoop*)> ThreadInitCallback;

  EventLoopThread(const ThreadInitCallback& cb = ThreadInitCallback(),
                  const string& name = string());
  ~EventLoopThread();
  EventLoop* startLoop();

private:
  ///> thread callback.
  void threadFunc();

  ///> loop_ will will be created in thread_ thread.
  ///> the thread_ is a EventLoop thread.
  EventLoop* loop_ /*GUARDED_BY(mutex_)*/;
  ///> will exit loop thread.
  bool exiting_;
  ///> representational thread object.
  Thread thread_;
  ///> wait loop_ is created.
  MutexLock mutex_;
  Condition cond_ /*GUARDED_BY(mutex_)*/;
  ///> thread initialization
  ThreadInitCallback callback_;
};

}  // namespace net
}  // namespace muduo

#endif  // MUDUO_NET_EVENTLOOPTHREAD_H

