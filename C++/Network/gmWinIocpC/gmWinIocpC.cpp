// gmWinIocpC.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "resource.h"
////////////////////////////////////////////////////////////////////////
//                                                                    //
#define MAX_TCPMSGSIZE	1024

#include <gmTCP/gmTCP.h>

gmTCPClient*	g__lpTCPClient = NULL;
bool		g__bVerbose = false;

#define N__CLIENT	1
//#define MAX_TCPMSGSIZE	1024
UINT	g__N;	// # of clients
USHORT	g__M;	// # of messages
gmCONN	*g__lpConnV = NULL;

HANDLE	g__hTimer = NULL;	// _WIN32_WINNT=0x0500
INT
MessageLoop(
	HANDLE*	lphObjects,	// handles that need to be waited on
	int	cObjects	// number of handles to wait on
);
INT
run(
	VOID
);
#ifdef __gmMESSAGE_IOCP__
DWORD WINAPI
IocpMessageLoop(
	HANDLE	hIOCP
);
#else	// __gmMESSAGE_IOCP__
VOID
ClientRun(
	VOID
);
#endif	// !__gmMESSAGE_IOCP__

typedef struct {
	gmHTCP	gmhTCP;	// 두 멤버의 순서를 지켜라.
			// gmPseudoTCP::mf__UpdateAllQueues 함수를 보아라.
	MTCPMSG	MTCPMsg;
} TCPPBUF, *LPTCPPBUF;
LPTCPPBUF	g__lpHx, g__lpHy;
//                                                                    //
////////////////////////////////////////////////////////////////////////

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];								// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];								// The title bar text

// Foward declarations of functions included in this code module:
ATOM			MyRegisterClass(HINSTANCE hInstance);
BOOL			InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

////////////////////////////////////////////////////////////////////////////////
extern "C" int	WINAPI
_tWinMain(HINSTANCE, HINSTANCE, LPTSTR, INT);

int
_tmain(int argc, _TCHAR *argv[])
{
	HINSTANCE	hInstance;

	hInstance = (HINSTANCE) GetModuleHandle(NULL);

/*
#define SW_HIDE             0
#define SW_SHOWNORMAL       1
#define SW_NORMAL           1
#define SW_SHOWMINIMIZED    2
#define SW_SHOWMAXIMIZED    3
#define SW_MAXIMIZE         3
#define SW_SHOWNOACTIVATE   4
#define SW_SHOW             5
#define SW_MINIMIZE         6
#define SW_SHOWMINNOACTIVE  7
#define SW_SHOWNA           8
#define SW_RESTORE          9
#define SW_SHOWDEFAULT      10
#define SW_FORCEMINIMIZE    11
#define SW_MAX              11
 */
//	int	i = _tWinMain(hInstance, NULL, NULL, SW_SHOWNORMAL);
	_ftprintf(stderr, _TEXT("---- hello, world --------\n"));
	int	i = _tWinMain(hInstance, (HINSTANCE) argc, (LPTSTR) argv, SW_MINIMIZE);
	_ftprintf(stderr, _TEXT("---- The End of The World ----\n"));

	return i;
}

