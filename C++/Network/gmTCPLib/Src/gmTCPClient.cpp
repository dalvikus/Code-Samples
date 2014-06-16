#include <gmTCP/gmTCP.h>
#include <ATLBASE.H>	// T2CA		! #include <ATLCONV.H>
#ifdef __gmTLS__
#include <schannel.h>	// SCHANNEL_CRED
#endif	// __gmTLS__

DWORD	WINAPI ClientSideWorkerThreadProc(LPVOID lpVoid)
{
#ifdef __gmIOCP__
	return ((gmTCPClient *) lpVoid)->mf__IOCPWork();
#else	// __gmIOCP__
	return ((gmTCPClient *) lpVoid)->mf__EventWork();
#endif	// !__gmIOCP__
}

gmTCPClient::gmTCPClient(
	gmCONN*	lpConnV,
	UINT	nConnC,
#ifdef __gmUSE_PUBLIC_QUEUE__
	UINT	PubSize,
#endif	// __gmUSE_PUBLIC_QUEUE__
	UINT	MaxTCPMsgSize,
	DWORD	dwMsgUnit,	// 매 MsgUnit메시지마다 알려준다. 
#ifdef __gmTLS__
	LPCTSTR	pszTargetName,	// InitializeSecurityContext에서 사용한다.
#endif	// __gmTLS__

	PINT	lpgmStatus,

	BOOL	bUseConsole
): gmTCPBase(
	lpConnV,
	nConnC,
#ifdef __gmUSE_PUBLIC_QUEUE__
	PubSize,
#endif	// __gmUSE_PUBLIC_QUEUE__
	MaxTCPMsgSize,
	dwMsgUnit,		// 매 MsgUnit메시지마다 알려준다. 

	bUseConsole
)
{
	if (m__baseStatus != 0) {
		if (lpgmStatus)
			*lpgmStatus = m__baseStatus;
		return;
	}

	INT	iReturnVal	= 0;

	// 2005/3/24 (EST) - ghkim
	mf__SetUID(0);		// 서버는 0으로 초기화한다.

#ifdef __gmTLS__
	// 2005/4/5 (EST) - ghkim
	m__pszTargetName	= pszTargetName;
	// InitializeSecurityContext(클라이언트), AcceptSecurityContext(서버)에
	// 사용할 Credential핸들을 얻는다.
	DWORD	dwTLSLastErrorCode;
	if (mf__AcquireCredentialsHandle(&dwTLSLastErrorCode) != SEC_E_OK) {
		iReturnVal = -1;
		goto NEXT;
	}
#endif	// __gmTLS__

#ifdef __gmIOCP__
	m__hIOCP			= NULL;
#else	// __gmIOCP__
	m__hEventManagerToWorker	= NULL;
#endif	// !__gmIOCP__
	m__hWorkerThread		= NULL;

#ifdef __gmIOCP__
	m__hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (!m__hIOCP) {
#ifdef _DEBUG
		mf__SystemErrorMessage(
			GetLastError(),
			TEXT("gmTCPClient::gmTCPClient: CreateIoCompletionPort(INVALID_HANDLE_VALUE)")
		);
#endif	// _DEBUG
		iReturnVal = -1;
		goto NEXT;
	}
#else	// __gmIOCP__
{
	UINT	MaxConn	= 0;
	for (UINT globalIndex = 0; gIndex < nConnC; ++gIndex)
		MaxConn += lpConnV[globalIndex].MaxConn;

	// MAXIMUM_WAIT_OBJECTS = 64
	// MAXIMUM_WAIT_OBJECTS / 2 - 2 = 30 
	if (MaxConn > (MAXIMUM_WAIT_OBJECTS / 2 - 2)) {
		mf__fprintf(m__CONERR,
			TEXT("# of clients = %u > Max. # of clients(%u)\n")
			,
			MaxConn,
			MAXIMUM_WAIT_OBJECTS / 2 - 2
		);
		iReturnVal = -1;
		goto NEXT;
	}

	if ((m__hEventManagerToWorker = CreateEvent(NULL, FALSE, FALSE, NULL)) == NULL) {
		iReturnVal = -1;
		goto NEXT;
	}
}
#endif	// !__gmIOCP__

	// 준비
	UINT	iTotalIndex, globalIndex, myMaxConn;
	for (
		iTotalIndex = 0, globalIndex = 0, myMaxConn = 0;
		globalIndex < nConnC;
		++globalIndex, iTotalIndex += myMaxConn
	) {
		myMaxConn = lpConnV[globalIndex].MaxConn;
		for (UINT index = 0; index < myMaxConn; ++index) {
			gmPseudoTCP&	gmhTCP = m__lpgmPseudoTCP[iTotalIndex + index];
////////////////////////////////////////////////////////////////////////////////
		SOCKET	sock = mf__CreateConnectSocket();
		if (sock == INVALID_SOCKET) {
			iReturnVal = -1;
			goto NEXT;
		}
		gmhTCP.m__sock = sock;

		memset(&gmhTCP.m__addr, 0, sizeof(struct sockaddr_in));
		gmhTCP.m__addr.sin_family		= AF_INET;
////////////////////////////////////////////////////////////////////////////////
#if 1
		struct hostent*	lpHostEnt;
{
		USES_CONVERSION;
		lpHostEnt = gethostbyname(T2CA(lpConnV[globalIndex].TCPAddr));
}
		if (!lpHostEnt) {
			iReturnVal = -1;
			goto NEXT;
		}
		gmhTCP.m__addr.sin_addr.s_addr	= *((u_long*) lpHostEnt->h_addr_list[0]);
#else
		gmhTCP.m__addr.sin_addr.s_addr	= inet_addr(lpgmConn[globalIndex].TCPAddr);
#endif
////////////////////////////////////////////////////////////////////////////////
		gmhTCP.m__addr.sin_port		= htons(lpConnV[globalIndex].TCPPort);
		if (connect(gmhTCP.m__sock, (LPSOCKADDR) &gmhTCP.m__addr, sizeof(struct sockaddr_in)) == SOCKET_ERROR) {
			m__WSALastErrorCode = WSAGetLastError();
#ifdef _DEBUG
			_TCHAR	Message[512];
			DWORD	BufferLength0 = 512;	// NULL문자를 포함한 Message버퍼의 크기(문자 단위)
			(VOID) mf__GetWSAErrorStringFromCode(Message, &BufferLength0, m__WSALastErrorCode);
			mf__fprintf(m__CONERR,
				TEXT("gmTCPClient::gmTCPClient: connect() failed!: WSALastErrorCode = %d(%s)\n")
				,
				m__WSALastErrorCode,
				Message
			);
#endif	// _DEBUG
			iReturnVal = -1;
			goto NEXT;
		} else {
			gmhTCP.mf__SetPseudoTCPStatus(gmPseudoTCP::gmPseudoTCPStatusEstablished);
#ifdef __gmIOCP__
#else	// __gmIOCP__
			if (!(gmhTCP.m__lpRecv->m__WSAOverlappedPlus.mWSAOverlapped.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL))) {
				iReturnVal = -1;
				goto NEXT;
			}
			if (!(gmhTCP.m__lpSend->m__WSAOverlappedPlus.mWSAOverlapped.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL))) {
				iReturnVal = -1;
				goto NEXT;
			}
#endif	// !__gmIOCP__
		}
		++m__NConn;

#ifdef __gmIOCP__
		if (!CreateIoCompletionPort(
			(HANDLE) gmhTCP.m__sock,
			m__hIOCP,
			(ULONG_PTR) &gmhTCP,	// (ULONG_PTR) i,
			0
		)) {
#ifdef _DEBUG
			mf__SystemErrorMessage(
				GetLastError(),
				TEXT("gmTCPClient::gmTCPClient: HANDLE CreateIoCompletionPort([%u]) failed!")
				,
				index
			);
#endif	// _DEBUG
			iReturnVal = -1;
			goto NEXT;
		}
#endif	// __gmIOCP__
		if (gmhTCP.mf__IOCPRecv() < 0) {
			iReturnVal = -1;
			goto NEXT;
		}

		gmhTCP.m__dwMilliSeconds = GetTickCount();
////////////////////////////////////////////////////////////////////////////////
		}
	}

	// Windows 2000 이상에서는 CreateThread의 제일 마지막 인자가
	// NULL이어도 상관없지만, Windows 98/98SE/Me(적어도 Me)에서는
	// NULL이면 안된다.
	DWORD	dwThreadID;
	if ((m__hWorkerThread = CreateThread(NULL, 0, ClientSideWorkerThreadProc, this, 0, &dwThreadID)) == NULL) {
#ifdef _DEBUG
		mf__SystemErrorMessage(
			GetLastError(),
			TEXT("gmTCPClient::gmTCPClient: HANDLE CreateThread(..., IOCPWorkerThread, ...) failed!")
		);
#endif	// _DEBUG
		iReturnVal = -1;
		goto NEXT;
	}

