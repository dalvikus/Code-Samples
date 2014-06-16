#ifndef __AGENT_SLOT_H__
#define __AGENT_SLOT_H__
////////////////////////////////////////////////////////////////////////////////


#include <Python.h>
#include "CSocketOUT.h"
#include "CDatabaseIN.h"
#include "CJobDown.h"
#include <assert.h>

class CAgentSlot : public CJobDown {
public:
	CAgentSlot(const char* id_str);
	~CAgentSlot();

private:
	int	m__ctrlSockfd;
	int	m__isCtrlSocketWritable;
	int	m__dataSockfd;
public:
	void	setCtrlSockfd(int ctrlSockfd)		{this->m__ctrlSockfd = ctrlSockfd;}
	void	setDataSockfd(int dataSockfd)		{this->m__dataSockfd = dataSockfd;}
	int	getCtrlSockfd(void)			{return this->m__ctrlSockfd;}
	int	getCtrlSocketWritable(void)		{return this->m__isCtrlSocketWritable;}
	int	getDataSockfd(void)			{return this->m__dataSockfd;}

public:
	CBuffer	m__ctrl_rPacket;
	CBuffer	m__wPacket;
	CBuffer	m__data_rPacket;
private:
	CSocketOUT*	m__ctrlSockOUT;
public:
	// Control Socket 에 쓸 때 사용되는 큐의 크기;
	//	Agent 를 교체할 때도 사용되므로, 적어도 Agent 는 담을 수 있어야 한다.
	static const size_t	CONTROL__WRITE_QUEUE_SIZE;
	void		setCtrlSocketOUT(void)
	{
		assert(this->m__ctrlSockfd >= 0);
		this->m__ctrlSockOUT = new CSocketOUT(this->m__ctrlSockfd, CONTROL__WRITE_QUEUE_SIZE);
	}
public:
	ssize_t	ctrlRead(void);
	int	ctrlWrite(void);
	ssize_t	dataRead(CDatabaseIN& databaseIN);
	int	timedoutWrite(void);

private:
	// Agent 가 내려받은 Command, Byte Stream, Script 의 집합
	PyObject*	m__ID__STR;
	PyObject*	m__commandID__SET;
	PyObject*	m__byteStreamID__SET;
	PyObject*	m__scriptID__SET;
public:
	PyObject*	getID__STR(void)		{return m__ID__STR;}
	PyObject*	getCommandID__SET(void)		{return m__commandID__SET;}
	PyObject*	getByteStreamID__SET(void)	{return m__byteStreamID__SET;}
	PyObject*	getScriptID__SET(void)		{return m__scriptID__SET;}

private:
	PyObject*	m__order__LIST;
public:
	PyObject*	getOrder__LIST(void)		{return this->m__order__LIST;}
public:
	int		pyEnqueOrder(PyObject* order);
	int		pyDoOrderDown(CBuffer& packet);
};


////////////////////////////////////////////////////////////////////////////////
#endif	// !__AGENT_SLOT_H__
