#include "CWorker.h"
#include "CWorkerArg.h"
#include "CFile.h"

#include <stdio.h>	// fprintf
#include <Python.h>
#include <pthread.h>	// pthread_create
#include <signal.h>	// SIGXXX
#include "CJobDown.h"
#include "CJobDownPacket.h"
#include "eprintf.h"
#include "gmCryptAPICore.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>	// inet_aton
#include "CSocket.h"
#include "CSocketOUT.h"

const char*	CWorker::BYTE_STREAM_BY_ID__VAR_NAME = "hByteStreamByID";
const char*	CWorker::SCRIPT_BY_ID__VAR_NAME = "hScriptByID";
const char*	CWorker::COMMAND_BY_ID__VAR_NAME = "hCommandByID";

const char*	CWorker::BYTE_STREAM__KEY_NAME__LIB_SAVED_PATHNAME	= "libSavedPathname";
const char*	CWorker::BYTE_STREAM__KEY_NAME__COMMAND_ID__SET		= "CommandID__SET";
const char*	CWorker::COMMAND__KEY_NAME__BYTE_STREAM_ID__LINK	= "ByteStreamID__LINK";
const char*	CWorker::COMMAND__KEY_NAME__JOB				= "hJob";
const char*	CWorker::COMMAND__KEY_NAME__NAME			= "Name";
const char*	CWorker::COMMAND__KEY_NAME__STATUS			= "Status";
const char*	CWorker::COMMAND__KEY_NAME__CONTROL			= "Control";
const char*	CWorker::JOB__KEY_NAME__COMMAND_ID__LINK		= "CommandID__LINK";

const char*	CWorker::AGENT_COPY__NAME			= "Name";
const char*	CWorker::AGENT_COPY__RESUME			= "Resume";
const char*	CWorker::AGENT_COPY__ORDER_ID			= "Order ID";

const size_t	CWorker::CONTROL__WRITE_QUEUE_SIZE	= 1024;
const size_t	CWorker::DATA__WRITE_QUEUE_SIZE		= 640;//000;

CWorker::CWorker(
	void
)
{
	PyGILState_STATE	gstate = PyGILState_Ensure();
	this->m__hWorkerArgByID = PyDict_New();
	if (!this->m__hWorkerArgByID) {
		if (PyErr_Occurred())
			PyErr_Print();
		assert(this->m__hWorkerArgByID);
	}

	PyObject*	hScriptByID = NULL;
	PyObject*	hByteStreamByID = NULL;
	PyObject*	hCommandByID = NULL;
	int	iError = 0;
while (1) {
	if (
		!(hScriptByID = PyDict_New()) ||
		!(hByteStreamByID = PyDict_New()) ||
		!(hCommandByID = PyDict_New())
	) {	// New reference
		if (PyErr_Occurred())
			PyErr_Print();
		assert(hCommandByID || hByteStreamByID || hScriptByID);
		iError = -1;

		break;
	}

	if (
		PyDict_SetItemString(this->m__globalDict, SCRIPT_BY_ID__VAR_NAME, hScriptByID) != 0 ||
		PyDict_SetItemString(this->m__globalDict, BYTE_STREAM_BY_ID__VAR_NAME, hByteStreamByID) != 0 ||
		PyDict_SetItemString(this->m__globalDict, COMMAND_BY_ID__VAR_NAME, hCommandByID) != 0
	) {
		if (PyErr_Occurred())
			PyErr_Print();
		assert(0);

		iError = -1;

		break;
	}

	break;
}
	if (iError == 0) {
		this->m__hScriptByID = hScriptByID;
		this->m__hByteStreamByID = hByteStreamByID;
		this->m__hCommandByID = hCommandByID;
	} else {
		this->m__hScriptByID = NULL;
		this->m__hByteStreamByID = NULL;
		this->m__hCommandByID = NULL;
	}
	Py_XDECREF(hScriptByID);
	Py_XDECREF(hByteStreamByID);
	Py_XDECREF(hCommandByID);
	PyGILState_Release(gstate);

	int	iReturnVal = 0;
	int		ctrlSockfd = -1;
	unsigned	ctrlPort = 0;
	int		dataSockfd = -1;
	unsigned	dataPort = 0;
do {
	// Control Socket 을 만들어 M/S 와 연결한다.
	ctrlSockfd = CWorker::setSockfd();
	if (ctrlSockfd < 0) {
		eprintf(E_ERR, EFMT("CWorker::setSockfd() for Control Socket: Failure"));

		iReturnVal = -1;
		break;
	}
	// M/S 에게 Control Socket 임을 알린다.
	if (CSocket::sendSockID(ctrlSockfd, CPacket::RESP_TYPE__CTRL_SOCKET, 0) < 0) {
		eprintf(E_FATAL, EFMT("CWorker::sendSockID() for Control Socket: Failure"));

		iReturnVal = -1;
		break;
	}
#if 1
	// M/S 로부터 Control Socket 의 Client Port 번호를 얻는다.
	CPacket::RESP_TYPE_T	ctrlType;
	unsigned	xxx;
	if (CSocket::recvSockID(ctrlSockfd, ctrlType, xxx) < 0) {
		eprintf(E_ERR, EFMT("CWorker::recvPort() for Control Socket: Failure"));

		iReturnVal = -1;
		break;
	}
	unsigned short	indexThread = xxx >> 16;
	unsigned short	indexSlot = xxx & 0xffff;
	(void) printf("Control socket: (indexThread, indexSlot) = (%d, %d)\n", indexThread, indexSlot);
	if (ctrlType != CPacket::RESP_TYPE__CTRL_SOCKET) {
		eprintf(E_ERR, EFMT("CWorker::recvPort() for Control Socket: Not Control Socket"));

		iReturnVal = -1;
	}

	// Data Socket 을 만들어 M/S 와 연결한다.
	dataSockfd = CWorker::setSockfd();
	if (dataSockfd < 0) {
		eprintf(E_ERR, EFMT("CWorker::setSockfd() for Data Socket: Failure"));

		iReturnVal = -1;
		break;
	}
	// M/S 에게 Data Socket 임을 알린다.
	if (CSocket::sendSockID(dataSockfd, CPacket::RESP_TYPE__DATA_SOCKET, xxx) < 0) {
		eprintf(E_FATAL, EFMT("CWorker::sendSockID() for Data Socket: Failure"));

		iReturnVal = -1;
		break;
	}
#else
	// M/S 로부터 Control Socket 의 Client Port 번호를 얻는다.
	CPacket::RESP_TYPE_T	ctrlType;
	if (CSocket::recvSockID(ctrlSockfd, ctrlType, ctrlPort) < 0) {
		eprintf(E_ERR, EFMT("CWorker::recvPort() for Control Socket: Failure"));

		iReturnVal = -1;
		break;
	}
	(void) printf("Control socket: Port # = %u\n", ctrlPort);
	if (ctrlType != CPacket::RESP_TYPE__CTRL_SOCKET) {
		eprintf(E_ERR, EFMT("CWorker::recvPort() for Control Socket: Not Control Socket"));

		iReturnVal = -1;
	}

	// Data Socket 을 만들어 M/S 와 연결한다.
	dataSockfd = CWorker::setSockfd();
	if (dataSockfd < 0) {
		eprintf(E_ERR, EFMT("CWorker::setSockfd() for Data Socket: Failure"));

		iReturnVal = -1;
		break;
	}
	// M/S 에게 Data Socket 임을 알린다.
	if (CSocket::sendSockID(dataSockfd, CPacket::RESP_TYPE__DATA_SOCKET, ctrlPort) < 0) {
		eprintf(E_FATAL, EFMT("CWorker::sendSockID() for Data Socket: Failure"));

		iReturnVal = -1;
		break;
	}
	// M/S 로부터 Data Socket 의 Client Port 번호를 얻는다.
	CPacket::RESP_TYPE_T	dataType;
	if (CSocket::recvSockID(dataSockfd, dataType, dataPort) < 0) {
		eprintf(E_ERR, EFMT("CWorker::recvPort() for Data Socket: Failure"));

		iReturnVal = -1;
		break;
	}
	(void) printf("Data socket: Port # = %u\n", dataPort);
	if (dataType != CPacket::RESP_TYPE__DATA_SOCKET) {
		eprintf(E_ERR, EFMT("CWorker::recvPort() for Data Socket: Not Data Socket"));

		iReturnVal = -1;
	}
#endif

	iReturnVal = 0;
} while (0);
	if (iReturnVal < 0) {
		if (ctrlSockfd >= 0) {
			if (shutdown(ctrlSockfd, SHUT_RDWR) < 0) {
				eprintf(E_ERR, EFMT("shutdown(SHUT_RDWR): Failure: %m"));
				assert(0);
			}
			if (close(ctrlSockfd) < 0) {
				eprintf(E_ERR, EFMT("close(ctrlSockfd): Failure"));
				assert(0);
			}

			ctrlSockfd = -1;
			ctrlPort = 0;
		}
		if (dataSockfd >= 0) {
			if (shutdown(dataSockfd, SHUT_RDWR) < 0) {
				eprintf(E_ERR, EFMT("shutdown(SHUT_RDWR): Failure: %m"));
				assert(0);
			}
			if (close(dataSockfd) < 0) {
				eprintf(E_ERR, EFMT("close(dataSockfd): Failure"));
				assert(0);
			}

			dataSockfd = -1;
			dataPort = 0;
		}
	}
	this->m__ctrlSockfd = ctrlSockfd;
	this->m__ctrlPort = ctrlPort;
	this->m__dataSockfd = dataSockfd;
	this->m__dataPort = dataPort;

	this->m__pCtrlSockOUT = new CSocketOUT(ctrlSockfd, CONTROL__WRITE_QUEUE_SIZE);
#if 0	// __USE_QUEUE_THREAD__
	this->m__pDataSockOUT = new CSocketOUT(dataSockfd, DATA__WRITE_QUEUE_SIZE);
#else
	this->m__pQueueSocketThread = new CQueueSocketThread(dataSockfd, DATA__WRITE_QUEUE_SIZE);
#endif
}

