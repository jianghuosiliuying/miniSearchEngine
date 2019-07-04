#pragma once

#include "Noncopyable.h"
#include <pthread.h>
#include <functional>

namespace mm
{
extern __thread int threadNum;

class Thread
: Noncopyable
{
public:
	using ThreadCallback = std::function<void()>;
	Thread(ThreadCallback && cb,int Num);

	void start();
	void join();

	~Thread();

private:
	static void * threadFunc(void *);

private:
	pthread_t _pthid;
	ThreadCallback _cb;
	bool _isRunning; 
    int Num_;
};

}//end of namespace mm

