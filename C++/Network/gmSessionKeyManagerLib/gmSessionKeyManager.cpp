#include <gmCommon/gmMisc.h>
#include <gmSessionKey/gmSessionKeyManager.h>
#ifdef _DEBUG
#include <STDIO.H>
#endif	// _DEBUG

#include <Shlwapi.h>	// SHDeleteKey

static inline void
RomeoMustDie(
	void
)
{
	int	i = *((int *) 0);
}

static TCHAR	SessionKeySubKey[]	= TEXT("Software\\SecureASP");
static TCHAR	SessionKeyValueName[]	= TEXT("Session Key");

gmSessionKeyManager::gmSessionKeyManager(
	// 2005/4/12 (EST) - ghkim
	ALG_ID	HashAlgID,
	ALG_ID	EncryptAlgID,

	BOOL	*pbStatus,
	PDWORD	pdwLastErrorCode
)
{
	BOOL	bReturnVal;

	m__dwLastErrorCode	= 0;

	m__hCryptProv	= NULL;
	m__hSessionKey	= NULL;

	m__pCertContext			= NULL;
	m__hCryptProvFromCertificate	= NULL;
	m__hKeyExchangeKey		= NULL;

	// 2005/4/12 (EST) - ghkim
	m__HashAlgID			= HashAlgID;
	m__EncryptionAlgID		= EncryptAlgID;

__try {
	// 데이터의 해시값을 얻을 때 사용하는 키컨테이너에 대한 핸들을 얻는다.
	if (!CryptAcquireContext(
		&m__hCryptProv,
		NULL,
		NULL,
		PROV_RSA_FULL,
		CRYPT_VERIFYCONTEXT
	)) {
		bReturnVal = FALSE;
		m__dwLastErrorCode = GetLastError();
		__leave;
	}

#if 1
// 인증서를 사용해서 Session Key를 얻는 경우
	// 인증서로부터 몇 가지 정보를 설정한다.
	if (!mf__CryptAcquireCertificatePrivateKey()) {
		bReturnVal = FALSE;
		__leave;
	}
#else
// TLS를 이용해서 서버로부터 Session Key를 받아오는 경우
#endif

	// Session Key를 얻는다.
	if (!mf__SetSessionKey()) {
		bReturnVal = FALSE;
		__leave;
	}

	// 2005/4/12 (EST) - ghkim
	if (!mf__GetSize()) {
		bReturnVal = FALSE;
		__leave;
	}

	bReturnVal = TRUE;
} __finally {
	if (!bReturnVal) {
		if (m__hCryptProv) {
			if (!CryptReleaseContext(m__hCryptProv, 0)) {
				// 여기서 문제가 생기면 어떻게 하나...
				RomeoMustDie();
				m__dwLastErrorCode = (DWORD) 0x80000001;
			}
			m__hCryptProv = NULL;
		}
	}
}

	*pbStatus = bReturnVal;
	*pdwLastErrorCode = m__dwLastErrorCode;
}


gmSessionKeyManager::~gmSessionKeyManager()
{
	if (m__hCryptProv) {
		if (m__hSessionKey) {
			if (!CryptDestroyKey(m__hSessionKey)) {
				m__dwLastErrorCode = (DWORD) 0x80000002;
				RomeoMustDie();
			}
			m__hSessionKey = NULL;
		}

		if (!CryptReleaseContext(m__hCryptProv, 0)) {
			// 여기서 문제가 생기면 어떻게 하나...
			m__dwLastErrorCode = (DWORD) 0x80000003;
			RomeoMustDie();
		}
		m__hCryptProv = NULL;
	}

#if 1
	DWORD	dwLastErrorCode;
	if (!mf__FinalizeCertificate(&dwLastErrorCode)) {
		m__dwLastErrorCode = (DWORD) 0x80000004;
		RomeoMustDie();
	}
#endif
}


