#include"../inc/TcpServer.h"
#include"../inc/TcpConnection.h"
#include"../inc/EventLoopThreadPool.h"
#include<cstring>

TcpServer::TcpServer(EventLoop* mainLoop, InetAddress &localAddr)
:_mainLoop(mainLoop)
,_acceptor(new Acceptor(mainLoop, listenAddr))
,_localAddress(localAddress)
,_ipPort(listenAddr.toIpPort())
,_eventLoopthreadPool(new EventLoopThreadPool(loop))
,_nextTcpConnId(1)
,_isRunning(false)
{
    _acceptor->setNewConnectionCallback(bind(&TcpServer::newConnection,this,std::placeholders::_1, std::placeholders::_2));
}

TcpServer::~TcpServer(){
    for(auto &item : _connections){
        TcpConnectionPtr conn(item.second); 
        item.second.reset();

        conn->getLoop()->runInLoop(&bind(TcpConnection::destoryConnection,conn));
    }
}

void TcpServer::run(){
    if(!_isRunning){
        _eventLoopThreadPool->run();

        _mainLoop->runInLoop(bind(&Acceptor::listen,_acceptor.get()));
        _mainLoop->loop();
    }
}

void TcpServer::newConnection(int fd, InetAddress &peerAddr){
    EventLoop* ioLoop = _eventLoopThreadPool->getNextLoop();

    TcpConnectionPtr conn(new TcpConnection(fd, ioLoop, loaclAddr, peerAddr));
    _connections[fd] = conn;

    conn->setConnectionCallback(_connectionCallback);
    conn->setMessageCallback(_messageCallback);
    conn->setCloseCallback(bind(&TcpServer::removeConnection, this, _1));

    //将该fd注册到epoll中
    ioLoop->runInLoop(bind(&TcpConnection::establishConnection,conn));
}

void TcpServer::removeConnection(const TcpConnectionPtr &conn){
    _mainLoop->runInLoop(bind(&TcpServer::removeConnectionInLoop, this, conn));
}

void TcpServer::removeConnectionInLoop(const TcpConnectionPtr &conn){
    _connections.erase(conn->fd());
    EventLoop *ioLoop = conn->getLoop();
    ioLoop->queueInLoop(bind(&TcpConnection::destoryConnection,conn));
}
