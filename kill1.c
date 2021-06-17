#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

int main()
{
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
		printf("the first child, pid==[%d]\n", getpid());
		sleep(1);
	}

	if (i == 1) 
	{
		printf("the second child, pid==[%d]\n", getpid());
		sleep(10);
	}

	if (i == 2)
	{
		printf("the third child, pid==[%d]\n", getpid());
		sleep(10);
	}

	if (i == 3)
	{
		printf("the father, pid==[%d]\n", getpid());
		sleep(10);
	}

	return 0;
}
