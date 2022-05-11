#ifndef CURRENTTHREAD_H
#define CURRENTTHREAD_H
#include <sys/types.h>
namespace web
{
pid_t gettid(); 
namespace CurrentThread
{
  // internal
  //__thread 修饰的变量是线程局部存储的，每个线程都有一份
  //__thread pod类型
  extern __thread int t_cachedTid;  //线程真实pid的缓存
  
  void cacheTid();
  inline int tid()
  {
    if (__builtin_expect(t_cachedTid == 0, 0))//允许程序员将最有可能执行的分支告诉编译器 概率很大
    {
      cacheTid();
    }
    return t_cachedTid;
  }
}  
}  

#endif
