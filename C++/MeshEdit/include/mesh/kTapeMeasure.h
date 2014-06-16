#pragma once
#ifndef __kTAPE_MEASURE__
#define __kTAPE_MEASURE__
////////////////////////////////////////////////////////////////////////////////


#include <Windows.h>
#include <d3dx9.h>
#include <vector>
#include <mesh/kPolygon.h>
#include <mesh/kDef.h>
#include <mesh/kEnum.h>
#if 1	// __statePtr
#include <mesh/kState.h>	// INTERSECTION
#endif
#include <mesh/kMeasure.h>

class kMesh;
struct POINT_INFO
{
	// 시작점 P 를 지나고 g__n 에 수직인 단면의 외곽선을 결정하는 선 위의 점들의 집합을 구한다;
	// 시작점은 Edge 상(__lastEdgeID)의 한 점이다;
	// Edge 의 시작에서 매개변수 t 에 해당하는 위치에 있다: t = 0:= Edge 의 시작점, t = 1:= Edge 의 끝점
	// 양 끝점(t = 0 또는 t = 1) 경우는 따로 처리한다; ID = __lastVertexID
	DWORD	vertexID;	// 이 값이 -1 이 아니면 Vertex 에서 시작
	DWORD	edgeID;		// 이 값이 -1 이 아니면 Vertex 가 아닌 Edge 위의 한 점에서 시작
				// 둘 중의 하나만 -1 이 아니어야 한다;
				// 모두 다 -1 이라면 둘레 선을 그리는 모드가 아니다.
	FLOAT	t;		// Vertex 가 아닌 Edge 에서 시작할 때 시작점의 매개변수 t;
	DWORD	faceID;		// 고려의 대상에서 제외되는 Face ID;
				// measureContinue 즉 이어서 계측할 때만 사용된다.
};
typedef enum {
#if 0
	START_TYPE__CONTINUE = 0,	// 지난 번 잰 이후에서 계속; POINT_INFO
#endif
	START_DEF_TYPE__GIRTH,		// 둘레의 길이; PLANE_DEF
	START_DEF_TYPE__TWO_POINT		// 곡면 위의 두 점을 지나는 길이; TWO_POINT_PLANE_DEF
} START_DEF_TYPE_T;
typedef enum {
	START_DEF_PTR_TYPE__GIRTH,		// 둘레의 길이; PLANE_DEF
	START_DEF_PTR_TYPE__TWO_POINT,		// 곡면 위의 두 점을 지나는 길이; TWO_POINT_PLANE_DEF
	START_DEF_PTR_TYPE__TWO_POINT2,		// 곡면 위의 두 점을 지나는 길이; TWO_POINT_PLANE_DEF2
} START_DEF_PTR_TYPE_T;
struct START_DEF {	// 단면의 의해 잘리는 둘레 또는 일부의 길이를 잴 때 필요한 정보가 모두 담긴 구조
	START_DEF_TYPE_T	type;

	// type 에 따라 다음 중의 하나만 사용
#if 0
	POINT_INFO		last_point;
#endif
	PLANE_DEF		plane_def;
	TWO_POINT_PLANE_DEF	two_point_plane_def;
};
struct START_DEF_PTR {
	START_DEF_PTR_TYPE_T	type;
	union {
#if 0
		POINT_INFO*		last_point_ptr;
#endif
		PLANE_DEF*		plane_def_ptr;
		TWO_POINT_PLANE_DEF*	two_point_plane_def_ptr;
		TWO_POINT_PLANE_DEF2*	two_point_plane_def2_ptr;
	};
};

