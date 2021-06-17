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
    //調用dup函數複製fd
    int newfd = dup(fd);
    printf("newfd:[%d], fd:[%d]\n", newfd, fd);

    //使用fd對文件進行寫的操作
    write(fd, "hello world", strlen("hello world"));

    //調用lseek函數移動文件指針到開始處
    lseek(fd, 0, SEEK_SET);

    //使用newfd讀文件
    char buf[64];
    memset(buf, 0x00, sizeof(buf));
    int n = read(newfd, buf, sizeof(buf));
    printf("read over: n==[%d], buf==[%s]\n", n, buf);

    //關閉文件
    close(fd);
    close(newfd);

    return 0;
}