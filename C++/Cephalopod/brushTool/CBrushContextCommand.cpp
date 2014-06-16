#include <brushTool/CBrushContextCommand.h>
#include <maya/MArgParser.h>
CBrushContextCommand::CBrushContextCommand()
{
#ifdef move__DEBUG2
	cout << "MPxContextCommand::MPxContextCommand()" << endl;
	cout << "MPxContextCommand::MPxContextCommand(): Thread ID = " << GetCurrentThreadId() << endl;
#endif
}
#if 0
#endif
CBrushContextCommand::~CBrushContextCommand()
{
#ifdef move__DEBUG2
	cout << "MPxContextCommand::~MPxContextCommand()" << endl;
#endif
}
MPxContext* CBrushContextCommand::makeObj()
{
#ifdef move__DEBUG2
	cout << "MPxContextCommand::makeObj()" << endl;
#endif
	__brushContextPtr = new CBrushContext();
	return __brushContextPtr;
}
void*
CBrushContextCommand::creator()
{
#ifdef move__DEBUG2
	cout << "MPxContextCommand::creator()" << endl;
#endif
	return new CBrushContextCommand;
}
MStatus	CBrushContextCommand::doEditFlags()
{
	MStatus	status = MStatus::kSuccess;
	if (!__brushContextPtr)
		return MStatus::kFailure;
	MArgParser	argData = parser();
	if (argData.isFlagSet("-d")) {
		delete __brushContextPtr;
		__brushContextPtr = 0;
		return MStatus::kSuccess;
	}
	if (!__brushContextPtr)
		return MStatus::kFailure;
	if (argData.isFlagSet("-al")) {
		bool	bDir;
		status = argData.getFlagArgument("-al", 0, bDir);
		if (!status) {
			status.perror("|-al| flag parsing failed.");
			return status;
		}
		return __brushContextPtr->attenuateLength(bDir);
	}
	if (argData.isFlagSet("-ci")) {
		unsigned	colorIndex;
		status = argData.getFlagArgument("-ci", 0, colorIndex);
		if (!status) {
			status.perror("|-ci| flag parsing failed.");
			return status;
		}
		__brushContextPtr->getBrushCircle().setColorIndex(colorIndex);
	}
	if (argData.isFlagSet("-r")) {
		double	brushRadius;
		status = argData.getFlagArgument("-r", 0, brushRadius);
		if (!status) {
			status.perror("|-r| flag parsing failed.");
			return status;
		}
		__brushContextPtr->getBrushCircle().setRadius(brushRadius);
	}
	if (argData.isFlagSet("-t")) {
		CBrushToolCommand::BRUSH_TYPE	brushType;
		status = argData.getFlagArgument("-t", 0, brushType);
		if (!status) {
			status.perror("|-t| flag parsing failed.");
			return status;
		}
		return __brushContextPtr->setBrushType(brushType);
	}
	return MStatus::kSuccess;
}
MStatus	CBrushContextCommand::doQueryFlags()
{
	MArgParser argData = parser();
	if (argData.isFlagSet("-d")) {
		setResult((bool) (__brushContextPtr != 0));
		return MStatus::kSuccess;
	}
	if (!__brushContextPtr)
		return MStatus::kFailure;
	if (argData.isFlagSet("-ci")) {
		setResult((int) __brushContextPtr->getBrushCircle().getColorIndex());
	}
	if (argData.isFlagSet("-r")) {
		setResult(__brushContextPtr->getBrushCircle().getRadius());
	}
	if (argData.isFlagSet("-t")) {
		setResult((int) __brushContextPtr->getBrushType());
	}
	return MStatus::kSuccess;
}
MStatus	CBrushContextCommand::appendSyntax()
{
	MSyntax mySyntax = syntax();
	if (MStatus::kSuccess != mySyntax.addFlag("-d", "-delete", MSyntax::kNoArg)) {
		return MStatus::kFailure;
	}
	if (MStatus::kSuccess != mySyntax.addFlag("-al", "-attenuateLength", MSyntax::kBoolean)) {
		return MStatus::kFailure;
	}
	if (MStatus::kSuccess != mySyntax.addFlag("-ci", "-colorIndex", MSyntax::kUnsigned)) {
		return MStatus::kFailure;
	}
	if (MStatus::kSuccess != mySyntax.addFlag("-r", "-radius", MSyntax::kDouble)) {
		return MStatus::kFailure;
	}
	if (MStatus::kSuccess != mySyntax.addFlag("-t", "-type", MSyntax::kLong)) {
		return MStatus::kFailure;
	}
	return MStatus::kSuccess;
}
