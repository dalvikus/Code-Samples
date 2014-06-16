#include <assert.h>
#include <mesh/kSelect.h>
#include <mesh/kMesh.h>
#include <mesh/kPick.h>

#if 3	// porting_DxViewer
kSelect::kSelect()
{
	__init();
}

void	kSelect::__init()
{
	m__pEffect = NULL;
	m__pVertexBuffer = NULL;
	m__pDeclaration = NULL;

#if 1	// contour
	__contourDeclarationPtr = 0;
	__contourVertexBufferPtr = 0;
	__contourLines = 0;
#endif
}

unsigned	kSelect::MAX_SEGMENT = 10000;
void
kSelect::__contourInit()
{
	HRESULT	hResult;
	D3DVERTEXELEMENT9	element[2] = {
		{
			0,
			0 * sizeof(D3DXVECTOR3),
			D3DDECLTYPE_FLOAT3,
			D3DDECLMETHOD_DEFAULT,
			D3DDECLUSAGE_POSITION,
			0
		},

		D3DDECL_END()
	};
	hResult = __devicePtr->CreateVertexDeclaration(element, &__contourDeclarationPtr);

	hResult = __devicePtr->CreateVertexBuffer(
		(2 * MAX_SEGMENT) * sizeof(D3DXVECTOR3),	// 한 라인당 2개의 포인트가 필요하다.
		D3DUSAGE_WRITEONLY,
		0,
		D3DPOOL_MANAGED,
		&__contourVertexBufferPtr,
		NULL
	);
	assert(D3D_OK == hResult);
}

int	kSelect::setAll(
	const kMesh*	meshPtr,
	const LPDIRECT3DDEVICE9		devicePtr,
	LPCTSTR				effectSourceFilePtr
)
{
	__meshPtr = meshPtr;

	HRESULT	hResult;

	assert(devicePtr);
	__devicePtr = devicePtr;

	m__pEffect = NULL;
	m__pVertexBuffer = NULL;
	m__pDeclaration = NULL;

	bool	bOk = true;
	bool	False = false;
do {
	assert(devicePtr);
	if (!devicePtr) {
		bOk = false;
		break;
	}

	m__NumberOfPoints = 0;
	m__MaximumNumberOfPoints = 0;
	hResult = devicePtr->CreateVertexDeclaration(kSelect::Element, &m__pDeclaration);
	assert(D3D_OK == hResult);

	DWORD	Flags = 0;
	LPD3DXBUFFER	pCompilationErrors = NULL;
	hResult = D3DXCreateEffectFromFile(
		devicePtr,	// LPDIRECT3DDEVICE9	devicePtr
		effectSourceFilePtr,	// LPCWSTR	pSrcFile
		NULL,		// const D3DXMACRO* pDefines
		NULL,		// LPD3DXINCLUDE pInclude
		Flags,		// DWORD Flags
		NULL,		// LPD3DXEFFECTPOOL pPool
		&m__pEffect,	// LPD3DXEFFECT* ppEffect
		&pCompilationErrors	// LPD3DXBUFFER* ppCompilationErrors
	);
	assert(D3D_OK == hResult);
	if (pCompilationErrors) {
		bOk = false;

//		LPCSTR	pErrorMessage = (LPCSTR) pCompilationErrors->GetBufferPointer();
		(void) pCompilationErrors->Release();	// ULONG

		break;
	}
} while (False);

	__contourInit();

	return 0;
}
#else
kSelect::kSelect(
	const kMesh*			meshPtr,
	const LPDIRECT3DDEVICE9		devicePtr,
	const LPCTSTR	pSrcFile,

	bool*			pbOk
) : __meshPtr(meshPtr)
{
	HRESULT	hResult;

	assert(devicePtr);
	__devicePtr = devicePtr;

	bool	bOk = true;
	bool	False = false;
do {
	assert(devicePtr);
	if (!devicePtr) {
		bOk = false;
		break;
	}

	m__NumberOfPoints = 0;
	m__MaximumNumberOfPoints = 0;
	hResult = devicePtr->CreateVertexDeclaration(kSelect::Element, &m__pDeclaration);
	assert(D3D_OK == hResult);

	DWORD	Flags = 0;
	LPD3DXBUFFER	pCompilationErrors = NULL;
	hResult = D3DXCreateEffectFromFile(
		devicePtr,	// LPDIRECT3DDEVICE9	devicePtr
		pSrcFile,	// LPCWSTR	pSrcFile
		NULL,		// const D3DXMACRO* pDefines
		NULL,		// LPD3DXINCLUDE pInclude
		Flags,		// DWORD Flags
		NULL,		// LPD3DXEFFECTPOOL pPool
		&m__pEffect,	// LPD3DXEFFECT* ppEffect
		&pCompilationErrors	// LPD3DXBUFFER* ppCompilationErrors
	);
	assert(D3D_OK == hResult);
	if (pCompilationErrors) {
		bOk = false;

//		LPCSTR	pErrorMessage = (LPCSTR) pCompilationErrors->GetBufferPointer();
		(void) pCompilationErrors->Release();	// ULONG

		break;
	}
} while (False);

	if (pbOk)
		*pbOk = bOk;
}
#endif

kSelect::~kSelect()
{
	Destroy();
}
void	kSelect::Destroy()
{
	if (m__pEffect) {
#ifdef _DEBUG
		ULONG	refCount = m__pEffect->Release();
		assert(0 == refCount);
#else
		m__pEffect->Release();
#endif
		m__pEffect = NULL;
	}
	if (m__pVertexBuffer) {
		m__MaximumNumberOfPoints = 0;
#ifdef _DEBUG
		ULONG	refCount = m__pVertexBuffer->Release();
		assert(0 == refCount);
#else
		m__pVertexBuffer->Release();
#endif
		m__pVertexBuffer = NULL;
	}
	if (m__pDeclaration) {
#ifdef _DEBUG
		ULONG	refCount = m__pDeclaration->Release();
		assert(0 == refCount);
#else
		m__pDeclaration->Release();
#endif
		m__pDeclaration = NULL;
	}

#if 1	// contour
	if (__contourDeclarationPtr) {
		__contourDeclarationPtr->Release();
		__contourDeclarationPtr = 0;
	}
	if (__contourVertexBufferPtr) {
		__contourVertexBufferPtr->Release();
		__contourVertexBufferPtr = 0;
	}
#endif
}

UINT	kSelect::NUMBER_OF_POINTS__UNIT = 100000;
UINT	kSelect::Stride = 1 * sizeof(D3DXVECTOR3);
/*
typedef struct D3DVERTEXELEMENT9 {
	WORD	Stream;
	WORD	Offset;
	BYTE	Type;
	BYTE	Method;
	BYTE	Usage;
	BYTE	UsageIndex;
} D3DVERTEXELEMENT9, *LPD3DVERTEXELEMENT9;
 */
D3DVERTEXELEMENT9	kSelect::Element[2] = {
	{
		0,
		0 * sizeof(D3DXVECTOR3),
		D3DDECLTYPE_FLOAT3,
		D3DDECLMETHOD_DEFAULT,
		D3DDECLUSAGE_POSITION,
		0
	},
	D3DDECL_END()
};


HRESULT
kSelect::ResetVertexBuffer(
	UINT	NumberOfPoints,
	LPDIRECT3DVERTEXBUFFER9*	ppVertexBuffer
)
{
	assert(NumberOfPoints);
	this->m__NumberOfPoints = NumberOfPoints;

	if (NumberOfPoints > this->m__MaximumNumberOfPoints) {
		if (this->m__pVertexBuffer) {
#ifdef _DEBUG
			ULONG	refCount = this->m__pVertexBuffer->Release();
			assert(0 == refCount);
#else
			this->m__pVertexBuffer->Release();
#endif
			this->m__pVertexBuffer = NULL;
		}
		UINT	q = NumberOfPoints / kSelect::NUMBER_OF_POINTS__UNIT;
		UINT	r = NumberOfPoints % kSelect::NUMBER_OF_POINTS__UNIT;
		this->m__MaximumNumberOfPoints = (r ? q + 1 : q) * kSelect::NUMBER_OF_POINTS__UNIT;

		HRESULT	hResult = __devicePtr->CreateVertexBuffer(
			kSelect::Stride * this->m__MaximumNumberOfPoints,
			D3DUSAGE_WRITEONLY,
			0,
			D3DPOOL_MANAGED,
			&this->m__pVertexBuffer,
			NULL
		);
#ifdef _DEBUG
		assert(D3D_OK == hResult);
#else
		if (hResult != D3D_OK)
			return (!D3D_OK);
#endif
	}

	if (ppVertexBuffer)
		*ppVertexBuffer = this->m__pVertexBuffer;

	return D3D_OK;
}

void	kSelect::ShowVertex(
	const std::vector<DWORD>&	prReprIDArray,
	FLOAT			pointSize,
	const D3DXVECTOR4&	pointColor
) const
{
	if (!prReprIDArray.size())
		return;
	// prReprIDArray 를 이용하여...
	LPDIRECT3DVERTEXBUFFER9		vertexBufferPtr = 0;
__try {
	unsigned	nPointRepr = (unsigned) prReprIDArray.size();
	// Vertex Buffer 를 만든 후...
	__devicePtr->CreateVertexBuffer(
		nPointRepr * sizeof(kMesh::D3DXVERTEX),
		D3DUSAGE_WRITEONLY,
		0,
		D3DPOOL_MANAGED,
		&vertexBufferPtr,
		NULL
	);
	// ...채운다.
	kMesh::D3DXVERTEX*	vertexPtr;
	vertexBufferPtr->Lock(0, 0, (void**) &vertexPtr, 0);
	for (size_t n = 0; n < nPointRepr; ++n) {
		const CPointRepr&	pointRepr = __meshPtr->GetPointReprPointer()[prReprIDArray.at(n)];
		vertexPtr[n].p = pointRepr.v.position;
		vertexPtr[n].p_ = pointRepr.v.position;
		vertexPtr[n].n = pointRepr.v.normal;
		vertexPtr[n].tu = 0.f;
		vertexPtr[n].tv = 0.f;
		vertexPtr[n].tx = -1.f;
		vertexPtr[n].c = D3DXVECTOR3(169.f / 255, 169.f / 255, 169.f / 255);	// dark grey
	}
	vertexBufferPtr->Unlock();

	__meshPtr->ShowVertex(vertexBufferPtr, __meshPtr->GetVertexDeclarationPointer(), sizeof(kMesh::D3DXVERTEX), nPointRepr, pointSize, pointColor);
} __finally {
	if (vertexBufferPtr) {
#ifdef _DEBUG
		ULONG	refCount = vertexBufferPtr->Release();
		assert(refCount == 0);
#else
		vertexBufferPtr->Release();
#endif
		vertexBufferPtr = 0;
	}
}
}

void	kSelect::ShowEdge(
	const std::vector<DWORD>&	edgeIDArray,
	const D3DXVECTOR4&	color
) const
{
	if (!edgeIDArray.size())
		return;
	// edgeIDArray 를 이용하여...
	LPDIRECT3DINDEXBUFFER9		edgeIndexBufferPtr = 0;
__try {
	unsigned	nEdge = (unsigned) edgeIDArray.size();
	// Index Buffer 를 만든 후...
	__devicePtr->CreateIndexBuffer(
		nEdge * sizeof(CEdge::LineIndex),
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX32,
		D3DPOOL_MANAGED,
		&edgeIndexBufferPtr,
		NULL
	);
	// ...채운다.
	CEdge::LineIndex*	edgeIndexPtr;
	edgeIndexBufferPtr->Lock(0, 0, (void**) &edgeIndexPtr, 0);
	for (DWORD n = 0; n < nEdge; ++n) {
		const CEdge&	edge = __meshPtr->GetEdgePointer()[edgeIDArray.at(n)];

		DWORD	iStart = edge.prStartID;
		iStart *= N_ELEMENT;
		DWORD	iEnd = edge.prEndID;
		iEnd *= N_ELEMENT;
		edgeIndexPtr[n].iStart = iStart;
		edgeIndexPtr[n].iEnd = iEnd;
	}
	edgeIndexBufferPtr->Unlock();

	__meshPtr->ShowEdge(edgeIndexBufferPtr, color, nEdge);
} __finally {
	if (edgeIndexBufferPtr) {
#ifdef _DEBUG
		ULONG	refCount = edgeIndexBufferPtr->Release();
		assert(refCount == 0);
#else
		edgeIndexBufferPtr->Release();
#endif
		edgeIndexBufferPtr = 0;
	}
}
}

void	kSelect::ShowFace(
	const std::vector<DWORD>&	faceIDArray
) const
{
	if (!faceIDArray.size())
		return;
	// faceIDArray 를 이용하여...
	LPDIRECT3DINDEXBUFFER9		faceIndexBufferPtr = 0;
__try {
	unsigned	nFace = (unsigned) faceIDArray.size();
	// Index Buffer 를 만든 후...
	__devicePtr->CreateIndexBuffer(
		nFace * sizeof(CFace::FaceIndex),
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX32,
		D3DPOOL_MANAGED,
		&faceIndexBufferPtr,
		NULL
	);
	// ...채운다.
	CFace::FaceIndex*	faceIndicesPtr;
	faceIndexBufferPtr->Lock(0, 0, (void**) &faceIndicesPtr, 0);
	for (size_t n = 0; n < nFace; ++n) {
		const CFace&	face = __meshPtr->GetFacePointer()[faceIDArray.at(n)];
		DWORD	i0 = face.prID0;
		i0 *= N_ELEMENT;
		DWORD	i1 = face.prID1;
		i1 *= N_ELEMENT;
		DWORD	i2 = face.prID2;
		i2 *= N_ELEMENT;
		faceIndicesPtr[n].i0 = i0;
		faceIndicesPtr[n].i1 = i1;
		faceIndicesPtr[n].i2 = i2;
	}
	faceIndexBufferPtr->Unlock();

	DWORD	cullMode;
	__devicePtr->GetRenderState(D3DRS_CULLMODE, &cullMode);
	__devicePtr->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	__meshPtr->ShowFace(faceIndexBufferPtr, nFace, SHADING_TEXTURE);
	__devicePtr->SetRenderState(D3DRS_CULLMODE, cullMode);
} __finally {
	if (faceIndexBufferPtr) {
#ifdef _DEBUG
		ULONG	refCount = faceIndexBufferPtr->Release();
		assert(refCount == 0);
#else
		faceIndexBufferPtr->Release();
#endif
		faceIndexBufferPtr = 0;
	}
}
}

