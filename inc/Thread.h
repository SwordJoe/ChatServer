#pragma once 
#include<thread>
#include<memory>
#include<functional>
#include<atomic>

class Thread
{
    using ThreadFunc = std::function<void()>;
public:
    Thread(ThreadFunc func);
    ~Thread();

    void run();
    void join();
    bool isRunning(){return _isRunning;}

private:
    std::shared_ptr<std::thread> _thread;
    std::thread::id _threadId;
    bool _isRunning;
    bool _isJoined;
    ThreadFunc _threadFunc;
    static std::atomic_int _threadCnt; 
}