NEXT:
	if (iReturnVal == -1) {
		if (m__lpgmPseudoTCP) {
////////////////////////////////////////////////////////////////////////////////
			UINT	iTotalIndex, globalIndex, myMaxConn;
			for (
				iTotalIndex = 0, globalIndex = 0, myMaxConn = 0;
				globalIndex < nConnC;
				++globalIndex, iTotalIndex += myMaxConn
			) {
				myMaxConn = lpConnV[globalIndex].MaxConn;
				for (UINT index = 0; index < myMaxConn; ++index) {
					gmPseudoTCP&	gmhTCP = m__lpgmPseudoTCP[iTotalIndex + index];
// Worker쓰레드가 동작하지 않으므로,
// 단지 해당 소켓을 닫는 것으로 충분하다.
				if (gmhTCP.m__sock == INVALID_SOCKET)
					continue;
				closesocket(gmhTCP.m__sock);	// 리턴값은 무시
				gmhTCP.m__sock = INVALID_SOCKET;
				}
			}
////////////////////////////////////////////////////////////////////////////////
		}

#ifdef __gmIOCP__
		if (m__hIOCP) {
			CloseHandle(m__hIOCP);		// 리턴값은 무시
			m__hIOCP = NULL;
		}
#else	// __gmIOCP__
		if (m__hEventManagerToWorker != NULL) {
			CloseHandle(m__hEventManagerToWorker);		// 리턴값은 무시
			m__hEventManagerToWorker = NULL;
		}
#endif	// !__gmIOCP__

		if (m__hWorkerThread != NULL) {
			CloseHandle(m__hWorkerThread);	// 리턴값은 무시
			m__hWorkerThread = NULL;
		}
	}

	if (lpgmStatus)
		*lpgmStatus = iReturnVal;
}

