#pragma once
#include <maya/MStatus.h>
#include <maya/MTransformationMatrix.h>
class MFnDependencyNode;
class MDagPath;
class MObject;
#if 1
class MPlug;
#endif
#if 0
class MObject;
#endif
class MTime;
class MMatrix;
#ifdef __cplusplus
MStatus
findExternalDagPath2(
	const MFnDependencyNode&	fnDepNode,
	MDagPath&	dagPathShape
);
#if 0
MStatus
findCreatorObj(
	const MFnDependencyNode&	fnDepNode,
	MObject&	creatorObj
);
#endif
#if 1
MStatus
findTentacleShapeNode(
	const MFnDependencyNode&	fnDepNode,
	MDagPath&	dagPathShape
);
#endif
namespace kNode {
	MStatus	findExternalDagPath(const MFnDependencyNode& fnDepNode, MDagPath& dagPathShape);
	MStatus	find_plug_in_shape_node(const MObject& creator, const char* typeName, MObject& shape);
#if 1
	MStatus	find_visibility_plug(const MPlug& plug_outputMesh, MPlug& plug_visibility);
#endif
}
namespace kUtil {
MStatus	getWorldMatrixAt(
	const MDagPath&	dagPathShape,
	const MTime&	time,
	MMatrix&	evaluatedWorldMatrix,
	const MTransformationMatrix::RotationOrder*	rotationOrderPtr = 0
);
}
#endif
