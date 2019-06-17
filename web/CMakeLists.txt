set(SRCS
    Channel.cpp
    Epoll.cpp
    EventLoop.cpp
    EventLoopThread.cpp
    EventLoopThreadPool.cpp
    HttpData.cpp
    Main.cpp
    Server.cpp
    Timer.cpp
    Util.cpp
    Buffer.cpp
    CountDownLatch.cpp
    AsyncLogging.cpp
    FileUtil.cpp
    LogFile.cpp
    Logging.cpp
    LogStream.cpp
    Thread.cpp
)

add_executable(server ${SRCS})
target_link_libraries(server pthread rt)