CWorker::~CWorker()
{
	if (this->m__pCtrlSockOUT) {
		delete this->m__pCtrlSockOUT;
		this->m__pCtrlSockOUT = NULL;
	}
#if 0	// __USE_QUEUE_THREAD__
	if (this->m__pDataSockOUT) {
		delete this->m__pDataSockOUT;
		this->m__pDataSockOUT = NULL;
	}
#else
	if (this->m__pQueueSocketThread) {
		delete this->m__pQueueSocketThread;
		this->m__pQueueSocketThread = NULL;
	}
#endif

	if (this->m__ctrlSockfd >= 0) {
		if (shutdown(this->m__ctrlSockfd, SHUT_RDWR) < 0) {
			eprintf(E_ERR, EFMT("shutdown(SHUT_RDWR): Failure: %m"));
			assert(0);
		}
		if (close(this->m__ctrlSockfd) < 0) {
			eprintf(E_ERR, EFMT("close(this->m__ctrlSockfd): Failure"));
			assert(0);
		}

		this->m__ctrlSockfd = -1;
		this->m__ctrlPort = 0;
	}

	if (this->m__dataSockfd >= 0) {
		if (shutdown(this->m__dataSockfd, SHUT_RDWR) < 0) {
			eprintf(E_ERR, EFMT("shutdown(SHUT_RDWR): Failure: %m"));
			assert(0);
		}
		if (close(this->m__dataSockfd) < 0) {
			eprintf(E_ERR, EFMT("close(this->m__dataSockfd): Failure"));
			assert(0);
		}

		this->m__dataSockfd = -1;
		this->m__dataPort = 0;
	}

	if (this->m__hWorkerArgByID) {
		PyObject*	keyWorker;
		PyObject*	workerArg;
		Py_ssize_t	pos = 0;	// 항상 0으로 설정한다.
		PyGILState_STATE	gstate = PyGILState_Ensure();
		while (PyDict_Next(m__hWorkerArgByID, &pos, &keyWorker, &workerArg)) {	// All are borrowed references.
			// 이미 설정되어 있다.
			CWorkerArg*	pWorkerArg = (CWorkerArg*) PyLong_AsVoidPtr(workerArg);
			if (!pWorkerArg) {
				if (PyErr_Occurred())
					PyErr_Print();
				assert(pWorkerArg);

				continue;
			}
			delete pWorkerArg;
		}

		Py_DECREF(this->m__hWorkerArgByID);
		PyGILState_Release(gstate);

		this->m__hWorkerArgByID = NULL;
	}
}


void
CWorker::help(
	void
)
{
	const char*	commandList[] = {
		"HELP",


		"LOAD__SYSINFO_FUNCTION \"setSysInfoFunc.py\"\n"
			"\tgetSysInfo.py 에 정의된 함수를 호출한 결과를\n"
			"\t__main__ 모듈에 hSysInfo 변수로 등록",

		"LOAD__WORKER_MODULE_BYTE_STREAM \"moduleName\" \"libLoadedModule.so\"\n"
			"\tlibModule.so 에 해당하는 바이트스트림을 libmoduleNameSaved.so 로 저장;\n"
			"\t\t1) Python 엔진에서 따로 관리(LIST__WORKER_MODULES 명령)\n"
			"\t\t2) Worker 쓰레드의 main함수로 moduleName 이 사용",

		"LOAD__ARGV_FUNCTION \"argv_func\" \"argv_func.py\"\n"
			"\tcmd_func 이 정의된 스크립트를 내려받아 __main__ 모듈에 등록;\n"
			"\t이후에 내려받는 다른 함수에서 사용가능",

		"LOAD__WORKER_SCRIPT_FUNCTION \"script_func\" \"script_func.py\"\n"
			"\t작업의 결과를 처리하는, script_func 이 정의된 스크립트를 내려받아\n"
			"\t\t1) __main__ 모듈에 등록\n"
			"\t\t2) Python 엔진에서도 따로 관리(LIST__WORKER_SCRIPTS 명령)",

		"LOAD__WORKER_VARIABLE \"setWorkerVar.py\"\n"
			"\tWorker 쓰레드의 동작과 관련된, worker 라는 변수를 __main__ 모듈에 등록",

		"CHECK__WORKERS\n"
			"\tworker 변수안에 있는,\n"
			"\t\"BYTE_STREAM__LINK\"(\"WORKER_SCRIPT__LINK\")에 필요한 것들이 제대로 등록되었는 지 확인",


		"LIST__ALL\n"
			"\t__main__ 모듈에 등록된 모든 오브젝트를 나열",
		"LIST__WORKER_MODULES\n"
			"\tPython 엔진에서 따로 관리하는 작업 모듈들을 나열",
		"LIST__WORKER_SCRIPTS\n"
			"\tPython 엔진에서 따로 관리하는, 작업의 결과를 처리하는 스크립트들을 나열",

		"SHOW__WORKERS\n"
			"\t__main__ 모듈에 등록된 worker 변수의 내용을 표시",


		"DO__WORK\n"
			"\thWorker 에 있는 내용에 따라 Worker 쓰레드를 동작시킨다.",

		NULL
	};

	char**	pCommand = const_cast<char**>(commandList);
	while (*pCommand) {
		(void) printf("%s\n", *pCommand);

		++pCommand;
	}
	(void) printf(
		"----------------------------------------\n"
		"명령을 실행하는 순서에 관련된 규칙은 다음과 같다:\n"
		"\t[규칙 1] XXX_FUNCTION 은 Python 엔진에서 실제로 실행하므로,\n"
		"\t\t그 전에 필요한 변수(hSysInfo)나 함수(cmd_func)는 미리 설정되어 있어야 한다.\n"
		"\t[규칙 2] 준비단계의 마지막 단계인 CHECK__WORKERS 에서 확인하는,\n"
		"\tXXX_LINK 에 해당하는 값들은 미리 설정되어 있어야 한다.\n"
		"*) LIST__XXX 나 SHOW__XXX 언제든지 사용가능하다.\n"
	);
}

void
CWorker::pyListAllObjects(
	void
)
{
	PyGILState_STATE	gstate = PyGILState_Ensure();

	PyObject*	key;
	PyObject*	val;
	Py_ssize_t	pos = 0;	// 처음에는 0으로 설정해야 한다.
	while (PyDict_Next(m__globalDict, &pos, &key, &val)) {	// All are borrowed references.
#if 1
		(void) PyObject_Print(key, stdout, 0);
		(void) fprintf(stdout, " = ");
		(void) PyObject_Print(val, stdout, 0);
		(void) fprintf(stdout, "\n");
#else
		// Key
		PyObject*	keyType = PyObject_Type(key);	// New reference
		(void) printf("(|%s|)", keyType ? ((PyTypeObject*) keyType)->tp_name : "N/A");
		Py_XDECREF(keyType);
		PyObject*	keyStr = PyObject_Str(key);	// New reference
		(void) printf("|%s| = ", keyStr ? PyString_AsString(keyStr) : "N/A");
		Py_XDECREF(keyStr);

		// Value
		PyObject*	valType = PyObject_Type(val);	// New reference
		(void) printf("(|%s|)", valType ? ((PyTypeObject*) valType)->tp_name : "N/A");
		Py_XDECREF(valType);
		PyObject*	valStr = PyObject_Str(val);	// New reference
		(void) printf("|%s|\n", valStr ? PyString_AsString(valStr) : "N/A");
		Py_XDECREF(valStr);
#endif
	}

	PyGILState_Release(gstate);
}

