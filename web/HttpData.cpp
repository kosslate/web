#include "HttpData.h"
#include "time.h"
#include "Channel.h"
#include "EventLoop.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <iostream>

using namespace std;

pthread_once_t MimeType::once_control = PTHREAD_ONCE_INIT;
std::unordered_map<std::string, std::string> MimeType::mime;

const __uint32_t DEFAULT_EVENT = EPOLLIN | EPOLLET | EPOLLONESHOT;
const int DEFAULT_EXPIRED_TIME = 2000;//ms
const int DEFAULT_KEEP_ALIVE_TIME = 5 * 60 * 1000;//ms


char favicon[555] = {
  '\x89', 'P', 'N', 'G', '\xD', '\xA', '\x1A', '\xA',
  '\x0', '\x0', '\x0', '\xD', 'I', 'H', 'D', 'R',
  '\x0', '\x0', '\x0', '\x10', '\x0', '\x0', '\x0', '\x10',
  '\x8', '\x6', '\x0', '\x0', '\x0', '\x1F', '\xF3', '\xFF',
  'a', '\x0', '\x0', '\x0', '\x19', 't', 'E', 'X',
  't', 'S', 'o', 'f', 't', 'w', 'a', 'r',
  'e', '\x0', 'A', 'd', 'o', 'b', 'e', '\x20',
  'I', 'm', 'a', 'g', 'e', 'R', 'e', 'a',
  'd', 'y', 'q', '\xC9', 'e', '\x3C', '\x0', '\x0',
  '\x1', '\xCD', 'I', 'D', 'A', 'T', 'x', '\xDA',
  '\x94', '\x93', '9', 'H', '\x3', 'A', '\x14', '\x86',
  '\xFF', '\x5D', 'b', '\xA7', '\x4', 'R', '\xC4', 'm',
  '\x22', '\x1E', '\xA0', 'F', '\x24', '\x8', '\x16', '\x16',
  'v', '\xA', '6', '\xBA', 'J', '\x9A', '\x80', '\x8',
  'A', '\xB4', 'q', '\x85', 'X', '\x89', 'G', '\xB0',
  'I', '\xA9', 'Q', '\x24', '\xCD', '\xA6', '\x8', '\xA4',
  'H', 'c', '\x91', 'B', '\xB', '\xAF', 'V', '\xC1',
  'F', '\xB4', '\x15', '\xCF', '\x22', 'X', '\x98', '\xB',
  'T', 'H', '\x8A', 'd', '\x93', '\x8D', '\xFB', 'F',
  'g', '\xC9', '\x1A', '\x14', '\x7D', '\xF0', 'f', 'v',
  'f', '\xDF', '\x7C', '\xEF', '\xE7', 'g', 'F', '\xA8',
  '\xD5', 'j', 'H', '\x24', '\x12', '\x2A', '\x0', '\x5',
  '\xBF', 'G', '\xD4', '\xEF', '\xF7', '\x2F', '6', '\xEC',
  '\x12', '\x20', '\x1E', '\x8F', '\xD7', '\xAA', '\xD5', '\xEA',
  '\xAF', 'I', '5', 'F', '\xAA', 'T', '\x5F', '\x9F',
  '\x22', 'A', '\x2A', '\x95', '\xA', '\x83', '\xE5', 'r',
  '9', 'd', '\xB3', 'Y', '\x96', '\x99', 'L', '\x6',
  '\xE9', 't', '\x9A', '\x25', '\x85', '\x2C', '\xCB', 'T',
  '\xA7', '\xC4', 'b', '1', '\xB5', '\x5E', '\x0', '\x3',
  'h', '\x9A', '\xC6', '\x16', '\x82', '\x20', 'X', 'R',
  '\x14', 'E', '6', 'S', '\x94', '\xCB', 'e', 'x',
  '\xBD', '\x5E', '\xAA', 'U', 'T', '\x23', 'L', '\xC0',
  '\xE0', '\xE2', '\xC1', '\x8F', '\x0', '\x9E', '\xBC', '\x9',
  'A', '\x7C', '\x3E', '\x1F', '\x83', 'D', '\x22', '\x11',
  '\xD5', 'T', '\x40', '\x3F', '8', '\x80', 'w', '\xE5',
  '3', '\x7', '\xB8', '\x5C', '\x2E', 'H', '\x92', '\x4',
  '\x87', '\xC3', '\x81', '\x40', '\x20', '\x40', 'g', '\x98',
  '\xE9', '6', '\x1A', '\xA6', 'g', '\x15', '\x4', '\xE3',
  '\xD7', '\xC8', '\xBD', '\x15', '\xE1', 'i', '\xB7', 'C',
  '\xAB', '\xEA', 'x', '\x2F', 'j', 'X', '\x92', '\xBB',
  '\x18', '\x20', '\x9F', '\xCF', '3', '\xC3', '\xB8', '\xE9',
  'N', '\xA7', '\xD3', 'l', 'J', '\x0', 'i', '6',
  '\x7C', '\x8E', '\xE1', '\xFE', 'V', '\x84', '\xE7', '\x3C',
  '\x9F', 'r', '\x2B', '\x3A', 'B', '\x7B', '7', 'f',
  'w', '\xAE', '\x8E', '\xE', '\xF3', '\xBD', 'R', '\xA9',
  'd', '\x2', 'B', '\xAF', '\x85', '2', 'f', 'F',
  '\xBA', '\xC', '\xD9', '\x9F', '\x1D', '\x9A', 'l', '\x22',
  '\xE6', '\xC7', '\x3A', '\x2C', '\x80', '\xEF', '\xC1', '\x15',
  '\x90', '\x7', '\x93', '\xA2', '\x28', '\xA0', 'S', 'j',
  '\xB1', '\xB8', '\xDF', '\x29', '5', 'C', '\xE', '\x3F',
  'X', '\xFC', '\x98', '\xDA', 'y', 'j', 'P', '\x40',
  '\x0', '\x87', '\xAE', '\x1B', '\x17', 'B', '\xB4', '\x3A',
  '\x3F', '\xBE', 'y', '\xC7', '\xA', '\x26', '\xB6', '\xEE',
  '\xD9', '\x9A', '\x60', '\x14', '\x93', '\xDB', '\x8F', '\xD',
  '\xA', '\x2E', '\xE9', '\x23', '\x95', '\x29', 'X', '\x0',
  '\x27', '\xEB', 'n', 'V', 'p', '\xBC', '\xD6', '\xCB',
  '\xD6', 'G', '\xAB', '\x3D', 'l', '\x7D', '\xB8', '\xD2',
  '\xDD', '\xA0', '\x60', '\x83', '\xBA', '\xEF', '\x5F', '\xA4',
  '\xEA', '\xCC', '\x2', 'N', '\xAE', '\x5E', 'p', '\x1A',
  '\xEC', '\xB3', '\x40', '9', '\xAC', '\xFE', '\xF2', '\x91',
  '\x89', 'g', '\x91', '\x85', '\x21', '\xA8', '\x87', '\xB7',
  'X', '\x7E', '\x7E', '\x85', '\xBB', '\xCD', 'N', 'N',
  'b', 't', '\x40', '\xFA', '\x93', '\x89', '\xEC', '\x1E',
  '\xEC', '\x86', '\x2', 'H', '\x26', '\x93', '\xD0', 'u',
  '\x1D', '\x7F', '\x9', '2', '\x95', '\xBF', '\x1F', '\xDB',
  '\xD7', 'c', '\x8A', '\x1A', '\xF7', '\x5C', '\xC1', '\xFF',
  '\x22', 'J', '\xC3', '\x87', '\x0', '\x3', '\x0', 'K',
  '\xBB', '\xF8', '\xD6', '\x2A', 'v', '\x98', 'I', '\x0',
  '\x0', '\x0', '\x0', 'I', 'E', 'N', 'D', '\xAE',
  'B', '\x60', '\x82',
};


