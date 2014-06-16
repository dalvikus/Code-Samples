#include "CQueue.h"
#include "CPacket.h"
#include "eprintf.h"
#include <assert.h>	// assert
#include <stdlib.h>	// malloc, free
#include <string.h>	// memcpy
#include <pthread.h>

CQueue::CQueue(
	size_t	queueSize
)
{
	this->m__queueSize = queueSize;
	this->m__queue = (char*) malloc(queueSize); assert(this->m__queue);

	this->m__dataSize = 0;
	this->m__head = this->m__queue;
	this->m__tail = this->m__queue;

	assert(pthread_mutex_init(&this->m__mutex, NULL) == 0);
}

CQueue::~CQueue()
{
	assert(pthread_mutex_destroy(&this->m__mutex) == 0);

	if (this->m__queue) {
		free(this->m__queue);
		this->m__queue = NULL;
	}
}


int
CQueue::__enqueue(
	const CBuffer&	wPacket
)
{
	size_t	bufSize = wPacket.getDataSize();
	const char*	buf = wPacket.getBuf();
	if (bufSize == 0)
		return 0;

	if (bufSize > this->m__queueSize - this->m__dataSize) {
		// 넣을 공간이 부족하다.
		eprintf(E_FATAL, EFMT("Not enough room in Queue: Queue size = %u < Buffer size = %u + Data size = %u"), this->m__queueSize, bufSize, this->m__dataSize);
		return -1;
	}

	size_t	len = bufSize;	// Queue 의 tail 에서부터 head 까지 채울 바이트 수 (tail < head 인 경우)
				// tail >= head 인 경우에는 tail 과 len 값이 변경된다. (아래에서)
	if (this->m__tail >= this->m__head) {
		// 두 부분에 넣을 수 있다.
		// 1) tail 부터 (queueSize - (tail - queue)) 바이트
		// 2) queue 부터 (head - queue) 바이트

		// 먼저 1) 부분에 넣는다.
		size_t	n = bufSize;
		size_t	n1 = this->m__queueSize - (this->m__tail - this->m__queue);
		if (n > n1) {
			len = n - n1;
			n = n1;
		} else {	// 1) 부분에 모두 들어간다.
			len = 0;
		}
		(void*) memcpy(this->m__tail, buf, n);
		this->m__dataSize += n;
		this->m__tail = this->m__queue + ((this->m__tail - this->m__queue) + n) % this->m__queueSize;
		buf += n;

		if (!len)
			return 0;
	}
	// tail 부터 len 바이트를 복사한다.
	(void*) memcpy(this->m__tail, buf, len);
	this->m__tail += len;
	this->m__dataSize += len;

	return 0;
}

int
CQueue::enqueue_ts(
	const CBuffer&	wPacket
)
{
	this->lock();
	int	iReturnVal = this->enqueue(wPacket);
	this->unlock();
	return iReturnVal;
}
int
CQueue::enqueue(
	const CBuffer&	wPacket
)
{
	int	iReturnVal = 0;

do {
	size_t	bufSize = wPacket.getDataSize();
	if (bufSize > this->getQueueSize()) {
		eprintf(E_FATAL, EFMT("CQueue::enqueue: Packet size(%u) too large(> queue size = %u)"), bufSize, this->getQueueSize());
		iReturnVal = -1;
		break;
	}

	if (this->getQueueSize() >= this->getDataSize() + bufSize) {	// 충분한 공간이 있다.
		// 큐에 쌓는 것으로 충분한다.
		iReturnVal = this->__enqueue(wPacket);
		break;
	}

	if (this->flush() < 0) {
		eprintf(E_ERR, EFMT("CQueue::flush(): Failure\n"));
		iReturnVal = -1;
		break;
	}

	iReturnVal = this->__enqueue(wPacket);
} while (0);

	return iReturnVal;
}

