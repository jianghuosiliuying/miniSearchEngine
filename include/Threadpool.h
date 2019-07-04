#pragma once

#include "TaskQueue.h"
#include "Thread.h"

#include <vector>
#include <memory>
#include <functional>
using std::vector;
using std::unique_ptr;

namespace mm
{
//extern __thread int threadNum;

using Task = std::function<void()>;
//class Thread;
class Threadpool
{
public:
	Threadpool(size_t, size_t);

	void start();
	void stop();

	void addTask(Task && task);

private:
	void threadFunc();
	Task getTask();

private:
	size_t _threadNum;
	size_t _queSize;
	vector<unique_ptr<Thread>> _threads;
	TaskQueue _que;
	bool _isExit;
};

}//end of namespace mm

