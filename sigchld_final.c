//父進程使用SIGCHLD信號完成對子進程的回收
//解決所有子進程先死掉後出現的三個殭屍進程
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

void waichild(int signo)
{
	pid_t wpid;

	while (1)
	{
		wpid = waitpid(-1, NULL, WNOHANG);
		if (wpid > 0)
		{
			printf("childe is quit, wpid==[%d]\n", wpid);
		}
		else if (wpid == 0)
		{
			printf("childe is living, wpid==[%d]\n", wpid);
			break;
		}
		else if (wpid == -1)
		{
			printf("no child is living, wpid==[%d]\n", wpid);
			break;
		}
	}
}

int main(int argc, char *argv[])
{
	pid_t pid;
	int i = 0;

	//將SIGCHLD阻塞
	sigset_t mask;
	sigemptyset(&mask);
	sigaddset(&mask, SIGCHLD);
	sigprocmask(SIG_BLOCK, &mask, NULL);

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

	if (i == 0) //第一個子進程
	{
		printf("[%d]---[%d]: child\n", i, getpid());
		// sleep(1);
	}

	if (i == 1) //第二個子進程
	{
		printf("[%d]---[%d]: child\n", i, getpid());
		// sleep(1);
	}

	if (i == 2) //第三個子進程
	{
		printf("[%d]---[%d]: child\n", i, getpid());
		// sleep(3)
	}

	if (i == 3) //父進程
	{
		printf("[%d]---[%d]: parent\n", i, getpid());
		struct sigaction act;
		act.sa_handler = waichild;
		sigemptyset(&act.sa_mask);
		act.sa_flags = 0;
		sleep(5);
		sigaction(SIGCHLD, &act, NULL);

		//完成SIGCHLD信號
		sigprocmask(SIG_UNBLOCK, &mask, NULL);

		while (1)
		{
			sleep(1);
		}
	}

	return 0;
}