#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>  //���ڴ����� 
#include <sys/types.h>  //�������Ͷ��� 
#include <sys/socket.h>  //�ṩsocket���������ݽṹ 
#include <netinet/in.h>  //�������ݽṹsockaddr_in��Э���ַ 
#include <unistd.h>     //�ṩͨ�õ��ļ���Ŀ¼�����򼰽��̲����ĺ��� 

#define MAXLINE 4096

int main(int argc, char ** argv)
{
	int listenfd, connfd;
	struct sockaddr_in servaddr;   //ipv4 
	char buff[4096];
	int n;

	if ( (listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {  //����socket 
		printf("creat socket error: %s(errno: %d)\n", strerror(errno), errno);
		return 0;
	}
	//��ʼ��Э���ַ 
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(6666);

	if ( bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1) { //��socket�Ͷ˿ں� 
		printf("bind socket error: %s(errno: %d)\n", strerror(errno), errno);
		return 0;
	}

	if ( listen(listenfd, 10) == -1) {  //�����˿ں� 
		printf("listen socket error: %s(errno: %d)\n", strerror(errno), errno);
		return 0;
	}

	printf("==========waiting for client's request==========\n");
	while(1)
	{
		if ( (connfd = accept(listenfd, (struct sockaddr*)NULL, NULL)) == -1) {  //�������Կͻ��˵��������� 
			printf("accept socket error:%s(errno: %d)\n", strerror(errno), errno);
			continue;
		}
		n = recv(connfd, buff, MAXLINE, 0);  //��socket�ж�ȡ�ַ� 
		buff[n] = '\0';
		printf("recv msg from client: %s\n", buff);
		close(connfd);  //�ر����� 
	}
	close(listenfd);  //�ر�socket 
	return 0;
}