gmTCPClient::~gmTCPClient()
{
	if (m__lpgmPseudoTCP) {
		INT	iReturnVal = mf__close();
#ifdef _DEBUG
		mf__printf(
			TEXT("gmTCPClient::mf__close(): 0x%08X\n")
			,
			iReturnVal
		);
#endif	// _DEBUG
	}

#ifdef __gmIOCP__
	if (m__hIOCP) {
		CloseHandle(m__hIOCP);		// 리턴값은 무시
		m__hIOCP = NULL;
	}
#else	// __gmIOCP__
	if (m__hEventManagerToWorker != NULL) {
		CloseHandle(m__hEventManagerToWorker);		// 리턴값은 무시
		m__hEventManagerToWorker = NULL;
	}
#endif	// !__gmIOCP__

	if (m__hWorkerThread != NULL) {
		CloseHandle(m__hWorkerThread);	// 리턴값은 무시
		m__hWorkerThread = NULL;
	}
}

SOCKET	gmTCPClient::mf__CreateConnectSocket(void)
{
	INT	iReturnVal = 0;

	SOCKET	mySock = INVALID_SOCKET;
	INT	iBufSize;

	mySock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_IP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (mySock == INVALID_SOCKET) {
		iReturnVal = -1;
		goto LAST;
	}

	iBufSize = 0;
	if (setsockopt(
		mySock,
		SOL_SOCKET, SO_SNDBUF,
		(char *) &iBufSize, sizeof(iBufSize)
	) == SOCKET_ERROR) {
		iReturnVal = -1;
		goto LAST;
	}

LAST:
	if (iReturnVal != 0) {
		if (mySock != INVALID_SOCKET)
			closesocket(mySock);	// 리턴값은 무시
		mySock = INVALID_SOCKET;
	}

	return mySock;
}

