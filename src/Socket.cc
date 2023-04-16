#include"../inc/Socket.h"
#include"../inc/InetAddress.h"
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<strings.h>
#include<netinet/tcp.h>
#include<netinet/in.h>

Socket::~Socket(){
    ::close(_fd);
}

void Socket::bind(const InetAddress &localAddr){
    ::bind(_fd, (sockaddr*)localAddr.getSockAddr(), sizeof(sockaddr_in));
}

void Socket::listen(){
    ::listen(_fd, 128);
}

int Socket::accept(InetAddress *peerAddr){
    sockaddr_in addr;
    socklen_t len = sizeof(addr);

    bzero(&addr, sizeof(addr));

    int connfd = accept4(_fd, (sockaddr*)&addr, &len, SOCK_NONBLOCK | SOCK_CLOEXEC);
    if(connfd > 0){
        peerAddr->setSockAddr(addr);
    }
    return connfd;
}

void Socket::shutdownWrite(){
    int ret=shutdown(_fd,SHUT_WR);
}

void Socket::setReuseAddr(bool on){
    int option=on?1:0;
    setsockopt(_fd,SOL_SOCKET, SO_REUSEPORT,&option, sizeof(option));
}

void Socket::setReusePort(bool on){
    int option = on ? 1 : 0;
    ::setsockopt(_fd, SOL_SOCKET, SO_REUSEPORT, &option, sizeof option);
}

void Socket::setKeepAlive(bool on){
    int option = on ? 1 : 0;
    ::setsockopt(_fd, SOL_SOCKET, SO_KEEPALIVE, &option, sizeof option);
}