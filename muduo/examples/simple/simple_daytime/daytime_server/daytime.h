#ifndef MUDUO_EXAMPLES_SIMPLE_DAYTIME_DAYTIME_H
#define MUDUO_EXAMPLES_SIMPLE_DAYTIME_DAYTIME_H

#include <muduo/net/TcpServer.h>

// RFC 867
/**
 * - RFC 867 (Daytime Protocol):
 *    A useful debugging and measurement tool is a daytime service.
 *    A daytime service simply sends a the current date and time
 *    as a character string without regard to the input.
 * - Imp:
 *    send datetime and wait close when client is connecting.
 */
class DaytimeServer
{
 public:
  DaytimeServer(muduo::net::EventLoop* loop,
                const muduo::net::InetAddress& listenAddr);

  void start();

 private:
  void onConnection(const muduo::net::TcpConnectionPtr& conn);

  void onMessage(const muduo::net::TcpConnectionPtr& conn,
                 muduo::net::Buffer* buf,
                 muduo::Timestamp time);

  muduo::net::TcpServer server_;
};

#endif  // MUDUO_EXAMPLES_SIMPLE_DAYTIME_DAYTIME_H
