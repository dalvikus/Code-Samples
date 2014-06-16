#include <gmTCP/gmTCP.h>
#ifdef __gmTLS__
#include <schannel.h>	// SCHANNEL_CRED
#endif	// __gmTLS__

DWORD	WINAPI ServerSideWorkerThreadProc(LPVOID lpVoid)
{
	return ((gmTCPServer *) lpVoid)->mf__IOCPWork();
}

gmTCPServer::gmTCPServer(
	gmCONN*	lpConnV,
	UINT	nConnC,
#ifdef __gmUSE_PUBLIC_QUEUE__
	UINT	PubSize,
#endif	// __gmUSE_PUBLIC_QUEUE__
	UINT	MaxTCPMsgSize,
	DWORD	dwMsgUnit,	// 매 dwMsgUnit메시지마다 알려준다.
#ifdef __gmTLS__
	LPCTSTR	pszSubject,	// 서버 인증에 사용할 인증서를 찾을 때 사용한다.
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
	dwMsgUnit,		// 매 dwMsgUnit메시지마다 알려준다. 

	bUseConsole
)
{
	if (m__baseStatus != 0) {
		if (lpgmStatus)
			*lpgmStatus = m__baseStatus;
		return;
	}

	// 2005/3/24 (EST) - ghkim
	mf__SetUID(1);		// 클러이언트는 0으로 초기화한다.

	m__hIOCP		= NULL;
	m__hWorkerThread	= NULL;

	INT	iReturnVal	= 0;
	INT	bi = 1;

#ifdef __gmTLS__
	// 2005/4/5 (EST) - ghkim
	// AcceptSecurityContext에 사용할 Credential핸들을 얻는다.
	m__pszSubject	= pszSubject;
	DWORD	dwTLSLastErrorCode;
	if (mf__AcquireCredentialsHandle(&dwTLSLastErrorCode) != SEC_E_OK) {
		iReturnVal = -1;
		goto NEXT;
	}
#endif	// __gmTLS__

	m__hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (!m__hIOCP) {
		// GetLastError
		mf__OutputDebugString(TEXT("gmTCPServer::gmTCPServer: HANDLE CreateIoCompletionPort(...): NULL: 0x%08X\n"), GetLastError());
		iReturnVal = -1;
		goto NEXT;
	}

	m__lppgmAcceptEx = new gmAcceptEx*[nConnC];
	if (!m__lppgmAcceptEx) {
		iReturnVal = -1;
		goto NEXT;
	}
	INT	gmStatus;
	UINT	globalIndex;
	for (globalIndex = 0; globalIndex < nConnC; ++globalIndex) {
		m__lppgmAcceptEx[globalIndex] = new gmAcceptEx((gmTCPBase*) this, globalIndex, &gmStatus);
		if (!m__lppgmAcceptEx[globalIndex] || gmStatus != 0) {
			iReturnVal = -1;
			goto NEXT;
		}
	}

	if ((m__hWorkerThread = CreateThread(NULL, 0, ServerSideWorkerThreadProc, this, 0, NULL)) == NULL) {
		mf__OutputDebugString(TEXT("gmTCPServer::gmTCPServer: HANDLE CreateThread(..., IOCPWorkerThread, ...): NULL: 0x%08X\n"), GetLastError());
		iReturnVal = -1;
		goto NEXT;
	}

NEXT:
	if (iReturnVal == -1) {
		if (m__hIOCP) {
			CloseHandle(m__hIOCP);		// 리턴값은 무시
			m__hIOCP = NULL;
		}

		if (m__hWorkerThread != NULL) {
			CloseHandle(m__hWorkerThread);	// 리턴값은 무시
			m__hWorkerThread = NULL;
		}
	}

	if (lpgmStatus)
		*lpgmStatus = iReturnVal;
}

gmTCPServer::~gmTCPServer()
{
	if (m__lpgmPseudoTCP != NULL) {
		INT	iReturnVal = mf__close();
	}

	if (m__hIOCP) {
		CloseHandle(m__hIOCP);		// 리턴값은 무시
		m__hIOCP = NULL;
	}

	if (m__hWorkerThread != NULL) {
		CloseHandle(m__hWorkerThread);	// 리턴값은 무시
		m__hWorkerThread = NULL;
	}
}