INT
StrToULong(
	LPTSTR	lpTStr,
	PULONG	lpULong
)
{
	LPTSTR	endptr;
	INT	base;
	if (_tcslen(lpTStr) >= 2 && lpTStr[0] == '0' && (lpTStr[1] == 'X' || lpTStr[1] == 'x'))
		base = 16;
	else if (_tcslen(lpTStr) >= 1 && lpTStr[0] == '0')
		base = 8;
	else
		base = 10;

	ULONG	ULong = _tcstoul(lpTStr, &endptr, base);
	if (*endptr != '\0') {
		_ftprintf(stderr, _TEXT("strtoul(\"%s\") fails!\n"), lpTStr);
		return -1;
	}

	*lpULong = ULong;
	return 0;
}
////////////////////////////////////////////////////////////////////////////////
int APIENTRY
_tWinMain(
	HINSTANCE	hInstance,
	HINSTANCE	hPrevInstance,	// argc
	LPTSTR		lpCmdLine,	// argv
	int		nCmdShow
)
{
////////////////////////////////////////////////////////////////////////////////
	UINT	i;
	ULONG	ULong;

	UINT	argc = (UINT) hPrevInstance;
	LPTSTR	*argv = (LPTSTR *) lpCmdLine;
	for (i = 0; i < argc; ++i) {
		_tprintf(
			_TEXT("argv[%d] = \"%s\"\n")
			,
			i,
			argv[i]
		);
	}

	if (argc < 3) {
		_ftprintf(
			stderr,
			_TEXT("Usage: %s NClients Port [ServerAddress [NMessages [LMilliSeconds]]]\n")
			,
			argv[0]
		);
		return 1;
	}
	if (StrToULong(argv[1], &ULong) != 0)
		return 1;
	g__N = (USHORT) ULong;
	if (StrToULong(argv[2], &ULong) != 0)
		return 1;
	USHORT	UPort = (USHORT) ULong;
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
//                                                                    //
	LPTSTR	lpAddress = TEXT("127.0.0.1");
	if (argc >= 4)
		lpAddress = argv[3];

	g__M = 16;
	if (argc >= 5) {
		if (StrToULong(argv[4], &ULong) != 0)
			return 1;
		g__M = (USHORT) ULong;
	}

	LARGE_INTEGER	liDueTime;
	liDueTime.QuadPart = 0;
	LONG	lPeriod = 100;	// in milliseconds
	if (argc >= 6) {
		if (StrToULong(argv[5], &ULong) != 0)
			return 1;
		lPeriod = (LONG) ULong;
	}
	if (!(g__hTimer = CreateWaitableTimer(NULL, FALSE, NULL))) {
		__SUICIDE__;
	}
	if (!SetWaitableTimer(
		g__hTimer,
		&liDueTime,
		lPeriod,
		NULL,
		NULL,
		TRUE
	)) {
		__SUICIDE__;
	}
//                                                                    //
////////////////////////////////////////////////////////////////////////
 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_GMWINIOCPC, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

////////////////////////////////////////////////////////////////////////
//                                                                    //
	WSADATA	wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData)) {
		__SUICIDE__;
	}
//                                                                    //
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
//                                                                    //
#if 1
	PCHAR	lpAddr = "localhost";
#else
	PCHAR	lpAddr = "127.0.0.1";
#endif
	struct hostent*	lpHostEnt;
	lpHostEnt = gethostbyname(lpAddr);
	if (lpHostEnt) {
		printf("h_name: %s\n", lpHostEnt->h_name);
		INT	i;
		PCHAR	lpStr;
		printf("h_aliases:\n");
		for (i = 0, lpStr = lpHostEnt->h_aliases[0]; lpStr; lpStr = lpHostEnt->h_aliases[++i])
			printf("\th_alias: %s\n", lpStr);
		printf("h_addrtype: %hd\n", lpHostEnt->h_addrtype);
		printf("h_length: %hd\n", lpHostEnt->h_length);
		printf("h_addr_list:\n");
		for (i = 0, lpStr = lpHostEnt->h_addr_list[0]; lpStr; lpStr = lpHostEnt->h_addr_list[++i]) {
			struct in_addr	sin_addr;
			sin_addr.s_addr = *((PINT) lpStr);
			printf("\th_addr: %s(0x%08X)\n", inet_ntoa(sin_addr), *((PINT) lpStr));
		}
	}

//	if (inet_addr(lpAddr) != INADDR_NONE) {
//		printf("hello, world\n");
//	}
	printf("hello, world: 0x%08X\n", inet_addr(lpAddr));
