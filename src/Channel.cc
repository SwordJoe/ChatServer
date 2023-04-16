#include"../inc/Channel.h"
#include"../inc/EventLoop.h"
#include<unistd.h>

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = EPOLLIN | EPOLLPRI;
const int Channel::kWriteEvent = EPOLLOUT;

Channel::Channel(int fd, EventLoop* loop)
:_loop(loop)
,_fd(fd)
,_event(kNoneEvent)
,_returnEvent(kNoneEvent)
,_isChannelInEpollWait(false)
,_isTied(false)
{}

Channel::~Channel(){
    ::close(_fd);
}

void Channel::tieTcpConnecionPtr(const shared_ptr<void> &obj){
    _tiedPtr = obj;
    _isTied = true;
}

//注册读事件
void Channel::registerReadEvent(){      
    _event |= kReadEvent;           
    _loop->updateChannelEvent(this);    
}

//关闭读事件
void Channel::cancelReadEvent(){
    _event &= ~kReadEvent;
    _loop->updateChannelEvent(this);
}

//注册写事件
void Channel::registerWirteEvent(){
    _event |= kWriteEvent;
    _loop->updateChannelEvent(this);
}

//关闭写事件
void Channel::cancelWriteEvent(){
    _event &= ~kWriteEvent;
    _loop->updateChannelEvent(this);
}

//关闭所有事件
void Channel::cancelAllEvents(){
    _event = kNoneEvent;
    _loop->updateChannelEvent(this);
}

void Channel::handleEvent(TimeStamp recvTime){
    if(_isTied){        //判断是否有和TcpConnectionPtr进行绑定
        shared_ptr<void> guard = _tiedPtr.lock();
        if(guard){
            handleEventWithGuard(recvTime);
        }
    }
    else{
        handleEventWithGuard(recvTime);
    }
}

void Channel::handleEventWithGuard(TimeStamp recvTime){
    if(_returnEvent & (EPOLLIN | EPOLLPRI)){
        if(_readCallback){
            _readCallback(recvTime);
        }
    }
    if( _returnEvent & EPOLLOUT){
        if(_writeCallback){
            _writeCallback();
        }
    }
    if((_returnEvent & EPOLLHUP) && !(_returnEvent & EPOLLIN)){
        if(_closeCallback){
            _closeCallback();
        }
    }
    
    if((_returnEvent & EPOLLHUP) && !(_returnEvent & EPOLLIN)){

    }
}