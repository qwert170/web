
#ifndef ACCEPTOR_H
#define ACCEPTOR_H
//接受（accept）客户端的连接
#include <functional>
#include "../base/noncopyable.h"
#include "../net/Channel.h"
#include "../net/Socket.h"

namespace web
{
namespace net
{
class EventLoop;
class InetAddress;

class Acceptor : noncopyable
{
 public:
  typedef std::function<void (int sockfd, const InetAddress&)> NewConnectionCallback;

  Acceptor(std::shared_ptr<EventLoop> loop, const InetAddress& listenAddr, bool reuseport);
  ~Acceptor();

  void setNewConnectionCallback(const NewConnectionCallback& cb){ newConnectionCallback_ = cb; }

  void listen(); //启动监听套接字

  bool listening() const { return listening_; }

 private:
  void handleRead();

  std::shared_ptr<EventLoop> loop_;
  Socket acceptSocket_;
  Channel acceptChannel_;
  NewConnectionCallback newConnectionCallback_;
  bool listening_;
  int idleFd_;
};

}  // namespace net
} 

#endif 
