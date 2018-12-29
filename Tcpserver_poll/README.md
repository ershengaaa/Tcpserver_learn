与select函数功能类似，也可实现多路IO复用，在这里服务端使用poll函数处理多个客户端发送的消息；客户端也使用了poll函数，用来判断是否有数据可读

```c
//poll函数
#include <poll.h>
int poll(struct pollfd *fds, unsigned int nfds, int timeout);
//pollfd结构体
struct pollfd {
	int fd;   //文件描述符
	short events;  //等待的事件
	short revents;  //实际发生了的事件
};
```

