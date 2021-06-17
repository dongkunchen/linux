//線程同步-互斥鎖
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>

//定義一把互斥鎖
pthread_mutex_t mutex;

void *mythread1(void *arg)
{
    while (1)
    {
        //加鎖
        pthread_mutex_lock(&mutex);
        printf("hello ");
        sleep(rand() % 3);
        printf("world\n");
        pthread_mutex_unlock(&mutex);
        sleep(rand() % 3);
    }
    pthread_exit(NULL);
}

void *mythread2(void *arg)
{
    while (1)
    {
        pthread_mutex_lock(&mutex);
        printf("HELLO ");
        sleep(rand() % 3);
        printf("WORLD\n");
        pthread_mutex_unlock(&mutex);
        sleep(rand() % 3);
    }
    pthread_exit(NULL);
}

int main()
{
    //互斥鎖初始化要在主線程內
    pthread_mutex_init(&mutex, NULL);

    //創建子線城
    pthread_t thread1;
    int ret = pthread_create(&thread1, NULL, mythread1, NULL);
    if (ret != 0)
    {
        printf("pthread_create error, [%s]\n", strerror(ret));
        return -1;
    }

    pthread_t thread2;
    ret = pthread_create(&thread2, NULL, mythread2, NULL);
    if (ret != 0)
    {
        printf("pthread_create error, [%s]\n", strerror(ret));
        return -1;
    }

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    //釋放互斥鎖
    pthread_mutex_destroy(&mutex);

    return 0;
}