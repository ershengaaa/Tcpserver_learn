#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <sys/epoll.h> //提供epoll函数 

#define IPADDRESS "127.0.0.1"  
#define DEFAULT_PORT  6666
#define MAXSIZE 1024
#define LISTENQ 5
#define FDSIZE 1000
#define EPOLLEVENTS 100

//创建套接字并进行绑定 
int socket_bind(const char* ip, int port);
//IO多路复用epoll 
void do_epoll(int listenfd);
//事件处理函数 
void handle_events(int epollfd, struct epoll_event *events, int num, int listenfd, char *buf);
//处理接收到的连接 
void handle_accept(int epollfd, int listenfd);
//读处理 
void do_read(int epollfd, int fd, char *buf);
//写处理 
void do_write(int epollfd, int fd, char *buf);
//添加事件 
void add_event(int epollfd, int fd, int state);
//修改事件 
void modify_event(int epollfd, int fd, int state);
//删除事件 
void delete_event(int epollfd, int fd, int state);

int main(int argc, char **argv) {
	int listenfd;
	listenfd = socket_bind(IPADDRESS, DEFAULT_PORT);
	listen(listenfd, LISTENQ);
	do_epoll(listenfd);
	return 0;
}

int socket_bind(const char* ip, int port) {
	int listenfd;
	struct sockaddr_in servaddr;
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (listenfd == -1) {
		perror("socket error: ");
		exit(1);
	}
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	inet_pton(AF_INET, ip, &servaddr.sin_addr); //点分十进制与二进制之间转换 
	servaddr.sin_port = htons(port);
	if (bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1) {
		perror("bind error: ");
		exit(1);
	}
	return listenfd;
}



void do_epoll(int listenfd) {
	int epollfd;
	struct epoll_event events[EPOLLEVENTS];  //监听事件 
	int ret;
	char buf[MAXSIZE];
	memset(buf, 0, MAXSIZE);
	epollfd = epoll_create(FDSIZE);  //创建一个描述符
	//添加 监听描述符事件 
	add_event(epollfd,listenfd, EPOLLIN);
	while (1)
	{
		//等待事件的发生 
		ret = epoll_wait(epollfd, events, EPOLLEVENTS, -1);
		//处理事件 
		handle_events(epollfd, events, ret, listenfd, buf);
	}
	close(epollfd);
}
void handle_events(int epollfd,struct epoll_event *events,int num, int listenfd, char *buf) {
	int i;
	int fd;
	for (i = 0; i < num; ++i) {
		fd = events[i].data.fd;
		//根据描述符的类型和事件类型进行处理 
		if ((fd == listenfd) && (events[i].events & EPOLLIN))
			//处理接收到的连接 
			handle_accept(epollfd, listenfd);
		else if (events[i].events & EPOLLIN) 
			///读处理 
			do_read(epollfd, fd, buf);
		else if (events[i].events & EPOLLOUT)
			//写处理 
			do_write(epollfd, fd, buf);
	}
}

void handle_accept(int epollfd, int listenfd) {
	int clifd;
	struct sockaddr_in cliaddr;
	socklen_t cliaddrlen;
	//接受连接 
	clifd = accept(listenfd, (struct sockaddr*)&cliaddr, &cliaddrlen);
	if (clifd == -1) 
		perror("accept error:");
	else {
		//连接成功，将描述符加入事件 
		printf("accept a new client: %s:%d\n", inet_ntoa(cliaddr.sin_addr), cliaddr.sin_port);
		add_event(epollfd, clifd, EPOLLIN);
	}
}

void do_read(int epollfd, int fd, char *buf) {
	int nread;
	nread = read(fd, buf, MAXSIZE);
	if (nread == -1) {
		perror("read error:");
		close(fd);
		delete_event(epollfd, fd, EPOLLIN);
	}
	else if (nread == 0) {
		fprintf(stderr, "client close.\n");
		close(fd);
		delete_event(epollfd, fd, EPOLLIN);
	}
	else
	{
		printf("read message is : %s",buf);
		//修改描述符对应的事件，由读改为写 
		modify_event(epollfd, fd, EPOLLOUT);
	}
}


void do_write(int epollfd, int fd, char * buf) {
	int nwrite;
	nwrite = write(fd, buf, strlen(buf));
	if (nwrite == -1) {
		perror("write error:");
		close(fd);
		delete_event(epollfd, fd, EPOLLOUT);
	}
	else
		modify_event(epollfd, fd, EPOLLIN);
	memset(buf, 0, MAXSIZE);
}

void add_event(int epollfd, int fd, int state) {
	struct epoll_event ev;
	ev.events = state;
	ev.data.fd = fd;
	epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
}

void modify_event(int epollfd, int fd, int state) {
    struct epoll_event ev;
    ev.events = state;
    ev.data.fd = fd;
    epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &ev);
}

void delete_event(int epollfd, int fd, int state) {
    struct epoll_event ev;
    ev.events = state;
    ev.data.fd = fd;
    epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, &ev);
}



