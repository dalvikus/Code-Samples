#include <gmTCP/gmTCP.h>

static
void
ByteToStringA(
	const PBYTE	pByte,
	DWORD		dwLength,
	LPSTR		pszStringA
)
{
	PBYTE	pB = pByte;
	PBYTE	pS = (PBYTE) pszStringA;
	for (DWORD i = 0; i < dwLength; ++i, ++pB, ++pS) {
		BYTE	Byte = *pB;
		// 상위 4비트
		Byte >>= 4;
		*pS++ = Byte < 10 ? '0' + Byte : 'a' + (Byte - 10);
		// 하위 4비트
		Byte = *pB & 0x0F;
		*pS = Byte < 10 ? '0' + Byte : 'a' + (Byte - 10);
	}
	*pS = '\0';
}

////////////////////////////////////////////////////////////////////////////////
// 2003/10/18
// gmPseudoTCP오브젝트의 생성과 소멸에 사용되는 멤버함수들의 정의
INT
gmPseudoTCP::mf__Initialize(
	gmTCPBase	*lpTCPBase,
	UINT	globalIndex,
	UINT	localIndex,
	UINT	index,
	UINT	RecvSize,
	UINT	SendSize
)
{
	// 2005/3/24 (EST) - ghkim
	// 매 m__dwMsgUnit메세지가 들어올 때마다 알린다.
	// gmTCPBase가 설정되면 상수이다.
	// Access를 빨리하기 위해서 각 gmPseudoTCP마다 저장한다.
	// 나중에 각 gmPseudoTCP마다 다르게 할 수도 있다.
	m__dwMsgUnit	= lpTCPBase->m__dwMsgUnit;

	m__lpTCPBase	= lpTCPBase;
	m__globalIndex	= globalIndex;
	m__localIndex	= localIndex;
	m__index	= index;
	m__ID		= (DWORD) ~0;
	m__gmPseudoTCPStatus	= gmPseudoTCPSTATUS__NOT_CONNECTED;
	m__lpRecv	= NULL;
	m__lpSend	= NULL;

	INT	iReturnVal = 0;
	INT	gmStatus;

	// 2005/4/6 (EST) - ghkim
#ifdef __gmTLS__
	m__bValidContext	= FALSE;	// 유효한 Context가 아니다.
#endif	// __gmTLS__

	m__lpTCPMsgBuf = new CHAR[lpTCPBase->mf__GetMaxTCPMsgSize()];
	if (!m__lpTCPMsgBuf) {
		iReturnVal = -1;
		goto NEXT;
	}

	// 주어진 소켓이 사용할 "받는" 큐의 설정
	m__lpRecv = new gmTrans(RecvSize, &gmStatus);
	if (m__lpRecv == 0 || gmStatus == -1) {
		m__lpTCPBase->mf__fprintf(m__lpTCPBase->m__CONERR,
			TEXT(".m__lpRecv = new gmTrans(...): 실패!\n")
		);
		if (m__lpRecv) {
			delete m__lpRecv;
			m__lpRecv = 0;
		}
		iReturnVal = -1;
		goto NEXT;
	}

	// 주어진 소켓이 사용할 "보내는" 큐의 설정
	m__lpSend = new gmTrans(SendSize, &gmStatus);
	if (m__lpSend == NULL || gmStatus == -1) {
		m__lpTCPBase->mf__fprintf(m__lpTCPBase->m__CONERR,
			TEXT(".m__lpSend = new gmTrans(...): 실패!\n")
		);
		if (m__lpSend) {
			delete m__lpSend;
			m__lpSend = 0;
		}
		iReturnVal = -1;
		goto NEXT;
	}

NEXT:
	if (iReturnVal == -1) {
		if (m__lpRecv) {
			delete m__lpRecv;
			m__lpRecv = 0;
		}
		if (m__lpSend) {
			delete m__lpSend;
			m__lpSend = 0;
		}
	}

	mf__Reset();

	return iReturnVal;
}

VOID
gmPseudoTCP::mf__Reset(
	VOID
)
{
	// 2005/4/6 (EST) - ghkim
#ifdef __gmTLS__
	// Context핸들에 할당된 메모리를 해제한다.
	if (m__bValidContext && m__lpTCPBase->mf__GetPSFT()->DeleteSecurityContext(&m__hContext) != SEC_E_OK) {
		// 여기서 문제가 생기면 어떻게 하나...
		m__lpTCPBase->mf__RomeoMustDie();
	}
	m__bValidContext	= FALSE;
#endif	// __gmTLS__

	gmQueue	*lpQueue;

	lpQueue = m__lpRecv->m__lpQueue;
	lpQueue->m__head = 0;
	lpQueue->m__tail = 0;
	lpQueue->m__length = 0;

	lpQueue = m__lpSend->m__lpQueue;
	lpQueue->m__head = 0;
	lpQueue->m__tail = 0;
	lpQueue->m__length = 0;

	m__dwOOB			= 0;
	// 2005/3/22 (EST) - ghkim
	m__MessageIncompleteHead	= 0;
	m__MessageIncompleteLength	= 0;
	// 2005/3/24 (EST) - ghkim
	m__dwUID			= m__lpTCPBase->m__dwUID;
	// 2005/3/35 (EST) - ghkim
	//	버그! 다음 값도 매 연결마다 초기화되어야 한다.
	m__bIOCPSendInProgress	= FALSE;
}

VOID
gmPseudoTCP::mf__Finalize(
	VOID
)
{
	// 2005/4/6 (EST) - ghkim
#ifdef __gmTLS__
	// Context핸들에 할당된 메모리를 해제한다.
	if (m__bValidContext && m__lpTCPBase->mf__GetPSFT()->DeleteSecurityContext(&m__hContext) != SEC_E_OK) {
		// 여기서 문제가 생기면 어떻게 하나...
		m__lpTCPBase->mf__RomeoMustDie();
	}
	m__bValidContext	= FALSE;
#endif	// __gmTLS__

	if (m__lpTCPMsgBuf) {
		delete m__lpTCPMsgBuf;
		m__lpTCPMsgBuf = 0;
	}

	if (m__lpRecv) {
		delete m__lpRecv;
		m__lpRecv = 0;
	}
	if (m__lpSend) {
		delete m__lpSend;
		m__lpSend = 0;
	}
}
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// WSARecv, WSASend에 사용되는 멤버함수들의 정의
DWORD
gmPseudoTCP::mf__SetRecvBuf(
	DWORD	*lpBytes
)
{
	UINT	dwBufferCount;

	gmQueue	*lpgmQueue = m__lpRecv->m__lpQueue;

	UINT	head = lpgmQueue->m__head;
	UINT	tail = lpgmQueue->m__tail;

	// 1. "받는" 큐가 비어있으면 head = tail 이다.
	// 이 경우는 head = 0 인 경우를 제외하면, 채워져야 할 부분이
	// 두 분이라고 보는 것이 타당하다.
	// 2. "받는" 큐가 가득찬 경우에도 head = tail 이다.
	if (lpgmQueue->m__length == lpgmQueue->m__size)
		return 0;

	if (tail >= head) {
/*
		────────
		──■━━━━━
		━━━━━━━━
		━━━━━━━━
		━━━━□───
	■	head
	□	tail
	━	채워져 있는 부분
	─	채워져야 하는 부분
 */
		// head = 0 일때는?
		m__lpRecv->m__WSABuf[0].len = lpgmQueue->m__size - tail;
		m__lpRecv->m__WSABuf[0].buf = lpgmQueue->m__lpBuf + tail;
		// head = 0 인 경우에는 버퍼의 길이(len)가 0 이므로 상관없다!
		m__lpRecv->m__WSABuf[1].len = head;
		m__lpRecv->m__WSABuf[1].buf = lpgmQueue->m__lpBuf;
		*lpBytes = lpgmQueue->m__size - tail + head;
		dwBufferCount = 2;
	} else {		// tail < head
/*
		━━━━━━━━
		━━□─────
		────────
		────────
		────■━━━
	■	head
	□	tail
	━	채워져 있는 부분
	─	채워져야 할 부분
 */
		m__lpRecv->m__WSABuf[0].len = head - tail;
		m__lpRecv->m__WSABuf[0].buf = lpgmQueue->m__lpBuf + tail;
		*lpBytes = head - tail;
		dwBufferCount = 1;
	}

	return dwBufferCount;
}

// 리턴값
//	-1	WSARecv에서 문제 발생(자세한 것은 m__WSALastError를 보라.)
//	0	WSARecv에서 데이터를 곧 바로 읽었다.
//	1	WSARecv에서 데이터를 읽는 중이다.
//	-2	받는 큐가 차서 메세지를 받을 수 없다! (문제가 있군.)
gmPseudoTCP::MSGTCP_IOCPRECV_XXX
gmPseudoTCP::mf__IOCPRecv(
	VOID
)
{
	DWORD	dwBytes;
	DWORD	dwBufCount = mf__SetRecvBuf(&dwBytes);
	if (dwBufCount == 0) {
#ifdef _DEBUG
		m__lpTCPBase->mf__fprintf(m__lpTCPBase->m__CONERR,
			TEXT("\"받는\" 큐가 차서 더 이상 메세지를 받을 수 없다.\n")
		);
#endif	// _DEBUG
		return MSGTCP_IOCPRECV_BUFFER_FULL;
	}

	INT	WSALastErrorCode = 0;
	DWORD	dwBytesReceived;
	m__lpRecv->m__WSAOverlappedPlus.mWSAOverlapped.Internal		= 0;
	m__lpRecv->m__WSAOverlappedPlus.mWSAOverlapped.InternalHigh	= 0;
	m__lpRecv->m__WSAOverlappedPlus.mWSAOverlapped.Offset		= 0;
	m__lpRecv->m__WSAOverlappedPlus.mWSAOverlapped.OffsetHigh	= 0;
//	m__lpRecv->m__WSAOverlappedPlus.mWSAOverlapped.hEvent		= NULL;
	m__lpRecv->m__WSAOverlappedPlus.mIOOpCode			= gmTCPBase::IOOP_RD;
	m__lpRecv->m__WSAOverlappedPlus.u.mBytesToBeRead		= dwBytes;
//	m__lpRecv->m__dwFlags = MSG_OOB;
	if (WSARecv(
		m__sock,
		m__lpRecv->m__WSABuf,
		dwBufCount,
		&dwBytesReceived,
		&m__lpRecv->m__dwFlags,
		(LPWSAOVERLAPPED) &m__lpRecv->m__WSAOverlappedPlus,
		NULL		// CompletionROUTINE 없이...
	) == SOCKET_ERROR && (WSALastErrorCode = WSAGetLastError()) != WSA_IO_PENDING) {
#ifdef _DEBUG
		_TCHAR	Message[512];
		DWORD	BufferLength0 = 512;	// NULL문자를 포함한 Message버퍼의 크기(문자 단위)
		(VOID) m__lpTCPBase->mf__GetWSAErrorStringFromCode(Message, &BufferLength0, WSALastErrorCode);
		m__lpTCPBase->mf__fprintf(m__lpTCPBase->m__CONERR,
			TEXT("[%u] mf__IOCPRecv(): ")
			TEXT("WSARecv() failed!: ")
			TEXT("WSAGetLastError() = %d(%s)\n"),
			m__localIndex,
			WSALastErrorCode,
			Message
		);
#endif	// _DEBUG
		m__lpTCPBase->mf__SetWSALastErrorCode(WSALastErrorCode);
		return MSGTCP_IOCPRECV_WSARECV_ERROR;
	}

	return WSALastErrorCode == WSA_IO_PENDING ? MSGTCP_IOCPRECV_PENDING_OK : MSGTCP_IOCPRECV_IMMEDIATELY_OK;
}


DWORD
gmPseudoTCP::mf__SetSendBuf(
	DWORD	*lpBytes
)
{
	UINT	dwBufferCount;

	gmQueue	*lpgmQueue = m__lpSend->m__lpQueue;

	UINT	head = lpgmQueue->m__head;
	UINT	tail = lpgmQueue->m__tail;

	// 1. "보내는" 큐가 가득 차있으면 head = tail 이다.
	// 이 경우는 tail = 0 인 경우를 제외하면, 채워져야 할 부분이
	// 두 분이라고 보는 것이 타당하다.
	// 2. "보내는" 큐가 비어있어도 head = tail 이다.
	if (lpgmQueue->m__length == 0)
		return 0;

	if (head >= tail) {
/*
		━━━━━━━━
		━━□─────
		────────
		────────
		────■━━━
	■	head
	□	tail
	━	보내져야 할 부분
	─	아무것도 없는 부분
 */
		// tail = 0 일때는?
		m__lpSend->m__WSABuf[0].len = lpgmQueue->m__size - head;
		m__lpSend->m__WSABuf[0].buf = lpgmQueue->m__lpBuf + head;
		// tail = 0 인 경우에는 버퍼의 길이(len)가 0 이므로 상관없다!
		m__lpSend->m__WSABuf[1].len = tail;
		m__lpSend->m__WSABuf[1].buf = lpgmQueue->m__lpBuf;

		*lpBytes = lpgmQueue->m__size - head + tail;
		dwBufferCount = 2;
	} else {		// head < tail
/*
		────────
		──■━━━━━
		━━━━━━━━
		━━━━━━━━
		━━━━□───
	■	head
	□	tail
	━	보내져야 할 부분
	─	아무것도 없는 부분
 */
		m__lpSend->m__WSABuf[0].len = tail - head;
		m__lpSend->m__WSABuf[0].buf = lpgmQueue->m__lpBuf + head;

		*lpBytes = tail - head;
		dwBufferCount = 1;
	}
	////////                                                ////////
	////////////////////////////////////////////////////////////////

	return dwBufferCount;
}

