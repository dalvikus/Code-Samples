#include <gmTCP/gmTCP.h>

/*
 * 서버인증에 사용할 인증서를 가져온다.
 * 이 인증서에는 개인키가 포함되어있어야 한다.
 * "인증서.TXT"에 이러한 인증서를 만들고,
 * 관리하는 방법과 예가 있다.
 *
 * 이 함수는 서버에서만 사용한다.
 * 인증서를 가져오는 저장소는 MY를 사용한다.
 */
static
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

gmTCPBase::gmTCPBase(
	gmCONN	*lpConnV,
	UINT	nConnC,
#ifdef __gmUSE_PUBLIC_QUEUE__
	UINT	PubSize,
#endif	// __gmUSE_PUBLIC_QUEUE__
	UINT	MaxTCPMsgSize,
	DWORD	dwMsgUnit,	// 매 dwMsgUnit메시지마다 알려준다. 

	BOOL	bUseConsole
) : myPRINT(bUseConsole)
{
	INT	iReturnVal = 0;

// 2005/4/5 (EST) - ghkim
#ifdef __gmTLS__
	m__bValidCredential	= FALSE;	// 유효한 Credential이 아니다.
	m__TLSLastErrorCode	= 0;

	m__hModule = LoadLibrary(TEXT("Secur32.dll"));
	if (!m__hModule) {
		iReturnVal = -1;
		goto NEXT;
	}
	INIT_SECURITY_INTERFACE	pInitSecurityInterface;
	pInitSecurityInterface = (INIT_SECURITY_INTERFACE) GetProcAddress(
		m__hModule,
#ifdef UNICODE
		"InitSecurityInterfaceW"
#else	// UNICODE
		"InitSecurityInterfaceA"
#endif	// !UNICODE
	);
	if (!pInitSecurityInterface) {
		iReturnVal = -1;
		goto NEXT;
	}
	m__pSFT = pInitSecurityInterface();
	if (!m__pSFT) {
		iReturnVal = -1;
		goto NEXT;
	}
#endif	// __gmTLS__

#ifdef __gmCRYPT__
	m__dwCryptLastErrorCode	= 0;
	m__hCryptProv		= 0;
	HCRYPTHASH	hEncryptHash	= 0;	// 문자열의 해시를 구하고, 이로부터 암호화 키를 얻는다.
	m__hEncryptKey		= 0;
	HCRYPTHASH	hDecryptHash = 0;
	m__hDecryptKey		= 0;
	if (!CryptAcquireContext(
		&m__hCryptProv,
		NULL,
		NULL,
		PROV_RSA_FULL,
		CRYPT_VERIFYCONTEXT
	)) {
		iReturnVal = -1;
		m__dwCryptLastErrorCode = GetLastError();
		goto NEXT;
	}

	// 암호화 키를 위한 해시오브젝트를 만든다.
	if (!CryptCreateHash(
		m__hCryptProv,
		CALG_MD5,
		0,
		0,
		&hEncryptHash
	)) {
		iReturnVal = -1;
		m__dwCryptLastErrorCode = GetLastError();
		goto NEXT;
	}
	// 문자열을 해시처리한다.
	if (!CryptHashData(
		hEncryptHash,
		(CONST BYTE *) "hello, world",
		12,
		0
	)) {
		iReturnVal = -1;
		m__dwCryptLastErrorCode = GetLastError();
		goto NEXT;
	}
	// 암호화 키를 만든다.
	if (!CryptDeriveKey(
		m__hCryptProv,
		CALG_RC4,	// Stream Cipher Algorithm
		hEncryptHash,
		0,
		&m__hEncryptKey
	)) {
		iReturnVal = -1;
		m__dwCryptLastErrorCode = GetLastError();
		goto NEXT;
	}

	// 암호화 키를 위한 해시오브젝트를 만든다.
	if (!CryptCreateHash(
		m__hCryptProv,
		CALG_MD5,
		0,
		0,
		&hDecryptHash
	)) {
		iReturnVal = -1;
		m__dwCryptLastErrorCode = GetLastError();
		goto NEXT;
	}
	// 문자열을 해시처리한다.
	if (!CryptHashData(
		hDecryptHash,
		(CONST BYTE *) "hello, world",
		12,
		0
	)) {
		iReturnVal = -1;
		m__dwCryptLastErrorCode = GetLastError();
		goto NEXT;
	}
	// 암호화 키를 만든다.
	if (!CryptDeriveKey(
		m__hCryptProv,
		CALG_RC4,	// Stream Cipher Algorithm
		hDecryptHash,
		0,
		&m__hDecryptKey
	)) {
		iReturnVal = -1;
		m__dwCryptLastErrorCode = GetLastError();
		goto NEXT;
	}
#endif	// __gmCRYPT__

// 2005/3/24 (EST) - ghkim
//	m__dwUID	= 0(클라이언트) 또는 1(서버);
	m__dwMsgUnit	= dwMsgUnit;

	m__nConnC	= nConnC;
	m__MaxTCPMsgSize	= MaxTCPMsgSize;
	m__RecvSize = (int) ceil(log((float) MaxTCPMsgSize) / log(2.f)) + 1;
	m__SendSize = m__RecvSize + 1;
#ifdef __gmUSE_PUBLIC_QUEUE__
#else	// __gmUSE_PUBLIC_QUEUE__
	m__RecvSize += 4;
#endif	// !__gmUSE_PUBLIC_QUEUE__

	// 모든 값을 초기화한다.
#ifdef __gmUSE_PUBLIC_QUEUE__
	m__lpPublicQueue	= NULL;
#endif	// __gmUSE_PUBLIC_QUEUE__
#ifdef __gmMESSAGE_IOCP__
	m__hMessageIOCP		= NULL;
#else	// __gmMESSAGE_IOCP__
	m__hEventNotify		= NULL;
#endif	// !__gmMESSAGE_IOCP__
	m__hEventWorkerToManager	= NULL;
	m__lpCriticalSection	= NULL;
	m__lpConnV	= NULL;

	m__lpConnV = new gmCONN[nConnC];
	if (!m__lpConnV) {
		iReturnVal = -1;
		goto NEXT;
	}
	UINT	i;
	for (i = 0; i < nConnC; ++i) {
		m__lpConnV[i] = lpConnV[i];
	}

	UINT	MaxConn;
	MaxConn	= 0;
	UINT	globalIndex;
	for (globalIndex = 0; globalIndex < nConnC; ++globalIndex)
		MaxConn += lpConnV[globalIndex].MaxConn;

#ifdef __gmUSE_PUBLIC_QUEUE__
	if (PubSize > 0)
		m__PubSize = PubSize;
	else
		m__PubSize = (int) ceil(log(MaxConn) / log(2.)) + m__RecvSize;
	mf__printf(
		TEXT("%d %d %d(%d)\n")
		,
		m__RecvSize,
		m__SendSize,
		m__PubSize,
		MaxConn
	);
#endif	// __gmUSE_PUBLIC_QUEUE__

	m__NConn	= 0;

	m__maxConn = MaxConn;
	m__lpgmPseudoTCP = new gmPseudoTCP[MaxConn];
	if (m__lpgmPseudoTCP == NULL) {
		mf__fprintf(m__CONOUT,
			TEXT("gmTCPBase::gmTCPBase: new gmPseudoTCP[]: NULL\n")
		);
		iReturnVal = -1;
		goto NEXT;
	}

	// 초기화
	UINT	iTotalIndex, myMaxConn;
	for (
		iTotalIndex = 0, globalIndex = 0, myMaxConn = 0;
		globalIndex < nConnC;
		++globalIndex, iTotalIndex += myMaxConn
	) {
		myMaxConn = lpConnV[globalIndex].MaxConn;
		for (UINT localIndex = 0; localIndex < myMaxConn; ++localIndex) {
			gmPseudoTCP&	gmhTCP = m__lpgmPseudoTCP[iTotalIndex + localIndex];
////////////////////////////////////////////////////////////////////////////////
			if (gmhTCP.mf__Initialize(
				this,
				globalIndex,
				localIndex,
				iTotalIndex + localIndex,
				m__RecvSize,	//lpConnV[globalIndex].RecvSize,
				m__SendSize	//lpConnV[globalIndex].SendSize
			) != 0) {
				// 2003/10/30
				// 각 연결을 준비할 수 없었다.
				// 처음 연결을 시도할 때이므로
				// 우선 프로그램을 종료하고 원인을 분석한다.
				// 문제가 해결되면 프로그램이 동작 중에는
				// 이 부분에서 큰 문제가 생기지 않을 것이다.
				mf__fprintf(m__CONERR,
					TEXT("gmTCPBase::gmTCPBase: gmPseudoTCP::mf__Initialize failed!\n")
				);
				iReturnVal = -1;
				goto NEXT;
			}
////////////////////////////////////////////////////////////////////////////////
		}
	}

#ifdef __gmUSE_PUBLIC_QUEUE__
	INT	myReturnVal;
	m__lpPublicQueue = new gmQueue(
		m__PubSize,	//PubSize,
		&myReturnVal
	);
	if (!m__lpPublicQueue || myReturnVal == -1) {
		mf__fprintf(m__CONERR,
			TEXT("gmTCPBase::gmTCPBase: m__lpPublicQueue = new gmQueue(...) failed!\n")
		);
		iReturnVal = -1;
		goto NEXT;
	}
#ifdef __gmTCP_DEBUG__
	m__MaxPublicQueueLength = 0;
#endif	// __gmTCP_DEBUG__
#endif	// __gmUSE_PUBLIC_QUEUE__

#ifdef __gmMESSAGE_IOCP__
	m__hMessageIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (!m__hMessageIOCP) {
		// GetLastError
		mf__OutputDebugString(TEXT("gmBase::gmBase: HANDLE CreateIoCompletionPort(...): NULL: 0x%08X\n"), GetLastError());
		iReturnVal = -1;
		goto NEXT;
	}
#else	// __gmMESSAGE_IOCP__
	if ((m__hEventNotify = CreateEvent(NULL, FALSE, FALSE, NULL)) == NULL) {
		iReturnVal = -1;
		goto NEXT;
	}
#endif	// !__gmMESSAGE_IOCP__

	if ((m__hEventWorkerToManager = CreateEvent(NULL, FALSE, FALSE, NULL)) == NULL) {
		iReturnVal = -1;
		goto NEXT;
	}

	m__lpCriticalSection = new CRITICAL_SECTION;
	if (!m__lpCriticalSection) {
		iReturnVal = -1;
		goto NEXT;
	}
#if	defined(_WIN32_WINNT) && (_WIN32_WINNT > 0x0403)
	SYSTEM_INFO	si;
	GetSystemInfo(&si);
	if (si.dwNumberOfProcessors > 1) {
		// See the document for InitializeCriticalSectionAndSpinCount.
		if (!InitializeCriticalSectionAndSpinCount(m__lpCriticalSection, 4000)) {
			iReturnVal = -1;
			goto NEXT;
		}
	} else {
		InitializeCriticalSection(m__lpCriticalSection);
	}
#else	// defined(_WIN32_WINNT) && (_WIN32_WINNT > 0x0403)
	InitializeCriticalSection(m__lpCriticalSection);
#endif	// !defined(_WIN32_WINNT) || !(_WIN32_WINNT > 0x0403)

NEXT:
#ifdef __gmCRYPT__
	if (hEncryptHash) {
		if (!CryptDestroyHash(hEncryptHash)) {
			iReturnVal = -1;
			m__dwCryptLastErrorCode = GetLastError();
		}
		hEncryptHash = 0;
	}
	if (hDecryptHash) {
		if (!CryptDestroyHash(hDecryptHash)) {
			iReturnVal = -1;
			m__dwCryptLastErrorCode = GetLastError();
		}
		hDecryptHash = 0;
	}
#endif	// __gmCRYPT__
	if (iReturnVal == -1) {
		mf__releaseBase();
	}

	m__baseStatus = iReturnVal;
}

