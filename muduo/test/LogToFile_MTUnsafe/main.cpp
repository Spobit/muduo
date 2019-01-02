#include <muduo/base/LogFile.h>
#include <muduo/base/TimeZone.h>
#include <muduo/base/Logging.h>
#include <muduo/base/AsyncLogging.h>
#include <iostream>

using namespace muduo;

//LogFile g_logFile("LogToFile_MTUnsafe", "", 1024*1024*100, true);
//LogFile g_logFile("LogToFile_MTUnsafe", "/", 1024*1024*100, false);
LogFile g_logFile("LogToFile_MTUnsafe", "/home/eit", 1024*1024*100, false);

void logFileOutput(const char* msg, int len)
{ g_logFile.append(msg, len); }

void logFileFlush()
{ g_logFile.flush(); }

int main(int argc, char *argv[])
{
  Logger::setOutput(logFileOutput);
  Logger::setFlush(logFileFlush);

  Timestamp startTime = Timestamp::now();
  int iLoop = 1000*1000;
  for (int i = 1; i <= iLoop; i++)
  {
    LOG_INFO << "\"" << Timestamp::now().toFormattedString(true) << "\""
             << " " << Fmt("%010u", i);
  }
  Timestamp endTime = Timestamp::now();
  std::cout << "Print " << iLoop << " times log to file:" << std::endl;
  std::cout << startTime.toFormattedString() << std::endl;
  std::cout << endTime.toFormattedString() << std::endl;

  return 0;
}
