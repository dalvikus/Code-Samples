#ifndef __WORKER_H__
#define __WORKER_H__
////////////////////////////////////////////////////////////////////////////////


#include "CPythonEngine.h"
#include "CSocketOUT.h"
#include "CQueueSocketThread.h"

class CWorker : public CPythonEngine {
// Agent 를 Update 하는 경우에 사용하는 구조체
public:
#define UPDATE_AGENT__COPY_LENGTH	(12 + 1)	// 새로운 Agent 의 이름의 길이(NUL 을 포함)
							// "Agent_XXXXXX"
typedef struct {
	char		agentCopy[UPDATE_AGENT__COPY_LENGTH];
						// 새로운 Agent 는 "agent_XXXXXX" 라는 이름으로 저장된다;
						// mkstemp 에서 사용되는 것으로 뒤의 XXXXXX 는 임의로 바뀐다.
						// (CWorker::update 참고)
	unsigned	orderID;		// Update Agent 명령을 내린 서버가 관리하는 Order ID;
						// 재시작 후에 제대로 살아났다는 응답패킷을 보낼 때 사용된다.
	unsigned	flag;			// Update 할 때 설정하는 몇 가지 플래그;
						// 아래의 UPDATE_AGENT__XXX 를 참고한다.
} UPDATE_AGENT_T;
#define UPDATE_AGENT__UPDATE	(1 << 0)	// Agent 를 할 지를 말해준다;
						// CWorker::pyUnpack 에서 패킷의 내용을 보고 결정한다.
						// CWorker::pyUnpack 을 호출한 함수에서는 이 비트를 보고
						// Update 할 지를 결정한다.
#define UPDATE_AGENT__RESUME	(1 << 1)	// 처리 중인 Job 을 재시작 후에 계속 처리할 것 인지를 말해준다.

public:
typedef enum {
	CMD_STAT__STOPPED	= 0,
	CMD_STAT__PAUSED,
	CMD_STAT__RUNNING,
} CMD_STAT_T;
typedef enum {
	CMD_CTRL__NONE		= 0,

	// Manager -> Worker
	CMD_CTRL__STOP,
	CMD_CTRL__PAUSE,
	CMD_CTRL__CONTINUE,

	// Worker -> Manager
	CMD_CTRL__STOP_ACK,
	CMD_CTRL__PAUSE_ACK,
	CMD_CTRL__CONTINUE_ACK,
} CMD_CTRL_T;

public:
	static const char*	BYTE_STREAM_BY_ID__VAR_NAME;
	static const char*	SCRIPT_BY_ID__VAR_NAME;
	static const char*	COMMAND_BY_ID__VAR_NAME;
private:
	PyObject*	m__hScriptByID;
	PyObject*	m__hByteStreamByID;
	PyObject*	m__hCommandByID;
public:
	const PyObject*	gethScriptByID(void) const		{return this->m__hScriptByID;}
	const PyObject*	gethByteStreamByID(void) const	{return this->m__hByteStreamByID;}
	const PyObject*	gethCommandByID(void) const		{return this->m__hCommandByID;}

private:
	PyObject*	m__hWorkerArgByID;

public:
	CWorker(void);
	virtual ~CWorker();

public:
	void	help(void);
	void	pyListAllObjects(void);

public:
	// 모든 Worker 를 중지시킨다.
	void	pyListWorkers(void);
	int	pyKill(PyObject* commandID);
	int	pyKillAll(void);

private:
	// 1. m__hScriptByID 에,
	// 내려받은 Script 에 대한 정보를 다음과 같이 설정한다.
	//	m__hScriptByName = {'Name': ScriptID, ...}
	//	m__hScriptByID = {
	//		ScriptID = {
	//			'Name': '...',
	//			'TypeBlob': (...),	# Tuple
	//			'MD5': '...',
	//			'Source': '...',
	//		},
	//		......
	//	}
	// scriptInfo 에는 다음 정보가 들어있다:
	//	{
	//		'Name': '...',
	//		'TypeBlob': (None,),	# Tuple
	//		'MD5': '...',
	//		'Source': '...'
	//	}
	// 2. "TypeBlob" 에 따라 Script 를 설정한다. (함수로 만들거나 변수로 설정하거나)
	// 자세한 것은 CPythonEngine::PyDoTypeBlob 함수에 나와있다.
	int	__pySetScript(
		PyObject*	scriptID,
#if 1	// Source 가 "인자"에...
		const CBuffer&	scriptSrc,
#else	// Source 가 "추가인자"에...
#endif
		const CBuffer&	scriptInfo
	);

	// 1. m__hByteStreamByID 에,
	// 내려받은 Byte Stream 에 대한 정보를 다음과 같이 설정한다.
	//	m__hByteStreamByID = {
	//		ByteStreamID = {
	//			'Name': '...',
	//			'Entry Point': '...',
	//			'MD5': '...',
	//			'Byte Stream': '...',
	//		},
	//		......
	//	}
	// byteStreamInfo 에는 다음 정보가 들어있다:
	//	{
	//		'Name': '...',
	//		'Entry Point': '...',
	//		'MD5': '...',
	//		'Byte Stream': '...'
	//	}
	// 2. 내려받은 Byte Stream 을 특정위치에 지정된 이름으로 저장한다.
public:
	static const char*	BYTE_STREAM__KEY_NAME__LIB_SAVED_PATHNAME;
	static const char*	BYTE_STREAM__KEY_NAME__COMMAND_ID__SET;
private:
	// 같은 Byte Stream 을 내려받았는 지 확인한다.
	PyObject*	__pyGethPrevByteStream(PyObject* byteStreamID, PyObject* hByteStream, const CBuffer& byteStreamBuffer);
	int		__pySaveByteStream(PyObject* byteStreamID, PyObject* hByteStream, const CBuffer& byteStreamBuffer);

