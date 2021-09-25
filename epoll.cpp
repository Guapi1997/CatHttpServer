#include "epoll.h"
#include "request.h"
#include "log.h"

Epoll::Epoll(SocketServer &_socketserver):socketserver(_socketserver), epoll_fd(-1), listen_fd(-1), event_count(0), events(NULL)
{
    // NOTE. 初始化 epoll, epoll 本身会占用一个句柄
    if((this->listen_fd = this->socketserver.getListenFd()) == -1)
    {
        perror("Epoll::Epoll-构造失败,socketserver不完整");
        exit(1);
    }
        
    this->epoll_fd = epoll_create(LISTENQ_EPOLL + 1);
    
    if(events == NULL)
        this->events = new epoll_event[MAXEVENTS];
    
    // EPOLLIN 文件描述符可读 EPOLLET 边缘触发，一次触发，全部读取
    Request *request = new Request(this->listen_fd, *this);
    this->epoll_add(listen_fd, request, EPOLLIN | EPOLLET);
    
    // 将 server 给初始化
    Debug("%s:%d Epoll句柄【%d】初始化完成.\n", __FILE__, __LINE__, this->epoll_fd);
}


Epoll::~Epoll()
{
    if(this->events != NULL)
    {
        delete[] this->events;
    }
    else
    {
        perror("Epoll::~Epoll");
        exit(1);
    }
    // 关闭epoll
    Debug("%s:%d Epoll销毁\n", __FILE__, __LINE__);
}


bool Epoll::epoll_add(int socket_fd, void *request, uint32_t event_flag)
{
    /*sock_fd 加入的 epoll 的套接字
    evenets 加入的监听事件
    */
    struct epoll_event event = {0};
    event.events = event_flag;
    event.data.ptr = request;
    
    // WARN：这里需要增加对事件的判断
    if(epoll_ctl(this->epoll_fd, EPOLL_CTL_ADD, socket_fd, &event) < 0)
    {
        perror("epoll add: socket fd failed");
        exit(1);
        return false;
    }        
    return true;
}


bool Epoll::epoll_mod(int socket_fd, void *rq, __uint32_t event_flag)
{
    Debug("%s:%d Epoll::epoll_mod\n", __FILE__, __LINE__);

    struct epoll_event event;
    event.events = event_flag;
    event.data.ptr = rq;

    if(epoll_ctl(epoll_fd, EPOLL_CTL_MOD, socket_fd, &event) < 0)
    {
        perror("Epoll::epoll_mod");
        return false;
    }

    Debug("%s:%d socket【%d】 Mod in Epoll\n Epoll::epoll_mod END\n", __FILE__, __LINE__, socket_fd);
    return true;
}


bool Epoll::epoll_del(int socket_fd, void *rq, uint32_t event_flag)
{
    struct epoll_event ev;
    ev.events = event_flag;
    ev.data.ptr = rq;
        
    
    Debug("%s:%d 取消监控套接字【%d】的Epoll监控\n", __FILE__, __LINE__, socket_fd);
    
    epoll_ctl(this->epoll_fd, EPOLL_CTL_DEL, socket_fd, &ev);
    return true;
}


bool Epoll::wait()
{
    if(this->event_count!=0)
    {
        perror("Epoll::wait");
        exit(1);
    }
    this->event_count = epoll_wait(this->epoll_fd, events, MAXEVENTS, -1);
    if(this->event_count < 0)
    {
        perror("epoll wait error");
        return false;
    }
    else
        return true;
}


int Epoll::getEpollFd()
{
    return this->epoll_fd;
}


bool Epoll::acceptConnection()
{
    // Note. LOG 也许可以当作，注释来写
    Debug("%s:%d Epoll::acceptConnection\n", __FILE__, __LINE__);

    int accept_fd;
    socklen_t client_addr_len = 0;
    struct sockaddr_in client_addr;
    memset(&client_addr, 0, sizeof(struct sockaddr_in));
    
    while((accept_fd = accept(listen_fd, (struct sockaddr*)&client_addr, &client_addr_len)) > 0)
    {
        // 这里可能要加入 socketserver 队列
        setNoBlockSocket(accept_fd);
        Request *rq = new Request(accept_fd, *this);
        uint32_t event_flag = EPOLLIN | EPOLLET | EPOLLONESHOT;
        
        this->epoll_add(accept_fd, rq, event_flag);
        rq->addTimer();
    }
    Debug("%s:%d Epoll::acceptConnection End\n", __FILE__, __LINE__);
    return true;
}


void Epoll::handle_events(PthreadPool &pthreadpool)
{
    int count = this->event_count;
    this->event_count = 0;
    for(int i = 0; i < count; ++i)
    {
        Request* request = ((Request*)this->events[i].data.ptr);
        if(request == nullptr)
        {
            perror("Epoll::handle_events A");
            exit(1);
        }
            
        int fd = request->getSocketFd();
        Debug("%s:%d 发生事件的套接字句柄【%d】\n", __FILE__, __LINE__ , fd);
        if(fd == this->socketserver.getListenFd())
        {
            this->acceptConnection();
        }
        else
        {
            //检查事件类型
            if((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP) || (!events[i].events && EPOLLIN) || request == NULL)
            {
                perror("Epoll::handle_events - 事件类型错误");
                exit(1);
            }
            else
            {   
                // 这里为什么要剥离时间控制器，因为 request的处理线程已经启动了，也许需要很久
                request->seperateTimer();
                pthreadpool.addTask(myHandler, this->events[i].data.ptr, 0);
                //  request->handleRequest();
            }
        }
    }
    
}


void Epoll::expired_events()
{
    Timer::ExpiredTimer();
}


void myHandler(void *args)
{
    Request *req = (Request*)args;
    req->handleRequest();
}