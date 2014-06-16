#include "CJobDownPacket.h"
#include "CPacket.h"

int
CJobDownPacket::pyPack(
	CBuffer&	packet,		// 여기에 넣는다.

	const PyObject*	jobID,
	JOB_DOWN_STAGE_PACKET_T	jobDownStage,

	int	isBinary,
	const PyObject*	what,
	const PyObject*	extraWhat
)
{
	int	iReturnVal = 0;

	unsigned int	jobID__NUM;
	unsigned int	stage;
	const char*	argPtr = NULL;
	Py_ssize_t	argLen = 0;
	const char*	extraArgPtr = NULL;
	PyGILState_STATE	gstate = PyGILState_Ensure();
do {
	PyErr_Clear();
	jobID__NUM = PyLong_AsUnsignedLong((PyObject*) jobID);
	if (PyErr_Occurred()) {
		PyErr_Print();
		assert(0);
		iReturnVal = -1;
		break;
	}
	stage = jobDownStage << BYTE_JOB_DOWN__STAGE_SHIFT;
	assert(what);
	if (PyString_AsStringAndSize((PyObject*) what, (char**) &argPtr, &argLen) < 0) {
		if (PyErr_Occurred())
			PyErr_Print();
		assert(0);
		iReturnVal = -1;
		break;
	}
	if (!isBinary) {	// 일반적인 문자열이라면, 패킹할 바이트의 길이에 마지막의 NUL 도 포함한다.
				// CPacket::packOrder 를 참고한다.
		++argLen;
	}
	if (!extraWhat)
		extraArgPtr = NULL;
	else {
		extraArgPtr = PyString_AsString((PyObject*) extraWhat);
		if (!extraArgPtr) {
			if (PyErr_Occurred())
				PyErr_Print();
			assert(0);
			iReturnVal = -1;
			break;
		}
	}
} while(0);
	PyGILState_Release(gstate);
	if (iReturnVal < 0)
		return -1;

	return CPacket::packOrder(
		packet,
		CPacket::ORD_TYPE__JOB_DOWN_CONTROL,
		jobID__NUM,
		&stage,
		isBinary,
		argPtr,
		argLen,
		extraArgPtr
	);
}
