// #include "httpparser/httprequestparser.h"
#include "parse/http_handle.h"
#include "request.h"
#include "log.h"


Request::Request(int _fd, Epoll &_epoll):socket_fd(_fd), timer(nullptr), epoll(_epoll)
{
    this->data = "";
}


Request::~Request()
{
    Debug("%s:%d Request::~Request 结束【%d】请求\n", __FILE__, __LINE__, this->socket_fd);

    this->epoll.epoll_del(this->socket_fd, (void*)this, EPOLLIN | EPOLLET | EPOLLONESHOT);
    close(this->socket_fd);
    if(this->timer != nullptr)
    {
        timer->clearReq();
        timer = nullptr;
    }
}


int Request::getSocketFd()
{
    return this->socket_fd;
}


int Request::getEpollFd()
{
    return this->epoll.getEpollFd();
}


void Request::addTimer()
{
    Debug("%s:%d Request::addTimer\n", __FILE__, __LINE__);
    if(this->timer != nullptr)
    {
        perror("Request::addTimer");
        exit(1);
    }
    this->timer = new Timer(this); 
    Timer::push(this->timer);
    Debug("%s:%d Request::addTimer End\n", __FILE__, __LINE__);
}


void Request::seperateTimer()
{
    Debug("%s:%d Request::seperateTimer\n", __FILE__, __LINE__);
    if(this->timer == nullptr)
    {
        perror("Request::seperateTimer");
        exit(1);
    }
    this->timer->clearReq();
    this->timer = nullptr;
    Debug("%s:%d Request::seperateTimer End\n", __FILE__, __LINE__);
}


void Request::handleRequest()
{
    Debug("%s:%d Request::handleRequest\n", __FILE__, __LINE__);
    // int count = readEND(this->socket_fd);
    int count = readET(this->socket_fd, this->data);

    if(count == 0)
    {
        LOG::Warn(__FILE__, __LINE__, "【%d】套接字连接断开或者异常", this->socket_fd);
        delete this;
    }
    else
    {   
        Debug("%s:%d 正确收到消息，更待套接字在 epoll 中的状态\n", __FILE__, __LINE__);
        
        Debug("request:\n%s\n", this->data.c_str());
        std::string reponse = http_request_handle(this->data.c_str(), (size_t)this->data.size());
        if(reponse != "Not Http")
        {
            Debug("Http 响应\n");
            Debug("\n%s\n", reponse.c_str());
            writen(this->socket_fd, (const void*)reponse.c_str(), reponse.size());
        }
        else
        {
            Debug("Http 不响应\n");
            reponse = "Cat Http";
            writen(this->socket_fd, (const void*)reponse.c_str(), reponse.size());
        }

        this->addTimer();  // 更新事件要写在前面
        if(this->epoll.epoll_mod(this->socket_fd, (void*)this, EPOLLIN | EPOLLET | EPOLLONESHOT) == false)
        {
            perror("Request::handleRequest");
            exit(1);
        }

    }
    Debug("%s:%d 本次事件处理完毕\n", __FILE__, __LINE__);
}

