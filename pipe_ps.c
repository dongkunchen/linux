//使用pipe 完成ps aux | grep bash操作
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
    //創建管道int pipe(int pipefd[2]);
    int fd[2];
    int ret = pipe(fd);
    if(ret<0)
    {
        perror("pipe error");
        return -1;
    }

    //創建子進程
    pid_t pid = fork();
    if(pid<0)
    {
        perror("fork error");
        return -1;
    }
    else if(pid>0)
    {
        //關閉讀端(0讀1寫)
        close(fd[0]);
        
        //將標準輸出重定向到寫端
        dup2(fd[1], STDOUT_FILENO);

        execlp("ps", "ps", "aux", NULL);
        perror("execlp error");        
    }
    else
    {
        //關閉寫端
        close(fd[1]);

        //將標準輸入重定向到讀端
        dup2(fd[0], STDIN_FILENO);

        // execlp("grep", "grep", "bash", NULL);
        execlp("grep", "grep","--color=auto","bash", NULL);//加上顏色
        perror("execlp error");
    }
    return 0;
}