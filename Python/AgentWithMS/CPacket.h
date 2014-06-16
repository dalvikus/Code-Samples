#ifndef __BYTE_H__
#define __BYTE_H__
////////////////////////////////////////////////////////////////////////////////


#include <zlib.h>	// Bytef
#include "CBuffer.h"


/*
 * 다음은 CPacket 의 사용 예이다.
#if 0
{
	CBuffer	packet;
	CPacket::ORD_TYPE_T	orderType = CPacket::ORD_TYPE__DISCOVERY;
	unsigned int		orderID = 907;
	unsigned int		xxxID = 777;
	unsigned int*		xxxIDPtr = &xxxID;
	int			isBinary = 1;
	const char*		argPtr = "";
	size_t			argLen = argPtr ? strlen(argPtr) + 1 : (size_t) -1;	// NUL 도 포함한다.
	const char*		extraArgPtr = NULL;//"hello, world";
	if (CPacket::packOrder(
		packet,

		orderType,
		orderID,
		xxxIDPtr,
		isBinary,
		argPtr, argLen,
		extraArgPtr
	) < 0) {
		(void) fprintf(stderr,
			"ERROR: "
			"__FILE__:= |%s|, __LILNE_NO__ = %d: "
			"CPacket::packOrder(): Failure\n"
			,
			__FILE__, __LINE__
		);

		return 1;
	}

	CPacket::ORD_TYPE_T	unpackedOrderType;
	unsigned int		unpackedOrderID;
	unsigned int		unpackedXxxID;
	unsigned int*		unpackedXxxIDPtr = &unpackedXxxID;
	int			unpackedIsBinary;
	CBuffer			unpackedArgBuffer;
	CBuffer			unpackedExtraArgBuffer;
	if (CPacket::unpackOrder(
		packet,

		unpackedOrderType,
		unpackedOrderID,
		&unpackedXxxIDPtr,
		unpackedIsBinary,
		unpackedArgBuffer,
		unpackedExtraArgBuffer
	) < 0) {
		(void) fprintf(stderr,
			"ERROR: "
			"__FILE__:= |%s|, __LILNE_NO__ = %d: "
			"CPacket::unpackOrder(): Failure\n"
			,
			__FILE__, __LINE__
		);

		return 1;
	}

	(void) printf(
		"////////////////////////////////////////////////////////////////////////\n"
		"Order Type: %d <-> %d\n"
		"Order ID: %d <-> %d\n"
		"XXX__ID: %d <-> %d\n"
		"isBinary: %s <-> %s\n"
		"Argument: %d <-> %d\n"
		"Extra-argument: %d <-> %d\n"
		"////////////////////////////////////////////////////////////////////////\n"
		,
		orderType, unpackedOrderType,
		orderID, unpackedOrderID,
		xxxIDPtr ? *xxxIDPtr : -1, unpackedXxxIDPtr ? *unpackedXxxIDPtr : -1,
		isBinary ? "True" : "False", unpackedIsBinary ? "True" : "False",
		argLen, unpackedArgBuffer.getDataSize(),
		extraArgPtr ? strlen(extraArgPtr) + 1 : -1,
		unpackedExtraArgBuffer.getDataSize() ? unpackedExtraArgBuffer.getDataSize() : -1
	);
}
{
	CBuffer	packet;
	int			isPyDict = 0;
	CPacket::RESP_TYPE_T	respType = CPacket::RESP_TYPE__FAIL;
	unsigned int		orderID = 504;
	const char*		returnPtr = "hello, world";
	if (CPacket::packResponse(
		packet,

		isPyDict,
		respType,
		orderID,
		returnPtr
	) < 0) {
		(void) fprintf(stderr,
			"ERROR: "
			"__FILE__:= |%s|, __LILNE_NO__ = %d: "
			"CPacket::packResponse(): Failure\n"
			,
			__FILE__, __LINE__
		);

		return 1;
	}

	int			unpackedIsPyDict;
	CPacket::RESP_TYPE_T	unpackedRespType;
	unsigned int		unpackedOrderID;
	CBuffer			returnBuffer;
	if (CPacket::unpackResponse(
		packet,

		unpackedIsPyDict,
		unpackedRespType,
		unpackedOrderID,
		returnBuffer
	) < 0) {
		(void) fprintf(stderr,
			"ERROR: "
			"__FILE__:= |%s|, __LILNE_NO__ = %d: "
			"CPacket::unpackResponse(): Failure\n"
			,
			__FILE__, __LINE__
		);

		return 1;
	}
	(void) printf(
		"////////////////////////////////////////////////////////////////////////\n"
		"Response Type: %d <-> %d\n"
		"Order ID: %d <-> %d\n"
		"Return: %d <-> %d\n"
		"////////////////////////////////////////////////////////////////////////\n"
		,
		respType, unpackedRespType,
		orderID, unpackedOrderID,
		returnPtr ? strlen(returnPtr) + 1 : -1,
		returnBuffer.getDataSize() ? returnBuffer.getDataSize() : -1
	);
}
#endif
 */