void MimeType::init() {
    mime[".html"] = "text/html";
    mime[".avi"] = "video/x-msvideo";
    mime[".bmp"] = "image/bmp";
    mime[".c"] = "text/plain";
    mime[".doc"] = "application/x-gzip";
    mime[".gif"] = "image/gif";
    mime[".gz"] = "application/x-gzip";
    mime[".htm"] = "text/html";
    mime[".ico"] = "image/x-icon";
    mime[".jpg"] = "iamge/jpeg";
    mime[".png"] = "image/png";
    mime[".txt"] = "text/plain";
    mime[".mp3"] = "audio/mp3";
    mime["default"] = "text/html";
}

std::string MimeType::getMime(const std::string &suffix)
{
    pthread_once(&once_control, MimeType::init);
    if (mime.find(suffix) == mime.end())
        return mime["default"];
    else
        return mime[suffix];
}

HttpData::HttpData(EventLoop *loop, int connd) : 
    loop_(loop), channel_(new Channel(loop, connd)),
    fd_(connd), error_(false), connectionState_(H_CONNECTED),
    method_(METHOD_GET), HTTPVersion_(HTTP_11), nowReadPos_(0),
    state_(STATE_PARSE_URI), hState_(H_START), keepAlive_(false) {
        //将单次事件循环和当前数据处理绑定
        channel_->setReadHandler(bind(&HttpData::handleRead, this));
        channel_->setWriteHandler(bind(&HttpData::handleWrite, this));
        channel_->setConnHandler(bind(&HttpData::handleConn, this));
}

