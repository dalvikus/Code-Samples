#include "CMgmtSrv.h"
#include "CFile.h"
#include "eprintf.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>	// fprintf
#include "CSocket.h"

// setScriptTree.py 의 있는 변수의 이름과 일치해야 한다.
const char*	CMgmtSrv::SCRIPT2_BY_ID			= "hScript2ByID";
const char*	CMgmtSrv::SCRIPT2_BY_NAME		= "hScript2ByNAME";
const char*	CMgmtSrv::BYTE_STREAM2_BY_ID		= "hByteStream2ByID";
const char*	CMgmtSrv::BYTE_STREAM2_BY_NAME		= "hByteStream2ByName";
const char*	CMgmtSrv::COMMAND2_BY_ID		= "hCommand2ByID";
const char*	CMgmtSrv::COMMAND2_BY_NAME		= "hCommand2ByName";
const char*	CMgmtSrv::JOB2_BY_ID			= "hJob2ByID";
const char*	CMgmtSrv::JOB2_BY_NAME			= "hJob2ByName";

const char*	CMgmtSrv::ACCEPT_SOCKET__KEY_NAME_ID_STR	= "ID Str";
const char*	CMgmtSrv::ACCEPT_SOCKET__KEY_NAME_CTRL_SOCKFD	= "Control Sockfd";
const char*	CMgmtSrv::ACCEPT_SOCKET__KEY_NAME_DATA_SOCKFD	= "Data Sockfd";

CMgmtSrv::CMgmtSrv(
	void
)
{
	this->m__listenSockfd = socket(PF_INET, SOCK_STREAM, 0);
	if (this->m__listenSockfd < 0) {
		eprintf(E_ERR, EFMT("socket(PF_INET, SOCK_STREAM, 0): Failure: %m"));
		assert(0);
	}
	int	reusePortTrue = 1;
	if (setsockopt(this->m__listenSockfd, SOL_SOCKET, SO_REUSEADDR, (const void*) &reusePortTrue, sizeof(reusePortTrue)) < 0) {
		eprintf(E_ERR, EFMT("setsockopt(SOL_SOCKET, SO_REUSEADDR, True): Failure: %m"));
		assert(0);
	}

	struct sockaddr_in	listenSockaddr;
	(void*) memset(&listenSockaddr, 0, sizeof(listenSockaddr));
	listenSockaddr.sin_family = AF_INET;
	listenSockaddr.sin_port = htons(2008);
	listenSockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(this->m__listenSockfd, (struct sockaddr*) &listenSockaddr, sizeof(listenSockaddr)) < 0) {
		eprintf(E_ERR, EFMT("bind(struct sockaddr): Failure: %m"));
		assert(0);
	}

	if (listen(this->m__listenSockfd, 10) < 0) {
		eprintf(E_ERR, EFMT("listen(..., %d): Failure: %m"), 10);
		assert(0);
	}

	PyGILState_STATE	gstate = PyGILState_Ensure();
	this->m__acceptSockfd__DICT = PyDict_New(); assert(this->m__acceptSockfd__DICT);
	PyGILState_Release(gstate);

	for (int i = 0; i < MAX_AGENT_THREAD; ++i)
		this->m__thread[i] = NULL;
	this->m__indexOfNextThread = 0;
	this->m__queueDatabaseThread = NULL;
}

CMgmtSrv::~CMgmtSrv()
{
	for (int i = 0; i < MAX_AGENT_THREAD; ++i) {
		if (this->m__thread[i]) {
			delete this->m__thread[i];
			this->m__thread[i] = NULL;
		}
	}

	if (this->m__acceptSockfd__DICT) {
		PyObject*	keyIndex;
		PyObject*	valAgentSockInfo;
		Py_ssize_t	pos = 0;
		PyGILState_STATE	gstate = PyGILState_Ensure();
		while (PyDict_Next(this->m__acceptSockfd__DICT, &pos, &keyIndex, &valAgentSockInfo)) {
			PyErr_Clear();
			int	index = (int) PyInt_AsLong(keyIndex);
			if (PyErr_Occurred()) {
				eprintf(E_FATAL, EFMT("PyInt_AsLong(|%O|): Failure"), keyIndex);
				continue;
			}
			PyGILState_Release(gstate);

			int	iThread = index >> 16;
			if (iThread < 0 || iThread >= MAX_AGENT_THREAD) {
				eprintf(E_ERR, EFMT("(index = %d) iThread = %d: Out of range(0 <= iThread < %u)"), index, iThread, MAX_AGENT_THREAD);
				continue;
			}
			CAgentThread*	thread = this->m__thread[iThread];
			if (!thread)
				continue;
			delete thread;
			this->m__thread[iThread] = NULL;

			gstate = PyGILState_Ensure();
		}
		PyGILState_Release(gstate);

		this->m__acceptSockfd__DICT = NULL;
	}

	if (close(this->m__listenSockfd) < 0) {
		eprintf(E_ERR, EFMT("close(this->m__listenSockfd): Failure"));
		assert(0);
	}
}

