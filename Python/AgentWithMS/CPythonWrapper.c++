#include "CPythonWrapper.h"
#include "eprintf.h"

#include <string.h>	// strcmp

CPythonWrapper::CPythonWrapper(
	void
)
{
	Py_SetProgramName((char*) "/usr/local/Package/Python-2.5.2/bin/python");
#if 1
	// signal handler를 설정하지 않도록 한다.
	// 0대신에 1즉 보통의 Py_Initialize를 호출하면 CTRL+C로 프로그램을 종료할 수 없다.
	// CTRL+Z로 프로그램을 suspend시켜서 kill하는 수 밖에 없었다.
	Py_InitializeEx(0);
#else
	Py_Initialize();
#endif
	PyEval_InitThreads();
	// Python 소스를 보면 PyEval_InitThreads 에서 lock 을 건다.
	// 풀어주지 않으면 다른 쓰레드에서 deadlock 상태가 된다.
	// 아니면 다른 쓰레드가 동작하지 않아야 하는 확신이 들 때까지
	// 그 lock 을 풀지 않을 수도 있다.
	// 그럴 필요가 있을 경우에는 여기서 미리 풀어놓지 말고 그 자리에서 풀어준다.
	// Lock 외에 PyThreadState 에 대해서도 메뉴얼에 나와있으나
	// 소스를 보면 이것으로 충분하다.
	// 또 다른 논쟁으로 다음을 참고한다.
	//	http://www.linuxjournal.com/article/3641
	PyEval_ReleaseLock();

	this->m__mainModule = PyImport_AddModule("__main__"); assert(this->m__mainModule);
	this->m__globalDict = PyModule_GetDict(this->m__mainModule); assert(this->m__globalDict);
}

CPythonWrapper::~CPythonWrapper()
{
	// 다음이 없다면 Thread 를 생성한 후 제거했을 경우,
	// Py_Finalize 에서 "Segmentation fault" 가 발생한다.
	// 아무래도 생성자에서 호출한 "PyEval_ReleaseLock" 과 관련이 된 것 같은데...
//	PyGILState_STATE	gstate = PyGILState_Ensure();
	(void) PyGILState_Ensure();
	Py_Finalize();
}