void
kSelect::ShowPoints(
	const D3DXMATRIX&	worldViewProjMatrix,
	FLOAT		pointSize,
	const D3DXVECTOR4&	pointColor
)
{
	HRESULT	hResult;

	hResult = this->m__pEffect->SetInt("g__mode", 0);
	assert(D3D_OK == hResult);
	hResult = this->m__pEffect->SetMatrix("mWorldViewProjection", &worldViewProjMatrix);
	assert(D3D_OK == hResult);
	hResult = this->m__pEffect->SetFloat("pointSize", pointSize);
	assert(D3D_OK == hResult);
	hResult = this->m__pEffect->SetVector("pointColor", &pointColor);
	assert(D3D_OK == hResult);
	D3DXHANDLE	hTechnique = this->m__pEffect->GetTechnique(0);
	assert(hTechnique);
	hResult = this->m__pEffect->SetTechnique(hTechnique);
	assert(S_OK == hResult);
	UINT	NumberOfPasses;
	hResult = this->m__pEffect->Begin(&NumberOfPasses, 0);
	assert(D3D_OK == hResult);
	for (UINT iPass = 0; iPass < NumberOfPasses; ++iPass) {
		hResult = this->m__pEffect->BeginPass(iPass);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetVertexDeclaration(this->m__pDeclaration);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetStreamSource(0, this->m__pVertexBuffer, 0, sizeof(D3DXVECTOR3));
		assert(D3D_OK == hResult);
		hResult = __devicePtr->DrawPrimitive(D3DPT_POINTLIST, 0, this->m__NumberOfPoints);
		assert(D3D_OK == hResult);
		hResult = this->m__pEffect->EndPass();
		assert(S_OK == hResult);
	}
	hResult = this->m__pEffect->End();
	assert(S_OK == hResult);
}


void
kSelect::DrawRectangle(
	int	iUpperRightX,
	int	iUpperRightY,
	int	iLowerLeftX,
	int	iLowerLeftY
)
{
	LPDIRECT3DVERTEXBUFFER9		pVB;
	HRESULT hResult = __devicePtr->CreateVertexBuffer(
		8 * sizeof(D3DXVECTOR3),
		D3DUSAGE_WRITEONLY,
		0,
		D3DPOOL_MANAGED,
		&pVB,
		NULL
	);
	assert(hResult == D3D_OK);
	D3DXVECTOR3*	pV = NULL;
	hResult = pVB->Lock(0, 0, (void**) &pV, 0);
	assert(D3D_OK == hResult);
	assert(pV);
	D3DVIEWPORT9	viewport;
	__devicePtr->GetViewport(&viewport);
	FLOAT	fUpperRightX = (2.f * iUpperRightX / viewport.Width - 1.f);
	FLOAT	fUpperRightY = -(2.f * iUpperRightY / viewport.Height - 1.f);
	FLOAT	fLowerLeftX = (2.f * iLowerLeftX / viewport.Width - 1.f);
	FLOAT	fLowerLeftY = -(2.f * iLowerLeftY / viewport.Height - 1.f);
	pV[0] = D3DXVECTOR3(fUpperRightX, fUpperRightY, 0.f);
	pV[1] = D3DXVECTOR3(fLowerLeftX, fUpperRightY, 0.f);

	pV[2] = D3DXVECTOR3(fLowerLeftX, fUpperRightY, 0.f);
	pV[3] = D3DXVECTOR3(fLowerLeftX, fLowerLeftY, 0.f);

	pV[4] = D3DXVECTOR3(fLowerLeftX, fLowerLeftY, 0.f);
	pV[5] = D3DXVECTOR3(fUpperRightX, fLowerLeftY, 0.f);

	pV[6] = D3DXVECTOR3(fUpperRightX, fLowerLeftY, 0.f);
	pV[7] = D3DXVECTOR3(fUpperRightX, fUpperRightY, 0.f);
	hResult = pVB->Unlock();
	assert(D3D_OK == hResult);

	hResult = this->m__pEffect->SetInt("g__mode", 1);
	assert(D3D_OK == hResult);
	D3DXHANDLE	hTechnique = this->m__pEffect->GetTechnique(0);
	assert(hTechnique);
	hResult = this->m__pEffect->SetTechnique(hTechnique);
	assert(S_OK == hResult);
	UINT	NumberOfPasses;
	hResult = this->m__pEffect->Begin(&NumberOfPasses, 0);
	assert(D3D_OK == hResult);
	for (UINT iPass = 0; iPass < NumberOfPasses; ++iPass) {
		hResult = this->m__pEffect->BeginPass(iPass);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetVertexDeclaration(this->m__pDeclaration);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetStreamSource(0, pVB, 0, sizeof(D3DXVECTOR3));
		assert(D3D_OK == hResult);
		hResult = __devicePtr->DrawPrimitive(D3DPT_LINELIST, 0, 4);
		assert(D3D_OK == hResult);
		hResult = this->m__pEffect->EndPass();
		assert(S_OK == hResult);
	}
	hResult = this->m__pEffect->End();
	assert(S_OK == hResult);

	if (pVB) {
		pVB->Release();
		pVB = NULL;
	}
}

void
kSelect::DrawGrid(
	const kGrid&	grid,
	const D3DXMATRIX&	worldMatrix,
	const D3DXMATRIX&	viewMatrix,
	const D3DXMATRIX&	projectionMatrix
) const
{
	GRID_TYPE	type = grid.__type;
	if (type == GRID_UNDEF)
		return;

	LPDIRECT3DVERTEXBUFFER9		gridVertexBuffer = 0;
do {
	float	zDepth = grid.__type == GRID_CROSS ? 0.f : (grid.__z == GRID_BACK ? 1.f : 0.f);
	const GRID_DIR&	dir = grid.__dir;
	const float	h_step = type == GRID_NORMAL ? grid.__step.h_step : FLT_MAX;
	const float	v_step = type == GRID_NORMAL ? grid.__step.v_step : FLT_MAX;

	D3DVIEWPORT9	viewport;
	__devicePtr->GetViewport(&viewport);
#if 1	// 기준의 화면
	float	Ox = grid.__O.Ox;
#if 1	// clamp
	if (Ox >= 1.f)
		Ox = 1.f;
	if (Ox <= -1.f)
		Ox = -1.f;
#endif	// clamp
	float	Oy = grid.__O.Oy;
#if 1	// clamp
	if (Oy >= 1.f)
		Oy = 1.f;
	if (Oy <= -1.f)
		Oy = -1.f;
#endif	// clamp
#else	// 기준이 화면/기준이 물체
	D3DXVECTOR3	o(0, 0, 0);
	D3DXVECTOR3	O;
	D3DXVec3Project(&O, &o, &viewport, &projectionMatrix, &viewMatrix, &worldMatrix);
	FLOAT	Ox = (2.f * O.x / viewport.Width - 1.f);
	FLOAT	Oy = -(2.f * O.y / viewport.Height - 1.f);
#endif	// 기준이 물체
	unsigned	numLines = 0;
#if 1
if (dir & GRID_VERTICAL) {
	numLines += (int) ceil(2 / h_step) + 1;	// 최대...
}
if (dir & GRID_HORIZONTAL) {
	numLines += (int) ceil(2 / v_step) + 1;	// 최대...
}
#else
	// x 축에 평행한 선의 갯수
	++numLines;	// (Ox, Oy) 를 지나고 x 축에 평행한 선
	for (FLOAT y = Oy + v_step; y <= 1; y += v_step)
		++numLines;
	for (FLOAT y = Oy - v_step; y >= -1; y -= v_step)
		++numLines;
	++numLines;	// (Ox, Oy) 를 지나고 y 축에 평행한 선
	for (FLOAT x = Ox + h_step; x <= 1; x += h_step)
		++numLines;
	for (FLOAT x = Ox - h_step; x >= -1; x -= h_step)
		++numLines;
#endif

	HRESULT hResult = __devicePtr->CreateVertexBuffer(
		2 * numLines * sizeof(D3DXVECTOR3),
		D3DUSAGE_WRITEONLY,
		0,
		D3DPOOL_MANAGED,
		&gridVertexBuffer,
		NULL
	);
	assert(hResult == D3D_OK);
	D3DXVECTOR3*	gridPointPtr = NULL;
	hResult = gridVertexBuffer->Lock(0, 0, (void**) &gridPointPtr, 0);
	assert(D3D_OK == hResult);
	assert(gridPointPtr);
#if 1
	unsigned	n = 0;
	// x 축에 평행한 선(수평선)의 갯수
	// assert(n < 2 * numLines);
if (dir & GRID_HORIZONTAL) {
	if (Oy >= -1 && Oy <= 1) {
		gridPointPtr[n++] = D3DXVECTOR3(-1, Oy, 0.f);	// (Ox, Oy) 를 지나고 x 축에 평행한 선
		gridPointPtr[n++] = D3DXVECTOR3(1, Oy, 0.f);	// (Ox, Oy) 를 지나고 x 축에 평행한 선
	}
	for (FLOAT y = Oy + v_step; y <= 1; y += v_step) {
		if (y < -1)
			continue;
		assert(n < 2 * numLines);
		gridPointPtr[n++] = D3DXVECTOR3(-1, y, 0.f);	// (Ox, Oy) 를 지나고 x 축에 평행한 선
		gridPointPtr[n++] = D3DXVECTOR3(1, y, 0.f);	// (Ox, Oy) 를 지나고 x 축에 평행한 선
	}
	for (FLOAT y = Oy - v_step; y >= -1; y -= v_step) {
		if (y > 1)
			continue;
		assert(n < 2 * numLines);
		gridPointPtr[n++] = D3DXVECTOR3(-1, y, 0.f);	// (Ox, Oy) 를 지나고 x 축에 평행한 선
		gridPointPtr[n++] = D3DXVECTOR3(1, y, 0.f);	// (Ox, Oy) 를 지나고 x 축에 평행한 선
	}
}
if (dir & GRID_VERTICAL) {
	// y 축에 평행한 선(수직선)의 갯수
	if (Ox >= -1 && Ox <= 1) {
		assert(n < 2 * numLines);
		gridPointPtr[n++] = D3DXVECTOR3(Ox, -1, 0.f);	// (Ox, Oy) 를 지나고 y 축에 평행한 선
		gridPointPtr[n++] = D3DXVECTOR3(Ox, 1, 0.f);	// (Ox, Oy) 를 지나고 y 축에 평행한 선
	}
	for (FLOAT x = Ox + h_step; x <= 1; x += h_step) {
		if (x < -1)
			continue;
		assert(n < 2 * numLines);
		gridPointPtr[n++] = D3DXVECTOR3(x, -1, 0.f);	// (Ox, Oy) 를 지나고 y 축에 평행한 선
		gridPointPtr[n++] = D3DXVECTOR3(x, 1, 0.f);	// (Ox, Oy) 를 지나고 y 축에 평행한 선
	}
	for (FLOAT x = Ox - h_step; x >= -1; x -= h_step) {
		if (x > 1)
			continue;
		assert(n < 2 * numLines);
		gridPointPtr[n++] = D3DXVECTOR3(x, -1, 0.f);	// (Ox, Oy) 를 지나고 y 축에 평행한 선
		gridPointPtr[n++] = D3DXVECTOR3(x, 1, 0.f);	// (Ox, Oy) 를 지나고 y 축에 평행한 선
	}
}
#else
	gridPointPtr[0] = D3DXVECTOR3(-1, Oy, 0.f);
	gridPointPtr[1] = D3DXVECTOR3(1, Oy, 0.f);
	gridPointPtr[2] = D3DXVECTOR3(Ox, -1, 0.f);
	gridPointPtr[3] = D3DXVECTOR3(Ox, 1, 0.f);
#endif
#if 0
	FLOAT	fUpperRightX = (2.f * iUpperRightX / viewport.Width - 1.f);
	FLOAT	fUpperRightY = -(2.f * iUpperRightY / viewport.Height - 1.f);
	FLOAT	fLowerLeftX = (2.f * iLowerLeftX / viewport.Width - 1.f);
	FLOAT	fLowerLeftY = -(2.f * iLowerLeftY / viewport.Height - 1.f);
	pV[0] = D3DXVECTOR3(fUpperRightX, fUpperRightY, 0.f);
	pV[1] = D3DXVECTOR3(fLowerLeftX, fUpperRightY, 0.f);

	pV[2] = D3DXVECTOR3(fLowerLeftX, fUpperRightY, 0.f);
	pV[3] = D3DXVECTOR3(fLowerLeftX, fLowerLeftY, 0.f);

	pV[4] = D3DXVECTOR3(fLowerLeftX, fLowerLeftY, 0.f);
	pV[5] = D3DXVECTOR3(fUpperRightX, fLowerLeftY, 0.f);

	pV[6] = D3DXVECTOR3(fUpperRightX, fLowerLeftY, 0.f);
	pV[7] = D3DXVECTOR3(fUpperRightX, fUpperRightY, 0.f);
#endif
	hResult = gridVertexBuffer->Unlock();
	assert(D3D_OK == hResult);

	hResult = this->m__pEffect->SetInt("g__mode", 2);
	assert(D3D_OK == hResult);
	hResult = this->m__pEffect->SetFloat("g__zDepth", zDepth);
	assert(D3D_OK == hResult);
	hResult = this->m__pEffect->SetVector("pointColor", &D3DXVECTOR4(0, 1, 0, 1));
	D3DXHANDLE	hTechnique = this->m__pEffect->GetTechnique(0);
	assert(hTechnique);
	hResult = this->m__pEffect->SetTechnique(hTechnique);
	assert(S_OK == hResult);
	UINT	NumberOfPasses;
	hResult = this->m__pEffect->Begin(&NumberOfPasses, 0);
	assert(D3D_OK == hResult);
	for (UINT iPass = 0; iPass < NumberOfPasses; ++iPass) {
		hResult = this->m__pEffect->BeginPass(iPass);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetVertexDeclaration(this->m__pDeclaration);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetStreamSource(0, gridVertexBuffer, 0, sizeof(D3DXVECTOR3));
		assert(D3D_OK == hResult);
		hResult = __devicePtr->DrawPrimitive(D3DPT_LINELIST, 0, numLines);
		assert(D3D_OK == hResult);
		hResult = this->m__pEffect->EndPass();
		assert(S_OK == hResult);
	}
	hResult = this->m__pEffect->End();
	assert(S_OK == hResult);
} while (0);
	if (gridVertexBuffer) {
		gridVertexBuffer->Release();
		gridVertexBuffer = NULL;
	}
}

