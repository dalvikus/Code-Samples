#include <baseShape/kUtil.h>
#include <maya/MPlug.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MPlugArray.h>
#include <maya/MFnDagNode.h>
#include <maya/MDagPath.h>
#include <maya/MAnimUtil.h>
#include <maya/MVector.h>
#include <maya/MObjectArray.h>
#include <maya/MFnAnimCurve.h>
#include <maya/MFnTransform.h>
#include <maya/MMatrix.h>
#include <maya/MPoint.h>
#ifdef __cplusplus
MStatus
findExternalDagPath2(
	const MFnDependencyNode&	fnDepNode,
	MDagPath&	dagPathShape
)
{
	MStatus	ms;
#if 0
	MObject	node_baseShapeCreator = plug_outputSurface.node(&ms);
	if (!ms) {
		cerr << "Error: findExternalDagPath: ";
		cerr << "|" << plug_outputSurface.info() << "|: ";
		cerr << "MPlug.node(): Failure: ";
		cerr << ms << endl;
		return ms;
	}
	MFnDependencyNode	depNode_baseShapeCreator(node_baseShapeCreator, &ms);
	if (!ms) {
		cerr << "Error: findExternalDagPath: ";
		cerr << "|" << plug_outputSurface.info() << "|: ";
		cerr << "MFnDependencyNode::MFnDependencyNode(MObject): Failure: ";
		cerr << ms << endl;
		return ms;
	}
#endif
	MPlug	plug_inputMesh = fnDepNode.findPlug("inputMesh", false, &ms);
	if (!ms) {
		cerr << "Error: findExternalDagPath: ";
		cerr << "MFnDependencyNode::findPlug(|inputMesh|, false): Failure";
		cerr << ms << endl;
		return MStatus::kFailure;
	}
	MPlugArray	connectedPlugs_inputMesh;
	bool	bFound_inputMesh = plug_inputMesh.connectedTo(connectedPlugs_inputMesh, true, false, &ms);
	if (!ms) {
		cerr << "Error: findExternalDagPath: ";
		cerr << "MPlug::connectedTo(true, false): Failure";
		cerr << ms << endl;
		return MStatus::kFailure;
	}
	if (!bFound_inputMesh) {
		cerr << "Warning: findExternalDagPath: ";
		cerr << "No connection to |inputMesh|";
		cerr << endl;
		return MStatus::kNotFound;
	}
	if (connectedPlugs_inputMesh.length() == 0) {
		cerr << "Warning: findExternalDagPath: ";
		cerr << "No connection to |inputMesh| as Destination";
		return MStatus::kNotFound;
	}
	MPlug	plug_worldMeshArray = connectedPlugs_inputMesh[0];
	bool	isElement = plug_worldMeshArray.isElement(&ms);
	if (!ms) {
		cerr << "Error: findExternalDagPath: ";
		cerr << "|" << plug_worldMeshArray.info() << "|: ";
		cerr << "MPlug.isElement(): Failure: ";
		cerr << ms << endl;
		return MStatus::kFailure;
	}
	if (!isElement) {
		cerr << "Warning: findExternalDagPath: ";
		cerr << "|" << plug_worldMeshArray.info() << "|: ";
		cerr << "MPlug.isElement() return False: ";
		cerr << "Not an element";
		return MStatus::kNotFound;
	}
	MPlug	plug_worldMesh = plug_worldMeshArray.array(&ms);
	if (!ms) {
		cerr << "Error: findExternalDagPath: ";
		cerr << "|" << plug_worldMeshArray.info() << "|: ";
		cerr << "MPlug.array(): Failure: ";
		cerr << ms << endl;
		return MStatus::kFailure;
	}
	MString	partialName_useLongNames = plug_worldMesh.partialName(false, false, false, false, false, true, &ms);
	if (!ms) {
		cerr << "Error: findExternalDagPath: ";
		cerr << "|" << plug_worldMesh.info() << "|: ";
		cerr << "MPlug.partialName(false, false, false, false, false, true): Failure";
		cerr << ms << endl;
		return MStatus::kFailure;
	}
	if (partialName_useLongNames != "worldMesh") {
		cerr << "Warning: findExternalDagPath: ";
		cerr << "|" << plug_worldMesh.info() << "|: ";
		cerr << "Not |worldMesh|: |" << partialName_useLongNames << "|" << endl;
		return MStatus::kNotFound;
	}
	MObject	mesh = plug_worldMesh.node(&ms);
	if (!ms) {
		cerr << "Error: findExternalDagPath: ";
		cerr << "|" << plug_worldMesh.info() << "|: ";
		cerr << "MPlug.node(): Failure: ";
		cerr << ms << endl;
		return MStatus::kFailure;
	}
	if (!mesh.hasFn(MFn::kMesh) || !mesh.hasFn(MFn::kShape)) {
		cerr << "Error: findExternalDagPath: ";
		cerr << "|" << plug_worldMesh.info() << "|: ";
		cerr << "No MFn::kMesh and MFn::kShape" << endl;
		return MStatus::kFailure;
	}
	MFnDagNode	dagNode(mesh, &ms);
	if (!ms) {
		cerr << "Error: findExternalDagPath: ";
		cerr << "|" << plug_worldMesh.info() << "|: ";
		cerr << "MFnDagNode::MFnDagNode(MObject): Failure: ";
		cerr << ms << endl;
		return MStatus::kFailure;
	}
	ms = dagNode.getPath(dagPathShape);
	if (!ms) {
		cerr << "Error: findExternalDagPath: ";
		cerr << "|" << plug_worldMesh.info() << "|: ";
		cerr << "MFnDagNode::getPath(MDagPath&): Failure: ";
		cerr << ms << endl;
		return MStatus::kFailure;
	}
	return MStatus::kSuccess;
}
namespace kNode {
MStatus	findExternalDagPath(const MFnDependencyNode& fnDepNode, MDagPath& dagPathShape)
{
	MStatus	ms;
#if 0
	MObject	node_shapeNode = plug_shapeNode.node(&ms);
	if (!ms) {
		cerr << "Error: findExternalDagPath: ";
		cerr << "MPlug.node(): Failure: ";
		cerr << ms << endl;
		return ms;
	}
	MFnDependencyNode	depNode_shapeNode(node_shapeNode, &ms);
	if (!ms) {
		cerr << "Error: findExternalDagPath: ";
		cerr << "MFnDependencyNode::MFnDependencyNode(MObject): Failure: ";
		cerr << ms << endl;
		return ms;
	}
#endif
	MPlug	plug_inputSurface = fnDepNode.findPlug("inputSurface", false, &ms);
	if (!ms) {
		cerr << "Error: findExternalDagPath: ";
		cerr << "MFnDependencyNode::findPlug(|inputSurface|, false): Failure";
		cerr << ms << endl;
		return MStatus::kFailure;
	}
	MPlugArray	connectedPlugs_inputSurface;
	bool	bFound_inputSurface = plug_inputSurface.connectedTo(connectedPlugs_inputSurface, true, false, &ms);
	if (!ms) {
		cerr << "Error: findExternalDagPath: ";
		cerr << "MPlug::connectedTo(true, false): Failure";
		cerr << ms << endl;
		return MStatus::kFailure;
	}
	if (!bFound_inputSurface) {
		cerr << "Warning: findExternalDagPath: ";
		cerr << "No connection to |inputSurface|";
		cerr << endl;
		return MStatus::kFailure;
	}
	if (connectedPlugs_inputSurface.length() == 0) {
		cerr << "Warning: findExternalDagPath: ";
		cerr << "No connection to |inputSurface| as Destination";
		return MStatus::kFailure;
	}
	MPlug	plug_outputSurface = connectedPlugs_inputSurface[0];
	MObject	node_baseShapeCreator = plug_outputSurface.node(&ms);
	if (!ms) {
		cerr << "Error: findExternalDagPath: ";
		cerr << "|" << plug_outputSurface.info() << "|: ";
		cerr << "MPlug.node(): Failure: ";
		cerr << ms << endl;
		return MStatus::kFailure;
	}
	MFnDependencyNode	depNode_baseShapeCreator(node_baseShapeCreator, &ms);
	if (!ms) {
		cerr << "Error: findExternalDagPath: ";
		cerr << "|" << plug_outputSurface.info() << "|: ";
		cerr << "MFnDependencyNode::MFnDependencyNode(MObject): Failure: ";
		cerr << ms << endl;
		return MStatus::kFailure;
	}
	MPlug	plug_inputMesh = depNode_baseShapeCreator.findPlug("inputMesh", false, &ms);
	if (!ms) {
		cerr << "Error: findExternalDagPath: ";
		cerr << "|" << plug_outputSurface.info() << "|: ";
		cerr << "MFnDependencyNode::findPlug(|inputMesh|, false): Failure";
		cerr << ms << endl;
		return MStatus::kFailure;
	}
	MPlugArray	connectedPlugs_inputMesh;
	bool	bFound_inputMesh = plug_inputMesh.connectedTo(connectedPlugs_inputMesh, true, false, &ms);
	if (!ms) {
		cerr << "Error: findExternalDagPath: ";
		cerr << "|" << plug_outputSurface.info() << "|: ";
		cerr << "MPlug::connectedTo(true, false): Failure";
		cerr << ms << endl;
		return MStatus::kFailure;
	}
	if (!bFound_inputMesh) {
		cerr << "Warning: findExternalDagPath: ";
		cerr << "|" << plug_outputSurface.info() << "|: ";
		cerr << "No connection to |inputMesh|";
		cerr << endl;
		return MStatus::kNotFound;
	}
	if (connectedPlugs_inputMesh.length() == 0) {
		cerr << "Warning: findExternalDagPath: ";
		cerr << "|" << plug_outputSurface.info() << "|: ";
		cerr << "No connection to |inputMesh| as Destination";
		return MStatus::kNotFound;
	}
	MPlug	plug_worldMeshArray = connectedPlugs_inputMesh[0];
	bool	isElement = plug_worldMeshArray.isElement(&ms);
	if (!ms) {
		cerr << "Error: findExternalDagPath: ";
		cerr << "|" << plug_worldMeshArray.info() << "|: ";
		cerr << "MPlug.isElement(): Failure: ";
		cerr << ms << endl;
		return MStatus::kFailure;
	}
	if (!isElement) {
		cerr << "Warning: findExternalDagPath: ";
		cerr << "|" << plug_worldMeshArray.info() << "|: ";
		cerr << "MPlug.isElement() return False: ";
		cerr << "Not an element";
		return MStatus::kNotFound;
	}
	MPlug	plug_worldMesh = plug_worldMeshArray.array(&ms);
	if (!ms) {
		cerr << "Error: findExternalDagPath: ";
		cerr << "|" << plug_worldMeshArray.info() << "|: ";
		cerr << "MPlug.array(): Failure: ";
		cerr << ms << endl;
		return MStatus::kFailure;
	}
	MString	partialName_useLongNames = plug_worldMesh.partialName(false, false, false, false, false, true, &ms);
	if (!ms) {
		cerr << "Error: findExternalDagPath: ";
		cerr << "|" << plug_worldMesh.info() << "|: ";
		cerr << "MPlug.partialName(false, false, false, false, false, true): Failure";
		cerr << ms << endl;
		return MStatus::kFailure;
	}
	if (partialName_useLongNames != "worldMesh") {
		cerr << "Warning: findExternalDagPath: ";
		cerr << "|" << plug_worldMesh.info() << "|: ";
		cerr << "Not |worldMesh|: |" << partialName_useLongNames << "|" << endl;
		return MStatus::kNotFound;
	}
	MObject	mesh = plug_worldMesh.node(&ms);
	if (!ms) {
		cerr << "Error: findExternalDagPath: ";
		cerr << "|" << plug_worldMesh.info() << "|: ";
		cerr << "MPlug.node(): Failure: ";
		cerr << ms << endl;
		return MStatus::kFailure;
	}
	if (!mesh.hasFn(MFn::kMesh) || !mesh.hasFn(MFn::kShape)) {
		cerr << "Error: findExternalDagPath: ";
		cerr << "|" << plug_worldMesh.info() << "|: ";
		cerr << "No MFn::kMesh and MFn::kShape" << endl;
		return MStatus::kFailure;
	}
	MFnDagNode	dagNode(mesh, &ms);
	if (!ms) {
		cerr << "Error: findExternalDagPath: ";
		cerr << "|" << plug_worldMesh.info() << "|: ";
		cerr << "MFnDagNode::MFnDagNode(MObject): Failure: ";
		cerr << ms << endl;
		return MStatus::kFailure;
	}
	ms = dagNode.getPath(dagPathShape);
	if (!ms) {
		cerr << "Error: findExternalDagPath: ";
		cerr << "|" << plug_worldMesh.info() << "|: ";
		cerr << "MFnDagNode::getPath(MDagPath&): Failure: ";
		cerr << ms << endl;
		return MStatus::kFailure;
	}
	return MStatus::kSuccess;
}
MStatus	find_plug_in_shape_node(const MObject& creator, const char* typeName, MObject& shape)
{
	MStatus	ms;
	MFnDependencyNode	depNode_creator(creator, &ms);
	if (!ms) {
		ms.perror("MFnDependencyNode::MFnDependencyNode(): Failure");
		return ms;
	}
	MPlug	plug_outputSurface = depNode_creator.findPlug("outputSurface", false, &ms);
	if (!ms) {
		ms.perror("MFnDependencyNode::findPlug(|outputSurface|): Failure");
		return ms;
	}
	MPlugArray	connectedPlugs_inputSurface;
	bool	bFound_inputSurface = plug_outputSurface.connectedTo(connectedPlugs_inputSurface, false, true, &ms);
	if (!ms) {
		ms.perror("MPlug::connectedTo(asDst = false, asSrc = true): Failure");
		return ms;
	}
	if (!bFound_inputSurface) {
		cerr << "WARN: No connection to |outputSurface|" << endl;
		return ms;
	}
	if (connectedPlugs_inputSurface.length() == 0) {
		cerr << "WARN: No connection from |outputSurface| as source" << endl;
		return ms;
	}
	MPlug	plug_inputSurface = connectedPlugs_inputSurface[0];
	MObject	obj_shape_node = plug_inputSurface.node(&ms);
	if (!ms) {
		ms.perror("MPlug::node(): Failure");
		return ms;
	}
	MFnDependencyNode	depNode_shape(obj_shape_node, &ms);
	if (!ms) {
		ms.perror("MFnDependencyNode::MFnDependencyNode(): Failure");
		return ms;
	}
	if (depNode_shape.typeName() != typeName) {
		cerr << "typeName does not match" << endl;
		return MStatus::kNotFound;
	}
	shape = obj_shape_node;
	return MStatus::kSuccess;
}
#if 1
MStatus	find_visibility_plug(const MPlug& plug_outputMesh, MPlug& plug_visibility)
{
	MStatus	ms;
	MPlugArray	connectedPlugs_inMesh;
	bool	bFound_inMesh = plug_outputMesh.connectedTo(connectedPlugs_inMesh, false, true, &ms);
	if (!ms) {
		cerr << "Error: find_visibility_plug: ";
		cerr << "|" << plug_outputMesh.info() << "|: ";
		cerr << "MPlug::connectedTo(false, true): Failure";
		cerr << ms << endl;
		return ms;
	}
	if (!bFound_inMesh) {
		cerr << "Warning: find_visibility_plug: ";
		cerr << "|" << plug_outputMesh.info() << "|: ";
		cerr << "No connection |.outputMesh| to |.inMesh|";
		cerr << endl;
		return ms;
	}
	if (connectedPlugs_inMesh.length() == 0) {
		cerr << "Warning: find_visibility_plug: ";
		cerr << "|" << plug_outputMesh.info() << "|: ";
		cerr << "No connection |.outputMesh| to |.inMesh| as Source";
		return MStatus::kNotFound;
	}
	MPlug&	plug_mesh = connectedPlugs_inMesh[0];
	MObject	mesh = plug_mesh.node(&ms);
	if (!ms) {
		cerr << "Error: find_visibility_plug: ";
		cerr << "|" << plug_mesh.info() << "|: ";
		cerr << "MPlug.node(): Failure: ";
		cerr << ms << endl;
		return ms;
	}
	MFnDagNode	dagShapeNode(mesh, &ms);
	if (!ms) {
		cerr << "Error: find_visibility_plug: ";
		cerr << "|" << plug_mesh.info() << "|: ";
		cerr << "MFnDagNode::MFnDagNode(MObject): Failure: ";
		cerr << ms << endl;
		return ms;
	}
	MDagPath	dagPathShape;
	ms = dagShapeNode.getPath(dagPathShape);
	if (!ms) {
		cerr << "Error: find_visibility_plug: ";
		cerr << "|" << plug_mesh.info() << "|: ";
		cerr << "MFnDagNode::getPath(MDagPath&): Failure: ";
		cerr << ms << endl;
		return ms;
	}
	if (!ms)
		return ms;
	MObject	transform = dagPathShape.transform(&ms);
	if (!ms)
		return ms;
	MFnDagNode	dagTransformNode(transform, &ms);
	if (!ms)
		return ms;
	MDagPath	dagPathTransform;
	ms = dagTransformNode.getPath(dagPathTransform);
	if (!ms)
		return ms;
	plug_visibility = dagTransformNode.findPlug("visibility", false, &ms);
	return ms;
}
#endif
}
#if 0
MStatus
findCreatorObj(
	const MFnDependencyNode&	fnDepNode,
	MObject&	creatorObj
)
{
	MStatus	ms;
#if 0
	MObject	node_shapeNode = plug_shapeNode.node(&ms);
	if (!ms) {
		cerr << "Error: findExternalDagPath: ";
		cerr << "MPlug.node(): Failure: ";
		cerr << ms << endl;
		return ms;
	}
	MFnDependencyNode	depNode_shapeNode(node_shapeNode, &ms);
	if (!ms) {
		cerr << "Error: findExternalDagPath: ";
		cerr << "MFnDependencyNode::MFnDependencyNode(MObject): Failure: ";
		cerr << ms << endl;
		return ms;
	}
#endif
	MPlug	plug_inputSurface = fnDepNode.findPlug("inputSurface", false, &ms);
	if (!ms) {
		cerr << "Error: findExternalDagPath: ";
		cerr << "MFnDependencyNode::findPlug(|inputSurface|, false): Failure";
		cerr << ms << endl;
		return MStatus::kFailure;
	}
	MPlugArray	connectedPlugs_inputSurface;
	bool	bFound_inputSurface = plug_inputSurface.connectedTo(connectedPlugs_inputSurface, true, false, &ms);
	if (!ms) {
		cerr << "Error: findExternalDagPath: ";
		cerr << "MPlug::connectedTo(true, false): Failure";
		cerr << ms << endl;
		return MStatus::kFailure;
	}
	if (!bFound_inputSurface) {
		cerr << "Warning: findExternalDagPath: ";
		cerr << "No connection to |inputSurface|";
		cerr << endl;
		return MStatus::kFailure;
	}
	if (connectedPlugs_inputSurface.length() == 0) {
		cerr << "Warning: findExternalDagPath: ";
		cerr << "No connection to |inputSurface| as Destination";
		return MStatus::kFailure;
	}
	MPlug	plug_outputSurface = connectedPlugs_inputSurface[0];
	creatorObj = plug_outputSurface.node(&ms);
	if (!ms) {
		cerr << "Error: findExternalDagPath: ";
		cerr << "|" << plug_outputSurface.info() << "|: ";
		cerr << "MPlug.node(): Failure: ";
		cerr << ms << endl;
		return MStatus::kFailure;
	}
	return MStatus::kSuccess;
}
#endif
MStatus
findTentacleShapeNode(
	const MFnDependencyNode&	fnDepNode,
	MDagPath&	dagPathShape
)
{
	MStatus	ms;
#if 0
	MObject	node_shapeNode = plug_shapeNode.node(&ms);
	if (!ms) {
		cerr << "Error: findExternalDagPath: ";
		cerr << "MPlug.node(): Failure: ";
		cerr << ms << endl;
		return ms;
	}
	MFnDependencyNode	depNode_shapeNode(node_shapeNode, &ms);
	if (!ms) {
		cerr << "Error: findExternalDagPath: ";
		cerr << "MFnDependencyNode::MFnDependencyNode(MObject): Failure: ";
		cerr << ms << endl;
		return ms;
	}
#endif
	MPlug	plug_worldMesh = fnDepNode.findPlug("worldMesh", false, &ms);
	if (!ms) {
		cerr << "Error: findExternalDagPath: ";
		cerr << "MFnDependencyNode::findPlug(|inputSurface|, false): Failure";
		cerr << ms << endl;
		return MStatus::kFailure;
	}
	MPlug	plug_worldMesh0 = plug_worldMesh[0];
	MPlugArray	connectedPlugs_inputMesh;
	bool	bFound_inputSurface = plug_worldMesh0.connectedTo(connectedPlugs_inputMesh, false, true, &ms);
	if (!ms) {
		cerr << "Error: findExternalDagPath: ";
		cerr << "MPlug::connectedTo(true, false): Failure";
		cerr << ms << endl;
		return MStatus::kFailure;
	}
	if (!bFound_inputSurface) {
		cerr << "Warning: findExternalDagPath: ";
		cerr << "No connection to |inputSurface|";
		cerr << endl;
		return MStatus::kFailure;
	}
	if (connectedPlugs_inputMesh.length() == 0) {
		cerr << "Warning: findExternalDagPath: ";
		cerr << "No connection to |inputSurface| as Destination";
		return MStatus::kFailure;
	}
	MPlug	plug_outputSurface = connectedPlugs_inputMesh[0];
	MObject	node_baseShapeCreator = plug_outputSurface.node(&ms);
	if (!ms) {
		cerr << "Error: findExternalDagPath: ";
		cerr << "|" << plug_outputSurface.info() << "|: ";
		cerr << "MPlug.node(): Failure: ";
		cerr << ms << endl;
		return MStatus::kFailure;
	}
	MFnDependencyNode	depNode_baseShapeCreator(node_baseShapeCreator, &ms);
	if (!ms) {
		cerr << "Error: findExternalDagPath: ";
		cerr << "|" << plug_outputSurface.info() << "|: ";
		cerr << "MFnDependencyNode::MFnDependencyNode(MObject): Failure: ";
		cerr << ms << endl;
		return MStatus::kFailure;
	}
	MPlug	plug_inputMesh = depNode_baseShapeCreator.findPlug("outputSurface", false, &ms);
	if (!ms) {
		cerr << "Error: findExternalDagPath: ";
		cerr << "|" << plug_outputSurface.info() << "|: ";
		cerr << "MFnDependencyNode::findPlug(|inputMesh|, false): Failure";
		cerr << ms << endl;
		return MStatus::kFailure;
	}
	MPlugArray	connectedPlugs_inputSurface;
	bool	bFound_inputMesh = plug_inputMesh.connectedTo(connectedPlugs_inputSurface, false, true, &ms);
	if (!ms) {
		cerr << "Error: findExternalDagPath: ";
		cerr << "|" << plug_outputSurface.info() << "|: ";
		cerr << "MPlug::connectedTo(true, false): Failure";
		cerr << ms << endl;
		return MStatus::kFailure;
	}
	if (!bFound_inputMesh) {
		cerr << "Warning: findExternalDagPath: ";
		cerr << "|" << plug_outputSurface.info() << "|: ";
		cerr << "No connection to |inputMesh|";
		cerr << endl;
		return MStatus::kNotFound;
	}
	if (connectedPlugs_inputSurface.length() == 0) {
		cerr << "Warning: findExternalDagPath: ";
		cerr << "|" << plug_outputSurface.info() << "|: ";
		cerr << "No connection to |inputMesh| as Destination";
		return MStatus::kNotFound;
	}
	MPlug	plug = connectedPlugs_inputSurface[0];
	MString	partialName_useLongNames = plug.partialName(false, false, false, false, false, true, &ms);
	if (!ms) {
		cerr << "Error: findExternalDagPath: ";
		cerr << "|" << plug.info() << "|: ";
		cerr << "MPlug.partialName(false, false, false, false, false, true): Failure";
		cerr << ms << endl;
		return MStatus::kFailure;
	}
	if (partialName_useLongNames != "inputSurface") {
		cerr << "Warning: findExternalDagPath: ";
		cerr << "|" << plug.info() << "|: ";
		cerr << "Not |worldMesh|: |" << partialName_useLongNames << "|" << endl;
		return MStatus::kNotFound;
	}
	MObject	mesh = plug.node(&ms);
	if (!ms) {
		cerr << "Error: findExternalDagPath: ";
		cerr << "|" << plug.info() << "|: ";
		cerr << "MPlug.node(): Failure: ";
		cerr << ms << endl;
		return MStatus::kFailure;
	}
	MFnDagNode	dagNode(mesh, &ms);
	if (!ms) {
		cerr << "Error: findExternalDagPath: ";
		cerr << "|" << plug.info() << "|: ";
		cerr << "MFnDagNode::MFnDagNode(MObject): Failure: ";
		cerr << ms << endl;
		return MStatus::kFailure;
	}
	ms = dagNode.getPath(dagPathShape);
	if (!ms) {
		cerr << "Error: findExternalDagPath: ";
		cerr << "|" << plug.info() << "|: ";
		cerr << "MFnDagNode::getPath(MDagPath&): Failure: ";
		cerr << ms << endl;
		return MStatus::kFailure;
	}
	return MStatus::kSuccess;
}
namespace kUtil {
struct kTransformationSet {
#if 1
	bool	is_translate_changed;
	MVector		translate;
	bool	is_rotate_changed;
	MTransformationMatrix::RotationOrder	order;
	double		rotate[3];
	bool	is_scale_changed;
	double		scale[3];
	bool	is_shear_changed;
	double		shear[3];
	bool	is_rotatePivot_changed;
	MPoint		rotatePivot;
	bool	is_rotatePivotTranslate_changed;
	MVector		rotatePivotTranslate;
	bool	is_scalePivot_changed;
	MPoint		scalePivot;
	bool	is_scalePivotTranslate_changed;
	MVector		scalePivotTranslate;
#endif
	MTransformationMatrix	copy;
	kTransformationSet(const MTransformationMatrix& m) :
		is_translate_changed(false),
		is_rotate_changed(false),
		is_scale_changed(false),
		is_shear_changed(false),
		is_rotatePivot_changed(false),
		is_rotatePivotTranslate_changed(false),
		is_scalePivot_changed(false),
		is_scalePivotTranslate_changed(false)
	{
		copy = m;
		MStatus	ms;
#if 1
		translate = m.getTranslation(MSpace::kTransform, &ms);
		ms = m.getRotation(rotate, order);
		ms = m.getScale(scale, MSpace::kTransform);
		ms = m.getShear(shear, MSpace::kTransform);
		rotatePivot = m.rotatePivot(MSpace::kTransform, &ms);
		rotatePivotTranslate = m.rotatePivotTranslation(MSpace::kTransform, &ms);
		scalePivot = m.scalePivot(MSpace::kTransform, &ms);
		scalePivotTranslate = m.scalePivotTranslation(MSpace::kTransform, &ms);
#endif
	}
	MStatus	setTransformationMatrix(MMatrix& m)
	{
		MStatus	ms;
		if (is_translate_changed) {
			ms = copy.setTranslation(translate, MSpace::kTransform);
			if (!ms) {cerr << ms << endl; return ms;}
		}
		if (is_rotate_changed) {
			ms = copy.setRotation(rotate, order);
			if (!ms) {cerr << ms << endl; return ms;}
		}
		if (is_scale_changed) {
			ms = copy.setScale(scale, MSpace::kTransform);
			if (!ms) {cerr << ms << endl; return ms;}
		}
		if (is_shear_changed) {
			ms = copy.setShear(shear, MSpace::kTransform);
			if (!ms) {cerr << ms << endl; return ms;}
		}
		if (is_rotatePivot_changed) {
			ms = copy.setRotatePivot(rotatePivot, MSpace::kTransform, true);
			if (!ms) {cerr << ms << endl; return ms;}
		}
		if (is_rotatePivotTranslate_changed) {
			ms = copy.setRotatePivotTranslation(rotatePivotTranslate, MSpace::kTransform);
			if (!ms) {cerr << ms << endl; return ms;}
		}
		if (is_scalePivot_changed) {
			ms = copy.setScalePivot(scalePivot, MSpace::kTransform, true);
			if (!ms) {cerr << ms << endl; return ms;}
		}
		if (is_scalePivotTranslate_changed) {
			ms = copy.setScalePivotTranslation(scalePivotTranslate, MSpace::kTransform);
			if (!ms) {cerr << ms << endl; return ms;}
		}
		m = copy.asMatrix();
		return MStatus::kSuccess;
	}
};
MStatus
getWorldMatrixAt(
	const MDagPath&	dagPathShape,
	const MTime&	time,
	MMatrix&	evaluatedWorldMatrix,
	const MTransformationMatrix::RotationOrder*	rotationOrderPtr
)
{
	MStatus	ms;
#if 0
	MDagPath	dagPath;
	MSelectionList	selList;
	ms = selList.add(nodeName);
	cout << ms << endl;
	ms = selList.getDagPath(0, dagPath);
	cout << ms << endl;
#endif
	MObject	transform = dagPathShape.transform(&ms); if (!ms) cout << ms << endl;
	MObject&	dagPath = transform;
	MFnTransform	transFn(transform, &ms); if (!ms) {cerr << ms << endl; return ms;}
	MTransformationMatrix	transformMatrix = transFn.transformation(&ms); if (!ms) {cerr << ms << endl; return ms;}
#if 0
	cout << "MDagPath::inclusiveMatrix: " << dagPathShape.inclusiveMatrix() << endl;
	cout << "MDagPath::exclusiveMatrix: " << dagPathShape.exclusiveMatrix() << endl;
	cout << "MDagPath::matrix: " << dagPathShape.inclusiveMatrix(&ms) * dagPathShape.exclusiveMatrixInverse() << endl;
	cout << "transformMatrix: " << transformMatrix.asMatrix() << endl;
#endif
	MTransformationMatrix::RotationOrder	rotationOrder;
	if (rotationOrderPtr) {
		rotationOrder = *rotationOrderPtr;
	} else {
		rotationOrder = transformMatrix.rotationOrder(&ms); if (!ms) {cerr << ms << endl; return ms;}
#ifdef MFnAnimCurve_DEBUG
		cout << "RotationOrder:= " <<
			(rotationOrder == MTransformationMatrix::kInvalid ? "kInvalid" :
			(rotationOrder == MTransformationMatrix::kXYZ ? "kXYZ" :
			(rotationOrder == MTransformationMatrix::kYZX ? "kYZX" :
			(rotationOrder == MTransformationMatrix::kZXY ? "kZXY" :
			(rotationOrder == MTransformationMatrix::kXZY ? "kXZY" :
			(rotationOrder == MTransformationMatrix::kYXZ ? "kYXZ" :
			(rotationOrder == MTransformationMatrix::kZYX ? "kZYX" :
			(rotationOrder == MTransformationMatrix::kLast ? "kLast" : "kUnknown")))))))) << endl;
#endif
	}
	bool	isAnimated = MAnimUtil::isAnimated(dagPath, false, &ms); if (!ms) {cerr << ms << endl; return ms;}
#ifdef MFnAnimCurve_DEBUG
	cout << "|" << dagPathShape.fullPathName() << "|: isAnimated? " << (isAnimated ? "True" : "False") << endl;
#endif
	if (!isAnimated) {
		evaluatedWorldMatrix = transformMatrix.asMatrix();
		return MStatus::kSuccess;
	}
	MPlugArray	animatedPlugs;
	MAnimUtil::findAnimatedPlugs(dagPath, animatedPlugs, false, &ms); if (!ms) {cerr << ms << endl; return ms;}
	unsigned	numPlugs = animatedPlugs.length();
#ifdef MFnAnimCurve_DEBUG
	cout << "# of animated plug(s) = " << numPlugs << endl;
#endif
#if 1
	kTransformationSet	transformationSet(transformMatrix);
#else
	double	scale[3];
	MVector	translate;
	double	rotate[3];
#if 1
	ms = transformMatrix.getScale(scale, MSpace::kTransform); if (!ms) {cerr << ms << endl; return ms;}
	translate = transformMatrix.getTranslation(MSpace::kTransform, &ms); if (!ms) {cerr << ms << endl; return ms;}
	ms = transformMatrix.getRotation(rotate, rotationOrder); if (!ms) {cerr << ms << endl; return ms;}
#endif
#endif
	for (unsigned i = 0; i < numPlugs; ++i) {
		MPlug	plug = animatedPlugs[i];
		MString	info = plug.info(&ms); if (!ms) {cerr << ms << endl; return ms;}
#ifdef MFnAnimCurve_DEBUG
		cout << info << endl;
#endif
		MObjectArray	animation;
		bool	bFound = MAnimUtil::findAnimation(plug, animation, &ms); if (!ms) {cerr << ms << endl; return ms;}
		if (!bFound) {
#ifdef MFnAnimCurve_DEBUG
			cout << "No animCurve(s)" << endl;
#endif
			continue;
		}
		unsigned	numCurves = animation.length();
#ifdef MFnAnimCurve_DEBUG
		cout << "# of animCurve(s) = " << numCurves << endl;
#endif
		for (unsigned n = 0; n < numCurves; ++n) {
			MObject&	animCurveNode = animation[n];
#ifdef MFnAnimCurve_DEBUG
			cout << "apiTypeStr():= |" << animCurveNode.apiTypeStr() << "|" << endl;
#endif
			if (!animCurveNode.hasFn(MFn::kAnimCurve)) {
#ifdef MFnAnimCurve_DEBUG
				cout << "Has No MFn::kAnimCurve" << endl;
#endif
				continue;
			}
			MFnAnimCurve	animCurve(animCurveNode, &ms); if (!ms) {cerr << ms << endl; return ms;}
#ifdef MFnAnimCurve_DEBUG
			MFnAnimCurve::AnimCurveType	animCurveType = animCurve.animCurveType(&ms); if (!ms) {cerr << ms << endl; return ms;}
			cout << animCurveType << endl;
#endif
#ifdef MFnAnimCurve_DEBUG
			unsigned	numKeys = animCurve.numKeys(&ms); if (!ms) {cerr << ms << endl; return ms;}
			cout << "# of key(s) = " << numKeys << endl;
			cout << "Evalate... at t = " << time << endl;
#endif
			double	value;
			ms = animCurve.evaluate(time, value); if (!ms) {cerr << ms << endl; return ms;}
#ifdef MFnAnimCurve_DEBUG
			cout << "value = " << value << endl;
#endif
			MString	attrName = plug.partialName(false, false, false, false, false, true, &ms);
			if (!ms) {cerr << ms << endl; return ms;}
#ifdef MFnAnimCurve_DEBUG
			cout << "partialName w/ useLongNames:= |" << attrName << "|" << endl;
#endif
			if (ms) {
#if 1
				if (attrName == "translateX") {
					transformationSet.is_translate_changed = true;
					transformationSet.translate.x = value;
				} else if (attrName == "translateY") {
					transformationSet.is_translate_changed = true;
					transformationSet.translate.y = value;
				} else if (attrName == "translateZ") {
					transformationSet.is_translate_changed = true;
					transformationSet.translate.z = value;
				} else if (attrName == "rotateX") {
					transformationSet.is_rotate_changed = true;
					transformationSet.rotate[0] = value;
				} else if (attrName == "rotateY") {
					transformationSet.is_rotate_changed = true;
					transformationSet.rotate[1] = value;
				} else if (attrName == "rotateZ") {
					transformationSet.is_rotate_changed = true;
					transformationSet.rotate[2] = value;
				} else if (attrName == "scaleX") {
					transformationSet.is_scale_changed = true;
					transformationSet.scale[0] = value;
				} else if (attrName == "scaleY") {
					transformationSet.is_scale_changed = true;
					transformationSet.scale[1] = value;
				} else if (attrName == "scaleZ") {
					transformationSet.is_scale_changed = true;
					transformationSet.scale[2] = value;
				} else if (attrName == "shearXY") {
					transformationSet.is_shear_changed = true;
					transformationSet.shear[0] = value;
				} else if (attrName == "shearXZ") {
					transformationSet.is_shear_changed = true;
					transformationSet.shear[1] = value;
				} else if (attrName == "shearYZ") {
					transformationSet.is_shear_changed = true;
					transformationSet.shear[2] = value;
				} else if (attrName == "rotatePivotX") {
					transformationSet.is_rotatePivot_changed = true;
					transformationSet.rotatePivot.x = value;
				} else if (attrName == "rotatePivotY") {
					transformationSet.is_rotatePivot_changed = true;
					transformationSet.rotatePivot.y = value;
				} else if (attrName == "rotatePivotZ") {
					transformationSet.is_rotatePivot_changed = true;
					transformationSet.rotatePivot.z = value;
				} else if (attrName == "rotatePivotTranslateX") {
					transformationSet.is_rotatePivotTranslate_changed = true;
					transformationSet.rotatePivotTranslate.x = value;
				} else if (attrName == "rotatePivotTranslateY") {
					transformationSet.is_rotatePivotTranslate_changed = true;
					transformationSet.rotatePivotTranslate.y = value;
				} else if (attrName == "rotatePivotTranslateZ") {
					transformationSet.is_rotatePivotTranslate_changed = true;
					transformationSet.rotatePivotTranslate.z = value;
				} else if (attrName == "scalePivotX") {
					transformationSet.is_scalePivot_changed = true;
					transformationSet.scalePivot.x = value;
				} else if (attrName == "scalePivotY") {
					transformationSet.is_scalePivot_changed = true;
					transformationSet.scalePivot.y = value;
				} else if (attrName == "scalePivotZ") {
					transformationSet.is_scalePivot_changed = true;
					transformationSet.scalePivot.z = value;
				} else if (attrName == "scalePivotTranslateX") {
					transformationSet.is_scalePivotTranslate_changed = true;
					transformationSet.scalePivotTranslate.x = value;
				} else if (attrName == "scalePivotTranslateY") {
					transformationSet.is_scalePivotTranslate_changed = true;
					transformationSet.scalePivotTranslate.y = value;
				} else if (attrName == "scalePivotTranslateZ") {
					transformationSet.is_scalePivotTranslate_changed = true;
					transformationSet.scalePivotTranslate.z = value;
				} else if (attrName == "rotateAxisX" || attrName == "rotateAxisY" || attrName == "rotateAxisZ") {
					cerr << "rotateAxis animation NOT allowed" << endl;
					return MStatus::kFailure;
				}
#else
				if (attrName == "translateX")
					translate.x = value;
				else if (attrName == "translateY")
					translate.y = value;
				else if (attrName == "translateZ")
					translate.z = value;
				else if (attrName == "rotateX")
					rotate[0] = value;
				else if (attrName == "rotateY")
					rotate[1] = value;
				else if (attrName == "rotateZ")
					rotate[2] = value;
				else if (attrName == "scaleX")
					scale[0] = value;
				else if (attrName == "scaleY")
					scale[1] = value;
				else if (attrName == "scaleZ")
					scale[2] = value;
#endif
			}
#ifdef MFnAnimCurve_DEBUG
			for (unsigned k = 0; k < numKeys; ++k) {
				cout << "keys[" << k << "]:..." << endl;
				MTime	t = animCurve.time(k, &ms);
				cout << ms << endl;
				cout << t << endl;
				double	v = animCurve.value(k, &ms);
				cout << ms << endl;
				cout << v << endl;
				double	w = animCurve.unitlessInput(k, &ms);
				cout << ms << endl;
				cout << w << endl;
				MFnAnimCurve::TangentType	inTangentType = animCurve.inTangentType(k, &ms);
				cout << ms << endl;
				cout << inTangentType << endl;
				MFnAnimCurve::TangentType	outTangentType = animCurve.outTangentType(k, &ms);
				cout << ms << endl;
				cout << outTangentType << endl;
			}
#endif
		}
	}
#if 1
	return transformationSet.setTransformationMatrix(evaluatedWorldMatrix);
#else
if (isAnimated) {
#ifdef MFnAnimCurve_DEBUG
	cout << transformMatrix.asMatrix() << endl;
	cout << "Scale:= " << transformMatrix.asScaleMatrix() << endl;
	cout << "Rotate:= " << transformMatrix.asRotateMatrix() << endl;
	cout << "Translate:= " << transformMatrix.getTranslation(MSpace::kTransform, &ms) << endl; if (!ms) {cerr << ms << endl; return ms;}
#endif
	MTransformationMatrix	evaluatedTransformationMatrix = transformMatrix;
#ifdef MFnAnimCurve_DEBUG
	cout << evaluatedTransformationMatrix.asMatrix() << endl;
#endif
	ms = evaluatedTransformationMatrix.setScale(scale, MSpace::kTransform); if (!ms) {cerr << ms << endl; return ms;}
#ifdef MFnAnimCurve_DEBUG
	cout << "Scale:= " << evaluatedTransformationMatrix.asScaleMatrix() << endl;
#endif
	ms = evaluatedTransformationMatrix.setRotation(rotate, rotationOrder); if (!ms) {cerr << ms << endl; return ms;}
#ifdef MFnAnimCurve_DEBUG
	cout << "Rotate:= " << evaluatedTransformationMatrix.asRotateMatrix() << endl;
#endif
	ms = evaluatedTransformationMatrix.setTranslation(translate, MSpace::kTransform); if (!ms) {cerr << ms << endl; return ms;}
#ifdef MFnAnimCurve_DEBUG
	cout << "Translate:= " << evaluatedTransformationMatrix.getTranslation(MSpace::kTransform, &ms) << endl; if (!ms) {cerr << ms << endl; return ms;}
	cout << evaluatedTransformationMatrix.asMatrix() << endl;
#endif
	evaluatedWorldMatrix = evaluatedTransformationMatrix.asMatrix();
#if	defined(MFnAnimCurve_DEBUG)
	cout << "At t = " << time << endl;
	cout << "worldMatrix: " << transformMatrix.asMatrix() << endl;
	cout << "evaluated worldMatrix: " << evaluatedWorldMatrix << endl;
	cout << "\t==? " << (evaluatedTransformationMatrix == transformMatrix ? "True" : "False") << endl;
	cout << "\tisEquivalent? " << (evaluatedTransformationMatrix.isEquivalent(transformMatrix) ? "True" : "False") << endl;
#endif
}
	return MStatus::kSuccess;
#endif
}
#ifdef MFnAnimCurve_DEBUG
#undef MFnAnimCurve_DEBUG
#endif
#define MFnAnimCurve_DEBUG
}
#endif
