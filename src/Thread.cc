#include"../inc/Thread.h"
#include<semaphore.h>

std::atomic_int Thread::_threadCnt = 0;

Thread::Thread(ThreadFunc func)
:_threadFunc(std::move(func))
,_threadId(0)
,_isRunning(false)
,_isJoined(false)
{}

Thread::~Thread(){
    if(_isRunning && !_isJoined){
        _thread->detach();
    }
}

void Thread::run(){
    _isRunning = true;
    sem_t sem;
    sem_init(&sem, false, 0);

    _thread = std::shared_ptr<std::thread>(new std::thread([&](){
        _threadId = std::this_thread::get_id();
        sem_post(&sem);

        _threadFunc();    
    }));

    sem_wait(&sem);
}

void Thread::join(){
    _isJoined = true;
    _thread->join();
}
