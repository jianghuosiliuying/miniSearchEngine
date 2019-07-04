#pragma once

namespace mm
{

class Socket
{
public:
	Socket();
	explicit
	Socket(int fd);

	int fd() const ;

	void shutdownWrite();

	~Socket();

private:
	int _fd;
};

}//end of namespace mm


