#ifndef __MGMT_SRV_H__
#define __MGMT_SRV_H__
////////////////////////////////////////////////////////////////////////////////


#include "CPythonEngine.h"
#include "CPacket.h"
#include "CQueueDatabaseThread.h"
#include "CAgentThread.h"

class CMgmtSrv : public CPythonEngine
{
private:
	int	m__listenSockfd;
public:
	int	getListenSockfd(void) const			{return this->m__listenSockfd;}
public:
	static const char*	ACCEPT_SOCKET__KEY_NAME_CTRL_SOCKFD;
	static const char*	ACCEPT_SOCKET__KEY_NAME_DATA_SOCKFD;
	static const char*	ACCEPT_SOCKET__KEY_NAME_ID_STR;
private:
	// ctrlSockfd: {
	//	ACCEPT_SOCKET__KEY_NAME_ID_STR: '...',
	//	ACCEPT_SOCKET__KEY_NAME_DATA_SOCKFD: '...',
	//	ACCEPT_SOCKET__KEY_NAME_AGENT_PTR: ####
	// }
	PyObject*	m__acceptSockfd__DICT;
public:
	PyObject*	getAcceptSockfd__DICT(void)		{return this->m__acceptSockfd__DICT;}
	const PyObject*	getAcceptSockfd__DICT(void) const	{return this->m__acceptSockfd__DICT;}
private:
	PyObject*	__pyFindSocket(const char* id_cstr);
public:
	// Agent 로부터 연결요청이 왔을 때 다음 단계를 거친다;
	//	Accept Connection -> recvSockID() -> sendPort()

public:
	int		pyAcceptAgent(void);
public:
	void	pyListAgent(void);
	int	enqueueOrder(int index, PyObject* order);
	// 주기적으로 Agent 들의 Running 상태를 검사해서 적절한 조치를 취한다.
	int	pyCheckAgent(void);

public:
	enum {
		MAX_AGENT_THREAD	= 2,
		QUEUE_THREAD_UNIT	= 2,	// 매 QUEUE_THREAD_UNIT 마다 CQueueDatabaseThread 를 만든다.
		DATABASE_QUEUE_SIZE	= 8192000,
	};
private:
	CAgentThread*	m__thread[MAX_AGENT_THREAD];
	int		m__indexOfNextThread;
private:
	CQueueDatabaseThread*	m__queueDatabaseThread;	// 새로 만든 쓰레드가 사용할 CQueueDatabaseThread 에 대한 포인터
					// 매 QUEUE_THREAD_UNIT 마다 새로 만들고,
					// 그 전까지 이전에 만든 포인터를 사용한다.
public:
	// 사용가능한 thread 를 찾는다.
	// indexThread:
	//	>= 0: 이 thread 를 사용한다
	//	< 0: 사용가능한 thread 가 없다
	void	setCtrlSockfd(const char* id_str, int ctrlSockfd, int& indexThread, int& indexSlot);
	int	setDataSockfd(int dataSockfd, int indexThread, int indexSlot);
public:
	CMgmtSrv(void);
	~CMgmtSrv();
public:
	void	help(void);
	void	pyListAllObjects(void);


/*
LOAD__SCRIPT_FUNCTION "setScriptTree2" "setScriptTree2.py" 1
LOAD__INSTALLER "setScriptTree2" "scriptTreeInstaller2.py"
 */
public:
	int	pyLoadInstaller2(const char* installerName, const char* installerArgumentScript);
	int	loadInstaller2WithOpen(const char* installerName, const char* installerArgumentScriptPathname);

public:
	// setScriptTree.h 에 있는 이름과 일치해야 한다.
	static const char*	SCRIPT2_BY_ID;
	static const char*	SCRIPT2_BY_NAME;
	static const char*	BYTE_STREAM2_BY_ID;
	static const char*	BYTE_STREAM2_BY_NAME;
	static const char*	COMMAND2_BY_ID;
	static const char*	COMMAND2_BY_NAME;
	static const char*	JOB2_BY_ID;
	static const char*	JOB2_BY_NAME;
public:
	void	pyEvalAll(void);
public:
	void	pyListJob(void);
	void	pyListCommand(void);
	void	pyListByteStream(void);

public:
	static int	updateAgentWithOpen(CBuffer& packet, const char* agentPathname, int resume);
	static int	updateAgent(CBuffer& packet, const CBuffer& byteStreamBuffer, int resume);
};


////////////////////////////////////////////////////////////////////////////////
#endif	// __MGMT_SRV_H__
