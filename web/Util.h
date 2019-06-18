#pragma once
#include <string>

int socket_bind_listen(int port);

int setSocketNonBlocking(int fd);

void handle_for_sigpipe();

void setSocketNodelay(int fd);

ssize_t readn(int fd, void *buff, size_t n);
ssize_t readvn(int sockfd, const struct iovec *iov, int iovcnt);
ssize_t writen(int fd, const void *buf, size_t count);

void shutdownWR(int fd);