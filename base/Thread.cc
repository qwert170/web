#include "../base/Thread.h"
#include "../base/CurrentThread.h"
#include <exception>
#include "../base/Logging.h"

#include <type_traits>
#include <cassert>
#include <string>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/prctl.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <linux/unistd.h>
#include <thread>

namespace web
{
std::atomic<int> Thread::numCreated_(0);

Thread::Thread(ThreadFunc &&func)
  : started_(false),
    joined_(false),
    pthreadId_(0),
    func_(std::move(func))
{
    numCreated_++;
}

Thread::~Thread()
{
  if (started_ && !joined_)
  {
    pthread_detach(pthreadId_);
  }
}

struct ThreadData
{
  typedef web::Thread::ThreadFunc ThreadFunc;
  ThreadFunc func_;
  pid_t* tid_;
  std::promise<void> &p_;
  ThreadData(ThreadFunc&& func,pid_t* tid,std::promise<void> &p)
    : func_(func),tid_(tid),p_(p){}
};

void *runInThread(void *arg)//调用回调函数
{
  ThreadData *td = (ThreadData *)arg;
  *td->tid_ = gettid();
  td->p_.set_value();
  try {
    td->func_();
  } catch (const std::exception &e) {
    fprintf(stderr, "%s\n", e.what());
    abort();
  }catch(...)
  {
    fprintf(stderr, "unknown exception caught in Thread %ld\n", (long int)*td->tid_);
    throw; 
  }
  return NULL;
  //日志输入时的名称
}

void Thread::start()
{
  assert(!started_);
  started_ = true;
  std::future<void> f = p_.get_future();
  ThreadData* td = new ThreadData(std::move(func_),&tid_, p_);
  if (pthread_create(&pthreadId_, NULL, runInThread, td))
  {
    started_ = false;
    delete td; 
    //log
    //LOG_SYSFATAL << "Failed in pthread_create";
  }else{
    started_ = true;
    f.get();
    assert(tid_ > 0);
  }
}

int Thread::join()
{
  assert(started_);
  assert(!joined_);
  joined_ = true;
  return pthread_join(pthreadId_, NULL);
}

} 
