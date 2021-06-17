#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
    int oldfd = open(argv[1], O_RDWR | O_CREAT, 0755);
    if(oldfd<0)
    {
        perror("open error");
        return -1;
    }
    
    int newfd = open(argv[2], O_RDWR | O_CREAT, 0755);
     if(newfd<0)
    {
        perror("open error");
        return -1;
    }
    
    dup2(oldfd, newfd);
    printf("newfd:[%d], oldfd:[%d]\n", newfd, oldfd);

    
    write(newfd, "hello world", strlen("hello world"));

    
    lseek(newfd, 0, SEEK_SET);

    
    char buf[64];
    memset(buf, 0x00, sizeof(buf));
    int n = read(oldfd, buf, sizeof(buf));
    printf("read over: n==[%d], buf==[%s]\n", n, buf);

    
    close(oldfd);
    close(newfd);

    return 0;
}
