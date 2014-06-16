#include <Python.h>
#include "CFile.h"
#include "CPythonEngine.h"

const char*	CPythonEngine::WORKER_MODULE_PATH = "./WorkerModule/";
const char*	CPythonEngine::WORKER_SCRIPT_PATH = "./WorkerScript/";


CPythonEngine::CPythonEngine(
	void
)
{
}

CPythonEngine::~CPythonEngine()
{
}


PyObject*
CPythonEngine::__pyAddVar(
	const char*	varName,
	PyObject*	varValue
)
{
	// main모듈에 변수를 설정한다.
	// varValue에 대한 소유권이 넘어간다.
	if (PyModule_AddObject(m__mainModule, varName, varValue)) {
		(void) fprintf(stderr,
			"__TRACE__: CPythonEngine::__pyAddVar(\"__main__\"::\"%s\"): PyModule_AddObject(\"%s\"): Failure\n"
			,
			varName, varName
		);

		return (PyObject*) -1;
	}
	// varValue에 대한 소유권은 Python으로 넘어갔다.
	// 여기서 의문이, valValue대한 참조를 여전히 유효한가이다.
	// 다시 한 번 읽어 확인해보자.
	PyObject*	objValue = PyDict_GetItemString(m__globalDict, varName);	// Borrowed reference
	// 이것이 varValue와 같느냐 하는 것이다.
	// 몇 개를 확인해보니 그렇다. 하지만 그렇다는 문서가 없으니
	// 이렇게 새로 얻은 것을 Python에 설정한 변수의 값에 해당하는 오브젝트라고 기억하자.
	return objValue;
}
PyObject*
CPythonEngine::getVarWithOpen(
	const char*	varName,
	const char*	funcName,
	const char*	funcScriptPathname,
	const PyObject*	pArg,
	int		overwrite
)
{
	// 해당 파일을 연다.
	CFile	scriptFile;
	char	fullPathname[128];
	(void) snprintf(fullPathname, 128, "%s/%s", WORKER_SCRIPT_PATH, funcScriptPathname);
	if (scriptFile.openFile(fullPathname, "r", 0) < 0) {
		(void) fprintf(stderr,
			"__TRACE__: CPythonEngine::getVarWithOpen: CFile::openFile(\"%s\", \"r\", 0): Failure\n"
			,
			funcScriptPathname
		);

		return NULL;
	}
	// 전체를 읽는다.
	if (scriptFile.readlines() <= 0) {
		(void) fprintf(stderr,
			"__TRACE__: CPythonEngine::getVarWithOpen: CFile::readlines(\"%s\"): Error or no data\n"
			,
			funcScriptPathname
		);

		return NULL;
	}

	const CBuffer&	scriptBuffer = *(scriptFile.getBuffer());
//	(void) printf("%s", scriptBuffer.getBuf());	// 스트립트의 내용
	return this->__pyGetVar(varName, funcName, scriptBuffer.getBuf(), pArg, overwrite);
}
PyObject*
CPythonEngine::__pyGetVar(
	const char*	varName,
	const char*	funcName,
	const char*	funcScript,
	const PyObject*	pArg,
	int		overwrite
)
{
	// varName 이 __main__ 모듈에 이미 등록되었는 지 알아본다.
	// 이미 등록되었다면 overwrite = 0 이면 에러로 처리한다.
	if (PyDict_GetItemString(this->m__globalDict, varName) && !overwrite) {
		(void) fprintf(stderr,
			"__TRACE__: CPythonEngine::__pyGetVar: %s: Already registered but overwrite = False\n"
			,
			varName
		);
		if (PyErr_Occurred())
			PyErr_Print();

		return NULL;
	}

	// 아무것도 없는 임시 모듈에서, 스트립트에 정의된 함수를 호출한다.
	PyObject*	objVar = this->__pyGetResult(funcName, funcScript, pArg);
	if (!objVar) {
		(void) fprintf(stderr,
			"__TRACE__: CPythonEngine::__pyGetVar: CPythonWrapper::__pyGetResult(\"%s\"): Failure\n"
			,
			funcName
		);
		if (PyErr_Occurred())
			PyErr_Print();

		return NULL;
	}
	// objVar: New reference

	// __main__ 모듈에 varName 이라는 변수로 등록한다.
	PyObject*	objSavedVar = this->__pyAddVar(varName, objVar);
	if (objSavedVar == (PyObject*) -1) {
		(void) fprintf(stderr,
			"__TRACE__: ERROR: CPythonEngine::__pyGetVar: CPythonEngine::addVar(\"%s\"): Failure\n"
			,
			varName
		);
		if (PyErr_Occurred())
			PyErr_Print();

		// 여전히 objVar에 대한 소유권은 이 함수에 남아있다.
		Py_DECREF(objVar);	// 해제한다.
		return NULL;
	}
	// 소유권은 Python으로 넘어갔다.
	// 하지만 다시 읽어올 때 그 결과가 objVar와 다를 수 있는가가 의문이다.
	// 확실한 경우은 objVar 와 같은 경우만을 고려한다!
	if (objSavedVar == NULL) {
		(void) fprintf(stderr,
			"__TRACE__: WARNING: CPythonEngine::__pyGetVar: CPythonEngine::addVar(\"%s\"): Cannot re-get...\n"
			,
			varName
		);
		if (PyErr_Occurred())
			PyErr_Print();

		return NULL;
	}
	if (objSavedVar != objVar) {
		(void) fprintf(stderr,
			"__TRACE__: WARNING: CPythonEngine::__pyGetVar: CPythonEngine::addVar(\"%s\"): Mis-match...\n"
			,
			varName
		);

		return NULL;
	}

	return objSavedVar;	// objVar 와 동일하다 대신에 이번에는 borrowed referece 이다.
}

