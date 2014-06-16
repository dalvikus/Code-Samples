// gmWinIocpS.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "resource.h"
////////////////////////////////////////////////////////////////////////
//                                                                    //
// 서버
#define MAX_TCPMSGSIZE	1024

#define	N__SERVER	1
#include <gmTCP/gmTCP.h>

gmTCPServer*	g__lpTCPServer = NULL;
bool		g__bVerbose = false;
gmCONN	*g__lpConnV = NULL;

INT
MessageLoop(
	HANDLE*	lphObjects,	// handles that need to be waited on
	int	cObjects	// number of handles to wait on
);
#ifdef __gmMESSAGE_IOCP__
DWORD WINAPI
IocpMessageLoop(
	HANDLE	hIOCP
);
#else	// __gmMESSAGE_IOCP__
VOID
ServerRun(
	VOID
);
#endif	// !__gmMESSAGE_IOCP__

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
	_ftprintf(stderr, TEXT("---- The End of The World ----\n"));

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
int APIENTRY _tWinMain(
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
			_TEXT("Usage: %s MaxClients Port\n")
			,
			argv[0]
		);
		return 1;
	}
	if (StrToULong(argv[1], &ULong) != 0)
		return 1;
	UINT	UN = (UINT) ULong;
	if (StrToULong(argv[2], &ULong) != 0)
		return 1;
	USHORT	UPort = (USHORT) ULong;
////////////////////////////////////////////////////////////////////////////////
 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_GMWINIOCP, szWindowClass, MAX_LOADSTRING);
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
	gmCONN	gmConn[N__SERVER];
	for (i = 0; i < N__SERVER; ++i) {
		gmConn[i].TCPPort	= UPort + 3 * i;
		gmConn[i].MaxConn	= UN;
	}
	g__lpConnV = gmConn;
	int	gmStatus = 0xCDCDCDCD;	// 임의의 값으로...
	g__lpTCPServer = new gmTCPServer(
		gmConn,
		N__SERVER,
		MAX_TCPMSGSIZE,
		0,
#ifdef __gmTLS__
		TEXT("TLS"),
#endif	// __gmTLS__
		&gmStatus
	);
	if (!g__lpTCPServer || gmStatus != 0) {
		__SUICIDE__;
	}
	g__lpTCPServer->mf__lprintf(TEXT("<gmIocpTcpServer>\n"));
//                                                                    //
////////////////////////////////////////////////////////////////////////

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_GMWINIOCP);

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
	HANDLE	hXXX;
#ifdef __gmMESSAGE_IOCP__
	if (!(hXXX = CreateThread(NULL, 0, IocpMessageLoop, g__lpTCPServer->mf__GetMessageIOCPHandle(), 0, NULL))) {
		__SUICIDE__;
	}
#else	// __gmMESSAGE_IOCP__
	hXXX = g__lpTCPServer->mf__GetNotifyEvent();
#endif	// !__gmMESSAGE_IOCP__
	msg.wParam = MessageLoop(&hXXX, 1);
//                                                                    //
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
//                                                                    //
	g__lpTCPServer->mf__lprintf(TEXT("</gmIocpTcpServer>\n"));
	delete g__lpTCPServer;

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
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon		= LoadIcon(hInstance, (LPCTSTR)IDI_GMWINIOCP);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH) (COLOR_WINDOW + 1);
	wcex.lpszMenuName	= (LPCTSTR) IDC_GMWINIOCP;
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
				g__lpTCPServer->mf__printf(
					TEXT("WM_MOUSEMOVE: (%u, %u)\n")
					,
					LOWORD(lParam), HIWORD(lParam)
				);
			}
			break;
		case WM_KEYDOWN:
			if (g__bVerbose) {
				g__lpTCPServer->mf__printf(
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
				g__lpTCPServer->mf__printf(
					TEXT("WM_KEYUP: 0x%08X\n")
					,
					wParam
				);
			}
			if (wParam == 'V')
				g__bVerbose ^= true;
			if (wParam == 'K')
				g__lpTCPServer->mf__flushAll();
			if (wParam == 'T')
				(VOID) g__lpTCPServer->mf__HeartBeat();
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
				return 1;
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
//////			run();
#ifdef __gmMESSAGE_IOCP__
			DWORD	dwThreadExitCode;
			if (!GetExitCodeThread(lphObjects[1], &dwThreadExitCode)) {
				dwThreadExitCode = (DWORD) -1;
			}
			PostQuitMessage(dwThreadExitCode);
#else	// __gmMESSAGE_IOCP__
			ServerRun();
#endif	// !__gmMESSAGE_IOCP__
		} else {
			__SUICIDE__;
		}	// End of else clause.
	}	// End of the always while loop.
}	// End of function.

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
				hIOCP,				//	HANDLE		CompletionPort
				&dwMessage,			//	LPDWORD		lpNumberOfBytes
				(PULONG_PTR) &gmhTCP,		//	PULONG_PTR	lpCompletionKey
				(LPOVERLAPPED *) &pdwUID,	//	LPOVERLAPPED	*lpOverlapped
				INFINITE			//	DWORD		dwMilliseconds
		);
