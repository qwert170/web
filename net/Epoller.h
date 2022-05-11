#ifndef EPOLLER_H
#define EPOLLER_H

#include <map>
#include <vector>
#include <memory>
#include <sys/epoll.h>

#include "Channel.h"
#include "../base/Timestamp.h"
#include "../net/EventLoop.h"
namespace web
{
namespace net
{
class Channel;

class Epoller : noncopyable
{
 public:
  typedef std::vector<Channel*> ChannelList;

  Epoller(EventLoop* loop);
  ~Epoller();

  Timestamp poll(int timeoutMs, ChannelList* activeChannels);
  void updateChannel(Channel* channel) ;
  void removeChannel(Channel* channel) ;
  bool hasChannel(Channel* channel) const;
  const char* operationToString(int op);
 private:
  void fillActiveChannels(int numEvents, ChannelList *activeChannels) const;
  void update(int operation, Channel *channel);
  void assertInLoopThread() const
  {
    ownerLoop_->assertInLoopThread();
  }

  typedef std::map<int, Channel*> ChannelMap;
  ChannelMap channels_;
  
  int epollfd_;
  using EventList = std::vector<struct epoll_event>;
  EventList events_;              
  EventLoop* ownerLoop_;
};

}  // namespace net
} 

#endif  
