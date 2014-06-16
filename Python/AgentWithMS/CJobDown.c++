#include "CAgentSlot.h"
#include "CJobDown.h"
#include "CJobDownPacket.h"
#include "CMgmtSrv.h"
#include "CPacket.h"

const char*	CJobDown::VAR__SCRIPT2_BY_ID		= "hScript2ByID";
const char*	CJobDown::SCRIPT__KEY_NAME__NAME		= "Name";
const char*	CJobDown::SCRIPT__KEY_NAME__MD5			= "MD5";
const char*	CJobDown::SCRIPT__KEY_NAME__REQ_ID__SET		= "ReqIDSet";
const char*	CJobDown::SCRIPT__KEY_NAME__TYPE_BLOB		= "TypeBlob";
const char*	CJobDown::SCRIPT__KEY_NAME__SOURCE		= "Source";
const char*	CJobDown::VAR__BYTE_STREAM2_BY_ID	= "hByteStream2ByID";
const char*	CJobDown::BYTE_STREAM__KEY_NAME__ENTRY_POINT	= "Entry Point";
const char*	CJobDown::BYTE_STREAM__KEY_NAME__MD5		= "MD5";
const char*	CJobDown::BYTE_STREAM__KEY_NAME__NAME		= "Name";
const char*	CJobDown::BYTE_STREAM__KEY_NAME__BYTE_STREAM	= "Byte Stream";

const char*	CJobDown::MAKE_JOB_SCRIPT	= "mkJob";
const char*	CJobDown::MAKE_JOB_SCRIPT__KEY_NAME__REQUIRED_SCRIPT_ID__LIST	= "RequiredScriptIDList";
const char*	CJobDown::MAKE_JOB_SCRIPT__KEY_NAME__BYTE_STREAM_ID		= "ByteStreamID";
const char*	CJobDown::MAKE_JOB_SCRIPT__KEY_NAME__COMMAND			= "hCommand";
const char*	CJobDown::MAKE_JOB_SCRIPT__KEY_NAME__COMMAND__KEY_NAME__ID	= "CommandID";
const char*	CJobDown::MAKE_JOB_SCRIPT__KEY_NAME__JOB			= "hJob";

CJobDown::CJobDown(
	void
)
{
	PyGILState_STATE	gstate = PyGILState_Ensure();

while (1) {
	this->m__jobID = NULL;
	this->m__jobDownNextStage = JOB_DOWN_STAGE__READY;

	this->m__scriptID__LIST = NULL;
	this->m__byteStreamID = NULL;
	this->m__hCommand = NULL;
	this->m__hJob = NULL;

	// m__hJob2ByID, m__hCommand2ByID, m__hByteStream2ByID 를 얻는다. (모두 borrowed reference)
	// "__main__" 모듈에서...
	PyObject*	mainModule = PyImport_AddModule("__main__");	// Borrowed reference
	if (!mainModule) {
		if (PyErr_Occurred())
			PyErr_Print();
		assert(0);
		break;
	}
	PyObject*	globalDict = PyModule_GetDict(mainModule);	// Borrowed reference
	if (!globalDict) {
		if (PyErr_Occurred())
			PyErr_Print();
		assert(0);
		break;
	}
	this->m__globalDict = globalDict;

	this->m__hScript2ByID = PyDict_GetItemString(globalDict, VAR__SCRIPT2_BY_ID);
	if (!(this->m__hScript2ByID)) {
		if (PyErr_Occurred())
			PyErr_Print();
		assert(0);
		break;
	}
	this->m__hByteStream2ByID = PyDict_GetItemString(globalDict, VAR__BYTE_STREAM2_BY_ID);
	if (!(this->m__hByteStream2ByID)) {
		if (PyErr_Occurred())
			PyErr_Print();
		assert(0);
		break;
	}

	this->m__mkJob = PyDict_GetItemString(globalDict, MAKE_JOB_SCRIPT);	// Borrowed reference
	if (!(this->m__mkJob)) {
		if (PyErr_Occurred())
			PyErr_Print();
		assert(0);
		break;
	}

	break;
}

	PyGILState_Release(gstate);
}

CJobDown::~CJobDown()
{
	PyGILState_STATE	gstate = PyGILState_Ensure();

	if (this->m__scriptID__LIST) {
		Py_DECREF(this->m__scriptID__LIST);
		this->m__scriptID__LIST = NULL;
	}
	if (this->m__hCommand) {
		Py_DECREF(this->m__hCommand);
		this->m__hCommand = NULL;
	}
	if (this->m__hJob) {
		Py_DECREF(this->m__hJob);
		this->m__hJob = NULL;
	}

	PyGILState_Release(gstate);
}


