#pragma once
#include<netinet/in.h>
class InetAddress;

class Socket
{
public:
    explicit Socket(int fd):_fd(fd){}
    ~Socket();

    int  fd() const{return _fd;}
    
    void bind(const InetAddress &localAddr);
    void listen();
    int  accept(InetAddress *peerAddr);

    void shutdownWrite();

    void setReuseAddr(bool on);
    void setReusePort(bool on);
    void setKeepAlive(bool on);

private:
    const int _fd;
};