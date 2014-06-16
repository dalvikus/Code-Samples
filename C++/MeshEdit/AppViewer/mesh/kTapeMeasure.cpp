#include <iostream>
using std::cout;
using std::cerr;
using std::endl;
#include <mesh/kTapeMeasure.h>
#include <mesh/kPolygon.h>
#include <mesh/kMesh.h>
#include <mesh/kState.h>
#include <assert.h>

const float	kTapeMeasure::Normal_Tolerance = 1.e-4f;
#if 0
extern DWORD		g__prID;
extern DWORD		g__lastPrID;
extern bool		g__Shift;
#endif
//extern FLOAT		g__a;

#if 1	// kState
#if 1	// Draw String...
#if 1	// Debugging...
extern std::vector<DWORD>	debugging_EdgeID_array;
extern std::vector<DWORD>	debugging_prReprID_array;
extern std::vector<DWORD>	debugging_FaceID_array;
#endif
#if 0
extern bool			g__showLowerFace;
extern std::vector<DWORD>	lower_FaceID_array;
extern std::vector<D3DXVECTOR3>	lower2_FaceID_array;
#endif

#if 0
extern bool		g__stringMoveO;
extern bool		g__bDir;	// g__stringMoveO 가 True 인 경우에만 사용된다;
					// Edge 의 어느 방향을 따라 움직이는 지 말해준다;
					// True:= prStartID -> prEndID, False:= prEndID -> prStartID
					// lastVertexID 가 (DWORD) -1 일 때만, 즉 Edge 상에 있을 때만, 의미가 있다.
#endif

#if 0
extern FLOAT		g__stringRotationAngleByAmount;
extern bool		g__stringHeading;
extern bool		g__stringAttitude;
#endif

#if 0
extern FLOAT		g__stringLength;			// String 의 총 길이
#endif
//extern D3DXVECTOR3	g__n;	// String 이 놓여있는 평면을 결정하는 법선벡터;
							// 여기서는 임의로 설정하고,
							// (g__stringHeading, g__stringAttitude) 를 이용하여 바꿀 수 있다.
							// 시작할 때 Normalized 된다.
#if 0
extern int		g__nSegment;			// String 을 구성하는 분할의 갯수
#endif

//extern DWORD		__lastVertexID;		// 이 값이 -1 이 아니면 Vertex 에서 시작
//extern DWORD		__lastEdgeID;		// 이 값이 -1 이 아니면 Vertex 가 아닌 Edge 위의 한 점에서 시작
							// 둘 중의 하나만 -1 이 아니어야 한다;
							// 모두 다 -1 이라면 둘레 선을 그리는 모드가 아니다.
//extern FLOAT		__lastT;			// Vertex 가 아닌 Edge 에서 시작할 때 시작점의 매개변수 t;
//extern DWORD		__lastFaceID;		// 고려의 대상에서 제외되는 Face ID;
							// g__stringContinueAtLastSegment 이 True 일 때만 의미가 있다.
// 마지막 Segment 의 끝점에서 값들...
//	String 을 그릴 때, g__stringContinueAtLastSegment 가 True 이면,
//	그 끝점을 시작점으로해서 String 을 그린다;
//	위의 모든 값들이 아래의 값들로 교체된다.
//extern bool		g__stringContinueAtLastSegment;
//extern DWORD		__lastVertexIDAtLastSegment;
//extern DWORD		__lastEdgeIDAtLastSegment;
//extern FLOAT		__lastTAtLastSegment;
//extern DWORD		__lastFaceIDAtLastSegment;
#endif	// Draw String...
#endif

kTapeMeasure::kTapeMeasure(LPDIRECT3DDEVICE9 devicePtr, kMesh* meshPtr) :
	__devicePtr(devicePtr), __meshPtr(meshPtr)
{
	__init();
}
kTapeMeasure::kTapeMeasure() : __devicePtr(0), __meshPtr(0)
{
	__init();
}
void	kTapeMeasure::setDeviceAndMesh(LPDIRECT3DDEVICE9 devicePtr, kMesh* meshPtr)
{
	__devicePtr = devicePtr;
	__meshPtr = meshPtr;
	__shiftAmount = ShiftFactor * __meshPtr->a();
}
kTapeMeasure::~kTapeMeasure()
{
	Destroy();
}
void	kTapeMeasure::Destroy()
{
#if 0
	// 아직 아무것도 없다.
	for (int i = 0; i < (int) measure_item_ptr_list.size(); ++i) {
		MEASURE_ITEM*&	measure_item_ptr = measure_item_ptr_list.at(i);
		if (measure_item_ptr) {
			delete measure_item_ptr;
			measure_item_ptr = 0;
		}
	}
	measure_item_ptr_list.resize(0);
#endif

#if 1	// 임의의 두 점 사이의 거리
	if (__twoPointVertexBufferPtr) {
		__twoPointVertexBufferPtr->Release();
		__twoPointVertexBufferPtr = 0;
	}
	if (__twoPointIndexBufferPtr) {
		__twoPointIndexBufferPtr->Release();
		__twoPointIndexBufferPtr = 0;
	}
#endif
#if 1	// Convex
	if (__convexVertexBufferPtr) {
		__convexVertexBufferPtr->Release();
		__convexVertexBufferPtr = 0;
	}
	if (__convexIndexBufferPtr) {
		__convexIndexBufferPtr->Release();
		__convexIndexBufferPtr = 0;
	}
#endif
}

int	kTapeMeasure::MAX_LOOP		= 2000;
FLOAT	kTapeMeasure::ShiftFactor	= .02f;
void	kTapeMeasure::__init()
{
#if 1	// manipulator; LINE MANIPULATOR
	__isReleased = false;
#endif	// manipulator; LINE MANIPULATOR
	__lastVertexID = (DWORD) -1;
	__lastEdgeID = (DWORD) -1;
	__lastT = 0.f;			// __lastVertexID 이 (DWORD) -1 가 아니라면 굳이 __lastT 가 필요하지않다.
	__lastFaceID = (DWORD) -1;	// 고려의 대상에서 제외되는 Face ID;
					// measureContinue 즉 이어서 계측할 때만 사용된다.

	// 마지막 Segment 의 끝점에서 값들...
	__lastVertexIDAtLastSegment = (DWORD) -1;
	__lastEdgeIDAtLastSegment = (DWORD) -1;
	__lastTAtLastSegment = 0.f;
	__lastFaceIDAtLastSegment = (DWORD) -1;

	if (__meshPtr) {
		__shiftAmount = ShiftFactor * __meshPtr->a();
	}

#if 1	// 임의의 두 점 사이의 거리
	__twoPointVertexBufferPtr = 0;
	__twoPointIndexBufferPtr = 0;
#endif
#if 1	// Convex
	__convexVertexBufferPtr = 0;
	__convexIndexBufferPtr = 0;
#endif

	__val			= 0;
	__val_saved		= 0;
	__length		= -1.f;
	__mode			= TAPE_MEASURE_MODE_CONTOUR;
	__method		= TAPE_MEASURE_METHOD_GIRTH;
	__crossSectionType	= TAPE_MEASURE_CROSS_SECTION_UNDEF;
}

// 둘레는 재는 시작점을 정한다.
void	kTapeMeasure::setBasePoint(
	DWORD	edgeID,
	FLOAT	t,
	DWORD	vertexID,
	DWORD	lastFaceID
#if 0	// doMeasure_s 에서 처리
	,
	const D3DXVECTOR3*	P1Ptr,
	const D3DXVECTOR3*	P2Ptr
#endif
)
{
	__lastEdgeID	= edgeID;
	__lastT		= t;
	__lastVertexID	= vertexID;

	__lastFaceID	= lastFaceID;

#if 0	// doMeasure_s 에서 처리
#if 1	// 임의의 두 점 사이의 거리
	__P1Ptr = P1Ptr;
	__P2Ptr = P2Ptr;
#endif
#endif
}

// nDir 벡터를, nDir 벡터와 수직인 axis 벡터 주위로 theta 만큼 회전시킨다;
//	Left-handed Coordinate System(LhCS):
//		왼손 엄지가 axis 방향
//		나머지 네 손가락이 감싸는 방향으로 회전
// 단위벡터 r1 를 수직인 단위벡터 r2 주위로 theta 만큼 반시계방향으로 회전시킨 벡터는 다음과 같다:
//	r1 (cos theta) + r2 x r1 (sin theta)
// 주) 여기서 r2 x r1 는 Left-handed Coordinate System 에서
// r2 를 왼손 엄지방향으로 했을 때 나머지 네 손가락이 감싸는 방향이다.
void	kTapeMeasure::setDir(D3DXVECTOR3& nDir, const D3DXVECTOR3& axis, FLOAT theta)
{
	D3DXVECTOR3	cross;
	D3DXVec3Cross(&cross, &axis, &nDir);
	nDir = nDir * cosf(theta) + cross * sinf(theta);
}

DWORD	kTapeMeasure::findEdge(
	const kMesh*	meshPtr,
	const D3DXVECTOR3&	nDir,

	// [IN]
	DWORD	faceID,
	FLOAT	u,	// barycentric coordinate U
	FLOAT	v,	// barycentric coordinate V

	FLOAT&	t,	// [OUT]
	bool	clockwise
)
{
	if (!meshPtr) {
		(void) fprintf(stderr, "FATAL: kTapeMeasure::findEdge: meshPtr is nil\n");
		return (DWORD) -1;
	}

	const CPointRepr*	pointReprArray = meshPtr->GetPointReprPointer();
	const CEdge*		edgeArray = meshPtr->GetEdgePointer();
	const CFace*		faceArray = meshPtr->GetFacePointer();

	const CFace&	face = faceArray[faceID];
	// 충돌하는 위치;
	// V1 + U * (V2 - V1) + V * (V3 - V1)
	const D3DXVECTOR3&	V1 = pointReprArray[face.prID0].v.position;
	const D3DXVECTOR3&	V2 = pointReprArray[face.prID1].v.position;
	const D3DXVECTOR3&	V3 = pointReprArray[face.prID2].v.position;
	const D3DXVECTOR3	P = V1 + u * (V2 - V1) + v * (V3 - V1);
	// 문제. P 를 지나고 nDir 에 수직인 단면이 삼각형과 만나는 Edge 상의 위치는?
	// 이론:
	//	시작점과 끝점이 각각 A, B 인 선분위의 한 점을 Q 라고 할 때,
	//	Q 는 다음과 같이 쓸 수 있다:
	//		Q = A + tAB, 여기서 AB = B - A 즉 A 에서 시작해서 B 에서 끝나는 벡터
	//	Q 가 이 선분 위에 있기 위해서는 0 <= t <= 1 이어야 한다: t = 0이면 Q = A, t = 1 이면 Q = B
	//	P 에서 Q 에 이르는 벡터(PQ)가 nDir 과 수직이라는 조건으로부터 t 를 구한다:
	//		PQ = Q - P = (A - P) + tAB = PA + tAB
	//
	//		nDir . PQ = 0
	//			  = nDir . PA + t nDir . AB
	//
	//	따라서 nDir . AB = 0 이 아니라면
	//
	//		t = - (nDir . PA) / (nDir . AB)
	//
	//	주1) nDir . AB = 0 인 경우는 AB 가 nDir 에 수직인 평면에 놓여있는 경우로서,
	//	P 가 AB 위에 있지 않는 한, P 를 지나고 nDir 에 수직인 평면이 AB 와 절대로 만나지 않는다.
	//
	//	주2) P 가 AB 위에 있는 지는 두 함수(PointLineSegmentIntersection, isPointOnLineSegment)를
	//	다음과 같이 이용한다.
	//		bool	isOn = false;	// P 가 AB 상에 있는가?
	//		float	s, distance;
	//		PointLineSegmentIntersection(A, B, P, s, distance);
	//		if (s >= 0.f && s <= 1.f) {	// P 에서 AB 에 내린 수선의 발, H가 AB 위에 위치한다;
	//						// distance = |PH|
	//			if (isPointOnLineSegment(distance)) {
	//				// P 가 AB 상에 있다.
	//				isOn = true;
	//			}
	//		}
	//	이 경우, A 또는 B 가 문제의 답이다.

	// 특별한 경우를 제외하고 두 Edge 와 만난다.
	// 이 중에서 handness 에 맞는 Edge 를 결정한다.
#if 1	// handness
	int	nEdge = 0;	// 만나는 Edge 의 수
	float	t0[2];		// 각 Edge 에서의 t 값
	DWORD	ID[2];		// 각 Edge 의 ID
	D3DXVECTOR3	Q[2];	// 각 Edge 와 만나는 점
#endif
	for (int iEdge = 0; iEdge < 3; ++iEdge) {
		DWORD	edgeID = face.edgeID[iEdge].id;
		const CEdge&	edge = edgeArray[edgeID];

		assert(edge.prEndID != (DWORD) -1);
		const D3DXVECTOR3&	A = pointReprArray[edge.prStartID].v.position;
		const D3DXVECTOR3	PA = A - P;
		assert(edge.prEndID != (DWORD) -1);
		const D3DXVECTOR3&	B = pointReprArray[edge.prEndID].v.position;
		const D3DXVECTOR3	AB = B - A;
		FLOAT	d = D3DXVec3Dot(&nDir, &AB);
		if (d == 0)	// nDir 에 수직인 평면과 AB 벡터가 평행이다; 절대로 만나지 않는다.
				// TO DO LIST: findEdge 에서 만나는 점이 Edge 가 아닌 Vertex 인 경우는 고려하지 않는다;
				// 고려하려면 앞의 [이론]에 따라 Vertex 를 구하고,
				// 이 함수를 호출한 곳에서
				// setBasePoint 를 이어서 호출할 때 해당 Vertex 를 적어주어야 하낟.
			continue;
		t = - D3DXVec3Dot(&nDir, &PA) / d;
		if (t > 1.f || t < 0.f) {	// Edge 의 연장선 상에서 만나다.
			continue;
		}
		// 이 Edge 와 만난다.
#if 1	// handness
		t0[nEdge] = t;
		ID[nEdge] = edgeID;
		Q[nEdge] = A + t * AB;
		++nEdge;
#ifdef _DEBUG2
		(void) fprintf(stdout, "kTapeMeasure::findEdge: clockwise = %s, t = %f, Edge ID = %d\n", clockwise ? "True" : "False", t, edgeID);
#endif
#else
		return edgeID;
#endif
	}
#if 1	// handness
	assert(nEdge == 2);
	// 문제. 둘 중의 어느 것이 handness 에 해당하는 것인가?
	// 답: DirectX 는 Left-handed (Cartesian) Coordinate System (LHS) 을 사용한다.
	// nDir 과 두 점 Q0, Q1 을 잇는 벡터 Q10 = Q1 - Q0 의 외적이,
	// 삼각형의 노말과 같은 방향인지 아닌지로 판단한다;
	// 같은 방향이라면 left-handed 그렇지 않다면 right-handed 이다.
#if 1
	D3DXVECTOR3	Q12 = Q[1] - Q[0];
	D3DXVECTOR3	nDirxQ12;	// 외적
	D3DXVECTOR3	N;	// 삼각형의 노말
{
	const D3DXVECTOR3&	N1 = pointReprArray[face.prID0].v.normal;
	const D3DXVECTOR3&	N2 = pointReprArray[face.prID1].v.normal;
	const D3DXVECTOR3&	N3 = pointReprArray[face.prID2].v.normal;
	N = N1 + u * (N2 - N1) + v * (N3 - N1);
}
	float	d = D3DXVec3Dot(&N, D3DXVec3Cross(&nDirxQ12, &nDir, &Q12));
#endif
	assert(d);
#if 0
	if (d) {	// Q[0] 에서 Q[1] 으로 향하는 벡터가 nDir 에 대해 clockwise 방향이다.
		if (clockwise) {	// Q[1]
		} else {		// Q[0]
		}
	} else {	// Q[1] 에서 Q[0] 으로 향하는 벡터가 nDir 에 대해 clockwise 방향이다.
		if (clockwise) {	// Q[0]
		} else {		// Q[1]
		}
	}
#endif
	if (d * (clockwise ? 1 : -1) > 0) {	// Q[1]
		t = t0[1];
		return ID[1];
	} else {			// Q[0]
		t = t0[0];
		return ID[0];
	}
#endif

#if 0
	// 이럴 수는 없는데...
	assert(0);
	return (DWORD) -1;
#endif
}

