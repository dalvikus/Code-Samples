#pragma once
#if 0
	kTime	kT(kTime::kTimeUnitFilm10);
	for (int i = 0; i < 101; ++i, ++kT) {
		printf("(%2d, %2d): %10.7f (%10.7f)\n", kT.nthFrame(), kT.nthStep(), kT(), kT++);
	}
#endif
class kTime {
public:
	struct kTimeUnit
	{
		unsigned	__fps;
		unsigned	__nSimulationPerFrame;
		double		__timeStep;
		kTimeUnit(unsigned fps, unsigned nSimulationPerFrame) :
			__fps(fps),
			__nSimulationPerFrame(nSimulationPerFrame),
			__timeStep(1. / fps / nSimulationPerFrame)
		{}
		static const unsigned		FPS_FILM;
		static const unsigned		FPS_TV;
		static const unsigned		NStep0;
		static const unsigned		N0_per_step;
	};
private:
	kTimeUnit	__timeUnit;
	unsigned	__nthFrame;
	unsigned	__nthStep;
	unsigned	__N_per_step;
public:
	unsigned	nthFrame() const	{return __nthFrame;}
	unsigned	nthStep() const		{return __nthStep;}
	const kTimeUnit&	timeUnit() const	{return __timeUnit;}
public:
	kTime(const kTimeUnit& timeUnit, unsigned nthFrame = 1, unsigned nthStep = 1, unsigned N_per_step = kTimeUnit::N0_per_step);
public:
	double	operator()(int incStep = 0) const;
	double	operator++(int) const;
public:
	void	operator++();
public:
	kTime	operator+(unsigned incStep) const;
	kTime&	operator+=(unsigned incStep);
public:
	static const kTimeUnit	kTimeUnitFilm10;
	static const kTimeUnit	kTimeUnitTV10;
public:
	double	deltaT() const;
	static const double	deltaT0;
};
