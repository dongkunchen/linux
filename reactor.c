#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include "wrap.h"

#define _BUF_LEN_ 1024
#define _EVENT_SIZE_ 1024

int gepfd = 0;

typedef struct xx_event
{
    int fd;
    int events;
    void (*call_back)(int fd, int events, void *arg);
    void *arg;
    char buf[1024];
    int buflen;
    int epfd;
} xevent;

xevent myevents[_EVENT_SIZE_ + 1];

void readData(int fd, int events, void *arg);

void eventadd(int fd, int events, void (*call_back)(int, int, void *), void *arg, xevent *ev)
{
    ev->fd = fd;
    ev->events = events;
    ev->call_back = call_back;

    struct epoll_event epv;
    epv.events = events;
    epv.data.ptr = ev;
    epoll_ctl(gepfd, EPOLL_CTL_ADD, fd, &epv);
}

void eventset(int fd, int events, void (*call_back)(int, int, void *), void *arg, xevent *ev)
{
    ev->fd = fd;
    ev->events = events;
    ev->call_back = call_back;

    struct epoll_event epv;
    epv.events = events;
    epv.data.ptr = ev;
    epoll_ctl(gepfd, EPOLL_CTL_MOD, fd, &epv);
}

void eventdel(xevent *ev, int fd, int events)
{
    printf("begin call %s\n", __FUNCTION__);

    ev->fd = 0;
    ev->events = 0;
    ev->call_back = NULL;
    memset(ev->buf, 0x00, sizeof(ev->buf));
    ev->buflen = 0;

    struct epoll_event epv;
    epv.data.ptr = NULL;
    epv.events = events;
    epoll_ctl(gepfd, EPOLL_CTL_DEL, fd, &epv);
}

void senddata(int fd, int events, void *arg)
{
    printf("begin call %s\n", __FUNCTION__);

    xevent *ev = arg;
    Write(fd, ev->buf, ev->buflen);
    eventset(fd, EPOLLIN, readData, arg, ev);
}

void readData(int fd, int events, void *arg)
{
    printf("begin call %s\n", __FUNCTION__);
    xevent *ev = arg;

    ev->buflen = Read(fd, ev->buf, sizeof(ev->buf));
    if (ev->buflen > 0) 
    {
        eventset(fd, EPOLLOUT, senddata, arg, ev);
    }
    else if (ev->buflen == 0) 
    {
        Close(fd);
        eventdel(ev, fd, EPOLLIN);
    }
}

void initAccept(int fd, int events, void *arg)
{
    printf("begin call %s.getfd =%d\n", __FUNCTION__, gepfd);

    int i;
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    int cfd = Accept(fd, (struct sockaddr *)&addr, &len);

    for (i = 0; i < _EVENT_SIZE_; i++)
    {
        if (myevents[i].fd == 0)
        {
            break;
        }
    }
    eventadd(cfd, EPOLLIN, readData, &myevents[i], &myevents[i]);
}

int main(int argc, char *argv[])
{
    int lfd = Socket(AF_INET, SOCK_STREAM, 0);

    int opt = 1;
    setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));

    struct sockaddr_in serv;
    bzero(&serv, sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_port = htons(8888);
    serv.sin_addr.s_addr = htonl(INADDR_ANY);
    Bind(lfd, (struct sockaddr *)&serv, sizeof(serv));

    Listen(lfd, 128);

    gepfd = epoll_create(1024);
    printf("gepfd === %d\n", gepfd);

    struct epoll_event events[1024];

    eventadd(lfd, EPOLLIN, initAccept, &myevents[_EVENT_SIZE_], &myevents[_EVENT_SIZE_]);

    while (1)
    {
        int nready = epoll_wait(gepfd, events, 1024, -1);
        if (nready < 0)
        {
            perror("epoll_wait error");
            if (errno == EINTR)
            {
                continue;
            }
            break;
        }
        else if (nready > 0)
        {
            int i = 0;
            for (i = 0; i < nready; i++)
            {
                xevent *xe = events[i].data.ptr;
                printf("fd=%d\n", xe->fd);

                if (xe->events & events[i].events)
                {
                    xe->call_back(xe->fd, xe->events, xe);
                }
            }
        }
    }

    Close(lfd);

    return 0;
}
