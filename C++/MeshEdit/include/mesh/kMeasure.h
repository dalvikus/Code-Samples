#pragma once
#ifndef __kMEASURE__
#define __kMEASURE__
////////////////////////////////////////////////////////////////////////////////


#include <map>
#include <vector>
#include <mesh/kDef.h>
#include <mesh/kEnum.h>
#include <DXUT/Core/DXUT.h>


#if 1	// BONE_DEF
class kMesh;
struct FACEID_LIST
{
	int	n;
	DWORD*	ptr;
};
struct BONE_DEF
{
private:
	bool	__calculated;
public:
	const FACEID_LIST*	__ptr;
	D3DXVECTOR3	centroid;
	D3DXVECTOR3	nDir;

public:
	BONE_DEF(const FACEID_LIST* ptr) : __ptr(ptr) {}
public:
	void	calculate(const kMesh& mesh, bool force = false);
};
#endif	// BONE_DEF

typedef enum {
	MEASURE_TYPE__TWO_POINT_DISTANCE	= 0,	// 두 점 사이의 (최단) 거리
	MEASURE_TYPE__POINT_PLANE_DISTANCE,		// 점과 평면사이의 (최단) 거리
	MEASURE_TYPE__TWO_PLANE_DISTANCE,		// 두 평면 사이의 거리
	MEASURE_TYPE__GIRTH,				// 둘레의 길이
	MEASURE_TYPE__TWO_POINT_GIRTH,			// 곡면 상의 두 점 사이의 (둘레)거리
} MEASURE_TYPE_T;
struct MEASURE_DEF__BASE
{
};

struct REF_POINT
{
	bool		use_inPoint;
	D3DXVECTOR3	inPoint;
	bool		set_outPoint;
	D3DXVECTOR3	outPoint;

	REF_POINT() :
		use_inPoint(false),
		set_outPoint(false)
	{
		inPoint = D3DXVECTOR3(0, 0, 0);
		outPoint = D3DXVECTOR3(0, 0, 0);
	}
};

class kMesh;
struct MEASURE_ITEM_PTR_DEF
{
public:
	D3DXVECTOR3	arrow_end_point;
protected:
	bool	__done;		// 계산이 되었는 지
	bool	__closed;
	float	__length;		// 계산 결과
	CGrowableArray<SEGMENT_VERTEX>	__pointArray;	// 계산에 사용된 점들의 모임

public:
	void	SetDirty()	{__done = false;}
	MEASURE_ITEM_PTR_DEF() : __done(false), __closed(false), __length(0.f) {}
	virtual ~MEASURE_ITEM_PTR_DEF() {
//		(void) fprintf(stdout, "~MEASURE_ITEM_PTR_DEF\n");
	}	// 실제로 MEASURE_ITEM_PTR_DEF__xxx 클래스를 new 하고,
		// 삭제할 때는 MEASURE_ITEM_PTR_DEF 클래스를 delete 하므로
		// 반드시 virtual 로 선언된 destructor 가 있어야 한다.
		// (이를 상속받는 클래스에서 그러지 않아도 된다.
		// 왜냐하면 virtual 을 상속받으면 무조건 virtual 이 되기 때문이다.)

public:
	virtual int	calculate(
		const kMesh&	mesh,
		bool&	closed,
		float&	length,
		CGrowableArray<SEGMENT_VERTEX>&	pointArray,
		REF_POINT*	refPointPtr = 0,
#if 1	// BONE_DEF
		bool	use_bone_if_possible = true,
#endif	// BONE_DEF
		bool	force = false
	) = 0;
};

// MEASURE_TYPE__TWO_POINT_DISTANCE
struct MEASURE_DEF__TWO_POINT_DISTANCE : MEASURE_DEF__BASE
{
	TWO_POINT_DEF	__twoPointDef;

