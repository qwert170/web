
#ifndef TIMERQUEUE_H
#define TIMERQUEUE_H

#include <set>
#include <vector>

#include <mutex>
#include "../base/Timestamp.h"
#include "../net/Callbacks.h"
#include "../net/Channel.h"
#include "../base/noncopyable.h"
namespace web
{
namespace net
{

class EventLoop;
class Timer;
class TimerId;

class TimerQueue : noncopyable
{
 public:
  explicit TimerQueue(std::shared_ptr<EventLoop> loop);
  ~TimerQueue();

  TimerId addTimer(TimerCallback cb,Timestamp when,double interval);
  void cancel(TimerId timerId);

 private:

  typedef std::pair<Timestamp, Timer*> Entry;
  typedef std::set<Entry> TimerList;
  typedef std::pair<Timer*, int64_t> ActiveTimer;
  typedef std::set<ActiveTimer> ActiveTimerSet;

  void addTimerInLoop(Timer* timer);
  void cancelInLoop(TimerId timerId);
  // called when timerfd alarms
  void handleRead();
  // move out all expired timers
  std::vector<Entry> getExpired(Timestamp now);
  void reset(const std::vector<Entry>& expired, Timestamp now);

  bool insert(Timer* timer);

  std::shared_ptr<EventLoop> loop_;
  const int timerfd_;
  Channel timerfdChannel_;
  // Timer list sorted by expiration
  TimerList timers_;

  // for cancel()
  ActiveTimerSet activeTimers_;
  bool callingExpiredTimers_; /* atomic */
  ActiveTimerSet cancelingTimers_;
};

}  // namespace net
} 
#endif  
