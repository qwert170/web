#ifndef TYPES_H
#define TYPES_H
//type类转换
#include <stdint.h>
#include <string.h>  // memset
#include <string>

#ifndef NDEBUG
#include <assert.h>
#endif

namespace web
{

using std::string;

inline void memZero(void* p, size_t n)
{
  memset(p, 0, n);
}

template<typename To, typename From>
inline To implicit_cast(From const &f)
{
  return f;
}

template<typename To, typename From>   
inline To down_cast(From* f)                  
{
  if (false)
  {
    implicit_cast<From*, To>(0);
  }

#if !defined(NDEBUG) && !defined(GOOGLE_PROTOBUF_NO_RTTI)
  assert(f == NULL || dynamic_cast<To>(f) != NULL);  
#endif
  return static_cast<To>(f);
}

}

#endif 