////////////////////////////////////////////////////////////////////////
// 둘레는 재는 시작점을 vDir 방향으로 움직인다.
//	최종 지점으로부터 nDir 방향으로 Edge 를 따라 움직인다;
//	1) Edge 를 따라 움직이므로 정확히 nDir 방향으로 움직일 수 없다.
//	nDir 방향으로 움직인다는 말은 움직인 벡터와 nDir 벡터의 내적이 양수가 되는 방향으로 움직인다는 것을 의미한다.
//	2) nDir 과 Edge 방향이 수직이면 움직일 수 없다.
//
// 리턴값:
//	<0	무슨 문제가 있다
//	0	가야할만큼 가지못하고 중간에 멈추었다:
//	1	가야할만큼 갔다
int	kTapeMeasure::moveBasePoint(const D3DXVECTOR3& vDir)
{
#ifdef __USE_A_OFFSET__
//	const DWORD*	pointer_to__a_vVertexID				= __meshPtr->pointer_to__a_vVertexID;
//	const int*	pointer_to__a_vVertexID_a_offset		= __meshPtr->pointer_to__a_vVertexID_a_offset;
	const EDGE*	pointer_to__a_vEdgeID				= __meshPtr->pointer_to__a_vEdgeID;
	const int*	pointer_to__a_vEdgeID_a_offset			= __meshPtr->pointer_to__a_vEdgeID_a_offset;
//	const FACE_ID_LOCAL_INDEX*	pointer_to__a_vFaceIDLocalIndex	= __meshPtr->pointer_to__a_vFaceIDLocalIndex;
//	const int*	pointer_to__a_vFaceIDLocalIndex_a_offset	= __meshPtr->pointer_to__a_vFaceIDLocalIndex_a_offset;
//	const DWORD*	pointer_to__a_faceID				= __meshPtr->pointer_to__a_faceID;
//	const int*	pointer_to__a_faceID_a_offset			= __meshPtr->pointer_to__a_faceID_a_offset;
#endif	// __USE_A_OFFSET__
	assert(__meshPtr);
	if (__lastVertexID == (DWORD) -1 && __lastEdgeID == (DWORD) -1) {
		return 0;	// 할 일이 없다.
	}

	int	iReturnVal = 1;

	// O 의 위치를 변화시킨다;
	// String 의 Segment 를 얻을 때와 마찬가지로, 모든 계산은 t 를 사용한다!
	const CPointRepr*	pointReprArray = __meshPtr->GetPointReprPointer();
	const CEdge*		edgeArray = __meshPtr->GetEdgePointer();
	bool&	bDir = __bDir;
	FLOAT	byAmountTotal = 0.1f * __meshPtr->a();	// 움직일 총 거리
	DWORD&	lastVertexID = __lastVertexID;
	DWORD&	lastEdgeID = __lastEdgeID;
	FLOAT&	t = __lastT;	// lastVertexID 가 (DWORD) -1 이 아닌 경우 즉
					// Vertex 가 아님 Edge 위의 한 점에서 시작할 때만 의미가 있다.
	while (byAmountTotal > 0.f) {
		if (lastVertexID == (DWORD) -1) {
			assert(lastEdgeID != (DWORD) -1);

			////////////////////////////////////////////////////////////////////////////////
			// Q. t 에 해당하는 위치에서 이동할 Vertex 까지의 거리는?
			//	Q. 그 Vertex 가 t = 0 인 위치인지 t = 1 인 위치인지 어떻게 아나?
			//		Q. 이동하고자 하는 방향은?
			//		A. vDir = g__Shift ? -g__n : g__n
			//		Q. Edge 를 따라 움직이는 방향(vMove)은?
			//		A. lastVertexID 가 (DWORD) -1 이 아닌 경우 결정된 Edge 의 bDir 이 말해준다:
			//			vMove = endV.p - startV.p
			//		Q. Edge 에서 t 가 증가하는 방향(vT)은?
			//		vMove 의 방향이 t 가 증가하는 방향(vT)은 아니다!
			//		bDir = True: vT 의 방향은  vMove 의 방향과 일치
			//		bDir = False: vT 의 방향은 vMove 의 방향과 반대
			//	A. 이동하고자 하는 방향(vDir)과 이동 중인 방향(vMove) 의 상대적인 상태
			//	그리고 t 가 증가하는 방향(vT)에 따라 다르다;
			// A. 현재의 Edge 를 따라 이동할 수 있는 총 t 를 구한다;
			// 따라서 이동가능한 총 거리는 이 값에도 Edge 의 길이를 곱한 값이 된다.
			const CEdge&	edge = edgeArray[lastEdgeID];
			DWORD	prStartID = (DWORD) -1, prEndID = (DWORD) -1;
			if (bDir) {
				prStartID = edge.prStartID;
				prEndID = edge.prEndID;
			} else {
				prStartID = edge.prEndID;
				prEndID = edge.prStartID;
			}
			const CPointRepr::VERTEX&	startV = pointReprArray[prStartID].v;
			const CPointRepr::VERTEX&	endV = pointReprArray[prEndID].v;
			D3DXVECTOR3	vMove = endV.position - startV.position;
			FLOAT	fDir = D3DXVec3Dot(&vMove, &vDir);
			if (fDir == 0.f) {	// 이동하려는 방향과 실제로 이동가능한 방향이 수직이므로
						// 더 이상 움직일 수 없다.
				iReturnVal = 0;	// 아직 더 가야하는데...
(void) fprintf(stderr, "kTapeMeasure::moveBasePoint: iReturnVal = 0: fDir = 0!\n");
				break;
			}
			FLOAT	fEdgeLength = D3DXVec3Length(&vMove);
			assert(fEdgeLength > 0.f);
			FLOAT	tLeft;	// 진행방향으로 남아있는 t
			if (fDir > 0.f) {	// Edge 의 방향과 같은 방향으로 움직여야한다.
				if (bDir) {	// t: 0 -> 1
					tLeft = 1.f - t;
				} else {	// t: 1 -> 0
					tLeft = t;
				}
			} else {	// Edge 의 방향과 반대 방향으로 움직여야한다.
				if (bDir) {	// t: 0 -> 1
					tLeft = t;
				} else {	// t: 1 -> 0
					tLeft = 1.f - t;
				}
			}
			FLOAT	fLengthLeft = fEdgeLength * tLeft;	// 식 (1)
			if (fLengthLeft <= byAmountTotal) {	// 아직 더 가야한다.
				byAmountTotal -= fLengthLeft;
				lastVertexID = fDir > 0.f ? prEndID : prStartID;
				lastEdgeID = (DWORD) -1;
				// t 값을 설정하는 것은 의미가 없다.
				//		t = bDir ? 1.f : 0.f;

				// 새로운 Edge 를 따라서...
				continue;
			}

			// t 값만 바꾸고, 여기서 멈춘다;
			// 앞의 "식 (1)"에서 fLengthLeft 대신에 byAmountTotal 를 사용해서 t 를 구한다.
			// lastVertexID(-1), lastEdgeID 는 그대로 유지된다.
			FLOAT	deltaT = byAmountTotal / fEdgeLength;	// 진행방향으로 증가해야하는 t
			if (fDir > 0.f) {	// Edge 의 방향과 같은 방향으로 움직여야한다.
				if (bDir) {	// t: 0 -> 1
					t += deltaT;
				} else {	// t: 1 -> 0
					t -= deltaT;
				}
			} else {	// Edge 의 방향과 반대 방향으로 움직여야한다.
				if (bDir) {	// t: 0 -> 1
					t -= deltaT;
				} else {	// t: 1 -> 0
					t += deltaT;
				}
			}
			// clamp...
			if (t < 0.f)
				t = 0.f;
			if (t > 1.f)
				t = 1.f;
			////////////////////////////////////////////////////////////////////////////////

			// 가야할만큼 갔다.
			iReturnVal = 1;
(void) fprintf(stderr, "kTapeMeasure::moveBasePoint: iReturnVal = 1: ok!\n");
			break;	// STOP!
		}

		// lastVertexID != (DWORD) -1
#ifdef __USE_A_OFFSET__
		size_t	nEdge = pointer_to__a_vEdgeID_a_offset[lastVertexID + 1] - pointer_to__a_vEdgeID_a_offset[lastVertexID];
#else
		const CPointRepr&	pointRepr = pointReprArray[lastVertexID];
		size_t	nEdge = pointRepr.vEdgeID.size();
#endif
		bool	bDone = false;		// True:= 앞으로 더 가야함을 뜻한다.
		bool	bAllDone = false;	// True:= 갈만큼 갔음을 뜻한다.
		for (size_t iEdge = 0; iEdge < nEdge; ++iEdge) {
			////////////////////////////////////////////////////////////////////////////////
			// Q. 어느 Edge 를 따라가야 vDir 과 같은 방향이 되는가?
			// A. lastVertexID 를 시작 점으로 하고 반대편에 있는 Vertex 를 끝 점으로하는 벡터를 생각한다;
			// EDGE::bDir 이 시작과 끝을 말해준다:
			//	vMove = endV.p - startV.p
			// 이 벡터가 vDir 와 같은 방향이 되는 Edge 를 따라가야한다.
			// NOTE: vMove 의 방향이 t 가 증가하는 방향(vT)을 뜻하지 않는다;
			// EDGE::bDir 이 말해준다:
			//	True:= vT 는 vMove 의 방향과 일치
			//	False:= vT 는 vMove 의 방향과 반대
#ifdef __USE_A_OFFSET__
			EDGE	Edge = pointer_to__a_vEdgeID[pointer_to__a_vEdgeID_a_offset[lastVertexID] + iEdge];
#else
			EDGE	Edge = pointRepr.vEdgeID.at(iEdge);
#endif
			DWORD	edgeID = Edge.id;
			bDir = Edge.bDir;
			const CEdge&	edge = edgeArray[edgeID];
			DWORD	prStartID = (DWORD) -1, prEndID = (DWORD) -1;
			if (bDir) {
				prStartID = edge.prStartID;	// lastVertexID
				prEndID = edge.prEndID;
			} else {
				prStartID = edge.prEndID;	// lastVertexID
				prEndID = edge.prStartID;
			}
			const CPointRepr::VERTEX&	startV = pointReprArray[prStartID].v;
			const CPointRepr::VERTEX&	endV = pointReprArray[prEndID].v;
			D3DXVECTOR3	vMove = endV.position - startV.position;
			FLOAT	fDir = D3DXVec3Dot(&vMove, &vDir);
			if (fDir <= 0.f) {	// 이동하려는 방향(vDir)과 이동하는 방향(vMove)이 일치해야한다!
				continue;
			}
			FLOAT	fEdgeLength = D3DXVec3Length(&vMove);
			if (fEdgeLength <= byAmountTotal) {	// 끝 점까지 가더라도 아직 더 가야한다.
				byAmountTotal -= fEdgeLength;
				lastVertexID = prEndID;
				lastEdgeID = (DWORD) -1;
				// t 값을 설정하는 것은 의미가 없다.
				//		t = bDir ? 1.f : 0.f;

				// 새로운 Edge 를 따라서...
				bDone = true;
				break;
			}
			// t 값만 바꾸고, 여기서 멈춘다;
			// 앞의 "식 (1)"에서 fLengthLeft 대신에 byAmountTotal 를 사용해서 t 를 구한다.
			// lastEdgeID 는 그대로 유지된다.
			t = byAmountTotal / fEdgeLength;
			if (!bDir)
				t = 1.f - t;
			lastVertexID = (DWORD) -1;
			lastEdgeID = edgeID;
			////////////////////////////////////////////////////////////////////////////////

			bAllDone = true;
			break;	// STOP!
		}
		if (bAllDone) {	// 갈만큼 다 갔다.
			iReturnVal = 1;
(void) fprintf(stderr, "kTapeMeasure::moveBasePoint: iReturnVal = 1: ok!\n");
			break;
		}
		if (!bDone) {	// 앞으로 한 발자국도 가지 못했다.
			// 더 이상 진행할 수가 없다.
			iReturnVal = 0;
(void) fprintf(stderr, "kTapeMeasure::moveBasePoint: iReturnVal = 0: no move\n");
			break;
		}
	}

	// 이어서 재는 데 사용되는 값들을 무효화한다.
#if 0
	__lastVertexIDAtLastSegment;
	__lastEdgeIDAtLastSegment;
	__lastTAtLastSegment;
	__lastFaceIDAtLastSegment;
#endif
	__lastFaceID = (DWORD) -1;

	return 0;
}

void	kTapeMeasure::measureContinue()
{
	__lastVertexID	= __lastVertexIDAtLastSegment;
	__lastEdgeID	= __lastEdgeIDAtLastSegment;
	__lastT		= __lastTAtLastSegment;
	__lastFaceID	= __lastFaceIDAtLastSegment;
}

