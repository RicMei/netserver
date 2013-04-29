#pragma once
#include "ESocket.h"

#include "EMutex.h"

class EConnectPool;
class EConnect
{
public:
	explicit EConnect(ESocket* es,EConnectPool* p_pool);
	~EConnect(void);
	

	bool sendMsg(void* msg,int len);
	//EMutex mutex;
	//void (*call_back)(int fd, int events, void *arg);  
	virtual bool msgPrase(void* p_data,int len){};
	void recvCallBack(int fd, int events, void *arg);
	void sendCallBack(int fd, int events, void *arg);
	void eventAdd(int fd,int events);
	void eventDel(int fd,int events);
	int getStatus();
	void setStatus(int st);
	int getFd();
	void setFd(int fd);
	void destory();
private:
	ESocket* s;
	EConnectPool* pool;
};

