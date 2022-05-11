#ifndef ASYNCLOGGING_H
#define ASYNCLOGGING_H

#include "../base/BlockingQueue.h"
#include "../base/BoundedBlockingQueue.h"

#include <mutex>
#include "../base/Thread.h"
#include "../base/LogStream.h"
#include <condition_variable>
#include <atomic>
#include <vector>

namespace web
{

class AsyncLogging : noncopyable
{
 public:

  AsyncLogging(const string& basename,off_t rollSize,int flushInterval = 3);
  ~AsyncLogging()
  {
    if (running_)
    {
      stop();
    }
  }

  void append(const char* logline, int len);

  void start()
  {
    running_ = true;
    thread_.start();
  }

  void stop()
  {
    running_ = false;
    cond_.notify_one();
    thread_.join();
  }

 private:

  void threadFunc();

  typedef web::detail::FixedBuffer<web::detail::kLargeBuffer> Buffer;
  typedef std::vector<std::unique_ptr<Buffer>> BufferVector;
  typedef BufferVector::value_type BufferPtr;

  const int flushInterval_;
  std::atomic<bool> running_;
  const string basename_;
  const off_t rollSize_;
  Thread thread_;
  std::mutex mutex_;
  std::condition_variable cond_ ;
  BufferPtr currentBuffer_ ;
  BufferPtr nextBuffer_ ;
  BufferVector buffers_ ;
};

} 

#endif  
