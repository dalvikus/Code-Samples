#include <assert.h>
#include <float.h>
#include <tentacleShape/tentacleShape.h>
#include <maya/MModelMessage.h>
#include <maya/MObjectArray.h>
#include <maya/MFnDoubleIndexedComponent.h>
#include <maya/MFnSingleIndexedComponent.h>
#include <maya/MGlobal.h>
#include <baseShape/kUtil.h>
#include <maya/MQuaternion.h>
#include <maya/MFnVectorArrayData.h>
#include <maya/MFnDoubleArrayData.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnCompoundAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MPlugArray.h>
#include <tentacleShape/kTentacle.h>
#if 1
#include <maya/MItMeshPolygon.h>
#include <maya/MItDependencyNodes.h>
#include <maya/MFnSkinCluster.h>
#include <maya/MDagPathArray.h>
#include <maya/MItGeometry.h>
#include <maya/MAnimUtil.h>
#include <maya/MFnPointArrayData.h>
#endif
#if 1
#include <maya/MFnMeshData.h>
#include <maya/MFnMesh.h>
#include <maya/MFloatPointArray.h>
#include <maya/MFloatArray.h>
#endif
#if 2
#define _INHERIT_kBODY
#ifdef _INHERIT_kBODY
#pragma warning(disable: 4250)
#endif
#include <simulation/kShSimulationMayaTentacle.h>
#ifdef _INHERIT_kBODY
#pragma warning(default: 4250)
#else
#include <simulation/kBodyMayaTentacle.h>
#endif
#include <maya/MAnimControl.h>
#endif
#if 1
const int	tentacleShape::numEllipseSamplePoints = 50;
MObject	tentacleShape::outputMesh;
#endif
#if 1
#if 1
MObject		tentacleShape::rampDisplay;
MObject		tentacleShape::__rampWakes;
MObject		tentacleShape::__tentacleParameter;
#if 0
MObject		tentacleShape::transversal;
#endif
#if 1
#if 1
MObject		tentacleShape::__drawTentacleRoot;
MObject		tentacleShape::__tentacleRoot;
#endif
#if 0
MObject		tentacleShape::__tentacleRemovedIndex;
#endif
MObject		tentacleShape::__tentacleDoAction;
MObject		tentacleShape::__tentacleArray;
#if 2
MObject		tentacleShape::__tentacleArrayArray;
MObject		tentacleShape::__frameIndex;
#endif
#if 1
int	tentacleShape::lastSelectedPhysicalIndex = -1;
#endif
#endif
#endif
#endif
#if 1
#if 1
#if 1
const MString	tentacleShape::TentacleArrayUpdated("hello, world: Tentacle Array Updated");
static void	__checkStatus(const MStatus& ms, const char* from = 0);
static int	compare(const void* arg1, const void* arg2);
MStatus
SetTentacleParameterValue(
	const MString*	strIndent0Ptr,
	const MPlug&	plug,
	MPlugArray&	hierarchy,
	bool*	isTentacleParameterPtr
);
#if 0
static void	listRamp(
	MPlug	plug_ramp,
	MDataHandle	handle_ramp
);
static void	copyRamp(
	MPlug	plug_ramp0,
	MDataHandle	handle_ramp0,
	MPlug	plug_ramp,
	MDataHandle	handle_ramp,
	MDataBlock	datablock,
	bool	directCopy
);
#endif
#endif
#endif
#endif
#if 1
void*	tentacleShape::creator()
{
	return new tentacleShape();
}
MStatus	tentacleShape::initialize()
{
	MStatus	ms;
#if 1
#if 1
	MFnEnumAttribute	rampWakes_attr;
	__rampWakes = rampWakes_attr.create("rampWakes", "rw", RampWakes::None, &ms);
	ms = rampWakes_attr.addField("None", RampWakes::None);
	ms = rampWakes_attr.addField("MajorRadius", RampWakes::MajorRadius);
	ms = rampWakes_attr.addField("Eccentricity", RampWakes::Eccentricity);
	ms = rampWakes_attr.addField("Rotation", RampWakes::Rotation);
	ms = addAttribute(__rampWakes);
	MFnEnumAttribute	rampDisplay_attr;
	rampDisplay = rampDisplay_attr.create("rampDisplay", "rd", RampDisplay::None, &ms);
	ms = rampDisplay_attr.addField("None", RampDisplay::None);
	ms = rampDisplay_attr.addField("View Port", RampDisplay::ViewPort);
	ms = rampDisplay_attr.addField("Camera Front", RampDisplay::CameraFront);
	ms = addAttribute(rampDisplay);
#if 1
	MObjectArray	tentacleParameter_children(NUM_TENTACLE_PARAM_CHILDREN);
#if 1
	MObjectArray	tentacleLength_children(NUM_TENTACLE_LENGTH_ATTRIBUTE_EDITOR_CHILDREN);
	MFnNumericAttribute	tentacleLength_sync_attr;
	tentacleLength_children[TENTACLE_ATTRIBUTE_EDITOR_SYNC] = tentacleLength_sync_attr.create("tentacleLength_sync", "tLEN_sync", MFnNumericData::kBoolean, 1, &ms);
	MFnNumericAttribute	tentacleLength_val_attr;
	tentacleLength_children[TENTACLE_ATTRIBUTE_EDITOR_VAL] = tentacleLength_val_attr.create("tentacleLength_val", "tLEN_val", MFnNumericData::kDouble, 10, &ms);
	MFnCompoundAttribute	tentacleLength_attr;
	tentacleParameter_children[TENTACLE_PARAM_LENGTH] = tentacleLength_attr.create("tentacleLength", "tLEN", &ms);
	for (int i = 0; i < NUM_TENTACLE_LENGTH_ATTRIBUTE_EDITOR_CHILDREN; ++i)
		ms = tentacleLength_attr.addChild(tentacleLength_children[i]);
#endif
#if 1
	MObjectArray	tentacleCrossSectionMajorRadius_children(NUM_TENTACLE_ATTRIBUTE_EDITOR_CHILDREN);
	MFnNumericAttribute	tentacleCrossSectionMajorRadius_val_attr;
	tentacleCrossSectionMajorRadius_children[TENTACLE_ATTRIBUTE_EDITOR_VAL] = tentacleCrossSectionMajorRadius_val_attr.create("tentacleCrossSectionMajorRadius_val", "tCSMR_val", MFnNumericData::kDouble, 1, &ms);
	MObjectArray	tentacleCrossSectionMajorRadius_scale_children(NUM_RAMP_CHILDREN);
	MFnNumericAttribute	tentacleCrossSectionMajorRadius_scale_Position_attr;
	tentacleCrossSectionMajorRadius_scale_children[RAMP_POSITION] = tentacleCrossSectionMajorRadius_scale_Position_attr.create("tentacleCrossSectionMajorRadius_scale_Position", "tCSMR_scale_Position", MFnNumericData::kFloat, -1, &ms);
	MFnNumericAttribute	tentacleCrossSectionMajorRadius_scale_FloatValue_attr;
	tentacleCrossSectionMajorRadius_scale_children[RAMP_FLOAT_VALUE] = tentacleCrossSectionMajorRadius_scale_FloatValue_attr.create("tentacleCrossSectionMajorRadius_scale_FloatValue", "tCSMR_scale_FloatValue", MFnNumericData::kFloat, -1, &ms);
	MFnEnumAttribute	tentacleCrossSectionMajorRadius_scale_Interp_attr;
	tentacleCrossSectionMajorRadius_scale_children[RAMP_INTERP_TYPE] = tentacleCrossSectionMajorRadius_scale_Interp_attr.create("tentacleCrossSectionMajorRadius_scale_Interp", "tCSMR_Interp", INTERP::Invalid, &ms);
	ms = tentacleCrossSectionMajorRadius_scale_Interp_attr.addField("None", INTERP::None);
	ms = tentacleCrossSectionMajorRadius_scale_Interp_attr.addField("Linear", INTERP::Linear);
	ms = tentacleCrossSectionMajorRadius_scale_Interp_attr.addField("Smooth", INTERP::Smooth);
	ms = tentacleCrossSectionMajorRadius_scale_Interp_attr.addField("Spline", INTERP::Spline);
	MFnCompoundAttribute	tentacleCrossSectionMajorRadius_scale_attr;
	tentacleCrossSectionMajorRadius_children[TENTACLE_ATTRIBUTE_EDITOR_RAMP] = tentacleCrossSectionMajorRadius_scale_attr.create("tentacleCrossSectionMajorRadius_scale", "tCSMR_scale", &ms);
	for (int i = 0; i < NUM_RAMP_CHILDREN; ++i)
		ms = tentacleCrossSectionMajorRadius_scale_attr.addChild(tentacleCrossSectionMajorRadius_scale_children[i]);
	ms = tentacleCrossSectionMajorRadius_scale_attr.setUsesArrayDataBuilder(true);
	ms = tentacleCrossSectionMajorRadius_scale_attr.setArray(true);
	MFnNumericAttribute	tentacleCrossSectionMajorRadius_sync_attr;
	tentacleCrossSectionMajorRadius_children[TENTACLE_ATTRIBUTE_EDITOR_SYNC] = tentacleCrossSectionMajorRadius_sync_attr.create("tentacleCrossSectionMajorRadius_sync", "tCSMR_sync", MFnNumericData::kBoolean, 1, &ms);
#if 0
	MFnNumericAttribute	tentacleCrossSectionMajorRadius_scale_wakes_attr;
	tentacleCrossSectionMajorRadius_children[TENTACLE_ATTRIBUTE_EDITOR_WAKES] = tentacleCrossSectionMajorRadius_scale_wakes_attr.create("tentacleCrossSectionMajorRadius_scale_wakes", "tCSMR_scale_wakes", MFnNumericData::kBoolean, 0., &ms);
#endif
	MFnCompoundAttribute	tentacleCrossSectionMajorRadius_attr;
	tentacleParameter_children[TENTACLE_PARAM_CROSS_SECTION_MAJOR_RADIUS] = tentacleCrossSectionMajorRadius_attr.create("tentacleCrossSectionMajorRadius", "tCSMR", &ms);
	for (int i = 0; i < NUM_TENTACLE_ATTRIBUTE_EDITOR_CHILDREN; ++i)
		ms = tentacleCrossSectionMajorRadius_attr.addChild(tentacleCrossSectionMajorRadius_children[i]);
#endif
#if 1
	MObjectArray	tentacleCrossSectionEccentricity_children(NUM_TENTACLE_ATTRIBUTE_EDITOR_CHILDREN);
	MFnNumericAttribute	tentacleCrossSectionEccentricity_val_attr;
	tentacleCrossSectionEccentricity_children[TENTACLE_ATTRIBUTE_EDITOR_VAL] = tentacleCrossSectionEccentricity_val_attr.create("tentacleCrossSectionEccentricity_val", "tCSE_val", MFnNumericData::kDouble, 0, &ms);
	MObjectArray	tentacleCrossSectionEccentricity_scale_children(NUM_RAMP_CHILDREN);
	MFnNumericAttribute	tentacleCrossSectionEccentricity_scale_Positioin_attr;
	tentacleCrossSectionEccentricity_scale_children[RAMP_POSITION] = tentacleCrossSectionEccentricity_scale_Positioin_attr.create("tentacleCrossSectionEccentricity_scale_Position", "tCSE_scale_Position", MFnNumericData::kFloat, -1, &ms);
	MFnNumericAttribute	tentacleCrossSectionEccentricity_scale_FloatValue_attr;
	tentacleCrossSectionEccentricity_scale_children[RAMP_FLOAT_VALUE] = tentacleCrossSectionEccentricity_scale_FloatValue_attr.create("tentacleCrossSectionEccentricity_scale_FloatValue", "tCSE_scale_FloatValue", MFnNumericData::kFloat, -1, &ms);
	MFnEnumAttribute	tentacleCrossSectionEccentricity_scale_Interp_attr;
	tentacleCrossSectionEccentricity_scale_children[RAMP_INTERP_TYPE] = tentacleCrossSectionEccentricity_scale_Interp_attr.create("tentacleCrossSectionEccentricity_scale_Interp", "tCSES_Interp", INTERP::Invalid, &ms);
	ms = tentacleCrossSectionEccentricity_scale_Interp_attr.addField("None", INTERP::None);
	ms = tentacleCrossSectionEccentricity_scale_Interp_attr.addField("Linear", INTERP::Linear);
	ms = tentacleCrossSectionEccentricity_scale_Interp_attr.addField("Smooth", INTERP::Smooth);
	ms = tentacleCrossSectionEccentricity_scale_Interp_attr.addField("Spline", INTERP::Spline);
	MFnCompoundAttribute	tentacleCrossSectionEccentricity_scale_attr;
	tentacleCrossSectionEccentricity_children[TENTACLE_ATTRIBUTE_EDITOR_RAMP] = tentacleCrossSectionEccentricity_scale_attr.create("tentacleCrossSectionEccentricity_scale", "tCSE_scale", &ms);
	for (int i = 0; i < NUM_RAMP_CHILDREN; ++i)
		ms = tentacleCrossSectionEccentricity_scale_attr.addChild(tentacleCrossSectionEccentricity_scale_children[i]);
	ms = tentacleCrossSectionEccentricity_scale_attr.setUsesArrayDataBuilder(true);
	ms = tentacleCrossSectionEccentricity_scale_attr.setArray(true);
	MFnNumericAttribute	tentacleCrossSectionEccentricity_sync_attr;
	tentacleCrossSectionEccentricity_children[TENTACLE_ATTRIBUTE_EDITOR_SYNC] = tentacleCrossSectionEccentricity_sync_attr.create("tentacleCrossSectionEccentricity_sync", "tCSE_sync", MFnNumericData::kBoolean, 1, &ms);
#if 0
	MFnNumericAttribute	tentacleCrossSectionEccentricity_scale_wakes_attr;
	tentacleCrossSectionEccentricity_children[TENTACLE_ATTRIBUTE_EDITOR_WAKES] = tentacleCrossSectionEccentricity_scale_wakes_attr.create("tentacleCrossSectionEccentricity_scale_wakes", "tCSE_scale_wakes", MFnNumericData::kBoolean, 0., &ms);
#endif
	MFnCompoundAttribute	tentacleCrossSectionEccentricity_attr;
	tentacleParameter_children[TENTACLE_PARAM_CROSS_SECTION_ECCENTRICITY] = tentacleCrossSectionEccentricity_attr.create("tentacleCrossSectionEccentricity", "tCSE", &ms);
	for (int i = 0; i < NUM_TENTACLE_ATTRIBUTE_EDITOR_CHILDREN; ++i)
		ms = tentacleCrossSectionEccentricity_attr.addChild(tentacleCrossSectionEccentricity_children[i]);
#endif
#if 1
	MObjectArray	tentacleCrossSectionRotation_children(NUM_TENTACLE_ATTRIBUTE_EDITOR_CHILDREN);
	MFnNumericAttribute	tentacleCrossSectionRotation_val_attr;
	tentacleCrossSectionRotation_children[TENTACLE_ATTRIBUTE_EDITOR_VAL] = tentacleCrossSectionRotation_val_attr.create("tentacleCrossSectionRotation_val", "tCSR_val", MFnNumericData::kDouble, 0, &ms);
	MObjectArray	tentacleCrossSectionRotation_local_children(NUM_RAMP_CHILDREN);
	MFnNumericAttribute	tentacleCrossSectionRotation_local_Position_attr;
	tentacleCrossSectionRotation_local_children[RAMP_POSITION] = tentacleCrossSectionRotation_local_Position_attr.create("tentacleCrossSectionRotation_local_Position", "tCSR_local_Position", MFnNumericData::kFloat, -1, &ms);
	MFnNumericAttribute	tentacleCrossSectionRotation_local_FloatValue_attr;
	tentacleCrossSectionRotation_local_children[RAMP_FLOAT_VALUE] = tentacleCrossSectionRotation_local_FloatValue_attr.create("tentacleCrossSectionRotation_local_FloatValue", "tCSR_local_FloatValue", MFnNumericData::kFloat, -1, &ms);
	MFnEnumAttribute	tentacleCrossSectionRotation_local_Interp_attr;
	tentacleCrossSectionRotation_local_children[RAMP_INTERP_TYPE] = tentacleCrossSectionRotation_local_Interp_attr.create("tentacleCrossSectionRotation_local_Interp", "tCSR_local_Interp", INTERP::Invalid, &ms);
	ms = tentacleCrossSectionRotation_local_Interp_attr.addField("None", INTERP::None);
	ms = tentacleCrossSectionRotation_local_Interp_attr.addField("Linear", INTERP::Linear);
	ms = tentacleCrossSectionRotation_local_Interp_attr.addField("Smooth", INTERP::Smooth);
	ms = tentacleCrossSectionRotation_local_Interp_attr.addField("Spline", INTERP::Spline);
	MFnCompoundAttribute	tentacleCrossSectionRotation_local_attr;
	tentacleCrossSectionRotation_children[TENTACLE_ATTRIBUTE_EDITOR_RAMP] = tentacleCrossSectionRotation_local_attr.create("tentacleCrossSectionRotation_local", "tCSR_local", &ms);
	for (int i = 0; i < NUM_RAMP_CHILDREN; ++i)
		ms = tentacleCrossSectionRotation_local_attr.addChild(tentacleCrossSectionRotation_local_children[i]);
	ms = tentacleCrossSectionRotation_local_attr.setUsesArrayDataBuilder(true);
	ms = tentacleCrossSectionRotation_local_attr.setArray(true);
	MFnNumericAttribute	tentacleCrossSectionRotation_sync_attr;
	tentacleCrossSectionRotation_children[TENTACLE_ATTRIBUTE_EDITOR_SYNC] = tentacleCrossSectionRotation_sync_attr.create("tentacleCrossSectionRotation_sync", "tCSR_sync", MFnNumericData::kBoolean, 1, &ms);
#if 0
	MFnNumericAttribute	tentacleCrossSectionRotation_local_wakes_attr;
	tentacleCrossSectionRotation_children[TENTACLE_ATTRIBUTE_EDITOR_WAKES] = tentacleCrossSectionRotation_local_wakes_attr.create("tentacleCrossSectionRotation_local_wakes", "tCSR_local_wakes", MFnNumericData::kBoolean, 0., &ms);
#endif
	MFnCompoundAttribute	tentacleCrossSectionRotation_attr;
	tentacleParameter_children[TENTACLE_PARAM_CROSS_SECTION_ROTATION] = tentacleCrossSectionRotation_attr.create("tentacleCrossSectionRotation", "tCSR", &ms);
	for (int i = 0; i < NUM_TENTACLE_ATTRIBUTE_EDITOR_CHILDREN; ++i)
		ms = tentacleCrossSectionRotation_attr.addChild(tentacleCrossSectionRotation_children[i]);
#endif
#if 1
	MObjectArray	tentacleCut_val_children(NUM_TENTACLE_CUT_VAL_CHILDREN);
#if 1
	MFnNumericAttribute	tentacleCut_frame_attr;
	tentacleCut_val_children[TENTACLE_CUT_VAL_FRAME] = tentacleCut_frame_attr.create("tentacleCut_frame", "tCUT_frame", MFnNumericData::kInt, 0, &ms);
#else
	MFnNumericAttribute	tentacleCut_have_attr;
	tentacleCut_val_children[TENTACLE_CUT_VAL_HAVE] = tentacleCut_have_attr.create("tentacleCut_have", "tCUT_have", MFnNumericData::kBoolean, 0, &ms);
#endif
	MFnNumericAttribute	tentacleCut_t_attr;
	tentacleCut_val_children[TENTACLE_CUT_VAL_T] = tentacleCut_t_attr.create("tentacleCut_t", "tCUT_t", MFnNumericData::kDouble, 0.5, &ms);
	MFnNumericAttribute	tentacleCut_polar_attr;
	tentacleCut_val_children[TENTACLE_CUT_VAL_POLAR] = tentacleCut_t_attr.create("tentacleCut_polar", "tCUT_polar", MFnNumericData::kDouble, 0, &ms);
	MFnNumericAttribute	tentacleCut_azimuthal_attr;
	tentacleCut_val_children[TENTACLE_CUT_VAL_AZIMUTHAL] = tentacleCut_azimuthal_attr.create("tentacleCut_azimuthal", "tCUT_azimuthal", MFnNumericData::kDouble, 0, &ms);
	MObjectArray	tentacleCut_children(NUM_TENTACLE_CUT_ATTRIBUTE_EDITOR_CHILDREN);
	MFnNumericAttribute	tentacleCut_sync_attr;
	tentacleCut_children[TENTACLE_ATTRIBUTE_EDITOR_SYNC] = tentacleCut_sync_attr.create("tentacleCut_sync", "tCUT_sync", MFnNumericData::kBoolean, 1, &ms);
	MFnCompoundAttribute	tentacleCut_val_attr;
	tentacleCut_children[TENTACLE_ATTRIBUTE_EDITOR_VAL] = tentacleCut_val_attr.create("tentacleCut_val", "tCUT_val", &ms);
	for (int i = 0; i < NUM_TENTACLE_CUT_VAL_CHILDREN; ++i)
		ms = tentacleCut_val_attr.addChild(tentacleCut_val_children[i]);
	MFnCompoundAttribute	tentacleCut_attr;
	tentacleParameter_children[TENTACLE_PARAM_CUT] = tentacleCut_attr.create("tentacleCut", "tCUT", &ms);
	for (int i = 0; i < NUM_TENTACLE_CUT_ATTRIBUTE_EDITOR_CHILDREN; ++i)
		ms = tentacleCut_attr.addChild(tentacleCut_children[i]);
#endif
	MFnCompoundAttribute	tentacleParameter_attr;
	__tentacleParameter = tentacleParameter_attr.create("tentacleParameter", "tParam", &ms);
	for (int i = 0; i < NUM_TENTACLE_PARAM_CHILDREN; ++i)
		ms = tentacleParameter_attr.addChild(tentacleParameter_children[i]);
	ms = addAttribute(__tentacleParameter);
#endif
#if 0
	ms = addAttribute(transversal);
	MFnNumericAttribute	transversalAttr;
	MFnNumericAttribute	transversalXAttr;
	MObject	transversalX = transversalXAttr.create("transversalX", "transX", MFnNumericData::kDouble, 0, &ms);
	MFnNumericAttribute	transversalYAttr;
	MObject	transversalY = transversalYAttr.create("transversalY", "transY", MFnNumericData::kDouble, 0, &ms);
	MFnNumericAttribute	transversalZAttr;
	MObject	transversalZ = transversalZAttr.create("transversalZ", "transZ", MFnNumericData::kDouble, 0, &ms);
	transversal = transversalAttr.create("transversal", "trans", transversalX, transversalY, transversalZ, &ms);
	ms = transversalAttr.setInternal(true);
	ms = transversalAttr.setArray(true);
	ms = addAttribute(transversal);
#endif
#if 1
#if 1
#if 1
	MObjectArray	tentacleRoot_children(NUM_TENTACLE_ROOT_CHILDREN);
	MFnNumericAttribute	tentaclePolyIdAttr;
	tentacleRoot_children[TENTACLE_ROOT_POLY_ID] = tentaclePolyIdAttr.create("tentacleRoot_polyId", "tR_ID", MFnNumericData::kInt, -1, &ms);
	MFnNumericAttribute	tentaclePositionAttr;
	tentacleRoot_children[TENTACLE_ROOT_POSITION] = tentaclePositionAttr.create("tentacleRoot_position", "tR_P", MFnNumericData::k3Double, 0, &ms);
	MFnNumericAttribute	tentacleNormalAttr;
	tentacleRoot_children[TENTACLE_ROOT_NORMAL] = tentacleNormalAttr.create("tentacleRoot_normal", "tR_N", MFnNumericData::k3Double, 0, &ms);
#if 1
#if 1
	MFnNumericAttribute	tentacleHitVertexListAttr;
	tentacleRoot_children[TENTACLE_ROOT_HIT_VERTEX_LIST] = tentacleHitVertexListAttr.create("tentacleRoot_hitVertexList", "tR_HIT_VLIST", MFnNumericData::k3Int, -1, &ms);
#else
	MFnNumericAttribute	tentacleHitTriangleIdAttr;
	tentacleRoot_children[TENTACLE_ROOT_HIT_TRIANGLE_ID] = tentacleHitTriangleIdAttr.create("tentacleRoot_hitTriangleId", "tR_HIT_TRI_ID", MFnNumericData::kInt, -1, &ms);
#endif
	MFnNumericAttribute	tentacleHitBary1Attr;
	tentacleRoot_children[TENTACLE_ROOT_HIT_BARY1] = tentacleHitBary1Attr.create("tentacleRoot_hitBary1", "tR_HIT_BARY1", MFnNumericData::kFloat, 0, &ms);
	MFnNumericAttribute	tentacleHitBary2Attr;
	tentacleRoot_children[TENTACLE_ROOT_HIT_BARY2] = tentacleHitBary2Attr.create("tentacleRoot_hitBary2", "tR_HIT_BARY2", MFnNumericData::kFloat, 0, &ms);
#endif
	MFnCompoundAttribute	tentacleRootAttr;
	__tentacleRoot = tentacleRootAttr.create("tentacleRoot", "tR", &ms);
	for (int i = 0; i < NUM_TENTACLE_ROOT_CHILDREN; ++i)
		ms = tentacleRootAttr.addChild(tentacleRoot_children[i]);
	ms = addAttribute(__tentacleRoot);
#endif
	MFnNumericAttribute	drawTentacleRootPointAttr;
	__drawTentacleRoot = drawTentacleRootPointAttr.create("drawTentacleRoot", "drawTR", MFnNumericData::kBoolean, 0, &ms);
	ms = addAttribute(__drawTentacleRoot);
#endif
#if 0
	MFnNumericAttribute	tentacleRemovedIndexAttr;
	__tentacleRemovedIndex = tentacleRemovedIndexAttr.create("tentacleRemovedIndex", "tRI", MFnNumericData::kInt, -1, &ms);
	ms = addAttribute(__tentacleRemovedIndex);
#endif
#if 1
	MObjectArray	tentacleArray_children(NUM_TENTACLE_ARRAY_CHILDREN);
#if 1
{
	MObjectArray	tentacleRoot_children(NUM_TENTACLE_ROOT_CHILDREN);
	MFnNumericAttribute	tentaclePolyIdAttr;
	tentacleRoot_children[TENTACLE_ROOT_POLY_ID] = tentaclePolyIdAttr.create("tentacleArray_root_polyId", "tA_r_ID", MFnNumericData::kInt, -1, &ms);
	MFnNumericAttribute	tentaclePositionAttr;
	tentacleRoot_children[TENTACLE_ROOT_POSITION] = tentaclePositionAttr.create("tentacleArray_root_position", "tA_r_P", MFnNumericData::k3Double, 0, &ms);
	MFnNumericAttribute	tentacleNormalAttr;
	tentacleRoot_children[TENTACLE_ROOT_NORMAL] = tentacleNormalAttr.create("tentacleArray_root_normal", "tA_r_N", MFnNumericData::k3Double, 0, &ms);
#if 1
#if 1
	MFnNumericAttribute	tentacleHitVertexListAttr;
	tentacleRoot_children[TENTACLE_ROOT_HIT_VERTEX_LIST] = tentacleHitVertexListAttr.create("tentacleArray_root_hitVertexList", "tA_r_HIT_VLIST", MFnNumericData::k3Int, -1, &ms);
#else
	MFnNumericAttribute	tentacleHitTriangleIdAttr;
	tentacleRoot_children[TENTACLE_ROOT_HIT_TRIANGLE_ID] = tentacleHitTriangleIdAttr.create("tentacleArray_root_hitTriangleId", "tA_r_HIT_TRI_ID", MFnNumericData::kInt, -1, &ms);
#endif
	MFnNumericAttribute	tentacleHitBary1Attr;
	tentacleRoot_children[TENTACLE_ROOT_HIT_BARY1] = tentacleHitBary1Attr.create("tentacleArray_root_hitBary1", "tA_r_HIT_BARY1", MFnNumericData::kFloat, 0, &ms);
	MFnNumericAttribute	tentacleHitBary2Attr;
	tentacleRoot_children[TENTACLE_ROOT_HIT_BARY2] = tentacleHitBary2Attr.create("tentacleArray_root_hitBary2", "tA_r_HIT_BARY2", MFnNumericData::kFloat, 0, &ms);
#endif
	MFnCompoundAttribute	tentacleRootAttr;
	tentacleArray_children[TENTACLE_ARRAY_ROOT] = tentacleRootAttr.create("tentacleArray_root", "tA_r", &ms);
	for (int i = 0; i < NUM_TENTACLE_ROOT_CHILDREN; ++i)
		ms = tentacleRootAttr.addChild(tentacleRoot_children[i]);
}
#else
	MFnNumericAttribute	tentacle_array_root_attr;
	tentacleArray_children[TENTACLE_ARRAY_ROOT] = tentacle_array_root_attr.create("tentacleArray_root", "tA_r", MFnNumericData::k3Double, 0, &ms);
#endif
#if 1
#if 1
	MFnTypedAttribute	tentacle_array_quaternion_attr;
	MPointArray		default_quaternion_array;
	MFnPointArrayData	tentacle_array_quaternion_data;
	tentacleArray_children[TENTACLE_ARRAY_QUATERNION] = tentacle_array_quaternion_attr.create(
		"tentacleArray_quaternion", "tA_q", MFnData::kPointArray,
		tentacle_array_quaternion_data.create(default_quaternion_array, &ms),
		&ms
	);
#else
	MFnMatrixAttribute	quaternion_attr;
{
	tentacleArray_children[TENTACLE_ARRAY_QUATERNION] = quaternion_attr.create("tentacleArray_quaternion", "tA_q", MFnMatrixAttribute::kDouble, &ms);
	MMatrix	m;
	ms = quaternion_attr.setDefault(m);
	ms = quaternion_attr.setArray(true);
}
#endif
#endif
	MFnTypedAttribute	tentacle_array_longitudinal_attr;
	MVectorArray	default_longitudinal_array;
	MFnVectorArrayData	tentacle_array_longitudinal_data;
	tentacleArray_children[TENTACLE_ARRAY_LOGITUDINAL] = tentacle_array_longitudinal_attr.create(
		"tentacleArray_longitudinal", "tA_l", MFnData::kVectorArray,
		tentacle_array_longitudinal_data.create(default_longitudinal_array, &ms),
		&ms
	);
	MFnTypedAttribute	tentacle_array_transversal_attr;
	MVectorArray	default_transversal_array;
	MFnVectorArrayData	tentacle_array_transversal_data;
	tentacleArray_children[TENTACLE_ARRAY_TRANSVERSAL] = tentacle_array_transversal_attr.create(
		"tentacleArray_transversal", "tA_t", MFnData::kVectorArray,
		tentacle_array_transversal_data.create(default_transversal_array, &ms),
		&ms
	);
	MFnNumericAttribute	tentacle_array_a_attr;
	tentacleArray_children[TENTACLE_ARRAY_MAJOR_RADIUS] = tentacle_array_a_attr.create("tentacleArray_a", "tA_a", MFnNumericData::kDouble, 1, &ms);
	MFnTypedAttribute	tentacle_array_scale_a_attr;
	MDoubleArray	default_scale_a_array;
	MFnDoubleArrayData	tentacle_array_scale_a_data;
	tentacleArray_children[TENTACLE_ARRAY_MAJOR_RADIUS_SCALE] = tentacle_array_scale_a_attr.create(
		"tentacleArray_scale_a", "tA_scale_a", MFnData::kDoubleArray,
		tentacle_array_scale_a_data.create(default_scale_a_array, &ms),
		&ms
	);
{
	MObjectArray	ramp_children(NUM_RAMP_CHILDREN);
	MFnNumericAttribute	Postion_attr;
	ramp_children[RAMP_POSITION] = Postion_attr.create("tentacleArray_ramp_a_Position", "tA_ramp_a_Position", MFnNumericData::kFloat, -1, &ms);
	MFnNumericAttribute	FloatValue_attr;
	ramp_children[RAMP_FLOAT_VALUE] = FloatValue_attr.create("tentacleArray_ramp_a_FloatValue", "tA_ramp_a_FloatValue", MFnNumericData::kFloat, -1, &ms);
	MFnEnumAttribute	Interp_attr;
	ramp_children[RAMP_INTERP_TYPE] = Interp_attr.create("tentacleArray_ramp_a_Interp", "tA_ramp_a_Interp", INTERP::Invalid, &ms);
	ms = Interp_attr.addField("None", INTERP::None);
	ms = Interp_attr.addField("Linear", INTERP::Linear);
	ms = Interp_attr.addField("Smooth", INTERP::Smooth);
	ms = Interp_attr.addField("Spline", INTERP::Spline);
	MFnCompoundAttribute	ramp_attr;
	tentacleArray_children[TENTACLE_ARRAY_MAJOR_RADIUS_RAMP] = ramp_attr.create("tentacleArray_ramp_a", "tA_ramp_a", &ms);
	for (int i = 0; i < NUM_RAMP_CHILDREN; ++i)
		ms = ramp_attr.addChild(ramp_children[i]);
	ms = ramp_attr.setUsesArrayDataBuilder(true);
	ms = ramp_attr.setArray(true);
}
	MFnNumericAttribute	tentacle_array_e_attr;
	tentacleArray_children[TENTACLE_ARRAY_ECCENTRICITY] = tentacle_array_e_attr.create("tentacleArray_e", "tA_e", MFnNumericData::kDouble, 0, &ms);
	MFnTypedAttribute	tentacle_array_scale_e_attr;
	MDoubleArray	default_scale_e_array;
	MFnDoubleArrayData	tentacle_array_scale_e_data;
	tentacleArray_children[TENTACLE_ARRAY_ECCENTRICITY_SCALE] = tentacle_array_scale_e_attr.create(
		"tentacleArray_scale_e", "tA_scale_e", MFnData::kDoubleArray,
		tentacle_array_scale_e_data.create(default_scale_e_array, &ms),
		&ms
	);
{
	MObjectArray	ramp_children(NUM_RAMP_CHILDREN);
	MFnNumericAttribute	Postion_attr;
	ramp_children[RAMP_POSITION] = Postion_attr.create("tentacleArray_ramp_e_Position", "tA_ramp_e_Position", MFnNumericData::kFloat, -1, &ms);
	MFnNumericAttribute	FloatValue_attr;
	ramp_children[RAMP_FLOAT_VALUE] = FloatValue_attr.create("tentacleArray_ramp_e_FloatValue", "tA_ramp_e_FloatValue", MFnNumericData::kFloat, -1, &ms);
	MFnEnumAttribute	Interp_attr;
	ramp_children[RAMP_INTERP_TYPE] = Interp_attr.create("tentacleArray_ramp_e_Interp", "tA_ramp_e_Interp", INTERP::Invalid, &ms);
	ms = Interp_attr.addField("None", INTERP::None);
	ms = Interp_attr.addField("Linear", INTERP::Linear);
	ms = Interp_attr.addField("Smooth", INTERP::Smooth);
	ms = Interp_attr.addField("Spline", INTERP::Spline);
	MFnCompoundAttribute	ramp_attr;
	tentacleArray_children[TENTACLE_ARRAY_ECCENTRICITY_RAMP] = ramp_attr.create("tentacleArray_ramp_e", "tA_ramp_e", &ms);
	for (int i = 0; i < NUM_RAMP_CHILDREN; ++i)
		ms = ramp_attr.addChild(ramp_children[i]);
	ms = ramp_attr.setUsesArrayDataBuilder(true);
	ms = ramp_attr.setArray(true);
}
	MFnNumericAttribute	tentacle_array_rotation_attr;
	tentacleArray_children[TENTACLE_ARRAY_ROTATION] = tentacle_array_rotation_attr.create("tentacleArray_rotation", "tA_rotation", MFnNumericData::kDouble, 0, &ms);
	MFnTypedAttribute	tentacle_array_local_rotation_attr;
	MDoubleArray	default_local_rotation_array;
	MFnDoubleArrayData	tentacle_array_local_rotation_data;
	tentacleArray_children[TENTACLE_ARRAY_ROTATION_LOCAL] = tentacle_array_local_rotation_attr.create(
		"tentacleArray_local_rotation", "tA_local_rotation", MFnData::kDoubleArray,
		tentacle_array_local_rotation_data.create(default_local_rotation_array, &ms),
		&ms
	);
{
	MObjectArray	ramp_children(NUM_RAMP_CHILDREN);
	MFnNumericAttribute	Postion_attr;
	ramp_children[RAMP_POSITION] = Postion_attr.create("tentacleArray_ramp_local_Position", "tA_ramp_local_Position", MFnNumericData::kFloat, -1, &ms);
	MFnNumericAttribute	FloatValue_attr;
	ramp_children[RAMP_FLOAT_VALUE] = FloatValue_attr.create("tentacleArray_ramp_local_FloatValue", "tA_ramp_local_FloatValue", MFnNumericData::kFloat, -1, &ms);
	MFnEnumAttribute	Interp_attr;
	ramp_children[RAMP_INTERP_TYPE] = Interp_attr.create("tentacleArray_ramp_local_Interp", "tA_ramp_local_Interp", INTERP::Invalid, &ms);
	ms = Interp_attr.addField("None", INTERP::None);
	ms = Interp_attr.addField("Linear", INTERP::Linear);
	ms = Interp_attr.addField("Smooth", INTERP::Smooth);
	ms = Interp_attr.addField("Spline", INTERP::Spline);
	MFnCompoundAttribute	ramp_attr;
	tentacleArray_children[TENTACLE_ARRAY_ROTATION_RAMP] = ramp_attr.create("tentacleArray_ramp_local", "tA_ramp_local", &ms);
	for (int i = 0; i < NUM_RAMP_CHILDREN; ++i)
		ms = ramp_attr.addChild(ramp_children[i]);
	ms = ramp_attr.setUsesArrayDataBuilder(true);
	ms = ramp_attr.setArray(true);
}
	MObjectArray	tentacleArray_cut_children(NUM_TENTACLE_CUT_VAL_CHILDREN);
#if 1
	MFnNumericAttribute	tentacle_array_cut_frame_attr;
	tentacleArray_cut_children[TENTACLE_CUT_VAL_FRAME] = tentacle_array_cut_frame_attr.create("tentacleArray_cut_frame", "tA_cF", MFnNumericData::kInt, 0, &ms);
#else
	MFnNumericAttribute	tentacle_array_cut_have_attr;
	tentacleArray_cut_children[TENTACLE_CUT_VAL_HAVE] = tentacle_array_cut_have_attr.create("tentacleArray_cut_have", "tA_cH", MFnNumericData::kBoolean, 0, &ms);
#endif
	MFnNumericAttribute	tentacle_array_cut_t_attr;
	tentacleArray_cut_children[TENTACLE_CUT_VAL_T] = tentacle_array_cut_t_attr.create("tentacleArray_cut_t", "tA_cT", MFnNumericData::kDouble, 0.5, &ms);
	MFnNumericAttribute	tentacle_array_cut_polar_attr;
	tentacleArray_cut_children[TENTACLE_CUT_VAL_POLAR] = tentacle_array_cut_polar_attr.create("tentacleArray_cut_polar", "tA_cP", MFnNumericData::kDouble, 0, &ms);
	MFnNumericAttribute	tentacle_array_cut_azimuthal_attr;
	tentacleArray_cut_children[TENTACLE_CUT_VAL_AZIMUTHAL] = tentacle_array_cut_azimuthal_attr.create("tentacleArray_cut_azimuthal", "tA_cA", MFnNumericData::kDouble, 0, &ms);
	MFnCompoundAttribute	tentacle_array_cut_attr;
	tentacleArray_children[TENTACLE_ARRAY_CUT] = tentacle_array_cut_attr.create("tentacleArray_cut", "tA_c", &ms);
	for (int i = 0; i < NUM_TENTACLE_CUT_VAL_CHILDREN; ++i)
		ms = tentacle_array_cut_attr.addChild(tentacleArray_cut_children[i]);
	MFnCompoundAttribute	tentacle_array_attr;
	__tentacleArray = tentacle_array_attr.create("tentacleArray", "tA", &ms);
	for (int i = 0; i < NUM_TENTACLE_ARRAY_CHILDREN; ++i)
		ms = tentacle_array_attr.addChild(tentacleArray_children[i]);
	ms = tentacle_array_attr.setArray(true);
	ms = tentacle_array_attr.setUsesArrayDataBuilder(true);
	ms = addAttribute(__tentacleArray);
#endif
#if 2
{
	MObjectArray	tentacleArrayArray_children(NUM_TENTACLE_ARRAY_ARRAY_CHILDREN);
#if 1
	MObjectArray	tentacleParameter_children(NUM_TENTACLE_PARAM_CHILDREN);
#if 1
	MObjectArray	tentacleLength_children(NUM_TENTACLE_LENGTH_ATTRIBUTE_EDITOR_CHILDREN);
	MFnNumericAttribute	tentacleLength_sync_attr;
	tentacleLength_children[TENTACLE_ATTRIBUTE_EDITOR_SYNC] = tentacleLength_sync_attr.create("TAA__tentacleLength_sync", "TAA__tLEN_sync", MFnNumericData::kBoolean, 1, &ms);
	MFnNumericAttribute	tentacleLength_val_attr;
	tentacleLength_children[TENTACLE_ATTRIBUTE_EDITOR_VAL] = tentacleLength_val_attr.create("TAA__tentacleLength_val", "TAA__tLEN_val", MFnNumericData::kDouble, 10, &ms);
	MFnCompoundAttribute	tentacleLength_attr;
	tentacleParameter_children[TENTACLE_PARAM_LENGTH] = tentacleLength_attr.create("TAA__tentacleLength", "TAA__tLEN", &ms);
	for (int i = 0; i < NUM_TENTACLE_LENGTH_ATTRIBUTE_EDITOR_CHILDREN; ++i)
		ms = tentacleLength_attr.addChild(tentacleLength_children[i]);
#endif
#if 1
	MObjectArray	tentacleCrossSectionMajorRadius_children(NUM_TENTACLE_ATTRIBUTE_EDITOR_CHILDREN);
	MFnNumericAttribute	tentacleCrossSectionMajorRadius_val_attr;
	tentacleCrossSectionMajorRadius_children[TENTACLE_ATTRIBUTE_EDITOR_VAL] = tentacleCrossSectionMajorRadius_val_attr.create("TAA__tentacleCrossSectionMajorRadius_val", "TAA__tCSMR_val", MFnNumericData::kDouble, 1, &ms);
	MObjectArray	tentacleCrossSectionMajorRadius_scale_children(NUM_RAMP_CHILDREN);
	MFnNumericAttribute	tentacleCrossSectionMajorRadius_scale_Position_attr;
	tentacleCrossSectionMajorRadius_scale_children[RAMP_POSITION] = tentacleCrossSectionMajorRadius_scale_Position_attr.create("TAA__tentacleCrossSectionMajorRadius_scale_Position", "TAA__tCSMR_scale_Position", MFnNumericData::kFloat, -1, &ms);
	MFnNumericAttribute	tentacleCrossSectionMajorRadius_scale_FloatValue_attr;
	tentacleCrossSectionMajorRadius_scale_children[RAMP_FLOAT_VALUE] = tentacleCrossSectionMajorRadius_scale_FloatValue_attr.create("TAA__tentacleCrossSectionMajorRadius_scale_FloatValue", "TAA__tCSMR_scale_FloatValue", MFnNumericData::kFloat, -1, &ms);
	MFnEnumAttribute	tentacleCrossSectionMajorRadius_scale_Interp_attr;
	tentacleCrossSectionMajorRadius_scale_children[RAMP_INTERP_TYPE] = tentacleCrossSectionMajorRadius_scale_Interp_attr.create("TAA__tentacleCrossSectionMajorRadius_scale_Interp", "TAA__tCSMR_Interp", INTERP::Invalid, &ms);
	ms = tentacleCrossSectionMajorRadius_scale_Interp_attr.addField("None", INTERP::None);
	ms = tentacleCrossSectionMajorRadius_scale_Interp_attr.addField("Linear", INTERP::Linear);
	ms = tentacleCrossSectionMajorRadius_scale_Interp_attr.addField("Smooth", INTERP::Smooth);
	ms = tentacleCrossSectionMajorRadius_scale_Interp_attr.addField("Spline", INTERP::Spline);
	MFnCompoundAttribute	tentacleCrossSectionMajorRadius_scale_attr;
	tentacleCrossSectionMajorRadius_children[TENTACLE_ATTRIBUTE_EDITOR_RAMP] = tentacleCrossSectionMajorRadius_scale_attr.create("TAA__tentacleCrossSectionMajorRadius_scale", "TAA__tCSMR_scale", &ms);
	for (int i = 0; i < NUM_RAMP_CHILDREN; ++i)
		ms = tentacleCrossSectionMajorRadius_scale_attr.addChild(tentacleCrossSectionMajorRadius_scale_children[i]);
	ms = tentacleCrossSectionMajorRadius_scale_attr.setUsesArrayDataBuilder(true);
	ms = tentacleCrossSectionMajorRadius_scale_attr.setArray(true);
	MFnNumericAttribute	tentacleCrossSectionMajorRadius_sync_attr;
	tentacleCrossSectionMajorRadius_children[TENTACLE_ATTRIBUTE_EDITOR_SYNC] = tentacleCrossSectionMajorRadius_sync_attr.create("TAA__tentacleCrossSectionMajorRadius_sync", "TAA__tCSMR_sync", MFnNumericData::kBoolean, 1, &ms);
#if 0
	MFnNumericAttribute	tentacleCrossSectionMajorRadius_scale_wakes_attr;
	tentacleCrossSectionMajorRadius_children[TENTACLE_ATTRIBUTE_EDITOR_WAKES] = tentacleCrossSectionMajorRadius_scale_wakes_attr.create("tentacleCrossSectionMajorRadius_scale_wakes", "tCSMR_scale_wakes", MFnNumericData::kBoolean, 0., &ms);
#endif
	MFnCompoundAttribute	tentacleCrossSectionMajorRadius_attr;
	tentacleParameter_children[TENTACLE_PARAM_CROSS_SECTION_MAJOR_RADIUS] = tentacleCrossSectionMajorRadius_attr.create("TAA__tentacleCrossSectionMajorRadius", "TAA__tCSMR", &ms);
	for (int i = 0; i < NUM_TENTACLE_ATTRIBUTE_EDITOR_CHILDREN; ++i)
		ms = tentacleCrossSectionMajorRadius_attr.addChild(tentacleCrossSectionMajorRadius_children[i]);
#endif
#if 1
	MObjectArray	tentacleCrossSectionEccentricity_children(NUM_TENTACLE_ATTRIBUTE_EDITOR_CHILDREN);
	MFnNumericAttribute	tentacleCrossSectionEccentricity_val_attr;
	tentacleCrossSectionEccentricity_children[TENTACLE_ATTRIBUTE_EDITOR_VAL] = tentacleCrossSectionEccentricity_val_attr.create("TAA__tentacleCrossSectionEccentricity_val", "TAA__tCSE_val", MFnNumericData::kDouble, 0, &ms);
	MObjectArray	tentacleCrossSectionEccentricity_scale_children(NUM_RAMP_CHILDREN);
	MFnNumericAttribute	tentacleCrossSectionEccentricity_scale_Positioin_attr;
	tentacleCrossSectionEccentricity_scale_children[RAMP_POSITION] = tentacleCrossSectionEccentricity_scale_Positioin_attr.create("TAA__tentacleCrossSectionEccentricity_scale_Position", "TAA__tCSE_scale_Position", MFnNumericData::kFloat, -1, &ms);
	MFnNumericAttribute	tentacleCrossSectionEccentricity_scale_FloatValue_attr;
	tentacleCrossSectionEccentricity_scale_children[RAMP_FLOAT_VALUE] = tentacleCrossSectionEccentricity_scale_FloatValue_attr.create("TAA__tentacleCrossSectionEccentricity_scale_FloatValue", "TAA__tCSE_scale_FloatValue", MFnNumericData::kFloat, -1, &ms);
	MFnEnumAttribute	tentacleCrossSectionEccentricity_scale_Interp_attr;
	tentacleCrossSectionEccentricity_scale_children[RAMP_INTERP_TYPE] = tentacleCrossSectionEccentricity_scale_Interp_attr.create("TAA__tentacleCrossSectionEccentricity_scale_Interp", "TAA__tCSES_Interp", INTERP::Invalid, &ms);
	ms = tentacleCrossSectionEccentricity_scale_Interp_attr.addField("None", INTERP::None);
	ms = tentacleCrossSectionEccentricity_scale_Interp_attr.addField("Linear", INTERP::Linear);
	ms = tentacleCrossSectionEccentricity_scale_Interp_attr.addField("Smooth", INTERP::Smooth);
	ms = tentacleCrossSectionEccentricity_scale_Interp_attr.addField("Spline", INTERP::Spline);
	MFnCompoundAttribute	tentacleCrossSectionEccentricity_scale_attr;
	tentacleCrossSectionEccentricity_children[TENTACLE_ATTRIBUTE_EDITOR_RAMP] = tentacleCrossSectionEccentricity_scale_attr.create("TAA__tentacleCrossSectionEccentricity_scale", "TAA__tCSE_scale", &ms);
	for (int i = 0; i < NUM_RAMP_CHILDREN; ++i)
		ms = tentacleCrossSectionEccentricity_scale_attr.addChild(tentacleCrossSectionEccentricity_scale_children[i]);
	ms = tentacleCrossSectionEccentricity_scale_attr.setUsesArrayDataBuilder(true);
	ms = tentacleCrossSectionEccentricity_scale_attr.setArray(true);
	MFnNumericAttribute	tentacleCrossSectionEccentricity_sync_attr;
	tentacleCrossSectionEccentricity_children[TENTACLE_ATTRIBUTE_EDITOR_SYNC] = tentacleCrossSectionEccentricity_sync_attr.create("TAA__tentacleCrossSectionEccentricity_sync", "TAA__tCSE_sync", MFnNumericData::kBoolean, 1, &ms);
#if 0
	MFnNumericAttribute	tentacleCrossSectionEccentricity_scale_wakes_attr;
	tentacleCrossSectionEccentricity_children[TENTACLE_ATTRIBUTE_EDITOR_WAKES] = tentacleCrossSectionEccentricity_scale_wakes_attr.create("tentacleCrossSectionEccentricity_scale_wakes", "tCSE_scale_wakes", MFnNumericData::kBoolean, 0., &ms);
#endif
	MFnCompoundAttribute	tentacleCrossSectionEccentricity_attr;
	tentacleParameter_children[TENTACLE_PARAM_CROSS_SECTION_ECCENTRICITY] = tentacleCrossSectionEccentricity_attr.create("TAA__tentacleCrossSectionEccentricity", "TAA__tCSE", &ms);
	for (int i = 0; i < NUM_TENTACLE_ATTRIBUTE_EDITOR_CHILDREN; ++i)
		ms = tentacleCrossSectionEccentricity_attr.addChild(tentacleCrossSectionEccentricity_children[i]);
#endif
#if 1
	MObjectArray	tentacleCrossSectionRotation_children(NUM_TENTACLE_ATTRIBUTE_EDITOR_CHILDREN);
	MFnNumericAttribute	tentacleCrossSectionRotation_val_attr;
	tentacleCrossSectionRotation_children[TENTACLE_ATTRIBUTE_EDITOR_VAL] = tentacleCrossSectionRotation_val_attr.create("TAA__tentacleCrossSectionRotation_val", "TAA__tCSR_val", MFnNumericData::kDouble, 0, &ms);
	MObjectArray	tentacleCrossSectionRotation_local_children(NUM_RAMP_CHILDREN);
	MFnNumericAttribute	tentacleCrossSectionRotation_local_Position_attr;
	tentacleCrossSectionRotation_local_children[RAMP_POSITION] = tentacleCrossSectionRotation_local_Position_attr.create("TAA__tentacleCrossSectionRotation_local_Position", "TAA__tCSR_local_Position", MFnNumericData::kFloat, -1, &ms);
	MFnNumericAttribute	tentacleCrossSectionRotation_local_FloatValue_attr;
	tentacleCrossSectionRotation_local_children[RAMP_FLOAT_VALUE] = tentacleCrossSectionRotation_local_FloatValue_attr.create("TAA__tentacleCrossSectionRotation_local_FloatValue", "TAA__tCSR_local_FloatValue", MFnNumericData::kFloat, -1, &ms);
	MFnEnumAttribute	tentacleCrossSectionRotation_local_Interp_attr;
	tentacleCrossSectionRotation_local_children[RAMP_INTERP_TYPE] = tentacleCrossSectionRotation_local_Interp_attr.create("TAA__tentacleCrossSectionRotation_local_Interp", "TAA__tCSR_local_Interp", INTERP::Invalid, &ms);
	ms = tentacleCrossSectionRotation_local_Interp_attr.addField("None", INTERP::None);
	ms = tentacleCrossSectionRotation_local_Interp_attr.addField("Linear", INTERP::Linear);
	ms = tentacleCrossSectionRotation_local_Interp_attr.addField("Smooth", INTERP::Smooth);
	ms = tentacleCrossSectionRotation_local_Interp_attr.addField("Spline", INTERP::Spline);
	MFnCompoundAttribute	tentacleCrossSectionRotation_local_attr;
	tentacleCrossSectionRotation_children[TENTACLE_ATTRIBUTE_EDITOR_RAMP] = tentacleCrossSectionRotation_local_attr.create("TAA__tentacleCrossSectionRotation_local", "TAA__tCSR_local", &ms);
	for (int i = 0; i < NUM_RAMP_CHILDREN; ++i)
		ms = tentacleCrossSectionRotation_local_attr.addChild(tentacleCrossSectionRotation_local_children[i]);
	ms = tentacleCrossSectionRotation_local_attr.setUsesArrayDataBuilder(true);
	ms = tentacleCrossSectionRotation_local_attr.setArray(true);
	MFnNumericAttribute	tentacleCrossSectionRotation_sync_attr;
	tentacleCrossSectionRotation_children[TENTACLE_ATTRIBUTE_EDITOR_SYNC] = tentacleCrossSectionRotation_sync_attr.create("TAA__tentacleCrossSectionRotation_sync", "TAA__tCSR_sync", MFnNumericData::kBoolean, 1, &ms);
#if 0
	MFnNumericAttribute	tentacleCrossSectionRotation_local_wakes_attr;
	tentacleCrossSectionRotation_children[TENTACLE_ATTRIBUTE_EDITOR_WAKES] = tentacleCrossSectionRotation_local_wakes_attr.create("tentacleCrossSectionRotation_local_wakes", "tCSR_local_wakes", MFnNumericData::kBoolean, 0., &ms);
#endif
	MFnCompoundAttribute	tentacleCrossSectionRotation_attr;
	tentacleParameter_children[TENTACLE_PARAM_CROSS_SECTION_ROTATION] = tentacleCrossSectionRotation_attr.create("TAA__tentacleCrossSectionRotation", "TAA__tCSR", &ms);
	for (int i = 0; i < NUM_TENTACLE_ATTRIBUTE_EDITOR_CHILDREN; ++i)
		ms = tentacleCrossSectionRotation_attr.addChild(tentacleCrossSectionRotation_children[i]);
#endif
#if 1
	MObjectArray	tentacleCut_val_children(NUM_TENTACLE_CUT_VAL_CHILDREN);
#if 1
	MFnNumericAttribute	tentacleCut_frame_attr;
	tentacleCut_val_children[TENTACLE_CUT_VAL_FRAME] = tentacleCut_frame_attr.create("TAA__tentacleCut_frame", "TAA__tCUT_frame", MFnNumericData::kInt, 0, &ms);
#else
	MFnNumericAttribute	tentacleCut_have_attr;
	tentacleCut_val_children[TENTACLE_CUT_VAL_HAVE] = tentacleCut_have_attr.create("TAA__tentacleCut_have", "TAA__tCUT_have", MFnNumericData::kBoolean, 0, &ms);
#endif
	MFnNumericAttribute	tentacleCut_t_attr;
	tentacleCut_val_children[TENTACLE_CUT_VAL_T] = tentacleCut_t_attr.create("TAA__tentacleCut_t", "TAA__tCUT_t", MFnNumericData::kDouble, 0.5, &ms);
	MFnNumericAttribute	tentacleCut_polar_attr;
	tentacleCut_val_children[TENTACLE_CUT_VAL_POLAR] = tentacleCut_t_attr.create("TAA__tentacleCut_polar", "TAA__tCUT_polar", MFnNumericData::kDouble, 0, &ms);
	MFnNumericAttribute	tentacleCut_azimuthal_attr;
	tentacleCut_val_children[TENTACLE_CUT_VAL_AZIMUTHAL] = tentacleCut_azimuthal_attr.create("TAA__tentacleCut_azimuthal", "TAA__tCUT_azimuthal", MFnNumericData::kDouble, 0, &ms);
	MObjectArray	tentacleCut_children(NUM_TENTACLE_CUT_ATTRIBUTE_EDITOR_CHILDREN);
	MFnNumericAttribute	tentacleCut_sync_attr;
	tentacleCut_children[TENTACLE_ATTRIBUTE_EDITOR_SYNC] = tentacleCut_sync_attr.create("TAA__tentacleCut_sync", "TAA__tCUT_sync", MFnNumericData::kBoolean, 1, &ms);
	MFnCompoundAttribute	tentacleCut_val_attr;
	tentacleCut_children[TENTACLE_ATTRIBUTE_EDITOR_VAL] = tentacleCut_val_attr.create("TAA__tentacleCut_val", "TAA__tCUT_val", &ms);
	for (int i = 0; i < NUM_TENTACLE_CUT_VAL_CHILDREN; ++i)
		ms = tentacleCut_val_attr.addChild(tentacleCut_val_children[i]);
	MFnCompoundAttribute	tentacleCut_attr;
	tentacleParameter_children[TENTACLE_PARAM_CUT] = tentacleCut_attr.create("TAA__tentacleCut", "TAA__tCUT", &ms);
	for (int i = 0; i < NUM_TENTACLE_CUT_ATTRIBUTE_EDITOR_CHILDREN; ++i)
		ms = tentacleCut_attr.addChild(tentacleCut_children[i]);
#endif
	MFnCompoundAttribute	tentacleParameter_attr;
	tentacleArrayArray_children[TENTACLE_ARRAY_ARRAY_SIMULATION_PARAMETER] = tentacleParameter_attr.create("TAA__simulation_parameter", "TAA__sp", &ms);
	for (int i = 0; i < NUM_TENTACLE_PARAM_CHILDREN; ++i)
		ms = tentacleParameter_attr.addChild(tentacleParameter_children[i]);
#endif
#if 1
	MObjectArray	tentacleArray_children(NUM_TENTACLE_ARRAY_CHILDREN);
#if 1
{
	MObjectArray	tentacleRoot_children(NUM_TENTACLE_ROOT_CHILDREN);
	MFnNumericAttribute	tentaclePolyIdAttr;
	tentacleRoot_children[TENTACLE_ROOT_POLY_ID] = tentaclePolyIdAttr.create("TAA__tentacleArray_root_polyId", "TAA__tA_r_ID", MFnNumericData::kInt, -1, &ms);
	MFnNumericAttribute	tentaclePositionAttr;
	tentacleRoot_children[TENTACLE_ROOT_POSITION] = tentaclePositionAttr.create("TAA__tentacleArray_root_position", "TAA__tA_r_P", MFnNumericData::k3Double, 0, &ms);
	MFnNumericAttribute	tentacleNormalAttr;
	tentacleRoot_children[TENTACLE_ROOT_NORMAL] = tentacleNormalAttr.create("TAA__tentacleArray_root_normal", "TAA__tA_r_N", MFnNumericData::k3Double, 0, &ms);
#if 1
#if 1
	MFnNumericAttribute	tentacleHitVertexListAttr;
	tentacleRoot_children[TENTACLE_ROOT_HIT_VERTEX_LIST] = tentacleHitVertexListAttr.create("TAA__tentacleArray_root_hitVertexList", "TAA_tA_r_HIT_VLIST", MFnNumericData::k3Int, -1, &ms);
#else
	MFnNumericAttribute	tentacleHitTriangleIdAttr;
	tentacleRoot_children[TENTACLE_ROOT_HIT_TRIANGLE_ID] = tentacleHitTriangleIdAttr.create("TAA__tentacleArray_root_hitTriangleId", "TAA__tA_r_HIT_TRI_ID", MFnNumericData::kInt, -1, &ms);
#endif
	MFnNumericAttribute	tentacleHitBary1Attr;
	tentacleRoot_children[TENTACLE_ROOT_HIT_BARY1] = tentacleHitBary1Attr.create("TAA__tentacleArray_root_hitBary1", "TAA__tA_r_HIT_BARY1", MFnNumericData::kFloat, 0, &ms);
	MFnNumericAttribute	tentacleHitBary2Attr;
	tentacleRoot_children[TENTACLE_ROOT_HIT_BARY2] = tentacleHitBary2Attr.create("TAA__tentacleArray_root_hitBary2", "TAA__tA_r_HIT_BARY2", MFnNumericData::kFloat, 0, &ms);
#endif
	MFnCompoundAttribute	tentacleRootAttr;
	tentacleArray_children[TENTACLE_ARRAY_ROOT] = tentacleRootAttr.create("TAA__tentacleArray_root", "TAA__tA_r", &ms);
	for (int i = 0; i < NUM_TENTACLE_ROOT_CHILDREN; ++i)
		ms = tentacleRootAttr.addChild(tentacleRoot_children[i]);
}
#else
	MFnNumericAttribute	tentacle_array_root_attr;
	tentacleArray_children[TENTACLE_ARRAY_ROOT] = tentacle_array_root_attr.create("TAA__tentacleArray_root", "TAA__tA_r", MFnNumericData::k3Double, 0, &ms);
#endif
#if 1
#if 1
	MFnTypedAttribute	tentacle_array_quaternion_attr;
	MPointArray		default_quaternion_array;
	MFnPointArrayData	tentacle_array_quaternion_data;
	tentacleArray_children[TENTACLE_ARRAY_QUATERNION] = tentacle_array_quaternion_attr.create(
		"TAA__tentacleArray_quaternion", "TAA__tA_q", MFnData::kPointArray,
		tentacle_array_quaternion_data.create(default_quaternion_array, &ms),
		&ms
	);
#else
{
	MFnMatrixAttribute	quaternion_attr;
	tentacleArray_children[TENTACLE_ARRAY_QUATERNION] = quaternion_attr.create("TAA__tentacleArray_quaternion", "TAA__tA_q", MFnMatrixAttribute::kDouble, &ms);
	MMatrix	m;
	ms = quaternion_attr.setDefault(m);
	ms = quaternion_attr.setArray(true);
}
#endif
#endif
	MFnTypedAttribute	tentacle_array_longitudinal_attr;
	MVectorArray	default_longitudinal_array;
	MFnVectorArrayData	tentacle_array_longitudinal_data;
	tentacleArray_children[TENTACLE_ARRAY_LOGITUDINAL] = tentacle_array_longitudinal_attr.create(
		"TAA__tentacleArray_longitudinal", "TAA__tA_l", MFnData::kVectorArray,
		tentacle_array_longitudinal_data.create(default_longitudinal_array, &ms),
		&ms
	);
	MFnTypedAttribute	tentacle_array_transversal_attr;
	MVectorArray	default_transversal_array;
	MFnVectorArrayData	tentacle_array_transversal_data;
	tentacleArray_children[TENTACLE_ARRAY_TRANSVERSAL] = tentacle_array_transversal_attr.create(
		"TAA__tentacleArray_transversal", "TAA__tA_t", MFnData::kVectorArray,
		tentacle_array_transversal_data.create(default_transversal_array, &ms),
		&ms
	);
	MFnNumericAttribute	tentacle_array_a_attr;
	tentacleArray_children[TENTACLE_ARRAY_MAJOR_RADIUS] = tentacle_array_a_attr.create("TAA__tentacleArray_a", "TAA__tA_a", MFnNumericData::kDouble, 1, &ms);
	MFnTypedAttribute	tentacle_array_scale_a_attr;
	MDoubleArray	default_scale_a_array;
	MFnDoubleArrayData	tentacle_array_scale_a_data;
	tentacleArray_children[TENTACLE_ARRAY_MAJOR_RADIUS_SCALE] = tentacle_array_scale_a_attr.create(
		"TAA__tentacleArray_scale_a", "TAA__tA_scale_a", MFnData::kDoubleArray,
		tentacle_array_scale_a_data.create(default_scale_a_array, &ms),
		&ms
	);
{
	MObjectArray	ramp_children(NUM_RAMP_CHILDREN);
	MFnNumericAttribute	Postion_attr;
	ramp_children[RAMP_POSITION] = Postion_attr.create("TAA__tentacleArray_ramp_a_Position", "TAA__tA_ramp_a_Position", MFnNumericData::kFloat, -1, &ms);
	MFnNumericAttribute	FloatValue_attr;
	ramp_children[RAMP_FLOAT_VALUE] = FloatValue_attr.create("TAA__tentacleArray_ramp_a_FloatValue", "TAA__tA_ramp_a_FloatValue", MFnNumericData::kFloat, -1, &ms);
	MFnEnumAttribute	Interp_attr;
	ramp_children[RAMP_INTERP_TYPE] = Interp_attr.create("TAA__tentacleArray_ramp_a_Interp", "TAA__tA_ramp_a_Interp", INTERP::Invalid, &ms);
	ms = Interp_attr.addField("None", INTERP::None);
	ms = Interp_attr.addField("Linear", INTERP::Linear);
	ms = Interp_attr.addField("Smooth", INTERP::Smooth);
	ms = Interp_attr.addField("Spline", INTERP::Spline);
	MFnCompoundAttribute	ramp_attr;
	tentacleArray_children[TENTACLE_ARRAY_MAJOR_RADIUS_RAMP] = ramp_attr.create("TAA__tentacleArray_ramp_a", "TAA__tA_ramp_a", &ms);
	for (int i = 0; i < NUM_RAMP_CHILDREN; ++i)
		ms = ramp_attr.addChild(ramp_children[i]);
	ms = ramp_attr.setUsesArrayDataBuilder(true);
	ms = ramp_attr.setArray(true);
}
	MFnNumericAttribute	tentacle_array_e_attr;
	tentacleArray_children[TENTACLE_ARRAY_ECCENTRICITY] = tentacle_array_e_attr.create("TAA__tentacleArray_e", "TAA__tA_e", MFnNumericData::kDouble, 0, &ms);
	MFnTypedAttribute	tentacle_array_scale_e_attr;
	MDoubleArray	default_scale_e_array;
	MFnDoubleArrayData	tentacle_array_scale_e_data;
	tentacleArray_children[TENTACLE_ARRAY_ECCENTRICITY_SCALE] = tentacle_array_scale_e_attr.create(
		"TAA__tentacleArray_scale_e", "TAA__tA_scale_e", MFnData::kDoubleArray,
		tentacle_array_scale_e_data.create(default_scale_e_array, &ms),
		&ms
	);
{
	MObjectArray	ramp_children(NUM_RAMP_CHILDREN);
	MFnNumericAttribute	Postion_attr;
	ramp_children[RAMP_POSITION] = Postion_attr.create("TAA__tentacleArray_ramp_e_Position", "TAA__tA_ramp_e_Position", MFnNumericData::kFloat, -1, &ms);
	MFnNumericAttribute	FloatValue_attr;
	ramp_children[RAMP_FLOAT_VALUE] = FloatValue_attr.create("TAA__tentacleArray_ramp_e_FloatValue", "TAA__tA_ramp_e_FloatValue", MFnNumericData::kFloat, -1, &ms);
	MFnEnumAttribute	Interp_attr;
	ramp_children[RAMP_INTERP_TYPE] = Interp_attr.create("TAA__tentacleArray_ramp_e_Interp", "TAA__tA_ramp_e_Interp", INTERP::Invalid, &ms);
	ms = Interp_attr.addField("None", INTERP::None);
	ms = Interp_attr.addField("Linear", INTERP::Linear);
	ms = Interp_attr.addField("Smooth", INTERP::Smooth);
	ms = Interp_attr.addField("Spline", INTERP::Spline);
	MFnCompoundAttribute	ramp_attr;
	tentacleArray_children[TENTACLE_ARRAY_ECCENTRICITY_RAMP] = ramp_attr.create("TAA__tentacleArray_ramp_e", "TAA__tA_ramp_e", &ms);
	for (int i = 0; i < NUM_RAMP_CHILDREN; ++i)
		ms = ramp_attr.addChild(ramp_children[i]);
	ms = ramp_attr.setUsesArrayDataBuilder(true);
	ms = ramp_attr.setArray(true);
}
	MFnNumericAttribute	tentacle_array_rotation_attr;
	tentacleArray_children[TENTACLE_ARRAY_ROTATION] = tentacle_array_rotation_attr.create("TAA__tentacleArray_rotation", "TAA__tA_rotation", MFnNumericData::kDouble, 0, &ms);
	MFnTypedAttribute	tentacle_array_local_rotation_attr;
	MDoubleArray	default_local_rotation_array;
	MFnDoubleArrayData	tentacle_array_local_rotation_data;
	tentacleArray_children[TENTACLE_ARRAY_ROTATION_LOCAL] = tentacle_array_local_rotation_attr.create(
		"TAA__tentacleArray_local_rotation", "TAA__tA_local_rotation", MFnData::kDoubleArray,
		tentacle_array_local_rotation_data.create(default_local_rotation_array, &ms),
		&ms
	);
{
	MObjectArray	ramp_children(NUM_RAMP_CHILDREN);
	MFnNumericAttribute	Postion_attr;
	ramp_children[RAMP_POSITION] = Postion_attr.create("TAA__tentacleArray_ramp_local_Position", "TAA__tA_ramp_local_Position", MFnNumericData::kFloat, -1, &ms);
	MFnNumericAttribute	FloatValue_attr;
	ramp_children[RAMP_FLOAT_VALUE] = FloatValue_attr.create("TAA__tentacleArray_ramp_local_FloatValue", "TAA__tA_ramp_local_FloatValue", MFnNumericData::kFloat, -1, &ms);
	MFnEnumAttribute	Interp_attr;
	ramp_children[RAMP_INTERP_TYPE] = Interp_attr.create("TAA__tentacleArray_ramp_local_Interp", "TAA__tA_ramp_local_Interp", INTERP::Invalid, &ms);
	ms = Interp_attr.addField("None", INTERP::None);
	ms = Interp_attr.addField("Linear", INTERP::Linear);
	ms = Interp_attr.addField("Smooth", INTERP::Smooth);
	ms = Interp_attr.addField("Spline", INTERP::Spline);
	MFnCompoundAttribute	ramp_attr;
	tentacleArray_children[TENTACLE_ARRAY_ROTATION_RAMP] = ramp_attr.create("TAA__tentacleArray_ramp_local", "TAA__tA_ramp_local", &ms);
	for (int i = 0; i < NUM_RAMP_CHILDREN; ++i)
		ms = ramp_attr.addChild(ramp_children[i]);
	ms = ramp_attr.setUsesArrayDataBuilder(true);
	ms = ramp_attr.setArray(true);
}
	MObjectArray	tentacleArray_cut_children(NUM_TENTACLE_CUT_VAL_CHILDREN);
#if 1
	MFnNumericAttribute	tentacle_array_cut_frame_attr;
	tentacleArray_cut_children[TENTACLE_CUT_VAL_FRAME] = tentacle_array_cut_frame_attr.create("TAA__tentacleArray_cut_frame", "TAA__tA_cF", MFnNumericData::kInt, 0, &ms);
#else
	MFnNumericAttribute	tentacle_array_cut_have_attr;
	tentacleArray_cut_children[TENTACLE_CUT_VAL_HAVE] = tentacle_array_cut_have_attr.create("TAA__tentacleArray_cut_have", "TAA__tA_cH", MFnNumericData::kBoolean, 0, &ms);
#endif
	MFnNumericAttribute	tentacle_array_cut_t_attr;
	tentacleArray_cut_children[TENTACLE_CUT_VAL_T] = tentacle_array_cut_t_attr.create("TAA__tentacleArray_cut_t", "TAA__tA_cT", MFnNumericData::kDouble, 0.5, &ms);
	MFnNumericAttribute	tentacle_array_cut_polar_attr;
	tentacleArray_cut_children[TENTACLE_CUT_VAL_POLAR] = tentacle_array_cut_polar_attr.create("TAA__tentacleArray_cut_polar", "TAA__tA_cP", MFnNumericData::kDouble, 0, &ms);
	MFnNumericAttribute	tentacle_array_cut_azimuthal_attr;
	tentacleArray_cut_children[TENTACLE_CUT_VAL_AZIMUTHAL] = tentacle_array_cut_azimuthal_attr.create("TAA__tentacleArray_cut_azimuthal", "TAA__tA_cA", MFnNumericData::kDouble, 0, &ms);
	MFnCompoundAttribute	tentacle_array_cut_attr;
	tentacleArray_children[TENTACLE_ARRAY_CUT] = tentacle_array_cut_attr.create("TAA__tentacleArray_cut", "TAA__tA_c", &ms);
	for (int i = 0; i < NUM_TENTACLE_CUT_VAL_CHILDREN; ++i)
		ms = tentacle_array_cut_attr.addChild(tentacleArray_cut_children[i]);
	MFnCompoundAttribute	tentacle_array_attr;
	tentacleArrayArray_children[TENTACLE_ARRAY_ARRAY_SIMULATION_OUTPUT] = tentacle_array_attr.create("TAA__simulation_output", "TAA__so", &ms);
	for (int i = 0; i < NUM_TENTACLE_ARRAY_CHILDREN; ++i)
		ms = tentacle_array_attr.addChild(tentacleArray_children[i]);
	ms = tentacle_array_attr.setArray(true);
	ms = tentacle_array_attr.setUsesArrayDataBuilder(true);
#endif
#if 0
	MFnCompoundAttribute	tentacleArrayArray_simulation_output_attr;
	cout << "# of children = " << tentacle_array_attr.numChildren(&ms) << endl;
	cout << "# of children = " << tentacleArrayArray_simulation_output_attr.numChildren(&ms) << endl;
	for (int i = 0; i < NUM_TENTACLE_ARRAY_CHILDREN; ++i)
		ms = tentacleArrayArray_simulation_output_attr.addChild(tentacleArray_children[i]);
	cout << "# of children = " << tentacleArrayArray_simulation_output_attr.numChildren(&ms) << endl;
	cout << "isArray? " << (tentacleArrayArray_simulation_output_attr.isArray(&ms) ? "True" : "False") << endl;
	cout << "usesArrayDataBuilder? " << (tentacleArrayArray_simulation_output_attr.usesArrayDataBuilder(&ms) ? "True" : "False") << endl;
	ms = tentacleArrayArray_simulation_output_attr.setArray(true);
	ms = tentacleArrayArray_simulation_output_attr.setUsesArrayDataBuilder(true);
	cout << "isArray? " << (tentacleArrayArray_simulation_output_attr.isArray(&ms) ? "True" : "False") << endl;
	cout << "usesArrayDataBuilder? " << (tentacleArrayArray_simulation_output_attr.usesArrayDataBuilder(&ms) ? "True" : "False") << endl;
#endif
	MFnCompoundAttribute	tentacle_array_array_attr;
	__tentacleArrayArray = tentacle_array_array_attr.create("tentacleArrayArray", "tAA", &ms);
	for (int i = 0; i < NUM_TENTACLE_ARRAY_ARRAY_CHILDREN; ++i)
		ms = tentacle_array_array_attr.addChild(tentacleArrayArray_children[i]);
	ms = tentacle_array_array_attr.setUsesArrayDataBuilder(true);
	ms = tentacle_array_array_attr.setArray(true);
	ms = addAttribute(__tentacleArrayArray);
}
	MFnNumericAttribute	frameIndexAttr;
	__frameIndex = frameIndexAttr.create("frameIndex", "fIdx", MFnNumericData::kInt, -1, &ms);
	ms = addAttribute(__frameIndex);
#endif
	MFnEnumAttribute	tentacleDoActionAttr;
	__tentacleDoAction = tentacleDoActionAttr.create("tentacleDoAction", "tDA", TentacleDo::None, &ms);
	ms = tentacleDoActionAttr.addField("Remove", TentacleDo::Remove);
	ms = tentacleDoActionAttr.addField("Refresh", TentacleDo::Refresh);
#if 1
	ms = tentacleDoActionAttr.addField("Load", TentacleDo::Load);
	ms = tentacleDoActionAttr.addField("Save", TentacleDo::Save);
#endif
	ms = addAttribute(__tentacleDoAction);
#endif
#endif
#endif
#if 1
	MFnTypedAttribute	typedAttr;
	tentacleShape::outputMesh = typedAttr.create("outputMesh", "out", MFnData::kMesh, &ms);
	ms = addAttribute(tentacleShape::outputMesh);
	ms = typedAttr.setArray(true);
#endif
	return baseShape::initialize();
}
#endif
tentacleShape::tentacleShape()
{
#if 1
#if 1
#if 1
	MStatus	ms;
	__activeListModifiedCallbackId = MModelMessage::addCallback(MModelMessage::kActiveListModified, tentacleShape::__activeListModifiedCallback, this, &ms);
#endif
#endif
#endif
}
tentacleShape::~tentacleShape()
{
#if 1
#if 1
#if 1
	MStatus	ms;
	ms = MMessage::removeCallback(__activeListModifiedCallbackId);
#endif
#endif
#endif
}
#if 1
void	tentacleShape::postConstructor()
{
#if 1
#if 1
#if 0
	MStatus	ms;
	MFnDoubleArrayData	doubleData;
	MDoubleArray	data(10);
	MObject		valData = doubleData.create(data, &ms);
	MDoubleArray	array1 = doubleData.array(&ms);
	MDoubleArray	array2 = doubleData.array(&ms);
	cout << array1 << endl;
	cout << array2 << endl;
	array1[0] = 1;
	cout << array1 << endl;
	cout << array2 << endl;
#endif
{
	MStatus	ms;
	MPlug	plug_compound_tentacleParameter(thisMObject(), __tentacleParameter);
	MPlug	plug_compound_tentacleCrossSectionMajorRadius = plug_compound_tentacleParameter.child(TENTACLE_PARAM_CROSS_SECTION_MAJOR_RADIUS, &ms);
	MPlug	plug_array_tentacleCrossSectionMajorRadius_scale = plug_compound_tentacleCrossSectionMajorRadius.child(TENTACLE_ATTRIBUTE_EDITOR_RAMP, &ms);
	MPlug	plug_element_tentacleCrossSectionMajorRadius_scale0 = plug_array_tentacleCrossSectionMajorRadius_scale.elementByLogicalIndex(0, &ms);
	plug_element_tentacleCrossSectionMajorRadius_scale0.child(RAMP_POSITION, &ms).setValue(0);
	plug_element_tentacleCrossSectionMajorRadius_scale0.child(RAMP_FLOAT_VALUE, &ms).setValue(1);
	plug_element_tentacleCrossSectionMajorRadius_scale0.child(RAMP_INTERP_TYPE, &ms).setValue(INTERP::Spline);
	MPlug	plug_element_tentacleCrossSectionMajorRadius_scale1 = plug_array_tentacleCrossSectionMajorRadius_scale.elementByLogicalIndex(1, &ms);
	plug_element_tentacleCrossSectionMajorRadius_scale1.child(RAMP_POSITION, &ms).setValue(1);
	plug_element_tentacleCrossSectionMajorRadius_scale1.child(RAMP_FLOAT_VALUE, &ms).setValue(0.2);
	plug_element_tentacleCrossSectionMajorRadius_scale1.child(RAMP_INTERP_TYPE, &ms).setValue(INTERP::Spline);
	__setRampTupleArray(RampWakes::MajorRadius);
	MPlug	plug_compound_tentacleCrossSectionEccentricity = plug_compound_tentacleParameter.child(TENTACLE_PARAM_CROSS_SECTION_ECCENTRICITY, &ms);
	MPlug	plug_array_tentacleCrossSectionEccentricity_scale = plug_compound_tentacleCrossSectionEccentricity.child(TENTACLE_ATTRIBUTE_EDITOR_RAMP, &ms);
	MPlug	plug_element_tentacleCrossSectionEccentricity_scale0 = plug_array_tentacleCrossSectionEccentricity_scale.elementByLogicalIndex(0, &ms);
	plug_element_tentacleCrossSectionEccentricity_scale0.child(RAMP_POSITION, &ms).setValue(0);
	plug_element_tentacleCrossSectionEccentricity_scale0.child(RAMP_FLOAT_VALUE, &ms).setValue(1);
	plug_element_tentacleCrossSectionEccentricity_scale0.child(RAMP_INTERP_TYPE, &ms).setValue(INTERP::Spline);
	MPlug	plug_element_tentacleCrossSectionEccentricity_scale1 = plug_array_tentacleCrossSectionEccentricity_scale.elementByLogicalIndex(1, &ms);
	plug_element_tentacleCrossSectionEccentricity_scale1.child(RAMP_POSITION, &ms).setValue(1);
	plug_element_tentacleCrossSectionEccentricity_scale1.child(RAMP_FLOAT_VALUE, &ms).setValue(0.8);
	plug_element_tentacleCrossSectionEccentricity_scale1.child(RAMP_INTERP_TYPE, &ms).setValue(INTERP::Spline);
	__setRampTupleArray(RampWakes::Eccentricity);
	MPlug	plug_compound_tentacleCrossSectionRotation = plug_compound_tentacleParameter.child(TENTACLE_PARAM_CROSS_SECTION_ROTATION, &ms);
	MPlug	plug_array_tentacleCrossSectionRotation_local = plug_compound_tentacleCrossSectionRotation.child(TENTACLE_ATTRIBUTE_EDITOR_RAMP, &ms);
	MPlug	plug_element_tentacleCrossSectionRotationLocal0 = plug_array_tentacleCrossSectionRotation_local.elementByLogicalIndex(0, &ms);
	plug_element_tentacleCrossSectionRotationLocal0.child(RAMP_POSITION, &ms).setValue(0);
	plug_element_tentacleCrossSectionRotationLocal0.child(RAMP_FLOAT_VALUE, &ms).setValue(0.5);
	plug_element_tentacleCrossSectionRotationLocal0.child(RAMP_INTERP_TYPE, &ms).setValue(INTERP::Spline);
	MPlug	plug_element_tentacleCrossSectionRotationLocal1 = plug_array_tentacleCrossSectionRotation_local.elementByLogicalIndex(1, &ms);
	plug_element_tentacleCrossSectionRotationLocal1.child(RAMP_POSITION, &ms).setValue(1);
	plug_element_tentacleCrossSectionRotationLocal1.child(RAMP_FLOAT_VALUE, &ms).setValue(0.5);
	plug_element_tentacleCrossSectionRotationLocal1.child(RAMP_INTERP_TYPE, &ms).setValue(INTERP::Spline);
	__setRampTupleArray(RampWakes::Rotation);
}
#endif
#endif
#if 0
	updateConnection = false;
#endif
	baseShape::postConstructor();
}
MStatus	tentacleShape::compute(const MPlug& plug, MDataBlock& datablock)
{
#pragma warning(disable: 4127)
#if 1
#if 1
	if (0) {
	} else if (plug == __rampWakes) {
		MStatus	ms;
		RampWakes::Type	type = __rampWakesType;
		__rampWakesType = RampWakes::None;
		cout << "compute(__rampWakes): " << type << endl;
		if (type == RampWakes::None) {
		}
		if (
			type == RampWakes::MajorRadius ||
			type == RampWakes::Eccentricity ||
			type == RampWakes::Rotation
		) {
			__setRampTupleArray(type);
		}
		ms = datablock.setClean(plug);
		return MStatus::kSuccess;
#if 1
	} else if (plug == __tentacleArray) {
		MStatus	ms;
		MDataHandle	handle_doAction = datablock.inputValue(__tentacleDoAction, &ms);
		TentacleDo::Action	action = (TentacleDo::Action) handle_doAction.asShort();
		handle_doAction.set(TentacleDo::None);
		handle_doAction.setClean();
		cout << "compute(__tentacleArray): " << action << endl;
		if (action == TentacleDo::None) {
		}
		if (
			action == TentacleDo::Refresh ||
			action == TentacleDo::Add ||
			action == TentacleDo::Update ||
			action == TentacleDo::Move
		) {
			__setRampTupleArray(RampWakes::MajorRadius);
			__setRampTupleArray(RampWakes::Eccentricity);
			__setRampTupleArray(RampWakes::Rotation);
		}
#if 1
		if (action == TentacleDo::Load || action == TentacleDo::Save) {
			cout << "**** TentacleDo::" << (action == TentacleDo::Load ? "Load" : "Save") << " ****" << endl;
#if 1
#pragma warning(disable: 4127)
do {
	MPlug	plug_frameIndex(thisMObject(), __frameIndex);
	int	frameIndex;
	ms = plug_frameIndex.getValue(frameIndex);
	cout << "frameIndex = " << frameIndex << " (" << ms << ")" << endl;
	if (!ms) {
		ms.perror("compute(): MPlug::getValue(int): Failure");
		break;
	}
	if (action == TentacleDo::Load) {
#if 1
do {
	MPlug	plug_tentacleArrayArray(thisMObject(), __tentacleArrayArray);
	MPlug	plug_simulation_output = plug_tentacleArrayArray.child(TENTACLE_ARRAY_ARRAY_SIMULATION_OUTPUT, &ms);
	MPlug	plug_simulation_parameter = plug_tentacleArrayArray.child(TENTACLE_ARRAY_ARRAY_SIMULATION_PARAMETER, &ms);
	cout << "# of __tentacleArray = " << plug_tentacleArrayArray.numElements() << endl;
	if (frameIndex < 0 || frameIndex >= (int) plug_tentacleArrayArray.numElements()) {
		cerr << "TentacleDo::Load: frame index = " << frameIndex << ": Invalid: # of __tentacleArray = " << plug_tentacleArrayArray.numElements() << endl;
		break;
	}
	MArrayDataHandle	inputArrayArray = datablock.inputArrayValue(__tentacleArrayArray);
#if MAYA_API_VERSION >= 200806
	ms = inputArrayArray.jumpToArrayElement(frameIndex);
#else
	ms = inputArrayArray.jumpToElement(frameIndex);
#endif
	MDataHandle	handleArray = inputArrayArray.inputValue();
	MDataHandle	handle_simulation_parameter = handleArray.child(plug_simulation_parameter);
	MDataHandle	handle_simulation_output = handleArray.child(plug_simulation_output);
MPlug	plug_parameter(thisMObject(), __tentacleParameter);
MDataHandle	handle_parameter = datablock.outputValue(plug_parameter, &ms);
handle_parameter.copy(handle_simulation_parameter);
handle_parameter.setClean();
	MPlug	plug_output(thisMObject(), __tentacleArray);
	MDataHandle	handle_output = datablock.outputValue(plug_output, &ms);
	handle_output.copy(handle_simulation_output);
	handle_output.setClean();
	ms = MGlobal::clearSelectionList();
} while (0);
#endif
	}
	if (action == TentacleDo::Save) {
#if 1
do {
	MPlug	plug_output(thisMObject(), __tentacleArray);
	MPlug	plug_tentacleArrayArray(thisMObject(), __tentacleArrayArray);
	if (frameIndex < 0 || frameIndex > (int) plug_tentacleArrayArray.numElements()) {
		cerr << "TentacleDo::Save: frame index = " << frameIndex << ": Invalid: # of __tentacleArray = " << plug_tentacleArrayArray.numElements() << endl;
		break;
	}
	if (plug_output.numElements() == 0) {
		cerr << "TentacleDo::Save: No tentacle(s)" << endl;
		break;
	}
#if 1
	MPlug	plug_simulation_output = plug_tentacleArrayArray.child(TENTACLE_ARRAY_ARRAY_SIMULATION_OUTPUT, &ms);
	MPlug	plug_simulation_parameter = plug_tentacleArrayArray.child(TENTACLE_ARRAY_ARRAY_SIMULATION_PARAMETER, &ms);
	MPlug	plug_parameter(thisMObject(), __tentacleParameter);
	MDataHandle	handle_parameter = datablock.inputValue(plug_parameter, &ms);
	MDataHandle	handle_output = datablock.inputValue(plug_output, &ms);
#endif
	MArrayDataHandle	outputArrayArray = datablock.outputArrayValue(__tentacleArrayArray);
	unsigned	numElements = outputArrayArray.elementCount();
	MArrayDataBuilder	builderArray(&datablock, __tentacleArrayArray, numElements + (frameIndex == (int) numElements ? 1 : 0));
	for (int physicalIndex = 0; physicalIndex < (int) numElements; ++physicalIndex) {
#if MAYA_API_VERSION >= 200806
		ms = outputArrayArray.jumpToArrayElement(physicalIndex);
#else
		ms = outputArrayArray.jumpToElement(physicalIndex);
#endif
		MDataHandle	handleArray = outputArrayArray.inputValue();
		MDataHandle	outputHandleArray = builderArray.addElement(physicalIndex);
		if (frameIndex == physicalIndex) {
#if 1
			MDataHandle	handle_simulation_parameter = outputHandleArray.child(plug_simulation_parameter);
			ms = handle_simulation_parameter.copy(handle_parameter);
			MDataHandle	handle_simulation_output = outputHandleArray.child(plug_simulation_output);
			ms = handle_simulation_output.copy(handle_output);
#endif
		} else {
			outputHandleArray.copy(handleArray);
		}
	}
	if (frameIndex == (int) numElements) {
		MDataHandle	handleArray = builderArray.addLast(&ms);
#if 1
		MDataHandle	handle_simulation_parameter = handleArray.child(plug_simulation_parameter);
		ms = handle_simulation_parameter.copy(handle_parameter);
		MDataHandle	handle_simulation_output = handleArray.child(plug_simulation_output);
		ms = handle_simulation_output.copy(handle_output);
#endif
	}
	outputArrayArray.set(builderArray);
	outputArrayArray.setAllClean();
} while (0);
#endif
	}
} while (0);
#pragma warning(default: 4127)
#endif
		}
#endif
		if (action == TentacleDo::Remove) {
#if 1
			int	iSelectBy;
			tentacleShape*	nonConstThis = (tentacleShape*) this;
			nonConstThis->getSelectBy(iSelectBy);
			MObjectArray	list = activeComponents();
			cout << "# of selected objects = " << list.length() << endl;
			MIntArray	indexArray;
			if (iSelectBy == selectByVertex && list[0].hasFn(MFn::kDoubleIndexedComponent) && list[0].hasFn(MFn::kSurfaceCVComponent)) {
				cout << "MFnDoubleIndexedComponent:" << endl;
				MFnDoubleIndexedComponent	fnComponent(list[0], &ms);
				for (int i = 0; i < fnComponent.elementCount(&ms); ++i) {
					int	uIndex;
					int	vIndex;
					ms = fnComponent.getElement(i, uIndex, vIndex);
					cout << "\t[" << i << "]: (" << uIndex << ", " << vIndex << ")" << endl;
					indexArray.append(uIndex);
				}
			} else if (iSelectBy == selectByGuide && list[0].hasFn(MFn::kSingleIndexedComponent) && list[0].hasFn(MFn::kMeshEdgeComponent)) {
				cout << "MFnSingleIndexedComponent:" << endl;
				MFnSingleIndexedComponent	fnComponent(list[0], &ms);
				cout << "# of single-indexed components = " << fnComponent.elementCount(&ms) << endl;
				for (int i = 0; i < fnComponent.elementCount(&ms); ++i) {
					int	index = fnComponent.element(i, &ms);
					cout << "\t[" << i << "]: physical index = " << index << endl;
					indexArray.append(index);
				}
			}
			MArrayDataHandle	outputArray = datablock.outputArrayValue(__tentacleArray);
			unsigned	numElements = outputArray.elementCount();
			cout << "# of ... = " << numElements << " (" << ms << "), indices = " << indexArray << endl;
			MArrayDataBuilder	builder(&datablock, __tentacleArray, numElements);
			int	firstValidPhysicalIndex = -1;
			int	numDeleted = 0;
			for (int physicalIndex = 0; physicalIndex < (int) numElements; ++physicalIndex) {
#if MAYA_API_VERSION >= 200806
				ms = outputArray.jumpToArrayElement(physicalIndex);
#else
				ms = outputArray.jumpToElement(physicalIndex);
#endif
				unsigned	logicalIndex = outputArray.elementIndex(&ms); assert(ms);
				cout << "physical index = " << physicalIndex << "; logical index = " << logicalIndex << endl;
				MDataHandle	srcHandle = outputArray.inputValue();
				bool	found = false;
				for (int i = 0; i < (int) indexArray.length(); ++i) {
					if (indexArray[i] == (int) physicalIndex) {
						found = true;
						break;
					}
				}
				if (!found) {
					if (firstValidPhysicalIndex == -1) {
						firstValidPhysicalIndex = physicalIndex - numDeleted;
					}
					MDataHandle	handle = builder.addElement(physicalIndex);
					handle.copy(srcHandle);
				} else
					++numDeleted;
			}
			outputArray.set(builder);
			outputArray.setAllClean();
#if 1
{
#if 1
	MObject&	objArray = outputMesh;
			MArrayDataHandle	outputArray = datablock.outputArrayValue(objArray);
			unsigned	numElements = outputArray.elementCount();
			cout << "# of ... = " << numElements << " (" << ms << "), indices = " << indexArray << endl;
			MArrayDataBuilder	builder(&datablock, objArray, numElements);
#if 0
			int	firstValidPhysicalIndex = -1;
			int	numDeleted = 0;
#endif
#if 1
			assert(numElements % 3 == 0);
			for (int physicalIndex = 0; physicalIndex < (int) numElements / 3; ++physicalIndex) {
				bool	found = false;
				for (int i = 0; i < (int) indexArray.length(); ++i) {
					if (indexArray[i] == (int) physicalIndex) {
						found = true;
						break;
					}
				}
				if (found) {
					continue;
				}
		for (int i = 0; i < 3; ++i) {
#if MAYA_API_VERSION >= 200806
				ms = outputArray.jumpToArrayElement(3 * physicalIndex + i);
#else
				ms = outputArray.jumpToElement(3 * physicalIndex + i);
#endif
#if 1
				unsigned	logicalIndex = outputArray.elementIndex(&ms); assert(ms);
				cout << "physical index = " << physicalIndex << "; logical index = " << logicalIndex << endl;
#endif
				MDataHandle	srcHandle = outputArray.inputValue();
				MDataHandle	handle = builder.addElement(3 * physicalIndex + i);
				handle.copy(srcHandle);
		}
			}
#else
			for (int physicalIndex = 0; physicalIndex < (int) numElements; ++physicalIndex) {
#if MAYA_API_VERSION >= 200806
				ms = outputArray.jumpToArrayElement(physicalIndex);
#else
				ms = outputArray.jumpToElement(physicalIndex);
#endif
#if 1
				unsigned	logicalIndex = outputArray.elementIndex(&ms); assert(ms);
				cout << "physical index = " << physicalIndex << "; logical index = " << logicalIndex << endl;
#endif
				MDataHandle	srcHandle = outputArray.inputValue();
				bool	found = false;
				for (int i = 0; i < (int) indexArray.length(); ++i) {
					if (indexArray[i] == (int) physicalIndex) {
						found = true;
						break;
					}
				}
				if (!found) {
#if 0
					if (firstValidPhysicalIndex == -1) {
						firstValidPhysicalIndex = physicalIndex - numDeleted;
					}
#endif
					MDataHandle	handle = builder.addElement(physicalIndex);
					handle.copy(srcHandle);
				} else
#if 0
					++numDeleted;
#else
#if 0
					updateConnection = true;
#else
					;
#endif
#endif
			}
#endif
			outputArray.set(builder);
			outputArray.setAllClean();
#endif
}
#endif
			cout << "firstValidPhysicalIndex = " << firstValidPhysicalIndex << endl;
			if (firstValidPhysicalIndex == -1) {
				lastSelectedPhysicalIndex = -1;
				ms = MGlobal::clearSelectionList();
			} else {
				MDagPath	tentacleDagPathShape;
				ms = findTentacleShapeNode(__externalDagPathShape.node(), tentacleDagPathShape);
				MFnSingleIndexedComponent	fnEdgeComp;
				MObject	edgeComp = fnEdgeComp.create(MFn::kMeshEdgeComponent, &ms);
				ms = fnEdgeComp.addElement(firstValidPhysicalIndex);
				lastSelectedPhysicalIndex = firstValidPhysicalIndex;
				MSelectionList	list;
				ms = list.add(tentacleDagPathShape, edgeComp);
				ms = MGlobal::setActiveSelectionList(list);
			}
#else
#if 0
			MDataHandle	handle_removedIndex = datablock.inputValue(__tentacleRemovedIndex, &ms);
			int	removedIndex = handle_removedIndex.asInt();
			cout << "removed index = " << removedIndex << endl;
			if (removedIndex < 0 && removedIndex != -9999) {
				return MStatus::kSuccess;
			}
			MArrayDataHandle	outputArray = datablock.outputArrayValue(__tentacleArray);
			unsigned	numElements = outputArray.elementCount();
			if (removedIndex >= 0 && removedIndex >= (int) numElements) {
				cerr << "Invalid Removed Index = " << removedIndex << " (# of elements = " << numElements << ")" << endl;
				return MStatus::kFailure;
			}
			MArrayDataBuilder	builder(&datablock, __tentacleArray, removedIndex < 0 ? 0 : numElements - 1);
			if (removedIndex >= 0) {
				int	index = 0;
				for (int n = 0; n < (int) numElements; ++n) {
					ms = outputArray.jumpToArrayElement(n);
					unsigned	elementIndex = outputArray.elementIndex(&ms); assert(ms);
					if ((int) elementIndex == removedIndex)
						continue;
					MDataHandle	handle = outputArray.inputValue();
					MDataHandle	outputHandle = builder.addElement(index++);
					outputHandle.copy(handle);
				}
			}
			outputArray.set(builder);
			outputArray.setAllClean();
#endif
#endif
#if 0
#if 1
{
			MPlug	plug_tentacleArrayArray(thisMObject(), __tentacleArrayArray);
			MArrayDataHandle	handleArrayArray = datablock.inputArrayValue(plug_tentacleArrayArray, &ms);
			cout << ms << endl;
			unsigned	numElements = handleArrayArray.elementCount(&ms);
			cout << numElements << endl;
		if (numElements) {
#if MAYA_API_VERSION >= 200806
			ms = handleArrayArray.jumpToArrayElement(numElements - 1);
#else
			ms = handleArrayArray.jumpToElement(numElements - 1);
#endif
			MDataHandle	handleArray = handleArrayArray.inputValue();
#if 1
{
			MPlug	plug_simulation_output = plug_tentacleArrayArray.child(TENTACLE_ARRAY_ARRAY_SIMULATION_OUTPUT, &ms);
			cout << ms << endl;
			MDataHandle	handle_simulation_output = handleArray.child(plug_simulation_output);
			MPlug	plug_output(thisMObject(), __tentacleArray);
			MDataHandle	handle_output = datablock.outputValue(plug_output, &ms);
			ms = handle_output.copy(handle_simulation_output);
			cout << ms << endl;
}
#endif
#if 1
{
			MPlug	plug_simulation_parameter = plug_tentacleArrayArray.child(TENTACLE_ARRAY_ARRAY_SIMULATION_PARAMETER, &ms);
			cout << ms << endl;
			MDataHandle	handle_simulation_parameter = handleArray.child(plug_simulation_parameter);
			MPlug	plug_parameter(thisMObject(), __tentacleParameter);
			MDataHandle	handle_parameter = datablock.outputValue(plug_parameter, &ms);
			ms = handle_parameter.copy(handle_simulation_parameter);
			cout << ms << endl;
}
#endif
		}
}
#endif
#endif
		}
		if (action == TentacleDo::Add) {
			MArrayDataHandle	outputArray = datablock.outputArrayValue(__tentacleArray);
			unsigned	numElements = outputArray.elementCount();
			MArrayDataBuilder	builder(&datablock, __tentacleArray, numElements + 1);
			for (int physicalIndex = 0; physicalIndex < (int) numElements; ++physicalIndex) {
#if MAYA_API_VERSION >= 200806
				ms = outputArray.jumpToArrayElement(physicalIndex);
#else
				ms = outputArray.jumpToElement(physicalIndex);
#endif
				MDataHandle	handle = outputArray.inputValue();
				MDataHandle	outputHandle = builder.addElement(physicalIndex);
				outputHandle.copy(handle);
			}
			MDataHandle	handle = builder.addLast(&ms);
#if 1
	MPlug	plug_tentacle_root(thisMObject(), __tentacleRoot);
	MPlug	plug_tentacle_root_polyId = plug_tentacle_root.child(TENTACLE_ROOT_POLY_ID, &ms);
	MPlug	plug_tentacle_root_position = plug_tentacle_root.child(TENTACLE_ROOT_POSITION, &ms);
	MPlug	plug_tentacle_root_normal = plug_tentacle_root.child(TENTACLE_ROOT_NORMAL, &ms);
#if 1
	MDataHandle	handle_tentacle_root = datablock.inputValue(plug_tentacle_root, &ms);
	MDataHandle	handle_tentacle_root_normal = handle_tentacle_root.child(plug_tentacle_root_normal);
	double3&	normal = handle_tentacle_root_normal.asDouble3();
#else
	MObject	data;
	MFnNumericData	val;
	ms = plug_tentacle_root_position.getValue(data);
	ms = val.setObject(data);
	double3		position;
	ms = val.getData(position[0], position[1], position[2]);
	ms = plug_tentacle_root_normal.getValue(data);
	val.setObject(data);
	double3		normal;
	ms = val.getData(normal[0], normal[1], normal[2]);
#endif
#if 1
#if 0
	MPlug	plug_hit_wNormal = plug_hit.child(TENTACLE_ROOT_WNORMAL, &ms);
	MFnNumericData	hit_wNormal_val;
	MObject	hit_wNormal_data = hit_wNormal_val.create(MFnNumericData::k3Double, &ms);
	ms = hit_wNormal_val.setData(wNormal.x, wNormal.y, wNormal.z);
	ms = plug_hit_wNormal.setValue(hit_wNormal_data);
	MVector	wNormal;
#endif
#if 0
	if (__externalDagPathShape.isValid()) {
	MFnMesh	mesh;
	mesh.setObject(__externalDagPathShape);
	int		polyId;
	ms = plug_tentacle_root_polyId.getValue(polyId);
	int		hitTriangleId;
	MPlug	plug_tentacle_root_hitTriangleId = plug_tentacle_root.child(TENTACLE_ROOT_HIT_TRIANGLE_ID, &ms);
	ms = plug_tentacle_root_hitTriangleId.getValue(hitTriangleId);
	float		hitBary1;
	MPlug	plug_tentacle_root_hitBary1 = plug_tentacle_root.child(TENTACLE_ROOT_HIT_BARY1, &ms);
	ms = plug_tentacle_root_hitBary1.getValue(hitBary1);
	float		hitBary2;
	MPlug	plug_tentacle_root_hitBary2 = plug_tentacle_root.child(TENTACLE_ROOT_HIT_BARY2, &ms);
	ms = plug_tentacle_root_hitBary2.getValue(hitBary2);
#if 0
	MObject	data;
	MFnNumericData	val;
	ms = plug_tentacle_root_position.getValue(data);
	ms = val.setObject(data);
	double3		position;
	ms = val.getData(position[0], position[1], position[2]);
	MPoint	hitPoint(position[0], position[1], position[2]);
		MPoint	wPosition;
		MVector	wNormal;
		ms = mesh.getClosestPointAndNormal(hitPoint, wPosition, wNormal, MSpace::kWorld, NULL);
		cout << "hitPoint: " << hitPoint << endl;
		cout << "wPosition: " << wPosition << endl;
		cout << "wNormal: " << wNormal << endl;
#endif
#if 0
		MPoint	wPosition;
		MVector	wNormal;
{
	MDagPath	dagPath = mesh.dagPath(&ms);
	MItMeshPolygon	polyIter(dagPath, MObject::kNullObj, &ms);
	cout << ms << endl;
	int	prevIndex;
	ms = polyIter.setIndex(polyId, prevIndex);
	MPointArray	wPoints;
	MIntArray	vertexList;
	ms = polyIter.getTriangle(hitTriangleId, wPoints, vertexList, MSpace::kWorld);
	cout << wPoints << endl;
#if 1
{
	MFloatVector	v1(wPoints[0]), v2(wPoints[1]), v3(wPoints[2]);
	cout << "calculated wPosition: " << (hitBary1 * v1 + hitBary2 * v2 + (1 - hitBary1 - hitBary2) * v3) << endl;
}
#endif
#if 1
{
#if 0
	MFloatVector	v12 = v2 - v1, v13 = v3 - v1;
	v12.normalize(); v13.normalize();
	MFloatVector	v = v12 ^ v13;
	v.normalize();
	cout << v << endl;
	MString	s("tilt: ");
	s += v * wNormal;
	s += " (";
	s += acos(v * wNormal) * 180 / M_PI;
	s += ")";
	s += ": isPlanar? ";
	s += polyIter.isPlanar() ? "True" : "False";
	MGlobal::displayWarning(s);
	cout << s << endl;
#endif
#if 1
#if 1
{
	int	numTriangles;
	ms = polyIter.numTriangles(numTriangles);
	MVector	n = MVector::zero;
	for (int k = 0; k < numTriangles; ++k) {
		MPointArray	points;
		MIntArray	vList;
		ms = polyIter.getTriangle(k, points, vList, MSpace::kWorld);
		MVector	v1 = points[1] - points[0]; v1.normalize();
		MVector	v2 = points[2] - points[0]; v1.normalize();
		MVector	v = v1 ^ v2; v.normalize();
		if (k && n * v < 0) {
			cerr << "WARN: normal[" << k << "]: reversed" << endl;
			v = -v;
		}
		n += v;
	}
	n.normalize();
	if (numTriangles && n * wNormal < 0) {
		cerr << "WARN: n: reversed" << endl;
		n = -n;
	}
	cout << "calculated wNormal: " << n << endl;
}
#else
	int	numTriangles;
	ms = polyIter.numTriangles(numTriangles);
	MVector	n = MVector::zero;
	for (int k = 0; k < numTriangles; ++k) {
		MPointArray	points;
		MIntArray	vList;
		ms = polyIter.getTriangle(k, points, vList, MSpace::kWorld);
		MVector	v1 = points[1] - points[0]; v1.normalize();
		MVector	v2 = points[2] - points[0]; v1.normalize();
		MVector	v = v1 ^ v2; v.normalize();
		n += v;
	}
	cout << "calculated wNormal: " << n.normal() << endl;
#endif
#endif
}
#endif
}
#endif
#if 0
	MItMeshFaceVertex	faceVertexIter(dagPath, polyIter.polygon(&ms), &ms);
	for (; !faceVertexIter.isDone(); faceVertexIter.next()) {
		cout << "faceId = " << faceVertexIter.faceId() << ", vertexId = " << faceVertexIter.vertId() << endl;
		MVector	normal;
		ms = faceVertexIter.getNormal(normal, MSpace::kWorld);
		cout << "normal: " << normal << endl;
	}
	cout << ms << endl;
#endif
	}
#endif
#endif
	MVector	transversal;
	transversal = MVector(1, 0, 0).rotateBy(MQuaternion(MVector(0, 0, 1), MVector(normal[0], normal[1], normal[2])));
	MPlug	plug_parameter(thisMObject(), __tentacleParameter);
	MPlug	plug_parameter_length = plug_parameter.child(TENTACLE_PARAM_LENGTH, &ms);
	MPlug	plug_parameter_length_val = plug_parameter_length.child(TENTACLE_ATTRIBUTE_EDITOR_VAL, &ms);
	double	length;
	ms = plug_parameter_length_val.getValue(length);
	double	unit_length = length / (NUM_TENTACLE_SAMPLE_POINTS - 1);
	double	unit_t = unit_length / length;
			MPlug		plug_root = plug.child(TENTACLE_ARRAY_ROOT, &ms);
			__checkStatus(ms, "MPlug::child(unsigned)");
			if (!ms) {
				cerr << ms << endl;
				return MStatus::kSuccess;
			}
			MDataHandle	handle_root = handle.child(plug_root);
#if 1
			ms = handle_root.copy(handle_tentacle_root);
#else
			double3&	root = handle_root.asDouble3();
			root[0] = position[0]; root[1] = position[1]; root[2] = position[2];
#endif
#if 1
#if 1
	MPlug		plug_quaternion = plug.child(TENTACLE_ARRAY_QUATERNION, &ms);
	MDataHandle	handle_quaternion = handle.child(plug_quaternion);
	MQuaternion	q = MQuaternion::identity;
	MPoint		p;
	p.x = q.x; p.y = q.y; p.z = q.z; p.w = q.w;
	MPointArray	quaternion_array(NUM_TENTACLE_SAMPLE_POINTS - 1, p);
	MFnPointArrayData	quaternion_data;
	MObject		quaternion_object = quaternion_data.create(quaternion_array, &ms);
	ms = handle_quaternion.set(quaternion_object);
#else
{
	MPlug		quaternion_plug = plug.child(TENTACLE_ARRAY_QUATERNION, &ms);
	MDataHandle	quaternion_handle = handle.child(quaternion_plug);
	MArrayDataHandle	quaternion_handle_array(quaternion_handle, &ms);
	MArrayDataBuilder	quaternion_handle_array_builder = quaternion_handle_array.builder(&ms);
	for (int i = 0; i < NUM_TENTACLE_SAMPLE_POINTS - 1; ++i) {
		MDataHandle	handle = quaternion_handle_array_builder.addElement(i, &ms);
		MMatrix	m;
		MQuaternion	q;
		m = q.asMatrix();
		handle.set(m);
	}
	ms = quaternion_handle_array.set(quaternion_handle_array_builder);
}
#endif
#endif
			MPlug		plug_longitudinal = plug.child(TENTACLE_ARRAY_LOGITUDINAL, &ms);
			__checkStatus(ms, "MPlug::child(unsigned)");
			MDataHandle	handle_longitudinal = handle.child(plug_longitudinal);
			MVectorArray	longitudinal_array(NUM_TENTACLE_SAMPLE_POINTS - 1, unit_length * MVector(normal));
			MFnVectorArrayData	longitudinal_data;
			MObject		longitudinal_object = longitudinal_data.create(longitudinal_array, &ms);
			__checkStatus(ms, "MFnVectorArrayData::create(MVectorArray&)");
			ms = handle_longitudinal.set(longitudinal_object);
			__checkStatus(ms, "MDataHandle::set()");
			MPlug		plug_transversal = plug.child(TENTACLE_ARRAY_TRANSVERSAL, &ms);
			__checkStatus(ms, "MPlug::child(unsigned)");
			MDataHandle	handle_transversal = handle.child(plug_transversal);
			MVectorArray	transversal_array(NUM_TENTACLE_SAMPLE_POINTS, MVector(transversal));
			MFnVectorArrayData	transversal_data;
			MObject		transversal_object = transversal_data.create(transversal_array, &ms);
			__checkStatus(ms, "MFnVectorArrayData::create(MVectorArray&)");
			ms = handle_transversal.set(transversal_object);
			__checkStatus(ms, "MDataHandle::set()");
	MPlug	plug_parameter_major_radius = plug_parameter.child(TENTACLE_PARAM_CROSS_SECTION_MAJOR_RADIUS, &ms);
	MPlug	plug_parameter_major_radius_val = plug_parameter_major_radius.child(TENTACLE_ATTRIBUTE_EDITOR_VAL, &ms);
	MPlug	plug_parameter_major_radius_scale_array = plug_parameter_major_radius.child(TENTACLE_ATTRIBUTE_EDITOR_RAMP, &ms);
	MDataHandle	handle_parameter = datablock.inputValue(plug_parameter, &ms);
	MDataHandle	handle_parameter_major_radius = handle_parameter.child(plug_parameter_major_radius);
	MDataHandle	handle_parameter_major_radius_ramp = handle_parameter_major_radius.child(plug_parameter_major_radius_scale_array);
			MPlug		plug_a = plug.child(TENTACLE_ARRAY_MAJOR_RADIUS, &ms);
			__checkStatus(ms, "MPlug::child(unsigned)");
			MDataHandle	handle_a = handle.child(plug_a);
			double&		a = handle_a.asDouble();
			a = 1;
	ms = plug_parameter_major_radius_val.getValue(a);
			MPlug		plug_scale_a = plug.child(TENTACLE_ARRAY_MAJOR_RADIUS_SCALE, &ms);
			__checkStatus(ms, "MPlug::child(unsigned)");
			MDataHandle	handle_scale_a = handle.child(plug_scale_a);
			MDoubleArray	scale_a_array(NUM_TENTACLE_SAMPLE_POINTS);
	for (int n = 0; n < NUM_TENTACLE_SAMPLE_POINTS; ++n) {
		float	x = n * (float) unit_t;
		float	y;
		(void) tentacleShape::GetInterpolatedValue(__majorRadiusScaleRampTupleArray, x, y, true);
		scale_a_array[n] = (double) y;
	}
			MFnDoubleArrayData	scale_a_data;
			MObject		scale_a_object = scale_a_data.create(scale_a_array, &ms);
			__checkStatus(ms, "MFnDoubleArrayData::create(MDoubleArray&)");
			ms = handle_scale_a.set(scale_a_object);
			__checkStatus(ms, "MDataHandle::set()");
			MPlug	plug_ramp_a = plug.child(TENTACLE_ARRAY_MAJOR_RADIUS_RAMP, &ms);
			MDataHandle	handle_ramp_a = handle.child(plug_ramp_a);
#if 1
			ms = handle_ramp_a.copy(handle_parameter_major_radius_ramp);
#else
			copyRamp(plug_parameter_major_radius_scale_array, handle_parameter_major_radius_ramp, plug_ramp_a, handle_ramp_a, datablock, true);
#endif
	MPlug	plug_parameter_eccentricity = plug_parameter.child(TENTACLE_PARAM_CROSS_SECTION_ECCENTRICITY, &ms);
	MPlug	plug_parameter_eccentricity_val = plug_parameter_eccentricity.child(TENTACLE_ATTRIBUTE_EDITOR_VAL, &ms);
	MPlug	plug_parameter_eccentricity_scale_array = plug_parameter_eccentricity.child(TENTACLE_ATTRIBUTE_EDITOR_RAMP, &ms);
	MDataHandle	handle_parameter_eccentricity = handle_parameter.child(plug_parameter_eccentricity);
	MDataHandle	handle_parameter_eccentricity_ramp = handle_parameter_eccentricity.child(plug_parameter_eccentricity_scale_array);
			MPlug		plug_e = plug.child(TENTACLE_ARRAY_ECCENTRICITY, &ms);
			__checkStatus(ms, "MPlug::child(unsigned)");
			MDataHandle	handle_e = handle.child(plug_e);
			double&		e = handle_e.asDouble();
			e = 0;
	ms = plug_parameter_eccentricity_val.getValue(e);
			MPlug		plug_scale_e = plug.child(TENTACLE_ARRAY_ECCENTRICITY_SCALE, &ms);
			__checkStatus(ms, "MPlug::child(unsigned)");
			MDataHandle	handle_scale_e = handle.child(plug_scale_e);
			MDoubleArray	scale_e_array(NUM_TENTACLE_SAMPLE_POINTS);
	for (int n = 0; n < NUM_TENTACLE_SAMPLE_POINTS; ++n) {
		float	x = n * (float) unit_t;
		float	y;
		(void) tentacleShape::GetInterpolatedValue(__eccentricityScaleRampTupleArray, x, y, true);
		scale_e_array[n] = (double) y;
	}
			MFnDoubleArrayData	scale_e_data;
			MObject		scale_e_object = scale_e_data.create(scale_e_array, &ms);
			__checkStatus(ms, "MFnDoubleArrayData::create(MDoubleArray&)");
			ms = handle_scale_e.set(scale_e_object);
			__checkStatus(ms, "MDataHandle::set()");
			MPlug	plug_ramp_e = plug.child(TENTACLE_ARRAY_ECCENTRICITY_RAMP, &ms);
			MDataHandle	handle_ramp_e = handle.child(plug_ramp_e);
			ms = handle_ramp_e.copy(handle_parameter_eccentricity_ramp);
	MPlug	plug_parameter_rotation = plug_parameter.child(TENTACLE_PARAM_CROSS_SECTION_ROTATION, &ms);
	MPlug	plug_parameter_rotation_val = plug_parameter_rotation.child(TENTACLE_ATTRIBUTE_EDITOR_VAL, &ms);
	MPlug	plug_parameter_rotation_local_array = plug_parameter_rotation.child(TENTACLE_ATTRIBUTE_EDITOR_RAMP, &ms);
	MDataHandle	handle_parameter_rotation = handle_parameter.child(plug_parameter_rotation);
	MDataHandle	handle_parameter_rotation_ramp = handle_parameter_rotation.child(plug_parameter_rotation_local_array);
			MPlug		plug_rotation = plug.child(TENTACLE_ARRAY_ROTATION, &ms);
			__checkStatus(ms, "MPlug::child(unsigned)");
			MDataHandle	handle_rotation = handle.child(plug_rotation);
			double&		rotation = handle_rotation.asDouble();
			rotation = 0;
	ms = plug_parameter_rotation_val.getValue(rotation);
			rotation *= M_PI / 180;
			MPlug		plug_local_rotation = plug.child(TENTACLE_ARRAY_ROTATION_LOCAL, &ms);
			__checkStatus(ms, "MPlug::child(unsigned)");
			MDataHandle	handle_local_rotation = handle.child(plug_local_rotation);
			MDoubleArray	local_rotation_array(NUM_TENTACLE_SAMPLE_POINTS);
	for (int n = 0; n < NUM_TENTACLE_SAMPLE_POINTS; ++n) {
		float	x = n * (float) unit_t;
		float	y;
		(void) tentacleShape::GetInterpolatedValue(__rotationLocalRampTupleArray, x, y, true);
		local_rotation_array[n] = ((double) y - 0.5) * 2 * M_PI;
	}
			MFnDoubleArrayData	local_rotation_data;
			MObject		local_rotation_object = local_rotation_data.create(local_rotation_array, &ms);
			__checkStatus(ms, "MFnDoubleArrayData::create(MDoubleArray&)");
			ms = handle_local_rotation.set(local_rotation_object);
			__checkStatus(ms, "MDataHandle::set()");
			MPlug	plug_ramp_local = plug.child(TENTACLE_ARRAY_ROTATION_RAMP, &ms);
			MDataHandle	handle_ramp_local = handle.child(plug_ramp_local);
			ms = handle_ramp_local.copy(handle_parameter_rotation_ramp);
	MPlug	plug_parameter_cut = plug_parameter.child(TENTACLE_PARAM_CUT, &ms);
	MPlug	plug_parameter_cut_param = plug_parameter_cut.child(TENTACLE_ATTRIBUTE_EDITOR_VAL, &ms);
			MPlug		plug_cut = plug.child(TENTACLE_ARRAY_CUT, &ms);
			__checkStatus(ms, "MPlug::child(unsigned)");
			MDataHandle	handle_cut = handle.child(plug_cut);
#if 1
	MDataHandle	handle_parameter_cut = handle_parameter.child(plug_parameter_cut);
	MDataHandle	handle_parameter_cut_param = handle_parameter_cut.child(plug_parameter_cut_param);
			handle_cut.copy(handle_parameter_cut_param);
#else
			MPlug		plug_cut_have = plug_cut.child(0, &ms);
			MDataHandle	handle_cut_have = handle_cut.child(plug_cut_have);
			__checkStatus(ms, "MPlug::child(unsigned)");
			bool&		have = handle_cut_have.asBool();
			have = false;
			MPlug		plug_cut_t = plug_cut.child(1, &ms);
			__checkStatus(ms, "MPlug::child(unsigned)");
			MDataHandle	handle_cut_t = handle_cut.child(plug_cut_t);
			double&		t = handle_cut_t.asDouble();
			t = 0.5;
			MPlug		plug_cut_polar = plug_cut.child(2, &ms);
			__checkStatus(ms, "MPlug::child(unsigned)");
			MDataHandle	handle_cut_polar = handle_cut.child(plug_cut_polar);
			double&		polar = handle_cut_polar.asDouble();
			polar = 0;
			MPlug		plug_cut_azimuthal = plug_cut.child(3, &ms);
			__checkStatus(ms, "MPlug::child(unsigned)");
			MDataHandle	handle_cut_azimuthal = handle_cut.child(plug_cut_azimuthal);
			double&		azimuthal = handle_cut_azimuthal.asDouble();
			azimuthal = 0;
#endif
#endif
			outputArray.set(builder);
			outputArray.setAllClean();
#if 1
{
		MStatus	ms;
#if 0
	MString	command("createNode transform");
	MString	meshTransformNode;
	ms = MGlobal::executeCommand(command, meshTransformNode, true);
	cout << ms << endl;
	cout << "|" << meshTransformNode << "|" << endl;
	command = "createNode mesh -p ";
	command += meshTransformNode;
	MString	meshShapeNode;
	ms = MGlobal::executeCommand(command, meshShapeNode, true);
	cout << ms << endl;
	cout << "|" << meshShapeNode << "|" << endl;
	command = "sets -add initialShadingGroup ";
	command += meshShapeNode;
	MString	shadingEngineNode;
	ms = MGlobal::executeCommand(command, shadingEngineNode, true);
	cout << ms << endl;
	cout << "|" << shadingEngineNode << "|" << endl;
#if 0
	command = "connectAttr m1.outputMesh ";
	command += meshShapeNode;
	command += ".inMesh";
	MString	result;
	ms = MGlobal::executeCommand(command, result, true);
	cout << ms << endl;
	cout << "|" << result << "|" << endl;
#endif
#endif
#if 1
#if 1
			MArrayDataHandle	outputArray = datablock.outputArrayValue(outputMesh);
			unsigned	numElements = outputArray.elementCount();
			MArrayDataBuilder	builder(&datablock, outputMesh, numElements + 1);
			for (int physicalIndex = 0; physicalIndex < (int) numElements; ++physicalIndex) {
#if MAYA_API_VERSION >= 200806
				ms = outputArray.jumpToArrayElement(physicalIndex);
#else
				ms = outputArray.jumpToElement(physicalIndex);
#endif
				MDataHandle	handle = outputArray.inputValue();
				MDataHandle	outputHandle = builder.addElement(physicalIndex);
				outputHandle.copy(handle);
			}
			MDataHandle	handle = builder.addLast(&ms);
#endif
		MDataHandle&	outputHandle = handle;
#else
		MDataHandle	outputHandle = datablock.outputValue(outputMesh, &ms);
#endif
		if (!ms) {
			ms.perror("tentacleShape::compute");
			return ms;
		}
		MFnMeshData	dataCreator;
		MObject	newOutputData = dataCreator.create(&ms);
		if (!ms) {
			ms.perror("tentacleShape::compute");
			return ms;
		}
		tentacleCreateMesh(newOutputData, ms);
		if (!ms) {
			ms.perror("tentacleShape::compute");
			return ms;
		}
		outputHandle.set(newOutputData);
#if 1
{
	for (int i = 0; i < 2; ++i) {
		MDataHandle	handle = builder.addLast(&ms);
		MDataHandle&	outputHandle = handle;
		if (!ms) {
			ms.perror("tentacleShape::compute: TentacleDo::Add: MANUAL_CUT");
			return ms;
		}
		MFnMeshData	dataCreator;
		MObject	newOutputData = dataCreator.create(&ms);
		if (!ms) {
			ms.perror("tentacleShape::compute: TentacleDo::Add: MANUAL_CUT");
			return ms;
		}
		tentacleCreateMesh(newOutputData, ms);
		if (!ms) {
			ms.perror("tentacleShape::compute: TentacleDo::Add: MANUAL_CUT");
			return ms;
		}
		outputHandle.set(newOutputData);
	}
}
#endif
#if 1
			outputArray.set(builder);
			outputArray.setAllClean();
#else
#endif
#if 0
	updateConnection = true;
#endif
}
#endif
#if 1
			assert(__externalDagPathShape.isValid(&ms));
			if (__externalDagPathShape.isValid(&ms)) {
				MDagPath	tentacleDagPathShape;
				ms = findTentacleShapeNode(__externalDagPathShape.node(), tentacleDagPathShape);
				MFnSingleIndexedComponent	fnEdgeComp;
				MObject	edgeComp = fnEdgeComp.create(MFn::kMeshEdgeComponent, &ms);
				ms = fnEdgeComp.addElement(numElements);
				lastSelectedPhysicalIndex = numElements;
				MSelectionList	list;
				ms = list.add(tentacleDagPathShape, edgeComp);
				ms = MGlobal::setActiveSelectionList(list);
			}
#endif
#if 0
{
			MArrayDataHandle	outputArrayArray = datablock.outputArrayValue(__tentacleArrayArray);
			unsigned	numElements = outputArrayArray.elementCount();
			MArrayDataBuilder	builderArray(&datablock, __tentacleArrayArray, numElements + 1);
			for (int physicalIndex = 0; physicalIndex < (int) numElements; ++physicalIndex) {
#if MAYA_API_VERSION >= 200806
				ms = outputArrayArray.jumpToArrayElement(physicalIndex);
#else
				ms = outputArrayArray.jumpToElement(physicalIndex);
#endif
				MDataHandle	handleArray = outputArrayArray.inputValue();
				MDataHandle	outputHandleArray = builderArray.addElement(physicalIndex);
				outputHandleArray.copy(handleArray);
			}
			MDataHandle	handleArray = builderArray.addLast(&ms);
#if 1
{
			MPlug	plug_tentacleArrayArray(thisMObject(), __tentacleArrayArray);
			MPlug	plug_simulation_parameter = plug_tentacleArrayArray.child(TENTACLE_ARRAY_ARRAY_SIMULATION_PARAMETER, &ms);
			MDataHandle	handle_simulation_parameter = handleArray.child(plug_simulation_parameter);
			MPlug	plug_parameter(thisMObject(), __tentacleParameter);
			MDataHandle	handle_parameter = datablock.inputValue(plug_parameter, &ms);
			ms = handle_simulation_parameter.copy(handle_parameter);
}
#endif
#if 1
{
			MPlug	plug_tentacleArrayArray(thisMObject(), __tentacleArrayArray);
			MPlug	plug_simulation_output = plug_tentacleArrayArray.child(TENTACLE_ARRAY_ARRAY_SIMULATION_OUTPUT, &ms);
			MDataHandle	handle_simulation_output = handleArray.child(plug_simulation_output);
			MPlug	plug_output(thisMObject(), __tentacleArray);
			MDataHandle	handle_output = datablock.inputValue(plug_output, &ms);
			ms = handle_simulation_output.copy(handle_output);
}
#endif
			outputArrayArray.set(builderArray);
			outputArrayArray.setAllClean();
#if 0
{
			MPlug	plug_tentacleArrayArray(thisMObject(), __tentacleArrayArray);
			MArrayDataHandle	handleArrayArray = datablock.outputArrayValue(plug_tentacleArrayArray, &ms);
			cout << ms << endl;
			unsigned	numElements = handleArrayArray.elementCount(&ms);
			cout << numElements << endl;
		if (numElements) {
#if MAYA_API_VERSION >= 200806
			ms = handleArrayArray.jumpToArrayElement(numElements - 1);
#else
			ms = handleArrayArray.jumpToElement(numElements - 1);
#endif
			MDataHandle	handle_parameter_output = handleArrayArray.outputValue();
#if 1
{
			MPlug	plug_simulation_output = plug_tentacleArrayArray.child(TENTACLE_ARRAY_ARRAY_SIMULATION_OUTPUT, &ms);
			cout << ms << endl;
			MDataHandle	handle_simulation_output = handle_parameter_output.child(plug_simulation_output);
			MArrayDataHandle	handleArray(handle_simulation_output, &ms);
			cout << ms << endl;
			for (int n = 0; n < (int) handleArray.elementCount(); ++n) {
#if MAYA_API_VERSION >= 200806
				ms = handleArray.jumpToArrayElement(n);
#else
				ms = handleArray.jumpToElement(n);
#endif
				MDataHandle	handle = handleArray.outputValue(&ms);
				cout << ms << endl;
#if 0
{
		MPlug		plug_longitudinal = plug.child(TENTACLE_ARRAY_LOGITUDINAL, &ms);
		MDataHandle	handle_longitudinal = handle.child(plug_longitudinal);
		MObject		longitudinalData = handle_longitudinal.data();
		MFnVectorArrayData	vectorData(longitudinalData, &ms);
		MVectorArray	longitudinals = vectorData.array(&ms);
	double	length0 = (NUM_TENTACLE_SAMPLE_POINTS - 1) * longitudinals[1].length();
	cout << "length0 = " << length0 << endl;
		longitudinals[0] = MVector(1, 1, 1);
#if 0
{
			MVectorArray	longitudinal_array(longitudinals.length());
			for (unsigned n = 0; n < longitudinals.length(); ++n)
				longitudinal_array[n] = longitudinals[n];
			MFnVectorArrayData	longitudinal_data;
			MObject		longitudinal_object = longitudinal_data.create(longitudinal_array, &ms);
			__checkStatus(ms, "MFnVectorArrayData::create(MVectorArray&)");
			ms = handle_longitudinal.set(longitudinal_object);
			__checkStatus(ms, "MDataHandle::set()");
			cout << ms << endl;
}
#endif
}
#endif
			}
}
#endif
		}
}
#endif
}
#endif
		}
		if (action == TentacleDo::Update && hasActiveComponents()) {
			int	iSelectBy;
			tentacleShape*	nonConstThis = (tentacleShape*) this;
			nonConstThis->getSelectBy(iSelectBy);
			MObjectArray	list = activeComponents();
			cout << "# of selected objects = " << list.length() << endl;
			MIntArray	indexArray;
			if (iSelectBy == selectByVertex && list[0].hasFn(MFn::kDoubleIndexedComponent) && list[0].hasFn(MFn::kSurfaceCVComponent)) {
				cout << "MFnDoubleIndexedComponent:" << endl;
				MFnDoubleIndexedComponent	fnComponent(list[0], &ms);
				for (int i = 0; i < fnComponent.elementCount(&ms); ++i) {
					int	uIndex;
					int	vIndex;
					ms = fnComponent.getElement(i, uIndex, vIndex);
					cout << "\t[" << i << "]: (" << uIndex << ", " << vIndex << ")" << endl;
					indexArray.append(uIndex);
				}
			} else if (iSelectBy == selectByGuide && list[0].hasFn(MFn::kSingleIndexedComponent) && list[0].hasFn(MFn::kMeshEdgeComponent)) {
				cout << "MFnSingleIndexedComponent:" << endl;
				MFnSingleIndexedComponent	fnComponent(list[0], &ms);
				cout << "# of single-indexed components = " << fnComponent.elementCount(&ms) << endl;
				for (int i = 0; i < fnComponent.elementCount(&ms); ++i) {
					int	index = fnComponent.element(i, &ms);
					cout << "\t[" << i << "]: physical index = " << index << endl;
					indexArray.append(index);
				}
			}
			MArrayDataHandle	outputArray = datablock.outputArrayValue(__tentacleArray);
			unsigned	numElements = outputArray.elementCount();
			cout << "# of ... = " << numElements << " (" << ms << "), indices = " << indexArray << endl;
			MArrayDataBuilder	builder(&datablock, __tentacleArray, numElements);
			for (int physicalIndex = 0; physicalIndex < (int) numElements; ++physicalIndex) {
#if MAYA_API_VERSION >= 200806
				ms = outputArray.jumpToArrayElement(physicalIndex);
#else
				ms = outputArray.jumpToElement(physicalIndex);
#endif
				unsigned	logicalIndex = outputArray.elementIndex(&ms); assert(ms);
				cout << "physical index = " << physicalIndex << "; logical index = " << logicalIndex << endl;
				MDataHandle	srcHandle = outputArray.inputValue();
				MDataHandle	handle = builder.addElement(physicalIndex);
				bool	found = false;
				for (int i = 0; i < (int) indexArray.length(); ++i) {
					if (indexArray[i] == (int) physicalIndex) {
						found = true;
						break;
					}
				}
				handle.copy(srcHandle);
				if (!found) {
					continue;
				}
#if 1
#if 0
	MPlug	plug_tentacle_root(thisMObject(), __tentacleRoot);
	MPlug	plug_tentacle_root_polyId = plug_tentacle_root.child(TENTACLE_ROOT_POLY_ID, &ms);
	MPlug	plug_tentacle_root_position = plug_tentacle_root.child(TENTACLE_ROOT_POSITION, &ms);
	MPlug	plug_tentacle_root_normal = plug_tentacle_root.child(TENTACLE_ROOT_NORMAL, &ms);
	MObject	data;
	MFnNumericData	val;
	ms = plug_tentacle_root_position.getValue(data);
	ms = val.setObject(data);
	double3		position;
	ms = val.getData(position[0], position[1], position[2]);
	ms = plug_tentacle_root_normal.getValue(data);
	val.setObject(data);
	double3		normal;
	ms = val.getData(normal[0], normal[1], normal[2]);
	MVector	transversal;
	transversal = MVector(1, 0, 0).rotateBy(MQuaternion(MVector(0, 0, 1), MVector(normal[0], normal[1], normal[2])));
#endif
	MPlug	plug_parameter(thisMObject(), __tentacleParameter);
	MPlug	plug_parameter_length = plug_parameter.child(TENTACLE_PARAM_LENGTH, &ms);
	MPlug	plug_parameter_length_val = plug_parameter_length.child(TENTACLE_ATTRIBUTE_EDITOR_VAL, &ms);
	double	length;
	ms = plug_parameter_length_val.getValue(length);
	double	unit_length = length / (NUM_TENTACLE_SAMPLE_POINTS - 1);
	double	unit_t = unit_length / length;
#if 0
			MPlug		plug_root = plug.child(TENTACLE_ARRAY_ROOT, &ms);
			__checkStatus(ms, "MPlug::child(unsigned)");
			MDataHandle	handle_root = handle.child(plug_root);
			double3&	root = handle_root.asDouble3();
			root[0] = position[0]; root[1] = position[1]; root[2] = position[2];
			MPlug		plug_longitudinal = plug.child(TENTACLE_ARRAY_LOGITUDINAL, &ms);
			__checkStatus(ms, "MPlug::child(unsigned)");
			MDataHandle	handle_longitudinal = handle.child(plug_longitudinal);
			MVectorArray	longitudinal_array(NUM_TENTACLE_SAMPLE_POINTS - 1, unit_length * MVector(normal));
			MFnVectorArrayData	longitudinal_data;
			MObject		longitudinal_object = longitudinal_data.create(longitudinal_array, &ms);
			__checkStatus(ms, "MFnVectorArrayData::create(MVectorArray&)");
			ms = handle_longitudinal.set(longitudinal_object);
			__checkStatus(ms, "MDataHandle::set()");
			MPlug		plug_transversal = plug.child(TENTACLE_ARRAY_TRANSVERSAL, &ms);
			__checkStatus(ms, "MPlug::child(unsigned)");
			MDataHandle	handle_transversal = handle.child(plug_transversal);
			MVectorArray	transversal_array(NUM_TENTACLE_SAMPLE_POINTS, MVector(transversal));
			MFnVectorArrayData	transversal_data;
			MObject		transversal_object = transversal_data.create(transversal_array, &ms);
			__checkStatus(ms, "MFnVectorArrayData::create(MVectorArray&)");
			ms = handle_transversal.set(transversal_object);
			__checkStatus(ms, "MDataHandle::set()");
#endif
#if 1
			MPlug		plug_longitudinal = plug.child(TENTACLE_ARRAY_LOGITUDINAL, &ms);
			__checkStatus(ms, "MPlug::child(unsigned)");
			MDataHandle	handle_longitudinal = handle.child(plug_longitudinal);
#if 1
			MFnVectorArrayData	vectorData;
			ms = vectorData.setObject(handle_longitudinal.data());
			unsigned	len = vectorData.length();
			MVectorArray	longitudinal_array(len);
			ms = vectorData.copyTo(longitudinal_array);
			for (unsigned n = 0; n < len; ++n) {
				MVector&	longiVector = longitudinal_array[n];
				double	unit_length0 = longiVector.length();
				if (unit_length0 > DBL_EPSILON)
					longiVector *= unit_length / unit_length0;
			}
			MFnVectorArrayData	longitudinal_data;
			MObject		longitudinal_object = longitudinal_data.create(longitudinal_array, &ms);
			__checkStatus(ms, "MFnVectorArrayData::create(MVectorArray&)");
			ms = handle_longitudinal.set(longitudinal_object);
			__checkStatus(ms, "MDataHandle::set()");
#else
			MFnVectorArrayData	vectorData;
			ms = vectorData.setObject(handle_longitudinal.data());
#if 0
	MStatus	ms;
	MFnDoubleArrayData	doubleData;
	MDoubleArray	data(10);
	MObject		valData = doubleData.create(data, &ms);
	MDoubleArray	array1 = doubleData.array(&ms);
	MDoubleArray	array2 = doubleData.array(&ms);
	cout << array1 << endl;
	cout << array2 << endl;
	array1[0] = 1;
	cout << array1 << endl;
	cout << array2 << endl;
#endif
			MVectorArray	longitudinal_array = vectorData.array(&ms);
			for (int k = 0; k < (int) longitudinal_array.length(); ++k) {
				MVector&	longiVector = longitudinal_array[k];
				double	unit_length0 = longiVector.length();
				if (unit_length0 > DBL_EPSILON)
					longiVector *= unit_length / unit_length0;
			}
			ms = handle_longitudinal.set(vectorData.object(&ms));
			__checkStatus(ms, "MDataHandle::set()");
#endif
#endif
	MPlug	plug_parameter_major_radius = plug_parameter.child(TENTACLE_PARAM_CROSS_SECTION_MAJOR_RADIUS, &ms);
	MPlug	plug_parameter_major_radius_val = plug_parameter_major_radius.child(TENTACLE_ATTRIBUTE_EDITOR_VAL, &ms);
	MPlug	plug_parameter_major_radius_scale_array = plug_parameter_major_radius.child(TENTACLE_ATTRIBUTE_EDITOR_RAMP, &ms);
	MDataHandle	handle_parameter = datablock.inputValue(plug_parameter, &ms);
	MDataHandle	handle_parameter_major_radius = handle_parameter.child(plug_parameter_major_radius);
	MDataHandle	handle_parameter_major_radius_ramp = handle_parameter_major_radius.child(plug_parameter_major_radius_scale_array);
			MPlug		plug_a = plug.child(TENTACLE_ARRAY_MAJOR_RADIUS, &ms);
			__checkStatus(ms, "MPlug::child(unsigned)");
			MDataHandle	handle_a = handle.child(plug_a);
			double&		a = handle_a.asDouble();
			a = 1;
	ms = plug_parameter_major_radius_val.getValue(a);
			MPlug		plug_scale_a = plug.child(TENTACLE_ARRAY_MAJOR_RADIUS_SCALE, &ms);
			__checkStatus(ms, "MPlug::child(unsigned)");
			MDataHandle	handle_scale_a = handle.child(plug_scale_a);
			MDoubleArray	scale_a_array(NUM_TENTACLE_SAMPLE_POINTS);
	for (int n = 0; n < NUM_TENTACLE_SAMPLE_POINTS; ++n) {
		float	x = n * (float) unit_t;
		float	y;
		(void) tentacleShape::GetInterpolatedValue(__majorRadiusScaleRampTupleArray, x, y, true);
		scale_a_array[n] = (double) y;
	}
			MFnDoubleArrayData	scale_a_data;
			MObject		scale_a_object = scale_a_data.create(scale_a_array, &ms);
			__checkStatus(ms, "MFnDoubleArrayData::create(MDoubleArray&)");
			ms = handle_scale_a.set(scale_a_object);
			__checkStatus(ms, "MDataHandle::set()");
			MPlug	plug_ramp_a = plug.child(TENTACLE_ARRAY_MAJOR_RADIUS_RAMP, &ms);
			MDataHandle	handle_ramp_a = handle.child(plug_ramp_a);
#if 1
			ms = handle_ramp_a.copy(handle_parameter_major_radius_ramp);
#else
			copyRamp(plug_parameter_major_radius_scale_array, handle_parameter_major_radius_ramp, plug_ramp_a, handle_ramp_a, datablock, true);
#endif
	MPlug	plug_parameter_eccentricity = plug_parameter.child(TENTACLE_PARAM_CROSS_SECTION_ECCENTRICITY, &ms);
	MPlug	plug_parameter_eccentricity_val = plug_parameter_eccentricity.child(TENTACLE_ATTRIBUTE_EDITOR_VAL, &ms);
	MPlug	plug_parameter_eccentricity_scale_array = plug_parameter_eccentricity.child(TENTACLE_ATTRIBUTE_EDITOR_RAMP, &ms);
	MDataHandle	handle_parameter_eccentricity = handle_parameter.child(plug_parameter_eccentricity);
	MDataHandle	handle_parameter_eccentricity_ramp = handle_parameter_eccentricity.child(plug_parameter_eccentricity_scale_array);
	MArrayDataHandle	arrayHandle_parameter_eccentricity_ramp(handle_parameter_eccentricity_ramp, &ms);
			MPlug		plug_e = plug.child(TENTACLE_ARRAY_ECCENTRICITY, &ms);
			__checkStatus(ms, "MPlug::child(unsigned)");
			MDataHandle	handle_e = handle.child(plug_e);
			double&		e = handle_e.asDouble();
			e = 0;
	ms = plug_parameter_eccentricity_val.getValue(e);
			MPlug		plug_scale_e = plug.child(TENTACLE_ARRAY_ECCENTRICITY_SCALE, &ms);
			__checkStatus(ms, "MPlug::child(unsigned)");
			MDataHandle	handle_scale_e = handle.child(plug_scale_e);
			MDoubleArray	scale_e_array(NUM_TENTACLE_SAMPLE_POINTS);
	for (int n = 0; n < NUM_TENTACLE_SAMPLE_POINTS; ++n) {
		float	x = n * (float) unit_t;
		float	y;
		(void) tentacleShape::GetInterpolatedValue(__eccentricityScaleRampTupleArray, x, y, true);
		scale_e_array[n] = (double) y;
	}
			MFnDoubleArrayData	scale_e_data;
			MObject		scale_e_object = scale_e_data.create(scale_e_array, &ms);
			__checkStatus(ms, "MFnDoubleArrayData::create(MDoubleArray&)");
			ms = handle_scale_e.set(scale_e_object);
			__checkStatus(ms, "MDataHandle::set()");
			MPlug	plug_ramp_e = plug.child(TENTACLE_ARRAY_ECCENTRICITY_RAMP, &ms);
			MDataHandle	handle_ramp_e = handle.child(plug_ramp_e);
			MArrayDataHandle	arrayHandle_ramp_e(handle_ramp_e, &ms);
			ms = handle_ramp_e.copy(handle_parameter_eccentricity_ramp);
	MPlug	plug_parameter_rotation = plug_parameter.child(TENTACLE_PARAM_CROSS_SECTION_ROTATION, &ms);
	MPlug	plug_parameter_rotation_val = plug_parameter_rotation.child(TENTACLE_ATTRIBUTE_EDITOR_VAL, &ms);
	MPlug	plug_parameter_rotation_local_array = plug_parameter_rotation.child(TENTACLE_ATTRIBUTE_EDITOR_RAMP, &ms);
	MDataHandle	handle_parameter_rotation = handle_parameter.child(plug_parameter_rotation);
	MDataHandle	handle_parameter_rotation_ramp = handle_parameter_rotation.child(plug_parameter_rotation_local_array);
	MArrayDataHandle	arrayHandle_parameter_rotation_ramp(handle_parameter_rotation_ramp, &ms);
			MPlug		plug_rotation = plug.child(TENTACLE_ARRAY_ROTATION, &ms);
			__checkStatus(ms, "MPlug::child(unsigned)");
			MDataHandle	handle_rotation = handle.child(plug_rotation);
			double&		rotation = handle_rotation.asDouble();
			rotation = 0;
	ms = plug_parameter_rotation_val.getValue(rotation);
			rotation *= M_PI / 180;
			MPlug		plug_local_rotation = plug.child(TENTACLE_ARRAY_ROTATION_LOCAL, &ms);
			__checkStatus(ms, "MPlug::child(unsigned)");
			MDataHandle	handle_local_rotation = handle.child(plug_local_rotation);
			MDoubleArray	local_rotation_array(NUM_TENTACLE_SAMPLE_POINTS);
	for (int n = 0; n < NUM_TENTACLE_SAMPLE_POINTS; ++n) {
		float	x = n * (float) unit_t;
		float	y;
		(void) tentacleShape::GetInterpolatedValue(__rotationLocalRampTupleArray, x, y, true);
		local_rotation_array[n] = ((double) y - 0.5) * 2 * M_PI;
	}
			MFnDoubleArrayData	local_rotation_data;
			MObject		local_rotation_object = local_rotation_data.create(local_rotation_array, &ms);
			__checkStatus(ms, "MFnDoubleArrayData::create(MDoubleArray&)");
			ms = handle_local_rotation.set(local_rotation_object);
			__checkStatus(ms, "MDataHandle::set()");
			MPlug	plug_ramp_local = plug.child(TENTACLE_ARRAY_ROTATION_RAMP, &ms);
			MDataHandle	handle_ramp_local = handle.child(plug_ramp_local);
			MArrayDataHandle	arrayHandle_ramp_local(handle_ramp_local, &ms);
			ms = handle_ramp_local.copy(handle_parameter_rotation_ramp);
	MPlug	plug_parameter_cut = plug_parameter.child(TENTACLE_PARAM_CUT, &ms);
	MPlug	plug_parameter_cut_param = plug_parameter_cut.child(TENTACLE_ATTRIBUTE_EDITOR_VAL, &ms);
			MPlug		plug_cut = plug.child(TENTACLE_ARRAY_CUT, &ms);
			__checkStatus(ms, "MPlug::child(unsigned)");
			MDataHandle	handle_cut = handle.child(plug_cut);
#if 1
	MDataHandle	handle_parameter_cut = handle_parameter.child(plug_parameter_cut);
	MDataHandle	handle_parameter_cut_param = handle_parameter_cut.child(plug_parameter_cut_param);
			handle_cut.copy(handle_parameter_cut_param);
#else
			MPlug		plug_cut_have = plug_cut.child(0, &ms);
			MDataHandle	handle_cut_have = handle_cut.child(plug_cut_have);
			__checkStatus(ms, "MPlug::child(unsigned)");
			bool&		have = handle_cut_have.asBool();
			have = false;
			MPlug		plug_cut_t = plug_cut.child(1, &ms);
			__checkStatus(ms, "MPlug::child(unsigned)");
			MDataHandle	handle_cut_t = handle_cut.child(plug_cut_t);
			double&		t = handle_cut_t.asDouble();
			t = 0.5;
			MPlug		plug_cut_polar = plug_cut.child(2, &ms);
			__checkStatus(ms, "MPlug::child(unsigned)");
			MDataHandle	handle_cut_polar = handle_cut.child(plug_cut_polar);
			double&		polar = handle_cut_polar.asDouble();
			polar = 0;
			MPlug		plug_cut_azimuthal = plug_cut.child(3, &ms);
			__checkStatus(ms, "MPlug::child(unsigned)");
			MDataHandle	handle_cut_azimuthal = handle_cut.child(plug_cut_azimuthal);
			double&		azimuthal = handle_cut_azimuthal.asDouble();
			azimuthal = 0;
#endif
#endif
			}
			outputArray.set(builder);
			outputArray.setAllClean();
#if 1
			ms = MUserEventMessage::postUserEvent(TentacleArrayUpdated);
#endif
		}
		if (action == TentacleDo::Move && hasActiveComponents()) {
			MIntArray	indexArray;
#if 1
			indexArray.append(lastSelectedPhysicalIndex);
			if (lastSelectedPhysicalIndex == -1) {
				MGlobal::displayError("TentacleDo::Move: lastSelectedPhysicalIndex = -1");
			}
#else
			int	iSelectBy;
			tentacleShape*	nonConstThis = (tentacleShape*) this;
			nonConstThis->getSelectBy(iSelectBy);
			MObjectArray	list = activeComponents();
			cout << "# of selected objects = " << list.length() << endl;
			if (iSelectBy == selectByVertex && list[0].hasFn(MFn::kDoubleIndexedComponent) && list[0].hasFn(MFn::kSurfaceCVComponent)) {
				cout << "MFnDoubleIndexedComponent:" << endl;
				MFnDoubleIndexedComponent	fnComponent(list[0], &ms);
				for (int i = 0; i < fnComponent.elementCount(&ms); ++i) {
					int	uIndex;
					int	vIndex;
					ms = fnComponent.getElement(i, uIndex, vIndex);
					cout << "\t[" << i << "]: (" << uIndex << ", " << vIndex << ")" << endl;
					indexArray.append(uIndex);
				}
			} else if (iSelectBy == selectByGuide && list[0].hasFn(MFn::kSingleIndexedComponent) && list[0].hasFn(MFn::kMeshEdgeComponent)) {
				cout << "MFnSingleIndexedComponent:" << endl;
				MFnSingleIndexedComponent	fnComponent(list[0], &ms);
				cout << "# of single-indexed components = " << fnComponent.elementCount(&ms) << endl;
				for (int i = 0; i < fnComponent.elementCount(&ms); ++i) {
					int	index = fnComponent.element(i, &ms);
					cout << "\t[" << i << "]: physical index = " << index << endl;
					indexArray.append(index);
				}
			}
#endif
			MArrayDataHandle	outputArray = datablock.outputArrayValue(__tentacleArray);
			unsigned	numElements = outputArray.elementCount();
			cout << "# of ... = " << numElements << " (" << ms << "), indices = " << indexArray << endl;
			MArrayDataBuilder	builder(&datablock, __tentacleArray, numElements);
			for (int physicalIndex = 0; physicalIndex < (int) numElements; ++physicalIndex) {
#if MAYA_API_VERSION >= 200806
				ms = outputArray.jumpToArrayElement(physicalIndex);
#else
				ms = outputArray.jumpToElement(physicalIndex);
#endif
				unsigned	logicalIndex = outputArray.elementIndex(&ms); assert(ms);
				cout << "physical index = " << physicalIndex << "; logical index = " << logicalIndex << endl;
				MDataHandle	srcHandle = outputArray.inputValue();
				MDataHandle	handle = builder.addElement(physicalIndex);
				bool	found = false;
				for (int i = 0; i < (int) indexArray.length(); ++i) {
					if (indexArray[i] == (int) physicalIndex) {
						found = true;
						break;
					}
				}
				handle.copy(srcHandle);
				if (!found) {
					continue;
				}
#if 1
	MPlug	plug_tentacle_root(thisMObject(), __tentacleRoot);
	MPlug	plug_tentacle_root_polyId = plug_tentacle_root.child(TENTACLE_ROOT_POLY_ID, &ms);
	MPlug	plug_tentacle_root_position = plug_tentacle_root.child(TENTACLE_ROOT_POSITION, &ms);
	MPlug	plug_tentacle_root_normal = plug_tentacle_root.child(TENTACLE_ROOT_NORMAL, &ms);
#if 1
	MDataHandle	handle_tentacle_root = datablock.inputValue(plug_tentacle_root, &ms);
#else
	MObject	data;
	MFnNumericData	val;
	ms = plug_tentacle_root_position.getValue(data);
	ms = val.setObject(data);
	double3		position;
	ms = val.getData(position[0], position[1], position[2]);
#endif
#if 0
	ms = plug_tentacle_root_normal.getValue(data);
	val.setObject(data);
	double3		normal;
	ms = val.getData(normal[0], normal[1], normal[2]);
	MVector	transversal;
	transversal = MVector(1, 0, 0).rotateBy(MQuaternion(MVector(0, 0, 1), MVector(normal[0], normal[1], normal[2])));
	MPlug	plug_parameter(thisMObject(), __tentacleParameter);
	MPlug	plug_parameter_length = plug_parameter.child(TENTACLE_PARAM_LENGTH, &ms);
	MPlug	plug_parameter_length_val = plug_parameter_length.child(TENTACLE_ATTRIBUTE_EDITOR_VAL, &ms);
	double	length;
	ms = plug_parameter_length_val.getValue(length);
	double	unit_length = length / (NUM_TENTACLE_SAMPLE_POINTS - 1);
	double	unit_t = unit_length / length;
#endif
			MPlug		plug_root = plug.child(TENTACLE_ARRAY_ROOT, &ms);
			__checkStatus(ms, "MPlug::child(unsigned)");
			MDataHandle	handle_root = handle.child(plug_root);
#if 1
			ms = handle_root.copy(handle_tentacle_root);
#else
			double3&	root = handle_root.asDouble3();
			root[0] = position[0]; root[1] = position[1]; root[2] = position[2];
#endif
#if 0
			MPlug		plug_longitudinal = plug.child(TENTACLE_ARRAY_LOGITUDINAL, &ms);
			__checkStatus(ms, "MPlug::child(unsigned)");
			MDataHandle	handle_longitudinal = handle.child(plug_longitudinal);
			MVectorArray	longitudinal_array(NUM_TENTACLE_SAMPLE_POINTS - 1, unit_length * MVector(normal));
			MFnVectorArrayData	longitudinal_data;
			MObject		longitudinal_object = longitudinal_data.create(longitudinal_array, &ms);
			__checkStatus(ms, "MFnVectorArrayData::create(MVectorArray&)");
			ms = handle_longitudinal.set(longitudinal_object);
			__checkStatus(ms, "MDataHandle::set()");
			MPlug		plug_transversal = plug.child(TENTACLE_ARRAY_TRANSVERSAL, &ms);
			__checkStatus(ms, "MPlug::child(unsigned)");
			MDataHandle	handle_transversal = handle.child(plug_transversal);
			MVectorArray	transversal_array(NUM_TENTACLE_SAMPLE_POINTS, MVector(transversal));
			MFnVectorArrayData	transversal_data;
			MObject		transversal_object = transversal_data.create(transversal_array, &ms);
			__checkStatus(ms, "MFnVectorArrayData::create(MVectorArray&)");
			ms = handle_transversal.set(transversal_object);
			__checkStatus(ms, "MDataHandle::set()");
#endif
#if 0
			MPlug		plug_longitudinal = plug.child(TENTACLE_ARRAY_LOGITUDINAL, &ms);
			__checkStatus(ms, "MPlug::child(unsigned)");
			MDataHandle	handle_longitudinal = handle.child(plug_longitudinal);
			MFnVectorArrayData	vectorData;
			ms = vectorData.setObject(handle_longitudinal.data());
#if 0
	MStatus	ms;
	MFnDoubleArrayData	doubleData;
	MDoubleArray	data(10);
	MObject		valData = doubleData.create(data, &ms);
	MDoubleArray	array1 = doubleData.array(&ms);
	MDoubleArray	array2 = doubleData.array(&ms);
	cout << array1 << endl;
	cout << array2 << endl;
	array1[0] = 1;
	cout << array1 << endl;
	cout << array2 << endl;
#endif
			MVectorArray	longitudinal_array = vectorData.array(&ms);
			for (int k = 0; k < (int) longitudinal_array.length(); ++k) {
				MVector&	longiVector = longitudinal_array[k];
				double	unit_length0 = longiVector.length();
				if (unit_length0 > DBL_EPSILON)
					longiVector *= unit_length / unit_length0;
			}
			ms = handle_longitudinal.set(vectorData.object(&ms));
			__checkStatus(ms, "MDataHandle::set()");
#endif
#if 0
	MPlug	plug_parameter_major_radius = plug_parameter.child(TENTACLE_PARAM_CROSS_SECTION_MAJOR_RADIUS, &ms);
	MPlug	plug_parameter_major_radius_val = plug_parameter_major_radius.child(TENTACLE_ATTRIBUTE_EDITOR_VAL, &ms);
	MPlug	plug_parameter_major_radius_scale_array = plug_parameter_major_radius.child(TENTACLE_ATTRIBUTE_EDITOR_RAMP, &ms);
	MDataHandle	handle_parameter = datablock.inputValue(plug_parameter, &ms);
	MDataHandle	handle_parameter_major_radius = handle_parameter.child(plug_parameter_major_radius);
	MDataHandle	handle_parameter_major_radius_ramp = handle_parameter_major_radius.child(plug_parameter_major_radius_scale_array);
			MPlug		plug_a = plug.child(TENTACLE_ARRAY_MAJOR_RADIUS, &ms);
			__checkStatus(ms, "MPlug::child(unsigned)");
			MDataHandle	handle_a = handle.child(plug_a);
			double&		a = handle_a.asDouble();
			a = 1;
	ms = plug_parameter_major_radius_val.getValue(a);
			MPlug		plug_scale_a = plug.child(TENTACLE_ARRAY_MAJOR_RADIUS_SCALE, &ms);
			__checkStatus(ms, "MPlug::child(unsigned)");
			MDataHandle	handle_scale_a = handle.child(plug_scale_a);
			MDoubleArray	scale_a_array(NUM_TENTACLE_SAMPLE_POINTS);
	for (int n = 0; n < NUM_TENTACLE_SAMPLE_POINTS; ++n) {
		float	x = n * (float) unit_t;
		float	y;
		assert(tentacleShape::GetInterpolatedValue(__majorRadiusScaleRampTupleArray, x, y, true) == 0);
		scale_a_array[n] = (double) y;
	}
			MFnDoubleArrayData	scale_a_data;
			MObject		scale_a_object = scale_a_data.create(scale_a_array, &ms);
			__checkStatus(ms, "MFnDoubleArrayData::create(MDoubleArray&)");
			ms = handle_scale_a.set(scale_a_object);
			__checkStatus(ms, "MDataHandle::set()");
			MPlug	plug_ramp_a = plug.child(TENTACLE_ARRAY_MAJOR_RADIUS_RAMP, &ms);
			MDataHandle	handle_ramp_a = handle.child(plug_ramp_a);
#if 1
			ms = handle_ramp_a.copy(handle_parameter_major_radius_ramp);
#else
			copyRamp(plug_parameter_major_radius_scale_array, handle_parameter_major_radius_ramp, plug_ramp_a, handle_ramp_a, datablock, true);
#endif
	MPlug	plug_parameter_eccentricity = plug_parameter.child(TENTACLE_PARAM_CROSS_SECTION_ECCENTRICITY, &ms);
	MPlug	plug_parameter_eccentricity_val = plug_parameter_eccentricity.child(TENTACLE_ATTRIBUTE_EDITOR_VAL, &ms);
	MPlug	plug_parameter_eccentricity_scale_array = plug_parameter_eccentricity.child(TENTACLE_ATTRIBUTE_EDITOR_RAMP, &ms);
	MDataHandle	handle_parameter_eccentricity = handle_parameter.child(plug_parameter_eccentricity);
	MDataHandle	handle_parameter_eccentricity_ramp = handle_parameter_eccentricity.child(plug_parameter_eccentricity_scale_array);
	MArrayDataHandle	arrayHandle_parameter_eccentricity_ramp(handle_parameter_eccentricity_ramp, &ms);
			MPlug		plug_e = plug.child(TENTACLE_ARRAY_ECCENTRICITY, &ms);
			__checkStatus(ms, "MPlug::child(unsigned)");
			MDataHandle	handle_e = handle.child(plug_e);
			double&		e = handle_e.asDouble();
			e = 0;
	ms = plug_parameter_eccentricity_val.getValue(e);
			MPlug		plug_scale_e = plug.child(TENTACLE_ARRAY_ECCENTRICITY_SCALE, &ms);
			__checkStatus(ms, "MPlug::child(unsigned)");
			MDataHandle	handle_scale_e = handle.child(plug_scale_e);
			MDoubleArray	scale_e_array(NUM_TENTACLE_SAMPLE_POINTS);
	for (int n = 0; n < NUM_TENTACLE_SAMPLE_POINTS; ++n) {
		float	x = n * (float) unit_t;
		float	y;
		assert(tentacleShape::GetInterpolatedValue(__eccentricityScaleRampTupleArray, x, y, true) == 0);
		scale_e_array[n] = (double) y;
	}
			MFnDoubleArrayData	scale_e_data;
			MObject		scale_e_object = scale_e_data.create(scale_e_array, &ms);
			__checkStatus(ms, "MFnDoubleArrayData::create(MDoubleArray&)");
			ms = handle_scale_e.set(scale_e_object);
			__checkStatus(ms, "MDataHandle::set()");
			MPlug	plug_ramp_e = plug.child(TENTACLE_ARRAY_ECCENTRICITY_RAMP, &ms);
			MDataHandle	handle_ramp_e = handle.child(plug_ramp_e);
			MArrayDataHandle	arrayHandle_ramp_e(handle_ramp_e, &ms);
			ms = handle_ramp_e.copy(handle_parameter_eccentricity_ramp);
	MPlug	plug_parameter_rotation = plug_parameter.child(TENTACLE_PARAM_CROSS_SECTION_ROTATION, &ms);
	MPlug	plug_parameter_rotation_val = plug_parameter_rotation.child(TENTACLE_ATTRIBUTE_EDITOR_VAL, &ms);
	MPlug	plug_parameter_rotation_local_array = plug_parameter_rotation.child(TENTACLE_ATTRIBUTE_EDITOR_RAMP, &ms);
	MDataHandle	handle_parameter_rotation = handle_parameter.child(plug_parameter_rotation);
	MDataHandle	handle_parameter_rotation_ramp = handle_parameter_rotation.child(plug_parameter_rotation_local_array);
	MArrayDataHandle	arrayHandle_parameter_rotation_ramp(handle_parameter_rotation_ramp, &ms);
			MPlug		plug_rotation = plug.child(TENTACLE_ARRAY_ROTATION, &ms);
			__checkStatus(ms, "MPlug::child(unsigned)");
			MDataHandle	handle_rotation = handle.child(plug_rotation);
			double&		rotation = handle_rotation.asDouble();
			rotation = 0;
	ms = plug_parameter_rotation_val.getValue(rotation);
			rotation *= M_PI / 180;
			MPlug		plug_local_rotation = plug.child(TENTACLE_ARRAY_ROTATION_LOCAL, &ms);
			__checkStatus(ms, "MPlug::child(unsigned)");
			MDataHandle	handle_local_rotation = handle.child(plug_local_rotation);
			MDoubleArray	local_rotation_array(NUM_TENTACLE_SAMPLE_POINTS);
	for (int n = 0; n < NUM_TENTACLE_SAMPLE_POINTS; ++n) {
		float	x = n * (float) unit_t;
		float	y;
		assert(tentacleShape::GetInterpolatedValue(__rotationLocalRampTupleArray, x, y, true) == 0);
		local_rotation_array[n] = ((double) y - 0.5) * 2 * M_PI;
	}
			MFnDoubleArrayData	local_rotation_data;
			MObject		local_rotation_object = local_rotation_data.create(local_rotation_array, &ms);
			__checkStatus(ms, "MFnDoubleArrayData::create(MDoubleArray&)");
			ms = handle_local_rotation.set(local_rotation_object);
			__checkStatus(ms, "MDataHandle::set()");
			MPlug	plug_ramp_local = plug.child(TENTACLE_ARRAY_ROTATION_RAMP, &ms);
			MDataHandle	handle_ramp_local = handle.child(plug_ramp_local);
			MArrayDataHandle	arrayHandle_ramp_local(handle_ramp_local, &ms);
			ms = handle_ramp_local.copy(handle_parameter_rotation_ramp);
	MPlug	plug_parameter_cut = plug_parameter.child(TENTACLE_PARAM_CUT, &ms);
	MPlug	plug_parameter_cut_param = plug_parameter_cut.child(TENTACLE_ATTRIBUTE_EDITOR_VAL, &ms);
			MPlug		plug_cut = plug.child(TENTACLE_ARRAY_CUT, &ms);
			__checkStatus(ms, "MPlug::child(unsigned)");
			MDataHandle	handle_cut = handle.child(plug_cut);
#if 1
	MDataHandle	handle_parameter_cut = handle_parameter.child(plug_parameter_cut);
	MDataHandle	handle_parameter_cut_param = handle_parameter_cut.child(plug_parameter_cut_param);
			handle_cut.copy(handle_parameter_cut_param);
#else
			MPlug		plug_cut_have = plug_cut.child(0, &ms);
			MDataHandle	handle_cut_have = handle_cut.child(plug_cut_have);
			__checkStatus(ms, "MPlug::child(unsigned)");
			bool&		have = handle_cut_have.asBool();
			have = false;
			MPlug		plug_cut_t = plug_cut.child(1, &ms);
			__checkStatus(ms, "MPlug::child(unsigned)");
			MDataHandle	handle_cut_t = handle_cut.child(plug_cut_t);
			double&		t = handle_cut_t.asDouble();
			t = 0.5;
			MPlug		plug_cut_polar = plug_cut.child(2, &ms);
			__checkStatus(ms, "MPlug::child(unsigned)");
			MDataHandle	handle_cut_polar = handle_cut.child(plug_cut_polar);
			double&		polar = handle_cut_polar.asDouble();
			polar = 0;
			MPlug		plug_cut_azimuthal = plug_cut.child(3, &ms);
			__checkStatus(ms, "MPlug::child(unsigned)");
			MDataHandle	handle_cut_azimuthal = handle_cut.child(plug_cut_azimuthal);
			double&		azimuthal = handle_cut_azimuthal.asDouble();
			azimuthal = 0;
#endif
#endif
#endif
			}
			outputArray.set(builder);
			outputArray.setAllClean();
#if 1
			ms = MUserEventMessage::postUserEvent(TentacleArrayUpdated);
#endif
		}
		ms = datablock.setClean(plug);
#if 0
#if 1
		ms = MUserEventMessage::postUserEvent(TentacleArrayUpdated);
#else
		MGlobal::MSelectionMode	mode = MGlobal::selectionMode(&ms);
		MSelectionMask	mask = MGlobal::componentSelectionMask(&ms);
		bool	have = mask.intersects(MSelectionMask::kSelectComponentsMask, &ms);
		MSelectionList	selectionList;
		ms = MGlobal::getActiveSelectionList(selectionList);
		ms = MGlobal::setActiveSelectionList(selectionList, MGlobal::kAddToList);
#endif
#endif
#if 3
#if 1
#if 1
		MPlug	plug_tentacleArray(thisMObject(), __tentacleArray);
		ms = __skinSmoothBind.set(__externalDagPathShape, plug_tentacleArray);
#else
		ms = __setSkinSmoothBind();
#endif
#endif
		ms = getTentacleArray(__tentacles);
#if 1
		ms = __setTentaclePoints();
#else
		ms = __setTentacleMesh();
#if 2
	if (__shSimulationMayaPtr) {
	cout << "ShSimulation: Status:= " << (__simulationStatus == Simulation::On ? "On" : (__simulationStatus == Simulation::Off ? "Off" : (__simulationStatus == Simulation::Paused ? "Paused" : (__simulationStatus == Simulation::ReadOnly ? "ReadOnly" : "Unknown")))) << endl;
if (__simulationStatus == Simulation::On || __simulationStatus == Simulation::ReadOnly) {
#pragma warning(disable: 4127)
do {
	MStatus	ms;
#ifdef _INHERIT_kBODY
	kBodyMayaTentacle*	bodyMayaTentaclePtr = (kBodyMayaTentacle*) __shSimulationMayaPtr;
#else
	kBodyMayaTentacle*	bodyMayaTentaclePtr = (kBodyMayaTentacle*) __shSimulationMayaPtr->bodyMayaPtr();
#endif
	if (!bodyMayaTentaclePtr || !bodyMayaTentaclePtr->isValid()) {
		cerr << "Simulation: Warning: No External Node" << endl;
		break;
	}
#if 1
	MTime	time = MAnimControl::currentTime();
	unsigned	nthFrame = (unsigned) time.value();
#if 11
	int	shiftFrame = getShiftFrame();
	if (shiftFrame > 0) {
		cout << "Simulation: Frame # = " << nthFrame << " shifted by " << shiftFrame << endl;
		nthFrame += shiftFrame;
	} else if (shiftFrame == 0) {
		cout << "Simulation: Frame # = " << nthFrame << endl;
	}
#else
	cout << "Simulation: Frame # = " << nthFrame << endl;
#endif
#if 1
	if (nthFrame > 1)
#endif
	(void) __shSimulationMayaPtr->setSimulationCache(nthFrame);
	cout << "kSimulationCache::setSimulationCache(nthFrame = " << nthFrame << "): done" << endl;
#if 1
	const kSimulationCache::kStrandArray&	strandArray = __shSimulationMayaPtr->strandArray();
#if 0
	const kPtrArray<kTentacle>&	tentacleArray0 = bodyMayaTentaclePtr->tentacleArray0();
#endif
#if 3
	const kPtrArray<kTentacle>&	tentacleArray0 = bodyMayaTentaclePtr->tentacleArray0();
	MPlug	plug_meshArray(thisMObject(), outputMesh);
	bool	isArray = true;
	int	numElements_in_outputMesh = -1;
	isArray = plug_meshArray.isArray(&ms);
	if (!ms) {
		cerr << "ERR: |.outputMesh|: MPlug::isArray(): Failure (isArray = " << (isArray ? "True" : "False") << ")" << endl;
		isArray = false;
	} else if (isArray == false) {
		cerr << "WARN: |.outputMesh| not an array" << endl;
	} else {
		cout << "INFO: |.outputMesh| is an array" << endl;
		numElements_in_outputMesh = plug_meshArray.numElements(&ms);
		if (!ms) {
			cerr << "ERR: |.outputMesh|: MPlug::numElements(): Failure (isArray = True)" << endl;
			numElements_in_outputMesh = 0;
		}
		cout << "INFO: |.outputMesh|: # of element(s) = " << numElements_in_outputMesh << endl;
	}
#endif
	ms = __strand_longitudinals.clear();
	ms = __strand_longitudinals.setLength(strandArray.length() * NUM_TENTACLE_SAMPLE_POINTS);
	for (int indexStrand = 0; indexStrand < (int) strandArray.length(); ++indexStrand) {
		ShVector3	samplePoints[NUM_TENTACLE_SAMPLE_POINTS];
#if 1
if (nthFrame == 1) {
		if (__shSimulationMayaPtr->getResult0(indexStrand, NUM_TENTACLE_SAMPLE_POINTS, samplePoints) < 0) {
			cerr << "kSimulationCache::getResult0(indexStrand = " << indexStrand << "): Failure" << endl;
			continue;
		}
} else {
#endif
		if (__shSimulationMayaPtr->getResult(indexStrand, nthFrame, NUM_TENTACLE_SAMPLE_POINTS, samplePoints) < 0) {
			cerr << "kSimulationCache::getResult(indexStrand = " << indexStrand << ", nthFrame = " << nthFrame << "): Failure" << endl;
			continue;
		}
#if 1
}
#endif
#if 3
if (isArray && numElements_in_outputMesh > indexStrand) {
		cout << "INFO: indexStrand = " << indexStrand << ": set |.outputMesh| done" << endl;
		const kSimulationCache::kStrand&	strand = strandArray(indexStrand);
		int	strandId = strand.strandId;
		MPointArray	strandSamplePoints_in_kWorld(NUM_TENTACLE_SAMPLE_POINTS);
		for (int k = 0; k < NUM_TENTACLE_SAMPLE_POINTS; ++k) {
			MPoint&	p = strandSamplePoints_in_kWorld[k];
			p.x = samplePoints[k][0];
			p.y = samplePoints[k][1];
			p.z = samplePoints[k][2];
		}
#if 1
		MPointArray	strandSamplePoints;
		MPointArray	tentacleSamplePoints;
		const kTentacle&	tentacle = tentacleArray0(strandId);
		tentacle.getSamplePoints(externalInclusiveMatrix(), strandSamplePoints, numEllipseSamplePoints, tentacleSamplePoints, &strandSamplePoints_in_kWorld);
		if (nthFrame == 1) {
			cout << "strandSamplePoints_in_kWorld0: " << strandSamplePoints_in_kWorld << endl;
			cout << "strandSamplePoints_in_kWorld: " << strandSamplePoints << endl;
		}
		MPlug	plug_mesh = plug_meshArray.elementByPhysicalIndex(strandId, &ms);
		ms = tentacleShape::tentacleSetPoints(plug_mesh, tentacleSamplePoints, MSpace::kWorld);
#else
{
		const kTentacle&	tentacle = tentacleArray0(strandId);
		MPlug	plug_mesh = plug_meshArray.elementByPhysicalIndex(strandId, &ms);
		if (!ms) continue;
		MObject	inMeshObj;
		ms = plug_mesh.getValue(inMeshObj);
		if (!ms) continue;
		MFnMesh	mesh(inMeshObj, &ms);
		if (!ms) continue;
		MPointArray	strandSamplePoints;
		MPointArray	tentacleSamplePoints;
		tentacle.getSamplePoints(externalInclusiveMatrix(), strandSamplePoints, numEllipseSamplePoints, tentacleSamplePoints, &strandSamplePoints_in_kWorld);
#if 0
		MPointArray	points;
		ms = mesh.getPoints(points, MSpace::kWorld);
		cout << ms << endl;
		cout << "points: " << points << endl;
		cout << "tentacleSamplePoints: " << tentacleSamplePoints << endl;
#endif
		ms = mesh.setPoints(tentacleSamplePoints, MSpace::kWorld);
#if 0
		ms = mesh.getPoints(points, MSpace::kWorld);
		cout << "points: " << points << endl;
#endif
#if 1
		MFnMeshData	meshDataFn;
		MObject	newMeshObj = meshDataFn.create();
		MFnMesh	newMeshFn;
		newMeshFn.copy(inMeshObj, newMeshObj);
		newMeshFn.setObject(newMeshObj);
		plug_mesh.setValue(newMeshObj);
#endif
}
#endif
} else {
	if (isArray == false) {
		cerr << "FATAL: 3: ...: indexStrand = " << indexStrand << ": |.outputMesh| not an array" << endl;
	} else if (numElements_in_outputMesh <= indexStrand) {
		cerr << "WARN: 3: ...: indexStrand = " << indexStrand << ": OUT OF RANGE(# of elements in |.outputMesh| = " << numElements_in_outputMesh << ")" << endl;
	} else {
		cerr << "3: ...: indexStrand = " << indexStrand << ": Unknown Error" << endl;
	}
}
#endif
#if 1
		for (int k = 0; k < NUM_TENTACLE_SAMPLE_POINTS; ++k) {
			__strand_longitudinals[indexStrand * NUM_TENTACLE_SAMPLE_POINTS + k] = MVector(samplePoints[k]);
		}
#else
#if 10
		if (__numExternalStrands > 0) {
			for (int k = 0; k < NUM_TENTACLE_SAMPLE_POINTS; ++k) {
				__strand_longitudinals[indexStrand * NUM_TENTACLE_SAMPLE_POINTS + k] = MVector(samplePoints[k]);
			}
			continue;
		}
#endif
		const kSimulationCache::kStrand&	strand = strandArray(indexStrand);
		int	strandId = strand.strandId;
		const kTentacle&	tentacle = tentacleArray0(strandId);
		if (tentacle.longitudinal.length() < 2) {
			cerr << "FATAL: kTentacle::longitudinal: no segment" << endl;
			continue;
		}
		const MVector&	longiVector0 = tentacle.longitudinal[0];
		double	len0 = longiVector0.length();
		const kRoot&	root = (kRoot&) tentacle;
		MPoint	basePoint = root.position;
		__strand_longitudinals[indexStrand * NUM_TENTACLE_SAMPLE_POINTS + 0] = basePoint;
#if 1
		basePoint *= __externalInclusiveMatrix;
		MMatrix	inv_m = __externalInclusiveMatrix.inverse();
#else
		basePoint *= __externalWorldMatrix;
		MMatrix	inv_m = __externalWorldMatrix.inverse();
#endif
		MPoint	point = basePoint;
		for (int k = 1; k < (int) NUM_TENTACLE_SAMPLE_POINTS; ++k) {
			MVector&	n = __strand_longitudinals[indexStrand * NUM_TENTACLE_SAMPLE_POINTS + k];
			MPoint	nextPoint(samplePoints[k][0], samplePoints[k][1], samplePoints[k][2]);
			MVector	offset(nextPoint - point);
#if 1
			offset = len0 * offset.transformAsNormal(inv_m);
#else
			offset = len * offset.transformAsNormal(inv_m);
#endif
			n = offset;
			point = nextPoint;
		}
#endif
	}
#endif
#endif
} while (0);
#pragma warning(default: 4127)
}
	}
#endif
#endif
#endif
#if 1
#if 2
	if (
		__shSimulationMayaPtr &&
		(__simulationStatus == Simulation::On || __simulationStatus == Simulation::ReadOnly)
	) {
		__computeBoundingBox(datablock);
	} else {
		__computeBoundingBox(plug, datablock);
	}
#else
		__computeBoundingBox(plug, datablock);
#endif
		childChanged(MPxSurfaceShape::kBoundingBoxChanged);
#endif
		return MStatus::kSuccess;
#endif
	}
#endif
#endif
#pragma warning(default: 4127)
#if 2
	MStatus	ms = baseShape::compute(plug, datablock);
if (plug == outputSurface && hasHistory()) {
#if 1
#if 1
	MPlug	plug_tentacleArray(thisMObject(), __tentacleArray);
	ms = __skinSmoothBind.set(__externalDagPathShape, plug_tentacleArray);
#else
	ms = __setSkinSmoothBind();
#endif
#endif
	ms = getTentacleArray(__tentacles);
#if 1
	ms = __setTentaclePoints();
#else
	ms = __setTentacleMesh();
	if (__shSimulationMayaPtr) {
	cout << "ShSimulation: Status:= " << (__simulationStatus == Simulation::On ? "On" : (__simulationStatus == Simulation::Off ? "Off" : (__simulationStatus == Simulation::Paused ? "Paused" : (__simulationStatus == Simulation::ReadOnly ? "ReadOnly" : "Unknown")))) << endl;
if (__simulationStatus == Simulation::On || __simulationStatus == Simulation::ReadOnly) {
#pragma warning(disable: 4127)
do {
	MStatus	ms;
#ifdef _INHERIT_kBODY
	kBodyMayaTentacle*	bodyMayaTentaclePtr = (kBodyMayaTentacle*) __shSimulationMayaPtr;
#else
	kBodyMayaTentacle*	bodyMayaTentaclePtr = (kBodyMayaTentacle*) __shSimulationMayaPtr->bodyMayaPtr();
#endif
	if (!bodyMayaTentaclePtr || !bodyMayaTentaclePtr->isValid()) {
		cerr << "Simulation: Warning: No External Node" << endl;
		break;
	}
#if 1
	MTime	time = MAnimControl::currentTime();
	unsigned	nthFrame = (unsigned) time.value();
#if 11
	int	shiftFrame = getShiftFrame();
	if (shiftFrame > 0) {
		cout << "Simulation: Frame # = " << nthFrame << " shifted by " << shiftFrame << endl;
		nthFrame += shiftFrame;
	} else if (shiftFrame == 0) {
		cout << "Simulation: Frame # = " << nthFrame << endl;
	}
#else
	cout << "Simulation: Frame # = " << nthFrame << endl;
#endif
#if 1
	if (nthFrame > 1)
#endif
	(void) __shSimulationMayaPtr->setSimulationCache(nthFrame);
	cout << "kSimulationCache::setSimulationCache(nthFrame = " << nthFrame << "): done" << endl;
#if 1
	const kSimulationCache::kStrandArray&	strandArray = __shSimulationMayaPtr->strandArray();
#if 0
	const kPtrArray<kTentacle>&	tentacleArray0 = bodyMayaTentaclePtr->tentacleArray0();
#endif
#if 3
	const kPtrArray<kTentacle>&	tentacleArray0 = bodyMayaTentaclePtr->tentacleArray0();
	MPlug	plug_meshArray(thisMObject(), outputMesh);
	bool	isArray = true;
	int	numElements_in_outputMesh = -1;
	isArray = plug_meshArray.isArray(&ms);
	if (!ms) {
		cerr << "ERR: |.outputMesh|: MPlug::isArray(): Failure (isArray = " << (isArray ? "True" : "False") << ")" << endl;
		isArray = false;
	} else if (isArray == false) {
		cerr << "WARN: |.outputMesh| not an array" << endl;
	} else {
		cout << "INFO: |.outputMesh| is an array" << endl;
		numElements_in_outputMesh = plug_meshArray.numElements(&ms);
		if (!ms) {
			cerr << "ERR: |.outputMesh|: MPlug::numElements(): Failure (isArray = True)" << endl;
			numElements_in_outputMesh = 0;
		}
		cout << "INFO: |.outputMesh|: # of element(s) = " << numElements_in_outputMesh << endl;
	}
#endif
	ms = __strand_longitudinals.clear();
	ms = __strand_longitudinals.setLength(strandArray.length() * NUM_TENTACLE_SAMPLE_POINTS);
	for (int indexStrand = 0; indexStrand < (int) strandArray.length(); ++indexStrand) {
		ShVector3	samplePoints[NUM_TENTACLE_SAMPLE_POINTS];
#if 1
if (nthFrame == 1) {
		if (__shSimulationMayaPtr->getResult0(indexStrand, NUM_TENTACLE_SAMPLE_POINTS, samplePoints) < 0) {
			cerr << "kSimulationCache::getResult0(indexStrand = " << indexStrand << "): Failure" << endl;
			continue;
		}
} else {
#endif
		if (__shSimulationMayaPtr->getResult(indexStrand, nthFrame, NUM_TENTACLE_SAMPLE_POINTS, samplePoints) < 0) {
			cerr << "kSimulationCache::getResult(indexStrand = " << indexStrand << ", nthFrame = " << nthFrame << "): Failure" << endl;
			continue;
		}
#if 1
}
#endif
#if 3
if (isArray && numElements_in_outputMesh > indexStrand) {
		cout << "INFO: indexStrand = " << indexStrand << ": set |.outputMesh| done" << endl;
		const kSimulationCache::kStrand&	strand = strandArray(indexStrand);
		int	strandId = strand.strandId;
		MPointArray	strandSamplePoints_in_kWorld(NUM_TENTACLE_SAMPLE_POINTS);
		for (int k = 0; k < NUM_TENTACLE_SAMPLE_POINTS; ++k) {
			MPoint&	p = strandSamplePoints_in_kWorld[k];
			p.x = samplePoints[k][0];
			p.y = samplePoints[k][1];
			p.z = samplePoints[k][2];
		}
#if 1
		MPointArray	strandSamplePoints;
		MPointArray	tentacleSamplePoints;
		const kTentacle&	tentacle = tentacleArray0(strandId);
		tentacle.getSamplePoints(externalInclusiveMatrix(), strandSamplePoints, numEllipseSamplePoints, tentacleSamplePoints, &strandSamplePoints_in_kWorld);
		if (nthFrame == 1) {
			cout << "strandSamplePoints_in_kWorld0: " << strandSamplePoints_in_kWorld << endl;
			cout << "strandSamplePoints_in_kWorld: " << strandSamplePoints << endl;
		}
		MPlug	plug_mesh = plug_meshArray.elementByPhysicalIndex(strandId, &ms);
		ms = tentacleShape::tentacleSetPoints(plug_mesh, tentacleSamplePoints, MSpace::kWorld);
#else
{
		const kTentacle&	tentacle = tentacleArray0(strandId);
		MPlug	plug_mesh = plug_meshArray.elementByPhysicalIndex(strandId, &ms);
		if (!ms) continue;
		MObject	inMeshObj;
		ms = plug_mesh.getValue(inMeshObj);
		if (!ms) continue;
		MFnMesh	mesh(inMeshObj, &ms);
		if (!ms) continue;
		MPointArray	strandSamplePoints;
		MPointArray	tentacleSamplePoints;
		tentacle.getSamplePoints(externalInclusiveMatrix(), strandSamplePoints, numEllipseSamplePoints, tentacleSamplePoints, &strandSamplePoints_in_kWorld);
#if 0
		MPointArray	points;
		ms = mesh.getPoints(points, MSpace::kWorld);
		cout << ms << endl;
		cout << "points: " << points << endl;
		cout << "tentacleSamplePoints: " << tentacleSamplePoints << endl;
#endif
		ms = mesh.setPoints(tentacleSamplePoints, MSpace::kWorld);
#if 0
		ms = mesh.getPoints(points, MSpace::kWorld);
		cout << "points: " << points << endl;
#endif
#if 1
		MFnMeshData	meshDataFn;
		MObject	newMeshObj = meshDataFn.create();
		MFnMesh	newMeshFn;
		newMeshFn.copy(inMeshObj, newMeshObj);
		newMeshFn.setObject(newMeshObj);
		plug_mesh.setValue(newMeshObj);
#endif
}
#endif
} else {
	if (isArray == false) {
		cerr << "FATAL: 3: ...: indexStrand = " << indexStrand << ": |.outputMesh| not an array" << endl;
	} else if (numElements_in_outputMesh <= indexStrand) {
		cerr << "WARN: 3: ...: indexStrand = " << indexStrand << ": OUT OF RANGE(# of elements in |.outputMesh| = " << numElements_in_outputMesh << ")" << endl;
	} else {
		cerr << "3: ...: indexStrand = " << indexStrand << ": Unknown Error" << endl;
	}
}
#endif
#if 1
		for (int k = 0; k < NUM_TENTACLE_SAMPLE_POINTS; ++k) {
			__strand_longitudinals[indexStrand * NUM_TENTACLE_SAMPLE_POINTS + k] = MVector(samplePoints[k]);
		}
#else
#if 10
		if (__numExternalStrands > 0) {
			for (int k = 0; k < NUM_TENTACLE_SAMPLE_POINTS; ++k) {
				__strand_longitudinals[indexStrand * NUM_TENTACLE_SAMPLE_POINTS + k] = MVector(samplePoints[k]);
			}
			continue;
		}
#endif
		const kSimulationCache::kStrand&	strand = strandArray(indexStrand);
		int	strandId = strand.strandId;
		const kTentacle&	tentacle = tentacleArray0(strandId);
		if (tentacle.longitudinal.length() < 2) {
			cerr << "FATAL: kTentacle::longitudinal: no segment" << endl;
			continue;
		}
		const MVector&	longiVector0 = tentacle.longitudinal[0];
		double	len0 = longiVector0.length();
		const kRoot&	root = (kRoot&) tentacle;
		MPoint	basePoint = root.position;
		__strand_longitudinals[indexStrand * NUM_TENTACLE_SAMPLE_POINTS + 0] = basePoint;
#if 1
		basePoint *= __externalInclusiveMatrix;
		MMatrix	inv_m = __externalInclusiveMatrix.inverse();
#else
		basePoint *= __externalWorldMatrix;
		MMatrix	inv_m = __externalWorldMatrix.inverse();
#endif
		MPoint	point = basePoint;
		for (int k = 1; k < (int) NUM_TENTACLE_SAMPLE_POINTS; ++k) {
			MVector&	n = __strand_longitudinals[indexStrand * NUM_TENTACLE_SAMPLE_POINTS + k];
			MPoint	nextPoint(samplePoints[k][0], samplePoints[k][1], samplePoints[k][2]);
			MVector	offset(nextPoint - point);
#if 1
			offset = len0 * offset.transformAsNormal(inv_m);
#else
			offset = len * offset.transformAsNormal(inv_m);
#endif
			n = offset;
			point = nextPoint;
		}
#endif
	}
#endif
#endif
} while (0);
#pragma warning(default: 4127)
}
	}
#endif
}
	return MStatus::kSuccess;
#else
	return baseShape::compute(plug, datablock);
#endif
}
#if 2
bool	tentacleShape::setInternalValue(const MPlug& plug, const MDataHandle& handle)
{
	if (plug == SimulationStatus) {
		Simulation::Status	lastStatus = __simulationStatus;
		__simulationStatus = (Simulation::Status) handle.asShort();
		if (lastStatus == __simulationStatus) {
		} else if (__simulationStatus == Simulation::Paused) {
			if (lastStatus == Simulation::ReadOnly) {
				if (__shSimulationMayaPtr) {
					delete __shSimulationMayaPtr;
					__shSimulationMayaPtr = 0;
				}
				__simulationStatus = Simulation::Off;
			}
		} else if (__simulationStatus == Simulation::Off) {
			if (__shSimulationMayaPtr) {
				delete __shSimulationMayaPtr;
				__shSimulationMayaPtr = 0;
			}
#if 10
			__numExternalStrands = -1;
#endif
		} else if (__simulationStatus == Simulation::On || __simulationStatus == Simulation::ReadOnly) {
			if (__simulationStatus == Simulation::ReadOnly) {
				if (__shSimulationMayaPtr) {
					delete __shSimulationMayaPtr;
					__shSimulationMayaPtr = 0;
				}
			}
if (__shSimulationMayaPtr) {
} else {
			unsigned	nFrame = (unsigned) MAnimControl::maxTime().value();
#if 11
			nFrame *= 2;
			setShiftFrame(0);
#endif
			__shSimulationMayaPtr = new kShSimulationMayaTentacle(kTime::kTimeUnitFilm10, nFrame, __simulationStatus == Simulation::ReadOnly);
#if 1
			if (__externalDagPathShape.isValid()) {
				MStatus	ms;
#ifdef _INHERIT_kBODY
				kBodyMayaTentacle*	bodyMayaTentaclePtr = (kBodyMayaTentacle*) __shSimulationMayaPtr;
#else
				kBodyMayaTentacle*	bodyMayaTentaclePtr = (kBodyMayaTentacle*) __shSimulationMayaPtr->bodyMayaPtr();
#endif
				if (bodyMayaTentaclePtr) {
					ms = bodyMayaTentaclePtr->setBody(__externalDagPathShape);
#if 1
					MObject	node = plug.node();
					ms = bodyMayaTentaclePtr->set0(node);
#else
					MDataBlock	datablock = forceCache();
					MDataHandle	cachedHandle = datablock.inputValue(cachedSurface, &ms);
					afterShaveData* cached = (afterShaveData*) cachedHandle.asPluginData();
					bodyMayaTentaclePtr->set__T0_in_kSpace_array(*cached->fGeometry);
#endif
				}
			}
#endif
#ifdef _DEBUG2
			ShSimParameter&	simulationParameter = __shSimulationMayaPtr->simulationParameter();
			baseShape::printSimulationParameter(simulationParameter);
#endif
			MPlug	plug(thisMObject(), Dynamics);
			bool	dynamics;
			plug.getValue(dynamics);
			__shSimulationMayaPtr->setDynamicsFlag(dynamics);
			applySimulationParameter(true);
#ifdef _DEBUG2
			baseShape::printSimulationParameter(simulationParameter);
#endif
			if (__shSimulationMayaPtr) {
				MStatus	ms;
				MObject	node = plug.node(&ms); if (!ms) cout << ms << endl;
				MFnDagNode	dagNode(node, &ms); if (!ms) cout << ms << endl;
				MDagPath	dagPathShape;
				ms = dagNode.getPath(dagPathShape); if (!ms) cout << ms << endl;
#if 0
				if (dagPathShape.isValid()) {
					__shSimulationMayaPtr->setDagPathShape(dagPathShape, false);
				}
#endif
#if 0
				if (__externalDagPathShape.isValid()) {
					__shSimulationMayaPtr->setDagPathShape(__externalDagPathShape, true);
#if 1
					__shSimulationMayaPtr->setInclusiveMatrix(__externalInclusiveMatrix, true);
					__shSimulationMayaPtr->setExclusiveMatrix(__externalExclusiveMatrix, true);
#else
					__shSimulationMayaPtr->setWorldMatrix(__externalWorldMatrix, true);
#endif
				}
#endif
if (__simulationStatus == Simulation::ReadOnly) {
	std::vector<double>	strandLengthArray(__segmentLengthInGuide.size());
	MPlug	plug(thisMObject(), FileCacheRead);
	MString	fileCacheReadFilename;
	plug.getValue(fileCacheReadFilename);
	if (__shSimulationMayaPtr->setStrandFromFileCache(fileCacheReadFilename.asChar(), &strandLengthArray) < 0) {
		cerr << "Error: kSimulationCache::setStrandFromFileCache(): Failure" << endl;
#if 10
		if (__isExternalFileReadOnly())
			__numExternalStrands = 0;
#endif
	} else {
#if 10
		if (__isExternalFileReadOnly()) {
			__numExternalStrands = (int) strandLengthArray.size();
			__segmentLengthInGuide.resize(__numExternalStrands);
		}
#endif
		const kSimulationCache::kStrandArray&	strandArray = __shSimulationMayaPtr->strandArray();
		for (int indexStrand = 0; indexStrand < (int) strandArray.length(); ++indexStrand) {
			int	strandId = strandArray(indexStrand).strandId;
			__segmentLengthInGuide.at(strandId) = strandLengthArray.at(strandId) / (baseShape::N_POINTS_IN_GUIDE - 1);
		}
	}
} else {
#if 1
#if 1
	std::vector<int>	strandIdArray;
#pragma warning(disable: 4127)
do {
	MSelectionMask	mask = MGlobal::componentSelectionMask(&ms);
	if (!mask.intersects(MSelectionMask::kSelectComponentsMask)) {
		break;
	}
	MSelectionList	list;
	MGlobal::getActiveSelectionList(list);
	for (unsigned iList = 0; iList < list.length(); ++iList) {
		MDagPath	dagPath;
		MObject		component;
		ms = list.getDagPath(iList, dagPath, component);
		if (dagPath.fullPathName() == "") {
			continue;
		}
		MFnDependencyNode	fnDepNode(dagPath.node(), &ms);
		MString	typeName = fnDepNode.typeName(&ms);
		if (
			!ms ||
			typeName != "tentacleShapeNode" ||
			!(
				component.hasFn(MFn::kSingleIndexedComponent) &&
				component.hasFn(MFn::kMeshEdgeComponent)
			)
		) {
			continue;
		}
		MFnSingleIndexedComponent	fnEdgeComp(component, &ms);
		for (int iElem = 0; iElem < fnEdgeComp.elementCount(&ms); ++iElem) {
			int	index = fnEdgeComp.element(iElem, &ms);
			if (ms) {
				cout << "index = " << index << endl;
				strandIdArray.push_back(index);
			}
		}
	}
} while (0);
#pragma warning(default: 4127)
#else
	MPlug	idPlug(thisMObject(), StrandIdList);
	MString	idList;
	idPlug.getValue(idList);
	std::vector<int>	strandIdArray;
#if 0
	strandIdArray.push_back(0);
	strandIdArray.push_back(1);
	strandIdArray.push_back(2);
#else
#ifdef _INHERIT_kBODY
	kBodyMayaTentacle*	bodyMayaTentaclePtr = (kBodyMayaTentacle*) __shSimulationMayaPtr;
#else
	kBodyMayaTentacle*	bodyMayaTentaclePtr = (kBodyMayaTentacle*) __shSimulationMayaPtr->bodyMayaPtr();
#endif
	const kPtrArray<kTentacle>&	tentacleArray0 = bodyMayaTentaclePtr->tentacleArray0();
	int	lastStrandId = tentacleArray0.length() - 1;
	strandIdArray.clear();
	cout << "|" << idList << "|" << endl;
	MStringArray	idListArray;
	idList.split(',', idListArray);
int	index_min = 0, index_max = lastStrandId;
{
	MString	s(",,1-3,-,1-,-1, ");
	MStringArray	ss;
	s.split(',', ss);
	cout << "s: |" << s << "|" << endl;
	for (unsigned n = 0; n < ss.length(); ++n) {
		cout << "ss[" << n << "]: |" << ss[n] << "|" << endl;
		MStringArray	sss;
		ss[n].split('-', sss);
		int	index = ss[n].index('-');
		cout << "index of '-' = " << index << endl;
		MString	start, end;
		if (index == -1) {
			start = ss[n];
			end = ss[n];
		} else {
			if (index == 0) {
				start = MString("0");
			} else {
				start = ss[n].substring(0, index - 1);
			}
			end = ss[n].substring(index + 1, ss[n].length() - 1);
		}
		cout << "start: |" << start << "|(" << (start.isInt() ? start.asInt() : index_min) << "), end: |" << end << "|(" << (end.isInt() ? end.asInt() : index_max) << ")" << endl;
		for (unsigned m = 0; m < sss.length(); ++m) {
			cout << "\tsss[" << m << "]: |" << sss[m] << "|" << endl;
		}
	}
}
	cout << "# of idListArray = " << idListArray.length() << endl;
	for (unsigned n = 0; n < idListArray.length(); ++n) {
		cout << "idList[" << n << "]: |" << idListArray[n] << "|" << endl;
		int	startId = -1, endId = -1;
		MStringArray	idRangeArray;
		idListArray[n].split('-', idRangeArray);
		bool	isOk = true;
		cout << "# of idRangeArray = " << idRangeArray.length() << endl;
		for (unsigned k = 0; k < idRangeArray.length(); ++k) {
			if (k == 2) {
				cerr << "Warning: |" << idListArray[n] << "|: Invalid Format: >2 element(s) Ignored" << endl;
				break;
			}
			cout << "\tidRange[" << k << "]: |" << idRangeArray[k] << "|" << endl;
			int	id = -1;
			char*	id_token_ptr = (char*) idRangeArray[k].asChar();
			yyy::strip(&id_token_ptr);
			if (*id_token_ptr) {
				char*	next_ptr = 0;
				errno = 0;
				id = strtol(id_token_ptr, &next_ptr, 0);
				if (errno == ERANGE) {
					cerr << "\tError: idRange[" << k << "]: |" << idRangeArray[k].asChar() << "|: ERANGE: Failure" << endl;
					isOk = false;
					break;
				}
				if (next_ptr == id_token_ptr) {
					cerr << "\tError: idRange[" << k << "]: |" << idRangeArray[k].asChar() << "|: no conversion: Failure" << endl;
					isOk = false;
					break;
				}
				yyy::strip(&next_ptr);
				if (*next_ptr) {
					cerr << "\tError: idRange[" << k << "]: |" << idRangeArray[k].asChar() << "|: invalid format: Failure" << endl;
					isOk = false;
					break;
				}
			}
			if (k)
				endId = id;
			else
				startId = id;
		}
		if (!isOk || (startId == -1 && endId == -1)) {
			cerr << "Warning: |" << idListArray[n] << "|: Invalid Format: Ignored" << endl;
			continue;
		}
		if (startId == -1)
			startId = 0;
		if (endId == -1)
			endId = 7;
		if (endId < startId) {
			cerr << "Warning: |" << idListArray[n] << "|: endId(" << endId << ") < startId(" << endId << "): Invalid Format: Ignored" << endl;
			continue;
		}
		cout << "startId = " << startId << ", endId = " << endId << endl;
		for (int id = startId; id <= endId; ++id) {
			bool	isExist = false;
			if (id > lastStrandId)
				continue;
			for (unsigned n = 0; n < strandIdArray.size(); ++n) {
				if (id == strandIdArray.at(n)) {
					isExist = true;
					break;
				}
			}
			if (isExist)
				continue;
			strandIdArray.push_back(id);
		}
	}
#endif
#endif
#endif
#if 1
#ifdef _INHERIT_kBODY
	kBodyMayaTentacle*	bodyMayaTentaclePtr = (kBodyMayaTentacle*) __shSimulationMayaPtr;
#else
	kBodyMayaTentacle*	bodyMayaTentaclePtr = (kBodyMayaTentacle*) __shSimulationMayaPtr->bodyMayaPtr();
#endif
	const kPtrArray<kTentacle>&	tentacleArray0 = bodyMayaTentaclePtr->tentacleArray0();
#if 1
	MPlug	plug_tentacleArray(thisMObject(), __tentacleArray);
#endif
	for (int indexStrand = 0; indexStrand < (int) strandIdArray.size(); ++indexStrand) {
		int	strandId = strandIdArray.at(indexStrand);
		const kTentacle&	tentacle = tentacleArray0(strandId);
		if (tentacle.longitudinal.length() < 2) {
			cerr << "FATAL: kTentacle::longitudinal: no segment" << endl;
			continue;
		}
		const MVector&	longiVector0 = tentacle.longitudinal[0];
		double	strandLength = tentacle.longitudinal.length() * longiVector0.length();
#if 1
#if 1
		int	na = 0, nb = 0, nc = 0;
		int	nA = DEFAULT_SEGMENT_COUNT, nB = 0;
#else
		int	na = 2, nb = 3, nc = 5;
		int	nA = DEFAULT_SEGMENT_COUNT / 2;
		int	nB = DEFAULT_SEGMENT_COUNT - nA;
#endif
		int	numSegments = na + nA + nb + nB + nc;
		KVector	segLen(numSegments);
		int	k = 0;
		for (int i = 0; i < na; ++i, ++k)
			segLen(k) = 0;
		for (int i = 0; i < nA; ++i, ++k)
			segLen(k) = strandLength / DEFAULT_SEGMENT_COUNT;
		for (int i = 0; i < nb; ++i, ++k)
			segLen(k) = 0;
		for (int i = 0; i < nB; ++i, ++k)
			segLen(k) = strandLength / DEFAULT_SEGMENT_COUNT;
		for (int i = 0; i < nc; ++i, ++k)
			segLen(k) = 0;
		cout << "numSegments = " << numSegments << ", k = " << k << endl;
		kSimulationCache::kStrandPtr	strandPtr = __shSimulationMayaPtr->addStrand(strandId, numSegments, 0, &segLen);
		if (strandPtr == NULL) {
			cerr << "Error: kSimulationCache::addStrand(): Failure" << endl;
			continue;
		}
#else
		if (__shSimulationMayaPtr->addStrand(strandId, DEFAULT_SEGMENT_COUNT, 0, strandLength) == NULL) {
			cerr << "Error: kSimulationCache::addStrand(): Failure" << endl;
		}
#endif
#if 1
		cout << "초기 모양 반영: indexStrand = " << indexStrand << ", strandId = " << strandId << endl;
		MPointArray	strandSamplePoints;
		MPointArray	tentacleSamplePoints;
#if 1
		const kSkinSmoothBind&	skinSmoothBind = bodyMayaTentaclePtr->skinSmoothBind();
		if (skinSmoothBind.hasSkinCluster()) {
			float	segment_length = (float) tentacle.longitudinal[0].length();
			MPlug	plug = plug_tentacleArray.elementByPhysicalIndex(strandId, &ms);
			kTentacle&	nonConst_tentacle = const_cast<kTentacle&>(tentacle);
			ms = skinSmoothBind.getPoints(strandId, plug, segment_length, nonConst_tentacle.wNormal, nonConst_tentacle.wPoints);
			tentacle.getSamplePoints(strandSamplePoints, numEllipseSamplePoints, tentacleSamplePoints);
		} else {
#endif
		tentacle.getSamplePoints(bodyMayaTentaclePtr->inclusiveMatrix(), strandSamplePoints, numEllipseSamplePoints, tentacleSamplePoints);
#if 1
		}
#endif
		ShVector3	samplePoints[NUM_TENTACLE_SAMPLE_POINTS];
		for (int k = 0; k < NUM_TENTACLE_SAMPLE_POINTS; ++k) {
			const MPoint&	p = strandSamplePoints[k];
			samplePoints[k][0] = p.x;
			samplePoints[k][1] = p.y;
			samplePoints[k][2] = p.z;
		}
		if (__shSimulationMayaPtr->setInitialCurvatureVector(indexStrand, NUM_TENTACLE_SAMPLE_POINTS, samplePoints) < 0) {
			cerr << "Error: kSimulationCache::setInitialCurvatureVector(): Failure" << endl;
		}
#endif
	}
#if 0
if (__isWorldMeshConnected) {
				__shSimulationMayaPtr->setMeshGeom(meshGeom());
				__shSimulationMayaPtr->setCPPtr(getControlPoints());
	baseShapeGeometry*	geometryPtr = meshGeom();
	MMatrix	m = __externalWorldMatrix;
	MVectorArray*	cpPtr = getControlPoints();
	MVectorArray&	cp = *cpPtr;
{
	MObject	dagPathTransform = dagPathShape.transform(&ms);
	bool	isAnimated = MAnimUtil::isAnimated(dagPathTransform, false, &ms);
	if (isAnimated) {
		MTime	time;
		time.setValue(0);
		getWorldMatrixAt(dagPathShape, time, m);
	}
}
	ShVector3	inSamples[MAX_SAMPLE_POINTS];
	kSimulationCache::kStrandArray&	strandArray = __shSimulationMayaPtr->strandArray();
	for (int indexStrand = 0; indexStrand < (int) strandArray.length(); ++indexStrand) {
		int	strandId = strandArray(indexStrand).strandId;
		MPoint	basePoint = geometryPtr->vertices[strandId];
#if 3
		basePoint *= m;
#endif
		MPoint	point = basePoint;
		for (int k = 0; k < (int) baseShape::N_POINTS_IN_GUIDE; ++k) {
			if (k) {
				MVector	offset = cp[baseShape::N_POINTS_IN_GUIDE * strandId + k];
				double	len = offset.length();
#if 3
				offset = offset.transformAsNormal(m);
#endif
				point += len * offset;
				ShVector3&	v = inSamples[k - 1];
				v[0] = point.x;
				v[1] = point.y;
				v[2] = point.z;
			}
		}
	}
}
#endif
}
			}
}
#endif
		} else {
			cerr << "Warning: Unknown Simulation Status(|" << __simulationStatus << "|)" << endl;
			__simulationStatus = lastStatus;
			return true;
		}
		return false;
	}
	return baseShape::setInternalValue(plug, handle);
}
#endif
MStatus	tentacleShape::setDependentsDirty(const MPlug& plugBeingDirtied, MPlugArray& affectedPlugs)
{
	MStatus	ms;
#if 1
#if 3
	if (plugBeingDirtied == time) {
		cout << "tentacleShape::setDependentsDirty: plugBeingDirtied = time" << endl;
		return MStatus::kSuccess;
	}
#endif
#if 1
	if (plugBeingDirtied == __tentacleDoAction) {
		MPlug	plug(thisMObject(), __tentacleArray);
		ms = affectedPlugs.append(plug);
		return ms;
	}
#if 1
	bool	isTentacleParameter = false;
	MPlugArray	hierarchy;
	SetTentacleParameterValue(
		0,
		plugBeingDirtied,
		hierarchy,
		&isTentacleParameter
	);
	if (!isTentacleParameter)
		return MStatus::kSuccess;
	assert(hierarchy.length() >= 2);
	TentacleParameter::Type	type = TentacleParameter::None;
	for (int i = 0; i < NUM_TENTACLE_PARAM_CHILDREN; ++i) {
		if (hierarchy[1] == hierarchy[0].child(i)) {
			type = (TentacleParameter::Type) i;
			break;
		}
	}
	assert(type != TentacleParameter::None);
#if 1
	bool	isDirty = false;
#pragma	warning(disable: 4127)
do {
	if (!hasActiveComponents())
		break;
	MPlug	plug_sync = hierarchy[1].child(TENTACLE_ATTRIBUTE_EDITOR_SYNC, &ms);
	if (!plug_sync || !ms)
		break;
	bool	isSync = plugBeingDirtied == plug_sync;
	bool	sync;
	ms = plug_sync.getValue(sync);
	if (!ms)
		break;
	if (
		(
			isSync &&
			sync == false
		) ||
		(
			sync == true &&
			!isSync
		)
	) {
		isDirty = true;
	}
} while (0);
#pragma	warning(default: 4127)
	if (isDirty) {
		MPlug	plug(thisMObject(), __tentacleArray);
		ms = affectedPlugs.append(plug);
		ms = plug.setAttribute(__tentacleDoAction);
		ms = plug.setValue(TentacleDo::Update);
		return MStatus::kSuccess;
	}
	if (
		type == TentacleParameter::MajorRadius ||
		type == TentacleParameter::Eccentricity ||
		type == TentacleParameter::Rotation
	) {
		if (hierarchy[2] == hierarchy[1].child(TENTACLE_ATTRIBUTE_EDITOR_RAMP, &ms)) {
			__rampWakesType = type == TentacleParameter::MajorRadius ? RampWakes::MajorRadius : (type == TentacleParameter::Eccentricity ? RampWakes::Eccentricity : RampWakes::Rotation);
			MPlug	plug(thisMObject(), __rampWakes);
			ms = affectedPlugs.append(plug);
			return MStatus::kSuccess;
		}
	}
#endif
#if 0
#if 1
	RampWakes::Type	rampType = RampWakes::None;
#pragma	warning(disable: 4127)
do {
#if 1
	rampType = RampWakes::None;
do {
	if (!plugBeingDirtied.isElement(&ms) || !ms)
		break;
	MPlug	array = plugBeingDirtied.array(&ms);
	if (!array || !ms)
		break;
	if (!array.isChild(&ms) || !ms)
		break;
	MPlug	parent = array.parent(&ms);
	if (!parent || !ms)
		break;
	if (!parent.isChild(&ms) || !ms)
		break;
	MPlug	tentacleParameter = parent.parent(&ms);
	if (!tentacleParameter || !ms)
		break;
	if (parent == tentacleParameter.child(TENTACLE_PARAM_CROSS_SECTION_MAJOR_RADIUS, &ms))
		rampType = RampWakes::MajorRadius;
	else if (parent == tentacleParameter.child(TENTACLE_PARAM_CROSS_SECTION_ECCENTRICITY, &ms))
		rampType = RampWakes::Eccentricity;
	else if (parent == tentacleParameter.child(TENTACLE_PARAM_CROSS_SECTION_ROTATION, &ms))
		rampType = RampWakes::Rotation;
} while (0);
	if (rampType != RampWakes::None) {
		break;
	}
#endif
#if 1
	rampType = RampWakes::None;
do {
	if (!plugBeingDirtied.isChild(&ms) || !ms)
		break;
	MPlug	parent = plugBeingDirtied.parent(&ms);
	if (!parent || !ms)
		break;
	if (!parent.isElement(&ms) || !ms)
		break;
	MPlug	array = parent.array(&ms);
	if (!array || !ms)
		break;
	if (!array.isChild(&ms) || !ms)
		break;
	MPlug	grandparent = array.parent(&ms);
	if (!grandparent || !ms)
		break;
	if (!grandparent.isChild(&ms) || !ms)
		break;
	MPlug	tentacleParameter = grandparent.parent(&ms);
	if (!tentacleParameter || !ms)
		break;
	if (grandparent == tentacleParameter.child(TENTACLE_PARAM_CROSS_SECTION_MAJOR_RADIUS, &ms))
		rampType = RampWakes::MajorRadius;
	else if (grandparent == tentacleParameter.child(TENTACLE_PARAM_CROSS_SECTION_ECCENTRICITY, &ms))
		rampType = RampWakes::Eccentricity;
	else if (grandparent == tentacleParameter.child(TENTACLE_PARAM_CROSS_SECTION_ROTATION, &ms))
		rampType = RampWakes::Rotation;
} while (0);
	if (rampType != RampWakes::None) {
		break;
	}
#endif
} while (0);
#pragma	warning(default: 4127)
	cout << "plugBeingDirtied: Ramp Type: " << rampType << endl;
	if (rampType != RampWakes::None) {
{
		MPlug	plug(thisMObject(), __tentacleArray);
		ms = affectedPlugs.append(plug);
		ms = plug.setAttribute(__tentacleDoAction);
		ms = plug.setValue(TentacleDo::Update);
}
		__rampWakesType = rampType;
		MPlug	plug(thisMObject(), __rampWakes);
		ms = affectedPlugs.append(plug);
	}
#endif
#endif
#endif
#endif
#endif
	return baseShape::setDependentsDirty(plugBeingDirtied, affectedPlugs);
}
#endif
#if 1
#if 1
#if 1
#if 0
static void	listRamp(
	MPlug	plug_ramp,
	MDataHandle	handle_ramp
)
{
	MStatus	ms;
	MPlug&	plug = plug_ramp;
	MArrayDataHandle	arrayHandle(handle_ramp);
	cout << "# of ramps = " << arrayHandle.elementCount(&ms) << " (" << ms << ")" << endl;
	for (int n = 0; n < (int) arrayHandle.elementCount(&ms); ++n) {
		ms = arrayHandle.jumpToArrayElement(n);
		if (!ms) cout << ms << endl;
		MDataHandle	handle = arrayHandle.inputValue(&ms);
		if (!ms) cout << ms << endl;
		MPlug	plug_Position = plug.child(RAMP_POSITION, &ms);
		if (!ms) cout << ms << endl;
		MDataHandle	handle_Position = handle.child(plug_Position);
		float&	position = handle_Position.asFloat();
		MPlug	plug_FloatValue = plug.child(RAMP_FLOAT_VALUE, &ms);
		if (!ms) cout << ms << endl;
		MDataHandle	handle_FloatValue = handle.child(plug_FloatValue);
		float&	floatValue = handle_FloatValue.asFloat();
		MPlug	plug_Interp = plug.child(RAMP_INTERP_TYPE, &ms);
		if (!ms) cout << ms << endl;
		MDataHandle	handle_Interp = handle.child(plug_Interp);
		short&	interpType = handle_Interp.asShort();
		cout << "ramps[" << n << "]: Position: " << position << ", FloatValue = " << floatValue << ", InterpType = " << interpType << endl;
	}
}
#endif
#if 0
static void	copyRamp(
	MPlug	plug_ramp0,
	MDataHandle	handle_ramp0,
	MPlug	plug_ramp,
	MDataHandle	handle_ramp,
	MDataBlock	datablock,
	bool	directCopy
)
{
	MStatus	ms;
	MArrayDataHandle	arrayHandle0(handle_ramp0, &ms);
	MArrayDataHandle	arrayHandle(handle_ramp, &ms);
	unsigned	numElements0 = arrayHandle0.elementCount(&ms);
	cout << "numElements0 = " << numElements0 << " (" << ms << ")" << endl;
	unsigned	numElements = arrayHandle.elementCount(&ms);
	cout << "numElements = " << numElements << " (" << ms << ")" << endl;
	MPlug&	plug = plug_ramp;
	if (directCopy) {
		MArrayDataBuilder	builder(&datablock, plug, 0, &ms);
		for (int n = 0; n < (int) numElements0; ++n) {
#if 0
			MDataHandle	outputHandle = builder.addElement(n, &ms);
			if (!ms) cout << ms << endl;
#else
			ms = arrayHandle0.jumpToArrayElement(n);
			if (!ms) cout << ms << endl;
			MDataHandle	handleFrom = arrayHandle0.inputValue(&ms);
			if (!ms) cout << ms << endl;
			MDataHandle	handleTo = builder.addElement(n, &ms);
			if (!ms) cout << ms << endl;
			ms = handleTo.copy(handleFrom);
			if (!ms) cout << ms << endl;
#endif
		}
		ms = arrayHandle.set(builder);
		if (!ms) cout << ms << endl;
		numElements = arrayHandle.elementCount(&ms);
		if (!ms) cout << ms << endl;
		cout << "Now, # of numElements = " << numElements << " (" << ms << ")" << endl;
		assert(numElements == numElements0);
		return;
	}
	MPlug&	plug0 = plug_ramp0;
	if (numElements0 > numElements) {
		MArrayDataBuilder	builder(&datablock, plug, 0, &ms);
		if (!ms) cout << ms << endl;
		int	deltaN = (int) (numElements0 - numElements);
		for (int n = 0; n < deltaN; ++n) {
#if 1
			MDataHandle	outputHandle = builder.addElement(n, &ms);
			if (!ms) cout << ms << endl;
#else
			ms = arrayHandle0.jumpToArrayElement(n);
			if (!ms) cout << ms << endl;
			MDataHandle	handle = arrayHandle0.inputValue(&ms);
			if (!ms) cout << ms << endl;
			MDataHandle	outputHandle = builder.addElement(n, &ms);
			if (!ms) cout << ms << endl;
			if (!ms) cout << ms << endl;
#endif
		}
		ms = arrayHandle.set(builder);
		if (!ms) cout << ms << endl;
		numElements = arrayHandle.elementCount(&ms);
		if (!ms) cout << ms << endl;
		cout << "Now, # of numElements = " << numElements << " (" << ms << ")" << endl;
		assert(numElements == numElements0);
	}
	for (int n = 0; n < (int) numElements0; ++n) {
		ms = arrayHandle0.jumpToArrayElement(n);
		if (!ms) cout << ms << endl;
		MDataHandle	handle0 = arrayHandle0.inputValue(&ms);
		if (!ms) cout << ms << endl;
		MPlug	plug_Position0 = plug0.child(RAMP_POSITION, &ms);
		if (!ms) cout << ms << endl;
		MDataHandle	handle_Position0 = handle0.child(plug_Position0);
		float&	position0 = handle_Position0.asFloat();
		MPlug	plug_FloatValue0 = plug0.child(RAMP_FLOAT_VALUE, &ms);
		if (!ms) cout << ms << endl;
		MDataHandle	handle_FloatValue0 = handle0.child(plug_FloatValue0);
		float&	floatValue0 = handle_FloatValue0.asFloat();
		MPlug	plug_Interp0 = plug0.child(RAMP_INTERP_TYPE, &ms);
		if (!ms) cout << ms << endl;
		MDataHandle	handle_Interp0 = handle0.child(plug_Interp0);
		short&	interpType0 = handle_Interp0.asShort();
		cout << "Position0: " << position0 << ", FloatValue0 = " << floatValue0 << ", InterpType0 = " << interpType0 << endl;
		if (n >= (int) numElements) {
			cerr << "FATAL: numElements(" << numElements << ") < numElements0(" << numElements0 << "): No more spaces" << endl;
			continue;
		}
		ms = arrayHandle.jumpToArrayElement(n);
		if (!ms) cout << ms << endl;
		MDataHandle	handle = arrayHandle.inputValue(&ms);
		if (!ms) cout << ms << endl;
		MPlug	plug_Position = plug.child(RAMP_POSITION, &ms);
		if (!ms) cout << ms << endl;
		MDataHandle	handle_Position = handle.child(plug_Position);
		float&	position = handle_Position.asFloat();
		MPlug	plug_FloatValue = plug.child(RAMP_FLOAT_VALUE, &ms);
		if (!ms) cout << ms << endl;
		MDataHandle	handle_FloatValue = handle.child(plug_FloatValue);
		float&	floatValue = handle_FloatValue.asFloat();
		MPlug	plug_Interp = plug.child(RAMP_INTERP_TYPE, &ms);
		if (!ms) cout << ms << endl;
		MDataHandle	handle_Interp = handle.child(plug_Interp);
		short&	interpType = handle_Interp.asShort();
		cout << "Position: " << position << ", FloatValue = " << floatValue << ", InterpType = " << interpType << endl;
		position = position0;
		floatValue = floatValue0;
		interpType = interpType0;
		cout << "[" << n << "]: Position: " << position << ", FloatValue = " << floatValue << ", InterpType = " << interpType << endl;
	}
	for (int n = numElements0; n < (int) numElements; ++n) {
		ms = arrayHandle.jumpToArrayElement(n);
		if (!ms) cout << ms << endl;
		MDataHandle	handle = arrayHandle.inputValue(&ms);
		if (!ms) cout << ms << endl;
		MPlug	plug_Position = plug.child(RAMP_POSITION, &ms);
		if (!ms) cout << ms << endl;
		MDataHandle	handle_Position = handle.child(plug_Position);
		float&	position = handle_Position.asFloat();
		MPlug	plug_FloatValue = plug.child(RAMP_FLOAT_VALUE, &ms);
		if (!ms) cout << ms << endl;
		MDataHandle	handle_FloatValue = handle.child(plug_FloatValue);
		float&	floatValue = handle_FloatValue.asFloat();
		MPlug	plug_Interp = plug.child(RAMP_INTERP_TYPE, &ms);
		if (!ms) cout << ms << endl;
		MDataHandle	handle_Interp = handle.child(plug_Interp);
		short&	interpType = handle_Interp.asShort();
		cout << "Position: " << position << ", FloatValue = " << floatValue << ", InterpType = " << interpType << endl;
		position = -1.f;
		floatValue = -1.f;
		interpType = -1;
		cout << "[" << n << "]: Position: " << position << ", FloatValue = " << floatValue << ", InterpType = " << interpType << endl;
	}
}
#endif
void	tentacleShape::__activeListModifiedCallback(void* voidPtr)
{
	if (!voidPtr)
		return;
	MStatus	ms;
	MSelectionList	list;
	MGlobal::getActiveSelectionList(list);
	int	physicalIndex = -1;
	MIntArray	physicalIndexArray;
	for (int i = 0; i < (int) list.length(); ++i) {
		MDagPath	dagPath;
		MObject		component;
		ms = list.getDagPath(i, dagPath, component);
		cout << "dagPath[" << i << "]: <" << dagPath.fullPathName(&ms) << "> (" << ms << ")" << endl;
		if (
			component.hasFn(MFn::kSingleIndexedComponent) &&
			component.hasFn(MFn::kMeshEdgeComponent)
		) {
			MStatus	ms;
			MFnSingleIndexedComponent	fnEdgeComp(component, &ms);
			if (fnEdgeComp.elementCount(&ms)) {
#if 1
				for (int k = 0; k < (int) fnEdgeComp.elementCount(); ++k) {
					cout << "[" << k << "]: physical index = " << fnEdgeComp.element(k, &ms) << endl;
					physicalIndexArray.append(fnEdgeComp.element(k));
				}
#endif
				physicalIndex = fnEdgeComp.element(0, &ms);
				if (ms && physicalIndex != -1)
					break;
			}
		}
	}
	if (lastSelectedPhysicalIndex != -1) {
		bool	found = false;
		for (int n = 0; n < (int) physicalIndexArray.length(); ++n) {
			if (lastSelectedPhysicalIndex == physicalIndexArray[n]) {
				found = true;
				break;
			}
		}
		if (physicalIndexArray.length() && !found) {
			MGlobal::displayError("lastSelectedPhysicalIndex != -1 but not found in physicalIndexArray");
		}
	} else {
		if (physicalIndexArray.length()) {
			MGlobal::displayWarning("lastSelectedPhysicalIndex = -1 but physicalIndexArray.length > 0");
		}
	}
	if (physicalIndex == -1) {
		if (lastSelectedPhysicalIndex != -1) {
			MGlobal::displayWarning("physicalIndex = -1 but lastSelectedPhysicalIndex != -1");
		}
		lastSelectedPhysicalIndex = -1;
		return;
	}
	if (lastSelectedPhysicalIndex == -1)
		return;
	physicalIndex = lastSelectedPhysicalIndex;
	tentacleShape&	tentacleNode = *((tentacleShape*) voidPtr);
#if 0
	if (tentacleNode.updateConnection) {
		cout << "updateConnection: True" << endl;
		MStatus	ms;
		MPlug	plug_meshArray(tentacleNode.thisMObject(), outputMesh);
		for (unsigned n = 0; n < plug_meshArray.numElements(); ++n) {
			cout << "outMesh[" << n << "]:..." << endl;
#if 1
	MString	command("createNode transform");
	MString	meshTransformNode;
	ms = MGlobal::executeCommand(command, meshTransformNode, true);
	cout << ms << endl;
	cout << "|" << meshTransformNode << "|" << endl;
	command = "createNode mesh -p ";
	command += meshTransformNode;
	MString	meshShapeNode;
	ms = MGlobal::executeCommand(command, meshShapeNode, true);
	cout << ms << endl;
	cout << "|" << meshShapeNode << "|" << endl;
	command = "sets -add initialShadingGroup ";
	command += meshShapeNode;
	MString	shadingEngineNode;
	ms = MGlobal::executeCommand(command, shadingEngineNode, true);
	cout << ms << endl;
	cout << "|" << shadingEngineNode << "|" << endl;
#if 1
	command = "connectAttr m1.outputMesh[";
	command += n;
	command += "] ";
	command += meshShapeNode;
	command += ".inMesh";
	MString	result;
	ms = MGlobal::executeCommand(command, result, true);
	cout << ms << endl;
	cout << "|" << result << "|" << endl;
#endif
#endif
		}
		tentacleNode.updateConnection = false;
	}
#endif
	MDataBlock	datablock = tentacleNode.forceCache();
	MPlug	plug_tentacleArray(tentacleNode.thisMObject(), tentacleShape::__tentacleArray);
	MArrayDataHandle	arrayHandle_tentacleArray = datablock.inputArrayValue(plug_tentacleArray, &ms);
	if (physicalIndex >= (int) arrayHandle_tentacleArray.elementCount(&ms)) {
		if (lastSelectedPhysicalIndex != -1) {
			MString	msg("physicalIndex NOT valid but last selected physical index(");
			msg += lastSelectedPhysicalIndex;
			msg += ") is VALID";
			MGlobal::displayError(msg);
		}
		cerr << "physical index = " << physicalIndex << " (last selected physical index = " << lastSelectedPhysicalIndex << ") but # of elements = " << arrayHandle_tentacleArray.elementCount(&ms) << endl;
		return;
	}
#if MAYA_API_VERSION >= 200806
	ms = arrayHandle_tentacleArray.jumpToArrayElement(physicalIndex);
#else
	ms = arrayHandle_tentacleArray.jumpToElement(physicalIndex);
#endif
	MDataHandle	handle_tentacle = arrayHandle_tentacleArray.inputValue(&ms);
	MPlug		plug_longitudinal = plug_tentacleArray.child(TENTACLE_ARRAY_LOGITUDINAL, &ms);
	MDataHandle	handle_longitudinal = handle_tentacle.child(plug_longitudinal);
	MObject		longitudinalData = handle_longitudinal.data();
	MFnVectorArrayData	vectorData(longitudinalData, &ms);
	MVectorArray	longitudinals = vectorData.array(&ms);
	assert(longitudinals.length() == NUM_TENTACLE_SAMPLE_POINTS - 1);
	double	length0 = (NUM_TENTACLE_SAMPLE_POINTS - 1) * longitudinals[1].length();
	MPlug		plug_majorRadius0 = plug_tentacleArray.child(TENTACLE_ARRAY_MAJOR_RADIUS, &ms);
	MDataHandle	handle_majorRadius0 = handle_tentacle.child(plug_majorRadius0);
	MPlug		plug_majorRadius_ramp0 = plug_tentacleArray.child(TENTACLE_ARRAY_MAJOR_RADIUS_RAMP, &ms);
	MDataHandle	handle_majorRadius_ramp0 = handle_tentacle.child(plug_majorRadius_ramp0);
	MPlug		plug_eccentricity0 = plug_tentacleArray.child(TENTACLE_ARRAY_ECCENTRICITY, &ms);
	MDataHandle	handle_eccentricity0 = handle_tentacle.child(plug_eccentricity0);
	MPlug		plug_eccentricity_ramp0 = plug_tentacleArray.child(TENTACLE_ARRAY_ECCENTRICITY_RAMP, &ms);
	MDataHandle	handle_eccentricity_ramp0 = handle_tentacle.child(plug_eccentricity_ramp0);
	MPlug		plug_rotation0 = plug_tentacleArray.child(TENTACLE_ARRAY_ROTATION, &ms);
	MDataHandle	handle_rotation0 = handle_tentacle.child(plug_rotation0);
	MPlug		plug_rotation_ramp0 = plug_tentacleArray.child(TENTACLE_ARRAY_ROTATION_RAMP, &ms);
	MDataHandle	handle_rotation_ramp0 = handle_tentacle.child(plug_rotation_ramp0);
	MPlug		plug_cut0 = plug_tentacleArray.child(TENTACLE_ARRAY_CUT, &ms);
	MDataHandle	handle_cut0 = handle_tentacle.child(plug_cut0);
	MPlug	plug_tentacleParameter(tentacleNode.thisMObject(), tentacleShape::__tentacleParameter);
	MDataHandle	handle_tentacleParameter = datablock.outputValue(plug_tentacleParameter, &ms);
	MPlug	plug_length = plug_tentacleParameter.child(TENTACLE_PARAM_LENGTH, &ms);
	MDataHandle	handle_length = handle_tentacleParameter.child(plug_length);
	MPlug	plug_length_sync = plug_length.child(TENTACLE_ATTRIBUTE_EDITOR_SYNC, &ms);
	MDataHandle	handle_length_sync = handle_length.child(plug_length_sync);
	if (handle_length_sync.asBool()) {
		MPlug	plug_length_val = plug_length.child(TENTACLE_ATTRIBUTE_EDITOR_VAL, &ms);
		MDataHandle	handle_length_val = handle_length.child(plug_length_val);
		handle_length_val.asDouble() = length0;
	}
	MPlug	plug_majorRadius = plug_tentacleParameter.child(TENTACLE_PARAM_CROSS_SECTION_MAJOR_RADIUS, &ms);
	MDataHandle	handle_majorRadius = handle_tentacleParameter.child(plug_majorRadius);
	MPlug	plug_majorRadius_sync = plug_majorRadius.child(TENTACLE_ATTRIBUTE_EDITOR_SYNC, &ms);
	MDataHandle	handle_majorRadius_sync = handle_majorRadius.child(plug_majorRadius_sync);
	if (handle_majorRadius_sync.asBool()) {
		MPlug	plug_majorRadius_val = plug_majorRadius.child(TENTACLE_ATTRIBUTE_EDITOR_VAL, &ms);
		MDataHandle	handle_majorRadius_val = handle_majorRadius.child(plug_majorRadius_val);
		ms = handle_majorRadius_val.copy(handle_majorRadius0);
		MPlug	plug_majorRadius_ramp = plug_majorRadius.child(TENTACLE_ATTRIBUTE_EDITOR_RAMP, &ms);
		MDataHandle	handle_majorRadius_ramp = handle_majorRadius.child(plug_majorRadius_ramp);
#if 1
		ms = handle_majorRadius_ramp.copy(handle_majorRadius_ramp0);
#else
		copyRamp(plug_majorRadius_ramp0, handle_majorRadius_ramp0, plug_majorRadius_ramp, handle_majorRadius_ramp, datablock, true);
#endif
	}
	MPlug	plug_eccentricity = plug_tentacleParameter.child(TENTACLE_PARAM_CROSS_SECTION_ECCENTRICITY, &ms);
	MDataHandle	handle_eccentricity = handle_tentacleParameter.child(plug_eccentricity);
	MPlug	plug_eccentricity_sync = plug_eccentricity.child(TENTACLE_ATTRIBUTE_EDITOR_SYNC, &ms);
	MDataHandle	handle_eccentricity_sync = handle_eccentricity.child(plug_eccentricity_sync);
	if (handle_eccentricity_sync.asBool()) {
		MPlug	plug_eccentricity_val = plug_eccentricity.child(TENTACLE_ATTRIBUTE_EDITOR_VAL, &ms);
		MDataHandle	handle_eccentricity_val = handle_eccentricity.child(plug_eccentricity_val);
		ms = handle_eccentricity_val.copy(handle_eccentricity0);
		MPlug	plug_eccentricity_ramp = plug_eccentricity.child(TENTACLE_ATTRIBUTE_EDITOR_RAMP, &ms);
		MDataHandle	handle_eccentricity_ramp = handle_eccentricity.child(plug_eccentricity_ramp);
#if 1
		ms = handle_eccentricity_ramp.copy(handle_eccentricity_ramp0);
#else
		copyRamp(plug_eccentricity_ramp0, handle_eccentricity_ramp0, plug_eccentricity_ramp, handle_eccentricity_ramp, datablock, true);
#endif
	}
	MPlug	plug_rotation = plug_tentacleParameter.child(TENTACLE_PARAM_CROSS_SECTION_ROTATION, &ms);
	MDataHandle	handle_rotation = handle_tentacleParameter.child(plug_rotation);
	MPlug	plug_rotation_sync = plug_rotation.child(TENTACLE_ATTRIBUTE_EDITOR_SYNC, &ms);
	MDataHandle	handle_rotation_sync = handle_rotation.child(plug_rotation_sync);
	if (handle_rotation_sync.asBool()) {
		MPlug	plug_rotation_val = plug_rotation.child(TENTACLE_ATTRIBUTE_EDITOR_VAL, &ms);
		MDataHandle	handle_rotation_val = handle_rotation.child(plug_rotation_val);
		ms = handle_rotation_val.copy(handle_rotation0);
		MPlug	plug_rotation_ramp = plug_rotation.child(TENTACLE_ATTRIBUTE_EDITOR_RAMP, &ms);
		MDataHandle	handle_rotation_ramp = handle_rotation.child(plug_rotation_ramp);
#if 1
		ms = handle_rotation_ramp.copy(handle_rotation_ramp0);
#else
		copyRamp(plug_rotation_ramp0, handle_rotation_ramp0, plug_rotation_ramp, handle_rotation_ramp, datablock, true);
#endif
	}
	MPlug	plug_cut = plug_tentacleParameter.child(TENTACLE_PARAM_CUT, &ms);
	MDataHandle	handle_cut = handle_tentacleParameter.child(plug_cut);
	MPlug	plug_cut_sync = plug_cut.child(TENTACLE_ATTRIBUTE_EDITOR_SYNC, &ms);
	MDataHandle	handle_cut_sync = handle_cut.child(plug_cut_sync);
	if (handle_cut_sync.asBool()) {
		MPlug	plug_cut_val = plug_cut.child(TENTACLE_ATTRIBUTE_EDITOR_VAL, &ms);
		MDataHandle	handle_cut_val = handle_cut.child(plug_cut_val);
		ms = handle_cut_val.copy(handle_cut0);
	}
}
#endif
#if 1
MStatus	tentacleShape::__computeBoundingBox(const MPlug& plug_tentacleArray, MDataBlock& datablock)
{
	MStatus	ms = MStatus::kSuccess;
	MDataHandle	lowerHandle = datablock.outputValue(bboxCorner1, &ms);
	MDataHandle	upperHandle = datablock.outputValue(bboxCorner2, &ms);
	double3&	lower = lowerHandle.asDouble3();
	double3&	upper = upperHandle.asDouble3();
	lower[0] = DBL_MAX;
	lower[1] = DBL_MAX;
	lower[2] = -DBL_MAX;
	upper[0] = -DBL_MAX;
	upper[1] = -DBL_MAX;
	upper[2] = DBL_MAX;
	MArrayDataHandle	handleArray = datablock.inputArrayValue(__tentacleArray, &ms);
	const MMatrix&	m = __externalInclusiveMatrix;
#if 1
	bool	hasSkinCluster = __skinSmoothBind.hasSkinCluster();
#endif
	for (int n = 0; n < (int) handleArray.elementCount(&ms); ++n) {
#if MAYA_API_VERSION >= 200806
		ms = handleArray.jumpToArrayElement(n);
#else
		ms = handleArray.jumpToElement(n);
#endif
		MDataHandle	handle = handleArray.inputValue(&ms);
		MPlug	plug = plug_tentacleArray.elementByPhysicalIndex(n, &ms);
#if 1
	if (hasSkinCluster) {
		MPlug		plug_longitudinal = plug.child(TENTACLE_ARRAY_LOGITUDINAL, &ms);
		MDataHandle	handle_longitudinal = handle.child(plug_longitudinal);
		MObject		longitudinalData = handle_longitudinal.data();
		MFnVectorArrayData	vectorData(longitudinalData, &ms);
		MVectorArray	longitudinals = vectorData.array(&ms);
		assert(longitudinals.length() == NUM_TENTACLE_SAMPLE_POINTS - 1);
		float	segment_length = (float) longitudinals[0].length();
		MVector		root_wNormal;
		MFloatVectorArray	wPoints;
		ms = __skinSmoothBind.getPoints(n, plug, segment_length, root_wNormal, wPoints);
		if (!ms)
			continue;
		for (int k = 0; k < (int) NUM_TENTACLE_SAMPLE_POINTS; ++k) {
			MPoint	point(wPoints[k]);
			if (point[0] < lower[0]) lower[0] = point[0];
			if (point[1] < lower[1]) lower[1] = point[1];
			if (point[2] > lower[2]) lower[2] = point[2];
			if (point[0] > upper[0]) upper[0] = point[0];
			if (point[1] > upper[1]) upper[1] = point[1];
			if (point[2] < upper[2]) upper[2] = point[2];
		}
	} else {
#endif
		MPlug		plug_root = plug.child(TENTACLE_ARRAY_ROOT, &ms);
		MDataHandle	handle_root = handle.child(plug_root);
#if 1
		MPlug	plug_root_position = plug_root.child(TENTACLE_ROOT_POSITION, &ms);
		MDataHandle	handle_root_position = handle_root.child(plug_root_position);
		double3&	root = handle_root_position.asDouble3();
#else
		double3&	root = handle_root.asDouble3();
#endif
		MPlug		plug_longitudinal = plug.child(TENTACLE_ARRAY_LOGITUDINAL, &ms);
		MDataHandle	handle_longitudinal = handle.child(plug_longitudinal);
		MObject		longitudinalData = handle_longitudinal.data();
		MFnVectorArrayData	vectorData(longitudinalData, &ms);
		MVectorArray	longitudinals = vectorData.array(&ms);
		assert(longitudinals.length() == NUM_TENTACLE_SAMPLE_POINTS - 1);
		if (longitudinals.length() != NUM_TENTACLE_SAMPLE_POINTS - 1) {
			cerr << "FATAL: longitudinals.length()(" << longitudinals.length() << ") != NUM_TENTACLE_SAMPLE_POINTS - 1(" << (NUM_TENTACLE_SAMPLE_POINTS - 1) << "): n = " << n << ": logical index = " << handleArray.elementIndex(&ms) << " (" << ms << ")" << endl;
			continue;
		}
		MPoint	basePoint0;
		basePoint0[0] = root[0]; basePoint0[1] = root[1]; basePoint0[2] = root[2];
		MPoint	basePoint = basePoint0 * m;
		MPoint	point = basePoint;
		for (int k = 0; k < (int) NUM_TENTACLE_SAMPLE_POINTS; ++k) {
			if (k) {
				MVector	offset = longitudinals[k - 1];
				double	len = offset.length();
				offset = offset.transformAsNormal(m);
				point += len * offset;
			}
			if (point[0] < lower[0]) lower[0] = point[0];
			if (point[1] < lower[1]) lower[1] = point[1];
			if (point[2] > lower[2]) lower[2] = point[2];
			if (point[0] > upper[0]) upper[0] = point[0];
			if (point[1] > upper[1]) upper[1] = point[1];
			if (point[2] < upper[2]) upper[2] = point[2];
		}
#if 1
	}
#endif
	}
	lowerHandle.setClean();
	upperHandle.setClean();
	childChanged(MPxSurfaceShape::kBoundingBoxChanged);
	return ms;
}
#if 2
MStatus	tentacleShape::__computeBoundingBox(MDataBlock& datablock)
{
	MStatus	ms = MStatus::kSuccess;
	MDataHandle	lowerHandle = datablock.outputValue(bboxCorner1, &ms);
	MDataHandle	upperHandle = datablock.outputValue(bboxCorner2, &ms);
	double3&	lower = lowerHandle.asDouble3();
	double3&	upper = upperHandle.asDouble3();
	lower[0] = DBL_MAX;
	lower[1] = DBL_MAX;
	lower[2] = -DBL_MAX;
	upper[0] = -DBL_MAX;
	upper[1] = -DBL_MAX;
	upper[2] = DBL_MAX;
	for (int i = 0; i < (int) __strand_longitudinals.length(); ++i) {
		MPoint	point = __strand_longitudinals[i];
		if (point[0] < lower[0]) lower[0] = point[0];
		if (point[1] < lower[1]) lower[1] = point[1];
		if (point[2] > lower[2]) lower[2] = point[2];
		if (point[0] > upper[0]) upper[0] = point[0];
		if (point[1] > upper[1]) upper[1] = point[1];
		if (point[2] < upper[2]) upper[2] = point[2];
	}
	lowerHandle.setClean();
	upperHandle.setClean();
	childChanged(MPxSurfaceShape::kBoundingBoxChanged);
	return ms;
}
#endif
#endif
#if 0
void	tentacleShape::getTransverseVectors(MVectorArray& transVectors)
{
	transVectors.clear();
	MPlug	plug_trans_array(thisMObject(), transversal);
	int	numVertices = meshGeom()->vertices.length();
	for (int i = 0; i < (int) (numVertices * N_POINTS_IN_GUIDE); ++i) {
		MPlug	plug_compound = plug_trans_array.elementByLogicalIndex(i);
		MObject	data;
		MStatus	ms = plug_compound.getValue(data);
		MFnNumericData	val(data, &ms);
		MVector	transVector;
		ms = val.getData(transVector.x, transVector.y, transVector.z);
		transVectors.append(transVector);
	}
}
#endif
#if 1
#if 1
MStatus	tentacleShape::getDrawTentacleRootStatus(bool& status)
{
	MPlug	plug(thisMObject(), __drawTentacleRoot);
	return plug.getValue(status);
}
MStatus	tentacleShape::getTentacleRootPosition(MPoint& position)
{
	MStatus	ms;
	MPlug	plug_tentacle_root(thisMObject(), __tentacleRoot);
	MPlug	plug = plug_tentacle_root.child(TENTACLE_ROOT_POSITION, &ms);
	MObject	data;
	ms = plug.getValue(data);
	if (!ms)
		return ms;
	MFnNumericData	val(data, &ms);
	if (!ms)
		return ms;
	return val.getData(position.x, position.y, position.z);
}
#endif
MStatus	tentacleShape::getTentacleArray(kPtrArray<kTentacle>& tentacleArray)
{
	MStatus	ms;
	tentacleArray.clear();
	MPlug	plug_tentacleArray(thisMObject(), __tentacleArray);
	unsigned	numElements = plug_tentacleArray.evaluateNumElements(&ms);
#if 1
#if 1
#if 1
#if 1
	return getTentacleArray(plug_tentacleArray, numElements, tentacleArray, __skinSmoothBind.hasSkinCluster() ? &__skinSmoothBind : NULL);
#else
	return getTentacleArray(plug_tentacleArray, numElements, tentacleArray);
#endif
#else
{
	for (unsigned n = 0; n < numElements; ++n) {
		const kTentacle&	tentacle = tentacleArray(n);
		MPlug	plug_mesh = plug_meshArray.elementByPhysicalIndex(n, &ms);
		if (!ms) continue;
		MObject	inMeshObj;
		ms = plug_mesh.getValue(inMeshObj);
		if (!ms) continue;
		MFnMesh	mesh(inMeshObj, &ms);
		if (!ms) continue;
		MPointArray	strandSamplePoints;
		MPointArray	tentacleSamplePoints;
		tentacle.getSamplePoints(externalInclusiveMatrix(), strandSamplePoints, numEllipseSamplePoints, tentacleSamplePoints);
#if 0
		MPointArray	points;
		ms = mesh.getPoints(points, MSpace::kWorld);
		cout << ms << endl;
		cout << "points: " << points << endl;
		cout << "tentacleSamplePoints: " << tentacleSamplePoints << endl;
#endif
		ms = mesh.setPoints(tentacleSamplePoints, MSpace::kWorld);
#if 0
		ms = mesh.getPoints(points, MSpace::kWorld);
		cout << "points: " << points << endl;
#endif
#if 1
		MFnMeshData	meshDataFn;
		MObject	newMeshObj = meshDataFn.create();
		MFnMesh	newMeshFn;
		newMeshFn.copy(inMeshObj, newMeshObj);
		newMeshFn.setObject(newMeshObj);
		plug_mesh.setValue(newMeshObj);
#endif
	}
}
	return ms;
#endif
#else
	return getTentacleArray(plug_tentacleArray, numElements, tentacleArray);
#endif
#else
	for (unsigned n = 0; n < numElements; ++n) {
		MPlug	plug = plug_tentacleArray.elementByPhysicalIndex(n, &ms);
		kTentacle*	tentaclePtr = new kTentacle;
		kTentacle&	tentacle = *tentaclePtr;
		MObject	valData;
		MFnNumericData		numericData;
		MFnVectorArrayData	vectorData;
		MFnDoubleArrayData	doubleData;
		MPlug	plug_root = plug.child(TENTACLE_ARRAY_ROOT, &ms);
		ms = plug_root.getValue(valData);
		ms = numericData.setObject(valData);
		MPoint&	root = tentacle.root;
		ms = numericData.getData(root[0], root[1], root[2]);
		MPlug	plug_longitudinal = plug.child(TENTACLE_ARRAY_LOGITUDINAL, &ms);
		ms = plug_longitudinal.getValue(valData);
#if 1
		ms = vectorData.setObject(valData);
		ms = vectorData.copyTo(tentacle.longitudinal);
#else
		tentacle.longitudinal = vectorData.array(&ms);
#endif
		assert(vectorData.array().length() == NUM_TENTACLE_SAMPLE_POINTS - 1);
		MPlug	plug_transversal = plug.child(TENTACLE_ARRAY_TRANSVERSAL, &ms);
		ms = plug_transversal.getValue(valData);
#if 1
		ms = vectorData.setObject(valData);
		ms = vectorData.copyTo(tentacle.transversal);
#else
		tentacle.transversal = vectorData.array(&ms);
#endif
		assert(vectorData.array().length() == NUM_TENTACLE_SAMPLE_POINTS);
		MPlug	plug_a = plug.child(TENTACLE_ARRAY_MAJOR_RADIUS, &ms);
		double	a;
		ms = plug_a.getValue(a);
		MPlug		plug_scale_a = plug.child(TENTACLE_ARRAY_MAJOR_RADIUS_SCALE, &ms);
		ms = plug_scale_a.getValue(valData);
		ms = doubleData.setObject(valData);
#if 1
		MDoubleArray	scale_a;
		ms = doubleData.copyTo(scale_a);
#else
		MDoubleArray	scale_a = doubleData.array(&ms);
#endif
		assert(scale_a.length() == NUM_TENTACLE_SAMPLE_POINTS);
		for (int k = 0; k < (int) scale_a.length(); ++k)
			tentacle.majorAxis[k] = a * scale_a[k];
		MPlug	plug_e = plug.child(TENTACLE_ARRAY_ECCENTRICITY, &ms);
		double	e;
		ms = plug_e.getValue(e);
		MPlug		plug_scale_e = plug.child(TENTACLE_ARRAY_ECCENTRICITY_SCALE, &ms);
		ms = plug_scale_e.getValue(valData);
		ms = doubleData.setObject(valData);
#if 1
		MDoubleArray	scale_e;
		ms = doubleData.copyTo(scale_e);
#else
		MDoubleArray	scale_e = doubleData.array(&ms);
#endif
		assert(scale_e.length() == NUM_TENTACLE_SAMPLE_POINTS);
		for (int k = 0; k < (int) scale_e.length(); ++k)
			tentacle.eccentricity[k] = e * scale_e[k];
		MPlug	plug_rotation = plug.child(TENTACLE_ARRAY_ROTATION, &ms);
		double	global_rotation;
		ms = plug_rotation.getValue(global_rotation);
		MPlug		plug_local_rotation = plug.child(TENTACLE_ARRAY_ROTATION_LOCAL, &ms);
		ms = plug_local_rotation.getValue(valData);
		ms = doubleData.setObject(valData);
#if 1
		MDoubleArray	local_rotation;
		ms = doubleData.copyTo(local_rotation);
#else
		MDoubleArray	local_rotation = doubleData.array(&ms);
#endif
		assert(local_rotation.length() == NUM_TENTACLE_SAMPLE_POINTS);
		for (int k = 0; k < (int) local_rotation.length(); ++k)
			tentacle.rotation[k] = global_rotation + (local_rotation[k] - 0.5);
		MPlug	plug_cut = plug.child(TENTACLE_ARRAY_CUT, &ms);
		kCut&	cut = *((kCut*) &tentacle);
		MPlug	plug_cut_have = plug_cut.child(TENTACLE_CUT_VAL_HAVE, &ms);
		ms = plug_cut_have.getValue(tentacle.have_cut);
		MPlug	plug_cut_t = plug_cut.child(TENTACLE_CUT_VAL_T, &ms);
		ms = plug_cut_t.getValue(cut.t);
		MPlug	plug_cut_polar = plug_cut.child(TENTACLE_CUT_VAL_POLAR, &ms);
		ms = plug_cut_polar.getValue(cut.polar);
		MPlug	plug_cut_azimuthal = plug_cut.child(TENTACLE_CUT_VAL_AZIMUTHAL, &ms);
		ms = plug_cut_azimuthal.getValue(cut.azimuthal);
		tentacleArray.append(tentaclePtr);
	}
	return MStatus::kSuccess;
#endif
}
MStatus	tentacleShape::getTentacleArray(
	const MPlug&	plug_tentacleArray,
	unsigned	numElements,
#if 1
	kPtrArray<kTentacle>&	tentacleArray,
	const kSkinSmoothBind*	skinSmoothBindPtr
#else
	kPtrArray<kTentacle>&	tentacleArray
#endif
)
{
	MStatus	ms;
	tentacleArray.clear();
	for (unsigned tentacleId = 0; tentacleId < numElements; ++tentacleId) {
		MPlug	plug = plug_tentacleArray.elementByPhysicalIndex(tentacleId, &ms);
		kTentacle*	tentaclePtr = new kTentacle;
		kTentacle&	tentacle = *tentaclePtr;
		MObject	valData;
		MFnNumericData		numericData;
		MFnVectorArrayData	vectorData;
		MFnDoubleArrayData	doubleData;
		MPlug	plug_root = plug.child(TENTACLE_ARRAY_ROOT, &ms);
#if 1
		MPlug	plug_root_position = plug_root.child(TENTACLE_ROOT_POSITION, &ms);
		ms = plug_root_position.getValue(valData);
#else
		ms = plug_root.getValue(valData);
#endif
		ms = numericData.setObject(valData);
#if 1
		MPoint&	root = ((kRoot&) tentacle).position;
		ms = numericData.getData(root[0], root[1], root[2]);
		MPlug	plug_root_polyId = plug_root.child(TENTACLE_ROOT_POLY_ID, &ms);
		int&	polyId = ((kRoot&) tentacle).polyId;
		ms = plug_root_polyId.getValue(polyId);
		MPlug	plug_root_normal = plug_root.child(TENTACLE_ROOT_NORMAL, &ms);
		ms = plug_root_normal.getValue(valData);
		ms = numericData.setObject(valData);
		MVector&	normal = ((kRoot&) tentacle).normal;
		ms = numericData.getData(normal[0], normal[1], normal[2]);
#else
		MPoint&	root = tentacle.root;
		ms = numericData.getData(root[0], root[1], root[2]);
#endif
		MPlug	plug_longitudinal = plug.child(TENTACLE_ARRAY_LOGITUDINAL, &ms);
		ms = plug_longitudinal.getValue(valData);
#if 1
		ms = vectorData.setObject(valData);
		ms = vectorData.copyTo(tentacle.longitudinal);
#else
		tentacle.longitudinal = vectorData.array(&ms);
#endif
		assert(vectorData.array().length() == NUM_TENTACLE_SAMPLE_POINTS - 1);
#if 1
if (skinSmoothBindPtr) {
	float	segment_length = (float) tentacle.longitudinal[0].length();
	ms = skinSmoothBindPtr->getPoints(tentacleId, plug, segment_length, tentacle.wNormal, tentacle.wPoints);
}
#endif
		MPlug	plug_transversal = plug.child(TENTACLE_ARRAY_TRANSVERSAL, &ms);
		ms = plug_transversal.getValue(valData);
#if 1
		ms = vectorData.setObject(valData);
		ms = vectorData.copyTo(tentacle.transversal);
#else
		tentacle.transversal = vectorData.array(&ms);
#endif
		assert(vectorData.array().length() == NUM_TENTACLE_SAMPLE_POINTS);
		MPlug	plug_a = plug.child(TENTACLE_ARRAY_MAJOR_RADIUS, &ms);
		double	a;
		ms = plug_a.getValue(a);
		MPlug		plug_scale_a = plug.child(TENTACLE_ARRAY_MAJOR_RADIUS_SCALE, &ms);
		ms = plug_scale_a.getValue(valData);
		ms = doubleData.setObject(valData);
#if 1
		MDoubleArray	scale_a;
		ms = doubleData.copyTo(scale_a);
#else
		MDoubleArray	scale_a = doubleData.array(&ms);
#endif
		assert(scale_a.length() == NUM_TENTACLE_SAMPLE_POINTS);
		for (int k = 0; k < (int) scale_a.length(); ++k)
			tentacle.majorAxis[k] = a * scale_a[k];
		MPlug	plug_e = plug.child(TENTACLE_ARRAY_ECCENTRICITY, &ms);
		double	e;
		ms = plug_e.getValue(e);
		MPlug		plug_scale_e = plug.child(TENTACLE_ARRAY_ECCENTRICITY_SCALE, &ms);
		ms = plug_scale_e.getValue(valData);
		ms = doubleData.setObject(valData);
#if 1
		MDoubleArray	scale_e;
		ms = doubleData.copyTo(scale_e);
#else
		MDoubleArray	scale_e = doubleData.array(&ms);
#endif
		assert(scale_e.length() == NUM_TENTACLE_SAMPLE_POINTS);
		for (int k = 0; k < (int) scale_e.length(); ++k)
			tentacle.eccentricity[k] = e * scale_e[k];
		MPlug	plug_rotation = plug.child(TENTACLE_ARRAY_ROTATION, &ms);
		double	global_rotation;
		ms = plug_rotation.getValue(global_rotation);
		MPlug		plug_local_rotation = plug.child(TENTACLE_ARRAY_ROTATION_LOCAL, &ms);
		ms = plug_local_rotation.getValue(valData);
		ms = doubleData.setObject(valData);
#if 1
		MDoubleArray	local_rotation;
		ms = doubleData.copyTo(local_rotation);
#else
		MDoubleArray	local_rotation = doubleData.array(&ms);
#endif
		assert(local_rotation.length() == NUM_TENTACLE_SAMPLE_POINTS);
		for (int k = 0; k < (int) local_rotation.length(); ++k)
			tentacle.rotation[k] = global_rotation + local_rotation[k];
		MPlug	plug_cut = plug.child(TENTACLE_ARRAY_CUT, &ms);
		kCut&	cut = *((kCut*) &tentacle);
#if 1
		MPlug	plug_cut_frame = plug_cut.child(TENTACLE_CUT_VAL_FRAME, &ms);
		ms = plug_cut_frame.getValue(tentacle.cut_frame);
#else
		MPlug	plug_cut_have = plug_cut.child(TENTACLE_CUT_VAL_HAVE, &ms);
		ms = plug_cut_have.getValue(tentacle.have_cut);
#endif
		MPlug	plug_cut_t = plug_cut.child(TENTACLE_CUT_VAL_T, &ms);
		ms = plug_cut_t.getValue(cut.t);
		MPlug	plug_cut_polar = plug_cut.child(TENTACLE_CUT_VAL_POLAR, &ms);
		ms = plug_cut_polar.getValue(cut.polar);
		MPlug	plug_cut_azimuthal = plug_cut.child(TENTACLE_CUT_VAL_AZIMUTHAL, &ms);
		ms = plug_cut_azimuthal.getValue(cut.azimuthal);
		tentacleArray.append(tentaclePtr);
	}
	return MStatus::kSuccess;
}
#if 3
#if 1
#if 0
MStatus	tentacleShape::__setSkinSmoothBind()
{
	MStatus	ms;
	if (!__externalDagPathShape.isValid(&ms)) {
		cerr << "FATAL: tentacleShape::__setSkinSmoothBind: __externalDagPathShape: NOT valid" << endl;
		return MStatus::kFailure;
	}
	ms = __skinSmoothBind.setSkinCluster(__externalDagPathShape);
	ms = __skinSmoothBind.setGeometryMatrix();
	ms = __skinSmoothBind.setInverseBindingMatrixArray();
	ms = __skinSmoothBind.setWeightsArray();
	ms = __skinSmoothBind.setJointHierachy();
	ms = __skinSmoothBind.setJointWorldMatrixArray();
#if 0
	MTime	t;
	for (int i = 0; i < 24; ++i) {
		t.setValue(1 + i);
		cout << "[#" << (1 + i) << "]: t = " << t.value() << endl;
		ms = __skinSmoothBind.setJointWorldMatrixArray(&t);
	}
#endif
	MPlug	plug_tentacleArray(thisMObject(), __tentacleArray);
	ms = __skinSmoothBind.setCache(plug_tentacleArray);
	return MStatus::kSuccess;
}
#endif
#endif
MStatus	tentacleShape::__setTentacleMesh()
{
	MStatus	ms;
	MPlug	plug_meshArray(thisMObject(), outputMesh);
	bool	isArray = plug_meshArray.isArray(&ms);
	if (!ms) {
		cerr << "ERR: tentacleShape::__setTentacleMesh: |.outputMesh|: MPlug::isArray(): Failure" << endl;
		return ms;
	}
	if (isArray == false) {
		cerr << "ERR: tentacleShape::__setTentacleMesh: |.outputMesh| not an array" << endl;
		return MStatus::kFailure;
	}
	unsigned	numElements = plug_meshArray.numElements(&ms);
#if 1
	assert(numElements % 3 == 0);
	numElements /= 3;
#endif
	if (!ms) {
		cerr << "ERR: tentacleShape::__setTentacleMesh: |.outputMesh|: MPlug::numElements(): Failure" << endl;
		return ms;
	}
	if (numElements != __tentacles.length()) {
		cerr << "ERR: tentacleShape::__setTentacleMesh: |.outMesh|: # of elements = " << numElements << " but # of tentacles = " << __tentacles.length() << endl;
		return MStatus::kFailure;
	}
	for (unsigned n = 0; n < numElements; ++n) {
		const kTentacle&	tentacle = __tentacles(n);
#if 1
		MPlug	plug_mesh = plug_meshArray.elementByPhysicalIndex(3 * n, &ms);
#else
		MPlug	plug_mesh = plug_meshArray.elementByPhysicalIndex(n, &ms);
#endif
#if 1
		MPointArray	strandSamplePoints;
		MPointArray	tentacleSamplePoints;
#if 1
		if (__skinSmoothBind.hasSkinCluster()) {
			tentacle.getSamplePoints(strandSamplePoints, numEllipseSamplePoints, tentacleSamplePoints);
		} else {
#endif
		tentacle.getSamplePoints(externalInclusiveMatrix(), strandSamplePoints, numEllipseSamplePoints, tentacleSamplePoints);
#if 1
		}
#endif
		ms = tentacleShape::tentacleSetPoints(plug_mesh, tentacleSamplePoints, MSpace::kWorld);
#else
		if (!ms) continue;
		MObject	inMeshObj;
		ms = plug_mesh.getValue(inMeshObj);
		if (!ms) continue;
		MFnMesh	mesh(inMeshObj, &ms);
		if (!ms) continue;
		MPointArray	strandSamplePoints;
		MPointArray	tentacleSamplePoints;
		tentacle.getSamplePoints(worldMatrix, strandSamplePoints, numEllipseSamplePoints, tentacleSamplePoints);
#if 0
		MPointArray	points;
		ms = mesh.getPoints(points, MSpace::kWorld);
		cout << ms << endl;
		cout << "points: " << points << endl;
		cout << "tentacleSamplePoints: " << tentacleSamplePoints << endl;
#endif
		ms = mesh.setPoints(tentacleSamplePoints, MSpace::kWorld);
#if 0
		ms = mesh.getPoints(points, MSpace::kWorld);
		cout << "points: " << points << endl;
#endif
#if 1
		MFnMeshData	meshDataFn;
		MObject	newMeshObj = meshDataFn.create();
		MFnMesh	newMeshFn;
		newMeshFn.copy(inMeshObj, newMeshObj);
		newMeshFn.setObject(newMeshObj);
		plug_mesh.setValue(newMeshObj);
#endif
#endif
	}
	return ms;
}
#if 1
MStatus	tentacleShape::__setTentaclePoints()
{
	MStatus	ms;
	ms = __setTentacleMesh();
#if 2
#if 1
	applySimulationParameter(true, true );
#endif
	if (__shSimulationMayaPtr) {
	cout << "ShSimulation: Status:= " << (__simulationStatus == Simulation::On ? "On" : (__simulationStatus == Simulation::Off ? "Off" : (__simulationStatus == Simulation::Paused ? "Paused" : (__simulationStatus == Simulation::ReadOnly ? "ReadOnly" : "Unknown")))) << endl;
if (__simulationStatus == Simulation::On || __simulationStatus == Simulation::ReadOnly) {
#pragma warning(disable: 4127)
do {
	MStatus	ms;
#ifdef _INHERIT_kBODY
	kBodyMayaTentacle*	bodyMayaTentaclePtr = (kBodyMayaTentacle*) __shSimulationMayaPtr;
#else
	kBodyMayaTentacle*	bodyMayaTentaclePtr = (kBodyMayaTentacle*) __shSimulationMayaPtr->bodyMayaPtr();
#endif
	if (!bodyMayaTentaclePtr || !bodyMayaTentaclePtr->isValid()) {
		cerr << "Simulation: Warning: No External Node" << endl;
		break;
	}
#if 1
	MTime	time = MAnimControl::currentTime();
	unsigned	nthFrame = (unsigned) time.value();
#if 11
	int	shiftFrame = getShiftFrame();
	if (shiftFrame > 0) {
		cout << "Simulation: Frame # = " << nthFrame << " shifted by " << shiftFrame << endl;
		nthFrame += shiftFrame;
	} else if (shiftFrame == 0) {
		cout << "Simulation: Frame # = " << nthFrame << endl;
	}
#else
	cout << "Simulation: Frame # = " << nthFrame << endl;
#endif
#if 0
if (__simulationStatus == Simulation::On) {
	kSimulationCache::kStrandArray&	strandArray = __shSimulationMayaPtr->strandArray();
		int	na = 2, nb = 3, nc = 5;
		int	nA = DEFAULT_SEGMENT_COUNT / 2;
		int	nB = DEFAULT_SEGMENT_COUNT - nA;
		int	numSegments = na + nA + nb + nB + nc;
		KVector	segLen(numSegments);
	for (int indexStrand = 0; indexStrand < (int) strandArray.length(); ++indexStrand) {
		kSimulationCache::kStrand&	strand = strandArray(indexStrand);
		ShSimStrand&	simulator = *strand.simulatorPtr;
		double	strandLength = strand.length;
		cout << "indexStrand = " << indexStrand << ", strandLength = " << strandLength << endl;
		double	ellA = strandLength / DEFAULT_SEGMENT_COUNT;
		double	ellB = (nthFrame >= 1 && nthFrame <= 10) ? strandLength / DEFAULT_SEGMENT_COUNT : 0;
		int	k = 0;
		for (int i = 0; i < na; ++i, ++k)
			segLen(k) = 0;
		for (int i = 0; i < nA; ++i, ++k)
			segLen(k) = ellA;
		for (int i = 0; i < nb; ++i, ++k)
			segLen(k) = 0;
		for (int i = 0; i < nB; ++i, ++k)
			segLen(k) = ellB;
		for (int i = 0; i < nc; ++i, ++k)
			segLen(k) = 0;
		cout << "numSegments = " << numSegments << ", k = " << k << endl;
		cout << "ellA = " << ellA << ", ellB = " << ellB << endl;
		simulator.setStrandLengthStiffnessUsingDiffernentSegmentLength(&segLen);
		cout << "indexStrand = " << indexStrand << ", strandLength = " << strandLength << endl;
	}
}
#endif
#if 0
	if (nthFrame > 1)
#endif
	(void) __shSimulationMayaPtr->setSimulationCache(nthFrame);
	cout << "kSimulationCache::setSimulationCache(nthFrame = " << nthFrame << "): done" << endl;
#if 1
	const kSimulationCache::kStrandArray&	strandArray = __shSimulationMayaPtr->strandArray();
#if 0
	const kPtrArray<kTentacle>&	tentacleArray0 = bodyMayaTentaclePtr->tentacleArray0();
#endif
#if 3
	const kPtrArray<kTentacle>&	tentacleArray0 = bodyMayaTentaclePtr->tentacleArray0();
#if 1
	MPlug	plug_tentacleArray(thisMObject(), __tentacleArray);
#endif
	MPlug	plug_meshArray(thisMObject(), outputMesh);
	bool	isArray = true;
	int	numElements_in_outputMesh = -1;
	isArray = plug_meshArray.isArray(&ms);
	if (!ms) {
		cerr << "ERR: |.outputMesh|: MPlug::isArray(): Failure (isArray = " << (isArray ? "True" : "False") << ")" << endl;
		isArray = false;
	} else if (isArray == false) {
		cerr << "WARN: |.outputMesh| not an array" << endl;
	} else {
		cout << "INFO: |.outputMesh| is an array" << endl;
		numElements_in_outputMesh = plug_meshArray.numElements(&ms);
#if 1
		assert(numElements_in_outputMesh % 3 == 0);
		numElements_in_outputMesh /= 3;
#endif
		if (!ms) {
			cerr << "ERR: |.outputMesh|: MPlug::numElements(): Failure (isArray = True)" << endl;
			numElements_in_outputMesh = 0;
		}
		cout << "INFO: |.outputMesh|: # of element(s) = " << numElements_in_outputMesh << endl;
	}
#endif
	ms = __strand_longitudinals.clear();
	ms = __strand_longitudinals.setLength(strandArray.length() * NUM_TENTACLE_SAMPLE_POINTS);
	for (int indexStrand = 0; indexStrand < (int) strandArray.length(); ++indexStrand) {
#if 0
	if (indexStrand == 0) {
		const kSimulationCache::kStrand&	strand = strandArray(indexStrand);
		int	strandId = strand.strandId;
	MPlug	plug_tentacle = plug_tentacleArray.elementByPhysicalIndex(strandId, &ms);
		MPlug	plug_root = plug_tentacle.child(TENTACLE_ARRAY_ROOT, &ms);
		MPlug	plug_root_position = plug_root.child(TENTACLE_ROOT_POSITION, &ms);
		MObject	valData;
		MFnNumericData		numericData;
		ms = plug_root_position.getValue(valData);
		ms = numericData.setObject(valData);
		MPoint	position;
		ms = numericData.getData(position[0], position[1], position[2]);
		cout << "root_position: " << position << endl;
		MPlug	plug_root_hit_triandle_id = plug_root.child(TENTACLE_ROOT_HIT_TRIANGLE_ID, &ms);
		MPlug	plug_root_poly_id = plug_root.child(TENTACLE_ROOT_POLY_ID, &ms);
		int	polyId = -1;
		ms = plug_root_poly_id.getValue(polyId);
		cout << "polyId = " << polyId << endl;
		int	hitTriangleId = -1;
		ms = plug_root_hit_triandle_id.getValue(hitTriangleId);
		cout << "hitTriangleId = " << hitTriangleId << endl;
		MPlug	plug_root_hit_bary1 = plug_root.child(TENTACLE_ROOT_HIT_BARY1, &ms);
		float	hitBary1;
		ms = plug_root_hit_bary1.getValue(hitBary1);
		cout << "hitBary1 = " << hitBary1 << endl;
		MPlug	plug_root_hit_bary2 = plug_root.child(TENTACLE_ROOT_HIT_BARY2, &ms);
		float	hitBary2;
		ms = plug_root_hit_bary2.getValue(hitBary2);
		cout << "hitBary2 = " << hitBary2 << endl;
	cout << "|" << __shSimulationMayaPtr->fullPathName() << "|" << endl;
	cout << "|" << __externalDagPathShape.fullPathName() << "|" << endl;
	MItMeshPolygon	polyIter(__externalDagPathShape, MObject::kNullObj, &ms);
	int	prevIndex;
	ms = polyIter.setIndex(polyId, prevIndex);
	cout << ms << "(prevIndex = " << prevIndex << ")" << endl;
	MPointArray	wPoints;
	MIntArray	vertexList;
	ms = polyIter.getTriangle(hitTriangleId, wPoints, vertexList, MSpace::kWorld);
	cout << "vertexList: " << vertexList << endl;
	cout << wPoints << endl;
#if 1
	int	indexTestVertex = vertexList[0];
	MPoint	V_prime_kWorld = wPoints[0];
	cout << "indexTestVertex = " << indexTestVertex << endl;
	cout << "V_prime_kWorld: " << V_prime_kWorld << endl;
#endif
	MPointArray	points;
	ms = polyIter.getTriangle(hitTriangleId, points, vertexList, MSpace::kObject);
	cout << points << endl;
	MPoint	wPosition = position * __externalInclusiveMatrix;
	MFloatVector	v1(wPoints[0]), v2(wPoints[1]), v3(wPoints[2]);
	cout << (hitBary1 * v1 + hitBary2 * v2 + (1 - hitBary1 - hitBary2) * v3) << endl;
	cout << wPosition << endl;
	MFloatVector	v10(points[0]), v20(points[1]), v30(points[2]);
	cout << position << endl;
	cout << (hitBary1 * v10 + hitBary2 * v20 + (1 - hitBary1 - hitBary2) * v30) << endl;
{
	FILE*	file = fopen("M:/weights", "w");
	MItDependencyNodes iter( MFn::kInvalid);
	for ( ; !iter.isDone(); iter.next() ) {
		MObject object = iter.item();
		if (object.apiType() == MFn::kSkinClusterFilter) {
			MFnSkinCluster	skinCluster(object);
{
#if 1
	MPlug	geometry_matrix_plug = skinCluster.findPlug("geomMatrix", false, &ms);
	if (!ms) {cerr << ms << endl; break;}
	MObject	valData;
	ms = geometry_matrix_plug.getValue(valData);
	if (!ms) {cerr << ms << endl; break;}
	MFnMatrixData		matrixData;
	ms = matrixData.setObject(valData);
	if (!ms) {cerr << ms << endl; break;}
	MMatrix	geometry_matrix = matrixData.matrix(&ms);
	cout << "geometry_matrix: " << geometry_matrix << endl;
#endif
#if 1
	MPlug	inverse_binding_matrix_plug_array = skinCluster.findPlug("bindPreMatrix", false, &ms);
#if 1
	MMatrix	inverse_binding_matrix[5];
#endif
	if (inverse_binding_matrix_plug_array.isArray()) {
		unsigned	numElements = inverse_binding_matrix_plug_array.numElements(&ms);
		cout << "# of joint(s) = " << numElements << endl;
		for (unsigned indexJoint = 0; indexJoint < numElements; ++indexJoint) {
			MPlug	inverse_binding_matrix_plug = inverse_binding_matrix_plug_array.elementByPhysicalIndex(indexJoint, &ms);
			if (!ms) {cerr << ms << endl; break;}
			MObject	valData;
			ms = inverse_binding_matrix_plug.getValue(valData);
			if (!ms) {cerr << ms << endl; break;}
			MFnMatrixData		matrixData;
			ms = matrixData.setObject(valData);
			if (!ms) {cerr << ms << endl; break;}
#if 1
			inverse_binding_matrix[indexJoint] = matrixData.matrix(&ms);
#else
			MMatrix	inverse_binding_matrix = matrixData.matrix(&ms);
			if (!ms) {cerr << ms << endl; break;}
			cout << inverse_binding_matrix << endl;
#endif
		}
	}
#if 1
	for (int i = 0; i < 5; ++i)
		cout << "B^-1[" << i << "]: " << inverse_binding_matrix[i] << endl;
#endif
#endif
#if 1
	cout << "weight list:" << endl;
#if 1
	MDoubleArray	weights_x;
#endif
	MPlug	weight_list_compound_plug_array = skinCluster.findPlug("weightList", false, &ms);
	if (weight_list_compound_plug_array.isArray(&ms) && ms) {
		unsigned	numElements = weight_list_compound_plug_array.numElements(&ms);
		cout << "# of vertices = " << numElements << endl;
		for (unsigned indexVertex = 0; indexVertex < numElements; ++indexVertex) {
#if 1
			if ((int) indexVertex != indexTestVertex)
				continue;
#endif
			MPlug	weights_compound_plug = weight_list_compound_plug_array.elementByPhysicalIndex(indexVertex, &ms);
			if (!ms) {cerr << ms << endl; break;}
			cout << weights_compound_plug.info(&ms) << endl;
			if (weights_compound_plug.isCompound(&ms) && ms) {
				unsigned	numChildren = weights_compound_plug.numChildren(&ms);
				if (!ms) {cerr << ms << endl; break;}
				cout << "# of children = " << numChildren << endl;
				if (numChildren != 1) {
					cerr << "ONLY one children(.weights) expected" << endl;
					break;
				}
				for (unsigned m = 0; m < numChildren; ++m) {
					MPlug	weight_plug_array = weights_compound_plug.child(m, &ms);
					if (!ms) {cerr << ms << endl; break;}
					cout << weight_plug_array.info(&ms) << endl;
					if (weight_plug_array.isArray(&ms) && ms) {
						unsigned	numElements = weight_plug_array.numElements(&ms);
						cout << "weights:";
						for (unsigned indexJoint = 0; indexJoint < numElements; ++indexJoint) {
							MPlug	weight_plug = weight_plug_array.elementByPhysicalIndex(indexJoint, &ms);
							double	weight;
							ms = weight_plug.getValue(weight);
							cout << " " <<  weight;
#if 1
							weights_x.append(weight);
#endif
						}
						cout << endl;
					}
				}
			}
		}
	}
#if 1
	cout << "indexTestVertex = " << indexTestVertex << endl;
	cout << "weights_x: " << weights_x << endl;
#endif
#endif
#if 1
	MPlug	matrix_plug_array = skinCluster.findPlug("matrix", false, &ms);
#if 1
	MMatrix	joint_world_matrix[5];
#endif
	if (matrix_plug_array.isArray()) {
		unsigned	numElements = matrix_plug_array.numElements(&ms);
		cout << "# of joint(s) = " << numElements << endl;
		for (unsigned indexJoint = 0; indexJoint < numElements; ++indexJoint) {
			MPlug	matrix_plug = matrix_plug_array.elementByPhysicalIndex(indexJoint, &ms);
#if 1
{
			if (!ms) {cerr << ms << endl; break;}
			MObject	valData;
			ms = matrix_plug.getValue(valData);
			if (!ms) {cerr << ms << endl; break;}
			MFnMatrixData		matrixData;
			ms = matrixData.setObject(valData);
			if (!ms) {cerr << ms << endl; break;}
			cout << matrixData.matrix(&ms) << endl;
}
#endif
#if 1
{
			MPlugArray	joint_plug_array;
			bool	bFound = matrix_plug.connectedTo(joint_plug_array, true, false, &ms);
			if (bFound && joint_plug_array.length()) {
				MObject	joint = joint_plug_array[0].node(&ms);
#if 0
MObject	joint_parent_plug = joint_plug_array[0].parent(&ms).node(&ms); cout << ms << endl;
MFnDagNode	joint_parent_dagNode(joint_parent_plug, &ms); cout << ms << endl;
MDagPath	joint_parent_dagPath;
ms = joint_parent_dagNode.getPath(joint_parent_dagPath); cout << ms << endl;
cout << "parent: |" << joint_parent_dagPath.fullPathName(&ms) << "|" << endl; cout << ms << endl;
cout << "parent matrix: |" << joint_parent_dagPath.inclusiveMatrix(&ms) << "|" << endl; cout << ms << endl;
#endif
				MFnDagNode	joint_dagNode(joint, &ms);
#if 0
cout << "# of parent = " << joint_dagNode.parentCount(&ms) << endl; if (!ms) {cerr << ms << endl; break;}
MFnDagNode	joint_parent_dagNode = joint_dagNode.parent(0, &ms); if (!ms) {cerr << ms << endl; break;}
MDagPath	joint_parent_dagPath;
ms = joint_parent_dagNode.getPath(joint_parent_dagPath); if (!ms) {cerr << ms << endl; break;}
cout << "parent: |" << joint_parent_dagPath.fullPathName(&ms) << "|" << endl; if (!ms) {cerr << ms << endl; break;}
cout << "parent matrix: |" << joint_parent_dagPath.inclusiveMatrix(&ms) << "|" << endl; if (!ms) {cerr << ms << endl; break;}
#endif
				MDagPath	joint_dagPath;
				ms = joint_dagNode.getPath(joint_dagPath);
				cout << "|" << joint_dagPath.fullPathName(&ms) << "|" << " (" << ms << ")" << endl;
				cout << "inclusive matrix(worldMatrix): " << joint_dagPath.inclusiveMatrix(&ms) << endl;
#if 1
				joint_world_matrix[indexJoint] = joint_dagPath.inclusiveMatrix(&ms);
#endif
				cout << "exclusive matrix(parentMatrix): " << joint_dagPath.exclusiveMatrix(&ms) << endl;
				cout << "(local) matrix such as worldMatrix = matrix * parentMatrix: " << joint_dagPath.inclusiveMatrix(&ms) * joint_dagPath.exclusiveMatrixInverse() << endl;
			}
}
#endif
		}
	}
#if 1
	for (int i = 0; i < 5; ++i) {
		cout << "W[" << i << "]: " << joint_world_matrix[i] << endl;
	}
#endif
#endif
	MMatrix	m;
	m = weights_x[0] * (inverse_binding_matrix[0] * joint_world_matrix[0]);
	for (int i = 1; i < 5; ++i) {
		m += weights_x[i] * (inverse_binding_matrix[i] * joint_world_matrix[i]);
	}
	cout << "V': " << V_prime_kWorld << endl;
	MPoint	V_prime_kObject = V_prime_kWorld * __externalInclusiveMatrix.inverse();
	cout << "V'0: " << V_prime_kObject << endl;
	MPoint	V_kObject = V_prime_kObject * m.inverse();
	cout << "V0: " << V_kObject << endl;
	cout << "MSpace::kObject in geometry: " << V_prime_kWorld * __externalInclusiveMatrix.inverse() * geometry_matrix * m.inverse() * geometry_matrix.inverse() << endl;
}
			MDagPathArray infs;
			MStatus stat;
			unsigned int nInfs = skinCluster.influenceObjects(infs, &stat);
			if (0 == nInfs) {
				stat = MS::kFailure;
			}
			unsigned int nGeoms = skinCluster.numOutputConnections();
			for (unsigned int ii = 0; ii < nGeoms; ++ii) {
				unsigned int index = skinCluster.indexForOutputConnection(ii,&stat);
				MDagPath skinPath;
				stat = skinCluster.getPathAtIndex(index,skinPath);
cout << "skinCluster: |" << skinPath.fullPathName() << "|" << endl;
				MItGeometry gIter(skinPath);
#if 1
if (file) {
				fprintf(file,
						"%s %d %u\n",skinPath.partialPathName().asChar(),
						gIter.count(),
						nInfs);
				for (unsigned int kk = 0; kk < nInfs; ++kk) {
					fprintf(file,"%s ",infs[kk].partialPathName().asChar());
				}
				fprintf(file,"\n");
}
#endif
				for (  ; !gIter.isDone(); gIter.next() ) {
					MObject comp = gIter.component(&stat);
					MDoubleArray wts;
					unsigned int infCount;
					stat = skinCluster.getWeights(skinPath,comp,wts,infCount);
					if (0 == infCount) {
						stat = MS::kFailure;
					}
#if 1
if (file) {
					fprintf(file,"%d ",gIter.index());
					for (unsigned int jj = 0; jj < infCount ; ++jj ) {
						fprintf(file,"%f ",wts[jj]);
					}
					fprintf(file,"\n");
}
#endif
				}
			}
		}
	}
	if (file)
	fclose(file);
}
	}
#endif
		ShVector3	samplePoints[NUM_TENTACLE_SAMPLE_POINTS];
#if 0
if (nthFrame == 1) {
		if (__shSimulationMayaPtr->getResult0(indexStrand, NUM_TENTACLE_SAMPLE_POINTS, samplePoints) < 0) {
			cerr << "kSimulationCache::getResult0(indexStrand = " << indexStrand << "): Failure" << endl;
			continue;
		}
} else {
#endif
		if (__shSimulationMayaPtr->getResult(indexStrand, nthFrame, NUM_TENTACLE_SAMPLE_POINTS, samplePoints) < 0) {
			cerr << "kSimulationCache::getResult(indexStrand = " << indexStrand << ", nthFrame = " << nthFrame << "): Failure" << endl;
			continue;
		}
#if 0
}
#endif
#if 3
if (isArray && numElements_in_outputMesh > indexStrand) {
		cout << "INFO: indexStrand = " << indexStrand << ": set |.outputMesh| done" << endl;
		const kSimulationCache::kStrand&	strand = strandArray(indexStrand);
		int	strandId = strand.strandId;
		MPointArray	strandSamplePoints_in_kWorld(NUM_TENTACLE_SAMPLE_POINTS);
		for (int k = 0; k < NUM_TENTACLE_SAMPLE_POINTS; ++k) {
			MPoint&	p = strandSamplePoints_in_kWorld[k];
			p.x = samplePoints[k][0];
			p.y = samplePoints[k][1];
			p.z = samplePoints[k][2];
		}
#if 1
		MPointArray	strandSamplePoints;
		MPointArray	tentacleSamplePoints;
		const kTentacle&	tentacle0 = tentacleArray0(strandId);
		const kTentacle&	tentacle = tentacle0;
#if 1
		if (__skinSmoothBind.hasSkinCluster()) {
			tentacle.getSamplePoints(strandSamplePoints, numEllipseSamplePoints, tentacleSamplePoints, &strandSamplePoints_in_kWorld);
		} else {
#endif
		tentacle.getSamplePoints(externalInclusiveMatrix(), strandSamplePoints, numEllipseSamplePoints, tentacleSamplePoints, &strandSamplePoints_in_kWorld);
#if 1
		}
#endif
		if (nthFrame == 1) {
		}
#if 1
		MPlug	plug_mesh = plug_meshArray.elementByPhysicalIndex(3 * strandId, &ms);
#else
		MPlug	plug_mesh = plug_meshArray.elementByPhysicalIndex(strandId, &ms);
#endif
		ms = tentacleShape::tentacleSetPoints(plug_mesh, tentacleSamplePoints, MSpace::kWorld);
#if 1
if (tentacle0.cut_frame > 0) {
		MPlug	plug_head_mesh = plug_meshArray.elementByPhysicalIndex(3 * strandId + 1, &ms);
		MPlug	plug_tail_mesh = plug_meshArray.elementByPhysicalIndex(3 * strandId + 2, &ms);
#if 1
		int	is_visibility = -1;
		int	is_head_visibility = -1;
		int	is_tail_visibility = -1;
		MPlug	plug_visibility;
		MPlug	plug_head_visibility;
		MPlug	plug_tail_visibility;
		ms = kNode::find_visibility_plug(plug_mesh, plug_visibility);
		if (ms) {
			bool	visibility;
			ms = plug_visibility.getValue(visibility);
			if (ms) {
				is_visibility = visibility ? 1 : 0;
			}
		}
		ms = kNode::find_visibility_plug(plug_head_mesh, plug_head_visibility);
		if (ms) {
			bool	visibility;
			ms = plug_head_visibility.getValue(visibility);
			if (ms) {
				is_head_visibility = visibility ? 1 : 0;
			}
		}
		ms = kNode::find_visibility_plug(plug_tail_mesh, plug_tail_visibility);
		if (ms) {
			bool	visibility;
			ms = plug_tail_visibility.getValue(visibility);
			if (ms) {
				is_tail_visibility = visibility ? 1 : 0;
			}
		}
		cout << "visibility: " << (is_visibility == -1 ? "NOT FOUND" : (is_visibility == 1 ? "On" : "Off")) << endl;
		cout << "head_visibility: " << (is_head_visibility == -1 ? "NOT FOUND" : (is_head_visibility == 1 ? "On" : "Off")) << endl;
		cout << "tail_visibility: " << (is_tail_visibility == -1 ? "NOT FOUND" : (is_tail_visibility == 1 ? "On" : "Off")) << endl;
#endif
if ((int) nthFrame >= tentacle0.cut_frame) {
	float	cut_t = tentacle0.t;
	MPlug	plug_tentacle = plug_tentacleArray.elementByPhysicalIndex(strandId, &ms);
#if 1
{
	bool	head = true;
#if 1
	kTentacle	cut_tentacle;
	ms = tentacleMeshSetCutTentacle(tentacle0, plug_tentacle, cut_tentacle, cut_t, head);
#endif
#if 1
	ShVector3	samplePoints[NUM_TENTACLE_SAMPLE_POINTS];
	if (__shSimulationMayaPtr->getResult(
		indexStrand, nthFrame,
		NUM_TENTACLE_SAMPLE_POINTS, samplePoints,
		cut_t, head
	) < 0) {
		cerr << "kSimulationCache::getResult(indexStrand = " << indexStrand << ", nthFrame = " << nthFrame << "): Failure" << endl;
		continue;
	}
	MPointArray	strandSamplePoints_in_kWorld(NUM_TENTACLE_SAMPLE_POINTS);
	for (int k = 0; k < NUM_TENTACLE_SAMPLE_POINTS; ++k) {
		MPoint&	p = strandSamplePoints_in_kWorld[k];
		p.x = samplePoints[k][0];
		p.y = samplePoints[k][1];
		p.z = samplePoints[k][2];
	}
	MPointArray	strandSamplePoints;
	MPointArray	tentacleSamplePoints;
#if 1
	if (__skinSmoothBind.hasSkinCluster()) {
		cut_tentacle.getSamplePoints(strandSamplePoints, numEllipseSamplePoints, tentacleSamplePoints, &strandSamplePoints_in_kWorld);
	} else {
#endif
	cut_tentacle.getSamplePoints(externalInclusiveMatrix(), strandSamplePoints, numEllipseSamplePoints, tentacleSamplePoints, &strandSamplePoints_in_kWorld);
#if 1
	}
#endif
	ms = tentacleShape::tentacleSetPoints(plug_head_mesh, tentacleSamplePoints, MSpace::kWorld);
#endif
#if 1
	MFloatArray	cutUArray, cutVArray;
	tentacleShape::tentacleMeshSetUVArray(cutUArray, cutVArray, cut_t, head);
	cout << tentacleShape::tentacle_setUVs(plug_head_mesh, cutUArray, cutVArray) << endl;
#endif
}
#endif
#if 1
if ((int) nthFrame == tentacle0.cut_frame) {
	bool	head = false;
#if 1
	kTentacle	cut_tentacle;
	ms = tentacleMeshSetCutTentacle(tentacle0, plug_tentacle, cut_tentacle, cut_t, head);
#endif
#if 1
	ShVector3	samplePoints[NUM_TENTACLE_SAMPLE_POINTS];
	if (__shSimulationMayaPtr->getResult(
		indexStrand, nthFrame,
		NUM_TENTACLE_SAMPLE_POINTS, samplePoints,
		cut_t, head
	) < 0) {
		cerr << "kSimulationCache::getResult(indexStrand = " << indexStrand << ", nthFrame = " << nthFrame << "): Failure" << endl;
		continue;
	}
	MPointArray	strandSamplePoints_in_kWorld(NUM_TENTACLE_SAMPLE_POINTS);
	for (int k = 0; k < NUM_TENTACLE_SAMPLE_POINTS; ++k) {
		MPoint&	p = strandSamplePoints_in_kWorld[k];
		p.x = samplePoints[k][0];
		p.y = samplePoints[k][1];
		p.z = samplePoints[k][2];
	}
	MPointArray	strandSamplePoints;
	MPointArray	tentacleSamplePoints;
#if 1
	if (__skinSmoothBind.hasSkinCluster()) {
		cut_tentacle.getSamplePoints(strandSamplePoints, numEllipseSamplePoints, tentacleSamplePoints, &strandSamplePoints_in_kWorld);
	} else {
#endif
	cut_tentacle.getSamplePoints(externalInclusiveMatrix(), strandSamplePoints, numEllipseSamplePoints, tentacleSamplePoints, &strandSamplePoints_in_kWorld);
#if 1
	}
#endif
	ms = tentacleShape::tentacleSetPoints(plug_tail_mesh, tentacleSamplePoints, MSpace::kWorld);
#endif
#if 1
	MFloatArray	cutUArray, cutVArray;
	tentacleShape::tentacleMeshSetUVArray(cutUArray, cutVArray, cut_t, head);
	cout << tentacleShape::tentacle_setUVs(plug_tail_mesh, cutUArray, cutVArray) << endl;
#endif
}
#endif
}
if ((int) nthFrame >= tentacle0.cut_frame) {
	if (is_visibility == 1)
		plug_visibility.setValue(false);
	if (is_head_visibility == 0)
		plug_head_visibility.setValue(true);
	if (is_tail_visibility == 0)
		plug_tail_visibility.setValue(true);
} else {
	if (is_head_visibility == 1)
		plug_head_visibility.setValue(false);
	if (is_tail_visibility == 1)
		plug_tail_visibility.setValue(false);
	if (is_visibility == 0)
		plug_visibility.setValue(true);
}
}
#endif
#else
{
		const kTentacle&	tentacle = tentacleArray0(strandId);
		MPlug	plug_mesh = plug_meshArray.elementByPhysicalIndex(strandId, &ms);
		if (!ms) continue;
		MObject	inMeshObj;
		ms = plug_mesh.getValue(inMeshObj);
		if (!ms) continue;
		MFnMesh	mesh(inMeshObj, &ms);
		if (!ms) continue;
		MPointArray	strandSamplePoints;
		MPointArray	tentacleSamplePoints;
		tentacle.getSamplePoints(externalInclusiveMatrix(), strandSamplePoints, numEllipseSamplePoints, tentacleSamplePoints, &strandSamplePoints_in_kWorld);
#if 0
		MPointArray	points;
		ms = mesh.getPoints(points, MSpace::kWorld);
		cout << ms << endl;
		cout << "points: " << points << endl;
		cout << "tentacleSamplePoints: " << tentacleSamplePoints << endl;
#endif
		ms = mesh.setPoints(tentacleSamplePoints, MSpace::kWorld);
#if 0
		ms = mesh.getPoints(points, MSpace::kWorld);
		cout << "points: " << points << endl;
#endif
#if 1
		MFnMeshData	meshDataFn;
		MObject	newMeshObj = meshDataFn.create();
		MFnMesh	newMeshFn;
		newMeshFn.copy(inMeshObj, newMeshObj);
		newMeshFn.setObject(newMeshObj);
		plug_mesh.setValue(newMeshObj);
#endif
}
#endif
} else {
	if (isArray == false) {
		cerr << "FATAL: 3: ...: indexStrand = " << indexStrand << ": |.outputMesh| not an array" << endl;
	} else if (numElements_in_outputMesh <= indexStrand) {
		cerr << "WARN: 3: ...: indexStrand = " << indexStrand << ": OUT OF RANGE(# of elements in |.outputMesh| = " << numElements_in_outputMesh << ")" << endl;
	} else {
		cerr << "3: ...: indexStrand = " << indexStrand << ": Unknown Error" << endl;
	}
}
#endif
#if 1
		for (int k = 0; k < NUM_TENTACLE_SAMPLE_POINTS; ++k) {
			__strand_longitudinals[indexStrand * NUM_TENTACLE_SAMPLE_POINTS + k] = MVector(samplePoints[k]);
		}
#else
#if 10
		if (__numExternalStrands > 0) {
			for (int k = 0; k < NUM_TENTACLE_SAMPLE_POINTS; ++k) {
				__strand_longitudinals[indexStrand * NUM_TENTACLE_SAMPLE_POINTS + k] = MVector(samplePoints[k]);
			}
			continue;
		}
#endif
		const kSimulationCache::kStrand&	strand = strandArray(indexStrand);
		int	strandId = strand.strandId;
		const kTentacle&	tentacle = tentacleArray0(strandId);
		if (tentacle.longitudinal.length() < 2) {
			cerr << "FATAL: kTentacle::longitudinal: no segment" << endl;
			continue;
		}
		const MVector&	longiVector0 = tentacle.longitudinal[0];
		double	len0 = longiVector0.length();
		const kRoot&	root = (kRoot&) tentacle;
		MPoint	basePoint = root.position;
		__strand_longitudinals[indexStrand * NUM_TENTACLE_SAMPLE_POINTS + 0] = basePoint;
#if 1
		basePoint *= __externalInclusiveMatrix;
		MMatrix	inv_m = __externalInclusiveMatrix.inverse();
#else
		basePoint *= __externalWorldMatrix;
		MMatrix	inv_m = __externalWorldMatrix.inverse();
#endif
		MPoint	point = basePoint;
		for (int k = 1; k < (int) NUM_TENTACLE_SAMPLE_POINTS; ++k) {
			MVector&	n = __strand_longitudinals[indexStrand * NUM_TENTACLE_SAMPLE_POINTS + k];
			MPoint	nextPoint(samplePoints[k][0], samplePoints[k][1], samplePoints[k][2]);
			MVector	offset(nextPoint - point);
#if 1
			offset = len0 * offset.transformAsNormal(inv_m);
#else
			offset = len * offset.transformAsNormal(inv_m);
#endif
			n = offset;
			point = nextPoint;
		}
#endif
	}
#endif
#endif
} while (0);
#pragma warning(default: 4127)
}
	}
#endif
	return ms;
}
#endif
#endif
const kPtrArray<RampTuple>*	tentacleShape::GetRampTupleArrayPtr() const
{
	MStatus	ms;
	MPlug	plug(thisMObject(), __rampWakes);
	short	val;
	ms = plug.getValue(val);
	if (val == RampWakes::MajorRadius)
		return &__majorRadiusScaleRampTupleArray;
	else if (val == RampWakes::Eccentricity)
		return &__eccentricityScaleRampTupleArray;
	else if (val == RampWakes::Rotation)
		return &__rotationLocalRampTupleArray;
	return 0;
}
void	tentacleShape::__setRampTupleArray(RampWakes::Type type)
{
	int	indexChild = -1;
	kPtrArray<RampTuple>*	rampTupleArrayPtr = 0;
	switch (type) {
	case RampWakes::MajorRadius:
		indexChild = TENTACLE_PARAM_CROSS_SECTION_MAJOR_RADIUS;
		rampTupleArrayPtr = &__majorRadiusScaleRampTupleArray;
		break;
	case RampWakes::Eccentricity:
		indexChild = TENTACLE_PARAM_CROSS_SECTION_ECCENTRICITY;
		rampTupleArrayPtr = &__eccentricityScaleRampTupleArray;
		break;
	case RampWakes::Rotation:
		indexChild = TENTACLE_PARAM_CROSS_SECTION_ROTATION;
		rampTupleArrayPtr = &__rotationLocalRampTupleArray;
		break;
	default:
		indexChild = -1;
		rampTupleArrayPtr = 0;
		break;
	}
	if (!rampTupleArrayPtr)
		return;
	kPtrArray<RampTuple>&	rampTupleArray = *rampTupleArrayPtr;
	rampTupleArray.clear();
	MStatus	ms;
	MPlug	plug(thisMObject(), __tentacleParameter);
	MPlug	plug_what = plug.child(indexChild, &ms);
	MPlug	plug_ramp = plug_what.child(TENTACLE_ATTRIBUTE_EDITOR_RAMP);
	int	numElements = plug_ramp.numElements();
	IndexedRampTuple*	indexedRampTuplePtr0 = 0;
#pragma	warning(disable: 4127)
	do {
		indexedRampTuplePtr0 = new IndexedRampTuple[1 + numElements + 1];
		IndexedRampTuple*	indexedRampTuplePtr = indexedRampTuplePtr0 + 1;
		for (int i = 0; i < numElements; ++i) {
			IndexedRampTuple&	indexedRampTuple = indexedRampTuplePtr[i];
			MPlug	plug_element = plug_ramp.elementByPhysicalIndex(i, &ms);
			indexedRampTuple.index = i;
			ms = plug_element.child(RAMP_POSITION).getValue(indexedRampTuple.Position);
			ms = plug_element.child(RAMP_FLOAT_VALUE).getValue(indexedRampTuple.FloatValue);
			ms = plug_element.child(RAMP_INTERP_TYPE).getValue(*((int*) &indexedRampTuple.InterpType));
		}
#if 0
		for (int i = 0; i < numElements; ++i) {
			const IndexedRampTuple&	indexedRampTuple = indexedRampTuplePtr[i];
			cout << "Unsorted_RampTuple[" << indexedRampTuple.index << "]:" << endl;
			cout << "\tPosition: " << indexedRampTuple.Position << endl;
			cout << "\tFloatValue: " << indexedRampTuple.FloatValue << endl;
			cout << "\tInterp: " << indexedRampTuple.InterpType << endl;
		}
#endif
		qsort(indexedRampTuplePtr, numElements, sizeof(IndexedRampTuple), compare);
#if 0
		for (int i = 0; i < numElements; ++i) {
			const IndexedRampTuple&	indexedRampTuple = indexedRampTuplePtr[i];
			cout << "Sorted_RampTuple[" << indexedRampTuple.index << "]:" << endl;
			cout << "\tPosition: " << indexedRampTuple.Position << endl;
			cout << "\tFloatValue: " << indexedRampTuple.FloatValue << endl;
			cout << "\tInterp: " << indexedRampTuple.InterpType << endl;
		}
#endif
		if (1||indexedRampTuplePtr[0].Position > 0) {
			indexedRampTuplePtr0[0].index = 0;
			indexedRampTuplePtr0[0].Position = 0;
			indexedRampTuplePtr0[0].FloatValue = indexedRampTuplePtr0[1].FloatValue;
			indexedRampTuplePtr0[0].InterpType = INTERP::None;
			for (int i = 0; i < numElements; ++i)
				++indexedRampTuplePtr[i].index;
			indexedRampTuplePtr = indexedRampTuplePtr0;
			++numElements;
		}
		IndexedRampTuple&	lastIndexedRampleTuple = indexedRampTuplePtr[numElements - 1];
		lastIndexedRampleTuple.InterpType = INTERP::None;
		if (1||lastIndexedRampleTuple.Position < 1) {
			indexedRampTuplePtr[numElements].index = numElements;
			indexedRampTuplePtr[numElements].Position = 1;
			indexedRampTuplePtr[numElements].FloatValue = lastIndexedRampleTuple.FloatValue;
			indexedRampTuplePtr[numElements].InterpType = INTERP::None;
			++numElements;
		}
#if 0
		for (int i = 0; i < numElements; ++i) {
			const IndexedRampTuple&	indexedRampTuple = indexedRampTuplePtr[i];
			cout << "Rationalized_Sorted_RampTuple[" << indexedRampTuple.index << "]:" << endl;
			cout << "\tPosition: " << indexedRampTuple.Position << endl;
			cout << "\tFloatValue: " << indexedRampTuple.FloatValue << endl;
			cout << "\tInterp: " << indexedRampTuple.InterpType << endl;
		}
#endif
		for (int i = 0; i < numElements; ++i) {
			RampTuple*	p = new RampTuple;
			*p = *((RampTuple*) &indexedRampTuplePtr[i]);
			rampTupleArray.append(p);
		}
		assert(numElements > 0);
	} while (0);
#pragma	warning(default: 4127)
	if (indexedRampTuplePtr0) {
		delete[] indexedRampTuplePtr0;
		indexedRampTuplePtr0 = 0;
	}
}
#if 0
void	tentacleShape::__setRampTupleArray(RampWakes::Type type, MDataBlock datablock)
{
	int	indexChild = -1;
	kPtrArray<RampTuple>*	rampTupleArrayPtr = 0;
	switch (type) {
	case RampWakes::MajorRadius:
		indexChild = TENTACLE_PARAM_CROSS_SECTION_MAJOR_RADIUS;
		rampTupleArrayPtr = &__majorRadiusScaleRampTupleArray;
		break;
	case RampWakes::Eccentricity:
		indexChild = TENTACLE_PARAM_CROSS_SECTION_ECCENTRICITY;
		rampTupleArrayPtr = &__eccentricityScaleRampTupleArray;
		break;
	case RampWakes::Rotation:
		indexChild = TENTACLE_PARAM_CROSS_SECTION_ROTATION;
		rampTupleArrayPtr = &__rotationLocalRampTupleArray;
		break;
	default:
		indexChild = -1;
		rampTupleArrayPtr = 0;
		break;
	}
	if (!rampTupleArrayPtr)
		return;
	kPtrArray<RampTuple>&	rampTupleArray = *rampTupleArrayPtr;
	rampTupleArray.clear();
	MStatus	ms;
	MPlug	plug(thisMObject(), __tentacleParameter);
	MPlug	plug_what = plug.child(indexChild, &ms);
	MPlug	plug_ramp = plug_what.child(TENTACLE_ATTRIBUTE_EDITOR_RAMP);
	int	numElements = plug_ramp.numElements();
	cout << "numElements = " << numElements << " (" << ms << ")" << endl;
	MArrayDataHandle	handleArray = datablock.inputArrayValue(plug_ramp, &ms);
	cout << ms << endl;
	int	numElements0 = handleArray.elementCount(&ms);
	cout << "numElements0 = " << numElements0 << " (" << ms << ")" << endl;
	IndexedRampTuple*	indexedRampTuplePtr0 = 0;
	do {
		indexedRampTuplePtr0 = new IndexedRampTuple[1 + numElements + 1];
		IndexedRampTuple*	indexedRampTuplePtr = indexedRampTuplePtr0 + 1;
		IndexedRampTuple*	p = indexedRampTuplePtr;
		int	n = 0;
		for (int i = 0; i < numElements; ++i) {
			IndexedRampTuple&	indexedRampTuple = *p;
			float&	Position = indexedRampTuple.Position;
			float&	FloatValue = indexedRampTuple.FloatValue;
			INTERP::TYPE&	InterpType = indexedRampTuple.InterpType;
#if 1
			ms = handleArray.jumpToArrayElement(i);
			cout << ms << endl;
			MPlug	plug_element = plug_ramp.elementByLogicalIndex(i, &ms);
			indexedRampTuple.index = i;
			MDataHandle	handle = handleArray.inputValue(&ms); cout << ms << endl;
			MPlug	plug_Position = plug_element.child(RAMP_POSITION, &ms); cout << ms << endl;
			MPlug	plug_FloatValue = plug_element.child(RAMP_FLOAT_VALUE, &ms); cout << ms << endl;
			MPlug	plug_InterpType = plug_element.child(RAMP_INTERP_TYPE, &ms); cout << ms << endl;
			MDataHandle	handle_Position = handle.child(plug_Position);
			Position = handle_Position.asFloat();
			MDataHandle	handle_FloatValue = handle.child(plug_FloatValue);
			FloatValue = handle_FloatValue.asFloat();
			MDataHandle	handle_InterpType = handle.child(plug_InterpType);
			InterpType = (INTERP::TYPE) handle_InterpType.asShort();
			cout << "[" << i << "]: Position: " << Position << endl;
			cout << "[" << i << "]: FloatValue: " << FloatValue << endl;
			cout << "[" << i << "]: InterpType: " << InterpType << endl;
			if (Position < 0.f || Position > 1.f)
				continue;
			if (FloatValue < 0.f || FloatValue > 1.f)
				continue;
			if (InterpType < INTERP::None || InterpType > INTERP::Spline)
				continue;
			++n;
			++p;
#else
			MPlug	plug_element = plug_ramp.elementByPhysicalIndex(i, &ms);
			indexedRampTuple.index = i;
			ms = plug_element.child(RAMP_POSITION).getValue(indexedRampTuple.Position);
			cout << "[" << i << "]: Position: " << indexedRampTuple.Position << " (" << ms << ")" << endl;
			ms = plug_element.child(RAMP_FLOAT_VALUE).getValue(indexedRampTuple.FloatValue);
			cout << "[" << i << "]: FloatValue: " << indexedRampTuple.FloatValue << " (" << ms << ")" << endl;
			ms = plug_element.child(RAMP_INTERP_TYPE).getValue(*((int*) &indexedRampTuple.InterpType));
			cout << "[" << i << "]: InterpType: " << indexedRampTuple.InterpType << " (" << ms << ")" << endl;
#endif
		}
#if 1
		for (int i = 0; i < n ; ++i) {
			const IndexedRampTuple&	indexedRampTuple = indexedRampTuplePtr[i];
			cout << "Unsorted_RampTuple[" << indexedRampTuple.index << "]:" << endl;
			cout << "\tPosition: " << indexedRampTuple.Position << endl;
			cout << "\tFloatValue: " << indexedRampTuple.FloatValue << endl;
			cout << "\tInterp: " << indexedRampTuple.InterpType << endl;
		}
#endif
		qsort(indexedRampTuplePtr, n , sizeof(IndexedRampTuple), compare);
#if 1
		for (int i = 0; i < n ; ++i) {
			const IndexedRampTuple&	indexedRampTuple = indexedRampTuplePtr[i];
			cout << "Sorted_RampTuple[" << indexedRampTuple.index << "]:" << endl;
			cout << "\tPosition: " << indexedRampTuple.Position << endl;
			cout << "\tFloatValue: " << indexedRampTuple.FloatValue << endl;
			cout << "\tInterp: " << indexedRampTuple.InterpType << endl;
		}
#endif
		if (1||indexedRampTuplePtr[0].Position > 0) {
			indexedRampTuplePtr0[0].index = 0;
			indexedRampTuplePtr0[0].Position = 0;
			indexedRampTuplePtr0[0].FloatValue = indexedRampTuplePtr0[1].FloatValue;
			indexedRampTuplePtr0[0].InterpType = INTERP::None;
			for (int i = 0; i < n ; ++i)
				++indexedRampTuplePtr[i].index;
			indexedRampTuplePtr = indexedRampTuplePtr0;
			++n;
		}
		IndexedRampTuple&	lastIndexedRampleTuple = indexedRampTuplePtr[n  - 1];
		lastIndexedRampleTuple.InterpType = INTERP::None;
		if (1||lastIndexedRampleTuple.Position < 1) {
			indexedRampTuplePtr[n].index = n;
			indexedRampTuplePtr[n].Position = 1;
			indexedRampTuplePtr[n].FloatValue = lastIndexedRampleTuple.FloatValue;
			indexedRampTuplePtr[n].InterpType = INTERP::None;
			++n;
		}
#if 1
		for (int i = 0; i < n ; ++i) {
			const IndexedRampTuple&	indexedRampTuple = indexedRampTuplePtr[i];
			cout << "Rationalized_Sorted_RampTuple[" << indexedRampTuple.index << "]:" << endl;
			cout << "\tPosition: " << indexedRampTuple.Position << endl;
			cout << "\tFloatValue: " << indexedRampTuple.FloatValue << endl;
			cout << "\tInterp: " << indexedRampTuple.InterpType << endl;
		}
#endif
		cout << "n = " << n << endl;
		for (int i = 0; i < n ; ++i) {
			RampTuple*	p = new RampTuple;
			*p = *((RampTuple*) &indexedRampTuplePtr[i]);
			rampTupleArray.append(p);
		}
		assert(n  > 0);
	} while (0);
	if (indexedRampTuplePtr0) {
		delete[] indexedRampTuplePtr0;
		indexedRampTuplePtr0 = 0;
	}
}
#endif
int	tentacleShape::GetInterpolatedValue(
	const kPtrArray<RampTuple>&	rampTupleArray,
	float	x,
	float&	y,
	bool	clamp,
	bool	verbose
)
{
	int	found = 0;
	for (int k = 0; k < (int) rampTupleArray.length() - 1; ++k) {
		const RampTuple&	left = rampTupleArray(k);
		const RampTuple&	right = rampTupleArray(k + 1);
		const float	x1 = left.Position;
		const float	x2 = right.Position;
		if (x1 <= x && x <= x2) {
			const float	y1 = left.FloatValue;
			const float	y2 = right.FloatValue;
			switch (left.InterpType) {
			case INTERP::None:
				y = y1;
				break;
			case INTERP::Linear:
				if (x1 == x2)
					y = y1;
				else
					y = y1 + (x - x1) * (y2 - y1) / (x2 - x1);
				break;
			case INTERP::Smooth:
				if (x1 == x2)
					y = y1;
				else {
					float	s = (x - x1) / (x2 - x1);
					float	dy = y2 - y1;
					y = y1 + s * s * (3 * dy - 2 * dy * s);
				}
				break;
			case INTERP::Spline:
				if (x1 == x2)
					y = y1;
				else {
					assert(k > 0);
					const float	p1 = rampTupleArray(k - 1).FloatValue;
					assert(k + 2 < (int) rampTupleArray.length());
					const float	p4 = rampTupleArray(k + 2).FloatValue;
					const float	p2 = y1;
					const float	p3 = y2;
					float	s = (x - x1) / (x2 - x1);
					float	s2 = s * s;
					float	s3 = s2 * s;
y = ((-s3 + 2 * s2 - s) * p1 + (3 * s3 - 5 * s2 + 2) * p2 + (-3 * s3 + 4 * s2 + s) * p3 + (s3 - s2) * p4) / 2;
				}
				break;
			default:
				cerr << "Invalid INTERP::Type: " << left.InterpType << endl;
				found = -1;
				break;
			}
			found = 1;
			break;
		}
	}
	int	rval = 0;
	if (found == -1) {
		y = 0.f;
		rval = -1;
	} else if (found == 0) {
		y = 0.f;
		rval = -2;
	} else {
		rval = 0;
		if (clamp) {
			if (y < 0.f) {
				if (verbose)
					cout << "x = " << x << ": y = " << y << " clamped to 0" << endl;
				y = 0.f;
			} else if (y > 1.f) {
				if (verbose)
					cout << "x = " << x << ": y = " << y << " clamped to 1" << endl;
				y = 1.f;
			}
		}
	}
	return rval;
}
#endif
#endif
#endif
#if 1
#if 1
#if 1
static void	__checkStatus(const MStatus& ms, const char* from)
{
	if (ms)
		return;
	if (from)
		cerr << from << ": ";
	cerr << "|" << ms << "|" << endl;
}
static int	compare(const void* arg1, const void* arg2)
{
	const IndexedRampTuple&	indexedRampTuple1 = *((IndexedRampTuple*) arg1);
	const IndexedRampTuple&	indexedRampTuple2 = *((IndexedRampTuple*) arg2);
	if (indexedRampTuple1.Position < indexedRampTuple2.Position)
		return -1;
	else if (indexedRampTuple1.Position > indexedRampTuple2.Position)
		return +1;
	else
		return indexedRampTuple1.index - indexedRampTuple2.index;
}
MStatus
SetTentacleParameterValue(
	const MString*	strIndent0Ptr,
	const MPlug&	plug,
	MPlugArray&	hierarchy,
	bool*	isTentacleParameterPtr
)
{
	MStatus	ms;
	if (isTentacleParameterPtr)
		*isTentacleParameterPtr = false;
	if (strIndent0Ptr && *strIndent0Ptr == "")
		cout << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << endl;
	if (!plug) {
		if (strIndent0Ptr)
			cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << endl;
		return MStatus::kSuccess;
	}
	ms = hierarchy.insert(plug, 0);
	MString	strIndent;
	if (strIndent0Ptr) {
		strIndent = *strIndent0Ptr;
		strIndent += "\t";
	}
	if (strIndent0Ptr)
		cout << strIndent << "info: |" << plug.info(&ms) << "| (" << ms << ")" << endl;
	MString	partialName = plug.partialName(false, false, false, false, false, true, &ms);
	if (strIndent0Ptr)
		cout << strIndent << "partialName: |" << partialName << "| (" << ms << ")" << endl;
	if (partialName == "tentacleParameter") {
		assert(isTentacleParameterPtr);
		if (isTentacleParameterPtr)
			*isTentacleParameterPtr = true;
	}
	bool	isChild = plug.isChild(&ms);
	if (strIndent0Ptr)
		cout << strIndent << "\tisChild? " << (isChild ? "True" : "False") << " (" << ms << ")" << endl;
	if (isChild) {
		MPlug	parent = plug.parent(&ms);
		if (strIndent0Ptr)
			cout << strIndent << "\tparent: |" << parent.info(&ms) << "| (" << ms << ")" << endl;
		SetTentacleParameterValue(
			strIndent0Ptr ? &strIndent : 0,
			parent,
			hierarchy,
			isTentacleParameterPtr ? (*isTentacleParameterPtr ? 0 : isTentacleParameterPtr) : 0
		);
	}
	bool	isElement = plug.isElement(&ms);
	if (strIndent0Ptr)
		cout << strIndent << "\tisElement? " << (isElement ? "True" : "False") << " (" << ms << ")" << endl;
	if (isElement) {
		MPlug	array = plug.array(&ms);
		if (strIndent0Ptr)
			cout << strIndent << "\tarray: |" << array.info(&ms) << "| (" << ms << ")" << endl;
		SetTentacleParameterValue(
			strIndent0Ptr ? &strIndent : 0,
			array,
			hierarchy,
			isTentacleParameterPtr ? (*isTentacleParameterPtr ? 0 : isTentacleParameterPtr) : 0
		);
	}
	if (strIndent0Ptr && *strIndent0Ptr == "")
		cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << endl;
	return MStatus::kSuccess;
}
#endif
#endif
#endif
#if 1
#if 1
#define UV
MStatus	tentacleShape::tentacleMeshSetCutTentacle(
	const kTentacle&	tentacle0,
	const MPlug&		plug_tentacle,
	kTentacle&	cut_tentacle,
	float	cut_t,
	bool	head
)
{
	if (cut_t < 0 || cut_t > 1) {
		return MStatus::kInvalidParameter;
	}
	MStatus	ms;
	cut_tentacle.position = tentacle0.position;
	const MVector&	normal = tentacle0.normal;
	MVector	transversal;
	transversal = MVector(1, 0, 0).rotateBy(MQuaternion(MVector(0, 0, 1), normal));
	for (int k = 0; k < (NUM_TENTACLE_SAMPLE_POINTS - 1); ++k)
		cut_tentacle.longitudinal[k] = tentacle0.longitudinal[0] * (head ? cut_t : (1 - cut_t));
	for (int k = 0; k < NUM_TENTACLE_SAMPLE_POINTS; ++k)
		cut_tentacle.transversal[k] = transversal;
	double	unit_t = (head ? cut_t : (1 - cut_t)) / (NUM_TENTACLE_SAMPLE_POINTS - 1);
	MPlug	plug_a = plug_tentacle.child(TENTACLE_ARRAY_MAJOR_RADIUS, &ms);
	double	a;
	ms = plug_a.getValue(a);
	for (int k = 0; k < NUM_TENTACLE_SAMPLE_POINTS; ++k) {
		float	x = (head ? 0 : cut_t) + k * (float) unit_t;
		float	y;
		(void) tentacleShape::GetInterpolatedValue(__majorRadiusScaleRampTupleArray, x, y, true);
		cut_tentacle.majorAxis[k] = a * y;
	}
	MPlug	plug_e = plug_tentacle.child(TENTACLE_ARRAY_ECCENTRICITY, &ms);
	double	e;
	ms = plug_e.getValue(e);
	for (int k = 0; k < NUM_TENTACLE_SAMPLE_POINTS; ++k) {
		float	x = (head ? 0 : cut_t) + k * (float) unit_t;
		float	y;
		(void) tentacleShape::GetInterpolatedValue(__eccentricityScaleRampTupleArray, x, y, true);
		cut_tentacle.eccentricity[k] = e * y;
	}
	MPlug	plug_rotation = plug_tentacle.child(TENTACLE_ARRAY_ROTATION, &ms);
	double	global_rotation;
	ms = plug_rotation.getValue(global_rotation);
	for (int k = 0; k < NUM_TENTACLE_SAMPLE_POINTS; ++k) {
		float	x = (head ? 0 : cut_t) + k * (float) unit_t;
		float	y;
		(void) tentacleShape::GetInterpolatedValue(__rotationLocalRampTupleArray, x, y, true);
		double	local_rotation = ((double) y - 0.5) * 2 * M_PI;
		cut_tentacle.rotation[k] = global_rotation + local_rotation;
	}
	return ms;
}
void	tentacleShape::tentacleMeshSetUVArray(
	MFloatArray&	uArray,
	MFloatArray&	vArray,
	float	cut_t,
	bool	head
)
{
	uArray.clear();
	vArray.clear();
	const float	d = 0.01f;
	const int	m = NUM_TENTACLE_SAMPLE_POINTS - 1;
	const int	n = numEllipseSamplePoints;
	for (int i = 0; i <= m; ++i) {
		float	u = d + (1 - 2 * d) / m * i;
		if (cut_t >= 0 && cut_t <= 1)
			u = d + (1 - 2 * d) * (head ? 0 : cut_t) + (1 - 2 * d) * ((float) (head ? cut_t : (1 - cut_t))) / m * i;
		for (int j = 0; j <= n; ++j) {
			uArray.append(u);
			vArray.append(d + (float) (0.9 - 2 * d) / n * (n - j));
		}
	}
	float	bottom_u = (cut_t < 0 || cut_t > 1) ? 0.05f : (head ? 0.05f : 0.85f);
	for (int j = 0; j < n; ++j) {
		uArray.append(bottom_u + (0.05f - d) * cos(2 * (float) M_PI / n * j));
		vArray.append(0.95f + (0.05f - d) * sin(2 * (float) M_PI / n * j));
	}
	float	top_u = (cut_t < 0 || cut_t > 1) ? 0.95f : (head ? 0.15f : 0.95f);
	for (int j = 0; j < n; ++j) {
		uArray.append(top_u + (0.05f - d) * cos(2 * (float) M_PI / n * j));
		vArray.append(0.95f + (0.05f - d) * sin(2 * (float) M_PI / n * j));
	}
}
MObject	tentacleShape::tentacleCreateMesh(MObject& outData, MStatus& stat)
{
	MFnMesh	mesh;
#ifdef UV
#endif
	const int	m = NUM_TENTACLE_SAMPLE_POINTS - 1;
	const int	n = numEllipseSamplePoints;
	const float	a = 1.0;
	const float	h = 10.0;
	MFloatPointArray	vertexArray;
	for (int i = 0; i <= m; ++i) {
		float	y = h / m * i;
		for (int j = 0; j < n; ++j) {
			vertexArray.append(MFloatPoint(
#if 1
				-a * sin(2 * (float) M_PI / n * j),
				y,
				a * cos(2 * (float) M_PI / n * j)
#else
				a * sin(2 * (float) M_PI / n * j),
				y,
				a * cos(2 * (float) M_PI / n * j)
#endif
			));
		}
	}
	const int	numVertices = (m + 1) * n;
	const int	numFaces = m * n + 1 + 1;
	MIntArray	polygonCounts;
	for (int k = 0; k < m * n; ++k)
		polygonCounts.append(4);
	polygonCounts.append(n);
	polygonCounts.append(n);
	MIntArray	polygonConnects;
	for (int k = 0; k < m * n; ++k) {
		int	i = k / n;
		int	j = k % n;
		int	j_plus_1 = j == (n - 1) ? 0 : (j + 1);
#if 0
		polygonConnects.append(n * i + j);
		polygonConnects.append(n * (i + 1) + j);
		polygonConnects.append(n * (i + 1) + j_plus_1);
		polygonConnects.append(n * i + j_plus_1);
#else
		polygonConnects.append(n * i + j);
		polygonConnects.append(n * i + j_plus_1);
		polygonConnects.append(n * (i + 1) + j_plus_1);
		polygonConnects.append(n * (i + 1) + j);
#endif
	}
#if 0
	for (int j = 0; j < n; ++j)
		polygonConnects.append(n * 0 + j);
	for (int j = 0; j < n; ++j)
		polygonConnects.append(n * m + ((n - 1) - j));
#else
	for (int j = 0; j < n; ++j)
		polygonConnects.append(n * 0 + ((n - 1) - j));
	for (int j = 0; j < n; ++j)
		polygonConnects.append(n * m + j);
#endif
	MObject	newMesh = mesh.create(
		numVertices, numFaces,
		vertexArray, polygonCounts, polygonConnects,
#ifdef UV
#if 0
		uArray, vArray,
#endif
#endif
		outData, &stat
	);
#ifdef UV
	MFloatArray	uArray, vArray;
#if 1
	tentacleShape::tentacleMeshSetUVArray(uArray, vArray);
#else
	for (int i = 0; i <= m; ++i) {
		float	u = d + (1 - 2 * d) / m * i;
		for (int j = 0; j <= n; ++j) {
			uArray.append(u);
			vArray.append(d + (float) (0.9 - 2 * d) / n * (n - j));
		}
	}
	for (int j = 0; j < n; ++j) {
		uArray.append(0.05f + (0.05f - d) * cos(2 * (float) M_PI / n * j));
		vArray.append(0.95f + (0.05f - d) * sin(2 * (float) M_PI / n * j));
	}
	for (int j = 0; j < n; ++j) {
		uArray.append(0.95f + (0.05f - d) * cos(2 * (float) M_PI / n * j));
		vArray.append(0.95f + (0.05f - d) * sin(2 * (float) M_PI / n * j));
	}
#endif
	MIntArray	uvCounts;
	for (int k = 0; k < m * n; ++k)
		uvCounts.append(4);
	uvCounts.append(n);
	uvCounts.append(n);
	MIntArray	uvIds;
	for (int k = 0; k < m * n; ++k) {
		int	i = k / n;
		int	j = k % n;
#if 0
		uvIds.append((n + 1) * i + j);
		uvIds.append((n + 1) * (i + 1) + j);
		uvIds.append((n + 1) * (i + 1) + (j + 1));
		uvIds.append((n + 1) * i + (j + 1));
#else
		uvIds.append((n + 1) * i + j);
		uvIds.append((n + 1) * i + (j + 1));
		uvIds.append((n + 1) * (i + 1) + (j + 1));
		uvIds.append((n + 1) * (i + 1) + j);
#endif
	}
	for (int j = 0; j < n; ++j)
		uvIds.append((m + 1) * (n + 1) + j);
	for (int j = 0; j < n; ++j)
		uvIds.append((m + 1) * (n + 1) + n + j);
#endif
#ifdef UV
	MStatus	ms;
	ms = mesh.setUVs(uArray, vArray);
	ms = mesh.assignUVs(uvCounts, uvIds);
#endif
	return newMesh;
}
MStatus	tentacleShape::tentacleSetPoints(MPlug& plug_mesh,  MPointArray& tentacleSamplePoints, MSpace::Space space)
{
	MStatus	ms;
	MObject	inMeshObj;
	ms = plug_mesh.getValue(inMeshObj);
	if (!ms) return ms;
	MFnMesh	mesh(inMeshObj, &ms);
	if (!ms) return ms;
	ms = mesh.setPoints(tentacleSamplePoints, space);
#if 1
	MFnMeshData	meshDataFn;
	MObject	newMeshObj = meshDataFn.create();
	MFnMesh	newMeshFn;
	newMeshFn.copy(inMeshObj, newMeshObj);
	newMeshFn.setObject(newMeshObj);
	plug_mesh.setValue(newMeshObj);
#endif
	return ms;
}
#if 1
MStatus	tentacleShape::tentacle_setUVs(
	MPlug&	plug_mesh,
	const MFloatArray& uArray, const MFloatArray& vArray
)
{
	MStatus	ms;
	MObject	inMeshObj;
	ms = plug_mesh.getValue(inMeshObj);
	if (!ms) return ms;
	MFnMesh	mesh(inMeshObj, &ms);
	if (!ms) return ms;
	ms = mesh.setUVs(uArray, vArray);
#if 1
	MFnMeshData	meshDataFn;
	MObject	newMeshObj = meshDataFn.create();
	MFnMesh	newMeshFn;
	newMeshFn.copy(inMeshObj, newMeshObj);
	newMeshFn.setObject(newMeshObj);
	plug_mesh.setValue(newMeshObj);
#endif
	return ms;
}
#endif
#else
#define UV
#ifdef UV
#ifdef VERTEX_UV_MISMATCH
#else
#endif
#else
#endif
MObject	tentacleShape::tentacleCreateMesh(const MTime& time,
							  MObject& outData,
							  MStatus& stat)
{
	int				numVertices, frame;
	float			cubeSize;
	MFloatPointArray		points;
	MFnMesh			meshFS;
	frame = (int)time.as( MTime::kFilm );
	if (frame == 0)
	  frame = 1;
	cubeSize		    		= 0.5f * (float)( frame % 10);
	const int numFaces			= 6;
	numVertices					= 8;
#if !(defined(UV) && !defined(VERTEX_UV_MISMATCH))
	const int numFaceConnects	= 24;
#endif
#if defined(UV) && !defined(VERTEX_UV_MISMATCH)
	MFloatPoint	vtx_1(-cubeSize, -cubeSize,  cubeSize);
	MFloatPoint	vtx_2( cubeSize, -cubeSize,  cubeSize);
	MFloatPoint	vtx_3(-cubeSize,  cubeSize,  cubeSize);
	MFloatPoint	vtx_4( cubeSize,  cubeSize,  cubeSize);
	MFloatPoint	vtx_5(-cubeSize,  cubeSize, -cubeSize);
	MFloatPoint	vtx_6( cubeSize,  cubeSize, -cubeSize);
	MFloatPoint	vtx_7(-cubeSize, -cubeSize, -cubeSize);
	MFloatPoint	vtx_8( cubeSize, -cubeSize, -cubeSize);
#else
	MFloatPoint vtx_1( -cubeSize, -cubeSize, -cubeSize );
	MFloatPoint vtx_2(  cubeSize, -cubeSize, -cubeSize );
	MFloatPoint vtx_3(  cubeSize, -cubeSize,  cubeSize );
	MFloatPoint vtx_4( -cubeSize, -cubeSize,  cubeSize );
	MFloatPoint vtx_5( -cubeSize,  cubeSize, -cubeSize );
	MFloatPoint vtx_6( -cubeSize,  cubeSize,  cubeSize );
	MFloatPoint vtx_7(  cubeSize,  cubeSize,  cubeSize );
	MFloatPoint vtx_8(  cubeSize,  cubeSize, -cubeSize );
#endif
	points.append( vtx_1 );
	points.append( vtx_2 );
	points.append( vtx_3 );
	points.append( vtx_4 );
	points.append( vtx_5 );
	points.append( vtx_6 );
	points.append( vtx_7 );
	points.append( vtx_8 );
	int face_counts[numFaces] = { 4, 4, 4, 4, 4, 4 };
	MIntArray faceCounts( face_counts, numFaces );
#if defined(UV) && !defined(VERTEX_UV_MISMATCH)
	int	polygon_connects[] = {
		0, 1, 3, 2,
		2, 3, 5, 4,
		4, 5, 7, 6,
		6, 7, 1, 0,
		1, 7, 5, 3,
		0, 2, 4, 6,
	};
	const MIntArray	polygonConnects(polygon_connects, sizeof(polygon_connects) / sizeof(int));
#else
	int	face_connects[numFaceConnects] = {
		0, 1, 2, 3,
		4, 5, 6, 7,
		3, 2, 6, 5,
		0, 3, 5, 4,
		0, 4, 7, 1,
		1, 7, 6, 2,
	};
	MIntArray faceConnects( face_connects, numFaceConnects );
#endif
#ifdef UV
	const float	uvs[] = {
#ifndef VERTEX_UV_MISMATCH
		0.375, 0.0,
		0.625, 0.0,
		0.375, 0.25,
		0.625, 0.25,
		0.375, 0.5,
		0.625, 0.5,
		0.375, 0.75,
		0.625, 0.75,
		0.375, 1.0,
		0.625, 1.0,
		0.875, 0.0,
		0.875, 0.25,
		0.125, 0.0,
		0.125, 0.25,
#else
		0.375, 0.0,
		0.625, 0.0,
		0.625, 0.25,
		0.375, 0.25,
		0.625, 0.5,
		0.375, 0.5,
		0.625, 0.75,
		0.375, 0.75,
		0.625, 1.0,
		0.375, 1.0,
		0.875, 0.0,
		0.875, 0.25,
		0.125, 0.0,
		0.125, 0.25,
#endif
	};
	const int	num_uvs = sizeof(uvs) / sizeof(float) / 2;
	MFloatArray	uArray, vArray;
	for (int i = 0; i < num_uvs; ++i) {
		uArray.append(uvs[2 * i + 0]);
		vArray.append(uvs[2 * i + 1]);
	}
#if 1
	const int	uv_counts[] = {4, 4, 4, 4, 4, 4,};
	MIntArray	uvCounts(uv_counts, sizeof(uv_counts) / sizeof(int));
	const int	uv_ids[] = {
#ifndef VERTEX_UV_MISMATCH
		0, 1, 3, 2,
		2, 3, 5, 4,
		4, 5, 7, 6,
		6, 7, 9, 8,
		1, 10, 11, 3,
		0, 2, 13, 12,
#else
		0, 1, 2, 3,
		3, 2, 4, 5,
		5, 4, 6, 7,
		7, 6, 8, 9,
		1, 10, 11, 2,
		12, 0, 3, 13,
#endif
	};
	MIntArray	uvIds(uv_ids, sizeof(uv_ids) / sizeof(int));
#endif
#endif
	MObject	newMesh = meshFS.create(
		numVertices, numFaces,
		points, faceCounts,
#if defined(UV) && !defined(VERTEX_UV_MISMATCH)
		polygonConnects,
#else
		faceConnects,
#endif
#ifdef UV
#if 0
		uArray, vArray,
#endif
#endif
		outData, &stat);
#ifdef UV
	cout << meshFS.setUVs(uArray, vArray) << endl;
	cout << meshFS.assignUVs(uvCounts, uvIds) << endl;
#endif
	return newMesh;
}
#endif
#endif
