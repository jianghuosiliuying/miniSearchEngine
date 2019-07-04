#pragma once

#include "Noncopyable.h"
#include <pthread.h>

namespace mm
{

class MutexLock;//类的前向声明 可以减少头文件的依赖

class Condition
: Noncopyable
{
public:
	Condition(MutexLock & mutex);
	~Condition();

	void wait();
	void notify();
	void notifyAll();

private:
	pthread_cond_t _cond;
	MutexLock & _mutex;
};

}//end of namespace mm

