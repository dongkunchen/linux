//kill函數測試:父進程殺死子進程 或 子進程殺死父進程
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
			//父進程殺死第一個子進程
			// if(i==0)
			// {
			// 	kill(pid,SIGKILL);
			// }
		}
		else if (pid == 0)
		{
			printf("child: pid==[%d], fpid==[%d]\n", getpid(), getppid());
			break;
		}
	}

	if (i == 0) //第一個子進程
	{
		printf("the first child, pid==[%d]\n", getpid());
		//測試子進程殺死父進程
		//kill(getppid(),SIGKILL);
		// sleep(10);
		sleep(1);
		//0殺死同一組
		//kill(0,SIGKILL); //-1很危險連窗口都殺了
	}

	if (i == 1) //第二個子進程
	{
		printf("the second child, pid==[%d]\n", getpid());
		sleep(10);
	}

	if (i == 2) //第三個子進程
	{
		printf("the third child, pid==[%d]\n", getpid());
		sleep(10);
	}

	if (i == 3) //父進程
	{
		printf("the father, pid==[%d]\n", getpid());
		sleep(10);
	}

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
