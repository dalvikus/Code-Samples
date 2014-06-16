#include <iostream>
	using std::cout;
	using std::cerr;
	using std::endl;
#include <mesh/kBone.h>
#include <mesh/kMesh.h>


kBone::kBone()
{
}

kBone::~kBone()
{
}


void	kBone::set(const kMesh& mesh, const PLANE_DEF& planeDef, const POINT_DEF* pointDefPtr, float fScale, bool both, const D3DXVECTOR3* __P0Ptr)
{
	if (!&mesh) {
		cerr << "FATAL: kBone::set: mesh is nil" << endl;
		return;
	}

	__inside.clear();
	__outside.clear();
	__insideFaceIDArray.clear();
	__outsideFaceIDArray.clear();

	// PLANE_DEF::normal 이 제대로된 방향인지 확인한 후, 설정한다.
	D3DXVECTOR3	P1;
	if (mesh.getPoint(planeDef.pointDef, P1) < 0) {
		cerr << "ERROR: kBone::set: PLANE_DEF: kMesh::getPoint(POINT_DEF): Failure" << endl;
		return;
	}
	D3DXVECTOR3	P2;
	if (pointDefPtr) {
		if (mesh.getPoint(*pointDefPtr, P2) < 0) {
			cerr << "ERROR: kBone::set: POINT_DEF: kMesh::getPoint(POINT_DEF): Failure" << endl;
			return;
		}
	}
	PLANE_DEF&	thisPlaneDef = *((PLANE_DEF*) this);
	POINT_DEF&	thisPointDef = *((POINT_DEF*) this);
	if (pointDefPtr) {
		thisPointDef = *pointDefPtr;
		if (D3DXVec3Dot(&planeDef.normal, &D3DXVECTOR3(P2 - P1)) < 0) {
			cerr << "INFO: kBone::set: PLANE_DEF::normal: anti-direction: reversed" << endl;

			thisPlaneDef.pointDef = planeDef.pointDef;
			thisPlaneDef.normal = -planeDef.normal;
		} else {
			thisPlaneDef = planeDef;
		}
	} else {
		thisPlaneDef = planeDef;
		POINT_DEF	invalidPointDef = {(DWORD) -1, 0, 0};
		thisPointDef = invalidPointDef;
	}
	D3DXVec3Normalize(&thisPlaneDef.normal, &thisPlaneDef.normal);	// 다시 확인

	// 기준방향/기준점 설정
	D3DXVECTOR3&	nDir = thisPlaneDef.normal;	// 기준이 되는 방향
	if (__P0Ptr) {
		__P0_star = *__P0Ptr;
	} else {
		if (both && pointDefPtr) {
			// P2 에서 PLANE_DEF 에 내린 수선의 발
			//	P2 에서 P0 에 이르는 벡터는
			//	P0 = P2 + t nDir
			//	P10 = P0 - P1 = (P2 - P1) + t nDir = P12 + t nDir
			//	P10 . nDir = 0 (서로 수직) 이라는 조건으로부터
			//	(P12 + t nDir) . nDir = P12 . nDir + t = 0
			//	t = -P12 . nDir
			//	따라서 P0 = P2 - (P12 . nDir) nDir
			__P0_star = P2 - D3DXVec3Dot(&D3DXVECTOR3(P2 - P1), &nDir) * nDir;
			__P0_star = P2;
		} else {
			__P0_star = P1;
		}
	}
	D3DXVECTOR3	P0;	// __P0_star 가 planeDef 에 내린 수선의 발
	P0 = __P0_star - D3DXVec3Dot(&D3DXVECTOR3(__P0_star - P1), &nDir) * nDir;
	// __P0_star 에서 임의의 점 P 에 이르는 벡터
	//	1) P 가 안쪽에 있을 때 (inside)
	//		__P0_star~P0 (길이 유지) + P0~P (길이 변화)
	//	2) P 가 바깥쪽에 있을 때 (outside)
	//		__P0_star~P0 (길이 유지) + P0~P2(길이 변화) + P2~P (길이 유지)
	cout << "P0*: [" << __P0_star.x << ", " << __P0_star.y << ", " << __P0_star.z << "]" << endl;
	cout << "P0: [" << P0.x << ", " << P0.y << ", " << P0.z << "]" << endl;
	cout << "P1: [" << P1.x << ", " << P1.y << ", " << P1.z << "]" << endl;
	cout << "P2: [" << P2.x << ", " << P2.y << ", " << P2.z << "]" << endl;
#if 1	// 테스트 점(__P0_star)
	if (pointDefPtr) {
		float	len = D3DXVec3Dot(&D3DXVECTOR3(P2 - P1), &nDir);	// P2 에서 planeDef 에 이르는 거리
		D3DXVECTOR3	A = P1 - len * nDir;	// 왼쪽 바깥쪽
		D3DXVECTOR3	B = P1 + .5f * len * nDir;	// 안쪽 중간지점
		D3DXVECTOR3	C = P1 + 2.f * len * nDir;	// 오른쪽 바깥쪽
		cout << "A: [" << A.x << ", " << A.y << ", " << A.z << "]" << endl;
		cout << "B: [" << B.x << ", " << B.y << ", " << B.z << "]" << endl;
		cout << "C: [" << C.x << ", " << C.y << ", " << C.z << "]" << endl;
	}
#endif

	// {in,out}sideFaceIDArray, {in,out}sidePointReprIDArray 를 결정
	if (pointDefPtr) {
		kMesh::CollectBetweenFaces(thisPlaneDef, thisPointDef, __insideFaceIDArray, &mesh);
	} else {
		PLANE_DEF	insidePlaneDef;
		insidePlaneDef.pointDef = thisPlaneDef.pointDef;
		insidePlaneDef.normal = -thisPlaneDef.normal;
		kMesh::CollectLowerFaces(NULL, insidePlaneDef, __insideFaceIDArray, &mesh);
	}
	if (pointDefPtr) {
		PLANE_DEF	outsidePlaneDef;
		outsidePlaneDef.pointDef = *pointDefPtr;
		outsidePlaneDef.normal = -thisPlaneDef.normal;
		kMesh::CollectLowerFaces(NULL, outsidePlaneDef, __outsideFaceIDArray, &mesh);
	}

	DWORD*	insidePointReprIDArray = 0;
	DWORD*	outsidePointReprIDArray = 0;
	const CFace*	faceArray = mesh.GetFacePointer();
do {
	insidePointReprIDArray = new DWORD[mesh.GetNumPointReprs()];
	FillMemory(insidePointReprIDArray, mesh.GetNumPointReprs() * sizeof(DWORD), 0);
	for (int n = 0; n < (int) __insideFaceIDArray.size(); ++n) {
		const CFace&	face = faceArray[__insideFaceIDArray.at(n)];
		for (int k = 0; k < 3; ++k)
			insidePointReprIDArray[face.prID[k]] = 1;
	}
	outsidePointReprIDArray = new DWORD[mesh.GetNumPointReprs()];
	FillMemory(outsidePointReprIDArray, mesh.GetNumPointReprs() * sizeof(DWORD), 0);
	for (int n = 0; n < (int) __outsideFaceIDArray.size(); ++n) {
		const CFace&	face = faceArray[__outsideFaceIDArray.at(n)];
		for (int k = 0; k < 3; ++k)
			outsidePointReprIDArray[face.prID[k]] = 1;
	}

	// 사이에 있는 점들을 nDir 방향을 따라 늘인다;
	// 경계(pointDef)에서 연속성을 갖도록 벗어난 곳에 있는 점들(nDir 방향)을 전체적으로 이동시킨다.
	D3DXVECTOR3	ell_INSIDE;	// pointDefPtr 이 NULL 이면 의미가 없다;
					// __outsideFaceIDArray 는 없으니 아래에서 사용되지 않는다.
	if (pointDefPtr) {
#if 1
		ell_INSIDE = D3DXVec3Dot(&D3DXVECTOR3(P2 - P1), &nDir) * nDir;
#else
		ell_INSIDE = D3DXVec3Dot(&D3DXVECTOR3(P2 - __P0_star), &nDir) * nDir;
#endif
	}
	CPointRepr::VERTEX*	vertexArray;
	mesh.meshLoader().meshSubsetPtr()->LockVertexBuffer(0, (void**) &vertexArray);
	const CPointRepr*	pointReprArray = mesh.GetPointReprPointer();
	for (int n = 0; n < (int) mesh.GetNumPointReprs(); ++n) {
		const CPointRepr&	pointRepr = pointReprArray[n];
		if (insidePointReprIDArray[n]) {
#if 1
			D3DXVECTOR3	ell = pointRepr.v.position - __P0_star;
			// __P0_star~P0 (길이 유지) + P0~P (길이 변화)
			D3DXVECTOR3	ell_longitudinal = D3DXVec3Dot(&ell, &nDir) * nDir;
			// __P0_star~P0, P0~P 로 분리
			D3DXVECTOR3	ell_longitudinal0 = P0 - __P0_star;
			D3DXVECTOR3	ell_inside = ell_longitudinal - ell_longitudinal0;
			D3DXVECTOR3	ell_transversal = ell - ell_longitudinal;
			float	scale_transversal = 1.f;
			if (both) {
				if (pointDefPtr) {
					// P0~P2 사이를 세 구간으로 나눈다.
					// P0~P3, P3~P4, P4~P2
					// P0~P3, P4~P2 사이는 scale_transversal 값을
					// 1부터 fScale 까지 변화시킨다. (smoothstep 함수를 사용)
					// P3~P4 사이는 scale_transversal 값으로 fScale 값을 사용한다.
					// P0~P3 구간의 크기와 P4~P2 구간의 크기는 서로 다르나
					// 여기서는 같다고 가정하고 그것도 P0~P2 구간의 10%라고 가정한다.
					// (TO DO LIST)
					const	float	left_range = 0.4f;
					assert(left_range <= .5f);
					const	float	right_range = 1.f - left_range;
					float	length0 = D3DXVec3Length(&ell_INSIDE);
					float	length = D3DXVec3Length(&ell_inside);
					if (length < length0 * left_range) {		// P0~P3 구간
						scale_transversal = smoothstep(0.f, 1.f, length0 * left_range, fScale, length);
					} else if (length > length0 * right_range) {	// P4~P2 구간
						scale_transversal = smoothstep(length0 * right_range, fScale, length0, 1.f, length);
					} else {				// P3~P4 구간
						scale_transversal = fScale;
					}
				} else {
					// TO DO LIST;
					// 제일 끝 점을 찾아 그 점을 P2 로 해서 앞 결과를 이용한다.
					(void) fprintf(stderr, "WARNING: kBone::set: both but pointDefPtr is nil: TO DO LIST\n");
				}
			}
#if 1
			__inside[n] = __P0_star + ((ell_longitudinal0 + fScale * ell_inside) + (both ? scale_transversal : 1) * ell_transversal);
#else
			__inside[n] = __P0_star + ((ell_longitudinal0 + fScale * ell_inside) + (both ? fScale : 1) * ell_transversal);
#endif
#else
			D3DXVECTOR3	ell = pointRepr.v.position - __P0_star;
			D3DXVECTOR3	ell_longitudinal = D3DXVec3Dot(&ell, &nDir) * nDir;
			D3DXVECTOR3	ell_transversal = ell - ell_longitudinal;
			__inside[n] = __P0_star + (fScale * ell_longitudinal + (both ? fScale : 1) * ell_transversal);
#endif
		}
		if (outsidePointReprIDArray[n]) {
#if 1
			D3DXVECTOR3	ell = pointRepr.v.position - __P0_star;
			// __P0_star~P1 (길이 유지) + P0~P2 (길이 변화) + P2~P (길이 유지)
			D3DXVECTOR3	ell_longitudinal = D3DXVec3Dot(&ell, &nDir) * nDir;
			// __P0_star~P1, P0~P2(ell_INSIDE), P2~P 로 분리
			D3DXVECTOR3	ell_longitudinal0 = P0 - __P0_star;
			D3DXVECTOR3	ell_outside = D3DXVec3Dot(&D3DXVECTOR3(pointRepr.v.position - P2), &nDir) * nDir;
			D3DXVECTOR3	ell_transversal = ell - ell_longitudinal;
			__outside[n] = __P0_star + ((ell_longitudinal0 + fScale * ell_INSIDE + ell_outside) + ell_transversal);
#else
			D3DXVECTOR3	ell = pointRepr.v.position - __P0_star;
			D3DXVECTOR3	ell_longitudinal = D3DXVec3Dot(&ell, &nDir) * nDir;
			D3DXVECTOR3	ell_transversal = ell - ell_longitudinal;
			// ell_longitudinal 은 __insideFaceIDArray 에 있는 부분과
			// __outsideFaceIDArray 에 있는 부분으로 나눈다.
			D3DXVECTOR3	ell_outside = ell_longitudinal - ell_INSIDE;
			__outside[n] = __P0_star + ((fScale * ell_INSIDE + ell_outside) + ell_transversal);
#endif
		}
	}
	mesh.meshLoader().meshSubsetPtr()->UnlockVertexBuffer();
} while (0);
	if (insidePointReprIDArray) {delete[] insidePointReprIDArray; insidePointReprIDArray = 0;}
	if (outsidePointReprIDArray) {delete[] outsidePointReprIDArray; outsidePointReprIDArray = 0;}
}
#if 1	// kDisplacementMap
void	kBone::set2(
	const kMesh&	mesh,
	const PLANE_DEF&	planeDef,
	const POINT_DEF*	pointDefPtr,
	float	fScale,
	bool	both,
	const D3DXVECTOR3*	__P0Ptr,
	const kDisplacementMap*	dismap_ptr
)
{
	if (!&mesh) {
		cerr << "FATAL: kBone::set: mesh is nil" << endl;
		return;
	}

	__inside.clear();
	__outside.clear();
	__insideFaceIDArray.clear();
	__outsideFaceIDArray.clear();

	// PLANE_DEF::normal 이 제대로된 방향인지 확인한 후, 설정한다.
	D3DXVECTOR3	P1;
	if (mesh.getPoint(planeDef.pointDef, P1) < 0) {
		cerr << "ERROR: kBone::set: PLANE_DEF: kMesh::getPoint(POINT_DEF): Failure" << endl;
		return;
	}
	D3DXVECTOR3	P2;
	if (pointDefPtr) {
		if (mesh.getPoint(*pointDefPtr, P2) < 0) {
			cerr << "ERROR: kBone::set: POINT_DEF: kMesh::getPoint(POINT_DEF): Failure" << endl;
			return;
		}
	}
	PLANE_DEF&	thisPlaneDef = *((PLANE_DEF*) this);
	POINT_DEF&	thisPointDef = *((POINT_DEF*) this);
	if (pointDefPtr) {
		thisPointDef = *pointDefPtr;
		if (D3DXVec3Dot(&planeDef.normal, &D3DXVECTOR3(P2 - P1)) < 0) {
			cerr << "INFO: kBone::set: PLANE_DEF::normal: anti-direction: reversed" << endl;

			thisPlaneDef.pointDef = planeDef.pointDef;
			thisPlaneDef.normal = -planeDef.normal;
		} else {
			thisPlaneDef = planeDef;
		}
	} else {
		thisPlaneDef = planeDef;
		POINT_DEF	invalidPointDef = {(DWORD) -1, 0, 0};
		thisPointDef = invalidPointDef;
	}
	D3DXVec3Normalize(&thisPlaneDef.normal, &thisPlaneDef.normal);	// 다시 확인

	// 기준방향/기준점 설정
	D3DXVECTOR3&	nDir = thisPlaneDef.normal;	// 기준이 되는 방향
	if (__P0Ptr) {
		__P0_star = *__P0Ptr;
	} else {
		if (both && pointDefPtr) {
			// P2 에서 PLANE_DEF 에 내린 수선의 발, P0
			//	P2 에서 P0 에 이르는 벡터는
			//	P0 = P2 + t nDir
			//	P10 = P0 - P1 = (P2 - P1) + t nDir = P12 + t nDir
			//	P10 . nDir = 0 (서로 수직) 이라는 조건으로부터
			//	(P12 + t nDir) . nDir = P12 . nDir + t = 0
			//	t = -P12 . nDir
			//	따라서 P0 = P2 - (P12 . nDir) nDir
			__P0_star = P2 - D3DXVec3Dot(&D3DXVECTOR3(P2 - P1), &nDir) * nDir;
			__P0_star = P2;
		} else {
			__P0_star = P1;
		}
	}
	D3DXVECTOR3	P0;	// __P0_star 가 planeDef 에 내린 수선의 발
	P0 = __P0_star - D3DXVec3Dot(&D3DXVECTOR3(__P0_star - P1), &nDir) * nDir;
	// __P0_star 에서 임의의 점 P 에 이르는 벡터
	//	1) P 가 안쪽에 있을 때 (inside)
	//		__P0_star~P0 (길이 유지) + P0~P (길이 변화)
	//	2) P 가 바깥쪽에 있을 때 (outside)
	//		__P0_star~P0 (길이 유지) + P0~P2(길이 변화) + P2~P (길이 유지)
	cout << "P0*: [" << __P0_star.x << ", " << __P0_star.y << ", " << __P0_star.z << "]" << endl;
	cout << "P0: [" << P0.x << ", " << P0.y << ", " << P0.z << "]" << endl;
	cout << "P1: [" << P1.x << ", " << P1.y << ", " << P1.z << "]" << endl;
	cout << "P2: [" << P2.x << ", " << P2.y << ", " << P2.z << "]" << endl;
#if 1	// 테스트 점(__P0_star)
	if (pointDefPtr) {
		float	len = D3DXVec3Dot(&D3DXVECTOR3(P2 - P1), &nDir);	// P2 에서 planeDef 에 이르는 거리
		D3DXVECTOR3	A = P1 - len * nDir;	// 왼쪽 바깥쪽
		D3DXVECTOR3	B = P1 + .5f * len * nDir;	// 안쪽 중간지점
		D3DXVECTOR3	C = P1 + 2.f * len * nDir;	// 오른쪽 바깥쪽
		cout << "A: [" << A.x << ", " << A.y << ", " << A.z << "]" << endl;
		cout << "B: [" << B.x << ", " << B.y << ", " << B.z << "]" << endl;
		cout << "C: [" << C.x << ", " << C.y << ", " << C.z << "]" << endl;
	}
#endif

	// {in,out}sideFaceIDArray, {in,out}sidePointReprIDArray 를 결정
	if (pointDefPtr) {
		kMesh::CollectBetweenFaces(thisPlaneDef, thisPointDef, __insideFaceIDArray, &mesh);
	} else {
		PLANE_DEF	insidePlaneDef;
		insidePlaneDef.pointDef = thisPlaneDef.pointDef;
		insidePlaneDef.normal = -thisPlaneDef.normal;
		kMesh::CollectLowerFaces(NULL, insidePlaneDef, __insideFaceIDArray, &mesh);
	}
	if (pointDefPtr) {
		PLANE_DEF	outsidePlaneDef;
		outsidePlaneDef.pointDef = *pointDefPtr;
		outsidePlaneDef.normal = -thisPlaneDef.normal;
		kMesh::CollectLowerFaces(NULL, outsidePlaneDef, __outsideFaceIDArray, &mesh);
	}

	DWORD*	insidePointReprIDArray = 0;
	DWORD*	outsidePointReprIDArray = 0;
	const CFace*	faceArray = mesh.GetFacePointer();
do {
	insidePointReprIDArray = new DWORD[mesh.GetNumPointReprs()];
	FillMemory(insidePointReprIDArray, mesh.GetNumPointReprs() * sizeof(DWORD), 0);
	for (int n = 0; n < (int) __insideFaceIDArray.size(); ++n) {
		const CFace&	face = faceArray[__insideFaceIDArray.at(n)];
		for (int k = 0; k < 3; ++k)
			insidePointReprIDArray[face.prID[k]] = 1;
	}
	outsidePointReprIDArray = new DWORD[mesh.GetNumPointReprs()];
	FillMemory(outsidePointReprIDArray, mesh.GetNumPointReprs() * sizeof(DWORD), 0);
	for (int n = 0; n < (int) __outsideFaceIDArray.size(); ++n) {
		const CFace&	face = faceArray[__outsideFaceIDArray.at(n)];
		for (int k = 0; k < 3; ++k)
			outsidePointReprIDArray[face.prID[k]] = 1;
	}

	// 사이에 있는 점들을 nDir 방향을 따라 늘인다;
	// 경계(pointDef)에서 연속성을 갖도록 벗어난 곳에 있는 점들(nDir 방향)을 전체적으로 이동시킨다.
	D3DXVECTOR3	ell_INSIDE;	// pointDefPtr 이 NULL 이면 의미가 없다;
					// __outsideFaceIDArray 는 없으니 아래에서 사용되지 않는다.
	if (pointDefPtr) {
#if 0
		ell_INSIDE = D3DXVec3Dot(&D3DXVECTOR3(P2 - P1), &nDir) * nDir;
#else
		ell_INSIDE = D3DXVec3Dot(&D3DXVECTOR3(P2 - __P0_star), &nDir) * nDir;
#endif
	}
	CPointRepr::VERTEX*	vertexArray;
	mesh.meshLoader().meshSubsetPtr()->LockVertexBuffer(0, (void**) &vertexArray);
	const CPointRepr*	pointReprArray = mesh.GetPointReprPointer();
	for (int n = 0; n < (int) mesh.GetNumPointReprs(); ++n) {
		const CPointRepr&	pointRepr = pointReprArray[n];
		if (insidePointReprIDArray[n]) {
#if 1	// kDisplacementMap
		const CPointRepr::VERTEX&	v = pointRepr.v;
		const D3DXVECTOR2&	texcoord = v.texcoord;
#if 0
		float	u0 = texcoord.x;
		float	v0 = texcoord.y;
#endif
#endif	// kDisplacementMap
#if 1
			D3DXVECTOR3	ell = pointRepr.v.position - __P0_star;
			// __P0_star~P0 (길이 유지) + P0~P (길이 변화)
			D3DXVECTOR3	ell_longitudinal = D3DXVec3Dot(&ell, &nDir) * nDir;
			// __P0_star~P0, P0~P 로 분리
			D3DXVECTOR3	ell_longitudinal0 = P0 - __P0_star;
			D3DXVECTOR3	ell_inside = ell_longitudinal - ell_longitudinal0;
			D3DXVECTOR3	ell_transversal = ell - ell_longitudinal;
			float	scale_transversal = 1.f;
			if (both) {
				if (pointDefPtr) {
					// P0~P2 사이를 세 구간으로 나눈다.
					// P0~P3, P3~P4, P4~P2
					// P0~P3, P4~P2 사이는 scale_transversal 값을
					// 1부터 fScale 까지 변화시킨다. (smoothstep 함수를 사용)
					// P3~P4 사이는 scale_transversal 값으로 fScale 값을 사용한다.
					// P0~P3 구간의 크기와 P4~P2 구간의 크기는 서로 다르나
					// 여기서는 같다고 가정하고 그것도 P0~P2 구간의 10%라고 가정한다.
					// (TO DO LIST)
					const	float	left_range = 0.3f;
					assert(left_range <= .5f);
					const	float	right_range = 1.f - left_range;
					float	length0 = D3DXVec3Length(&ell_INSIDE);
					float	length = D3DXVec3Length(&ell_inside);
					if (length < length0 * left_range) {		// P0~P3 구간
						scale_transversal = smoothstep(0.f, 1.f, length0 * left_range, fScale, length);
					} else if (length > length0 * right_range) {	// P4~P2 구간
						scale_transversal = smoothstep(length0 * right_range, fScale, length0, 1.f, length);
					} else {				// P3~P4 구간
						scale_transversal = fScale;
					}
#if 1	// kDisplacementMap
		if (dismap_ptr) {
			float	val;
			Gdiplus::Status	status = dismap_ptr->GetPixelAtUV(texcoord.x, texcoord.y, val, kDisplacementMap::CHANNEL_RED);
			if (status != Gdiplus::Ok) {
				cerr << "kDisplacementMap::GetPixel(): Failure" << endl;
			} else {
				val /= dismap_ptr->MaxRed() / (float) 255;	// clamp to [0, 1]
				float	scale_transversal0 = scale_transversal;
				if (fScale > 1.f) {
					// f = 1 + factor = 1 + alpha * factor
					float	factor = scale_transversal0 - 1.f;
					factor *= val;
					scale_transversal = 1.f + factor;
				} else {
					// f = 1 - factor = 1 - alpha * factor
					float	factor = 1.f - scale_transversal0;
					factor *= val;
					scale_transversal = 1.f - factor;
				}
#if 0	// verbose
				cout << "scale_transversal0 = " << scale_transversal0 << ", alpha = " << val << ", scale_transversal = " << scale_transversal << endl;
#endif	// verbose
#if 0	// 주어진 Point Repr. ID 에 해당하는 Vertex ID 는 둘 이상일 수 있다;
	// 따라서 (u, v) 가 둘 이상일 수 있다. 하지만 Texture Map 에서 읽었을 때의 값은 같아야 한다.
	// 이를 확인한다.
#ifdef __USE_A_OFFSET__
	unsigned	numVertices = mesh.pointer_to__a_vVertexID_a_offset[n + 1] - mesh.pointer_to__a_vVertexID_a_offset[n];
#else
	unsigned	numVertices = pointRepr.vVertexID.size();
#endif
	if (numVertices > 1) {
		cout << "prID = " << n << ": # of vertices = " << numVertices << endl;
		cout << "(u0 = " << u0 << ", v0 = " << v0 << ")" << endl;
#ifdef __USE_A_OFFSET__
		for (int k = mesh.pointer_to__a_vVertexID_a_offset[n]; k < mesh.pointer_to__a_vVertexID_a_offset[n + 1]; ++k) {
			DWORD	vertexID = mesh.pointer_to__a_vVertexID[k];
#else
		for (std::set<DWORD>::const_iterator it = pointRepr.vVertexID.begin(); it != pointRepr.vVertexID.end(); ++it) {
			DWORD	vertexID = *it;
#endif
			cout << "vertexID = " << vertexID << endl;
			CPointRepr::VERTEX&	vertex = vertexArray[vertexID];
			float	val;
			Gdiplus::Color	color_at_floored_uv;
			Gdiplus::Status	status = dismap_ptr->GetPixelAtUV(vertex.texcoord.x, vertex.texcoord.y, val, kDisplacementMap::CHANNEL_RED, &color_at_floored_uv);
			cout << "color: " << color_at_floored_uv.GetValue() << endl;
#ifdef __USE_A_OFFSET__
		}
#else
		}
#endif
	}
#endif	// 확인
#if 0	// verbose
				(void) fprintf(stdout, "At (u = %f, v = %f): val = %f w/ ID = %d\n", texcoord.x, texcoord.y, val, n);
#endif	// verbose
			}
		}
#endif	// kDisplacementMap
				} else {
					// TO DO LIST;
					// 제일 끝 점을 찾아 그 점을 P2 로 해서 앞 결과를 이용한다.
					(void) fprintf(stderr, "WARNING: kBone::set: both but pointDefPtr is nil: TO DO LIST\n");
				}
			}
#if 1
			__inside[n] = __P0_star + ((ell_longitudinal0 + fScale * ell_inside) + (both ? scale_transversal : 1) * ell_transversal);
#else
			__inside[n] = __P0_star + ((ell_longitudinal0 + fScale * ell_inside) + (both ? fScale : 1) * ell_transversal);
#endif
#else
			D3DXVECTOR3	ell = pointRepr.v.position - __P0_star;
			D3DXVECTOR3	ell_longitudinal = D3DXVec3Dot(&ell, &nDir) * nDir;
			D3DXVECTOR3	ell_transversal = ell - ell_longitudinal;
			__inside[n] = __P0_star + (fScale * ell_longitudinal + (both ? fScale : 1) * ell_transversal);
#endif
		}
		if (outsidePointReprIDArray[n]) {
#if 1
			D3DXVECTOR3	ell = pointRepr.v.position - __P0_star;
			// __P0_star~P1 (길이 유지) + P0~P2 (길이 변화) + P2~P (길이 유지)
			D3DXVECTOR3	ell_longitudinal = D3DXVec3Dot(&ell, &nDir) * nDir;
			// __P0_star~P1, P0~P2(ell_INSIDE), P2~P 로 분리
			D3DXVECTOR3	ell_longitudinal0 = P0 - __P0_star;
			D3DXVECTOR3	ell_outside = D3DXVec3Dot(&D3DXVECTOR3(pointRepr.v.position - P2), &nDir) * nDir;
			D3DXVECTOR3	ell_transversal = ell - ell_longitudinal;
			__outside[n] = __P0_star + ((ell_longitudinal0 + fScale * ell_INSIDE + ell_outside) + ell_transversal);
#else
			D3DXVECTOR3	ell = pointRepr.v.position - __P0_star;
			D3DXVECTOR3	ell_longitudinal = D3DXVec3Dot(&ell, &nDir) * nDir;
			D3DXVECTOR3	ell_transversal = ell - ell_longitudinal;
			// ell_longitudinal 은 __insideFaceIDArray 에 있는 부분과
			// __outsideFaceIDArray 에 있는 부분으로 나눈다.
			D3DXVECTOR3	ell_outside = ell_longitudinal - ell_INSIDE;
			__outside[n] = __P0_star + ((fScale * ell_INSIDE + ell_outside) + ell_transversal);
#endif
		}
	}
	mesh.meshLoader().meshSubsetPtr()->UnlockVertexBuffer();
} while (0);
	if (insidePointReprIDArray) {delete[] insidePointReprIDArray; insidePointReprIDArray = 0;}
	if (outsidePointReprIDArray) {delete[] outsidePointReprIDArray; outsidePointReprIDArray = 0;}
}
#endif	// kDisplacementMap

