//event建立簡單服務端
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <netinet/in.h>
#include <event2/event.h>
#include <ctype.h>

#define MAX 1000

struct event_get_fd
{
    evutil_socket_t fd;
    struct event *ev;
} event[MAX];

void init_ev_fd()
{
    int i = 0;
    for (i = 0; i < MAX; i++)
    {
        event[i].fd = -1;
        event[i].ev = NULL;
    }
}

void setEventFd(evutil_socket_t fd, struct event *ev)
{
    int i = 0;
    //查找存放的位置
    for (i = 0; i < MAX; i++)
    {
        if (event[i].fd == -1)
        {
            break;
        }
    }
    //若找不到合適的位置,直接退出進程
    if (i == MAX)
    {
        exit(1);
    }
    event[i].fd = fd;
    event[i].ev = ev;
}

int findEv(int fd)
{
    int i = 0;
    for (i = 0; i < MAX; i++)
    {
        if (event[i].fd == fd)
        {
            break;
        }
    }
    if (i == MAX)
    {
        printf("not find fd\n");
        exit(1);
    }
    return i;
}

void readcb(evutil_socket_t connfd, short events, void *arg)
{
    char buf[1024];
    memset(buf, 0x00, sizeof(buf));

    int num = findEv(connfd);
    int n = read(connfd, buf, sizeof(buf));
    if (n <= 0)
    {
        perror("read error");
        close(connfd);
        event_del(event[num].ev);
        event[num].fd = -1;
        event[num].ev = NULL;
    }
    else if (n == 0)
    {
        perror("client clode\n");
        close(connfd);
        event_del(event[num].ev);
        printf("[%p], [%d]\n", event[num].ev, num);
        event[num].fd = -1;
        event[num].ev = NULL;
    }
    else
    {
        int i = 0;
        for (i = 0; i < n; i++)
        {
            buf[i] = toupper(buf[i]);
        }

        write(connfd, buf, strlen(buf));
    }
}

void conncb(evutil_socket_t fd, short events, void *arg)
{
    struct event_base *base = (struct event_base *)arg;

    //接受新的客戶端連接
    struct sockaddr_in cliaddr;
    socklen_t addrlen = sizeof(struct sockaddr_in);

    int connfd = accept(fd, (struct sockaddr *)&cliaddr, &addrlen);
    if (connfd < 0)
    {
        perror("accept error");
        exit(1);
    }

    char sIP[128];
    memset(sIP, 0x00, sizeof(sIP));
    printf("received from %s at port %d\n", inet_ntop(AF_INET, &cliaddr.sin_addr, sIP, sizeof(sIP)), ntohs(cliaddr.sin_port));

    struct event *readev = event_new(base, connfd, EV_READ | EV_PERSIST, readcb, base);
    event_add(readev, NULL);
    setEventFd(connfd, readev);
}

int main()
{
    init_ev_fd();

    int lfd = socket(AF_INET, SOCK_STREAM, 0);

    int opt = 1;
    setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in serv;
    bzero(&serv, sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_port = htons(8888);
    serv.sin_addr.s_addr = htonl(INADDR_ANY);
    bind(lfd, (struct sockaddr *)&serv, sizeof(serv));

    listen(lfd, 120);

    //創建地基
    struct event_base *base = event_base_new();
    if (base == NULL)
    {
        printf("event_base_new error\n");
        return -1;
    }

    //創建監聽文件描述符對應事件
    struct event *ev = event_new(base, lfd, EV_READ | EV_PERSIST, conncb, base);
    if (ev == NULL)
    {
        printf("event_new error\n");
    }

    //將新的事件節點上base地基
    event_add(ev, NULL);

    //進入事件循環等待
    event_base_dispatch(base);

    //釋放資源
    event_base_free(base);
    event_free(ev);

    close(lfd);
    return 0;
}