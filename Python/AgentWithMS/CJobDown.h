#ifndef __JOB_DOWN_H__
#define __JOB_DOWN_H__
////////////////////////////////////////////////////////////////////////////////


#include <Python.h>
#include "CBuffer.h"

class CJobDown {
public:
// 현재 Job 의 어느 부분을 내려받는 지를 말해준다.
typedef enum {
	JOB_DOWN_STAGE__READY	= 0,	// Job 의 모든 부분을 내려받았다.
					// 새로운 Job 을 내려받을 수 있다.
					// 0 으로 설정되어야 한다.
					// 이유는 아래의 JOB_DOWN_STAGE__PENDING 의 설명을 참고한다.

					// 각 Stage 마다 사용하는 ID 가 있다.
	JOB_DOWN_STAGE__SCRIPT,		// (ScriptID) Script 를 내려받는 중
	JOB_DOWN_STAGE__BYTE_STREAM,	// (ByteStreamID) Byte Stream...
	JOB_DOWN_STAGE__COMMAND,	// (CommandID) Command...
	JOB_DOWN_STAGE__JOB,		// (JobID) Job...

	// 최상위비트가 설정되었다면 현재 해당 Stage 를 처리 중이다.
	// JOB_DOWN_STAGE__READY 는 제외한다. (JOB_DOWN_STAGE__READY 가 0 으로 설정된 이유이다.)
	// doJobDown 에서 사용한다.
	JOB_DOWN_STAGE__PENDING	= 0x8000,
} JOB_DOWN_STAGE_T;
private:
	JOB_DOWN_STAGE_T	m__jobDownNextStage;	// Job 의 어느 부분을 처리해야하는 지를 말해준다.
public:
	JOB_DOWN_STAGE_T	getJobDownStage(void) const	{return this->m__jobDownNextStage;}
public:
	// 처리가 끝난 Stage 에서 다음 Stage 를 결정한다.
	int	pyUpdateJobNextStage(JOB_DOWN_STAGE_T downDoneStage, const PyObject* downDoneID);

private:
	PyObject*	m__jobID;		// 어느 Job 을 처리 중인가?
						// m__jobID = NULL 라면 보내야할 Job 부분이 없다.
						// m__jobDownNextStage = JOB_DOWN_STAGE__READY 와 동등하다.
	PyObject*	m__scriptID__LIST;	// 내려보낼 Script 의 목록
						// 각 Job 에는 Return Script 가 반드시 있으므로,
						// 최소한 하나는 있다.
						// 앞에서부터 꺼내간다. (FIFO)
	PyObject*	m__byteStreamID;	// 내려보낼 Byte Stream
						// NULL 이면 내려보낼 Byte Stream 이 없음을 뜻한다.
	PyObject*	m__hCommand;		// 내려보낼 Command 의 정의
						// NULL 이면 내려보낼 Command 의 정의가 없음을 뜻한다.
						// hCommand2ByID[commandID] 부분을 복사해서 쓴다.
						//	1. 'CommandID' 키를 추가한다.
						//	2. 'hJob' 키를 추가한다.
						// {
						//	'CommandID': ...,
						//
						//	'ByteStreamID__LINK': ...,
						//	'ArgumentScriptID__LINK': ...,
						//	'ParseScriptID__LINK: ...',
						//	'HeartBeatInterval': ...,
						// }
	PyObject*	m__hJob;		// 내려보낼 Job 의 정의;
						// hJobByID[m__jobID] 부분을 복사해서 사용한다.
						// {
						//	'CommandID__LINK': ...,
						//	'Schedule': {...},
						//	'Rule': {...},
						//	'ReturnScriptID__LINK': ...,
						//	'Argument__STR': '...',
						//	'hTask': {...},
						// }

public:
	CJobDown(void);
	virtual ~CJobDown();

public:
	static const char*	SCRIPT__KEY_NAME__NAME;
	static const char*	SCRIPT__KEY_NAME__TYPE_BLOB;
	static const char*	SCRIPT__KEY_NAME__MD5;
	static const char*	SCRIPT__KEY_NAME__REQ_ID__SET;
	static const char*	SCRIPT__KEY_NAME__SOURCE;
	static const char*	BYTE_STREAM__KEY_NAME__NAME;
	static const char*	BYTE_STREAM__KEY_NAME__ENTRY_POINT;
	static const char*	BYTE_STREAM__KEY_NAME__MD5;
	static const char*	BYTE_STREAM__KEY_NAME__BYTE_STREAM;
public:
	// ID 를 통해 Script 와 Byte Stream 를 접근하기위하여 참고하는 전역변수
	static const char*	VAR__SCRIPT2_BY_ID;
	static const char*	VAR__BYTE_STREAM2_BY_ID;
private:
	PyObject*	m__hScript2ByID;
	PyObject*	m__hByteStream2ByID;

private:
	// Job 큐(this->m__jobID__LIST)에서 Job 을 꺼내,
	// 그 Job 에 관한 정보로부터 윗부분을 채운다:
	//	(m__scriptID__LIST, m__byteStreamID, m__hCommand, m__hJob)
	// m__byteStreamID 와 m__hCommand 는 없을 수도 있다.
	// m__hCommand 가 없다면 m__byteStreamID 도 없다. 하지만 반대는 성립하지 않는다.
	// 왜냐하면 서로 다른 Command 이나 같은 Byte Stream 을 사용할 수도 있기 때문이다.
	// Python/C API 를 써서 구하기에 귀찮은 작업이므로 Python Script 을 이용한다. (mkJob.py)
	// 내려보낼 Job 을 준비했다면 1 을, 없다면 0 을 리턴한다.
	// 문제가 있다면 -1 을 리턴한다.
	PyObject*	m__globalDict;
	static const char*	MAKE_JOB_SCRIPT;
	static const char*	MAKE_JOB_SCRIPT__KEY_NAME__REQUIRED_SCRIPT_ID__LIST;
	static const char*	MAKE_JOB_SCRIPT__KEY_NAME__BYTE_STREAM_ID;
	static const char*	MAKE_JOB_SCRIPT__KEY_NAME__COMMAND;
	static const char*	MAKE_JOB_SCRIPT__KEY_NAME__COMMAND__KEY_NAME__ID;
	static const char*	MAKE_JOB_SCRIPT__KEY_NAME__JOB;
	PyObject*	m__mkJob;	// Bytecode 화된 Python 오브젝트
private:
	int		__pyMakeJob(void);
protected:
	// m__jobDownNextStage 를 봐서 어느 Job 의 어느 부분을 내려보내야할 지를 결정한다.
	// packet 에 패킹해서 넣는다.
	// m__jobDownNextStage 에 최상위 비트(JOB_DOWN_STAGE__PENDING)가 설정되어있다면
	// 다음을 기약한다.
	int		__pyDoJobDown(CBuffer& packet);
};


////////////////////////////////////////////////////////////////////////////////
#endif	// !__JOB_DOWN_H__
