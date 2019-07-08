#pragma once

#include "Redis.h"
#include "MutexLock.h"
#include <queue>

using namespace std;

namespace mm
{

using connect = Redis *;
using ElemType = connect ;

class Redispool
{
public:
	Redispool(size_t connectNum);
	~Redispool();
    connect getConnect();
    void backConnect(connect connect);
private:
	size_t _connectNum;
	queue<ElemType> _que;
	MutexLock _mutex;
};

}//end of namespace mm

