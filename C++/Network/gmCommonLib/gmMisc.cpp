#include <gmCommon/gmMisc.h>
#include <TCHAR.H>

void
ByteToStringA(
	const PBYTE	pByte,
	DWORD		dwLength,
	LPSTR		pszStringA
)
{
	PBYTE	pB = pByte;
	PBYTE	pS = (PBYTE) pszStringA;
	for (DWORD i = 0; i < dwLength; ++i, ++pB, ++pS) {
		BYTE	Byte = *pB;
		// 상위 4비트
		Byte >>= 4;
		*pS++ = Byte < 10 ? '0' + Byte : 'A' + (Byte - 10);
		// 하위 4비트
		Byte = *pB & 0x0F;
		*pS = Byte < 10 ? '0' + Byte : 'A' + (Byte - 10);
	}
	*pS = '\0';
}

int
StringAToByte(
	LPSTR	pszStringAndByte
)
{
	PBYTE	pB = (PBYTE) pszStringAndByte;
	PBYTE	pS = (PBYTE) pszStringAndByte;
	BYTE	Byte;
	while ((Byte = *pS)) {
		BYTE	ByteX = 0;
		if (Byte >= '0' && Byte <= '9')
			ByteX = (Byte - '0') << 4;
		else if (Byte >= 'a' && Byte <= 'f')
			ByteX = (Byte - 'a' + 10) << 4;
		else if (Byte >= 'A' && Byte <= 'F')
			ByteX = (Byte - 'A' + 10) << 4;
		else {
			// Invalid hexdigit
			return -1;
		}

		if ((Byte = *++pS) == '\0') {
			// 아직 끝나지 않은 hexdigit
			return -2;
		}

		if (Byte >= '0' && Byte <= '9')
			ByteX |= Byte - '0';
		else if (Byte >= 'a' && Byte <= 'f')
			ByteX |= Byte - 'a' + 10;
		else if (Byte >= 'A' && Byte <= 'F')
			ByteX |= Byte - 'A' + 10;
		else {
			// Invalid hexdigit
			return -1;
		}

		*pB++ = ByteX;
		++pS;
	}

	return 0;
}

BOOL
ByteToString(
	const PBYTE	pByte,
	DWORD		cbByteLength,
	LPTSTR		pString,
	PDWORD		pcbStringLength		// [IN/OUT]
)
{
	DWORD	cbStringLength = 2 * sizeof(TEXT(' ')) * (cbByteLength + 1);
	if (*pcbStringLength < cbStringLength) {
		// 공간이 충분하지 않다.
		*pcbStringLength = cbStringLength;	// 이 값을 참고하라.
		return FALSE;
	}
	*pcbStringLength = cbStringLength;

	PBYTE	pB = pByte;
	TCHAR	*pTChar = pString;
	for (DWORD i = 0; i < cbByteLength; ++i, ++pB, ++pTChar) {
		BYTE	Byte = *pB;
		// 상위 4비트
		Byte >>= 4;
		*pTChar++ = Byte < 10 ? TEXT('0') + Byte : TEXT('A') + (Byte - 10);
		// 하위 4비트
		Byte = *pB & 0x0F;
		*pTChar = Byte < 10 ? TEXT('0') + Byte : TEXT('A') + (Byte - 10);
	}
	*pTChar = TEXT('\0');

	return TRUE;
}

/*
 * 주어진 "Value Name"(= lpszGivenValueName)이 서브키에 있는 "Value Name"중의 하나인지 알아본다.
 */
BOOL
FindValueName(
	HKEY	hKey,
	LPCTSTR	lpszGivenValueName,
	PDWORD	pdwLastErrorCode
)
{
	BOOL	bReturnVal = FALSE;	// 기본값	! 문제가 생겼을 때, 이 값이 리턴값이다.

	// 서브키에 있는 "Value Name"을 저장할 장소를,
	// 크기가 얼마인지 알 수 없으므로, RegQueryInfoKey를 이용해서
	// 동적으로 할당받는다.
	LPTSTR	lpszValueName = NULL;

__try {
	// 서브키의 정보 중에서
	DWORD	cValues;		// Value의 갯수와
	DWORD	cMaxValueNameLen;	// 가장 긴 "Value Name"의 크기를 얻는다.
	if (RegQueryInfoKey(
		hKey,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		&cValues,
		&cMaxValueNameLen,	// 제일 마지막의 NULL문자(= T('\0'))를 포함하지 않는다.
		NULL,
		NULL,
		NULL
	) != ERROR_SUCCESS) {
		bReturnVal = FALSE;
		*pdwLastErrorCode = GetLastError();
		__leave;
	}
	if (cValues) {
		lpszValueName = new TCHAR[cMaxValueNameLen + 1];	// T('\0')
		if (!lpszValueName) {
			// 메모리를 할당받지 못했다.
			bReturnVal = FALSE;
			__leave;
		}
	}
	for (DWORD i = 0; i < cValues; ++i) {
		DWORD	cValueName = cMaxValueNameLen + 1;		// T('\0')
		if (RegEnumValue(
			hKey,
			i,
			lpszValueName,
			&cValueName,	// 여기서는 NULL문자(= T('\0'))까지 포함한 크기이다.
			NULL,
			NULL,
			NULL,
			NULL
		) != ERROR_SUCCESS) {
			bReturnVal = FALSE;
			*pdwLastErrorCode = GetLastError();
			__leave;
		}

		if (!_tcscmp(lpszValueName, lpszGivenValueName)) {
			// 일치하는 "Value Name"을 찾았다.
			break;
		}
	}
	if (i == cValues) {
		// 일치하는 "Value Name"을 찾지 못했다.
		bReturnVal = FALSE;
	} else {
		// 찾았다!
		bReturnVal = TRUE;
	}
} __finally {
	if (lpszValueName) {
		delete lpszValueName;
		lpszValueName = NULL;
	}
}

	return bReturnVal;
}