int
CQueue::set_iovec(
	struct iovec	iov[2]
)
{
	int	iovcnt;
	if (this->m__tail > this->m__head) {
		iov[0].iov_base = this->m__head;
		iov[0].iov_len = this->m__tail - this->m__head;
		iovcnt = 1;
	} else {
		iov[0].iov_base = this->m__head;
		iov[0].iov_len = this->m__queueSize - (this->m__head - this->m__queue);
		iov[1].iov_base = this->m__queue;
		iov[1].iov_len = this->m__tail - this->m__queue;
		iovcnt = 2;
	}

	return iovcnt;
}
int
CQueue::get_rPacket(
	CBuffer&	rPacket,
	int		isFetch
)
{
	int	iReturnVal = 0;
	(void*) rPacket.setDataSize(0);

do {
	if (!this->m__dataSize) {	// 큐에 아무런 것도 없다.
		iReturnVal = 0;
		break;
	}

	// 큐에 적어도 하나의 패킷이 있다.
	if (!rPacket.setBufSizeAtLeast(4)) {
		eprintf(E_ERR, EFMT("CBuffer::setBufSizeAtLeast(4): Failure"));
		iReturnVal = -1;
		break;
	}
	CPacket::BYTE*	pByte = (CPacket::BYTE*) rPacket.getBuf();	// 읽어들인 처음 위치
	// 처음 4바이트를 읽어 패킷의 크기가 얼마인지 알아낸다.
	char*	p = this->m__head;
	int	n = this->m__queueSize - (p - this->m__queue);	// p 이후로 큐에 있는 바이트 수
	int	i = 0;
	if (n < 4) {
		for (; i < n; ++i)
			*pByte++ = *p++;
		p = this->m__queue;
	}
	for (; i < 4; ++i)
		*pByte++ = *p++;

	// 읽어들인 처음 위치로 간다.
	pByte -= 4;
	unsigned	packetSize;
	loadInteger(&pByte, 3, packetSize);
	if (!rPacket.setBufSizeAtLeast(4 + packetSize)) {
		eprintf(E_ERR, EFMT("CBuffer::setBufSizeAtLeast(4 + %u): Failure"), packetSize);
		iReturnVal = -1;
		break;
	}

	// 다음으로 읽어들일 위치로 이동한다.
	pByte = (CPacket::BYTE*) rPacket.getBuf() + 4;

	n = this->m__queueSize - (p - this->m__queue);	// p 이후로 큐에 있는 바이트 수
	if ((size_t) n < packetSize) {
		(void*) memcpy(pByte, p, n);

		packetSize -= n;
		pByte += n;
		p = this->m__queue;
	}
	(void*) memcpy(pByte, p, packetSize);

	if (isFetch) {
		// head 의 위치를 변경한다.
		n = this->m__queueSize - (this->m__head - this->m__queue);	// head 이후로 큐에 있는 바이트 수
		if ((size_t) n < 4 + packetSize)
			this->m__head = this->m__queue + (4 + packetSize - n);
		else
			this->m__head += 4 + packetSize;
		this->m__dataSize -= 4 + packetSize;
	}
	rPacket.setDataSize(4 + packetSize);
	iReturnVal = 1;
} while (0);

	return iReturnVal;
}

int
CQueue::incHead(
	size_t	byAmount
)
{
	if (byAmount > this->m__dataSize)
		return -1;
	size_t	n = this->m__queueSize - (this->m__head - this->m__queue);	// head 이후로 큐에 있는 바이트 수
	if (n < byAmount)
		this->m__head = this->m__queue + (byAmount - n);
	else
		this->m__head += byAmount;
	this->m__dataSize -= byAmount;
	return 0;
}

int
CQueue::set_iovec_ts(
	struct iovec	iov[2]
)
{
	this->lock();
	int	iovcnt = this->set_iovec(iov);
	this->unlock();
	return iovcnt;
}
int
CQueue::get_rPacket_ts(
	CBuffer&	rPacket,
	int		isFetch
)
{
	this->lock();
	int	iReturnVal = this->get_rPacket(rPacket, isFetch);
	this->unlock();
	return iReturnVal;
}

int
CQueue::incHead_ts(
	size_t	byAmount
)
{
	this->lock();
	int	iReturnVal = this->incHead(byAmount);
	this->unlock();
	return iReturnVal;
}
