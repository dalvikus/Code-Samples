#include "CAgentThread.h"
#include "eprintf.h"
#include <sys/poll.h>

//#define __USE_POLL__
void
thread_main(
	void*	voidPtr
)
{
	CAgentThread&	thread = *((CAgentThread*) voidPtr);
sleep(3);
	CDatabaseIN&	databaseIN = *((CDatabaseIN*) thread.getQueueDatabaseThread());
	CAgentSlot**	slots = thread.slots();

	int	ctrl_rfds__LIST[CAgentThread::MAX_AGENT_SLOT];
	int	n_ctrl_rfds__LIST;
	int	ctrl_wfds__LIST[CAgentThread::MAX_AGENT_SLOT];
	int	n_ctrl_wfds__LIST;
	int	data_rfds__LIST[CAgentThread::MAX_AGENT_SLOT];
	int	n_data_rfds__LIST;
	while (1) {
		thread.set__fd_set(
			ctrl_rfds__LIST, n_ctrl_rfds__LIST,
			ctrl_wfds__LIST, n_ctrl_wfds__LIST,
			data_rfds__LIST, n_data_rfds__LIST
		);
#ifdef __USE_POLL__
		////////////////////////////////////////////////////////////////////////
		struct pollfd	ufds[3 * CAgentThread::MAX_AGENT_SLOT];
		for (int i = 0; i < n_ctrl_rfds__LIST; ++i) {
			ufds[i].fd = ctrl_rfds__LIST[i] & 0xffff;
			ufds[i].events = POLLIN;
		}
		for (int i = 0; i < n_ctrl_wfds__LIST; ++i) {
			ufds[n_ctrl_rfds__LIST + i].fd = ctrl_wfds__LIST[i] & 0xffff;
			ufds[n_ctrl_rfds__LIST + i].events = POLLOUT;
		}
		for (int i = 0; i < n_data_rfds__LIST; ++i) {
			ufds[n_ctrl_rfds__LIST + n_ctrl_wfds__LIST + i].fd = data_rfds__LIST[i] & 0xffff;
			ufds[n_ctrl_rfds__LIST + n_ctrl_wfds__LIST + i].events = POLLIN;
		}
#else	// !__USE_POLL__/__USE_SELECT__
		////////////////////////////////////////////////////////////////////////
		fd_set	rfds;
		fd_set	wfds;
		int	fdmaxplus1;
		fdmaxplus1 = -1;
		FD_ZERO(&rfds);
		for (int i = 0; i < n_ctrl_rfds__LIST; ++i) {
			int	fd = ctrl_rfds__LIST[i] & 0xffff;
			if (fd > fdmaxplus1)
				fdmaxplus1 = fd;
			FD_SET(fd, &rfds);
		}
		for (int i = 0; i < n_data_rfds__LIST; ++i) {
			int	fd = data_rfds__LIST[i] & 0xffff;
			if (fd > fdmaxplus1)
				fdmaxplus1 = fd;
			FD_SET(fd, &rfds);
		}
		FD_ZERO(&wfds);
		for (int i = 0; i < n_ctrl_wfds__LIST; ++i) {
			int	fd = ctrl_wfds__LIST[i] & 0xffff;
			if (fd > fdmaxplus1)
				fdmaxplus1 = fd;
			FD_SET(fd, &wfds);
		}
		++fdmaxplus1;
#endif	// !__USE_SELECT__
//(void) printf("fdmaxplus1 = %d, n_ctrl_wfds__LIST = %d, n_ctrl_rfds__LIST = %d, n_data_rfds__LIST = %d\n", fdmaxplus1, n_ctrl_wfds__LIST, n_ctrl_rfds__LIST, n_data_rfds__LIST);
		////////////////////////////////////////////////////////////////////////

#ifdef __USE_POLL__
		int	retval = poll(ufds, n_ctrl_rfds__LIST + n_ctrl_wfds__LIST + n_data_rfds__LIST, 1000);
		if (retval < 0) {
			eprintf(E_ERR, EFMT("poll(): Failure: %m"));
			continue;
		}
#else	// !__USE_POLL__/__USE_SELECT__
		struct timeval  tv;
		tv.tv_sec = 1;
		tv.tv_usec = 0;
		int	retval = select(fdmaxplus1, &rfds, &wfds, NULL, &tv);
		if (retval < 0) {
			eprintf(E_ERR, EFMT("select(): Failure: %m"));
			continue;
		}
#endif	// !__USE_SELECT__

		for (int i = 0; i < n_ctrl_wfds__LIST; ++i) {
			int	ctrl_sockfd = ctrl_wfds__LIST[i];
#ifdef __USE_POLL__
			if (!(ufds[n_ctrl_rfds__LIST + i].revents & POLLOUT))
				continue;
#else	// !__USE_POLL__/__USE_SELECT__
			if (!FD_ISSET(ctrl_sockfd & 0xffff, &wfds))
				continue;
#endif	// !__USE_SELECT__

			int	iSlot = ctrl_sockfd >> 16;
			if (slots[iSlot]->ctrlWrite() < 0) {
				eprintf(E_ERR, EFMT("CAgentSlot::ctrlWrite(): Failure"));
			}
		}

		for (int i = 0; i < n_ctrl_rfds__LIST; ++i) {
			int	ctrl_sockfd = ctrl_rfds__LIST[i];
#ifdef __USE_POLL__
			if (!(ufds[i].revents & POLLIN))
				continue;
#else	// !__USE_POLL__/__USE_SELECT__
			if (!FD_ISSET(ctrl_sockfd & 0xffff, &rfds))
				continue;
#endif	// !__USE_SELECT__

			int	iSlot = ctrl_sockfd >> 16;
			if (slots[iSlot] == NULL) {
				continue;
			}
			ssize_t	nread = slots[iSlot]->ctrlRead();
			if (nread < 0) {
				eprintf(E_ERR, EFMT("CAgentSlot::ctrlRead(): Failure"));
			} else if (nread == 0) {
				if (thread.closeSlot(iSlot) < 0) {
					eprintf(E_ERR, EFMT("CAgentThread::closeSlot(): Failure"));
				}
			}
		}

		for (int i = 0; i < n_data_rfds__LIST; ++i) {
			int	data_sockfd = data_rfds__LIST[i];
#ifdef __USE_POLL__
			if (!(ufds[n_ctrl_rfds__LIST + n_ctrl_wfds__LIST + i].revents & POLLIN))
				continue;
#else	// !__USE_POLL__/__USE_SELECT__
			if (!FD_ISSET(data_sockfd & 0xffff, &rfds))
				continue;
#endif	// !__USE_SELECT__

			int	iSlot = data_sockfd >> 16;
			if (slots[iSlot] == NULL) {
				continue;
			}
			ssize_t	nread = slots[iSlot]->dataRead(databaseIN);
			if (nread < 0) {
				eprintf(E_ERR, EFMT("CAgentSlot::dataRead(): Failure"));
			} else if (nread == 0) {
				if (thread.closeSlot(iSlot) < 0) {
					eprintf(E_ERR, EFMT("CAgentThread::closeSlot(): Failure"));
				}
			}
		}

		// TIMEDOUT
		for (int i = 0; i < CAgentThread::MAX_AGENT_SLOT; ++i) {
			CAgentSlot*	slot = NULL;
			thread.lock();
			slot = slots[i];
			thread.unlock();
			if (!slot)
				continue;

			if (slot->timedoutWrite() < 0) {
				eprintf(E_ERR, EFMT("CAgentSlot::timedoutWrite(): Failure"));
			}
		}
	}
}


