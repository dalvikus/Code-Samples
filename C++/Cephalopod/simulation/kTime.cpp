#include <simulation/kTime.h>
#include <stdio.h>
const unsigned		kTime::kTimeUnit::FPS_FILM		= 24;
const unsigned		kTime::kTimeUnit::FPS_TV		= 30;
const unsigned		kTime::kTimeUnit::N0_per_step		= 10;
const unsigned		kTime::kTimeUnit::NStep0		= 10;
const kTime::kTimeUnit	kTime::kTimeUnitFilm10(kTime::kTimeUnit::FPS_FILM, kTime::kTimeUnit::NStep0);
const kTime::kTimeUnit	kTime::kTimeUnitTV10(kTime::kTimeUnit::FPS_TV, kTime::kTimeUnit::NStep0);
const double		kTime::deltaT0	= 1. / kTime::kTimeUnitFilm10.__fps / kTime::kTimeUnitFilm10.__nSimulationPerFrame / kTime::kTimeUnit::N0_per_step;
kTime::kTime(
	const kTimeUnit&	timeUnit,
	unsigned	nthFrame,
	unsigned	nthStep,
	unsigned	N_per_step
) :
	__timeUnit(timeUnit),
	__nthFrame(nthFrame),
	__nthStep(nthStep),
	__N_per_step(N_per_step)
{
	if (nthStep == 0 || nthStep > __timeUnit.__nSimulationPerFrame) {
		char	buf[64];
#if defined(_MSC_VER) && (_MSC_VER >= 1400)
		(void) _snprintf_s(buf, 64, _TRUNCATE,
#else
		(void) _snprintf(buf, 64,
#endif
			"nthStep: %d: Out Of Range([1, %d])"
			,
			nthStep, __timeUnit.__nSimulationPerFrame
#if defined(_MSC_VER) && (_MSC_VER > 1400)
		);
#else
		);
#endif
		throw buf;
	}
}
double	kTime::operator()(int incStep) const
{
	return __timeUnit.__timeStep * ((__nthFrame - 1) * __timeUnit.__nSimulationPerFrame + (__nthStep - 1) + incStep);
}
double	kTime::operator++(int) const
{
	return __timeUnit.__timeStep * ((__nthFrame - 1) * __timeUnit.__nSimulationPerFrame + __nthStep);
}
void	kTime::operator++()
{
	++__nthStep;
	if (__nthStep > __timeUnit.__nSimulationPerFrame) {
		++__nthFrame;
		__nthStep = 1;
	}
}
kTime	kTime::operator+(unsigned incStep) const
{
	kTime	kT = *this;
	kT += incStep;
	return kT;
}
kTime&	kTime::operator+=(unsigned incStep)
{
	unsigned	nthStep = (__nthStep - 1) + incStep;
	__nthFrame += nthStep / __timeUnit.__nSimulationPerFrame;
	__nthStep = nthStep % __timeUnit.__nSimulationPerFrame + 1;
	return *this;
}
double	kTime::deltaT() const
{
	return 1. / __timeUnit.__fps / __timeUnit.__nSimulationPerFrame / __N_per_step;
}
