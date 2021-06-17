#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>

void myfunc(int signo)
{
    int fd = open("mydeamon.log", O_RDWR | O_CREAT | O_APPEND, 0755);
    if(fd<0)
    {
        return;
    }

    time_t t;
    time(&t);
    char *p = ctime(&t);

    write(fd, p, strlen(p));

    close(fd);
    
    return;
}

int main()
{
    pid_t pid = fork();
    if (pid < 0 || pid > 0)
    {
        exit(1);
    }

    setsid();

    chdir("/home/ig26/log");

    umask(0000);

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    struct sigaction act;
    act.sa_handler = myfunc;
    act.sa_flags = 0;
    sigemptyset(&act.sa_mask);
    sigaction(SIGALRM, &act, NULL);

    struct itimerval tm;
    tm.it_interval.tv_sec = 2;
    tm.it_interval.tv_usec = 0;
    tm.it_value.tv_sec = 3;
    tm.it_value.tv_usec = 0;
    setitimer(ITIMER_REAL, &tm, NULL);

    printf("hello world\n");

    while (1)
    {
        sleep(1);
    }
    
}
