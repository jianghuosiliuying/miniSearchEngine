#include "../include/TaskQueue.h"

#include <iostream>
using std::cout;
using std::endl;

namespace mm
{

TaskQueue::TaskQueue(size_t sz)
: _sz(sz)
, _mutex()
, _notEmpty(_mutex)
, _notFull(_mutex)
, _flag(true)
{}

bool TaskQueue::empty() const
{
	return _que.empty();
}
bool TaskQueue::full() const
{
	return _que.size() == _sz;
}

//在生产者线程运行
void TaskQueue::push(ElemType elem)
{
	{
	MutexLockGuard autoLock(_mutex);
	while(full()) {//防止条件变量被异常唤醒
		_notFull.wait();//有可能被异常唤醒
	}
	//....return;
	_que.push(elem);
	}
	_notEmpty.notify();
}

//在消费者线程运行
ElemType TaskQueue::pop()
{
	ElemType ret;
	{
	MutexLockGuard autoLock(_mutex);
	while(_flag && empty()) {
	//while(empty()) {
		_notEmpty.wait();
	}
	
	if(_flag) {
		ret = _que.front();
		_que.pop();
		_notFull.notify();
	} else {
		return nullptr;
	}
	}
	return ret;
}

void TaskQueue::wakeup()
{
	_flag = false;
	_notEmpty.notifyAll();
}

}//end of namespace mm
