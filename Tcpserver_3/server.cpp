#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define MAXLINE 4096

int main(int argc, char ** argv)
{
    int listenfd, acceptfd;
    struct sockaddr_in servaddr;

    if ( (listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        printf("creat socket error: %s(errno: %d)\n", strerror(errno), errno);
        return 0;
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(6666);

    if ( bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1) {
        printf("bind socket error: %s(errno: %d)\n", strerror(errno), errno);
        return 0;
    }

    if ( listen(listenfd, 10) == -1) {
        printf("listen socket error: %s(errno: %d)\n", strerror(errno), errno);
        return 0;
    }

    printf("==========waiting for client's request==========\n");
    if ((acceptfd = accept(listenfd, (struct sockaddr*)NULL, NULL)) == -1) {
        printf("accept socket error: %s(errno: %d)\n", strerror(errno), errno);
    }
	else
	{
		printf("accept sucess\n");
		int rcvbuf_len = 0;
		socklen_t len = sizeof(rcvbuf_len);
		//获取默认缓冲区大小，也可以通过setsockopt()来设置默认缓冲区大小 
		if (getsockopt(acceptfd, SOL_SOCKET, SO_RCVBUF, &rcvbuf_len, &len) < 0) {
			perror("getsockopt: ");
		}
		printf("the recv buf len: %d\n", rcvbuf_len);
	}
	char recvmsg[246988] = {0};  //定义接收大小 
	ssize_t totallen = 0;
	while (1) { //循环处理数据包 
		sleep(1);   //每接受一次就休眠一次 
		ssize_t readlen = read(acceptfd, recvmsg, sizeof(recvmsg));  // 接收数据包 
		printf("readLen: %d\n", readlen);
		if (readlen < 0) {
			perror("read: ");
			return -1;
		}
		else if (readlen == 0) {
			printf("read finish: len = % ld\n", totallen);
			close(acceptfd);
			return 0;
		}
		else
		{
			totallen += readlen;
		}
	}
	close(acceptfd);
	return 0;
}