//                                                                    //
////////////////////////////////////////////////////////////////////////

	srand((unsigned) time(NULL) * (0x1219 + 0x1129) * 1024);
	g__lpHx	= (LPTCPPBUF) malloc(N__CLIENT * g__N * g__M * sizeof(TCPPBUF));
	g__lpHy	= (LPTCPPBUF) malloc(N__CLIENT * g__N * g__M * sizeof(TCPPBUF));
	for (i = 0; i < N__CLIENT * g__N * g__M; ++i) {
		g__lpHx[i].gmhTCP		= (gmHTCP) 0;

		g__lpHx[i].MTCPMsg.Header.hType	= (USHORT) (rand() * 15 / RAND_MAX);		// 0 .. 15
		g__lpHx[i].MTCPMsg.Header.cbData	= (USHORT) (rand() * (MAX_TCPMSGSIZE - 4) / RAND_MAX + 4);	// 0 .. MAX_SIZE
		g__lpHx[i].MTCPMsg.pbData	= NULL;
	}

////////////////////////////////////////////////////////////////////////
//                                                                    //
	gmCONN	gmConn[N__CLIENT];
	for (i = 0; i < N__CLIENT; ++i) {
		gmConn[i].TCPAddr	= lpAddress;
		gmConn[i].TCPPort	= UPort + 3 * i;
		gmConn[i].MaxConn	= g__N;
	}
	g__lpConnV = gmConn;

	int	gmStatus = 0xCDCDCDCD;	// 임의의 값으로...
	g__lpTCPClient = new gmTCPClient(
		gmConn,
		N__CLIENT,
		MAX_TCPMSGSIZE,
		1,
#ifdef __gmTLS__
		TEXT("TLS"),
#endif	// __gmTLS__
		&gmStatus
	);
	if (!g__lpTCPClient || gmStatus != 0) {
		if (g__lpTCPClient)
			delete g__lpTCPClient;
		return 2;
	}
	g__lpTCPClient->mf__lprintf(TEXT("<gmIocpTcpClient>\n"));
	gmPseudoTCP	*lpgmPseudoTCP = g__lpTCPClient->mf__GetLpPseudoTCP();
	for (i = 0; i < N__CLIENT * g__N * g__M; ++i) {
		g__lpHx[i].gmhTCP = &lpgmPseudoTCP[i / (1 * g__M)];
	}
//                                                                    //
////////////////////////////////////////////////////////////////////////
	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_GMWINIOCPC);

//	// Main message loop:
//	while (GetMessage(&msg, NULL, 0, 0)) 
//	{
//		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
//		{
//			TranslateMessage(&msg);
//			DispatchMessage(&msg);
//		}
//	}
////////////////////////////////////////////////////////////////////////
//                                                                    //
	HANDLE	hXXX[2];
	hXXX[0] = g__hTimer;
#ifdef __gmMESSAGE_IOCP__
	if (!(hXXX[1] = CreateThread(NULL, 0, IocpMessageLoop, g__lpTCPClient->mf__GetMessageIOCPHandle(), 0, NULL))) {
		__SUICIDE__;
	}
#else	// __gmMESSAGE_IOCP__
	hXXX[1] = g__lpTCPClient->mf__GetNotifyEvent();
#endif	// !__gmMESSAGE_IOCP__
	msg.wParam = MessageLoop(hXXX, 2);
//                                                                    //
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
//                                                                    //
	CloseHandle(g__hTimer);
	free(g__lpHx);
	free(g__lpHy);

	g__lpTCPClient->mf__lprintf(TEXT("</gmIocpTcpClient>\n"));
	delete g__lpTCPClient;

	if (WSACleanup()) {
		__SUICIDE__;
	}
