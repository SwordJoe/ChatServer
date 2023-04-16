#pragma once
#include<string>

class TimeStamp
{
public:
    TimeStamp();
    explicit TimeStamp(int64_t microSecondsSinceEpoch);
    
    static TimeStamp now();
    std::string toString() const;

private:
    int64_t _microSecondsSinceEpoch;
};