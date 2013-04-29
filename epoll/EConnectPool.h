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
	void EventAdd(int fd, int events,void *p_connect);			//����event eventid Ϊ EPOLLIN|EPOLLOUT �����Ĳ��� 
	void EventDel(int fd, int events,void *p_connect);						//ɾ��event eventid	
	void RecvData(int fd, int events,void *arg);
	//void SendData(int fd,int eventid,void *arg);
	void AcceptConect( int eventid,void *arg );
	//void postData(void *data,int len);
	int getEpfd();
	void delConnect(EConnect* ec);
	
private:
	int epfd; //�������յ�fd
	int port;
	ConnectMap pool;
	struct epoll_event eventsAry[2048];
	EMutex mutex;
};