void	kBone::apply(
	kMesh&	mesh,
	const std::set<DWORD>*	excludeSetPtr
)
{
	if (!&mesh) {
		cerr << "FATAL: kBone::set: mesh is nil" << endl;
		return;
	}

	HRESULT	hResult;
	CPointRepr*	pointReprArray = mesh.GetPointReprPointer();
	CFace*		faceArray = mesh.GetFacePointer();
	CEdge*		edgeArray = mesh.GetEdgePointer();
#ifdef __USE_A_OFFSET__
	const DWORD*	pointer_to__a_vVertexID				= mesh.pointer_to__a_vVertexID;
	const int*	pointer_to__a_vVertexID_a_offset		= mesh.pointer_to__a_vVertexID_a_offset;
	const EDGE*	pointer_to__a_vEdgeID				= mesh.pointer_to__a_vEdgeID;
	const int*	pointer_to__a_vEdgeID_a_offset			= mesh.pointer_to__a_vEdgeID_a_offset;
	const FACE_ID_LOCAL_INDEX*	pointer_to__a_vFaceIDLocalIndex	= mesh.pointer_to__a_vFaceIDLocalIndex;
	const int*	pointer_to__a_vFaceIDLocalIndex_a_offset	= mesh.pointer_to__a_vFaceIDLocalIndex_a_offset;
//	const DWORD*	pointer_to__a_faceID				= __meshPtr->pointer_to__a_faceID;
//	const int*	pointer_to__a_faceID_a_offset			= __meshPtr->pointer_to__a_faceID_a_offset;
#endif	// __USE_A_OFFSET__

#if 1	// Lock...
	CPointRepr::VERTEX*	vertexArray;	// 텍스쳐 메쉬 정보를 담은 Vertex Buffer
	mesh.meshLoader().meshSubsetPtr()->LockVertexBuffer(0, (void**) &vertexArray);
#if 1	// lock
	LPDIRECT3DVERTEXBUFFER9	pVB_n = mesh.showNormal().GetVertexBufferPointer();
	kShowNormal::Vertex*	pV_n;		// 노말 정보를 담은 Vertex Buffer
	pVB_n->Lock(0, 0, (void**) &pV_n, 0);

	kMesh::D3DXVERTEX* 	pV;	// kMesh::__vertexBufferPtr
	mesh.GetVertexBufferPointer()->Lock(0, 0, (void**) &pV, 0);

	FLOAT*	pVB_ = NULL;
	hResult = mesh.GetMeshPointer()->LockVertexBuffer(0, (void**) &pVB_);
	assert(D3D_OK == hResult);
#endif	// lock
#endif	// Lock...


#if 1	// Vertex 위치 변경
#if 1
#if 1	// __inside
{
	const VERTEX_HASH&	pointReprHash = __inside;
#if 1	// common
	for (VERTEX_HASH::const_iterator cit = pointReprHash.begin(); cit != pointReprHash.end(); ++cit) {
		DWORD	prID = (*cit).first;
		if (excludeSetPtr && excludeSetPtr->find(prID) != excludeSetPtr->end())
				continue;

		const D3DXVECTOR3&	p = (*cit).second;
		pV[prID].p = p;
		pV_n[2 * prID + 0].Position = p;
		pV_n[2 * prID + 1].Position = p;
		pVB_[8 * prID + 0] = p.x;
		pVB_[8 * prID + 1] = p.y;
		pVB_[8 * prID + 2] = p.z;
		pointReprArray[prID].v.position = p;
#ifdef __USE_A_OFFSET__
		for (int k = mesh.pointer_to__a_vVertexID_a_offset[prID]; k < mesh.pointer_to__a_vVertexID_a_offset[prID + 1]; ++k)
			vertexArray[mesh.pointer_to__a_vVertexID[k]].position = p;
#else
		const std::set<DWORD>&	vVertexID = pointReprArray[prID].vVertexID;
		for (std::set<DWORD>::const_iterator it = vVertexID.begin(); it != vVertexID.end(); ++it)
			vertexArray[*it].position = p;
#endif
	}
#endif	// common
}
#endif	// __inside
#if 1	// __outside
{
	const VERTEX_HASH&	pointReprHash = __outside;
#if 1	// common
	for (VERTEX_HASH::const_iterator cit = pointReprHash.begin(); cit != pointReprHash.end(); ++cit) {
		DWORD	prID = (*cit).first;
		if (excludeSetPtr && excludeSetPtr->find(prID) != excludeSetPtr->end())
				continue;

		const D3DXVECTOR3&	p = (*cit).second;
		pV[prID].p = p;
		pV_n[2 * prID + 0].Position = p;
		pV_n[2 * prID + 1].Position = p;
		pVB_[8 * prID + 0] = p.x;
		pVB_[8 * prID + 1] = p.y;
		pVB_[8 * prID + 2] = p.z;
		pointReprArray[prID].v.position = p;
#ifdef __USE_A_OFFSET__
		for (int k = mesh.pointer_to__a_vVertexID_a_offset[prID]; k < mesh.pointer_to__a_vVertexID_a_offset[prID + 1]; ++k)
			vertexArray[mesh.pointer_to__a_vVertexID[k]].position = p;
#else
		const std::set<DWORD>&	vVertexID = pointReprArray[prID].vVertexID;
		for (std::set<DWORD>::const_iterator it = vVertexID.begin(); it != vVertexID.end(); ++it)
			vertexArray[*it].position = p;
#endif
	}
#endif	// common
}
#endif	// __outside
#else
	for (int n = 0; n < (int) mesh.GetNumPointReprs(); ++n) {
		if (excludeSetPtr) {
			std::set<DWORD>::const_iterator	cit = excludeSetPtr->find(n);
			if (cit != excludeSetPtr->end())
				continue;
		}

		CPointRepr&	pointRepr = pointReprArray[n];
		CPointRepr::VERTEX&	v = pointRepr.v;
		VERTEX_HASH::const_iterator	cit = __inside.find(n);
		if (cit != __inside.end()) {
			v.position = (*cit).second;
#ifdef __USE_A_OFFSET__
			for (int k = pointer_to__a_vVertexID_a_offset[n]; k < pointer_to__a_vVertexID_a_offset[n + 1]; ++k)
				vertexArray[pointer_to__a_vVertexID[k]].position = v.position;
#else
			const std::set<DWORD>&	vVertexID = pointRepr.vVertexID;
			for (std::set<DWORD>::const_iterator it = vVertexID.begin(); it != vVertexID.end(); ++it)
				vertexArray[*it].position = v.position;
#endif
		}
		cit = __outside.find(n);
		if (cit != __outside.end()) {
			v.position = (*cit).second;
#ifdef __USE_A_OFFSET__
			for (int k = pointer_to__a_vVertexID_a_offset[n]; k < pointer_to__a_vVertexID_a_offset[n + 1]; ++k)
				vertexArray[pointer_to__a_vVertexID[k]].position = v.position;
#else
			const std::set<DWORD>&	vVertexID = pointRepr.vVertexID;
			for (std::set<DWORD>::const_iterator it = vVertexID.begin(); it != vVertexID.end(); ++it)
				vertexArray[*it].position = v.position;
#endif
		}
	}
#endif
#endif	// Vertex 위치 변경

#if 1	// Vertex 위치 변경; ZBrush Smooth Brush
#if 1	// __inside
cout << "W/ Smooth? " << (mesh.statePtr()->brush.flag.Smooth ? "True" : "False") << endl;
if (mesh.statePtr()->brush.flag.Smooth) {
for (int niter = 0; niter < 20; ++niter)
{
	const VERTEX_HASH&	pointReprHash = __inside;
	for (VERTEX_HASH::const_iterator cit = pointReprHash.begin(); cit != pointReprHash.end(); ++cit) {
		DWORD	prID = (*cit).first;
		if (excludeSetPtr && excludeSetPtr->find(prID) != excludeSetPtr->end())
				continue;

		kMesh::D3DXVERTEX&	v = pV[prID];
		v.pTmp = v.p;
	}
	for (VERTEX_HASH::const_iterator cit = pointReprHash.begin(); cit != pointReprHash.end(); ++cit) {
		DWORD	prID = (*cit).first;
		if (excludeSetPtr && excludeSetPtr->find(prID) != excludeSetPtr->end())
				continue;

		kMesh::D3DXVERTEX&	v = pV[prID];
/*
 * 각 Edge 를 구성하는 두 Vertex 의 중점이 각 Vertex 의 새로운 위치에 영향을 미친다;
 * Vertex 의 새로운 위치들의 centroid 와 원래 위치의 중점의 실제로 새로운 위치가 된다.
 */
		// 주어진 Vertex 를 한 끝으로 하는 모든 Edge 에서 Edge 의 중점을 구한다.
		// 이 점들의 centroid 와 원래 점의 중점이 새로운 위치가 된다.
		D3DXVECTOR3	centroid(0, 0, 0);
		unsigned	numPoints = 0;
#ifdef __USE_A_OFFSET__
		numPoints = pointer_to__a_vEdgeID_a_offset[prID + 1] - pointer_to__a_vEdgeID_a_offset[prID];
		for (int k = pointer_to__a_vEdgeID_a_offset[prID]; k < pointer_to__a_vEdgeID_a_offset[prID + 1]; ++k) {
			const EDGE	e = pointer_to__a_vEdgeID[k];
			const CEdge&	edge = edgeArray[e.id];
			centroid += .5f * (pV[edge.prStartID].p + pV[edge.prEndID].p);
		}
#else
		const CPointRepr&	pointRepr = pointReprArray[prID];
		numPoints = pointRepr.vEdgeID.size();
		for (size_t i = 0; i < pointRepr.vEdgeID.size(); ++i) {
			EDGE	e = pointRepr.vEdgeID.at(i);
			const CEdge&	edge = edgeArray[e.id];
			centroid += .5f * (pV[edge.prStartID].p + pV[edge.prEndID].p);
		}
#endif
//		cout << "numPoints = " << numPoints << endl;
		centroid /= (float) numPoints;
		v.pTmp = .5f * (v.p + centroid);
	}
	for (VERTEX_HASH::const_iterator cit = pointReprHash.begin(); cit != pointReprHash.end(); ++cit) {
		DWORD	prID = (*cit).first;
		if (excludeSetPtr && excludeSetPtr->find(prID) != excludeSetPtr->end())
				continue;

		D3DXVECTOR3&	p = pV[prID].p;
		p = pV[prID].pTmp;

		pV_n[2 * prID + 0].Position = p;
		pV_n[2 * prID + 1].Position = p;
		pVB_[8 * prID + 0] = p.x;
		pVB_[8 * prID + 1] = p.y;
		pVB_[8 * prID + 2] = p.z;
		pointReprArray[prID].v.position = p;
#if 1	// 텍스쳐 메쉬 변형
#ifdef __USE_A_OFFSET__
		for (int k = pointer_to__a_vVertexID_a_offset[prID]; k < pointer_to__a_vVertexID_a_offset[prID + 1]; ++k)
			vertexArray[pointer_to__a_vVertexID[k]].position = p;
#else
		const std::set<DWORD>&	vVertexID = pointReprArray[prID].vVertexID;
		for (std::set<DWORD>::const_iterator it = vVertexID.begin(); it != vVertexID.end(); ++it)
			vertexArray[*it].position = p;
#endif
#endif	// 텍스쳐 메쉬 변형
	}
}
}
#endif	// __inside
#endif	// Vertex 위치 변경; ZBrush Smooth Brush


#if 1	// Vertex 의 새로운 노말 결정
#if 1	// __inside
{
	const VERTEX_HASH&	pointReprHash = __inside;
	const std::vector<DWORD>&	faceIDArray = __insideFaceIDArray;
#if 1	// common
	// 각 Vertex 의 노말을 다시 계산;
	// Vertex 를 공유하는 Face 들의 노말의 평균이 해당 Vertex 의 노말
	// 1. 각 Face 의 노말을 저장할 임시 공간
	for (int i = 0; i < (int) faceIDArray.size(); ++i) {
		DWORD	faceID = faceIDArray.at(i);
		CFace&	face = faceArray[faceID];
		D3DXVECTOR3&	p0 = pV[face.prID0].p;
		D3DXVECTOR3&	p1 = pV[face.prID1].p;
		D3DXVECTOR3&	p2 = pV[face.prID2].p;
		D3DXVECTOR3	v01 = p1 - p0;
//		D3DXVec3Normalize(&v01, &v01);
		D3DXVECTOR3	v02 = p2 - p0;
//		D3DXVec3Normalize(&v02, &v02);
		D3DXVECTOR3&	normal = face.normal;
		D3DXVec3Cross(&normal, &v01, &v02);
		D3DXVec3Normalize(&normal, &normal);
	}
	// 2. 각 Vertex 의 노말을 공유하는 Face 들의 노말로부터 계산
	for (VERTEX_HASH::const_iterator cit = pointReprHash.begin(); cit != pointReprHash.end(); ++cit) {
		DWORD	prID = (*cit).first;
		if (excludeSetPtr && excludeSetPtr->find(prID) != excludeSetPtr->end())
				continue;

		D3DXVECTOR3&	normal = pV[prID].n;
		unsigned	numFaces = 0;
#ifdef __USE_A_OFFSET__
		numFaces = pointer_to__a_vFaceIDLocalIndex_a_offset[prID + 1] - pointer_to__a_vFaceIDLocalIndex_a_offset[prID];
#else
		numFaces = pointReprArray[prID].vFaceIdLocalIndex.size();
#endif
#ifdef __USE_A_OFFSET__
		for (int k = pointer_to__a_vFaceIDLocalIndex_a_offset[prID]; k < pointer_to__a_vFaceIDLocalIndex_a_offset[prID + 1]; ++k) {
			const FACE_ID_LOCAL_INDEX&	faceIDLocalIndex = pointer_to__a_vFaceIDLocalIndex[k];
#else
		for (size_t k = 0; k < pointReprArray[prID].vFaceIdLocalIndex.size(); ++k) {
			const FACE_ID_LOCAL_INDEX&	faceIDLocalIndex = pointReprArray[prID].vFaceIdLocalIndex.at(k);
#endif
			normal += faceArray[faceIDLocalIndex.ID].normal;
#ifdef __USE_A_OFFSET__
		}
#else
		}
#endif
		normal /= (float) numFaces;
		D3DXVec3Normalize(&normal, &normal);
		// __meshPtr 에 사용되는 노말도 업데이트
	/*
		pVertices[i].n.x = pVB[8 * i + 3];
		pVertices[i].n.y = pVB[8 * i + 4];
		pVertices[i].n.z = pVB[8 * i + 5];
	*/
		pVB_[8 * prID + 3] = normal.x;
		pVB_[8 * prID + 4] = normal.y;
		pVB_[8 * prID + 5] = normal.z;
	}
#endif	// common
}
#endif	// __inside
#if 1	// __outside
{
	const VERTEX_HASH&	pointReprHash = __outside;
	const std::vector<DWORD>&	faceIDArray = __outsideFaceIDArray;
#if 1	// common
	// 각 Vertex 의 노말을 다시 계산;
	// Vertex 를 공유하는 Face 들의 노말의 평균이 해당 Vertex 의 노말
	// 1. 각 Face 의 노말을 저장할 임시 공간
	for (int i = 0; i < (int) faceIDArray.size(); ++i) {
		DWORD	faceID = faceIDArray.at(i);
		CFace&	face = faceArray[faceID];
		D3DXVECTOR3&	p0 = pV[face.prID0].p;
		D3DXVECTOR3&	p1 = pV[face.prID1].p;
		D3DXVECTOR3&	p2 = pV[face.prID2].p;
		D3DXVECTOR3	v01 = p1 - p0;
//		D3DXVec3Normalize(&v01, &v01);
		D3DXVECTOR3	v02 = p2 - p0;
//		D3DXVec3Normalize(&v02, &v02);
		D3DXVECTOR3&	normal = face.normal;
		D3DXVec3Cross(&normal, &v01, &v02);
		D3DXVec3Normalize(&normal, &normal);
	}
	// 2. 각 Vertex 의 노말을 공유하는 Face 들의 노말로부터 계산
	for (VERTEX_HASH::const_iterator cit = pointReprHash.begin(); cit != pointReprHash.end(); ++cit) {
		DWORD	prID = (*cit).first;
		if (excludeSetPtr && excludeSetPtr->find(prID) != excludeSetPtr->end())
				continue;

		D3DXVECTOR3&	normal = pV[prID].n;
		unsigned	numFaces = 0;
#ifdef __USE_A_OFFSET__
		numFaces = pointer_to__a_vFaceIDLocalIndex_a_offset[prID + 1] - pointer_to__a_vFaceIDLocalIndex_a_offset[prID];
#else
		numFaces = pointReprArray[prID].vFaceIdLocalIndex.size();
#endif
#ifdef __USE_A_OFFSET__
		for (int k = pointer_to__a_vFaceIDLocalIndex_a_offset[prID]; k < pointer_to__a_vFaceIDLocalIndex_a_offset[prID + 1]; ++k) {
			const FACE_ID_LOCAL_INDEX&	faceIDLocalIndex = pointer_to__a_vFaceIDLocalIndex[k];
#else
		for (size_t k = 0; k < pointReprArray[prID].vFaceIdLocalIndex.size(); ++k) {
			const FACE_ID_LOCAL_INDEX&	faceIDLocalIndex = pointReprArray[prID].vFaceIdLocalIndex.at(k);
#endif
#if 1	// Face 노말
			normal += faceArray[faceIDLocalIndex.ID].normal;
#else
			int	index = faceIndex[faceIDLocalIndex.ID];
			if (index == (DWORD) -1) {
				cerr << "face ID = " << faceIDLocalIndex.ID << endl;
				continue;
			}
//			assert(index != (DWORD) -1);
			normal += faceNormalArray[index];
#endif
#ifdef __USE_A_OFFSET__
		}
#else
		}
#endif
		normal /= (float) numFaces;
		D3DXVec3Normalize(&normal, &normal);
		// __meshPtr 에 사용되는 노말도 업데이트
	/*
		pVertices[i].n.x = pVB[8 * i + 3];
		pVertices[i].n.y = pVB[8 * i + 4];
		pVertices[i].n.z = pVB[8 * i + 5];
	*/
		pVB_[8 * prID + 3] = normal.x;
		pVB_[8 * prID + 4] = normal.y;
		pVB_[8 * prID + 5] = normal.z;
	}
#endif	// common
}
#endif	// __outside
#endif	// Vertex 의 새로운 노말 결정

#if 1	// __inside/__outside 에 저장된 Vertex 위치 갱신
#if 1	// __inside
{
	VERTEX_HASH&	pointReprHash = __inside;
	for (VERTEX_HASH::iterator it = pointReprHash.begin(); it != pointReprHash.end(); ++it) {
		DWORD	prID = (*it).first;
		if (excludeSetPtr && excludeSetPtr->find(prID) != excludeSetPtr->end())
				continue;

		(*it).second = pointReprArray[prID].v.position;
	}
}
#endif	// __inside
#if 1	// __outside
{
	VERTEX_HASH&	pointReprHash = __outside;
	for (VERTEX_HASH::iterator it = pointReprHash.begin(); it != pointReprHash.end(); ++it) {
		DWORD	prID = (*it).first;
		if (excludeSetPtr && excludeSetPtr->find(prID) != excludeSetPtr->end())
				continue;

		(*it).second = pointReprArray[prID].v.position;
	}
}
#endif	// __outside
#endif


#if 1	// Unlock...
	mesh.meshLoader().meshSubsetPtr()->UnlockVertexBuffer();
#if 1	// unlock
	hResult = mesh.GetMeshPointer()->UnlockVertexBuffer();
	assert(D3D_OK == hResult);
	mesh.GetVertexBufferPointer()->Unlock();
	pVB_n->Unlock();
#endif	// unlock
#endif	// Unlock...
}

