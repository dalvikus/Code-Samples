#ifndef __gmQUEUE__
#define __gmQUEUE__
////////////////////////////////////////////////////////////////////////////////


#ifdef __gmCRYPT__	// 2.2
#include <WinCrypt.h>	// HCRYPTKEY
#endif	// __gmCRYPT__	// 2.2
/*
	1) m__head = m__tail 이면
		m__length = 0
	이거나
		m__length = m__size
	이다.

	2) m__length = 0 이거나 m__length = m__size 이면
		m__head = m__tail
	이다.
 */
class gmQueue
{
	friend class	gmPseudoTCP;
	// mf__PushP, mf__Push, mf__Shift, mf__ShiftEx
	friend class	gmTCPBase;

private:
	UINT	m__exponent;
private:	// friend class	gmPseudoTCP
	UINT	m__mask;
	UINT	m__head;
	UINT	m__tail;

	UINT	m__size;
	UINT	m__length;

	PCHAR	m__lpBuf;

public:
	gmQueue(
		UINT,	// Queue size in exponent
		PINT	lpgmStatus = 0
	);
	~gmQueue();

#ifdef __gmUSE_PUBLIC_QUEUE__
private:	// friend class	gmPseudoTCP
	INT	mf__PushP(LPTCPPBUF);
	INT	mf__ShiftP(PTCPPBUF);	// lpTCPPBuf->TCPBuf.Header.cbData (IN/OUT)

private:	// friend class	gmTCPBase
	INT	mf__ShiftPEx(LPTCPPBUF, PUINT lpMsgLen);
					// lpTCPPBuf->TCPBuf.Header.cbData (IN/OUT)
#endif	// __gmUSE_PUBLIC_QUEUE__

private:	// friend class	gmPseudoTCP
	INT	mf__Push(
#ifdef __gmCRYPT__	// 2.2
		HCRYPTKEY	hEncryptKey,
#endif	// __gmCRYPT__
		PMTCPMSG	pMTCPMsg
	);

	INT	mf__ReadHeader(PMTCPHEADER pHeader);
	INT	mf__Shift(PMTCPMSG pMTCPMsg);	// pMTCPMsg->Header.cbData (IN/OUT)
	INT	mf__ShiftEx(PMTCPMSG pMTCPMsg, PUINT lpMsgLen = NULL);
						// pMTCPMsg->Header.cbData (IN/OUT)
	INT	mf__ShiftEx(UINT MTCPMsgLen);
};


////////////////////////////////////////////////////////////////////////////////
#endif	// __gmQUEUE__
