#include"../inc/TcpConnection.h"

TcpConnection::TcpConnection(int fd, EventLoop *loop, InetAddress &localAddr, InetAddress &peerAddr)
:_fd(fd)
,_channel(new Channel(fd, loop))
,_ioLoop(loop)
,_localAddr(localAddr)
,_peerAddr(peerAddr)
,_state(kDisConnected)
{
    _channel->setReadCallback(bind(&TcpConnection::handleRead, this, std::placeholders::_1));
    _channel->setWriteCallback(bind(&TcpConnection::handleWrite, this));
    _channel->setClosseCallback(bind(&TcpConnection::handleClose,this));
}

TcpConnection::~TcpConnection(){

}

void TcpConnection::send(const string &buf){

}

void TcpConnection::sendInLoop(const void* message, size_t len){

}

void TcpConnection::shutdown(){

}

void TcpConnection::shutdownInLoop(){

}

void TcpConnection::establishConnection(){
    setState(kConnected);
    _channel->tieTcpConnecionPtr(shared_from_this());
    _channel->registerReadEvent();
    
    _connectionCallback(shared_from_this());        //该_connectionCallback是TCP对象被创建出来时，注册给该TCP对象的
}

void TcpConnection::destoryConnection(){
    if(_state == kConnected){
        setState(kDisConnected);
        _channel->cancelAllEvents();
    }
    _ioLoop->deleteChannel(_channel.get());
}

void TcpConnection::handleRead(TimeStamp recvTime){

}

void TcpConnection::handleWrite(){

}

void TcpConnection::handleClose(){
    setState(kDisConnected);
    _channel->cancelAllEvents();

    _closeCallback(shared_from_this());
}

