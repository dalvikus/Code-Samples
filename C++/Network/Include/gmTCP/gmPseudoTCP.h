#ifndef __gmPSEUDO_TCP__
#define __gmPSEUDO_TCP__
////////////////////////////////////////////////////////////////////////////////


// 2005/4/6 (EST) - ghkim
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

typedef struct _MSGBLOB {
	ULONG	cbBuffer;
	PVOID	pvBuffer;
} MSGBLOB, *PMSGBLOB;
#endif	// __gmTLS__

class gmTCPBase;

class gmPseudoTCP
{
////////////////////////////////////////////////////////////////////////
// 2003/10/23	friend를 써서 application프로그램에서 호출되는,
//		public으로 선언된 함수의 갯수를 최소화했다.
	friend class	gmTCPBase;
	// mf__Initialize, mf__Finalize

	friend class	gmTCPServer;
	// mf__IOOP_RD, mf__IOOP_WR, m__globalIndex, m__localIndex, m__ID
	// m__sock, mf__IOCPRecv, mf__close, mf__closeRecv

	friend class	gmTCPClient;
	// mf__IOOP_RD, mf__IOOP_WR, m__localIndex, m__sock, mf__IOCPRecv
	// mf__close, mf__closeRecv

	friend class	gmAcceptEx;
	// mf__Reset
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
// gmPseudoTCP오브젝트의 생성과 소멸에 사용되는 멤버함수들
// 생성자와 소멸자를 대신한다.
// 2003/10/18
// 2003/10/24	friend를 사용
private:	// friend class	gmTCPBase
	// 생성자
	INT	mf__Initialize(
		gmTCPBase	*lpTCPBase,
		UINT	globalIndex,
		UINT	localIndex,
		UINT	index,
		UINT	RecvSize,
		UINT	SendSize
	);
	// 소멸자
	VOID	mf__Finalize(VOID);

private:	// friend class	gmAcceptEx
	// gmAcceptEx::mf__IsConnectionAccepted에서
	// 새로운 연결이 이루어질 때마다 호출되는 멤버함수
	// (큐의 초기화에 사용된다.)
	VOID	mf__Reset(VOID);
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
// 2003/10/24	friend를 사용
private:
	// 공용큐에 넣기전에 mf__UpdateAllQueues에서 사용되는 임시 저장소
	// TCP메시지의 크기가 상수라면 필요하지 않을텐데...
	PCHAR	m__lpTCPMsgBuf;

private:
	// 이 클래스를 생성한 클래스(gmTCPBase)의 여러멤버를
	// 접근하기 위해서 사용되는 gmTCPBase클래스에 대한 포인터
	gmTCPBase	*m__lpTCPBase;

private:	// friend class	gmAcceptEx, gmTCPClient
	// 서버의 경우(gmAcceptEx)에는 연결된 클라이언트의 주소를,
	// 클라이언트의 경우에는 연결하고자 하는 서버의 주소를 저장한다.
	// 클라이언트의 경우에는 m__lpTCPBase->m__lpConnV를 통해서
	// 연결하고자 하는 서버의 주소를 결정하므로,
	// 클라이언트의 경우에는 이 주소가 큰 의미가 없다.(중복)
	struct sockaddr_in	m__addr;

private:	// friend class	gmTCPServer, gmTCPClient, gmAcceptEx
	// 접속된 클라이언트와 통신하는 데에 사용되는 소켓
	SOCKET	m__sock;

private:	// friend class	gmAcceptEx, gmTCPServer, gmTCPClient
	// WSARecv를 통해서 메시지를 받은 최근의 상대적인 시각
	DWORD	m__dwMilliSeconds;

private:
	// WSASend가 진행중일 때, 또 다른 WSASend를 호출하지 못하도록
	// 하기 위해서 사용되는 멤버
	BOOL	m__bIOCPSendInProgress;
public:
	BOOL	mf__bIOCPSendInProgress(VOID);
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
// 2003/10/17
// 2003/10/24	friend를 사용
// 이 오브젝트를 확인할 수 있는 여러 멤버(함수)
	// 서버의 경우에는 연결을 기다리는,
	// 클라이언트의 경우에는 연결하고자 하는
	// 포트와 관련된 인덱스이다.
private:	// friend class	gmTCPServer
	UINT	m__globalIndex;
public:
	UINT	mf__GetGlobalIndex(VOID)	{return m__globalIndex;}

