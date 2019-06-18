#pragma once

#include <algorithm>
#include <vector>
#include <string>

#include <assert.h>
#include <string.h>

class Buffer {
    public:
    static const size_t kCheapPrepend = 8;
    static const size_t kInitialSize = 1024;

    //初始化缓冲区，读写的指针位置都是预留位置
    explicit Buffer(size_t initialSize = kInitialSize) : 
        m_buffer(kCheapPrepend + initialSize), 
        m_readerIndex(kCheapPrepend),
        m_writerIndex(kCheapPrepend){
            assert(readableByte() == 0);
            assert(writeableByte() == initialSize);
            assert(prependableByete() == kCheapPrepend);
    }
    //有效的数据(写指针位置 - 读指针位置)
    size_t  readableByte() {
        return m_writerIndex - m_readerIndex; 
    }
    //剩余的空间，扩容的考虑
    size_t  writeableByte() {
        return m_buffer.size() - m_writerIndex;
    }

    //获取开始数据的位置
    size_t prependableByete() {
        return m_readerIndex;
    }
    //获取有效的数据头
    const char* peek() const {
        return begin() + m_readerIndex;
    }
    //尾部加入数据，前面数据的结尾
    const char* beginWrite() const {
        return begin() + m_writerIndex;
    }

    //加入数据的位置
    char* beginWrite() {
        return begin() + m_writerIndex;
    }

    //需要加入的数据长度，需要判断长度问题
    void hasWritten(size_t len) {
        assert(len <= writeableByte());
        m_writerIndex += len;
    }

    //找到标志位
    const char* findCRLF() const {
        const char* crlf = std::search(peek(),beginWrite(),
            kCRLF, kCRLF + 2);
        return crlf == beginWrite() ? NULL : crlf;
    }
    
    //删除一定长度的数据，从后往前删除
    void unwrite(size_t len) {
        assert(len <= readableByte());
        m_writerIndex -= len;
    }

    //删除一定长度的数据，从前到后开始删除
    void retrieve(size_t len) {
        assert(len <= readableByte());
        if (len < readableByte()) {
            m_readerIndex += len;
        }
        else {
            retrieveAll();
        }
    }

    //获取数据开始位置到输入位置的数据
    void retrieveUtil(const char* end) {
        assert(peek() <= end);
        assert(end <= beginWrite());
        retrieve(end - peek());
    }

    //清空所有的数据
    void retrieveAll() {
        m_readerIndex = kCheapPrepend;
        m_writerIndex = kCheapPrepend;
    }

    //转化一定长度的数据为string类型
    std::string retrieveAsString(size_t len) {
        assert(len <= readableByte());
        std::string result(peek(), len);
        retrieve(len);
        return result;
    }

    //调整读写指针位置
    void makeSpace(int len) {
        //剩余空间 + 已经插入的数据 + 预留空间　< 给定长度 + 预留空间，也就是说空间不足，进行扩容
        if (writeableByte() + prependableByete() < len + kCheapPrepend) {
            m_buffer.resize(m_writerIndex + len);
        }
        //空间足够放下
        else {
            assert(kCheapPrepend < m_readerIndex);
            //当前数据长度
            size_t readable = readableByte();
            //将m_readerIndex到m_riterIndex的数据，复制到预留空间之后，感觉没变|－_－|
            std::copy(begin() + m_readerIndex, 
                begin() + m_writerIndex, begin() + kCheapPrepend);
            m_readerIndex = kCheapPrepend;
            m_writerIndex = m_readerIndex + readable;
            assert(readable == readableByte());
        }
    }

    //增加数据
    void append(const std::string &str) {
        append(str.c_str(), str.size());
    }
    //增加数据
    //可写的数据长度 < 增加的数据长度，调整空间
    //可写的数据长度 >= 增加的数据长度，将数据拷贝到可写指针之后，可写指针移动增加数据长度
    void append(const char* data, size_t len) {
        //预留空间 < 数据长度
        if (writeableByte() < len) {
            makeSpace(len);
        }
        assert(writeableByte() >= len);
        //将数据拷贝到原数据尾部，类似与 str += str1;
        std::copy(data, data + len, beginWrite());
        //剩余空间减少
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
    std::vector<char> m_buffer;//利用向量存储数据，动态扩容
    size_t m_readerIndex;//定义读数据的头指针
    size_t m_writerIndex;//定义写数据的头指针
};