#if 0	// GetQueuedCompletionStatus함수의 처리에 관한 다음 내용을 보라.
////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////
		// 아래에 리턴값의 의미를 GetQueuedCompletionStatus에 대한
		// Platform SDK설명서의 내용과 함께 적었다.
		////////////////////////////////////////////////////////////////

/*
 * If the function dequeues a completion packet for a successful I/O operation from the completion port,
 * the return value is nonzero. The function stores information in the variables
 * pointed to by the lpNumberOfBytesTransferred, lpCompletionKey, and lpOverlapped parameters.
 */
		// IO Completion포트에서 처리된 결과를 성공적으로 가져왔다면 리턴값은 0이 아니다.
		// 그리고 그 결과들은 lpNumberofBytes, lpCompletionKey, lpOverlapped가 가리키는 곳에 저장한다.
		//	여기서 사용되는 변수로 다시 말하면 다음과 같다.
		//	(1) 리턴값이 0이 아니다.
		//	(2) dwMessage, gmhTCP은 적절하게 설정된다.
		//	(3) pdwUID값도 적절하게 설정되었으므로
		//	pdwUID가 가리키는 값 즉 dwUID값이 의미가 있다.

		if (!bStatus) {
			// 문제가 생겼다.
			// 아래에서 좀 더 자세히 그 상황을 알아보자.
		}
/*
 * If *lpOverlapped is NULL and the function does not dequeue a completion
 * packet from the completion port, the return value is zero.
 * The function does not store information in the variables pointed to
 * by the lpNumberOfBytes and lpCompletionKey parameters.
 * To get extended error information, call GetLastError.
 * If the function did not dequeue a completion packet because the wait timed out,
 * GetLastError returns WAIT_TIMEOUT.
 */
		// lpOverlapped이 가리키는 값이 NULL이고, 이 함수가 IO Completion포트로부터 처리된 결과를
		// 가져오지 못했다면 리턴값은 0이다. 또한 lpCompletionKey, lpOverlapped가 가리키는 곳에
		// 어떤 값도 저장하지 않는다. 이 경우에 에러코드를 얻으려면 GetLastError함수를 호출한다.
		// (한 예로) 이 함수를 호출할 때, dwMilliseconds값은 INFINITE로 하지 않아서
		// 타임아웃값을 설정했을 때, 그 시간안에 어떤 결과도 얻지 못했을 때 이런 결과가 나타난다.
		// 여기서 사용된 변수로 말하면 다음과 같다.
		//	(1) 리턴값이 0이다.
		//	(2) pdwUID값은 0으로 설정되었다. 즉 pdwUID가 가리키는 값을 읽으면 안된다.
		//	(3) dwMessage나 gmhTCP값은 변하지 않는다.
		if (!bStatus && !pdwUID) {
			DWORD	dwLastErrorCode = GetLastError();
			//	WAIT_TIMEOUT	258L
			if (dwLastErrorCode != WAIT_TIMEOUT) {
				// 다른 어떤 경우가 있을까?
#ifdef _DEBUG
				__SUICIDE__;
#else	// _DEBUG
				// 로그를 남기고 무시한다. 그래도 궁금하다.
#endif	// !_DEBUG
			}
			continue;
		}

