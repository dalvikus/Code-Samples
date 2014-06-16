#pragma once
#ifndef __kBODY_MAYA_H__
#define __kBODY_MAYA_H__
#include <simulation/kBody.h>
#include <maya/MStatus.h>
#include <maya/MTime.h>
#include <maya/MMatrix.h>
#include <maya/MDagPath.h>
#include <maya/MVectorArray.h>
#if 1
#include <baseShape/kPtrArray.h>
#endif
#if 1
#include "../../../SBD/SBDlib/src_allheader/ShSimBody.h"
#else
#include <SHD/ShSimBody.h>
#endif
#include <maya/MFnMesh.h>
#if 1
#include <baseShape/kSkinSmoothBind.h>
#endif
class kBodyMaya : public MDagPath, public virtual kBody
{
protected:
	MMatrix		__inclusiveMatrix;
	MMatrix		__exclusiveMatrix;
	MObject		__objectTransform;
	MTransformationMatrix::RotationOrder	__rotationOrder;
	MFnMesh		__mesh;
public:
	const MMatrix&	inclusiveMatrix() const	{return __inclusiveMatrix;}
private:
	MStatus	__isReady(bool& isAnimated) const;
public:
	virtual MStatus	setBody(const MDagPath& bodyShape);
public:
	kBodyMaya() : __rotationOrder(MTransformationMatrix::kInvalid) {}
	virtual ~kBodyMaya() {}
public:
	virtual int	getTVector(
#if 1
		kTime			kT,
#else
		double			t,
#endif
		int			strandId,
		TVector&		inStart
	) const;
#if 0
private:
	struct WorldMatrixCache {
		MMatrix	worldMatrix;
		MMatrix	worldMatrix_1;
		MMatrix	worldMatrix_2;
	};
	typedef WorldMatrixCache*		WorldMatrixCachePtr;
	typedef kPtrArray<WorldMatrixCache>	WorldMatrixCacheArray;
	WorldMatrixCacheArray		__worldMatrixCacheArray;
public:
	void	setWorldMatrixCacheVoid() {__worldMatrixCacheArray.clear();}
#endif
#if 1
protected:
	kPtrArray<ShVector9>	__T0_in_kSpace_array;
#if 1
protected:
	kSkinSmoothBind		__skinSmoothBind;
public:
	const kSkinSmoothBind&	skinSmoothBind() const	{return __skinSmoothBind;}
#endif
#if 1
public:
#if 0
	int	getTVector(
		const kTime&	kT,
		const ShVector9&	T0_in_kObject,
		TVector&	inStart
	) const;
	virtual int	getTVector(
		double		t,
		const ShVector9&	T0_in_kObject,
		TVector&	inStart,
		double		deltaT
	) const;
#endif
public:
	bool	getNearestSurfacePoint(
		const kTime&		kT,
		const ShVector3&	in,
		ShVector3&		out
	) const;
	virtual bool	getNearestSurfacePoint(
		double			t,
		const ShVector3&	in,
		ShVector3&		out
	) const;
#endif
#if 1
public:
	virtual int	setCurrentTime(ShScalar nowTime)	{currentBodyTime = nowTime; return 0;}
#if 0
public:
	virtual void	getT0Vector9(ShVector9* outNow, void* pIndex = 0);
	virtual void	getT1Vector6(ShVector6* outNow, void* pIndex = 0);
	virtual void	getT2Vector6(ShVector6* outNow, void* pIndex = 0);
#endif
public:
	virtual bool	getNearestSurfacePoint(ShVector3 in, ShVector3 out);
#endif
#endif
};
#endif
