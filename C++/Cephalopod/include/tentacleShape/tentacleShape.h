#pragma once
#ifndef __TENTACLE_SHAPE__
#define __TENTACLE_SHAPE__
#define TENTACLE__SHAPE_NODE_NAME	"tentacleShapeNode"
#include <baseShape/baseShape.h>
#if 3
#include <tentacleShape/kTentacle.h>
#if 1
#include <baseShape/kSkinSmoothBind.h>
#endif
#else
struct kTentacle;
#endif
#include <tentacleShape/kRamp.h>
#include <tentacleShape/kTentacleEnum.h>
#if 1
class MIntArray;
class MFloatArray;
#endif
class tentacleShape : public baseShape
{
public:
	static void*	creator();
	static MStatus	initialize();
public:
	tentacleShape();
	virtual ~tentacleShape();
#if 1
public:
	virtual void	postConstructor();
	virtual MStatus	compute(const MPlug& plug, MDataBlock& datablock);
	virtual MStatus	setDependentsDirty(const MPlug& plugBeingDirtied, MPlugArray& affectedPlugs);
#endif
#if 1
#if 1
#if 1
#if 1
private:
	MStatus		__computeBoundingBox(const MPlug& plug_tentacleArray, MDataBlock& datablock);
#if 2
	MStatus		__computeBoundingBox(MDataBlock& datablock);
#endif
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
	kPtrArray<RampTuple>	__rotationLocalRampTupleArray;
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
		bool	clamp,
		bool	verbose = false
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
public:
	static MObject	__tentacleArray;
private:
#if 2
	static MObject	__tentacleArrayArray;
	static MObject	__frameIndex;
#endif
public:
	MStatus	getTentacleArray(kPtrArray<kTentacle>& tentacleArray);
	static MStatus	getTentacleArray(
		const MPlug&	plug_tentacleArray,
		unsigned	numElements,
#if 1
		kPtrArray<kTentacle>&	tentacleArray,
		const kSkinSmoothBind*	skinSmoothBindPtr = NULL
#else
		kPtrArray<kTentacle>&	tentacleArray
#endif
	);
#if 3
private:
	MStatus	__setTentacleMesh();
protected:
	MStatus	__setTentaclePoints();
private:
	kPtrArray<kTentacle>	__tentacles;
public:
	const kPtrArray<kTentacle>&	tentacles() const	{return __tentacles;}
#if 1
private:
	kSkinSmoothBind	__skinSmoothBind;
public:
	const kSkinSmoothBind&	skinSmoothBind() const	{return __skinSmoothBind;}
#if 0
private:
	MStatus	__setSkinSmoothBind();
#endif
#endif
#endif
#endif
#endif
#endif
#endif
#if 1
private:
	static MObject	outputMesh;
public:
	MStatus	tentacleMeshSetCutTentacle(
		const kTentacle&	tentacle0,
		const MPlug&		plug_tentacle,
		kTentacle&	cut_tentacle,
		float	cut_t,
		bool	head
	);
	static void	tentacleMeshSetUVArray(
		MFloatArray&	uArray,
		MFloatArray&	vArray,
		float	cut_t = -1.f,
		bool	head = true
	);
	static MObject	tentacleCreateMesh(MObject& outData, MStatus& ms);
	static MStatus	tentacleSetPoints(MPlug& plug_mesh,  MPointArray& tentacleSamplePoints, MSpace::Space space = MSpace::kWorld);
#if 1
	static MStatus	tentacle_setUVs(
		MPlug&	plug_mesh,
		const MFloatArray& uArray, const MFloatArray& vArray
	);
#endif
	static const int	numEllipseSamplePoints;
#if 0
public:
	bool	updateConnection;
#endif
#endif
#if 2
#if 1
public:
	virtual bool	setInternalValue(const MPlug&, const MDataHandle&);
#endif
#if 1
private:
	MVectorArray	__strand_longitudinals;
public:
	const MVectorArray&	strand_longitudinals() const	{return __strand_longitudinals;}
#endif
#endif
};
#endif