gmTCPBase::~gmTCPBase()
{
	mf__releaseBase();
}

VOID
gmTCPBase::mf__releaseBase(
	VOID
)
{
#ifdef __gmTLS__
	if (m__hModule) {
		if (!FreeLibrary(m__hModule)) {
			// 여기서 문제가 생기면 어떻게 하나...
			mf__RomeoMustDie();
		}
		m__hModule = 0;
	}
	// Credential핸들에 할당된 메모리를 해제한다.
	if (m__bValidCredential && m__pSFT && m__pSFT->FreeCredentialsHandle(&m__hCredential) != SEC_E_OK) {
		// 여기서 문제가 생기면 어떻게 하나...
		mf__RomeoMustDie();
		m__pSFT = 0;
	}
#endif	// __gmTLS__

#ifdef __gmCRYPT__
	if (m__hCryptProv) {
		if (m__hEncryptKey) {
			if (!CryptDestroyKey(m__hEncryptKey)) {
				m__dwCryptLastErrorCode = GetLastError();
			}
			m__hEncryptKey = 0;
		}
		if (m__hDecryptKey) {
			if (!CryptDestroyKey(m__hDecryptKey)) {
				m__dwCryptLastErrorCode = GetLastError();
			}
			m__hDecryptKey = 0;
		}

		if (!CryptReleaseContext(m__hCryptProv, 0)) {
			m__dwCryptLastErrorCode = GetLastError();
		}
		m__hCryptProv = 0;
	}
#endif	// __gmCRYPT__

#ifdef __gmTCP_DEBUG__
	mf__fprintf(m__CONERR,
		TEXT("# of max. bytes in queue = %u / %u\n")
		,
		m__MaxPublicQueueLength,
		m__lpPublicQueue->m__size
	);
#endif	// __gmTCP_DEBUG__
	if (m__lpgmPseudoTCP) {
		UINT	MaxConn	= 0;
		for (UINT globalIndex = 0; globalIndex < m__nConnC; ++globalIndex)
			MaxConn += m__lpConnV[globalIndex].MaxConn;
		UINT	N_disconnected = 0;
		for (UINT i = 0; i < MaxConn; ++i) {
			if (N_disconnected == MaxConn)
				break;
			gmPseudoTCP&	gmhTCP = m__lpgmPseudoTCP[i];
			if (gmhTCP.mf__GetPseudoTCPStatus() & gmPseudoTCP::gmPseudoTCPSTATUS__NOT_CONNECTED) {
				gmhTCP.mf__Finalize();
				++N_disconnected;
			}
		}

		delete[] m__lpgmPseudoTCP;
		m__lpgmPseudoTCP = NULL;
	}

	delete m__lpConnV;
	m__lpConnV = NULL;

#ifdef __gmMESSAGE_IOCP__
	if (m__hMessageIOCP) {
		CloseHandle(m__hMessageIOCP);		// 리턴값은 무시
		m__hMessageIOCP = NULL;
	}
#else	// __gmMESSAGE_IOCP__
	if (m__hEventNotify != NULL) {
		CloseHandle(m__hEventNotify);		// 리턴값은 무시
		m__hEventNotify = NULL;
	}
#endif	// !__gmMESSAGE_IOCP__

	if (m__hEventWorkerToManager != NULL) {
		CloseHandle(m__hEventWorkerToManager);	// 리턴값은 무시
		m__hEventWorkerToManager = NULL;
	}

	if (m__lpCriticalSection) {
		DeleteCriticalSection(m__lpCriticalSection);
		delete m__lpCriticalSection;
		m__lpCriticalSection = NULL;
	}

#ifdef __gmUSE_PUBLIC_QUEUE__
	if (m__lpPublicQueue) {
		delete m__lpPublicQueue;
		m__lpPublicQueue = NULL;
	}
#endif	// __gmUSE_PUBLIC_QUEUE__
};

