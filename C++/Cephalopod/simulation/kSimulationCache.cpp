#include <float.h>
#define _INHERIT_kBODY
#include <iostream>
	using std::cout;
	using std::cerr;
	using std::endl;
#ifndef _INHERIT_kBODY
#define NT_PLUGIN
#endif
#include <simulation/kSimulationCache.h>
#include <assert.h>
#include <vector>
#define OS	1
#if 1
#include "../../../SBD/SBDlib/src_allheader/ShSimCollisionBody.h"
#include "../../../SBD/SBDlib/src_allheader/ShSimWind.h"
#include "../../../SBD/SBDlib/src_allheader/ShSimMinCurvatureStrand.h"
#else
	#include <SHD/ShSimCollisionBody.h>
	#include <SHD/ShSimWind.h>
	#include <SHD/ShSimMinCurvatureStrand.h>
#endif
#undef LINUX
#undef MSWIN
#undef TT
#undef CONSOLE
#undef OS
#include <maya/MPoint.h>
#include <maya/MVector.h>
void	printStrand(FILE* fp, int index, const ShVector3f* points, int n, const char* prefix)
{
#if 1
	(void) fprintf(fp, "%sStrand[%d] (float):\n", prefix ? prefix : "", index);
	for (int k = 0; k < n; ++k) {
		fprintf(fp, "\t(%f, %f, %f)\n", points[k][0], points[k][1], points[k][2]);
	}
#else
	assert(n > 5);
	(void) fprintf(fp,
		"%sStrand[%d] (float):\n\t(%f, %f, %f) (%f, %f, %f) ...\n\t(%f, %f, %f) (%f, %f, %f)\n"
		,
		prefix ? prefix : "",
		index,
		points[0][0], points[0][1], points[0][2],
		points[1][0], points[1][1], points[1][2],
		points[n - 2][0], points[n - 2][1], points[n - 2][2],
		points[n - 1][0], points[n - 1][1], points[n - 1][2]
	);
#endif
}
void	printStrand(FILE* fp, int index, const ShVector3* points, int n, const char* prefix)
{
#if 1
	(void) fprintf(fp, "%sStrand[%d] (double):\n", prefix ? prefix : "", index);
	for (int k = 0; k < n; ++k) {
		fprintf(fp, "\t(%f, %f, %f)\n", (float) points[k][0], (float) points[k][1], (float) points[k][2]);
	}
#else
	assert(n > 5);
	(void) fprintf(fp,
		"%sStrand[%d] (double):\n\t(%f, %f, %f) (%f, %f, %f) ...\n\t(%f, %f, %f) (%f, %f, %f)\n"
		,
		prefix ? prefix : "",
		index,
		(float) points[0][0], (float) points[0][1], (float) points[0][2],
		(float) points[1][0], (float) points[1][1], (float) points[1][2],
		(float) points[n - 2][0], (float) points[n - 2][1], (float) points[n - 2][2],
		(float) points[n - 1][0], (float) points[n - 1][1], (float) points[n - 1][2]
	);
#endif
}
static ShSimParameter*	__makeSimulationParameterPtr(
#ifndef _INHERIT_kBODY
	kBodyMaya*	bodyMayaPtr,
#endif
#if 1
	ShScalar inverseTimeStep,
#else
	ShScalar timeStep,
#endif
	ShScalar dissipation, numRaceType raceType)
{
	ShSimParameter*	p = new ShSimParameter();
	if (!p) {
		(void) fprintf(stderr, "Error: __makeSimulationParameterPtr: new ShSimParameter(): Failure\n");
		return 0;
	}
#if 1
	p->setInverseTimestepParameter(inverseTimeStep);
#else
	p->setTimestepParameter(timeStep);
#endif
	p->setDissipationLambdaParameter(dissipation);
#if 1
#if 0
	ShSimWind*	pWind = new ShSimWind();
	p->pWind = pWind;
	ShSimCollisionBody*	pCollBody = new ShSimCollisionBody();
	p->pCollBody = pCollBody;
#if 0
#endif
#endif
#ifndef _INHERIT_kBODY
	p->pBody = bodyMayaPtr;
#endif
#else
	ShSimWind::setSimPar(p);
	p->externalGlobalForceHandler = &ShSimWind::windForce;
#endif
	p->setRaceDependentParameter(raceType);
	return p;
}
void	kSimulationCache::setSimulationParameter(
	kSimulationCache::kSimulationParameter::kType	type,
	kSimulationCache::kSimulationParameter::kValue&	val
)
{
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
#if 0
	void	toggleIncludeStiffness();
	void	setYoungModulusEI(ShScalar inVal);
	void	toggleIncludeGravityAndInertia();
	void	setGravityParameter(ShScalar inGravity);
	void	toggleIncludeAirFriction();
	void	setAirFrictionParameter(ShScalar inAirfriction);
	void	changeIncludeDissipation(numDissipationMode type);
	void	setDissipationLambdaParameter(ShScalar inDissipation);
	void	setDissipationGammaParameter(ShScalar inDissipation);
	void	changeIncludeWind(numWindMode type);
	void	setWindRatio(ShScalar ratio);
	void	setWindDirection2D(ShScalar inAngleDegree);
	void	setWindDirection3D(ShVector3 inDirection);
	bool	includeHairBodyOptimalCollision;
	void	toggleIncludeHairBodyCollision();
	void	setHBCParameter(ShScalar *ratio, ShScalar *criticalDepth);
	void	toggleIncludeHairHairCollision();
#endif
	switch (type) {
	case kSimulationParameter::RaceType:
#ifdef _DEBUG2
(void) printf("kSimulationCache::setSimulationParameter: Race Type: |%s|\n",
	(val.raceType == RACE_ASIAN ? "RACE_ASIAN" : (val.raceType == RACE_CAUCASIAN1 ? "RACE_CAUCASIAN1" : (val.raceType == RACE_CAUCASIAN2 ? "RACE_CAUCASIAN2" : (val.raceType == RACE_AFRICAN ? "RACE_AFRICAN" : "RACE_UNKNOWN")))));
(void) fflush(stdout);
#endif
		if (val.raceType != RACE_ASIAN && val.raceType != RACE_CAUCASIAN1 && val.raceType != RACE_CAUCASIAN2 && val.raceType != RACE_AFRICAN) {
			(void) fprintf(stderr, "Error: kSimulationCache::setSimulationParameter: Unknown Race Type\n");
			(void) fflush(stderr);
			break;
		}
		if (val.raceType != __simulationParameterPtr->currentRace) {
			__simulationParameterPtr->setRaceDependentParameter(val.raceType);
			bool	setParameterDirty = true;
			__setAllFramesVoid(setParameterDirty, 0);
			for (int indexFrame = 0; indexFrame < (int) __frameCacheArray.length(); ++indexFrame) {
				kFrameCache&	frameCache = __frameCacheArray(indexFrame);
				if (!&frameCache)
					continue;
				frameCache.setYoungModulus(__simulationParameterPtr->fYoungModulus);
#if 1
#if 1
				frameCache.setAverageRadius(__simulationParameterPtr->sSection.fAverageRadius);
				frameCache.setEllipticity(__simulationParameterPtr->sSection.fEllipticity);
#else
				frameCache.setAverageRadius(__simulationParameterPtr->fAverageRadius);
				frameCache.setEllipticity(__simulationParameterPtr->fEllipticity);
#endif
				frameCache.setVolumeDensity(__simulationParameterPtr->fVolumeDensity);
				frameCache.setHelixK0STep(__simulationParameterPtr->fHelixK0STep);
				frameCache.setHelixK1Curvature(__simulationParameterPtr->fHelixK1Curvature);
#endif
			}
			for (int indexStrand = 0; indexStrand < (int) __strandArray.length(); ++indexStrand) {
				kStrand&	strand = __strandArray(indexStrand);
				ShSimStrand&	simulator = *strand.simulatorPtr;
				if (!&simulator)
					continue;
				unsigned	numSegment = simulator.pStrandData.numSegment;
				KVector	segLen(numSegment);
				for (int i = 0; i < (int) numSegment; ++i)
					segLen(i) = simulator.pSimSegmentList[i].pSegmentData.fSegmentLength;
				simulator.setStrandLengthStiffnessUsingDiffernentSegmentLength(&segLen);
				simulator.setCoordinateNaturalVelocity(NULL, NULL, NULL);
				if (simulator.getCurvatureVectorList(&strand.initialCurvatureData.vector) < 0) {
					(void) fprintf(stderr, "FATAL: kSimulationCache::setSimulationParameter: ShSimStrand::getCurvatureVectorList(): Failure\n");
					(void) fflush(stderr);
				}
				if (simulator.getCurvatureVelocityVectorList(&strand.initialCurvatureData.velocityVector) < 0) {
					(void) fprintf(stderr, "FATAL: kSimulationCache::setSimulationParameter: ShSimStrand::getCurvatureVelocityVectorList(): Failure\n");
					(void) fflush(stderr);
				}
			}
		}
		break;
	case kSimulationParameter::IncludeStiffness:
#ifdef _DEBUG2
(void) printf("kSimulationCache::setSimulationParameter: IncludeStiffness? %s\n", val.bVal ? "True" : "False");
(void) fflush(stdout);
#endif
		if (val.bVal != __simulationParameterPtr->includeStiffness) {
			__simulationParameterPtr->toggleIncludeStiffness();
		}
		break;
	case kSimulationParameter::YoungModulus:
#ifdef _DEBUG2
(void) printf("kSimulationCache::setSimulationParameter: YoungModulus = %lf\n", val.dVal);
(void) fflush(stdout);
#endif
		__simulationParameterPtr->setYoungModulusEI(val.dVal);
		for (int indexStrand = 0; indexStrand < (int) __strandArray.length(); ++indexStrand) {
			ShSimStrand*	simulatorPtr = __strandArray(indexStrand).simulatorPtr;
			if (!simulatorPtr)
				continue;
			simulatorPtr->setStiffnessMatrix();
		}
		break;
#if 1
	case kSimulationParameter::fAverageRadius:
#ifdef _DEBUG2
(void) printf("kSimulationCache::setSimulationParameter: fAverageRadius = %lf\n", val.dVal);
(void) fflush(stdout);
#endif
		__simulationParameterPtr->setREVRadius(val.dVal * 1.e-4);
		break;
	case kSimulationParameter::fEllipticity:
#ifdef _DEBUG2
(void) printf("kSimulationCache::setSimulationParameter: fEllipticity = %lf\n", val.dVal);
(void) fflush(stdout);
#endif
		__simulationParameterPtr->setREVEllipticity(val.dVal);
		break;
	case kSimulationParameter::fVolumeDensity:
#ifdef _DEBUG2
(void) printf("kSimulationCache::setSimulationParameter: fVolumeDensity = %lf\n", val.dVal);
(void) fflush(stdout);
#endif
		__simulationParameterPtr->setREVVdensity(val.dVal);
		break;
	case kSimulationParameter::fHelixK0STep:
#ifdef _DEBUG2
(void) printf("kSimulationCache::setSimulationParameter: fHelixK0STep = %lf\n", val.dVal);
(void) fflush(stdout);
#endif
		__simulationParameterPtr->setHelixK0(val.dVal);
		for (int indexStrand = 0; indexStrand < (int) __strandArray.length(); ++indexStrand) {
			ShSimStrand*	simulatorPtr = __strandArray(indexStrand).simulatorPtr;
			if (!simulatorPtr)
				continue;
			simulatorPtr->setCoordinateNaturalOnly(NULL);
		}
		break;
	case kSimulationParameter::fHelixK1Curvature:
#ifdef _DEBUG2
(void) printf("kSimulationCache::setSimulationParameter: fHelixK1Curvature = %lf\n", val.dVal);
(void) fflush(stdout);
#endif
		__simulationParameterPtr->setHelixK1(val.dVal);
		for (int indexStrand = 0; indexStrand < (int) __strandArray.length(); ++indexStrand) {
			ShSimStrand*	simulatorPtr = __strandArray(indexStrand).simulatorPtr;
			if (!simulatorPtr)
				continue;
			simulatorPtr->setCoordinateNaturalOnly(NULL);
		}
		break;
	case kSimulationParameter::inHelixK0K1:
#ifdef _DEBUG2
(void) printf("kSimulationCache::setSimulationParameter: inK0 = %lf, inK1 = %lf\n", val.v2Val[0], val.v2Val[1]);
(void) fflush(stdout);
#endif
		ShVector3	vec;
		ShSimParameter::setCurvatureFromK0K1(val.v2Val[0], val.v2Val[1], vec);
		for (int indexStrand = 0; indexStrand < (int) __strandArray.length(); ++indexStrand) {
			ShSimStrand*	simulatorPtr = __strandArray(indexStrand).simulatorPtr;
			if (!simulatorPtr)
				continue;
			simulatorPtr->setCoordinateOnly(NULL, vec);
		}
		break;
	case kSimulationParameter::fLengthFactor:
#ifdef _DEBUG2
(void) printf("kSimulationCache::setSimulationParameter: fLengthFactor = %lf\n", val.dVal);
(void) fflush(stdout);
#endif
		__simulationParameterPtr->fLengthFactor = val.dVal;
		for (int indexStrand = 0; indexStrand < (int) __strandArray.length(); ++indexStrand) {
			ShSimStrand*	simulatorPtr = __strandArray(indexStrand).simulatorPtr;
			if (!simulatorPtr)
				continue;
			simulatorPtr->dynamicStrandLengthFactorStiffness(val.dVal);
		}
		break;
#endif
	case kSimulationParameter::IncludeGravityAndInertia:
#ifdef _DEBUG2
(void) printf("kSimulationCache::setSimulationParameter: IncludeGravityAndInertia? %s\n", val.bVal ? "True" : "False");
(void) fflush(stdout);
#endif
		if (val.bVal != __simulationParameterPtr->includeGravityAndInertia) {
			__simulationParameterPtr->toggleIncludeGravityAndInertia();
		}
		break;
	case kSimulationParameter::GravityParameter:
#ifdef _DEBUG2
(void) printf("kSimulationCache::setSimulationParameter: GravityParameter = %d\n", val.iVal);
(void) fflush(stdout);
#endif
		__simulationParameterPtr->setGravityParameter(val.iVal * EARTH_GRAVITY);
		break;
	case kSimulationParameter::IncludeAirFriction:
#ifdef _DEBUG2
(void) printf("kSimulationCache::setSimulationParameter: IncludeAirFriction? %s\n", val.bVal ? "True" : "False");
(void) fflush(stdout);
#endif
		if (val.bVal != __simulationParameterPtr->includeAirFriction) {
			__simulationParameterPtr->toggleIncludeAirFriction();
		}
		break;
	case kSimulationParameter::AirFrictionParameter:
#ifdef _DEBUG2
(void) printf("kSimulationCache::setSimulationParameter: AirFrictionParameter = %d\n", val.iVal);
(void) fflush(stdout);
#endif
		__simulationParameterPtr->setAirFrictionParameter(val.iVal * 1.e-5);
		break;
	case kSimulationParameter::DissipationMode:
#ifdef _DEBUG2
(void) printf("kSimulationCache::setSimulationParameter: Dissipation Mode: |%s|\n",
	(val.dissipationMode == NODISSIPATE ? "NODISSIPATE" : (val.dissipationMode == LAMBDA ? "LAMBDA" : (val.dissipationMode == GAMMA ? "GAMMA" : "UNKNOWN"))));
(void) fflush(stdout);
#endif
		if (val.dissipationMode != NODISSIPATE && val.dissipationMode != LAMBDA && val.dissipationMode != GAMMA) {
			(void) fprintf(stderr, "Error: kSimulationCache::setSimulationParameter: Unknown Dissipation Mode\n");
			(void) fflush(stderr);
			break;
		}
		if (val.dissipationMode != __simulationParameterPtr->choiceDissipation) {
			__simulationParameterPtr->changeIncludeDissipation(val.dissipationMode);
		}
		break;
	case kSimulationParameter::DissipationLambdaParameter:
#ifdef _DEBUG2
(void) printf("kSimulationCache::setSimulationParameter: DissipationLambdaParameter = %lf\n", val.dVal);
(void) fflush(stdout);
#endif
		__simulationParameterPtr->setDissipationLambdaParameter(val.dVal);
		break;
	case kSimulationParameter::DissipationGammaParameter:
#ifdef _DEBUG2
(void) printf("kSimulationCache::setSimulationParameter: DissipationGammaParameter = %lf\n", val.dVal);
(void) fflush(stdout);
#endif
		__simulationParameterPtr->setDissipationGammaParameter(val.dVal);
		break;
	case kSimulationParameter::WindMode:
#ifdef _DEBUG2
(void) printf("kSimulationCache::setSimulationParameter: Wind Mode: |%s|\n",
#if 1
	(val.windMode == NOWIND ? "NOWIND" : (val.windMode == UNIFORMWIND ? "UNIFORM" : (val.windMode == PTNDEPENDENT ? "PTNDEPENDENT" : (val.windMode == PTNDEPENDENTVELOCITY ? "PTNDEPENDENTVELOCITY" : "UNKNOWN")))));
#else
	(val.windMode == NOWIND ? "NOWIND" : (val.windMode == UNIFORM ? "UNIFORM" : (val.windMode == PTNDEPENDENT ? "PTNDEPENDENT" : (val.windMode == PTNDEPENDENTVELOCITY ? "PTNDEPENDENTVELOCITY" : "UNKNOWN")))));
#endif
(void) fflush(stdout);
#endif
#if 1
		if (val.windMode != NOWIND && val.windMode != UNIFORMWIND && val.windMode != PTNDEPENDENT && val.windMode != PTNDEPENDENTVELOCITY) {
			(void) fprintf(stderr, "Error: kSimulationCache::setSimulationParameter: Unknown Wind Mode\n");
			(void) fflush(stderr);
			break;
		}
#else
		if (val.windMode != NOWIND && val.windMode != UNIFORM && val.windMode != PTNDEPENDENT && val.windMode != PTNDEPENDENTVELOCITY) {
			(void) fprintf(stderr, "Error: kSimulationCache::setSimulationParameter: Unknown Wind Mode\n");
			(void) fflush(stderr);
			break;
		}
#endif
		if (val.windMode != __simulationParameterPtr->choiceWind) {
			__simulationParameterPtr->changeIncludeWind(val.windMode);
		}
		break;
#if 1
	case kSimulationParameter::WindRatio:
#ifdef _DEBUG2
(void) printf("kSimulationCache::setSimulationParameter: WindRatio = %d\n", val.iVal);
(void) fflush(stdout);
#endif
		__simulationParameterPtr->setPWindRatio(val.iVal);
		break;
	case kSimulationParameter::WindDirection2D:
#ifdef _DEBUG2
(void) printf("kSimulationCache::setSimulationParameter: WindDirection2D = %d\n", val.iVal);
(void) fflush(stdout);
#endif
		__simulationParameterPtr->setPWindDirection2D(val.iVal);
		break;
	case kSimulationParameter::WindDirection3D:
#ifdef _DEBUG2
(void) printf("kSimulationCache::setSimulationParameter: WindDirection3D: (%d, %d)\n", val.i2Val[0], val.i2Val[1]);
(void) fflush(stdout);
#endif
		__simulationParameterPtr->setPWindDirection3D(val.i2Val[0], val.i2Val[1]);
		break;
#else
	case kSimulationParameter::WindRatio:
	case kSimulationParameter::WindDirection2D:
	case kSimulationParameter::WindDirection3D:
		if (__simulationParameterPtr->pWind) {
			ShSimWind*	pWind = (ShSimWind*) __simulationParameterPtr->pWind;
			switch (type) {
			case kSimulationParameter::WindRatio:
#ifdef _DEBUG2
(void) printf("kSimulationCache::setSimulationParameter: WindRatio = %lf\n", val.dVal);
(void) fflush(stdout);
#endif
				pWind->setWindRatio(val.dVal);
				break;
			case kSimulationParameter::WindDirection2D:
#ifdef _DEBUG2
(void) printf("kSimulationCache::setSimulationParameter: WindDirection2D = %lf\n", val.dVal);
(void) fflush(stdout);
#endif
				pWind->setWindDirection2D(val.dVal);
				break;
			case kSimulationParameter::WindDirection3D:
#ifdef _DEBUG2
(void) printf("kSimulationCache::setSimulationParameter: WindDirection3D: (%lf, %f, %lf)\n", val.v3Val[0], val.v3Val[1], val.v3Val[2]);
(void) fflush(stdout);
#endif
				pWind->setWindDirection3D(val.v3Val);
				break;
			default:
				break;
			}
		}
#endif
		break;
	case kSimulationParameter::IncludeHairBodyOptimalCollision:
#ifdef _DEBUG2
(void) printf("kSimulationCache::setSimulationParameter: IncludeHairBodyOptimalCollision? %s\n", val.bVal ? "True" : "False");
(void) fflush(stdout);
#endif
		__simulationParameterPtr->includeHairBodyOptimalCollision = val.bVal;
		break;
	case kSimulationParameter::IncludeHairBodyCollision:
#ifdef _DEBUG2
(void) printf("kSimulationCache::setSimulationParameter: IncludeHairBodyCollision? %s\n", val.bVal ? "True" : "False");
(void) fflush(stdout);
#endif
		if (val.bVal != __simulationParameterPtr->includeHairBodyCollision) {
			__simulationParameterPtr->toggleIncludeHairBodyCollision();
		}
		break;
	case kSimulationParameter::HBCParameter:
#if 1
#ifdef _DEBUG2
(void) printf("kSimulationCache::setSimulationParameter: HBCParameter: ratio = %lf, criticalDepth = %lf\n", val.v2Val[0], val.v2Val[1]);
(void) fflush(stdout);
#endif
		__simulationParameterPtr->setPHBCParameter(&val.v2Val[0], &val.v2Val[1]);
#else
		if (__simulationParameterPtr->pCollBody) {
#ifdef _DEBUG2
(void) printf("kSimulationCache::setSimulationParameter: HBCParameter: ratio = %lf, criticalDepth = %lf\n", val.v2Val[0], val.v2Val[1]);
(void) fflush(stdout);
#endif
			ShSimCollisionBody*	pCollBody = (ShSimCollisionBody*) __simulationParameterPtr->pCollBody;
			if (pCollBody) {
				pCollBody->setHBCParameter(&val.v2Val[0], &val.v2Val[1]);
			}
		}
#endif
		break;
	case kSimulationParameter::IncludeHairHairCollision:
#ifdef _DEBUG2
(void) printf("kSimulationCache::setSimulationParameter: IncludeHairHairCollision? %s\n", val.bVal ? "True" : "False");
(void) fflush(stdout);
#endif
		if (val.bVal != __simulationParameterPtr->includeHairHairCollision) {
			__simulationParameterPtr->toggleIncludeHairHairCollision();
		}
		break;
	case kSimulationParameter::HHCParameter:
		(void) printf("kSimulationCache::setSimulationParameter: HHCParameter: Hair-Hair Collision: AS OF 3/7/2009 (EST), NOT IMPLEMENTED YET (INCOMPLETE)\n");
		(void) fflush(stdout);
		break;
	default:
		(void) fprintf(stderr, "Error: kSimulationCache::setSimParameter: %d: Unknown type: Ignored", type);
		(void) fflush(stderr);
		break;
	}
}
kSimulationCache::kSimulationCache(
#ifndef _INHERIT_kBODY
	kBodyMaya*		bodyMayaPtr,
#endif
	kTime::kTimeUnit	timeUnit,
	unsigned		nFrame,
	bool			readOnly
) :
#ifndef _INHERIT_kBODY
	__bodyMayaPtr(bodyMayaPtr),