	MEASURE_DEF__TWO_POINT_DISTANCE(const TWO_POINT_DEF& twoPointDef) : __twoPointDef(twoPointDef) {}
};
struct MEASURE_ITEM_PTR_DEF__TWO_POINT_DISTANCE : MEASURE_ITEM_PTR_DEF
{
public:
	TWO_POINT_DEF	__twoPointDef;

public:
	MEASURE_ITEM_PTR_DEF__TWO_POINT_DISTANCE(const TWO_POINT_DEF& twoPointDef) : __twoPointDef(twoPointDef) {}
	virtual ~MEASURE_ITEM_PTR_DEF__TWO_POINT_DISTANCE() {}

public:
	virtual int	calculate(
		const kMesh&	mesh,
		bool&	closed,
		float&	length,
		CGrowableArray<SEGMENT_VERTEX>&	pointArray,
		REF_POINT*	refPointPtr = 0,
#if 1	// BONE_DEF
		bool	use_bone_if_possible = true,
#endif	// BONE_DEF
		bool	force = false
	);
};

// MEASURE_TYPE__POINT_PLANE_DISTANCE
struct MEASURE_DEF__POINT_PLANE_DISTANCE : MEASURE_DEF__BASE
#if 1	// BONE_DEF
	, BONE_DEF
#endif	// BONE_DEF
{
	POINT_PLANE_DEF	__pointPlaneDef;

#if 1	// BONE_DEF
	MEASURE_DEF__POINT_PLANE_DISTANCE(const POINT_PLANE_DEF& pointPlaneDef, const FACEID_LIST* ptr = NULL) : BONE_DEF(ptr), __pointPlaneDef(pointPlaneDef) {}
#else	// BONE_DEF/!BONE_DEF
	MEASURE_DEF__POINT_PLANE_DISTANCE(const POINT_PLANE_DEF& pointPlaneDef) : __pointPlaneDef(pointPlaneDef) {}
#endif	// !BONE_DEF
};
struct MEASURE_ITEM_PTR_DEF__POINT_PLANE_DISTANCE : MEASURE_ITEM_PTR_DEF
#if 1	// BONE_DEF
	, BONE_DEF
#endif	// BONE_DEF
{
public:
	POINT_PLANE_DEF	__pointPlaneDef;

public:
#if 1	// BONE_DEF
	MEASURE_ITEM_PTR_DEF__POINT_PLANE_DISTANCE(
		const MEASURE_DEF__POINT_PLANE_DISTANCE&	pointPlaneDistanceDef
	) : BONE_DEF(pointPlaneDistanceDef)
	{
		__pointPlaneDef = pointPlaneDistanceDef.__pointPlaneDef;
	}
#else	// BONE_DEF/!BONE_DEF
	MEASURE_ITEM_PTR_DEF__POINT_PLANE_DISTANCE(const POINT_PLANE_DEF& pointPlaneDef) : __pointPlaneDef(pointPlaneDef) {}
#endif	// !BONE_DEF
	virtual ~MEASURE_ITEM_PTR_DEF__POINT_PLANE_DISTANCE() {}

public:
	virtual int	calculate(
		const kMesh&	mesh,
		bool&	closed,
		float&	length,
		CGrowableArray<SEGMENT_VERTEX>&	pointArray,
		REF_POINT*	refPointPtr = 0,
#if 1	// BONE_DEF
		bool	use_bone_if_possible = true,
#endif	// BONE_DEF
		bool	force = false
	);
};

// MEASURE_TYPE__TWO_PLANE_DISTANCE
struct MEASURE_DEF__TWO_PLANE_DISTANCE : MEASURE_DEF__BASE
#if 1	// BONE_DEF
	, BONE_DEF
#endif	// BONE_DEF
{
	TWO_PLANE_DEF	__twoPlaneDef;

#if 1	// BONE_DEF
	MEASURE_DEF__TWO_PLANE_DISTANCE(const TWO_PLANE_DEF& twoPlaneDef, const FACEID_LIST* ptr = NULL) : BONE_DEF(ptr), __twoPlaneDef(twoPlaneDef) {}
#else	// BONE_DEF/!BONE_DEF
	MEASURE_DEF__TWO_PLANE_DISTANCE(const TWO_PLANE_DEF& twoPlaneDef) : __twoPlaneDef(twoPlaneDef) {}
#endif	// !BONE_DEF
};
struct MEASURE_ITEM_PTR_DEF__TWO_PLANE_DISTANCE : MEASURE_ITEM_PTR_DEF
#if 1	// BONE_DEF
	, BONE_DEF
