//比較線城id是否相等
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>

void *mythread(void *arg)
{
    printf("child thread, pid==[%d], id==[%ld]\n", getpid(), pthread_self());
}

int main()
{
    //創建子線程
    pthread_t thread;
    int ret = pthread_create(&thread, NULL, mythread, NULL);
    if(ret!=0)
    {
        printf("pthread_create error, [%s]\n", strerror(ret));
        return -1;
    }
    printf("main thread, pid==[%d], id==[%ld]\n", getpid(), pthread_self());

    //比較線程id
    if(pthread_equal(thread, pthread_self())!=0)
    {
        printf("two thread id is same\n");
    }
    else
    {
        printf("two thread id is not same\n");
    }

    //目的是為了回收前讓子線程執行起來
    sleep(1);
    return 0;
}