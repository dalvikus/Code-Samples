#ifndef __PYTHON_ENGINE_H__
#define __PYTHON_ENGINE_H__
////////////////////////////////////////////////////////////////////////////////


#include <Python.h>
#include "CBuffer.h"
#include "CPythonWrapper.h"
#include "CWorkerModule.h"

class CPythonEngine : public CPythonWrapper
{
public:
	CPythonEngine(void);
	virtual ~CPythonEngine();


	// 여러 명령어를 구현한다.
public:
	virtual void	help(void) = 0;
	virtual void	pyListAllObjects(void) = 0;

public:
	// 각 스크립트가 들어가 있는 디렉토리 (테스트할 때만 사용된다)
	static const char*	WORKER_SCRIPT_PATH;
	// 각 모듈이 들어갈 디렉토리
	static const char*	WORKER_MODULE_PATH;
	// 동적으로 사용하는 라이브러리(lib###.so)의 내용을 디스크에 저장한다.
	// 파일이름: WORKER_MODULE_PATH + "/lib" + entryPoint + ".so"

private:
	// 새로 만들거나 얻은 오브젝트를 특정변수의 값으로 설정한다.
	// 이 때 소유권은 Python으로 넘어간다.
	// 단, 처름에 사용한 오브젝트의 참조는 여전히 유효한지가 의문이다.
	// 따라서 설정한 후 다시 읽어 해당 참조를 얻은 값을 리턴한다.
	// 몇 개를 테스트해보니 답은 '그렇다'이나 명백한 문서는 없다.
	// 리턴값:
	//	(PyObject*) -1: 설정할 때 문제가 있다. varValue의 소유권은 그대로 이므로, 알아서 해제해야 한다.
	//	NULL: 다시 얻을 때 문제가 있다.
	//	!= varValue: 소유권뿐만 아니라 참조 자체도 넘어갔다. (이럴 수가...)
	// (PyObject*) -1을 제외하고 모두 소유권이 넘어간다.
	PyObject*	__pyAddVar(const char* varName, PyObject* varValue);
private:
	// funcScript 에 정의된 함수가 리턴하는 값으로,
	// __main__ 모듈에 varName 을 설정한다.
	// 그 오브젝트에 대한 참조(borrowed reference)를 리턴한다.
	// 그 함수는 borrwed reference 인 pArg 를 매개변수로 사용한다.
	// overwrite = 0이고, 이미 함수가 등록되었다면 역시 NULL 값을 리턴한다.
	PyObject*	__pyGetVar(const char* varName, const char* funcName, const char* funcScript, const PyObject* pArg, int overwrite);
public:
	// 함수가 정의된 파일을 열어 그 내용을 가지고 getVar 를 호출한다.
	PyObject*	getVarWithOpen(const char* varName, const char* funcName, const char* funcScriptPathname, const PyObject* pArg, int overwrite);

private:
	// 함수의 정의를 담은 funcScript 를 이용해서 funcName 함수를 등록하고,
	// 그 함수에 대한 참조(borrowed reference)를 돌려준다.
	// 정상적으로 등록되지 않았다면 NULL 을 돌려준다.
	// overwrite = 0이고, 이미 함수가 등록되었다면 역시 NULL 값을 리턴한다.
	PyObject*	__pySetFunc(const char* funcName, const char* funcScript, int overwrite);
public:
	// Python locking 을 가진 __pySetFunc 의 wrapper
	PyObject*	pySetFunc(const char* funcName, const char* funcScript, int overwrite);
public:
	// CPythonWrapper::registerObject(..., isFunction = True) 의 wrapper 이다.
	// 함수가 정의된 파일을 열어 그 내용을 가지고 setArgvFunction 을 호출한다.
	PyObject*	setFuncWithOpen(const char* funcName, const char* funcScriptPathname, int overwrite);

	// 주어진 Script 소스를 TypeBlob 에 따라 적절하게 처리한다.
/*
 * 네 종류가 있다.
	[경우 1] 주어진 이름의 함수를 등록한다.
		(None,):= 주어진 이름의 함수를 등록
	[경우 2] Script 의 내용을 인자로 사용하여 특정 함수를 호출하는 경우
		(None, 'varName'):= Script 내용을 특정 변수(varName)로 설정
	[경우 3] 주어진 이름의 함수를 실행하여 그 결과를 주어진 이름의 변수로 설정한다.
		('hSysInfo',()):= 주어진 이름의 함수를 실행하여, 그 결과를 hSysInfo 에 넣는다.
		이 때 인자는 () 를 사용하여 함수를 호출한다. 없더라도 반드시 ()은 사용해야 한다.
	[경우 4] Script 의 내용을 특정 변수로 설정한다.
		('setJob',):= Script 의 내용을 인자로 사용하여 "setJob" 이라는 함수를 호출한다.
		이 때 추가적으로 사용하는 인자가 있을 수도 있다.
		('setJob', 'a', 'b') 라면 'a', 'b' 가 추가인자로 사용된다.
 */
protected:
	// __eval 에서 recursive 하게 호출되므로 PyGILState_Ensure/PyGILState_Ensure 로 묶지 않았다.
	// 따라서 외부에서 직접 호출하지 말아야 한다.
	// 대신 PyDoTypeBlob 을 호출한다.
	int	__pyDoTypeBlob(PyObject* typeBlob, PyObject* scriptName, PyObject* scriptSource, int overwrite);

private:
	// Recursive 하게 호출되므로 PyGILState_Ensure/PyGILState_Release 를 생략했다.
	// 따라서 외부에서 직접 호출하지 말아야한다. (private!!!)
	// evalAll 을 통해서만 호출해야한다.
	void	__pyEvalAll(PyObject* dictByID, PyObject* id);
protected:
	void	__pyEvalAll(PyObject* dictByID);
};


////////////////////////////////////////////////////////////////////////////////
#endif	// __PYTHON_ENGINE_H__