class kTapeMeasure : k2DInterfaceBase
{
public:
	static const float	Normal_Tolerance;
private:
	// TO DO LIST: DirectX 에 관련된 부분을 클래스로 만들어 상속받자.
	LPDIRECT3DDEVICE9	__devicePtr;
	kMesh*			__meshPtr;
	void	__init();
public:
	kTapeMeasure(LPDIRECT3DDEVICE9 devicePtr, kMesh* meshPtr);
	kTapeMeasure();
	void	setDeviceAndMesh(LPDIRECT3DDEVICE9 devicePtr, kMesh* meshPtr);
	virtual ~kTapeMeasure();
	void	Destroy();

private:
	D3DXVECTOR3	__center;	// 둘레를 잴 때 나온 점들의 중심; TAPE_MEASURE_METHOD_GIRTH 에만 해당
	D3DXVECTOR3	__right;	// 둘레를 재는 단면을 회전시킬 때 사용되는 Right 방향;
					// 첫 번째 Segment 의 방향; 원래는 접선방향으로 정해야 하나...
	D3DXVECTOR3	__up;		// 둘레를 재는 단면을 회전시킬 때 사용되는 Up 방향;
					// __right x __up 으로 정한다
			// 어느 경우든 적어도 하나의 Segment 가 있어야 정의된다.
public:
	const D3DXVECTOR3&	center() const	{return __center;}
	D3DXVECTOR3&		center()	{return __center;}
	const D3DXVECTOR3&	right() const	{return __right;}
	D3DXVECTOR3&		right()		{return __right;}
	const D3DXVECTOR3&	up() const	{return __up;}
private:
	static int	MAX_LOOP;
	static FLOAT	ShiftFactor;	// 모든 점들을 이 값에 대응하는 값만큼 N 방향으로 움직여서
					// Mesh 에 묻히지 않도록 한다.
	FLOAT		__shiftAmount;

private:
	// 시작점 P 를 지나고 g__n 에 수직인 단면의 외곽선을 결정하는 선 위의 점들의 집합을 구한다;
	// 시작점은 Edge 상(__lastEdgeID)의 한 점이다;
	// Edge 의 시작에서 매개변수 t 에 해당하는 위치에 있다: t = 0:= Edge 의 시작점, t = 1:= Edge 의 끝점
	// 양 끝점(t = 0 또는 t = 1) 경우는 따로 처리한다; ID = __lastVertexID
	DWORD		__lastVertexID;		// 이 값이 -1 이 아니면 Vertex 에서 시작
	DWORD		__lastEdgeID;		// 이 값이 -1 이 아니면 Vertex 가 아닌 Edge 위의 한 점에서 시작
						// 둘 중의 하나만 -1 이 아니어야 한다;
						// 모두 다 -1 이라면 둘레 선을 그리는 모드가 아니다.
	FLOAT		__lastT;		// Vertex 가 아닌 Edge 에서 시작할 때 시작점의 매개변수 t;
	DWORD		__lastFaceID;		// 고려의 대상에서 제외되는 Face ID;
						// measureContinue 즉 이어서 계측할 때만 사용된다.
// 마지막 Segment 의 끝점에서 값들...
//	둘레를 잴 때 즉 measureContinue 함수에서,
//	그 끝점을 시작점으로해서 둘레의 길이를 잰다.
//	위의 네 값들이 아래의 값들로 교체된다.
	DWORD		__lastVertexIDAtLastSegment;
	DWORD		__lastEdgeIDAtLastSegment;
	FLOAT		__lastTAtLastSegment;
	DWORD		__lastFaceIDAtLastSegment;

#if 1	// 단면의 의해 잘리는 둘레나 둘레의 일부의 길이를 잴 때,
	// 다음 구조만 설정한 후, doMeasure_s 를 호출하면 된다;
	// 임의 둘레 측정시 사용
private:
	START_DEF	__startDef;
public:
	const START_DEF&	startDef() const	{return __startDef;}
	START_DEF&		startDef()		{return __startDef;}
#endif
#if 1	// manipulator; LINE MANIPULATOR
private:
	bool	__isReleased;	// 측정이 일어난 후, line manipulator 에 의해 선이 바뀌었을 때 True 로 설정;
				// 새로운 측정 시 False 로 재설정
public:
	const bool&	isReleased() const	{return __isReleased;}
	bool&		isReleased()		{return __isReleased;}
#endif	// manipulator; LINE MANIPULATOR

public:
	// 둘레는 재는 시작점을 정한다.
	void	setBasePoint(
		DWORD	edgeID,
		FLOAT	t,
		DWORD	vertexID,
		DWORD	lastFaceID
	);
	// 둘레를 재는 단면에 수직인 벡터를 설정한다;
	static void	setDir(D3DXVECTOR3& nDir, const D3DXVECTOR3& axis, FLOAT theta);
	// Barycentric coordinates (u, v) 에 의해서 결정되는, 삼각형(ID = faceID) 내부의 한 점 P 를 지나고
	// nDir 에 수직인 단면이 만나는 Edge 에 대한 정보를 얻는다.
	static DWORD	kTapeMeasure::findEdge(
		const kMesh*	meshPtr,
		const D3DXVECTOR3&	nDir,

		DWORD	faceID,
		FLOAT	u,	// barycentric coordinate U
		FLOAT	v,	// barycentric coordinate V

		FLOAT&	t,	// [OUT]
		// nDir 방향이 왼손 엄지손가락 방향으로 했을 때,
		// 나머지 왼손 네 손가락이 둘레를 감싸는 방향에 있는 점;
		// clockwise 라면 끝점, counter-clockwise 라면 시작점
		bool	clockwise = true
	);

private:
	bool	__bDir;		// 둘레는 재는 시작점을 움직일 때 한 방향으로 움직이게 한다;
				// 시작점을 움직일 때 계속해서 위쪽으로 움직이거나
				// 아래쪽으로 시작점이 움직이게 된다.
				//
				// Edge 의 어느 방향을 따라 움직이는 지 말해준다;
				// True:= prStartID -> prEndID, False:= prEndID -> prStartID
				// lastVertexID 가 (DWORD) -1 일 때만, 즉 Edge 상에 있을 때만, 의미가 있다.
public:
	int	moveBasePoint(const D3DXVECTOR3& vDir);	// 둘레를 재는 시작점을 vDir 방향으로 움직인다.
public:
/*
 * 선분을 구성하는 점들은...
 *
	TAPE_MEASURE_MODE_T/TAPE_MEASURE_METHOD_T	TAPE_MEASURE_METHOD_GIRTH	TAPE_MEASURE_METHOD_TWO_POINT
	-----------------------------------------	-------------------------	-----------------------------
	TAPE_MEASURE_MODE_CONTOUR			__segmentVertexBuffer		__twoPointVertexBufferPtr
	TAPE_MEASURE_MODE_WRAP				__convexVertexBufferPtr		__convexVertexBufferPtr

	주1) __segmentVertexBuffer 는 kMesh::__segmentVertexBuffer 를,
	__twoPointVertexBufferPtr 는 kTapeMeasure::__twoPointVertexBufferPtr 를
	__convexVertexBufferPtr 는 kTapeMeasure::__convexVertexBufferPtr 를 의미한다.
	주2) TAPE_MEASURE_METHOD_TWO_POINT 의 경우,
	(1) 결과가 없을 수 있으므로 kTapeMeasure::__twoPointArray 의 크기나
	kTapeMeasure::__convexPointArray 의 크기를 확인한 후 해당 Vertex Buffer 를 사용해야 한다.
	(2) 이 점들을 바탕으로 length 를 구했으므로 이 값이 음수이면 제대로 결과를 얻을 수 없었음을 뜻한다.
	(3) __segmentVertexBuffer 에는
		성공한 경우: 완전한(양 끝점을 잇는 선분을 제외한) 선분을 이루는 점들
		실패한 경우: 시작점에서 출발하여 얻은 둘레에 해당하는 점들 (디버깅용)
 */
	static int	doMeasure_s(
		const kMesh*		meshPtr,

		TAPE_MEASURE_MODE_T	mode,
		const START_DEF_PTR&	startDef,
#if 0
		D3DXVECTOR3		nDir,	// 단면을 결정하는 법선벡터;
						// startDef.type = START_TYPE__TWO_POINT 인 경우에는 따로 계산
						// startDef.type = START_TYPE__GIRTH 인 경우 PLANE_DEF 에 있는 값을 사용
#endif

		// [OUT]
		POINT_INFO*		lastPointInfoPtr,
		CGrowableArray<SEGMENT_VERTEX>&	segmentPointArray,	// 단면의 의해서 잘리는 점들의 모임;
							// startDef.type = START_TYPE__TWO_POINT 인 경우에는
							// 계산 과정에서 얻은 점들이 여기에 들어간다.
							// 따라서 twoPointArray 에 유용한 점들이 없다고 하더라도
							// 여기에는 그 과정에서 얻은 점들이 들어있다. (디버깅용)
		FLOAT&	length,		// startDef.type = START_TYPE__TWO_POINT:
					//	twoPointArray 에 있는 점들이 만드는 열린 곡선의 길이
					// startDef.type = START_TYPE__GIRTH:
					//	segmentPointArray 에 있는 점들이 만드는 곡선의 길이
					// mode = TAPE_MEASURE_MODE_WRAP 의 경우, convexPointArray 를 사용한다.
		bool&	closed,		// segmentPointArray 에 있는 점들이 닫힌 곡선을 만드는 지 말해준다
		CGrowableArray<SEGMENT_VERTEX>&	twoPointArray,		// startDef.type = START_TYPE__TWO_POINT 인 경우에 설정
		CGrowableArray<SEGMENT_VERTEX>&	convexPointArray	// mode = TAPE_MEASURE_MODE_WRAP 인 경우에 설정
	);

