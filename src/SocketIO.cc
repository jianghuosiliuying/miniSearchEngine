#include "../include/SocketIO.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <iostream>

using namespace std;

namespace mm
{

SocketIO::SocketIO(int fd)
: _fd(fd)
{}
#if 0
int SocketIO::readTrain(char * buff)
{
    int dataLen;
    recvCycle(&dataLen,4);//接收数据长度
    int ret=recvCycle(buff,dataLen);//接收数据长度
    return ret;
}
#endif

int SocketIO::recvCycle(void *p,int len)
{
    int ret,total=0;
    char* pStart=(char*)p;
    while(total<len)
    {
        ret=recv(_fd,pStart+total,len-total,0);
        if(ret==0){
            return -1;
        }
        total+=ret;
    }
    return total;
}

int SocketIO::readn(char * buff, int len)
{
	int left = len;
	char * p = buff;
	while(left > 0) {
		int ret = ::read(_fd, p, left);
		if(ret == -1 && errno == EINTR)
			continue;
		else if(ret == -1) {
			perror("read");
			return len - left;
		}
		else if(ret == 0) {
			return len - left;
		} else {
			left -= ret;
			p += ret;
		}
	}
	return len - left;
}

//每一次读取一行数据
int SocketIO::readline(char * buff, int maxlen)
{
	int left = maxlen - 1;
	char * p = buff; 
	int ret;
	int total = 0;
	while(left > 0) {
		ret = recvPeek(p, left);		
		//查找 '\n'
		for(int idx = 0; idx != ret; ++idx) {
			if(p[idx] == '\n') {
				int sz = idx + 1;
				readn(p, sz);
				total += sz;
				p += sz;
				*p = '\0';
				return total;
			}
		}
		//如果没有发现 '\n'
		readn(p, ret);
		left -= ret;
		p += ret;
		total += ret;
	}
	*p = '\0';// 最终没有发现'\n'
	return total;
}

int SocketIO::recvPeek(char * buff, int len)
{
	int ret;
	do {
		ret = ::recv(_fd, buff, len, MSG_PEEK);
	} while(ret == -1 && errno == EINTR);
	return ret;
}

int SocketIO::writeTrain(const char * buff)
{
    Train train;
    train.dataLen_=strlen(buff);
    strcpy(train.buf_,buff);
    int ret=send(_fd,&train,4+train.dataLen_,0);
    //cout<<"++"<<train.buf_<<"--"<<ret<<endl;
    return ret;
}
	
int SocketIO::writen(const char * buff, int len)
{
	int left = len;
	const char * p = buff;
	while(left > 0) {
		int ret = ::write(_fd, p, left);
		if(ret == -1 && errno == EINTR)
			continue;
		else if(ret == -1) {
			perror("write");
			return len - left;
		} else {
			left -= ret;
			p += ret;
		}
	}
	return len - left;
}

}//end of namespace mm
