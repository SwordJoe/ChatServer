#include"../inc/EventLoop.h"
#include<memory>
#include<thread>
#include<sys/eventfd.h>
#include<unistd.h>
#include<fcntl.h>
#include<strings.h>

__thread EventLoop* _loopInThisThread = nullptr;

EventLoop::EventLoop()
:_epollfd(epoll_create1(EPOLL_CLOEXEC))
,_eventfd(::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC))
,_looping(false)
,_quit(false)
,_callingPendingFunctors(false)
,_eventfdChannel(new Channel(_eventfd, this))
,_threadId(std::this_thread::get_id())
{
    _loopInThisThread = this;

    
    _eventfdChannel->registerReadEvent();       //注册_eventfd的读事件
    _eventfdChannel->setReadCallback(bind(&EventLoop::handleRead, this));       //设置_eventfd的读事件回调函数
}

EventLoop::~EventLoop(){
    _eventfdChannel->cancelAllEvents();
    deleteChannel(_eventfdChannel.get());
    //::close(_eventfd);      //文件描述符应该在哪里关闭? 在Channel类的析构函数中关闭_fd
    _loopInThisThread = nullptr;
}

void EventLoop::loop(){
    _looping = true;
    _quit = false;

    while(_quit == false){
        _activeChannels.clear();        //每次epoll_wait()监听之前都先清空活跃Channel列表，epoll_wait()返回的时候填充活跃Channel列表
        int eventsNum = epoll_wait(_epollfd, &*_events.begin(), _events.size(), -1);
        TimeStamp nowTime(TimeStamp::now());

        if(eventsNum > 0){
            for(int i=0; i<eventsNum; ++i){
                Channel* channel = static_cast<Channel*>(_events[i].data.ptr);
                channel->setEvent(_events[i].events);
                _activeChannels.push_back(channel);
            }
            if(eventsNum ==_events.size()){     //需要扩容
                _events.resize(_events.size() * 2);
            }
        }
        else if(eventsNum == 0){

        }
        else{
            perror("epoll_wait");
        }

        for(auto channel : _activeChannels){
            channel->handleEvent(nowTime);
        }

        _looping = false;
    }
}

void EventLoop::quitLoop(){
    _quit = true;
    if( _threadId != std::this_thread::get_id()){
        wakeupLoop();
    }
}

void EventLoop::runInLoop(Functor functor){
    if( _threadId == std::this_thread::get_id()){
        functor();
    }
    else{
        queueInLoop(functor);
    }
}

void EventLoop::queueInLoop(Functor functor){
    {
        unique_lock<mutex> lock(_mutex);
        _pendingFunctors.emplace_back(functor);
    }
    if(_threadId != std::this_thread::get_id() || _callingPendingFunctors){
        wakeupLoop();
    }
}

void EventLoop::wakeupLoop(){
    uint64_t var = 1;
    ssize_t n = read(_eventfd, &var, sizeof(var));
    if( n == -1){
        perror("wakeupLoop");
    }
}

void EventLoop::updateChannelEvent(Channel* channel){
    bool isChannelInEpollWait = channel->isChannelInEpollWait();
    
    if(!isChannelInEpollWait){   //channel还未被添加到Loop中
        _allChannels[channel->fd()] = channel;  
        channel->setChannelInEpollWaitState(true);
        update(channel, EPOLL_CTL_ADD);
    }
    else{   //channel已经被添加到Loop中
        if(channel->isNonEvent()){      //如果Channel对读写事件都不关心，直接删除
            update(channel, EPOLL_CTL_DEL);
            channel->setChannelInEpollWaitState(false);
        }
        else{
            update(channel,EPOLL_CTL_MOD);
        }
    }
}

void EventLoop::deleteChannelFromChannelMap(Channel* channel){
    _allChannels.erase(channel->fd());  //先将channel从ChannelMap中删除
    
    if(channel->isChannelInEpollWait() == true){   //如果channel中的fd有被epoll监听，将其从epoll监听队列删除
        update(channel, EPOLL_CTL_DEL);
    }
    channel->setChannelInEpollWaitState(false);
}

void EventLoop::update(Channel* channel, int operation){
    epoll_event event;
    bzero(&event, sizeof(event));

    int fd = channel->fd();
    event.events = channel->event();
    event.data.fd = fd;
    event.data.ptr = channel;

    int ret = ::epoll_ctl(_epollfd, operation, fd, &event);
    if(ret<0){
        perror("EventLoop::update epoll_ctl");
    }
}