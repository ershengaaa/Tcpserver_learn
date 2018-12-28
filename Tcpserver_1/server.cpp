#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>  //用于错误处理 
#include <sys/types.h>  //数据类型定义 
#include <sys/socket.h>  //提供socket函数及数据结构 
#include <netinet/in.h>  //定义数据结构sockaddr_in，协议地址 
#include <unistd.h>     //提供通用的文件，目录，程序及进程操作的函数 

#define MAXLINE 4096

int main(int argc, char ** argv)
{
	int listenfd, connfd;
	struct sockaddr_in servaddr;   //ipv4 
	char buff[4096];
	int n;

	if ( (listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {  //创建socket 
		printf("creat socket error: %s(errno: %d)\n", strerror(errno), errno);
		return 0;
	}
	//初始化协议地址 
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(6666);

	if ( bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1) { //绑定socket和端口号 
		printf("bind socket error: %s(errno: %d)\n", strerror(errno), errno);
		return 0;
	}

	if ( listen(listenfd, 10) == -1) {  //监听端口号 
		printf("listen socket error: %s(errno: %d)\n", strerror(errno), errno);
		return 0;
	}

	printf("==========waiting for client's request==========\n");
	while(1)
	{
		if ( (connfd = accept(listenfd, (struct sockaddr*)NULL, NULL)) == -1) {  //接收来自客户端的连接请求 
			printf("accept socket error:%s(errno: %d)\n", strerror(errno), errno);
			continue;
		}
		n = recv(connfd, buff, MAXLINE, 0);  //从socket中读取字符 
		buff[n] = '\0';
		printf("recv msg from client: %s\n", buff);
		close(connfd);  //关闭连接 
	}
	close(listenfd);  //关闭socket 
	return 0;
}
