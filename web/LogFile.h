#pragma once
#include "FileUtil.h"
#include "MutexLock.h"
#include "noncopyable.h"
#include <memory>
#include <string>


//自动归档功能
class LogFile : noncopyable {
    public:
    //每次append，flushEveryN数据，flush一下，向文件写，文件也是带缓冲区
    LogFile(const std::string& basename, int flushEveryN = 1024);
    ~LogFile();

    void append(const char* logline, int len);
    void flush();
    bool rollFile();

    private:
    void append_unlocked(const char *logline, int len);

    //定义路径，缓存次数
    const std::string basename_;
    const int flushEveryN_;

    int count_;
    //unique_ptr的锁和文件
    std::unique_ptr<MutexLock> mutex_;
    std::unique_ptr<AppendFile> file_;
};