PyObject*
CPythonEngine::setFuncWithOpen(
	const char*	funcName,
	const char*	funcScriptPathname,
	int		overwrite
)
{
	// 해당 파일을 연다.
	CFile	scriptFile;
	char	fullPathname[128];
	(void) snprintf(fullPathname, 128, "%s/%s", WORKER_SCRIPT_PATH, funcScriptPathname);
	int	isPipe = 0;
	if (scriptFile.openFile(fullPathname, "r", isPipe) < 0) {
		(void) fprintf(stderr,
			"ERROR: "
			"__FILE__:= |%s|, __LINE_NO__ = %d: "
			"CPythonEngine::setFuncWithOpen(|%s|): "
			"CFile::openFile(|%s|, |r|, %s): Failure\n"
			,
			__FILE__, __LINE__, funcName, funcScriptPathname, isPipe ? "True" : "False"
		);

		return NULL;
	}
	// 전체를 읽는다.
	if (scriptFile.readlines() <= 0) {
		(void) fprintf(stderr,
			"ERROR: "
			"__FILE__:= |%s|, __LINE_NO__ = %d: "
			"CPythonEngine::setFuncWithOpen(|%s|): CFile::readlines(|%s|): Error or no data\n"
			,
			__FILE__, __LINE__, funcName, funcScriptPathname
		);

		return NULL;
	}

	const CBuffer&	scriptBuffer = *(scriptFile.getBuffer());
//	(void) printf("%s", scriptBuffer.getBuf());	// 스트립트의 내용
	return this->pySetFunc(funcName, scriptBuffer.getBuf(), overwrite);
}
PyObject*
CPythonEngine::pySetFunc(
	const char*	funcName,
	const char*	funcScript,
	int		overwrite
)
{
	PyGILState_STATE	gstate = PyGILState_Ensure();
	PyObject*	obj = this->__pySetFunc(funcName, funcScript, overwrite);
	PyGILState_Release(gstate);
	return obj;
}
PyObject*
CPythonEngine::__pySetFunc(
	const char*	funcName,
	const char*	funcScript,
	int		overwrite
)
{
	int	isFunction	= 1;	// 등록하려는 오브젝트가 함수이다.
	PyObject*	funcRef = this->__pyRegisterObject(funcName, funcScript, overwrite, isFunction);
	if (!funcRef) {
		(void) fprintf(stderr,
			"ERROR: "
			"__FILE__:= |%s|, __LINE_NO__ = %d: "
			"CPythonEngine::__pySetFunc(|%s|): CPythonEngine::__pyRegisterObject(): Failure\n"
			,
			__FILE__, __LINE__, funcName
		);

		return NULL;
	}

	return funcRef;
}

