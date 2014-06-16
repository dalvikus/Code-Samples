#ifndef __gmTCP_DEF__
#define __gmTCP_DEF__
////////////////////////////////////////////////////////////////////////////////

#define __SUICIDE__	{int i = *((int *) 0);}

const USHORT	gmMAGIC_TYPE_ID		= 0x1219;
const UINT	gmTCPMSG_CLOSE_SEND	= 0x80000001;
const DWORD	gmCONNECTION_TIMEOUT	= 10000;	// 10 초
const DWORD	gmDISCON_BLOCK_TIMEOUT	= 4096;		// 약 4초
const DWORD	gmCOMMAND_CHANNEL	= 0x1219;
const DWORD	gmSUBCOMMAND_NONBLOCK	= 0x0907;
const DWORD	gmSUBCOMMAND_BLOCK	= 0x0908;
const DWORD	gmSUBCOMMAND_TERM	= 0x1231;

typedef LPVOID	gmHTCP;	// TCP Handle: gmPseudoTCP 클래스에 대한 포인터

typedef struct {
	LPCTSTR	TCPAddr;
	USHORT	TCPPort;
	UINT	MaxConn;

//	UINT	RecvSize;	// recv. queue size in exponent
//	UINT	SendSize;	// send queue in exponent
} gmCONN;

typedef struct {
	USHORT	hType;
	USHORT	cbData;
} MTCPHEADER, *PMTCPHEADER;

typedef struct {
	MTCPHEADER	Header;
	PBYTE		pbData;
} MTCPMSG, *PMTCPMSG;

#ifdef __gmUSE_PUBLIC_QUEUE__
typedef struct {
	gmHTCP	gmhTCP;	// 두 멤버의 순서를 지켜라.
			// gmPseudoTCP::mf__UpdateAllQueues 함수를 보아라.
	TCPBUF	TCPBuf;
} TCPPBUF, *LPTCPPBUF;
#endif	// __gmUSE_PUBLIC_QUEUE__

////////////////////////////////////////////////////////////////////////
////////////////
// 2005/3/23 (EST) - ghkim
// "받는" 큐가 중지되었을 때, 큐를 활성화활 때 문제가 있는 경우를 위해
// 다음 2개를 정의한다.
#define TCPBUF__OK_BUT		1		// "받는" 큐에서 메시지를 가져올 수 있었으나,
						// 중지된 큐를 활성화할 때 문제가 있다.
						// 자세한 것은 gmPseudoTCP::mf__peek을 보라.
						// gmPseudoTCP::mf__disconnect를 호출하여
						// 해당 연결을 끊어야 한다.
#define TCPBUF__INTERNAL_ERROR	0x80000001L	// 이미 메시지를 가져왔는 데도 불구하고,
						// 큐에 공간이 없다고 한다.
////////////////
// 2005/3/23 (EST) - ghkim
// gmPseudoTCP::mf__Shift를 호출할 때, 파라미터의 정당성을 확인한다.
// 이 때, 헤더정보와 일치하지 않으면 다음 값을 리턴한다.
#define TCPBUF__BAD_PARAMETER	0x80000002L
// gmPseudoTCP::mf__peek를 호출할 때, 이미 연결이 끊어졌을 수 있다.
#define TCPBUF__NOT_ESTABLISHED	0x80000003L

#define TCPBUF__OK		0	// 성공
#define TCPBUF__NO_HEADER	-1
	// 헤더정보도 없다. (mf__Shift...)
	// 아무런 문제가 되지 않는다. 다음을 기다리는 수밖에...
#define TCPBUF__PARTIAL_DATA	-2
	// 헤더정보는 있지만 "완벽한" 데이터가 없다. (mf__Shift...)
	// 아무런 문제가 되지 않는다. 다음을 기다리는 수밖에...
#define TCPBUF__NOT_ENOUGH	-3
	// "완벽한" 데이터가 있지만
	// 데이터를 저장할 충분한 공간이 없다. (mf__Push..., mf__Shift...)
	// mf__Push... 의 경우에는 문제가 되지 않지만,
	// mf__Shift... 의 경우에는 데이터가 엉켰음이 틀림없다.
	// 왜냐하면 데이터의 크기는 커봐야 MAX_TCPMSGSIZE 이기 때문이고
	// 테이터의 크기는 항상 이 값과 비교하기 때문이다.
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
// mf__PostMsg()함수에서...
#define TCPPOSTMSG__ERROR		-1
	// 어느 정도 큐가 차서 큐의 내용을 보내려 했으나,
	// 큐의 내용을 보내는 데에 실패(WSASend의 실패)