/*
 * If *lpOverlapped is not NULL and the function dequeues a completion
 * packet for a failed I/O operation from the completion port,
 * the return value is zero. The function stores information in the variables
 * pointed to by lpNumberOfBytes, lpCompletionKey, and lpOverlapped.
 * To get extended error information, call GetLastError.
 */
		// lpOverlapped이 가리키는 값이 0이 아니며,
		// 이 함수가 실패한 IO Completion포트에서 결과를 가져왔을 때의
		// 리턴값은 0이다. 그렇지만 lpNuberOfByters, lpCompletionKey 그리고
		// lpOverlapped들이 가리키는 값은 설정된다.
		// 에러코드를 얻기 위하여 GetLastError함수를 호출한다.
		//	여기서 사용되는 변수로 다시 말하면 다음과 같다.
		//	(1) 리턴값은 0이다.
		//	(2) dwMessage, gmhTCP 값이 설정된다.
		//	(3) pdwUID가 가리키는 값도 설정되므로 즉 dwUID값도 설정된다.
		if (!bStatus && pdwUID) {
			// IO Completion포트와 관련된 핸들이 네트워크 소켓핸들일 때,
			// 연결된 네크워크를 강제로 끊을 때가 이런 경우이다.
			// 정상적으로 네트워크 연결을 끊으면 리턴값은 0이 아니다.
			// 이 예는 gmTCPServer::mf__IOCPWork함수에 있다.
			// 그런데 여기서 사용되는 것처럼 실제로 IO와 아무런 상관이 없는 핸들의 경우에는
			// 어떤 상황이 이 경우에 해당하는 지 모르겠다.
#ifdef _DEBUG
			__SUICIDE__;
#else	// _DEBUG
				// 로그를 남기고 무시한다. 그래도 궁금하다.
#endif	// !_DEBUG
			continue;
		}

/*
 * If a socket handle associated with a completion port is closed,
 * GetQueuedCompletionStatus returns ERROR_SUCCESS,
 * with lpNumberOfBytes equal zero.
 */
		// 이 함수에 사용되는 IO Completion포트에 관련된 핸들이,
		// 네트워크에 사용되는 소켓핸들이고, 그 소켓이 닫혔다면
		// 이 함수는 ERROR_SUCCESS(0)을 리턴하고, lpNumberOfBytes가 가리키는 값은 0이다.
		//	여기서 사용되는 변수로 다시 말하면 다음과 같다.
		//	(1) 리턴값은 ERROR_SUCCESS 즉 0이다.
		//	(2) dwMessage = 0이다.
		//	Q. 소켓에 대한 핸들을 IO Completion포트에 사용할 수 있다.
		//	하지만 소켓에다 데이터를 쓰고, 소켓으로부터 데이터를 읽은 때에는
		//	WSASend, WSARecv를 사용한다. 만약 "정상적"으로 상대편에서 연결을 끊으면
		//	WSARecv의 결과는 리턴된다. 물론 이 경우에 lpNuberOfBytes가 가리키는 값은 0이다.
		//	하지만 이 함수의 리턴값은 0이 아니다.
		//	마찬가지로 gmTCPServer::mf__IOCPWork함수를 보라.
		//	그러면 여기서 말하는 소켓과 닫혔다는 말과 리턴값이 0이다라는 말은
		//	어떤 상황에 해당될까?
		if (bStatus == ERROR_SUCCESS && !dwMessage) {
			// 아직 이런 상황을 찾지 못했다.
#ifdef _DEBUG
			__SUICIDE__;
#else	// _DEBUG
				// 로그를 남기고 무시한다. 그래도 궁금하다.
#endif	// !_DEBUG
			continue;
		}

		// 여기서 사용되는 핸들이 직접적인 IO와 아무런 관련이 없으므로,
		// 위의 어떤 에러상황(리턴값 = 0)을 관찰되지 않는다.
		// 단, dwMilliseconds를 INFINITE로 설정하지 않은 경우를 제외하고.
		// 따라서 dwMilliseconds를 INFINITE로 설정했을 때,
		// 리턴값이 0인 경우는 ALL OR NOTHING정책에 따라서 판단하면 될 것이다.
		//	(1) 이 쓰레드를 끝낼 것인가 즉 프로그램을 종료시킬 것인가
		//	(2) 아니면 그냥 무시할 것인가
		// 여기서는 (1)의 경우를 따르겠다! 즉
		//	if (!bStatus) {
		//		// 모든 경우를 망라해서...
		//		RomeoMustDie();
		//	}
		//
		//	// 리턴값이 0이 아닌 경우이다.
		//	// 모든 값(dwMessage, gmhTCP, pdwUID)들이 의미가 있다.
