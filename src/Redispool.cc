#include "../include/Redispool.h"
#include "../include/Configuration.h"
#include <iostream>

using std::cout;
using std::endl;

namespace mm
{
Redispool * Redispool::createRedispool()
{
    if(_predispool==nullptr){
        _predispool=new Redispool();
        atexit(destroy);
    }
    return _predispool;
}

void Redispool::destroy()
{
    if(_predispool){
        delete _predispool;
    }
}

Redispool::Redispool()
: _connectNum(stoi(Configuration::createConfig()->getConfigMap()["redisconnectNum"]))
, _mutex()
{
    for(size_t i=0;i!=_connectNum;++i)
    {
        Redis * r=new Redis();
        if(!r->connect("192.168.80.128",6379)){
            cout<<i<<" connect create error."<<endl;
        }
        _que.push(r);
    }
}

Redispool::~Redispool()
{
    while(!_que.empty()){
        Redis * r=_que.front();
        delete r;
        _que.pop();
    }
}

Redis * Redispool::getConnect()
{
    Redis * r;
    {
	MutexLockGuard autoLock(_mutex);
        r=_que.front();
        _que.pop();
    }
    return r;
}
void Redispool::backConnect(Redis * r)
{
    {
	MutexLockGuard autoLock(_mutex);
        _que.push(r);
    }
}

Redispool * Redispool::_predispool=nullptr;//饿汉模式

}//end of namespace mm
