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
    pthread_attr_t attr;

    pthread_attr_init(&attr);

    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    pthread_t thread;
    int ret = pthread_create(&thread, &attr , mythread, NULL);
    if(ret!=0)
    {
        printf("pthread_create error, [%s]\n", strerror(ret));
        return -1;
    }
    printf("main thread, pid==[%d], id==[%ld]\n", getpid(), pthread_self());

    pthread_attr_destroy(&attr);

    ret = pthread_join(thread, NULL);
    if(ret!=0)
    {
        printf("pthread_join error:[%s]\n", strerror(ret));
    }

    return 0;
}