void
CWorker::pyListWorkers(
	void
)
{
	PyGILState_STATE	gstate = PyGILState_Ensure();
	assert(this->m__hCommandByID);

	PyObject*	commandID;
	PyObject*	hCommand;
	Py_ssize_t	pos = 0;
	while (PyDict_Next(this->m__hCommandByID, &pos, &commandID, &hCommand)) {	// All borrowed refs.
		PyObject*	workerArg = PyDict_GetItem(this->m__hWorkerArgByID, commandID);
		if (!workerArg) {
			eprintf(E_ERR, EFMT("PyDict_GetItem(|%O|, |%O|): Failure"), this->m__hWorkerArgByID, commandID);
			continue;
		}
		CWorkerArg*	pWorkerArg = (CWorkerArg*) PyLong_AsVoidPtr(workerArg);
		if (!pWorkerArg) {
			eprintf(E_ERR, EFMT("PyLong_AsVoidPtr(|%O|): Failure"), workerArg);
			continue;
		}
		(void) fprintf(stderr, "Command ID = |"); (void) PyObject_Print(commandID, stderr, 0);
		(void) fprintf(stderr, "|, Thread ID = |%x|, hCommand = |", (unsigned) pWorkerArg->id); (void) PyObject_Print(hCommand, stderr, 0);
		(void) fprintf(stderr, "|\n");
	}

	PyGILState_Release(gstate);
}

int
CWorker::pyKill(
	PyObject*	commandID
)
{
	assert(this->m__hCommandByID);
	int	iReturnVal = 0;

	PyGILState_STATE	gstate = PyGILState_Ensure();
	CWorkerArg*	pWorkerArg = NULL;
	PyObject*	control = NULL;
do {
	PyObject*	workerArg = PyDict_GetItem(this->m__hWorkerArgByID, commandID);
	if (!workerArg) {
		eprintf(E_WARN, EFMT("PyDict_GetItem(|%O|, |%O|): Failure"), this->m__hWorkerArgByID, commandID);

		iReturnVal = -1;
		break;
	}
	pWorkerArg = (CWorkerArg*) PyLong_AsVoidPtr(workerArg);
	if (!pWorkerArg) {
		eprintf(E_ERR, EFMT("PyLong_AsVoidPtr(|%O|): Failure"), workerArg);

		iReturnVal = -1;
		break;
	}

	PyObject*	hCommand = PyDict_GetItem(this->m__hCommandByID, commandID);
	if (!hCommand) {
		eprintf(E_ERR, EFMT("PyDict_GetItem(|%O|, |%O|): Failure"), this->m__hCommandByID, commandID);

		iReturnVal = -1;
		break;
	}
	long	ctrl = CMD_CTRL__STOP;
	control = PyInt_FromLong(ctrl); assert(control);	// New ref.
	if (PyDict_SetItemString(hCommand, COMMAND__KEY_NAME__CONTROL, control) != 0) {
		eprintf(E_ERR, EFMT("PyDict_SetItemString(|%O|, |%s|, |%O|): Failure"), hCommand, COMMAND__KEY_NAME__CONTROL, control);

		iReturnVal = -1;
		break;
	}
} while (0);
	PyGILState_Release(gstate);
	if (iReturnVal < 0)
		return iReturnVal;

	// Waiting for Worker to terminate...
	if (pthread_join(pWorkerArg->id, NULL) != 0) {
		(void) fprintf(stderr, "pthread_join(|%x|): Failure\n", (unsigned int) pWorkerArg->id);
		return -1;
	}

	// Worker 가 가졌던 자원을 돌려준다.
	delete pWorkerArg;

	// this->m__hWorkerArgByID 에서 commandID 에 해당하는 item 을 삭제한다.
{
	PyGILState_STATE	gstate = PyGILState_Ensure();
do {
	if (PyDict_DelItem(this->m__hWorkerArgByID, commandID) != 0) {
		eprintf(E_ERR, EFMT("PyDict_DelItem(|%O|, |%O|): Failure"), this->m__hWorkerArgByID, commandID);

		iReturnVal = -1;
		break;
	}
} while (0);
	PyGILState_Release(gstate);
}
	return iReturnVal;
}

int
CWorker::pyKillAll(
	void
)
{
	assert(this->m__hWorkerArgByID);

	PyObject*	commandID;
	PyObject*	workerArg;
	Py_ssize_t	pos = 0;	// 항상 0으로 설정한다.
	int	iError = 0;
	PyGILState_STATE	gstate = PyGILState_Ensure();
	while (PyDict_Next(m__hWorkerArgByID, &pos, &commandID, &workerArg)) {	// All are borrowed references.
		PyGILState_Release(gstate);
		if (this->pyKill(commandID) < 0) {
			PyGILState_STATE	gstate_ = PyGILState_Ensure();
			eprintf(E_ERR, EFMT("CWorker::pyKill(|%O|): Failure"), commandID);
			iError = -1;
			PyGILState_Release(gstate_);
		}
		gstate = PyGILState_Ensure();
	}
	PyGILState_Release(gstate);

	return iError;
}

