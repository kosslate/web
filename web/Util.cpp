#include "Util.h"
#include <unistd.h>  //close open 函数
#include <fcntl.h>
#include <signal.h>
//#include <errno.h>
#include <string.h> //bzero
#include <sys/socket.h> //socket
#include <netinet/in.h> //sockaddr_in
#include <netinet/tcp.h>
#include <sys/uio.h>



void shutdownWR(int fd)
{
    shutdown(fd, SHUT_WR);
    //printf("shutdown\n");
}

ssize_t readn(int fd, void *buff, size_t n) {
   return read(fd, buff, n);
}
ssize_t readvn(int sockfd, const struct iovec *iov, int iovcnt) {
    return readv(sockfd, iov, iovcnt);
}


ssize_t writen(int fd, const void *buf, size_t count) {
    return write(fd, buf, count);
}


void setSocketNodelay(int fd) {
    int enable = 1;
    //禁用nagle算法
    //nagle算法：写缓存积累到一定量的时候才会被发送出去
    setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (void*)&enable, sizeof(enable));
}


//安全的屏蔽SIGPIPE,异常不退出，正常应该直接退出
void handle_for_sigpipe(){
    struct sigaction sa;
    memset(&sa, '\0', sizeof(sa));
    sa.sa_handler = SIG_IGN;
    sa.sa_flags = 0;
    if(sigaction(SIGPIPE, &sa, NULL)) {
        return;
    }
}

//设置非阻塞模式
int setSocketNonBlocking(int fd) {
    //获取文件状态标识
    int flag = fcntl(fd, F_GETFL, 0);
    if (flag == -1) {
        return -1;
    }
    flag |= O_NONBLOCK;//按位，或运算
    //设置文件状态为非阻塞模式
    if(fcntl(fd, F_SETFL, flag) == -1)
        return -1;
    return 0;
}

//获取监听的文件描述符
int socket_bind_listen(int port) {
    if (port < 0 || port > 65535) {
        return -1;
    }
    int listen_fd = 0;
    //获取套接字，失败返回－１，成功返回正值
    if((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        return -1;
    }
    int optval = 1;
    //可重用端口，消除当前地址已经被使用的错误
    if(setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
        return -1;
    }

    struct sockaddr_in server_addr;
    bzero((char*)&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);//将主机数转化为网络字节数，无符号长整形
    server_addr.sin_port = htons((unsigned short)port);//将主机字节顺序转化为网络字节顺序，无符号短整形
    //绑定文件描述符和服务端地址
    if (bind(listen_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        return -1;
    }
    //开始监听，最大等待队列长为LISTENQ，监听客户端的最大个数
    if (listen(listen_fd, 2048) == -1) {
        return -1;
    }

    if (listen_fd == -1) {
        close(listen_fd);
        return -1;
    }

    return listen_fd;
}
