#ifndef LOGGING_H
#define LOGGING_H

#include "../base/LogStream.h"
#include "../base/Timestamp.h"
#include <string>
#include <functional>

namespace web
{
using std::string;
class Logger
{
 public:
  enum LogLevel
  {
    TRACE,DEBUG,INFO,WARN,ERROR,FATAL,NUM_LOG_LEVELS,
  };

  Logger(string file, int line,const char* func,int savedErrno,LogLevel level );
  Logger(string file, int line, bool toAbort);
  ~Logger();


  static LogLevel logLevel();
  static void setLogLevel(LogLevel level);

  typedef void (*OutputFunc)(const char* msg, int len);
  typedef void (*FlushFunc)();
  static void setOutput(OutputFunc);
  static void setFlush(FlushFunc);


  typedef Logger::LogLevel LogLevel;
  void formatTime();
  
  Timestamp time_;
  LogStream stream_;
  LogLevel level_;
  int line_;
  string basename_;
  string func_;
  int savedErrno;
};

extern Logger::LogLevel g_logLevel;

inline Logger::LogLevel Logger::logLevel()
{
  return g_logLevel;
}

#define LOG(level)                                                \
  if(web::Logger::level>=web::g_logLevel)                         \
  web::Logger(__FILE__,__LINE__,__func__,                         \
              (web::Logger::level==web::Logger::ERROR||           \
              web::Logger::level==web::Logger::FATAL)             \
              ?errno:0,web::Logger::level).stream_     

#define LOG_SYSERR web::Logger(__FILE__, __LINE__, false).stream_
#define LOG_SYSFATAL web::Logger(__FILE__, __LINE__, true).stream_

const char* strerror_tl(int savedErrno);

}  

#endif 
