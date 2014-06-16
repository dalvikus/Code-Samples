#include <gmTCP/gmTCP.h>

gmAcceptEx::gmAcceptEx(
	gmTCPBase*	lpTCPBase,
	UINT	globalIndex,

	PINT	lmgmStatus
)
{
	INT	iReturnVal = 0;

// 0.
	m__lpTCPBase	= lpTCPBase;
	m__hIOCP	= ((gmTCPServer*) lpTCPBase)->mf__GetIOCPHandle();

	m__globalIndex	= globalIndex;

	m__ListenSocket	= INVALID_SOCKET;
	DWORD	dwBytesReturned;
	GUID	guidAcceptEx = WSAID_ACCEPTEX;
	GUID	guidGetAcceptExSockaddrs = WSAID_GETACCEPTEXSOCKADDRS;

// 1.
	if ((m__ListenSocket = mf__CreateListenSocket()) == INVALID_SOCKET) {
		iReturnVal = -1;
		goto EXIT;
	}

	struct sockaddr_in	ServAddr;
	memset(&ServAddr, 0, sizeof(ServAddr));
	ServAddr.sin_family		= AF_INET;
	ServAddr.sin_addr.s_addr	= htonl(INADDR_ANY);
	gmCONN	*lpConnV;
	lpConnV = m__lpTCPBase->mf__GetLpConnV();
	ServAddr.sin_port		= htons(lpConnV[m__globalIndex].TCPPort);
	if (bind(
		m__ListenSocket,
		(LPSOCKADDR) &ServAddr,
		sizeof(ServAddr)
	) == SOCKET_ERROR) {
		iReturnVal = -1;
		goto EXIT;
	}

	if (listen(
		m__ListenSocket,
		5
	) == SOCKET_ERROR) {
		iReturnVal = -1;
		goto EXIT;
	}

// 2.
	if (!CreateIoCompletionPort(
		(HANDLE) m__ListenSocket,
		m__hIOCP,
		(ULONG_PTR) this,	// (ULONG_PTR) -1,
		0
	)) {
		iReturnVal = -1;
		goto EXIT;
	}

// 3.
	// Load the AcceptEx extension function from the provider for this socket
	if (WSAIoctl(
		m__ListenSocket,
		SIO_GET_EXTENSION_FUNCTION_POINTER,
		&guidAcceptEx,
		sizeof(guidAcceptEx),
		&m__lpFnAcceptEx,
		sizeof(m__lpFnAcceptEx),
		&dwBytesReturned,
		NULL,
		NULL
	) == SOCKET_ERROR) {
		iReturnVal = -1;
		goto EXIT;
	}
	// Load the GetAcceptExSockaddrs extension function from the provider for this socket
	if (WSAIoctl(
		m__ListenSocket,
		SIO_GET_EXTENSION_FUNCTION_POINTER,
		&guidGetAcceptExSockaddrs,
		sizeof(guidGetAcceptExSockaddrs),
		&m__lpFnGetAcceptExSockaddrs,
		sizeof(m__lpFnGetAcceptExSockaddrs),
		&dwBytesReturned,
		NULL,
		NULL
	) == SOCKET_ERROR) {
		iReturnVal = -1;
		goto EXIT;
	}

	if (mf__PostAcceptEx() != 0) {
		iReturnVal = -1;
		goto EXIT;
	}

EXIT:
	if (iReturnVal != 0) {
		mf__CleanUp();
	}
	if (lmgmStatus)
		*lmgmStatus = iReturnVal;
}

gmAcceptEx::~gmAcceptEx()
{
	mf__CleanUp();
}

VOID
gmAcceptEx::mf__CleanUp(
	VOID
)
{
	if (m__ListenSocket != INVALID_SOCKET) {
		closesocket(m__ListenSocket);	// 리턴값은 무시!
		m__ListenSocket = INVALID_SOCKET;
	}
}

SOCKET
gmAcceptEx::mf__CreateListenSocket(
	VOID
)
{
	SOCKET	mySock = WSASocket(
		AF_INET,
		SOCK_STREAM,
		IPPROTO_IP,
		NULL,
		0,
		WSA_FLAG_OVERLAPPED
	);
	if (mySock == INVALID_SOCKET)
		return INVALID_SOCKET;

	// mySock != INVALID_SOCKET
	// 소켓옵션을 설정하려면 여기서...

	return mySock;
}

