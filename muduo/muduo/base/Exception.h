// Use of this source code is governed by a BSD-style license
// that can be found in the License file.
//
// Author: Shuo Chen (chenshuo at chenshuo dot com)

#ifndef MUDUO_BASE_EXCEPTION_H
#define MUDUO_BASE_EXCEPTION_H

#include <muduo/base/Types.h>
#include <exception>

namespace muduo
{

class Exception : public std::exception
{
private:
  string message_; ///< exception infos
  string stack_; ///< thread stack infos

public:
  Exception(string what);
  ~Exception() noexcept override = default;

  // default copy-ctor and operator= are okay.

  const char* what() const noexcept override
  {
    return message_.c_str();
  }

  const char* stackTrace() const noexcept
  {
    return stack_.c_str();
  }
};

}  // namespace muduo

#endif  // MUDUO_BASE_EXCEPTION_H