INT
gmTCPServer::mf__IOCPWork(
	VOID
)
{
	BOOL	bStatus;
	DWORD	dwIOSize;	// IOCP를 통하여 전송된 테이터의 크기
	LPWSAOVERLAPPEDPLUS	lpWSAOverlappedPlus;	// IOCP에 사용된 OVERLAPPED구조체에 대한 포인터
	UINT	IOOpCode;
	gmPseudoTCP	*gmhTCP;
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
					TEXT("gmTCPServer::mf__IOCPWork: ")
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

		// 다른 쓰레드에서 lpWSAOverlappedPlus에 해당하는
		// 메모리를 해제했다면 이 값은 더 이상 의미가 없다.
		// 예를 들어, mf__disconnect가 그런 함수이다.
		// 이와같이 "예외처리"를 하든지 아니면,
		// 바뀐 mf__close처럼 mf__disconnect를 사용하지 말아야 한다.
////////////////////////////////////////////////////////////////////////////////
		__try {
			IOOpCode = lpWSAOverlappedPlus->mIOOpCode;
			// 이 값이 IOOP_ACCEPT, IOOP_RD, IOOP_WR중의 하나라는 보장이 없다.
			// 메모리가 해지되는 중간에 얻을 수도 있기 때문이다.
		} __except (1) {
			// 2003/10/30
			// gmTrans클래스를 동적으로 관리하지 않으므로,
			// 위와 같은 문제는 발생하지 않아야한다.
			// 이를 확인한다.
			mf__fprintf(m__CONERR,
				TEXT("gmTCPServer::mf__IOCPWork: __try, __except...\n")
			);
			__SUICIDE__;	// SEVERE
			IOOpCode = IOOP_INVLD;
		}
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
#if 0
	mf__fprintf(m__CONERR,
		TEXT(">>>> T/F:= %s, Handle:= 0x%08X(0x%08X), %s:= %u(%d) <<<<\n")
		,
		bStatus ? TEXT("T") : TEXT("F"),
		gmhTCP,
		lpWSAOverlappedPlus,
		IOOpCode == IOOP_INVLD
			? TEXT("IOOP_INVLD")
			: IOOpCode == IOOP_ACCEPT
				? TEXT("IOOP_ACCEPT")
				: IOOpCode == IOOP_RD
					? TEXT("IOOP_RD")
					: IOOpCode == IOOP_WR
						? TEXT("IOOP_WR")
						: TEXT("IOOP_XXX"),
		dwIOSize,
		IOOpCode == IOOP_INVLD
			? -1
			: IOOpCode == IOOP_ACCEPT || IOOpCode == IOOP_RD || IOOpCode == IOOP_WR
				? lpWSAOverlappedPlus->u.mAcceptExIndex
				: -2
	);
#endif	// _DEBUG
////////////////////////////////////////////////////////////////////////

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
			mf__SystemErrorMessage(
				GetLastError(),
				TEXT("!bStatus && !lpWSAOverlappedPlus")
			);
			mf__fprintf(m__CONERR,
				TEXT("gmTCPServer::mf__IOCPWork: ")
				TEXT("!bStatus && !lpOverlappedPlus: ")
				TEXT("Ignored!\n")
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
			switch (IOOpCode) {
			case IOOP_ACCEPT:
				(VOID) SetEvent(m__hEventWorkerToManager);
				break;
			case IOOP_RD:
			case IOOP_WR:
				// IOOP_RD, IOOP_WR
				// 클라이언트와의 연결이 "비정상적으로" 끊어졌다.
#ifdef _DEBUG
				mf__fprintf(m__CONERR,
					TEXT("%u.%u(0x%08X) 번째 클라이언트와의 연결이 \"비 정상적으로\" 끊어졌다.\n")
					,
					gmhTCP->m__globalIndex,
					gmhTCP->m__localIndex,
					gmhTCP
				);
#endif	// _DEBUG
				gmhTCP->mf__closeRecv();
				break;
			case IOOP_INVLD:
				//break;
			default:
				mf__fprintf(m__CONERR,
					TEXT("IOOpCode = %s: ")
					TEXT("누군가 강제로 클라이언트와의 연결을 끊었다.\n")
					,
					IOOpCode == IOOP_INVLD
						? "IOOP_INVLD"
						: "IOOP_XXX"
				);
				break;
			}

			continue;
		}

/*
 * If a socket handle associated with a completion port is closed,
 * GetQueuedCompletionStatus returns ERROR_SUCCESS,
 * with lpNumberOfBytes equal zero.
 */
		if (bStatus == ERROR_SUCCESS && !dwIOSize) {
			mf__fprintf(m__CONERR,
				TEXT("gmTCPServer::mf__IOCPWork: ")
				TEXT("bStatus == ERROR_SUCCESS && !dwIOSize: ")
				TEXT("Ignored!\n")
			);
			// 무시한다. 단, 로그는 남기고.
			continue;
		}
////////////////////////////////////////////////////////////////////////

		// bStatus = TRUE!

//		if (IOOpCode != IOOP_ACCEPT) {
//			if (!bSuccess || (bSuccess && (0 == dwIoSize))) {
//				......
//				continue;
//			}
//		}
		if (!dwIOSize && IOOpCode != IOOP_ACCEPT) {
			if (IOOpCode != IOOP_RD && IOOpCode != IOOP_WR) {
				mf__fprintf(m__CONERR,
					TEXT("!dwIOSize && IOOpCode != IOOP_ACCEPT: IOOpCode = %s\n")
					,
					IOOpCode == IOOP_INVLD
						? "IOOP_INVLD"
						: "IOOP_XXX"
				);
				continue;
			}
			// 클라이언트와의 연결이 "정상적으로" 끊어졌다.
#ifdef _DEBUG
			mf__fprintf(m__CONERR,
				TEXT("%u.%u(0x%08X) 번째 클라이언트와의 연결이 \"정상적으로\" 끊어졌다.\n")
				,
				gmhTCP->m__globalIndex,
				gmhTCP->m__localIndex,
				gmhTCP
			);
#endif	// _DEBUG
			gmhTCP->mf__closeRecv();
			continue;
		}

		switch (IOOpCode) {
		case IOOP_ACCEPT:
////////////////////////////////////////////////////////////////////////
{
	gmAcceptEx&	gmhAcceptEx = *((gmAcceptEx*) gmhTCP);
	gmPseudoTCP	*gmhPseudoTCP = gmhAcceptEx.mf__IsConnectionAccepted();
	if (gmhPseudoTCP) {
		if (!CreateIoCompletionPort(
			(HANDLE) gmhPseudoTCP->m__sock,
			m__hIOCP,
			(ULONG_PTR) gmhPseudoTCP,
			0
		)) {
			// 이 연결을 "정상적으로" 끊는다.
			gmhPseudoTCP->mf__closeRecv();
			break;
		}

		if (gmhPseudoTCP->mf__IOCPRecv() == gmPseudoTCP::MSGTCP_IOCPRECV_WSARECV_ERROR) {
			// 이 연결을 "정상적으로" 끊는다.
			gmhPseudoTCP->mf__closeRecv();
			break;
		}
	}

	// 다른 연결을 기다리자...
	if (gmhAcceptEx.mf__PostAcceptEx() != 0) {
		// 더 이상 다른 연결을 받아들일 수 없다!
		mf__fprintf(m__CONERR,
			TEXT("SEVERE: ")
			TEXT("gmTCPServer::gmTCPServer: ")
			TEXT("mf__PostAcceptEx failed: ")
			TEXT("다음 연결을 받아들일 수 없다!\n")
		);
	}
}
////////////////////////////////////////////////////////////////////////
			break;
		case IOOP_RD:
			if (gmhTCP->mf__IOOP_RD(dwIOSize) == -1) {
				gmhTCP->mf__closeRecv();
			}
			break;
		case IOOP_WR:
			gmhTCP->mf__IOOP_WR(dwIOSize);
			break;
		case IOOP_INVLD:
			//break;
		default:
			mf__fprintf(m__CONERR,
				TEXT("IOOpCode = %s\n")
				,
				IOOpCode == IOOP_INVLD
					? "IOOP_INVLD"
					: "IOOP_XXX"
			);
			break;
		}
	}

	return 0;
}

