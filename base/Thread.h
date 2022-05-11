#ifndef THREAD_H
#define HREAD_H
//线程
#include <atomic>
#include "../base/Types.h"
#include "../base/noncopyable.h"
#include <functional>
#include <memory>
#include <string>
#include <future>

namespace web
{
using std::string;

class Thread : noncopyable
{
 public:
  typedef std::function<void ()> ThreadFunc;

  explicit Thread(ThreadFunc &&func);
  ~Thread();

  void start();//启动线程
  int join(); // return pthread_join()

  bool started() const { return started_; }//是否启动

  pid_t tid() const { return tid_; }

  static int numCreated() { return numCreated_.load(); }//创建的个数

 private:

  bool       started_;//线程是否启动
  bool       joined_;
  pthread_t  pthreadId_;//线程id
  pid_t      tid_;//真实id
  ThreadFunc func_;//线程回调的函数
  std::promise<void> p_;
  static std::atomic<int> numCreated_;//新创建线程的个数
};

}  
#endif 
