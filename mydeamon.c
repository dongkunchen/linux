//創建守護進程
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>

void myfunc(int signo)
{
    //打開文件
    int fd = open("mydeamon.log", O_RDWR | O_CREAT | O_APPEND, 0755);
    if(fd<0)
    {
        return;
    }

    //獲取當前的系統時間
    time_t t;
    time(&t);
    char *p = ctime(&t);

    //將時間寫入文件
    write(fd, p, strlen(p));

    close(fd);
    
    return;
}

int main()
{
    //父進程fork子進程然後父進程退出
    pid_t pid = fork();
    if (pid < 0 || pid > 0)
    {
        exit(1);
    }

    //子進程調用setsid函數創建會話
    setsid();

    //改變當前工作目錄
    chdir("/home/ig26/log");

    //改變文件掩碼
    umask(0000);

    //關閉標準輸入 輸出 和 錯誤輸出 文件描述符
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    //核心代碼操作
    struct sigaction act;
    act.sa_handler = myfunc;
    act.sa_flags = 0;
    sigemptyset(&act.sa_mask);
    sigaction(SIGALRM, &act, NULL);

    //設置時鐘
    struct itimerval tm;
    tm.it_interval.tv_sec = 2;
    tm.it_interval.tv_usec = 0;
    tm.it_value.tv_sec = 3;
    tm.it_value.tv_usec = 0;
    setitimer(ITIMER_REAL, &tm, NULL);

    //測試是否擺脫終端輸出
    printf("hello world\n");

    while (1)
    {
        sleep(1);
    }
    
}