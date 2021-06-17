//本地socket客戶端
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
    //創建socket
    int cfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (cfd<0)
    {
        perror("socket error");
        return -1;
    }

    //刪除client.sock文件不然有這文件bind會抱錯
    unlink("client.sock");

    //綁定
    struct sockaddr_un client;
    client.sun_family = AF_UNIX;
    strcpy(client.sun_path, "./client.sock");
    int ret = bind(cfd, (struct sockaddr *)&client, sizeof(client));
    if (ret<0)
    {
        perror("bind error");
        return -1;
    }

  
    //連接服務端
    struct sockaddr_un serv;
    serv.sun_family = AF_UNIX;
    strcpy(serv.sun_path, "./serv.sock");
    connect(cfd, (struct sockaddr *)&serv, sizeof(serv));
   
    
    int n;
    int i;
    char buf[1024];
    while (1)
    {
        memset(buf, 0x00, sizeof(buf));
        n = read(STDIN_FILENO, buf, sizeof(buf));
        //發送數據
        write(cfd, buf, n);
        //讀數據
        memset(buf, 0x00, sizeof(buf));
        n = read(cfd, buf, sizeof(buf));
        if (n<=0)
        {
            perror("read error or client closed");
            break;
        }
        printf("n==[%d], buf==[%s]\n", n, buf);
    }
    
    close(cfd);

    return 0;
}