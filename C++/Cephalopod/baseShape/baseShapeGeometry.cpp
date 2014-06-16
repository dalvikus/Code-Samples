#include <baseShape/baseShapeGeometry.h>
baseShapeGeometry::baseShapeGeometry() : faceCount(0) {}
baseShapeGeometry::~baseShapeGeometry() {}
baseShapeGeometry&	baseShapeGeometry::operator=(const baseShapeGeometry& other)
{
	if (&other != this) {
		vertices	= other.vertices;
#if 2
		wakes		= other.wakes;
#endif
		face_counts	= other.face_counts;
		face_connects	= other.face_connects;
		normals		= other.normals;
		ucoords		= other.ucoords;
		vcoords		= other.vcoords;
		faceCount	= other.faceCount;
	}
	return *this;
}
