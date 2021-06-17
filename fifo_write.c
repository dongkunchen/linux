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
    // int ret = mkfifo("./myfifo", 0777);
    int ret = access("./myfifo", F_OK);
    //先利用access函數判斷是否存在0存在非0不存在創建
    if (ret != 0)
    {
        ret = mkfifo("./myfifo", 0777);
        if (ret < 0)
        {
            perror("mkfifo error");
            return -1;
        }
    }

    //打開文件
    int fd = open("./myfifo", O_RDWR);
    if (fd < 0)
    {
        perror("open error");
        return -1;
    }

    //寫fifo文件
    int i = 0;
    char buf[64];
    while (1)
    {
        memset(buf, 0x00, sizeof(buf));
        sprintf(buf, "%d:%s", i, "hello world");
        write(fd, buf , strlen(buf));
        sleep(1);
        i++;
    }
    //關閉文件
    close(fd);

    //等待輸入------暫停類似systme("pause"); linux無法用system
    //getchar();

    return 0;
}