#if 2	// ShowLower2Face
inline static void
FillLowerFace(
	DWORD	prStartID,
	DWORD	prEndID,
	float	t,

	const CPointRepr*		pointReprArray,
	const kMesh::D3DXVERTEX*	vertexBufferArray,

	LOWER_FACE&	lowerFace
)
{
	if (prEndID == -1) {	// t = 0
		const CPointRepr::VERTEX&	startV = pointReprArray[prStartID].v;
		lowerFace.p = startV.position;
		lowerFace.n = startV.normal;
		lowerFace.tu = startV.texcoord.x;
		lowerFace.tv = startV.texcoord.y;
		const kMesh::D3DXVERTEX&	startVc = vertexBufferArray[prStartID];
		lowerFace.c = startVc.c;

		return;
	}
	const CPointRepr::VERTEX&	startV = pointReprArray[prStartID].v;
	const CPointRepr::VERTEX&	endV = pointReprArray[prEndID].v;
	lowerFace.p = startV.position + t * (endV.position - startV.position);
	lowerFace.n = startV.normal + t * (endV.normal - startV.normal);
	lowerFace.tu = startV.texcoord.x + t * (endV.texcoord.x - startV.texcoord.x);
	lowerFace.tv = startV.texcoord.y + t * (endV.texcoord.y - startV.texcoord.y);
	const kMesh::D3DXVERTEX&	startVc = vertexBufferArray[prStartID];
	const kMesh::D3DXVERTEX&	endVc = vertexBufferArray[prEndID];
	lowerFace.c = startVc.c + t * (endVc.c - startVc.c);
}
#endif	// ShowLower2Face
#if 1
int	kTapeMeasure::doMeasure_s(
	const kMesh*		meshPtr,

	TAPE_MEASURE_MODE_T	mode,
	const START_DEF_PTR&	startDefPtr,
#if 0
	D3DXVECTOR3		nDir,	// 단면을 결정하는 법선벡터;
					// startDefPtr.type = START_DEF_PTR_TYPE__TWO_POINT 인 경우에는 따로 계산
					// startDefPtr.type = START_DEF_PTR_TYPE__GIRTH 인 경우 PLANE_DEF 에 있는 값을 사용
#endif

	// [OUT]
	POINT_INFO*		lastPointInfoPtr,
	CGrowableArray<SEGMENT_VERTEX>&	segmentPointArray,	// 단면의 의해서 잘리는 점들의 모임;
						// startDefPtr.type = START_DEF_PTR_TYPE__TWO_POINT 인 경우에는
						// 계산 과정에서 얻은 점들이 여기에 들어간다.
						// 따라서 twoPointArray 에 유용한 점들이 없다고 하더라도
						// 여기에는 그 과정에서 얻은 점들이 들어있다. (디버깅용)
	FLOAT&	length,		// startDefPtr.type = START_DEF_PTR_TYPE__TWO_POINT 인 경우:
       				//	twoPointArray 에 있는 점들이 만드는 열린 곡선의 길이
				// 그렇지 않은 경우:
				//	segmentPointArray 에 있는 점들이 만드는 곡선의 길이
	bool&	closed,		// segmentPointArray 에 있는 점들이 닫힌 곡선을 만드는 지 말해준다
	CGrowableArray<SEGMENT_VERTEX>&	twoPointArray,		// startDefPtr.type = START_DEF_PTR_TYPE__TWO_POINT 인 경우에 설정
	CGrowableArray<SEGMENT_VERTEX>&	convexPointArray	// mode = TAPE_MEASURE_MODE_WRAP 인 경우에 설정
)
{
#ifdef __USE_A_OFFSET__
//	const DWORD*	pointer_to__a_vVertexID				= meshPtr->pointer_to__a_vVertexID;
//	const int*	pointer_to__a_vVertexID_a_offset		= meshPtr->pointer_to__a_vVertexID_a_offset;
//	const EDGE*	pointer_to__a_vEdgeID				= meshPtr->pointer_to__a_vEdgeID;
//	const int*	pointer_to__a_vEdgeID_a_offset			= meshPtr->pointer_to__a_vEdgeID_a_offset;
	const FACE_ID_LOCAL_INDEX*	pointer_to__a_vFaceIDLocalIndex	= meshPtr->pointer_to__a_vFaceIDLocalIndex;
	const int*	pointer_to__a_vFaceIDLocalIndex_a_offset	= meshPtr->pointer_to__a_vFaceIDLocalIndex_a_offset;
	const DWORD*	pointer_to__a_faceID				= meshPtr->pointer_to__a_faceID;
	const int*	pointer_to__a_faceID_a_offset			= meshPtr->pointer_to__a_faceID_a_offset;
#endif	// __USE_A_OFFSET__
	const CPointRepr*	pointReprArray = meshPtr->GetPointReprPointer();
	const CEdge*		edgeArray = meshPtr->GetEdgePointer();
	const CFace*		faceArray = meshPtr->GetFacePointer();
#if 1
	// POINT_INFO 와 (있다면) TWO_POINT, 그리고 단면을 결정하는 nDir 를 설정한다.
	POINT_INFO	currentPoint;
	DWORD&	__lastVertexID	= currentPoint.vertexID;
	DWORD&	__lastEdgeID	= currentPoint.edgeID;
	FLOAT&	__lastT		= currentPoint.t;
	DWORD&	__lastFaceID	= currentPoint.faceID;
	D3DXVECTOR3	nDir;
	// startDefPtr.type = START_DEF_PTR_TYPE__TWO_POINT 인 경우에 설정
	SEGMENT_VERTEX*	__P1Ptr = 0;
	SEGMENT_VERTEX*	__P2Ptr = 0;
	// startDefPtr 로부터 설정; START_DEF_PTR_TYPE__TWO_POINT 의 경우엔 계산해서 설정

	if (0/* startDefPtr.type == START_DEF_PTR_TYPE__CONTINUE */) {
#if 0
		// POINT_INFO 로부터 currentPoint 를 설정; 단순한 복사
		currentPoint = *startDefPtr.last_point_ptr;

		// __P1Ptr, __P2Ptr 설정
		__P1Ptr = __P2Ptr = 0;
#endif
	} else if (startDefPtr.type == START_DEF_PTR_TYPE__GIRTH) {
		// PLANE_DEF 로부터 currentPoint 를 설정
		const PLANE_DEF&	planeDef = *startDefPtr.plane_def_ptr;
		nDir = planeDef.normal;	// nDir 값을 설정
		__lastVertexID = (DWORD) -1;
		__lastEdgeID = kTapeMeasure::findEdge(
			meshPtr,
			planeDef.normal,
			planeDef.pointDef.ID, planeDef.pointDef.baryU, planeDef.pointDef.baryV,
			__lastT,
			true
		);
		if (__lastEdgeID == (DWORD) -1) {
			(void) fprintf(stderr, "ERROR: kTapeMeasure::doMeasure_s: findEdge: Failure\n");
			return -1;
		}
		__lastFaceID = planeDef.pointDef.ID;

		// __P1Ptr, __P2Ptr 설정
		__P1Ptr = __P2Ptr = 0;
	} else if (startDefPtr.type == START_DEF_PTR_TYPE__TWO_POINT) {
		// TWO_POINT_PLANE_DEF 로부터 currentPoint 를 설정
		TWO_POINT_PLANE_DEF&	_2pDef = *startDefPtr.two_point_plane_def_ptr;
		static SEGMENT_VERTEX	P1;
		static SEGMENT_VERTEX	P2;
		__P1Ptr = &P1;
		__P2Ptr = &P2;
#if 1	// 뼈
		if (meshPtr->getPoint(_2pDef.point0Def, P1.P, &P1.P0) < 0) {
			(void) fprintf(stderr, "ERROR: kTapeMeasure::doMeasure_s: getPoint: Failure\n");
			return -1;
		}
		if (meshPtr->getPoint(_2pDef.point1Def, P2.P, &P2.P0) < 0) {
			(void) fprintf(stderr, "ERROR: kTapeMeasure::doMeasure_s: getPoint: Failure\n");
			return -1;
		}
#else	// 뼈/!뼈
		if (meshPtr->getPoint(_2pDef.point0Def, P1.P) < 0) {
			(void) fprintf(stderr, "ERROR: kTapeMeasure::doMeasure_s: getPoint: Failure\n");
			return -1;
		}
		if (meshPtr->getPoint(_2pDef.point1Def, P2.P) < 0) {
			(void) fprintf(stderr, "ERROR: kTapeMeasure::doMeasure_s: getPoint: Failure\n");
			return -1;
		}
#endif	// !뼈

	if (D3DXVec3LengthSq(&_2pDef.normal) == 0.f) {
		D3DXVECTOR3	N1, N2;	// 각 점에서의 법선벡터
		if (meshPtr->getNormal(_2pDef.point0Def, N1) < 0) {
			(void) fprintf(stderr, "ERROR: kTapeMeasure::doMeasure_s: getNormal: Failure\n");
			return -1;
		}
		if (meshPtr->getNormal(_2pDef.point1Def, N2) < 0) {
			(void) fprintf(stderr, "ERROR: kTapeMeasure::doMeasure_s: getNormal: Failure\n");
			return -1;
		}
		D3DXVECTOR3	N = .5f * (N1 + N2);	// 평균벡터
		// 현재 위치와 이전 위치 사이에서 둘레를 재자;
		// P1, P2 는 반드시 지난다.
		// 1. 평면에 수직인 방향을 정한다;
		// 이를 기준으로 아래 2. 의 findEdge 에서 시작 Edge 를 정한다.
		D3DXVec3Cross(&nDir, &D3DXVECTOR3(P2.P - P1.P), &N);	// nDir 값을 설정
		D3DXVec3Normalize(&nDir, &nDir);	// 길이를 1로 만든다.
		_2pDef.normal = nDir;	// 계산된 노말로 설정
#if 0	// 법선벡터를 확인하기 위하여...
		SetManipulator(Intersection1, nDir);
#endif
	} else {
		nDir = _2pDef.normal;
	}
#if 1	// SEGMENT_VERTEX
		D3DXVECTOR3	N1, N2;	// 각 점에서의 법선벡터
		if (meshPtr->getNormal(_2pDef.point0Def, N1) < 0) {
			(void) fprintf(stderr, "ERROR: kTapeMeasure::doMeasure_s: getNormal: Failure\n");
			return -1;
		}
		if (meshPtr->getNormal(_2pDef.point1Def, N2) < 0) {
			(void) fprintf(stderr, "ERROR: kTapeMeasure::doMeasure_s: getNormal: Failure\n");
			return -1;
		}
		P1.N = N1;
		P2.N = N2;
#if 1	// SEGMENT_VERTEX::{EdgeID,VertexID,FaceID}
		P1.EdgeID = (DWORD) -1;
		P1.VertexID = (DWORD) -1;
		P1.FaceID = _2pDef.point0Def.ID;
		P2.EdgeID = (DWORD) -1;
		P2.VertexID = (DWORD) -1;
		P2.FaceID = _2pDef.point1Def.ID;
#endif	// SEGMENT_VERTEX::{EdgeID,VertexID,FaceID}
#endif

		if (_2pDef.point0Def.ID == _2pDef.point1Def.ID) {	// 같은 평면 상에 있는 두 점 사이의 거리
			(void) fprintf(stderr, "onTheSameFace? True\n");
#if 1	// onTheSameFace; 복사해서 돌려준다.
			segmentPointArray.Add(P1);
			segmentPointArray.Add(P2);

			twoPointArray.RemoveAll();
			if (__P2Ptr) {
				twoPointArray.Add(P1);
				twoPointArray.Add(P2);
			}

			convexPointArray.RemoveAll();
			if (mode == TAPE_MEASURE_MODE_WRAP) {
				convexPointArray.Add(P1);
				convexPointArray.Add(P2);
			}

			D3DXVECTOR3	P12 = P2.P - P1.P;
			length = D3DXVec3Length(&P12);
#if 1	// 뼈
			D3DXVECTOR3	P120 = P2.P0 - P1.P0;
			length = D3DXVec3Length(&P120);
#endif	// 뼈
			closed = false;
#endif	// onTheSameFace

			return 1;	// # of segment(s)
		}

		// 2. 첫 번째 점에서 출발하자.
		__lastVertexID = (DWORD) -1;
		__lastEdgeID = kTapeMeasure::findEdge(
			meshPtr,
			nDir,

			_2pDef.point0Def.ID, _2pDef.point0Def.baryU, _2pDef.point0Def.baryV,

			__lastT,
			true				// 시작점에서 출발해야 한다.
		);
		__lastFaceID = _2pDef.point0Def.ID;
	} else if (startDefPtr.type == START_DEF_PTR_TYPE__TWO_POINT2) {
		// TWO_POINT_PLANE_DEF2 로부터 currentPoint 설정;
		// TWO_POINT_PLANE_DEF 와 비슷;
		// TWO_POINT_PLANE_DEF::point1Def 대신에 TWO_POINT_PLANE_DEF2::segmentVertex 사용
		TWO_POINT_PLANE_DEF2&	_2p2Def = *startDefPtr.two_point_plane_def2_ptr;
		static SEGMENT_VERTEX	P1;
		static SEGMENT_VERTEX	P2;
		__P1Ptr = &P1;
		__P2Ptr = &P2;
#if 1	// 뼈
		if (meshPtr->getPoint(_2p2Def.point0Def, P1.P, &P1.P0) < 0) {
			(void) fprintf(stderr, "ERROR: kTapeMeasure::doMeasure_s: getPoint: Failure\n");
			return -1;
		}
#else	// 뼈/!뼈
		if (meshPtr->getPoint(_2p2Def.point0Def, P1.P) < 0) {
			(void) fprintf(stderr, "ERROR: kTapeMeasure::doMeasure_s: getPoint: Failure\n");
			return -1;
		}
#endif	// !뼈
		P2 = _2p2Def.segmentVertex;

	if (D3DXVec3LengthSq(&_2p2Def.normal) == 0.f) {
		D3DXVECTOR3	N1, N2;	// 각 점에서의 법선벡터
		if (meshPtr->getNormal(_2p2Def.point0Def, N1) < 0) {
			(void) fprintf(stderr, "ERROR: kTapeMeasure::doMeasure_s: getNormal: Failure\n");
			return -1;
		}
		N2 = _2p2Def.segmentVertex.N;
		D3DXVECTOR3	N = .5f * (N1 + N2);	// 평균벡터
		// 현재 위치와 이전 위치 사이에서 둘레를 재자;
		// P1, P2 는 반드시 지난다.
		// 1. 평면에 수직인 방향을 정한다;
		// 이를 기준으로 아래 2. 의 findEdge 에서 시작 Edge 를 정한다.
		D3DXVec3Cross(&nDir, &D3DXVECTOR3(P2.P - P1.P), &N);	// nDir 값을 설정
		D3DXVec3Normalize(&nDir, &nDir);	// 길이를 1로 만든다.
		_2p2Def.normal = nDir;	// 계산된 노말로 설정
#if 0	// 법선벡터를 확인하기 위하여...
		SetManipulator(Intersection1, nDir);
#endif
	} else {
		nDir = _2p2Def.normal;
	}
		D3DXVECTOR3	N1;	// 각 점에서의 법선벡터
		if (meshPtr->getNormal(_2p2Def.point0Def, N1) < 0) {
			(void) fprintf(stderr, "ERROR: kTapeMeasure::doMeasure_s: getNormal: Failure\n");
			return -1;
		}
		P1.N = N1;
#if 1	// SEGMENT_VERTEX::{EdgeID,VertexID,FaceID}
		P1.EdgeID = (DWORD) -1;
		P1.VertexID = (DWORD) -1;
		P1.FaceID = _2p2Def.point0Def.ID;
#endif	// SEGMENT_VERTEX::{EdgeID,VertexID,FaceID}

#if 1	// SEGMENT_VERTEX::{EdgeID,VertexID,FaceID}
	// 시작점(point0Def)과 끝점(segmentVertex)가 같은 면에 있는 지 확인한다.
	bool	onTheSameFace = false;
	const POINT_DEF&	point0Def = _2p2Def.point0Def;
	const SEGMENT_VERTEX&	segmentVertex = _2p2Def.segmentVertex;
	if (segmentVertex.FaceID != (DWORD) -1) {
		assert(segmentVertex.EdgeID == (DWORD) -1);
		assert(segmentVertex.VertexID == (DWORD) -1);

		if (point0Def.ID == segmentVertex.FaceID) {	// 같은 평면 상에 있는 두 점 사이의 거리
			onTheSameFace = true;
		}
	} else if (segmentVertex.EdgeID != (DWORD) -1) {
		assert(segmentVertex.FaceID == (DWORD) -1);
		assert(segmentVertex.VertexID == (DWORD) -1);

		DWORD	edgeID = segmentVertex.EdgeID;
#ifdef __USE_A_OFFSET__
#else
		const CEdge	edge = edgeArray[edgeID];
#endif
		for (
#ifdef __USE_A_OFFSET__
			int	k = pointer_to__a_faceID_a_offset[edgeID];
#else
			int	k = 0;
#endif
#ifdef __USE_A_OFFSET__
			k < pointer_to__a_faceID_a_offset[edgeID + 1];
#else
			k < (int) edge.faceID.size();
#endif
			++k
		) {
#ifdef __USE_A_OFFSET__
			DWORD	faceID = pointer_to__a_faceID[k];
#else
			DWORD	faceID = edge.faceID[k];
#endif

			if (faceID == point0Def.ID) {	// 같은 평면 상에 있는 두 점 사이의 거리
				onTheSameFace = true;
				break;
			}
		}
	} else if (segmentVertex.VertexID != (DWORD) -1) {
		assert(segmentVertex.FaceID == (DWORD) -1);
		assert(segmentVertex.EdgeID == (DWORD) -1);

		DWORD	vertexID = segmentVertex.VertexID;
#ifdef __USE_A_OFFSET__
		size_t	nFace = pointer_to__a_vFaceIDLocalIndex_a_offset[vertexID + 1] - pointer_to__a_vFaceIDLocalIndex_a_offset[vertexID];
#else
		const CPointRepr&	pointRepr = pointReprArray[vertexID];
		size_t	nFace = pointRepr.vFaceIdLocalIndex.size();
#endif
		for (size_t n = 0; n < nFace; ++n) {
#ifdef __USE_A_OFFSET__
			const FACE_ID_LOCAL_INDEX&	faceIDLocalIndex = pointer_to__a_vFaceIDLocalIndex[pointer_to__a_vFaceIDLocalIndex_a_offset[vertexID] + n];
			DWORD	faceID = faceIDLocalIndex.ID;
#else
			DWORD	faceID = pointRepr.vFaceIdLocalIndex.at(n).ID;
#endif

			if (faceID == point0Def.ID) {
				onTheSameFace = true;
				break;
			}
		}
	} else {
		assert(0);
	}
	if (onTheSameFace) {
		(void) fprintf(stderr, "onTheSameFace? True\n");
#if 1	// onTheSameFace; 복사해서 돌려준다.
		segmentPointArray.Add(P1);
		segmentPointArray.Add(P2);

		twoPointArray.RemoveAll();
		if (__P2Ptr) {
			twoPointArray.Add(P1);
			twoPointArray.Add(P2);
		}

		convexPointArray.RemoveAll();
		if (mode == TAPE_MEASURE_MODE_WRAP) {
			convexPointArray.Add(P1);
			convexPointArray.Add(P2);
		}

		D3DXVECTOR3	P12 = P2.P - P1.P;
		length = D3DXVec3Length(&P12);
#if 1	// 뼈
		D3DXVECTOR3	P120 = P2.P0 - P1.P0;
		length = D3DXVec3Length(&P120);
#endif	// 뼈
		closed = false;
#endif	// onTheSameFace

		return 1;
	}
#endif	// SEGMENT_VERTEX::{EdgeID,VertexID,FaceID}

		// 2. 첫 번째 점에서 출발하자.
		__lastVertexID = (DWORD) -1;
		__lastEdgeID = kTapeMeasure::findEdge(
			meshPtr,
			nDir,

			_2p2Def.point0Def.ID, _2p2Def.point0Def.baryU, _2p2Def.point0Def.baryV,

			__lastT,
			true				// 시작점에서 출발해야 한다.
		);
		__lastFaceID = _2p2Def.point0Def.ID;
	} else
		assert(0);
#endif
	if (fabsf(D3DXVec3Length(&nDir) - 1.f) > Normal_Tolerance) {
		(void) fprintf(stdout, "nDir: [%f, %f, %f] (%f)\n", nDir.x, nDir.y, nDir.z, D3DXVec3Length(&nDir));
		assert(fabsf(D3DXVec3Length(&nDir) - 1.f) < Normal_Tolerance);
	}

#if 1	// 임의의 두 점 사이의 거리
	std::vector<float>	lengthArray;
#if 1	// 뼈
	std::vector<float>	lengthArray0;
#endif	// 뼈

	bool	n2_found = false;	// 두 점 사이의 거리를 재는 경우,
					// 시작점에서 출발한 곡선이 끝점을 지나는 지를 말해준다;
					// 이 값이 True 이어야만 두 점 사이를 지나는 곡선이 만들어 진다.
	// A:= P1 또는 P2 가 놓여있는 선분의 시작점, B:= P1 또는 P2 가 놓여있는 선분의 끝점
	float	dB1 = 1.f;	// P1 에서 첫 번째 segmentPointArray 성분까지의 거리;
				// 마지막에 twoPointArray 를 설정할 때 결정
	float	dA2 = 0.f;	// 마지막 segmentPointArray 성분에서 P2 까지의 거리;
				// n2_found 가 True 일 때 결정
#if 1	// 뼈
	float	dB10 = 1.f;
	float	dA20 = 0.f;
#endif	// 뼈
#endif

#if 1	// statePtr
	kState&	state = *(((kMesh*) meshPtr)->statePtr());
#endif
	length = 0.f;
#if 1	// 뼈
	FLOAT	length0 = 0.f;
#endif	// 뼈
	assert(meshPtr);
	if (__lastVertexID == (DWORD) -1 && __lastEdgeID == (DWORD) -1) {
		return 0;	// 할 일이 없다.
	}

// 시작점 P 를 지나고 nDir 에 수직인 단면의 외곽선을 결정하는 선 위의 점들의 집합을 구한다;
// 시작점은 Edge 상의 한 점이다;
// Edge 의 시작에서 매개변수 t 에 해당하는 위치에 있다: t = 0:= Edge 의 시작점, t = 1:= Edge 의 끝점
// 양 끝점(t = 0 또는 t = 1) 경우는 따로 처리한다; ID = g__stringVertexID
	// nDir 에 수직인 평면을 생각한다.
////////////////////////////////////////////////////////////////////////


#if 2	// ShowLower2Face
	kMesh::D3DXVERTEX*	vertexBufferArray;
	meshPtr->GetVertexBufferPointer()->Lock(0, 0, (void**) &vertexBufferArray, 0);
#endif	// ShowLower2Face
	// __lastT 로부터 시작점의 (P, N) 을 구한다.
	D3DXVECTOR3	P, N;
#if 1	// 뼈
	D3DXVECTOR3	P0;
#endif	// 뼈
#if 2	// ShowLower2Face
	LOWER_FACE	lowerFaceAtT;
#endif	// ShowLower2Face
	if (__lastVertexID == (DWORD) -1) {	// 정말로 필요한 곳이 이 경우이다.
		assert(__lastEdgeID != (DWORD) -1);
		const CEdge&	edge = edgeArray[__lastEdgeID];
		const CPointRepr::VERTEX&	startV = pointReprArray[edge.prStartID].v;
		const CPointRepr::VERTEX&	endV = pointReprArray[edge.prEndID].v;
		assert(__lastT >= 0.f && __lastT <= 1.f);
		P = startV.position + __lastT * (endV.position - startV.position);
#if 1	// 뼈
		const CPointRepr::VERTEX2&	startV0 = pointReprArray[edge.prStartID].v;
		const CPointRepr::VERTEX2&	endV0 = pointReprArray[edge.prEndID].v;
		P0 = startV0.position0 + __lastT * (endV0.position0 - startV0.position0);
		cout << "P: [" << P.x << ", " << P.y << ", " << P.z << "]" << endl;
		cout << "P0: [" << P0.x << ", " << P0.y << ", " << P0.z << "]" << endl;
#endif	// 뼈
		N = startV.normal + __lastT * (endV.normal - startV.normal);	// TO DO LIST: Quaternion 을 사용하자!
		D3DXVec3Normalize(&N, &N);
#if 2	// ShowLower2Face
		FillLowerFace(edge.prStartID, edge.prEndID, __lastT, pointReprArray, vertexBufferArray, lowerFaceAtT);
#endif	// ShowLower2Face
	} else {	// 해당 Vertex 에서의 (P, N) 을 이용한다.
		const CPointRepr::VERTEX&	v = pointReprArray[__lastVertexID].v;
		P = v.position;
#if 1	// 뼈
		const CPointRepr::VERTEX2&	v0 = pointReprArray[__lastVertexID].v;
		P0 = v0.position0;
#endif	// 뼈
		N = v.normal;
#if 2	// ShowLower2Face
		FillLowerFace(__lastVertexID, (DWORD) -1, 0, pointReprArray, vertexBufferArray, lowerFaceAtT);
#endif	// ShowLower2Face
	}
#if 1	// SEGMENT_VERTEX
	SEGMENT_VERTEX	v;
	v.P = P;
#if 1	// 뼈
	v.P0 = P0;
#endif	// 뼈
	v.N = N;
#if 1	// SEGMENT_VERTEX::{EdgeID,VertexID,FaceID}
	v.EdgeID = __lastVertexID == (DWORD) -1 ? __lastEdgeID : (DWORD) -1;
	v.VertexID = __lastVertexID == (DWORD) -1 ? (DWORD) -1 : __lastVertexID;
	v.FaceID = (DWORD) -1;
#endif	// SEGMENT_VERTEX::{EdgeID,VertexID,FaceID}
	segmentPointArray.Add(v);
#endif

{
	POINT_DEF&	pointDef = startDefPtr.plane_def_ptr->pointDef;
	cout << "************************************************************************" << endl;
	cout << "Tape Measure:" << endl;
	cout << "\tPOINT_DEF: (" << pointDef.ID << ", " << pointDef.baryU << "f, " << pointDef.baryV << "f)" << endl;
	cout << "\tnDir: [" << (float) nDir.x << ", " << (float) nDir.y << ", " << (float) nDir.z << "]" << endl;
	cout << "************************************************************************" << endl;
}
#if 2	// ShowLowerFace, ShowLower2Face
	if (state.g__bString && state.g__showLowerFace) {
		state.lower_FaceID_array.resize(0);
		state.lower2_FaceID_array.resize(0);
		state.lowerFaceArray.resize(0);

#if 2	// ShowLowerFace
#if 1
#if 1	// kMesh::CollectFaces
#if 0	// Between
#if 0	// 서로 다른 두 평면 사이에 있는 면들의 목록
		PLANE_DEF	planeDef0 = {					// 임의로 한 평면을 정해놓고 테스트
			{64158, 0.106995f, 0.710498f},
			D3DXVECTOR3(-0.537863f, 0.841028f, 0.0580994f)
		};
		PLANE_DEF	planeDef1;
		planeDef1.pointDef = startDefPtr.plane_def_ptr->pointDef;
		planeDef1.normal = nDir;
		kMesh::CollectBetweenFaces(planeDef0, planeDef1, state.lower_FaceID_array, meshPtr);
#else	// 평행한 두 평면 사이에 있는 면들의 목록
		POINT_DEF	pointDef = {61334, 0.592714f, 0.255863f};	// 임으로 한 점을 정해놓고 테스트
		PLANE_DEF	planeDef;
		planeDef.pointDef = startDefPtr.plane_def_ptr->pointDef;
		planeDef.normal = nDir;
		kMesh::CollectBetweenFaces(planeDef, pointDef, state.lower_FaceID_array, meshPtr);
#endif
#else	// Between/!Between
	// 평면 아래에 있는 면들의 목록
#if 1
		PLANE_DEF	planeDef;
		planeDef.pointDef = startDefPtr.plane_def_ptr->pointDef;
		planeDef.normal = nDir;
		kMesh::CollectLowerFaces(NULL, planeDef, state.lower_FaceID_array, meshPtr);
#else
		DWORD*	lowerFaceIDArray = 0;
		do {
			lowerFaceIDArray = new DWORD[meshPtr->GetNumFaces()];
			FillMemory(lowerFaceIDArray, meshPtr->GetNumFaces() * sizeof(DWORD), 0);

			PLANE_DEF	planeDef;
			planeDef.pointDef = startDefPtr.plane_def_ptr->pointDef;
			planeDef.normal = nDir;
			kMesh::CollectLowerFaces(NULL, planeDef, lowerFaceIDArray, meshPtr);

			for (int i = 0; i < (int) meshPtr->GetNumFaces(); ++i) {
				if (lowerFaceIDArray[i])
					state.lower_FaceID_array.push_back(i);
			}
		} while (0);
		if (lowerFaceIDArray) {delete[] lowerFaceIDArray; lowerFaceIDArray = 0;}
#endif
#endif	// !Between
#else	// kMesh::CollectFaces/!kMesh::CollectFaces
		// 단면에 의해 잘린 면이 이 점을 통과하는 면만 있는 것이 아니다; TO DO LIST
		// 1. 시작할 꼭지점을 정한다: startVertexID
		DWORD	startFaceID = startDefPtr.type == START_DEF_PTR_TYPE__GIRTH ?
			startDefPtr.plane_def_ptr->pointDef.ID : 
			startDefPtr.two_point_plane_def_ptr->point0Def.ID;
		const CFace&	startFace = faceArray[startFaceID];
		DWORD	startVertexID = -1;
		for (int k = 0; k < 3; ++k) {
			const CPointRepr&	pointRepr = pointReprArray[startFace.prID[k]];
			if (D3DXVec3Dot(&nDir, &D3DXVECTOR3(pointRepr.v.position - P)) < 0) {	// nDir 반대방향(아래쪽)
				startVertexID = startFace.prID[k];
				break;
			}
		}
		assert(startVertexID != -1);
		// 2. 이 꼭지점에 연결된 면들 중에서
		// nDir 반대방향에 있는 면(세 꼭지점이 모두 nDir 반대방향에 있는 면)들을
		// "순차적으로" 모은다.
		DWORD*	doneFaceIDArray = 0;
		DWORD*	donePrIDArray = 0;
	do {
		doneFaceIDArray = new DWORD[meshPtr->GetNumFaces()];
		FillMemory(doneFaceIDArray, meshPtr->GetNumFaces() * sizeof(DWORD), 0);
		donePrIDArray = new DWORD[meshPtr->GetNumPointReprs()];
		FillMemory(donePrIDArray, meshPtr->GetNumPointReprs() * sizeof(DWORD), 0);

		// startFace 도 이미 조사가 끝났다.
		doneFaceIDArray[startFaceID] = 1;
		// startVertexID 를 제외한 모든 점들은 이미 조사가 끝났다.
		donePrIDArray[startFace.prID[0]] = 1;
		donePrIDArray[startFace.prID[1]] = 1;
		donePrIDArray[startFace.prID[2]] = 1;
		donePrIDArray[startVertexID] = 0;
#if 1	// no stack overflow
		std::vector<DWORD>	nextFaceIDArray;
		nextFaceIDArray.push_back(startFaceID);
		while (nextFaceIDArray.size()) {
			std::vector<DWORD>	nextFaceIDArray_tmp;
			for (int n = 0; n < (int) nextFaceIDArray.size(); ++n) {
				CollectLowerFaces(
#ifdef __USE_A_OFFSET__
					meshPtr,
#endif	// __USE_A_OFFSET__
					faceArray[nextFaceIDArray.at(n)],
					nextFaceIDArray_tmp,

					P,
					nDir,

					pointReprArray,
					faceArray,

					donePrIDArray,
					doneFaceIDArray,

					state.lower_FaceID_array
				);
			}
			nextFaceIDArray = nextFaceIDArray_tmp;
		}
#endif	// no stack overflow
#if 0	// stack overflow
		CollectLowerFaces(
			startFace,

			P,
			nDir,

			pointReprArray,
			faceArray,

			donePrIDArray,
			doneFaceIDArray,

			state.lower_FaceID_array
		);
#endif	// stack overflow
	} while (0);
		if (doneFaceIDArray) {delete[] doneFaceIDArray; doneFaceIDArray = 0;}
		if (donePrIDArray) {delete[] donePrIDArray; donePrIDArray = 0;}
#endif	// !kMesh::CollectFaces
#else
		// 연결되지 않은 면들도 포함된다.
		for (int i = 0; i < (int) meshPtr->GetNumFaces(); ++i) {
			const CFace&	face = faceArray[i];
			bool	isLower = true;
			for (int k = 0; k < 3; ++k) {
				const CPointRepr&	pointRepr = pointReprArray[face.prID[k]];
				if (D3DXVec3Dot(&nDir, &D3DXVECTOR3(pointRepr.v.position - P)) > 0) {	// nDir 방향에 있다.
					isLower = false;
					break;
				}
			}
			if (!isLower)
				continue;

			state.lower_FaceID_array.push_back(i);
		}
#endif
#endif	// ShowLowerFace
	}
#endif	// ShowLowerFace, ShowLower2Face

	D3DXVECTOR3	O = P;		// 마무리된 Segment 의 끝점이자 새로운 Segment 의 시작점
#if 1	// 뼈
	D3DXVECTOR3	O0 = P0;
#endif	// 뼈
	FLOAT	t = __lastT;			// 단면상에 있는 점이 Edge 상에 위치하는 지를 말해준다;
					// (0, 1) 인 경우:= Edge 상에 위치한다
					// 0 또는 1:= Edge 의 끝 즉 Vertex 에 위치한다.
					// 그 외:= Edge 의 연장선 상에 위치한다.
					// lastVertexID 가 (DWORD) -1 이 아닌 경우에만 의미가 있다!
	DWORD	lastFaceID = __lastFaceID;	// 이어서 Segment 를 만들 때만 사용된다.
	DWORD	lastEdgeID = __lastEdgeID;
	DWORD	lastVertexID = __lastVertexID;

	// 처리된 Face ID 를 기록한다;
	//	주어진 Face 를 처리하려고 할 때,
	//	이미 그 Face 가 처리되었다면
	//	하나의 닫힌 String 을 그렸음을 뜻하므로 이 함수를 끝낸다.
	DWORD*	faceWalkHistoryArray = new DWORD[meshPtr->GetNumFaces()];
	FillMemory(faceWalkHistoryArray, meshPtr->GetNumFaces() * sizeof(DWORD), 0);
	bool	bAllDone = false;	// 닫힌 String 을 끄렸음을 뜻한다.
	for (int iLoop = 0; iLoop < MAX_LOOP; ++iLoop) {
		// 만들어진 총 선분의 갯수는, 문제가 없는 경우,
		//	두 점 사이의 거리의 경우: 1 + (segmentPointArray.GetSize() - 1) + 1
		//	둘레의 경우: segmentPointArray 의 갯수
		// 문제가 있는 경우란...
		//	둘레를 재는 경우 닫히지 않은 경우
		//	두 점 사이의 거리를 재는 경우 두 점을 지나는 곡선을 찾을 수 없는 경우
		// 이 경우 총 선분의 갯수는 다르다;
		// 실제로 총 성분의 갯수는 이 loop 다음에서 결정되는 numSegments 이다.
		if ((startDefPtr.type == START_DEF_PTR_TYPE__GIRTH ? segmentPointArray.GetSize() : 1 + segmentPointArray.GetSize()) == (int) kMesh::MAX_SEGMENT)
			break;

		if (lastVertexID != (DWORD) -1) {	// 단면이 꼭지점(Vertex)을 지난다.
#ifdef __USE_A_OFFSET__
			size_t	nFace = pointer_to__a_vFaceIDLocalIndex_a_offset[lastVertexID + 1] - pointer_to__a_vFaceIDLocalIndex_a_offset[lastVertexID];
#else
			const CPointRepr&	pointRepr = pointReprArray[lastVertexID];
			size_t	nFace = pointRepr.vFaceIdLocalIndex.size();
#endif
			for (size_t n = 0; n < nFace; ++n) {
#ifdef __USE_A_OFFSET__
				const FACE_ID_LOCAL_INDEX&	faceIDLocalIndex = pointer_to__a_vFaceIDLocalIndex[pointer_to__a_vFaceIDLocalIndex_a_offset[lastVertexID] + n];
				DWORD	faceID = faceIDLocalIndex.ID;
#else
				DWORD	faceID = pointRepr.vFaceIdLocalIndex.at(n).ID;
#endif
				if (lastFaceID != (DWORD) -1 && faceID == lastFaceID) {	// 방금 지나온 Face 라면
					// 건너뛴다.
					continue;
				}

				const CFace&	face = faceArray[faceID];
				// 이 Face 에서 lastVertexID 에 해당하는 Vertex 의 마주보는 Edge 는?
				DWORD	edgeID = (DWORD) -1;
#if 2
				int	index0 = -1;
				for (int i = 0; i < 3; ++i) {
					if (face.prID[i] == lastVertexID) {
						index0 = i;
						break;
					}
				}
				assert(index0 != -1);
				edgeID = index0 == 0 ? face.edgeID12.id : (index0 == 1 ? face.edgeID20.id : face.edgeID01.id);
#else
				if (face.prID0 == lastVertexID)
					edgeID = face.edgeID12.id;
				else if (face.prID1 == lastVertexID)
					edgeID = face.edgeID20.id;
				else if (face.prID2 == lastVertexID)
					edgeID = face.edgeID01.id;
				assert(edgeID != (DWORD) -1);
#endif

////////////////////////////////////////////////////////////////////////////////
#if 1
	// 이 Edge 상의 임의의 점 P 의 좌표는?
	// P 를 매개변수 t 를 써서 A, B 로 표시하면
	//	P = (1 - t) A + t B
	// OP 벡터는 OA + t AB 벡터이다.
	// 이 벡터가 nDir 과 수직이어야 하므로
	//	(OA + t AB) . nDir = 0
	// 으로부터 t 를 구할 수 있다.
	// 만약 AB . nDir = 0 이라면 nDir 에 수직인 평명과 AB 벡터가 평행이므로
	// 절대로 AB 와 만날 수 없다.
	const CEdge&	edge = edgeArray[edgeID];
	assert(edge.prStartID != (DWORD) -1);
	const D3DXVECTOR3&	A = pointReprArray[edge.prStartID].v.position;
	D3DXVECTOR3	OA = A - O;
	assert(edge.prEndID != (DWORD) -1);
	const D3DXVECTOR3&	B = pointReprArray[edge.prEndID].v.position;
	D3DXVECTOR3	AB = B - A;
#if 1	// 뼈
	const D3DXVECTOR3&	A0 = pointReprArray[edge.prStartID].v.position0;
	const D3DXVECTOR3&	B0 = pointReprArray[edge.prEndID].v.position0;
	D3DXVECTOR3	AB0 = B0 - A0;
#endif	// 뼈
	FLOAT	d = D3DXVec3Dot(&nDir, &AB);
	if (d == 0)	// nDir 에 수직인 평면과 AB 벡터가 평행이다; 절대로 만나지 않는다.
		continue;
	//	assert(d);
	float	t0 = - D3DXVec3Dot(&nDir, &OA) / d;
	if (t0 > 1.f || t0 < 0.f) {	// Edge 의 연장선 상에서 만나다.
		continue;
	}

#if 2	// ShowLower2Face
if (state.g__bString && state.g__showLowerFace) {
	LOWER_FACE	lowerFaceAtT0;
	FillLowerFace(edge.prStartID, edge.prEndID, t0, pointReprArray, vertexBufferArray, lowerFaceAtT0);

	DWORD	prID1, prID2;	// 다른 꼭지점(Vertex)에 해당하는 ID
	if (index0 == 0) {
		// 1, 2 중의 하나
		prID1 = face.prID[1];
		prID2 = face.prID[2];
	} else if (index0 == 1) {
		// 2, 0 중의 하나
		prID1 = face.prID[2];
		prID2 = face.prID[0];
	} else {
		// 0, 1 중의 하나
		prID1 = face.prID[0];
		prID2 = face.prID[1];
	}
	// 어느 꼭지점이 아래쪽(nDir 반대방향)에 있는가?
	const D3DXVECTOR3&	v1 = pointReprArray[prID1].v.position;
	const D3DXVECTOR3&	v2 = pointReprArray[prID2].v.position;
	if (D3DXVec3Dot(&nDir, &D3DXVECTOR3(v1 - O)) < 0) {	// 아래 방향이다.
		// (O, v1, O' = A + t * AB) 이 하나의 삼각형을 이룬다.
		state.lower2_FaceID_array.push_back(O);
		state.lower2_FaceID_array.push_back(v1);
		state.lower2_FaceID_array.push_back(A + t0 * AB);

		state.lowerFaceArray.push_back(lowerFaceAtT);
		LOWER_FACE	lowerFaceAtV1;
		FillLowerFace(prID1, (DWORD) -1, 0, pointReprArray, vertexBufferArray, lowerFaceAtV1);
		state.lowerFaceArray.push_back(lowerFaceAtV1);
		state.lowerFaceArray.push_back(lowerFaceAtT0);
	} else if (D3DXVec3Dot(&nDir, &D3DXVECTOR3(v2 - O)) < 0) {	// 아래 방향이다.
		// (O, O' = A + t * AB, v2) 이 하나의 삼각형을 이룬다.
		state.lower2_FaceID_array.push_back(O);
		state.lower2_FaceID_array.push_back(A + t0 * AB);
		state.lower2_FaceID_array.push_back(v2);

		state.lowerFaceArray.push_back(lowerFaceAtT);
		state.lowerFaceArray.push_back(lowerFaceAtT0);
		LOWER_FACE	lowerFaceAtV2;
		FillLowerFace(prID2, (DWORD) -1, 0, pointReprArray, vertexBufferArray, lowerFaceAtV2);
		state.lowerFaceArray.push_back(lowerFaceAtV2);
	}
}
#endif	// ShowLower2Face

	// 이 Edge 와 만난다.
	t = t0;
#if 2	// ShowLower2Face
	FillLowerFace(edge.prStartID, edge.prEndID, t, pointReprArray, vertexBufferArray, lowerFaceAtT);
#endif	// ShowLower2Face

	if (faceWalkHistoryArray[faceID]) {
		bAllDone = true;
		break;
	}
	faceWalkHistoryArray[faceID] = 1;

#if 1	// 0부터...
	O = A + t * AB;
#if 1	// 뼈
	O0 = A0 + t * AB0;
#endif	// 뼈
	const SEGMENT_VERTEX&	end = segmentPointArray[segmentPointArray.GetSize() - 1];
#if 1	// 임의의 두 점 사이의 거리
	if (__P2Ptr) {
		float	s, distance;
		PointLineSegmentIntersection(end.P, O, __P2Ptr->P, s, distance);
		if (s >= 0.f && s <= 1.f) {
			if (isPointOnLineSegment(meshPtr, distance)) {
				D3DXVECTOR3	PA = end.P - __P2Ptr->P;
				D3DXVECTOR3	PB = O - __P2Ptr->P;
				D3DXVECTOR3	AB = O - end.P;
				dA2 = D3DXVec3Length(&PA);
				float	dB2 = D3DXVec3Length(&PB);
				(void) fprintf(stdout,
					"s = %f, d = %f (%f): (%f + %f = %f(? %f))\n"
					, s,
					distance, distance / meshPtr->a(),
					dA2, dB2, dA2 + dB2, D3DXVec3Length(&AB)
				);
#if 1	// 뼈
				D3DXVECTOR3	PA0 = end.P0 - __P2Ptr->P0;
				D3DXVECTOR3	PB0 = O0 - __P2Ptr->P0;
				D3DXVECTOR3	AB0 = O0 - end.P0;
				dA20 = D3DXVec3Length(&PA0);
				float	dB20 = D3DXVec3Length(&PB0);
				(void) fprintf(stdout,
					"s = %f, d = %f (%f): (%f + %f = %f(? %f))\n"
					, s,
					distance, distance / meshPtr->a(),
					dA20, dB20, dA20 + dB20, D3DXVec3Length(&AB0)
				);
#endif	// 뼈
				const SEGMENT_VERTEX&	v1 = end;
				const SEGMENT_VERTEX&	v2 = *__P2Ptr;
				(void) fprintf(stdout,
					"len(P1,P2) = %f\n"
					"P1: [%f, %f, %f] (%d, %d, %d)\n"
					"P2: [%f, %f, %f] (%d, %d, %d)\n"
					,
					dA2,
					v1.P.x, v1.P.y, v1.P.z, v1.EdgeID, v1.VertexID, v1.FaceID,
					v2.P.x, v2.P.y, v2.P.z, v2.EdgeID, v2.VertexID, v2.FaceID
				);

				n2_found = true;
#if 1	// clockwise
				bAllDone = true;
				break;
#endif
			}
		}
	}
#endif
	float	len = D3DXVec3Length(&D3DXVECTOR3(O - end.P));
	lengthArray.push_back(len);
	length += len;
#if 1	// 뼈
	float	len0 = D3DXVec3Length(&D3DXVECTOR3(O0 - end.P0));
	lengthArray0.push_back(len0);
	length0 += len0;
#endif	// 뼈
	SEGMENT_VERTEX	v;
	v.P = O;
#if 1	// 뼈
	v.P0 = O0;
#endif	// !뼈
	const CPointRepr::VERTEX&	startV = pointReprArray[edge.prStartID].v;
	const CPointRepr::VERTEX&	endV = pointReprArray[edge.prEndID].v;
	v.N = startV.normal + t * (endV.normal - startV.normal);
#if 1	// SEGMENT_VERTEX::{EdgeID,VertexID,FaceID}
	// 바로 아래의 lastEdgeID, lastVertexID 의 계산과 동일
	if (t == 0.f || t == 1.f) {
		v.EdgeID = (DWORD) -1;
		v.VertexID = t == 0.f ? edge.prStartID : edge.prEndID;
	} else {
		v.EdgeID = edgeID;
		v.VertexID = (DWORD) -1;
	}
	v.FaceID = (DWORD) -1;
#endif	// SEGMENT_VERTEX::{EdgeID,VertexID,FaceID}
	segmentPointArray.Add(v);
#endif

	// 다음 Segment 를 만든다;
	// 1. 시작 위치가 Vertex 자체인지, 아니면 Edge 상의 한 점인지...
	lastVertexID = t == 1.f ? edge.prEndID : (t == 0.f ? edge.prStartID : (DWORD) -1);
	// 2. 시작 위치가 Edge 상의 한 점이라면 그 Edge 는...
	lastEdgeID = edgeID;
	if (t == 0.f || t == 1.f)
		lastEdgeID = (DWORD) -1;
	// 3. Segment 가 막 그려진 Face 는...
	lastFaceID = faceID;
#endif
////////////////////////////////////////////////////////////////////////////////

				break;
			}
		} else {	// lastVertexID = (DWORD) -1; 단면이 변(Edge)를 지난다.
			assert(lastEdgeID != (DWORD) -1);
			const CEdge	edge = edgeArray[lastEdgeID];
			// Edge 를 공유하는 두 Face 에 대해서...
#if 1	// debugging...
#define ADD_ID(id, array)				\
{							\
bool	isAdded = false;			\
for (size_t n_ = 0; n_ < array.size(); ++n_) {	\
	if (id == array.at(n_)) {		\
		isAdded = true;			\
		break;				\
	}					\
}						\
if (!isAdded)					\
	array.push_back(id);			\
}
#ifdef _DEBUG
#if 1	// FACE_ID_ARRAY
#ifdef __USE_A_OFFSET__
#else
if (edge.faceID.size() != 2) {
	(void) fprintf(stdout,
		"loop[%d]: # of faces != 2: "
		"Edge ID = %d (%d, %d) w/ ("
		,
		iLoop, edge.id, edge.prStartID, edge.prEndID
	);
	(void) fprintf(stdout, "%d", edge.faceID.at(0));
	for (size_t n = 1; n < edge.faceID.size(); ++n) {
		(void) fprintf(stdout, ", %d", edge.faceID.at(n));
	}
	(void) fprintf(stdout, ")\n");

	ADD_ID(edge.id, state.debugging_EdgeID_array);
	ADD_ID(edge.prStartID, state.debugging_prReprID_array);
	ADD_ID(edge.prEndID, state.debugging_prReprID_array);
	for (size_t n = 0; n < edge.faceID.size(); ++n)
		ADD_ID(edge.faceID.at(n), state.debugging_FaceID_array);
}
#endif
#endif
(void) fflush(stdout);
#endif
#undef ADD_ID
#endif
			for (
#ifdef __USE_A_OFFSET__
				int	k = pointer_to__a_faceID_a_offset[edge.id];
#else
				int	k = 0;
#endif
#if 1	// FACE_ID_ARRAY
#ifdef __USE_A_OFFSET__
				k < pointer_to__a_faceID_a_offset[edge.id + 1];
#else
				k < 2 && k < (int) edge.faceID.size();
#endif
#else
				k < 2;
#endif
				++k
			) {
#ifdef __USE_A_OFFSET__
				DWORD	faceID = pointer_to__a_faceID[k];
#else
				DWORD	faceID = edge.faceID[k];
#endif
#if 1	// FACE_ID_ARRAY
#else
				if (faceID == -1) {	// Warning...
					continue;
				}
#endif
				if (lastFaceID != (DWORD) -1 && lastFaceID == faceID) {	// 방금 처리된 Face 라면
					// 건너뛴다.
					continue;
				}

				const CFace&	face = faceArray[faceID];
				bool	bDone = false;
				// Face 를 구성하는 세 Edge 에 대해서...
#if 2
				int	index0 = -1;
				for (int i = 0; i < 3; ++i) {
					if (face.edgeID[i].id == lastEdgeID) {
						index0 = i;
						break;
					}
				}
				assert(index0 != -1);
#endif

				for (int i = 0; i < 3; ++i) {
					DWORD	edgeID = face.edgeID[i].id;
					if (edgeID == lastEdgeID) {	// 현재 고려 중인 Edge 를
						// (당연히) 제외한다.
						continue;
					}

////////////////////////////////////////////////////////////////////////////////
#if 1
	// 이 Edge 상의 임의의 점 P 의 좌표는?
	const CEdge&	edge = edgeArray[edgeID];
	assert(edge.prStartID != (DWORD) -1);
	const D3DXVECTOR3&	A = pointReprArray[edge.prStartID].v.position;
	D3DXVECTOR3	OA = A - O;
	assert(edge.prEndID != (DWORD) -1);
	const D3DXVECTOR3&	B = pointReprArray[edge.prEndID].v.position;
	D3DXVECTOR3	AB = B - A;
#if 1	// 뼈
	const D3DXVECTOR3&	A0 = pointReprArray[edge.prStartID].v.position0;
	const D3DXVECTOR3&	B0 = pointReprArray[edge.prEndID].v.position0;
	D3DXVECTOR3	AB0 = B0 - A0;
#endif	// 뼈
	FLOAT	d = D3DXVec3Dot(&nDir, &AB);
	if (d == 0)	// nDir 에 수직인 평면과 AB 벡터가 평행이다; 절대로 만나지 않는다.
		continue;
	//	assert(d);
	float	t0 = - D3DXVec3Dot(&nDir, &OA) / d;
	if (t0 > 1.f || t0 < 0.f) {	// Edge 의 연장선 상에서 만나다.
		continue;
	}

#if 2	// ShowLower2Face
if (state.g__bString && state.g__showLowerFace) {
	LOWER_FACE	lowerFaceAtT0;
	FillLowerFace(edge.prStartID, edge.prEndID, t0, pointReprArray, vertexBufferArray, lowerFaceAtT0);

	// 단면이 index0 번째 Edge 상의 한 점(a)과 i번째 Edge 상의 한 점(b)을 지난다;
	// 세 Vertex 가 이 단면에 의해 하나의 Vertex(va), 두 개의 Vertex(vb, vc) 로 나뉜다.
	// 하나의 Vertex 가 nDir 과 반대방향에 있다면 (a, va, b) 가
	// 두 개의 Vertex 가 nDir 과 반대방향에 있다면 (a, b, vm), (b, vb, vm), (vc, a, vm) 가
	// 반대방향에 있는 삼각형이다; vm 은 vb 와 vc 의 중점이다.
	// i 번째 Edge 가 index0 번째 바로 다음 Edge 라는 가정이고,
	// 반대로 index0 번째 Edge 가 i 번째 Edge 바로 다음 Edge 라면 결과는 다음과 같다.
	//	(b, va, a) 또는
	//	(b, a, vm), (a, vb, vm), (vc, b, vm)
	// 여기서 바로 다음 Edge 라 할 때의 방향은
	// 삼각형을 이루는 Vertex 들을 결정하는 반시계방향을 뜻한다.
	//
	// 여기서 a = O, b = A + t * AB 이다;
	// 먼저 (va), (vb, vc) 을 알아내자.
	// index0 = 0, i = 1	(va = v1), (vb = v2, vc = v0)
	// index0 = 0, i = 2	(va = v0), (vb = v1, vc = v2)
	// index0 = 1, i = 2	(va = v2), (vb = v0, vc = v1)
	// index0 = 1, i = 0	(va = v1), (vb = v2, vc = v0)
	// index0 = 2, i = 0	(va = v0), (vb = v1, vc = v2)
	// index0 = 2, i = 1	(va = v2), (vb = v0, vc = v1)
	DWORD	prIDa = (DWORD) -1, prIDb = (DWORD) -1, prIDc = (DWORD) -1;	// va, vb, vc 에 해당하는 Vertex ID
	bool	clockwise = true;	// i 번째 Edge 가 index0 번째 Edge 의 바로 다음 Edge 인가?
	if (index0 == 0) {
		if (i == 1) {
			prIDa = face.prID[1];
			prIDb = face.prID[2];
			prIDc = face.prID[0];
			clockwise = true;
		} else if (i == 2) {
			prIDa = face.prID[0];
			prIDb = face.prID[1];
			prIDc = face.prID[2];
			clockwise = false;
		} else {
			assert(0);
		}

	} else if (index0 == 1) {
		if (i == 2) {
			prIDa = face.prID[2];
			prIDb = face.prID[0];
			prIDc = face.prID[1];
			clockwise = true;
		} else if (i == 0) {
			prIDa = face.prID[1];
			prIDb = face.prID[2];
			prIDc = face.prID[0];
			clockwise = false;
		} else {
			assert(0);
		}
	} else {
		assert(index0 == 2);
		if (i == 0) {
			prIDa = face.prID[0];
			prIDb = face.prID[1];
			prIDc = face.prID[2];
			clockwise = true;
		} else if (i == 1) {
			prIDa = face.prID[2];
			prIDb = face.prID[0];
			prIDc = face.prID[1];
			clockwise = false;
		} else {
			assert(0);
		}
	}
	const D3DXVECTOR3&	a = O;
	const D3DXVECTOR3&	b = A + t0 * AB;
	const D3DXVECTOR3&	va = pointReprArray[prIDa].v.position;
	const D3DXVECTOR3&	vb = pointReprArray[prIDb].v.position;
	const D3DXVECTOR3&	vc = pointReprArray[prIDc].v.position;
	LOWER_FACE	lowerFaceAtVa;
	FillLowerFace(prIDa, (DWORD) -1, 0, pointReprArray, vertexBufferArray, lowerFaceAtVa);
	LOWER_FACE	lowerFaceAtVb;
	FillLowerFace(prIDb, (DWORD) -1, 0, pointReprArray, vertexBufferArray, lowerFaceAtVb);
	LOWER_FACE	lowerFaceAtVc;
	FillLowerFace(prIDc, (DWORD) -1, 0, pointReprArray, vertexBufferArray, lowerFaceAtVc);
	// (va), (vb, vc) 중의 어느 쌍이 nDir 과 반대방향에 있는가?
	if (D3DXVec3Dot(&nDir, &D3DXVECTOR3(va - O)) < 0) {
		// (va) 가 반대방향에 있다.
		if (clockwise) {	// (a, va, b)
			state.lower2_FaceID_array.push_back(a);
			state.lower2_FaceID_array.push_back(va);
			state.lower2_FaceID_array.push_back(b);

			state.lowerFaceArray.push_back(lowerFaceAtT);
			state.lowerFaceArray.push_back(lowerFaceAtVa);
			state.lowerFaceArray.push_back(lowerFaceAtT0);
		} else {		// (b, va, a)
			state.lower2_FaceID_array.push_back(b);
			state.lower2_FaceID_array.push_back(va);
			state.lower2_FaceID_array.push_back(a);

			state.lowerFaceArray.push_back(lowerFaceAtT0);
			state.lowerFaceArray.push_back(lowerFaceAtVa);
			state.lowerFaceArray.push_back(lowerFaceAtT);
		}
	} else {
		// (vb, vc) 가 반대방향에 있다.
		D3DXVECTOR3	vm = .5f * (vb + vc);
		LOWER_FACE	lowerFaceAtVm;
		lowerFaceAtVm.p = .5f * (lowerFaceAtVb.p + lowerFaceAtVc.p);
		lowerFaceAtVm.n = .5f * (lowerFaceAtVb.n + lowerFaceAtVc.n);
		lowerFaceAtVm.tu = .5f * (lowerFaceAtVb.tu + lowerFaceAtVc.tu);
		lowerFaceAtVm.tv = .5f * (lowerFaceAtVb.tv + lowerFaceAtVc.tv);
		lowerFaceAtVm.c = .5f * (lowerFaceAtVb.c + lowerFaceAtVc.c);
		if (clockwise) {	// (a, b, vm), (b, vb, vm), (vc, a, vm)
			// (a, b, vm)
			state.lower2_FaceID_array.push_back(a);
			state.lower2_FaceID_array.push_back(b);
			state.lower2_FaceID_array.push_back(vm);

			state.lowerFaceArray.push_back(lowerFaceAtT);
			state.lowerFaceArray.push_back(lowerFaceAtT0);
			state.lowerFaceArray.push_back(lowerFaceAtVm);
			// (b, vb, vm)
			state.lower2_FaceID_array.push_back(b);
			state.lower2_FaceID_array.push_back(vb);
			state.lower2_FaceID_array.push_back(vm);

			state.lowerFaceArray.push_back(lowerFaceAtT0);
			state.lowerFaceArray.push_back(lowerFaceAtVb);
			state.lowerFaceArray.push_back(lowerFaceAtVm);
			// (vc, a, vm)
			state.lower2_FaceID_array.push_back(vc);
			state.lower2_FaceID_array.push_back(a);
			state.lower2_FaceID_array.push_back(vm);

			state.lowerFaceArray.push_back(lowerFaceAtVc);
			state.lowerFaceArray.push_back(lowerFaceAtT);
			state.lowerFaceArray.push_back(lowerFaceAtVm);
		} else {		// (b, a, vm), (a, vb, vm), (vc, b, vm)
			// (b, a, vm)
			state.lower2_FaceID_array.push_back(b);
			state.lower2_FaceID_array.push_back(a);
			state.lower2_FaceID_array.push_back(vm);

			state.lowerFaceArray.push_back(lowerFaceAtT0);
			state.lowerFaceArray.push_back(lowerFaceAtT);
			state.lowerFaceArray.push_back(lowerFaceAtVm);
			// (a, vb, vm)
			state.lower2_FaceID_array.push_back(a);
			state.lower2_FaceID_array.push_back(vb);
			state.lower2_FaceID_array.push_back(vm);

			state.lowerFaceArray.push_back(lowerFaceAtT);
			state.lowerFaceArray.push_back(lowerFaceAtVb);
			state.lowerFaceArray.push_back(lowerFaceAtVm);
			// (vc, b, vm)
			state.lower2_FaceID_array.push_back(vc);
			state.lower2_FaceID_array.push_back(b);
			state.lower2_FaceID_array.push_back(vm);

			state.lowerFaceArray.push_back(lowerFaceAtVc);
			state.lowerFaceArray.push_back(lowerFaceAtT0);
			state.lowerFaceArray.push_back(lowerFaceAtVm);
		}
	}
}
#endif	// ShowLower2Face

	// 이 Edge 와 만난다.
	t = t0;
#if 2	// ShowLower2Face
	FillLowerFace(edge.prStartID, edge.prEndID, t, pointReprArray, vertexBufferArray, lowerFaceAtT);
#endif	// ShowLower2Face

	if (faceWalkHistoryArray[faceID]) {
		bAllDone = true;
		break;
	}
	faceWalkHistoryArray[faceID] = 1;

#if 1	// 0부터...
	O = A + t * AB;
#if 1	// 뼈
	O0 = A0 + t * AB0;
#endif	// 뼈
	const SEGMENT_VERTEX&	end = segmentPointArray[segmentPointArray.GetSize() - 1];
#if 1	// 임의의 두 점 사이의 거리
	if (__P2Ptr) {
		float	s, distance;
		PointLineSegmentIntersection(end.P, O, __P2Ptr->P, s, distance);
		if (s >= 0.f && s <= 1.f) {
			if (isPointOnLineSegment(meshPtr, distance)) {
				D3DXVECTOR3	PA = end.P - __P2Ptr->P;
				D3DXVECTOR3	PB = O - __P2Ptr->P;
				D3DXVECTOR3	AB = O - end.P;
				dA2 = D3DXVec3Length(&PA);
				float	dB2 = D3DXVec3Length(&PB);
				(void) fprintf(stdout,
					"s = %f, d = %f (%f): (%f + %f = %f(? %f))\n"
					, s,
					distance, distance / meshPtr->a(),
					dA2, dB2, dA2 + dB2, D3DXVec3Length(&AB)
				);
#if 1	// 뼈
				D3DXVECTOR3	PA0 = end.P0 - __P2Ptr->P0;
				D3DXVECTOR3	PB0 = O0 - __P2Ptr->P0;
				D3DXVECTOR3	AB0 = O0 - end.P0;
				dA20 = D3DXVec3Length(&PA0);
				float	dB20 = D3DXVec3Length(&PB0);
				(void) fprintf(stdout,
					"s = %f, d = %f (%f): (%f + %f = %f(? %f))\n"
					, s,
					distance, distance / meshPtr->a(),
					dA20, dB20, dA20 + dB20, D3DXVec3Length(&AB0)
				);
#endif	// 뼈
				const SEGMENT_VERTEX&	v1 = end;
				const SEGMENT_VERTEX&	v2 = *__P2Ptr;
				(void) fprintf(stdout,
					"len(P1,P2) = %f\n"
					"P1: [%f, %f, %f] (%d, %d, %d)\n"
					"P2: [%f, %f, %f] (%d, %d, %d)\n"
					,
					dA2,
					v1.P.x, v1.P.y, v1.P.z, v1.EdgeID, v1.VertexID, v1.FaceID,
					v2.P.x, v2.P.y, v2.P.z, v2.EdgeID, v2.VertexID, v2.FaceID
				);

				n2_found = true;
#if 1	// clockwise
				bAllDone = true;
				break;
#endif
			}
		}
	}
#endif
	float	len = D3DXVec3Length(&D3DXVECTOR3(O - end.P));
	lengthArray.push_back(len);
	length += len;
#if 1	// 뼈
	float	len0 = D3DXVec3Length(&D3DXVECTOR3(O0 - end.P0));
	lengthArray0.push_back(len0);
	length0 += len0;
#endif	// 뼈
	SEGMENT_VERTEX	v;
	v.P = O;
#if 1	// 뼈
	v.P0 = O0;
#endif	// !뼈
	const CPointRepr::VERTEX&	startV = pointReprArray[edge.prStartID].v;
	const CPointRepr::VERTEX&	endV = pointReprArray[edge.prEndID].v;
	v.N = startV.normal + t * (endV.normal - startV.normal);
#if 1	// SEGMENT_VERTEX::{EdgeID,VertexID,FaceID}
	// 바로 아래의 lastEdgeID, lastVertexID 의 계산과 동일
	if (t == 0.f || t == 1.f) {
		v.EdgeID = (DWORD) -1;
		v.VertexID = t == 0.f ? edge.prStartID : edge.prEndID;
	} else {
		v.EdgeID = edgeID;
		v.VertexID = (DWORD) -1;
	}
	v.FaceID = (DWORD) -1;
#endif	// SEGMENT_VERTEX::{EdgeID,VertexID,FaceID}
	segmentPointArray.Add(v);
#endif

	// 다음 Segment 를 만든다;
	// 1. 시작 위치가 Vertex 자체인지, 아니면 Edge 상의 한 점인지...
	lastVertexID = t == 1.f ? edge.prEndID : (t == 0.f ? edge.prStartID : (DWORD) -1);
	// 2. 시작 위치가 Edge 상의 한 점이라면 그 Edge 는...
	lastEdgeID = edgeID;
	if (t == 0.f || t == 1.f)
		lastEdgeID = (DWORD) -1;
	// 3. Segment 가 막 그려진 Face 는...
	lastFaceID = faceID;
#endif
////////////////////////////////////////////////////////////////////////////////

					bDone = true;
					break;
				}
				if (bDone || bAllDone)
					break;
			}
		}
		if (bAllDone) {	// 하나의 닫힌 String 을 만들었다.
			break;
		}
	}
	if (faceWalkHistoryArray) {
		delete[] faceWalkHistoryArray;
		faceWalkHistoryArray = NULL;
	}
#if 2	// ShowLower2Face
	meshPtr->GetVertexBufferPointer()->Unlock();
#endif	// ShowLower2Face

	int	numSegments = 0;	// 그려질 총 선분의 갯수는...
	if (segmentPointArray.GetSize()) {
		if (startDefPtr.type == START_DEF_PTR_TYPE__GIRTH) {
			// 둘레를 잴 때 닫힌 곡선의 경우, 끝점은 시작점과 같으므로 끝점을 제외한다.
			const SEGMENT_VERTEX&	end = segmentPointArray[segmentPointArray.GetSize() - 1];
			D3DXVECTOR3	AB = segmentPointArray[0].P - end.P;
			float	len = D3DXVec3Length(&AB);
{
			const SEGMENT_VERTEX&	v1 = segmentPointArray[0];
			const SEGMENT_VERTEX&	v2 = end;
			D3DXVECTOR3	p1 = v1.P;
			D3DXVECTOR3	p2 = v2.P;
			D3DXVECTOR3	v = p2 - p1;
			float	ell = D3DXVec3Length(&v);
			D3DXVec3Normalize(&p1, &p1);
			D3DXVec3Normalize(&p2, &p2);
			D3DXVECTOR3	v0 = p2 - p1;
			float	ell0 = D3DXVec3Length(&v0);
			(void) fprintf(stdout,
				"len(P1,P2) = %f (%f: less than FLT_EPSILON? %s)\n"
				"P1: [%f, %f, %f] (%d, %d, %d)\n"
				"P2: [%f, %f, %f] (%d, %d, %d)\n"
				,
				ell, ell0, ell0 < FLT_EPSILON ? "True" : "False",
				v1.P.x, v1.P.y, v1.P.z, v1.EdgeID, v1.VertexID, v1.FaceID,
				v2.P.x, v2.P.y, v2.P.z, v2.EdgeID, v2.VertexID, v2.FaceID
			);
}
			if (len / meshPtr->a() < DISTANCE_TOLERANCE) {
				closed = true;
				segmentPointArray.Remove(segmentPointArray.GetSize() - 1);
			} else
				closed = false;

			numSegments = segmentPointArray.GetSize();
			if (!closed)
				--numSegments;
		} else {
			closed = false;

			if (n2_found)
				numSegments = 1 + (segmentPointArray.GetSize() - 1) + 1;
			else {
				numSegments = -1;
				length = -1.f;
			}
		}
	} else {
		closed = false;
		numSegments = -1;
		length = -1.f;
	}
	(void) fprintf(stdout,
		"# of points = %d, # of length = %d, # of segments = %d\n"
		,
		segmentPointArray.GetSize(), lengthArray.size(), numSegments
	);
#if 1	// 뼈
	(void) fprintf(stdout,
		"# of points = %d, # of length0 = %d, # of segments = %d\n"
		,
		segmentPointArray.GetSize(), lengthArray0.size(), numSegments
	);
#endif	// 뼈

	if (numSegments) {
		if (lastPointInfoPtr) {
			lastPointInfoPtr->vertexID = lastVertexID;
			lastPointInfoPtr->edgeID = lastEdgeID;
			lastPointInfoPtr->t = t;
			lastPointInfoPtr->faceID = lastFaceID;
		}


#if 1	// 임의의 두 점 사이의 거리
		twoPointArray.RemoveAll();
		if (__P2Ptr) {
/*
 *	(P1) - [0] --- [1] --- [2] ... [N - 1] - (P2)		N = segmentPointArray 의 갯수
 *
 *	dB1 = len(P1, [0]), dA2 = len([N - 1], P2)
 *	dB1 의 여기서 결정, dA2 는 n2_found 에서 결정
 */
			D3DXVECTOR3	P1B = segmentPointArray[0].P - __P1Ptr->P;
			dB1 = D3DXVec3Length(&P1B);

			float	len = 0;
			for (int n = 0; n < (int) lengthArray.size(); ++n)
				len += lengthArray.at(n);
			length += dB1 + dA2;
#if 1	// 뼈
			D3DXVECTOR3	P1B0 = segmentPointArray[0].P0 - __P1Ptr->P0;
			dB10 = D3DXVec3Length(&P1B0);

			float	len0 = 0;
			for (int n = 0; n < (int) lengthArray0.size(); ++n)
				len0 += lengthArray0.at(n);
			length0 += dB10 + dA20;
#endif	// 뼈

#if 1
			twoPointArray.Add(*__P1Ptr);
#else
			twoPointArray.push_back(*__P1Ptr);
#endif
			for (int n = 0; n < segmentPointArray.GetSize(); ++n)
				twoPointArray.Add(segmentPointArray[n]);
			assert(segmentPointArray.GetSize() > 0);
			const SEGMENT_VERTEX&	v1 = segmentPointArray[segmentPointArray.GetSize() - 1];
			const SEGMENT_VERTEX&	v2 = *__P2Ptr;
			D3DXVECTOR3	p1 = v1.P;
			D3DXVECTOR3	p2 = v2.P;
			D3DXVECTOR3	v = p2 - p1;
			float	ell = D3DXVec3Length(&v);
			D3DXVec3Normalize(&p1, &p1);
			D3DXVec3Normalize(&p2, &p2);
			D3DXVECTOR3	v0 = p2 - p1;
			float	ell0 = D3DXVec3Length(&v0);
			(void) fprintf(stdout,
				"len(P1,P2) = %f (%f: less than FLT_EPSILON? %s)\n"
				"P1: [%f, %f, %f] (%d, %d, %d)\n"
				"P2: [%f, %f, %f] (%d, %d, %d)\n"
				,
				ell, ell0, ell0 < FLT_EPSILON ? "True" : "False",
				v1.P.x, v1.P.y, v1.P.z, v1.EdgeID, v1.VertexID, v1.FaceID,
				v2.P.x, v2.P.y, v2.P.z, v2.EdgeID, v2.VertexID, v2.FaceID
			);
#if 1	// 끝점이 실제로는 같은 점일 수 있다.
			if (v1.EdgeID == v2.EdgeID && v1.VertexID == v2.VertexID && v1.FaceID == v2.FaceID) {
//				assert(ell0 < FLT_EPSILON);
				if (ell0 >= FLT_EPSILON)
					(void) fprintf(stdout, "ell0 >= FLT_EPSILON\n");
				--numSegments;
			} else {
				twoPointArray.Add(*__P2Ptr);
			}
#else
			twoPointArray.Add(*__P2Ptr);
#endif
			(void) fprintf(stdout, "dB1 = %f, len = %f, dA2 = %f; length = %f (%f)\n", dB1, len, dA2, length, dB1 + len + dA2);
#if 1	// 뼈
			(void) fprintf(stdout, "dB10 = %f, len0 = %f, dA20 = %f; length0 = %f (%f)\n", dB10, len0, dA20, length0, dB10 + len0 + dA20);
#endif	// 뼈
		}
#endif

#if 1	// Convex
		// N 개의 점들을 둘러싸는(wrapping) 점들을 구한다.
		convexPointArray.RemoveAll();
		if (mode == TAPE_MEASURE_MODE_WRAP) {
#if 1	// 뼈
			kTapeMeasure::wrapping(closed, nDir, __P2Ptr ? twoPointArray : segmentPointArray, convexPointArray, length0, length);
#else	// 뼈/!뼈
			kTapeMeasure::wrapping(closed, nDir, __P2Ptr ? twoPointArray : segmentPointArray, convexPointArray, length);
#endif	// !뼈
			numSegments = convexPointArray.GetSize();
			if (!closed)
				--numSegments;
		}
#endif
	}	// numSegments > 0
////////////////////////////////////////////////////////////////////////
#if 1	// 마지막 주문; 5. 측정선 선명하게...
	// 선을 이루는 각 점을, 각 점에서의 노말방향으로 kState::SHIFT_ALONG_NORMAL 만큼 이동
{
if (closed) {	// (평면 상에 있는) 닫힌 곡선을 이루는 각 점을,
       		// 각 점들의 중심(centroid)로부터 좀 더 멀어지게 한다.
	// 1. segmentPointArray
	unsigned	numPoints = segmentPointArray.GetSize();
	D3DXVECTOR3	centroid(0, 0, 0);
	for (size_t n = 0; n < numPoints; ++n) {
		SEGMENT_VERTEX&	v = segmentPointArray[n];
		centroid += v.P;
	}
	centroid /= (float) numPoints;
	cout << "centroid: [" << centroid.x << ", " << centroid.y << ", " << centroid.z << "]" << endl;
	for (int n = 0; n < segmentPointArray.GetSize(); ++n) {
		SEGMENT_VERTEX&	v = segmentPointArray[n];
		D3DXVECTOR3&	P = v.P;
		P += meshPtr->shift_amount_along_normal() * v.N;
	}
	// 2. convexPointArray
if (mode == TAPE_MEASURE_MODE_WRAP) {
	for (int n = 0; n < convexPointArray.GetSize(); ++n) {
		SEGMENT_VERTEX&	v = convexPointArray[n];
		D3DXVECTOR3&	P = v.P;
		P += meshPtr->shift_amount_along_normal() * v.N;
	}
}
} else {
	// 1. twoPointArray
if (twoPointArray.GetSize() > 2) {
	for (int n = 1; n < twoPointArray.GetSize() - 1; ++n) {
		SEGMENT_VERTEX&	v = twoPointArray[n];
		D3DXVECTOR3&	P = v.P;
		P += meshPtr->shift_amount_along_normal() * v.N;
	}
}
	// 2. convexPointArray
if (mode == TAPE_MEASURE_MODE_WRAP && convexPointArray.GetSize() > 3) {
	for (int n = 1; n < convexPointArray.GetSize() - 1; ++n) {
		SEGMENT_VERTEX&	v = convexPointArray[n];
		D3DXVECTOR3&	P = v.P;
		P += meshPtr->shift_amount_along_normal() * v.N;
	}
}
}
}
#endif	// 마지막 주문; 5. 측정선 선명하게...

#if 1	// 뼈
	cout << "length = " << length << ", length0 = " << length0 << endl;
	length = length0;
#endif	// 뼈
	if (meshPtr->__hWnd) {
		cout << "sizeof(WPARAM): " << sizeof(WPARAM) << endl;
		cout << "sizeof(LPARAM): " << sizeof(LPARAM) << endl;
		cout << "PostMessage: " << (
			PostMessage(
				meshPtr->__hWnd,
				WM_USER__LENGTH,
				(WPARAM) (startDefPtr.type == START_DEF_PTR_TYPE__GIRTH ? 0 : 1),
				(LPARAM) *((int*) &length)
			) ? "Ok" : "Oops"
		) << endl;
	}
	return numSegments;
}
#endif

