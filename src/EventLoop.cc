#include "../include/EventLoop.h" 
#include "../include/Acceptor.h"
#include "../include/Timer.h"
#include "../include/TcpConnection.h"

#include <unistd.h>
#include <assert.h>
#include <sys/eventfd.h>
#include <iostream>

using namespace std;

namespace mm
{

EventLoop::EventLoop(Acceptor & acceptor)
: _efd(createEpollFd())
, _eventfd(createEventFd())
,pTimer_(Timer::createTimer())
,timerfd_(pTimer_->getFd())
, _acceptor(acceptor)
, _eventList(1024)
, _isLooping(false)
{
	addEpollFdRead(_acceptor.fd());
	addEpollFdRead(_eventfd);
	addEpollFdRead(timerfd_);
}

void EventLoop::loop()
{
	_isLooping = true;
	while(_isLooping) {
		waitEpollFd();
	}
}

void EventLoop::unloop()
{
	if(_isLooping) 
		_isLooping = false;
}

void EventLoop::runInLoop(Functor && cb)
{
	{
	MutexLockGuard autolock(_mutex);
	_pendingFunctors.push_back(std::move(cb));
	}

	wakeup();
}

void EventLoop::waitEpollFd()
{
	int nready;
	do {
		nready = epoll_wait(_efd, &*_eventList.begin(), _eventList.size(), 5000);
	}while(nready == -1 && errno == EINTR);

	if(nready == -1) {
		perror("epoll_wait");
		return;
	} else if(nready == 0) {
		cout << ">> epoll_wait timeout!" << endl;
	} else {
		if((size_t)nready == _eventList.size()) {
			_eventList.resize(2 * nready);
		}

		for(int idx = 0; idx != nready; ++idx) {
			int fd = _eventList[idx].data.fd;
			if(fd == _acceptor.fd()) {
				//处理新连接
				if(_eventList[idx].events & EPOLLIN) {
					handleNewConnection();
				}
			} else if(fd == _eventfd) {//处理旧连接给客户端发数据
				if(_eventList[idx].events & EPOLLIN) {
					handleSendRead();
					//cout << ">>before doPendingFunctors()" << endl;
					doPendingFunctors();//在这里发送数据
					//cout << ">>after doPendingFunctors()" << endl;
				}
            }else if(fd==timerfd_){//处理时间超时回写事件
                if(_eventList[idx].events & EPOLLIN){
                    cout<<"i will handle update cache."<<endl;
                    pTimer_->Timer::handleRead();//包含读取描述符和执行回写事件
                }
            } else {
				//处理消息,接收客户端数据
				if(_eventList[idx].events & EPOLLIN) {
					handleMessage(fd);
				}
			}
		}
	}
}

void EventLoop::handleNewConnection()
{
	int peerfd = _acceptor.accept();
	addEpollFdRead(peerfd);
	TcpConnectionPtr conn(new TcpConnection(peerfd, this));
	conn->setConnectionCallback(_onConnection);
	conn->setMessageCallback(_onMessage);
	conn->setCloseCallback(_onClose);

	_conns.insert(std::make_pair(peerfd, conn));

	conn->handleConnectionCallback();
}

void EventLoop::handleMessage(int fd)
{
	bool isClosed = isConnectionClosed(fd);
	auto iter = _conns.find(fd);
	assert(iter != _conns.end());//运行时断言

	if(!isClosed) {
		iter->second->handleMessageCallback();
	} else {
		delEpollFdRead(fd);
		iter->second->handleCloseCallback();
		_conns.erase(iter);
	}
}

void EventLoop::handleSendRead()
{//子线程完成计算，通知主线程向客户端发送数据
	uint64_t howmany;
	int ret = ::read(_eventfd, &howmany, sizeof(howmany));
	if(ret != sizeof(howmany)) {
		perror("read");
	}
}

void EventLoop::wakeup()
{//唤醒子线程进行计算
	uint64_t one = 1;
	int ret = ::write(_eventfd, &one, sizeof(one));
	if(ret != sizeof(one)) {
		perror("write");
	}
}

void EventLoop::doPendingFunctors()
{
	vector<Functor> tmp;
	{
		MutexLockGuard autolock(_mutex);
		tmp.swap(_pendingFunctors);
	}

	for(auto & functor : tmp)
	{
		functor();
	}
}

bool EventLoop::isConnectionClosed(int fd)
{
	int ret;
	do{
		char buff[1024];
		ret = recv(fd, buff, sizeof(buff), MSG_PEEK);
	}while(ret == -1 && errno == EINTR);

	return (ret == 0);
}
	
int EventLoop::createEpollFd()
{
	int ret = ::epoll_create1(0);
	if(ret == -1) {
		perror("epoll_create1");
	}
	return ret;
}

int EventLoop::createEventFd()
{
	int ret = ::eventfd(0, 0);
	if(ret == -1) {
		perror("eventfd");
	}
	return ret;
}

void EventLoop::addEpollFdRead(int fd)
{
	struct epoll_event evt;
	evt.data.fd = fd;
	evt.events = EPOLLIN;
	int ret = epoll_ctl(_efd, EPOLL_CTL_ADD, fd, &evt);
	if(-1 == ret) {
		perror("epoll_ctl");
	}
}

void EventLoop::delEpollFdRead(int fd)
{
	struct epoll_event evt;
	evt.data.fd = fd;
	int ret = epoll_ctl(_efd, EPOLL_CTL_DEL, fd, &evt); 
	if(-1 == ret) {
		perror("epoll_ctl");
	}
}

}//end of namespace mm
