#pragma once

#include "Redis.h"
#include "MutexLock.h"
#include <queue>

using namespace std;

namespace mm
{


class Redispool
{
public:
    static Redispool * createRedispool();
    static void destroy();
    Redis * getConnect();
    void backConnect(Redis * connect);
private:
	Redispool();
	~Redispool();
private:
    static Redispool * _predispool;
	size_t _connectNum;
	queue<Redis*> _que;
	MutexLock _mutex;
};

}//end of namespace mm

