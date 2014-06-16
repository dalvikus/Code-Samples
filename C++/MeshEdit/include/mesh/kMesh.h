#pragma once

#include <mesh/kPolygon.h>
#include <DXUT/Core/DXUT.h>
#include <DXUT/Optional/SDKmesh.h>
#include <DXUT/Optional/SDKmisc.h>
#include <mesh/kState.h>
#include <mesh/kArea.h>
#include <mesh/kMeshLoader.h>
#include <mesh/kSelect.h>
#include <mesh/kShowNormal.h>
#include <mesh/kTapeMeasure.h>
#include <mesh/kSimulation.h>
#include <mesh/kBodyElement.h>
#include <mesh/kManipulator.h>
#ifdef __READ_FILES__
#if 1	// kFileCache
#include <mesh/kFileCache.h>
#endif	// kFileCache
#endif	// __READ_FILES__

#if 1	// manipulator callback
void	normal_manipulator_done_callback_manual_measure(kMesh* pointer_to_kMesh, int /* nType */, START_DEF_PTR* pointer_to_START_DEF_PTR, void* valuePtr);
void	normal_manipulator_done_callback_defined_measure(kMesh* pointer_to_kMesh, int nType, START_DEF_PTR* /* pointer_to_START_DEF_PTR */, void* valuePtr);
void	point_manipulator_done_callback_manual_measure(kMesh* pointer_to_kMesh, int /* nType */, START_DEF_PTR* pointer_to_START_DEF_PTR, void* valuePtr);
void	point_manipulator_done_callback_defined_measure(kMesh* pointer_to_kMesh, int nType, START_DEF_PTR* /* pointer_to_START_DEF_PTR */, void* valuePtr);
#endif
void
CLAMP2(
	D3DXVECTOR3&	c,
	FLOAT		lowerBound = 0.f,
	FLOAT		upperBound = 1.f
);

struct kIsometric : k2DInterfaceBase
{
	ISOMETRIC_TYPE_T	__type;
	ISOMETRIC_PLANE_T	__plane;

#if 1	// 색깔 등고선을 따로 처리
	bool		__isColorSaved;		// 색깔 등고선 모드로 갈 때 기존의 모델 색깔을 보존해야 한다.
	SHADING_TYPE	__savedShadingType;	// 색깔 등고선 모드로 갈 때
						// 강제로 쉐이딩 모드를 SHADING_NONE 으로 한다;
						// 원래 쉐이딩 모드를 기억해서 나중에 색깔 등고선 모드가 해제될 때
						// 원래 쉐이딩 모드로 돌아가도록 한다.
	const bool&	getIsColorSavedRef() const	{return __isColorSaved;}
	bool&		getIsColorSavedRef()		{return __isColorSaved;}
	const SHADING_TYPE&	getSavedShadingTypeRef() const	{return __savedShadingType;}
	SHADING_TYPE&		getSavedShadingTypeRef()	{return __savedShadingType;}
#endif

	kIsometric() : __savedShadingType(SHADING_GREY), __isColorSaved(false), __type(ISOMETRIC_UNDEF), __plane(ISOMETRIC_XY) {}

	// 2D 에서 __view, __rotation 를 참조할 때
	// override
	virtual int	getValue(int subType, void* dataPtr);
	virtual int	setValue(int subType, const void* dataPtr);

	// 이 클래스를 접근할 수 있는 곳에서 바로 참조할 때
	const ISOMETRIC_TYPE_T&		getTypeRef() const	{return __type;}
	ISOMETRIC_TYPE_T&		getTypeRef()		{return __type;}
	const ISOMETRIC_PLANE_T&	getPlaneRef() const	{return __plane;}
	ISOMETRIC_PLANE_T&		getPlaneRef()		{return __plane;}
};

struct kModel : k2DInterfaceBase
{
	OBJECT_TYPE	__object;
	SHADING_TYPE	__shading;

	kModel() : __object((OBJECT_TYPE) ((int) OBJECT_FACE | (int) OBJECT_VOXEL)), __shading(SHADING_GREY) {}

	// 2D 에서 __view, __rotation 를 참조할 때
	// override
	virtual int	getValue(int subType, void* dataPtr);
	virtual int	setValue(int subType, const void* dataPtr);

	// 이 클래스를 접근할 수 있는 곳에서 바로 참조할 때
	const OBJECT_TYPE&	getObjectRef() const	{return __object;}
	OBJECT_TYPE&		getObjectRef()		{return __object;}
	const SHADING_TYPE&	getShadingRef() const	{return __shading;}
	SHADING_TYPE&		getShadingRef()		{return __shading;}
};

#define LAST_POSITION__QUEUE_SIZE	3
class kMesh : CDXUTXFileMesh
#ifdef __READ_FILES__
#if 1	// kFileCache
	, virtual public kFileCache