void HttpData::reset() {
    fileName_.clear();
    path_.clear();
    nowReadPos_ = 0;
    state_ = STATE_PARSE_URI;
    hState_ = H_START;
    headers_.clear();
    //keepAlive_ = false;
    if (timer_.lock()) {
        shared_ptr<TimerNode> my_timer(timer_.lock());
        my_timer->clearReq();
        timer_.reset();
    }
}

void HttpData::seperateTimer() {
    if (timer_.lock()) {
        shared_ptr<TimerNode> my_timer(timer_.lock());
        my_timer->clearReq();
        timer_.reset();
    }
}

void HttpData::handleRead() {
    __uint32_t &events_ = channel_->getEvents();
    do {
        int savedErrno = 0;
        ssize_t n = inBuffer_.readFd(fd_, &savedErrno);
        if (connectionState_ == H_DISCONNECTED) {
            inBuffer_.retrieveAll();
            break;
        }
        if (n < 0) {
            perror("1");
            error_ = true;
            handleError(fd_, 400, "Bad Request");
            break;
        }
        else if (n == 0) {
	    connectionState_ = H_DISCONNECTING;
            break;
        }
        if (state_ == STATE_PARSE_URI) {
            URIState flag = this->parseURI();
            if (flag == PARSE_URI_AGAIN) {
                break;
            }
            else if (flag == PARSE_URI_ERROR) {
                perror("2");
		LOG << "FD = " << fd_ << "," << inBuffer_.peek() << "******";
                inBuffer_.retrieveAll();
                error_ = true;
                handleError(fd_, 400, "Bad Request");
                break;
            }
            else {
                state_ = STATE_PARSE_HEADERS;
            }
        }
        if (state_ == STATE_PARSE_HEADERS) {
            HeaderState flag = this->parseHeaders();
            if (flag == PARSE_HEADER_AGAIN) {
                break;
            }
            else if (flag ==  PARSE_HEADER_ERROR) {
                perror("3");
                error_ = true;
                handleError(fd_, 400, "Bad Request");
            }
            if (method_ == METHOD_POST) {
                //POST方法
                state_ = STATE_RECV_BODY;
            }
            else {
                state_ = STATE_ANALYSIS;
            }
        }
        if (state_ == STATE_RECV_BODY) {
            int conten_length = -1;
            if (headers_.find("Content-length") != headers_.end()) {
                conten_length = stoi(headers_["Content-length"]);
            }
            else {
                error_ = true;
                handleError(fd_, 400, "Bad Request: Lack of argument(Content-length)");
                break;
            }
            if (static_cast<int>(inBuffer_.findCRLF() - inBuffer_.peek() + 2) < conten_length) {
                break;
            }
            state_ = STATE_ANALYSIS;
        }
        if (state_ == STATE_ANALYSIS) {
            AnalysisState flag = this->analysisRequest();
            if (flag ==ANALYSIS_SUCCESS) {
                state_ = STATE_FINISH;
                break;
            }
            else {
                error_ = true;
                break;
            }
        }
    }while(false);
    if (!error_) {
        if (outBuffer_.writeableByte() > 0) {
            handleWrite();
        }
        if (!error_ && state_ == STATE_FINISH) {
            this->reset();
            if (inBuffer_.readableByte() > 0) {
                if (connectionState_ != H_DISCONNECTING) {
                    handleRead();
                }
            }
        }
        else if (!error_ && connectionState_ != H_DISCONNECTED) {
            events_ |= EPOLLIN;
        }
    }
}

