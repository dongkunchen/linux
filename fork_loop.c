#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

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
		printf("[%d]---[%d]: child\n", i, getpid());
	}

	if (i == 1)
	{
		printf("[%d]---[%d]: child\n", i, getpid());
	}

	if (i == 2) 
	{
		printf("[%d]---[%d]: child\n", i, getpid());
	}

	if (i == 3) 
	{
		printf("[%d]---[%d]: parent\n", i, getpid());
	}

	sleep(10);

	return 0;
}