#if 1	// kDisplacementMap
// 노말방향으로...
void	kBone::apply2(
	kMesh&	mesh,
	const D3DXVECTOR3&	P0,
	float	fScale,
	const std::set<DWORD>*	excludeSetPtr
)
{
	if (!&mesh) {
		cerr << "FATAL: kBone::set: mesh is nil" << endl;
		return;
	}

	HRESULT	hResult;
	CPointRepr*	pointReprArray = mesh.GetPointReprPointer();
	CFace*		faceArray = mesh.GetFacePointer();
	CEdge*		edgeArray = mesh.GetEdgePointer();
#ifdef __USE_A_OFFSET__
	const DWORD*	pointer_to__a_vVertexID				= mesh.pointer_to__a_vVertexID;
	const int*	pointer_to__a_vVertexID_a_offset		= mesh.pointer_to__a_vVertexID_a_offset;
	const EDGE*	pointer_to__a_vEdgeID				= mesh.pointer_to__a_vEdgeID;
	const int*	pointer_to__a_vEdgeID_a_offset			= mesh.pointer_to__a_vEdgeID_a_offset;
	const FACE_ID_LOCAL_INDEX*	pointer_to__a_vFaceIDLocalIndex	= mesh.pointer_to__a_vFaceIDLocalIndex;
	const int*	pointer_to__a_vFaceIDLocalIndex_a_offset	= mesh.pointer_to__a_vFaceIDLocalIndex_a_offset;
//	const DWORD*	pointer_to__a_faceID				= __meshPtr->pointer_to__a_faceID;
//	const int*	pointer_to__a_faceID_a_offset			= __meshPtr->pointer_to__a_faceID_a_offset;
#endif	// __USE_A_OFFSET__

#if 1	// Lock...
	CPointRepr::VERTEX*	vertexArray;	// 텍스쳐 메쉬 정보를 담은 Vertex Buffer
	mesh.meshLoader().meshSubsetPtr()->LockVertexBuffer(0, (void**) &vertexArray);
#if 1	// lock
	LPDIRECT3DVERTEXBUFFER9	pVB_n = mesh.showNormal().GetVertexBufferPointer();
	kShowNormal::Vertex*	pV_n;		// 노말 정보를 담은 Vertex Buffer
	pVB_n->Lock(0, 0, (void**) &pV_n, 0);

	kMesh::D3DXVERTEX*	pV;	// kMesh::__vertexBufferPtr
	mesh.GetVertexBufferPointer()->Lock(0, 0, (void**) &pV, 0);

	FLOAT*	pVB_ = NULL;
	hResult = mesh.GetMeshPointer()->LockVertexBuffer(0, (void**) &pVB_);
	assert(D3D_OK == hResult);
#endif	// lock
#endif	// Lock...


#if 1	// Vertex 위치 변경
#if 1	// __inside
{
	const VERTEX_HASH&	pointReprHash = __inside;
#if 1	// common
	for (VERTEX_HASH::const_iterator cit = pointReprHash.begin(); cit != pointReprHash.end(); ++cit) {
		DWORD	prID = (*cit).first;
		if (excludeSetPtr && excludeSetPtr->find(prID) != excludeSetPtr->end())
				continue;

		const D3DXVECTOR3&	p = (*cit).second;
		pV[prID].p = p;
#if 0
		pV_n[2 * prID + 0].Position = p;
		pV_n[2 * prID + 1].Position = p;
		pVB_[8 * prID + 0] = p.x;
		pVB_[8 * prID + 1] = p.y;
		pVB_[8 * prID + 2] = p.z;
		pointReprArray[prID].v.position = p;
#ifdef __USE_A_OFFSET__
		for (int k = mesh.pointer_to__a_vVertexID_a_offset[prID]; k < mesh.pointer_to__a_vVertexID_a_offset[prID + 1]; ++k)
			vertexArray[mesh.pointer_to__a_vVertexID[k]].position = p;
#else
		const std::set<DWORD>&	vVertexID = pointRepr.vVertexID;
		for (std::set<DWORD>::const_iterator it = vVertexID.begin(); it != vVertexID.end(); ++it)
			vertexArray[*it].position = p;
#endif
#endif
	}
#endif	// common
}
#endif	// __inside
#if 1	// __outside
{
	const VERTEX_HASH&	pointReprHash = __outside;
#if 1	// common
	for (VERTEX_HASH::const_iterator cit = pointReprHash.begin(); cit != pointReprHash.end(); ++cit) {
		DWORD	prID = (*cit).first;
		if (excludeSetPtr && excludeSetPtr->find(prID) != excludeSetPtr->end())
				continue;

		const D3DXVECTOR3&	p = (*cit).second;
		pV[prID].p = p;
#if 0
		pV_n[2 * prID + 0].Position = p;
		pV_n[2 * prID + 1].Position = p;
		pVB_[8 * prID + 0] = p.x;
		pVB_[8 * prID + 1] = p.y;
		pVB_[8 * prID + 2] = p.z;
		pointReprArray[prID].v.position = p;
#ifdef __USE_A_OFFSET__
		for (int k = mesh.pointer_to__a_vVertexID_a_offset[prID]; k < mesh.pointer_to__a_vVertexID_a_offset[prID + 1]; ++k)
			vertexArray[mesh.pointer_to__a_vVertexID[k]].position = p;
#else
		const std::set<DWORD>&	vVertexID = pointRepr.vVertexID;
		for (std::set<DWORD>::const_iterator it = vVertexID.begin(); it != vVertexID.end(); ++it)
			vertexArray[*it].position = p;
#endif
#endif
	}
#endif	// common
}
#endif	// __outside
#endif	// Vertex 위치 변경

#if 1	// Vertex 위치 변경; ZBrush Smooth Brush
#if 1	// __inside
cout << "W/ Smooth? " << (mesh.statePtr()->brush.flag.Smooth ? "True" : "False") << endl;
if (1 || mesh.statePtr()->brush.flag.Smooth) {	// 무조건 smoothing...
for (int niter = 0; niter < 10; ++niter)
{
	const VERTEX_HASH&	pointReprHash = __inside;
	for (VERTEX_HASH::const_iterator cit = pointReprHash.begin(); cit != pointReprHash.end(); ++cit) {
		DWORD	prID = (*cit).first;
		if (excludeSetPtr && excludeSetPtr->find(prID) != excludeSetPtr->end())
				continue;

		kMesh::D3DXVERTEX&	v = pV[prID];
		v.pTmp = v.p;
	}
	for (VERTEX_HASH::const_iterator cit = pointReprHash.begin(); cit != pointReprHash.end(); ++cit) {
		DWORD	prID = (*cit).first;
		if (excludeSetPtr && excludeSetPtr->find(prID) != excludeSetPtr->end())
				continue;

		kMesh::D3DXVERTEX&	v = pV[prID];
/*
 * 각 Edge 를 구성하는 두 Vertex 의 중점이 각 Vertex 의 새로운 위치에 영향을 미친다;
 * Vertex 의 새로운 위치들의 centroid 와 원래 위치의 중점의 실제로 새로운 위치가 된다.
 */
		// 주어진 Vertex 를 한 끝으로 하는 모든 Edge 에서 Edge 의 중점을 구한다.
		// 이 점들의 centroid 와 원래 점의 중점이 새로운 위치가 된다.
		D3DXVECTOR3	centroid(0, 0, 0);
		unsigned	numPoints = 0;
#ifdef __USE_A_OFFSET__
		numPoints = pointer_to__a_vEdgeID_a_offset[prID + 1] - pointer_to__a_vEdgeID_a_offset[prID];
		for (int k = pointer_to__a_vEdgeID_a_offset[prID]; k < pointer_to__a_vEdgeID_a_offset[prID + 1]; ++k) {
			const EDGE	e = pointer_to__a_vEdgeID[k];
			const CEdge&	edge = edgeArray[e.id];
			centroid += .5f * (pV[edge.prStartID].p + pV[edge.prEndID].p);
		}
#else
		const CPointRepr&	pointRepr = pointReprArray[prID];
		numPoints = pointRepr.vEdgeID.size();
		for (size_t i = 0; i < pointRepr.vEdgeID.size(); ++i) {
			EDGE	e = pointRepr.vEdgeID.at(i);
			const CEdge&	edge = edgeArray[e.id];
			centroid += .5f * (pV[edge.prStartID].p + pV[edge.prEndID].p);
		}
#endif
//		cout << "numPoints = " << numPoints << endl;
		centroid /= (float) numPoints;
		v.pTmp = .5f * (v.p + centroid);
	}
	for (VERTEX_HASH::const_iterator cit = pointReprHash.begin(); cit != pointReprHash.end(); ++cit) {
		DWORD	prID = (*cit).first;
		if (excludeSetPtr && excludeSetPtr->find(prID) != excludeSetPtr->end())
				continue;

		D3DXVECTOR3&	p = pV[prID].p;
		p = pV[prID].pTmp;

#if 0
		pV_n[2 * prID + 0].Position = p;
		pV_n[2 * prID + 1].Position = p;
		pVB_[8 * prID + 0] = p.x;
		pVB_[8 * prID + 1] = p.y;
		pVB_[8 * prID + 2] = p.z;
		pointReprArray[prID].v.position = p;
#if 1	// 텍스쳐 메쉬 변형
#ifdef __USE_A_OFFSET__
		for (int k = pointer_to__a_vVertexID_a_offset[prID]; k < pointer_to__a_vVertexID_a_offset[prID + 1]; ++k)
			vertexArray[pointer_to__a_vVertexID[k]].position = p;
#else
		const std::set<DWORD>&	vVertexID = pointRepr.vVertexID;
		for (std::set<DWORD>::const_iterator it = vVertexID.begin(); it != vVertexID.end(); ++it)
			vertexArray[*it].position = p;
#endif
#endif
#endif	// 텍스쳐 메쉬 변형
	}
}
}
#endif	// __inside
#endif	// Vertex 위치 변경; ZBrush Smooth Brush


#if 1	// Vertex 의 새로운 노말 결정
#if 1	// __inside
{
	const VERTEX_HASH&	pointReprHash = __inside;
	const std::vector<DWORD>&	faceIDArray = __insideFaceIDArray;
#if 1	// common
	// 각 Vertex 의 노말을 다시 계산;
	// Vertex 를 공유하는 Face 들의 노말의 평균이 해당 Vertex 의 노말
	// 1. 각 Face 의 노말을 저장할 임시 공간
	for (int i = 0; i < (int) faceIDArray.size(); ++i) {
		DWORD	faceID = faceIDArray.at(i);
		CFace&	face = faceArray[faceID];
		D3DXVECTOR3&	p0 = pV[face.prID0].p;
		D3DXVECTOR3&	p1 = pV[face.prID1].p;
		D3DXVECTOR3&	p2 = pV[face.prID2].p;
		D3DXVECTOR3	v01 = p1 - p0;
//		D3DXVec3Normalize(&v01, &v01);
		D3DXVECTOR3	v02 = p2 - p0;
//		D3DXVec3Normalize(&v02, &v02);
		D3DXVECTOR3&	normal = face.normal;
		D3DXVec3Cross(&normal, &v01, &v02);
		D3DXVec3Normalize(&normal, &normal);
	}
	// 2. 각 Vertex 의 노말을 공유하는 Face 들의 노말로부터 계산
	for (VERTEX_HASH::const_iterator cit = pointReprHash.begin(); cit != pointReprHash.end(); ++cit) {
		DWORD	prID = (*cit).first;
		if (excludeSetPtr && excludeSetPtr->find(prID) != excludeSetPtr->end())
				continue;

		D3DXVECTOR3&	normal = pV[prID].n;
		unsigned	numFaces = 0;
#ifdef __USE_A_OFFSET__
		numFaces = pointer_to__a_vFaceIDLocalIndex_a_offset[prID + 1] - pointer_to__a_vFaceIDLocalIndex_a_offset[prID];
#else
		numFaces = pointReprArray[prID].vFaceIdLocalIndex.size();
#endif
#ifdef __USE_A_OFFSET__
		for (int k = pointer_to__a_vFaceIDLocalIndex_a_offset[prID]; k < pointer_to__a_vFaceIDLocalIndex_a_offset[prID + 1]; ++k) {
			const FACE_ID_LOCAL_INDEX&	faceIDLocalIndex = pointer_to__a_vFaceIDLocalIndex[k];
#else
		for (size_t k = 0; k < pointReprArray[prID].vFaceIdLocalIndex.size(); ++k) {
			const FACE_ID_LOCAL_INDEX&	faceIDLocalIndex = pointReprArray[prID].vFaceIdLocalIndex.at(k);
#endif
			normal += faceArray[faceIDLocalIndex.ID].normal;
#ifdef __USE_A_OFFSET__
		}
#else
		}
#endif
		normal /= (float) numFaces;
		D3DXVec3Normalize(&normal, &normal);
#if 0
		// __meshPtr 에 사용되는 노말도 업데이트
	/*
		pVertices[i].n.x = pVB[8 * i + 3];
		pVertices[i].n.y = pVB[8 * i + 4];
		pVertices[i].n.z = pVB[8 * i + 5];
	*/
		pVB_[8 * prID + 3] = normal.x;
		pVB_[8 * prID + 4] = normal.y;
		pVB_[8 * prID + 5] = normal.z;
#endif
	}
#endif	// common
}
#endif	// __inside
#if 0	// __outside
{
	const VERTEX_HASH&	pointReprHash = __outside;
	const std::vector<DWORD>&	faceIDArray = __outsideFaceIDArray;
#if 1	// common
	// 각 Vertex 의 노말을 다시 계산;
	// Vertex 를 공유하는 Face 들의 노말의 평균이 해당 Vertex 의 노말
	// 1. 각 Face 의 노말을 저장할 임시 공간
	for (int i = 0; i < (int) faceIDArray.size(); ++i) {
		DWORD	faceID = faceIDArray.at(i);
		CFace&	face = faceArray[faceID];
		D3DXVECTOR3&	p0 = pV[face.prID0].p;
		D3DXVECTOR3&	p1 = pV[face.prID1].p;
		D3DXVECTOR3&	p2 = pV[face.prID2].p;
		D3DXVECTOR3	v01 = p1 - p0;
//		D3DXVec3Normalize(&v01, &v01);
		D3DXVECTOR3	v02 = p2 - p0;
//		D3DXVec3Normalize(&v02, &v02);
		D3DXVECTOR3&	normal = face.normal;
		D3DXVec3Cross(&normal, &v01, &v02);
		D3DXVec3Normalize(&normal, &normal);
	}
	// 2. 각 Vertex 의 노말을 공유하는 Face 들의 노말로부터 계산
	for (VERTEX_HASH::const_iterator cit = pointReprHash.begin(); cit != pointReprHash.end(); ++cit) {
		DWORD	prID = (*cit).first;
		if (excludeSetPtr && excludeSetPtr->find(prID) != excludeSetPtr->end())
				continue;

		D3DXVECTOR3&	normal = pV[prID].n;
		unsigned	numFaces = 0;
#ifdef __USE_A_OFFSET__
		numFaces = pointer_to__a_vFaceIDLocalIndex_a_offset[prID + 1] - pointer_to__a_vFaceIDLocalIndex_a_offset[prID];
#else
		numFaces = __pointReprPtr[prID].vFaceIdLocalIndex.size();
#endif
#ifdef __USE_A_OFFSET__
		for (int k = pointer_to__a_vFaceIDLocalIndex_a_offset[prID]; k < pointer_to__a_vFaceIDLocalIndex_a_offset[prID + 1]; ++k) {
			const FACE_ID_LOCAL_INDEX&	faceIDLocalIndex = pointer_to__a_vFaceIDLocalIndex[k];
#else
		for (size_t k = 0; k < __pointReprPtr[prID].vFaceIdLocalIndex.size(); ++k) {
			const FACE_ID_LOCAL_INDEX&	faceIDLocalIndex = __pointReprPtr[prID].vFaceIdLocalIndex.at(k);
#endif
#if 1	// Face 노말
			normal += faceArray[faceIDLocalIndex.ID].normal;
#else
			int	index = faceIndex[faceIDLocalIndex.ID];
			if (index == (DWORD) -1) {
				cerr << "face ID = " << faceIDLocalIndex.ID << endl;
				continue;
			}
//			assert(index != (DWORD) -1);
			normal += faceNormalArray[index];
#endif
#ifdef __USE_A_OFFSET__
		}
#else
		}
#endif
		normal /= (float) numFaces;
		D3DXVec3Normalize(&normal, &normal);
		// __meshPtr 에 사용되는 노말도 업데이트
	/*
		pVertices[i].n.x = pVB[8 * i + 3];
		pVertices[i].n.y = pVB[8 * i + 4];
		pVertices[i].n.z = pVB[8 * i + 5];
	*/
		pVB_[8 * prID + 3] = normal.x;
		pVB_[8 * prID + 4] = normal.y;
		pVB_[8 * prID + 5] = normal.z;
	}
#endif	// common
}
#endif	// __outside
#endif	// Vertex 의 새로운 노말 결정

#if 1	// kDisplacementMap
#if 1	// __inside
#if 1	// kDisplacementMap
	cout << "**************** kDisplacementMap ****************" << endl;
	cout << "**************************************************" << endl;
//	const WCHAR	bmpFile[] = L"D:/home/icentrik/GDI+/gdiplus-1.0.1/debug/victoria-body.bmp";
	const WCHAR	bmpFile[] = L"D:/home/icentrik/My Documents/Visual Studio Projects/Pick/Media/Scanner/여자모델_Vic090609/tex/victoria-depth-left-thigh.bmp";
	kDisplacementMap	dismap(bmpFile);
	if (dismap.GetLastStatus() != Gdiplus::Ok) {
		cerr << "kDisplacementMap::kDisplacementMap(|" << bmpFile << "|): Failure: " << dismap.GetLastStatus() << endl;
	}
	cout << "***********************************************" << endl;
#endif	// kDisplacementMap
#endif	// __inside
#if 1	// Vertex 위치 변경; 노말방향으로...
{
	VERTEX_HASH&	pointReprHash = __inside;
	float	amplitude = mesh.a() * .5f;
#if 1	// common
	for (VERTEX_HASH::iterator it = pointReprHash.begin(); it != pointReprHash.end(); ++it) {
		DWORD	prID = (*it).first;
		if (excludeSetPtr && excludeSetPtr->find(prID) != excludeSetPtr->end())
				continue;

		const kMesh::D3DXVERTEX&	v = pV[prID];
		D3DXVECTOR3&	p = (*it).second;
#if 1	// smoothstep
		float	f_smooth = 1.f;
{
		D3DXVECTOR3	ell_inside = v.p - P0;
		D3DXVECTOR3	ell_longitudinal = D3DXVec3Dot(&ell_inside, &__nDir) * __nDir;
		// P0~P2 사이를 세 구간으로 나눈다.
		// P0~P3, P3~P4, P4~P2
		// P0~P3, P4~P2 사이는 scale_transversal 값을
		// 1부터 fScale 까지 변화시킨다. (smoothstep 함수를 사용)
		// P3~P4 사이는 scale_transversal 값으로 fScale 값을 사용한다.
		// P0~P3 구간의 크기와 P4~P2 구간의 크기는 서로 다르나
		// 여기서는 같다고 가정하고 그것도 P0~P2 구간의 10%라고 가정한다.
		// (TO DO LIST)
		const	float	left_range = 0.2f;
		assert(left_range <= .5f);
		const	float	right_range = 1.f - left_range;
		float	length = D3DXVec3Length(&ell_longitudinal);
		if (length < __length * left_range) {		// P0~P3 구간
			f_smooth = smoothstep(0.f, 0.f, __length * left_range, 1.f, length);
		} else if (length > __length * right_range) {	// P4~P2 구간
			f_smooth = smoothstep(__length * right_range, 1.f, __length, 0.f, length);
		} else {				// P3~P4 구간
			f_smooth = 1.f;
		}
}
#endif	// smoothstep
#if 1	// GetPixel
		float	val;
{
		Gdiplus::Status	status = dismap.GetPixelAtUV(v.tu, v.tv, val, kDisplacementMap::CHANNEL_RED);
		if (status != Gdiplus::Ok) {
			cerr << "kDisplacementMap::GetPixel(): Failure" << endl;
			continue;
		} else {
			val /= dismap.MaxRed() / (float) 255;	// clamp to [0, 1]
		}
}
#endif	// GetPixel
		float	f = f_smooth * val;
#if 0
		if (fScale > 1.f) {
			// f = 1 + factor = 1 + alpha * factor
			float	factor = fScale - 1.f;
			factor *= val;
			f = 1.f + factor;
		} else {
			// f = 1 - factor = 1 - alpha * factor
			float	factor = 1.f - fScale;
			factor *= val;
			f = 1.f - factor;
		}
#endif
		p = v.p + (f * amplitude) * v.n;
	}
#endif	// common
}
#endif	// Vertex 위치 변경
#endif	// kDisplacementMap
#if 1	// Vertex 위치 변경
#if 1	// __inside
{
	const VERTEX_HASH&	pointReprHash = __inside;
#if 1	// common
	for (VERTEX_HASH::const_iterator cit = pointReprHash.begin(); cit != pointReprHash.end(); ++cit) {
		DWORD	prID = (*cit).first;
		if (excludeSetPtr && excludeSetPtr->find(prID) != excludeSetPtr->end())
				continue;

		const D3DXVECTOR3&	p = (*cit).second;
		pV[prID].p = p;
		pV_n[2 * prID + 0].Position = p;
		pV_n[2 * prID + 1].Position = p;
		pVB_[8 * prID + 0] = p.x;
		pVB_[8 * prID + 1] = p.y;
		pVB_[8 * prID + 2] = p.z;
		pointReprArray[prID].v.position = p;
#ifdef __USE_A_OFFSET__
		for (int k = mesh.pointer_to__a_vVertexID_a_offset[prID]; k < mesh.pointer_to__a_vVertexID_a_offset[prID + 1]; ++k)
			vertexArray[mesh.pointer_to__a_vVertexID[k]].position = p;
#else
		const std::set<DWORD>&	vVertexID = pointReprArray[prID].vVertexID;
		for (std::set<DWORD>::const_iterator it = vVertexID.begin(); it != vVertexID.end(); ++it)
			vertexArray[*it].position = p;
#endif
	}
#endif	// common
}
#endif	// __inside
#if 1	// __outside
{
	const VERTEX_HASH&	pointReprHash = __outside;
#if 1	// common
	for (VERTEX_HASH::const_iterator cit = pointReprHash.begin(); cit != pointReprHash.end(); ++cit) {
		DWORD	prID = (*cit).first;
		if (excludeSetPtr && excludeSetPtr->find(prID) != excludeSetPtr->end())
				continue;

		const D3DXVECTOR3&	p = (*cit).second;
		pV[prID].p = p;
		pV_n[2 * prID + 0].Position = p;
		pV_n[2 * prID + 1].Position = p;
		pVB_[8 * prID + 0] = p.x;
		pVB_[8 * prID + 1] = p.y;
		pVB_[8 * prID + 2] = p.z;
		pointReprArray[prID].v.position = p;
#ifdef __USE_A_OFFSET__
		for (int k = mesh.pointer_to__a_vVertexID_a_offset[prID]; k < mesh.pointer_to__a_vVertexID_a_offset[prID + 1]; ++k)
			vertexArray[mesh.pointer_to__a_vVertexID[k]].position = p;
#else
		const std::set<DWORD>&	vVertexID = pointReprArray[prID].vVertexID;
		for (std::set<DWORD>::const_iterator it = vVertexID.begin(); it != vVertexID.end(); ++it)
			vertexArray[*it].position = p;
#endif
	}
#endif	// common
}
#endif	// __outside
#endif	// Vertex 위치 변경

#if 1	// Vertex 위치 변경; ZBrush Smooth Brush
#if 1	// __inside
cout << "W/ Smooth? " << (mesh.statePtr()->brush.flag.Smooth ? "True" : "False") << endl;
if (mesh.statePtr()->brush.flag.Smooth) {
for (int niter = 0; niter < 1; ++niter)
{
	const VERTEX_HASH&	pointReprHash = __inside;
	for (VERTEX_HASH::const_iterator cit = pointReprHash.begin(); cit != pointReprHash.end(); ++cit) {
		DWORD	prID = (*cit).first;
		if (excludeSetPtr && excludeSetPtr->find(prID) != excludeSetPtr->end())
				continue;

		kMesh::D3DXVERTEX&	v = pV[prID];
		v.pTmp = v.p;
	}
	for (VERTEX_HASH::const_iterator cit = pointReprHash.begin(); cit != pointReprHash.end(); ++cit) {
		DWORD	prID = (*cit).first;
		if (excludeSetPtr && excludeSetPtr->find(prID) != excludeSetPtr->end())
				continue;

		kMesh::D3DXVERTEX&	v = pV[prID];
/*
 * 각 Edge 를 구성하는 두 Vertex 의 중점이 각 Vertex 의 새로운 위치에 영향을 미친다;
 * Vertex 의 새로운 위치들의 centroid 와 원래 위치의 중점의 실제로 새로운 위치가 된다.
 */
		// 주어진 Vertex 를 한 끝으로 하는 모든 Edge 에서 Edge 의 중점을 구한다.
		// 이 점들의 centroid 와 원래 점의 중점이 새로운 위치가 된다.
		D3DXVECTOR3	centroid(0, 0, 0);
		unsigned	numPoints = 0;
#ifdef __USE_A_OFFSET__
		numPoints = pointer_to__a_vEdgeID_a_offset[prID + 1] - pointer_to__a_vEdgeID_a_offset[prID];
		for (int k = pointer_to__a_vEdgeID_a_offset[prID]; k < pointer_to__a_vEdgeID_a_offset[prID + 1]; ++k) {
			const EDGE	e = pointer_to__a_vEdgeID[k];
			const CEdge&	edge = edgeArray[e.id];
			centroid += .5f * (pV[edge.prStartID].p + pV[edge.prEndID].p);
		}
#else
		const CPointRepr&	pointRepr = pointReprArray[prID];
		numPoints = pointRepr.vEdgeID.size();
		for (size_t i = 0; i < pointRepr.vEdgeID.size(); ++i) {
			EDGE	e = pointRepr.vEdgeID.at(i);
			const CEdge&	edge = edgeArray[e.id];
			centroid += .5f * (pV[edge.prStartID].p + pV[edge.prEndID].p);
		}
#endif
//		cout << "numPoints = " << numPoints << endl;
		centroid /= (float) numPoints;
		v.pTmp = .5f * (v.p + centroid);
	}
	for (VERTEX_HASH::const_iterator cit = pointReprHash.begin(); cit != pointReprHash.end(); ++cit) {
		DWORD	prID = (*cit).first;
		if (excludeSetPtr && excludeSetPtr->find(prID) != excludeSetPtr->end())
				continue;

		D3DXVECTOR3&	p = pV[prID].p;
		p = pV[prID].pTmp;

		pV_n[2 * prID + 0].Position = p;
		pV_n[2 * prID + 1].Position = p;
		pVB_[8 * prID + 0] = p.x;
		pVB_[8 * prID + 1] = p.y;
		pVB_[8 * prID + 2] = p.z;
		pointReprArray[prID].v.position = p;
#if 1	// 텍스쳐 메쉬 변형
#ifdef __USE_A_OFFSET__
		for (int k = pointer_to__a_vVertexID_a_offset[prID]; k < pointer_to__a_vVertexID_a_offset[prID + 1]; ++k)
			vertexArray[pointer_to__a_vVertexID[k]].position = p;
#else
		const std::set<DWORD>&	vVertexID = pointReprArray[prID].vVertexID;
		for (std::set<DWORD>::const_iterator it = vVertexID.begin(); it != vVertexID.end(); ++it)
			vertexArray[*it].position = p;
#endif
#endif	// 텍스쳐 메쉬 변형
	}
}
}
#endif	// __inside
#endif	// Vertex 위치 변경; ZBrush Smooth Brush


#if 1	// Vertex 의 새로운 노말 결정
#if 1	// __inside
{
	const VERTEX_HASH&	pointReprHash = __inside;
	const std::vector<DWORD>&	faceIDArray = __insideFaceIDArray;
#if 1	// common
	// 각 Vertex 의 노말을 다시 계산;
	// Vertex 를 공유하는 Face 들의 노말의 평균이 해당 Vertex 의 노말
	// 1. 각 Face 의 노말을 저장할 임시 공간
	for (int i = 0; i < (int) faceIDArray.size(); ++i) {
		DWORD	faceID = faceIDArray.at(i);
		CFace&	face = faceArray[faceID];
		D3DXVECTOR3&	p0 = pV[face.prID0].p;
		D3DXVECTOR3&	p1 = pV[face.prID1].p;
		D3DXVECTOR3&	p2 = pV[face.prID2].p;
		D3DXVECTOR3	v01 = p1 - p0;
//		D3DXVec3Normalize(&v01, &v01);
		D3DXVECTOR3	v02 = p2 - p0;
//		D3DXVec3Normalize(&v02, &v02);
		D3DXVECTOR3&	normal = face.normal;
		D3DXVec3Cross(&normal, &v01, &v02);
		D3DXVec3Normalize(&normal, &normal);
	}
	// 2. 각 Vertex 의 노말을 공유하는 Face 들의 노말로부터 계산
	for (VERTEX_HASH::const_iterator cit = pointReprHash.begin(); cit != pointReprHash.end(); ++cit) {
		DWORD	prID = (*cit).first;
		if (excludeSetPtr && excludeSetPtr->find(prID) != excludeSetPtr->end())
				continue;

		D3DXVECTOR3&	normal = pV[prID].n;
		unsigned	numFaces = 0;
#ifdef __USE_A_OFFSET__
		numFaces = pointer_to__a_vFaceIDLocalIndex_a_offset[prID + 1] - pointer_to__a_vFaceIDLocalIndex_a_offset[prID];
#else
		numFaces = pointReprArray[prID].vFaceIdLocalIndex.size();
#endif
#ifdef __USE_A_OFFSET__
		for (int k = pointer_to__a_vFaceIDLocalIndex_a_offset[prID]; k < pointer_to__a_vFaceIDLocalIndex_a_offset[prID + 1]; ++k) {
			const FACE_ID_LOCAL_INDEX&	faceIDLocalIndex = pointer_to__a_vFaceIDLocalIndex[k];
#else
		for (size_t k = 0; k < pointReprArray[prID].vFaceIdLocalIndex.size(); ++k) {
			const FACE_ID_LOCAL_INDEX&	faceIDLocalIndex = pointReprArray[prID].vFaceIdLocalIndex.at(k);
#endif
			normal += faceArray[faceIDLocalIndex.ID].normal;
#ifdef __USE_A_OFFSET__
		}
#else
		}
#endif
		normal /= (float) numFaces;
		D3DXVec3Normalize(&normal, &normal);
		// __meshPtr 에 사용되는 노말도 업데이트
	/*
		pVertices[i].n.x = pVB[8 * i + 3];
		pVertices[i].n.y = pVB[8 * i + 4];
		pVertices[i].n.z = pVB[8 * i + 5];
	*/
		pVB_[8 * prID + 3] = normal.x;
		pVB_[8 * prID + 4] = normal.y;
		pVB_[8 * prID + 5] = normal.z;
	}
#endif	// common
}
#endif	// __inside
#if 1	// __outside
{
	const VERTEX_HASH&	pointReprHash = __outside;
	const std::vector<DWORD>&	faceIDArray = __outsideFaceIDArray;
#if 1	// common
	// 각 Vertex 의 노말을 다시 계산;
	// Vertex 를 공유하는 Face 들의 노말의 평균이 해당 Vertex 의 노말
	// 1. 각 Face 의 노말을 저장할 임시 공간
	for (int i = 0; i < (int) faceIDArray.size(); ++i) {
		DWORD	faceID = faceIDArray.at(i);
		CFace&	face = faceArray[faceID];
		D3DXVECTOR3&	p0 = pV[face.prID0].p;
		D3DXVECTOR3&	p1 = pV[face.prID1].p;
		D3DXVECTOR3&	p2 = pV[face.prID2].p;
		D3DXVECTOR3	v01 = p1 - p0;
//		D3DXVec3Normalize(&v01, &v01);
		D3DXVECTOR3	v02 = p2 - p0;
//		D3DXVec3Normalize(&v02, &v02);
		D3DXVECTOR3&	normal = face.normal;
		D3DXVec3Cross(&normal, &v01, &v02);
		D3DXVec3Normalize(&normal, &normal);
	}
	// 2. 각 Vertex 의 노말을 공유하는 Face 들의 노말로부터 계산
	for (VERTEX_HASH::const_iterator cit = pointReprHash.begin(); cit != pointReprHash.end(); ++cit) {
		DWORD	prID = (*cit).first;
		if (excludeSetPtr && excludeSetPtr->find(prID) != excludeSetPtr->end())
				continue;

		D3DXVECTOR3&	normal = pV[prID].n;
		unsigned	numFaces = 0;
#ifdef __USE_A_OFFSET__
		numFaces = pointer_to__a_vFaceIDLocalIndex_a_offset[prID + 1] - pointer_to__a_vFaceIDLocalIndex_a_offset[prID];
#else
		numFaces = pointReprArray[prID].vFaceIdLocalIndex.size();
#endif
#ifdef __USE_A_OFFSET__
		for (int k = pointer_to__a_vFaceIDLocalIndex_a_offset[prID]; k < pointer_to__a_vFaceIDLocalIndex_a_offset[prID + 1]; ++k) {
			const FACE_ID_LOCAL_INDEX&	faceIDLocalIndex = pointer_to__a_vFaceIDLocalIndex[k];
#else
		for (size_t k = 0; k < pointReprArray[prID].vFaceIdLocalIndex.size(); ++k) {
			const FACE_ID_LOCAL_INDEX&	faceIDLocalIndex = pointReprArray[prID].vFaceIdLocalIndex.at(k);
#endif
#if 1	// Face 노말
			normal += faceArray[faceIDLocalIndex.ID].normal;
#else
			int	index = faceIndex[faceIDLocalIndex.ID];
			if (index == (DWORD) -1) {
				cerr << "face ID = " << faceIDLocalIndex.ID << endl;
				continue;
			}
//			assert(index != (DWORD) -1);
			normal += faceNormalArray[index];
#endif
#ifdef __USE_A_OFFSET__
		}
#else
		}
#endif
		normal /= (float) numFaces;
		D3DXVec3Normalize(&normal, &normal);
		// __meshPtr 에 사용되는 노말도 업데이트
	/*
		pVertices[i].n.x = pVB[8 * i + 3];
		pVertices[i].n.y = pVB[8 * i + 4];
		pVertices[i].n.z = pVB[8 * i + 5];
	*/
		pVB_[8 * prID + 3] = normal.x;
		pVB_[8 * prID + 4] = normal.y;
		pVB_[8 * prID + 5] = normal.z;
	}
#endif	// common
}
#endif	// __outside
#endif	// Vertex 의 새로운 노말 결정

#if 1	// __inside/__outside 에 저장된 Vertex 위치 갱신
#if 1	// __inside
{
	VERTEX_HASH&	pointReprHash = __inside;
	for (VERTEX_HASH::iterator it = pointReprHash.begin(); it != pointReprHash.end(); ++it) {
		DWORD	prID = (*it).first;
		if (excludeSetPtr && excludeSetPtr->find(prID) != excludeSetPtr->end())
				continue;

		(*it).second = pointReprArray[prID].v.position;
	}
}
#endif	// __inside
#if 1	// __outside
{
	VERTEX_HASH&	pointReprHash = __outside;
	for (VERTEX_HASH::iterator it = pointReprHash.begin(); it != pointReprHash.end(); ++it) {
		DWORD	prID = (*it).first;
		if (excludeSetPtr && excludeSetPtr->find(prID) != excludeSetPtr->end())
				continue;

		(*it).second = pointReprArray[prID].v.position;
	}
}
#endif	// __outside
#endif


#if 1	// Unlock...
	mesh.meshLoader().meshSubsetPtr()->UnlockVertexBuffer();
#if 1	// unlock
	hResult = mesh.GetMeshPointer()->UnlockVertexBuffer();
	assert(D3D_OK == hResult);
	mesh.GetVertexBufferPointer()->Unlock();
	pVB_n->Unlock();
#endif	// unlock
#endif	// Unlock...
}
#if 0	// moved to kMesh::simulateWholeBody
void	kBone::apply_all(
	kMesh&	mesh,
	float	fScale
)
{
#ifndef __KEKO__
#define AREA2_HINT
//#ifdef AREA2_HINT
	bool	sub = mesh.statePtr()->brush.flag.Sub || mesh.statePtr()->g__Shift;
//#endif	// AREA2_HINT
#endif	// !__KEKO__
#ifdef AREA2_HINT
	kState&	state = *mesh.statePtr();
#endif	// AREA2_HINT


#ifndef STANDARD_MODEL
	cerr << "kBone::apply_all: No standard model" << endl;
	return;
#endif	// !STANDARD_MODEL
#ifndef __KEKO__
	int	index_to_standard = mesh.index_to_standard();
	if (
		index_to_standard != 1 /* ETRI 초기 표준모델 */ &&
		index_to_standard != 6 /* ETRI 091211 표준모델(남: Michael) */ &&
		index_to_standard != 7 /* ETRI 091211 표준모델(여: Victoria) */
	) {
		cerr << "kBone::apply_all: index_to_standard = 1, 6, 7 only" << endl;
		return;
	}
#endif	// !__KEKO__
	if (!&mesh) {
		cerr << "FATAL: kBone::set: mesh is nil" << endl;
		return;
	}
	int	subset_index;	// 여기서 사용되는 몸의 subset 의 인덱스;
				// 전체(얼굴, 눈, 눈썹, 몸)이 있을 경우 3(ID = 0, 1),
				// 몸만 있는 경우에는 0 (ID = 3, 4, 5, 6, 7)
#ifdef __KEKO__
	subset_index = 0;
#else	// __KEKO__/!__KEKO__
	subset_index = index_to_standard == 1 /* ETRI 초기 표준모델 */ ? 3 : 0;
#endif	// !__KEKO__

	HRESULT	hResult;
	CPointRepr*	pointReprArray = mesh.GetPointReprPointer();
	CFace*		faceArray = mesh.GetFacePointer();
#ifndef __KEKO__
	CEdge*		edgeArray = mesh.GetEdgePointer();
#endif	// !__KEKO__
#ifdef __USE_A_OFFSET__
	const DWORD*	pointer_to__a_vVertexID				= mesh.pointer_to__a_vVertexID;
	const int*	pointer_to__a_vVertexID_a_offset		= mesh.pointer_to__a_vVertexID_a_offset;
	const EDGE*	pointer_to__a_vEdgeID				= mesh.pointer_to__a_vEdgeID;
	const int*	pointer_to__a_vEdgeID_a_offset			= mesh.pointer_to__a_vEdgeID_a_offset;
	const FACE_ID_LOCAL_INDEX*	pointer_to__a_vFaceIDLocalIndex	= mesh.pointer_to__a_vFaceIDLocalIndex;
	const int*	pointer_to__a_vFaceIDLocalIndex_a_offset	= mesh.pointer_to__a_vFaceIDLocalIndex_a_offset;
//	const DWORD*	pointer_to__a_faceID				= __meshPtr->pointer_to__a_faceID;
//	const int*	pointer_to__a_faceID_a_offset			= __meshPtr->pointer_to__a_faceID_a_offset;
#endif	// __USE_A_OFFSET__

#if 1	// Lock...
	CPointRepr::VERTEX*	vertexArray;	// 텍스쳐 메쉬 정보를 담은 Vertex Buffer
	mesh.meshLoader().meshSubsetPtr()->LockVertexBuffer(0, (void**) &vertexArray);
	DWORD*	subsetArray;
	mesh.meshLoader().meshPtr()->LockAttributeBuffer(0, &subsetArray);
#if 1	// lock
	LPDIRECT3DVERTEXBUFFER9	pVB_n = mesh.showNormal().GetVertexBufferPointer();
	kShowNormal::Vertex*	pV_n;		// 노말 정보를 담은 Vertex Buffer
	pVB_n->Lock(0, 0, (void**) &pV_n, 0);

	kMesh::D3DXVERTEX*	pV;	// kMesh::__vertexBufferPtr
	mesh.GetVertexBufferPointer()->Lock(0, 0, (void**) &pV, 0);

	FLOAT*	pVB_ = NULL;
	hResult = mesh.GetMeshPointer()->LockVertexBuffer(0, (void**) &pVB_);
	assert(D3D_OK == hResult);
#endif	// lock
#endif	// Lock...
	DWORD*	faceIDArray = 0;
#if 1	// FLAG
	CPointRepr::Area2Flag*	pointReprFlagArray = 0;
#else	// FLAG/!FLAG
	DWORD*	pointReprIDArray = 0;
#endif	// !FLAG
	faceIDArray = new DWORD[mesh.GetNumFaces()];
	FillMemory(faceIDArray, mesh.GetNumFaces() * sizeof(DWORD), 0);
#if 1	// FLAG
	pointReprFlagArray = new CPointRepr::Area2Flag[mesh.GetNumPointReprs()];
	FillMemory(pointReprFlagArray, mesh.GetNumPointReprs() * sizeof(CPointRepr::Area2Flag), 0);
#else	// FLAG/!FLAG
	pointReprIDArray = new DWORD[mesh.GetNumPointReprs()];
	FillMemory(pointReprIDArray, mesh.GetNumPointReprs() * sizeof(DWORD), 0);
#endif	// !FLAG
	for (DWORD faceID = 0; faceID < (DWORD) mesh.GetNumFaces(); ++faceID) {
		const CFace&	face = faceArray[faceID];
		if (subsetArray[faceID] != (DWORD) subset_index)	// 0: victoria-face
						// 1: victoria-eyebrow
						// 2: victoria-eye
						// 3: victoria-body
			continue;

		faceIDArray[faceID] = 1;
		for (int i = 0; i < 3; ++i)
#if 1	// FLAG
			pointReprFlagArray[face.prID[i]].subset = true;
#else	// FLAG/!FLAG
			pointReprIDArray[face.prID[i]] = 1;
#endif	// !FLAG
	}


#ifdef AREA2_HINT
	struct Area2
	{
		float	before;
		float	after;
		bool	increase;	// 면적이 증가했는가?

		Area2() : increase(false) {}
	};
	typedef std::map<DWORD, Area2>	AREA2_HASH;
	AREA2_HASH	area2_dic;
#if 1	// FLAG
	VERTEX_HASH	vertex_dic;	// pointReprFlagArray[prID].move 값이 True 인 Vertex 의 위치를 기억한다.
#else	// FLAG/!FLAG
	VERTEX_HASH	vertex_dic;	// pointReprArray[prID] = 2 인 Vertex 의 위치를 기억한다.
#endif	// !FLAG
#endif	// AREA2_HINT
#ifdef AREA2_HINT
if (sub) {
	cout << "**** before ****" << endl;
	for (DWORD faceID = 0; faceID < (DWORD) mesh.GetNumFaces(); ++faceID) {
		if (faceIDArray[faceID] == 0)
			continue;

		CFace&	face = faceArray[faceID];
		D3DXVECTOR3&	p0 = pV[face.prID0].p;
		D3DXVECTOR3&	p1 = pV[face.prID1].p;
		D3DXVECTOR3&	p2 = pV[face.prID2].p;
		D3DXVECTOR3	v01 = p1 - p0;
		D3DXVECTOR3	v02 = p2 - p0;
		D3DXVECTOR3	Va;
		D3DXVec3Cross(&Va, &v01, &v02);
//		cout << "Area[" << face.id << "] = " << fabsf(D3DXVec3Length(&Va)) << endl;
		Area2&	area2 = area2_dic[faceID];
		area2.before = fabsf(D3DXVec3Length(&Va));
	}
}
#endif	// AREA2_HINT


{
#if 1	// Vertex 위치 변경; 노말방향으로...
#if 1	// kDisplacementMap
	cout << "**************** kDisplacementMap ****************" << endl;
	cout << "**************************************************" << endl;
//	const WCHAR	bmpFile[] = L"D:/home/icentrik/GDI+/gdiplus-1.0.1/debug/victoria-body.bmp";
#ifdef __KEKO__
	const WCHAR	bmpFile[] = L"./Resources/keko-dismap.bmp";
#if 1	// 텍스쳐도 변경
	kMeshLoader::Material&	material = *mesh.meshLoader().GetMaterials().GetAt(subset_index);
	SAFE_RELEASE(material.texturePtr);
	D3DXCreateTextureFromFile(DXUTGetD3D9Device(), bmpFile, &material.texturePtr);
#endif
#else	// __KEKO__/!__KEKO__
//	const WCHAR	bmpFile[] = L"D:/home/icentrik/My Documents/Visual Studio Projects/Pick/Media/Scanner/여자모델_Vic090609/tex/victoria-whole-body-displacement.bmp";
	WCHAR		bmpFile[MAX_PATH];
	swprintf_s(bmpFile, MAX_PATH, L"Media/Scanner/에트리표준모델-091211/%s/tex/%s-whole-body-displacement.bmp", index_to_standard == 6 ? L"michael" : L"victoria", index_to_standard == 6 ? L"michael" : L"victoria");
#if 0	// 텍스쳐도 변경
	kMeshLoader::Material&	material = *mesh.meshLoader().GetMaterials().GetAt(subset_index);
	SAFE_RELEASE(material.texturePtr);
	D3DXCreateTextureFromFile(DXUTGetD3D9Device(), bmpFile, &material.texturePtr);
#endif
#endif	// !__KEKO__
	kDisplacementMap	dismap(bmpFile);
	if (dismap.GetLastStatus() != Gdiplus::Ok) {
		cerr << "kDisplacementMap::kDisplacementMap(|" << bmpFile << "|): Failure: " << dismap.GetLastStatus() << endl;
	}
	cout << "***********************************************" << endl;
{
	float	amplitude = fScale * mesh.a();
	for (DWORD prID = 0; prID < (DWORD) mesh.GetNumPointReprs(); ++prID) {
		if (pointReprArray[prID].flag.fixed)
			continue;
#if 1	// FLAG
		if (pointReprFlagArray[prID].subset == false)
			continue;
#else	// FLAG/!FLAG
		if (pointReprIDArray[prID] == 0)
			continue;
#endif	// !FLAG

		kMesh::D3DXVERTEX&	v = pV[prID];

		D3DXVECTOR3&	p = v.p;
#if 1
{
#if 1	// smoothstep
		float	f_smooth = 1.f;
#if 0
{
		D3DXVECTOR3	ell_inside = v.p - P0;
		D3DXVECTOR3	ell_longitudinal = D3DXVec3Dot(&ell_inside, &__nDir) * __nDir;
		// P0~P2 사이를 세 구간으로 나눈다.
		// P0~P3, P3~P4, P4~P2
		// P0~P3, P4~P2 사이는 scale_transversal 값을
		// 1부터 fScale 까지 변화시킨다. (smoothstep 함수를 사용)
		// P3~P4 사이는 scale_transversal 값으로 fScale 값을 사용한다.
		// P0~P3 구간의 크기와 P4~P2 구간의 크기는 서로 다르나
		// 여기서는 같다고 가정하고 그것도 P0~P2 구간의 10%라고 가정한다.
		// (TO DO LIST)
		const	float	left_range = 0.2f;
		assert(left_range <= .5f);
		const	float	right_range = 1.f - left_range;
		float	length = D3DXVec3Length(&ell_longitudinal);
		if (length < __length * left_range) {		// P0~P3 구간
			f_smooth = smoothstep(0.f, 0.f, __length * left_range, 1.f, length);
		} else if (length > __length * right_range) {	// P4~P2 구간
			f_smooth = smoothstep(__length * right_range, 1.f, __length, 0.f, length);
		} else {				// P3~P4 구간
			f_smooth = 1.f;
		}
}
#endif
#endif	// smoothstep
#if 1	// GetPixel
		float	val;
{
		Gdiplus::Status	status = dismap.GetPixelAtUV(v.tu, v.tv, val, kDisplacementMap::CHANNEL_RED);
		if (status != Gdiplus::Ok) {
			cerr << "kDisplacementMap::GetPixel(): Failure" << endl;
			continue;
		} else {
			val /= dismap.MaxRed() / (float) 255;	// clamp to [0, 1]
		}
}
#endif	// GetPixel
		float	f = f_smooth * val;
#if 0
		if (fScale > 1.f) {
			// f = 1 + factor = 1 + alpha * factor
			float	factor = fScale - 1.f;
			factor *= val;
			f = 1.f + factor;
		} else {
			// f = 1 - factor = 1 - alpha * factor
			float	factor = 1.f - fScale;
			factor *= val;
			f = 1.f - factor;
		}
#endif
		if (f * amplitude < FLT_EPSILON) {
//			cout << "prID = " << prID << ": No move" << endl;
			continue;
		}
#if 1	// FLAG
		assert(pointReprFlagArray[prID].subset == true);
		pointReprFlagArray[prID].move = true;
#else	// FLAG/!FLAG
		assert(pointReprIDArray[prID] == 1);
		pointReprIDArray[prID] = 2;
#endif	// !FLAG
#ifdef AREA2_HINT
		if (sub)
			vertex_dic[prID] = p;
#endif	// AREA2_HINT
		if (mesh.statePtr()->brush.flag.Sub || mesh.statePtr()->g__Shift)
			p -= (f * amplitude) * v.n;
		else
			p += (f * amplitude) * v.n;
}
#endif	// 변형

		pV_n[2 * prID + 0].Position = p;
		pV_n[2 * prID + 1].Position = p;
		pVB_[8 * prID + 0] = p.x;
		pVB_[8 * prID + 1] = p.y;
		pVB_[8 * prID + 2] = p.z;
		pointReprArray[prID].v.position = p;
#if 1	// 텍스쳐 메쉬 변형
#ifdef __USE_A_OFFSET__
		for (int k = mesh.pointer_to__a_vVertexID_a_offset[prID]; k < mesh.pointer_to__a_vVertexID_a_offset[prID + 1]; ++k)
			vertexArray[mesh.pointer_to__a_vVertexID[k]].position = p;
#else
		const std::set<DWORD>&	vVertexID = pointReprArray[prID].vVertexID;
		for (std::set<DWORD>::const_iterator it = vVertexID.begin(); it != vVertexID.end(); ++it)
			vertexArray[*it].position = p;
#endif
#endif	// 텍스쳐 메쉬 변형
	}
}
#endif	// kDisplacementMap
#endif	// Vertex 위치 변경

#ifndef __KEKO__
#if 1	// Vertex 위치 변경; ZBrush Smooth Brush
if (!sub && state.area2_control.Smooth == 2) {
for (int niter = 0; niter < (1 + (int) state.area2_control.SmoothIncLevel) * (int) mesh.statePtr()->brush.flag.SmoothLevel; ++niter)
{
{
	unsigned	NumSmooth = 0;
	for (DWORD prID = 0; prID < (DWORD) mesh.GetNumPointReprs(); ++prID) {
#if 1	// FLAG
		if (pointReprFlagArray[prID].move == false)
			continue;
#else	// FLAG/!FLAG
		if (pointReprIDArray[prID] < 2)
			continue;
#endif	// !FLAG

		++NumSmooth;
		kMesh::D3DXVERTEX&	v = pV[prID];
		v.pTmp = v.p;
	}
	cout << "# of smoothed vertices = " << NumSmooth << endl;
	for (DWORD prID = 0; prID < (DWORD) mesh.GetNumPointReprs(); ++prID) {
#if 1	// FLAG
		if (pointReprFlagArray[prID].move == false)
			continue;
#else	// FLAG/!FLAG
		if (pointReprIDArray[prID] < 2)
			continue;
#endif	// !FLAG

		kMesh::D3DXVERTEX&	v = pV[prID];
/*
 * 각 Edge 를 구성하는 두 Vertex 의 중점이 각 Vertex 의 새로운 위치에 영향을 미친다;
 * Vertex 의 새로운 위치들의 centroid 와 원래 위치의 중점의 실제로 새로운 위치가 된다.
 */
		// 주어진 Vertex 를 한 끝으로 하는 모든 Edge 에서 Edge 의 중점을 구한다.
		// 이 점들의 centroid 와 원래 점의 중점이 새로운 위치가 된다.
		D3DXVECTOR3	centroid(0, 0, 0);
		unsigned	numPoints = 0;
#ifdef __USE_A_OFFSET__
		numPoints = pointer_to__a_vEdgeID_a_offset[prID + 1] - pointer_to__a_vEdgeID_a_offset[prID];
		for (int k = pointer_to__a_vEdgeID_a_offset[prID]; k < pointer_to__a_vEdgeID_a_offset[prID + 1]; ++k) {
			const EDGE	e = pointer_to__a_vEdgeID[k];
			const CEdge&	edge = edgeArray[e.id];
			centroid += .5f * (pV[edge.prStartID].p + pV[edge.prEndID].p);
		}
#else
		const CPointRepr&	pointRepr = pointReprArray[prID];
		numPoints = pointRepr.vEdgeID.size();
		for (size_t i = 0; i < pointRepr.vEdgeID.size(); ++i) {
			EDGE	e = pointRepr.vEdgeID.at(i);
			const CEdge&	edge = edgeArray[e.id];
			centroid += .5f * (pV[edge.prStartID].p + pV[edge.prEndID].p);
		}
#endif
//		cout << "numPoints = " << numPoints << endl;
		centroid /= (float) numPoints;
		v.pTmp = .5f * (v.p + centroid);
	}
	for (DWORD prID = 0; prID < (DWORD) mesh.GetNumPointReprs(); ++prID) {
#if 1	// FLAG
		if (pointReprFlagArray[prID].move == false)
			continue;
#else	// FLAG/!FLAG
		if (pointReprIDArray[prID] < 2)
			continue;
#endif	// !FLAG

		D3DXVECTOR3&	p = pV[prID].p;
		p = pV[prID].pTmp;

		pV_n[2 * prID + 0].Position = p;
		pV_n[2 * prID + 1].Position = p;
		pVB_[8 * prID + 0] = p.x;
		pVB_[8 * prID + 1] = p.y;
		pVB_[8 * prID + 2] = p.z;
		pointReprArray[prID].v.position = p;
#if 1	// 텍스쳐 메쉬 변형
#ifdef __USE_A_OFFSET__
		for (int k = pointer_to__a_vVertexID_a_offset[prID]; k < pointer_to__a_vVertexID_a_offset[prID + 1]; ++k)
			vertexArray[pointer_to__a_vVertexID[k]].position = p;
#else
		const std::set<DWORD>&	vVertexID = pointReprArray[prID].vVertexID;
		for (std::set<DWORD>::const_iterator it = vVertexID.begin(); it != vVertexID.end(); ++it)
			vertexArray[*it].position = p;
#endif
#endif	// 텍스쳐 메쉬 변형
	}
}
}
}
#endif	// Vertex 위치 변경; ZBrush Smooth Brush
#endif	// !__KEKO__


#if 1	// Vertex 의 새로운 노말 결정
{
#if 1	// common
	// 각 Vertex 의 노말을 다시 계산;
	// Vertex 를 공유하는 Face 들의 노말의 평균이 해당 Vertex 의 노말
	// 1. 각 Face 의 노말을 저장할 임시 공간
	for (DWORD faceID = 0; faceID < (DWORD) mesh.GetNumFaces(); ++faceID) {
		if (faceIDArray[faceID] == 0)
			continue;

		CFace&	face = faceArray[faceID];
		D3DXVECTOR3&	p0 = pV[face.prID0].p;
		D3DXVECTOR3&	p1 = pV[face.prID1].p;
		D3DXVECTOR3&	p2 = pV[face.prID2].p;
		D3DXVECTOR3	v01 = p1 - p0;
//		D3DXVec3Normalize(&v01, &v01);
		D3DXVECTOR3	v02 = p2 - p0;
//		D3DXVec3Normalize(&v02, &v02);
		D3DXVECTOR3&	normal = face.normal;
		D3DXVec3Cross(&normal, &v01, &v02);
		D3DXVec3Normalize(&normal, &normal);
	}
	// 2. 각 Vertex 의 노말을 공유하는 Face 들의 노말로부터 계산
	for (DWORD prID = 0; prID < (DWORD) mesh.GetNumPointReprs(); ++prID) {
#if 1	// FLAG
		if (pointReprFlagArray[prID].subset == false)
			continue;
#else	// FLAG/!FLAG
		if (pointReprIDArray[prID] == 0)
			continue;
#endif	// !FLAG

		D3DXVECTOR3&	normal = pV[prID].n;
		unsigned	numFaces = 0;
#ifdef __USE_A_OFFSET__
		numFaces = pointer_to__a_vFaceIDLocalIndex_a_offset[prID + 1] - pointer_to__a_vFaceIDLocalIndex_a_offset[prID];
#else
		numFaces = pointReprArray[prID].vFaceIdLocalIndex.size();
#endif
#ifdef __USE_A_OFFSET__
		for (int k = pointer_to__a_vFaceIDLocalIndex_a_offset[prID]; k < pointer_to__a_vFaceIDLocalIndex_a_offset[prID + 1]; ++k) {
			const FACE_ID_LOCAL_INDEX&	faceIDLocalIndex = pointer_to__a_vFaceIDLocalIndex[k];
#else
		for (size_t k = 0; k < pointReprArray[prID].vFaceIdLocalIndex.size(); ++k) {
			const FACE_ID_LOCAL_INDEX&	faceIDLocalIndex = pointReprArray[prID].vFaceIdLocalIndex.at(k);
#endif
			normal += faceArray[faceIDLocalIndex.ID].normal;
#ifdef __USE_A_OFFSET__
		}
#else
		}
#endif
		normal /= (float) numFaces;
		D3DXVec3Normalize(&normal, &normal);
		// __meshPtr 에 사용되는 노말도 업데이트
	/*
		pVertices[i].n.x = pVB[8 * i + 3];
		pVertices[i].n.y = pVB[8 * i + 4];
		pVertices[i].n.z = pVB[8 * i + 5];
	*/
		pVB_[8 * prID + 3] = normal.x;
		pVB_[8 * prID + 4] = normal.y;
		pVB_[8 * prID + 5] = normal.z;
	}
#endif	// common
}
#endif	// Vertex 의 새로운 노말 결정
}


#ifdef AREA2_HINT
if (sub) {
	cout << "**** after ****" << endl;
	for (DWORD faceID = 0; faceID < (DWORD) mesh.GetNumFaces(); ++faceID) {
		if (faceIDArray[faceID] == 0)
			continue;

		CFace&	face = faceArray[faceID];
		D3DXVECTOR3&	p0 = pV[face.prID0].p;
		D3DXVECTOR3&	p1 = pV[face.prID1].p;
		D3DXVECTOR3&	p2 = pV[face.prID2].p;
		D3DXVECTOR3	v01 = p1 - p0;
		D3DXVECTOR3	v02 = p2 - p0;
		D3DXVECTOR3	Va;
		D3DXVec3Cross(&Va, &v01, &v02);
//		cout << "Area[" << face.id << "] = " << fabsf(D3DXVec3Length(&Va)) << endl;
		Area2&	area2 = area2_dic[faceID];
		area2.after = fabsf(D3DXVec3Length(&Va));
	}
}
#endif	// AREA2_HINT
#ifdef AREA2_HINT
if (sub) {
#if 0	// debugging...
	state.area2_vertex_array.clear();
	cout << "# of area2_vertex_array = " << state.area2_vertex_array.size() << endl;
	AREA2_HASH::const_iterator	cit_hash;
	for (cit_hash = area2_dic.begin(); cit_hash != area2_dic.end(); ++cit_hash) {
		DWORD	faceID = (*cit_hash).first;
		const Area2&	area2 = (*cit_hash).second;
		cout << "Area2[" << faceID << "]: " << area2.before << " -> " << area2.after << endl;
		if (area2.after > area2.before) {
			cout << "hello, world" << endl;
			CFace&	face = faceArray[faceID];
//			state.debugging_FaceID_array.push_back(faceID);
			// 중복되게 prID 가 들어간다. 하지만 보는 데에는 지장이 없다.
			state.area2_vertex_array.push_back(face.prID[0]);
			state.area2_vertex_array.push_back(face.prID[1]);
			state.area2_vertex_array.push_back(face.prID[2]);
		}
	}
	cout << "# of area2_vertex_array = " << state.area2_vertex_array.size() << endl;
#endif	// debugging...
	AREA2_HASH::iterator	it_hash;
	int	num_inc_faces = 0;
	state.area2_face_array.clear();
	for (it_hash = area2_dic.begin(); it_hash != area2_dic.end(); ++it_hash) {
		DWORD	faceID = (*it_hash).first;
		Area2&	area2 = (*it_hash).second;
//		cout << "Area2[" << faceID << "]: " << area2.before << " -> " << area2.after << endl;
#if 1
		float	delta_area_percent = (area2.after - area2.before) / area2.before * 100.f;
		if (delta_area_percent > (float) state.Area2Threshold) {
			state.area2_face_array.push_back(faceID);
			++num_inc_faces;
			area2.increase = true;
			CFace&	face = faceArray[faceID];
			for (int i = 0; i < 3; ++i) {
				DWORD	prID = face.prID[i];
#if 1	// FLAG
				if (!pointReprArray[prID].flag.fixed)
					pointReprFlagArray[prID].inc = true;
#else	// FLAG/!FLAG
				switch (pointReprIDArray[prID]) {
				case 3: // 이미 설정했다; face 단위로 처리되므로 같은 prID 가 두 번 설정될 수 있다...
//					cout << "pointReprIDArray[" << prID << "] = " << pointReprIDArray[prID] << " (= 3)" << endl;
					break;
				case 2:	// 움직인 점
					pointReprIDArray[prID] = 3;
					break;
				case 1:	// 움직이지 않는 점
					break;
				default:
					cout << "pointReprIDArray[" << prID << "] = " << pointReprIDArray[prID] << " (< 1!)" << endl;
					break;
				}
#endif	// !FLAG
			}
		}
#else
		if (area2.after > area2.before) {
			area2.increase = true;
			CFace&	face = faceArray[faceID];
			for (int i = 0; i < 3; ++i) {
				DWORD	prID = face.prID[i];
				switch (pointReprIDArray[prID]) {
				case 3: // 이미 설정했다; face 단위로 처리되므로 같은 prID 가 두 번 설정될 수 있다...
//					cout << "pointReprIDArray[" << prID << "] = " << pointReprIDArray[prID] << " (= 3)" << endl;
					break;
				case 2:	// 움직인 점
					pointReprIDArray[prID] = 3;
					break;
				case 1:	// 움직이지 않는 점
					break;
				default:
					cout << "pointReprIDArray[" << prID << "] = " << pointReprIDArray[prID] << " (< 1!)" << endl;
					break;
				}
			}
		}
#endif
	}
//	cout << "# of face area incresed = " << num_inc_faces << endl;
	for (DWORD prID = 0; prID < (DWORD) mesh.GetNumPointReprs(); ++prID) {
#if 1	// FLAG
		if (pointReprFlagArray[prID].inc == false)
			continue;
#else	// FLAG/!FLAG
		if (pointReprIDArray[prID] < 3)
			continue;
#endif	// !FLAG

		// 이 점이 움직이면 이 점을 공유하는 주변의 삼각형 중의 적어도 하나의 면적이 증가한다.
		// 몇 개의 삼각형의 면적이 증가하는가?
		unsigned	numFaces = 0;
		unsigned	numFaces_area_increased = 0;
#ifdef __USE_A_OFFSET__
		numFaces = pointer_to__a_vFaceIDLocalIndex_a_offset[prID + 1] - pointer_to__a_vFaceIDLocalIndex_a_offset[prID];
#else
		numFaces = pointReprArray[prID].vFaceIdLocalIndex.size();
#endif
#ifdef __USE_A_OFFSET__
		for (int k = pointer_to__a_vFaceIDLocalIndex_a_offset[prID]; k < pointer_to__a_vFaceIDLocalIndex_a_offset[prID + 1]; ++k) {
			const FACE_ID_LOCAL_INDEX&	faceIDLocalIndex = pointer_to__a_vFaceIDLocalIndex[k];
#else
		for (size_t k = 0; k < pointReprArray[prID].vFaceIdLocalIndex.size(); ++k) {
			const FACE_ID_LOCAL_INDEX&	faceIDLocalIndex = pointReprArray[prID].vFaceIdLocalIndex.at(k);
#endif
			DWORD	faceID = faceIDLocalIndex.ID;
			AREA2_HASH::const_iterator	cit = area2_dic.find(faceID);
			if (cit == area2_dic.end()) {
				// 다른 subset 에 있는 Face
				continue;
			}
			const Area2&	area2 = (*cit).second;
			if (area2.increase)
				++numFaces_area_increased;
#ifdef __USE_A_OFFSET__
		}
#else
		}
#endif
//		cout << "prID = " << prID << ": " << numFaces_area_increased << "/" << numFaces << endl;
		if (numFaces_area_increased == numFaces)	// TO DO LIST; all or nothing?
#if 1	// FLAG
			pointReprFlagArray[prID].all = true;
#else	// FLAG/!FLAG
			pointReprIDArray[prID] = 4;
#endif	// !FLAG
	}
#if 0	// debugging...
if (state.area2_control.ShowArea2 == 1) {	// inc
	state.area2_vertex_array.clear();
	for (DWORD prID = 0; prID < (DWORD) mesh.GetNumPointReprs(); ++prID) {
#if 1	// FLAG
		if (pointReprFlagArray[prID].inc == false)
			continue;
#else	// FLAG/!FLAG
		if (pointReprIDArray[prID] < 4)
			continue;
#endif	// !FLAG

		state.area2_vertex_array.push_back(prID);
	}
}
if (state.area2_control.ShowArea2 == 2) {	// all
	state.area2_vertex_array.clear();
	for (DWORD prID = 0; prID < (DWORD) mesh.GetNumPointReprs(); ++prID) {
#if 1	// FLAG
		if (pointReprFlagArray[prID].all == false)
			continue;
#else	// FLAG/!FLAG
		if (pointReprIDArray[prID] < 4)
			continue;
#endif	// !FLAG

		state.area2_vertex_array.push_back(prID);
	}
}
#endif	// debugging...
}
#endif	// AREA2_HINT

#if 1	// 특정 Vertex (Flag::repos 가 True 인) 의 위치를 재설정
#ifdef AREA2_HINT	// 특정 Vertex (Flag::repos 가 True 인) 의 위치를 재설정
if (
	sub &&
	(state.area2_control.ShowArea2 == 1 || state.area2_control.ShowArea2 == 2) &&
	state.area2_control.Repos
) {
#if 1	// FLAG
	// pointReprFlagArray[prID].repos 값이 True 인 Vertex 들을 원래 위치로
	// repos 를 설정...; TO DO LIST
if (state.area2_control.ShowArea2 == 1) {
	for (DWORD prID = 0; prID < (DWORD) mesh.GetNumPointReprs(); ++prID) {
		if (pointReprArray[prID].flag.fixed)
			continue;
		pointReprFlagArray[prID].repos = pointReprFlagArray[prID].inc;
	}
} else if (state.area2_control.ShowArea2 == 2) {
	cerr << "ShowArea2 = 2; not implemented yet" << endl;
}
#else	// FLAG/!FLAG
	DWORD	id = 4;
	// pointReprIDArray[prID] = id 인 Vertex 들을 원래 위치로
#endif	// !FLAG
{
#if 1	// Vertex 위치 변경; 노말방향으로...
#if 1	// LOOPing
	int	num_rest = 0;	// LOOPing 후에도 위치가 아직 결정되지 않은 Vertex 의 갯수
	int	loop_counter = 0;	// 문제가 있을 때 무한루프에 빠지지 않도록...
#define MAX_LOOP	10		// 최대 MAX_LOOP 만큼 반복
do {
	++loop_counter;
	cout << "LOOPing..." << endl;
	num_rest = 0;	// 초기화
#endif	// LOOPing
	for (DWORD prID = 0; prID < (DWORD) mesh.GetNumPointReprs(); ++prID) {
#if 1	// FLAG
		if (pointReprFlagArray[prID].repos == false)
			continue;
#else	// FLAG/!FLAG
		if (pointReprIDArray[prID] != id)
			continue;
#endif	// !FLAG

		kMesh::D3DXVERTEX&	v = pV[prID];

		D3DXVECTOR3&	p = v.p;
#if 1	// LOOPing
//		cout << "prID = " << prID << endl;
		if (pointReprFlagArray[prID].done == true)
			continue;
		// 이 Vertex 를 지금 단계에서 바꿀 수 있는 지 알아본다;
		// 이 Vertex 주변의 Vertex 들 중에서
		//	1) Flag::all 이 False 이거나 즉 Flag::inc 가 False 이거나
		//	Flag::inc 가 True 이더라도 어떤 기준에 의해 Flag::all 이 False 이거나
		//	2) Flag::done 이 True 인 Vertex 가 있는 지 알아본다.
		int	num_hint_vertex = 0;	// 이 Vertex 가 움직일 방향을 결정해줄 주변 Vertex 의 갯수
		D3DXVECTOR3	avg_p(0, 0, 0);	// num_hint_vertex 개의 주변 Vertex 들에 의해 결정되는
						// 이 Vertex 이 "평균" 위치
{
#ifdef __USE_A_OFFSET__
		for (int k = pointer_to__a_vEdgeID_a_offset[prID]; k < pointer_to__a_vEdgeID_a_offset[prID + 1]; ++k) {
			const EDGE	e = pointer_to__a_vEdgeID[k];
			const CEdge&	edge = edgeArray[e.id];
#else
		const CPointRepr&	pointRepr = pointReprArray[prID];
		for (size_t i = 0; i < pointRepr.vEdgeID.size(); ++i) {
			EDGE	e = pointRepr.vEdgeID.at(i);
			const CEdge&	edge = edgeArray[e.id];
#endif
			DWORD	prID_ref = edge.prStartID == prID ? edge.prEndID : edge.prStartID;
			const CPointRepr::Area2Flag&	flag = pointReprFlagArray[prID_ref];
			if (flag.repos && !flag.done)
				continue;
//			cout << "prID_ref = " << prID_ref << endl;
			// 이 Vertex 에서 주어진 Vertex 에 이르는 원래 벡터만큼,
			// 이 Vertex 의 새로운 위치에 더해 주어진 Vertex 의 새로운 위치를 결정한다.
			if (vertex_dic.find(prID_ref) == vertex_dic.end()) {
				cout << "prID_ref = " << prID_ref << "(" << prID << "): No vertex_dic" << endl;
				cout << ".fixed? " << (pointReprArray[prID_ref].flag.fixed ? "True" : "False") << endl;
				continue;
			}
			avg_p += pV[prID_ref].p + (vertex_dic[prID] - vertex_dic[prID_ref]);
			++num_hint_vertex;
#ifdef __USE_A_OFFSET__
		}
#else
		}
#endif
}
//		cout << "num_hint_vertex = " << num_hint_vertex << endl;
		if (num_hint_vertex == 0) {
			++num_rest;
			continue;
		}
		p = avg_p / (float) num_hint_vertex;
		pointReprFlagArray[prID].done = true;
#else	// LOOPing/!LOOPing
//		p = .5f * (vertex_dic[prID] + p);	// 원래 위치로...
		p = vertex_dic[prID];	// 원래 위치로...
#endif	// LOOPing

		pV_n[2 * prID + 0].Position = p;
		pV_n[2 * prID + 1].Position = p;
		pVB_[8 * prID + 0] = p.x;
		pVB_[8 * prID + 1] = p.y;
		pVB_[8 * prID + 2] = p.z;
		pointReprArray[prID].v.position = p;
#if 1	// 텍스쳐 메쉬 변형
#ifdef __USE_A_OFFSET__
		for (int k = mesh.pointer_to__a_vVertexID_a_offset[prID]; k < mesh.pointer_to__a_vVertexID_a_offset[prID + 1]; ++k)
			vertexArray[mesh.pointer_to__a_vVertexID[k]].position = p;
#else
		const std::set<DWORD>&	vVertexID = pointReprArray[prID].vVertexID;
		for (std::set<DWORD>::const_iterator it = vVertexID.begin(); it != vVertexID.end(); ++it)
			vertexArray[*it].position = p;
#endif
#endif	// 텍스쳐 메쉬 변형
	}
#if 1	// LOOPing
} while (num_rest && loop_counter < MAX_LOOP);
#endif	// LOOPing
#endif	// Vertex 위치 변경

#if 1	// Vertex 위치 변경; ZBrush Smooth Brush
if (state.area2_control.Smooth == 1 || state.area2_control.Smooth == 2) {
	// Smoothing 에 사용될 Vertex 는...
	for (DWORD prID = 0; prID < (DWORD) mesh.GetNumPointReprs(); ++prID) {
		if (state.area2_control.Smooth == 1) {
			if (state.area2_control.ShowArea2 == 1)
				pointReprFlagArray[prID].smooth = pointReprFlagArray[prID].inc;
			else if (state.area2_control.ShowArea2 == 1)
				pointReprFlagArray[prID].smooth = pointReprFlagArray[prID].all;
		} else if (state.area2_control.Smooth == 2)
			pointReprFlagArray[prID].smooth = pointReprFlagArray[prID].move;
	}
	if (state.area2_control.Smooth == 1 && state.area2_control.SmoothIncLevel) {
		// smooth 가 설정된 Vertex 에 대해 boundary0 설정
		for (DWORD prID = 0; prID < (DWORD) mesh.GetNumPointReprs(); ++prID) {
			CPointRepr::Area2Flag&	flag = pointReprFlagArray[prID];
			flag.boundary0 = flag.smooth;
		}
		for (unsigned n = 0; n < state.area2_control.SmoothIncLevel; ++n) {
			// boundary0 가 설정된 Vertex 주위의 Vertex 중에서
			// smooth 가 설정되지 않은 Vertex 에 boundary 를 설정한다.
			for (DWORD prID = 0; prID < (DWORD) mesh.GetNumPointReprs(); ++prID) {
				CPointRepr::Area2Flag&	flag = pointReprFlagArray[prID];
				if (flag.boundary0 == false) {
					flag.boundary = false;
					continue;
				}
{
#ifdef __USE_A_OFFSET__
		for (int k = pointer_to__a_vEdgeID_a_offset[prID]; k < pointer_to__a_vEdgeID_a_offset[prID + 1]; ++k) {
			const EDGE	e = pointer_to__a_vEdgeID[k];
			const CEdge&	edge = edgeArray[e.id];
#else
		const CPointRepr&	pointRepr = pointReprArray[prID];
		for (size_t i = 0; i < pointRepr.vEdgeID.size(); ++i) {
			EDGE	e = pointRepr.vEdgeID.at(i);
			const CEdge&	edge = edgeArray[e.id];
#endif
			DWORD	prID_ref = edge.prStartID == prID ? edge.prEndID : edge.prStartID;
			CPointRepr::Area2Flag&	flag_ref = pointReprFlagArray[prID_ref];
			flag_ref.boundary = !flag_ref.smooth;
#ifdef __USE_A_OFFSET__
		}
#else
		}
#endif
}
			}

			// boundary 가 설정된 Vertex 에 smooth 를 설정한다.
			for (DWORD prID = 0; prID < (DWORD) mesh.GetNumPointReprs(); ++prID) {
				CPointRepr::Area2Flag&	flag = pointReprFlagArray[prID];
				if (flag.smooth)	// 이미 설정되었다면 건너뛴다.
					continue;
				flag.smooth = flag.boundary;
			}

			// boundary 가 설정된 Vertex 가 boundary0 가 설정된 Vertex 가 된다.
			for (DWORD prID = 0; prID < (DWORD) mesh.GetNumPointReprs(); ++prID) {
				CPointRepr::Area2Flag&	flag = pointReprFlagArray[prID];
				flag.boundary0 = flag.boundary;
			}
		}
	}
for (int niter = 0; niter < (1 + (int) state.area2_control.SmoothIncLevel) * (int) mesh.statePtr()->brush.flag.SmoothLevel; ++niter)
{
	unsigned	NumSmooth = 0;
	for (DWORD prID = 0; prID < (DWORD) mesh.GetNumPointReprs(); ++prID) {
#if 1	// FLAG
		if (pointReprFlagArray[prID].smooth == false)
			continue;
#else	// FLAG/!FLAG
		if (pointReprIDArray[prID] != id)
			continue;
#endif	// !FLAG

		++NumSmooth;
		kMesh::D3DXVERTEX&	v = pV[prID];
		v.pTmp = v.p;
	}
	cout << "# of smoothed vertices = " << NumSmooth << endl;
	for (DWORD prID = 0; prID < (DWORD) mesh.GetNumPointReprs(); ++prID) {
#if 1	// FLAG
		if (pointReprFlagArray[prID].smooth == false)
			continue;
#else	// FLAG/!FLAG
		if (pointReprIDArray[prID] != id)
			continue;
#endif	// !FLAG

		kMesh::D3DXVERTEX&	v = pV[prID];
/*
 * 각 Edge 를 구성하는 두 Vertex 의 중점이 각 Vertex 의 새로운 위치에 영향을 미친다;
 * Vertex 의 새로운 위치들의 centroid 와 원래 위치의 중점의 실제로 새로운 위치가 된다.
 */
		// 주어진 Vertex 를 한 끝으로 하는 모든 Edge 에서 Edge 의 중점을 구한다.
		// 이 점들의 centroid 와 원래 점의 중점이 새로운 위치가 된다.
		D3DXVECTOR3	centroid(0, 0, 0);
		unsigned	numPoints = 0;
#ifdef __USE_A_OFFSET__
		numPoints = pointer_to__a_vEdgeID_a_offset[prID + 1] - pointer_to__a_vEdgeID_a_offset[prID];
		for (int k = pointer_to__a_vEdgeID_a_offset[prID]; k < pointer_to__a_vEdgeID_a_offset[prID + 1]; ++k) {
			const EDGE	e = pointer_to__a_vEdgeID[k];
			const CEdge&	edge = edgeArray[e.id];
			centroid += .5f * (pV[edge.prStartID].p + pV[edge.prEndID].p);
		}
#else
		const CPointRepr&	pointRepr = __pointReprPtr[prID];
		numPoints = pointRepr.vEdgeID.size();
		for (size_t i = 0; i < pointRepr.vEdgeID.size(); ++i) {
			EDGE	e = pointRepr.vEdgeID.at(i);
			const CEdge&	edge = edgeArray[e.id];
			centroid += .5f * (pV[edge.prStartID].p + pV[edge.prEndID].p);
		}
#endif
//		cout << "numPoints = " << numPoints << endl;
		centroid /= (float) numPoints;
		v.pTmp = .5f * (v.p + centroid);
	}
	for (DWORD prID = 0; prID < (DWORD) mesh.GetNumPointReprs(); ++prID) {
#if 1	// FLAG
		if (pointReprFlagArray[prID].smooth == false)
			continue;
#else	// FLAG/!FLAG
		if (pointReprIDArray[prID] != id)
			continue;
#endif	// !FLAG

		D3DXVECTOR3&	p = pV[prID].p;
		p = pV[prID].pTmp;

		pV_n[2 * prID + 0].Position = p;
		pV_n[2 * prID + 1].Position = p;
		pVB_[8 * prID + 0] = p.x;
		pVB_[8 * prID + 1] = p.y;
		pVB_[8 * prID + 2] = p.z;
		pointReprArray[prID].v.position = p;
#if 1	// 텍스쳐 메쉬 변형
#ifdef __USE_A_OFFSET__
		for (int k = pointer_to__a_vVertexID_a_offset[prID]; k < pointer_to__a_vVertexID_a_offset[prID + 1]; ++k)
			vertexArray[pointer_to__a_vVertexID[k]].position = p;
#else
		const std::set<DWORD>&	vVertexID = pointReprArray[prID].vVertexID;
		for (std::set<DWORD>::const_iterator it = vVertexID.begin(); it != vVertexID.end(); ++it)
			vertexArray[*it].position = p;
#endif
#endif	// 텍스쳐 메쉬 변형
	}
}
}
#endif	// Vertex 위치 변경; ZBrush Smooth Brush


#if 1	// Vertex 의 새로운 노말 결정
{
#if 1	// common
	// 각 Vertex 의 노말을 다시 계산;
	// Vertex 를 공유하는 Face 들의 노말의 평균이 해당 Vertex 의 노말
	// 1. 각 Face 의 노말을 저장할 임시 공간
	for (DWORD faceID = 0; faceID < (DWORD) mesh.GetNumFaces(); ++faceID) {
		if (faceIDArray[faceID] == 0)
			continue;

		CFace&	face = faceArray[faceID];
		D3DXVECTOR3&	p0 = pV[face.prID0].p;
		D3DXVECTOR3&	p1 = pV[face.prID1].p;
		D3DXVECTOR3&	p2 = pV[face.prID2].p;
		D3DXVECTOR3	v01 = p1 - p0;
//		D3DXVec3Normalize(&v01, &v01);
		D3DXVECTOR3	v02 = p2 - p0;
//		D3DXVec3Normalize(&v02, &v02);
		D3DXVECTOR3&	normal = face.normal;
		D3DXVec3Cross(&normal, &v01, &v02);
		D3DXVec3Normalize(&normal, &normal);
	}
	// 2. 각 Vertex 의 노말을 공유하는 Face 들의 노말로부터 계산
	for (DWORD prID = 0; prID < (DWORD) mesh.GetNumPointReprs(); ++prID) {
#if 1	// FLAG
		if (pointReprFlagArray[prID].subset == false)
			continue;
#else	// FLAG/!FLAG
		if (pointReprIDArray[prID] == 0)
			continue;
#endif	// !FLAG

		D3DXVECTOR3&	normal = pV[prID].n;
		unsigned	numFaces = 0;
#ifdef __USE_A_OFFSET__
		numFaces = pointer_to__a_vFaceIDLocalIndex_a_offset[prID + 1] - pointer_to__a_vFaceIDLocalIndex_a_offset[prID];
#else
		numFaces = __pointReprPtr[prID].vFaceIdLocalIndex.size();
#endif
#ifdef __USE_A_OFFSET__
		for (int k = pointer_to__a_vFaceIDLocalIndex_a_offset[prID]; k < pointer_to__a_vFaceIDLocalIndex_a_offset[prID + 1]; ++k) {
			const FACE_ID_LOCAL_INDEX&	faceIDLocalIndex = pointer_to__a_vFaceIDLocalIndex[k];
#else
		for (size_t k = 0; k < __pointReprPtr[prID].vFaceIdLocalIndex.size(); ++k) {
			const FACE_ID_LOCAL_INDEX&	faceIDLocalIndex = __pointReprPtr[prID].vFaceIdLocalIndex.at(k);
#endif
			normal += faceArray[faceIDLocalIndex.ID].normal;
#ifdef __USE_A_OFFSET__
		}
#else
		}
#endif
		normal /= (float) numFaces;
		D3DXVec3Normalize(&normal, &normal);
		// __meshPtr 에 사용되는 노말도 업데이트
	/*
		pVertices[i].n.x = pVB[8 * i + 3];
		pVertices[i].n.y = pVB[8 * i + 4];
		pVertices[i].n.z = pVB[8 * i + 5];
	*/
		pVB_[8 * prID + 3] = normal.x;
		pVB_[8 * prID + 4] = normal.y;
		pVB_[8 * prID + 5] = normal.z;
	}
#endif	// common
}
#endif	// Vertex 의 새로운 노말 결정
}
}
#endif	// AREA2_HINT
#endif	// 특정 Vertex (Flag::repos 가 True 인) 의 위치를 재설정


#if 1	// FLAG
	SAFE_DELETE_ARRAY(pointReprFlagArray);
#else	// FLAG/!FLAG
	SAFE_DELETE_ARRAY(pointReprIDArray);
#endif	// !FLAG
	SAFE_DELETE_ARRAY(faceIDArray);
#if 1	// Unlock...
	mesh.meshLoader().meshSubsetPtr()->UnlockVertexBuffer();
	mesh.meshLoader().meshPtr()->UnlockAttributeBuffer();
#if 1	// unlock
	hResult = mesh.GetMeshPointer()->UnlockVertexBuffer();
	assert(D3D_OK == hResult);
	mesh.GetVertexBufferPointer()->Unlock();
	pVB_n->Unlock();
#endif	// unlock
#endif	// Unlock...
}
#endif
#endif	// kDisplacementMap

