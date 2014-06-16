#include "CBuffer.h"

#ifdef __TRACE__
#include <stdio.h>	// stderr, fprintf
#endif	// __TRACE__
#include <stdlib.h>	// malloc, relloc, free
#include <string.h>	// memcpy

#define BUF_SIZE_UNIT	80	// 최소한 2보다 커야 한다!!!
				// 1. 초기화에 성공했는 지는 처음으로 할당된 버퍼가 있는 지를 보면 안다;
				//	CBuffer.getBufSize() > 0
				// 2. '\n'으로 끝나지 않는 경우를 대비해서
				// fgets에 사용하는 버퍼의 크기는 최소 2이어야 한다.

// Contructor에서 최소단위(BUF_SIZE_UNIT바이트)의 메모리를 할당한다.
CBuffer::CBuffer(
	void
)
{
#ifdef __TRACE__
	(void) fprintf(stderr, "__TRACE__: CBuffer::CBuffer(): malloc...\n");
#endif	// __TRACE__
	m__buf = (char*) malloc(BUF_SIZE_UNIT);
	m__buf_size = m__buf ? BUF_SIZE_UNIT : 0;	// m__size = 0이라면,
							// contructor에서 메모리를 할당할 때 문제가 있었음을 나타낸다.
							// CBuffer::getSize()를 통해 확인할 수 있다.
	m__data_size = 0;
}

#if 0
CBuffer::CBuffer(
	const CBuffer&	c
)
{
	size_t	data_size = c.getDataSize();
	(void) fprintf(stderr, "__TRACE__: CBuffer::CBuffer(const CBuffer&): realloc(): Failure\n");
	char*	newBuf = (char*) realloc(m__buf, data_size);
	if (!newBuf) {
		// 메모리 할당에 문제가 있다.
#ifdef __TRACE__
		(void) fprintf(stderr, "__TRACE__: CBuffer::CBuffer(const CBuffer&): realloc(): Failure\n");
#endif	// __TRACE__

		return;
	}

	m__buf = newBuf;
	m__buf_size = data_size;
	m__data_size = data_size;
	(void) memcpy(m__buf, c.getBuf(), data_size);
}
#endif

CBuffer::~CBuffer()
{
	if (m__buf) {
#ifdef __TRACE__
		(void) fprintf(stderr, "__TRACE__: CBuffer::~CBuffer(): free...(%p)\n", m__buf);
#endif	// __TRACE__
		free(m__buf);

		m__buf = 0;
		m__buf_size = 0;
		m__data_size = 0;
	}
}


// 기존에 할당된 메모리를 byAmount 만큼 늘인다.
char*
CBuffer::incBuf(
	size_t	byAmount
)
{
	char*	newBuf = (char*) realloc(m__buf, m__buf_size + byAmount);
	if (!newBuf) {
		// 메모리 할당에 문제가 있다.
#ifdef __TRACE__
		(void) fprintf(stderr, "__TRACE__: CBuffer::incBuf(): realloc(): Failure\n");
#endif	// __TRACE__

		return NULL;
	}

	m__buf = newBuf;
	m__buf_size += byAmount;
	return newBuf;
}

// 기존에 할당된 메모리를 최소단위(BUF_SIZE_UNIT바이트)만큼 늘인다.
char*
CBuffer::incBuf(
	void
)
{
	return this->incBuf(BUF_SIZE_UNIT);
}

// 버퍼의 크기를 최소 byAmount 만큼 확보한다.
char*
CBuffer::setBufSizeAtLeast(
	size_t	byAmount
)
{
	size_t	bufSizeForNewData = m__buf_size - m__data_size;
	if (bufSizeForNewData >= byAmount) {
		// 이미 충분한 메모리가 있다.
		return m__buf;
	}

	return this->incBuf(byAmount - bufSizeForNewData);
}
char*
CBuffer::setDataSize(
	size_t	data_size
)
{
	size_t	x_data_size = m__data_size;	// 임시로 저장
						// CBuffer::setBufSizeAtLeast() 가 실패했을 경우를 위해서...

	this->m__data_size = 0;
	if (this->setBufSizeAtLeast(data_size) == NULL) {
#ifdef __TRACE__
		(void) fprintf(stderr, "__TRACE__: CBuffer::setDataSize: CBuffer::setBufSizeAtLeast(): Failure\n");
#endif	// __TRACE__
		m__data_size = x_data_size;	// 적어도 m__data_size 만큼은 원상복귀한다.

		return NULL;
	}
	m__data_size = data_size;
	return m__buf;
}
char*
CBuffer::incDataSize(
	size_t	amount
)
{
	if (this->setBufSizeAtLeast(amount) == NULL) {
#ifdef __TRACE__
		(void) fprintf(stderr, "__TRACE__: CBuffer::incDataSize: CBuffer::setBufSizeAtLeast(): Failure\n");
#endif	// __TRACE__

		return NULL;
	}

	m__data_size += amount;
	return m__buf;
}

int
CBuffer::addChar(
	char	c
)
{
	if (this->m__data_size < this->m__buf_size) {
		// 적어도 하나의 문자를 넣은 공간은 있다.
		*(this->m__buf + this->m__data_size) = c;
		if (c)
			++this->m__data_size;
		return 0;
	}

	// 더 이상 버퍼의 여유가 있다.
	if (this->incBuf() == NULL) {
		return -1;
	}
	*(this->m__buf + this->m__data_size) = c;
	if (c)
		++this->m__data_size;
	return 0;
}
#undef BUF_SIZE_UNIT
