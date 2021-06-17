//多進程版本高併發服務器
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

    //父進程對子進程回收代碼
    //1.先阻塞SIGCHLD信號防止直接回收掉
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
        //接手新的連接
        len = sizeof(client);
        memset(sIP, 0x00, sizeof(sIP));
        cfd = Accept(lfd, (struct sockaddr *)&client, &len);
        printf("client:[%s] [%d]\n", inet_ntop(AF_INET, &client.sin_addr.s_addr, sIP, sizeof(sIP)), ntohs(client.sin_port));

        //接收一個新的連接,創建一個子進程,讓子進程收發數據
        pid = fork();
        if (pid < 0)
        {
            perror("fork error");
            exit(-1);
        }
        else if (pid > 0)
        {
            //父進程關閉通信文件描述符
            close(cfd);

            //父進程對子進程回收代碼
            //2.註冊SIGCHLD信號處裡函數
            struct sigaction act;
            act.sa_handler = waitchild;
            act.sa_flags = 0;
            sigaction(SIGCHLD, &act, NULL);

            //解除對SIGCHLD信號的阻塞
            sigprocmask(SIG_UNBLOCK, &mask, NULL);
        }
        else if (pid == 0)
        {
            //子進程關閉監聽文件描述符
            close(lfd);

            int i = 0;
            int n;
            char buf[1024];
            while (1)
            {
                //讀數據
                n = Read(cfd, buf, sizeof(buf));
                if (n <= 0)
                {
                    printf("read error or client closed, n==[%d]\n", n);
                    break;
                }
                printf("[%d]---->n==[%d], buf==[%s]\n", ntohs(client.sin_port), n, buf);
                //將小寫轉換成為大寫
                for (i = 0; i < n; i++)
                {
                    buf[i] = toupper(buf[i]);
                }

                //發送數據
                Write(cfd, buf, n);
            }

            //關閉cfd
            close(cfd);

            exit(0);
        }
    }

    //關閉監聽文件描述符
    close(lfd);

    return 0;
}