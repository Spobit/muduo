#include <muduo/base/LogFile.h>
#include <muduo/base/TimeZone.h>
#include <muduo/base/Logging.h>
#include <muduo/base/AsyncLogging.h>
#include <iostream>

using namespace muduo;


///> logFile
//LogFile g_logFile("mytest", 1024*1024*100, false, 3, 0xfffffff);

//void logFileOutput(const char* msg, int len)
//{ g_logFile.append(msg, len); }

//void logFileFlush()
//{ g_logFile.flush(); }

//int main(int argc, char *argv[])
//{
//  Logger::setOutput(logFileOutput);
//  Logger::setFlush(logFileFlush);

//  for (int i = 1; i <= 1000000; i++)
//  {
//    LOG_INFO << "\"" << Timestamp::now().toFormattedString(true) << "\""
//             << " " << Fmt("%07d", i);
//  }

//  return 0;
//}


///> AsyncLogging
AsyncLogging g_asyncLog("mytest", 1024*1024*100);

void logFileOutput(const char* msg, int len)
{ g_asyncLog.append(msg, len); }

// do not need flushing capability, it intends to do line flushing, disable it.
void logFileFlush() { return; }

void thrdFun()
{
  for (int i = 1; i <= 1000000; i++)
  {
    LOG_INFO << "\"" << Timestamp::now().toFormattedString(true) << "\""
             << " " << Fmt("%07d", i);
  }
}

int main()
{
  Logger::setOutput(logFileOutput);
  Logger::setFlush(logFileFlush);

  std::cout << Timestamp::now().toFormattedString(true) << std::endl;
  g_asyncLog.start();
  {
    Thread thrd1(thrdFun);
    Thread thrd2(thrdFun);
    thrd1.start();
    thrd2.start();

    thrd1.join();
    thrd2.join();
  }
  g_asyncLog.stop();
  std::cout << Timestamp::now().toFormattedString(true) << std::endl;

  return 0;
}

