#ifndef __SOCKET_OUT_H__
#define __SOCKET_OUT_H__
////////////////////////////////////////////////////////////////////////////////


#include "CPacket.h"
#include "CBuffer.h"
#include "CQueue.h"

class CSocketOUT : public CQueue {
private:
	int	m__sockfd;
public:
	CSocketOUT(int sockfd, size_t wQueueSize);
	virtual ~CSocketOUT();

public:
	// wPacket 의 내용을 큐에 쓴다.
	//	큐에 충분한 공간이 없을 때, 먼저 flush() 를 호출하여 큐를 비운다.
	static const long	FLUSH__TIMED_OUT;		// 이 간격으로
	static const unsigned	FLUSH__MAX_TRIAL;		// 총 이 횟수만큼 시도한다.
	// Queue 에 있는 내용을 모두 Socket 에 쓴다.
	//	1:= Queue 에 아무것도 없을 때
	//	0:= Queue 가 비워지지 않았을 때
	//	-1:= 문제가 있을 때
	int	flush(void);
	int	flush_ts(void);
};


////////////////////////////////////////////////////////////////////////////////
#endif	// !__SOCKET_OUT_H__
