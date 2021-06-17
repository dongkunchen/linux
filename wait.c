#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
	pid_t pid = fork();
	if(pid<0)
	{
		perror("fork error");
		return -1;
	}
	else if(pid>0)
	{
		printf("father: [%d], pid==[%d], fpid==[%d]\n", pid, getpid(), getppid());
		//pid_t wpid = wait(NULL);
		int wstatus;
		pid_t wpid = wait(&wstatus);
		printf("wpid==[%d\n]", wpid);
		if(WIFEXITED(wstatus))//正常退出
		{
			printf("chile normal exit, status==[%d]\n",  WEXITSTATUS(wstatus));
		}
		else if(WIFSIGNALED(wstatus))//被信號殺死
		{
			printf("child killed by singnal, signo==[%d]\n",  WTERMSIG(wstatus));
		}
	}
	else if(pid==0)
	{
		printf("child: pid==[%d], fpid==[%d]\n", getpid(), getppid());
		sleep(5);
		return 9;
	}
	return 0;
}		
