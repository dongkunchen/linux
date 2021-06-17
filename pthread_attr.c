//創建線程時設置分離屬性
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>

void *mythread(void *arg)
{
    printf("child thread, pid==[%d], id==[%ld]\n", getpid(), pthread_self());
    sleep(2);
}

int main()
{
    //定義pthread_attr_t類型的變量;
    pthread_attr_t attr;

    //初始化attr變量
    pthread_attr_init(&attr);

    //設置attr為分離屬性
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    //創建子線程
    pthread_t thread;
    int ret = pthread_create(&thread, &attr , mythread, NULL);
    if(ret!=0)
    {
        printf("pthread_create error, [%s]\n", strerror(ret));
        return -1;
    }
    printf("main thread, pid==[%d], id==[%ld]\n", getpid(), pthread_self());

    //釋放線程屬性
    pthread_attr_destroy(&attr);

    //驗證如果線程阻塞就是非分離 沒阻塞就是分離 用sleep看有無立刻返回
    ret = pthread_join(thread, NULL);
    if(ret!=0)
    {
        printf("pthread_join error:[%s]\n", strerror(ret));
    }

    return 0;
}