void
kSelect::DrawCircle(
	unsigned	centerX,
	unsigned	centerY,
	FLOAT	fRadius,

	FLOAT		pointSize,
	D3DXVECTOR4*	pPointColor
)
{
	UINT	nStep = 2;
	UINT	nPoints = 360 / nStep;
	LPDIRECT3DVERTEXBUFFER9		pVB;
	HRESULT hResult = __devicePtr->CreateVertexBuffer(
		nPoints * sizeof(D3DXVECTOR3),
		D3DUSAGE_WRITEONLY,
		0,
		D3DPOOL_MANAGED,
		&pVB,
		NULL
	);
	assert(hResult == D3D_OK);
	D3DXVECTOR3*	pV = NULL;
	hResult = pVB->Lock(0, 0, (void**) &pV, 0);
	assert(D3D_OK == hResult);
	assert(pV);
	D3DVIEWPORT9	viewport;
	__devicePtr->GetViewport(&viewport);
	FLOAT	fCenterX = (2.f * centerX / viewport.Width - 1.f);
	FLOAT	fCenterY = -(2.f * centerY / viewport.Height - 1.f);
	fRadius *= 2.f / viewport.Width;
	float	aspectRatio = (float) viewport.Width / viewport.Height;
	for (UINT i = 0; i < nPoints; ++i) {
		double	angle = nStep * i * D3DX_PI / 180.;
		FLOAT	x = fCenterX + fRadius * cosf((FLOAT) angle);
		FLOAT	y = fCenterY + fRadius * sinf((FLOAT) angle) * aspectRatio;
		pV[i] = D3DXVECTOR3(x, y, 0.f);
	}
	hResult = pVB->Unlock();
	assert(D3D_OK == hResult);

	hResult = this->m__pEffect->SetInt("g__mode", 1);
	assert(D3D_OK == hResult);
	hResult = this->m__pEffect->SetFloat("pointSize", pointSize);
	assert(D3D_OK == hResult);
	hResult = this->m__pEffect->SetVector("pointColor", pPointColor);
	assert(D3D_OK == hResult);
	D3DXHANDLE	hTechnique = this->m__pEffect->GetTechnique(0);
	assert(hTechnique);
	hResult = this->m__pEffect->SetTechnique(hTechnique);
	assert(S_OK == hResult);
	UINT	NumberOfPasses;
	hResult = this->m__pEffect->Begin(&NumberOfPasses, 0);
	assert(D3D_OK == hResult);
	for (UINT iPass = 0; iPass < NumberOfPasses; ++iPass) {
		hResult = this->m__pEffect->BeginPass(iPass);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetVertexDeclaration(this->m__pDeclaration);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetStreamSource(0, pVB, 0, sizeof(D3DXVECTOR3));
		assert(D3D_OK == hResult);
		hResult = __devicePtr->DrawPrimitive(D3DPT_LINELIST, 0, nPoints / 2);
		assert(D3D_OK == hResult);
		hResult = this->m__pEffect->EndPass();
		assert(S_OK == hResult);
	}
	hResult = this->m__pEffect->End();
	assert(S_OK == hResult);

	if (pVB) {
		pVB->Release();
		pVB = NULL;
	}
}