#if 1	// 둘레 표시
int	kTapeMeasure::getValue(int subType, void* dataPtr)
{
	if (!dataPtr) {
		(void) fprintf(stderr, "Fatal: kTapeMeasure::getValue: dataPtr is nil\n");
		return -1;
	}

	switch (subType) {
#if 0
	case TAPE_MEASURE__HEAD:
	case TAPE_MEASURE__NECK:
	case TAPE_MEASURE__BREAST:
	case TAPE_MEASURE__WAIST:
	case TAPE_MEASURE__HIP:
	case TAPE_MEASURE__LEFT_THIGH:
	case TAPE_MEASURE__RIGHT_THIGH:
	case TAPE_MEASURE__LEFT_FOREARM:
	case TAPE_MEASURE__RIGHT_FOREARM:
	case TAPE_MEASURE__HEIGHT	:
	case TAPE_MEASURE__SEATED_HEIGHT:
	case TAPE_MEASURE__SHOULDER_WIDTH:
	case TAPE_MEASURE__LEFT_LEG:
	case TAPE_MEASURE__RIGHT_LEG:
	case TAPE_MEASURE__LEFT_ARM:
	case TAPE_MEASURE__RIGHT_ARM:
	case TAPE_MEASURE__LEFT_ARM_INSIDE:
	case TAPE_MEASURE__LEFT_ARM_OUTSIDE:
	case TAPE_MEASURE__RIGHT_ARM_INSIDE:
	case TAPE_MEASURE__RIGHT_ARM_OUTSIDE:
	case TAPE_MEASURE__LEFT_LEG_INSIDE:
	case TAPE_MEASURE__LEFT_LEG_OUTSIDE:
	case TAPE_MEASURE__RIGHT_LEG_INSIDE:
	case TAPE_MEASURE__RIGHT_LEG_OUTSIDE:
		*((bool*) dataPtr) = __val & subType ? true : false;
		return 0;
		break;
#endif
	case TAPE_MEASURE__TYPE:
	{
		TAPE_MEASURE_TYPE_VISIBLE&	typeVisible = *((TAPE_MEASURE_TYPE_VISIBLE*) dataPtr);
		typeVisible.visible = __val & typeVisible.type ? true : false;
		return 0;
	}
		break;
	case TAPE_MEASURE__MODE:
		*((TAPE_MEASURE_MODE_T*) dataPtr) = __mode;
		return 0;
		break;
	case TAPE_MEASURE__METHOD:
		*((TAPE_MEASURE_METHOD_T*) dataPtr) = __method;
		return 0;
		break;
	case TAPE_MEASURE__CROSS_SECTION:
		*((TAPE_MEASURE_CROSS_SECTION_T*) dataPtr) = __crossSectionType;
		return 0;
		break;

	case TAPE_MEASURE__USER:
	{
		USER_DEFINE_AREA&	this_uda = *((USER_DEFINE_AREA*) dataPtr);
		const char*	name_ptr = this_uda.name.c_str();
		if (!name_ptr) {
			(void) fprintf(stderr, "ERROR: name NOT given\n");
			return -1;
		}
		for (int n = 0; n < (int) __userDefinedArea.size(); ++n) {
			USER_DEFINE_AREA&	uda = __userDefinedArea.at(n);
			if (!uda.name.c_str()) {
				(void) fprintf(stderr, "FATAL: name is nil\n");
				return -1;
			}
			if (strcmp(name_ptr, uda.name.c_str()) == 0) {	// FOUND
				this_uda = uda;
				return 0;
			}
		}
		(void) fprintf(stderr, "ERROR: name:= |%s| NOT found\n", name_ptr);
		return -1;
	}
		break;
	case TAPE_MEASURE__NAME_VISIBLE:
	{
		TAPE_MEASURE_NAME_VISIBLE&	nameVisible = *((TAPE_MEASURE_NAME_VISIBLE*) dataPtr);
		// nameVisible.name 에 해당하는 이름이 있는 지,
		// 있다면 visible 값을 무엇인지 알아낸다.
		if (!nameVisible.name) {
			(void) fprintf(stderr, "ERROR: name is nil\n");
			return -1;
		}
		for (int n = 0; n < (int) __userDefinedArea.size(); ++n) {
			const USER_DEFINE_AREA&	uda = __userDefinedArea.at(n);
			if (!uda.name.c_str()) {
				(void) fprintf(stderr, "FATAL: name is nil\n");
				return -1;
			}
			if (strcmp(nameVisible.name, uda.name.c_str()) == 0) {	// FOUND
				nameVisible.visible = uda.visible;
				return 0;
			}
		}
		(void) fprintf(stderr, "ERROR: name:= |%s| not found\n", nameVisible.name);
		return -1;	// NOT FOUND
	}

	default:
		(void) fprintf(stderr, "Error: kTapeMeasure::getValue: TAPE_MEASURE__TYPE = %d: Unknown Type\n", subType);
		break;
	}
	return -1;
}
int	kTapeMeasure::setValue(int subType, const void* dataPtr)
{
	if (!dataPtr) {
		(void) fprintf(stderr, "Fatal: kTapeMeasure::getValue: dataPtr is nil\n");
		return -1;
	}

	switch (subType) {
#if 0	// TAPE_MEASURE__TYPE
	case TAPE_MEASURE__HEAD:
	case TAPE_MEASURE__NECK:
	case TAPE_MEASURE__BREAST:
	case TAPE_MEASURE__WAIST:
	case TAPE_MEASURE__HIP:
	case TAPE_MEASURE__LEFT_THIGH:
	case TAPE_MEASURE__RIGHT_THIGH:
	case TAPE_MEASURE__LEFT_FOREARM:
	case TAPE_MEASURE__RIGHT_FOREARM:
	case TAPE_MEASURE__HEIGHT	:
	case TAPE_MEASURE__SEATED_HEIGHT:
	case TAPE_MEASURE__SHOULDER_WIDTH:
	case TAPE_MEASURE__LEFT_LEG:
	case TAPE_MEASURE__RIGHT_LEG:
	case TAPE_MEASURE__LEFT_ARM:
	case TAPE_MEASURE__RIGHT_ARM:
	case TAPE_MEASURE__LEFT_ARM_INSIDE:
	case TAPE_MEASURE__LEFT_ARM_OUTSIDE:
	case TAPE_MEASURE__RIGHT_ARM_INSIDE:
	case TAPE_MEASURE__RIGHT_ARM_OUTSIDE:
	case TAPE_MEASURE__LEFT_LEG_INSIDE:
	case TAPE_MEASURE__LEFT_LEG_OUTSIDE:
	case TAPE_MEASURE__RIGHT_LEG_INSIDE:
	case TAPE_MEASURE__RIGHT_LEG_OUTSIDE:
		if (*((bool*) dataPtr))
			__val |= subType;
		else
			__val &= ~subType;
		return 0;
#endif
	case TAPE_MEASURE__TYPE:
	{
		TAPE_MEASURE_TYPE_VISIBLE&	typeVisible = *((TAPE_MEASURE_TYPE_VISIBLE*) dataPtr);
		if (typeVisible.visible)
			__val |= typeVisible.type;
		else
			__val &= ~typeVisible.type;
		return 0;
	}
		break;

	case TAPE_MEASURE__MODE:
		__mode = *((TAPE_MEASURE_MODE_T*) dataPtr);
		__convexPointArray.RemoveAll();
		return 0;
		break;
	case TAPE_MEASURE__METHOD:
		__method = *((TAPE_MEASURE_METHOD_T*) dataPtr);
		__meshPtr->ResetTapeMeasure();
		return 0;
		break;
	case TAPE_MEASURE__CROSS_SECTION:
		__crossSectionType = *((TAPE_MEASURE_CROSS_SECTION_T*) dataPtr);
		return 0;
		break;

	case TAPE_MEASURE__NAME:
	{
		const TAPE_MEASURE_NAME_T&	nameType = *((TAPE_MEASURE_NAME_T*) dataPtr);
		// nameType.old_name 이 NULL 이 아니라면,
		// 해당 이름이 있어야 한다.
		int	old_index = -1;
		if (nameType.old_name) {
			for (int n = 0; n < (int) __userDefinedArea.size(); ++n) {
				USER_DEFINE_AREA&	uda = __userDefinedArea.at(n);
				if (!uda.name.c_str()) {
					(void) fprintf(stderr, "FATAL: name is nil\n");
					return -1;
				}
				if (strcmp(nameType.old_name, uda.name.c_str()) == 0) {	// FOUND
					old_index = n;
					break;
				}
			}
			if (old_index == -1) {
				(void) fprintf(stderr, "ERROR: old_name:= |%s|: NOT found\n", nameType.old_name);
				return -1;
			}
		}
		// nameType.new 이 NULL 이 아니라면
		// 해당 이름은 없어야 한다.
		if (nameType.new_name) {
			for (int n = 0; n < (int) __userDefinedArea.size(); ++n) {
				USER_DEFINE_AREA&	uda = __userDefinedArea.at(n);
				if (!uda.name.c_str()) {
					(void) fprintf(stderr, "FATAL: name is nil\n");
					return -1;
				}
				if (strcmp(nameType.new_name, uda.name.c_str()) == 0) {	// FOUND
					(void) fprintf(stderr, "ERROR: new_name:= |%s|: alread exist\n", nameType.new_name);
					return -1;
				}
			}
		}

		// 세 가지 가능성이 있다:
		//	1. old_name = NULL, new_name != NULL
		//		new_name 을 새로 등록
		//	2. old_name != NULL, new_name != NULL
		//		이름을 old_name 에서 new_name 으로 변경
		//	3. old_name != NULL, new_name = NULL
		//		old_name 을 삭제
		if (!nameType.old_name && !nameType.new_name) {
			(void) fprintf(stderr, "ERROR: old_name is nil and new_name is nil\n");
			return -1;
		}
		if (!nameType.old_name && nameType.new_name) {	// 새로 등록
			if (__length < 0.f) {
				(void) fprintf(stderr, "ERROR: no measure\n");
				return -1;
			}
			USER_DEFINE_AREA	uda;
			uda.name = nameType.new_name;
			uda.mode = __mode;
			uda.method = __method;
			uda.crossSectionType = __crossSectionType;
			uda.length = __length;
			uda.visible = false;

			__userDefinedArea.push_back(uda);
		}
		if (nameType.old_name && nameType.new_name) {	// 이름 변경
			__userDefinedArea.at(old_index).name = nameType.new_name;
		}
		if (nameType.old_name && !nameType.new_name) {	// 이름 삭제
			__userDefinedArea.erase(__userDefinedArea.begin() + old_index);
		}
		return 0;
	}
		break;
	case TAPE_MEASURE__NAME_VISIBLE:
	{
		const TAPE_MEASURE_NAME_VISIBLE&	nameVisible = *((TAPE_MEASURE_NAME_VISIBLE*) dataPtr);
		// nameVisible.name 에 해당하는 이름이 있는 지,
		// 있다면 visible 값을 설정한다.
		if (!nameVisible.name) {
			(void) fprintf(stderr, "ERROR: name is nil\n");
			return -1;
		}
		for (int n = 0; n < (int) __userDefinedArea.size(); ++n) {
			USER_DEFINE_AREA&	uda = __userDefinedArea.at(n);
			if (!uda.name.c_str()) {
				(void) fprintf(stderr, "FATAL: name is nil\n");
				return -1;
			}
			if (strcmp(nameVisible.name, uda.name.c_str()) == 0) {	// FOUND
				uda.visible = nameVisible.visible;
				return 0;
			}
		}
		(void) fprintf(stderr, "ERROR: name:= |%s| not found\n", nameVisible.name);
		return -1;	// NOT FOUND
	}

	default:
		(void) fprintf(stderr, "Error: kTapeMeasure::setValue: TAPE_MEASURE__TYPE = %d: Unknown Type\n", subType);
		break;
	}
	return -1;
}
#endif

