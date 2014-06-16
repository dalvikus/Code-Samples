#pragma	once
#ifndef __RAMP_H__
#define __RAMP_H__
struct INTERP {
	enum TYPE {
		Invalid = -1,
		None = 0,
		Linear,
		Smooth,
		Spline,
	};
};
struct RampTuple {
	float	Position;
	float	FloatValue;
	INTERP::TYPE	InterpType;
};
struct IndexedRampTuple : RampTuple
{
	int	index;
};
#endif