void
CMgmtSrv::setCtrlSockfd(
	const char*	id_str,
	int	ctrlSockfd,
	int&	indexThread,
	int&	indexSlot
)
{
	if (this->m__indexOfNextThread == -1) {
		// 사용되던 slot 이 반환되었을 수도 있다.
		for (int i = 0; i < MAX_AGENT_THREAD; ++i) {
			if (this->m__thread[i]->getIndexOfNextSlot() != -1) {
				this->m__indexOfNextThread = i;
				break;
			}
		}
		if (this->m__indexOfNextThread == -1) {	// 여전히 없다.
			indexThread = indexSlot = -1;
			return;
		}
	}

	indexThread = this->m__indexOfNextThread;
	if (this->m__thread[indexThread] == NULL) {
		this->m__thread[indexThread] = new CAgentThread();
		if (indexThread % QUEUE_THREAD_UNIT)
			this->m__thread[indexThread]->setQueueDatabaseThread(this->m__queueDatabaseThread);
		else
			this->m__queueDatabaseThread = this->m__thread[indexThread]->setQueueDatabaseThread(DATABASE_QUEUE_SIZE);
	}
	CAgentThread&	thread = *(this->m__thread[indexThread]);
	thread.setCtrlSockfd(id_str, ctrlSockfd, indexSlot);

	while (1) {
		++this->m__indexOfNextThread;
		if (this->m__indexOfNextThread == indexThread) {	// 한 바퀴 돌았다.
			// 더 이상 가능한 thread 가 없다.
			this->m__indexOfNextThread = -1;
			break;
		}
		if (this->m__indexOfNextThread == MAX_AGENT_THREAD)
			this->m__indexOfNextThread = 0;
		if (this->m__thread[this->m__indexOfNextThread] == NULL || this->m__thread[this->m__indexOfNextThread]->getIndexOfNextSlot() != -1) {	// 다음에는 이 thread 를 이용한다.
			break;
		}
	}
}

int
CMgmtSrv::setDataSockfd(
	int	dataSockfd,
	int	indexThread,
	int	indexSlot
)
{
	if (indexThread < 0 || indexThread >= MAX_AGENT_THREAD) {
		eprintf(E_ERR, EFMT("indexThread = %d: Out of range"), indexThread);
		return -1;
	}

	if (this->m__thread[indexThread] == NULL) {
		eprintf(E_ERR, EFMT("thread[%d] is NULL"), indexThread);
		return -1;
	}

	return this->m__thread[indexThread]->setDataSockfd(dataSockfd, indexSlot);
}

PyObject*
CMgmtSrv::__pyFindSocket(
	const char*	id_cstr		// |Host|.|Port|
)
{
	PyObject*	hSocketByFD = 0;
	PyGILState_STATE	gstate = PyGILState_Ensure();

	PyObject*	keyIndex;
	PyObject*	valAgentSockInfo;
	Py_ssize_t	pos = 0;
	while (PyDict_Next(this->m__acceptSockfd__DICT, &pos, &keyIndex, &valAgentSockInfo)) {
		PyObject*	IDStr = PyDict_GetItemString(valAgentSockInfo, ACCEPT_SOCKET__KEY_NAME_ID_STR); assert(IDStr);
		const char*	IDStr_cstr = PyString_AsString(IDStr); assert(IDStr_cstr);
		if (strcmp(IDStr_cstr, id_cstr) == 0) {
			hSocketByFD = valAgentSockInfo;
			break;
		}
	}

	PyGILState_Release(gstate);
	return hSocketByFD;
}

