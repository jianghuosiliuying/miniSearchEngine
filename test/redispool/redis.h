#include <iostream>
#include <hiredis/hiredis.h>
#include <string>

using namespace std;

class Redis
{
public:
    Redis()
    :_connect(nullptr)
    ,_reply(nullptr)
    {
        cout<<"RedisLRU()"<<endl;
    }
    ~Redis()
    {
        if(_connect){
            redisFree(_connect);
        }
        this->_connect=nullptr;
        this->_reply=nullptr;
    }
    bool connect(string host,int port)
    {
        this->_connect=redisConnect(host.c_str(),port);
        if(this->_connect!=nullptr && this->_connect->err){
            printf("connect error:%s\n",this->_connect->errstr);
            return 0;
        }
        return 1;
    }
    string get(string key)
    {
        _reply=(redisReply*)redisCommand(_connect,"GET %s",key.c_str());
        printf("Succeed to execute command: get %s\n",key.c_str());
        if(_reply->type==REDIS_REPLY_NIL){
            return string("-1");
        }
        string str=_reply->str;
        freeReplyObject(_reply);
        return str;
    }
    void set(string key,string value)
    {
        redisCommand(_connect,"SET %s %s",key.c_str(),value.c_str());
        printf("Succeed to execute command: set %s %s\n",key.c_str(),value.c_str());
    }
private:
    redisContext * _connect;
    redisReply * _reply;
};

int main()
{
    std::cout << "Hello world" << std::endl;
    Redis * r=new Redis();
    if(!r->connect("192.168.80.128",6379)){
        printf("connect error.\n");
        return 0;
    }
    r->set("小米","雷军");
    //cout<<"Get the name is "<<r->get("h1")<<endl;
    cout<<"Get the name is "<<r->get("小米")<<endl;
    delete r;
    return 0;
}

