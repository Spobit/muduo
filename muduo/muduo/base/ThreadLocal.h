// Use of this source code is governed by a BSD-style license
// that can be found in the License file.
//
// Author: Shuo Chen (chenshuo at chenshuo dot com)

#ifndef MUDUO_BASE_THREADLOCAL_H
#define MUDUO_BASE_THREADLOCAL_H

#include <muduo/base/Mutex.h>  // MCHECK
#include <muduo/base/noncopyable.h>

#include <pthread.h>

namespace muduo
{
/**
 * - Brief:
 *    create thread local variable by pthread_key_create(), then, other thread 
 *    who can access the key who create by pthread_key_create() can access the 
 *    variable.
 *
 * - Functions:
 *    pthread_key_create()
 *    pthread_key_delete()
 *    pthread_setspecific()
 *    pthread_getspecific()
 *    void (*destructor)(void*)
 */
template<typename T>
class ThreadLocal : noncopyable
{
private:
  pthread_key_t pkey_;

public:
  ThreadLocal()
  {
    MCHECK(pthread_key_create(&pkey_, &ThreadLocal::destructor));
  }

  ~ThreadLocal()
  {
    MCHECK(pthread_key_delete(pkey_));
  }

  T& value()
  {
    ///> The pthread_getspecific() function shall return the thread-specific
    ///> data value associated with the given key.
    ///> If no thread-specific data value is associated with key, then the value
    ///> NULL shall be returned.
    T* perThreadValue = static_cast<T*>(pthread_getspecific(pkey_));
    if (!perThreadValue)
    {
      T* newObj = new T();
      MCHECK(pthread_setspecific(pkey_, newObj));
      perThreadValue = newObj;
    }
    return *perThreadValue;
  }

private:
  static void destructor(void *x)
  {
    T* obj = static_cast<T*>(x);
    typedef char T_must_be_complete_type[sizeof(T) == 0 ? -1 : 1];
    T_must_be_complete_type dummy; (void) dummy;
    delete obj;
  }
};

}  // namespace muduo

#endif  // MUDUO_BASE_THREADLOCAL_H
