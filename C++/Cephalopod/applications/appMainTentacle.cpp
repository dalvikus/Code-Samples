#include "kMayaAppTentacle.h"
#include <maya/MPointArray.h>
#if 1
#include <simulation/kTime.h>
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
#include <baseShape/baseShape.h>
#endif
int	main(int , char* argv[])
{
	MStatus	ms;
	int	verbose_level = 1;
	kMayaAppTentacle	mayaAppTentacle(
		argv[0],
		"tentacleShapeNode",
		"tentacle",
		"debug/tentacle.mll",
		"skinnedMovingTentacle2.ma",
		verbose_level,
		&ms
	);
	if (!ms) {
		ms.perror("kMayaApp::kMayaApp()");
		return 1;
	}
	ms = mayaAppTentacle.set0(verbose_level);
	if (!ms) {
		ms.perror("kMayaAppTentacle::set(): Failure");
		return 1;
	}
	const kPtrArray<kTentacle>&	tentacleArray0 = mayaAppTentacle.tentacleArray0();
	cout << "# of tentacleArray0 = " << tentacleArray0.length() << endl;
	const kPtrArray<ShVector9>&	T0_in_kObject_array = mayaAppTentacle.T0_in_kObject_array();
	cout << "# of T0_in_kObject_array = " << T0_in_kObject_array.length() << endl;
	double	t0 = 0.091;
	double	t = t0;
	MPointArray	strandSamplePoints;
	unsigned	numEllipseSamplePoints = 10;
	MPointArray	tentacleSamplePoints;
	int	tentacle_index = 1;
	ms = mayaAppTentacle.getSamplePoints(t, tentacle_index, strandSamplePoints, numEllipseSamplePoints, tentacleSamplePoints);
	if (!ms) {
		ms.perror("kMayaAppTentacle::getSamplePoints(): Failure");
	} else {
		cout << "strandSamplePoints: " << strandSamplePoints << endl;
		cout << "t0 = " << t0 << ", t = " << t << endl;
	}
#if 1
#pragma warning(disable: 4127)
do {
#if 1
	kShSimulationMayaTentacle	shSimulationMayaTentacle(kTime::kTimeUnitFilm10, 24, false);
#ifdef _INHERIT_kBODY
	kBodyMayaTentacle&	bodyMayaTentacle = shSimulationMayaTentacle;
#else
	kBodyMayaTentacle&	bodyMayaTentacle = *((kBodyMayaTentacle*) shSimulationMayaTentacle.bodyMayaPtr());
#endif
	if (&bodyMayaTentacle) {
		ms = bodyMayaTentacle.setBody(mayaAppTentacle.body());
		ms = bodyMayaTentacle.set0(mayaAppTentacle.node());
	} else {
		cerr << "bodyMayaTentaclePtr is nil" << endl;
		break;
	}
#endif
#if 1
{
#if 1
	MStatus	ms;
	MFnDependencyNode	fnDepNode(mayaAppTentacle.node(), &ms);
	if (!ms) {
		ms.perror("MFnDependencyNode::MFnDependencyNode(MObject&): Failure");
		return ms;
	}
	MPlug	plug_tentacleArray = fnDepNode.findPlug("tentacleArray", true, &ms);
	if (!ms) {
		ms.perror("MFnDependencyNode::findPlug(|tentacleArray|): Failure");
		return ms;
	}
#endif
	const kPtrArray<kTentacle>&	tentacleArray0 = mayaAppTentacle.tentacleArray0();
	std::vector<int>	strandIdArray;
	for (int strandId = 0; strandId < (int) tentacleArray0.length(); ++strandId)
		strandIdArray.push_back(strandId);
	for (int indexStrand = 0; indexStrand < (int) strandIdArray.size(); ++indexStrand) {
		int	strandId = strandIdArray.at(indexStrand);
		const kTentacle&	tentacle = tentacleArray0(strandId);
		if (tentacle.longitudinal.length() < 2) {
			cerr << "FATAL: kTentacle::longitudinal: no segment" << endl;
			continue;
		}
		const MVector&	longiVector0 = tentacle.longitudinal[0];
		double	strandLength = (tentacle.longitudinal.length() - 1) * longiVector0.length();
		if (shSimulationMayaTentacle.addStrand(strandId, DEFAULT_SEGMENT_COUNT, strandLength) == NULL) {
			cerr << "Error: kSimulationCache::addStrand(): Failure" << endl;
			continue;
		}
#if 1
		cout << "초기 모양 반영: indexStrand = " << indexStrand << ", strandId = " << strandId << endl;
		MPointArray	strandSamplePoints;
		MPointArray	tentacleSamplePoints;
#if 1
		const kSkinSmoothBind&	skinSmoothBind = mayaAppTentacle.skinSmoothBind();
		if (skinSmoothBind.hasSkinCluster()) {
			float	segment_length = (float) tentacle.longitudinal[0].length();
			MPlug	plug = plug_tentacleArray.elementByPhysicalIndex(strandId, &ms);
			kTentacle&	nonConst_tentacle = const_cast<kTentacle&>(tentacle);
			ms = skinSmoothBind.getPoints(strandId, plug, segment_length, nonConst_tentacle.wNormal, nonConst_tentacle.wPoints);
			tentacle.getSamplePoints(strandSamplePoints, numEllipseSamplePoints, tentacleSamplePoints);
		} else {
#endif
		tentacle.getSamplePoints(bodyMayaTentacle.inclusiveMatrix(), strandSamplePoints, numEllipseSamplePoints, tentacleSamplePoints);
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
		if (shSimulationMayaTentacle.setInitialCurvatureVector(indexStrand, NUM_TENTACLE_SAMPLE_POINTS, samplePoints) < 0) {
			cerr << "Error: kSimulationCache::setInitialCurvatureVector(): Failure" << endl;
		}
#endif
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
	ShSimParameter&	simulationParameter = shSimulationMayaTentacle.simulationParameter();
#ifdef _DEBUG
	baseShape::printSimulationParameter(simulationParameter);
#endif
	MString	fileName("D:/home/icentrik/My Documents/maya/projects/default/scenes/skinnedMovingTentacle.kps");
	if (simulationParameter.fileReadEveryRecord((char*) fileName.asChar()) < 0) {
		cerr << "ShSimParameter::fileReadEveryRecord(): Failure" << endl;
		break;
	} else {
		baseShape::printSimulationParameter(simulationParameter);
	}
#ifdef _DEBUG
	baseShape::printSimulationParameter(simulationParameter);
#endif
}
#endif
#if 1
{
#if 1
#if 1
	shSimulationMayaTentacle.setDynamicsFlag(true);
#else
{
	MFnDependencyNode	fnDepNode(mayaAppTentacle.node(), &ms);
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
	shSimulationMayaTentacle.setDynamicsFlag(dynamics);
}
#endif
#endif
#ifdef INCLUDE_HAIR_BODY_COLLISION_TEST
	shSimulationMayaTentacle.simulationParameter().includeHairBodyCollision = true;
	(void) shSimulationMayaTentacle.setSimulationCache(1);
#else
	(void) shSimulationMayaTentacle.setSimulationCache(1);
	(void) shSimulationMayaTentacle.setSimulationCache(12);
	(void) shSimulationMayaTentacle.setSimulationCache(10);
	(void) shSimulationMayaTentacle.setSimulationCache(24);
	(void) shSimulationMayaTentacle.setSimulationCache(20);
#endif
}
#endif
#if 1
{
#ifndef INCLUDE_HAIR_BODY_COLLISION_TEST
	MString	fileCacheWriteFilename("D:/zzz/fileCache");
	shSimulationMayaTentacle.writeToFileCache(fileCacheWriteFilename.asChar());
#endif
}
#endif
} while (0);
#pragma warning(default: 4127)
#endif
	return 0;
}
