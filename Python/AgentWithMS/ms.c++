#include <Python.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>	// sleep(3)

#include "eprintf.h"
#include "CFile.h"
#include "CPacket.h"
#include "CPacketList.h"
#include "CMgmtSrv.h"
#include "CJobDownPacket.h"
#include "CSocket.h"

int
main(
	int		argc,
	const char*	argv[]
)
{
	CMgmtSrv	ms;
// 다음 명령을 미리 실행한다.
//	LOAD__SCRIPT_FUNCTION |setScriptTree2| |setScriptTree2.py| 1
//	LOAD__INSTALLER |setScriptTree2| |scriptTreeInstaller2.py|
//	EVAL__ALL
{
	int	overwriteTrue = 1;
	assert(ms.setFuncWithOpen("setScriptTree2", "setScriptTree2.py", overwriteTrue));
	assert(ms.loadInstaller2WithOpen("setScriptTree2", "scriptTreeInstaller2.py") == 0);
	ms.pyEvalAll();
}

	int	listenSockfd = ms.getListenSockfd();

	CFile	file(stdin, 0, 0);
	const CBuffer&	buffer = *(file.getBuffer());
	if (!buffer.getBufSize()) {
		// Contructor에서 메모리를 할당할 때 문제가 있었다.
		// 이 사실을 알리고 쓰레드를 종료한다.
		(void) fprintf(stderr, "CBuffer::CBuffer(): Failure\n");

		return 1;
	}

	int	n = STDIN_FILENO;
	if (listenSockfd > n)
		n = listenSockfd;
	++n;
	int	timeout = 0;
	PyObject*	lastCommand = NULL;
	while (1) {
		fd_set	rfds;
		struct timeval	tv;

		FD_ZERO(&rfds);
		FD_SET(STDIN_FILENO, &rfds);
		FD_SET(listenSockfd, &rfds);
		tv.tv_sec = 1;
		tv.tv_usec = 0;

		if (!timeout)
			(void) fprintf(stderr, "CMD> ");
		int	retval = select(n, &rfds, NULL, NULL, &tv);
		/* Don't rely on the value of tv now! */

		if (retval) {
			if (FD_ISSET(listenSockfd, &rfds)) {
				if (ms.pyAcceptAgent() < 0) {
					eprintf(E_ERR, EFMT("CMgmtSrv::PyAcceptAgent: Failure"));
				}

				continue;
			}

			/* FD_ISSET(0, &rfds) will be true. */
//			(void) printf("FD_ISSET(0, &rfds) = %d: Data is available now.\n", FD_ISSET(0, &rfds));

			file.readline();
			if (buffer.getDataSize() == 0) {
				// The End
				(void) printf("Bye~\n");

				break;
			}
//			(void) fprintf(stderr, "CMD> |%s|\n", buffer.getBuf());
			int	isValidCommand = 1;
			const char*	command = buffer.getBuf();
RESTART:
			if (strstr(command, "HELP")) {
				ms.help();
			} else if (strstr(command, "LOAD__SCRIPT_FUNCTION")) {
////////////////////////////////////////////////////////////////////////////////
	char	funcName[128];
	funcName[0] = '\0';
	char	funcScriptPathname[128];
	funcScriptPathname[0] = '\0';
	int	overwrite = 0;
	int	valid = 0;
	if (sscanf(command, "LOAD__SCRIPT_FUNCTION |%s |%s %d", funcName, funcScriptPathname, &overwrite) >= 2) {
		size_t	len;
		char*	p;
		len = strlen(funcName);
		p = strchr(funcName, '|');
		if (len && p && len == (size_t) (p - funcName + 1)) {
			*p = '\0';
#if 0
			(void) printf("FUNCTION NAME:= |%s|\n", funcName);
#endif

			len = strlen(funcScriptPathname);
			p = strchr(funcScriptPathname, '|');
			if (len && p && len == (size_t) (p - funcScriptPathname + 1)) {
				*p = '\0';
#if 0
				(void) printf("SCRIPT PATHNAME: |%s|\n", funcScriptPathname);
#endif
				valid = 1;
			}
		}
	}
	if (valid) {
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
	PyObject*	objFunc = ms.setFuncWithOpen(funcName, funcScriptPathname, overwrite);
	if (!objFunc) {
		(void) fprintf(stderr,
			"CPythonEngine::setFuncWithOpen(|%s|, |%s|): Failure\n"
			,
			funcName, funcScriptPathname
		);
	} else {
#if 1	// 확인
		(void) printf("Ok\n");
#else
		(void) fprintf(stdout, "FUNCTION OBJECT: ");
		(void) PyObject_Print(objFunc, stdout, 0);
		(void) fprintf(stdout, "\n");
#endif
	}
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
	} else {
		(void) fprintf(stderr, "Usage: LOAD__SCRIPT_FUNCTION |func_name| |func_code.py| [Overwrite]\n");
	}
////////////////////////////////////////////////////////////////////////////////
			} else if (strstr(command, "LOAD__INSTALLER")) {
////////////////////////////////////////////////////////////////////////////////
	char	installerName[128];
	installerName[0] = '\0';
	char	installerArgPathname[128];
	installerArgPathname[0] = '\0';
	int	valid = 0;
	if (sscanf(command, "LOAD__INSTALLER |%s |%s", installerName, installerArgPathname) >= 2) {
		size_t	len;
		char*	p;
		len = strlen(installerName);
		p = strchr(installerName, '|');
		if (len && p && len == (size_t) (p - installerName + 1)) {
			*p = '\0';
#if 0
			(void) printf("INSTALLER TYPE NAME:= |%s|\n", installerName);
#endif

			len = strlen(installerArgPathname);
			p = strchr(installerArgPathname, '|');
			if (len && p && len == (size_t) (p - installerArgPathname + 1)) {
				*p = '\0';
#if 0
				(void) printf("INSTALLER ARGUMENT SCRIPT PATHNAME:= |%s|\n", installerArgPathname);
#endif
				valid = 1;
			}
		}
	}
	if (valid) {
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
	if (ms.loadInstaller2WithOpen(installerName, installerArgPathname) < 0) {
		(void) fprintf(stderr,
			"FATAL: "
			"__main__: "
			"CWorker::loadInstaller2WithOpen(|%s|, |%s|): Failure\n"
			,
			installerName, installerArgPathname
		);
	} else {
		(void) printf("Ok\n");
	}
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
	} else {
		(void) fprintf(stderr, "Usage: LOAD__INSTALLER |installerName| |installerArgument.py|\n");
	}
////////////////////////////////////////////////////////////////////////////////
			} else if (strstr(command, "EVAL__ALL")) {
				ms.pyEvalAll();
			} else if (strstr(command, "LIST__ALL")) {
				ms.pyListAllObjects();
			} else if (strstr(command, "LIST__JOB")) {
				ms.pyListJob();
			} else if (strstr(command, "LIST__COMMAND")) {
				ms.pyListCommand();
			} else if (strstr(command, "LIST__BYTE_STREAM")) {
				ms.pyListByteStream();
			} else if (strstr(command, "ENQUE__JOB")) {
////////////////////////////////////////////////////////////////////////////////
	int	jobID;
	int	iThread, iSlot;
	int	valid = 0;
	if (sscanf(command, "ENQUE__JOB (%d, %d) %d", &iThread, &iSlot, &jobID) == 3) {
		valid = 1;
	}
	if (valid) {
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
	PyGILState_STATE	gstate = PyGILState_Ensure();
	PyObject*	order = Py_BuildValue(
		"{s: I, s: I}", "Type", CPacket::ORD_TYPE__ADD_JOB, "ID", jobID
	); assert(order);
	if (ms.enqueueOrder(iThread << 16 | iSlot, order) < 0) {
		eprintf(E_ERR, EFMT("CMgmtSrv::enqueueOrder(|%d, %d|, |%O|): Failure"), iThread, iSlot, order);
	}
	Py_DECREF(order);
	PyGILState_Release(gstate);
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
	} else {
		(void) fprintf(stderr, "Usage: ENQUE__JOB #agentID jobID\n");
	}
////////////////////////////////////////////////////////////////////////////////
			} else if (strstr(command, "LIST__AGENT")) {
				ms.pyListAgent();
			} else if (strstr(command, "UPDATE__AGENT")) {
////////////////////////////////////////////////////////////////////////////////
	int	iThread, iSlot;
	char	agentPathname[128];
	agentPathname[0] = '\0';
	int	valid = 0;
	int	resume = 1;
	if (sscanf(command, "UPDATE__AGENT (%d, %d) |%s %d", &iThread, &iSlot, agentPathname, &resume) >= 2) {
		size_t	len;
		char*	p;
		len = strlen(agentPathname);
		p = strchr(agentPathname, '|');
		if (len && p && len == (size_t) (p - agentPathname + 1)) {
			*p = '\0';
#if 0
			(void) printf("AGENT PATHNAME: |%s|\n", agentPathname);
#endif
			valid = 1;
		}
	}
	if (valid) {
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
	PyGILState_STATE	gstate = PyGILState_Ensure();
	PyObject*	order = Py_BuildValue(
		"{s: I, s: s, s: O}",
		"Type", CPacket::ORD_TYPE__UPDATE_AGENT, "Pathname", agentPathname, "Resume", resume ? Py_True : Py_False
	); assert(order);
	if (ms.enqueueOrder(iThread << 16 | iSlot, order) < 0) {
		eprintf(E_ERR, EFMT("CMgmtSrv::pyEnqueOrder(|%d, %d|, |%O|): Failure"), iThread, iSlot, order);
	}
	Py_DECREF(order);
	PyGILState_Release(gstate);
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
	} else {
		(void) fprintf(stderr, "Usage: UPDATE_AGENT |agentPathname|\n");
	}
////////////////////////////////////////////////////////////////////////////////
			} else {
				isValidCommand = 0;
				if (strcspn(command, " \t\n") == 0 && lastCommand) {
					PyGILState_STATE	gstate = PyGILState_Ensure();
					command = PyString_AsString(lastCommand);
					PyGILState_Release(gstate);
					goto RESTART;
				} else
					ms.help();
			}
			if (isValidCommand) {
				PyGILState_STATE	gstate = PyGILState_Ensure();
				Py_XDECREF(lastCommand);
				lastCommand = PyString_FromString(command); assert(lastCommand);
				PyGILState_Release(gstate);
			}

			timeout = 0;
		} else {
			if (ms.pyCheckAgent() < 0) {
				eprintf(E_ERR, EFMT("CMgmtSrv::pyCheckAgent(): Failure"));
			}
			timeout = 1;
		}
	}
	PyGILState_STATE	gstate = PyGILState_Ensure();
	Py_XDECREF(lastCommand);
	PyGILState_Release(gstate);

	return 0;
}
