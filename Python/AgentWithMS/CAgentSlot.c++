#include "CAgentSlot.h"
#include "CSocket.h"
#include "CJobDownPacket.h"
#include "eprintf.h"
#include "CMgmtSrv.h"
#include <sys/socket.h>
#include <assert.h>
#include <unistd.h>

const size_t	CAgentSlot::CONTROL__WRITE_QUEUE_SIZE	= 8192000;

CAgentSlot::CAgentSlot(
	const char*	id_str
)
{
	PyGILState_STATE	gstate = PyGILState_Ensure();
	this->m__ID__STR = PyString_FromString(id_str); assert(this->m__ID__STR);
	this->m__commandID__SET = PySet_New(NULL); assert(this->m__commandID__SET);
	this->m__byteStreamID__SET = PySet_New(NULL); assert(this->m__byteStreamID__SET);
	this->m__scriptID__SET = PySet_New(NULL); assert(this->m__scriptID__SET);
	this->m__order__LIST = PyList_New(0); assert(this->m__order__LIST);
	PyGILState_Release(gstate);

	this->m__ctrlSockfd = -1;
	this->m__isCtrlSocketWritable = 0;
	this->m__dataSockfd = -1;
	this->m__ctrlSockOUT = NULL;
}

CAgentSlot::~CAgentSlot()
{
	if (this->m__ctrlSockfd != -1) {
		if (shutdown(this->m__ctrlSockfd, SHUT_RDWR) < 0) {
			eprintf(E_ERR, EFMT("shutdown(ctrlSockfd, SHUT_RDWR): Failure: %m"));
		}
		if (close(this->m__ctrlSockfd) < 0) {
			eprintf(E_ERR, EFMT("close(ctrlSockfd): Failure: %m"));
		}
		this->m__ctrlSockfd = -1;
	}
	if (this->m__dataSockfd != -1) {
		if (shutdown(this->m__dataSockfd, SHUT_RDWR) < 0) {
			eprintf(E_ERR, EFMT("shutdown(dataSockfd, SHUT_RDWR): Failure: %m"));
		}
		if (close(this->m__dataSockfd) < 0) {
			eprintf(E_ERR, EFMT("close(dataSockfd): Failure: %m"));
		}
		this->m__dataSockfd = -1;
	}

	if (this->m__ctrlSockOUT) {
		delete this->m__ctrlSockOUT;
		this->m__ctrlSockOUT = NULL;
	}

	PyGILState_STATE	gstate = PyGILState_Ensure();
	if (this->m__ID__STR) {
		Py_DECREF(this->m__ID__STR);
		this->m__ID__STR = NULL;
	}
	if (this->m__commandID__SET) {
		Py_DECREF(this->m__commandID__SET);
		this->m__commandID__SET = NULL;
	}
	if (this->m__byteStreamID__SET) {
		Py_DECREF(this->m__byteStreamID__SET);
		this->m__byteStreamID__SET = NULL;
	}
	if (this->m__scriptID__SET) {
		Py_DECREF(this->m__scriptID__SET);
		this->m__scriptID__SET = NULL;
	}
	if (this->m__order__LIST) {
		Py_DECREF(this->m__order__LIST);
		this->m__order__LIST = NULL;
	}
	PyGILState_Release(gstate);
}

int
CAgentSlot::pyEnqueOrder(
	PyObject*	order
)
{
	PyGILState_STATE	gstate = PyGILState_Ensure();
	int	iReturnVal = PyList_Append(this->m__order__LIST, order);
	printf("|"); PyObject_Print(this->m__order__LIST, stdout, 0); printf("|\n");
	PyGILState_Release(gstate);
	return iReturnVal;
}