//处理写
void HttpData::handleWrite() {
    //　没有错误，且连接状态是关闭的状态
    if (!error_ && connectionState_ != H_DISCONNECTED) {
        //获取当前事件
        __uint32_t &events_ = channel_->getEvents();
        //ssize_t n = writen(fd_, outBuffer_.peek(), outBuffer_.readableByte());
        if (writen(fd_, outBuffer_.peek(), outBuffer_.readableByte()) < 0) {
            perror("witten");
            events_ = 0;
            error_ = true;
        }
	//outBuffer_.retrieve(n);
        //可写的数据大于０,事件设置为有数据需要写
        if (outBuffer_.writeableByte() > 0) {
            events_ |= EPOLLOUT;
        }
    }
}

//处理连接事件
void HttpData::handleConn() {
    //分离定时器和事件循环
    seperateTimer();
    //获取当前活跃的事件
    __uint32_t &events_ = channel_->getEvents();
    //如果没有出现错误且，连接状态是已经连接
    if (!error_ && connectionState_ == H_CONNECTED) {
        //当前事件不为０
        if (events_ != 0) {
            //更新超时时间
            int timeout = DEFAULT_EXPIRED_TIME;
            if (keepAlive_) {
                timeout = DEFAULT_KEEP_ALIVE_TIME;
            }
            if ((events_ & EPOLLIN) && (events_ & EPOLLOUT)) {
                events_ = __uint32_t(0);
                events_ |= EPOLLOUT;
            }
            events_ |= EPOLLET;
            loop_->updatePoller(channel_, timeout);
        }
        else if (keepAlive_) {
            events_ |= (EPOLLIN | EPOLLET);
            int timeout = DEFAULT_KEEP_ALIVE_TIME;
            loop_->updatePoller(channel_, timeout);
        }
        else {
            events_ |= (EPOLLIN | EPOLLET);
            int timeout = (DEFAULT_KEEP_ALIVE_TIME >> 1);
            loop_->updatePoller(channel_, timeout);
        }
    }
    //设置事件为，可写数据，边缘触发
    else if (!error_ && connectionState_ == H_DISCONNECTING && (events_ & EPOLLOUT)) {
        events_ = (EPOLLOUT | EPOLLET);
    }
    //关闭连接
    else {
        loop_->runInLoop(bind(&HttpData::handleClose, shared_from_this()));
    }
}

//解析URI
URIState HttpData::parseURI() {
    Buffer buf = inBuffer_;
    const char* begin = buf.peek();
    const char* end = buf.findCRLF();
    if (end) {
        const char* start = begin;
        const char* space = find(start, end, ' ');
        string request_line(start, space);
        bool succeed = false;
        int posGet = request_line.find("GET");
        int posPost = request_line.find("POST");
        int posHead = request_line.find("HEAD");

        if (posGet >= 0) {
            method_ = METHOD_GET;
        }
        else if (posPost >= 0) {
            method_  = METHOD_POST;
        }
        else if (posHead >= 0) {
            method_ = METHOD_HEAD;
        }
        else {
            return PARSE_URI_ERROR;
        }
        start = space + 1;
        space = std::find(start, end, ' ');
        if (space != end ) {
            const char* question = find(start, space, '?');
            if (question != space) {
                string m(start, question);
                fileName_ = m;
            }
            else {
                fileName_ = "index.html";
            }
            start = space + 1;
            succeed = end - start == 8 && equal(start, end - 1, "HTTP/1.");
            if (succeed) {
                if(*(end - 1) == '1') {
                    HTTPVersion_ = HTTP_11;
		    //cout << "succeed: " <<
                }
                else if(*(end - 1) == '0') {
                    HTTPVersion_ = HTTP_10;
                }
                else {
                    return PARSE_URI_ERROR;
                }
            }
            else {
                return PARSE_URI_ERROR;
            }

        }
	
        return PARSE_URI_SUCCESS;
    }
    return PARSE_URI_AGAIN;
}

