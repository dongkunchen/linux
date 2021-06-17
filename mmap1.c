#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>

int main()
{
    int fd = open("./test.log", O_RDWR);
    if(fd<0)
    {
        perror("open error");
        return -1;
    }
    int len = lseek(fd, 0, SEEK_END);

    void * addr = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(addr==MAP_FAILED)
    {
        perror("mmap error");
        return -1;
    }
    pid_t pid = fork();
    if(pid<0)
    {
        perror("fork error");
        return -1;
    }
    else if(pid>0)
    {
        memcpy(addr, "hello world", strlen("hello world"));
        wait(NULL);
    }
    else if(pid==0)
    {
        sleep(1);
        char *p = (char *)addr;
        printf("[%s]", p);
    }
    return 0;
}
