#pragma once

#include <hiredis/hiredis.h>
#include <string>

using namespace std;

namespace mm
{
class Redis
{
public:
    Redis();
    ~Redis();
    bool connect(string host,int port);
    string get(string key);
    void set(string key,string value);
private:
    redisContext * _connect;
    redisReply * _reply;
};

}//end of namespace mm
