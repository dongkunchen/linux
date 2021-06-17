//驗證SIGCHLD信號如何產生
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

void sighandler(int signo)
{
    printf("signo==[%d]\n", signo);
}

int main()
{

	int i = 0;

	signal(SIGCHLD, sighandler);

	pid_t pid = fork();


	if (pid < 0)
	{
		perror("fork error");
		return -1;
	}
	else if (pid > 0)
	{
		printf("father process, pid==[%d] child pid==[%d]\n", getpid(), pid);
		while (1)
		{
			sleep(1);
		}
	}
	else
	{
		printf("child process, father pid==[%d], pid==[%d]: child\n", getppid(), getpid());
		while (1)
		{
			sleep(1);
		}
	}

	return 0;
}
