#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

void sighandler(int signo)
{
    printf("signo==[%d]\n", signo);
}

int main()
{
    signal(SIGALRM, sighandler);


    int n = alarm(5);
    printf("n==[%d]\n", n);

    sleep(2);
    n = alarm(1);
    printf("n==[%d]\n", n);

    sleep(10);
    return 0;
}
