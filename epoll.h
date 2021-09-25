#ifndef EPOLL
#define EPOLL
#include "util.h"
#include "socketserver.h"
#include "timer.h"
#include "pthreadpool.h"

#include <sys/epoll.h>

const int LISTENQ_EPOLL = 1024;
const int MAXEVENTS = 5000;

void myHandler(void *args);

class Epoll
{
private:
    SocketServer &socketserver;
    
    int epoll_fd;
    int listen_fd;
    int event_count;
    struct epoll_event *events;
    
public:
    Epoll(SocketServer&);
    ~Epoll();
    bool epoll_add(int , void*, uint32_t); // 加入监听套接字
    bool epoll_mod(int , void*, uint32_t);
    bool epoll_del(int , void*, uint32_t);
    bool wait();

    int getEpollFd();
    void handle_events(PthreadPool&);
    void expired_events();
    bool acceptConnection();
};
#endif