	// 이 함수는 정의된 길이 중에서 예외적인 경우에만 처리;
	//	1. twoPointArray, convexPointArray 값은 처리하지 않음
	//	2. 3D 단면을 위한 LOWER_FACE 도 처리하지 않음
	// doMeasure_s 를 사용할 것
	void	measureContinue();	// 마지막으로 Segment 를 그렸던 곳에서부터 다시 Segment 를 만들 준비를 한다;
					// AtLastSegment 의 값들로 대체한다.

#if 1	// 둘레 표시
private:
	int	__val;
	int	__val_saved;
	TAPE_MEASURE_MODE_T		__mode;
	TAPE_MEASURE_METHOD_T		__method;
	TAPE_MEASURE_CROSS_SECTION_T	__crossSectionType;
	float				__length;	// 어떤 식으로든 doMeasure 를 호출한 후에 설정된다;
							// 필요할 때마다 저장해야 한다 (__userDefinedArea에...)
							// doMeasure_s 도 마찬가지...
							// 단, 임의계측(manual)에서만
public:
	float&	length()	{return __length;}	// doMeasure_s 를 호출한 후에 __length 를 설정하도록
public:
	// 2D 에서 __val 를 참조할 때
	// override
	virtual int	getValue(int subType, void* dataPtr);
	virtual int	setValue(int subType, const void* dataPtr);

