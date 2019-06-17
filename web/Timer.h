#pragma once
#include "HttpData.h"
#include "noncopyable.h"
#include "MutexLock.h"
#include <unistd.h>
#include <memory>
#include <queue>
#include <deque>

class HttpData;

class TimerNode {
    public:
    //创建时间节点，输入响应数据，超时时间
    TimerNode(std::shared_ptr<HttpData> requestData, int timeout);
    //析构函数
    ~TimerNode();
    //
    TimerNode(TimerNode &tn);
    //根据超时时间更新
    void update(int timeout);
    //节点有效
    bool isValid();
    //清空响应
    void clearReq();
    //设置删除
    void setDeleted() { deleted_ = true;}
    //判读当前节点待删除
    bool isDeleted() const { return deleted_; }

    size_t getExpTime() const { return expiredTime_; }

    private:
    bool deleted_;
    size_t expiredTime_;
    std::shared_ptr<HttpData> SPHttpData;
};

struct TimerCmp {
    bool operator()(std::shared_ptr<TimerNode> &a, 
        std::shared_ptr<TimerNode> &b) {
            return a->getExpTime() > b->getExpTime();
    }
};

class TimerManager {
    public:
    TimerManager();
    ~TimerManager();
    void addTimer(std::shared_ptr<HttpData> SPHttpData, int timeout);
    void handleExpiredEvent();

    private:
    typedef std::shared_ptr<TimerNode> SPTimerNode;
    //利用优先队列进行存储,从大到小排序
    std::priority_queue
        <SPTimerNode, std::deque<SPTimerNode>, 
        TimerCmp> timerNodeQueue;
};