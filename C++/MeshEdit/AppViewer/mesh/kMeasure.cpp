#include <iostream>
using std::cout;
using std::cerr;
using std::endl;
#include <map>
#include <mesh/kMeasure.h>
#include <mesh/kPolygon.h>	// CEdge
#include <mesh/kTapeMeasure.h>
#include <mesh/kMesh.h>

#if 1	// BONE_DEF
#include <math/linearleastsquare.h>
void	BONE_DEF::calculate(const kMesh& mesh, bool force)
{
	if (__calculated && !force)
		return;

	centroid = D3DXVECTOR3(0, 0, 0);
	nDir = D3DXVECTOR3(0, 0, 0);
	if (!__ptr) {
		cerr << "BONE_DEF::calculate(): __ptr is nil" << endl;
		return;
	}

	std::vector<D3DXVECTOR3>	v_list;
	const CFace*	faceArray = mesh.GetFacePointer();
	const DWORD*	faceIDArray = __ptr->ptr;
	DWORD*	pointReprIDArray = new DWORD[mesh.GetNumPointReprs()];
	FillMemory(pointReprIDArray, mesh.GetNumPointReprs() * sizeof(DWORD), 0);
	for (int n = 0; n < __ptr->n; ++n) {
		DWORD	faceID = faceIDArray[n];
		const CFace&	face = faceArray[faceID];
		for (int i = 0; i < 3; ++i)
			pointReprIDArray[face.prID[i]] = 1;
	}
	const CPointRepr*	pointReprArray = mesh.GetPointReprPointer();
	for (int n = 0; n < (int) mesh.GetNumPointReprs(); ++n) {
		if (pointReprIDArray[n] == 0)
			continue;
		v_list.push_back(pointReprArray[n].v.position);
	}
	SAFE_DELETE_ARRAY(pointReprIDArray);

	kLinearLeastSquare::linear_least_square_line_3(v_list, centroid, nDir);
	D3DXVec3Normalize(&nDir, &nDir);
#if 0	// 확인
	kMesh*	nonConstThis = (kMesh*) &mesh;
	nonConstThis->statePtr()->points.RemoveAll();
	nonConstThis->statePtr()->points.Add(centroid);
	D3DXVECTOR3	P;
	P = centroid + 50 * nDir;	// 100?
	nonConstThis->statePtr()->points.Add(P);
	P = centroid + 75 * nDir;
	nonConstThis->statePtr()->points.Add(P);
	P = centroid + 100 * nDir;
	nonConstThis->statePtr()->points.Add(P);
#endif
	__calculated = true;
}
#endif	// BONE_DEF

int	MEASURE_ITEM_PTR_DEF__TWO_POINT_DISTANCE::calculate(
	const kMesh&	mesh,
	bool&	closed,
	float&	length,
	CGrowableArray<SEGMENT_VERTEX>&	pointArray,
	REF_POINT*	refPointPtr,
#if 1	// BONE_DEF
	bool	/* use_bone_if_possible */,
#endif	// BONE_DEF
	bool	force
)
{
	if (__done && !force) {
		closed = __closed;
		length = __length;
		pointArray = __pointArray;

		return 0;
	}

	SEGMENT_VERTEX	P1;
#if 1	// 뼈
	mesh.getPoint(__twoPointDef.point0Def, P1.P, &P1.P0);
#else	// 뼈/!뼈
	mesh.getPoint(__twoPointDef.point0Def, P1.P);
#endif	// !뼈
	mesh.getNormal(__twoPointDef.point0Def, P1.N);
	P1.EdgeID = (DWORD) -1;
	P1.VertexID = (DWORD) -1;
	P1.FaceID = __twoPointDef.point0Def.ID;

	SEGMENT_VERTEX	P2;
#if 1	// 뼈
	mesh.getPoint(__twoPointDef.point1Def, P2.P, &P2.P0);
#else	// 뼈/!뼈
	mesh.getPoint(__twoPointDef.point1Def, P2.P);
#endif	// !뼈
	mesh.getNormal(__twoPointDef.point1Def, P2.N);
	P2.EdgeID = (DWORD) -1;
	P2.VertexID = (DWORD) -1;
	P2.FaceID = __twoPointDef.point1Def.ID;

	closed = false;
#if 1	// 뼈
	D3DXVECTOR3	v0 = P2.P0 - P1.P0;
	length = D3DXVec3Length(&v0);
#else	// 뼈/!뼈
	D3DXVECTOR3	v = P2.P - P1.P;
	length = D3DXVec3Length(&v);
#endif	// !뼈
	arrow_end_point = .5f * (P1.P + P2.P);
	if (refPointPtr) {
		if (refPointPtr->use_inPoint) {
			D3DXVECTOR3	v = refPointPtr->inPoint - P1.P;	// 이 벡터만큼 옮긴다.
			P1.P += v;
			P1.EdgeID = (DWORD) -1;
			P1.VertexID = (DWORD) -1;
			P1.FaceID = (DWORD) -1;
			P2.P += v;
			P2.EdgeID = (DWORD) -1;
			P2.VertexID = (DWORD) -1;
			P2.FaceID = (DWORD) -1;
		}
		if (refPointPtr->set_outPoint)
			refPointPtr->outPoint = P2.P;
	}

	pointArray.Add(P1);
	pointArray.Add(P2);

	// copy
	__done = true;
	__closed = closed;
	__length = length;
	__pointArray = pointArray;

	return 0;
}

