#include "../net/Epoller.h"
#include "../base/Logging.h"
#include "../net/Channel.h"

using namespace web;
using namespace web::net;

Epoller::Epoller(EventLoop* loop):
      ownerLoop_(loop),
      epollfd_(::epoll_create1(EPOLL_CLOEXEC)),
      events_(16)    
{
  if (epollfd_ < 0)
  {
    LOG_SYSFATAL << "EPollPoller::EPollPoller";
  }
}

Epoller::~Epoller()
{
    ::close(epollfd_);
}

Timestamp Epoller::poll(int timeoutMs, ChannelList* activeChannels)
{
  LOG(TRACE) << "fd total count " << channels_.size();
  int numEvents = ::epoll_wait(epollfd_,
                               &*events_.begin(),
                               static_cast<int>(events_.size()),
                               timeoutMs);
  int savedErrno = errno;
  Timestamp now(Timestamp::now());
  if (numEvents > 0)
  {
    LOG(TRACE) << numEvents << " events happened";
    fillActiveChannels(numEvents, activeChannels);
    if (implicit_cast<size_t>(numEvents) == events_.size())
    {
      events_.resize(events_.size()*2);
    }
  }
  else if (numEvents == 0)
  {
    LOG(TRACE) << "nothing happened";
  }
  else
  {
    if (savedErrno != EINTR)
    {
      errno = savedErrno;
      LOG_SYSERR << "Epoller::poll()";
    }
  }
  return now;
}

void Epoller::fillActiveChannels(int numEvents,ChannelList* activeChannels) const
{
  assert(implicit_cast<size_t>(numEvents) <= events_.size());
  for (int i = 0; i < numEvents; ++i)
  {
    Channel* channel = static_cast<Channel*>(events_[i].data.ptr);
#ifndef NDEBUG
    int fd = channel->fd();
    ChannelMap::const_iterator it = channels_.find(fd);
    assert(it != channels_.end());
    assert(it->second == channel);
#endif
    channel->set_revents(events_[i].events);
    activeChannels->push_back(channel);
  }
}

void Epoller::updateChannel(Channel* channel)
{
  Epoller::assertInLoopThread();
  LOG(TRACE) << "fd = " << channel->fd()<< " events = " << channel->events();
  int fd = channel->fd();
  if (channel->state() == Channel::kDisconnected)
  {
    assert(channels_.find(fd) == channels_.end());
    channels_[fd] = channel;
    update(EPOLL_CTL_ADD, channel);
    channel->set_state(Channel::kConnecting);
  }else if(channel->state()==Channel::kConnected)
  {
    if (!channel->isNoneEvent())
    {
      update(EPOLL_CTL_ADD, channel);
      channel->set_state(Channel::kConnecting);
    }
  }else if(channel->state()==Channel::kConnecting)
  {
     if (channel->isNoneEvent()) {
      update(EPOLL_CTL_DEL, channel);
      channel->set_state(Channel::kConnected);
    } else {
      update(EPOLL_CTL_MOD, channel);
    }
  }
}

void Epoller::removeChannel(Channel* channel)
{
  Epoller::assertInLoopThread();
  int fd = channel->fd();
  LOG(TRACE) << "fd = " << fd;

  assert(channels_.find(fd) != channels_.end());
  assert(channels_[fd] == channel);
  assert(channel->isNoneEvent());
  
  Channel::States s = channel->state();
  size_t n = channels_.erase(fd);
  (void)n;
  assert(n == 1);

  if (s == Channel::kConnecting)
  {
    update(EPOLL_CTL_DEL, channel);
  }
}

void Epoller::update(int operation, Channel* channel)
{
  struct epoll_event event;
  memZero(&event, sizeof event);
  event.events = channel->events();
  event.data.ptr = channel;
  int fd = channel->fd();
  LOG(TRACE) << "epoll_ctl op = " << operationToString(operation)
    << " fd = " << fd << " event = { " << channel->eventsToString() << " }";
  if (::epoll_ctl(epollfd_, operation, fd, &event) < 0)
  {
    if (operation == EPOLL_CTL_DEL)
    {
      LOG_SYSERR << "epoll_ctl op =" << operationToString(operation) << " fd =" << fd;
    }
    else
    {
      LOG_SYSFATAL << "epoll_ctl op =" << operationToString(operation) << " fd =" << fd;
    }
  }
}

const char* Epoller::operationToString(int op)
{
  switch (op)
  {
    case EPOLL_CTL_ADD:return "ADD";
    case EPOLL_CTL_DEL:return "DEL";
    case EPOLL_CTL_MOD:return "MOD";
    default:
      assert(false && "ERROR op");
      return "Unknown Operation";
  }
}

bool Epoller::hasChannel(Channel* channel) const
{
  assertInLoopThread();
  ChannelMap::const_iterator it = channels_.find(channel->fd());
  return it != channels_.end() && it->second == channel;
}

