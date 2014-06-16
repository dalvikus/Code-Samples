#include "CWorker.h"	// CWorker::WORKER_ARG_T
#include "CWorkerModule.h"

#include <stdio.h>	// printf, fprintf, fgets, feof, ferror, popen, pclose
#include <errno.h>	// ETIMEDOUT
#include <string.h>	// strchr
#include <time.h>	// clock_gettime

/*
workerTypeII.c++에서...

이 lib###.so를 사용하는 실행파일은 반드시 "-rdynamic"옵션을 사용해서 만든다. 예를 들어,
이 때, 당장은 사용하지 않지만 lib###.so을 로드할 때 거기에서 사용하는 ###.o도 함께 넣어준다.

	g++ -Wall -rdynamic workerTypeIIModuleExample.c++ CBuffer.o CFile.o -ldl -lrt

workerTypeIIModuleExample.c++만을 보면 CFile.o나 -lrt에 있는 함수를 직접 사용하지 않는다.
하지만 lib###.so에서 사용하므로 이렇게 미리 넣어주어야 한다!
 */

int
main(
	int		argc,
	const char*	argv[]
)
{
	if (argc < 3) {
		(void) fprintf(stderr,
			"Usage: %s entryPoint modulePathname(lib###.so) program [interval [nLineSkipped [nLineTailed]]]\n"
			,
			argv[0]
		);

		return 0;
	}

	CWorkerModule	workerModule(argv[1], argv[2]);
	if (workerModule.getHandle() == NULL) {
		(void) fprintf(stderr,
			"CWorkerModule::CWorkerModule(\"%s\", \"%s\"): Failure\n"
			,
			argv[1], argv[2]
		);

		return 0;
	}
	CWorker::WORKER_ARG_T	workerArg;
	int	copy = 0;	// Borrowed reference
	if (workerArg.arg.setArg(argc - 3, argv + 3, copy) < 0) {
		(void) fprintf(stderr, "CArg::setArg(int, const char**): Failure\n");

		return 1;
	}
	((void (*)(void*)) workerModule.getEntryPoint())((void*) &workerArg);

	return 0;
}
