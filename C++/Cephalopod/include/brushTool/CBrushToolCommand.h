#pragma once
#include <maya/MPxToolCommand.h>
#include <brushTool/CSelection.h>
#define BRUSH__TOOL_COMMAND_NAME	"brushToolCommand"
class CBrushToolCommand : MPxToolCommand
{
public:
	enum {
		BT_INVALID	= -1,
#if 1
		BT_TRANSLATE	= 0,
		BT_ROTATE,
#else
		BT_CUT		= 0,
		BT_SCALE,
		BT_TRANSLATE,
		BT_ROTATE,
#endif
#if 1
#if 0
		BT_TENTACLE_TWIST,
		BT_TENTACLE_SHAPE,
#endif
#if 1
		BT_TENTACLE_CREATE,
		BT_TENTACLE_MOVE,
#endif
#endif
	};
	typedef int		BRUSH_TYPE;
	static BRUSH_TYPE	BrushType;
	static void*	creator();
protected:
	SelectionArrayPtr	__activeSelectionArrayPtr;
public:
	void	setActiveSelectionArrayPtr(SelectionArrayPtr activeSelectionArrayPtr)	{__activeSelectionArrayPtr = activeSelectionArrayPtr;}
protected:
	enum {
		DOIT = 0,
		UNDOIT,
		REDOIT
	};
public:
	CBrushToolCommand() : __activeSelectionArrayPtr(0) {}
	virtual ~CBrushToolCommand() {}
	virtual MStatus		doIt(const MArgList&)	{return action(DOIT, true);}
	virtual bool		isUndoable() const		{return true;}
	virtual MStatus		redoIt()			{return action(REDOIT, true);}
	MStatus			redoIt(bool)	{return action(REDOIT, false);}
	virtual MStatus		undoIt()			{return action(UNDOIT, true);}
	MStatus			undoIt(bool)	{return action(UNDOIT, false);}
	virtual MStatus		finalize();
#if 0
	MStatus		parseArgs(const MArgList& args);
	static MSyntax	newSyntax();
#endif
private:
	virtual MStatus		action(int flag, bool byMaya);
public:
	virtual MStatus		rebuildActiveSelectionList(bool undo) = 0;
};
#if 0
class CBrushToolCommand_Cut : CBrushToolCommand
{
public:
	CBrushToolCommand_Cut() {}
	virtual ~CBrushToolCommand_Cut() {}
private:
	ToolCommandInside<Arg_Cut, Data_Cut>	__blobCut;
public:
	const InsideArray_Cut&		getInsideArray() const		{return __blobCut.insideArray();}
	InsideArray_Cut&		getInsideArray()		{return __blobCut.insideArray();}
private:
	virtual MStatus		action(int flag, bool byMaya);
public:
	virtual MStatus		rebuildActiveSelectionList(bool undo);
};
class CBrushToolCommand_Scale : CBrushToolCommand
{
public:
	CBrushToolCommand_Scale() {}
	virtual ~CBrushToolCommand_Scale() {}
private:
	ToolCommandInside<Arg_Scale, Data_Scale>	__blobScale;
public:
	const Arg_Scale&		getArg() const			{return __blobScale.arg();}
	Arg_Scale&			getArg()			{return __blobScale.arg();}
	const InsideArray_Scale&	getInsideArray() const		{return __blobScale.insideArray();}
	InsideArray_Scale&		getInsideArray()		{return __blobScale.insideArray();}
private:
	virtual MStatus		action(int flag, bool byMaya);
public:
	virtual MStatus		rebuildActiveSelectionList(bool undo);
};
#endif
class CBrushToolCommand_Translate : CBrushToolCommand
{
public:
	CBrushToolCommand_Translate() {}
	virtual ~CBrushToolCommand_Translate() {}
private:
	ToolCommandInside<Arg_Translate, Data_Translate>	__blobTranslate;
public:
	const Arg_Translate&		getArg() const			{return __blobTranslate.arg();}
	Arg_Translate&			getArg()			{return __blobTranslate.arg();}
	const InsideArray_Translate&	getInsideArray() const		{return __blobTranslate.insideArray();}
	InsideArray_Translate&		getInsideArray()		{return __blobTranslate.insideArray();}
private:
	virtual MStatus		action(int flag, bool byMaya);
public:
	virtual MStatus		rebuildActiveSelectionList(bool undo);
public:
	const static double	TRANSLATE_SCALE_FACTOR;
};
class CBrushToolCommand_Rotate : CBrushToolCommand
{
public:
	CBrushToolCommand_Rotate() {}
	virtual ~CBrushToolCommand_Rotate() {}
private:
	ToolCommandInside<Arg_Rotate, Data_Rotate>	__blobRotate;
public:
	const Arg_Rotate&		getArg() const			{return __blobRotate.arg();}
	Arg_Rotate&			getArg()			{return __blobRotate.arg();}
	const InsideArray_Rotate&	getInsideArray() const		{return __blobRotate.insideArray();}
	InsideArray_Rotate&		getInsideArray()		{return __blobRotate.insideArray();}
private:
	virtual MStatus		action(int flag, bool byMaya);
public:
	virtual MStatus		rebuildActiveSelectionList(bool undo);
public:
	const static double	ROTATE_NORMALIZATION;
};
#if 1
#if 0
class CBrushToolCommand_Tentacle_Twist : CBrushToolCommand
{
public:
	CBrushToolCommand_Tentacle_Twist() {}
	virtual ~CBrushToolCommand_Tentacle_Twist() {}
private:
	ToolCommandInside<Arg_Tentacle_Twist, Data_Tentacle_Twist>	__blobTentacleTwist;
public:
	const Arg_Tentacle_Twist&		getArg() const			{return __blobTentacleTwist.arg();}
	Arg_Tentacle_Twist&			getArg()			{return __blobTentacleTwist.arg();}
	const InsideArray_Tentacle_Twist&	getInsideArray() const		{return __blobTentacleTwist.insideArray();}
	InsideArray_Tentacle_Twist&		getInsideArray()		{return __blobTentacleTwist.insideArray();}
private:
	virtual MStatus		action(int flag, bool byMaya);
public:
	virtual MStatus		rebuildActiveSelectionList(bool undo);
public:
	const static double	ROTATE_NORMALIZATION;
};
class CBrushToolCommand_Tentacle_Shape : CBrushToolCommand
{
public:
	CBrushToolCommand_Tentacle_Shape() {}
	virtual ~CBrushToolCommand_Tentacle_Shape() {}
private:
	ToolCommandInside<Arg_Tentacle_Shape, Data_Tentacle_Shape>	__blobTentacleShape;
public:
	const Arg_Tentacle_Shape&		getArg() const			{return __blobTentacleShape.arg();}
	Arg_Tentacle_Shape&			getArg()			{return __blobTentacleShape.arg();}
	const InsideArray_Tentacle_Shape&	getInsideArray() const		{return __blobTentacleShape.insideArray();}
	InsideArray_Tentacle_Shape&		getInsideArray()		{return __blobTentacleShape.insideArray();}
private:
	virtual MStatus		action(int flag, bool byMaya);
public:
	virtual MStatus		rebuildActiveSelectionList(bool undo);
};
#endif
#if 1
class CBrushToolCommand_Tentacle_Create : CBrushToolCommand
{
public:
	CBrushToolCommand_Tentacle_Create() {}
	virtual ~CBrushToolCommand_Tentacle_Create() {}
#if 0
private:
	ToolCommandInside<Arg_Tentacle_Create, Data_Tentacle_Create>	__blobTentacleCreate;
public:
	const Arg_Tentacle_Create&		getArg() const			{return __blobTentacleCreate.arg();}
	Arg_Tentacle_Create&			getArg()			{return __blobTentacleCreate.arg();}
	const InsideArray_Tentacle_Create&	getInsideArray() const		{return __blobTentacleCreate.insideArray();}
	InsideArray_Tentacle_Create&		getInsideArray()		{return __blobTentacleCreate.insideArray();}
#endif
private:
	virtual MStatus		action(int flag, bool byMaya);
public:
	virtual MStatus		rebuildActiveSelectionList(bool undo);
#if 1
private:
	Arg_Tentacle_Create	__arg_Tentacle_Create;
public:
	const Arg_Tentacle_Create&	arg_Tentacle_Create() const	{return __arg_Tentacle_Create;}
	Arg_Tentacle_Create&		arg_Tentacle_Create()		{return __arg_Tentacle_Create;}
public:
	void	doPress(const MDagPath& nodeDagPath) const;
	void	doDrag(const MDagPath& nodeDagPath) const;
	void	doRelease(const MDagPath& nodeDagPath) const;
#endif
};
#endif
#endif
