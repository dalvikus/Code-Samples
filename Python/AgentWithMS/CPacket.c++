#include "CPacket.h"
#include "CZip.h"

#include <stdio.h>
#include <string.h>

unsigned int	CPacket::ZIPPED_STRING_LENGTH_LOWER_BOUND = 40;

////////////////////////////////////////////////////////////////////////////////
// 양수 i 가 차지하는 (바이트 수 - 1)를 구한다.
inline
static unsigned int
getNByte(
	unsigned int	i
) {
	if (i < 0x00000100)
		return 0;
	else if (i < 0x00010000)
		return 1;
	else if (i < 0x01000000)
		return 2;
	return 3;
}

// NByte의 양수 ui 를 *ppByte가 가리키는 곳에 쓴다.
// 0xABCDEF 가 있다면 0xAB, 0xCD, 0xEF 가 써지도록 한다.
inline
static void
saveInteger(
	CPacket::BYTE**	ppByte,
	unsigned int	NByte,	// 0:= 한 바이트, 1:= 두 바이트, 2:= 세 바이트, 3:= 네 바이트
	unsigned int	ui
)
{
	CPacket::BYTE*	pByte = *ppByte;

	pByte += NByte;		// 낮은 바이트부터 써서 낮은 주소부터 채운다.
	++NByte;		// 쓰여질 바이트 갯수는 1 + NByte이다.
	for (unsigned int i = 0; i < NByte; ++i) {
		*pByte-- = ui & 0xFF;	// 그 다음 바이트를 높은 주소에 쓴다.
		ui >>= 8;
	}
	// 다 쓴 후에는 pByte 는 원래보다 하나 앞에 가 있다.
	// 최종적으로 양수가 쓰여진 다음 바이트를 가리키도록 한다.
	pByte += 1 + NByte;

	*ppByte = pByte;
}

// *pByte 가 가리키는 곳에서부터 크기가 (1 + NByte)인 양수를 읽어 ui 에 저장한다.
// *ppByte 는 그 다음을 가리킨다.
inline
void
loadInteger(
	CPacket::BYTE**	ppByte,
	unsigned int	NByte,
	unsigned int&	ui
)
{
	CPacket::BYTE*	pByte = *ppByte;

	// 위에서부터 읽어 상위바이트를 설정한다.
	ui = 0;
	for (unsigned int i = 0; i < 1 + NByte; ++i) {
		ui <<= 8;
		ui |= *pByte++;
	}

	*ppByte = pByte;
}

// *ppByte 가 가리키는 곳에서부터 4 바이트는 압축된 바이트의 수를 표시하고,
// 그 이후로는 압축된 바이트들이 들어있다.
// 압축을 풀어 buffer 의 버퍼에 저장하고, *ppByte 는 그 다음 바이트를 가리키도록 한다.
// 압축을 푼 후의 결과가 NUL 로 끝나는 문자열의 경우 마지막에 NUL 을 추가한다.
static int
inflateByte(
	CPacket::BYTE**	ppByte,
	CBuffer&	buffer,
	int		isBinary = 0	// 바이너리라면 마지막에 NUL 을 추가하지 않는다.
)
{
	CPacket::BYTE*	pByte = *ppByte;

	unsigned int	compressedSize;
	loadInteger(&pByte, 3, compressedSize);
	// pByte 는 압축된 바이트들의 시작을 가리킨다.

	// pByte 가 가리키는 곳에서 부터 compressedSize 바이트들의 압축을 푼다.
	CZip	zlib(&buffer);
	// 버퍼를 초기화한다.
	CBuffer&	zlibBuffer = zlib.getBuffer();
	zlibBuffer.setDataSize(0);
	if (zlib.uncompress((Bytef*) pByte, compressedSize) < 0) {
#ifdef __TRACE__
		(void) fprintf(stderr, "__TRACE__: inflateByte: CZip::uncompressed: Failure\n");
#endif	// __TRACE__

		return -1;
	}
	if (!isBinary) {
		// 끝에 NUL 을 표시한다.
		*(zlibBuffer.getBuf() + zlibBuffer.getDataSize()) = '\0';
	}
	pByte += compressedSize;
	// pByte 는 압축된 바이트들은 지난 위치를 가리킨다.

	*ppByte = pByte;
	return 0;
}
// strPtr이 가리키는 곳에서부터 시작하여 strLen 바이트를 압축한다.
// *ppByte 가 가리키는 곳에서부터 그 상황을 쓴다.
// *ppByte 에는 4바이트의 크기를 기록하고 그 다음에는 압축된 데이터를 쓴다.
// 마지막으로 *ppByte 는 그 다음 위치로 이동한다.
// 압축을 하면서 메모리 위치가 바뀔 수 있다.
// 따라서 *ppByte 값이 전혀 다른 곳으로 바뀔 수도 있다.
// 결국 오프셋을 통해서 위치를 찾아야 한다.
static int
deflateByte(
	CPacket::BYTE**	ppByte,		// [OUT]
	CBuffer&	buffer,
	const char*	strPtr,
	size_t		strLen
)
{
	// 버퍼의 시작에서 얼마만큼의 위치에 압축한 바이트의 크기를 넣을 지를 기억한다.
	size_t	offsetZippedByteSize = buffer.getDataSize();
	// 압축된 바이트의 크기를 넣은 공간은 건너뛴다.
	if (buffer.incDataSize(4) == NULL) {
#ifdef __TRACE__
		(void) fprintf(stderr, "__TRACE__: deflateByte: CBuffer::incDataSize(): Failure\n");
#endif	// __TRACE__

		return -1;
	}

	CZip	zlib(&buffer);	// 현재 패킷을 담고 있는 버퍼를 이용한다.
	// strPtr 가 가리키는 곳에서부터 strLen 바이트를 압축한다. (NUL은 제외)
	// 그 결과를 CZip 의 내부버퍼에 둔다.
	if (zlib.compress((Bytef*) strPtr, strLen) < 0) {
#ifdef __TRACE__
		(void) fprintf(stderr, "__TRACE__: deflateByte: CZip::compress(strErrValue): Failure\n");
#endif	// __TRACE__
			return -1;
	}
	size_t	compressedSize = buffer.getDataSize() - offsetZippedByteSize - 4;	// 압축된 바이트 수
											// 4:= 크기를 저장할 바이트 수
	// 압축된 바이트 크기를 저장할 위치는 buffer 의 시작에서부터 offsetZippedByteSize 만큼 이동한 곳이다.
	CPacket::BYTE*	pByte = (CPacket::BYTE*) buffer.getBuf() + offsetZippedByteSize;
	saveInteger(&pByte, 3, compressedSize);		// 해당 위치에 쓴다.
	pByte += compressedSize;	// pByte 를 그 다음 위치로 옮긴다.
					// buffer 안에 있는 데이터의 크기는 CZip::compress 에서 늘어났다.

	*ppByte = pByte;
	return 0;
}