int	MEASURE_ITEM_PTR_DEF__POINT_PLANE_DISTANCE::calculate(
	const kMesh&	mesh,
	bool&	closed,
	float&	length,
	CGrowableArray<SEGMENT_VERTEX>&	pointArray,
	REF_POINT*	refPointPtr,
#if 1	// BONE_DEF
	bool	use_bone_if_possible,
#endif	// BONE_DEF
	bool	force
)
{
	if (__done && !force) {
		closed = __closed;
		length = __length;
		pointArray = __pointArray;

		return 0;
	}
#if 1	// BONE_DEF
	if (__ptr)
		BONE_DEF::calculate(mesh, force);
#endif	// BONE_DEF

	SEGMENT_VERTEX	P1;
#if 1	// 뼈
	mesh.getPoint(__pointPlaneDef.pointDef, P1.P, &P1.P0);
#else	// 뼈/!뼈
	mesh.getPoint(__pointPlaneDef.pointDef, P1.P);
#endif	// !뼈
	mesh.getNormal(__pointPlaneDef.pointDef, P1.N);
	P1.EdgeID = (DWORD) -1;
	P1.VertexID = (DWORD) -1;
	P1.FaceID = __pointPlaneDef.pointDef.ID;

	SEGMENT_VERTEX	P2;
	P2.EdgeID = (DWORD) -1;
	P2.VertexID = (DWORD) -1;
	P2.FaceID = (DWORD) -1;
	// Q. 한 점 P 에서, 법선 벡터가 N 이고 평면 위의 한 점이 Q 인 평면 사이의 거리는?
	// A. P 를 지나고 N 에 평행한 벡터가 만드는 임의의 위치를 매개변수 t 로 쓰면
	// P + t * N 이다; 두 점 사이의 거리는 |t * N| 이다.
	// 이 점과 Q 를 잇는 벡터가 N 과 수직이어야 한다:
	//	((Q - P) + t * N) . N = 0
	//	t = (Q - P) . N / -N . N
	// N 이 단위벡터라면
	//	t = N . (P - Q)
	// 따라서 다른 한 점은 P - (N . (P - Q)) * N 이다.
	D3DXVECTOR3&	N = P2.N;
	N = __pointPlaneDef.planeDef.normal;
#if 1	// BONE_DEF
	if (__ptr)
		N = BONE_DEF::nDir;
#endif	// BONE_DEF
	D3DXVec3Normalize(&N, &N);
	D3DXVECTOR3	Q;
#if 1	// 뼈
	D3DXVECTOR3	Q0;
	mesh.getPoint(__pointPlaneDef.planeDef.pointDef, Q, &Q0);
#else	// 뼈/!뼈
	mesh.getPoint(__pointPlaneDef.planeDef.pointDef, Q);
#endif	// !뼈
	D3DXVECTOR3&	P = P1.P;
	float	t = D3DXVec3Dot(&N, &D3DXVECTOR3(P - Q));
	P2.P = P - t * N;
#if 1	// 뼈
	D3DXVECTOR3&	P0 = P1.P0;
	float	t0 = D3DXVec3Dot(&N, &D3DXVECTOR3(P0 - Q0));
	P2.P0 = P0 - t0 * N;
#endif	// 뼈

	closed = false;
#if 1	// 뼈
	length = fabsf(t0);
#else	// 뼈/!뼈
	length = fabsf(t);
#endif	// !뼈
	arrow_end_point = .5f * (P1.P + P2.P);
	if (refPointPtr) {
		if (refPointPtr->use_inPoint) {
			D3DXVECTOR3	v = refPointPtr->inPoint - P1.P;	// 이 벡터만큼 옮긴다.
			P1.P += v;
			P1.EdgeID = (DWORD) -1;
			P1.VertexID = (DWORD) -1;
			P1.FaceID = (DWORD) -1;
			P2.P += v;
			P2.EdgeID = (DWORD) -1;
			P2.VertexID = (DWORD) -1;
			P2.FaceID = (DWORD) -1;
		}
		if (refPointPtr->set_outPoint)
			refPointPtr->outPoint = P2.P;
	}

	pointArray.Add(P1);
	pointArray.Add(P2);

	// copy
	__done = true;
	__closed = closed;
	__length = length;
	__pointArray = pointArray;

	return 0;
}

