#pragma once
#include "Channel.h"
#include "HttpData.h"
#include "Timer.h"
#include <vector>
#include <unordered_map>
#include <sys/epoll.h>
#include <memory>

class Epoll {
    public:
    Epoll();
    ~Epoll();
    //注册新的文件描述符
    void epoll_add(SP_Channel request, int timeout);
    //修改文件描述符
    void epoll_mod(SP_Channel request, int timeout);
    //删除文件描述符
    void epoll_del(SP_Channel request);
    //核心函数，epoll_wait
    std::vector<std::shared_ptr<Channel>> poll();
    //获取事件的响应，分发处理函数，加入线程池
    std::vector<std::shared_ptr<Channel>> getEventsRequest(int events_num);
    //定时器加入
    void add_timer(std::shared_ptr<Channel> request_data, int timeout);
    //获取epoll事件
    int getEpollFd()
    {
        return epollFd_;
    }
    //处理超时事件
    void handleExpired();

    private:
    static const int MAXFDS = 100000;
    int epollFd_;
    std::vector<epoll_event> events_;
    std::shared_ptr<Channel> fd2chan_[MAXFDS];
    std::shared_ptr<HttpData> fd2http_[MAXFDS];
    TimerManager timerManager_;
};