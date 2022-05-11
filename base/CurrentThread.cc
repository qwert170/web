#include "../base/CurrentThread.h"

#include <cxxabi.h>
#include <execinfo.h>
#include <stdlib.h>
#include <sys/prctl.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>

namespace web
{
pid_t gettid()
{
  return static_cast<pid_t>(::syscall(SYS_gettid));
} 
namespace CurrentThread
{
__thread int t_cachedTid = 0;

void cacheTid() {
  if (t_cachedTid == 0) {
    t_cachedTid = gettid();
  }
}
}  
} 
