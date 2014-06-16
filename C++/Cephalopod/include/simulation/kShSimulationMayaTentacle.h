#pragma once
#ifndef __kSH_SIMULATION_BODY_TENTACLE_H__
#define __kSH_SIMULATION_BODY_TENTACLE_H__
#include <simulation/kSimulationCache.h>
#ifdef _INHERIT_kBODY
#include <simulation/kBodyMayaTentacle.h>
#endif
class kShSimulationMayaTentacle : public kSimulationCache
#ifdef _INHERIT_kBODY
	,
	public kBodyMayaTentacle
#endif
{
public:
	kShSimulationMayaTentacle(kTime::kTimeUnit timeUnit = kTime::kTimeUnitFilm10, unsigned nFrame = 24, bool readOnly = false);
	virtual ~kShSimulationMayaTentacle();
};
#endif
