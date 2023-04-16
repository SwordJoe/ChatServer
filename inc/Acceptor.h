#pragma once
#include"InetAddress.h"
#include"Channel.h"
#include<functional>

class EventLoop;

class Acceptor
{
    using NewConnectionCallback = std::function<void(int fd, InetAddress &addr)>;
public: 
    Acceptor(EventLoop* mainLoop, const InetAddress &servAddr);
    ~Acceptor();

private:
    void setReuseAddr();
    void setReusePort();
    void bind();
    void listen();
    void setNewConnectionCallback(const NewConnectionCallback &cb){
        _newConnectionCallback = cb;
    }

private:
    void handleRead();

private:
    int _lfd;
    EventLoop* _mainLoop;

    Channel _acceptChannel;
    NewConnectionCallback _newConnectionCallback;
    bool _isListening;
    InetAddress _servAddr;
};