	// 주어진 포트와 연결된 오브젝트의 인덱스이다.
private:	// friend class	gmTCPServer, gmTCPClient
	UINT	m__localIndex;
public:
	UINT	mf__GetLocalIndex(VOID)		{return m__localIndex;}

	// 2005/4/4 (EST) - ghkim
	// 전체적인 인덱스이다.
private:
	UINT	m__index;
public:
	UINT	mf__GetIndex(VOID)		{return m__index;}

	// 이 오브젝트가 실제로 응용프로그램의 어떤 정보와
	// 관련을 짓는 데 사용되는 값(포인터나 인덱스 등등)이다.
	// 예를 들어, 서버의 경우에는 접속된 클라이언트와 초기 설정이
	// 성공적으로 마쳤을 때 해당 클라이언트의 "캐릭터 아이디"가 될 수 있다.
	// 즉 가장 최근에 접속한 캐릭터 아이티를 가지고 있다.
	// 초기값은 (DWORD) ~0
private:	// friend class	gmTCPServer
	DWORD	m__ID;
public:
	DWORD	mf__GetID(VOID) {return m__ID;}
	VOID	mf__SetID(DWORD ID) {m__ID = ID;}
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
// 연결된 클라이언트와 데이터를 주고 받을 때에
// 사용되는 버퍼등을 관리하는 클래스로서
// "동적"으로 관리된다. 즉 클라이언트와 연결되면
// 버퍼를 할당하고, 연결을 끊었을 때(끊겼을 때)에는
// 버퍼를 시스템에 반환한다.
// 2003/10/17	mf__Initialize, mf__Finalize를 통해서
//		"정적"으로 관리된다.
//		주된 이유는 gmTCPServer::mf__IOCPWork에서
//		예외(exception)이 발생하기 않기 위해서
private:
	gmTrans	*m__lpRecv;
	gmTrans	*m__lpSend;
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
// 2003/10/17
// 2003/10/23	friend를 사용
// WSARecv, WSASend에 사용되는 멤버함수들
private:
	DWORD	mf__SetRecvBuf(DWORD*);
	DWORD	mf__SetSendBuf(DWORD*);
private:
	INT	mf__IOCPSend(VOID);
// 리턴값
//	-1	WSASend에서 문제 발생(자세한 것은 m__WSALastError를 보라.)
//	0	WSASend에서 데이터를 곧 바로 보냈다.
//	1	WSASend에서 데이터를 보내는 중이다.
//	2	보낼 테이터가 없다.
private:
	// mf__IOCPSend의 blocking버젼
// 리턴값
//	-1	WSASend에서 문제 발생(자세한 것은 m__WSALastError를 보라.)
//	0	WSASend에서 모든 데이터를 보냈다.
//	1	WSASend에서 일부 데이터를 보냈다.
//	2	보낼 테이터가 없다.
	INT	mf__IOCPSendWithBlocking(VOID);
public:	// friend class	gmTCPServer, gmTCPClient
// 2005/3/23 (EST) - ghkim
// mf__IOCPRecv의 리턴값을 정의했다.
	typedef enum {
		MSGTCP_IOCPRECV_IMMEDIATELY_OK	= 0x00010000L,	// WSARecv에서 곧 바로 네트워크 데이터를 읽었다.
		MSGTCP_IOCPRECV_PENDING_OK	= 0x00010001L,	// WSARecv에서 네트워크 데이터를 읽도록 설정했다.
		MSGTCP_IOCPRECV_BUFFER_FULL	= 0x00010002L,	// "받는" 큐가 가득찼다. WSARecv를 호출하지 않았다.
							// 계속받기를 원하면 어딘가(gmPseudoTCP::mf__peek, gmPseudoTCP::mf__Shift)에서 이를 확인하고,
							// 어딘가(gmPseudoTCP::mf__IOCPRecvContinue)에서 다시 활성화해야 한다.
		MSGTCP_IOCPRECV_WSARECV_ERROR	= 0x80010001L,	// WSARecv호출에 문제가 있다.
							// 에러코드는 gmBase::mf__GetWSALastErrorCode로부터 얻는다.
	} MSGTCP_IOCPRECV_XXX;
private:
	MSGTCP_IOCPRECV_XXX	mf__IOCPRecv(VOID);
// 리턴값
//	-1	WSARecv에서 문제 발생(자세한 것은 m__WSALastError를 보라.)
//	0	WSARecv에서 데이터를 곧 바로 읽었다.
//	1	WSARecv에서 데이터를 읽는 중이다.
//	-2	받는 큐가 차서 메세지를 받을 수 없다! (문제가 있군.)
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
// 개인큐에 메시지를 쌓아두고 나중에 보내거나
// 곧 바로 보낼 때에 사용되는 멤버함수들
public:
	// 2003/10/30
	// 함수의 prototype변경
	// 문제가 생긴 경우(리턴값 = -1)에 그 연결을 끊도록 한다.
	//VOID	mf__flush(VOID);	// 전
	INT	mf__flush(VOID);	// 후

private:
	INT	mf__PostMsg(PMTCPMSG pMTCPMsg);
	INT	mf__SendMsg(PMTCPMSG pMTCPMsg);
public:
	INT	mf__PostMessage(PMTCPMSG pMTCPMsg);
	INT	mf__SendMessage(PMTCPMSG pMTCPMsg);
	INT	mf__SendMessageEx(PMTCPMSG pMTCPMsg, DWORD dwMilliSeconds);
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
// 2003/10/17
// 2003/10/23	friend를 사용
// gmPseudoTCP 오브젝트의 상태와 관련된 멤버(함수)
public:
	enum gmPseudoTCPSTATUS {
		gmPseudoTCPStatusAccepted		= 0x00000001,
		gmPseudoTCPStatusEstablished		= 0x00000002,
		gmPseudoTCPStatusCloseWait		= 0x00000004,
		gmPseudoTCPStatusDisconnectWait		= 0x00000008,
		gmPseudoTCPSTATUS__NOT_CONNECTED	= 0x00000010
	};
private:
	gmPseudoTCPSTATUS	m__gmPseudoTCPStatus;
private:	// friend class	gmAcceptEx, gmTCPClient
	VOID	mf__SetPseudoTCPStatus(gmPseudoTCPSTATUS);
public:
	BOOL	mf__SetPseudoTCPStatusEstablished(VOID);
public:
	gmPseudoTCPSTATUS	mf__GetPseudoTCPStatus(VOID);
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
// 2003/10/10
// 2003/10/23	friend를 사용
// 상대방과의 연결을 끊는 데에 사용되는 멤버함수들
public:
	// application 프로그램에서 연결을 끊을 때에 사용되는 함수로서,
	// IOCP의 작업큐에 특정한 정보를 남긴다.
	// 이를 받은 worker쓰레드는 mf__closeEnd를 통해서 필요한 작업을
	// 마치고 이를 application 프로그램에 이벤트를 써서
	// 알려준다.(TRUE일 때) 이 때 사용되는 이벤트는
	// m__lpTCPBase->m__hEventWorkerToManager이다.
	INT	mf__disconnect(BOOL bBlocking = TRUE);	// Manager에서 호출
private:	// friend class	gmTCPServer, gmTCPClient
	INT	mf__closeEnd(BOOL bBlocking);		// Worker에서 호출

private:
	// IOCP작업을 처리하는 worker쓰레드에서 연결에 문제가
	// 있어서 해당 연결을 끊을 때에 사용되는 함수로서,
	// 받는 쪽만 먼저 닫고 이를 application 프로그램에 공용큐에
	// 특별한 메시지를 남김으로써 알린다.
	INT	mf__closeRecv(VOID);			// Worker에서 호출
public:
	// mf__closeRecv에 요구에 따라서 application 프로그램에서
	// 나머지 반쪽의 연결을 끊는다.
	// 해당연결의 정리작업도 한다.(주로 이 목적을 위해서 만들어졌다.)
	INT	mf__closeSend(VOID);			// Manager에서 호출
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
// IOCP의 작업에 따라 Worker쓰레드에서 호출하는 멤버함수들
// 2003/10/17
// 2003/10/23	friend를 사용
private:	// friend class	gmTCPServer, gmTCPClient
	VOID	mf__IOOP_WR(DWORD);	// WSASend가 끝난 경우
private:	// friend class gmTCPServer, gmTCPClient
	INT	mf__IOOP_RD(DWORD);	// WSARecv가 끝난 경우
private:
#ifdef __gmUSE_PUBLIC_QUEUE__
	INT	mf__UpdateAllQueues(DWORD);
					// mf__IOCP_RD에서만 호출되는 함수
#endif	// __gmUSE_PUBLIC_QUEUE__
////////////////////////////////////////////////////////////////////////


#ifdef __gmUSE_PUBLIC_QUEUE__
#else	// __gmUSE_PUBLIC_QUEUE__
////////////////////////////////////////////////////////////////////////
// > 11/20/2003 - ghkim
// 각 연결은 상대에게서 받은 메시지를 "공용큐"에 저장하기 않고
// 자신의 "받는 큐"에 저장한다.
// + 11/28/2003 - ghkim
// (Library에서) 받는 큐가 가득한 경우에, 더 이상 메시지를 받지 않도록 한다.
// (Application에서) 나중에 큐가 비었을 때, 다시 메시지를 받도록 한다.
// DWORD값을 사용하는 mf__SetOOB대신에 플래그를 이용하는
// mf__SetOOBFlag, mf__ClearOOBFlag를 사용한다.
public:
	enum gmPseudoTCPOOBSTATUS {
		gmPseudoTCPOOBStatusDisconnect		= 0x00000001,
		gmPseudoTCPOOBStatusPaused		= 0x00000002
	};
public:
	BOOL	mf__bDisconnect(VOID) {return mf__GetOOB() & gmPseudoTCPOOBStatusDisconnect;}
	BOOL	mf__bPaused(VOID) {return mf__GetOOB() & gmPseudoTCPOOBStatusPaused;}
	MSGTCP_IOCPRECV_XXX	mf__IOCPRecvContinue(VOID);
private:
	// Worker쓰레드가 manager쓰레드와 통신하기 위해서
	// 사용되는 변수이다.
	// 일종의 out of band(OOB)에 사용된다.
	DWORD	m__dwOOB;
private:
	VOID	mf__SetOOBFlag(gmPseudoTCPOOBSTATUS dwF);
	VOID	mf__ClearOOBFlag(gmPseudoTCPOOBSTATUS dwF);
private:
	DWORD	mf__GetOOB(VOID);