// *ppByte 가 가리키는 곳에 bytePtr 에서 시작하는 byteLen 개의 바이트를 복사한다.
// *ppByte 가 가리키는 곳의 메모리의 주소가 바뀔 수 있으므로 주의해야한다.
static int
byteCopy(
	CPacket::BYTE**	ppByte,
	const char*	bytePtr,
	size_t		byteLen,
	CBuffer&	buffer
)
{
	// 적어도 byteLen 바이트는 확보한다.
	// CBuffer::setBufSizeAtLeast 의 호출 후에 메모리의 위치가 바뀔 수 있으므로,
	// *ppByte 자체는 의미가 없다. buffer 버퍼의 시작에서부터의 오프셋이 중요하다.
	size_t	offset = buffer.getDataSize();
	if (buffer.setBufSizeAtLeast(byteLen) < 0) {
#ifdef __TRACE__
		(void) fprintf(stderr, "__TRACE__: byteCopy: CBuffer::setBufSizeAtLeast(): Failure\n");
#endif	// __TRACE__

		return -1;
	}
	CPacket::BYTE*	pByte = (CPacket::BYTE*) buffer.getBuf() + offset;
	(void*) memcpy(pByte, bytePtr, byteLen);
	// 다음 바이트를 가리키도록 한다.
	pByte += byteLen;
	(void*) buffer.incDataSize(byteLen);

	*ppByte = pByte;
	return 0;
}
////////////////////////////////////////////////////////////////////////////////


