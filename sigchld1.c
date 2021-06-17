#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

void waichild(int signo)
{
	pid_t wpid = waitpid(-1, NULL, WNOHANG);
	if(wpid>0)
	{
		printf("childe is quit, wpid==[%d]\n", wpid);
	}
	else if(wpid==0)
	{
		printf("childe is living, wpid==[%d]\n", wpid);
	}
	else if(wpid==-1)
	{
		printf("no child is living, wpid==[%d]\n", wpid);
	}
}

int main(int argc, char *argv[])
{
	pid_t pid;
	int i = 0;
	for (i = 0; i < 3; i++)
	{
		pid_t pid = fork();
		if (pid < 0)
		{
			perror("fork error");
			return -1;
		}
		else if (pid > 0)
		{
			printf("father: pid==[%d], fpid==[%d]\n", getpid(), getppid());
		}
		else if (pid == 0)
		{
			printf("child: pid==[%d], fpid==[%d]\n", getpid(), getppid());
			break;
		}
	}

	if (i == 0) 
	{
		printf("[%d]---[%d]: child\n", i, getpid());
		sleep(1);
	}

	if (i == 1) 
	{
		printf("[%d]---[%d]: child\n", i, getpid());
		sleep(2);
	}

	if (i == 2) 
	{
		printf("[%d]---[%d]: child\n", i, getpid());
		sleep(3);
	}

	if (i == 3) 
	{
		printf("[%d]---[%d]: parent\n", i, getpid());
		struct sigaction act;
		act.sa_handler = waichild;
		sigemptyset(&act.sa_mask);
		act.sa_flags = 0;
		sigaction(SIGCHLD, &act, NULL);
		while (1)
		{
			sleep(1);
		}
		
	}

	return 0;
}

