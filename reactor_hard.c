//反應堆簡單版
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <ctype.h>
#include "wrap.h"

#define MAX_EVENTS 1024
#define BUFLEN 4096
#define SERV_PORT 8888

void recvdata(int fd, int events, void *arg);
void senddata(int fd, int events, void *arg);

//描述就緒文件描述符相關訊息
struct myevent_s
{
    int fd;
    int events;
    void *arg;
    void (*call_back)(int fd, int events, void *arg);
    int status;
    char buf[BUFLEN];
    int len;
    long last_active;
};

int g_efd;
int g_lfd;
struct myevent_s g_events[MAX_EVENTS + 1];

//將結構體myevent_s成員變量初始化
void eventset(struct myevent_s *ev, int fd, void (*call_back)(int, int, void *), void *arg)
{
    ev->fd = fd;
    ev->call_back = call_back;
    ev->events = 0;               //對應監聽事件
    ev->arg = arg;                //自己指向自己
    ev->status = 0;               //是否監聽:1->在紅黑樹上(監聽), 0->不在(不監聽)
                                  //memset(ev->buf, 0, sizeof(ev->buf));
                                  //ev->len = 0;
    ev->last_active = time(NULL); //調用eventset函數的時間

    return;
}

void eventadd(int efd, int events, struct myevent_s *ev)
{
    struct epoll_event epv = {0, {0}}; //需要監聽事件的結構體
    int op;                            //EPOLL_CTL_MOD（修改樹中節點屬性），EPOLL_CTL_ADD（向樹中插入節點）
    epv.data.ptr = ev;                 //指向一個 myevent_s 結構體變量
    epv.events = ev->events = events;  //監聽事件賦值 EPOLLIN 或 EPOLLOUT

    if (ev->status == 1)
    {                       //已經在紅黑樹 g_efd 裡
        op = EPOLL_CTL_MOD; //修改其屬性
    }
    else
    {                       //不在紅黑樹禮
        op = EPOLL_CTL_ADD; //將其加入紅黑樹 g_efd, 並將status置1
        ev->status = 1;
    }

    if (epoll_ctl(efd, op, ev->fd, &epv) < 0)
    {
        printf("event add failed [fd=%d], events[%d]\n", ev->fd, events);
    }
    else
    {
        printf("event add OK [fd=%d], op=%d, events[%0X]\n", ev->fd, op, events);
    }

    return;
}

void eventdel(int efd, struct myevent_s *ev)
{
    struct epoll_event epv = {0, {0}};

    if (ev->status != 1) //不在紅黑樹上
        return;

    epv.data.ptr = ev;
    ev->status = 0;                              //修改狀態
    epoll_ctl(efd, EPOLL_CTL_DEL, ev->fd, &epv); //從紅黑樹 efd 上將 ev->fd 摘除

    return;
}

//當文件描述符就緒,epoll返回,調用該函樹與客戶端建立鏈接
//回調函數 - 監聽文件描述符發送讀事件時被調用
void acceptconn(int lfd, int events, void *arg)
{
    struct sockaddr_in cin;
    socklen_t len = sizeof(cin);
    int cfd, i;
    //獲取客戶端套接字
    if ((cfd = Accept(lfd, (struct sockaddr *)&cin, &len)) == -1)
    {
        if (errno != EAGAIN && errno != EINTR)
        {
            /* 暫時不做出錯處理 */
        }
        printf("%s: accept, %s\n", __func__, strerror(errno));
        return;
    }

    do
    {
        //查找自定義數組空閒位置
        for (i = 0; i < MAX_EVENTS; i++) //從全局數組g_events中找一個空閒元素
            if (g_events[i].status == 0) //類似於select中找值為-1的元素
                break;                   //跳出 for循環
        //超出數組的最大限制
        if (i == MAX_EVENTS)
        {
            printf("%s: max connect limit[%d]\n", __func__, MAX_EVENTS);
            break; //跳出do while(0) 不執行後續代碼
        }

        int flag = 0;
        flag = fcntl(cfd, F_GETFL, 0);
        flag != O_NONBLOCK;
        if ((flag = fcntl(cfd, F_SETFL, flag)) < 0)
        // if ((flag = fcntl(cfd, F_SETFL, O_NONBLOCK)) < 0)
        { //將cfd也設置為非阻塞
            printf("%s: fcntl nonblocking failed, %s\n", __func__, strerror(errno));
            break;
        }

        //將客戶端套接字放入全局數組
        eventset(&g_events[i], cfd, recvdata, &g_events[i]);
        //將cfd添加到紅黑樹g_efd中,監聽讀事件
        eventadd(g_efd, EPOLLIN, &g_events[i]);

    } while (0);

    printf("new connect [%s:%d][time:%ld], pos[%d]\n", inet_ntoa(cin.sin_addr), ntohs(cin.sin_port), g_events[i].last_active, i);
    return;
}

