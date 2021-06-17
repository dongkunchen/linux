#include "threadpoolsimple.h"
 
ThreadPool *thrPool = NULL;
 
int beginnum = 1000;
 
void *thrRun(void *arg)
{
    ThreadPool *pool = (ThreadPool*)arg;
    int taskpos = 0;
    PoolTask *task = (PoolTask *)malloc(sizeof(PoolTask));
    while(1){
        pthread_mutex_lock(&thrPool->pool_lock);
        while(thrPool->job_num <= 0 && !thrPool->shutdown ){
            pthread_cond_wait(&thrPool->not_empty_task,&thrPool->pool_lock);
        }
        
        if(thrPool->job_num){
            taskpos = (thrPool->job_pop++)% thrPool->max_job_num;
            memcpy(task,&thrPool->tasks[taskpos],sizeof(PoolTask));
            task->arg = task;
            thrPool->job_num --;
            pthread_cond_signal(&thrPool->empty_task);
        }
        if(thrPool->shutdown){
            pthread_mutex_unlock(&thrPool->pool_lock);
            pthread_exit(NULL);
        }
 
        pthread_mutex_unlock(&thrPool->pool_lock);
        task->task_func(task->arg);
    }
    free(task);
}
void create_threadpool(int thrnum,int maxtasknum)
{
    printf("begin call %s-----\n",__FUNCTION__);
    thrPool = (ThreadPool*)malloc(sizeof(ThreadPool));
    thrPool->thr_num = thrnum;
    thrPool->max_job_num = maxtasknum;
    thrPool->shutdown = 0;
    thrPool->job_push = 0;
    thrPool->job_pop = 0;
    thrPool->job_num = 0;
    thrPool->tasks = (PoolTask*)malloc((sizeof(PoolTask)*maxtasknum));
    pthread_mutex_init(&thrPool->pool_lock,NULL);
    pthread_cond_init(&thrPool->empty_task,NULL);
    pthread_cond_init(&thrPool->not_empty_task,NULL);
    int i = 0;
    thrPool->threads = (pthread_t *)malloc(sizeof(pthread_t)*thrnum);
    for(i = 0;i < thrnum;i ++){
        pthread_create(&thrPool->threads[i],NULL,thrRun,(void*)thrPool);
    }
    printf("end call %s-----\n",__FUNCTION__);
}
void destroy_threadpool(ThreadPool *pool)
{
    pool->shutdown = 1;
    pthread_cond_broadcast(&pool->not_empty_task);
    int i = 0;
    for(i = 0; i < pool->thr_num ; i ++){
        pthread_join(pool->threads[i],NULL);
    }
    pthread_cond_destroy(&pool->not_empty_task);
    pthread_cond_destroy(&pool->empty_task);
    pthread_mutex_destroy(&pool->pool_lock);
 
    free(pool->tasks);
    free(pool->threads);
    free(pool);
}
void addtask(ThreadPool *pool)
{
    pthread_mutex_lock(&pool->pool_lock);
    while(pool->max_job_num <= pool->job_num){
        pthread_cond_wait(&pool->empty_task,&pool->pool_lock);
    }
    int taskpos = (pool->job_push++)%pool->max_job_num;
    pool->tasks[taskpos].tasknum = beginnum++;
    pool->tasks[taskpos].arg = (void*)&pool->tasks[taskpos];
    pool->tasks[taskpos].task_func = taskRun;
    pool->job_num ++;
    pthread_mutex_unlock(&pool->pool_lock);
 
 
    pthread_cond_signal(&pool->not_empty_task);
}

void taskRun(void *arg)
{
    PoolTask *task = (PoolTask*)arg;
    int num = task->tasknum;
    printf("task %d is runing %lu\n",num,pthread_self());
    sleep(1);
    printf("task %d is done %lu\n",num,pthread_self());
}
 
int main()
{
    create_threadpool(3,20);
    int i = 0;
    for(i = 0;i < 50 ; i ++){
        addtask(thrPool);
    }
    sleep(20);
    destroy_threadpool(thrPool);
    return 0;
}
