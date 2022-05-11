#ifndef EVENTLOOPTHREAD_H
#define EVENTLOOPTHREAD_H

#include <mutex>
#include <condition_variable>
#include "EventLoop.h"
#include "../base/Thread.h"
namespace web
{
namespace net
{
class EventLoop;

class EventLoopThread : noncopyable
{
 public:
  typedef std::function<void(const std::shared_ptr<EventLoop>&)> ThreadInitCallback;

  EventLoopThread(const ThreadInitCallback& cb = ThreadInitCallback());
  ~EventLoopThread();
  std::shared_ptr<EventLoop> getLoop() { return loop_; }
  std::shared_ptr<EventLoop> startLoop();
 private:
  void threadFunc();

  std::shared_ptr<EventLoop> loop_ ;
  bool exiting_;
  Thread thread_;
  std::mutex mutex_;
  std::condition_variable cond_ ;
  ThreadInitCallback callback_;
};

}  // namespace net
}  

#endif  

