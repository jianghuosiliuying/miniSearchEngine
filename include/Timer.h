#pragma once

#include <functional>

using namespace std;

namespace mm
{
class Timer
{
using TimerCallback=function<void()>;
public:
    static Timer * createTimer();
    static void destory();
    Timer * initTime(int initTime,int intervalTime,TimerCallback && cb);
    void start();
    void stop();
    int getFd();
    void setTimer(int initTime,int intervalTime);
    void handleRead();//包含读描述符和执行回写事件

private:
    Timer(){}
    ~Timer(){}
    int createTimerfd();
private:
    static Timer * ptimer_;
    int fd_;
    int initalTime_;
    int intervalTime_;
    TimerCallback cb_;
    bool isStarted_;
};


}//end of namespace mm
