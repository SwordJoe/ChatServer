#pragma once
#include<memory>
#include<functional>
using namespace std;

class Buffer;
class TcpConnection;
class TimeStamp;

using TcpConnectionPtr = shared_ptr<TcpConnection>;
using ConnectionCallback = function<void(const TcpConnectionPtr&)>;
using CloseCallback = function<void(const TcpConnectionPtr&)>;
using MessageCallback = function<void(const TcpConnectionPtr&, Buffer*, TimeStamp)>;

using EventCallback = function<void()>;
using ReadEventCallback = function<void(TimeStamp)>;