// 아래의 값들은 모두 양수(>0)이어야 한다.
// 자세한 것은 gmQueue::mf__PostMsg() 함수를 보라.
#define TCPPOSTMSG__OK					0
	// 큐에 데이터를 넣는 데에 성공
#define TCPPOSTMSG__SEND_OK_IMMEDIATELY_BUT		1
	// 어느 정도 큐가 차서, 큐의 내용을 "곧 바로" 보내는 데에는 성공했으나 나머지 큐에 공간이 없다.
#define TCPPOSTMSG__SEND_OK_BUT				2
	// 어느 정도 큐가 차서, 큐의 내용을 보내는 데에는 성공했으나 나머지 큐에 공간이 없다.
#define TCPPOSTMSG__NOT_ESTABLISHED			3
	// 큐에 데이터를 넣으려고 했으나
	// 해당하는 상대와의 연결이 이미 끊어졌다.
#define TCPPOSTMSG__WSASEND_IN_PROGRESS_NOT_ENOUGH	4
	// 어느 정도 큐가 차서 큐의 내용을 보내려 했으나,
	// 이전의 WSASend가 진행 중이고 나머지 큐에도 공간이 없다.

// mf__SendMsg()함수에서...
#define TCPSENDMSG__NOT_ESTABLISHED		0
	// 큐에 메시지를 넣으려고 했으나
	// 해당하는 상대와의 연결이 이미 끊어졌다.
#define TCPSENDMSG__NOT_ENOUGH			1
	// 큐에 메시지를 넣을 충분한 공간이 없다.
#define TCPSENDMSG__WSASEND_IN_PROGRESS		2
	// 큐에 메시지를 넣었다. 그러나,
	// 이전의 WSASend가 진행 중이다.
#define TCPSENDMSG__ERROR			-1
	// WSASend에서 문제가 발생했고 WSAGetLastError() != WSAECONNRESET
#define TCPSENDMSG__OK_IMMEDIATELY		3
	// WSASend에서 곧 바로 데이터를 보냈다.
#define TCPSENDMSG__OK				4
	// WSASend에서 데이터를 보내는 중이다.

// mf__SendMessageEx()함수에서...
#define TCPSENDMSGEX__NOT_ESTABLISHED		0
	// 큐에 메시지를 넣으려고 했으나
	// 해당하는 상대와의 연결이 이미 끊어졌다.
#define TCPSENDMSGEX__NOT_ENOUGH		1
	// 큐에 메시지를 넣을 충분한 공간이 없다.
#define TCPSENDMSGEX__IN_PROGRESS_TIMEOUT	2
	// 진행중인 WSASend가 끝나기를 기다렸지만
	// 시간초과가 일어났다.
#define TCPSENDMSGEX__ERROR			-1
	// WSASend에서 문제가 발생했다.
#define TCPSENDMSGEX__OK			3
	// WSASend에서 모든 데이터를 보냈다.
#define TCPSENDMSGEX__OK_PARTIAL		4
	// WSASend에서 일부 데이터를 보냈다.
////////////////////////////////////////////////////////////////////////

#ifdef __gmCRYPT__
#if 1	// 2.2
#define TCPBUF__CRYPT_ERROR		-3
#else	// 2.1
#define TCPSENDMSG__ENCRYPT_ERROR	-3
#endif	// 2.1
#endif	// __gmCRYPT__

////////////////////////////////////////////////////////////////////////
#define TCPPUBLICQUEUE__WARN	-1
	// 특정 "받는 큐"의 데이터가 엉켰다. 우선 연결을 끊는다.
#define TCPPUBLICQUEUE__ERROR	-2
	// 공용큐가 가득찼다. 서버를 멈추어야 한다.(?)
////////////////////////////////////////////////////////////////////////

// 2005/4/21 (EST) - ghkim
//	gmPseudoTCP::SendMessage함수에서 TCPSENDMSG__WSASEND_IN_PROGRESS가 리턴된 경우
//	SENDMESSAGE_MAXLOOPCOUNTER동안 시도한다. 이 때, SENDMESSAGE_TIMEINTERVAL밀리초동안 잠시 멈추어있다.
const DWORD	SENDMESSAGE_MAXLOOPCOUNTER	= 10;	// # of Trials
const DWORD	SENDMESSAGE_INTERVAL		= 10;	// Milliseconds


////////////////////////////////////////////////////////////////////////////////
#endif	// __gmTCP_DEF__
