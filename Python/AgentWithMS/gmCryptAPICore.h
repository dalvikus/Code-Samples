#ifndef __gm_CRYPT_API_CORE_H__
#define __gm_CRYPT_API_CORE_H__
////////////////////////////////////////////////////////////////////////////////

#include <openssl/evp.h>	// EVP_CIPHER
#include <openssl/md5.h>
#ifdef OPENSSL_096B	// OpenSSL-0.9.6b
#define EVP_MAX_BLOCK_LENGTH	32
#else	// OpenSSL-0.9.7
// defined in evp.h
#endif

int	_MD5(
	const unsigned char	*pbMessageData,
	size_t			cbMessageDataLen,
	unsigned char		*pbDigestData		// [OUT]
);
int	GetMessageDigestMD5HexStr(
	const unsigned char	*pbMessageData,
	size_t			cbMessageDataLen,
	unsigned char		*pszDigestDataHexStr	// [OUT]
);

int	_CipherX(
	const EVP_CIPHER	*cipher,
	const unsigned char	*pbPlainData,
	size_t			cbPlainDataLen,
	const unsigned char	*pbPasswordData,
	size_t			cbPasswordDataLen,
	int			iEncryptDecrypt,	// 1: Encrypt, 0: Decrypt
	unsigned char		*pbCipherData,
	size_t			*pcbCipherDataLen
);
int	GetRC2EncryptedHexStr(
	const unsigned char	*pbPlainData,
	size_t			cbPlainDataLen,
	const unsigned char	*pbPasswordData,
	size_t			cbPasswordDataLen,
	unsigned char		*pszRC2EncryptedHexStr	// [OUT]
);
int	GetRC2DecryptedData(
	const unsigned char	*pszRC2EncryptedHexStr,
	size_t			cbRC2EncryptedHexStrLen,
	const unsigned char	*pbPasswordData,
	size_t			cbPasswordDataLen,
	unsigned char		*pbRC2DecryptedData,
	size_t			*cbRC2DecryptedDataLen
);


////////////////////////////////////////////////////////////////////////////////
#endif	// !__gm_CRYPT_API_CORE_H__
