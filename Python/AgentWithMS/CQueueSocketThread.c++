#include "CQueueSocketThread.h"
#include "CPacket.h"
#include "eprintf.h"
#include <assert.h>
#include <string.h>	// memcpy

void
queueSocketThreadMain(
	void*	voidPtr
)
{
	CQueueSocketThread&	queue_socket = *((CQueueSocketThread*) voidPtr);
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

		CThread::THREAD_FLAG_T	flag = queue_socket.getFlag();
		if (flag == CThread::THREAD_FLAG__KILL) {
			(void) printf("queueSocketThreadMain: Le Fine\n");
			return;
		}

		if (queue_socket.flush_ts() < 0) {
			eprintf(E_ERR, EFMT("CQueue::flush_ts(): Failure"));
			continue;
		}
	}
}


CQueueSocketThread::CQueueSocketThread(
	int	sockfd,
	size_t	queueSize
) : CSocketOUT(sockfd, queueSize), CThread(queueSocketThreadMain, this)
{
}

CQueueSocketThread::~CQueueSocketThread()
{
}
