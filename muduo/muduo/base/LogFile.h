// Use of this source code is governed by a BSD-style license
// that can be found in the License file.
//
// Author: Shuo Chen (chenshuo at chenshuo dot com)

#ifndef MUDUO_BASE_LOGFILE_H
#define MUDUO_BASE_LOGFILE_H

#include <muduo/base/Mutex.h>
#include <muduo/base/Types.h>

#include <memory>

namespace muduo
{

namespace FileUtil
{
class AppendFile;
}

/**
 * - LogFile
 * LogFile is a class that has roll capability. roll capability is that the
 * class can use a new log file when roll condation hit. there has two kind
 * of roll condation, detail to see \m append_unlocked():
 *  1) file size
 *  2) written numbers
 *
 * \m file_ is a very nice method, it make roll file very easy. RAII.
 */
class LogFile : noncopyable
{
private:
  const static int kRollPerSeconds_ = 60*60*24;
    // how mush time use a new file, one day.
private:
  const string basename_;
    // recommend process name.
    // processName.20181224-122022.hostName.pid.log
  const off_t rollSize_; // roll file when written size is greate than it.
  const int flushInterval_; // flush interval.
  const int checkEveryN_;
    // to flush or roll when written time is greate than it.
  int count_; // count of current roll file written time.

  std::unique_ptr<MutexLock> mutex_;
  time_t startOfPeriod_; // the time of create file
  time_t lastRoll_;
  time_t lastFlush_; // last flush of current roll file
  std::unique_ptr<FileUtil::AppendFile> file_;

public:
  LogFile(const string& basename,
          off_t rollSize,
          bool threadSafe = true,
          int flushInterval = 3,
          int checkEveryN = 1024);
  ~LogFile();

  void append(const char* logline, int len);
  void flush();

  /**
   * create file when now time has time offset in second relating with last roll.
   */
  bool rollFile();

private:
  /**
   * size roll when written bytes is over \m rollSize_.
   * time roll when written time is over \m checkEveryN_
   * second flush when time is over \m flushInterval_.
   * Note: the matching second is not roll.
   */
  void append_unlocked(const char* logline, int len);
  // get a log file name: processName.20181224-122022.hostName.pid.log
  static string getLogFileName(const string& basename, time_t* now);

};

}  // namespace muduo
#endif  // MUDUO_BASE_LOGFILE_H