	// 1. m__hCommandByID 에,
	// 내려받은 Byte Stream 에 대한 정보를 다음과 같이 설정한다.
	//	m__hCommandByID = {
	//		CommandID = {
	//			'Name': '...',
	//			'ByteStreamID__LINK': ####,
	//			'ArgumentScriptID__LINK': ####,
	//			'ParseScriptID__LINK': ####,
	//			'HeartBeatInterval': ####,
	//		},
	//		......
	//	}
	// commandInfo 에는 다음 정보가 들어있다:
	//	{
	//		'CommandID': ####,
	//		'Name': '...',
	//		'ByteStreamID__LINK': ####,
	//		'ArgumentScriptID__LINK': ####,
	//		'ParseScriptID__LINK': ####,
	//		'HeartBeatInterval': ####,
	//	}
	// 2. Worker 쓰레드를 만든다.
public:
	static const char*	COMMAND__KEY_NAME__BYTE_STREAM_ID__LINK;
	static const char*	COMMAND__KEY_NAME__JOB;
	static const char*	COMMAND__KEY_NAME__NAME;
	static const char*	COMMAND__KEY_NAME__STATUS;
	static const char*	COMMAND__KEY_NAME__CONTROL;
private:
	// hCommand 의 COMMAND__KEY_NAME__BYTE_STREAM_ID__LINK 이 가리키는 Byte Stream 을 사용하여,
	// Worker 쓰레드를 만든다.
	int	__pyCreateWorkerThread(PyObject* commandID, PyObject* hCommand, int overwrite);
	int	__pySetCommand(PyObject* commandID, const CBuffer& commandInfo);

public:
	static const char*	JOB__KEY_NAME__COMMAND_ID__LINK;
private:
	int	__pySetJob(PyObject* jobID, const CBuffer& jobInfo);
public:
	// rPacket 에 있는 "들어오는" 패킷을 해석하고,
	// 적절하게 "나가는" 패킷을 wPacket 에 쓴다.
	// Agent 를 업데이트 하는 경우, 새로운 Agent 의 이름을,
	// agentCopy 가 가리킨다.
	int	pyUnpack(const CBuffer& rPacket, CBuffer& wPacket, UPDATE_AGENT_T& ua);

public:
	static const char*	AGENT_COPY__NAME;
	static const char*	AGENT_COPY__RESUME;
	static const char*	AGENT_COPY__ORDER_ID;
	// 내려받은 Agent 의 내용을 임시파일로 저장한 후,
	// 다시 살아나기 위한 쉘 스크립트를 작성한다.
	int	update(const CBuffer& agent, UPDATE_AGENT_T& ua);
	int	pySaveAll(void);
	int	pyReload(void);
#if 1
	// 내려받은 Set 에 대한 정보를 설정한다:
	//	"{'ScriptID__SET': set([...]), 'ByteStreamID__SET': set([...]), 'CommandID__SET': set([...])}"
	int	pyGetSet(CBuffer& setInfo);
#else
	int	restart(UPDATE_AGENT_T& ua);
#endif

private:
	int	m__ctrlSockfd;
	unsigned	m__ctrlPort;
public:
	int	getCtrlSockfd(void) const	{return this->m__ctrlSockfd;}
private:
	unsigned	m__dataPort;
	int	m__dataSockfd;
public:
	int	getDataSockfd(void) const	{return this->m__dataSockfd;}
	void	setDataSockfd(int dataSockfd)	{this->m__dataSockfd = dataSockfd;}
public:
	static int	setSockfd(void);
	// M/S 와 연결되자마자 연결의 종류(Control, Data)에 따라 해당 응답패킷을 먼저 보낸다;
	//	Control Socket... -> sendSockID() -> recvPort() ->
	//	Data Socket... -> sendSockID() -> recvPort()
private:
	CSocketOUT*	m__pCtrlSockOUT;
public:
	// Control Socket 에 쓸 때 사용되는 큐의 크기;
	//	대부분 M/S 에 답하는 형식을 취하므로 그리 크지 않아도 된다.
	static const size_t	CONTROL__WRITE_QUEUE_SIZE;
	CSocketOUT*	getCtrlSocketOUT(void)	{return this->m__pCtrlSockOUT;}
#if 0	// __USE_QUEUE_THREAD__
private:
	CSocketOUT*	m__pDataSockOUT;
public:
	// Data Socket 에 쓸 때 사용되는 큐의 크기;
	//	1) Agent 가 수집한 정보를 보낼 때 사용하므로 수집된 정보의 크기에 따라 설정한다.
	//	2) 가장 큰 것은 ps 의 결과일 것이다.
	//	3) 만약 각 쓰레드가 수집한 결과를 하나의 독립된 쓰레드를 사용해서 보낼 때에는
	//	이 값은 최소한으로 설정하면 된다.
	CSocketOUT*	getDataSocketOUT(void)	{return this->m__pDataSockOUT;}
#else
private:
	CQueueSocketThread*	m__pQueueSocketThread;
public:
	CQueueSocketThread*	getQueueSocketThread(void)	{return this->m__pQueueSocketThread;}
#endif
public:
	static const size_t	DATA__WRITE_QUEUE_SIZE;
};


////////////////////////////////////////////////////////////////////////////////
#endif	// __WORKER_H__
