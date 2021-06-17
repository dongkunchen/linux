#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <ctype.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include "wrap.h"

void waitchild(int signo)
{
    pid_t wpid;
    while (1)
    {
        wpid = waitpid(-1, NULL, WNOHANG);
        if (wpid > 0)
        {
            printf("child exit, wpid==[%d]\n", wpid);
        }
        else if (wpid == 0 || wpid == -1)
        {
            break;
        }
    }
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

    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGCHLD);
    sigprocmask(SIG_BLOCK, &mask, NULL);

    pid_t pid;
    int cfd;
    char sIP[16];
    socklen_t len;
    struct sockaddr_in client;
    while (1)
    {
        len = sizeof(client);
        memset(sIP, 0x00, sizeof(sIP));
        cfd = Accept(lfd, (struct sockaddr *)&client, &len);
        printf("client:[%s] [%d]\n", inet_ntop(AF_INET, &client.sin_addr.s_addr, sIP, sizeof(sIP)), ntohs(client.sin_port));

        pid = fork();
        if (pid < 0)
        {
            perror("fork error");
            exit(-1);
        }
        else if (pid > 0)
        {
            close(cfd);

            struct sigaction act;
            act.sa_handler = waitchild;
            act.sa_flags = 0;
            sigaction(SIGCHLD, &act, NULL);

            sigprocmask(SIG_UNBLOCK, &mask, NULL);
        }
        else if (pid == 0)
        {
            close(lfd);

            int i = 0;
            int n;
            char buf[1024];
            while (1)
            {
                n = Read(cfd, buf, sizeof(buf));
                if (n <= 0)
                {
                    printf("read error or client closed, n==[%d]\n", n);
                    break;
                }
                printf("[%d]---->n==[%d], buf==[%s]\n", ntohs(client.sin_port), n, buf);
                for (i = 0; i < n; i++)
                {
                    buf[i] = toupper(buf[i]);
                }

                Write(cfd, buf, n);
            }

            close(cfd);

            exit(0);
        }
    }

    close(lfd);

    return 0;
}
