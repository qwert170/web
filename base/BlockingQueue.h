#ifndef BLOCKINGQUEUE_H
#define BLOCKINGQUEUE_H
//无界缓冲区
#include <mutex>
#include <condition_variable>
#include "noncopyable.h"
#include <deque>
#include <assert.h>

namespace web
{
template<typename T>
class BlockingQueue : noncopyable
{
 public:
  using queue_type = std::deque<T>;

  BlockingQueue():mutex_(),queue_(){}

  void put(const T& x)
  {
    std::unique_lock<std::mutex> lock(mutex_);
    queue_.push_back(x);
    notEmpty_.notify_one(); 
  }

  void put(T&& x)
  {
    std::unique_lock<std::mutex> lock(mutex_);
    queue_.push_back(std::move(x));
    notEmpty_.notify_one();
  }

  T take()
  {
    std::unique_lock<std::mutex> lock(mutex_);
    while (queue_.empty())
    {
      notEmpty_.wait(lock);
    }
    assert(!queue_.empty());
    T front(std::move(queue_.front()));
    queue_.pop_front();
    return front;
  }



  size_t size() const
  {
    std::unique_lock<std::mutex> lock(mutex_);
    return queue_.size();
  }

 private:
  mutable std::mutex        mutex_;
  std::condition_variable   notEmpty_ ;
  queue_type                queue_ ;
};  

} 

#endif  
