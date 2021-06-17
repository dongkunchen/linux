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
    //創建管道int pipe(int pipefd[2]);
    int fd[2];
    int ret = pipe(fd);
    if (ret < 0)
    {
        perror("pipe error");
        return -1;
    }
    
    //註冊SIGPIPE信號處理函數
    signal(SIGPIPE, sighandler);

    //關閉讀端
    close(fd[0]);

    write(fd[1], "hello world", strlen("hello world"));

    return 0;
}