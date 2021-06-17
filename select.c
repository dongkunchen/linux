//多路IO復用-select
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <ctype.h>
#include "wrap.h"

int main()
{

    int maxfd;
    int nready;
    int cfd;
    int i;
    int ret;
    int n;
    char buf[FD_SETSIZE];
    socklen_t len;
    int maxi;
    int connfd[FD_SETSIZE];
    fd_set tmpfds, readfds;
    struct sockaddr_in cliaddr;
    //創建socket
    int lfd = Socket(AF_INET, SOCK_STREAM, 0);

    //設置端口複用 不用等待一分鐘就能在連
    int opt = 1;
    setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));

    //綁定
    struct sockaddr_in serv;
    bzero(&serv, sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_port = htons(8888);
    serv.sin_addr.s_addr = htonl(INADDR_ANY);
    Bind(lfd, (struct sockaddr *)&serv, sizeof(serv));

    //設置監聽
    Listen(lfd, 128);

    //清空readfds和tempfds集合
    FD_ZERO(&readfds);
    FD_ZERO(&tmpfds);

    //將lfd加入readfds中,委託內和監控
    FD_SET(lfd, &readfds);

    for (i = 0; i < FD_SETSIZE; i++)
    {
        connfd[i] = -1;
    }

    maxfd = lfd;
    len = sizeof(struct sockaddr_in);

    while (1)
    {
        tmpfds = readfds;
        nready = select(maxfd + 1, &tmpfds, NULL, NULL, NULL);
        if (nready > 0)
        {
            if (FD_ISSET(lfd, &tmpfds))
            {
                cfd = Accept(lfd, (struct sockaddr *)&cliaddr, &len);
                if (cfd < 0)
                {
                    if (errno == ECONNABORTED || errno == EINTR) //被信號中斷
                    {
                        continue;
                    }
                    break;
                }
                for (i = 0; i < FD_SETSIZE; i++)
                {
                    if (connfd[i] == -1)
                    {
                        connfd[i] = cfd;
                        break;
                    }
                }
                if (i == FD_SETSIZE)
                {
                    close(cfd);
                    printf("too many clients, i==[%d]\n", i);
                    //exit(1);
                    continue;
                }
                if (i > maxi)
                {
                    maxi = i;
                }
                char sIP[16];
                memset(sIP, 0x00, sizeof(sIP));
                printf("receive from client--->IP[%s],PORT:[%d]\n", inet_ntop(AF_INET, &cliaddr.sin_addr.s_addr, sIP, sizeof(sIP)), ntohs(cliaddr.sin_port));
                FD_SET(cfd, &readfds);
                if (maxfd < cfd)
                {
                    maxfd = cfd;
                }
                if (--nready == 0)
                {
                    continue;
                }
            }

            //有數據發來的情況
            for (i = 0; i <= maxi; i++)
            {
                int sockfd = connfd[i];
                //判斷sockfd文件描述符是否有變化
                if (sockfd == -1)
                {
                    continue;
                }

                if (FD_ISSET(sockfd, &tmpfds))
                {
                    //讀數據
                    memset(buf, 0x00, sizeof(buf));
                    n = Read(sockfd, buf, sizeof(buf));
                    if (n < 0)
                    {
                        perror("read over\n"); 
                        close(sockfd);
                        FD_CLR(sockfd, &readfds);
                        connfd[i] = -1;
                    }
                    else if (n == 0)
                    {
                        perror("client is closed\n");
                        //關閉連接
                        close(sockfd);
                        //將sockfd從readfds中刪除
                        FD_CLR(sockfd, &readfds);
                        connfd[i] = -1;
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
    }

    close(lfd);

    return 0;
}