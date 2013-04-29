#pragma once
#include "head.h"
#include <time.h>
#include "EMutex.h"
#define buff_size  2046
/*
*	基本数据模板类
*/
template <typename T>
struct data_buff_template
{
	T len;
	char buff[buff_size];
	data_buff_template();
	int get_free_size()
	{
		return buff_size - len;
	}
	//int get_used_size()
	//{
	//	return sizeof(len);
	//}
	data_buff_template(data_buff_template<T>& p_buff);//非拷贝构造函数
	data_buff_template(int &p_len ,char* p_buf)
	{
		clear();
		len = p_len;
		bcopy(p_buf,buff,p_len);
		
	}
	void clear()
	{
		bzero(this,*this);
	}
};
/*
*读写数据的模板类
*/
template <typename T>
struct wr_buff :public data_buff_template<T>
{
	wr_buff(wr_buff<T>& p_buff):data_buff_template<T>(p_buff){};
	bool push_one_msg(char* p_data,int p_len);		//一条消息入栈
 	void post_one_msg();								//一条消息出栈							
};

template <typename T>
bool wr_buff<T>::push_one_msg( char* p_data,int p_len )
{
	if(get_free_size() > p_len)
	{
		len +=p_len;
		bcopy(p_data,buff,p_len);
		return true;
	}
	return false;

}

template <typename T>
void wr_buff<T>::post_one_msg()
{
	if(len > sizeof(T))
	{
		if(*((T*)buff) > 0 && len >= *((T*)buff) + sizeof(T) )
		{
			bcopy(buff+sizeof(T),buff,*((T*)buff));
		}
		else
		{
			//直接置空
			clear();
		}
	}
}


//暂时用int
typedef data_buff_template<int> base_buff;
typedef wr_buff<int> data_buff;
class ESocket
{
	//static const int buff_size = 2046;
	static const int sync_time_interval = 60; // 60 毫秒 同步时间间隔
public:
	explicit ESocket(int sid,struct sockaddr_in* p_addr = NULL);
	~ESocket(void);
	data_buff r_buff;
	data_buff w_buff;	
	int events;
	long last_time;
	//void *arg;
	int status;
	EMutex mutex;							//互斥锁
public:
	bool sync();
	bool foce_sync();		//当buff不够时候 强制发送
	int recv_to_buff();		//接收
	bool send_msg(void* p_data,int len);
	void event_add(int fd,int p_events);
	void event_del(int fd,int p_events);
			/**
		 * \brief 获取套接口对方的地址
		 * \return IP地址
		 */
		inline const char *getIP() const { return inet_ntoa(addr.sin_addr); }
		inline const DWORD getAddr() const { return addr.sin_addr.s_addr; }

		/**
		 * \brief 获取套接口对方端口
		 * \return 端口
		 */
		inline const unsigned short getPort() const { return ntohs(addr.sin_port); }

		/**
		 * \brief 获取套接口本地的地址
		 * \return IP地址
		 */
		inline const char *getLocalIP() const { return inet_ntoa(local_addr.sin_addr); }

		/**
		 * \brief 获取套接口本地端口
		 * \return 端口
		 */
		inline const unsigned short getLocalPort() const { return ntohs(local_addr.sin_port); }

protected:
	bool sync_all_msg();
	bool sync_one_msg();
public:
	int sock;
	struct sockaddr_in local_addr;
	struct sockaddr_in addr;
	//int state;    // 0未连接  1已连接  2要断开
};

