#define _INHERIT_kBODY
#ifdef _INHERIT_kBODY
#pragma warning(disable: 4250)
#endif
#include <afterShave/kShSimulationMayaFur.h>
#ifdef _INHERIT_kBODY
#pragma warning(default: 4250)
#else
#include <afterShave/kBodyMayaFur.h>
#endif
kShSimulationMayaFur::kShSimulationMayaFur(kTime::kTimeUnit timeUnit, unsigned nFrame, bool readOnly) :
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
	__bodyMayaPtr = new kBodyMayaFur;
	__simulationParameterPtr->pBody = dynamic_cast<ShSimBody*>(__bodyMayaPtr);
#endif
}
kShSimulationMayaFur::~kShSimulationMayaFur()
{
#ifndef _INHERIT_kBODY
	if (__bodyMayaPtr) {
		delete __bodyMayaPtr;
		__bodyMayaPtr = NULL;
	}
#endif
}
