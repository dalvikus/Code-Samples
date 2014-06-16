#include <Python.h>
#include <string.h>
#include "eprintf.h"

void
eprintf(
	E_LEVEL			e_level,
	const char* const	format,
	...
)
{
	// 이 함수를 호출하기 직전의 errno 값을 기억한 후,
	// "%m" 의 경우에 해당 에러문자열을 쓰도록 한다.
	int	errnum = errno;

	const char*	fmt = format;
	FILE*	fp = stderr;

	(void) fprintf(fp,
		"Error Level:= |%s|\n"
		,
		e_level == E_FATAL ? "FATAL" : (
		e_level == E_ERR ? "ERROR" : (
		e_level == E_WARN ? "WARNING" : (
		e_level == E_INFO ? "INFOMATION" : (
		e_level == E_TRACE ? "TRACE" : "UNKNOWN"
		))))
	);
	va_list	ap;
	va_start(ap, format);
	while (*fmt) {
		char	c = *fmt++;
		if (c != '%') {
			if (fputc(c, fp) == EOF) {
				(void) fprintf(stderr,
					"ERROR: "
					"__FILE__:= |%s|, __LINE_NO__ = %d: "
					"CPythonWrapper::printf: "
					,
					__FILE__, __LINE__
				);
				perror("fputc");
				va_end(ap);
				return;
			}
			continue;
		}
		// '%'

		c = *fmt;
		if (c == '\0') {
			(void) fprintf(stderr,
				"WARNING: "
				"__FILE__:= |%s|, __LINE_NO__ = %d: "
				"CPythonWrapper::printf: "
				"format:= |%s|: Last character is '%%': ???\n"
				,
				__FILE__, __LINE__, format
			);
			continue;
		}
		++fmt;

		switch (c) {
		case '%':
			if (fputc('%', fp) == EOF) {
				(void) fprintf(stderr,
					"ERROR: "
					"__FILE__:= |%s|, __LINE_NO__ = %d: "
					"CPythonWrapper::printf: "
					,
					__FILE__, __LINE__
				);
				perror("fputc");
				va_end(ap);
				return;
			}
			break;
		case 'c':
			(void) fprintf(fp, "%c", va_arg(ap, int));
		case 's':
			(void) fprintf(fp, "%s", va_arg(ap, const char*));
			break;
		case 'O':
{
			PyGILState_STATE	gstate = PyGILState_Ensure();
			PyObject*	Str = PyObject_Str(va_arg(ap, PyObject*)); assert(Str);
			(void) fprintf(fp, "%s", PyString_AsString(Str));
			PyGILState_Release(gstate);
}
			break;
		case 'd':
			(void) fprintf(fp, "%d", va_arg(ap, int));
			break;
		case 'u':
			(void) fprintf(fp, "%u", va_arg(ap, unsigned int));
			break;
		case 'm':
{
#if 0
	// GNU version:
	// cygwin 에서는 사용된다.
	char	buf__NotUsed[512];
	char*	s = strerror_r(errnum, buf__NotUsed, 512);
	(void) fprintf(fp, "|%s|(s = %p, buf__NotUsed = %p)", buf__NotUsed, s, buf__NotUsed);
#else
	(void) fprintf(fp, "|%s|", strerror(errnum));
#endif
}
			break;
		default:
			(void) fprintf(stderr,
				"WARNING: "
				"CPythonWrapper::printf: "
				"Unknown format character: |%c|: Ignored\n"
				,
				c
			);
			break;
		}
	}
	va_end(ap);

	PyGILState_STATE	gstate = PyGILState_Ensure();
	if (PyErr_Occurred())
		PyErr_Print();
	PyGILState_Release(gstate);
}
