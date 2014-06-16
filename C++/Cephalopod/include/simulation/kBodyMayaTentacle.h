#pragma once
#ifndef __kBODY_MAYA_TENTACLE_H__
#define __kBODY_MAYA_TENTACLE_H__
#include <simulation/kBodyMaya.h>
#include <baseShape/kPtrArray.h>
#include <tentacleShape/kTentacle.h>
class MObject;
class MPlug;
class kBodyMayaTentacle : public kBodyMaya
{
public:
	kBodyMayaTentacle() : kBodyMaya() {}
	virtual ~kBodyMayaTentacle() {}
#if 1
private:
	kPtrArray<kTentacle>	__tentacleArray0;
public:
	const kPtrArray<kTentacle>&	tentacleArray0() const	{return __tentacleArray0;}
public:
	MStatus	set0(MObject& node);
#else
public:
	void	set__T0_in_kSpace_array(const kPtrArray<ShVector9>& T0_in_kObject_array);
#endif
#if 1
public:
	static MStatus	set0(
		MObject&	node,
		kPtrArray<kTentacle>&	tentacleArray0,
		kPtrArray<ShVector9>&	T0_in_kSpace_array,
		int	verbose_level
	);
	static MStatus	set0(
		MPlug&	plug_tentacleArray,
		kPtrArray<kTentacle>&	tentacleArray0,
		kPtrArray<ShVector9>&	T0_in_kSpace_array,
		int	verbose_level
	);
#endif
};
#endif