INT
gmTCPServer::mf__close(
	VOID
)
{
	INT	iReturnVal = 0;

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
					TEXT("gmTCPServer::mf__close: ")
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

#if 1
	for (UINT globalIndex = 0; globalIndex < m__nConnC; ++globalIndex) {
		closesocket(m__lppgmAcceptEx[globalIndex]->mf__GetListenSocket());	// 리턴값은 무시!
		if (WaitForSingleObject(m__hEventWorkerToManager, 4096) == WAIT_OBJECT_0) {
			delete m__lppgmAcceptEx[globalIndex];
			m__lppgmAcceptEx[globalIndex] = NULL;
		}
	}
	delete [] m__lppgmAcceptEx;
	m__lppgmAcceptEx = NULL;
#endif

	if (PostQueuedCompletionStatus(
		m__hIOCP,
		gmSUBCOMMAND_TERM,		// DWORD	dwNumberOfBytesTransferred
		(ULONG_PTR) 0x0101,		// ULONG_PTR	dwCompletionKey
		(LPOVERLAPPED) gmCOMMAND_CHANNEL	// LPOVERLAPPED	lpOverlapped
	)) {
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

INT
gmTCPServer::mf__HeartBeat(
	VOID
)
{
	INT	iReturnVal = 0;

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
		if (gmhTCP.mf__GetPseudoTCPStatus() & gmPseudoTCP::gmPseudoTCPSTATUS__NOT_CONNECTED)
			continue;

		DWORD	dwTickCount = GetTickCount();
		if (dwTickCount - gmhTCP.m__dwMilliSeconds > gmCONNECTION_TIMEOUT) {
			if (gmhTCP.mf__disconnect() < 0) {	// blocking
				// 2003/10/30
				// 끊을 때에 문제가 생긴다면...
				mf__fprintf(m__CONERR,
					TEXT("ERROR: ")
					TEXT("gmTCPServer::mf__HeartBeat: ")
					TEXT("gmPseudoTCP::mf__disconnect failed: ")
					TEXT("특정 연결(%u.%u)을 끊을 수 없다.\n")
					,
					gmhTCP.m__globalIndex,
					gmhTCP.m__localIndex
				);
			}
		}
////////////////////////////////////////////////////////////////////////////////
		}
	}

	return iReturnVal;
}


