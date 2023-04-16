#include"../inc/Buffer.h"
#include<errno.h>
#include<sys/uio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<iostream>
using namespace std;

size_t Buffer::readFd(int fd, int* saveErrno){
    char extraBuf[65536] = {0};     //64K
    struct iovec vec[32];

    size_t writable = writeableBytes();
    vec[0].iov_base = begin() + _writeIndex;    //第一次读到_buffer中
    vec[0].iov_len = writable;

    vec[1].iov_base = extraBuf;             //剩余部分读取到extraBuf中
    vec[1].iov_len = sizeof(extraBuf);

    const int iovcnt = (writable < sizeof(extraBuf) ? 2 : 1);
    size_t n = ::readv(fd, vec, iovcnt);
    if(n < 0){
        *saveErrno = errno;
    }
    else if( n < writable){     //如果从客户端读取到的字节数比可写入数据区域字节数少，就直接将可写标志位往后移动
        _writeIndex += n;
    }
    else{       //如果从客户端读取到的字节数比可写入数据区域字节数要大，将extraBuf追加到buffer缓冲区后面，需要对缓冲区扩容
        _writeIndex = _buffer.size();      
        append(extraBuf, n - writable);         //将extraBuf中n-writable个字节拷贝到_buffer中
    }
    return n;
}

// size_t Buffer::readFd(int fd, int *saveErrno){
//     size_t writeable = writeableBytes();                    //当前缓冲区可写字节数
//     size_t n = ::recv(fd, beginWrite(), INT64_MAX, MSG_PEEK);    //先窥探TCP缓冲区中的数据长度
//     if(n>writeable){        //如果TCP缓冲区的数据长度大于可写数据长度,扩容
//         makeSpace(n);
//     }
//     n = ::recv(fd, beginWrite(), n,0);             //进行真正的读数据
//     _writeIndex += n;                                       //可写标志位后移
//     cout<<"读取"<<n<<"字节"<<endl;
//     return n;
// }



size_t Buffer::writeFd(int fd, int* saveErrno){
    size_t n = ::write(fd, peek(), readableBytes() );       //将outputBuffer中的数据有多少发多少出去，尽量都发送完毕
    if(n < 0){
        *saveErrno = errno;
    }
    return n;
}