int
CMgmtSrv::pyAcceptAgent(
	void
)
{
	struct sockaddr_in	acceptSockaddr;
	(void*) memset(&acceptSockaddr, 0, sizeof(acceptSockaddr));
	socklen_t	addrlen = sizeof(acceptSockaddr);
	int	acceptSockfd = accept(this->m__listenSockfd, (struct sockaddr*) &acceptSockaddr, &addrlen);
	if (acceptSockfd < 0) {
		eprintf(E_ERR, EFMT("accept(...): Failure: %m"));
		assert(0);
	}

	CPacket::RESP_TYPE_T	sockType;
	unsigned	index;
	if (CSocket::recvSockID(acceptSockfd, sockType, index) < 0) {
		eprintf(E_ERR, EFMT("CSocket::recvSockID(): Failure"));
		return -1;
	}

	uint16_t	clientPort = ntohs(acceptSockaddr.sin_port);
	struct hostent	*clientHostent = gethostbyaddr((const char*) &acceptSockaddr.sin_addr, sizeof(acceptSockaddr.sin_addr), AF_INET);
	char	id_str[128];
	(void) snprintf(id_str, 128, "|%s|:|%d|", clientHostent->h_name, clientPort);
	(void) printf("Agent: %s (%s)\n", id_str, sockType == CPacket::RESP_TYPE__CTRL_SOCKET ? "Control" : "Data");
	assert(sockType == CPacket::RESP_TYPE__CTRL_SOCKET || sockType == CPacket::RESP_TYPE__DATA_SOCKET);

	if (sockType == CPacket::RESP_TYPE__DATA_SOCKET) {
		int	indexThread = index >> 16;
		int	indexSlot = index & 0xffff;
		(void) printf("(indexThread, indexSlot) = (%d, %d)\n", indexThread, indexSlot);
		if (this->setDataSockfd(acceptSockfd, indexThread, indexSlot) < 0) {
			eprintf(E_ERR, EFMT("CMgmtSrv::setDataSockfd(): Failure"));
			return -1;
		}

		return 0;
	}

	// CPacket::RESP_TYPE__CTRL_SOCKET
	int	indexThread;
	int	indexSlot;
	this->setCtrlSockfd(id_str, acceptSockfd, indexThread, indexSlot);
	if (indexThread < 0) {
		eprintf(E_ERR, EFMT("indexThread = -1"));
		return -1;
	}
	(void) printf("(indexThread, indexSlot) = (%d, %d)\n", indexThread, indexSlot);
	index = (indexThread << 16) | indexSlot;
	if (CSocket::sendSockID(acceptSockfd, sockType, index) < 0) {
		eprintf(E_ERR, EFMT("CSocket::sendSockID(): Failure"));
		return -1;
	}

	PyObject*	indexObj = NULL;
	PyObject*	hSocketByFD = NULL;
	PyGILState_STATE	gstate = PyGILState_Ensure();
do {
	indexObj = PyInt_FromLong((long) index); assert(indexObj);
	hSocketByFD = Py_BuildValue(
		"{s: s, s: i, s: i}"
		,
		ACCEPT_SOCKET__KEY_NAME_ID_STR, id_str,
		ACCEPT_SOCKET__KEY_NAME_CTRL_SOCKFD, acceptSockfd,
		ACCEPT_SOCKET__KEY_NAME_DATA_SOCKFD, -1
	); assert(hSocketByFD);
	assert(PyDict_SetItem(this->m__acceptSockfd__DICT, indexObj, hSocketByFD) == 0);
} while (0);
	Py_XDECREF(indexObj); indexObj = NULL;
	Py_XDECREF(hSocketByFD); hSocketByFD = NULL;
	PyGILState_Release(gstate);

	return 0;
}

