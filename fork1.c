#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

//驗證是否共享全局
int g_var = 99;
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
		g_var++;
		printf("[%p]",&g_var);//地址相同
	}
	else if(pid==0)
	{
		sleep(1);
		printf("[%p]",&g_var);//地址相同
		printf("child: pid==[%d], fpid==[%d]\n", getpid(), getppid());
		printf("child: g_var==[%d]\n", g_var);//父子進程不能共享全局變量
	}
	return 0;
}		
