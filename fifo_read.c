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
    /*int ret = mkfifo("./myfifo", 0777);
    if(ret<0)
    {
        perror("mkfifo error");
        return -1;
    }*/

    //打開文件
    int fd = open("./myfifo", O_RDWR);
    if (fd < 0)
    {
        perror("open error");
        return -1;
    }

    //寫fifo文件
    int n;
    char buf[64];
    while (1)
    {
        memset(buf, 0x00, sizeof(buf));
        n = read(fd, buf, sizeof(buf));
        printf("n==[%d], buf==[%s]\n", n, buf);
    }

    //關閉文件
    close(fd);

    //等待輸入------暫停類似systme("pause"); linux無法用system
    //getchar();

    return 0;
}