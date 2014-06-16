#include <stdio.h>	// printf, sprintf, fprintf
#include <stdlib.h>	// malloc, free
#include <string.h>	// strlen
#include <openssl/evp.h>
#include <openssl/md5.h>
#include "gmCryptAPICore.h"

int	_MD5(
	const unsigned char	*pbMessageData,
	size_t			cbMessageDataLen,
	unsigned char		*pbDigestData		// [OUT]
)
{
	EVP_MD_CTX	mdctx;
	const EVP_MD	*md5 = EVP_md5();	// MD5


/*
 * http://www.openssl.org/docs/crypto/EVP_DigestInit.html#
 * EVP_DigestInit() behaves in the same way as EVP_DigestInit_ex() except the passed context ctx does not have to be initialized, and it always uses the default digest implementation.
 */
#ifdef OPENSSL_096B	// OpenSSL-0.9.6b
	EVP_DigestInit(&mdctx, md5);
#else	// OpenSSL-0.9.7
	EVP_MD_CTX_init(&mdctx);
	if (!EVP_DigestInit_ex(&mdctx, md5, NULL)) {
		fprintf(stderr, "EVP_DigestInit_ex: Failed!\n");
		(void) EVP_MD_CTX_cleanup(&mdctx);
		return -1;
	}
#endif
/*
 * wrong type argument to unary exclamation mark
 * OpenSSL-0.9.6b:
 *	void    EVP_DigestInit(EVP_MD_CTX *ctx, const EVP_MD *type);
 *	void    EVP_DigestUpdate(EVP_MD_CTX *ctx,const void *d, unsigned int cnt);
 *	void    EVP_DigestFinal(EVP_MD_CTX *ctx,unsigned char *md,unsigned int *s);
 * OpenSSL-0.9.7:
 *	int     EVP_DigestFinal_ex(EVP_MD_CTX *ctx,unsigned char *md,unsigned int *s);
 *	int     EVP_Digest(void *data, unsigned int count, unsigned char *md, unsigned int *size, const EVP_MD *type, ENGINE *impl);
 *	int     EVP_DigestFinal(EVP_MD_CTX *ctx,unsigned char *md,unsigned int *s);
 */
#ifdef OPENSSL_096B	// OpenSSL-0.9.6b
	EVP_DigestUpdate(&mdctx, pbMessageData, cbMessageDataLen);
#else	// OpenSSL-0.9.7
	if (!EVP_DigestUpdate(&mdctx, pbMessageData, cbMessageDataLen)) {
		fprintf(stderr, "EVP_DigestUpdate: Failed!\n");
		(void) EVP_MD_CTX_cleanup(&mdctx);
		return -1;
	}
#endif
/*
 * EVP_DigestFinal() is similar to EVP_DigestFinal_ex() except the digest contet ctx is automatically cleaned up.
 */
#ifdef OPENSSL_096B	// OpenSSL-0.9.6b
	EVP_DigestFinal(&mdctx, pbDigestData, NULL);
#else	// Openssl-0.9.7
	if (!EVP_DigestFinal_ex(&mdctx, pbDigestData, NULL)) {
		fprintf(stderr, "EVP_DigestFinal_ex: Failed!\n");
		(void) EVP_MD_CTX_cleanup(&mdctx);
		return -1;
	}
	(void) EVP_MD_CTX_cleanup(&mdctx);
#endif

	return 0;
}

int	GetMessageDigestMD5HexStr(
	const unsigned char	*pbMessageData,
	size_t			cbMessageDataLen,
	unsigned char		*pszDigestedDataHexStr	// [OUT]
)
{
	int		i;
	unsigned char	bDigestedData[MD5_DIGEST_LENGTH];


	if (_MD5(pbMessageData, cbMessageDataLen, bDigestedData) < 0) {
		fprintf(stderr, "_MD5: Failed!\n");
		return -1;
	}
	for (i = 0; i < MD5_DIGEST_LENGTH; ++i) {
		sprintf((char *) pszDigestedDataHexStr + 2 * i, "%02X", bDigestedData[i]);
	}

	return 0;
}


