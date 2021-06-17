//raise函數測試
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>

//SIGALRM信號處理函數
void sighandler(int signo)
{
    printf("signo==[%d]\n", signo);
}

int main()
{
    //註冊信號SIGALRM信號處理函數
    signal(SIGALRM, sighandler);
    
    // int setitimer(int which, const struct itimerval *new_value,
    //                  struct itimerval *old_value);
    struct itimerval tm;
    //週期性時間賦值
    tm.it_interval.tv_sec = 1;
    tm.it_interval.tv_usec = 0;
    //第一次觸發的時間
    tm.it_value.tv_sec = 3;
    tm.it_value.tv_usec = 0;

    setitimer(ITIMER_REAL, &tm, NULL);

    while (1)
    {
        sleep(1);
    }
    
    return 0;
}