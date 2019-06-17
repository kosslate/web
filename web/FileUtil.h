#pragma once
#include "noncopyable.h"
#include <string>

class AppendFile : noncopyable {
    public:
    //打开log文件
    explicit AppendFile(std::string filename);
    //关闭log文件
    ~AppendFile();

    //向文件写数据
    void append(const char *logline, const size_t len);
    void flush();

    private:
    size_t write(const char* logline, size_t len);
    FILE* fp_;
    char buffer_[64 * 1024];
};