int	MEASURE_ITEM_PTR_DEF__TWO_PLANE_DISTANCE::calculate(
	const kMesh&	mesh,
	bool&	closed,
	float&	length,
	CGrowableArray<SEGMENT_VERTEX>&	pointArray,
	REF_POINT*	refPointPtr,
#if 1	// BONE_DEF
	bool	use_bone_if_possible,
#endif	// BONE_DEF
	bool	force
)
{
	if (__done && !force) {
		closed = __closed;
		length = __length;
		pointArray = __pointArray;

		return 0;
	}
#if 1	// BONE_DEF
	if (__ptr)
		BONE_DEF::calculate(mesh, force);
#endif	// BONE_DEF

	SEGMENT_VERTEX	P1;
	P1.EdgeID = (DWORD) -1;
	P1.VertexID = (DWORD) -1;
	P1.FaceID = (DWORD) -1;

	SEGMENT_VERTEX	P2;
	P2.EdgeID = (DWORD) -1;
	P2.VertexID = (DWORD) -1;
	P2.FaceID = (DWORD) -1;

	D3DXVECTOR3&	P = P1.P;
#if 1	// 뼈
	D3DXVECTOR3&	P0 = P1.P0;
	mesh.getPoint(__twoPlaneDef.plane0Def.pointDef, P, &P0);
#else	// 뼈/!뼈
	mesh.getPoint(__twoPlaneDef.plane0Def.pointDef, P);
#endif	// !뼈
	P1.N = __twoPlaneDef.plane0Def.normal;
	D3DXVec3Normalize(&P1.N, &P1.N);
	D3DXVECTOR3&	N = P2.N;
	N = __twoPlaneDef.plane1Def.normal;
#if 1	// BONE_DEF
	if (__ptr)
		N = BONE_DEF::nDir;
#endif	// BONE_DEF
	D3DXVec3Normalize(&N, &N);
	cout << "N1.N2 = " << D3DXVec3Dot(&P1.N, &N) << endl;
	D3DXVECTOR3	Q;
#if 1	// 뼈
	D3DXVECTOR3	Q0;
	mesh.getPoint(__twoPlaneDef.plane1Def.pointDef, Q, &Q0);
#else	// 뼈/!뼈
	mesh.getPoint(__twoPlaneDef.plane1Def.pointDef, Q);
#endif	// !뼈
	float	t = D3DXVec3Dot(&N, &D3DXVECTOR3(P - Q));
	P2.P = P - t * N;
#if 1	// 뼈
	float	t0 = D3DXVec3Dot(&N, &D3DXVECTOR3(P0 - Q0));
	P2.P0 = P0 - t0 * N;
#endif	// 뼈

	closed = false;
#if 1	// 뼈
	length = fabsf(t0);
#else	// 뼈/!뼈
	length = fabsf(t);
#endif	// !뼈
	arrow_end_point = .5f * (P1.P + P2.P);
	if (refPointPtr) {
		if (refPointPtr->use_inPoint) {
			D3DXVECTOR3	v = refPointPtr->inPoint - P1.P;	// 이 벡터만큼 옮긴다.
			P1.P += v;
			P1.EdgeID = (DWORD) -1;
			P1.VertexID = (DWORD) -1;
			P1.FaceID = (DWORD) -1;
			P2.P += v;
			P2.EdgeID = (DWORD) -1;
			P2.VertexID = (DWORD) -1;
			P2.FaceID = (DWORD) -1;
		}
		if (refPointPtr->set_outPoint)
			refPointPtr->outPoint = P2.P;
	}

	pointArray.Add(P1);
	pointArray.Add(P2);

	// copy
	__done = true;
	__closed = closed;
	__length = length;
	__pointArray = pointArray;

	return 0;
}