INT
gmAcceptEx::mf__PostAcceptEx(
	VOID
)
{
	INT	iReturnVal = 0;

	// mOverlapped 멤버의 초기화를 잊지말자!
	// 그렇지 않으면 WSANOTSOCK에러가 발생!
	m__AcceptExOverlappedPlus.mWSAOverlapped.Internal	= 0;
	m__AcceptExOverlappedPlus.mWSAOverlapped.InternalHigh	= 0;
	m__AcceptExOverlappedPlus.mWSAOverlapped.Offset		= 0;
	m__AcceptExOverlappedPlus.mWSAOverlapped.OffsetHigh	= 0;
	m__AcceptExOverlappedPlus.mWSAOverlapped.hEvent		= NULL;
	// GetQueuedCompletionStatus에서 리턴되는 LPOVERLAPPED에 대한 주소를
	// (실제로는 OVERLAPPEDPLUS의 주소를) 적음으로서,
	// OVERLAPPEDPLUS의 멤버인 mIOOpCode의 값(이 경우에는 IOOP_ACCEPT)이
	// 무언인지를 판단할 수 있다.
	m__AcceptExOverlappedPlus.mIOOpCode		= gmTCPServer::IOOP_ACCEPT;
	// u.mAcceptExIndex는 이용되지 않는다.
	// 이용할 수 있는 무엇이 있을까? 아직은...
	m__AcceptExOverlappedPlus.u.mAcceptExIndex	= 0;

	if ((m__AcceptExSocket = mf__CreateAcceptExSocket()) == INVALID_SOCKET) {
		// 2003/10/30
		// 메시지 출력
		m__lpTCPBase->mf__fprintf(m__lpTCPBase->m__CONERR,
			TEXT("gmAcceptEx::mf__PostAcceptEx: ")
			TEXT("gmAcceptEx::mf__CreateAcceptExSocket failed!\n")
		);
		iReturnVal = -1;
		goto NEXT;
	}

	INT	WSALastError;
	BOOL	bAcceptEx;
	DWORD	dwBytesReceived;
	if ((bAcceptEx = m__lpFnAcceptEx(
		m__ListenSocket,
		m__AcceptExSocket,
		(LPVOID) m__AcceptExBuffer,
		0,
		sizeof(SOCKADDR_STORAGE) + 16,
		sizeof(SOCKADDR_STORAGE) + 16,
		&dwBytesReceived, 
		(LPOVERLAPPED) &m__AcceptExOverlappedPlus
	)) == FALSE && (WSALastError = WSAGetLastError()) != ERROR_IO_PENDING) {
		m__lpTCPBase->mf__SystemErrorMessage(
			WSALastError,
			TEXT("gmAcceptEx::mf__PostAcceptEx")
		);
		iReturnVal = -1;
		goto NEXT;
	}

NEXT:
	if (iReturnVal != 0) {
	}

	return iReturnVal;
}

SOCKET
gmAcceptEx::mf__CreateAcceptExSocket(
	VOID
)
{
	SOCKET	mySock = WSASocket(
		AF_INET,
		SOCK_STREAM,
		IPPROTO_IP,
		NULL,
		0,
		WSA_FLAG_OVERLAPPED
	);
	if (mySock == INVALID_SOCKET) {
		INT	WSALastErrorCode = WSAGetLastError();
#ifdef _DEBUG
		// 2003/10/30
		// 메시지 출력
		_TCHAR	Message[512];
		DWORD	BufferLength0 = 512;	// NULL문자를 포함한 Message버퍼의 크기(문자 단위)
		(VOID) m__lpTCPBase->mf__GetWSAErrorStringFromCode(Message, &BufferLength0, WSALastErrorCode);
		m__lpTCPBase->mf__fprintf(m__lpTCPBase->m__CONERR,
			TEXT("gmAcceptEx::mf__CreateAcceptExSocket: WSASocket failed!: ")
			TEXT("WSAGetLastError(): %d(%s)\n")
			,
			WSALastErrorCode,
			Message
		);
#endif	// _DEBUG
		m__lpTCPBase->mf__SetWSALastErrorCode(WSALastErrorCode);

		return INVALID_SOCKET;
	}

	// mySock != INVALID_SOCKET
	// 소켓옵션을 설정하려면 여기서...
	INT	bReUseAddr = TRUE;
	if (setsockopt(
		mySock,
		SOL_SOCKET,
		SO_REUSEADDR,
		(PCHAR) &bReUseAddr,
		sizeof(bReUseAddr)
	) == SOCKET_ERROR) {
		INT	WSALastErrorCode = WSAGetLastError();
#ifdef _DEBUG
		// 2003/10/30
		// 메시지 출력
		TCHAR	Message[512];
		DWORD	BufferLength0 = 512;	// NULL문자를 포함한 Message버퍼의 크기(문자 단위)
		(VOID) m__lpTCPBase->mf__GetWSAErrorStringFromCode(Message, &BufferLength0, WSALastErrorCode);
		m__lpTCPBase->mf__fprintf(m__lpTCPBase->m__CONERR,
			TEXT("gmAcceptEx::mf__CreateAcceptExSocket: WSASocket failed!: ")
			TEXT("WSAGetLastError(): %d(%s)\n")
			,
			WSALastErrorCode,
			Message
		);
#endif	// _DEBUG
		m__lpTCPBase->mf__SetWSALastErrorCode(WSALastErrorCode);

		(VOID) closesocket(mySock);	// 리턴값은 무시!

		return INVALID_SOCKET;
	}

	return mySock;
}

