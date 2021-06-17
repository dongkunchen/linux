#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <signal.h>
#ifndef _WIN32
#include <netinet/in.h>
# ifdef _XOPEN_SOURCE_EXTENDED
#  include <arpa/inet.h>
# endif
#include <sys/socket.h>
#endif
 
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/listener.h>
#include <event2/util.h>
#include <event2/event.h>
#include <ctype.h>
 
static const char MESSAGE[] = "Hello, World!\n";
 
static const int PORT = 9995;
 
static void listener_cb(struct evconnlistener *, evutil_socket_t,
    struct sockaddr *, int socklen, void *);
static void conn_writecb(struct bufferevent *, void *);
static void conn_readcb(struct bufferevent *, void *);
static void conn_eventcb(struct bufferevent *, short, void *);
static void signal_cb(evutil_socket_t, short, void *);
 
int
main(int argc, char **argv)
{
	struct event_base *base; //地基
	struct evconnlistener *listener; //鏈接監聽器
	struct event *signal_event;//信號事件
 
	struct sockaddr_in sin;
#ifdef _WIN32
	WSADATA wsa_data;
	WSAStartup(0x0201, &wsa_data);
#endif
 
	//創建地基--相當於epoll樹根
	base = event_base_new();
	if (!base) {
		fprintf(stderr, "Could not initialize libevent!\n");
		return 1;
	}
 
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(PORT);
	
	//創建鏈接監聽器--socket bind listen accept
	//listener_cb回調函數
	//LEV_OPT_REUSEABLE|LEV_OPT_CLOSE_ON_FREE端口復用,當鏈接監聽器釋放的時候關閉套接字(監聽文件描述符)
	listener = evconnlistener_new_bind(base, listener_cb, (void *)base,
	    LEV_OPT_REUSEABLE|LEV_OPT_CLOSE_ON_FREE, -1,
	    (struct sockaddr*)&sin,
	    sizeof(sin));
 
	if (!listener) {
		fprintf(stderr, "Could not create a listener!\n");
		return 1;
	}
	
	//設置SIGINT(ctrl+c)信號的事件回調(也有上地基event_add)
	signal_event = evsignal_new(base, SIGINT, signal_cb, (void *)base);
	
	if (!signal_event || event_add(signal_event, NULL)<0) {
		fprintf(stderr, "Could not create/add a signal event!\n");
		return 1;
	}
 
	//進入等待事件循環---相當於while(1)
	event_base_dispatch(base);
 
	//釋放資源
	evconnlistener_free(listener);
	event_free(signal_event);
	event_base_free(base);
 
	printf("done\n");
	return 0;
}

//listener: 鏈接監聽器
//fd: 通信文件描述符
//sa和socklen: 客戶端IP及端口號
//user_data: 參數
static void
listener_cb(struct evconnlistener *listener, evutil_socket_t fd,
    struct sockaddr *sa, int socklen, void *user_data)
{
	struct event_base *base = user_data;
	struct bufferevent *bev;
 
	//創建bufferevent緩衝區
	//BEV_OPT_CLOSE_ON_FREE: bufferevent釋放的時候自動關閉通信文件描述符
	bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
	if (!bev) {
		fprintf(stderr, "Error constructing bufferevent!");
		event_base_loopbreak(base);//退出循環,程序結束
		return;
	}
	//設置回調函數: 讀回調,寫回調和事件回調
	bufferevent_setcb(bev, conn_readcb, conn_writecb, conn_eventcb, NULL);
	//使bufferevent設置生效
	bufferevent_enable(bev, EV_WRITE);
	bufferevent_enable(bev, EV_READ);

	//使bufferevent設置無效
	// bufferevent_disable(bev, EV_READ);
 
	bufferevent_write(bev, MESSAGE, strlen(MESSAGE));
}

static void
conn_readcb(struct bufferevent *bev, void *user_data)
{
	char buf[1024];
	memset(buf, 0x00, sizeof(buf));
	int n = bufferevent_read(bev, buf, sizeof(buf));
	printf("n=[%d],buf==[%s]\n", n, buf);
	int i = 0;
	for (i = 0; i < n; i++)
	{
		buf[i] = toupper(buf[i]);
	}
	//往bufferevent的寫緩衝區寫數據
	bufferevent_write(bev, buf, n);//寫bufferevent緩衝區會觸發寫事件回調
}

static void
conn_writecb(struct bufferevent *bev, void *user_data)
{
	//獲取寫緩衝區指針
	struct evbuffer *output = bufferevent_get_output(bev);
	if (evbuffer_get_length(output) == 0) {
		printf("flushed answer\n");
		// bufferevent_free(bev);
	}
}

static void
conn_eventcb(struct bufferevent *bev, short events, void *user_data)
{
	if (events & BEV_EVENT_EOF) {
		printf("Connection closed.\n");
	} else if (events & BEV_EVENT_ERROR) {
		printf("Got an error on the connection: %s\n",
		    strerror(errno));/*XXX win32*/
	}
	/* None of the other events can happen here, since we haven't enabled
	 * timeouts */
	bufferevent_free(bev);
}

//SIGINT(ctrl+c)信號回調函數
static void
signal_cb(evutil_socket_t sig, short events, void *user_data)
{
	struct event_base *base = user_data;
	struct timeval delay = { 2, 0 };
 
	printf("Caught an interrupt signal; exiting cleanly in two seconds.\n");
 
	event_base_loopexit(base, &delay);
}