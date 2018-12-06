// Use of this source code is governed by a BSD-style license
// that can be found in the License file.
//
// Author: Shuo Chen (chenshuo at chenshuo dot com)

#ifndef MUDUO_BASE_THREADPOOL_H
#define MUDUO_BASE_THREADPOOL_H

#include <muduo/base/Condition.h>
#include <muduo/base/Mutex.h>
#include <muduo/base/Thread.h>
#include <muduo/base/Types.h>

#include <deque>
#include <vector>

namespace muduo
{

/**
 * - Brief:
 *    1) Create specified number of threads, let theme do tasks.
 *    2) Or, no thread, just self, let self do tasks.
 * - Do tasks by threads:
 *    start() --> runInThread() --> take() --> do task.
 *    create threads --> run thread callback --> wait or take task --> do task.
 *    use run() to add task.
 * - Do task by self:
 *    start()   run()
 *    use run() to do task.
 * - Notes:
 *    ThreadPool class is Thread-Safe, but the Thread-Safe of task is controled
 *    by yourself.
 *    ThreadPool object owner maybe block when call run, is that you want?
 */

class ThreadPool : noncopyable
{
public:
  typedef std::function<void ()> Task;
private:
  mutable MutexLock mutex_;
  ///> task deque is not empty, the emphasis is on can do task.
  Condition notEmpty_;
  ///> task deque is not full, the emphasis is on can push task in deque.
  Condition notFull_;
  string name_; ///< specify threadPool-object-name string by user
  ///> every thread who is in pool will call it before do tasks.
  ///> if no thread in pool, pool will call it if it is not null pointer.
  Task threadInitCallback_;
  std::vector<std::unique_ptr<muduo::Thread>> threads_;
  std::deque<Task> queue_; ///< tasks queue
  ///> if is 0 or negative, queue is no restriction,
  ///> otherelse, it mean max tasks count.
  ///> why no use in constant?
  size_t maxQueueSize_;
  bool running_; ///< see start() and stop()

public:
  explicit ThreadPool(const string& nameArg = string("ThreadPool"));
  ~ThreadPool();

  // Must be called before start().
  ///> set max tasks count in deque. see maxQueueSize_.
  void setMaxQueueSize(int maxSize) { maxQueueSize_ = maxSize; }
  ///> set thread creat by pool or thread pool initialization before do tasks.
  ///> see start() and runInThread().
  void setThreadInitCallback(const Task& cb) { threadInitCallback_ = cb; }

  ///> create threads and call initialization(threadInitCallback_).
  void start(int numThreads);
  void stop();

  const string& name() const { return name_; }
  size_t queueSize() const; ///< tasks count

  // Could block if maxQueueSize > 0, because of tasks deque is full,
  // blocking until no full.
  ///> if is no threads in pool, pool-thread do task; otherelse, add task to deque.
  ///> maybe block.
  void run(Task f);

private:
  ///> if tasks is full in queue.
  bool isFull() const;
  ///> initialization and do tasks in someone thread create by pool.
  void runInThread();
  ///> take a task from deque, maybe block.
  Task take();
};

}  // namespace muduo

#endif  // MUDUO_BASE_THREADPOOL_H