#endif	// BONE_DEF
{
public:
	TWO_PLANE_DEF	__twoPlaneDef;

public:
#if 1	// BONE_DEF
	MEASURE_ITEM_PTR_DEF__TWO_PLANE_DISTANCE(
		const MEASURE_DEF__TWO_PLANE_DISTANCE&	twoPlaneDistanceDef
	) : BONE_DEF(twoPlaneDistanceDef)
	{
		__twoPlaneDef = twoPlaneDistanceDef.__twoPlaneDef;
	}
#else	// BONE_DEF/!BONE_DEF
	MEASURE_ITEM_PTR_DEF__TWO_PLANE_DISTANCE(const TWO_PLANE_DEF& twoPlaneDef) : __twoPlaneDef(twoPlaneDef) {}
#endif	// !BONE_DEF
	virtual ~MEASURE_ITEM_PTR_DEF__TWO_PLANE_DISTANCE() {}

public:
	virtual int	calculate(
		const kMesh&	mesh,
		bool&	closed,
		float&	length,
		CGrowableArray<SEGMENT_VERTEX>&	pointArray,
		REF_POINT*	refPointPtr = 0,
#if 1	// BONE_DEF
		bool	use_bone_if_possible = true,
#endif	// BONE_DEF
		bool	force = false
	);
};

// MEASURE_TYPE__GIRTH
struct MEASURE_DEF__GIRTH : MEASURE_DEF__BASE
#if 1	// BONE_DEF
	, BONE_DEF
#endif	// BONE_DEF
{
	TAPE_MEASURE_MODE_T	__mode;
	PLANE_DEF	__planeDef;

#if 1	// BONE_DEF
	MEASURE_DEF__GIRTH(TAPE_MEASURE_MODE_T mode, const PLANE_DEF& planeDef, const FACEID_LIST* ptr = NULL) : BONE_DEF(ptr), __mode(mode), __planeDef(planeDef) {}
#else	// BONE_DEF/!BONE_DEF
	MEASURE_DEF__GIRTH(TAPE_MEASURE_MODE_T mode, const PLANE_DEF& planeDef) : __mode(mode), __planeDef(planeDef) {}
#endif	// !BONE_DEF
};
struct MEASURE_ITEM_PTR_DEF__GIRTH : MEASURE_ITEM_PTR_DEF
#if 1	// BONE_DEF
	, BONE_DEF
#endif	// BONE_DEF
{
public:
	TAPE_MEASURE_MODE_T	__mode;
	PLANE_DEF	__planeDef;

public:
#if 1	// BONE_DEF
	MEASURE_ITEM_PTR_DEF__GIRTH(
		TAPE_MEASURE_MODE_T	mode,
		const MEASURE_DEF__GIRTH&	girthDef
	) : BONE_DEF(girthDef), __mode(mode)
	{
		__planeDef = girthDef.__planeDef;
	}
#else	// BONE_DEF/!BONE_DEF
	MEASURE_ITEM_PTR_DEF__GIRTH(
		TAPE_MEASURE_MODE_T	mode,
		PLANE_DEF	planeDef
	) : __mode(mode), __planeDef(planeDef)
	{
	}
#endif	// !BONE_DEF
	virtual ~MEASURE_ITEM_PTR_DEF__GIRTH() {
//		(void) fprintf(stdout, "~MEASURE_ITEM_PTR_DEF__GIRTH\n");
	}

public:
	virtual int	calculate(
		const kMesh&	mesh,
		bool&	closed,
		float&	length,
		CGrowableArray<SEGMENT_VERTEX>&	pointArray,
		REF_POINT*	refPointPtr = 0,
#if 1	// BONE_DEF
		bool	use_bone_if_possible = true,
#endif	// BONE_DEF
		bool	force = false
	);
};

// MEASURE_TYPE__TWO_POINT_GIRTH
struct MEASURE_DEF__TWO_POINT_GIRTH : MEASURE_DEF__BASE
{
	TAPE_MEASURE_MODE_T	__mode;
	TWO_POINT_DEF	__twoPointDef;

