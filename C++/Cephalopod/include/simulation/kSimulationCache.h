#pragma once
#include <simulation/kTime.h>
#include <baseShape/kPtrArray.h>
#include <simulation/kBody.h>
#ifndef _INHERIT_kBODY
#include <simulation/kBodyMaya.h>
#endif
#include <simulation/kFrameCache.h>
#include <simulation/kStrandCache.h>
#if 1
#define OS	1
#if 1
#include "../../../SBD/SBDlib/src_allheader/ShSimFileMultiStrand.h"
#include "../../../SBD/SBDlib/src_allheader/ShSimWind.h"
#include "../../../SBD/SBDlib/src_allheader/ShSimCollisionBody.h"
#else
	#include <SHD/ShSimFileMultiStrand.h>
	#include <SHD/ShSimWind.h>
	#include <SHD/ShSimCollisionBody.h>
#endif
#undef LINUX
#undef MSWIN
#undef TT
#undef CONSOLE
#undef OS
#include <string>
#endif
void	printStrand(FILE* fp, int index, const ShVector3* points, int n, const char* prefix = NULL);
void	printStrand(FILE* fp, int index, const ShVector3f* points, int n, const char* prefix = NULL);
class kSimulationCache
#ifdef _INHERIT_kBODY
	: public virtual kBody
