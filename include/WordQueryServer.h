#pragma once

#include "Threadpool.h"
#include "TcpServer.h"

namespace mm
{
class Configuration;
class Threadpool;
class Redispool;
class SpellcorrectServer//文本纠错类
{
public:
    SpellcorrectServer(const string & conffileName);
	void onConnection(const TcpConnectionPtr & conn);
	void onMessage(const TcpConnectionPtr & conn);
	void onClose(const TcpConnectionPtr & conn);

	void start();

private:
    Configuration * conf_;//更改为单例模式
    Redispool * _predispool;//redis连接池
	Threadpool threadpool_;
	TcpServer server_;
};

}//end of namespace mm
