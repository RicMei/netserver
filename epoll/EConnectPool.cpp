#include "EConnectPool.h"


EConnectPool::EConnectPool(int p_port)
{
	port = p_port;
}


EConnectPool::~EConnectPool(void)
{
}
void EConnectPool::run()
{
	//��ѭ��
	//���ӳ�ʱ����ô 
	while(true)
	{
		long now = time(NULL);
		msleep(10);
		if(pool.size() > 0)
		{
			int ret = epoll_wait(epfd,eventsAry,2048,0);
			if(ret > 0)
			{
				for(int i = 0;i < ret;i++)
				{
					//����¼�
					EConnect *ec = eventsAry[i].data.ptr;
					if (eventsAry[i].events & (EPOLLERR | EPOLLPRI))
					{
						//�����˴��� �Ͽ� 
						ec->destory();
					}
					else if ( eventsAry[i].events & (EPOLLIN))
					{
						//��������������
						if(epfd == ec->getFd() ) //����Ǳ�
						{
							AcceptConect(epfd,ec);
						}
						else
						{
							ec->recvCallBack(epfd,eventsAry[i].events,ec);
						}
						
					}
					else if ( eventsAry[i].events & (EPOLLOUT) )
					{
						//д������δ��
						ec->sendCallBack(epfd,eventsAry[i].events,ec);
					}

				}
			}
		}
	}
}

void EConnectPool::init()
{
	epfd = epoll_create(1024);
	assert(-1 != kdpfd);
	//����accept���ӵ�fd
	int listenFd = socket(AF_INET,SOCK_STREAM,0);
	fcntl(listenFd,F_SETFL,O_NONBLOCK);// set non-blocking
	printf("server listen fd = %d\n",listenFd);
	ESocket* es = new ESocket(listenFd);
	EConnect* ec = new EConnect(es,this);
	EventAdd(epfd,EPOLLIN,ec);

	sockaddr_in sin;
	bzero(&sin,sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons(port);
	bind(listenFd,(const sockaddr*)&sin,sizeof(sin));
	listen(listenFd,1024);

}

void EConnectPool::EventAdd(int fd, int events,void *p_connect )
{

	((EConnect*)(p_connect))->eventAdd(fd,events);
	 
}

void EConnectPool::EventDel(int fd, int events,void *p_connect)
{
	EConnect* ec = (EConnect*)p_connect;
	if(ec->getStatus() != 1) return; //���뵽event�¼��еĶ���1
	struct epoll_event epv = {0,{0}};
	epv.data.prt = ec;
	if(epoll_ctl(epfd, EPOLL_CTL_DEL, ec->getFd(), &epv) >= 0)
	{
		ec->setStatus(0);
	}
}


void EConnectPool::AcceptConect( int eventid,void *arg )
{
	mutex.lock();
	struct sockaddr_in sin;
	socklen_t len = sizeof(struct sockaddr_in);
	int nfd;
	if(nfd = accept(epfd,(struct sockaddr*)&sin,&len) == -1)
	{
		//����ʧ��
		if(errno != EAGAIN && errno != EINTR)
		{
			
		}
		std::cout << "--------real accept failed ---------" << std::endl;
		mutex.unlock();
		return;
	}
	//��ʱ����Ҫѭ��
	//Ϊ�׽ӿ����÷�����
	if (fcntl(nfd, F_SETFL, O_NONBLOCK)) < 0)
	{
		std::cout << "------set noblock failed------" << std::endl;
		mutex.unlock();
		return ;
	}



	//new һ��connect
	ESocket* nS = new ESocket(nfd,&sin);
	EConnect* nConnect = new EConnect(nS,this); 
	
	pool->insert(ConnectMap::value_type(nfd,nConnect));
	EventAdd(epfd,EPOLLIN|EPOLLOUT|EPOLLERR|EPOLLHUP,nConnect); //���Ӽ����¼�
	mutex.unlock();
}
int EConnectPool::getEpfd()
{
	return epfd;
}

void EConnectPool::delConnect( EConnect* ec )
{
	mutex.lock();
	ConnectMapIter it = pool.find(ec->getFd());
	if(it != pool.end())
	{
		pool.erase(ec->getFd());
	}
	delete ec;
	mutex.unlock();
}

