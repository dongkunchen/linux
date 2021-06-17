#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <ctype.h>

int main()
{
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    if(lfd<0)
    {
        perror("socket error");
        return -1;
    }

    struct sockaddr_in serv;
    bzero(&serv, sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_port = htons(8888);
    serv.sin_addr.s_addr = htonl(INADDR_ANY);
    int ret = bind(lfd, (struct sockaddr *)&serv, sizeof(serv));
    if(ret<0)
    {
        perror("bind error");
        return -1;
    }

    listen(lfd, 128);

    int cfd = accept(lfd, NULL, NULL);
    printf("lfd==[%d], cfd==[%d]\n", lfd, cfd);

    int i = 0;
    int n = 0;
    char buf[1024];

    while(1)
    {
        memset(buf , 0x00, sizeof(buf));
        n = read(cfd, buf, sizeof(buf));
        if(n<=0)
        {
            printf("read error or client close, n==[%d]\n", n);
            break;
        }
        printf("n==[%d], buf==[%s]\n", n, buf);
        for ( i = 0; i < n; i++)
        {
            buf[i] = toupper(buf[i]);
        }

        write(cfd, buf, n);        
    }

    close(lfd);
    close(cfd);

}