int
CWorker::pyUnpack(
	const CBuffer&	rPacket,
	CBuffer&	wPacket,
	UPDATE_AGENT_T&	ua
)
{
	int	iReturnVal = 0;
	PyGILState_STATE	gstate = PyGILState_Ensure();
do {
	CPacket::ORD_TYPE_T     unpackedOrderType;
	unsigned int            unpackedOrderID;
	unsigned int            unpackedXxxID;
	unsigned int*           unpackedXxxIDPtr = &unpackedXxxID;
	int                     unpackedIsBinary;
	CBuffer                 unpackedArgBuffer;
	CBuffer                 unpackedExtraArgBuffer;
	if (CPacket::unpackOrder(
		rPacket,

		unpackedOrderType,
		unpackedOrderID,
		&unpackedXxxIDPtr,
		unpackedIsBinary,
		unpackedArgBuffer,
		unpackedExtraArgBuffer
	) < 0) {
		(void) fprintf(stderr,
			"ERROR: "
			"__FILE__:= |%s|, __LILNE_NO__ = %d: "
			"CPacket::unpackOrder(): Failure\n"
			,
			__FILE__, __LINE__
		);

		iReturnVal = -1;
		break;
	}
#if 0	// 내용을 확인한다.
	(void) printf(
		"////////////////////////////////////////////////////////////////////////\n"
		"Order Type: %d\n"
		"Order ID: %d\n"
		"XXX__ID: %d\n"
		"isBinary: %s\n"
		"Argument: |%s|(%d)\n"
		"Extra-argument: |%s|(%d)\n"
		"////////////////////////////////////////////////////////////////////////\n"
		,
		unpackedOrderType,
		unpackedOrderID,
		unpackedXxxIDPtr ? *unpackedXxxIDPtr : (unsigned int) -1,
		unpackedIsBinary ? "True" : "False",
		unpackedIsBinary ? "0x..." : unpackedArgBuffer.getBuf(), unpackedArgBuffer.getDataSize(),
		unpackedExtraArgBuffer.getDataSize() ? unpackedExtraArgBuffer.getBuf() : "N/A",
		unpackedExtraArgBuffer.getDataSize() ? unpackedExtraArgBuffer.getDataSize() : (unsigned int) -1
	);
#endif

	if (unpackedOrderType == CPacket::ORD_TYPE__UPDATE_AGENT) {
		if (update(unpackedArgBuffer, ua) < 0) {
			iReturnVal = -1;
			break;
		}
		ua.orderID = unpackedOrderID;
		ua.flag = UPDATE_AGENT__UPDATE;
		if (unpackedXxxID)
			ua.flag |= UPDATE_AGENT__RESUME;
		// 받았다는 응답을 보낸다.
		int			isPyDict = 0;
		CPacket::RESP_TYPE_T	respType = CPacket::RESP_TYPE__ACK;
		unsigned int		orderID = unpackedOrderID;
		if (CPacket::packResponse(
			wPacket,

			isPyDict,
			respType,
			orderID,
			NULL
		) < 0) {
			eprintf(E_ERR, EFMT("CPacket::packResponse(RESP_TYPE__ACK): Failure"));

			iReturnVal = -1;
		}

		iReturnVal = 0;
		break;
	}

	if (unpackedOrderType != CPacket::ORD_TYPE__JOB_DOWN_CONTROL) {
		(void) fprintf(stderr,
			"INFO: "
			"__FILE__:= |%s|, __LINE_NO__ = %d: "
			"CPacket::ORD_TYPE__JOB_DOWN Only: Ignored...\n"
			,
			__FILE__, __LINE__
		);

		iReturnVal = -1;
		break;
	}

	unsigned int	jdsp = (unpackedXxxID & BYTE_JOB_DOWN__STAGE_MASK) >> BYTE_JOB_DOWN__STAGE_SHIFT;
	CJobDown::JOB_DOWN_STAGE_T	jds;
	if (jdsp == CJobDownPacket::JOB_DOWN_STAGE_PACKET_TYPE__SCRIPT)
		jds = CJobDown::JOB_DOWN_STAGE__SCRIPT;
	else if (jdsp == CJobDownPacket::JOB_DOWN_STAGE_PACKET_TYPE__BYTE_STREAM)
		jds = CJobDown::JOB_DOWN_STAGE__BYTE_STREAM;
	else if (jdsp == CJobDownPacket::JOB_DOWN_STAGE_PACKET_TYPE__COMMAND)
		jds = CJobDown::JOB_DOWN_STAGE__COMMAND;
	else if (jdsp == CJobDownPacket::JOB_DOWN_STAGE_PACKET_TYPE__JOB)
		jds = CJobDown::JOB_DOWN_STAGE__JOB;
	else {
		(void) fprintf(stderr,
			"ERROR: "
			"__FILE__:= |%s|, __LINE__NO__ = %d: "
			"CWorker::pyUnpack: CJobDownPacket::JOB_DOWN_PACKET_TYPE_T = %d: Unknown\n"
			,
			__FILE__, __LINE__, jdsp
		);

		iReturnVal = -1;
		break;
	}
#if 1
	(void) printf("Job-down Stage = (%d)%s\n", jds,
		jds == CJobDown::JOB_DOWN_STAGE__SCRIPT ? "Script" :
		jds == CJobDown::JOB_DOWN_STAGE__BYTE_STREAM ? "Byte Stream" :
		jds == CJobDown::JOB_DOWN_STAGE__COMMAND ? "Command" : "Job"
	);
#endif

	// jds 에 따라 주어진 내용을 처리한다.
	PyObject*	id = NULL;
do {
	id = PyLong_FromUnsignedLong(unpackedOrderID);
	if (!id) {
		eprintf(E_ERR, EFMT("PyLong_FromUnsignedLong(|%u|): Failure"), unpackedOrderID);

		iReturnVal = -1;
		break;
	}

	if (jds == CJobDown::JOB_DOWN_STAGE__SCRIPT) {
		if (this->__pySetScript(
			id,
#if 1	// Source 가 "인자"에...
			unpackedArgBuffer,
#else	// Source 가 "추가인자"에...
#endif
			unpackedExtraArgBuffer
		) < 0) {
			(void) fprintf(stderr,
				"ERROR: "
				"__FILE__:= |%s|, __LINE_NO__ = %d: "
				"CWorker::pyUnpack: "
				"CWorker::setScript(|%d|, |%s|): Failure\n"
				,
				__FILE__, __LINE__,
				unpackedOrderID,
				unpackedExtraArgBuffer.getDataSize() ? unpackedExtraArgBuffer.getBuf() : "N/A"
			);

			iReturnVal = -1;
		}
		iReturnVal = 0;
	} else if (jds == CJobDown::JOB_DOWN_STAGE__BYTE_STREAM) {
		PyObject*	hByteStream = NULL;
		PyObject*	byteStream = NULL;
		PyObject*	commandID__SET = NULL;
do {
		hByteStream = (PyObject*) this->__pySetObjectFromString(unpackedExtraArgBuffer.getBuf()); assert(hByteStream);
		const char*	byteStreamPtr = unpackedArgBuffer.getBuf();
		Py_ssize_t	byteStreamLen = unpackedArgBuffer.getDataSize();
		byteStream = PyString_FromStringAndSize(byteStreamPtr, byteStreamLen); assert(byteStream);

		PyObject*	hPrevByteStream = this->__pyGethPrevByteStream(id, hByteStream, unpackedArgBuffer);
		if (hPrevByteStream == (PyObject*) 1) {
			// 똑같은 내용의 Byte Stream 이 등록되어있다.
			eprintf(E_INFO, EFMT("Byte Stream(|%O|): Already registered: Ignored"), id);
			iReturnVal = 0;
			break;
		}
		if (hPrevByteStream) {
			PyObject*	prevCommandID__SET = PyDict_GetItemString(hPrevByteStream, BYTE_STREAM__KEY_NAME__COMMAND_ID__SET); assert(prevCommandID__SET);
			commandID__SET = PySet_New(prevCommandID__SET); assert(commandID__SET);	// New ref.
			PyObject*	set = PySet_New(prevCommandID__SET); assert(set);	// New ref.
			Py_ssize_t	nset = PySet_Size(set);
			for (Py_ssize_t i = 0; i < nset; ++i) {
				PyObject*	commandID = PySet_Pop(set); assert(commandID);
				PyGILState_Release(gstate);
				if (this->pyKill(commandID) < 0) {
					eprintf(E_ERR, EFMT("CWorker::pyKill(|%O|): Failure"), commandID);
				}
				gstate = PyGILState_Ensure();
			}
			Py_DECREF(set);
		}

		if (!commandID__SET)
			commandID__SET = PySet_New(NULL); assert(commandID__SET);
		assert(PyDict_SetItemString(hByteStream, BYTE_STREAM__KEY_NAME__COMMAND_ID__SET, commandID__SET) == 0);
		if (this->__pySaveByteStream(id, hByteStream, unpackedArgBuffer) < 0) {
			(void) fprintf(stderr,
				"ERROR: "
				"__FILE__:= |%s|, __LINE_NO__ = %d: "
				"CWorker::pyUnpack: "
				"CWorker::saveByteStream(|%d|, |%s|, |%s|): Failure\n"
				,
				__FILE__, __LINE__,
				unpackedOrderID,
				unpackedArgBuffer.getDataSize() ? unpackedArgBuffer.getBuf() : "N/A",
				unpackedExtraArgBuffer.getDataSize() ? unpackedExtraArgBuffer.getBuf() : "N/A"
			);

			iReturnVal = -1;
		}
		if (commandID__SET) {
			PyObject*	set = PySet_New(commandID__SET);
			Py_ssize_t	nset = PySet_Size(set);
			int	overwriteTrue = 1;
			for (Py_ssize_t i = 0; i < nset; ++i) {
				PyObject*	commandID = PySet_Pop(set); assert(commandID);
				PyObject*	hCommand = PyDict_GetItem(this->m__hCommandByID, commandID); assert(hCommand);
				if (this->__pyCreateWorkerThread(commandID, hCommand, overwriteTrue) < 0) {
					eprintf(E_ERR, EFMT("CWorker::__pyCreateWorkerThread(|%O|, |%O|): Failure"), commandID, hCommand);
				}
			}
			Py_DECREF(set); set = NULL;
		}

		iReturnVal = 0;
} while (0);
		Py_XDECREF(hByteStream); hByteStream = NULL;
		Py_XDECREF(byteStream); byteStream = NULL;
		Py_XDECREF(commandID__SET); commandID__SET = NULL;
	} else if (jds == CJobDown::JOB_DOWN_STAGE__COMMAND) {
		if (this->__pySetCommand(id, unpackedArgBuffer) < 0) {
			(void) fprintf(stderr,
				"ERROR: "
				"__FILE__:= |%s|, __LINE_NO__ = %d: "
				"CWorker::pyUnpack: "
				"CWorker::__pySetCommand(|%s|): Failure\n"
				,
				__FILE__, __LINE__,
				unpackedArgBuffer.getDataSize() ? unpackedArgBuffer.getBuf() : "N/A"
			);

			iReturnVal = -1;
		}
		iReturnVal = 0;
	} else {	// CJobDown::JOB_DOWN_STAGE__JOB
		if (this->__pySetJob(id, unpackedArgBuffer) < 0) {
			(void) fprintf(stderr,
				"ERROR: "
				"__FILE__:= |%s|, __LINE_NO__ = %d: "
				"CWorker::pyUnpack: "
				"CWorker::__pySetJob(|%s|): Failure\n"
				,
				__FILE__, __LINE__,
				unpackedArgBuffer.getDataSize() ? unpackedArgBuffer.getBuf() : "N/A"
			);

			iReturnVal = -1;
		}
		iReturnVal = 0;
	}
} while (0);
	Py_XDECREF(id); id = NULL;
	// iReturnVal?

	// Send "Job-down Control" packet
{
	// 패킷을 만든다.
	CPacket::ORD_TYPE_T	orderType = CPacket::ORD_TYPE__JOB_DOWN_CONTROL;
	unsigned int		orderID = unpackedOrderID;
	unsigned int		xxxID = unpackedXxxID;
	unsigned int*		xxxIDPtr = &xxxID;
	int	isBinary = 0;
	if (CPacket::packOrder(
		wPacket,

		orderType,
		orderID,
		xxxIDPtr,
		isBinary,
		NULL, 0,
		NULL
	) < 0) {
		(void) fprintf(stderr,
			"ERROR: "
			"__FILE__:= |%s|, __LILNE_NO__ = %d: "
			"CPacket::packOrder(): Failure\n"
			,
			__FILE__, __LINE__
		);

		iReturnVal = -1;
		break;
	}

	// 패킷을 풀어서 확인한다.
#if 0	// 확인
{
	CPacket::ORD_TYPE_T	unpackedOrderType;
	unsigned int		unpackedOrderID;
	unsigned int		unpackedXxxID;
	unsigned int*		unpackedXxxIDPtr = &unpackedXxxID;
	int			unpackedIsBinary;
	CBuffer			unpackedArgBuffer;
	CBuffer			unpackedExtraArgBuffer;
	if (CPacket::unpackOrder(
		wPacket,

		unpackedOrderType,
		unpackedOrderID,
		&unpackedXxxIDPtr,
		unpackedIsBinary,
		unpackedArgBuffer,
		unpackedExtraArgBuffer
	) < 0) {
		(void) fprintf(stderr,
			"ERROR: "
			"__FILE__:= |%s|, __LILNE_NO__ = %d: "
			"CPacket::unpackOrder(): Failure\n"
			,
			__FILE__, __LINE__
		);

		iReturnVal = -1;
		break;
	}

	(void) printf(
		"////////////////////////////////////////////////////////////////////////\n"
		"Order Type: %d\n"
		"Order ID: %d\n"
		"XXX__ID: %d\n"
		"isBinary: %s\n"
		"Argument: %s(%d)\n"
		"Extra-argument: %s(%d)\n"
		"////////////////////////////////////////////////////////////////////////\n"
		,
		unpackedOrderType,
		unpackedOrderID,
		unpackedXxxIDPtr ? *unpackedXxxIDPtr : -1,
		unpackedIsBinary ? "True" : "False",
		unpackedArgBuffer.getDataSize() ? unpackedArgBuffer.getBuf() : "N/A",
		unpackedArgBuffer.getDataSize() ? unpackedArgBuffer.getDataSize() : -1,
		unpackedExtraArgBuffer.getDataSize() ? unpackedExtraArgBuffer.getBuf() : "N/A",
		unpackedExtraArgBuffer.getDataSize() ? unpackedExtraArgBuffer.getDataSize() : -1
	);
}
#endif
}
} while (0);
	PyGILState_Release(gstate);
	return iReturnVal;
}
int
CWorker::__pySetScript(
	PyObject*	scriptID,
#if 1	// Source 가 "인자"에...
	const CBuffer&	scriptSrc,
#else	// Source 가 "추가인자"에...
#endif
	const CBuffer&	scriptInfo
)
{
	assert(this->m__hScriptByID);
	assert(scriptInfo.getDataSize());

	int	iReturnVal = 0;

	PyObject*	hScript = NULL;
#if 1
	PyObject*	source = NULL;
#else
#endif
do {
	hScript = (PyObject*) this->__pySetObjectFromString(scriptInfo.getBuf());	// New ref.
	if (!hScript) {
		assert(hScript);
		iReturnVal = -1;
		break;
	}

	// <Script...>만의 고유한 일을 하자.
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
	PyObject*	name = PyDict_GetItemString(hScript, CJobDown::SCRIPT__KEY_NAME__NAME); assert(name);
#if 1
	source = PyString_FromString(scriptSrc.getBuf());
	if (!source) {
		eprintf(E_ERR, EFMT("PyString_FromString(|%s|): Failure"), scriptSrc.getBuf());
		iReturnVal = -1;
		break;
	}
	if (PyDict_SetItemString(hScript, CJobDown::SCRIPT__KEY_NAME__SOURCE, source) != 0) {
		eprintf(E_ERR, EFMT("PyDict_SetItemString(|%O|, |%s|: |%O|): Failure"), hScript, CJobDown::SCRIPT__KEY_NAME__SOURCE, source);
		iReturnVal = -1;
		break;
	}
#else
	PyObject*	source = PyDict_GetItemString(hScript, CJobDown::SCRIPT__KEY_NAME__SOURCE); assert(source);
#endif
////////////////////////////////////////////////////////////////////////
	// 같은 Script 를 내려받았는 지 확인한다.
	// 이름도 같고, Script 의 내용도 같다면 반복할 필요가 없다.
	PyErr_Clear();
	PyObject*	hPrevScript = PyDict_GetItem(this->m__hScriptByID, scriptID);
	if (hPrevScript) {	// 같은 scriptID 를 가진 Script 를 이미 내려받았다.
		PyObject*	prevName = PyDict_GetItemString(hPrevScript, CJobDown::SCRIPT__KEY_NAME__NAME); assert(name);
		int	isEqual = PyObject_RichCompareBool(name, prevName, Py_EQ); assert(isEqual >= 0);
		if (isEqual) {	// 적어도 이름은 같다.
			// 다음은 소스 자체를 비교하자.
			PyObject*	prevSource = PyDict_GetItemString(hPrevScript, CJobDown::SCRIPT__KEY_NAME__SOURCE); assert(source);
			isEqual = PyObject_RichCompareBool(source, prevSource, Py_EQ); assert(isEqual >= 0);
			if (isEqual) {
				// 소스도 같다.
				// 처리할 필요가 없다.
				eprintf(E_WARN, EFMT("Script(|%O|): Nothing new: Skipped"), name);
				return 0;
			}
		}
	}
	if (PyErr_Occurred()) {
		eprintf(E_ERR, EFMT("PyDict_GetItem(|%O|, |%O|): Failure"), this->m__hScriptByID, scriptID);

		iReturnVal = -1;
		break;
	}
////////////////////////////////////////////////////////////////////////
	int	overwriteTrue = 1;
	PyObject*	typeBlob = PyDict_GetItemString(hScript, CJobDown::SCRIPT__KEY_NAME__TYPE_BLOB);
	if (this->__pyDoTypeBlob(typeBlob, name, source, overwriteTrue) < 0) {
		(void) fprintf(stderr,
			"ERROR: "
			"__FILE__:= |%s|, __LINE_NO__ = %d: "
			"CWorker::setScript: "
			"CPythonEngine::__pyDoTypeBlob(|"
			,
			__FILE__, __LINE__
		); (void) PyObject_Print(typeBlob, stderr, 0);
		(void) fprintf(stderr, "|, |"); (void) PyObject_Print(name, stderr, 0);
		(void) fprintf(stderr, "|, |"); (void) PyObject_Print(source, stderr, 0);
		(void) fprintf(stderr, "|): Failure\n");
		assert(0);

		iReturnVal = -1;
		break;
	}
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

	if (PyDict_SetItem(this->m__hScriptByID, scriptID, hScript) != 0) {
		// 앞서 CPythonEngine::__pyDoTypeBlob 에서 처리된 것은 어떻게 하지???
		if (PyErr_Occurred())
			PyErr_Print();
		assert(0);
		iReturnVal = -1;
		break;
	}
} while(0);
	Py_XDECREF(hScript); hScript = NULL;
#if 1
	Py_XDECREF(source); source = NULL;
#else
#endif

	return iReturnVal;
}