void
CMgmtSrv::pyListAgent(
	void
)
{
	if (!this->m__acceptSockfd__DICT) {
		eprintf(E_FATAL, EFMT("CMgmtSrv::listAgents: m__acceptSockfd__DICT = NULL"));

		return;
	}

	PyObject*	keyIndex;
	PyObject*	valAgentSockInfo;
	Py_ssize_t	pos = 0;
	PyGILState_STATE	gstate = PyGILState_Ensure();
	while (PyDict_Next(this->m__acceptSockfd__DICT, &pos, &keyIndex, &valAgentSockInfo)) {
		(void) printf("sockfd = |");
		(void) PyObject_Print(keyIndex, stdout, 0);
		(void) printf("| -> agent = |");
		(void) PyObject_Print(valAgentSockInfo, stdout, 0);
		(void) printf("|\n");

#if 0
		PyObject*	agent = PyDict_GetItemString(valAgentSockInfo, ACCEPT_SOCKET__KEY_NAME_AGENT_PTR); assert(agent);
		CAgent*	agentPtr = (CAgent*) PyLong_AsVoidPtr(agent); assert(agentPtr);

		(void) printf("\tOrder__LIST:= |");
		(void) PyObject_Print(agentPtr->getOrder__LIST(), stdout, 0);
		(void) printf("|\n");
		(void) printf("\tScriptID__SET:= |");
		(void) PyObject_Print(agentPtr->getScriptID__SET(), stdout, 0);
		(void) printf("|\n");
		(void) printf("\tByteStreamID__SET:= |");
		(void) PyObject_Print(agentPtr->getByteStreamID__SET(), stdout, 0);
		(void) printf("|\n");
		(void) printf("\tCommandID__SET:= |");
		(void) PyObject_Print(agentPtr->getCommandID__SET(), stdout, 0);
		(void) printf("|\n");
#endif
	}
	PyGILState_Release(gstate);
}

int
CMgmtSrv::pyCheckAgent(
	void
)
{
	if (!this->m__acceptSockfd__DICT) {
		eprintf(E_FATAL, EFMT("CMgmtSrv::listAgents: m__acceptSockfd__DICT = NULL"));

		return -1;
	}

#if 1
	return 0;
#else
	int	iReturnVal = 0;
	PyGILState_STATE	gstate = PyGILState_Ensure();

	PyObject*	keys = NULL;
do {
	keys = PyDict_Keys(this->m__acceptSockfd__DICT); assert(keys);
	PyErr_Clear();
	Py_ssize_t	size = PyList_Size(keys); assert(!PyErr_Occurred());
	for (Py_ssize_t i = 0; i < size; ++i) {
		PyObject*	key = PyList_GetItem(keys, i); assert(key);
		PyObject*	val = PyDict_GetItem(this->m__acceptSockfd__DICT, key); assert(val);
		PyObject*	agent = PyDict_GetItemString(val, ACCEPT_SOCKET__KEY_NAME_AGENT_PTR); assert(agent);
		CAgent*	agentPtr = (CAgent*) PyLong_AsVoidPtr(agent); assert(agentPtr);
		CAgent::THREAD_FLAG_T	flag = agentPtr->getFlag();
		if (flag == CAgent::THREAD_FLAG__TERMINATE) {
			int	ctrlSockfd = agentPtr->getCtrlSockfd();
			(void) printf("Agent # = %d: Terminated\n", ctrlSockfd);
			PyGILState_Release(gstate);
			delete agentPtr;
			gstate = PyGILState_Ensure();
			assert(shutdown(ctrlSockfd, SHUT_RDWR) == 0);
			assert(close(ctrlSockfd) == 0);
			int	dataSockfd = agentPtr->getDataSockfd();
			if (dataSockfd != -1) {
				assert(shutdown(dataSockfd, SHUT_RDWR) == 0);
				assert(close(dataSockfd) == 0);
			}
			assert(PyDict_DelItem(this->m__acceptSockfd__DICT, key) == 0);
		}
	}
} while (0);
	Py_XDECREF(keys); keys = NULL;

	PyGILState_Release(gstate);
	return iReturnVal;
#endif
}