class CPacket {
public:
typedef unsigned char	BYTE;	// 부호가 있으면 바이트를 워드나
				// 그 이상으로 확장할 때 비트가 원하는 곳 외에도 설정된다.
				// 예를 들어, 0x80을 4바이트로 변환할 때, 원하는 것은
				// 0x00000080이다. 부호가 있는 것으로 생각하면 0xFFFFFF80이 된다.
typedef enum {
	VAL_TYPE__INTERGER,	// 정수
	VAL_TYPE__FIXED_POINT,	// 유한 소수
	VAL_TYPE__STRING,	// NUL 로 끝나는 문자열
	VAL_TYPE__STREAM,	// 압축된 바이트스트림
} VAL_TYPE;
typedef struct {
	VAL_TYPE	type;
	CBuffer		buffer;
	union {
		int	ival;
		struct {
			float	fval;
			int	iFixed;
			int	iPoint;
		} fixedPoint;
		const char*	str;
		struct {
			size_t		len;
			const Bytef*	buf;
		} byteStream;
	} value;
} TYPE_VALUE_T;

typedef enum {
	PACKET_TYPE__INVALID	= -1,
	PACKET_TYPE__UNKNOWN	= 0,
	PACKET_TYPE__ERR	= 1,
	PACKET_TYPE__PERF	= 2,
	PACKET_TYPE__ORD	= 3,
	PACKET_TYPE__RESP	= 4
} PACKET_TYPE_T;

typedef enum {
	SEV__CLEAR		= 0,	// 제일 작은 값
	SEV__FATAL		= 1,	// 연속적으로...
	SEV__CRITICAL		= 2,
	SEV__WARNING		= 3,
	SEV__ATTENTION		= 4,
	SEV__INFORMATION	= 5,	// 제일 큰 값
} SEVERITY_T;

/*
바이트 헤더의 "장애바이트 비트"의 설정여부에 따라 주어진 "바이트 스트림"은
"성능(DATA)바이트 스트림"이 되거나 "장애바이트 스트림"이 된다.
	바이트 스트림 = 성능바이트 스트림 | 장애바이트 스트림

"성능바이트 스트림"은 다음과 같이 구성된다.

        성능바이트:= 성능바이트 헤더 + JOB_ID + [압축/바이너리 헤더 +[ 압축 크기] + ]성능값[ + 압축 크기 + 추가내용]

"성능바이트 헤터"에는 성능값이 일반 문자열인지 아니면 압축되거나 바이너리화된 값인지를 말해주는
"압축/바이너리 비트"가 있다. 압축되었다면 다음 바이트에 압축된 크기를 말하는 4바이트가 이어진다.
추가내용이 있을 경우에는 "추가내용 비트"를 설정한다.
이 때 추가내용은 항상 압축된다.

"장애바이트 스트림"은 다음과 같이 구성된다.

        장애바이트:= 장애바이트 헤더 + JOB_ID + 이벤트 헤더 + EVENT_ID + [압축 크기 +]장애값[ + 압축 크기 + 상세설명]

"장애(ERROR)바이트 헤더"에는 "심각도"를 말하는 8단계에 대한 정보가 있다.
또한 이어지는 장애값이 압축되었는 지를 말해주는 "압축 비트"와
상세설명이 있는 지를 말하는 "상세설명 비트"도 있다.
상세설명은 항상 압축된다.

    *: 어느 경우든 "바이트 헤더" 다음에는 JOB_ID를 말해주는 바이트가 온다.
    몇 바이트를 차지하는 지는 "바이트 헤더"의 "JOB_ID 크기 비트"를 보면 된다.

"추가내용"이나 "상세설명"이 정해진 패킷의 크기보다 클 수가 있다.
그런 경우는 다음과 같이 처리한다.

    첫 번째 내용을 보낼 때:
	"압축 헤더"의 "연속(/마지막) 비트"를 설정해서 지금 보내는 데이터가 전부가 아님을 말해준다.
	즉 이런 데이터를 받은 서버는 "추가내용"이나 "상세설명"이 더 있다는 것을 인식한다.
    두 번째 이후에 보낼 때:
	바이트 헤더의 최하위 세 개의 비트를 모두 설정한 후,
	JOB_ID 다음에 이어지는 추가설명이나 상세설명을 보낸다.
	마지막 패킷임을 표시하기 위해 압축 헤더의 "(연속/)마지막 비트"를 설정한다.

	추가바이트:= 바이트 헤더 + JOB_ID + 압축 헤더 + 추가내용/상세설명

    #: 연속적인 데이터의 무결성을 확신하기 위하여 "압축 헤더"의 특정 비트를 설정하고,
    "압축 헤더"의 바로 다음에 무결성을 확인할 수 있는 데이터(예를 들어, 일련번호)를 넣을 수도 있다.

1. 바이트 헤더
        1 ? ? ? ? ? ? ?
        = =============
        |       |
        |       +------ 장애바이트 비트의 설정여부에 따라 해석이 달라진다.
        |               아래의 "성능바이트 헤더", "장애바이트 헤더"의 내용을 참고한다.
        +-------------- 장애바이트 비트: 바이트가 장애바이트인지를 말해준다.
 */
#define BYTE__ERROR	0x40
/*

    1.1. 성능바이트 헤더
        o x 1 1 1 1 1 -
        = = === = = = =
        | |  |  | | | |
        | |  |  | | | +-------- 사용하지 않는다.
        | |  |  | | +---------- 연속 비트: 이 비트가 설정되어있다면 JOB_ID 뒤에 오는 데이터는
        | |  |  | |             이전 패킷에서 미쳐 보내지 못한 추가내용이 있음을 뜻한다.
        | |  |  | +------------ 추가내용 비트: 추가내용이 있는 지 말해준다.
        | |  |  |               추가내용은 반드시 압축한다.
        | |  |  +-------------- 압축/바이너리 헤더 비트: 성능값에 앞서 "압축/바이너리 헤더"가 있는 지 말해준다.
        | |  +----------------- JOB_ID 크기 비트: JOB_ID 가 몇 바이트의 양수인지 말해준다.
        | |                     00: 한 바이트, 01: 두 바이트, 10: 세 바이트, 11: 네 바이트
        | +-------------------- 장애데이터 비트: 성능데이터이므로 이 비트는 설정되지 않는다.
        +---------------------- 성능/장애데이터 비트: 성능데이터 또는 장이데이터이다.
 */
#define BYTE_PERF__CONTINUE		0x02
#define BYTE_PERF__EXTRA		0x04
#define BYTE_PERF__ZB			0x08
#define BYTE_PERF__JOB_ID_SIZE_SHIFT	4
#define BYTE_PERF__JOB_ID_SIZE_MASK	(0x03 << BYTE_PERF__JOB_ID_SIZE_SHIFT)
/*

    1.2. 장애바이트 헤더
        o o 1 1 1 1 1 -
        = = === = = = =
        | |  |  | | | |
        | |  |  | | | +-------- 사용하지 않는다.
        | |  |  | | +---------- 연속 비트: 이 비트가 설정되어있다면 JOB_ID 뒤에 오는 데이터는
        | |  |  | |             이전 패킷에서 미쳐 보내지 못한 추가내용이 있음을 뜻한다.
        | |  |  | +------------ 상세내용 비트: 상세내용이 있는 지를 말해준다.
        | |  |  |               상세내용은 항상 압축된다.
        | |  |  +-------------- 압축 비트: 장애값이 압축되었는 지를 말해준다.
        | |  |                  장애값은 항상 문자열이다.
        | |  |                  여러 개의 값이 있을 때는 특별한 구분자(예를 들어 ',')를 사용한다.
        | |  |                  장애값은 이를 보여주는 웹콘솔에서 처리한다.
        | |  +----------------- JOB_ID 크기 비트: JOB_ID 가 몇 바이트의 양수인지 말해준다.
        | |                     00: 한 바이트, 01: 두 바이트, 10: 세 바이트, 11: 네 바이트
        | +-------------------- 장애데이터 비트: 장애데이터이므로 이 비트는 설정된다.
        +---------------------- 성능/장애데이터 비트: 성능데이터 또는 장이데이터이다.
 */
#define BYTE_ERROR__CONTINUE		0x02
#define BYTE_ERROR__DETAIL		0x04
#define BYTE_ERROR__ZIPPED		0x08
#define BYTE_ERROR__JOB_ID_SIZE_SHIFT	4
#define BYTE_ERROR__JOB_ID_SIZE_MASK	(0x03 << BYTE_ERROR__JOB_ID_SIZE_SHIFT)
/*
2. 이벤트 헤더
        1 1 1 1 1 1 1 1
        === ===== =====
         |    |     |
         |    |     +--- 사용하지 않는다.
         |    +--------- 심각도 비트: 총 8단계의 장애단계를 표시할 수 있다.
         |               0: CLEAR
         |               1: FATAL
         |               2: CRITICAL
         |               3: WARNING
         |               4: ATTENTION
         |               5: INFORMATION
         +-------------- EVENT_ID 크기 비트: EVENT_ID 가 몇 바이트의 양수인지 말해준다.
                         00: 한 바이트, 01: 두 바이트, 10: 세 바이트, 11: 네 바이트
 */
#define BYTE_EVENT__SIZE_SHIFT		6
#define BYTE_EVENT__SIZE_MASK		(0x03 << BYTE_EVENT__SIZE_SHIFT)
#define BYTE_EVENT__SEVERITY_SHIFT	3
#define BYTE_EVENT__SEVERITY_MASK	(0x07 << BYTE_EVENT__SEVERITY_SHIFT)
/*

3. 압축/바이너리 헤더
        1 ? ? ? ? ? ? ?
        = =============
        |       |
        |       +------ 압축 비트의 설정여부에 따라 해석이 달라진다.
        +-------------- 압축 비트: 압축되었는 지 또는 바이너리화되었는 지를 말해준다.
 */
#define BYTE_ZB__ZIPPED			0x80
/*

    3.1. 압축비트가 설정된 경우
        o - - - - - - -
        = =============
        |       |
        |       +------- 사용하지 않는다.
        +--------------- 압축 비트가 설정되었으므로 값은 압축되었다.

    3.2. 압축비트가 설정되지 않은 경우
        x ! 1 1 1 - - -
        = === === =====
        |  |   |    |
        |  |   |    +-- 사용하지 않는다.
        |  |   +------- 소수점 비트: 소숫점 표현을 말한다.
        |  |            00: 소숫점이 없다.
        |  |            01: 소숫점 한 자리 즉 실제 값은 여기서 표현하는 정수의 10분의 1이다.
        |  |            10: 소숫점 두 자리 즉 실제 값은 여기서 표현하는 정수의 100분의 1이다.
        |  |            11: 소숫점 세 자리 즉 실제 값은 여기서 표현하는 정수의 1000분의 1이다.
        |  +----------- 크기 비트: 몇 비트를 사용해서 정수를 표현했는 지를 말해준다.
        |               00: 한 바이트, 01: 두 바이트, 10: 세 바이트, 11: 네 바이트
        |
        |
        +-------------- 압축 비트가 설정되지 않았으므로 값은 바이너리화되었음을 뜻한다.
 */
#define BYTE_ZB_BINARY__POINT_SHIFT	3
#define BYTE_ZB_BINARY__POINT_MASK	(0x03 << BYTE_ZB_BINARY__POINT_SHIFT)
#define BYTE_ZB_BINARY__SIZE_SHIFT	5
#define BYTE_ZB_BINARY__SIZE_MASK	(0x03 << BYTE_ZB_BINARY__SIZE_SHIFT)
/*
 */

public:
	// 압축하려는 바이트들의 갯수가 적어도 이 보다는 커야 압축효과가 있다.
	static unsigned int	ZIPPED_STRING_LENGTH_LOWER_BOUND;
public:
	// 장애데이터를 패킹해서 packet 버퍼에 넣는다.
	// 문제가 생기면 음수를 리턴한다.
	// 여기서 만든 패킷의 크기가 packetSizeUpperBound 보다 더 클 경우,
	// 새로운 패킷을 만든다.
	static int
	packErrorData(
		CBuffer&	packet,			// [OUT]

		int		jobID,
		int		eventID,
		int		severity,
		const char*	strErrValue,
		const char*	strErrDetail,

		size_t		packetSizeUpperBound
	);
	// packet 버퍼에 패킹된 장애데이터로부터 필요한 정보를 얻는다.
	static int	unpackErrorData(
		const CBuffer&	packet,

		unsigned int&	jobID,			// [OUT]
		unsigned int&	eventID,		// [OUT]
		unsigned int&	severity,		// [OUT]
		CBuffer&	strErrValue,		// [OUT]
		int&		isDetail,		// [OUT]
		CBuffer&	strErrDetail		// [OUT]
	);

