#ifndef __DATABASE_IN_H__
#define __DATABASE_IN_H__
////////////////////////////////////////////////////////////////////////////////


#include "CPacket.h"
#include "CBuffer.h"
#include "CQueue.h"

class CDatabaseIN : public CQueue {
public:
	CDatabaseIN(size_t wQueueSize);
	virtual ~CDatabaseIN();

public:
	// Queue 에 있는 내용을 모두 Socket 에 쓴다.
	//	1:= Queue 에 아무것도 없을 때
	//	0:= Queue 가 비워지지 않았을 때
	//	-1:= 문제가 있을 때
	// 최대 MAX_FETCH 까지 가져온다.
	static const size_t	MAX_FETCH;
	int	flush(void);
	int	flush_ts(void);
};


////////////////////////////////////////////////////////////////////////////////
#endif	// !__DATABASE_IN_H__
