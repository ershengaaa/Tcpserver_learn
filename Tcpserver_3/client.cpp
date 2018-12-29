#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#define MAXLINE 4096

int main(int argc, char ** argv)
{
    int connfd, n;
    char recvline[4096], sendline[4096];
    struct sockaddr_in servaddr;
    if (argc != 2) {
        printf("usage: ./client <ipaddress>\n");
        return 0;
    }

    if( (connfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("create socket error: %s(errno: %d)\n", strerror(errno), errno);
        return 0;
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(6666);
    if ( inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0) {
        printf("inet_pton error for %s\n", argv[1]);
        return 0;
    }

    if (connect(connfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        printf("connect error: %s(errno: %d)\n", strerror(errno), errno);
        return 0;
    }

    ssize_t writelen;
	char sendmsg[246988] = {0};
	int count = 0;
	while (1)
	{
		++count;
		if (count == 5) {
			return 0;
		}
		writelen = write(connfd, sendmsg, sizeof(sendmsg)); //循环给服务端发送数据包 
		if (writelen < 0) {
			printf("write error\n");
			close(connfd);
			return 0;
		}
		else
		{
			printf("write success, writelen: %d\n", writelen);
		}
	}
	close(connfd);
	return 0;
}
