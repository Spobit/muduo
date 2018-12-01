#ifndef MUDUO_EXAMPLES_SIMPLE_CHARGEN_CHARGEN_H
#define MUDUO_EXAMPLES_SIMPLE_CHARGEN_CHARGEN_H

#include <muduo/net/TcpServer.h>

// RFC 864
/**
 * - RFC 864 (Character generator service)
 *    A useful debugging and measurement tool is a character generator service.
 *    Character generator service simply sends data without regard to the input.
 * - Intension:
 *    Server keep sending data to client, for test.
 * - Method:
 *    1) Fst, connection will send msg when ChargenServer::onConnection(),
 *    2) Scd, connection will send msg in ChargenServer::onWriteComplete()
 *       when msg was sent complete.
 */
class ChargenServer
{
public:
  ChargenServer(muduo::net::EventLoop* loop,
                const muduo::net::InetAddress& listenAddr,
                bool print = false);

  void start();

private:
  void onConnection(const muduo::net::TcpConnectionPtr& conn);

  void onMessage(const muduo::net::TcpConnectionPtr& conn,
                 muduo::net::Buffer* buf,
                 muduo::Timestamp time);

  void onWriteComplete(const muduo::net::TcpConnectionPtr& conn);
  void printThroughput();

  muduo::net::TcpServer server_;

  muduo::string message_;
  int64_t transferred_;
  muduo::Timestamp startTime_;
};

#endif  // MUDUO_EXAMPLES_SIMPLE_CHARGEN_CHARGEN_H
