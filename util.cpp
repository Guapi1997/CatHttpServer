#include "util.h"
#include <string.h>
#include <signal.h>
#include <stdio.h>
#include "log.h"

void handle_for_sigpipe()
{
    struct sigaction sa;
    memset(&sa, '\0', sizeof(sa));
    sa.sa_handler = SIG_IGN;
    sa.sa_flags = 0;
    if(sigaction(SIGPIPE, &sa, NULL))
        return;
}


ssize_t readn(int fd, void *buff, size_t n)
{
    size_t nleft = n;
    ssize_t nread = n;
    ssize_t readSum = 0;
    char *ptr = (char*) buff;
    while(nleft > 0)
    {
        if((nread = read(fd, ptr, nleft)) < 0)
        {
            if(errno == EINTR)
            {
                nread = 0;
            }
            else if(errno == EAGAIN)
            {
                return readSum;
            }
            else
                return -1;
        }
        else if(nread == 0)
            break;
        readSum += nread;
        nleft -= nread;
        ptr += nread;
    }
    return readSum;
}


ssize_t writen(int fd, const void *buff, size_t n)
{
    size_t nleft = n;
    ssize_t nwrite = 0;
    ssize_t writeSum = 0;
    char *ptr = (char*)buff;
    while(nleft > 0)
    {
        if((nwrite = write(fd, buff, nleft)) <= 0)
        {
            if(errno == EINTR || errno == EAGAIN)
            {
                nwrite = 0;
                LOG::Warn(__FILE__, __LINE__, "写入失败");
                continue;
            }
            else
                return -1;
        }
        writeSum += nwrite;
        nleft -= nwrite;
        ptr += nwrite; 
    }
    LOG::Info(__FILE__, __LINE__, "已经发送信息");
    Debug("%s:%d 写入到【%d】套接字的字数 %d\n", __FILE__, __LINE__, fd, (int)writeSum);
    return writeSum;
}


ssize_t readET(const int fd, string& content)
{
    ssize_t readSum = 0;
    uint8_t sumtime = 0;
    uint8_t trytime = MAXREADTIME;
    
    while(trytime > 0 && readSum < 4096)
    {
        sumtime++;
        char buff[1024];
        ssize_t readNum = readn(fd, buff, sizeof(buff));
        
        if(readNum == 0)
            trytime--;
        else
        {
            readSum += readNum;
            content += string(buff, readNum);
        }
    }
    
    return readSum;
}
