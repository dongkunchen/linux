//多路IO復用-poll
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <poll.h>
#include <ctype.h>
#include "wrap.h"

int main()
{

    int i;
    int n;
    int cfd;
    int ret;
    int nready;
    int maxi;
    char buf[1024];
    socklen_t len;
    int sockfd;
    fd_set tmpfds, readfds;
    struct sockaddr_in serv, cliaddr;
    //創建socket
    int lfd = Socket(AF_INET, SOCK_STREAM, 0);

    //設置端口複用 不用等待一分鐘就能在連
    int opt = 1;
    setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));

    //綁定
    bzero(&serv, sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_port = htons(8888);
    serv.sin_addr.s_addr = htonl(INADDR_ANY);
    // ret = Bind(lfd, (struct sockaddr *)&serv, sizeof(serv));
    Bind(lfd, (struct sockaddr *)&serv, sizeof(serv));

    //設置監聽
    Listen(lfd, 128);

    struct pollfd client[1024];

    for (i = 0; i < 1024; i++)
    {
        client[i].fd = -1;
    }

    //將監聽文件描述符委託給內和
    client[0].fd = lfd;
    client[0].events = POLLIN;

    maxi = 0;

    while (1)
    {
        nready = poll(client, maxi + 1, -1);
        if (nready < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }
            perror("poll error");
            exit(1);
        }

        //有客戶端連接請求
        if (client[0].revents & POLLIN)
        {
            cfd = Accept(lfd, NULL, NULL);

            //尋找client數組中可用位置
            for (i = 1; i < 1024; i++)
            {
                if (client[i].fd == -1)
                {
                    client[i].fd = cfd;
                    client[i].events = POLLIN;
                    break;
                }
            }

            //若沒有位置,則關閉連接
            if (i == 1024)
            {
                Close(cfd);
                continue;
            }

            if (maxi < i)
            {
                maxi = i;
            }

            //nready發生變化文件描述符個數
            if (--nready == 0)
            {
                continue;
            }
        }

        //有數據到來的情況
        for (i = 0; i <= maxi; i++)
        {
            if (client[i].fd == -1)
            {
                continue;
            }
            if (client[i].revents == POLLIN)
            {
                sockfd = client[i].fd;
                memset(buf, 0x00, sizeof(buf));
                n = Read(sockfd, buf, sizeof(buf));
                if (n <= 0)
                {
                    printf("read error or client closed,n==[%d]\n", n);
                    Close(sockfd);
                    client[i].fd = -1;
                }
                else
                {
                    printf("n==[%d], buf==[%s]\n", n, buf);

                    int k = 0;
                    for (k = 0; k < n; k++)
                    {
                        buf[k] = toupper(buf[k]);
                    }
                    Write(sockfd, buf, n);
                }

                if (--nready == 0)
                {
                    break;
                }
            }
        }
    }

    close(lfd);

    return 0;
}