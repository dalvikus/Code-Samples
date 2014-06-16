#include "CSocketOUT.h"
#include "eprintf.h"
#include <sys/socket.h>
#include <assert.h>
#include <pthread.h>

const unsigned	CSocketOUT::FLUSH__MAX_TRIAL	= 3;
const long	CSocketOUT::FLUSH__TIMED_OUT	= 10;	// microsecond(s)

CSocketOUT::CSocketOUT(
	int	sockfd,
	size_t		wQueueSize
) : CQueue(wQueueSize)
{
	this->m__sockfd = sockfd;
}

CSocketOUT::~CSocketOUT()
{
}


int
CSocketOUT::flush_ts(
	void
)
{
	this->lock();
	int	isEmpty = this->flush();
	this->unlock();
	return isEmpty;
}
int
CSocketOUT::flush(
	void
)
{
	int	isEmpty = 0;

do {
	// Queue 에 있는 내용을 비우려고 시도한다.
	// FLUSH__TIMED_OUT 마이크로초 간격으로 총 FLUSH__MAX_TRIAL 회 시도한다.
	int	n = this->m__sockfd + 1;
	for (unsigned i = 0; i < FLUSH__MAX_TRIAL; ++i) {
		if (!this->getDataSize()) {	// 아무것도 없다.
			isEmpty = 1;
			break;
		}

		fd_set	wfds;
		FD_ZERO(&wfds);
		FD_SET(this->m__sockfd, &wfds);
		struct timeval	tv;
		tv.tv_sec = 0;
		tv.tv_usec = FLUSH__TIMED_OUT;

		if (select(n, NULL, &wfds, NULL, &tv) < 0) {
			eprintf(E_WARN, EFMT("select(): Failure: %m"));
			continue;
		}
		if (!FD_ISSET(this->m__sockfd, &wfds)) {
			// TIMEDOUT
			continue;
		}

		int	isOk;
		do {
			struct	iovec	iov[2];
			int	iovcnt = this->set_iovec(iov);
			ssize_t	nSend = writev(this->m__sockfd, iov, iovcnt);
			if (nSend < 0) {
				eprintf(E_ERR, EFMT("writev(): Failure: %m"));

				isOk = 0;
				break;
			}
//			(void) printf("# of byte(s) sent = %d\n", nSend);
			// Ok

			// head 의 위치(iHead)를 nSend(modulo queueSize) 만큼 증가시킨다.
			if (this->incHead(nSend) < 0) {
				eprintf(E_ERR, EFMT("CQueue::incHead(|%d|): Failure"), nSend);
				isOk = 0;
				break;
			}
			isOk = 1;
		} while (0);
		if (isOk == 0)
			eprintf(E_ERR, EFMT("CQueue::flush(): Failure"));
	}

	isEmpty = this->getDataSize() ? 0 : 1;
} while (0);

	return isEmpty;
}
