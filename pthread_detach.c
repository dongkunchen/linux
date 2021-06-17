//設置子線程為分離屬性
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
    pthread_t thread;
    int ret = pthread_create(&thread, NULL, mythread, NULL);
    if(ret!=0)
    {
        printf("pthread_create error, [%s]\n", strerror(ret));
        return -1;
    }
    printf("main thread, pid==[%d], id==[%ld]\n", getpid(), pthread_self());

    //設置線程為分離屬性
    pthread_detach(thread);

    //驗證是否分離可調用join測試
    ret = pthread_join(thread, NULL);
    if(ret!=0)
    {
        printf("pthread_join error:[%s]\n",strerror(ret));
    }
    //目的是為了回收前讓子線程執行起來
    sleep(1);
    return 0;
}