	// 이 클래스를 접근할 수 있는 곳에서 바로 참조할 때
	const int&				getValRef() const		{return __val;}
	int&					getValRef()			{return __val;}
	const int&				getValSavedRef() const		{return __val_saved;}
	int&					getValSavedRef()		{return __val_saved;}
	const TAPE_MEASURE_MODE_T&		getModeRef() const		{return __mode;}
	TAPE_MEASURE_MODE_T&			getModeRef()			{return __mode;}
	const TAPE_MEASURE_METHOD_T&		getMethodRef() const		{return __method;}
	TAPE_MEASURE_METHOD_T&			getMethodRef()			{return __method;}
	const TAPE_MEASURE_CROSS_SECTION_T&	getCrossSectionType() const	{return __crossSectionType;}
	TAPE_MEASURE_CROSS_SECTION_T&		getCrossSectionType()		{return __crossSectionType;}
#if 1	// 사용자 지정 부위
public:
	struct USER_DEFINE_AREA
	{
		std::string	name;		// 이름

		TAPE_MEASURE_MODE_T	mode;	// 길이를 재는 방법
		TAPE_MEASURE_METHOD_T	method;
		TAPE_MEASURE_CROSS_SECTION_T	crossSectionType;
		TWO_POINT_PLANE_DEF	__2pDef;	// 시작점과 끝점;
						// method 가 TAPE_MEASURE_METHOD_GIRTH 인 경우, 하나만 사용
		float		length;		// 길이
		bool		visible;	// 화면에 해당 부위를 표시되었는 지 여부
	};
private:
	std::vector<USER_DEFINE_AREA>	__userDefinedArea;
#endif
#endif

#if 1	// 임의의 두 점 사이의 거리
public:
	static float	DISTANCE_TOLERANCE;
	// 주어진 점(P)이 선분 위에 있는 지 판단한다;
	// 주어진 점에서 선분에 내린 수선의 발(H)은 선분 위에 있다.
	// P 에서 H 까지의 거리를 DISTANCE_TOLERANCE 와 비교하여 판단한다.
	// 이 부분에서 DISTANCE_TOLERANCE 를 조절할 필요가 있을 것이다.
	bool	isPointOnLineSegment(float distance);
	static bool	isPointOnLineSegment(const kMesh* meshPtr, float distance);
	// 서로 다른 두 점 A, B 를 잇는 선분(line segment)과 한 점 P 가 있을 때
	// P 에서 선분또는 선분의 연장선에 이르는 최단 거리(1)와
	// 만나는 점을 결정하는 t(H = A + tAB)를 구한다.
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
	static int	PointLineSegmentIntersection(
		// [IN]
		const D3DXVECTOR3&	A,
		const D3DXVECTOR3&	B,
		const D3DXVECTOR3&	P,
		
		// [OUT]
		float&	t,
		float& distance
	);
private:
	CGrowableArray<SEGMENT_VERTEX>	__twoPointArray;
public:
	const CGrowableArray<SEGMENT_VERTEX>&	twoPointArray() const	{return __twoPointArray;}
	CGrowableArray<SEGMENT_VERTEX>&	twoPointArray()		{return __twoPointArray;}
private:
	LPDIRECT3DVERTEXBUFFER9		__twoPointVertexBufferPtr;
	LPDIRECT3DINDEXBUFFER9		__twoPointIndexBufferPtr;
public:
	const LPDIRECT3DVERTEXBUFFER9&		twoPointVertexBufferPtr() const	{return __twoPointVertexBufferPtr;}
	const LPDIRECT3DINDEXBUFFER9&		twoPointIndexBufferPtr() const	{return __twoPointIndexBufferPtr;}
	void	SetTwoPointBuffer();
#endif

#if 1	// Convex
private:
	CGrowableArray<SEGMENT_VERTEX>	__convexPointArray;
public:
	const CGrowableArray<SEGMENT_VERTEX>&	convexPointArray() const	{return __convexPointArray;}
	CGrowableArray<SEGMENT_VERTEX>&	convexPointArray()		{return __convexPointArray;}
private:
	LPDIRECT3DVERTEXBUFFER9		__convexVertexBufferPtr;
	LPDIRECT3DINDEXBUFFER9		__convexIndexBufferPtr;
public:
	const LPDIRECT3DVERTEXBUFFER9&		convexVertexBufferPtr() const	{return __convexVertexBufferPtr;}
	const LPDIRECT3DINDEXBUFFER9&		convexIndexBufferPtr() const	{return __convexIndexBufferPtr;}
	void	SetConvexBuffer(bool closed);
public:
	// 주어진 점들의 배열과 이 점들이 나열된 순서를 말해주는 법선벡터를 사용하여
	// 점들을 둘러싸는 점들을 얻는다:
	//	법선벡터의 끝점에서 시작점을 봤을 때, 주어진 점들은 반시계방향으로 차례대로 나열되었다;
	//	점들을 오른손의 엄지손가락을 제외한 네 손가락으로 차례대로 감쌌을 때
	//	엄지손가락이 가리키는 방향이 nDir 이다.
	//
	// 리턴값:
	//	<0:= 실패
	//	0:= 성공
	static int	wrapping(
		// [IN]
		bool	closed,	// inPointArray 에 있는 점들이 닫힌 곡선을 만드는 지 아닌지
		const D3DXVECTOR3&	nDir,
		const CGrowableArray<SEGMENT_VERTEX>&	inPointArray,

		// [OUT]
		CGrowableArray<SEGMENT_VERTEX>&	outPointArray,
#if 1	// 뼈
		float&	wrapping_length0,	// outPointArray 에 있는 점들을 이은 길이의 합 (원래 길이)
#endif	// 뼈
		float&	wrapping_length		// outPointArray 에 있는 점들을 이은 길이의 합
	);
#endif
};


////////////////////////////////////////////////////////////////////////////////
#endif	// !__kTAPE_MEASURE__