#if 1
// 인증서를 사용해서 Session Key를 얻는 경우
BOOL
gmSessionKeyManager::mf__FinalizeCertificate(
	PDWORD	pdwLastErrorCode
)
{
	*pdwLastErrorCode	= 0;
	BOOL	bReturnVal	= TRUE;

	// PCCERT_CONTEXT가 있어야 HCRYPTPROV가 있고,
	// HCRYPTPROV가 있어야 HCRYPTKEY가 있다.
	if (m__pCertContext) {
		(void) CertFreeCertificateContext(m__pCertContext);	// SDK에 따르면 이 함수는 항상 TRUE를 리턴한다.
		m__pCertContext = NULL;

		if (m__hCryptProvFromCertificate) {
			if (m__hKeyExchangeKey) {
				if (!CryptDestroyKey(m__hKeyExchangeKey)) {
					*pdwLastErrorCode = GetLastError();
					bReturnVal = FALSE;
				}
				m__hKeyExchangeKey = NULL;
			}

			if (m__bCallerFreeProv && !CryptReleaseContext(m__hCryptProvFromCertificate, 0)) {
				*pdwLastErrorCode = GetLastError();
				bReturnVal = FALSE;
			}
			m__hCryptProvFromCertificate = NULL;
		}
	}

	return bReturnVal;
}


