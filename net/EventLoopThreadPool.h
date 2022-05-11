#ifndef EVENTLOOPTHREADPOOL_H
#define EVENTLOOPTHREADPOOL_H

#include "../base/noncopyable.h"
#include "../base/Types.h"
#include "EventLoopThread.h"

#include <functional>
#include <memory>
#include <vector>
#include <cassert>
namespace web
{
namespace net
{
class EventLoop;
class EventLoopThread;

class EventLoopThreadPool : noncopyable
{
 public:
  typedef std::function<void(const std::shared_ptr<EventLoop>&)> ThreadInitCallback;

  EventLoopThreadPool(std::shared_ptr<EventLoop> baseLoop);
  ~EventLoopThreadPool();

  void setThreadNum(int numThreads) { numThreads_ = numThreads; }
  void start(const ThreadInitCallback& cb = ThreadInitCallback());

  std::shared_ptr<EventLoop> getNextLoop();
  std::vector<std::shared_ptr<EventLoop>> getAllLoops();

  bool started() const{ return started_; }

 private:
  std::shared_ptr<EventLoop> baseLoop_;
  bool started_;
  int numThreads_;
  int next_;
  std::vector<std::unique_ptr<EventLoopThread>> threads_;
  std::vector<std::shared_ptr<EventLoop>> loops_;
};
}  // namespace net
}  

#endif 
