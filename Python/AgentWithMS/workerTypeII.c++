////////////////////////////////////////////////////////////////////////////////
/*
man dlopen에 예가 나와있다. symbol의 이름에 대해서는 다음 주소를 참고했다.
	http://www.cs.indiana.edu/~welu/notes/node36.html#c++_name_mangling

설명서에 나온대로 두 번에 걸쳐서 할 수 있다.

	g++ -Wall -c workerTypeII.c++ 
	ld -shared workerTypeII.o -soname libworkerTypeII.so -o libworkerTypeII.so

소스가 C++이므로 gcc를 사용했더라도 C++로 인식해서 g++을 사용한 것과 같다.
즉 lib###.so에 있는 C++에 있는 symbol은 보이는 것과 다르다.

다음과 같이 한 번에 할 수도 있다.

	g++ -o libworkerTypeII.so workerTypeII.c++ -shared -Wl,-soname="libworkerTypeII.so"

C에서 사용하는 symbol이름과 C++에서 사용하는 symbol이름이 다른다.
(objdump를 다음과 같이 사용하여

	objdump -t libworkerTypeII.so

libworkerTypeII.so에 있는 symbol들의 이름을 확인할 수 있다.)
C에서처럼 사용하기 위해서는 전체의 앞뒤를

	extern "C" {
		......
	}

로 묶는다.

이 lib###.so를 사용하는 실행파일은 반드시 "-rdynamic"옵션을 사용해서 만든다. 예를 들어,
이 때, 당장은 사용하지 않지만 lib###.so을 로드할 때 거기에서 사용하는 ###.o도 함께 넣어준다.

	g++ -Wall -rdynamic `python-config --cflags` workerTypeIIModuleExample.c++ CArg.o CWorkerModule.o -ldl -lrt `python-config --ldflags`

workerTypeIIModuleExample.c++만을 보면 CFile.o나 -lrt에 있는 함수를 직접 사용하지 않는다.
하지만 lib###.so에서 사용하므로 이렇게 미리 넣어주저야 한다!
 */

#include <Python.h>
#include "CFile.h"
#include "CWorkerArg.h"

