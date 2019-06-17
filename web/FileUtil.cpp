#include "FileUtil.h"
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

using namespace std;

AppendFile::AppendFile(string filename) : 
    fp_(fopen(filename.c_str(), "ae")) {
        //设置缓冲区大小
        setbuffer(fp_, buffer_, sizeof buffer_);
}

AppendFile::~AppendFile() {
    //关闭文件
    fclose(fp_);
}

void AppendFile::append(const char *logline, const size_t len) {
    size_t n = this->write(logline, len);
    size_t remain = len - n;
    //写入全部的数据
    while (remain > 0) {
        size_t x = this->write(logline + n, remain);
        if (x == 0) {
            int err = ferror(fp_);
            if (err) {
                fprintf(stderr, "AppendFile::append() fail !\n");
            }
            break;            
        }
        n += x;
        remain = len - n;
    }
}

void AppendFile::flush() {
    //将缓冲区数据写入文件
    fflush(fp_);
}

size_t AppendFile::write(const char *logline, size_t len) {
    //从始至终只有一个线程能够访问，使用无锁的write进行加速
    return fwrite_unlocked(logline, 1, len , fp_);
}