PyObject*
CPythonWrapper::__pyGetResult(
	const char*	funcName,
	const char*	funcCode,
	const PyObject*	argObject
)
{
	PyObject*	resultObject = NULL;	// 이 값이 NULL이면 어디에선가 문제가 있는 것이다.

	PyObject*	tmpModule = NULL;
do {
	// 주어진 Python 코드(funcCode) 안에 주어진 이름(funcName)의 함수만이 존재하는 지 확인한다.
	// 다른 것들이 있다면 "__main__" 모듈을 지저분하게 만드니,
	// 임시의 새로운 모듈을 만들고 그 안에서 테스트한다.
	const char	tmpModuleName[] = "tmpModule";
	tmpModule = PyModule_New(tmpModuleName);	// New ref.
	if (!tmpModule) {
		eprintf(E_ERR, EFMT("PyModule_New(|%s|): Failure"), tmpModuleName);

		break;
	}
	PyObject*	tmpDict = PyModule_GetDict(tmpModule);	// Borrowed ref.
	if (!tmpDict) {
		eprintf(E_ERR, EFMT("PyModule_GetDict(|%O|): Failure"), tmpModule);

		break;
	}
	// 새로운 모듈에는 기본적으로 두 개의 오브젝트(변수)("__name__": "...", "__doc__": "")만이 존재한다.
	// 여기에 주어진 Python 코드 안에 정의된 함수가 등록된다.
	// 그렇게 등록된 함수의 이름이 주어진 함수의 이름(funcName)과 일치하는 지,
	// 또는 또다른 이름의 함수나 변수가 주어진 Python 코드(funcCode) 안에 있지는 않은 지 알아본다.
	//	*: global값을 "__main__"에 있는 것으로 설정하지 않으면,
	//	"__main__"에 있는 "__builtins__"가 제공하는 것을 사용할 수 없다: dict, list, set, ...
	PyObject*	codeObject = PyRun_String(funcCode, Py_file_input, this->m__globalDict, tmpDict);	// New ref.
	if (!codeObject) {
		eprintf(E_ERR, EFMT("PyRun_String(|%s| in |%s|): Failure"), funcCode, tmpModuleName);

		break;
	}
	// 제대로 등록되었는 지만 관심이 있으므로,
	// 여기서 바로 Py_DECREF를 호출하여 codeObject 를 제거한다.
	Py_DECREF(codeObject);

	// 어떤 오브젝트들이 등록되었는 지 확인한다.
	PyObject*	key;
	PyObject*	val;
	Py_ssize_t	pos = 0;	// 처음에는 반드시 0으로 해야 한다라고 되어있다.
	int		iError = 0;	// 확인 중에 어떤 문제가 있다면 음수(-1)로 설정한다.
	while (PyDict_Next(tmpDict, &pos, &key, &val)) {	// key, val: Borrowed ref.
		// key 의 문자열 표현을 얻는다.
		PyObject*	key_Str = NULL;
		const char*	keyName = NULL;
		do {
			key_Str = PyObject_Str(key);
			if (!key_Str) {
				eprintf(E_ERR, EFMT("PyObject_Str(|%O|): Failure"), key);

				break;
			}
			keyName = PyString_AsString(key_Str);
			if (!keyName) {
				eprintf(E_ERR, EFMT("PyString_AsString(|%O|): Failure"), key_Str);

				break;
			}
		} while (0);
		Py_XDECREF(key_Str); key_Str = NULL;
		if (!keyName) {
			iError = -1;
			continue;
		}

		// "__name__", "__doc__"은 제외한다.
		if (
			!strcmp(keyName, "__name__") ||		// "__name__"
			!strcmp(keyName, "__doc__")		// "__doc__"
		) {
			continue;
		}

		if (strcmp(keyName, funcName)) {
			// 주어진 Python 코드(funcCode) 안에 또다른 이름의 오브젝트가 있다.
			eprintf(E_WARN, EFMT("Unknown key(|%s|)"), keyName);

			iError = -1;
			continue;
		}

		// 주어진 이름(funcName)의 오브젝트가 있다.
		// 오브젝트(val)가 함수인지 확인한다.
		if (!PyFunction_Check(val)) {
			eprintf(E_ERR, EFMT("Key:= |%s|, Val:= |%O|: Not a function"), keyName, val);

			iError = -1;
			continue;
		}
	}
	if (iError < 0) {
		// 주어진 funcCode 에 어떤 문제가 있다.
		break;
	}

	// 등록된 함수의 코드 오브젝트를 얻는다.
	PyObject*	funcRef = PyDict_GetItemString(tmpDict, funcName);	// Borrowed ref.
	if (!funcRef) {
		eprintf(E_ERR, EFMT("PyDict_GetItemString(|%O|, |%s|): Failure"), tmpDict, funcName);

		break;
	}

	resultObject = PyObject_CallObject(funcRef, (PyObject*) argObject);	// New ref.
	if (!resultObject) {
		eprintf(E_ERR, EFMT("PyObject_CallObject(|%O|, |%O|): Failure"), funcRef, argObject);

		break;
	}
} while (0);
	Py_XDECREF(tmpModule); tmpModule = NULL;

	return resultObject;
}

