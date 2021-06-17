//讀寫鎖測試程序
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>

int number = 0;

//定義一把讀寫鎖
pthread_rwlock_t rwlock;

//寫線程回調函數
void *thread_write(void *arg)
{
    int i = *(int *)arg;
    int cur;
    while(1)
    {
        //加寫鎖
        pthread_rwlock_wrlock(&rwlock);
        cur = number;
        cur++;
        number = cur;
        printf("[%d]-W:[%d]\n", i, cur);

        //解鎖
        pthread_rwlock_unlock(&rwlock);
        sleep(rand()%3);
    }
}

void *thread_read(void *arg)
{
    int i = *(int *)arg;
    int cur;
    while(1)
    {
        //加讀鎖
        pthread_rwlock_rdlock(&rwlock);

        cur = number;
        printf("[%d]-R:[%d]\n", i, cur);
        
        //解鎖加在等待上面
        pthread_rwlock_unlock(&rwlock);
        sleep(rand()%3);

    }
}

int main()
{
    //創建子線程
    int n = 8;
    int i = 0;
    int arr[8];
    pthread_t thread[8];

    //初始化讀寫鎖
    pthread_rwlock_init(&rwlock, NULL);

    //創建三個寫子線程
    for (i = 0; i < 3; i++)
    {
        arr[i] = i;
        pthread_create(&thread[i], NULL, thread_write, &arr[i]);
    }
    //創建五個讀子線程
    for (i = 3; i < n; i++)
    {
        arr[i] = i;
        pthread_create(&thread[i], NULL, thread_read, &arr[i]);
    }

    int j = 0;
    for (j = 0; j < n; j++)
    {
        pthread_join(thread[j], NULL);
    }
    
    pthread_rwlock_destroy(&rwlock);
    
    return 0;
}
