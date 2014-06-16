#include "CSocket.h"
#include "CSocketOUT.h"
#include "eprintf.h"
#include <sys/socket.h>
#include <assert.h>

const size_t	CSocket::WRITE_PACKET__QUEUE_SIZE	= 1024;
const unsigned	CSocket::SEND__MAX_TRIAL	= 6;
const long	CSocket::SEND__TIMED_OUT	= 100;	// microsecond(s)
const unsigned	CSocket::RECV__MAX_TRIAL	= 6;
const long	CSocket::RECV__TIMED_OUT	= 100;	// microsecond(s)

int
CSocket::read(
	int	sockfd,
	CBuffer&	rPacket,
	CPacket::PACKET_TYPE_T&	packetType,
	int&	isComplete
)
{
	size_t	nReadUpToNow = rPacket.getDataSize();	// 현재까지 읽어들인 데이터
	size_t	len;	// 읽어들일 데이터의 크기
			//	1) 헤더바이트를 포함해서 최소한 5바이트를 읽었다면
			//	패킷의 나머지 바이트 수
			//	2) 아직 5바이트를 읽지 못했다면
			//	5바이트가 되기위한 바이트 수
	// 5바이트를 기준으로...
	//	4바이트:= 크기바이트, 1바이트:= 헤더바이터
	if (nReadUpToNow > 4) {
		// 현재까지 패킷의 일부로 읽어들인 바이트 수 = nReadUpToNow - 4
		// 완전한 패킷이 되기위해서는 몇 바이트를 더 읽어야 하는 지 안다.
		size_t	packetSize;
		packetType = CPacket::whichType(rPacket, &packetSize);
		len = packetSize - (nReadUpToNow - 4);
	} else {
		// 5바이트가 되도록 한다.
		len = 5 - nReadUpToNow;
	}
	assert(len);

	// 적어도 len 바이트를 읽어들일 만큼의 공간을 확보한다.
	if (!rPacket.setBufSizeAtLeast(len)) {
		eprintf(E_ERR, EFMT("CBuffer::setBufSizeAtLeast(|%d|): Failure"), len);

		return -1;
	}
	ssize_t	nRead = recv(sockfd, rPacket.getBufForNewData(), len, 0);
	if (nRead < 0) {
		eprintf(E_FATAL, EFMT("recv(): Failure: %m"));

		return -1;
	}
	rPacket.incDataSize(nRead);

	if ((size_t) nRead < len) {
		// 원하는 만큼은 아니지만 읽을만큼 읽었다.
		// 하지만 아직 완전한 패킷은 아니다.
		isComplete = 0;
		return nRead;
	}
	// 원하는 만큼 읽었다.
	// 더 읽을 수도 있다; 하지만 다음에...
	isComplete = nReadUpToNow > 4 ? 1 : 0;
	return nRead;
}

ssize_t
CSocket::readPacket(
	int	sockfd,
	CBuffer&	rPacket
)
{
	(void*) rPacket.setDataSize(0);

	ssize_t	n_total = 0;
	int	n = sockfd + 1;
	fd_set	rfds;
	struct timeval	tv;
	for (unsigned i = 0; i < RECV__MAX_TRIAL; ++i) {
		FD_ZERO(&rfds);
		FD_SET(sockfd, &rfds);
		tv.tv_sec = 0;
		tv.tv_usec = RECV__TIMED_OUT;

		int	retval = select(n, &rfds, NULL, NULL, &tv);
		if (retval < 0) {
			eprintf(E_ERR, EFMT("select(): Failure: %m"));
			return -1;
		}
		if (!retval) {
//			eprintf(E_ERR, EFMT("select(): TIMEDOUT(= %d)"), RECV__TIMED_OUT);
			continue;
		}

		// FD_ISSET(sockfd, &rfds)
		////////////////////////////////////////////////////////////////
		CPacket::PACKET_TYPE_T	packetType;
		int	isComplete;
		ssize_t	nread = CSocket::read(sockfd, rPacket, packetType, isComplete);
		if (nread < 0) {
			eprintf(E_ERR, EFMT("CSocket::read(): Failure"));
			return -1;
		}
#ifdef __TRACE__
		(void) printf("# of byte(s) = %d, isComplete = %s\n", nread, isComplete ? "True" : "False");
#endif	// __TRACE__

		if (nread == 0) {
			eprintf(E_ERR, EFMT("Socket closed"));

			return 0;
		}

		n_total += nread;
		if (!isComplete)
			continue;

		// 하나의 완전한 패킷이 있다.
#ifdef __TRACE__
		(void) printf("Complete packet? Ok\n");
#endif	// __TRACE__
		return n_total;
	}
	// RECV__MAX_TRIAL 번 시도했다.

	return -1;
}

