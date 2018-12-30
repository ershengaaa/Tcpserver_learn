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
#include <sys/epoll.h> //�ṩepoll���� 

#define IPADDRESS "127.0.0.1"  
#define DEFAULT_PORT  6666
#define MAXSIZE 1024
#define LISTENQ 5
#define FDSIZE 1000
#define EPOLLEVENTS 100

//�����׽��ֲ����а� 
int socket_bind(const char* ip, int port);
//IO��·����epoll 
void do_epoll(int listenfd);
//�¼������� 
void handle_events(int epollfd, struct epoll_event *events, int num, int listenfd, char *buf);
//������յ������� 
void handle_accept(int epollfd, int listenfd);
//������ 
void do_read(int epollfd, int fd, char *buf);
//д���� 
void do_write(int epollfd, int fd, char *buf);
//����¼� 
void add_event(int epollfd, int fd, int state);
//�޸��¼� 
void modify_event(int epollfd, int fd, int state);
//ɾ���¼� 
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
	inet_pton(AF_INET, ip, &servaddr.sin_addr); //���ʮ�����������֮��ת�� 
	servaddr.sin_port = htons(port);
	if (bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1) {
		perror("bind error: ");
		exit(1);
	}
	return listenfd;
}



void do_epoll(int listenfd) {
	int epollfd;
	struct epoll_event events[EPOLLEVENTS];  //�����¼� 
	int ret;
	char buf[MAXSIZE];
	memset(buf, 0, MAXSIZE);
	epollfd = epoll_create(FDSIZE);  //����һ��������
	//��� �����������¼� 
	add_event(epollfd,listenfd, EPOLLIN);
	while (1)
	{
		//�ȴ��¼��ķ��� 
		ret = epoll_wait(epollfd, events, EPOLLEVENTS, -1);
		//�����¼� 
		handle_events(epollfd, events, ret, listenfd, buf);
	}
	close(epollfd);
}
void handle_events(int epollfd,struct epoll_event *events,int num, int listenfd, char *buf) {
	int i;
	int fd;
	for (i = 0; i < num; ++i) {
		fd = events[i].data.fd;
		//���������������ͺ��¼����ͽ��д��� 
		if ((fd == listenfd) && (events[i].events & EPOLLIN))
			//������յ������� 
			handle_accept(epollfd, listenfd);
		else if (events[i].events & EPOLLIN) 
			///������ 
			do_read(epollfd, fd, buf);
		else if (events[i].events & EPOLLOUT)
			//д���� 
			do_write(epollfd, fd, buf);
	}
}

void handle_accept(int epollfd, int listenfd) {
	int clifd;
	struct sockaddr_in cliaddr;
	socklen_t cliaddrlen;
	//�������� 
	clifd = accept(listenfd, (struct sockaddr*)&cliaddr, &cliaddrlen);
	if (clifd == -1) 
		perror("accept error:");
	else {
		//���ӳɹ����������������¼� 
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
		//�޸���������Ӧ���¼����ɶ���Ϊд 
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



