//取消線程函數
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>

void *mythread(void *arg)
{
    while(1)
    {
        int a;
        int b;
        //設置取消點
        pthread_testcancel();
    }
}

int main()
{
    pthread_t thread;
    int ret = pthread_create(&thread, NULL, mythread, NULL);
    if(ret!=0)
    {
        printf("pthread_create error, [%s]\n", strerror(ret));
        return -1;
    }
    printf("main thread, pid==[%d], id==[%ld]\n", getpid(), pthread_self());

    //取消子線程(沒設置取消點沒做用)
    pthread_cancel(thread);

    pthread_join(thread, NULL);
    //目的是為了回收前讓子線程執行起來
    sleep(1);
    return 0;
}