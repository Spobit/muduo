#ifndef MUDUO_EXAMPLES_MAXCONNECTION_ECHO_H
#define MUDUO_EXAMPLES_MAXCONNECTION_ECHO_H

#include <muduo/net/TcpServer.h>

// RFC 862
/**
 * - Brief:
 *    Control max connector count.
 * - Imp:
 *    if it has been large than max connector count, the connector will be
 *    closed, see EchoServer::onConnection().
 */
class EchoServer
{
 public:
  EchoServer(muduo::net::EventLoop* loop,
             const muduo::net::InetAddress& listenAddr,
             int maxConnections);

  void start();

 private:
  void onConnection(const muduo::net::TcpConnectionPtr& conn);

  void onMessage(const muduo::net::TcpConnectionPtr& conn,
                 muduo::net::Buffer* buf,
                 muduo::Timestamp time);

  muduo::net::TcpServer server_;
  int numConnected_; // should be atomic_int
  const int kMaxConnections_;
};

#endif  // MUDUO_EXAMPLES_MAXCONNECTION_ECHO_H
