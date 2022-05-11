#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <atomic>
#include "../base/Types.h"
#include "../net/TcpConnection.h"
#include "../base/noncopyable.h"
#include <map>

namespace web
{
namespace net
{
class Acceptor;
class EventLoop;
class EventLoopThreadPool;

class TcpServer : noncopyable
{
 public:
  typedef std::function<void(const std::shared_ptr<EventLoop>&)> ThreadInitCallback;
  enum Option{kNoReusePort,kReusePort,};

  TcpServer(std::shared_ptr<EventLoop> loop,const InetAddress& listenAddr,Option option = kNoReusePort);
  ~TcpServer();  

  const string& ipPort() const { return ipPort_; }
  std::shared_ptr<EventLoop> getLoop() const { return loop_; }

  void setThreadNum(int numThreads);
  void setThreadInitCallback(const ThreadInitCallback& cb){ threadInitCallback_ = cb; }

  std::shared_ptr<EventLoopThreadPool> threadPool(){ return threadPool_; }

  void start();

  void setConnectionCallback(const ConnectionCallback& cb){ connectionCallback_ = cb; }
  void setMessageCallback(const MessageCallback& cb){ messageCallback_ = cb; }
  void setWriteCompleteCallback(const WriteCompleteCallback& cb){ writeCompleteCallback_ = cb; }

  void newConnection(int sockfd, const InetAddress& peerAddr);
  void removeConnection(const TcpConnectionPtr& conn);
  void removeConnectionInLoop(const TcpConnectionPtr& conn);
 private:
  typedef std::map<string, TcpConnectionPtr> ConnectionMap;

  std::shared_ptr<EventLoop> loop_;  
  const string ipPort_;

  std::unique_ptr<Acceptor> acceptor_; 
  std::shared_ptr<EventLoopThreadPool> threadPool_;

  ConnectionCallback connectionCallback_;
  MessageCallback messageCallback_;
  WriteCompleteCallback writeCompleteCallback_;
  ThreadInitCallback threadInitCallback_;

  std::atomic<int> started_;

  int nextConnId_;
  ConnectionMap connections_;
};

}  // namespace net
}  

#endif 