int
CPacket::packErrorData(
	CBuffer&	packet,

	int		jobID,
	int		eventID,
	int		severity,
	const char*	strErrValue,
	const char*	strErrDetail,

	size_t		packetSizeUpperBound
)
{
	if (severity < SEV__CLEAR || severity > SEV__INFORMATION) {
#ifdef __TRACE__
		(void) fprintf(stderr,
			"__TRACE__: CPacket::packErrorData: severity = %d: Invalid\n"
			,
			severity
		);
#endif	// __TRACE__

		return -1;
	}

	// 버퍼를 초기화한다.
	(void*) packet.setDataSize(0);
	// pByte 는 버퍼의 시작위치를 가리킨다.
	BYTE*	pByte = (BYTE*) packet.getBuf();

	// 패킷을 넣을 공간을 충분히 확보한다.
	//	패킹의 총 크기: 4
	//	장애바이트 헤더: 1
	//	JOB_ID: (최대) 4
	//	이벤트 헤더: 1
	//	EVENT_ID: (최대) 4
	//	압축될 바이트의 크기: 4
	if (!packet.setBufSizeAtLeast(16)) {
#ifdef __TRACE__
		(void) fprintf(stderr, "__TRACE__: CPacket::packErrorData: CBuffer::setBufSizeAtLeast(): Failure\n");
#endif	// __TRACE__

		return -2;
	}

	// 패킷의 크기를 적을 공간은 지금은 건너뛴다.
	// 제일 마지막에 패킷을 마무리한 후 적을 것이다.
	pByte += 4;
	(void*) packet.setDataSize(4);
	// pByte 는 "장애바이트 헤더"를 가리킨다.

	*pByte = 0x00;	// 모든 비트를 클리어한다.
	*pByte |= BYTE__DATA;	// 성능데이터 또는 장애데이터
	*pByte |= BYTE__ERROR;	// 장애데이터
	unsigned int	NJobID = getNByte(jobID);
	*pByte |= NJobID << BYTE_ERROR__JOB_ID_SIZE_SHIFT;	// JOB ID 의 크기 바이트 설정
	size_t	valueLen = strlen(strErrValue);
	int	isZipped = valueLen > ZIPPED_STRING_LENGTH_LOWER_BOUND ? 1 : 0;
	if (isZipped)
		*pByte |= BYTE_ERROR__ZIPPED;
	int	isDetail = strErrDetail ? 1 : 0;
	if (isDetail)
		*pByte |= BYTE_ERROR__DETAIL;
	++pByte;
	(void*) packet.incDataSize(1);
	// pByte 는 JOB_ID 를 적을 위치를 가리킨다.

	// JOB_ID 를 쓴다.
	saveInteger(&pByte, NJobID, jobID);
	(void*) packet.incDataSize(1 + NJobID);
	// pByte 는 이벤트 헤더를 쓸 위치를 가리키고 있다.

	// 이벤트 헤더를 쓴다.
	// 모든 비트를 클리어한다.
	*pByte = 0x00;
	unsigned int	NEventID = getNByte(eventID);
	*pByte |= NEventID << BYTE_EVENT__SIZE_SHIFT;
	*pByte |= severity << BYTE_EVENT__SEVERITY_SHIFT;
	++pByte;
	(void*) packet.incDataSize(1);
	// pByte 는 EVENT_ID 를 쓸 위치를 가리키고 있다.

	// EVENT_ID 를 쓴다.
	saveInteger(&pByte, NEventID, eventID);
	(void*) packet.incDataSize(1 + NEventID);
	// pByte 는 장애값을 가리킨다.
	// 장애값을 압축한다면 압축 헤더를,
	// 그렇지 않다면 장애값을 나타내는 문자열의 시작 위치를 가리킨다.

	if (isZipped) {
		if (deflateByte(&pByte, packet, strErrValue, valueLen) < 0) {
#ifdef __TRACE__
			(void) fprintf(stderr, "__TRACE__: CPacket::packErrorData: deflateByte(\"장애값\"): Failure\n");
#endif	// __TRACE__

			return -1;
		}
	} else {
#if 1
		if (byteCopy(&pByte, strErrValue, valueLen + 1, packet) < 0) {	// NUL 까지 포함해서
#ifdef __TRACE__
			(void) fprintf(stderr, "__TRACE__: CPacket::packErrorData: byteCopy(\"장애값\"): Failure\n");
#endif	// __TRACE__

			return -1;
		}
#else
		size_t	byteLen = valueLen + 1;	// NUL 문자를 포함해서
		// 적어도 byteLen 바이트는 확보한다.
		// CBuffer::setBufSizeAtLeast 의 호출 후에 메모리의 위치가 바뀔 수 있으므로,
		// pByte 자체는 의미가 없다. packet 버퍼의 시작에서부터의 오프셋이 중요하다.
		size_t	offset = packet.getDataSize();
		if (packet.setBufSizeAtLeast(byteLen) < 0) {
#ifdef __TRACE__
			(void) fprintf(stderr, "__TRACE__: CPacket::packErrorData: CBuffer::setBufSizeAtLeast(\"장애값\"): Failure\n");
#endif	// __TRACE__

			return -2;
		}
		pByte = (BYTE*) packet.getBuf() + offset;
		// NUL 을 포함해서 byteLen 바이트를 pByte 가 가리키는 곳에 복사한다.
		(void*) memcpy(pByte, strErrValue, byteLen);
		// 다음 바이트를 가리키도록 한다.
		pByte += byteLen;
		(void*) packet.incDataSize(byteLen);
#endif
	}
	// pByte 는 (있다면) 상세설명이 들어갈 위치를 가리킨다.
	if (isDetail) {
		if (deflateByte(&pByte, packet, strErrDetail, strlen(strErrDetail)) < 0) {
#ifdef __TRACE__
			(void) fprintf(stderr, "__TRACE__: CPacket::packErrorData: deflateByte(\"상세설명\"): Failure\n");
#endif	// __TRACE__

			return -1;
		}
	}

	pByte = (BYTE*) packet.getBuf();
	saveInteger(&pByte, 3, packet.getDataSize() - 4);

	return 0;
}
int
CPacket::unpackErrorData(
	const CBuffer&	packet,

	unsigned int&	jobID,
	unsigned int&	eventID,
	unsigned int&	severity,
	CBuffer&	strErrValue,

	int&		isDetail,
	CBuffer&	strErrDetail
)
{
	BYTE*	pByte = (BYTE*) packet.getBuf();

	unsigned int	packetSize;
	loadInteger(&pByte, 3, packetSize);
	// pByte 는 "장애데이터 헤더"를 가리킨다.

	if (!(*pByte & BYTE__DATA)) {
		(void) fprintf(stderr,
			"__FILE__:= |%s|, __LINE_NO__ = %d: "
			"BYTE__DATA: Not set\n"
			,
			__FILE__, __LINE__
		);

		return -1;
	}
	if (!(*pByte & BYTE__ERROR)) {
#ifdef __TRACE__
		(void) fprintf(stderr, "__TRACE__: CPacket::unpackErrorData: BYTE__ERROR: Not set\n");
#endif	// __TRACE__

		return -1;
	}
	unsigned int	NJobID = (*pByte & BYTE_ERROR__JOB_ID_SIZE_MASK) >> BYTE_ERROR__JOB_ID_SIZE_SHIFT;
	int	isZipped = *pByte & BYTE_ERROR__ZIPPED;
	isDetail = *pByte & BYTE_ERROR__DETAIL;
	++pByte;
	// pByte 는 JOB_ID 를 담은 정수바이트의 시작을 가리킨다.

	loadInteger(&pByte, NJobID, jobID);
	// pByte 는 이벤트 헤더를 가리킨다.

	unsigned int	NEventID = (*pByte & BYTE_EVENT__SIZE_MASK) >> BYTE_EVENT__SIZE_SHIFT;
	severity = (*pByte & BYTE_EVENT__SEVERITY_MASK) >> BYTE_EVENT__SEVERITY_SHIFT;
	if (severity < SEV__CLEAR || severity > SEV__INFORMATION) {
#ifdef __TRACE__
		(void) fprintf(stderr, "__TRACE__: CPacket::unpackErrorData: Severity = %d: Invalid\n"
			,
			severity
		);
#endif	// __TRACE__

		return -1;
	}
	++pByte;
	// pByte 는 EVENT_ID 를 담은 정수바이트의 시작을 가리킨다.

	loadInteger(&pByte, NEventID, eventID);
	// pByte 는 장애값을 가리킨다.

	// isZipped 가 True 이면 압축 헤더를, False 이면 장애값을 말하는 문자열의 시작을 가리킨다.
	if (isZipped) {
		if (inflateByte(&pByte, strErrValue) < 0) {
#ifdef __TRACE__
			(void) fprintf(stderr, "__TRACE__: inflateByte(\"장애값\"): Failure\n");
#endif	// __TRACE__

			return -2;
		}
		// pByte 는 (있다면) 상세설명의 압축 헤더를 가리킨다.
	} else {
		size_t	valueLen = strlen((const char*) pByte) + 1;	// 마지막의 NUL 문자를 포함해서
		// pByte 가 가리키는 곳에서 부터 valueLen 개의 바이트를 strErrValue 의 버퍼에 저장한다.
		if (!strErrValue.setBufSizeAtLeast(valueLen)) {
#ifdef __TRACE__
			(void) fprintf(stderr, "__TRACE__: CPacket::unpackErrorData: \"장애값\": CBuffer.setBufSizeAtLeast: Failure\n");
#endif	// __TRACE__

			return -3;
		}
		(void*) memcpy(strErrValue.getBuf(), pByte, valueLen);
		pByte += valueLen;
		(void*) strErrValue.incDataSize(valueLen);
		// pByte 는 (있다면) 상세설명의 압축 헤더를 가리킨다.
	}

	if (isDetail) {
		if (inflateByte(&pByte, strErrDetail) < 0) {
#ifdef __TRACE__
			(void) fprintf(stderr, "__TRACE__: inflateByte(\"상세설명\"): Failure\n");
#endif	// __TRACE__

			return -2;
		}
		// pByte 는 패킷의 바로 다음 바이트를 가리킨다.
	}
	// pByte 는 패킷의 바로 다음 바이트를 가리킨다.

	return packetSize == (unsigned int) (pByte - (BYTE*) packet.getBuf() - 4) ? 0 : -4;
}

