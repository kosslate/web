#include "EventLoop.h"
#include "Server.h"
#include "Logging.h"
#include <iostream>


int main(int argc, char *argv[]) {
    int threadNum = 4;
    int port  = 8080;
    std::string logFile = "/home/liu/server.log";
    #ifndef _PTHREADS
        std::cout << "_PTHREADS is not defined !" << std::endl;
    #endif
    Logger::setLogFileName(logFile);
    EventLoop mainloop;
    Server myServer(&mainloop, threadNum, port);
    myServer.start();
    mainloop.loop();
    return 0;
}