#include "codec.h"

#include <muduo/base/Logging.h>
#include <muduo/base/Mutex.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/TcpServer.h>

#include <set>
#include <stdio.h>
#include <unistd.h>

using namespace muduo;
using namespace muduo::net;

class ChatServer : noncopyable
{
public:
  ChatServer(EventLoop* loop, const InetAddress& listenAddr)
    : server_(loop, listenAddr, "ChatServer"),
      codec_(std::bind(&ChatServer::onStringMessage, this, _1, _2, _3))
  {
    LOG_INFO << "ChatServer started successfully!";

    // set connection and disconnection handler
    server_.setConnectionCallback(
          std::bind(&ChatServer::onConnection, this, _1));
    // set msg handler, but just decode, then call ChatServer::onStringMessage()
    // so, real msg handler
    server_.setMessageCallback(
          std::bind(&LengthHeaderCodec::onMessage, &codec_, _1, _2, _3));
  }
  ~ChatServer() {LOG_INFO << "ChatServer exit!";}

  void start() {server_.start();}

private:
  void onConnection(const TcpConnectionPtr& conn)
  {
    LOG_INFO << conn->localAddress().toIpPort() << " -> "
             << conn->peerAddress().toIpPort() << " is "
             << (conn->connected() ? "UP" : "DOWN");

    if (conn->connected())
      connections_.insert(conn);
    else
      connections_.erase(conn);
  }

  void onStringMessage(const TcpConnectionPtr&,
                       const string& message,
                       Timestamp)
  {
    for (ConnectionList::iterator it = connections_.begin();
         it != connections_.end();
         ++it)
    {
      codec_.send(get_pointer(*it), message);
    }
  }

  typedef std::set<TcpConnectionPtr> ConnectionList;
  TcpServer server_; ///< real server class
  LengthHeaderCodec codec_; ///< msg code class
  ConnectionList connections_; ///< client connection list
};

int main(int argc, char* argv[])
{
  LOG_INFO << "pid = " << getpid();

  if (argc < 2)
  {
    printf("Usage: %s port\n", argv[0]);
  }
  else
  {
    EventLoop loop;
    uint16_t port = static_cast<uint16_t>(atoi(argv[1]));
    InetAddress serverAddr(port);
    ChatServer server(&loop, serverAddr);
    server.start();
    loop.loop();
  }
}

