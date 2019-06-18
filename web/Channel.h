#pragma once
#include "Timer.h"
#include <string>
#include <unordered_map>
#include <memory>
#include <sys/epoll.h>
#include <functional>
#include <sys/epoll.h>

class EventLoop;
class HttpData;

class Channel {
    private:
    typedef std::function<void()> CallBack;
    EventLoop *loop_;
    int fd_;

    __uint32_t events_;
    __uint32_t revents_;//活跃事件
    __uint32_t lastEvents_;

    // 方便找到上层持有该Channel的对象
    std::weak_ptr<HttpData> holder_;

    private:
    //解析字符串/需要修改为muduo中的vector<char>类型数据
    int parse_URI();
    int parse_Headers();
    int analysisRequest();

    //主要的回调函数，读，写，错误，连接
    CallBack readHandler_;
    CallBack writeHandler_;
    CallBack errorHandler_;
    CallBack connHandler_;

    public:
    Channel(EventLoop *loop);
    Channel(EventLoop *loop, int fd);
    ~Channel();
    int getFd();
    void setFd(int fd);


    void setHolder(std::shared_ptr<HttpData> holder)
    {
        holder_ = holder;
    }
    //获取共享指针
    std::shared_ptr<HttpData> getHolder()
    {
        std::shared_ptr<HttpData> ret(holder_.lock());
        return ret;
    }

    //设置回调函数
    void setReadHandler(CallBack &&readHandler)
    {
        readHandler_ = readHandler;
    }
    void setWriteHandler(CallBack &&writeHandler)
    {
        writeHandler_ = writeHandler;
    }
    void setErrorHandler(CallBack &&errorHandler)
    {
        errorHandler_ = errorHandler;
    }
    void setConnHandler(CallBack &&connHandler)
    {
        connHandler_ = connHandler;
    }

    void handleEvents() {
        events_ =0;
        //如果当前EPOLL的事件是对方已经关闭且没有数据到来
        if ((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN)) {
            //当前事件为０，返回
            events_ = 0;
            return;
        }
        //如果当前EPOLL活跃的事件是错误
        if (revents_ & EPOLLERR) {
            //回调函数不为空，错误处理
            if (errorHandler_) {
                errorHandler_();
            }
            events_ = 0;
            return;
        }
        //如果当前活跃的事件是数据到来，带外数据，对端正常关闭/检测关闭
        if (revents_ & (EPOLLIN | EPOLLPRI | EPOLLRDHUP)) {
            handleRead();
        }
        //如果当前活跃的事件是写数据
        if (revents_ & EPOLLOUT) {
            handleWrite();
        }
        //其他情况需要连接
        handleConn();
    }

    //四个核心处理函数
    void handleRead();
    void handleWrite();
    void handleError(int fd, int err_num, std::string short_msg);
    void handleConn();

    //设置事件
    void setRevents(__uint32_t ev) {
        revents_ = ev;
    }
    void setEvents(__uint32_t ev) {
        events_ = ev;
    }

    //获取事件
    __uint32_t& getEvents() {
        return events_;
    }
    bool EqualAndUpdateLastEvents() {
        bool ret = (lastEvents_ == events_);
        lastEvents_ = events_;
        return ret;
    }
    __uint32_t getLastEvents() {
        return lastEvents_;
    }
};
//
typedef std::shared_ptr<Channel> SP_Channel;