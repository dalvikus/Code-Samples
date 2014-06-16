#ifndef __WORKER_ARG_H__
#define __WORKER_ARG_H__
////////////////////////////////////////////////////////////////////////////////


#include "CWorker.h"
#include "CWorkerModule.h"

class CWorkerArg {
private:
	const CWorker*		m__pWorker;
	const CWorkerModule*	m__pWorkerModule;
public:
	void	setWorker(const CWorker* pWorker)			{this->m__pWorker = pWorker;}
	const CWorker*		getWorker(void) const			{return this->m__pWorker;}
	void	setWorkerModule(const CWorkerModule* pWorkerModule)	{this->m__pWorkerModule = pWorkerModule;}
	const CWorkerModule*	getWorkerModule(void) const		{return this->m__pWorkerModule;}

public:
	pthread_t	id;
	PyObject*	hCommand;	// 이 값을 Key 하여, hWorker 에서 해당하는 Val(hCommand) 을 얻는다.

public:
	CWorkerArg(void)
	{
		this->m__pWorkerModule = NULL;
	}
	~CWorkerArg()
	{
		if (m__pWorkerModule) {
			delete this->m__pWorkerModule;
			this->m__pWorkerModule = NULL;
		}
	}
};


////////////////////////////////////////////////////////////////////////////////
#endif	// __WORKER_ARG_H__

