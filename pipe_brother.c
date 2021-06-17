#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

int main()
{
    int fd[2];
    int ret = pipe(fd);
    pid_t pid;
    if (ret < 0)
    {
        perror("pipe error");
        return -1;
    }

    int i = 0;
    int n = 2;
    for (i = 0; i < n; i++)
    {
        pid_t pid = fork();
        if (pid < 0)
        {
            perror("fork error");
            return -1;
        }
        else if (pid == 0)
        {
            break;
        }
    }

    if (i == n)
    {
        close(fd[0]);
        close(fd[1]);
        pid_t wpid;
        int wstatus;
        while (1)
        {
            if (wpid == 0) 
            {
                sleep(1);
                continue;
            }
            else if (wpid == -1)
            {
                printf("no child is living, wpid==[%d]\n", wpid);
                exit(0);
            }
            else if (wpid > 0)
            {
                if (WIFEXITED(wstatus)) 
                {
                    printf("chile normal exit, status==[%d]\n", WEXITSTATUS(wstatus));
                }
                else if (WIFSIGNALED(wstatus)) 
                {
                    printf("child killed by singnal, signo==[%d]\n", WTERMSIG(wstatus));
                }
            }
        }
    }
    
    
   
	if (i == 0) 
	{
		close(fd[0]);

        dup2(fd[1], STDOUT_FILENO);
        execlp("ps", "ps", "aux", NULL);
        perror("execlp error");

        close(fd[1]);
	}

	if (i == 1) 
	{
		printf("child: fpid==[%d], cpid==[%d]\n", getppid(), getpid());
        close(fd[1]);

        dup2(fd[0], STDIN_FILENO);
        execlp("grep", "grep", "--color","bash", NULL);
        perror("execlp error");

        close(fd[0]);

    }

    return 0;
}