float	kBone::smoothstep(float x1, float y1, float x2, float y2, float x)
{
	if (x1 == x2) {
		// x1 = x2 이므로 y1 = y2 이어야 할텐데...
		if (y1 != y2) {
			(void) fprintf(stderr, "FATAL: x1 = x2 but y1 != y2\n");
		}
		return y1;
	}
	float	x_left = x1;
	float	y_left = y1;
	float	x_right = x2;
	float	y_right = y2;
	if (x1 > x2) {
		// 입력하는 순서가 바뀌었다.
		(void) fprintf(stderr, "WARNING: x1 > x2\n");
		x_left = x2;
		y_left = y2;
		x_right = x1;
		y_right = y1;
	}
	// clamp...
	if (x < x_left) {
		(void) fprintf(stderr, "WARNING: x < x_left(%s)\n", x1 < x2 ? "x1" : "x2");
		return y_left;
	}
	if (x > x_right) {
		(void) fprintf(stderr, "WARNING: x > x_right(%s)\n", x1 < x2 ? "x2" : "x1");
		return y_right;
	}

	float	s = (x - x_left) / (x_right - x_left);
	float	dy = y_right - y_left;
	return y1 + s * s * (3 * dy - 2 * dy * s);
	// 만약 y1 = 0, y2 = 1 이라면...
	// return s * s * (3 - 2 * s)
}

