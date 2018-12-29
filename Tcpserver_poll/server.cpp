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
#include <poll.h>

#define IPADDRESS "127.0.0.1"
#define DEFAULT_PORT  6666
#define MAXLINE 1024    //����������ݰ� 
#define LISTENQ 5      //��Ӧsocket�Ŷ����ӵ������ 
#define OPEN_MAX 1000  //���ӵ������ 
#define INFTIM -1   

//����socket�����а󶨺ͼ��� 
int bind_and_listen() {
	int serverfd;
    struct sockaddr_in my_addr;
	unsigned int sin_size;
    if ((serverfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        return -1;
    }
    printf("socket ok\n");
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(DEFAULT_PORT);
    my_addr.sin_addr.s_addr = INADDR_ANY;
    bzero(&(my_addr.sin_zero), 0);
    if (bind(serverfd, (struct sockaddr*)&my_addr, sizeof(struct sockaddr)) == -1) {
        perror("bind");
        return -2;
    }
    printf("bind ok\n");
    if (listen(serverfd, LISTENQ) == -1) {
        perror("listen");
        return -3;
    }

    printf("listen ok\n");
	return serverfd;
}

//IO��·����poll 
void do_poll(int listenfd) {
	int connfd, sockfd; 
	struct sockaddr_in cliaddr;
	socklen_t cliaddrlen;
	struct pollfd clientfds[OPEN_MAX];
	int maxi;
	int i;
	int nready; 
	//��Ӽ��������� 
	clientfds[0].fd = listenfd;
	clientfds[0].events = POLLIN;
	//��ʼ���ͻ����������� 
	for (i = 1; i < OPEN_MAX; ++i) {
		clientfds[i].fd = -1;
	}
	maxi = 0;
	//ѭ������ 
	while(1) {
		nready = poll(clientfds, maxi + 1, INFTIM); //��ȡ�����������ĸ��� 
		if (nready == -1) {
			perror("poll error:");
			exit(1);
		}
		if (clientfds[0].revents & POLLIN) { //�����ļ��������Ƿ�׼���� 
			cliaddrlen = sizeof(cliaddr);
			//�����µ����� 
			if ((connfd = accept(listenfd, (struct sockaddr*)(&cliaddr), &cliaddrlen)) == -1) {
				if (errno == EINTR)
					continue;
				else {
					perror("accept error:");
					exit(1);
				}
			}
			fprintf(stdout, "accept a new client: %s:%d\n", inet_ntoa(cliaddr.sin_addr), cliaddr.sin_port);
			//���µ��ļ����������뵽������ 
			for (i = 1; i < OPEN_MAX ; ++i) {
				if (clientfds[i].fd < 0) {
					clientfds[i].fd = connfd;
					break;
				}
			}
			//�ﵽ�������� 
			if (i == OPEN_MAX) {
				fprintf(stderr, "too many clients.\n");
				exit(1);
			}
			//���µ����������뵽�������������� 
			clientfds[i].events = POLLIN;
			//��¼�ͻ�����socket�ĸ��� 
			maxi = (i > maxi ? i : maxi);
			if (--nready <= 0) {
				continue;
			}
		}
		//�����������Ͽͻ��˷����İ� 
		char buf[MAXLINE];
		memset(buf, 0, MAXLINE);
		int readlen = 0;
		for (i = 1; i <= maxi; ++i) {
			if (clientfds[i].fd < 0) {
				continue;
			}
			if (clientfds[i].revents & POLLIN) { //���Կͻ��������Ƿ�׼���� 
			//���ܿͻ��˷�������Ϣ 
				readlen = read(clientfds[i].fd, buf, MAXLINE);
				if (readlen == 0) {
					close(clientfds[i].fd);
					clientfds[i].fd = -1;
					continue;
				}
				write(STDOUT_FILENO, buf, readlen);
				//��ͻ��˻��� 
				write(clientfds[i].fd, buf, readlen);
			}
		}
	}
}

int main(int argc, char **argv) {
	int listenfd = bind_and_listen();
	if (listenfd < 0) {
		return 0;
	}
	do_poll(listenfd);
	return 0;
}
	

