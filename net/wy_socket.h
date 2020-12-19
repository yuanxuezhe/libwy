#ifndef WY_SOCKET_H 
#define WY_SOCKET_H 

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "../base/wy_comm.h"

struct tcp_packet {
    int len;
    char buf[1024];
};

ssize_t readn(int fd, void *buf, size_t count)
{
    size_t nleft = count;
    ssize_t nread;
    char *bufp = (char*)buf;
    while (nleft > 0)
    {
        if ((nread = read(fd, bufp, nleft)) < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }
            return 1;
        }
        else if (nread == 0)
        {
            return count - nleft;
        }
        bufp += nread;
        nleft -= nread;
    }

    return count;
}

ssize_t writen(int fd, const void *buf, size_t count)
{
    size_t nleft = count;
    ssize_t nwrite;
    char *bufp = (char*)buf;
    while (nleft > 0)
    {
        if ((nwrite = write(fd, bufp, nleft)) < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }
            return 1;
        }
        else if (nwrite == 0)
        {
            continue;
        }
        bufp += nwrite;
        nleft -= nwrite;
    }

    return count;
}

/**
 * read_timeout 读超时检测函数，不包含读操作
 * @fd: 文件描述符
 * @wait_seconds: 等待超时秒数，如果为0标识不检测超时
 * 成功（未超时）返回0，失败返回-1并且error = ETIMEDOUT
 */
int read_timeout(int fd, unsigned int wait_seconds)
{
    int ret = 0;
    if (wait_seconds >0)
    {
        fd_set read_fdset;
        struct timeval timeout;

        FD_ZERO(&read_fdset);
        FD_SET(fd, &read_fdset);

        timeout.tv_sec = wait_seconds;
        timeout.tv_usec = 0;

        do
        {
            ret = select(fd+1, &read_fdset, NULL, NULL, &timeout);
        } while (ret < 0 && errno == EINTR);
        
        if (ret == 0)
        {
            ret = -1;
            errno = ETIMEDOUT;
        }
        else if (ret == 1)
        {
            ret = 0;
        }
    }
    return ret; 
}

/**
 * write_timeout 写超时检测函数，不包含写操作
 * @fd: 文件描述符
 * @wait_seconds: 等待超时秒数，如果为0标识不检测超时
 * 成功（未超时）返回0，失败返回-1并且error = ETIMEDOUT
 */
int write_timeout(int fd, unsigned int wait_seconds)
{
    int ret = 0;
    if (wait_seconds >0)
    {
        fd_set write_fdset;
        struct timeval timeout;

        FD_ZERO(&write_fdset);
        FD_SET(fd, &write_fdset);

        timeout.tv_sec = wait_seconds;
        timeout.tv_usec = 0;

        do
        {
            ret = select(fd+1, NULL, &write_fdset, NULL, &timeout);
        } while (ret < 0 && errno == EINTR);
        
        if (ret == 0)
        {
            ret = -1;
            errno = ETIMEDOUT;
        }
        else if (ret == 1)
        {
            ret = 0;
        }
    }
    return ret; 
}

/**
 * accept_timeout 带超时的accept
 * @fd: 套接字
 * @addr: 输出参数，返回对方地址
 * @wait_seconds: 等待超时秒数，如果为0标识不检测超时
 * 成功（未超时）返回0，失败返回-1并且error = ETIMEDOUT
 */
int accept_timeout(int fd, struct sockaddr_in *addr, unsigned int wait_seconds)
{
    int ret = 0;
    socklen_t addrlen = sizeof(struct sockaddr_in);

    if (wait_seconds >0)
    {
        /*
         * 说明：accept和connect都会阻塞进程，accept的本质是从listen的队列中读一个连接，是一个读事件
         * 三次握手机制是由TCP/IP协议实现的，并不是由connect函数实现的，connect函数只是发起一个连接，
         * connect并非读写事件,所以只能设置connect非阻塞，而用select监测写事件（读事件必须由对方先发送报文，时间太长了）
         * 所以accept可以由select管理
         * 强调：服务端套接字是被动套接字，实际上只有读事件
         * */
        fd_set accept_fdset;
        struct timeval timeout;

        FD_ZERO(&accept_fdset);
        FD_SET(fd, &accept_fdset);

        timeout.tv_sec = wait_seconds;
        timeout.tv_usec = 0;

        do
        {
            ret = select(fd+1, &accept_fdset, NULL, NULL, &timeout);
        } while (ret < 0 && errno == EINTR);
        
        if (ret == 0)
        {
            ret = -1;
            errno = ETIMEDOUT;
            return ret;
        }
        else if (ret == -1)
        {
            return ret;
        }

        //成功无需处理，直接往下执行
    }

    //一旦检测出select有事件发生，表示有三次握手成功的客户端连接到来了
    //此时调用accept不会被阻塞
    if (addr != NULL)
    {
        ret = accept(fd, (struct sockaddr*)addr, &addrlen);
    }
    else
    {
        ret = accept(fd, NULL,NULL);
    }

    if (ret == -1)
    {
        ERR_EXIT("accept_timeout");
    }
    
    return ret; 
}