int
CPythonEngine::__pyDoTypeBlob(
	PyObject*	typeBlob,
	PyObject*	scriptName,
	PyObject*	scriptSource,
	int		overwrite
)
{
	assert(PyTuple_CheckExact(typeBlob));
	const char*	strScriptName = PyString_AsString(scriptName); assert(strScriptName);
	const char*	strScriptSource = PyString_AsString(scriptSource); assert(strScriptSource);
	PyObject*	firstArg = PyTuple_GetItem(typeBlob, 0); assert(firstArg);
	if (firstArg == Py_None) {	// [경우 1] 또는 [경우 2]
		if (PyTuple_Size(typeBlob) == 2) {	// [경우 2]
			// scriptSource:= '{...}';
			// varName = eval(scriptSource)
			PyObject*	varName = PyTuple_GetItem(typeBlob, 1); assert(varName);
			PyObject*	val = PyRun_String(
				strScriptSource,
				Py_eval_input,
				this->m__globalDict,
				this->m__globalDict
			);	// New reference
			if (!val) {
				(void) fprintf(stderr,
					"ERROR: "
					"__FILE__:= |%s|, __LINE_NO__ = %d: "
					"CWorker::__pyDoTypeBlob: "
					"PyRun_String(|%s|)\n"
					,
					__FILE__, __LINE__, strScriptSource
				);

				if (PyErr_Occurred())
					PyErr_Print();

				assert(0);
			}
			assert(PyDict_SetItem(this->m__globalDict, varName, val) == 0);
			Py_DECREF(val);
		} else {				// [경우 1]
			// Source:= "def..."
			// eval(compile(scriptSource, 'as |' + scriptName + '|', 'exec'))
			assert(this->__pySetFunc(strScriptName, strScriptSource, overwrite));
		}
	} else {			// [경우 3] 또는 [경우 4]
		// 크기가 2 이고 두 번째가 다시 Tuple 인 경우라면 [경우 3],
		// 그렇지 않다면 [경우 4]
		PyObject*	arg = NULL;
		if (PyTuple_Size(typeBlob) == 2) {
			arg = PyTuple_GetItem(typeBlob, 1); assert(arg);
		}
		if (arg && PyTuple_CheckExact(arg)) {	// [경우 3]
			// Source:= 'def...'
			// Source 에 정의된 함수를 호출한다.
			// 함수의 이름은 scriptName 이다.
			// typeBlob 의 첫 번째는 호출된 결과를 저장할 변수의 이름이다.
			// 함수를 호출할 때 사용하는 인자(argument)가 두 번째의 Tuple 인 arg 이다.
			// varName = eval(compile(scriptSource, 'as |' + scriptName + '|', 'exec'))
			const char*	varName = PyString_AsString(firstArg); assert(varName);
			assert(this->__pyGetVar(varName, strScriptName, strScriptSource, arg, overwrite));
		} else {				// [경우 4]
			// Source:= '...'
			// typeBlob 의 첫 번째에 해당하는 함수를 호출한다.
			// 리턴값은 사용하지 않는다.
			// typeBlob 의 첫 번째를 Source 로 바꿔치기해서 만들어지는 Tuple 을 인자로 사용한다.
			// Source 에 해당하는 변수를 만든다.
			PyObject*	arg1 = PyRun_String(
				strScriptSource,
				Py_eval_input,
				this->m__globalDict,
				this->m__globalDict
			);	// New reference
			if (!arg1) {
				(void) fprintf(stderr,
					"ERROR: "
					"__FILE__:= |%s|, __LINE_NO__ = %d: "
					"CWorker::__pyDoTypeBlob: "
					"PyRun_String(|%s|)\n"
					,
					__FILE__, __LINE__, strScriptSource
				);

				if (PyErr_Occurred())
					PyErr_Print();

				assert(0);
			}
			Py_INCREF(firstArg);	// 원본을 유지해야 하므로, referece count 를 1 증가시킨다.
			// 바꿔치기한다.
			assert(PyTuple_SetItem(typeBlob, 0, arg1) == 0);
			// firstArg 의 reference count 는 1 줄어든다.
			// 하지만 앞에서 1 증가시켰으므로 여전히 firstArg 는 살아있다.
			// Source 로부터 만든 변수 arg1 은 새로운 오브젝트이나,
			// Tuple 에 들어가면서(steal) 소유권은 해당 Tuple 로 넘어간다.
			// 따라서 이후에 신경쓰지 않아도 된다.
			PyObject*	func = PyDict_GetItem(this->m__globalDict, firstArg); assert(func);
			PyObject*	result = PyObject_CallObject(func, typeBlob);	// New reference
			if (!result) {
				(void) fprintf(stderr,
					"ERROR: "
					"__FILE__:= |%s|, __LINE_NO__ = %d: "
					"CWorker::__pyDoTypeBlob: "
					"PyObject_CallObject(|"
					,
					__FILE__, __LINE__
				);
				(void) PyObject_Print(func, stderr, 0);
				(void) fprintf(stderr, "|, |");
				(void) PyObject_Print(typeBlob, stderr, 0);
				(void) fprintf(stderr, "|: Failure\n");

				if (PyErr_Occurred())
					PyErr_Print();
			} else {
				// 무엇을 리턴했는 지 보자.
#if 0	// 확인
				(void) fprintf(stderr, "(|"); (void) PyObject_Print(firstArg, stderr, 0);
				(void) fprintf(stderr, "|)(...) = |"); (void) PyObject_Print(result, stderr, 0);
				(void) fprintf(stderr, "|\n");
#endif
				Py_DECREF(result);	// 더 이상 사용하지 않으므로 여기서 파괴한다.
			}
			// 원래대로 되돌린다.
			// firstArg 의 소유권은 Tuple 로 넘어간다.
			assert(PyTuple_SetItem(typeBlob, 0, firstArg) == 0);
		}
	}

	return 0;
}