	// "받는 큐"에서 메시지를 가져올 때 사용되는 멤버함수들
private:
	INT	mf__peek(PMTCPMSG pMTCPMsg, PUINT lpMsgLen = NULL);
	// lpTCPBuf->Header.cbData (IN/OUT)
public:
	// 큐에서 데이터를 가져오고 그 만큼 큐를 비운다.
	INT	mf__GetMessage(PMTCPMSG pMTCPMsg)			{return mf__peek(pMTCPMsg);}
	// 큐에서 데이터를 가져오나 큐를 비우지 않는다.
	// 나중에 *msgLen와 mf__Shift를 이용해서 큐를 비울 수 있다.
	INT	mf__PeekMessage(PMTCPMSG pMTCPMsg, PUINT lpMsgLen)	{return mf__peek(pMTCPMsg, lpMsgLen);}
	INT	mf__Shift(UINT MTCPMsgLen);	// 큐의 크기를 msgLen만큼 줄일다.
	UINT	mf__GetRecvQueueSize(VOID);	// 큐의 크기를 안다.

	// 2005/3/22 (EST) - ghkim
private:
	UINT	m__MessageIncompleteHead;		// "받는 큐"에서 메시지의 시작을 가리키는 인덱스
	UINT	m__MessageIncompleteLength;
private:
	DWORD	mf__HowManyMessages(gmQueue *pRecvQueue, BOOL bBlind);
	// 2005/3/24 (EST) - ghkim
	// 특정 gmPseudoTCP에 다시 연결이 될 때,
	// 이전의 gmPseudoTCP가 설정한 IOCP는 무료로 하기 위하여
	// 각 연결마다 유일한 값을 주자.
private:
	DWORD	m__dwUID;
public:
	DWORD	mf__GetUID(void)	{return m__dwUID;}
	// 2005/4/4 (EST) - ghkim
	PDWORD	mf__GetPUID(void)	{return &m__dwUID;}

private:
	// 2005/3/24 (EST) - ghkim
	// 몇 개의 메세지가 들어올 때 마다 알릴 것인지를 결정한다.
	// gmTCPBase가 결정되면 상수이나,
	//	(1) Access를 빨리하기 위하여,
	//	(2) 나중에 각 gmPseudoTCP마다 다르게 할 수도 있으므로
	// 여기서 따로 보관한다.
	DWORD	m__dwMsgUnit;

