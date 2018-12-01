// Use of this source code is governed by a BSD-style license
// that can be found in the License file.
//
// Author: Shuo Chen (chenshuo at chenshuo dot com)

#ifndef MUDUO_BASE_CURRENTTHREAD_H
#define MUDUO_BASE_CURRENTTHREAD_H

#include <muduo/base/Types.h>

namespace muduo
{
namespace CurrentThread
{
  // internal
  extern __thread int t_cachedTid;
  extern __thread char t_tidString[32];
  extern __thread int t_tidStringLength;
  extern __thread const char* t_threadName;
  void cacheTid();

  /**
   * - __builtin_expect:
   *    将流水线引入cpu，可以提高cpu的效率。更简单的说，让cpu可以预先取出下一条
   *    指令，可以提高cpu的效率. 如果命中跳转指令，那么预先取出的指令就无用了。
   *    因此，跳转指令会降低流水线的效率，也就是降低cpu的效率。
   *    综上，在写程序时应该尽量避免跳转语句。那么如何避免跳转语句呢？
   *    答案就是使用__builtin_expect。
   *    这个指令是gcc引入的，作用是"允许程序员将最有可能执行的分支告诉编译器"。
   *    这个指令的写法为：__builtin_expect(EXP, N)。意思是：EXP==N的概率很大。
   *    一般的使用方法是将__builtin_expect指令封装为LIKELY和UNLIKELY宏。
   *    这两个宏的写法如下。
   *      #define LIKELY(x) __builtin_expect(!!(x), 1) //x很可能为真
   *      #define UNLIKELY(x) __builtin_expect(!!(x), 0) //x很可能为假
   *    回到代码中,if (__builtin_expect(t_cachedTid == 0, 0)),表明tid为0的
   *    可能性很小,那么反汇编代码可以代码优化为0的情况为跳转。
   *
   */
  inline int tid()
  {
    if (__builtin_expect(t_cachedTid == 0, 0))
    {
      cacheTid();
    }
    return t_cachedTid;
  }

  inline const char* tidString() // for logging
  {
    return t_tidString;
  }

  inline int tidStringLength() // for logging
  {
    return t_tidStringLength;
  }

  inline const char* name()
  {
    return t_threadName;
  }

  bool isMainThread();

  void sleepUsec(int64_t usec);  // for testing

  string stackTrace(bool demangle);
}  // namespace CurrentThread
}  // namespace muduo

#endif  // MUDUO_BASE_CURRENTTHREAD_H
