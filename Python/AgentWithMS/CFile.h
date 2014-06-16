#ifndef __FILE_H__
#define __FILE_H__
////////////////////////////////////////////////////////////////////////////////


#if 0	// Socket 을 사용하여 패킷을 주고 받으므로 더 이상 사용하지 않는다.
#include "CPacket.h"
#endif
#include "CBuffer.h"
#include <stdio.h>	// FILE, fopen, fclose, popen, pclose, fgets, feof, ferror

class CFile {
private:
	int	m__autoClose;
	FILE*	m__f;
	int	m__pipe;	// -1: None, 0: fopen, 1: popen
public:
	const FILE*	getFILE(void) const	{return m__f;}
	FILE*		getFILE(void)		{return m__f;}

// 읽어들인 데이터를 저장할 공간을 관리한다.
private:
	int		m__autoFree;
	CBuffer*	m__buffer;
public:
	const CBuffer*	getBuffer(void) const	{return m__buffer;}
	CBuffer*	getBuffer(void)		{return m__buffer;}

public:
	CFile(const FILE* f = NULL, int pipe = -1, int autoClose = 1, CBuffer* buffer = 0);
	~CFile();

public:
	// 처리해야 할 파일을 설정한다.
	//	pipe = 0: 보통 파일
	//	pipe = 1: 파이프
	// 리턴값:
	//	0: 정상적으로 파일스트림을 설정했다.
	//	-1: 설정할 때 문제가 있다.
	//	-2: 이미 설정된 파일스트림이 있다.
	int	openFile(const char* pathname_or_command, const char* mode, int pipe);
	// 열린 파일스트림을 닫는다.
	// 리턴값:
	//	>= 0: 정상적으로 닫았다. (파이프의 경우에는 실행한 명령어의 리턴값)
	//	-1: 닫을 때 문제가 있었다.
	//	-2: 열린 파일스트림이 없다.
	int	closeFile(void);

	// 파일스트림(FILE*)으로부터 읽는다.
	// 읽어들인 바이트 수를 리턴한다.
	// 리턴값:
	//	> 0: m__buffer.getBuf()가 가리키는 곳에서부터 m__buffer.getDataSize()개의 바이트가 그 결과이다.
	//	0: 파일의 끝이다.
	//	-1: 파일스트림으로부터 읽을 때 문제가 있었다.
	//	-2: 데이터를 저장할 공간을 확보할 때 문제가 있었다.
	//	-3: 아직 파일스트림이 설정되지 않았다
	int	readline(void);	// 주어진 파일스트림(FILE*)로부터 줄 단위로 읽는다.
	int	readlines(int nLineToBeSkipped = 0, int nLineToBeTailed = 0);
				// 주어진 파일스트림(FILE*)로부터 모든 내용을 읽는다.
				// 이 때 처음 몇 줄(nLineToBeSkipped)과 마지막 몇 줄(nLineToBeTailed)을 무시한다.

	// 바이너리 데이터를 읽거나 쓴다.
	// 파일스트림(FILE*)으로부터 모든 데이터를 읽어 버퍼에 저장한다.
	// 읽어들인 바이트의 총 갯수를 리턴한다.
	// 따라서 음수이면 에러가 있었음을 뜻한다.
	int	readAll(void);
	// 버퍼에 있는 모든 바이너리 데이터를 열려있는 파일에 쓴다.
	// 성공적으로 썼으면 0을 그렇지 않으면 음수값을 리턴한다.
	int	writeAll(int iFlush = 0);

#if 0
public:
	// 이 파일스트림이 패킷과 관련된 거라면,
	// 파일스트림으로부터 패킷을 읽는다.
	int	readPacket(CPacket::PACKET_TYPE_T& packetType);
#endif
};


////////////////////////////////////////////////////////////////////////////////
#endif	// !__FILE_H__
