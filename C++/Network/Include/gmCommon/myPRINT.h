#ifndef __myPRINT__
#define __myPRINT__
////////////////////////////////////////////////////////////////////////////////


#include <stdio.h>	// FILE
#include <Windows.h>	//INT, LPCTSTR, BOOL, TRUE, HANDLE, DWORD

class myPRINT
{
// 로그를 파일로 남기기 위하여...
private:
	FILE*	m__lpFile;
public:
	INT	mf__lprintf(LPCTSTR, ...);

private:
	BOOL	m__bUseConsole;
	BOOL	m__bAllocConsole;

public:
	myPRINT(BOOL bUseConsole = TRUE);
	~myPRINT();

	HANDLE	m__CONOUT;
	HANDLE	m__CONERR;

	// StringCchVPrintfEx, WriteConsole을 써서 구현했다.
	INT	mf__fprintf(HANDLE, LPCTSTR, ...);
	INT	mf__printf(LPCTSTR, ...);

	INT	mf__MessageBox(LPCTSTR, ...);
	INT	mf__SystemErrorMessage(DWORD, LPCTSTR, ...);
	INT	mf__OutputDebugString(LPCTSTR, ...);
};


////////////////////////////////////////////////////////////////////////////////
#endif	// __myPRINT__
