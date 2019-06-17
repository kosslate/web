#include "Buffer.h"
#include "Util.h"
#include <sys/uio.h>
#include <iostream>

const ssize_t kExtraBufferSize = 10240;
const char Buffer::kCRLF[] = "\r\n";

const size_t Buffer::kCheapPrepend;
const size_t Buffer::kInitialSize;


ssize_t Buffer::readFd(int fd, int* savedErrno) {
    char extrabuf[kExtraBufferSize];
    struct iovec vec[2];
    const size_t writeable = writeableByte();
    vec[0].iov_base = begin() + m_writerIndex;
    vec[0].iov_len = writeable;
    vec[1].iov_base = extrabuf;
    vec[1].iov_len = sizeof extrabuf;
    //如果在缓冲中有足够的空间，就不需要读extrabuf
    //如果使用extrabuf，最多使用127k bytes
    const int iovcnt = (writeable < sizeof extrabuf) ? 2 : 1;
    const ssize_t n = readvn(fd, vec, iovcnt);
    //返回0，报错
    if (n < 0) {
        *savedErrno = errno;
    }
    //当前读入数据小于预留空间(可写的指针位置)，可写指针移动数据位数
    else if (static_cast<size_t>(n) <= writeable) {
        m_writerIndex += n;
    }
    //读取数据大于预留空间(可写的指针位置)，
    else {
        m_writerIndex = m_buffer.size();
        append(extrabuf, n - writeable);
    }
    return n;
}

