#include <simulation/kBodyMaya.h>
#include <maya/MAnimUtil.h>
#include <maya/MAnimControl.h>
#include <maya/MPlugArray.h>
#include <maya/MObjectArray.h>
#include <maya/MFnAnimCurve.h>
#include <maya/MFnTransform.h>
#include <maya/MFnMesh.h>
#include <baseShape/kUtil.h>
#include <float.h>
#include <maya/MQuaternion.h>
MStatus	kBodyMaya::__isReady(bool& isAnimated) const
{
	MStatus	ms;
	bool	isValid = MDagPath::isValid(&ms);
	if (!ms || !isValid) {
		if (!ms) {
			cerr << "Error: kBodyMaya::__isReady: " << ms << endl;
			return ms;
		}
		cerr << "Error: kBodyMaya::__isReady: __externalDagPathShape NOT valid" << endl;
		return MStatus::kFailure;
	}
	if (__rotationOrder == MTransformationMatrix::kInvalid) {
		cerr << "Error: kBodyMaya::__isReady: __rotationOrder is kInvalid" << endl;
		return MStatus::kFailure;
	}
	isAnimated = MAnimUtil::isAnimated(__objectTransform, false, &ms);
	if (!ms) {
		cerr << "Error: kBodyMaya::__isReady: MAnimUtil::isAnimated(): Failure: " << ms << endl;
		return ms;
	}
	return MStatus::kSuccess;
}
int
kBodyMaya::getTVector(
	kTime			kT,
	int			strandId,
	TVector&		inStart
) const
{
	MStatus	ms;
	MTime	time;
	ms = time.setUnit(MTime::kSeconds);
	if (!ms) {
		ms.perror("MTime::setUnit(MTime::kSeconds)");
		return -1;
	}
#if 1
	const double	deltaTime = kT.deltaT();
#else
	const kTime::kTimeUnit& timeUnit = kT.timeUnit();
	const double	timeStep = 1. / timeUnit.__fps / timeUnit.__nSimulationPerFrame;
	const unsigned	N = 10;
	const double	deltaTime = timeStep / N;
#endif
	double	now = kT();
#if 1
	double	t0 = now, t_1 = now - deltaTime, t_2 = now - 2 * deltaTime;
#if 1
	if (__skinSmoothBind.hasSkinCluster()) {
		time.setValue(t0);
		t0 = time.value();
		MPoint	wPoint_0;
		MVector	wNormal_0;
		ms = __skinSmoothBind.getPointAndNormal(strandId, wPoint_0, wNormal_0, &time);
		time.setValue(t_1);
		t_1 = time.value();
		MPoint	wPoint_1;
		MVector	wNormal_1;
		ms = __skinSmoothBind.getPointAndNormal(strandId, wPoint_1, wNormal_1, &time);
		time.setValue(t_2);
		t_2 = time.value();
		MPoint	wPoint_2;
		MVector	wNormal_2;
		ms = __skinSmoothBind.getPointAndNormal(strandId, wPoint_2, wNormal_2, &time);
		inStart.T0.position[0] = wPoint_0.x;
		inStart.T0.position[1] = wPoint_0.y;
		inStart.T0.position[2] = wPoint_0.z;
		inStart.T0.tangent[0] = wNormal_0.x;
		inStart.T0.tangent[1] = wNormal_0.y;
		inStart.T0.tangent[2] = wNormal_0.z;
		MQuaternion	q(MVector(0, 0, 1), wNormal_0);
		MVector	binormal = MVector(1, 0, 0).rotateBy(q);
		inStart.T0.normal[0] = binormal.x;
		inStart.T0.normal[1] = binormal.y;
		inStart.T0.normal[2] = binormal.z;
		MVector	p10(wPoint_0 - wPoint_1);
		p10 /= t0 - t_1;
		inStart.T1.position[0] = p10.x;
		inStart.T1.position[1] = p10.y;
		inStart.T1.position[2] = p10.z;
		MVector	p21(wPoint_1 - wPoint_2);
		p21 /= t_1 - t_2;
		MVector	p012(p10 - p21);
		p012 /= (t0 - t_2) / 2;
		inStart.T2.position[0] = p012.x;
		inStart.T2.position[1] = p012.y;
		inStart.T2.position[2] = p012.z;
		MVector	v10(wNormal_0 - wNormal_1);
		v10 /= t0 - t_1;
		inStart.T1.tangent[0] = v10.x;
		inStart.T1.tangent[1] = v10.y;
		inStart.T1.tangent[2] = v10.z;
		MVector	v21(wNormal_1 - wNormal_2);
		v21 /= t_1 - t_2;
		MVector	v012(v10 - v21);
		v012 /= (t0 - t_2) / 2;
		inStart.T2.tangent[0] = v012.x;
		inStart.T2.tangent[1] = v012.y;
		inStart.T2.tangent[2] = v012.z;
		return 0;
	}
#endif
	if (strandId < 0 || strandId >= (int) __T0_in_kSpace_array.length()) {
		cerr << "kBodyMaya::getTVector: strandId = " << strandId << ": OUT OF RANGE (# of __T0_in_kSpace_array = " << __T0_in_kSpace_array.length() << ")" << endl;
		return -1;
	}
	const ShVector9&	T0_in_kObject = __T0_in_kSpace_array(strandId);
	bool	isAnimated = false;
	if (!__isReady(isAnimated)) {
		cerr << "Error: kBodyMaya::getTVector: kBodyMaya::__isReady(): Failure" << endl;
		return -1;
	}
	if (kT.nthFrame() == 1 && kT.nthStep() == 1)
		isAnimated = false;
	MMatrix	mNow0, mNow_1, mNow_2;
#if 0
	{
		double	t1 = now - deltaTime;
		ms = time.setValue(t1); if (!ms) ms.perror("MTime::setValue()");
		double	t2 = time.value();
		if (fabs(t1 - t2) > DBL_EPSILON)
			cerr << "hell, world(DBL_EPSILON): t1(before) = " << t1 << ", t2(after) = " << t2 << endl;
	}
#endif
	if (isAnimated) {
#if 0
		int	index = kT.timeUnit().__nSimulationPerFrame * (kT.nthFrame() - 1) + (kT.nthStep() - 1);
		bool	isCached = true;
		if (index >= (int) __worldMatrixCacheArray.length()) {
			isCached = false;
			WorldMatrixCachePtr	p = 0;
			bool	isOk = true;
#pragma warning(disable: 4127)
			do {
				p = new WorldMatrixCache;
				if (!p) {
					isOk = false;
					break;
				}
				MMatrix	m;
				if (!time.setValue(now)) {
					isOk = false;
					break;
				}
				t0 = time.value();
				if (!kUtil::getWorldMatrixAt(*((MDagPath*) this), time, m, &__rotationOrder)) {
					isOk = false;
					break;
				}
				mNow0 = m * __exclusiveMatrix;
				p->worldMatrix = mNow0;
				if (!time.setValue(now - deltaTime)) {
					isOk = false;
					break;
				}
				t_1 = time.value();
				if (!kUtil::getWorldMatrixAt(*((MDagPath*) this), time, m, &__rotationOrder)) {
					isOk = false;
					break;
				}
				mNow_1 = m * __exclusiveMatrix;
				p->worldMatrix_1 = mNow_1;
				if (!time.setValue(now - 2 * deltaTime)) {
					isOk = false;
					break;
				}
				t_2 = time.value();
				if (!kUtil::getWorldMatrixAt(*((MDagPath*) this), time, m, &__rotationOrder)) {
					isOk = false;
					break;
				}
				mNow_2 = m * __exclusiveMatrix;
				p->worldMatrix_2 = mNow_2;
				isOk = true;
			} while (false);
#pragma warning(default: 4127)
			if (isOk == false) {
				if (p) {
					delete p;
					p = 0;
				}
				cout << "hello, world: Setting __worldMatrixCacheArray: Failure" << endl;
				return -1;
			} else {
				((kBodyMaya*) this)->__worldMatrixCacheArray.append(p);
			}
		} else {
#if 1
			time.setValue(now);
			t0 = time.value();
			time.setValue(now - deltaTime);
			t_1 = time.value();
			time.setValue(now - 2 * deltaTime);
			t_2 = time.value();
#endif
		}
		if (isCached) {
			const WorldMatrixCache&	worldMatrixCache = __worldMatrixCacheArray(index);
			mNow0 = worldMatrixCache.worldMatrix;
			mNow_1 = worldMatrixCache.worldMatrix_1;
			mNow_2 = worldMatrixCache.worldMatrix_2;
		} else {
		}
#else
		ms = time.setValue(now); if (!ms) ms.perror("MTime::setValue");
		t0 = time.value();
		kUtil::getWorldMatrixAt(*((MDagPath*) this), time, mNow0);
		mNow0 *= __exclusiveMatrix;
		ms = time.setValue(now - deltaTime); if (!ms) ms.perror("MTime::setValue");
		t_1 = time.value();
		kUtil::getWorldMatrixAt(*((MDagPath*) this), time, mNow_1);
		mNow_1 *= __exclusiveMatrix;
		ms = time.setValue(now - 2 * deltaTime); if (!ms) ms.perror("MTime::setValue");
		t_2 = time.value();
		kUtil::getWorldMatrixAt(*((MDagPath*) this), time, mNow_2);
		mNow_2 *= __exclusiveMatrix;
#endif
	} else {
		mNow0 = __inclusiveMatrix;
	}
#endif
	MPoint	basePoint;
	basePoint.x = T0_in_kObject.position[0];
	basePoint.y = T0_in_kObject.position[1];
	basePoint.z = T0_in_kObject.position[2];
	MPoint	p0 = basePoint * mNow0;
	inStart.T0.position[0] = p0.x;
	inStart.T0.position[1] = p0.y;
	inStart.T0.position[2] = p0.z;
	if (isAnimated) {
		MPoint	p_1 = basePoint * mNow_1;
		MVector	p10(p0 - p_1);
		p10 /= t0 - t_1;
		inStart.T1.position[0] = p10.x;
		inStart.T1.position[1] = p10.y;
		inStart.T1.position[2] = p10.z;
		MPoint	p_2 = basePoint * mNow_2;
		MVector	p21(p_1 - p_2);
		p21 /= t_1 - t_2;
		MVector	p012(p10 - p21);
		p012 /= (t0 - t_2) / 2;
		inStart.T2.position[0] = p012.x;
		inStart.T2.position[1] = p012.y;
		inStart.T2.position[2] = p012.z;
	} else {
		inStart.T1.position[0] = 0.;
		inStart.T1.position[1] = 0.;
		inStart.T1.position[2] = 0.;
		inStart.T2.position[0] = 0.;
		inStart.T2.position[1] = 0.;
		inStart.T2.position[2] = 0.;
	}
	MPoint	point;
	point.x = T0_in_kObject.normal[0];
	point.y = T0_in_kObject.normal[1];
	point.z = T0_in_kObject.normal[2];
	MVector	n0(point * mNow0 - basePoint * mNow0);
	n0.normalize();
	inStart.T0.normal[0] = n0.x;
	inStart.T0.normal[1] = n0.y;
	inStart.T0.normal[2] = n0.z;
	MVector	tangent(T0_in_kObject.tangent);
	MVector	v0 = tangent;
	v0 = v0.transformAsNormal(mNow0);
	inStart.T0.tangent[0] = v0.x;
	inStart.T0.tangent[1] = v0.y;
	inStart.T0.tangent[2] = v0.z;
	if (isAnimated) {
		MVector	v_1 = tangent;
		v_1 = v_1.transformAsNormal(mNow_1);
		MVector	v10(v0 - v_1);
		v10 /= t0 - t_1;
		inStart.T1.tangent[0] = v10.x;
		inStart.T1.tangent[1] = v10.y;
		inStart.T1.tangent[2] = v10.z;
		MVector	v_2 = tangent;
		v_2 = v_2.transformAsNormal(mNow_2);
		MVector	v21(v_1 - v_2);
		v21 /= t_1 - t_2;
		MVector	v012(v10 - v21);
		v012 /= (t0 - t_2) / 2;
		inStart.T2.tangent[0] = v012.x;
		inStart.T2.tangent[1] = v012.y;
		inStart.T2.tangent[2] = v012.z;
	} else {
		inStart.T1.tangent[0] = 0.;
		inStart.T1.tangent[1] = 0.;
		inStart.T1.tangent[2] = 0.;
		inStart.T2.tangent[0] = 0.;
		inStart.T2.tangent[1] = 0.;
		inStart.T2.tangent[2] = 0.;
	}
#ifdef _DEBUG3
	cout << "(" << kT.nthFrame() << ", " << kT.nthStep() << "): t = " << now << "(" << t0 << "): " << endl;
	cout << "\tT0.position: [";
	cout << inStart.T0.position[0] << ", " << inStart.T0.position[1] << ", " << inStart.T0.position[2];
	cout << "]" << endl;
	cout << "\tT0.tangent: [";
	cout << inStart.T0.tangent[0] << ", " << inStart.T0.tangent[1] << ", " << inStart.T0.tangent[2];
	cout << "]" << endl;
	cout << "\tT0.normal: [";
	cout << inStart.T0.normal[0] << ", " << inStart.T0.normal[1] << ", " << inStart.T0.normal[2];
	cout << "]" << endl;
	cout << "\tT1.position: [";
	cout << inStart.T1.position[0] << ", " << inStart.T1.position[1] << ", " << inStart.T1.position[2];
	cout << "]" << endl;
	cout << "\tT1.tangent: [";
	cout << inStart.T1.tangent[0] << ", " << inStart.T1.tangent[1] << ", " << inStart.T1.tangent[2];
	cout << "]" << endl;
	cout << "\tT2.position: [";
	cout << inStart.T2.position[0] << ", " << inStart.T2.position[1] << ", " << inStart.T2.position[2];
	cout << "]" << endl;
	cout << "\tT2.tangent: [";
	cout << inStart.T2.tangent[0] << ", " << inStart.T2.tangent[1] << ", " << inStart.T2.tangent[2];
	cout << "]" << endl;
#endif
	return 0;
}
#if 1
#if 1
MStatus	kBodyMaya::setBody(const MDagPath& bodyShape)
{
	MStatus	ms;
	ms = MDagPath::set(bodyShape);
	if (!ms) {
		cerr << "Error: MayaBody::set: MDagPath::set(|" << bodyShape.fullPathName() << "|): Failure: " << ms << endl;
		return ms;
	}
	__objectTransform = MDagPath::transform(&ms);
	if (!ms) {
		cerr << "Error: MayaBody::set: MDagPath::transform(): Failure: " << ms << endl;
		return ms;
	}
	MFnTransform	transFn(__objectTransform);
	MTransformationMatrix	transformMatrix = transFn.transformation();
	__rotationOrder = transformMatrix.rotationOrder();
	MFnDagNode	fnDagNodeTransform(__objectTransform);
	MDagPath	dagPathTransform;
	fnDagNodeTransform.getPath(dagPathTransform);
	__inclusiveMatrix = dagPathTransform.inclusiveMatrix(&ms);
	if (!ms) {
		cerr << "MayaBody::set: MDagPath::inclusiveMatrix(): Failure: " << ms << endl;
		return ms;
	}
	__exclusiveMatrix = dagPathTransform.exclusiveMatrix(&ms);
	if (!ms) {
		cerr << "MayaBody::set: MDagPath::exclusiveMatrix(): Failure: " << ms << endl;
		return ms;
	}
	ms = __mesh.setObject(bodyShape);
	if (!ms) {
		cerr << "Error: MayaBody::set: MFnMesh::setObject(|" << bodyShape.fullPathName() << "|): Failure: " << ms << endl;
		return ms;
	}
	return MStatus::kSuccess;
}
#endif
#if 1
bool	kBodyMaya::getNearestSurfacePoint(
	const kTime&		kT,
	const ShVector3&	in,
	ShVector3&		out
) const
{
	return getNearestSurfacePoint(kT(), in, out);
}
bool	kBodyMaya::getNearestSurfacePoint(
	double			t,
	const ShVector3&	in,
	ShVector3&		out
) const
{
	cout << "kBodyMaya::getNearestSurfacePoint: t = " << t << endl;
	MStatus	ms;
	bool	isAnimated = false;
	ms = __isReady(isAnimated);
	if (!ms) {
		cerr << "Error: kBodyMaya::getNearestSurfacePoint: kBodyMaya::__isReady(): Failure: " << ms << endl;
		return false;
	}
	MMatrix	worldMatrix;
	if (isAnimated) {
		MTime	time;
		time.setUnit(MTime::kSeconds);
		time.setValue(t);
		MMatrix	m;
		ms = kUtil::getWorldMatrixAt(*((MDagPath*) this), time, m, &__rotationOrder);
		if (!ms) {
			cerr << "Error: kBodyMaya::getNearestSurfacePoint: getWorldMatrixAt(): Failure: " << ms << endl;
			return false;
		}
		worldMatrix = m * __exclusiveMatrix;
	} else {
		worldMatrix = __inclusiveMatrix;
	}
	MMatrix	inverseWorldMatrix = worldMatrix.inverse();
	MPoint	inP(in[0], in[1], in[2]);
	inP *= inverseWorldMatrix;
	MPoint	outP;
	MVector	normal;
#if 1
	const MFnMesh&	mesh = __mesh;
#else
	MFnMesh	mesh(__externalDagPathShape, &ms);
	if (!ms) {
		cerr << "kBodyMaya: MFnMesh::MFnMesh(const MDagPath&): Failure" << endl;
		return false;
	}
#endif
	if (!mesh.getClosestPointAndNormal(inP, outP, normal)) {
		cerr << "kBodyMaya::getClosestPointAndNormal: MFnMesh::getClosestPointAndNormal(): Failure" << endl;
		return false;
	}
	MVector	N(outP - inP);
	if (N.x * normal.x + N.y * normal.y + N.z * normal.z < 0)
		return false;
	outP *= worldMatrix;
	out[0] = outP.x;
	out[1] = outP.y;
	out[2] = outP.z;
	return true;
}
bool	kBodyMaya::getNearestSurfacePoint(ShVector3 in, ShVector3 out)
{
	ShVector3	in0; in0[0] = in[0]; in0[1] = in[1]; in0[2] = in[2];
	ShVector3	out0;
	bool	rval = kBodyMaya::getNearestSurfacePoint(currentBodyTime, in0, out0);
	out[0] = out0[0]; out[1] = out0[1]; out[2] = out0[2];
	return rval;
}
#endif
#endif
