#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>

#define NUM 5000

int number = 0;

pthread_mutex_t mutex;

void *mythread1(void *arg)
{
    int i = 0;
    int n;
    for (i = 0; i < NUM; i++)
    {
        pthread_mutex_lock(&mutex);
        n = number;
        n++;
        number = n;
        printf("1:[%d]\n", number);
        pthread_mutex_unlock(&mutex);
    }
}

void *mythread2(void *arg)
{
    int i = 0;
    int n;
    for (i = 0; i < NUM; i++)
    {
        pthread_mutex_lock(&mutex);
        n = number;
        n++;
        number = n;
        printf("2:[%d]\n", number);
        pthread_mutex_unlock(&mutex);
    }
}

int main()
{
    pthread_mutex_init(&mutex, NULL);

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

    pthread_mutex_destroy(&mutex);

    printf("number==[%d]\n",number);

    return 0;
}
