#pragma once

#include <string>

using std::string;

namespace mm
{
struct Train
{
    int dataLen_;
    char buf_[655350];
};

class SocketIO
{
public:
	explicit
	SocketIO(int fd);

    int readTrain(char * buff);//小火车接收，最大65536
    int recvCycle(void* p,int len);
	int readn(char * buff, int len);
	int readline(char * buff, int maxlen);
	int writen(const char * buff, int len);
	int writeTrain(const char * buff);//小火车发送，最大4096
private:
	int recvPeek(char * buff, int len);

private:
	int _fd;
};

}//end of namespace mm