// 리턴값
//	-1	WSASend에서 문제 발생(자세한 것은 m__WSALastError를 보라.)
//	0	WSASend에서 데이터를 곧 바로 보냈다.
//	1	WSASend에서 데이터를 보내는 중이다.
//	2	보낼 데이터가 없다.
INT
gmPseudoTCP::mf__IOCPSend(
	VOID
)
{
	DWORD	dwBytes;
	DWORD	dwBufferCount = mf__SetSendBuf(&dwBytes);
	if (dwBufferCount == 0) {
		return 2;
	}

	INT	WSALastErrorCode	= 0;
	DWORD	dwNumberOfBytesSent;
	m__lpSend->m__WSAOverlappedPlus.mWSAOverlapped.Internal		= 0;
	m__lpSend->m__WSAOverlappedPlus.mWSAOverlapped.InternalHigh	= 0;
	m__lpSend->m__WSAOverlappedPlus.mWSAOverlapped.Offset		= 0;
	m__lpSend->m__WSAOverlappedPlus.mWSAOverlapped.OffsetHigh	= 0;
	// 2003/10/30
	// 이벤트방식을 쓰는 클라이언트의 경우에는 이 값이
	// 초기에 상수로서 지정된다. 이 값이 NULL냐 아니냐에 따라서
	// 이벤트방식을 사용하는 지, IOCP방식을 사용하는 지를 알 수 있다.
	// 물론 클라이언트의 경우에.
//	m__lpSend->m__WSAOverlappedPlus.mWSAOverlapped.hEvent		= NULL;
	m__lpSend->m__WSAOverlappedPlus.mIOOpCode			= gmTCPBase::IOOP_WR;
	m__lpSend->m__WSAOverlappedPlus.u.mBytesToBeWritten		= dwBytes;
	if (WSASend(
		m__sock,
		m__lpSend->m__WSABuf,
		dwBufferCount,
		&dwNumberOfBytesSent,
		m__lpSend->m__dwFlags,
		(LPWSAOVERLAPPED) &m__lpSend->m__WSAOverlappedPlus,
		NULL		// CompletionROUTINE 없이...
	) == SOCKET_ERROR && (WSALastErrorCode = WSAGetLastError()) != WSA_IO_PENDING) {
		INT	WSALastErrorCode = WSAGetLastError();
#ifdef _DEBUG
		_TCHAR	Message[512];
		DWORD	BufferLength0 = 512;	// NULL문자를 포함한 Message버퍼의 크기(문자 단위)
		(VOID) m__lpTCPBase->mf__GetWSAErrorStringFromCode(Message, &BufferLength0, WSALastErrorCode);
		m__lpTCPBase->mf__fprintf(m__lpTCPBase->m__CONERR,
			TEXT("[%u.%u] gmPseudoTCP::mf__IOCPSend(): ")
			TEXT("WSASend() failed!: ")
			TEXT("WSAGetLastError() = %d(%s)\n")
			,
			m__globalIndex,
			m__localIndex,
			WSALastErrorCode,
			Message
		);
#endif	// _DEBUG
		m__lpTCPBase->mf__SetWSALastErrorCode(WSALastErrorCode);
		return -1;
	}
	// 2003/10/16
	// 비록 WSALastErrorCode != WAS_IO_PENDING 일지라도
	// 이 값을 설정되어야 한다.
	// 왜냐하면 이 값이 설정되어있지 않으면 또 다른
	// WSASend를 호출할 것이고, 이 때에는 현재 보내는 버퍼의
	// 값들(m__head, m__tail, m__length, ...)을 근거로 이루어 질텐데
	// 이 값들은 GetQueuedCompletionStatus를 통해서 갱신되기 때문에
	// 정확한 값들이 될 수 없기 때문이다.
	m__bIOCPSendInProgress = TRUE;	// 얘 때문에 내가 얼마나 애를 태웠는지...

	return WSALastErrorCode == WSA_IO_PENDING ? 1 : 0;
}
// 리턴값
//	-1	WSASend에서 문제 발생(자세한 것은 m__WSALastError를 보라.)
//	0	WSASend에서 모든 데이터를 보냈다.
//	1	WSASend에서 일부 데이터를 보냈다.
//	2	보낼 테이터가 없다.
INT
gmPseudoTCP::mf__IOCPSendWithBlocking(
	VOID
)
{
	DWORD	dwBytes;
	DWORD	dwBufferCount = mf__SetSendBuf(&dwBytes);
	if (dwBufferCount == 0) {
		return 2;
	}

	DWORD	dwNumberOfBytesSent;
	if (WSASend(
		m__sock,
		m__lpSend->m__WSABuf,
		dwBufferCount,
		&dwNumberOfBytesSent,
		0,
		NULL,
		NULL
	) == SOCKET_ERROR) {
		m__lpTCPBase->mf__SetWSALastErrorCode(WSAGetLastError());
		return -1;
	}
	gmQueue	*lpgmQueue = m__lpSend->m__lpQueue;
	lpgmQueue->m__length -= dwNumberOfBytesSent;
	lpgmQueue->m__head = (lpgmQueue->m__head + dwNumberOfBytesSent) & lpgmQueue->m__mask;

	return dwNumberOfBytesSent == dwBytes ? 0 : 1;
}
////////////////////////////////////////////////////////////////////////////////

