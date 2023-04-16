#pragma once
#include"TimeStamp.h"
#include"Channel.h"
#include<vector>
#include<unordered_map>
#include<functional>
#include<mutex>
#include<atomic>
#include<sys/epoll.h>

using Functor = std::function<void()>;
using ChannelMap = std::unordered_map<int, Channel*>;   
using ChannelList = std::vector<Channel*>;

class EventLoop
{
public:
    EventLoop();
    ~EventLoop();

    void loop();
    void quitLoop();
    void runInLoop(Functor functor);
    void queueInLoop(Functor functor);
    void wakeupLoop();

    void updateChannelEvent(Channel* channel);
    void deleteChannelFromChannelMap(Channel* channel);

    bool isInLoopThread() const { return _threadId == std::this_thread::get_id();}

private:
    void handleRead();              //处理_eventfd上的读事件
    void doPendingFunctors();      
    void update(Channel* channel, int operation);

private:
    int _epollfd;
    int _eventfd;

    std::thread::id _threadId;
    TimeStamp _epollReturnTime;
    unique_ptr<Channel> _eventfdChannel;

    std::atomic_bool _looping;
    std::atomic_bool _quit;
    std::atomic_bool _callingPendingFunctors;
    
    ChannelMap  _allChannels;                    //所有文件描述符
    ChannelList _activeChannels;                //活跃的文件描述符，只在epoll_wait处使用，并且每次使用完要清空
    std::vector<epoll_event> _events;           //活跃的事件
    std::vector<Functor>  _pendingFunctors;     //待执行的函数
    std::mutex _mutex;
};