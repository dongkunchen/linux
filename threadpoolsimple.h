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
    int tasknum;//模擬任務編號
    void *arg;//回調函數參數
    void (*task_func)(void *arg);//任務的回調函數    
}PoolTask;

typedef struct _ThreadPool
{
    int max_job_num;//最大任務個數
    int job_num;//實際任務個數
    PoolTask *tasks;//任務對列數組
    int job_push;//入隊位置
    int job_pop;//出隊位置

    int thr_num;//線程池內線程個數
    pthread_t *threads;//線程池內線程數組
    int shutdown;//是否關閉線程池
    pthread_mutex_t pool_lock;//線程池的鎖
    pthread_cond_t empty_task;//任務隊列為空的條件
    pthread_cond_t not_empty_task;//任務隊列不為空的條件

}ThreadPool;

void create_threadpool(int thrnum, int maxtasknum);//創建線程池--thrnum 代表線程個數
void destroy_threadpool(ThreadPool *pool);//摧毀線程池
void addtask(ThreadPool *pool);//添加任務到線程池
void taskRun(void *arg);//任務回調函數

#endif

