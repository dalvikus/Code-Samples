#ifndef __gmACCEPT_EX__
#define __gmACCEPT_EX__
////////////////////////////////////////////////////////////////////////


#include <WinSock2.h>
#include <MSWSock.h>
#include <gmTCP/gmTCPDef.h>
#include <gmTCP/gmTCP.h>
class	gmTCPServer;

class gmAcceptEx
{
private:
	gmTCPBase	*m__lpTCPBase;
	HANDLE		m__hIOCP;

	UINT		m__globalIndex;

private:
	LPFN_ACCEPTEX	m__lpFnAcceptEx;
	LPFN_GETACCEPTEXSOCKADDRS	m__lpFnGetAcceptExSockaddrs;

	SOCKET	m__ListenSocket;
	SOCKET	mf__CreateListenSocket(VOID);

	SOCKET	m__AcceptExSocket;
	SOCKET	mf__CreateAcceptExSocket(VOID);

	CHAR	m__AcceptExBuffer[2 * (sizeof(SOCKADDR_STORAGE) + 16)];
	WSAOVERLAPPEDPLUS	m__AcceptExOverlappedPlus;


public:
	SOCKET
	mf__GetListenSocket(VOID) {return m__ListenSocket;}

	INT	mf__PostAcceptEx(VOID);
	gmPseudoTCP	*mf__IsConnectionAccepted(VOID);


private:
	VOID	mf__CleanUp(VOID);
public:
	gmAcceptEx(
		gmTCPBase*	lpTCPBase,
		UINT	globalIndex,

		PINT	lpgmStatus = NULL
	);	// Constructor
	~gmAcceptEx();	// Destructor
};


////////////////////////////////////////////////////////////////////////
#endif	// __gmACCEPT_EX__
