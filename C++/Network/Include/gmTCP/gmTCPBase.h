#ifndef __gmTCP_BASE__
#define __gmTCP_BASE__
////////////////////////////////////////////////////////////////////////////////



#ifdef __gmUSE_PUBLIC_QUEUE__
#else	// __gmUSE_PUBLIC_QUEUE__
class	gmPseudoTCP;
#endif	// __gmUSE_PUBLIC_QUEUE__

// 2005/4/5 (EST) - ghkim
#ifdef __gmTLS__
// Sspi.h를 #include하기 전에 다음을 정의해야 한다.
// 그렇지 않으면
//	You must define one of SECURITY_WIN32, SECURITY_KERNEL, or
// 라는 메시지가 뜨고 컴파일이 되지 않는다.
// SDK메뉴얼에는 이것에 대한 언급이 없는 데, 예제들에는 정의되어있다.
// Sspi.h의 첫 부분에 이를 확인하는 다음과 같은 부분이 있다.
/*
#ifndef ISSP_LEVEL
#error  You must define one of SECURITY_WIN32, SECURITY_KERNEL, or
#error  SECURITY_MAC 
#endif // !ISSP_LEVEL
 */
#define SECURITY_WIN32
#include <Sspi.h>
#endif	// __gmTLS__

#ifdef __gmCRYPT__
#include <WinCrypt.h>
#endif	// __gmCRYPT__

