#include "CDatabaseIN.h"
#include "eprintf.h"
#include <sys/socket.h>
#include <assert.h>
#include <pthread.h>

const size_t	CDatabaseIN::MAX_FETCH	= 10;

CDatabaseIN::CDatabaseIN(
	size_t		wQueueSize
) : CQueue(wQueueSize)
{
}

CDatabaseIN::~CDatabaseIN()
{
}


int
CDatabaseIN::flush_ts(
	void
)
{
	this->lock();
	int	isEmpty = this->flush();
	this->unlock();
	return isEmpty;
}
int
CDatabaseIN::flush(
	void
)
{
	int	isEmpty = 0;

	int	isFetchTrue = 1;
	CBuffer	rPacket;
	for (size_t iFetch = 0; iFetch < MAX_FETCH; ++iFetch) {
		int	nPacket = this->get_rPacket(rPacket, isFetchTrue);
		if (nPacket < 0) {
			eprintf(E_ERR, EFMT("CQueue::get_rPacket(): Failure"));
			isEmpty = -1;
			break;
		}
		if (!nPacket) {
			isEmpty = 1;
			break;
		}

		////////////////////////////////////////////////////////
do {
	CPacket::PACKET_TYPE_T	packetType;
	unsigned		packetSize;
	packetType = CPacket::whichType(rPacket, &packetSize);
	(void) printf("CDatabaseIN::flush(): Type:= |%s|, Packet Size = %d byte(s)\n",
		packetType == CPacket::PACKET_TYPE__ERR ? "PACKET_TYPE__ERR" :
		packetType == CPacket::PACKET_TYPE__PERF ? "PACKET_TYPE__PERF" :
		packetType == CPacket::PACKET_TYPE__ORD ? "PACKET_TYPE__ORD" :
		packetType == CPacket::PACKET_TYPE__RESP ? "PACKET_TYPE__RESP" : "PACKET_TYPE__UNKNOWN"
		,
		packetSize
	);
	if (packetType != CPacket::PACKET_TYPE__RESP) {
		(void) printf("CDatabaseIN::flush(): PACKET_TYPE__RESP Only...\n");
		break;
	}

	int	isPyDict;
	CPacket::RESP_TYPE_T	unpackedRespType;
	unsigned int		unpackedOrderID;
	CBuffer			returnBuffer;
	if (CPacket::unpackResponse(
		rPacket,

		isPyDict,
		unpackedRespType,
		unpackedOrderID,
		returnBuffer
	) < 0) {
		eprintf(E_ERR, EFMT("CPacket::unpackResponse(): Failure"));
		break;
	}
	(void) printf(
		"////////////////////////////////////////////////////////////////////////\n"
		"// CDatabaseIN::flush():\n"
		"\tIs PyDict?: %s\n"
		"\tResponse Type: %s\n"
		"\tOrder ID: %d\n"
		"\tReturn: |%s|(%d)\n"
		"////////////////////////////////////////////////////////////////////////\n"
		,
		returnBuffer.getDataSize() ? (isPyDict ? "True" : "False") : "N/A",
		unpackedRespType == CPacket::RESP_TYPE__ACK ? "ACK" :
		unpackedRespType == CPacket::RESP_TYPE__FAIL ? "FAIL" :
		unpackedRespType == CPacket::RESP_TYPE__OK ? "OK" : "UNKNOWN",
		unpackedOrderID,
		returnBuffer.getDataSize() ? returnBuffer.getBuf() : "N/A",
		returnBuffer.getDataSize() ? returnBuffer.getDataSize() : (unsigned) -1
	);
} while (0);
		////////////////////////////////////////////////////////
	}

	return isEmpty;
}