#ifdef __gmIOCP__
INT
gmTCPClient::mf__IOCPWork(
	VOID
)
{
	BOOL	bStatus;
	DWORD	dwIOSize;	// IOCP를 통하여 전송된 테이터의 크기
	gmPseudoTCP	*gmhTCP;
	LPWSAOVERLAPPEDPLUS	lpWSAOverlappedPlus;	// IOCP에 사용된 OVERLAPPED구조체에 대한 포인터
	while (1) {
		bStatus = GetQueuedCompletionStatus(
				m__hIOCP,				// HANDLE		CompletionPort
				&dwIOSize,				// LPDWORD		lpNumberOfBytes
				(PULONG_PTR) &gmhTCP,			// (PULONG_PTR) &index,			// PULONG_PTR		lpCompletionKey
				(LPOVERLAPPED*) &lpWSAOverlappedPlus,	// LPOVERLAPPED*	lpOverlapped
				INFINITE				// DWORD		dwMilliseconds
		);

////////////////////////////////////////////////////////////////////////////////
		if ((DWORD) lpWSAOverlappedPlus == gmCOMMAND_CHANNEL) {
			// 쓰레드관리자로부터의 명령이 도착했다.
			if (dwIOSize == gmSUBCOMMAND_TERM) {
#ifdef _DEBUG
				mf__fprintf(m__CONERR,
					TEXT("dwIOSize = 0x%08X, dwCompletionKey = 0x%08X\n")
					,
					dwIOSize,
					gmhTCP
				);
#endif	// _DEBUG
				ExitThread((DWORD) gmhTCP);
			}
			if (
				dwIOSize == gmSUBCOMMAND_NONBLOCK
					||
				dwIOSize == gmSUBCOMMAND_BLOCK
			)
				gmhTCP->mf__closeEnd(dwIOSize);
			else {
				// 2003/10/30
				// 이럴 수가 있나?
				// 로그를 남기고 무시한다.
				mf__fprintf(m__CONERR,
					TEXT("gmTCPClient::mf__IOCPWork: ")
					TEXT("lpWSAOverlappedPlus = 0x%04X, but ")
					TEXT("dwIOSize != 0x%04X && dwIOSize != 0x%04X\n")
					,
					gmCOMMAND_CHANNEL,
					gmSUBCOMMAND_NONBLOCK,
					gmSUBCOMMAND_BLOCK
				);
			}

			continue;
		}
////////////////////////////////////////////////////////////////////////////////

#if 0
////////////////////////////////////////////////////////////////////////
	mf__fprintf(m__CONERR,
		TEXT("--------------------------------\n")
		TEXT("GetQueuedCompletionStatus: %s\n")
		TEXT("\tlpCompletionKey = 0x%08X\n")
		TEXT("\tdwIOSize = %u\n")
		TEXT("\tlpOverlapped = 0x%08X\n")
		TEXT("\t\tmIOOpCode = %s\n")
		TEXT("\t\t%s = %u\n")
		TEXT("--------------------------------\n")
		,
		bStatus ? TEXT("TRUE") : TEXT("FALSE"),
		gmhTCP,
		dwIOSize,
		lpWSAOverlappedPlus,
		lpWSAOverlappedPlus->mIOOpCode == IOOP_RD ? TEXT("IOOP_RD") : TEXT("IOOP_WR"),
		lpWSAOverlappedPlus->mIOOpCode == IOOP_RD ? TEXT("# of bytes to be read") : TEXT("# of bytes to be written"),
		lpWSAOverlappedPlus->u.mBytesToBeRead
	);
////////////////////////////////////////////////////////////////////////
#endif

////////////////////////////////////////////////////////////////////////
		if (!bStatus) {		// 문제발생
#ifdef _DEBUG
			mf__fprintf(m__CONERR,
				TEXT("BOOL GetQueuedCompletionStatus(): FALSE!\n")
			);
#endif	// _DEBUG
		}
/*
 * If *lpOverlapped is NULL and the function does not dequeue a completion
 * packet from the completion port, the return value is zero.
 * The function does not store information in the variables pointed to
 * by the lpNumberOfBytes and lpCompletionKey parameters.
 * To get extended error information, call GetLastError.
 * If the function did not dequeue a completion packet because the wait timed out,
 * GetLastError returns WAIT_TIMEOUT.
 */
		if (!bStatus && !lpWSAOverlappedPlus) {
#ifdef _DEBUG
			DWORD	dwLastError = GetLastError();
			mf__SystemErrorMessage(
				dwLastError,
				TEXT("!bStatus && !lpWSAOverlappedPlus")
			);
			mf__fprintf(m__CONERR,
				TEXT("gmTCPClient::mf__IOCPWork: !bStatus && !lpOverlappedPlus: ignored!\n")
			);
#endif	// _DEBUG
			// 무시한다. 단, 로그는 남기고.
			continue;
		}

/*
 * If *lpOverlapped is not NULL and the function dequeues a completion
 * packet for a failed I/O operation from the completion port,
 * the return value is zero. The function stores information in the variables
 * pointed to by lpNumberOfBytes, lpCompletionKey, and lpOverlapped.
 * To get extended error information, call GetLastError.
 */
		if (!bStatus && lpWSAOverlappedPlus) {
#ifdef _DEBUG
			mf__SystemErrorMessage(
				GetLastError(),
				TEXT("!bStatus && lpWSAOverlappedPlus")
			);
#endif	// _DEBUG
			// 서버와의 연결이 "비정상적으로" 끊어졌다.
#ifdef _DEBUG
			mf__fprintf(m__CONERR,
				TEXT("[C#%u(0x%08X)] 서버와의 연결이 \"비 정상적으로\" 끊어졌다.\n")
				,
				gmhTCP->m__localIndex,
				gmhTCP
			);
#endif	// _DEBUG
			gmhTCP->mf__closeRecv();
			continue;
		}

/*
 * If a socket handle associated with a completion port is closed,
 * GetQueuedCompletionStatus returns ERROR_SUCCESS, with lpNumberOfBytes equal zero.
 */
		if (bStatus == ERROR_SUCCESS && !dwIOSize) {
			mf__fprintf(m__CONERR,
				TEXT("gmTCPClient::mf__IOCPWork: ")
				TEXT("bStatus == ERROR_SUCCESS && !dwIOSize: ")
				TEXT("Ignored!\n")
			);
			// 무시한다. 단, 로그는 남기고.
			continue;
		}
////////////////////////////////////////////////////////////////////////

		// bStatus = TRUE!

		if (!dwIOSize) {
			// 서버와의 연결이 "정상적으로" 끊어졌다.
#ifdef _DEBUG
			mf__fprintf(m__CONERR,
				TEXT("[C#%u(%08X)] 서버와의 연결이 \"정상적으로\" 끊어졌다.\n")
				,
				gmhTCP->m__localIndex,
				gmhTCP
			);
#endif	// _DEBUG
			gmhTCP->mf__closeRecv();
			continue;
		}

		UINT	myIOOpCode = lpWSAOverlappedPlus->mIOOpCode;
		switch (myIOOpCode) {
		case IOOP_RD:
			if (gmhTCP->mf__IOOP_RD(dwIOSize) == -1) {
				gmhTCP->mf__closeRecv();
			}
			break;
		case IOOP_WR:
			gmhTCP->mf__IOOP_WR(dwIOSize);
			break;
		default:
			// 이럴수야 없지!
			// 무시하고 지나간다. 단, 로그는 남기고...
			mf__fprintf(m__CONERR,
				TEXT("gmTCPClient::mf__IOCPWork: ")
				TEXT("Unknown lpWSAOverlappedPlus->mIOOpCode: %u: ")
				TEXT("Ignored!\n")
				,
				myIOOpCode
			);
			break;
		}
	}

	return 0;
}
#else	// __gmIOCP__
INT
gmTCPClient::mf__EventWork(
	VOID
)
{
	HANDLE	hEvents[MAXIMUM_WAIT_OBJECTS];
//	UINT	dwIndices[MAXIMUM_WAIT_OBJECTS];
	gmHTCP	gmTCPHandles[MAXIMUM_WAIT_OBJECTS];
//	UINT	dwIndex, dwI;
	UINT	dwI;
	DWORD	dwEvents;
	DWORD	dwWaitResult;

	gmPseudoTCP*	gmhTCP;
	BOOL	bWSAOverlappedResult;
	LPWSAOVERLAPPED	lpWSAOverlapped;
	DWORD	dwIOSize;
	DWORD	dwFlags;
	while (TRUE) {
		hEvents[0] = m__hEventManagerToWorker;
		mf__lock();
		UINT	iTotalIndex, globalIndex, myMaxConn;
		for (
			dwI = 0, dwEvents = 1,
			iTotalIndex = 0, globalIndex = 0, myMaxConn = 0;
			globalIndex < m__nConnC;
			++globalIndex, iTotalIndex += myMaxConn
		) {
			myMaxConn = m__lpConnV[globalIndex].MaxConn;
			for (UINT index = 0; index < myMaxConn; ++index) {
				gmPseudoTCP&	gmhTCP = m__lpgmPseudoTCP[iTotalIndex + index];
////////////////////////////////////////////////////////////////////////////////
//		for (dwI = 0, dwEvents = 1, dwIndex = 0; dwIndex < m__ParamTCP.maxClients; ++dwIndex) {
			if (!(gmhTCP.mf__GetPseudoTCPStatus() & gmPseudoTCP::gmPseudoTCPStatusEstablished))
				continue;

			gmTCPHandles[dwI] = &gmhTCP;
			hEvents[1 + 2 * dwI + 0] = gmhTCP.m__lpRecv->m__WSAOverlappedPlus.mWSAOverlapped.hEvent;
			hEvents[1 + 2 * dwI + 1] = gmhTCP.m__lpSend->m__WSAOverlappedPlus.mWSAOverlapped.hEvent;
			dwEvents += 2;
			++dwI;
//		}
			}
		}
////////////////////////////////////////////////////////////////////////////////
		mf__unlock();

		dwWaitResult = WaitForMultipleObjects(
			dwEvents,
			hEvents,
			FALSE,
			INFINITE
		);

		if (
			WAIT_OBJECT_0 <= dwWaitResult
				&&
			dwWaitResult < WAIT_OBJECT_0 + dwEvents
		) {
			// 아래에서...
		} else {
			mf__fprintf(m__CONERR,
				TEXT("gmTCPClient::mf__EventWork: ")
				TEXT("WaitForMultipleObjects(): ")
				TEXT("!(WAIT_OBJECT_0 <= ... < WAIT_OBJECT_0 + dwEvents): ")
				TEXT("Ignored!\n")
			);
			// WAIT_FAILED도 처리해야 하는데...
			continue;
		}

		// WAIT_OBJECT_0 <= ... < WAIT_OBJECT_0 + dwEvents
		// 적절한 이벤트가 발생했다.
		dwI = dwWaitResult - WAIT_OBJECT_0;

		if (dwI == 0) {
			// m__hEventManagerToWorker
			mf__fprintf(m__CONERR,
				TEXT("gmTCPClient::mf__EventWork: ")
				TEXT("m__hEventManagerToWorker가 발생!: ")
				TEXT("쓰레드를 종료한다.\n")
			);
			ExitThread(gmSUBCOMMAND_TERM);
		}

		// dwI != 0
//		dwIndex = dwIndices[(dwI - 1) / 2];
//		gmhTCP = &m__lpgmPseudoTCP[dwIndex];
		gmhTCP = (gmPseudoTCP*) gmTCPHandles[(dwI - 1) / 2];

		// gmTCPBase::mf__flushAll에서 강제로 클라이언트와의 접속을
		// 끊기위해서 gmTCPBase::mf__closeRecv를 호출할 때
		// m__lpRecv, m__lpSend값이 NULL일 수 있다. - ghkim(2003/6/11)
		if (!gmhTCP->m__lpRecv || !gmhTCP->m__lpSend) {
			mf__fprintf(m__CONERR,
				TEXT("!gmhTCP->m__lpRecv || !gmhTCP->m__lpSend\n")
			);
			gmhTCP->mf__closeRecv();
			continue;
		}
		// 유효한 m__lpRecv, m__lpSend이다.
		lpWSAOverlapped = (dwI - 1) % 2 == 0
			? (LPWSAOVERLAPPED) &gmhTCP->m__lpRecv->m__WSAOverlappedPlus
			: (LPWSAOVERLAPPED) &gmhTCP->m__lpSend->m__WSAOverlappedPlus
		;
		bWSAOverlappedResult = WSAGetOverlappedResult(
			gmhTCP->m__sock,
			lpWSAOverlapped,
			&dwIOSize,
			FALSE,
			&dwFlags
		);
		if (!bWSAOverlappedResult) {
			m__WSALastErrorCode = WSAGetLastError();
#ifdef _DEBUG
			_TCHAR	Message[512];
			DWORD	BufferLength0 = 512;	// NULL문자를 포함한 Message버퍼의 크기(문자 단위)
			mf__GetWSAErrorStringFromCode(Message, &BufferLength0, m__WSALastErrorCode);
			mf__fprintf(m__CONERR,
				TEXT("WSAGetOverlappedResult() failed: %s(%u)\n")
				,
				Message,
				m__WSALastErrorCode
			);
#endif	// _DEBUG
			if (m__WSALastErrorCode != WSAECONNRESET) {
				mf__fprintf(m__CONERR,
					TEXT("WSALastError() != WSAECONNRESET\n")
				);
			}
			// m__WSALastError = WSAECONNRESET
			// 서버와의 연결이 "비정상적으로" 끊어졌다.
#ifdef _DEBUG
			mf__fprintf(m__CONERR,
				TEXT("[C#%u(0x%08X): %s] 서버와의 연결이 \"비 정상적으로\" 끊어졌다.\n")
				,
				gmhTCP->m__localIndex,
				gmhTCP,
				(dwI - 1) % 2 ? "WSASend(?)" : "WSARecv"
			);
#endif	// _DEBUG
			gmhTCP->mf__closeRecv();
			continue;
		}

		if (!dwIOSize) {
			// 서버와의 연결이 "정상적으로" 끊어졌다.
#ifdef _DEBUG
			mf__fprintf(m__CONERR,
				TEXT("[C#%u(%08X): %s] 서버와의 연결이 \"정상적으로\" 끊어졌다.\n")
				,
				gmhTCP->m__localIndex,
				gmhTCP,
				(dwI - 1) % 2 ? "WSASend(?)" : "WSARecv"
			);
#endif	// _DEBUG
			gmhTCP->mf__closeRecv();
			continue;
		}

		if ((dwI - 1) % 2 == 0) {
			// WSARecv
			if (gmhTCP->mf__IOOP_RD(dwIOSize) == -1) {
				gmhTCP->mf__closeRecv();
			}
		} else {	// WSASend
			gmhTCP->mf__IOOP_WR(dwIOSize);
		}
	}
}
#endif	// !__gmIOCP__