int
CSocket::writePacket(
	int	sockfd,
	const CBuffer&	wPacket
)
{
	CSocketOUT	sockOUT(sockfd, WRITE_PACKET__QUEUE_SIZE);
	assert(sockOUT.enqueue(wPacket) == 0);

	// 정해진 시간 안에 보낼 수 없다면 문제가 있는 것으로 처리한다.
	int	n = sockfd + 1;
	fd_set	wfds;
	struct timeval	tv;
	for (unsigned i = 0; i < SEND__MAX_TRIAL; ++i) {
		FD_ZERO(&wfds);
		FD_SET(sockfd, &wfds);
		tv.tv_sec = 0;
		tv.tv_usec = SEND__TIMED_OUT;

		int	retval = select(n, NULL, &wfds, NULL, &tv);
		if (retval < 0) {
			eprintf(E_ERR, EFMT("select(): Failure: %m"));
			return -1;
		}

		if (!retval) {
//			eprintf(E_ERR, EFMT("select(): TIMEDOUT(= %d)"), SEND__TIMED_OUT);
			continue;
		}

		// FD_ISSET(sockfd, &wfds)
		int	isEmpty = sockOUT.flush();
		if (isEmpty < 0) {
			eprintf(E_ERR, EFMT("CQueue::flush(): Failure"));
			return -1;
		}
		// isEmpty = 1:= Queue 가 비워졌다.
		// isEmpty = 0:= 아직 Queue 에 무언가 남아있다.
		if (isEmpty)
			return 0;
	}
	// SEND__MAX_TRIAL 번을 시도했다.

	return -1;
}

int
CSocket::sendSockID(
	int	sockfd,
	CPacket::RESP_TYPE_T	sockType,
	unsigned	port
)
{
	assert(sockfd >= 0);
	if (sockType != CPacket::RESP_TYPE__CTRL_SOCKET && sockType != CPacket::RESP_TYPE__DATA_SOCKET) {
		eprintf(E_ERR, EFMT("sockType = %d: CTRL_SOCKET or DATA_SOCKET Only..."));
		return -1;
	}

	CBuffer	wPacket;
	int		isPyDictFalse = 0;
	if (CPacket::packResponse(
		wPacket,

		isPyDictFalse,
		sockType,
		port,
		NULL
	) < 0) {
		eprintf(E_ERR, EFMT("CPacket::packResponse(|%s|): Failure"), sockType == CPacket::RESP_TYPE__CTRL_SOCKET ? "Control Socket" : "Data Socket");

		return -1;
	}

	return CSocket::writePacket(sockfd, wPacket);
}

int
CSocket::recvSockID(
	int	sockfd,
	CPacket::RESP_TYPE_T&	sockType,
	unsigned&	port
)
{
	CBuffer	rPacket;
	ssize_t	nread = CSocket::readPacket(sockfd, rPacket);
	if (nread < 0) {
		eprintf(E_ERR, EFMT("CSocket::readPacket(): Failure"));

		return -1;
	}

	if (nread == 0) {
		eprintf(E_ERR, EFMT("CSocket::readPacket(): Socket closed"));

		return -1;
	}

	int	isPyDict;
	CBuffer	returnBuffer;
	if (CPacket::unpackResponse(
		rPacket,

		isPyDict,
		sockType,
		port,
		returnBuffer
	) < 0) {
		eprintf(E_ERR, EFMT("CPacket::unpackResponse(): Failure"));

		return -1;
	}
	return 0;
}
