#include <muduo/base/LogFile.h>
#include <muduo/base/TimeZone.h>
#include <muduo/base/Logging.h>
#include <muduo/base/AsyncLogging.h>
#include <iostream>

using namespace muduo;

AsyncLogging g_asyncLog("mytest", 1024*1024*100);
const int g_iLoop = 1000*1000;

void logFileOutput(const char* msg, int len)
{ g_asyncLog.append(msg, len); }

// do not need flushing capability, it intends to do line flushing, disable it.
void logFileFlush() { return; }

void thrdFun()
{
  for (int i = 1; i <= g_iLoop; i++)
  {
    LOG_INFO << "\"" << Timestamp::now().toFormattedString(true) << "\""
             << " " << Fmt("%07d", i);
  }
}

int main()
{
  Logger::setOutput(logFileOutput);
  Logger::setFlush(logFileFlush);

  g_asyncLog.start();
  {
    Timestamp startTime = Timestamp::now();

    Thread* thrdArray[6];
    for (size_t i = 0; i < sizeof(thrdArray) / sizeof(Thread*); i++)
      thrdArray[i] = new Thread(thrdFun);
    for (size_t i = 0; i < sizeof(thrdArray) / sizeof(Thread*); i++)
      thrdArray[i]->start();
    for (size_t i = 0; i < sizeof(thrdArray) / sizeof(Thread*); i++)
      thrdArray[i]->join();

    Timestamp endTime = Timestamp::now();
    std::cout << "Print " << g_iLoop * 2 << " times log to file:" << std::endl;
    std::cout << startTime.toFormattedString() << std::endl;
    std::cout << endTime.toFormattedString() << std::endl;
  }
  g_asyncLog.stop();

  return 0;
}