int
CJobDown::pyUpdateJobNextStage(
	JOB_DOWN_STAGE_T	downDoneStage,
	const PyObject*		stageID
)
{
	int	iReturnVal = 0;
	PyGILState_STATE	gstate = PyGILState_Ensure();

	switch (downDoneStage) {
	case JOB_DOWN_STAGE__SCRIPT:	// stageID:= ScriptID
		// Script 을 내려보냈다.
		// 내려보낸 것들의 집합에 추가한다.
		if (PySet_Add(((CAgentSlot*) this)->getScriptID__SET(), (PyObject*) stageID) != 0) {
			(void) fprintf(stderr,
				"ERROR: "
				"__FILE__:= |%s|, __LINE_NO__ = %d: "
				"CJobDown::updateJobNextStage: JOB_DOWN_STAGE__SCRIPT: "
				"PySet_Add(|"
				,
				__FILE__, __LINE__
			); (void) PyObject_Print(((CAgentSlot*) this)->getScriptID__SET(), stderr, 0);
			(void) fprintf(stderr, "|, ID = |"); (void) PyObject_Print((PyObject*) stageID, stderr, 0);
			(void) fprintf(stderr, "|): Failure\n");

			if (PyErr_Occurred())
				PyErr_Print();

			iReturnVal = -1;
			break;
		}

		// 다음 Script 를 내려보낸다.
		if (PyList_Size(this->m__scriptID__LIST) == 0) {
			// 모든 Script 는 모두 내려보냈다.
			Py_DECREF(this->m__scriptID__LIST); this->m__scriptID__LIST = NULL;
			// 다음은...
			if (this->m__byteStreamID == NULL) {
				// 내려보내야할 Byte Stream 이 없다.
				this->m__jobDownNextStage = this->m__hCommand ? JOB_DOWN_STAGE__COMMAND : JOB_DOWN_STAGE__JOB;
			} else {
				// 내려보내야할 Byte Stream 이 있다.
				this->m__jobDownNextStage = JOB_DOWN_STAGE__BYTE_STREAM;
			}
		} else {
			// 다음 Script 를 내려보낸다.
			// this->m__jobDownNextStage 는 그대로 유지된다. (JOB_DOWN_STAGE__SCRIPT)
			// 단, Pending 상태만 해제한다.
//			this->m__jobDownNextStage &= ~JOB_DOWN_STAGE__PENDING;
			unsigned int	jobDownNextStage = (unsigned int) this->m__jobDownNextStage;
			jobDownNextStage &= ~JOB_DOWN_STAGE__PENDING;
			this->m__jobDownNextStage = (JOB_DOWN_STAGE_T) jobDownNextStage;
		}
		break;
	case JOB_DOWN_STAGE__BYTE_STREAM:	// stageID:= ByteStreamID
		// Byte Stream 을 내려보냈다. 
		// 내려보낸 것들의 집합에 추가한다. (stageID 와 m__byteStreamID 은 같다.)
		if (PySet_Add(((CAgentSlot*) this)->getByteStreamID__SET(), (PyObject*) stageID) != 0) {
			(void) fprintf(stderr,
				"ERROR: "
				"__FILE__:= |%s|, __LINE_NO__ = %d: "
				"CJobDown::updateJobNextStage: JOB_DOWN_STAGE__BYTE_STREAM: "
				"PySet_Add(|"
				,
				__FILE__, __LINE__
			); (void) PyObject_Print(((CAgentSlot*) this)->getByteStreamID__SET(), stderr, 0);
			(void) fprintf(stderr, "|, ID = |"); (void) PyObject_Print((PyObject*) stageID, stderr, 0);
			(void) fprintf(stderr, "|): Failure\n");

			if (PyErr_Occurred())
				PyErr_Print();

			iReturnVal = -1;
			break;
		}

		// m__byteStreamID 는 더 이상 사용하지 않는다.
		this->m__byteStreamID = NULL;

		// 다음은...
		this->m__jobDownNextStage = this->m__hCommand ? JOB_DOWN_STAGE__COMMAND : JOB_DOWN_STAGE__JOB;
		break;
	case JOB_DOWN_STAGE__COMMAND:		// stageID:= CommandID
		// Command 정의를 내려보냈다.
		// 내려보낸 Command 들의 집합에 추가한다.
		if (PySet_Add(((CAgentSlot*) this)->getCommandID__SET(), (PyObject*) stageID) != 0) {
			(void) fprintf(stderr,
				"ERROR: "
				"__FILE__:= |%s|, __LINE_NO__ = %d: "
				"CJobDown::updateJobNextStage: JOB_DOWN_STAGE__COMMAND: "
				"PySet_Add(|"
				,
				__FILE__, __LINE__
			); (void) PyObject_Print(((CAgentSlot*) this)->getCommandID__SET(), stderr, 0);
			(void) fprintf(stderr, "|, ID = |"); (void) PyObject_Print((PyObject*) stageID, stderr, 0);
			(void) fprintf(stderr, "|): Failure\n");

			if (PyErr_Occurred())
				PyErr_Print();

			iReturnVal = -1;
			break;
		}

		// m__hCommand 는 더 이상 사용하지 않는다.
		Py_DECREF(this->m__hCommand); this->m__hCommand = NULL;

		// 다음은 Job 정의를 내려보낸다.
		this->m__jobDownNextStage = JOB_DOWN_STAGE__JOB;
		break;
	case JOB_DOWN_STAGE__JOB:		// stageID:= JobID
		// Job 정의를 내려보냈다.
		// m__hJob 은 더 이상 사용하지 않는다.
		Py_DECREF(this->m__hJob); this->m__hJob = NULL;

		// 새로운 Job 에 대해서 반복한다.
		this->m__jobID = NULL;
		this->m__jobDownNextStage = JOB_DOWN_STAGE__READY;
		break;
	default:
		(void) fprintf(stderr,
			"ERROR: "
			"__FILE__:= |%s|, __LINE_NO__ = %d: "
			"CJobDown::updateJobNextStage: "
			"Stage ID = |"
			,
			__FILE__, __LINE__
		); (void) PyObject_Print((PyObject*) stageID, stderr, 0);
		(void) fprintf(stderr, "|\ndownDoneStage = |%d|: Invalid\n", downDoneStage);
		iReturnVal = -1;
	}

	PyGILState_Release(gstate);
	return iReturnVal;
}

