#pragma once

#include <d3dx9.h>

class kMesh;
class kShowNormal {
public:
#if 3	// porting_DxViewer
	kShowNormal();
private:
	void	__init();
public:
	int	setAll(const LPD3DXMESH meshPtr, const LPDIRECT3DDEVICE9 devicePtr, LPCTSTR effectSourceFilePtr);
#else
	kShowNormal (
		const LPDIRECT3DDEVICE9	pDevice,
		const LPCTSTR	pSrcFile,
		bool*	pbOk = NULL
	);
#endif
	virtual ~kShowNormal();
	void	Destroy();
private:
	LPDIRECT3DDEVICE9	m__pDevice;
	LPD3DXEFFECT	m__pEffect;

	LPDIRECT3DVERTEXBUFFER9		m__pVertexBuffer;
	LPDIRECT3DVERTEXDECLARATION9	m__pDeclaration;
	UINT	m__NumLines;
public:
	static UINT	Stride;
	static D3DVERTEXELEMENT9	Element[3];

public:
	void	SetMesh(LPD3DXMESH pMesh);
	void	ShowNormals(FLOAT fScale, const D3DXVECTOR4& cColor, const D3DXMATRIX& worldMatrix, const D3DXMATRIX& viewMatrix, const D3DXMATRIX& projectionMatrix) const;

	LPD3DXEFFECT	GetEffectPointer(void)	{return this->m__pEffect;}
	LPDIRECT3DVERTEXBUFFER9	GetVertexBufferPointer(void)	{return this->m__pVertexBuffer;}
	LPDIRECT3DVERTEXDECLARATION9	GetDeclarationPointer(void)	{return this->m__pDeclaration;}

public:
	struct Vertex {
		D3DXVECTOR3	Position;
		D3DXVECTOR3	Normal;
	};
};