#ifdef __gmUSE_PUBLIC_QUEUE__
/*
 * INPUT
 *	LPTCPPBUF
 *		OUT	LPTCPPBUF->gmhTCP
 *		OUT	LPTCPPBUF->TCPBUF.Header.hType
 *		IN/OUT	LPTCPPBUF->TCPBUF.Header.cbData
 *		IN	LPTCPPBUF->TCPBUF.pbData
 *
 * RETURN
 *	>= 0	Ok	= LPTCPPBUF->TCPBUF.Header.cbData	// # of bytes
 *	< 0	Failure
 *			TCPBUF__NO_HEADER
 *			TCPBUF__PARTIAL_DATA
 *			TCPBUF__NOT_ENOUGH
 */
INT
gmTCPBase::mf__peek(
	LPTCPPBUF	lpTCPPBuf,
	PUINT		lpMsgLen
)
{
	mf__lock();

	INT	iReturnVal;		// 리턴값
	// lpTCPPBuf->TCPBuf.Header.cbData (IN/OUT)
	iReturnVal = m__lpPublicQueue->mf__ShiftPEx(lpTCPPBuf, lpMsgLen);
	if (iReturnVal == TCPBUF__OK && !lpMsgLen) {
#ifdef __gmIOCP_DEBUG_PUBLIC_QUEUE__
		mf__fprintf(m__CONERR,
			TEXT("<- Public Queue: %u (%.2f%%)\n")
			,
			m__lpPublicQueue->m__length,
			1. * m__lpPublicQueue->m__length / m__lpPublicQueue->m__size * 100
		);
#endif	// __gmIOCP_DEBUG_PUBLIC_QUEUE__
	}

	mf__unlock();
	return iReturnVal;
}