// 2005/4/5 (EST) - ghkim
#ifdef __gmTLS__
/*
 * 서버인증에 사용할 인증서를 가져온다.
 * 서버인증에 사용할 인증서를 가져온다.
 * 인증서를 가져올 저장고는 szSubsystemProtocol(예, CA, MY, ROOT, ...)이고,
 * 인증서의 Subject는 szSubject를 포함해야 한다.
 * "인증서.TXT"에 이러한 인증서를 만들고,
 * 이를 관리하는 방법과 예가 있다.
 * [註] 이 인증서에는 개인키가 포함되어있어야 한다.
 */
PCCERT_CONTEXT
gmTCPServer::mf__GetCertificateContext(
	LPCTSTR	szSubsystemProtocol,
	LPCTSTR	szSubject
)
{
	m__TLSLastErrorCode	= 0;

	HCERTSTORE	hCertStore = NULL;
#if 1
	if (!(hCertStore = CertOpenStore(
#if 1
		// CERT_STORE_PROV_SYSTEM	CERT_STORE_PROV_SYSTEM_W
		// CERT_STORE_PROV_SYSTEM은 UNICODE용으로 정의되어 있지 않다.
#ifdef UNICODE
		CERT_STORE_PROV_SYSTEM_W,
#else	// UNICODE
		CERT_STORE_PROV_SYSTEM_A,
#endif	// !UNICODE
#else
		CERT_STORE_PROV_SYSTEM
#endif
		0,
		NULL,
#if 0
		CERT_SYSTEM_STORE_LOCAL_MACHINE,
#else
		CERT_SYSTEM_STORE_CURRENT_USER,
#endif
		szSubsystemProtocol
	))) {
		m__TLSLastErrorCode = GetLastError();
		return NULL;
	}
#else
	if (!(hCertStore = CertOpenSystemStore(
		NULL,		// Use default CSP. Otherwise call CryptAcquireContext.
		szSubsystemProtocol
	))) {
		m__TLSLastErrorCode = GetLastError();
		return NULL;
	}
#endif

	PCCERT_CONTEXT	pCertContext = CertFindCertificateInStore(
		hCertStore,
		X509_ASN_ENCODING,
		0,
		// CERT_FIND_SUBJECT_STR	CERT_FIND_SUBJECT_STR_W
		// CERT_FIND_SUBJECT_STR은 UNICODE용으로만 정의되어있다.
#ifdef UNICODE
		CERT_FIND_SUBJECT_STR_W,	// Searches for a certificate containing the specified subject name string.
#else	// UNICODE
		CERT_FIND_SUBJECT_STR_A,	// Searches for a certificate containing the specified subject name string.
#endif	// !UNICODE
		szSubject,
		NULL				// Pointer to the last CERT_CONTEXT structure returned by this function. This parameter must be NULL on the first call of the function. A pPrevCertContext parameter that is not NULL is always freed by this function, even if the function causes an error.
	);
	if (!pCertContext) {
		m__TLSLastErrorCode = GetLastError();
		//	CRYPT_E_NOT_FOUND	0x80092004L
		return NULL;
	}

	DWORD	dwLastErrorCode = 0;
	if (hCertStore) {
#if 0
		// 강제로 해당메모리도 해제하도록 한다.
		// -> 이렇게 하면 안된다. 왜?
		//	CertFindCertificateInStore에서 얻은 인증서에 대한 포인터를
		//	이 함수가 리턴한 이후에도 계속 사용할 것이므로,
		//	CertOpenSystemStore에서 얻은 hCertStore를 강제로 해제하면 안된다.
		if (CertCloseStore(hCertStore, CERT_CLOSE_STORE_FORCE_FLAG) == FALSE) {
			dwLastErrorCode = GetLastError();
			// 어떤 문제가 있다.
		}
#else
#if 1
		// 계속 사용되는 메모리가 있는 지 확인한다.
		// -> CertFindCertificateInStore에서 인증서에 대한 포인터를 얻었으므로 당연하다.
		if (CertCloseStore(hCertStore, CERT_CLOSE_STORE_CHECK_FLAG) == FALSE) {
			dwLastErrorCode = GetLastError();
			if (dwLastErrorCode == CRYPT_E_PENDING_CLOSE) {
				// 어딘가에서 사용되고 있다.
				// 따라서 해당메모리는 그대로 남아있을 것이다.

				// 당연하다!
				// pCertContext에서 사용하고 있다.
				// 메모리를 해제하면 더 이상 pCertContext는 의미가 없다.
				dwLastErrorCode = 0;
			} else {
				// 다른 문제가 있다.
			}
		}
#else
		// 계속 사용되는 메모리가 있는 지에 대한 관심이 없다.
		if (CertCloseStore(hCertStore, 0) == FALSE) {
			// 어떤 문제가 있다.
			dwLastErrorCode = GetLastError();
		}
#endif
#endif
	}
	if (dwLastErrorCode && pCertContext) {
		// 어떤 문제가 있었으므로 pCertContext도 사용하지 말자.
		(void) CertFreeCertificateContext(pCertContext);	// SDK에 따르면 이 함수는 항상 TRUE를 리턴한다.
		m__TLSLastErrorCode = dwLastErrorCode;
		return NULL;
	}

	return pCertContext;
}


