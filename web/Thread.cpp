#include "Thread.h"
#include "CurrentThread.h"
#include <memory>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/prctl.h>
#include <sys/types.h>
#include <linux/unistd.h>
#include <stdint.h>
#include <assert.h>

#include <iostream>

using namespace std;

namespace CurrentThread {
    __thread int t_cachedTid = 0;
    __thread char t_tidString[32];
    __thread int t_tidStringLength = 6;
    __thread const char* t_threadName = "default";
}

pid_t gettid() {
    return static_cast<pid_t>(::syscall(SYS_gettid));
}

void CurrentThread::cacheTid() {
    if (t_cachedTid == 0) {
        t_cachedTid = gettid();
        t_tidStringLength = snprintf(t_tidString, sizeof t_tidString, "%5d ", t_cachedTid);
    }
}

//在线程中保留name,tid数据
struct ThreadData {
    typedef Thread::ThreadFunc ThreadFunc;
    ThreadFunc func_;
    string name_;
    pid_t* tid_;
    CountDownLatch* latch_;

    ThreadData(const ThreadFunc &func, const string& name, pid_t *tid, CountDownLatch *latch)
    :   func_(func),
        name_(name),
        tid_(tid),
        latch_(latch) { }
    
    void runInThread() {
        *tid_ = CurrentThread::tid();
        tid_ = nullptr;
        latch_->countDown();
        latch_ = nullptr;

        CurrentThread::t_threadName = name_.empty() ? "Thread" : name_.c_str();
        prctl(PR_SET_NAME, CurrentThread::t_threadName);
        func_();
        CurrentThread::t_threadName = "finished";
    }
};

void *startThread(void *obj) {
    ThreadData *data = static_cast<ThreadData*>(obj);
    data->runInThread();
    delete data;
    return nullptr;
}

Thread::Thread(const ThreadFunc &func, const string &n) :
    started_(false), joined_(false), tid_(0),pthreadId_(0),
    func_(func), name_(n), latch_(1) {
        setDefaultName();
}

Thread::~Thread() {
    if (started_ && !joined_) {
        pthread_detach(pthreadId_);
    }
}

void Thread::setDefaultName() {
    if (name_.empty()) {
        char buf[32];
        snprintf(buf, sizeof buf, "Thread");
        name_ = buf;
    }
}

void Thread::start() {
    assert(!started_);
    started_ = true;
    ThreadData *data = new ThreadData(func_, name_, &tid_, &latch_);
    //startThread(data);
    //p_thread.reset(new std::thread(func_));
    //latch_.wait();
    //assert(tid_ > 0);
    if (pthread_create(&pthreadId_, nullptr, &startThread, data)) {
        started_ = false;
        delete data;
    }
    else {
        latch_.wait();
        assert(tid_ > 0);
    }
}

int Thread::join()
{
    assert(started_);
    assert(!joined_);
    joined_ = true;
    return pthread_join(pthreadId_, nullptr);
}