int	_CipherX(
	const EVP_CIPHER	*cipher,
	const unsigned char	*pbPlainData,
	size_t			cbPlainDataLen,
	const unsigned char	*pbPasswordData,
	size_t			cbPasswordDataLen,
	int			iEncryptDecrypt,
	unsigned char		*pbCipherData,		// [OUT]: Sufficient space should be allocated!
	size_t			*pcbCipherDataLen	// [OUT]
)
{
	EVP_CIPHER_CTX	ctx;
	unsigned char	bKeyData[MD5_DIGEST_LENGTH];	// 16bytes
	unsigned char	bIVData[MD5_DIGEST_LENGTH];	// 16bytes
	int		iOutLen;
	int		iFinalLen;


	if (_MD5(pbPasswordData, cbPasswordDataLen, bKeyData) < 0) {
		fprintf(stderr, "_MD5: Failed!\n");
		return -1;
	}
#if 0
	int	i;
	printf("-K: ");
	for (i = 0; i < MD5_DIGEST_LENGTH; ++i) {
		printf("%02X", bKeyData[i]);
	}
	printf("\n");
#endif
	if (_MD5(bKeyData, MD5_DIGEST_LENGTH, bIVData) < 0) {
		fprintf(stderr, "_MD5: Failed!\n");
		return -1;
	}
#if 0
	printf("-iv: ");
	for (i = 0; i < MD5_DIGEST_LENGTH; ++i) {
		printf("%02X", bIVData[i]);
	}
	printf("\n");
#endif
	/* Don't set key or IV because we will modify the parameters */
#if 0	// always set key/IV ONCE!
	EVP_CIPHER_CTX_init(&ctx);
	if (!EVP_CipherInit_ex(&ctx, cipher, NULL, NULL, NULL, iEncryptDecrypt)) {
		(void) EVP_CIPHER_CTX_cleanup(&ctx);
		fprintf(stderr, "EVP_CipherInit_ex: Failed!\n");
		return -1;
	}
#endif
#if 0
	printf("%d\n", EVP_MAX_BLOCK_LENGTH);
	// 32
	printf("%d %d\n", EVP_CIPHER_key_length(cipher), EVP_CIPHER_CTX_key_length(&ctx));
	// 16, 16
	printf("%d %d\n", EVP_CIPHER_iv_length(cipher), EVP_CIPHER_CTX_iv_length(&ctx));
	// 8, 8
#endif
/*
	if (!EVP_CIPHER_CTX_set_key_length(&ctx, 10)) {
		(void) EVP_CIPHER_CTX_cleanup(&ctx);
		fprintf(stderr, "EVP_CIPHER_CTX_set_key_length: Failed!\n");
		return -1;
	}
 */
	/* We finished modifying parameters so now we can set key and IV */
/*
 * http://www.openssl.org/docs/crypto/EVP_EncryptInit.html#
 * EVP_EncryptInit(), EVP_DecryptInit() and EVP_CipherInit() behave in a similar way to EVP_EncryptInit_ex(), EVP_DecryptInit_ex and EVP_CipherInit_ex() except the ctx paramter does not need to be initialized and they always use the default cipher implementation.
 */
#ifdef OPENSSL_096B	// OpenSSL-0.9.6b
	if (!EVP_CipherInit(&ctx, cipher, bKeyData, bIVData, iEncryptDecrypt)) {
		fprintf(stderr, "EVP_CipherInit: Failed!\n");
		(void) EVP_CIPHER_CTX_cleanup(&ctx);
		return -1;
	}
#else	// OpenSSL-0.9.7
	EVP_CIPHER_CTX_init(&ctx);
	if (!EVP_CipherInit_ex(&ctx, cipher, NULL, bKeyData, bIVData, iEncryptDecrypt)) {
		fprintf(stderr, "EVP_CipherInit_ex: Failed!\n");
		(void) EVP_CIPHER_CTX_cleanup(&ctx);
		return -1;
	}
#endif

/*
 * warning: passing arg 4 of `EVP_CipherUpdate' discards qualifiers from pointer target type
 */
/*
 * OpenSSL-0.9.6b:
 *	int     EVP_CipherUpdate(EVP_CIPHER_CTX *ctx, unsigned char *out, int *outl, unsigned char *in, int inl);
 * OpenSSL-0.9.7:
 *	int     EVP_CipherUpdate(EVP_CIPHER_CTX *ctx, unsigned char *out, int *outl, const unsigned char *in, int inl);
 */
#ifdef OPENSSL_096B	// OpenSSL-0.9.6b
	if(!EVP_CipherUpdate(&ctx, pbCipherData, &iOutLen, (unsigned char *) pbPlainData, cbPlainDataLen)) {
		fprintf(stderr, "EVP_CipherUpdate: Failed!\n");
		(void) EVP_CIPHER_CTX_cleanup(&ctx);
		return -1;
	}
#else	// OpenSSL-0.9.7
	if(!EVP_CipherUpdate(&ctx, pbCipherData, &iOutLen, pbPlainData, cbPlainDataLen)) {
		fprintf(stderr, "EVP_CipherUpdate: Failed!\n");
		(void) EVP_CIPHER_CTX_cleanup(&ctx);
		return -1;
	}
#endif
	*pcbCipherDataLen = iOutLen;
/*
 * EVP_EncryptFinal(), EVP_DecryptFinal() and EVP_CipherFinal() behave in a similar way to EVP_EncryptFinal_ex(), EVP_DecryptFinal_ex() and EVP_CipherFinal_ex() except ctx is automatically cleaned up after the call.
 */
#ifdef OPENSSL_096B	// OpenSSL-0.9.6b
	if(!EVP_CipherFinal(&ctx, pbCipherData + iOutLen, &iFinalLen)) {
		fprintf(stderr, "EVP_CipherFinal: Failed!\n");
		(void) EVP_CIPHER_CTX_cleanup(&ctx);
		return -1;
	}
#else	// OpenSSL-0.9.7
	if(!EVP_CipherFinal_ex(&ctx, pbCipherData + iOutLen, &iFinalLen)) {
		fprintf(stderr, "EVP_CipherFinal_ex: Failed!\n");
		(void) EVP_CIPHER_CTX_cleanup(&ctx);
		return -1;
	}
#endif
	*pcbCipherDataLen += iFinalLen;

	(void) EVP_CIPHER_CTX_cleanup(&ctx);

	return 0;
}

