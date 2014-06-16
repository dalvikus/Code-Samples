#pragma once
#define USE_SOFTWARE_OVERLAYS
#include <maya/MPxContext.h>
#include <maya/MMessage.h>
#include <brushTool/CBrushCircle.h>
#include <brushTool/CBrushToolCommand.h>
#if 1
#include <baseShape/kSkinSmoothBind.h>
#endif
#define BRUSH_TOOL__CLASS_NAME	"BrushTool"
class CBrushContext : public MPxContext
{
public:
	CBrushContext();
	virtual ~CBrushContext();
	virtual void		toolOnSetup(MEvent& event);
	virtual void		toolOffCleanup();
	virtual MStatus		doPress(MEvent& event);
	virtual MStatus		doDrag(MEvent& event);
	virtual MStatus		doRelease(MEvent& event);
	virtual MStatus		doEnterRegion(MEvent& event);
public:
	virtual void		getClassName(MString& name) const	{name.set(BRUSH_TOOL__CLASS_NAME);}
private:
	short		start_x, start_y;
	short		last_x, last_y;
private:
	CBrushCircle	__brushCircle;
public:
	const CBrushCircle&	getBrushCircle() const	{return __brushCircle;}
	CBrushCircle&		getBrushCircle()	{return __brushCircle;}
private:
	static float	PERIOD;
	MCallbackId	__timerCallbackId;
	static void	timerCallback(float elapsedTime, float lastTime, void* contextPtr);
private:
#if 0
	MStatus	setInsideArray_Cut(InsideArray_Cut& insideArray_Cut);
	MStatus	setInsideArray_Scale(InsideArray_Scale& insideArray_Scale);
#endif
	MStatus	setInsideArray_Translate(InsideArray_Translate& insideArray_Translate);
	MStatus	setInsideArray_Rotate(InsideArray_Rotate& insideArray_Rotate);
#if 1
#if 0
	MStatus	setInsideArray_Tentacle_Twist(InsideArray_Tentacle_Twist& insideArray_Tentacle_Twist);
	MStatus	setInsideArray_Tentacle_Shape(InsideArray_Tentacle_Shape& insideArray_Tentacle_Shape);
#endif
#if 1
	MStatus	setInsideArray_Tentacle_Create(InsideArray_Tentacle_Create& insideArray_Tentacle_Create);
#endif
#endif
private:
	CBrushToolCommand::BRUSH_TYPE	__brushType;
	CBrushToolCommand*		__toolCommandPtr;
public:
	CBrushToolCommand::BRUSH_TYPE	getBrushType() const;
	MStatus				setBrushType(CBrushToolCommand::BRUSH_TYPE brushType);
private:
	MCallbackId	__activeListModifiedCallbackId;
	static void	__activeListModifiedCallback(void* contextPtr);
#if 1
private:
#if 1
	MCallbackId	__userEventTentacleArrayUpdatedCallbackId;
	static void	__userEventTentacleArrayUpdatedCallback(void* contextPtr);
#endif
#endif
	SelectionArray	__activeSelectionArray;
public:
	MStatus		buildActiveSelectionList(void);
public:
	static MStatus	attenuateLength(bool bDir);
#if 1
#if 1
private:
	MFnMesh		__mesh;
	MDagPath	__nodeDagPath;
#if MAYA_API_VERSION >= 200800
	MMeshIsectAccelParams	__accelParams;
#else
#if MAYA_API_VERSION == 700
#endif
#endif
#if 1
private:
	kSkinSmoothBind	__skinSmoothBind;
#if 0
	unsigned		__numJoints;
	MFnSkinCluster		__skinCluster;
	MMatrix			__geometry_matrix;
	kPtrArray<MMatrix>	__inverse_binding_matrix_array;
	kPtrArray<MDoubleArray>	__weights_array;
#endif
#endif
#endif
#endif
};
