#ifndef __gmTCP__
#define __gmTCP__
////////////////////////////////////////////////////////////////////////////////


// 3/19/2005 - ghkim
//	TCHAR.H를 포한해서 일반적인 문자를 다룬다.
//	자세한 설명은 MSDN의 "Generic-Text Mappings"을 참고한다.
#include <TCHAR.H>

#include <Winsock2.h>

////////////////////////////////
#include <math.h>	// ceil, log
////////////////////////////////
#include <gmTCP/gmTCPDef.h>

#include <gmCommon/myPRINT.h>

#include <gmTCP/gmQueue.h>
#include <gmTCP/gmTrans.h>
#include <gmTCP/gmPseudoTCP.h>
#define gmPostMessage(gmhTCP, lpTCPBuf) (((gmPseudoTCP*) gmhTCP)->mf__PostMessage(lpTCPBuf))
#define gmSendMessage(gmhTCP, lpTCPBuf) (((gmPseudoTCP*) gmhTCP)->mf__SendMessage(lpTCPBuf))
#define gmSendMessageEx(gmhTCP, lpTCPBuf, dwMS) (((gmPseudoTCP*) gmhTCP)->mf__SendMessageEx(lpTCPBuf, dwMS))
#define	gmGetIndex(gmhTCP)	(((gmPseudoTCP*) gmhTCP)->mf__GetGlobalIndex())
#include <gmTCP/gmTCPBase.h>

#include <gmTCP/gmAcceptEx.h>
#include <gmTCP/gmTCPServer.h>
#include <gmTCP/gmTCPClient.h>
////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
#endif	// __gmTCP__
