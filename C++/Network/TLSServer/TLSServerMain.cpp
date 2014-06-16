inline void
RomeoMustDie(
	void
)
{
#ifdef _DEBUG
	__asm {
		INT	3;
	};
#else
	int	i = *((int *) 0);
#endif
}

// 2005/4/8 (EST) - ghkim
//	연결할 수 있는 최대 클라이언트 수를 정의한다.
#define MAX_TLSCLIENT	4096

#define MAX_TCPMSGSIZE	0x0400
#include <gmTCP/gmTCP.h>

#ifdef __gmMESSAGE_IOCP__
BOOL	*pbHandshakeDone;
#endif	// __gmMESSAGE_IOCP__

// 2005/4/8 (EST) - ghkim
//	Windows.h보다 Winsock2.h가 먼저 포함되면 안된다는 걸 잊지말 것!
#include <gmCommon/gmMisc.h>			// ByteToStringA
#include <gmSessionKey/gmSessionKeyManager.h>	// gmSessionKeyManager

#include <ATLBase.h>

int
main(
	void
)
{
	// 2005/4/8 (EST) - ghkim
	//	gmSessionKeyManager라이브러리를 사용한다.
	//	사용예는 Keys프로젝트를 보라.
	BOOL	bStatus;
	DWORD	dwLastErrorCode;
	gmSessionKeyManager	SessionKeyManager(
		// 2005/4/12 (EST) - ghkim
		CALG_MD5,	// 해시 알고리즘
		CALG_RC2,	// 암호화 알고리즘

		&bStatus,
		&dwLastErrorCode
	);
#ifdef _DEBUG
	_tprintf(TEXT("%c: 0x%08X\n"), bStatus == TRUE ? TEXT('T') : TEXT('F'), dwLastErrorCode);
#endif	// _DEBUG
	if (!bStatus) {
		_tprintf(TEXT("Error Code: 0x%08X\n"), dwLastErrorCode);
		return 1;
	}
	_tprintf(TEXT("OK\n"));
	// 2005/4/12 (EST) - ghkim
	DWORD	cbHashedAndEncryptedDataLength = SessionKeyManager.mf__cbHashedAndEncryptedDatalength();

////////////////////////////////////////////////////////////////////////
//                                                                    //
	WSADATA	wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData)) {
		RomeoMustDie();
	}
//                                                                    //
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
//                                                                    //
	gmTCPServer*	lpTCPServer = NULL;
	gmCONN	*lpConnV = NULL;
	gmCONN	gmConn;	// 하나의 연결만 기다린다.
	gmConn.TCPAddr = NULL;
	gmConn.TCPPort = 32005;
	gmConn.MaxConn = 16;//MAX_TLSCLIENT;
	int	gmStatus;
	lpTCPServer = new gmTCPServer(
		&gmConn,
		1,
		MAX_TCPMSGSIZE,
#ifdef __gmMESSAGE_IOCP__
		1,	// 한 번에 하나씩 메시지가 들어왔음을 알린다.
#endif	// __gmMESSAGE_IOCP__
#ifdef __gmTLS__
		TEXT("TLS"),
#endif	// __gmTLS__
		&gmStatus
	);
	if (!lpTCPServer || gmStatus != 0) {
		RomeoMustDie();
	}
//                                                                    //
////////////////////////////////////////////////////////////////////////

