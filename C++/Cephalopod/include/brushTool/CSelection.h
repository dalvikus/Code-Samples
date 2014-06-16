#pragma once
#include <maya/MVectorArray.h>
#include <maya/MPointArray.h>
#include <maya/MString.h>
#include <maya/MMatrix.h>
#include <baseShape/kPtrArray.h>
#include <maya/MDagPath.h>
#ifdef _DEBUG
#include <assert.h>
#endif
#include <maya/MDoubleArray.h>
#if 1
#include <maya/MFloatPoint.h>
#include <maya/MFnMesh.h>
#endif
struct SelectedGuide {
	int		uIndex;
	MVectorArray	offsets;
#if 1
#endif
#if 1
	MVector		root_wNormal;
#endif
#if 1
	MVectorArray	transversals;
#endif
	unsigned	vertexBits;
};
typedef SelectedGuide*	SelectedGuidePtr;
class CSelection {
public:
	MDagPath	dagPath;
#if 1
	bool		hasSkinCluster;
#endif
	int		selectBy;
	int		numPoints;
	MMatrix		worldMatrix;
	kPtrArray<SelectedGuide>	guideArray;
public:
#if 1
	CSelection() : hasSkinCluster(false) {}
#else
	CSelection() {}
#endif
	CSelection(const CSelection& other);
	CSelection&	operator=(const CSelection& other);
	virtual ~CSelection() {}
};
typedef CSelection*		SelectionPtr;
typedef kPtrArray<CSelection>	SelectionArray;
typedef SelectionArray*		SelectionArrayPtr;
void	listActiveSelectionArray(
	const char*	typeName,
	SelectionArray&	selectionArray,
	const char*	callerName
);
#if 1
struct Arg_Translate
{
	MVector		xDir;
	MVector		yDir;
	MVector		deltaX;
};
struct Data_Translate
{
	int	uIndex;
	unsigned	vertexBits;
	MDoubleArray	factorArray;
	MVectorArray	offsets__;
	MVectorArray	offsets;
#if 1
#endif
#if 1
#if 1
	MVector		root_wNormal;
#endif
	MVectorArray	transversals__;
	MVectorArray	transversals;
#endif
};
typedef Data_Translate*	Data_TranslatePtr;
struct Arg_Rotate
{
	MVector		axis;
	double		byAngleAmount;
};
struct Data_Rotate
{
	int	uIndex;
	unsigned	vertexBits;
	MDoubleArray	factorArray;
	MVectorArray	offsets__;
	MVectorArray	offsets;
#if 1
#endif
#if 1
#if 1
	MVector		root_wNormal;
#endif
	MVectorArray	transversals__;
	MVectorArray	transversals;
#endif
};
typedef Data_Rotate*	Data_RotatePtr;
#if 1
#if 0
struct Arg_Tentacle_Twist
{
	double	byAngleAmount;
};
struct Data_Tentacle_Twist
{
	int	uIndex;
	unsigned	vertexBits;
	MDoubleArray	factorArray;
	MVectorArray	longitudinals;
	MVectorArray	transversals__;
	MVectorArray	transversals;
};
typedef Data_Tentacle_Twist*	Data_Tentacle_TwistPtr;
struct Arg_Tentacle_Shape
{
	double	factor;
};
struct Data_Tentacle_Shape
{
	int	uIndex;
	unsigned	vertexBits;
	MDoubleArray	factorArray;
	MVectorArray	transversals__;
	MVectorArray	transversals;
};
typedef Data_Tentacle_Shape*	Data_Tentacle_ShapePtr;
#endif
#if 1
struct Arg_Tentacle_Create
{
	bool	hitted;
#if 1
#if 1
#if 1
	int	hitVertexList[3];
#else
	int	hitTriangleId;
#endif
	float	hitBary1;
	float	hitBary2;
#endif
	int	polyId;
	MPoint	position;
	MVector	normal;
#else
	MFloatPoint	hitPoint;
#endif
	Arg_Tentacle_Create() : hitted(false) {}
};
struct Data_Tentacle_Create
{
#if 0
	int	uIndex;
	unsigned	vertexBits;
	MDoubleArray	factorArray;
	MVectorArray	transversals__;
	MVectorArray	transversals;
#endif
};
typedef Data_Tentacle_Create*	Data_Tentacle_CreatePtr;
#endif
#endif
#if 0
struct Arg_Cut
{
	double	factor;
};
typedef Arg_Cut	Arg_Scale;
struct Data_Cut
{
	int	uIndex;
	double	segmentLength__;
	double	segmentLength;
};
typedef Data_Cut*	Data_CutPtr;
typedef Data_Cut	Data_Scale;
typedef Data_Scale*	Data_ScalePtr;
#endif
template <class TData>
class ToolCommandData {
public:
	MDagPath	dagPath;
#if 1
	bool		hasSkinCluster;
#endif
	int		selectBy;
	int		numPoints;
	MMatrix		worldMatrix;
	kPtrArray<TData>	dataArray;
public:
#if 1
	ToolCommandData() : hasSkinCluster(false) {};
#else
	ToolCommandData() {};
#endif
	ToolCommandData(const ToolCommandData& other)	{*this = other;}
	ToolCommandData&	operator=(const ToolCommandData& other);
	virtual ~ToolCommandData() {}
};
template <class TData>
ToolCommandData<TData>&	ToolCommandData<TData>::operator=(const ToolCommandData<TData>& other)
{
	typedef TData*	TPtr;
	if (this != &other) {
		this->dagPath			= other.dagPath;
#if 1
		this->hasSkinCluster		= other.hasSkinCluster;
#endif
		this->selectBy			= other.selectBy;
		this->numPoints			= other.numPoints;
		this->worldMatrix		= other.worldMatrix;
		for (unsigned n = 0; n < other.dataArray.length(); ++n) {
			TPtr	p = new TData(other.dataArray(n));
			this->dataArray.append(p);
		}
	}
	return *this;
}
template <class TArg, class TData>
class ToolCommandInside {
private:
	TArg	__arg;
public:
	const TArg&	arg() const	{return __arg;}
	TArg&		arg()		{return __arg;}
private:
	typedef ToolCommandData<TData>	TInside;
	typedef kPtrArray<TInside>	TInsideArray;
	TInsideArray	__insideArray;
public:
	const TInsideArray&	insideArray() const	{return __insideArray;}
	TInsideArray&		insideArray()		{return __insideArray;}
};
#if 0
typedef ToolCommandData<Data_Cut>		DataArray_Cut;
typedef DataArray_Cut*				DataArrayPtr_Cut;
typedef kPtrArray<DataArray_Cut>		InsideArray_Cut;
typedef ToolCommandData<Data_Scale>		DataArray_Scale;
typedef DataArray_Scale*			DataArrayPtr_Scale;
typedef kPtrArray<DataArray_Scale>		InsideArray_Scale;
#endif
typedef ToolCommandData<Data_Translate>		DataArray_Translate;
typedef DataArray_Translate*			DataArrayPtr_Translate;
typedef kPtrArray<DataArray_Translate>		InsideArray_Translate;
typedef ToolCommandData<Data_Rotate>		DataArray_Rotate;
typedef DataArray_Rotate*			DataArrayPtr_Rotate;
typedef kPtrArray<DataArray_Rotate>		InsideArray_Rotate;
#if 1
#if 0
typedef ToolCommandData<Data_Tentacle_Twist>	DataArray_Tentacle_Twist;
typedef DataArray_Tentacle_Twist*		DataArrayPtr_Tentacle_Twist;
typedef kPtrArray<DataArray_Tentacle_Twist>	InsideArray_Tentacle_Twist;
typedef ToolCommandData<Data_Tentacle_Shape>	DataArray_Tentacle_Shape;
typedef DataArray_Tentacle_Shape*		DataArrayPtr_Tentacle_Shape;
typedef kPtrArray<DataArray_Tentacle_Shape>	InsideArray_Tentacle_Shape;
#endif
#if 1
#if 1
typedef ToolCommandData<Data_Tentacle_Create>	DataArray_Tentacle_Create;
typedef DataArray_Tentacle_Create*		DataArrayPtr_Tentacle_Create;
typedef kPtrArray<DataArray_Tentacle_Create>	InsideArray_Tentacle_Create;
#endif
#endif
#endif
#endif
