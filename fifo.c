//fifo完成兩個進程間通信測試
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

int main()
{
    //創建fifo文件
    int ret = mkfifo("./myfifo", 0777);
    if(ret<0)
    {
        perror("mkfifo error");
        return -1;
    }

    //打開文件
    int fd = open("./myfifo", O_RDWR);
    if(fd<0)
    {
        perror("open error");
        return -1;
    }

    //寫fifo文件
    write(fd, "hello world", strlen("hello world"));

    //關閉文件
    close(fd);

    //等待輸入
    getchar();//暫停類似systme("pause"); linux無法用system

    return 0;
}