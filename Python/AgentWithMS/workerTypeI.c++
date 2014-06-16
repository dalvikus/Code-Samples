#include <Python.h>
#include "CFile.h"
#include "CWorkerArg.h"
#include "CQueueSocketThread.h"
#include "CPacket.h"

#include <stdio.h>	// printf, fprintf
#include <string.h>	// strchr
#include <signal.h>	// signal, SIGXXX
#include <time.h>	// clock_gettime

extern "C" {
////////////////////////////////////////////////////////////////////////////////

#if 0
volatile sig_atomic_t	controlX = 0;	// 0: CONTINUE, 1: PAUSE, 2: STOP

// signal handler for "controlX"
static void
catch_signal(
	int	signo
)
{
	(void) fprintf(stderr, "WorkerTypeI: catch_signal: Signal(%d) delivered\n", signo);
	if (signo == SIGHUP)
		controlX = 0;		// CONTINUE
	else if (signo == SIGUSR1)
		controlX = 1;		// PAUSE
	else if (signo == SIGUSR2)
		controlX = 2;		// STOP
	else
		(void) fprintf(stderr, "catch_signal: Unknown signal(%d)\n", signo);
}
#endif

void
workerTypeI(
	void*	arg
)
{
	CWorkerArg*	pWorkerArg = (CWorkerArg*) arg;
sleep(3);
	CWorker&		worker = *(const_cast<CWorker*>(pWorkerArg->getWorker()));
	CQueueSocketThread&	queue_socket = *(worker.getQueueSocketThread());
#if 1
while (1) {
		struct timeval	tv;
		tv.tv_sec = 0;
		tv.tv_usec = 10000;
		if (select(0, NULL, NULL, NULL, &tv) < 0) {
			perror("workerTypeI: select()");

			continue;
		}

//		(void) printf("hello, world: Worker: Type I\n");
		// Job 의 목록을 보자.
		CWorker::CMD_CTRL_T	ctrl;
		const char*	name_cstr = NULL;

		PyGILState_STATE	gstate = PyGILState_Ensure();
do {
		PyObject*	hCommand = pWorkerArg->hCommand;
//		PyObject*	name = PyDict_GetItemString(hCommand, CWorker::COMMAND__KEY_NAME__NAME); assert(name);
		PyObject*	name = PyDict_GetItemString(hCommand, "Name"); assert(name);
		name_cstr = PyString_AsString(name); assert(name_cstr);
//		PyObject*	control = PyDict_GetItemString(hCommand, CWorker::COMMAND__KEY_NAME__CONTROL); assert(control);
		PyObject*	control = PyDict_GetItemString(hCommand, "Control"); assert(control);
		ctrl = (CWorker::CMD_CTRL_T) PyInt_AsLong(control);
#if 1
		PyObject*	parseScriptID = PyDict_GetItemString(hCommand, "ArgumentScriptID__LINK"); assert(parseScriptID);
		PyObject*	hScript = PyDict_GetItem((PyObject*) worker.gethScriptByID(), parseScriptID); assert(hScript);
		PyObject*	scriptName = PyDict_GetItemString(hScript, "Name"); assert(scriptName);
		PyObject*	ref = PyDict_GetItem((PyObject*) worker.globalDict(), scriptName); assert(ref);
		PyObject*	result = NULL;
		PyObject*	result_Str = NULL;
	do {
		result = PyObject_CallObject(ref, NULL); assert(result);
{
		CBuffer		data_wPacket;
		int	isPyDictFalse = 0;
		CPacket::RESP_TYPE_T	respType = CPacket::RESP_TYPE__OK;
		unsigned	orderID = 504;
		PyObject*	result_Str = PyObject_Str(result); assert(result_Str);
		const char*	result_Str_cstr = PyString_AsString(result_Str); assert(result_Str_cstr);
		if (CPacket::packResponse(
			data_wPacket,

			isPyDictFalse,
			respType,
			orderID,
			result_Str_cstr
		) < 0) {
			eprintf(E_ERR, EFMT("CPacket::packResponse(): Failure"));
		} else {
			if (queue_socket.enqueue_ts(data_wPacket) < 0) {
				eprintf(E_ERR, EFMT("CSocketO::enqueue_ts(): Failure"));
			}
		}
}
		(void) fprintf(stderr, "result:= |");
		(void) PyObject_Print(result, stderr, 0);
		(void) fprintf(stderr, "|\n");
	} while (0);
		Py_XDECREF(result); result = NULL;
		Py_XDECREF(result_Str); result_Str = NULL;
#endif
		break;
		PyObject*	hCommandJob = PyDict_GetItemString(hCommand, "hJob"); assert(hCommandJob);
#if 0
		(void) printf("hJob in Worker Type I = %p -> |", hCommand); (void) PyObject_Print(hCommandJob, stdout, 0); (void) printf("|\n");
#else
		PyObject*	key;
		PyObject*	val;
		Py_ssize_t	pos = 0;
		while (PyDict_Next(hCommandJob, &pos, &key, &val)) {
			(void) printf("Job ID:= |"); (void) PyObject_Print(key, stdout, 0); (void) printf("|\n");
		}
#endif
} while (0);
		PyGILState_Release(gstate);

		if (ctrl == CWorker::CMD_CTRL__STOP) {
			(void) printf("Worker Type I(|%s|) returns...\n", name_cstr);
			return;
		}
}
#endif

#if 0
#if 0
	sigset_t	addset, original_set;
	sigaddset(&addset, SIGUSR2);
	pthread_sigmask(SIG_BLOCK, &addset, &original_set);
#endif
#if 1
	if (signal(SIGHUP, catch_signal) == SIG_ERR) {
		(void) fprintf(stderr, "workerTypeI: signal(SIGHUP): Failure\n");
		return;
	}
	if (signal(SIGUSR1, catch_signal) == SIG_ERR) {
		(void) fprintf(stderr, "workerTypeI: signal(SIGUSR1): Failure\n");
		return;
	}
	if (signal(SIGUSR2, catch_signal) == SIG_ERR) {
		(void) fprintf(stderr, "workerTypeI: signal(SIGUSR2): Failure\n");
		return;
	}
#endif

	if (argc < 1) {
		(void) fprintf(stderr, "What is COMMAND_STRING?\n");

		return;
	}
	int	nLineToBeSkipped = 0;	// 각 명령어의 처음 몇 줄(헤더)은 무시한다.
					// 단순히 줄 단위로 할 수도 있고,
					// 복잡하게는 내용을 가지고 판단할 수도 있다.
					// 실제로 LINUX의 vmstat의 경우 주기적으로 헤더가 나타난다.
					// 물론 "-n"옵션을 사용해서 처음에만 나타나게 한다.
					// 하지만 명령어에 따라 시스템에 따라 다르게 처리되어야 한다는 것을 염두에 둔다.
	if (argc > 1) {
		int	i = atoi(argv[1]);
		if (i > 0)
			nLineToBeSkipped = i;
	}

	CFile	file;
	const CBuffer&	buffer = *(file.getBuffer());
	if (!buffer.getBufSize()) {
		// Contructor에서 메모리를 할당할 때 문제가 있었다.
		// 이 사실을 알리고 쓰레드를 종료한다.
		(void) fprintf(stderr, "CBuffer::CBuffer(): Failure\n");

		return;
	}

	if (file.openFile(argv[0], "r", 1)) {
		// cv를 사용하여 정보를 수집할 수 없다.
		// 이 사실을 알리고 쓰레드를 종료한다.
		(void) fprintf(stderr, "popen(\"%s\"): Failure\n", argv[0]);

		return;
	}
	int	fileid = fileno(file.getFILE());
	if (fileid < 0) {
#ifdef __TRACE__
		perror("workerTypeI: fileno(): ");
#endif	// __TRACE__
		return;
	}

	do {
		fd_set	rfds;

		// 파이트에서 읽을 내용이 있는 지 알아본다.
		FD_ZERO(&rfds);
		FD_SET(fileid, &rfds);
		// 최대 1초까지...
#if 0
		struct timespec	ts;
		ts.tv_sec = 0;
		ts.tv_nsec = 1000000000;

		sigset_t	set;
//		sigaddset(&set, SIGUSR2);
		sigemptyset(&set);
//		sigfillset(&set);
		int	retval = pselect(fileid + 1, &rfds, NULL, NULL, &ts, &set);
#else
		struct timeval	tv;
		tv.tv_sec = 1;
		tv.tv_usec = 0;

		int	retval = select(fileid + 1, &rfds, NULL, NULL, &tv);
#endif
		if (retval < 0) {
			perror("workerTypeI: select()");

//			return;
		}
		if (retval == 0) {
//			(void) printf("1초 동안 어떤 데이터도 없었다\n");
			continue;
		}

		// 읽을 내용이 있다.
		if (file.readline() <= 0) {
			break;
		}

		// signal 핸들러에서 처리된 signal 이 있는 지 알아본다.
		if (controlX == 2) {	// STOP
			(void) fprintf(stderr, "WorkerTypeI: SIGUSR2 delivered\n");

//			return;
		}

		// 버퍼에 담긴 내용(완전한 하나의 줄)을 사용한다.
		if (nLineToBeSkipped) {
			// 처음 몇 줄은 건너뛴다.
			--nLineToBeSkipped;
		} else {
			// 읽어들인 내용이 있음을 WorkerParent 쓰레드에 알린다.

#if 1
{
		PyGILState_STATE	gstate = PyGILState_Ensure();
while (1) {
////////////////////////////////////////////////////////////////////////////////
		struct timespec ts;
		(void) clock_gettime(CLOCK_REALTIME, &ts);
		PyObject*	objArgv = Py_BuildValue(
			"(s, l, s)"
			,
			pWorkerArg->keyCommand,
			ts.tv_sec,
			buffer.getBuf()
		);
		if (!objArgv) {
			(void) fprintf(stderr,
				"ERROR: "
				"workerTypeI: "
				"Py_BuildValue(|(s, l, s)|): Failure\n"
			);
			if (PyErr_Occurred())
				PyErr_Print();

			break;
		}
		PyObject*	objReturn = PyObject_CallObject(const_cast<PyObject*>(*callbackPtr), objArgv);
		Py_DECREF(objArgv);	// 더 이상 사용하지 않는다.
		if (!objReturn) {
			(void) fprintf(stderr,
				"ERROR: "
				"workerTypeI: "
				"PyObject_CallObject(|"
			);
			int	flags = 0;	// Py_PRINT_RAW: No string qutes etc.
			(void) PyObject_Print(const_cast<PyObject*>(*callbackPtr), stderr, flags);
			(void) fprintf(stderr, "|): Failure\n");
			if (PyErr_Occurred())
				PyErr_Print();

			break;
		}

		int	flags = 0;	// Py_PRINT_RAW: No string qutes etc.
		(void) fprintf(stdout, "id:= %p: ", &pWorkerArg->id);
		(void) PyObject_Print(objReturn, stdout, flags);
		(void) fprintf(stdout, "\n");

		Py_DECREF(objReturn);	// 더 이상 사용하지 않는다.
		break;
////////////////////////////////////////////////////////////////////////////////
}
		/* Release the thread. No Python API allowed beyond this point. */
		PyGILState_Release(gstate);
}
#else
			struct timespec ts;
			(void) clock_gettime(CLOCK_REALTIME, &ts);
			(void) printf("////////////////////////////////////////////////////////////////////////////////\n...%ld...\n%s////////////////////////////////////////////////////////////////////////////////\n", ts.tv_sec, buffer.getBuf());
#endif
		}
	} while (1);
#endif

	return;
}

////////////////////////////////////////////////////////////////////////////////
}
