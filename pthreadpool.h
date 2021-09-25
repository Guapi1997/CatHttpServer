#ifndef PTHREADPOOL 
#define PTHREADPOOL

#include <pthread.h>
#include <queue>
using std::queue;

const int PTHREADPOOLMAX = 10;

typedef struct {
    void (*function)(void*);
    void *argument;
} PthreadPoolTask;


class PthreadPool
{
private:
    int thread_count; // 线程池大小
    int started;  // 运行线程数量
    int state; // 线程状态
    pthread_mutex_t task_lock;
    pthread_cond_t task_notify;
    pthread_t *threads;
    queue<PthreadPoolTask> task_queue;

public:
    PthreadPool();
    ~PthreadPool();
    void destory();
    
    bool addTask(void (*function)(void*), void *argument, int flags);
    static void* threadpool_thread(void* arg);
};

void myHandle(void *arg);

#endif