HeaderState HttpData::parseHeaders() {
    Buffer buf = inBuffer_;
    buf.retrieveUtil(buf.findCRLF() + 2);
    const char* crlf = buf.findCRLF();
    if (crlf) {
        const char* colon = std::find(buf.peek(), crlf, ':');
        if (crlf != colon) {
            string filed(buf.peek(), colon);
            ++ colon;
            while (colon < crlf && isspace(*colon)) {
                ++ colon;
            }
            string value(colon, crlf);
            while (!value.empty() && isspace(value[value.size()-1])) {
                value.resize(value.size()-1);
            }
            headers_[filed] = value;
        }
        else {
            return PARSE_HEADER_ERROR;
        }
        buf.retrieveUtil(buf.peek() + 2);
    }
    else {
        return PARSE_HEADER_ERROR;
    }
    inBuffer_.retrieveAll();
    return PARSE_HEADER_SUCCESS;
}

//分析响应
AnalysisState HttpData::analysisRequest() {
    //post模式
    if (method_ == METHOD_POST) {
    }
    //get模式或者head模式
    else if (method_ == METHOD_GET || METHOD_HEAD) {
        string header;
        header += "HTTP/1.1 200 OK\r\n";
        //判断头部是否含有连接参数，且生存周期是否存在
        if (headers_.find("Connection") != headers_.end() &&
            (headers_["Connection"] == "Keep-Alive" || 
            headers_["Connection"] == "Keep-alive")) {
                keepAlive_ = true;
                header += string("Connection: Keep-Alive\r\n")+
                    "Keep-Alive: timeout=" + 
                    to_string(DEFAULT_KEEP_ALIVE_TIME) + "\r\n";
            }
        //找到文件的名的位置
        int dot_pos = fileName_.find('.');
        //定义文件名
        string filetype;
        if (dot_pos < 0) {
            filetype = MimeType::getMime("default");
        }
        else {
            filetype = MimeType::getMime(fileName_.substr(dot_pos));
        }

        //回声测试
        if (fileName_ == "hello") {
            strcpy(outBuffer_.beginWrite(), "HTTP/1.1 200 OK\r\nContent-type: text/plain\r\n\r\nHello World");
            return ANALYSIS_SUCCESS;
        }
        if (fileName_ == "favicon.ico") {
            header += "Content-Type: image/png\r\n";
            header += "Content-Length: " + to_string(sizeof favicon) + "\r\n";
            header += "Server: Liu's Web Server\r\n";

            header += "\r\n";
            //拷贝到开始写的位置
            strcpy(outBuffer_.beginWrite(),header.c_str());
            //在后面加上字符串数据
            outBuffer_.append(string(favicon, favicon + sizeof favicon));
            return ANALYSIS_SUCCESS;
        }
        //获取linux文件属性的结构体
        struct stat sbuf;
        //struct stat{
        //  dev_t st_dev;(文件所在的设备ID)
        //　ino_t st_ino;(节点号)
        //  nlink_t st_nlink;(硬连接数量)
        //  uid_t st_uid;(user id)
        //  gid_t st_gid;(group id)
        //  dev_t st_rdev;(设备号)
        //  off_t st_size;（文件大小，字节单位）
        //  blkcnt_t st_blocks;(系统块的大小)
        //　blkcnt_t st_blocks;(文件所占快数)
        //  time_t st_time;(最近存取时间)
        //  time_t st_mtime;(最近修改时间)
        //  time_t st_ctime;
        //返回０，成功
        if (stat(fileName_.c_str(), &sbuf) < 0) {
            header.clear();
            handleError(fd_, 400, "Not Found!");
        }
        //头部增加
        header += "Conten-Type: " + filetype + "\r\n";
        header += "Content-Length: " + to_string(sbuf.st_size) + "\r\n";
        header +="Server: Liu's Web Server\r\n";

        header += "\r\n";
        //将头部拷贝到写开始的位置
        outBuffer_.append(header);
	//cout << "head: " << outBuffer_.peek() << endl;
        //如果只有头，直接返回
        if (method_ == METHOD_HEAD) {
            return ANALYSIS_SUCCESS;
        }

        //内存映射的步骤
        //1.打开文件，得到文件描述符
        //2.利用mmap建立内存映射，返回映射首地址的指针
        //3.关闭文件
        //4.munmap关闭内存映射
        int src_fd = open(fileName_.c_str(), O_RDONLY, 0);
        if (src_fd < 0) {
            outBuffer_.retrieveAll();
            handleError(fd_, 400, "Not Found!");
            return ANALYSIS_ERROR;
        }

        //mmap：将一个文件或其他对象映射到内存
        //第一个参数：内存其实地址，设置为NULL,系统自动选定地址
        //第二个参数：映射的文件大小
        //第三个参数：映射区域的包含方式(PROT_XXX)
        //(EXEC 可执行)(READ 可读取)(WRITE 可写入)(NONE 不能存取)
        //第四个参数：映射特性(MAP_XXX)
        //(SHARED 复制回文件)(PRIVATE 不写入文件/产生一个复制）
        //第五个参数：文件描述符
        //第六个参数：偏移量，必须是分页大小的整数倍，一般为０
        void *mmapRet = mmap(NULL, sbuf.st_size, PROT_READ, MAP_PRIVATE, src_fd, 0);
        close(src_fd);
        if (mmapRet == (void *)-1) {
            munmap(mmapRet, sbuf.st_size);
            outBuffer_.retrieveAll();
            handleError(fd_, 404, "Not Found!");
            return ANALYSIS_ERROR;
        }

        char *src_addr = static_cast<char*>(mmapRet);
        outBuffer_.append(string(src_addr, src_addr + sbuf.st_size));
	
	//cout << "out_buffer: " << outBuffer_.peek() << endl;
        munmap(mmapRet, sbuf.st_size);
        return ANALYSIS_SUCCESS;
    }
    return ANALYSIS_ERROR;
}


