# Tcpserver

这里用来记录个人的tcp网络编程学习的一个过程，主要将平时写的一些代码放入其中，也是因为学Tcp网络编程也有段时间了,应该做一个小小的总结

Tcpserver编写的一个大致流程(以epoll函数处理服务端与客户端为例)：

```c
//常用头文件
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <poll.h>
#include <sys/epoll.h>

/* 创建套接字并绑定 */
int socket_and_bind(const char* ip, int port);
/* IO多路复用处理函数 */
void do_epoll(int listenfd);
/* 事件处理函数 */
void handle_events(int epollfd, struct epoll_event *events,int num, int listenfd, char *buf); 
/* 处理收到的连接 */
void handle_acept(int epollfd, int listenfd);
/* 读处理 */
void do_read(int epollfd, int fd, char *buf);
/* 写处理 */
void_write(int epollfd, int fd, char *buf);
/* 添加事件 */
void add_event(int epollfd, int fd, int state);
/* 修改事件 */
void modify_event(int epollfd, int fd, int state);
/* 删除事件 */
void delete_event(int epollfd, int fd, int state);
```

其实就是基于Tcp交互流程，加入select或poll或epoll来提高服务器的处理能力