int
CJobDown::__pyMakeJob(
	void
)
{
	// Order 가 있을 때에만 가능하다.
	if (PyList_Size(((CAgentSlot*) this)->getOrder__LIST()) == 0)
		return 0;

	// 처리되는 부분이 없을 때에만 가능하다.
	if (this->m__jobDownNextStage != JOB_DOWN_STAGE__READY)
		return 0;

	// ORD_TYPE__ADD_JOB 만 취급한다.
	PyObject*	order = PyList_GetItem(((CAgentSlot*) this)->getOrder__LIST(), 0); assert(order);
	PyObject*	type = PyDict_GetItemString(order, "Type"); assert(type);
	PyErr_Clear();
	long	longType = PyInt_AsLong(type);
	if (longType == -1 && PyErr_Occurred()) {
		assert(0);
	}
	if ((CPacket::ORD_TYPE_T) longType != CPacket::ORD_TYPE__ADD_JOB) {
		return 0;
	}

	PyObject*	jobID = PyDict_GetItemString(order, "ID"); assert(jobID);

	int	iReturnVal = 0;		// iReturnVal 가 0 이면 CAgentSlot::m__order__LIST 에서 첫 번째를 삭제한다.
					// 아니라면 문제가 발생한 것이므로 그대로 둔다.
	PyObject*	argv = NULL;
	PyObject*	result = NULL;
while (1) {
	argv = Py_BuildValue(
		"(O, O, O, O)"
		,
		((CAgentSlot*) this)->getScriptID__SET(),
		((CAgentSlot*) this)->getByteStreamID__SET(),
		((CAgentSlot*) this)->getCommandID__SET(),
		jobID
	);
	if (!argv) {
		(void) fprintf(stderr,
			"ERROR: "
			"__FILE__:= |%s|, __LINE_NO__ = %d: "
			"CJobDown::__pyMakeJob: "
			"Py_BuildValue(|(O)|, |"
			,
			__FILE__, __LINE__
		); (void) PyObject_Print(jobID, stderr, 0);
		(void) fprintf(stderr, "|): Failure\n");

		if (PyErr_Occurred())
			PyErr_Print();

		iReturnVal = -1;
		break;
	}
	result = PyObject_CallObject(this->m__mkJob, argv);
	if (!result) {
		(void) fprintf(stderr,
			"ERROR: "
			"__FILE__:= |%s|, __FILE_NO__ = %d: "
			"CJobDown::__pyMakeJob: "
			"PyObject_CallObject(|"
			,
			__FILE__, __LINE__
		); (void) PyObject_Print(this->m__mkJob, stderr, 0);
		(void) fprintf(stderr, "|, |");
		(void) PyObject_Print(argv, stderr, 0);
		(void) fprintf(stderr, "|): Failure\n");

		if (PyErr_Occurred())
			PyErr_Print();

		iReturnVal = -1;
		break;
	}

	(void) fprintf(stderr, "RESULT:= |"); (void) PyObject_Print(result, stderr, 0); (void) fprintf(stderr, "|\n");
	if (result == Py_None) {
		// There might by some errors in mkJob.py.
		iReturnVal = -1;
		break;
	}

	// 설정은 거꾸로 한다. (Job -> Command -> ByteStream -> Script)
	// 나중의 것이 처음 것을 덮어쓰도록 한다.
	// Job...
	PyObject*	hJob = PyDict_GetItemString(result, MAKE_JOB_SCRIPT__KEY_NAME__JOB); assert(hJob);	// Borrowed ref.
	this->m__jobDownNextStage = JOB_DOWN_STAGE__JOB;
	Py_INCREF(hJob);	// result 를 없애도 살아있어야 한다.

	// Command...
	PyObject*	hCommand = PyDict_GetItemString(result, MAKE_JOB_SCRIPT__KEY_NAME__COMMAND); assert(hCommand);
	if (hCommand != Py_None) {
		this->m__jobDownNextStage = JOB_DOWN_STAGE__COMMAND;
		Py_INCREF(hCommand);
	} else
		hCommand = NULL;

	// ByteStream...
	// byteStreamID 와 hCommand 는 NULL 일 수도 있다.
	PyObject*	byteStreamID = PyDict_GetItemString(result, MAKE_JOB_SCRIPT__KEY_NAME__BYTE_STREAM_ID); assert(byteStreamID);
	if (byteStreamID != Py_None) {
		this->m__jobDownNextStage = JOB_DOWN_STAGE__BYTE_STREAM;
		Py_INCREF(byteStreamID);
	} else
		byteStreamID = NULL;

	// Script...
	PyObject*	requiredScriptID__LIST = PyDict_GetItemString(result, MAKE_JOB_SCRIPT__KEY_NAME__REQUIRED_SCRIPT_ID__LIST); assert(requiredScriptID__LIST);
	Py_ssize_t	len = PyList_Size(requiredScriptID__LIST);
	if (len) {
		this->m__jobDownNextStage = JOB_DOWN_STAGE__SCRIPT;
		Py_INCREF(requiredScriptID__LIST);
	} else
		requiredScriptID__LIST = NULL;
#if 0
	// Python 오브젝트(dictionary) 를 문자열로 바꾸고,
	// 다시 그 문자열로부터 Python 오브젝트를 만들어
	// 서로 일치하는 지 알아본다.
	fprintf(stderr, "hJob:= |"); PyObject_Print(hJob, stderr, 0); fprintf(stderr, "|\n");
	// C 문자열로 바꾼다.
	PyObject*	hJobStr = PyObject_Str(hJob); assert(hJobStr);		// New ref.
	char*		hJobStr__STR = PyString_AsString(hJobStr); assert(hJobStr__STR);
	fprintf(stderr, "hJobStr:= |%s|\n", hJobStr__STR);
	// 다시 Python 오브젝트를 만든다.
	PyObject*	val = PyRun_String(hJobStr__STR, Py_eval_input,
		this->m__globalDict, this->m__globalDict); assert(val);		// New ref.
	fprintf(stderr, "val:= |"); PyObject_Print(val, stderr, 0); fprintf(stderr, "|\n");
	fprintf(stderr, "%d\n", PyObject_RichCompareBool(hJob, val, Py_EQ));

	Py_DECREF(val);
	Py_DECREF(hJobStr);
#endif

	// 마지막으로 처리할 Job 의 목록에서 첫 번째를 없앤다.
	// 소유권이 이 클래스로 넘어와야하므로 ref. count 를 1 증가시킨다.
	Py_INCREF(jobID);
	if (PyList_SetSlice(((CAgentSlot*) this)->getOrder__LIST(), 0, 1, NULL) != 0) {
		(void) fprintf(stderr,
			"ERROR: "
			"__FILE__:= |%s|, __LINE_NO__ = %d: "
			"CJobDown::__pyMakeJob: "
			"PyList_SetSlice(|"
			,
			__FILE__, __LINE__
		); (void) PyObject_Print(((CAgentSlot*) this)->getOrder__LIST(), stderr, 0);
		(void) fprintf(stderr, "|, 0, 1, NULL): Failure\n");

		if (PyErr_Occurred())
			PyErr_Print();

		Py_DECREF(jobID);	// ref. count 를 원래대로 만든다.
		iReturnVal = -1;
		break;
	}

	// 위에서 얻을 값들을 다음에 사용할 수 있도록 설정한다.
	this->m__jobID = jobID;
	this->m__scriptID__LIST = requiredScriptID__LIST;
	this->m__byteStreamID = byteStreamID;
	this->m__hCommand = hCommand;
	this->m__hJob = hJob;
	iReturnVal = 1;		// 처리할 Job 이 있다.

	break;
}
	Py_XDECREF(argv); argv = NULL;
	Py_XDECREF(result); result = NULL;

	if (iReturnVal <= 0) {
		// Job 에 관한 부분을 초기화한다.
		this->m__jobID = NULL;
		this->m__jobDownNextStage = JOB_DOWN_STAGE__READY;
		Py_XDECREF(this->m__scriptID__LIST); this->m__scriptID__LIST = NULL;
		this->m__byteStreamID = NULL;
		Py_XDECREF(this->m__hCommand); this->m__hCommand = NULL;
		Py_XDECREF(this->m__hJob); this->m__hJob = NULL;

		return -1;
	}

	return 1;	// 내려보내야할 Job 있다.
}
int
CJobDown::__pyDoJobDown(
	CBuffer&	packet
)
{
	int	iReturnVal = 0;
do {
	iReturnVal = this->__pyMakeJob();
	if (iReturnVal < 0) {
		(void) fprintf(stderr,
			"ERROR: "
			"__FILE__:= |%s|, __LINE_NO__ = %d: "
			"CJobDown::__pyDoJobDown: "
			"CJobDown::__pyMakeJob: Failure\n"
			,
			__FILE__, __LINE__
		);

		break;
	}

	// 처리할 Job 이 있는가?
	if (m__jobDownNextStage == JOB_DOWN_STAGE__READY) {
		// 처리할 Job 이 없다.
//		(void) fprintf(stderr, "No job\n");
		iReturnVal = 0;
		break;
	}

	// 진행 중인 부분이 있나?
	if (m__jobDownNextStage & JOB_DOWN_STAGE__PENDING) {
		// (m__jobDownNextStage & ~JOB_DOWN_STAGE__PENDING) Stage 가 처리 중이다.
		(void) fprintf(stderr, "JOB_DOWN_STAGE__PENDING...\n");
		iReturnVal = 0;
		break;
	}

	if (this->m__jobDownNextStage == JOB_DOWN_STAGE__SCRIPT) {
		if (this->m__scriptID__LIST == NULL) {
			(void) fprintf(stderr,
				"FATAL: "
				"__FILE__:= |%s|, __LINE_NO__ = %d: "
				"CJobDown::__pyDoJobDown: "
				"JOB_DOWN_STAGE_PACKET_TYPE__SCRIPT: "
				"m__scriptID__LIST = NULL\n"
				,
				__FILE__, __LINE__
			);

			iReturnVal = -1;
			break;
		}

		// m__scriptID__LIST 에 있는 첫 번째 Script ID 에 해당하는 Script 를 내려보낸다.
		// iReturnVal = 1:= 내려보낼 부분이 있다.
#if 1	// Source 를 "인자"에...
		PyObject*	source_Str = NULL;
#else	// Source 를 "추가인자"에...
		PyObject*	jobID_Str = NULL;	// PyObject_Str 에 의해서 "새로" 만들어진다.
#endif
		PyObject*	extraWhat = NULL;	// Py_BuildValue 에 의해서 "새로" 만들어진다.
		PyObject*	extraWhat_Str = NULL;	// PyObject_Str 에 의해서 "새로" 만들어진다.
	while (1) {
		PyObject*	scriptID = PyList_GetItem(this->m__scriptID__LIST, 0);
				// Borrowed reference
		if (!scriptID) {
			(void) fprintf(stderr,
				"ERROR: "
				"__FILE__:= |%s|, __LINE_NO__ = %d: "
				"CJobDown::__pyDoJobDown: "
				"JOB_DOWN_STAGE_PACKET_TYPE__SCRIPT: "
				"PyList_GetItem(|"
				,
				__FILE__, __LINE__
			); (void) PyObject_Print(this->m__scriptID__LIST, stderr, 0);
			(void) fprintf(stderr, "|, 0): Failure\n");

			iReturnVal = -1;
			break;
		}

		// Script 에 대한 정보를 얻는다.
		PyObject*	hScript = PyDict_GetItem(this->m__hScript2ByID, scriptID);
		if (!hScript) {
			(void) fprintf(stderr,
				"ERROR: "
				"__FILE__:= |%s|, __LINE_NO__ = %d: "
				"CJobDown::__pyDoJobDown: "
				"JOB_DOWN_STAGE_PACKET_TYPE__SCRIPT: "
				"PyDict_GetItem(|"
				,
				__FILE__, __LINE__
			); (void) PyObject_Print(this->m__hScript2ByID, stderr, 0);
			(void) fprintf(stderr, "|, |"); (void) PyObject_Print(scriptID, stderr, 0);
			(void) fprintf(stderr, "|: Failure\n");

			if (PyErr_Occurred())
				PyErr_Print();

			assert(hScript);
			iReturnVal = -1;
			break;
		}
		// Source 를 얻는다.
		PyObject*	source = PyDict_GetItemString(hScript, SCRIPT__KEY_NAME__SOURCE);
		if (!source) {
			if (PyErr_Occurred())
				PyErr_Print();
			assert(source);
			iReturnVal = -1;
			break;
		}
		// 추가적으로 (SCRIPT__KEY_NAME__NAME, SCRIPT__KEY_NAME__TYPE_BLOB, SCRIPT__KEY_NAME__REQ_ID__SET, SCRIPT__KEY_NAME__SOURCE)를 보낸다.
		PyObject*	name = PyDict_GetItemString(hScript, SCRIPT__KEY_NAME__NAME);
		if (!name) {
			if (PyErr_Occurred())
				PyErr_Print();
			assert(name);
			iReturnVal = -1;
			break;
		}
		PyObject*	typeBlob = PyDict_GetItemString(hScript, SCRIPT__KEY_NAME__TYPE_BLOB);
		if (!typeBlob) {
			if (PyErr_Occurred())
				PyErr_Print();
			assert(typeBlob);
			iReturnVal = -1;
			break;
		}
		PyObject*	reqIDSet = PyDict_GetItemString(hScript, SCRIPT__KEY_NAME__REQ_ID__SET);
		if (!reqIDSet) {
			if (PyErr_Occurred())
				PyErr_Print();
			assert(reqIDSet);
			iReturnVal = -1;
			break;
		}

		extraWhat = Py_BuildValue(
			"{s: O, s: O, s: O"
#if 1
#else
			", s: O"
#endif
			"}"
			,
			SCRIPT__KEY_NAME__NAME, name,
			SCRIPT__KEY_NAME__TYPE_BLOB, typeBlob,
			SCRIPT__KEY_NAME__REQ_ID__SET, reqIDSet
#if 1
#else
			,
			SCRIPT__KEY_NAME__SOURCE, source
#endif
		);	// New ref.
		if (!extraWhat) {
			if (PyErr_Occurred())
				PyErr_Print();
			assert(extraWhat);
			iReturnVal = -1;
			break;
		}
//		(void) PyObject_Print(extraWhat, stderr, 0); fprintf(stderr, "\n");
		// 문자열 표현으로 바꾼다.
		// Agent 에서는 PyRun_String(..., Py_eval_input, ...) 을 호출하여,
		// 해당 dictionary 로 만들어 필요한 정보를 얻는다.
		extraWhat_Str = PyObject_Str(extraWhat);	// New ref.
		if (!extraWhat_Str) {
			if (PyErr_Occurred())
				PyErr_Print();
			assert(extraWhat_Str);
			iReturnVal = -1;
			break;
		}

#if 1
		source_Str = PyObject_Str(source);
		if (!source_Str) {
			if (PyErr_Occurred())
				PyErr_Print();
			assert(source_Str);
			iReturnVal = -1;
			break;
		}
#else
		jobID_Str = PyObject_Str(this->m__jobID);	// New ref.
		if (!jobID_Str) {
			if (PyErr_Occurred())
				PyErr_Print();
			assert(jobID_Str);
			iReturnVal = -1;
			break;
		}
#endif
		int	isBinary = 0;
		iReturnVal = CJobDownPacket::pyPack(
			packet,
			scriptID,
			CJobDownPacket::JOB_DOWN_STAGE_PACKET_TYPE__SCRIPT,
			isBinary,
#if 1
			source_Str,
#else
			jobID_Str,
#endif
			extraWhat_Str
		);
		if (iReturnVal < 0) {
			(void) fprintf(stderr,
				"ERROR: "
				"__FILE__:= |%s|, __LILE_NO__ = %d: "
				"CJobDown::__pyDoJobDown: "
				"JOB_DOWN_STAGE_PACKET_TYPE__SCRIPT: "
				"CJobDownPacket::pyPack(..., JOB_DOWN_STAGE_PACKET_TYPE__SCRIPT, Script ID = |"
				,
				__FILE__, __LINE__
			); (void) PyObject_Print(scriptID, stderr, 0);
			(void) fprintf(stderr, "|, Binary? = False, |");
			(void) PyObject_Print(source, stderr, 0);
			(void) fprintf(stderr, "|, |"); (void) PyObject_Print(extraWhat_Str, stderr, 0);
			(void) fprintf(stderr, "|: Failure\n");
		}

		break;
	}
#if 1
		Py_XDECREF(source_Str); source_Str = NULL;
#else
		Py_XDECREF(jobID_Str); jobID_Str = NULL;
#endif
		Py_XDECREF(extraWhat); extraWhat = NULL;
		Py_XDECREF(extraWhat_Str); extraWhat_Str = NULL;

		if (iReturnVal < 0) {
			// 무언가 문제가 있었다.
			break;
		}
		// iReturnVal = 0;

		// scriptID 에 해당하는 Source 를 잘 패킷해서 packet 에 넣었다.
		// m__scriptID__LIST 에서 첫 번째 item 을 삭제한다.
		if (PyList_SetSlice(this->m__scriptID__LIST, 0, 1, NULL) != 0) {
			(void) fprintf(stderr,
				"ERROR: "
				"__FILE__:= |%s|, __LINE_NO__ = %d: "
				"CJobDown::__pyDoJobDown: "
				"JOB_DOWN_STAGE_PACKET_TYPE__SCRIPT: "
				"PyList_SetSlice(|"
				,
				__FILE__, __LINE__
			); (void) PyObject_Print(this->m__scriptID__LIST, stderr, 0);
			(void) fprintf(stderr, "|, 0, 1, NULL): Failure\n");

			if (PyErr_Occurred())
				PyErr_Print();

			// 패킹된 모든 데이터를 무효로한다.
			packet.setDataSize(0);
			iReturnVal = -1;
			break;
		}
	} else if (this->m__jobDownNextStage == JOB_DOWN_STAGE__BYTE_STREAM) {
		if (this->m__byteStreamID == NULL) {
			(void) fprintf(stderr,
				"FATAL: "
				"__FILE__:= |%s|, __LINE_NO__ = %d: "
				"CJobDown::__pyDoJobDown: "
				"JOB_DOWN_STAGE_PACKET_TYPE__BYTE_STREAM: "
				"m__byteStreamID = NULL\n"
				,
				__FILE__, __LINE__
			);

			iReturnVal = -1;
			break;
		}

		// m__byteStreamID 에 해당하는 Byte Stream 을 내려보낸다.
		// iReturnVal = 1:= 내려보낼 부분이 있다.
		PyObject*	jobID_Str = NULL;	// PyObject_Str 에 의해서 "새로" 만들어진다.
		PyObject*	extraWhat = NULL;	// Py_BuildValue 에 의해서 "새로" 만들어진다.
		PyObject*	extraWhat_Str = NULL;	// PyObject_Str 에 의해서 "새로" 만들어진다.
	while (1) {
		// Byte Stream 에 대한 정보를 얻는다.
		PyObject*	hByteStream = PyDict_GetItem(this->m__hByteStream2ByID, this->m__byteStreamID);
		if (!hByteStream) {
			(void) fprintf(stderr,
				"ERROR: "
				"__FILE__:= |%s|, __LINE_NO__ = %d: "
				"CJobDown::__pyDoJobDown: "
				"JOB_DOWN_STAGE_PACKET_TYPE__BYTE_STREAM: "
				"PyDict_GetItem(|"
				,
				__FILE__, __LINE__
			); (void) PyObject_Print(this->m__hByteStream2ByID, stderr, 0);
			(void) fprintf(stderr, "|, |"); (void) PyObject_Print(this->m__byteStreamID, stderr, 0);
			(void) fprintf(stderr, "|: Failure\n");

			if (PyErr_Occurred())
				PyErr_Print();

			assert(hByteStream);
			iReturnVal = -1;
			break;
		}
		// Byte Stream 를 얻는다.
		PyObject*	byteStream = PyDict_GetItemString(hByteStream, BYTE_STREAM__KEY_NAME__BYTE_STREAM);
		if (!byteStream) {
			if (PyErr_Occurred())
				PyErr_Print();
			assert(byteStream);
			iReturnVal = -1;
			break;
		}
		// 추가적으로 (BYTE_STREAM__KEY_NAME__NAME, BYTE_STREAM__KEY_NAME__ENTRY_POINT, BYTE_STREAM__KEY_NAME__MD5)를 보낸다.
		PyObject*	name = PyDict_GetItemString(hByteStream, BYTE_STREAM__KEY_NAME__NAME);
		if (!name) {
			if (PyErr_Occurred())
				PyErr_Print();
			assert(name);
			iReturnVal = -1;
			break;
		}
		PyObject*	entryPoint = PyDict_GetItemString(hByteStream, BYTE_STREAM__KEY_NAME__ENTRY_POINT);
		if (!entryPoint) {
			if (PyErr_Occurred())
				PyErr_Print();
			assert(entryPoint);
			iReturnVal = -1;
			break;
		}
		PyObject*	md5 = PyDict_GetItemString(hByteStream, BYTE_STREAM__KEY_NAME__MD5);
		if (!md5) {
			if (PyErr_Occurred())
				PyErr_Print();
			assert(md5);
			iReturnVal = -1;
			break;
		}

		extraWhat = Py_BuildValue(
			"{s: O, s: O, s: O}"
			,
//			BYTE_STREAM__KEY_NAME__BYTE_STREAM, byteStream,
			BYTE_STREAM__KEY_NAME__NAME, name,
			BYTE_STREAM__KEY_NAME__ENTRY_POINT, entryPoint,
			BYTE_STREAM__KEY_NAME__MD5, md5
		);	// New ref.
		if (!extraWhat) {
			if (PyErr_Occurred())
				PyErr_Print();
			assert(extraWhat);
			iReturnVal = -1;
			break;
		}
//		(void) PyObject_Print(extraWhat, stderr, 0); fprintf(stderr, "\n");
		// 문자열 표현으로 바꾼다.
		// Agent 에서는 PyRun_String(..., Py_eval_input, ...) 을 호출하여,
		// 해당 dictionary 로 만들어 필요한 정보를 얻는다.
		extraWhat_Str = PyObject_Str(extraWhat);	// New ref.
		if (!extraWhat_Str) {
			if (PyErr_Occurred())
				PyErr_Print();
			assert(extraWhat_Str);
			iReturnVal = -1;
			break;
		}

		jobID_Str = PyObject_Str(this->m__jobID);	// New ref.
		if (!jobID_Str) {
			if (PyErr_Occurred())
				PyErr_Print();
			assert(jobID_Str);
			iReturnVal = -1;
			break;
		}
		int	isBinary = 1;
		iReturnVal = CJobDownPacket::pyPack(
			packet,
			this->m__byteStreamID,
			CJobDownPacket::JOB_DOWN_STAGE_PACKET_TYPE__BYTE_STREAM,
			isBinary, byteStream,
			extraWhat_Str
		);
		if (iReturnVal < 0) {
			(void) fprintf(stderr,
				"ERROR: "
				"__FILE__:= |%s|, __LILE_NO__ = %d: "
				"CJobDown::__pyDoJobDown: "
				"JOB_DOWN_STAGE_PACKET_TYPE__BYTE_STREAM: "
				"CJobDownPacket::pyPack(..., JOB_DOWN_STAGE_PACKET_TYPE__BYTE_STREAM, ByteStream ID = |"
				,
				__FILE__, __LINE__
			); (void) PyObject_Print(this->m__byteStreamID, stderr, 0);
			(void) fprintf(stderr, "|, Binary? = True, |");
			(void) PyObject_Print(byteStream, stderr, 0);
			(void) fprintf(stderr, "|, |"); (void) PyObject_Print(extraWhat_Str, stderr, 0);
			(void) fprintf(stderr, "|: Failure\n");
		}

		break;
	}
		Py_XDECREF(jobID_Str); jobID_Str = NULL;
		Py_XDECREF(extraWhat); extraWhat = NULL;
		Py_XDECREF(extraWhat_Str); extraWhat_Str = NULL;

		if (iReturnVal < 0) {
			// 무언가 문제가 있었다.
			break;
		}
		// iReturnVal = 0;
	} else if (this->m__jobDownNextStage == JOB_DOWN_STAGE__COMMAND) {
		if (this->m__hCommand == NULL) {
			(void) fprintf(stderr,
				"FATAL: "
				"__FILE__:= |%s|, __LINE_NO__ = %d: "
				"CJobDown::__pyDoJobDown: "
				"JOB_DOWN_STAGE__COMMAND: "
				"m__hCommand = NULL\n"
				,
				__FILE__, __LINE__
			);

			iReturnVal = -1;
			break;
		}

		// m__hCommand 에 저장된 Command 의 정의를 내려보낸다.
		PyObject*	commandID = PyDict_GetItemString(this->m__hCommand, MAKE_JOB_SCRIPT__KEY_NAME__COMMAND__KEY_NAME__ID);
		if (!commandID) {
			if (PyErr_Occurred())
				PyErr_Print();
			assert(commandID);
			iReturnVal = -1;
			break;
		}
		// m__hCommand 를 문자열 표현으로 바꾼다.
		PyObject*	hCommand_Str = PyObject_Str(this->m__hCommand);
		if (!hCommand_Str) {
			if (PyErr_Occurred())
				PyErr_Print();
			assert(hCommand_Str);
			iReturnVal = -1;
			break;
		}
		int	isBinary = 0;
		iReturnVal = CJobDownPacket::pyPack(
			packet,
			commandID,
			CJobDownPacket::JOB_DOWN_STAGE_PACKET_TYPE__COMMAND,
			isBinary, hCommand_Str,
			NULL
		);
		if (iReturnVal < 0) {
			(void) fprintf(stderr,
				"ERROR: "
				"__FILE__:= |%s|, __LILE_NO__ = %d: "
				"CJobDown::__pyDoJobDown: "
				"JOB_DOWN_STAGE_PACKET_TYPE__COMMAND: "
				"CJobDownPacket::pyPack(..., JOB_DOWN_STAGE_PACKET_TYPE__COMMAND, Command ID = |"
				,
				__FILE__, __LINE__
			); (void) PyObject_Print(commandID, stderr, 0);
			(void) fprintf(stderr, "|, Binary? = False, |");
			(void) PyObject_Print(hCommand_Str, stderr, 0);
			(void) fprintf(stderr, "|, NULL): Failure\n");

			break;
		}
	} else if (this->m__jobDownNextStage == JOB_DOWN_STAGE__JOB) {
		if (this->m__hJob == NULL) {
			(void) fprintf(stderr,
				"FATAL: "
				"__FILE__:= |%s|, __LINE_NO__ = %d: "
				"CJobDown::__pyDoJobDown: m__hJob = NULL\n"
				,
				__FILE__, __LINE__
			);

			iReturnVal = -1;
			break;
		}

		// m__hJob 에 저장된 Job 의 정의를 내려보낸다.
		// m__hJob 를 문자열 표현으로 바꾼다.
		PyObject*	hJob_Str = PyObject_Str(this->m__hJob);
		if (!hJob_Str) {
			if (PyErr_Occurred())
				PyErr_Print();
			assert(hJob_Str);
			iReturnVal = -1;
			break;
		}
		int	isBinary = 0;
		iReturnVal = CJobDownPacket::pyPack(
			packet,
			this->m__jobID,
			CJobDownPacket::JOB_DOWN_STAGE_PACKET_TYPE__JOB,
			isBinary, hJob_Str,
			NULL
		);
		if (iReturnVal < 0) {
			(void) fprintf(stderr,
				"ERROR: "
				"__FILE__:= |%s|, __LILE_NO__ = %d: "
				"CJobDown::__pyDoJobDown: "
				"JOB_DOWN_STAGE_PACKET_TYPE__JOB: "
				"CJobDownPacket::pyPack(..., JOB_DOWN_STAGE_PACKET_TYPE__JOB, Job ID = |"
				,
				__FILE__, __LINE__
			); (void) PyObject_Print(this->m__jobID, stderr, 0);
			(void) fprintf(stderr, "|, Binary? = False, |");
			(void) PyObject_Print(hJob_Str, stderr, 0);
			(void) fprintf(stderr, "|, NULL): Failure\n");

			break;
		}
	} else {
		(void) fprintf(stderr,
			"FATAL: "
			"__FILE__:= |%s|, __LINE_NO__ = %d: "
			"CJobDown::__pyDoJobDown: this->m__jobDownNextStage = %d: Invalid\n"
			,
			__FILE__, __LINE__, this->m__jobDownNextStage
		);

		iReturnVal = -1;
		break;
	}

	// 처리 중임을 표시한다.
//	this->m__jobDownNextStage ^= JOB_DOWN_STAGE__PENDING;
	unsigned int	jobDownNextStage = (unsigned int) this->m__jobDownNextStage;
	jobDownNextStage ^= JOB_DOWN_STAGE__PENDING;
	this->m__jobDownNextStage = (JOB_DOWN_STAGE_T) jobDownNextStage;

	iReturnVal = 1;		// 내려보낼 부분이 있다.
} while (0);

	return iReturnVal;
}