#if 1	// 임의의 두 점 사이의 거리
float	kTapeMeasure::DISTANCE_TOLERANCE = 1.e-4f;
bool	kTapeMeasure::isPointOnLineSegment(float distance)
{
	return distance / __meshPtr->a() < DISTANCE_TOLERANCE;
}
bool	kTapeMeasure::isPointOnLineSegment(const kMesh* meshPtr, float distance)
{
	return distance / meshPtr->a() < DISTANCE_TOLERANCE;
}
// 서로 다른 두 점 A, B 를 잇는 선분(line segment)과 한 점 P 가 있을 때
// P 에서 선분또는 선분의 연장선에 이르는 최단 거리(1)와
// 만나는 점을 결정하는 t(H = A + tAB)를 구한다.
//	두 점 A, B 를 잇는 선분을 A에서 B로 가는 벡터(AB = B - A)로 생각한다.
//	1. 임의의 한 점 H 는 H = A + tAB 로 쓸 수 있다;
//		t < 0:= H 가 A 보다 앞에 있다.
//		t > 1:= H 가 B 보다 뒤에 있다.
//		0 <= t <= 1:= H 가 A, B 사이에 있다. (t = 0 이면 H = A, t = 1 이면 H = B)
//	2. 점 P 에서 H 에 이르는 거리는
//	벡터 PH = H - P = PA + tAB (PA = A - P) 의 길이이다: |PH|
//	t 는 두 벡터 AB 와 PH 가 수직이라는 조건으로부터 구한다.
//		AB.PH = AB.(PA + tAB) = AB.PA + t |AB|^2 = 0
//		t = -AB.PA / |AB|^2
//
// 리턴값:
//	-1: A = B (예외적인 상황)
//		1) t = 0
//		2) distance = |PA|
//	0:
//		1) 수선의 발 H 를 결정하는 즉 H = A + tAB 에서 t
//			t < 0: A 보다 뒤에 있다
//			t > 1: B 보다 앞에 있다
//			0 <= t <= 1: A, B 사이에 있다 (t = 0: H = A, t = 1: H = B)
//		2) distance = |PH|
int	kTapeMeasure::PointLineSegmentIntersection(
	// [IN]
	const D3DXVECTOR3&	A,
	const D3DXVECTOR3&	B,
	const D3DXVECTOR3&	P,
	
	// [OUT]
	float&	t,
	float& distance
)
{
	D3DXVECTOR3	AB = B - A;
	D3DXVECTOR3	PA = A - P;
	float	len2 = D3DXVec3LengthSq(&AB);
	if (fabsf(len2) < FLT_EPSILON) {	// 두 점이 일치한다; 선분이 아니다
		(void) fprintf(stderr, "WARN: kTapeMeasure::PointLineSegmentIntersection: A = B\n");
		t = 0.f;	// 어차피 이 값은 사용되지 않는다;
				// 다만 이 값은 [0, 1] 사이의 값으로 만드는 이유는,
				// 이 경우에도 distance 를 따져볼 여지를 주기 위함이다.
				// (이 함수를 호출한 다음을 참고한다.)
		distance = D3DXVec3Length(&PA);
		return -1;
	}
	t = -D3DXVec3Dot(&AB, &PA) / len2;
	D3DXVECTOR3	PH = PA + t * AB;
	distance = D3DXVec3Length(&PH);
	return 0;
}

