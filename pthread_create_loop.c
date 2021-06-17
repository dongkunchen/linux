//尋換創建子線程
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>

void *mythread(void *arg)
{
    int i = *(int *)arg;
    printf("[%d]:child thread, pid==[%d], id==[%ld]\n", i,  getpid(), pthread_self());
}

int main()
{
    //創建子線程
    int ret;
    int i = 0;
    int n = 5;
    int arr[5];
    pthread_t thread[5];
    for (i = 0; i < n; i++)
    {
        arr[i] = i;
        ret = pthread_create(&thread[i], NULL, mythread, &arr[i]);
        if (ret != 0)
        {
            printf("pthread_create error, [%s]\n", strerror(ret));
            return -1;
        }
        // sleep(1);
    }

    printf("main thread, pid==[%d], id==[%ld]\n", getpid(), pthread_self());

    //目的是為了回收前讓子線程執行起來
    sleep(1);
    return 0;
}