int	MEASURE_ITEM_PTR_DEF__GIRTH::calculate(
	const kMesh&	mesh,
	bool&	closed,
	float&	length,
	CGrowableArray<SEGMENT_VERTEX>&	pointArray,
	REF_POINT*	/* refPointPtr */,
#if 1	// BONE_DEF
	bool	use_bone_if_possible,
#endif	// BONE_DEF
	bool	force
)
{
	if (__done && !force) {
		closed = __closed;
		length = __length;
		pointArray = __pointArray;

		return 0;
	}
#if 1	// BONE_DEF
	if (__ptr)
		BONE_DEF::calculate(mesh, force);
#endif	// BONE_DEF

	PLANE_DEF	planeDef = __planeDef;
#if 1	// BONE_DEF
	if (__ptr)
		planeDef.normal = BONE_DEF::nDir;
#endif	// BONE_DEF
	START_DEF_PTR	startDefPtr;
	startDefPtr.type = START_DEF_PTR_TYPE__GIRTH;
	startDefPtr.plane_def_ptr = &planeDef;
	CGrowableArray<SEGMENT_VERTEX>	segmentPointArray;
	CGrowableArray<SEGMENT_VERTEX>	twoPointArray;
	CGrowableArray<SEGMENT_VERTEX>	convexPointArray;
	(void) kTapeMeasure::doMeasure_s(
		&mesh,

		__mode,
		startDefPtr,

		// [OUT]
		NULL,
		segmentPointArray, length, closed,
		twoPointArray, convexPointArray
	);
	pointArray = __mode == TAPE_MEASURE_MODE_CONTOUR ? segmentPointArray : convexPointArray;

	mesh.getPoint(planeDef.pointDef, arrow_end_point);

	// copy
	__done = true;
	__closed = closed;
	__length = length;
	__pointArray = pointArray;

	return 0;
}

int	MEASURE_ITEM_PTR_DEF__TWO_POINT_GIRTH::calculate(
	const kMesh&	mesh,
	bool&	closed,
	float&	length,
	CGrowableArray<SEGMENT_VERTEX>&	pointArray,
	REF_POINT*	/* refPointPtr */,
#if 1	// BONE_DEF
	bool	/* use_bone_if_possible */,
#endif	// BONE_DEF
	bool	force
)
{
	if (__done && !force) {
		closed = __closed;
		length = __length;
		pointArray = __pointArray;

		return 0;
	}

	START_DEF_PTR	startDefPtr;
	startDefPtr.type = START_DEF_PTR_TYPE__TWO_POINT;
	startDefPtr.two_point_plane_def_ptr = &__2pDef;
	CGrowableArray<SEGMENT_VERTEX>	segmentPointArray;
	CGrowableArray<SEGMENT_VERTEX>	twoPointArray;
	CGrowableArray<SEGMENT_VERTEX>	convexPointArray;
	int	numSegments = kTapeMeasure::doMeasure_s(
		&mesh,

		__mode,
		startDefPtr,

		// [OUT]
		NULL,
		segmentPointArray, length, closed,
		twoPointArray, convexPointArray
	);
	(void) fprintf(stdout,
		"# of segments = %d, length = %f, closed? %s; "
		"# of segment point(s) = %d, # of two point(s) = %d, # of convex point(s) = %d\n"
		,
		numSegments, length, closed ? "True" : "False",
#if 1	// SEGMENT_VERTEX
		segmentPointArray.GetSize(), twoPointArray.GetSize(), convexPointArray.GetSize()
#else
		segmentPointArray.size(), twoPointArray.size(), convexPointArray.size()
#endif
	);
	pointArray = __mode == TAPE_MEASURE_MODE_CONTOUR ? twoPointArray : convexPointArray;

	mesh.getPoint(__2pDef.point0Def, arrow_end_point);

	// copy
	__done = true;
	__closed = closed;
	__length = length;
	__pointArray = pointArray;

	return 0;
}


