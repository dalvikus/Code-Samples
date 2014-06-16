#pragma once
#include <baseShape/kPtrArray.h>
#include <simulation/kBody.h>
#if 1
#include "../../../SBD/SBDlib/src_allheader/kvector.h"
#include "../../../SBD/SBDlib/src_allheader/SuperHelixType.h"
#else
#include <SHD/kvector.h>
#include <SHD/SuperHelixType.h>
#endif
#define OS	1
#if 1
#include "../../../SBD/SBDlib/src_allheader/ShSimStrand.h"
#else
	#include <SHD/ShSimStrand.h>
#endif
#undef LINUX
#undef MSWIN
#undef TT
#undef CONSOLE
#undef OS
class kStrandCache
{
public:
	struct CurvatureData
	{
		KVector		vector;
		KVector		velocityVector;
		CurvatureData(unsigned nSegment) :
			vector(3 * nSegment),
			velocityVector(3 * nSegment)
		{}
	};
	typedef CurvatureData*	CurvatureDataPtr;
	struct CacheData
	{
		kBody::TVector	rootTVector;
#if 1
		CurvatureData	curvatureData;
		CacheData(unsigned nSegment) : curvatureData(nSegment) {}
#else
		KVector		curvatureVector;
		KVector		curvatureVelocityVector;
		CacheData(unsigned nSegment) : curvatureVector(3 * nSegment), curvatureVelocityVector(3 * nSegment) {}
#endif
	};
private:
	CacheData	__data;
public:
	const CacheData&	data() const	{return __data;}
	CacheData&	data() 			{return __data;}
private:
	int	__indexStrand;
public:
	int	indexStrand() const	{return __indexStrand;}
public:
#if 1
	kStrandCache(int indexStrand,  KVector& segmentLengthArray);
#else
	kStrandCache(int indexStrand, unsigned nSegment);
#endif
	~kStrandCache();
public:
	enum {
		kSimulated	= 1 << 0,
		kCached		= 1 << 1,
	};
public:
	typedef int	kCacheState;
private:
	kCacheState	__state;
public:
	static double	TVector_kTol;
	bool	isRootTVectorChanged(const kBody::TVector& rootTVector, bool bSetCacheFalse = false, double tolerance = TVector_kTol) const;
	bool		isCached() const		{return __state & kCached ? true : false;}
	bool		isSimulated() const		{return __state & kSimulated ? true : false;}
	void	setCachedFalse()	{__state &= ~kCached;}
	void	setCachedTrue()		{__state |= kCached;}
	void	setSimulatedFalse()	{__state &= ~kSimulated;}
	void	setSimulatedTrue()	{__state |= kSimulated;}
#if 1
private:
	KVector	__segmentLengthArray;
public:
	const KVector&	segmentLengthArray() const	{return __segmentLengthArray;}
	KVector&	segmentLengthArray()		{return __segmentLengthArray;}
#else
private:
	unsigned	__nSegment;
#endif
public:
	int	pushCachedData(const ShSimStrand& simulator, const kBody::TVector rootTVector);
	int	popCachedData(ShSimStrand& simulator);
};
typedef kStrandCache*		kStrandCachePtr;
typedef kPtrArray<kStrandCache>	kStrandCacheArray;