#endif	// kFileCache
#endif	// __READ_FILES__
{
public:
	HWND	__hWnd;	// 길이를 재거나(WM_USER__LENGTH), 각도를 잴 때(WM_USER__ANGLE)
			// 그 결과를 PostMessage 를 통해 알릴 때 사용되는 HWND

#ifdef __USE_A_OFFSET__
public:
	const DWORD*	pointer_to__a_vVertexID;
	const int*	pointer_to__a_vVertexID_a_offset;
	const EDGE*	pointer_to__a_vEdgeID;
	const int*	pointer_to__a_vEdgeID_a_offset;
	const FACE_ID_LOCAL_INDEX*	pointer_to__a_vFaceIDLocalIndex;
	const int*	pointer_to__a_vFaceIDLocalIndex_a_offset;
	const DWORD*	pointer_to__a_faceID;
	const int*	pointer_to__a_faceID_a_offset;
#endif	// __USE_A_OFFSET__
#if 1	// Vertex Buffer 데이터 구조
public:
	// 새로운 항목을 추가하려면,
	// 앞의 N_ELEMENT 값을 변경한 후 다음 내용을 변경해야 한다.
	// D3DVERTEXELEMENT9	kMesh::Element[N_ELEMENT + 1];
	// 특히 UsageIndex 값을 잘 설정해야 한다!!!
#define N_ELEMENT				\
(						\
	1 + /* p */				\
	1 + /* n */				\
	1 + /* tu, tv, tx */			\
	1 + /* c */				\
	(					\
		1 + /* c_ */			\
		1 + /* XY, YZ, ZX */		\
	 	1 + /* nTmp */			\
	 	1 + /* p_ */			\
	 	1 + /* pTmp */			\
	 	2 * LAST_POSITION__QUEUE_SIZE	\
	 )					\
)
	struct D3DXVERTEX {
		D3DXVECTOR3	p;		// D3DDECLUSAGE_POSITION; UsageIndex = 0
		D3DXVECTOR3	n;
		FLOAT		tu, tv, tx;
// index 를 통해 primitive 를 그릴 때(DrawIndexedPrimitive),
// index 는 Vertex Buffer 에서 Position 을 담은 곳을 가리켜야한다.
// Position 은 D3DXVECTOR3 이므로 3의 배수이어야한다.
// Texture 는 (u, v) 만 필요하지만, 이렇게 되면 넣어진 데이터가 3의 배수가 되지않아
// index 를 통해서는 Primitive 를 그릴 수 없다.
// 그래서 또 하나의 의미없는 FLOAT 인 tx 를 넣어주었다.
// 하지만 이는 나중에 Vertex 의 정보를 담을 때 또 다른 의미있는 값으로 사용될 수 있다.
// 예를 들어, 해당 Vertex 가 어느 부위에 해당하는 지를 말해줄 때 사용될 수 있다.
// 하지만 이 정보는 Attribute Buffer 를 사용하는 게 더 낫다.
// 또 모르지 다른 용도가 있을 지...
		D3DXVECTOR3	c;		// D3DDECLUSAGE_COLOR; UsageIndex = 0

		// 등고선 색깔로 바꾸기 전의 색깔; 임시 저장소
		D3DXVECTOR3	c_;		// D3DDECLUSAGE_COLOR; UsageIndex = 1
		// 등고선 색깔을 저장
		FLOAT		XY, YZ, ZX;	// D3DDECLUSAGE_COLOR; UsageIndex = 2

		// n 을 임시로 저장할 때 사용;
		// findNormalAmountByVolumeChange 에서는 n 을 사용한다.
		// 본래의 n 대신에 다른 n, 예를 들어, follow 의 경우, 을 사용하고 자 할 때,
		// 원래의 n 을 임시로 nTmp 로 옮기고 n 을 새로 설정한 후
		// findNormalAmountByVolumeChange 을 호출한다.
		// 처리후 원래대로 돌려놓는다.
		D3DXVECTOR3	nTmp;		// D3DDECLUSAGE_NORMAL; UsageIndex = 2

		// 처음 위치
		D3DXVECTOR3	p_;		// D3DDECLUSAGE_POSITION; UsageIndex = 1
		// 임시 위치
		D3DXVECTOR3	pTmp;		// D3DDECLUSAGE_POSITION; UsageIndex = 2

		// 최대 3 개까지 이전 위치를 기억한다; 원형큐로 구현한다.
		// 아래의 m__iHead, m__iTail 을 
		D3DXVECTOR3	pQueue[LAST_POSITION__QUEUE_SIZE];	// UsageIndex = 3,...
		D3DXVECTOR3	cQueue[LAST_POSITION__QUEUE_SIZE];	// UsageIndex = 3,...
	};
#endif
private:
	LPDIRECT3DDEVICE9		__devicePtr;
public:
	LPDIRECT3DDEVICE9		GetDevicePointer() const		{return __devicePtr;}
private:
	LPD3DXEFFECT			__effectPtr;
	LPDIRECT3DVERTEXDECLARATION9	__vertexDeclarationPtr;
	LPDIRECT3DVERTEXBUFFER9		__vertexBufferPtr;
	LPDIRECT3DINDEXBUFFER9		__edgeIndexBufferPtr;
	LPDIRECT3DINDEXBUFFER9		__faceIndexBufferPtr;
public:
	LPD3DXEFFECT			GetEffectPointer() const		{return __effectPtr;}
	LPDIRECT3DVERTEXDECLARATION9	GetVertexDeclarationPointer() const	{return __vertexDeclarationPtr;}
	const LPDIRECT3DVERTEXBUFFER9	GetVertexBufferPointer() const		{return __vertexBufferPtr;}
	const LPDIRECT3DINDEXBUFFER9	GetEdgeIndexBufferPointer() const	{return __edgeIndexBufferPtr;}
	const LPDIRECT3DINDEXBUFFER9	GetFaceIndexBufferPointer() const	{return __faceIndexBufferPtr;}
#if 3	// porting_DxViewer
#if 4	// SDKmesh
private:
	struct D3DVERTEX {
		D3DXVECTOR3	p;
		D3DXVECTOR3	n;
		FLOAT		tu, tv;

		static const DWORD	FVF;
	};
#endif
public:
	kMesh();
private:
	void	__init();
	bool	__effectInit(
		const LPCTSTR		effectSourceFilePtr,
#if 2	// Texture
		const LPDIRECT3DBASETEXTURE9	texturePtr
#endif
	);
	void	__tapeMeasureInit();

private:
	kState*		__statePtr;
	D3DXVECTOR3	__cm;
	FLOAT		__radius;
	D3DXVECTOR3	__left_lower_near;
	D3DXVECTOR3	__right_upper_far;
	D3DXVECTOR3	__pivotCenter;
	FLOAT	__a;
public:
	kState*			statePtr()		{return __statePtr;}
	const D3DXVECTOR3&	cm() const		{return __cm;}
	float			radius() const		{return __radius;}
	const D3DXVECTOR3&	left_lower_near() const	{return __left_lower_near;}
	const D3DXVECTOR3&	right_upper_far() const	{return __right_upper_far;}
	const D3DXVECTOR3&	pivotCenter() const	{return __pivotCenter;}
	FLOAT	a() const	{return __a;}
private:
	bool	__objRHLoaded;	// Right-handed (Cartesian) Coordinate System 에서 만든 .OBJ 를 읽어들였는가?
#if 1	// Timer
	CDXUTTimer	__timer;
#endif
#ifdef STANDARD_MODEL
private:
	int	__index_to_standard;
public:
	int	index_to_standard()	{return __index_to_standard;}
#endif	// STANDARD_MODEL
public:
	struct SetAllArg
	{
#ifdef STANDARD_MODEL
		int	index_to_standard;
#endif	// STANDARD_MODEL
		kState*			statePtr;
		LPCTSTR			xSourceFilePtr;
		LPDIRECT3DDEVICE9	devicePtr;
		LPCTSTR			effectSourceFilePtr;
		float		fScale;
#ifdef __READ_FILES__
		bool		stillRead;	// 측정모델의 경우 설정
#endif	// __READ_FILES__
		char*		objFilePtr;
		WCHAR*		texturePath;

		HWND	hWnd;	// PostMessage 를 받을 윈도우핸들

#ifdef __READ_FILES__
		LPCWSTR_ARRAY*	textureArrayPtr;


		SetAllArg() : textureArrayPtr(0) {}
#endif	// __READ_FILES__
	};
public:
	int	setAll(const SetAllArg& arg);
	int	setAll(
#ifdef STANDARD_MODEL
		int	index_to_standard,
#endif	// STANDARD_MODEL
		kState*	statePtr,

		LPCTSTR	xSourceFilePtr,
		const LPDIRECT3DDEVICE9	devicePtr,
		LPCTSTR	effectSourceFilePtr,

		float	fScale,
#ifdef __READ_FILES__
		bool	stillRead,	// 측정모델의 경우 설정
#endif	// __READ_FILES__
		const char*	objFilePtr,

		const WCHAR*	texturePath,

		HWND	hWnd = NULL	// PostMessage 를 받을 윈도우핸들

#ifdef __READ_FILES__
		,
		const LPCWSTR_ARRAY*	textureArrayPtr = NULL	// 이미 정의된 텍스쳐 대신에 모델마다 다른 텍스쳐를 쓰고자 할 때 (REAL MODEL 의 경우)
#endif	// __READ_FILES__
	);
#endif
public:
	virtual ~kMesh();
	void	__Destroy();
	static D3DVERTEXELEMENT9	Element[N_ELEMENT + 1];

////////////////////////////////////////////////////////////////////////
private:
	DWORD		__numPointReprs;
	CPointRepr*	__pointReprPtr;
	DWORD		__numEdges;
	CEdge*		__edgePtr;
	DWORD		__numFaces;
	CFace*		__facePtr;
public:
	DWORD			GetNumPointReprs() const	{return __numPointReprs;}
	const CPointRepr*	GetPointReprPointer() const	{return __pointReprPtr;}
	CPointRepr*		GetPointReprPointer()		{return __pointReprPtr;}
	DWORD			GetNumEdges() const		{return __numEdges;}
	CEdge*			GetEdgePointer()		{return __edgePtr;}
	const CEdge*		GetEdgePointer() const		{return __edgePtr;}
	DWORD			GetNumFaces() const		{return __numFaces;}
	CFace*			GetFacePointer()		{return __facePtr;}
	const CFace*		GetFacePointer() const		{return __facePtr;}

private:
	float		__fScale;
	DWORD*		__adjacencyPtr;
	DWORD*		__vertexID2pointReprIDMap;
public:
	const DWORD*	adjacencyPtr() const	{return __adjacencyPtr;}
public:
	// meshPtr 를 사용하여,
	// __pointReprPtr, __facePtr, __edgePtr 를 Point Repr. ID 를 사용하여 구성한다;
	// meshPtr 에서 같은 위치인 Vertex ID 를 하나의 같은 Point Repr. ID 로 만든다.
	// 이는 위치만을 고려한 Vertex Buffer 를 만들고 이를 사용하여
	// SetMesh 에서 새로운 메쉬를 구성한다.
	int	SetPolygon(const LPD3DXMESH meshPtr);
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
private:
	kMesh*	__meshPtr0;	// 포준체형 kMesh 에 대한 포인터
public:
	kMesh*&		meshPtr0()		{return __meshPtr0;}
private:
	LPD3DXMESH	__meshPtr;
public:
	LPD3DXMESH	GetMeshPointer() const			{return __meshPtr;}
public:
	int	SetMesh();	// 위치만을 고려한 D3DXMESH 를 만들어 __meshPtr 으로 설정한다.;
////////////////////////////////////////////////////////////////////////


#if 1	// 큐 관리
private:
	int		m__iHead;	// 시작 데이터의 위치
	int		m__iTail;	// 마지막 데이터의 위치
	unsigned	m__uSize;	// 큐에 있는 데이터의 크기
	int		m__iCurrent;	// 큐에 있는 데이터 중에서 데이터의 위치;
					// 보통 마지막 데이터를 가리킨다.
	float		m__fVolumeChangeQueue[LAST_POSITION__QUEUE_SIZE];	// 저장하는 순간의 __fVolumeChange
public:
	int		GetHead() const		{return m__iHead;}
	int		GetTail() const		{return m__iTail;}
	unsigned	GetQueueSize() const	{return m__uSize;}
	int		GetCurrent() const	{return m__iCurrent;}
public:
	void	history();
#endif


public:
	int	ShowVertex(
		const LPDIRECT3DVERTEXBUFFER9	vertexBufferPtr,
		const LPDIRECT3DVERTEXDECLARATION9	vertexDeclarationPtr,
		UINT				stride,
		DWORD				nPointRepr,
		FLOAT				pointSize = 1.f,
		D3DXVECTOR4			pointColor = D3DXVECTOR4(1.f, 0.f, 0.f, 1.f),
		UINT		StartVertex = 0,
		float	zDelta = 0.f
	) const;
	int	ShowEdge(
		const LPDIRECT3DINDEXBUFFER9	edgeIndexBufferPtr,
		const D3DXVECTOR4&		color,
		DWORD				nEdge
	) const;
	int	ShowFace(
		const LPDIRECT3DINDEXBUFFER9	faceIndexBufferPtr,
		DWORD				nFace,
		SHADING_TYPE	shadingType = SHADING_NONE,
		// kSelect::ShowLowerFace 에서 사용할 수 있도록...
		int				fillMode = D3DFILL_SOLID,
		// fillMode 가 D3DFILL_SOLID 이 아닐 때에만 의미가 있다; mesh.fx 참고
		D3DXVECTOR4			color = D3DXVECTOR4(1.f, 0.f, 0.f, 1.f)
	) const;
	int	ShowSubsetFace() const;

#if 1	// 부피 계산
#if 1	// 절대값
private:
	FLOAT	m__fTotalVolume0;	// V(p_):= 초기 p 값 즉 p_ 를 사용해 계산된 총 부피;
					// 절대값은 별로 의미가 없다.
	void	CalculateTotalVolume(bool use_p_ = true, float* vol_ptr = 0);	// use_p_ = True:= V(p_)
										// use_p_ = False:= V(p)
										// vol_ptr = NULL:= update m__fTotalVolume0
public:
	FLOAT	GetTotalVolume()	{return m__fTotalVolume0;}
#endif
#if 1	// 상대값
private:
	FLOAT	__fVolumeChange;	// __calculateVolumeChange 에 의해 계산
	// __fVolumeChange = V(p) - v(p_), V(p):= Vertex 의 좌표가 p 일 때의 부피
	//	단, pPointReprFlag 를 사용하여, 좌표가 변화된 p 에 대해서만 계산
	//	NULL 이라면 모든 Vertex 가 변화되었다고 가정
	void	__calculateVolumeChange(kMesh::D3DXVERTEX* pV, DWORD* pPointReprFlag = NULL);
public:
	FLOAT	GetVolumeChange()	{return __fVolumeChange;}
#endif
#endif

#if 0
public:
	void	Tessellation(DWORD nTessellation, DWORD* pTessellationFlag);
#endif

// String
private:
	LPDIRECT3DVERTEXBUFFER9		__segmentVertexBuffer;
	LPDIRECT3DINDEXBUFFER9		__segmentIndexBuffer;
	DWORD	__numSegments;
	TAPE_MEASURE_METHOD_T		__segmentMethod;
#if 1	// SEGMENT_VERTEX
#if 1	// 뼈
	static D3DVERTEXELEMENT9	__SegmentVertexElement[4 + 1];
#else	// 뼈/!뼈
	static D3DVERTEXELEMENT9	__SegmentVertexElement[3 + 1];
#endif	// 뼈
#else
	static D3DVERTEXELEMENT9	__SegmentVertexElement[1 + 1];
#endif
	LPDIRECT3DVERTEXDECLARATION9	__SegmentVertexDeclarationPtr;
public:
	LPDIRECT3DVERTEXBUFFER9&		GetSegmentVertexBuffer()	{return __segmentVertexBuffer;}
	const LPDIRECT3DVERTEXBUFFER9&		GetSegmentVertexBuffer() const	{return __segmentVertexBuffer;}
	static DWORD	MAX_SEGMENT;
	DWORD					GetNumSegments()		{return __numSegments;}
	DWORD&	numSegments()	{return __numSegments;}
	const TAPE_MEASURE_METHOD_T&		segmentMethod() const		{return __segmentMethod;}
	TAPE_MEASURE_METHOD_T&			segmentMethod()			{return __segmentMethod;}
	void					SetNSegment(DWORD numSegments)	{__numSegments = numSegments > MAX_SEGMENT ? MAX_SEGMENT : numSegments;}
	LPDIRECT3DINDEXBUFFER9&			GetSegmentIndexBuffer()		{return __segmentIndexBuffer;}
	const LPDIRECT3DINDEXBUFFER9&		GetSegmentIndexBuffer() const	{return __segmentIndexBuffer;}
	void	SetSegmentBuffer(
		LPDIRECT3DVERTEXBUFFER9&	segmentVertexBuffer,
		LPDIRECT3DINDEXBUFFER9&		segmentIndexBuffer,
#if 1	// SEGMENT_VERTEX
		const CGrowableArray<SEGMENT_VERTEX>&	pointArray,
#else
		const std::vector<D3DXVECTOR3>&	pointArray,
#endif
		bool	closed
	);
private:
	int	ShowString(
		const LPDIRECT3DVERTEXBUFFER9	segmentVertexBuffer,
		const LPDIRECT3DINDEXBUFFER9	segmentIndexBuffer,
		DWORD	nSegment,
		const D3DXVECTOR4&	color,
		UINT	StartIndex = 0
	) const;
public:
	void	ShowSegment() const;

#if 4	// isometric
#if 1	// 마지막 주문; 5. 측정선 선명하게...
private:
	float	__shift_amount_along_normal;	// kState::SHIFT_FACTOR_ALONG_NORMAL * __a
public:
	float	shift_amount_along_normal() const	{return __shift_amount_along_normal;}
#else
private:
	static FLOAT	ShiftFactor;	// 모든 점들을 이 값에 대응하는 값만큼 N 방향으로 움직여서
					// Mesh 에 묻히지 않도록 한다.
	FLOAT		__shiftAmount;
#endif
private:
	LPDIRECT3DVERTEXBUFFER9		__x_isometricVertexBufferPtr;
	LPDIRECT3DVERTEXBUFFER9		__y_isometricVertexBufferPtr;
	LPDIRECT3DVERTEXBUFFER9		__z_isometricVertexBufferPtr;
	unsigned	__x_isometricLines;
	unsigned	__y_isometricLines;
	unsigned	__z_isometricLines;
public:
	const LPDIRECT3DVERTEXBUFFER9		x_isometricVertexBufferPtr() const	{return __x_isometricVertexBufferPtr;}
	const unsigned				x_isometricLines() const		{return __x_isometricLines;}
	const LPDIRECT3DVERTEXBUFFER9		y_isometricVertexBufferPtr() const	{return __y_isometricVertexBufferPtr;}
	const unsigned				y_isometricLines() const		{return __y_isometricLines;}
	const LPDIRECT3DVERTEXBUFFER9		z_isometricVertexBufferPtr() const	{return __z_isometricVertexBufferPtr;}
	const unsigned				z_isometricLines() const		{return __z_isometricLines;}
private:
	LPDIRECT3DVERTEXDECLARATION9	__isometricDeclarationPtr;
	void		__isometricInit();
public:
	// nDir 에 수직인 단면으로 Mesh 를 자른 선을 그린다;
	// 최대 nSlice 개의 일정 간격으로 떨어진 단면으로 자른다.
	static unsigned	MAX_SLICE;
	int	setIsometricLines(
		ISOMETRIC_PLANE_T	plane,
		const D3DXVECTOR3&	nDir,
		int			nSlice,
		LPDIRECT3DVERTEXBUFFER9	isometricVertexBufferPtr,	// [OUT]
		unsigned&		isometricLines
	);
	int	ShowIsometric(
		const LPDIRECT3DVERTEXBUFFER9	isometricVertexBufferPtr,
		const unsigned			isometricLines,
		const D3DXVECTOR4&	lineColor,

		UINT	Stride = sizeof(D3DXVECTOR3),
		const D3DXMATRIX*	m_ptr = 0
	) const;
#endif
	int	DrawAxis(const D3DXVECTOR3& upDir, const D3DXVECTOR3& rightDir) const;
	int	DrawAxis0() const;
	int	DrawPivot() const;


private:
	static D3DVERTEXELEMENT9	__OnePointElement[2];
	LPDIRECT3DVERTEXDECLARATION9	__OnePointDeclarationPtr;
public:
	const LPDIRECT3DVERTEXDECLARATION9	OnePointDeclarationPtr() const	{return __OnePointDeclarationPtr;}

public:
	int	findNormalAmountByVolumeChange(
		float&	fNormalAmount,		// [OUT]
		float	fNormalAmountGuess,	// 항상 양수이어야 한다!
		float	volumeChange,		// +:= 늘어난다, -:= 줄어든다
		unsigned&	max_iter,
		float	volumeChangeTolerance
	) const;

	int	ShowShape(float volumeChange) const;
#if 0	// group
	void	ShowGroup(
		const kID&	gid,
		int	showType	// 0x01:= Vertex, 0x02:= Edge, 0x04:= Face
	) const;
#endif

public:
	int	ShowBoundingBox() const;
	int	ShowBoundingSphere() const;

#if 1	// meshFromOBJ
private:
	kMeshLoader	__meshLoader;
public:
	const kMeshLoader&	meshLoader() const	{return __meshLoader;}
	kMeshLoader&	meshLoader()			{return __meshLoader;}
#endif	// meshFromOBJ

#if 1	// mapping
public:
#if 1	// FACE_ID_LOCAL_INDEX
	// 주어진 Point Repr. ID 에 해당하는 점이 어떤 Face 의 몇 번째 Vertex 인지를 말해준다;
	// 여러 개가 있을 수 있으나 이를 사용해 좌표를 얻는 데 목적이 있으므로
	// 같은 Point Repr. ID 를 주었으므로 어떤 것을 사용해도 상관없다.
	//
	// 리턴값:
	//	0	성공
	//	-1	실패
	int	GetFaceIDLocalIndex(DWORD prID, /* [OUT] */ FACE_ID_LOCAL_INDEX& faceIDLocalIndex) const;
	// 특정 Face 의 몇 번째 Vertex 에 해당하는 점에 해당하는 Point Repr. ID 을 얻을 때 사용된다;
	// CFace::prID[] 가 말해준다.
	//
	// 리턴값:
	//	(DWORD) -1	실패
	//	그렇지 않으면	성공
	DWORD	GetPrID(const FACE_ID_LOCAL_INDEX& faceIDLocalIndex) const;
#endif
#endif	// mapping

#if 1	// All in One
private:
	kSelect		__select;
	kShowNormal	__showNormal;
protected:
	kTapeMeasure	__tapeMeasure;
public:
	const kSelect&	select() const			{return __select;}
	kSelect&	select()			{return __select;}
	const kShowNormal&	showNormal() const	{return __showNormal;}
	kShowNormal&		showNormal()		{return __showNormal;}
	const kTapeMeasure&	tapeMeasure() const	{return __tapeMeasure;}
	kTapeMeasure&		tapeMeasure()		{return __tapeMeasure;}
public:
	void	OnResetDevice()
	{
		__select.GetEffectPointer()->OnResetDevice();
		__showNormal.GetEffectPointer()->OnResetDevice();
		__effectPtr->OnResetDevice();
	}
	void	OnLostDevice()
	{
#ifndef __WRITE_FILES__
		__select.GetEffectPointer()->OnLostDevice();
		__showNormal.GetEffectPointer()->OnLostDevice();
#endif	// __WRITE_FILES__
		__effectPtr->OnLostDevice();
	}
	void	DestroyAll()
	{
		CDXUTXFileMesh::Destroy();
		__Destroy();
		__meshLoader.Destroy();
		__select.Destroy();
		__showNormal.Destroy();
		__tapeMeasure.Destroy();
	}
#endif	// All in One
#if 3	// follow
public:
	// selectedPrIDArray 에 있는 각 점이, mesh0 에 있는 대응하는 점을 향해 bDir(True) 방향으로 움직인다;
	// TO DO LIST: Normal 도 바뀌어야 하고, 움직이는 정도도 조절할 수 있어야 한다.
	void	follow(const kMesh& mesh0, bool bDir);
#endif	// follow
	void	__Set__next_prID(
		DWORD	prID,
		std::vector<DWORD>&	next_prID,
		const DWORD*	pPointReprFlag
	);
	void	setColorIsometric(
		DWORD	prID,
		bool	shiftKeyPressed,
		unsigned	nChannel,
		unsigned	nStep
	);

	// 메쉬를 변형한 후, 변화된 부피를 계산한다.
	void	ChangeMesh(
#if 1
		float	fNormalAmount,
		unsigned	nChannel
#else
		bool	plus,
		bool	shiftKeyPressed,
		unsigned	nChannel,
		unsigned	nStep
#endif
	);

	void	brushInit();
	void	brush();
	static bool	IntersectTriangle(
		const D3DXVECTOR3&	orig,
		const D3DXVECTOR3&	dir,
		const D3DXVECTOR3&	v0,
		const D3DXVECTOR3&	v1,
		const D3DXVECTOR3&	v2,

		FLOAT*	t,
		FLOAT*	u,
		FLOAT*	v
	);
#if 0	// kSelect
	void	selection(
		int	g__iMouseX,
		int	g__iMouseY,
		bool	g__bDrag,
		bool	shiftKeyPressed,
		int	g__iDragBeginX,
		int	g__iDragBeginY,

		UINT&	g__NSelectedPoints,	// [OUT]
		DWORD&	g__prID			// [OUT]
	);
#endif	// kSelect
private:
	LPDIRECT3DVERTEXBUFFER9		__intersectionVertexBufferPtr;
public:
	const LPDIRECT3DVERTEXBUFFER9	intersectionVertexBufferPtr() const	{return __intersectionVertexBufferPtr;}
	LPDIRECT3DVERTEXBUFFER9		intersectionVertexBufferPtr()		{return __intersectionVertexBufferPtr;}
	static HRESULT	Pick(
		const LPD3DXMESH	meshPtr,

		// [IN]
		bool	g_bUseD3DXIntersect,
		bool	g_bAllHits,
		const D3DXVECTOR3&	vPickRayOrig,
		const D3DXVECTOR3&	vPickRayDir,

		// [OUT]
		int&		nearestIntersectionIndex,
		DWORD&		dwNumIntersection,
		INTERSECTION*	intersectionArray
	);

#if 1	// 렌더링
protected:
	// 적어도 아래 세 행렬은 매 프레임마다 설정해야 한다!
	D3DXMATRIX	__worldMatrix;
	D3DXMATRIX	__viewMatrix;
	D3DXMATRIX	__projectionMatrix;
private:
	D3DXMATRIX	__worldViewProjectionMatrix;	// __worldMatrix * __viewMatrix * __projectionMatrix

	D3DXMATRIX	__cameraRotationMatrix;		// 카메라 자체의 월드좌표에 대한 회전;
	D3DXMATRIX	__cameraRotationMatrixInverse;	// 월드좌표에서의 카메라 __upDir, __rightDir, __viewDir 을 결정
	D3DXVECTOR3	__upDir;
	D3DXVECTOR3	__rightDir;
	D3DXVECTOR3	__viewDir;
public:
	void	setMatrix(const D3DXMATRIX& worldMatrix, const D3DXMATRIX& viewMatrix, const D3DXMATRIX& projectionMatrix, const D3DXMATRIX& cameraRotationMatrix)
	{
		__worldMatrix = worldMatrix;
		__viewMatrix = viewMatrix;
		__projectionMatrix = projectionMatrix;
		__worldViewProjectionMatrix = worldMatrix * viewMatrix * projectionMatrix;

		if (D3DXMatrixInverse(&__cameraRotationMatrixInverse, NULL, &cameraRotationMatrix))
			__cameraRotationMatrix = cameraRotationMatrix;
		else {	// 역행렬이 존재하지 않을 때
			D3DXMatrixIdentity(&__cameraRotationMatrix);
			D3DXMatrixIdentity(&__cameraRotationMatrixInverse);
		}
		D3DXVECTOR3	vLocalUp = D3DXVECTOR3(0, 1, 0);
		D3DXVECTOR3	vLocalRight = D3DXVECTOR3(1, 0, 0);
		D3DXVECTOR3	vLocalAhead = D3DXVECTOR3(0, 0, 1);
		D3DXVec3TransformCoord(&__upDir, &vLocalUp, &__cameraRotationMatrixInverse);
		D3DXVec3TransformCoord(&__rightDir, &vLocalRight, &__cameraRotationMatrixInverse);
		D3DXVec3TransformCoord(&__viewDir, &vLocalAhead, &__cameraRotationMatrixInverse);
	}
#endif	// 렌더링

#if 1	// 2D 와의 통신을 위한 함수
	// override
	virtual int	getValue(TYPE_XXX type, int subType, void* dataPtr);
	virtual int	setValue(TYPE_XXX type, int subType, const void* dataPtr);
#endif	// 2D 와의 통신을 위한 함수

#if 1	// 길이 측정
	MEASURE_PTR_HASH	__measure_ptr_dic;
public:
#if 1	// 뼈
	int	getPoint(DWORD faceID, float baryU, float baryV, D3DXVECTOR3& P, D3DXVECTOR3* P0Ptr = 0) const;
	int	getPoint(const POINT_DEF& point_def, D3DXVECTOR3& P, D3DXVECTOR3* P0Ptr = 0) const;
	int	getPoint(const INTERSECTION& intersection, D3DXVECTOR3& P, D3DXVECTOR3* P0Ptr = 0) const;
#else	// 뼈/!뼈
	int	getPoint(DWORD faceID, float baryU, float baryV, D3DXVECTOR3& P) const;
	int	getPoint(const POINT_DEF& point_def, D3DXVECTOR3& P) const;
	int	getPoint(const INTERSECTION& intersection, D3DXVECTOR3& P) const;
#endif	// !뼈
	int	getNormal(DWORD faceID, float baryU, float baryV, D3DXVECTOR3& N) const;
	int	getNormal(const POINT_DEF& pointDef, D3DXVECTOR3& N) const;
	int	getNormal(const INTERSECTION& intersection, D3DXVECTOR3& N) const;
	int	ResetTapeMeasure();
	void	setMeasureItem(MEASURE_ITEM& m_item, float* lengthPtr = 0, REF_POINT* refPointPtr = 0);
	const MEASURE_PTR_HASH&	measure_ptr_dic() const	{return __measure_ptr_dic;}
	int	setMeasure(TAPE_MEASURE_TYPE_T type, float* lengthPtr = 0);
	void	setMeasure();

public:
	// 계측:
	//	type:= TYPE__TAPE_MEASURE
	//	subType:= TAPE_MEASURE__TYPE 중의 하나
	//	dataPtr:= subType 에 해당하는 길이(둘레의 길이 또는 순수한 길이)에 대한 포인터(float*)
//	int	getValue(TYPE_XXX type, int subType, void* dataPtr);
	// 계측:
	//	type:= TYPE__TAPE_MEASURE
	//	subType:= TAPE_MEASURE__TYPE 중의 하나
	//	dataPtr:= subType 에 해당하는 부위를 표시할 지를 말해주는 포인터(bool*)
	//	주) 아무것도 나타나지 않게 하려면
	//	subType = TAPE_MEASURE__UNDEF, *((bool*) dataPtr) = true 로 설정한 후 이 함수를 호출한다:
	//		bool	set = true;
	//		setValue(TYPE__TAPE_MEASURE, TAPE_MEASURE__UNDEF, &set);
//	int	setValue(TYPE_XXX type, int subType, const void* dataPtr);
public:
	// 표시할 때
	int	kMesh::Render__TAPE_MEASURE() const;
#endif	// 길이 측정

#if 1	// 체중
private:
	// 어딘가에서 다음 3 개의 값들을 설정한다.
#define N_WEIGHT_MEASURE	3	// WEIGHT_MEASURE__YYY: 3
	float	__weightValues[N_WEIGHT_MEASURE];
	int	getWeight(WEIGHT_MEASURE__TYPE weightMeasureType, float* weightPtr);
#endif	// 체중

#if 4	// 특정 부위
private:
	bool	__overlap;
private:
	kArea::DEFINED_AREA_DIC		__defined_area_dic;
public:
	const kArea::DEFINED_AREA_DIC&	defined_area_dic() const	{return __defined_area_dic;}
private:
	kArea	__area;
public:
	const kArea&	area() const	{return __area;}
	kArea&		area()		{return __area;}
public:
#if 1
	void	ShowDefinedArea();
#else
	void	ShowDefinedArea()	{__area.setColor(*this);}
#endif
#endif	// 특정 부위

#if 1	// 모눈
public:
	void	DrawGrid() const;
#endif	// 모눈

#if 1	// 등고선
protected:
	kIsometric	__isometric;
public:
	void	ShowIsometric() const;
#endif	// 등고선

#if 1	// 모델
private:
	kModel		__model;
public:
	const kModel&	getModelRef() const	{return __model;}
	void	ShowModel() const;
#endif	// 모델

#if 1	// 시뮬레이션
#if 1	// 색깔템플릿
private:
	int	__indexTemplateColor;
	bool	__setTemplateColorDone;	// 이미 색깔변화가 반영되었는 지 말해준다.
public:
	void	IncIndexTemplateColor();
	void	DecIndexTemplateColor();
public:
	void	ShowTemplateColor();
#endif	// 색깔템플릿
private:
	kSimulation	__simulation;
	int	__execSimulation(SIMULATION_EXEC_TYPE execType);
public:
	const kSimulation&	simulation() const	{return __simulation;}
	kSimulation&		simulation()		{return __simulation;}
private:
	// 표준체형으로 갔을 때의 총 부피 변화량을 구한다.
	void	__standard_total_volume_change(float& totalVolumeChange);
	// 표준체형으로 변환하기위해 normal 을 변경한다.
	void	standard_set_normal();
#endif	// 시뮬레이션

#if 1	// 조작툴(manipulator)
private:
	MANIPULATOR_TYPE_T	__manipulatorType;
	kManipulator*		__manipulatorPtr;
public:
	MANIPULATOR_TYPE_T	manipulatorType() const		{return __manipulatorType;}
	const kManipulator*	manipulatorPtr() const		{return __manipulatorPtr;}
	kManipulator*		manipulatorPtr()		{return __manipulatorPtr;}

public:
	void	RenderManipulator() const;
	// 화살표의 시작점(0) 또는 끝점(1) 선택되었는 지를 말해준다.
	int	ManipulatorPick(const POINT& ptCursor);

#if 1	// normal manipulator
private:
	kNormalManipulator	__normal_manipulator;
public:
	const kNormalManipulator&	normal_manipulator() const	{return __normal_manipulator;}
	kNormalManipulator&		normal_manipulator()		{return __normal_manipulator;}
private:
	int	__setNormalManipulator(int n);	// 둘레측정에 사용되는...
public:
	void	SetNormalManipulator(const INTERSECTION& intersection, const D3DXVECTOR3& nDir);	// 테스트용
#endif
#if 1	// point manipulator
private:
	kPointManipulator	__point_manipulator;
public:
	const kPointManipulator&	point_manipulator() const	{return __point_manipulator;}
	kPointManipulator&		point_manipulator()		{return __point_manipulator;}
private:
	int	__setPointManipulator(int n);
public:
	void	SetPointManipulator(const INTERSECTION& intersection);	// 테스트용
public:
	// 스크린 좌표계의 좌표 p2 가 마우스가 위치한 곳에 있는 지...
	static bool	isPointPicked(const D3DXVECTOR3& p2, const POINT& ptCursor, int delta);
#endif	// point manipulator
#if 1	// line manipulator
private:
	kLineManipulator	__line_manipulator;
public:
	const kLineManipulator&		line_manipulator() const	{return __line_manipulator;}
	kLineManipulator&		line_manipulator()		{return __line_manipulator;}
private:
	int	__setLineManipulator(int n);
#endif
#endif	// 조작툴(manipulator)

#if 1	// 체성분
private:
	kBodyElement	__bodyElement;
public:
	// BODY_ELEMENT__SUBTYPE 이 BODY_ELEMENT__VS 인 경우에 해당 영역을 특정 색깔로 표시하기 위해...
	void	ShowBodyElement();
#endif	// 체성분
#if 1	// ShowLowerFace
private:
	// kState::LOWER_FACE
	static D3DVERTEXELEMENT9	LowerFaceElement[5];
public:
	void	ShowLowerFace(
		const std::vector<DWORD>&	faceIDArray
	) const;
	void	ShowLower2Face(
		const std::vector<D3DXVECTOR3>&	faceIDArray,
		const D3DXMATRIX&	mWorld,
		const D3DXMATRIX&	mWorldViewProjection
	) const;
	void	ShowLower2Face(
		const std::vector<LOWER_FACE>&	lowerFaceArray,
		const D3DXMATRIX&	mWorld,
		const D3DXMATRIX&	mWorldViewProjection
	) const;
#endif	// ShowLowerFace

#if 1	// 2D단면
private:
	int	__2dProjectNumPoints;
	LPDIRECT3DVERTEXBUFFER9		__2dProjectVertexBufferPtr;
	int	__2dInterpolatedProjectNumPoints;
	LPDIRECT3DVERTEXBUFFER9		__2dInterpolatedProjectVertexBufferPtr;
	int	__2dFillProjectNumPoints;
	LPDIRECT3DVERTEXBUFFER9		__2dFillProjectVertexBufferPtr;
public:
	void	set2dProjectVertexBuffer();
	int	_2dProjectNumPoints() const	{return __2dProjectNumPoints;}
	const LPDIRECT3DVERTEXBUFFER9		_2dProjectVertexBufferPtr() const	{return __2dProjectVertexBufferPtr;}
	int	_2dInterpolatedProjectNumPoints() const	{return __2dInterpolatedProjectNumPoints;}
	const LPDIRECT3DVERTEXBUFFER9		_2dInterpolatedProjectVertexBufferPtr() const	{return __2dInterpolatedProjectVertexBufferPtr;}
	int	_2dFillProjectNumPoints() const	{return __2dFillProjectNumPoints;}
	const LPDIRECT3DVERTEXBUFFER9		_2dFillProjectVertexBufferPtr() const	{return __2dFillProjectVertexBufferPtr;}
#endif	// 2D단면

#if 1	// ColorBand
private:
	LPDIRECT3DVERTEXBUFFER9		__colorBandVertexBufferPtr;
	LPD3DXLINE	__indicatorLinePtr;	// indicator 라인용 ID3DXLine 에 대한 포인터
public:
	void	ShowColorBand() const;
#endif	// ColorBand
#if 1	// Save/Restore Color;
	// 색깔에 변화를 주는 툴(체성분영역표시, 영역표시, 성형, 색깔템플릿 표시)을 사용하기 전에
	// 모델에 입혀진 색깔을 보존하고, 툴을 사용후에는 원래 색깔로 되돌려야 한다.
public:
	void	SaveColor();
	void	RestoreColor();
#endif	// Save/Restore Color

#if 1	// CollectFaces
public:
	// PLANE_DEF 에 결정되는 평면의 아래쪽에 있는 Face 들의 목록을 얻는다;
	// 아래의 __CollectLowerFaces 함수를 반복적으로 호출한다:
	//	planeDef 로부터 startFaceID 를 결정한후,
	//	nextFaceIDArray 에 있는 Face 의 갯수가 0이 될 때까지
	//	__CollectLowerFaces 함수를 반복적으로 호출한다.
	static void
	CollectLowerFaces(
		const DWORD*		faceIDArray,		// [IN]; 이 Face 들 중에서
								//	이 값이 NULL 이면 메쉬에 있는 모든 Face 들을 조사한다.
		const PLANE_DEF&	planeDef,		// [IN]; 이 평면에 대해 아래쪽에 있는
		std::vector<DWORD>&	lowerFaceIDArray,	// [OUT]; Face 들의 목록

		const kMesh*		meshPtr			// [IN]; 메쉬의 Vertex, Face 들의 정보을 참고해서...
	);
	static void
	CollectLowerFaces(
		const DWORD*		faceIDArray,		// [IN]; 이 Face 들 중에서
								//	이 값이 NULL 이면 메쉬에 있는 모든 Face 들을 조사한다.
		const PLANE_DEF&	planeDef,		// [IN]; 이 평면에 대해 아래쪽에 있는
		DWORD*			lowerFaceIDArray,	// [OUT]; Face 들의 목록

		const kMesh*		meshPtr			// [IN]; 메쉬의 Vertex, Face 들의 정보을 참고해서...
	);
	static void
	__CollectLowerFaces(
		const DWORD*		faceIDArray,		// [IN]; 이 Face 들 중에서
								//	이 값이 NULL 이면 메쉬에 있는 모든 Face 들을 조사한다.
		const D3DXVECTOR3&	P,			// [IN]; (P, nDir) 에 의해 결정되는 평면에 대해 아래쪽에 있는
		const D3DXVECTOR3&	nDir,
		DWORD*			lowerFaceIDArray,	// [OUT]; Face 들의 목록

		const kMesh*		meshPtr,		// [IN]; 메쉬의 Vertex, Face 들의 정보을 참고해서
		DWORD			startFaceID,		// [IN]; 아래쪽에 있는 이 Face 에서 시작해서
		std::vector<DWORD>&	nextFaceIDArray,	// [OUT]; 아래쪽에 있는 다음 Face 들의 목록을 구한다.
		DWORD*			donePrIDArray,		// [IN/OUT]; 이미 테스트를 끝낸 Vertex 들의 목록과
		DWORD*			doneFaceIDArray		// [IN/OUT]; Face 들의 목록을 갱신한다.
	);
	static void
	kMesh::CollectBetweenFaces(
		const PLANE_DEF&	planeDef,		// [IN]; 이 평면과
		const POINT_DEF&	pointDef,		// [IN]; 이 점에 첫 번째 평면의 법선벡터에 의해서 결정되는 평면사이에 있는
		std::vector<DWORD>&	betweenFaceIDArray,	// [OUT]; Face 들의 목록

		const kMesh*		meshPtr			// [IN]; 메쉬의 Vertex, Face 들의 정보을 참고해서...
	);
	static void
	kMesh::CollectBetweenFaces(
		const PLANE_DEF&	planeDef,		// [IN]; 이 평면과
		const POINT_DEF&	pointDef,		// [IN]; 이 점에 첫 번째 평면의 법선벡터에 의해서 결정되는 평면사이에 있는
		DWORD*			betweenFaceIDArray,	// [OUT]; Face 들의 목록

		const kMesh*		meshPtr			// [IN]; 메쉬의 Vertex, Face 들의 정보을 참고해서...
	);
	static void
	kMesh::CollectBetweenFaces(
		const PLANE_DEF&	planeDef0,		// [IN]; 이 평면과
		const PLANE_DEF&	pointDef1,		// [IN]; 이 평면 사이에 있는
		std::vector<DWORD>&	betweenFaceIDArray,	// [OUT]; Face 들의 목록

		const kMesh*		meshPtr			// [IN]; 메쉬의 Vertex, Face 들의 정보을 참고해서...
	);
	static void
	kMesh::CollectBetweenFaces(
		const PLANE_DEF&	planeDef0,		// [IN]; 이 평면과
		const PLANE_DEF&	pointDef1,		// [IN]; 이 평면 사이에 있는
		DWORD*			betweenFaceIDArray,	// [OUT]; Face 들의 목록

		const kMesh*		meshPtr			// [IN]; 메쉬의 Vertex, Face 들의 정보을 참고해서...
	);

	void	elongate();
#endif	// CollectFaces

#if 1	// 모든 계측부위 표시
	void	SetAllMeasure(const int* valPtr = 0);
	void	SetAllMeasureDirty(const int* valPtr = 0);
	void	DisplayAllMeasure(CDXUTTextHelper& txtHelper) const;
#endif	// 모든 계측부위 표시

	struct COLLECT_CALLBACK_ARGUMENT
	{
		float	brush_radius;
		D3DVIEWPORT9	viewport;
		D3DXVECTOR3	P0;
		D3DXVECTOR3	nDir;

#if 1
		// D3DXVec3Project 함수 호출을 최소화한다; 행렬계산을...
		D3DXMATRIX	m;	// object space -> projection space; D3DXVec3TransformCoord 에서 사용
		D3DXVECTOR2	_p0;	// projection space 의 좌표
#else
		D3DXVECTOR2	P02;		// brush 중심의 screen space 좌표
		// 주어진 점의 object space 에서의 좌표를 screen space 좌표로 변환할 때 사용;
		// D3DXVec3Project 함수
		D3DXMATRIX	projectionMatrix;
		D3DXMATRIX	viewMatrix;
		D3DXMATRIX	worldMatrix;
#endif
	};
	typedef int	(*COLLECT_CALLBACK_POINTER)(const kMesh* meshPtr, const CFace& face, const void* voidPtr, DWORD* donePrIDArray);

	static void	CollectFaces(
		const kMesh*	meshPtr,	// [IN]; 메쉬의 Vertex, Face 들의 정보을 참고해서...
		const DWORD*	faceIDArray0,	// [IN]; 이 Face 들 중에서
						//	이 값이 NULL 이면 메쉬에 있는 모든 Face 들을 조사한다.
		const COLLECT_CALLBACK_POINTER	callback_ptr,
		const void*		voidPtr,

		DWORD		startFaceID,	// [IN]; 여기서 시작해서...

		std::vector<DWORD>&	faceIDArray,	// [OUT]; Face 들의 목록
		std::vector<DWORD>&	pointReprIDArray
	);
	static void
	__CollectFaces(
		const kMesh*	meshPtr,	// [IN]; 메쉬의 Vertex, Face 들의 정보을 참고해서
		const DWORD*	faceIDArray0,	// [IN]; 이 Face 들 중에서
						//	이 값이 NULL 이면 메쉬에 있는 모든 Face 들을 조사한다.
		const COLLECT_CALLBACK_POINTER	callback_ptr,
		const void*		voidPtr,

		DWORD		faceID,	// [IN]; 아래쪽에 있는 이 Face 에서 시작해서

//		std::vector<DWORD>&	faceIDArray,	// [OUT]; Face 들의 목록

		DWORD*	next_activeFaceIDArray,	// [OUT]; 아래쪽에 있는 다음 Face 들의 목록을 구한다.
		DWORD*	doneFaceIDArray,		// [IN/OUT]; Face 들의 목록을 갱신한다.
		DWORD*	donePrIDArray
	);
	static void	CollectFacesBelowIron(
		const kMesh*	meshPtr,	// [IN]; 메쉬의 Vertex, Face 들의 정보을 참고해서...
		const DWORD*	faceIDArray0,	// [IN]; 이 Face 들 중에서
						//	이 값이 NULL 이면 메쉬에 있는 모든 Face 들을 조사한다.
		const COLLECT_CALLBACK_POINTER	callback_ptr,
		const void*		voidPtr,

		std::vector<DWORD>&	faceIDArray,	// [OUT]; Face 들의 목록
		std::vector<DWORD>&	pointReprIDArray
	);


	// Texture 를 표시하는 메쉬는 변경하지 않는다!
	void	MakeVertexSmooth(UINT niter = 1);

	void	PlotPoints(const CGrowableArray<D3DXVECTOR3>& points, float pointSize = 5.f, const D3DXVECTOR4* pointColor_ptr = NULL) const;
#if 1	// SHIFT
private:
	float	__shift;	// 물체의 좌측면이 화면의 좌측면에서 얼마나 벗어났는 지;
				// 기본값은 물체의 중앙이 화면의 중앙에 위치하도록 하는 것으로
				// 이에 해당하는 값은 (W - (H/h)w)/2 이다.
				// 여기서
				//	H/W: 화면의 높이/넓이
				//	h/w: 물체의 높이/넓이
				// 로 물체를 로딩할 때 설정한다; kMesh::setAll 에서...
public:
	float	GetShift() const	{return __shift;}
	void	SetShift(float shift_amount);
	void	SetShift(SHIFT_ALIGN_TYPE align_type);
#endif	// SHIFT
#if 1	// Voxel
public:
	void	RenderVoxel() const;
#endif	// Voxel

#if 1	// 체형변화 시뮬레이션
	// 부피 단위(liter)
private:
	// 체형변화에 필요한 준비를 한다;
	//	1. CPointRepr::Area2Flag 플래그의 초기화와 설정
	//	2. CFace::Flag::active 플래그의 초기화와 설정
	//	3. CPointRepr::UVAL::fval 의 설정(CPointRepr::Area2Flag::move 가 True 인 경우에 한해서)
	int	__whole_body_simulation_initialize(float scale);
	// 몸을 구성하는 각 Vertex 를 "적절히" 움직인다.
	int	__whole_body_simulation_do(float scale);	// scale < 0: 살 빼기, scale > 0:= 살 찌우기
	// __vertexBufferPtr 에 저장된 Vertex 에 일어난 변화를 다른 곳에도 영향을 준다.
	int	__whole_body_simulation_finalize();
public:
	int	simulateWholeBody(float scale, float* dV_ptr = 0, float* V_ptr = 0);
public:
	// 원래 부피로부터...
	int	simulateWholeBody0(
		bool	show,
		float	dV0,		// dV0:= 원래 부피로부터의 변화량
		float&	scale /* OUT */,
		float*	dV_ptr = 0,	// dV_ptr:= 실제 변화량
		float	dV_percent_error0 = 1.f,	// dV_percent_error0:= 오차 한계(단위 %)
		float*	dV_percent_error_ptr = 0
	);
	// 현재 부피로부터...
	int	simulateWholeBody1(
		bool	show,
		float	dV0,		// dV0:= 현재 부피로부터 변화량
		float&	scale /* OUT */,
		float*	dV_ptr = 0,
		float	dV_percent_error0 = 1.f,
		float*	dV_percent_error_ptr = 0
	);
public:
	struct SCALE_CACHE
	{
		bool	found;
		float	scale;
#if 1	// Cache 에 저장
		float	dV;
		std::vector<D3DXVECTOR3>	vertexArray;
#endif	// Cache 에 저장

		SCALE_CACHE() : found(false) {}
	};
private:
	std::vector<SCALE_CACHE>	__scale_cache_list;	// 크기는 VOLUME_{MIN,STEP,MAX} 에 의해서 결정
public:
	int	simulateWholeBody2(bool show, int index, float& dV /* OUT */, float dV_percent_error0 = 1.f);
#if 1	// Simulation Cache
private:
	typedef std::map<int, float>	SIMULATION_CACHE_HASH;	// scale -> dV; scale 을 정수로 저장
	SIMULATION_CACHE_HASH	__simulationCache;
#endif	// Simulation Cache
#endif	// 체형변화 시뮬레이션
#if 1	// 모델스냅샷
private:
	struct SNAPSHOT
	{
		bool	saved;
		std::vector<D3DXVECTOR3>	data;

		SNAPSHOT() : saved(false) {}
	};
	std::vector<SNAPSHOT>	__snapshots;
public:
	int	snapshot_save(int index, bool overwrite = false);
	int	snapshot_load(int index);
#endif	// 모델스냅샷

#if 1	// 마지막 주문; 6. 3D 단면
private:
	OBJECT_TYPE	__lastObjectType;
	SHADING_TYPE	__lastShadingType;
public:
	bool	measureDone;	// 3D 단면을 설정한 후, 계측을 시작했는 지...
	int	Set3DCrossSection(bool);
#endif	// 마지막 주문; 6. 3D 단면
};
