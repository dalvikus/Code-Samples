#include <simulation/kBodyMayaTentacle.h>
#include <baseShape/kPtrArray.h>
#include <maya/MPlug.h>
#include <tentacleShape/tentacleShape.h>
#if 1
MStatus	kBodyMayaTentacle::set0(MObject& node)
{
#if 1
{
	MStatus	ms;
	MFnDependencyNode	fnDepNode(node, &ms);
	if (!ms) {
		ms.perror("MFnDependencyNode::MFnDependencyNode(MObject&): Failure");
		return ms;
	}
	MPlug	plug_tentacleArray = fnDepNode.findPlug("tentacleArray", true, &ms);
	if (!ms) {
		ms.perror("MFnDependencyNode::findPlug(|tentacleArray|): Failure");
		return ms;
	}
	ms = __skinSmoothBind.set(*this, plug_tentacleArray);
}
#endif
	int	verbose_level = 0;
	return kBodyMayaTentacle::set0(node, __tentacleArray0, __T0_in_kSpace_array, verbose_level);
}
#else
void	kBodyMayaTentacle::set__T0_in_kSpace_array(const kPtrArray<ShVector9>& T0_in_kObject_array)
{
	__T0_in_kSpace_array.clear();
	for (unsigned i = 0; i < T0_in_kObject_array.length(); ++i) {
		const ShVector9&	T0_in_kObject__src = T0_in_kObject_array(i);
		ShVector9*	p = new ShVector9;
		__T0_in_kSpace_array.append(p);
		ShVector9&	T0_in_kObject = *p;
		T0_in_kObject.position[0] = T0_in_kObject__src.position[0];
		T0_in_kObject.position[1] = T0_in_kObject__src.position[1];
		T0_in_kObject.position[2] = T0_in_kObject__src.position[2];
		T0_in_kObject.tangent[0] = T0_in_kObject__src.tangent[0];
		T0_in_kObject.tangent[1] = T0_in_kObject__src.tangent[1];
		T0_in_kObject.tangent[2] = T0_in_kObject__src.tangent[2];
		T0_in_kObject.normal[0] = T0_in_kObject__src.normal[0];
		T0_in_kObject.normal[1] = T0_in_kObject__src.normal[1];
		T0_in_kObject.normal[2] = T0_in_kObject__src.normal[2];
	}
}
#endif
#if 1
MStatus	kBodyMayaTentacle::set0(
	MObject&	node,
	kPtrArray<kTentacle>&	tentacleArray0,
	kPtrArray<ShVector9>&	T0_in_kSpace_array,
	int	verbose_level
)
{
	if (node.isNull()) {
		cerr << "node not set" << endl;
		return MStatus::kInvalidParameter;
	}
	MStatus	ms;
	MFnDependencyNode	fnDepNode(node, &ms);
	if (!ms) {
		ms.perror("MFnDependencyNode::MFnDependencyNode(MObject&): Failure");
		return ms;
	}
	MPlug	plug_tentacleArray = fnDepNode.findPlug("tentacleArray", true, &ms);
	if (!ms) {
		ms.perror("MFnDependencyNode::findPlug(|tentacleArray|): Failure");
		return ms;
	}
	return kBodyMayaTentacle::set0(plug_tentacleArray, tentacleArray0, T0_in_kSpace_array, verbose_level);
}
MStatus	kBodyMayaTentacle::set0(
	MPlug&	plug_tentacleArray,
	kPtrArray<kTentacle>&	tentacleArray0,
	kPtrArray<ShVector9>&	T0_in_kSpace_array,
	int	verbose_level
)
{
	MStatus	ms;
	T0_in_kSpace_array.clear();
#if 1
#if 1
	unsigned	numElements = plug_tentacleArray.evaluateNumElements(&ms);
	if (verbose_level > 1)
		cout << "INFO: # of tentacles = " << numElements << endl;
	ms = tentacleShape::getTentacleArray(plug_tentacleArray, numElements, tentacleArray0);
	if (!ms) {
		ms.perror("tentacleShape::getTentacleArray(const MPlug&)");
		return ms;
	}
	for (int n = 0; n < (int) tentacleArray0.length(); ++n) {
		const kTentacle&	tentacle = tentacleArray0(n);
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
	for (int n = 0; n < (int) tentacleArray0.length(); ++n) {
		const kTentacle&	tentacle = tentacleArray0(n);
		ShVector9	T0_in_kObject;
		if (tentacle.getT0_in_kObject(T0_in_kObject) < 0) {
			cerr << "kTentacle::getT0_in_kObject(ShVector9&): Failure" << endl;
			continue;
		}
		ShVector9*	p = new ShVector9;
		*p = T0_in_kObject;
		T0_in_kSpace_array.append(p);
	}
#endif
#endif
	return MStatus::kSuccess;
}
#endif
