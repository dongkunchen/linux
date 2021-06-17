#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>         
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <strings.h>
#include <dirent.h>
#include <signal.h>
#include "wrap.h"
#include "pub.h"

int send_head(int connfd,char *cord,char *msg,char *file,int len)
{
	char buf[1024]={0};
	
	sprintf(buf,"HTTP/1.1 %s %s\r\n",cord,msg);
	sprintf(buf+strlen(buf), "Content-Type:%s\r\n",file);
	if (len>0)
	{
		sprintf(buf+strlen(buf), "Content-Length:%d\r\n", len);
	}
	strcat(buf,"\r\n");
	printf ("%s",buf);
	Write(connfd,buf,strlen(buf));
	return 0;
}

int send_file (int connfd,char *file)
{
	//使用文件I/O读写文件
	int num=0;
	int fd=open(file,O_RDONLY);
	if (fd<0)
	{
		perror("open error");
		return -1;
	}
	int n=0;
	char buf[1024];
	while (1)
	{
		//bzero (buf,1024);
		memset(buf,0x00,sizeof(buf));
		n = read(fd,buf,sizeof(buf));
		//printf("---- %s----\n",buf);
		if (n<=0)
			break;
		else
			Write(connfd,buf,n);
	}
	return 0;
}


void http_reques(int connfd,int epfd)
{
	//获取头部信息
	int n=0;
	char buf[1000]={0};
	memset(buf, 0x00, sizeof(buf));
	n=Readline(connfd,buf,sizeof(buf));
	if (n<=0)
	{
		printf("readline error\n");
		close (connfd);
		epoll_ctl(epfd,EPOLL_CTL_DEL,connfd,NULL);
		return;
	}
	printf ("%s\n",buf);
	char type[16]={0};
	char name[255]={0};
	char port[16]={0};
	sscanf(buf,"%[^ ] %[^ ] %[^ \r\n]",type,name,port);
	
	//去除前面的“/”
	int n1=0;
	char *pname=name;
	if (strlen(pname)<=1)
	{
		strcpy(pname,"./");
	}
	else 
	{
		pname=name+1;
	}
	//通过对字符的转换来处理中文编码问题
	decode_str(pname, pname);
	printf("[%s]\n", pname);
	//将缓存区其他的请求行 空行 正文清空
	while ((n1=Readline(connfd,buf,sizeof(buf)))>0);

	//判断文件是否存在
	struct stat st;
	if (stat(pname,&st)<0)
	{
		printf ("file not exist\n");
		//响应头
		send_head(connfd, "404", "NOT FOUND", get_mime_type(".html"), 0);
		
		//响应文件内容
		send_file(connfd, "error.html");	

	}
	else
	{
		//若为目录文件
		if (S_ISREG(st.st_mode))
		{
			printf ("file exist\n");
			send_head (connfd,"200","OK",get_mime_type(pname),st.st_size);
			send_file (connfd,pname);
		}
		else if (S_ISDIR(st.st_mode))
		{
			printf ("目錄文件\n");
			char buffer[1024];
			//响应头
			send_head (connfd,"200","OK",get_mime_type(".html"),0);
			//响应前面的头
			send_file (connfd,"html/dir_header.html");

			struct dirent **namelist;
			int num,wn;
			//解析目录
			num = scandir(pname,&namelist,NULL,alphasort);
			if (num < 0)
			{
				perror ("scandir");
				close (connfd);
				epoll_ctl(epfd,EPOLL_CTL_DEL,connfd,NULL);
				return;
			}
			else 
			{
				//循环读
				while (num--)
				{
					//bzero (buffer,1024);
					memset (buffer,0x00,sizeof(buffer));
					if (namelist[num]->d_type==DT_DIR)
					{
						//拼接目录
			       		sprintf(buffer, "<li><a href=%s/>%s</a></li>", namelist[num]->d_name, namelist[num]->d_name);
					}
					else
					{
						//拼接普通文件
			       		sprintf(buffer, "<li><a href=%s>%s</a></li>", namelist[num]->d_name, namelist[num]->d_name);
					}
					free(namelist[num]);
					Write(connfd,buffer,strlen(buffer)+1);
				}
				free(namelist);
			}
			//发送尾
			send_file(connfd,"html/dir_tail.html");
		}
	}
}