#ifdef __gmMESSAGE_IOCP__
{
	gmTCPBase	*lpTCPBase = (gmTCPBase *) lpTCPServer;
	HANDLE	hMessageIOCP = lpTCPBase->mf__GetMessageIOCPHandle();
	pbHandshakeDone = new BOOL[MAX_TLSCLIENT];

	BOOL		bStatus;
	DWORD		dwMessage;	// "받는" 큐에 있는 메시지의 갯수
	PDWORD		pdwUID;
//	DWORD		dwUID;		// 서버의 경우에는 0이 아니고, 클라이언트는 반드시 0이다.
	gmPseudoTCP	*hTCP;
	while (1) {
/*
		BOOL
		GetQueuedCompletionStatus(
			HANDLE		CompletionPort,
			LPDWORD		lpNumberOfBytes,
			PULONG_PTR	lpCompletionKey,
			LPOVERLAPPED	*lpOverlapped,
			DWORD		dwMilliseconds
		);
 */
		bStatus = GetQueuedCompletionStatus(
				hMessageIOCP,				//	HANDLE		CompletionPort
				&dwMessage,			//	LPDWORD		lpNumberOfBytes
				(PULONG_PTR) &hTCP,		//	PULONG_PTR	lpCompletionKey
				(LPOVERLAPPED *) &pdwUID,	//	LPOVERLAPPED	*lpOverlapped
				INFINITE			//	DWORD		dwMilliseconds
		);
		if (!bStatus) {
			RomeoMustDie();
		}

		UINT	index = hTCP->mf__GetIndex();
		////////////////////////////////////////////////////////////////
		// 성공적으로 IO Completion포트로부터 처리된 결과를 얻었다.
		//	(1) 리턴값이 0이 아니다.
		//	(2) dwMessage, gmhTCP은 적절하게 설정된다.
		//	(3) pdwUID값도 적절하게 설정되었으므로
		//	pdwUID가 가리키는 값 즉 dwUID값이 의미가 있다.
		////////////////////////////////////////////////////////////////
		DWORD	dwUID = *pdwUID;

#ifdef _DEBUG
		lpTCPBase->mf__fprintf(lpTCPBase->m__CONERR,
			TEXT("hTCP: %p, # = 0x%08X, dwUID = 0x%08X\n"), hTCP, dwMessage, dwUID
		);
#endif	// _DEBUG
		// 2005/3/25 (EST) - ghkim
		if (hTCP->mf__GetUID() != dwUID) {
			// 이전 연결에서 오는 정보이다.
			lpTCPBase->mf__fprintf(lpTCPBase->m__CONERR,
				TEXT("hTCP: 0x%p, # = 0x%08X, dwUID = 0x%08X(!= 0x%08X): \"일치하지 않은\" 연결에서 오는 정보이다.\n")
				,
				hTCP, dwMessage, dwUID, hTCP->mf__GetUID()
			);
			RomeoMustDie();
		}

		BOOL	bInitProtocol = FALSE;
		if (dwMessage & 0x80000000L) {
			// [경우 1] OOB정보이다.
			DWORD	dwOOB = dwMessage & 0xFFFF;
			if (dwOOB == 0x0001) {
				// 연결이 "강제로" 끊긴 경우이다.
				// "정상적"으로 끊긴 경우에는 메시지를 가져올 때 알 수 있다. (TCPBUF__NOT_ESTABLISHED)
				if (!hTCP->mf__bDisconnect() || hTCP->mf__closeSend()) {
						RomeoMustDie();
				}
				continue;
			} else if (dwOOB == 0x0002) {
				bInitProtocol = TRUE;
			} else {
				RomeoMustDie();
			}
		}

		BYTE	bData[MAX_TCPMSGSIZE];
		MTCPMSG	MsgReceived = {
			{
				(USHORT) -1,
				MAX_TCPMSGSIZE		// [IN/OUT]이므로 반드시 설정해야 한다.
			},
			bData
		};
		if (!bInitProtocol) {
			// [경우 2] "받는" 큐에 dwMessage개의 메시지가 있는 경우이다.
			// 서버는 클아이언트와의 연결이 확정되어야 클라이언트로부터 스프림을 읽는다.
			// 다음은 이를 설정하는 가장 간단한 예이다.
			// 실제로는 다른 절차를 통해서 설정해야 한다.
			// 예를 들어, IP가 블랙리스트에 있는 지 어떤 지를 조사할 수 있다.
			if (hTCP->mf__SetPseudoTCPStatusEstablished()) {
				pbHandshakeDone[index] = FALSE;
			}

			// 한번에 하나씩...
#ifdef __gmTLS__
			if (!hTCP->RecvMessage(MsgReceived)) {
				if (hTCP->mf__closeSend()) {
					RomeoMustDie();
				}
				continue;
			}
#endif	// __gmTLS__
		} else {
			// 클라이언트의 경우에,
			// Handshake Protocol의 처음에는
			// MsgReceived는 사용되지 않는다.
		}

		if (pbHandshakeDone[index]) {
			////////////////////////////////////////////////
			// 암호화된 메시지를 푼다.
			MSGBLOB	CipherTextBlob = {
				MsgReceived.Header.cbData,
				bData
			};
			BYTE	PlainTextBuffer[MAX_TCPMSGSIZE];
			MSGBLOB	PlainTextBlob = {
				MAX_TCPMSGSIZE,
				PlainTextBuffer
			};
			SECURITY_STATUS	SecurityStatus = hTCP->mf__DecryptMessage(CipherTextBlob, PlainTextBlob);
			if (SecurityStatus != SEC_E_OK) {
				RomeoMustDie();
			}
// [IN]
{
	USES_CONVERSION;
	// 제일 마지막에 '\0\을 넣어 문자열을 만들어 출력하자.
	PlainTextBuffer[PlainTextBlob.cbBuffer] = '\0';
	lpTCPBase->mf__fprintf(lpTCPBase->m__CONERR,
		TEXT("Plain Data: \"%s\"\n")
		,
		A2CT((LPCSTR) PlainTextBuffer)
	);
}

			////////////////////////////////////////////////
			// 2005/4/8 (EST) - ghkim
			//	문자열을 해시처리하고 Session Key로 암호화한 문자열을 만든다.
			// 2005/4/12 (EST) - ghkim
			if (!SessionKeyManager.mf__GetHashedAndEncryptedData(
				PlainTextBuffer,	// 데이터가 있는 버퍼
				PlainTextBlob.cbBuffer,	// 데이터의 크기
				MAX_TCPMSGSIZE		// 버퍼의 크기
			)) {
				RomeoMustDie();
			}
			// 이 데이터를 출력할 수 있는 문자열로 바꾼다.
			CHAR	String[MAX_TCPMSGSIZE];
			ByteToStringA(
				PlainTextBuffer,		// 데이터가 있는 버퍼
				cbHashedAndEncryptedDataLength,	// 데이터의 크기
				String				// 바이트문자열을 저장할 버퍼
			);
// [OUT]
{
	USES_CONVERSION;
	lpTCPBase->mf__fprintf(lpTCPBase->m__CONERR,
		TEXT("Hashed And Encrypted Data: \"%s\"\n")
		,
		A2CT(String)
	);
}

			////////////////////////////////////////////////
			// 메시지를 다시 암호화하여 되돌려 보낸다.
#if 1
			PlainTextBlob.cbBuffer = 2 * cbHashedAndEncryptedDataLength;
			PlainTextBlob.pvBuffer = String;
#endif
			CipherTextBlob.cbBuffer = MAX_TCPMSGSIZE;
			SecurityStatus = hTCP->mf__EncryptMessage(PlainTextBlob, CipherTextBlob);
			if (SecurityStatus != SEC_E_OK) {
				RomeoMustDie();
			}
			MTCPMSG	MsgToBeSent = {
				{
					(USHORT) -1,
					(USHORT) CipherTextBlob.cbBuffer
				},
				bData
			};
			if (!hTCP->SendMessage(MsgToBeSent)) {
				RomeoMustDie();
			}
			////////////////////////////////////////////////
			// 서버의 역할은 다했다.
			// 클라이언트가 연결을 끊기를 기다린다.
		} else {
#ifdef __gmTLS__
			BYTE	BufferToBeSent[MAX_TCPMSGSIZE];
			MTCPMSG	MsgToBeSent = {
				{
					(USHORT) -1,
					MAX_TCPMSGSIZE
				},
				BufferToBeSent
			};
			SECURITY_STATUS	SecurityStatus = hTCP->HandshakeProtocol(
				MsgReceived,
				MsgToBeSent,
				TRUE		// 서버
			);
#endif	// __gmTLS__
			if (SecurityStatus == SEC_E_OK) {
				// Handshake Protocol이 끝났다.
#ifdef _DEBUG
				lpTCPBase->mf__fprintf(lpTCPBase->m__CONERR,
					TEXT("[%u.%u] Handshake Protocol이 끝났다.\n")
					,
					hTCP->mf__GetGlobalIndex(),
					hTCP->mf__GetLocalIndex()
				);
#endif	// _DEBUG
				pbHandshakeDone[index] = TRUE;
			} else if (SecurityStatus == SEC_I_CONTINUE_NEEDED) {
				// Handshake Protocol의 중간단계이다.
#ifdef _DEBUG
				lpTCPBase->mf__fprintf(lpTCPBase->m__CONERR,
					TEXT("[%u.%u] Handshake Protocol의 중간단계이다.\n")
					,
					hTCP->mf__GetGlobalIndex(),
					hTCP->mf__GetLocalIndex()
				);
#endif	// _DEBUG
			} else {
//				RomeoMustDie();
				if (SecurityStatus == (SECURITY_STATUS) -3) {
					// gmPseudoTCP::SendMessage에서 문제가 있다.
					// 연결을 끊는다.
					(void) hTCP->mf__disconnect();
				}
			}
		}
	}

	delete[] pbHandshakeDone;
}
#endif	// __gmMESSAGE_IOCP__

////////////////////////////////////////////////////////////////////////
//                                                                    //
	if (lpTCPServer) {
		delete lpTCPServer;
		lpTCPServer = NULL;
	}
//                                                                    //
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
//                                                                    //
	if (WSACleanup()) {
		RomeoMustDie();
	}
//                                                                    //
////////////////////////////////////////////////////////////////////////

	return 0;
}
