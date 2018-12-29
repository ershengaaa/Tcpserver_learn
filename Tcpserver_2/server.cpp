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
	
	char recvmsg[100];
	ssize_t readLen = read(acceptfd, recvmsg, sizeof(recvmsg));  //先接收100个字节 
	if (readLen < 0) {
		printf("read error: %s(errno: %d)\n", strerror(errno), errno);
		return -1;
	}

	recvmsg[9] = '\0';
	printf("readLen:%d, recvmsg:%s\n", readLen, recvmsg);
	sleep(5);    //休眠5s 
	recvmsg[1] = '9';  //把第2个字符改为9 
	ssize_t writeLen = write(acceptfd, recvmsg, sizeof(recvmsg));  //给客户端回更新之后的包 
	printf("writeLen:%d, sendMsg:%s\n", writeLen, recvmsg);
	if (writeLen < 0) {
		printf("write error: %s(errno: %d)\n", strerror(errno), errno);
		return -1;
	}
	close(acceptfd);
	return 0;
}






