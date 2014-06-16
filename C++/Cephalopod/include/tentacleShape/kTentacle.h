#pragma once
#ifndef __TENTACLE_H__
#define	__TENTACLE_H__
#include <maya/MPoint.h>
#include <maya/MVectorArray.h>
#include <maya/MDoubleArray.h>
#if 1
#include "../../SBD/SBDlib/src_allheader/SuperHelixType.h"
#else
#include <SHD/SuperHelixType.h>
#endif
#if 1
#if 0
#include <maya/MMatrixArray.h>
#endif
#include <maya/MFloatVectorArray.h>
#endif
#define NUM_TENTACLE_SAMPLE_POINTS	31
struct kCut
{
	float	t;
	float	polar;
	float	azimuthal;
	kCut() : t(-1.f), polar(0.f), azimuthal(0.f) {}
	friend ostream&	operator<<(ostream& os, const kCut& cut);
};
#if 1
struct kRoot
{
	int	polyId;
	MPoint	position;
	MVector	normal;
#if 1
	MVector	wNormal;
#endif
#if 0
	int	hitTriangleId;
	float	hitBary1;
	float	hitBary2;
#endif
#if 0
	kRoot() : polyId(-1), hitTriangleId(-1) {}
#else
	kRoot() : polyId(-1) {}
#endif
};
#endif
class MMatrix;
class MPointArray;
struct kTentacle : kCut
#if 1
	, kRoot
#endif
{
#if 1
#else
	MPoint		root;
#endif
#if 1
#if 0
	float		unit_length;
	MMatrixArray	qm;
#endif
	MFloatVectorArray	wPoints;
#endif
	MVectorArray	longitudinal;
	MVectorArray	transversal;
	MDoubleArray	majorAxis;
	MDoubleArray	eccentricity;
	MDoubleArray	rotation;
#if 1
	int	cut_frame;
#else
	bool	have_cut;
#endif
	kTentacle(int numPoints = NUM_TENTACLE_SAMPLE_POINTS);
	friend ostream&	operator<<(ostream& os, const kTentacle& tentacle);
	MStatus	getSamplePoints(
		const MMatrix&	worldMatrix,
		MPointArray&	strandSamplePoints,
		unsigned	numEllipseSamplePoints,
		MPointArray&	tentacleSamplePoints
#if 3
		,
		const MPointArray*	givenStrandSamplePointsPtr = NULL,
		MSpace::Space		space = MSpace::kWorld
#endif
	) const;
	int	getT0_in_kObject(ShVector9& T0_in_kObject) const;
	MStatus	getT0_in_kObject(MPoint& position0, MVector& longiVector0, MVector& transVector0) const;
#if 1
	MStatus	getSamplePoints(
		MPointArray&	strandSamplePoints,
		unsigned	numEllipseSamplePoints,
		MPointArray&	tentacleSamplePoints,
		const MPointArray*	givenStrandSamplePointsPtr = NULL
	) const;
#endif
};
#endif