void HttpData::handleError(int fd, int err_num, string short_msg) {
    short_msg = " " + short_msg;
    char send_buff[4096];
    string body_buff, header_buff;
    body_buff += "<html><title>哎~出错了</title>";
    body_buff += "<body bgcolor=\"ffffff\">";
    body_buff += to_string(err_num) + short_msg;
    body_buff += "<hr><em> Liu's Web Server</em>\n</body></html>";

    header_buff += "HTTP/1.1 " + to_string(err_num) + short_msg + "\r\n";
    header_buff += "Content-Type: text/html\r\n";
    header_buff += "Connection: Close\r\n";
    header_buff += "Content-Length: " + to_string(body_buff.size()) + "\r\n";
    header_buff += "Server: Liu's Web Server\r\n";;
    header_buff += "\r\n";
    // 错误处理不考虑writen不完的情况
    sprintf(send_buff, "%s", header_buff.c_str());
    writen(fd, send_buff, strlen(send_buff));
    sprintf(send_buff, "%s", body_buff.c_str());
    writen(fd, send_buff, strlen(send_buff));
}

void HttpData::handleClose() {
    connectionState_ = H_DISCONNECTED;
    shared_ptr<HttpData> guard(shared_from_this());
    loop_->removeFromPoller(channel_);
}

void HttpData::newEvent() {
    channel_->setEvents(DEFAULT_EVENT);
    loop_->addToPoller(channel_, DEFAULT_EXPIRED_TIME);
}
