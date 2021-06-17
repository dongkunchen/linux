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
		printf("father: pid==[%d], fpid==[%d]\n", getpid(), getppid());
		sleep(1);
	}
	else if(pid==0)
	{
		printf("child: pid==[%d], fpid==[%d]\n", getpid(), getppid());
		// execl("/bin/ls","ls","-l",NULL);
		// execl("./test","test","hello","world","ni","hao",NULL);
		// execlp("ls","ls","-l",NULL);//execlp不帶路徑使用環境變量
		execlp("./test","test","hello","world","ni","hao",NULL);
		perror("execl error");
	}
	return 0;
}		
