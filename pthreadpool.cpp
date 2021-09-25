#include "pthreadpool.h"
#include "util.h"
#include "log.h"
#include "request.h"

#include <pthread.h>

PthreadPool::PthreadPool():thread_count(PTHREADPOOLMAX), started(0), state(0)
{
    this->task_lock = PTHREAD_MUTEX_INITIALIZER;
    this->task_notify = PTHREAD_COND_INITIALIZER;

    this->threads = (pthread_t*)malloc(sizeof(pthread_t) * thread_count);
    for(int i=0; i<thread_count; i++)
    {
        if(pthread_create(&threads[i], NULL, threadpool_thread, (void*)this) != 0)
        {
            perror("PthreadPool::PthreadPool");
            exit(1);
        }
        this->started++;
    }
}


PthreadPool::~PthreadPool()
{
    Debug("%s:%d 线程池释放\n", __FILE__, __LINE__);
    if(this->threads == NULL || this->started > 0)
    {
        perror("PthreadPool::~PthreadPool");
        exit(1);
    }
    
    
    while(!task_queue.empty())
    {
        task_queue.pop();
        pthread_mutex_lock(&(this->task_lock));
        pthread_mutex_destroy(&(this->task_lock));
        pthread_cond_destroy(&(this->task_notify));
    }
    
}


bool PthreadPool::addTask(void (*function)(void*), void *argument, int flags)
{
    Debug("%s:%d PthreadPool::addTask\n", __FILE__, __LINE__);
    
    if(pthread_mutex_lock(&this->task_lock) != 0)
    {
        perror("PthreadPool::addTask lock");
        exit(1);
    }

    PthreadPoolTask task;
    task.function = function;
    task.argument = argument;
    this->task_queue.push(task);
    if(pthread_cond_signal(&this->task_notify) != 0)
    {
        perror("PthreadPool::addTask notify");
        pthread_mutex_unlock(&this->task_lock);
        exit(1);
    }
    if(pthread_mutex_unlock(&this->task_lock) != 0)
    {
        perror("PthreadPool::addTask unlock");
        exit(1);
    }
    Debug("%s:%d PthreadPool::addTask END\n", __FILE__, __LINE__);
    return true;
}


void PthreadPool::destory()
{
    this->state = -1;
    if((pthread_cond_broadcast(&(this->task_notify)) != 0) ||
            (pthread_mutex_unlock(&(this->task_lock)) != 0)) {
                // Note. 如果任何有线程仍然在锁，就表示有问题
                perror("PthreadPool::destory");
                exit(1);
        }
}


void* PthreadPool::threadpool_thread(void *threadpool)
{
    PthreadPool *pool = (PthreadPool*)threadpool;
    PthreadPoolTask task;
    
    while(true)
    {
        if(pthread_mutex_lock(&(pool->task_lock)) != 0)
        {
            perror("PthreadPool::threadpool_thread lock");
            exit(1);
        }
        
        while(pool->task_queue.size()==0)
        {
            Debug("%s:%d 等待条件变量\n", __FILE__, __LINE__);
            pthread_cond_wait(&(pool->task_notify), &(pool->task_lock));
            Debug("%s:%d 获得条件变量\n", __FILE__, __LINE__);
        }
            
        task =  pool->task_queue.back();
        pool->task_queue.pop();

        if(pthread_mutex_unlock(&(pool->task_lock)) != 0)
        {
            perror("PthreadPool::threadpool_thread unlock");
            exit(1);
        }       
        

        (*task.function)(task.argument);
    }
    
    pool->started--;
    pthread_mutex_unlock(&(pool->task_lock));
    pthread_exit(NULL);
    return NULL;
}