// AcceptSecurityContext에서 사용할 Credential에 대한 핸들을 얻는다.
//	[註] AcquireCredentialsHandle은 Windows 98에서 사용할 수 없다.
// 성공이면 SEC_E_OK를 리턴하고, 실패하면 다른 값을 리턴한다.
// 리턴값이 -1이면 인증서를 찾지 못한 경우이고,
// 이 때에는 m__TLSLastErrorCode를 참고해서 이유를 알아볼 수 있다.
SECURITY_STATUS
gmTCPServer::mf__AcquireCredentialsHandle(
	PDWORD	pdwLastErrorCode	// [OUT]	! 서버:인증서를 얻을 문제가 생긴 경우, GetLastError의 결과
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
	SChannelCred.grbitEnabledProtocols	= SP_PROT_TLS1_SERVER;
	// 서버의 경우에는 서버인증에 사용할 인증서를 명시해야 한다.
	PCCERT_CONTEXT	pCertContext = mf__GetCertificateContext(TEXT("MY"), m__pszSubject);
	if (!pCertContext) {
		// 해당하는 인증서를 얻는 데 문제가 있다.
		// 0이 아닌 m__dwLastErrorCode를 참고한다.
		*pdwLastErrorCode = m__TLSLastErrorCode;
		return (SECURITY_STATUS) -1;
	}
	SChannelCred.cCreds = 1;
	SChannelCred.paCred = &pCertContext;

#ifdef UNICODE
	SECURITY_STATUS	SecurityStatus = m__pSFT->AcquireCredentialsHandleW(
		NULL,			//	SEC_CHAR	*pszPrincipal	! [IN] This parameter is not used with the Schannel SSP and should be set to NULL.
		UNISP_NAME_W,		//	SEC_CHAR	*pszPackage	! [IN] Set to WDIGEST_SP_NAME for the Digest SSP and to UNISP_NAME for the Schannel SSP.
		SECPKG_CRED_INBOUND,
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
		SECPKG_CRED_INBOUND,
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

	// SCHANNEL_CRED.paCred을 만들 때 사용했던,
	// mf__GetCertificateContext에서 만든 pCertContex와 관련된 메모리를 해제하자.
	(void) CertFreeCertificateContext(*SChannelCred.paCred);	// SDK에 따르면 이 함수는 항상 TRUE를 리턴한다.

	m__bValidCredential = SecurityStatus == SEC_E_OK ? TRUE : FALSE;

	return SecurityStatus;
}
#endif	// __gmTLS__