class gmTCPBase : public myPRINT
{
////////////////////////////////////////////////////////////////////////
// 2003/10/23	friend를 써서 application프로그램에서 호출되는,
//		public으로 선언된 함수의 갯수를 최소화했다.
	friend class	gmPseudoTCP;
	// mf__notify, mf__SetWSALastErrorCode, m__hEventWorkerToManager
	// 2003/10/30
	// gmAcceptEx클래스에서도 mf__SetWSALastErrorCode를
	// 호출할 수 있도록 friend에 등록
	friend class	gmAcceptEx;
	// mf__SetWSALastErrorCode
////////////////////////////////////////////////////////////////////////


public:
	enum IOOP {
		IOOP_INVLD,		// Invalid
//#ifdef gmTCP_SERVER
		IOOP_ACCEPT,		// Accept
//#endif	// gmTCP_SERVER
		IOOP_RD,		// Read
		IOOP_WR			// Write
	};


////////////////////////////////////////////////////////////////////////
// 오브젝트를 생성할 때, 주어지는 정보
protected:	// friend class	gmAcceptEx
	gmCONN	*m__lpConnV;
	UINT	m__maxConn;
public:
	gmCONN	*mf__GetLpConnV(VOID) {return m__lpConnV;}
protected:
	UINT	m__nConnC;
protected:	// friend class	gmPseudoTCP
	// 총 연결 수
	// (클라이언트의 경우에 m__NConn = 0이면 스스로 소멸시키기 위하여,
	// 서버의 경우에는 총 연결이 몇 개인지를 보고할 때 등등...)
	UINT	m__NConn;
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
// TCP메시지의 크기와 관련된 멤버(함수)
// 2003/10/17
// 2003/10/24	friend 사용
private:
	UINT	m__MaxTCPMsgSize;
private:	// friend class	gmPseudoTCP
	UINT	mf__GetMaxTCPMsgSize(VOID) {
		return m__MaxTCPMsgSize;
	}
private:
	UINT	m__RecvSize;
	UINT	m__SendSize;
	UINT	m__PubSize;
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
// 생성자와 소멸자
// 2003/10/24
protected:
	// gmTCPBase를 상속받은 클래스(gmTCPServer, gmTCPClient)에서
	// gmTCPBase를 초기화할 때, 생성자가 제대로 일을 처리했는 지에
	// 대한 정보를 가지고 있는 변수
	INT	m__baseStatus;
private:
	VOID	mf__releaseBase(VOID);
protected:
	gmTCPBase(
		gmCONN*	lpConnV,
		UINT	nConnC,
#ifdef __gmUSE_PUBLIC_QUEUE__
		UINT	PubSize,
#endif	// __gmUSE_PUBLIC_QUEUE__
		UINT	MaxTCPMsgSize,
		DWORD	dwMsgUnit,	// 매 dwMsgUnit메시지마다 알려준다. 

		BOOL	bUseConsole
	);
private:
	virtual INT	mf__close(VOID)	= NULL;
protected:
	~gmTCPBase();
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
// 이 클래스에서 관리하는 gmPseudoTCP클래스 배열에 대한 포인터
// 2003/10/24
protected:	// friend class	gmAcceptEx
	gmPseudoTCP	*m__lpgmPseudoTCP;
public:
	gmPseudoTCP	*mf__GetLpPseudoTCP(VOID) {return m__lpgmPseudoTCP;}
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
private:
	// 2005/3/24 (EST) - ghkim
	// 특정 gmPseudoTCP에 다시 연결이 될 때,
	// 이전의 gmPseudoTCP가 설정한 IOCP는 무료로 하기 위하여
	// 각 연결마다 유일한 값을 주자.
	// 이 값의 초기값은 1이고, 각 연결마다 1씩 증가한다.
	// 이 값이 0이면 처음으로 돌아왔다는 걸 듯한다.
	// Q. 0xFFFFFFFF번째 전의 연결정보가 이 때까지 살아있을까?
	// Q. 불가능하다.
	// 서버에게만 의미가 있다.
	// 서버는 이 값을 1로 초기화하고,
	// 연결이 이루어질 때마다 이 값을 1증가시킨다.
	// 이 값이 0이 되면 다시 1로 초기화한다.
	// 즉 서버는 이 값이 0일 수 없다.
	// 클라이언트는 이 값을 항상 0으로 한다.
	DWORD	m__dwUID;
public:
	DWORD	mf__GetUID(void)	{return m__dwUID;}
	void	mf__SetUID(DWORD dwUID)	{m__dwUID = dwUID;}
private:
	// 2005/3/24 (EST) - ghkim
	// 몇 개의 메시지마다 알려줄 지를 결정한다.
	// 0이라면 메시지 갯수가 0이 아니라면 무조건 알려준다.
	DWORD	m__dwMsgUnit;
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
// 공용큐의 관리에 사용되는 멤버(함수)들
// 2003/10/24	friend를 사용
	// 공용큐에 새로운 메시지가 있다고 알릴 때에 사용
	// (gmPseudoTCP::mf__UpdateAllQueues,
	// gmPseudoTCP::mf__closeRecv에서만 사용)
#ifdef __gmMESSAGE_IOCP__
private:
	HANDLE	m__hMessageIOCP;
public:
	HANDLE	mf__GetMessageIOCPHandle(void)	{return m__hMessageIOCP;}
#else	// __gmMESSAGE_IOCP__
private:
	HANDLE	m__hEventNotify;
public:
	HANDLE	mf__GetNotifyEvent(VOID) {return m__hEventNotify;}
private:	// friend class	gmPseudoTCP
	BOOL	mf__notify(VOID) {return SetEvent(m__hEventNotify);}
public:
	DWORD	mf__wait(DWORD dwMS) {
		return WaitForSingleObject(m__hEventNotify, dwMS);
	}
#endif	// !__gmMESSAGE_IOCP__
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
// 호출한 소켓함수(WSA...)중에서, 문제가 있던 호출의
// WSAGetLastError함수의 리턴값을 관리하는 데에 사용되는 멤버(함수)들
// 2003/10/17
// 2003/10/24	friend를 사용
protected:
	INT	m__WSALastErrorCode;
public:
	INT	mf__GetWSALastErrorCode(VOID) {
		mf__lock();
		INT	WSALastErrorCode = m__WSALastErrorCode;
		mf__unlock();
		return WSALastErrorCode;
	}
private:	// friend class	gmPseudoTCP
		// friend class	gmAcceptEx
// 2003/10/30
// mf__SetWSALastErrorCode를 gmAcceptEx클래스에서도 호출할 수 있도록
// gmAcceptEx를 friend클래스로 등록
	VOID	mf__SetWSALastErrorCode(INT WSALastErrorCode) {
		mf__lock();
		m__WSALastErrorCode = WSALastErrorCode;
		mf__unlock();
		return;
	}
////////////////////////////////////////////////////////////////////////


#ifdef __gmUSE_PUBLIC_QUEUE__
////////////////////////////////////////////////////////////////////////
// 2003/10/17
// "공용큐(Public Queue)"와 관련된 멤버(함수)들
// 1. 공용큐에 접근하기 위한 멤버(함수)
private:
	gmQueue	*m__lpPublicQueue;
private:	// friend class	gmPseudoTCP
	gmQueue	*mf__GetPublicQueue(VOID) {return m__lpPublicQueue;}
public:
	UINT	mf__GetPublicQueueLength(VOID) {return m__lpPublicQueue->m__length;}

#ifdef __gmTCP_DEBUG__
// 2. 공용큐가 최대 얼마나 사용하는 지를 알아보기 위한 멤버(함수)
private:
	UINT	m__MaxPublicQueueLength;
private:	// friend class	gmPseudoTCP
	UINT	mf__GetMaxPublicQueueLength(VOID) {
		mf__lock();
		UINT MaxPublicQueueLength = m__MaxPublicQueueLength;
		mf__unlock();
		return MaxPublicQueueLength;
	}
	VOID	mf__SetMaxPublicQueueLength(UINT MaxPublicQueueLength) {
		mf__lock();
		m__MaxPublicQueueLength = MaxPublicQueueLength;
		mf__unlock();
		return;
	}
#endif	// __gmTCP_DEBUG__
////////////////////////////////////////////////////////////////////////
#else	// __gmUSE_PUBLIC_QUEUE__
////////////////////////////////////////////////////////////////////////
// 각 연결은 상대에게서 받은 메시지를 "공용큐"에 저장하기 않고
// 자신의 "받는 큐"에 저장한다.
// 11/20/2003
////////////////////////////////////////////////////////////////////////
#endif	// !__gmUSE_PUBLIC_QUEUE__


#ifdef __gmUSE_PUBLIC_QUEUE__
////////////////////////////////////////////////////////////////////////
// 공용큐에서 메시지를 가져올 때 사용되는 멤버함수들
private:
	INT	mf__peek(LPTCPPBUF lpTCPPBuf, PUINT lpMsgLen = NULL);
	// lpTCPPBuf->TCPBuf.Header.cbData (IN/OUT)
public:
	INT	mf__GetMessage(LPTCPPBUF lpTCPPBuf) {
		return mf__peek(lpTCPPBuf);
	}
	INT	mf__PeekMessage(LPTCPPBUF lpTCPPBuf, PUINT lpMsgLen) {
		return mf__peek(lpTCPPBuf, lpMsgLen);
	}
	VOID	mf__Shift(UINT msgLen);
////////////////////////////////////////////////////////////////////////
#endif	// __gmUSE_PUBLIC_QUEUE__


////////////////////////////////////////////////////////////////////////
// 모든 연결의 개인큐에 쌓여있는 메시지를 보낼 때 사용되는 멤버함수
public:
	VOID	mf__flushAll(VOID);
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
// 
// 2003/10/17
// 2003/10/24	friend 사용
protected:	// friend class	gmPseudoTCP
	HANDLE	m__hEventWorkerToManager;
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
// 2003/10/23
// mf__lock가 한 쓰레드에서 중복호출되더라도 deadlock의 문제는 없다.
// 왜냐하면 Critical Section은 이 문제가 생기지 않도록 고안되었기 때문이다.
// Critical Section Objects와 EnterCriticalSection함수에
// 대한 메뉴얼을 참고하라.
private:
	LPCRITICAL_SECTION	m__lpCriticalSection;
protected:	// friend class	gmPseudoTCP
	VOID	mf__lock() {EnterCriticalSection(m__lpCriticalSection);}
	VOID	mf__unlock() {LeaveCriticalSection(m__lpCriticalSection);}
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
// 2003/10/31
// GetWSAErrorStringFromCode를 멤버함수로 등록
protected:
	INT	mf__GetWSAErrorStringFromCode(LPTSTR pszMessage, PDWORD pcbMessage, INT WSAErrorCode);
////////////////////////////////////////////////////////////////////////

#ifdef __gmUSE_PUBLIC_QUEUE__
#else	// __gmUSE_PUBLIC_QUEUE__
////////////////////////////////////////////////////////////////////////
// 2003/11/27
// 각각의 연결을 순차적으로 접근하기위한 멤버(함수)
private:
	UINT	m__iterIndex;
public:
	VOID	mf__iterReset(VOID) {m__iterIndex = 0;}
	// mf__nextTCP를 호출해서 특정조건을 만족하는 TCP연결을 찾기 전에,
	// 반드시 mf__iterReset을 호출해야 한다.
	gmPseudoTCP	*mf__nextTCP(gmPseudoTCP::gmPseudoTCPSTATUS gmPseudoTCPStatus = (gmPseudoTCP::gmPseudoTCPSTATUS) ~gmPseudoTCP::gmPseudoTCPSTATUS__NOT_CONNECTED);
////////////////////////////////////////////////////////////////////////
#endif	// __gmUSE_PUBLIC_QUEUE__

// 2005/4/5 (EST) - ghkim
#ifdef __gmTLS__
private:
	void	mf__RomeoMustDie(void)	{int i = *((int *) 0);}

protected:
	DWORD	m__TLSLastErrorCode;
public:
	DWORD	mf__GetTLSLastErrorCode(void)		{return m__TLSLastErrorCode;}

