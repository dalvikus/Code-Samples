#include <gmTCP/gmTCP.h>

gmQueue::gmQueue(
	UINT	QueueSizeInExponent,
	PINT	lpgmStatus
)
{
	INT	iReturnVal = 0;

	// queueSizeInExponent 의 최대값을 얼마로 할까?
	// 10: 2^10 = 1K
	// 12: 2^12 = 4K
	// 13: 2^13 = 8K
	// ...
	m__exponent	= QueueSizeInExponent;
	m__mask		= 0;
	for (UINT i = 0; i < QueueSizeInExponent; ++i)
		m__mask |= 1 << i;
	m__size		= 1 << QueueSizeInExponent;

	m__head		= 0;
	m__tail		= 0;
	m__length	= 0;

	m__lpBuf = new CHAR[m__size];
	if (m__lpBuf == 0)
		iReturnVal = -1;

	if (lpgmStatus != 0)
		*lpgmStatus = iReturnVal;
}

gmQueue::~gmQueue()
{
	if (m__lpBuf) {
		delete m__lpBuf;
//		free(m__lpBuf);
		m__lpBuf = 0;
	}
}

INT
gmQueue::mf__Push(
#ifdef __gmCRYPT__	// 2.2
	HCRYPTKEY	hEncryptKey,
#endif	// __gmCRYPT__	// 2.2
	PMTCPMSG	pMTCPMsg
)
{
	if (m__length + sizeof(MTCPHEADER) + pMTCPMsg->Header.cbData > m__size) {
		// 충분한 공간이 없다.
		return TCPBUF__NOT_ENOUGH;
	}

	UINT	i;
	PBYTE	lpC = (PBYTE) pMTCPMsg;
	for (i = 0; i < sizeof(MTCPHEADER); ++i) {
		m__lpBuf[m__tail] = *lpC++;
		m__tail = (m__tail + 1) & m__mask;
	}
#ifdef __gmCRYPT__	// 2.2
	DWORD	cbDataLength = pMTCPMsg->Header.cbData;
	if (!CryptEncrypt(hEncryptKey, NULL, FALSE, 0, pMTCPMsg->pbData, &cbDataLength, cbDataLength)) {
		return TCPBUF__CRYPT_ERROR;
	}
#endif	// __gmCRYPT__	// 2.2
#if 0
	lpC = lpTCPBuf->pbData;
	for (i = 0; i < lpTCPBuf->Header.cbData; ++i) {
		m__lpBuf[m__tail] = *lpC++;
		m__tail = (m__tail + 1) & m__mask;
	}
#else
	lpC = pMTCPMsg->pbData;
	UINT	cbData = pMTCPMsg->Header.cbData;
	if (m__tail + cbData > m__size) {
		// 두 번 나누어서 복사
		// 1. m__size - m__tail
		// 2. cbData - (m__size - m__tail)
		UINT	myHBytes = m__size - m__tail;
		memcpy(&m__lpBuf[m__tail], lpC, myHBytes);
		lpC += myHBytes;
		cbData -= myHBytes;
		m__tail = 0;
	}
	memcpy(&m__lpBuf[m__tail], lpC, cbData);
	m__tail = (m__tail + cbData) & m__mask;
#endif

	m__length += sizeof(MTCPHEADER) + pMTCPMsg->Header.cbData;

	return TCPBUF__OK;
}

INT
gmQueue::mf__Shift(
	PMTCPMSG	pMTCPMsg	// pMTCPMsg->Header.cbData (IN/OUT)
)
{
	MTCPHEADER	Header;
	INT	iReturnVal = mf__ReadHeader(&Header);
	if (iReturnVal != TCPBUF__OK)
		return iReturnVal;

	if (Header.cbData > pMTCPMsg->Header.cbData) {
		// 데이터를 복사할 버퍼의 크기가 충분하지 않다.
		return TCPBUF__NOT_ENOUGH;
	}

	// 헤더를 복사
	memcpy(&pMTCPMsg->Header, &Header, sizeof(MTCPHEADER));
	m__head = (m__head + sizeof(MTCPHEADER)) & m__mask;

	// 데이터를 복사 & 큐의 정보(m__head) 변경
	PBYTE	lpC = pMTCPMsg->pbData;
	UINT	cbData = Header.cbData;
	if (m__head + cbData > m__size) {
		// 두 번 나누어서 복사
		// 1. m__size - m__head
		// 2. cbData - (m__size - m__head)
		UINT	myHBytes = m__size - m__head;
		memcpy(lpC, &m__lpBuf[m__head], myHBytes);
		lpC += myHBytes;
		cbData -= myHBytes;
		m__head = 0;
	}
	memcpy(lpC, &m__lpBuf[m__head], cbData);
	m__head = (m__head + cbData) & m__mask;

	// 큐의 정보(m__length) 변경
	m__length -= sizeof(MTCPHEADER) + Header.cbData;

	return TCPBUF__OK;
}