INT
gmPseudoTCP::mf__flush(
	VOID
)
{
	m__lpTCPBase->mf__lock();
	INT	iReturnVal = 0;

	if (
		!(m__gmPseudoTCPStatus & gmPseudoTCPStatusEstablished)
			||
		m__bIOCPSendInProgress == TRUE
	) {
		m__lpTCPBase->mf__unlock();
		return 0;
	}

	// 해당하는 클라이언트의 "보내는" 큐에 있는 내용을 내보낸다.
	if (mf__IOCPSend() < 0) {
		// 해당하는 클라이언트와의 연결을 끊을 수 밖에...
		m__lpTCPBase->mf__fprintf(m__lpTCPBase->m__CONERR,
			TEXT("gmPseudoTCP::mf__flush: ")
			TEXT("gmPseudoTCP::mf__IOCPSend() failed!\n")
		);
		iReturnVal = -1;
	}

	// 2005/3/25 (EST) - ghkim
	//	받는 큐에 있는 메시지가 있음을 알린다.
	if (!mf__bNotifyMessage(TRUE)) {
		iReturnVal = -1;
	}

	m__lpTCPBase->mf__unlock();
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
// >> 리턴값 <<
//
// 큐에 메세지를 넣은 경우
//	TCPPOSTMSG__OK
// 큐에 메세지를 넣지 못한 경우
//	TCPPOSTMSG__ERROR
//		어느 정도 큐가 차서 큐의 내용을 보내려 했으나, 큐의 내용을 보내는 데에 실패
//	TCPPOSTMSG__SEND_OK_IMMEDIATELY_BUT
//		어느 정도 큐가 차서, 큐의 내용을 "곧 바로" 보내는 데에는 성공했으나 나머지 큐에 공간이 없다.
//	TCPPOSTMSG__SEND_OK_BUT
//		어느 정도 큐가 차서, 큐의 내용을 보내는 데에는 성공했으나 나머지 큐에 공간이 없다.
//	TCPPOSTMSG__WSASEND_IN_PROGRESS_NOT_ENOUGH
//		어느 정도 큐가 차서 큐의 내용을 보내려 했으나, 이전의 WSASend가 진행중이고 나머지 큐에도 공간이 없다.
// 기타
//	TCPPOSTMSG__NOT_ESTABLISHED
//		해당하는 클라이언트와의 연결이 이미 끊어졌다.
////////////////////////////////////////////////////////////////////////////////
INT
gmPseudoTCP::mf__PostMsg(
	PMTCPMSG	pMTCPMsg
)
{
	INT	iReturnVal;

	if (!(m__gmPseudoTCPStatus & gmPseudoTCP::gmPseudoTCPStatusEstablished)) {
		// 이미 해당하는 클라이언트와의 접속을 끊었다.
#ifdef __gmIOCP_DEBUG_XXX__
		m__lpTCPBase->mf__fprintf(m__lpTCPBase->m__CONERR,
			TEXT("gmPseudoTCP::mf__SendMsg(): ")
			TEXT("[ #%d ] 번째 상대와의 연결이 이미 끊어졌다.\n")
			,
			m__localIndex
		);
#endif	// __gmIOCP_DEBUG_XXX__
		return TCPPOSTMSG__NOT_ESTABLISHED;
	}

	gmQueue*	lpgmQueue = m__lpSend->m__lpQueue;

	UINT	dwSize = lpgmQueue->m__length + (4 + pMTCPMsg->Header.cbData);

	// 큐가 어느 정도 찼는 지 알아본다.
	if (dwSize > lpgmQueue->m__size / 2) {	// 일종의 "더블 버퍼링"
		// 어느 정도 큐가 찼기 때문에 큐를 비운다.

		if (m__bIOCPSendInProgress == FALSE) {
			switch (mf__IOCPSend()) {	// -1, 0, 1, 2
			case 0:
				iReturnVal = TCPPOSTMSG__SEND_OK_IMMEDIATELY_BUT;
				break;
			case 1:
				iReturnVal = TCPPOSTMSG__SEND_OK_BUT;
				break;
			case 2:		// 보낼 데이터가 없다.
/*
 * Jul 14, 2003 - ghkim
 * m__size가 즉 SendSize가 2 * MAX_TCPMSGSIZE로 잡아준 경우에 발생한다.
 * 이유는 dwSize를 계산할 때에 4가 더해지기 때문에.
 * 예를 들어
 *	m__size = 2048, m__length = 0
 *	MTCP_MSG.hByes = 1023
 * 의 경우에 dwSize > lpgmQueue->m__size / 2가 된다.
 * 그러나, 보낼 데이터는 없다.(m__length = 0)
 *
 * 해결: 곧 바로 데이터를 보낸 것처럼 해서 다음 단계로 넘어가면 된다.
 */
				iReturnVal = TCPPOSTMSG__SEND_OK_IMMEDIATELY_BUT;
				break;
			default:	// -1
				iReturnVal = TCPPOSTMSG__ERROR;
				break;
			}
		} else {
			// WSASend가 진행중이고 큐에 여유가 없다.
			iReturnVal = TCPPOSTMSG__WSASEND_IN_PROGRESS_NOT_ENOUGH;
		}
		// mf__IOCPSend가 실패했거나(< 0), 큐에 여유공간이 없다면
		// 이 함수는 리턴한다.
		if (iReturnVal < 0 || dwSize > lpgmQueue->m__size) {
			return iReturnVal;
		}
	}

	(VOID) lpgmQueue->mf__Push(
#ifdef __gmCRYPT__	// 2.2
		m__lpTCPBase->m__hEncryptKey,
#endif	// __gmCRYPT__	// 2.2
		pMTCPMsg
	);
#ifdef __gmCRYPT__
#if 0	// 2.1
	if (!mf__EncryptQueue(lpgmQueue, sizeof(MTCPHEADER) + pMTCPMsg->Header.cbData)) {
		return TCPSENDMSG__ENCRYPT_ERROR;
	}
#endif	// 2.1
#endif	// __gmCRYPT__
	// 이미 앞에서 충분한 공간이 있음을 확인했으니,
	// TCPBUF__NOT_ENOUGH 가 리턴값이 될 수 없다.

	return TCPPOSTMSG__OK;
}

// 큐에 메세지를 넣은 경우
//	TCPPOSTMSG__OK
INT
gmPseudoTCP::mf__PostMessage(
	PMTCPMSG	pMTCPMsg
)
{
	m__lpTCPBase->mf__lock();
	INT	iReturnVal = mf__PostMsg(pMTCPMsg);
	m__lpTCPBase->mf__unlock();
	return iReturnVal;
}


////////////////////////////////////////////////////////////////////////////////
// 2005/4/21 (EST) - ghkim
//	TCPSENDMSG__WSASEND_IN_PROGRESS의 경우에는 큐에 넣지 않도록 했다.
//	이 함수를 호출한 응용프로그램에서 이 경우를 처리하도록 하기 위해서이다.
//	예를 들어, 주어진 시간동안 몇 번 이 함수를 호출하기를 시도해서
//	그래도 계속 이 경우가 나오면 그 때는 연결을 끊는다.
//	SendMessage함수를 보라.
// >> 리턴값 <<
//
// 큐에 넣지도 못한 경우
//	TCPSENDMSG__NOT_ENOUGH
//		큐에 충분한 공간이 없다.
//	TCPSENDMSG__WSASEND_IN_PROGRESS
//		이전의 WSASend가 진행중이다.
// 일단, 큐에 쌓아두는 데는 성공한 경우
//	TCPSENDMSG__OK_IMMEDIATELY
//		WSASend에서 곧 바로 데이터를 보냈다.
//	TCPSENDMSG__OK
//		WSASend에서 데이터를 보내는 중이다.
//	TCPSENDMSG__ERROR
//		WSASend에서 문제 발생
// 기타
//	TCPSENDMSG__NOT_ESTABLISHED
//		해당하는 클라이언트와의 연결이 이미 끊어졌다.
////////////////////////////////////////////////////////////////////////////////
INT
gmPseudoTCP::mf__SendMsg(
	PMTCPMSG	pMTCPMsg
)
{
	INT	iReturnVal;

	if (!(m__gmPseudoTCPStatus & gmPseudoTCP::gmPseudoTCPStatusEstablished)) {
		// 상대와의 연결이 이미 끊어졌다.
#ifdef __gmIOCP_DEBUG_XXX__
		m__lpTCPBase->mf__fprintf(m__lpTCPBase->m__CONERR,
			TEXT("gmPseudoTCP::mf__SendMsg(): ")
			TEXT("[ #%d ] 번째 상대와의 연결이 이미 끊어졌다.\n")
			,
			m__localIndex
		);
#endif	// __gmIOCP_DEBUG_XXX__
		return TCPSENDMSG__NOT_ESTABLISHED;
	}

	if (m__bIOCPSendInProgress == TRUE) {
		// 이전에 호출한 WSASend가 진행 중이다.
		return TCPSENDMSG__WSASEND_IN_PROGRESS;
	}

	gmQueue*	lpgmQueue = m__lpSend->m__lpQueue;
	if (lpgmQueue->mf__Push(
#ifdef __gmCRYPT__	// 2.2
		m__lpTCPBase->m__hEncryptKey,
#endif	// __gmCRYPT__	// 2.2
		pMTCPMsg
	) != TCPBUF__OK) {
		// TCPBUF__NOT_ENOUGH
		// 큐에 이 메시지를 저장할 공간이 없다.
		return TCPSENDMSG__NOT_ENOUGH;
	}
#ifdef __gmCRYPT__
#if 0	// 2.1
	if (!mf__EncryptQueue(lpgmQueue, sizeof(MTCPHEADER) + pMTCPMsg->Header.cbData)) {
		return TCPSENDMSG__ENCRYPT_ERROR;
	}
#endif	// 2.1
#endif	// __gmCRYPT__

	// 리턴값
	//	-1	WSASend에서 문제 발생(자세한 것은 m__WSALastError를 보라.)
	//	0	WSASend에서 데이터를 곧 바로 보냈다.
	//	1	WSASend에서 데이터를 보내는 중이다.
	//	2	보낼 데이터가 없다.
	switch (mf__IOCPSend()) {	// -1, 0, 1, 2(x)
	case 0:
		iReturnVal = TCPSENDMSG__OK_IMMEDIATELY;
		break;
	case 1:
		iReturnVal = TCPSENDMSG__OK;
		break;
/*
	방금 데이터를 넣었는 데
	보낼 데이터가 없다구....
	이럴 수는 없다.
	case 2:
		// unreachable!
		break;
 */
	default:	// -1
		iReturnVal = TCPSENDMSG__ERROR;
		break;
	}

	return iReturnVal;
}

// 큐에 메세지를 넣고, 그것을 보내는 데에 성공한 경우
//	TCPSENDMSG__OK_IMMEDIATELY
//	TCPSENDMSG__OK
INT
gmPseudoTCP::mf__SendMessage(
	PMTCPMSG	pMTCPMsg
)
{
	m__lpTCPBase->mf__lock();
	INT	iReturnVal = mf__SendMsg(pMTCPMsg);
	m__lpTCPBase->mf__unlock();
	return iReturnVal;
}

BOOL
gmPseudoTCP::mf__bIOCPSendInProgress(
	VOID
)
{
	m__lpTCPBase->mf__lock();
	BOOL	bIOCPSendInProgress = m__bIOCPSendInProgress;
	m__lpTCPBase->mf__unlock();
	return bIOCPSendInProgress;
}
INT
gmPseudoTCP::mf__SendMessageEx(
	PMTCPMSG	pMTCPMsg,
	DWORD	dwMilliSeconds
)
{
	if (!(mf__GetPseudoTCPStatus() & gmPseudoTCPStatusEstablished)) {
		return TCPSENDMSGEX__NOT_ESTABLISHED;
	}

	DWORD	dwMS = 0;
	while (mf__bIOCPSendInProgress() == TRUE) {
		if (dwMS > dwMilliSeconds) {
			return TCPSENDMSGEX__IN_PROGRESS_TIMEOUT;
		}
		Sleep(100);
		dwMS += 100;
	}

	INT	iReturnVal;
	m__lpTCPBase->mf__lock();

	gmQueue	*lpgmQueue = m__lpSend->m__lpQueue;
	if (lpgmQueue->mf__Push(
#ifdef __gmCRYPT__	// 2.2
		m__lpTCPBase->m__hEncryptKey,
#endif	// __gmCRYPT__	// 2.2
		pMTCPMsg
	) != TCPBUF__OK) {
		// TCPBUF__NOT_ENOUGH
		// 큐에 이 메시지를 저장할 공간이 없다.
		m__lpTCPBase->mf__unlock();
		return TCPSENDMSGEX__NOT_ENOUGH;
	}
#ifdef __gmCRYPT__
#if 0	// 2.1
	if (!mf__EncryptQueue(lpgmQueue, sizeof(MTCPHEADER) + pMTCPMsg->Header.cbData)) {
		m__lpTCPBase->mf__unlock();
		return TCPSENDMSG__ENCRYPT_ERROR;
	}
#endif	// 2.1
#endif	// __gmCRYPT__

	// 리턴값
	//	-1	WSASend에서 문제 발생(자세한 것은 m__WSALastError를 보라.)
	//	0	WSASend에서 모든 데이터를 보냈다.
	//	1	WSASend에서 일부 데이터를 보냈다.
	//	2	보낼 테이터가 없다.
	switch (mf__IOCPSendWithBlocking()) {	// -1, 0, 1, 2(x)
	case 0:
		iReturnVal = TCPSENDMSGEX__OK;
		break;
	case 1:
		iReturnVal = TCPSENDMSGEX__OK_PARTIAL;
		break;
/*
	방금 데이터를 넣었는 데
	보낼 데이터가 없다구....
	이럴 수는 없다.
	case 2:
		// unreachable!
		break;
 */
	default:	// -1
		iReturnVal = TCPSENDMSGEX__ERROR;
		break;
	}

	m__lpTCPBase->mf__unlock();
	return iReturnVal;
}

////////////////////////////////////////////////////////////////////////////////
// gmPseudoTCP오브젝트 상태와 관련된 함수들의 정의
gmPseudoTCP::gmPseudoTCPSTATUS
gmPseudoTCP::mf__GetPseudoTCPStatus(
	VOID
)
{
	m__lpTCPBase->mf__lock();
	gmPseudoTCPSTATUS	mygmPseudoTCPSTATUS = m__gmPseudoTCPStatus;
	m__lpTCPBase->mf__unlock();
	return mygmPseudoTCPSTATUS;
}

VOID
gmPseudoTCP::mf__SetPseudoTCPStatus(
	gmPseudoTCPSTATUS	gmPseudoTCPStatus
)
{
	m__lpTCPBase->mf__lock();
	m__gmPseudoTCPStatus = gmPseudoTCPStatus;
	m__lpTCPBase->mf__unlock();
	return;
}

// 2005/4/4 (EST) - ghkim
//	처음 연결되어서 상태를 ESTABLISHED로 바꾸면 TRUE를,
//	그렇지 않으면 FALSE를 리턴한다.
BOOL
gmPseudoTCP::mf__SetPseudoTCPStatusEstablished(
	VOID
)
{
	BOOL	bReturnVal;
	m__lpTCPBase->mf__lock();
	if (m__gmPseudoTCPStatus & gmPseudoTCPStatusAccepted) {
		bReturnVal = TRUE;
		m__gmPseudoTCPStatus = gmPseudoTCPStatusEstablished;
	} else {
		bReturnVal = FALSE;
	}
	m__lpTCPBase->mf__unlock();
	return bReturnVal;
}
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// 2003/10/10
// 상대방과의 연결을 끊는 데에 사용되는 멤버함수들이 정의
INT
gmPseudoTCP::mf__disconnect(
	BOOL	bBlocking
)
{
	m__lpTCPBase->mf__lock();

	if (
		// 연결이 되어 있지 않은 경우
		m__gmPseudoTCPStatus & gmPseudoTCPSTATUS__NOT_CONNECTED
			||
		// mf__closeRecv가 호출된 경우
		m__gmPseudoTCPStatus & gmPseudoTCPStatusCloseWait
	) {
		m__lpTCPBase->mf__unlock();
		return 0;
	}

	// WSASend가 있다면 이를 중지시킨다.
	// 그렇지만 WSARecv는 취소되지 않는다.
	// 왜냐하면 mf__disconnect는 Manager쓰레드에서 호출하고
	// WSARecv는 Worker쓰레드에서 호출하는데,
	// CancelIo는 다른 쓰레드에서 설정한 작업에는
	// 영향을 주지 않는다고 되어있기 때문에.
	//
	//	All pending I/O operations issued by the calling thread
	//	for the file handle were successfully canceled.
	//
	if (
		m__bIOCPSendInProgress == TRUE
			&&
		!CancelIo((HANDLE) m__sock)
	) {
		m__lpTCPBase->mf__SystemErrorMessage(
			GetLastError(),
			TEXT("[%u.%u] gmPseudoTCP::mf__disconnect: m__bIOCPSendInProress && CancelIo")
			,
			m__globalIndex,
			m__localIndex
		);
		// 어떻게 하나...
		// 2003/10/30
		// 메시지만 출력한다.
	}

	// 더 이상 해당 소켓을 사용하여 데이터를 보내지 않는다.
	if (shutdown(m__sock, SD_SEND) == SOCKET_ERROR) {
		m__lpTCPBase->mf__SystemErrorMessage(
			WSAGetLastError(),
			TEXT("[%u.%u] gmPseudoTCP::mf__disconnect: shutdown(SD_SEND)")
			,
			m__globalIndex,
			m__localIndex
		);
		// 어떻게 하나...
		// 메시지만 출력한다.
	}

	m__gmPseudoTCPStatus = gmPseudoTCPStatusDisconnectWait;

	m__lpTCPBase->mf__unlock();

	INT	iReturnVal = 0;
	// Worker쓰레드에게 이 gmPseudoTCP에 해당하는 연결을 끊도록 한다.
	if (!PostQueuedCompletionStatus(
		((gmTCPServer *) m__lpTCPBase)->mf__GetIOCPHandle(),
		bBlocking ? gmSUBCOMMAND_BLOCK : gmSUBCOMMAND_NONBLOCK,
						// DWORD	dwNumberOfBytesTransferred
		(ULONG_PTR) this,		// ULONG_PTR	dwCompletionKey
		(LPOVERLAPPED) gmCOMMAND_CHANNEL	// LPOVERLAPPED	lpOverlapped
	)) {
		// 이 연결을 끊을 방법이 없단 말인가...
		// 2003/10/30
		m__lpTCPBase->mf__fprintf(m__lpTCPBase->m__CONERR,
			TEXT("ERROR: ")
			TEXT("gmPseudoTCP::mf__disconnect: ")
			TEXT("PostQueuedCompletionStatus failed\n")
		);
		iReturnVal = -1;
	} else {
		if (
			bBlocking
				&&
			WaitForSingleObject(
				m__lpTCPBase->m__hEventWorkerToManager,
				gmDISCON_BLOCK_TIMEOUT
			) != WAIT_OBJECT_0
		) {
			// Worker쓰레드로부터 아무런 신호를 받지 못했거나 시간초과
			// 2003/10/30
			m__lpTCPBase->mf__fprintf(m__lpTCPBase->m__CONERR,
				TEXT("ERROR: ")
				TEXT("gmPseudoTCP::mf__disconnect: ")
				TEXT("blocking: YES, but WaitForSingleObject failed\n")
			);
			iReturnVal = -2;
		}
	}

	return iReturnVal;
}

INT
gmPseudoTCP::mf__closeRecv(
	VOID
)
{
	m__lpTCPBase->mf__lock();

	if (
		// 연결이 되어 있지 않은 경우
		// (
		//	Q. 어찌 이럴 수가?
		//	A. Manager가 호출한 mf__disconnect받아
		//	Worker가 mf__close를 호출해서 연결을 끊은 경우
		// )
		m__gmPseudoTCPStatus & gmPseudoTCPSTATUS__NOT_CONNECTED
			||
		// mf__disconnect가 호출된 경우
		// (
		//	이 경우는 mf__close에서 처리하도록 한다.
		// )
		m__gmPseudoTCPStatus & gmPseudoTCPStatusDisconnectWait
			||
		// mf__closeRecv가 호출된 경우
		// (
		//	두 번 호출되는 것을 방지하기 위하여
		// )
		m__gmPseudoTCPStatus == gmPseudoTCPStatusCloseWait
	) {
		m__lpTCPBase->mf__unlock();
		return 0;
	}

	if (!CancelIo((HANDLE) m__sock)) {
		m__lpTCPBase->mf__SystemErrorMessage(
			GetLastError(),
			TEXT("gmPseudoTCP::mf__closeRecv: CancelIo")
		);
		// 2003/10/30
		// 메시지만 출력한다.
	} 

	// 더 이상 해당 소켓을 이용하여 데이터를 받지 않는다.
	if(shutdown(m__sock, SD_RECEIVE)) {
		m__lpTCPBase->mf__SystemErrorMessage(
			WSAGetLastError(),
			TEXT("gmPseudoTCP::mf__closeRecv: shutdown(SD_RECEIVE)")
		);
		// 2003/10/30
		// 메시지만 출력한다.
	}

	m__gmPseudoTCPStatus = gmPseudoTCPStatusCloseWait;

	m__lpTCPBase->mf__unlock();


	INT	iReturnVal = 0;
#ifdef __gmUSE_PUBLIC_QUEUE__
	// 1. "받는 큐"에 이전의 데이터가 없는 지 확인하고,
	// 있다면 먼저 이를 "공용큐"에 넣어야 한다.
	// 나중에...

	// 2. 해당하는 클라이언트의 "보내는 쪽"을 닫으라는
	// TCP메세지를 "공용큐"에 넣는다.
	UINT	ui = gmTCPMSG_CLOSE_SEND;
	TCPPBUF	myTCPPBuf;
	myTCPPBuf.gmhTCP	= this;
	myTCPPBuf.TCPBuf.Header.hType	= gmMAGIC_TYPE_ID;
	myTCPPBuf.TCPBuf.Header.cbData	= 4;
	myTCPPBuf.TCPBuf.pbData	= (PCHAR) &ui;
	if ((m__lpTCPBase->mf__GetPublicQueue())->mf__PushP(&myTCPPBuf) != TCPBUF__OK) {
		// 이 연결을 끊을 방법이 없단 말인가...
		iReturnVal = -1;
		m__lpTCPBase->mf__fprintf(m__lpTCPBase->m__CONERR,
			TEXT("SEVERE: ")
			TEXT("gmPseudoTCP::mf__closeRecv: gmQueue::mf__PushP failed\n")
		);
		__SUICIDE__;	// SEVERE
	} else {
		// "공용큐"에 TCP메세지가 있음을 알린다.
		(void) m__lpTCPBase->mf__notify();
	}
#else	// __gmUSE_PUBLIC_QUEUE__
	mf__SetOOBFlag(gmPseudoTCPOOBStatusDisconnect);
#ifdef __gmMESSAGE_IOCP__
	if (!PostQueuedCompletionStatus(
		m__lpTCPBase->m__hMessageIOCP,
		(0x8000 << 16) | (0x0001),
		(ULONG_PTR) this,
		(LPOVERLAPPED) &m__dwUID
	)) {
		int	i = *((int *) 0);
	}
#else	// __gmMESSAGE_IOCP__
	if (!m__lpTCPBase->mf__notify()) {
		int	i = *((int *) 0);
	}
#endif	// !__gmMESSAGE_IOCP__
#endif	// !__gmUSE_PUBLIC_QUEUE__

	return iReturnVal;
}

INT
gmPseudoTCP::mf__closeEnd(
	BOOL	bBlocking
)
{
	m__lpTCPBase->mf__lock();

	if (!(m__gmPseudoTCPStatus & gmPseudoTCPStatusDisconnectWait)) {
		// Q. 이럴수가 있나?
		m__lpTCPBase->mf__unlock();
		return 0;
	}

	// 진행중인 WSARecv를 취소한다.
	if (!CancelIo((HANDLE) m__sock)) {
		m__lpTCPBase->mf__SystemErrorMessage(
			GetLastError(),
			TEXT("gmPseudoTCP::mf__close: CancelIo")
		);
	}

	// 소켓을 닫는다.
	// 이미 앞에서 WSARecv작업을 취소했으므로 이 정보는
	// IOCP작업큐에 쌓이지 않는다.
	if(closesocket(m__sock)) {
		m__lpTCPBase->mf__SystemErrorMessage(
			WSAGetLastError(),
			TEXT("gmPseudoTCP::mf__close: closesocket")
		);
		// 2003/10/30
		// 메시지만 출력한다.
	}

	m__sock = INVALID_SOCKET;
	m__gmPseudoTCPStatus = gmPseudoTCPSTATUS__NOT_CONNECTED;
	--m__lpTCPBase->m__NConn;

	if (bBlocking)
		(VOID) SetEvent(m__lpTCPBase->m__hEventWorkerToManager);

	m__lpTCPBase->mf__unlock();
	return 0;
}

INT
gmPseudoTCP::mf__closeSend(
	VOID
)
{
	m__lpTCPBase->mf__lock();

	if (!(m__gmPseudoTCPStatus & gmPseudoTCPStatusCloseWait)) {
		// Q. 이럴수가 있나?
		m__lpTCPBase->mf__unlock();
		return 0;
	}

	if (
		m__bIOCPSendInProgress == TRUE
			&&
		!CancelIo((HANDLE) m__sock)
	) {
		m__lpTCPBase->mf__SystemErrorMessage(
			GetLastError(),
			TEXT("gmPseudoTCP::mf__closeSend: CancelIo")
		);
		// 2003/10/30
		// 메시지만 출력한다.
	}

	// 소켓을 닫는다.
	// mf__closeRecv에서 WSARecv작업을 취소했으므로 이 정보는
	// IOCP작업큐에 쌓이지 않는다.
	if(closesocket(m__sock)) {
		m__lpTCPBase->mf__SystemErrorMessage(
			WSAGetLastError(),
			TEXT("gmPseudoTCP::mf__closeSend: closesocket")
		);
		// 어떻게 하나...
		// 2003/10/30
		// 메시지만 출력한다.
	}

	m__sock = INVALID_SOCKET;
	m__gmPseudoTCPStatus = gmPseudoTCPSTATUS__NOT_CONNECTED;
	--m__lpTCPBase->m__NConn;

	m__lpTCPBase->mf__unlock();
	return 0;
}
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// 2003/10/17
// IOCP의 작업에 따라 Worker쓰레드에서 호출하는 멤버함수들의 정의
VOID
gmPseudoTCP::mf__IOOP_WR(
	DWORD	dwIOSize
)
{
	m__lpTCPBase->mf__lock();

	if (!m__lpSend) {
		m__lpTCPBase->mf__fprintf(m__lpTCPBase->m__CONERR,
			TEXT("gmPseudoTCP::mf__IOOP_WR: m__lpSend = 0\n")
		);
		m__lpTCPBase->mf__unlock();
		return;
	}

	// "보내는" 큐의 상태(m__length, m__head)를 수정한다.
	gmQueue	*lpgmQueue = m__lpSend->m__lpQueue;

	lpgmQueue->m__length -= dwIOSize;
	lpgmQueue->m__head = (lpgmQueue->m__head + dwIOSize) & lpgmQueue->m__mask;
	m__bIOCPSendInProgress = FALSE;

	m__lpTCPBase->mf__unlock();
	return;
}

#ifdef __gmUSE_PUBLIC_QUEUE__
/*
 * 목적:
 *	특정 클라이언트의 받는 큐로부터 TCP메세지를 읽어서 공용큐에 넣는다.
 * 리턴값:
 *	> 0:	읽어들인 메세지의 갯수
 *	0:	데이터가 들어왔지만 아직 메세지 단위는 아니다.
 *
 * TCPPUBLICQUEUE_WARN	특정 "받는 큐"의 데이터가 엉켰다. 우선 연결을 끊는다.
 * TCPPUBLICQUQUE_ERROR	공용큐가 가득찼다. 서버를 멈추어야 한다.(?)
 */
INT
gmPseudoTCP::mf__UpdateAllQueues(
	DWORD	dwBytes
)
{
	gmQueue	*lpPublicQueue = m__lpTCPBase->mf__GetPublicQueue();

	gmQueue*	lpgmQueue = m__lpRecv->m__lpQueue;
	lpgmQueue->m__length += dwBytes;
	lpgmQueue->m__tail = (lpgmQueue->m__tail + dwBytes) & lpgmQueue->m__mask;

	TCPPBUF		myTCPPBuf;
	LPTCPBUF	lpTCPBuf = &myTCPPBuf.TCPBuf;
	myTCPPBuf.TCPBuf.pbData	= m__lpTCPMsgBuf;

	INT	nMsg = 0;		// 읽어들인 메세지의 갯수
	do {
		lpTCPBuf->Header.cbData = m__lpTCPBase->mf__GetMaxTCPMsgSize();
		// lpTCPBuf->Header.cbData (IN/OUT)
		INT	iReturnVal = lpgmQueue->mf__Shift(lpTCPBuf);
		if (iReturnVal == TCPBUF__NOT_ENOUGH) {
			// 해당하는 클라이언트의 받는 큐가 엉켰다.
			// (의도적이던 아니면 버그이던)
			// 해당하는 클라이언트와의 연결을 끊는다.
			m__lpTCPBase->mf__fprintf(m__lpTCPBase->m__CONERR,
				TEXT("gmPseudoTCP::mf__UpdateAllQueues: 클라이언트(0x%08X: #%u)의 받는 큐가 엉켰다.\n")
				,
				this,
				m__localIndex
			);
			return TCPPUBLICQUEUE__WARN;
		}
		if (iReturnVal != TCPBUF__OK)
			break;

		// 공용큐에 적어도
		//	4 +	// m__localIndex
		//	2 +	// type
		//	2 +	// # of bytes
		//	# of bytes
		// 의 공간이 있어야 한다.
		if (lpPublicQueue->m__length + 8 + myTCPPBuf.TCPBuf.Header.cbData > lpPublicQueue->m__size) {
			m__lpTCPBase->mf__fprintf(m__lpTCPBase->m__CONERR,
				TEXT("gmPseudoTCP::mf__UpdateAllQueues: 공용큐가 가득찼다.\n")
			);
			return TCPPUBLICQUEUE__ERROR;
		}

		// 공용큐에 저장한다.
		myTCPPBuf.gmhTCP	= this;
		lpPublicQueue->mf__PushP(&myTCPPBuf);
		// 이미 앞에서 충분한 공간이 있음을 확인했으니,
		// TCPBUF__NOT_ENOUGH 가 리턴값이 될 수 없다.
#ifdef __gmIOCP_DEBUG_PUBLIC_QUEUE__
		m__lpTCPBase->mf__fprintf(m__lpTCPBase->m__CONERR,
			TEXT("-> Public Queue: %u (%.2f%%)\n")
			,
			lpPublicQueue->m__length,
			1. * lpPublicQueue->m__length / lpPublicQueue->m__size * 100
		);
#endif	// __gmIOCP_DEBUG_PUBLIC_QUEUE__
		++nMsg;
	} while (1);
	if (nMsg > 0) {	// 공용큐에 저장된 메세지가 있다면
		// 이 사실을 알린다.
		(void) m__lpTCPBase->mf__notify();

		m__dwMilliSeconds = GetTickCount();
	}
#ifdef __gmTCP_DEBUG__
	if (lpPublicQueue->m__length > m__lpTCPBase->mf__GetMaxPublicQueueLength())
		m__lpTCPBase->mf__SetMaxPublicQueueLength(lpPublicQueue->m__length);
#endif	// __gmTCP_DEBUG__

	return nMsg;
}
#endif	// __gmUSE_PUBLIC_QUEUE__

INT
gmPseudoTCP::mf__IOOP_RD(
	DWORD		dwIOSize
)
{
	INT	iReturnVal;
	m__lpTCPBase->mf__lock();

	if (!m__lpRecv) {
		m__lpTCPBase->mf__fprintf(m__lpTCPBase->m__CONERR,
			TEXT("gmPseudoTCP::mf__IOOP_RD: m__lpRecv = 0\n")
		);
		iReturnVal = 0;
		goto LAST;
	}

#ifdef __gmUSE_PUBLIC_QUEUE__
	switch (mf__UpdateAllQueues(dwIOSize)) {
	case TCPPUBLICQUEUE__WARN:
		// 특정 클라이언트의 "받는 큐"가 엉켰다.
		// 연결을 "강제로" 끊는다.
		m__lpTCPBase->mf__fprintf(m__lpTCPBase->m__CONERR,
			TEXT("gmPseudoTCP::mf__IOOP_RD: ")
			TEXT("gmPseudoTCP::mf__UpdateAllQueues() failed!: ")
			TEXT("특정 클라이언트(0x%08X: #%u.%u)의 서버와의 연결을 \"강제로\" 끊는다.\n")
			,
			this,
			m__globalIndex,
			m__localIndex
		);
		iReturnVal = -1;
		goto LAST;
		break;
	case TCPPUBLICQUEUE__ERROR:
		// 공용큐에 문제가 있다.
		m__lpTCPBase->mf__fprintf(m__lpTCPBase->m__CONERR,
			TEXT("SEVERE: ")
			TEXT("gmPseudoTCP::mf__IOOP_RD: ")
			TEXT("gmPseudoTCP::mf__UpdateAllQueues failed: ")
			TEXT("공용큐에 문제가 있다!\n")
		);
		__SUICIDE__;	// SEVERE
		iReturnVal = -1;
		goto LAST;
		break;
	default:	// >= 0
		// Ok!
		break;
	}
#else	// __gmUSE_PUBLIC_QUEUE__
	gmQueue*	lpgmQueue;
	lpgmQueue = m__lpRecv->m__lpQueue;
#ifdef __gmCRYPT__
#if 0	// 2.1
	if (!mf__DecryptQueue(lpgmQueue, dwIOSize)) {
		iReturnVal = -1;
		goto LAST;
	}
#endif	// 2.1
#endif	// __gmCRYPT__
	lpgmQueue->m__length += dwIOSize;
	lpgmQueue->m__tail = (lpgmQueue->m__tail + dwIOSize) & lpgmQueue->m__mask;

	m__MessageIncompleteLength += dwIOSize;
	if (!mf__bNotifyMessage()) {
		// 연결을 끊도록 한다.
		iReturnVal = -1;
		goto LAST;
	}
#endif	// !__gmUSE_PUBLIC_QUEUE__

	switch (mf__IOCPRecv()) {
	case MSGTCP_IOCPRECV_IMMEDIATELY_OK:
	case MSGTCP_IOCPRECV_PENDING_OK:
		break;
	case MSGTCP_IOCPRECV_BUFFER_FULL:
#ifdef _DEBUG
		m__lpTCPBase->mf__fprintf(m__lpTCPBase->m__CONERR,
			TEXT("gmPseudoTCP::mf__IOOP_RD: ")
			TEXT("gmPseudoTCP::mf__IOCPRecv: ")
			TEXT("특정 클라이언트(0x%08X: #%u.%u)로부터 메시지를 일시적으로 받지 않는다.\n")
			,
			this,
			m__globalIndex,
			m__localIndex
		);
#endif	// _DEBUG
#ifdef __gmUSE_PUBLIC_QUEUE__
#else	// __gmUSE_PUBLIC_QUEUE__
		mf__SetOOBFlag(gmPseudoTCPOOBStatusPaused);
#endif	// !__gmUSE_PUBLIC_QUEUE__
		break;
	default:	// MSGTCP_IOCPRECV_WSARECV_ERROR
		// 연결을 "강제로" 끊는다.
		m__lpTCPBase->mf__fprintf(m__lpTCPBase->m__CONERR,
			TEXT("gmPseudoTCP::mf__IOOP_RD: ")
			TEXT("gmPseudoTCP::mf__IOCPRecv: ")
			TEXT("특정 클라이언트(0x%08X: #%u.%u)와의 연결을 \"강제로\" 끊는다.\n")
			,
			this,
			m__globalIndex,
			m__localIndex
		);
		iReturnVal = -1;
		goto LAST;
	}
	iReturnVal = 0;

LAST:
	m__lpTCPBase->mf__unlock();
	return iReturnVal;
}
////////////////////////////////////////////////////////////////////////////////


#ifdef __gmUSE_PUBLIC_QUEUE__
#else	// __gmUSE_PUBLIC_QUEUE__
/*
 * INPUT
 *	LPTCPBUF
 *		OUT	LPTCPBUF.Header.hType
 *		IN/OUT	LPTCPBUF.Header.cbData
 *		IN	LPTCPBUF.pbData
 *
 * RETURN
 *	>= 0	Ok	= LPTCPBUF.Header.cbData	// # of bytes
 *			TCPBUF__OK
 *	< 0	Failure
 *			TCPBUF__NO_HEADER
 *			TCPBUF__PARTIAL_DATA
 *			TCPBUF__NOT_ENOUGH
 */
INT
gmPseudoTCP::mf__peek(
	PMTCPMSG	pMTCPMsg,
	PUINT		lpMsgLen
)
{
	INT	iReturnVal;
	m__lpTCPBase->mf__lock();

	if (!(m__gmPseudoTCPStatus & gmPseudoTCPStatusEstablished)) {
		iReturnVal = TCPBUF__NOT_ESTABLISHED;
		goto LAST;
	}

	// lpTCPBuf.Header.cbData [IN/OUT]
	iReturnVal = m__lpRecv->m__lpQueue->mf__ShiftEx(pMTCPMsg, lpMsgLen);
	if (
		!lpMsgLen &&			// 메시지를 큐에서 가져올 때,
		iReturnVal == TCPBUF__OK	// 메시지가 가져왔다.
	) {
		if (
			m__dwOOB & gmPseudoTCPOOBStatusPaused &&	// 큐가 가득차서 받을 수 없는 상태였다면
			mf__bPolicyPausedQueueContinue()		// "정책"에 따라 계속해서 메시지를 받아야 한다면
		) {
			// 적어도 하나의 메시지는 처리하여 큐를 비웠으므로
			// 다음 데이터를 받아들일 수 있다.
			switch (mf__IOCPRecvContinue()) {
			case MSGTCP_IOCPRECV_IMMEDIATELY_OK:
			case MSGTCP_IOCPRECV_PENDING_OK:
				// 이 함수의 리턴값을 받은 후에 플래그를 리셋시키지 말아야 한다.
				// 즉 WSARecv를 호출하는 것과 이 플래그를 리셋시키는 것은 동시에 이루어져야 한다.
				//	WSARecv를 호출하고 나중에 (Manager쓰레드에서) 플래그를 리셋시키는 경우를 생각하자.
				//	플래그를 리셋시키기전에 WSARecv의 결과를 받을 수 있다. (Worker쓰레드에서)
				//	그 때 WSARecv를 다시 호출하려고 한다.
				//	아직 Manager쓰레드에서 큐의 내용을 가져가지 않았으므로
				//	더 이상 큐의 공간은 없다. 따라서 Worker쓰레드는 이 플래그를 설정해서
				//	Manager쓰레드에서 이를 참고하도록 한다.
				//	그런데, 아직 Manager쓰레드는 이 함수를 호출한 후에 아직 플래그를 리셋하지 않았으므로
				//	이 때, 플래그를 리셋한다. 그러면 Worker쓰레드에서 설정한 플래그까지도 동시에 리셋이 되어서
				//	Worker쓰레드가 의도한 대로 동작하지 않는다.
				m__dwOOB &= ~gmPseudoTCPOOBStatusPaused;
				break;
			case MSGTCP_IOCPRECV_WSARECV_ERROR:
				// mf__IOCPRecv에서 WSARecv호출에 문제가 있다.
				// 이 연결을 더 이상 사용하면 안된다.
				iReturnVal = TCPBUF__OK_BUT;
				break;
			default:	// MSGTCP_IOCPRECV_BUFFER_FULL
				// "받는 큐"가 가득찬 경우이다.
				// 적어도 하나의 메시지는 처리했으므로 이럴 수는 없다.
				iReturnVal = TCPBUF__INTERNAL_ERROR;
				break;
			}
		}
	}
#ifdef __gmCRYPT__	// 2.2
	DWORD	cbDataLength;
	cbDataLength = pMTCPMsg->Header.cbData;
	if (!CryptDecrypt(m__lpTCPBase->m__hDecryptKey, NULL, FALSE, 0, pMTCPMsg->pbData, &cbDataLength)) {
		int	i = *((int *) 0);
	}
#endif	// __gmCRYPT__	// 2.2

LAST:
	m__lpTCPBase->mf__unlock();
	return iReturnVal;
}


// "받는 큐"가 중지된 상태였을 때, 그 상태를 해제할 지를 판단한다.
inline BOOL
gmPseudoTCP::mf__bPolicyPausedQueueContinue(
	void
)
{
//	return TRUE;	// 무조건 설정한다.

	// 큐에 쌓여있는 데이터의 크기가 전체 큐의 크기의 반보다 작을 때, 큐의 중지상태를 해제한다.
	// 데이터의 크기가 너무 많은 때, 중지상태를 풀면 곧 다시 중지상태가 되기 때문이다.
	return m__lpRecv->m__lpQueue->m__size > 2 * m__lpRecv->m__lpQueue->m__length ? TRUE : FALSE;
}


// 2005/3/25 (EST) - ghkim
BOOL
gmPseudoTCP::mf__bNotifyMessage(
	BOOL	bBlind
)
{
	BOOL	bReturnVal;
	m__lpTCPBase->mf__lock();

	DWORD	dwMessage;
	// m__MessageIncompleteHead부터 시작해서,
	// m__MessageIncompleteLength바이트에는
	// 몇 개의 메시지가 있는 지 알아본다.
	while ((dwMessage = mf__HowManyMessages(m__lpRecv->m__lpQueue, bBlind))) {
		// 2005/3/20 - ghkim
		// "받는" 큐에 적어도 하나의 완전한 메시지가 있을 때에야 비로서
		// 이벤트를 발생시킨다.
#ifdef __gmMESSAGE_IOCP__
		// 한 번에 알릴 수 있는 메시지의 갯수는 최대 XXX개이다.
		if (!PostQueuedCompletionStatus(
			m__lpTCPBase->m__hMessageIOCP,
			dwMessage,
			(ULONG_PTR) this,
			(LPOVERLAPPED) &m__dwUID
		)) {
			// 연결을 끊도록 한다.
			bReturnVal = FALSE;
			goto __LAST__;
		}
#else	// __gmMESSAGE_IOCP__
		if (!m__lpTCPBase->mf__notify()) {
			// 연결을 끊도록 한다.
			bReturnVal = FALSE;
			goto __LAST__;
		}
#endif	// !__gmMESSAGE_IOCP__
	}
	bReturnVal = TRUE;

__LAST__:
	m__lpTCPBase->mf__unlock();
	return bReturnVal;
}

INT
gmPseudoTCP::mf__Shift(
	UINT	msgLen
)
{
	INT	iReturnVal;
	m__lpTCPBase->mf__lock();

	if (!(m__gmPseudoTCPStatus & gmPseudoTCPStatusEstablished)) {
		iReturnVal = TCPBUF__NOT_ESTABLISHED;
		goto LAST;
	}

	iReturnVal = m__lpRecv->m__lpQueue->mf__ShiftEx(msgLen);
	if (iReturnVal != TCPBUF__OK) {
		goto LAST;
	}

	// "받는" 큐가 정지된 상태라면 이를 활성화한다.
	if (
		m__dwOOB & gmPseudoTCPOOBStatusPaused &&	// 큐가 가득차서 받을 수 없는 상태였다면
		mf__bPolicyPausedQueueContinue()		// "정책"에 따라 계속해서 메시지를 받아야 한다면
	) {
		// 적어도 하나의 메시지는 처리하여 큐를 비웠으므로
		// 다음 데이터를 받아들일 수 있다.
		switch (mf__IOCPRecvContinue()) {
		case MSGTCP_IOCPRECV_IMMEDIATELY_OK:
		case MSGTCP_IOCPRECV_PENDING_OK:
			// 이 함수의 리턴값을 받은 후에 플래그를 리셋시키지 말아야 한다.
			// 즉 WSARecv를 호출하는 것과 이 플래그를 리셋시키는 것은 동시에 이루어져야 한다.
			//	WSARecv를 호출하고 나중에 (Manager쓰레드에서) 플래그를 리셋시키는 경우를 생각하자.
			//	플래그를 리셋시키기전에 WSARecv의 결과를 받을 수 있다. (Worker쓰레드에서)
			//	그 때 WSARecv를 다시 호출하려고 한다.
			//	아직 Manager쓰레드에서 큐의 내용을 가져가지 않았으므로
			//	더 이상 큐의 공간은 없다. 따라서 Worker쓰레드는 이 플래그를 설정해서
			//	Manager쓰레드에서 이를 참고하도록 한다.
			//	그런데, 아직 Manager쓰레드는 이 함수를 호출한 후에 아직 플래그를 리셋하지 않았으므로
			//	이 때, 플래그를 리셋한다. 그러면 Worker쓰레드에서 설정한 플래그까지도 동시에 리셋이 되어서
			//	Worker쓰레드가 의도한 대로 동작하지 않는다.
			m__dwOOB &= ~gmPseudoTCPOOBStatusPaused;
			iReturnVal = TCPBUF__OK;
			break;
		case MSGTCP_IOCPRECV_WSARECV_ERROR:
			// mf__IOCPRecv에서 WSARecv호출에 문제가 있다.
			// 이 연결을 더 이상 사용하면 안된다.
			iReturnVal = TCPBUF__OK_BUT;
			break;
		default:	// MSGTCP_IOCPRECV_BUFFER_FULL
			// "받는 큐"가 가득찬 경우이다.
			// 적어도 하나의 메시지는 처리했으므로 이럴 수는 없다.
			iReturnVal = TCPBUF__INTERNAL_ERROR;
			break;
		}
	}

LAST:
	m__lpTCPBase->mf__unlock();
	return iReturnVal;
}

UINT
gmPseudoTCP::mf__GetRecvQueueSize(
	VOID
)
{
	m__lpTCPBase->mf__lock();
	UINT	uSize = m__lpRecv->m__lpQueue->m__length;
	m__lpTCPBase->mf__unlock();
	return uSize;
}

VOID
gmPseudoTCP::mf__SetOOBFlag(
	gmPseudoTCPOOBSTATUS	dwF
)
{
	m__lpTCPBase->mf__lock();

	m__dwOOB |= dwF;

	m__lpTCPBase->mf__unlock();
}

VOID
gmPseudoTCP::mf__ClearOOBFlag(
	gmPseudoTCPOOBSTATUS	dwF
)
{
	m__lpTCPBase->mf__lock();

	m__dwOOB &= ~dwF;

	m__lpTCPBase->mf__unlock();
}

gmPseudoTCP::MSGTCP_IOCPRECV_XXX
gmPseudoTCP::mf__IOCPRecvContinue(
	VOID
)
{
	MSGTCP_IOCPRECV_XXX	iReturnVal;
	m__lpTCPBase->mf__lock();

	if (!(m__dwOOB & gmPseudoTCPOOBStatusPaused)) {
		iReturnVal = MSGTCP_IOCPRECV_IMMEDIATELY_OK;
		goto LAST;
	}

	iReturnVal = mf__IOCPRecv();
	switch (iReturnVal) {
	case MSGTCP_IOCPRECV_IMMEDIATELY_OK:
	case MSGTCP_IOCPRECV_PENDING_OK:
		// 이 함수의 리턴값을 받은 후에 플래그를 리셋시키지 말아야 한다.
		// 즉 WSARecv를 호출하는 것과 이 플래그를 리셋시키는 것은 동시에 이루어져야 한다.
		//	WSARecv를 호출하고 나중에 (Manager쓰레드에서) 플래그를 리셋시키는 경우를 생각하자.
		//	플래그를 리셋시키기전에 WSARecv의 결과를 받을 수 있다. (Worker쓰레드에서)
		//	그 때 WSARecv를 다시 호출하려고 한다.
		//	아직 Manager쓰레드에서 큐의 내용을 가져가지 않았으므로
		//	더 이상 큐의 공간은 없다. 따라서 Worker쓰레드는 이 플래그를 설정해서
		//	Manager쓰레드에서 이를 참고하도록 한다.
		//	그런데, 아직 Manager쓰레드는 이 함수를 호출한 후에 아직 플래그를 리셋하지 않았으므로
		//	이 때, 플래그를 리셋한다. 그러면 Worker쓰레드에서 설정한 플래그까지도 동시에 리셋이 되어서
		//	Worker쓰레드가 의도한 대로 동작하지 않는다.
		m__dwOOB &= ~gmPseudoTCPOOBStatusPaused;
		break;
	case MSGTCP_IOCPRECV_WSARECV_ERROR:
		// mf__IOCPRecv에서 WSARecv호출에 문제가 있다.
		// 이 연결을 더 이상 사용하면 안된다.
		break;
	default:	// MSGTCP_IOCPRECV_BUFFER_FULL
		// "받는 큐"가 가득찬 경우이다.
		break;
	}

LAST:
	m__lpTCPBase->mf__unlock();
	return iReturnVal;
}


DWORD
gmPseudoTCP::mf__GetOOB(
	VOID
)
{
	m__lpTCPBase->mf__lock();

	DWORD	dwX = m__dwOOB;

	m__lpTCPBase->mf__unlock();
	return dwX;
}



DWORD
gmPseudoTCP::mf__HowManyMessages(
	gmQueue	*pRecvQueue,
	BOOL	bBlind
)
{
	UINT	mask = pRecvQueue->m__mask;
	PCHAR	pBuf = pRecvQueue->m__lpBuf;

	DWORD	dwMessage = 0;
	UINT	SavedMessageIncompleteHead	= m__MessageIncompleteHead;
	UINT	SavedMessageIncompleteLength	= m__MessageIncompleteLength;

	while (1) {
		if (
			!bBlind &&			// 적당한 조건이 맞으면 알린다.
			m__dwMsgUnit &&			// 매 m__dwMsgUnit단위로 메시지를 처리할 때,
			dwMessage == m__dwMsgUnit	// 원하는 만큼의 메시지가 들어왔다면
		) {
			// 모을만큼 모았다.
			break;
		}

		if (m__MessageIncompleteLength < sizeof(MTCPHEADER)) {
			// 헤더정보가 없다.
			break;
		}

		// 헤더정보를 읽는다.
		// 헤더정보를 읽는다.
		MTCPHEADER	Header;
		PBYTE	pByte = (PBYTE) &Header;
		for (UINT i = 0; i < sizeof(MTCPHEADER); ++i) {
			*pByte++ = pBuf[(m__MessageIncompleteHead + i) & mask];
		}

		UINT	MessageLength = sizeof(MTCPHEADER) + Header.cbData;
		if (MessageLength > m__MessageIncompleteLength) {
			// 모든 데이터가 들어오지 않았다.
			break;
		}

		// 완전한 메시지가 들어왔다.
		++dwMessage;

		// 다음 메시지를 조사하기 위하여 length, head값을 변경한다.
		m__MessageIncompleteHead = (m__MessageIncompleteHead + MessageLength) & mask;
		m__MessageIncompleteLength -= MessageLength;

		// 다음 메시지를 조사한다.
	}
	if (
		!bBlind &&			// 적당한 조건이 맞으면 알린다.
		dwMessage &&			// 처리할 메시지가 있는 데...
		m__dwMsgUnit &&			// 매 m__dwMsgUnit단위로 메시지를 처리할 때,
		dwMessage != m__dwMsgUnit &&	// 원하는 만큼의 메시지가 들어오지 않았으므로 다음 차례를 기다린다.

						// 하지만, "받는" 큐가 정지된 상태라면 기다리지 않는다.
		!(m__dwOOB & gmPseudoTCPOOBStatusPaused)
	) {
		// 다음에 다시 시작할 수 있도록 원래대로 되돌린다.
		dwMessage = 0;
		m__MessageIncompleteHead	= SavedMessageIncompleteHead;
		m__MessageIncompleteLength	= SavedMessageIncompleteLength;
	}

	return dwMessage;
}
#endif	// !__gmUSE_PUBLIC_QUEUE__

#ifdef __gmTLS__
/*
 * InitializeSecurityContext를 사용한다.
 * 클라리언트가 서버에 보낼 Token을 만든다.
 * 필요하면 서버로부터 받은 Token을 이용한다.

	SECURITY_STATUS SEC_Entry
	InitializeSecurityContext(
		PCredHandle	phCredential,	// [IN]		! Handle to the credentials returned by AcquireCredentialsHandle used to build the security context. Using InitializeSecurityContext requires at least OUTBOUND credentials.
		PCtxtHandle	phContext,	// [IN/OUT]	! Pointer to a CtxtHandle structure. On the first call to InitializeSecurityContext, this pointer is NULL. On the second call, this parameter is a pointer to the handle to the partially formed context returned in the phNewContext parameter by the first call.	When using the Schannel SSP, on the first call to InitializeSecurityContext, specify NULL. On future calls, specify the token received in the phNewContext parameter after the first call to this function.
		SEC_CHAR	*pszTargetName,	// [IN]		! Pointer to a NULL-terminated string that indicates the target of the context.		NULL-terminated string that uniquely identifies the target server. Schannel uses this value to locate the session in the session cache when reestablishing a connection.
		ULONG		fContextReq,	// [IN]		! Bit flags that indicate the requirements of the context. Not all packages can support all requirements. Flags used for this parameter are prefixed with ISC_REQ_; for example: ISC_REQ_DELEGATE.
		ULONG		Reserved1,	//		! Reserved value; must be zero.
		ULONG		TargetDataRep,	// [IN]		! Not used with Digest or Schannel. Specify zero.
		PSecBufferDesc	pInput,		// [IN/OUT]	! Pointer to a SecBufferDesc structure that contains pointers to the buffers supplied as input to the package. The pointer must be NULL on the first call to the function. On subsequent calls to the function, it is a pointer to a buffer allocated with enough memory to hold the token returned by the remote peer.		When using the Schannel SSP, on the first call to the function, specify NULL. On additional calls, there must be two buffers. The first is of type SECBUFFER_TOKEN and contains the token received from the server. The second buffer receives data that is not processed by Schannel, if any.
		ULONG		Reserved2,	//		! Reserved value; must be zero.
		PCtxtHandle	phNewContext,	// [IN/OUT]	! Pointer to a CtxtHandle structure. On the first call to InitializeSecurityContext, this pointer receives the new context handle. On the second call, phNewContext can be the same as the handle specified in the phContext parameter.		When using the Schannel SSP, on calls after the first call, pass the handle returned here as phContext parameter and specify NULL for phNewContext.
		PSecBufferDesc	pOutput,	// [OUT]	! Pointer to a SecBufferDesc structure that contains pointers to the SecBuffer structure that receives the output data. If a buffer was typed as SEC_READWRITE in the input, it will be there on output. The system will allocate a buffer for the security token if requested (through ISC_REQ_ALLOCATE_MEMORY) and fill in the address in the buffer descriptor for the security token.	When using the Schannel SSP, if the ISC_REQ_ALLOCATE_MEMORY flag is specified, the Schannel SSP will allocate memory for the buffer and put the appropriate information in the SecBufferDesc.
		PULONG		pfContextAttr,	// [OUT]	! Pointer to a variable to receive a set of bit flags that indicate the attributes of the established context.	Flags used for this parameter are prefixed with ISC_RET, such as ISC_RET_DELEGATE.	Do not check for security-related attributes until the final function call returns successfully. Attribute flags not related to security, such as the ASC_RET_ALLOCATED_MEMORY flag, can be checked before the final return.	Particular context attributes can change during a negotiation dialog with a remote peer.
		PTimeStamp	ptsExpiry	// [OUT]	! Pointer to a TimeStamp structure that receives the expiration time of the context. It is recommended that the security package always return this value in local time. This parameter is optional and NULL should be passed for short-lived clients.	
	);
 */
SECURITY_STATUS
gmPseudoTCP::mf__InitializeSecurityContext(
	MSGBLOB&	InputMessage,
	MSGBLOB&	OutputMessage
)
{
	BOOL	bFirstCall = m__bValidContext ? FALSE : TRUE;
	if (bFirstCall && m__bValidContext) {
		// 처음 호출했는 데, 이미 m__phContext값이 있다.
		return (SECURITY_STATUS) -1;
	}
	if (!bFirstCall && !m__bValidContext) {
		// 처움 호출이 아닌데, m__phContext값이 설정되어 있지 않다.
		return (SECURITY_STATUS) -1;
	}

	// 처음 호출할 때에는 Input이 없다.
	if (!bFirstCall) {
		// InputBuffers[0].cbBuffer의 크기가 필요이상으로 크다면
		// 그 정보를 InputBuffers[1]에 쓴다.
		// InputBuffers[1].BufferType는 SECBUFFER_EXTRA로 바뀌고,
		// InputBuffers[1].cbBuffer에는 사용되지 않은 바이트 수를 쓴다.
		// 물론 모자라면 InitializeSecurityContext는 SEC_E_INCOMPLETE_MESSAGE를 리턴한다.
		m__InputBuffers[0].BufferType	= SECBUFFER_TOKEN;
		m__InputBuffers[0].cbBuffer	= InputMessage.cbBuffer;
		m__InputBuffers[0].pvBuffer	= InputMessage.pvBuffer;
		m__InputBuffers[1].BufferType	= SECBUFFER_EMPTY;
		m__InputBuffers[1].cbBuffer	= 0;
		m__InputBuffers[1].pvBuffer	= NULL;

		m__InputBufferDesc.ulVersion	= SECBUFFER_VERSION;
		m__InputBufferDesc.cBuffers	= 2;
		m__InputBufferDesc.pBuffers	= m__InputBuffers;
	}
	ULONG	fContextReq =
		// 다음 세 옵션은 설정하더라도 ISC_RET에는 나타나지 않는다.
		// 반면에 AcceptSecurityContext에서는 ASC_REQ에 설정하지 않아도 ASC_RET에 항상 설정된다.	-> 설정하지 않는다.
//		ISC_REQ_REPLAY_DETECT |		// 0x00000004	! Detect replayed packets.
//		ISC_REQ_SEQUENCE_DETECT |	// 0x00000008	! Detect messages received out of sequence.
//		ISC_REQ_CONFIDENTIALITY |	// 0x00000010	! Encrypt messages.

		// 다음 두 옵션도 ISC_RET에 나타나지 않는다.
		// 반면에 AcceptSecurityContext에서는 ASC_REQ에 어떻게 설정해도 항상 ASC_RET_STREAM이 된다.	-> 설정하지 않는다.
//		ISC_REQ_STREAM |		// 0x00008000	! Support a stream-oriented connection.
//		ISC_REQ_DATAGRAM |		// 0x00000400	! Datagram semantics must be used.

		// 다음 옵션들은 있는 ISC_RET에 그대로 나타나야 한다.
		//	SChannel에서 Output을 위한 메모리를 할당한다.
		//		-> 그냥 맡기자.
		//		-> pOutputMessage에 저장한다.
////		ISC_REQ_ALLOCATE_MEMORY |	// 0x00000100	! Schannel and Digest will allocate output buffers for you. When you have finished using the output buffers, free them by calling the FreeContextBuffer function.
		//	문제가 생기면 다른 쪽(peer)가 알아듣게 해야 한다.	-> 설정한다.
		ISC_REQ_EXTENDED_ERROR |	// 0x00004000	! When errors occur, the remote party will be notified.
		//	SChannel이 메시지를 잘 묶도록 한다.	-> 설정하지 않는다.
//		ISC_REQ_CONNECTION |		// 0x00000800	! The security context will not handle formatting messages.

		// 다음 세 옵션에 대한 설명은 다음에...
//		ISC_REQ_MUTUAL_AUTH |		// 0x00000002	! Client and server will be authenticated.
//		ISC_REQ_USE_SUPPLIED_CREDS |	// 0x00000080	! Schannel must not attempt to supply credentials for the client automatically.
		// 믿을만한 CA인지 자동적으로 확인하지 않도록 한다.
		// 믿는 CA에서 오지 않은 인증서라면 InquireSecurityContext에서 SEC_E_UNTRUSTED_ROOT가 리턴된다.
		// 이런 경우에 다음 옵션을 설정한다.
//		ISC_REQ_MANUAL_CRED_VALIDATION |// 0x00080000	! Schannel must not authenticate the server automatically.

		// 다음 옵션은 설정하나 안하나 ISC_RET에는 나타나지 않는다.
		// 특히 AcceptSecurityContext에서는 사용하면 안된다.	-> 사용하지 않는다.
//		ISC_REQ_INTEGRITY |		// 0x00010000	! Sign messages and verify signatures.
		0
	;

	ULONG	fContextAttr;
	// 초기화한다.
	m__OutputBuffers[0].BufferType	= SECBUFFER_TOKEN;
	m__OutputBuffers[0].cbBuffer	= OutputMessage.cbBuffer;
	m__OutputBuffers[0].pvBuffer	= OutputMessage.pvBuffer;
	m__OutputBufferDesc.ulVersion	= SECBUFFER_VERSION;
	m__OutputBufferDesc.cBuffers	= 1;
	m__OutputBufferDesc.pBuffers	= m__OutputBuffers;

#ifdef UNICODE
	SECURITY_STATUS	SecurityStatus = m__lpTCPBase->mf__GetPSFT()->InitializeSecurityContextW(
		&m__lpTCPBase->mf__GetCredential(),		//	PCredHandle	phCredential	! [IN]
////		NULL,				//	PCtxtHandle	phContext	! [IN] On the first call to InitializeSecurityContext, this pointer is NULL.
////		&m__hContext,			// 	PCtxtHandle	phContext	! [IN] When using the Schannel SSP, on the first call to InitializeSecurityContext, specify NULL. On future calls, specify the token received in the phNewContext parameter after the first call to this function.
		bFirstCall ? NULL : &m__hContext,
		(SEC_WCHAR *) ((gmTCPClient *) m__lpTCPBase)->mf__GetPTargetName(),	//	SEC_CHAR	*pszTargetName	! [IN]
		fContextReq,			//	ULONG		fContextReq	! [IN]
		0,				//	ULONG		Reserved1	! Reserved value; must be zero.
		0,				//	ULONG		TargetDataRep	! [IN] Not used with Digest or Schannel. Specify zero.
////		NULL,				//	PSecBufferDesc	pInput		! [IN/OUT] The pointer must be NULL on the first call to the function.
////		&m__InputBufferDesc,		//	PSecBufferDesc	pInput		! [IN/OUT] On subsequent calls to the function, it is a pointer to a buffer allocated with enough memory to hold the token returned by the remote peer.
		bFirstCall ? NULL : &m__InputBufferDesc,
		0,				//	ULONG		Reserved2	! Reserved value; must be zero.
////		&m__hContext,			//	PCtxtHandle	phNewContext	! [IN/OUT] On the first call to InitializeSecurityContext, this pointer receives the new context handle.
////		NULL,				//	PCtxtHandle	phNewContext	! [IN/OUT] When using the Schannel SSP, on calls after the first call, pass the handle returned here as phContext parameter and specify NULL for phNewContext.
		bFirstCall ? &m__hContext : NULL,
		&m__OutputBufferDesc,		//	PSecBufferDes	pOutput		! [OUT] Pointer to a SecBufferDesc structure that contains pointers to the SecBuffer structure that receives the output data. If a buffer was typed as SEC_READWRITE in the input, it will be there on output. The system will allocate a buffer for the security token if requested (through ISC_REQ_ALLOCATE_MEMORY) and fill in the address in the buffer descriptor for the security token.	When using the Schannel SSP, if the ISC_REQ_ALLOCATE_MEMORY flag is specified, the Schannel SSP will allocate memory for the buffer and put the appropriate information in the SecBufferDesc.
		&fContextAttr,			//	PULONG		pfContextAttr	! [OUT]
		NULL				//	PTimeStamp	ptsExpiry	! [OUT] This parameter is optional and NULL should be passed for short-lived clients.
	);
#else	// UNICODE
	SECURITY_STATUS	SecurityStatus = m__lpTCPBase->mf__GetPSFT()->InitializeSecurityContextA(
		&m__lpTCPBase->mf__GetCredential(),		//	PCredHandle	phCredential	! [IN]
////		NULL,				//	PCtxtHandle	phContext	! [IN] On the first call to InitializeSecurityContext, this pointer is NULL.
////		&m__hContext,			// 	PCtxtHandle	phContext	! [IN] When using the Schannel SSP, on the first call to InitializeSecurityContext, specify NULL. On future calls, specify the token received in the phNewContext parameter after the first call to this function.
		bFirstCall ? NULL : &m__hContext,
		(SEC_CHAR *) ((gmTCPClient *) m__lpTCPBase)->mf__GetPTargetName(),	//	SEC_CHAR	*pszTargetName	! [IN]
		fContextReq,			//	ULONG		fContextReq	! [IN]
		0,				//	ULONG		Reserved1	! Reserved value; must be zero.
		0,				//	ULONG		TargetDataRep	! [IN] Not used with Digest or Schannel. Specify zero.
////		NULL,				//	PSecBufferDesc	pInput		! [IN/OUT] The pointer must be NULL on the first call to the function.
////		&m__InputBufferDesc,		//	PSecBufferDesc	pInput		! [IN/OUT] On subsequent calls to the function, it is a pointer to a buffer allocated with enough memory to hold the token returned by the remote peer.
		bFirstCall ? NULL : &m__InputBufferDesc,
		0,				//	ULONG		Reserved2	! Reserved value; must be zero.
////		&m__hContext,			//	PCtxtHandle	phNewContext	! [IN/OUT] On the first call to InitializeSecurityContext, this pointer receives the new context handle.
////		NULL,				//	PCtxtHandle	phNewContext	! [IN/OUT] When using the Schannel SSP, on calls after the first call, pass the handle returned here as phContext parameter and specify NULL for phNewContext.
		bFirstCall ? &m__hContext : NULL,
		&m__OutputBufferDesc,		//	PSecBufferDes	pOutput		! [OUT] Pointer to a SecBufferDesc structure that contains pointers to the SecBuffer structure that receives the output data. If a buffer was typed as SEC_READWRITE in the input, it will be there on output. The system will allocate a buffer for the security token if requested (through ISC_REQ_ALLOCATE_MEMORY) and fill in the address in the buffer descriptor for the security token.	When using the Schannel SSP, if the ISC_REQ_ALLOCATE_MEMORY flag is specified, the Schannel SSP will allocate memory for the buffer and put the appropriate information in the SecBufferDesc.
		&fContextAttr,			//	PULONG		pfContextAttr	! [OUT]
		NULL				//	PTimeStamp	ptsExpiry	! [OUT] This parameter is optional and NULL should be passed for short-lived clients.
	);
#endif	// !UNICODE
/*
 * 다음 중의 하나일 때 성공이다. 즉 m__hContext는 유효하다. (이것도 어디까지나 추측이다.)

	SEC_E_OK
	SEC_I_CONTINUE_NEEDED
+	SEC_I_COMPLETE_NEEDED
+	SEC_I_COMPLETE_AND_CONTINUE
	SEC_E_INCOMPLETE_MESSAGE
	SEC_I_INCOMPLETE_CREDENTIALS

+: SChannel에서는 사용되지 않는다.

  * 하지만 여기서는 모든 과정은 메시지 단위로 이루어지므로
  * MessageTLS에서는 다음 두 경우만을 성공으로 본다.

	SEC_E_OK
	SEC_I_CONTINUE_NEEDED

  */
	if (
		SecurityStatus == SEC_E_OK ||			// Handshake 마지막이다.
		SecurityStatus == SEC_I_CONTINUE_NEEDED ||	// Handshake 도중이다.
		SecurityStatus == SEC_I_COMPLETE_NEEDED ||
		SecurityStatus == SEC_I_COMPLETE_AND_CONTINUE ||
		SecurityStatus == SEC_E_INCOMPLETE_MESSAGE ||
		SecurityStatus == SEC_I_INCOMPLETE_CREDENTIALS
	) {
		m__bValidContext	= TRUE;
		OutputMessage.cbBuffer	= m__OutputBuffers[0].cbBuffer;
	} else {
		return SecurityStatus;
	}

	// 메시지 단위로 모든 것이 이루어지므로 남아도 안된다.
	if (!bFirstCall && m__InputBuffers[1].BufferType != SECBUFFER_EMPTY) {
		// SECBUFFER_EXTRA일 것이다.
		// 즉 주어진 메시지중에서 처리하고 남은 것이 있다.
		SecurityStatus = (SECURITY_STATUS) -1;
	}
	// 모자라다면 SEC_I_INCOMPLETE_MESSAGE가 리턴된다.

	return SecurityStatus;
}


/*
 * AcceptSecurityContext를 사용한다.
 * 클라리언트가 서버에 보낸 Token을 사용한다.
 * 서버가 클라이언트에 보낼 Token을 만든다.

	SECURITY_STATUS SEC_Entry
	AcceptSecurityContext(
		PCredHandle	phCredential,	// [IN]		! Handle to the server's credentials. The server calls the AcquireCredentialsHandle function with the INBOUND flag set to retrieve this handle.
		PCtxtHandle	phContext,	// [IN/OUT]	! Pointer to a CtxtHandle structure. On the first call to AcceptSecurityContext, this pointer is NULL. On subsequent calls, phContext is the handle to the partially formed context that was returned in the phNewContext parameter by the first call.
		PSecBufferDesc	pInput,		// [IN]		! Pointer to a SecBufferDesc structure generated by a client call to InitializeSecurityContext that contains the input buffer descriptor.	When using the Schannel ssp, the first buffer must be of type SECBUFFER_TOKEN and contain the security token received from the client. The second buffer should be of type SECBUFFER_EMPTY.
		ULONG		fContextReq,	// [IN]		! Bit flags that specify the attributes required by the server to establish the context. Bit flags can be combined using bitwise OR operations.	Flags used for this parameter are prefixed with ASC_REQ, such as ASC_REQ_DELEGATE.
		ULONG		TargetDataRep,	// [IN]		! Not used with Schannel or Digest SSPs. Specify zero (0) with when used with either of these packages.
		PCtxtHandle	phNewContext,	// [IN/OUT]	! Pointer to a CtxtHandle structure. On the first call to AcceptSecurityContext, this pointer receives the new context handle. On subsequent calls, phNewContext can be the same as the handle specified in the phContext parameter.
		PSecBufferDesc	pOutput,	// [IN/OUT]	! Pointer to a SecBufferDesc structure that contains the output buffer descriptor. This buffer is sent to the client for input into additional calls to InitializeSecurityContext. An output buffer may be generated even if the function returns SEC_E_OK. Any buffer generated must be sent back to the client application.	When using Schannel, on output, this buffer receives a token for the security context. The token must be sent to the client. The function can also return a buffer of type SECBUFFER_EXTRA.
		PULONG		pfContextAttr,	// [OUT]	! Pointer to a variable that receives a set of bit flags indicating the attributes of the established context. For a description of the various attributes, see Context Requirements. Flags used for this parameter are prefixed with ASC_RET, such as ASC_RET_DELEGATE.	Do not check for security-related attributes until the final function call returns successfully. Attribute flags not related to security, such as the ASC_RET_ALLOCATED_MEMORY flag, can be checked before the final return.
		PTimeStamp	ptsTimeStamp	// [OUT]	! Pointer to a TimeStamp structure that receives the expiration time of the context. It is recommended that the security package always return this value in local time.	This is optional when using the Schannel SSP. When the remote party has supplied a certificate to be used for authentication, this parameter receives the expiration time for that certificate. If no certificate was supplied, a maximum time value is returned.	Until the last call of the authentication process, the expiration time for the context can be incorrect because more information will be provided during later stages of the negotiation dialog. Therefore, ptsTimeStamp must be NULL until the last call to the function.	Windows 2000/NT, Windows Me/98/95:  This parameter is not used.
	);
 */
SECURITY_STATUS
gmPseudoTCP::mf__AcceptSecurityContext(
	MSGBLOB&	InputMessage,
	MSGBLOB&	OutputMessage
)
{
	BOOL	bFirstCall = m__bValidContext ? FALSE : TRUE;
	if (bFirstCall && m__bValidContext) {
		// 처음 호출했는 데, 이미 m__phContext값이 있다.
		return (SECURITY_STATUS) -1;
	}
	if (!bFirstCall && !m__bValidContext) {
		// 처움 호출이 아닌데, m__phContext값이 설정되어 있지 않다.
		return (SECURITY_STATUS) -1;
	}

	// InputBuffers[0].cbBuffer의 크기가 필요이상으로 크다면
	// 그 정보를 InputBuffers[1]에 쓴다.
	// InputBuffers[1].BufferType는 SECBUFFER_EXTRA로 바뀌고,
	// InputBuffers[1].cbBuffer에는 사용되지 않은 바이트 수를 쓴다.
	// 물론 모자라면 AcceptSecurityContext는 SEC_E_INCOMPLETE_MESSAGE를 리턴한다.
	m__InputBuffers[0].BufferType	= SECBUFFER_TOKEN;
	m__InputBuffers[0].cbBuffer	= InputMessage.cbBuffer;
	m__InputBuffers[0].pvBuffer	= InputMessage.pvBuffer;
	m__InputBuffers[1].BufferType	= SECBUFFER_EMPTY;
	m__InputBuffers[1].cbBuffer	= 0;
	m__InputBuffers[1].pvBuffer	= NULL;
	m__InputBufferDesc.ulVersion	= SECBUFFER_VERSION;
	m__InputBufferDesc.cBuffers	= 2;
	m__InputBufferDesc.pBuffers	= m__InputBuffers;
	ULONG	fContextReq = 
		// 다음 세 옵션은 요구(Request)하지 않더라도 항상 설정(Return)된다.	-> 설정하지 않는다.
//		ASC_REQ_CONFIDENTIALITY |	// 0x00000010	! Encrypt and decrypt messages.
//		ASC_REQ_REPLAY_DETECT |		// 0x00000004	! Detect replayed packets.
//		ASC_REQ_SEQUENCE_DETECT |	// 0x00000008	! Detect messages received out of sequence.

		// 다음 두 옵션은 어떻게 설정해도 항상 ASC_RET_STREAM이 된다.	-> 설정하지 않는다.
//		ASC_REQ_DATAGRAM |		// 0x00000400	! Datagram semantics must be used.
//		ASC_REQ_STREAM |		// 0x00010000	! Support a stream-oriented connection. Schannel will set this flag automatically if you do not.

		// 다음 옵션들은 있는 ASC_RET에 그대로 나타나야 한다.
		//	SChannel에서 Output을 위한 메모리를 할당한다.
		//		-> 그냥 맡기자.
		//		-> pOutputMessage에 저장한다.
////		ASC_REQ_ALLOCATE_MEMORY |	// 0x00000100	! Digest and Schannel will allocate output buffers for you. When you have finished using the output buffers, free them by calling the FreeContextBuffer function.
		//	문제가 생기면 다른 쪽(peer)가 알아듣게 해야 한다.	-> 설정한다.
		ASC_REQ_EXTENDED_ERROR |	// 0x00008000	! When errors occur, the remote party will be notified.
		//	SChannel이 메시지를 잘 묶도록 한다.	-> 설정하지 않는다.
//		ASC_REQ_CONNECTION |		// 0x00000800	! The security context will not handle formatting messages.
		//	클라이언트로부터 인증서를 요구하지 않는다.	-> 설정하지 않는다.
//		ASC_REQ_MUTUAL_AUTH |		// 0x00000002	! The client is required to supply a certificate to be used for client authentication.
		0
	;

	m__OutputBuffers[0].BufferType	= SECBUFFER_TOKEN;	// 항상(?)
	m__OutputBuffers[0].cbBuffer	= OutputMessage.cbBuffer;
	m__OutputBuffers[0].pvBuffer	= OutputMessage.pvBuffer;

	m__OutputBufferDesc.ulVersion	= SECBUFFER_VERSION;
	m__OutputBufferDesc.cBuffers	= 1;
	m__OutputBufferDesc.pBuffers	= m__OutputBuffers;
	ULONG	fContextAttr;
	SECURITY_STATUS	SecurityStatus = m__lpTCPBase->mf__GetPSFT()->AcceptSecurityContext(
		&m__lpTCPBase->mf__GetCredential(),		//	PCredHandle	phCredential	! [IN]
////		NULL,				//	PCtxtHandle	phContext	! [IN] On the first call to AcceptSecurityContext, this pointer is NULL.
////		&m__hContext,			//	PCtxtHandle	phContext	! [IN] On subsequent calls, phContext is the handle to the partially formed context that was returned in the phNewContext parameter by the first call.
		bFirstCall ? NULL : &m__hContext,
		&m__InputBufferDesc,		//	PSecBufferDesc	pInput		! [IN] When using the Schannel ssp, the first buffer must be of type SECBUFFER_TOKEN and contain the security token received from the client. The second buffer should be of type SECBUFFER_EMPTY.
		fContextReq,			//	ULONG		fContextReq	! [IN]
		0,				//	ULONG		TargetDataRep	! [IN] Not used with Schannel or Digest SSPs. Specify zero (0) with when used with either of these packages.
////		&m__hContext,			//	PCtxtHandle	phNewContext	! [IN/OUT] On the first call to AcceptSecurityContext, this pointer receives the new context handle. On subsequent calls, phNewContext can be the same as the handle specified in the phContext parameter.
		bFirstCall ? &m__hContext : NULL,
		&m__OutputBufferDesc,		//	PSecBufferDesc	pOutput		! [IN/OUT] Pointer to a SecBufferDesc structure that contains the output buffer descriptor. This buffer is sent to the client for input into additional calls to InitializeSecurityContext. An output buffer may be generated even if the function returns SEC_E_OK. Any buffer generated must be sent back to the client application.	When using Schannel, on output, this buffer receives a token for the security context. The token must be sent to the client. The function can also return a buffer of type SECBUFFER_EXTRA.
		&fContextAttr,			//	PULONG		pfContextAttr	! [OUT]
		NULL				//	PTimeStamp	ptsExpiry	! [OUT] This parameter is optional and NULL should be passed for short-lived clients.
	);
/*
 * AcceptSecurityContext의 리턴값은 InitializeSecurityContext와 달리,
 * 어느 경우가 성공인지 실패인지가 명확히 말해주지 않는다.
 * 즉 m__hContext가 유효한지 어떤 지를 판단할 수 없다.
 * 어쨌든 리턴값은 다음 중의 하나이다.

	SEC_E_OK
	SEC_I_CONTINUE_NEEDED
+	SEC_I_COMPLETE_NEEDED
+	SEC_I_COMPLETE_AND_CONTINUE
	SEC_E_INVALID_TOKEN
	SEC_E_INVALID_HANDLE
x	SEC_E_LOGON_DENIED
	SEC_E_INTERNAL_ERROR
x	SEC_E_NO_AUTHENTICATING_AUTHORITY
	SEC_E_NO_CREDENTIALS
+	SEC_E_SECURITY_QOS_FAILED
+	STATUS_LOGON_FAILURE
	SEC_E_INCOMPLETE_MESSAGE
	SEC_E_UNSUPPORTED_FUNCTION
	SEC_E_INSUFFICIENT_MEMORY

+: SChannel에서는 사용되지 않는다.
x: SChannel에서 사용되는지 확실하지 않다.
 */
	// 메시지 단위로 모든 처리가 이루어지므로 다음 둘 중의 하나를 성공으로 본다.
	if (
		SecurityStatus == SEC_E_OK ||			// Handshake 마지막이다.
		SecurityStatus == SEC_I_CONTINUE_NEEDED		// Handshake 도중이다.
	) {
		m__bValidContext	= TRUE;
		OutputMessage.cbBuffer	= m__OutputBuffers[0].cbBuffer;
	} else {
		return SecurityStatus;
	}

	// 메시지 단위로 모든 것이 이루어지므로 남아도 안된다.
	if (m__InputBuffers[1].BufferType != SECBUFFER_EMPTY) {
		// SECBUFFER_EXTRA일 것이다.
		// 즉 주어진 메시지중에서 처리하고 남은 것이 있다.
		SecurityStatus = (SECURITY_STATUS) -1;
	}
	// 모자라다면 SEC_I_INCOMPLETE_MESSAGE가 리턴된다.

	return SecurityStatus;
}


// Handshake Protocol이 끝났을 때에 호출해야 한다.
SECURITY_STATUS
gmPseudoTCP::mf__GetSizes(
	void
)
{
	SECURITY_STATUS	SecurityStatus = QueryContextAttributes(
		&m__hContext,
		SECPKG_ATTR_SIZES,
		&m__Sizes
	);
	if (SecurityStatus != SEC_E_OK)
		return SecurityStatus;

#ifdef UNICODE
	SecurityStatus = m__lpTCPBase->mf__GetPSFT()->QueryContextAttributesW(
		&m__hContext,
		SECPKG_ATTR_STREAM_SIZES,
		&m__StreamSizes
	);
#else	// UNICODE
	SecurityStatus = m__lpTCPBase->mf__GetPSFT()->QueryContextAttributesA(
		&m__hContext,
		SECPKG_ATTR_STREAM_SIZES,
		&m__StreamSizes
	);
#endif	// !UNICODE

	return SecurityStatus;
}


/*
 * 다음은 EncryptMessage함수에 대한 설명에서 필요한 부분만 요약했다.
 *
	Pointer to a SecBufferDesc structure.
	On input, the structure references one or more SecBuffer structures.
	Only one of these can be of type SECBUFFER_DATA.
	That buffer contains the message to be encrypted.
	The function does not process buffers with the SECBUFFER_READONLY attribute.

	The message in the SecBuffer structure is encrypted in place;
	that is, the encrypted message overwrites the original data in the structure.
	Only one SecBuffer structure of type SECBUFFER_DATA is permitted.

	The length of the SecBuffer structure that contains the message must be
	no greater than cbMaximumMessage,
	which is obtained from the QueryContextAttributes (SECPKG_ATTR_STREAM_SIZES) function.


	When used with the Schannel SSP, the pMessage parameter must contain a SecBufferDesc structure
	with the buffers described in the following table.

		Note  These buffers must be supplied in the order shown.

	Buffer type			Description
	SECBUFFER_STREAM_HEADER		Used internally. No initialization required.
	SECBUFFER_DATA			Contains the plaintext message to be encrypted.
	SECBUFFER_STREAM_TRAILER	Used internally. No initialization required.
	SECBUFFER_EMPTY			Used internally. No initialization required. Size can be zero.

	When you use the Schannel SSP, determine the maximum size of each of the buffers
	by calling the QueryContextAttributes function and specifying the SECPKG_ATTR_STREAM_SIZES attribute.
	This function returns a SecPkgContext_StreamSizes structure
	whose members contain the maximum sizes for the header (cbHeader member),
	message (cbMaximumMessage member) and trailer (cbTrailer member) buffers.

	For optimal performance, the pMessage structures should be allocated from contiguous memory.
 */
SECURITY_STATUS
gmPseudoTCP::mf__EncryptMessage(
	MSGBLOB&	InputMessage,
	MSGBLOB&	OutputMessage
)
{
	if (InputMessage.cbBuffer > m__StreamSizes.cbMaximumMessage) {
		// 암화화된 메시지의 크기는 SecPkgContext_StreamSizes.cbMaximumMessage보다
		// 크면 안된다.
		return (SECURITY_STATUS) -1;
	}

	ULONG	cbMessage = InputMessage.cbBuffer;
	// (머리 + 암호화된 메시지 + 꼬리)가 포함될 수 있는 공간의 크기를 구한다.
	ULONG	cbIoBuffer = m__StreamSizes.cbHeader + cbMessage + m__StreamSizes.cbTrailer;
	if (cbIoBuffer > OutputMessage.cbBuffer) {
		// 저장할 공간보다 크면 안된다.
		return (SECURITY_STATUS) -1;
	}

	// "머리" 부분을 먼저 쓰고,
	m__MessageBuffers[0].BufferType	= SECBUFFER_STREAM_HEADER;
	m__MessageBuffers[0].cbBuffer	= m__StreamSizes.cbHeader;
	m__MessageBuffers[0].pvBuffer	= OutputMessage.pvBuffer;	// No initialization required.

	// ("머리") 바로 다음에 암호화할 메시지를 쓴다.
	PBYTE	pbMessage = (PBYTE) OutputMessage.pvBuffer + m__StreamSizes.cbHeader;
	// 암호화될 부분(머리, 메시지, 꼬리)는 가능한 연속적인 메모리에 있어야 한다.
	CopyMemory(pbMessage, InputMessage.pvBuffer, cbMessage);
	m__MessageBuffers[1].BufferType	= SECBUFFER_DATA;
	m__MessageBuffers[1].cbBuffer	= cbMessage;	// 암호화할 메시지의 크기
	m__MessageBuffers[1].pvBuffer	= pbMessage;	// 암호화할 메시지

	// 마지막으로 (메시지) 바로 다음에 "꼬리"를 쓴다.
	m__MessageBuffers[2].BufferType	= SECBUFFER_STREAM_TRAILER;
	m__MessageBuffers[2].cbBuffer	= m__StreamSizes.cbTrailer;
	m__MessageBuffers[2].pvBuffer	= pbMessage + cbMessage;	// No initialization required.

	m__MessageBuffers[3].BufferType	= SECBUFFER_EMPTY;		// No initialization required.
	m__MessageBuffers[3].cbBuffer	= 0;				// Size can be zero.

	m__MessageBufferDesc.ulVersion	= SECBUFFER_VERSION;
	m__MessageBufferDesc.cBuffers	= 4;
	m__MessageBufferDesc.pBuffers	= m__MessageBuffers;

	// Encrypt Message
// 각 연결마다 독립적인 Context핸들을 사용하므로 Lock을 걸 필요가 없다.(?)
//	2005/4/7 (EST) - ghkim
//	m__lpTCPBase->mf__lock();
	SECURITY_STATUS	SecurityStatus = m__lpTCPBase->mf__GetPSFT()->EncryptMessage(
		&m__hContext,
		0,			// [IN]		! The value of this parameter is not used with the Microsoft Digest SSP or the Schannel SSP. With those SSPs, zero should be specified.
		&m__MessageBufferDesc,	// [IN/OUT]
		0			// [IN]		! The value of this parameter is not used with the Microsoft Digest SSP or the Schannel SSP. Zero should be used for this parameter when using either of these SSPs.
	);
//	m__lpTCPBase->mf__unlock();
	if (SecurityStatus == SEC_E_OK) {
		// 암호화된 총 크기는...
		OutputMessage.cbBuffer = m__MessageBuffers[0].cbBuffer + m__MessageBuffers[1].cbBuffer + m__MessageBuffers[2].cbBuffer;
	}

	return SecurityStatus;
}


/*
 * 다음은 DecryptMessage함수에 대한 설명에서 필요한 부분만 요약했다.
 *
	Pointer to a SecBufferDesc structure.
	On input, the structure references one or more SecBuffer structures.
	One of these may be of type SECBUFFER_DATA.
	That buffer contains the encrypted message.
	The encrypted message is decrypted in place, overwriting the original contents of its buffer.

	When used with the Schannel SSP with contexts that are not connection-oriented,
	on input, the structure must contain four SecBuffer structures.
	Exactly one buffer must be of type SECBUFFER_DATA and contain an encrypted message,
	which is decrypted in place. The remaining buffers are used for output
	and must be of type SECBUFFER_EMPTY.
	For connection-oriented contexts, a SECBUFFER_DATA type buffer must be supplied,
	as noted for non-connection-oriented contexts.
	Additionally, a second SECBUFFER_TOKEN type buffer that contains a security token
	must also be supplied.
 */
SECURITY_STATUS
gmPseudoTCP::mf__DecryptMessage(
	MSGBLOB&	InputMessage,
	MSGBLOB&	OutputMessage
)
{
	m__MessageBuffers[0].BufferType	= SECBUFFER_DATA;
	m__MessageBuffers[0].cbBuffer	= InputMessage.cbBuffer;
	m__MessageBuffers[0].pvBuffer	= InputMessage.pvBuffer;
	m__MessageBuffers[1].BufferType	= SECBUFFER_EMPTY;
	m__MessageBuffers[2].BufferType	= SECBUFFER_EMPTY;
	m__MessageBuffers[3].BufferType	= SECBUFFER_EMPTY;

	m__MessageBufferDesc.ulVersion	= SECBUFFER_VERSION;
	m__MessageBufferDesc.cBuffers	= 4;
	m__MessageBufferDesc.pBuffers	= m__MessageBuffers;

// 각 연결마다 독립적인 Context핸들을 사용하므로 Lock을 걸 필요가 없다.(?)
//	2005/4/7 (EST) - ghkim
//	m__lpTCPBase->mf__lock();
	SECURITY_STATUS	SecurityStatus = m__lpTCPBase->mf__GetPSFT()->DecryptMessage(
		&m__hContext,
		&m__MessageBufferDesc,	// [IN/OUT]
		0,			// [IN]		! For the Microsoft Digest and Schannel SSPs, specify as zero. Microsoft Digest manages sequence numbering internally and sequence numbers are not used with Schannel.
		NULL			// [OUT]	! For Schannel SSP, specify as NULL.
	);
//	m__lpTCPBase->mf__unlock();
/*
	When you use the Schannel SSP, the DecryptMessage function returns
	SEC_I_CONTEXT_EXPIRED when the message sender has shut down the connection.
	After receiving this return value, your application must complete the following steps:
		Call EncryptMessage, passing in an empty input buffer.
		Send the output buffers from the EncryptMessage call to the remote party (the sender of the decrypted message).
		Delete the security context by calling the DeleteSecurityContext function.

	When you use the Schannel SSP, DecryptMessage returns SEC_I_RENEGOTIATE
	when the message sender wants to renegotiate the connection (security context).
	An application handles a requested renegotiation by calling AcceptSecurityContext (server side) or
	InitializeSecurityContext (client side) and passing in empty input buffers.
	After this initial call returns a value,
	proceed as though your application was creating a new connection.
 */
	// 어떤 메시지를 보내는 지 알기때문에 SEC_I_CONTEXT_EXPIRED, SEC_I_RENEGOTIATE를
	// 처리하지 않아도 된다.
	if (SecurityStatus != SEC_E_OK) {
		// SEC_E_INVALID_TOKEN		0x80090308L	! 메시지의 시작위치를 한 바이트 다음 위치로 바꾸었을 때
		// SEC_E_MESSAGE_ALTERED	0x8009030FL	! 메시지 중에서 한 바이트를 바꾸었을 때
		// SEC_E_INCOMPLETE_MESSAGE	0x80090318L	! 메시지의 크기를 한 바이트 줄였을 때
		return SecurityStatus;
	}
	// 각 버퍼의 타입을 조사해서 암호가 풀린 메시지(SECBUFFER_DATA)와
	// 그리고 (있다면) 남은(SECBUFFER_EXTRA) 바이트를 얻는다.
/*
#define SECBUFFER_EMPTY			0	// Undefined, replaced by provider
#define SECBUFFER_DATA			1	// Packet data
#define SECBUFFER_EXTRA			5	// Extra data
#define SECBUFFER_STREAM_TRAILER	6	// Security Trailer
#define SECBUFFER_STREAM_HEADER		7	// Security Header
 */
	for (int i = 0; i < 4; ++i) {
		switch (m__MessageBuffers[i].BufferType) {
		case SECBUFFER_STREAM_HEADER:
//			printf("[%u]: EXTRA(# = %u)\n", i, m__MessageBuffers[i].cbBuffer);
			break;
		case SECBUFFER_DATA:
//			printf("[%u]: DATA(# = %u)\n", i, m__MessageBuffers[i].cbBuffer);
			OutputMessage.cbBuffer = m__MessageBuffers[i].cbBuffer;
			MoveMemory(OutputMessage.pvBuffer, m__MessageBuffers[i].pvBuffer, m__MessageBuffers[i].cbBuffer);
			break;
		case SECBUFFER_STREAM_TRAILER:
//			printf("[%u]: TRAILER(# = %u)\n", i, m__MessageBuffers[i].cbBuffer);
			break;
		case SECBUFFER_EXTRA:
			// 필요한 데이터보다 많은 데이터가 왔을 때,
			// 해석하고 남은 바이트 수가 이 버퍼에 기록된다.
//			printf("[%u]: EXTRA(# = %u)\n", i, m__MessageBuffers[i].cbBuffer);
			SecurityStatus = (SECURITY_STATUS) -2;
			break;
		case SECBUFFER_EMPTY:
			// 주어진 데이터가 모두 다 해석되었다.
//			printf("[%u]: EMPTY\n", i);
			break;
		default:
			SecurityStatus = (SECURITY_STATUS) -1;
			break;
		}
	}

	return SecurityStatus;
}

BOOL
gmPseudoTCP::RecvMessage(
	MTCPMSG&	MsgReceived
)
{
	BOOL	bReturnVal;

	switch (mf__GetMessage(&MsgReceived)) {
//////// 이미 "받는" 큐에 메시지가 있다는 걸 알았으므로, 오직 이 경우만이 성공한 경우이다.
	case TCPBUF__OK:
		bReturnVal = TRUE;
		break;

//////// 나머지 경우는 모두 실패
	case TCPBUF__NOT_ESTABLISHED:
		// 연결이 이미 끊어졌다. (정상적으로)
////		if (hTCP->mf__closeSend()) {
////			RomeoMustDie();
////		}
//		break;
	case TCPBUF__NO_HEADER:
	case TCPBUF__PARTIAL_DATA:
		// 다음에 가져올 수 밖에...
//		break;
	case TCPBUF__NOT_ENOUGH:
		// MTCPMSG.Header.cbData값을 제대로 설정했나?
		// 아니면 들어오는 데이터가 제대로 된 것인가?
//		break;
	case TCPBUF__OK_BUT:
		// 정지된 "받는" 큐를 활성화할 때 문제가 있었다.
		// WSARecv에 문제가 있다.
		//	+ 많은 경우는 상대편에서 연결을 끊었을 때 생긴다.
		// 해당 연결을 끊어야 한다.
////		if (lpTCPBase->mf__GetWSALastErrorCode() == WSAECONNRESET) {
////			hTCP->mf__disconnect();
////		} else {
////			RomeoMustDie();
////		}
//		break;
	case TCPBUF__INTERNAL_ERROR:
		// 정지된 "받는" 큐를 활성화할 때 문제가 있었다.
		// 큐가 가득 찼다는 말인데, 메시지를 적어도 하나 처리하였으므로
		// 이럴 수는 없는데...
//		break;
	default:
		// 이럴 수가???
		bReturnVal = FALSE;
		break;
	}

	return bReturnVal;
}

#define MAXLOOPCOUNTER	10	// 이전에 호출한 WSASend가 진행중인 경우에,
				// 최대 MAXLOOPCOUNTER의 시도안에 메시지를 보내야 한다.
// 2005/4/21 (EST) - ghkim
//	TCPSENDMSG__WSASEND_IN_PROGRESS의 경우에 단순히 루프를 반복하지 않고,
//	루프를 돌기전에 Sleep을 써서 주어진 시간동안 멈추어 있도록 한다.
BOOL
gmPseudoTCP::SendMessage(
	MTCPMSG&	MsgToBeSent
)
{
	BOOL	bReturnVal;

	for (UINT LoopCounter = 0; LoopCounter < SENDMESSAGE_MAXLOOPCOUNTER; ++LoopCounter) {
		INT	iReturnVal = mf__SendMessage(&MsgToBeSent);	// gmPseudoTCP::mf__SendMsg의 리턴값과 같다.
		if (iReturnVal == TCPSENDMSG__WSASEND_IN_PROGRESS) {
			// 이전에 호출한 WSASend가 진행 중이다.
			// 다음에 보내자.
			// 최대 MAXLOOPCOUNTER만큼 시도를 한다.
#ifdef _DEBUG
			m__lpTCPBase->mf__fprintf(m__lpTCPBase->m__CONERR,
				TEXT("gmPseudoTCP::SendMessage: TCPSENDMSG__WSASEND_IN_PROGRESS\n")
			);
#endif	// _DEBUG
			Sleep(SENDMESSAGE_INTERVAL);
			continue;
		}

// 위의 경우()를 제외하고,
// 나머지 경우는 한 번의 gmPseudoTCP::mf__SendMessage의 호출로,
// 제대로 메시지를 보냈는 지 아닌 지를 알 수 있다.
		switch (iReturnVal) {
//////// 다음 두 경우만이 메시지를 보내기 위해, 성공적으로 WSASend를 호출한 경우이다.
		case TCPSENDMSG__OK_IMMEDIATELY:
			// WSASend에서 곧 바로 데이터를 보냈다.
			// 데이터를 다 보냈다.
//			break;
		case TCPSENDMSG__OK:
			// WSASend에서 데이터를 보내는 중이다.
			// Q: 다 보냈는 지 어떻게 알지?
			// A: BOOL gmPseudoTCP::mf__bIOCPSendInProgress(VOID)를 호출하자.
			bReturnVal = TRUE;
			break;

//////// 나머지는 모두 실패한 경우이다.
		case TCPSENDMSG__NOT_ESTABLISHED:
			// 연결이 이미 끊어졌다. (정상적으로)
////			if (hTCP->mf__closeSend()) {
////				RomeoMustDie();
////			}
//			break;
		case TCPSENDMSG__NOT_ENOUGH:
			// 큐에 이 메시지를 저장할 공간이 없다.
			// 큐가 가득찼거나 아니면 이 메시지의 크기가 너무 크든가 둘 중에 하나다.
			// 큐가 가득찼다면 다음에 보내자.
//			break;
		case TCPSENDMSG__ERROR:
			// WSASend에 문제가 있다.
			//	+ 많은 경우는 상대편에서 연결을 끊었을 때 생긴다.
////			if (lpTCPBase->mf__GetWSALastErrorCode() == WSAECONNRESET) {
////				hTCP->mf__disconnect();
////			} else {
////				RomeoMustDie();
////			}
//			break;
		default:
			// 이럴 수가???
			bReturnVal = FALSE;
			break;
		}
		// 이전에 호출한 WSASend가 진행중인 경우(TCPSENDMSG__WSASEND_IN_PROGRESS)를 제외하고,
		// 여기서 메시지를 보낼 수 있는 지 없는 지가 결정된다.
		break;
	}

	return LoopCounter == MAXLOOPCOUNTER ? FALSE : bReturnVal;
}

SECURITY_STATUS
gmPseudoTCP::HandshakeProtocol(
	MTCPMSG&	MsgFromPeer,		// [IN] 상대편에게서 얻은 메시지에 대한 정보이다.
						//	단, 클라이언트의 경우에 시작단계에서는 의미없는 값이다.
	MTCPMSG&	MsgToPeer,		// [IN/OUT] 이 함수에서 사용할 버퍼를 정해준다.
	BOOL		bServer
)
{
	SECURITY_STATUS	SecurityStatus;

	// TLS에 사용되는 변수
	MSGBLOB	TokenFromPeer = {
		MsgFromPeer.Header.cbData,
		MsgFromPeer.pbData
	};
	MSGBLOB	TokenToPeer = {
		MsgToPeer.Header.cbData,
		MsgToPeer.pbData
	};
	if (bServer) {
		SecurityStatus = mf__AcceptSecurityContext(
			TokenFromPeer,	// 상대편에게서 받은 Token
			TokenToPeer	// 상대편으로로 보낼 Token
		);
	} else {
		SecurityStatus = mf__InitializeSecurityContext(
			TokenFromPeer,	// 상대편에게서 받은 Token
			TokenToPeer	// 상대편으로로 보낼 Token
		);
	}
	if (
		SecurityStatus != SEC_E_OK &&
		SecurityStatus != SEC_I_CONTINUE_NEEDED
	) {
		// 메시지단위로 데이터가 전송되므로,
		// 위의 두 경우를 제외하고는 모두 문제가 있든 경우이다.
		return SecurityStatus;
//		return (SECURITY_STATUS) -1;
	}

	if (!TokenToPeer.cbBuffer) {
		// 보낼 것이 없다면 그냥 리턴한다.
		if (SecurityStatus != SEC_E_OK) {
			// 단, 이 때는 SEC_E_OK의 경우에 한한다.
			m__lpTCPBase->mf__RomeoMustDie();
			return (SECURITY_STATUS) -2;
		}
		goto LAST;
	}
	// SEC_I_CONTINUE_NEEDED인 경우(항상 보낼 것이 있어야 한다)거나
	// SEC_E_OK라도 보낼 것이 있는 경우

	MsgToPeer.Header.cbData = (USHORT) TokenToPeer.cbBuffer;
	if (!SendMessage(MsgToPeer)) {
		// 메시지를 보낼 때, 문제가 있었다.
		return (SECURITY_STATUS) -3;
	}

LAST:
	if (SecurityStatus == SEC_E_OK) {
		// Handshake Protocol이 끝났다.
		if ((SecurityStatus = mf__GetSizes()) != SEC_E_OK) {
			return SecurityStatus;
		}
		return SEC_E_OK;
	}

	return SecurityStatus;
}
#endif	// __gmTLS__

#ifdef __gmCRYPT__
#if 0	// 2.1
// dwIOSize바이트 데이터를 큐에 넣은 직후에 이 함수를 호출해서,
// 그 데이터를 암호화한다.
gmPseudoTCP::mf__EncryptQueue(
	gmQueue	*lpgmQueue,
	DWORD	dwIOSize
)
{
	return TRUE;
	BYTE	*pbData;
	DWORD	cbDataLength;
	CHAR	buf[4096];
	if (lpgmQueue->m__tail < dwIOSize) {
		// 데이터가 큐의 두 부분에 써졌다.
		// 후반부(lpgmQueue->mf__lpBuf + 0)에 lpgmQueue->m__tail바이트
		pbData = (BYTE *) lpgmQueue->m__lpBuf;
		cbDataLength = lpgmQueue->m__tail;
#if 0
	ByteToStringA(pbData, cbDataLength, buf);
	m__lpTCPBase->mf__fprintf(m__lpTCPBase->m__CONERR,
		"Encrypt(%u): %s\n"
		,
		cbDataLength,
		buf
	);
#endif
		if (!CryptEncrypt(
			m__lpTCPBase->m__hEncryptKey,
			NULL,
			FALSE,		// 아직 끝이 아니다. 영원히...
			0,
			pbData,
			&cbDataLength,
			cbDataLength	// 스트림방식이므로 버퍼의 크기와 데이터의 크기가 같다.
		)) {
			m__lpTCPBase->m__dwCryptLastErrorCode = GetLastError();
			return FALSE;
		}
#if 0
	ByteToStringA(pbData, cbDataLength, buf);
	m__lpTCPBase->mf__fprintf(m__lpTCPBase->m__CONERR,
		"Encrypt:\t%s\n"
		,
		buf
	);
#endif
		// 전반부(lpgmQueue->mf__lpBuf + (lpgmQueue->m__size - N))에 N(= dwIOSize - lpgmQueue->m__tail)바이트
		cbDataLength = dwIOSize - cbDataLength;
		pbData = (BYTE *) lpgmQueue->m__lpBuf + (lpgmQueue->m__size - cbDataLength);
	} else {
		// 전반부(lpgmQueue->mf__lpBuf + (lpgmQueue->m__tail - dwIOSize))에 dwIOSize바이트가 들어갔다.
		pbData = (BYTE *) lpgmQueue->m__lpBuf + (lpgmQueue->m__tail - dwIOSize);
		cbDataLength = dwIOSize;
	}
#if 0
	ByteToStringA(pbData, cbDataLength, buf);
	m__lpTCPBase->mf__fprintf(m__lpTCPBase->m__CONERR,
		"Encrypt(%u): %s\n"
		,
		cbDataLength,
		buf
	);
#endif
	if (!CryptEncrypt(
		m__lpTCPBase->m__hEncryptKey,
		NULL,
		FALSE,
		0,
		pbData,
		&cbDataLength,
		cbDataLength
	)) {
		m__lpTCPBase->m__dwCryptLastErrorCode = GetLastError();
		return FALSE;
	}
#if 0
	ByteToStringA(pbData, cbDataLength, buf);
	m__lpTCPBase->mf__fprintf(m__lpTCPBase->m__CONERR,
		"Encrypt:\t%s\n"
		,
		buf
	);
#endif

	return TRUE;
}

// 큐에 dwIOSize바이트 데이터가 큐에 써진 직후에 이 함수를 호출해서 그 데이터의 암호화를 푼다.
// 큐의 상태(m__length, m__tail)를 바꾸기 직전이다.
gmPseudoTCP::mf__DecryptQueue(
	gmQueue	*lpgmQueue,
	DWORD	dwIOSize
)
{
	return TRUE;
	BYTE	*pbData;
	DWORD	cbDataLength;
	DWORD	dwRest = lpgmQueue->m__size - lpgmQueue->m__tail;	// 큐의 전반부 크기
	CHAR	buf[4096];
	if (dwIOSize > dwRest) {
		// 큐의 후반부(lpgmQueue->m__lpBuf + 0)에도 (dwIOSize - dwRest)바이트 데이터가 있다.
		pbData = (BYTE *) lpgmQueue->m__lpBuf;
		cbDataLength = dwIOSize - dwRest;
#if 1
	ByteToStringA(pbData, cbDataLength, buf);
	m__lpTCPBase->mf__fprintf(m__lpTCPBase->m__CONERR,
		"Decrypt(%u): %s\n"
		,
		cbDataLength,
		buf
	);
#endif
		if (!CryptDecrypt(
			m__lpTCPBase->m__hDecryptKey,
			NULL,
			FALSE,				// 아직 끝이 아니다. 영원히...
			0,
			pbData,
			&cbDataLength
		)) {
			m__lpTCPBase->m__dwCryptLastErrorCode = GetLastError();
			return FALSE;
		}
#if 1
	ByteToStringA(pbData, cbDataLength, buf);
	m__lpTCPBase->mf__fprintf(m__lpTCPBase->m__CONERR,
		"Decrypt:\t%s\n"
		,
		buf
	);
#endif
		// 전반부(lpgmQueue->m__lpBuf + lpgmQueue->m__tail)에 있는 dwRest바이트 데이터도 암호화를 푼다.
		cbDataLength = dwRest;
	} else {
		// 전반부(lpgmQueue->m__lpBuf + lpgmQueue->m__tail)에 있는 dwIOSize바이트 데이터의 암호화를 푼다.
		cbDataLength = dwIOSize;
	}
	// 큐의 전반부(lpgmQueue->m__lpBuf + lpgmQueue->m__tail)에 있는 cbDataLength바이트의 데이터의 암호화를 푼다.
	pbData = (BYTE *) lpgmQueue->m__lpBuf + lpgmQueue->m__tail;
#if 1
	ByteToStringA(pbData, cbDataLength, buf);
	m__lpTCPBase->mf__fprintf(m__lpTCPBase->m__CONERR,
		"Decrypt(%u): %s\n"
		,
		cbDataLength,
		buf
	);
#endif

	if (!CryptDecrypt(
		m__lpTCPBase->m__hDecryptKey,
		NULL,
		FALSE,
		0,
		pbData,//(BYTE *) lpgmQueue->m__lpBuf + lpgmQueue->m__tail,
		&cbDataLength
	)) {
		m__lpTCPBase->m__dwCryptLastErrorCode = GetLastError();
		return FALSE;
	}
#if 1
	ByteToStringA(pbData, cbDataLength, buf);
	m__lpTCPBase->mf__fprintf(m__lpTCPBase->m__CONERR,
		"Decrypt:\t%s\n"
		,
		buf
	);
#endif

	return TRUE;
}
#endif	// 2.1
#endif	// __gmCRYPT__
