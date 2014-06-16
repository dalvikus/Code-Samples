#ifndef __gmTRANS__
#define __gmTRANS__
////////////////////////////////////////////////////////////////////////////////

typedef struct _WSAOVERLAPPEDPLUS {
	WSAOVERLAPPED	mWSAOverlapped;	// WSAOVERLAPPED 멤버가 제일 먼저 와야 한다.
		// 1. 설정할 때
		//	gmTCPServer::mf__PostAcceptEx
		//	gmPseudoTCP::mf__IOCP[Recv|Send]
		// 2. 결과를 얻을 때
		//	gmTCP[Server|Client]::mf__IOCPWork
	DWORD		mIOOpCode;
	union {
		DWORD	mBytesToBeWritten;	// IOOP_WR
		DWORD	mBytesToBeRead;		// IOOP_RD
//#ifdef gmTCP_SERVER
		UINT	mAcceptExIndex;		// IOOP_ACCEPT
//#endif	// gmTCP_SERVER
	} u;
} WSAOVERLAPPEDPLUS, *LPWSAOVERLAPPEDPLUS;

class gmTrans
{
	friend class	gmPseudoTCP;
	// m__WSABuf, m__dwFlags
	friend class	gmTCPClient;
	// m__WSAOverlappedPlus

public:
	gmTrans(
		UINT,	// Queue size in exponent
		PINT	lpgmStatus = 0
	);
	~gmTrans();

private:	// friend class	gmPseudoTCP
	// WSARecv, WSASend와 WSAGetOverlappedResult(IOCP의 경우)에서
	// 사용되는 변수들
	// NB. m__dwFlags은 WSARecv, WSASend의 항상 0으로 사용되므로
	// 필요하지 않을 수 있다. 하지만 WSAGetOverlappedResult에서는
	// 이전에 호출한 WSARecv나 WSASend에서 설정한 값(실제는 0)을
	// 알기 위해서 필요한 변수이다. 이 때, 사용되는 포인터를
	// NULL로 할 수 있다면 역시 필요하지 않다.
	////////////////////////////////////////////////////////////////
	WSABUF			m__WSABuf[2];	// 기껏해야 두 부분이 필요하다.
	DWORD			m__dwFlags;
private:	// friend class	gmTCPClient
	WSAOVERLAPPEDPLUS	m__WSAOverlappedPlus;
	////////////////////////////////////////////////////////////////

	gmQueue	*m__lpQueue;
};


////////////////////////////////////////////////////////////////////////////////
#endif	// __gmTRANS__