MEASURE_ITEM::MEASURE_ITEM() :
	segmentVertexBuffer(0),
	segmentIndexBuffer(0),
	numSegments(0),
	length(0.f),
	closed(false),
#if 1	// kMeasure
	allSet(false)
#else	// kMeasure/!kMeasure
	allSet(false),
	__invalid(true)
#endif	// !kMeasure
{
//	(void) fprintf(stdout, "MEASURE_ITEM()\n");
}
MEASURE_ITEM::MEASURE_ITEM(
	const LPDIRECT3DDEVICE9	devicePtr,
#if 1	// kMeasure
	unsigned	maxSegments
#else	// kMeasure/!kMeasure
	unsigned	maxSegments,
	const LENGTH_DEF&	lengthDef
#endif	// !kMeasure
) : numSegments(0), length(0.f), closed(false), allSet(false)
#if 1	// manipulator; LINE MANIPULATOR
	, __isReleased(false)
#endif	// manipulator; LINE MANIPULATOR
{
#if 1	// kMeasure
	__Create(devicePtr, maxSegments);
#else	// kMeasure/!kMeasure
	__Create(devicePtr, maxSegments, lengthDef);
#endif	// !kMeasure
}
MEASURE_ITEM::~MEASURE_ITEM()
{
//			(void) fprintf(stdout, "~MEASURE_ITEM()\n");
	__Destroy();
}

#if 1	// kMeasure
void	MEASURE_ITEM::__Create(const LPDIRECT3DDEVICE9 devicePtr, unsigned maxSegments)
#else	// kMeasure/!kMeasure
void	MEASURE_ITEM::__Create(const LPDIRECT3DDEVICE9 devicePtr, unsigned maxSegments, const LENGTH_DEF& lengthDef)
#endif	// !kMeasure
{
	devicePtr->CreateVertexBuffer(
		maxSegments * sizeof(SEGMENT_VERTEX),
		D3DUSAGE_WRITEONLY,
		0,
		D3DPOOL_MANAGED,
		&segmentVertexBuffer,
		NULL
	);
	devicePtr->CreateIndexBuffer(
		maxSegments * sizeof(CEdge::LineIndex),
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX32,
		D3DPOOL_MANAGED,
		&segmentIndexBuffer,
		NULL
	);

#if 1	// kMeasure
#else	// kMeasure/!kMeasure
	*((LENGTH_DEF*) this) = lengthDef;
	__invalid = false;
#endif	// !kMeasure
}

void	MEASURE_ITEM::__Destroy()
{
	if (segmentVertexBuffer) {
		segmentVertexBuffer->Release();
		segmentVertexBuffer = 0;
	}
	if (segmentIndexBuffer) {
		segmentIndexBuffer->Release();
		segmentIndexBuffer = 0;
	}
}

MEASURE::MEASURE(
	const LPDIRECT3DDEVICE9	devicePtr,
	const std::vector<MEASURE_ITEM*>	m_item_ptr_list,
	MEASURE_TAG	tg
) : measure_item_ptr_list(m_item_ptr_list), tag(tg), length(0.f), lineVertexBuffer(0)
{
//	(void) fprintf(stdout, "MEASURE()\n");
	size_t	nElement = measure_item_ptr_list.size();

	if (nElement > 1) {
		devicePtr->CreateVertexBuffer(
			(UINT) nElement * sizeof(D3DXVECTOR3),	// 한 Segment 당 두 개의 점
			D3DUSAGE_WRITEONLY,
			0,
			D3DPOOL_MANAGED,
			&lineVertexBuffer,
			NULL
		);
	}
}
MEASURE::~MEASURE()
{
//	(void) fprintf(stdout, "~MEASURE()\n");
	__Destroy();
}

void	MEASURE::__Destroy()
{
	if (lineVertexBuffer) {
		lineVertexBuffer->Release();
		lineVertexBuffer = 0;
	}

	for (int i = 0; i < (int) measure_item_ptr_list.size(); ++i) {
		MEASURE_ITEM*&	measure_item_ptr = measure_item_ptr_list.at(i);	// SHOULD be reference!
		if (measure_item_ptr) {
			delete measure_item_ptr;
			measure_item_ptr = 0;
		}
	}
	measure_item_ptr_list.resize(0);
}
