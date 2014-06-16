#include <Python.h>
#include <stdio.h>
#include <marshal.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>	// sleep(3)

#include "CFile.h"
#include "CPacket.h"
#include "CJobDownPacket.h"
#include "CWorkerModule.h"
#include "CWorker.h"
#include "eprintf.h"
#include <sys/socket.h>
#include "CSocket.h"
#include "CSocketOUT.h"

int
main(
	int		argc,
	const char*	argv[]
)
{
#if 0
{
	CWorker*	worker[1000];
	for (int i = 0; i < 1000; ++i) {
		printf("i = %d\n", i);
		worker[i] = new CWorker();
	}
}
#endif
	CWorker	worker;

#if 1	// Using socket...
	// Agent 의 Manager 쓰레드는 Control Socket 만을 이용한다.
	int		ctrl_sockfd = worker.getCtrlSockfd();
	CSocketOUT&	ctrl_sockOUT = *(worker.getCtrlSocketOUT());
	CBuffer		ctrl_rPacket;
	int		ctrl_isWritable = 0;
	CBuffer		ctrl_wPacket;
#else
	CFile	fileWriteToMS;
	if (fileWriteToMS.openFile("SunOS_r", "wb", 0) < 0) {
		(void) fprintf(stderr, "CFile::openFile(|%s|, |wb|): Failure\n", "SunOS_r");
		return 1;
	}
	int	filenoWriteToMS = fileno(fileWriteToMS.getFILE());
	if (filenoWriteToMS < 0) {
		perror("fileno(|SunOS_r|): Failure\n");
		return 1;
	}
	CBuffer&	wPacket = *(fileWriteToMS.getBuffer());
	CFile	fileReadFromMS;
	if (fileReadFromMS.openFile("SunOS_w", "rb", 0) < 0) {
		(void) fprintf(stderr, "CFile::openFile(|%s|, |rb|): Failure\n", "SunOS_w");
		return 1;
	}
	int	filenoReadFromMS = fileno(fileReadFromMS.getFILE());
	if (filenoReadFromMS < 0) {
		perror("fileno(|SunOS_w|): Failure\n");
		return 1;
	}
	const CBuffer&	rPacket = *(fileReadFromMS.getBuffer());
#endif

	CFile	file(stdin, 0, 0);
	const CBuffer&	buffer = *(file.getBuffer());
	if (!buffer.getBufSize()) {
		// Contructor에서 메모리를 할당할 때 문제가 있었다.
		// 이 사실을 알리고 쓰레드를 종료한다.
		(void) fprintf(stderr, "CBuffer::CBuffer(): Failure\n");

		return 1;
	}

////////////////////////////////////////////////////////////////////////////////
	if (argc > 1) {	// Agent 가 다시 시작하는 경우...
		// argv[1]:= Old agent's name
		// argv[2]:= Order ID
		// argv[3]:= Resume
		// argv[4]:= Set...
		unsigned	orderID = (unsigned) -1;
		CBuffer&	msgBuffer = *(worker.getMsgBuffer()); msgBuffer.setDataSize(0);
		int	iReturnVal = 0;
		for (int i = 0; i < argc; ++i) {
			worker.evsprintf("\nargv[%d] = |%s| (%s)", i, argv[i],
				i == 0 ? "Current Agent Name" :
				i == 1 ? "Old Agent Name" :
				i == 2 ? "Order ID" :
				i == 3 ? "Resume?" : "Set..."
			);
		}
		worker.evsprintf("\n");
do {
		if (argc < 5) {
			if (worker.evsprintf("Usage: |%s| old_agent_name order_id resume\n\n", argv[0]) < 0) {
				eprintf(E_FATAL, EFMT("evsprintf..."));
			}

			iReturnVal = -1;
			break;
		}

		// 전의 Agent 를 삭제한다.
		if (unlink(argv[1]) < 0) {
			if (worker.evsprintf("unlink(|%s|): Failure: %m", argv[1]) < 0) {
				eprintf(E_FATAL, EFMT("evsprintf..."));
			}
		}

		orderID = (unsigned) atoi(argv[2]);
		if (atoi(argv[3])) {
			if (worker.pyReload() < 0) {
				eprintf(E_WARN, EFMT("CWorker::pyReload(): Failure"));
				iReturnVal = -1;
				break;
			}
		}
} while (0);

		// 재시작했음을 알린다.
	PyGILState_STATE	gstate = PyGILState_Ensure();
	PyObject*	result_Str = NULL;
do {
		PyObject*	msg = Py_None;
		if (msgBuffer.getDataSize()) {
			msg = PyString_FromString(msgBuffer.getBuf()); assert(msg);
		}
		PyObject*	setInfoByDict = Py_None;
		if (argc > 4) {
			setInfoByDict = PyString_FromString(argv[4]); assert(setInfoByDict);
		}
		PyObject*	result = Py_BuildValue(
			"{s: O, s: O}"
			,
			"xxx__SET", setInfoByDict,
			"Result", msg
		); assert(result);
		if (msg != Py_None) {
			Py_DECREF(msg);
			msg = NULL;
		}
		if (setInfoByDict != Py_None) {
			Py_DECREF(setInfoByDict);
			setInfoByDict = NULL;
		}
		result_Str = PyObject_Str(result); assert(result_Str);
		Py_DECREF(result);
		const char*	result_Str_cstr = PyString_AsString(result_Str); assert(result_Str_cstr);
		int	isPyDictTrue = 1;
		CPacket::RESP_TYPE_T	respType = iReturnVal == -1 ? CPacket::RESP_TYPE__FAIL : CPacket::RESP_TYPE__OK;
		if (CPacket::packResponse(
			ctrl_wPacket,

			isPyDictTrue,
			respType,
			orderID,
#if 1
			result_Str_cstr
#else
			msgBuffer.getDataSize() == 0 ? NULL : msgBuffer.getBuf()
#endif
		) < 0) {
			eprintf(E_ERR, EFMT("CPacket::packResponse(%s): Failure"), iReturnVal == -1 ? "RESP_TYPE__FAIL" : "RESP_TYPE__OK");
			iReturnVal = -1;
			break;
		}
//		(void) printf("hello, world: # of byte(s) = %d\n", ctrl_wPacket.getDataSize());
#if 1	// Using socket...
	ctrl_isWritable = 1;
	assert(ctrl_sockOUT.enqueue(ctrl_wPacket) == 0);
#else
		int	flushTrue = 1;
		assert(fileWriteToMS.writeAll(flushTrue) == 0);
#endif
		iReturnVal = 0;
} while (0);
	Py_XDECREF(result_Str); result_Str = NULL;
	PyGILState_Release(gstate);
	}
////////////////////////////////////////////////////////////////////////////////

	int	n = STDIN_FILENO;
#if 1	// Using socket...
	fd_set	rfds;
	fd_set	wfds;
	if (ctrl_sockfd > n)
		n = ctrl_sockfd;
#else
	if (filenoReadFromMS > n)
		n = filenoReadFromMS;
#endif
	++n;
	int	timeout = 0;
	CBuffer	lastCommandBuffer;
	char	*lastCommand = NULL;
	while (1) {
		FD_ZERO(&rfds);
		FD_SET(STDIN_FILENO, &rfds);
#if 1	// Using socket...
		FD_SET(ctrl_sockfd, &rfds);
		fd_set*	pwfds = NULL;
		if (ctrl_isWritable) {
			pwfds = &wfds;
			FD_ZERO(pwfds);
			FD_SET(ctrl_sockfd, pwfds);
		}
#else
		FD_SET(filenoReadFromMS, &rfds);
#endif
		struct timeval	tv;
		tv.tv_sec = 1;
		tv.tv_usec = 0;

		if (!timeout)
			(void) fprintf(stderr, "CMD> ");
		int	retval = select(n, &rfds, pwfds, NULL, &tv);

		if (!retval) {	// TIMEDOUT
			timeout = 1;
			continue;
		}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
#if 1	// Using socket...
	if (ctrl_isWritable && FD_ISSET(ctrl_sockfd, pwfds)) {
		int	isEmpty = ctrl_sockOUT.flush();
		if (isEmpty < 0) {
                                eprintf(E_ERR, EFMT("CQueue::flush(): Failure"));
                                continue;
		}
		// isEmpty = 1:= Queue 가 비워졌다.
		// isEmpty = 0:= 아직 Queue 에 무언가 남아있다.
		ctrl_isWritable = !isEmpty;
	}
#endif
if (
#if 1
FD_ISSET(ctrl_sockfd, &rfds)
#else
FD_ISSET(filenoReadFromMS, &rfds)
#endif
) {
#if 1
////////////////////////////////////////////////////////////////////////
// 하나의 완전한 패킷을 얻으려고 시도한다.
	CPacket::PACKET_TYPE_T	packetType;
	int	isComplete;
	ssize_t	nread = CSocket::read(ctrl_sockfd, ctrl_rPacket, packetType, isComplete);
	if (nread < 0) {
		eprintf(E_ERR, EFMT("CSocket::read(): Failure"));
		continue;
	}

#ifdef __TRACE__
	(void) printf("# of byte(s) = %d, isComplete = %s\n", nread, isComplete ? "True" : "False");
#endif	// __TRACE__
	if (nread == 0) {
		(void) printf("M/S is dead: The End\n");
		if (worker.pyKillAll() < 0) {
			(void) fprintf(stderr, "CWorker::pyKillAll(): Failure\n");
		}

		break;
	}
	if (!isComplete)
		continue;

	// 하나의 완전한 패킷이 있다.
	// 사용 후에는 패킷의 크기를 0으로 해서 다음 패킷을 받아들이도록 해야한다!!!
#ifdef __TRACE__
	(void) printf("Complete packet? Ok\n");
#endif	// __TRACE__
////////////////////////////////////////////////////////////////////////
#else
	CPacket::PACKET_TYPE_T	packetType;
	int	nread = fileReadFromMS.readPacket(packetType);
	if (nread == 0) {
		(void) printf("M/S is dead: The End\n");
		if (worker.pyKillAll() < 0) {
			(void) fprintf(stderr, "CWorker::pyKillAll(): Failure\n");
		}

		break;
	}
	if (nread < 0) {
		(void) fprintf(stderr, "CFile::readPacket(|%s|): Failure\n", "SunOS_w");
		return 1;
	}
#endif
	int	iReturnVal = 0;
do {
	(void) printf("Received Packet: Type:= |%s|, Packet Size = %d byte(s)\n",
		packetType == CPacket::PACKET_TYPE__ERR ? "PACKET_TYPE__ERR" :
		packetType == CPacket::PACKET_TYPE__PERF ? "PACKET_TYPE__PERF" :
		packetType == CPacket::PACKET_TYPE__ORD ? "PACKET_TYPE__ORD" : "PACKET_TYPE__RESP"
		,
		ctrl_rPacket.getDataSize()
	);

	if (packetType == CPacket::PACKET_TYPE__RESP) {
		int	isPyDict;
		CPacket::RESP_TYPE_T	unpackedRespType;
		unsigned int		unpackedOrderID;
		CBuffer			returnBuffer;
		if (CPacket::unpackResponse(
			ctrl_rPacket,

			isPyDict,
			unpackedRespType,
			unpackedOrderID,
			returnBuffer
		) < 0) {
			eprintf(E_ERR, EFMT("CPacket::unpackResponse(): Failure"));

			iReturnVal = -1;
			break;
		}

		(void) printf(
			"////////////////////////////////////////////////////////////////////////\n"
			"Is PyDict?: %s\n"
			"Response Type: %s\n"
			"Order ID: %d\n"
			"Return: |%s|(%d)\n"
			"////////////////////////////////////////////////////////////////////////\n"
			,
			returnBuffer.getDataSize() ? (isPyDict ? "True" : "False") : "N/A",
			unpackedRespType == CPacket::RESP_TYPE__ACK ? "ACK" :
			unpackedRespType == CPacket::RESP_TYPE__FAIL ? "FAIL" :
			unpackedRespType == CPacket::RESP_TYPE__OK ? "OK" : "UNKNOWN",
			unpackedOrderID,
			returnBuffer.getDataSize() ? returnBuffer.getBuf() : "N/A",
			returnBuffer.getDataSize() ? returnBuffer.getDataSize() : (unsigned) -1
		);

#if 0
		int	dataSockfd = CWorker::setSockfd();
		if (dataSockfd < 0) {
			eprintf(E_FATAL, EFMT("CWorker::setSockfd() for data: Failure"));

			iReturnVal = -1;
			break;
		}
		worker.setDataSockfd(dataSockfd);
#endif

		iReturnVal = 0;
		break;
	}

	if (packetType != CPacket::PACKET_TYPE__ORD) {
		eprintf(E_WARN, EFMT("CPacket::PACKET_TYPE__ORD Only as of 5/16/2008..."));

		iReturnVal = 0;
		break;
	}

	CWorker::UPDATE_AGENT_T	ua = {
		"agent_XXXXXX",		// 항상 마지막은 "XXXXXX" 로 끝나야 한다.(man mkstemp 참고)
		(unsigned) -1,
		(unsigned) 0		// Agent 를 Update 하라는 패킷이라면
					// UPDATE_AGENT__UPDATE 비트가 설정된다.
	};
	if (worker.pyUnpack(ctrl_rPacket, ctrl_wPacket, ua) < 0) {
		eprintf(E_ERR, EFMT("CWorker::pyUnpack: Failure"));

		iReturnVal = -1;
		break;
	}

	// 패킷을 보낸다.
	(void) printf("Send Packet: # of byte(s) to be sent = %d\n", ctrl_wPacket.getDataSize());
#if 1
	assert(ctrl_sockOUT.enqueue(ctrl_wPacket) == 0);
	ctrl_isWritable = 1;
#else
	int	flushTrue = 1;
	assert(fileWriteToMS.writeAll(flushTrue) == 0);
#endif

	if (!(ua.flag & UPDATE_AGENT__UPDATE)) {
		iReturnVal = 0;
		break;
	}

	// 새로운 Agent 로 프로그램을 재시작한다.
////////////////////////////////////////////////////////////////////////////////
{
// <Restart>...
#if 0
	int i = worker.restart(argv[0], ua) < 0;
	// 정상적이라면 여기까지 도달하지 않아야한다.
	if (i < 0) {
		eprintf(E_FATAL, EFMT("CWorker::restart(|%s|->|%s| with (Order ID = %u, Resume = %s): Failure")
			,
			argv[0], ua.agentCopy, ua.orderID, ua.flag & CWorker::UPDATE_AGENT__RESUME ? "True" : "False"
		);
	}
#else
	int	iReturnVal = 0;			// 어떤 문제가 발생하면 음수(-1)로 설정한다.
	CBuffer&	msgBuffer = *(worker.getMsgBuffer()); msgBuffer.setDataSize(0);
do {
	if (ua.flag & UPDATE_AGENT__RESUME) {
		if (worker.pySaveAll() < 0) {
			iReturnVal = -1;
			if (worker.evsprintf("CWorker::pySaveAll(): Failure") < 0) {
				eprintf(E_FATAL, EFMT("evsprintf..."));
			}

			break;
		}
	}

	// 동작 중인 모든 쓰레드를 중지시킨다.
	if (worker.pyKillAll() < 0) {
		iReturnVal = -1;
		if (worker.evsprintf("CWorker::pyKillAll(): Failure") < 0) {
			eprintf(E_FATAL, EFMT("evsprintf..."));
		}

		break;
	}

	// M/S 와의 연결을 끊는다.
#if 1
#if 1
	if (shutdown(ctrl_sockfd, SHUT_RDWR) < 0) {
		eprintf(E_ERR, EFMT("shutdown(SHUT_RDWR): Failure: %m"));
		iReturnVal = -1;
		break;
	}
	if (close(ctrl_sockfd) < 0) {
		eprintf(E_ERR, EFMT("close(this->m__connectSockfd): Failure"));
		iReturnVal = -1;
		break;
	}
	sleep(5);
#else
	if (close(filenoWriteToMS) < 0) {
		iReturnVal = -1;
		if (worker.evsprintf("close(filenoWriteToMS): Failure: %m") < 0) {
			eprintf(E_FATAL, EFMT("evsprintf..."));
		}

		break;
	}
	if (close(filenoReadFromMS) < 0) {
		iReturnVal = -1;
		if (worker.evsprintf("close(filenoReadFromMS): Failure: %m") < 0) {
			eprintf(E_FATAL, EFMT("evsprintf..."));
		}

		break;
	}
	// 너무 빨리 FIFO 를 닫으면, M/S 가 제때 반응하지 않는다.
	// 시간을 주어 차라리 M/S 를 다시 시작하자.
	sleep(5);
#endif
#endif

	char	strOrderID[8];
	if (snprintf(strOrderID, 8, "%u", ua.orderID) < 0) {
		iReturnVal = -1;
		if (worker.evsprintf("snprintf() for Order ID: Failure: %m") < 0) {
			eprintf(E_FATAL, EFMT("evsprintf..."));
		}

		break;
	}
	char	strResume[2];
	strResume[0] = ua.flag & UPDATE_AGENT__RESUME ? '1' : '0';
	strResume[1] = '\0';

	CBuffer	setInfoByDict;
	if (worker.pyGetSet(setInfoByDict) < 0) {
		iReturnVal = -1;
		if (worker.evsprintf("CWorker::pyGetSet(): Failure") < 0) {
			eprintf(E_FATAL, EFMT("evsprintf..."));
		}

		break;
	}
//	(void) printf("|%s|(%d)\n", setInfoByDict.getBuf(), setInfoByDict.getDataSize());
#if 1
	(void) printf("Updating: Wait...\n");
	(void) execl(
		ua.agentCopy,
		ua.agentCopy, argv[0], strOrderID, strResume,
		setInfoByDict.getDataSize() ? setInfoByDict.getBuf() : "", NULL
	);
#else
{
	FILE*	f = NULL;
do {
	f = fopen("./runMe.sh", "w");
	if (!f) {
		if (worker.evsprintf("fopen(|%s|, |w|): Failure: %m", "./runMe.sh") < 0) {
			eprintf(E_FATAL, EFMT("evsprintf..."));
		}
		iReturnVal = -1;
		break;
	}
	(void) fprintf(f, "%s \"%s\" \"%s\" \"%s\" \"%s\"\n", ua.agentCopy, argv[0], strOrderID, strResume, setInfoByDict.getDataSize() ? setInfoByDict.getBuf() : "");
	if (fclose(f) < 0) {
		if (worker.evsprintf("fclose(|%s|): Failure: %m", "./runMe.sh") < 0) {
			eprintf(E_FATAL, EFMT("evsprintf..."));
		}
		iReturnVal = -1;
		break;
	}
	f = NULL;
} while (0);
	if (f) {
		(void) fclose(f);
		f = NULL;
	}
	if (iReturnVal == 0) {
		(void) printf("Updating: Wait...\n");
		return 1;
	}
}
#endif

	// execl 에서 문제가 있었다.
	if (worker.evsprintf(
		"execl(|%s|, |%s|, |%s|, |%s|, |%s|, NULL): Failure: %m"
		,
		ua.agentCopy, ua.agentCopy, argv[0], strOrderID, strResume
	) < 0) {
		eprintf(E_FATAL, EFMT("evsprintf..."));
	}
} while (0);
	// 정상적이라면 여기까지 도달하지 않아야한다.
#if 0
	if (iReturnVal < 0)
#endif
	(void) printf("Err:= |%s|\n", msgBuffer.getBuf());
#endif
// </Restart>....
}
////////////////////////////////////////////////////////////////////////////////
} while (0);
#if 1
	ctrl_rPacket.setDataSize(0);
#endif

	timeout = 1;
	continue;
}
////////////////////////////////////////////////////////////////////////

		if (!FD_ISSET(STDIN_FILENO, &rfds))
			continue;

////////////////////////////////////////////////////////////////////////
{
		file.readline();
		if (buffer.getDataSize() == 0) {
			// The End
			if (worker.pyKillAll() < 0) {
				(void) fprintf(stderr, "CWorker::pyKillAll(): Failure\n");
			}
#if 0
			continue;
#else
			(void) printf("Bye~\n");
#endif

			break;
		}
//			(void) fprintf(stderr, "CMD> |%s|\n", buffer.getBuf());
		int	isValidCommand = 1;
		const char*	command = buffer.getBuf();
RESTART:
		if (strstr(command, "HELP")) {
			worker.help();
		} else if (strstr(command, "SAVE__ALL")) {
			worker.pySaveAll();
		} else if (strstr(command, "LIST__ALL")) {
			worker.pyListAllObjects();
		} else if (strstr(command, "LIST__WORKERS")) {
			worker.pyListWorkers();
		} else {
			isValidCommand = 0;
			if (strcspn(command, " \t\n") == 0 && lastCommand) {
				command = lastCommand;
				goto RESTART;
			} else {
				(void) printf("Available commands are...\n");
				(void) printf("========================================\n");
				worker.help();
				(void) printf("========================================\n");
			}
		}
		if (isValidCommand) {
			(void*) lastCommandBuffer.setDataSize(0);
			size_t	len = strlen(command) + 1;
			assert(lastCommandBuffer.setBufSizeAtLeast(len));
			lastCommand = lastCommandBuffer.getBuf();
			(void*) memcpy(lastCommand, command, len);
		}

		timeout = 0;
}
////////////////////////////////////////////////////////////////////////
	}

	return 0;
}
