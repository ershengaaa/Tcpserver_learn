#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>   //���ڴ����� 
#include <sys/types.h>  //�������Ͷ��� 
#include <sys/socket.h> //�ṩsocket���������ݽṹ 
#include <netinet/in.h> //�������ݽṹsockaddr_in��Э���ַ 
#include <arpa/inet.h>  //�ṩip��ַת������ 
#include <unistd.h>    //�ṩͨ�õ��ļ���Ŀ¼�����򼰽��̲����ĺ��� 
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

	if( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {  //����socket 
		printf("create socket error: %s(errno: %d)\n", strerror(errno), errno);
		return 0;
	}
	//��ʼ��Э���ַ 
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(6666);
	if ( inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0) {  //ת���ַ����������ַ 
		printf("inet_pton error for %s\n", argv[1]);
		return 0;
	}

	if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {  //����ָ��������Ķ˿� 
		printf("connect error: %s(errno: %d)\n", strerror(errno), errno);
		return 0;
	}

	printf("send msg to server: \n");
	fgets(sendline, 4096, stdin);   //�����ַ��� 
	if (send(sockfd, sendline, strlen(sendline), 0) < 0) {   //��socket��д����Ϣ 
		printf("send msg error: %s(errno: %d)\n", strerror(errno), errno);
		return 0;
	}
	close(sockfd); //�ر�socket 
	return 0;
}
