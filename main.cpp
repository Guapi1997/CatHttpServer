#include "socketserver.h"
#include "epoll.h"
#include "util.h"
#include "log.h"
#include "pthreadpool.h"


int main()
{
    handle_for_sigpipe();
    LOG::RANK = 2;
    SocketServer server;
    if(server.socket_bind_listen() == true)
    {
        LOG::Info(__FILE__,  __LINE__,  "监听服务开启");
    }
    else
    {
        perror("main");
        exit(1);
    }
    
    Epoll epoll(server);
    PthreadPool pthreadpool;

    while(true)
    {
        LOG::Info(__FILE__, __LINE__, "Epoll Wait...");
        if(epoll.wait() == true)
            epoll.handle_events(pthreadpool);
        epoll.expired_events();
    }
    return 0;
}