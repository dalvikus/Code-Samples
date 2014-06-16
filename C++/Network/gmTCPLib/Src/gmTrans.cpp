#include <gmTCP/gmTCP.h>

gmTrans::gmTrans(
	UINT	QueueSizeInExponent,
	PINT	lpgmStatus
)
{
	INT	iReturnVal = 0;

	m__dwFlags	= 0;
	m__WSAOverlappedPlus.mWSAOverlapped.hEvent	= NULL;

	INT	gmStatus;
	m__lpQueue = new gmQueue(QueueSizeInExponent, &gmStatus);
	if (!m__lpQueue || gmStatus != 0) {
		iReturnVal = -1;
	}

	if (iReturnVal != 0) {
		if (m__lpQueue) {
			delete m__lpQueue;
			m__lpQueue = 0;
		}
	}

	if (lpgmStatus)
		*lpgmStatus = iReturnVal;
}

gmTrans::~gmTrans()
{
	if (m__lpQueue) {
		delete m__lpQueue;
		m__lpQueue = 0;
	}
}