INT
gmQueue::mf__ShiftEx(
	PMTCPMSG	pMTCPMsg,	// pMTCPMsg->Header.cbData (IN/OUT)
	PUINT		lpMsgLen
)
{
	MTCPHEADER	Header;
	INT	iReturnVal = mf__ReadHeader(&Header);
	if (iReturnVal != TCPBUF__OK)
		return iReturnVal;

	if (Header.cbData > pMTCPMsg->Header.cbData) {
		// 데이터를 복사할 버퍼의 크기가 충분하지 않다.
		return TCPBUF__NOT_ENOUGH;
	}

	// 헤더정보를 복사
	memcpy(&pMTCPMsg->Header, &Header, sizeof(MTCPHEADER));

	// 데이터를 복사 & 큐의 정보(m__head) 변경
	UINT	prevHead = m__head;		// 이전의 m__head를 저장한다. lpMsgLen != NULL일 때에만 의미가 있다.
	m__head = (m__head + sizeof(MTCPHEADER)) & m__mask;
	PBYTE	lpC = pMTCPMsg->pbData;
	UINT	cbData = Header.cbData;
	if (m__head + cbData > m__size) {
		// 두 번 나누어서 복사
		// 1. m__size - m__head
		// 2. cbData - (m__size - m__head)
		UINT	myHBytes = m__size - m__head;
		memcpy(lpC, &m__lpBuf[m__head], myHBytes);
		lpC += myHBytes;
		cbData -= myHBytes;
		m__head = 0;
	}
	memcpy(lpC, &m__lpBuf[m__head], cbData);
	m__head = (m__head + cbData) & m__mask;

	// 큐의 정보(m__length) 변경
	if (lpMsgLen) {
		m__head = prevHead;
		*lpMsgLen = sizeof(MTCPHEADER) + Header.cbData;
	} else
		m__length -= sizeof(MTCPHEADER) + Header.cbData;

	return TCPBUF__OK;
}

INT
gmQueue::mf__ShiftEx(
	UINT	MTCPMsgLen
)
{
	MTCPHEADER	Header;
	INT	iReturnVal = mf__ReadHeader(&Header);
	if (
		iReturnVal != TCPBUF__OK ||
		sizeof(MTCPHEADER) + Header.cbData != MTCPMsgLen
	) {
		return TCPBUF__BAD_PARAMETER;
	}

	m__head = (m__head + MTCPMsgLen) & m__mask;
	m__length -= MTCPMsgLen;
	return TCPBUF__OK;
}

INT
gmQueue::mf__ReadHeader(
	PMTCPHEADER	pHeader
)
{
	DWORD	dwMessage = 0;

	if (m__length < sizeof(MTCPHEADER)) {
		// 헤더정보가 없다.
		return TCPBUF__NO_HEADER;
	}

	// 헤더정보를 읽는다.
	MTCPHEADER	Header;
	PBYTE	pByte = (PBYTE) &Header;
	for (UINT i = 0; i < sizeof(MTCPHEADER); ++i) {
		*pByte++ = m__lpBuf[(m__head + i) & m__mask];
	}

	if (sizeof(MTCPHEADER) + Header.cbData > m__length) {
		// 모든 데이터가 들어오지 않았다.
		return TCPBUF__PARTIAL_DATA;
	}

	// 헤더정보를 복사한다.
	memcpy(pHeader, &Header, sizeof(MTCPHEADER));
	return TCPBUF__OK;
}

#ifdef __gmUSE_PUBLIC_QUEUE__
INT
gmQueue::mf__PushP(
	LPTCPPBUF	lpTCPPBuf
)
{
	USHORT	ucbData = lpTCPPBuf->TCPBuf.Header.cbData;

	if (m__length + 8 + ucbData > m__size) {
		// 충분한 공간이 없다.
		return TCPBUF__NOT_ENOUGH;
	}

	UINT	i;
	PBYTE	lpC = (PBYTE) lpTCPPBuf;
	for (i = 0; i < 8; ++i) {
		m__lpBuf[m__tail] = *lpC++;
		m__tail = (m__tail + 1) & m__mask;
	}
#if 0
	lpC = lpTCPPBuf->TCPBuf.pbData;
	for (i = 0; i < ucbData; ++i) {
		m__lpBuf[m__tail] = *lpC++;
		m__tail = (m__tail + 1) & m__mask;
	}
#else
	lpC = lpTCPPBuf->TCPBuf.pbData;
	UINT	cbData = lpTCPPBuf->TCPBuf.Header.cbData;
	if (m__tail + cbData > m__size) {
		// 두 번 나누어서 복사
		// 1. m__size - m__tail
		// 2. cbData - (m__size - m__tail)
		UINT	myHBytes = m__size - m__tail;
		memcpy(&m__lpBuf[m__tail], lpC, myHBytes);
		lpC += myHBytes;
		cbData -= myHBytes;
		m__tail = 0;
	}
	memcpy(&m__lpBuf[m__tail], lpC, cbData);
	m__tail = (m__tail + cbData) & m__mask;
#endif

	m__length += 8 + ucbData;

	return TCPBUF__OK;
}

