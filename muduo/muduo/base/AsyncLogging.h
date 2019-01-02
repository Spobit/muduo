// Use of this source code is governed by a BSD-style license
// that can be found in the License file.
//
// Author: Shuo Chen (chenshuo at chenshuo dot com)

#ifndef MUDUO_BASE_ASYNCLOGGING_H
#define MUDUO_BASE_ASYNCLOGGING_H

#include <muduo/base/BlockingQueue.h>
#include <muduo/base/BoundedBlockingQueue.h>
#include <muduo/base/CountDownLatch.h>
#include <muduo/base/Mutex.h>
#include <muduo/base/Thread.h>
#include <muduo/base/LogStream.h>

#include <atomic>
#include <vector>

namespace muduo
{

class AsyncLogging : noncopyable
{
private:
  typedef muduo::detail::FixedBuffer<muduo::detail::kLargeBuffer> Buffer;
  typedef std::vector<std::unique_ptr<Buffer>> BufferVector;
  typedef BufferVector::value_type BufferPtr;

private:
  const string basename_;
  const string storedpath_;
  const off_t rollSize_;
  const int flushInterval_;
    ///> every flushInterval_ seconds flush buffer to file.

  muduo::Condition cond_ /*GUARDED_BY(mutex_)*/;
    // notify thread to write log to file.

  std::atomic<bool> running_;
    // thread is running.
  muduo::Thread thread_;
    // log thread
  muduo::CountDownLatch latch_;
    // wait for thread callback is invoked.

  /**
   * - foreground buffer
   * 1st: the first, all log will write to \m currentBuffer_.
   * 2st: when \m currentBuffer_ is full, push to \m buffers_ and move
   *      \m nextBuffer_ to \m currentBuffer_.
   * 3st: when \m currentBuffer_ is full, push to \m buffers_ and create a new
   *      buffer to \m currentBuffer_.
   * 4st: same as above action.
   */
  BufferPtr currentBuffer_ /*GUARDED_BY(mutex_)*/;
    // foreground current buffer
  BufferPtr nextBuffer_ /*GUARDED_BY(mutex_)*/;
    // foreground next buffer
  BufferVector buffers_ /*GUARDED_BY(mutex_)*/;
    // foreground vector buffers

  muduo::MutexLock mutex_;

public:

  AsyncLogging(const string& basename,
               const string& storedpath,
               off_t rollSize,
               int flushInterval = 3);
  ~AsyncLogging() { if (running_) stop(); }

  // write log to file
  // notify thread while \m currentBuffer_ is full.
  void append(const char* logline, int len);
  void start()
  {
    running_ = true;
    thread_.start();
    latch_.wait();
  }
  void stop() NO_THREAD_SAFETY_ANALYSIS
  {
    running_ = false;
    cond_.notify();
    thread_.join();
  }

private:
  // write log to file
  /**
   * -
   * 1st: create LogFile object and background buffers.
   * 2st: write log to file.
   *    1) wait for write log.
   *    2) foreground buffer to background buffer, reset foreground buffer.
   *    3) drop logs when log is large than 25.
   *    4) write logs to file (buffer).
   *    5) drop vector elements when it is large than 2.
   *    5) reset newBuffer1 and newBuffer2 variable.
   *    6) clean vector.
   *    7) flush (date to file, it is real into disk file).
   *    8) while.
   *
   * - background buffer
   */
  void threadFunc();
};

}  // namespace muduo

#endif  // MUDUO_BASE_ASYNCLOGGING_H