int
CPacket::packPerfData(
	CBuffer&	packet,

	int		jobID,
	const TYPE_VALUE_T&	typeValue,
	const char*	strPerfExtra,

	size_t		packetSizeUpperBound
)
{
	// 버퍼를 초기화한다.
	(void*) packet.setDataSize(0);
	// pByte 는 버퍼의 시작위치를 가리킨다.
	BYTE*	pByte = (BYTE*) packet.getBuf();

	// 패킷을 넣을 공간을 충분히 확보한다.
	//	패킹의 총 크기: 4
	//	성능바이트 헤더: 1
	//	JOB_ID: (최대) 4
	//	압축/바이너리 헤더: 1
	//		(압축) 압축될 바이트의 크기: 4
	//		(바이너리) 정수: (최대) 4
	if (!packet.setBufSizeAtLeast(14)) {
#ifdef __TRACE__
		(void) fprintf(stderr, "__TRACE__: CPacket::packPerfData: CBuffer::setBufSizeAtLeast(): Failure\n");
#endif	// __TRACE__

		return -2;
	}

	// 패킷의 크기를 적을 공간은 지금은 건너뛴다.
	// 제일 마지막에 패킷을 마무리한 후 적을 것이다.
	pByte += 4;
	(void*) packet.setDataSize(4);
	// pByte 는 "장애바이트 헤더"를 가리킨다.

	*pByte = 0x00;	// 모든 비트를 클리어한다.
	*pByte |= BYTE__DATA;	// 성능데이터 또는 장애데이터
	unsigned int	NJobID = getNByte(jobID);
	*pByte |= NJobID << BYTE_PERF__JOB_ID_SIZE_SHIFT;	// JOB ID 의 크기 바이트 설정
	int	isZB = 1;		// VAL_TYPE__INTERGER, VAL_TYPE__FIXED_POINT 인 경우는 무조건 1 (바이너리)
					// VAL_TYPE__STRING 인 경우는 일정 길이 이상이면 1 (압축)
	int	valueLen = -1;		// VAL_TYPE__STRING 인 경우에만 >= 0 이다.
	if (typeValue.type == VAL_TYPE__STRING) {
		valueLen = strlen(typeValue.value.str);
		if ((size_t) valueLen < ZIPPED_STRING_LENGTH_LOWER_BOUND)
			isZB = 0;
	}
	if (isZB)
		*pByte |= BYTE_PERF__ZB;
	int	isExtra = strPerfExtra ? 1 : 0;
	if (isExtra)
		*pByte |= BYTE_PERF__EXTRA;
	++pByte;
	(void*) packet.incDataSize(1);
	// pByte 는 JOB_ID 를 적을 위치를 가리킨다.

	// JOB_ID 를 쓴다.
	saveInteger(&pByte, NJobID, jobID);
	(void*) packet.incDataSize(1 + NJobID);
	// pByte 는 "압축/바이너리 헤더"를 가리거나(isZB = 1) 문자열을 적을 위치를 가리킨다.(isZB = 0)

	if (isZB) {
		if (typeValue.type == VAL_TYPE__STRING) {	// 압축한다.
			// pByte 는 압축 헤더를 기리킨다.
			*pByte++ = 0x80;	// 압축 비트를 설정하고, 압축 정보를 담을 바이트로 이동한다.
			(void*) packet.incDataSize(1);
			if (deflateByte(&pByte, packet, typeValue.value.str, valueLen) < 0) {
#ifdef __TRACE__
				(void) fprintf(stderr, "__TRACE__: CPacket::packPerfData: deflateByte(\"성능값\"): Failure\n");
#endif	// __TRACE__

				return -1;
			}
		} else {	// 바이너리화한다.
			// pByte 는 바이너리 헤더를 기리킨다.
			// 모든 비트를 클리어한다.
			*pByte = 0x00;
			int	ival; 
			if (typeValue.type == VAL_TYPE__INTERGER) {
				ival = typeValue.value.ival;
			} else {		// VAL_TYPE__FIXED_POINT
				ival = typeValue.value.fixedPoint.iFixed;
				int	iPoint = typeValue.value.fixedPoint.iPoint;
				if (iPoint < 0 || iPoint > 3) {
#ifdef __TRACE__
					(void) fprintf(stderr,
						"__TRACE__: CPacket::packPerfData: iPoint = %d: Invalid\n"
						,
						iPoint
					);
#endif	// __TRACE__

					return -1;
				}
				*pByte |= iPoint << BYTE_ZB_BINARY__POINT_SHIFT;
			}
			unsigned int	NVal = getNByte(ival);
			*pByte |= NVal << BYTE_ZB_BINARY__SIZE_SHIFT;
			++pByte;	// pByte 는 정수를 넣을 바이트를 가리킨다.
			(void*) packet.incDataSize(1);

			// ival 를 저장한다.
			saveInteger(&pByte, NVal, ival);
			(void*) packet.incDataSize(1 + NVal);
			// pByte 는 (있다면) 추가내용을 적을 바이트를 가리킨다.
		}
	} else {
		if (byteCopy(&pByte, typeValue.value.str, valueLen + 1, packet) < 0) {	// NUL 까지 포함해서
#ifdef __TRACE__
			(void) fprintf(stderr, "__TRACE__: CPacket::packPerfData: byteCopy(\"성능값\"): Failure\n");
#endif	// __TRACE__

			return -1;
		}
	}
	// pByte 는 (있다면) 추가내용이 들어갈 위치를 가리킨다.
	if (isExtra) {	// 항상 압축한다.
		if (deflateByte(&pByte, packet, strPerfExtra, strlen(strPerfExtra)) < 0) {
#ifdef __TRACE__
			(void) fprintf(stderr, "__TRACE__: CPacket::packPerfData: deflateByte(\"추가내용\"): Failure\n");
#endif	// __TRACE__

			return -1;
		}
	}

	pByte = (BYTE*) packet.getBuf();
	saveInteger(&pByte, 3, packet.getDataSize() - 4);

	return 0;
}
int
CPacket::unpackPerfData(
	const CBuffer&	packet,

	unsigned int&	jobID,
	TYPE_VALUE_T&	typeValue,

	int&		isExtra,
	CBuffer&	strPerfExtra
)
{
	BYTE*	pByte = (BYTE*) packet.getBuf();

	unsigned int	packetSize;
	loadInteger(&pByte, 3, packetSize);
	// pByte 는 "성능데이터 헤더"를 가리킨다.

	if (!(*pByte & BYTE__DATA)) {
		(void) fprintf(stderr,
			"__FILE__:= |%s|, __LINE_NO__ = %d: "
			"BYTE__DATA: Not set\n"
			,
			__FILE__, __LINE__
		);

		return -1;
	}
	if (*pByte & BYTE__ERROR) {
#ifdef __TRACE__
		(void) fprintf(stderr, "__TRACE__: CPacket::unpackPerfData: BYTE__ERROR: Set\n");
#endif	// __TRACE__

		return -1;
	}
	unsigned int	NJobID = (*pByte & BYTE_PERF__JOB_ID_SIZE_MASK) >> BYTE_PERF__JOB_ID_SIZE_SHIFT;
	int	isZB = *pByte & BYTE_PERF__ZB;
	isExtra = *pByte & BYTE_PERF__EXTRA;
	++pByte;
	// pByte 는 JOB_ID 를 담은 정수바이트의 시작을 가리킨다.

	loadInteger(&pByte, NJobID, jobID);
	// pByte 는 "압축/바이너리 헤더"를 가리키거나(isZB = 1) "성능값"에 해당하는 문자열의 시작을 가리킨다.(isZB = 0)

	if (isZB) {
		if (*pByte & BYTE_ZB__ZIPPED) {
			typeValue.type = VAL_TYPE__STRING;
			++pByte;
			// pByte 는 압축된 상황을 가리킨다.
			if (inflateByte(&pByte, typeValue.buffer) < 0) {
#ifdef __TRACE__
				(void) fprintf(stderr, "__TRACE__: inflateByte(\"성능값\"): Failure\n");
#endif	// __TRACE__

				return -2;
			}
			// pByte 는 (있다면) 추가내용의 압축 헤더를 가리킨다.
		} else {
			int	NVal = (*pByte & BYTE_ZB_BINARY__SIZE_MASK) >> BYTE_ZB_BINARY__SIZE_SHIFT;
			int	iPoint = (*pByte & BYTE_ZB_BINARY__POINT_MASK) >> BYTE_ZB_BINARY__POINT_SHIFT;
			if (iPoint)
				typeValue.type = VAL_TYPE__FIXED_POINT;
			else
				typeValue.type = VAL_TYPE__INTERGER;
			++pByte;
			// pByte 는 정수를 가리킨다.
			unsigned int	ival;
			loadInteger(&pByte, NVal, ival);
			if (iPoint) {
				typeValue.value.fixedPoint.fval = ival / (float) (iPoint == 3 ? 1000 : (iPoint == 2 ? 100 : 10));
				typeValue.value.fixedPoint.iFixed = ival;
				typeValue.value.fixedPoint.iPoint = iPoint;
			} else {
				typeValue.value.ival = ival;
			}
		}
	} else {
		typeValue.type = VAL_TYPE__STRING;
		size_t	byteLen = strlen((const char*) pByte) + 1;	// 마지막의 NUL 문자를 포함해서
		// pByte 가 가리키는 곳에서 부터 byteLen 개의 바이트를 TYPE_VALUE_T::buffer 의 버퍼에 저장한다.
		CBuffer&	strPerfValue = typeValue.buffer;
		if (!strPerfValue.setBufSizeAtLeast(byteLen)) {
#ifdef __TRACE__
			(void) fprintf(stderr, "__TRACE__: CPacket::unpackPerfData: \"성능값\": CBuffer.setBufSizeAtLeast: Failure\n");
#endif	// __TRACE__

			return -3;
		}
		(void*) memcpy(strPerfValue.getBuf(), pByte, byteLen);
		pByte += byteLen;
		(void*) strPerfValue.incDataSize(byteLen);
		// pByte 는 (있다면) 추가내용의 압축 헤더를 가리킨다.
	}

	if (isExtra) {
		if (inflateByte(&pByte, strPerfExtra) < 0) {
#ifdef __TRACE__
			(void) fprintf(stderr, "__TRACE__: inflateByte(\"추가내용\"): Failure\n");
#endif	// __TRACE__

			return -2;
		}
		// pByte 는 패킷의 바로 다음 바이트를 가리킨다.
	}
	// pByte 는 패킷의 바로 다음 바이트를 가리킨다.

	return packetSize == (unsigned int) (pByte - (BYTE*) packet.getBuf() - 4) ? 0 : -4;
}