/**
 * activate_nonblock - 设置套接字非阻塞
 * @fd：文件描述符
 * 成功返回0，失败返回-1
 * */
int activate_nonblock(int fd)
{
    int ret = 0;
    int flags = fcntl(fd, F_GETFL);
    if (flags == -1)
        return -1;
    flags = flags | O_NONBLOCK;
    ret = fcntl(fd, F_SETFL, flags);
    if (ret == -1)
        return -1;
    return ret;
}

/**
 * deactivate_nonblock - 设置套接字阻塞
 * @fd：文件描述符
 * 成功返回0，失败返回-1
 * */
int deactivate_nonblock(int fd)
{
    int ret = 0;
    int flags = fcntl(fd, F_GETFL);
    if (flags == -1)
        return -1;
    flags = flags & (~O_NONBLOCK);
    ret = fcntl(fd, F_SETFL, flags); 
    if (ret == -1)
        return -1;
    return ret;
}

/**
 * connect_timeout - 带超时的connect(方法中已执行connect)
 * @fd：文件描述符
 * @addr：地址结构体指针
 * @wait_seconds：等待超时秒数，如果为0表示不检测超时
 * 成功返回0.失败返回-1，超时返回-1并且errno = ETIMEDOUT
 * */
int connect_timeout(int fd, struct sockaddr_in *addr, unsigned int wait_seconds)
{
    int ret = 0;
    //connect()函数是连接服务器，本来connect会阻塞，但是设置未阻塞之后，
    //客户端仍然会三次握手机制，如果三次握手失败，那么客户端一定无法向文件描述符中写入数据
    //如果连接成功，那么客户端就可以向文件描述符写入数据了，
    //所以交给select监管的文件描述符如果可以写，说明连接成功，不可以写说明连接失败

    //设置当前文件描述符未阻塞--设置非阻塞之后，
    //connect在网络中非常耗时，所以需要设置成非阻塞，如果有读事件，说明可能连接成功
    //这样有利于做超时限制
    if (wait_seconds > 0)
    {
        if (activate_nonblock(fd) == -1)
            return -1;
    }
    ret = connect(fd, (struct sockaddr *) addr, sizeof(struct sockaddr));
    if (ret == -1 && errno == EINPROGRESS)
    {
        fd_set connect_fdset;
        FD_ZERO(&connect_fdset);
        FD_SET(fd, &connect_fdset);
        struct timeval timeout;
        timeout.tv_sec = wait_seconds;
        timeout.tv_usec = 0;
        do
        {
            ret = select(fd + 1, NULL, &connect_fdset, NULL, &timeout);
        } while (ret == -1 && errno == EINTR);
        //ret==-1 不需要处理，正好给ret赋值
        //select()报错,但是此时不能退出当前connect_timeout()函数
        //因为还需要取消文件描述符的非阻塞
        if (ret == 0)
        {
            errno = ETIMEDOUT;
            ret = -1;
        }
        else if (ret == 1)
        {
            //ret返回为1（表示套接字可写），可能有两种情况，一种是连接建立成功，一种是套接字产生错误，
            //此时错误信息不会保存至errno变量中，因此，需要调用getsockopt来获取。
            int err = 0;
            socklen_t len = sizeof(err);
            ret = getsockopt(fd, SOL_SOCKET, SO_ERROR, &err, &len);
            if (ret == 0 && err != 0)
            {
                errno = err;
                ret = -1;
            }
            //说明套接字没有发生错误，成功
        }
    }
    if (wait_seconds > 0)
    {
        if (deactivate_nonblock(fd) == -1)
            return -1;
    }
    return ret;
}


char* msgdirection(int sock)
{
    char128 p;
    char* a = p;
    struct sockaddr_in addr;
    socklen_t addrlen;
    addrlen = sizeof(addr);
    int n = 0;
    if (!getpeername(sock, (struct sockaddr *)&addr, &addrlen))
    {
        n = snprintf(a, sizeof(p),"Msg direction:[%s:%d]", inet_ntoa(addr.sin_addr),  ntohs(addr.sin_port));
    }

    if (!getsockname(sock, (struct sockaddr *)&addr, &addrlen))
    {
        snprintf(a + n, sizeof(p) - n," ==> [%s:%d]", inet_ntoa(addr.sin_addr),  ntohs(addr.sin_port));
    }

    return a;
}

#endif	// WY_SOCKET_H