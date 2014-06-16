#ifndef __PYTHON_WRAPPER_H__
#define __PYTHON_WRAPPER_H__
////////////////////////////////////////////////////////////////////////////////


#include <Python.h>
#include <stdarg.h>
#include "eprintf.h"

enum {
	False	= 0,
	True	= 1,
};

class CPythonWrapper {
protected:
	PyObject*	m__mainModule;
	PyObject*	m__globalDict;
public:
	PyObject*	mainModule(void)	{return m__mainModule;}
	PyObject*	globalDict(void)	{return m__globalDict;}
	const PyObject*	globalDict(void) const	{return m__globalDict;}

public:
	CPythonWrapper(void);
	virtual ~CPythonWrapper();

protected:
	// Python함수를 등록한 후, 컴파일된 코드 오브젝트에 대한 참조(borrowed reference)를 얻는다.
	// overwrite는 같은 이름의 오브젝트가 있을 때 어떻게 할 지를 말해준다.
	// 처음으로 등록할 때는 사용되지 않는다.
	// 제대로 등록했을 때는 해당 코드 오브젝트에 대한 참조(Borrowed Reference)를 되돌려준다.
	// 그렇지 않을 때는 NULL(0)을 되돌려준다. 다음은 문제가 있는 상황이다.
	//	0) Python함수를 호출할 때 문제가 발생했다.
	//	1) 여기서 정한 함수이름과 코드안에 있는 함수 이름이 다른다.
	//	2) 코드안에 함수외에 다른 오브젝트(변수 또는 함수)가 있다.
	//	3) 같은 이름의 오브젝트(변수 또는 함수)가 이미 등록이 되어있는 데,
	//	overwrite가 1이 아니다.
	// overwrite를 항상 1로 하면 될 것 같으나 확인을 위해서 0을 사용하도록 했다.
	PyObject*	__pyRegisterObject(const char* funcName, const char* funcCode, int overwrite, int isFunction);

protected:
	// Python 함수에 대한 정의를 담은 Script 소스(funcCode)를 임시 모듈 안에 등록한 후,
	// 그 함수를 실행한 결과에 대한 Python 오브젝트(new ref.)를 리턴한다.
	// NULL이 아닌 결과가 리터되면 이 함수를 호출한 곳에서 알아서 처리해야 한다;
	// 사용 후에는 Py_DECREF 를 호출하여 그 오브젝터를 삭제해야 한다.
	PyObject*	__pyGetResult(const char* funcName, const char* funcCode, const PyObject* argObject);

protected:
	// Python 변수표현으로부터 Python 오브젝트를 만든다.
	// 함수도 이럴 수 있으면 좋겠구만...
	const PyObject*	__pySetObjectFromString(const char* pyVarExpr);	// New reference


protected:
	CBuffer	m__msgBuffer;
public:
	const CBuffer*	getMsgBuffer(void) const	{return &this->m__msgBuffer;}
	CBuffer*	getMsgBuffer(void)		{return &this->m__msgBuffer;}
public:
//	void eprintf(E_LEVEL e_level, const char* const	format, ...);
	int __evsprintf(CBuffer& msgBuffer, const char* format, ...);
	int evsprintf(const char* format, ...);
};


////////////////////////////////////////////////////////////////////////////////
#endif	// !__PYTHON_WRAPPER_H__