#if 1
void	kSelect::DrawContour(
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
)
{
#if 1	// 원래 점들
if (__2dProjectNumPoints) {
	HRESULT hResult;
	hResult = m__pEffect->SetInt("g__mode", 1);
	assert(D3D_OK == hResult);
	hResult = m__pEffect->SetFloat("pointSize", pointSize);
	assert(D3D_OK == hResult);
	hResult = m__pEffect->SetVector("pointColor", &pointColor);
	assert(D3D_OK == hResult);
	D3DXHANDLE	hTechnique = m__pEffect->GetTechnique(0);
	assert(hTechnique);
	hResult = m__pEffect->SetTechnique(hTechnique);
	assert(S_OK == hResult);
	UINT	NumberOfPasses;
	hResult = m__pEffect->Begin(&NumberOfPasses, 0);
	assert(D3D_OK == hResult);
	for (UINT iPass = 0; iPass < NumberOfPasses; ++iPass) {
		hResult = m__pEffect->BeginPass(iPass);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetVertexDeclaration(__contourDeclarationPtr);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetStreamSource(0, __2dProjectVertexBufferPtr, 0, sizeof(D3DXVECTOR3));
		assert(D3D_OK == hResult);
		hResult = __devicePtr->DrawPrimitive(D3DPT_POINTLIST, 0, __2dProjectNumPoints);
		assert(D3D_OK == hResult);
		hResult = m__pEffect->EndPass();
		assert(S_OK == hResult);
	}
	hResult = m__pEffect->End();
	assert(S_OK == hResult);
}
#endif

#if 1	// Interpolated 된 점들
if (__2dInterpolatedProjectNumPoints) {
	HRESULT hResult;
	hResult = m__pEffect->SetInt("g__mode", 1);
	assert(D3D_OK == hResult);
	hResult = m__pEffect->SetFloat("pointSize", interpolatedPointSize);
	assert(D3D_OK == hResult);
	hResult = m__pEffect->SetVector("pointColor", &interpolatedPointColor);
	assert(D3D_OK == hResult);
	D3DXHANDLE	hTechnique = m__pEffect->GetTechnique(0);
	assert(hTechnique);
	hResult = m__pEffect->SetTechnique(hTechnique);
	assert(S_OK == hResult);
	UINT	NumberOfPasses;
	hResult = m__pEffect->Begin(&NumberOfPasses, 0);
	assert(D3D_OK == hResult);
	for (UINT iPass = 0; iPass < NumberOfPasses; ++iPass) {
		hResult = m__pEffect->BeginPass(iPass);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetVertexDeclaration(__contourDeclarationPtr);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetStreamSource(0, __2dInterpolatedProjectVertexBufferPtr, 0, sizeof(D3DXVECTOR3));
		assert(D3D_OK == hResult);
		hResult = __devicePtr->DrawPrimitive(D3DPT_POINTLIST, 0, __2dInterpolatedProjectNumPoints);
		assert(D3D_OK == hResult);
		hResult = m__pEffect->EndPass();
		assert(S_OK == hResult);
	}
	hResult = m__pEffect->End();
	assert(S_OK == hResult);
}
#endif

#if 1	// START_TYPE__TWO_POINT 의 경우, 양 끝점 사이를 채우는 점들
if (__2dFillProjectNumPoints) {
	HRESULT hResult;
	hResult = m__pEffect->SetInt("g__mode", 1);
	assert(D3D_OK == hResult);
	hResult = m__pEffect->SetFloat("pointSize", fillPointSize);
	assert(D3D_OK == hResult);
	hResult = m__pEffect->SetVector("pointColor", &fillPointColor);
	assert(D3D_OK == hResult);
	D3DXHANDLE	hTechnique = m__pEffect->GetTechnique(0);
	assert(hTechnique);
	hResult = m__pEffect->SetTechnique(hTechnique);
	assert(S_OK == hResult);
	UINT	NumberOfPasses;
	hResult = m__pEffect->Begin(&NumberOfPasses, 0);
	assert(D3D_OK == hResult);
	for (UINT iPass = 0; iPass < NumberOfPasses; ++iPass) {
		hResult = m__pEffect->BeginPass(iPass);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetVertexDeclaration(__contourDeclarationPtr);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetStreamSource(0, __2dFillProjectVertexBufferPtr, 0, sizeof(D3DXVECTOR3));
		assert(D3D_OK == hResult);
		hResult = __devicePtr->DrawPrimitive(D3DPT_POINTLIST, 0, __2dFillProjectNumPoints);
		assert(D3D_OK == hResult);
		hResult = m__pEffect->EndPass();
		assert(S_OK == hResult);
	}
	hResult = m__pEffect->End();
	assert(S_OK == hResult);
}
#endif
}
#else
void	kSelect::DrawContour(
	TAPE_MEASURE_METHOD_T	method,

	bool	closed,
	int	numSegments,
	const D3DXVECTOR3*	pointPtr,

	const D3DXVECTOR3&	nDir,
	const D3DXVECTOR3&	xDir,

	FLOAT		pointSize,
	const D3DXVECTOR4&	cPointColor,
	FLOAT		interpolatedPointSize,
	const D3DXVECTOR4&	interpolatedPointColor
)
{
	int	numPoints = closed ? numSegments : numSegments + 1;
do {
	if (numPoints < 2) {	// Do nothing
		break;
	}

	// numPoints 개의 점들이 하나의 닫힌 곡선을 만든다고 가정하자;
	// 각 점들은, 각 점들의 중심을 원점으로 하는 2차원 평면상의 점들로 변환한다;
	// 각 점들의 기준축에 대한 (ell, phi) 만 알면 된다;
	// 중심에서 각 점까지의 거리가 ell 이 된다.
	// 기준축은 수직인 두 축 z축((0, 0, 1): theta 의 기준축)과 x축((1, 0, 0): phi 의 기준축)을 말한다.
	// z축을 nDir 로 바꾸는 회전이동에 대해 x축이 어떻게 바뀌는 지 알아본다.
	D3DXVECTOR3	zn0(0, 0, 1);
	D3DXVECTOR3	nDir0;
	D3DXVec3Normalize(&nDir0, &nDir);
	FLOAT		theta = D3DXVec3Dot(&zn0, &nDir0);	// 두 벡터사이의 각도
	D3DXVECTOR3	axis;	// zn0 을 nDir0 로 옮길 때 사용하는 회전축
	D3DXVec3Cross(&axis, &zn0, &nDir0);
	D3DXMATRIX	m;	// 이 회전에 해당하는 행렬
	D3DXMatrixRotationAxis(&m, &axis, acosf(theta));
	D3DXVECTOR3	xn;	// x축(xn0)이 회전에 의해서 바뀌는 방향벡터
	D3DXVec3TransformCoord(&xn, &D3DXVECTOR3(1, 0, 0), &m);
		// 원래는 D3DXVec3TransformNormal 을 사용해야 하나,
		// 회전만 있는 경우에는 결과가 같다.
		// D3DXVec3TransformNormal 를 호출하면 주어진 행렬의 역행렬을 구한 후
		// 다시 Transpose 를 계산하므로 시간이 더 걸린다.
//	FLOAT	ell = D3DXVec3Dot(&nDir0, &xn);
	D3DXVECTOR3	yn;
	D3DXVec3Cross(&yn, &xn, &nDir0);	// 2D단면을 볼 때는 오른손 좌표계;
						// 화면에서 나오는 방향이 z축이라고 했을 때,
						// 오른쪽이 x축, 위쪽이 y축
	// xn축과 yn축에 이르는 방향코사인이 곧 (x, y)가 된다.
	if (method == TAPE_MEASURE_METHOD_TWO_POINT) {
		xn = xDir;
		D3DXVec3Cross(&yn, &xn, &nDir);	// 2D단면을 볼 때는 오른손 좌표계;
						// 화면에서 나오는 방향이 z축이라고 했을 때,
						// 오른쪽이 x축, 위쪽이 y축
	}

	D3DXVECTOR3	center(0, 0, 0);
	if (method == TAPE_MEASURE_METHOD_TWO_POINT) {
		center = .5f * (pointPtr[0] + pointPtr[numPoints - 1]);
	} else {
		for (int n = 0; n < numPoints; ++n)
			center += pointPtr[n];
		center /= (float) numPoints;
	}
	D3DXVECTOR3*	contourPointPtr = 0;
	__contourVertexBufferPtr->Lock(0, 0, (void**) &contourPointPtr, 0);
	FLOAT	r_max = FLT_MIN;
	float	x_max = -FLT_MAX;
	float	x_min = FLT_MAX;
	float	y_max = -FLT_MAX;
	float	y_min = FLT_MAX;
	for (int n = 0; n < numPoints; ++n) {
		D3DXVECTOR3	w = pointPtr[n] - center;
		D3DXVECTOR3	w0; D3DXVec3Normalize(&w0, &w);
		FLOAT	ell = D3DXVec3Length(&w);
		FLOAT	cosine1 = D3DXVec3Dot(&w0, &xn);
		FLOAT	cosine2 = D3DXVec3Dot(&w0, &yn);
		D3DXVECTOR3&	v = contourPointPtr[n];
//	FLOAT	fCenterX = (2.f * centerX / viewport.Width - 1.f);
//	FLOAT	fCenterY = -(2.f * centerY / viewport.Height - 1.f);
		// Upper Left: (-1, 1)
		// Lower Right: (1, -1)
		v.x = ell * cosine1;
		if (v.x > x_max)
			x_max = v.x;
		if (v.x < x_min)
			x_min = v.x;
		if (fabsf(v.x) > r_max)
			r_max = fabsf(v.x);
		v.y = ell * cosine2;
		if (v.y > y_max)
			y_max = v.y;
		if (v.y < y_min)
			y_min = v.y;
		if (fabsf(v.y) > r_max)
			r_max = fabsf(v.y);
		v.z = 0;
//		(void) fprintf(stdout, "P[%d]: [%f, %f] (%f, %f)\n", n, v.x, v.y, cosine1, cosine2);
	}
	const float	box_a = .4f;
	const float	offset = .05f;
	// Upper Left: (-1, 1)
	// Lower Right: (1, -1)
	const float	box_center_x = (1.f - box_a / 2) - offset;
	const float	box_center_y = -(1.f - box_a / 2) + offset;
	float	scale;	// W x H 박스가 한 변의 길이가 box_a 인 정사각형 박스 안에 들어가도록
	if (x_max - x_min > y_max - y_min)	// W > H; W 가 box_a 에 들어가도록
		scale = box_a / (x_max - x_min);
	else	// H > W; H 가 box_a 에 들어가도록
		scale = box_a / (y_max - y_min);
	D3DVIEWPORT9	viewport;
	DXUTGetD3D9Device()->GetViewport(&viewport);
	float	aspect_ratio = (FLOAT) viewport.Width / viewport.Height;
	for (int n = 0; n < numPoints; ++n) {
		D3DXVECTOR3&	v = contourPointPtr[n];
		v.x *= scale / aspect_ratio; v.x += box_center_x;
		v.y *= scale; v.y += box_center_y;
	}
	__contourVertexBufferPtr->Unlock();

#if 1	// 원래 점
	HRESULT hResult;
	hResult = this->m__pEffect->SetInt("g__mode", 1);
	assert(D3D_OK == hResult);
	hResult = this->m__pEffect->SetFloat("pointSize", pointSize);
	assert(D3D_OK == hResult);
	hResult = this->m__pEffect->SetVector("pointColor", &cPointColor);
	assert(D3D_OK == hResult);
	D3DXHANDLE	hTechnique = this->m__pEffect->GetTechnique(0);
	assert(hTechnique);
	hResult = this->m__pEffect->SetTechnique(hTechnique);
	assert(S_OK == hResult);
	UINT	NumberOfPasses;
	hResult = this->m__pEffect->Begin(&NumberOfPasses, 0);
	assert(D3D_OK == hResult);
	for (UINT iPass = 0; iPass < NumberOfPasses; ++iPass) {
		hResult = this->m__pEffect->BeginPass(iPass);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetVertexDeclaration(__contourDeclarationPtr);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetStreamSource(0, __contourVertexBufferPtr, 0, sizeof(D3DXVECTOR3));
		assert(D3D_OK == hResult);
		hResult = __devicePtr->DrawPrimitive(D3DPT_POINTLIST, 0, numPoints);
		assert(D3D_OK == hResult);
		hResult = this->m__pEffect->EndPass();
		assert(S_OK == hResult);
	}
	hResult = this->m__pEffect->End();
	assert(S_OK == hResult);
#endif

	// Catmull-Rom interpolation 에 의해 numPoints 점들 사이의 세그먼트를 채운다;
//	if (numPoints < 4) {	// 적어도 4개의 점이 있어야 한다.
//		break;
//	}
	const int	M = 10;	// 각 점들 사이를 N 개의 점들로 채운다.
	// 닫힌 곡선이라 가정하면 점의 갯수와 세그먼트의 갯수가 같다.
	LPDIRECT3DVERTEXDECLARATION9	interpolationDeclarationPtr = 0;
	LPDIRECT3DVERTEXBUFFER9		interpolationVertexBufferPtr = 0;
	do {
		HRESULT	hResult;
		D3DVERTEXELEMENT9	element[2] = {
			{
				0,
				0 * sizeof(D3DXVECTOR3),
				D3DDECLTYPE_FLOAT3,
				D3DDECLMETHOD_DEFAULT,
				D3DDECLUSAGE_POSITION,
				0
			},

			D3DDECL_END()
		};
		hResult = __devicePtr->CreateVertexDeclaration(element, &interpolationDeclarationPtr);

		hResult = __devicePtr->CreateVertexBuffer(
			M * numSegments * sizeof(D3DXVECTOR3),	// 한 세그먼트당 10개의 점으로 채운다.
			D3DUSAGE_WRITEONLY,
			0,
			D3DPOOL_MANAGED,
			&interpolationVertexBufferPtr,
			NULL
		);
		assert(D3D_OK == hResult);

		D3DXVECTOR3*	contourPointPtr = 0;
		__contourVertexBufferPtr->Lock(0, 0, (void**) &contourPointPtr, 0);
		D3DXVECTOR3*	interpolationPointPtr = 0;
		interpolationVertexBufferPtr->Lock(0, 0, (void**) &interpolationPointPtr, 0);
		// Catmull-Rom Interpolation 은 주어진 네 점(P1, P2, P3, P4)를 이용하여
		// P2, P3 사이에서 부드러운 곡선을 만든다;
		// 닫힌 곡선의 경우: TAPE_MEASURE_METHOD_GIRTH
		// P[0]		~ P[1]		사이는	P[N - 1], P[0], P[1], P[2]		를 사용하여
		// P[1]		~ P[2]		사이는	P[0], P[1], P[2], P[3]			를 사용하여
		// P[2]		~ P[3]		사이는	P[1], P[2], P[3], P[4]			를 사용하여
		// ......
		// P[N - 2]	~ P[N - 1]	사이는	P[N - 3], P[N - 2], P[N - 1], P[0]	를 사용하여
		// P[N - 1]	~ P[0]		사이는	P[N - 2], P[N - 1], P[0], P[1]		를 사용하여
		// 열린 곡선의 경우: TAPE_MEASURE_METHOD_TWO_POINT
		for (int n = 0; n < numSegments; ++n) {
			// n 번째 segment 즉 n 번째 점과 (n + 1)번째 점 사이를 부드러운 점들로 채운다;
			D3DXVECTOR3*	P0Ptr;
			D3DXVECTOR3*	P1Ptr;
			D3DXVECTOR3*	P2Ptr;
			D3DXVECTOR3*	P3Ptr;
#if 1
			int	n1 = n;
			int	n0 = n1 - 1;
			if (n0 < 0) {
				n0 = closed ? numPoints - 1 : n1;
			}
			int	n2 = n1 + 1;
			if (n2 >= numPoints) {	// closed 인 경우에만 해당
				assert(closed);
				n2 = 0;
			}
			int	n3 = n2 + 1;
			if (n3 >= numPoints) {
				n3 = closed ? 0 : n2;
			}

			P0Ptr = &contourPointPtr[n0];
			P1Ptr = &contourPointPtr[n1];
			P2Ptr = &contourPointPtr[n2];
			P3Ptr = &contourPointPtr[n3];
#else
			P0Ptr = n == 0 ? &contourPointPtr[numPoints - 1] : &contourPointPtr[n - 1];
			P1Ptr = &contourPointPtr[n];
			P2Ptr = n == numPoints - 1 ? &contourPointPtr[0] : &contourPointPtr[n + 1];
			P3Ptr = n == numPoints - 2 ? &contourPointPtr[0] : (n == numPoints - 1 ? &contourPointPtr[1] : &contourPointPtr[n + 2]);
#endif
			// P1, P2 사이를 M 개의 점들로 채운다; (1 + M) 개의 세그먼트를 만든다.
			for (int m = 0; m < 10; ++m) {
				FLOAT	s = 1.f / (1 + M) * (1 + m);
				D3DXVec3CatmullRom(&interpolationPointPtr[M * n + m], P0Ptr, P1Ptr, P2Ptr, P3Ptr, s);
			}
		}
		interpolationVertexBufferPtr->Unlock();
		__contourVertexBufferPtr->Unlock();

#if 1	// Interpolated 된 점
{
		HRESULT hResult;
		hResult = this->m__pEffect->SetInt("g__mode", 1);
		assert(D3D_OK == hResult);
		hResult = this->m__pEffect->SetFloat("pointSize", interpolatedPointSize);
		assert(D3D_OK == hResult);
		hResult = this->m__pEffect->SetVector("pointColor", &interpolatedPointColor);
		assert(D3D_OK == hResult);
		D3DXHANDLE	hTechnique = this->m__pEffect->GetTechnique(0);
		assert(hTechnique);
		hResult = this->m__pEffect->SetTechnique(hTechnique);
		assert(S_OK == hResult);
		UINT	NumberOfPasses;
		hResult = this->m__pEffect->Begin(&NumberOfPasses, 0);
		assert(D3D_OK == hResult);
		for (UINT iPass = 0; iPass < NumberOfPasses; ++iPass) {
			hResult = this->m__pEffect->BeginPass(iPass);
			assert(D3D_OK == hResult);
			hResult = __devicePtr->SetVertexDeclaration(interpolationDeclarationPtr);
			assert(D3D_OK == hResult);
			hResult = __devicePtr->SetStreamSource(0, interpolationVertexBufferPtr, 0, sizeof(D3DXVECTOR3));
			assert(D3D_OK == hResult);
			hResult = __devicePtr->DrawPrimitive(D3DPT_POINTLIST, 0, M * numSegments);
			assert(D3D_OK == hResult);
			hResult = this->m__pEffect->EndPass();
			assert(S_OK == hResult);
		}
		hResult = this->m__pEffect->End();
		assert(S_OK == hResult);
}
#endif
	} while (0);
	if (interpolationDeclarationPtr) {
		interpolationDeclarationPtr->Release();
		interpolationDeclarationPtr = 0;
	}
	if (interpolationVertexBufferPtr) {
		interpolationVertexBufferPtr->Release();
		interpolationVertexBufferPtr = 0;
	}
} while (0);
}
#endif


