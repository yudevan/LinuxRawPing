#pragma once

#include <pthread.h>
// While the class IPbuffer is not thread safe it does include at mutex if you 
// want to create thread safe actions.

class Mutex
{
    
protected:
    pthread_mutex_t mutex;
public:
	Mutex(void)
	{ 
		init();
	}
    
	virtual ~Mutex(void)
	{   
		pthread_mutex_destroy(&mutex);
	}
    
	virtual int init()
	{
        return pthread_mutex_init( &mutex, NULL );
	}
    
	virtual int lock()
	{ 
        return pthread_mutex_lock( &mutex );
	}
    
	virtual int unlock()
	{ 
        return pthread_mutex_unlock( &mutex );
	}
    
	virtual int trylock()
	{ 
        return pthread_mutex_trylock( &mutex );
	}
    
};

class Lock
{
 protected:
	Mutex &refMtx;
 public:
	Lock(Mutex &mut):refMtx(mut)
	{   
		refMtx.lock(); 
	}
    
	virtual ~Lock(void)
	{ 
		refMtx.unlock();  
	}
};

