#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <ctype.h>
#include <sys/socket.h>
#include <pthread.h>
#include "wrap.h"

typedef struct info
{
    int cfd; 
    int idx;
    pthread_t thread;
    struct sockaddr_in client;
}INFO;

INFO thInfo[1024];

void *thread_work(void *arg)
{
    INFO *p = (INFO *)arg;
    printf("idx==[%d]\n", p->idx);

    char sIP[16];
    memset(sIP, 0x00, sizeof(sIP));
    printf("new client:[%s][%d]\n", inet_ntop(AF_INET, &(p->client.sin_addr.s_addr), sIP, sizeof(sIP)), ntohs(p->client.sin_port));

    int n;
    int cfd = p->cfd;
    struct sockaddr_in client;
    memcpy(&client, &(p->client), sizeof(client));

    char buf[1024];

    while (1)
    {
        memset(buf, 0x00, sizeof(buf));
        n = Read(cfd, buf, sizeof(buf));
        if (n <= 0)
        {
            printf("read error or client closed,n==[%d]\n", n);
            Close(cfd);
            p->cfd = -1;
            pthread_exit(NULL);
        }
        printf("n==[%d], buf==[%s]\n", n, buf);

        for (int i = 0; i < n; i++)
        {
            buf[i] = toupper(buf[i]);
        }
        Write(cfd, buf, n);
    }

    close(cfd);

    pthread_exit(NULL);
}

void init_thInfo()
{
    int i = 0;
    for (i = 0; i < 1024; i++)
    {
        thInfo[i].cfd = -1;
    }
}
int findIndex()
{
    int i;
    for (i = 0; i < 1024; i++)
    {
        if (thInfo[i].cfd == -1)
        {
            break;
        }
    }
    if (i == 1024)
    {
        return -1;
    }
    return i;
}

int main()
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

    init_thInfo();

    int cfd;
    int ret;
    int idx;
    socklen_t len;
    pthread_t thread;
    struct sockaddr_in client;    
    while (1)
    {
        len = sizeof(client);
        bzero(&client, sizeof(client));
        cfd = Accept(lfd, (struct sockaddr *)&client, &len);

        idx = findIndex();
        if(idx==-1)
        {
            Close(cfd);
            continue;
        }

        thInfo[idx].cfd = cfd;
        thInfo[idx].idx = idx;
        memcpy(&thInfo[idx].client, &client, sizeof(client));

        ret = pthread_create(&thInfo[idx].thread, NULL, thread_work, &thInfo[idx].cfd);
        if(ret!=0)
        {
            printf("create thread error:[%s]\n", strerror(ret));
            exit(-1);
        }

        pthread_detach(thInfo[idx].thread);
    }

    close(lfd);

    return 0;
}