#if 0
HRESULT
kSelect::Pick(
	bool	bBrush,
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
)
{
#if 1
	IDirect3DDevice9*	devicePtr = DXUTGetD3D9Device();
	D3DVIEWPORT9	viewport;
	devicePtr->GetViewport(&viewport);

	if (bBrush) {
		HRESULT	hResult;

		// SHIFT 키와 CONTROL 키가 동시에 눌러졌다면 이는 MoveTool 에 의한 동작에 해당하므로 건너뛴다.
//		if (g__Shift && g__Control)
//			return S_OK;

		POINT	ptCursor;
		GetCursorPos(&ptCursor);
		ScreenToClient(DXUTGetHWND(), &ptCursor);
#if 1
		D3DXVECTOR3*	pSelectV = NULL;
		LPDIRECT3DVERTEXBUFFER9	pSelectVB = NULL;
		hResult = ResetVertexBuffer(kSelect::NUMBER_OF_POINTS__UNIT, &pSelectVB);
		assert(D3D_OK == hResult);
		assert(pSelectVB);
		hResult = pSelectVB->Lock(0, 0, (void**) &pSelectV, 0);
		assert(D3D_OK == hResult);
		assert(pSelectV);

		UINT	k = 0;
		FLOAT	zNearest = FLT_MAX;
		const CPointRepr*	pPointRepr = __meshPtr->GetPointReprPointer();
#if 1
		LPDIRECT3DVERTEXBUFFER9	pVB = __meshPtr->GetVertexBufferPointer();
		kMesh::D3DXVERTEX*	pV;
		pVB->Lock(0, 0, (void**) &pV, 0);
		FLOAT	aspectRatio = (FLOAT) viewport.Width / viewport.Height;

		D3DXVECTOR3	o = D3DXVECTOR3((FLOAT) ptCursor.x, (FLOAT) ptCursor.y, 0.f);
		D3DXVECTOR3	O;	// Object 공간에서의 좌표
		D3DXVec3Unproject(&O, &o, &viewport, &projectionMatrix, &viewMatrix, &worldMatrix);
#endif
#if 1
		const CFace*	pFace = __meshPtr->GetFacePointer();
		DWORD*	pFaceFlag = new DWORD[__meshPtr->GetNumFaces()];
		memset(pFaceFlag, 0, __meshPtr->GetNumFaces() * sizeof(DWORD));
		BRUSHPOINT*	pBrushPointFlag = new BRUSHPOINT[__meshPtr->GetNumPointReprs()];
		memset(pBrushPointFlag, 0, __meshPtr->GetNumPointReprs() * sizeof(BRUSHPOINT));
		for (DWORD iFace = 0; iFace < __meshPtr->GetNumFaces(); ++iFace) {
			const CFace&	face = pFace[iFace];
			// 1. 삼각형의 normal 이 카메라를 보고 있는가?
			// 2. 삼각형을 이루는 각 꼭지점을 이전에 다룬 적이 있는가?
			// 없다면 원 안에 들어오는 꼭지점이 있는가?
			// 3. 카메라에서 꼭지점에 이르는 반직선이
			// 충돌하는 삼각형이 있는가?
			// (1., 2., 3. 모두를 만족하는 꼭지점에 대해서,
			// 그러한 삼각형들만을 고려해서...)

			// 1. 삼각형의 normal 이 카메라를 보고 있는가?
			D3DXVECTOR3	_0 = pPointRepr[face.prID0].v.p;
			D3DXVECTOR3	_1 = pPointRepr[face.prID1].v.p;
			D3DXVECTOR3	_2 = pPointRepr[face.prID2].v.p;
			D3DXVECTOR3	G = (_0 + _1 + _2) / 3.f;	// 삼각형의 무게중심
			D3DXVECTOR3	_01 = _1 - _0;
			D3DXVECTOR3	_02 = _2 - _0;
			D3DXVECTOR3	n;	// 삼각형의 normal 벡터
			D3DXVECTOR3	r = G - O;	// O 에서 G 에 이르는 벡터
			// n 과 r 의 내적이 음수이면 삼각형이 카메라를 보고있다.
			if (D3DXVec3Dot(&r, D3DXVec3Cross(&n, &_01, &_02)) > 0.f)
				continue;

			// 2. 삼각형을 이루는 각 꼭지점을 이전에 다룬 적이 있는가?
			// 없다면 원 안에 들어오는 꼭지점이 있는가?
			bool	bInsideCircle = false;
			for (int i = 0; i < 3; ++i) {
				const DWORD&	prID = face.prID[i];
				if (pBrushPointFlag[prID].flag)	// 이 꼭지점을 다룬 적이 있는가?
					continue;

				// 원 안에 들어오는가?
				pBrushPointFlag[prID].flag = 1;	// 이 꼭지점은 다루었다!
				const D3DXVECTOR3&	Q = pPointRepr[prID].v.p;
				D3DXVECTOR3	q;
				D3DXVec3Project(&q, &Q, &viewport, &projectionMatrix, &viewMatrix, &worldMatrix);
				if (q.z < 0 || q.z > 1.f)
					continue;

				D3DXVECTOR2	A((q.x - ptCursor.x) / brushRadius, (q.y - ptCursor.y) / brushRadius * aspectRatio);
				FLOAT	d2 = D3DXVec2LengthSq(&A);
				if (d2 > 1.f) {
					continue;
				}
				// 원 안에 있다!
				BRUSHPOINT&	brushPoint = pBrushPointFlag[prID];
				brushPoint.flag = 2;	// 다뤄진 이 꼭지점은 원 안에 있다!
				brushPoint.d2 = d2;
				brushPoint.z = q.z;

				bInsideCircle = true;		// 따라서 이 꼭지점을 포함한 삼각형은 나중에 고려할 대상이다.
			}
			if (bInsideCircle)
				pFaceFlag[iFace] = 1;
		}
		// 3. 카메라에서 꼭지점에 이르는 반직선이
		// 충돌하는 삼각형이 있는가?
		// (1., 2., 3. 모두를 만족하는 꼭지점에 대해서,
		// 그러한 삼각형들만을 고려해서...)
		for (DWORD iFace = 0; iFace < __meshPtr->GetNumFaces(); ++iFace) {
			// 1. 고려할 삼각형을 이루는 꼭지점 중에서
			// 2. 원 안에 있는 꼭지점 중에서
			// 다른 고려할 삼각형과 충돌하는 지 알아보자.
			if (pFaceFlag[iFace] == 0)
				continue;
			// 1. 고려할 삼각형안에 있다.
			for (int i = 0; i < 3; ++i) {
				DWORD	prID = pFace[iFace].prID[i];
				if (pBrushPointFlag[prID].flag != 2)
					continue;

				// 충돌 테스트까지 끝날 것이다.
				pBrushPointFlag[prID].flag = 3;
				// 2. 원 안에 있다.
				bool	bCollide = false;
				const CPointRepr&	pointRepr = pPointRepr[prID];
				const D3DXVECTOR3&	Q = pointRepr.v.p;
				for (DWORD kFace = 0; kFace < __meshPtr->GetNumFaces(); ++kFace) {
					if (kFace == iFace)	// 자기 자신은 제외
						continue;
					if (pFaceFlag[kFace] == 0)	// 고려하지않을 삼각형은 제외
						continue;

					FLOAT	fBary1, fBary2;
					FLOAT	fDist;
					const CFace&	face = pFace[kFace];
					D3DXVECTOR3	r = Q - O;
					D3DXVECTOR3	rn;	// normalized r
					D3DXVec3Normalize(&rn, &r);
					if (IntersectTriangle(
						O, rn,

						pPointRepr[face.prID0].v.p,
						pPointRepr[face.prID1].v.p,
						pPointRepr[face.prID2].v.p,

						&fDist, &fBary1, &fBary2
					)) {
						// 자기 뒤에 있는 삼각형과 충돌할 수 있다.
						if (D3DXVec3LengthSq(&r) > fDist * fDist) {
							bCollide = true;	// 앞쪽에 충돌하는 삼각형이 있다.
							break;			// 더 이상 조사할 이유가 없다.
						}
					}
				}
				if (bCollide)	// 충돌하는 삼각형이 있다; 즉 카메라에서는 안 보인다.
					continue;

				// 카메라에서 보이고 원 안에 있다.
				BRUSHPOINT&	brushPoint = pBrushPointFlag[prID];
				if (isShiftKeyPressed)
					pV[prID].c -= (1.f - brushPoint.d2) * D3DXVECTOR3(0.f, 0.f, .5f);
				else
					pV[prID].c += (1.f - brushPoint.d2) * D3DXVECTOR3(0.5f, 0.f, 0.f);
				CLAMP2(pV[prID].c);

				if (k >= kSelect::NUMBER_OF_POINTS__UNIT)
					continue;
				FLOAT&	z = brushPoint.z;
				if (z < zNearest) {
					selectedPrID = prID;
					zNearest = z;
				}
				pSelectV[k++] = Q;
			}
		}

		SAFE_DELETE_ARRAY(pFaceFlag);
		SAFE_DELETE_ARRAY(pBrushPointFlag);
#else
		for (DWORD i = 0; i < g__pMesh->GetNumPointReprs(); ++i) {
			DWORD	prID = pPointRepr[i].id;

			D3DXVECTOR3&	Q = pPointRepr[i].v.p;
			D3DXVECTOR3	q;
			D3DXVec3Project(&q, &Q, &viewport, &mProj, &mView, &mWorld);
			if (q.z < 0 || q.z > 1.f)
				continue;

			D3DXVECTOR2	A((q.x - ptCursor.x) / g__brushRadius, (q.y - ptCursor.y) / g__brushRadius * aspectRatio);
			FLOAT	d2 = D3DXVec2LengthSq(&A);
			if (d2 > 1.f)
				continue;

#if 1
			// O 에서 시작에서 OQ 방향으로 나가는 ray 가 충돌하는 삼각형이 있는가?
			D3DXVECTOR3	R = Q - O;
			if (D3DXVec3Dot(&R, &pPointRepr[i].v.n) > 0.f)
				continue;
			BOOL	bHit;
			DWORD	faceID;
			D3DXIntersect(g__pMesh->GetMeshPointer(), &O, &R, &bHit, &faceID, NULL, NULL, NULL, NULL, NULL);
			if (bHit) {
				const CFace&	face = (g__pMesh->GetFacePointer())[faceID];
				if (face.prID0 != prID && prID != face.prID1 && prID != face.prID2)
					continue;
			}
#endif

#if 1
			DWORD	index = prID;
#else
			DWORD	index = (g__pMesh->GetPointReprIDMappedPointer())[prID];
#endif
			if (g__Shift)
				pV[index].c -= (1.f - d2) * D3DXVECTOR3(0.f, 0.f, .5f);
			else
				pV[index].c += (1.f - d2) * D3DXVECTOR3(0.5f, 0.f, 0.f);
			CLAMP2(pV[index].c);

			if (k >= kSelect::NUMBER_OF_POINTS__UNIT)
				continue;
			if (q.z < zNearest) {
				g__prID = prID;
				zNearest = q.z;
			}
			pSelectV[k++] = Q;
		}
#endif
		pVB->Unlock();
		hResult = pSelectVB->Unlock();
		assert(D3D_OK == hResult);
//		k = 0;
		if (k == 0) {
			selectedPrID = (DWORD) -1;
		}
		SetNumberOfPoints(k);
#endif

//		ReleaseCapture();
		return S_OK;
	}

	HRESULT	hResult;

	D3DXVECTOR3*	pSelectV = NULL;
	LPDIRECT3DVERTEXBUFFER9	pSelectVB = NULL;
	hResult = ResetVertexBuffer(kSelect::NUMBER_OF_POINTS__UNIT, &pSelectVB);
	assert(D3D_OK == hResult);
	assert(pSelectVB);
	hResult = pSelectVB->Lock(0, 0, (void**) &pSelectV, 0);
	assert(D3D_OK == hResult);
	assert(pSelectV);

	UINT	k = 0;
	FLOAT	zNearest = FLT_MAX;
	const CPointRepr*	pPointRepr = __meshPtr->GetPointReprPointer();
	for (DWORD i = 0; i < __meshPtr->GetNumPointReprs(); ++i) {
		DWORD	prID = pPointRepr[i].id;
		const D3DXVECTOR3&	Q = pPointRepr[i].v.p;
#if 1
		D3DXVECTOR3	q;
		D3DXVec3Project(&q, &Q, &viewport, &projectionMatrix, &viewMatrix, &worldMatrix);
		if (q.z < 0 || q.z > 1.f)
			continue;
		FLOAT	f = ceilf(q.x);
		int	qx = q.x < f + .5f ? (int) f : (int) (f + 1.f);
		int	high, low;
		high = iMouseX + 2;
		low = iMouseX - 2;
		if (bDrag) {
			if (iMouseX > iDragBeginX)
				low = iDragBeginX - 2;
			else
				high = iDragBeginX + 2;
		}
		if (qx > high || qx < low)
			continue;
		f = ceilf(q.y);
		int	qy = q.y < f + .5f ? (int) f : (int) (f + 1.f);
		high = iMouseY + 2;
		low = iMouseY - 2;
		if (bDrag) {
			if (iMouseY > iDragBeginY)
				low = iDragBeginY - 2;
			else
				high = iDragBeginY + 2;
		}
		if (qy > high || qy < low)
			continue;
#endif

		//
		if (k >= kSelect::NUMBER_OF_POINTS__UNIT)
			continue;
		if (q.z < zNearest) {
			selectedPrID = prID;
			zNearest = q.z;
		}
		pSelectV[k++] = Q;
	}
	hResult = pSelectVB->Unlock();
	assert(D3D_OK == hResult);
	if (k == 0) {
		selectedPrID = (DWORD) -1;
	}
	SetNumberOfPoints(k);

	return S_OK;
#else
#if 0	// 원래 Pick.cpp 에 있던 코드로 충돌하는 삼각형에 대한 정보를 얻는 부분;
	g_dwNumIntersections = 0L;
#endif
	if (!GetCapture())
		return S_OK;

#if 0	// Brush 모드
	HRESULT		hr;
	IDirect3DDevice9*	pD3Device = DXUTGetD3D9Device();
	D3DVIEWPORT9	viewport;
	pD3Device->GetViewport(&viewport);
{
	if (g__brush) {
		HRESULT	hResult;
		if (!GetCapture() || (g__Shift && g__Control))
			return S_OK;

		POINT	ptCursor;
		GetCursorPos(&ptCursor);
		ScreenToClient(DXUTGetHWND(), &ptCursor);
#if 1
		assert(g__pSelect);
		D3DXVECTOR3*	pSelectV = NULL;
		LPDIRECT3DVERTEXBUFFER9	pSelectVB = NULL;
		hResult = g__pSelect->ResetVertexBuffer(kSelect::NUMBER_OF_POINTS__UNIT, &pSelectVB);
		assert(D3D_OK == hResult);
		assert(pSelectVB);
		hResult = pSelectVB->Lock(0, 0, (void**) &pSelectV, 0);
		assert(D3D_OK == hResult);
		assert(pSelectV);

		UINT	k = 0;
		FLOAT	zNearest = FLT_MAX;
		const CPointRepr*	pPointRepr = g__pMesh->GetPointReprPointer();
#if 1
		LPDIRECT3DVERTEXBUFFER9	pVB = g__pMesh->GetVertexBufferPointer();
		kMesh::D3DXVERTEX*	pV;
		pVB->Lock(0, 0, (void**) &pV, 0);
		FLOAT	aspectRatio = (FLOAT) viewport.Width / viewport.Height;

		D3DXVECTOR3	o = D3DXVECTOR3((FLOAT) ptCursor.x, (FLOAT) ptCursor.y, 0.f);
		D3DXVECTOR3	O;	// Object 공간에서의 좌표
		D3DXVec3Unproject(&O, &o, &viewport, &mProj, &mView, &mWorld);
#endif
#if 1
		const CFace*	pFace = g__pMesh->GetFacePointer();
		DWORD*	pFaceFlag = new DWORD[g__pMesh->GetNumFaces()];
		memset(pFaceFlag, 0, g__pMesh->GetNumFaces() * sizeof(DWORD));
		BRUSHPOINT*	pBrushPointFlag = new BRUSHPOINT[g__pMesh->GetNumPointReprs()];
		memset(pBrushPointFlag, 0, g__pMesh->GetNumPointReprs() * sizeof(BRUSHPOINT));
		for (DWORD iFace = 0; iFace < g__pMesh->GetNumFaces(); ++iFace) {
			const CFace&	face = pFace[iFace];
			// 1. 삼각형의 normal 이 카메라를 보고 있는가?
			// 2. 삼각형을 이루는 각 꼭지점을 이전에 다룬 적이 있는가?
			// 없다면 원 안에 들어오는 꼭지점이 있는가?
			// 3. 카메라에서 꼭지점에 이르는 반직선이
			// 충돌하는 삼각형이 있는가?
			// (1., 2., 3. 모두를 만족하는 꼭지점에 대해서,
			// 그러한 삼각형들만을 고려해서...)

			// 1. 삼각형의 normal 이 카메라를 보고 있는가?
			D3DXVECTOR3	_0 = pPointRepr[face.prID0].v.p;
			D3DXVECTOR3	_1 = pPointRepr[face.prID1].v.p;
			D3DXVECTOR3	_2 = pPointRepr[face.prID2].v.p;
			D3DXVECTOR3	G = (_0 + _1 + _2) / 3.f;	// 삼각형의 무게중심
			D3DXVECTOR3	_01 = _1 - _0;
			D3DXVECTOR3	_02 = _2 - _0;
			D3DXVECTOR3	n;	// 삼각형의 normal 벡터
			D3DXVECTOR3	r = G - O;	// O 에서 G 에 이르는 벡터
			// n 과 r 의 내적이 음수이면 삼각형이 카메라를 보고있다.
			if (D3DXVec3Dot(&r, D3DXVec3Cross(&n, &_01, &_02)) > 0.f)
				continue;

			// 2. 삼각형을 이루는 각 꼭지점을 이전에 다룬 적이 있는가?
			// 없다면 원 안에 들어오는 꼭지점이 있는가?
			bool	bInsideCircle = false;
			for (int i = 0; i < 3; ++i) {
				const DWORD&	prID = face.prID[i];
				if (pBrushPointFlag[prID].flag)	// 이 꼭지점을 다룬 적이 있는가?
					continue;

				// 원 안에 들어오는가?
				pBrushPointFlag[prID].flag = 1;	// 이 꼭지점은 다루었다!
				const D3DXVECTOR3&	Q = pPointRepr[prID].v.p;
				D3DXVECTOR3	q;
				D3DXVec3Project(&q, &Q, &viewport, &mProj, &mView, &mWorld);
				if (q.z < 0 || q.z > 1.f)
					continue;

				D3DXVECTOR2	A((q.x - ptCursor.x) / g__brushRadius, (q.y - ptCursor.y) / g__brushRadius * aspectRatio);
				FLOAT	d2 = D3DXVec2LengthSq(&A);
				if (d2 > 1.f) {
					continue;
				}
				// 원 안에 있다!
				BRUSHPOINT&	brushPoint = pBrushPointFlag[prID];
				brushPoint.flag = 2;	// 다뤄진 이 꼭지점은 원 안에 있다!
				brushPoint.d2 = d2;
				brushPoint.z = q.z;

				bInsideCircle = true;		// 따라서 이 꼭지점을 포함한 삼각형은 나중에 고려할 대상이다.
			}
			if (bInsideCircle)
				pFaceFlag[iFace] = 1;
		}
		// 3. 카메라에서 꼭지점에 이르는 반직선이
		// 충돌하는 삼각형이 있는가?
		// (1., 2., 3. 모두를 만족하는 꼭지점에 대해서,
		// 그러한 삼각형들만을 고려해서...)
		for (DWORD iFace = 0; iFace < g__pMesh->GetNumFaces(); ++iFace) {
			// 1. 고려할 삼각형을 이루는 꼭지점 중에서
			// 2. 원 안에 있는 꼭지점 중에서
			// 다른 고려할 삼각형과 충돌하는 지 알아보자.
			if (pFaceFlag[iFace] == 0)
				continue;
			// 1. 고려할 삼각형안에 있다.
			for (int i = 0; i < 3; ++i) {
				DWORD	prID = pFace[iFace].prID[i];
				if (pBrushPointFlag[prID].flag != 2)
					continue;

				// 충돌 테스트까지 끝날 것이다.
				pBrushPointFlag[prID].flag = 3;
				// 2. 원 안에 있다.
				bool	bCollide = false;
				const CPointRepr&	pointRepr = pPointRepr[prID];
				const D3DXVECTOR3&	Q = pointRepr.v.p;
				for (DWORD kFace = 0; kFace < g__pMesh->GetNumFaces(); ++kFace) {
					if (kFace == iFace)	// 자기 자신은 제외
						continue;
					if (pFaceFlag[kFace] == 0)	// 고려하지않을 삼각형은 제외
						continue;

					FLOAT	fBary1, fBary2;
					FLOAT	fDist;
					const CFace&	face = pFace[kFace];
					D3DXVECTOR3	r = Q - O;
					D3DXVECTOR3	rn;	// normalized r
					D3DXVec3Normalize(&rn, &r);
					if (IntersectTriangle(
						O, rn,

						pPointRepr[face.prID0].v.p,
						pPointRepr[face.prID1].v.p,
						pPointRepr[face.prID2].v.p,

						&fDist, &fBary1, &fBary2
					)) {
						// 자기 뒤에 있는 삼각형과 충돌할 수 있다.
						if (D3DXVec3LengthSq(&r) > fDist * fDist) {
							bCollide = true;	// 앞쪽에 충돌하는 삼각형이 있다.
							break;			// 더 이상 조사할 이유가 없다.
						}
					}
				}
				if (bCollide)	// 충돌하는 삼각형이 있다; 즉 카메라에서는 안 보인다.
					continue;

				// 카메라에서 보이고 원 안에 있다.
				BRUSHPOINT&	brushPoint = pBrushPointFlag[prID];
				if (g__Shift)
					pV[prID].c -= (1.f - brushPoint.d2) * D3DXVECTOR3(0.f, 0.f, .5f);
				else
					pV[prID].c += (1.f - brushPoint.d2) * D3DXVECTOR3(0.5f, 0.f, 0.f);
				CLAMP2(pV[prID].c);

				if (k >= kSelect::NUMBER_OF_POINTS__UNIT)
					continue;
				FLOAT&	z = brushPoint.z;
				if (z < zNearest) {
					g__prID = prID;
					zNearest = z;
				}
				pSelectV[k++] = Q;
			}
		}

		SAFE_DELETE_ARRAY(pFaceFlag);
		SAFE_DELETE_ARRAY(pBrushPointFlag);
#else
		for (DWORD i = 0; i < g__pMesh->GetNumPointReprs(); ++i) {
			DWORD	prID = pPointRepr[i].id;

			D3DXVECTOR3&	Q = pPointRepr[i].v.p;
			D3DXVECTOR3	q;
			D3DXVec3Project(&q, &Q, &viewport, &mProj, &mView, &mWorld);
			if (q.z < 0 || q.z > 1.f)
				continue;

			D3DXVECTOR2	A((q.x - ptCursor.x) / g__brushRadius, (q.y - ptCursor.y) / g__brushRadius * aspectRatio);
			FLOAT	d2 = D3DXVec2LengthSq(&A);
			if (d2 > 1.f)
				continue;

#if 1
			// O 에서 시작에서 OQ 방향으로 나가는 ray 가 충돌하는 삼각형이 있는가?
			D3DXVECTOR3	R = Q - O;
			if (D3DXVec3Dot(&R, &pPointRepr[i].v.n) > 0.f)
				continue;
			BOOL	bHit;
			DWORD	faceID;
			D3DXIntersect(g__pMesh->GetMeshPointer(), &O, &R, &bHit, &faceID, NULL, NULL, NULL, NULL, NULL);
			if (bHit) {
				const CFace&	face = (g__pMesh->GetFacePointer())[faceID];
				if (face.prID0 != prID && prID != face.prID1 && prID != face.prID2)
					continue;
			}
#endif

#if 1
			DWORD	index = prID;
#else
			DWORD	index = (g__pMesh->GetPointReprIDMappedPointer())[prID];
#endif
			if (g__Shift)
				pV[index].c -= (1.f - d2) * D3DXVECTOR3(0.f, 0.f, .5f);
			else
				pV[index].c += (1.f - d2) * D3DXVECTOR3(0.5f, 0.f, 0.f);
			CLAMP2(pV[index].c);

			if (k >= kSelect::NUMBER_OF_POINTS__UNIT)
				continue;
			if (q.z < zNearest) {
				g__prID = prID;
				zNearest = q.z;
			}
			pSelectV[k++] = Q;
		}
#endif
		pVB->Unlock();
		hResult = pSelectVB->Unlock();
		assert(D3D_OK == hResult);
		g__NSelectedPoints = k;
//		k = 0;
		if (k == 0) {
			g__prID = (DWORD) -1;
			g__lastPrID = (DWORD) -1;
		}
		g__pSelect->SetNumberOfPoints(k);
#endif

//		ReleaseCapture();
		return S_OK;
	}
}
#endif
#if 0	// 원래 Pick.cpp 에 있던 코드로 2차원 화면 상의 한 점을 찍었을 때,
	// 3차원 반직선(Ray)을 생성하는 부분;
	// 이해가 안 가는 부분이 있고, 이해만 충분히 하면 계산된 행렬을 모든 Vertex 에 대해서 쓸 수 있어 유용하다.
	// 단지 주석 처리만 한 이유이다.
	D3DXVECTOR3	vPickRayDir;
	D3DXVECTOR3	vPickRayOrig;
	const D3DSURFACE_DESC*	pd3dsdBackBuffer = DXUTGetD3D9BackBufferSurfaceDesc();

	// Get the Pick ray from the mouse position
//	if (GetCapture())
{
	const D3DXMATRIX*	pmatProj = g_Camera.GetProjMatrix();

	POINT	ptCursor;
	GetCursorPos(&ptCursor);
	ScreenToClient(DXUTGetHWND(), &ptCursor);

	// Compute the vector of the Pick ray in screen space
	D3DXVECTOR3 v;
#if 1
	v.x = 2.f * ptCursor.x / pd3dsdBackBuffer->Width - 1;
	v.y = -(2.f * ptCursor.y / pd3dsdBackBuffer->Height - 1);
	v.x /= pmatProj->_11;
	v.y /= pmatProj->_22;
	v.z =  1.f;
#else
	v.x =  (((2.0f * ptCursor.x) / pd3dsdBackBuffer->Width) - 1) / pmatProj->_11;
	v.y = -(((2.0f * ptCursor.y) / pd3dsdBackBuffer->Height) - 1) / pmatProj->_22;
	v.z =  1.0f;
#endif

	// Get the inverse view matrix
	const D3DXMATRIX	matView = *g_Camera.GetViewMatrix();
	const D3DXMATRIX	matWorld = *g_Camera.GetWorldMatrix();
	D3DXMATRIX	mWorldView = matWorld * matView;
	D3DXMATRIX	m;
	D3DXMatrixInverse(&m, NULL, &mWorldView);

	// Transform the screen space Pick ray into 3D space
	vPickRayDir.x  = v.x*m._11 + v.y*m._21 + v.z*m._31;
	vPickRayDir.y  = v.x*m._12 + v.y*m._22 + v.z*m._32;
	vPickRayDir.z  = v.x*m._13 + v.y*m._23 + v.z*m._33;
	vPickRayOrig.x = m._41;
	vPickRayOrig.y = m._42;
	vPickRayOrig.z = m._43;
}
#endif

#if 0
	if (!(g__Shift || g__Control))
		return S_OK;
//	if (g__Shift && g__Control)
//		return S_OK;

	if (g__Control) {
		HRESULT	hResult;

		assert(g__pSelect);
		D3DXVECTOR3*	pSelectV = NULL;
		LPDIRECT3DVERTEXBUFFER9	pSelectVB = NULL;
		hResult = g__pSelect->ResetVertexBuffer(kSelect::NUMBER_OF_POINTS__UNIT, &pSelectVB);
		assert(D3D_OK == hResult);
		assert(pSelectVB);
		hResult = pSelectVB->Lock(0, 0, (void**) &pSelectV, 0);
		assert(D3D_OK == hResult);
		assert(pSelectV);

		UINT	k = 0;
		FLOAT	zNearest = FLT_MAX;
		const CPointRepr*	pPointRepr = g__pMesh->GetPointReprPointer();
		for (DWORD i = 0; i < g__pMesh->GetNumPointReprs(); ++i) {
			DWORD	prID = pPointRepr[i].id;
			const D3DXVECTOR3&	Q = pPointRepr[i].v.p;
#if 1
			D3DXVECTOR3	q;
			D3DXVec3Project(&q, &Q, &viewport, &mProj, &mView, &mWorld);
			if (q.z < 0 || q.z > 1.f)
				continue;
			FLOAT	f = ceilf(q.x);
			int	qx = q.x < f + .5f ? (int) f : (int) (f + 1.f);
			int	high, low;
			high = g__iMouseX + 2;
			low = g__iMouseX - 2;
			if (g__bDrag) {
				if (g__iMouseX > g__iDragBeginX)
					low = g__iDragBeginX - 2;
				else
					high = g__iDragBeginX + 2;
			}
			if (qx > high || qx < low)
				continue;
			f = ceilf(q.y);
			int	qy = q.y < f + .5f ? (int) f : (int) (f + 1.f);
			high = g__iMouseY + 2;
			low = g__iMouseY - 2;
			if (g__bDrag) {
				if (g__iMouseY > g__iDragBeginY)
					low = g__iDragBeginY - 2;
				else
					high = g__iDragBeginY + 2;
			}
			if (qy > high || qy < low)
				continue;
#endif

			//
			if (k >= kSelect::NUMBER_OF_POINTS__UNIT)
				continue;
			if (q.z < zNearest) {
				g__prID = prID;
				zNearest = q.z;
			}
			pSelectV[k++] = Q;
		}
		hResult = pSelectVB->Unlock();
		assert(D3D_OK == hResult);
		g__NSelectedPoints = k;
		if (k == 0) {
			g__prID = (DWORD) -1;
		}
		g__pSelect->SetNumberOfPoints(k);

		return S_OK;
	}
#endif

	// g__Shift
	// Get the Picked triangle
//	if (GetCapture())
#if 0	// 원래 Pick.cpp 에 있던 코드로 충돌하는 삼각형에 대한 정보를 얻는 부분;
	// 여기서는 더 이상 사용되지 않으나 나중에 참고할 수 있도 있어 단지 주석처리만 한다.
{
        LPD3DXMESH              pMesh;
#if 1
#if 1
	pMesh = g__pMesh->GetMeshPointer();
#else
	pMesh = g_Mesh.GetMesh();
#endif
#else
        g_Mesh.GetMesh()->CloneMeshFVF(D3DXMESH_MANAGED,
            g_Mesh.GetMesh()->GetFVF(), pD3Device, &pMesh);
#endif

        LPDIRECT3DVERTEXBUFFER9 pVB;
        LPDIRECT3DINDEXBUFFER9  pIB;

        pMesh->GetVertexBuffer( &pVB );
        pMesh->GetIndexBuffer( &pIB );

	D3DVERTEX*	pVertices;
#if 1
	bool	b32BitIndex = pMesh->GetOptions() & D3DXMESH_32BIT;
	BYTE*	pIndices;
#else
        WORD*      pIndices;
#endif

        pIB->Lock( 0, 0, (void**)&pIndices, 0 );
        pVB->Lock( 0, 0, (void**)&pVertices, 0 );

        if( g_bUseD3DXIntersect )
        {
            // When calling D3DXIntersect, one can get just the closest intersection and not
            // need to work with a D3DXBUFFER.  Or, to get all intersections between the ray and
            // the mesh, one can use a D3DXBUFFER to receive all intersections.  We show both
            // methods.
            if( !g_bAllHits )
            {
                // Collect only the closest intersection
                BOOL bHit;
                DWORD dwFace;
                FLOAT fBary1, fBary2, fDist;
                D3DXIntersect(pMesh, &vPickRayOrig, &vPickRayDir, &bHit, &dwFace, &fBary1, &fBary2, &fDist,
                    NULL, NULL);
                if( bHit )
                {
                    g_dwNumIntersections = 1;
                    g_IntersectionArray[0].dwFace = dwFace;
                    g_IntersectionArray[0].fBary1 = fBary1;
                    g_IntersectionArray[0].fBary2 = fBary2;
                    g_IntersectionArray[0].fDist = fDist;
                }
                else
                {
                    g_dwNumIntersections = 0;
                }
            }
            else
            {
                // Collect all intersections
                BOOL bHit;
                LPD3DXBUFFER pBuffer = NULL;
                D3DXINTERSECTINFO* pIntersectInfoArray;
                if( FAILED( hr = D3DXIntersect(pMesh, &vPickRayOrig, &vPickRayDir, &bHit, NULL, NULL, NULL, NULL,
                    &pBuffer, &g_dwNumIntersections) ) )
                {
#if 0
                    SAFE_RELEASE(pMesh);
#endif
                    SAFE_RELEASE(pVB);
                    SAFE_RELEASE(pIB);
                    return hr;
                }
                if( g_dwNumIntersections > 0 )
                {
                    pIntersectInfoArray = (D3DXINTERSECTINFO*)pBuffer->GetBufferPointer();
                    if( g_dwNumIntersections > MAX_INTERSECTIONS )
                        g_dwNumIntersections = MAX_INTERSECTIONS;
                    for( DWORD iIntersection = 0; iIntersection < g_dwNumIntersections; iIntersection++ )
                    {
                        g_IntersectionArray[iIntersection].dwFace = pIntersectInfoArray[iIntersection].FaceIndex;
                        g_IntersectionArray[iIntersection].fBary1 = pIntersectInfoArray[iIntersection].U;
                        g_IntersectionArray[iIntersection].fBary2 = pIntersectInfoArray[iIntersection].V;
                        g_IntersectionArray[iIntersection].fDist = pIntersectInfoArray[iIntersection].Dist;
                    }
                }
                SAFE_RELEASE( pBuffer );
            }

        }
        else
        {
            // Not using D3DX
#if 1
            DWORD dwNumFaces = g__pMesh->GetMeshPointer()->GetNumFaces();
#else
            DWORD dwNumFaces = g_Mesh.GetMesh()->GetNumFaces();
#endif
            FLOAT fBary1, fBary2;
            FLOAT fDist;
            for( DWORD i=0; i<dwNumFaces; i++ )
            {
#if 1
			int	i0, i1, i2;
			if (b32BitIndex) {
				i0 = ((DWORD*) pIndices)[3 * i + 0];
				i1 = ((DWORD*) pIndices)[3 * i + 1];
				i2 = ((DWORD*) pIndices)[3 * i + 2];
			} else {
				i0 = ((WORD*) pIndices)[3 * i + 0];
				i1 = ((WORD*) pIndices)[3 * i + 1];
				i2 = ((WORD*) pIndices)[3 * i + 2];
			}
			D3DXVECTOR3	v0 = pVertices[i0].p;
			D3DXVECTOR3	v1 = pVertices[i1].p;
			D3DXVECTOR3	v2 = pVertices[i2].p;
#if 0
D3DXVECTOR3 v0 = pVertices[(b32BitIndex ? ((DWORD*) pIndices) : ((WORD*) pIndices))[3 * i + 0]].p;
D3DXVECTOR3 v1 = pVertices[(b32BitIndex ? ((DWORD*) pIndices) : ((WORD*) pIndices))[3 * i + 1]].p;
D3DXVECTOR3 v2 = pVertices[(b32BitIndex ? ((DWORD*) pIndices) : ((WORD*) pIndices))[3 * i + 2]].p;
#endif
#else
                D3DXVECTOR3 v0 = pVertices[pIndices[3*i+0]].p;
                D3DXVECTOR3 v1 = pVertices[pIndices[3*i+1]].p;
                D3DXVECTOR3 v2 = pVertices[pIndices[3*i+2]].p;
#endif

                // Check if the Pick ray passes through this point
                if( IntersectTriangle( vPickRayOrig, vPickRayDir, v0, v1, v2,
                                       &fDist, &fBary1, &fBary2 ) )
                {
                    if( g_bAllHits || g_dwNumIntersections == 0 || fDist < g_IntersectionArray[0].fDist )
                    {
                        if( !g_bAllHits )
                            g_dwNumIntersections = 0;
                        g_IntersectionArray[g_dwNumIntersections].dwFace = i;
                        g_IntersectionArray[g_dwNumIntersections].fBary1 = fBary1;
                        g_IntersectionArray[g_dwNumIntersections].fBary2 = fBary2;
                        g_IntersectionArray[g_dwNumIntersections].fDist = fDist;
                        g_dwNumIntersections++;
                        if( g_dwNumIntersections == MAX_INTERSECTIONS )
                            break;
                    }
                }
            }
        }

	// Now, for each intersection, add a triangle to g_pVB and compute texture coordinates
	if (g_dwNumIntersections > 0) {
            D3DVERTEX* v;
            D3DVERTEX* vThisTri;
#if 1
#else
            WORD* iThisTri;
#endif
            D3DVERTEX  v1, v2, v3;
            INTERSECTION* pIntersection;

            g_pVB->Lock( 0, 0, (void**)&v, 0 );

            for( DWORD iIntersection = 0; iIntersection < g_dwNumIntersections; iIntersection++ )
            {
                pIntersection = &g_IntersectionArray[iIntersection];

                vThisTri = &v[iIntersection * 3];
#if 1
		int	i = pIntersection->dwFace;
		int	i0, i1, i2;
		if (b32BitIndex) {
			i0 = ((DWORD*) pIndices)[3 * i + 0];
			i1 = ((DWORD*) pIndices)[3 * i + 1];
			i2 = ((DWORD*) pIndices)[3 * i + 2];
		} else {
			i0 = ((WORD*) pIndices)[3 * i + 0];
			i1 = ((WORD*) pIndices)[3 * i + 1];
			i2 = ((WORD*) pIndices)[3 * i + 2];
		}
		vThisTri[0] = pVertices[i0];
		vThisTri[1] = pVertices[i1];
		vThisTri[2] = pVertices[i2];
#else
                iThisTri = &pIndices[3*pIntersection->dwFace];
                // get vertices hit
                vThisTri[0] = pVertices[iThisTri[0]];
                vThisTri[1] = pVertices[iThisTri[1]];
                vThisTri[2] = pVertices[iThisTri[2]];
#endif

                // If all you want is the vertices hit, then you are done.  In this sample, we
                // want to show how to infer texture coordinates as well, using the BaryCentric
                // coordinates supplied by D3DXIntersect
                FLOAT dtu1 = vThisTri[1].tu - vThisTri[0].tu;
                FLOAT dtu2 = vThisTri[2].tu - vThisTri[0].tu;
                FLOAT dtv1 = vThisTri[1].tv - vThisTri[0].tv;
                FLOAT dtv2 = vThisTri[2].tv - vThisTri[0].tv;
                pIntersection->tu = vThisTri[0].tu + pIntersection->fBary1 * dtu1 + pIntersection->fBary2 * dtu2;
                pIntersection->tv = vThisTri[0].tv + pIntersection->fBary1 * dtv1 + pIntersection->fBary2 * dtv2;
            }

            g_pVB->Unlock();
	}

	pVB->Unlock();
	pIB->Unlock();

#if 0
	SAFE_RELEASE(pMesh);
#endif
	SAFE_RELEASE(pVB);
	SAFE_RELEASE(pIB);
}
#endif

	return S_OK;
#endif
}
#endif

