#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
	int i = 0;
	int n = 3;
	for (i = 0; i < n; i++)
	{
		pid_t pid = fork();
		if (pid < 0)
		{
			perror("fork error");
			return -1;
		}
		else if (pid > 0)
		{
			printf("father: fpid==[%d], cpid==[%d]\n", getpid(), pid);
			sleep(1);//也可放到下面一起
		}
		else if (pid == 0)
		{
			printf("child: fpid==[%d], cpid==[%d]\n", getppid(), getpid());
			break;
		}
	}

	if (i == 3) //父進程
	{
		printf("[%d]:father: fpid==[%d]\n", i, getpid());
		pid_t wpid;
		int wstatus;
		while (1)
		{
			pid_t wpid = waitpid(-1, &wstatus, WNOHANG); //跟wait差別
			if(wpid==0)//子進程還活著
			{
				continue;
			}
			else if(wpid==-1)
			{
				printf("no child is living, wpid==[%d]\n", wpid);
				exit(0);
			}
			else if (wpid > 0)
			{
				if (WIFEXITED(wstatus)) //正常退出
				{
					printf("chile normal exit, status==[%d]\n", WEXITSTATUS(wstatus));
				}
				else if (WIFSIGNALED(wstatus)) //被信號殺死
				{
					printf("child killed by singnal, signo==[%d]\n", WTERMSIG(wstatus));
				}
			}
		}
	}

	if (i == 0) //第一個子進程
	{
		printf("[%d]:child: cpid==[%d]\n", i, getpid());
		execlp("ls","ls","-l",NULL);
		perror("excel error");
		exit(-1);
	}

	if (i == 1) //第二個子進程
	{
		printf("[%d]:child: cpid==[%d]\n", i, getpid());
		execl("/hello","hello","1111111",NULL);
		perror("excel error");
		return -1;
	}

	if (i == 2) //第三個子進程
	{
		printf("[%d]---[%d]: parent\n", i, getpid());
		execl("/test","test",NULL);
		perror("excel error");
		return -1;
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
