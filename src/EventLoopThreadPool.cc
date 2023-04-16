#include"../inc/EventLoopThread.h"
#include"../inc/EventLoop.h"

EventLoopThread::EventLoopThread()
:_loop(nullptr)
,_thread()
,_isExiting(false)
,_mutex()
,_cond()
{}

EventLoopThread::~EventLoopThread(){
    _isExiting = true;
    if(_loop != nullptr){
        _loop->quitLoop();
        _thread->join();
    }
}

EventLoop* EventLoopThread::startLoop(){
    _thread.run();

    EventLoop* loop = nullptr;
    {
        std::unique_lock<std::mutex> lock(_mutex);
        while(_loop == nullptr){
            _cond.wait(lock);
        }
        loop = _loop;
    }
    return loop;
}

//下面的代码都在子线程中执行
void EventLoopThread::threadFunc(){
    EventLoop loop; //在子线程中生成一个栈变量，利用_cond通知主线程，将栈变量返回给主线程
    {
        std::unique_lock<std::mutex> lock(_mutex);
        _loop = &loop;
        _cond.notify_one();
    }

    loop.loop();        //在子线程中开始epoll_wait()循环
    std::unique_lock<mutex> lock(_mutex);
    _loop = nullptr;
}


EventLoopThreadPool::EventLoopThreadPool(EventLoop* mainLoop)
:_mainLoop(mainLoop)
,_isRunning(false)
,_threadsNum(0)
,_next(0)
{}

void EventLoopThreadPool::run(){
    _isRunning = true;

    for(int i=0; i<_threasNum; ++i){
        EventLoopThread* th = new EventLoopThread();
        _threads.push_back(std::unique_ptr<EventLoopThread>(th));
        _loops.push_back(th->startLoop());
    }
}

EventLoop* getNextLoop(){
    EventLoop* tmpLoop = _mainLoop;

    if(!_loops.empty()){
        tmpLoop = _loops[_next];
        _next = (_next+1)%_threadsNum;
    }

    return tmpLoop;
}