PyObject*
CWorker::__pyGethPrevByteStream(
	PyObject*	byteStreamID,
	PyObject*	hByteStream,
	const CBuffer&	byteStreamBuffer
)
{
	PyObject*	hPrevByteStream = NULL;

do {
	PyObject*	entryPoint = PyDict_GetItemString(hByteStream, CJobDown::BYTE_STREAM__KEY_NAME__ENTRY_POINT); assert(entryPoint);
	const char*	strEntryPoint = PyString_AsString(entryPoint); assert(strEntryPoint);
	PyErr_Clear();
	hPrevByteStream = PyDict_GetItem(this->m__hByteStreamByID, byteStreamID);
	if (PyErr_Occurred()) {
		eprintf(E_ERR, EFMT("PyDict_GetItem(|%O|, |%O|): Failure"), this->m__hByteStreamByID, byteStreamID);
		assert(0);
	}
	if (!hPrevByteStream) {
		break;
	}

	// 같은 byteStreamID 를 가진 Byte Stream 을 이미 내려받았다.
	PyObject*	prevEntryPoint = PyDict_GetItemString(hPrevByteStream, CJobDown::BYTE_STREAM__KEY_NAME__ENTRY_POINT); assert(prevEntryPoint);
	int	isEqual = PyObject_RichCompareBool(entryPoint, prevEntryPoint, Py_EQ); assert(isEqual >= 0);
	// 내려받은 Byte Stream 의 MD5 값과 이 전에 내려받아 저장한 Byte Stream 의 MD5 값을 비교한다.
	// 이 전에 내려받은 Byte Stream 의 MD5...
	PyObject*	prevMD5 = PyDict_GetItemString(hPrevByteStream, CJobDown::BYTE_STREAM__KEY_NAME__MD5); assert(prevMD5);
	const char*	prevMD5_cstr = PyString_AsString(prevMD5); assert(prevMD5_cstr);
	// 새로 내려받은 Byte Stream 의 MD5...
	unsigned char	bDigestedDataHexStr[2 * MD5_DIGEST_LENGTH + 1];	// 1 for NUL
	assert(GetMessageDigestMD5HexStr((const unsigned char*) byteStreamBuffer.getBuf(), byteStreamBuffer.getDataSize(), bDigestedDataHexStr) == 0);
	bDigestedDataHexStr[2 * MD5_DIGEST_LENGTH] = '\0';	// 문자열의 끝을 표시
	// 소문자로 바꾼다.
	unsigned char*	p = bDigestedDataHexStr;
	do {
		*p = tolower(*p);
	} while (*++p);
	(void) printf("MD5: |%s| -> |%s|\n", bDigestedDataHexStr, prevMD5_cstr);
	if (strcmp(prevMD5_cstr, (const char*) bDigestedDataHexStr) == 0)
		hPrevByteStream = (PyObject*) 1;
} while (0);

	return hPrevByteStream;
}
int
CWorker::__pySaveByteStream(
	PyObject*	byteStreamID,
	PyObject*	hByteStream,
	const CBuffer&	byteStreamBuffer
)
{
	assert(this->m__hByteStreamByID);
	assert(hByteStream);
	assert(byteStreamBuffer.getDataSize());

	int	iReturnVal = 0;

	FILE*		f = NULL;	// Byte Stream 을 저장할 파일과 관련된 파일스트림에 대한 포인터
	PyObject*	libSavedPathname = NULL;
do {
	PyObject*	entryPoint = PyDict_GetItemString(hByteStream, CJobDown::BYTE_STREAM__KEY_NAME__ENTRY_POINT); assert(entryPoint);
	const char*	strEntryPoint = PyString_AsString(entryPoint); assert(strEntryPoint);
	const char*	byteStreamPtr = NULL;
	Py_ssize_t	byteStreamLen = 0;
	byteStreamPtr = byteStreamBuffer.getBuf();
	byteStreamLen = byteStreamBuffer.getDataSize();

	// 특정한 파일이름으로 저장한다.
	char	strLibSavedPathname[256];	// 저장할 파일이름
	(void) snprintf(strLibSavedPathname, 256, "%s/lib%s_", CPythonEngine::WORKER_MODULE_PATH, strEntryPoint);
////////////////////////////////////////////////////////////////////////
// 같은 서버에서 서로 다른 Agent 를 실행하는 경우,
// 이미 해당 모듈이 있을 수 있다.
// 이 경우 덮어쓰면 이 모듈을 사용하던 Agent 에 문제가 생긴다.
// 같은 이름의 파일이 있다면 그 내용을 읽어 새로 내려받을 모듈과 비교한다.
// 그 내용까지 같다면 같은 파일을 사용한다.
// 그렇지 않다면 다른 이름을 사용한다;
//	mkstemp 처럼 "_XXXXXX" 을 사용한다.
// 왜냐하면 같은 모듈이더라고 각 Agent 는 각기 다른 모듈을 사용할 수 있으므로.
////////////////////////////////////////////////////////////////////////
	int	isExist = 0;
	int	isSame = 0;
do {
	struct stat	sb;
	if (stat(strLibSavedPathname, &sb) < 0) {
		int	errnum = errno;
		eprintf(E_INFO, EFMT("stat(|%s|): Failure: %d(%d?): %m"), strLibSavedPathname, errnum);
		isExist = 0;
		break;
	}
	isExist = 1;

	CFile	fg;
	int	isPipeFalse = 0;
	if (fg.openFile(strLibSavedPathname, "rb", isPipeFalse) < 0) {
		eprintf(E_ERR, EFMT("CFile::openFile(|%s|, |rb|): Failure"), strLibSavedPathname);
		iReturnVal = -1;
		break;
	}
	if (fg.readAll() < 0) {
		eprintf(E_ERR, EFMT("CFile::readAll(|%s|): Failure"), strLibSavedPathname);
		iReturnVal = -1;
		break;
	}
	if (fg.closeFile() < 0) {
		eprintf(E_ERR, EFMT("CFile::closeFile(|%s|): Failure"), strLibSavedPathname);
		iReturnVal = -1;
		break;
	}
	if ((size_t) byteStreamLen != fg.getBuffer()->getDataSize()) {
		isSame = 0;
		break;
	}
	isSame = memcmp(fg.getBuffer()->getBuf(), byteStreamPtr, byteStreamLen) == 0;
} while(0);
	(void) printf("isExist? |%s|, isSame? |%s|\n", isExist ? "True" : "False", isExist ? (isSame ? "True" : "False") : "N/A");
	if (isExist) {
		if (!isSame) {
			// 다른 이름으로 저장한다.
			(void) snprintf(strLibSavedPathname, 256, "%s/lib%s_XXXXXX", CPythonEngine::WORKER_MODULE_PATH, strEntryPoint);
			int	fd = mkstemp(strLibSavedPathname);
			if (fd < 0) {
				perror("mkstemp");
				iReturnVal = -1;
				break;
			}
			if (write(fd, byteStreamPtr, byteStreamLen) != byteStreamLen) {
				perror("write");
				return -1;
			}
			if (close(fd) < 0) {
				perror("close");
				return -1;
			}
		}
	}

if (!isExist) {
	if ((f = fopen(strLibSavedPathname, "wb")) == NULL) {
		perror(strLibSavedPathname);
		iReturnVal = -1;
		break;
	}
	// 파일에 쓴다.
	if (fwrite(byteStreamPtr, 1, byteStreamLen, f) != (size_t) byteStreamLen) {
		perror("fwrite");
		iReturnVal = -1;
		break;
	}
	// 파일을 닫는다.
	if (fclose(f) != 0) {
		f = NULL;
		perror("fclose");
		iReturnVal = -1;
		break;
	}
	f = NULL;
}

	libSavedPathname = PyString_FromString(strLibSavedPathname); assert(libSavedPathname);
	(void) printf("strLibSavedPathname:= |%s|\n", strLibSavedPathname);
	// 파일에 실행모드를 설정한다.
	// 어떤 SYSTEM(HP, Cyswin) 에서는 실행모드가 설정되어있어야 한다.
	if (chmod(strLibSavedPathname, S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) < 0) {
		perror("chmod");
		iReturnVal = -1;
		break;
	}
////////////////////////////////////////////////////////////////////////

	// 저장한 파일이름을 기억한다.
	if (PyDict_SetItemString(hByteStream, BYTE_STREAM__KEY_NAME__LIB_SAVED_PATHNAME, libSavedPathname) != 0) {
		(void) fprintf(stderr,
			"ERROR: "
			"__FILE__:= |%s|, __LINE_NO__ = %d: "
			"CWorker::saveByteStream: "
			"PyDict_SetItemString(|"
			,
			__FILE__, __LINE__
		); (void) PyObject_Print(hByteStream, stderr, 0);
		(void) fprintf(stderr, "|, |%s|): Failure\n", BYTE_STREAM__KEY_NAME__LIB_SAVED_PATHNAME);
		assert(0);
		iReturnVal = -1;
		break;
	}

	// Byte Stream 자체의 내용을 파일로 내려받았으므로 따로 저장할 필요가 없다.
	// 단지 MD5 값만을 기억해서 나중에 같은 이름의 Byte Stream 을 내려받았을 때,
	// 덮어쓸 것인지를 결정한다.

	if (PyDict_SetItem(this->m__hByteStreamByID, byteStreamID, hByteStream) != 0) {
		if (PyErr_Occurred())
			PyErr_Print();
		assert(0);
		iReturnVal = -1;
		break;
	}
} while (0);
	if (f) {
		(void) fclose(f);
		f = NULL;
	}
	Py_XDECREF(libSavedPathname); libSavedPathname = NULL;

	return iReturnVal;
}