void	kTapeMeasure::SetTwoPointBuffer()
{
	size_t	numPoints = __twoPointArray.GetSize();
	if (!numPoints)
		return;

	if (__twoPointVertexBufferPtr) {
		__twoPointVertexBufferPtr->Release();
		__twoPointVertexBufferPtr = 0;
	}
	__devicePtr->CreateVertexBuffer(
		(UINT) numPoints * sizeof(SEGMENT_VERTEX),
		D3DUSAGE_WRITEONLY,
		0,
		D3DPOOL_MANAGED,
		&__twoPointVertexBufferPtr,
		NULL
	);
	SEGMENT_VERTEX*	vertexArrayPtr;
	__twoPointVertexBufferPtr->Lock(0, 0, (void**) &vertexArrayPtr, 0);
	CopyMemory(vertexArrayPtr, __twoPointArray.GetData(), numPoints * sizeof(SEGMENT_VERTEX));
	__twoPointVertexBufferPtr->Unlock();

	if (__twoPointIndexBufferPtr) {
		__twoPointIndexBufferPtr->Release();
		__twoPointIndexBufferPtr = 0;
	}
	int	last_index = __twoPointArray.GetSize() - 1;
	__devicePtr->CreateIndexBuffer(
		(UINT) last_index * sizeof(CEdge::LineIndex),
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX32,
		D3DPOOL_MANAGED,
		&__twoPointIndexBufferPtr,
		NULL
	);
	CEdge::LineIndex*	indexArrayPtr;
	__twoPointIndexBufferPtr->Lock(0, 0, (void**) &indexArrayPtr, 0);
	for (int n = 0; n < last_index; ++n) {
		indexArrayPtr[n].iStart = n;
		indexArrayPtr[n].iEnd = n + 1;
	}
	__twoPointIndexBufferPtr->Unlock();
}
#endif

