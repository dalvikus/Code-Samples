#include <TCHAR.H>
#include <gmCommon/myPRINT.h>
#include <strsafe.h>
#include <time.h>

myPRINT::myPRINT(BOOL bUseConsole)
{
	m__bAllocConsole	= FALSE;
	m__bUseConsole		= bUseConsole;
	INT	iReturnVal = 0;

	m__lpFile = NULL;
	if (m__bUseConsole == FALSE)
		return;

	// STD_INPUT_HANDLE 과 STD_OUTPUT_HANDLE 은 Redirected 됐을 수도
	// 있으므로, 먼저 STD_ERROR_HANDLE 을 얻는다.
	// 만약 성공하면 이미 유용한 콘솔은 있고 그렇지 않다면
	// AllocConsole() 을 이용해서 필요한 콘솔을 생성한다.
	m__CONERR = GetStdHandle(STD_ERROR_HANDLE);
	if (m__CONERR == INVALID_HANDLE_VALUE) {
		(VOID) MessageBox(
			NULL,
			TEXT("GetStdHandle(STD_ERROR_HANDLE) failed!"),
			TEXT("Message Box: Error!"),
			MB_OK
		);
		iReturnVal = -1;
		goto NEXT;
	} else if (m__CONERR == NULL) {
		// 유용한 콘솔이 없다!
		// AllocConsole() 을 이용해서 새로 만들자.
		if (!AllocConsole()) {
			(VOID) MessageBox(
				NULL,
				TEXT("AllocHandle(VOID) failed!"),
				TEXT("Message Box: Error!"),
				MB_OK
			);
			iReturnVal = -1;
			goto NEXT;
		}
		m__bAllocConsole = TRUE;

		m__CONERR = GetStdHandle(STD_ERROR_HANDLE);
		if (m__CONERR == INVALID_HANDLE_VALUE || m__CONERR == NULL) {
			INT	i = *((PINT) 0);
		}
	}

	// 사용가능한 콘솔이 있다!
	m__CONOUT = GetStdHandle(STD_OUTPUT_HANDLE);
	if (m__CONOUT == INVALID_HANDLE_VALUE) {
		(VOID) MessageBox(
			NULL,
			TEXT("GetStdHandle(STD_OUTPUT_HANDLE) failed!"),
			TEXT("Message Box: Error!"),
			MB_OK
		);
		iReturnVal = -1;
		goto NEXT;
	} else if (m__CONOUT == NULL) {
		// 이럴수가...
		// 이미 사용가능한 콘솔이 있지 않나?...
		(VOID) MessageBox(
			NULL,
			TEXT("GetStdHandle(STD_OUTPUT_HANDLE): ")
			TEXT("No Associated Standard Output Handle!"),
			TEXT("Message Box: Warning!"),
			MB_OK
		);
		iReturnVal = -1;
		goto NEXT;
	}


NEXT:
	if (iReturnVal != 0) {
		if (m__bAllocConsole == TRUE) {
			if (0 && !FreeConsole()) {
				(VOID) MessageBox(
					NULL,
					TEXT("FreeConsole(VOID) failed!"),
					TEXT("Message Box: Error!"),
					MB_OK
				);
			}
			m__bAllocConsole = FALSE;
		}
		m__CONOUT = NULL;
		m__CONERR = NULL;
	}
}

myPRINT::~myPRINT(void)
{
	if (m__lpFile)
		fclose(m__lpFile);	// 리턴값은 무시

	if (m__bUseConsole == FALSE)
		return;

	if (m__bAllocConsole == TRUE) {
		if (0 && !FreeConsole()) {
			(VOID) MessageBox(
				NULL,
				TEXT("FreeConsole(VOID) failed!"),
				TEXT("Message Box: Error!"),
				MB_OK
			);
		}
		m__bAllocConsole = FALSE;
	}
	m__CONOUT = NULL;
	m__CONERR = NULL;
}