#if 1	// 마지막 주문; 7. 다리 길이 변경
void	kBone::leg_elongate(kMesh& mesh, const POINT_DEF& pointDef, float scale)
{
	if (!&mesh) {
		cerr << "FATAL: kBone::set: mesh is nil" << endl;
		return;
	}

	// PLANE_DEF::normal 이 제대로된 방향인지 확인한 후, 설정한다.
	D3DXVECTOR3	P1;
	if (mesh.getPoint(pointDef, P1) < 0) {	// rightLegPointDef 을 써도 상관없다.
		cerr << "ERROR: kBone::leg_elongate: PLANE_DEF: kMesh::getPoint(POINT_DEF): Failure" << endl;
		return;
	}
	D3DXVECTOR3	nDir = D3DXVECTOR3(0, -1, 0);	// 항상 아래쪽

	CPointRepr*	pointReprArray = mesh.GetPointReprPointer();
	for (int n = 0; n < (int) mesh.GetNumPointReprs(); ++n) {
		CPointRepr&	pointRepr = pointReprArray[n];
		D3DXVECTOR3&	p = pointRepr.v.position;
		if (p.y > P1.y)	// 윗쪽
			continue;
		D3DXVECTOR3	ell = p - P1;
		D3DXVECTOR3	ell_longitudinal = D3DXVec3Dot(&ell, &nDir) * nDir;
		D3DXVECTOR3	ell_transversal = ell - ell_longitudinal;
		p = P1 + (scale * ell_longitudinal + ell_transversal);
	}

	// 발에 해당하는, 허벅지 기준 아래에 있는, 복셀도 움직인다.
if (mesh.meshLoader().GetNumVoxels()) {
	kMeshLoader::VOXEL_VERTEX*	voxelVertexArray;
	mesh.meshLoader().voxelVertexBufferPtr()->Lock(0, 0, (void**) &voxelVertexArray, 0);

	DWORD*	footIDArray = 0;
	footIDArray = new DWORD[mesh.meshLoader().GetNumVoxels()];
	FillMemory(footIDArray, mesh.meshLoader().GetNumVoxels() * sizeof(DWORD), 0);
do {
	// 발에 해당하는 복셀과 그 중에서 y값이 제일 큰 값을 알아낸다.
	float	y_max = -FLT_MAX;
	for (int index = 0; index < (int) mesh.meshLoader().GetNumVoxels(); ++index) {
		// 하나의 복셀이 실제로는 8개의 점으로 이루어진 정육면체를 만든다(kMeshLoader::__setVoxelBuffer)
		// 그 중에서도
		// 4번째(index = 3)번째 Vertex 가 left-lower-near 를 결정하고,
		const kMeshLoader::VOXEL_VERTEX&	voxel3 = voxelVertexArray[index * 8 + 3];
		const D3DXVECTOR3&	p3 = voxel3.p;
		// 6번째(index = 5)번째 Vertex 가 right-upper-far 를 결정한다.
		const kMeshLoader::VOXEL_VERTEX&	voxel5 = voxelVertexArray[index * 8 + 5];
		const D3DXVECTOR3&	p5 = voxel5.p;
		// 두 Vertex 의 중점이 해당 복셀의 위치가 된다.
		D3DXVECTOR3	p = .5f * (p3 + p5);
		// 이 점이 허벅지를 결정하는 평면 윗쪽에 있는 지 검사한다.
		D3DXVECTOR3	v = p - P1;
		if (D3DXVec3Dot(&v, &nDir) < 0)	// 반대방향이면 평면 아래쪽에 있다.
			continue;
		footIDArray[index] = 1;
		if (y_max < p.y)
			y_max = p.y;
	}
	D3DXVECTOR3	P0(0, y_max, 0);	// nDir 의 방향이 항상 (0, -1, 0)이다!
	for (int index = 0; index < (int) mesh.meshLoader().GetNumVoxels(); ++index) {
		if (footIDArray[index] == 0)
			continue;

		// 발에 해당하는 복셀만...
		// 복셀의 위치를 아래쪽으로 이동시킨다;
		// P1 에서 복셀의 위치까지는 크게 두 부분으로 나뉜다:
		//	P1 에서 발의 제일 윗부분(늘어나는 부분)
		//	발의 제일 윗부분에서 복셀까지 부분(늘어나지 않는 부분)
		const kMeshLoader::VOXEL_VERTEX&	voxel3 = voxelVertexArray[index * 8 + 3];
		const D3DXVECTOR3&	p3 = voxel3.p;
		// 6번째(index = 5)번째 Vertex 가 right-upper-far 를 결정한다.
		const kMeshLoader::VOXEL_VERTEX&	voxel5 = voxelVertexArray[index * 8 + 5];
		const D3DXVECTOR3&	p5 = voxel5.p;
		// 두 Vertex 의 중점이 해당 복셀의 위치가 된다.
		D3DXVECTOR3	p = .5f * (p3 + p5);
		// 복셀의 중심을 앞서처럼 움직인다.
		D3DXVECTOR3	ell = p - P1;
		D3DXVECTOR3	ell_longitudinal = D3DXVec3Dot(&ell, &nDir) * nDir;
		// ell_longitudinal 을 두 부분으로 나눈다;
		D3DXVECTOR3	ell_longitudinal0 = D3DXVECTOR3(0, y_max - P1.y, 0);
		D3DXVECTOR3	ell_foot = ell_longitudinal - ell_longitudinal0;
		D3DXVECTOR3	ell_transversal = ell - ell_longitudinal;
		D3DXVECTOR3	q = P1 + (scale * ell_longitudinal0 + ell_foot + ell_transversal);
		D3DXVECTOR3	v = q - p;	// 복셀의 중심이 움직인 상대적인 벡터
		// 이 복셀에 해당하는 8개의 Vertex 들은 v 만큼씩 움직인다.
		for (int i = 0; i < 8; ++i) {
			kMeshLoader::VOXEL_VERTEX&	voxel = voxelVertexArray[index * 8 + i];
			voxel.p += v;
		}
	}
} while (0);
	SAFE_DELETE_ARRAY(footIDArray);
	mesh.meshLoader().voxelVertexBufferPtr()->Unlock();
}
}
#endif	// 마지막 주문; 7. 다리 길이 변경
