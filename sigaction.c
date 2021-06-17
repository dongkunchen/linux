#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

void sighandler(int signo)
{
    printf("signo==[%d]\n", signo);
    sleep(3);
}

int main()
{
    struct sigaction act;
    act.sa_handler = sighandler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGINT, &act, NULL);

    while(1)
    {
        sleep(1);
    }

    return 0;
}
