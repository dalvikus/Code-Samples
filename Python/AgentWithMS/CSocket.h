#ifndef __SOCKET_H__
#define __SOCKET_H__
////////////////////////////////////////////////////////////////////////////////


#include "CPacket.h"
#include "CBuffer.h"

class CSocket {
public:
	// 현재까지 읽어들인 패킷(rPacket)의 일부를 바탕으로,
	// 기껐해야 한 패킷에 해당하는 내용을 읽는다.
	// 읽어들인 바이트 수를 리턴한다.
	// isComplete 는 다음 값을 가진다; 리턴값이 양수일 때만 의미가 있다.
	//	1:= 완전한 패킷
	//	0:= 불완전한 패킷
	// *: 완전한 패킷의 경우에만 packetType 이 의미가 있다.
	// **: 리턴값이 0이면 Socket 이 닫혔음을 뜻한다.
	static ssize_t	read(int sockfd, CBuffer& rPacket, CPacket::PACKET_TYPE_T& packetType, int& isComplete);

public:
	// 하나의 완전한 패킷을 읽는다.
	static const long	RECV__TIMED_OUT;	// 이 마이크로초 안에 읽어야 한다.
	static const unsigned	RECV__MAX_TRIAL;	// 총 이 횟수만큼 시도한다.
							// 두 수의 곱 즉 총 시간이 너무 크지않도록 한다.
	static ssize_t	readPacket(int sockfd, CBuffer& rPacket);

	// 완전한 하나의 패킷을 쓴다.
	static const size_t	WRITE_PACKET__QUEUE_SIZE;	// writePacket 에서 사용하는 CSocketO 에 있는 쓰기용 큐의 크기
							// 쓰려는 패킷을 담을 수 있어야 한다.
	static const long	SEND__TIMED_OUT;	// 이 마이크로초 안에 쓸 수 있어야 한다.
	static const unsigned	SEND__MAX_TRIAL;	// 총 이 횟수만큼 시도한다.
							// 두 수의 곱 즉 총 시간이 너무 크지않도록 한다.
	static int	writePacket(int sockfd, const CBuffer& wPacket);

public:
	static int	sendSockID(int sockfd, CPacket::RESP_TYPE_T sockType, unsigned port);
	static int	recvSockID(int sockfd, CPacket::RESP_TYPE_T& sockType, unsigned& port);
};


////////////////////////////////////////////////////////////////////////////////
#endif	// !__SOCKET_H__
