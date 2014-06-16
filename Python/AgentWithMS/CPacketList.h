#ifndef __PACKET_LIST_H__
#define __PACKET_LIST_H__
////////////////////////////////////////////////////////////////////////////////


#include <Python.h>
#include "CPacket.h"
#include <zlib.h>
#include "CBuffer.h"


/*
 * 다음은 CPacketList 의 사용 예이다.
#if 0
{
	PyObject*	x = Py_BuildValue(
		"["
			"[i, i, i, i, s, s], "
			"[i, i, i, i], "
			"[i, i, i, s], "
			"[i, i, f]"
#if 0
			"[i, i, f]"
			"[i, i, f]"
			"[i, i, f]"
			"[i, i, f]"
#endif
			"[i, i, s, s]"
		"]"
		,
		CPacket::PACKET_TYPE__ERR, 1024, 342342, CPacket::SEV__FATAL, "25", "Detail...: hello, world",
		CPacket::PACKET_TYPE__ERR, 9999, 516, CPacket::SEV__CLEAR,
		CPacket::PACKET_TYPE__PERF, 145, 25, "Extra...",
		CPacket::PACKET_TYPE__PERF, 152053, 7.56,
#if 0
		CPacket::PACKET_TYPE__PERF, 2, 7.500006,
		CPacket::PACKET_TYPE__PERF, 2, .50,
		CPacket::PACKET_TYPE__PERF, 2, .0050,
		CPacket::PACKET_TYPE__PERF, 2, .00050,
#endif
		CPacket::PACKET_TYPE__PERF, 2, "hello, world: high", "hello, world...hello, world"
	);
	(void) printf("x(%p) = |", x);
	(void) PyObject_Print(x, stdout, 0);
	(void) printf("\n");

	CPacketList	packetList;
	int	NPacket = packetList.pyPack(x, 1024);
	if (NPacket < 0) {
		(void) fprintf(stderr, "CPacketList::pack: Failure\n");
	} else {
		const CBuffer**	packetListPtr = packetList.getPacketList();
		for (int i = 0; i < NPacket; ++i) {
			const CBuffer&	packet = *packetListPtr[i];
			CPacket::PACKET_TYPE_T	packetType = CPacket::whichType(packet);
if (packetType == CPacket::PACKET_TYPE__ERR) {
	unsigned int	jobID;
	unsigned int	eventID;
	unsigned int	severity;
	CBuffer		strErrValue;
	int		isDetail;
	CBuffer		strErrDetail;
	if (CPacket::unpackErrorData(
		packet,
		jobID, eventID, severity, strErrValue, isDetail, strErrDetail
	) < 0) {
#ifdef __TRACE__
		(void) fprintf(stderr,
			"__TRACE__: CPacketList::pack: index = %d: CPacket::unpackErrorData(): Failure\n"
			,
			index
		);
#endif	// __TRACE__
		return 1;
	}
	(void) printf(
		"JOB_ID = %d, EVENT_ID = %d, "
		"Severity = %s, "
		"Error Value:= |%s|, "
		"Error Details:= |%s|\n"
		,
		jobID, eventID,
		severity == CPacket::SEV__CLEAR ? "SEV__CLEAR" : (
		severity == CPacket::SEV__FATAL ? "SEV__FATAL" : (
		severity == CPacket::SEV__CRITICAL ? "SEV__CRITICAL" : (
		severity == CPacket::SEV__WARNING ? "SEV__WARNING" : (
		severity == CPacket::SEV__ATTENTION ? "SEV__ATTENTION" : "SEV__INFORMATION")))),
		strErrValue.getBuf(),
		isDetail ? strErrDetail.getBuf() : "None"
	);
} else if (packetType == CPacket::PACKET_TYPE__PERF) {
	unsigned int	jobID;
	CPacket::TYPE_VALUE_T	typeValue;
	int		isExtra;
	CBuffer		strPerfExtra;
	if (CPacket::unpackPerfData(packet, jobID, typeValue, isExtra, strPerfExtra) < 0) {
#ifdef __TRACE__
		(void) fprintf(stderr, "__TRACE__: CPacket::unpackPerfData(): Failure\n");
#endif	// __TRACE__

		return 1;
	}
	char	strValue[64];
	if (typeValue.type == CPacket::VAL_TYPE__INTERGER) {
		(void) snprintf(strValue, 64, "%d<INT>", typeValue.value.ival);
	} else if (typeValue.type == CPacket::VAL_TYPE__FIXED_POINT) {
		(void) snprintf(strValue, 64, "%.3f<FIXED_POINT>(%d, %d)",
			typeValue.value.fixedPoint.fval,
			typeValue.value.fixedPoint.iFixed, typeValue.value.fixedPoint.iPoint
		);
	} else {	// CPacket::VAL_TYPE__STRING
		(void) snprintf(strValue, 64, "\"%s\"<STR>", typeValue.buffer.getBuf());
	}
	const CPacket::VAL_TYPE&	type = typeValue.type;
	(void) printf(
		"JOB_ID = %d, Type:= %s, Value: %s, Extra:= |%s|\n"
		,
		jobID,
		type == CPacket::VAL_TYPE__INTERGER ? "VAL_TYPE__INTERGER" : (
		type == CPacket::VAL_TYPE__FIXED_POINT ? "VAL_TYPE__FIXED_POINT" : "VAL_TYPE__STRING"),
		strValue,
		isExtra ? strPerfExtra.getBuf() : "None"
	);
}
		}
	}

	Py_XDECREF(x);
}
#endif
 */

