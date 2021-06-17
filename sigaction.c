//sigaction測試:完成信號的註冊
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

//信號處理函數
void sighandler(int signo)
{
    printf("signo==[%d]\n", signo);
    sleep(3);
}

int main()
{
    //註冊SIGINT信號處理函數
    struct sigaction act;
    act.sa_handler = sighandler;//信號處理函數
    sigemptyset(&act.sa_mask);//阻塞的信號
    act.sa_flags = 0;
    sigaction(SIGINT, &act, NULL);

    while(1)
    {
        sleep(1);
    }

    return 0;
}