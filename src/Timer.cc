#include <iostream>

#include "../include/Timer.h"
#include <unistd.h>
#include <poll.h>
#include <sys/timerfd.h>
#include <iostream>

using namespace std;

namespace mm
{
Timer * Timer::createTimer()
{
    if(ptimer_==nullptr){
        ptimer_=new Timer();
        atexit(destory);
    }
    return ptimer_;
}
void Timer::destory()
{
    if(ptimer_)
        delete ptimer_;
}

Timer * Timer::initTime(int initTime,int intervalTime,TimerCallback && cb)
{
    fd_=createTimerfd();
    initalTime_=initTime;
    intervalTime_=intervalTime;
    cb_=move(cb);
    isStarted_=false;
    return ptimer_;
}
int Timer::getFd()
{   return fd_;  }

void Timer::start()
{
    isStarted_=true;
    setTimer(initalTime_,intervalTime_);
#if 0
    struct pollfd pfd;
    pfd.fd=fd_;
    pfd.events=POLLIN;
    while(isStarted_)
    {
        int nready=poll(&pfd,1,5000);
        if(-1==nready && errno==EINTR)   continue;
        if(-1==nready){
            perror("poll");
            return;
        }else if(0==nready){
            cout<<">>poll timeout!"<<endl;
        }else{
            if(pfd.revents & POLLIN){
                handleRead();//处理读事件
                if(cb_)
                    cb_();//执行任务
            }
        }
    }
#endif
}
int Timer::createTimerfd()
{
    int fd=timerfd_create(CLOCK_REALTIME,0);
    if(-1==fd){
        perror("timerfd_create");
    }
    return fd;
}
void Timer::stop()
{
    if(isStarted_){
        setTimer(0,0);//停止计时器
        isStarted_=false;
    }
}
void Timer::setTimer(int initTime,int intervalTime)
{
    struct itimerspec value;
    value.it_value.tv_sec=initTime;
    value.it_value.tv_nsec=0;
    value.it_interval.tv_sec=intervalTime;
    value.it_interval.tv_nsec=0;
    int ret=timerfd_settime(fd_,0,&value,nullptr);
    if(-1==ret){
        perror("timerfd_settime");
    }
}
void Timer::handleRead()
{
    uint64_t howmany;
    int ret=read(fd_,&howmany,sizeof(howmany));
    if(ret!=sizeof(howmany)){
        perror("read");
    }
    if(cb_)//执行回调函数，回写cache
        cb_();
}

Timer * Timer::ptimer_=createTimer();//饱汉模式

}//end of namespace mm

