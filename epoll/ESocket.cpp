#include "ESocket.h"

template <typename T>
data_buff_template<T>::data_buff_template( data_buff_template<T>& p_buff )
{
	::clear();
	if(p_buff.len > sizeof(T))
	{
		T d_len = *((T*)(p_buff.buff));
		if(d_len > 0 && p_buff.len >= d_len+sizeof(T))
		{
			len = p_buff.len;
			bcopy(p_buff.buff+sizeof(T),buff,d_len);	
		}
	}
}
ESocket::ESocket(int sid,struct sockaddr_in* p_addr = NULL)
{
	//bzero(this,sizeof(*this));
	
	r_buff.clear();
	w_buff.clear();
	events = 0;
	status = 0;
	sock = sid;
	last_time = time(NULL);
	bzero(&this->addr, sizeof(struct sockaddr_in));
	if(NULL == p_addr)
	{
		socklen_t len = sizeof(struct sockaddr);
		getpeername(this->sock, (struct sockaddr *)&this->addr, &len); // 本地对本地
		
	}
	else
	{
		bcopy(addr, &this->addr, sizeof(struct sockaddr_in));
	}
	//设置本地ip
	bzero(&local_addr,sizeof(struct sockaddr_in));
	socklen_t len = sizeof(struct sockaddr_in);
	getsockname(this->sock, (struct sockaddr *)&local_addr, &len);
	
}


ESocket::~ESocket(void)
{
	
}

bool ESocket::sync()
{
	if(time(NULL) - last_time > sync_time_interval)
	{
		sync_all_msg();		
		//int retcode = send(sock, pBuffer, nSize, MSG_NOSIGNAL);
		last_time = time(NULL);
	}
}


bool ESocket::foce_sync()
{
	bool ret = sync_all_msg();
	last_time = time(NULL);
	return ret;
}

bool ESocket::sync_all_msg()
{
	int i = 0;
	for(;sync_one_msg();i++)
	{
		//do everything you want
	}
	return i;
}

bool ESocket::sync_one_msg()
{
	mutex.lock();
	data_buff t_buff(w_buff);
	if(t_buff.len > 0)
	{
		if( -1 != send(sock,&t_buff,sizeof(t_buff.len)+len,0) )
		{
			w_buff.post_one_msg();
			mutex.unlock();
			return true;
		}
		else
		{
			//异常处理 ....
			mutex.unlock();
			return false;
		}
	}
	mutex.unlock();
	return false;

}

bool ESocket::send_msg( void* p_data,int len )
{
	//尝试两次之后抛弃
	mutex.lock();
	if(w_buff.push_one_msg((char*)p_data,len))
	{
		mutex.unlock();
		return true;
	}
	else
	{
		sync_all_msg();
		if(w_buff.push_one_msg((char*)p_data,len))
		{
			mutex.unlock();
			return true;
		}
	}
	mutex.unlock();
	return false;
}

void ESocket::event_del(int fd,int p_events)
{
	struct epoll_event epv = {0,{0}};
	epv.events = p_events;
	int ctl_res = epoll_ctl(fd, EPOLL_CTL_DEL, sock, &epv); // return -1 / 0
	if(ctl_res < 0)
	{
		std::cout << "----epoll_ctl del failed-----" << std::endl;
	}
}



void ESocket::event_add( int fd,int events )
{
		/*
	EPOLLIN     //表示对应的文件描述符可以读（包括对端SOCKET正常关闭）；
	EPOLLOUT    //表示对应的文件描述符可以写；
	EPOLLPRI    //表示对应的文件描述符有紧急的数据可读（这里应该表示有带外数据到来）；
	EPOLLERR    //表示对应的文件描述符发生错误；
	EPOLLHUP    //表示对应的文件描述符被挂断；
	EPOLLET     //将EPOLL设为边缘触发(Edge Triggered)模式，这是相对于水平触发(Level Triggered)来说的。
	EPOLLONESHOT//只监听一次事件，当监听完这次事件之后，如果还需要继续监听这个socket的话，需要再次把这个socket加入到EPOLL队列里。
	*/
	struct epoll_event epv = {0,{0}};		//实例化一个epv对象 
	int op;									//epoll_ctl 的其中一个参数 EPOLL_CTL_MOD | EPOLL_CTL_ADD | EPOLL_CTL_DEL
	epv.data.ptr = this;						//让这个指针指向我们自己定义的
	epv.events = p_events;					//event 类型 EPOLLIN | EPOLLOUT | EPOLLPRI | EPOLLERR | EPOLLHUP | EPOLLET | EPOLLONESHOT
	events = p_events;
	if(status == 1) //表示已经存在 接下来
	{
		op = EPOLL_CTL_MOD;
	}
	else
	{
		op = EPOLL_CTL_ADD;
		status = 1; // 设置为已经存在 
	}
	int ctl_res = epoll_ctl(fd, op, sock, &epv); // return -1 / 0
	if(ctl_res < 0)
	{
		std::cout << "----epoll_ctl add failed-----" << std::endl;
	}
}