BOOL
gmSessionKeyManager::mf__CryptAcquireCertificatePrivateKey(
	void
)
{
	BOOL	bReturnVal;

	m__hCryptProvFromCertificate	= NULL;
	m__pCertContext			= NULL;
	m__hKeyExchangeKey		= NULL;

__try {
	// 인증서에 대한 핸들을 얻는다.
	// 인증서에는 개인키 정보가 있어야 한다.
	DWORD	dwLastErrorCode;
	m__pCertContext = GetCertificateContext(TEXT("MY"), TEXT("Session Key"), &dwLastErrorCode);
	if (!m__pCertContext) {
		bReturnVal = FALSE;
		m__dwLastErrorCode = dwLastErrorCode;
		__leave;
	}

	// 인증서에 있는 "키컨테이너"를 접근할 수 있는 핸들을 얻는다.
/*
	The CryptAcquireCertificatePrivateKey function acquires a HCRYPTPROV cryptographic service provider (CSP) handle
	including access to its related key container and the dwKeySpec
	for a user's specified certificate context.
	This function is used to get access to a user's private key
	when the user's certificate is available
	and the handle of a CSP with the user's key container is not available.
	This function can be used by the owner of a private key and not by any other user.

	If a CSP handle and the key container containing a user's private key are available,
	the CryptGetUserKey function can be used.

	BOOL WINAPI CryptAcquireCertificatePrivateKey(
		PCCERT_CONTEXT	pCert,			// [IN] Pointer to a CERT_CONTEXT structure containing the specified certificate context. This is the certificate context for which a private key will be acquired.
		DWORD		dwFlags,		// [IN] The following flag values are defined.
							//	CRYPT_ACQUIRE_CACHE_FLAG		! An HCRYPTPROV value is returned as phCryptProv. If an HCRYPTPROV value is already acquired and cached, that HCRYPTPROV value is returned. Otherwise, a new HCRYPTPROV value is acquired and cached by using the certificate's CERT_KEY_CONTEXT_PROP_ID.
							//						! In any case, when this flag is set, pfCallerFreeProv returns FALSE and the calling application must not free the cached HCRYPTPROV value.
							//	CRYPT_ACQUIRE_USE_PROV_INFO_FLAG	! Uses the certificate's property CERT_KEY_PROV_INFO_PROP_ID to determine if caching should be accomplished. For more information on CERT_KEY_PROV_INFO_PROP_ID, see CertSetCertificateContextProperty.
							//						! Uses caching, but only if during a previous call the dwFlags of the CRYPT_KEY_PROV_INFO structure was CERT_SET_KEY_CONTEXT_PROP.
							//	CRYPT_ACQUIRE_COMPARE_KEY_FLAG		! The public key in the certificate is compared with the public key returned by the CSP. If the keys do not match, the acquisition operation fails and the last error code is set to NTE_BAD_PUBLIC_KEY. If a cached HCRYPTPROV value is returned, no comparison is made.
							//	CRYPT_ACQUIRE_SILENT_FLAG		! The application requests that the CSP not display any user interface (UI) for this context. If the CSP must display the UI to operate, the call fails and the NTE_SILENT_CONTEXT error code is set as the last error.
		void		*pvReserved,		// [IN] Reserved for future use and must be NULL.
		HCRYPTPROV	*phCryptProv,		// [OUT] Pointer to the returned HCRYPTPROV value. Depending on the dwFlags value, the HCRYPTPROV value must be freed by the calling application or explicitly freed on the last free action of the certificate context.
		DWORD		*pdwKeySpec,		// [OUT] Pointer to a DWORD value identifying the private key to use from the acquired provider's key container. It can be AT_KEYEXCHANGE or AT_SIGNATURE.
		BOOL		*pfCallerFreeProv	// [OUT] Pointer to a BOOL flag. The flag is returned set to FALSE if:
							//	Public key acquisition or comparison fails.
							//	CRYPT_ACQUIRE_CACHE_FLAG is set.
							//	CRYPT_ACQUIRE_USE_PROV_INFO_FLAG is set, the certificate context property is set to CERT_KEY_PROV_INFO_PROP_ID with the CRYPT_KEY_PROV_INFO structure, and the dwFlags parameter is set to CERT_SET_KEY_CONTEXT_PROP_ID.
							// If the pfCallerFreeProv flag is returned FALSE, the calling application must not release the returned HCRYPTPROV value. The returned HCRYPTPROV value is released on the last free action of the certificate context.
							// When the pfCallerFreeProv flag is set to TRUE, the returned HCRYPTPROV value must be released by calling the CryptReleaseContext function.
	);

 */
	DWORD	dwKeySpec;
	if (!CryptAcquireCertificatePrivateKey(
		m__pCertContext,		//	PCCERT_CONTEXT	pCert		! [IN]
		0,//CRYPT_ACQUIRE_SILENT_FLAG,	//	DWORD		dwFlags		! [IN] 개인키를 접근할 때(CryptImportKey), UI가 나타나게 하지 않는다.
		NULL,				//	void		*pvReserved	! [IN] Reserved for future use and must be NULL.
		&m__hCryptProvFromCertificate,	//	HCRYPTPROV	*phCryptProv	! [OUT]
		&dwKeySpec,			//	DWORD		*pdwKeySpec	! [OUT]
		&m__bCallerFreeProv		//	BOOL		*pfCallerFreeProv	! [OUT]
	)) {
		bReturnVal = FALSE;
		m__dwLastErrorCode = GetLastError();
		// 개인키가 포함된 인증서를 가져올 때
		//	강력한 개인 키 보호(E). 이 옵션을 사용하면 응용 프로그램이 개인키를 사용할 때마다 묻습니다.
		// 항목을 체크했다. 그러면,
		// 개인키를 가져올 때, 보안 수준을 "보통"이나 "높음"으로 설정할 수 있다.
		// "보통"으로 설정한 경우, CryptImportKey를 써서 개인키에 접근할 때
		//	응용 프로그램이 보호 항목의 액세스를 요청합니다.
		// 라는 창을 띄운다.
		// "높음"으로 설정한 경우에는 암호를 사용해야 한다.
		// 어느 경우든 UI창은 뜨게 되어있다.
		// 따라서 이 경우에 CRYPT_ACQUIRE_SILENT_FLAG옵션을 설정하면 다음 오류코드가 발생한다.
		//	NTE_SILENT_CONTEXT	0x80090022
		//	CRYPT_E_NO_KEY_PROPERTY	0x8009200BL	! 개인키를 가지고 있지 않을 때
		__leave;
	}
	// dwKeySpec = ?	AT_KEYEXCHANGE(1) or AT_SIGNATURE(2)

	// Key-Exchange Key를 얻는다.
	if (!CryptGetUserKey(
		m__hCryptProvFromCertificate,
		AT_KEYEXCHANGE,
		&m__hKeyExchangeKey
	)) {
		bReturnVal = FALSE;
		m__dwLastErrorCode = GetLastError();
		__leave;
	}

	bReturnVal = TRUE;
} __finally {
	if (!bReturnVal) {
		DWORD	dwLastErrorCode;
		(void) mf__FinalizeCertificate(&dwLastErrorCode);
	}
}

	return bReturnVal;
}
#else
// TLS를 이용해서 서버로부터 Session Key를 받아오는 경우
#endif


/*
 * 레지스트리에 저장된, 인증서의 Key-Exchange Key로 암호화된 Session Key를 얻는다.
 * 없다면 만든다.
 */