int
CAgentSlot::pyDoOrderDown(
	CBuffer&	packet
)
{
	int	iReturnVal = 0;

	PyGILState_STATE	gstate = PyGILState_Ensure();
	PyObject*	order = NULL;
do {
	iReturnVal = this->__pyDoJobDown(packet);
	if (iReturnVal < 0) {
		eprintf(E_ERR, EFMT("CJobDown::__pyDoJobDown: Failure"));

		break;
	}

	if (iReturnVal > 0) {
		// Job-down stage...
		break;
	}

	// iReturnVal = 0
	//	1) 처리할 Job 이 없거나
	//	2) Job-down stage 처리 중이다. (JOB_DOWN_STAGE__PENDING)
	if (this->getJobDownStage() & JOB_DOWN_STAGE__PENDING) {
		break;
	}
	// 처리할 Job 이 없다.
	// 다른 명령(order)이 있는 지 알아본다.
	if (PyList_Size(this->m__order__LIST) == 0) {
		// 어떤 명령도 없다.
		break;
	}

	order = PyList_GetItem(this->m__order__LIST, 0); assert(order);
	PyObject*	type = PyDict_GetItemString(order, "Type");
	PyErr_Clear();
	long	longType = PyInt_AsLong(type);
	if (longType == -1 && PyErr_Occurred()) {
		assert(0);
	}
	// 배열에서 첫 번째 성분을 지운다.
	Py_INCREF(order);	// 지우기 전에 소유권을 가져온다.
	if (PyList_SetSlice(this->m__order__LIST, 0, 1, NULL) != 0) {
		eprintf(E_ERR, EFMT("PyList_SetSlice(|%O|, 0, 1, NULL): Failure"), this->m__order__LIST);

		iReturnVal = -1;
		break;
	}
	if ((CPacket::ORD_TYPE_T) longType == CPacket::ORD_TYPE__UPDATE_AGENT) {
		PyObject*	pathname = PyDict_GetItemString(order, "Pathname"); assert(pathname);
		const char*	pathname_cstr = PyString_AsString(pathname); assert(pathname_cstr);
		PyObject*	resume = PyDict_GetItemString(order, "Resume"); assert(resume);
		iReturnVal = CMgmtSrv::updateAgentWithOpen(packet, pathname_cstr, resume == Py_True);
		if (iReturnVal < 0) {
			eprintf(E_ERR, EFMT("CMgmtSrv::updateAgentWithOpen(|%s|): Failure"), pathname_cstr);
			break;
		}
	} else {
		//...
	}
} while(0);
	if (iReturnVal == 0) {
		Py_XDECREF(order); order = NULL;
	}
	PyGILState_Release(gstate);

	return iReturnVal;
}