CAgentThread::CAgentThread(
	void
) : CThread(thread_main, this)
{
	for (int i = 0; i < MAX_AGENT_SLOT; ++i)
		this->m__slot[i] = NULL;
	this->m__indexOfNextSlot = 0;

	this->m__autoFree = 0;
	this->m__queueDatabaseThread = NULL;
}

CAgentThread::~CAgentThread()
{
	for (int i = 0; i < MAX_AGENT_SLOT; ++i) {
		if (this->m__slot[i]) {
			delete this->m__slot[i];
			this->m__slot[i] = NULL;
		}
	}

	if (this->m__autoFree && this->m__queueDatabaseThread) {
		delete this->m__queueDatabaseThread;
		this->m__queueDatabaseThread = NULL;
	}
}


void
CAgentThread::setCtrlSockfd(
	const char*	id_str,
	int	ctrlSockfd,
	int&	indexSlot
)
{
	this->lock();

do {
	if (this->m__indexOfNextSlot == -1) {
		indexSlot = -1;
		break;
	}

	indexSlot = this->m__indexOfNextSlot;
	if (this->m__slot[indexSlot] == NULL)
		this->m__slot[indexSlot] = new CAgentSlot(id_str);
	this->m__slot[indexSlot]->setCtrlSockfd(ctrlSockfd);
	this->m__slot[indexSlot]->setCtrlSocketOUT();

	// 다음에 사용가능한 slot 을 찾는다.
	while (1) {
		++this->m__indexOfNextSlot;
		if (this->m__indexOfNextSlot == indexSlot) {	// 한 바퀴 돌았다.
			// 더 이상 가능한 slot 이 없다.
			this->m__indexOfNextSlot = -1;
			break;
		}
		if (this->m__indexOfNextSlot == MAX_AGENT_SLOT)
			this->m__indexOfNextSlot = 0;
		if (this->m__slot[this->m__indexOfNextSlot] == NULL || this->m__slot[this->m__indexOfNextSlot]->getCtrlSockfd() == -1) {	// 다음에는 이 slot 을 이용한다.
			break;
		}
	}
} while (0);

	this->unlock();
}