int
CMgmtSrv::enqueueOrder(
	int	index,
	PyObject*	order
)
{
	int	iReturnVal = 0;

do {
	int	iThread = index >> 16;
	if (iThread < 0 || iThread >= MAX_AGENT_THREAD) {
		eprintf(E_ERR, EFMT("(index = %d) iThread = %d: Out of range(0 <= iThread < %u)"), index, iThread, MAX_AGENT_THREAD);
		iReturnVal = -1;
		break;
	}
	CAgentThread*	thread = this->m__thread[iThread];
	if (!thread) {
		eprintf(E_ERR, EFMT("this->m__thread[iThread = %d] is NULL"), iThread);
		iReturnVal = -1;
		break;
	}
	int	iSlot = index & 0xffff;
	if (iSlot < 0 || iSlot >= CAgentThread::MAX_AGENT_SLOT) {
		eprintf(E_ERR, EFMT("(index = %d) iSlot = %d: Out of range(0 <= iSlot < %u)"), index, iSlot, CAgentThread::MAX_AGENT_SLOT);
		iReturnVal = -1;
		break;
	}
	CAgentSlot*	slot = thread->slots()[iSlot];
	if (!slot) {
		eprintf(E_ERR, EFMT("thread->slots()[iSlot = %d] is NULL"), iSlot);
		iReturnVal = -1;
		break;
	}
	iReturnVal = slot->pyEnqueOrder(order);
} while (0);

	return iReturnVal;
}


void
CMgmtSrv::help(
	void
)
{
	const char*	commandList[] = {
		"HELP",
		"LOAD__SCRIPT_FUNCTION |script_func| |script_func.py| [Overwrite]",
		"LOAD__INSTALLER |installerName| |installerArgument.py|",

		"EVAL__ALL",

		"LIST__JOB",
		"LIST__COMMAND",
		"LIST__BYTE_STREAM",

		"ENQUE__JOB #jobID",
		"DO__JOB_DOWN",

		"LIST__AGENT",

		NULL
	};

	char**	pCommand = const_cast<char**>(commandList);
	while (*pCommand) {
		(void) printf("%s\n", *pCommand);

		++pCommand;
	}
}

void
CMgmtSrv::pyListAllObjects(
	void
)
{
	PyGILState_STATE	gstate = PyGILState_Ensure();

	PyObject*	key;
	PyObject*	val;
	Py_ssize_t	pos = 0;	// 처음에는 0으로 설정해야 한다.
	while (PyDict_Next(m__globalDict, &pos, &key, &val)) {	// All are borrowed references.
		(void) PyObject_Print(key, stdout, 0);
		(void) fprintf(stdout, " = ");
		(void) PyObject_Print(val, stdout, 0);
		(void) fprintf(stdout, "\n");
	}

	PyGILState_Release(gstate);
}

