#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

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
		sleep(20);
		printf("father: [%d], pid==[%d], fpid==[%d]\n", pid, getpid(), getppid());
	}
	else if(pid==0)
	{
		printf("child: pid==[%d], fpid==[%d]\n", getpid(), getppid());
	}
	return 0;
}		