INT
myPRINT::mf__printf(LPCTSTR lpFormat, ...)
{
	if (!m__bUseConsole)
		return 0;

	if (!m__CONOUT) {
		// There is no associated standard handle.
		return 0;
	}

	va_list	arglist;
	va_start(arglist, lpFormat);

	INT	iReturnVal = 0;

	TCHAR	buffer[512];
	LPTSTR	lpEndPtr;
	HRESULT hResult = StringCchVPrintfEx(
		buffer,			// LPTSTR	pszDest
		512,			// size_t	cchDest	(in characters)
		&lpEndPtr,		// LPTSTR*	ppszDestEnd
		NULL,			// size_t*	pcchRemaing
		0,			// DWORD	dwFlags
		lpFormat,		// LPCTSTR	pszFormat
		arglist			// va_list	argList
	);
	if (hResult != S_OK && hResult != STRSAFE_E_INSUFFICIENT_BUFFER) {
		va_end(arglist);
		return -1;
	}
	DWORD	dwNumberOfCharsToWrite = (DWORD) (lpEndPtr - buffer);

	DWORD	dwNumberOfCharsWritten;
	if (WriteConsole(
		m__CONOUT,
		buffer,
		dwNumberOfCharsToWrite,
		(LPDWORD) &dwNumberOfCharsWritten,
		NULL
	) == 0) {
		iReturnVal = -1;
		DWORD	dwLastError = GetLastError();
	} else {
		iReturnVal = dwNumberOfCharsWritten;
	}

	va_end(arglist);

	return iReturnVal;
}

INT
myPRINT::mf__lprintf(LPCTSTR lpFormat, ...)
{
	if (!m__lpFile) {
		// fopen에서 실패!
		return 0;
	}

	va_list	arglist;
	va_start(arglist, lpFormat);
	time_t	ltime;
	time(&ltime);
	fprintf(m__lpFile, "%.24s: ", ctime(&ltime));	// w/o "\n\0"
	INT	iReturnVal = _vftprintf(m__lpFile, lpFormat, arglist);
	va_end(arglist);

	return iReturnVal;
}

INT
myPRINT::mf__fprintf(HANDLE m__CONXXX, LPCTSTR lpFormat, ...)
{
	if (!m__bUseConsole)
		return 0;

	if (!m__CONXXX) {
		// There is no associated standard handle.
		return 0;
	}

	va_list	arglist;
	va_start(arglist, lpFormat);

	INT	iReturnVal = 0;

	TCHAR	buffer[512];
	LPTSTR	lpEndPtr;
	HRESULT hResult = StringCchVPrintfEx(
		buffer,			// LPTSTR	pszDest
		512,			// size_t	cchDest	(in characters)
		&lpEndPtr,		// LPTSTR*	ppszDestEnd
		NULL,			// size_t*	pcchRemaing
		0,			// DWORD	dwFlags
		lpFormat,		// LPCTSTR	pszFormat
		arglist			// va_list	argList
	);
	if (hResult != S_OK && hResult != STRSAFE_E_INSUFFICIENT_BUFFER) {
		va_end(arglist);
		return -1;
	}
	DWORD	dwNumberOfCharsToWrite = (DWORD) (lpEndPtr - buffer);

	DWORD	dwNumberOfCharsWritten;
	if (WriteConsole(
		m__CONXXX,
		buffer,
		dwNumberOfCharsToWrite,
		(LPDWORD) &dwNumberOfCharsWritten,
		NULL
	) == 0) {
		iReturnVal = -1;
		DWORD	dwLastError = GetLastError();
	} else {
		iReturnVal = dwNumberOfCharsWritten;
	}

	va_end(arglist);

	return iReturnVal;
}

