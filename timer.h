#ifndef SOCKETTIMER
#define SOCKETTIMER
#include <sys/time.h>
#include <pthread.h>
#include <queue>
#include <deque>
using std::priority_queue;
using std::deque;

/*首先 读时间队列需要加锁 */
class Timer;

const int MAXTIMEOUT = 500;

struct timerCmp
{
    bool operator()(const Timer *a, const Timer *b) const;
};


// 超时的时间器一定要连 socket 和 epoll 一起删除

class Timer
{
public:
    static pthread_mutex_t timer_qlock;
    static void ExpiredTimer();
    static void push(Timer*);

private:
    int timeout;
    bool isdeleted;
    void *request;
    size_t expired_time;
    static priority_queue<Timer*, deque<Timer*>, struct timerCmp> TimerQueue;
    
public:
    Timer(void *);
    ~Timer();
    bool isDeleted();
    void clearReq();
    size_t getExpTime() const;
};

#endif