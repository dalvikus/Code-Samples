#include "CQueueDatabaseThread.h"
#include "CPacket.h"
#include "eprintf.h"
#include <assert.h>
#include <string.h>	// memcpy

void
queueDatabaseThreadMain(
	void*	voidPtr
)
{
	CQueueDatabaseThread&	queue_database = *((CQueueDatabaseThread*) voidPtr);
	CBuffer	rPacket;

	while (1) {
		struct timeval  tv;
		tv.tv_sec = 1;
		tv.tv_usec = 0;
		int	retval = select(0, NULL, NULL, NULL, &tv);
		if (retval < 0) {
			eprintf(E_ERR, EFMT("select(): Failure: %m"));
			continue;
		}

		CThread::THREAD_FLAG_T	flag = queue_database.getFlag();
		if (flag == CThread::THREAD_FLAG__KILL) {
			(void) printf("DatabaseocketThreadMain: Le Fine\n");
			return;
		}

		if (queue_database.flush_ts() < 0) {
			eprintf(E_ERR, EFMT("CDatabaseIN::flush_ts(): Failure"));
		}
	}
}


CQueueDatabaseThread::CQueueDatabaseThread(
	size_t	queueSize
) : CDatabaseIN(queueSize), CThread(queueDatabaseThreadMain, this)
{
}

CQueueDatabaseThread::~CQueueDatabaseThread()
{
}
