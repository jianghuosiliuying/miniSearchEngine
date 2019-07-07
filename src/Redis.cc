#include "../include/Redis.h"
#include <iostream>

namespace mm
{
Redis::Redis()
:_connect(nullptr)
,_reply(nullptr)
{
    cout<<"Redis()"<<endl;
}
Redis::~Redis()
{
    if(_connect){
        redisFree(_connect);
    }
    this->_connect=nullptr;
    this->_reply=nullptr;
}
bool Redis::connect(string host,int port)
{
    this->_connect=redisConnect(host.c_str(),port);
    if(this->_connect!=nullptr && this->_connect->err){
        printf("connect error:%s\n",this->_connect->errstr);
        return 0;
    }
    return 1;
}
string Redis::get(string key)
{
    _reply=(redisReply*)redisCommand(_connect,"GET %s",key.c_str());
    //printf("Succeed to execute command: get %s\n",key.c_str());
    if(_reply->type==REDIS_REPLY_NIL){
        return string();//返回空串
    }
    string str=_reply->str;
    freeReplyObject(_reply);
    return str;
}
void Redis::set(string key,string value)
{
    redisCommand(_connect,"SET %s %s",key.c_str(),value.c_str());
    //printf("Succeed to execute command: set %s %s\n",key.c_str(),value.c_str());
}

}//end of namespace mm