	MEASURE_DEF__TWO_POINT_GIRTH(TAPE_MEASURE_MODE_T mode, const TWO_POINT_DEF& twoPointDef) : __mode(mode), __twoPointDef(twoPointDef) {}
};
struct MEASURE_ITEM_PTR_DEF__TWO_POINT_GIRTH : MEASURE_ITEM_PTR_DEF
{
	TAPE_MEASURE_MODE_T	__mode;
	TWO_POINT_PLANE_DEF	__2pDef;

	// Obsolete;
	// Use MEASURE_ITEM_PTR_DEF__TWO_POINT_GIRTH(TAPE_MEASURE_MODE_T, const TWO_POINT_DEF&, const D3DXVECTOR3*)
	MEASURE_ITEM_PTR_DEF__TWO_POINT_GIRTH(
		TAPE_MEASURE_MODE_T	mode,
		const POINT_DEF&	p0Def,
		const POINT_DEF&	p1Def,
		const D3DXVECTOR3*	normalPtr = 0
	) : __mode(mode)
	{
		__2pDef.point0Def = p0Def;
		__2pDef.point1Def = p1Def;
		if (normalPtr)
			__2pDef.normal = *normalPtr;
	}
	MEASURE_ITEM_PTR_DEF__TWO_POINT_GIRTH(
		TAPE_MEASURE_MODE_T	mode,
		const TWO_POINT_DEF&	twoPointDef,
		const D3DXVECTOR3*	normalPtr = 0
	) : __mode(mode)
	{
		*((TWO_POINT_DEF*) &__2pDef) = twoPointDef;
		if (normalPtr)
			__2pDef.normal = *normalPtr;
	}
	virtual ~MEASURE_ITEM_PTR_DEF__TWO_POINT_GIRTH() {
//		(void) fprintf(stdout, "~MEASURE_ITEM_PTR_DEF__TWO_POINT_GIRTH\n");
	}

	virtual int	calculate(
		const kMesh&	mesh,
		bool&	closed,
		float&	length,
		CGrowableArray<SEGMENT_VERTEX>&	pointArray,
		REF_POINT*	refPointPtr = 0,
#if 1	// BONE_DEF
		bool	use_bone_if_possible = true,
#endif	// BONE_DEF
		bool	force = false
	);
};

struct MEASURE_DEF {
	MEASURE_TYPE_T	__type;
	MEASURE_DEF__BASE*	__ptr;
};
struct MEASURE_DEF_LIST
{
	int	n;	// ptr 에 있는 MEASURE_DEF 갯수
	MEASURE_DEF*	ptr;
};
struct MEASURE_TAG
{
	LPCWSTR		name;
	D3DXVECTOR2	arrow_start_point;
	D3DCOLOR	color;
};
struct MEASURE_TYPE_DEF {
	TAPE_MEASURE_TYPE_T	type;
	MEASURE_TAG		tag;
	MEASURE_DEF_LIST	defList;
};
struct MEASURE_TYPE_DEF_LIST
{
	int	n;
	MEASURE_TYPE_DEF*	ptr;
};
struct MEASURE_ITEM_DEF {
	MEASURE_TYPE_T		__type;
	MEASURE_ITEM_PTR_DEF*	__ptr;

	MEASURE_ITEM_DEF() : __ptr(0) {}
	~MEASURE_ITEM_DEF() {
//		(void) fprintf(stdout, "~MEASURE_ITEM_DEF\n");
		if (__ptr) {
			delete __ptr;
			__ptr = 0;
		}
	}
};

#if 1	// 이미 정의된 둘레 측정
#if 1	// kMeasure
#else	// kMeasure/!kMeasure
struct LENGTH_DEF
{
	POINT_DEF	pointDef;	// 단면의 시작위치;
					// nDir = D3DXVECTOR3(0, 0, 0) 이면 단면이 아닌 점
	D3DXVECTOR3	nDir;		// 단면의 법선벡터
};
//typedef std::vector<LENGTH_DEF>	LENGTH_DEF_LIST;	// 둘 이상이면 거리 측정; 하나라면 둘레 특정
#endif	// !kMeasure

