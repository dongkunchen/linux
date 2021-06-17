#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <ctype.h>
#include <sys/un.h>

int main()
{
    int lfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (lfd<0)
    {
        perror("socket error");
        return -1;
    }

    unlink("serv.sock");
    struct sockaddr_un serv;
    serv.sun_family = AF_UNIX;
    strcpy(serv.sun_path, "./serv.sock");
    int ret = bind(lfd, (struct sockaddr *)&serv, sizeof(serv));
    if (ret<0)
    {
        perror("bind error");
        return -1;
    }

    listen(lfd, 128);

    struct sockaddr_un client;
    socklen_t len = sizeof(client);
    int cfd = accept(lfd, (struct sockaddr *)&client, &len);
    if (cfd<0)
    {
        perror("accept error");
        return -1;
    }
    printf("client:[%s]\n", client.sun_path);
    
    int n;
    int i;
    char buf[1024];
    while (1)
    {
        memset(buf, 0x00, sizeof(buf));
        int n = read(cfd, buf, sizeof(buf));
        if (n<=0)
        {
            perror("read error or client closed");
            break;
        }
        printf("n==[%d], buf==[%s]\n", n, buf);
        
        for (i = 0; i < n; i++)
        {
            buf[i] = toupper(buf[i]);
        }
        
        write(cfd, buf, n);
    }
    
    close(cfd);
    close(lfd);

    return 0;
}
