#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>  //提供进程等待，进程间通讯及共享内存的函数 
using namespace std;
//接收指定长度的数据包 
int MyRecv(int isock, char * pchBuf, size_t tCount) {
	size_t tByteRead = 0;
	int iThisRead;
	while (tByteRead < tCount) {
		do {
			iThisRead = read(isock, pchBuf, tCount - iThisRead);
		}while ((iThisRead < 0) && (errno == EINTR));
		if (iThisRead < 0) {
			return (iThisRead);
		}
		else if (iThisRead == 0)
			return (tByteRead);
		tByteRead += iThisRead;
		pchBuf += iThisRead;
	}
}
#define DEFAULT_PORT  6666
int main(int argc, char **argv) 
{
	int sockfd, acceptfd;  //监听socket:sockfd，数据传输socket：acceptfd 
	struct sockaddr_in my_addr;
	unsigned int sin_size, myport = 6666, lisnum = 10;
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		return -1;
	}
	printf("socket ok\n");
	//memset(&my_addr, 0, sizeof(my_addr));
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(DEFAULT_PORT);
	my_addr.sin_addr.s_addr = INADDR_ANY;
	bzero(&(my_addr.sin_zero), 0);
	if (bind(sockfd, (struct sockaddr*)&my_addr, sizeof(struct sockaddr)) == -1) {
		perror("bind");
		return -2;
	}
	printf("bind ok\n");
	if (listen(sockfd, lisnum) == -1) {
		perror("listen");
		return -3;
	}
	printf("listen ok\n");
	char recvmsg[10];
	sin_size = sizeof(my_addr);
	acceptfd = accept(sockfd, (struct sockaddr*)&my_addr, &sin_size);
	if (acceptfd < 0) {
		close(sockfd);
		printf("accept failed\n");
		return -4;
	}
	ssize_t readlen = MyRecv(acceptfd, recvmsg, sizeof(int));  //读取包，即解包 
	if (readlen < 0) {
		printf("read failed\n");
		return -1;
	}
	int len = (int)ntohl(*(int *)recvmsg);   //将网络字节序转换为主机序 
	printf("len: %d\n", len);
	readlen = MyRecv(acceptfd, recvmsg, len);  //读取数据 
	if (readlen < 0) {
		printf("read failed\n");
		return -1;
	}
	recvmsg[len] = '\0';
	printf("recvmsg: %s\n", recvmsg);
	close(acceptfd);
	return 0;
}
