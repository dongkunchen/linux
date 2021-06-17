#ifndef _THREADPOOL_H
#define _THREADPOOL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>

typedef struct _PoolTask
{
    int tasknum;
    void *arg;
    void (*task_func)(void *arg);
}PoolTask;

typedef struct _ThreadPool
{
    int max_job_num;
    int job_num;
    PoolTask *tasks;
    int job_push;
    int job_pop;

    int thr_num;
    pthread_t *threads;
    int shutdown;
    pthread_mutex_t pool_lock;
    pthread_cond_t empty_task;
    pthread_cond_t not_empty_task;

}ThreadPool;

void create_threadpool(int thrnum, int maxtasknum);
void destroy_threadpool(ThreadPool *pool);
void addtask(ThreadPool *pool);
void taskRun(void *arg);

#endif

