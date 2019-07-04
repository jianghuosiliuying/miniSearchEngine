#include "../include/Threadpool.h"
//#include "../include/Thread.h"

#include <unistd.h>

#include <iostream>
using std::cout;
using std::endl;

namespace mm 
{
//__thread int threadNum=0;

Threadpool::Threadpool(size_t Num, size_t queSize)
: _threadNum(Num)
, _queSize(queSize)
, _que(_queSize) 
, _isExit(false)
{
	_threads.reserve(_threadNum);
}


void Threadpool::start()
{
	for(size_t idx = 1; idx <= _threadNum; ++idx) {
		unique_ptr<Thread> thread(new Thread(
			std::bind(&Threadpool::threadFunc, this),idx));
		_threads.push_back(std::move(thread));
	}

	for(auto & thread : _threads) {
		thread->start();
	}
}

void Threadpool::stop()
{
	if(!_isExit) {
		//先将任务队列中的任务执行完毕
		while(!_que.empty()) {
			::sleep(1);
		}

		_isExit = true;
		cout << ">>> _isExit = true " << endl;

		_que.wakeup();

		for(auto & thread : _threads) {
			thread->join();
		}
	}
}

void Threadpool::addTask(Task && task)
{
	_que.push(task);
}

Task Threadpool::getTask()
{
	return _que.pop();
}

//每一个子线程运行
void Threadpool::threadFunc()
{
	while(!_isExit) {
		Task task = getTask();
		if(task) {
			task();
		}
	}
}


}//end of namespace mm
