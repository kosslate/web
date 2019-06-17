#pragma once
#include "noncopyable.h"
#include <pthread.h>
#include <cstdio>
//#include <thread>
//#include <mutex>
//考虑改成c++11版本的互斥锁
class MutexLock : noncopyable {
    public:
    MutexLock() {
        //线程初始化，互斥器为空
        pthread_mutex_init(&mutex, nullptr);
    }
    ~MutexLock() {
        //加锁然后销毁互斥器
        //mutex.unlock();
        //mutex.~mutex();
        pthread_mutex_lock(&mutex);
        pthread_mutex_destroy(&mutex);
    }
    void lock() {
        //对互斥器加锁
        //mutex.lock();
        pthread_mutex_lock(&mutex);
    }
    void unlock() {
        //对互斥器解锁
        //mutex.unlock();
        pthread_mutex_unlock(&mutex);
    }
    /*std::mutex &get() {
        return mutex;
    }*/
    pthread_mutex_t *get() {
        //获取互斥器
        return &mutex;
    }

    private:
    //std::mutex mutex;
    pthread_mutex_t mutex;

    private:
    //友元类不受访问权限影响
    friend class Condition;
};

class MutexLockGuard : noncopyable {
    public:
    //构造函数，互斥器构造
    explicit MutexLockGuard(MutexLock &_mutex) : mutex(_mutex) {
        mutex.lock();
    }
    ~MutexLockGuard() {
        mutex.unlock();
    }
    private:
    MutexLock &mutex;
};