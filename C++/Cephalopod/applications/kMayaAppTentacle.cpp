#include "kMayaAppTentacle.h"
#include <maya/MFnDependencyNode.h>
#include <maya/MPlug.h>
#include <maya/MTime.h>
#include <tentacleShape/tentacleShape.h>
#include <baseShape/kUtil.h>
#include <simulation/kBodyMayaTentacle.h>
kMayaAppTentacle::kMayaAppTentacle(
	const char*	argv0,
	const char*	plug_in_shape_nodename,
	const char*	plug_in_name,
	const char*	plug_in_pathname_or,
	const char*	scene_filename_or,
	int		verbose_level,
	MStatus*	status_ptr
) :
	kMayaApp(
		argv0,
		plug_in_shape_nodename,
		plug_in_name, plug_in_pathname_or,
		scene_filename_or,
		verbose_level, status_ptr
	)
{
}
kMayaAppTentacle::~kMayaAppTentacle()
{
}
#if 1
MStatus	kMayaAppTentacle::set0(int verbose_level)
{
#if 1
{
	MStatus	ms;
	MFnDependencyNode	fnDepNode(__node, &ms);
	if (!ms) {
		ms.perror("MFnDependencyNode::MFnDependencyNode(MObject&): Failure");
		return ms;
	}
	MPlug	plug_tentacleArray = fnDepNode.findPlug("tentacleArray", true, &ms);
	if (!ms) {
		ms.perror("MFnDependencyNode::findPlug(|tentacleArray|): Failure");
		return ms;
	}
	ms = __skinSmoothBind.set(__body, plug_tentacleArray);
}
#endif
	return kBodyMayaTentacle::set0(__node, __tentacleArray0, __T0_in_kSpace_array, verbose_level);
}
#else
MStatus	kMayaAppTentacle::__set(MPlug& plug_tentacleArray, int verbose_level)
{
	MStatus	ms;
	__T0_in_kSpace_array.clear();
#if 1
#if 1
	unsigned	numElements = plug_tentacleArray.evaluateNumElements(&ms);
	if (verbose_level > 1)
		cout << "INFO: # of tentacles = " << numElements << endl;
	ms = tentacleShape::getTentacleArray(plug_tentacleArray, numElements, __tentacleArray0);
	if (!ms) {
		ms.perror("tentacleShape::getTentacleArray(const MPlug&)");
		return ms;
	}
	for (int n = 0; n < (int) __tentacleArray0.length(); ++n) {
		const kTentacle&	tentacle = __tentacleArray0(n);
		unsigned	numStrandSamplePoints = tentacle.transversal.length();
		if (verbose_level > 1) {
			cout << "# of sample points in strand = " << numStrandSamplePoints << endl;
			cout << tentacle << endl;
		}
	}
#endif
#endif
#if 1
#if 1
	for (int n = 0; n < (int) __tentacleArray0.length(); ++n) {
		const kTentacle&	tentacle = __tentacleArray0(n);
#if 1
		ShVector9	T0_in_kObject;
		if (tentacle.getT0_in_kObject(T0_in_kObject) < 0) {
			cerr << "kTentacle::getT0_in_kObject(ShVector9&): Failure" << endl;
			continue;
		}
		ShVector9*	p = new ShVector9;
		*p = T0_in_kObject;
		__T0_in_kSpace_array.append(p);
#if 0
		kTime	kT(timeUnit, 1, 1);
		for (int nthFrame = 1; nthFrame <= 2; ++nthFrame) {
			for (int nthStep = 1; nthStep <= 10; ++nthStep) {
				kBody::TVector	inStart;
				if (mayaBody.getTVector(kT, T0_in_kObject, inStart) < 0) {
					cerr << "MayaBody::getTVector(kTime&,...): Failure" << endl;
					continue;
				}
				cout << "(" << kT.nthFrame() << ", " << kT.nthStep() << "): t = " << kT() << endl;
				cout << "\tT0.position: [";
				cout << inStart.T0.position[0] << ", " << inStart.T0.position[1] << ", " << inStart.T0.position[2];
				cout << "]" << endl;
				cout << "\tT0.tangent: [";
				cout << inStart.T0.tangent[0] << ", " << inStart.T0.tangent[1] << ", " << inStart.T0.tangent[2];
				cout << "]" << endl;
				cout << "\tT0.normal: [";
				cout << inStart.T0.normal[0] << ", " << inStart.T0.normal[1] << ", " << inStart.T0.normal[2];
				cout << "]" << endl;
				cout << "\tT1.position: [";
				cout << inStart.T1.position[0] << ", " << inStart.T1.position[1] << ", " << inStart.T1.position[2];
				cout << "]" << endl;
				cout << "\tT1.tangent: [";
				cout << inStart.T1.tangent[0] << ", " << inStart.T1.tangent[1] << ", " << inStart.T1.tangent[2];
				cout << "]" << endl;
				cout << "\tT2.position: [";
				cout << inStart.T2.position[0] << ", " << inStart.T2.position[1] << ", " << inStart.T2.position[2];
				cout << "]" << endl;
				cout << "\tT2.tangent: [";
				cout << inStart.T2.tangent[0] << ", " << inStart.T2.tangent[1] << ", " << inStart.T2.tangent[2];
				cout << "]" << endl;
				++kT;
			}
		}
#endif
#endif
	}
#endif
#endif
	return MStatus::kSuccess;
}
MStatus	kMayaAppTentacle::set(int verbose_level)
{
	if (__node.isNull()) {
		cerr << "__node not set" << endl;
		return MStatus::kInvalidParameter;
	}
	MStatus	ms;
	MFnDependencyNode	fnDepNode(__node, &ms);
	if (!ms) {
		ms.perror("MFnDependencyNode::MFnDependencyNode(MObject&): Failure");
		return ms;
	}
	MPlug	plug_tentacleArray = fnDepNode.findPlug("tentacleArray", true, &ms);
	if (!ms) {
		ms.perror("MFnDependencyNode::findPlug(|tentacleArray|): Failure");
		return ms;
	}
	return __set(plug_tentacleArray, verbose_level);
}
#endif
MStatus	kMayaAppTentacle::getSamplePoints(
	double&	t,
	int tentacle_index,
	MPointArray&	strandSamplePoints,
	unsigned	numEllipseSamplePoints,
	MPointArray&	tentacleSamplePoints
) const
{
	if (tentacle_index < 0 || tentacle_index >= (int) __tentacleArray0.length()) {
		cerr << "ERROR: kMayaAppTentacle::getSamplePoints: tentacle_index = " << tentacle_index << ": out of range" << endl;
		return MStatus::kFailure;
	}
	MStatus	ms;
	MTime	time;
	ms = time.setUnit(MTime::kSeconds);
	if (!ms) {
		ms.perror("MTime::setUnit(MTime::kSeconds): Failure");
		return ms;
	}
	ms = time.setValue(t);
	if (!ms) {
		ms.perror("MTime::setValue(): Failure");
		return ms;
	}
	t = time.value();
#if 1
	if (__skinSmoothBind.hasSkinCluster()) {
		const kTentacle&	tentacle = __tentacleArray0(tentacle_index);
		MFnDependencyNode	fnDepNode(__node, &ms);
		if (!ms) {
			ms.perror("MFnDependencyNode::MFnDependencyNode(MObject&): Failure");
			return ms;
		}
		MPlug	plug_tentacleArray = fnDepNode.findPlug("tentacleArray", true, &ms);
		if (!ms) {
			ms.perror("MFnDependencyNode::findPlug(|tentacleArray|)");
			return ms;
		}
		MPlug	plug = plug_tentacleArray.elementByPhysicalIndex(tentacle_index, &ms);
		float	segment_length = (float) tentacle.longitudinal[0].length();
		kTentacle&	nonConst_tentacle = const_cast<kTentacle&>(tentacle);
		ms = __skinSmoothBind.getPoints(tentacle_index, plug, segment_length, nonConst_tentacle.wNormal, nonConst_tentacle.wPoints);
		ms = tentacle.getSamplePoints(strandSamplePoints, numEllipseSamplePoints, tentacleSamplePoints);
		return ms;
	}
#endif
	MMatrix	worldMatrix;
	ms = kUtil::getWorldMatrixAt(__body, time, worldMatrix, NULL);
	if (!ms) {
		ms.perror("kUtil::getWorldMatrixAt(): Failure");
		return ms;
	}
	const kTentacle&	tentacle = __tentacleArray0(tentacle_index);
	ms = tentacle.getSamplePoints(worldMatrix, strandSamplePoints, numEllipseSamplePoints, tentacleSamplePoints);
	if (!ms) {
		ms.perror("kTentacle::getSamplePoints(): Failure");
	}
	return ms;
}