int
CMgmtSrv::loadInstaller2WithOpen(
	const char*	installerName,
	const char*	installerArgumentPathname
)
{
	// 해당 파일을 연다.
	CFile	scriptFile;
	char	fullPathname[128];
	(void) snprintf(fullPathname, 128, "%s/%s", CPythonEngine::WORKER_SCRIPT_PATH, installerArgumentPathname);
	if (scriptFile.openFile(fullPathname, "r", 0) < 0) {
		(void) fprintf(stderr,
			"ERROR: "
			"CWorker::loadInstaller2WithOpen: "
			"Installer:= |%s|: "
			"CFile::openFile(\"%s\", \"r\", 0): Failure\n"
			,
			installerName, installerArgumentPathname
		);

		return -1;
	}
	// 전체를 읽는다.
	if (scriptFile.readlines() <= 0) {
		(void) fprintf(stderr,
			"ERROR: "
			"CWorker::loadInstaller2WithOpen: "
			"Installer:= |%s|: "
			"CFile::readlines(\"%s\"): Error or no data\n"
			,
			installerName, installerArgumentPathname
		);

		return -1;
	}

	const CBuffer&	scriptBuffer = *(scriptFile.getBuffer());
//	(void) printf("%s", scriptBuffer.getBuf());	// Script 의 내용
	return this->pyLoadInstaller2(installerName, scriptBuffer.getBuf());
}
int
CMgmtSrv::pyLoadInstaller2(
	const char*	installerName,
	const char*	installerArgScript
)
{
	int		iReturnVal = 0;
	PyGILState_STATE	gstate = PyGILState_Ensure();
{
	PyObject*	installerArg = NULL;
while (1) {
	const PyObject*	installer = PyDict_GetItemString(this->globalDict(), installerName);
	if (!installer) {
		(void) fprintf(stderr,
			"ERROR: "
			"CWorker::loadInstaller2(|%s|): "
			"CPythonWrapper::getScriptFunc(|%s|): Failure\n"
			,
			installerName, installerName
		);

		iReturnVal = -1;
		break;
	}

	installerArg = (PyObject*) this->__pySetObjectFromString(installerArgScript);
	if (!installerArg) {
		(void) fprintf(stderr,
			"ERROR: "
			"__FILE__:= |%s|, __LINE_NO__ = %d: "
			"CWorker::loadInstaller2(|%s|): "
			"CPythonWrapper::__pySetObjectFromString(): Failure\n"
			,
			__FILE__, __LINE__, installerName
		);

		iReturnVal = -1;
		break;
	}
	// installerArg: New reference

	PyObject*	objArg = NULL;
	PyObject*	objResult = NULL;
while (1) {
	objArg = Py_BuildValue("(O)", installerArg);
	if (!objArg) {
		(void) fprintf(stderr,
			"ERROR: "
			"__FILE__:= |%s|, __LINE_NO__ = %d: "
			"CWorker::loadInstaller2(|%s|): "
			"Py_BuildValue(|(O)|): |"
			,
			__FILE__, __LINE__, installerName
		);
		(void) PyObject_Print(installerArg, stderr, 0);
		(void) fprintf(stderr, "|: Failure\n");

		if (PyErr_Occurred())
			PyErr_Print();

		break;
	}
	objResult = PyObject_CallObject(const_cast<PyObject*>(installer), objArg);
	if (!objResult) {
		(void) fprintf(stderr,
			"ERROR: "
			"__FILE__:= |%s|, __LINE_NO__ = %d: "
			"CWorker::loadInstaller2(|%s|): "
			"PyObject_CallObject(|"
			,
			__FILE__, __LINE__, installerName
		);
		(void) PyObject_Print(const_cast<PyObject*>(installer), stderr, 0);
		(void) fprintf(stderr, "|, |");
		(void) PyObject_Print(installerArg, stderr, 0);
		(void) fprintf(stderr, "|: Failure\n");

		if (PyErr_Occurred())
			PyErr_Print();

		iReturnVal = -1;
		break;
	}

#ifdef __TRACE__
	(void) fprintf(stderr,
		"TRACE: "
		"CWorker::loadInstaller2(|%s|): |"
		,
		installerName
	);
	(void) PyObject_Print(objResult, stderr, 0);
	(void) fprintf(stderr, "|: Ok\n");
#endif	// __TRACE__

	break;
}
	Py_XDECREF(objArg); objArg = NULL;
	Py_XDECREF(objResult); objResult = NULL;
	break;
}
	Py_XDECREF(installerArg);
}
	PyGILState_Release(gstate);
	return iReturnVal;
}

void
CMgmtSrv::pyEvalAll(
	void
)
{
	PyGILState_STATE	gstate = PyGILState_Ensure();

	PyObject*	hScriptTreeByID = PyDict_GetItemString(this->m__globalDict, SCRIPT2_BY_ID); assert(hScriptTreeByID);
	this->__pyEvalAll(hScriptTreeByID);

#if 0
	PyObject*	keyIndex;
	PyObject*	valAgentSockInfo;
	Py_ssize_t	pos = 0;
	while (PyDict_Next(this->m__acceptSockfd__DICT, &pos, &keyIndex, &valAgentSockInfo)) {
		PyObject*	agent = PyDict_GetItemString(valAgentSockInfo, ACCEPT_SOCKET__KEY_NAME_AGENT_PTR); assert(agent);
		CAgent*	agentPtr = (CAgent*) PyLong_AsVoidPtr(agent); assert(agentPtr);

		printf("%d\n", PySet_Clear(agentPtr->getScriptID__SET()));
		printf("%d\n", PySet_Clear(agentPtr->getByteStreamID__SET()));
		printf("%d\n", PySet_Clear(agentPtr->getCommandID__SET()));
	}
#endif

	PyGILState_Release(gstate);
}

