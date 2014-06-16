#include <simulation/kStrandCache.h>
#include <assert.h>
double	kStrandCache::TVector_kTol = 1.e-10;
kStrandCache::kStrandCache(
	int		indexStrand,
#if 1
	 KVector&	segmentLengthArray
#else
	unsigned	nSegment
#endif
) :
	__state(kSimulated | ~kCached),
	__indexStrand(indexStrand),
#if 1
	__segmentLengthArray(&segmentLengthArray),
	__data(segmentLengthArray.size())
#else
	__nSegment(nSegment),
	__data(nSegment)
#endif
{
	segmentLengthArray.print("kStrandCache::kStrandCache(): segmentLengthArray");
}
kStrandCache::~kStrandCache()
{
}
int	kStrandCache::pushCachedData(const ShSimStrand& simulator, kBody::TVector rootTVector)
{
	ShSimStrand&	non_const_simulator = *((ShSimStrand*) &simulator);
	int	nSegment = non_const_simulator.getCurvatureVectorList(&__data.curvatureData.vector);
#if 1
	assert(nSegment == (int) __segmentLengthArray.size());
#else
	assert(nSegment == (int) __nSegment);
#endif
	if (nSegment < 0)
		return -1;
	nSegment = non_const_simulator.getCurvatureVelocityVectorList(&__data.curvatureData.velocityVector);
#if 1
	assert(nSegment == (int) __segmentLengthArray.size());
#else
	assert(nSegment == (int) __nSegment);
#endif
	if (nSegment < 0)
		return -2;
	__data.rootTVector = rootTVector;
	__state |= kCached;
	return 0;
}
int	kStrandCache::popCachedData(ShSimStrand& simulator)
{
	assert(__state & kCached);
	if (!(__state & kCached))
		return -1;
#if 1
#if 1
	simulator.setStrandLengthStiffnessUsingDiffernentSegmentLength(&__segmentLengthArray);
#endif
	simulator.setCoordinateOnly(&__data.curvatureData.vector, NULL);
	simulator.setVelocityOnly(&__data.curvatureData.velocityVector);
#else
	simulator.setCoordinateVelocity(&__data.curvatureData.vector, &__data.curvatureData.velocityVector);
#endif
	return 0;
}
inline bool	isVectorEquivalent(const ShVector3& a, const ShVector3& b, double tolerance)
{
#if	0 && defined(_DEBUG)
	double	length = sqrt((a[0] - b[0]) * (a[0] - b[0]) + (a[1] - b[1]) * (a[1] - b[1]) + (a[2] - b[2]) * (a[2] - b[2]));
	(void) printf("a: [%f, %f, %f], b: [%f, %f, %f], length(a - b) = %f\n", a[0], a[1], a[2], b[0], b[1], b[2], length);
	return length < tolerance;
#else
	return sqrt((a[0] - b[0]) * (a[0] - b[0]) + (a[1] - b[1]) * (a[1] - b[1]) + (a[2] - b[2]) * (a[2] - b[2])) < tolerance;
#endif
}
inline bool	isTVectorChanged(const kBody::TVector& __rootTVector, const kBody::TVector& rootTVector, double tolerance)
{
	const ShVector9&	T0a = __rootTVector.T0;
	const ShVector9&	T0b = rootTVector.T0;
	if (!isVectorEquivalent(T0a.position, T0b.position, tolerance))
		return true;
	if (!isVectorEquivalent(T0a.tangent, T0b.tangent, tolerance))
		return true;
	if (!isVectorEquivalent(T0a.normal, T0b.normal, tolerance))
		return true;
	const ShVector6&	T1a = __rootTVector.T1;
	const ShVector6&	T1b = rootTVector.T1;
	if (!isVectorEquivalent(T1a.position, T1b.position, tolerance))
		return true;
	if (!isVectorEquivalent(T1a.tangent, T1b.tangent, tolerance))
		return true;
	const ShVector6&	T2a = __rootTVector.T2;
	const ShVector6&	T2b = rootTVector.T2;
	if (!isVectorEquivalent(T2a.position, T2b.position, tolerance))
		return true;
	if (!isVectorEquivalent(T2a.tangent, T2b.tangent, tolerance))
		return true;
	return false;
}
bool	kStrandCache::isRootTVectorChanged(const kBody::TVector& rootTVector, bool bSetCacheFalse, double tolerance) const
{
	bool	b = isTVectorChanged(__data.rootTVector, rootTVector, tolerance);
	if (b && bSetCacheFalse) {
		kStrandCache*	nonConstThis = (kStrandCache*) this;
		nonConstThis->setCachedFalse();
	}
	return b;
}
