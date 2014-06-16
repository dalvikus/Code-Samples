#ifndef __JOB_DOWN_PACKET_H__
#define __JOB_DOWN_PACKET_H__
////////////////////////////////////////////////////////////////////////////////


#include <Python.h>
#include "CBuffer.h"

class CJobDownPacket {
public:
/*
        1 1 1 1 1 1 1 1
        ========= =====
            |       |
            |       +-- 단계 비트: 어느 단계인지를 말해준다. (총 8단계)
            +---------- 사용하지 않는다.
 */
#define BYTE_JOB_DOWN__STAGE_SHIFT	0
#define BYTE_JOB_DOWN__STAGE_MASK	(0x07 << BYTE_JOB_DOWN__STAGE_SHIFT)
// Job 을 내려받는 패킷이 어느 단계에 해당하는 것인지...
typedef enum  {
	JOB_DOWN_STAGE_PACKET_TYPE__SCRIPT	= 0,	// JOB_DOWN_STAGE__SCRIPT 에 해당
	JOB_DOWN_STAGE_PACKET_TYPE__BYTE_STREAM,	// JOB_DOWN_STAGE__BYTE_STREAM
	JOB_DOWN_STAGE_PACKET_TYPE__COMMAND,		// JOB_DOWN_STAGE__COMMAND
	JOB_DOWN_STAGE_PACKET_TYPE__JOB,		// JOB_DOWN_STAGE__JOB
} JOB_DOWN_STAGE_PACKET_T;

	// CPacket 에 정의된 "명령패킷"을 사용한다.
	// ORD_TYPE_T 로는 ORD_TYPE__JOB_DOWN 을 사용하고,
	// JobID 를 ORD__ID 로 사용한다.
	// XXX__ID 부분에 JOB_DOWN_STAGE_PACKET_T 을 써서 어느 단계인 지를 말해준다.
	// 나머지는 CPacket::packOrder 와 유사하다.
	static int	pyPack(
		CBuffer&	packet,		// 여기에 넣는다.

		const PyObject*	jobID,
		JOB_DOWN_STAGE_PACKET_T	jobDownStage,

		int	isBinary,		// what 에 포
		const PyObject*	what,		// <type 'str'>
		const PyObject*	extraWhat	// <type 'str'>
	);
};


////////////////////////////////////////////////////////////////////////////////
#endif	// !__JOB_DOWN_PACKET_H__
