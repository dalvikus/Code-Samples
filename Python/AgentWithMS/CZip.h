#ifndef __ZIP_H__
#define __ZIP_H__
////////////////////////////////////////////////////////////////////////////////


/*
 * zlib 의 예제인 zpipe.c 를 참고해서,
 * 주어진 바이트스트림을 압축하고, 압축된 바이트스트림의 압축을 푸는 함수를 구현했다.
 *	http://www.zlib.net/zlib_how.html
 */
#include <zlib.h>
#include "CBuffer.h"

class CZip {
private:
	int		m__autoFree;	// 이 값이 0이 아닌 경우, m__pBuffer 를 할당한다.
	CBuffer*	m__pBuffer;
public:
	CZip(CBuffer* pBuffer);	// 외부에서 pBuffer가 주어진 경우에는 m__autoFree = 0을 설정한다.
	~CZip();
public:
	const CBuffer&	getBuffer(void) const	{return *m__pBuffer;}
	CBuffer&	getBuffer(void) 	{return *m__pBuffer;}
public:
	// 어느 경우든 성공했을 때 0을 그렇지 않았을 때 음수를 리턴한다.
	// buf 가 가리키는 len 바이트를 압축해서 m__pBuffer 가 가리키는 곳에 차례로 넣는다.
	// m__pBuffer 가 가리키는 곳의 끝에 압축한 결과는 넣는다.
	// m__pBuffer 를 관리해야 하는 것은 이 함수를 호출한 함수의 몫이다.
	int	compress(const Bytef* buf, size_t len);
	// buf 가 가리키는 len 바이트의 데이터의 압축을 풀어 m__pBuffer 가 가리키는 곳에 차례로 넣는다.
	// m__pBuffer 가 가리키는 곳의 끝에 압축을 푼 결과는 넣는다.
	// m__pBuffer 를 관리해야 하는 것은 이 함수를 호출한 함수의 몫이다.
	int	uncompress(const Bytef* buf, size_t len);
};


////////////////////////////////////////////////////////////////////////////////
#endif	// !__ZIP_H__
