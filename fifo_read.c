#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

int main()
{
    int fd = open("./myfifo", O_RDWR);
    if (fd < 0)
    {
        perror("open error");
        return -1;
    }

    int n;
    char buf[64];
    while (1)
    {
        memset(buf, 0x00, sizeof(buf));
        n = read(fd, buf, sizeof(buf));
        printf("n==[%d], buf==[%s]\n", n, buf);
    }

    close(fd);


    return 0;
}
