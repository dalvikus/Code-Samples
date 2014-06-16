#include <mesh/kShowNormal.h>
#include <assert.h>

UINT	kShowNormal::Stride = sizeof(kShowNormal::Vertex);
D3DVERTEXELEMENT9	kShowNormal::Element[3] = {
	{
		0,
		0 * sizeof(D3DXVECTOR3),
		D3DDECLTYPE_FLOAT3,
		D3DDECLMETHOD_DEFAULT,
		D3DDECLUSAGE_POSITION,
		0
	},
	{
		0,
		1 * sizeof(D3DXVECTOR3),
		D3DDECLTYPE_FLOAT3,
		D3DDECLMETHOD_DEFAULT,
		D3DDECLUSAGE_NORMAL,
		0
	},

	D3DDECL_END()
};

#if 3	// porting_DxViewer
kShowNormal::kShowNormal()
{
	__init();
}

void	kShowNormal::__init()
{
	m__pEffect = NULL;
	m__pVertexBuffer = NULL;
	m__pDeclaration = NULL;
}
int	kShowNormal::setAll(
	const LPD3DXMESH	meshPtr,
	const LPDIRECT3DDEVICE9	devicePtr,
	LPCTSTR		effectSourceFilePtr
)
{
	assert(devicePtr);
	this->m__pDevice = devicePtr;

	bool	bOk = true;
	HRESULT	hResult = D3D_OK;
	bool	False = false;
do {
	assert(devicePtr);
	if (!devicePtr) {
		bOk = false;
		break;
	}

	hResult = devicePtr->CreateVertexDeclaration(kShowNormal::Element, &this->m__pDeclaration);
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
	this->m__NumLines = 0;

	SetMesh(meshPtr);

	return 0;
}
#else
kShowNormal::kShowNormal(
	const LPDIRECT3DDEVICE9	pDevice,
	const LPCTSTR	pSrcFile,
	bool*	pbOk
)
{
	assert(pDevice);
	this->m__pDevice = pDevice;

	this->m__pEffect = NULL;
	this->m__pVertexBuffer = NULL;
	this->m__pDeclaration = NULL;

	bool	bOk = true;
	HRESULT	hResult = D3D_OK;
	bool	False = false;
do {
	assert(pDevice);
	if (!pDevice) {
		bOk = false;
		break;
	}

	hResult = pDevice->CreateVertexDeclaration(kShowNormal::Element, &this->m__pDeclaration);
	assert(D3D_OK == hResult);

	DWORD	Flags = 0;
	LPD3DXBUFFER	pCompilationErrors = NULL;
	hResult = D3DXCreateEffectFromFile(
		pDevice,	// LPDIRECT3DDEVICE9	pDevice
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
	this->m__NumLines = 0;

	if (pbOk)
		*pbOk = bOk;
}
#endif

kShowNormal::~kShowNormal()
{
	Destroy();
}
void	kShowNormal::Destroy()
{
	if (this->m__pEffect) {
#ifdef _DEBUG
		ULONG	refCount = this->m__pEffect->Release();
		assert(0 == refCount);
#else
		this->m__pEffect->Release();
#endif
		this->m__pEffect = NULL;
	}
	if (this->m__pVertexBuffer) {
#ifdef _DEBUG
		ULONG	refCount = this->m__pVertexBuffer->Release();
		assert(0 == refCount);
#else
		this->m__pVertexBuffer->Release();
#endif
		this->m__pVertexBuffer = NULL;
	}
	if (this->m__pDeclaration) {
#ifdef _DEBUG
		ULONG	refCount = this->m__pDeclaration->Release();
		assert(0 == refCount);
#else
		this->m__pDeclaration->Release();
#endif
		this->m__pDeclaration = NULL;
	}
}


void
kShowNormal::SetMesh(
	const LPD3DXMESH	pMesh
)
{
	HRESULT	hResult;

	// Position 과 Normal 정보가 있어야한다!
	int	iPos= -1;
	int	iNorm= -1;
	D3DVERTEXELEMENT9	declIn[MAX_FVF_DECL_SIZE];
	hResult = pMesh->GetDeclaration(declIn);
	assert(D3D_OK == hResult);
	for (int i = 0; declIn[i].Stream != 255 && i < MAX_FVF_DECL_SIZE; ++i) {
		if (declIn[i].Usage == D3DDECLUSAGE_POSITION && declIn[i].UsageIndex == 0)
			iPos = i;
		if (declIn[i].Usage == D3DDECLUSAGE_NORMAL && declIn[i].UsageIndex == 0)
			iNorm = i;
	}
	assert(iPos != -1 && iNorm != -1);

	DWORD	dwNumVertices = pMesh->GetNumVertices();
	this->m__NumLines = dwNumVertices;
	hResult = this->m__pDevice->CreateVertexBuffer(
		kShowNormal::Stride * 2 * this->m__NumLines,
		D3DUSAGE_WRITEONLY,
		0,
		D3DPOOL_MANAGED,
		&this->m__pVertexBuffer,
		NULL
	);
	assert(D3D_OK == hResult);

	BYTE*	pSrc= NULL;
	hResult = pMesh->LockVertexBuffer(D3DLOCK_READONLY, (LPVOID*) &pSrc);
	assert(D3D_OK == hResult);
	Vertex*	pDst = NULL;
	hResult = this->m__pVertexBuffer->Lock(0, 0, (void**) &pDst, 0);
	assert(D3D_OK == hResult);
	D3DXVECTOR3	zero(0.f, 0.f, 0.f);
	int	MeshStride = pMesh->GetNumBytesPerVertex();
	for (DWORD iVert = 0; iVert < dwNumVertices; ++iVert) {
		Vertex&	v0 = pDst[2 * iVert + 0];
		Vertex&	v1 = pDst[2 * iVert + 1];

		v0.Position = *((D3DXVECTOR3*) (pSrc + (MeshStride * iVert) + declIn[iPos].Offset));
		v0.Normal = zero;

		v1.Position = v0.Position;
		v1.Normal = *((D3DXVECTOR3*) (pSrc + (MeshStride * iVert) + declIn[iNorm].Offset));
	}
	hResult = this->m__pVertexBuffer->Unlock();
	assert(D3D_OK == hResult);
	hResult = pMesh->UnlockVertexBuffer();
	assert(D3D_OK == hResult);
#if 0
    HRESULT hr = S_OK;

    NumLines = 0;
    
    UnskinnedVB.resize(0);
    SAFE_RELEASE(SkinnedVB);
    SAFE_RELEASE(Skin);

    if(pNewMesh == NULL)
        return;

    IDirect3DDevice9* device = DXUTGetD3D9Device();


    {//EFFECT
     V( device->CreateVertexDeclaration( Elements, &Declaration ) );
     
     ID3DXBuffer* pErrors = NULL;
     V( SASCreateEffectFromResource(
             device, 
             NULL, MAKEINTRESOURCE(IDR_SHOWVECTORS9FX), MAKEINTRESOURCE(RT_RCDATA),
             NULL, 
             NULL, 
             0,
             NULL, 
             &Effect, 
             &pErrors));
    if(pErrors)
         DXVGetApp().OutputA( (const char*)pErrors->GetBufferPointer() );
    SAFE_RELEASE(pErrors);

    }//EFFECT




    D3DVERTEXELEMENT9 declIn[ MAX_FVF_DECL_SIZE ];
    V( pNewMesh->GetDeclaration(declIn) );


    int iPos= -1;
    int iNorm= -1;
    int iTan= -1;
    int iBi= -1;
    for( int i = 0 ; 
        declIn[i].Stream != 255 && i < MAX_FVF_DECL_SIZE;
        i++)
    {
        if(declIn[i].Usage == D3DDECLUSAGE_POSITION && declIn[i].UsageIndex == 0)
            iPos = i;
        if(declIn[i].Usage == D3DDECLUSAGE_NORMAL && declIn[i].UsageIndex == 0)
            iNorm = i;
        if(declIn[i].Usage == D3DDECLUSAGE_TANGENT && declIn[i].UsageIndex == 0)
            iTan = i;
        if(declIn[i].Usage == D3DDECLUSAGE_BINORMAL && declIn[i].UsageIndex == 0)
            iBi = i;
    }

    if(iPos == -1 || iNorm == -1 || iTan == -1 || iBi == -1)
        return;

    if( (( declIn[iPos].Type & (D3DDECLTYPE_FLOAT3|D3DDECLTYPE_FLOAT4)) == 0 )|| 
        (( declIn[iNorm].Type & (D3DDECLTYPE_FLOAT3|D3DDECLTYPE_FLOAT4)) == 0 ) ||
        (( declIn[iTan].Type & (D3DDECLTYPE_FLOAT3|D3DDECLTYPE_FLOAT4)) == 0 ) ||
        (( declIn[iBi].Type & (D3DDECLTYPE_FLOAT3|D3DDECLTYPE_FLOAT4)) == 0 ) )
        return;

    NumLines = pNewMesh->GetNumVertices();

    int NormalsSizeInBytes = Stride*NumLines*2;
    int MeshStride = pNewMesh->GetNumBytesPerVertex();



    V( device->CreateVertexBuffer( NormalsSizeInBytes , 0, 0, D3DPOOL_MANAGED, &SkinnedVB, NULL) );

    BYTE* pSrc= NULL;
    V(  pNewMesh->LockVertexBuffer( D3DLOCK_READONLY, (LPVOID*)&pSrc ) );

    D3DXVECTOR3 zero(0,0,0);
    UnskinnedVB.resize( NumLines*2 );
    for( DWORD iVert = 0; iVert < pNewMesh->GetNumVertices(); iVert++)
    {
        Vertex& v0 = UnskinnedVB[(iVert*2)];
        Vertex& v1 = UnskinnedVB[(iVert*2)+1];

        v0.Position = *(D3DXVECTOR3*) (pSrc+(MeshStride*iVert)+declIn[iPos].Offset);
        v0.Normal = zero;
        v0.Tangent = zero;
        v0.Binormal = zero;

        v1 = v0;
        v1.Normal = *(D3DXVECTOR3*) (pSrc+(MeshStride*iVert)+declIn[iNorm].Offset);
        v1.Tangent = *(D3DXVECTOR3*) (pSrc+(MeshStride*iVert)+declIn[iTan].Offset);
        v1.Binormal = *(D3DXVECTOR3*) (pSrc+(MeshStride*iVert)+declIn[iBi].Offset);
    }
    V( pNewMesh->UnlockVertexBuffer() );

    //Fill in with initial values so unskinned meshes do not have to do this every render.
    pSrc=(BYTE*)(void*)&UnskinnedVB.front();
    BYTE* pDst=NULL;
    V( SkinnedVB->Lock(0, 0, (void**)&pDst, 0 ) );
    {
        memcpy( pDst, pSrc, Stride*NumLines*2 );
    }
    V( SkinnedVB->Unlock() );


    if(pNewSkin)
    {
        V( D3DXCreateSkinInfo( NumLines*2, Elements, pNewSkin->GetNumBones(), &Skin) );
        for( UINT iBone = 0; iBone < pNewSkin->GetNumBones(); iBone++)
        {
            DWORD numInfs = pNewSkin->GetNumBoneInfluences(iBone);

            std::vector<DWORD> srcVertices( numInfs );
            std::vector<FLOAT> srcWeights( numInfs );
            std::vector<DWORD> dstVertices( numInfs * 2);
            std::vector<FLOAT> dstWeights( numInfs * 2);

            V( pNewSkin->GetBoneInfluence( iBone, &(*srcVertices.begin()) , &(*srcWeights.begin()) ) );

        
            for( UINT iInf = 0; iInf < numInfs; iInf++ )
            {
                DWORD vert = srcVertices[iInf];
                FLOAT weight = srcWeights[iInf];
                
                dstVertices[iInf*2+0] = vert*2+0;
                dstVertices[iInf*2+1] = vert*2+1;
                dstWeights[iInf*2+0] = weight;
                dstWeights[iInf*2+1] = weight;
            }

            V( Skin->SetBoneOffsetMatrix( iBone, pNewSkin->GetBoneOffsetMatrix(iBone)) );
            V( Skin->SetBoneInfluence(iBone, numInfs*2 , &(*dstVertices.begin()), &(*dstWeights.begin()) ) );
        }
    }
#endif
}

void
kShowNormal::ShowNormals(
	FLOAT		fScale,
	const D3DXVECTOR4&	cColor,
	const D3DXMATRIX&	worldMatrix,
	const D3DXMATRIX&	viewMatrix,
	const D3DXMATRIX&	projectionMatrix
) const
{
	HRESULT	hResult;

	hResult = this->m__pEffect->SetMatrix("World", &worldMatrix);
	assert(D3D_OK == hResult);
	hResult = this->m__pEffect->SetMatrix("View", &viewMatrix);
	assert(D3D_OK == hResult);
	hResult = this->m__pEffect->SetMatrix("Projection", &projectionMatrix);
	assert(D3D_OK == hResult);

	hResult = this->m__pEffect->SetFloat("Scale", fScale);
	assert(D3D_OK == hResult);
	hResult = this->m__pEffect->SetVector("Color", &cColor);
	assert(D3D_OK == hResult);

	hResult = this->m__pEffect->SetTechnique(this->m__pEffect->GetTechnique(0));
	assert(D3D_OK == hResult);

	UINT	numPasses;
	hResult = this->m__pEffect->Begin(&numPasses, 0);
	assert(D3D_OK == hResult);
	for (UINT iPass = 0; iPass < numPasses; ++iPass) {
		hResult = this->m__pEffect->BeginPass(iPass);
		assert(D3D_OK == hResult);

		hResult = this->m__pDevice->SetVertexDeclaration(this->m__pDeclaration);
		assert(D3D_OK == hResult);
		hResult = this->m__pDevice->SetStreamSource(0, this->m__pVertexBuffer, 0 , kShowNormal::Stride);
		assert(D3D_OK == hResult);
		hResult = this->m__pDevice->DrawPrimitive(D3DPT_LINELIST, 0, this->m__NumLines);
		assert(D3D_OK == hResult);

		hResult = this->m__pEffect->EndPass();
		assert(D3D_OK == hResult);
	}
	hResult = this->m__pEffect->End();
	assert(D3D_OK == hResult);
#if 0
    HRESULT hr = S_OK;

    if(!SkinnedVB)
        return;

    IDirect3DDevice9* device = DXUTGetD3D9Device();

    V( Effect->SetMatrix( "World", &WorldMatrix) );
    V( Effect->SetMatrix( "View", DXVGetCamera().GetViewMatrix() ));
    V( Effect->SetMatrix( "Projection", DXVGetCamera().GetProjMatrix() ));

    if(usage == D3DDECLUSAGE_NORMAL)
    {
        V( Effect->SetVector( "Color", &D3DXVECTOR4(1,0,0,1)) );
    }
    else if(usage == D3DDECLUSAGE_TANGENT)
    {
        V( Effect->SetVector( "Color", &D3DXVECTOR4(0,0,1,1)) );
    }
    else if(usage == D3DDECLUSAGE_BINORMAL)
    {
        V( Effect->SetVector( "Color", &D3DXVECTOR4(0,1,0,1)) );
    }



    V( Effect->SetInt( "RenderUsage", usage) );
    V( Effect->SetFloat( "Scale", DXVGetRender9().GetModelRadius()*0.05f) );



    V( Effect->SetTechnique( Effect->GetTechnique(0)) );

    UINT numPasses;
    V( Effect->Begin( &numPasses, 0 ));

    for( UINT iPass = 0; iPass < numPasses; iPass++ )
    {
        V( Effect->BeginPass( iPass ) );

        V( device->SetVertexDeclaration( Declaration ) );
        V( device->SetStreamSource(0, SkinnedVB, 0 , Stride ) );

        V( device->DrawPrimitive( D3DPT_LINELIST, 0, NumLines) );

        V( Effect->EndPass() );
    }
    V( Effect->End() );
#endif
}