	// 2005/3/23 (EST) - ghkim
	// "받는 큐"가 중지된 상태였을 때, 그 상태를 해제할 지를 판단한다.
private:
	BOOL	mf__bPolicyPausedQueueContinue(void);

	// 2005/3/25 (EST) - ghkim
	//	"받는" 큐에 메시지가 있다면 이를 알린다.
	BOOL	mf__bNotifyMessage(BOOL bBlind = FALSE);	// bBlind = FALSE	적당한 조건이 돼야 알린다.
								// bBlind = TRUE	무조건 알린다. (mf__flush에서 사용한다.)
////////////////////////////////////////////////////////////////////////
#endif	// !__gmUSE_PUBLIC_QUEUE__

	// 2005/4/6 (EST) - ghkim
#ifdef __gmTLS__
private:
	CtxtHandle	m__hContext;
	BOOL		m__bValidContext;	// m__hContext가 유효한 Context인지 아닌지를 말해준다.
public:
	PCtxtHandle	mf__GetCtxtHandle(void) {return &m__hContext;}

private:
	SecBufferDesc	m__InputBufferDesc, m__OutputBufferDesc;
	SecBuffer	m__InputBuffers[2], m__OutputBuffers[1];

public:
	SECURITY_STATUS	mf__InitializeSecurityContext(MSGBLOB& InputMessage, MSGBLOB& OutputMessage);
	SECURITY_STATUS	mf__AcceptSecurityContext(MSGBLOB& InputMessage, MSGBLOB& OutputMessage);

