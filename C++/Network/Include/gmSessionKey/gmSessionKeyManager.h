#ifndef __gmSESSION_KEY_MANAGER__
#define __gmSESSION_KEY_MANAGER__
////////////////////////////////////////////////////////////////////////////////


#include <Windows.h>

class gmSessionKeyManager
{
private:
	DWORD	m__dwLastErrorCode;

private:
	// 데이터의 해시값을 얻을 때 사용한다.
	HCRYPTPROV	m__hCryptProv;

#if 1
// 인증서를 사용해서 Session Key를 얻는 경우
	PCCERT_CONTEXT	m__pCertContext;
	BOOL		m__bCallerFreeProv;
	HCRYPTPROV	m__hCryptProvFromCertificate;	// typedef ULONG_PTR HCRYPTPROV;
	HCRYPTKEY	m__hKeyExchangeKey;		// typedef ULONG_PTR HCRYPTPROV;
#else
// TLS를 이용해서 서버로부터 Session Key를 받아오는 경우
#endif

	HCRYPTKEY	m__hSessionKey;

private:
#if 1
// 인증서를 사용해서 Session Key를 얻는 경우
	BOOL	mf__CryptAcquireCertificatePrivateKey(void);
	BOOL	mf__FinalizeCertificate(PDWORD pdwLastErrorCode);
#else
// TLS를 이용해서 서버로부터 Session Key를 받아오는 경우
#endif

	BOOL	mf__SetSessionKey(void);

public:
	gmSessionKeyManager(
		// 2005/4/12 (EST) - ghkim
		// 두 알고리즘은 고정되어 있다.
		ALG_ID	HashAlgID,
		ALG_ID	EncrypAlgID,

		BOOL	*pbStatus,
		PDWORD	pdwLastErrorCode
	);
	~gmSessionKeyManager();

	// 2005/5/12 (EST) - ghkim
	// 이 클래스의 목적이 주어진 문자열을 해시처리해서 암화화한 문자열과,
	// 나중에 주어지는 문자열을 똑같이 처리해서 얻어진 문자열을 비교함으로써,
	// 두 개의 문자열이 같은 지 알아보는 데 있다.
	// 그러기위해서는 해시처리하는 알고리즘이나 암호화하는 알고리즘은 고정되어 있어야한다.
	// 따라서 생성자에서 해시 데이터의 크기나 그 해시 데이터의 암호화된 데이터의 크기를
	// 미리 알 수 있다.
private:
	ALG_ID	m__HashAlgID;		// CALG_MD5, CALG_SHA1
	ALG_ID	m__EncryptionAlgID;	// CALG_RC2(block cipher), CALG_RC4(stream cipher)
	DWORD	m__cbHashedAndEncryptedDatalength;
	BOOL	mf__GetSize(void);
public:
	DWORD	mf__cbHashedAndEncryptedDatalength(void)	{return m__cbHashedAndEncryptedDatalength;}
	BOOL
	mf__GetHashedAndEncryptedData(
		PBYTE	pbPlainData,
		DWORD	cbPlainDataLength,			// [IN] 데이터의 크기
		DWORD	cbPlainDataBufferLength			// [IN] 버퍼의 크기
	);
};


////////////////////////////////////////////////////////////////////////////////
#endif	// !__gm_SESSION_KEY_MANAGER__
