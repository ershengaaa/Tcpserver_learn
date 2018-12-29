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
#include "define.h"   //发送的结构体 
using namespace std;


#define DEFAULT_PORT  6666
int main(int argc, char **argv)
{
    int sockfd, acceptfd;
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
    char recvmsg[1000];
    sin_size = sizeof(my_addr);
    acceptfd = accept(sockfd, (struct sockaddr*)&my_addr, &sin_size);
    if (acceptfd < 0) {
        close(sockfd);
        printf("accept failed\n");
        return -4;
    }
    ssize_t readlen = MyRecv(acceptfd, recvmsg, sizeof(int));  //解包 
    if (readlen < 0) {
        printf("read failed\n");
        return -1;
    }
    int len = (int)ntohl(*(int *)recvmsg);
    printf("len: %d\n", len);
    //将结构体中的数据逐条进行解析 
    readlen = MyRecv(acceptfd, recvmsg, len);
    if (readlen < 0) {
        printf("read failed\n");
        return -1;
    }
	char *pBuff = recvmsg;
	Pkg recvPkg;
	int iLen = 0
	//对于int型的数据，需要将其网络字节序转换为主机序;
	memcpy(&recvPkg.head.num, pBuff + iLen, sizeof(int)); //包id 
	iLen += sizeof(int);
	recvPkg.head.num = ntohl(recvPkg.head.num);
	printf("RecvPkg.head.num:%d\n", recvPkg.head.num);

	memcpy(&recvPkg.head.index, pBuff + iLen, sizeof(int)); //学生编号 
    iLen += sizeof(int);
    recvPkg.head.index = ntohl(recvPkg.head.index);
    printf("RecvPkg.head.index:%d\n", recvPkg.head.index);

	memcpy(&recvPkg.content.sex, pBuff + iLen, sizeof(char)); //性别 
    iLen += sizeof(char);
    printf("RecvPkg.content.sex:%c\n", recvPkg.content.sex);

	memcpy(&recvPkg.content.score, pBuff + iLen, sizeof(int)); //分数 
    iLen += sizeof(int);
    recvPkg.content.score = ntohl(recvPkg.content.score);
    printf("RecvPkg.content.score:%d\n", recvPkg.content.score);

	memcpy(&recvPkg.content.address, pBuff + iLen, sizeof(recvPkg.content.address)); //地址 
    iLen += sizeof(recvPkg.content.address);
    printf("RecvPkg.content.address:%s\n", recvPkg.content.address);

	memcpy(&recvPkg.content.age, pBuff + iLen, sizeof(int)); //年龄 
	iLen += sizeof(int);
	recvPkg.content.age = ntohl(recvPkg.content.age);
	printf("RecvPkg.content.age:%d\n", recvPkg.content.age);

	close(acceptfd);
	return 0;
}