INT
gmQueue::mf__ShiftP(
	LPTCPPBUF	lpTCPPBuf	// lpTCPPBuf->TCPBuf.Header.cbData (IN/OUT)
)
{
	if (m__length < 8) {
		// 헤터정보가 없다.
		return TCPBUF__NO_HEADER;
	}

	CHAR	cHeader[8];	// index(4 bytes) + hType(2 bytes) + cbData(2 bytes)
	PUSHORT	lpcbData	= (PUSHORT) cHeader + 3;

	for (UINT i = 0; i < 8; ++i)
		cHeader[i] = m__lpBuf[(m__head + i) & m__mask];

	if (*lpcbData > lpTCPPBuf->TCPBuf.Header.cbData) {
		// 데이터를 복사할 버퍼의 크기가 충분하지 않다.
		return TCPBUF__NOT_ENOUGH;
	}

	if ((UINT) (8 + *lpcbData) > m__length) {
		// 모든 데이터가 들어오지 않았다.
		return TCPBUF__PARTIAL_DATA;
	}

	// 헤더정보를 복사
	*((PUINT) lpTCPPBuf) = *((PUINT) cHeader);
	*((PUINT) lpTCPPBuf + 1) = *((PUINT) cHeader + 1);
	m__head = (m__head + 8) & m__mask;
	// 데이터를 복사
#if 0
	for (i = 0; i < *lpcbData; ++i) {
		lpTCPPBuf->TCPBuf.pbData[i] = m__lpBuf[m__head];
		m__head = (m__head + 1) & m__mask;
	}
#else
	PBYTE	lpC = lpTCPPBuf->TCPBuf.pbData;
	UINT	cbData = *lpcbData;
	if (m__head + cbData > m__size) {
		// 두 번 나누어서 복사
		// 1. m__size - m__head
		// 2. cbData - (m__size - m__head)
		UINT	myHBytes = m__size - m__head;
		memcpy(lpC, &m__lpBuf[m__head], myHBytes);
		lpC += myHBytes;
		cbData -= myHBytes;
		m__head = 0;
	}
	memcpy(lpC, &m__lpBuf[m__head], cbData);
	m__head = (m__head + cbData) & m__mask;
#endif

	m__length -= 8 + *lpcbData;

	return TCPBUF__OK;
}

INT
gmQueue::mf__ShiftPEx(
	LPTCPPBUF	lpTCPPBuf,	// lpTCPPBuf->TCPBuf.Header.cbData (IN/OUT)
	PUINT		lpMsgLen
)
{
	if (m__length < 8) {
		// 헤터정보가 없다.
		return TCPBUF__NO_HEADER;
	}

	CHAR	cHeader[8];	// index(4 bytes) + hType(2 bytes) + cbData(2 bytes)
	PUSHORT	lpcbData	= (PUSHORT) cHeader + 3;

	for (UINT i = 0; i < 8; ++i)
		cHeader[i] = m__lpBuf[(m__head + i) & m__mask];

	if (*lpcbData > lpTCPPBuf->TCPBuf.Header.cbData) {
		// 데이터를 복사할 버퍼의 크기가 충분하지 않다.
		return TCPBUF__NOT_ENOUGH;
	}

	if ((UINT) (8 + *lpcbData) > m__length) {
		// 모든 데이터가 들어오지 않았다.
		return TCPBUF__PARTIAL_DATA;
	}

	// 예전의 Head 값을 기억
	UINT	PrevHead = m__head;

	// 헤더정보를 복사
	*((PUINT) lpTCPPBuf) = *((PUINT) cHeader);
	*((PUINT) lpTCPPBuf + 1) = *((PUINT) cHeader + 1);
	m__head = (m__head + 8) & m__mask;
	// 데이터를 복사
#if 0
	for (i = 0; i < *lpcbData; ++i) {
		lpTCPPBuf->TCPBuf.pbData[i] = m__lpBuf[m__head];
		m__head = (m__head + 1) & m__mask;
	}
#else
	PBYTE	lpC = lpTCPPBuf->TCPBuf.pbData;
	UINT	cbData = *lpcbData;
	if (m__head + cbData > m__size) {
		// 두 번 나누어서 복사
		// 1. m__size - m__head
		// 2. cbData - (m__size - m__head)
		UINT	myHBytes = m__size - m__head;
		memcpy(lpC, &m__lpBuf[m__head], myHBytes);
		lpC += myHBytes;
		cbData -= myHBytes;
		m__head = 0;
	}
	memcpy(lpC, &m__lpBuf[m__head], cbData);
	m__head = (m__head + cbData) & m__mask;
#endif

	if (lpMsgLen) {
		m__head = PrevHead;
		*lpMsgLen = 8 + *lpcbData;
	} else
		m__length -= 8 + *lpcbData;

	return TCPBUF__OK;
}
#endif	// __gmUSE_PUBLIC_QUEUE__
