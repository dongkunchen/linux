#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>

int g_var = 9;

void *mythread(void *arg)
{
    printf("child thread, pid==[%d], id==[%ld]\n", getpid(), pthread_self());
    printf("[%p]\n", &g_var);
    pthread_exit(&g_var);
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

    void *p = NULL;
    pthread_join(thread, &p);
    int n = *(int *)p;
    printf("child exit status:[%d], [%p]\n", n, p);

   
    return 0;
}
