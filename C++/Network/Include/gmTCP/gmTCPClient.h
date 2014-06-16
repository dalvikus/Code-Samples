#ifndef __gmTCP_CLIENT__
#define __gmTCP_CLIENT__
////////////////////////////////////////////////////////////////////////////////


class gmTCPClient: public gmTCPBase
{
private:
#ifdef __gmIOCP__
	HANDLE	m__hIOCP;
	INT	mf__IOCPWork(VOID);
#else	// __gmIOCP__
	HANDLE	m__hEventManagerToWorker;
	INT	mf__EventWork(VOID);
#endif	// !__gmIOCP__
	friend DWORD	WINAPI ClientSideWorkerThreadProc(LPVOID lpVoid);

private:
	SOCKET	mf__CreateConnectSocket(VOID);
	HANDLE	m__hWorkerThread;
	// 서버와의 연결을 끊을 때
	INT	mf__close(VOID);

public:
	gmTCPClient(
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

		PINT	lpgmStatus	= NULL,

		BOOL	bUseConsole	= TRUE
	);
	~gmTCPClient();

	UINT	mf__ConnectionN(VOID) {
		mf__lock();
		UINT	myNConn = m__NConn;
		mf__unlock();
		return myNConn;
	}

// 2005/4/5 (EST) - ghkim
#ifdef __gmTLS__
private:
	// InitializeSecurityContext에서 사용한다.
	// 계속 사용하므로 이 포인터는 계속 유효해야 한다.
	LPCTSTR		m__pszTargetName;
public:
	LPCTSTR		mf__GetPTargetName(void)	{return m__pszTargetName;}

private:
	SECURITY_STATUS	mf__AcquireCredentialsHandle(PDWORD pdwLastErrorCode);
#endif	// __gmTLS__
};


////////////////////////////////////////////////////////////////////////////////
#endif	// __gmTCP_CLIENT__
