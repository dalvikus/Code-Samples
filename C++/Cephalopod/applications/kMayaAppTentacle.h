#pragma once
#ifndef __kMAYA_APP_TENTACLE_H__
#define __kMAYA_APP_TENTACLE_H__
#include "kMayaApp.h"
#include <baseShape/kPtrArray.h>
#include <tentacleShape/kTentacle.h>
class MPlug;
class kMayaAppTentacle : public kMayaApp
{
public:
	kMayaAppTentacle(
		const char*	argv0,
		const char*	plug_in_shape_nodename,
		const char*	plug_in_name,
		const char*	plug_in_pathname_or,
		const char*	scene_filename_or,
		int		verbose_level = 0,
		MStatus*	status_ptr = NULL
	);
	~kMayaAppTentacle();
private:
	kPtrArray<kTentacle>	__tentacleArray0;
	kPtrArray<ShVector9>	__T0_in_kSpace_array;
public:
	const kPtrArray<kTentacle>&	tentacleArray0() const		{return __tentacleArray0;}
	const kPtrArray<ShVector9>&	T0_in_kObject_array() const	{return __T0_in_kSpace_array;}
#if 1
public:
	MStatus	set0(int verbose_level = 0);
#else
private:
	MStatus	__set(MPlug& plug_tentacleArray, int verbose_level = 0);
public:
	MStatus	set(int verbose_level = 0);
#endif
public:
	MStatus	getSamplePoints(
		double&	t,
		int tentacle_index,
		MPointArray&	strandSamplePoints,
		unsigned	numEllipseSamplePoints,
		MPointArray&	tentacleSamplePoints
	) const;
};
#endif