	// Handshake Protocol이 모두 이루어진 후에
	// QueryContextAttributes함수를 호출함으로써
	// 해당 CtxtHandle에서 사용하는 변수들의 최대값을 얻을 수 있다.
/*
 * SECPKG_ATTR_SIZES
 *
	typedef struct _SecPkgContext_Sizes {
		ULONG	cbMaxToken;
		ULONG	cbMaxSignature;
		ULONG	cbBlockSize;
		ULONG	cbSecurityTrailer;
	} SecPkgContext_Sizes, *PSecPkgContext_Sizes;
 */
/*
 * SECPKG_ATTR_STREAM_SIZES
 *
	typedef struct _SecPkgContext_StreamSizes {
		ULONG	cbHeader;
		ULONG	cbTrailer;
		ULONG	cbMaximumMessage;
		ULONG	cBuffers;
		ULONG	cbBlockSize;
	} SecPkgContext_StreamSizes, *PSecPkgContext_StreamSizes;
 */
private:
	SecPkgContext_Sizes		m__Sizes;
	SecPkgContext_StreamSizes	m__StreamSizes;
public:
	SECURITY_STATUS	mf__GetSizes(void);

private:
	SecBufferDesc	m__MessageBufferDesc;
	SecBuffer	m__MessageBuffers[4];
public:
	SECURITY_STATUS	mf__EncryptMessage(MSGBLOB& InputMessage, MSGBLOB& OutputMessage);
	SECURITY_STATUS	mf__DecryptMessage(MSGBLOB& InputMessage, MSGBLOB& OutputMessage);

	// 2005/4/6 (EST) - ghkim
public:
	BOOL	RecvMessage(MTCPMSG& Msg);
	BOOL	SendMessage(MTCPMSG& Msg);
	SECURITY_STATUS	HandshakeProtocol(
		MTCPMSG&	MsgFromPeer,	// [IN]
		MTCPMSG&	MsgToPeer,	// [IN/OUT]
		BOOL		bServer		// 서버: TRUE, 클라이언트: FALSE
	);
#endif	// __gmTLS__

#ifdef __gmCRYPT__
#if 0	// 2.1
private:
	BOOL	mf__EncryptQueue(gmQueue *lpgmQueue, DWORD dwIOSize);
	BOOL	mf__DecryptQueue(gmQueue *lpgmQueue, DWORD dwIOSize);
#endif	// 2.1
#endif	// __gmCRYPT__
};
////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////
#endif	// __gmPSEUDO_TCP__
