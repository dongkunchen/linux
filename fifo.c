#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

int main()
{
    int ret = mkfifo("./myfifo", 0777);
    if(ret<0)
    {
        perror("mkfifo error");
        return -1;
    }

    int fd = open("./myfifo", O_RDWR);
    if(fd<0)
    {
        perror("open error");
        return -1;
    }

    write(fd, "hello world", strlen("hello world"));

    close(fd);

    getchar();

    return 0;
}
