#pragma once
#include <baseShape/kPtrArray.h>
#include <simulation/kStrandCache.h>
#define OS	1
#if 1
#include "../../../SBD/SBDlib/src_allheader/ShSimParameter.h"
#else
	#include <SHD/ShSimParameter.h>
#endif
#undef LINUX
#undef MSWIN
#undef TT
#undef CONSOLE
#undef OS
class kFrameCache
{
private:
	int		__indexFrame;
public:
	int		indexFrame() const	{return __indexFrame;}
private:
#if 0
numRaceType	ShSimParameter::currentRace
bool		ShSimParameter::includeStiffness
ShScalar	ShSimParameter::fYoungModulus
bool		ShSimParameter::includeGravityAndInertia
ShScalar	ShSimParameter::fGravityValue
bool		ShSimParameter::includeAirFriction
ShScalar	ShSimParameter::fAirFriction
numDissipationMode	ShSimParameter::choiceDissipation
ShScalar	ShSimParameter::fLambdaDissipation
ShScalar	ShSimParameter::fGammaDissipation
numWindMode	ShSimParameter::choiceWind
ShScalar	ShSimWind.getWindRatio();
ShScalar	ShSimWind.getWindDirection2D();
void		ShSimWind.getWindDirection3D(ShVector3 direction3D);
bool		ShSimParameter::includeHairBodyOptimalCollision
bool		ShSimParameter::includeHairBodyCollision
void		ShSimCollisionBody.getHBCParameter(ShScalar* ratio, ShScalar* criticalDepth);
bool		ShSimParameter::includeHairHairCollision
#endif
	struct kSimulationParameter {
		bool		includeStiffness;
		ShScalar	fYoungModulus;
#if 1
		ShScalar	fAverageRadius;
		ShScalar	fEllipticity;
		ShScalar	fVolumeDensity;
		ShScalar	fHelixK0STep;
		ShScalar	fHelixK1Curvature;
		ShScalar	fLengthFactor;
#endif
		bool		includeGravityAndInertia;
		ShScalar	fGravityValue;
		bool		includeAirFriction;
		ShScalar	fAirFriction;
		numDissipationMode	choiceDissipation;
		ShScalar	fLambdaDissipation;
		ShScalar	fGammaDissipation;
		numWindMode	choiceWind;
#if 1
		ShScalar	fWindRatio;
		ShScalar	fWindTheta;
		ShScalar	fWindPhi;
#else
		bool		hasWind;
		ShScalar	WindRatio;
		ShScalar	WindDirection2D;
		ShVector3	WindDirection3D;
#endif
		bool		includeHairBodyOptimalCollision;
		bool		includeHairBodyCollision;
#if 1
		ShScalar	fHBCRatio;
		ShScalar	fHBCDepth;
#else
		bool		hasHBCParameter;
		ShScalar	HBCParameter_ratio;
		ShScalar	HBCParameter_criticalDepth;
#endif
		bool		includeHairHairCollision;
	};
#if 1
	kSimulationParameter	__parameter;
public:
	void	setYoungModulus(ShScalar fYoungModulus)		{__parameter.fYoungModulus = fYoungModulus;}
#if 1
	void	setAverageRadius(ShScalar fAverageRadius)	{__parameter.fAverageRadius = fAverageRadius;}
	void	setEllipticity(ShScalar fEllipticity)		{__parameter.fEllipticity = fEllipticity;}
	void	setVolumeDensity(ShScalar fVolumeDensity)	{__parameter.fVolumeDensity = fVolumeDensity;}
	void	setHelixK0STep(ShScalar fHelixK0STep)		{__parameter.fHelixK0STep = fHelixK0STep;}
	void	setHelixK1Curvature(ShScalar fHelixK1Curvature)	{__parameter.fHelixK1Curvature = fHelixK1Curvature;}
#endif
#else
#if 1
	numRaceType	__currentRace;
#else
	ShSimParameter	__parameter;
#endif
#endif
private:
	kStrandCacheArray	__strandCacheArray;
public:
	kStrandCacheArray&		strandCacheArray()		{return __strandCacheArray;}
	const kStrandCacheArray&	strandCacheArray() const	{return __strandCacheArray;}
public:
#if 1
	kStrandCachePtr	addStrandCache(int strandId, const KVector& segmentLengthArray);
#else
	kStrandCachePtr	addStrandCache(int strandId, unsigned nSegment);
#endif
	void	popStrandCache();
	void	clear();
public:
	kFrameCache(int indexFrame);
	~kFrameCache();
private:
	bool	__isParameterDirty;
public:
	bool	isParameterDirty() const	{return __isParameterDirty;}
	void	setParameterDirtyTrue()		{__isParameterDirty = true;}
public:
	void	setParameter(const ShSimParameter& parameter);
public:
	bool	isParameterChanged(const ShSimParameter& parameter, bool reset = true);
	void	setCachedFalse(int indexStrand = -1);
public:
	bool	isCached() const;
};
typedef kFrameCache*		kFrameCachePtr;
