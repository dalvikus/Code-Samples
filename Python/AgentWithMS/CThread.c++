#include "CThread.h"
#include <assert.h>
#include <stdio.h>

CThread::CThread(
	void	(*thread_main)(void*),
	void*	voidPtr
)
{
	assert(pthread_mutex_init(&this->m__mutex, NULL) == 0);

	this->m__flag = THREAD_FLAG__RUNNING;
	assert(pthread_create(&this->m__id, NULL, (void* (*)(void*)) thread_main, voidPtr) == 0);
}

CThread::~CThread()
{
	this->setFlag(THREAD_FLAG__KILL);
	assert(pthread_join(this->m__id, NULL) == 0);

	assert(pthread_mutex_destroy(&this->m__mutex) == 0);
}


void
CThread::setFlag(
	THREAD_FLAG_T	flag
)
{
	assert(pthread_mutex_lock(&this->m__mutex) == 0);
	this->m__flag = flag;
	assert(pthread_mutex_unlock(&this->m__mutex) == 0);
}

CThread::THREAD_FLAG_T
CThread::getFlag(
	void
)
{
	assert(pthread_mutex_lock(&this->m__mutex) == 0);
	THREAD_FLAG_T	flag = this->m__flag;
	assert(pthread_mutex_unlock(&this->m__mutex) == 0);

	return flag;
}