////////////////////////////////////////////////////////////////////////////////
#else	// 다음 정책에 따른다. 자세한 것은 바로 위의 내용을 보라.
		if (!bStatus) {
			__SUICIDE__;
		}
#endif

		////////////////////////////////////////////////////////////////
		// 성공적으로 IO Completion포트로부터 처리된 결과를 얻었다.
		//	(1) 리턴값이 0이 아니다.
		//	(2) dwMessage, gmhTCP은 적절하게 설정된다.
		//	(3) pdwUID값도 적절하게 설정되었으므로
		//	pdwUID가 가리키는 값 즉 dwUID값이 의미가 있다.
		////////////////////////////////////////////////////////////////
		DWORD	dwUID = *pdwUID;

		g__lpTCPServer->mf__fprintf(g__lpTCPServer->m__CONERR,
			TEXT("hTCP: %p, # = 0x%08X, dwUID = 0x%08X\n"), gmhTCP, dwMessage, dwUID
		);
		// 2005/3/25 (EST) - ghkim
		if (gmhTCP->mf__GetUID() != dwUID) {
			// 이전 연결에서 오는 정보이다.
			g__lpTCPServer->mf__fprintf(g__lpTCPServer->m__CONERR,
				TEXT("hTCP: 0x%p, # = 0x%08X, dwUID = 0x%08X(!= 0x%08X): \"일치하지 않은\" 연결에서 오는 정보이다.\n")
				,
				gmhTCP, dwMessage, dwUID, gmhTCP->mf__GetUID()
			);
			__SUICIDE__;
		}

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
		BYTE	bData[MAX_TCPMSGSIZE];
		MTCPMSG	MTCPMsg;
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
#if 0	// mf__PeekMessage를 이용해서...
			UINT	MsgLen;
			INT	iReturnVal = gmhTCP->mf__PeekMessage(&MTCPMsg, &MsgLen);
#else	// mf__GetMessage를 이용해서...
			INT	iReturnVal = gmhTCP->mf__GetMessage(&MTCPMsg);
