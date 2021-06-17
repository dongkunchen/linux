#include <unistd.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <signal.h>
#include <dirent.h>

#include "pub.h"
#include "wrap.h"

int http_request(int cfd, int epfd);

int main()
{
	struct sigaction act;
	act.sa_handler = SIG_IGN;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	sigaction(SIGPIPE, &act, NULL);
	
	char path[255] = {0};
	sprintf(path, "%s/%s", getenv("HOME"), "webpath");
	chdir(path);
	
	int lfd = tcp4bind(9999, NULL);
	
	Listen(lfd, 128);

	int epfd = epoll_create(1024);
	if(epfd<0)
	{
		perror("epoll_create error");
		close(lfd);
		return -1;
	}
	
	struct epoll_event ev;
	ev.data.fd = lfd;
	ev.events = EPOLLIN;
	epoll_ctl(epfd, EPOLL_CTL_ADD, lfd, &ev);
	
	int i;
	int cfd;
	int nready;
	int sockfd;
	struct epoll_event events[1024];
	while(1)
	{
		nready = epoll_wait(epfd, events, 1024, -1);
		if(nready<0)
		{
			if(errno==EINTR)
			{
				continue;
			}
			break;
		}
		
		for(i=0; i<nready; i++)
		{
			sockfd = events[i].data.fd;
			if(sockfd==lfd)
			{
				cfd = Accept(lfd, NULL, NULL);
				
				int flag = fcntl(cfd, F_GETFL);
				flag |= O_NONBLOCK;
				fcntl(cfd, F_SETFL, flag);
				
				ev.data.fd = cfd;
				ev.events = EPOLLIN;
				epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev);
			}
			else 
			{
				http_request(sockfd, epfd);
			}			
		}		
	}
}

int send_header(int cfd, char *code, char *msg, char *fileType, int len)
{
	char buf[1024] = {0};
	sprintf(buf, "HTTP/1.1 %s %s\r\n", code, msg);
	sprintf(buf+strlen(buf), "Content-Type:%s\r\n", fileType);
	if(len>0)
	{
		sprintf(buf+strlen(buf), "Content-Length:%d\r\n", len);
	}
	strcat(buf, "\r\n");
	Write(cfd, buf, strlen(buf));
	return 0;
}

int send_file(int cfd, char *fileName)
{
	int fd = open(fileName, O_RDONLY);
	if(fd<0)
	{
		perror("open error");
		return -1;
	}
	
	int n;
	char buf[1024];
	while(1)
	{
		memset(buf, 0x00, sizeof(buf));
		n = read(fd, buf, sizeof(buf));
		if(n<=0)
		{
			break;
		}
		else 
		{
			Write(cfd, buf, n);
		}
	}
}

int http_request(int cfd, int epfd)
{
	int n;
	char buf[1024];
	memset(buf, 0x00, sizeof(buf));
	n = Readline(cfd, buf, sizeof(buf));
	if(n<=0)
	{
		close(cfd);
		
		epoll_ctl(epfd, EPOLL_CTL_DEL, cfd, NULL);
		return -1;	
	}
	printf("buf==[%s]\n", buf);
	//GET /hanzi.c HTTP/1.1
	char reqType[16] = {0};
	char fileName[255] = {0};
	char protocal[16] = {0};
	sscanf(buf, "%[^ ] %[^ ] %[^ \r\n]", reqType, fileName, protocal);
	//printf("[%s]\n", reqType);
	printf("--[%s]--\n", fileName);
	//printf("[%s]\n", protocal);
	
	char *pFile = fileName;
	if(strlen(fileName)<=1)
	{
		strcpy(pFile, "./");
	}
	else 
	{
		pFile = fileName+1;
	}
	
	strdecode(pFile, pFile);
	printf("[%s]\n", pFile);
	
	while((n=Readline(cfd, buf, sizeof(buf)))>0);
	
	struct stat st;
	if(stat(pFile, &st)<0)
	{
		printf("file not exist\n");
		
		send_header(cfd, "404", "NOT FOUND", get_mime_type(".html"), 0);
		
		send_file(cfd, "error.html");	
	}
	else 
	{
		if(S_ISREG(st.st_mode))
		{
			printf("file exist\n");
			send_header(cfd, "200", "OK", get_mime_type(pFile), st.st_size);
			
			send_file(cfd, pFile);
		}
		else if(S_ISDIR(st.st_mode))
		{
			printf("folder File\n");
			
			char buffer[1024];
			send_header(cfd, "200", "OK", get_mime_type(".html"), 0);	
			
			send_file(cfd, "html/dir_header.html");	
			
			struct dirent **namelist;
			int num;

			num = scandir(pFile, &namelist, NULL, alphasort);
			if (num < 0)
			{
			   perror("scandir");
			   close(cfd);
			   epoll_ctl(epfd, EPOLL_CTL_DEL, cfd, NULL);
			   return -1;
			   
			}
			else 
			{
			   while (num--) 
			   {
			       printf("%s\n", namelist[num]->d_name);
			       memset(buffer, 0x00, sizeof(buffer));
			       if(namelist[num]->d_type==DT_DIR)
			       {
			       		sprintf(buffer, "<li><a href=%s/>%s</a></li>", namelist[num]->d_name, namelist[num]->d_name);
			       }
			       else
			       {
			       		sprintf(buffer, "<li><a href=%s>%s</a></li>", namelist[num]->d_name, namelist[num]->d_name);
			       }
			       free(namelist[num]);
			       Write(cfd, buffer, strlen(buffer));
			   }
			   free(namelist);
			}
			sleep(10);
			send_file(cfd, "html/dir_tail.html");		
		}
	}
	
	return 0;
}
