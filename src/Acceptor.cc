#include"../inc/Acceptor.h"
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/epoll.h>
#include<errno.h>
#include<unistd.h>
#include<strings.h>


//创建一个非阻塞用于监听客户端连接的文件描述符
static int createNonBlockListenFd(){
    int fd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
    return fd;
}

Acceptor::Acceptor(EventLoop* mainLoop, const InetAddress &servAddr)
:_lfd(createNonBlockListenFd())
,_mainLoop(mainLoop)
,_acceptChannel(_lfd,mainLoop)
,_isListening(false)
{
    setReuseAddr();
    setReusePort();
    bind();
}

void Acceptor::setReuseAddr(){
    int on = 1;
    int ret = setsockopt(_lfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    int(ret == -1)
    {
        perror("setReuseAddr");
    }
}

void Acceptor::setReusePort(){
    int on = 1;
    int ret = setsockopt(_lfd, SOL_SOCKET, SO_REUSEPORT, &on, sizeof(on));
    if(ret == -1){
        perror("setReusePort");
    }
}

void Acceptor::bind(){
    int ret = ::bind(_lfd,(struct sockaddr*)_servAddr.getSockAddr(),sizeof(sockaddr_in));
    if(ret == -1){
        perror("bind");
    }
}

void Acceptor::listen(){
    int ret = ::listen(_lfd, 128);
    if(ret == -1){
        perror("listen");
    }
    _acceptChannel.registerReadEvent();         //注册监听文件描述符的读事件
}

void Acceptor::handleRead(){
    InetAddress peerAddr;

    sockaddr_in addr;
    bzero(&addr,sizeof(addr));
    socklen_t len = sizeof(addr);

    int connfd = ::accept4(_lfd, (struct sockaddr*)&addr, &len, SOCK_NONBLOCK | SOCK_CLOEXEC);
    if(connfd > 0){
        peerAddr.setSockAddr(addr);
        _newConnectionCallback(connfd, peerAddr);
    }
    else{
        perror("Acceptor::handleRead");
    }

}