INT
gmTCPClient::mf__close(
	VOID
)
{
	INT	iReturnVal;

#ifdef __gmIOCP__
{
	UINT	iTotalIndex, globalIndex, myMaxConn;
	for (
		iTotalIndex = 0, globalIndex = 0, myMaxConn = 0;
		globalIndex < m__nConnC;
		++globalIndex, iTotalIndex += myMaxConn
	) {
		myMaxConn = m__lpConnV[globalIndex].MaxConn;
		for (UINT index = 0; index < myMaxConn; ++index) {
			gmPseudoTCP&	gmhTCP = m__lpgmPseudoTCP[iTotalIndex + index];
////////////////////////////////////////////////////////////////////////////////
			if (gmhTCP.mf__disconnect() < 0) {	// blocking
				// 2003/10/30
				// 끊을 때에 문제가 생긴다면...
				mf__fprintf(m__CONERR,
					TEXT("ERROR: ")
					TEXT("gmTCPClient::mf__close: ")
					TEXT("gmPseudoTCP::mf__disconnect failed: ")
					TEXT("특정 연결(%u.%u)을 끊을 수 없다.\n")
					,
					gmhTCP.m__globalIndex,
					gmhTCP.m__localIndex
				);
			}
////////////////////////////////////////////////////////////////////////////////
		}
	}
}
#endif	// __gmIOCP__

	if (
#ifdef __gmIOCP__
		PostQueuedCompletionStatus(
		m__hIOCP,
		gmSUBCOMMAND_TERM,		// DWORD	dwNumberOfBytesTransferred
		(ULONG_PTR) 0x0101,		// ULONG_PTR	dwCompletionKey
		(LPOVERLAPPED) gmCOMMAND_CHANNEL// LPOVERLAPPED	lpOverlapped
	)
#else	// __gmIOCP__
		SetEvent(m__hEventManagerToWorker)
#endif	// !__gmIOCP__
	) {
		DWORD	dwWaitResult;
		if ((dwWaitResult = WaitForSingleObject(m__hWorkerThread, INFINITE)) != WAIT_OBJECT_0) {
#ifdef _DEBUG
			mf__SystemErrorMessage(
				GetLastError(),
				TEXT("gmTCPClient::mf__close: WaitForSingleObject(m__hThread) fails!\n")
			);
#endif	// _DEBUG
			iReturnVal = -1;
		} else {	// 정상적으로 Worker 쓰레드를 종료 
			// 뒷마무리...
			GetExitCodeThread(m__hWorkerThread, (LPDWORD) &iReturnVal);
		}
	} else
		iReturnVal = -1;

	return iReturnVal;
}

