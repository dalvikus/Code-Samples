#ifndef __WORKER_MODULE_H__
#define __WORKER_MODULE_H__
////////////////////////////////////////////////////////////////////////////////


class CWorkerModule {
private:
	void*	m__handle;
	void*	m__entryPoint;
public:
	const void*	getHandle(void) const		{return m__handle;}
	const void*	getEntryPoint(void) const	{return m__entryPoint;}

public:
	CWorkerModule(const char* entryPoint, const char* libPathname);
	~CWorkerModule();
};


////////////////////////////////////////////////////////////////////////////////
#endif	// !__WORKER_MODULE_H__
