#ifndef CHANNEL_H
#define CHANNEL_H

#include "../base/noncopyable.h"
#include "../base/Timestamp.h"

#include <functional>
#include <memory>

namespace web
{
  
namespace net
{
class EventLoop;

class Channel : noncopyable
{
 public:
  typedef std::function<void()> EventCallback;
  typedef std::function<void(Timestamp)> ReadEventCallback;
  enum States { kDisconnected, kConnecting, kConnected};

  Channel(std::shared_ptr<EventLoop> loop, int fd);
  ~Channel();

  void handleEvent(Timestamp receiveTime);

  void setReadCallback(ReadEventCallback cb){ readCallback_ = std::move(cb); }
  void setWriteCallback(EventCallback cb){ writeCallback_ = std::move(cb); }
  void setCloseCallback(EventCallback cb){ closeCallback_ = std::move(cb); }
  void setErrorCallback(EventCallback cb){ errorCallback_ = std::move(cb); }

  std::shared_ptr<EventLoop> ownerLoop() { return loop_; }
  int fd() const { return fd_; }
  int events() const { return events_; }
  States state() const {return state_;}
  void set_revents(int revt) { revents_ = revt; }
  void set_state(States s){state_=s;}
  bool isNoneEvent() const { return events_ == kNoneEvent; }
  bool isWriting() const { return events_ & kWriteEvent; }
  bool isReading() const { return events_ & kReadEvent; }

  void enableReading() { events_ |= kReadEvent; update(); }
  void disableReading() { events_ &= ~kReadEvent; update(); }
  void enableWriting() { events_ |= kWriteEvent; update(); }
  void disableWriting() { events_ &= ~kWriteEvent; update(); }
  void disableAll() { events_ = kNoneEvent; update(); }
  
  // for debug
  string reventsToString() const;
  string eventsToString() const;

  void remove();

 private:
  static string eventsToString(int fd, int ev);

  void update();

  static const int kNoneEvent;
  static const int kReadEvent;
  static const int kWriteEvent;

  std::shared_ptr<EventLoop> loop_;
  const int  fd_;
  int        events_;
  int        revents_;


  States state_;

  ReadEventCallback readCallback_;
  EventCallback writeCallback_;
  EventCallback closeCallback_;
  EventCallback errorCallback_;
};

}  // namespace net
} 

#endif 