#if 1	// kMeasure
struct MEASURE_ITEM : MEASURE_ITEM_DEF
#else	// kMeasure/!kMeasure
struct MEASURE_ITEM : MEASURE_ITEM_DEF, LENGTH_DEF
#endif	// !kMeasure
{
public:
	LPDIRECT3DVERTEXBUFFER9		segmentVertexBuffer;	// 각 점들을 담을 그릇
	LPDIRECT3DINDEXBUFFER9		segmentIndexBuffer;
	float	length;
	bool	closed;
	int	numSegments;
#if 1	// manipulator; LINE MANIPULATOR
	bool	__isReleased;	// line manipulator 에 의해 선의 바뀌었음을 뜻한다.
	const bool&	isReleased() const	{return __isReleased;}
	bool&		isReleased()		{return __isReleased;}
#endif	// manipulator; LINE MANIPULATOR

	bool	allSet;			// 그릇에 점들이 담겼는 지;
					// 중복계산하지 않도록

#if 1	// kMeasure
#else	// kMeasure/!kMeasure
#if 1	// MEASURE_ITEM_DEF
	// kMesh::setMeasure 에서 MEASURE_ITEM 의 갯수가 1보다 큰 경우,
	// 갯수가 2의 배수가 가정한다.
	// 하지만 새로운 방식 MEASURE_ITEM_DEF 를 사용하는 경우에는 그걸 필요가 없다.
	// 이전과의 호환을 위해 MEASURE_ITEM_DEF 을 사용하는 경우,
	// 첫 번째에 모든 정보가 담겨있다. 두 번째를 만들기는 하지만 두 번째는 의미없는 것으로 한다.
private:
	bool	__invalid;
public:
	bool	invalid() const	{return __invalid;}
#endif
#endif	// !kMesh

#if 1	// kMeasure
#else	// kMeasure/!kMeasure
public:
	bool	isPoint() const	{return nDir.x == 0.f && nDir.y == 0.f && nDir.z == 0.f;}
#endif	// !kMeasure

public:
	MEASURE_ITEM();
#if 1	// kMeasure
	MEASURE_ITEM(const LPDIRECT3DDEVICE9 devicePtr, unsigned maxSegments);
#else	// kMeasure/!kMeasure
	MEASURE_ITEM(const LPDIRECT3DDEVICE9 devicePtr, unsigned maxSegments, const LENGTH_DEF& lengthDef);
#endif	// !kMeasure
	~MEASURE_ITEM();
private:
#if 1	// kMeasure
	void	__Create(const LPDIRECT3DDEVICE9 devicePtr, unsigned maxSegments);
#else	// kMeasure/!kMeasure
	void	__Create(const LPDIRECT3DDEVICE9 devicePtr, unsigned maxSegments, const LENGTH_DEF& lengthDef);
#endif	// !kMeasure
	void	__Destroy();
};
//	std::vector<MEASURE_ITEM*>		measure_item_ptr_list;

struct MEASURE
{
public:
	MEASURE_TAG	tag;
public:
	float	length;	// 각 segment 길이의 합
#if 1	// kMeasure
#else	// kMeasure/!kMeasure
#if 1	// MEASURE_ITEM_DEF
	int	numSegments;	// lineVertexBuffer 에 있는 선분의 갯수;
				// MEASURE_ITEM_DEF 을 사용하면 invalid 한 MEASURE_ITEM 이 있을 수 있으므로,
				// measure_item_ptr_list 의 갯수가 1보다 큰 경우라 할지라도
				// measure_item_ptr_list 의 갯수와 같지 않다.
#endif
#endif	// !kMeasure
	LPDIRECT3DVERTEXBUFFER9		lineVertexBuffer;
	std::vector<MEASURE_ITEM*>	measure_item_ptr_list;

	MEASURE(const LPDIRECT3DDEVICE9 devicePtr, const std::vector<MEASURE_ITEM*> m_item_ptr_list, MEASURE_TAG tg);
	virtual ~MEASURE();
private:
	void	__Destroy();
};
typedef std::map<TAPE_MEASURE_TYPE_T, MEASURE*>	MEASURE_PTR_HASH;
#endif


////////////////////////////////////////////////////////////////////////////////
#endif	// !__kMEASURE__