/*
 * 서버인증에 사용할 인증서를 가져온다.
 * 이 인증서에는 개인키가 포함되어있어야 한다.
 * "인증서.TXT"에 이러한 인증서를 만들고,
 * 관리하는 방법과 예가 있다.
 *
 * 이 함수는 서버에서만 사용한다.
 * 인증서를 가져오는 저장소는 MY를 사용한다.
 */
PCCERT_CONTEXT
GetCertificateContext(
	LPCTSTR	szSubsystemProtocol,
	LPCTSTR	pszSubject,
	PDWORD	pdwLastErrorCode
)
{
	HCERTSTORE	hCertStore = NULL;
#if 1
	if (!(hCertStore = CertOpenStore(
#if 1
/*
 * Provider types that pass a string to pvPara have alternate forms:
 * one that passes the string as Unicode (an LPCWSTR variable) and
 * another that passes the string as ASCII (an LPCSTR variable).
 * The names of providers that pass ASCII strings must be suffixed with "_A" as in CERT_STORE_PROV_FILENAME_A.
 * The names of providers that pass Unicode strings can be suffixed with "_W" as in sz_CERT_STORE_PROV_REG_W.
 * The names of providers without either the "_A" or "_W" use the default, Unicode form.
 */
		// CERT_STORE_PROV_SYSTEM	CERT_STORE_PROV_SYSTEM_W
		// CERT_STORE_PROV_SYSTEM은 UNICODE용으로 정의되어 있지 않다.
#ifdef UNICODE
		CERT_STORE_PROV_SYSTEM_W,
#else	// UNICODE
		CERT_STORE_PROV_SYSTEM_A,
#endif	// !UNICODE
#else
		CERT_STORE_PROV_SYSTEM
#endif
		0,
		NULL,
#if 0
		CERT_SYSTEM_STORE_LOCAL_MACHINE,
#else
		CERT_SYSTEM_STORE_CURRENT_USER,
#endif
		szSubsystemProtocol
	))) {
		*pdwLastErrorCode = GetLastError();
		return NULL;
	}
#else
	if (!(hCertStore = CertOpenSystemStore(
		NULL,		// Use default CSP. Otherwise call CryptAcquireContext.
		TEXT("MY")
	))) {
		*pdwLastErrorCode = GetLastError();
		return NULL;
	}
#endif

	PCCERT_CONTEXT	pCertContext = CertFindCertificateInStore(
		hCertStore,
		X509_ASN_ENCODING,
		0,
#if 1
		// CERT_FIND_SUBJECT_STR	CERT_FIND_SUBJECT_STR_W
		// CERT_FIND_SUBJECT_STR은 UNICODE용으로 정의되어 있지 않다.
#ifdef UNICODE
		CERT_FIND_SUBJECT_STR_W,	// Searches for a certificate containing the specified subject name string.
#else	// UNICODE
		CERT_FIND_SUBJECT_STR_A,	// Searches for a certificate containing the specified subject name string.
#endif	// !UNICODE
#else
		CERT_FIND_SUBJECT_NAME,		// Searches for a certificate with an exact match of the entire subject name with the name in the CERT_NAME_BLOB structure.
#endif
		pszSubject,
		NULL				// Pointer to the last CERT_CONTEXT structure returned by this function. This parameter must be NULL on the first call of the function. A pPrevCertContext parameter that is not NULL is always freed by this function, even if the function causes an error.
	);
	if (!pCertContext) {
		*pdwLastErrorCode = GetLastError();
		//	CRYPT_E_NOT_FOUND	0x80092004L
		return NULL;
	}

	DWORD	dwLastErrorCode = 0;
	// 인증서 저장고를 닫는다.
	// 이 때, 여기서 찾아낸 인증서에 대한 핸들을 앞으로도 사용할 것이므로
	// CERT_CLOSE_FORCE_FLAG를 사용하면 안된다.
	if (hCertStore) {
#if 1
		// 계속 사용되는 메모리가 있는 지 확인한다.
		if (CertCloseStore(hCertStore, CERT_CLOSE_STORE_CHECK_FLAG) == FALSE) {
			dwLastErrorCode = GetLastError();
			if (dwLastErrorCode == CRYPT_E_PENDING_CLOSE) {
				// 어딘가에서 사용되고 있다.
				// 따라서 해당메모리는 그대로 남아있을 것이다.

				// 당연하다!
				// pCertContext에서 사용하고 있다.
				// 메모리를 해제하면 더 이상 pCertContext는 의미가 없다.
				dwLastErrorCode = 0;
			} else {
				// 다른 문제가 있다.
			}
		}
#else
		// 계속 사용되는 메모리가 있는 지에 대한 관심이 없다.
		if (CertCloseStore(hCertStore, 0) == FALSE) {
			// 어떤 문제가 있다.
			dwLastErrorCode = GetLastError();
		}
#endif
	}
	if (dwLastErrorCode && pCertContext) {
		// 어떤 문제가 있었으므로 pCertContext도 사용하지 말자.
		(void) CertFreeCertificateContext(pCertContext);	// SDK에 따르면 이 함수는 항상 TRUE를 리턴한다.
		*pdwLastErrorCode = dwLastErrorCode;
		return NULL;
	}

	return pCertContext;
}
