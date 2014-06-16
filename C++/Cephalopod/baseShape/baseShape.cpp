#pragma warning(disable: 4127)
#define _INHERIT_kBODY
#if 0
#include <maya/MFnMatrixData.h>
#endif
#if 0
#if 1
#include <maya/MModelMessage.h>
#include <tentacle/kTentacle.h>
#include <maya/MGlobal.h>
#include <maya/MQuaternion.h>
#include <maya/MFnCompoundAttribute.h>
#include <maya/MFloatPoint.h>
#include <maya/MFloatPointArray.h>
#include <maya/MFnVectorArrayData.h>
#include <maya/MFnDoubleArrayData.h>
#endif
#endif
#include <maya/MFnMesh.h>
#include <maya/MItMeshPolygon.h>
#if 0
#include <maya/MItMeshVertex.h>
#endif
#include <baseShape/kUtil.h>
#if 2
#if 4
#if 7
#include <maya/MFnStringData.h>
#endif
#ifdef _INHERIT_kBODY
#pragma warning(disable: 4250)
#endif
#include <simulation/kShSimulationMayaTentacle.h>
#ifdef _INHERIT_kBODY
#pragma warning(default: 4250)
#else
#include <simulation/kBodyMayaTentacle.h>
#endif
#endif
#include <maya/MItDag.h>
#include <Maya/MFnDagNode.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MFnTransform.h>
#include <maya/MAnimControl.h>
#endif
#if 1
#include <assert.h>
#include <float.h>
#include "baseShapeGeometryIterator.h"
#include <maya/MStringArray.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnDoubleIndexedComponent.h>
#include <maya/MPlugArray.h>
#endif
#include <math.h>
#include <maya/MIOStream.h>
#include <baseShape/baseShape.h>
#if 0
#if 1
#if 1
const MString	baseShape::TentacleArrayUpdated("hello, world: Tentacle Array Updated");
static void	__checkStatus(const MStatus& ms, const char* from = 0);
static int	compare(const void* arg1, const void* arg2);
MStatus
SetTentacleParameterValue(
	const MString*	strIndent0Ptr,
	const MPlug&	plug,
	MPlugArray&	hierarchy,
	bool*	isTentacleParameterPtr
);
static void	listRamp(
	MPlug	plug_ramp,
	MDataHandle	handle_ramp
);
#if 0
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
#include <baseShape/baseShapeUI.h>
#include <baseShape/baseShapeCreator.h>
#include <baseShape/baseShapeData.h>
#include "api_macros.h"
#include <maya/MFnPluginData.h>
#include <maya/MMatrix.h>
#include <maya/MAttributeSpecArray.h>
#include <maya/MAttributeSpec.h>
#include <maya/MAttributeIndex.h>
#include <maya/MObjectArray.h>
#include <maya/MFnSingleIndexedComponent.h>
#include <maya/MDagPath.h>
#include <maya/MFnAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MPointArray.h>
#if 9
#if 0
static bool	__call_generateHair_only = false;
#endif
#endif
#define _DEBUG2
#if 0
#if 1
namespace xxx {
void	strip(char** s_ptr)
{
	char*	s = *s_ptr;
	while (*s) {
		if (*s == ' ' || *s == '\t' || *s == '\n')
			++s;
		else
			break;
	}
	*s_ptr = s;
}
};
#else
namespace xxx {
void	strip(char** s_ptr)
{
	char*	s = *s_ptr;
	while (*s) {
		if (*s == ' ' || *s == '\t' || *s == '\n')
			++s;
		else
			break;
	}
	*s_ptr = s;
}
void	setIdArray(const char s0[], std::vector<int>& idArray, int endId_default = 99, int startId_default = 0)
{
	size_t	len = strlen(s0) + 1;
	char*	s = new char[len];
	strcpy(s, s0);
#ifdef _DEBUG
	char*	range_token_ptr0 = new char[len];
	char*	id_token_ptr0 = new char[len];
#endif
	char*	s_ptr = (char*) s;
#ifdef _DEBUG
	(void) printf("Info: s_ptr -> |%s|\n", s_ptr);
#endif
	char*	range_token_ptr = 0;
	while ((range_token_ptr = strsep(&s_ptr, ","))) {
#ifdef _DEBUG2
		(void) printf("Info: ================================================================\n");
		(void) printf("Info: range_token_ptr -> |%s| (s_ptr -> |%s|)\n", range_token_ptr, s_ptr);
#endif
#ifdef _DEBUG
		strcpy(range_token_ptr0, range_token_ptr);
#endif
		bool	isOk = true;
		int	startId0 = -1, endId0 = -1;
		int	n = 2;
		char*	id_token_ptr = 0;
		while (n-- && (id_token_ptr = strsep(&range_token_ptr, "-"))) {
#ifdef _DEBUG2
			(void) printf("Info: ------------------------------------------------\n");
			(void) printf("Info: id_token_ptr -> |%s| (range_token_ptr -> |%s|)\n", id_token_ptr, range_token_ptr);
			strcpy(id_token_ptr0, id_token_ptr);
#endif
			strip(&id_token_ptr);
			if (!*id_token_ptr) {
#ifdef _DEBUG2
				(void) printf("Info: id_token_ptr -> |%s|: default: Ok\n", id_token_ptr0);
#endif
				continue;
			}
			char*	next_ptr = 0;
			errno = 0;
			long	l = strtol(id_token_ptr, &next_ptr, 0);
			if (errno == ERANGE) {
#ifdef _DEBUG2
				printf("Error: id_token_ptr -> |%s|: ERANGE: Failure\n", id_token_ptr0);
#endif
				isOk = false;
				break;
			}
			if (next_ptr == id_token_ptr) {
#ifdef _DEBUG2
				(void) printf("Error: id_token_ptr -> |%s|: no conversion: Failure\n", id_token_ptr0);
#endif
				isOk = false;
				break;
			}
			strip(&next_ptr);
			if (*next_ptr) {
#ifdef _DEBUG2
				(void) printf("Error: id_token_ptr -> |%s|: invalid format: Failure\n", id_token_ptr0);
#endif
				isOk = false;
				break;
			}
			if (n == 1)
				startId0 = l;
			else
				endId0 = l;
#ifdef _DEBUG2
			(void) printf("Info: (errno = %d) long = %ld (next_ptr -> |%s|): Ok\n", errno, l, next_ptr);
#endif
		}
#ifdef _DEBUG2
		(void) printf("Info: n = %d, range_token_ptr -> |%s|\n", n, range_token_ptr);
#endif
		if (
			isOk == false ||
			range_token_ptr ||
			(n == 0 && startId0 == -1 && endId0 == -1)
		) {
#ifdef _DEBUG
			(void) printf("Error: range_token_ptr -> |%s|: invalid format: Failure\n", range_token_ptr0);
#endif
			continue;
		}
		int	startId = startId0 == -1 ? startId_default : startId0;
		int	endId = endId0 == -1 ? (n == 0 ? startId0 : endId_default) : endId0;
		if (endId < startId) {
#ifdef _DEBUG
			(void) printf("Error: range_token_ptr -> |%s|: invalid order: startId = %d(startId0 = %d), endId = %d(endId0 = %d): Failure\n", range_token_ptr0, startId, startId0, endId, endId0);
#endif
			continue;
		}
#ifdef _DEBUG
		(void) printf("Info: range_token_ptr0 -> |%s|: n = %d, startId = %d(startId0 = %d), endId = %d(endId0 = %d): Ok\n", range_token_ptr0, n, startId, startId0, endId, endId0);
#endif
		for (int id = startId; id <= endId; ++id) {
			bool	isExist = false;
			for (size_t n = 0; n < idArray.size(); ++n) {
				if (idArray.at(n) == id) {
					isExist = true;
					break;
				}
			}
			if (isExist == false)
				idArray.push_back(id);
		}
	}
	delete[] s;
#ifdef _DEBUG
	delete[] range_token_ptr0;
	delete[] id_token_ptr0;
#endif
}
}
#endif
#endif
#undef _DEBUG2
#if 0
MObject		baseShape::beInterpolated;
#if 1
MObject		baseShape::hairControlPoints;
MObject		baseShape::numHairs;
MObject		baseShape::numSamplePointsInHair;
#endif
#endif
MObject baseShape::inputSurface;
MObject baseShape::outputSurface;
MObject baseShape::cachedSurface;
MObject baseShape::worldSurface;
MObject baseShape::bboxCorner1;
MObject baseShape::bboxCorner2;
MObject		baseShape::time;
MObject		baseShape::wakes;
#if 2
#if 4
MObject		baseShape::worldMeshConnected;
MObject		baseShape::SimulationStatus;
#if 6
MObject		baseShape::FileCacheWrite;
MObject		baseShape::FileCacheRead;
MObject		baseShape::StrandIdList;
#endif
#if 7
MObject		baseShape::Dynamics;
baseShape::ShSimulationParameter	baseShape::__ShSimulationParameter;
#endif
#endif
#endif
#if 1
MObject		baseShape::selectBy;
MObject		baseShape::guide;
#if 0
#if 1
MObject		baseShape::rampDisplay;
MObject		baseShape::__rampWakes;
MObject		baseShape::__tentacleParameter;
#if 0
MObject		baseShape::transversal;
#endif
#if 1
#if 1
MObject		baseShape::__drawTentacleRoot;
MObject		baseShape::__tentacleRoot;
#endif
#if 0
MObject		baseShape::__tentacleRemovedIndex;
#endif
MObject		baseShape::__tentacleDoAction;
MObject		baseShape::__tentacleArray;
#if 1
int	baseShape::lastSelectedPhysicalIndex = -1;
#endif
#endif
#endif
#endif
#if 0
#if 1
const unsigned	baseShape::N_POINTS_IN_GUIDE	= NUM_TENTACLE_SAMPLE_POINTS;
#endif
#else
const unsigned	baseShape::N_POINTS_IN_GUIDE	= 31;
#endif
const double	STRAND_DEFAULT_LENGTH			= 15;
const double	SEGMENT_LENGTH_DEFAULT_IN_GUIDE		= STRAND_DEFAULT_LENGTH / (baseShape::N_POINTS_IN_GUIDE - 1);
MObject		baseShape::numPointsInGuide;
MObject		baseShape::seglen;
#if 0
unsigned	baseShape::N_POINTS_IN_HAIR		= 36;
MObject		baseShape::numPointsInHair;
const double	SEGMENT_LENGTH_DEFAULT_IN_HAIR		= 1.;
MObject		baseShape::segmentLengthInHair;
#endif
#if 0
MObject		baseShape::attenuateLength;
#endif
#if 2
#if 10
MObject		baseShape::__externalFileReadOnly;
bool	baseShape::__isExternalFileReadOnly() const
{
	MPlug	plug(thisMObject(), __externalFileReadOnly);
	bool	boolValue;
	plug.getValue(boolValue);
	return boolValue;
}
#endif
#if 11
MObject		baseShape::__shiftFrame;
int	baseShape::getShiftFrame() const
{
	MPlug	plug(thisMObject(), __shiftFrame);
	int	shiftFrame;
	plug.getValue(shiftFrame);
	return shiftFrame;
}
void	baseShape::setShiftFrame(int shiftFrame)
{
	MPlug	plug(thisMObject(), __shiftFrame);
	plug.setValue(shiftFrame);
}
#endif
#endif
int	gIndentDepth = 0;
inline MString	strIndent(void)
{
	MString	str;
	str += gIndentDepth;
	str += "\t";
	return str;
}
#endif
MTypeId	baseShape::id(0x80099 + 1);
baseShape::baseShape()
{
#ifdef baseShape_DEBUG
	cout << "baseShape::baseShape()" << endl;
#endif
#if 0
#if 1
#if 1
	MStatus	ms;
	__activeListModifiedCallbackId = MModelMessage::addCallback(MModelMessage::kActiveListModified, baseShape::__activeListModifiedCallback, this, &ms);
#endif
#endif
#endif
#if 0
	__hairInterpolatorPtr = NULL;
#endif
}
baseShape::~baseShape()
{
#ifdef baseShape_DEBUG
	cout << "baseShape::~baseShape()" << endl;
#endif
#if 0
#if 1
#if 1
	MStatus	ms;
	ms = MMessage::removeCallback(__activeListModifiedCallbackId);
#endif
#endif
#endif
#if 0
	if (__hairInterpolatorPtr) {
		delete __hairInterpolatorPtr;
		__hairInterpolatorPtr = NULL;
	}
#endif
#if 4
	if (__shSimulationMayaPtr) {
		delete __shSimulationMayaPtr;
		__shSimulationMayaPtr = 0;
	}
#endif
}
void	baseShape::postConstructor()
{
#ifdef baseShape_DEBUG
	cout << "baseShape::postConstructor()" << endl;
#endif
	setRenderable( true );
	fHasHistoryOnCreate = false;
#if 4
	__isWorldMeshConnected = false;
	__shSimulationMayaPtr = NULL;
	__simulationStatus = Simulation::Off;
#if 7
#if 0
#endif
	MStatus	ms;
	MPlug	plug_HBCParameter(thisMObject(), __ShSimulationParameter.HBCParameter);
	MFnNumericData	HBCParameter_val;
	MObject	HBCParameter_data = HBCParameter_val.create(MFnNumericData::k2Double, &ms);
	ms = HBCParameter_val.setData(0.30, 0.50);
	ms = plug_HBCParameter.setValue(HBCParameter_data);
#endif
#if 10
	__numExternalStrands = -1;
#endif
#endif
#if 0
	__numPolygons = -1;
	__displayHair = false;
	HairInterpolator::Arg	arg(N_POINTS_IN_GUIDE);
	__hairInterpolatorPtr = hairInterpolatorCreator(arg);
#endif
#if 0
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
}
#if 0
int&	baseShape::xRef()
{
	cout << "xRef: ENTER" << endl;
	MDataBlock	datablock = forceCache();
	cout << "xRef: Calling MDataBlock::inputValue()......" << endl;
	MDataHandle	handle = datablock.inputValue(numPointsInHair);
	cout << "xRef: Calling MDataBlock::inputValue()...Done" << endl;
	cout << "xRef: RETURN" << endl;
	return handle.asInt();
}
int	baseShape::calculateY()
{
	cout << "calculateY: ENTER" << endl;
	cout << "calculateY: Calling xRef()......" << endl;
	int&	iRef = xRef();
	cout << "calculateY: Calling xRef()...Done" << endl;
	int	i = iRef;
	++i;
	cout << "calculateY: RETURN" << endl;
	return iRef;
}
MStatus baseShape::computeX(const MPlug& plug, MDataBlock& datablock)
{
	cout << "computeX: ENTER" << endl;
	MStatus	ms;
	MDataHandle	inHandle = datablock.outputValue(numPointsInHair);
	inHandle.set(10);
	datablock.setClean(plug);
	cout << "computeX: RETURN" << endl;
	return MStatus::kSuccess;
}
#endif
MStatus	baseShape::compute(const MPlug& plug, MDataBlock& datablock)
{
	++gIndentDepth;
	MStatus	ms = __compute(plug, datablock);
	--gIndentDepth;
	return ms;
}
#if 0
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
#endif
#endif
MStatus	baseShape::__compute(const MPlug& plug, MDataBlock& datablock)
{
#ifdef baseShape_DEBUG
	cout << strIndent() << "baseShape::compute(): plug: |" << plug.info() << "|" << endl;
#endif
	if (plug == outputSurface) {
		return computeOutputSurface(plug, datablock);
	} else if (plug == cachedSurface) {
		return computeOutputSurface(plug, datablock);
	} else if (plug == worldSurface) {
		return computeWorldSurface(plug, datablock);
#if 0
	} else if (plug == numPointsInHair) {
		cout << "compute: (plug = numPointsInHair): ENTER" << endl;
		cout << "compute: Calling computeX()......" << endl;
		MStatus	ms = computeX(plug, datablock);
		cout << "compute: Calling computeX()...Done" << endl;
		cout << "coupute: RETURN" << endl;
		return ms;
#endif
#if 0
#if 1
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
			type == RampWakes::Rotation ||
			0
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
		if (action == TentacleDo::Add || action == TentacleDo::Update || action == TentacleDo::Move) {
			__setRampTupleArray(RampWakes::MajorRadius);
			__setRampTupleArray(RampWakes::Eccentricity);
			__setRampTupleArray(RampWakes::Rotation);
		}
		if (action == TentacleDo::Remove) {
#if 1
			int	iSelectBy;
			baseShape*	nonConstThis = (baseShape*) this;
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
				ms = outputArray.jumpToArrayElement(physicalIndex);
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
		}
		if (action == TentacleDo::Add) {
			MArrayDataHandle	outputArray = datablock.outputArrayValue(__tentacleArray);
			unsigned	numElements = outputArray.elementCount();
			MArrayDataBuilder	builder(&datablock, __tentacleArray, numElements + 1);
			for (int physicalIndex = 0; physicalIndex < (int) numElements; ++physicalIndex) {
				ms = outputArray.jumpToArrayElement(physicalIndex);
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
	MPlug	plug_parameter(thisMObject(), __tentacleParameter);
	MPlug	plug_parameter_length = plug_parameter.child(TENTACLE_PARAM_LENGTH, &ms);
	MPlug	plug_parameter_length_val = plug_parameter_length.child(TENTACLE_ATTRIBUTE_EDITOR_VAL, &ms);
	double	length;
	ms = plug_parameter_length_val.getValue(length);
	double	unit_length = length / (NUM_TENTACLE_SAMPLE_POINTS - 1);
	double	unit_t = unit_length / length;
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
		assert(baseShape::GetInterpolatedValue(__majorRadiusScaleRampTupleArray, x, y, true) == 0);
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
		assert(baseShape::GetInterpolatedValue(__eccentricityScaleRampTupleArray, x, y, true) == 0);
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
		assert(baseShape::GetInterpolatedValue(__rotationLocalampTupleArray, x, y, true) == 0);
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
		}
		if (action == TentacleDo::Update && hasActiveComponents()) {
			int	iSelectBy;
			baseShape*	nonConstThis = (baseShape*) this;
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
				ms = outputArray.jumpToArrayElement(physicalIndex);
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
		assert(baseShape::GetInterpolatedValue(__majorRadiusScaleRampTupleArray, x, y, true) == 0);
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
		assert(baseShape::GetInterpolatedValue(__eccentricityScaleRampTupleArray, x, y, true) == 0);
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
		assert(baseShape::GetInterpolatedValue(__rotationLocalampTupleArray, x, y, true) == 0);
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
			baseShape*	nonConstThis = (baseShape*) this;
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
				ms = outputArray.jumpToArrayElement(physicalIndex);
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
	MObject	data;
	MFnNumericData	val;
	ms = plug_tentacle_root_position.getValue(data);
	ms = val.setObject(data);
	double3		position;
	ms = val.getData(position[0], position[1], position[2]);
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
			double3&	root = handle_root.asDouble3();
			root[0] = position[0]; root[1] = position[1]; root[2] = position[2];
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
		assert(baseShape::GetInterpolatedValue(__majorRadiusScaleRampTupleArray, x, y, true) == 0);
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
		assert(baseShape::GetInterpolatedValue(__eccentricityScaleRampTupleArray, x, y, true) == 0);
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
		assert(baseShape::GetInterpolatedValue(__rotationLocalampTupleArray, x, y, true) == 0);
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
#if 1
		__computeBoundingBox(plug, datablock);
		childChanged(MPxSurfaceShape::kBoundingBoxChanged);
#endif
		return MStatus::kSuccess;
#endif
#endif
#endif
	} else {
#ifdef baseShape_DEBUG
		cout << strIndent() << "baseShape::compute(): MS::kUnknownParameter" << endl;
#endif
		return MS::kUnknownParameter;
	}
}
bool
baseShape::getInternalValue(
	const MPlug&	plug,
	MDataHandle&	result
)
{
	++gIndentDepth;
	bool	b = __getInternalValue(plug, result);
	--gIndentDepth;
	return b;
}
bool
baseShape::__getInternalValue(
	const MPlug&	plug,
	MDataHandle&	result
)
{
#ifdef baseShape_DEBUG
	cout << strIndent() << "baseShape::getInternalValue(): plug: |" << plug.info() << "(" << plug.partialName() << ")|" << endl;
#endif
	bool isOk = true;
#if 0
do {
	MStatus	ms;
	MStringArray	theArray;
	ms = plug.info().split('.', theArray);
	assert(ms);
	unsigned	len = theArray.length();
	if (len)
		break;
	int	index = theArray[1].index('[');
	--index;
	assert(index > 0 || index < -1);
	MString	name = theArray[1].substring(0, index);
	if (
#if 1
		name == "guide"
#else
		plug == guide
#endif
	) {
#ifdef baseShape_DEBUG
		cout << strIndent() << "baseShape::getInternalValue(): plug == guide" << endl;
		int	i = -1;
		if (index > 0) {
			if (len == 2) {
				i = plug.logicalIndex(&ms);
				assert(ms);
			} else {
				assert(len == 3);
				MPlug	parentPlug = plug.parent(&ms);
				assert(ms);
				i = parentPlug.logicalIndex(&ms);
				assert(ms);
			}
		}
		cout << strIndent() << "baseShape::getInternalValue(): index = " << i << endl;
		if (i >= 0) {
			MDataBlock	datablock = forceCache();
			MArrayDataHandle	cpHandle = datablock.outputArrayValue(mControlPoints, &ms);
			assert(ms);
			unsigned	n = cpHandle.elementCount(&ms);
			assert(ms);
			cout << "n = " << n << endl;
			if (!n) {
				cout << "To be continued..." << endl;
				return false;
			}
			cout << "n > 0" << endl;
			cpHandle.jumpToElement(i);
			MDataHandle	pntHandle = cpHandle.inputValue(&ms);
			assert(ms);
			double3&	pnt = pntHandle.asDouble3();
			result.set(pnt[0], pnt[1], pnt[2]);
			return true;
		}
#endif
		return true;
	}
} while (false);
#endif
#if 0
	if (plug == hairControlPoints || plug == numHairs || plug == numSamplePointsInHair) {
		if (!__hairInterpolatorPtr || !__displayHair || __numPolygons == -1) {
			if (plug == hairControlPoints)
				result.set(0, 0, 0);
			else if (plug == numHairs)
				result.set(-1);
			else
				result.set(-1);
			return true;
		}
		if (plug == numSamplePointsInHair) {
			result.set((int) __hairInterpolatorPtr->numPoints());
			return true;
		}
		if (plug == numHairs) {
			unsigned	nHair = 0;
			const HairInterpolator::HairArray&	hairArray = __hairInterpolatorPtr->getHairArray();
			for (int polyId = 0; polyId < __numPolygons; ++polyId) {
				const HairInterpolator::HairPtr	hairPtr = polyId >= (int) hairArray.length() ? NULL : hairArray[polyId];
				if (!hairPtr)
					continue;
				nHair += hairPtr->nHair();
			}
			result.set((int) nHair);
			return true;
		}
		if (plug == hairControlPoints) {
			MStatus	ms;
			unsigned	indexTotalSamplePoints = plug.logicalIndex(&ms);
			if (!ms) {
				result.set(0, 0, 0);
				return true;
			}
			const HairInterpolator::HairArray&	hairArray = __hairInterpolatorPtr->getHairArray();
			const unsigned	N = __hairInterpolatorPtr->numPoints();
			unsigned	numTotalSamplePoints = 0;
			for (int polyId = 0; polyId < __numPolygons; ++polyId) {
				const HairInterpolator::HairPtr	hairPtr = polyId >= (int) hairArray.length() ? NULL : hairArray[polyId];
				if (!hairPtr)
					continue;
				unsigned	numSamplePointsInPolygon = hairPtr->nHair() * N;
				if (numTotalSamplePoints + numSamplePointsInPolygon <= indexTotalSamplePoints) {
					numTotalSamplePoints += numSamplePointsInPolygon;
					continue;
				}
				unsigned	n = indexTotalSamplePoints - numTotalSamplePoints;
				const MVectorArray&	hairs = hairPtr->hairs();
				const MVector&	hairPoint = hairs[n];
				result.set(hairPoint.x, hairPoint.y, hairPoint.z);
				return true;
			}
			result.set(0, 0, 0);
			return true;
		}
		return true;
	}
#endif
	if (plug == seglen) {
		MStatus	ms;
		int	index = plug.logicalIndex(&ms);
		if (index >= 0 && index < (int) __segmentLengthInGuide.size())
			result.set(__segmentLengthInGuide.at(index));
		return true;
	}
#if 4
#if 0
	if (plug == segmentLengthInHair) {
		cout << "Calling calculateY()......" << endl;
		int	i = calculateY();
		cout << "Calling calculateY()...Done" << endl;
		result.set((double) i);
		return true;
	}
#endif
#endif
#if 0
	if (plug == beInterpolated) {
		result.set(__displayHair);
		return true;
	}
#endif
	if (
		plug == mControlPoints ||
		plug == mControlValueX ||
		plug == mControlValueY ||
		plug == mControlValueZ
	) {
#ifdef baseShape_DEBUG2
		cout << strIndent() << "baseShape::getInternalValue(): plug == mControlPoints: " << (plug == mControlPoints ? "True" : "False") << endl;
		cout << strIndent() << "baseShape::getInternalValue(): plug == mControlValueX: " << (plug == mControlValueX ? "True" : "False") << endl;
		cout << strIndent() << "baseShape::getInternalValue(): plug == mControlValueY: " << (plug == mControlValueY ? "True" : "False") << endl;
		cout << strIndent() << "baseShape::getInternalValue(): plug == mControlValueZ: " << (plug == mControlValueZ ? "True" : "False") << endl;
		cout << strIndent() << "baseShape::getInternalValue(): plug == mControlValue: hasHistory(): " << (hasHistory() ? "True" : "False") << endl;
#endif
		if (hasHistory()) {
			MObject	updatedMeshDataRefObj = meshDataRef();
#ifdef baseShape_DEBUG
			cout << strIndent() << "baseShape::getInternalValue(): Calling MPxNode::getInternalValue()..." << endl;
#endif
#if 1
#else
			MStatus	ms;
			if (plug == mControlPoints && !plug.isArray(&ms) && ms) {
				MPoint	pnt;
				int	index = plug.logicalIndex(&ms);
				if (ms && index % N_POINTS_IN_GUIDE == 0 && value(index / N_POINTS_IN_GUIDE, pnt)) {
#ifdef baseShape_DEBUG
					cout << strIndent() << "baseShape::getInternalValue(): |.cv[" << (index / N_POINTS_IN_GUIDE) << "][0]: Set manually..." << endl;
#endif
#if 0
					cout << pnt << endl;
#endif
#if 1
#else
					MDataBlock	datablock = forceCache();
					MMatrix	worldMat = getWorldMatrix(datablock, 0);
#endif
#if 0
					cout << worldMat << endl;
#endif
#if 0
					baseShapeGeometry*	geometryPtr = meshGeom();
					pnt = geometryPtr->vertices[index / N_POINTS_IN_GUIDE];
					cout << pnt << endl;
#endif
#if 0
					MPoint	point = pnt;
					MVectorArray*	cpPtr = getControlPoints();
					MVectorArray&	cp = *cpPtr;
					for (int i = 1; i < N_POINTS_IN_GUIDE; ++i) {
						MPoint	pt = cp[index + i];
						cout << pt << endl;
						point += pt;
						cout << "\t" << point << endl;
						cout << "\t" << point * worldMat << endl;
					}
#endif
#if 1
#else
					pnt *= worldMat;
#endif
#if 0
					cout << pnt << endl;
#endif
					result.set(pnt[0], pnt[1], pnt[2]);
					return true;
				}
			} else if (plug == mControlValueX) {
				MPlug	parentPlug = plug.parent();
				int	index = parentPlug.logicalIndex(&ms);
				double	val;
				if (ms && index % N_POINTS_IN_GUIDE == 0 && value(index / N_POINTS_IN_GUIDE, 0, val)) {
					result.set(val);
					return true;
				}
			} else if (plug == mControlValueY) {
				MPlug	parentPlug = plug.parent();
				int	index = parentPlug.logicalIndex(&ms);
				double	val;
				if (ms && index % N_POINTS_IN_GUIDE == 0 && value(index / N_POINTS_IN_GUIDE, 1, val)) {
					result.set(val);
					return true;
				}
			} else if (plug == mControlValueZ) {
				MPlug	parentPlug = plug.parent();
				int	index = parentPlug.logicalIndex(&ms);
				double	val;
				if (ms && index % N_POINTS_IN_GUIDE == 0 && value(index / N_POINTS_IN_GUIDE, 2, val)) {
					result.set(val);
					return true;
				}
			}
#endif
			return MPxNode::getInternalValue(plug, result);
		} else {
			double	val = 0.0;
			if (plug == mControlPoints && !plug.isArray()) {
				MPoint	pnt;
				int	index = plug.logicalIndex();
				value(index, pnt);
				result.set(pnt[0], pnt[1], pnt[2]);
			} else if (plug == mControlValueX) {
				MPlug	parentPlug = plug.parent();
				int	index = parentPlug.logicalIndex();
				value(index, 0, val);
				result.set(val);
			} else if (plug == mControlValueY) {
				MPlug	parentPlug = plug.parent();
				int	index = parentPlug.logicalIndex();
				value(index, 1, val);
				result.set(val);
			} else if (plug == mControlValueZ) {
				MPlug	parentPlug = plug.parent();
				int	index = parentPlug.logicalIndex();
				value(index, 2, val);
				result.set(val);
			}
		}
	} else if (plug == mHasHistoryOnCreate) {
		result.set(fHasHistoryOnCreate);
#if 1
	} else if (plug == selectBy) {
		return false;
#endif
	} else {
#ifdef baseShape_DEBUG
		cout << strIndent() << "baseShape::getInternalValue(): Calling MPxSurfaceShape::getInternalValue()..." << endl;
#endif
		isOk = MPxSurfaceShape::getInternalValue(plug, result);
	}
	return isOk;
}
bool
baseShape::setInternalValue(
	const MPlug&	plug,
	const MDataHandle&	handle
)
{
	++gIndentDepth;
	bool	b = __setInternalValue(plug, handle);
	--gIndentDepth;
	return b;
}
bool	baseShape::__setInternalValue(const MPlug& plug, const MDataHandle& handle)
{
	MStatus	ms;
#ifdef baseShape_DEBUG
	cout << strIndent() << "baseShape::setInternalValue(): plug: |" << plug.info() << "|" << endl;
#endif
	bool	isOk = true;
#if 2
#if 11
	if (plug == __shiftFrame) {
		int	shiftFrame = handle.asInt();
		if (shiftFrame < 0) {
			cerr << "Error: shiftFrame = " << shiftFrame << ": should be non-negative" << endl;
			return true;
		}
		return false;
	}
#endif
#if 10
	if (plug == __externalFileReadOnly) {
		bool	isExternalFileReadOnly = handle.asBool();
		if (isExternalFileReadOnly) {
#if 0
			if (__hairInterpolatorPtr) {
				delete __hairInterpolatorPtr;
				__hairInterpolatorPtr = NULL;
			}
			MPlug	plug(thisMObject(), beInterpolated);
			plug.setValue(false);
			__displayHair = false;
#endif
#if 0
			plug.setAttribute(selectBy);
#else
			MPlug	plug(thisMObject(), selectBy);
#endif
			short	shortValue = selectByGuide;
			plug.setValue(shortValue);
		} else {
#if 0
			HairInterpolator::Arg	arg(N_POINTS_IN_GUIDE);
			__hairInterpolatorPtr = hairInterpolatorCreator(arg);
#endif
		}
		return false;
	}
	if (plug == selectBy) {
		if (__isExternalFileReadOnly()) {
			cerr << "External File Read Only Mode: Ignored" << endl;
			return true;
		}
		return false;
	}
#endif
#endif
	if (plug == seglen) {
		int index = plug.logicalIndex();
		if (index >= 0 && index < (int) __segmentLengthInGuide.size()) {
			double	len = handle.asDouble();
			__segmentLengthInGuide.at(index) = len;
#if 2
			if (__shSimulationMayaPtr) {
				(void) __shSimulationMayaPtr->setStrandLength(index, (N_POINTS_IN_GUIDE - 1) * len);
			}
#endif
			verticesUpdated();
			MDataBlock	datablock = forceCache();
			MArrayDataHandle	cpH = datablock.outputArrayValue(mControlPoints);
			MArrayDataBuilder	oldBuilder = cpH.builder();
			MArrayDataBuilder	builder(oldBuilder);
			for (int k = 1; k < (int) N_POINTS_IN_GUIDE; ++k) {
				double3&	n = builder.addElement(N_POINTS_IN_GUIDE * index + k).asDouble3();
				MVector	offset(n[0], n[1], n[2]);
				offset.normalize();
				offset = len * offset;
				n[0] = offset.x;
				n[1] = offset.y;
				n[2] = offset.z;
			}
			cpH.set(builder);
			cpH.setAllClean();
		}
		return true;
	}
#if 0
	if (plug == attenuateLength) {
		if (!__externalDagPathShape.isValid()) {
			cerr << "Attenuate Length: No external node" << endl;
			return true;
		}
		int index = plug.logicalIndex();
		if (index < 0 && index >= (int) __segmentLengthInGuide.size()) {
			cerr << "Attenuate Length: index = " << index << ": OUT_OF_RANGE" << endl;
			return true;
		}
		bool	attenuateLengthTrue = handle.asBool();
		double	factor = __attenuateLength_factorArray[index];
		if (!attenuateLengthTrue)
			factor = 1 / factor;
		MDataBlock	datablock = forceCache();
		MArrayDataHandle	cpH = datablock.outputArrayValue(mControlPoints);
		MArrayDataBuilder	oldBuilder = cpH.builder();
		MArrayDataBuilder	builder(oldBuilder);
		for (int k = 1; k < (int) N_POINTS_IN_GUIDE; ++k) {
			double3&	n = builder.addElement(N_POINTS_IN_GUIDE * index + k).asDouble3();
			MVector	offset(n[0], n[1], n[2]);
			offset = factor * offset;
			n[0] = offset.x;
			n[1] = offset.y;
			n[2] = offset.z;
		}
		cpH.set(builder);
		cpH.setAllClean();
		__segmentLengthInGuide.at(index) = factor * __segmentLengthInGuide.at(index);
		verticesUpdated();
		return true;
	}
#endif
#if 4
#if 7
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
	if (plug == Dynamics) {
		bool	dynamics = handle.asBool();
		if (__shSimulationMayaPtr && __simulationStatus != Simulation::ReadOnly)
			__shSimulationMayaPtr->setDynamicsFlag(dynamics);
		return false;
	}
	if (plug == __ShSimulationParameter.InverseTimeStep) {
		int	InverseTimeStep = handle.asInt();
		if (__shSimulationMayaPtr) {
			cerr << "Warning: InverseTimeStep applied ONLY before simulation" << endl;
			return true;
		}
		if (InverseTimeStep != 240) {
			cerr << "Waring: Inverse Time Step: AS OF 3/8/2009 (EST), Only 240 in Maya Plug-in" << endl;;
			return true;
		}
		return false;
	}
	if (plug == __ShSimulationParameter.ParameterFileRead || plug == __ShSimulationParameter.ParameterFileWrite) {
		MString&	fileName = handle.asString();
		cout << "hello, world: parameterFile: |" << plug.info() << "|:= |" << fileName << "|" << endl;
		ShSimParameter	parameter;
		if (plug == __ShSimulationParameter.ParameterFileRead) {
			if (parameter.fileReadEveryRecord((char*) fileName.asChar()) < 0) {
				cerr << "ShSimParameter::fileReadEveryRecord(): Failure" << endl;
			} else {
				printSimulationParameter(parameter);
				setShSimulationParameterFromShSimParameter(parameter, __shSimulationMayaPtr == NULL);
				if (__shSimulationMayaPtr && __simulationStatus != Simulation::ReadOnly) {
					applySimulationParameter(false);
				}
			}
		} else {
			if (__shSimulationMayaPtr) {
				ShSimParameter&	simulationParameter = __shSimulationMayaPtr->simulationParameter();
				if (simulationParameter.fileWriteEveryRecord((char*) fileName.asChar()) < 0) {
					cerr << "ShSimParameter::fileWriteEveryRecord(): Failure" << endl;
				}
			} else {
				setShSimulationParameterToShSimParameter(parameter);
				printSimulationParameter(parameter);
				if (parameter.fileWriteEveryRecord((char*) fileName.asChar()) < 0) {
					cerr << "ShSimParameter::fileWriteEveryRecord(): Failure" << endl;
				}
			}
		}
		return false;
	}
	MString	long_attribute_name_only = plug.partialName(false, false, false, false, false, true);
	if (
		plug == __ShSimulationParameter.RaceType ||
		plug == __ShSimulationParameter.IncludeStiffness ||
		plug == __ShSimulationParameter.YoungModulus ||
#if 1
		plug == __ShSimulationParameter.fAverageRadius ||
		plug == __ShSimulationParameter.fEllipticity ||
		plug == __ShSimulationParameter.fVolumeDensity ||
		plug == __ShSimulationParameter.fHelixK0STep ||
		plug == __ShSimulationParameter.fHelixK1Curvature ||
		plug == __ShSimulationParameter.inHelixK0STep ||
		plug == __ShSimulationParameter.inHelixK1Curvature ||
		plug == __ShSimulationParameter.fLengthFactor ||
#endif
		plug == __ShSimulationParameter.IncludeGravityAndInertia ||
		plug == __ShSimulationParameter.GravityParameter ||
		plug == __ShSimulationParameter.IncludeAirFriction ||
		plug == __ShSimulationParameter.AirFrictionParameter ||
		plug == __ShSimulationParameter.DissipationMode ||
		plug == __ShSimulationParameter.DissipationLambdaParameter ||
		plug == __ShSimulationParameter.DissipationGammaParameter ||
		plug == __ShSimulationParameter.WindMode ||
		plug == __ShSimulationParameter.WindRatio ||
		plug == __ShSimulationParameter.WindDirection2D ||
#if 1
		long_attribute_name_only == "WindDirection3D0" ||
		long_attribute_name_only == "WindDirection3D1" ||
#else
		plug == __ShSimulationParameter.WindDirection3D ||
#endif
		plug == __ShSimulationParameter.IncludeHairBodyOptimalCollision ||
		plug == __ShSimulationParameter.IncludeHairBodyCollision ||
#if 1
		long_attribute_name_only == "HBCParameter0" ||
		long_attribute_name_only == "HBCParameter1" ||
#else
		plug == __ShSimulationParameter.HBCParameter ||
#endif
		plug == __ShSimulationParameter.IncludeHairHairCollision ||
		plug == __ShSimulationParameter.HHCParameter
	) {
		kSimulationCache::kSimulationParameter::kType	type;
		if (plug == __ShSimulationParameter.RaceType)
			type = kSimulationCache::kSimulationParameter::RaceType;
		else if (plug == __ShSimulationParameter.IncludeStiffness)
			type = kSimulationCache::kSimulationParameter::IncludeStiffness;
		else if (plug == __ShSimulationParameter.YoungModulus)
			type = kSimulationCache::kSimulationParameter::YoungModulus;
#if 1
		else if (plug == __ShSimulationParameter.fAverageRadius)
			type = kSimulationCache::kSimulationParameter::fAverageRadius;
		else if (plug == __ShSimulationParameter.fEllipticity)
			type = kSimulationCache::kSimulationParameter::fEllipticity;
		else if (plug == __ShSimulationParameter.fVolumeDensity)
			type = kSimulationCache::kSimulationParameter::fVolumeDensity;
		else if (plug == __ShSimulationParameter.fHelixK0STep)
			type = kSimulationCache::kSimulationParameter::fHelixK0STep;
		else if (plug == __ShSimulationParameter.fHelixK1Curvature)
			type = kSimulationCache::kSimulationParameter::fHelixK1Curvature;
		else if (plug == __ShSimulationParameter.inHelixK0STep)
			type = kSimulationCache::kSimulationParameter::inHelixK0K1;
		else if (plug == __ShSimulationParameter.inHelixK1Curvature)
			type = kSimulationCache::kSimulationParameter::inHelixK0K1;
		else if (plug == __ShSimulationParameter.fLengthFactor)
			type = kSimulationCache::kSimulationParameter::fLengthFactor;
#endif
		else if (plug == __ShSimulationParameter.IncludeGravityAndInertia)
			type = kSimulationCache::kSimulationParameter::IncludeGravityAndInertia;
		else if (plug == __ShSimulationParameter.GravityParameter)
			type = kSimulationCache::kSimulationParameter::GravityParameter;
		else if (plug == __ShSimulationParameter.IncludeAirFriction)
			type = kSimulationCache::kSimulationParameter::IncludeAirFriction;
		else if (plug == __ShSimulationParameter.AirFrictionParameter)
			type = kSimulationCache::kSimulationParameter::AirFrictionParameter;
		else if (plug == __ShSimulationParameter.DissipationMode)
			type = kSimulationCache::kSimulationParameter::DissipationMode;
		else if (plug == __ShSimulationParameter.DissipationLambdaParameter)
			type = kSimulationCache::kSimulationParameter::DissipationLambdaParameter;
		else if (plug == __ShSimulationParameter.DissipationGammaParameter)
			type = kSimulationCache::kSimulationParameter::DissipationGammaParameter;
		else if (plug == __ShSimulationParameter.WindMode)
			type = kSimulationCache::kSimulationParameter::WindMode;
		else if (plug == __ShSimulationParameter.WindRatio)
			type = kSimulationCache::kSimulationParameter::WindRatio;
		else if (plug == __ShSimulationParameter.WindDirection2D)
			type = kSimulationCache::kSimulationParameter::WindDirection2D;
#if 1
		else if (
			long_attribute_name_only == "WindDirection3D0" ||
			long_attribute_name_only == "WindDirection3D1"
		)
#else
		else if (plug == __ShSimulationParameter.WindDirection3D)
#endif
			type = kSimulationCache::kSimulationParameter::WindDirection3D;
		else if (plug == __ShSimulationParameter.IncludeHairBodyOptimalCollision)
			type = kSimulationCache::kSimulationParameter::IncludeHairBodyOptimalCollision;
		else if (plug == __ShSimulationParameter.IncludeHairBodyCollision)
			type = kSimulationCache::kSimulationParameter::IncludeHairBodyCollision;
#if 1
		else if (
			long_attribute_name_only == "HBCParameter0" ||
			long_attribute_name_only == "HBCParameter1"
		)
#else
		else if (plug == __ShSimulationParameter.HBCParameter)
#endif
			type = kSimulationCache::kSimulationParameter::HBCParameter;
		else if (plug == __ShSimulationParameter.IncludeHairHairCollision)
			type = kSimulationCache::kSimulationParameter::IncludeHairHairCollision;
		else if (plug == __ShSimulationParameter.HHCParameter)
			type = kSimulationCache::kSimulationParameter::HHCParameter;
		else
			return true;
		kSimulationCache::kSimulationParameter::kValue	val;
		if (
			plug == __ShSimulationParameter.IncludeStiffness ||
			plug == __ShSimulationParameter.IncludeGravityAndInertia ||
			plug == __ShSimulationParameter.IncludeAirFriction ||
			plug == __ShSimulationParameter.IncludeHairBodyOptimalCollision ||
			plug == __ShSimulationParameter.IncludeHairBodyCollision ||
			plug == __ShSimulationParameter.IncludeHairHairCollision
		) {
			val.bVal = handle.asBool();
		} else if (
			plug == __ShSimulationParameter.YoungModulus ||
#if 1
			plug == __ShSimulationParameter.fAverageRadius ||
			plug == __ShSimulationParameter.fEllipticity ||
			plug == __ShSimulationParameter.fVolumeDensity ||
			plug == __ShSimulationParameter.fHelixK0STep ||
			plug == __ShSimulationParameter.fHelixK1Curvature ||
			plug == __ShSimulationParameter.fLengthFactor ||
#endif
			plug == __ShSimulationParameter.DissipationLambdaParameter ||
			plug == __ShSimulationParameter.DissipationGammaParameter
		) {
			val.dVal = handle.asDouble();
		} else if (
			plug == __ShSimulationParameter.GravityParameter ||
			plug == __ShSimulationParameter.AirFrictionParameter ||
			plug == __ShSimulationParameter.WindRatio ||
			plug == __ShSimulationParameter.WindDirection2D
		) {
			val.iVal = handle.asInt();
		} else if (plug == __ShSimulationParameter.RaceType) {
			numRaceType	raceType = (numRaceType) handle.asShort();
			if (raceType == RACE_ASIAN || raceType == RACE_CAUCASIAN1 || raceType == RACE_CAUCASIAN2 || raceType == RACE_AFRICAN) {
				ShSimParameter*	simulationParameterPtr = 0;
				if (!__shSimulationMayaPtr || __simulationStatus == Simulation::ReadOnly) {
					ShSimParameter	paramerer;
					paramerer.setRaceDependentParameter(raceType);
					simulationParameterPtr = &paramerer;
					MPlug	plug(thisMObject(), __ShSimulationParameter.YoungModulus);
					plug.setValue((int) simulationParameterPtr->fYoungModulus);
#if 1
					plug.setAttribute(__ShSimulationParameter.fAverageRadius);
#if 1
					plug.setValue((int) simulationParameterPtr->sSection.fAverageRadius);
#else
					plug.setValue((int) simulationParameterPtr->fAverageRadius);
#endif
					plug.setAttribute(__ShSimulationParameter.fEllipticity);
#if 1
					plug.setValue((int) simulationParameterPtr->sSection.fEllipticity);
#else
					plug.setValue((int) simulationParameterPtr->fEllipticity);
#endif
					plug.setAttribute(__ShSimulationParameter.fVolumeDensity);
					plug.setValue((int) simulationParameterPtr->fVolumeDensity);
					plug.setAttribute(__ShSimulationParameter.fHelixK0STep);
					plug.setValue((int) simulationParameterPtr->fHelixK0STep);
					plug.setAttribute(__ShSimulationParameter.fHelixK1Curvature);
					plug.setValue((int) simulationParameterPtr->fHelixK1Curvature);
#endif
				} else {
					simulationParameterPtr = &__shSimulationMayaPtr->simulationParameter();
				}
#if 0
				__ShSimulationParameter.fAverageRadius = simulationParameterPtr->fAverageRadius;
				__ShSimulationParameter.fEllipticity = simulationParameterPtr->fEllipticity;
				__ShSimulationParameter.fHelixK0STep = simulationParameterPtr->fHelixK0STep;
				__ShSimulationParameter.fHelixK1Curvature = simulationParameterPtr->fHelixK1Curvature;
#endif
			}
			val.raceType = raceType;
		} else if (plug == __ShSimulationParameter.DissipationMode) {
			val.dissipationMode = (numDissipationMode) handle.asShort();
#if 1
		} else if (
			long_attribute_name_only == "WindDirection3D0" ||
			long_attribute_name_only == "WindDirection3D1"
		) {
			MPlug	plug_WindDirection3D(thisMObject(), __ShSimulationParameter.WindDirection3D);
			MObject	WindDirection3D_data;
			plug_WindDirection3D.getValue(WindDirection3D_data);
			MFnNumericData	WindDirection3D_val(WindDirection3D_data);
			WindDirection3D_val.getData(val.i2Val[0], val.i2Val[1]);
			if (long_attribute_name_only == "WindDirection3D0") {
				val.i2Val[0] = handle.asInt();
			} else {
				val.i2Val[1] = handle.asInt();
			}
		} else if (plug == __ShSimulationParameter.WindMode) {
#else
		} else if (plug == __ShSimulationParameter.WindDirection3D) {
			double3&	d3 = handle.asDouble3();
			val.v3Val[0] = d3[0];
			val.v3Val[1] = d3[1];
			val.v3Val[2] = d3[2];
		} else if (plug == __ShSimulationParameter.WindMode) {
#endif
			val.windMode = (numWindMode) handle.asShort();
#if 1
		} else if (plug == __ShSimulationParameter.inHelixK0STep || plug == __ShSimulationParameter.inHelixK1Curvature) {
			MPlug	plug(thisMObject(), __ShSimulationParameter.inHelixK0STep);
			plug.getValue(val.v2Val[0]);
			plug.setAttribute(__ShSimulationParameter.inHelixK1Curvature);
			plug.getValue(val.v2Val[1]);
			if (plug == __ShSimulationParameter.inHelixK0STep)
				val.v2Val[0] = handle.asDouble();
			else
				val.v2Val[1] = handle.asDouble();
#endif
#if 1
		} else if (
			long_attribute_name_only == "HBCParameter0" ||
			long_attribute_name_only == "HBCParameter1"
		) {
			MPlug	plug_HBCParameter(thisMObject(), __ShSimulationParameter.HBCParameter);
			MObject	HBCParameter_data;
			plug_HBCParameter.getValue(HBCParameter_data);
			MFnNumericData	HBCParameter_val(HBCParameter_data);
			HBCParameter_val.getData(val.v2Val[0], val.v2Val[1]);
			if (long_attribute_name_only == "HBCParameter0") {
				val.v2Val[0] = handle.asDouble();
			} else {
				val.v2Val[1] = handle.asDouble();
			}
		} else if (plug == __ShSimulationParameter.HHCParameter) {
#else
		} else if (plug == __ShSimulationParameter.HBCParameter) {
			double2&	d2 = handle.asDouble2();
			val.v2Val[0] = d2[0];
			val.v2Val[1] = d2[1];
		} else if (plug == __ShSimulationParameter.HHCParameter) {
#endif
			cout << "Hair-Hair Collision: AS OF 3/7/2009 (EST), NOT IMPLEMENTED YET (INCOMPLETE)" << endl;
		} else {
			return true;
		}
		if (__shSimulationMayaPtr && __simulationStatus != Simulation::ReadOnly) {
			__shSimulationMayaPtr->setSimulationParameter(type, val);
			if (type == kSimulationCache::kSimulationParameter::RaceType) {
				ShSimParameter&	simulationParameter = __shSimulationMayaPtr->simulationParameter();
				ShScalar	fYoungModulus = simulationParameter.fYoungModulus;
				MPlug	plug(thisMObject(), __ShSimulationParameter.YoungModulus);
				plug.setValue((double) fYoungModulus);
#if 1
#if 1
				ShScalar	fAverageRadius = simulationParameter.sSection.fAverageRadius;
#else
				ShScalar	fAverageRadius = simulationParameter.fAverageRadius;
#endif
				plug.setAttribute(__ShSimulationParameter.fAverageRadius);
				plug.setValue((double) fAverageRadius);
#if 1
				ShScalar	fEllipticity = simulationParameter.sSection.fEllipticity;
#else
				ShScalar	fEllipticity = simulationParameter.fEllipticity;
#endif
				plug.setAttribute(__ShSimulationParameter.fEllipticity);
				plug.setValue((double) fEllipticity);
				ShScalar	fVolumeDensity = simulationParameter.fVolumeDensity;
				plug.setAttribute(__ShSimulationParameter.fVolumeDensity);
				plug.setValue((double) fVolumeDensity);
				ShScalar	fHelixK0STep = simulationParameter.fHelixK0STep;
				plug.setAttribute(__ShSimulationParameter.fHelixK0STep);
				plug.setValue((double) fHelixK0STep);
				ShScalar	fHelixK1Curvature = simulationParameter.fHelixK1Curvature;
				plug.setAttribute(__ShSimulationParameter.fHelixK1Curvature);
				plug.setValue((double) fHelixK1Curvature);
#endif
			}
		}
		return false;
	}
#endif
#if 6
	if (
		plug == FileCacheRead ||
		plug == FileCacheWrite ||
		plug == StrandIdList
	) {
		MString&	filename = handle.asString();
		cout << "hello, world: fileCache: |" << plug.info() << "|:= |" << filename << "|" << endl;
		if (plug == FileCacheWrite) {
			MString&	fileCacheWriteFilename = handle.asString();
			if (!__shSimulationMayaPtr) {
				cerr << "Warning: No ShSimulation" << endl;
			}
			if (__shSimulationMayaPtr && __simulationStatus != Simulation::On) {
				cerr << "Warning: ShSimulation: Status:= " << (__simulationStatus == Simulation::On ? "On" : (__simulationStatus == Simulation::Paused ? "Paused" : (__simulationStatus == Simulation::ReadOnly ? "ReadOnly" : "Unknown"))) << ": Not On" << endl;
			}
			if (__shSimulationMayaPtr && __simulationStatus == Simulation::On) {
				__shSimulationMayaPtr->writeToFileCache(fileCacheWriteFilename.asChar());
			}
		}
		return false;
	}
#endif
#if 0
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
#ifdef _DEBUG
			ShSimParameter&	simulationParameter = __shSimulationMayaPtr->simulationParameter();
			baseShape::printSimulationParameter(simulationParameter);
#endif
			MPlug	plug(thisMObject(), Dynamics);
			bool	dynamics;
			plug.getValue(dynamics);
			__shSimulationMayaPtr->setDynamicsFlag(dynamics);
			applySimulationParameter(true);
#ifdef _DEBUG
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
	MPlug	idPlug(thisMObject(), StrandIdList);
	MString	idList;
	idPlug.getValue(idList);
	int	lastStrandId = cachedGeom()->vertices.length() - 1;
	std::vector<int>	strandIdArray;
#if 1
	strandIdArray.clear();
	cout << "|" << idList << "|" << endl;
	MStringArray	idListArray;
	idList.split(',', idListArray);
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
			xxx::strip(&id_token_ptr);
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
				xxx::strip(&next_ptr);
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
if (__simulationStatus == Simulation::ReadOnly) {
	std::vector<double>	strandLengthArray(__segmentLengthInGuide.size());
	MPlug	plug(thisMObject(), FileCacheRead);
	MString	fileCacheReadFilename;
	plug.getValue(fileCacheReadFilename);
	if (__shSimulationMayaPtr->setStrandFromFileCache(fileCacheReadFilename.asChar(), strandIdArray, &strandLengthArray) < 0) {
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
#ifdef _INHERIT_kBODY
	kBodyMayaTentacle*	bodyMayaTentaclePtr = (kBodyMayaTentacle*) __shSimulationMayaPtr;
#else
	kBodyMayaTentacle*	bodyMayaTentaclePtr = (kBodyMayaTentacle*) __shSimulationMayaPtr->bodyMayaPtr();
#endif
	const kPtrArray<kTentacle>&	tentacleArray0 = bodyMayaTentaclePtr->tentacleArray0();
	for (unsigned n = 0; n < strandIdArray.size(); ++n) {
		int	strandId = strandIdArray.at(n);
		const kTentacle&	tentacle = tentacleArray0(strandId);
		if (tentacle.longitudinal.length() < 2) {
			cerr << "FATAL: kTentacle::longitudinal: no segment" << endl;
			continue;
		}
		const MVector&	longiVector0 = tentacle.longitudinal[0];
		double	strandLength = (tentacle.longitudinal.length() - 1) * longiVector0.length();
		if (__shSimulationMayaPtr->addStrand(strandIdArray.at(n), DEFAULT_SEGMENT_COUNT, strandLength) < 0) {
			cerr << "Error: kSimulationCache::addStrand(): Failure" << endl;
		}
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
		} else {
			cerr << "Warning: Unknown Simulation Status(|" << __simulationStatus << "|)" << endl;
			__simulationStatus = lastStatus;
			return true;
		}
		return false;
	}
#endif
	if (plug == worldMeshConnected) {
		__isWorldMeshConnected = handle.asBool();
			MStatus	ms;
			MDagPath	dagPathShape;
#if 1
			ms = kNode::findExternalDagPath(plug.node(), dagPathShape);
#else
			ms = findExternalDagPath(plug, dagPathShape);
#endif
			if (ms != MStatus::kSuccess) {
				cerr << "Info: setInternalValue: " << endl;
				cerr << "findExternalDagPath(|" << plug.info() << "|) NOT return MStatus::kSuccess: ";
				cerr << ms << endl;
			} else {
				cout << "External Dag Path: " << dagPathShape.fullPathName(&ms) << endl;
				__externalDagPathShape.set(dagPathShape);
#if 0
				if (__hairInterpolatorPtr) {
					MFnMesh	fnMesh(dagPathShape, &ms);
					__numPolygons = fnMesh.numPolygons(&ms);
				}
#endif
#if 0
{
				MObject	objMesh = dagPathShape.node(&ms);
				MFnMesh	fnMesh(objMesh, &ms);
				int	numPolygons = fnMesh.numPolygons(&ms);
				MItMeshPolygon	polyIter(objMesh, &ms);
				__attenuateLength_averageArea = 0;
				MDoubleArray	areaArray;
				areaArray.setLength(numPolygons);
				for (; !polyIter.isDone(); polyIter.next()) {
					double	area;
					polyIter.getArea(area);
					__attenuateLength_averageArea += area;
					areaArray[polyIter.index()] = area;
				}
				__attenuateLength_averageArea /= numPolygons;
				MItMeshVertex	vertexIter(objMesh, &ms);
				__attenuateLength_factorArray.setLength(vertexIter.count());
				for (; !vertexIter.isDone(); vertexIter.next()) {
					MIntArray	faceArray;
					ms = vertexIter.getConnectedFaces(faceArray);
					double	avgArea = 0;
					for (int i = 0; i < (int) faceArray.length(); ++i)
						avgArea += areaArray[faceArray[i]];
					avgArea /= faceArray.length();
					double	factor = sqrt(avgArea);
					if (factor > 1)
						factor = 1 / factor;
					__attenuateLength_factorArray[vertexIter.index()] = factor;
				}
}
#endif
			}
		return true;
	}
#endif
#if 0
	if (plug == beInterpolated) {
#if 10
		if (__isExternalFileReadOnly()) {
			cerr << "External File Read Only Mode: Ignored" << endl;
			return true;
		}
#endif
		__displayHair = handle.asBool();
		return true;
	}
#endif
	if (plug == mControlPoints || plug == mControlValueX || plug == mControlValueY || plug == mControlValueZ) {
#if 0
{
		cout << strIndent() << "baseShape::setInternalValue(): plug == mControlValue: hasHistory(): " << (hasHistory() ? "True" : "False") << endl;
		MStatus	ms;
		MString	name = plug.partialName(true, true, true, true, true, true, &ms);
		cout << ms << endl;
		cout << "|" << name << "|" << endl;
		MStringArray	cmds;
		MPlug&	nonConstMPlug = *(const_cast<MPlug*>(&plug));
		cout << "getSetAttrCmds" << endl;
		ms = nonConstMPlug.getSetAttrCmds(cmds, MPlug::kAll, true);
		cout << "/getSetAttrCmds" << endl;
		cout << ms << endl;
		for (unsigned n = 0; n < cmds.length(); ++n)
			cout << "cmds[" << n << "]:= |" << cmds[n] << "|" << endl;
		double3&	d3 = handle.asDouble3();
		cout << "(" << d3[0] << ", " << d3[1] << ", " << d3[2] << ")" << endl;
		unsigned	index = plug.logicalIndex(&ms);
		cout << ms << endl;
		cout << index << endl;
		{
		MPlug	parentPlug = plug.parent(&ms);
		cout << ms << endl;
		cout << "|" << parentPlug.name() << "|" << endl;
		}
}
#endif
		if (hasHistory())	{
#if 0
			MStatus	ms;
			MPlug	_plug(thisMObject(), mControlPoints);
	cout << "baseShape::setInternalValue(): _plug.isArray? " << (_plug.isArray() ? "True" : "False") << endl;
	cout << _plug.numElements(&ms) << endl;
	cout << ms << endl;
	MPlug	_plug1 = _plug.elementByLogicalIndex(0, &ms);
	cout << ms << endl;
	cout << _plug1.name(&ms) << endl;
	cout << ms << endl;
	bool	isLocked = _plug1.isLocked(&ms);
	cout << ms << endl;
	cout << "isLocked = " << (isLocked ? "True" : "False") << endl;
	ms = _plug1.setLocked(true);
	cout << ms << endl;
	isLocked = _plug1.isLocked(&ms);
	cout << ms << endl;
	cout << "isLocked = " << (isLocked ? "True" : "False") << endl;
	cout << "baseShape::setInternalValue(): _plug.isElement? " << (_plug.isElement() ? "True" : "False") << endl;
#if 0
	cout << "baseShape::setInternalValue(): _plug.isCompound? " << (_plug.isCompound() ? "True" : "False") << endl;
	cout << _plug.numChildren(&ms) << endl;
	cout << ms << endl;
	MPlug	_plug2 = _plug.child(0, &ms);
	cout << ms << endl;
	cout << _plug2.name() << endl;
	cout << "baseShape::setInternalValue(): _plug.isChild? " << (_plug.isChild() ? "True" : "False") << endl;
#endif
#else
#if 0
			MStatus	ms;
			bool	bChild = plug.isChild(&ms);
			cout << ms << endl;
			int	index = -1;
			if (bChild) {
				MPlug	parentPlug = plug.parent(&ms);
				cout << ms << endl;
				index = parentPlug.logicalIndex(&ms);
				cout << ms << endl;
			} else {
				index = plug.logicalIndex(&ms);
				cout << ms << endl;
			}
			cout << "index = " << index << endl;
			if (index <= 0)
				return true;
#endif
#endif
			verticesUpdated();
#ifdef baseShape_DEBUG
			cout << strIndent() << "MPxNode::setInternalValue()" << endl;
#endif
#if 0
			MObject	updatedMeshDataRefObj = meshDataRef();
#endif
			return MPxNode::setInternalValue(plug, handle);
		} else {
			if (plug == mControlPoints && !plug.isArray()) {
				int index = plug.logicalIndex();
				MPoint point;
				double3& ptData = handle.asDouble3();
				point.x = ptData[0];
				point.y = ptData[1];
				point.z = ptData[2];
				setValue( index, point );
			}
			else if( plug == mControlValueX ) {
				MPlug parentPlug = plug.parent();
				int index = parentPlug.logicalIndex();
				setValue( index, 0, handle.asDouble() );
			}
			else if( plug == mControlValueY ) {
				MPlug parentPlug = plug.parent();
				int index = parentPlug.logicalIndex();
				setValue( index, 1, handle.asDouble() );
			}
			else if( plug == mControlValueZ ) {
				MPlug parentPlug = plug.parent();
				int index = parentPlug.logicalIndex();
				setValue( index, 2, handle.asDouble() );
			}
		}
	} else if (plug == mHasHistoryOnCreate) {
		fHasHistoryOnCreate = handle.asBool();
	} else {
		isOk = MPxSurfaceShape::setInternalValue(plug, handle);
	}
	return isOk;
}
MStatus	baseShape::connectionMade(const MPlug& plug, const MPlug& otherPlug, bool asSrc)
{
#if	defined(baseShape_DEBUG)
	cout << "baseShape::connectionMade(): To plug: |" << plug.info() << "|" << endl;
	cout << "baseShape::connectionMade(): From plug: |" << otherPlug.info() << "|" << endl;
#endif
	if (plug == inputSurface) {
		MStatus	stat;
		MObject	thisObj = thisMObject();
		MPlug	historyPlug(thisObj, mHasHistoryOnCreate);
		stat = historyPlug.setValue(true);
		MCHECKERROR(stat, "connectionMade: setValue(mHasHistoryOnCreate)");
#if 1
#if 1
{
		MStatus	ms;
		MObject	nodeShape = plug.node(&ms); if (!ms) cerr << ms << endl;
		MFnDagNode	fnDagNodeShape(nodeShape, &ms); if (!ms) cerr << ms << endl;
		MDagPath	localDagPathShape;
		ms = fnDagNodeShape.getPath(localDagPathShape); if (!ms) cerr << ms << endl;
		MObject	nodeTransform = localDagPathShape.transform(&ms); if (!ms) cerr << ms << endl;
		MFnDagNode	fnDagNodeTransform(nodeTransform);
		MDagPath	localDagPathTransform;
		fnDagNodeTransform.getPath(localDagPathTransform);
		__localInclusiveMatrix = localDagPathTransform.inclusiveMatrix();
		__localExclusiveMatrix = localDagPathTransform.exclusiveMatrix();
#ifdef _DEBUG3
		cout << "__localWorldMatrix: " << __localWorldMatrix << endl;
#endif
}
#else
{
		MStatus	ms;
		MObject	node = plug.node(&ms); if (!ms) cerr << ms << endl;
		MFnDagNode	dagNode(node, &ms); if (!ms) cerr << ms << endl;
		MDagPath	localDagPathShape;
		ms = dagNode.getPath(localDagPathShape); if (!ms) cerr << ms << endl;
		MObject	transform = localDagPathShape.transform(&ms); if (!ms) cerr << ms << endl;
		MFnTransform	fnTransform(transform, &ms); if (!ms) cerr << ms << endl;
		MTransformationMatrix	xformMatrix = fnTransform.transformation(&ms); if (!ms) cerr << ms << endl;
		__localWorldMatrix = xformMatrix.asMatrix();
#ifdef _DEBUG3
		cout << "__localWorldMatrix: " << __localWorldMatrix << endl;
#endif
}
#endif
#endif
	}
	return MPxNode::connectionMade(plug, otherPlug, asSrc);
}
MStatus	baseShape::connectionBroken(const MPlug& plug, const MPlug& otherPlug, bool asSrc)
{
#if	defined(baseShape_DEBUG)
	cout << "baseShape::connectionBroken(): To plug: |" << plug.info() << "|" << endl;
	cout << "baseShape::connectionBroken(): From plug: |" << otherPlug.info() << "|" << endl;
#endif
	if (plug == inputSurface) {
		MStatus	stat;
		MObject	thisObj = thisMObject();
		MPlug	historyPlug(thisObj, mHasHistoryOnCreate);
		stat = historyPlug.setValue(false);
		MCHECKERROR(stat, "connectionBroken: setValue(mHasHistoryOnCreate)");
#if 4
		if (__shSimulationMayaPtr) {
			delete __shSimulationMayaPtr;
			__shSimulationMayaPtr = NULL;
		}
#endif
#if 1
		__localInclusiveMatrix = MMatrix::identity;
		__localExclusiveMatrix = MMatrix::identity;
		__externalInclusiveMatrix = MMatrix::identity;
		__externalExclusiveMatrix = MMatrix::identity;
#else
		__localWorldMatrix = MMatrix::identity;
		__externalWorldMatrix = MMatrix::identity;
#endif
	}
	return MPxNode::connectionBroken(plug, otherPlug, asSrc);
}
MStatus
baseShape::shouldSave(
	const MPlug&	plug,
	bool&	result
)
{
	++gIndentDepth;
	MStatus	ms = __shouldSave(plug, result);
	--gIndentDepth;
	return ms;
}
MStatus	baseShape::__shouldSave(const MPlug& plug, bool& result)
{
#ifdef baseShape_DEBUG
	cout << strIndent() << "baseShape::shouldSave(): " << plug.info() << endl;
#endif
	MStatus status = MS::kSuccess;
	if (plug == mControlPoints || plug == mControlValueX || plug == mControlValueY || plug == mControlValueZ) {
		if (hasHistory()) {
			status = MPxNode::shouldSave(plug, result);
		} else {
			result = false;
		}
	} else if (plug == cachedSurface) {
		if (hasHistory()) {
			result = false;
		} else {
			MObject	data;
			status = plug.getValue(data);
			MCHECKERROR(status, "shouldSave: MPlug::getValue");
			result = !data.isNull();
		}
	} else {
		status = MPxNode::shouldSave( plug, result );
	}
	return status;
}
#if 0
#endif
void
baseShape::componentToPlugs(
	MObject&	component,
	MSelectionList&	list
) const
{
	++gIndentDepth;
	__componentToPlugs(component, list);
	--gIndentDepth;
}
void
baseShape::__componentToPlugs(
	MObject&	component,
	MSelectionList&	list
) const
{
#ifdef baseShape_DEBUG
	cout << strIndent() << "baseShape::componentToPlugs()" << endl;
#endif
	MStatus	ms;
	MObject	thisNode = thisMObject();
#if 1
#if 0
	int	iSelectBy;
	baseShape*	nonConstThis = (baseShape*) this;
	nonConstThis->getSelectBy(iSelectBy);
#endif
	if (
		component.hasFn(MFn::kDoubleIndexedComponent) &&
		component.hasFn(MFn::kSurfaceCVComponent)
	) {
		MFnDoubleIndexedComponent	fnCVComp(component, &ms);
		MPlug	arrayPlug(thisNode, mControlPoints);
		int	len = fnCVComp.elementCount(&ms);
		for (int i = 0; i < len; ++i) {
			int	uIndex;
			int	vIndex;
			ms = fnCVComp.getElement(i, uIndex, vIndex);
			MPlug	cvPlug = arrayPlug.elementByLogicalIndex(N_POINTS_IN_GUIDE * uIndex + vIndex, &ms);
			ms = list.add(cvPlug);
		}
		return;
	}
	if (
		component.hasFn(MFn::kSingleIndexedComponent) &&
		component.hasFn(MFn::kMeshEdgeComponent)
	) {
		MFnSingleIndexedComponent	fnEdgeComp(component, &ms);
		MPlug	arrayPlug(thisMObject(), guide);
		unsigned	len = fnEdgeComp.elementCount(&ms);
		for (int i = 0; i < (int) len; ++i) {
			int	index = fnEdgeComp.element(i, &ms);
			MPlug	edgePlug = arrayPlug.elementByLogicalIndex(index, &ms);
			ms = list.add(edgePlug, true);
		}
		return;
	}
#else
	if (component.hasFn(MFn::kSingleIndexedComponent)) {
		MFnSingleIndexedComponent	fnVtxComp(component);
		MPlug	plug(thisMObject(), mControlPoints);
		bool	b = convertToTweakNodePlug(plug);
#ifdef baseShape_DEBUG
		cout << strIndent() << "baseShape::componentToPlugs(): convertToTweakNodePlug(): " << (b ? "True" : "False") << endl;
#endif
		int	len = fnVtxComp.elementCount();
		for (int i = 0; i < len; ++i) {
			plug.selectAncestorLogicalIndex(fnVtxComp.element(i), plug.attribute());
			list.add(plug);
		}
	}
#endif
}
MPxSurfaceShape::MatchResult
baseShape::matchComponent(
	const MSelectionList&		item,
	const MAttributeSpecArray&	spec,
	MSelectionList&			list
)
{
	++gIndentDepth;
	MPxSurfaceShape::MatchResult	r = __matchComponent(item, spec, list);
	--gIndentDepth;
	return r;
}
MPxSurfaceShape::MatchResult
baseShape::__matchComponent(
	const MSelectionList&		item,
	const MAttributeSpecArray&	spec,
	MSelectionList&			list
)
{
#ifdef baseShape_DEBUG
	cout << strIndent() << "baseShape::matchComponent()" << endl;
#endif
	MPxSurfaceShape::MatchResult result = MPxSurfaceShape::kMatchOk;
#if	0 || defined(baseShape_DEBUG2)
{
	MStatus	ms;
	unsigned	nAttrSpec = spec.length();
	cout << "# of MAttributeSpecArray = " << nAttrSpec << endl;
	for (unsigned iAttrSpec = 0; iAttrSpec < nAttrSpec; ++iAttrSpec) {
		MAttributeSpec	attrSpec = spec[iAttrSpec];
		cout << "MAttributeSpecArray[" << iAttrSpec << "]:" << endl;
		cout << "\tname:= |" << attrSpec.name() << "|" << endl;
		int	dim = attrSpec.dimensions();
		cout << "\tdimension = " << dim << endl;
		if (!dim)
			continue;
		for (int i = 0; i < dim; ++i) {
			MAttributeIndex	attrIndex = attrSpec[i];
			cout << "\tMAttributeIndexArray[" << i << "]:" << endl;
			MAttributeIndex::MIndexType	type = attrIndex.type();
			cout << "\t\ttype:= " <<
			(
				type == MAttributeIndex::kInteger ? "kInteger" :
				(
					type == MAttributeIndex::kFloat ? "kFloat" :
					"kUnknownIndexType"
				)
			) << endl;
			cout << "\t\thasRange(): " << (attrIndex.hasRange() ? "True" : "False") << endl;
			cout << "\t\thasValidRange(): " << (attrIndex.hasValidRange() ? "True" : "False") << endl;
			cout << "\t\thasLowerBound(): " << (attrIndex.hasLowerBound() ? "True" : "False") << endl;
			cout << "\t\thasUpperBound(): " << (attrIndex.hasUpperBound() ? "True" : "False") << endl;
			cout << "\t\tisBounded(): " << (attrIndex.isBounded() ? "True" : "False") << endl;
		}
	}
}
#endif
	MAttributeSpec	attrSpec = spec[0];
#if 1
	if (1 == spec.length() && attrSpec.name() == "e" && attrSpec.dimensions() == 1) {
#ifdef baseShape_DEBUG
		cout << strIndent() << "::matchComponent(): |.e[###]|" << endl;
#endif
		MStatus	ms;
		do {
			int	numVertices = meshGeom()->vertices.length();
			MAttributeIndex	attrIndex = attrSpec[0];
			if (attrIndex.type() != MAttributeIndex::kInteger) {
				result = MPxSurfaceShape::kMatchInvalidAttribute;
				break;
			}
			int	upper = 0;
			int	lower = 0;
			if (attrIndex.hasLowerBound()) {
				attrIndex.getLower(lower);
			}
			if (attrIndex.hasUpperBound()) {
				attrIndex.getUpper(upper);
			}
			if (lower > upper || upper >= numVertices) {
				cerr << strIndent() << "::matchComponent(): Invalid attribute range: [0, " << numVertices << "): " << "lower = " << lower << ", upper = " << upper << endl;
				result = MPxSurfaceShape::kMatchInvalidAttributeRange;
				break;
			}
#ifdef baseShape_DEBUG
			cout << strIndent() << "::matchComponent(): range: [0, " << numVertices << "): " << "lower = " << lower << ", upper = " << upper << endl;
#endif
			MDagPath	path;
			ms = item.getDagPath(0, path);
			if (!ms) {
				result = MPxSurfaceShape::kMatchNone;
				break;
			}
			MFnSingleIndexedComponent	fnEdgeComp;
			MObject	edgeComp = fnEdgeComp.create(MFn::kMeshEdgeComponent, &ms);
			if (!ms) {
				result = MPxSurfaceShape::kMatchNone;
				break;
			}
			bool	bOk = true;
			for (int i = lower; i <= upper; ++i) {
				ms = fnEdgeComp.addElement(i);
				if (!ms) {
					bOk = false;
					break;
				}
			}
			if (!bOk) {
				result = MPxSurfaceShape::kMatchNone;
				break;
			}
			ms = list.add(path, edgeComp);
			if (!ms) {
				result = MPxSurfaceShape::kMatchNone;
				break;
			}
			result = MPxSurfaceShape::kMatchOk;
		} while (0);
		return result;
	}
	if (1 == spec.length() && attrSpec.dimensions() == 2 && attrSpec.name() == "cv") {
#ifdef baseShape_DEBUG
		cout << strIndent() << "::matchComponent(): |.cv[###][###]|" << endl;
#endif
		MStatus	ms;
		do {
			int	numVertices = meshGeom()->vertices.length();
			MAttributeIndex	attrIndexU = attrSpec[0];
			if (attrIndexU.type() != MAttributeIndex::kInteger) {
				result = MPxSurfaceShape::kMatchInvalidAttribute;
				break;
			}
			int	upperU = 0;
			int	lowerU = 0;
			if (attrIndexU.hasLowerBound()) {
				attrIndexU.getLower(lowerU);
			}
			if (attrIndexU.hasUpperBound()) {
				attrIndexU.getUpper(upperU);
			}
			if (lowerU > upperU || upperU >= numVertices) {
				cerr << strIndent() << "::matchComponent(): Invalid attribute range(U): [0, " << numVertices << "): " << "lower = " << lowerU << ", upper = " << upperU << endl;
				result = MPxSurfaceShape::kMatchInvalidAttributeRange;
				break;
			}
#ifdef baseShape_DEBUG
			cout << strIndent() << "::matchComponent(): range(U): [0, " << numVertices << "): " << "lower = " << lowerU << ", upper = " << upperU << endl;
#endif
			MAttributeIndex	attrIndexV = attrSpec[1];
			if (attrIndexV.type() != MAttributeIndex::kInteger) {
				result = MPxSurfaceShape::kMatchInvalidAttribute;
				break;
			}
			int	upperV = 0;
			int	lowerV = 0;
			if (attrIndexV.hasLowerBound()) {
				attrIndexV.getLower(lowerV);
			}
			if (attrIndexV.hasUpperBound()) {
				attrIndexV.getUpper(upperV);
			}
			if (lowerV > upperV || upperV >= (int) N_POINTS_IN_GUIDE) {
				cerr << strIndent() << "::matchComponent(): Invalid attribute range(V): [0, " << N_POINTS_IN_GUIDE << "): " << "lower = " << lowerV << ", upper = " << upperV << endl;
				result = MPxSurfaceShape::kMatchInvalidAttributeRange;
				break;
			}
#ifdef baseShape_DEBUG
			cout << strIndent() << "::matchComponent(): range(V): [0, " << N_POINTS_IN_GUIDE << "): " << "lower = " << lowerV << ", upper = " << upperV << endl;
#endif
			MDagPath	path;
			ms = item.getDagPath(0, path);
			if (!ms) {
				result = MPxSurfaceShape::kMatchNone;
				break;
			}
			MFnDoubleIndexedComponent	fnCVComp;
			MObject	cvComp = fnCVComp.create(MFn::kSurfaceCVComponent, &ms);
			if (!ms) {
				result = MPxSurfaceShape::kMatchNone;
				break;
			}
			bool	bOk = true;
			for (int iU = lowerU; iU <= upperU; ++iU) {
				for (int iV = lowerV; iV <= upperV; ++iV) {
					ms = fnCVComp.addElement(iU, iV);
					if (!ms) {
						bOk = false;
						break;
					}
				}
				if (!bOk)
					break;
			}
			if (!bOk) {
				result = MPxSurfaceShape::kMatchNone;
				break;
			}
			ms = list.add(path, cvComp);
			if (!ms) {
				result = MPxSurfaceShape::kMatchNone;
				break;
			}
			result = MPxSurfaceShape::kMatchOk;
		} while (0);
		return result;
	}
	if (1 == spec.length() && attrSpec.dimensions() > 0 && attrSpec.name() == "xxx") {
		MStatus	ms;
		int	numVertices = 100;
		MAttributeIndex	attrIndex = attrSpec[0];
		MAttributeIndex	attrIndex2 = attrSpec[1];
		int	upper = 0;
		int	lower = 0;
		if (attrIndex.hasLowerBound()) {
			attrIndex.getLower(lower);
		}
		if (attrIndex.hasUpperBound()) {
			attrIndex.getUpper(upper);
		}
cout << "lower = " << lower << ", upper = " << upper << endl;
		if ( (lower > upper) || (upper >= numVertices) ) {
			result = MPxSurfaceShape::kMatchInvalidAttributeRange;
		} else {
			MDagPath	path;
			item.getDagPath(0, path);
cout << "|" << path.fullPathName() << "|" << endl;
#if 1
			MFnDoubleIndexedComponent	fnVtxComp;
			MObject	vtxComp = fnVtxComp.create(MFn::kSurfaceCVComponent, &ms);
cout << "MStatus: |" << ms << "|" << endl;
			int	lower2 = -1;
			ms = attrIndex2.getLower(lower2);
cout << "MStatus: |" << ms << "|" << endl;
cout << "lower2 = " << lower2 << endl;
			ms = fnVtxComp.addElement(lower, lower2);
cout << "MStatus: |" << ms << "|" << endl;
			ms = fnVtxComp.getElement(0, lower, lower2);
cout << "MStatus: |" << ms << "|" << endl;
cout << "lower1 = " << lower << ", lower2 = " << lower2 << endl;
#else
			MFnSingleIndexedComponent	fnVtxComp;
			MObject	vtxComp = fnVtxComp.create(MFn::kMeshVertComponent, &ms);
cout << "MStatus: |" << ms << "|" << endl;
			for (int i = lower; i <= upper; ++i) {
				fnVtxComp.addElement(i);
			}
#endif
cout << "# = " << list.length() << endl;
			list.add(path, vtxComp);
cout << "# = " << list.length() << endl;
for (unsigned index = 0; index < list.length(); ++index) {
	MDagPath	path;
	MObject		component;
	list.getDagPath(index, path, component);
	cout << "[" << index << "]: |" << path.fullPathName() << "|" << endl;
	cout << "[" << index << "]: |" << component.apiTypeStr() << "| (" << component.isNull() << ")" << endl;
}
		}
		return MPxSurfaceShape::kMatchOk;
	}
#else
	if (
		1 == spec.length() &&
		attrSpec.dimensions() > 0 &&
		attrSpec.name() == "vtx"
	) {
		int	numVertices = meshGeom()->vertices.length();
		MAttributeIndex	attrIndex = attrSpec[0];
		int	upper = 0;
		int	lower = 0;
		if (attrIndex.hasLowerBound()) {
			attrIndex.getLower(lower);
		}
		if (attrIndex.hasUpperBound()) {
			attrIndex.getUpper(upper);
		}
		if (lower > upper || upper >= numVertices) {
			result = MPxSurfaceShape::kMatchInvalidAttributeRange;
		} else {
#ifdef baseShape_DEBUG
			cout << "lower = " << lower << ", upper = " << upper << endl;
#endif
			MDagPath	path;
			item.getDagPath(0, path);
			MFnSingleIndexedComponent	fnVtxComp;
			MObject	vtxComp = fnVtxComp.create(MFn::kMeshVertComponent);
			for (int i = lower; i <= upper; ++i) {
				fnVtxComp.addElement(i);
			}
			list.add(path, vtxComp);
		}
		return MPxSurfaceShape::kMatchOk;
	}
#endif
#ifdef baseShape_DEBUG
	cout << strIndent() << "baseShape::matchComponent(): Pass to the parent class..." << endl;
#endif
	return MPxSurfaceShape::matchComponent(item, spec, list);
}
bool baseShape::match( const MSelectionMask & mask,
			  		 const MObjectArray& componentList ) const
{
#ifdef baseShape_DEBUG
	cout << "baseShape::match()" << endl;
#endif
	bool result = false;
	if( componentList.length() == 0 ) {
		result = mask.intersects( MSelectionMask::kSelectMeshes );
	}
	else {
		for ( int i=0; i<(int)componentList.length(); i++ ) {
			if ( (componentList[i].apiType() == MFn::kMeshVertComponent) &&
				 (mask.intersects(MSelectionMask::kSelectMeshVerts))
			) {
				result = true;
				break;
			}
		}
	}
	return result;
}
MObject	baseShape::createFullVertexGroup() const
{
#ifdef baseShape_DEBUG
	cout << "baseShape::createFullVertexGroup()" << endl;
#endif
	MFnSingleIndexedComponent	fnComponent;
	MObject	fullComponent = fnComponent.create(MFn::kMeshVertComponent);
	int	numVertices = ((baseShape*) this)->meshGeom()->vertices.length();
	fnComponent.setCompleteData(numVertices);
	return fullComponent;
}
MObject	baseShape::localShapeInAttr() const
{
#ifdef baseShape_DEBUG
	cout << "baseShape::localShapeInAttr()" << endl;
#endif
	return inputSurface;
}
MObject	baseShape::localShapeOutAttr() const
{
#ifdef baseShape_DEBUG
	cout << "baseShape::localShapeOutAttr()" << endl;
#endif
	return outputSurface;
}
MObject	baseShape::worldShapeOutAttr() const
{
	++gIndentDepth;
	MObject	o = __worldShapeOutAttr();
	--gIndentDepth;
	return o;
}
MObject	baseShape::__worldShapeOutAttr() const
{
#ifdef baseShape_DEBUG
	cout << strIndent() << "baseShape::worldShapeOutAttr()" << endl;
#endif
	return worldSurface;
}
MObject	baseShape::cachedShapeAttr() const
{
#ifdef baseShape_DEBUG
	cout << "baseShape::cachedShapeAttr()" << endl;
#endif
	return cachedSurface;
}
MObject baseShape::geometryData() const
{
#ifdef baseShape_DEBUG
	cout << "baseShape::geometryData()" << endl;
#endif
	baseShape*	nonConstThis = (baseShape*) this;
	MDataBlock	datablock = nonConstThis->forceCache();
	MDataHandle	handle = datablock.inputValue(inputSurface);
	return handle.data();
}
void	baseShape:: closestPoint(const MPoint&, MPoint& theClosestPoint, double) const
{
#ifdef baseShape_DEBUG
	cout << "baseShape::closestPoint()" << endl;
#endif
	cout << "baseShape::closestPoint(): Incomplete!" << endl;
	baseShapeGeometry*	geometryPtr = ((baseShape*) this)->meshGeom();
	int	numVertices = geometryPtr->vertices.length();
	for (int ii = 0; ii < numVertices; ++ii) {
		MPoint	tryThisOne = geometryPtr->vertices[ii];
	}
	theClosestPoint = geometryPtr->vertices[0];
}
void
baseShape::transformUsing(
	const MMatrix&	mat,
	const MObjectArray&	componentList
)
{
#ifdef baseShape_DEBUG
	cout << "baseShape::transformUsing()" << endl;
#endif
	transformUsing(mat, componentList, MPxSurfaceShape::kNoPointCaching, NULL);
}
#if 1
#endif
void
baseShape::transformUsing(
	const MMatrix&		mat,
	const MObjectArray&	componentList,
	MVertexCachingMode	cachingMode,
	MPointArray*		pointCache
)
{
	++gIndentDepth;
	__transformUsing(mat, componentList, cachingMode, pointCache);
	--gIndentDepth;
}
void
baseShape::__transformUsing(
	const MMatrix&		mat,
	const MObjectArray&	componentList,
	MVertexCachingMode	cachingMode,
	MPointArray*		pointCache
)
{
#ifdef baseShape_DEBUG
	cout << strIndent() << "baseShape::transformUsing(MPointArray*)" << endl;
#endif
	MStatus	stat;
	baseShapeGeometry*	geometryPtr = meshGeom();
	bool	savePoints = cachingMode == MPxSurfaceShape::kSavePoints;
	unsigned	i = 0,j = 0;
	unsigned	len = componentList.length();
	int	iSelectBy;
	getSelectBy(iSelectBy);
#ifdef baseShape_DEBUG
	cout << strIndent() << "baseShape::transformUsing(MObjectArray&): selectBy:= " << (iSelectBy == baseShape::selectByGuide ? "byGuide" : (iSelectBy == baseShape::selectByVertex ? "byVertex" : "byUnknown")) << endl;
#endif
#ifdef baseShape_DEBUG
	cout << strIndent() << "baseShape::transformUsing(MPointArray&): cachingMode = " << (
		cachingMode == MPxSurfaceShape::kNoPointCaching ? "kNoPointCaching" :
		cachingMode == MPxSurfaceShape::kSavePoints ? "kSavePoints" :
		cachingMode == MPxSurfaceShape::kRestorePoints ? "kRestorePoints" :
		cachingMode == MPxSurfaceShape::kUpdatePoints ? "kUpdatePoints" :
		"kUnknown"
	) << endl;
	cout << strIndent() << "baseShape::transformUsing(MPointArray&): MObjectArray.length() returns " << len << endl;
#endif
	if (cachingMode == MPxSurfaceShape::kRestorePoints) {
		unsigned int cacheLen = pointCache->length();
		if (len > 0) {
			for ( i = 0; i < len && j < cacheLen; i++ )
			{
				MObject comp = componentList[i];
				MFnSingleIndexedComponent fnComp( comp );
				int elemCount = fnComp.elementCount();
				for ( int idx=0; idx<elemCount && j < cacheLen; idx++, ++j ) {
					int elemIndex = fnComp.element( idx );
					geometryPtr->vertices[elemIndex] = (*pointCache)[j];
				}
			}
		} else {
			len = geometryPtr->vertices.length();
			for ( unsigned int idx = 0; idx < len && j < cacheLen; ++idx, ++j ) {
				geometryPtr->vertices[idx] = (*pointCache)[j];
			}
		}
	} else {
		if (len > 0) {
		if (iSelectBy == selectByGuide) {
			for (i = 0; i < len; ++i) {
				MObject	comp = componentList[i];
				MFnSingleIndexedComponent	fnComp(comp, &stat);
				cout << "===============================: " << stat << endl;
				int	elemCount = fnComp.elementCount(&stat);
				if (savePoints && 0 == i) {
					pointCache->setSizeIncrement(elemCount);
				}
				for (int idx = 0; idx < elemCount; ++idx) {
					int	elemIndex = fnComp.element(idx, &stat);
#if 1
#ifdef baseShape_DEBUG
					cout << "index = " << elemIndex << endl;
#endif
					if (elemIndex == 0)
						continue;
#endif
					if (savePoints) {
						pointCache->append(geometryPtr->vertices[elemIndex]);
					}
					geometryPtr->vertices[elemIndex] *= mat;
					geometryPtr->normals[idx] = geometryPtr->normals[idx].transformAsNormal(mat);
				}
			}
		} else if (iSelectBy == selectByVertex) {
			for (i = 0; i < len; ++i) {
				MObject	comp = componentList[i];
				MFnDoubleIndexedComponent	fnComp(comp, &stat);
				cout << "===============================: " << stat << endl;
				int	elemCount = fnComp.elementCount(&stat);
				if (savePoints && 0 == i) {
					pointCache->setSizeIncrement(elemCount);
				}
				for (int idx = 0; idx < elemCount; ++idx) {
					int	elemIndex;
					int&	uIndex = elemIndex;
					int	vIndex;
					stat = fnComp.getElement(idx, uIndex, vIndex);
#if 1
#ifdef baseShape_DEBUG
					cout << "(uIndex, vIndex):= (" << uIndex << ", " << vIndex << ")" << endl;
#endif
					if (vIndex == 0)
						continue;
#endif
					if (savePoints) {
						pointCache->append(geometryPtr->vertices[elemIndex]);
					}
					geometryPtr->vertices[elemIndex] *= mat;
					geometryPtr->normals[idx] = geometryPtr->normals[idx].transformAsNormal(mat);
				}
			}
		}
		} else {
			len = geometryPtr->vertices.length();
			if (savePoints) {
				pointCache->setSizeIncrement(len);
			}
			for ( unsigned int idx = 0; idx < len; ++idx ) {
				if (savePoints) {
					pointCache->append(geometryPtr->vertices[idx]);
				}
				geometryPtr->vertices[idx] *= mat;
				geometryPtr->normals[idx] =
					geometryPtr->normals[idx].transformAsNormal( mat );
			}
		}
	}
#if 1
	MDataBlock	datablock = forceCache();
#if 0
	MDataHandle	cachedHandle = datablock.outputValue(cachedSurface, &stat);
	MCHECKERRORNORET(stat, "transformAsNormal error getting cachedSurface")
	baseShapeData*	cached = (baseShapeData*) cachedHandle.asPluginData();
#endif
	MDataHandle	dHandle = datablock.outputValue(mControlPoints, &stat);
	MCHECKERRORNORET(stat, "transformUsing get dHandle")
#else
	if (hasHistory() && NULL != cached) {
#if 1
#else
		stat = buildControlPoints(datablock, geometryPtr->vertices.length());
		MCHECKERRORNORET(stat, "transformUsing buildControlPoints")
#endif
		MArrayDataHandle	cpHandle(dHandle, &stat);
		MCHECKERRORNORET(stat, "transformUsing get cpHandle")
		for (i = 0; i < len; ++i) {
			MObject	comp = componentList[i];
			MFnSingleIndexedComponent	fnComp(comp);
			int	elemCount = fnComp.elementCount();
			for (int idx = 0; idx < elemCount; ++idx) {
				int	elemIndex = fnComp.element(idx);
				cpHandle.jumpToElement(elemIndex);
				MDataHandle	pntHandle = cpHandle.outputValue();
				double3&	pnt = pntHandle.asDouble3();
				MPoint	oldPnt = cached->fGeometry->vertices[elemIndex];
				MPoint	newPnt = geometryPtr->vertices[elemIndex];
				MPoint	offset = newPnt - oldPnt;
				pnt[0] += offset[0];
				pnt[1] += offset[1];
				pnt[2] += offset[2];
			}
		}
	}
	if ( NULL == cached ) {
		cerr << "NULL cachedSurface data found\n";
	} else {
		*(cached->fGeometry) = *geometryPtr;
	}
#endif
	MPlug	pCPs(thisMObject(), mControlPoints);
	pCPs.setValue(dHandle);
	computeBoundingBox(datablock);
	childChanged(MPxSurfaceShape::kBoundingBoxChanged);
}
void
baseShape::tweakUsing( const MMatrix & mat,
					 const MObjectArray & componentList,
					 MVertexCachingMode cachingMode,
					 MPointArray* pointCache,
					 MArrayDataHandle& handle )
{
#ifdef baseShape_DEBUG
	cout << "baseShape::tweakUsing()" << endl;
#endif
	baseShapeGeometry* geometryPtr = meshGeom();
	bool savePoints    = (cachingMode == MPxSurfaceShape::kSavePoints);
	bool updatePoints  = (cachingMode == MPxSurfaceShape::kUpdatePoints);
	MArrayDataBuilder builder = handle.builder();
	MPoint delta, currPt, newPt;
	unsigned int i=0;
	unsigned int len = componentList.length();
	unsigned int cacheIndex = 0;
	unsigned int cacheLen = (NULL != pointCache) ? pointCache->length() : 0;
	if (cachingMode == MPxSurfaceShape::kRestorePoints) {
		if (len > 0) {
			for ( i=0; i<len; i++ )
			{
				MObject comp = componentList[i];
				MFnSingleIndexedComponent fnComp( comp );
				int elemCount = fnComp.elementCount();
				for ( int idx=0; idx<elemCount && cacheIndex < cacheLen; idx++, cacheIndex++) {
					int elemIndex = fnComp.element( idx );
					double3 & pt = builder.addElement( elemIndex ).asDouble3();
					MPoint& cachePt = (*pointCache)[cacheIndex];
					pt[0] += cachePt.x;
					pt[1] += cachePt.y;
					pt[2] += cachePt.z;
				}
			}
		} else {
			len = geometryPtr->vertices.length();
			for ( unsigned int idx = 0; idx < len && idx < cacheLen; ++idx ) {
				double3 & pt = builder.addElement( idx ).asDouble3();
				MPoint& cachePt = (*pointCache)[cacheIndex];
				pt[0] += cachePt.x;
				pt[1] += cachePt.y;
				pt[2] += cachePt.z;
			}
		}
	} else {
		if (len > 0) {
			for ( i=0; i<len; i++ )
			{
				MObject comp = componentList[i];
				MFnSingleIndexedComponent fnComp( comp );
				int elemCount = fnComp.elementCount();
				if (savePoints) {
					pointCache->setSizeIncrement(elemCount);
				}
				for ( int idx=0; idx<elemCount; idx++ )
				{
					int elemIndex = fnComp.element( idx );
					double3 & pt = builder.addElement( elemIndex ).asDouble3();
					currPt = newPt = geometryPtr->vertices[elemIndex];
					newPt *= mat;
					delta.x = newPt.x - currPt.x;
					delta.y = newPt.y - currPt.y;
					delta.z = newPt.z - currPt.z;
					pt[0] += delta.x;
					pt[1] += delta.y;
					pt[2] += delta.z;
					if (savePoints) {
						pointCache->append(delta*(-1.0));
					} else if (updatePoints && cacheIndex < cacheLen) {
						MPoint& cachePt = (*pointCache)[cacheIndex];
						cachePt[0] -= delta.x;
						cachePt[1] -= delta.y;
						cachePt[2] -= delta.z;
						cacheIndex++;
					}
				}
			}
		} else {
			len = geometryPtr->vertices.length();
			if (savePoints) {
				pointCache->setSizeIncrement(len);
			}
			for ( unsigned int idx = 0; idx < len; ++idx ) {
				double3 & pt = builder.addElement( idx ).asDouble3();
				currPt = newPt = geometryPtr->vertices[idx];
				newPt *= mat;
				delta.x = newPt.x - currPt.x;
				delta.y = newPt.y - currPt.y;
				delta.z = newPt.z - currPt.z;
				pt[0] += delta.x;
				pt[1] += delta.y;
				pt[2] += delta.z;
				if (savePoints) {
					pointCache->append(delta*-1.0);
				} else if (updatePoints && idx < cacheLen) {
					MPoint& cachePt = (*pointCache)[idx];
					cachePt[0] -= delta.x;
					cachePt[1] -= delta.y;
					cachePt[2] -= delta.z;
				}
			}
		}
	}
	handle.set(builder);
	childChanged( MPxSurfaceShape::kBoundingBoxChanged );
}
bool baseShape::vertexOffsetDirection( MObject & component,
									 MVectorArray & direction,
									 MVertexOffsetMode mode,
									 bool normalize )
{
#ifdef baseShape_DEBUG
	cout << "baseShape::vertexOffsetDirection()" << endl;
#endif
	MStatus stat;
	bool offsetOkay = false ;
	MFnSingleIndexedComponent fnComp( component, &stat );
	if ( !stat || (component.apiType() != MFn::kMeshVertComponent) ) {
		return false;
	}
	offsetOkay = true ;
	baseShapeGeometry * geometryPtr = meshGeom();
	if ( NULL == geometryPtr ) {
		return false;
	}
	int count = fnComp.elementCount();
	for ( int idx=0; idx<count; idx++ )
	{
		MVector normal = geometryPtr->normals[ fnComp.element(idx) ];
		if( mode == MPxSurfaceShape::kNormal ) {
			if( normalize ) normal.normalize() ;
			direction.append( normal );
		}
		else {
			MVector uAxis, vAxis ;
			int    i, j, k;
			double a;
			normal.normalize();
			i = 0;  a = fabs( normal[0] );
			if ( a < fabs(normal[1]) ) { i = 1; a = fabs(normal[1]); }
			if ( a < fabs(normal[2]) ) i = 2;
			j = (i+1)%3;  k = (j+1)%3;
			a = sqrt(normal[i]*normal[i] + normal[j]*normal[j]);
			uAxis[i] = -normal[j]/a; uAxis[j] = normal[i]/a; uAxis[k] = 0.0;
			vAxis = normal^uAxis;
			if ( mode == MPxSurfaceShape::kUTangent ||
				 mode == MPxSurfaceShape::kUVNTriad )
			{
				if( normalize ) uAxis.normalize() ;
				direction.append( uAxis );
			}
			if ( mode == MPxSurfaceShape::kVTangent ||
				 mode == MPxSurfaceShape::kUVNTriad )
			{
				if( normalize ) vAxis.normalize() ;
				direction.append( vAxis );
			}
			if ( mode == MPxSurfaceShape::kUVNTriad ) {
				if( normalize ) normal.normalize() ;
				direction.append( normal );
			}
		}
	}
	return offsetOkay ;
}
bool	baseShape::isBounded() const
{
#ifdef baseShape_DEBUG2
	cout << "baseShape::isBounded()" << endl;
#endif
	return true;
}
MBoundingBox	baseShape::boundingBox() const
{
#ifdef baseShape_DEBUG2
	cout << "baseShape::boundingBox()" << endl;
#endif
    MObject thisNode = thisMObject();
    MPlug   c1Plug( thisNode, bboxCorner1 );
    MPlug   c2Plug( thisNode, bboxCorner2 );
    MObject corner1Object;
    MObject corner2Object;
    c1Plug.getValue( corner1Object );
    c2Plug.getValue( corner2Object );
    double3 corner1, corner2;
    MFnNumericData fnData;
    fnData.setObject( corner1Object );
    fnData.getData( corner1[0], corner1[1], corner1[2] );
    fnData.setObject( corner2Object );
    fnData.getData( corner2[0], corner2[1], corner2[2] );
    MPoint corner1Point( corner1[0], corner1[1], corner1[2] );
    MPoint corner2Point( corner2[0], corner2[1], corner2[2] );
    return MBoundingBox( corner1Point, corner2Point );
}
#pragma warning(disable: 4100)
MPxGeometryIterator*
baseShape::geometryIteratorSetup(
	MObjectArray&	componentList,
	MObject&	components,
	bool		forReadOnly
)
{
	++gIndentDepth;
	MPxGeometryIterator*	p = __geometryIteratorSetup(componentList, components, forReadOnly);
	--gIndentDepth;
	return p;
}
MPxGeometryIterator*
baseShape::__geometryIteratorSetup(
	MObjectArray&	componentList,
	MObject&	components,
	bool
)
{
	return NULL;
#if 0
#ifdef baseShape_DEBUG
	cout << strIndent() << "baseShape::geometryIteratorSetup()" << endl;
#endif
	baseShapeGeometryIterator*	result = NULL;
#if 1
	if (components.isNull()) {
		result = new baseShapeGeometryIterator(this, componentList);
	} else {
		result = new baseShapeGeometryIterator(this, components);
	}
#else
	if (components.isNull()) {
		result = new baseShapeGeometryIterator(meshGeom(), componentList);
	} else {
		result = new baseShapeGeometryIterator(meshGeom(), components);
	}
#endif
	return result;
#endif
}
bool	baseShape::acceptsGeometryIterator(bool)
{
#ifdef baseShape_DEBUG
	cout << "baseShape::acceptsGeometryIterator()" << endl;
#endif
	return false;
#if 0
	return true;
#endif
}
bool	baseShape::acceptsGeometryIterator(MObject&, bool, bool)
{
#ifdef baseShape_DEBUG
	cout << "baseShape::acceptsGeometryIterator(bool forReadOnly)" << endl;
#endif
	return false;
#if 0
	return true;
#endif
}
#pragma warning(default: 4100)
bool baseShape::hasHistory()
{
	return fHasHistoryOnCreate;
}
#if 1
MStatus	baseShape::computeBoundingBox(MDataBlock&)
{
	return MStatus::kSuccess;
}
#else
MStatus	baseShape::computeBoundingBox(MDataBlock& datablock)
{
	++gIndentDepth;
	MStatus	ms = __computeBoundingBox(datablock);
	--gIndentDepth;
	return ms;
}
#endif
#if 1
MStatus	baseShape::__computeBoundingBox(MDataBlock&)
{
	return MStatus::kSuccess;
}
#else
MStatus	baseShape::__computeBoundingBox(MDataBlock& datablock)
{
#ifdef baseShape_DEBUG
	cout << strIndent() << "baseShape::computeBoundingBox()" << endl;
#endif
	MStatus	stat = MStatus::kSuccess;
	MDataHandle	lowerHandle = datablock.outputValue(bboxCorner1);
	MDataHandle	upperHandle = datablock.outputValue(bboxCorner2);
	double3&	lower = lowerHandle.asDouble3();
	double3&	upper = upperHandle.asDouble3();
	baseShapeGeometry*	geometryPtr = meshGeom();
	int	cnt = geometryPtr->vertices.length();
	if (cnt == 0)
		return stat;
#if 1
	MVectorArray*	cpPtr = getControlPoints();
	MVectorArray&	cp = *cpPtr;
	lower[0] = DBL_MAX;
	lower[1] = DBL_MAX;
	lower[2] = -DBL_MAX;
	upper[0] = -DBL_MAX;
	upper[1] = -DBL_MAX;
	upper[2] = DBL_MAX;
#if 1
	MMatrix		m = __externalInclusiveMatrix;
#else
	MMatrix		m = __externalWorldMatrix;
#endif
	for (int index = 0; index < cnt; ++index) {
		MPoint	basePoint = geometryPtr->vertices[index];
#if 3
		basePoint *= m;
#endif
		MPoint	point = basePoint;
		for (int k = 0; k < (int) N_POINTS_IN_GUIDE; ++k) {
			MVector	offset = cp[N_POINTS_IN_GUIDE * index + k];
#if 3
			offset = offset.transformAsNormal(m);
#endif
			point += offset;
			if (point[0] < lower[0]) lower[0] = point[0];
			if (point[1] < lower[1]) lower[1] = point[1];
			if (point[2] > lower[2]) lower[2] = point[2];
			if (point[0] > upper[0]) upper[0] = point[0];
			if (point[1] > upper[1]) upper[1] = point[1];
			if (point[2] < upper[2]) upper[2] = point[2];
		}
	}
#else
	MPoint	tmppnt = geometryPtr->vertices[0];
	lower[0] = tmppnt[0]; lower[1] = tmppnt[1]; lower[2] = tmppnt[2];
	upper[0] = tmppnt[0]; upper[1] = tmppnt[1]; upper[2] = tmppnt[2];
	for (int i = 0; i < cnt; ++i) {
		MPoint pnt = geometryPtr->vertices[i];
		if ( pnt[0] < lower[0] ) lower[0] = pnt[0];
		if ( pnt[1] < lower[1] ) lower[1] = pnt[1];
		if ( pnt[2] > lower[2] ) lower[2] = pnt[2];
		if ( pnt[0] > upper[0] ) upper[0] = pnt[0];
		if ( pnt[1] > upper[1] ) upper[1] = pnt[1];
		if ( pnt[2] < upper[2] ) upper[2] = pnt[2];
	}
#endif
	lowerHandle.setClean();
	upperHandle.setClean();
	childChanged(MPxSurfaceShape::kBoundingBoxChanged);
	return stat;
}
#endif
MStatus
baseShape::computeInputSurface(
	const MPlug&	,
	MDataBlock&	datablock
)
{
	MStatus	stat = MS::kSuccess;
	if (hasHistory()) {
		MDataHandle	inputHandle = datablock.inputValue(inputSurface, &stat);
		MCHECKERROR(stat, "computeInputSurface error getting inputSurface")
		baseShapeData*	surf = (baseShapeData*) inputHandle.asPluginData();
		if (NULL == surf) {
			cerr << "NULL inputSurface data found\n";
			return stat;
		}
		baseShapeGeometry*	geometryPtr = surf->fGeometry;
		MFnPluginData	fnDataCreator;
		MTypeId	tmpid(baseShapeData::id);
#ifdef baseShapeData_DEBUG
cout << "<MFnPluginData::create(): fnDataCreator.data()>" << endl;
#endif
		MObject	newCachedObject = fnDataCreator.create(tmpid, &stat);
		MCHECKERROR(stat, "compute : error creating Cached baseShapeData")
#ifdef baseShapeData_DEBUG
cout << "</MFnPluginData::create(): fnDataCreator.data()>" << endl;
#endif
		baseShapeData*	newCachedData = (baseShapeData*) fnDataCreator.data(&stat);
		MCHECKERROR(stat, " error gettin proxy cached baseShapeData object")
		*(newCachedData->fGeometry) = *geometryPtr;
		MDataHandle	cachedHandle = datablock.outputValue(cachedSurface,&stat);
		MCHECKERROR(stat, "computeInputSurface error getting cachedSurface")
#ifdef baseShapeData_DEBUG
cout << "<MDataHandle::set(): fnDataCreator.data()>" << endl;
#endif
		cachedHandle.set(newCachedData);
#ifdef baseShapeData_DEBUG
cout << "</MDataHandle::set(): fnDataCreator.data()>" << endl;
#endif
	}
	return stat;
}
MStatus	baseShape::computeOutputSurface(const MPlug& plug, MDataBlock& datablock)
{
	MStatus stat;
#if 2
	{
		MStatus	ms;
		MDataHandle	inputData = datablock.inputValue(time, &ms);
		if (ms) {
		} else {
			cerr << "time: Failure: " << ms << endl;
		}
	}
#endif
	if (!computeInputSurface(plug, datablock)) {
		return MS::kFailure;
	}
	MDataHandle	cachedHandle = datablock.outputValue(cachedSurface, &stat);
	MCHECKERROR(stat, "computeOutputSurface error getting cachedSurface")
	baseShapeData* cached = (baseShapeData*) cachedHandle.asPluginData();
	if (NULL == cached) {
		cerr << "NULL cachedSurface data found\n";
	}
	datablock.setClean(plug);
	if (hasHistory()) {
		MStatus	ms;
#if 0
		MObject	creatorObj;
		MPlug	plug_any(thisMObject(), inputSurface);
		ms = findCreatorObj(plug_any.node(), creatorObj);
		MFnDependencyNode	creatorDepNode(creatorObj, &ms);
		MPlug	plug_creator_externalWorldMatrix = creatorDepNode.findPlug("externalWorldMatrix", true, &ms);
		cout << ms << endl;
		MObject	matrixObj;
		ms = plug_creator_externalWorldMatrix.getValue(matrixObj);
		MFnMatrixData	matrixData(matrixObj, &ms);
		cout << matrixData.matrix(&ms) << " (" << ms << ")" << endl;
#endif
		MDagPath	dagPathShape;
		ms = kNode::findExternalDagPath(plug.node(), dagPathShape);
		__externalDagPathShape.set(dagPathShape);
#if 1
		if (__externalDagPathShape.isValid()) {
			MStatus	ms;
			MObject	transform = __externalDagPathShape.transform(&ms); if (!ms) cerr << ms << endl;
#if 1
			MFnDagNode	fnDagNodeTransform(transform);
			MDagPath	__externalDagPathTransform;
			fnDagNodeTransform.getPath(__externalDagPathTransform);
			__externalInclusiveMatrix = __externalDagPathTransform.inclusiveMatrix();
			__externalExclusiveMatrix = __externalDagPathTransform.exclusiveMatrix();
#ifdef _DEBUG3
			cout << "__externalInclusiveMatrix: " << __externalInclusiveMatrix << endl;
			cout << "__externalExclusiveMatrix: " << __externalExclusiveMatrix << endl;
#endif
#if 0
			MFnTransform	fnTransform(transform, &ms); if (!ms) cerr << ms << endl;
			MTransformationMatrix	xformMatrix = fnTransform.transformation(&ms); if (!ms) cerr << ms << endl;
			MMatrix	m = xformMatrix.asMatrix();
			cout << "m: " << m << endl;
			cout << "inclusiveMatrix: " << __externalInclusiveMatrix << endl;
			cout << "m * exclusiveMatrix: " << m * __externalExclusiveMatrix << endl;
			cout << "exclusiveMatrix * m: " << __externalExclusiveMatrix * m << endl;
#endif
#else
			MFnTransform	fnTransform(transform, &ms); if (!ms) cerr << ms << endl;
			MTransformationMatrix	xformMatrix = fnTransform.transformation(&ms); if (!ms) cerr << ms << endl;
			__externalWorldMatrix = xformMatrix.asMatrix();
#ifdef _DEBUG3
			cout << "__externalWorldMatrix: " << __externalWorldMatrix << endl;
#endif
#endif
		}
#if 0
#if 9
	if (!__call_generateHair_only) {
#endif
		if (!buildControlPoints(datablock, *(cached->fGeometry))) {
			return MStatus::kFailure;
		}
#if 9
	} else {
		__call_generateHair_only = false;
	}
#endif
#else
		if (!buildControlPoints(datablock, *(cached->fGeometry))) {
			return MStatus::kFailure;
		}
#endif
#if 2
#if 0
if (__shSimulationMayaPtr) {
	cout << "ShSimulation: Status:= " << (__simulationStatus == Simulation::On ? "On" : (__simulationStatus == Simulation::Paused ? "Paused" : (__simulationStatus == Simulation::ReadOnly ? "ReadOnly" : "Unknown"))) << endl;
if (__simulationStatus == Simulation::On || __simulationStatus == Simulation::ReadOnly) {
#if 0
	assert(__shSimulationMayaPtr->isValid(false));
#ifdef _DEBUG3
	cout << "Body: ";
	cout << "local MDagPath? " << (__shSimulationMayaPtr->isValid(false) ? "True" : "False") << endl;
	cout << "world MDagPath? " << (__shSimulationMayaPtr->isValid(true) ? "True" : "False") << endl;
#endif
#endif
	MStatus	ms;
#ifdef _INHERIT_kBODY
	kBodyMayaTentacle*	bodyMayaTentaclePtr = (kBodyMayaTentacle*) __shSimulationMayaPtr;
#else
	kBodyMayaTentacle*	bodyMayaTentaclePtr = (kBodyMayaTentacle*) __shSimulationMayaPtr->bodyMayaPtr();
#endif
	if (!bodyMayaTentaclePtr || !bodyMayaTentaclePtr->isValid()) {
		cerr << "Simulation: Warning: No External Node" << endl;
	} else {
#if 1
#else
		__shSimulationMayaPtr->setMeshGeom(cached->fGeometry);
		__shSimulationMayaPtr->setCPPtr(getControlPoints());
#endif
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
		(void) __shSimulationMayaPtr->setSimulationCache(nthFrame);
	MArrayDataHandle	cpH = datablock.outputArrayValue(mControlPoints, &stat);
	MArrayDataBuilder	oldBuilder = cpH.builder();
	MArrayDataBuilder	builder(oldBuilder);
	baseShapeGeometry&	geometry = *(cached->fGeometry);
	const kSimulationCache::kStrandArray&	strandArray = __shSimulationMayaPtr->strandArray();
	for (int indexStrand = 0; indexStrand < (int) strandArray.length(); ++indexStrand) {
		unsigned	numSamplePoints = N_POINTS_IN_GUIDE;
		ShVector3	samplePoints[N_POINTS_IN_GUIDE];
		if (__shSimulationMayaPtr->getResult(indexStrand, nthFrame, numSamplePoints, samplePoints) < 0) {
			cerr << "Error" << endl;
			continue;
		}
		const kSimulationCache::kStrand&	strand = strandArray(indexStrand);
		int	strandId = strand.strandId;
#if 10
		if (__numExternalStrands > 0) {
			int	index = N_POINTS_IN_GUIDE * strandId;
			for (int k = 0; k < (int) N_POINTS_IN_GUIDE; ++k) {
				double3&	n = builder.addElement(index + k).asDouble3();
				n[0] = samplePoints[k][0];
				n[1] = samplePoints[k][1];
				n[2] = samplePoints[k][2];
			}
			continue;
		}
#endif
		double	len0 = __segmentLengthInGuide.at(strandId);
		MPoint	basePoint = geometry.vertices[strandId];
#if 1
		basePoint *= __externalInclusiveMatrix;
		MMatrix	inv_m = __externalInclusiveMatrix.inverse();
#else
		basePoint *= __externalWorldMatrix;
		MMatrix	inv_m = __externalWorldMatrix.inverse();
#endif
		MPoint	point = basePoint;
		int	index = N_POINTS_IN_GUIDE * strandId;
		for (int k = 1; k < (int) N_POINTS_IN_GUIDE; ++k) {
			double3&	n = builder.addElement(index + k).asDouble3();
			MPoint	nextPoint(samplePoints[k][0], samplePoints[k][1], samplePoints[k][2]);
			MVector	offset(nextPoint - point);
#if 1
			offset = len0 * offset.transformAsNormal(inv_m);
#else
			offset = len * offset.transformAsNormal(inv_m);
#endif
			n[0] = offset.x;
			n[1] = offset.y;
			n[2] = offset.z;
			point = nextPoint;
		}
	}
	cpH.set(builder);
	cpH.setAllClean();
	}
}
}
#endif
#endif
#if 0
if (__hairInterpolatorPtr) {
	HairInterpolator&	hairInterpolator = *__hairInterpolatorPtr;
	cout << "HairInterpolation: " << (__displayHair ? "True" : "False") << endl;
if (!__externalDagPathShape.isValid()) {
	cerr << "HairInterpolation: Warning: No External Node" << endl;
} else {
#define ALL_HAIRS
if (__displayHair) {
	MStatus	ms;
	MTime	time = MAnimControl::currentTime();
	int	indexFrame = (int) time.value() - 1;
	cout << "Interpolation: indexFrame = " << indexFrame << endl;
#ifdef ALL_HAIRS
	baseShapeGeometry&	geometry = *(cached->fGeometry);
#endif
	HairInterpolator::HairRef	hairRef(
		indexFrame,
#ifdef _DEBUG9
		&geometry,
#endif
		&__externalDagPathShape,
		&__segmentLengthInGuide,
		getControlPoints()
	);
#ifdef _DEBUG9
	hairInterpolator.print(hairRef);
#endif
#ifdef ALL_HAIRS
	HairInterpolator::HairArrayPtr	p0 = NULL;
	for (int polyId = 0; polyId < geometry.faceCount; ++polyId) {
		int	iReturnVal = hairInterpolator.generateHair(polyId, hairRef);
		if (iReturnVal < 0) {
			cerr << "Error: HairInterpolator::generateHair(polyId = " << polyId << "): Failure" << endl;
			continue;
		}
		const HairInterpolator::HairArrayPtr	p = (HairInterpolator::HairArrayPtr) &hairInterpolator.getHairArray();
		if (p0 && p != p0) {
			assert(p == p0);
			cerr << "Fatal: p(= 0x" << p << ") != p0(= 0x" << p0 << ")" << endl;
		}
#else
		int	iReturnVal = hairInterpolator.generateHair(3, hairRef);
		if (iReturnVal < 0) {
			cerr << "Error: HairInterpolator::generateHair(polyId = 3): Failure" << endl;
		}
#endif
#ifdef _DEBUG9
		cout << "HairInterpolator::generateHair() returns with iReturnVal = " << iReturnVal << ": ";
		cout << "__hairArrayPtr = 0x" << &hairInterpolator.getHairArray() << endl;
#endif
#ifdef ALL_HAIRS
	}
#endif
}
#ifdef ALL_HAIRS
#undef ALL_HAIRS
#endif
}
}
#endif
#else
		applyTweaks(datablock, cached->fGeometry);
#endif
	} else {
		MArrayDataHandle	cpHandle = datablock.inputArrayValue(mControlPoints, &stat);
		cpHandle.setAllClean();
	}
	MFnPluginData	fnDataCreator;
	MTypeId	tmpid(baseShapeData::id);
#ifdef baseShapeData_DEBUG
cout << "<MFnPluginData::create(): fnDataCreator.data()>" << endl;
#endif
	MObject	newDataObject = fnDataCreator.create(tmpid, &stat);
	MCHECKERROR(stat, "compute : error creating baseShapeData")
#ifdef baseShapeData_DEBUG
cout << "</MFnPluginData::create(): fnDataCreator.data()>" << endl;
#endif
	baseShapeData*	newData = (baseShapeData*) fnDataCreator.data(&stat);
	MCHECKERROR(stat, "compute : error gettin at proxy baseShapeData object")
	if (NULL != cached) {
		*(newData->fGeometry) = *(cached->fGeometry);
	} else {
		cerr << "computeOutputSurface: NULL cachedSurface data\n";
	}
	MDataHandle	outHandle = datablock.outputValue(outputSurface);
#ifdef baseShapeData_DEBUG
cout << "<MDataHandle::set(): fnDataCreator.data()>" << endl;
#endif
	outHandle.set(newData);
#ifdef baseShapeData_DEBUG
cout << "</MDataHandle::set(): fnDataCreator.data()>" << endl;
#endif
	stat = computeBoundingBox(datablock);
	MCHECKERROR(stat, "computeBoundingBox")
	return stat;
}
MStatus baseShape::computeWorldSurface( const MPlug& plug, MDataBlock& datablock )
{
	MStatus stat;
	computeOutputSurface(plug, datablock);
	MDataHandle inHandle = datablock.outputValue( outputSurface );
	baseShapeData* outSurf = (baseShapeData*)inHandle.asPluginData();
	if ( NULL == outSurf ) {
		cerr << "computeWorldSurface: outSurf NULL\n";
		return MS::kFailure;
	}
	MFnPluginData fnDataCreator;
	MTypeId tmpid( baseShapeData::id );
#ifdef baseShapeData_DEBUG
cout << "<MFnPluginData::create(): fnDataCreator.data()>" << endl;
#endif
	MObject newDataObject = fnDataCreator.create( tmpid, &stat );
#ifdef baseShapeData_DEBUG
cout << "</MFnPluginData::create(): fnDataCreator.data()>" << endl;
#endif
	MCHECKERROR( stat, "compute : error creating baseShapeData")
	baseShapeData * newData = (baseShapeData*)fnDataCreator.data( &stat );
	MCHECKERROR( stat, "compute : error gettin at proxy baseShapeData object")
	MMatrix worldMat = getWorldMatrix(datablock, 0);
	newData->setMatrix( worldMat );
	*(newData->fGeometry) = *(outSurf->fGeometry);
	int arrayIndex = plug.logicalIndex( &stat );
	MCHECKERROR( stat, "computWorldSurface : logicalIndex" );
	MArrayDataHandle worldHandle = datablock.outputArrayValue( worldSurface,
															   &stat );
 	MCHECKERROR( stat, "computWorldSurface : outputArrayValue" );
	MArrayDataBuilder builder = worldHandle.builder( &stat );
 	MCHECKERROR( stat, "computWorldSurface : builder" );
	MDataHandle outHandle = builder.addElement( arrayIndex, &stat );
	MCHECKERROR( stat, "computWorldSurface : addElement" );
#ifdef baseShapeData_DEBUG
cout << "<MDataHandle::set(): fnDataCreator.data()>" << endl;
#endif
	outHandle.set( newData );
#ifdef baseShapeData_DEBUG
cout << "</MDataHandle::set(): fnDataCreator.data()>" << endl;
#endif
	return stat;
}
#if 1
#else
MStatus	baseShape::applyTweaks(MDataBlock& datablock, baseShapeGeometry* geometryPtr)
{
	MStatus	stat;
	MArrayDataHandle	cpHandle = datablock.inputArrayValue(mControlPoints, &stat);
	MCHECKERROR(stat, "applyTweaks get cpHandle")
	int	elemCount = cpHandle.elementCount();
	for (int idx = 0; idx < elemCount; ++idx) {
		int	elemIndex = cpHandle.elementIndex();
		MDataHandle	pntHandle = cpHandle.outputValue();
		double3&	pnt = pntHandle.asDouble3();
		MPoint	offset(pnt[0], pnt[1], pnt[2]);
		MPoint&	oldPnt = geometryPtr->vertices[elemIndex];
		oldPnt = oldPnt + offset;
		cpHandle.next();
	}
	return stat;
}
#endif
bool
baseShape::value(
	int	pntInd,
	int	vlInd,
	double&	val
) const
{
	bool	result = false;
	baseShape*	nonConstThis = (baseShape*) this;
	baseShapeGeometry*	geometryPtr = nonConstThis->cachedGeom();
	if (NULL != geometryPtr) {
		MPoint	point = geometryPtr->vertices[pntInd];
		val = point[vlInd];
		result = true;
	}
	return result;
}
bool
baseShape::value(
	int	pntInd,
	MPoint&	val
) const
{
	bool	result = false;
	baseShape*	nonConstThis = (baseShape*) this;
	baseShapeGeometry*	geometryPtr = nonConstThis->cachedGeom();
	if (NULL != geometryPtr) {
		MPoint	point = geometryPtr->vertices[pntInd];
		val = point;
		result = true;
	}
	return result;
}
bool
baseShape::setValue(
	int	pntInd,
	int	vlInd,
	double	val
)
{
	bool	result = false;
	baseShape*	nonConstThis = (baseShape*) this;
	baseShapeGeometry*	geometryPtr = nonConstThis->cachedGeom();
	if (NULL != geometryPtr) {
		MPoint&	point = geometryPtr->vertices[pntInd];
		point[vlInd] = val;
		result = true;
	}
	verticesUpdated();
	return result;
}
bool
baseShape::setValue(
	int	pntInd,
	const MPoint&	val
)
{
	bool	result = false;
	baseShape*	nonConstThis = (baseShape*) this;
	baseShapeGeometry*	geometryPtr = nonConstThis->cachedGeom();
	if (NULL != geometryPtr) {
		geometryPtr->vertices[pntInd] = val;
		result = true;
	}
	verticesUpdated();
	return result;
}
#if 1
namespace {
	static int	__i = 0;
}
#endif
#if 1
#endif
MObject
baseShape::meshDataRef()
{
	MDataBlock	datablock = forceCache();
#ifdef baseShapeData_DEBUG2
	int	i = ::__i++;
cout << "<fnDataCreator.data()>: ::__i = " << i << endl;
#endif
#ifdef baseShape_DEBUG2
	cout << "baseShape::meshDataRef(): IN: ::__i = " << i << endl;
#endif
	MDataHandle	handle = datablock.inputValue(outputSurface);
#ifdef baseShape_DEBUG2
	cout << "baseShape::meshDataRef(): OUT: ::__i = " << i << endl;
#endif
#ifdef baseShapeData_DEBUG2
cout << "</fnDataCreator.data()>: ::__i = " << i << endl;
#endif
	return handle.data();
}
baseShapeGeometry*
baseShape::meshGeom()
{
#ifdef baseShape_DEBUG2
	cout << "baseShape::meshGeom()" << endl;
#endif
	MStatus		stat;
	baseShapeGeometry*	result = NULL;
	MObject		tmpObj = meshDataRef();
	MFnPluginData	fnData(tmpObj);
	baseShapeData*	data = (baseShapeData*) fnData.data(&stat);
	MCHECKERRORNORET(stat, "meshGeom : Failed to get baseShapeData");
	if (NULL != data) {
		result = data->fGeometry;
	}
	return result;
}
MObject baseShape::cachedDataRef()
{
	MDataBlock	datablock = forceCache();
	MDataHandle	handle = datablock.outputValue(cachedSurface);
	return handle.data();
}
baseShapeGeometry*
baseShape::cachedGeom()
{
	MStatus		stat;
	baseShapeGeometry*	result = NULL;
	MObject		tmpObj = cachedDataRef();
	MFnPluginData	fnData(tmpObj);
	baseShapeData*	data = (baseShapeData*) fnData.data(&stat);
	MCHECKERRORNORET(stat, "cachedGeom : Failed to get baseShapeData");
	if (NULL != data) {
		result = data->fGeometry;
	}
	return result;
}
#if 1
MStatus	baseShape::getSelectBy(int& iSelectBy)
{
	iSelectBy = selectByUnknown;
	MStatus	ms;
	MDataBlock	datablock = forceCache();
	MDataHandle	dHandle = datablock.inputValue(selectBy, &ms);
	MCHECKERROR(ms, "MDataBlock::inputValue(selectBy)");
	iSelectBy = (int) dHandle.asShort();
	return MStatus::kSuccess;
}
#endif
#if 1
#if 1
MStatus	baseShape::buildControlPoints(MDataBlock&, baseShapeGeometry&)
{
	return MStatus::kSuccess;
}
#else
MStatus	baseShape::buildControlPoints(MDataBlock& datablock, baseShapeGeometry& geometry)
{
	++gIndentDepth;
	MStatus	ms = __buildControlPoints(datablock, geometry);
	--gIndentDepth;
	return ms;
}
#endif
#if 1
MStatus	baseShape::__buildControlPoints(MDataBlock&, baseShapeGeometry&)
{
	return MStatus::kSuccess;
}
#else
MStatus	baseShape::__buildControlPoints(MDataBlock& datablock, baseShapeGeometry& geometry)
{
#ifdef baseShape_DEBUG
	cout << strIndent() << "baseShape::buildControlPoints()" << endl;
#endif
	MStatus	stat;
	MArrayDataHandle	cpH = datablock.outputArrayValue(mControlPoints, &stat);
	MCHECKERROR(stat, "compute get cpH")
	MArrayDataBuilder	oldBuilder = cpH.builder();
	MPointArray&	vertexArray = geometry.vertices;
	MVectorArray&	normalArray = geometry.normals;
	unsigned	vertexCount = vertexArray.length();
#ifdef baseShape_DEBUG
	cout << strIndent() << "baseShape::buildControlPoints(): count = " << N_POINTS_IN_GUIDE * vertexCount << ", oldBuilder.elementCount() = " << oldBuilder.elementCount() << endl;
#endif
#if 1
	if (__segmentLengthInGuide.size() == 0) {
		for (int i = 0; i < (int) vertexCount; ++i) {
			__segmentLengthInGuide.push_back(SEGMENT_LENGTH_DEFAULT_IN_GUIDE);
		}
	}
#endif
#if 1
#if 0
	MArrayDataHandle	transDataHandle = datablock.outputArrayValue(transversal, &stat);
	MCHECKERROR(stat, "MDataBlock::outputArrayValue(transversal)")
	MArrayDataBuilder	transDataBuilder(&datablock, transversal, N_POINTS_IN_GUIDE * vertexCount, &stat);
	MCHECKERROR(stat, "MArrayDataBuilder::MArrayDataBuilder(MDataBlock*, transversal)")
#endif
#endif
	if (N_POINTS_IN_GUIDE * vertexCount != oldBuilder.elementCount()) {
		MArrayDataBuilder	builder(oldBuilder);
		MCHECKERROR(stat, "compute - create builder")
		for (int vtx = 0; vtx < (int) vertexCount; ++vtx) {
#if 1
			MVector	normal = __segmentLengthInGuide.at(vtx) * normalArray[vtx];
			int	index = N_POINTS_IN_GUIDE * vtx;
			double3&	n = builder.addElement(index).asDouble3();
#if 1
			n[0] = vertexArray[vtx].x;
			n[1] = vertexArray[vtx].y;
			n[2] = vertexArray[vtx].z;
#else
			n[0] = n[1] = n[2] = 0;
#endif
#if 0
#if 0
			MVector	transVector(geometry.wakes[vtx * WAKE_POINTS] - vertexArray[vtx]);
			transVector.normalize();
			for (int k = 0; k < (int) N_POINTS_IN_GUIDE; ++k) {
				double3&	trans = transDataBuilder.addElement(index + k).asDouble3();
#if 1
				const double	angle = 30;
				MQuaternion	q(M_PI / 180 * angle * k, normal);
				MVector	v = transVector.rotateBy(q);
				trans[0] = v.x;
				trans[1] = v.y;
				trans[2] = v.z;
#else
				trans[0] = transVector.x;
				trans[1] = transVector.y;
				trans[2] = transVector.z;
#endif
			}
#endif
#endif
			for (int k = 1; k < (int) N_POINTS_IN_GUIDE; ++k) {
				double3&	n = builder.addElement(index + k).asDouble3();
				n[0] = normal.x;
				n[1] = normal.y;
				n[2] = normal.z;
			}
#else
			 builder.addElement(vtx).asDouble3();
#endif
		}
		cpH.set(builder);
#if 0
#if 0
		transDataHandle.set(transDataBuilder);
#endif
#endif
	}
	cpH.setAllClean();
	return stat;
}
#endif
#else
MStatus
baseShape::buildControlPoints(
	MDataBlock&	datablock,
	int	count
)
{
#ifdef baseShape_DEBUG
	cout << "baseShape::buildControlPoints()" << endl;
#endif
	MStatus	stat;
	MArrayDataHandle	cpH = datablock.outputArrayValue(mControlPoints, &stat);
	MCHECKERROR(stat, "compute get cpH")
	MArrayDataBuilder	oldBuilder = cpH.builder();
#ifdef baseShape_DEBUG
	cout << "count = " << count << ", oldBuilder.elementCount() = " << oldBuilder.elementCount() << endl;
#endif
	if (count != (int) oldBuilder.elementCount()) {
		MArrayDataBuilder	builder(oldBuilder);
		MCHECKERROR(stat, "compute - create builder")
		for (int vtx = 0; vtx < count; ++vtx) {
#if 1
			double3&	pt = builder.addElement(vtx).asDouble3();
#else
			 builder.addElement(vtx).asDouble3();
#endif
#ifdef baseShape_DEBUG
			pt[0] = 0.1 + vtx;
			pt[1] = 0.2 + vtx;
			pt[2] = 0.3 + vtx;
#endif
		}
		cpH.set(builder);
	}
	cpH.setAllClean();
	return stat;
}
#endif
void
baseShape::verticesUpdated(
	void
)
{
	childChanged(MPxSurfaceShape::kBoundingBoxChanged);
	childChanged(MPxSurfaceShape::kObjectChanged);
}
void*	baseShape::creator()
{
	return new baseShape();
}
MStatus	baseShape::initialize()
{
	MStatus				stat;
    MFnTypedAttribute	typedAttr;
    inputSurface = typedAttr.create( "inputSurface", "is",
									  baseShapeData::id,
									  MObject::kNullObj, &stat );
    MCHECKERROR( stat, "create inputSurface attribute" )
	typedAttr.setStorable( false );
    ADD_ATTRIBUTE( inputSurface );
	MAKE_NUMERIC_ATTR(	bboxCorner1, "bboxCorner1", "bb1",
						MFnNumericData::k3Double, 0,
						false, false, false );
	MAKE_NUMERIC_ATTR(	bboxCorner2, "bboxCorner2", "bb2",
						MFnNumericData::k3Double, 0,
						false, false, false );
    outputSurface = typedAttr.create( "outputSurface", "os",
									  baseShapeData::id,
									  MObject::kNullObj, &stat );
    MCHECKERROR( stat, "create outputSurface attribute" )
    ADD_ATTRIBUTE( outputSurface );
	typedAttr.setWritable( false );
    worldSurface = typedAttr.create( "worldSurface", "ws",
									  baseShapeData::id,
									  MObject::kNullObj, &stat );
    MCHECKERROR( stat, "create worldSurface attribute" );
	typedAttr.setCached( false );
	typedAttr.setWritable( false );
	stat = typedAttr.setArray( true );
    MCHECKERROR( stat, "set array" );
 	stat = typedAttr.setUsesArrayDataBuilder( true );
    MCHECKERROR( stat, "set uses array data builder" );
	stat = typedAttr.setDisconnectBehavior( MFnAttribute::kDelete );
    MCHECKERROR( stat, "set disconnect behavior data builder" );
 	stat = typedAttr.setWorldSpace( true );
    MCHECKERROR( stat, "set world space" );
    ADD_ATTRIBUTE( worldSurface );
    cachedSurface = typedAttr.create( "cachedSurface", "cs",
									  baseShapeData::id,
									  MObject::kNullObj, &stat );
    MCHECKERROR( stat, "create cachedSurface attribute" )
	typedAttr.setReadable( true );
	typedAttr.setWritable( true );
	typedAttr.setStorable( true );
    ADD_ATTRIBUTE( cachedSurface );
#if 1
{
	MStatus	ms;
	MFnEnumAttribute	enumAttr;
	selectBy = enumAttr.create("selectBy", "sb", selectByGuide, &ms);
	ms = enumAttr.addField("byGuide", selectByGuide);
	ms = enumAttr.addField("byVertex", selectByVertex);
	ms = enumAttr.setInternal(true);
	ms = addAttribute(selectBy);
	MFnNumericAttribute	guideAttr;
	MFnNumericAttribute	fnNumAttrX;
	MObject	guideX = fnNumAttrX.create("guideX", "gX", MFnNumericData::kDouble, 0, &ms);
	MFnNumericAttribute	fnNumAttrY;
	MObject	guideY = fnNumAttrY.create("guideY", "gY", MFnNumericData::kDouble, 0, &ms);
	MFnNumericAttribute	fnNumAttrZ;
	MObject	guideZ = fnNumAttrZ.create("guideZ", "gZ", MFnNumericData::kDouble, 0, &ms);
	guide = guideAttr.create("guide", "g", guideX, guideY, guideZ, &ms);
	ms = guideAttr.setInternal(true);
	ms = guideAttr.setArray(true);
	ms = addAttribute(guide);
#if 0
#if 1
	MFnEnumAttribute	rampWakes_attr;
	__rampWakes = rampWakes_attr.create("rampWakes", "rw", RampWakes::None, &ms);
	ms = rampWakes_attr.addField("None", RampWakes::None);
	ms = rampWakes_attr.addField("MajorRadius", RampWakes::MajorRadius);
	ms = rampWakes_attr.addField("Eccentricity", RampWakes::Eccentricity);
	ms = rampWakes_attr.addField("Rotation", RampWakes::Rotation);
	ms = addAttribute(__rampWakes);
	ms = attributeAffects(__rampWakes, __tentacleArray);
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
	MFnNumericAttribute	tentacleCut_have_attr;
	tentacleCut_val_children[TENTACLE_CUT_VAL_HAVE] = tentacleCut_have_attr.create("tentacleCut_have", "tCUT_have", MFnNumericData::kBoolean, 0, &ms);
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
	MFnNumericAttribute	tentacle_array_root_attr;
	tentacleArray_children[TENTACLE_ARRAY_ROOT] = tentacle_array_root_attr.create("tentacleArray_root", "tA_r", MFnNumericData::k3Double, 0, &ms);
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
	MFnNumericAttribute	tentacle_array_cut_have_attr;
	tentacleArray_cut_children[TENTACLE_CUT_VAL_HAVE] = tentacle_array_cut_have_attr.create("tentacleArray_cut_have", "tA_cH", MFnNumericData::kBoolean, 0, &ms);
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
	MFnEnumAttribute	tentacleDoActionAttr;
	__tentacleDoAction = tentacleDoActionAttr.create("tentacleDoAction", "tDA", TentacleDo::None, &ms);
	ms = tentacleDoActionAttr.addField("Remove", TentacleDo::Remove);
	ms = tentacleDoActionAttr.addField("None", TentacleDo::None);
	ms = tentacleDoActionAttr.addField("Add", TentacleDo::Add);
	ms = tentacleDoActionAttr.addField("Update", TentacleDo::Update);
	ms = addAttribute(__tentacleDoAction);
#if 1
	MObjectArray	tentacleRoot_children(NUM_TENTACLE_ROOT_CHILDREN);
	MFnNumericAttribute	tentaclePolyIdAttr;
	tentacleRoot_children[TENTACLE_ROOT_POLY_ID] = tentaclePolyIdAttr.create("tentacleRoot_polyId", "tR_ID", MFnNumericData::kInt, 0, &ms);
	MFnNumericAttribute	tentaclePositionAttr;
	tentacleRoot_children[TENTACLE_ROOT_POSITION] = tentaclePositionAttr.create("tentacleRoot_position", "tR_P", MFnNumericData::k3Double, 0, &ms);
	MFnNumericAttribute	tentacleNormalAttr;
	tentacleRoot_children[TENTACLE_ROOT_NORMAL] = tentacleNormalAttr.create("tentacleRoot_normal", "tR_N", MFnNumericData::k3Double, 0, &ms);
	MFnCompoundAttribute	tentacleRootAttr;
	__tentacleRoot = tentacleRootAttr.create("tentacleRoot", "tR", &ms);
	for (int i = 0; i < NUM_TENTACLE_ROOT_CHILDREN; ++i)
		ms = tentacleRootAttr.addChild(tentacleRoot_children[i]);
	ms = addAttribute(__tentacleRoot);
#endif
#endif
#endif
#endif
	MFnNumericAttribute	numPointsInGuideAttr;
	numPointsInGuide = numPointsInGuideAttr.create("numPointsInGuide", "npg", MFnNumericData::kInt, N_POINTS_IN_GUIDE, &ms);
	ms = numPointsInGuideAttr.setInternal(true);
	ms = addAttribute(numPointsInGuide);
	MFnNumericAttribute	seglenAttr;
	seglen = seglenAttr.create("seglen", "sl", MFnNumericData::kDouble, SEGMENT_LENGTH_DEFAULT_IN_GUIDE, &ms);
	ms = seglenAttr.setInternal(true);
	ms = seglenAttr.setArray(true);
	ms = addAttribute(seglen);
#if 0
	MFnNumericAttribute	attenuateLengthAttr;
	attenuateLength = attenuateLengthAttr.create("attenuateLength", "al", MFnNumericData::kBoolean, 0., &ms);
	ms = attenuateLengthAttr.setInternal(true);
	ms = attenuateLengthAttr.setArray(true);
	ms = addAttribute(attenuateLength);
#endif
#if 0
	MFnNumericAttribute	numPointsInHairAttr;
	numPointsInHair = numPointsInHairAttr.create("numPointsInHair", "nph", MFnNumericData::kInt, N_POINTS_IN_HAIR, &ms);
	ms = numPointsInHairAttr.setInternal(true);
	ms = addAttribute(numPointsInHair);
	MFnNumericAttribute	segmentLengthInHairAttr;
	segmentLengthInHair = segmentLengthInHairAttr.create("segmentLengthInHair", "slh", MFnNumericData::kDouble, SEGMENT_LENGTH_DEFAULT_IN_HAIR, &ms);
	ms = segmentLengthInHairAttr.setInternal(true);
	ms = addAttribute(segmentLengthInHair);
#endif
#if 0
	cout << "guide: isReadable? " << (guideAttr.isReadable() ? "True" : "False") << endl;
	cout << "guide: isWritable? " << (guideAttr.isWritable() ? "True" : "False") << endl;
	cout << "guide: isConnectable? " << (guideAttr.isConnectable() ? "True" : "False") << endl;
	cout << "guide: isStorable? " << (guideAttr.isStorable() ? "True" : "False") << endl;
	cout << "guide: isCached? " << (guideAttr.isCached() ? "True" : "False") << endl;
	cout << "guide: isArray? " << (guideAttr.isArray() ? "True" : "False") << endl;
	cout << "guide: indexMatters? " << (guideAttr.indexMatters() ? "True" : "False") << endl;
	cout << "guide: isKeyable? " << (guideAttr.isKeyable() ? "True" : "False") << endl;
	cout << "guide: isChannelBoxFlagSet? " << (guideAttr.isChannelBoxFlagSet() ? "True" : "False") << endl;
	cout << "guide: isHidden? " << (guideAttr.isHidden() ? "True" : "False") << endl;
	cout << "guide: isUsedAsColor? " << (guideAttr.isUsedAsColor() ? "True" : "False") << endl;
	cout << "guide: isIndeterminant? " << (guideAttr.isIndeterminant() ? "True" : "False") << endl;
	cout << "guide: isRenderSource? " << (guideAttr.isRenderSource() ? "True" : "False") << endl;
	cout << "guide: isDynamic? " << (guideAttr.isDynamic() ? "True" : "False") << endl;
	cout << "guide: isWorldSpace? " << (guideAttr.isWorldSpace() ? "True" : "False") << endl;
	cout << "guide: isAffectsWorldSpace? " << (guideAttr.isAffectsWorldSpace() ? "True" : "False") << endl;
	cout << "guide: usesArrayDataBuilder? " << (guideAttr.usesArrayDataBuilder() ? "True" : "False") << endl;
	cout << "guide: internal? " << (guideAttr.internal() ? "True" : "False") << endl;
#endif
}
#endif
{
	MStatus	ms;
	MFnNumericAttribute	timeAttr;
	time = timeAttr.create("time", "t", MFnNumericData::kFloat, 0.);
 	timeAttr.setStorable(true);
	ms = addAttribute(time);
	MFnNumericAttribute	wakesAttr;
	wakes = wakesAttr.create("wakes", "wk", MFnNumericData::kBoolean, 0.);
	ms = addAttribute(wakes);
}
#if 2
{
	MStatus	ms;
#if 4
	MFnNumericAttribute	worldMeshConnectedAttr;
	worldMeshConnected = worldMeshConnectedAttr.create("worldMeshConnected", "wmc", MFnNumericData::kBoolean, 0.);
 	ms = worldMeshConnectedAttr.setInternal(true);
	ms = addAttribute(worldMeshConnected);
	MFnEnumAttribute	SimulationStatusAttr;
	SimulationStatus = SimulationStatusAttr.create("SimulationStatus", "ss", Simulation::Off, &ms);
	ms = SimulationStatusAttr.addField("Off", Simulation::Off);
	ms = SimulationStatusAttr.addField("Paused", Simulation::Paused);
	ms = SimulationStatusAttr.addField("On", Simulation::On);
	ms = SimulationStatusAttr.addField("ReadOnly", Simulation::ReadOnly);
 	ms = SimulationStatusAttr.setInternal(true);
	ms = addAttribute(SimulationStatus);
#if 6
{
	MFnTypedAttribute	fileCacheReadAttr;
	MFnStringData	fnStringData;
	MObject		string = fnStringData.create("C:/fileCache", &ms);
	FileCacheRead = fileCacheReadAttr.create("FileCacheRead", "fcR", MFnData::kString, string, &ms);
	fileCacheReadAttr.setInternal(true);
	ms = addAttribute(FileCacheRead);
	MFnTypedAttribute	fileCacheWriteAttr;
	string = fnStringData.create("C:/fileCache", &ms);
	FileCacheWrite = fileCacheWriteAttr.create("FileCacheWrite", "fcW", MFnData::kString, string, &ms);
	fileCacheWriteAttr.setInternal(true);
	ms = addAttribute(FileCacheWrite);
	MFnTypedAttribute	strandIdListAttr;
	string = fnStringData.create("0-7", &ms);
	StrandIdList = strandIdListAttr.create("StrandIdList", "id", MFnData::kString, string, &ms);
	strandIdListAttr.setInternal(true);
	ms = addAttribute(StrandIdList);
}
#endif
#if 7
	MFnNumericAttribute	DynamicsAttr;
	Dynamics = DynamicsAttr.create("Dynamics", "dyn", MFnNumericData::kBoolean, 1);
	ms = DynamicsAttr.setInternal(true);
	ms = addAttribute(Dynamics);
{
	MFnEnumAttribute	RaceTypeAttr;
	__ShSimulationParameter.RaceType = RaceTypeAttr.create("RaceType", "enumRT", RACE_ASIAN, &ms);
	ms = RaceTypeAttr.addField("Asian", RACE_ASIAN);
	ms = RaceTypeAttr.addField("Caucasian1", RACE_CAUCASIAN1);
	ms = RaceTypeAttr.addField("Caucasian2", RACE_CAUCASIAN2);
	ms = RaceTypeAttr.addField("African", RACE_AFRICAN);
	ms = RaceTypeAttr.setInternal(true);
	ms = addAttribute(__ShSimulationParameter.RaceType);
	MFnNumericAttribute	IncludeStiffnessAttr;
	__ShSimulationParameter.IncludeStiffness = IncludeStiffnessAttr.create("IncludeStiffness", "spIncS", MFnNumericData::kBoolean, 1);
	ms = IncludeStiffnessAttr.setInternal(true);
	ms = addAttribute(__ShSimulationParameter.IncludeStiffness);
	MFnNumericAttribute	YoungModulusAttr;
	__ShSimulationParameter.YoungModulus = YoungModulusAttr.create("YoungModulus", "spYM", MFnNumericData::kDouble, 1.0, &ms);
	ms = YoungModulusAttr.setInternal(true);
	ms = addAttribute(__ShSimulationParameter.YoungModulus);
	MFnNumericAttribute	IncludeGravityAndInertiaAttr;
	__ShSimulationParameter.IncludeGravityAndInertia = IncludeGravityAndInertiaAttr.create("IncludeGravityAndInertia", "spIncGI", MFnNumericData::kBoolean, 1);
	ms = IncludeGravityAndInertiaAttr.setInternal(true);
	ms = addAttribute(__ShSimulationParameter.IncludeGravityAndInertia);
	MFnNumericAttribute	GravityParameterAttr;
	__ShSimulationParameter.GravityParameter = GravityParameterAttr.create("GravityParameter", "spG", MFnNumericData::kInt, 1, &ms);
	ms = GravityParameterAttr.setInternal(true);
	ms = addAttribute(__ShSimulationParameter.GravityParameter);
	MFnNumericAttribute	IncludeAirFrictionAttr;
	__ShSimulationParameter.IncludeAirFriction = IncludeAirFrictionAttr.create("IncludeAirFriction", "spIncAF", MFnNumericData::kBoolean, 1);
	ms = IncludeAirFrictionAttr.setInternal(true);
	ms = addAttribute(__ShSimulationParameter.IncludeAirFriction);
	MFnNumericAttribute	AirFrictionParameterAttr;
	__ShSimulationParameter.AirFrictionParameter = AirFrictionParameterAttr.create("AirFrictionParameter", "spAF", MFnNumericData::kInt, 10, &ms);
	ms = AirFrictionParameterAttr.setInternal(true);
	ms = addAttribute(__ShSimulationParameter.AirFrictionParameter);
	MFnEnumAttribute	DissipationModeAttr;
	__ShSimulationParameter.DissipationMode = DissipationModeAttr.create("DissipationMode", "enumDM", LAMBDA, &ms);
	ms = DissipationModeAttr.addField("NODISSIPATE", NODISSIPATE);
	ms = DissipationModeAttr.addField("LAMBDA", LAMBDA);
	ms = DissipationModeAttr.addField("GAMMA", GAMMA);
	ms = DissipationModeAttr.setInternal(true);
	ms = addAttribute(__ShSimulationParameter.DissipationMode);
	MFnNumericAttribute	DissipationLambdaParameterAttr;
	__ShSimulationParameter.DissipationLambdaParameter = DissipationLambdaParameterAttr.create("DissipationLambdaParameter", "spDLP", MFnNumericData::kDouble, 0.30, &ms);
	ms = DissipationLambdaParameterAttr.setInternal(true);
	ms = addAttribute(__ShSimulationParameter.DissipationLambdaParameter);
	MFnNumericAttribute	DissipationGammaParameterAttr;
	__ShSimulationParameter.DissipationGammaParameter = DissipationGammaParameterAttr.create("DissipationGammaParameter", "spDGP", MFnNumericData::kDouble, 0.10, &ms);
	ms = DissipationGammaParameterAttr.setInternal(true);
	ms = addAttribute(__ShSimulationParameter.DissipationGammaParameter);
	MFnEnumAttribute	WindModeAttr;
	__ShSimulationParameter.WindMode = WindModeAttr.create("WindMode", "enumWM", NOWIND, &ms);
	ms = WindModeAttr.addField("NOWIND", NOWIND);
#if 1
	ms = WindModeAttr.addField("UNIFORMWIND", UNIFORMWIND);
	ms = WindModeAttr.addField("PERIODICWIND", PERIODICWIND);
#else
	ms = WindModeAttr.addField("UNIFORM", UNIFORM);
#endif
	ms = WindModeAttr.addField("PTNDEPENDENT", PTNDEPENDENT);
	ms = WindModeAttr.addField("PTNDEPENDENTVELOCITY", PTNDEPENDENTVELOCITY);
	ms = WindModeAttr.setInternal(true);
	ms = addAttribute(__ShSimulationParameter.WindMode);
	MFnNumericAttribute	WindRatioAttr;
	__ShSimulationParameter.WindRatio = WindRatioAttr.create("WindRatio", "spWR", MFnNumericData::kInt, 1, &ms);
	ms = WindRatioAttr.setInternal(true);
	ms = addAttribute(__ShSimulationParameter.WindRatio);
	MFnNumericAttribute	WindDirection2DAttr;
	__ShSimulationParameter.WindDirection2D = WindDirection2DAttr.create("WindDirection2D", "spWD2D", MFnNumericData::kInt, 90, &ms);
	ms = WindDirection2DAttr.setInternal(true);
	ms = addAttribute(__ShSimulationParameter.WindDirection2D);
	MFnNumericAttribute	WindDirection3DAttr;
	__ShSimulationParameter.WindDirection3D = WindDirection3DAttr.create("WindDirection3D", "spWD3D", MFnNumericData::k2Int, 90, &ms);
	ms = WindDirection3DAttr.setInternal(true);
	ms = addAttribute(__ShSimulationParameter.WindDirection3D);
	MFnNumericAttribute	IncludeHairBodyOptimalCollisionAttr;
	__ShSimulationParameter.IncludeHairBodyOptimalCollision = IncludeHairBodyOptimalCollisionAttr.create("IncludeTentacleBodyOptimalCollision", "spIncHBOC", MFnNumericData::kBoolean, 0);
	ms = IncludeHairBodyOptimalCollisionAttr.setInternal(true);
	ms = addAttribute(__ShSimulationParameter.IncludeHairBodyOptimalCollision);
	MFnNumericAttribute	IncludeHairBodyCollisionAttr;
	__ShSimulationParameter.IncludeHairBodyCollision = IncludeHairBodyCollisionAttr.create("IncludeTentacleBodyCollision", "spIncHBC", MFnNumericData::kBoolean, 0);
	ms = IncludeHairBodyCollisionAttr.setInternal(true);
	ms = addAttribute(__ShSimulationParameter.IncludeHairBodyCollision);
	MFnNumericAttribute	HBCParameterAttr;
	__ShSimulationParameter.HBCParameter = HBCParameterAttr.create("TBCParameter", "spHBC", MFnNumericData::k2Double, 0.30, &ms);
	ms = HBCParameterAttr.setInternal(true);
	ms = addAttribute(__ShSimulationParameter.HBCParameter);
	MFnNumericAttribute	IncludeHairHairCollisionAttr;
	__ShSimulationParameter.IncludeHairHairCollision = IncludeHairHairCollisionAttr.create("IncludeTentacleTentacleCollision", "spIncHHC", MFnNumericData::kBoolean, 0);
	ms = IncludeHairHairCollisionAttr.setInternal(true);
	ms = addAttribute(__ShSimulationParameter.IncludeHairHairCollision);
	MFnNumericAttribute	HHCParameterAttr;
	__ShSimulationParameter.HHCParameter = HHCParameterAttr.create("TTCParameter", "spHHC", MFnNumericData::kDouble, 0.30, &ms);
	ms = HHCParameterAttr.setInternal(true);
	ms = addAttribute(__ShSimulationParameter.HHCParameter);
#if 1
#if 0
#endif
	MFnNumericAttribute	fAverageRadiusAttr;
	__ShSimulationParameter.fAverageRadius = fAverageRadiusAttr.create("fAverageRadius", "spAR", MFnNumericData::kDouble, 50.0, &ms);
	ms = fAverageRadiusAttr.setInternal(true);
	ms = addAttribute(__ShSimulationParameter.fAverageRadius);
	MFnNumericAttribute	fEllipticityAttr;
	__ShSimulationParameter.fEllipticity = fEllipticityAttr.create("fEllipticity", "spE", MFnNumericData::kDouble, 1.0, &ms);
	ms = fEllipticityAttr.setInternal(true);
	ms = addAttribute(__ShSimulationParameter.fEllipticity);
	MFnNumericAttribute	fVolumeDensityAttr;
	__ShSimulationParameter.fVolumeDensity = fVolumeDensityAttr.create("fVolumeDensity", "spVD", MFnNumericData::kDouble, 1.3, &ms);
	ms = fVolumeDensityAttr.setInternal(true);
	ms = addAttribute(__ShSimulationParameter.fVolumeDensity);
	MFnNumericAttribute	fHelixK0STepAttr;
	__ShSimulationParameter.fHelixK0STep = fHelixK0STepAttr.create("fHelixK0STep", "spK0", MFnNumericData::kDouble, 0.0, &ms);
	ms = fHelixK0STepAttr.setInternal(true);
	ms = addAttribute(__ShSimulationParameter.fHelixK0STep);
	MFnNumericAttribute	fHelixK1CurvatureAttr;
	__ShSimulationParameter.fHelixK1Curvature = fHelixK1CurvatureAttr.create("fHelixK1Curvature", "spK1", MFnNumericData::kDouble, 0.0, &ms);
	ms = fHelixK1CurvatureAttr.setInternal(true);
	ms = addAttribute(__ShSimulationParameter.fHelixK1Curvature);
	MFnNumericAttribute	inHelixK0STepAttr;
	__ShSimulationParameter.inHelixK0STep = inHelixK0STepAttr.create("inHelixK0STep", "spK0in", MFnNumericData::kDouble, 0.0, &ms);
	ms = inHelixK0STepAttr.setInternal(true);
	ms = addAttribute(__ShSimulationParameter.inHelixK0STep);
	MFnNumericAttribute	inHelixK1CurvatureAttr;
	__ShSimulationParameter.inHelixK1Curvature = inHelixK1CurvatureAttr.create("inHelixK1Curvature", "spK1in", MFnNumericData::kDouble, 0.0, &ms);
	ms = inHelixK1CurvatureAttr.setInternal(true);
	ms = addAttribute(__ShSimulationParameter.inHelixK1Curvature);
	MFnNumericAttribute	fLengthFactorAttr;
	__ShSimulationParameter.fLengthFactor = fLengthFactorAttr.create("fLengthFactor", "spLF", MFnNumericData::kDouble, 1.0, &ms);
	ms = fLengthFactorAttr.setInternal(true);
	ms = addAttribute(__ShSimulationParameter.fLengthFactor);
#endif
	MFnNumericAttribute	InverseTimeStepAttr;
	__ShSimulationParameter.InverseTimeStep = InverseTimeStepAttr.create("InverseTimeStep", "spITS", MFnNumericData::kInt, 240, &ms);
	ms = InverseTimeStepAttr.setInternal(true);
	ms = addAttribute(__ShSimulationParameter.InverseTimeStep);
	__ShSimulationParameter.fTolerance = DEFAULT_TOLERANCE;
	MFnTypedAttribute	ParameterFileRead;
	MFnStringData	fnStringData;
	MObject		string = fnStringData.create("", &ms);
	__ShSimulationParameter.ParameterFileRead = ParameterFileRead.create("ParameterFileRead", "pfR", MFnData::kString, string, &ms);
	ParameterFileRead.setInternal(true);
	ms = addAttribute(__ShSimulationParameter.ParameterFileRead);
	MFnTypedAttribute	ParameterFileWrite;
	string = fnStringData.create("", &ms);
	__ShSimulationParameter.ParameterFileWrite = ParameterFileWrite.create("ParameterFileWrite", "pfW", MFnData::kString, string, &ms);
	ParameterFileWrite.setInternal(true);
	ms = addAttribute(__ShSimulationParameter.ParameterFileWrite);
}
#endif
#endif
#if 10
{
	MFnNumericAttribute	externalFileReadOnlyAttr;
	__externalFileReadOnly = externalFileReadOnlyAttr.create("externalFileReadOnly", "efro", MFnNumericData::kBoolean, 1);
 	ms = externalFileReadOnlyAttr.setInternal(true);
	ms = addAttribute(__externalFileReadOnly);
}
#endif
#if 11
{
	MFnNumericAttribute	shiftFrameAttr;
	__shiftFrame = shiftFrameAttr.create("shiftFrame", "sf", MFnNumericData::kInt, 0, &ms);
	ms = shiftFrameAttr.setInternal(true);
	ms = addAttribute(__shiftFrame);
}
#endif
}
#endif
#if 0
{
	MStatus	ms;
	MFnNumericAttribute	beInterpolatedAttr;
	beInterpolated = beInterpolatedAttr.create("beInterpolated", "bin", MFnNumericData::kBoolean, 0.);
 	ms = beInterpolatedAttr.setInternal(true);
	ms = addAttribute(beInterpolated);
#if 1
	MFnNumericAttribute	hairControlPointsAttr;
	hairControlPoints = hairControlPointsAttr.create("hairControlPoints", "hcp", MFnNumericData::k3Double, 0, &ms);
	ms = hairControlPointsAttr.setInternal(true);
	ms = hairControlPointsAttr.setArray(true);
	ms = addAttribute(hairControlPoints);
	MFnNumericAttribute	numHairsAttr;
	numHairs = numHairsAttr.create("numHairs", "nhair", MFnNumericData::kInt, -1, &ms);
	ms = numHairsAttr.setInternal(true);
	ms = addAttribute(numHairs);
	MFnNumericAttribute	numSamplePointsInHairAttr;
	numSamplePointsInHair = numSamplePointsInHairAttr.create("numSamplePointsInHair", "nsph", MFnNumericData::kInt, -1, &ms);
	ms = numSamplePointsInHairAttr.setInternal(true);
	ms = addAttribute(numSamplePointsInHair);
#endif
}
#endif
#if 1
#endif
#if 2
	ATTRIBUTE_AFFECTS(time, outputSurface);
	ATTRIBUTE_AFFECTS(worldMeshConnected, outputSurface);
	ATTRIBUTE_AFFECTS(SimulationStatus, outputSurface);
	ATTRIBUTE_AFFECTS(wakes, outputSurface);
#if 11
	ATTRIBUTE_AFFECTS(__shiftFrame, outputSurface);
#endif
#if 7
{
	MStatus	ms;
	ms = attributeAffects(__ShSimulationParameter.RaceType, outputSurface);
	ms = attributeAffects(__ShSimulationParameter.IncludeStiffness, outputSurface);
	ms = attributeAffects(__ShSimulationParameter.YoungModulus, outputSurface);
	ms = attributeAffects(__ShSimulationParameter.IncludeGravityAndInertia, outputSurface);
	ms = attributeAffects(__ShSimulationParameter.GravityParameter, outputSurface);
	ms = attributeAffects(__ShSimulationParameter.IncludeAirFriction, outputSurface);
	ms = attributeAffects(__ShSimulationParameter.AirFrictionParameter, outputSurface);
	ms = attributeAffects(__ShSimulationParameter.DissipationMode, outputSurface);
	ms = attributeAffects(__ShSimulationParameter.DissipationLambdaParameter, outputSurface);
	ms = attributeAffects(__ShSimulationParameter.DissipationGammaParameter, outputSurface);
	ms = attributeAffects(__ShSimulationParameter.WindMode, outputSurface);
	ms = attributeAffects(__ShSimulationParameter.WindRatio, outputSurface);
	ms = attributeAffects(__ShSimulationParameter.WindDirection2D, outputSurface);
	ms = attributeAffects(__ShSimulationParameter.WindDirection3D, outputSurface);
	ms = attributeAffects(__ShSimulationParameter.IncludeHairBodyOptimalCollision, outputSurface);
	ms = attributeAffects(__ShSimulationParameter.IncludeHairBodyCollision, outputSurface);
	ms = attributeAffects(__ShSimulationParameter.HBCParameter, outputSurface);
	ms = attributeAffects(__ShSimulationParameter.IncludeHairHairCollision, outputSurface);
	ms = attributeAffects(__ShSimulationParameter.HHCParameter, outputSurface);
#if 1
	ms = attributeAffects(__ShSimulationParameter.fAverageRadius, outputSurface);
	ms = attributeAffects(__ShSimulationParameter.fEllipticity, outputSurface);
	ms = attributeAffects(__ShSimulationParameter.fVolumeDensity, outputSurface);
	ms = attributeAffects(__ShSimulationParameter.fHelixK0STep, outputSurface);
	ms = attributeAffects(__ShSimulationParameter.fHelixK1Curvature, outputSurface);
	ms = attributeAffects(__ShSimulationParameter.inHelixK0STep, outputSurface);
	ms = attributeAffects(__ShSimulationParameter.inHelixK1Curvature, outputSurface);
	ms = attributeAffects(__ShSimulationParameter.fLengthFactor, outputSurface);
#endif
}
#endif
#if 0
	ATTRIBUTE_AFFECTS(time, numPointsInHair);
#endif
#else
	ATTRIBUTE_AFFECTS(time, outputSurface);
#endif
#if 9
#if 0
	ATTRIBUTE_AFFECTS(beInterpolated, outputSurface);
#if 1
	ATTRIBUTE_AFFECTS(outputSurface, hairControlPoints);
	ATTRIBUTE_AFFECTS(outputSurface, numHairs);
	ATTRIBUTE_AFFECTS(outputSurface, numSamplePointsInHair);
#endif
#endif
#endif
	ATTRIBUTE_AFFECTS(inputSurface, outputSurface);
	ATTRIBUTE_AFFECTS(inputSurface, mControlPoints);
	ATTRIBUTE_AFFECTS(inputSurface, worldSurface);
	ATTRIBUTE_AFFECTS(outputSurface, worldSurface);
	ATTRIBUTE_AFFECTS(inputSurface, bboxCorner1);
	ATTRIBUTE_AFFECTS(inputSurface, bboxCorner2);
	ATTRIBUTE_AFFECTS(cachedSurface, outputSurface);
	ATTRIBUTE_AFFECTS(cachedSurface, worldSurface);
#if 9
#if 0
	ATTRIBUTE_AFFECTS(seglen, outputSurface);
	ATTRIBUTE_AFFECTS(attenuateLength, outputSurface);
	ATTRIBUTE_AFFECTS(mControlPoints, outputSurface);
#endif
#endif
	ATTRIBUTE_AFFECTS(mControlPoints, cachedSurface);
	ATTRIBUTE_AFFECTS(mControlValueX, cachedSurface);
	ATTRIBUTE_AFFECTS(mControlValueY, cachedSurface);
	ATTRIBUTE_AFFECTS(mControlValueZ, cachedSurface);
	ATTRIBUTE_AFFECTS(mControlPoints, worldSurface);
	ATTRIBUTE_AFFECTS(mControlValueX, worldSurface);
	ATTRIBUTE_AFFECTS(mControlValueY, worldSurface);
	ATTRIBUTE_AFFECTS(mControlValueZ, worldSurface);
	return MS::kSuccess;
}
#if 0
#if 1
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
#if 9
MStatus	baseShape::setDependentsDirty(
#if 0
	const MPlug& plugBeingDirtied, MPlugArray& affectedPlugs
#else
	const MPlug& , MPlugArray&
#endif
)
{
#ifdef baseShape_DEBUG
	cout << "baseShape::setDependentsDirty(): plugBeingDirtied:= |" << plugBeingDirtied.info() << "(" << plugBeingDirtied.partialName() << ")|" << endl;
#endif
	MStatus	ms;
#if 0
	if (plugBeingDirtied == beInterpolated || plugBeingDirtied == seglen || plugBeingDirtied == attenuateLength || plugBeingDirtied == mControlPoints) {
		MPlug	plug(thisMObject(), outputSurface);
		ms = affectedPlugs.append(plug);
		__call_generateHair_only = true;
	}
	if (plugBeingDirtied == outputSurface) {
		MPlug	plug(thisMObject(), hairControlPoints);
		ms = affectedPlugs.append(plug);
		plug.setAttribute(numHairs);
		ms = affectedPlugs.append(plug);
		plug.setAttribute(numSamplePointsInHair);
		ms = affectedPlugs.append(plug);
		__call_generateHair_only = true;
	}
#endif
#if 0
#if 1
	if (plugBeingDirtied == __tentacleDoAction) {
		cout << "plugBeingDirtied: __tentacleDoAction" << endl;
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
	if (isDirty) {
		MPlug	plug(thisMObject(), __tentacleArray);
		ms = affectedPlugs.append(plug);
		ms = plug.setAttribute(__tentacleDoAction);
		ms = plug.setValue(TentacleDo::Update);
	}
#endif
#if 1
	RampWakes::Type	rampType = RampWakes::None;
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
	return MStatus::kSuccess;
}
#endif
#if 0
MStatus	initializePlugin(MObject obj)
{
	MString	classification("baseShape");
	MStatus	ms;
	MFnPlugin	plugin(obj, "DN2 Company", "0.1", "Any", &ms);
	if (!ms) {
		cerr << "Error: initializePlugin: ";
		cerr << "MFnPlugin(|Any|): Failure: ";
		cerr << ms << endl;
		return ms;
	}
	ms = plugin.registerData(
		"baseShapeData",
		baseShapeData::id,
		&baseShapeData::creator,
		MPxData::kGeometryData
	);
	if (!ms) {
		cerr << "Error: initializePlugin: ";
		cerr << "MFnPlugin::registerData(MPxData::kGeometryData): Failure: ";
		cerr << ms << endl;
		return ms;
	}
	ms = plugin.registerShape(
		"baseShapeNode",
		baseShape::id,
		&baseShape::creator,
		&baseShape::initialize,
		&baseShapeUI::creator,
		&classification
	);
	if (!ms) {
		cerr << "Error: initializePlugin: ";
		cerr << "MFnPlugin::registerShape(" << "baseShapeNode" << "|): Failure: ";
		cerr << ms << endl;
		return ms;
	}
	ms = plugin.registerNode(
		"baseShapeCreator",
		baseShapeCreator::id,
		&baseShapeCreator::creator,
		&baseShapeCreator::initialize,
		MPxNode::kDependNode,
		&classification
	);
	if (!ms) {
		cerr << "Error: initializePlugin: ";
		cerr << "MFnPlugin::registerNode(" << "baseShapeCreator" << "|): Failure: ";
		cerr << ms << endl;
		return ms;
	}
	return MStatus::kSuccess;
}
MStatus	uninitializePlugin(MObject obj)
{
	MStatus	ms;
	MFnPlugin	plugin(obj, "DN2 Company", "0.1", "Any", &ms);
	if (!ms) {
		cerr << "Error: uninitializePlugin: ";
		cerr << "MFnPlugin(|Any|): Failure: ";
		cerr << ms << endl;
		return ms;
	}
	ms = plugin.deregisterNode(baseShape::id);
	if (!ms) {
		cerr << "Error: uninitializePlugin: ";
		cerr << "MFnPlugin::deregisterNode(|" << "baseShapeNode" << "|): Failure: ";
		cerr << ms << endl;
		return ms;
	}
	ms = plugin.deregisterData(baseShapeData::id);
	if (!ms) {
		cerr << "Error: uninitializePlugin: ";
		cerr << "MFnPlugin::deregisterData(MPxData::kGeometryData): Failure: ";
		cerr << ms << endl;
		return ms;
	}
	ms = plugin.deregisterNode(baseShapeCreator::id);
	if (!ms) {
		cerr << "Error: uninitializePlugin: ";
		cerr << "MFnPlugin::deregisterNode(|" << "baseShapeCreator" << "|): Failure: ";
		cerr << ms << endl;
		return ms;
	}
	return MStatus::kSuccess;
}
#endif
#if 2
#if 7
#if 1
void	baseShape::applySimulationParameter(bool all, bool exceptRaceType)
#else
void	baseShape::applySimulationParameter(bool all)
#endif
{
	if (!__shSimulationMayaPtr) {
		cerr << "Warning: baseShape::applySimulationParameter: No kShSimulationMaya" << endl;
		return;
	}
	if (__simulationStatus == Simulation::ReadOnly) {
		cerr << "Warning: baseShape::applySimulationParameter: Simulation: Status = ReadOnly" << endl;
		return;
	}
	kSimulationCache::kSimulationParameter::kType	type;
	kSimulationCache::kSimulationParameter::kValue	val;
#if 1
	MPlug	plug(thisMObject(), __ShSimulationParameter.IncludeStiffness);
	plug.setAttribute(__ShSimulationParameter.IncludeStiffness);
	plug.getValue(val.bVal);
	type = kSimulationCache::kSimulationParameter::IncludeStiffness;
	__shSimulationMayaPtr->setSimulationParameter(type, val);
	if (!exceptRaceType) {
		plug.setAttribute(__ShSimulationParameter.RaceType);
		short	hRaceType;
		MStatus	ms = plug.getValue(hRaceType);
		val.raceType = (numRaceType) hRaceType;
		type = kSimulationCache::kSimulationParameter::RaceType;
		__shSimulationMayaPtr->setSimulationParameter(type, val);
	}
#else
	MPlug	plug(thisMObject(), __ShSimulationParameter.RaceType);
	short	hRaceType;
	MStatus	ms = plug.getValue(hRaceType);
	val.raceType = (numRaceType) hRaceType;
	type = kSimulationCache::kSimulationParameter::RaceType;
	__shSimulationMayaPtr->setSimulationParameter(type, val);
	plug.setAttribute(__ShSimulationParameter.IncludeStiffness);
	plug.getValue(val.bVal);
	type = kSimulationCache::kSimulationParameter::IncludeStiffness;
	__shSimulationMayaPtr->setSimulationParameter(type, val);
#endif
	plug.setAttribute(__ShSimulationParameter.YoungModulus);
	plug.getValue(val.dVal);
	type = kSimulationCache::kSimulationParameter::YoungModulus;
	__shSimulationMayaPtr->setSimulationParameter(type, val);
	plug.setAttribute(__ShSimulationParameter.IncludeGravityAndInertia);
	plug.getValue(val.bVal);
	type = kSimulationCache::kSimulationParameter::IncludeGravityAndInertia;
	__shSimulationMayaPtr->setSimulationParameter(type, val);
	plug.setAttribute(__ShSimulationParameter.GravityParameter);
	plug.getValue(val.iVal);
	type = kSimulationCache::kSimulationParameter::GravityParameter;
	__shSimulationMayaPtr->setSimulationParameter(type, val);
	plug.setAttribute(__ShSimulationParameter.IncludeAirFriction);
	plug.getValue(val.bVal);
	type = kSimulationCache::kSimulationParameter::IncludeAirFriction;
	__shSimulationMayaPtr->setSimulationParameter(type, val);
	plug.setAttribute(__ShSimulationParameter.AirFrictionParameter);
	plug.getValue(val.iVal);
	type = kSimulationCache::kSimulationParameter::AirFrictionParameter;
	__shSimulationMayaPtr->setSimulationParameter(type, val);
	plug.setAttribute(__ShSimulationParameter.DissipationMode);
	short	hDissipationMode;
	plug.getValue(hDissipationMode);
	val.dissipationMode = (numDissipationMode) hDissipationMode;
	type = kSimulationCache::kSimulationParameter::DissipationMode;
	__shSimulationMayaPtr->setSimulationParameter(type, val);
	plug.setAttribute(__ShSimulationParameter.DissipationLambdaParameter);
	plug.getValue(val.dVal);
	type = kSimulationCache::kSimulationParameter::DissipationLambdaParameter;
	__shSimulationMayaPtr->setSimulationParameter(type, val);
	plug.setAttribute(__ShSimulationParameter.DissipationGammaParameter);
	plug.getValue(val.dVal);
	type = kSimulationCache::kSimulationParameter::DissipationGammaParameter;
	__shSimulationMayaPtr->setSimulationParameter(type, val);
	plug.setAttribute(__ShSimulationParameter.WindMode);
	short	hWindMode;
	plug.getValue(hWindMode);
	val.windMode = (numWindMode) hWindMode;
	type = kSimulationCache::kSimulationParameter::WindMode;
	__shSimulationMayaPtr->setSimulationParameter(type, val);
	plug.setAttribute(__ShSimulationParameter.WindRatio);
	plug.getValue(val.iVal);
	type = kSimulationCache::kSimulationParameter::WindRatio;
	__shSimulationMayaPtr->setSimulationParameter(type, val);
	plug.setAttribute(__ShSimulationParameter.WindDirection2D);
	plug.getValue(val.iVal);
	type = kSimulationCache::kSimulationParameter::WindDirection2D;
	__shSimulationMayaPtr->setSimulationParameter(type, val);
	plug.setAttribute(__ShSimulationParameter.WindDirection3D);
	MObject	WindDirection3D_data;
	plug.getValue(WindDirection3D_data);
	MFnNumericData	WindDirection3D_val(WindDirection3D_data);
	WindDirection3D_val.getData(val.i2Val[0], val.i2Val[1]);
	type = kSimulationCache::kSimulationParameter::WindDirection3D;
	__shSimulationMayaPtr->setSimulationParameter(type, val);
	plug.setAttribute(__ShSimulationParameter.IncludeHairBodyOptimalCollision);
	plug.getValue(val.bVal);
	type = kSimulationCache::kSimulationParameter::IncludeHairBodyOptimalCollision;
	__shSimulationMayaPtr->setSimulationParameter(type, val);
	plug.setAttribute(__ShSimulationParameter.IncludeHairBodyCollision);
	plug.getValue(val.bVal);
	type = kSimulationCache::kSimulationParameter::IncludeHairBodyCollision;
	__shSimulationMayaPtr->setSimulationParameter(type, val);
	plug.setAttribute(__ShSimulationParameter.HBCParameter);
	MObject	HBCParameter_data;
	plug.getValue(HBCParameter_data);
	MFnNumericData	HBCParameter_val(HBCParameter_data);
	HBCParameter_val.getData(val.v2Val[0], val.v2Val[1]);
	type = kSimulationCache::kSimulationParameter::HBCParameter;
	__shSimulationMayaPtr->setSimulationParameter(type, val);
	plug.setAttribute(__ShSimulationParameter.IncludeHairHairCollision);
	plug.getValue(val.bVal);
	type = kSimulationCache::kSimulationParameter::IncludeHairHairCollision;
	__shSimulationMayaPtr->setSimulationParameter(type, val);
	plug.setAttribute(__ShSimulationParameter.HHCParameter);
	plug.getValue(val.dVal);
	type = kSimulationCache::kSimulationParameter::HHCParameter;
	__shSimulationMayaPtr->setSimulationParameter(type, val);
#if 1
	plug.setAttribute(__ShSimulationParameter.fAverageRadius);
	plug.getValue(val.dVal);
	type = kSimulationCache::kSimulationParameter::fAverageRadius;
	__shSimulationMayaPtr->setSimulationParameter(type, val);
	plug.setAttribute(__ShSimulationParameter.fEllipticity);
	plug.getValue(val.dVal);
	type = kSimulationCache::kSimulationParameter::fEllipticity;
	__shSimulationMayaPtr->setSimulationParameter(type, val);
	plug.setAttribute(__ShSimulationParameter.fVolumeDensity);
	plug.getValue(val.dVal);
	type = kSimulationCache::kSimulationParameter::fVolumeDensity;
	__shSimulationMayaPtr->setSimulationParameter(type, val);
	plug.setAttribute(__ShSimulationParameter.fHelixK0STep);
	plug.getValue(val.dVal);
	type = kSimulationCache::kSimulationParameter::fHelixK0STep;
	__shSimulationMayaPtr->setSimulationParameter(type, val);
	plug.setAttribute(__ShSimulationParameter.fHelixK1Curvature);
	plug.getValue(val.dVal);
	type = kSimulationCache::kSimulationParameter::fHelixK1Curvature;
	__shSimulationMayaPtr->setSimulationParameter(type, val);
	plug.setAttribute(__ShSimulationParameter.inHelixK0STep);
	plug.getValue(val.v2Val[0]);
	plug.setAttribute(__ShSimulationParameter.inHelixK1Curvature);
	plug.getValue(val.v2Val[1]);
	type = kSimulationCache::kSimulationParameter::inHelixK0K1;
	__shSimulationMayaPtr->setSimulationParameter(type, val);
	plug.setAttribute(__ShSimulationParameter.fLengthFactor);
	plug.getValue(val.dVal);
	type = kSimulationCache::kSimulationParameter::fLengthFactor;
	__shSimulationMayaPtr->setSimulationParameter(type, val);
#endif
	if (all) {
		ShSimParameter&	simulationParameter = __shSimulationMayaPtr->simulationParameter();
		simulationParameter.fTolerance = __ShSimulationParameter.fTolerance;
#if 0
		simulationParameter.fAverageRadius = __ShSimulationParameter.fAverageRadius;
		simulationParameter.fEllipticity = __ShSimulationParameter.fEllipticity;
		simulationParameter.fHelixK0STep = __ShSimulationParameter.fHelixK0STep;
		simulationParameter.fHelixK1Curvature = __ShSimulationParameter.fHelixK1Curvature;
#endif
	} else {
#if 1
		cerr << "Warning: The Parameters(fTolerance) NOT Apply unless Simulation Status is Off" << endl;
#else
		cerr << "Warning: The Parameters(fTolerance, fAverageRadius, fEllipticity, fHelixK0STep, fHelixK1Curvature) NOT Apply unless Simulation Status is Off" << endl;
#endif
	}
}
void	baseShape::setShSimulationParameterToShSimParameter(ShSimParameter& parameter)
{
	MStatus	ms;
	MPlug	plug(thisMObject(), __ShSimulationParameter.InverseTimeStep);
	int	intTimeStepInverse;
	plug.getValue(intTimeStepInverse);
	parameter.fTimeStepInverse = (ShScalar) intTimeStepInverse;
	parameter.fTolerance = __ShSimulationParameter.fTolerance;
	plug.setAttribute(__ShSimulationParameter.RaceType);
	short	hRaceType;
	plug.getValue(hRaceType);
	parameter.currentRace = (numRaceType) hRaceType;
#if 1
	plug.setAttribute(__ShSimulationParameter.fAverageRadius);
	double	fAverageRadius;
	plug.getValue(fAverageRadius);
#if 1
	parameter.sSection.fAverageRadius = fAverageRadius * 1.e-4;
#else
	parameter.fAverageRadius = fAverageRadius * 1.e-4;
#endif
	plug.setAttribute(__ShSimulationParameter.fEllipticity);
#if 1
	plug.getValue(parameter.sSection.fEllipticity);
#else
	plug.getValue(parameter.fEllipticity);
#endif
	plug.setAttribute(__ShSimulationParameter.fVolumeDensity);
	plug.getValue(parameter.fVolumeDensity);
	plug.setAttribute(__ShSimulationParameter.fHelixK0STep);
	plug.getValue(parameter.fHelixK0STep);
	plug.setAttribute(__ShSimulationParameter.fHelixK1Curvature);
	plug.getValue(parameter.fHelixK1Curvature);
	plug.setAttribute(__ShSimulationParameter.fLengthFactor);
	plug.getValue(parameter.fLengthFactor);
#else
	parameter.fAverageRadius = __ShSimulationParameter.fAverageRadius;
	parameter.fEllipticity = __ShSimulationParameter.fEllipticity;
	parameter.fHelixK0STep = __ShSimulationParameter.fHelixK0STep;
	parameter.fHelixK1Curvature = __ShSimulationParameter.fHelixK1Curvature;
#endif
	plug.setAttribute(__ShSimulationParameter.IncludeStiffness);
	plug.getValue(parameter.includeStiffness);
	plug.setAttribute(__ShSimulationParameter.YoungModulus);
	plug.getValue(parameter.fYoungModulus);
	plug.setAttribute(__ShSimulationParameter.IncludeGravityAndInertia);
	plug.getValue(parameter.includeGravityAndInertia);
	plug.setAttribute(__ShSimulationParameter.GravityParameter);
	int	intGravityParameter;
	plug.getValue(intGravityParameter);
	parameter.fGravityValue = intGravityParameter * EARTH_GRAVITY;
	plug.setAttribute(__ShSimulationParameter.IncludeAirFriction);
	plug.getValue(parameter.includeAirFriction);
	int	intAirFrictionParameter;
	plug.setAttribute(__ShSimulationParameter.AirFrictionParameter);
	plug.getValue(intAirFrictionParameter);
	parameter.fAirFriction = intAirFrictionParameter * 1.e-5;
	plug.setAttribute(__ShSimulationParameter.DissipationMode);
	short	hDissipationMode;
	plug.getValue(hDissipationMode);
	parameter.choiceDissipation = (numDissipationMode) hDissipationMode;
	plug.setAttribute(__ShSimulationParameter.DissipationLambdaParameter);
	plug.getValue(parameter.fLambdaDissipation);
	plug.setAttribute(__ShSimulationParameter.DissipationGammaParameter);
	plug.getValue(parameter.fGammaDissipation);
	plug.setAttribute(__ShSimulationParameter.WindMode);
	short	hWindMode;
	plug.getValue(hWindMode);
	parameter.choiceWind = (numWindMode) hWindMode;
	plug.setAttribute(__ShSimulationParameter.IncludeHairBodyOptimalCollision);
	plug.getValue(parameter.includeHairBodyOptimalCollision);
	plug.setAttribute(__ShSimulationParameter.IncludeHairBodyCollision);
	plug.getValue(parameter.includeHairBodyCollision);
	plug.setAttribute(__ShSimulationParameter.IncludeHairHairCollision);
	plug.getValue(parameter.includeHairHairCollision);
}
void	baseShape::setShSimulationParameterFromShSimParameter(const ShSimParameter& parameter, bool off)
{
	MStatus	ms;
	if ((int) parameter.fTimeStepInverse != 240) {
		cerr << "Waring: baseShape::setShSimulationParameterFromShSimParameter: Inverse Time Step: AS OF 3/8/2009 (EST), Only 240 in Maya Plug-in" << endl;;
	}
	MPlug	plug(thisMObject(), __ShSimulationParameter.RaceType);
	plug.setAttribute(__ShSimulationParameter.RaceType);
	plug.setValue((short) parameter.currentRace);
	if (off) {
		__ShSimulationParameter.fTolerance = parameter.fTolerance;
#if 0
		__ShSimulationParameter.fAverageRadius = parameter.fAverageRadius;
		__ShSimulationParameter.fEllipticity = parameter.fEllipticity;
		__ShSimulationParameter.fHelixK0STep = parameter.fHelixK0STep;
		__ShSimulationParameter.fHelixK1Curvature = parameter.fHelixK1Curvature;
#endif
	}
#if 1
	plug.setAttribute(__ShSimulationParameter.fAverageRadius);
#if 1
	ms = plug.setValue(parameter.sSection.fAverageRadius / 1.e-4) ;
#else
	ms = plug.setValue(parameter.fAverageRadius / 1.e-4) ;
#endif
	plug.setAttribute(__ShSimulationParameter.fEllipticity);
#if 1
	ms = plug.setValue(parameter.sSection.fEllipticity) ;
#else
	ms = plug.setValue(parameter.fEllipticity) ;
#endif
	plug.setAttribute(__ShSimulationParameter.fVolumeDensity);
	ms = plug.setValue(parameter.fVolumeDensity) ;
	plug.setAttribute(__ShSimulationParameter.fHelixK0STep);
	ms = plug.setValue(parameter.fHelixK0STep) ;
	plug.setAttribute(__ShSimulationParameter.fHelixK1Curvature);
	ms = plug.setValue(parameter.fHelixK1Curvature) ;
	plug.setAttribute(__ShSimulationParameter.fLengthFactor);
	ms = plug.setValue(parameter.fLengthFactor) ;
#endif
	plug.setAttribute(__ShSimulationParameter.IncludeStiffness);
	ms = plug.setValue(parameter.includeStiffness) ;
	plug.setAttribute(__ShSimulationParameter.YoungModulus);
	ms = plug.setValue(parameter.fYoungModulus);
	plug.setAttribute(__ShSimulationParameter.IncludeGravityAndInertia);
	plug.setValue(parameter.includeGravityAndInertia);
	plug.setAttribute(__ShSimulationParameter.GravityParameter);
	plug.setValue((int) ceil(parameter.fGravityValue / EARTH_GRAVITY));
	plug.setAttribute(__ShSimulationParameter.IncludeAirFriction);
	plug.setValue(parameter.includeAirFriction);
	plug.setAttribute(__ShSimulationParameter.AirFrictionParameter);
	plug.setValue((int) ceil(parameter.fAirFriction / 1.e-5));
	plug.setAttribute(__ShSimulationParameter.DissipationMode);
	plug.setValue((short) parameter.choiceDissipation);
	plug.setAttribute(__ShSimulationParameter.DissipationLambdaParameter);
	plug.setValue(parameter.fLambdaDissipation);
	plug.setAttribute(__ShSimulationParameter.DissipationGammaParameter);
	plug.setValue(parameter.fGammaDissipation);
	plug.setAttribute(__ShSimulationParameter.WindMode);
	plug.setValue((short) parameter.choiceWind);
#if 0
	if (parameter.pWind) {
		ShSimWind&	wind = *((ShSimWind*) parameter.pWind);
		plug.setAttribute(__ShSimulationParameter.WindRatio);
		plug.setValue((int) wind.getWindRatio());
		plug.setAttribute(__ShSimulationParameter.WindDirection2D);
		plug.setValue(wind.getWindDirection2D());
		plug.setAttribute(__ShSimulationParameter.WindDirection3D);
		ShVector3	windDirection3D;
		wind.getWindDirection3D(windDirection3D);
		MFnNumericData	WindDirection3D_val;
		MObject	WindDirection3D_data = WindDirection3D_val.create(MFnNumericData::k3Double, &ms);
		WindDirection3D_val.setData(windDirection3D[0], windDirection3D[1], windDirection3D[2]);
		plug.setValue(WindDirection3D_data);
	}
#endif
	plug.setAttribute(__ShSimulationParameter.IncludeHairBodyOptimalCollision);
	plug.setValue(parameter.includeHairBodyOptimalCollision);
	plug.setAttribute(__ShSimulationParameter.IncludeHairBodyCollision);
	plug.setValue(parameter.includeHairBodyCollision);
#if 0
	if (parameter.pCollBody) {
		plug.setAttribute(__ShSimulationParameter.HBCParameter);
		ShScalar	ratio, criticalDepth;
		ShSimCollisionBody&	body = *((ShSimCollisionBody*) parameter.pCollBody);
		body.getHBCParameter(&ratio, &criticalDepth);
		MFnNumericData	HBCParameter_val;
		MObject	HBCParameter_data = HBCParameter_val.create(MFnNumericData::k2Double, &ms);
		HBCParameter_val.setData(ratio, criticalDepth);
		plug.setValue(HBCParameter_data);
	}
#endif
	plug.setAttribute(__ShSimulationParameter.IncludeHairHairCollision);
	plug.setValue(parameter.includeHairHairCollision);
}
void	baseShape::printSimulationParameter(const ShSimParameter& parameter)
{
	cout << "Simulation Parameters..." << endl;
	cout << "\t" << "(fTimeStepInverse = " << parameter.fTimeStepInverse << ")" << endl;
	cout << "\t" << "(fTolerance = " << parameter.fTolerance << ")" << endl;
	cout << "\t" << "Race: " << (parameter.currentRace == RACE_ASIAN ? "RACE_ASIAN" : (parameter.currentRace == RACE_CAUCASIAN1 ? "RACE_CAUCASIAN1" : (parameter.currentRace == RACE_CAUCASIAN2 ? "RACE_CAUCASIAN2" : (parameter.currentRace == RACE_AFRICAN ? "RACE_AFRICAN" : "RACE_UNKNOWN")))) << endl;
	cout << "\t" << "IncludeStiffness? " << (parameter.includeStiffness ? "True" : "False") << endl;
#if 1
	cout << "\t" << "fAverageRadius = " << parameter.sSection.fAverageRadius << endl;
	cout << "\t" << "fEllipticity = " << parameter.sSection.fEllipticity << endl;
#else
	cout << "\t" << "fAverageRadius = " << parameter.fAverageRadius << endl;
	cout << "\t" << "fEllipticity = " << parameter.fEllipticity << endl;
#endif
	cout << "\t" << "fVolumeDensity = " << parameter.fVolumeDensity << endl;
	cout << "\t" << "fHelixK0STep = " << parameter.fHelixK0STep << endl;
	cout << "\t" << "fHelixK1Curvature = " << parameter.fHelixK1Curvature << endl;
	cout << "\t" << "fLengthFactor = " << parameter.fLengthFactor << endl;
	cout << "\t" << "fYoungModulus = " << parameter.fYoungModulus << endl;
	cout << "\t" << "IncludeGravityAndInertia? " << (parameter.includeGravityAndInertia ? "True" : "False") << endl;
	cout << "\t" << "fGravityValue = " << parameter.fGravityValue << endl;
	cout << "\t" << "IncludeAirFriction? " << (parameter.includeAirFriction ? "True" : "False") << endl;
	cout << "\t" << "fAirFriction = " << parameter.fAirFriction << endl;
	cout << "\t" << "Dissipation Mode: " << (parameter.choiceDissipation == NODISSIPATE ? "NODISSIPATE" : (parameter.choiceDissipation == LAMBDA ? "LAMBDA" : (parameter.choiceDissipation == GAMMA ? "GAMMA" : "UNKNOWN"))) << endl;
	cout << "\t" << "fLambdaDissipation = " << parameter.fLambdaDissipation << endl;
	cout << "\t" << "fGammaDissipation = " << parameter.fGammaDissipation << endl;
#if 1
	cout << "\t" << "Wind Mode: " << (parameter.choiceWind == NOWIND ? "NOWIND" : (parameter.choiceWind == UNIFORMWIND ? "UNIFORM" : (parameter.choiceWind == PTNDEPENDENT ? "PTNDEPENDENT" : (parameter.choiceWind == PTNDEPENDENTVELOCITY ? "PTNDEPENDENTVELOCITY" : "UNKNOWN")))) << endl;
#else
	cout << "\t" << "Wind Mode: " << (parameter.choiceWind == NOWIND ? "NOWIND" : (parameter.choiceWind == UNIFORM ? "UNIFORM" : (parameter.choiceWind == PTNDEPENDENT ? "PTNDEPENDENT" : (parameter.choiceWind == PTNDEPENDENTVELOCITY ? "PTNDEPENDENTVELOCITY" : "UNKNOWN")))) << endl;
#endif
#if 1
{
	ShSimParameter&	nonConstParameter = *((ShSimParameter*) &parameter);
	cout << "\t" << "fWindRatio = " << parameter.fWindRatio << endl;
	ShScalar	ratio;
	nonConstParameter.getPWindRatio(&ratio);
	cout << "\t" << "Wind Ratio: ratio = " << ratio << endl;
	cout << "\t" << "fWindTheta = " << parameter.fWindTheta << ", fWindPhi = " << parameter.fWindPhi << endl;
	cout << "\t" << "vWindDirection: (" << parameter.vWindDirection[0] << ", " << parameter.vWindDirection[1] << ", " << parameter.vWindDirection[2] << ")" << endl;
	ShScalar	theta, phi;
	nonConstParameter.getPWindDirection3D(&theta, &phi);
	ShVector3	dir;
	nonConstParameter.getPWindDirection3D(dir);
	cout << "\t" << "Wind Direction: theta = " << theta << ", phi = " << phi << endl;
	cout << "\t" << "Wind Direction: (" << dir[0] << ", " << dir[1] << ", " << dir[2] << ")" << endl;
}
#else
	if (parameter.pWind) {
		ShSimWind&	wind = *((ShSimWind*) parameter.pWind);
		cout << "\t" << "WindRatio = " << wind.getWindRatio() << endl;
		cout << "\t" << "Direction2D = " << wind.getWindDirection2D() << endl;
		ShVector3	direction3D;
		wind.getWindDirection3D(direction3D);
		cout << "\t" << "Direction3D: (" << direction3D[0] << ", " << direction3D[1] << ", " << direction3D[2] << ")" << endl;
	} else {
		cout << "\t" << "No ShSimWind" << endl;
	}
#endif
	cout << "\t" << "includeHairBodyOptimalCollision? " << (parameter.includeHairBodyOptimalCollision ? "True" : "False") << endl;
	cout << "\t" << "includeHairBodyCollision? " << (parameter.includeHairBodyCollision ? "True" : "False") << endl;
#if 1
{
	ShSimParameter&	nonConstParameter = *((ShSimParameter*) &parameter);
	cout << "\t" << "HBC Parameter: fHBCRatio = " << parameter.fHBCRatio << ", fHBCDepth = " << parameter.fHBCDepth << endl;
	ShScalar	ratio, criticalDepth;
	nonConstParameter.getPHBCParameter(&ratio, &criticalDepth);
	cout << "\t" << "HBC Parameter: ratio = " << ratio << ", criticalDepth = " << criticalDepth << endl;
}
#else
	if (parameter.pCollBody) {
		ShSimCollisionBody&	body = *((ShSimCollisionBody*) parameter.pCollBody);
		ShScalar	ratio, criticalDepth;
		body.getHBCParameter(&ratio, &criticalDepth);
		cout << "\t" << "ratio = " << ratio << ", criticalDepth = " << criticalDepth << endl;
	} else {
		cout << "\t" << "No ShSimCollisionBody" << endl;
	}
#endif
	cout << "\t" << "includeHairHairCollision? " << (parameter.includeHairHairCollision ? "True" : "False") << endl;
}
#endif
#endif
#if 0
#if 1
#if 1
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
void	baseShape::__activeListModifiedCallback(void* voidPtr)
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
	cout << "baseShape::__activeListModifiedCallback: physical indices: " << physicalIndexArray << " (lastSelectedPhysicalIndex = " << lastSelectedPhysicalIndex << ")" << endl;
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
	baseShape&	tentacleNode = *((baseShape*) voidPtr);
	MDataBlock	datablock = tentacleNode.forceCache();
	MPlug	plug_tentacleArray(tentacleNode.thisMObject(), baseShape::__tentacleArray);
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
	ms = arrayHandle_tentacleArray.jumpToArrayElement(physicalIndex);
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
	MPlug	plug_tentacleParameter(tentacleNode.thisMObject(), baseShape::__tentacleParameter);
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
MStatus	baseShape::__computeBoundingBox(const MPlug& plug, MDataBlock& datablock)
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
	for (int n = 0; n < (int) handleArray.elementCount(&ms); ++n) {
		ms = handleArray.jumpToArrayElement(n);
		MDataHandle	handle = handleArray.inputValue(&ms);
		MPlug		plug_root = plug.child(TENTACLE_ARRAY_ROOT, &ms);
		MDataHandle	handle_root = handle.child(plug_root);
		double3&	root = handle_root.asDouble3();
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
	}
	lowerHandle.setClean();
	upperHandle.setClean();
	childChanged(MPxSurfaceShape::kBoundingBoxChanged);
	return ms;
}
#endif
#if 0
void	baseShape::getTransverseVectors(MVectorArray& transVectors)
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
MStatus	baseShape::getDrawTentacleRootStatus(bool& status)
{
	MPlug	plug(thisMObject(), __drawTentacleRoot);
	return plug.getValue(status);
}
MStatus	baseShape::getTentacleRootPosition(MPoint& position)
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
MStatus	baseShape::getTentacleArray(kPtrArray<kTentacle>& tentacleArray)
{
	MStatus	ms;
	tentacleArray.clear();
	MPlug	plug_tentacleArray(thisMObject(), __tentacleArray);
	unsigned	numElements = plug_tentacleArray.evaluateNumElements(&ms);
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
		kCut&	cut = tentacle.cut;
		MPlug	plug_cut_have = plug_cut.child(TENTACLE_CUT_VAL_HAVE, &ms);
		ms = plug_cut_have.getValue(cut.have);
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
const kPtrArray<RampTuple>*	baseShape::GetRampTupleArrayPtr() const
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
		return &__rotationLocalampTupleArray;
	return 0;
}
void	baseShape::__setRampTupleArray(RampWakes::Type type)
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
		rampTupleArrayPtr = &__rotationLocalampTupleArray;
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
	if (indexedRampTuplePtr0) {
		delete[] indexedRampTuplePtr0;
		indexedRampTuplePtr0 = 0;
	}
}
#if 0
void	baseShape::__setRampTupleArray(RampWakes::Type type, MDataBlock datablock)
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
		rampTupleArrayPtr = &__rotationLocalampTupleArray;
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
int	baseShape::GetInterpolatedValue(
	const kPtrArray<RampTuple>&	rampTupleArray,
	float	x,
	float&	y,
	bool	clamp
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
				y = 0.f;
				cout << "x = " << x << ": y = " << y << " clamped to 0" << endl;
				assert(y >= 0.f);
			} else if (y > 1.f) {
				y = 1.f;
				cout << "x = " << x << ": y = " << y << " clamped to 1" << endl;
				assert(y <= 1.f);
			}
		}
	}
	return rval;
}
#endif
#endif
#endif