#endif

			// 연결이 끊어지지 않은 경우이다.
			switch (iReturnVal) {
			case TCPBUF__NOT_ESTABLISHED:
				// 서버와 해당하는 클라이언트와의 연결이 이미 끊어졌다. (정상적으로)
				g__lpTCPServer->mf__fprintf(g__lpTCPServer->m__CONERR,
					TEXT("hTCP: 0x%p, # = 0x%08X, dwUID = 0x%08X: mf__GetMessage: 서버와 해당하는 클라이언트와의 연결이 이미 끊어졌다. (정상적으로)\n")
					,
					gmhTCP,
					dwMessage,
					dwUID
				);
				if (gmhTCP->mf__closeSend() != 0) {
					__SUICIDE__;
				}
				bLoopContinue = FALSE;
				break;
			case TCPBUF__OK:
////////////////////////////////////////////////////////////////////////////////
// 읽어들인 데이터를 가지고 어떤 일을 하자.
// 여기서는 단지 에코(ECHO)만 할 뿐이다.
#if 1
{
#if 1
	////////////////////////////////////////
	// 읽어들인 데이터의 정보를 읽는다.
	g__lpTCPServer->mf__fprintf(g__lpTCPServer->m__CONERR,
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
#endif	// SERVER
	////////////////////////////////////////
	// 데이터를 에코(ECHO)한다.
	switch (gmPostMessage(gmhTCP, &MTCPMsg)) {	// gmPseudoTCP::mf__PostMsg의 리턴값과 같다.
	case TCPPOSTMSG__NOT_ESTABLISHED:
		// 서버와 해당하는 클라이언트와의 연결이 이미 끊어졌다. (정상적으로)
		g__lpTCPServer->mf__fprintf(g__lpTCPServer->m__CONERR,
			TEXT("hTCP: 0x%p, # = 0x%08X, dwUID = 0x%08X: mf__PostMessage: 서버와 해당하는 클라이언트와의 연결이 이미 끊어졌다. (정상적으로)\n")
			,
			gmhTCP,
			dwMessage,
			dwUID
		);
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
		g__lpTCPServer->mf__fprintf(g__lpTCPServer->m__CONERR,
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
		g__lpTCPServer->mf__fprintf(g__lpTCPServer->m__CONERR,
			TEXT("[0x%08X <] hType = %u, cbData = %u, seq # = %u: (TCPPOSTMSG__WSASEND_IN_PROGRESS_NOT_ENOUGH) Lost!\n")
			,
			&gmhTCP,
			hType,
			cbData,
			*((PUINT) bData)
		);
		break;
	case TCPPOSTMSG__ERROR:
		// WSASend에 문제가 있다.
		g__lpTCPServer->mf__fprintf(g__lpTCPServer->m__CONERR,
			TEXT("gmPseudoTCP::mf__PostMessage: TCPPOSTMSG__ERROR: WSASend에 문제가 있다.\n")
		);

		if (
			g__lpTCPServer->mf__GetWSALastErrorCode() == WSAECONNRESET
				||
			g__lpTCPServer->mf__GetWSALastErrorCode() == WSAECONNABORTED
		) {
			// 이미 연결이 끊어졌다. (비정상적으로)
			g__lpTCPServer->mf__fprintf(g__lpTCPServer->m__CONERR,
				TEXT("gmPseudoTCP::mf__PostMessage: WSAECONNRESET\n")
			);
			if (gmhTCP->mf__disconnect()) {
				__SUICIDE__;
			}
			bLoopContinue = FALSE;
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
}
#endif
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
				// WSARecv에 문제가 있다.
				g__lpTCPServer->mf__fprintf(g__lpTCPServer->m__CONERR,
					TEXT("gmPseudoTCP::mf__GetMessage: TCPBUF__OK_BUT: 정지된 \"받는\" 큐를 활성화활 때 문제가 있다.: WSARecv에 문제가 있다.\n")
				);
				//	+ 많은 경우는 상대편에서 연결을 끊었을 때 생긴다.
				// 해당 연결을 끊어야 한다.
				{
					DWORD	dwLastErrorCode = g__lpTCPServer->mf__GetWSALastErrorCode();
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
				// 큐가 가득 찼다는 말인데, 메시지를 적어도 하나 처리하였으므로
				// 이럴 수는 없는데...
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
ServerRun(
	VOID
)
{
	BYTE	buf[MAX_TCPMSGSIZE];
	TCPBUF	TCPBuf;
	TCPBuf.pbData	= buf;
	////////
	USHORT&	hType	= TCPBuf.Header.hType;
	USHORT&	cbData	= TCPBuf.Header.cbData;

	UINT	NMsgsProcessed;
	UINT	NLoops = 0;
	do {
		if (NLoops > 0)	// once
			break;

		NMsgsProcessed = 0;

		gmPseudoTCP*	gmhTCP;
		g__lpTCPServer->mf__iterReset();
		while ((gmhTCP = g__lpTCPServer->mf__nextTCP())) {
			// 서버는 클아이언트와의 연결이 확정되어야 클라이언트로부터 스프림을 읽는다.
			// 다음은 이를 설정하는 가장 간단한 예이다.
			// 실제로는 다른 절차를 통해서 설정해야 한다.
			// 예를 들어, IP가 블랙리스트에 있는 지 어떤 지를 조사할 수 있다.
			gmhTCP->mf__SetPseudoTCPStatusEstablished();

			// 초기화
			cbData = MAX_TCPMSGSIZE;	// [IN/OUT]이므로 반드시 설정해야 한다.
			UINT	msgLen;	// 연결이 끊어지지 않았다면 아래에서 msgLen를 이용해서 큐를 비울 수 있다.
			INT	iReturnVal = gmhTCP->mf__PeekMessage(&TCPBuf, &msgLen);
			if (gmhTCP->mf__bDisconnect()) {	// 연결이 끊어졌다.
				if (gmhTCP->mf__closeSend() != 0) {
					__SUICIDE__;
				}
				continue;
			}

			// 연결이 끊어지지 않은 경우이다.
			// 원하는 작업을 한 후에,
			// msgLen와 gmPseudoTCP::mf__Shift를 이용해서 큐를 비울 수 있다.
			switch (iReturnVal) {
			case TCPBUF__OK:
////////////////////////////////////////////////////////////////////////////////
// 읽어들인 데이터를 가지고 어떤 일을 하자.
// 여기서는 단지 에코(ECHO)만 할 뿐이다.
{
	++NMsgsProcessed;	// 읽은 메시지의 갯수를 하나 증가시킨다.
#if 1
	////////////////////////////////////////
	// 읽어들인 데이터의 정보를 읽는다.
	g__lpTCPServer->mf__fprintf(g__lpTCPServer->m__CONERR,
		TEXT("[0x%08X <] hType = %u, cbData = %u, seq # = %u (%u)\n")
		,
		&gmhTCP,
		hType,
		cbData,
		*((PUINT) buf)
		, gmhTCP->mf__GetRecvQueueSize()
	);
	////////////////////////////////////////
#endif
#if 1
	////////////////////////////////////////
	// 클라이언트가 보낸 데이터인코딩을 알기때문에 이를 확인한다.
	{
		char	cbuf[MAX_TCPMSGSIZE];
		memset(cbuf, hType < 10 ? '0' + hType : 'A' + (hType - 10), cbData);
		*((PUINT) cbuf) = *((PUINT) buf);
		if (memcmp(buf, cbuf, cbData) != 0) {
			__SUICIDE__;
		}
	}
	////////////////////////////////////////
#endif
	////////////////////////////////////////
	// 데이터를 에코(ECHO)한다.
	switch (gmPostMessage(gmhTCP, &TCPBuf)) {	// gmPseudoTCP::mf__PostMsg의 리턴값과 같다.
	case TCPPOSTMSG__NOT_ESTABLISHED:
		// 서버와 해당하는 클라이언트와의 연결이 이미 끊어졌다. (정상적으로)
		if (gmhTCP->mf__Shift(msgLen) != TCPBUF__OK) {
			__SUICIDE__;
		}
		break;
	case TCPPOSTMSG__OK:
		// 큐에 잘 넣었다.
		// 필요하다면 어느 정도 찬 큐를 비우기도 했다.
		if (gmhTCP->mf__Shift(msgLen) != TCPBUF__OK) {
			__SUICIDE__;
		}
		break;
	case TCPPOSTMSG__SEND_OK_IMMEDIATELY_BUT:
	case TCPPOSTMSG__SEND_OK_BUT:
		// 어느 정도 큐가 차서 이 큐를 비우는 데에는 성공했으나
		// 나머지 큐에 여유 공간이 없어서, 큐에 넣을 수 없었다.
	case TCPPOSTMSG__WSASEND_IN_PROGRESS_NOT_ENOUGH:
		// 어느 정도 큐가 차서 이 큐를 비울려고 했으나
		// 이미 진행중인 WSASend 가 있어서, 큐에 넣을 수 없었다.
		break;
	case TCPPOSTMSG__ERROR:
		if (
			g__lpTCPServer->mf__GetWSALastErrorCode() == WSAECONNRESET
				||
			g__lpTCPServer->mf__GetWSALastErrorCode() == WSAECONNABORTED
		) {
			// 이미 연결이 끊어졌다. (비정상적으로)
			g__lpTCPServer->mf__fprintf(g__lpTCPServer->m__CONERR,
				TEXT("gmPseudoTCP::mf__PostMessage: WSAECONNRESET\n")
			);
			if (gmhTCP->mf__Shift(msgLen) != TCPBUF__OK) {
				__SUICIDE__;
			}
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
}
////////////////////////////////////////////////////////////////////////////////
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
		++NLoops;
	} while (NMsgsProcessed);
}
#endif	// !__gmMESSAGE_IOCP__
