#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>   //用于错误处理 
#include <sys/types.h>  //数据类型定义 
#include <sys/socket.h> //提供socket函数及数据结构 
#include <netinet/in.h> //定义数据结构sockaddr_in，协议地址 
#include <arpa/inet.h>  //提供ip地址转换函数 
#include <unistd.h>    //提供通用的文件，目录，程序及进程操作的函数 
#define MAXLINE 4096

int main(int argc, char ** argv)
{
	int sockfd, n;
	char recvline[4096], sendline[4096];
	struct sockaddr_in servaddr;   //ipv4 
	if (argc != 2) {
		printf("usage: ./client <ipaddress>\n");
		return 0;
	}

	if( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {  //创建socket 
		printf("create socket error: %s(errno: %d)\n", strerror(errno), errno);
		return 0;
	}
	//初始化协议地址 
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(6666);
	if ( inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0) {  //转换字符串到网络地址 
		printf("inet_pton error for %s\n", argv[1]);
		return 0;
	}

	if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {  //连接指定计算机的端口 
		printf("connect error: %s(errno: %d)\n", strerror(errno), errno);
		return 0;
	}

	printf("send msg to server: \n");
	fgets(sendline, 4096, stdin);   //读入字符串 
	if (send(sockfd, sendline, strlen(sendline), 0) < 0) {   //向socket中写入信息 
		printf("send msg error: %s(errno: %d)\n", strerror(errno), errno);
		return 0;
	}
	close(sockfd); //关闭socket 
	return 0;
}