int	GetRC2EncryptedHexStr(
	const unsigned char	*pbPlainData,
	size_t			cbPlainDataLen,
	const unsigned char	*pbPasswordData,
	size_t			cbPasswordDataLen,
	unsigned char		*pszRC2EncryptedHexStr	// [OUT]
)
{
	int		i;
	unsigned char	*pbCipherData;
	size_t		cbCipherDataLen = 0;


	if (!(pbCipherData = (unsigned char *) malloc(cbPlainDataLen + EVP_MAX_BLOCK_LENGTH))) {
		fprintf(stderr, "malloc: Failed!\n");
		return 1;
	}
	if (_CipherX(EVP_rc2_cbc(), pbPlainData, cbPlainDataLen, pbPasswordData, cbPasswordDataLen, 1, pbCipherData, &cbCipherDataLen) < 0) {
		fprintf(stderr, "_CipherX: Failed!\n");
		return 1;
	}
	for (i = 0; i < (int) cbCipherDataLen; ++i) {
		sprintf((char *) pszRC2EncryptedHexStr + 2 * i, "%02X", pbCipherData[i]);
	}

	return 0;
}

int	GetRC2DecryptedData(
	const unsigned char	*pszRC2EncryptedHexStr,
	size_t			cbRC2EncryptedHexStrLen,
	const unsigned char	*pbPasswordData,
	size_t			cbPasswordDataLen,
	unsigned char		*pbRC2DecryptedData,
	size_t			*pcbRC2DecryptedDataLen
)
{
	int		i;
	size_t		cbRC2EncryptedDataLen = cbRC2EncryptedHexStrLen >> 1;
	unsigned char	*pbRC2EncryptedData;


	if (!(pbRC2EncryptedData = (unsigned char *) malloc(cbRC2EncryptedDataLen))) {
		fprintf(stderr, "malloc: Failed!\n");
		return -1;
	}
	for (i = 0; i < (int) cbRC2EncryptedDataLen; ++i) {
		unsigned char	bHigh = pszRC2EncryptedHexStr[2 * i];
		unsigned char	bLow = pszRC2EncryptedHexStr[2 * i + 1];
		pbRC2EncryptedData[i] = (bHigh < 'A' ? bHigh - '0' : 10 + (bHigh - 'A')) << 4 | (bLow < 'A' ? bLow - '0' : 10 + (bLow - 'A'));
	}
	if (_CipherX(EVP_rc2_cbc(), pbRC2EncryptedData, cbRC2EncryptedDataLen, pbPasswordData, cbPasswordDataLen, 0, pbRC2DecryptedData, pcbRC2DecryptedDataLen) < 0) {
		fprintf(stderr, "_CipherX: Failed!\n");
		free(pbRC2EncryptedData);
		return 1;
	}
	free(pbRC2EncryptedData);

	return 0;
}
