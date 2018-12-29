利用select函数以及fd_set文件描述符集合来来实现多路IO复用，实现一个服务端与多个客户端进行通信

```c
int select(int maxfdp, fd_set *readfds, fd_set *writefds, fd_set *errorfds, struct timeval*timeout); //seect函数

fd_set set;     //文件描述符集合
FD_ZERO(&set);  //将set清零
FD_SET(fd, &set); //将fd加入set
FD_CLR(fd, &set);  //将fd从set中清除
FD_ISSET(fd, &set); //如果fd在set中则真
```
