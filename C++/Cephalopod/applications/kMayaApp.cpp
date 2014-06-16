#define MLIBRARY_DONTUSE_MFC_MANIFEST
#include "kMayaApp.h"
#include <maya/MLibrary.h>
#include <maya/MString.h>
#include <maya/MGlobal.h>
#include <maya/MFileIO.h>
#include <maya/MFnDependencyNode.h>
#include <baseShape/kUtil.h>
kMayaApp::kMayaApp(
	const char*	argv0,
	const char*	plug_in_shape_nodename,
	const char*	plug_in_name,
	const char*	plug_in_full_pathname_or,
	const char*	scene_filename_or,
	int		verbose_level,
	MStatus*	status_ptr
)
{
	MStatus	ms;
	bool	bFound = false;
#pragma warning(disable: 4127)
do {
	ms = MLibrary::initialize(true, (char*) argv0, true);
	if (!ms) {
		ms.perror("MLibrary::initialize");
		break;
	}
	__flags.initialized = true;
	ms = __plug_in_name.set(plug_in_name);
	if (!ms) {
		ms.perror("MString::set(): Failure");
		break;
	}
	MString	cmd("loadPlugin \"");
	cmd += plug_in_full_pathname_or;
	cmd += "\"";
	ms = MGlobal::executeCommand(cmd);
	if (!ms) {
		MString	str("MGlobal::executeCommand(\"loadPlugin('");
		str += plug_in_full_pathname_or;
		str += "')\"): Faiure";
		ms.perror(str.asChar());
		break;
	}
	__flags.plugin_loaded = true;
	MString	fileName(scene_filename_or);
	ms = MFileIO::open(fileName, NULL, true);
	if (ms) {
		if (verbose_level > 0)
			cout << ">>>> Load Successfull <<<<\n";
	} else {
		cout << ">>>> Load Failed <<<<\n";
		MString	str("MFileIO::open(|");
		str += scene_filename_or;
		str += "|): Failure";
		ms.perror(str.asChar());
		break;
	}
	MSelectionList	list;
	ms = MGlobal::getSelectionListByName("*", list);
	for (unsigned iList = 0; iList < list.length(); ++iList) {
		MDagPath	dagPath;
		MObject		component;
		ms = list.getDagPath(iList, dagPath, component);
		if (dagPath.fullPathName() == "") {
			continue;
		}
		MFnDependencyNode	fnDepNode(dagPath.node());
		MString	typeName = fnDepNode.typeName();
		if (typeName != plug_in_shape_nodename) {
#if 1
	MObject	objField = fnDepNode.object(&ms);
	if (!ms || !objField.hasFn(MFn::kField))
		continue;
	__fieldArray.append(objField);
	if (verbose_level > 0)
		cout << dagPath.fullPathName() << ": |" << typeName << "|" << endl;
#if 0
	if (objField.hasFn(MFn::kAir)) {
		cout << "MFn::kAir" << endl;
		MFnAirField	airField(objField, &ms);
		cout << "\t" << "direction: " << airField.direction(&ms) << endl;
		cout << "\t" << "speed: " << airField.speed(&ms) << endl;
		cout << "\t" << "inheritVelocity: " << airField.inheritVelocity(&ms) << endl;
		cout << "\t" << "inheritRotation: " << airField.inheritRotation(&ms) << endl;
		cout << "\t" << "componentOnly? " << (airField.componentOnly(&ms) ? "True" : "False") << endl;
		cout << "\t" << "spread: " << airField.spread(&ms) << endl;
		cout << "\t" << "enableSpread? " << (airField.enableSpread(&ms) ? "True" : "False") << endl;
	} else if (objField.hasFn(MFn::kDrag)) {
		cout << "MFn::kDrag" << endl;
		MFnDragField	dragField(objField, &ms);
	} else if (objField.hasFn(MFn::kGravity)) {
		cout << "MFn::kGravity" << endl;
		MFnGravityField	gravityField(objField, &ms);
		cout << "\t" << "direction: " << gravityField.direction(&ms) << endl;
	} else if (objField.hasFn(MFn::kNewton)) {
		cout << "MFn::kNewton" << endl;
		MFnNewtonField	newtonField(objField, &ms);
	} else if (objField.hasFn(MFn::kRadial)) {
		cout << "MFn::kRadial" << endl;
		MFnRadialField	radialField(objField, &ms);
	} else if (objField.hasFn(MFn::kTurbulence)) {
		cout << "MFn::kTurbulence" << endl;
		MFnTurbulenceField	turbulenceField(objField, &ms);
	} else if (objField.hasFn(MFn::kUniform)) {
		cout << "MFn::kUniform" << endl;
		MFnUniformField	uniformField(objField);
		cout << "\t" << "direction: " << uniformField.direction(&ms) << endl;
	} else if (objField.hasFn(MFn::kVolumeAxis)) {
		cout << "MFn::kVolumeAxis" << endl;
		MFnVolumeAxisField	volumeAxisField(objField, &ms);
	} else if (objField.hasFn(MFn::kVortex)) {
		cout << "MFn::kVortex" << endl;
		MFnVortexField	vortexField(objField, &ms);
	}
#endif
#endif
			continue;
		}
		__node = dagPath.node();
		ms = kNode::findExternalDagPath(dagPath.node(), __body);
		if (!ms) {
			ms.perror("findExternalDagPath()");
			continue;
		}
		if (verbose_level > 0)
			cout << "INFO: list[" << iList << "]: " << __body.fullPathName() << endl;
		bFound = true;
		ms = MStatus::kSuccess;
		break;
	}
} while (0);
#pragma warning(default: 4127)
	if (!bFound)
		ms = MStatus::kNotFound;
	if (status_ptr)
		*status_ptr = ms;
}
kMayaApp::~kMayaApp()
{
	if (!__flags.initialized)
		return;
	MStatus	ms;
	ms = MFileIO::newFile(true);
	if (!ms) {
		ms.perror("MFileIO::newFile(true)");
	}
	if (__flags.plugin_loaded) {
		MString	cmd = "unloadPlugin ";
		cmd += __plug_in_name;
		ms = MGlobal::executeCommand(cmd);
		if (!ms) {
			ms.perror("MGlobal::executeCommand(unloadPlugin): Failure");
		}
	}
	MLibrary::cleanup();
}
MStatus	kMayaApp::getSetting(Setting& setting, int verbose_level)
{
	MStatus	ms;
#pragma warning(disable: 4127)
do {
	MString	timeUnit;
	ms = MGlobal::executeCommand("currentUnit -query -time", timeUnit);
	if (!ms) {
		cerr << "MGlobal::executeCommand('currentUnit -query -time'): Failure" << endl;
		break;
	}
	if (verbose_level > 0)
		cout << "timeUnit: |" << timeUnit << "|" << endl;
	int&	fps = setting.fps;
	if (timeUnit == "game")
		fps = 15;
	else if (timeUnit == "film")
		fps = 24;
	else if (timeUnit == "pal")
		fps = 25;
	else if (timeUnit == "ntsc")
		fps = 30;
	else if (timeUnit == "show")
		fps = 48;
	else if (timeUnit == "palf")
		fps = 50;
	else if (timeUnit == "ntscf")
		fps = 60;
	else {
		unsigned	len = timeUnit.length();
		const char*	timeUnit_str = timeUnit.asChar();
		if (
			len > 3 &&
			(timeUnit_str[len - 3] == 'f' && timeUnit_str[len - 2] == 'p' && timeUnit_str[len - 1] == 's')
		) {
			if (verbose_level > 0)
				cout << "sscanf = " << sscanf_s(timeUnit.asChar(), "%dfps", &fps) << endl;
		}
	}
	if (verbose_level > 0)
		cout << "fps = " << fps << endl;
	double	val;
	ms = MGlobal::executeCommand("playbackOptions -query -minTime", val);
	if (!ms) {
		cerr << "MGlobal::executeCommand('playbackOptions -query -minTime'): Failure" << endl;
		break;
	}
	setting.minTime = val;
	if (verbose_level > 0)
		cout << "minTime: |" << val << "|" << endl;
	ms = MGlobal::executeCommand("playbackOptions -query -maxTime", val);
	if (!ms) {
		cerr << "MGlobal::executeCommand('playbackOptions -query -maxTime'): Failure" << endl;
		break;
	}
	setting.maxTime = val;
	if (verbose_level > 0)
		cout << "maxTime: |" << val << "|" << endl;
	ms = MGlobal::executeCommand("playbackOptions -query -animationStartTime", val);
	if (!ms) {
		cerr << "MGlobal::executeCommand('playbackOptions -query -animationStartTime'): Failure" << endl;
		break;
	}
	setting.animationStartTime = val;
	if (verbose_level > 0)
		cout << "animationStartTime: |" << val << "|" << endl;
	ms = MGlobal::executeCommand("playbackOptions -query -animationEndTime", val);
	if (!ms) {
		cerr << "MGlobal::executeCommand('playbackOptions -query -animationEndTime'): Failure" << endl;
		break;
	}
	setting.animationEndTime = val;
	if (verbose_level > 0)
		cout << "animationEndTime: |" << val << "|" << endl;
	ms = MGlobal::executeCommand("playbackOptions -query -by", val);
	if (!ms) {
		cerr << "MGlobal::executeCommand('playbackOptions -query -by'): Failure" << endl;
		break;
	}
	setting.by = val;
	if (verbose_level > 0)
		cout << "by: |" << val << "|" << endl;
} while (0);
#pragma warning(default: 4127)
	return ms;
}