//                                                                    //
////////////////////////////////////////////////////////////////////////

	return msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage is only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style		= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC) WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon		= LoadIcon(hInstance, (LPCTSTR)IDI_GMWINIOCPC);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH) (COLOR_WINDOW + 1);
	wcex.lpszMenuName	= (LPCTSTR) IDC_GMWINIOCPC;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HANDLE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	TCHAR szHello[MAX_LOADSTRING];
	LoadString(hInst, IDS_HELLO, szHello, MAX_LOADSTRING);

	switch (message) 
	{
		case WM_COMMAND:
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			switch (wmId)
			{
				case IDM_ABOUT:
				   DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
				   break;
				case IDM_EXIT:
				   DestroyWindow(hWnd);
				   break;
				default:
				   return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
////////////////////////////////////////////////////////////////////////
//                                                                    //
		case WM_MOUSEMOVE:
			if (g__bVerbose) {
				g__lpTCPClient->mf__printf(
					TEXT("WM_MOUSEMOVE: (%u, %u)\n")
					,
					LOWORD(lParam), HIWORD(lParam)
				);
			}
			break;
		case WM_KEYDOWN:
			if (g__bVerbose) {
				g__lpTCPClient->mf__printf(
					TEXT("WM_KEYUP: 0x%08X\n")
					,
					wParam
				);
			}
			if (wParam == 'X') {
				DestroyWindow(hWnd);
			}
			break;
		case WM_KEYUP:
			if (g__bVerbose) {
				g__lpTCPClient->mf__printf(
					TEXT("WM_KEYUP: 0x%08X\n")
					,
					wParam
				);
			}
			if (wParam == 'V')
				g__bVerbose ^= true;
			if (wParam == 'K')
				g__lpTCPClient->mf__flushAll();
			break;
//                                                                    //
////////////////////////////////////////////////////////////////////////
		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			// TODO: Add any drawing code here...
			RECT rt;
			GetClientRect(hWnd, &rt);
			DrawText(hdc, szHello, _tcslen(szHello), &rt, DT_CENTER);
			EndPaint(hWnd, &ps);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

// Mesage handler for about box.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
				return TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
			{
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;
	}
    return FALSE;
}

////////////////////////////////////////////////////////////////////////
//                                                                    //
// From Plastform SDK Documentation
int
MessageLoop(
	HANDLE*	lphObjects,	// handles that need to be waited on
	int	cObjects	// number of handles to wait on
)
{
	// The message loop lasts until we get a WM_QUIT message,
	// upon which we shall return from the function.
	while (TRUE) {
		// block-local variable
		DWORD	result;
		MSG	msg;

		// Read all of the messages in this next loop,
		// removing each message as we read it.
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			// If it is a quit message, exit.
			if (msg.message == WM_QUIT)
				return msg.wParam;
////////////////////////////////////////////////////////////////////////////////
/*
 * Messages sent by PostThreadMessage are not associated with a window.
 * Messages that are not associated with a window cannot be dispatched
 * by the DispatchMessage function. 
 */
if (!msg.hwnd && msg.message == WM_USER) {
}
////////////////////////////////////////////////////////////////////////////////
			// Otherwise, dispatch the message.
			DispatchMessage(&msg);
		}	// End of PeekMessage while loop.

		// Wait for any message sent or posted to this queue
		// or for one of the passed handles be set to signaled.
		result = MsgWaitForMultipleObjects(
			cObjects, lphObjects,
       			FALSE,
			INFINITE,
			QS_ALLINPUT
		);

		// The result tells us the type of event we have.
		if (result == WAIT_OBJECT_0 + cObjects) {
			// New messages have arrived.
			// Continue to the top of the always while loop to
			// dispatch them and resume waiting.
			continue;
		} else if (result == WAIT_OBJECT_0) {
			// One of the handles became signaled.
//			DoStuff (result - WAIT_OBJECT_0);
			run();
		} else if (result == WAIT_OBJECT_0 + 1) {
#ifdef __gmMESSAGE_IOCP__
			DWORD	dwThreadExitCode;
			if (!GetExitCodeThread(lphObjects[1], &dwThreadExitCode)) {
				dwThreadExitCode = (DWORD) -1;
			}
			PostQuitMessage(dwThreadExitCode);
#else	// __gmMESSAGE_IOCP__
			ClientRun();
#endif	// !__gmMESSAGE_IOCP__
		} else {
			__SUICIDE__;
		}
	}
}

