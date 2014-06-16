#include <maya/MPointArray.h>
#include <maya/MMatrix.h>
#include <maya/MQuaternion.h>
#include <math.h>
#include <tentacleShape/kTentacle.h>
kTentacle::kTentacle(int numPoints) :
#if 1
	cut_frame(0)
#else
	have_cut(false)
#endif
{
	longitudinal.setLength(numPoints - 1);
	transversal.setLength(numPoints);
	majorAxis.setLength(numPoints);
	eccentricity.setLength(numPoints);
	rotation.setLength(numPoints);
}
MStatus	kTentacle::getSamplePoints(
	const MMatrix&	worldMatrix,
	MPointArray&	strandSamplePoints,
	unsigned	numEllipseSamplePoints,
	MPointArray&	tentacleSamplePoints
#if 3
		,
		const MPointArray*	givenStrandSamplePointsPtr,
		MSpace::Space		space
#endif
) const
{
#if 3
	if (!(space == MSpace::kWorld || space == MSpace::kObject))
		return MStatus::kInvalidParameter;
	if (givenStrandSamplePointsPtr) {
		if (!(givenStrandSamplePointsPtr->length() == transversal.length()))
			return MStatus::kInvalidParameter;
	}
#endif
	MStatus	ms;
#if 1
	MMatrix	mInv = worldMatrix.inverse();
#endif
	MMatrix	mInvT = worldMatrix.inverse().transpose();
	unsigned	numStrandSamplePoints = transversal.length();
#if 1
	ms = strandSamplePoints.setLength(numStrandSamplePoints);
	if (!ms) {
		cerr << "kTentacle::setSamplePoints: MPointArray::setLength(" << numStrandSamplePoints << "): Failure: " << ms << endl;
		return ms;
	}
#if 1
#if 3
	MVectorArray	transversal_array_in_kObject(numStrandSamplePoints);
	MVectorArray	longitudinal_array_in_kObject(numStrandSamplePoints - 1);
if (givenStrandSamplePointsPtr) {
#if 1
	if (space == MSpace::kWorld)
		ms = strandSamplePoints.copy(*givenStrandSamplePointsPtr);
	else {
		const MPointArray&	givenStrandSamplePoints = *givenStrandSamplePointsPtr;
		for (int k = 0; k < (int) numStrandSamplePoints; ++k)
			strandSamplePoints[k] = givenStrandSamplePoints[k] * worldMatrix;
	}
#endif
#if 1
{
	double	len0 = MVector(strandSamplePoints[1] - strandSamplePoints[0]).length();
	for (int k = 1; k < (int) numStrandSamplePoints; ++k) {
		MVector	longiVector = strandSamplePoints[k] - strandSamplePoints[k - 1];
		longitudinal_array_in_kObject[k - 1] = len0 * longiVector.transformAsNormal(mInv);
	}
	int	k = 0;
	for (; k < (int) numStrandSamplePoints - 1; ++k) {
		const MVector&	longiVector0 = longitudinal[k];
		const MVector&	longiVector = longitudinal_array_in_kObject[k];
		const MVector&	transVector0 = transversal[k];
		MVector&	transVector = transversal_array_in_kObject[k];
		transVector = transVector0.rotateBy(MQuaternion(longiVector0, longiVector));
	}
	transversal_array_in_kObject[k] = transversal_array_in_kObject[k - 1];
}
#endif
} else {
	ms = transversal_array_in_kObject.copy(transversal);
	ms = longitudinal_array_in_kObject.copy(longitudinal);
#if 1
	MPoint	basePoint0(kRoot::position);
#else
	MPoint	basePoint0(root);
#endif
	MPoint	point = basePoint0 * worldMatrix;
	double	len0 = longitudinal[0].length();
	for (int k = 0; k < (int) numStrandSamplePoints; ++k) {
		if (k) {
			MVector	offset = longitudinal[k - 1];
			offset *= mInvT;
			offset.normalize();
			point += len0 * offset;
		}
		strandSamplePoints[k] = point;
	}
}
#else
{
#if 1
	MPoint	basePoint0(kRoot::position);
#else
	MPoint	basePoint0(root);
#endif
	MPoint	point = basePoint0 * worldMatrix;
	double	len0 = longitudinal[0].length();
	for (int k = 0; k < (int) numStrandSamplePoints; ++k) {
		if (k) {
			MVector	offset = longitudinal[k - 1];
			offset *= mInvT;
			offset.normalize();
			point += len0 * offset;
		}
		strandSamplePoints[k] = point;
	}
}
#endif
#endif
#endif
#if 1
#if 1
#if 3
	MVectorArray	transversal_array_in_kWorld(numStrandSamplePoints);
	MVectorArray	longitudinal_array_in_kWorld(numStrandSamplePoints);
	for (int vIndex = 0; vIndex < (int) numStrandSamplePoints; ++vIndex) {
		const MPoint&	point = strandSamplePoints[vIndex];
		MVector&	transVector = transversal_array_in_kWorld[vIndex];
		MVector&	longiVector = longitudinal_array_in_kWorld[vIndex];
#if 3
		const MVector&	transVector0 = transversal_array_in_kObject[vIndex];
#else
		MVector	transVector;
		MVector	transVector0 = transversal[vIndex];
#endif
#if 1
		MPoint	point0 = point * mInv;
		MPoint	p = point0 + transVector0;
		p *= worldMatrix;
		transVector = p - point;
#else
		transVector = transVector0 * mInvT;
#endif
		transVector.normalize();
		int	index = vIndex + 1 == (int) numStrandSamplePoints ? vIndex : vIndex + 1;
#if 3
		const MVector&	longiVector0 = longitudinal_array_in_kObject[index - 1];
		longiVector = longiVector0 * mInvT;
#else
		MVector	longiVector0 = longitudinal[index - 1];
		MVector	longiVector = longiVector0 * mInvT;
#endif
		longiVector.normalize();
	}
#endif
#endif
	if (numStrandSamplePoints == 0) {
		cerr << "kTentacle::setSamplePoints: numStrandSamplePoints = 0!" << endl;
		return MStatus::kInvalidParameter;
	}
	ms = tentacleSamplePoints.setLength(numStrandSamplePoints * numEllipseSamplePoints);
	if (!ms) {
		cerr << "kTentacle::setSamplePoints: MPointArray::setLength(" << numStrandSamplePoints << " * " << numEllipseSamplePoints << "): Failure: " << ms << endl;
		return ms;
	}
	for (int vIndex = 0; vIndex < (int) numStrandSamplePoints; ++vIndex) {
		const MPoint&	point = strandSamplePoints[vIndex];
#if 3
		const MVector&	transVector = transversal_array_in_kWorld[vIndex];
		const MVector&	longiVector = longitudinal_array_in_kWorld[vIndex];
#else
		MVector	transVector;
		MVector	transVector0 = transversal[vIndex];
#if 1
		MPoint	point0 = point * mInv;
		MPoint	p = point0 + transVector0;
		p *= worldMatrix;
		transVector = p - point;
#else
		transVector = transVector0 * mInvT;
#endif
		transVector.normalize();
		int	index = vIndex + 1 == (int) numStrandSamplePoints ? vIndex : vIndex + 1;
		MVector	longiVector0 = longitudinal[index - 1];
		MVector	longiVector = longiVector0 * mInvT;
		longiVector.normalize();
#endif
		double	a = majorAxis[vIndex];
		double	e = eccentricity[vIndex];
		if (e < 0)
			e = 0;
		if (e >= 1)
			e = 1 - 1e-7;
		double	e2 = e * e;
		double	f = a * sqrt(1 - e2);
		double	angle0 = rotation[vIndex];
		for (int n = 0; n < (int) numEllipseSamplePoints; ++n) {
			double	angle = 2 * M_PI * n / numEllipseSamplePoints;
			MQuaternion	q(angle0 + angle, longiVector);
			double	c = cos(angle);
			double	r = f / sqrt(1 - e2 * c * c);
			MVector	v = transVector.rotateBy(q);
			v *= r;
			tentacleSamplePoints[vIndex * numEllipseSamplePoints + n] = point + v;
		}
	}
#endif
	return MStatus::kSuccess;
}
#if 1
MStatus	kTentacle::getSamplePoints(
	MPointArray&	strandSamplePoints,
	unsigned	numEllipseSamplePoints,
	MPointArray&	tentacleSamplePoints,
	const MPointArray*	givenStrandSamplePointsPtr
) const
{
	MStatus	ms;
	MPointArray	wPoints_in_kWorld;
	if (!givenStrandSamplePointsPtr) {
		for (unsigned n = 0; n < wPoints.length(); ++n)
			ms = wPoints_in_kWorld.append(wPoints[n]);
	}
	const MPointArray&	worldPts = givenStrandSamplePointsPtr ? *givenStrandSamplePointsPtr : wPoints_in_kWorld;
	if (!worldPts.length()) {
		cerr << "FATAL: kTentacle::getSamplePoints: NO worldPts" << endl;
		return MStatus::kFailure;
	}
	if (!(
		worldPts.length() == majorAxis.length() &&
		worldPts.length() == eccentricity.length() &&
		worldPts.length() == rotation.length()
	)) {
		cerr << "FATAL: kTentacle::getSamplePoints: # of points: MISMATCH!" << endl;
		return MStatus::kFailure;
	}
#if 0
	MVectorArray	longitudinal_array;
	MVectorArray	transversal_array;
	for (unsigned i = 1; i < worldPts.length(); ++i) {
		MVector	longitudinal = worldPts[i] - worldPts[i - 1];
		ms = longitudinal.normalize();
		ms = longitudinal_array.append(longitudinal);
		MVector	transversal0 = MVector(1, 0, 0).rotateBy(MQuaternion(MVector(0, 0, 1), longitudinal));
		ms = transversal_array.append(transversal0.rotateBy(longitudinal, rotation[i - 1]));
		if (i + 1 == worldPts.length()) {
			ms = transversal_array.append(transversal0.rotateBy(longitudinal, rotation[i]));
		}
	}
#endif
	strandSamplePoints.clear();
	ms = tentacleSamplePoints.clear();
	int	numStrandSamplePoints = worldPts.length();
	ms = strandSamplePoints.setLength(numStrandSamplePoints);
	ms = tentacleSamplePoints.setLength(numStrandSamplePoints * numEllipseSamplePoints);
	for (int vIndex = 0; vIndex < numStrandSamplePoints; ++vIndex) {
		MPoint&	point = strandSamplePoints[vIndex];
		point = worldPts[vIndex];
		int	index = vIndex + 1 == numStrandSamplePoints ? vIndex : vIndex + 1;
		MVector	longiVector = worldPts[index] - worldPts[index - 1];
		ms = longiVector.normalize();
		MVector	transVector = MVector(1, 0, 0).rotateBy(MQuaternion(MVector(0, 0, 1), longiVector));
		double	a = majorAxis[vIndex];
		double	e = eccentricity[vIndex];
		if (e < 0)
			e = 0;
		if (e >= 1)
			e = 1 - 1e-7;
		double	e2 = e * e;
		double	f = a * sqrt(1 - e2);
		double	angle0 = rotation[vIndex];
		for (int n = 0; n < (int) numEllipseSamplePoints; ++n) {
			double	angle = 2 * M_PI * n / numEllipseSamplePoints;
			MQuaternion	q(angle0 + angle, longiVector);
			double	c = cos(angle);
			double	r = f / sqrt(1 - e2 * c * c);
			MVector	v = transVector.rotateBy(q);
			v *= r;
			tentacleSamplePoints[vIndex * numEllipseSamplePoints + n] = point + v;
		}
	}
	return MStatus::kSuccess;
}
#endif
int	kTentacle::getT0_in_kObject(ShVector9& T0_in_kObject) const
{
	MPoint	position0;
	MVector	longiVector0;
	MVector	transVector0;
	MStatus	ms = getT0_in_kObject(position0, longiVector0, transVector0);
	if (!ms) {
		cerr << "kTentacle::getT0_in_kObject(): Failure" << endl;
		return -1;
	}
	T0_in_kObject.position[0] = position0.x;
	T0_in_kObject.position[1] = position0.y;
	T0_in_kObject.position[2] = position0.z;
	T0_in_kObject.tangent[0] = longiVector0.x;
	T0_in_kObject.tangent[1] = longiVector0.y;
	T0_in_kObject.tangent[2] = longiVector0.z;
	T0_in_kObject.normal[0] = transVector0.x;
	T0_in_kObject.normal[1] = transVector0.y;
	T0_in_kObject.normal[2] = transVector0.z;
	return 0;
}
MStatus	kTentacle::getT0_in_kObject(MPoint& position0, MVector& longiVector0, MVector& transVector0) const
{
	if (longitudinal.length() == 0) {
		cerr << "Fatal: kTentacle::setT0_in_kObject: longitudinal.length() = 0!" << endl;
		return MStatus::kInvalidParameter;
	}
#if 1
	position0 = kRoot::position;
	longiVector0 = kRoot::normal;
	longiVector0.normalize();
#else
	position0 = root;
	longiVector0 = longitudinal[0];
	longiVector0.normalize();
#endif
	transVector0 = MVector(1, 0, 0).rotateBy(MQuaternion(MVector(0, 0, 1), longiVector0)).rotateBy(MQuaternion(rotation[0], longiVector0));
	return MStatus::kSuccess;
}
ostream&	operator<<(ostream& os, const kCut& cut)
{
	os << "\t" << "cut: {" << endl;
	os << "\t" << "\t" << "t: " << cut.t << endl;
	os << "\t" << "\t" << "polar: " << cut.polar << endl;
	os << "\t" << "\t" << "azimuthal: " << cut.azimuthal << endl;
	os << "\t" << "}";
	return os;
}
#if 1
ostream&	operator<<(ostream& os, const kRoot& root)
{
	os << "\t" << "root: {" << endl;
	os << "\t" << "\t" << "polyId: " << root.polyId << endl;
	os << "\t" << "\t" << "position: " << root.position << endl;
	os << "\t" << "\t" << "normal: " << root.normal << endl;
	os << "\t" << "}";
	return os;
}
#endif
ostream&	operator<<(ostream& os, const kTentacle& tentacle)
{
	os << "tentacle: {" << endl;
#if 1
	os << (kRoot&) tentacle << endl;
#else
	os << "\t" << "root: " << tentacle.root << endl;
#endif
	os << "\t" << "longitudinal: " << tentacle.longitudinal << endl;
	os << "\t" << "transversal: " << tentacle.transversal << endl;
	os << "\t" << "majorAxis: " << tentacle.majorAxis << endl;
	os << "\t" << "eccentricity: " << tentacle.eccentricity << endl;
	os << "\t" << "rotation: " << tentacle.rotation << endl;
#if 1
	os << "\t" << "cut frame: " << (tentacle.cut_frame > 0 ? tentacle.cut_frame : -1) << endl;
	if (tentacle.cut_frame > 0)
		os << *((kCut*) &tentacle) << endl;
#else
	os << "\t" << "have cut? " << (tentacle.have_cut ? "True" : "False") << endl;
	if (tentacle.have_cut)
		os << *((kCut*) &tentacle) << endl;
#endif
	os << "}";
	return os;
}
