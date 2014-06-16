#pragma once
#ifndef __MESH_LOADER__
#define __MESH_LOADER__


#include <vector>
#include <map>
#include <DXUT/Core/DXUT.h>
#include <DXUT/Core/DXUTmisc.h>
#include <mesh/kPolygon.h>
#ifdef __READ_FILES__
#if 1	// kFileCache
#include <mesh/kFileCache.h>
#endif	// kFileCache
#endif	// __READ_FILES__

class kMeshLoader
#ifdef __READ_FILES__
#if 1	// kFileCache
	: virtual public kFileCache
#endif	// kFileCache
#endif	// __READ_FILES__
{
#if 1	// Voxel
public:
	struct VOXEL_VERTEX {
		D3DXVECTOR3	p;
		D3DXVECTOR3	n;
		D3DXVECTOR3	c;
	};
public:
	struct VOXEL
	{
		float	size;
		float	X, Y, Z;
		UINT	R, G, B;
	};
private:
	LPDIRECT3DVERTEXDECLARATION9	__voxelVertexDeclarationPtr;
	LPDIRECT3DVERTEXBUFFER9		__voxelVertexBufferPtr;
	LPDIRECT3DINDEXBUFFER9		__voxelIndexBufferPtr;
	UINT	__num_voxels;
	void	__setVoxelBuffer(IDirect3DDevice9* devicePtr, const CGrowableArray<VOXEL>& voxels);
public:
	LPDIRECT3DVERTEXDECLARATION9	voxelVertexDeclarationPtr() const	{return __voxelVertexDeclarationPtr;}
	LPDIRECT3DVERTEXBUFFER9	voxelVertexBufferPtr() const	{return __voxelVertexBufferPtr;}
	LPDIRECT3DINDEXBUFFER9	voxelIndexBufferPtr() const	{return __voxelIndexBufferPtr;}
	UINT	GetNumVoxels() const			{return __num_voxels;}
#endif	// Voxel

private:
	LPD3DXMESH		__meshPtr;	// 이것을 만드는 것이 목적이다.
#if 1	// DrawSubset
	LPD3DXMESH		__meshSubsetPtr;	// Subset 별로 렌더링하기 위해...
							// 특히 텍스쳐를 사용하기 위하여...
#endif
public:
	LPD3DXMESH		meshPtr()		{return __meshPtr;}
	const LPD3DXMESH	meshPtr() const		{return __meshPtr;}
#if 1	// DrawSubset
	LPD3DXMESH		meshSubsetPtr()		{return __meshSubsetPtr;}
	const LPD3DXMESH	meshSubsetPtr() const	{return __meshSubsetPtr;}
#endif
public:
	kMeshLoader();
	~kMeshLoader();


#if 0	// group
	// cplusplus.com; map
private:
	struct classcomp {
		bool operator()(const std::string& lhs, const std::string& rhs) const
		{
			return lhs.compare(rhs) < 0 ? true : false;
		}
	};
public:
	struct GROUP_INFO {
		kID	__gid;
		std::vector<kID>	__fidList;

		// Contructor
		GROUP_INFO()	{}	// default contructor
		GROUP_INFO(kID gid, kID fid) : __gid(gid)		{__fidList.push_back(fid);}

		// Copy Contructor
		GROUP_INFO&	operator=(const GROUP_INFO& other)
		{
			if (this != &other) {
				this->__gid = other.__gid;
				this->__fidList = other.__fidList;
			}

			return *this;
		}
	};
	typedef std::map<std::string, struct GROUP_INFO, classcomp>	GROUP_INFO_HASH;
private:
	struct classcomp2 {
		bool operator()(const kID& lhs, const kID& rhs) const	{return lhs.id() < rhs.id();}
	};
public:
	typedef std::map<kID, std::string, classcomp2>	GID_NAME_HASH;
	typedef std::map<kID, kID, classcomp2>		FID2GID_HASH;
private:
	GROUP_INFO_HASH		__groupMap;	// name -> GROUP_INFO
	GID_NAME_HASH		__gidMap;
	FID2GID_HASH		__fid2gidMap;
public:
	const GROUP_INFO_HASH&	groupMap() const	{return __groupMap;}
	const GID_NAME_HASH&	gidMap() const		{return __gidMap;}
	const FID2GID_HASH&	fid2gidMap() const	{return __fid2gidMap;}
#endif

public:
#if 0
	struct VERTEX
	{
		D3DXVECTOR3	position;
		D3DXVECTOR3	normal;
		D3DXVECTOR2	texcoord;
	};
#endif
public:
	// Material properties per mesh subset
	struct Material
	{
		WCHAR	strName[MAX_PATH];

		D3DXVECTOR3	vAmbient;
		D3DXVECTOR3	vDiffuse;
		D3DXVECTOR3	vSpecular;

		int	nShininess;
		float	fAlpha;

		bool	bSpecular;

		WCHAR		strTexture[MAX_PATH];
		LPDIRECT3DTEXTURE9	texturePtr;
		D3DXHANDLE	hTechnique;
	};
private:
	CGrowableArray<Material*>	__Materials;	// Holds material properties per subset
public:
	// 텍스쳐를 사용하기 위해, kMesh::ShowSubsetFace 에서 사용
	const CGrowableArray<Material*>&	GetMaterials() const	{return __Materials;}


private:
	static D3DVERTEXELEMENT9	VERTEX_DECL[3 + 1];	// POSITION, NORMAL, TEXCOORD
public:
	struct FACE_INDEX
	{
		DWORD	v_index;
		DWORD	vt_index;
#if 0	// kMesh::SetMesh 에서 노말은 따로 계산한다.
		DWORD	vn_index;
#endif
	};
private:
	struct	F
	{
		FACE_INDEX	faceIndex[3];
	};
public:
	// 여기서 만든다.
	HRESULT	Create(
		IDirect3DDevice9*	devicePtr,
		const char*	strFileName,
		const WCHAR*	texturePath
#ifdef __READ_FILES__
		,
		const LPCWSTR_ARRAY*	textureArrayPtr
#ifdef MULTI_STANDARD
		,
		int	index_to_standard
#endif	// MULTI_STANDARD
#endif	// __READ_FILES__
	);
	void	Destroy();
private:
	HRESULT
	kMeshLoader::__LoadGeometryFromOBJ(
		bool	v_vt_only,
		const char*	strFileName,
		const WCHAR*	texturePath,

		// [OUT]
		// .OBJ 에서 읽어들인 Raw Data;
		// Face 의 순서가 바뀌지 않는다고 가정했을 때,
		// 1) v, vt 의 순서가 바뀌었다 할지라도
		//	__VertexBufferIndexByF, __Indices 정보가 있다면
		//	다음 세 값을 .OBJ 파일로부터 읽은 후, __Vertices 를 구성할 수 있다.
		// 2) v, vt 의 순서도 바뀌지 않는다면
		//	이 함수를 호출할 필요도 없이 따로 저장된 __Vertices, __Indices, __Attributes 를 읽으면 된다.
		CGrowableArray<D3DXVECTOR3>&	__vArray,
		CGrowableArray<D3DXVECTOR2>&	__vtArray,
		CGrowableArray<F>&		__fArray,

		// [OUT]
		CGrowableArray<CPointRepr::VERTEX>&	__Vertices,
		CGrowableArray<FACE_INDEX>&		__VertexBufferIndexByF,	// 1) v, vt 의 순서가 바뀔 경우...
										// 2) v, vt 의 순서가 바뀌지 않는다면 불필요
		CGrowableArray<DWORD>&	__Indices,
		CGrowableArray<DWORD>&	__Attributes
#if 1	// Voxel
		,
		CGrowableArray<VOXEL>&	__voxels
#endif	// Voxel
	);
	HRESULT	__LoadMaterialsFromMTL(LPCTSTR strFileName);
	void	__InitMaterial(Material* pMaterial);

	// Used for a hashtable vertex cache when creating the mesh from a .obj file
	struct CacheEntry
	{
		UINT	index;
		CacheEntry*	pNext;
	};
	// 주어진 Vertex 정보 VERTEX 가 __Vertices 에 있는 어떤 것들과도 중복되지 않는 지 검사한 후,
	// 새로운 VERTEX 라면 __Vertices 에 추가한다;
	// VERTEX 가 들어있는 __Vertices 에 대한 인덱스를 리턴한다.
	//
	// 중복된 지를 검사할 때 (hashIndex, __VertexCache) 가 사용된다; 일종의 Linked List
	DWORD	kMeshLoader::__AddVertex(
		const CPointRepr::VERTEX&	vertex,

		UINT	hashIndex,
		CGrowableArray<CacheEntry*>&	__VertexCache,	// [OUT]

		CGrowableArray<CPointRepr::VERTEX>&		__Vertices	// [OUT]
	);
};


#endif	// !__MESH_LOADER__