INT
myPRINT::mf__MessageBox(LPCTSTR lpFormat, ...)
{
	va_list	ap;
	va_start(ap, lpFormat);

	TCHAR	buffer[512];
	LPTSTR	lpEndPtr;
	HRESULT hResult = StringCchVPrintfEx(
		buffer,			// LPTSTR	pszDest
		512,			// size_t	cchDest	(in characters)
		&lpEndPtr,		// LPTSTR*	ppszDestEnd
		NULL,			// size_t*	pcchRemaing
		0,			// DWORD	dwFlags
		lpFormat,		// LPCTSTR	pszFormat
		ap			// va_list	argList
	);
	if (hResult != S_OK && hResult != STRSAFE_E_INSUFFICIENT_BUFFER) {
		va_end(ap);
		return -1;
	}
	DWORD	dwNumberOfCharsToWrite = (DWORD) (lpEndPtr - buffer);
	MessageBox(NULL, buffer, TEXT(""), MB_OK);

	va_end(ap);

	return 0;
}

INT
myPRINT::mf__SystemErrorMessage(
	DWORD	dwErrorCode,
	LPCTSTR	lpFormat,
	...
)
{
	va_list		ap;
	va_start(ap, lpFormat);
	TCHAR	buffer[512];
	LPTSTR	lpEndPtr;
	HRESULT	hResult = StringCchVPrintfEx(
		buffer,		// LPTSTR	pszDest
		512,		// size_t	cchDest	(in characters)
		&lpEndPtr,	// LPTSTR*	ppszDestEnd
		NULL,		// size_t*	pcchRemaing
		0,		// DWORD	dwFlags
		lpFormat,	// LPCTSTR	pszFormat
		ap		// va_list	argList
	);
	if (hResult != S_OK && hResult != STRSAFE_E_INSUFFICIENT_BUFFER)
		return -1;
	va_end(ap);

	LPVOID	lpMsgBuf = NULL;

	DWORD	dwTCHARs = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
//		FORMAT_MESSAGE_IGNORE_INSERTS |
		FORMAT_MESSAGE_MAX_WIDTH_MASK	// no limit of characters in a line && no newline break
		,
		NULL,
		dwErrorCode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
//		MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
		(LPTSTR) &lpMsgBuf,
		0,
		NULL
	);
	if (!dwTCHARs) {
		mf__fprintf(m__CONERR,
			TEXT("DWORD FormatMessage(...) fails: ")
			TEXT("DWORD GetLastError(VOID) = %u")
			TEXT("\n")
			,
			GetLastError()
		);
		return -1;
	}

	mf__fprintf(m__CONERR,
		TEXT("User: %s")
		TEXT("\n")			// "\n" appended
		TEXT("System: (%u) %s")
		TEXT("\n")			// "\n" appended
		,
		buffer,
		dwErrorCode,
		lpMsgBuf
	);

	if (LocalFree(lpMsgBuf) != NULL) {
		mf__fprintf(m__CONERR,
			TEXT("HLOCAL LocalFree(HLOCAL) fails: ")
			TEXT("DWORD GetLastError(VOID) = 0x%08X")
			TEXT("\n")
			TEXT("\tlpMsgBuf = 0x%08X NOT freed!")
			TEXT("\n")
			,
			GetLastError()
		);
	}

	return 0;
}

INT
myPRINT::mf__OutputDebugString(LPCTSTR lpFormat, ...)
{
	va_list	ap;
	va_start(ap, lpFormat);

	TCHAR	buffer[512];
	LPTSTR	lpEndPtr;
	HRESULT hResult = StringCchVPrintfEx(
		buffer,			// LPTSTR	pszDest
		512,			// size_t	cchDest	(in characters)
		&lpEndPtr,		// LPTSTR*	ppszDestEnd
		NULL,			// size_t*	pcchRemaing
		0,			// DWORD	dwFlags
		lpFormat,		// LPCTSTR	pszFormat
		ap			// va_list	argList
	);
	if (hResult != S_OK && hResult != STRSAFE_E_INSUFFICIENT_BUFFER) {
		va_end(ap);
		return -1;
	}
	DWORD	dwNumberOfCharsToWrite = (DWORD) (lpEndPtr - buffer);
	OutputDebugString(buffer);

	va_end(ap);

	return 0;
}
