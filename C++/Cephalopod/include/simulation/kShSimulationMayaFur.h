#pragma once
#ifndef __kSH_SIMULATION_BODY_FUR_H__
#define __kSH_SIMULATION_BODY_FUR_H__
#include <afterShave/kSimulationCache.h>
#ifdef _INHERIT_kBODY
#include <afterShave/kBodyMayaFur.h>
#endif
class kShSimulationMayaFur : public kSimulationCache
#ifdef _INHERIT_kBODY
	,
	public kBodyMayaFur
#endif
{
public:
	kShSimulationMayaFur(kTime::kTimeUnit timeUnit = kTime::kTimeUnitFilm10, unsigned nFrame = 24, bool readOnly = false);
	virtual ~kShSimulationMayaFur();
};
#endif