// hCommand 의 COMMAND__KEY_NAME__BYTE_STREAM_ID__LINK 이 가리키는
// Byte Stream 을 사용하여 Worker 쓰레드를 만든다.
int
CWorker::__pyCreateWorkerThread(
	PyObject*	commandID,
	PyObject*	hCommand,
	int		overwrite
)
{
	assert(this->m__hByteStreamByID);
	assert(this->m__hCommandByID);

	int	iReturnVal = 0;

	PyObject*	status = NULL;
	PyObject*	control = NULL;
do {
////////////////////////////////////////////////////////////////////////
if (!overwrite) {
	// this->m__hWorkerArgByID 에 등록된 Command 가 있는 지 확인한다.
	PyErr_Clear();
	PyObject*	prevWorkerArg = PyDict_GetItem(this->m__hWorkerArgByID, commandID);
	if (prevWorkerArg) {
		eprintf(E_WARN, EFMT("Command(|%O|) is running with CWorkerArg(|%O|): Stop it first"), commandID, prevWorkerArg);
		(void) fprintf(stderr, "|: Stop it first!\n");

		iReturnVal = 0;
		break;
	}
	if (PyErr_Occurred()) {
		eprintf(E_ERR, EFMT("PyDict_GetItem(|%O|, |%O|): Failure"), this->m__hWorkerArgByID, commandID);

		iReturnVal = -1;
		break;
	}
	// this->m__hCommandByID 에 등록된 Command 가 있는 지 확인한다.
	PyErr_Clear();
	PyObject*	hPrevCommand = PyDict_GetItem(this->m__hCommandByID, commandID);
	if (hPrevCommand) {
		eprintf(E_WARN, EFMT("Command(|%d|) is running as |%O|: Stop it first"), commandID, hPrevCommand);
		(void) fprintf(stderr, "|: Stop it first!\n");

		iReturnVal = 0;
		break;
	}
	if (PyErr_Occurred()) {
		eprintf(E_ERR, EFMT("PyDict_GetItem(|%O|, |%O|): Failure"), this->m__hCommandByID, commandID);

		iReturnVal = -1;
		break;
	}
}
////////////////////////////////////////////////////////////////////////

	// Worker 쓰레드에 넘겨줄 CWorkerArg 를 만든다.
	// hCommand['ByteStreamID__LINK] 에 해당하는 hByteStream 로부터
	// "Entry Point" 와 "libSavedPathname" 을 얻는다.
	PyObject*	byteStreamID = PyDict_GetItemString(hCommand, COMMAND__KEY_NAME__BYTE_STREAM_ID__LINK);	assert(byteStreamID);
	PyObject*	hByteStream = PyDict_GetItem(this->m__hByteStreamByID, byteStreamID); assert(hByteStream);
	PyObject*	entryPoint = PyDict_GetItemString(hByteStream, CJobDown::BYTE_STREAM__KEY_NAME__ENTRY_POINT); assert(entryPoint);
	const char*	strEntryPoint = PyString_AsString(entryPoint); assert(strEntryPoint);
	PyObject*	libSavedPathname = PyDict_GetItemString(hByteStream, BYTE_STREAM__KEY_NAME__LIB_SAVED_PATHNAME);
	const char*	strLibSavedPathname = PyString_AsString(libSavedPathname);
	CWorkerModule*	pWorkerModule = new CWorkerModule(strEntryPoint, strLibSavedPathname);
	if (!pWorkerModule || pWorkerModule->getHandle() == NULL) {
		(void) fprintf(stderr,
			"ERROR: "
			"__FILE__:= |%s|, __LINE_NO__ = %d: "
			"CWorker::__pyCreateWorkerThread: "
			"new CWorkerModule(|%s|, |%s|): Failure\n"
			,
			__FILE__, __LINE__, strEntryPoint, strLibSavedPathname
		);

		iReturnVal = -1;
		break;
	}
	CWorkerArg*	pWorkerArg = new CWorkerArg();
	pWorkerArg->setWorker(this);
	pWorkerArg->hCommand = hCommand;
	pWorkerArg->setWorkerModule(pWorkerModule);
#if 1
	if (pthread_create(
		&pWorkerArg->id,
		NULL,
		(void* (*)(void*)) pWorkerModule->getEntryPoint(),
		(void*) pWorkerArg
	) != 0) {
		(void) fprintf(stderr,
			"ERROR: "
			"__FILE__:= |%s|, __LINE_NO__ = %d: "
			"CWorker::__pyCreateWorkerThread: "
			"pthread_create: Failure\n"
			,
			__FILE__, __LINE__
		);

		iReturnVal = -1;
		break;
	}
#endif
	long	stat = CMD_STAT__RUNNING;
	status = PyInt_FromLong(stat); assert(status);		// New ref.
	assert(PyDict_SetItemString(hCommand, COMMAND__KEY_NAME__STATUS, status) == 0);
	long	ctrl = CMD_CTRL__NONE;
	control = PyInt_FromLong(ctrl); assert(control);	// New ref.
	assert(PyDict_SetItemString(hCommand, COMMAND__KEY_NAME__CONTROL, control) == 0);

	// m__hWorkerArgByID 에 등록
	PyObject*	workerArg = PyLong_FromVoidPtr(pWorkerArg); assert(workerArg);	// New ref.
	if (PyDict_SetItem(this->m__hWorkerArgByID, commandID, workerArg) != 0) {
		if (PyErr_Occurred())
			PyErr_Print();
		assert(0);
		Py_DECREF(workerArg);
		iReturnVal = -1;
		break;
	}
	Py_DECREF(workerArg);
////////////////////////////////////////////////////////////////////////

	// 사용하는 Byte Stream 의 BYTE_STREAM__KEY_NAME__COMMAND_ID__SET 에 현재 Command ID 를 등록한다.
	PyObject*	commandID__SET = PyDict_GetItemString(hByteStream, BYTE_STREAM__KEY_NAME__COMMAND_ID__SET);
	if (!commandID__SET) {
		eprintf(E_ERR, EFMT("PyDict_GetItemString(|%O|, |%s|): Failure"), hByteStream, BYTE_STREAM__KEY_NAME__COMMAND_ID__SET);

		iReturnVal = -1;
		break;
	}
	if (PySet_Add(commandID__SET, commandID) != 0) {
		eprintf(E_ERR, EFMT("PySet_Add(|%O|, |%u|)"), commandID__SET, commandID);

		iReturnVal = -1;
		break;
	}

	if (PyDict_SetItem(this->m__hCommandByID, commandID, hCommand) != 0) {
		if (PyErr_Occurred())
			PyErr_Print();
		assert(0);
		iReturnVal = -1;
		break;
	}
} while (0);
	Py_XDECREF(control); control = NULL;
	Py_XDECREF(status); status = NULL;

	return iReturnVal;
}
int
CWorker::__pySetCommand(
	PyObject*	commandID,
	const CBuffer&	commandInfo
)
{
	assert(this->m__hByteStreamByID);
	assert(this->m__hCommandByID);
	assert(commandInfo.getDataSize());

	int	iReturnVal = 0;

	PyObject*	hCommand = NULL;
do {
	hCommand = (PyObject*) this->__pySetObjectFromString(commandInfo.getBuf());	// New ref.
	if (!hCommand) {
		assert(hCommand);
		iReturnVal = -1;
		break;
	}

	int	overwriteFalse = 0;
	if (this->__pyCreateWorkerThread(commandID, hCommand, overwriteFalse) < 0) {
		eprintf(E_ERR, EFMT("CWorker::createWorkerThread(|%O|, |%O|): Failure"), commandID, hCommand);

		iReturnVal = -1;
		break;
	}
	iReturnVal = 0;
} while (0);
	Py_XDECREF(hCommand); hCommand = NULL;

	return iReturnVal;
}