PyObject*
CPythonWrapper::__pyRegisterObject(
	const char*	objName,
	const char*	objCode,
	int		overwrite,
	int		isFunction
)
{
	PyObject*	objRef = NULL;		// 이 값이 NULL이면 어디에선가 문제가 있는 것이다.

	PyObject*	tmpModule = NULL;	// 제일 마지막에 이 값이 NULL이 아니라면 Py_DECREF를 호출해야 한다.
do {
	// 같은 이름의 오브젝트(변수 또는 함수)가 등록되었는 지 알아본다.
	PyObject*	objPrev = PyDict_GetItemString(this->m__globalDict, objName);	// Borrowed ref.
	if (objPrev) {
		if (!overwrite) {
			eprintf(E_ERR, EFMT("|%s|: Already registered as |%O|"), objName, objPrev);

			break;
		}
	}

	// 등록하기 전에 주어진 함수의 정의(objCode)에 주어진 이름(objName)의 함수만이 존재하는 지 확인한다.
	// 새로운 임시 모듈을 만들고 그 안에서 테스트한다.
	const char	tmpModuleName[] = "tmpModule";
	tmpModule = PyModule_New(tmpModuleName);		// New ref.
	if (!tmpModule) {
		eprintf(E_ERR, EFMT("PyModule_New(|%s|): Failure"), tmpModuleName);

		break;
	}
	PyObject*	tmpDict;
	tmpDict = PyModule_GetDict(tmpModule);	// Borrowed ref.
	if (!tmpDict) {
		eprintf(E_ERR, EFMT("PyModule_GetDict(|%O|): Failure"), tmpModule);

		break;
	}
	// 새로운 모듈에는 두 개의 오브젝트(변수)("__name__": "...", "__doc__": "")만이 존재한다.
	// 여기에 주어진 함수를 등록해서 함수의 이름이 일치하는 지,
	// 또 다른 오브젝트는 있지 않은 지 확인한다.
	// global값을 "__main__"에 있는 것으로 설정하지 않으면,
	// "__main__"에 있는 "__builtins__"가 제공하는 것을 사용할 수 없다: dict, list, set, ...
	PyObject*	codeObject = PyRun_String(objCode, Py_file_input, this->m__globalDict, tmpDict);	// New ref.
	if (!codeObject) {
		eprintf(E_ERR, EFMT("PyRun_String(|%s| in |%s|): Failure"), objCode, tmpModuleName);

		break;
	}
	// 제대로 등록되었는 지만 관심이 있으므로 여기서 바로 Py_DECREF를 호출한다.
	Py_DECREF(codeObject);

	// 어떤 오브젝트들이 등록되었는 지 확인한다.
	// 설명서(Python/C API Reference Manual)의 7.4.1 Dictionary Objects에 있는 예를 사용했다.
	PyObject*	key;
	PyObject*	val;
	Py_ssize_t	pos = 0;	// 처음에는 반드시 0으로 해야 한다라고 되어있다.
	int		iError = 0;	// 확인 중에 어떤 문제가 있다면 음수(-1)로 설정한다.
	while (PyDict_Next(tmpDict, &pos, &key, &val)) {	// key, val: Borrowed ref.
		// key 의 문자열 표현을 얻는다.
		PyObject*	key_Str = NULL;
		const char*	keyName = NULL;
		do {
			key_Str = PyObject_Str(key);
			if (!key_Str) {
				eprintf(E_ERR, EFMT("PyObject_Str(|%O|): Failure"), key);

				break;
			}
			keyName = PyString_AsString(key_Str);
			if (!keyName) {
				eprintf(E_ERR, EFMT("PyString_AsString(|%O|): Failure"), key_Str);

				break;
			}
		} while (0);
		Py_XDECREF(key_Str); key_Str = NULL;
		if (!keyName) {
			iError = -1;
			continue;
		}

		// "__name__", "__doc__"은 제외한다.
		if (
			!strcmp(keyName, "__name__") ||		// "__name__"
			!strcmp(keyName, "__doc__")		// "__doc__"
		) {
			continue;
		}

		if (strcmp(keyName, objName)) {
			// 주어진 Python 코드(funcCode) 안에 또다른 이름의 오브젝트가 있다.
			eprintf(E_WARN, EFMT("Unknown key(|%s|)"), keyName);

			iError = -1;
			continue;
		}

		// 등록하려는 오브젝트가 함수가 아니라면 다음 값으로 넘어간다.
		if (!isFunction) {
			continue;
		}

		// 주어진 이름(objName)의 함수가 있다.
		// 오브젝트(val)가 함수인지 확인한다.
		if (!PyFunction_Check(val)) {
			eprintf(E_ERR, EFMT("Key:= |%s|, Val:= |%O|: Not a function"), keyName, val);

			iError = -1;
			continue;
		}
	}
	if (iError < 0) {
		// 주어진 objCode 에 어떤 문제가 있다.
		break;
	}

	// "__main__" 모듈에 등록한다.
	codeObject = PyRun_String(objCode, Py_file_input, this->m__globalDict, this->m__globalDict);	// New ref.
	if (!codeObject) {
		eprintf(E_ERR, EFMT("PyRun_String(|%s| in |%s|): Failure"), objCode, "__main__");

		break;
	}
	// 더 이상 사용하지 않는다.
	Py_DECREF(codeObject);

	// 등록된 오즈젝트에 대한 참조를 얻는다.
	objRef = PyDict_GetItemString(this->m__globalDict, objName);	// Borrowed ref.
	if (!objRef) {
		eprintf(E_ERR, EFMT("PyDict_GetItemString(|%O|, |%s|): Failure"), this->m__globalDict, objName);

		break;
	}
} while (0);
	Py_XDECREF(tmpModule); tmpModule = NULL;

	return objRef;
}

