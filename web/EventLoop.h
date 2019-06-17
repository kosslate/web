#pragma once
#include "Thread.h"
#include "Epoll.h"
#include "Logging.h"
#include "Channel.h"
#include "CurrentThread.h"
#include "Util.h"
#include <vector>
#include <memory>
#include <functional>

#include <iostream>
using namespace std;


class EventLoop
{
public:
    //回调函数
    typedef std::function<void()> Functor;
    EventLoop();
    ~EventLoop();
    //核心函数
    void loop();
    //退出事件循环
    void quit();
    //执行
    void runInLoop(Functor&& cb);
    //队列等待执行
    void queueInLoop(Functor&& cb);
    //判断线程号相等
    bool isInLoopThread() const { return threadId_ == CurrentThread::tid(); }
    void assertInLoopThread()
    {
        assert(isInLoopThread());
    }
    void shutdown(shared_ptr<Channel> channel)
    {
        shutdownWR(channel->getFd());
    }
    void removeFromPoller(shared_ptr<Channel> channel)
    {
        poller_->epoll_del(channel);
    }
    void updatePoller(shared_ptr<Channel> channel, int timeout = 0)
    {
        poller_->epoll_mod(channel, timeout);
    }
    void addToPoller(shared_ptr<Channel> channel, int timeout = 0)
    {
        poller_->epoll_add(channel, timeout);
    }
    
private:
    // 声明顺序 wakeupFd_ > pwakeupChannel_
    bool looping_;
    shared_ptr<Epoll> poller_;
    int wakeupFd_;
    bool quit_;
    bool eventHandling_;
    mutable MutexLock mutex_;
    std::vector<Functor> pendingFunctors_;
    bool callingPendingFunctors_;
    const pid_t threadId_; 
    shared_ptr<Channel> pwakeupChannel_;
    
    void wakeup();
    void handleRead();
    void doPendingFunctors();
    void handleConn();
};