VOID
gmTCPBase::mf__Shift(
	UINT	msgLen
)
{
	mf__lock();

	m__lpPublicQueue->mf__ShiftEx(msgLen);
#ifdef __gmIOCP_DEBUG_PUBLIC_QUEUE__
	mf__fprintf(m__CONERR,
		TEXT("<- Public Queue: %u (%.2f%%)\n")
		,
		m__lpPublicQueue->m__length,
		1. * m__lpPublicQueue->m__length / m__lpPublicQueue->m__size * 100
	);
#endif	// __gmIOCP_DEBUG_PUBLIC_QUEUE__

	mf__unlock();
	return;
}
#endif	// __gmUSE_PUBLIC_QUEUE__

VOID
gmTCPBase::mf__flushAll(
	VOID
)
{
	INT	iReturnVal = 0;

	UINT	iTotalIndex, globalIndex, myMaxConn;
	for (
		iTotalIndex = 0, globalIndex = 0, myMaxConn = 0;
		globalIndex < m__nConnC;
		++globalIndex, iTotalIndex += myMaxConn
	) {
		myMaxConn = m__lpConnV[globalIndex].MaxConn;
		for (UINT index = 0; index < myMaxConn; ++index) {
			gmPseudoTCP&	gmhTCP = m__lpgmPseudoTCP[iTotalIndex + index];
			// 2003/10/30
			// 연결에 문제가 있어 그 연결을 끊는다.
			if (gmhTCP.mf__flush() < 0) {
				mf__fprintf(m__CONERR,
					TEXT("INFO: ")
					TEXT("gmTCPBase::mf__flushAll: ")
					TEXT("gmPseudoTCP::mf__flush failed: ")
					TEXT("특정 연결(%u.%u) 연결을 끊는다.\n")
					,
					gmhTCP.m__globalIndex,
					gmhTCP.m__localIndex
				);
				if (gmhTCP.mf__disconnect() < 0) {	// blocking
					// 2003/10/30
					// 끊을 때에 문제가 생긴다면...
					mf__fprintf(m__CONERR,
						TEXT("ERROR: ")
						TEXT("gmTCPBase::mf__flushAll: ")
						TEXT("gmPseudoTCP::mf__disconnect failed: ")
						TEXT("특정 연결(%u.%u)을 끊을 수 없다.\n")
						,
						gmhTCP.m__globalIndex,
						gmhTCP.m__localIndex
					);
				}
			}
		}
	}
}