#if 1	// Convex
void	kTapeMeasure::SetConvexBuffer(bool closed)
{
	size_t	numPoints = __convexPointArray.GetSize();
	if (!numPoints)
		return;

	if (__convexVertexBufferPtr) {
		__convexVertexBufferPtr->Release();
		__convexVertexBufferPtr = 0;
	}
	__devicePtr->CreateVertexBuffer(
		(UINT) numPoints * sizeof(SEGMENT_VERTEX),
		D3DUSAGE_WRITEONLY,
		0,
		D3DPOOL_MANAGED,
		&__convexVertexBufferPtr,
		NULL
	);
	SEGMENT_VERTEX*	vertexArrayPtr;
	__convexVertexBufferPtr->Lock(0, 0, (void**) &vertexArrayPtr, 0);
	CopyMemory(vertexArrayPtr, __convexPointArray.GetData(), numPoints * sizeof(SEGMENT_VERTEX));
	__convexVertexBufferPtr->Unlock();

	if (__convexIndexBufferPtr) {
		__convexIndexBufferPtr->Release();
		__convexIndexBufferPtr = 0;
	}
	int	last_index = __convexPointArray.GetSize() - 1;
	__devicePtr->CreateIndexBuffer(
		(closed ? last_index + 1 : last_index) * sizeof(CEdge::LineIndex),
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX32,
		D3DPOOL_MANAGED,
		&__convexIndexBufferPtr,
		NULL
	);
	CEdge::LineIndex*	indexArrayPtr;
	__convexIndexBufferPtr->Lock(0, 0, (void**) &indexArrayPtr, 0);
	for (int n = 0; n < last_index; ++n) {
		indexArrayPtr[n].iStart = n;
		indexArrayPtr[n].iEnd = n + 1;
	}
	if (closed) {
		indexArrayPtr[last_index].iStart = last_index;
		indexArrayPtr[last_index].iEnd = 0;
	}
	__convexIndexBufferPtr->Unlock();
}

