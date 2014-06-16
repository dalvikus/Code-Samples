#include "CWorkerModule.h"

#include <stdio.h>	// NULL
#include <dlfcn.h>

CWorkerModule::CWorkerModule(
	const char*	entryPoint,
	const char*	libPathname
)
{
	m__handle = dlopen(libPathname, RTLD_LAZY);
	if (!m__handle) {
#ifdef __TRACE__
		(void) fprintf(stderr,
			"__TRACE__: dlopen(\"%s\"): Failure: %s\n"
			,
			libPathname, dlerror()
		);
#endif	// __TRACE__

		m__entryPoint = NULL;
		return;
	}

	m__entryPoint = dlsym(m__handle, entryPoint);
        const char*     dlError;
        if ((dlError = dlerror()) != NULL)  {
#ifdef __TRACE__
                (void) fprintf(stderr,
			"dlsym(\"%s\"): Failure: %s\n"
			,
			entryPoint, dlError
		);
#endif	// __TRACE__

		(void) dlclose(m__handle);
		m__handle = NULL;
		m__entryPoint = NULL;
                return;
        }
}

CWorkerModule::~CWorkerModule()
{
	if (m__handle) {
		(void) dlclose(m__handle);

		m__handle = NULL;
		m__entryPoint = NULL;
	}
}