INT
run(
	VOID
)
{
	static UINT	ix = 0;
	static UINT	N = 0;

	UINT	iN;
	UINT	n;
	LPTCPPBUF	lpTCPPBuf;
	while (1) {
		if (N == N__CLIENT * g__N) {
			printf("hello, world: ix = %u\n", ix);
			if (!CancelWaitableTimer(g__hTimer)) {
				__SUICIDE__;
			}
			g__lpTCPClient->mf__flushAll();
			return 0;
		}
		// select iN (0 .. N__CLIENT * g__N)
		iN = N__CLIENT * rand() * g__N / RAND_MAX;
		if (iN >= N__CLIENT * g__N) {
			continue;
		}
		lpTCPPBuf = &g__lpHx[iN * g__M];

		n = (UINT) lpTCPPBuf->MTCPMsg.pbData;
		//
		if (n == g__M) {
			// already done with this index
			lpTCPPBuf->MTCPMsg.pbData = (PBYTE) ~0;
			++N;
			continue;
		}
		if (lpTCPPBuf->MTCPMsg.pbData == (PBYTE) ~0)
			continue;

		//
{
	BYTE		buf[MAX_TCPMSGSIZE];
	TCPPBUF&	myTCPPBuf = g__lpHx[iN * g__M + n];
	myTCPPBuf.MTCPMsg.pbData = buf;
	memset(buf, myTCPPBuf.MTCPMsg.Header.hType < 10 ? '0' + myTCPPBuf.MTCPMsg.Header.hType : 'A' + (myTCPPBuf.MTCPMsg.Header.hType - 10), myTCPPBuf.MTCPMsg.Header.cbData);
	*((PUINT) buf) = ix;

////////////////////////////////////////////////////////////////////////////////
{
	INT	iReturnVal = gmPostMessage(
		myTCPPBuf.gmhTCP,
		&myTCPPBuf.MTCPMsg
	);
	switch (iReturnVal) {
	case TCPPOSTMSG__NOT_ESTABLISHED:
		break;
	case TCPPOSTMSG__OK:
		++ix;
#if 1
		g__lpTCPClient->mf__fprintf(g__lpTCPClient->m__CONERR,
			TEXT("[0x%08X >] hType = %u, cbData = %u, seq # = %u\n")
			,
			myTCPPBuf.gmhTCP,
			myTCPPBuf.MTCPMsg.Header.hType,
			myTCPPBuf.MTCPMsg.Header.cbData,
			*((PUINT) buf)
		);
#endif
		lpTCPPBuf->MTCPMsg.pbData = (PBYTE) (n + 1);
		break;
	case TCPPOSTMSG__SEND_OK_IMMEDIATELY_BUT:
	case TCPPOSTMSG__SEND_OK_BUT:
	case TCPPOSTMSG__WSASEND_IN_PROGRESS_NOT_ENOUGH:
		break;
	case TCPPOSTMSG__ERROR:
		if (
			g__lpTCPClient->mf__GetWSALastErrorCode() == WSAECONNRESET
				||
			g__lpTCPClient->mf__GetWSALastErrorCode() == WSAECONNABORTED
		) {
			g__lpTCPClient->mf__fprintf(g__lpTCPClient->m__CONERR,
				TEXT("gmPseudoTCP::mf__PostMessage: WSAECONNRESET\n")
			);
		} else {
			// WSASend의 실패
			__SUICIDE__;
		}
		break;
	default:
		__SUICIDE__;
		break;
	}
}
////////////////////////////////////////////////////////////////////////////////
}
		break;
	}

	return 0;
}
//                                                                    //
////////////////////////////////////////////////////////////////////////

