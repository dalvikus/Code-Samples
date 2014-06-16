#include <simulation/kFrameCache.h>
#if 1
#include "../../../SBD/SBDlib/src_allheader/ShSimWind.h"
#include "../../../SBD/SBDlib/src_allheader/ShSimCollisionBody.h"
#else
#include <SHD/ShSimWind.h>
#include <SHD/ShSimCollisionBody.h>
#endif
#include <assert.h>
kFrameCache::kFrameCache(int indexFrame) : __indexFrame(indexFrame), __isParameterDirty(true)
{
}
kFrameCache::~kFrameCache()
{
}
#if 1
kStrandCachePtr	kFrameCache::addStrandCache(int strandId, const KVector& segmentLengthArray)
#else
kStrandCachePtr	kFrameCache::addStrandCache(int strandId, unsigned nSegment)
#endif
{
#if 1
	kStrandCachePtr	p = new kStrandCache(strandId, const_cast<KVector&>(segmentLengthArray));
#else
	kStrandCachePtr	p = new kStrandCache(strandId, nSegment);
#endif
	__strandCacheArray.append(p);
	return p;
}
void	kFrameCache::popStrandCache()
{
	int	lastIndex = (int) __strandCacheArray.length() - 1;
	if (lastIndex < 0)
		return;
	kStrandCachePtr&	p = __strandCacheArray[lastIndex];
	delete p;
	p = 0;
	__strandCacheArray.pop();
}
void	kFrameCache::clear()
{
	__strandCacheArray.clear();
}
void	kFrameCache::setParameter(const ShSimParameter& parameter)
{
#if 1
	__parameter.includeStiffness		= parameter.includeStiffness;
	__parameter.fYoungModulus		= parameter.fYoungModulus;
#if 1
#if 1
	__parameter.fAverageRadius		= parameter.sSection.fAverageRadius;
	__parameter.fEllipticity		= parameter.sSection.fEllipticity;
#else
	__parameter.fAverageRadius		= parameter.fAverageRadius;
	__parameter.fEllipticity		= parameter.fEllipticity;
#endif
	__parameter.fVolumeDensity		= parameter.fVolumeDensity;
	__parameter.fHelixK0STep		= parameter.fHelixK0STep;
	__parameter.fHelixK1Curvature		= parameter.fHelixK1Curvature;
	__parameter.fLengthFactor		= parameter.fLengthFactor;
#endif
	__parameter.includeGravityAndInertia		= parameter.includeGravityAndInertia;
	__parameter.fGravityValue		= parameter.fGravityValue;
	__parameter.includeAirFriction		= parameter.includeAirFriction;
	__parameter.fAirFriction		= parameter.fAirFriction;
	__parameter.choiceDissipation		= parameter.choiceDissipation;
	__parameter.fLambdaDissipation		= parameter.fLambdaDissipation;
	__parameter.fGammaDissipation		= parameter.fGammaDissipation;
	__parameter.choiceWind			= parameter.choiceWind;
#if 1
	__parameter.fWindRatio			= parameter.fWindRatio;
	__parameter.fWindTheta			= parameter.fWindTheta;
	__parameter.fWindPhi			= parameter.fWindPhi;
#else
	__parameter.hasWind = parameter.pWind != NULL;
	if (__parameter.hasWind) {
		ShSimWind&	wind = *((ShSimWind*) parameter.pWind);
		__parameter.WindRatio = wind.getWindRatio();
		__parameter.WindDirection2D = wind.getWindDirection2D();
		wind.getWindDirection3D(__parameter.WindDirection3D);
	}
#endif
	__parameter.includeHairBodyOptimalCollision		= parameter.includeHairBodyOptimalCollision;
	__parameter.includeHairBodyCollision	= parameter.includeHairBodyCollision;
#if 1
	__parameter.fHBCRatio			= parameter.fHBCRatio;
	__parameter.fHBCDepth			= parameter.fHBCDepth;
#else
	__parameter.hasHBCParameter = parameter.pCollBody != NULL;
	if (__parameter.hasHBCParameter) {
		ShSimCollisionBody&	body = *((ShSimCollisionBody*) parameter.pCollBody);
		body.getHBCParameter(&__parameter.HBCParameter_ratio, &__parameter.HBCParameter_criticalDepth);
	}
#endif
	__parameter.includeHairHairCollision	= parameter.includeHairHairCollision;
#else
#if 1
	__currentRace = parameter.currentRace;
#else
	__parameter = parameter;
#endif
#endif
	__isParameterDirty = false;
}
bool	kFrameCache::isParameterChanged(const ShSimParameter& parameter, bool reset)
{
	bool	isChanged = false;
#if 1
#pragma warning(disable: 4127)
	do {
		if (__parameter.includeStiffness != parameter.includeStiffness) {
			isChanged = true;
			break;
		}
		if (__parameter.fYoungModulus != parameter.fYoungModulus) {
			isChanged = true;
			break;
		}
#if 1
#if 1
		if (__parameter.fAverageRadius != parameter.sSection.fAverageRadius) {
			isChanged = true;
			break;
		}
		if (__parameter.fEllipticity != parameter.sSection.fEllipticity) {
			isChanged = true;
			break;
		}
#else
		if (__parameter.fAverageRadius != parameter.fAverageRadius) {
			isChanged = true;
			break;
		}
		if (__parameter.fEllipticity != parameter.fEllipticity) {
			isChanged = true;
			break;
		}
#endif
		if (__parameter.fVolumeDensity != parameter.fVolumeDensity) {
			isChanged = true;
			break;
		}
		if (__parameter.fHelixK0STep != parameter.fHelixK0STep) {
			isChanged = true;
			break;
		}
		if (__parameter.fHelixK1Curvature != parameter.fHelixK1Curvature) {
			isChanged = true;
			break;
		}
		if (__parameter.fLengthFactor != parameter.fLengthFactor) {
			isChanged = true;
			break;
		}
#endif
		if (__parameter.includeGravityAndInertia != parameter.includeGravityAndInertia) {
			isChanged = true;
			break;
		}
		if (__parameter.fGravityValue != parameter.fGravityValue) {
			isChanged = true;
			break;
		}
		if (__parameter.includeAirFriction != parameter.includeAirFriction) {
			isChanged = true;
			break;
		}
		if (__parameter.fAirFriction != parameter.fAirFriction) {
			isChanged = true;
			break;
		}
		if (__parameter.choiceDissipation != parameter.choiceDissipation) {
			isChanged = true;
			break;
		}
		if (__parameter.fLambdaDissipation != parameter.fLambdaDissipation) {
			isChanged = true;
			break;
		}
		if (__parameter.fGammaDissipation != parameter.fGammaDissipation) {
			isChanged = true;
			break;
		}
		if (__parameter.choiceWind != parameter.choiceWind) {
			isChanged = true;
			break;
		}
#if 1
		if (__parameter.fWindRatio != parameter.fWindRatio) {
			isChanged = true;
			break;
		}
		if (__parameter.fWindTheta != parameter.fWindTheta) {
			isChanged = true;
			break;
		}
		if (__parameter.fWindPhi != parameter.fWindPhi) {
			isChanged = true;
			break;
		}
#else
		bool	hasWind = parameter.pWind != NULL;
		if (hasWind != __parameter.hasWind) {
			isChanged = true;
			break;
		}
		if (hasWind) {
			ShSimWind&	wind = *((ShSimWind*) parameter.pWind);
			ShScalar	WindRatio = wind.getWindRatio();
			if (__parameter.WindRatio != WindRatio) {
				isChanged = true;
				break;
			}
			ShScalar	WindDirection2D = wind.getWindDirection2D();
			if (__parameter.WindDirection2D != WindDirection2D) {
				isChanged = true;
				break;
			}
			ShVector3	WindDirection3D;
			wind.getWindDirection3D(WindDirection3D);
			if (WindDirection3D[0] != __parameter.WindDirection3D[0] || WindDirection3D[1] != __parameter.WindDirection3D[1] || WindDirection3D[2] != __parameter.WindDirection3D[2]) {
				isChanged = true;
				break;
			}
		}
#endif
		if (__parameter.includeHairBodyOptimalCollision != parameter.includeHairBodyOptimalCollision) {
			isChanged = true;
			break;
		}
		if (__parameter.includeHairBodyCollision != parameter.includeHairBodyCollision) {
			isChanged = true;
			break;
		}
#if 1
		if (__parameter.fHBCRatio != parameter.fHBCRatio) {
			isChanged = true;
			break;
		}
		if (__parameter.fHBCDepth != parameter.fHBCDepth) {
			isChanged = true;
			break;
		}
#else
		bool	hasHBCParameter = parameter.pCollBody != NULL;
		if (hasHBCParameter != __parameter.hasHBCParameter) {
			isChanged = true;
			break;
		}
		if (hasHBCParameter) {
			ShSimCollisionBody&	body = *((ShSimCollisionBody*) parameter.pCollBody);
			ShScalar	ratio, criticalDepth;
			body.getHBCParameter(&ratio, &criticalDepth);
			if (__parameter.HBCParameter_ratio != ratio || __parameter.HBCParameter_criticalDepth != criticalDepth) {
				isChanged = true;
				break;
			}
		}
#endif
		if (__parameter.includeHairHairCollision != parameter.includeHairHairCollision) {
			isChanged = true;
			break;
		}
	} while (0);
#pragma warning(default: 4127)
#else
#if 1
	isChanged = __currentRace != parameter.currentRace;
#else
	isChanged = __parameter.currentRace != parameter.currentRace;
#endif
#endif
	if (isChanged && reset)
		setParameter(parameter);
	return isChanged;
}
void	kFrameCache::setCachedFalse(int indexStrand)
{
	for (int i = 0; i < (int) __strandCacheArray.length(); ++i) {
		kStrandCachePtr	p = __strandCacheArray[i];
		if (!p)
			continue;
		if (indexStrand >= 0) {
			if (i == indexStrand) {
				p->setCachedFalse();
				break;
			}
		} else
			p->setCachedFalse();
	}
}
bool	kFrameCache::isCached() const
{
	unsigned	numSimulated = 0;
	unsigned	numCached = 0;
	for (unsigned i = 0; i < __strandCacheArray.length(); ++i) {
		const kStrandCachePtr&	p = __strandCacheArray[i];
		if (!p)
			continue;
		if (p->isSimulated()) {
			++numSimulated;
			if (p->isCached())
				++numCached;
		}
	}
	return numCached && numCached == numSimulated;
}