BOOL
gmSessionKeyManager::mf__SetSessionKey(
	void
)
{
	BOOL	bReturnVal;

	// 서브키를 새로 만들었거나
	// 서브키가 있더라도 특정 "Value Name"이 없을 때에는
	// 인증서로부터 임의의 Session Key를 만들고,
	// 이 Session Key를 인증서의 Key-Exchange Key로 암호화한 후 Export한다.
	// 그 Export된 Blob을 가리킨다.
	PBYTE	pEncryptedAndExportedSessionKeyBlob = NULL;
	DWORD	dwEncryptedAndExportedSessionKeyBlobLength;

	HKEY	hKey = NULL;
__try {
	DWORD	dwDisposition;	// 새로 만들었는 지, 아니면 있는 것을 열었는 지를 말해준다.
	if (RegCreateKeyEx(
		HKEY_LOCAL_MACHINE,	// HKEY_CURRENT_USER,
		SessionKeySubKey,
		0,
		NULL,
		0,
		KEY_ALL_ACCESS,
		NULL,
		&hKey,
		&dwDisposition
	) != ERROR_SUCCESS) {
		bReturnVal = FALSE;
		m__dwLastErrorCode = GetLastError();
		// ERROR_ACCESS_DENIED	5L	KEY_READ가 없거나, 존재하지 않는 SubKey일 때
		__leave;
	}

	if (
		dwDisposition == REG_CREATED_NEW_KEY ||				// 서브키를 새로 만들었거나
		!FindValueName(hKey, SessionKeyValueName, &m__dwLastErrorCode)	// 서브키가 있더라도 특정 "Value Name"이 없을 때
	) {
		// 특정 "Value Name"의 "값"을 "적절히" 설정한다.

#if 1
// 인증서를 사용해서 Session Key를 얻는 경우
		// 인증서를 사용하여 임의의 Session Key를 만들고,
		// 만들어진 Session Key를 인증서의 공유키로 암호화한 후
		// Export할 Blob을 만든다.

		// 임의의 Session Key를 만든다.
		// Session Key를 만든다.
		if (!CryptGenKey(
			m__hCryptProvFromCertificate,
			m__EncryptionAlgID,
			CRYPT_EXPORTABLE,
			&m__hSessionKey
		)) {
			bReturnVal = FALSE;
			m__dwLastErrorCode = GetLastError();
			__leave;
		}

		// Session Key를 Key-Exchange Key로 암호화해서 Export한다.
		// pEncryptedAndExportedSessionKeyBlob가 가리키는 버퍼의 크기가 얼마가 되어야 하는 지 알아본다.
		if (!CryptExportKey(
			m__hSessionKey,		// Export하는 이 키를
			m__hKeyExchangeKey,	// Key-Exchange Key로 암호화한다.
			SIMPLEBLOB,		// Session Key를 Export한다.
			0,			//	DWORD	dwFlags		! [IN]
			NULL,			// 크기가 얼마인가?
			&dwEncryptedAndExportedSessionKeyBlobLength	// 여기에 필요한 크기를 저장한다.
		)) {
			bReturnVal = FALSE;
			m__dwLastErrorCode = GetLastError();
			__leave;
		}
		pEncryptedAndExportedSessionKeyBlob = new BYTE[dwEncryptedAndExportedSessionKeyBlobLength];
		if (!pEncryptedAndExportedSessionKeyBlob) {
			// 메모리를 할당받을 수 없다.
			bReturnVal = FALSE;
			__leave;
		}
		// 이제 Export한다.
		if (!CryptExportKey(
			m__hSessionKey,
			m__hKeyExchangeKey,
			SIMPLEBLOB,
			0,
			pEncryptedAndExportedSessionKeyBlob,		// 여기에 저장한다.
			&dwEncryptedAndExportedSessionKeyBlobLength	// 여기에 필요한 크기를 저장한다.
		)) {
			bReturnVal = FALSE;
			m__dwLastErrorCode = GetLastError();
			__leave;
		}
#else
// TLS를 이용해서 서버로부터 Session Key를 받아오는 경우
#endif

		if (RegSetValueEx(
			hKey,
			SessionKeyValueName,
			0,
			REG_BINARY,
			(const BYTE *) pEncryptedAndExportedSessionKeyBlob,
			dwEncryptedAndExportedSessionKeyBlobLength
		) != ERROR_SUCCESS) {
			bReturnVal = FALSE;
			m__dwLastErrorCode = GetLastError();
			__leave;
		}
	}

#if 1
	// 2005/4/11 (EST) - ghkim
	//	레지스트리에 해당하는 키를 만들고 값을 저장한 경우에도
	//	암호를 입력해야 한다.
#else
	// 서브키가 있고(dwDispositio = REG_OPENED_EXISTING_KEY),
	// 주어진 "Value Name"이 있을 때다.
	// 주어진 "Value Name"의 값을 얻는다.
#endif

	DWORD	Type;
	if (RegQueryValueEx(
		hKey,
		SessionKeyValueName,
		0,
		&Type,
		NULL,						// 얼마나 필요한지
		&dwEncryptedAndExportedSessionKeyBlobLength	// 여기에다 써라.
	) != ERROR_SUCCESS) {
		bReturnVal = FALSE;
		m__dwLastErrorCode = GetLastError();
		__leave;
	}
	if (Type != REG_BINARY) {
		// 저장할 때는 REG_BINARY로 했는 데...
		__leave;
	}
	pEncryptedAndExportedSessionKeyBlob = new BYTE[dwEncryptedAndExportedSessionKeyBlobLength];
	if (!pEncryptedAndExportedSessionKeyBlob) {
		// 메모리 할당에 문제가 있다.
		__leave;
	}
	if (RegQueryValueEx(
		hKey,
		SessionKeyValueName,
		0,
		&Type,
		pEncryptedAndExportedSessionKeyBlob,		// 여기에 저장하자.
		&dwEncryptedAndExportedSessionKeyBlobLength
	) != ERROR_SUCCESS) {
		bReturnVal = FALSE;
		m__dwLastErrorCode = GetLastError();
		__leave;
	}

#if 1
// 인증서를 사용해서 Session Key를 얻는 경우
	// 인증서의 공유키로 암호돠된 이 값을 인증서의 개인키로 풀어서 Import한다.
	if (!CryptImportKey(
		m__hCryptProvFromCertificate,
		pEncryptedAndExportedSessionKeyBlob,
		dwEncryptedAndExportedSessionKeyBlobLength,
		0,		// Key-Exchange Key가 묵시적으로 사용된다. Export할 때는 명시해야 하지만.
		CRYPT_EXPORTABLE,
		&m__hSessionKey
	)) {
		bReturnVal = FALSE;
		m__dwLastErrorCode = GetLastError();
		//	NTE_NO_KEY	0x8009000DL	// 인증서를 설치할 때 개인키를 보호하기 위해 보안 수준을 "높음"을 설정한 경우, 지정한 암호가 맞지 않을 때(CRYPT_ACQUIRE_SILENT_FLAG옵션과 관련된 CryptAcquireCertificatePrivateKey함수의 오류처리부분을 보라.)
		//	NTE_BAD_KEY	0x80090003L
		//	NTE_BAD_DATA	0x80090005L	// Export할 때 사용했던 Session Key를 사용하지 않을 때
							// Exchange Key이외의 키로 암호화되었을 때(여기에는 데이터가 바뀐 경우도 포함된다.)
		//	NTE_BAD_VER	0x80090007L	// 데이터의 헤더를 고쳤을 때
		__leave;
	}
#else
// TLS를 이용해서 서버로부터 Session Key를 받아오는 경우
#endif

	bReturnVal = TRUE;
} __finally {
	if (hKey) {
		if (RegCloseKey(hKey) != ERROR_SUCCESS) {
			// 여기서 문제가 생기면 어쩌나...
			bReturnVal = FALSE;
			m__dwLastErrorCode = GetLastError();
			RomeoMustDie();
		}
		hKey = NULL;
	}

	if (pEncryptedAndExportedSessionKeyBlob) {
		delete pEncryptedAndExportedSessionKeyBlob;
		pEncryptedAndExportedSessionKeyBlob = NULL;
	}
}

	return bReturnVal;
}


