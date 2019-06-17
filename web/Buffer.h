#pragma once

#include <algorithm>
#include <vector>
#include <string>

#include <assert.h>
#include <string.h>

class Buffer {
    public:
    static const size_t kCheapPrepend = 8;
    static const size_t kInitialSize = 12288;//这个决定影响的数据量，太大会降低，太小会限制

    //初始化缓冲区，读写的指针位置都是预留位置
    explicit Buffer(size_t initialSize = kInitialSize) : 
        m_buffer(kCheapPrepend + initialSize), 
        m_readerIndex(kCheapPrepend),
        m_writerIndex(kCheapPrepend){
            assert(readableByte() == 0);
            assert(writeableByte() == initialSize);
            assert(prependableByete() == kCheapPrepend);
    }
    //获取需要读的数据字节(写指针位置 - 读指针位置)
    size_t  readableByte() {
        return m_writerIndex - m_readerIndex; 
    }
    //获取需要写的数据的字节(缓冲区长度 - 写指针位置)
    size_t  writeableByte() {
        return m_buffer.size() - m_writerIndex;
    }

    //获取准备好的字节，读指针的位置
    size_t prependableByete() {
        return m_readerIndex;
    }
    //获取读指针后面的数据
    const char* peek() const {
        return begin() + m_readerIndex;
    }
    //获取写指针后面的数据
    const char* beginWrite() const {
        return begin() + m_writerIndex;
    }

    //开始写的位置
    char* beginWrite() {
        return begin() + m_writerIndex;
    }

    //已经写了一定长度，写指针移动一定长度
    void hasWritten(size_t len) {
        assert(len <= writeableByte());
        m_writerIndex += len;
    }

    //找到固定字符的位置
    const char* findCRLF() const {
        const char* crlf = std::search(peek(),beginWrite(),
            kCRLF, kCRLF + 2);
        return crlf == beginWrite() ? NULL : crlf;
    }
    void unwrite(size_t len) {
        assert(len <= readableByte());
        m_writerIndex -= len;
    }

    
    void retrieve(size_t len) {
        assert(len <= readableByte());
        if (len < readableByte()) {
            m_readerIndex += len;
        }
        else {
            retrieveAll();
        }
    }

    //检索结束位置
    void retrieveUtil(const char* end) {
        assert(peek() <= end);
        assert(end <= beginWrite());
        retrieve(end - peek());
    }

    //恢复读写指针位置，也就是清空操作
    void retrieveAll() {
        m_readerIndex = kCheapPrepend;
        m_writerIndex = kCheapPrepend;
    }

    std::string retrieveAsString(size_t len) {
        assert(len <= readableByte());
        std::string result(peek(), len);
        retrieve(len);
        return result;
    }

    //调整读写指针位置
    void makeSpace(int len) {
        //写位置 ＋ 预留空间　< 输入长度 + 预留空间,调整空间
        if (writeableByte() + prependableByete() < len + kCheapPrepend) {
            m_buffer.resize(m_writerIndex + len);
        }
        else {
            //将可读数据移动到预留空间之后，可读指针和写指针移动
            assert(kCheapPrepend < m_readerIndex);
            size_t readable = readableByte();
            std::copy(begin() + m_readerIndex, 
                begin() + m_writerIndex, begin() + kCheapPrepend);
            m_readerIndex = kCheapPrepend;
            m_writerIndex = m_readerIndex + readable;
            assert(readable == readableByte());
        }
    }

    //增加字符串
    void append(const std::string &str) {
        append(str.c_str(), str.size());
    }
    //增加数据
    //可写的数据长度 < 增加的数据长度，调整空间
    //可写的数据长度 >= 增加的数据长度，将数据拷贝到可写指针之后，可写指针移动增加数据长度
    void append(const char* data, size_t len) {
        if (writeableByte() < len) {
            makeSpace(len);
        }
        assert(writeableByte() >= len);
        std::copy(data, data + len, beginWrite());
        hasWritten(len);
    }

    //读取数据，加入缓冲区
    ssize_t readFd(int fd, int* savedErrno);

    //缓冲区最大容量
    size_t internalCapacity() const {
        return m_buffer.capacity();
    }
    private:
    //缓冲区的头
    char* begin() {
        return &*m_buffer.begin(); 
    }
    const char* begin() const {
        return &*m_buffer.begin(); 
    }
    private:
    //静态数组
    static const char kCRLF[];
    std::vector<char> m_buffer;//利用向量存储数据
    size_t m_readerIndex;//定义读数据的头指针
    size_t m_writerIndex;//定义写数据的头指针
};