void
CPythonEngine::__pyEvalAll(
	PyObject*	dictByID,
	PyObject*	id
)
{
	int	overwrite = 1;
#if 1
	PyObject*	valDict = PyDict_GetItem(dictByID, id); assert(valDict);

	PyObject*	done = PyDict_GetItemString(valDict, "EvalDone"); assert(done);
	if (done == Py_True)
		return;

	PyObject*	reqIDSet = PyDict_GetItemString(valDict, "ReqIDSet"); assert(reqIDSet);
	// 복사해서 사용한다.
	// 다른 방법은???
	PyObject*	reqSet = PySet_New(reqIDSet); assert(reqSet);	// New reference
	Py_ssize_t	n = PySet_Size(reqSet);
	for (Py_ssize_t i = 0; i < n; ++i) {
		PyObject*	id_ = PySet_Pop(reqSet); assert(id_);	// New reference
		__pyEvalAll(dictByID, id_);
		Py_DECREF(id_);						// Destroy
	}
	Py_DECREF(reqSet);						// Destroy

	PyObject*	typeBlob = PyDict_GetItemString(valDict, "TypeBlob"); assert(typeBlob);
	PyObject*	scriptName = PyDict_GetItemString(valDict, "Name"); assert(scriptName);
	PyObject*	scriptSource = PyDict_GetItemString(valDict, "Source"); assert(scriptSource);
	if (this->__pyDoTypeBlob(typeBlob, scriptName, scriptSource, overwrite) < 0)
		return;

//	(void) fprintf(stderr, "|%s|: Evaluate Done\n", strScriptName);
	assert(PyDict_SetItemString(valDict, "EvalDone", Py_True) == 0);
#endif
}

void
CPythonEngine::__pyEvalAll(
	PyObject*	dictByID
)
{
	PyObject*	keys = PyDict_Keys(dictByID); assert(keys);
	Py_ssize_t	n = PyList_Size(keys);
	for (Py_ssize_t i = 0; i < n; ++i) {
		PyObject*	keyID = PyList_GetItem(keys, i); assert(keyID);
		__pyEvalAll(dictByID, keyID);
	}
	Py_DECREF(keys);
}
