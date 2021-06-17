#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
    int fd = open(argv[1], O_RDWR | O_CREAT, 0777);
    if(fd<0)
    {
        perror("open error");
        return -1;
    }
    
    dup2(fd, STDOUT_FILENO);

    printf("hello world");
    
    close(fd);

    return 0;
}