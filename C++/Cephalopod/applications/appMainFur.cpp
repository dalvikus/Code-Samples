#include <iostream>
	using std::cout;
	using std::cerr;
	using std::endl;
#define NT_PLUGIN
#define REQUIRE_IOSTREAM
#define MLIBRARY_DONTUSE_MFC_MANIFEST
#include "kMayaAppFur.h"
#if 1
#include <afterShave/kTime.h>
#define _INHERIT_kBODY
#ifdef _INHERIT_kBODY
#pragma warning(disable: 4250)
#endif
#include <afterShave/kShSimulationMayaFur.h>
#ifdef _INHERIT_kBODY
#pragma warning(default: 4250)
#else
#include <afterShave/kBodyMayaFur.h>
#endif
#include <afterShave/afterShaveShape.h>
#endif
int	main(int, const char* argv[])
{
	MStatus	ms;
	int	verbose_level = 1;
	kMayaAppFur	mayaAppFur(
		argv[0],
		"afterShaveShapeNode",
		"afterShave", "debug/afterShave.mll",
		"movingBody2.ma",
		verbose_level,
		&ms
	);
	if (!ms) {
		ms.perror("kMayaApp::kMayaApp(...): Failure");
		return 1;
	}
	ms = mayaAppFur.set();
	if (!ms) {
		ms.perror("kMayaAppFur::set(): Failure");
		return 1;
	}
#if 1
#pragma warning(disable: 4127)
do {
#if 1
	kShSimulationMayaFur	shSimulationMayaFur(kTime::kTimeUnitFilm10, 24, false);
#ifdef _INHERIT_kBODY
	kBodyMayaFur&	bodyMayaFur = shSimulationMayaFur;
#else
	kBodyMayaFur&	bodyMayaFur = *((kBodyMayaFur*) shSimulationMayaFur.bodyMayaPtr());
#endif
	if (&bodyMayaFur) {
		ms = bodyMayaFur.set(mayaAppFur.body());
		bodyMayaFur.set__T0_in_kSpace_array(mayaAppFur.geometry());
	} else {
		cerr << "bodyMayaFurPtr is nil" << endl;
		break;
	}
#endif
#if 1
{
	std::vector<double>	__segmentLengthInGuide;
	for (int i = 0; i < 8; ++i) {
		__segmentLengthInGuide.push_back(afterShaveShape::SEGMENT_LENGTH_DEFAULT_IN_GUIDE);
	}
	std::vector<int>	strandIdArray;
	for (int i = 0; i < 8; ++i)
		strandIdArray.push_back(i);
	for (unsigned n = 0; n < strandIdArray.size(); ++n) {
		if (shSimulationMayaFur.addStrand(
			strandIdArray.at(n),
			DEFAULT_SEGMENT_COUNT,
			(afterShaveShape::N_POINTS_IN_GUIDE - 1) * __segmentLengthInGuide.at(n)
		) == NULL) {
			cerr << "Error: kSimulationCache::addStrand(): Failure" << endl;
			continue;
		}
#ifdef INCLUDE_HAIR_BODY_COLLISION_TEST
		break;
#endif
	}
#if 0
if (__isWorldMeshConnected) {
			__shSimulationMayaPtr->setMeshGeom(meshGeom());
			__shSimulationMayaPtr->setCPPtr(getControlPoints());
afterShaveGeometry*	geometryPtr = meshGeom();
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
	for (int k = 0; k < (int) afterShaveShape::N_POINTS_IN_GUIDE; ++k) {
		if (k) {
			MVector	offset = cp[afterShaveShape::N_POINTS_IN_GUIDE * strandId + k];
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
#endif
#if 1
{
	ShSimParameter&	simulationParameter = shSimulationMayaFur.simulationParameter();
#ifdef _DEBUG
	afterShaveShape::printSimulationParameter(simulationParameter);
#endif
	MString	fileName("D:/home/icentrik/My Documents/maya/projects/default/scenes/movingCube.kps");
	if (simulationParameter.fileReadEveryRecord((char*) fileName.asChar()) < 0) {
		cerr << "ShSimParameter::fileReadEveryRecord(): Failure" << endl;
		break;
	} else {
		afterShaveShape::printSimulationParameter(simulationParameter);
	}
#ifdef _DEBUG
	afterShaveShape::printSimulationParameter(simulationParameter);
#endif
}
#endif
#if 1
{
#if 1
#if 1
	shSimulationMayaFur.setDynamicsFlag(true);
#else
{
	MFnDependencyNode	fnDepNode(mayaAppFur.node(), &ms);
	if (!ms) {
		ms.perror("MFnDependencyNode::MFnDependencyNode(MObject&): Failure");
		break;
	}
	MPlug	plug_Dynamics = fnDepNode.findPlug("Dynamics", true, &ms);
	if (!ms) {
		ms.perror("MFnDependencyNode::findPlug(|Dynamics|): Failure");
		break;
	}
	bool	dynamics;
	plug_Dynamics.getValue(dynamics);
	shSimulationMayaFur.setDynamicsFlag(dynamics);
}
#endif
#endif
#ifdef INCLUDE_HAIR_BODY_COLLISION_TEST
	shSimulationMayaFur.simulationParameter().includeHairBodyCollision = true;
	(void) shSimulationMayaFur.setSimulationCache(1);
#else
	(void) shSimulationMayaFur.setSimulationCache(1);
	(void) shSimulationMayaFur.setSimulationCache(12);
	(void) shSimulationMayaFur.setSimulationCache(10);
	(void) shSimulationMayaFur.setSimulationCache(24);
	(void) shSimulationMayaFur.setSimulationCache(20);
#endif
}
#endif
#if 1
{
#ifndef INCLUDE_HAIR_BODY_COLLISION_TEST
	MString	fileCacheWriteFilename("D:/zzz/fileCache");
	shSimulationMayaFur.writeToFileCache(fileCacheWriteFilename.asChar());
#endif
}
#endif
} while (0);
#pragma warning(default: 4127)
#endif
}