ssize_t
CAgentSlot::ctrlRead(
	void
)
{
	CPacket::PACKET_TYPE_T	packetType;
	int	isComplete;
	ssize_t	nread = CSocket::read(this->m__ctrlSockfd, this->m__ctrl_rPacket, packetType, isComplete);
	if (nread < 0) {
		eprintf(E_ERR, EFMT("Control Socket: CSocket::read(): Failure"));
		return -1;
	}
#ifdef __TRACE__
	(void) printf("Control Socket: # of byte(s) = %d, isComplete = %s\n", nread, isComplete ? "True" : "False");
#endif	// __TRACE__

	if (nread == 0) {
		return 0;
	}

	if (!isComplete)
		return nread;

	// 하나의 완전한 패킷이 있다.
#ifdef __TRACE__
	(void) printf("Control Socket: Complete packet? Ok\n");
#endif	// __TRACE__
	// 사용 후에는 버퍼의 크기를 0으로 해야 한다.
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
	// 사용 후에는 버퍼의 크기를 0으로 해야 한다!!!
	int	iReturnVal = 0;
do {
	(void) printf("Received Packet: Type:= |%s|, Packet Size = %d byte(s)\n",
		packetType == CPacket::PACKET_TYPE__ERR ? "PACKET_TYPE__ERR" :
		packetType == CPacket::PACKET_TYPE__PERF ? "PACKET_TYPE__PERF" :
		packetType == CPacket::PACKET_TYPE__ORD ? "PACKET_TYPE__ORD" : "PACKET_TYPE__RESP"
		,
		nread
	);

	if (packetType == CPacket::PACKET_TYPE__RESP) {
		int	isPyDict;
		CPacket::RESP_TYPE_T	unpackedRespType;
		unsigned int		unpackedOrderID;
		CBuffer			returnBuffer;
		if (CPacket::unpackResponse(
			this->m__ctrl_rPacket,

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
			"// Control socket\n"
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

		iReturnVal = 0;
		break;
	}

	if (packetType != CPacket::PACKET_TYPE__ORD) {
		eprintf(E_WARN, EFMT("CPacket::PACKET_TYPE__ORD Only as of 5/16/2008..."));

		iReturnVal = 0;
		break;
	}

	// CPacket::PACKET_TYPE__ORD
	////////////////////////////////////////////////////////////////
{
	CPacket::ORD_TYPE_T     unpackedOrderType;
	unsigned int            unpackedOrderID;
	unsigned int            unpackedXxxID;
	unsigned int*           unpackedXxxIDPtr = &unpackedXxxID;
	int                     unpackedIsBinary;
	CBuffer                 unpackedArgBuffer;
	CBuffer                 unpackedExtraArgBuffer;
	if (CPacket::unpackOrder(
		this->m__ctrl_rPacket,

		unpackedOrderType,
		unpackedOrderID,
		&unpackedXxxIDPtr,
		unpackedIsBinary,
		unpackedArgBuffer,
		unpackedExtraArgBuffer
	) < 0) {
		eprintf(E_ERR, EFMT("CPacket::unpackOrder(): Failure"));

		iReturnVal = -1;
		break;
	}

	if (unpackedOrderType != CPacket::ORD_TYPE__JOB_DOWN_CONTROL) {
		eprintf(E_WARN, EFMT("CPacket::ORD_TYPE__JOB_DOWN_CONTROL Only as of 5/16/2008..."));

		iReturnVal = 0;
		break;
	}

	unsigned int	jdps = (unpackedXxxID & BYTE_JOB_DOWN__STAGE_MASK) >> BYTE_JOB_DOWN__STAGE_SHIFT;
#ifdef __TRACE__
	(void) printf(", Down-done Stage = (%d)%s\n", jdps,
		jdps == CJobDownPacket::JOB_DOWN_STAGE_PACKET_TYPE__SCRIPT ? "Script" : (
		jdps == CJobDownPacket::JOB_DOWN_STAGE_PACKET_TYPE__BYTE_STREAM ? "Byte Stream" : (
		jdps == CJobDownPacket::JOB_DOWN_STAGE_PACKET_TYPE__COMMAND ? "Command" : (
		jdps == CJobDownPacket::JOB_DOWN_STAGE_PACKET_TYPE__JOB ? "Job" : "Unknown?"
		)))
	);
#endif	// __TRACE__

	PyGILState_STATE	gstate = PyGILState_Ensure();
	PyObject*	stageID = NULL;
do {
	PyErr_Clear();
	stageID = PyLong_FromUnsignedLong(unpackedOrderID);
	if (PyErr_Occurred()) {
		eprintf(E_ERR, EFMT("PyLong_FromUnsignedLong() for stageID: Failure"));

		iReturnVal = -1;
		break;
	}
#ifdef __TRACE__
	(void) printf("Stage ID = "); (void) PyObject_Print(stageID, stdout, 0);
#endif	// __TRACE__
} while (0);
	PyGILState_Release(gstate);
	if (iReturnVal < 0)
		break;

	CJobDown::JOB_DOWN_STAGE_T	jds;
	jds =
	jdps == CJobDownPacket::JOB_DOWN_STAGE_PACKET_TYPE__SCRIPT ? CJobDown::JOB_DOWN_STAGE__SCRIPT :
	jdps == CJobDownPacket::JOB_DOWN_STAGE_PACKET_TYPE__BYTE_STREAM ? CJobDown::JOB_DOWN_STAGE__BYTE_STREAM :
	jdps == CJobDownPacket::JOB_DOWN_STAGE_PACKET_TYPE__COMMAND ? CJobDown::JOB_DOWN_STAGE__COMMAND :
	CJobDown::JOB_DOWN_STAGE__JOB;
	if (this->pyUpdateJobNextStage((CJobDown::JOB_DOWN_STAGE_T) jds, stageID) < 0) {
		eprintf(E_ERR, EFMT("CAgent::pyUpdateJobNextStage(|%s|, |%O|): Failure")
			,
			jdps == CJobDownPacket::JOB_DOWN_STAGE_PACKET_TYPE__SCRIPT ? "Script" : (
			jdps == CJobDownPacket::JOB_DOWN_STAGE_PACKET_TYPE__BYTE_STREAM ? "Byte Stream" : (
			jdps == CJobDownPacket::JOB_DOWN_STAGE_PACKET_TYPE__COMMAND ? "Command" : (
			jdps == CJobDownPacket::JOB_DOWN_STAGE_PACKET_TYPE__JOB ? "Job" : "Unknown?"
			))), stageID
		);

		iReturnVal = -1;
		break;
	}
}
	////////////////////////////////////////////////////////////////
} while (0);
	this->m__ctrl_rPacket.setDataSize(0);	// 더 이상 이 패킷은 사용하지 않는다!!!
////////////////////////////////////////////////////////////////////////

	return iReturnVal < 0 ? -1 : nread;
}

int
CAgentSlot::dataRead(
	CDatabaseIN&	databaseIN
)
{
	CPacket::PACKET_TYPE_T	packetType;
	int	isComplete;
	ssize_t	nread = CSocket::read(this->m__dataSockfd, this->m__data_rPacket, packetType, isComplete);
	if (nread < 0) {
		eprintf(E_ERR, EFMT("Data Socket: CSocket::read(): Failure"));
		return -1;
	}
#ifdef __TRACE__
	(void) printf("Data Socket: # of byte(s) = %d, isComplete = %s\n", nread, isComplete ? "True" : "False");
#endif	// __TRACE__

	if (!isComplete)
		return nread;

	// 하나의 완전한 패킷이 있다.
#ifdef __TRACE__
	(void) printf("Data Socket: Complete packet? Ok\n");
#endif	// __TRACE__
	// 사용 후에는 버퍼의 크기를 0으로 해야 한다.
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
	// 사용 후에는 버퍼의 크기를 0으로 해야 한다!!!
	int	iReturnVal = 0;
do {
	(void) printf("Received Packet: Type:= |%s|, Packet Size = %d byte(s)\n",
		packetType == CPacket::PACKET_TYPE__ERR ? "PACKET_TYPE__ERR" :
		packetType == CPacket::PACKET_TYPE__PERF ? "PACKET_TYPE__PERF" :
		packetType == CPacket::PACKET_TYPE__ORD ? "PACKET_TYPE__ORD" : "PACKET_TYPE__RESP"
		,
		nread
	);

	if (packetType != CPacket::PACKET_TYPE__RESP) {
		eprintf(E_WARN, EFMT("CPacket::PACKET_TYPE__RESP Only as of 5/16/2008..."));

		iReturnVal = 0;
		break;
	}

	if (packetType == CPacket::PACKET_TYPE__RESP) {
		int	isPyDict;
		CPacket::RESP_TYPE_T	unpackedRespType;
		unsigned int		unpackedOrderID;
		CBuffer			returnBuffer;
		if (CPacket::unpackResponse(
			this->m__data_rPacket,

			isPyDict,
			unpackedRespType,
			unpackedOrderID,
			returnBuffer
		) < 0) {
			eprintf(E_ERR, EFMT("CPacket::unpackResponse(): Failure"));

			iReturnVal = -1;
			break;
		}

#if 0
		(void) printf(
			"////////////////////////////////////////////////////////////////////////\n"
			"// Control socket\n"
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
#endif
#if 1
	PyGILState_STATE	gstate = PyGILState_Ensure();
	PyObject*		result = NULL;
do {
	const char*	id_str = PyString_AsString(this->m__ID__STR); assert(id_str);
	result = PyString_FromFormat("%s: %s", id_str, returnBuffer.getDataSize() ? returnBuffer.getBuf() : "N/A"); assert(result);
	const char*	result_cstr = PyString_AsString(result); assert(result_cstr);
	
	int	isPyDictFalse = 0;
	CPacket::RESP_TYPE_T	respType = CPacket::RESP_TYPE__OK;
	unsigned	orderID = 907;
	if (CPacket::packResponse(
		this->m__wPacket,

		isPyDictFalse,
		respType,
		orderID,
		result_cstr
	) < 0) {
		eprintf(E_ERR, EFMT("CPacket::packResponse(): Failure"));
		iReturnVal = -1;
		break;
	} else {
		if (databaseIN.enqueue_ts(this->m__wPacket) < 0) {
			eprintf(E_ERR, EFMT("CDatabaseIN::enqueue_ts(): Failure"));
			iReturnVal = -1;
			break;
		}
	}
} while (0);
	Py_XDECREF(result); result = NULL;
	PyGILState_Release(gstate);
#else
	if (databaseIN.enqueue_ts(data_rPacket) < 0) {
		eprintf(E_ERR, EFMT("CDatabaseIN::enque_ts(): Failure"));
		iReturnVal = -1;
		break;
	}
#endif

		iReturnVal = 0;
		break;
	}
} while (0);
	this->m__data_rPacket.setDataSize(0);	// 더 이상 이 패킷은 사용하지 않는다!!!

	return iReturnVal < 0 ? -1 : nread;
}

int
CAgentSlot::ctrlWrite(
	void
)
{
	int	isEmpty = this->m__ctrlSockOUT->flush();
	if (isEmpty < 0) {
		eprintf(E_ERR, EFMT("CQueue::flush(): Failure"));
		return -1;
	}
	// isEmpty = 1:= Queue 가 비워졌다.
	// isEmpty = 0:= 아직 Queue 에 무언가 남아있다.
	this->m__isCtrlSocketWritable = !isEmpty;

	return 0;
}

int
CAgentSlot::timedoutWrite(
	void
)
{
	int	iReturnVal = 0;
do {
#ifdef __TRACE__
	PyGILState_STATE	gstate = PyGILState_Ensure();
	(void) printf("peerAgent: ID = %d -> |", agent.getCtrlSockfd());
	(void) PyObject_Print(agent.getOrder__LIST(), stdout, 0);
	(void) printf("|\n");
	PyGILState_Release(gstate);
#endif	// __TRACE__

	int	nPacket = this->pyDoOrderDown(this->m__wPacket);
	if (nPacket < 0) {
		eprintf(E_ERR, EFMT("CAgentSlot::pyDoOrderDown: Failure"));

		iReturnVal = -1;
		break;
	}
	if (nPacket == 0) {
//		eprintf(E_INFO, EFMT("CAgentSlot::pyDoOrderDown: No order"));

		iReturnVal = 0;
		break;
	}
#ifdef __TRACE__
	CJobDown::JOB_DOWN_STAGE_T	jobDownStage = agent.getJobDownStage();
	unsigned	jds = (unsigned) jobDownStage;
	int		isPending = jds & CJobDown::JOB_DOWN_STAGE__PENDING;
	jds &= ~CJobDown::JOB_DOWN_STAGE__PENDING;
	(void) printf("Start Job-down Stage...: (%d)|%s| with Pending = |%s|\n", jds,
		jds == CJobDown::JOB_DOWN_STAGE__SCRIPT ? "Script" : (
		jds == CJobDown::JOB_DOWN_STAGE__BYTE_STREAM ? "Byte Stream" : (
		jds == CJobDown::JOB_DOWN_STAGE__COMMAND ? "Command" : (
		jds == CJobDown::JOB_DOWN_STAGE__JOB ? "Job" : "Unknown?"
		))),
		isPending ? "True" : "False"
	);
#endif	// __TRACE__

	// nPacket = 1
	// 패킷이 있다. 보낸다.
	(void) printf("Send Packet: # of byte(s) to be sent = %d\n", this->m__wPacket.getDataSize());
	assert(this->m__ctrlSockOUT->enqueue(this->m__wPacket) == 0);
	this->m__isCtrlSocketWritable = 1;
} while (0);
	return iReturnVal;
}