void
CMgmtSrv::pyListByteStream(
	void
)
{
	PyGILState_STATE	gstate = PyGILState_Ensure();
{
	PyObject*	hByteStream2ByName = PyDict_GetItemString(this->m__globalDict, BYTE_STREAM2_BY_NAME); assert(hByteStream2ByName);
	(void) fprintf(stderr, "%s:= |", BYTE_STREAM2_BY_NAME); (void) PyObject_Print(hByteStream2ByName, stderr, 0); (void) fprintf(stderr, "|\n");
	PyObject*	hByteStream2ByID = PyDict_GetItemString(this->m__globalDict, BYTE_STREAM2_BY_ID); assert(hByteStream2ByID);
	(void) fprintf(stderr, "%s:= |", BYTE_STREAM2_BY_ID); (void) PyObject_Print(hByteStream2ByID, stderr, 0); (void) fprintf(stderr, "|\n");
}
	PyGILState_Release(gstate);
}
void
CMgmtSrv::pyListCommand(
	void
)
{
	PyGILState_STATE	gstate = PyGILState_Ensure();
{
	PyObject*	hCommand2ByName = PyDict_GetItemString(this->m__globalDict, COMMAND2_BY_NAME); assert(hCommand2ByName);
	(void) fprintf(stderr, "%s:= |", COMMAND2_BY_NAME); (void) PyObject_Print(hCommand2ByName, stderr, 0); (void) fprintf(stderr, "|\n");
	PyObject*	hCommand2ByID = PyDict_GetItemString(this->m__globalDict, COMMAND2_BY_ID); assert(hCommand2ByID);
	(void) fprintf(stderr, "%s:= |", COMMAND2_BY_ID); (void) PyObject_Print(hCommand2ByID, stderr, 0); (void) fprintf(stderr, "|\n");
}
	PyGILState_Release(gstate);
}
void
CMgmtSrv::pyListJob(
	void
)
{
	PyGILState_STATE	gstate = PyGILState_Ensure();
{
	PyObject*	hJob2ByName = PyDict_GetItemString(this->m__globalDict, JOB2_BY_NAME); assert(hJob2ByName);
	(void) fprintf(stderr, "%s:= |", JOB2_BY_NAME); (void) PyObject_Print(hJob2ByName, stderr, 0); (void) fprintf(stderr, "|\n");
	PyObject*	hJob2ByID = PyDict_GetItemString(this->m__globalDict, JOB2_BY_ID); assert(hJob2ByID);
	(void) fprintf(stderr, "%s:= |", JOB2_BY_ID); (void) PyObject_Print(hJob2ByID, stderr, 0); (void) fprintf(stderr, "|\n");
}
	PyGILState_Release(gstate);
}

int
CMgmtSrv::updateAgentWithOpen(
	CBuffer&	packet,
	const char*	agentPathname,
	int	resume
)
{
	// 해당 파일을 연다.
	CFile	agentFile;
	int	isPipeFalse = 0;
	if (agentFile.openFile(agentPathname, "rb", isPipeFalse) < 0) {
		eprintf(E_ERR, EFMT("CFile::openFile(|%s|, |rb|): Failure"), agentPathname);

		return -1;
	}
	// 전체를 읽는다.
	if (agentFile.readAll() <= 0) {
		eprintf(E_ERR, EFMT("CFile::readAll(): Failure"));

		return -1;
	}

	const CBuffer&	byteStreamBuffer = *(agentFile.getBuffer());
	return CMgmtSrv::updateAgent(packet, byteStreamBuffer, resume);
}
int
CMgmtSrv::updateAgent(
	CBuffer&	packet,
	const CBuffer&	byteStreamBuffer,
	int		resume
)
{
	int	iReturnVal = 0;

	CPacket::ORD_TYPE_T	orderType = CPacket::ORD_TYPE__UPDATE_AGENT;
	unsigned int		orderID = 907;
	unsigned int		xxxID = resume;
	unsigned int*		xxxIDPtr = &xxxID;
	int			isBinaryTrue = 1;
	if (CPacket::packOrder(
		packet,

		orderType,
		orderID,
		xxxIDPtr,
		isBinaryTrue,
		byteStreamBuffer.getBuf(), byteStreamBuffer.getDataSize(),
		NULL
	) < 0) {
		eprintf(E_ERR, EFMT("CPacket::packOrder(ORD_TYPE__UPDATE_AGENT): Failure"));

		iReturnVal = -1;
	} else
		iReturnVal = 1;	// 보낼 패킷이 있다.

	return iReturnVal;
}