class CPacketList {
private:
	// 주어진 "데이터"로부터 패킷을 만든다.
	// 패킷의 최대 크기가 정해져있으므로, 여러 개의 패킷을 만들 수 있다.
	// 한 번 만들어진 패킷의 공간은 다음을 위해서 그대로 둔다.
	// 물론 기존의 패킷 공간을 이용할 때는 그 크기를 0으로 해서 초기화해야 한다.
	int		m__indexPacket;	// m__packetList 중에서 사용 가능한 패킷 공간에 대한 첨자
					// CPacket::pack() 함수의 시작에서 -1으로 최기화한다.
					// 패킷이 필요한 시점에서 m__indexPacket 값을 1 증가시켜,
					// 그에 맞는 패킷 공간을 찾는다.
					// 증가하는 m__indexPacket 이 m__packetListSize 과 같을 때,
					// 새로운 패킷 공간을 할당받고 m__packetListSize 은 1 증가시킨다.
					// incPacket() 함수를 사용한다.
	size_t		m__packetListSize;	// 할당된 총 패킷 공간의 갯수
	CBuffer**	m__packetList;
	int		incPacket(void);	// 패킷 공간의 크기를 일정하게(PACKET_INC_UNIT) 증가시킨다.
public:
	size_t		getNPacket(void)		{return 1 + this->m__indexPacket;}
	const CBuffer**	getPacketList(void) const	{return const_cast<const CBuffer**>(m__packetList);}
	// 다음과 같이 사용한다.
#if 0
	int	packetNumber = CPacketList::getPacketList();
	if (packetNumber > 0) {		// 만들어진 패킷이 없다면 0을, 패킷을 만들 때 문제가 생겼다면 음수를 리턴한다.
		const CBuffer**	pPacketList = CPacketList::getPacketList();
		for (int i = 0; i < packetNumber; ++i) {
			size_t		len = pPacketList[i]->getDataSize();
			const BYTE*	pByte = pPacketList[i]->getBuf();
			// 현재 패킷의 내용은 pByte 가 가리키고 있고,
			// 거기서부터 len 개의 바이트가 그 내용이다.
		}
	}
#endif

public:
	CPacketList(void);
	~CPacketList();

public:
	// 성능데이터:= [[PACKET_TYPE_T, JOB_ID, 성능값, "추가내용"], ...]
	// 장애데이터:= [[PACKET_TYPE_T, JOB_ID, 심각도, "장애값", "상세설명"], ...]
	//	심각도: 최대 7단계까지 설정할 수 있다. (현재는 5단계)
	//		0(SEV__FATAL)
	//		1(SEV__CRITICAL)
	//		2(SEV__WARNING)
	//		3(SEV__ATTENTION)
	//		4(SEV__INFORMATION)
	// 한 패킷에 두 데이터가 섞여 있을 수 있으므로, 제일 앞에 PACKET_TYPE_T 값(PACKET_TYPE__ERR, PACKET_TYPE__PERF)을 넣는다.
	//	[
	//		[PACKET_TYPE__ERR, JOB_ID, EVENT_ID, 심각도, "장애값", "상세설명"],
	//		[PACKET_TYPE__PERF, JOB_ID, 성능값, "추가내용"],
	//		......
	//	]

	// "데이터"을 담은 dataList 를 이용해서 패킷을 만든다.
	// 가능한 최대 패킷은 packetSizeUpperBound 이다.
	// 이 크기는 적어도 성능값이나 "장애값"은 담을 수 있을 정도이어야 한다. (몇 십 바이트 정도)
	// 문제가 있을 때에는 음수를 리턴한다.
	// 정상적일 때 만들어진 패킷의 갯수를 리턴한다.
	int	pyPack(const PyObject* dataList, size_t packetSizeUpperBound);
};


////////////////////////////////////////////////////////////////////////////////
#endif	// !__PACKET_LIST_H__