bool
kSelect::IntersectTriangle(
	const D3DXVECTOR3&	orig,
	const D3DXVECTOR3&	dir,
	const D3DXVECTOR3&	v0,
	const D3DXVECTOR3&	v1,
	const D3DXVECTOR3&	v2,

	FLOAT*	t,
	FLOAT*	u,
	FLOAT*	v
)
{
	// Find vectors for two edges sharing vert0
	D3DXVECTOR3	edge1 = v1 - v0;
	D3DXVECTOR3	edge2 = v2 - v0;

	// Begin calculating determinant - also used to calculate U parameter
	D3DXVECTOR3	pvec;
	D3DXVec3Cross(&pvec, &dir, &edge2);

	// If determinant is near zero, ray lies in plane of triangle
	FLOAT	det = D3DXVec3Dot(&edge1, &pvec);

	D3DXVECTOR3	tvec;
	if (det > 0) {
		tvec = orig - v0;
	} else {
		tvec = v0 - orig;
		det = -det;
	}

	if (det < 0.0001f)
		return FALSE;

	// Calculate U parameter and test bounds
	*u = D3DXVec3Dot(&tvec, &pvec);
	if (*u < 0.0f || *u > det)
		return FALSE;

	// Prepare to test V parameter
	D3DXVECTOR3	qvec;
	D3DXVec3Cross(&qvec, &tvec, &edge1);

	// Calculate V parameter and test bounds
	*v = D3DXVec3Dot(&dir, &qvec);
	if (*v < 0.0f || *u + *v > det)
		return FALSE;

	// Calculate t, scale parameters, ray intersects triangle
	*t = D3DXVec3Dot(&edge2, &qvec);
	FLOAT	fInvDet = 1.0f / det;
	*t *= fInvDet;
	*u *= fInvDet;
	*v *= fInvDet;

	return TRUE;
}

