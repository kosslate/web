#pragma once
#include "Condition.h"
#include "MutexLock.h"
#include "noncopyable.h"

//确保Thread中传进去的func真的启动以后，外层的start才返回

class CountDownLatch : noncopyable {
    public:
    explicit CountDownLatch(int count);
    void wait();
    void countDown();

    private:
    mutable MutexLock mutex_;
    Condition condition_;
    int count_;
};