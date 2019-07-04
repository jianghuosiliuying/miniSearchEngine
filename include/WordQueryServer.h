#pragma once

#include "Threadpool.h"
#include "TcpServer.h"
//#include "../include/Timer.h"

namespace mm
{
class Timer;
class Configuration;
class CacheManger;
class Threadpool;
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
    CacheManger * cacheM_;
    Timer * timer_;
	Threadpool threadpool_;
	TcpServer server_;
};

}//end of namespace mm
