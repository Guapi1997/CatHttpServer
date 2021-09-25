#ifndef REQUEST
#define REQUEST
#include "util.h"
#include "timer.h"
#include "epoll.h"

#include <unistd.h>
#include <sys/epoll.h>
#include <stdio.h>
#include <sys/socket.h>
#include <string>
using std::string;


class Request 
{
private:
    int socket_fd;
    string data;
    Timer *timer;
    Epoll &epoll;

public:
    Request(int, Epoll&);
    ~Request();
    
    int getSocketFd();
    int getEpollFd();
    
    void addTimer();
    void seperateTimer();
    
    void handleRequest();
};
#endif