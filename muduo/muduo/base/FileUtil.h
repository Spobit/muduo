// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

// Author: Shuo Chen (chenshuo at chenshuo dot com)
//
// This is a public header file, it must only include public header files.

#ifndef MUDUO_BASE_FILEUTIL_H
#define MUDUO_BASE_FILEUTIL_H

#include <muduo/base/noncopyable.h>
#include <muduo/base/StringPiece.h>
#include <sys/types.h>  // for off_t

namespace muduo
{
namespace FileUtil
{
/**
* - pread:
* pread()  reads  up to count bytes from file descriptor fd at offset offset
* (from the start of the file) into the buffer starting at buf.  The file offset
* is not changed.
*/

/**
* read small file 64KB to a buffer.
*/
class ReadSmallFile : noncopyable
{
public:
  static const int kBufferSize = 64*1024;
private:
  int fd_;
  int err_;
  char buf_[kBufferSize];

public:
  ReadSmallFile(StringArg filename);
  ~ReadSmallFile();

  // read file data to \a content, max is 64KB.
  // is type of \a content vector<char> ?
  // return errno
  template<typename String>
  int readToString(int maxSize,
                   String* content,
                   int64_t* fileSize,
                   int64_t* modifyTime,
                   int64_t* createTime);
  // Read at maxium kBufferSize-1 byte into buf_, and buf_ is null-terminated.
  // \a size is read size.
  // return errno
  int readToBuffer(int* size);
  const char* buffer() const { return buf_; }
};

// read the file data to \a content, max is 64KB.
// returns errno if error happens.
// This function encapsulates the ReadSmallFile class.
template<typename String>
int readFile(StringArg filename,
             int maxSize,
             String* content,
             int64_t* fileSize = NULL,
             int64_t* modifyTime = NULL,
             int64_t* createTime = NULL)
{
  ReadSmallFile file(filename);
  return file.readToString(maxSize, content, fileSize, modifyTime, createTime);
}

/**
 * RAII class, wirte data to file, the fd has 64KB buffer.
 * it writing operation use fwrite_unlocked().
 *
 * - when time data write to file?
 * 1. \m writtenBytes_ is greate than 64KB buffer.
 * 2. class dtor.
 */
// not thread safe
class AppendFile : noncopyable
{
private:
  FILE* fp_;
  char buffer_[64*1024];
  off_t writtenBytes_;


public:
  explicit AppendFile(StringArg filename);
  ~AppendFile();

  void append(const char* logline, size_t len);
  void flush();
  off_t writtenBytes() const { return writtenBytes_; }

private:
  // non blocking write to file
  size_t write(const char* logline, size_t len);
};

}  // namespace FileUtil
}  // namespace muduo

#endif  // MUDUO_BASE_FILEUTIL_H

