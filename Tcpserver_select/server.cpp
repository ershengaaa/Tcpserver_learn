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

#define DEFAULT_PORT  6666
int main(int argc, char **argv)
{
    int serverfd, acceptfd;
    struct sockaddr_in my_addr;
    unsigned int sin_size, myport = 6666, lisnum = 10;
    if ((serverfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        return -1;
    }
    printf("socket ok\n");
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(DEFAULT_PORT);
    my_addr.sin_addr.s_addr = INADDR_ANY;
    bzero(&(my_addr.sin_zero), 0);  //初始化 
    if (bind(serverfd, (struct sockaddr*)&my_addr, sizeof(struct sockaddr)) == -1) {
        perror("bind");
        return -2;
    }
    printf("bind ok\n");
    if (listen(serverfd, lisnum) == -1) {
        perror("listen");
        return -3;
    }

    printf("listen ok\n");
    fd_set client_fdset;  	//监听文件描述符集合 
	int maxsock;     		//监听文件描述符中最大的文件号 
	struct timeval tv; 		 //设置超时时间 
	int client_sockfd[5]; 	 //存放活动的sockfd 
	bzero((void *)client_sockfd, sizeof(client_sockfd));
	int conn_amount = 0;   	//用来记录文件描述符集合 
	maxsock = serverfd;   
	char buffer[1024];
	int ret = 0;
	while (1) {  //循环处理 
		FD_ZERO(&client_fdset); 	 //初始化文件描述符到集合 
		FD_SET(serverfd, &client_fdset);  //加入服务器描述符
		//设置超时时间为30s 
		tv.tv_sec = 30;
		tv.tv_usec = 0;
		//将活动的sockfd加入到文件描述符中 
		for (int i = 0; i < 5; ++i) {
			if (client_sockfd[i] != 0) {
				FD_SET(client_sockfd[i], &client_fdset);
			}
		}
		ret = select(maxsock + 1, &client_fdset, NULL, NULL, &tv); 	//select函数，实现多路IO复用 
		if (ret < 0) {
			perror("select error\n");
			break;
		}
		else if (ret == 0) {
			printf("time out\n");
			continue;
		}
		//轮询各个文件描述符 
		for (int  i = 0; i < conn_amount; ++i) {
			if (FD_ISSET(client_sockfd[i], &client_fdset)) { 	// FD_ISSET检查是否sockfd是否可读 
				printf("start recv from client[%d]:\n", i);
				//检查是否有数据传输，没有则关闭连接 ，并在文件描述符中清空 
				ret = recv(client_sockfd[i], buffer, 1024, 0);
				if (ret <= 0) {
					printf("client[%d] close\n", i);
					close(client_sockfd[i]);
					FD_CLR(client_sockfd[i], &client_fdset);
					client_sockfd[i] = 0;
				}
				else
					printf("recv from client[%d] :%s\n", i, buffer);
			}
		}
		//检查是否有新连接 ，如果有，则接收，放入文件描述符集合 
		if (FD_ISSET(serverfd, &client_fdset)) {
			struct sockaddr_in client_addr;
			size_t size = sizeof(sockaddr_in);
			//接受连接 
			int sock_client = accept(serverfd, (struct sockaddr*)(&client_addr), (unsigned int *)(&size));
			if (sock_client < 0) {
				perror("accept error\n");
				continue;
			}
			//把连接加入到文件描述符集合 
			if (conn_amount < 5) {
				client_sockfd[conn_amount ++] = sock_client;
				bzero(buffer, 1024);
				strcpy(buffer, "this is server! welcome!\n");
				send(sock_client, buffer, 1024, 0);
				printf("new connection client[%d] %s:%d\n", conn_amount, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
				bzero(buffer, sizeof(buffer));
				ret = recv(sock_client, buffer, 1024,0);
				if (ret < 0) {
					perror("recv error\n");
					close(serverfd);
					return -1;
				}
				printf("recv: %s\n", buffer);
				//如果当前的文件号大于最大的，则将最大值更新 
				if (sock_client > maxsock) {
					maxsock = sock_client;
				}
				else {
					printf("max connections!!!quit!!\n");
					break;
				}
			}
		}
	}
	//关闭所有连接 
	for (int i = 0; i < 5; ++i) {
		if (client_sockfd[i] != 0) {
			close(client_sockfd[i]);
		}
	}
	close(serverfd); //关闭socket 
	return 0;
}

