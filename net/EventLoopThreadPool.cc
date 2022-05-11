#include "../net/EventLoopThreadPool.h"
#include "../net/EventLoop.h"
#include "../net/EventLoopThread.h"

#include <stdio.h>

using namespace web;
using namespace web::net;

EventLoopThreadPool::EventLoopThreadPool(std::shared_ptr<EventLoop> baseLoop)
  : baseLoop_(baseLoop),
    started_(false),
    numThreads_(0),
    next_(0)
{}

EventLoopThreadPool::~EventLoopThreadPool(){}

void EventLoopThreadPool::start(const ThreadInitCallback& cb)
{
  assert(!started_);
  baseLoop_->assertInLoopThread();
  started_ = true;

  for (int i = 0; i < numThreads_; ++i)
  {
    EventLoopThread* t = new EventLoopThread(cb);
    threads_.push_back(std::unique_ptr<EventLoopThread>(t));
    loops_.push_back(t->startLoop());
  }
  if (numThreads_ == 0 && cb)
  {
    cb(baseLoop_);
  }
}

std::shared_ptr<EventLoop> EventLoopThreadPool::getNextLoop()
{
  baseLoop_->assertInLoopThread();
  assert(started_);
  std::shared_ptr<EventLoop> loop = baseLoop_;

  if (!loops_.empty())
  {
    loop = loops_[next_];
    ++next_;
    if (implicit_cast<size_t>(next_) >= loops_.size())
    {
      next_ = 0;
    }
  }
  return loop;
}


std::vector<std::shared_ptr<EventLoop>> EventLoopThreadPool::getAllLoops()
{
  baseLoop_->assertInLoopThread();
  assert(started_);
  if (loops_.empty())
  {
    return std::vector<std::shared_ptr<EventLoop>>(1, baseLoop_);
  }
  else
  {
    return loops_;
  }
}
