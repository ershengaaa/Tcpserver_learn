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
#include <sys/epoll.h>  //�ṩepoll���� 

#define IPADDRESS "127.0.0.1"
#define MAXSIZE 1024
#define SERV_PORT 6666
#define FD_SIZE 1024
#define EPOLLEVENTS 20

//�������� 
void handle_connection(int sockfd);
//�����¼� 
void handle_events(int epollfd, struct epoll_event *events, int num, int sockfd, char *buf);
//������ 
void do_read(int epollfd,int fd,int sockfd,  char *buf);
//д���� 
void do_write(int epollfd,int fd, int sockfd, char *buf);
//����¼� 
void add_event(int epollfd,  int fd, int state);
//�޸��¼� 
void modify_event(int epollfd, int fd, int state);
//ɾ���¼� 
void delete_event(int epollfd, int fd, int state);
//���͵���Ϣ��ţ���01��02��03... 
int count = 0;

int main(int argc, char **argv) {
	int sockfd;
	struct sockaddr_in servaddr;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	bzero(&servaddr, sizeof(sockaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);
	inet_pton(AF_INET, IPADDRESS, &servaddr.sin_addr);  //����ֶ����Ƹ�Ϊʮ���� 
	connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)); //�������� 
	handle_connection(sockfd);   //�������� 
	close(sockfd);
	return 0;
}

void handle_connection(int sockfd) {
	int epollfd;
	struct epoll_event events[EPOLLEVENTS];
	char buf[MAXSIZE];
	int ret;
	epollfd = epoll_create(FD_SIZE);  //����һ�������� 
	add_event(epollfd, STDIN_FILENO, EPOLLIN); //��ӱ�׼��������ͼ��¼� 
	while (1) {
		//��ȡ�Ѿ�׼�����¼������д��� 
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
			//������ 
			do_read(epollfd, fd, sockfd, buf);
		}
		else if (events[i].events & EPOLLOUT) {
			//д���� 
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
		//���Ա�׼������������ݣ���һ��д״̬���¼� 
		if (fd == STDIN_FILENO) {
			add_event(epollfd, sockfd, EPOLLOUT);
		}
		//���Է���˵����ݣ��Ƚ����������׼�����У����Ѷ��¼�ɾ�������д�¼� 
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
	snprintf(temp, sizeof(temp), "%s_%02d\n", buf, count++); //���һ������ŵ����� 
	nwrite = write(fd, temp, strlen(temp));
	if (nwrite == -1) {
		perror("write error:");
		close(fd);
	}
	else {
		//���׼���д����������ɾ���¼� 
		if (fd == STDOUT_FILENO) {
			delete_event(epollfd, fd, EPOLLOUT);
		}
		//��������д����Ҫ�޸Ĵ��¼���״̬ 
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