#ifdef __gmTLS__
// InitializeSecurityContext에서 사용할 Credential에 대한 핸들을 얻는다.
//	[註] AcquireCredentialsHandle은 Windows 98에서 사용할 수 없다.
// 성공이면 SEC_E_OK를 리턴하고, 실패하면 다른 값을 리턴한다.
SECURITY_STATUS
gmTCPClient::mf__AcquireCredentialsHandle(
	PDWORD	pdwLastErrorCode	// [OUT]	! 클라이언트: 사용하지 않는다.
)
{
	// AcquireCredentialsHandle에 사용될
	// SChannel-specific data인 SCHANNEL_CRED구조체를 정의한다.
/*
	typedef struct _SCHANNEL_CRED {
=		DWORD		dwVersion;			! Set to SCHANNEL_CRED_VERSION.
=		DWORD		cCreds;				! Number of structures in the paCred array.
=		PCCERT_CONTEXT	*paCred;			! Array of pointers to CERT_CONTEXT structures. Each pointer specifies a certificate containing a private key to be used in authenticating the application. Typically, this array contains one structure for each key exchange method supported by the application.
		HCERTSTORE	hRootStore;			! Optional. Valid for server applications only. Handle to a certificate store containing self-signed root certificates for certification authorities (CAs) trusted by the application. This member is used only by server-side applications requiring client authentication.
		DWORD		cMappers;			! Reserved.
		struct _HMAPPER	**aphMappers;			! Reserved.
+		DWORD		cSupportedAlgs;			! Number of algorithms in the palgSupportedAlgs array.
+		ALG_ID		*palgSupportedAlgs;		! Optional. Pointer to an array of ALG_ID representing the algorithms supported by connections made with credentials acquired using this structure. If cSupportedAlgs is zero or palgSupportedAlgs is NULL, Schannel uses the system defaults.		CALG_RSA_KEYX, CALG_DH_EPHEM
+		DWORD		grbitEnabledProtocols;		! Optional. DWORD containing a bit string representing the protocols supported by connections made with credentials acquired using this structure.	SP_PROT_TLS1_CLIENT(: Transport Layer Security 1.0 client-side.), SP_PROT_TLS1_SERVER(Transport Layer Security 1.0 server-side.), ...
%		DWORD		dwMinimumCipherStrength;	! Minimum bulk encryption cipher strength allowed for connections, in bits.	If this member is zero, Schannel uses the system default. If this member is -1, the SSL3/TLS MAC-only cipher suites (also known as NULL cipher) are enabled.
		DWORD		dwMaximumCipherStrength;	! Maximum bulk encryption cipher strength allowed for connections, in bits.	If this member is zero, Schannel uses the system default.
		DWORD		dwSessionLifespan;		! The maximum life span of credentials acquired using this structure.
+		DWORD		dwFlags;			! Contains bit flags that control the behavior of Schannel in Windows 2000 and Windows XP. Can be zero or a combination of the following values.	SCH_CRED_NO_DEFAULT_CREDS(: ...), SCH_CRED_MANUAL_CRED_VALIDATION(: ...), ...
		DWORD		reserved;			! Reserved. Must be zero.
	} SCHANNEL_CRED, *PSCHANNEL_CRED;
 *	=: Required
 *	+: Optional but...
 *	%: 패킷암호화를 비활성화할 수 있다. 활성화하기 위해서는 Renegotiate를 이용해야 한다.(?)
 */
	// SCHANNEL_CRED구조체의 대부분은 0이므로 먼저 0으로 채운다.
	// 즉 대부분은 기본값을 사용한다.
	SCHANNEL_CRED	SChannelCred		= {0};
	SChannelCred.dwVersion			= SCHANNEL_CRED_VERSION;
	SChannelCred.grbitEnabledProtocols	= SP_PROT_TLS1_CLIENT;	// SP_PROT_TLS1 = SP_PROT_TLS1_SERVER | SP_PROT_TLS1_CLIENT
	// SCHANNEL_CRED.dwFlags는 Windows 2000이나 Windows XP에서만 사용할 수 있다.
	// 이 중에서 SCH_CRED_MANUAL_CRED_VALIDATION옵션은,
	// 클라이언트에서 서버가 보낸 인증서의 신뢰성을 "신뢰된 루트 인증 기관"을 통해서
	// 자동적으로 하지 말 것을 지정한다.
	// "인증서.TXT"에 이에 관한 자세히 설명이 있다.
//	SChannelCred.dwFlags |= SCH_CRED_MANUAL_CRED_VALIDATION;	// SCH_CRED_AUTO_CRED_VALIDATION(기본값)

#ifdef UNICODE
	SECURITY_STATUS	SecurityStatus = m__pSFT->AcquireCredentialsHandleW(
		NULL,			//	SEC_CHAR	*pszPrincipal	! [IN] This parameter is not used with the Schannel SSP and should be set to NULL.
		UNISP_NAME_W,		//	SEC_CHAR	*pszPackage	! [IN] Set to WDIGEST_SP_NAME for the Digest SSP and to UNISP_NAME for the Schannel SSP.
		SECPKG_CRED_OUTBOUND,
					//	ULONG		fCredentialUse	! [IN]
		NULL,			//	PLUID		pvLogonID	! [IN] This parameter is not used with the Schannel SSP and should be set to NULL.
		&SChannelCred,		//	PVOID		pAuthData	! [IN] When using the Schannel SSP in the Windows 2000, Windows XP, or Windows Server 2003 family of operating systems, specify an SCHANNEL_CRED structure that indicates the protocol to use and the settings for various customizable channel features.
		NULL,			//	SEC_GET_KEY_FN	pGetKeyFn	! [IN] This parameter is not used with the Schannel SSP and should be set to NULL.
		NULL,			//	PVOID		pvGetKeyArgument	! [IN] This parameter is not used with the Digest SSP or the Schannel SSP and should be set to NULL.
		&m__hCredential,	//	PCredHandle	phCredential	! [OUT] Pointer to a CredHandle structure to receive the credential handle.
		NULL			//	PTimeStamp	ptsExpiry	! [OUT] This parameter is optional with the Schannel SSP.
	);
#else	// UNICODE
	SECURITY_STATUS	SecurityStatus = m__pSFT->AcquireCredentialsHandleA(
		NULL,			//	SEC_CHAR	*pszPrincipal	! [IN] This parameter is not used with the Schannel SSP and should be set to NULL.
		UNISP_NAME_A,		//	SEC_CHAR	*pszPackage	! [IN] Set to WDIGEST_SP_NAME for the Digest SSP and to UNISP_NAME for the Schannel SSP.
		SECPKG_CRED_OUTBOUND,
					//	ULONG		fCredentialUse	! [IN]
		NULL,			//	PLUID		pvLogonID	! [IN] This parameter is not used with the Schannel SSP and should be set to NULL.
		&SChannelCred,		//	PVOID		pAuthData	! [IN] When using the Schannel SSP in the Windows 2000, Windows XP, or Windows Server 2003 family of operating systems, specify an SCHANNEL_CRED structure that indicates the protocol to use and the settings for various customizable channel features.
		NULL,			//	SEC_GET_KEY_FN	pGetKeyFn	! [IN] This parameter is not used with the Schannel SSP and should be set to NULL.
		NULL,			//	PVOID		pvGetKeyArgument	! [IN] This parameter is not used with the Digest SSP or the Schannel SSP and should be set to NULL.
		&m__hCredential,	//	PCredHandle	phCredential	! [OUT] Pointer to a CredHandle structure to receive the credential handle.
		NULL			//	PTimeStamp	ptsExpiry	! [OUT] This parameter is optional with the Schannel SSP.
	);
#endif	// !UNICODE
	//	SEC_E_UNKNOWN_CREDENTIALS        0x8009030DL

	m__bValidCredential = SecurityStatus == SEC_E_OK ? TRUE : FALSE;

	return SecurityStatus;
}
#endif	// __gmTLS__
