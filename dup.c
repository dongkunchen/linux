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
    
    int newfd = dup(fd);
    printf("newfd:[%d], fd:[%d]\n", newfd, fd);

    
    write(fd, "hello world", strlen("hello world"));

    
    lseek(fd, 0, SEEK_SET);

    
    char buf[64];
    memset(buf, 0x00, sizeof(buf));
    int n = read(newfd, buf, sizeof(buf));
    printf("read over: n==[%d], buf==[%s]\n", n, buf);

   
    close(fd);
    close(newfd);

    return 0;
}
