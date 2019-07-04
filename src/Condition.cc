#include "../include/Condition.h"
#include "../include/MutexLock.h"  //在Condtion的实现文件里面加载MutexLock的头文件

#include <stdio.h>
#include <errno.h>

namespace mm
{

Condition::Condition(MutexLock & mutex)
: _mutex(mutex)
{
	if(pthread_cond_init(&_cond, NULL)) {
		perror("pthread_cond_init");
	}
}

Condition::~Condition()
{
	if(pthread_cond_destroy(&_cond)) {
		perror("pthread_cond_destroy");
	}
}

void Condition::wait()
{
	if(pthread_cond_wait(&_cond, _mutex.getMutexLockPtr())) {
		perror("pthread_cond_wait");
	}
}

void Condition::notify()
{
	if(pthread_cond_signal(&_cond)) {
		perror("pthread_cond_signal");
	}
}

void Condition::notifyAll()
{
	if(pthread_cond_broadcast(&_cond)) {
		perror("pthread_cond_broadcast");
	}
}

}//end of namespace mm