#endif
	__timeUnit(timeUnit),
	__nFrame(nFrame),
	__readOnly(readOnly)
#if 1
	,
	__filePtr(0)
#endif
{
	(void) fprintf(stdout, "_INHERIT_kBODY: %s\n",
#ifdef _INHERIT_kBODY
			"True"
#else
			"False"
#endif
	); (void) fflush(stdout);
	__simulationParameterPtr = __makeSimulationParameterPtr(
#ifndef _INHERIT_kBODY
		bodyMayaPtr,
#endif
#if 1
		timeUnit.__fps * timeUnit.__nSimulationPerFrame,
#else
		1. / timeUnit.__fps / timeUnit.__nSimulationPerFrame,
#endif
		DEFAULT_LAMBDADISSIPATION, RACE_ASIAN
	);
#ifdef _DEBUG
	__numDynamics = 0;
	__numKinematics = 0;
#endif
#if 0
	int	iReturnVal = __setInitialCurvatureVector();
	if (iReturnVal < 0) {
		assert(iReturnVal == 0);
	}
#endif
}
kSimulationCache::~kSimulationCache()
{
#if 1
	if (__filePtr) {
		delete __filePtr;
		__filePtr = 0;
	}
#endif
	if (__simulationParameterPtr) {
#if 1
#if 0
		ShSimWind*	pWind = (ShSimWind*) __simulationParameterPtr->pWind;
		if (pWind) {
			delete pWind;
			pWind = 0;
		}
		ShSimCollisionBody*	pCollBody = (ShSimCollisionBody*) __simulationParameterPtr->pCollBody;
		if (pCollBody) {
			delete pCollBody;
			pCollBody = 0;
		}
#endif
#endif
		delete __simulationParameterPtr;
		__simulationParameterPtr = 0;
	}
}
inline int	kSimulationCache::__find_indexStrand(int strandId)
{
	for (int i = 0; i < (int) __strandArray.length(); ++i) {
		if (strandId == __strandArray(i).strandId)
			return i;
	}
	return -1;
}
int	kSimulationCache::setStrandLength(int strandId, double len)
{
	int	indexStrand = __find_indexStrand(strandId);
	if (indexStrand < 0)
		return -1;
	ShSimStrand*	simulatorPtr = __strandArray(indexStrand).simulatorPtr;
	simulatorPtr->setStrandLengthStiffness(len);
	simulatorPtr->setCoordinateNaturalVelocity(NULL, NULL, NULL);
	for (int indexFrame = 0; indexFrame < (int) __frameCacheArray.length(); ++indexFrame) {
		kFrameCache&	frameCache = __frameCacheArray(indexFrame);
		if (!&frameCache)
			continue;
		frameCache.setCachedFalse(indexStrand);
	}
	return 0;
}
const kSimulationCache::kStrandPtr	kSimulationCache::addStrand(int strandId, unsigned nSegment, double len, KVector* segLenPtr)
{
	if (!__simulationParameterPtr) {
		(void) fprintf(stderr, "FATAL: kSimulationCache::addStrand: __simulationParameterPtr is nil\n");
		return NULL;
	}
	ShSimStrand*	simulatorPtr = new ShSimStrand(__simulationParameterPtr, nSegment);
	if (!simulatorPtr) {
		(void) fprintf(stderr, "Error: kSimulationCache::addStrand: new ShSimStrand(): Failure\n");
		return NULL;
	}
	if (segLenPtr)
		simulatorPtr->setStrandLengthStiffnessUsingDiffernentSegmentLength(segLenPtr);
	else
		simulatorPtr->setStrandLengthStiffness(len);
	simulatorPtr->setCoordinateNaturalVelocity(NULL, NULL, NULL);
#if 1
	kStrandPtr	strandPtr = new kStrand(strandId, nSegment, len);
#else
	kStrandFileCachePtr	strandPtr = new kStrandFileCache(strandId, nSegment, len);
#endif
	if (!strandPtr) {
		(void) fprintf(stderr, "Error: kSimulationCache::addStrand: new kStrandFileCache: Failure\n");
		if (simulatorPtr) {
			delete simulatorPtr;
			simulatorPtr = 0;
		}
		return NULL;
	}
	if (segLenPtr) {
		KVector&	segLen = *segLenPtr;
		for (int i = 0; i < (int) nSegment; ++i)
			strandPtr->length += segLen(i);
	} else
		strandPtr->length = len;
	if (simulatorPtr->getCurvatureVectorList(&strandPtr->initialCurvatureData.vector) < 0) {
		(void) fprintf(stderr, "Error: kSimulationCache::addStrand: ShSimStrand::getCurvatureVectorList(): Failure\n");
		if (simulatorPtr) {
			delete simulatorPtr;
			simulatorPtr = 0;
		}
		return NULL;
	}
	if (simulatorPtr->getCurvatureVelocityVectorList(&strandPtr->initialCurvatureData.velocityVector) < 0) {
		(void) fprintf(stderr, "Error: kSimulationCache::addStrand: ShSimStrand::getCurvatureVelocityVectorList(): Failure\n");
		if (simulatorPtr) {
			delete simulatorPtr;
			simulatorPtr = 0;
		}
		return NULL;
	}
	int	indexStrand = (int) __strandArray.length();
	bool	isDone = true;
	for (unsigned n = 0; n < __frameCacheArray.length(); ++n) {
#if 1
		ShSimStrand&	simulator = *simulatorPtr;
		unsigned	numSegment = simulator.pStrandData.numSegment;
		KVector	segLen(numSegment);
		for (int i = 0; i < (int) numSegment; ++i)
			segLen(i) = simulator.pSimSegmentList[i].pSegmentData.fSegmentLength;
		if (!__frameCacheArray(n).addStrandCache(indexStrand, segLen)) {
#else
		if (!__frameCacheArray(n).addStrandCache(indexStrand, nSegment)) {
#endif
			isDone = false;
			for (int i = 0; i < (int) n; ++i)
				__frameCacheArray(i).popStrandCache();
#if 1
		}
#else
		}
#endif
	}
	if (isDone) {
		strandPtr->simulatorPtr = simulatorPtr;
		__strandArray.append(strandPtr);
	} else {
		if (strandPtr) {
			delete strandPtr;
			strandPtr = 0;
		}
		if (simulatorPtr) {
			delete simulatorPtr;
			simulatorPtr = 0;
		}
	}
	return strandPtr;
}
#if 1
int	kSimulationCache::writeToFileCache(const char* fullPathname) const
{
#if 1
#if 0
	if (!pFileOutput) {
		pFileOutput = new ShSimFileMultiStrand();
	}
	if (isWrite) {
		ret = pFileOutput->initWrite(name, beginStrandIndex, beginStrandIndex+strandDoCount-1);
		if (ret < 0) {
			print_err(FNC"initWrite(%s, count %d, begin %d) fail\n", name, strandDoCount, beginStrandIndex);
			return -1;
		}
		print_log(FNC"initWrite(%s, count %d, begin %d) success\n", name, strandDoCount, beginStrandIndex);
		gOutputWrite = true;
		if (gTotalFrameFixed) {
			setFixedTotalFrame(totalShowFrame, gTotalFrameFixed);
		}
		setSimulationTime(0.0, true);
		setBeforeKinematicsForFisrtFrame();
	}
#endif
	cout << "kSimulationCache::writeToFileCache: NOT YET" << endl;
	int	iReturnVal = 0;
	ShSimFileMultiStrand*	filePtr = 0;
	ShStrandRoot*	strandRootArrayPtr = 0;
#pragma warning(disable: 4127)
do {
	int	ret;
#if 1
	filePtr = new ShSimFileMultiStrand(__simulationParameterPtr);
#else
	filePtr = new ShSimFileMultiStrand();
#endif
	if (!filePtr) {
		cerr << "kSimulationCache::writeToFileCache: new ShSimFileMultiStrand(): Failure" << endl;
		iReturnVal = -1;
		break;
	}
	int	beginStrandIndex = -1;
	int	strandDoCount = (int) __strandArray.length();
{
	bool	isIdContinuous = true;
	int	lastStrandId = -1;
	for (int indexStrand = 0; indexStrand < (int) __strandArray.length(); ++indexStrand) {
		const kStrand&	strand = __strandArray(indexStrand);
		int	strandId = strand.strandId;
		if (lastStrandId == -1) {
			beginStrandIndex = strandId;
			lastStrandId = strandId;
			break;
		}
		if (strandId != lastStrandId + 1) {
			isIdContinuous = false;
			break;
		}
	}
	if (!isIdContinuous) {
		cerr << "kSimulationCache::writeToFileCache: strand Id NOT continuous" << endl;
		iReturnVal = -1;
		break;
	}
}
	ret = filePtr->initWrite((char*) fullPathname, beginStrandIndex, beginStrandIndex + strandDoCount - 1);
	if (ret < 0) {
		cerr << "kSimulationCache::writeToFileCache: ShSimFileMultiStrand::initWrite(): Failure" << endl;
		iReturnVal = -1;
		break;
	}
	strandRootArrayPtr = new ShStrandRoot[strandDoCount];
	if (!strandRootArrayPtr) {
		cerr << "kSimulationCache::writeToFileCache: new ShStrandRoot[" << strandDoCount << "]: Failure" << endl;
		iReturnVal = -1;
		break;
	}
	for (int indexStrand = 0; indexStrand < strandDoCount; ++indexStrand) {
		const kStrand&	strand = __strandArray(indexStrand);
		strandRootArrayPtr[indexStrand].pStrand = strand.simulatorPtr;
	}
	double	outputWriteTimeInterval = 1. / __timeUnit.__fps;
#if 1
	ret = filePtr->writeMultiOutputHeader(strandRootArrayPtr, outputWriteTimeInterval);
#else
	ret = filePtr->writeMultiOutputHeader(strandRootArrayPtr, strandDoCount, beginStrandIndex, outputWriteTimeInterval);
#endif
	if (ret < 0) {
		cerr << "kSimulationCache::writeToFileCache: ShSimFileMultiStrand::writeMultiOutputHeader(): Failure" << endl;
		iReturnVal = -1;
		break;
	}
	for (int indexFrame = 0; indexFrame < (int) __frameCacheArray.length(); ++indexFrame) {
		kFrameCache&	frameCache = *__frameCacheArray[indexFrame];
		bool	isOk = true;
		for (int indexStrand = 0; indexStrand < (int) __strandArray.length(); ++indexStrand) {
			kStrandCache&	strandCache = *frameCache.strandCacheArray()[indexStrand];
			strandRootArrayPtr[indexStrand].inStart = strandCache.data().rootTVector.T0;
			const kStrand&	strand = __strandArray(indexStrand);
			ShSimStrand&	simulator = *strand.simulatorPtr;
			if (strandCache.popCachedData(simulator) < 0) {
				isOk = false;
			}
		}
		if (!isOk)
			continue;
		kTime	kT(__timeUnit, 1 + indexFrame, 1);
#if 0
		for (int indexStrand = 0; indexStrand < (int) __strandArray.length(); ++indexStrand) {
			ShSimStrand&	simulator = *strandRootArrayPtr[indexStrand].pStrand;
			unsigned	numSegment = simulator.pStrandData.numSegment;
			KVector	segLen(numSegment);
			for (int i = 0; i < (int) numSegment; ++i)
				segLen(i) = simulator.pSimSegmentList[i].pSegmentData.fSegmentLength;
			cout << "indexStrand = " << indexStrand << endl;
			segLen.print("segmentLengthArray");
		}
		ret = filePtr->writeMultiOutputStrandCurvatureWithSegmentLengthArray(1 + indexFrame, strandRootArrayPtr, strandDoCount, kT());
#else
		ret = filePtr->writeMultiOutputStrandCurvature(1 + indexFrame, strandRootArrayPtr, strandDoCount, kT());
#endif
		if (ret < 0) {
			cerr << "kSimulationCache::writeToFileCache: ShSimFileMultiStrand::writeMultiOutputStrandCurvature() for indexFrame = " << indexFrame << ": Failure" << endl;
		}
	}
	iReturnVal = 0;
} while (0);
#pragma warning(default: 4127)
	if (strandRootArrayPtr) {
		for (int indexStrand = 0; indexStrand < (int) __strandArray.length(); ++indexStrand) {
			strandRootArrayPtr[indexStrand].pStrand = 0;
		}
		delete[] strandRootArrayPtr;
		strandRootArrayPtr = 0;
	}
	if (filePtr) {
		delete filePtr;
		filePtr = 0;
	}
	if (iReturnVal < 0)
		(void) fflush(stderr);
	cout << "kSimulationCache::writeToFileCache: NOT YET" << endl;
	cout << "iReturnVal = " << iReturnVal << endl;
	return iReturnVal;
#else
	ShSimFileStrand*	filePtr = new ShSimFileStrand();
	if (!filePtr) {
		(void) fprintf(stderr, "Error: kSimulationCache::writeToFileCache: new ShSimFileStrand(): Failure\n");
		return -1;
	}
	for (int indexStrand = 0; indexStrand < (int) __strandArray.length(); ++indexStrand) {
		const kStrand&	strand = __strandArray(indexStrand);
		int	strandId = strand.strandId;
		char	pathname[MAX_PATH];
		(void) _snprintf_s(pathname, MAX_PATH, _TRUNCATE, "%s.%07u", fullPathname ? fullPathname : "C:/nil", strandId);
		if (filePtr->open(pathname, true) < 0) {
			(void) fprintf(stderr, "Error: Cannot open |%s| for |Write|\n", pathname);
			continue;
		}
#if 1
		const ShSimStrand&	simulator = *strand.simulatorPtr;
		const unsigned&		numSegment = simulator.pStrandData.numSegment;
		KVector	segLen(numSegment);
		for (int i = 0; i < (int) numSegment; ++i)
			segLen(i) = simulator.pSimSegmentList[i].pSegmentData.fSegmentLength;
		ShScalar	timeStep = 1. / __timeUnit.__fps / __timeUnit.__nSimulationPerFrame;
		if (filePtr->writeOutputHeader(strand.strandId, timeStep, (uint16) strand.numSegment, &segLen) < 0) {
			(void) fprintf(stderr, "Error: kSimulationCache::writeToFileCache: ShSimFileStrand::writeOutputHeader(strandId = %d): Failure\n", strand.strandId);
			(void) filePtr->close();
			continue;
		}
#else
		if (filePtr->writeOutputHeader(strand.strandId, strand.length, (uint16) strand.numSegment) < 0) {
			(void) fprintf(stderr, "Error: kSimulationCache::writeToFileCache: ShSimFileStrand::writeOutputHeader(strandId = %d): Failure\n", strand.strandId);
			(void) filePtr->close();
			continue;
		}
#endif
		for (int indexFrame = 0; indexFrame < (int) __frameCacheArray.length(); ++indexFrame) {
			const kStrandCache::CacheData&	data = __frameCacheArray(indexFrame).strandCacheArray()(indexStrand).data();
			if (filePtr->writeOutputStrandCurvature((uint32) indexFrame, (ShVector9*) &data.rootTVector.T0, (KVector*) &data.curvatureData.vector) < 0) {
				(void) fprintf(stderr,
					"Error: kSimulationCache::writeToFileCache: "
					"ShSimFileStrand::writeOutputStrandCurvature(indexFrame = %d) at indexStrand = %d (strandId = %d): Failure\n"
					,
					indexFrame, indexStrand, strandId
				);
				break;
			}
		}
		(void) filePtr->close();
	}
	if (filePtr) {
		delete filePtr;
		filePtr = 0;
	}
	return 0;
#endif
}
int	kSimulationCache::setStrandFromFileCache(const char* fullPathname, std::vector<double>* strandLengthArrayPtr)
{
#if 1
#if 0
int	ShSimulation::openOutputFileMultipleStrand(char *name, bool isWrite);
	if (!pFileOutput) {
		pFileOutput = new ShSimFileMultiStrand();
	}
{
	ret = pFileOutput->initRead(name);
	if (ret < 0) {
		print_err(FNC"initRead() fail (%s)\n", name);
		return -1;
	}
	print_log(FNC"initRead() success\n");
	strcpy(nameCache, pFileOutput->nameWithBeginEndIndex);
	strandCount = pFileOutput->countSRArray;
	beginStrandIndex = pFileOutput->beginIndexSRArray;
	print_log(FNC"strandCount %d, beginStrandIndex %d\n", strandCount, beginStrandIndex);
	setStrandDoCount(strandCount);
	setFixedTotalFrame(pFileOutput->outputTotalFrame, true);
	setSimulationTime(0.0);
	ShScalar timeStep;
	ShStrandRoot *srArray;
	srArray = pFileOutput->readMultiOutputHeader(&strandDoCount, &beginStrandIndex, &timeStep);
	if (!srArray) {
		print_err(FNC"readMultiOutputHeader() fail\n");
		return -1;
	}
	print_log(FNC"strandDoCount %d, beginStrandIndex %d\n", strandDoCount, beginStrandIndex);
	outputWriteTimeInterval = timeStep;
	print_log(FNC"writeTimestep %f\n", outputWriteTimeInterval);
	ShSimStrandInitData initData = {0};
	for (unsigned int i=0; i<strandDoCount; i++) {
		initData.segmentCount = srArray[i].segmentNumber;
		initData.segmentLengthArray = srArray[i].segmentLengthArray;
		if (!initData.segmentLengthArray) {
			print_err(FNC"[%d] segmentLengthArray == NULL", i);
		}
#if 0
		else {
			print_log(FNC"[%d] %f (segmentLengthArray^2)\n", i, initData.segmentLengthArray->len2());
		}
#endif
#if 0
		initData.inCoordinate = NULL;
		initData.inCoordinateNatural = NULL;
		initData.inVelocity = NULL;
#endif
		ret = newStrandByInitDataSingleStrand(&srArray[i], &initData);
		if (ret < 0) {
			print_err(FNC"newStrandByInitDataSingleStrand[%d] fail\n", i);
			return -1;
		}
#ifdef	SHOW_EACH_STRAND_INFO
		print_log(FNC"after newStrandByInitDataSingleStrand[%d]\n", i);
#endif
	}
	pRootStrandArray = srArray;
}
#endif
	if (__filePtr) {
		delete __filePtr;
		__filePtr = 0;
	}
	int	iReturnVal = 0;
#pragma warning(disable: 4127)
do {
#if 1
	__filePtr = new ShSimFileMultiStrand(__simulationParameterPtr);
#else
	__filePtr = new ShSimFileMultiStrand();
#endif
	if (!__filePtr) {
		cerr << "kSimulationCache::setStrandFromFileCache: new ShSimFileMultiStrand(): Failure" << endl;
		iReturnVal = -1;
		break;
	}
	int	ret;
	ret = __filePtr->initRead((char*) fullPathname);
	if (ret < 0) {
		cerr << "kSimulationCache::setStrandFromFileCache: ShSimFileMultiStrand::initRead(): Failure" << endl;
		iReturnVal = -1;
		break;
	}
	cout << "|" << __filePtr->namePre << "|" << endl;
	cout << "|" << __filePtr->nameFolder << "|" << endl;
	cout << "|" << __filePtr->nameWithBeginEndIndex << "|" << endl;
	int	strandCount = __filePtr->countSRArray;
	int	beginStrandIndex = __filePtr->beginIndexSRArray;
	cout << "strandCount = " << strandCount << endl;
	cout << "beginStrandIndex = " << beginStrandIndex << endl;
	ShScalar	timeStep;
	ShStrandRoot*	strandRootArrayPtr = __filePtr->readMultiOutputHeader(NULL, NULL, &timeStep);
	if (!strandRootArrayPtr) {
		cerr << "kSimulationCache::setStrandFromFileCache: ShSimFileMultiStrand::readMultiOutputHeader(): Failure" << endl;
		iReturnVal = -1;
		break;
	}
	cout << "timeStep = " << timeStep << endl;
	for (int indexCount = 0; indexCount < strandCount; ++indexCount) {
		int	strandId = beginStrandIndex + indexCount;
		unsigned	nSegment = strandRootArrayPtr[indexCount].segmentNumber;
		KVector&	segLen = *strandRootArrayPtr[indexCount].segmentLengthArray;
		kStrandPtr	strandPtr = addStrand(strandId, nSegment, 0, &segLen);
		if (!strandPtr) {
			(void) fprintf(stderr,
				"Error: kSimulationCache::setStrandFromFileCache: strandId = %d: |%s|: "
				"kSimulationCache::addStrand(): Failure: Ignored\n"
				,
				strandId, fullPathname
			);
			(void) fflush(stderr);
			continue;
		}
		ShScalar	len = 0;
		for (int i = 0; i < (int) nSegment; ++i)
			len += segLen(i);
		strandPtr->length = len;
	}
	iReturnVal = 0;
} while (0);
#pragma warning(default: 4127)
	if (strandLengthArrayPtr) {
#if 10
		strandLengthArrayPtr->resize(__strandArray.length());
#endif
		for (unsigned n = 0; n < __strandArray.length(); ++n) {
			const kStrand&	strand = __strandArray(n);
			if (strand.strandId >= (int) strandLengthArrayPtr->size())
				return -1;
			strandLengthArrayPtr->at(strand.strandId) = strand.length;
		}
	}
	return iReturnVal;
#else
	if (!fullPathname) {
		(void) fprintf(stderr, "Error: kSimulationCache::setStrandFromFileCache(): fullPathname is nil");
		return -1;
	}
	for (unsigned n = 0; n < strandIdArray.size(); ++n) {
		int	strandId = strandIdArray.at(n);
		ShSimFileStrand*	filePtr = new ShSimFileStrand();
		if (!filePtr) {
			(void) fprintf(stderr,
				"Error: kSimulationCache::setStrandFromFileCache: strandId = %d: "
				"new ShSimFileStrand(): Failure: Ignored\n"
				,
				strandId
			);
			continue;
		}
		char	pathname[MAX_PATH];
		(void) _snprintf_s(pathname, MAX_PATH, _TRUNCATE, "%s.%07u", fullPathname ? fullPathname : "C:/nil", strandId);
		if (filePtr->open(pathname, false) < 0) {
			(void) fprintf(stderr,
				"Error: kSimulationCache::setStrandFromFileCache: strandId = %d: "
				"Cannot open |%s| for |Read|: Ignored\n"
				,
				strandId, pathname
			);
			delete filePtr;
			filePtr = 0;
			continue;
		}
#if 1
		uint32		index;
		ShScalar	timeStep;
		int	nSegment = filePtr->readOutputHeader0(&index, &timeStep);
		if (nSegment < 0) {
			(void) fprintf(stderr,
				"Error: kSimulationCache::setStrandFromFileCache: strandId = %d: "
				"ShSimFileStrand::readOutputHeader0(|%s|): Failure: Ignored\n"
				,
				strandId, pathname
			);
			(void) filePtr->close();
			delete filePtr;
			filePtr = 0;
			continue;
		}
		KVector	segLen(nSegment);
		ShScalar	len = 0;
		if (filePtr->readOutputHeader1(&segLen) < 0) {
			(void) fprintf(stderr,
				"Error: kSimulationCache::setStrandFromFileCache: strandId = %d: "
				"ShSimFileStrand::readOutputHeader1(|%s|): Failure: Ignored\n"
				,
				strandId, pathname
			);
			(void) filePtr->close();
			delete filePtr;
			filePtr = 0;
			continue;
		}
		for (int i = 0; i < (int) nSegment; ++i)
			len += segLen(i);
		kStrandFileCachePtr	strandPtr = (kStrandFileCachePtr) addStrand(strandId, nSegment, 0, &segLen);
		if (!strandPtr) {
			(void) fprintf(stderr,
				"Error: kSimulationCache::setStrandFromFileCache: strandId = %d: |%s|: "
				"kSimulationCache::addStrand(): Failure: Ignored\n"
				,
				strandId, pathname
			);
			(void) filePtr->close();
			delete filePtr;
			filePtr = 0;
			continue;
		}
		strandPtr->length = len;
#else
		uint32		index;
		ShScalar	len;
		int	nSegment = filePtr->readOutputHeader(&index, &len);
		if (nSegment < 0 || (int) index != strandId) {
			if (nSegment < 0) {
				(void) fprintf(stderr,
					"Error: kSimulationCache::setStrandFromFileCache: strandId = %d: "
					"ShSimFileStrand::readOutputHeader(|%s|): Failure: Ignored\n"
					,
					strandId, pathname
				);
			} else {
				(void) fprintf(stderr,
					"Error: kSimulationCache::setStrandFromFileCache: strandId = %d: "
					"ShSimFileStrand::readOutputHeader(|%s|): index(%d) != strandId: Ignored\n"
					,
					strandId, pathname, index
				);
			}
			(void) filePtr->close();
			delete filePtr;
			filePtr = 0;
			continue;
		}
		kStrandFileCachePtr	strandPtr = (kStrandFileCachePtr) addStrand(strandId, nSegment, len);
		if (!strandPtr) {
			(void) fprintf(stderr,
				"Error: kSimulationCache::setStrandFromFileCache: strandId = %d: |%s|: "
				"kSimulationCache::addStrand(): Failure: Ignored\n"
				,
				strandId, pathname
			);
			(void) filePtr->close();
			delete filePtr;
			filePtr = 0;
			continue;
		}
#endif
		strandPtr->filePtr = filePtr;
	}
	if (strandLengthArrayPtr) {
		for (unsigned n = 0; n < __strandArray.length(); ++n) {
			const kStrand&	strand = __strandArray(n);
			if (strand.strandId >= (int) strandLengthArrayPtr->size())
				return -1;
			strandLengthArrayPtr->at(strand.strandId) = strand.length;
		}
	}
	return 0;
#endif
}
#endif
#if 1
#if 0
int	kSimulationCache::__setInitialCurvatureVector()
{
	for (int indexStrand = 0; indexStrand < (int) __strandArray.length(); ++indexStrand) {
		kStrandCache::CurvatureDataPtr	p = new kStrandCache::CurvatureData(fSegmentUniformCount);
		ShSimStrand&	simulator = *pStrandRootList[strandId].pStrand;
		int	nSegment = simulator.getCurvatureVectorList(&p->vector);
		assert(nSegment == (int) fSegmentUniformCount);
		if (nSegment < 0) {
			delete p;
			p = NULL;
			return -1;
		}
		nSegment = simulator.getCurvatureVelocityVectorList(&p->velocityVector);
		assert(nSegment == (int) fSegmentUniformCount);
		if (nSegment < 0) {
			delete p;
			p = NULL;
			return -1;
		}
		__initialCurvatureDataPtrArray.append(p);
		if (setInitialCurvatureVector(indexStrand, *p) < 0) {
			kStrandCache::CurvatureDataPtr&	q = __initialCurvatureDataPtrArray[indexStrand];
			delete q;
			q = NULL;
			return -1;
		}
	}
	return 0;
}
#endif
int	kSimulationCache::setInitialCurvatureVector(int indexStrand, const kStrandCache::CurvatureData& curvatureData)
{
	if (indexStrand < 0 || indexStrand >= (int) __strandArray.length()) {
		cerr << "FATAL: kSimulationCache::setInitialCurvatureVector(int, kSimulationCache::CurvatureData): indexStrand = " << indexStrand << ": OUT OF RANGE (__strandArray.length = " << __strandArray.length() << ")" << endl;
		return -1;
	}
#if 1
	kStrand&	strand = __strandArray(indexStrand);
	strand.initialCurvatureData.vector.setVector((KVector*) &curvatureData.vector);
	strand.initialCurvatureData.velocityVector.setVector((KVector*) &curvatureData.velocityVector);
#else
	kStrandCache::CurvatureDataPtr	p = __initialCurvatureDataPtrArray[indexStrand];
	p->vector.setVector((KVector*) &curvatureData.vector);
	p->velocityVector.setVector((KVector*) &curvatureData.velocityVector);
#endif
	return 0;
}
int	kSimulationCache::setInitialCurvatureVector(int indexStrand, unsigned inCountSample, const ShVector3* inSamples)
{
	if (indexStrand < 0 || indexStrand >= (int) __strandArray.length()) {
		cerr << "FATAL: kSimulationCache::setInitialCurvatureVector(int, unsigned, const ShVector3*): indexStrand = " << indexStrand << ": OUT OF RANGE (__strandArray.length = " << __strandArray.length() << ")" << endl;
		return -1;
	}
	const kStrand&	strand = __strandArray(indexStrand);
	ShSimStrand&		simulator = *strand.simulatorPtr;
#ifdef _DEBUG2
{
(void) printf("========================================\n");
(void) printf("-------- before fitting --------\n");
	kStrandCache::CurvatureData	curvatureData(strand.numSegment);
	int	nSegment;
	nSegment = simulator.getCurvatureVectorList(&curvatureData.vector);
	assert(nSegment == (int) strand.numSegment);
	if (nSegment < 0) {
		return -1;
	}
	curvatureData.vector.print("vector0");
	nSegment = simulator.getCurvatureVelocityVectorList(&curvatureData.velocityVector);
	assert(nSegment == (int) strand.numSegment);
	if (nSegment < 0) {
		return -1;
	}
	curvatureData.velocityVector.print("velocity0");
}
#endif
	kTime	kT(__timeUnit, 1, 1);
	kBody::TVector	inStart;
#ifdef _INHERIT_kBODY
	int	iReturnVal = getTVector(
		kT,
		strand.strandId,
		inStart
	);
#else
	int	iReturnVal = __bodyMayaPtr->getTVector(
		kT,
		strand.strandId,
		inStart
	);
#endif
#ifdef _DEBUG2
	cout << "indexStrand = " << indexStrand << ", strandId = " << strand.strandId << endl;
	cout << "T0.position: [" << inStart.T0.position[0] << ", " << inStart.T0.position[1] << ", " << inStart.T0.position[2] << "]" << endl;
	cout << "T0.normal: [" << inStart.T0.normal[0] << ", " << inStart.T0.normal[1] << ", " << inStart.T0.normal[2] << "]" << endl;
	cout << "T0.tangent: [" << inStart.T0.tangent[0] << ", " << inStart.T0.tangent[1] << ", " << inStart.T0.tangent[2] << "]" << endl;
#endif
	if (iReturnVal < 0) {
		assert(iReturnVal == 0);
		return -1;
	}
	simulator.pStrandData.vStart = inStart.T0;
	iReturnVal = ShSimGetMinCurvatureStrand(&simulator, inCountSample, (ShVector3*) inSamples, true);
	if (iReturnVal < 0) {
		assert(iReturnVal == 0);
		return -1;
	}
	kStrandCache::CurvatureData	curvatureData(strand.numSegment);
	int	nSegment;
	nSegment = simulator.getCurvatureVectorList(&curvatureData.vector);
	assert(nSegment == (int) strand.numSegment);
	if (nSegment < 0) {
		return -1;
	}
#ifdef _DEBUG2
(void) printf("-------- after fitting --------\n");
	curvatureData.vector.print("vector0'");
#endif
	nSegment = simulator.getCurvatureVelocityVectorList(&curvatureData.velocityVector);
	assert(nSegment == (int) strand.numSegment);
	if (nSegment < 0) {
		return -1;
	}
#ifdef _DEBUG2
	curvatureData.velocityVector.print("velocity0'");
(void) printf("========================================\n");
#endif
	setInitialCurvatureVector(indexStrand, curvatureData);
	return 0;
}
#endif
void	kSimulationCache::__setAllFramesVoid(bool setParameterDirty, int indexFrameStart, int indexStrand)
{
	for (int indexFrame = indexFrameStart; indexFrame < (int) __nFrame; ++indexFrame) {
		if (indexFrame >= (int) __frameCacheArray.length()) {
			continue;
		}
		__frameCacheArray[indexFrame]->setCachedFalse(indexStrand);
		if (setParameterDirty)
			__frameCacheArray[indexFrame]->setParameterDirtyTrue();
	}
}
int	kSimulationCache::__isStrandCacheDirty(const kFrameCache& frameCache, const kStrandCache& strandCache) const
{
#ifndef _INHERIT_kBODY
	if (!__bodyMayaPtr)
		return -1;
#endif
#if 0
	if (indexFrame < 0 || indexFrame >= (int) __frameCacheArray.length())
		return false;
	const kFrameCache&	frameCache = __frameCacheArray(indexFrame);
#endif
	int	indexFrame = frameCache.indexFrame();
#if 0
	const kStrandCacheArray&	strandCacheArray = frameCache.strandCacheArray();
	if (indexStrand < 0 || indexStrand >= (int) strandCacheArray.length())
		return false;
	const kStrandCache&	strandCache = strandCacheArray(indexStrand);
#endif
	int	indexStrand = strandCache.indexStrand();
	assert(indexStrand >= 0 && indexStrand < (int) __strandArray.length());
#ifdef _DEBUG
	int	strandId = __strandArray(indexStrand).strandId;
#endif
	if (!strandCache.isSimulated()) {
#ifdef _DEBUG
		(void) printf("indexFrame = %d, indexStrand = %d (strandId = %d): isSimulated? False\n", indexFrame, indexStrand, strandId);
#endif
		return 0;
	}
#ifdef _DEBUG
(void) printf("indexFrame = %d, indexStrand = %d (strandId = %d): isSimulated? True\n", indexFrame, indexStrand, strandId);
#endif
	if (!strandCache.isCached()) {
#ifdef _DEBUG
		(void) printf("indexFrame = %d, indexStrand = %d (strandId = %d): isCached? False\n", indexFrame, indexStrand, strandId);
#endif
		return 1;
	}
#ifdef _DEBUG
(void) printf("indexFrame = %d, indexStrand = %d (strandId = %d): isCached? True\n", indexFrame, indexStrand, strandId);
#endif
	kTime	kT(__timeUnit, 1 + indexFrame, 1);
	kBody::TVector	inStart;
#ifdef _INHERIT_kBODY
	int	iReturnVal = getTVector(
		kT + __timeUnit.__nSimulationPerFrame,
		__strandArray(indexStrand).strandId,
		inStart
	);
#else
	int	iReturnVal = __bodyMayaPtr->getTVector(
		kT + __timeUnit.__nSimulationPerFrame,
		__strandArray(indexStrand).strandId,
		inStart
	);
#endif
	if (iReturnVal < 0) {
		(void) fprintf(stderr,
			"Error: kSimulationCache::__isStrandCacheDirty: getTVector(indexFrame = %d, indexStrand = %d (strandId = %d)): Failure\n"
			,
			indexFrame, indexStrand, __strandArray(indexStrand).strandId
		);
		return -1;
	}
	if (!strandCache.isRootTVectorChanged(inStart, true)) {
#ifdef _DEBUG
		(void) printf("indexFrame = %d, indexStrand = %d (strandId = %d): isRootTVectorChanged? False: Skipped\n", indexFrame, indexStrand, strandId);
#endif
		return 0;
	}
#ifdef _DEBUG
	(void) printf("indexFrame = %d, indexStrand = %d (strandId = %d): isRootTVectorChanged? ", indexFrame, indexStrand, strandId);
	(void) printf("True: Re-simulate: __setAllFramesVoid(false, %d, %d) Forced\n", indexFrame, indexStrand);
#endif
	return 1;
}
int	kSimulationCache::__setResult(
	unsigned	nthFrame,
	bool		bForce,
	int		numSamplePoints
)
{
	if (__readOnly) {
		(void) fprintf(stderr, "FATAL: kSimulationCache::__setResult: ReadOnly\n");
		return -1;
	}
	if (!__simulationParameterPtr) {
		(void) fprintf(stderr, "FATAL: kSimulationCache::__setResult: __simulationParameterPtr is nil\n");
		return -1;
	}
	assert(nthFrame > 0 && nthFrame <= __nFrame);
	if (nthFrame <= 0 || nthFrame > __nFrame)
		return -1;
	std::vector<int>	lastDoneFrameArray(__strandArray.length(), -1);
#ifdef _DEBUG
	unsigned	nSim = 0;
#endif
	unsigned	nSimulationPerFrame = __timeUnit.__nSimulationPerFrame;
	for (int indexFrame = 0; indexFrame < (int) nthFrame; ++indexFrame) {
		if (indexFrame >= (int) __frameCacheArray.length()) {
			kFrameCachePtr	frameCachePtr = new kFrameCache(indexFrame);
			if (!frameCachePtr) {
				(void) fprintf(stderr, "Error: kSimulationCache::__setResult: new kFrameCache(): Failure\n");
				return -1;
			}
			for (int indexStrand = 0; indexStrand < (int) __strandArray.length(); ++indexStrand) {
#if 1
				const kStrand&	strand = __strandArray(indexStrand);
				ShSimStrand&	simulator = *strand.simulatorPtr;
				unsigned	numSegment = simulator.pStrandData.numSegment;
				KVector	segLen(numSegment);
				for (int i = 0; i < (int) numSegment; ++i)
					segLen(i) = simulator.pSimSegmentList[i].pSegmentData.fSegmentLength;
				if (!frameCachePtr->addStrandCache(indexStrand, segLen)) {
#else
				if (!frameCachePtr->addStrandCache(indexStrand, __strandArray(indexStrand).numSegment)) {
#endif
					(void) fprintf(stderr, "Error: kSimulationCache::__setResult: addStrandCache(indexStrand = %d (strandId = %d)): Failure\n", indexStrand, __strandArray(indexStrand).strandId);
					frameCachePtr->clear();
					delete frameCachePtr;
					frameCachePtr = 0;
					break;
#if 1
				}
#else
				}
#endif
			}
			if (!frameCachePtr)
				return -1;
			frameCachePtr->setParameter(*__simulationParameterPtr);
			__frameCacheArray.append(frameCachePtr);
		}
		kFrameCache&	frameCache = *__frameCacheArray[indexFrame];
		if (indexFrame + 1 == (int) nthFrame) {
			if (bForce) {
#ifdef _DEBUG
				(void) printf("indexFrame = %d: Force Flag Set: __setAllFramesVoid(true, %d) Forced\n", indexFrame, indexFrame);
#endif
				bool	setParameterDirty = true;
				__setAllFramesVoid(setParameterDirty, indexFrame);
			} else {
				bool	isCached = frameCache.isCached();
#ifdef _DEBUG
				(void) printf("indexFrame = %d: isCached? %s\n", indexFrame, isCached ? "True" : "False");
#endif
				if (isCached) {
					bool	reset = true;
					bool	isParameterChanged = frameCache.isParameterChanged(*__simulationParameterPtr, reset);
					if (isParameterChanged) {
#ifdef _DEBUG
						(void) printf("indexFrame = %d: isParameterChanged? True: __setAllFramesVoid(true, %d) Forced\n", indexFrame, indexFrame);
#endif
						bool	setParameterDirty = true;
						__setAllFramesVoid(setParameterDirty, indexFrame);
					} else {
#ifdef _DEBUG
						(void) printf("indexFrame = %d: isParameterChanged? False: Nothing for kFrameCache\n", indexFrame);
#endif
					}
				} else {
				}
			}
		} else {
			if (frameCache.isParameterDirty()) {
#ifdef _DEBUG
				(void) printf("indexFrame = %d: isParameterDirty? True: set simulation parameter dirty\n", indexFrame);
#endif
				frameCache.setParameter(*__simulationParameterPtr);
			}
		}
		for (int indexStrand = 0; indexStrand < (int) __strandArray.length(); ++indexStrand) {
			const kStrand&	strand = __strandArray(indexStrand);
			kStrandCache&	strandCache = *frameCache.strandCacheArray()[indexStrand];
			int	isDirty = __isStrandCacheDirty(frameCache, strandCache);
			if (isDirty < 0)
				return -1;
			if (!isDirty)
				continue;
			ShSimStrand&		simulator = *strand.simulatorPtr;
			if (indexFrame != lastDoneFrameArray[indexStrand] + 1) {
				kFrameCache&	lastFrameCache = *__frameCacheArray[indexFrame - 1];
				kStrandCache&	lastStrandCache = *lastFrameCache.strandCacheArray()[indexStrand];
				if (lastStrandCache.popCachedData(simulator) < 0) {
					return -1;
				}
#ifdef _DEBUG
				(void) printf("indexFrame = %d, indexStrand = %d (strandId = %d): Simulation: ", indexFrame, indexStrand, __strandArray(indexStrand).strandId);
				(void) printf("Load last frame(Frame # = %d) data\n", indexFrame);
#endif
			} else {
#ifdef _DEBUG
				(void) printf("indexFrame = %d, indexStrand = %d (strandId = %d): Simulation: ", indexFrame, indexStrand, __strandArray(indexStrand).strandId);
				(void) printf("Continuing %s\n", indexFrame ? "from last frame" : "Start");
#endif
				if (indexFrame == 0) {
					kStrandCache::CurvatureData&	non_const_curvatureData = *((kStrandCache::CurvatureData*) &strand.initialCurvatureData);
#ifdef _DEBUG2
(void) printf("-------- initial condition --------\n");
					non_const_curvatureData.vector.print("vector0");
					non_const_curvatureData.velocityVector.print("velocityVector0");
(void) printf("-----------------------------------\n");
#endif
#if 1
					simulator.setCoordinateOnly(&non_const_curvatureData.vector, NULL);
					simulator.setVelocityOnly(NULL);
#else
					simulator.setCoordinateVelocity(&non_const_curvatureData.vector, &non_const_curvatureData.velocityVector);
#endif
				}
			}
			kTime	kT(__timeUnit, 1 + indexFrame, 1);
			for (int iStep = 0; iStep < (int) nSimulationPerFrame; ++iStep, ++kT) {
#ifdef _INHERIT_kBODY
				(void) setCurrentTime(kT());
#else
				if (__bodyMayaPtr)
					(void) __bodyMayaPtr->setCurrentTime(kT());
#endif
#ifdef _DEBUG_
if (indexFrame == 0 && iStep == 0) {
	KVector	curvatureVector(3 * strand.numSegment);
	KVector	curvatureVelocityVector(3 * strand.numSegment);
	(void) simulator.getCurvatureVectorList(&curvatureVector);
	(void) simulator.getCurvatureVelocityVectorList(&curvatureVelocityVector);
	curvatureVector.print("vector0");
	curvatureVelocityVector.print("velocity0");
}
#endif
				simulator.updateCurvaturesAtSegments();
				kBody::TVector	inStart;
#ifdef _INHERIT_kBODY
				int	iReturnVal = getTVector(
					kT,
					strand.strandId,
					inStart
				);
#else
				int	iReturnVal = __bodyMayaPtr->getTVector(
					kT,
					strand.strandId,
					inStart
				);
#endif
				if (iReturnVal < 0) {
					assert(iReturnVal == 0);
					return -1;
				}
#if 1
#if 0
#endif
#if 1
				simulator.updateVextexPTNBVectorsAtSegments(true, &inStart.T0);
#else
				simulator.updateVextexPTNBVectorsAtSegments(&inStart.T0);
#endif
				if (__simulationParameterPtr->includeHairBodyOptimalCollision) {
					ShSimGetMinCurvatureStrandToAvoidBody(&simulator, (ShSimBody*) __simulationParameterPtr->pBody);
				}
#if 0
#ifdef	INCLUDE_HAIR_BODY_OPTIMAL_COLLISION_RESPONSE
		if (pShPars->includeHairBodyOptimalCollision) {
			ShSimGetMinCurvatureStrandToAvoidBody(pStrandRootList[i].pStrand, pBodyMotion);
		}
#endif
		if (bodyMove) {
			pBodyMotion->getT1Vector6(&pStrandRootList[i].inStartT1);
			pBodyMotion->getT2Vector6(&pStrandRootList[i].inStartT2);
		} else {
			Sh3VectorSetToZero(pStrandRootList[i].inStartT1.position);
			Sh3VectorSetToZero(pStrandRootList[i].inStartT1.tangent);
			Sh3VectorSetToZero(pStrandRootList[i].inStartT2.position);
			Sh3VectorSetToZero(pStrandRootList[i].inStartT2.tangent);
		}
#endif
				ShVector6	v6Zero = {{0., 0., 0.}, {0., 0., 0.}};
#if 1
				simulator.updateVextexT1T2VectorsAtSegments(
					true,
					indexFrame == 0 && iStep == 0 ? &v6Zero : &inStart.T1,
					indexFrame == 0 && iStep == 0 ? &v6Zero : &inStart.T2,
					true
				);
#else
				simulator.updateVextexT1T2VectorsAtSegments(
					indexFrame == 0 && iStep == 0 ? &v6Zero : &inStart.T1,
					indexFrame == 0 && iStep == 0 ? &v6Zero : &inStart.T2
				);
#endif
#else
				ShVector6	v6Zero = {{0., 0., 0.}, {0., 0., 0.}};
				simulator.updateVextexVectorsAtSegments(
					&inStart.T0,
					indexFrame == 0 && iStep == 0 ? &v6Zero : &inStart.T1,
					indexFrame == 0 && iStep == 0 ? &v6Zero : &inStart.T2
				);
#endif
				if (__dynamics)
					(void) simulator.solveEquation();
#ifdef _DEBUG
				++__numDynamics;
				++nSim;
#endif
if (numSamplePoints > 1) {
				simulator.updateCurvaturesAtSegments();
#ifdef _INHERIT_kBODY
				int	iReturnVal = getTVector(
					kT + 1,
					strand.strandId,
					inStart
				);
#else
				int	iReturnVal = __bodyMayaPtr->getTVector(
					kT + 1,
					strand.strandId,
					inStart
				);
#endif
				if (iReturnVal < 0) {
					assert(iReturnVal == 0);
					return -1;
				}
#if 1
#if 1
				simulator.updateVextexPTNBVectorsAtSegments(true, &inStart.T0);
#else
				simulator.updateVextexPTNBVectorsAtSegments(&inStart.T0);
#endif
#else
				simulator.updateVextexVectorsAtSegments(&inStart.T0, &inStart.T1, &inStart.T2);
#endif
				ShVector3	samplePoints[100];
				simulator.getSamplePositions(numSamplePoints, samplePoints);
				(void) fprintf(stdout,
					"_DEBUG: (nthFrame = %d, nthStep = %d, TIME = %f)\n"
					,
					1 + indexFrame, 1 + iStep, kT(1)
				);
				printStrand(stdout, indexStrand, samplePoints, numSamplePoints, "_DEBUG: ");
}
			}
			kBody::TVector	inStart;
#ifdef _INHERIT_kBODY
			int	iReturnVal = getTVector(
				kT,
				strand.strandId,
				inStart
			);
#else
			int	iReturnVal = __bodyMayaPtr->getTVector(
				kT,
				strand.strandId,
				inStart
			);
#endif
			if (iReturnVal < 0) {
				assert(iReturnVal == 0);
				return -1;
			}
			strandCache.pushCachedData(simulator, inStart);
			lastDoneFrameArray[indexStrand] = indexFrame;
		}
	}
	return 0;
}
int
kSimulationCache::setSimulationCache(
	unsigned	nthFrame,
	bool		force,
	int		numSamplePoints
)
{
if (__readOnly) {
#if 1
	if (!__filePtr) {
		cerr << "kSimulationCache::setSimulationCache: __filePtr is nil" << endl;
		return -1;
	}
	unsigned	strandDoCount = (unsigned) __strandArray.length();
	ShStrandRoot*	strandRootArrayPtr = __filePtr->getSRArrayInternal();
	if (!strandRootArrayPtr) {
		cerr << "kSimulationCache::setSimulationCache: ShStrandRoot::pSRArrayInternal is nil" << endl;
		return -1;
	}
	for (int indexFrame = 0; indexFrame < (int) nthFrame; ++indexFrame) {
		if (indexFrame < (int) __frameCacheArray.length())
			continue;
		kFrameCachePtr	frameCachePtr = new kFrameCache(indexFrame);
		if (!frameCachePtr) {
			(void) fprintf(stderr,
				"Error: kSimulationCache::setSimulationCache: new kFrameCache(): Failure: "
				"indexFrame = %d: Invalid kFrameCache\n"
				,
				indexFrame
			);
			__frameCacheArray.append(frameCachePtr);
			continue;
		}
		ShScalar	nowTime;
#if 0
		int	ret = __filePtr->readMultiOutputStrandCurvatureWithSegmentLengthArray(1 + indexFrame, strandRootArrayPtr, strandDoCount, &nowTime);
		for (int indexStrand = 0; indexStrand < (int) __strandArray.length(); ++indexStrand) {
			ShSimStrand&	simulator = *strandRootArrayPtr[indexStrand].pStrand;
			unsigned	numSegment = simulator.pStrandData.numSegment;
			KVector	segLen(numSegment);
			for (int i = 0; i < (int) numSegment; ++i)
				segLen(i) = simulator.pSimSegmentList[i].pSegmentData.fSegmentLength;
			cout << "indexStrand = " << indexStrand << endl;
			segLen.print("segmentLengthArray");
		}
#else
		int	ret = __filePtr->readMultiOutputStrandCurvature(1 + indexFrame, strandRootArrayPtr, strandDoCount, &nowTime);
#endif
		cout << "nowTime = " << nowTime << "(" << (1 + indexFrame) << ")" << endl;
		if (ret < 0) {
			cerr << "kSimulationCache::setSimulationCache: ShSimFileMultiStrand::readMultiOutputStrandCurvature(" << (1 + indexFrame) <<"): Failure" << endl;
			delete frameCachePtr;
			frameCachePtr = 0;
			continue;
		}
		for (int indexStrand = 0; indexStrand < (int) __strandArray.length(); ++indexStrand) {
#if 1
			ShSimStrand&	simulator = *strandRootArrayPtr[indexStrand].pStrand;
			unsigned	numSegment = simulator.pStrandData.numSegment;
			KVector	segLen(numSegment);
			for (int i = 0; i < (int) numSegment; ++i)
				segLen(i) = simulator.pSimSegmentList[i].pSegmentData.fSegmentLength;
			segLen.print("setSimulationCache: __readOnly = True");
			kStrandCachePtr	strandCachePtr = frameCachePtr->addStrandCache(indexStrand, segLen);
			const kStrand&	strand = __strandArray(indexStrand);
			strand.simulatorPtr->setStrandLengthStiffnessUsingDiffernentSegmentLength(&segLen);
#else
			const kStrand&	strand = __strandArray(indexStrand);
			kStrandCachePtr	strandCachePtr = frameCachePtr->addStrandCache(indexStrand, strand.numSegment);
#endif
			if (!strandCachePtr) {
				(void) fprintf(stderr,
					"Error: kSimulationCache::setSimulationCache: "
					"addStrandCache(indexStrand = %d (strandId = %d)): Failure: "
					"indexFrame = %d, indexStrand = %d: Invalid kStrandCache\n"
					,
					indexStrand, __strandArray(indexStrand).strandId,
					indexFrame, indexStrand
				);
				continue;
			}
			kStrandCache::CacheData&	data = strandCachePtr->data();
			data.rootTVector.T0 = strandRootArrayPtr[indexStrand].inStart;
			data.curvatureData.vector.setVector(strandRootArrayPtr[indexStrand].curvatureArray);
			strandCachePtr->setCachedTrue();
		}
		__frameCacheArray.append(frameCachePtr);
	}
#else
	for (int indexFrame = 0; indexFrame < (int) nthFrame; ++indexFrame) {
		if (indexFrame < (int) __frameCacheArray.length())
			continue;
		kFrameCachePtr	frameCachePtr = new kFrameCache(indexFrame);
		if (!frameCachePtr) {
			(void) fprintf(stderr,
				"Error: kSimulationCache::setSimulationCache: new kFrameCache(): Failure: "
				"indexFrame = %d: Invalid kFrameCache\n"
				,
				indexFrame
			);
			__frameCacheArray.append(frameCachePtr);
			continue;
		}
		for (int indexStrand = 0; indexStrand < (int) __strandArray.length(); ++indexStrand) {
			const kStrand&	strand = __strandArray(indexStrand);
			kStrandCachePtr	strandCachePtr = frameCachePtr->addStrandCache(indexStrand, strand.numSegment);
			if (!strandCachePtr) {
				(void) fprintf(stderr,
					"Error: kSimulationCache::setSimulationCache: "
					"addStrandCache(indexStrand = %d (strandId = %d)): Failure: "
					"indexFrame = %d, indexStrand = %d: Invalid kStrandCache\n"
					,
					indexStrand, __strandArray(indexStrand).strandId,
					indexFrame, indexStrand
				);
				continue;
			}
			kStrandFileCache&	strandFileCache = *((kStrandFileCachePtr) &strand);
			int	strandId = strandFileCache.strandId;
			typedef ShSimFileStrand*	ShSimFileStrandPtr;
			ShSimFileStrandPtr&	filePtr = strandFileCache.filePtr;
			if (!filePtr) {
				(void) fprintf(stderr,
					"FATAL: kSimulationCache::setSimulationCache: "
					"indexStrand = %d (strandId = %d): filePtr is nul: ignored\n"
					,
					indexStrand, strandId
				);
				continue;
			}
			kStrandCache::CacheData&	data = strandCachePtr->data();
			uint32	index;
			if (filePtr->readOutputStrandCurvature(
				&index,
				&data.rootTVector.T0,
				&data.curvatureData.vector
			) < 0) {
				(void) fprintf(stderr,
					"Error: kStrandCache::setSimulationCache: "
					"indexStrand = %d (strandId = %d): "
					"ShSimFileStrand::readOutputStrandCurvature(): Failure: "
					"Ignored w/ Invalidating filePtr\n"
					,
					indexStrand, strandId
				);
				delete filePtr;
				filePtr = NULL;
				continue;
			}
			if ((int) index != indexFrame) {
				(void) fprintf(stderr,
					"Error: kStrandCache::setSimulationCache: "
					"indexStrand = %d (strandId = %d): "
					"ShSimFileStrand::readOutputStrandCurvature(): "
					"index(%s) != indexFrame(%d): Ignored w/ Invalidating filePtr\n"
					,
					indexStrand, strandId, index, indexFrame
				);
				delete filePtr;
				filePtr = NULL;
				continue;
			}
			strandCachePtr->setCachedTrue();
		}
		__frameCacheArray.append(frameCachePtr);
	}
#endif
} else  {
	if (!__simulationParameterPtr) {
		(void) fprintf(stderr, "FATAL: kSimulationCache::setSimulationCache: __simulationParameterPtr is nil\n");
		return -1;
	}
	if (nthFrame == 0 || nthFrame > __nFrame)
		return -1;
	bool	isDirty = false;
	kFrameCache*	frameCachePtr = NULL;
	if (__frameCacheArray.length() >= nthFrame)
		frameCachePtr = __frameCacheArray[nthFrame - 1];
	bool	isCached = false;
	bool	isParameterChanged = false;
	bool	reset = false;
	if (
		force	||
		!frameCachePtr ||
		(isCached = frameCachePtr->isCached()) == false ||
		(isCached && (isParameterChanged = frameCachePtr->isParameterChanged(*__simulationParameterPtr, reset)) == true)
	) {
		cout << "isCached? " << (isCached ? "True" : "False") << endl;
		cout << "isParameterChanged? " << (isParameterChanged ? "True" : "False") << endl;
		isDirty = true;
	}
	if (
		isDirty == false &&
		frameCachePtr && isCached == 1
	) {
		kFrameCache&	frameCache = *frameCachePtr;
		for (int indexStrand = 0; indexStrand < (int) __strandArray.length(); ++indexStrand) {
			kStrandCache&	strandCache = *frameCache.strandCacheArray()[indexStrand];
			int	i = __isStrandCacheDirty(frameCache, strandCache);
			if (i < 0)
				return -1;
			if (i) {
				isDirty = true;
				break;
			}
		}
	}
	return isDirty && __setResult(nthFrame, force, numSamplePoints) < 0 ? -1 : 0;
}
	return 0;
}
int
kSimulationCache::getResult(
	int		indexStrand,
	unsigned	nthFrame,
	unsigned	numSamplePoints,
	ShVector3*	samplePoints
#if 1
	,
	float	cut_t,
	bool	head
#endif
)
{
	if (indexStrand < 0 || indexStrand >= (int) __strandArray.length())
		return -1;
	kStrand&	strand = __strandArray(indexStrand);
	if (nthFrame == 0 || nthFrame > __frameCacheArray.length())
		return -1;
	const kFrameCache&	frameCache = __frameCacheArray(nthFrame - 1);
	if (!&frameCache) {
		return -1;
	}
	const kStrandCache&	strandCache = frameCache.strandCacheArray()(indexStrand);
	if (!&strandCache || !strandCache.isCached())
		return -1;
	const kStrandCache::CacheData&	data = strandCache.data();
	ShSimStrand&	simulator = *strand.simulatorPtr;
#if 1
	KVector&	segLen = const_cast<KVector&>(strandCache.segmentLengthArray());
	strand.simulatorPtr->setStrandLengthStiffnessUsingDiffernentSegmentLength(&segLen);
#endif
	simulator.setCoordinateOnly((KVector*) &data.curvatureData.vector);
	simulator.updateCurvaturesAtSegments();
#if 1
	simulator.updateVextexPTNBVectorsAtSegments(true, (ShVector9*) &data.rootTVector.T0);
#else
	simulator.updateVextexPTNBVectorsAtSegments((ShVector9*) &data.rootTVector.T0);
#endif
#if 1
	if (cut_t < 0 || cut_t > 1) {
		if (simulator.getSamplePositions(numSamplePoints, samplePoints) < 0)
			return -1;
	} else {
		double	unit_length = (head ? cut_t : (1 - cut_t)) * simulator.pStrandData.fStrandLength / (numSamplePoints - 1);
		for (int k = 0; k < (int) numSamplePoints; ++k) {
			simulator.get3StrandPositionVector(samplePoints[k], (head ? 0 : cut_t) * simulator.pStrandData.fStrandLength + unit_length * k);
		}
	}
#else
	if (simulator.getSamplePositions(numSamplePoints, samplePoints) < 0)
		return -1;
#if 0
	double	unit_length = simulator.pStrandData.fStrandLength / (numSamplePoints - 1);
	for (int i = 0; i < (int) numSamplePoints; ++i) {
		cout << "samplePoints[" << i << "]: [" << samplePoints[i][0] << ", " << samplePoints[i][1] << ", " << samplePoints[i][2] << "]" << endl;
		ShVector3	p;
		simulator.get3StrandPositionVector(p, unit_length * i);
		cout << "samplePoints[" << i << "]: [" << p[0] << ", " << p[1] << ", " << p[2] << "]" << endl;
	}
#endif
#endif
#if 0
	double	unit_length = simulator.pStrandData.fStrandLength / (numSamplePoints - 1);
	for (int i = 0; i < (int) numSamplePoints; ++i) {
		cout << "samplePoints[" << i << "]: [" << samplePoints[i][0] << ", " << samplePoints[i][1] << ", " << samplePoints[i][2] << "]" << endl;
		ShVector3	p;
		simulator.get3StrandPositionVector(p, unit_length * i);
		cout << "samplePoints[" << i << "]: [" << p[0] << ", " << p[1] << ", " << p[2] << "]" << endl;
	}
#endif
	return 0;
}
#if 1
int
kSimulationCache::getResult0(
	int		indexStrand,
	unsigned	numSamplePoints,
	ShVector3*	samplePoints
)
{
	if (indexStrand < 0 || indexStrand >= (int) __strandArray.length())
		return -1;
	kStrand&	strand = __strandArray(indexStrand);
#if 0
	if (nthFrame == 0 || nthFrame > __frameCacheArray.length())
		return -1;
	const kFrameCache&	frameCache = __frameCacheArray(nthFrame - 1);
	if (!&frameCache) {
		return -1;
	}
	const kStrandCache&	strandCache = frameCache.strandCacheArray()(indexStrand);
	if (!&strandCache || !strandCache.isCached())
		return -1;
	const kStrandCache::CacheData&	data = strandCache.data();
#endif
	ShSimStrand&	simulator = *strand.simulatorPtr;
#if 1
	simulator.setCoordinateOnly((KVector*) &strand.initialCurvatureData.vector);
#else
	simulator.setCoordinateOnly((KVector*) &data.curvatureData.vector);
#endif
	simulator.updateCurvaturesAtSegments();
#if 1
#if 1
	TVector	inStart0;
	kTime	kT0(__timeUnit, 1, 1);
#ifdef _INHERIT_kBODY
	if (getTVector(
		kT0,
		strand.strandId,
		inStart0
	) < 0) {
		return -1;
	}
#else
	if (__bodyMayaPtr->getTVector(
		kT0,
		strand.strandId,
		inStart0
	) < 0) {
		return -1;
	}
#endif
	simulator.updateVextexPTNBVectorsAtSegments(true, &inStart0.T0);
#else
	simulator.updateVextexPTNBVectorsAtSegments(true, (ShVector9*) &data.rootTVector.T0);
#endif
#else
	simulator.updateVextexPTNBVectorsAtSegments((ShVector9*) &data.rootTVector.T0);
#endif
	if (simulator.getSamplePositions(numSamplePoints, samplePoints) < 0)
			return -1;
	return 0;
}
#endif
