#include "../net/EventLoopThread.h"
#include "../net/EventLoop.h"

using namespace web;
using namespace web::net;

EventLoopThread::EventLoopThread(const ThreadInitCallback& cb)
  :loop_(NULL),
    exiting_(false),
    thread_(std::bind(&EventLoopThread::threadFunc, this)),
    mutex_(),
    callback_(cb)
{
   if (!loop_) {
    std::unique_lock<std::mutex> lock(mutex_);
    cond_.wait(lock, [&]() { return loop_; });
  }
}

EventLoopThread::~EventLoopThread()
{
  exiting_ = true;
  if (loop_ != NULL) 
  {
    loop_->quit();
    thread_.join();
  }
}

std::shared_ptr<EventLoop> EventLoopThread::startLoop()
{
  assert(!thread_.started());
  thread_.start();

  std::shared_ptr<EventLoop> loop = NULL;
  {
    std::unique_lock<std::mutex> lock(mutex_);
    while (loop_ == NULL)
    {
      cond_.wait(lock);
    }
    loop = loop_;
  }

  return loop;
}

void EventLoopThread::threadFunc()
{
  if (callback_)
  {
    callback_(loop_);
  }
  {
    loop_=std::make_shared<EventLoop>() ;
    std::unique_lock<std::mutex> lock(mutex_);
    cond_.notify_one();
  }
  loop_->loop();
  std::unique_lock<std::mutex> lock(mutex_);
  loop_ = NULL;
}

