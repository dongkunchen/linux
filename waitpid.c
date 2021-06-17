#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
	pid_t pid = fork();
	if (pid < 0)
	{
		perror("fork error");
		return -1;
	}
	else if (pid > 0)
	{
		printf("father: [%d], pid==[%d], fpid==[%d]\n", pid, getpid(), getppid());
		//pid_t wpid = wait(NULL);
		int wstatus;
		//pid_t wpid = waitpid(pid,&wstatus,0);//跟wait
		//WNOHANG表示不阻塞
		while (1)
		{
			pid_t wpid = waitpid(-1, &wstatus, WNOHANG); //跟wait差別
			// printf("wpid==[%d\n]", wpid);
			if (wpid > 0)
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
			else if(wpid==0)//子進程還活著
			{
				//printf("chlid is living,wpid==[%d]\n", wpid);
			}
			else if(wpid==-1)
			{
				printf("no child is living, wpid==[%d]\n", wpid);
				break;
			}
		}
	}
	else if (pid == 0)
	{
		printf("child: pid==[%d], fpid==[%d]\n", getpid(), getppid());
		sleep(2);
		return 9;
	}
	return 0;
}
