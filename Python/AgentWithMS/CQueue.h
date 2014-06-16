#ifndef __QUEUE_H__
#define __QUEUE_H__
////////////////////////////////////////////////////////////////////////////////


#include <stdio.h>	// size_t
#include <sys/uio.h>	// struct iov
#include <pthread.h>
#include <assert.h>
#include "CBuffer.h"

class CQueue {
private:
	char*		m__queue;
	size_t		m__queueSize;
protected:
	size_t		getQueueSize(void)	{return this->m__queueSize;}
public:
	CQueue(size_t queueSize);
	virtual ~CQueue();
private:
	char*		m__head;	// 데이터의 시작을 가리킨다.
					// 큐의 공간 중에서 head 이후의 공간 = m__queueSize - (m__head - m__queue)
	char*		m__tail;	// 데이터의 마지막 다음을 가리킨다.
					// 큐의 공간 중에서 tail 까지의 공간 = (m__tail - m__queue)
	size_t		m__dataSize;	// 데이터의 크기
					// m__tail > m__head 인 경우:
					//	m__dataSize = m__tail - m__head
					// m__tail <= m__head 인 경우:
					//	m__dataSize = m__queueSize - (m__head - m__tail)
protected:
	size_t	getDataSize(void)	{return this->m__dataSize;}

private:
	// CQueue 클래스를 상속받는 클래스는
	// 큐가 더 이상 새로운 패킷을 넣지못할 정도로 가득찼을 때,
	// 어떻게 큐를 비울지를 정해야 한다.
	virtual int	flush(void) = 0;
	virtual int	flush_ts(void) = 0;
private:
	// wPacket 에 있는 내용을 Queue 에 넣는다.
	int	__enqueue(const CBuffer& wPacket);
public:
	int	enqueue(const CBuffer& wPacket);
	int	enqueue_ts(const CBuffer& wPacket);

public:
	// 큐에 있는 데이터를 접근할 때,
	//	1) 모든 데이터를 접근할 수도 있고,
	//	2) 패킷 단위로 접근할 수도 있다.
	// 큐에 있는 데이터의 정보를 iovec 에 설정한다;
	//	struct iov 는 writev 에 정의된 것과 같다.
	//	최대 두 개의 iov가 있을 수 있다.
	//	리턴값이 몇 개의 iov 가 설정되었는 지 말해준다.
	int	set_iovec(struct iovec iov[2]);
	// 큐의 내용을 읽어 rPacket 을 설정한다.
	// 큐에는 패킷단위로 패킷이 쌓이므로 결과값은 0(없다) 아니면 1(있다)이다.
	// 문제가 있을 때에는 -1을 리턴한다.
	// isFetch = True 이면 패킷 내용을 가져온 후 큐의 head 도 이동시킨다.
	// 즉 큐에서 패킷에 해당하는 부분을 삭제한다.
	int	get_rPacket(CBuffer& rPacket, int isFetch);
protected:
	// 큐의 head 를 byAmount 만큼 이동시킨다;
	// byAmount 가 dataSize 보다 클 수 없다.
	int	incHead(size_t byAmount);

	// Thread-safe 버젼
private:
	pthread_mutex_t	m__mutex;
protected:
	void	lock(void)	{assert(pthread_mutex_lock(&this->m__mutex) == 0);}
	void	unlock(void)	{assert(pthread_mutex_unlock(&this->m__mutex) == 0);}
protected:
	int	set_iovec_ts(struct iovec iov[2]);
	int	incHead_ts(size_t byAmount);
public:
	int	get_rPacket_ts(CBuffer& rPacket, int isFetch);
};


////////////////////////////////////////////////////////////////////////////////
#endif	// !__QUEUE_H__
