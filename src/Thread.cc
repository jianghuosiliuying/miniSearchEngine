#include "../include/Thread.h"
#include <iostream>
using std::cout;
using std::endl;

namespace mm
{
__thread int threadNum=0;

using ThreadCallback=std::function<void()>;
struct ThreadData
{
    int Num_;
    ThreadCallback cb_;
    ThreadData(const int Num,ThreadCallback cb)
    :Num_(Num)
    ,cb_(cb)
    {}
    void runInThread()
    {
        //任务执行之前: do something
        threadNum=Num_;
        cout<<"I am "<<threadNum<<" thread"<<endl;
        if(cb_)
            cb_();
        //任务执行之后: do something....
    }
};

Thread::Thread(ThreadCallback && cb,int Num)
: _pthid(0)
, _cb(std::move(cb))
, _isRunning(false)
,Num_(Num)
{
	cout <<Num_<< " Thread()" << endl;
}

void Thread::start()
{
    ThreadData * data=new ThreadData(Num_,_cb);
	if(pthread_create(&_pthid, NULL, threadFunc, data))
        perror("pthread_create");
	_isRunning = true;
}

void * Thread::threadFunc(void * arg)
{
	ThreadData * pdata = static_cast<ThreadData*>(arg);
	if(pdata)
		pdata->runInThread();
    delete pdata;

	return nullptr;
}

void Thread::join()
{
	if(_isRunning){
		pthread_join(_pthid, NULL);
        _isRunning=false;
    }
}

Thread::~Thread()
{
	if(_isRunning) {
		pthread_detach(_pthid);	
        _isRunning=false;
	}
	cout <<threadNum<< "~Thread()" << endl;
}

}//end of namespace mm
