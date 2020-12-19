#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>          /* See NOTES */ 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "../base/wy_err.h"
#include <string.h>
#include <errno.h>
#include "wy_socket.h"
#include <vector>
#include <sys/epoll.h>

using namespace std;

typedef vector<struct epoll_event> EventList;

int main()
{
    int listenfd;
    int on = 1;
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
       ERR_EXIT("socket!");
    }
    if ((setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR,  &on,  sizeof(on))) < 0)
    {
        ERR_EXIT("setsockopt");
    }

    sockaddr_in addr_in;
    memset(&addr_in, 0, sizeof(addr_in));
    addr_in.sin_family = AF_INET;
    addr_in.sin_port = htons(9158);
    addr_in.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(listenfd, (struct sockaddr *)&addr_in, sizeof(addr_in)) < 0)
    {
        ERR_EXIT("bind!");
    }

    if (listen(listenfd, SOMAXCONN) < 0)
    {
        ERR_EXIT("listen!");
    }

    int epollfd;
    epollfd = epoll_create1(EPOLL_CLOEXEC);

    struct epoll_event event;
    EventList events(16);
    event.data.fd = listenfd;
    event.events = EPOLLIN | EPOLLET;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &event);

    char recvbuf[1024];
    struct sockaddr_in peeraddr;
    socklen_t peerlen;
    int conn;

    int nready;
    while (1)
    {
        nready =   (epollfd, &*events.begin(), events.size(), -1);

        if (nready == -1)
        {
            if (errno == EINTR)
            {
                continue;
            }
            
            ERR_EXIT("epoll_wait");
        }

        if (nready == 0)
        {
            continue;
        }

        if ((size_t)nready == events.size())
        {
            events.resize(events.size() * 2);
        }
        
        for (size_t i = 0; i < nready; i++)
        {
            if (events[i].data.fd == listenfd)
            {
                peerlen = sizeof(peeraddr);
                conn = accept(listenfd, (struct sockaddr*)&peeraddr, &peerlen);
                if (conn == -1)
                {
                    printf("Accept Conn error!");
                    continue;
                }

                printf("ip:%s port:%d \n", inet_ntoa(peeraddr.sin_addr),  ntohs(peeraddr.sin_port));
                //clients.push_back(conn);
                
                activate_nonblock(conn);
                event.data.fd = conn;
                event.events = EPOLLIN | EPOLLET;
                epoll_ctl(epollfd, EPOLL_CTL_ADD, conn, &event);
            }
            else if (events[i].events & EPOLLIN)
            {
                conn = events[i].data.fd;
                if (conn < 0)
                {
                    continue;
                }
                
                memset(recvbuf, 0, sizeof(recvbuf));
                int ret = readn(conn, recvbuf, sizeof(recvbuf));
                if (ret == 0)
                {
                    printf("client close \n");
                    close(conn);
                    event = events[i];
                    epoll_ctl(epollfd, EPOLL_CTL_DEL, conn, &event);
                    continue;
                }
                else if (ret == -1)
                {       
                    ERR_EXIT("read");
                }  

                printf("%s\n",msgdirection(conn));
                
                fputs(recvbuf, stdout);

                writen(conn, recvbuf, sizeof(recvbuf));
            }  
        }
    }
     
    return 0;
}