void recvdata(int fd, int events, void *arg)
{
    struct myevent_s *ev = (struct myevent_s *)arg;
    int len;

    //讀文件描述符 數據存入myevent_s成員buf中
    len = recv(fd, ev->buf, sizeof(ev->buf), 0);

    eventdel(g_efd, ev); //將該節點從紅黑樹上摘除

    if (len > 0)
    {

        ev->len = len;
        ev->buf[len] = '\0'; //手動添加字符串標記
        printf("C[%d]:%s\n", fd, ev->buf);

        //設置該 fd 對應的回調函數 senddata
        eventset(ev, fd, senddata, ev);

        //將fd加入紅黑樹g_efd中,監聽其寫事件,當套接字可以寫得時候,觸發epoll_wait返回
        eventadd(g_efd, EPOLLOUT, ev);
    }
    else if (len == 0)
    {
        close(ev->fd);
        /* ev-g_events 地址相減得到偏移元素位置 */
        printf("[fd=%d] pos[%ld], closed\n", fd, ev - g_events);
    }
    else
    {
        close(ev->fd);
        printf("recv[fd=%d] error[%d]:%s\n", fd, errno, strerror(errno));
    }

    return;
}

void senddata(int fd, int events, void *arg)
{
    struct myevent_s *ev = (struct myevent_s *)arg;
    int len;

    len = send(fd, ev->buf, ev->len, 0);
    //直接将数据 回写给客户端。未作处理

    if (len > 0)
    {

        printf("send[fd=%d], [%d]%s\n", fd, len, ev->buf);
        eventdel(g_efd, ev);
        //从红黑树g_efd中移除
        eventset(ev, fd, recvdata, ev);
        //将该fd的 回调函数改为 recvdata
        eventadd(g_efd, EPOLLIN, ev);
        //从新添加到红黑树上， 设为监听读事件
    }
    else
    {
        close(ev->fd);
        //关闭链接
        eventdel(g_efd, ev);
        //从红黑树g_efd中移除
        printf("send[fd=%d] error %s\n", fd, strerror(errno));
    }

    return;
}

void initlistensocket(int efd, short port)
{
    //创建监听套接字
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    //设置监听套接字属性为套接字属性为非阻塞
    fcntl(lfd, F_SETFL, O_NONBLOCK);

    //结构体最后一个元素初始化,将listenfd信息存放
    eventset(&g_events[MAX_EVENTS], lfd, acceptconn, &g_events[MAX_EVENTS]);

    //将listenfd加入epoll数中
    eventadd(efd, EPOLLIN, &g_events[MAX_EVENTS]);

    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));            //bzero(&sin, sizeof(sin))
    sin.sin_family = AF_INET;                // ipv4 地址
    sin.sin_addr.s_addr = htonl(INADDR_ANY); //任意ip
    sin.sin_port = htons(port);
    //绑定ip和端口,将主动套接字变为被动连接套接字
    bind(lfd, (struct sockaddr *)&sin, sizeof(sin));
    //监听,最大等待队列20,创建2个队列,一个是已经完成3次握手的队列,一个是正在等待3次握手完成的队列
    listen(lfd, 20);

    return;
}

int main(int argc, char *argv[])
{
    //使用用户指定端口.如未指定,用默认端口
    unsigned short port = SERV_PORT;
    if (argc == 2)
        port = atoi(argv[1]);

    //创建epoll句柄
    g_efd = epoll_create(MAX_EVENTS + 1);
    if (g_efd <= 0)
        printf("create efd in %s err %s\n", __func__, strerror(errno));

    //服务端套接字初始化,将套接字加入epoll树中
    initlistensocket(g_efd, port);

    //创建已经满足就绪事件的文件描述符数组 
    struct epoll_event events[MAX_EVENTS + 1];
    printf("server running:port[%d]\n", port);

    int checkpos = 0, i; //一次循环检测100个。 使用checkpos控制检测对象
    while (1)
    {

        /* 超时验证，每次测试100个链接，不测试listenfd 当客户端60秒内没有和服务器通信，则关闭此客户端链接 */
        long now = time(NULL); //当前时间
        for (i = 0; i < 100; i++, checkpos++)
        {                               //一次循环检测100个。 使用checkpos控制检测对象
            if (checkpos == MAX_EVENTS) //过滤listenfd
                checkpos = 0;
            if (g_events[checkpos].status != 1) //不在红黑树 g_efd 上
                continue;
            long duration = now - g_events[checkpos].last_active; //客户端不活跃的世间

            if (duration >= 60)
            {
                close(g_events[checkpos].fd); //关闭与该客户端链接
                printf("[fd=%d] timeout\n", g_events[checkpos].fd);
                //g_efd树根  ,结构体地址
                eventdel(g_efd, &g_events[checkpos]); //将该客户端 从红黑树 g_efd移除,并将自定义数组中的status置为0
            }
        }
        //关闭连接结束

        int nfd = epoll_wait(g_efd, events, MAX_EVENTS + 1, 1000);
        if (nfd < 0)
        {
            printf("epoll_wait error, exit\n");
            break;
        }

        for (i = 0; i < nfd; i++)
        {
            /*使用自定义结构体myevent_s类型指针, 接收 联合体data的void *ptr成员*/
            struct myevent_s *ev = (struct myevent_s *)events[i].data.ptr;

            if ((events[i].events & EPOLLIN) && (ev->events & EPOLLIN))
            { //读就绪事件
                //调用回调函数
                ev->call_back(ev->fd, events[i].events, ev->arg);
            }
            if ((events[i].events & EPOLLOUT) && (ev->events & EPOLLOUT))
            { //写就绪事件
                ev->call_back(ev->fd, events[i].events, ev->arg);
            }
        } //end for
    }
    /* 退出前释放所有资源 */
    return 0;
}