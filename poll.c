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
    int lfd = Socket(AF_INET, SOCK_STREAM, 0);

    int opt = 1;
    setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));

    bzero(&serv, sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_port = htons(8888);
    serv.sin_addr.s_addr = htonl(INADDR_ANY);
    // ret = Bind(lfd, (struct sockaddr *)&serv, sizeof(serv));
    Bind(lfd, (struct sockaddr *)&serv, sizeof(serv));

    Listen(lfd, 128);

    struct pollfd client[1024];

    for (i = 0; i < 1024; i++)
    {
        client[i].fd = -1;
    }

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

        if (client[0].revents & POLLIN)
        {
            cfd = Accept(lfd, NULL, NULL);

            for (i = 1; i < 1024; i++)
            {
                if (client[i].fd == -1)
                {
                    client[i].fd = cfd;
                    client[i].events = POLLIN;
                    break;
                }
            }

            if (i == 1024)
            {
                Close(cfd);
                continue;
            }

            if (maxi < i)
            {
                maxi = i;
            }

            if (--nready == 0)
            {
                continue;
            }
        }

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
