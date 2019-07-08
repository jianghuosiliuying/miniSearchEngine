#include "MutexLock.h"

#include <stdio.h>
#include <errno.h>

namespace mm
{

MutexLock::MutexLock()
: _isLocking(false)
{
	if(pthread_mutex_init(&_mutex, NULL)) {
		perror("pthread_mutex_init");
	}
}

MutexLock::~MutexLock()
{
	if(pthread_mutex_destroy(&_mutex)) {
		perror("pthread_mutex_destroy");
	}
}

void MutexLock::lock()
{
	if(pthread_mutex_lock(&_mutex)) {
		perror("pthread_mutex_lock");
		return;
	}
	_isLocking = true;
}

void MutexLock::unlock()
{
	if(pthread_mutex_unlock(&_mutex)) {
		perror("pthread_mutex_unlock");
		return;
	}
	_isLocking = false;
}

}//end of namespace mm