const PyObject*
CPythonWrapper::__pySetObjectFromString(
	const char*	pyVarExpr
)
{
	PyObject*	objBlob = NULL;

	PyObject*	objModule = NULL;
do {
	objModule = PyModule_New("hello, world");		// New reference
	if (!objModule) {
		(void) fprintf(stderr,
			"ERROR: __FILE__:= |%s|, __LINE__ = %d: "
			"PyModule_New(|hello, world|): Failure\n"
			,
			__FILE__, __LINE__
		);
		if (PyErr_Occurred())
			PyErr_Print();

		break;
	}
	PyObject*	objDict;
	objDict = PyModule_GetDict(objModule);	// Borrowed reference
						// ...'This function never fails.'...
	objBlob = PyRun_String(pyVarExpr, Py_eval_input, this->m__globalDict, this->m__globalDict);
			// New reference
	if (!objBlob) {
		(void) fprintf(stderr,
			"ERROR: __FILE__:= |%s|, __LINE__ = %d: "
			"PyRun_String(|%s|): Failure\n"
			,
			__FILE__, __LINE__, pyVarExpr
		);
		if (PyErr_Occurred())
			PyErr_Print();

		break;
	}
} while (0);
	Py_XDECREF(objModule); objModule = NULL;

	return objBlob;
}


