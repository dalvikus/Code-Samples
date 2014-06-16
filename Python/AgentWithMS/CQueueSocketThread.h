#ifndef __QUEUE_SOCKET_THREAD_H__
#define __QUEUE_SOCKET_THREAD_H__
////////////////////////////////////////////////////////////////////////////////


#include "CBuffer.h"
#include "CSocketOUT.h"
#include "CThread.h"
#include <pthread.h>

class CQueueSocketThread : public CSocketOUT, public CThread {
public:
	CQueueSocketThread(int sockfd, size_t queueSize);
	~CQueueSocketThread();
};


////////////////////////////////////////////////////////////////////////////////
#endif	// !__QUEUE_SOCKET_THREAD_H__
