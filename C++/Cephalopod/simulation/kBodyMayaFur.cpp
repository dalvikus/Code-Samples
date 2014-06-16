#include <afterShave/kBodyMayaFur.h>
#include <afterShave/afterShaveShape.h>
#if 0
#include <maya/MAnimUtil.h>
#include <maya/MAnimControl.h>
#include <maya/MPlugArray.h>
#include <maya/MObjectArray.h>
#include <maya/MFnAnimCurve.h>
#include <maya/MFnTransform.h>
#include <maya/MFnMesh.h>
#include <afterShave/kUtil.h>
#include <float.h>
#endif
void	kBodyMayaFur::set__T0_in_kSpace_array(const afterShaveGeometry& geometry)
{
#if 0
	if (!cpPtr) {
		cerr << "kBodyMaya::set__T0_in_kSpace_array: cpPtr is nil" << endl;
		return;
	}
	const MVectorArray&	cp = *cpPtr;
#endif
	__T0_in_kSpace_array.clear();
	for (unsigned n = 0; n < geometry.vertices.length(); ++n) {
		const MPoint&	position0 = geometry.vertices[n];
		const MPoint&	transVector0 = geometry.wakes[WAKE_POINTS * n];
#if 1
		const MVector&	longiVector0 = geometry.normals[n];
#else
		MVector	longiVector0 = cp[afterShaveShape::N_POINTS_IN_GUIDE * n + 1];
		longiVector0.normalize();
#endif
		ShVector9*	p = new ShVector9;
		__T0_in_kSpace_array.append(p);
		ShVector9&	T0_in_kObject = *p;
		T0_in_kObject.position[0] = position0.x;
		T0_in_kObject.position[1] = position0.y;
		T0_in_kObject.position[2] = position0.z;
		T0_in_kObject.tangent[0] = longiVector0.x;
		T0_in_kObject.tangent[1] = longiVector0.y;
		T0_in_kObject.tangent[2] = longiVector0.z;
		T0_in_kObject.normal[0] = transVector0.x;
		T0_in_kObject.normal[1] = transVector0.y;
		T0_in_kObject.normal[2] = transVector0.z;
	}
}
