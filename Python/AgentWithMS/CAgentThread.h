#ifndef __AGENT_THREAD_H__
#define __AGENT_THREAD_H__
////////////////////////////////////////////////////////////////////////////////


#include "CAgentSlot.h"
#include "CQueueDatabaseThread.h"
#include "CThread.h"

class CAgentThread : public CThread {
public:
	CAgentThread(void);
	~CAgentThread();

public:
	enum {
		MAX_AGENT_SLOT	= 2
	};
private:
	CAgentSlot*	m__slot[MAX_AGENT_SLOT];
public:
	CAgentSlot**	slots(void)	{return this->m__slot;}
private:
	int		m__indexOfNextSlot;
public:
	int		getIndexOfNextSlot(void)	{return this->m__indexOfNextSlot;}
public:
	// 사용가능한 slot 을 찾는다.
	// indexSlot:
	//	>= 0: Ok
	//	-1: 더 이상 사용가능한 slot 이 없다
	void	setCtrlSockfd(const char* id_str, int ctrlSockfd, int& indexSlot);
	// indexSlot 번째 slot 의 Data Socket 에 해당하는 값을 설정한다.
	//	0: Ok
	//	-1: 설정에 문제가 있다.
	//		사용가능한 slot 이 아니거나 이미 Data Socket 에 해당하는 값이 설정되어 있다.
	int	setDataSockfd(int dataSockfd, int indexSlot);

	int	closeSlot(int iSlot);

public:
	void	set__fd_set(
		int	ctrl_rfds__LIST[MAX_AGENT_SLOT],
		int&	n_ctrl_rfds__LIST,
		int	ctrl_wfds__LIST[MAX_AGENT_SLOT],
		int&	n_ctrl_wfds__LIST,
		int	data_rfds__LIST[MAX_AGENT_SLOT],
		int&	n_data_rfds__LIST
	);

private:
	int		m__autoFree;
	CQueueDatabaseThread*	m__queueDatabaseThread;
public:
	CQueueDatabaseThread*	getQueueDatabaseThread(void)	{return this->m__queueDatabaseThread;}
	CQueueDatabaseThread*	setQueueDatabaseThread(size_t queueSize);
	void		setQueueDatabaseThread(CQueueDatabaseThread* queueDatabaseThread);
};


////////////////////////////////////////////////////////////////////////////////
#endif	// !__AGENT_THREAD_H__
