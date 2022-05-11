
#ifndef BOUNDEDBLOCKINGQUEUE_H
#define BOUNDEDBLOCKINGQUEUE_H
//有界缓冲区
#include <mutex>
#include <condition_variable>
#include "noncopyable.h"
#include "../base/Types.h"
#include <assert.h>
#include <functional>
#include <boost/circular_buffer.hpp>

namespace web
{

template<typename T>
class BoundedBlockingQueue : noncopyable
{
 public:
  explicit BoundedBlockingQueue(int maxSize):mutex_(),queue_(maxSize){}

  void put(const T& x)
  {
    std::unique_lock<std::mutex> lock(mutex_);
    while (queue_.full())
    {
      notFull_.wait(lock);
    }
    assert(!queue_.full());
    queue_.push_back(x);
    notEmpty_.notify_one();
  }

  void put(T&& x)
  {
    std::unique_lock<std::mutex> lock(mutex_);
    while (queue_.full())
    {
      notFull_.wait(lock);
    }
    assert(!queue_.full());
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
    notFull_.notify_one();
    return front;
  }

  bool empty() const
  {
    std::unique_lock<std::mutex> lock(mutex_);
    return queue_.empty();
  }

  bool full() const
  {
    std::unique_lock<std::mutex> lock(mutex_);
    return queue_.full();
  }

  size_t size() const//个数
  {
    std::unique_lock<std::mutex> lock(mutex_);
    return queue_.size();
  }

  size_t capacity() const//容量
  {
    std::unique_lock<std::mutex> lock(mutex_);
    return queue_.capacity();
  }

 private:
  mutable std::mutex          mutex_;
  std::condition_variable     notEmpty_ ;
  std::condition_variable     notFull_ ;
  boost::circular_buffer<T>   queue_ ;
};

}

#endif