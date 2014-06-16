#include "workerTypeI.h"
#include "CWorker.h"	// CWorker::WORKER_ARG_T

#include <stdio.h>	// printf, fprintf
#include <string.h>	// strchr
#include <time.h>	// clock_gettime

int
main(
	int		argc,
	const char*	argv[]
)
{
	// Call directly "workerTypeI" function...
	if (argc < 2) {
		(void) fprintf(stderr, "Usage: %s program [nLineSkipped]\n", argv[0]);

		return 0;
	}

	CWorker::WORKER_ARG_T	workerArg;
	int	copy = 0;	// Borrowed reference
	if (workerArg.arg.setArg(argc - 1, argv + 1, copy) < 0) {
		(void) fprintf(stderr, "CArg::setArg(int, const char**): Failure\n");

		return 1;
	}
	workerTypeI((void*) &workerArg);

	return 0;
}
