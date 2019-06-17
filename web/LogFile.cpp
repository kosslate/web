#include "LogFile.h"
#include "FileUtil.h"
#include <assert.h>
#include <stdio.h>
#include <time.h>

using namespace std;

//初始化日志文件
LogFile::LogFile(const string &basename, int flushEveryN) : 
    basename_(basename), flushEveryN_(flushEveryN), count_(0),
    mutex_(new MutexLock) {
        file_.reset(new AppendFile(basename));
}

LogFile::~LogFile() {  }

//互斥器加锁，然后无锁增加数据
//在muduo中，判断了互斥器是否为空，实际都执行了下面的操作
void LogFile::append(const char* Logline, int len) {
    MutexLockGuard lock(*mutex_);
    append_unlocked(Logline, len);
}

void LogFile::flush() {
    MutexLockGuard lock(*mutex_);
    file_->flush();
}

//无锁增加数据
void LogFile::append_unlocked(const char *logline, int len) {
    file_->append(logline, len);
    ++count_;
    //字节数大于刷新字节，将缓冲区的数据写入文件中
    if (count_ >= flushEveryN_) {
        count_ = 0;
        file_->flush();
    }
}