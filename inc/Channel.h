#pragma once 
#include"TimeStamp.h"
#include"CallBacks.h"
#include<memory>
#include<sys/epoll.h>

class EventLoop;

class Channel
{
public:
    Channel(int fd, EventLoop* loop);
    ~Channel();

    void handleEvent(TimeStamp recvTime);   //处理事件
    void setReadCallback(ReadEventCallback cb){ _readCallback = std::move(cb);}
    void setWriteCallback(EventCallback cb){ _writeCallback = std::move(cb);}
    void setClosseCallback(EventCallback cb){ _closeCallback = std::move(cb);}

    int fd() const{return _fd;}
    int event() const{return _event;}
    int setEvent(int event){_event = event;}
    bool isNonEvent() const{return _event & kNoneEvent;}
    bool isReadEvent() const{return _event & kReadEvent;}
    bool isWriteEvent() const{return _event & kWriteEvent;}
    bool isChannelInEpollWait() const {return _isChannelInEpollWait;}
    void setChannelInEpollWaitState(bool state) { _isChannelInEpollWait = state;}
    EventLoop* onwerLoop(){ return _loop;}
    void removeFromLoop();
    void tieTcpConnecionPtr(const shared_ptr<void> &obj);

    void registerReadEvent();
    void cancelReadEvent();
    void registerWirteEvent();
    void cancelWriteEvent();
    void cancelAllEvents();

private:
    void handleEventWithGuard(TimeStamp recvTime);  //处理文件描述符上的事件
    //enum isChannelInLoop{ kChannelNotInLoop, kChannelInLoop};

private:
    EventLoop* _loop;
    const int _fd;
    int _event;                         //Channel关注的事件
    int _returnEvent;                   //epoll_wait()返回的Channel的实际事件
    bool _isChannelInEpollWait;         //Channel是否在Epoll队列中被监听
    bool _isTied;                       //是否与TcpConnection指针绑定
    weak_ptr<void> _tiedPtr;

    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;

    ReadEventCallback _readCallback;
    EventCallback _writeCallback;
    EventCallback _closeCallback;
};