CPacket::PACKET_TYPE_T
CPacket::whichType(
	const CBuffer&	packet,
	unsigned int*	pPacketSize
)
{
	BYTE*	pByte = (BYTE*) packet.getBuf();
	if (packet.getDataSize() < 5)
		return PACKET_TYPE__UNKNOWN;

	unsigned int	packetSize;
	loadInteger(&pByte, 3, packetSize);
	// pByte 는 "성능데이터 헤더"를 가리킨다.

#if 1
	// 패킷의 앞부분만 받고 이 함수를 호출할 수도 있다.
#else
	if (packetSize != packet.getDataSize() - 4) {
		// 너무 간단하다.
		// 패킷의 제일 마지막에 체크썸을 넣을 수도 있다.
		return PACKET_TYPE__INVALID;
	}
#endif
	if (pPacketSize)
		*pPacketSize = packetSize;
	return *pByte & BYTE__DATA ?
		(*pByte & BYTE__ERROR ? PACKET_TYPE__ERR : PACKET_TYPE__PERF) :
		(*pByte & BYTE__ORDER ? PACKET_TYPE__ORD : PACKET_TYPE__RESP);
}

int
CPacket::packOrder(
	CBuffer&	packet,		// 여기에다 패킹한다.
	ORD_TYPE_T	orderType,
	unsigned int	orderID,
	const unsigned int*	pXxxID,	// NULL 이 아니면 pXxxID 가 가리키는 값이 XXX__ID 이다.
	int		isBinary,	// "인자(argument)"가 바이너리인지 아닌지
					// 인자가 있어야만 의미가 있다.
	const char*	argPtr,		// NULL 이면 "인자가 없다"
	size_t		argLen,		// 인자가 있어야만 의미가 있다.
	const char*	extraArgPtr	// NULL 이 아니면 "추가인자"가 있다.
					// "인자"가 NULL 이면 이 값은 의미가 없다.
)
{
	if (orderType < ORD_TYPE__ADD_JOB || orderType > ORD_TYPE__EXEC_COMMAND) {
		(void) fprintf(stderr,
			"ERROR: "
			"__FILE__:= |%s|, __LINE_NO__ = %d: "
			"orderType = %d: Invalid\n"
			,
			__FILE__, __LINE__,
			orderType
		);

		return -1;
	}

	// 버퍼를 초기화한다.
	(void*) packet.setDataSize(0);
	// pByte 는 버퍼의 시작위치를 가리킨다.
	BYTE*	pByte = (BYTE*) packet.getBuf();

	// 패킷을 넣을 공간을 충분히 확보한다.
	//	패킹의 총 크기: 4
	//	명령바이트 헤더: 1
	//	명령유형 헤더: 1
	//	ORDER__ID: (최대) 4
	//	XXX__ID: (최대) 4	// 있다면...
	//	압축될 바이트의 크기: 4
	if (!packet.setBufSizeAtLeast(18)) {
		(void) fprintf(stderr,
			"FATAL: "
			"__FILE__:= |%s|, __LINE_NO__ = %d: "
			"CBuffer::setBufSizeAtLeast(18): Failure\n"
			,
			__FILE__, __LINE__
		);

		return -1;
	}

	// 패킷의 크기를 적을 공간은 지금은 건너뛴다.
	// 제일 마지막에 패킷을 마무리한 후 적을 것이다.
	pByte += 4;
	(void*) packet.setDataSize(4);
	// pByte 는 "명령바이트 헤더"를 가리킨다.

	// "명령바이트 헤더"
	*pByte = 0x00;	// 모든 비트를 클리어한다.
	*pByte |= BYTE__ORDER;	// 명령 패킷
	unsigned int	NXxxID = (unsigned int) -1;	// XXX__ID 가 있을 때만 의미가 있다.
	if (pXxxID) {	// XXX__ID 가 있다.
		*pByte |= BYTE_ORDER__XXX_ID;
		NXxxID = getNByte(*pXxxID);
		*pByte |= NXxxID << BYTE_ORDER__XXX_ID_SIZE_SHIFT;	// XXX__ID 의 크기 바이트 설정
	}
	int	isZipped = -1;	// "인자"가 있을 때에만 의미가 있다.
		// 바이너리의 경우 반드시 압축한다.
		// NUL 로 끝나는 문자열인 경우 길이가 ZIPPED_STRING_LENGTH_LOWER_BOUND 보다 커야 압축된다.
	if (argPtr) {	// "인자"가 있다.
		*pByte |= BYTE_ORDER__ARGUMENT;
		if (isBinary) {
			*pByte |= BYTE_ORDER__BINARY;
			isZipped = 1;
		} else {
			if (argLen == 0) {
				(void) fprintf(stderr,
					"ERROR: "
					"__FILE__:= |%s|, __LINE_NO__ = %d: "
					"|인자(Argument)| is NOT a binary but has length = 0: Invalid\n"
					,
					__FILE__, __LINE__
				);

				return -1;
			}
			isZipped = argLen > ZIPPED_STRING_LENGTH_LOWER_BOUND ? 1 : 0;
		}
		if (isZipped)
			*pByte |= BYTE_ORDER__ZIPPED;
	}
	++pByte;
	(void*) packet.incDataSize(1);
	// pByte 는 "명령유형 헤더"를 가리킨다.

	// "명령유형 헤더"
	*pByte = 0x00;	// 모든 비트를 클리어한다.
	// ORDER__ID 의 크기 비트를 설정한다.
	unsigned int	NOrderID = getNByte(orderID);
	*pByte |= NOrderID << BYTE_ORDER__ID_SIZE_SHIFT;
	// 명령유형을 설정한다.
	*pByte |= (orderType << BYTE_ORDER__TYPE_SHIFT) & BYTE_ORDER__TYPE_MASK;
	if (extraArgPtr) {
		if (!argPtr) {
			(void) fprintf(stderr,
				"ERROR: "
				"__FILE__:= |%s|, __LINE_NO__ = %d: "
				"extraArgPtr is NOT NULL but argPtr = NULL: Invalid\n"
				,
				__FILE__, __LINE__
			);

			return -1;
		}
		*pByte |= BYTE_ORDER__EXTRA;
	}
	++pByte;
	(void*) packet.incDataSize(1);
	// pByte 는 ORDER__ID 를 넣을 자리를 가리킨다.

	// ORDER__ID 를 쓴다.
	saveInteger(&pByte, NOrderID, orderID);
	(void*) packet.incDataSize(1 + NOrderID);
	// pByte 는 패킷의 마지막 다음 위치를 가리키거나(XXX__ID 와 "인자" 어느 것도 없는 경우),
	// XXX__ID 를 넣을 위치나(XXX__ID 가 있는 경우)
	// "인자"가 있을 때는 "인자"를 넣을 위치를 가리킨다.

	if (pXxxID) {	// XXX__ID 가 있다.
		saveInteger(&pByte, NXxxID, *pXxxID);
		(void*) packet.incDataSize(1 + NXxxID);
	}
	// pByte 는 패킷의 마지막 다음 위치를 가리키거나("인자"가 없는 경우)
	// "인자"를 넣을 위치를 가리킨다.

	if (argPtr) {
		if (isZipped) {
			// 압축한다.
			if (deflateByte(&pByte, packet, argPtr, argLen) < 0) {
				(void) fprintf(stderr,
					"ERROR: "
					"__FILE__:= |%s|, __LINE_NO__ = %d: "
					"deflateByte(|인자(Argument)|): Failure\n"
					,
					__FILE__, __LINE__
				);

				return -1;
			}
		} else {
			// 복사한다.
			if (byteCopy(&pByte, argPtr, argLen, packet) < 0) {
				(void) fprintf(stderr,
					"ERROR: "
					"__FILE__:= |%s|, __LINE_NO__ = %d: "
					"byteCopy(|인자(Argument)|): Failure\n"
					,
					__FILE__, __LINE__
				);

				return -1;
			}
		}
		// pByte 는 패킷의 마지막 다음 위치를 가리키거나
		// "추가인자"를 넣을 위치를 가리킨다.
		if (extraArgPtr) {
			// "추가인자"가 있다. (반드시 NUL 로 끝나는 문자열이어야한다.)
			size_t	extraArgLen = strlen(extraArgPtr) + 1;	// NUL 까지 포함해서 복사한다.
			if (byteCopy(&pByte, extraArgPtr, extraArgLen, packet) < 0) {
				(void) fprintf(stderr,
					"ERROR: "
					"__FILE__:= |%s|, __LINE_NO__ = %d: "
					"byteCopy(|추가인자|): Failure\n"
					,
					__FILE__, __LINE__
				);

				return -1;
			}
		}
		// pByte 는 패킷의 마지막 다음 위치를 가리킨다.
	}

	// 패킷의 크기를 적고 마무리한다.
	pByte = (BYTE*) packet.getBuf();
	saveInteger(&pByte, 3, packet.getDataSize() - 4);

	return 0;
}
int
CPacket::unpackOrder(
	const CBuffer&	packet,		// 여기에 패킹된 패킷이 들어있다.

	ORD_TYPE_T&	orderType,
	unsigned int&	orderID,

	unsigned int**	pXxxIDPtr,
	int&		isBinary,
	CBuffer&	argBuffer,
	CBuffer&	extraArgBuffer
)
{
	BYTE*	pByte = (BYTE*) packet.getBuf();

	unsigned int	packetSize;
	loadInteger(&pByte, 3, packetSize);
	// pByte 는 "명령바이트 헤더"를 가리킨다.

	// "명령바이트 헤더"
	if (*pByte & BYTE__DATA) {	// 성능데이터 또는 장애데이터
		(void) fprintf(stderr,
			"__FILE__:= |%s|, __LINE_NO__ = %d: "
			"BYTE__DATA: Set: |성능데이터|이거나 |장애데이터|이다\n"
			,
			__FILE__, __LINE__
		);

		return -1;
	}
	if (!(*pByte & BYTE__ORDER)) {	// 명령 패킷이 아닌 경우
		(void) fprintf(stderr,
			"__FILE__:= |%s|, __LINE_NO__ = %d: "
			"BYTE__ORDER: |명령 패킷|이 아니다\n"
			,
			__FILE__, __LINE__
		);

		return -1;
	}
	unsigned int	NXxxID = (unsigned int) -1;	// XXX__ID 가 차지하는 바이트 수 - 1
							// XXX__ID 가 설정되어있을 때만 의미가 있다.
	int	isXxxID = *pByte & BYTE_ORDER__XXX_ID;
	if (isXxxID) {
		NXxxID = (*pByte & BYTE_ORDER__XXX_ID_SIZE_MASK) >> BYTE_ORDER__XXX_ID_SIZE_SHIFT;
	}
	int	isArgument = *pByte & BYTE_ORDER__ARGUMENT;
	isBinary = *pByte & BYTE_ORDER__BINARY;
	int	isZipped = *pByte & BYTE_ORDER__ZIPPED;
	++pByte;
	// pByte 는 "명령유형 헤더"를 가리킨다.

	// "명령유형 헤더"
	unsigned int	NOrderID = (*pByte & BYTE_ORDER__ID_SIZE_MASK) >> BYTE_ORDER__ID_SIZE_SHIFT;
	orderType = (ORD_TYPE_T) ((*pByte & BYTE_ORDER__TYPE_MASK) >> BYTE_ORDER__TYPE_SHIFT);
	int	isExtraArgument = *pByte & BYTE_ORDER__EXTRA;
	++pByte;
	// pByte 는 ORDER__ID 를 담은 정수바이트의 시작을 가리킨다.

	loadInteger(&pByte, NOrderID, orderID);
	// pByte 는 패킷의 마지막 다음 위치를 가리키거나(XXX__ID 와 "인자" 모두가 없는 경우)
	// XXX__ID 를 담은 정수바이트의 시작 또는 "인자"의 시작위치를 가리킨다.

	if (isXxxID)
		loadInteger(&pByte, NXxxID, **pXxxIDPtr);
	else
		*pXxxIDPtr = NULL;
	// pByte 는 패킷의 마지막 다음 위치를 가리키거나("인자"가 없는 경우)
	// "인자"의 시작위치를 가리킨다.

	// "인자"를 담을 버퍼를 초기화한다.
	// "인자"가 없다면 데이터의 크기는 0이다.
	argBuffer.setDataSize(0);
	if (isArgument) {
		if (isZipped) {
			if (inflateByte(&pByte, argBuffer, isBinary) < 0) {
				(void) fprintf(stderr,
					"ERROR: "
					"__FILE__:= |%s|, __LINE_NO__ = %d: "
					"inflateByte(|인자(Argument)|): Failure\n"
					,
					__FILE__, __LINE__
				);

				return -1;
			}
		} else {
			size_t	argLen = strlen((const char*) pByte) + 1;	// NUL 까지 포함해서
			if (argBuffer.setBufSizeAtLeast(argLen) < 0) {
				(void) fprintf(stderr,
					"ERROR: "
					"__FILE__:= |%s|, __LINE_NO__ = %d: "
					"CBuffer::setBufSizeAtLeast(): Failure\n"
					,
					__FILE__, __LINE__
				);

				return -1;
			}
			(void*) memcpy(argBuffer.getBuf(), pByte, argLen);
			pByte += argLen;
			(void*) argBuffer.incDataSize(argLen);
		}
		// pByte 는 패킷의 마지막 다음 위치를 가리키거나
		// "추가인자"의 시작위치를 가리킨다.

		// "추가인자"를 담을 버퍼를 초기화한다.
		extraArgBuffer.setDataSize(0);
		// "추가인자"가 없다면 데이터의 크기는 0이다.
		if (isExtraArgument) {
			size_t	extraArgLen = strlen((const char*) pByte) + 1;	// NUL 까지 포함해서
			if (extraArgBuffer.setBufSizeAtLeast(extraArgLen) < 0) {
				(void) fprintf(stderr,
					"ERROR: "
					"__FILE__:= |%s|, __LINE_NO__ = %d: "
					"CBuffer::setBufSizeAtLeast(): Failure\n"
					,
					__FILE__, __LINE__
				);

				return -1;
			}
			(void*) memcpy(extraArgBuffer.getBuf(), pByte, extraArgLen);
			pByte += extraArgLen;
			(void*) extraArgBuffer.incDataSize(extraArgLen);
			// pByte 는 패킷의 마지막 다음 위치를 가리킨다.
		}
		// pByte 는 패킷의 마지막 다음 위치를 가리킨다.
	}
	// pByte 는 패킷의 마지막 다음 위치를 가리킨다.

	return packetSize == (unsigned int) (pByte - (BYTE*) packet.getBuf() - 4) ? 0 : -1;
}

