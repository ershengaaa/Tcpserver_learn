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
#include <sys/epoll.h>  //提供epoll函数 

#define IPADDRESS "127.0.0.1"
#define MAXSIZE 1024
#define SERV_PORT 6666
#define FD_SIZE 1024
#define EPOLLEVENTS 20

//处理连接 
void handle_connection(int sockfd);
//处理事件 
void handle_events(int epollfd, struct epoll_event *events, int num, int sockfd, char *buf);
//读处理 
void do_read(int epollfd,int fd,int sockfd,  char *buf);
//写处理 
void do_write(int epollfd,int fd, int sockfd, char *buf);
//添加事件 
void add_event(int epollfd,  int fd, int state);
//修改事件 
void modify_event(int epollfd, int fd, int state);
//删除事件 
void delete_event(int epollfd, int fd, int state);
//发送的信息编号，即01、02、03... 
int count = 0;

int main(int argc, char **argv) {
	int sockfd;
	struct sockaddr_in servaddr;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	bzero(&servaddr, sizeof(sockaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);
	inet_pton(AF_INET, IPADDRESS, &servaddr.sin_addr);  //将点分二进制改为十进制 
	connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)); //发起连接 
	handle_connection(sockfd);   //处理连接 
	close(sockfd);
	return 0;
}

void handle_connection(int sockfd) {
	int epollfd;
	struct epoll_event events[EPOLLEVENTS];
	char buf[MAXSIZE];
	int ret;
	epollfd = epoll_create(FD_SIZE);  //创建一个描述符 
	add_event(epollfd, STDIN_FILENO, EPOLLIN); //添加标准输入的类型及事件 
	while (1) {
		//获取已经准备的事件，进行处理 
		ret = epoll_wait(epollfd, events, EPOLLEVENTS, -1);
		handle_events(epollfd, events, ret, sockfd, buf);
	}
	close(epollfd);
}

void  handle_events(int epollfd, struct epoll_event *events, int num, int sockfd, char *buf) {
	int fd;
	int i;
	for (i = 0; i < num; ++i) {
		fd = events[i].data.fd;
		if (events[i].events & EPOLLIN) {
			//读处理 
			do_read(epollfd, fd, sockfd, buf);
		}
		else if (events[i].events & EPOLLOUT) {
			//写处理 
			do_write(epollfd, fd, sockfd, buf);
		}
	}
}

void do_read(int epollfd, int fd, int sockfd, char *buf) {
	int nread;
	nread = read(fd, buf, MAXSIZE);
	if (nread == -1) {
		perror("read error:");
		close(fd);
	}
	else {
		//来自标准输入读到的数据，加一个写状态的事件 
		if (fd == STDIN_FILENO) {
			add_event(epollfd, sockfd, EPOLLOUT);
		}
		//来自服务端的数据，先将其输出到标准输入中，并把读事件删除，添加写事件 
		else{
			delete_event(epollfd, fd, EPOLLIN);
			add_event(epollfd, STDOUT_FILENO, EPOLLOUT);
		}
	}
}

void do_write(int epollfd, int fd, int sockfd, char *buf) {
	int nwrite;
	char temp[100];
	buf[strlen(buf)-1] = '\0';
	snprintf(temp, sizeof(temp), "%s_%02d\n", buf, count++); //输出一个带编号的数据 
	nwrite = write(fd, temp, strlen(temp));
	if (nwrite == -1) {
		perror("write error:");
		close(fd);
	}
	else {
		//向标准输出写，操作结束删除事件 
		if (fd == STDOUT_FILENO) {
			delete_event(epollfd, fd, EPOLLOUT);
		}
		//向网络中写，需要修改此事件的状态 
		else
			modify_event(epollfd, fd, EPOLLIN);
	}
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
