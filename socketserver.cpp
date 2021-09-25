#include "socketserver.h"
#include "log.h"

SocketServer::SocketServer(int _port, bool _isblock):port(_port), is_block(_isblock)
{
    // 这里只做一个简单的设置，并不去打开
    this->port = _port;
    this->is_block = _isblock;
    init_sockaddr_in(this->server_addr, this->port, INET_ADDRSTRLEN);
    
    if((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("SocketServer::SocketServer 监听套接字对象初始化失败");
        exit(1);
    }
    
    Debug("%s:%d 建立监听套接字对象【%d】\n", __FILE__, __LINE__, listen_fd);
}


SocketServer::~SocketServer()
{
    // 析构所有的监听端口
    close(listen_fd);
}


bool SocketServer::socket_bind_listen()
{
    if(port < 0 || port > 65535)
        return false;
    
    // WATCH
    int optval = 1;
    if(setsockopt(this->listen_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
        return false;

    if(bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
        return false;

    if(is_block == false && setNoBlockSocket(listen_fd) == false)
    {
        perror("监听套接字设置为非阻塞失败");
        close(listen_fd);
        exit(1);
        return false;
    }
    else
        LOG::Info(__FILE__, __LINE__, "监听套接字设置为非阻塞");

   
    
    if(listen(listen_fd, LISTENQ_SOCKET) == -1)
    {
        perror("监听打开失败");
        exit(1);
        return false;
    }
    LOG::Info(__FILE__, __LINE__, "开始监听 Port = %d", port);

    return true;
}


int SocketServer::getListenFd()
{
    return listen_fd;
}


int SocketServer::addNewConnection()
{
    return 0;
}


bool setNoBlockSocket(int fd)
{
    /* 设置文件非阻塞 */
    int flag = fcntl(fd, F_GETFL, 0);
    if(flag == -1) 
        return false;
    flag |= O_NONBLOCK;
    if(fcntl(fd, F_SETFL, flag) == -1)
        return false;
    return true;
} 


bool init_sockaddr_in(struct sockaddr_in &addr, uint16_t port, uint32_t inaddr)
{
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons((unsigned short)port);
    return true;
}