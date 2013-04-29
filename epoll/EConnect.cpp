#include "EConnect.h"
#include "EConnectPool.h"

EConnect::EConnect(ESocket* es,EConnectPool* p_pool)
{
	s = es;
	pool = p_poll;
}


EConnect::~EConnect(void)
{
	delete s;
}

void EConnect::recvCallBack( int fd, int events, void *arg )
{
	s->mutex.lock();
	base_buff recv_buff(s->r_buff);
	if(recv_buff.len > 0)
	{
		//�յ�һ�������ļ�¼�� ....
		msgPrase(recv_buff.buff,recv_buff.len);
		s->r_buff.post_one_msg();
	}
	else
	{
		//δ�յ�һ��������¼ ��ʱ������ �ȴ��´��¼����� 
	}
	s->mutex.unlock();
}

void EConnect::sendCallBack( int fd, int events, void *arg )
{
	s->sync();
}

void EConnect::eventAdd(int fd,int events)
{
	s->event_add(fd,events);
}
void EConnect::eventDel(int fd,int events)
{
	s->event_del(fd,events);
}

int EConnect::getStatus()
{
	if(s)
	{
		return s->status;
	}
	return 0;
}

void EConnect::setStatus( int st )
{
	if(s)
	{
		s->status = st;
	}
}

int EConnect::getFd()
{
	if(s)
	{
		return s->sock;
	}
	return -1;
}

void EConnect::setFd( int fd )
{
	if(s)
	{
		s->sock = fd;
	}
}

void EConnect::destory()
{
	
	if(s && pool )
	{
		s->event_del(pool->getEpfd(),s->events);
		close(s->sock);
		delete s;
		pool->delConnect(this);
	}
	s = NULL;
	pool = NULL;
}