// 2005/4/12 (EST) - ghkim
// 주어진 문자열을 해시처리한 데이터의 크기를 구해서
// 그 해시 데이터가 암호화되는 크기를 구한다.
BOOL
gmSessionKeyManager::mf__GetSize(
	void
)
{
	BOOL	bReturnVal;

	HCRYPTHASH	hHash			= NULL;		// typedef ULONG_PTR HCRYPTHASH;
__try {
// 해시 데이터의 크기를 구한다.
	if (!CryptCreateHash(
		m__hCryptProv,
		m__HashAlgID,
		0,		// Key를 사용하지 않는 HASH이다.
		0,		// Reserved for future use and must be zero.
		&hHash
	)) {
		bReturnVal = FALSE;
		m__dwLastErrorCode = GetLastError();
		__leave;
	}

	DWORD	dwHashSize;
	DWORD	dwDataLength;
	// 해시 데이터의 바이트 수를 얻는다.
	dwDataLength = sizeof(dwHashSize);
	if (!CryptGetHashParam(
		hHash,			//	HCRYPTHASH	hHash	! [IN]
		HP_HASHSIZE,		//	DWORD	dwParam		! [IN]
		(BYTE *) &dwHashSize,	//	BYTE	*pbData		! [IN] pcbCipherText가 기리키는 곳에 바이트 수를 적는다.
		&dwDataLength,		//	DWORD	*pdwDataLen,	! [IN/OUT] pcbCipherText가 가리키는 버퍼(DWORD)의 크기(바이트 수)이다.
		0			//	DWORD	dwFlags		! [IN] Reserved for future use and must be zero.
	)) {
		bReturnVal = FALSE;
		m__dwLastErrorCode = GetLastError();
		__leave;
	}

// 암호화된 데이터의 크기를 구한다.
	// 주어진 크기(바이트 수)의 데이터가
	// 암호화되어 저장될 버퍼의 크기(바이트 수)를 구한다.
	// Stream cipher에서는 원래 데이터의 크기와 같지만
	// block cipher에서는 그렇지 않기 때문이다.
	DWORD	cbEncryptedDataLength = dwHashSize;
	if (!CryptEncrypt(
		m__hSessionKey,		//	HCRYPTKEY	hKey		! [IN] 암호화에 사용될 키
		0,			//	HCRYPTHASH	hHash		! [IN] 암화된 데이터는 해시처리 되지 않는다.
		TRUE,			//	BOOL		Final		! [IN] 주어진 데이터가 암호화의 마지막 단계인지 아닌지 말한다.
		0,			//	DWORD		dwFlags		! [IN] The following dwFlags value is defined but reserved for a future release:
					//					!	CRYPT_OAEP	When set with the MS Enhanced Provider and RSA encryption/decryption causes PKCS #1 version 2 formatting to be used.
		NULL,			//	BYTE		*pbData		! [IN/OUT] 암호화될(IN) 데이터가 저장되어(IN) 있고, 암호화되어(OUT) 저장될(OUT) 버퍼를 가리킨다.					!! 버퍼의 크기(바이트 수)를 구해서
		&cbEncryptedDataLength,	//	DWORD		*pdwDataLen	! [IN/OUT] 암호화될(IN) 데이터가 저장되어(IN) 있는 버퍼의 크기(바이트 수)이다. 암호화된(OUT) 데이터가 차지하는 크기(바이트 수)이다.	!! 여기에 저장한다.
		0			//	DWORD		dwBufLen	! [IN] pbData가 가리키는 버퍼의 크기(바이트 수)이다.												!! 아직 버퍼의 크기를 결정하지 못했으므로, 이 값은 의미가 없다.
	)) {
		bReturnVal = FALSE;
		m__dwLastErrorCode = GetLastError();
		// NTE_BAD_HASH_STATE	0x8009000CL
		__leave;
	}

	m__cbHashedAndEncryptedDatalength = cbEncryptedDataLength;
	bReturnVal = TRUE;
} __finally {
	if (hHash) {
		if (!CryptDestroyHash(hHash)) {
			// 여기서 문제가 생기면 어쩌나...
			bReturnVal = FALSE;
			RomeoMustDie();
		}
		hHash = NULL;
	}
}

	return bReturnVal;
}