void	kSelect::ShowAngle(const D3DXMATRIX& worldMatrix, const D3DXMATRIX& viewMatrix, const D3DXMATRIX& projectionMatrix, const POINT& startPoint, const POINT& endPoint, float zDepth)
{
	if (startPoint.x == -1)
		return;

	LPDIRECT3DVERTEXBUFFER9		angleVertexBuffer = 0;
	LPDIRECT3DINDEXBUFFER9		angleIndexBuffer = 0;
do {
	D3DVIEWPORT9	viewport;
	__devicePtr->GetViewport(&viewport);

	HRESULT hResult = __devicePtr->CreateVertexBuffer(
		3 * sizeof(D3DXVECTOR3),
		D3DUSAGE_WRITEONLY,
		0,
		D3DPOOL_MANAGED,
		&angleVertexBuffer,
		NULL
	);
	assert(hResult == D3D_OK);
	hResult = __devicePtr->CreateIndexBuffer(
#if 1
		3 * sizeof(DWORD),
#else
		2 * sizeof(CEdge::LineIndex),
#endif
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX32,
		D3DPOOL_MANAGED,
		&angleIndexBuffer,
		NULL
	);
	assert(D3D_OK == hResult);

	D3DXVECTOR3*	anglePointPtr = NULL;
	hResult = angleVertexBuffer->Lock(0, 0, (void**) &anglePointPtr, 0);
	assert(D3D_OK == hResult);
	assert(anglePointPtr);
	anglePointPtr[0].x = (2.f * endPoint.x / viewport.Width - 1.f);
	anglePointPtr[0].y = -(2.f * endPoint.y / viewport.Height - 1.f);
	anglePointPtr[1].x = (2.f * startPoint.x / viewport.Width - 1.f);
	anglePointPtr[1].y = -(2.f * startPoint.y / viewport.Height - 1.f);
	anglePointPtr[2].x = (2.f * endPoint.x / viewport.Width - 1.f);
	anglePointPtr[2].y = -(2.f * startPoint.y / viewport.Height - 1.f);
	hResult = angleVertexBuffer->Unlock();
	assert(D3D_OK == hResult);

	// point
{
	hResult = this->m__pEffect->SetInt("g__mode", 2);
	assert(D3D_OK == hResult);
	hResult = this->m__pEffect->SetFloat("pointSize", 3.f);
	assert(D3D_OK == hResult);
	hResult = this->m__pEffect->SetVector("pointColor", &D3DXVECTOR4(1, 0, 0, 0));
	assert(D3D_OK == hResult);
	D3DXHANDLE	hTechnique = this->m__pEffect->GetTechnique(0);
	assert(hTechnique);
	hResult = this->m__pEffect->SetTechnique(hTechnique);
	assert(S_OK == hResult);
	UINT	NumberOfPasses;
	hResult = this->m__pEffect->Begin(&NumberOfPasses, 0);
	assert(D3D_OK == hResult);
	for (UINT iPass = 0; iPass < NumberOfPasses; ++iPass) {
		hResult = this->m__pEffect->BeginPass(iPass);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetVertexDeclaration(this->m__pDeclaration);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetStreamSource(0, angleVertexBuffer, 0, sizeof(D3DXVECTOR3));
		assert(D3D_OK == hResult);
		hResult = __devicePtr->DrawPrimitive(D3DPT_POINTLIST, endPoint.x == -1 ? 1 : 0, endPoint.x == -1 ? 1 : 2);
		assert(D3D_OK == hResult);
		hResult = this->m__pEffect->EndPass();
		assert(S_OK == hResult);
	}
	hResult = this->m__pEffect->End();
	assert(S_OK == hResult);
}

	if (endPoint.x == -1)
		break;

	// line
#if 1
	DWORD*			angleIndicesPtr;
#else
	CEdge::LineIndex*	angleIndicesPtr;
#endif
	angleIndexBuffer->Lock(0, 0, (void**) &angleIndicesPtr, 0);
#if 1
	angleIndicesPtr[0] = 0;
	angleIndicesPtr[1] = 1;
	angleIndicesPtr[2] = 2;
#else
	angleIndicesPtr[0].iStart = 0;
	angleIndicesPtr[0].iEnd = 1;
	angleIndicesPtr[1].iStart = 1;
	angleIndicesPtr[1].iEnd = 2;
#endif
	angleIndexBuffer->Unlock();

	hResult = this->m__pEffect->SetInt("g__mode", 2);
	assert(D3D_OK == hResult);
	hResult = this->m__pEffect->SetFloat("g__zDepth", zDepth);
	assert(D3D_OK == hResult);
	hResult = this->m__pEffect->SetVector("pointColor", &D3DXVECTOR4(1, 0, 1, 1));
	D3DXHANDLE	hTechnique = this->m__pEffect->GetTechnique(0);
	assert(hTechnique);
	hResult = this->m__pEffect->SetTechnique(hTechnique);
	assert(S_OK == hResult);
	UINT	NumberOfPasses;
	hResult = this->m__pEffect->Begin(&NumberOfPasses, 0);
	assert(D3D_OK == hResult);
	for (UINT iPass = 0; iPass < NumberOfPasses; ++iPass) {
		hResult = this->m__pEffect->BeginPass(iPass);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetVertexDeclaration(this->m__pDeclaration);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetStreamSource(0, angleVertexBuffer, 0, sizeof(D3DXVECTOR3));
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetIndices(angleIndexBuffer);
		assert(D3D_OK == hResult);
#if 1
		hResult = __devicePtr->DrawIndexedPrimitive(D3DPT_LINESTRIP, 0, 0, 3, 0, 2);
#else
		hResult = __devicePtr->DrawIndexedPrimitive(D3DPT_LINELIST, 0, 0, 3, 0, 2);
#endif
//		hResult = __devicePtr->DrawPrimitive(D3DPT_LINELIST, 0, numLines);
		assert(D3D_OK == hResult);
		hResult = this->m__pEffect->EndPass();
		assert(S_OK == hResult);
	}
	hResult = this->m__pEffect->End();
	assert(S_OK == hResult);
} while (0);
	if (angleVertexBuffer) {
		angleVertexBuffer->Release();
		angleVertexBuffer = NULL;
	}
	if (angleIndexBuffer) {
		angleIndexBuffer->Release();
		angleIndexBuffer = 0;
	}
}

