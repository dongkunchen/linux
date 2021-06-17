
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>


int main()
{
    
    int cfd = socket(AF_INET, SOCK_STREAM, 0);
    if(cfd<0)
    {
        perror("socket error");
        return -1;
    }
    
    
    struct sockaddr_in serv;
    serv.sin_family = AF_INET;
    serv.sin_port = htons(8888);
    inet_pton(AF_INET, "127.0.0.1", &serv.sin_addr.s_addr);
    printf("[%x]\n", serv.sin_addr.s_addr);
    int ret = connect(cfd, (struct sockaddr *)&serv, sizeof(serv));
    if(ret<0)
    {
        perror("connect error");
        return -1;
    }

    int n = 0;
    char buf[256];
    while(1)
    {
       
        memset(buf, 0x00, sizeof(buf));
        n = read(STDIN_FILENO, buf, sizeof(buf));
        
        write(cfd, buf, n);
 
        memset(buf, 0x00, sizeof(buf));
        n = read(cfd, buf, sizeof(buf));
        if(n<=0)
        {
            printf("read error or server closed, n==[%d]\n", n);
            break;
        }
        printf("n==[%d], buf==[%s]\n", n, buf);
    }

    
    close(cfd);

    return 0;
}
