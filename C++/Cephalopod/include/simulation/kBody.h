#pragma once
#include <simulation/kTime.h>
#if 0
#define OS	1
#endif
#if 1
	#include "../../../SBD/SBDlib/src_allheader/SuperHelixType.h"
#else
	#include <SHD/SuperHelixType.h>
#endif
#if 0
#undef LINUX
#undef MSWIN
#undef TT
#undef CONSOLE
#undef OS
#endif
#pragma warning(disable: 4100)
#if 1
#include "../../../SBD/SBDlib/src_allheader/ShSimBody.h"
#else
#include <SHD/ShSimBody.h>
#endif
#pragma warning(default: 4100)
class kBody : public ShSimBody
{
public:
	struct TVector {
		ShVector9	T0;
		ShVector6	T1;
		ShVector6	T2;
	};
public:
	virtual int	getTVector(
		kTime			kT,
		int			strandId,
		TVector&		inStart
	) const = 0;
#if 0
	virtual int	getTVector(
		double		t,
		const ShVector9&	T0_in_kObject,
		TVector&	inStart,
		double		deltaT = kTime::deltaT0
	) const = 0;
	virtual bool	getNearestSurfacePoint(
		double			t,
		const ShVector3&	in,
		ShVector3&		out
	) const = 0;
#endif
};
