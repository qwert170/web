#include "../base/Logging.h"
#include "../net/Channel.h"
#include "../net/EventLoop.h"

#include <sstream>
#include <sys/epoll.h>
#include <poll.h>

using namespace web;
using namespace web::net;

const int Channel::kNoneEvent = EPOLLET;
const int Channel::kReadEvent = EPOLLIN | EPOLLPRI |EPOLLRDHUP;
const int Channel::kWriteEvent = EPOLLOUT;

Channel::Channel(std::shared_ptr<EventLoop> loop, int fd__)
  : loop_(loop),fd_(fd__),events_(kNoneEvent),revents_(0),state_(kDisconnected){}

Channel::~Channel()
{
  if (loop_->isInLoopThread())
  {
    assert(!loop_->hasChannel(this));
  }
}

void Channel::update()
{
  loop_->updateChannel(this);
}

void Channel::remove()
{
  loop_->removeChannel(this);
}

void Channel::handleEvent(Timestamp receiveTime)
{
  LOG(TRACE) << reventsToString();
  if ((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN))
  {
    LOG(WARN) << "fd = " << fd_ << " Channel::handle_event() EPOLLHUP";
    if (closeCallback_) closeCallback_();
  }

  if (revents_ & EPOLLERR)
  {
    LOG(WARN) << "fd = " << fd_ << " Channel::handle_event() EPOLLERR";
    if (errorCallback_) errorCallback_();
  }
  if (revents_ & (EPOLLIN | EPOLLPRI | EPOLLRDHUP))
  {
    if (readCallback_) readCallback_(receiveTime);
  }
  if (revents_ & EPOLLOUT)
  {
    if (writeCallback_) writeCallback_();
  }
}

string Channel::reventsToString() const
{
  return eventsToString(fd_, revents_);
}

string Channel::eventsToString() const
{
  return eventsToString(fd_, events_);
}

string Channel::eventsToString(int fd, int ev)
{
  std::ostringstream oss;
  oss << fd << ": ";
  if (ev & POLLIN)
    oss << "IN ";
  if (ev & POLLPRI)
    oss << "PRI ";
  if (ev & POLLOUT)
    oss << "OUT ";
  if (ev & POLLHUP)
    oss << "HUP ";
  if (ev & POLLRDHUP)
    oss << "RDHUP ";
  if (ev & POLLERR)
    oss << "ERR ";
  if (ev & POLLNVAL)
    oss << "NVAL ";

  return oss.str();
}
