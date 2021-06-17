//udp服務端
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <ctype.h>

int main()
{
    //創建socket
    int cfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(cfd<0)
    {
        perror("socket error");
        return -1;
    }

    //綁定
    struct sockaddr_in serv;
    struct sockaddr_in client;
    bzero(&serv, sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_port = htons(8888);
    serv.sin_addr.s_addr = htonl(INADDR_ANY);
    bind(cfd, (struct sockaddr *)&serv, sizeof(serv));

    int n;
    int i;
    socklen_t len;
    char buf[1024];
    while (1)
    {
        //讀取數據
        memset(buf, 0x00, sizeof(buf));
        len = sizeof(client);
        n = recvfrom(cfd, buf, sizeof(buf), 0, (struct sockaddr *)&client, &len);
        
        //將小寫轉成大寫
        for (i = 0; i < n; i++)
        {
            buf[i] = toupper(buf[i]);
        }
        printf("[%d]:n==[%d], buf==[%s]\n", ntohs(client.sin_port), n, buf);
        //發送數據
        sendto(cfd, buf, n, 0, (struct sockaddr *)&client, len);        
    }

    //關閉套接字
    close(cfd);

    return 0;

}