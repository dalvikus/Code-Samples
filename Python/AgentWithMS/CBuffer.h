#ifndef __BUFFER_H__
#define __BUFFER_H__
////////////////////////////////////////////////////////////////////////////////


#include <stdlib.h>	// malloc, relloc, free

class CBuffer {
private:
	char*	m__buf;
	size_t	m__buf_size;	// 버퍼의 총 크기
	size_t	m__data_size;	// 데이터의 총 크기
public:
	char*	getBuf(void)			{return m__buf;}
	const char*	getBuf(void) const	{return m__buf;}
	size_t	getBufSize(void) const		{return m__buf_size;}
	size_t	getDataSize(void) const		{return m__data_size;}

public:
	char*	getBufForNewData(void)		{return m__buf + m__data_size;}
	size_t	getBufSizeForNewData(void)	{return m__buf_size - m__data_size - 1;}
							// 제일 마지막 줄이 '\n'로 끝나지 않을 경우,
							// 강제로 '\n'을 추가할 공간을 마련하기 위해서
							// 가능한 공간에서 1을 뺐다.

public:
	CBuffer(void);
	~CBuffer();

public:
	// 기존에 할당된 메모리를 최소단위(BUF_SIZE_UNIT바이트)만큼 늘인다.
	char*	incBuf(void);
	char*	incBuf(size_t byAmount);
	char*	setBufSizeAtLeast(size_t byAmount);
	char*	setDataSize(size_t data_size);
	char*	incDataSize(size_t amount);

public:
	// 제일 마지막에 특정 문자를 추가한다.
	// NUL 이 아니라면 버퍼안에 있는 문자의 갯수도 1 증가시킨다.
	int	addChar(char c);
};


////////////////////////////////////////////////////////////////////////////////
#endif	// !__BUFFER_H__
