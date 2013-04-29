#pragma once
#include <pthread.h>
struct EMutex
{
	pthread_mutex_t mutex;
	EMutex()
	{
		pthread_mutex_init(&mutex,NULL); 
	}
	~EMutex()
	{
		pthread_mutex_destroy(&mutex); 
	}
	void lock()
	{
		pthread_mutex_lock(mutex);
	}
	void unlock()
	{
		pthread_mutex_unlock(mutex);
	}
};