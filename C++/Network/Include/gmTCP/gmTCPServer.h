#ifndef __gmTCP_SERVER__
#define __gmTCP_SERVER__
////////////////////////////////////////////////////////////////////////////////


#include <MSWSock.h>

// 2005/4/5 (EST) - ghkim
#ifdef __gmTLS__
#include <WinCrypt.h>
#endif	// __gmTLS__

class gmTCPServer: public gmTCPBase
{
private:
	HANDLE	m__hIOCP;
	// 쓰레드에서 호출한다.
	INT	mf__IOCPWork(VOID);
	friend DWORD	WINAPI ServerSideWorkerThreadProc(LPVOID lpVoid);

private:
	HANDLE	m__hWorkerThread;
	INT	mf__close(VOID);

public:
	gmTCPServer(
		gmCONN*	lpConnV,
		UINT	nConnC,
#ifdef __gmUSE_PUBLIC_QUEUE__
		UINT	PubSize,
#endif	// __gmUSE_PUBLIC_QUEUE__
		UINT	MaxTCPMsgSize,
		DWORD	dwMsgUnit,	// 매 dwMsgUnit메시지마다 알려준다.
#ifdef __gmTLS__
		LPCTSTR	pszSubject,	// 인증서를 찾을 때 사용한다.
#endif	// __gmTLS__

		PINT	lpgmStatus	= NULL,

		BOOL	bUseConsole	= TRUE
	);
	~gmTCPServer();


////////////////////////////////////////////////////////////////////////
	INT	mf__HeartBeat(VOID);
	VOID	mf__setStatusEstablished(gmHTCP gmhTCP) {
		((gmPseudoTCP*) gmhTCP)->mf__SetPseudoTCPStatusEstablished();
	}
////////////////////////////////////////////////////////////////////////
	// Obsolete!
	// Use gmPseudoTCP::mf__GetID(), gmPseudoTCP::mf__SetID(ID) as
	//	ID = gmhTCP->mf__GetID();
	//	gmhTCP->mf__SetID(ID);
	DWORD	mf__TCPGetCharID(gmHTCP gmhTCP) {
		return ((gmPseudoTCP*) gmhTCP)->mf__GetID();
	}
	VOID	mf__setID(gmHTCP gmhTCP, UINT ID) {
		((gmPseudoTCP *) gmhTCP)->mf__SetID(ID);
	}
////////////////////////////////////////////////////////////////////////

	ULONG	mf__getUAddress(gmHTCP gmhTCP) {
		return ((gmPseudoTCP*) gmhTCP)->m__addr.sin_addr.s_addr;
	}

	USHORT	mf__getUPort (gmHTCP gmhTCP) {
		return ntohs(((gmPseudoTCP*) gmhTCP)->m__addr.sin_port);
	}
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
private:
	gmAcceptEx	**m__lppgmAcceptEx;
public:
	HANDLE	mf__GetIOCPHandle(VOID) {return m__hIOCP;}
////////////////////////////////////////////////////////////////////////

// 2005/4/5 (EST) - ghkim
#ifdef __gmTLS__
private:
	LPCTSTR		m__pszSubject;	// 인증서를 찾을 때 사용

private:
	// 서버인증에 사용할 인증서를 가져온다.
	// 인증서를 가져올 저장고는 szSubsystemProtocol(예, CA, MY, ROOT, ...)이고,
	// 인증서의 Subject는 pszSubject가 가리키는 문자열을 포함해야 한다.
	// [註] 이 인증서에는 개인키가 포함되어있어야 한다.
	PCCERT_CONTEXT	mf__GetCertificateContext(LPCTSTR pszSubsystemProtocol, LPCTSTR pszSubject);

	SECURITY_STATUS	mf__AcquireCredentialsHandle(PDWORD pdwLastErrorCode);
#endif	// __gmTLS__
};


////////////////////////////////////////////////////////////////////////////////
#endif	// __gmTCP_SERVER__
