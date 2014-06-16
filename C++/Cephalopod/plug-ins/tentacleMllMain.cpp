#include <maya/MFnPlugin.h>
#include <baseShape/baseShapeData.h>
#include <baseShape/baseShapeCreator.h>
#if 1
#include <tentacleShape/tentacleShape.h>
#include <tentacleShape/tentacleShapeUI.h>
#else
#include <baseShape/baseShape.h>
#include <baseShape/baseShapeUI.h>
#endif
#include <brushTool/CBrushToolCommand.h>
#include <brushTool/CBrushContextCommand.h>
#define CREATOR_NODE_NAME	BASE_SHAPE__CREATOR_NODE_NAME
#define DATA_NAME		BASE_SHAPE__DATA_NAME
#if 1
#define SHAPE_NODE_NAME		TENTACLE__SHAPE_NODE_NAME
#define CLASS_NAME		"tentacle"
#else
#define SHAPE_NODE_NAME		BASE__SHAPE_NODE_NAME
#define CLASS_NAME		"baseShape"
#endif
#define VENDOR_NAME		"DN2 Company: " ## CLASS_NAME
#define VERSION			"0.1"
#define REQUIRED_API_VERSION	"Any"
#include <baseShape/arrowLocator.h>
#include <maya/MMessage.h>
MStatus	initializePlugin(MObject obj)
{
	MString	classification(CLASS_NAME);
	MStatus	ms;
	MFnPlugin	plugin(obj, VENDOR_NAME, VERSION, REQUIRED_API_VERSION, &ms);
	if (!ms) {
		cerr << "Error: initializePlugin: ";
		cerr << "MFnPlugin(|" << REQUIRED_API_VERSION << "|): Failure: ";
		cerr << ms << endl;
		return ms;
	}
	ms = plugin.registerContextCommand(
		BRUSH__CONTEXT_COMMAND_NAME, CBrushContextCommand::creator,
		BRUSH__TOOL_COMMAND_NAME, CBrushToolCommand::creator
	);
	if (!ms) {
		cerr << "MFnPlugin::registerContextCommand(|" << BRUSH__CONTEXT_COMMAND_NAME << "|, |" << BRUSH__TOOL_COMMAND_NAME << "|): Failure: " << ms << endl;
		return ms;
	}
	ms = plugin.registerData(
		BASE_SHAPE__DATA_NAME,
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
		SHAPE_NODE_NAME,
		baseShape::id,
#if 1
		&tentacleShape::creator,
		&tentacleShape::initialize,
		&tentacleShapeUI::creator,
#else
		&baseShape::creator,
		&baseShape::initialize,
		&baseShapeUI::creator,
#endif
		&classification
	);
	if (!ms) {
		cerr << "Error: initializePlugin: ";
		cerr << "MFnPlugin::registerShape(" << SHAPE_NODE_NAME << "|): Failure: ";
		cerr << ms << endl;
		return ms;
	}
	ms = plugin.registerNode(
		BASE_SHAPE__CREATOR_NODE_NAME,
		baseShapeCreator::id,
		&baseShapeCreator::creator,
		&baseShapeCreator::initialize,
		MPxNode::kDependNode,
		&classification
	);
	if (!ms) {
		cerr << "Error: initializePlugin: ";
		cerr << "MFnPlugin::registerNode(" << BASE_SHAPE__CREATOR_NODE_NAME << "|): Failure: ";
		cerr << ms << endl;
		return ms;
	}
	ms = plugin.registerNode(
		SIMULATION_PARAMETER_ARROW_LOCATOR_NAME,
		arrowLocator::id,
		&arrowLocator::creator,
		&arrowLocator::initialize,
		MPxNode::kLocatorNode
	);
	if (!ms) {
		cerr << "Error: initializePlugin: ";
		cerr << "MFnPlugin::registerNode(" << SIMULATION_PARAMETER_ARROW_LOCATOR_NAME << "|): Failure: ";
		cerr << ms << endl;
		return ms;
	}
	return MStatus::kSuccess;
}
MStatus	uninitializePlugin(MObject obj)
{
	MStatus	ms;
	MFnPlugin	plugin(obj, VENDOR_NAME, VERSION, REQUIRED_API_VERSION, &ms);
	if (!ms) {
		cerr << "Error: uninitializePlugin: ";
		cerr << "MFnPlugin(|Any|): Failure: ";
		cerr << ms << endl;
		return ms;
	}
	ms = plugin.deregisterContextCommand(
		BRUSH__CONTEXT_COMMAND_NAME, BRUSH__TOOL_COMMAND_NAME
	);
	if (!ms) {
		cerr << "MFnPlugin::deregisterContextCommand(|" << BRUSH__CONTEXT_COMMAND_NAME << "|, |" << BRUSH__TOOL_COMMAND_NAME << "|): Failure: " << ms << endl;
		return ms;
	}
	ms = plugin.deregisterNode(baseShape::id);
	if (!ms) {
		cerr << "Error: uninitializePlugin: ";
		cerr << "MFnPlugin::deregisterNode(|" << SHAPE_NODE_NAME << "|): Failure: ";
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
		cerr << "MFnPlugin::deregisterNode(|" << BASE_SHAPE__CREATOR_NODE_NAME << "|): Failure: ";
		cerr << ms << endl;
		return ms;
	}
	ms = plugin.deregisterNode(arrowLocator::id);
	if (!ms) {
		cerr << "Error: uninitializePlugin: ";
		cerr << "MFnPlugin::deregisterNode(|" << SIMULATION_PARAMETER_ARROW_LOCATOR_NAME << "|): Failure: ";
		cerr << ms << endl;
		return ms;
	}
	return MStatus::kSuccess;
}
