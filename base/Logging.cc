#include "../base/Logging.h"

#include "../base/CurrentThread.h"
#include "../base/Timestamp.h"

#include<sys/time.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <thread>
#include <sstream>

namespace web
{
using string=std::string;
__thread char t_errnobuf[512];
__thread char t_time[64];
__thread time_t t_lastSecond;

const char* strerror_tl(int savedErrno)
{
  return strerror_r(savedErrno, t_errnobuf, sizeof t_errnobuf);
}

Logger::LogLevel initLogLevel()
{
  if (::getenv("LOG(TRACE)"))
    return Logger::TRACE;
  else if (::getenv("LOG(DEBUG)"))
    return Logger::DEBUG;
  else
    return Logger::INFO;
}

Logger::LogLevel g_logLevel = initLogLevel();

const char* LogLevelName[Logger::NUM_LOG_LEVELS] =
{
  "TRACE ",
  "DEBUG ",
  "INFO  ",
  "WARN  ",
  "ERROR ",
  "FATAL ",
};

void defaultOutput(const char* msg, int len)
{
  fwrite(msg, 1, len, stdout);
}

void defaultFlush()
{
  fflush(stdout);
}

Logger::OutputFunc g_output = defaultOutput;
Logger::FlushFunc g_flush = defaultFlush;


}  

using namespace web;

void Logger::formatTime()
{
  int64_t microSecondsSinceEpoch = time_.microSecondsSinceEpoch();
  time_t seconds = static_cast<time_t>(microSecondsSinceEpoch / Timestamp::kMicroSecondsPerSecond);
  int microseconds = static_cast<int>(microSecondsSinceEpoch % Timestamp::kMicroSecondsPerSecond);
  if (seconds != t_lastSecond)
  {
    t_lastSecond = seconds;
    struct tm tm_time;
      if (!time_.valid())
    {
      ::gmtime_r(&seconds, &tm_time);
    }
    

    int len = snprintf(t_time, sizeof(t_time), "%4d%02d%02d %02d:%02d:%02d",
        tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
        tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
    assert(len == 17); (void)len;
  }

}



Logger::Logger(string file, int line, const char* func,int savedErrno,LogLevel level)
  : level_(level), basename_(file), line_(line),time_(Timestamp::now()),func_(func)
{
  stream_ << time_ .toFormattedString()<< " ";
  stream_ << LogLevelName[level] << " ";
  stream_ << func << ' ';
  if (savedErrno != 0) {
    stream_ << strerror_tl(savedErrno) << " (errno=" << savedErrno << ") ";
  }
}

Logger::Logger(string file, int line, bool toAbort)
  : level_(toAbort?FATAL:ERROR),basename_(file), line_(line),time_(Timestamp::now())
  {
    stream_ << time_ .toFormattedString()<< " ";
  }

Logger::~Logger()
{
  stream_ << " - " << basename_ << ':' << line_ << '\n';
  const LogStream::Buffer& buf(stream_.buffer());
  g_output(buf.data(), buf.length());
  if (level_ == FATAL)
  {
    g_flush();
    abort();
  }
}

void Logger::setLogLevel(Logger::LogLevel level)
{
  g_logLevel = level;
}

void Logger::setOutput(OutputFunc out)
{
  g_output = out;
}

void Logger::setFlush(FlushFunc flush)
{
  g_flush = flush;
}
