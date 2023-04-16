#include"../inc/InetAddress.h"
#include<string.h>

InetAddress::InetAddress(uint16_t port, string ip){
    bzero(&_addr, sizeof(_addr));
    _addr.sin_family = AF_INET;
    _addr.sin_port = htons(port);
    _addr.sin_addr.s_addr = inet_addr(ip.c_str());
}

string InetAddress::toIp() const{
    char buf[64] = {0};
    inet_ntop(AF_INET, &_addr.sin_addr, buf, sizeof(buf));
    return buf;
}

string InetAddress::toIpPort() const{
    string str = toIp();
    uint16_t port = ntohs(_addr.sin_port);
    str = str + " : " + to_string(port);
    return str;
}

uint16_t InetAddress::toPort() const{
    return ntohs(_addr.sin_port);
}