#define _INHERIT_kBODY
#ifdef _INHERIT_kBODY
#pragma warning(disable: 4250)
#endif
#include <simulation/kShSimulationMayaTentacle.h>
#ifdef _INHERIT_kBODY
#pragma warning(default: 4250)
#else
#include <simulation/kBodyMayaTentacle.h>
#endif
kShSimulationMayaTentacle::kShSimulationMayaTentacle(kTime::kTimeUnit timeUnit, unsigned nFrame, bool readOnly) :
	kSimulationCache(
#ifndef _INHERIT_kBODY
		NULL,
#endif
		timeUnit, nFrame, readOnly
	)
{
#ifdef _INHERIT_kBODY
	__simulationParameterPtr->pBody = dynamic_cast<ShSimBody*>(this);
#else
	__bodyMayaPtr = new kBodyMayaTentacle;
	__simulationParameterPtr->pBody = dynamic_cast<ShSimBody*>(__bodyMayaPtr);
#endif
}
kShSimulationMayaTentacle::~kShSimulationMayaTentacle()
{
#ifndef _INHERIT_kBODY
	if (__bodyMayaPtr) {
		delete __bodyMayaPtr;
		__bodyMayaPtr = NULL;
	}
#endif
}
