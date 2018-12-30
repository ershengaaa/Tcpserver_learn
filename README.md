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

# select、poll及epoll的区别

select、poll、epoll三者都是多路IO复用的机制。多路IO复用就通过一种机制，可以监视多个描述符，一旦某个描述符就绪（一般是读或写就绪），能够通知程序进行相应的读写操作，但三者本质上仍是`同步IO`。异步IO不需要自己进行读、写处理，异步IO的实现会负责把数据从内核拷贝到用户空间


- select与poll-->一般认为poll()比select()高级

>1.poll()不要求开发者在计算最大文件描述符时进行+1的操作
>
>2.poll()在应对大数目的文件描述符的时候速度更快，因为对于select()来说内核需要检查大量描述符对应的fd_set中的每一位比特位，比较费事
>
>3.select()可以监控的文件描述符数目是固定的，相对来说也较少。而对于poll来说，可以创建特定大小的数组来保存监控的描述符，而不受文件描述符值大小的影响，而且poll可以监控的文件数目远大于select()
>
>4.对于select来说，其监控的fd_set在select返回后会变化，所以在下一次进入select()之前都需要重新初始化
>
>5.每次在超时后在下一次进入到select()之前都需要重新设定

- select()的优点

>1.select()的可移植性更好
>2.select()对于超时值提供了更好的精度

- epoll的优点

>1.支持一个进程打开大数目的socket描述符(FD)
>2.IO效率不随FD数目增加而下降
>3.使用mmap加速内核与用户空间的消息传递
