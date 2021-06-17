#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
    int fd[2];
    int ret = pipe(fd);
    if(ret<0)
    {
        perror("pipe error");
        return -1;
    }

    pid_t pid = fork();
    if(pid<0)
    {
        perror("fork error");
        return -1;
    }
    else if(pid>0)
    {
        close(fd[0]);
        
        dup2(fd[1], STDOUT_FILENO);

        execlp("ps", "ps", "aux", NULL);
        perror("execlp error");        
    }
    else
    {
        close(fd[1]);

        dup2(fd[0], STDIN_FILENO);

        // execlp("grep", "grep", "bash", NULL);
        execlp("grep", "grep","--color=auto","bash", NULL);
        perror("execlp error");
    }
    return 0;
}