void	kSelect::GetSelectedFaceIDList(std::vector<DWORD>& selectedFaceIDList)
{
	DWORD*	selectFaceIDFlagPtr = 0;
do {
	DWORD	numFaces = __meshPtr->GetNumFaces();
	selectFaceIDFlagPtr = new DWORD[numFaces];
	FillMemory(selectFaceIDFlagPtr, numFaces * sizeof(DWORD), 0);
//	DWORD	numPointReprs = __meshPtr->GetNumPointReprs();
#ifdef __USE_A_OFFSET__
//	const DWORD*	pointer_to__a_vVertexID				= __meshPtr->pointer_to__a_vVertexID;
//	const int*	pointer_to__a_vVertexID_a_offset		= __meshPtr->pointer_to__a_vVertexID_a_offset;
//	const EDGE*	pointer_to__a_vEdgeID				= __meshPtr->pointer_to__a_vEdgeID;
//	const int*	pointer_to__a_vEdgeID_a_offset			= __meshPtr->pointer_to__a_vEdgeID_a_offset;
	const FACE_ID_LOCAL_INDEX*	pointer_to__a_vFaceIDLocalIndex	= __meshPtr->pointer_to__a_vFaceIDLocalIndex;
	const int*	pointer_to__a_vFaceIDLocalIndex_a_offset	= __meshPtr->pointer_to__a_vFaceIDLocalIndex_a_offset;
//	const DWORD*	pointer_to__a_faceID				= __meshPtr->pointer_to__a_faceID;
//	const int*	pointer_to__a_faceID_a_offset			= __meshPtr->pointer_to__a_faceID_a_offset;
#else	// __USE_A_OFFSET__/!__USE_A_OFFSET__
	const CPointRepr*	pointReprList = __meshPtr->GetPointReprPointer();
#endif	// !__USE_A_OFFSET__
	for (std::set<DWORD>::const_iterator it = __selectedPrIDSet.begin(); it != __selectedPrIDSet.end(); ++it) {
		DWORD	prID = *it;
#ifdef __USE_A_OFFSET__
		for (int k = pointer_to__a_vFaceIDLocalIndex_a_offset[prID]; k < pointer_to__a_vFaceIDLocalIndex_a_offset[prID + 1]; ++k) {
			const FACE_ID_LOCAL_INDEX&	faceIdLocalIndex = pointer_to__a_vFaceIDLocalIndex[k];
			selectFaceIDFlagPtr[faceIdLocalIndex.ID] = 1;
		}
#else
		const std::vector<FACE_ID_LOCAL_INDEX>&	faceIdLocalIndexList = pointReprList[prID].vFaceIdLocalIndex;
		for (int i = 0; i < (int) faceIdLocalIndexList.size(); ++i) {
			const FACE_ID_LOCAL_INDEX&	faceIdLocalIndex = faceIdLocalIndexList.at(i);
			selectFaceIDFlagPtr[faceIdLocalIndex.ID] = 1;
		}
#endif
	}

	for (DWORD faceID = 0; faceID < numFaces; ++faceID) {
		if (selectFaceIDFlagPtr[faceID])
			selectedFaceIDList.push_back(faceID);
	}
} while (0);
	if (selectFaceIDFlagPtr) {
		delete[] selectFaceIDFlagPtr;
		selectFaceIDFlagPtr = 0;
	}
}

void	kSelect::selection(kMesh& mesh, const CModelViewerCamera2& camera)
{
	if (!&mesh)
		return;
////////////////////////////////////////////////////////////////////////
	kState&	state = *mesh.statePtr();
//		meshRef.selection(state.g__iMouseX, state.g__iMouseY, state.g__bDrag, state.g__Shift, state.g__iDragBeginX, state.g__iDragBeginY, state.g__NSelectedPoints, state.g__prID);
	int	g__iMouseX = state.g__iMouseX;
	int	g__iMouseY = state.g__iMouseY;
	int	g__iDragBeginX = state.g__iDragBeginX;
	int	g__iDragBeginY = state.g__iDragBeginY;
	bool	g__bDrag = state.g__bDrag;
	UINT&	g__NSelectedPoints = state.g__NSelectedPoints;
	DWORD&	g__prID = state.g__prID;
////////////////////////////////////////////////////////////////////////


	const CPointRepr*	pointReprArray = mesh.GetPointReprPointer();
	unsigned	numPointReprs = mesh.GetNumPointReprs();
	DWORD*	selectedPointReprIDArray = new DWORD[numPointReprs];
	FillMemory(selectedPointReprIDArray, numPointReprs * sizeof(DWORD), 0);
do {
	HRESULT	hResult;

	FLOAT	zNearest = FLT_MAX;
	D3DVIEWPORT9	viewport;
	__devicePtr->GetViewport(&viewport);
#if 1	// D3DXVec3Project 에 사용되는 행렬을 미리 계산; 모든 점들에 대해서 공통
	D3DXMATRIX	m1;
	D3DXMatrixMultiply(&m1, camera.GetWorldMatrix(), camera.GetViewMatrix());
	D3DXMATRIX	m;
	D3DXMatrixMultiply(&m, &m1, camera.GetProjMatrix());
#if 0
	g__iMouseX = (int) (2.f * (float) g__iMouseX / (viewport.Width - 1) - 1.f);
	g__iMouseY = (int) (1.f - 2.f * (float) g__iMouseY / (viewport.Height - 1));
	g__iDragBeginX = (int) (2.f * (float) g__iDragBeginX / (viewport.Width - 1) - 1.f);
	g__iDragBeginY = (int) (1.f - 2.f * (float) g__iDragBeginX / (viewport.Height - 1));
#endif
#endif
	for (DWORD prID = 0; prID < (DWORD) numPointReprs; ++prID) {
//		DWORD	prID = pointReprArray[prID].id;
		const D3DXVECTOR3&	Q = pointReprArray[prID].v.position;
#if 1
		D3DXVECTOR3	q;
#if 1
		D3DXVec3TransformCoord(&q, &Q, &m);	// projection space:
							//	(-1, -1): left, lower
							//	x: [-1, 1], y: [-1, 1]
		// Convert to screen space:
		//	(0, 0): left, upper
		//	x: [0, W], y: [H, 0]
// (-1, 1)	(0, 0)
// (1, -1)	(W - 1, H - 1)
// (x, y)	((x + 1) / 2 * W - 1, (1 - y) / 2 * H - 1)
		q.x = (q.x + 1) / 2 * viewport.Width;
		q.y = (1 - q.y) / 2 * viewport.Height;
		q.z = viewport.MinZ + q.z * (viewport.MaxZ - viewport.MinZ);
#else
		D3DXVECTOR3	qq;
		D3DXVec3TransformCoord(&qq, &Q, &m);
		qq.z = viewport.MinZ + qq.z * (viewport.MaxZ - viewport.MinZ);
		D3DXVec3Project(&q, &Q, &viewport, camera.GetProjMatrix(), camera.GetViewMatrix(), camera.GetWorldMatrix());
		(void) fprintf(stdout, "qq: [%f, %f, %f], q: [%f, %f, %f]\n", qq.x, qq.y, qq.z, q.x, q.y, q.z);
#endif
		if (q.z < 0 || q.z > 1.f)
			continue;

		FLOAT	f = ceilf(q.x);
		int	qx = q.x < f + .5f ? (int) f : (int) (f + 1.f);
		int	high, low;
		high = g__iMouseX + 2;
		low = g__iMouseX - 2;
		if (g__bDrag) {
			if (g__iMouseX > g__iDragBeginX)
				low = g__iDragBeginX - 2;
			else
				high = g__iDragBeginX + 2;
		}
		if (qx > high || qx < low)
			continue;
		f = ceilf(q.y);
		int	qy = q.y < f + .5f ? (int) f : (int) (f + 1.f);
		high = g__iMouseY + 2;
		low = g__iMouseY - 2;
		if (g__bDrag) {
			if (g__iMouseY > g__iDragBeginY)
				low = g__iDragBeginY - 2;
			else
				high = g__iDragBeginY + 2;
		}
		if (qy > high || qy < low)
			continue;
#endif

		// selected!
		selectedPointReprIDArray[prID] = 1;
		if (q.z < zNearest) {
			g__prID = prID;
			zNearest = q.z;
		}
#if 0
		if (__selectedPrIDSet.size() >= kSelect::NUMBER_OF_POINTS__UNIT)
			continue;
		if (q.z < zNearest) {
			g__prID = prID;
			zNearest = q.z;
		}
//		pSelectV[k++] = Q;
		if (!shiftKeyPressed)
			__selectedPrIDSet.insert(prID);
		else {
			std::set<DWORD>::iterator	it = __selectedPrIDSet.find(prID);
			if (it != __selectedPrIDSet.end())
				__selectedPrIDSet.erase(it);
			else
				__selectedPrIDSet.insert(prID);
		}
#endif
	}
	// CTRL 키만 눌렸을 경우 __selectedPrIDSet 를 새로 설정
	// SHIFT 키가 눌렸을 경우 __selectedPrIDSet 에 추가
	// ALT 키가 눌렸을 경우 __selectedPrIDSet 에서 삭제
	if (!(state.g__Shift || state.g__Alt))
		__selectedPrIDSet.clear();

	for (DWORD prID = 0; prID < (DWORD) numPointReprs; ++prID) {
		if (selectedPointReprIDArray[prID] == 0)
			continue;

		if (state.g__Alt) {
			std::set<DWORD>::iterator	it = __selectedPrIDSet.find(prID);
			if (it != __selectedPrIDSet.end())
				__selectedPrIDSet.erase(it);
		} else {
			__selectedPrIDSet.insert(prID);
		}
	}
//	for (std::set<DWORD>::const_iterator it = __selectedPrIDSet.begin(); it != __selectedPrIDSet.end(); ++it) {
//		pSelectV[numSelectedPoints++] = pointReprArray[*it].v.position;
//	}
	UINT	numSelectedPoints = __selectedPrIDSet.size();
//	cout << "numSelectedPoints = " << numSelectedPoints << endl;
	g__NSelectedPoints = numSelectedPoints;
	m__NumberOfPoints = numSelectedPoints;
	if (numSelectedPoints == 0) {
		g__prID = (DWORD) -1;
		break;
	}

	// numSelectedPoints > 0
	SAFE_RELEASE(m__pVertexBuffer);
	hResult = __devicePtr->CreateVertexBuffer(
		kSelect::Stride * numSelectedPoints,
		D3DUSAGE_WRITEONLY,
		0,
		D3DPOOL_MANAGED,
		&m__pVertexBuffer,
		NULL
	);
	assert(D3D_OK == hResult);
	D3DXVECTOR3*	pSelectV = NULL;
	hResult = m__pVertexBuffer->Lock(0, 0, (void**) &pSelectV, 0);
	assert(D3D_OK == hResult);
	// fill vertex buffer
	int	n = 0;
	for (std::set<DWORD>::const_iterator it = __selectedPrIDSet.begin(); it != __selectedPrIDSet.end(); ++it) {
		pSelectV[n++] = pointReprArray[*it].v.position;
	}
	hResult = m__pVertexBuffer->Unlock();
	assert(D3D_OK == hResult);
} while (0);
	SAFE_DELETE_ARRAY(selectedPointReprIDArray);
}
