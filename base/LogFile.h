
#ifndef LOGFILE_H
#define LOGFILE_H

#include "../base/Types.h"
#include "../base/noncopyable.h"
#include <memory>
#include <string>
#include <mutex>
#include <sys/types.h>
namespace web
{
using std::string;
namespace FileUtil
{
class AppendFile;
}

class LogFile : noncopyable
{
 public:
  LogFile(const string& basename,
          off_t rollSize,
          int flushInterval = 3,
          int checkEveryN = 1024);
  ~LogFile();

  void append(const char* logline, int len);
  void flush();
  bool rollFile();

 private:
  void append_unlocked(const char* logline, int len);//不加锁的append方式

  static string getLogFileName(const string& basename, time_t* now);//获取日志文件的名称

  const string basename_;
  const off_t rollSize_;//日志文件达到rolsize生成一个新文件
  const int flushInterval_;//日志写入间隔时间
  const int checkEveryN_;// 每调用checkEveryN_次日志写，就滚动一次日志

  int count_;// 写入的次数

  std::unique_ptr<std::mutex> mutex_;
  time_t startOfPeriod_;//开始记录日志时间
  time_t lastRoll_;//上一次滚动日志文件时间
  time_t lastFlush_;	//上一次日志写入文件时间
  std::unique_ptr<FileUtil::AppendFile> file_;//文件智能指针

  const static int kRollPerSeconds_ = 60*60*24;
};

} 
#endif  
