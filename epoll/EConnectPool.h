#pragma once
#include "head.h"
#include <map>
#include "EMutex.h"
//#include <>
#include "EConnect.h"
class EConnectPool
{
	typedef map<int,EConnect*> ConnectMap;
	typedef map<int,EConnect*> ConnectMapIter;
public:
	EConnectPool(int p_port);
	~EConnectPool(void);
	void init();
	virtual void run();
	void EventAdd(int fd, int events,void *p_connect);			//增加event eventid 为 EPOLLIN|EPOLLOUT 这样的参数 
	void EventDel(int fd, int events,void *p_connect);						//删除event eventid	
	void RecvData(int fd, int events,void *arg);
	//void SendData(int fd,int eventid,void *arg);
	void AcceptConect( int eventid,void *arg );
	//void postData(void *data,int len);
	int getEpfd();
	void delConnect(EConnect* ec);
	
private:
	int epfd; //用来接收的fd
	int port;
	ConnectMap pool;
	struct epoll_event eventsAry[2048];
	EMutex mutex;
};

