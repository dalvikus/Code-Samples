#ifndef __QUEUE_DATABASE_THREAD_H__
#define __QUEUE_DATABASE_THREAD_H__
////////////////////////////////////////////////////////////////////////////////


#include "CBuffer.h"
#include "CDatabaseIN.h"
#include "CThread.h"
#include <pthread.h>

class CQueueDatabaseThread : public CDatabaseIN, public CThread {
public:
	CQueueDatabaseThread(size_t queueSize);
	~CQueueDatabaseThread();
};


////////////////////////////////////////////////////////////////////////////////
#endif	// !__QUEUE_DATABASE_THREAD_H__
