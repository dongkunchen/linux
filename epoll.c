
#include "wrap.h"
#include <sys/epoll.h>
#include <ctype.h>

int main()
{
    int ret;
    int n;
    int i;
    int k;
    int nready;
    int lfd;
    int cfd;
    int epfd;
    int sockfd;
    char buf[1024];
    socklen_t socklen;
    struct sockaddr_in serv;
    struct epoll_event ev;
    struct epoll_event events[1024];

    
    lfd = Socket(AF_INET, SOCK_STREAM, 0);
    
    int opt = 1;
    setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));

    
    bzero(&serv, sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_port = htons(8888);
    serv.sin_addr.s_addr = htonl(INADDR_ANY);
    Bind(lfd, (struct sockaddr *)&serv, sizeof(serv));

    Listen(lfd, 128);

    
    epfd = epoll_create(1024);
    if (epfd < 0)
    {
        perror("create epoll error");
        return -1;
    }

    
    ev.data.fd = lfd;
    ev.events = EPOLLIN;
    epoll_ctl(epfd, EPOLL_CTL_ADD, lfd, &ev);

    while (1)
    {
        nready = epoll_wait(epfd, events, 1024, -1);
        if (nready < 0)
        {
            perror("epoll_wait error");
            if (errno == EINTR)
            {
                continue;
            }
            break;
        }

        for (i = 0; i < nready; i++)
        {
            
            sockfd = events[i].data.fd;
            if (sockfd == lfd)
            {
                cfd = Accept(lfd, NULL, NULL);
                
                ev.data.fd = cfd;
                ev.events = EPOLLIN;
                epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev);
                continue;
            }

            
            memset(buf, 0x00, sizeof(buf));
          
            
            n = recv(sockfd, buf, sizeof(buf),0);
            if (n <= 0)
            {
                
                printf("n==[%d], buf==[%s]\n", n, buf);
                close(sockfd);
                
                epoll_ctl(epfd, EPOLL_CTL_DEL, sockfd, NULL);
            }
            else
            {
                printf("n==[%d], buf==[%s]\n", n, buf);
                for (k = 0; k < n; k++)
                {
                    buf[k] = toupper(buf[k]);
                }
              
                send(sockfd, buf, n, 0);
            }
        }
    }

    close(epfd);
    close(lfd);
    return 0;
}
