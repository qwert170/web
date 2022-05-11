#ifndef THREADPOOL_H
#define THREADPOOL_H
//线程池
#include "../base/Thread.h"

#include <sys/types.h>
#include <pthread.h>
#include <mutex>
#include <queue>
#include <vector>
#include <string>
#include <condition_variable>
#include <thread>

namespace web
{
class ThreadPool : noncopyable
{
 public:
  using Task=std::function<void ()>;

  inline ThreadPool(size_t threadnum=4);
  inline ~ThreadPool();

  template <class F, class... Args>
  auto addTask(F &&f, Args &&...args) -> std::future<decltype(f(args...))>;
 private:
  

  std::vector<web::Thread> pool;//线程池
  std::queue<Task> tasks ;// 任务队列
  std::mutex m_lock;// 同步锁
  std::condition_variable cv;// 条件阻塞
  std::atomic<bool> stoped;// 是否打开

};

inline ThreadPool::ThreadPool(size_t threadnum):stoped(true)
{
  for(size_t i=0;i<threadnum;i++)
  {
    pool.emplace_back([this]{
      while(this->stoped){
        std::function<void()> task;
        {
          std::unique_lock<std::mutex> lock(this->m_lock);

          this->cv.wait(lock,[this]{
            return !this->stoped.load() || !this->tasks.empty();
          });
          if(!this->stoped &&this->tasks.empty()){return;}
          task=std::move(this->tasks.front());
          this->tasks.pop();
        }
        task();
      }
    });
  }
}

inline ThreadPool::~ThreadPool()
{
  stoped.store(false);
  cv.notify_all();
  for(auto &&thread:pool)
  {
    thread.join();
  }
}

template <class F,class... Args>
auto ThreadPool::addTask(F &&f,Args &&...args)->std::future<decltype(f(args...))>
{
  if(!stoped.load())
  {
    fprintf(stderr, "Thread pool is stoped\n");
    throw;
  }
  
  using type=decltype(f(args...));
  auto task = std::make_shared<std::packaged_task<type()>>(
      std::bind(std::forward<F>(f), std::forward<Args>(args)...));

  std::future<type> future = task->get_future(); 
  {
    std::lock_guard<std::mutex> lock{m_lock};
    tasks.emplace([task]() { (*task)(); });
  }
  cv.notify_one(); 
  return future;
}

}
#endif  