	// 성능데이터를 패킹해서 packet 버퍼에 넣는다.
	// 문제가 생기면 음수를 리턴한다.
	// 여기서 만든 패킷의 크기가 packetSizeUpperBound 보다 더 클 경우,
	// 새로운 패킷을 만든다.
	static int
	packPerfData(
		CBuffer&	packet,			// [OUT]

		int		jobID,
		const TYPE_VALUE_T&	typeValue,
		const char*	strPerfExtra,

		size_t		packetSizeUpperBound
	);
	// packet 버퍼에 패킹된 성능데이터로부터 필요한 정보를 얻는다.
	static int
	unpackPerfData(
		const CBuffer&	packet,

		unsigned int&	jobID,
		TYPE_VALUE_T&	typeValue,

		int&		isExtra,
		CBuffer&	strPerfExtra
	);

public:
	// 패킹된 packet 이 어떤 유형인지 말해준다.
	// 5번째 바이트의 최상위비트를 보면 알 수 있다.
	// 설정되어있으면 PACKET_TYPE__ERR 그렇지 않다면 PACKET_TYPE__PERF 이다.
	// 처음 4바이트는 전체 패킷의 길이를 말하므로,
	// 최소한으로 일치하는 지 확인한다.
	// 그렇지 않으면 PACKET_TYPE__INVALID 를 리턴한다.
	// pPacketSize 가 NULL 이 아니라면, pPacketSize 가 가리키는 곳에 packet 의 길이를 쓴다.
	static PACKET_TYPE_T	whichType(const CBuffer& packet, unsigned int* pPacketSize = NULL);

/*
2. 패킷 정의
기존의 데이터(성능, 장애) 패킷 외에
M/S 와 Agent 사이에 사용되는 통신 패킷("명령(Order)", "응답(Response)")을 정의한다.

기존의 성능/장애바이트 헤더에서는 최하위 두 비트를 사용하지 않았다.
그 두 비트를 이용하여 기존의 데이터 패킷과 통신 패킷을 구별한다.
최하위 두 비트 대신에 최상위 두 비트를 사용한다. (기존의 성능/장애바이트 헤더 수정)

명령패킷과 응답패킷은 다음 구조를 가진다.

	명령패킷:= 명령바이트 헤더 + 명령유형 헤더 + ORDER__ID[ + XXX__ID][ +[압축 크기 +] 인자][ + 추가 인자]
	응답패킷:= 응답바이트 헤더 + 응답유형 헤더 + ORDER__ID[ +[ 압축 크기 +] 결과]

처음 세 부분(명령/응답바이트 헤더, 명령유형/응답유형 헤더 + ORDER__ID)는 반드시 보내고,
명령또는 응답에 따라 나머지 부분이 올 수도 있다. ([..]은 ...부분의 선택적임을 뜻한다.)

*: 성능패킷과 장애패킷에 대한 정의는 CPacket.h 에 나와있다.


성능/장애/명령/응답바이트 헤더
------------------------------

    1 ? ? ? ? ? ? ?
    -
    |
    |
    +-------------- 데이터 비트: 이 비트가 설정되어 있다면
                    이 패킷은 데이터(성능, 장애)와 관련된 것이다.
                    그렇지 않다면 "명령(Order)"이나 "응답(Response)"에 관련된 것이다.
 */
#define BYTE__DATA	0x80
/*

명령바이트 헤더
---------------
    x o 1 1 1 1 1 1
    = = = === = = =
    | | |  |  | | |
    | | |  |  | | |
    | | |  |  | | +---- 압축 비트: "인자"가 압축되었는 지를 알려준다.
    | | |  |  | |       바이너리는 반드시 압축한다.
    | | |  |  | |       "인자"가 있을 때만 의미가 있다.
    | | |  |  | +------ 바이너리 비트: "인자"가 바이너리인지 NUL 로 끝나는 문자열인지 말해준다.
    | | |  |  |         "인자"가 있을 때만 의미가 있다.
    | | |  |  +-------- 인자 비트: "인자"("명령" 패킷의 경우)가 있는 지를 알려준다.
    | | |  +----------- XXX__ID 크기 비트: XXX__ID 가 몇 바이트로 표현되는 지를 말해준다.
    | | |               XXX__ID 비트가 설정되었을 때만 유효하다.
    | | +-------------- XXX__ID 비트: XXX__ID 가 있는 지를 말해준다.
    | +---------------- 명령 비트: 이 비트가 설정되었으므로 "명령(Order)" 패킷이다.
    +------------------ 이 비트가 설정되지 않았으므로 "명령(Order)" 패킷이거나 "응답(Response)"패킷이다.
 */
#define BYTE__ORDER			0x40
#define BYTE_ORDER__XXX_ID		0x20
#define BYTE_ORDER__XXX_ID_SIZE_SHIFT	3
#define BYTE_ORDER__XXX_ID_SIZE_MASK	(0x03 << BYTE_ORDER__XXX_ID_SIZE_SHIFT)
#define BYTE_ORDER__ARGUMENT		0x04
#define BYTE_ORDER__BINARY		0x02
#define BYTE_ORDER__ZIPPED		0x01
/*

명령유형 헤더
-------------
    1 1 1 1 1 1 1 1
    === ========= =
     |      |     |
     |      |     +---- 추가인자 비트: 추가인자가 있을 때 사용한다.
     |      |           NUL 로 끝나는 문자열이어야 한다. 압축하지 않으므로 짧아야 한다.
     |      |           현재까지는 ORD_TYPE__DOWNLOAD_BYTE_STREAM 에서만 사용한다. (Entry Point)
     |      +---------- ORDER 유형 비트 (최대 2^5 = 32)
     |                                                  XXX__ID 인자    주(註)
     |                  =============================== ======= ====    ======
     |                  ORD_TYPE__ADD_JOB                  x      o     JOB__ID 는 Job 의 정의에 포함되어 있다
     |                  ORD_TYPE__DELETE_JOB               o      x     JOB__ID
     |                  ORD_TYPE__PAUSE_JOB                o      x     JOB__ID
     |                  ORD_TYPE__CONTINUE_JOB             o      x     JOB__ID
     |                  ORD_TYPE__UPDATE_JOB_SCHEDULE      o      o     JOB__ID
     |                  ORD_TYPE__UPDATE_JOB_RULE          o      o     JOB__ID
     |                  ORD_TYPE__DOWNLOAD_SCRIPT          o      o     SCRIPT__ID
     |                  ORD_TYPE__UPDATE_SCRIPT            o      o     SCRIPT__ID
     |                  ORD_TYPE__DOWNLOAD_BYTE_STREAM     o      o     BYTE_STREAM__ID, 바이너리, Entry Point
     |                  ORD_TYPE__UPDATE_BYTE_STREAM       o      o     BYTE_STREAM__ID, 바이너리
     |                  ORD_TYPE__UPDATE_AGENT             o      o     Restart Job?, 바이너리
     |                  ORD_TYPE__RESTART_AGENT            o      x     Restart Job?
     |                  ORD_TYPE__DISCOVERY                o      o     SCRIPT__ID
     |                  ORD_TYPE__EXEC_COMMAND             x      o
     |                  *: ORD_TYPE__DOWNLOAD_BYTE_STREAM 의 경우에만 추가인자(Entry Point)를 사용한다.
     |
     +----------------- ORDER__ID 크기 비트: ORDER__ID 가 몇 바이트로 표시되는 지를 말해준다.
 */
#define BYTE_ORDER__ID_SIZE_SHIFT	6
#define BYTE_ORDER__ID_SIZE_MASK	(0x03 << BYTE_ORDER__ID_SIZE_SHIFT)
#define BYTE_ORDER__TYPE_SHIFT		1
#define BYTE_ORDER__TYPE_MASK		(0x1F << BYTE_ORDER__TYPE_SHIFT)
#define BYTE_ORDER__EXTRA		0x01
typedef enum {
	ORD_TYPE__ADD_JOB			= 0,	// 제일 작은 값
	ORD_TYPE__JOB_DOWN_CONTROL,			// M/S 에서 Agent 로 Job 을 내려보낼 때 사용한다.
							// 여러 단계를 통해 Job 을 내려보내다.
							// ORDER__ID 에는 각 단계의 ID 를 넣는다.
							//	Script ID, ByteStream ID, Command ID, Job ID
							// 어느 단계인지는 XXX__ID 를 사용한다.
							// CJobDownPacket.h 에 정의되어있다.
	ORD_TYPE__DELETE_JOB,
	ORD_TYPE__PAUSE_JOB,
	ORD_TYPE__CONTINUE_JOB,
	ORD_TYPE__UPDATE_JOB_SCHEDULE,
	ORD_TYPE__UPDATE_JOB_RULE,
	ORD_TYPE__DOWNLOAD_SCRIPT,
	ORD_TYPE__UPDATE_SCRIPT,
	ORD_TYPE__DOWNLOAD_BYTE_STREAM,
	ORD_TYPE__UPDATE_BYTE_STREAM,
	ORD_TYPE__UPDATE_AGENT,
	ORD_TYPE__RESTART_AGENT,
	ORD_TYPE__DISCOVERY,
	ORD_TYPE__EXEC_COMMAND,				// 제일 큰 값
} ORD_TYPE_T;
/*

응답바이트 헤더
---------------
    x x 1 1 1 - - -
    = = = = = =====
    | | | | |   |
    | | | | |   +------ 사용하지 않는다.
    | | | | +---------- PyDict 비트: "결과"가 Python dictionary 의 문자열 표현인지를 말해준다.
    | | | |             이 비트가 설정되면 "결과"를 evaluate 해서 생긴 dictionary 의,
    | | | |             "Result" 키에 해당하는 부분만을 DB 에 저장하고,
    | | | |             다른 키에 해당하는 부분은 M/S 가 사용한다.
    | | | |             물론 ORDER__ID 가 DB 에 저장할 경우에 한해서 DB 에 저장한다.
    | | | |             예를 들어, ORD_TYPE__UPDATE_AGENT 의 경우가 그런 경우이다.
    | | | +------------ 압축 비트: "결과"가 압축되었는 지를 알려준다.
    | | +-------------- 결과 비트: "결과"가 있는 지를 알려준다.
    | |                 결과는 반드시 NUL 로 끝나는 문자열이다.
    | +---------------- 명령 비트: 이 비트가 설정되지 않았으므로 "응답(Response)" 패킷이다.
    +------------------ 이 비트가 설정되지 않았으므로 "명령(Order)" 패킷이거나 "응답(Response)"패킷이다.
 */
#define BYTE_RESP__RETURN		0x20
#define BYTE_RESP__ZIPPED		0x10
#define BYTE_RESP__PY_DICT		0x08
/*

응답유형 헤더
-------------
    1 1 1 1 1 - - -
    === ===== =====
     |    |     |
     |    |     +------ 사용하지 않는다.
     |    +------------ 응답유형 비트: 응답유형을 말해준다.
     |                  (ORD_STATE__WC_SENT)
     |                  (ORD_STATE__MS_SENT)
     |                  RESP_TYPE__ACK (ORD_STATE__ACK)
     |                  RESP_TYPE__OK (ORD_STATE__OK)
     |                  RESP_TYPE__FAIL (ORD_STATE__FAIL)
     |                  괄호안의 표현은 W/C 이 참고하는 OrderTable 에 저장되는 값이다.
     |                  ORD_STATE__WC_SENT 와 ORD_STATE__MS_SENT 는 OrderTable 에만 사용된다.
     |
     +----------------- ORDER__ID 크기 비트: ORDER__ID 가 몇 바이트로 표시되는 지를 말해준다.
 */
#define BYTE_RESP__ID_SIZE_SHIFT	6
#define BYTE_RESP__ID_SIZE_MASK		(0x03 << BYTE_RESP__ID_SIZE_SHIFT)
#define BYTE_RESP__TYPE_SHIFT		3
#define BYTE_RESP__TYPE_MASK		(0x07 << BYTE_RESP__TYPE_SHIFT)
typedef enum {
	ORD_STATE__WC_SENT	= 0,
	ORD_STATE__MS_SENT	= 1,
	ORD_STATE__ACK,
	ORD_STATE__OK,
	ORD_STATE__FAIL,
} ORD_STATE_T;
typedef enum {
	RESP_TYPE__ACK,

// M/S 와 연결할 때,
// 연결 후에 이 연결이 Control 을 위한 것인지, Data 전송을 위한 것인지 말해주는 응답패킷을 보내야한다.
	RESP_TYPE__CTRL_SOCKET,
	RESP_TYPE__CTRL_PORT,	// M/S 가 Agent 에게 Control 을 위해 어떤 port 를 쓰는 지를 알릴 때 사용된다.
	RESP_TYPE__DATA_SOCKET,

	RESP_TYPE__OK,
	RESP_TYPE__FAIL,
} RESP_TYPE_T;
/*
 */
	static int
	packOrder(
		CBuffer&	packet,		// 여기에다 패킹한다.
		ORD_TYPE_T	orderType,
		unsigned int	orderID,
		const unsigned int*	pXxxID,	// NULL 이 아니면 pXxxID 가 가리키는 값이 XXX__ID 이다.
		int		isBinary,	// "인자(argument)"가 바이너리인지 아닌지
						// 인자가 있어야만 의미가 있다.
		const char*	argPtr,		// NULL 이면 "인자가 없다"
		size_t		argLen,		// 인자가 있어야만 의미가 있다.
						// argPtr 이 가리키는 곳에서부터 유효한 바이트 수
						// NUL 로 끝나는 문자열의 경우 NUL 도 포함한다.
						// NUL 로 끝나는 문자열의 경우 0 보다 커야한다.
		const char*	extraArgPtr	// NULL 이 아니면 "추가인자"가 있다.
						// "인자"가 NULL 이면 이 값은 의미가 없다.
	);
	static int
	unpackOrder(
		const CBuffer&	packet,		// 여기에 패킹된 패킷이 들어있다.

		ORD_TYPE_T&	orderType,
		unsigned int&	orderID,

		unsigned int**	pXxxIDPtr,	// pXxxIDPtr 는 XXX__ID 의 값을 가리키는 포인터에 대한 포인터
						// 따라서 XXX__ID 를 담을 공간을 확보한 뒤,
						// 그 주소에 대한 포인터를 인자로 사용한다.
						// *pXxxIDPtr 값이 NULL 이면 XXX__ID 값은 없다.
						// 다음과 같이 사용한다.
						//	unsigned int	xxxID;
						//	unsigned int*	xxxIDPtr = &xxxID;
						// 다음과 같이 unpackOrder 를 호출한다.
						//	unpackOrder(..., &xxxIDPtr, ...);
						// unpackOrder 이 성공적일 때, XXX__ID 를 다음과 같이 확인한다.
						//	if (*xxxIDPtr == NULL) {
						//		// XXX__ID 가 설정되지 않았다.
						//	} else {
						//		// xxxID 가 설정된 XXX__ID 이다.
						//	}
		int&		isBinary,
		CBuffer&	argBuffer,	// argBuffer 나 extraArgBuffer 의 경우,
						// 설정된 "인자(argument)"나 "추가인자"가 없다면
						// 버퍼의 크기가 0이 된다.(CBuffer.getDataSize() = 0)
		CBuffer&	extraArgBuffer
	);

	static int
	packResponse(
		CBuffer&	packet,		// 여기에다 패킹한다.
		int		isPyDict,	// "결과"가 Python dictionary 의 문자열 표현인가?
		RESP_TYPE_T	respType,
		unsigned int	orderID,
		const char*	pReturn		// NULL 이 아니면 "결과(return)"이 있다.
						// NUL 까지 포함되어서 패킹된다.
	);
	static int
	unpackResponse(
		const CBuffer&	packet,		// 패킹된 패킷은 여기에 들어있다.

		int&		isPyDict,
		RESP_TYPE_T&	respType,
		unsigned int&	orderID,
		CBuffer&	returnBuffer	// "결과"는 여기에 저장된다.
						// 결과가 없는 경우, 버퍼의 크기는 0이다.
	);
};

// *pByte 가 가리키는 곳에서부터 크기가 (1 + NByte)인 양수를 읽어 ui 에 저장한다.
// *ppByte 는 그 다음을 가리킨다.
void	loadInteger(CPacket::BYTE** ppByte, unsigned NByte, unsigned& ui);


////////////////////////////////////////////////////////////////////////////////
#endif	// !__BYTE_H__
