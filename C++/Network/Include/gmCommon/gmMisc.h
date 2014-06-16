#ifndef __gm_MISC__
#define __gm_MISC__
////////////////////////////////////////////////////////////////////////////////


#include <Windows.h>

void
ByteToStringA(
	const PBYTE	pByte,
	DWORD		dwLength,
	LPSTR		pszString
);

int
StringAToByte(
	LPSTR	pszString
);

// 저장할 공간이 충분하지 않으면 FALSE를 리턴한다.
BOOL
ByteToString(
	const PBYTE	pByte,
	DWORD		cbByteLength,	// [IN] pByte의 크기(바이트 단위)
	LPTSTR		pString,
	PDWORD		pcbStringLength	// [IN/OUT] pString의 크기(바이트 단위)를 가리키는 포인터
);

PCCERT_CONTEXT
GetCertificateContext(
	LPCTSTR	szSubsystemProtocol,
	LPCTSTR	pszSubject,
	PDWORD	pdwLastErrorCode
);

BOOL
FindValueName(
	HKEY	hKey,
	LPCTSTR	lpszGivenValueName,
	PDWORD	pdwLastErrorCode
);


////////////////////////////////////////////////////////////////////////////////
#endif	// !__gm_MISC__
