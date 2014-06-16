#pragma once
#include <maya/MPointArray.h>
#include <maya/MIntArray.h>
#include <maya/MDoubleArray.h>
#include <maya/MVectorArray.h>
#if 2
#define WAKE_POINTS	(4 * 4)
#endif
class baseShapeGeometry
{
public:
	baseShapeGeometry();
	~baseShapeGeometry();
	baseShapeGeometry&	operator=(const baseShapeGeometry&);
public:
	MPointArray	vertices;
#if 2
	MPointArray	wakes;
#endif
#if 5
#if 0
	stat = surfFn.getPoints(vertices, MSpace::kObject);
	for (int i = 0; i < surfFn.numPolygons(); ++i) {
		MIntArray	polyVerts;
		surfFn.getPolygonVertices(i, polyVerts);
		int	pvc = polyVerts.length();
		counts.append(pvc);
		for (int v = 0; v < pvc; ++v) {
			connects.append(polyVerts[v]);
		}
	}
#endif
	int	faceCount;
	MIntArray	face_counts;
	MIntArray	face_connects;
#endif
	MVectorArray	normals;
	MDoubleArray	ucoords;
	MDoubleArray	vcoords;
};
