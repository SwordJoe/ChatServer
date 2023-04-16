#pragma once
#include"InetAddress.h"
#include"CallBacks.h"
#include"Buffer.h"
#include"Channel.h"
#include"TimeStamp.h"
#include"EventLoop.h"
#include<memory>
#include<functional>
#include<string>
#include<atomic>


class TcpConnection : public enable_shared_from_this<TcpConnection>
{
public:
    TcpConnection(int fd, EventLoop* loop, InetAddress &loaclAddr, InetAddress &peerAddr);
    ~TcpConnection();

    int fd() const{return _fd;}
    EventLoop* getLoop() const {return _ioLoop;}
    
    void send(const string &buf);
    void shutdown();
    void establishConnection();
    void destoryConnection();

    void setConnectionCallback(const ConnectionCallback &cb){_connectionCallback = cb;}
    void setMessageCallback(const MessageCallback &cb){_messageCallback = cb;}
    void setCloseCallback(const CloseCallback &cb){_closeCallback = cb;}

private:
    enum kState{kDisConnected, kConnected};
    void setState(kState state){ _state = state;}

    void handleRead(TimeStamp recvTime);
    void handleWrite();
    void handleClose();

    void sendInLoop(const void *message, size_t len);
    void shutdownInLoop();

private:
    int _fd;    
    unique_ptr<Channel> _channel;
    EventLoop* _ioLoop;   
    atomic_int _state;

    InetAddress _localAddr;
    InetAddress _peerAddr;

    ConnectionCallback _connectionCallback;
    MessageCallback _messageCallback;
    CloseCallback _closeCallback;

    Buffer _inputBuffer;
    Buffer _outputBuffer;
};