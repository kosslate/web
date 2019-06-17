#pragma once
#include "CountDownLatch.h"
#include "noncopyable.h"
#include <functional>
#include <memory>
#include <thread>
#include <pthread.h>
#include <string>
#include <sys/syscall.h>
#include <unistd.h>


class Thread : noncopyable 
{
    public:
    //定义线程回调函数
    typedef std::function<void()> ThreadFunc;
    //线程函数
    explicit Thread(const ThreadFunc&, const std::string& name = std::string());
    //析构函数
    ~Thread();
    //启动
    void start();
    //分离线程0
    int join();
    //判断是否启动
    bool started() const { return started_; }
    //返回线程数pid
    pid_t tid() const { return tid_; }
    //名字
    const std::string& name() const { return name_; }
    //获取线程id
    /*std::thread::id getThreadId() const{ 
        assert(started_); 
        return pthreadId_->get_id(); 
    }*/
    private:
    //设置默认名字
    void setDefaultName();
    bool started_;
    bool joined_;
    //std::thread::id pthreadId_;
    //std::unique_ptr<std::thread> p_thread;
    //线程id
    pthread_t pthreadId_;
    //进程数
    pid_t tid_;
    //回调函数
    ThreadFunc func_;
    std::string name_;
    CountDownLatch latch_;
};