#ifdef __gmMESSAGE_IOCP__
DWORD WINAPI
IocpMessageLoop(
	HANDLE	hIOCP
)
{
	BOOL		bStatus;
	DWORD		dwMessage;	// "받는" 큐에 있는 메시지의 갯수
	PDWORD		pdwUID;
//	DWORD		dwUID;		// 서버의 경우에는 0이 아니고, 클라이언트는 반드시 0이다.
	gmPseudoTCP	*gmhTCP;
	while (1) {
		bStatus = GetQueuedCompletionStatus(
				hIOCP,				//	HANDLE		CompletionPort
				&dwMessage,			//	LPDWORD		lpNumberOfBytes
				(PULONG_PTR) &gmhTCP,		//	PULONG_PTR	lpCompletionKey
				(LPOVERLAPPED *) &pdwUID,	//	LPOVERLAPPED	*lpOverlapped
				INFINITE			//	DWORD		dwMilliseconds
		);

		if (!bStatus) {
			// 자세한 것은 gmWinIocpS의 해당 함수(IocpMessageLoop)를 보라.
			__SUICIDE__;
		}

		////////////////////////////////////////////////////////////////
		// 성공적으로 IO Completion포트로부터 처리된 결과를 얻었다.
		//	(1) 리턴값이 0이 아니다.
		//	(2) dwMessage, gmhTCP은 적절하게 설정된다.
		//	(3) pdwUID값도 적절하게 설정되었으므로
		//	pdwUID가 가리키는 값 즉 dwUID값이 의미가 있다.
		////////////////////////////////////////////////////////////////
		DWORD	dwUID = *pdwUID;

		g__lpTCPClient->mf__fprintf(g__lpTCPClient->m__CONERR,
			TEXT("hTCP: %p, # = %u, dwUID = 0x%08X\n"), gmhTCP, dwMessage, dwUID
		);

		if (dwMessage & 0x80000000L) {
			// [경우 1] OOB정보이다.
			DWORD	dwOOB = dwMessage & 0xFFFF;
			// 현재 0x0001하나밖에 없다.
			switch (dwOOB) {
			case 0x0001:	// 클라이언트와의 연결이 "강제로" 끊긴 경우이다.
					// "정상적"으로 끊긴 경우에는 메시지를 가져올 때 알 수 있다. (TCPBUF__NOT_ESTABLISHED)
				if (gmhTCP->mf__bDisconnect()) {
					if (gmhTCP->mf__closeSend() != 0) {
						__SUICIDE__;
					}
#if 1	// CLIENT
					if (g__lpTCPClient->mf__ConnectionN() == 0) {
						return 1024;
					}
#endif	// CLIENT
				} else {
					__SUICIDE__;
				}
				break;
			default:
				__SUICIDE__;
			}
			continue;
		}

		// [경우 2] "받는" 큐에 dwMessage개의 메시지가 있는 경우이다.
		BYTE		bData[MAX_TCPMSGSIZE];
		MTCPMSG		MTCPMsg;
		MTCPMsg.pbData	= bData;
		////////
		USHORT&	hType	= MTCPMsg.Header.hType;
		USHORT&	cbData	= MTCPMsg.Header.cbData;

		BOOL	bLoopContinue = TRUE;
		DWORD	N = 0;
		do {
			// 서버는 클아이언트와의 연결이 확정되어야 클라이언트로부터 스프림을 읽는다.
			// 다음은 이를 설정하는 가장 간단한 예이다.
			// 실제로는 다른 절차를 통해서 설정해야 한다.
			// 예를 들어, IP가 블랙리스트에 있는 지 어떤 지를 조사할 수 있다.
			gmhTCP->mf__SetPseudoTCPStatusEstablished();

			// 초기화
			cbData = MAX_TCPMSGSIZE;	// [IN/OUT]이므로 반드시 설정해야 한다.
			INT	iReturnVal = gmhTCP->mf__GetMessage(&MTCPMsg);

			// 연결이 끊어지지 않은 경우이다.
			switch (iReturnVal) {
			case TCPBUF__NOT_ESTABLISHED:
				// 서버와 해당하는 클라이언트와의 연결이 이미 끊어졌다. (정상적으로)
				if (gmhTCP->mf__closeSend() != 0) {
					__SUICIDE__;
				}
				bLoopContinue = FALSE;
				break;
			case TCPBUF__OK:
////////////////////////////////////////////////////////////////////////////////
// 읽어들인 데이터를 가지고 어떤 일을 하자.
// 여기서는 단지 에코(ECHO)만 할 뿐이다.
{
#if 1
	////////////////////////////////////////
	// 읽어들인 데이터의 정보를 읽는다.
	g__lpTCPClient->mf__fprintf(g__lpTCPClient->m__CONERR,
		TEXT("[0x%08X <] hType = %u, cbData = %u, seq # = %u (%u)\n")
		,
		&gmhTCP,
		hType,
		cbData,
		*((PUINT) bData)
		, gmhTCP->mf__GetRecvQueueSize()
	);
	////////////////////////////////////////
#endif
#if 1	// SERVER
#else	// SERVER
	////////////////////////////////////////
	// 클라이언트가 보낸 데이터인코딩을 알기때문에 이를 확인한다.
	{
		BYTE	cbuf[MAX_TCPMSGSIZE];
		memset(cbuf, hType < 10 ? '0' + hType : 'A' + (hType - 10), cbData);
		*((PUINT) cbuf) = *((PUINT) bData);
		if (memcmp(bData, cbuf, cbData) != 0) {
			__SUICIDE__;
		}
	}
	////////////////////////////////////////
#endif	// !SERVER(= CLIENT)
#if 1	// SERVER
#else	// SERVER
	////////////////////////////////////////
	// 데이터를 에코(ECHO)한다.
	switch (gmPostMessage(gmhTCP, &TCPBuf)) {	// gmPseudoTCP::mf__PostMsg의 리턴값과 같다.
	case TCPPOSTMSG__NOT_ESTABLISHED:
		// 서버와 해당하는 클라이언트와의 연결이 이미 끊어졌다. (정상적으로)
		if (gmhTCP->mf__closeSend()) {
			__SUICIDE__;
		}
		bLoopContinue = FALSE;
		break;
	case TCPPOSTMSG__OK:
		// 큐에 잘 넣었다.
		// 필요하다면 어느 정도 찬 큐를 비우기도 했다.

		//

		break;
	case TCPPOSTMSG__SEND_OK_IMMEDIATELY_BUT:
	case TCPPOSTMSG__SEND_OK_BUT:
		// 어느 정도 큐가 차서 이 큐를 비우는 데에는 성공했으나
		// 나머지 큐에 여유 공간이 없어서, 큐에 넣을 수 없었다.
		g__lpTCPClient->mf__fprintf(g__lpTCPClient->m__CONERR,
			TEXT("[0x%08X <] hType = %u, cbData = %u, seq # = %u: (TCPPOSTMSG__SEND_OK_IMMEDIATELY_BUT/TCPPOSTMSG__SEND_OK_BUT) Lost!\n")
			,
			&gmhTCP,
			hType,
			cbData,
			*((PUINT) bData)
		);
		break;
	case TCPPOSTMSG__WSASEND_IN_PROGRESS_NOT_ENOUGH:
		// 어느 정도 큐가 차서 이 큐를 비울려고 했으나
		// 이미 진행중인 WSASend 가 있어서, 큐에 넣을 수 없었다.
		g__lpTCPClient->mf__fprintf(g__lpTCPClient->m__CONERR,
			TEXT("[0x%08X <] hType = %u, cbData = %u, seq # = %u: (TCPPOSTMSG__WSASEND_IN_PROGRESS_NOT_ENOUGH) Lost!\n")
			,
			&gmhTCP,
			hType,
			cbData,
			*((PUINT) bData)
		);
		break;
	case TCPPOSTMSG__ERROR:
		if (
			g__lpTCPClient->mf__GetWSALastErrorCode() == WSAECONNRESET
				||
			g__lpTCPClient->mf__GetWSALastErrorCode() == WSAECONNABORTED
		) {
			// 이미 연결이 끊어졌다. (비정상적으로)
			g__lpTCPClient->mf__fprintf(g__lpTCPClient->m__CONERR,
				TEXT("gmPseudoTCP::mf__PostMessage: WSAECONNRESET\n")
			);
		} else {
			// WSASend의 실패
			__SUICIDE__;
		}
		break;
	default:	// 이럴 수가 있나?
		__SUICIDE__;
		break;
	}
	////////////////////////////////////////
#endif	// !SERVER(= CLIENT)
}
////////////////////////////////////////////////////////////////////////////////
				break;
			case TCPBUF__NO_HEADER:
			case TCPBUF__PARTIAL_DATA:
				// 다음에 가져올 수 밖에...
				__SUICIDE__;
				break;
			case TCPBUF__NOT_ENOUGH:
				// MTCPMSG.Header.cbData값을 제대로 설정했나?
				// 아니면 들어오는 데이터가 제대로 된 것인가?
				__SUICIDE__;
				break;
			case TCPBUF__OK_BUT:
				// 정지된 "받는" 큐를 활성화할 때 문제가 있었다.
				// 해당 연결을 끊어야 한다.
				{
					DWORD	dwLastErrorCode = g__lpTCPClient->mf__GetWSALastErrorCode();
					if (dwLastErrorCode == WSAECONNRESET) {
						gmhTCP->mf__disconnect();
						bLoopContinue = FALSE;
					} else {
						__SUICIDE__;
					}
				}
				break;
			case TCPBUF__INTERNAL_ERROR:
				// 정지된 "받는" 큐를 활성화할 때 문제가 있었다.
				__SUICIDE__;
			default:
				// 이럴 수가???
				__SUICIDE__;
				break;
			}
		} while (++N != dwMessage && bLoopContinue);
	}

	return 0;
}
#else	// __gmMESSAGE_IOCP__
VOID
ClientRun(
	VOID
)
{
	static UINT	iSeq = 0;

	INT	iReturnVal = 0;

	BYTE	buf[MAX_TCPMSGSIZE];
	TCPBUF	myTCPBuf;
	myTCPBuf.pbData	= buf;
	////////
	USHORT&	hType	= myTCPBuf.Header.hType;
	USHORT&	cbData	= myTCPBuf.Header.cbData;

	UINT	NMsgsProcessed;
	do {
		NMsgsProcessed = 0;

		gmPseudoTCP	*gmhTCP;
		g__lpTCPClient->mf__iterReset();
		while ((gmhTCP = g__lpTCPClient->mf__nextTCP())) {
			if (gmhTCP->mf__bDisconnect()) {
				if (gmhTCP->mf__closeSend() != 0) {
					__SUICIDE__;
				}
				if (g__lpTCPClient->mf__ConnectionN() == 0) {
					PostQuitMessage(0);
				}
				continue;
			}

			myTCPBuf.Header.cbData = MAX_TCPMSGSIZE;
			switch (gmhTCP->mf__GetMessage(&myTCPBuf)) {
			case TCPBUF__OK:
				{
					UINT	ix = *((PUINT) buf);
					if (ix != iSeq) {
						UINT	iy = ix;
					}
					++iSeq;
				}

				g__lpTCPClient->mf__fprintf(g__lpTCPClient->m__CONERR,
					TEXT("[0x%08X <] hType = %u, cbData = %u, seq # = %u (%u)\n")
					,
					&gmhTCP,
					myTCPBuf.Header.hType,
					myTCPBuf.Header.cbData,
					*((PUINT) buf)
					, gmhTCP->mf__GetRecvQueueSize()
				);
				++NMsgsProcessed;
				break;
			case TCPBUF__NO_HEADER:
			case TCPBUF__PARTIAL_DATA:
				// 다음에 가져올 수 밖에...
				break;
			case TCPBUF__NOT_ENOUGH:
				// TCPBUF.Header.cbData값을 제대로 설정했나?
				// 아니면 들어오는 데이터가 제대로 된 것인가?
				__SUICIDE__;
				break;
			default:
				// 이럴 수가???
				__SUICIDE__;
				break;
			}
		}
	} while (NMsgsProcessed);
}
#endif	// !__gmMESSAGE_IOCP__
