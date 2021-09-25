#ifndef SOCKETSERVER
#define SOCKETSERVER
#include "util.h"

#include <queue>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <map>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <unordered_set>
using std::string;
using std::unordered_set;

const int LISTENQ_SOCKET = 1024;

bool init_sockaddr_in(struct sockaddr_in &, uint16_t , uint32_t);

bool setNoBlockSocket(int fd);


class SocketServer
{
private:
    int port; // 侦听端口 80
    string listen_addr; // 指定侦听地址，暂不使用
    bool is_block; // 套接字是否阻塞， 默认 true
    
    int listen_fd;
    struct sockaddr_in server_addr;

public:
    SocketServer(int _port = 80, bool _is_block = false); 
    ~SocketServer();
    bool socket_bind_listen();
    int getListenFd(); 
    int addNewConnection();
    
};

#endif