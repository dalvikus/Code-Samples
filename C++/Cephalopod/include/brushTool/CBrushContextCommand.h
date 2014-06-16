#pragma once
#include <maya/MPxContextCommand.h>
#include <brushTool/CBrushContext.h>
#define BRUSH__CONTEXT_COMMAND_NAME	"brushContextCommand"
class CBrushContextCommand : public MPxContextCommand
{
public:
	CBrushContextCommand();
	virtual ~CBrushContextCommand();
	virtual MPxContext*	makeObj();
	static	void*		creator();
	virtual MStatus		doEditFlags();
	virtual MStatus		doQueryFlags();
	virtual MStatus		appendSyntax();
protected:
	CBrushContext*		__brushContextPtr;
};