int
CPythonWrapper::__evsprintf(
	CBuffer&	msgBuffer,
	const char*	format,
	...
)
{
	int	size = 80;
	while (1) {
		if (!msgBuffer.setBufSizeAtLeast(size)) {
			eprintf(E_FATAL, EFMT("CBuffer::setBufSizeAtLeast(80): Failure"));
			return -1;
		}

		va_list	ap;
		va_start(ap, format);
		char*	p = msgBuffer.getBufForNewData();
		errno = 0;	// Clear errno
		int	n = vsnprintf(p, size, format, ap);
		int	lastErrorNo = errno;
		va_end(ap);
		if (lastErrorNo) {
			// vsnprintf 에서 어떤 문제가 있었다.
			perror("CWorker::__evsprintf: vnsprintf(): Failure");
			return -1;
		}
		if (n > -1 || n < size) {
			// Ok
			msgBuffer.incDataSize(n);
			break;
		}
// "man vsnprint" 에서...
		if (n > -1)		/* glibc 2.1 */
			size = n + 1;	/* precisely what is needed */
		else {			/* glibc 2.0 */
			// 에러가 생겼을 때와 어떻게 구별하나???
			size *= 2;	/* twice the old size */
		}
	}

	return 0;
}
int
CPythonWrapper::evsprintf(
	const char*	format,
	...
)
{
	int	iReturnVal = 0;
	const char*	fmt = format;
	// 이 함수를 시작하기 전에 발생한 에러에 대한 코드가 있다를 이를 기억한다.
	// format 중에서 "%m" 있을 때, 이 에러코드에 대한 문자열을 출력할 때 사용한다.
	int	errnum = errno;

#if 0
	if (__evsprintf(this->m__msgBuffer,
		"Error Level:= |%s|\n"
		,
		e_level == E_FATAL ? "FATAL" : (
		e_level == E_ERR ? "ERROR" : (
		e_level == E_WARN ? "WARNING" : (
		e_level == E_INFO ? "INFOMATION" : (
		e_level == E_TRACE ? "TRACE" : "UNKNOWN"
		))))
	) < 0) {
		(void) fprintf(stderr, "__evsprintf: Failure\n");
		iReturnVal = -1;
		break;
	}
#endif

	va_list	ap;
	va_start(ap, format);
	while (*fmt) {
		char	c = *fmt++;
		if (c != '%') {
			if (this->m__msgBuffer.addChar(c) < 0) {
				eprintf(E_FATAL, EFMT("CBuffer::addChar(|%c|): Failure"), c);
				iReturnVal = -1;
				break;
			}
			continue;
		}
		// '%'

		c = *fmt;
		if (c == '\0') {
			eprintf(E_WARN, EFMT("format:= |%s|: Last character is '%%': ???"));
			break;
		}
		++fmt;

		switch (c) {
		case '%':
			if (this->m__msgBuffer.addChar('%') < 0) {
				eprintf(E_FATAL, EFMT("CBuffer::addChar(|%%|): Failure"));
				iReturnVal = -1;
				break;
			}
			break;
		case 's':
			if (__evsprintf(this->m__msgBuffer, "%s", va_arg(ap, const char*)) < 0) {
				(void) fprintf(stderr, "__evsprintf: Failure\n");
				iReturnVal = -1;
				break;
			}
			break;
		case 'O':
{
	PyGILState_STATE	gstate = PyGILState_Ensure();

do {
	PyObject*	Str = PyObject_Str(va_arg(ap, PyObject*));
	if (!Str) {
		(void) fprintf(stderr, "PyObject_Str(): Failure\n");
		iReturnVal = -1;
		break;
	}
	const char*	Str_cstr = PyString_AsString(Str);
	if (!Str_cstr) {
		(void) fprintf(stderr, "PyString_AsString(): Failure\n");
		iReturnVal = -1;
		break;
	}
	if (__evsprintf(this->m__msgBuffer, "%s", Str_cstr) < 0) {
		(void) fprintf(stderr, "__evsprintf: Failure\n");
		iReturnVal = -1;
		break;
	}
} while (0);

	PyGILState_Release(gstate);
}
			break;
		case 'd':
			if (__evsprintf(this->m__msgBuffer, "%d", va_arg(ap, int)) < 0) {
				(void) fprintf(stderr, "__evsprintf: Failure\n");
				iReturnVal = -1;
				break;
			}
			break;
		case 'u':
			if (__evsprintf(this->m__msgBuffer, "%u", va_arg(ap, unsigned int)) < 0) {
				(void) fprintf(stderr, "__evsprintf: Failure\n");
				iReturnVal = -1;
				break;
			}
			break;
		case 'm':
{
	// GNU version:
	char	buf__NotUsed[10];
	char*	s = strerror_r(errnum, buf__NotUsed, 10);
	if (this->__evsprintf(this->m__msgBuffer, "%s", s) < 0) {
		(void) fprintf(stderr, "__evsprintf: Failure\n");
		iReturnVal = -1;
	}
	break;
}
		default:
			eprintf(E_WARN, EFMT("Unknown format character: |%c|: Ignored\n"), c);
			break;
		}
		if (iReturnVal < 0)
			break;
	}
	va_end(ap);

	// 제일 마지막에 '\0' 을 넣는다.
	if (this->m__msgBuffer.addChar('\0') < 0) {
		eprintf(E_FATAL, EFMT("CBuffer::addChar(|NUL|): Failure"));
		iReturnVal = -1;
	}
	return iReturnVal;
}