/*
 * 3/19/2005 - ghkim
 *	1. 변수의 이름을 바꾸었다.
 *		lpBuf	-> pszMessage
 *		mylpBuf	-> pszLocalMessage
 *	2. 버퍼오버플로우를 대비해서 메시지를 저장할 버퍼의 크기를
 *	가리키는 변수를 사용했다.
 *
 * 리턴값:
 *	-1: 버퍼의 크기를 알고자 할 때, pszMessage = NULL로 하면
 *	필요한 버퍼의 크기가 pcchMessage0이 가리키는 곳에 써진다.
 *	-2: 버퍼의 크기가 충분하지 않을 때, 필요한 버퍼의 크기가
 *	pcchMessage0이 가리키는 곳에 써진다.
 *	0보다 크거나 같을 때: 버퍼에 써진 NULL문자를 제외한 문자 수
 */
INT
gmTCPBase::mf__GetWSAErrorStringFromCode(
	_TCHAR	*pszMessage,	// [OUT]
	DWORD	*pcchMessage0,	// [IN/OUT]	! pszMessage버퍼에 들어갈 수 있는 _T('\0')을 포함한 (바이트가 아닌) 문자의 수
	INT	WSAErrorCode	// [IN]
)
{
	_TCHAR	*pszLocalMessage;
	switch (WSAErrorCode) {
	case WSAEINTR:
		pszLocalMessage = _TEXT("WSAEINTR");
		break;
	case WSAEBADF:
		pszLocalMessage = _TEXT("WSAEBADF");
		break;
	case WSAEACCES:
		pszLocalMessage = _TEXT("WSAEACCES");
		break;
	case WSAEFAULT:
		pszLocalMessage = _TEXT("WSAEFAULT");
		break;
	case WSAEINVAL:
		pszLocalMessage = _TEXT("WSAEINVAL");
		break;
	case WSAEMFILE:
		pszLocalMessage = _TEXT("WSAEMFILE");
		break;
	case WSAEWOULDBLOCK:
		pszLocalMessage = _TEXT("WSAEWOULDBLOCK");
		break;
	case WSAEINPROGRESS:
		pszLocalMessage = _TEXT("WSAEINPROGRESS");
		break;
	case WSAEALREADY:
		pszLocalMessage = _TEXT("WSAEALREADY");
		break;
	case WSAENOTSOCK:
		pszLocalMessage = _TEXT("WSAENOTSOCK");
		break;
	case WSAEDESTADDRREQ:
		pszLocalMessage = _TEXT("WSAEDESTADDRREQ");
		break;
	case WSAEMSGSIZE:
		pszLocalMessage = _TEXT("WSAEMSGSIZE");
		break;
	case WSAEPROTOTYPE:
		pszLocalMessage = _TEXT("WSAEPROTOTYPE");
		break;
	case WSAENOPROTOOPT:
		pszLocalMessage = _TEXT("WSAENOPROTOOPT");
		break;
	case WSAEPROTONOSUPPORT:
		pszLocalMessage = _TEXT("WSAEPROTONOSUPPORT");
		break;
	case WSAESOCKTNOSUPPORT:
		pszLocalMessage = _TEXT("WSAESOCKTNOSUPPORT");
		break;
	case WSAEOPNOTSUPP:
		pszLocalMessage = _TEXT("WSAEOPNOTSUPP");
		break;
	case WSAEPFNOSUPPORT:
		pszLocalMessage = _TEXT("WSAEPFNOSUPPORT");
		break;
	case WSAEAFNOSUPPORT:
		pszLocalMessage = _TEXT("WSAEAFNOSUPPORT");
		break;
	case WSAEADDRINUSE:
		pszLocalMessage = _TEXT("WSAEADDRINUSE");
		break;
	case WSAEADDRNOTAVAIL:
		pszLocalMessage = _TEXT("WSAEADDRNOTAVAIL");
		break;
	case WSAENETDOWN:
		pszLocalMessage = _TEXT("WSAENETDOWN");
		break;
	case WSAENETUNREACH:
		pszLocalMessage = _TEXT("WSAENETUNREACH");
		break;
	case WSAENETRESET:
		pszLocalMessage = _TEXT("WSAENETRESET");
		break;
	case WSAECONNABORTED:
		pszLocalMessage = _TEXT("WSAECONNABORTED");
		break;
	case WSAECONNRESET:
		pszLocalMessage = _TEXT("WSAECONNRESET");
		break;
	case WSAENOBUFS:
		pszLocalMessage = _TEXT("WSAENOBUFS");
		break;
	case WSAEISCONN:
		pszLocalMessage = _TEXT("WSAEISCONN");
		break;
	case WSAENOTCONN:
		pszLocalMessage = _TEXT("WSAENOTCONN");
		break;
	case WSAESHUTDOWN:
		pszLocalMessage = _TEXT("WSAESHUTDOWN");
		break;
	case WSAETOOMANYREFS:
		pszLocalMessage = _TEXT("WSAETOOMANYREFS");
		break;
	case WSAETIMEDOUT:
		pszLocalMessage = _TEXT("WSAETIMEDOUT");
		break;
	case WSAECONNREFUSED:
		pszLocalMessage = _TEXT("WSAECONNREFUSED");
		break;
	case WSAELOOP:
		pszLocalMessage = _TEXT("WSAELOOP");
		break;
	case WSAENAMETOOLONG:
		pszLocalMessage = _TEXT("WSAENAMETOOLONG");
		break;
	case WSAEHOSTDOWN:
		pszLocalMessage = _TEXT("WSAEHOSTDOWN");
		break;
	case WSAEHOSTUNREACH:
		pszLocalMessage = _TEXT("WSAEHOSTUNREACH");
		break;
	case WSAENOTEMPTY:
		pszLocalMessage = _TEXT("WSAENOTEMPTY");
		break;
	case WSAEPROCLIM:
		pszLocalMessage = _TEXT("WSAEPROCLIM");
		break;
	case WSAEUSERS:
		pszLocalMessage = _TEXT("WSAEUSERS");
		break;
	case WSAEDQUOT:
		pszLocalMessage = _TEXT("WSAEDQUOT");
		break;
	case WSAESTALE:
		pszLocalMessage = _TEXT("WSAESTALE");
		break;
	case WSAEREMOTE:
		pszLocalMessage = _TEXT("WSAEREMOTE");
		break;
	case WSASYSNOTREADY:
		pszLocalMessage = _TEXT("WSASYSNOTREADY");
		break;
	case WSAVERNOTSUPPORTED:
		pszLocalMessage = _TEXT("WSAVERNOTSUPPORTED");
		break;
	case WSANOTINITIALISED:
		pszLocalMessage = _TEXT("WSANOTINITIALISED");
		break;
	case WSAEDISCON:
		pszLocalMessage = _TEXT("WSAEDISCON");
		break;
	case WSAENOMORE:
		pszLocalMessage = _TEXT("WSAENOMORE");
		break;
	case WSAECANCELLED:
		pszLocalMessage = _TEXT("WSAECANCELLED");
		break;
	case WSAEINVALIDPROCTABLE:
		pszLocalMessage = _TEXT("WSAEINVALIDPROCTABLE");
		break;
	case WSAEINVALIDPROVIDER:
		pszLocalMessage = _TEXT("WSAEINVALIDPROVIDER");
		break;
	case WSAEPROVIDERFAILEDINIT:
		pszLocalMessage = _TEXT("WSAEPROVIDERFAILEDINIT");
		break;
	case WSASYSCALLFAILURE:
		pszLocalMessage = _TEXT("WSASYSCALLFAILURE");
		break;
	case WSASERVICE_NOT_FOUND:
		pszLocalMessage = _TEXT("WSASERVICE_NOT_FOUND");
		break;
	case WSATYPE_NOT_FOUND:
		pszLocalMessage = _TEXT("WSATYPE_NOT_FOUND");
		break;
	case WSA_E_NO_MORE:
		pszLocalMessage = _TEXT("WSA_E_NO_MORE");
		break;
	case WSA_E_CANCELLED:
		pszLocalMessage = _TEXT("WSA_E_CANCELLED");
		break;
	case WSAEREFUSED:
		pszLocalMessage = _TEXT("WSAEREFUSED");
		break;
	case WSAHOST_NOT_FOUND:
		pszLocalMessage = _TEXT("WSAHOST_NOT_FOUND");
		break;
	case WSATRY_AGAIN:
		pszLocalMessage = _TEXT("WSATRY_AGAIN");
		break;
	case WSANO_RECOVERY:
		pszLocalMessage = _TEXT("WSANO_RECOVERY");
		break;
	case WSANO_DATA:
		pszLocalMessage = _TEXT("WSANO_DATA");
		break;
	case WSA_QOS_RECEIVERS:
		pszLocalMessage = _TEXT("WSA_QOS_RECEIVERS");
		break;
	case WSA_QOS_SENDERS:
		pszLocalMessage = _TEXT("WSA_QOS_SENDERS");
		break;
	case WSA_QOS_NO_SENDERS:
		pszLocalMessage = _TEXT("WSA_QOS_NO_SENDERS");
		break;
	case WSA_QOS_NO_RECEIVERS:
		pszLocalMessage = _TEXT("WSA_QOS_NO_RECEIVERS");
		break;
	case WSA_QOS_REQUEST_CONFIRMED:
		pszLocalMessage = _TEXT("WSA_QOS_REQUEST_CONFIRMED");
		break;
	case WSA_QOS_ADMISSION_FAILURE:
		pszLocalMessage = _TEXT("WSA_QOS_ADMISSION_FAILURE");
		break;
	case WSA_QOS_POLICY_FAILURE:
		pszLocalMessage = _TEXT("WSA_QOS_POLICY_FAILURE");
		break;
	case WSA_QOS_BAD_STYLE:
		pszLocalMessage = _TEXT("WSA_QOS_BAD_STYLE");
		break;
	case WSA_QOS_BAD_OBJECT:
		pszLocalMessage = _TEXT("WSA_QOS_BAD_OBJECT");
		break;
	case WSA_QOS_TRAFFIC_CTRL_ERROR:
		pszLocalMessage = _TEXT("WSA_QOS_TRAFFIC_CTRL_ERROR");
		break;
	case WSA_QOS_GENERIC_ERROR:
		pszLocalMessage = _TEXT("WSA_QOS_GENERIC_ERROR");
		break;
	case WSA_IO_PENDING:
		pszLocalMessage = _TEXT("WSA_IO_PENDING");
		break;
	case WSA_IO_INCOMPLETE:
		pszLocalMessage = _TEXT("WSA_IO_INCOMPLETE");
		break;
	case WSA_INVALID_HANDLE:
		pszLocalMessage = _TEXT("WSA_INVALID_HANDLE");
		break;
	case WSA_INVALID_PARAMETER:
		pszLocalMessage = _TEXT("WSA_INVALID_PARAMETER");
		break;
	case WSA_NOT_ENOUGH_MEMORY:
		pszLocalMessage = _TEXT("WSA_NOT_ENOUGH_MEMORY");
		break;
	case WSA_OPERATION_ABORTED:
		pszLocalMessage = _TEXT("WSA_OPERATION_ABORTED");
		break;
	case WSA_INVALID_EVENT:
		pszLocalMessage = _TEXT("WSA_INVALID_EVENT");
		break;
	case WSA_MAXIMUM_WAIT_EVENTS:
		pszLocalMessage = _TEXT("WSA_MAXIMUM_WAIT_EVENTS");
		break;
	case WSA_WAIT_FAILED:
		pszLocalMessage = _TEXT("WSA_WAIT_FAILED");
		break;
//	case WSA_WAIT_EVENT_0:
//		pszLocalMessage = _TEXT("WSA_WAIT_EVENT_0");
//		break;
	case WSA_WAIT_IO_COMPLETION:
		pszLocalMessage = _TEXT("WSA_WAIT_IO_COMPLETION");
		break;
	case WSA_WAIT_TIMEOUT:
		pszLocalMessage = _TEXT("WSA_WAIT_TIMEOUT");
		break;
//	case WSA_INFINITE:
//		pszLocalMessage = _TEXT("WSA_INFINITE");
//		break;
	default:
		pszLocalMessage = _TEXT("Unknown Error Code");
		break;
	}

	DWORD	cchLocalMessage0 = _tcslen(pszLocalMessage) + 1;
	if (!pszMessage) {
		// pcchMessage0는 OUT으로만 사용된다.
		*pcchMessage0 = cchLocalMessage0;
		return -1;
	}
	if (*pcchMessage0 < cchLocalMessage0) {
		// 버퍼의 크기가 충분하지 않다.
		*pcchMessage0 = cchLocalMessage0;	// 필요한 최소한의 크기를 알려준다.
		return -2;
	}
	// 버퍼에 복사한다.
	return _stprintf(pszMessage, _TEXT("%s"), pszLocalMessage);
}

#ifdef __gmUSE_PUBLIC_QUEUE__
#else	// __gmUSE_PUBLIC_QUEUE__
gmPseudoTCP	*
gmTCPBase::mf__nextTCP(
	gmPseudoTCP::gmPseudoTCPSTATUS	gmPseudoTCPStatus
)
{
	while (m__iterIndex < m__maxConn) {
		gmPseudoTCP	*gmhTCP = &m__lpgmPseudoTCP[m__iterIndex++];
		if (gmhTCP->m__gmPseudoTCPStatus & gmPseudoTCPStatus)
			return gmhTCP;
	}

	return NULL;
}
#endif	// !__gmUSE_PUBLIC_QUEUE__