#include <stdio.h>	// printf, fprintf, fgets, feof, ferror, popen, pclose
#include <errno.h>	// ETIMEDOUT
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
	(void) fprintf(stderr, "WorkerTypeII: catch_signal: Signal(%d) delivered\n", signo);
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
workerTypeII(
	void*	arg
)
{
	CWorkerArg*	pWorkerArg = (CWorkerArg*) arg;
#if 1
while (1) {
		struct timeval	tv;
		tv.tv_sec = 1;
		tv.tv_usec = 0;
		if (select(0, NULL, NULL, NULL, &tv) < 0) {
			perror("workerTypeII: select()");

			continue;
		}

//		(void) printf("hello, world: Worker: Type II\n");
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
break;
		PyObject*	hCommandJob = PyDict_GetItemString(hCommand, "hJob"); assert(hCommandJob);
		(void) printf("hJob in Worker Type II = |"); (void) PyObject_Print(hCommandJob, stdout, 0); (void) printf("|\n");
} while (0);
		PyGILState_Release(gstate);

		if (ctrl == CWorker::CMD_CTRL__STOP) {
			(void) printf("Worker Type II(|%s|) returns...\n", name_cstr);
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
		(void) fprintf(stderr, "workerTypeII: signal(SIGHUP): Failure\n");
		return;
	}
	if (signal(SIGUSR1, catch_signal) == SIG_ERR) {
		(void) fprintf(stderr, "workerTypeII: signal(SIGUSR1): Failure\n");
		return;
	}
	if (signal(SIGUSR2, catch_signal) == SIG_ERR) {
		(void) fprintf(stderr, "workerTypeII: signal(SIGUSR2): Failure\n");
		return;
	}
#endif

	if (argc < 1) {
		(void) fprintf(stderr, "What is COMMAND_STRING?\n");

		return;
	}
	int	interval = 1;
	if (argc > 1) {
		int	i = atoi(argv[1]);
		if (i > 0)
			interval = i;
	}
	int	nLineToBeSkipped = 0;	// 각 명령어의 처음 몇 줄(헤더)은 무시한다.
					// 단순히 줄 단위로 할 수도 있고,
					// 복잡하게는 내용을 가지고 판단할 수도 있다.
					// 실제로 LINUX의 vmstat의 경우 주기적으로 헤더가 나타난다.
					// 물론 "-n"옵션을 사용해서 처음에만 나타나게 한다.
					// 하지만 명령어에 따라 시스템에 따라 다르게 처리되어야 한다는 것을 염두에 둔다.
	if (argc > 2) {
		int	i = atoi(argv[2]);
		if (i > 0)
			nLineToBeSkipped = i;
	}
	int	nLineToBeTailed = 0;	// 마찬가지로 제일 마지막 몇 줄을 무시할 수도 있다.
	if (argc > 3) {
		int	i = atoi(argv[3]);
		if (i > 0)
			nLineToBeTailed = i;
	}

	CBuffer		buffer;
	if (!buffer.getBufSize()) {
		// Contructor에서 메모리를 할당할 때 문제가 있었다.
		// 이 사실을 알리고 쓰레드를 종료한다.
		(void) fprintf(stderr, "CBuffer::CBuffer(): Failure\n");

		return;
	}

	do {
{
#if 1
		struct timeval	tv;
		// 최대 1초까지...
		tv.tv_sec = interval;
		tv.tv_usec = 0;

		if (select(0, NULL, NULL, NULL, &tv) < 0) {
			perror("WARNING: workerTypeI: select(): Failure");

			continue;
		}
#else
		(void) pthread_mutex_lock(cv.getMutex());
		if (pthread_cond_timedwait(cv.getCV(), cv.getMutex(), &ts) != ETIMEDOUT) {
			(void) fprintf(stderr, "pthread_cond_timedwait(): Not ETIMEDOUT: Ignored\n");

			continue;
		}
		(void) pthread_mutex_unlock(cv.getMutex());
#endif
}
		// signal 핸들러에서 처리된 signal 이 있는 지 알아본다.
		if (controlX == 2) {	// STOP
			(void) fprintf(stderr, "WorkerTypeII: SIGUSR2 delivered\n");

//			return;
		}

		CFile	file(NULL, -1, 1, &buffer);
		if (file.openFile(argv[0], "r", 1) < 0) {
			(void) fprintf(stderr, "CFile::openFile(\"%s\", \"r\", pipe = True): Failure\n", argv[1]);
			return;
		}
		int	fileid = fileno(file.getFILE());
		if (fileid < 0) {
#ifdef __TRACE__
			perror("workerTypeII: fileno(): ");
#endif	// __TRACE__
			return;
		}

		fd_set	rfds;

		// 파이트에서 읽을 내용이 있는 지 알아본다.
		FD_ZERO(&rfds);
		FD_SET(fileid, &rfds);
#if 0
		struct timespec	ts_;
		ts_.tv_sec = 1;
		ts_.tv_nsec = 0;

		sigset_t	set;
		sigemptyset(&set);
		sigfillset(&set);
		int	retval = pselect(fileid + 1, &rfds, NULL, NULL, &ts_, &set);
#else
		struct timeval	tv;
		// 최대 1초까지...
		tv.tv_sec = 1;
		tv.tv_usec = 0;

		int	retval = select(fileid + 1, &rfds, NULL, NULL, &tv);
#endif
		if (retval < 0) {
			perror("workerTypeII: select()");

//			return;
		}
		if (retval == 0) {
			(void) printf("1초 동안 어떤 데이터도 없었다\n");
			continue;
		}

		// 읽을 내용이 있다.
		if (file.readlines(nLineToBeSkipped, nLineToBeTailed) < 0) {
			(void) fprintf(stderr, "CFile::readlines(): Failure\n");
			return;
		}

#if 1
{
		PyGILState_STATE	gstate = PyGILState_Ensure();

		PyObject*	objArgv = NULL;
		PyObject*	objReturn = NULL;
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
				"workerTypeII: "
				"Py_BuildValue(|(s)|): Failure\n"
			);
			if (PyErr_Occurred())
				PyErr_Print();

			break;
		}
		PyObject*	objReturn = PyObject_CallObject(const_cast<PyObject*>(*callbackPtr), objArgv);
		if (!objReturn) {
			(void) fprintf(stderr,
				"ERROR: "
				"workerTypeII: "
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

		break;
////////////////////////////////////////////////////////////////////////////////
}
		Py_XDECREF(objArgv);
		Py_XDECREF(objReturn);

		/* Release the thread. No Python API allowed beyond this point. */
		PyGILState_Release(gstate);
}
#else
		// 모은 내용을 쓴다.
		(void) printf("////////////////////////////////////////////////////////////////////////////////\n...%ld...\n%s////////////////////////////////////////////////////////////////////////////////\n", ts.tv_sec, buffer.getBuf());
#endif
	} while (1);
#endif

	return;
}

////////////////////////////////////////////////////////////////////////////////
}
/*
 * From http://docs.hp.com/en/B2355-90695/pthread_cond_wait.3T.html
 *
	pthread_mutex_t	mutex = PTHREAD_MUTEX_INITIALIZER;
	pthread_cond_t	cond = PTHREAD_COND_INITIALIZER;
	struct timespec	abstime;

	(void) pthread_mutex_lock(&mutex);
//	abstime = absolute time to timeout.

	while (predicate == FALSE) {
		ret = pthread_cond_timedwait(&cond, &mutex, &abstime);
		if (ret == ETIMEDOUT) {
			if (predicate == FALSE) {
				// Code for time-out condition
			} else {
				// success condition
				break;
			}
		}
	}
	(void) pthread_mutex_unlock(&mutex);
//	Code for success condition.
 */
