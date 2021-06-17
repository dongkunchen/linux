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
			//sleep(1);//也可放到下面一起
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
	}

	if (i == 1) //第二個子進程
	{
		printf("[%d]---[%d]: child\n", i, getpid());
	}

	if (i == 2) //第三個子進程
	{
		printf("[%d]---[%d]: child\n", i, getpid());
	}

	if (i == 3) //父進程
	{
		printf("[%d]---[%d]: parent\n", i, getpid());
	}

	sleep(10);

	return 0;
}

//會產生 2的n次方-1個子進程  2的n次方進程(+一個父進程)
// int main()
// {
// 	int i = 0;
// 	for (i = 0; i < 3; i++)
// 	{
// 		pid_t pid = fork();
// 		if (pid < 0)
// 		{
// 			perror("fork error");
// 			return -1;
// 		}
// 		else if (pid > 0)
// 		{
// 			printf("father: pid==[%d]\n", getpid());
// 			sleep(1);
// 		}
// 		else if (pid == 0)
// 		{
// 			printf("child: pid==[%d]\n", getpid());
// 		}
// 	}
// 	return 0;
// }
