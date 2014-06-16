#include "CZip.h"

#ifdef __TRACE__
#include <stdio.h>	// fprintf
#endif	// __TRACE__

CZip::CZip(
	CBuffer*	pBuffer
)
{
	if (pBuffer) {
		m__autoFree = 0;
		m__pBuffer = pBuffer;
	} else {
		m__autoFree = 1;
		m__pBuffer = new CBuffer;
	}
}

CZip::~CZip()
{
	if (m__autoFree && m__pBuffer) {
		delete m__pBuffer;

		m__pBuffer = NULL;
	}
}


// m__pBuffer 가 가리키는 곳의 끝에 압축한 결과는 넣는다.
// m__pBuffer 를 관리해야 하는 것은 이 함수를 호출한 함수의 몫이다.
int
CZip::compress(
	const Bytef*	buf,
	size_t		len
)
{
	int	ret;
	z_stream	strm;
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	ret = deflateInit(&strm, Z_DEFAULT_COMPRESSION);
	if (ret != Z_OK) {
#ifdef __TRACE__
		(void) fprintf(stderr, "CZip::compress(): deflataInit(): ret != Z_OK: Failure\n");
#endif	// __TRACE__

		return -1;
	}

	strm.avail_in = len;
	int	flush = Z_FINISH;	// 여기서 압축을 멈추지 않을거라면 Z_NO_FLUSH 라고 할 수도 있다.
	strm.next_in = const_cast<Bytef*>(buf);

	do {
		// 적어도 얼마의 공간(256바이트)은 확보한다.
		if (!this->m__pBuffer->setBufSizeAtLeast(256)) {
#ifdef __TRACE__
			(void) fprintf(stderr, "CZip::compress(): CBuffer::setBufSizeAtLeast(): Failure\n");
#endif	// __TRACE__
			// zlib 가 사용하는 메모리를 해제한다.
			(void) deflateEnd(&strm);

			return -2;
		}
		size_t	n = this->m__pBuffer->getBufSizeForNewData();		// 여유 공간의 크기
		strm.avail_out = n;						// 쓰여질 공간의 총 크기
		strm.next_out = (Bytef*) this->m__pBuffer->getBufForNewData();	// 쓰여질 공간의 시작 위치
		ret = deflate(&strm, flush);
// zpipe.c 에서는 다음과 같이 assert 문을 사용했다.
//		assert(ret != Z_STREAM_ERROR);	/* state not clobbered */
		if (ret == Z_STREAM_ERROR) {
#ifdef __TRACE__
			(void) fprintf(stderr, "CZip::compressed(): deflate() == Z_STREAM_ERROR: Failure\n");
#endif	// __TRACE__
			// zlib 가 사용하는 메모리를 해제한다.
			(void) deflateEnd(&strm);

			return -3;
		}
		size_t	nCompressed = n - strm.avail_out;			// 압축된 데이터의 크기
		(void*) this->m__pBuffer->incDataSize(nCompressed);
	} while (strm.avail_out == 0);					// 아직도 압축될 데이터가 남아 있을 수 있다.
// zpipe.c 에서는 다음과 같이 assert 문을 사용했다.
//	assert(ret == Z_STREAM_END);	/* stream will be complete */
	if (ret != Z_STREAM_END) {
#ifdef __TRACE__
		(void) fprintf(stderr, "CZip::compress(): ret != Z_STREAM_END: Failure\n");
#endif	// __TRACE__
			// zlib 가 사용하는 메모리를 해제한다.
			(void) deflateEnd(&strm);

		return -4;
	}

	// zlib 가 사용하는 메모리를 해제한다.
	(void) deflateEnd(&strm);

	return 0;
}

// m__pBuffer 가 가리키는 곳의 끝에 압축을 푼 결과는 넣는다.
// m__pBuffer 를 관리해야 하는 것은 이 함수를 호출한 함수의 몫이다.
int
CZip::uncompress(
	const Bytef*	buf,
	size_t		len
)
{
	int	ret;
	z_stream	strm;
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	strm.avail_in = 0;
	strm.next_in = Z_NULL;
	ret = inflateInit(&strm);
	if (ret != Z_OK) {
#ifdef __TRACE__
		(void) fprintf(stderr, "CZip::uncompress(): inflateInit(): ret != Z_OK: Failure\n");
#endif	// !__TRACE__

		return -1;
	}
	strm.avail_in = len;
	strm.next_in = const_cast<Bytef*>(buf);
	do {
		// 적어도 얼마의 공간(256바이트)은 확보한다.
		if (!this->m__pBuffer->setBufSizeAtLeast(256)) {
#ifdef __TRACE__
			(void) fprintf(stderr, "CZip::uncompress(): CBuffer::setBufSizeAtLeast(): Failure\n");
#endif	// __TRACE__
			// zlib 가 사용하는 메모리를 해제한다.
			(void) inflateEnd(&strm);

			return -2;
		}
		size_t	n = this->m__pBuffer->getBufSizeForNewData();
		strm.avail_out = n;						// 쓰여질 공간의 총 크기
		strm.next_out = (Bytef*) this->m__pBuffer->getBufForNewData();	// 쓰여질 공간의 시작 위치
		ret = inflate(&strm, Z_NO_FLUSH);
// zpipe.c 에서는 다음과 같이 assert 문을 사용했다.
//		assert(ret != Z_STREAM_ERROR);	/* state not clobbered */
		if (ret == Z_STREAM_ERROR) {
#ifdef __TRACE__
			(void) fprintf(stderr, "CZip::uncompressed(): inflate() == Z_STREAM_ERROR: Failure\n");
#endif	// __TRACE__
			// zlib 가 사용하는 메모리를 해제한다.
			(void) inflateEnd(&strm);

			return -3;
		}
		switch (ret) {
		case Z_NEED_DICT:
			ret = Z_DATA_ERROR;     /* and fall through */
		case Z_DATA_ERROR:
		case Z_MEM_ERROR:
#ifdef __TRACE__
			(void) fprintf(stderr, "CZip::uncompressed(): inflate() == Z_STREAM_ERROR: Failure\n");
#endif	// __TRACE__
			// zlib 가 사용하는 메모리를 해제한다.
			(void) inflateEnd(&strm);

			return -4;
		}
		size_t	nUncompressed = n - strm.avail_out;		// 압축이 풀린 데이터의 크기
		(void*) this->m__pBuffer->incDataSize(nUncompressed);
	} while (strm.avail_out == 0);				// 아직도 덜 풀린 데이터가 남아 있을 수 있다.

	// zlib 가 사용하는 메모리를 해제한다.
	(void) inflateEnd(&strm);

	return 0;
}
