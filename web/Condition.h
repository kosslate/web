#pragma once
#include "noncopyable.h"
#include "MutexLock.h"
//#include <mutex>
//#include <chrono>
//#include <condition_variable>
#include <pthread.h>
#include <errno.h>
#include <cstdint>
#include <time.h>


class Condition : noncopyable {
    public:
    //考虑之后转换成c++11条件变量
    explicit Condition(MutexLock &_mutex) : mutex(_mutex) {
        pthread_cond_init(&cond, nullptr);
    }
    ~Condition() {
        pthread_cond_destroy(&cond);
    }
    void wait() {
        //cond.wait(mutex.get());
        pthread_cond_wait(&cond, mutex.get());
    }
    void notify() {
        //cond.notify_one();
        pthread_cond_signal(&cond);
    }
    void notifyAll() {
        //cond.notify_all();
        pthread_cond_broadcast(&cond);
    }
    //输入超时时间，如果超时则返回真
    bool waitForSeconds(int seconds) {
        struct timespec abstime;
        //实时时间
        clock_gettime(CLOCK_REALTIME, &abstime);
        abstime.tv_sec += static_cast<time_t>(seconds);
        return ETIMEDOUT == pthread_cond_timedwait(&cond, mutex.get(), &abstime);
    }

    /*void waitForSeconds(double seconds) {
        const int64_t kNanoSecondsPerSecond = 1000000000;
        int64_t nannoseconds = static_cast<int64_t>(seconds * kNanoSecondsPerSecond);
        cond.wait_for(mutex, std::chrono::nanoseconds(nannoseconds));
    }*/

    private:
    MutexLock &mutex;
    //std::condition_variable_any cond;
    pthread_cond_t cond;
};