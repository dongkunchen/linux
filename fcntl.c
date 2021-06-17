#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
    int fd = open(argv[1], O_RDWR);
    if(fd<0)
    {
        perror("open error");
        return -1;
    }
    int flags = fcntl(fd, F_GETFL, 0);
    flags = flags | O_APPEND;
    fcntl(fd, F_SETFL, flags);

    write(fd, "hello world", strlen("hello world"));
   
    close(fd);
  
    return 0;
}