int
CWorker::__pySetJob(
	PyObject*	jobID,
	const CBuffer&	jobInfo
)
{
	assert(this->m__hCommandByID);
	assert(jobInfo.getDataSize());

	int	iReturnVal = 0;

	PyObject*	hJob = NULL;
do {
	hJob = (PyObject*) this->__pySetObjectFromString(jobInfo.getBuf());	// New ref.
	if (!hJob) {
		assert(hJob);
		iReturnVal = -1;
		break;
	}

	// 이 Job 을 처리하는 Command 을 찾아 거기에 이 Job 을 등록한다.
	PyObject*	commandID = PyDict_GetItemString(hJob, JOB__KEY_NAME__COMMAND_ID__LINK); assert(commandID);
	PyObject*	hCommand = PyDict_GetItem(this->m__hCommandByID, commandID); assert(hCommand);
	PyObject*	hCommandJob = PyDict_GetItemString(hCommand, COMMAND__KEY_NAME__JOB); assert(hJob);
	// 해당 Job 이 이미 등록되었는 지 확인한다.
	PyErr_Clear();
	PyObject*	hPrevJob = PyDict_GetItem(hCommandJob, jobID);
	if (hPrevJob) {	// 같은 Job 이 등록되었다.
		eprintf(E_WARN, EFMT("Job(|%O|): Already registered in |%O|: Stop it first"), jobID, hCommandJob);

		iReturnVal = 0;
		break;
	}
	if (PyErr_Occurred()) {
		eprintf(E_ERR, EFMT("PyDict_GetItem(|%O|, |%O|): Failure"), hCommandJob, jobID);

		iReturnVal = -1;
		break;
	}
	if (PyDict_SetItem(hCommandJob, jobID, hJob) != 0) {
		eprintf(E_ERR, EFMT("PyDict_SetItem(|%O|, |%O|, |%O|): Failure"), hCommandJob, jobID, hJob);

		iReturnVal = -1;
		break;
	}
} while (0);
	Py_XDECREF(hJob); hJob = NULL;

	return iReturnVal;
}

