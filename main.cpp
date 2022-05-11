#include "./base/Logging.h"
#include "./base/AsyncLogging.h"
#include "./net/EventLoop.h"
#include "./net/TcpConnection.h"
#include "./net/TcpServer.h"
#include "./net/InetAddress.h"
#include <iostream>

using namespace web;
using namespace web::net;

int main(int argc, char *argv[])
{
  //std::cout<<"abc";
  web::AsyncLogging *g_asyncLog =new web::AsyncLogging("TcpServer",  1024 * 1024, 3);

  web::Logger::setLogLevel(web::Logger::TRACE);
  g_asyncLog->start();
  
  std::shared_ptr<web::net::EventLoop> mainLoop =std::make_shared<web::net::EventLoop>();

  web::net::InetAddress  addr("127.0.0.1", 9006);
  std::shared_ptr<web::net::TcpServer> server =std::make_shared<web::net::TcpServer>(mainLoop, addr);
  server->start();
  mainLoop->loop();
  return 0;
}