int
CAgentThread::setDataSockfd(
	int	dataSockfd,
	int	indexSlot
)
{
	int	iReturnVal = 0;
	this->lock();

do {
	if (indexSlot < 0 || indexSlot >= MAX_AGENT_SLOT) {
		eprintf(E_ERR, EFMT("indexSlot = %d: Out of range"), indexSlot);
		iReturnVal = -1;
		break;
	}

	if (this->m__slot[indexSlot] == NULL) {
		eprintf(E_ERR, EFMT("slot[%d]: slot is NULL"), indexSlot);
		iReturnVal = -1;
		break;
	}

	CAgentSlot&	slot = *(this->m__slot[indexSlot]);
	if (slot.getCtrlSockfd() == -1) {
		eprintf(E_ERR, EFMT("slot[%d]: Control Socket not set yet"), indexSlot);
		iReturnVal = -1;
		break;
	}

	slot.setDataSockfd(dataSockfd);
	iReturnVal = 0;
} while (0);

	this->unlock();
	return iReturnVal;
}

int
CAgentThread::closeSlot(
	int	iSlot
)
{
	if (iSlot < 0 || iSlot >= MAX_AGENT_SLOT) {
		eprintf(E_ERR, EFMT("CAgentThread::closeSlot(): iSlot = %d: Out of range(0 <= iSlot < MAX_AGENT_SLOT)"), iSlot);
		return -1;
	}
	CAgentSlot*	slot = this->m__slot[iSlot];
	if (slot == NULL) {
		eprintf(E_ERR, EFMT("CAgentSlot::closeSlot(): slot[iSlot = %d] is NULL"), iSlot);
		return -1;
	}

	delete slot;
	this->m__slot[iSlot] = NULL;
	if (this->m__indexOfNextSlot == -1)
		this->m__indexOfNextSlot = iSlot;

	return 0;
}

void
CAgentThread::set__fd_set(
	int	ctrl_rfds__LIST[MAX_AGENT_SLOT],
	int&	n_ctrl_rfds__LIST,
	int	ctrl_wfds__LIST[MAX_AGENT_SLOT],
	int&	n_ctrl_wfds__LIST,
	int	data_rfds__LIST[MAX_AGENT_SLOT],
	int&	n_data_rfds__LIST
)
{
	this->lock();
	n_ctrl_rfds__LIST = 0;
	n_ctrl_wfds__LIST = 0;
	n_data_rfds__LIST = 0;
do {
	for (int i = 0; i < CAgentThread::MAX_AGENT_SLOT; ++i) {
		if (this->m__slot[i] == NULL)
			continue;
		CAgentSlot&	slot = *(this->m__slot[i]);
		int	ctrlSockfd;
		if ((ctrlSockfd = slot.getCtrlSockfd()) == -1)
			continue;

		if (ctrlSockfd >= FD_SETSIZE) {
			eprintf(E_ERR, EFMT("ctrlSockfd(= %d) >= FD_SETSIZE: select() impossible"), ctrlSockfd);
			continue;
		}
		ctrl_rfds__LIST[n_ctrl_rfds__LIST++] = (i << 16) | ctrlSockfd;
		if (slot.getCtrlSocketWritable())
			ctrl_wfds__LIST[n_ctrl_wfds__LIST++] = (i << 16) | ctrlSockfd;

		int	dataSockfd = slot.getDataSockfd();
		if (dataSockfd >= FD_SETSIZE) {
			eprintf(E_ERR, EFMT("dataSockfd(= %d) >= FD_SETSIZE: select() impossible"), dataSockfd);
			continue;
		}
		if (dataSockfd == -1)
			continue;
		data_rfds__LIST[n_data_rfds__LIST++] = (i << 16) | dataSockfd;
	}
} while(0);
	this->unlock();
}

CQueueDatabaseThread*
CAgentThread::setQueueDatabaseThread(
	size_t queueSize
)
{
	this->m__autoFree = 1;
	this->m__queueDatabaseThread = new CQueueDatabaseThread(queueSize);
	return this->m__queueDatabaseThread;
}
void
CAgentThread::setQueueDatabaseThread(
	CQueueDatabaseThread*	queueDatabaseThread
)
{
	this->m__autoFree = 0;
	this->m__queueDatabaseThread = queueDatabaseThread;
}
