#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

void sighandler(int signo)
{
    printf("signo==[%d]\n", signo);
}

int main()
{
    signal(SIGINT, sighandler);
    signal(SIGQUIT, sighandler);

    sigset_t set;

    sigemptyset(&set);

    sigaddset(&set, SIGINT);
    sigaddset(&set, SIGQUIT);

    sigprocmask(SIG_BLOCK, &set, NULL);

    int i = 0;
    int j = 1;
    sigset_t pend;

    while (1)
    {
        sigemptyset(&pend);
        sigpending(&pend);
        for (i = 1; i < 32; i++)
        {
            if (sigismember(&pend, i) == 1)
            {
                printf("1");
            }
            else
            {
                printf("0");
            }
        }
        printf("\n");

        if(j++%10==0)
        {
            sigprocmask(SIG_UNBLOCK, &set, NULL);
        }
        else
        {
            sigprocmask(SIG_BLOCK, &set, NULL);
        }
        sleep(1);
    }
    return 0;
}