int
CPacket::packResponse(
	CBuffer&	packet,		// 여기에다 패킹한다.
	int		isPyDict,	// "결과"가 Python dictionary 의 문자열 표현인가?
	RESP_TYPE_T	respType,
	unsigned int	orderID,
	const char*	returnPtr	// NULL 이 아니면 "결과(return)"이 있다.
)
{
	if (respType < RESP_TYPE__ACK || respType > RESP_TYPE__FAIL) {
		(void) fprintf(stderr,
			"ERROR: "
			"__FILE__:= |%s|, __LINE_NO__ = %d: "
			"respType = %d: Invalid\n"
			,
			__FILE__, __LINE__,
			respType
		);

		return -1;
	}

	// 버퍼를 초기화한다.
	(void*) packet.setDataSize(0);
	// pByte 는 버퍼의 시작위치를 가리킨다.
	BYTE*	pByte = (BYTE*) packet.getBuf();

	// 패킷을 넣을 공간을 충분히 확보한다.
	//	패킹의 총 크기: 4
	//	응답바이트 헤더: 1
	//	응답유형 헤더: 1
	//	ORDER__ID: (최대) 4
	//	압축될 바이트의 크기: 4
	if (!packet.setBufSizeAtLeast(14)) {
		(void) fprintf(stderr,
			"FATAL: "
			"__FILE__:= |%s|, __LINE_NO__ = %d: "
			"CBuffer::setBufSizeAtLeast(14): Failure\n"
			,
			__FILE__, __LINE__
		);

		return -1;
	}

	// 패킷의 크기를 적을 공간은 지금은 건너뛴다.
	// 제일 마지막에 패킷을 마무리한 후 적을 것이다.
	pByte += 4;
	(void*) packet.setDataSize(4);
	// pByte 는 "응답바이트 헤더"를 가리킨다.

	// "응답바이트 헤더"
	*pByte = 0x00;	// 모든 비트를 클리어한다.
	int	isZipped = -1;	// "결과"가 있을 때에만 의미가 있다.
		// NUL 로 끝나는 문자열인 경우 길이가 ZIPPED_STRING_LENGTH_LOWER_BOUND 보다 커야 압축된다.
	size_t	returnLen = (size_t) -1;	// "결과"의 바이트 수 (NUL을 제외한)
				// "결과"가 있을 때에만 의미가 있다.
				// 압축하는 경우에 사용된다.
	if (returnPtr) {	// "결과"가 있다.
		*pByte |= BYTE_RESP__RETURN;
		returnLen = strlen(returnPtr) + 1;	// NUL 까지 포함한다.
		isZipped = returnLen > ZIPPED_STRING_LENGTH_LOWER_BOUND ? 1 : 0;
		if (isZipped)
			*pByte |= BYTE_RESP__ZIPPED;
		if (isPyDict)
			*pByte |= BYTE_RESP__PY_DICT;
	}
	++pByte;
	(void*) packet.incDataSize(1);
	// pByte 는 "응답유형 헤더"를 가리킨다.

	// "응답유형 헤더"
	*pByte = 0x00;	// 모든 비트를 클리어한다.
	// ORDER__ID 의 크기 비트를 설정한다.
	unsigned int	NOrderID = getNByte(orderID);
	*pByte |= NOrderID << BYTE_RESP__ID_SIZE_SHIFT;
	// 응답유형을 설정한다.
	*pByte |= (respType << BYTE_RESP__TYPE_SHIFT) & BYTE_RESP__TYPE_MASK;
	++pByte;
	(void*) packet.incDataSize(1);
	// pByte 는 ORDER__ID 를 넣을 자리를 가리킨다.

	// ORDER__ID 를 쓴다.
	saveInteger(&pByte, NOrderID, orderID);
	(void*) packet.incDataSize(1 + NOrderID);
	// pByte 는 패킷의 마지막 다음 위치를 가리키거나("결과"가 없는 경우)
	// "결과"가 있을 때는 "결과"를 넣을 위치를 가리킨다.

	if (returnPtr) {
		if (isZipped) {
			// 압축한다.
			if (deflateByte(&pByte, packet, returnPtr, returnLen) < 0) {
				(void) fprintf(stderr,
					"ERROR: "
					"__FILE__:= |%s|, __LINE_NO__ = %d: "
					"deflateByte(|결과(Return)|): Failure\n"
					,
					__FILE__, __LINE__
				);

				return -1;
			}
		} else {
			if (byteCopy(&pByte, returnPtr, returnLen, packet) < 0) {
				(void) fprintf(stderr,
					"ERROR: "
					"__FILE__:= |%s|, __LINE_NO__ = %d: "
					"byteCopy(|결과(Return)|): Failure\n"
					,
					__FILE__, __LINE__
				);

				return -1;
			}
		}
		// pByte 는 패킷의 마지막 다음 위치를 가리킨다.
	}

	// 패킷의 크기를 적고 마무리한다.
	pByte = (BYTE*) packet.getBuf();
	saveInteger(&pByte, 3, packet.getDataSize() - 4);

	return 0;
}
int
CPacket::unpackResponse(
	const CBuffer&	packet,		// 패킹된 패킷은 여기에 들어있다.

	int&		isPyDict,
	RESP_TYPE_T&	respType,
	unsigned int&	orderID,
	CBuffer&	returnBuffer	// "결과"는 여기에 저장된다.
					// 결과가 없는 경우, 버퍼의 크기는 0이다.
)
{
	BYTE*	pByte = (BYTE*) packet.getBuf();

	unsigned int	packetSize;
	loadInteger(&pByte, 3, packetSize);
	// pByte 는 "응답바이트 헤더"를 가리킨다.

	// "응답바이트 헤더"
	if (*pByte & BYTE__DATA) {	// 성능데이터 또는 장애데이터
		(void) fprintf(stderr,
			"__FILE__:= |%s|, __LINE_NO__ = %d: "
			"BYTE__DATA: Set: |성능데이터|이거나 |장애데이터|이다\n"
			,
			__FILE__, __LINE__
		);

		return -1;
	}
	if (*pByte & BYTE__ORDER) {	// 응답 패킷이 아닌 경우
		(void) fprintf(stderr,
			"__FILE__:= |%s|, __LINE_NO__ = %d: "
			"BYTE__ORDER: Set: |응답 패킷|이 아니다\n"
			,
			__FILE__, __LINE__
		);

		return -1;
	}
	int	isReturn = *pByte & BYTE_RESP__RETURN;
	int	isZipped = *pByte & BYTE_RESP__ZIPPED;
	isPyDict = *pByte & BYTE_RESP__PY_DICT;
	++pByte;
	// pByte 는 "응답유형 헤더"를 가리킨다.

	// "응답유형 헤더"
	unsigned int	NOrderID = (*pByte & BYTE_RESP__ID_SIZE_MASK) >> BYTE_RESP__ID_SIZE_SHIFT;
	respType = (RESP_TYPE_T) ((*pByte & BYTE_RESP__TYPE_MASK) >> BYTE_RESP__TYPE_SHIFT);
	++pByte;
	// pByte 는 ORDER__ID 를 담은 정수바이트의 시작을 가리킨다.

	loadInteger(&pByte, NOrderID, orderID);
	// pByte 는 패킷의 마지막 다음 위치를 가리키거나
	// "결과"의 시작위치를 가리킨다.

	// "결과"를 저장한 버퍼를 초기화한다.
	returnBuffer.setDataSize(0);
	// "결과"가 없다면 데이터의 크기는 0이다.
	if (isReturn) {
		if (isZipped) {
			if (inflateByte(&pByte, returnBuffer) < 0) {
				(void) fprintf(stderr,
					"ERROR: "
					"__FILE__:= |%s|, __LINE_NO__ = %d: "
					"inflateByte(|결과(Return)|): Failure\n"
					,
					__FILE__, __LINE__
				);

				return -1;
			}
		} else {
			size_t	returnLen = strlen((const char*) pByte) + 1;	// NUL 까지 포함해서
			if (returnBuffer.setBufSizeAtLeast(returnLen) < 0) {
				(void) fprintf(stderr,
					"ERROR: "
					"__FILE__:= |%s|, __LINE_NO__ = %d: "
					"CBuffer::setBufSizeAtLeast(): Failure\n"
					,
					__FILE__, __LINE__
				);

				return -1;
			}
			(void*) memcpy(returnBuffer.getBuf(), pByte, returnLen);
			pByte += returnLen;
			(void*) returnBuffer.incDataSize(returnLen);
		}
		// pByte 는 패킷의 마지막 다음 위치를 가리킨다.
	}
	// pByte 는 패킷의 마지막 다음 위치를 가리킨다.

	int	iReturnVal = 0;
	if (packetSize != (unsigned int) (pByte - (BYTE*) packet.getBuf() - 4)) {
		eprintf(E_FATAL, EFMT("packetSize(= %d) != ...%d\n"), packetSize, pByte - (BYTE*) packet.getBuf() - 4);
		iReturnVal = -1;
	}
	return iReturnVal;
}
