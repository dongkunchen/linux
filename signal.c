#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

void sighandler(int signo)
{
    printf("signo==[%d]\n", signo);
}

int main()
{
    int fd[2];
    int ret = pipe(fd);
    if (ret < 0)
    {
        perror("pipe error");
        return -1;
    }
    
    signal(SIGPIPE, sighandler);

    close(fd[0]);

    write(fd[1], "hello world", strlen("hello world"));

    return 0;
}
