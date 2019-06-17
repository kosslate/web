#pragma once
#include "noncopyable.h"
#include <assert.h>
#include <string.h>
#include <string>

class AsyncLogging;
const int kSmallBuffer = 4000;
const int kLargeBuffer = 4000 * 1000;

template<int SIZE>
class FixedBuffer : noncopyable {
    public:
    //初始化数据
    FixedBuffer() : cur_(data_) { }
    ~FixedBuffer() { }
    //增加数据
    void append(const char* buf, size_t len) {
        //判断有效数据长度和增加数据长度
        if (avail() > static_cast<int>(len)) {
            //将buf拷贝到cur_中
            memcpy(cur_, buf, len);
            //长度增加
            cur_ += len;
        }
    }

    //获取数据
    const char*  data() const { return data_; }
    //获取长度，增加多少数据
    int length() const { return static_cast<int>(cur_ - data_); }
    //当前数据
    char* current() { return cur_; }
    //判断是否有效
    int avail() const { return static_cast<int>(end() - cur_); }
    //增加长度
    void add(size_t len) { cur_ += len; }
    //初始化为原来的
    void reset() { cur_ = data_; }
    //全部清０
    void bzero() { memset(data_, 0, sizeof data_); }

    private:
    const char* end() const { return data_ + sizeof data_; }
    char data_[SIZE];
    char* cur_; 
};

class LogStream : noncopyable {
    public:
    typedef FixedBuffer<kSmallBuffer> Buffer;

    //重载判断运算符
    LogStream& operator<<(bool v) {
        buffer_.append(v ? "1" : "0", 1);
        return *this;
    }

    //重载数据类型的运算符，左移运算符,各种数据类型
    LogStream& operator<<(short);
    LogStream& operator<<(unsigned short);
    LogStream& operator<<(int);
    LogStream& operator<<(unsigned int);
    LogStream& operator<<(long);
    LogStream& operator<<(unsigned long);
    LogStream& operator<<(long long);
    LogStream& operator<<(unsigned long long);

    LogStream& operator<<(const void*);
    LogStream& operator<<(float v) {
        *this << static_cast<double>(v);
        return *this;
    }

    LogStream& operator<<(double);
    LogStream& operator<<(long double);

    LogStream& operator<<(char v) {
        buffer_.append(&v, 1);
        return *this;
    }

    LogStream& operator<<(const char* str)
    {
        if (str)
            buffer_.append(str, strlen(str));
        else
            buffer_.append("(null)", 6);
        return *this;
    }

    LogStream& operator<<(const unsigned char* str)
    {
        return operator<<(reinterpret_cast<const char*>(str));
    }

    LogStream& operator<<(const std::string& v)
    {
        buffer_.append(v.c_str(), v.size());
        return *this;
    }
    //增加数据
    void append(const char* data, int len) { buffer_.append(data, len); }
    //获取缓冲区的数据
    const Buffer& buffer() const { return buffer_; }
    //还原数据
    void resetBuffer() { buffer_.reset(); }


    private:
    //静态检查
    //void staticCheck();
    //
    template<typename T>
    void formatInteger(T);

    Buffer buffer_;
    //最大的数据长度？？？
    static const int kMaxNumericSize = 32;
};