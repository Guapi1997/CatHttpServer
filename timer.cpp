#include <stdio.h>

#include "request.h"
#include "timer.h"
#include "log.h"




pthread_mutex_t Timer::timer_qlock = PTHREAD_MUTEX_INITIALIZER;
priority_queue<Timer*, deque<Timer*>, struct timerCmp> Timer::TimerQueue = {};

void Timer::ExpiredTimer()
{
    pthread_mutex_lock(&Timer::timer_qlock);
    while(!TimerQueue.empty())
    {
        Timer *ptimer_now = TimerQueue.top();
        if(ptimer_now->isDeleted())
        {
            TimerQueue.pop();
            delete ptimer_now;
        }
        else
            break;
    }
    pthread_mutex_unlock(&Timer::timer_qlock);
}


void Timer::push(Timer *timer)
{
    
    pthread_mutex_lock(&Timer::timer_qlock);
    Timer::TimerQueue.push(timer);
    pthread_mutex_unlock(&Timer::timer_qlock);
    
    Debug("%s:%d【%d】套接字加入到时间队列\n", __FILE__, __LINE__, ((Request*)timer->request)->getSocketFd());
}


Timer::Timer(void *_request):timeout(MAXTIMEOUT), isdeleted(false), request(_request)
{
    struct timeval now;
    gettimeofday(&now, NULL);
    this->expired_time = ((now.tv_sec * 1000) + (now.tv_usec / 1000)) + timeout;
}


Timer::~Timer()
{
    // Note. 解析时间类的同时，解析时间对象对应的请求对象
    if(this->request != nullptr)
    {
        Request* rq = (Request*) request;
        Debug("%s:%d 【%d】套接字超时, 在时间对象中析构\n", __FILE__, __LINE__ ,rq->getSocketFd());
        delete rq;
        this->request =NULL;
    }
}


bool Timer::isDeleted()
{
    if(this->request == nullptr)
        isdeleted = true;

    if(isdeleted == true)
        return isdeleted;

    struct timeval now;
    gettimeofday(&now, NULL);
    size_t temp = ((now.tv_sec * 1000) + (now.tv_usec /1000));
    if(temp > this->expired_time)
    {
        Debug("%s:%d【%d】超过时间片\n", __FILE__, __LINE__, ((Request*)this->request)->getSocketFd());
        
        this->isdeleted = true;
    }
        
    return this->isdeleted;
}


void Timer::clearReq()
{
    Debug("%s:%d Timer::clearReq\n", __FILE__, __LINE__);
    this->request = nullptr;
    this->isdeleted = true;
    Debug("%s:%d Timer::clearReq END\n", __FILE__, __LINE__);
}


size_t Timer::getExpTime() const
{
    return this->expired_time;
}


// Wait 这里说不定可以直接在 Timer 类中重定义比较符号
bool timerCmp::operator()(const Timer *a, const Timer *b) const
{
    return a->getExpTime() > b->getExpTime();
}