int
CWorker::pySaveAll(
	void
)
{
	struct hXxxByID_t {
		const char*	pathname;
		PyObject*	ref;
	} hXxxByID[] = {
		{"./.m__hScriptByID", this->m__hScriptByID},
		{"./.m__hByteStreamByID", this->m__hByteStreamByID},
		{"./.m__hCommandByID", this->m__hCommandByID}
	};

	int	iReturnVal = 0;
	PyGILState_STATE	gstate = PyGILState_Ensure();

	for (unsigned int i = 0; i < (sizeof(hXxxByID) / sizeof(hXxxByID_t)); ++i) {
		FILE*	f = NULL;
		f = fopen(hXxxByID[i].pathname, "w");
		if (!f) {
			iReturnVal = -1;
			break;
		}
		(void) PyObject_Print(hXxxByID[i].ref, f, 0);
		(void) fprintf(f, "\n");
		if (f) {
			(void) fclose(f);
			f = NULL;
		}
	}

	PyGILState_Release(gstate);
	return iReturnVal;
}

int
CWorker::update(
	const CBuffer&	newAgent,
	UPDATE_AGENT_T&	ua
)
{
	int	fd = mkstemp(ua.agentCopy);
	if (fd < 0) {
		perror("mkstemp");
		return -1;
	}
	if (write(fd, newAgent.getBuf(), newAgent.getDataSize()) != (ssize_t) newAgent.getDataSize()) {
		perror("write");
		return -1;
	}
	if (close(fd) < 0) {
		perror("close");
		return -1;
	}

	if (chmod(ua.agentCopy, S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) < 0) {
		perror("chmod");
		return -1;
	}

	return 0;
}

int
CWorker::pyReload(
	void
)
{
	struct hXxxByID_t {
		const char*	pathname;
		const char*	varName;
		PyObject**	refPtr;
	} hXxxByID[] = {
		{"./.m__hScriptByID", SCRIPT_BY_ID__VAR_NAME, &this->m__hScriptByID},
		{"./.m__hByteStreamByID", BYTE_STREAM_BY_ID__VAR_NAME, &this->m__hByteStreamByID},
		{"./.m__hCommandByID", COMMAND_BY_ID__VAR_NAME, &this->m__hCommandByID}
	};

	int	iReturnVal = 0;
	PyGILState_STATE	gstate = PyGILState_Ensure();

	// 변수들을 읽어들인다.
	for (unsigned int i = 0; i < (sizeof(hXxxByID) / sizeof(hXxxByID_t)); ++i) {
		CFile	file;
		int	iPipeFalse = 0;
		assert(file.openFile(hXxxByID[i].pathname, "r", iPipeFalse) == 0);
		assert(file.readlines() >= 0);
		const CBuffer&	buffer = *(file.getBuffer());
#if 0
		(void) printf("////////\n");
		(void) printf("|%s|(%d)", buffer.getBuf(), buffer.getDataSize());
		(void) printf("////////\n");
#endif
		PyObject*	ref = (PyObject*) this->__pySetObjectFromString(buffer.getBuf()); assert(ref);	// New ref.
		assert(PyDict_SetItemString(this->m__globalDict, hXxxByID[i].varName, ref) == 0);
		Py_DECREF(ref);
		*hXxxByID[i].refPtr = ref;
	}

	// Script 를 "TypeBlob" 에 따라 처리한다.
	PyObject*	scriptID;
	PyObject*	hScript;
	Py_ssize_t	pos = 0;
	while (PyDict_Next(this->m__hScriptByID, &pos, &scriptID, &hScript)) {
		assert(PyDict_SetItemString(hScript, "EvalDone", Py_False) == 0);
	}
	this->__pyEvalAll(this->m__hScriptByID);

	// 원래의 명령어를 실행한다.
	PyObject*	commandID;
	PyObject*	hCommand;
	pos = 0;
	int	overwriteTrue = 1;
	while (PyDict_Next(this->m__hCommandByID, &pos, &commandID, &hCommand)) {
		if (this->__pyCreateWorkerThread(commandID, hCommand, overwriteTrue) < 0) {
			eprintf(E_ERR, EFMT("CWorker::createWorkerThread(|%O|, |%O|): Failure"), commandID, hCommand);
		}
	}

	PyGILState_Release(gstate);
	return iReturnVal;
}

#if 1
int
CWorker::pyGetSet(
	CBuffer&	setInfo
)
{
	int	iReturnVal = 0;
	PyGILState_STATE	gstate = PyGILState_Ensure();

	PyObject*	xxx__DICT = NULL;
	PyObject*	xxx__DICT_Str = NULL;
	PyObject*	scriptID__LIST = NULL;
	PyObject*	scriptID__SET = NULL;
	PyObject*	byteStreamID__LIST = NULL;
	PyObject*	byteStreamID__SET = NULL;
	PyObject*	commandID__LIST = NULL;
	PyObject*	commandID__SET = NULL;
do {
	scriptID__LIST = PyDict_Keys(this->m__hScriptByID); assert(scriptID__LIST);
	scriptID__SET = PySet_New(scriptID__LIST); assert(scriptID__SET);
	byteStreamID__LIST = PyDict_Keys(this->m__hByteStreamByID); assert(byteStreamID__LIST);
	byteStreamID__SET = PySet_New(byteStreamID__LIST); assert(byteStreamID__SET);
	commandID__LIST = PyDict_Keys(this->m__hCommandByID); assert(commandID__LIST);
	commandID__SET = PySet_New(commandID__LIST);
	xxx__DICT = Py_BuildValue(
		"{s: O, s: O, s: O}"
		,
		"ScriptID__SET", scriptID__SET,
		"ByteStreamID__SET", byteStreamID__SET,
		"CommandID__SET", commandID__SET
	); assert(xxx__DICT);
	xxx__DICT_Str = PyObject_Str(xxx__DICT); assert(xxx__DICT_Str);
	char*		buffer;
	Py_ssize_t	length;
	if (PyString_AsStringAndSize(xxx__DICT_Str, &buffer, &length) < 0) {
		eprintf(E_ERR, EFMT("PyString_AsStringAndSize(|%O|): Failure"), xxx__DICT_Str);
		iReturnVal = -1;
		break;
	}

	setInfo.setDataSize(0);
	if (!setInfo.setBufSizeAtLeast(length + 1)) {
		eprintf(E_ERR, EFMT("CBuffer::setBufSizeAtLeast(%d)"), length + 1);
		iReturnVal = -1;
		break;
	}
	(void*) memcpy(setInfo.getBuf(), buffer, length + 1);
	setInfo.setDataSize(length + 1);
	iReturnVal = 0;
} while (0);
	Py_XDECREF(xxx__DICT); xxx__DICT = NULL;
	Py_XDECREF(xxx__DICT_Str); xxx__DICT_Str = NULL;
	Py_XDECREF(scriptID__LIST); scriptID__LIST = NULL;
	Py_XDECREF(scriptID__SET); scriptID__SET = NULL;
	Py_XDECREF(byteStreamID__LIST); byteStreamID__LIST = NULL;
	Py_XDECREF(byteStreamID__SET); byteStreamID__SET = NULL;
	Py_XDECREF(commandID__LIST); commandID__LIST = NULL;
	Py_XDECREF(commandID__SET); commandID__SET = NULL;

	PyGILState_Release(gstate);
	return iReturnVal;
}
#else
int
CWorker::restart(
	UPDATE_AGENT_T&	ua
)
{
	return 0;
}
#endif

int
CWorker::setSockfd(
	void
)
{
	int	sockfd = 0;

do {
	sockfd = socket(PF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		eprintf(E_ERR, EFMT("socket(PF_INET, SOCK_STREAM, 0): Failure: %m"));
		assert(0);
	}

	struct sockaddr_in	peerSockaddr;
	(void*) memset(&peerSockaddr, 0, sizeof(peerSockaddr));
	peerSockaddr.sin_family = AF_INET;
	peerSockaddr.sin_port = htons(2008);
//	peerSockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
#if 1
#if 1
	if (!inet_aton("127.0.0.1", &peerSockaddr.sin_addr)) {
		eprintf(E_ERR, EFMT("inet_aton(|%s|): Failure"), "localhost");
		assert(0);
	}
#else
	peerSockaddr.sin_addr = inet_addr("localhost");
#endif
#else
	if (inet_pton(AF_INET, "localhost", (void*) &peerSockaddr.sin_addr) < 0) {
		eprintf(E_ERR, EFMT("inet_pton(|%s|): Failure: %m"), "localhost");
		assert(0);
	}
#endif
	if (connect(sockfd, (struct sockaddr*) &peerSockaddr, sizeof(peerSockaddr)) < 0) {
		eprintf(E_ERR, EFMT("connect(|%s|, #%d): Failure: %m"), "localhost", 2008);
		assert(0);
	}
} while (0);

	return sockfd;
}