// 평범한 데이터를 해시를 구해서 Session Key로 암호화한다.
BOOL
gmSessionKeyManager::mf__GetHashedAndEncryptedData(
	PBYTE	pbPlainData,
	DWORD	cbPlainDataLength,	// [IN] 데이터의 크기
	DWORD	cbPlainDataBufferLength	// [IN] 버퍼의 크기
)
{
	if (cbPlainDataBufferLength < m__cbHashedAndEncryptedDatalength) {
		// 버퍼의 크기가 충분하지 않다.
		return FALSE;
	}

	BOOL	bReturnVal;

	HCRYPTHASH	hHash			= NULL;		// typedef ULONG_PTR HCRYPTHASH;

__try {
	// 해시 오브젝트를 만든다.
	if (!CryptCreateHash(
		m__hCryptProv,
		m__HashAlgID,
		0,		// Key를 사용하지 않는 HASH이다.
		0,		// Reserved for future use and must be zero.
		&hHash
	)) {
		bReturnVal = FALSE;
		m__dwLastErrorCode = GetLastError();
		__leave;
	}

	// 주어진 문자열을 해시처리한다.
	if (!CryptHashData(
		hHash,
		pbPlainData,
		cbPlainDataLength,
		0
	)) {
		bReturnVal = FALSE;
		m__dwLastErrorCode = GetLastError();
		__leave;
	}

	// 해시된 결과를 얻는다.(CryptGetHashParam)
/*
 * The CryptGetHashParam function completes the hash.
 * After CryptGetHashParam has been called, no more data can be added to the hash.
 * Additional calls to CryptHashData or CryptHashSessionKey fail.
 * After the application is done with the hash,
 * CryptDestroyHash should be called to destroy the hash object.
 */
	DWORD	dwHashSize = m__cbHashedAndEncryptedDatalength;
	// 암호화하는 알고리즘 방식이 블럭방식이라면
	// 해시 데이터의 크기보다 클 수도 있다.
	if (!CryptGetHashParam(
		hHash,
		HP_HASHVAL,
		pbPlainData,	//	BYTE	*pbData		! [IN] pHashData가 기리키는 곳에 해시 테이터를 적는다.
		&dwHashSize,	//	DWORD	*pdwDataLen,	! [IN/OUT] dwDataLength는 버퍼의 크기(바이트 수)이다.
		0
	)) {
		bReturnVal = FALSE;
		m__dwLastErrorCode = GetLastError();
		__leave;
	}
	// 더 이상 이 해시를 이용해서 CryptHashData를 호출할 수 없다.


	if (!CryptEncrypt(
		m__hSessionKey,				//	HCRYPTKEY	hKey		! [IN] 암호화에 사용될 키
		0,					//	HCRYPTHASH	hHash		! [IN] 암화된 데이터는 해시처리 되지 않는다.
		TRUE,					//	BOOL		Final		! [IN] 주어진 데이터가 암호화의 마지막 단계인지 아닌지 말한다.
		0,					//	DWORD		dwFlags		! [IN] The following dwFlags value is defined but reserved for a future release:
							//					!	CRYPT_OAEP	When set with the MS Enhanced Provider and RSA encryption/decryption causes PKCS #1 version 2 formatting to be used.
		pbPlainData,				//	BYTE		*pbData		! [IN/OUT] 암호화될(IN) 데이터가 저장되어(IN) 있고, 암호화되어(OUT) 저장될(OUT) 버퍼를 가리킨다.
		&dwHashSize,				//	DWORD		*pdwDataLen	! [IN/OUT] 암호화될 데이터가 암호화된 후 차지하는 공간의 크기(바이트 수)이다.
		m__cbHashedAndEncryptedDatalength	//	DWORD		dwBufLen	! [IN] pbData가 가리키는 버퍼의 크기(바이트 수)이다.
	)) {
		bReturnVal = FALSE;
		m__dwLastErrorCode = GetLastError();
		//	ERROR_MORE_DATA		234L
		//	NTE_BAD_HASH_STATE	0x8009000CL
		__leave;
	}

	bReturnVal				= TRUE;
} __finally {
	if (hHash) {
		if (!CryptDestroyHash(hHash)) {
			// 여기서 문제가 생기면 어쩌나...
			bReturnVal = FALSE;
			RomeoMustDie();
		}
		hHash = NULL;
	}
}

	return bReturnVal;
}
