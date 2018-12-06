// Use of this source code is governed by a BSD-style license
// that can be found in the License file.
//
// Author: Shuo Chen (chenshuo at chenshuo dot com)

#ifndef MUDUO_BASE_THREAD_H
#define MUDUO_BASE_THREAD_H

#include <muduo/base/Atomic.h>
#include <muduo/base/CountDownLatch.h>
#include <muduo/base/Types.h>

#include <functional>
#include <memory>
#include <pthread.h>

namespace muduo
{

class Thread : noncopyable
{
public:
  typedef std::function<void ()> ThreadFunc;
private:
  bool started_; ///< true if call pthread_create() successfully.
  bool joined_; ///< true if call pthread_join().
  pthread_t pthreadId_;
  pid_t tid_; ///< tid of thread created
  ThreadFunc func_; ///< thread callback
  string name_; ///< thread string id
  CountDownLatch latch_; ///> communication between owner thrd and thrd created.
  static AtomicInt32 numCreated_; ///< count of thread created by Thread class.

public:
  explicit Thread(ThreadFunc, const string& name = string());
  // FIXME: make it movable in C++11
  ~Thread();

  void start();
  int join(); // return pthread_join()

  bool started() const { return started_; }
  // pthread_t pthreadId() const { return pthreadId_; }
  pid_t tid() const { return tid_; }
  const string& name() const { return name_; }

  static int numCreated() { return numCreated_.get(); }

private:
  void setDefaultName();
};

}  // namespace muduo
#endif  // MUDUO_BASE_THREAD_H
