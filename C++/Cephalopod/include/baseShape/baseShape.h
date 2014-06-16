#pragma once
#define BASE_SHAPE__NODE_NAME	"baseShapeNode"
#if 1
#include <maya/MUserEventMessage.h>
#endif
#include <baseShape/kPtrArray.h>
#include <maya/MDagPath.h>
#include <vector>
#if 0
#include <baseShape/HairInterpolator.h>
#endif
#if 4
class ShSimParameter;
class kShSimulationMayaTentacle;
#endif
#if 1
#include <maya/MPxComponentShape.h>
#else
#include <maya/MPxSurfaceShape.h>
#endif
#include <maya/MPxGeometryIterator.h>
#include <maya/MTypeId.h>
#include <maya/MPlug.h>
#include <maya/MDataBlock.h>
#include <maya/MVectorArray.h>
#include <maya/MDoubleArray.h>
#include <maya/MSelectionList.h>
#include <maya/MSelectionMask.h>
#include <baseShape/baseShapeGeometry.h>
#include <baseShape/baseShapeData.h>
#if 0
#if 1
#include <tentacle/kRamp.h>
#include <tentacle/kTentacleEnum.h>
struct	kTentacle;
#endif
#endif
class MPointArray;
class baseShape : public MPxComponentShape
{
public:
	baseShape();
	virtual ~baseShape();
	virtual void			postConstructor();
#if 1
	virtual MStatus	__compute(const MPlug& plug, MDataBlock& data);
#endif
	virtual MStatus	compute(const MPlug& plug, MDataBlock& data);
#if 1
	virtual bool	__getInternalValue(const MPlug&, MDataHandle&);
	virtual bool	__setInternalValue(const MPlug&, const MDataHandle&);
#endif
	virtual bool	getInternalValue(const MPlug&, MDataHandle&);
	virtual bool	setInternalValue(const MPlug&, const MDataHandle&);
	virtual MStatus	connectionMade(const MPlug& plug, const MPlug& otherPlug, bool asSrc);
	virtual MStatus	connectionBroken(const MPlug& plug, const MPlug& otherPlug, bool asSrc);
#if 1
	virtual MStatus	__shouldSave(const MPlug& plug, bool& result);
#endif
	virtual MStatus	shouldSave(const MPlug& plug, bool& result);
#if 1
	virtual void	__componentToPlugs(MObject&, MSelectionList&) const;
	virtual MatchResult	__matchComponent(const MSelectionList& item, const MAttributeSpecArray& spec, MSelectionList& list);
#endif
	virtual void	componentToPlugs(MObject&, MSelectionList&) const;
	virtual MatchResult	matchComponent(const MSelectionList& item, const MAttributeSpecArray& spec, MSelectionList& list);
	virtual bool	match(const MSelectionMask& mask, const MObjectArray& componentList) const;
	virtual MObject	createFullVertexGroup() const;
	virtual MObject	localShapeInAttr() const;
	virtual MObject	localShapeOutAttr() const;
#if 1
 	virtual MObject	__worldShapeOutAttr() const;
#endif
 	virtual MObject	worldShapeOutAttr() const;
 	virtual MObject	cachedShapeAttr() const;
	virtual MObject	geometryData() const;
	virtual void	closestPoint(const MPoint& toThisPoint, MPoint& theClosestPoint, double tolerance) const;
	virtual void	transformUsing(const MMatrix& mat, const MObjectArray& componentList);
#if 1
	virtual void	__transformUsing(const MMatrix& mat, const MObjectArray& componentList, MPxSurfaceShape::MVertexCachingMode cachingMode, MPointArray* pointCache);
#endif
	virtual void	transformUsing(const MMatrix& mat, const MObjectArray& componentList, MPxSurfaceShape::MVertexCachingMode cachingMode, MPointArray* pointCache);
	virtual void			tweakUsing( const MMatrix & mat,
										const MObjectArray & componentList,
										MVertexCachingMode cachingMode,
										MPointArray* pointCache,
										MArrayDataHandle& handle );
	virtual bool			vertexOffsetDirection( MObject & component,
                                                   MVectorArray & direction,
                                                   MVertexOffsetMode mode,
												   bool normalize );
	virtual bool		isBounded() const;
	virtual MBoundingBox	boundingBox() const;
#if 1
	virtual MPxGeometryIterator*	__geometryIteratorSetup(MObjectArray&, MObject&, bool forReadOnly = false);
#endif
	virtual MPxGeometryIterator*	geometryIteratorSetup(MObjectArray&, MObject&, bool forReadOnly = false);
	virtual bool			acceptsGeometryIterator(bool writeable = true);
	virtual bool			acceptsGeometryIterator(MObject&, bool writeable = true, bool forReadOnly = false);
#if 9
	virtual MStatus		setDependentsDirty(const MPlug& plugBeingDirtied, MPlugArray& affectedPlugs);
#endif
	bool		hasHistory();
    MStatus 		  		computeInputSurface( const MPlug&, MDataBlock& );
	MStatus		computeOutputSurface(const MPlug&, MDataBlock&);
    MStatus 		  		computeWorldSurface( const MPlug&, MDataBlock& );
#if 1
	MStatus		__computeBoundingBox(MDataBlock&);
#endif
	MStatus		computeBoundingBox(MDataBlock&);
#if 1
#else
	MStatus					applyTweaks( MDataBlock&, baseShapeGeometry* );
#endif
	bool		value(int pntInd, int vlInd, double& val) const;
	bool		value(int pntInd, MPoint& val) const;
	bool		setValue(int pntInd, int vlInd, double val);
	bool		setValue(int pntInd, const MPoint& val);
	MObject		meshDataRef();
	baseShapeGeometry*	meshGeom();
	MObject		cachedDataRef();
	baseShapeGeometry*	cachedGeom();
#if 1
	MStatus		__buildControlPoints(MDataBlock&, baseShapeGeometry& geometry);
	MStatus		buildControlPoints(MDataBlock&, baseShapeGeometry& geometry);
#else
	MStatus		buildControlPoints(MDataBlock&, int count);
#endif
	void		verticesUpdated();
	static void*	creator();
	static MStatus	initialize();
public:
	static MObject	inputSurface;
	static MObject	outputSurface;
	static MObject	worldSurface;
	static MObject	cachedSurface;
	static MObject	bboxCorner1;
	static MObject	bboxCorner2;
public:
	static MTypeId	id;
	bool	fHasHistoryOnCreate;
#if 1
#if 3
#if 1
protected:
	MDagPath	__externalDagPathShape;
	MMatrix		__localInclusiveMatrix;
	MMatrix		__localExclusiveMatrix;
	MMatrix		__externalInclusiveMatrix;
	MMatrix		__externalExclusiveMatrix;
public:
	const MMatrix&	localInclusiveMatrix() const	{return __localInclusiveMatrix;}
	const MMatrix&	localExclusiveMatrix() const	{return __localExclusiveMatrix;}
	const MMatrix&	externalInclusiveMatrix() const	{return __externalInclusiveMatrix;}
	const MMatrix&	externalExclusiveMatrix() const	{return __externalExclusiveMatrix;}
#else
private:
	MMatrix		__localWorldMatrix;
	MMatrix		__externalWorldMatrix;
public:
	const MMatrix&	localWorldMatrix() const	{return __localWorldMatrix;}
	const MMatrix&	externalWorldMatrix() const	{return __externalWorldMatrix;}
#endif
#endif
#if 2
protected:
	static MObject	worldMeshConnected;
	bool	__isWorldMeshConnected;
	kShSimulationMayaTentacle*		__shSimulationMayaPtr;
#if 4
#if 7
public:
	const kShSimulationMayaTentacle*		shSimulationMayaPtr() const	{return __shSimulationMayaPtr;}
protected:
	struct ShSimulationParameter {
		MObject	RaceType;
		MObject	IncludeStiffness;
		MObject	YoungModulus;
		MObject	IncludeGravityAndInertia;
		MObject	GravityParameter;
		MObject	IncludeAirFriction;
		MObject	AirFrictionParameter;
		MObject	DissipationMode;
		MObject	DissipationLambdaParameter;
		MObject	DissipationGammaParameter;
		MObject	WindMode;
		MObject	WindRatio;
		MObject	WindDirection2D;
#if 1
		MObject	WindDirection3D;
#else
		MObject	WindDirection3D;
#endif
		MObject	IncludeHairBodyOptimalCollision;
		MObject	IncludeHairBodyCollision;
		MObject	HBCParameter;
		MObject	IncludeHairHairCollision;
		MObject	HHCParameter;
		MObject InverseTimeStep;
		MObject	ParameterFileRead;
		MObject ParameterFileWrite;
#if 1
		MObject	fAverageRadius;
		MObject	fEllipticity;
		MObject	fVolumeDensity;
		MObject	fHelixK0STep;
		MObject	fHelixK1Curvature;
		MObject	inHelixK0STep;
		MObject	inHelixK1Curvature;
		MObject	fLengthFactor;
#else
		double	fAverageRadius;
		double	fEllipticity;
		double	fHelixK0STep;
		double	fHelixK1Curvature;
#endif
		double	fTolerance;
	};
	static MObject	Dynamics;
	static ShSimulationParameter	__ShSimulationParameter;
protected:
#if 1
	void	applySimulationParameter(bool all, bool exceptRaceType = false);
#else
	void	applySimulationParameter(bool all);
#endif
	void	setShSimulationParameterFromShSimParameter(const ShSimParameter& parameter, bool off);
	void	setShSimulationParameterToShSimParameter(ShSimParameter& parameter);
public:
	static void	printSimulationParameter(const ShSimParameter& parameter);
#endif
#if 6
protected:
	static MObject	FileCacheRead;
	static MObject	FileCacheWrite;
#endif
protected:
	static MObject	StrandIdList;
protected:
	static MObject			SimulationStatus;
	struct Simulation {
		enum Status {
			Off = -1,
			Paused = 0,
			On = 1,
			ReadOnly = 2,
		};
	};
	Simulation::Status	__simulationStatus;
#endif
#if 10
protected:
	static MObject	__externalFileReadOnly;
	bool	__isExternalFileReadOnly() const;
	int	__numExternalStrands;
public:
	int	numExternalStrands() const	{return __numExternalStrands;}
#endif
#if 11
protected:
	static MObject	__shiftFrame;
	int	getShiftFrame() const;
	void	setShiftFrame(int shiftFrame);
#endif
#endif
#if 1
protected:
	static MObject	time;
	static MObject	wakes;
public:
	bool	displayWakes() const
	{
		MPlug	plug(thisMObject(), wakes);
		bool	display;
		MStatus	ms = plug.getValue(display);
		return ms && display ? true : false;
	}
public:
	enum {
		selectByUnknown = -1,
		selectByGuide = 0,
		selectByVertex
	};
	static MObject	selectBy;
public:
	MStatus	getSelectBy(int& iSelectBy);
private:
	static MObject	guide;
public:
	static const unsigned	N_POINTS_IN_GUIDE;
protected:
	static MObject	numPointsInGuide;
	std::vector<double>	__segmentLengthInGuide;
	static MObject		seglen;
#endif
#if 0
#if 1
#if 1
private:
	MStatus		__computeBoundingBox(const MPlug& plug_tentacleArray, MDataBlock& datablock);
#endif
#if 1
private:
	MCallbackId	__activeListModifiedCallbackId;
	static void	__activeListModifiedCallback(void* voidPtr);
public:
	static int	lastSelectedPhysicalIndex;
#endif
#if 1
public:
	static const MString	TentacleArrayUpdated;
#endif
public:
	static MObject	rampDisplay;
private:
	static MObject	__rampWakes;
	RampWakes::Type	__rampWakesType;
private:
	kPtrArray<RampTuple>	__majorRadiusScaleRampTupleArray;
	kPtrArray<RampTuple>	__eccentricityScaleRampTupleArray;
	kPtrArray<RampTuple>	__rotationLocalampTupleArray;
	void	__setRampTupleArray(RampWakes::Type type);
#if 0
	void	__setRampTupleArray(RampWakes::Type type, MDataBlock datablock);
#endif
public:
	const kPtrArray<RampTuple>*	GetRampTupleArrayPtr() const;
	static int	GetInterpolatedValue(
		const kPtrArray<RampTuple>&	rampTupleArray,
		float	x,
		float&	y,
		bool	clamp
	);
#if 0
	struct TentacleParameter {
		MObject	tentacleCrossSectionRadius;
		MObject	tentacleCrossSectionEccentricity;
		MObject	tentacleCrossSectionRotation;
		MObject	tentacleCut;
	};
	static TentacleParameter	__tentacleParameter;
#endif
private:
	static MObject	__tentacleParameter;
#if 0
private:
	static MObject	transversal;
public:
	void	getTransverseVectors(MVectorArray& transVectors);
#endif
#if 1
#if 1
private:
	static MObject	__drawTentacleRoot;
	static MObject	__tentacleRoot;
public:
	MStatus	getDrawTentacleRootStatus(bool& status);
	MStatus	getTentacleRootPosition(MPoint& position);
#endif
#if 0
private:
	static MObject	__tentacleRemovedIndex;
#endif
private:
	static MObject	__tentacleDoAction;
private:
	static MObject	__tentacleArray;
public:
	MStatus	getTentacleArray(kPtrArray<kTentacle>& tentacleArray);
#endif
#endif
#endif
#if 0
	static unsigned	N_POINTS_IN_HAIR;
	static MObject	numPointsInHair;
	static MObject	segmentLengthInHair;
	int&	xRef();
	int	calculateY();
	MStatus	computeX(const MPlug& plug, MDataBlock& data);
#endif
#endif
#if 0
private:
	static HairInterpolator*	hairInterpolatorCreator(const HairInterpolator::Arg& arg);
private:
	HairInterpolator*		__hairInterpolatorPtr;
	static MObject			beInterpolated;
	bool				__displayHair;
	int				__numPolygons;
#if 1
	static MObject			hairControlPoints;
	static MObject			numHairs;
	static MObject			numSamplePointsInHair;
#endif
public:
	const HairInterpolator&		hairInterpolator() const	{return *__hairInterpolatorPtr;}
	bool				displayHair() const		{return __displayHair;}
	int				numPolygons() const		{return __numPolygons;}
#endif
#if 0
private:
	static MObject	attenuateLength;
	double		__attenuateLength_averageArea;
	MDoubleArray	__attenuateLength_factorArray;
#endif
};
