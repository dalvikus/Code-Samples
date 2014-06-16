#ifndef __THREAD_H__
#define __THREAD_H__
////////////////////////////////////////////////////////////////////////////////


#include <pthread.h>
#include <assert.h>

class CThread {
public:
typedef enum {
	THREAD_FLAG__RUNNING	= 0,

	THREAD_FLAG__KILL,		// Order
	THREAD_FLAG__TERMINATE,		// Response

	THREAD_FLAG__PAUSE,		// Order
	THREAD_FLAG__STOPPED,		// Response

	THREAD_FLAG__RESUME,		// Order
	THREAD_FLAG__CONTINUE,		// Response
} THREAD_FLAG_T;
public:
	CThread(void (*thread_main)(void*), void* voidPtr);
	virtual ~CThread();

private:
	THREAD_FLAG_T	m__flag;
	pthread_t	m__id;		// 쓰레드 ID
private:
	pthread_mutex_t	m__mutex;
public:
	void	lock(void)	{assert(pthread_mutex_lock(&this->m__mutex) == 0);}
	void	unlock(void)	{assert(pthread_mutex_unlock(&this->m__mutex) == 0);}
public:
	THREAD_FLAG_T	getFlag(void);
	void		setFlag(THREAD_FLAG_T flag);
};


////////////////////////////////////////////////////////////////////////////////
#endif	// !__THREAD_H__
