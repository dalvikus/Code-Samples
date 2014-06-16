#include <iostream>
	using std::cout;
	using std::cerr;
	using std::endl;
#define NT_PLUGIN
#define REQUIRE_IOSTREAM
#include "kMayaAppFur.h"
#include <maya/MFnDependencyNode.h>
#include <maya/MPlug.h>
#include <maya/MFnPluginData.h>
#include <afterShave/afterShaveData.h>
kMayaAppFur::kMayaAppFur(
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
kMayaAppFur::~kMayaAppFur()
{
}
MStatus	kMayaAppFur::set()
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
#if 1
#pragma warning(disable: 4127)
do {
	MPlug	plug_controlPoints = fnDepNode.findPlug("controlPoints", true, &ms);
	if (!ms) {
		ms.perror("MFnDependencyNode::findPlug(|controlPoints|): Failure");
		break;
	}
#if 1
	bool	isArray = plug_controlPoints.isArray(&ms);
	if (!ms) {
		ms.perror("MPlug::isArray(): Failure");
		break;
	}
	if (!isArray) {
		cerr << ".controlPoints: not an array" << endl;
		break;
	}
	unsigned	numElements = plug_controlPoints.numElements(&ms);
	if (!ms) {
		ms.perror("MPlug::numElements(): Failure");
		break;
	}
	ms = __cp.setLength(numElements);
	if (!ms) {
		ms.perror("MVectorArray::setLength(): Failure");
		break;
	}
	for (unsigned n = 0; n < numElements; ++n) {
		MPlug	plug = plug_controlPoints.elementByPhysicalIndex(n, &ms);
		if (!ms) {
			ms.perror("MPlug::elementByPhysicalIndex(): Failure");
			continue;
		}
		bool	isCompound = plug.isCompound(&ms);
		if (!ms) {
			ms.perror("MPlug::isCompound(): Failure");
			continue;
		}
		if (!isCompound) {
			cerr << "not a compound" << endl;
			continue;
		}
		unsigned	numChildren = plug.numChildren(&ms);
		if (!ms) {
			ms.perror("MPlug::numChildren(): Failure");
			continue;
		}
		if (numChildren != 3) {
			cerr << "# of children != 3" << endl;
			continue;
		}
		MVector&	v = __cp[n];
		MPlug	plug_x = plug.child(0, &ms); if (!ms) {ms.perror("MPlug::child(): Failure"); continue;}
		ms = plug_x.getValue(v.x); if (!ms) {ms.perror("MPlug::getValue(double&): Failure"); continue;}
		MPlug	plug_y = plug.child(1, &ms); if (!ms) {ms.perror("MPlug::child(): Failure"); continue;}
		ms = plug_y.getValue(v.y); if (!ms) {ms.perror("MPlug::getValue(double&): Failure"); continue;}
		MPlug	plug_z = plug.child(2, &ms); if (!ms) {ms.perror("MPlug::child(): Failure"); continue;}
		ms = plug_z.getValue(v.z); if (!ms) {ms.perror("MPlug::getValue(double&): Failure"); continue;}
	}
#endif
} while (0);
#pragma warning(default: 4127)
#endif
#if 1
#pragma warning(disable: 4127)
do {
	MPlug	plug_outputSurface = fnDepNode.findPlug("outputSurface", true, &ms);
	if (!ms) {
		ms.perror("MFnDependencyNode::findPlug(|outputSurface|): Failure");
		break;
	}
	cout << ".outputSurface: ok" << endl;
	MObject	data;
	ms = plug_outputSurface.getValue(data);
	if (!ms) {
		ms.perror("MPlug::getValue(MObject&): Failure");
		break;
	}
	MFnPluginData	fnData(data, &ms);
	if (!ms) {
		ms.perror("MFnPluginData::MFnPluginData(MObject): Failure");
		break;
	}
	afterShaveData*	afterShave_data = (afterShaveData*) fnData.data(&ms);
	if (!ms) {
		ms.perror("MFnPluginData::data(): Failure");
		break;
	}
	__geometry = *afterShave_data->fGeometry;
} while (0);
#pragma warning(default: 4127)
#endif
	return MStatus::kSuccess;
}
