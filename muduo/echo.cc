#include "echo.h"

#include "muduo/base/Logging.h"

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;

EchoServer::EchoServer(muduo::net::EventLoop* loop, const muduo::net::InetAddress& listenAddr) 
    : loop(loop), server(loop, listenAddr, "Echo服务器")
{
    server.setConnectionCallback(
        std::bind(&EchoServer::onConnection, this, _1));
    server.setMessageCallback(
        std::bind(&EchoServer::onMessage, this, _1, _2, _3));
}

void EchoServer::start() {
    server.start();
}

void EchoServer::onConnection(const muduo::net::TcpConnectionPtr& conn) {
    LOG_INFO << "Echo服务器- " << conn->peerAddress().toIpPort() << " -> "
        << conn->localAddress().toIpPort() << "is "
        << (conn->connected() ? "已连接" : "无法连接");
}

void onMessage(const muduo::net::TcpConnectionPtr& conn, muduo::net::Buffer* buf, muduo::Timestamp time) {
    muduo::string msg(buf->retrieveAllAsString());
    LOG_INFO << conn->name() << " echo " << msg.size() << " bytes." 
             << "data received at " << time.toString();
    conn->send(msg);
}