#pragma once

#include <vector>
#include <set>
#include <d3dx9.h>
#include <mesh/kEnum.h>
#include <mesh/kGrid.h>

class CModelViewerCamera2;
class kMesh;
class kSelect {
private:
	const kMesh*	__meshPtr;	// 선택하는 대상에 대한 정보를 담은 클래스에 대한 포인터:
				//	Effect, Vertex Declaration, Vertex Buffer, ...
public:
#if 3	// porting_DxViewer
	kSelect();
private:
	void	__init();
public:
	int	setAll(const kMesh* meshPtr, const LPDIRECT3DDEVICE9 devicePtr, LPCTSTR effectSourceFilePtr);
#else
	kSelect(
		const kMesh*	meshPtr,
		const LPDIRECT3DDEVICE9		devicePtr,
		LPCTSTR	pSrcFile,
		bool*	pbOk = NULL
	);
#endif
	virtual ~kSelect();
	void	Destroy();
private:
	LPDIRECT3DDEVICE9	__devicePtr;
	LPD3DXEFFECT	m__pEffect;

	UINT	m__MaximumNumberOfPoints;
	UINT	m__NumberOfPoints;
	LPDIRECT3DVERTEXBUFFER9		m__pVertexBuffer;
	LPDIRECT3DVERTEXDECLARATION9	m__pDeclaration;
public:
	static UINT	NUMBER_OF_POINTS__UNIT;
	static UINT	Stride;
	static D3DVERTEXELEMENT9	Element[2];

public:
	HRESULT	ResetVertexBuffer(UINT NumberOfPoints, LPDIRECT3DVERTEXBUFFER9* ppVertexBuffer = NULL);
	void	ShowPoints(const D3DXMATRIX& pMatWorldViewProjection, FLOAT pointSize, const D3DXVECTOR4& cPointColor);
	void	ShowVertex(
		const std::vector<DWORD>&	prReprIDArray,
		FLOAT			pointSize = 5.f,
		const D3DXVECTOR4&	pointColor = D3DXVECTOR4(1.f, 0.f, 1.f, 1.f)
	) const;
	void	ShowEdge(
		const std::vector<DWORD>&	edgeIDArray,
		const D3DXVECTOR4&	color
	) const;
	void	ShowFace(
		const std::vector<DWORD>&	faceIDArray
	) const;

	LPD3DXEFFECT	GetEffectPointer(void)	{return this->m__pEffect;}
	LPDIRECT3DVERTEXBUFFER9	GetVertexBufferPointer(void)	{return this->m__pVertexBuffer;}
	LPDIRECT3DVERTEXDECLARATION9	GetDeclarationPointer(void)	{return this->m__pDeclaration;}
	UINT	GetNumberOfPoints(void)	{return this->m__NumberOfPoints;}
	void	SetNumberOfPoints(UINT NumberOfPoints)	{this->m__NumberOfPoints = NumberOfPoints;}

	void	DrawRectangle(int iUpperRightX, int iUpperRightY, int iLowerLeftX, int iLowerLeftY);
	void	DrawCircle(unsigned centerX, unsigned centerY, FLOAT fRadius, FLOAT pointSize, D3DXVECTOR4* cPointColor);
private:
	std::set<DWORD>	__selectedPrIDSet;
public:
	const std::set<DWORD>&	selectedPrIDSet() const	{return __selectedPrIDSet;}
	std::set<DWORD>&		selectedPrIDSet()		{return __selectedPrIDSet;}
	void	GetSelectedFaceIDList(std::vector<DWORD>& selectedFaceIDList);

private:
	// Brush 안에 속한 점들에 대한 정보;
	// 일종의 Cache 로 충돌 검사 후 다시 그 정보를 얻게하지않기 위하여...
	struct BRUSHPOINT {
		DWORD	flag;	// 0: 어떤 정보도 모으지 않았다. (기본값)
				// 1: 원 안에 들어오지 않았다.
				// 2: 원 안에 있다.
				// 3: 충돌 테스트까지 끝났다.
		// 다음 두 값은 flag = 2 일 때만 의미있다.
		FLOAT	d2;
		FLOAT	z;
	};
public:
	static bool
	IntersectTriangle(
		const D3DXVECTOR3&	orig,
		const D3DXVECTOR3&	dir,
		const D3DXVECTOR3&	v0,
		const D3DXVECTOR3&	v1,
		const D3DXVECTOR3&	v2,

		FLOAT*	t,
		FLOAT*	u,
		FLOAT*	v
	);
#if 0
	HRESULT	Pick(
		bool	bBrush,	// 브러쉬 모드인지 아니면 단순 Picking 모드인지
		FLOAT	brushRadius,
		bool	isShiftKeyPressed,

		const D3DXMATRIX&	worldMatrix,
		const D3DXMATRIX&	viewMatrix,
		const D3DXMATRIX&	projectionMatrix,

		// Point 의 크기가 5라고 가정; 해당 점이 1픽셀, 좌우 2픽셀도 역시 해당 점의 영역이라고 가정
		int	iMouseX,
		int	iMouseY,
		bool	bDrag,
		int	iDragBeginX,
		int	iDragBeginY,

		DWORD&	selectedPrID
	);
#endif
#if 5	// contour
private:
	LPDIRECT3DVERTEXBUFFER9		__contourVertexBufferPtr;
	LPDIRECT3DVERTEXDECLARATION9	__contourDeclarationPtr;
	void		__contourInit();
	unsigned	__contourLines;
public:
	static unsigned	MAX_SEGMENT;
#if 1
	void	DrawContour(
		int	__2dProjectNumPoints,
		LPDIRECT3DVERTEXBUFFER9	__2dProjectVertexBufferPtr,
		FLOAT	pointSize,
		const D3DXVECTOR4&	pointColor,

		int	__2dInterpolatedProjectNumPoints,
		LPDIRECT3DVERTEXBUFFER9	__2dInterpolatedProjectVertexBufferPtr,
		FLOAT	interpolatedPointSize,
		const D3DXVECTOR4&	interpolatedPointColor,

		int	__2dFillProjectNumPoints,
		LPDIRECT3DVERTEXBUFFER9	__2dFillProjectVertexBufferPtr,
		FLOAT	fillPointSize,
		const D3DXVECTOR4&	fillPointColor
	);
#else
	void	DrawContour(
		TAPE_MEASURE_METHOD_T	method,

		bool	close,
		int	numSegments,
		const D3DXVECTOR3*	pointPtr,
		const D3DXVECTOR3&	nDir,
		const D3DXVECTOR3&	xDir,
		FLOAT		pointSize,
		const D3DXVECTOR4&	cPointColor,
		FLOAT	interpolatedPointSize,
		const D3DXVECTOR4&	interpolatedPointColor
	);
#endif

	void	DrawGrid(const kGrid& grid, const D3DXMATRIX& worldMatrix, const D3DXMATRIX& viewMatrix, const D3DXMATRIX& projectionMatrix) const;
	void	ShowAngle(const D3DXMATRIX& worldMatrix, const D3DXMATRIX& viewMatrix, const D3DXMATRIX& projectionMatrix, const POINT& startPoint, const POINT& endPoint, float zDepth);
#endif

	void	selection(kMesh& mesh, const CModelViewerCamera2& camera);
};