gmPseudoTCP*
gmAcceptEx::mf__IsConnectionAccepted(
	VOID
)
{
	gmPseudoTCP*	gmhPseudoTCP = NULL;

	// AcceptEx의 설명에 따라 listening socket의 성질을 물려받자.
	// (그런데 어떤 성질을?)
	if (setsockopt(
		m__AcceptExSocket, 
		SOL_SOCKET,
		SO_UPDATE_ACCEPT_CONTEXT,
		(PCHAR) &m__ListenSocket,
		sizeof(m__ListenSocket)
	) == SOCKET_ERROR) {
		gmhPseudoTCP = NULL;
		goto NEXT;
	}

	// Local address(?)와 Remote address를 얻는다.
	struct sockaddr_in*	lpLocalAddr;
	struct sockaddr_in*	lpRemoteAddr;
	INT	LocalAddrLen, RemoteAddrLen;
	m__lpFnGetAcceptExSockaddrs(
		m__AcceptExBuffer,
		0,		// m__lpFnAcceptEx에서 설정한 값과 같아야 한다!
		sizeof(SOCKADDR_STORAGE) + 16,
		sizeof(SOCKADDR_STORAGE) + 16,
		(LPSOCKADDR*) &lpLocalAddr, &LocalAddrLen,
		(LPSOCKADDR*) &lpRemoteAddr, &RemoteAddrLen
	);
#if 0
	mf__fprintf(m__CONERR,
		TEXT("다음 연결이 시도되었다.\n")
		TEXT("\tLocal: %s, %d\n")
		TEXT("\tRemote: %s, %d\n")
		TEXT(".....%s\n")
		,
		inet_ntoa(lpLocalAddr->sin_addr),
		ntohs(lpLocalAddr->sin_port),
		inet_ntoa(lpRemoteAddr->sin_addr),
		ntohs(lpRemoteAddr->sin_port),
		index == m__ParamTCP.maxClients
			? TEXT("사용할 수 있는 소켓이 없어 연결을 끊는다.")
			: TEXT("연결이 이루어졌다.")
	);
#endif

	UINT	iTotalIndex, globalIndex, myMaxConn;
	gmCONN	*lpConnV;
	lpConnV = m__lpTCPBase->mf__GetLpConnV();
	for (
		iTotalIndex = 0, globalIndex = 0, myMaxConn = 0;
		globalIndex < m__globalIndex;
		++globalIndex, iTotalIndex += myMaxConn
	) {
		myMaxConn = lpConnV[globalIndex].MaxConn;
	}
	UINT	index;
	gmTCPServer	*lpTCPServer;
	lpTCPServer	= (gmTCPServer*) m__lpTCPBase;
	gmPseudoTCP	*lpgmPseudoTCP;
	lpgmPseudoTCP	= m__lpTCPBase->mf__GetLpPseudoTCP();
	for (index = 0; index < lpConnV[m__globalIndex].MaxConn; ++index) {
		if (lpgmPseudoTCP[iTotalIndex + index].mf__GetPseudoTCPStatus() & gmPseudoTCP::gmPseudoTCPSTATUS__NOT_CONNECTED)
			break;
	}
	if (index == lpConnV[m__globalIndex].MaxConn) {
		// 더 이상 연결을 만들 수 없다.
		// 방금 전의 연결을 끊는다!
		closesocket(m__AcceptExSocket);	// 리턴값은 무시
		gmhPseudoTCP = NULL;
		goto NEXT;
	}
	gmhPseudoTCP = &lpgmPseudoTCP[iTotalIndex + index];

	gmhPseudoTCP->m__sock = m__AcceptExSocket;
	gmhPseudoTCP->m__addr	= *lpRemoteAddr;
	gmhPseudoTCP->m__dwMilliSeconds	= GetTickCount();
	gmhPseudoTCP->mf__SetPseudoTCPStatus(gmPseudoTCP::gmPseudoTCPStatusAccepted);

	gmhPseudoTCP->mf__Reset();
	DWORD	dwUID;
	dwUID = m__lpTCPBase->mf__GetUID();
	++dwUID;
	if (!dwUID) {
		// 0xFFFFFFFF개의 연결이 있었다고? 놀랍다!
		// 서버인 경우에는 0이 될 수 없다.
		dwUID	= 1;
	}
	m__lpTCPBase->mf__SetUID(dwUID);

NEXT:
	if (gmhPseudoTCP) {
	} else {
	}

	return gmhPseudoTCP;
}