int main(int argc, char *argv[])
{
	//解決客戶瀏覽器突然中斷 web server會中斷問題
    struct sigaction act;
    act.sa_handler = SIG_IGN;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGPIPE, &act, NULL);
    
    //切换目录到家目录
	char path[100]={0};
	sprintf (path,"%s/%s",getenv("HOME"),"xixi/w_网络/http");
	chdir (path);
	
	//创建网络描述符
	int sfd=socket(AF_INET,SOCK_STREAM,0);
	if (sfd<0)
	{
		perror ("socker");
		exit(-1);
	}

	//设置端口复用
	int opt=1;
	setsockopt(sfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));

	//初始化包含应用层和网络层的ip结构体
	struct sockaddr_in ip,ipp;
	ip.sin_family =AF_INET;
	ip.sin_port =htons(10034);
	ip.sin_addr.s_addr =inet_addr("0.0.0.0");
	//ip.sin_addr.s_addr =inet_addr("172.26.46.72");
	socklen_t len = sizeof(ip);

	//绑定
	if (bind(sfd,(struct sockaddr*)&ip,len))
	{
		perror ("bind");
		exit(-1);
	}

	//监听
	if (listen(sfd,155))
	{
		perror ("listen");
		exit (-1);
	}

	//epoll跟结点的创建
	int epfd=epoll_create (1000);
	if (epfd <0)
	{
		perror ("epoll create");
		exit (-1);
	}

	struct epoll_event tep,ep[1000];
	int client[1000],ret,n,socked;
	char buf[100];
	int max=-1,connfd,j,cfd;
	char str[100];

	//将接收到的客户端初始化为-1
	for (int i=0;i<1000;i++)
	{
		client[i]=-1;
	}
	
	//将监听的文件描述符上树
	tep.events=EPOLLIN;
	tep.data.fd=sfd;
	int res=epoll_ctl(epfd,EPOLL_CTL_ADD,sfd,&tep);
	if (res==-1)
	{
		perror ("epoll_ctl");
		exit (-1);
	}
	
	//循环监听
	while (1)
	{
		res=epoll_wait(epfd,ep,1000,-1);
		if (res==-1)
		{
			perror ("wait");
			exit(-1);
		}

		for (int i=0;i<res;i++)
		{
			cfd=ep[i].data.fd;
			if (!(ep[i].events & EPOLLIN))
				continue;
			if (ep[i].data.fd==sfd)
			{
				//接收文件描述符
				len =sizeof(ipp);
				connfd=accept(sfd,(void*)&ipp,&len);
				//设置非阻塞I/O，保证后面可以读完缓存区的信息流
				int flag=fcntl(connfd,F_GETFL);
				flag |=O_NONBLOCK;
				fcntl(connfd,F_SETFL,flag);
				printf("received from %s at PORT %d\n", 
						inet_ntop(AF_INET, &ipp.sin_addr, str, sizeof(str)), 
						ntohs(ipp.sin_port));
			
				for (j=0;j<1000;j++)
				{
					if (client[j]<0)
					{
						client[j]=connfd;
						break;
					}
				}
				if (connfd ==1000)
				{
					puts ("too many client");
					exit(-1);
				}
				if (j >max)
					max=j;
				tep.events=EPOLLIN;
				tep.data.fd= connfd;
				ret=epoll_ctl(epfd,EPOLL_CTL_ADD,connfd,&tep);
				if (ret==-1)
				{
					perror ("ctl add");
					exit(-1);
				}
			}
			else
			{
				//处理http信息
				http_reques(cfd,epfd);
			}
		}

	}
	close (sfd);
	close (epfd);
	return 0;
}
