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
	decode_str(pname, pname);
	printf("[%s]\n", pname);
	while ((n1=Readline(connfd,buf,sizeof(buf)))>0);

	struct stat st;
	if (stat(pname,&st)<0)
	{
		printf ("file not exist\n");
		send_head(connfd, "404", "NOT FOUND", get_mime_type(".html"), 0);
		
		send_file(connfd, "error.html");	

	}
	else
	{
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
			send_head (connfd,"200","OK",get_mime_type(".html"),0);
			send_file (connfd,"html/dir_header.html");

			struct dirent **namelist;
			int num,wn;
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
				while (num--)
				{
					memset (buffer,0x00,sizeof(buffer));
					if (namelist[num]->d_type==DT_DIR)
					{
			       		sprintf(buffer, "<li><a href=%s/>%s</a></li>", namelist[num]->d_name, namelist[num]->d_name);
					}
					else
					{
			       		sprintf(buffer, "<li><a href=%s>%s</a></li>", namelist[num]->d_name, namelist[num]->d_name);
					}
					free(namelist[num]);
					Write(connfd,buffer,strlen(buffer)+1);
				}
				free(namelist);
			}
			send_file(connfd,"html/dir_tail.html");
		}
	}
}

int main(int argc, char *argv[])
{
    struct sigaction act;
    act.sa_handler = SIG_IGN;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGPIPE, &act, NULL);
    
	char path[100]={0};
	sprintf (path,"%s/%s",getenv("HOME"),"xixi/w_网络/http");
	chdir (path);
	
	int sfd=socket(AF_INET,SOCK_STREAM,0);
	if (sfd<0)
	{
		perror ("socker");
		exit(-1);
	}

	int opt=1;
	setsockopt(sfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));

	struct sockaddr_in ip,ipp;
	ip.sin_family =AF_INET;
	ip.sin_port =htons(10034);
	ip.sin_addr.s_addr =inet_addr("0.0.0.0");
	socklen_t len = sizeof(ip);

	if (bind(sfd,(struct sockaddr*)&ip,len))
	{
		perror ("bind");
		exit(-1);
	}

	if (listen(sfd,155))
	{
		perror ("listen");
		exit (-1);
	}

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

	for (int i=0;i<1000;i++)
	{
		client[i]=-1;
	}
	
	tep.events=EPOLLIN;
	tep.data.fd=sfd;
	int res=epoll_ctl(epfd,EPOLL_CTL_ADD,sfd,&tep);
	if (res==-1)
	{
		perror ("epoll_ctl");
		exit (-1);
	}
	
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
				len =sizeof(ipp);
				connfd=accept(sfd,(void*)&ipp,&len);
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
				http_reques(cfd,epfd);
			}
		}

	}
	close (sfd);
	close (epfd);
	return 0;
}
