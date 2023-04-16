#pragma once 
#include"EventLoop.h"
#include"Acceptor.h"
#include"TcpConnection.h"
#include"CallBacks.h"
#include"InetAddress.h"
#include<memory>
#include<unordered_map>
#include<atomic>

class TcpServer
{
public:
    TcpServer(EventLoop* mainLoop, InetAddress &listenAddr);
    ~TcpServer();

    void setConnectionCallback(const ConnectionCallback &cb){_connectionCallback = cb;}
    void setMessageCallback(const MessageCallback &cb){_messageCallback = cb;}

    void setThreadsNum(int threadsNum){ _eventLoopThreadPool->setThreadsNum(threadsNum);}
    void run();

private:
    void newConnection(int fd, InetAddress &peerAddr);
    void removeConnection(const TcpConnectionPtr &conn);
    void removeConnectionInLoop(const TcpConnectionPtr &conn);

private:
    EventLoop* _mainLoop;
    unique_ptr<Acceptor> _acceptor;
    InetAddress _localAddress;
    std::string _ipPort;

    shared_ptr<EventLoopThreadPool> _eventLoopThreadPool;
    
    std::atomic_bool isRunning;
    int _nextTcpConnId;

    ConnectionCallback _connectionCallback;
    MessageCallback  _messageCallback;
    std::unordered_map<int, TcpConnectionPtr> _connections;
};