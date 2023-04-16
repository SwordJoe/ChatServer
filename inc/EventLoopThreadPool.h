#pragma once
#include"Thread.h"
#include<mutex>
#include<condition_variable>

class EventLoop;

class EventLoopThread
{
public:
    EventLoopThread();
    ~EventLoopThread();
    EventLoop* startLoop();


private:
    void threadFunc();

private:
    EventLoop* _loop;
    Thread _thread;    
    bool _isExiting;
    std::mutex _mutex;
    std::condition_variable _cond;
};


class EventLoopThreadPool
{
public:
    EventLoopThreadPool(EventLoop* mainLoop);
    ~EventLoopThreadPool() = default;

    void setThreadNum(int num){ _threadNum = num;}
    void run();
    bool isRunning() const { return _isRunning;}
    EventLoop* getNextLoop();

private:
    EventLoop* _mainLoop;
    bool _isRunning;
    int _threadsNum;
    int _next;
    std::vector<std::unique_ptr<EventLoopThread>> _threads;
    std::vector<EventLoop*> _loops;

};