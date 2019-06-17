#pragma once
#include "EventLoop.h"
#include "Channel.h"
#include "EventLoopThreadPool.h"
#include <memory>

class Server {
    public:
    //利用事件，线程个数，端口对服务端进行初始化，里面调用回调函数
    Server(EventLoop *loop, int threadNum, int port);
    //空的析构函数
    ~Server() {}
    //获取当前的事件循环
    EventLoop* getloop() const {return loop_;}
    //启动服务端
    void start();
    //处理新的连接
    void handNewConn();
    //处理当前连接，修改注册的事件(mod模式)
    void handThisConn() {return loop_->updatePoller(acceptChannel_);}


    private:
    EventLoop *loop_;
    int threadNum_;
    std::unique_ptr<EventLoopThreadPool> eventLoopThreadPool_;
    bool started_;
    std::shared_ptr<Channel> acceptChannel_;
    int port_;
    int listenFd_;
    static const int MAXFDS = 100000;
};