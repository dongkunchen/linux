//創建子線程:傳遞參數
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>

struct Test
{
    int data;
    char name[64];
};

void *mythread(void *arg)
{
    // int n = *(int *)arg;
    //int *p = (int *)arg;
    struct Test *p = (struct Test *)arg; 
    // printf("n==[%d]\n", n);
    printf("[%d],[%s]\n", p->data, p->name);
    printf("child thread, pid==[%d], id==[%ld]\n", getpid(), pthread_self());
}

int main()
{
    int n = 99;
    struct Test t;
    memset(&t, 0x00, sizeof(struct Test));
    t.data = 88;
    stpcpy(t.name, "xiaowen");

    pthread_t thread;
    // int ret = pthread_create(&thread, NULL, mythread, &n);
    int ret = pthread_create(&thread, NULL, mythread, &t);
    if(ret!=0)
    {
        printf("pthread_create error, [%s]\n", strerror(ret));
        return -1;
    }
    printf("main thread, pid==[%d], id==[%ld]\n", getpid(), pthread_self());

    //目的是為了回收前讓子線程執行起來
    sleep(1);
    return 0;
}