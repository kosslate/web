#pragma once
#include <stdint.h>

namespace CurrentThread {
    //内部实现
    extern __thread int t_cachedTid;
    extern __thread char t_tidString[32];
    extern __thread int t_tidStringLength;
    extern __thread const char* t_threadName;
    void cacheTid();
    //内联函数
    inline int tid() {
        if (__builtin_expect(t_cachedTid == 0, 0)) {
            cacheTid();
        }
        return t_cachedTid;
    }

    //用于logging
    inline const char* tidString() {
        return t_tidString;
    }

    //用于logging
    inline int tidStringLength() {
        return t_tidStringLength;
    }
}