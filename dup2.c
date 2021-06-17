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
    //調用dup函數複製fd
    int newfd = open(argv[2], O_RDWR | O_CREAT, 0755);
     if(newfd<0)
    {
        perror("open error");
        return -1;
    }
    //調用dup2函數
    dup2(oldfd, newfd);
    printf("newfd:[%d], oldfd:[%d]\n", newfd, oldfd);

    //使用fd對文件進行寫的操作
    write(newfd, "hello world", strlen("hello world"));

    //調用lseek函數移動文件指針到開始處
    lseek(newfd, 0, SEEK_SET);

    //使用newfd讀文件
    char buf[64];
    memset(buf, 0x00, sizeof(buf));
    int n = read(oldfd, buf, sizeof(buf));
    printf("read over: n==[%d], buf==[%s]\n", n, buf);

    //關閉文件
    close(oldfd);
    close(newfd);

    return 0;
}