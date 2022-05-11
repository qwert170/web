#ifndef HTTP_HTTPSERVER_H
#define HTTP_HTTPSERVER_H

#include "../web1/net/TcpServer.h"

namespace web
{
namespace net
{

class HttpRequest;
class HttpResponse;

class HttpServer : noncopyable
{
 public:
  typedef std::function<void (const HttpRequest&,HttpResponse*)> HttpCallback;

  HttpServer(std::shared_ptr<EventLoop> loop,
             const InetAddress& listenAddr,
             const string& name,
             TcpServer::Option option = TcpServer::kNoReusePort);

  std::shared_ptr<EventLoop> getLoop() const { return server_.getLoop(); }

  void setHttpCallback(const HttpCallback& cb)
  {
    httpCallback_ = cb;
  }

  void setThreadNum(int numThreads)
  {
    server_.setThreadNum(numThreads);
  }

  void start();

 private:
  void onConnection(const TcpConnectionPtr& conn);
  void onMessage(const TcpConnectionPtr& conn,
                 Buffer* buf,
                 Timestamp receiveTime);
  void onRequest(const TcpConnectionPtr&, const HttpRequest&);

  TcpServer server_;
  HttpCallback httpCallback_;
};

}  // namespace net
} 

#endif 