	// SSPI를 초기화할 때 사용된다.
	// 설명은 MSDN(Initilizing the Security Package)을 내용을 보고,
	// 사용하는 예는 Samples/security/SSPI/SSL폴더안에 있는 WebServer.c, WebClient.c를 보라.
protected:
	HMODULE			m__hModule;
	PSecurityFunctionTable	m__pSFT;	// Windows 98에서는 이 포인터를 통해서 함수들을 접근해야 한다.
public:
	PSecurityFunctionTable	mf__GetPSFT(void)	{return m__pSFT;}

protected:
	CredHandle	m__hCredential;
	BOOL		m__bValidCredential;	// m__hCredential이 AcquireCredentialsHandle을 통해서 얻어진 유효한 것인지를 나타낸다.
						// Destructor에서 사용된다.
	CredHandle	mf__GetCredential(void)		{return m__hCredential;}

private:
	// InitializeSecurityContext(클라이언트), AcceptSecurityContext(서버)에
	// 사용할 Credential에 대한 핸들을 얻는다.
	//	[註] AcquireCredentialsHandle은 Windows 98에서 사용할 수 없다.
	// 성공이면 SEC_E_OK를 리턴하고, 실패하면 다른 값을 리턴한다.
	// 서버에서, 리턴값이 -1이면 인증서를 찾지 못한 경우이고,
	// 이 때에는 m__TLSLastErrorCode를 참고해서 이유를 알아볼 수 있다.
	virtual SECURITY_STATUS	mf__AcquireCredentialsHandle(PDWORD pdwLastErrorCode) = 0;
#endif	// __gmTLS__

#ifdef __gmCRYPT__
	DWORD		m__dwCryptLastErrorCode;
	HCRYPTPROV	m__hCryptProv;
	HCRYPTKEY	m__hEncryptKey;		// 특정 문자열을 통해 만들어진 암호화할 때 사용되는 키
	HCRYPTKEY	m__hDecryptKey;		// 특정 문자열을 통해 만들어진 암호화를 풀 때 사용되는 키
#endif	// __gmCRYPT__
};



////////////////////////////////////////////////////////////////////////////////
#endif	// __gmTCP_BASE__
