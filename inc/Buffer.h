#pragma once
#include<vector>
#include<string>
#include<algorithm>

class Buffer
{
    //static const size_t kReserved = 8;
    static const size_t kInitial = 1024;
public:
    Buffer(size_t initialSpace = kInitial):_buffer(kInitial),_readIndex(0),_writeIndex(0){}
    
    size_t readableBytes() const { return _writeIndex - _readIndex;}
    size_t writeableBytes() const { return _buffer.size() - _writeIndex;}
    
    void retrieve(size_t len){
        if(readableBytes() > len){
            _readIndex += len;
        }
        else{
            retrieveAll();
        }
    }

    void retrieveAll(){
        _readIndex = _writeIndex = 0;
    }

    std::string retrieveAsString(size_t len){
        std::string str(&*_buffer.begin()+_readIndex, len);
        retrieve(len);
        return str;
    }

    void retrieveAllAsString(){
        return retrieveAsString(readableBytes());
    }

    void append(const char *data, size_t len){
        if(writeableBytes() < len){
            expandSpace(len);
        }
        copy(data, data+len, &*_buffer.begin()+_writeIndex);
        _writeIndex += len;
    }

    void expandSpace(size_t len){
        if(writeableBytes() + _readIndex < len){
            _buffer.resize(_writeIndex + len);
        }
        else{
            size_t readable = readableBytes();
            copy(&*buffer.begin()+_readIndex, &*buffer.begin()+_writeIndex, &*buffer.begin());
            _readIndex = 0;
            _writeIndex = _readIndex + readable; 
        }
    }

    void peekInt32(){

    }

    void readFd(int fd);
    void writeFd(int fd);



private:
    vector<char> _buffer;
    size_t _readIndex;
    size_t _writeIndex;
};