// 주어진 점들의 배열과 이 점들이 나열된 순서를 말해주는 법선벡터를 사용하여
// 점들을 둘러싸는 점들을 얻는다:
//	법선벡터의 끝점에서 시작점을 봤을 때, 주어진 점들은 반시계방향으로 차례대로 나열되었다;
//	점들을 오른손의 엄지손가락을 제외한 네 손가락으로 차례대로 감쌌을 때
//	엄지손가락이 가리키는 방향이 nDir 이다.
//
//#define _DEBUG2
int	kTapeMeasure::wrapping(
	// [IN]
	bool	closed,	// inPointArray 에 있는 점들이 닫힌 곡선을 만드는 지 아닌지
	const D3DXVECTOR3&	nDir,
	const CGrowableArray<SEGMENT_VERTEX>&	inPointArray,

	// [OUT]
	CGrowableArray<SEGMENT_VERTEX>&	outPointArray,
#if 1	// 뼈
	float&	wrapping_length0,	// outPointArray 에 있는 점들을 이은 길이의 합
#endif	// 뼈
	float&	wrapping_length		// outPointArray 에 있는 점들을 이은 길이의 합
)
{
#ifdef _DEBUG
(void) fprintf(stdout, "kTapeMeasure::wrapping: finding wrapping contour...\n");
#endif
	if (inPointArray.GetSize() == 0)
		return 0;

	outPointArray.RemoveAll();	// N 개의 점들로 이루어진 배열
	std::vector<int>	indexArray;
				// inPointArray 배열을 참조하는 인덱스 배열
				// 닫힌 곡선:
				//	__n0, __n0 + 1, ..., M0, 0, 1, ..., __n0 - 1, __n0
				// 열린 곡선:
				//	0, 1, ..., M0
				// 여기서 M0 은 inPointArray 배열의 마지막 인덱스이고,
				// __n0 은 닫힌 곡선의 시작 인덱스(아래 구하는 과정 참고)이다.
				// 주) 닫힌 곡선의 경우 시작 인덱스와 끝 인덱스가 같다: __n0

#if 0
	int	n0 = -1;	// indexArray 의 시작 위치이면서
				// 동시에 복록 다각형의 꼭지점을 말해주는 인덱스;
				// n0 = 0 부터 시작해서 아래 알고리즘에 따라 M - 1 에서 멈춘다.
				// 여기서 M 은 indexArray.size() - 1 즉 indexArray 배열의 마지막 인덱스이다.
				// 주) 아래 알고리즘에 따라 다음 n0 를 찾는 도중에
				// n0 = M 이 될 수도 있다; 바로 멈춘다.
#endif
/*
 * 알고리즘
 *	n0 번째 점에서 시작해서 n0 + 1 번째 점에서 끝나는 길이가 1인 벡터(V1)를 기준으로,
 *	n0 번째 점에서 시작해서 n(n = n0 + 2, n0 + 3, ..., M) 번째 점으로 끝나는 길이가 1인 벡터(VN)들이
 *	상대적으로 어떤 위치에 있는 지를 바탕으로 다음 n0 를 구한다;
 *	점들은 nDir 기준으로 반시계방향으로 만들어진다. (이 함수를 호출하기 전에 findEdge, setBasePoint 함수에서)
 *	다음 값 f 에 따라 어느 점이 다음 n0 이 될지를 결정한다:
 *		f = nDir . (V1 x VN) = VN . (nDir x V1)
 *	f < 0 이면 VN 벡터는 V1 벡터보다 안쪽에 있고, f > 0 라면 바깥쪽에 있다.
 *	따라서 제일 큰 양수의 f 가 나오는 벡터가 있다면 해당 하는 점이 n0 가 된다.
 *	모든 f 가 음수라면 n0 + 1 이 다음 n0 가 된다.
 *
 *	주) 여기서 f 는 n 번째 점에서 V0 = nDir x V1 에 의해서 결정되는 평면에 이르는 부호 거리와 같다.
 *	이는 V0 x nDir 에 의해서 결정되는 평면에 이르는 부호거리(g)가 양수일 때에 한해서 이렇다.
 *	만약 f 가 양수이면서 동시에 d 가 음수라면 비록 작은 f 라 할지라도 더 바깥쪽에 있다.
 *		g = VN . (V0 x nDir) = VN . ((nDir x V1) x nDir)
 *		= + VN . (nDir x (V1 x nDir)) = VN . (V1 - (nDir . V1) nDir)
 *		= V1 . VN - (nDir . VN )(nDir . V1)
 */
do {
	// indexArray 를 설정한다.
	int	__n0 = -1;	// indexArray 배열의 첫 번째 값
	if (!closed) {	// 열린 곡선
		__n0 = 0;
		for (int n = 0; n < (int) inPointArray.GetSize(); ++n)
			indexArray.push_back(n);
	} else {	// 닫힌 곡선
		// __n0 를 구한다;
		//	중심에서 시작점(P)을 향하는 벡터에 대해 투영된 좌표의 값이 제일 큰 점

		D3DXVECTOR3	center = D3DXVECTOR3(0, 0, 0);	// 중심
		for (int n = 0; n < (int) inPointArray.GetSize(); ++n)
			center += inPointArray[n].P;
		center /= (float) inPointArray.GetSize();

		D3DXVECTOR3	axis = inPointArray[0].P - center;		// 기준축
		__n0 = -1;
		float	vx0 = -FLT_MAX;
		for (int n = 0; n < (int) inPointArray.GetSize(); ++n) {
			D3DXVECTOR3	v = inPointArray[n].P - center;
			float	vx = D3DXVec3Dot(&axis, &v);
#ifdef _DEBUG2
			(void) fprintf(stdout, "\t%f\n", D3DXVec3Dot(&axis, &v));
#endif
			if (vx > vx0) {
				vx0 = vx;
				__n0 = n;
			}
		}
#ifdef _DEBUG2
		(void) fprintf(stdout, "\t__n0 = %d, vx0 = %f\n", __n0, vx0);
#endif

		for (int n = __n0; n < (int) inPointArray.GetSize(); ++n)	// __n0, __n0 + 1, ..., M0
			indexArray.push_back(n);
		for (int n = 0; n <= __n0; ++n)		// 0, 1, ..., __n0 - 1, __n0
			indexArray.push_back(n);
	}

	if (!indexArray.size()) {
		(void) fprintf(stdout, "No point(s)\n");
		break;
	}

	int	last_index = (int) indexArray.size() - 1;	// indexArray 배열의 마지막 인덱스
#ifdef _DEBUG2
for (int n = 0; n < last_index; ++n)
	(void) fprintf(stdout, "%d, ", indexArray.at(n));
(void) fprintf(stdout, "%d\n", indexArray.at(last_index));
#endif
	int	__index_n0 = 0;	// 시작점에 해당하는 indexArray 배열에 대한 인덱스
	while (1) {
		outPointArray.Add(inPointArray[indexArray.at(__index_n0)]);
		if (__index_n0 >= last_index - 1) {
			if (!closed && __index_n0 != last_index) {	// 열린 곡선이면
				// 마지막 점도 볼록다각형의 한 점이 된다.
				outPointArray.Add(inPointArray[indexArray.at(last_index)]);
			}
#ifdef _DEBUG2
for (int n = 0; n < last_index; ++n)
	(void) fprintf(stdout, "%d, ", indexArray.at(n));
(void) fprintf(stdout, "%d\n", indexArray.at(last_index));
#endif
			(void) fprintf(stdout, "# of (convex) point(s) = %d\n", outPointArray.GetSize());
			// 여기서 맘춘다.
			break;
		}

		int	n0 = indexArray.at(__index_n0);		// 시작점에 해당하는 inPointArray 배열에 대한 인덱스
		int	n1 = indexArray.at(__index_n0 + 1);	// 기준 벡터를 만드는 끝점에 해당하는 inPointArray 배열에 대한 인덱스
		D3DXVECTOR3	V1 = inPointArray[n1].P - inPointArray[n0].P;	// 기준 벡터: V1
		D3DXVec3Normalize(&V1, &V1);			// 상대적으로 비교하기 위해 길이를 1로 만든다.
#ifdef _DEBUG2
		(void) fprintf(stdout, "__index_n0 = %d; n0 = %d, n1 = %d (__n0 = %d, N = %d)\n", __index_n0, n0, n1, __n0, inPointArray.size());
#endif

		int	index_n0 = -1;	// 다음 n0 에 대한, indexArray 배열에 대한 인덱스
		float	f_max = -FLT_MAX;
		float	g_min = +FLT_MAX;
		for (int n = __index_n0 + 2; n <= last_index; ++n) {
			int	n2 = indexArray.at(n);		// 비교 벡터를 만드는 끝점에 해당하는 inPointArray 배열에 대한 인덱스
			D3DXVECTOR3	VN = inPointArray[n2].P - inPointArray[n0].P;	// 비교 벡터: VN
			D3DXVec3Normalize(&VN, &VN);		// 상대적으로 비교하기 위해 길이를 1로 만든다.
#if 1
			D3DXVECTOR3	V0;	// V0 = nDir x V1
			D3DXVec3Cross(&V0, &nDir, &V1);
			D3DXVec3Normalize(&V0, &V0);		// 상대적으로 비교하기 위해 길이를 1로 만든다.
			float	f = D3DXVec3Dot(&VN, &V0);
#ifdef _DEBUG2
			(void) fprintf(stdout, "(%d, %d) / (%d, %d): f = %f\n", n0, n1, n0, n2, f);
#endif
			if (f > 0) {
//......
//	f = nDir . (V1 x VN) = VN . (nDir x V1) = VN . V0
//......
//	주) 여기서 f 는 n 번째 점에서 V0 = nDir x V1 에 의해서 결정되는 평면에 이르는 부호 거리와 같다.
//	이는 V0 x nDir = V1 에 의해서 결정되는 평면에 이르는 부호거리(g)가 양수일 때에 한해서 이렇다.
//	만약 f 가 양수이면서 동시에 g 가 음수라면 비록 작은 f 라 할지라도 더 바깥쪽에 있다.
//		g = VN . (V0 x nDir) = VN . V1
				float	g = D3DXVec3Dot(&V1, &VN);// - D3DXVec3Dot(&nDir, &VN) * D3DXVec3Dot(&nDir, &V1);
#ifdef _DEBUG2
				(void) fprintf(stdout, "(%d, %d) / (%d, %d): f = %f, g = %f\n", n0, n1, n0, n2, f, g);
#endif
#if 1
				if (g < 0) {
					if (g < g_min) {
						g_min = g;
						index_n0 = n;
#ifdef _DEBUG2
						(void) fprintf(stdout, "(%d, %d) / (%d, %d): f = %f, *g = %f\n", n0, n1, n0, n2, f, g);
#endif
					}
				} else {
					if (g_min >= 0 && f > f_max) {	// index_n0 가 g 가 음수인 경우에서
									// 결정되지 않은 경우에 한해서...
						f_max = f;
						index_n0 = n;
#ifdef _DEBUG2
						(void) fprintf(stdout, "(%d, %d) / (%d, %d): *f = %f, g = %f\n", n0, n1, n0, n2, f, g);
#endif
					}
				}
#endif
			}
#else
			D3DXVECTOR3	V1xVN;
			D3DXVec3Cross(&V1xVN, &V1, &VN);
			float	f = D3DXVec3Dot(&nDir, &V1xVN);
			if (f > f_max) {
				f_max = f;
				index_n0 = n;
			}
#ifdef _DEBUG2
			(void) fprintf(stdout, "(%d, %d) / (%d, %d): f = %f\n", n0, n1, n0, n2, f);
#endif
#endif
		}
#if 1
		if (index_n0 == -1)
			++__index_n0;
		else
			__index_n0 = index_n0;
#ifdef _DEBUG2
		(void) fprintf(stdout, "index_n0 = %d; __index_n0 = %d\n", index_n0, __index_n0);
#endif
#else
		if (f_max < 0)
			++__index_n0;
		else
			__index_n0 = index_n0;
#ifdef _DEBUG2
		(void) fprintf(stdout, "index_n0 = %d, f_max = %f; __index_n0 = %d\n", index_n0, f_max, __index_n0);
#endif
#endif
		// 새로운 index_n0 에서 반복한다.
	}
} while (0);

	// 길이의 합을 구한다.
#if 1	// 뼈
	wrapping_length0 = 0.f;
#endif	// 뼈
	wrapping_length = 0.f;
	for (int n = 0; n < (int) outPointArray.GetSize() - 1; ++n) {
#if 1	// 뼈
		D3DXVECTOR3	v0 = outPointArray[n + 1].P0 - outPointArray[n].P0;
		wrapping_length0 += D3DXVec3Length(&v0);
#endif	// 뼈
		D3DXVECTOR3	v = outPointArray[n + 1].P - outPointArray[n].P;
		wrapping_length += D3DXVec3Length(&v);
	}
	if (closed) {
#if 1	// 뼈
		D3DXVECTOR3	v0 = outPointArray[0].P0 - outPointArray[outPointArray.GetSize() - 1].P0;
		wrapping_length0 += D3DXVec3Length(&v0);
#endif	// 뼈
		D3DXVECTOR3	v = outPointArray[0].P - outPointArray[outPointArray.GetSize() - 1].P;
		wrapping_length += D3DXVec3Length(&v);
	}
#ifdef _DEBUG
(void) fprintf(stdout, "...done\n");
#endif

	return 0;
}
#endif
