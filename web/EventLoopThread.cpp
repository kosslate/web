#include "EventLoopThread.h"
#include <functional>

//事件循环线程
EventLoopThread::EventLoopThread() : loop_(NULL), exiting_(false),
    thread_(std::bind(&EventLoopThread::threadFunc,this), "EventLoopThread"),
    mutex_(), cond_(mutex_) { }

//析构线程，如果当前循环不为空，线程退出，
EventLoopThread::~EventLoopThread() {
    exiting_ = true;
    if (loop_ != NULL) {
        loop_->quit();
        //等到线程执行完毕，再来执行线程
        thread_.join();
    }
}

EventLoop* EventLoopThread::startLoop() {
    assert(!thread_.started());
    //启动线程
    thread_.start();
    {
        MutexLockGuard lock(mutex_);
        // 一直等到threadFun在Thread里真正跑起来
        while (loop_ == NULL) {
            //阻塞等待事件到来
            cond_.wait();
        }
    }
    //返回事件循环
    return loop_;
}


void EventLoopThread::threadFunc() {
    EventLoop loop;
    {
        MutexLockGuard lock(mutex_);
        loop_ = &loop;
        //唤醒至少一个阻塞在条件变量上的线程
        cond_.notify();
    }
    //进行事件循环，真正的执行函数
    loop.loop();
    loop_ = NULL;
}
