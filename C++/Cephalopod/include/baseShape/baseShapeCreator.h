#pragma once
#define BASE_SHAPE__CREATOR_NODE_NAME	"baseShapeCreator"
#if 0
#endif
#if 3
#include <maya/MMatrix.h>
#endif
#include <maya/MPxNode.h>
#include <maya/MTypeId.h>
#include <maya/MPointArray.h>
#include <maya/MIntArray.h>
#include <maya/MVectorArray.h>
class baseShapeCreator : public MPxNode
{
public:
	baseShapeCreator();
	virtual ~baseShapeCreator();
#if 1
	MStatus	__compute(const MPlug& plug, MDataBlock& data);
#endif
	virtual MStatus	compute(const MPlug& plug, MDataBlock& data);
#if 0
private:
	static MObject	__externalWorldMatrix;
public:
	virtual MStatus	connectionMade(const MPlug& plug, const MPlug& otherPlug, bool asSrc);
	virtual MStatus	connectionBroken(const MPlug& plug, const MPlug& otherPlug, bool asSrc);
#endif
	static void*	creator();
	static MStatus	initialize();
	MStatus
	computeInputMesh(
		const MPlug&	plug,
		MDataBlock&	datablock,
		MPointArray&	vertices,
		MIntArray&	counts,
		MIntArray&	connects,
		MVectorArray&	normals
	);
	void
	buildCube(
		double	cube_size,
		MPointArray&	pa,
		MIntArray&	faceCounts,
		MIntArray&	faceConnects,
		MVectorArray&	normals
	);
	void
	buildSphere(
		double	radius,
		int	divisions,
		MPointArray&	pa,
		MIntArray&	faceCounts,
		MIntArray&	faceConnects,
		MVectorArray&	normals
	);
public:
	static MObject	size;
	static MObject	shapeType;
	static MObject	inputMesh;
	static MObject	outputSurface;
public:
	static MTypeId	id;
#if 3
	MMatrix	__matrix;
#endif
};