#endif
{
#ifndef _INHERIT_kBODY
protected:
	kBodyMaya*	__bodyMayaPtr;
public:
	const kBodyMaya*	bodyMayaPtr() const	{return __bodyMayaPtr;}
	kBodyMaya*		bodyMayaPtr()		{return __bodyMayaPtr;}
#if 0
#if 0
	void	setMeshGeom(afterShaveGeometry* geometryPtr)		{__bodyMayaPtr->setMeshGeom(geometryPtr);}
	void	setCPPtr(MVectorArray* cpPtr)				{__bodyMayaPtr->setCPPtr(cpPtr);}
#endif
#if 1
	MStatus	set(const MDagPath& srcShape, bool external)		{return __bodyMayaPtr->set(srcShape, external);}
	void	set__T0_in_kSpace_array(const afterShaveGeometry* geometryPtr, const MVectorArray* cpPtr)	{__bodyMayaPtr->set__T0_in_kSpace_array(geometryPtr, cpPtr);}
#else
	MStatus	setDagPathShape(const MDagPath& dagPath, bool external)	{return __bodyMayaPtr->setDagPathShape(dagPath, external);}
#if 1
	void	setInclusiveMatrix(const MMatrix& matrix, bool external)	{__bodyMayaPtr->setInclusiveMatrix(matrix, external);}
	void	setExclusiveMatrix(const MMatrix& matrix, bool external)	{__bodyMayaPtr->setExclusiveMatrix(matrix, external);}
#else
	void	setWorldMatrix(const MMatrix& matrix, bool external)	{__bodyMayaPtr->setWorldMatrix(matrix, external);}
#endif
#endif
	bool	isValid(bool external, MStatus* pReturnStatus = NULL) const	{return __bodyMayaPtr->isValid(external, pReturnStatus);}
#endif
#endif
public:
	struct kSimulationParameter {
		enum kType {
			RaceType,
			IncludeStiffness,
			YoungModulus,
#if 1
			fAverageRadius,
			fEllipticity,
			fVolumeDensity,
			fHelixK0STep,
			fHelixK1Curvature,
			inHelixK0K1,
			fLengthFactor,
#endif
			IncludeGravityAndInertia,
			GravityParameter,
			IncludeAirFriction,
			AirFrictionParameter,
			DissipationMode,
			DissipationLambdaParameter,
			DissipationGammaParameter,
			WindMode,
			WindRatio,
			WindDirection2D,
			WindDirection3D,
			IncludeHairBodyOptimalCollision,
			IncludeHairBodyCollision,
			HBCParameter,
			IncludeHairHairCollision,
			HHCParameter,
		};
		union kValue {
			bool			bVal;
			numRaceType		raceType;
			numDissipationMode	dissipationMode;
			numWindMode		windMode;
			ShScalar		dVal;
			int			iVal;
			int			i2Val[2];
			ShVector2		v2Val;
			ShVector3		v3Val;
		};
	};
	void	setSimulationParameter(kSimulationParameter::kType type, kSimulationParameter::kValue& val);
protected:
	bool	__dynamics;
	ShSimParameter*		__simulationParameterPtr;
public:
	void	setDynamicsFlag(bool dynamics)	{__dynamics = dynamics;}
	const ShSimParameter&	simulationParameter() const	{return *__simulationParameterPtr;}
	ShSimParameter&	simulationParameter()			{return *__simulationParameterPtr;}
#if 1
private:
	bool	__readOnly;
public:
	int	writeToFileCache(const char* fullPathname) const;
	int	setStrandFromFileCache(const char* fullPathname, std::vector<double>* strandLengthArrayPtr = 0);
#endif
public:
	struct kStrand {
		int		strandId;
		unsigned	numSegment;
		double		length;
		kStrandCache::CurvatureData	initialCurvatureData;
		ShSimStrand*	simulatorPtr;
		kStrand(int id, unsigned nSegment, double len) : strandId(id), numSegment(nSegment), length(len), initialCurvatureData(nSegment), simulatorPtr(0) {}
		virtual ~kStrand() {if (simulatorPtr) {delete simulatorPtr; simulatorPtr = 0;}}
	};
	typedef kStrand*	kStrandPtr;
	typedef kPtrArray<kStrand>	kStrandArray;
private:
	kStrandArray	__strandArray;
public:
	const kStrandArray&	strandArray() const	{return __strandArray;}
	kStrandArray&		strandArray()		{return __strandArray;}
private:
#if 1
	ShSimFileMultiStrand*	__filePtr;
#else
	struct kStrandFileCache : public kStrand
	{
		ShSimFileMultiStrand*	filePtr;
		kStrandFileCache(int id, unsigned nSegment, double len) : kStrand(id, nSegment, len), filePtr(0) {}
		virtual ~kStrandFileCache() {if (filePtr) {delete filePtr; filePtr = 0;}}
	};
	typedef kStrandFileCache*	kStrandFileCachePtr;
#endif
private:
	int	__find_indexStrand(int strandId);
public:
#if 1
	const kStrandPtr	addStrand(int strandId, unsigned nSegment, double len, KVector* segLenPtr = 0);
#else
	const kStrandPtr	addStrand(int strandId, unsigned nSegment, double len);
#endif
	int	setStrandLength(int strandId, double len);
private:
	kTime::kTimeUnit	__timeUnit;
	unsigned		__nFrame;
public:
	kSimulationCache(
#ifndef _INHERIT_kBODY
		kBodyMaya*	bodyMayaPtr,
#endif
		kTime::kTimeUnit	timeUnit,
		unsigned	nFrame,
		bool		readOnly = false
	);
	virtual ~kSimulationCache();
private:
	kPtrArray<kFrameCache>	__frameCacheArray;
private:
	void	__setAllFramesVoid(bool setParameterDirty, int indexFrameStart, int indexStrand = -1);
	int	__isStrandCacheDirty(const kFrameCache& frameCache, const kStrandCache& strandCache) const;
private:
	int	__setResult(
		unsigned	nthFrame,
		bool		bForce = false,
		int		numSamplePoints = -1
	);
public:
	int	setSimulationCache(unsigned nthFrame, bool force = false, int numSamplePoints = -1);
public:
#if 1
#if 1
	int	getResult(int indexStrand, unsigned nthFrame, unsigned numSamplePoints, ShVector3* samplePoints, float cut_t = -1, bool head = true);
#else
	int	getResult(int indexStrand, unsigned nthFrame, unsigned numSamplePoints, ShVector3* samplePoints);
#endif
#if 1
	int	getResult0(int indexStrand, unsigned numSamplePoints, ShVector3* samplePoints);
#endif
#else
	int	getResult(
		unsigned	nthFrame,
		unsigned	numSamplePoints,
		bool		bForce = false,
		bool		bVerbose = false
	);
#endif
#ifdef _DEBUG
private:
	unsigned	__numKinematics;
	unsigned	__numDynamics;
public:
	unsigned	getNumKinematics() const	{return __numKinematics;}
	unsigned	getNumDynamics() const		{return __numDynamics;}
#endif
#if 1
#if 0
private:
	kPtrArray<kStrandCache::CurvatureData>	__initialCurvatureDataPtrArray;
	int	__setInitialCurvatureVector();
#endif
public:
	int	setInitialCurvatureVector(int indexStrand, const kStrandCache::CurvatureData& curvatureData);
	int	setInitialCurvatureVector(int indexStrand, unsigned inCountSample, const ShVector3* inSamples);
#endif
};
