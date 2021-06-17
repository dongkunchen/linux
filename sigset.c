//signal函數測試給沒有讀端的信號產生SIGPIPE信號
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

//信號處理函數
void sighandler(int signo)
{
    printf("signo==[%d]\n", signo);
}

int main()
{
    //註冊信號處理函數
    signal(SIGINT, sighandler);
    signal(SIGQUIT, sighandler);
    //定義信號集變量

    sigset_t set;

    //初始化信號集
    sigemptyset(&set);

    //將SIGINT SIGQUIT加入set集合中
    sigaddset(&set, SIGINT);
    sigaddset(&set, SIGQUIT);

    //將set集合中的SIGINT SIGQUIT加入阻塞信號集中
    sigprocmask(SIG_BLOCK, &set, NULL);

    int i = 0;
    int j = 1;
    sigset_t pend;

    while (1)
    {
        //獲取未決信號集
        sigemptyset(&pend);
        sigpending(&pend);
        for (i = 1; i < 32; i++)
        {
            if (sigismember(&pend, i) == 1)
            {
                printf("1");
            }
            else
            {
                printf("0");
            }
        }
        printf("\n");

        //循環10次解除對SIGINT SIGQUIT信號的阻塞
        if(j++%10==0)
        {
            sigprocmask(SIG_UNBLOCK, &set, NULL);
        }
        else
        {
            sigprocmask(SIG_BLOCK, &set, NULL);
        }
        sleep(1);
    }
    return 0;
}