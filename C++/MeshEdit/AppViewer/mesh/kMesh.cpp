#include <iostream>
using std::cout;
using std::cerr;
using std::endl;
#include <mesh/kState.h>
#include <mesh/kMesh.h>
#include <assert.h>
#include <DXUT/Core/DXUT.h>
#include <fstream>
using namespace std;
#ifdef __READ_FILES__
#if 1	// kFileCache
#else	// kFileCache/!kFileCache
#ifdef MULTI_STANDARD
#include <mesh/__adjacencyPtr_M__>		// ADJACENCY_PTR__FILENAME
#include <mesh/__vertexID2pointReprIDMap_M__>	// VERTEXID2POINTREPRIDMAP__FILENAME
#include <mesh/__pointReprPtr_M__>		// POINTREPR_PTR__FILENAME
#include <mesh/__edgePtr_M__>			// EDGE_PTR__FILENAME
#include <mesh/__facePtr_M__>			// FACE_PTR__FILENAME

#include <mesh/__mesh_READ_FILES_M__.h>
#else	// MULTI_STANDARD/!MULTI_STANDARD
#include <mesh/__adjacencyPtr__>		// ADJACENCY_PTR__FILENAME
#include <mesh/__vertexID2pointReprIDMap__>	// VERTEXID2POINTREPRIDMAP__FILENAME
#include <mesh/__pointReprPtr__>		// POINTREPR_PTR__FILENAME
#include <mesh/__edgePtr__>			// EDGE_PTR__FILENAME
#include <mesh/__facePtr__>			// FICE_PTR__FILENAME
#endif	// !MULTI_STANDARD
#endif	// !kFileCache
#endif	// __READ_FILES__
#if 1	// defined
#include <mesh/__defined__.h>
#endif	// defined
#include <mesh/kBone.h>
#include <math/linearleastsquare.h>


D3DVERTEXELEMENT9	kMesh::__OnePointElement[2] = {
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

#if 1	// SEGMENT_VERTEX
D3DVERTEXELEMENT9	kMesh::__SegmentVertexElement[5] = {	// 뼈: 5, !뼈: 4
	{
		0,
		0 * sizeof(D3DXVECTOR3),
		D3DDECLTYPE_FLOAT3,
		D3DDECLMETHOD_DEFAULT,
		D3DDECLUSAGE_POSITION,
		0
	},
#if 1	// 뼈
	{
		0,
		1 * sizeof(D3DXVECTOR3),
		D3DDECLTYPE_FLOAT3,
		D3DDECLMETHOD_DEFAULT,
		D3DDECLUSAGE_POSITION,
		1
	},
	{
		0,
		2 * sizeof(D3DXVECTOR3),
		D3DDECLTYPE_FLOAT3,
		D3DDECLMETHOD_DEFAULT,
		D3DDECLUSAGE_NORMAL,
		0
	},
	{
		0,
		3 * sizeof(D3DXVECTOR3),
		D3DDECLTYPE_FLOAT2,
		D3DDECLMETHOD_DEFAULT,
		D3DDECLUSAGE_TEXCOORD,
		0
//		D3DDECLUSAGE_NORMAL,
//		1
	},
#else	// 뼈/!뼈
	{
		0,
		1 * sizeof(D3DXVECTOR3),
		D3DDECLTYPE_FLOAT3,
		D3DDECLMETHOD_DEFAULT,
		D3DDECLUSAGE_NORMAL,
		0
	},
	{
		0,
		2 * sizeof(D3DXVECTOR3),
		D3DDECLTYPE_FLOAT2,
		D3DDECLMETHOD_DEFAULT,
		D3DDECLUSAGE_TEXCOORD,
		0
//		D3DDECLUSAGE_NORMAL,
//		1
	},
#endif	// 뼈

	D3DDECL_END()
};
#else
D3DVERTEXELEMENT9	kMesh::__SegmentVertexElement[2] = {
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
#endif

#if 3	// porting_DxViewer
#if 4	// SDKmesh
const DWORD	kMesh::D3DVERTEX::FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;
#endif
kMesh::kMesh()
{
	__init();
}

int	kMesh::setAll(const SetAllArg& arg)
{
	return setAll(
#ifdef STANDARD_MODEL
		arg.index_to_standard,
#endif	// STANDARD_MODEL
		arg.statePtr,
		arg.xSourceFilePtr,
		arg.devicePtr,
		arg.effectSourceFilePtr,
		arg.fScale,
#ifdef __READ_FILES__
		arg.stillRead,
#endif	// __READ_FILES__
		arg.objFilePtr,
		arg.texturePath,

		arg.hWnd
#ifdef __READ_FILES__
		,
		arg.textureArrayPtr
#endif	// __READ_FILES__
	);
}
int	kMesh::setAll(
#ifdef STANDARD_MODEL
	int	index_to_standard,
#endif	// STANDARD_MODEL
	kState*		statePtr,

	LPCTSTR		xSourceFilePtr,
	const LPDIRECT3DDEVICE9		devicePtr,
	LPCTSTR				effectSourceFilePtr,

	float		fScale,
#ifdef __READ_FILES__
	bool		stillRead,
#endif	// __READ_FILES__
	const char*	objFilePtr,

	const WCHAR*		texturePath,

	HWND	hWnd
#ifdef __READ_FILES__
	,
	const LPCWSTR_ARRAY*	textureArrayPtr
#endif	// __READ_FILES__
)
{
#ifdef STANDARD_MODEL
	__index_to_standard = index_to_standard;
#endif	// STANDARD_MODEL
	__hWnd = hWnd;
#if 1	// Timer
	double	fTime0 = __timer.GetTime();	// 이 함수에서 걸린 총 시간을 알기위해
	__timer.GetElapsedTime();		// 각 단계마다 걸린 시간을 알기위해
#endif
	__statePtr = statePtr;
	__devicePtr = devicePtr;
	__area.SetMaterialColor(__statePtr->g__colorMtrl);
	__bodyElement.SetMaterialColor(__statePtr->g__colorMtrl);

	devicePtr->CreateVertexDeclaration(kMesh::__OnePointElement, &__OnePointDeclarationPtr);
#if 4	// SDKmesh
	Create(devicePtr, xSourceFilePtr);
	SetFVF(devicePtr, D3DVERTEX::FVF);
#endif
	if (!__effectInit(effectSourceFilePtr, m_pTextures[0]))
		return -1;

#if 1	// ColorBand
{
	HRESULT	hr = __devicePtr->CreateVertexBuffer(
		3 * 6 * sizeof(D3DXVERTEX),
		D3DUSAGE_WRITEONLY,
		0,
		D3DPOOL_DEFAULT,//D3DPOOL_MANAGED,
		&__colorBandVertexBufferPtr,
		NULL
	);
	assert(D3D_OK == hr);

	hr = D3DXCreateLine(__devicePtr, &__indicatorLinePtr);
	assert(D3D_OK == hr);
}
#endif	// ColorBand
	__tapeMeasureInit();
	__isometricInit();

	LPD3DXMESH	meshPtr = 0;
#if 4	// SDKmesh
	LPD3DXMESH	meshPtr0 = 0;
	if (objFilePtr) {
		// t2: 입안 모델 제거(턱, 혀, 치아), 밖에서 보이지 않는 눈 부분, 눈썹, 눈썹 위 이마 제거
		//	lEye2.f[1408:2175]
		// t2_1: 왼쪽에서 Iris 부분을 제거(lEye2.f[1408:2175]); 마야에서 triangulate 할 때 생긴 에러(?)
		// t3: 왼쪽 눈 제거(lEye2 에 rEye4 에 해당하는 부분까지 포함되어 있음)
		// t4: 오른쪽 눈 제거
		(void) fprintf(stdout, "Calling kMeshLoader::Create...(%f)\n", __timer.GetElapsedTime());
		__meshLoader.Create(
			devicePtr,
#ifdef __READ_FILES__
			stillRead ? objFilePtr : NULL,
#else	// __READ_FILES__/!__READ_FILES__
			objFilePtr,
#endif	// !__READ_FILES__
			texturePath
#ifdef __READ_FILES__
			,
			textureArrayPtr
#ifdef MULTI_STANDARD
			,
			index_to_standard
#endif	// MULTI_STANDARD
#endif	// __READ_FILES__
		);
		(void) fprintf(stdout, "...Done (%f)\n", __timer.GetElapsedTime());
		meshPtr0 = __meshLoader.meshPtr();
	} else {
		meshPtr0 = GetMesh();
	}
	__objRHLoaded = objFilePtr != NULL;
	__fScale = fScale;
	meshPtr = meshPtr0;
#if 0	// tessellation
	LPD3DXMESH	meshPtr2 = 0;
	meshPtr->CloneMeshFVF(
		D3DXMESH_MANAGED | D3DXMESH_32BIT,
		CPointRepr::D3DXVERTEX::FVF,
		devicePtr,
		&meshPtr2
	);
	// 이전 메쉬를 없애고,
#if 1	// No clone
#else
	if (meshPtr) {
		meshPtr->Release();
		meshPtr = 0;
	}
#endif
	// 새로 만든다.
	HRESULT	hr = D3DXTessellateNPatches(
		meshPtr2,
		NULL,
		2,
		FALSE,
		&meshPtr,
		NULL
	);
	if (meshPtr2) {
		meshPtr2->Release();
		meshPtr2 = 0;
	}
#endif
#endif

#ifndef __READ_FILES__
	(void) fprintf(stdout, "Calling kMesh::SetPolygon... (%f)\n", __timer.GetElapsedTime());
	SetPolygon(meshPtr);
	(void) fprintf(stdout, "...Done (%f)\n", __timer.GetElapsedTime());
#endif
#ifdef __READ_FILES__
	int	numElements;	// 배열 자체의 크기로 의미있는 성분의 갯수와는 다르다;
				// for loop counter 에서 실제 갯수를 따진다:
				// 예를 들어 __Vertices__ 의 경우 numElements / 5
#ifdef MULTI_STANDARD
#if 1	// kFileCache
	assert(index_to_standard >= 0 && index_to_standard < (int) kFileCache::__numEntries_in_MESH_READ_FILES);
	const MESH_READ_FILES&	mesh_READ_FILES = kFileCache::__pointer_to_MESH_READ_FILES[index_to_standard];
#else	// kFileCache/!kFileCache
	assert(index_to_standard >= 0 && index_to_standard < sizeof(__mesh_READ_FILES__) / sizeof(MESH_READ_FILES));
	const MESH_READ_FILES&	mesh_READ_FILES = __mesh_READ_FILES__[index_to_standard];
#endif	// !kFileCache
	// 다른 함수에서 참조하는 포인터를 설정한다; kPolygon.h
	pointer_to__a_vVertexID = mesh_READ_FILES.__a_vVertexID.ptr;
	pointer_to__a_vVertexID_a_offset = mesh_READ_FILES.__a_vVertexID_a_offset.ptr;
	pointer_to__a_vEdgeID = mesh_READ_FILES.__a_vEdgeID.ptr;
	pointer_to__a_vEdgeID_a_offset = mesh_READ_FILES.__a_vEdgeID_a_offset.ptr;
	pointer_to__a_vFaceIDLocalIndex = mesh_READ_FILES.__a_vFaceIDLocalIndex.ptr;
	pointer_to__a_vFaceIDLocalIndex_a_offset = mesh_READ_FILES.__a_vFaceIDLocalIndex_a_offset.ptr;
	pointer_to__a_faceID = mesh_READ_FILES.__a_faceID.ptr;
	pointer_to__a_faceID_a_offset = mesh_READ_FILES.__a_faceID_a_offset.ptr;
#else	// MULTI_STANDARD/!MULTI_STANDARD
	pointer_to__a_vVertexID = __a_vVertexID;
	pointer_to__a_vVertexID_a_offset = __a_vVertexID_a_offset;
	pointer_to__a_vEdgeID = __a_vEdgeID;
	pointer_to__a_vEdgeID_a_offset = __a_vEdgeID_a_offset;
	pointer_to__a_vFaceIDLocalIndex = __a_vFaceIDLocalIndex;
	pointer_to__a_vFaceIDLocalIndex_a_offset = __a_vFaceIDLocalIndex_a_offset;
	pointer_to__a_faceID = __a_faceID;
	pointer_to__a_faceID_a_offset = __a_faceID_a_offset;
#endif	// MULTI_STANDARD
#ifdef __USE_A_OFFSET__
	(void) fprintf(stdout, "Reading... (%f)\n", __timer.GetElapsedTime());
#ifdef MULTI_STANDARD
	__numFaces = mesh_READ_FILES.__numFaces__;
	__numEdges = mesh_READ_FILES.__numEdges__;
	__numPointReprs = mesh_READ_FILES.__numPointReprs__;
#else	// MULTI_STANDARD/!MULTI_STANDARD
	__numFaces = __numFaces__;
	__numEdges = __numEdges__;
	__numPointReprs = __numPointReprs__;
#endif	// !MULTI_STANDARD

	(void) fprintf(stdout, "\tSetting __adjacencyPtr...");
	////////////////////////////////////////////////////////////////////////
{
	if (__adjacencyPtr) {
		delete[] __adjacencyPtr;
		__adjacencyPtr = 0;
	}
	__adjacencyPtr = new DWORD[3 * __numFaces];
#ifdef MULTI_STANDARD
	const DWORD*	__adjacencyPtr__ = mesh_READ_FILES.__adjacencyPtr__.ptr;
	numElements = mesh_READ_FILES.__adjacencyPtr__.n;
	assert(numElements / 3 == (int) __numFaces);
#else	// MULTI_STANDARD/!MULTI_STANDARD
	assert(sizeof(__adjacencyPtr__) / (3 * sizeof(DWORD)) == __numFaces);
#endif	// !MULTI_STANDARD
	for (int n = 0; n < (int) (3 * __numFaces); ++n)
		__adjacencyPtr[n] = __adjacencyPtr__[n];
}
	////////////////////////////////////////////////////////////////////////
	(void) fprintf(stdout, "...Done\n");

	(void) fprintf(stdout, "\tSetting __vertexID2pointReprIDMap...");
	////////////////////////////////////////////////////////////////////////
{
	if (__vertexID2pointReprIDMap) {
		delete[] __vertexID2pointReprIDMap;
		__vertexID2pointReprIDMap = 0;
	}
	__vertexID2pointReprIDMap = new DWORD[3 * __numFaces];
#ifdef MULTI_STANDARD
	const int*	__vertexID2pointReprIDMap__ = mesh_READ_FILES.__vertexID2pointReprIDMap__.ptr;
	numElements = mesh_READ_FILES.__vertexID2pointReprIDMap__.n;
	assert(numElements / 3 == (int) __numFaces);
#else	// MULTI_STANDARD/!MULTI_STANDARD
	assert(sizeof(__vertexID2pointReprIDMap__) / (3 * sizeof(int)) == __numFaces);
#endif	// !MULTI_STANDARD
	for (int n = 0; n < (int) (3 * __numFaces); ++n)
		__vertexID2pointReprIDMap[n] = __vertexID2pointReprIDMap__[n];
}
	////////////////////////////////////////////////////////////////////////
	(void) fprintf(stdout, "...Done\n");

	(void) fprintf(stdout, "\tSetting __pointReprPtr...");
	////////////////////////////////////////////////////////////////////////
{
	if (__pointReprPtr) {
		delete[] __pointReprPtr;
		__pointReprPtr = 0;
	}
	__pointReprPtr = new CPointRepr[__numPointReprs];
	for (DWORD prID = 0; prID < __numPointReprs; ++prID)
		__pointReprPtr[prID].id = prID;
}
	////////////////////////////////////////////////////////////////////////
	(void) fprintf(stdout, "...Done\n");

	(void) fprintf(stdout, "\tSetting __edgePtr...");
	////////////////////////////////////////////////////////////////////////
{
	if (__edgePtr) {
		delete[] __edgePtr;
		__edgePtr = 0;
	}
	__edgePtr = new CEdge[__numEdges];
#ifdef MULTI_STANDARD
	const int*	__edgePtr__ = mesh_READ_FILES.__edgePtr__.ptr;
	numElements = mesh_READ_FILES.__edgePtr__.n;
	assert(numElements / 2 == (int) __numEdges);
#else	// MULTI_STANDARD/!MULTI_STANDARD
	assert(__numEdges == sizeof(__edgePtr__) / (2 * sizeof(int)));
#endif	// !MULTI_STANDARD
	for (DWORD edgeID = 0; edgeID < __numEdges; ++edgeID) {
		CEdge&	edge = __edgePtr[edgeID];
		edge.id = edgeID;
//		(void) fprintf(filePtr, "%d,%d,", edge.prStartID, edge.prEndID);
		edge.prStartID = __edgePtr__[2 * edgeID + 0];
		edge.prEndID = __edgePtr__[2 * edgeID + 1];
	}
}
	////////////////////////////////////////////////////////////////////////
	(void) fprintf(stdout, "...Done\n");

	(void) fprintf(stdout, "\tSetting __facePtr...");
	////////////////////////////////////////////////////////////////////////
{
	if (__facePtr) {
		delete[] __facePtr;
		__facePtr = 0;
	}
	__facePtr = new CFace[__numFaces];
/*
		(void) fprintf(filePtr,
			"%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,",
			face.prID[0], face.prID[1], face.prID[2],
			face.edgeID[0].id, face.edgeID[0].bDir, face.edgeID[1].id, face.edgeID[1].bDir, face.edgeID[2].id, face.edgeID[2].bDir,
			face.ajID[0], face.ajID[1], face.ajID[2]
		);
 */
#ifdef MULTI_STANDARD
	const int*	__facePtr__ = mesh_READ_FILES.__facePtr__.ptr;
	numElements = mesh_READ_FILES.__facePtr__.n;
	assert(numElements / 12 == (int) __numFaces);
#else	// MULTI_STANDARD/!MULTI_STANDARD
	assert((sizeof(__facePtr__) / sizeof(int)) % 12 == 0);
	numElements = sizeof(__facePtr__) / sizeof(int);
	assert(numElements / 12 == __numFaces);
#endif	// !MULTI_STANDARD
	for (int index_facePtr = 0; index_facePtr < numElements / 12; ++index_facePtr) {
		CFace&	face = __facePtr[index_facePtr];
		face.id = index_facePtr;
		face.prID[0]	= __facePtr__[12 * index_facePtr + 0];
		face.prID[1]	= __facePtr__[12 * index_facePtr + 1];
		face.prID[2]	= __facePtr__[12 * index_facePtr + 2];
		face.edgeID[0].id	= __facePtr__[12 * index_facePtr + 3];
		face.edgeID[0].bDir	= __facePtr__[12 * index_facePtr + 4] ? true : false;
		face.edgeID[1].id	= __facePtr__[12 * index_facePtr + 5];
		face.edgeID[1].bDir	= __facePtr__[12 * index_facePtr + 6] ? true : false;
		face.edgeID[2].id	= __facePtr__[12 * index_facePtr + 7];
		face.edgeID[2].bDir	= __facePtr__[12 * index_facePtr + 8] ? true : false;
		face.ajID[0]	= __facePtr__[12 * index_facePtr + 9];
		face.ajID[1]	= __facePtr__[12 * index_facePtr + 10];
		face.ajID[2]	= __facePtr__[12 * index_facePtr + 11];
	}
}
	////////////////////////////////////////////////////////////////////////
	(void) fprintf(stdout, "...Done\n");

	(void) fprintf(stdout, "...Done (%f)\n", __timer.GetElapsedTime());	// Reading...
#endif	// __USE_A_OFFSET__
#endif	// __READ_FILES__
#if 1	// SetPolygon 을 호출한 후에 CPointRepr::vVertexID 와 meshPtr 의 Vertex Buffer 를 이용하여 설정
	// __pointReprPtr 설정
{
	LPDIRECT3DVERTEXBUFFER9	vertexBufferPtr;
	meshPtr->GetVertexBuffer(&vertexBufferPtr);	// D3D_OK
	CPointRepr::VERTEX*	vertexArray;
	vertexBufferPtr->Lock(0, 0, (void**) &vertexArray, 0);	// D3D_OK
	for (int i = 0; i < (int) __numPointReprs; ++i) {
#if 0	// SetPolygon 을 호출한 후에 CPointRepr::vVertexID 와 meshPtr 의 Vertex Buffer 를 이용하여 설정
#if 1	// tiny.x; Up 방향 = z 축, RHS (정면 = y축)
		CPointRepr::VERTEX	v = pVertices[vertexID];

		// Up 방향 = z축; x축에 대해서 90도 회전
#if 1	// kMeshLoader::meshFromOBJ (woman.obj)
		v.position *= __fScale;
#else	// tiny.x
		float	f = v.p.y;
		v.p.y = v.p.z;
		v.p.z = -f;
#endif

		pointRepr.v = v;
#else
		pointRepr.v = pVertices[vertexID];	// CPointRepr::v
							// Bulk Copy
#endif
#endif
		CPointRepr&	pointRepr = __pointReprPtr[i];
		CPointRepr::VERTEX&	v = pointRepr.v;
#ifdef __USE_A_OFFSET__
		v = vertexArray[pointer_to__a_vVertexID[pointer_to__a_vVertexID_a_offset[i]]];
#else
		std::set<DWORD>::const_iterator it = pointRepr.vVertexID.begin();
		v = vertexArray[*it];
#endif
#if 1	// 뼈
		pointRepr.v.position0 = pointRepr.v.position;
#endif	// 뼈
//		v.position *= __fScale;
	}
#if 0	// kDisplacementMap; 같은 Vertex 에 해당하는 서로 다른 (u, v)의 값이 같은 지 확인: 빗질할 때 맞추기가 어렵다...
	const WCHAR	bmpFile[] = L"D:/home/icentrik/My Documents/Visual Studio Projects/Pick/Media/Scanner/여자모델_Vic090609/tex/victoria-depth-left-thigh.bmp";
	kDisplacementMap	dismap(bmpFile);
for (DWORD faceID = 0; faceID < __numFaces; ++faceID) {
	const CFace&	face = __facePtr[faceID];
	for (int i = 0; i < 3; ++i) {
		DWORD	prID = face.prID[i];
#ifdef __USE_A_OFFSET__
		unsigned	numVertices = pointer_to__a_vVertexID_a_offset[prID + 1] - pointer_to__a_vVertexID_a_offset[prID];
#else
		unsigned	numVertices = vVertexID.size();
#endif
		if (numVertices < 2)
			continue;
		cout << "faceID = " << faceID << ": prID[" << i << "] = " << prID << endl;
		cout << "**** # of vertices = " << numVertices << " ****" << endl;
		if (numVertices > 2) {
			cout << "numVertices > 2" << endl;
			continue;
		}
		int	index = 0;
		int	r[2];
#ifdef __USE_A_OFFSET__
		for (int k = pointer_to__a_vVertexID_a_offset[prID]; k < pointer_to__a_vVertexID_a_offset[prID + 1]; ++k) {
			DWORD	vertexID = pointer_to__a_vVertexID[k];
#else
		const CPointRepr&	pointRepr = __pointReprPtr[prID];
		const std::set<DWORD>&	vVertexID = pointRepr.vVertexID;
		for (std::set<DWORD>::const_iterator it = vVertexID.begin(); it != vVertexID.end(); ++it) {
			DWORD	vertexID = *it;
#endif
			CPointRepr::VERTEX&	vertex = vertexArray[vertexID];
			cout << "Vertex[" << vertexID << "]: (" << vertex.texcoord.x << ", " << vertex.texcoord.y << ")" << endl;
			float	val;
			Gdiplus::Color	color_at_floored_uv;
			Gdiplus::Status	status = dismap.GetPixelAtUV(vertex.texcoord.x, vertex.texcoord.y, val, kDisplacementMap::CHANNEL_RED, &color_at_floored_uv);
			cout << "val: " << val << ", color: " << color_at_floored_uv.GetValue() << endl;
			r[index++] = color_at_floored_uv.GetRed();
#ifdef __USE_A_OFFSET__
		}
#else
		}
#endif
		cout << "diff = " << (r[0] > r[1] ? r[0] - r[1] : r[1] - r[0]) << endl;
	}
}
#endif	// kDisplacementMap
	vertexBufferPtr->Unlock();
	SAFE_RELEASE(vertexBufferPtr);
}
#endif
#ifdef STANDARD_MODEL
#if 1	// 마지막 주문; 7. 다리 길이 변경
if (index_to_standard == 6 || index_to_standard == 7) {
	float	scale = 1.2f;
	POINT_DEF	rightLegPointDef = {18039, 0.354008f, 0.171513f};
	POINT_DEF	leftLegPointDef = {19597, 0.208489f, 0.373029f};
	kBone::leg_elongate(*this, leftLegPointDef, scale);	// rightLegPointDef is ok
}
#endif	// 마지막 주문; 7. 다리 길이 변경
#if 0	// 뼈의 길이 변경
if (index_to_standard == 1) {
#if 1	// kDisplacementMap
	cout << "**************** kDisplacementMap ****************" << endl;
	cout << "**************************************************" << endl;
//	const WCHAR	bmpFile[] = L"D:/home/icentrik/GDI+/gdiplus-1.0.1/debug/victoria-body.bmp";
	const WCHAR	bmpFile[] = L"D:/home/icentrik/My Documents/Visual Studio Projects/Pick/Media/Scanner/여자모델_Vic090609/tex/victoria-depth-left-thigh.bmp";
	kDisplacementMap	dismap(bmpFile);
do {
	if (dismap.GetLastStatus() != Gdiplus::Ok) {
		cerr << "kDisplacementMap::kDisplacementMap(|" << bmpFile << "|): Failure: " << dismap.GetLastStatus() << endl;
		break;
	}
#if 0	// GDI+ 테스트
	int	width = dismap.Width();
	int	height = dismap.Height();
	cout << "H(" << height << ") x W(" << width << ")" << endl;
	Gdiplus::Color	pixelColor;
//	bitmap->GetPixel(21, 38, &pixelColor);
//	bitmap->GetPixel(43, 79, &pixelColor);
//	bitmap->GetPixel(97, 176, &pixelColor);
	dismap.GetPixel(45, 176, &pixelColor);
	if (dismap.GetLastStatus() != Gdiplus::Ok) {
		cerr << "Bitmap::GetPixel(" << 45 << ", " << 176 << "): Failure: " << dismap.GetLastStatus() << endl;
		break;
	}
	(void) fprintf(stdout,
		"0x%08x: R: 0x%02x, G: 0x%02x, B: 0x%02x, A: 0x%02x\n"
		,
		pixelColor.GetValue(),
		pixelColor.GetRed(), pixelColor.GetGreen(), pixelColor.GetBlue(), pixelColor.GetAlpha()
	);
	float	u = .2f, v = .2f;
	float	val;
	Gdiplus::Color	color_at_floored_uv;
	Gdiplus::Status	status = dismap.GetPixelAtUV(u, v, val, kDisplacementMap::CHANNEL_RED, &color_at_floored_uv);
	if (status != Gdiplus::Ok) {
		cerr << "kDisplacementMap::GetPixel(): Failure" << endl;
	}
	(void) fprintf(stdout, "At (u = %f, v = %f): val = %f\n", u, v, val);
	(void) fprintf(stdout,
		"0x%08x: R: 0x%02x, G: 0x%02x, B: 0x%02x, A: 0x%02x\n"
		,
		pixelColor.GetValue(),
		pixelColor.GetRed(), pixelColor.GetGreen(), pixelColor.GetBlue(), pixelColor.GetAlpha()
	);
#endif	// 테스트
} while (0);
	cout << "***********************************************" << endl;
#endif	// kDisplacementMap
/*
POINT_DEF: (63763, 0.19955, 0.350599)
nDir: [0, 1, 0]

POINT_DEF: (69918, 0.652526, 0.254092)
nDir: [0, 1, 0]
 */
	PLANE_DEF	planeDef0 = {	// 허벅지 위쪽
		{63763, 0.19955f, 0.350599f},
		D3DXVECTOR3(0, 1, 0)
	};
	PLANE_DEF	planeDef1 = {	// 허벅지 아래쪽
		{69918, 0.652526f, 0.254092f},
		D3DXVECTOR3(0, 1, 0)
	};
#if 1	// kBone
#if 0	// 다리를 길어 보이게 하기
	PLANE_DEF	legPlaneDef = {
		{64169, 0.824042f, 0.0886678f},
		D3DXVECTOR3(0, -1, 0)
	};
	kBone	longLeg;
	longLeg.set(*this, legPlaneDef, NULL, 1.2f, false, NULL);
	longLeg.apply(*this);

	PLANE_DEF	armPlaneDef = {
		{56702, 0.0836451f, 0.461388f},
		D3DXVECTOR3(0.999626f, -0.0234326f, 0.0140953f)
	};
	kBone	longArm;
	longArm.set(*this, armPlaneDef, NULL, 1.2f, false, NULL);
	longArm.apply(*this);
#else	// 다리를 길어 보이게 하기/테스트
#if 1	// 실제
	PLANE_DEF	planeDef2 = {	// 정강이 위쪽
#if 0
		{71579, 0.63355f, 0.352928f},
		D3DXVECTOR3(-0.801211f, 0.577214f, 0.157755f)

#else
		{70939, 0.159472f, 0.829287f},
//		{69918, 0.652526f, 0.254092f},
		D3DXVECTOR3(0, 1, 0)
#endif
	};
	kBone::VERTEX_HASH::const_iterator	cit_hash;
	kBone	boneBetween;
	D3DXVECTOR3	P0(0, 0, 0);
/*
A: [75.9515, 984.014, -63.8398]		// 왼쪽 바깥쪽
B: [75.9515, 639.343, -63.8398]		// 안쪽 중간지점
C: [75.9515, 294.673, -63.8398]		// 오른쪽 바깥쪽
 */
	P0 = D3DXVECTOR3(75.9515f, 984.014f, -63.8398f);
//	P0 = D3DXVECTOR3(75.9515f, 639.343f, -63.8398f);
//	P0 = D3DXVECTOR3(75.9515f, 294.673f, -63.8398f);
	boneBetween.set(*this, planeDef0, &planeDef1.pointDef, 2.f, false, &P0);
	const kBone::VERTEX_HASH&	insideBetween = boneBetween.inside();
	const kBone::VERTEX_HASH&	outsideBetween = boneBetween.outside();
	cout << "# of ..." << insideBetween.size() << endl;
	cout << "# of ..." << outsideBetween.size() << endl;
	__statePtr->lower_FaceID_array = boneBetween.insideFaceIDArray();
	__statePtr->lower_FaceID_array = boneBetween.outsideFaceIDArray();
	std::vector<D3DXVECTOR3>	v;	// principal component analysis; pricipal axis (뼈 방향)
	for (cit_hash = insideBetween.begin(); cit_hash != insideBetween.end(); ++cit_hash) {
		__statePtr->debugging_prReprID_array.push_back((*cit_hash).first);
		v.push_back((*cit_hash).second);
	}
	D3DXVECTOR3&	centroid = boneBetween.centroid();
	D3DXVECTOR3&	nDir = boneBetween.nDir();
	kLinearLeastSquare::linear_least_square_line_3(v, centroid, nDir);
{
	__statePtr->points.RemoveAll();
	__statePtr->points.Add(centroid);
	D3DXVECTOR3	P = centroid + 100 * nDir;	// 100?
	__statePtr->points.Add(P);
}
#if 1
{
	P0 = centroid;
	planeDef0.normal = nDir;
	D3DXVECTOR3	P1;
	getPoint(planeDef0.pointDef, P1);
	D3DXVECTOR3	P0;	// P0:= centroid 에서 planeDef 에 내린 수선의 발
	P0 = centroid - D3DXVec3Dot(&D3DXVECTOR3(centroid - P1), &nDir) * nDir;
	boneBetween.set2(*this, planeDef0, &planeDef1.pointDef, 2.f, true, &P0, &dismap);
	const kBone::VERTEX_HASH&	insideBetween = boneBetween.inside();
	const kBone::VERTEX_HASH&	outsideBetween = boneBetween.outside();
	cout << "# of ..." << insideBetween.size() << endl;
	cout << "# of ..." << outsideBetween.size() << endl;
	__statePtr->lower_FaceID_array = boneBetween.insideFaceIDArray();
	__statePtr->lower_FaceID_array = boneBetween.outsideFaceIDArray();
	for (cit_hash = insideBetween.begin(); cit_hash != insideBetween.end(); ++cit_hash) {
		__statePtr->debugging_prReprID_array.push_back((*cit_hash).first);
	}
	__statePtr->debugging_prReprID_array.resize(0);
}
#endif

	boneBetween.apply(*this);
	D3DXVECTOR3	P0_star = boneBetween.P0_star();

	kBone	boneLower;
	PLANE_DEF	lowerPlaneDef;
	lowerPlaneDef.pointDef = planeDef2.pointDef;
	lowerPlaneDef.normal = -planeDef2.normal;
	boneLower.set(*this, lowerPlaneDef, NULL, 2.f, false, &P0_star);
	const kBone::VERTEX_HASH&	insideLower = boneLower.inside();
	const kBone::VERTEX_HASH&	outsideLower = boneLower.outside();
	cout << "# of ..." << insideLower.size() << endl;
	cout << "# of ..." << outsideLower.size() << endl;
	__statePtr->lower_FaceID_array = boneLower.insideFaceIDArray();
	for (int n = 0; n < (int) boneBetween.insideFaceIDArray().size(); ++n)
		__statePtr->lower_FaceID_array.push_back(boneBetween.insideFaceIDArray().at(n));
//	__statePtr->lower_FaceID_array = boneLower.outsideFaceIDArray();
	std::set<DWORD>	excludeSet;
	for (cit_hash = insideLower.begin(); cit_hash != insideLower.end(); ++cit_hash) {
		DWORD	prID = (*cit_hash).first;
		kBone::VERTEX_HASH::const_iterator	it = insideBetween.find(prID);
		if (it != insideBetween.end())
			excludeSet.insert(prID);
	}
	cout << "# of DWORD in excludeSet = " << excludeSet.size() << endl;
	std::set<DWORD>::const_iterator	cit_set;
	for (cit_set = excludeSet.begin(); cit_set != excludeSet.end(); ++cit_set)
		cout << *cit_set << endl;
	boneLower.apply(*this, &excludeSet);
#else	// 실제/예제
	kBone::VERTEX_HASH::const_iterator	cit_hash;
	kBone	boneBetween;
	boneBetween.set(*this, planeDef0, &planeDef1.pointDef, 2.f, true);
	const kBone::VERTEX_HASH&	insideBetween = boneBetween.inside();
	const kBone::VERTEX_HASH&	outsideBetween = boneBetween.outside();
	cout << "# of ..." << insideBetween.size() << endl;
	cout << "# of ..." << outsideBetween.size() << endl;
#if 1
	__statePtr->lower_FaceID_array = boneBetween.insideFaceIDArray();
	__statePtr->lower_FaceID_array = boneBetween.outsideFaceIDArray();
#else
	DWORD*	lowerFaceIDArray = 0;
	lowerFaceIDArray = new DWORD[__numFaces];
	FillMemory(lowerFaceIDArray, __numFaces * sizeof(DWORD), 0);
	for (cit_hash = insideBetween.begin(); cit_hash != insideBetween.end(); ++cit_hash) {
		DWORD	prID = (*cit_hash).first;
#ifdef __USE_A_OFFSET__
		for (int k = pointer_to__a_vFaceIDLocalIndex_a_offset[prID]; k < pointer_to__a_vFaceIDLocalIndex_a_offset[prID + 1]; ++k) {
			const FACE_ID_LOCAL_INDEX&	faceIDLocalIndex = pointer_to__a_vFaceIDLocalIndex[k];
#else
		for (size_t k = 0; k < __pointReprPtr[prID].vFaceIdLocalIndex.size(); ++k) {
			const FACE_ID_LOCAL_INDEX&	faceIDLocalIndex = __pointReprPtr[prID].vFaceIdLocalIndex.at(k);
#endif
			DWORD	faceID = faceIDLocalIndex.ID;
			if (lowerFaceIDArray[faceID]) {	// 이미 등록되어있다.
				continue;
			}
			bool	allInside = true;
			const CFace&	face = __facePtr[faceID];
			for (int i = 0; i < 3; ++i) {
				kBone::VERTEX_HASH::const_iterator	it = insideBetween.find(face.prID[i]);
				if (it == insideBetween.end()) {
					allInside = false;
					break;
				}
			}
			if (allInside)
				lowerFaceIDArray[faceID] = 1;
#ifdef __USE_A_OFFSET__
		}
#else
		}
#endif
//		__statePtr->debugging_prReprID_array.push_back((*cit_hash).first);
	}
	for (int n = 0; n < (int) __numFaces; ++n) {
		if (lowerFaceIDArray[n])
			__statePtr->lower_FaceID_array.push_back(n);
	}
	if (lowerFaceIDArray) {
		delete[] lowerFaceIDArray;
		lowerFaceIDArray = 0;
	}
#endif
	for (cit_hash = insideBetween.begin(); cit_hash != insideBetween.end(); ++cit_hash)
		__statePtr->debugging_prReprID_array.push_back((*cit_hash).first);
	boneBetween.apply(*this);

	kBone	boneLower;
	PLANE_DEF	lowerPlaneDef;
	lowerPlaneDef.pointDef = planeDef1.pointDef;
	lowerPlaneDef.normal = -planeDef1.normal;
	boneLower.set(*this, lowerPlaneDef, NULL, 2.f, true);
	const kBone::VERTEX_HASH&	insideLower = boneLower.inside();
	const kBone::VERTEX_HASH&	outsideLower = boneLower.outside();
	cout << "# of ..." << insideLower.size() << endl;
	cout << "# of ..." << outsideLower.size() << endl;
	__statePtr->lower_FaceID_array = boneLower.insideFaceIDArray();
//	__statePtr->lower_FaceID_array = boneLower.outsideFaceIDArray();
	std::set<DWORD>	excludeSet;
	for (cit_hash = insideLower.begin(); cit_hash != insideLower.end(); ++cit_hash) {
		DWORD	prID = (*cit_hash).first;
		kBone::VERTEX_HASH::const_iterator	it = insideBetween.find(prID);
		if (it != insideBetween.end())
			excludeSet.insert(prID);
	}
	cout << "# of DWORD in excludeSet = " << excludeSet.size() << endl;
	std::set<DWORD>::const_iterator	cit_set;
	for (cit_set = excludeSet.begin(); cit_set != excludeSet.end(); ++cit_set)
		cout << *cit_set << endl;
	boneLower.apply(*this, &excludeSet);
#endif	// 예제
#endif	// 테스트
#else	// kBone/!kBone
	std::vector<DWORD>	betweenFaceIDArray;	// 허벅지 위쪽과 아래쪽 사이에 있는 면들의 목록
#if 1	// PLANE_DEF
	kMesh::CollectBetweenFaces(planeDef0, planeDef1, betweenFaceIDArray, this);
#else	// PLANE_DEF/POINT_DEF
	POINT_DEF	pointDef = planeDef1.pointDef;
	kMesh::CollectBetweenFaces(planeDef0, pointDef, betweenFaceIDArray, this);
#endif	// POINT_DEF

	std::vector<DWORD>	lowerFaceIDArray;	// 허벅지 아래에 있는 면들의 목록;
							// 허벅지 아래쪽을 정의하는 PLANE_DEF 의 normal 이
							// 허벅지 위쪽을 향해야 한다.
	kMesh::CollectLowerFaces(NULL, planeDef1, lowerFaceIDArray, this);

	__statePtr->lower_FaceID_array = lowerFaceIDArray;

	DWORD*	betweenPointReprIDArray = 0;
	DWORD*	lowerPointReprIDArray = 0;
	do {
		betweenPointReprIDArray = new DWORD[__numPointReprs];
		FillMemory(betweenPointReprIDArray, __numPointReprs * sizeof(DWORD), 0);
		for (int n = 0; n < (int) betweenFaceIDArray.size(); ++n) {
			const CFace&	face = __facePtr[betweenFaceIDArray.at(n)];
			for (int k = 0; k < 3; ++k)
				betweenPointReprIDArray[face.prID[k]] = 1;
		}
		lowerPointReprIDArray = new DWORD[__numPointReprs];
		FillMemory(lowerPointReprIDArray, __numPointReprs * sizeof(DWORD), 0);
		for (int n = 0; n < (int) lowerFaceIDArray.size(); ++n) {
			const CFace&	face = __facePtr[lowerFaceIDArray.at(n)];
			for (int k = 0; k < 3; ++k)
				lowerPointReprIDArray[face.prID[k]] = 1;
		}
#if 0
		__statePtr->debugging_prReprID_array.resize(0);
		for (int i = 0; i < (int) __numPointReprs; ++i) {
			if (betweenPointReprIDArray[i])	// lowerPointReprIDArray[i]
				__statePtr->debugging_prReprID_array.push_back(i);
		}
#endif

#if 0	// 뼈 길이 변화
		float	f = 2.f;
{
		CPointRepr::VERTEX*	vertexArray;
		__meshLoader.meshSubsetPtr()->LockVertexBuffer(0, (void**) &vertexArray);
		D3DXVECTOR3	P0;
		getPoint(planeDef0.pointDef, P0);
		D3DXVECTOR3	P1;
#if 1	// PLANE_DEF
		getPoint(planeDef1.pointDef, P1);
#else	// PLANE_DEF/POINT_DEF
		getPoint(pointDef, P1);
#endif	// POINT_DEF
		D3DXVECTOR3	P0P1 = P1 - P0;
		D3DXVECTOR3	nDir = D3DXVec3Dot(&P0P1, &planeDef0.normal) ? planeDef0.normal : -planeDef0.normal;
#if 0	// between
		D3DXVECTOR3	ell_between = D3DXVec3Dot(&P0P1, &nDir) * nDir;
#endif	// between
		for (int n = 0; n < (int) __numPointReprs; ++n) {
#if 0	// between
			if (betweenPointReprIDArray[n]) {
				CPointRepr&	pointRepr = __pointReprPtr[n];
				CPointRepr::VERTEX&	v = pointRepr.v;
				D3DXVECTOR3	ell = v.position - P0;
				D3DXVECTOR3	ell_longitudinal = D3DXVec3Dot(&ell, &nDir) * nDir;
				D3DXVECTOR3	ell_transversal = ell - ell_longitudinal;
				v.position = P0 + (f * ell_longitudinal + ell_transversal);
#ifdef __USE_A_OFFSET__
				for (int k = pointer_to__a_vVertexID_a_offset[n]; k < pointer_to__a_vVertexID_a_offset[n + 1]; ++k)
					vertexArray[pointer_to__a_vVertexID[k]].position = v.position;
#else
				const std::set<DWORD>&	vVertexID = pointRepr.vVertexID;
				for (std::set<DWORD>::const_iterator it = vVertexID.begin(); it != vVertexID.end(); ++it)
					vertexArray[*it].position = v.position;
#endif
			}
			if (lowerPointReprIDArray[n]) {
				CPointRepr&	pointRepr = __pointReprPtr[n];
				CPointRepr::VERTEX&	v = pointRepr.v;
				D3DXVECTOR3	ell = v.position - P0;
				D3DXVECTOR3	ell_longitudinal = D3DXVec3Dot(&ell, &nDir) * nDir;
				D3DXVECTOR3	ell_transversal = ell - ell_longitudinal;
				// ell_longitudinal 은 betweenFaceIDArray 에 있는 부분과
				// lowerFaceIDArray 에 있는 부분으로 나눈다.
				D3DXVECTOR3	ell_lower = ell_longitudinal - ell_between;
				v.position = P0 + (f * ell_between + ell_lower + ell_transversal);
#ifdef __USE_A_OFFSET__
				for (int k = pointer_to__a_vVertexID_a_offset[n]; k < pointer_to__a_vVertexID_a_offset[n + 1]; ++k)
					vertexArray[pointer_to__a_vVertexID[k]].position = v.position;
#else
				const std::set<DWORD>&	vVertexID = pointRepr.vVertexID;
				for (std::set<DWORD>::const_iterator it = vVertexID.begin(); it != vVertexID.end(); ++it)
					vertexArray[*it].position = v.position;
#endif
			}
#else	// between/lower
			if (lowerPointReprIDArray[n]) {
				CPointRepr&	pointRepr = __pointReprPtr[n];
				CPointRepr::VERTEX&	v = pointRepr.v;
				D3DXVECTOR3	ell = v.position - P1;
				D3DXVECTOR3	ell_longitudinal = D3DXVec3Dot(&ell, &nDir) * nDir;
				D3DXVECTOR3	ell_transversal = ell - ell_longitudinal;
				v.position = P1 + (f * ell_longitudinal + ell_transversal);
#ifdef __USE_A_OFFSET__
				for (int k = pointer_to__a_vVertexID_a_offset[n]; k < pointer_to__a_vVertexID_a_offset[n + 1]; ++k)
					vertexArray[pointer_to__a_vVertexID[k]].position = v.position;
#else
				for (std::set<DWORD>::const_iterator it = vVertexID.begin(); it != vVertexID.end(); ++it)
					vertexArray[*it].position = v.position;
#endif
			}
#endif	// lower
		}
		__meshLoader.meshSubsetPtr()->UnlockVertexBuffer();
}
#endif	// 뼈 길이 변화
	} while (0);
	SAFE_DELETE_ARRAY(betweenPointReprIDArray);
	SAFE_DELETE_ARRAY(lowerPointReprIDArray);
#endif	// !kBone
}
#endif	// 뼈의 길이 변경
#endif	// STANDARD_MODEL
#if 1	// __fScale; kMesh
{
	for (int i = 0; i < (int) __numPointReprs; ++i) {
		CPointRepr&	pointRepr = __pointReprPtr[i];
		CPointRepr::VERTEX&	v = pointRepr.v;
#if 1	// 뼈
		pointRepr.v.position0 *= __fScale;
#endif	// 뼈
		v.position *= __fScale;
	}
}
#endif	// __fScale; kMesh
#ifdef __WRITE_FILES__
#ifndef __USE_A_OFFSET
	(void) fprintf(stdout, "Writing...\n");
	// SetPolygon 에서 결정한 CPointRepr (CPointRepr::v 제외), CFace, CEdge, __vertexID2pointReprIDMap 를 출력:
	//	__pointReprPtr, __facePtr, __edgePtr, __adjacencyPtr, __vertexID2pointReprIDMap
	//
	// 서로 다른 .OBJ 파일이더라도
	//	1) 같은 오브젝트를 기술하고
	//	2) Face 의 순서가 같다면
	// 이 모든 것들이 같다.
{
#if 1	// __adjacencyPtr
{
	FILE*	filePtr = 0;
	char	filename[] = ADJACENCY_PTR__FILENAME;
do {
#if defined(_MSC_VER) && (_MSC_VER >= 1400)	// Visual Studio 2005 or above
	errno_t	err = fopen_s(&filePtr, filename, WRITE_MODE);
	assert(err == 0);
	if (err != 0) {
		break;
	}
#else
	filePtr = fopen(filename, WRITE_MODE);
#endif
	(void) fprintf(stdout, "\tWriting %s...\n", filename);	// {
	////////////////////////////////////////////////////////////////////////
#ifdef MULTI_STANDARD
	(void) fprintf(filePtr, "static const DWORD __adjacencyPtr_ID%d__[%d]={", STANDARD_ID, 3 * __numFaces);
	(void) fprintf(stdout, "/* ... */	{__adjacencyPtr_ID%d__, %d},\n", STANDARD_ID, 3 * __numFaces);
#else	// MULTI_STANDARD/!MULTI_STANDARD
	(void) fprintf(filePtr, "static const DWORD __adjacencyPtr__[%d]={", 3 * __numFaces);
#endif	// !MULTI_STANDARD
	for (DWORD faceID = 0; faceID < __numFaces; ++faceID)
		(void) fprintf(filePtr,
			"%s%d,%s%d,%s%d,"
			,
			__adjacencyPtr[3 * faceID + 0] == (DWORD) -1 ? "(DWORD)" : "",
			__adjacencyPtr[3 * faceID + 0],
			__adjacencyPtr[3 * faceID + 1] == (DWORD) -1 ? "(DWORD)" : "",
			__adjacencyPtr[3 * faceID + 1],
			__adjacencyPtr[3 * faceID + 2] == (DWORD) -1 ? "(DWORD)" : "",
			__adjacencyPtr[3 * faceID + 2]
		);
	(void) fprintf(filePtr, "};\n");
	////////////////////////////////////////////////////////////////////////
	(void) fprintf(stdout, "...Done\n");			// }
} while (0);
	if (filePtr) {
		(void) fclose(filePtr);
		filePtr = 0;
	}
}
#endif
#if 1	// __vertexID2pointReprIDMap
{
	FILE*	filePtr = 0;
	char	filename[] = VERTEXID2POINTREPRIDMAP__FILENAME;
do {
#if defined(_MSC_VER) && (_MSC_VER >= 1400)	// Visual Studio 2005 or above
	errno_t	err = fopen_s(&filePtr, filename, WRITE_MODE);
	assert(err == 0);
	if (err != 0) {
		break;
	}
#else
	filePtr = fopen(filename, WRITE_MODE);
#endif
	(void) fprintf(stdout, "\tWriting %s...\n", filename);	// {
	////////////////////////////////////////////////////////////////////////
#ifdef MULTI_STANDARD
	(void) fprintf(filePtr, "static const int __vertexID2pointReprIDMap_ID%d__[%d]={", STANDARD_ID, 3 * __numFaces);
	(void) fprintf(stdout, "/* ... */	{__vertexID2pointReprIDMap_ID%d__, %d},\n", STANDARD_ID, 3 * __numFaces);
#else	// MULTI_STANDARD/!MULTI_STANDARD
	(void) fprintf(filePtr, "static const int __vertexID2pointReprIDMap__[%d]={", 3 * __numFaces);
#endif	// !MULTI_STANDARD
	for (DWORD vertexID = 0; vertexID < 3 * __numFaces; ++vertexID)
		(void) fprintf(filePtr, "%d,", __vertexID2pointReprIDMap[vertexID]);
	(void) fprintf(filePtr, "};\n");
	////////////////////////////////////////////////////////////////////////
	(void) fprintf(stdout, "...Done\n");			// }
} while (0);
	if (filePtr) {
		(void) fclose(filePtr);
		filePtr = 0;
	}
}
#endif
#if 1	// __pointReprPtr
{
	FILE*	filePtr = 0;
	char	filename[] = POINTREPR_PTR__FILENAME;
do {
#if defined(_MSC_VER) && (_MSC_VER >= 1400)	// Visual Studio 2005 or above
	errno_t	err = fopen_s(&filePtr, filename, WRITE_MODE);
	assert(err == 0);
	if (err != 0) {
		break;
	}
#else
	filePtr = fopen(filename, WRITE_MODE);
#endif
	(void) fprintf(stdout, "\tWriting %s...\n", filename);	// {
	////////////////////////////////////////////////////////////////////////
#ifdef MULTI_STANDARD
	(void) fprintf(filePtr, "static const int __numPointReprs_ID%d__=%d;\n", STANDARD_ID, __numPointReprs);
	(void) fprintf(stdout, "/* ... */	__numPointReprs_ID%d__,\n", STANDARD_ID, __numPointReprs);
#else	// MULTI_STANDARD/!MULTI_STANDARD
	(void) fprintf(filePtr, "static const int __numPointReprs__=%d;\n", __numPointReprs);
#endif	// !MULTI_STANDARD
	CGrowableArray<DWORD>			a_vVertexID;
	CGrowableArray<int>			a_vVertexID_a_offset;
	CGrowableArray<EDGE>			a_vEdgeID;
	CGrowableArray<int>			a_vEdgeID_a_offset;
	CGrowableArray<FACE_ID_LOCAL_INDEX>	a_vFaceIDLocalIndex;
	CGrowableArray<int>			a_vFaceIDLocalIndex_a_offset;
	for (DWORD prID = 0; prID < __numPointReprs; ++prID) {
		const CPointRepr&	pointRepr = __pointReprPtr[prID];

		const std::set<DWORD>&	vVertexID = pointRepr.vVertexID;
		a_vVertexID_a_offset.Add(a_vVertexID.GetSize());
		for (std::set<DWORD>::const_iterator it = vVertexID.begin(); it != vVertexID.end(); ++it)
			a_vVertexID.Add(*it);

		const std::vector<EDGE>&	vEdgeID = pointRepr.vEdgeID;
		a_vEdgeID_a_offset.Add(a_vEdgeID.GetSize());
		for (int n = 0; n < (int) vEdgeID.size(); ++n)
			a_vEdgeID.Add(vEdgeID.at(n));

		const std::vector<FACE_ID_LOCAL_INDEX>&	vFaceIdLocalIndex = pointRepr.vFaceIdLocalIndex;
		a_vFaceIDLocalIndex_a_offset.Add(a_vFaceIDLocalIndex.GetSize());
		for (int n = 0; n < (int) vFaceIdLocalIndex.size(); ++n)
			a_vFaceIDLocalIndex.Add(vFaceIdLocalIndex.at(n));
	}

#ifdef MULTI_STANDARD
	(void) fprintf(filePtr, "static const DWORD __a_vVertexID_ID%d[%d]={", STANDARD_ID, a_vVertexID.GetSize());
	(void) fprintf(stdout, "/* ... */	{__a_vVertexID_ID%d, %d},\n", STANDARD_ID, a_vVertexID.GetSize());
#else	// MULTI_STANDARD/!MULTI_STANDARD
	(void) fprintf(filePtr, "static const DWORD __a_vVertexID[%d]={", a_vVertexID.GetSize());
#endif	// !MULTI_STANDARD
	for (int n = 0; n < a_vVertexID.GetSize(); ++n)
		(void) fprintf(filePtr, "%d,", a_vVertexID.GetAt(n));
	(void) fprintf(filePtr, "};\n");
#ifdef MULTI_STANDARD
	(void) fprintf(filePtr, "static const int __a_vVertexID_a_offset_ID%d[%d]={", STANDARD_ID, a_vVertexID_a_offset.GetSize() + 1);
	(void) fprintf(stdout, "/* ... */	{__a_vVertexID_a_offset_ID%d, %d},\n", STANDARD_ID, a_vVertexID_a_offset.GetSize() + 1);
#else	// MULTI_STANDARD/!MULTI_STANDARD
	(void) fprintf(filePtr, "static const int __a_vVertexID_a_offset[%d]={", a_vVertexID_a_offset.GetSize() + 1);
#endif	// !MULTI_STANDARD
	for (int n = 0; n < a_vVertexID_a_offset.GetSize(); ++n)
		(void) fprintf(filePtr, "%d,", a_vVertexID_a_offset.GetAt(n));
	// pointer_to__a_vVertexID_a_offset 의 크기는 __numPointReprs 보다 1만큼 더 크다.
	//	prID 에 해당하는 CPointRepr::vVertexID 에 대한 목록을 얻기위해
	//	pointer_to__a_vVertexID_a_offset[prID] 를 인덱스로 사용하여 pointer_to__a_vVertexID 배열을 접근한다;
	//	pointer_to__a_vVertexID 배열의 pointer_to__a_vVertexID_a_offset[prID] 번째 성분부터
	//	pointer_to__a_vVertexID_a_offset[prID + 1] 번째 성분 바로 전(前)까지가 이 목록에 해당한다.
	//	이 관계가 prID = __numPointReprs - 1 일 때도 성립하도록
	//	pointer_to__a_vVertexID_a_offset 에 다음 값을 추가한다.
	(void) fprintf(filePtr, "%d,", a_vVertexID.GetSize());
	(void) fprintf(filePtr, "};\n");

#ifdef MULTI_STANDARD
	(void) fprintf(filePtr, "static const EDGE __a_vEdgeID_ID%d[%d]={", STANDARD_ID, a_vEdgeID.GetSize());
	(void) fprintf(stdout, "/* ... */	{__a_vEdgeID_ID%d, %d},\n", STANDARD_ID, a_vEdgeID.GetSize());
#else	// MULTI_STANDARD/!MULTI_STANDARD
	(void) fprintf(filePtr, "static const EDGE __a_vEdgeID[%d]={", a_vEdgeID.GetSize());
#endif	// !MULTI_STANDARD
	for (int n = 0; n < a_vEdgeID.GetSize(); ++n) {
		const EDGE&	edge = a_vEdgeID.GetAt(n);
		(void) fprintf(filePtr, "{%d,%d},", edge.id, edge.bDir);
	}
	(void) fprintf(filePtr, "};\n");
#ifdef MULTI_STANDARD
	(void) fprintf(filePtr, "static const int __a_vEdgeID_a_offset_ID%d[%d]={", STANDARD_ID, a_vEdgeID_a_offset.GetSize() + 1);
	(void) fprintf(stdout, "/* ... */	{__a_vEdgeID_a_offset_ID%d, %d},\n", STANDARD_ID, a_vEdgeID_a_offset.GetSize() + 1);
#else	// MULTI_STANDARD/!MULTI_STANDARD
	(void) fprintf(filePtr, "static const int __a_vEdgeID_a_offset[%d]={", a_vEdgeID_a_offset.GetSize() + 1);
#endif	// !MULTI_STANDARD
	for (int n = 0; n < a_vEdgeID_a_offset.GetSize(); ++n)
		(void) fprintf(filePtr, "%d,", a_vEdgeID_a_offset.GetAt(n));
	// 바로 전(前) 설명과 같은 이유로...
	(void) fprintf(filePtr, "%d,", a_vEdgeID.GetSize());
	(void) fprintf(filePtr, "};\n");

#ifdef MULTI_STANDARD
	(void) fprintf(filePtr, "static const FACE_ID_LOCAL_INDEX __a_vFaceIDLocalIndex_ID%d[%d]={", STANDARD_ID, a_vFaceIDLocalIndex.GetSize());
	(void) fprintf(stdout, "/* ... */	{__a_vFaceIDLocalIndex_ID%d, %d},\n", STANDARD_ID, a_vFaceIDLocalIndex.GetSize());
#else	// MULTI_STANDARD/!MULTI_STANDARD
	(void) fprintf(filePtr, "static const FACE_ID_LOCAL_INDEX __a_vFaceIDLocalIndex[%d]={", a_vFaceIDLocalIndex.GetSize());
#endif	// !MULTI_STANDARD
	for (int n = 0; n < a_vFaceIDLocalIndex.GetSize(); ++n) {
		const FACE_ID_LOCAL_INDEX&	faceIDLocalIndex = a_vFaceIDLocalIndex.GetAt(n);
		(void) fprintf(filePtr, "{%d,%d},", faceIDLocalIndex.ID, faceIDLocalIndex.local_index);
	}
	(void) fprintf(filePtr, "};\n");
#ifdef MULTI_STANDARD
	(void) fprintf(filePtr, "static const int __a_vFaceIDLocalIndex_a_offset_ID%d[%d]={", STANDARD_ID, a_vFaceIDLocalIndex_a_offset.GetSize() + 1);
	(void) fprintf(stdout, "/* ... */	{__a_vFaceIDLocalIndex_a_offset_ID%d, %d},\n", STANDARD_ID, a_vFaceIDLocalIndex_a_offset.GetSize() + 1);
#else	// MULTI_STANDARD/!MULTI_STANDARD
	(void) fprintf(filePtr, "static const int __a_vFaceIDLocalIndex_a_offset[%d]={", a_vFaceIDLocalIndex_a_offset.GetSize() + 1);
#endif	// !MULTI_STANDARD
	for (int n = 0; n < a_vFaceIDLocalIndex_a_offset.GetSize(); ++n)
		(void) fprintf(filePtr, "%d,", a_vFaceIDLocalIndex_a_offset.GetAt(n));
	// 바로 전(前) 설명과 같은 이유로...
	(void) fprintf(filePtr, "%d,", a_vFaceIDLocalIndex.GetSize());
	(void) fprintf(filePtr, "};\n");
	////////////////////////////////////////////////////////////////////////
	(void) fprintf(stdout, "...Done\n");			// }
} while (0);
	if (filePtr) {
		(void) fclose(filePtr);
		filePtr = 0;
	}
}
#endif
#if 1	// __edgePtr
{
	FILE*	filePtr = 0;
	char	filename[] = EDGE_PTR__FILENAME;
do {
#if defined(_MSC_VER) && (_MSC_VER >= 1400)	// Visual Studio 2005 or above
	errno_t	err = fopen_s(&filePtr, filename, WRITE_MODE);
	assert(err == 0);
	if (err != 0) {
		break;
	}
#else
	filePtr = fopen(filename, WRITE_MODE);
#endif
	(void) fprintf(stdout, "\tWriting %s...\n", filename);	// {
	////////////////////////////////////////////////////////////////////////
#ifdef MULTI_STANDARD
	(void) fprintf(filePtr, "static const int __numEdges_ID%d__=%d;\n", STANDARD_ID, __numEdges);
	(void) fprintf(stdout, "/* ... */	__numEdges_ID%d__,\n", STANDARD_ID, __numEdges);
#else	// MULTI_STANDARD/!MULTI_STANDARD
	(void) fprintf(filePtr, "static const int __numEdges__=%d;\n", __numEdges);
#endif	// !MULTI_STANDARD
	CGrowableArray<DWORD>	a_faceID;
	CGrowableArray<int>	a_faceID_a_offset;
#ifdef MULTI_STANDARD
	(void) fprintf(filePtr, "static const int __edgePtr_ID%d__[%d]={", STANDARD_ID, 2 * __numEdges);
	(void) fprintf(stdout, "/* ... */	{__edgePtr_ID%d__, %d},\n", STANDARD_ID, 2 * __numEdges);
#else	// MULTI_STANDARD/!MULTI_STANDARD
	(void) fprintf(filePtr, "static const int __edgePtr__[%d]={", 2 * __numEdges);
#endif	// !MULTI_STANDARD
	for (DWORD edgeID = 0; edgeID < __numEdges; ++edgeID) {
		const CEdge&	edge = __edgePtr[edgeID];

		(void) fprintf(filePtr, "%d,%d,", edge.prStartID, edge.prEndID);

		const std::vector<DWORD>&	faceID = edge.faceID;
		a_faceID_a_offset.Add(a_faceID.GetSize());
		for (int n = 0; n < (int) faceID.size(); ++n)
			a_faceID.Add(faceID.at(n));
	}
	(void) fprintf(filePtr, "};\n");

#ifdef MULTI_STANDARD
	(void) fprintf(filePtr, "static const DWORD __a_faceID_ID%d[%d]={", STANDARD_ID, a_faceID.GetSize());
	(void) fprintf(stdout, "/* ... */	{__a_faceID_ID%d, %d},\n", STANDARD_ID, a_faceID.GetSize());
#else	// MULTI_STANDARD/!MULTI_STANDARD
	(void) fprintf(filePtr, "static const DWORD __a_faceID[%d]={", a_faceID.GetSize());
#endif	// !MULTI_STANDARD
	for (int n = 0; n < a_faceID.GetSize(); ++n)
		(void) fprintf(filePtr, "%d,", a_faceID.GetAt(n));
	(void) fprintf(filePtr, "};\n");
#ifdef MULTI_STANDARD
	(void) fprintf(filePtr, "static const int __a_faceID_a_offset_ID%d[%d]={", STANDARD_ID, a_faceID_a_offset.GetSize() + 1);
	(void) fprintf(stdout, "/* ... */	{__a_faceID_a_offset_ID%d, %d},\n", STANDARD_ID, a_faceID_a_offset.GetSize() + 1);
#else	// MULTI_STANDARD/!MULTI_STANDARD
	(void) fprintf(filePtr, "static const int __a_faceID_a_offset[%d]={", a_faceID_a_offset.GetSize() + 1);
#endif	// !MULTI_STANDARD
	for (int n = 0; n < a_faceID_a_offset.GetSize(); ++n)
		(void) fprintf(filePtr, "%d,", a_faceID_a_offset.GetAt(n));
	// CPointRepr 의 a_vVertexID_a_offset 에 있던 설명과 같은 이유로...
	(void) fprintf(filePtr, "%d,", a_faceID.GetSize());
	(void) fprintf(filePtr, "};\n");
	////////////////////////////////////////////////////////////////////////
	(void) fprintf(stdout, "...Done\n");			// }
} while (0);
	if (filePtr) {
		(void) fclose(filePtr);
		filePtr = 0;
	}
}
#endif
#if 1	// __facePtr
{
	FILE*	filePtr = 0;
	char	filename[] = FACE_PTR__FILENAME;
do {
#if defined(_MSC_VER) && (_MSC_VER >= 1400)	// Visual Studio 2005 or above
	errno_t	err = fopen_s(&filePtr, filename, WRITE_MODE);
	assert(err == 0);
	if (err != 0) {
		break;
	}
#else
	filePtr = fopen(filename, WRITE_MODE);
#endif
	(void) fprintf(stdout, "\tWriting %s...\n", filename);	// {
	////////////////////////////////////////////////////////////////////////
#ifdef MULTI_STANDARD
	(void) fprintf(filePtr, "static const int __numFaces_ID%d__=%d;\n", STANDARD_ID, __numFaces);
	(void) fprintf(stdout, "/* ... */	__numFaces_ID%d__,\n", STANDARD_ID, __numFaces);
	(void) fprintf(filePtr, "static const int __facePtr_ID%d__[%d]={", STANDARD_ID, 12 * __numFaces);
	(void) fprintf(stdout, "/* ... */	{__facePtr_ID%d__, %d},\n", STANDARD_ID, 12 * __numFaces);
#else	// MULTI_STANDARD/!MULTI_STANDARD
	(void) fprintf(filePtr, "static const int __numFaces__=%d;\n", __numFaces);
	(void) fprintf(filePtr, "static const int __facePtr__[%d]={", 12 * __numFaces);
#endif	// !MULTI_STANDARD
	for (DWORD faceID = 0; faceID < __numFaces; ++faceID) {
		const CFace&	face = __facePtr[faceID];
		(void) fprintf(filePtr,
			"%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,",
			face.prID[0], face.prID[1], face.prID[2],
			face.edgeID[0].id, face.edgeID[0].bDir, face.edgeID[1].id, face.edgeID[1].bDir, face.edgeID[2].id, face.edgeID[2].bDir,
			face.ajID[0], face.ajID[1], face.ajID[2]
		);
	}
	(void) fprintf(filePtr, "};\n");
	////////////////////////////////////////////////////////////////////////
	(void) fprintf(stdout, "...Done\n");			// }
} while (0);
	if (filePtr) {
		(void) fclose(filePtr);
		filePtr = 0;
	}
}
#endif
}
	(void) fprintf(stdout, "...Done\n");

	(void) fprintf(stdout, "********************************************************\n");
	(void) fprintf(stdout, "Copy files, recompile with __READ_FILES__ and run\n");
	(void) fprintf(stdout, "********************************************************\n");
exit(0);
#endif	// !__USE_A_OFFSET
#endif	// __WRITE_FILES__
#if 1	// __fScale; kMeshLoader
	// 텍스쳐를 입힐 메쉬의 크기를 그렇지 않은 메쉬의 크기로 맞춘다.
	if (objFilePtr && __meshLoader.meshSubsetPtr()) {
		CPointRepr::VERTEX*	vertexArray;
		__meshLoader.meshSubsetPtr()->LockVertexBuffer(0, (void**) &vertexArray);
		for (int n = 0; n < (int) __meshLoader.meshSubsetPtr()->GetNumVertices(); ++n) {
			CPointRepr::VERTEX&	v = vertexArray[n];
			v.position *= __fScale;
		}
		__meshLoader.meshSubsetPtr()->UnlockVertexBuffer();
	}
#endif	// __fScale; kMeshLoader
#if 0	// group
	// 그룹정보를 파일에 쓴다. (확인용)
	const kMeshLoader::GROUP_INFO_HASH&	__groupMap = __meshLoader.groupMap();
	if (__groupMap.size()) {
		FILE*	filePtr_group = 0;
		char	filename[] = "file_group";
#if defined(_MSC_VER) && (_MSC_VER >= 1400)	// Visual Studio 2005 or above
		errno_t	err = fopen_s(&filePtr_group, filename, "w");
		assert(err == 0);
		if (err != 0) {
			return 0;
		}
#else
		filePtr0 = fopen(filename, "w");
#endif

		// 먼저 Group 정보를 쓴다.
		kMeshLoader::GROUP_INFO_HASH::const_iterator	it;
		for (it = __groupMap.begin(); it != __groupMap.end(); ++it) {
			const struct kMeshLoader::GROUP_INFO&	groupInfo = it->second;
			fprintf(filePtr_group, "%s (%d):\n", it->first.c_str(), groupInfo.__gid.id());
			for (int i = 0; i < (int) groupInfo.__fidList.size(); ++i)
				fprintf(filePtr_group, "\t%d\n", groupInfo.__fidList.at(i).id());
		}

		// 주어진 Face ID 에 해당하는 Group ID 를 쓴다. (확인용)
		for (DWORD faceID = 0; faceID < __numFaces; ++faceID) {
			kID	fid(faceID);
			const kMeshLoader::FID2GID_HASH&	__fid2gidMap = __meshLoader.fid2gidMap();
			kMeshLoader::FID2GID_HASH::const_iterator	it;
			it = __fid2gidMap.find(fid);
			if (it == __fid2gidMap.end()) {	// 이럴 수가...
				fprintf(stderr, "fid = %d: No gid\n", fid.id());
			} else {
				fprintf(filePtr_group, "fid = %d -> gid = %d\n", fid.id(), it->second.id());
			}
//			const CFace&	face = __facePtr[faceID];
		}
		fclose(filePtr_group);
	}
#endif
	(void) fprintf(stdout, "SetMesh... (%f)\n", __timer.GetElapsedTime());
	SetMesh();	// 새롭게 D3DXMESH 를 만들어 __meshPtr 을 설정한다;
			// 대부분은 __vertexBufferPtr, __pointReprPtr 을 통해서 필요한 것들을 다룰 수 있다.
			// 노말을 결정하기 위해 D3DXComputeNormals 을 사용할 때 필요하다.
			// 노말을 결정하고 버려도 되는 데,
			// 하지만 D3DXIntersect 에서는 __meshPtr 을 사용하는 게 빠르므로 그대로 둔다.
	(void) fprintf(stdout, "...Done (%f)\n", __timer.GetElapsedTime());
#if 0	// 뼈의 길이 변경;
	// 렌더링만 변경; __pointReprPtr 은 그대로 두고 다음만 변경한다:
	//	__vertexBufferPtr (렌더링)
	//	__meshPtr (Bounding Box, Collision)
	//	kMeshLoader::__meshSubsetPtr (텍스쳐)
{
/*
POINT_DEF: (63763, 0.19955, 0.350599)
nDir: [0, 1, 0]

POINT_DEF: (69918, 0.652526, 0.254092)
nDir: [0, 1, 0]
 */
	PLANE_DEF	planeDef0 = {	// 허벅지 위쪽
		{63763, 0.19955f, 0.350599f},
		D3DXVECTOR3(0, 1, 0)
	};
	PLANE_DEF	planeDef1 = {	// 허벅지 아래쪽
		{69918, 0.652526f, 0.254092f},
		D3DXVECTOR3(0, 1, 0)
	};

	std::vector<DWORD>	betweenFaceIDArray;	// 허벅지 위쪽과 아래쪽 사이에 있는 면들의 목록
#if 1	// PLANE_DEF
	kMesh::CollectBetweenFaces(planeDef0, planeDef1, betweenFaceIDArray, this);
#else	// PLANE_DEF/POINT_DEF
	POINT_DEF	pointDef = planeDef1.pointDef;
	kMesh::CollectBetweenFaces(planeDef0, pointDef, betweenFaceIDArray, this);
#endif	// POINT_DEF

	std::vector<DWORD>	lowerFaceIDArray;	// 허벅지 아래에 있는 면들의 목록;
							// 허벅지 아래쪽을 정의하는 PLANE_DEF 의 normal 이
							// 허벅지 위쪽을 향해야 한다.
	kMesh::CollectLowerFaces(NULL, planeDef1, lowerFaceIDArray, this);

	__statePtr->lower_FaceID_array = lowerFaceIDArray;

	DWORD*	betweenPointReprIDArray = 0;
	DWORD*	lowerPointReprIDArray = 0;
	do {
		betweenPointReprIDArray = new DWORD[__numPointReprs];
		FillMemory(betweenPointReprIDArray, __numPointReprs * sizeof(DWORD), 0);
		for (int n = 0; n < (int) betweenFaceIDArray.size(); ++n) {
			const CFace&	face = __facePtr[betweenFaceIDArray.at(n)];
			for (int k = 0; k < 3; ++k)
				betweenPointReprIDArray[face.prID[k]] = 1;
		}
		lowerPointReprIDArray = new DWORD[__numPointReprs];
		FillMemory(lowerPointReprIDArray, __numPointReprs * sizeof(DWORD), 0);
		for (int n = 0; n < (int) lowerFaceIDArray.size(); ++n) {
			const CFace&	face = __facePtr[lowerFaceIDArray.at(n)];
			for (int k = 0; k < 3; ++k)
				lowerPointReprIDArray[face.prID[k]] = 1;
		}
#if 1
		__statePtr->debugging_prReprID_array.resize(0);
		for (int i = 0; i < (int) __numPointReprs; ++i) {
			if (betweenPointReprIDArray[i])	// lowerPointReprIDArray[i]
				__statePtr->debugging_prReprID_array.push_back(i);
		}
#endif

#if 1	// 뼈 길이 변화
		float	f = 2.f;
		kMesh::D3DXVERTEX*	bufferVertexArray;
		__vertexBufferPtr->Lock(0, 0, (void**) &bufferVertexArray, 0);
		kMesh::D3DVERTEX*	meshVertexArray;
		__meshPtr->LockVertexBuffer(0, (void**) &meshVertexArray);
		CPointRepr::VERTEX*	subsetVertexArray;
		__meshLoader.meshSubsetPtr()->LockVertexBuffer(0, (void**) &subsetVertexArray);
{
		D3DXVECTOR3	P0;
		getPoint(planeDef0.pointDef, P0);
		D3DXVECTOR3	P1;
#if 1	// PLANE_DEF
		getPoint(planeDef1.pointDef, P1);
#else	// PLANE_DEF/POINT_DEF
		getPoint(pointDef, P1);
#endif	// POINT_DEF
		D3DXVECTOR3	P0P1 = P1 - P0;
		D3DXVECTOR3	nDir = D3DXVec3Dot(&P0P1, &planeDef0.normal) ? planeDef0.normal : -planeDef0.normal;
#if 0	// between
		D3DXVECTOR3	ell_between = D3DXVec3Dot(&P0P1, &nDir) * nDir;
#endif	// between
		for (int n = 0; n < (int) __numPointReprs; ++n) {
#if 0	// between
			if (betweenPointReprIDArray[n]) {
				CPointRepr&	pointRepr = __pointReprPtr[n];
				CPointRepr::VERTEX&	v = pointRepr.v;
				D3DXVECTOR3	ell = v.position - P0;
				D3DXVECTOR3	ell_longitudinal = D3DXVec3Dot(&ell, &nDir) * nDir;
				D3DXVECTOR3	ell_transversal = ell - ell_longitudinal;
				v.position = P0 + (f * ell_longitudinal + ell_transversal);
#ifdef __USE_A_OFFSET__
				for (int k = pointer_to__a_vVertexID_a_offset[n]; k < pointer_to__a_vVertexID_a_offset[n + 1]; ++k)
					subsetVertexArray[pointer_to__a_vVertexID[k]].position = v.position;
#else
				for (std::set<DWORD>::const_iterator it = vVertexID.begin(); it != vVertexID.end(); ++it)
					subsetVertexArray[*it].position = v.position;
#endif
			}
			if (lowerPointReprIDArray[n]) {
				CPointRepr&	pointRepr = __pointReprPtr[n];
				CPointRepr::VERTEX&	v = pointRepr.v;
				D3DXVECTOR3	ell = v.position - P0;
				D3DXVECTOR3	ell_longitudinal = D3DXVec3Dot(&ell, &nDir) * nDir;
				D3DXVECTOR3	ell_transversal = ell - ell_longitudinal;
				// ell_longitudinal 은 betweenFaceIDArray 에 있는 부분과
				// lowerFaceIDArray 에 있는 부분으로 나눈다.
				D3DXVECTOR3	ell_lower = ell_longitudinal - ell_between;
				v.position = P0 + (f * ell_between + ell_lower + ell_transversal);
#ifdef __USE_A_OFFSET__
				for (int k = pointer_to__a_vVertexID_a_offset[n]; k < pointer_to__a_vVertexID_a_offset[n + 1]; ++k)
					subsetVertexArray[pointer_to__a_vVertexID[k]].position = v.position;
#else
				for (std::set<DWORD>::const_iterator it = vVertexID.begin(); it != vVertexID.end(); ++it)
					subsetVertexArray[*it].position = v.position;
#endif
			}
#else	// between/lower
			if (lowerPointReprIDArray[n]) {
#if 1
				D3DXVECTOR3	P = __pointReprPtr[n].v.position;
				D3DXVECTOR3	ell = P - P1;
				D3DXVECTOR3	ell_longitudinal = D3DXVec3Dot(&ell, &nDir) * nDir;
				D3DXVECTOR3	ell_transversal = ell - ell_longitudinal;
				P = P1 + (f * ell_longitudinal + ell_transversal);
				bufferVertexArray[n].p = P;
				meshVertexArray[n].p = P;
#ifdef __USE_A_OFFSET__
				for (int k = pointer_to__a_vVertexID_a_offset[n]; k < pointer_to__a_vVertexID_a_offset[n + 1]; ++k)
					subsetVertexArray[pointer_to__a_vVertexID[k]].position = P;
#else
				for (std::set<DWORD>::const_iterator it = vVertexID.begin(); it != vVertexID.end(); ++it)
					subsetVertexArray[*it].position = P;
#endif
#else
				CPointRepr&	pointRepr = __pointReprPtr[n];
				CPointRepr::VERTEX&	v = pointRepr.v;
				D3DXVECTOR3	ell = v.position - P1;
				D3DXVECTOR3	ell_longitudinal = D3DXVec3Dot(&ell, &nDir) * nDir;
				D3DXVECTOR3	ell_transversal = ell - ell_longitudinal;
				v.position = P1 + (f * ell_longitudinal + ell_transversal);
#ifdef __USE_A_OFFSET__
				for (int k = pointer_to__a_vVertexID_a_offset[n]; k < pointer_to__a_vVertexID_a_offset[n + 1]; ++k)
					subsetVertexArray[pointer_to__a_vVertexID[k]].position = v.position;
#else
				for (std::set<DWORD>::const_iterator it = vVertexID.begin(); it != vVertexID.end(); ++it)
					subsetVertexArray[*it].position = v.position;
#endif
#endif
			}
#endif	// lower
		}
}
		__meshLoader.meshSubsetPtr()->UnlockVertexBuffer();
		__meshPtr->UnlockVertexBuffer();
		__vertexBufferPtr->Unlock();
#endif	// 뼈 길이 변화
	} while (0);
	SAFE_DELETE_ARRAY(betweenPointReprIDArray);
	SAFE_DELETE_ARRAY(lowerPointReprIDArray);
}
#endif	// 뼈의 길이 변경
#if 1	// (목, 손, 발)경계선 결정
{
	__statePtr->debugging_EdgeID_array.clear();
	for (DWORD prID = 0; prID < __numPointReprs; ++prID)
		__pointReprPtr[prID].flag.fixed = false;
	for (DWORD edgeID = 0; edgeID < __numEdges; ++edgeID) {
		const CEdge	edge = __edgePtr[edgeID];
#ifdef __USE_A_OFFSET__
		UINT	num_faces = pointer_to__a_faceID_a_offset[edgeID + 1] - pointer_to__a_faceID_a_offset[edgeID];
#else
		UINT	num_faces = edge.faceID.size();
#endif
		if (num_faces != 2) {
//			cout << "Edge ID = " << edgeID << ": # of faces = " << num_faces << endl;
			if (num_faces == 1) {
//				__statePtr->debugging_EdgeID_array.push_back(edgeID);
				__pointReprPtr[edge.prStartID].flag.fixed = true;
				__pointReprPtr[edge.prEndID].flag.fixed = true;
			}
		}
	}
	cout << "# of abnormal edge(s) = " << __statePtr->debugging_EdgeID_array.size() << endl;
#if 0	// 젖꼭지 제외
	DWORD	nipples[] = {
	// left
	2753, 1672, 1673, 1674, 1675, 1676, 1677, 1678, 1679, 1680, 1681, 1682, 1683, 1684, 1685, 1686, 1687, 1688, 1689, 1690, 1691, 1692, 1693, 1694, 1695, 1696, 1697, 1698, 1699, 1700, 1701, 1702, 1703, 1704, 1705, 1706, 1707, 1708, 1709, 1710, 1711, 1712, 1713, 1714, 1715, 1716, 1717, 1718, 1719, 1720, 1721, 1722, 1723, 1724, 1725, 1726, 1727, 1728, 1729, 1730, 1731, 1732, 1733, 1734, 1735, 1736, 1737, 1738, 1739, 1740, 1741, 1742, 1743, 1744, 1745, 1746, 1747, 1748, 1749, 1750, 1751, 1752, 1753, 1754, 1755, 1756, 1757, 1758, 1759, 1760, 1761, 1762, 1763, 1764, 1765, 1766, 1767, 1768, 1769, 1770, 1771, 1772, 1773, 1774, 1775, 1776, 1777, 1778, 1779, 1780, 1781, 1782, 1783, 1784, 1785, 1786, 1787, 1788, 1789, 1790, 1791, 1792, 1793, 1794, 1795, 1796, 1797, 1798, 1799, 1800, 1801, 1802, 1803, 1804, 1805, 1806, 1807, 1808, 1809, 1810, 1811, 1812, 1813, 1814, 1815, 1816, 1817, 1818, 1819, 1820, 1821, 1822, 1823, 1824, 1825, 1826, 1827, 1828, 1829, 1830, 1831, 1832, 1833, 1834, 1835, 1836, 1837, 1838,
	// right
	4115, 3117, 3118, 3119, 3120, 3121, 3122, 3123, 3124, 3125, 3126, 3127, 3128, 3129, 3130, 3131, 3132, 3133, 3134, 3135, 3136, 3137, 3138, 3139, 3140, 3141, 3142, 3143, 3144, 3145, 3146, 3147, 3148, 3149, 3150, 3151, 3152, 3153, 3154, 3155, 3156, 3157, 3158, 3185, 3186, 3187, 3188, 3189, 3190, 3191, 3192, 3193, 3194, 3195, 3196, 3197, 3198,
	};

//	__statePtr->debugging_prReprID_array.clear();
	for (int k = 0; k < sizeof(nipples) / sizeof(DWORD); ++k) {
		DWORD	prID = nipples[k];
//		__pointReprPtr[prID].flag.fixed = true;
//		__statePtr->debugging_prReprID_array.push_back(prID);
	}
//	cout << "# of fixed point(s) = " << __statePtr->debugging_prReprID_array.size() << endl;
#endif
}
#endif	// (목, 손, 발)경계선 결정


	CalculateTotalVolume();

{
	LPD3DXMESH	pMesh;
	pMesh = __meshPtr;
	LPDIRECT3DVERTEXBUFFER9	pVB;
	pMesh->GetVertexBuffer(&pVB);
	D3DVERTEX*	pVertices;
	pVB->Lock(0, 0, (void**) &pVertices, 0);
	D3DXComputeBoundingSphere(&pVertices[0].p, pMesh->GetNumVertices(), sizeof(D3DVERTEX), &__cm, &__radius);
#if 0	// Confirm!
	D3DXVECTOR3	c(0, 0, 0);
	for (int i = 0; i < (int) pMesh->GetNumVertices(); ++i) {
		c += pVertices[i].p;
	}
	c /= (FLOAT) pMesh->GetNumVertices();
	float	r = FLT_MIN;
	for (int i = 0; i < (int) pMesh->GetNumVertices(); ++i) {
		float	a = D3DXVec3Length(&D3DXVECTOR3(c - pVertices[i].p));
		if (a > r)
			r = a;
	}
	cout << "__cm: [" << __cm.x << ", " << __cm.y << ", " << __cm.z << "]" << endl;
	cout << "c: [" << c.x << ", " << c.y << ", " << c.z << "]" << endl;
	cout << "__radius = " << __radius << endl;
	cout << "r = " << r << endl;
#endif

	// Bounding Box; (left.x, lower.y, near,z), (right.x, upper.y, far.z)
	D3DXComputeBoundingBox(&pVertices[0].p, pMesh->GetNumVertices(), sizeof(D3DVERTEX), &__left_lower_near, &__right_upper_far);
	__pivotCenter = .5f * (__left_lower_near + __right_upper_far);
#if 0	// confirm
	D3DXVECTOR3	lln, ruf;
	lln.x = FLT_MAX; lln.y = FLT_MAX; lln.z = FLT_MAX;
	ruf.x = FLT_MIN; ruf.y = FLT_MIN; ruf.z = FLT_MIN;
	for (int i = 0; i < (int) pMesh->GetNumVertices(); ++i) {
		D3DXVECTOR3&	v = pVertices[i].p;
		if (v.x < lln.x)
			lln.x = v.x;
		if (v.y < lln.y)
			lln.y = v.y;
		if (v.z < lln.z)
			lln.z = v.z;
		if (v.x > ruf.x)
			ruf.x = v.x;
		if (v.y > ruf.y)
			ruf.y = v.y;
		if (v.z > ruf.z)
			ruf.z = v.z;
	}
	cout << "__left_lower_near: [" << __left_lower_near.x << ", " << __left_lower_near.y << ", " << __left_lower_near.z << "]" << endl;
	cout << "lln: [" << lln.x << ", " << lln.y << ", " << lln.z << "]" << endl;
	cout << "__right_upper_far: [" << __right_upper_far.x << ", " << __right_upper_far.y << ", " << __right_upper_far.z << "]" << endl;
	cout << "ruf: [" << ruf.x << ", " << ruf.y << ", " << ruf.z << "]" << endl;
#endif
#if 1	// 마지막 주문; 8. 복셀까지 고려해서 경계 설정
if (__meshLoader.GetNumVoxels()) {
	cout << "voxel..." << endl;
	D3DXVECTOR3&	lln = __left_lower_near;
	D3DXVECTOR3&	ruf = __right_upper_far;

	kMeshLoader::VOXEL_VERTEX*	voxelVertexArray;
	__meshLoader.voxelVertexBufferPtr()->Lock(0, 0, (void**) &voxelVertexArray, 0);
	for (int index = 0; index < (int) __meshLoader.GetNumVoxels(); ++index) {
		// 하나의 복셀이 실제로는 8개의 점으로 이루어진 정육면체를 만든다(kMeshLoader::__setVoxelBuffer)
		// 그 중에서도
		// 4번째(index = 3)번째 Vertex 가 left-lower-near 를 결정하고,
		const kMeshLoader::VOXEL_VERTEX&	voxel3 = voxelVertexArray[index * 8 + 3];
		const D3DXVECTOR3&	p3 = voxel3.p;
		if (p3.x < lln.x)
			lln.x = p3.x;
		if (p3.y < lln.y)
			lln.y = p3.y;
		if (p3.z < lln.z)
			lln.z = p3.z;
		// 6번째(index = 5)번째 Vertex 가 right-upper-far 를 결정한다.
		const kMeshLoader::VOXEL_VERTEX&	voxel5 = voxelVertexArray[index * 8 + 5];
		const D3DXVECTOR3&	p5 = voxel5.p;
		if (p5.x > ruf.x)
			ruf.x = p5.x;
		if (p5.y > ruf.y)
			ruf.y = p5.y;
		if (p5.z > ruf.z)
			ruf.z = p5.z;
	}
	__meshLoader.voxelVertexBufferPtr()->Unlock();
	__pivotCenter = .5f * (__left_lower_near + __right_upper_far);
}
#endif	// 마지막 주문; 8. 복셀까지 고려해서 경계 설정
#if 1	// 마지막 주문; 14. 키
	cout << "키: " << (__right_upper_far.y - __left_lower_near.y) / 10.f << endl;
#endif	// 마지막 주문; 14. 키

	pVB->Unlock();
#if 1
#else
	SAFE_RELEASE(pMesh);
#endif
	SAFE_RELEASE(pVB);

	// 반지름이 r 인 구 안에 n 개의 점들이 균일하게 분포되었다고 했을 때,
	// 각 점이 차지하는 부피는 3/4 \pi r^3 / n
	// 이 부피를 길이가 a 인 정육면체의 부피라고 했을 때 a 값은
	// a^3 = 3/4 \pi r^3 / n 으로부터
	// a = r * (3/4 \pi / n)^{1/3}
	//   = r * e^{1/3 ln(3/4 \pi / n)}
//	__a = (FLOAT) exp(log(4. * D3DX_PI / 3. / GetNumPointReprs()) / 3.) * r;
	// 실제부피(V0)를 N 개의 공(또는 정육면체)로 채웠을 때,
	// 공(또는 정육면체)의 반지름, r(또는 한 변의 길이, a)는?
	//	공: 4π/3 r^3 = V0 / N	r = e^{1/3 ln((V0 / N) / (4π/3))}
	//	정육면체: a^3 = V0 / N	a = e^{1/3 ln(V0/N)}
	cout << "__a = " << __a << endl;
	__a = (FLOAT) exp(log(m__fTotalVolume0 / GetNumPointReprs() / (4 * D3DX_PI / 3)) / 3.);
	cout << "__a = " << __a << endl;
	__a = (FLOAT) exp(log(m__fTotalVolume0 / GetNumPointReprs() / (4. * D3DX_PI / 3.)) / 3.);
	cout << "__a = " << __a << endl;
	__a = (FLOAT) exp(log(m__fTotalVolume0 / GetNumPointReprs()) / 3.);
	cout << "__a = " << __a << endl;
}
#if 1	// SHIFT
	SetShift(SHIFT_ALIGN_CENTER);
#endif	// SHIFT


	(void) fprintf(stdout, "Call setIsometricLines three times... (%f)\n", __timer.GetElapsedTime());
#if 1	// 마지막 주문; 5. 측정선 선명하게...
	__shift_amount_along_normal = kState::SHIFT_FACTOR_ALONG_NORMAL * __a;
#else
	__shiftAmount = ShiftFactor * __a;
#endif	// 마지막 주문; 5. 측정선 선명하게...
	setIsometricLines(ISOMETRIC_XY, D3DXVECTOR3(0, 0, 1), MAX_SLICE / 2, __z_isometricVertexBufferPtr, __z_isometricLines);
	setIsometricLines(ISOMETRIC_YZ, D3DXVECTOR3(1, 0, 0), MAX_SLICE / 2, __x_isometricVertexBufferPtr, __x_isometricLines);
	setIsometricLines(ISOMETRIC_ZX, D3DXVECTOR3(0, 1, 0), MAX_SLICE / 2, __y_isometricVertexBufferPtr, __y_isometricLines);
	(void) fprintf(stdout, "...Done (%f)\n", __timer.GetElapsedTime());

#ifdef __KEKO__
	__select.setAll(this, devicePtr, TEXT("./Resources/select.fx"));
	__showNormal.setAll(__meshPtr, devicePtr, TEXT("./Resources/showNormal.fx"));
#else	// __KEKO__/!__KEKO__
	__select.setAll(this, devicePtr, TEXT("../include/mesh/select.fx"));
	__showNormal.setAll(__meshPtr, devicePtr, TEXT("../include/mesh/showNormal.fx"));
#endif	// !__KEKO__
	__tapeMeasure.setDeviceAndMesh(devicePtr, this);

	__devicePtr->CreateVertexBuffer(
		2 * sizeof(D3DXVECTOR3),
		D3DUSAGE_WRITEONLY,
		0,
		D3DPOOL_MANAGED,
		&__intersectionVertexBufferPtr,
		NULL
	);

	(void) fprintf(stdout, "Setting defined... (%f)\n", __timer.GetElapsedTime());
#if 1	// 2D 와의 통신을 위해 필요한 정의나 함수의 구현
#ifdef STANDARD_MODEL
if (
	index_to_standard >= 0 &&
	index_to_standard < sizeof(measureTypeDefList) / sizeof(MEASURE_TYPE_DEF_LIST)
) {
#if 1	// 길이(둘레, 넓이, 길이) 측정;
	// CTRL + '8' 키를 사용해 필요한 정보(MEASURE_ITEM_DEF) 출력:
	// 둘레: 머리, 목, 가슴(줄자), 허리, 엉덩이(줄자), 왼쪽 허벅지, 오른쪽 허벅지, 왼쪽 팔뚝, 오른쪽 팔뚝
	// 길이: 키, 앉은 키, 어깨 넓이, 왼쪽 다리(X), 오른쪽 다리(X), 왼쪽 팔(X), 오른쪽 팔(X),
	//	왼쪽 팔 안쪽, 왼쪽 팔 바깥쪽, 오른쪽 팔 안쪽, 오른쪽 팔 바깥쪽,
	//	왼쪽 다리 안쪽, 왼쪽 다리 바깥쪽, 오른쪽 다리 안쪽, 오른쪽 다리 바깥쪽
#if 1	// defined
{
	const MEASURE_TYPE_DEF_LIST&	ETRI_typeDefList = measureTypeDefList[index_to_standard];
	cout << "# of MEASURE_TYPE_DEF_LIST = " << ETRI_typeDefList.n << endl;
	for (int k = 0; k < ETRI_typeDefList.n; ++k) {
		const MEASURE_TYPE_DEF&	typeDef = ETRI_typeDefList.ptr[k];
//		cout << "MEASURE_TYPE_DEF[" << k << "]:" << endl;
//		cout << "\t# of MEASURE_DEF = " << typeDef.defList.n << endl;

		TAPE_MEASURE_TYPE_T	type = typeDef.type;
		MEASURE_PTR_HASH::iterator	it;
		it = __measure_ptr_dic.find(type);
		if (it != __measure_ptr_dic.end())
			__measure_ptr_dic.erase(it);
#if 1	// MEASURE 설정
		std::vector<MEASURE_ITEM*>	m_item_ptr_list;
		for (int i = 0; i < typeDef.defList.n; ++i) {
			MEASURE_ITEM*		m_item_ptr = new MEASURE_ITEM(__devicePtr, MAX_SEGMENT);
			MEASURE_ITEM_DEF&	itemDef = *((MEASURE_ITEM_DEF*) m_item_ptr);
#if 1	// MEASURE_ITEM_DEF 설정
			const MEASURE_DEF&	mDef = typeDef.defList.ptr[i];
			MEASURE_TYPE_T		type = mDef.__type;
			switch (type) {
			case MEASURE_TYPE__TWO_POINT_DISTANCE:
//				cout << "MEASURE_TYPE_T: MEASURE_TYPE__TWO_POINT_DISTANCE" << endl;
{
	const MEASURE_DEF__TWO_POINT_DISTANCE&	def = *((MEASURE_DEF__TWO_POINT_DISTANCE*) mDef.__ptr);
	itemDef.__type = MEASURE_TYPE__TWO_POINT_DISTANCE;
	itemDef.__ptr = new MEASURE_ITEM_PTR_DEF__TWO_POINT_DISTANCE(def.__twoPointDef);
}
				break;
			case MEASURE_TYPE__POINT_PLANE_DISTANCE:
//				cout << "MEASURE_TYPE_T: MEASURE_TYPE__POINT_PLANE_DISTANCE" << endl;
{
	const MEASURE_DEF__POINT_PLANE_DISTANCE&	def = *((MEASURE_DEF__POINT_PLANE_DISTANCE*) mDef.__ptr);
	itemDef.__type = MEASURE_TYPE__POINT_PLANE_DISTANCE;
#if 1	// BONE_DEF
	itemDef.__ptr = new MEASURE_ITEM_PTR_DEF__POINT_PLANE_DISTANCE(def);
#else	// BONE_DEF/!BONE_DEF
	itemDef.__ptr = new MEASURE_ITEM_PTR_DEF__POINT_PLANE_DISTANCE(def.__pointPlaneDef);
#endif	// !BONE_DEF
}
				break;
			case MEASURE_TYPE__TWO_PLANE_DISTANCE:
//				cout << "MEASURE_TYPE_T: MEASURE_TYPE__TWO_PLANE_DISTANCE" << endl;
{
	const MEASURE_DEF__TWO_PLANE_DISTANCE&	def = *((MEASURE_DEF__TWO_PLANE_DISTANCE*) mDef.__ptr);
	itemDef.__type = MEASURE_TYPE__TWO_PLANE_DISTANCE;
#if 1	// BONE_DEF
	itemDef.__ptr = new MEASURE_ITEM_PTR_DEF__TWO_PLANE_DISTANCE(def);
#else	// BONE_DEF/!BONE_DEF
	itemDef.__ptr = new MEASURE_ITEM_PTR_DEF__TWO_PLANE_DISTANCE(def.__twoPlaneDef);
#endif	// !BONE_DEF
}
				break;
			case MEASURE_TYPE__GIRTH:
//				cout << "MEASURE_TYPE_T: MEASURE_TYPE__GIRTH" << endl;
{
	const MEASURE_DEF__GIRTH&	def = *((MEASURE_DEF__GIRTH*) mDef.__ptr);
	itemDef.__type = MEASURE_TYPE__GIRTH;
#if 1	// BONE_DEF
	itemDef.__ptr = new MEASURE_ITEM_PTR_DEF__GIRTH(def.__mode, def);
#else	// BONE_DEF/!BONE_DEF
	itemDef.__ptr = new MEASURE_ITEM_PTR_DEF__GIRTH(def.__mode, def.__planeDef);
#endif	// !BONE_DEF
}
				break;
			case MEASURE_TYPE__TWO_POINT_GIRTH:
//				cout << "MEASURE_TYPE_T: MEASURE_TYPE__TWO_POINT_GIRTH" << endl;
{
	const MEASURE_DEF__TWO_POINT_GIRTH&	def = *((MEASURE_DEF__TWO_POINT_GIRTH*) mDef.__ptr);
	itemDef.__type = MEASURE_TYPE__TWO_POINT_GIRTH;
	itemDef.__ptr = new MEASURE_ITEM_PTR_DEF__TWO_POINT_GIRTH(def.__mode, def.__twoPointDef);
}
				break;
			default:
				cerr << "MEASURE_TYPE_T: " << type << ": Unknown Type" << endl;
				break;
			}
#endif	// MEASURE_ITEM_DEF 설정
			m_item_ptr_list.push_back(m_item_ptr);
		}
		MEASURE*	measure_ptr = new MEASURE(__devicePtr, m_item_ptr_list, typeDef.tag);
#endif	// MEASURE 설정
		__measure_ptr_dic[type] = measure_ptr;
	}
#if 0
do {	// 머리
	MEASURE_PTR_HASH::iterator	it;
	it = __measure_ptr_dic.find(TAPE_MEASURE_HEAD);
	if (it != __measure_ptr_dic.end())
		__measure_ptr_dic.erase(it);

#if 0	// 더 이상 사용되지 않는다.
	LENGTH_DEF	lenthDef = {
		{86366, 0.430915f, 0.142952f},
		D3DXVECTOR3(0.000000f, 1.000000f, 0.000000f)
	};
#endif
	std::vector<MEASURE_ITEM*>	m_item_ptr_list;
	MEASURE_ITEM*	m_item_ptr = new MEASURE_ITEM(__devicePtr, MAX_SEGMENT);//, lenthDef);
	PLANE_DEF	planeDef;
	POINT_DEF	pointDef = {15743, 0.221824f, 0.408594f};
	planeDef.pointDef = pointDef;
	planeDef.normal = D3DXVECTOR3(0, 1, 0);
	MEASURE_ITEM_DEF&	def = *((MEASURE_ITEM_DEF*) m_item_ptr);
	def.__type = MEASURE_TYPE__GIRTH;
	def.__ptr = new MEASURE_ITEM_PTR_DEF__GIRTH(TAPE_MEASURE_MODE_CONTOUR, planeDef);
	m_item_ptr_list.push_back(m_item_ptr);
	MEASURE*	measure_ptr = new MEASURE(__devicePtr, m_item_ptr_list);
	__measure_ptr_dic[TAPE_MEASURE_HEAD] = measure_ptr;
} while (0);
#endif
}
#else	// defined/!defined
#if 1	// 둘레 정의
do {	// 머리
	MEASURE_PTR_HASH::iterator	it;
	it = __measure_ptr_dic.find(TAPE_MEASURE_HEAD);
	if (it != __measure_ptr_dic.end())
		__measure_ptr_dic.erase(it);

#if 1	// 더 이상 사용되지 않는다.
	LENGTH_DEF	lenthDef = {
		{86366, 0.430915f, 0.142952f},
		D3DXVECTOR3(0.000000f, 1.000000f, 0.000000f)
	};
#endif
	std::vector<MEASURE_ITEM*>	m_item_ptr_list;
	MEASURE_ITEM*	m_item_ptr = new MEASURE_ITEM(__devicePtr, MAX_SEGMENT, lenthDef);
	PLANE_DEF	planeDef;
	POINT_DEF	pointDef = {15743, 0.221824f, 0.408594f};
	planeDef.pointDef = pointDef;
	planeDef.normal = D3DXVECTOR3(0, 1, 0);
	MEASURE_ITEM_DEF&	def = *((MEASURE_ITEM_DEF*) m_item_ptr);
	def.__type = MEASURE_TYPE__GIRTH;
	def.__ptr = new MEASURE_ITEM_PTR_DEF__GIRTH(TAPE_MEASURE_MODE_CONTOUR, planeDef);
	m_item_ptr_list.push_back(m_item_ptr);
	MEASURE*	measure_ptr = new MEASURE(__devicePtr, m_item_ptr_list);
	__measure_ptr_dic[TAPE_MEASURE_HEAD] = measure_ptr;
} while (0);
do {	// 목
	MEASURE_PTR_HASH::iterator	it;
	it = __measure_ptr_dic.find(TAPE_MEASURE_NECK);
	if (it != __measure_ptr_dic.end())
		__measure_ptr_dic.erase(it);

#if 1	// 더 이상 사용되지 않는다.
	LENGTH_DEF	lenthDef = {
		{86366, 0.430915f, 0.142952f},
		D3DXVECTOR3(0.000000f, 1.000000f, 0.000000f)
	};
#endif
	std::vector<MEASURE_ITEM*>	m_item_ptr_list;
	MEASURE_ITEM*	m_item_ptr = new MEASURE_ITEM(__devicePtr, MAX_SEGMENT, lenthDef);
	PLANE_DEF	planeDef;
	POINT_DEF	pointDef = {115150, 0.242606f, 0.727244f};
	planeDef.pointDef = pointDef;
	planeDef.normal = D3DXVECTOR3(0, 1, 0);
	MEASURE_ITEM_DEF&	def = *((MEASURE_ITEM_DEF*) m_item_ptr);
	def.__type = MEASURE_TYPE__GIRTH;
	def.__ptr = new MEASURE_ITEM_PTR_DEF__GIRTH(TAPE_MEASURE_MODE_CONTOUR, planeDef);
	m_item_ptr_list.push_back(m_item_ptr);
	MEASURE*	measure_ptr = new MEASURE(__devicePtr, m_item_ptr_list);
	__measure_ptr_dic[TAPE_MEASURE_NECK] = measure_ptr;
} while (0);
do {	// 가슴
	MEASURE_PTR_HASH::iterator	it;
	it = __measure_ptr_dic.find(TAPE_MEASURE_BREAST);
	if (it != __measure_ptr_dic.end())
		__measure_ptr_dic.erase(it);

#if 1	// 더 이상 사용되지 않는다.
	LENGTH_DEF	lenthDef = {
		{86366, 0.430915f, 0.142952f},
		D3DXVECTOR3(0.000000f, 1.000000f, 0.000000f)
	};
#endif
	std::vector<MEASURE_ITEM*>	m_item_ptr_list;
	MEASURE_ITEM*	m_item_ptr = new MEASURE_ITEM(__devicePtr, MAX_SEGMENT, lenthDef);
	PLANE_DEF	planeDef;
	POINT_DEF	pointDef = {57914, 0.406461f, 0.384281f};
	planeDef.pointDef = pointDef;
	planeDef.normal = D3DXVECTOR3(0, 1, 0);
	MEASURE_ITEM_DEF&	def = *((MEASURE_ITEM_DEF*) m_item_ptr);
	def.__type = MEASURE_TYPE__GIRTH;
	def.__ptr = new MEASURE_ITEM_PTR_DEF__GIRTH(TAPE_MEASURE_MODE_WRAP, planeDef);
	m_item_ptr_list.push_back(m_item_ptr);
	MEASURE*	measure_ptr = new MEASURE(__devicePtr, m_item_ptr_list);
	__measure_ptr_dic[TAPE_MEASURE_BREAST] = measure_ptr;
} while (0);
do {	// 허리
	MEASURE_PTR_HASH::iterator	it;
	it = __measure_ptr_dic.find(TAPE_MEASURE_WAIST);
	if (it != __measure_ptr_dic.end())
		__measure_ptr_dic.erase(it);

#if 1	// 더 이상 사용되지 않는다.
	LENGTH_DEF	lenthDef = {
		{86366, 0.430915f, 0.142952f},
		D3DXVECTOR3(0.000000f, 1.000000f, 0.000000f)
	};
#endif
	std::vector<MEASURE_ITEM*>	m_item_ptr_list;
	MEASURE_ITEM*	m_item_ptr = new MEASURE_ITEM(__devicePtr, MAX_SEGMENT, lenthDef);
	PLANE_DEF	planeDef;
	POINT_DEF	pointDef = {61429, 0.279595f, 0.336564f};
	planeDef.pointDef = pointDef;
	planeDef.normal = D3DXVECTOR3(0, 1, 0);
	MEASURE_ITEM_DEF&	def = *((MEASURE_ITEM_DEF*) m_item_ptr);
	def.__type = MEASURE_TYPE__GIRTH;
	def.__ptr = new MEASURE_ITEM_PTR_DEF__GIRTH(TAPE_MEASURE_MODE_CONTOUR, planeDef);
	m_item_ptr_list.push_back(m_item_ptr);
	MEASURE*	measure_ptr = new MEASURE(__devicePtr, m_item_ptr_list);
	__measure_ptr_dic[TAPE_MEASURE_WAIST] = measure_ptr;
} while (0);
do {	// 엉덩이
	MEASURE_PTR_HASH::iterator	it;
	it = __measure_ptr_dic.find(TAPE_MEASURE_HIP);
	if (it != __measure_ptr_dic.end())
		__measure_ptr_dic.erase(it);

#if 1	// 더 이상 사용되지 않는다.
	LENGTH_DEF	lenthDef = {
		{86366, 0.430915f, 0.142952f},
		D3DXVECTOR3(0.000000f, 1.000000f, 0.000000f)
	};
#endif
	std::vector<MEASURE_ITEM*>	m_item_ptr_list;
	MEASURE_ITEM*	m_item_ptr = new MEASURE_ITEM(__devicePtr, MAX_SEGMENT, lenthDef);
	PLANE_DEF	planeDef;
	POINT_DEF	pointDef = {65025, 0.815195f, 0.053784f};
	planeDef.pointDef = pointDef;
	planeDef.normal = D3DXVECTOR3(0, 1, 0);
	MEASURE_ITEM_DEF&	def = *((MEASURE_ITEM_DEF*) m_item_ptr);
	def.__type = MEASURE_TYPE__GIRTH;
	def.__ptr = new MEASURE_ITEM_PTR_DEF__GIRTH(TAPE_MEASURE_MODE_WRAP, planeDef);
	m_item_ptr_list.push_back(m_item_ptr);
	MEASURE*	measure_ptr = new MEASURE(__devicePtr, m_item_ptr_list);
	__measure_ptr_dic[TAPE_MEASURE_HIP] = measure_ptr;
} while (0);
do {	// 왼쪽 허벅지
	MEASURE_PTR_HASH::iterator	it;
	it = __measure_ptr_dic.find(TAPE_MEASURE_LEFT_THIGH);
	if (it != __measure_ptr_dic.end())
		__measure_ptr_dic.erase(it);

#if 1	// 더 이상 사용되지 않는다.
	LENGTH_DEF	lenthDef = {
		{86366, 0.430915f, 0.142952f},
		D3DXVECTOR3(0.000000f, 1.000000f, 0.000000f)
	};
#endif
	std::vector<MEASURE_ITEM*>	m_item_ptr_list;
	MEASURE_ITEM*	m_item_ptr = new MEASURE_ITEM(__devicePtr, MAX_SEGMENT, lenthDef);
	PLANE_DEF	planeDef;
	POINT_DEF	pointDef = {63898, 0.366757f, 0.103650f};
	planeDef.pointDef = pointDef;
	planeDef.normal = D3DXVECTOR3(0, 1, 0);
	MEASURE_ITEM_DEF&	def = *((MEASURE_ITEM_DEF*) m_item_ptr);
	def.__type = MEASURE_TYPE__GIRTH;
	def.__ptr = new MEASURE_ITEM_PTR_DEF__GIRTH(TAPE_MEASURE_MODE_CONTOUR, planeDef);
	m_item_ptr_list.push_back(m_item_ptr);
	MEASURE*	measure_ptr = new MEASURE(__devicePtr, m_item_ptr_list);
	__measure_ptr_dic[TAPE_MEASURE_LEFT_THIGH] = measure_ptr;
} while (0);
do {	// 오른쪽 허벅지
	MEASURE_PTR_HASH::iterator	it;
	it = __measure_ptr_dic.find(TAPE_MEASURE_RIGHT_THIGH);
	if (it != __measure_ptr_dic.end())
		__measure_ptr_dic.erase(it);

#if 1	// 더 이상 사용되지 않는다.
	LENGTH_DEF	lenthDef = {
		{86366, 0.430915f, 0.142952f},
		D3DXVECTOR3(0.000000f, 1.000000f, 0.000000f)
	};
#endif
	std::vector<MEASURE_ITEM*>	m_item_ptr_list;
	MEASURE_ITEM*	m_item_ptr = new MEASURE_ITEM(__devicePtr, MAX_SEGMENT, lenthDef);
	PLANE_DEF	planeDef;
	POINT_DEF	pointDef = {65710, 0.526200f, 0.101572f};
	planeDef.pointDef = pointDef;
	planeDef.normal = D3DXVECTOR3(0, 1, 0);
	MEASURE_ITEM_DEF&	def = *((MEASURE_ITEM_DEF*) m_item_ptr);
	def.__type = MEASURE_TYPE__GIRTH;
	def.__ptr = new MEASURE_ITEM_PTR_DEF__GIRTH(TAPE_MEASURE_MODE_CONTOUR, planeDef);
	m_item_ptr_list.push_back(m_item_ptr);
	MEASURE*	measure_ptr = new MEASURE(__devicePtr, m_item_ptr_list);
	__measure_ptr_dic[TAPE_MEASURE_RIGHT_THIGH] = measure_ptr;
} while (0);
do {	// 왼쪽 팔뚝
	MEASURE_PTR_HASH::iterator	it;
	it = __measure_ptr_dic.find(TAPE_MEASURE_LEFT_FOREARM);
	if (it != __measure_ptr_dic.end())
		__measure_ptr_dic.erase(it);

#if 1	// 더 이상 사용되지 않는다.
	LENGTH_DEF	lenthDef = {
		{86366, 0.430915f, 0.142952f},
		D3DXVECTOR3(0.000000f, 1.000000f, 0.000000f)
	};
#endif
	std::vector<MEASURE_ITEM*>	m_item_ptr_list;
	MEASURE_ITEM*	m_item_ptr = new MEASURE_ITEM(__devicePtr, MAX_SEGMENT, lenthDef);
	PLANE_DEF	planeDef;
	POINT_DEF	pointDef = {57248, 0.314191f, 0.006411f};
	planeDef.pointDef = pointDef;
	planeDef.normal = D3DXVECTOR3(-0.956910f, 0.279197f, -0.079870f);
	MEASURE_ITEM_DEF&	def = *((MEASURE_ITEM_DEF*) m_item_ptr);
	def.__type = MEASURE_TYPE__GIRTH;
	def.__ptr = new MEASURE_ITEM_PTR_DEF__GIRTH(TAPE_MEASURE_MODE_CONTOUR, planeDef);
	m_item_ptr_list.push_back(m_item_ptr);
	MEASURE*	measure_ptr = new MEASURE(__devicePtr, m_item_ptr_list);
	__measure_ptr_dic[TAPE_MEASURE_LEFT_FOREARM] = measure_ptr;
} while (0);
do {	// 오른쪽 팔뚝
	MEASURE_PTR_HASH::iterator	it;
	it = __measure_ptr_dic.find(TAPE_MEASURE_RIGHT_FOREARM);
	if (it != __measure_ptr_dic.end())
		__measure_ptr_dic.erase(it);

#if 1	// 더 이상 사용되지 않는다.
	LENGTH_DEF	lenthDef = {
		{86366, 0.430915f, 0.142952f},
		D3DXVECTOR3(0.000000f, 1.000000f, 0.000000f)
	};
#endif
	std::vector<MEASURE_ITEM*>	m_item_ptr_list;
	MEASURE_ITEM*	m_item_ptr = new MEASURE_ITEM(__devicePtr, MAX_SEGMENT, lenthDef);
	PLANE_DEF	planeDef;
	POINT_DEF	pointDef = {60035, 0.317288f, 0.417311f};
	planeDef.pointDef = pointDef;
	planeDef.normal = D3DXVECTOR3(-0.971231f, -0.143522f, -0.190048f);
	MEASURE_ITEM_DEF&	def = *((MEASURE_ITEM_DEF*) m_item_ptr);
	def.__type = MEASURE_TYPE__GIRTH;
	def.__ptr = new MEASURE_ITEM_PTR_DEF__GIRTH(TAPE_MEASURE_MODE_CONTOUR, planeDef);
	m_item_ptr_list.push_back(m_item_ptr);
	MEASURE*	measure_ptr = new MEASURE(__devicePtr, m_item_ptr_list);
	__measure_ptr_dic[TAPE_MEASURE_RIGHT_FOREARM] = measure_ptr;
} while (0);
#endif	// 둘레 정의

#if 1	// 길이 정의
#if 1
do {	// 키
	MEASURE_PTR_HASH::iterator	it;
	it = __measure_ptr_dic.find(TAPE_MEASURE_HEIGHT);
	if (it != __measure_ptr_dic.end())
		__measure_ptr_dic.erase(it);

	LENGTH_DEF	def_length[] = {
		{	// 정수리
			{20790, 0.025643f, 0.962431f},
			D3DXVECTOR3(0, 0, 0)
		},
		{	// 골반
			{62710, 0.448102f, 0.258171f},
			D3DXVECTOR3(0, 1, 0)
		},
		{	// 골반
			{65847, 0.015407f, 0.824872f},
			D3DXVECTOR3(0.086850f, 0.996197f, 0.007047f)
		},
		{	// 오른쪽 무릎
			{72432, 0.102591f, 0.257141f},
			D3DXVECTOR3(0.086850f, 0.996197f, 0.007047f)
		},
		{	// 오른쪽 무릎
			{72432, 0.102591f, 0.257141f},
			D3DXVECTOR3(0.086850f, 0.996197f, 0.007047f)
		},
		{	// 오른쪽 발뒤꿈치
			{73637, 0.016866f, 0.185059f},
			D3DXVECTOR3(0, 0, 0)
		},
	};
	std::vector<MEASURE_ITEM*>	m_item_ptr_list;
	int	nElement = sizeof(def_length) / sizeof(LENGTH_DEF);
	for (int i = 0; i < nElement; ++i) {
		MEASURE_ITEM*	m_item_ptr = new MEASURE_ITEM(__devicePtr, MAX_SEGMENT, def_length[i]);
		m_item_ptr_list.push_back(m_item_ptr);
	}
	MEASURE*	measure_ptr = new MEASURE(__devicePtr, m_item_ptr_list);
	__measure_ptr_dic[TAPE_MEASURE_HEIGHT] = measure_ptr;
} while (0);
do {	// 앉은 키
	MEASURE_PTR_HASH::iterator	it;
	it = __measure_ptr_dic.find(TAPE_MEASURE_SEATED_HEIGHT);
	if (it != __measure_ptr_dic.end())
		__measure_ptr_dic.erase(it);

	LENGTH_DEF	def_length[] = {
		{	// 정수리
			{20790, 0.025643f, 0.962431f},
			D3DXVECTOR3(0, 0, 0)
		},
		{	// 골반
			{62710, 0.448102f, 0.258171f},
			D3DXVECTOR3(0, 1, 0)
		},
	};
	std::vector<MEASURE_ITEM*>	m_item_ptr_list;
	int	nElement = sizeof(def_length) / sizeof(LENGTH_DEF);
	for (int i = 0; i < nElement; ++i) {
		MEASURE_ITEM*	m_item_ptr = new MEASURE_ITEM(__devicePtr, MAX_SEGMENT, def_length[i]);
		m_item_ptr_list.push_back(m_item_ptr);
	}
	MEASURE*	measure_ptr = new MEASURE(__devicePtr, m_item_ptr_list);
	__measure_ptr_dic[TAPE_MEASURE_SEATED_HEIGHT] = measure_ptr;
} while (0);
#endif
do {	// 어깨 넓이
	MEASURE_PTR_HASH::iterator	it;
	it = __measure_ptr_dic.find(TAPE_MEASURE_SHOULDER_WIDTH);
	if (it != __measure_ptr_dic.end())
		__measure_ptr_dic.erase(it);

#if 1	// 더 이상 사용되지 않는다.
	LENGTH_DEF	lenthDef = {
		{86366, 0.430915f, 0.142952f},
		D3DXVECTOR3(0.000000f, 1.000000f, 0.000000f)
	};
#endif
	std::vector<MEASURE_ITEM*>	m_item_ptr_list;
	MEASURE_ITEM*	m_item_ptr = new MEASURE_ITEM(__devicePtr, MAX_SEGMENT, lenthDef);
	POINT_DEF	point0Def = {56509, 0.018941f, 0.184516f};
	POINT_DEF	point1Def = {59286, 0.547807f, 0.072270f};
	MEASURE_ITEM_DEF&	def = *((MEASURE_ITEM_DEF*) m_item_ptr);
	def.__type = MEASURE_TYPE__TWO_POINT_GIRTH;
	def.__ptr = new MEASURE_ITEM_PTR_DEF__TWO_POINT_GIRTH(TAPE_MEASURE_MODE_WRAP, point0Def, point1Def);
	m_item_ptr_list.push_back(m_item_ptr);
	MEASURE*	measure_ptr = new MEASURE(__devicePtr, m_item_ptr_list);
	__measure_ptr_dic[TAPE_MEASURE_SHOULDER_WIDTH] = measure_ptr;
} while (0);
#if 1	// 팔, 다리를 안쪽,바깥쪽으로 분리; 부활
do {	// 왼쪽 다리
	MEASURE_PTR_HASH::iterator	it;
	it = __measure_ptr_dic.find(TAPE_MEASURE_LEFT_LEG);
	if (it != __measure_ptr_dic.end())
		__measure_ptr_dic.erase(it);

	LENGTH_DEF	def_length[] = {
		{	// 허벅지 윗부분
			{63780, 0.277089f, 0.326401f},
			D3DXVECTOR3(-0.025932f, 0.984654f, -0.172584f)
		},
		{	// 무릎
			{71314, 0.424358f, 0.008220f},
			D3DXVECTOR3(-0.025932f, 0.984654f, -0.172584f)
		},
		{	// 무릎
			{71314, 0.424358f, 0.008220f},
			D3DXVECTOR3(-0.025932f, 0.984654f, -0.172584f)
		},
		{	// 발뒤꿈치
			{74634, 0.960823f, 0.020719f},
			D3DXVECTOR3(0, 0, 0)
		},
	};
	std::vector<MEASURE_ITEM*>	m_item_ptr_list;
	int	nElement = sizeof(def_length) / sizeof(LENGTH_DEF);
	for (int i = 0; i < nElement; ++i) {
		MEASURE_ITEM*	m_item_ptr = new MEASURE_ITEM(__devicePtr, MAX_SEGMENT, def_length[i]);
		m_item_ptr_list.push_back(m_item_ptr);
	}
	MEASURE*	measure_ptr = new MEASURE(__devicePtr, m_item_ptr_list);
	__measure_ptr_dic[TAPE_MEASURE_LEFT_LEG] = measure_ptr;
} while (0);
do {	// 오른쪽 다리
	MEASURE_PTR_HASH::iterator	it;
	it = __measure_ptr_dic.find(TAPE_MEASURE_RIGHT_LEG);
	if (it != __measure_ptr_dic.end())
		__measure_ptr_dic.erase(it);

	LENGTH_DEF	def_length[] = {
		{	// 허벅지 윗부분
			{66488, 0.188731f, 0.313384f},
			D3DXVECTOR3(-0.025932f, 0.984654f, -0.172584f)
		},
		{	// 무릎
			{72746, 0.530067f, 0.334535f},
			D3DXVECTOR3(-0.025932f, 0.984654f, -0.172584f)
		},
		{	// 무릎
			{72746, 0.530067f, 0.334535f},
			D3DXVECTOR3(-0.025932f, 0.984654f, -0.172584f)
		},
		{	// 발뒤꿈치
			{73684, 0.008145f, 0.150210f},
			D3DXVECTOR3(0, 0, 0)
		},
	};
	std::vector<MEASURE_ITEM*>	m_item_ptr_list;
	int	nElement = sizeof(def_length) / sizeof(LENGTH_DEF);
	for (int i = 0; i < nElement; ++i) {
		MEASURE_ITEM*	m_item_ptr = new MEASURE_ITEM(__devicePtr, MAX_SEGMENT, def_length[i]);
		m_item_ptr_list.push_back(m_item_ptr);
	}
	MEASURE*	measure_ptr = new MEASURE(__devicePtr, m_item_ptr_list);
	__measure_ptr_dic[TAPE_MEASURE_RIGHT_LEG] = measure_ptr;
} while (0);
do {	// 왼쪽 팔
	MEASURE_PTR_HASH::iterator	it;
	it = __measure_ptr_dic.find(TAPE_MEASURE_LEFT_ARM);
	if (it != __measure_ptr_dic.end())
		__measure_ptr_dic.erase(it);

	LENGTH_DEF	def_length[] = {
		{	// 팔 윗부분
			{81085, 0.073198f, 0.486765f},
			D3DXVECTOR3(-0.998316f, 0.058097f, 0.000468f)
		},
		{	// 팔꿈치
			{56777, 0.059849f, 0.894122f},
			D3DXVECTOR3(-0.998316f, 0.058097f, 0.000468f)
		},
		{	// 팔꿈치
			{81965, 0.369442f, 0.201236f},
			D3DXVECTOR3(-0.931428f, 0.061501f, 0.358703f)
		},
		{	// 손목
			{81085, 0.073198f, 0.486765f},
			D3DXVECTOR3(-0.931428f, 0.061501f, 0.358703f)
		},
	};
	std::vector<MEASURE_ITEM*>	m_item_ptr_list;
	int	nElement = sizeof(def_length) / sizeof(LENGTH_DEF);
	for (int i = 0; i < nElement; ++i) {
		MEASURE_ITEM*	m_item_ptr = new MEASURE_ITEM(__devicePtr, MAX_SEGMENT, def_length[i]);
		m_item_ptr_list.push_back(m_item_ptr);
	}
	MEASURE*	measure_ptr = new MEASURE(__devicePtr, m_item_ptr_list);
	__measure_ptr_dic[TAPE_MEASURE_LEFT_ARM] = measure_ptr;
} while (0);
do {	// 오른쪽 팔
	MEASURE_PTR_HASH::iterator	it;
	it = __measure_ptr_dic.find(TAPE_MEASURE_RIGHT_ARM);
	if (it != __measure_ptr_dic.end())
		__measure_ptr_dic.erase(it);

	LENGTH_DEF	def_length[] = {
		{	// 팔 위부분
			{58673, 0.103706f, 0.770020f},
			D3DXVECTOR3(-0.970453f, -0.220734f, -0.097510f)
		},
		{	// 팔꿈치
			{82418, 0.251168f, 0.664359f},
			D3DXVECTOR3(-0.970453f, -0.220734f, -0.097510f)
		},
		{	// 팔꿈치
			{82418, 0.251168f, 0.664359f},
			D3DXVECTOR3(-0.964311f, -0.052404f, -0.259560f)
		},
		{	// 손목
			{84823, 0.257910f, 0.566288f},
			D3DXVECTOR3(-0.964311f, -0.052404f, -0.259560f)
		},
	};
	std::vector<MEASURE_ITEM*>	m_item_ptr_list;
	int	nElement = sizeof(def_length) / sizeof(LENGTH_DEF);
	for (int i = 0; i < nElement; ++i) {
		MEASURE_ITEM*	m_item_ptr = new MEASURE_ITEM(__devicePtr, MAX_SEGMENT, def_length[i]);
		m_item_ptr_list.push_back(m_item_ptr);
	}
	MEASURE*	measure_ptr = new MEASURE(__devicePtr, m_item_ptr_list);
	__measure_ptr_dic[TAPE_MEASURE_RIGHT_ARM] = measure_ptr;
} while (0);
#endif
do {	// 왼쪽 팔 (안쪽)
	MEASURE_PTR_HASH::iterator	it;
	it = __measure_ptr_dic.find(TAPE_MEASURE_LEFT_ARM_INSIDE);
	if (it != __measure_ptr_dic.end())
		__measure_ptr_dic.erase(it);

#if 1	// 더 이상 사용되지 않는다.
	LENGTH_DEF	lenthDef = {
		{86366, 0.430915f, 0.142952f},
		D3DXVECTOR3(0.000000f, 1.000000f, 0.000000f)
	};
#endif
	std::vector<MEASURE_ITEM*>	m_item_ptr_list;
	MEASURE_ITEM*	m_item_ptr;
	m_item_ptr = new MEASURE_ITEM(__devicePtr, MAX_SEGMENT, lenthDef);
{
	POINT_DEF	point0Def = {56950, 0.556603f, 0.268797f};
	POINT_DEF	point1Def = {81040, 0.561017f, 0.294061f};
	MEASURE_ITEM_DEF&	def = *((MEASURE_ITEM_DEF*) m_item_ptr);
	def.__type = MEASURE_TYPE__TWO_POINT_GIRTH;
	def.__ptr = new MEASURE_ITEM_PTR_DEF__TWO_POINT_GIRTH(TAPE_MEASURE_MODE_WRAP, point0Def, point1Def);
}
	m_item_ptr_list.push_back(m_item_ptr);
#if 1	// 가짜로...
	m_item_ptr = new MEASURE_ITEM();
	m_item_ptr_list.push_back(m_item_ptr);
#endif
	m_item_ptr = new MEASURE_ITEM(__devicePtr, MAX_SEGMENT, lenthDef);
{
	POINT_DEF	point0Def = {82198, 0.325851f, 0.469118f};
	POINT_DEF	point1Def = {81040, 0.561017f, 0.294061f};
	MEASURE_ITEM_DEF&	def = *((MEASURE_ITEM_DEF*) m_item_ptr);
	def.__type = MEASURE_TYPE__TWO_POINT_GIRTH;
	def.__ptr = new MEASURE_ITEM_PTR_DEF__TWO_POINT_GIRTH(TAPE_MEASURE_MODE_WRAP, point0Def, point1Def);
}
	m_item_ptr_list.push_back(m_item_ptr);
#if 1	// 가짜로...
	m_item_ptr = new MEASURE_ITEM();
	m_item_ptr_list.push_back(m_item_ptr);
#endif
	MEASURE*	measure_ptr = new MEASURE(__devicePtr, m_item_ptr_list);
	__measure_ptr_dic[TAPE_MEASURE_LEFT_ARM_INSIDE] = measure_ptr;
} while (0);
do {	// 왼쪽 팔 (바깥쪽)
	MEASURE_PTR_HASH::iterator	it;
	it = __measure_ptr_dic.find(TAPE_MEASURE_LEFT_ARM_OUTSIDE);
	if (it != __measure_ptr_dic.end())
		__measure_ptr_dic.erase(it);

#if 1	// 더 이상 사용되지 않는다.
	LENGTH_DEF	lenthDef = {
		{86366, 0.430915f, 0.142952f},
		D3DXVECTOR3(0.000000f, 1.000000f, 0.000000f)
	};
#endif
	std::vector<MEASURE_ITEM*>	m_item_ptr_list;
	MEASURE_ITEM*	m_item_ptr;
	m_item_ptr = new MEASURE_ITEM(__devicePtr, MAX_SEGMENT, lenthDef);
{
	POINT_DEF	point0Def = {56540, 0.439220f, 0.528292f};
	POINT_DEF	point1Def = {81116, 0.567838f, 0.280615f};
	MEASURE_ITEM_DEF&	def = *((MEASURE_ITEM_DEF*) m_item_ptr);
	def.__type = MEASURE_TYPE__TWO_POINT_GIRTH;
	def.__ptr = new MEASURE_ITEM_PTR_DEF__TWO_POINT_GIRTH(TAPE_MEASURE_MODE_WRAP, point0Def, point1Def);
}
	m_item_ptr_list.push_back(m_item_ptr);
#if 1	// 가짜로...
	m_item_ptr = new MEASURE_ITEM();
	m_item_ptr_list.push_back(m_item_ptr);
#endif
	m_item_ptr = new MEASURE_ITEM(__devicePtr, MAX_SEGMENT, lenthDef);
{
	POINT_DEF	point0Def = {81638, 0.585758f, 0.282053f};
	POINT_DEF	point1Def = {81116, 0.567838f, 0.280615f};
	MEASURE_ITEM_DEF&	def = *((MEASURE_ITEM_DEF*) m_item_ptr);
	def.__type = MEASURE_TYPE__TWO_POINT_GIRTH;
	def.__ptr = new MEASURE_ITEM_PTR_DEF__TWO_POINT_GIRTH(TAPE_MEASURE_MODE_WRAP, point0Def, point1Def);
}
	m_item_ptr_list.push_back(m_item_ptr);
#if 1	// 가짜로...
	m_item_ptr = new MEASURE_ITEM();
	m_item_ptr_list.push_back(m_item_ptr);
#endif
	MEASURE*	measure_ptr = new MEASURE(__devicePtr, m_item_ptr_list);
	__measure_ptr_dic[TAPE_MEASURE_LEFT_ARM_OUTSIDE] = measure_ptr;
} while (0);
do {	// 오른쪽 팔 (안쪽)
	MEASURE_PTR_HASH::iterator	it;
	it = __measure_ptr_dic.find(TAPE_MEASURE_RIGHT_ARM_INSIDE);
	if (it != __measure_ptr_dic.end())
		__measure_ptr_dic.erase(it);

#if 1	// 더 이상 사용되지 않는다.
	LENGTH_DEF	lenthDef = {
		{86366, 0.430915f, 0.142952f},
		D3DXVECTOR3(0.000000f, 1.000000f, 0.000000f)
	};
#endif
	std::vector<MEASURE_ITEM*>	m_item_ptr_list;
	MEASURE_ITEM*	m_item_ptr;
	m_item_ptr = new MEASURE_ITEM(__devicePtr, MAX_SEGMENT, lenthDef);
{
	POINT_DEF	point0Def = {59046, 0.097483f, 0.517813f};
	POINT_DEF	point1Def = {82458, 0.096169f, 0.430583f};
	MEASURE_ITEM_DEF&	def = *((MEASURE_ITEM_DEF*) m_item_ptr);
	def.__type = MEASURE_TYPE__TWO_POINT_GIRTH;
	def.__ptr = new MEASURE_ITEM_PTR_DEF__TWO_POINT_GIRTH(TAPE_MEASURE_MODE_WRAP, point0Def, point1Def);
}
	m_item_ptr_list.push_back(m_item_ptr);
#if 1	// 가짜로...
	m_item_ptr = new MEASURE_ITEM();
	m_item_ptr_list.push_back(m_item_ptr);
#endif
	m_item_ptr = new MEASURE_ITEM(__devicePtr, MAX_SEGMENT, lenthDef);
{
	POINT_DEF	point0Def = {84733, 0.087797f, 0.654378f};
	POINT_DEF	point1Def = {82458, 0.096169f, 0.430583f};
	MEASURE_ITEM_DEF&	def = *((MEASURE_ITEM_DEF*) m_item_ptr);
	def.__type = MEASURE_TYPE__TWO_POINT_GIRTH;
	def.__ptr = new MEASURE_ITEM_PTR_DEF__TWO_POINT_GIRTH(TAPE_MEASURE_MODE_WRAP, point0Def, point1Def);
}
	m_item_ptr_list.push_back(m_item_ptr);
#if 1	// 가짜로...
	m_item_ptr = new MEASURE_ITEM();
	m_item_ptr_list.push_back(m_item_ptr);
#endif
	MEASURE*	measure_ptr = new MEASURE(__devicePtr, m_item_ptr_list);
	__measure_ptr_dic[TAPE_MEASURE_RIGHT_ARM_INSIDE] = measure_ptr;
} while (0);
do {	// 오른쪽 팔 (바깥쪽)
	MEASURE_PTR_HASH::iterator	it;
	it = __measure_ptr_dic.find(TAPE_MEASURE_RIGHT_ARM_OUTSIDE);
	if (it != __measure_ptr_dic.end())
		__measure_ptr_dic.erase(it);

#if 1	// 더 이상 사용되지 않는다.
	LENGTH_DEF	lenthDef = {
		{86366, 0.430915f, 0.142952f},
		D3DXVECTOR3(0.000000f, 1.000000f, 0.000000f)
	};
#endif
	std::vector<MEASURE_ITEM*>	m_item_ptr_list;
	// 첫 번째
	MEASURE_ITEM*	m_item_ptr;
	m_item_ptr = new MEASURE_ITEM(__devicePtr, MAX_SEGMENT, lenthDef);
{
	POINT_DEF	point0Def = {59301, 0.006432f, 0.615452f};
	POINT_DEF	point1Def = {83625, 0.448489f, 0.026192f};
	MEASURE_ITEM_DEF&	def = *((MEASURE_ITEM_DEF*) m_item_ptr);
	def.__type = MEASURE_TYPE__TWO_POINT_GIRTH;
	def.__ptr = new MEASURE_ITEM_PTR_DEF__TWO_POINT_GIRTH(TAPE_MEASURE_MODE_WRAP, point0Def, point1Def);
}
	m_item_ptr_list.push_back(m_item_ptr);
#if 1	// 가짜로...
	m_item_ptr = new MEASURE_ITEM();
	m_item_ptr_list.push_back(m_item_ptr);
#endif
	// 두 번째
	m_item_ptr = new MEASURE_ITEM(__devicePtr, MAX_SEGMENT, lenthDef);
{
	POINT_DEF	point0Def = {84828, 0.265014f, 0.658732f};
	POINT_DEF	point1Def = {83625, 0.448489f, 0.026192f};
	MEASURE_ITEM_DEF&	def = *((MEASURE_ITEM_DEF*) m_item_ptr);
	def.__type = MEASURE_TYPE__TWO_POINT_GIRTH;
	def.__ptr = new MEASURE_ITEM_PTR_DEF__TWO_POINT_GIRTH(TAPE_MEASURE_MODE_WRAP, point0Def, point1Def);
}
	m_item_ptr_list.push_back(m_item_ptr);
#if 1	// 가짜로...
	m_item_ptr = new MEASURE_ITEM();
	m_item_ptr_list.push_back(m_item_ptr);
#endif
	MEASURE*	measure_ptr = new MEASURE(__devicePtr, m_item_ptr_list);
	__measure_ptr_dic[TAPE_MEASURE_RIGHT_ARM_OUTSIDE] = measure_ptr;
} while (0);
do {	// 왼쪽 다리 (안쪽)
	MEASURE_PTR_HASH::iterator	it;
	it = __measure_ptr_dic.find(TAPE_MEASURE_LEFT_LEG_INSIDE);
	if (it != __measure_ptr_dic.end())
		__measure_ptr_dic.erase(it);

#if 1	// 더 이상 사용되지 않는다.
	LENGTH_DEF	lenthDef = {
		{86366, 0.430915f, 0.142952f},
		D3DXVECTOR3(0.000000f, 1.000000f, 0.000000f)
	};
#endif
	std::vector<MEASURE_ITEM*>	m_item_ptr_list;
	MEASURE_ITEM*	m_item_ptr = new MEASURE_ITEM(__devicePtr, MAX_SEGMENT, lenthDef);
	POINT_DEF	point0Def = {64476, 0.321285f, 0.668669f};
	POINT_DEF	point1Def = {74333, 0.221292f, 0.665281f};
	MEASURE_ITEM_DEF&	def = *((MEASURE_ITEM_DEF*) m_item_ptr);
	def.__type = MEASURE_TYPE__TWO_POINT_GIRTH;
	def.__ptr = new MEASURE_ITEM_PTR_DEF__TWO_POINT_GIRTH(TAPE_MEASURE_MODE_WRAP, point0Def, point1Def);
	m_item_ptr_list.push_back(m_item_ptr);
	MEASURE*	measure_ptr = new MEASURE(__devicePtr, m_item_ptr_list);
	__measure_ptr_dic[TAPE_MEASURE_LEFT_LEG_INSIDE] = measure_ptr;
} while (0);
do {	// 왼쪽 다리 (바깥쪽)
	MEASURE_PTR_HASH::iterator	it;
	it = __measure_ptr_dic.find(TAPE_MEASURE_LEFT_LEG_OUTSIDE);
	if (it != __measure_ptr_dic.end())
		__measure_ptr_dic.erase(it);

#if 1	// 더 이상 사용되지 않는다.
	LENGTH_DEF	lenthDef = {
		{86366, 0.430915f, 0.142952f},
		D3DXVECTOR3(0.000000f, 1.000000f, 0.000000f)
	};
#endif
	std::vector<MEASURE_ITEM*>	m_item_ptr_list;
	MEASURE_ITEM*	m_item_ptr = new MEASURE_ITEM(__devicePtr, MAX_SEGMENT, lenthDef);
	POINT_DEF	point0Def = {64261, 0.598616f, 0.322904f};
	POINT_DEF	point1Def = {74678, 0.226481f, 0.352553f};
	MEASURE_ITEM_DEF&	def = *((MEASURE_ITEM_DEF*) m_item_ptr);
	def.__type = MEASURE_TYPE__TWO_POINT_GIRTH;
	def.__ptr = new MEASURE_ITEM_PTR_DEF__TWO_POINT_GIRTH(TAPE_MEASURE_MODE_WRAP, point0Def, point1Def);
	m_item_ptr_list.push_back(m_item_ptr);
	MEASURE*	measure_ptr = new MEASURE(__devicePtr, m_item_ptr_list);
	__measure_ptr_dic[TAPE_MEASURE_LEFT_LEG_OUTSIDE] = measure_ptr;
} while (0);
do {	// 오른쪽 다리 (안쪽)
	MEASURE_PTR_HASH::iterator	it;
	it = __measure_ptr_dic.find(TAPE_MEASURE_RIGHT_LEG_INSIDE);
	if (it != __measure_ptr_dic.end())
		__measure_ptr_dic.erase(it);

#if 1	// 더 이상 사용되지 않는다.
	LENGTH_DEF	lenthDef = {
		{86366, 0.430915f, 0.142952f},
		D3DXVECTOR3(0.000000f, 1.000000f, 0.000000f)
	};
#endif
	std::vector<MEASURE_ITEM*>	m_item_ptr_list;
	MEASURE_ITEM*	m_item_ptr = new MEASURE_ITEM(__devicePtr, MAX_SEGMENT, lenthDef);
	POINT_DEF	point0Def = {66209, 0.259354f, 0.478601f};
	POINT_DEF	point1Def = {73868, 0.154951f, 0.400970f};
	MEASURE_ITEM_DEF&	def = *((MEASURE_ITEM_DEF*) m_item_ptr);
	def.__type = MEASURE_TYPE__TWO_POINT_GIRTH;
	def.__ptr = new MEASURE_ITEM_PTR_DEF__TWO_POINT_GIRTH(TAPE_MEASURE_MODE_WRAP, point0Def, point1Def);
	m_item_ptr_list.push_back(m_item_ptr);
	MEASURE*	measure_ptr = new MEASURE(__devicePtr, m_item_ptr_list);
	__measure_ptr_dic[TAPE_MEASURE_RIGHT_LEG_INSIDE] = measure_ptr;
} while (0);
do {	// 오른쪽 다리 (바깥쪽)
	MEASURE_PTR_HASH::iterator	it;
	it = __measure_ptr_dic.find(TAPE_MEASURE_RIGHT_LEG_OUTSIDE);
	if (it != __measure_ptr_dic.end())
		__measure_ptr_dic.erase(it);

#if 1	// 더 이상 사용되지 않는다.
	LENGTH_DEF	lenthDef = {
		{86366, 0.430915f, 0.142952f},
		D3DXVECTOR3(0.000000f, 1.000000f, 0.000000f)
	};
#endif
	std::vector<MEASURE_ITEM*>	m_item_ptr_list;
	MEASURE_ITEM*	m_item_ptr = new MEASURE_ITEM(__devicePtr, MAX_SEGMENT, lenthDef);
	POINT_DEF	point0Def = {73721, 0.485668f, 0.005433f};
	POINT_DEF	point1Def = {65561, 0.382790f, 0.059710f};
	MEASURE_ITEM_DEF&	def = *((MEASURE_ITEM_DEF*) m_item_ptr);
	def.__type = MEASURE_TYPE__TWO_POINT_GIRTH;
	def.__ptr = new MEASURE_ITEM_PTR_DEF__TWO_POINT_GIRTH(TAPE_MEASURE_MODE_WRAP, point0Def, point1Def);
	m_item_ptr_list.push_back(m_item_ptr);
	MEASURE*	measure_ptr = new MEASURE(__devicePtr, m_item_ptr_list);
	__measure_ptr_dic[TAPE_MEASURE_RIGHT_LEG_OUTSIDE] = measure_ptr;
} while (0);
#endif	// 길이 정의
#endif	// !defined
#endif	// 길이(둘레, 넓이, 길이) 측정

#if 0	// 체중
#if 1	// 체중 값을 설정
	__weightValues[0] =;	// WEIGHT_MEASURE__LEFT
	__weightValues[1] =;	// WEIGHT_MEASURE__RIGHT
	__weightValues[2] =;	// WEIGHT_MEASURE__TOTAL
#endif	// 체중 값을 설정
#endif	// 체중

#if 1	// 특정 부위
	// kState::printFaceIDList 설정해서 얻은 값 (SHIFT + CTRL + '8')
#if 1	// defined
{
	const AREA_TYPE_DEF_LIST&	ETRI_areaTypeDefList = areaTypeDefList[index_to_standard];
	cout << "# of def. = " << ETRI_areaTypeDefList.n << endl;
	for (int k = 0; k < ETRI_areaTypeDefList.n; ++k) {
		const AREA_TYPE_DEF&	areaTypeDef = ETRI_areaTypeDefList.ptr[k];
		const AREA_FACEID_LIST&	faceIDList = areaTypeDef.faceIDList;
		cout << "AreaTypeDefList[" << k << "]: " << areaTypeDef.type << " (" << faceIDList.n << ")" << endl;

		kArea::DEFINED_AREA_DIC::iterator	it;
		it = __defined_area_dic.find(areaTypeDef.type);
		if (it != __defined_area_dic.end())
			__defined_area_dic.erase(it);

//		__defined_area_dic[areaTypeDef.type] = defArea;
		kArea::DEFINED_AREA&	defArea = __defined_area_dic[areaTypeDef.type];
		defArea.value = .9f;	// 원하는 색깔이...
		std::set<DWORD>&	face_id_set = defArea.face_id_set;
		for (int i = 0; i < faceIDList.n; ++i) {
			face_id_set.insert(faceIDList.ptr[i]);
		}
	}
}
#else	// defined/!defined
do {	// 몸통
	kArea::DEFINED_AREA_DIC::iterator	it;
	it = __defined_area_dic.find(AREA_TRUNK);
	if (it != __defined_area_dic.end())
		__defined_area_dic.erase(it);

	kArea::DEFINED_AREA	defArea;
	defArea.value = .9f;	// 원하는 색깔이...
	std::set<DWORD>&	face_id_set = defArea.face_id_set;
	DWORD	faceID[] = {51744,51745,51746,51747,51748,51749,51750,51751,51752,51753,51754,51755,51756,51757,51758,51767,51768,51769,51770,51771,51772,51773,51774,51775,51776,51777,51778,51779,51780,51781,51782,51783,51784,51785,51786,51787,51788,51789,51790,51791,51792,51793,51794,51797,51798,51799,51800,51801,51802,51803,51804,51811,51812,51813,51814,51964,51965,51966,51967,51968,51969,51970,51971,51972,51973,51974,51975,51976,51977,51978,51979,51980,51981,51982,51983,51984,51985,51986,51987,51988,51989,51990,51991,51992,51993,51994,51995,51996,51997,51998,51999,52000,52001,52002,52003,52004,52005,52006,52007,52008,52009,52010,52011,52012,52013,52014,52015,52016,52017,52018,52019,52020,52021,52022,52023,52024,52025,52026,52027,52028,52029,52030,52031,52032,52033,52034,52035,52036,52037,52038,52039,52040,52041,52042,52043,52044,52045,52046,52047,52048,52049,52050,52051,52052,52053,52054,52055,52056,52057,52058,52059,52060,52061,52062,52063,52064,52065,52066,52067,52068,52069,52070,52071,52072,52073,52074,52075,52076,52077,52078,52079,52080,52081,52082,52083,52084,52085,52086,52087,52088,52089,52090,52091,52092,52093,52094,52095,52096,52097,52098,52099,52100,52101,52102,52103,52104,52105,52106,52107,52108,52109,52110,52111,52112,52113,52114,52115,52116,52117,52118,52119,52120,52125,52126,52127,52128,52129,52130,52131,52132,52133,52134,52135,52136,52137,52138,52139,52140,52141,52142,52143,52144,52145,52146,52147,52148,52149,52150,52151,52152,52153,52154,52167,52168,52169,52170,52171,52172,52180,52181,52182,52183,52184,52185,52190,52191,52192,52193,52194,52195,52196,52197,52198,52199,52200,52201,52202,52203,52204,52205,52206,52207,52208,52209,52210,52211,52212,52213,52214,52215,52216,52217,52218,52219,52220,52221,52222,52223,52224,52225,52226,52227,52228,52229,52230,52231,52232,52233,52234,52235,52236,52237,52250,52251,52252,52313,52314,52315,52316,52317,52318,52319,52320,52321,52322,52407,52408,52433,52434,52435,52436,52437,52438,52439,52440,52462,52463,52464,52465,52466,52467,52468,52469,52470,52471,52472,52473,52474,52475,52476,52477,52487,52488,52489,52491,52492,52497,52498,52499,52500,52501,52502,52503,52504,52505,52506,52507,52508,52509,52510,52511,52512,52513,52514,52515,52516,52517,52518,52519,52520,52521,52522,52523,52524,52525,52526,52527,52528,52529,52530,52531,52532,52533,52534,52535,52536,52537,52538,52539,52540,52541,52542,52543,52544,52545,52546,52547,52548,52549,52550,52551,52552,52553,52554,52555,52556,52557,52558,52559,52560,52561,52562,52563,52564,52565,52566,52567,52568,52569,52570,52571,52631,52632,52633,52634,52635,52636,52637,52638,52639,52640,52641,52642,52643,52644,52645,52646,52647,52648,52649,52650,52651,52652,52653,52654,52655,52656,52657,52658,52659,52660,52661,52662,52663,52664,52665,52666,52667,52668,52669,52670,52671,52672,52673,52674,52675,52676,52677,52678,52679,52680,52681,52682,52683,52684,52685,52686,52687,52688,52689,52690,52691,52692,52693,52694,52695,52696,52697,52698,52699,52700,52701,52702,52703,52704,52705,52706,52707,52708,52709,52710,52711,52712,52713,52714,52715,52716,52717,52718,52719,52720,52721,52722,52723,52724,52725,52726,52727,52728,52729,52730,52731,52732,52733,52734,52735,52736,52737,52738,52739,52740,52741,52742,52743,52744,52745,52746,52747,52748,52749,52750,52751,52752,52753,52754,52755,52756,52757,52758,52759,52760,52761,52762,52763,52764,52765,52766,52767,52768,52769,52770,52771,52772,52773,52774,52775,52776,52777,52778,52779,52780,52781,52782,52783,52784,52785,52786,52787,52788,52789,52790,52791,52792,52793,52794,52795,52796,52797,52798,52799,52800,52801,52802,52803,52804,52805,52806,52807,52808,52809,52810,52811,52812,52813,52814,52815,52816,52817,52818,52819,52820,52821,52822,52823,52824,52825,52826,52827,52828,52829,52830,52831,52832,52833,52834,52835,52836,52837,52838,52839,52840,52841,52842,52843,52844,52845,52846,52847,52848,52849,52850,52851,52852,52853,52854,52855,52856,52857,52858,52859,52860,52861,52862,52863,52864,52865,52866,52867,52868,52869,52870,52871,52872,52873,52874,52875,52876,52877,52878,52879,52880,52881,52882,52883,52884,52885,52886,52887,52888,52889,52890,52891,52892,52893,52894,52895,52896,52897,52898,52899,52900,52901,52902,52903,52904,52905,52906,52907,52908,52909,52910,52911,52912,52913,52914,52915,52916,52917,52918,52919,52920,52921,52922,52923,52924,52925,52926,52927,52928,52929,52930,52931,52932,52933,52934,52935,52936,52937,52938,52939,52940,52941,52942,52943,52944,52945,52946,52947,52948,52949,52950,52951,52952,52953,52954,52955,52956,52957,52958,52959,52960,52961,52962,52963,52964,52965,52966,52967,52968,52969,52970,52971,52972,52973,52974,52975,52976,52977,52978,52979,52980,52981,52982,53038,53039,53040,53041,53042,53043,53044,53045,53046,53047,53048,53049,53050,53051,53052,53053,53054,53055,53056,53057,53058,53059,53060,53061,53062,53063,53064,53065,53066,53067,53068,53069,53074,53075,53076,53077,53078,53079,53080,53081,53082,53083,53084,53085,53086,53087,53088,53089,53090,53091,53092,53093,53094,53095,53096,53097,53098,53099,53100,53101,53102,53103,53104,53105,53106,53107,53108,53109,53110,53111,53112,53113,53118,53119,53120,53121,53122,53123,53150,53151,53152,53153,53154,53155,53156,53237,53238,53239,53240,53241,53242,53243,53244,53245,53246,53247,53248,53249,53250,53251,53260,53261,53262,53263,53264,53265,53266,53267,53268,53269,53270,53271,53272,53273,53274,53275,53276,53277,53278,53279,53280,53281,53282,53283,53284,53285,53286,53287,53290,53291,53292,53293,53294,53295,53296,53297,53304,53305,53306,53307,53457,53458,53459,53460,53461,53462,53463,53464,53465,53466,53467,53468,53469,53470,53471,53472,53473,53474,53475,53476,53477,53478,53479,53480,53481,53482,53483,53484,53485,53486,53487,53488,53489,53490,53491,53492,53493,53494,53495,53496,53497,53498,53499,53500,53501,53502,53503,53504,53505,53506,53507,53508,53509,53510,53511,53512,53513,53514,53515,53516,53517,53518,53519,53520,53521,53522,53523,53524,53525,53526,53527,53528,53529,53530,53531,53532,53533,53534,53535,53536,53537,53538,53539,53540,53541,53542,53543,53544,53545,53546,53547,53548,53549,53550,53551,53552,53553,53554,53555,53556,53557,53558,53559,53560,53561,53562,53563,53564,53565,53566,53567,53568,53569,53570,53571,53572,53573,53574,53575,53576,53577,53578,53579,53580,53581,53582,53583,53584,53585,53586,53587,53588,53589,53590,53591,53592,53593,53594,53595,53596,53597,53598,53599,53600,53601,53602,53603,53604,53605,53606,53607,53608,53609,53610,53611,53612,53613,53618,53619,53620,53621,53622,53623,53624,53625,53626,53627,53628,53629,53630,53631,53632,53633,53634,53635,53636,53637,53638,53639,53640,53641,53642,53643,53644,53645,53646,53647,53660,53661,53662,53663,53664,53665,53673,53674,53675,53676,53677,53678,53683,53684,53685,53686,53687,53688,53689,53690,53691,53692,53693,53694,53695,53696,53697,53698,53699,53700,53701,53702,53703,53704,53705,53706,53707,53708,53709,53710,53711,53712,53713,53714,53715,53716,53717,53718,53719,53720,53721,53722,53723,53724,53725,53726,53727,53728,53729,53730,53743,53744,53745,53806,53807,53808,53809,53810,53811,53812,53813,53814,53815,53900,53901,53926,53927,53928,53929,53930,53931,53932,53933,53955,53956,53957,53958,53959,53960,53961,53962,53963,53964,53965,53966,53967,53968,53969,53970,53980,53981,53982,53984,53985,53990,53991,53992,53993,53994,53995,53996,53997,53998,53999,54000,54001,54002,54003,54004,54005,54006,54007,54008,54009,54010,54011,54012,54013,54014,54015,54016,54017,54018,54019,54020,54021,54022,54023,54024,54025,54026,54027,54028,54029,54030,54031,54032,54033,54034,54035,54036,54037,54038,54039,54040,54041,54042,54043,54044,54045,54046,54047,54048,54049,54050,54051,54052,54053,54054,54055,54056,54057,54058,54059,54060,54061,54062,54063,54064,54124,54125,54126,54127,54128,54129,54130,54131,54132,54133,54134,54135,54136,54137,54138,54139,54140,54141,54142,54143,54144,54145,54146,54147,54148,54149,54150,54151,54152,54153,54154,54155,54156,54157,54158,54159,54160,54161,54162,54163,54164,54165,54166,54167,54168,54169,54170,54171,54172,54173,54174,54175,54176,54177,54178,54179,54180,54181,54182,54183,54184,54185,54186,54187,54188,54189,54190,54191,54192,54193,54194,54195,54196,54197,54198,54199,54200,54201,54202,54203,54204,54205,54206,54207,54208,54209,54210,54211,54212,54213,54214,54215,54216,54217,54218,54219,54220,54221,54222,54223,54224,54225,54226,54227,54228,54229,54230,54231,54232,54233,54234,54235,54236,54237,54238,54239,54240,54241,54242,54243,54244,54245,54246,54247,54248,54249,54250,54251,54252,54253,54254,54255,54256,54257,54258,54259,54260,54261,54262,54263,54264,54265,54266,54267,54268,54269,54270,54271,54272,54273,54274,54275,54276,54277,54278,54279,54280,54281,54282,54283,54284,54285,54286,54287,54288,54289,54290,54291,54292,54293,54294,54295,54296,54297,54298,54299,54300,54301,54302,54303,54304,54305,54306,54307,54308,54309,54310,54311,54312,54313,54314,54315,54316,54317,54318,54319,54320,54321,54322,54323,54324,54325,54326,54327,54328,54329,54330,54331,54332,54333,54334,54335,54336,54337,54338,54339,54340,54341,54342,54343,54344,54345,54346,54347,54348,54349,54350,54351,54352,54353,54354,54355,54356,54357,54358,54359,54360,54361,54362,54363,54364,54365,54366,54367,54368,54369,54370,54371,54372,54373,54374,54375,54376,54377,54378,54379,54380,54381,54382,54383,54384,54385,54386,54387,54388,54389,54390,54391,54392,54393,54394,54395,54396,54397,54398,54399,54400,54401,54402,54403,54404,54405,54406,54407,54408,54409,54410,54411,54412,54413,54414,54415,54416,54417,54418,54419,54420,54421,54422,54423,54424,54425,54426,54427,54428,54429,54430,54431,54432,54433,54434,54435,54436,54437,54438,54439,54440,54441,54442,54443,54444,54445,54446,54447,54448,54449,54450,54451,54452,54453,54454,54455,54456,54457,54458,54459,54460,54461,54462,54463,54464,54465,54466,54467,54468,54469,54470,54471,54472,54473,54474,54475,54531,54532,54533,54534,54535,54536,54537,54538,54539,54540,54541,54542,54543,54544,54545,54546,54547,54548,54549,54550,54551,54552,54553,54554,54555,54556,54557,54558,54559,54560,54561,54562,54567,54568,54569,54570,54571,54572,54573,54574,54575,54576,54577,54578,54579,54580,54581,54582,54583,54584,54585,54586,54587,54588,54589,54590,54591,54592,54593,54594,54595,54596,54597,54598,54599,54600,54601,54602,54603,54604,54605,54606,54611,54612,54613,54614,54615,54616,54643,54644,54645,54646,54647,54648,54649,54654,54655,54656,54657,54658,54659,54660,54661,54662,54663,54664,54665,54666,54667,54668,54669,54670,54671,54672,54673,54674,54675,54676,54677,54678,54679,54680,54681,54682,54683,54684,54685,54686,54687,54688,54689,54690,54691,54692,54693,54694,54695,54696,54697,54698,54699,54700,54701,54702,54703,54704,54705,54706,54707,54708,54709,54710,54711,54712,54713,54714,54715,54716,54717,54718,54719,54720,54721,54722,54723,54724,54725,54726,54727,54728,54729,54730,54731,54732,54733,54734,54735,54736,54737,54738,54739,54740,54741,54742,54743,54744,54745,54746,54747,54748,54749,54750,54751,54752,54753,54754,54755,54756,54757,54758,54759,54760,54761,54762,54763,54764,54765,54766,54767,54768,54769,54770,54771,54772,54773,54774,54775,54776,54777,54778,54779,54780,54781,54782,54783,54784,54785,54786,54787,54788,54789,54790,54791,54792,54793,54794,54795,54796,54797,54798,54799,54800,54801,54802,54803,54804,54805,54806,54807,54808,54809,54810,54811,54812,54813,54814,54815,54816,54817,54818,54819,54820,54821,54822,54823,54824,54825,54826,54827,54828,54829,54830,54831,54832,54833,54834,54835,54836,54837,54838,54839,54840,54841,54842,54843,54844,54845,54846,54847,54848,54849,54850,54851,54852,54853,54854,54855,54856,54857,54858,54859,54860,54861,54862,54863,54864,54865,54866,54867,54868,54869,54870,54871,54872,54873,54874,54875,54876,54877,54878,54879,54880,54881,54882,54883,54884,54885,54886,54887,54888,54889,54890,54891,54892,54893,54894,54895,54896,54897,54898,54899,54900,54901,54902,54903,54904,54905,54906,54907,54908,54909,54910,54911,54912,54913,54914,54915,54916,54917,54918,54919,54920,54921,54922,54923,54924,54925,54926,54927,54928,54929,54930,54931,54932,54933,54934,54935,54936,54937,54938,54939,54940,54941,54942,54943,54944,54945,54946,55101,55102,55103,55104,55105,55106,55107,55108,55109,55110,55111,55112,55113,55114,55115,55116,55117,55118,55119,55120,55121,55122,55123,55124,55125,55126,55127,55128,55129,55130,55131,55132,55133,55134,55135,55136,55137,55138,55139,55140,55141,55142,55143,55144,55145,55146,55147,55148,55149,55150,55151,55152,55153,55154,55155,55156,55157,55158,55159,55160,55161,55162,55163,55164,55165,55166,55167,55168,55169,55170,55171,55172,55173,55174,55175,55176,55177,55178,55179,55180,55181,55182,55183,55184,55185,55186,55187,55188,55189,55190,55191,55192,55193,55194,55195,55196,55197,55207,55208,55209,55210,55215,55216,55217,55218,55225,55226,55233,55234,55237,55238,55239,55240,55241,55242,55243,55244,55245,55246,55247,55248,55249,55250,55251,55252,55253,55254,55255,55256,55257,55258,55259,55260,55261,55262,55263,55264,55265,55266,55267,55268,55269,55270,55271,55272,55273,55274,55275,55276,55277,55278,55279,55280,55281,55282,55283,55284,55285,55286,55287,55288,55289,55290,55291,55292,55293,55294,55295,55296,55297,55298,55299,55300,55301,55302,55303,55304,55305,55306,55307,55308,55309,55310,55311,55312,55313,55314,55315,55316,55317,55318,55319,55320,55321,55322,55323,55324,55325,55326,55327,55328,55329,55330,55331,55332,55333,55334,55335,55336,55337,55338,55339,55340,55341,55342,55343,55344,55345,55346,55347,55348,55349,55350,55351,55352,55353,55354,55355,55356,55357,55358,55359,55360,55361,55362,55363,55364,55365,55366,55367,55368,55369,55370,55371,55372,55373,55374,55375,55376,55377,55378,55380,55381,55382,55383,55384,55385,55386,55387,55388,55389,55390,55391,55394,55395,55396,55397,55398,55399,55400,55401,55402,55403,55410,55411,55412,55413,55414,55415,55426,55427,55502,55503,55504,55505,55506,55507,55616,55617,55618,55619,55620,55621,55622,55623,55624,55625,55626,55627,55628,55629,55630,55631,55632,55633,55636,55637,55638,55643,55644,55645,55646,55647,55648,55649,55650,55651,55652,55653,55654,55655,55663,55664,55665,55666,55667,55668,55669,55670,55671,55672,55673,55674,55675,55676,55677,55678,55679,55680,55681,55682,55699,55700,55705,55706,55707,55708,55709,55710,55711,55712,55713,55714,55715,55716,55717,55718,55719,55720,55721,55723,55724,55725,55726,55838,55839,55840,55841,55842,55843,55844,55845,55847,55856,55857,55858,55859,55875,56134,56135,56136,56137,56147,56148,56149,56151,56152,56153,56154,56155,56156,56157,56158,56159,56160,56161,56162,56163,56164,56165,56166,56167,56168,56169,56170,56171,56172,56173,56174,56175,56176,56177,56178,56179,56180,56181,56182,56183,56184,56185,56186,56187,56190,56191,56192,56193,56194,56195,56199,56200,56305,56306,56307,56308,56309,56310,56311,56312,56313,56314,56315,56316,56317,56318,56319,56320,56321,56322,56323,56324,56325,56326,56327,56328,56329,56330,56331,56332,56333,56334,56335,56336,56337,56341,56342,56343,56344,56345,56346,56347,56348,56349,56350,56351,56352,56353,56354,56358,56359,56360,56361,56362,56363,56364,56365,56366,56367,56368,56369,56370,56371,56372,56373,56374,56375,56378,56380,56381,56382,56383,56384,56385,56389,56391,56392,56395,56396,56398,56399,56400,56408,56409,56410,56411,56412,56413,56414,56415,56416,56417,56418,56419,56420,56421,56422,56423,56424,56425,56426,56427,56428,56429,56430,56431,56432,56433,56434,56435,56436,56437,56438,56439,56440,56441,56442,56443,56446,56447,56448,56449,56450,56451,56452,56453,56454,56455,56461,56464,56465,56466,56484,56485,56628,56734,56735,56736,56737,56738,56739,56740,56741,56742,56743,56744,56745,56746,56747,56748,56749,56750,56751,56752,56753,56790,56791,56792,56793,56794,56795,56796,56797,56798,56799,56800,56815,56816,56818,56831,56832,56833,56834,56835,56836,56837,56851,56852,56853,56854,56855,57365,57366,57367,57370,57371,57372,57373,57374,57375,57376,57377,57378,57379,57380,57381,57382,57383,57384,57385,57386,57387,57388,57389,57390,57391,57392,57393,57394,57395,57396,57397,57398,57399,57400,57401,57402,57403,57404,57405,57406,57407,57408,57409,57410,57411,57412,57413,57414,57415,57417,57418,57419,57422,57423,57426,57427,57428,57429,57430,57431,57432,57433,57434,57435,57436,57437,57438,57439,57440,57441,57442,57443,57444,57445,57446,57447,57448,57449,57450,57451,57452,57453,57454,57455,57456,57457,57458,57459,57460,57461,57462,57463,57464,57465,57466,57467,57468,57469,57470,57471,57472,57473,57474,57475,57476,57477,57478,57479,57480,57481,57482,57483,57484,57485,57486,57487,57488,57489,57490,57491,57492,57493,57494,57495,57496,57497,57498,57499,57500,57501,57502,57503,57504,57505,57506,57507,57508,57509,57510,57511,57512,57513,57514,57515,57516,57517,57518,57519,57520,57521,57522,57523,57524,57525,57526,57527,57528,57529,57530,57531,57532,57533,57534,57535,57536,57537,57538,57539,57540,57541,57542,57543,57544,57545,57546,57547,57548,57549,57550,57551,57552,57553,57554,57555,57556,57557,57558,57559,57560,57561,57562,57563,57564,57565,57566,57567,57568,57569,57570,57571,57572,57573,57574,57575,57576,57577,57578,57579,57580,57581,57582,57583,57584,57585,57586,57587,57588,57589,57590,57591,57592,57593,57594,57595,57596,57597,57598,57599,57600,57601,57602,57603,57604,57605,57606,57607,57608,57609,57610,57611,57612,57613,57614,57615,57616,57617,57618,57619,57620,57621,57622,57623,57624,57625,57626,57627,57628,57629,57630,57631,57632,57633,57634,57635,57636,57637,57638,57639,57640,57641,57642,57643,57644,57645,57646,57647,57648,57649,57650,57651,57652,57653,57654,57655,57656,57657,57658,57659,57660,57661,57662,57663,57664,57665,57666,57667,57668,57669,57670,57671,57672,57673,57674,57675,57676,57677,57678,57679,57680,57681,57682,57683,57684,57685,57686,57687,57688,57689,57690,57691,57692,57693,57694,57695,57696,57697,57698,57699,57700,57701,57702,57703,57704,57705,57706,57707,57708,57709,57710,57711,57712,57713,57714,57715,57716,57717,57718,57873,57874,57875,57876,57877,57878,57879,57880,57881,57882,57883,57884,57885,57886,57887,57888,57889,57890,57891,57892,57893,57894,57895,57896,57897,57898,57899,57900,57901,57902,57903,57904,57905,57906,57907,57908,57909,57910,57911,57912,57913,57914,57915,57916,57917,57918,57919,57920,57921,57922,57923,57924,57925,57926,57927,57928,57929,57930,57931,57932,57933,57934,57935,57936,57937,57938,57939,57940,57941,57942,57943,57944,57945,57946,57947,57948,57949,57950,57951,57952,57953,57954,57955,57956,57957,57958,57959,57960,57961,57962,57963,57964,57965,57966,57967,57968,57969,57979,57980,57981,57982,57987,57988,57989,57990,57997,57998,58005,58006,58009,58010,58011,58012,58013,58014,58015,58016,58017,58018,58019,58020,58021,58022,58023,58024,58025,58026,58027,58028,58029,58030,58031,58032,58033,58034,58035,58036,58037,58038,58039,58040,58041,58042,58043,58044,58045,58046,58047,58048,58049,58050,58051,58052,58053,58054,58055,58056,58057,58058,58059,58060,58061,58062,58063,58064,58065,58066,58067,58068,58069,58070,58071,58072,58073,58074,58075,58076,58077,58078,58079,58080,58081,58082,58083,58084,58085,58086,58087,58088,58089,58090,58091,58092,58093,58094,58095,58096,58097,58098,58099,58100,58101,58102,58103,58104,58105,58106,58107,58108,58109,58110,58111,58112,58113,58114,58115,58116,58117,58118,58119,58120,58121,58122,58123,58124,58125,58126,58127,58128,58129,58130,58131,58132,58133,58134,58135,58136,58137,58138,58139,58140,58141,58142,58143,58144,58145,58146,58147,58148,58149,58150,58152,58153,58154,58155,58156,58157,58158,58159,58160,58161,58162,58163,58166,58167,58168,58169,58170,58171,58172,58173,58174,58175,58182,58183,58184,58185,58186,58187,58198,58199,58274,58275,58276,58277,58278,58279,58388,58389,58390,58391,58392,58393,58394,58395,58396,58397,58398,58399,58400,58401,58402,58403,58404,58405,58408,58409,58410,58415,58416,58417,58418,58419,58420,58421,58422,58423,58424,58425,58426,58427,58435,58436,58437,58438,58439,58440,58441,58442,58443,58444,58445,58446,58447,58448,58449,58450,58451,58452,58453,58454,58471,58472,58477,58478,58479,58480,58481,58482,58483,58484,58485,58486,58487,58488,58489,58490,58491,58492,58493,58495,58496,58497,58498,58610,58611,58612,58613,58614,58615,58616,58617,58619,58628,58629,58630,58631,58647,58906,58907,58908,58909,58919,58920,58921,58923,58924,58925,58926,58927,58928,58929,58930,58931,58932,58933,58934,58935,58936,58937,58938,58939,58940,58941,58942,58943,58944,58945,58946,58947,58948,58949,58950,58951,58952,58953,58954,58955,58956,58957,58958,58959,58962,58963,58964,58965,58966,58967,58971,58972,59077,59078,59079,59080,59081,59082,59083,59084,59085,59086,59087,59088,59089,59090,59091,59092,59093,59094,59095,59096,59097,59098,59099,59100,59101,59102,59103,59104,59105,59106,59107,59108,59109,59113,59114,59115,59116,59117,59118,59119,59120,59121,59122,59123,59124,59125,59126,59130,59131,59132,59133,59134,59135,59136,59137,59138,59139,59140,59141,59142,59143,59144,59145,59146,59147,59150,59152,59153,59154,59155,59156,59157,59161,59163,59164,59167,59168,59170,59171,59172,59180,59181,59182,59183,59184,59185,59186,59187,59188,59189,59190,59191,59192,59193,59194,59195,59196,59197,59198,59199,59200,59201,59202,59203,59204,59205,59206,59207,59208,59209,59210,59211,59212,59213,59214,59215,59218,59219,59220,59221,59222,59223,59224,59225,59226,59227,59233,59236,59237,59238,59256,59257,59400,59506,59507,59508,59509,59510,59511,59512,59513,59514,59515,59516,59517,59518,59519,59520,59521,59522,59523,59524,59525,59562,59563,59564,59565,59566,59567,59568,59569,59570,59571,59572,59587,59588,59590,59603,59604,59605,59606,59607,59608,59609,59623,59624,59625,59626,59627,60137,60138,60139,60142,60143,60144,60145,60146,60147,60148,60149,60150,60151,60152,60153,60154,60155,60156,60157,60158,60159,60160,60161,60162,60163,60164,60165,60166,60167,60168,60169,60170,60171,60172,60173,60174,60175,60176,60177,60178,60179,60180,60181,60182,60183,60184,60185,60186,60187,60189,60190,60191,60194,60195,60198,60199,60200,60201,60202,60203,60204,60205,60206,60207,60208,60209,60210,60211,60212,60213,60214,60215,60216,60217,60218,60219,60220,60221,60222,60223,60224,60225,60226,60227,60228,60229,60230,60231,60232,60233,60234,60235,60236,60237,60238,60239,60240,60241,60242,60243,60244,60245,60246,60247,60248,60249,60250,60251,60252,60253,60254,60255,60256,60257,60258,60259,60260,60261,60262,60263,60264,60265,60266,60267,60268,60269,60270,60271,60272,60273,60274,60275,60276,60277,60278,60279,60280,60281,60282,60283,60284,60285,60286,60287,60288,60289,60290,60291,60292,60293,60294,60295,60296,60297,60298,60299,60300,60301,60302,60303,60304,60305,60306,60307,60308,60309,60310,60311,60312,60313,60314,60315,60316,60317,60318,60319,60320,60321,60322,60323,60324,60325,60326,60327,60328,60329,60330,60331,60332,60333,60334,60335,60336,60337,60338,60339,60340,60341,60342,60343,60344,60345,60346,60347,60348,60349,60350,60351,60352,60353,60354,60355,60356,60357,60358,60359,60360,60361,60362,60363,60364,60365,60366,60367,60368,60369,60370,60371,60372,60373,60374,60375,60376,60377,60378,60379,60380,60381,60382,60383,60384,60385,60386,60387,60388,60389,60390,60391,60392,60393,60394,60395,60396,60397,60398,60399,60400,60401,60402,60403,60404,60405,60406,60407,60408,60409,60410,60411,60412,60413,60414,60415,60416,60417,60418,60419,60420,60421,60422,60423,60424,60425,60426,60427,60428,60429,60430,60431,60432,60433,60434,60435,60436,60437,60438,60439,60440,60441,60442,60443,60444,60445,60446,60447,60448,60449,60450,60451,60452,60453,60454,60455,60456,60457,60458,60459,60460,60461,60462,60463,60464,60465,60466,60467,60468,60469,60470,60471,60472,60473,60474,60475,60476,60477,60478,60479,60480,60481,60482,60483,60484,60485,60486,60487,60488,60489,60490,60491,60492,60493,60494,60495,60496,60497,60498,60499,60500,60501,60502,60503,60504,60505,60506,60507,60508,60509,60510,60511,60512,60513,60514,60515,60516,60517,60518,60519,60520,60521,60522,60523,60524,60525,60526,60527,60528,60529,60530,60531,60532,60533,60534,60535,60536,60537,60538,60539,60540,60541,60542,60543,60544,60545,60546,60547,60548,60549,60550,60551,60552,60553,60554,60555,60556,60557,60558,60559,60560,60561,60562,60563,60564,60565,60566,60567,60568,60569,60570,60571,60572,60573,60574,60575,60576,60577,60578,60579,60580,60581,60582,60583,60584,60585,60586,60587,60588,60589,60590,60591,60592,60593,60594,60595,60596,60597,60598,60599,60600,60601,60602,60603,60604,60605,60606,60607,60608,60609,60610,60611,60612,60613,60614,60615,60616,60617,60618,60619,60620,60621,60622,60623,60624,60625,60626,60627,60628,60629,60630,60631,60632,60633,60634,60635,60636,60637,60638,60639,60640,60641,60642,60643,60644,60645,60646,60647,60648,60649,60650,60651,60652,60653,60654,60655,60656,60657,60658,60659,60660,60661,60662,60663,60664,60665,60666,60667,60668,60669,60670,60671,60672,60673,60674,60675,60676,60677,60678,60679,60680,60681,60682,60683,60684,60685,60686,60687,60688,60689,60690,60691,60692,60693,60694,60695,60696,60697,60698,60699,60700,60701,60702,60703,60704,60705,60706,60707,60708,60709,60710,60711,60712,60713,60714,60715,60716,60717,60718,60719,60720,60721,60722,60723,60724,60725,60726,60727,60728,60729,60730,60731,60732,60733,60734,60735,60736,60737,60738,60739,60740,60741,60742,60743,60744,60745,60746,60747,60748,60749,60750,60751,60752,60753,60754,60755,60756,60757,60758,60759,60760,60761,60762,60763,60764,60765,60766,60767,60768,60769,60770,60771,60772,60773,60774,60775,60776,60777,60778,60779,60780,60781,60782,60783,60784,60785,60786,60787,60788,60789,60790,60791,60792,60793,60794,60795,60796,60797,60798,60799,60800,60801,60802,60803,60804,60805,60806,60807,60808,60809,60810,60811,60812,60813,60814,60815,60816,60817,60818,60819,60820,60821,60822,60823,60824,60825,60826,60827,60828,60829,60830,60831,60832,60833,60834,60835,60836,60837,60838,60839,60840,60841,60842,60843,60844,60845,60846,60847,60848,60849,60850,60851,60852,60853,60854,60855,60856,60857,60858,60859,60860,60861,60862,60863,60864,60865,60866,60867,60868,60869,60870,60871,60872,60873,60874,60875,60876,60877,60878,60879,60880,60881,60882,60883,60884,60885,60886,60887,60888,60889,60890,60891,60892,60893,60894,60895,60896,60897,60898,60899,60900,60901,60902,60903,60904,60905,60906,60907,60908,60909,60910,60911,60912,60913,60914,60915,60916,60917,60918,60919,60920,60921,60922,60923,60924,60925,60926,60927,60928,60929,60930,60931,60932,60933,60934,60935,60936,60937,60938,60939,60940,60941,60942,60943,60944,60945,60946,60947,60948,60949,60950,60951,60952,60953,60954,60955,60956,60957,60958,60959,60960,60961,60962,60963,60964,60965,60966,60967,60968,60969,60970,60971,60972,60973,60974,60975,60976,60977,60978,60979,60980,60981,60982,60983,60984,60985,60986,60987,60988,60989,60990,60991,60992,60993,60994,60995,60996,60997,60998,60999,61000,61001,61002,61003,61004,61005,61006,61007,61008,61009,61010,61011,61012,61013,61014,61015,61016,61017,61018,61019,61020,61021,61022,61023,61024,61025,61026,61027,61028,61029,61030,61031,61032,61033,61034,61035,61036,61037,61038,61039,61040,61041,61042,61043,61044,61045,61046,61047,61048,61049,61050,61051,61052,61053,61054,61055,61056,61057,61058,61059,61060,61061,61062,61063,61064,61065,61066,61067,61068,61069,61070,61071,61072,61073,61074,61075,61076,61077,61078,61079,61080,61081,61082,61083,61084,61085,61086,61087,61088,61089,61090,61091,61092,61093,61094,61095,61096,61097,61098,61099,61100,61101,61102,61103,61104,61105,61106,61107,61108,61109,61110,61111,61112,61113,61114,61115,61116,61117,61118,61119,61120,61121,61122,61123,61124,61125,61126,61127,61128,61129,61130,61131,61132,61133,61134,61135,61136,61137,61138,61139,61140,61141,61142,61143,61144,61145,61146,61147,61148,61149,61150,61151,61152,61153,61154,61155,61156,61157,61158,61159,61160,61161,61162,61163,61164,61165,61166,61167,61168,61169,61170,61171,61172,61173,61174,61175,61176,61177,61178,61179,61180,61181,61182,61183,61184,61185,61186,61187,61188,61189,61190,61191,61192,61193,61194,61195,61196,61197,61198,61199,61200,61201,61202,61203,61204,61205,61206,61207,61208,61209,61210,61211,61212,61213,61214,61215,61216,61217,61218,61219,61220,61221,61222,61223,61224,61225,61226,61227,61228,61229,61230,61231,61232,61233,61234,61235,61236,61237,61238,61239,61240,61241,61242,61243,61244,61245,61246,61247,61248,61249,61250,61251,61252,61253,61254,61255,61256,61257,61258,61259,61260,61261,61262,61263,61264,61265,61266,61267,61268,61269,61270,61271,61272,61273,61274,61275,61276,61277,61278,61279,61280,61281,61282,61283,61284,61285,61286,61287,61288,61289,61290,61291,61292,61293,61294,61295,61296,61297,61298,61299,61300,61301,61302,61303,61304,61305,61306,61307,61308,61309,61310,61311,61312,61313,61314,61315,61316,61317,61318,61319,61320,61321,61322,61323,61324,61325,61326,61327,61328,61329,61330,61331,61332,61333,61334,61335,61336,61337,61338,61339,61340,61341,61342,61343,61344,61345,61346,61347,61348,61349,61350,61351,61352,61353,61354,61355,61356,61357,61358,61359,61360,61361,61362,61363,61364,61365,61366,61367,61368,61369,61370,61371,61372,61373,61374,61375,61376,61377,61378,61379,61380,61381,61382,61383,61384,61385,61386,61387,61388,61389,61390,61391,61392,61393,61394,61395,61396,61397,61398,61399,61400,61401,61402,61403,61404,61405,61406,61407,61408,61409,61410,61411,61412,61413,61414,61415,61416,61417,61418,61419,61420,61421,61422,61423,61424,61425,61426,61427,61428,61429,61430,61431,61432,61433,61434,61435,61436,61437,61438,61439,61440,61441,61442,61443,61444,61445,61446,61447,61448,61449,61450,61451,61452,61453,61454,61455,61456,61457,61458,61459,61460,61461,61462,61463,61464,61465,61466,61467,61468,61469,61470,61471,61472,61473,61474,61475,61476,61477,61478,61479,61480,61481,61482,61483,61484,61485,61486,61487,61488,61489,61490,61491,61492,61493,61494,61495,61496,61497,61498,61499,61500,61501,61502,61503,61504,61505,61506,61507,61508,61509,61510,61511,61512,61513,61514,61515,61516,61517,61518,61519,61520,61521,61522,61523,61524,61525,61526,61527,61528,61529,61530,61531,61532,61533,61534,61535,61536,61537,61538,61539,61540,61541,61542,61543,61544,61545,61546,61547,61548,61549,61550,61551,61552,61553,61554,61555,61556,61557,61558,61559,61560,61561,61562,61563,61564,61565,61566,61567,61568,61569,61570,61571,61572,61573,61574,61575,61576,61577,61578,61579,61580,61581,61582,61583,61584,61585,61586,61587,61588,61589,61590,61591,61592,61593,61594,61595,61596,61597,61598,61599,61600,61601,61602,61603,61604,61605,61606,61607,61608,61609,61610,61611,61612,61613,61614,61615,61616,61617,61618,61619,61620,61621,61622,61623,61624,61625,61626,61627,61628,61629,61630,61631,61632,61633,61634,61635,61636,61637,61638,61639,61640,61641,61642,61643,61644,61645,61646,61647,61648,61649,61650,61651,61652,61653,61654,61655,61656,61657,61658,61659,61660,61661,61662,61663,61664,61665,61666,61667,61668,61669,61670,61671,61672,61673,61674,61675,61676,61677,61678,61679,61680,61681,61682,61683,61684,61685,61686,61687,61688,61689,61690,61691,61692,61693,61694,61695,61696,61697,61698,61699,61700,61701,61702,61703,61704,61705,61706,61707,61708,61709,61710,61711,61712,61713,61714,61715,61716,61717,61718,61719,61720,61721,61722,61723,61724,61725,61726,61727,61728,61729,61730,61731,61732,61733,61734,61735,61736,61737,61738,61739,61740,61741,61742,61743,61744,61745,61746,61747,61748,61749,61750,61751,61752,61753,61754,61755,61756,61757,61758,61759,61760,61761,61762,61763,61764,61765,61766,61767,61768,61769,61770,61771,61772,61773,61774,61775,61776,61777,61778,61779,61780,61781,61782,61783,61784,61785,61786,61787,61788,61789,61790,61791,61792,61793,61794,61795,61796,61797,61798,61799,61800,61801,61802,61803,61804,61805,61806,61807,61808,61809,61810,61811,61812,61813,61814,61815,61816,61817,61818,61819,61820,61821,61822,61823,61824,61825,61826,61827,61828,61829,61830,61831,61832,61833,61834,61835,61836,61837,61838,61839,61840,61841,61842,61843,61844,61845,61846,61847,61848,61849,61850,61851,61852,61853,61854,61855,61856,61857,61858,61859,61860,61861,61862,61863,61864,61865,61866,61867,61868,61869,61870,61871,61872,61873,61874,61875,61876,61877,61878,61879,61880,61881,61882,61883,61884,61885,61886,61887,61888,61889,61890,61891,61892,61893,61894,61895,61896,61897,61898,61899,61900,61901,61902,61903,61904,61905,61906,61907,61908,61909,61910,61911,61912,61913,61914,61915,61916,61917,61918,61919,61920,61921,61922,61923,61924,61925,61926,61927,61928,61929,61930,61931,61932,61933,61934,61935,61936,61937,61938,61939,61940,61941,61942,61943,61944,61945,61946,61947,61948,61949,61950,61951,61952,61953,61954,61955,61956,61957,61958,61959,61960,61961,61962,61963,61964,61965,61966,61967,61968,61969,61970,61971,61972,61973,61974,61975,61976,61977,61978,61979,61980,61981,61982,61983,61984,61985,61986,61987,61988,61989,61990,61991,61992,61993,61994,61995,61996,61997,61998,61999,62000,62001,62002,62003,62004,62005,62006,62007,62008,62009,62010,62011,62012,62013,62014,62015,62016,62017,62018,62019,62020,62021,62022,62023,62024,62025,62026,62027,62028,62029,62030,62031,62032,62033,62034,62035,62036,62037,62038,62039,62040,62041,62042,62043,62044,62045,62046,62047,62048,62049,62050,62051,62052,62053,62054,62055,62056,62057,62058,62059,62060,62061,62062,62063,62064,62065,62066,62067,62068,62069,62070,62071,62072,62073,62074,62075,62076,62077,62078,62079,62080,62081,62082,62083,62084,62085,62086,62087,62088,62089,62090,62091,62092,62093,62094,62095,62096,62097,62098,62099,62100,62101,62102,62103,62104,62105,62106,62107,62108,62109,62110,62111,62112,62113,62114,62115,62116,62117,62118,62119,62120,62121,62122,62123,62124,62125,62126,62127,62128,62129,62130,62131,62132,62133,62134,62135,62136,62137,62138,62139,62140,62141,62142,62143,62144,62145,62146,62147,62148,62149,62150,62151,62152,62153,62154,62155,62156,62157,62158,62159,62160,62161,62162,62163,62164,62165,62166,62167,62168,62169,62170,62171,62172,62173,62174,62175,62176,62177,62178,62179,62180,62181,62182,62183,62184,62185,62186,62187,62188,62189,62190,62191,62192,62193,62194,62195,62196,62197,62198,62199,62200,62201,62202,62203,62204,62205,62206,62207,62208,62209,62210,62211,62212,62213,62214,62215,62216,62217,62218,62219,62220,62221,62222,62223,62224,62225,62226,62227,62228,62229,62230,62231,62232,62233,62234,62235,62236,62237,62238,62239,62240,62241,62242,62243,62244,62245,62246,62247,62248,62249,62250,62251,62252,62253,62254,62255,62256,62257,62258,62259,62260,62261,62262,62263,62264,62265,62266,62267,62268,62269,62270,62271,62272,62273,62274,62275,62276,62277,62278,62279,62280,62281,62282,62283,62284,62285,62286,62287,62288,62289,62290,62291,62292,62293,62294,62295,62296,62297,62298,62299,62300,62301,62302,62303,62304,62305,62306,62307,62308,62309,62310,62311,62312,62313,62314,62315,62316,62317,62318,62319,62320,62321,62322,62323,62324,62325,62326,62327,62328,62329,62330,62331,62332,62333,62334,62335,62336,62337,62338,62339,62340,62341,62342,62343,62344,62345,62346,62347,62348,62349,62350,62405,62406,62407,62408,62409,62410,62411,62412,62413,62414,62415,62416,62417,62418,62419,62420,62421,62422,62425,62426,62439,62440,62462,62463,62464,62465,62466,62467,62468,62469,62470,62471,62472,62473,62474,62475,62476,62477,62478,62479,62480,62481,62482,62483,62484,62485,62486,62487,62488,62491,62492,62493,62497,62498,62515,62516,62517,62518,62519,62520,62525,62530,62531,62532,62533,62534,62535,62536,62539,62540,62541,62649,62650,62651,62652,62653,62654,62655,62656,62657,62658,62659,62660,62661,62662,62663,62664,62665,62666,62687,62688,62689,62690,62691,62692,62693,62694,62695,62710,62711,62712,62713,62714,62715,62716,62717,62718,62719,62720,62721,62722,62723,62724,62725,62726,62727,62728,62729,62730,62731,62732,62733,62734,62735,62736,62737,62738,62739,62740,62741,62742,62743,62744,62745,62746,62747,62748,62749,62750,62751,62752,62753,62754,62755,62756,62757,62758,62759,62760,62761,62762,62763,62764,62765,62766,62767,62768,62769,62770,62771,62772,62773,62774,62775,62776,62777,62778,62779,62780,62781,62782,62783,62784,62785,62786,62787,62788,62789,62790,62791,62792,62793,62794,62795,62796,62797,62798,62799,62800,62801,62802,62803,62804,62805,62806,62807,62808,62809,62810,62811,62812,62813,62814,62815,62816,62817,62818,62819,62820,62821,62822,62823,62824,62825,62826,62827,62828,62829,62830,62831,62832,62833,62834,62835,62836,62837,62838,62839,62840,62841,62842,62843,62844,62845,62846,62847,62848,62849,62850,62851,62852,62853,62854,62855,62856,62857,62858,62859,62860,62861,62862,62863,62864,62865,62866,62867,62868,62869,62870,62871,62872,62873,62874,62875,62876,62877,62878,62885,62888,62889,62890,62891,62892,62893,62894,62895,62896,62897,62898,62899,62900,62901,62902,62903,62904,62905,62906,62907,62908,62909,62910,62911,62912,62913,62914,62915,62916,62917,62918,62919,62920,62921,62922,62923,62924,62925,62926,62927,62928,62929,62930,62931,62932,62933,62934,62935,62936,62937,62938,62939,62940,62941,62942,62943,62944,62945,62946,62947,62948,62949,62950,62951,62952,62953,62954,62955,62956,62957,62958,62959,62960,62961,62962,62963,62964,62965,62966,62967,62968,62969,62970,62971,62972,62973,62974,62975,62976,62977,62978,62979,62980,62981,62982,62983,62984,62985,62986,62987,62988,62989,62990,62991,62992,62993,62994,62995,62996,62997,62998,62999,63000,63001,63002,63003,63004,63005,63006,63007,63008,63009,63010,63011,63012,63013,63014,63015,63016,63017,63018,63019,63020,63021,63022,63023,63024,63025,63026,63027,63028,63029,63030,63031,63032,63033,63034,63035,63036,63037,63038,63039,63040,63041,63042,63043,63044,63045,63046,63047,63048,63049,63050,63051,63052,63053,63054,63055,63056,63057,63058,63059,63060,63061,63062,63063,63064,63065,63066,63067,63068,63069,63070,63071,63072,63073,63074,63129,63130,63131,63132,63133,63134,63135,63136,63137,63138,63139,63140,63141,63142,63143,63144,63145,63146,63149,63150,63163,63164,63186,63187,63188,63189,63190,63191,63192,63193,63194,63195,63196,63197,63198,63199,63200,63201,63202,63203,63204,63205,63206,63207,63208,63209,63210,63211,63212,63215,63216,63217,63221,63222,63239,63240,63241,63242,63243,63244,63249,63254,63255,63256,63257,63258,63259,63260,63263,63264,63265,63373,63374,63375,63376,63377,63378,63379,63380,63381,63382,63383,63384,63385,63386,63387,63388,63389,63390,63411,63412,63413,63414,63415,63416,63417,63418,63419,63434,63435,63436,63437,63438,63439,63440,63441,63442,63443,63444,63445,63446,63447,63448,63449,63450,63451,63452,63453,63454,63455,63456,63457,63458,63459,63460,63461,63462,63463,63464,63465,63466,63467,63468,63469,63470,63471,63472,63473,63474,63475,63476,63477,63478,63479,63480,63481,63482,63483,63484,63485,63486,63487,63488,63489,63490,63491,63492,63493,63494,63495,63496,63497,63498,63499,63500,63501,63502,63503,63504,63505,63506,63507,63508,63509,63510,63511,63512,63513,63514,63515,63516,63517,63518,63519,63520,63521,63522,63523,63524,63525,63526,63527,63528,63529,63530,63531,63532,63533,63534,63535,63536,63537,63538,63539,63540,63541,63542,63543,63544,63545,63546,63547,63548,63549,63550,63551,63552,63553,63554,63555,63556,63557,63558,63559,63560,63561,63562,63563,63564,63565,63566,63567,63568,63569,63570,63571,63572,63573,63574,63575,63576,63577,63578,63579,63580,63581,63582,63583,63584,63585,63586,63587,63588,63589,63590,63591,63592,63593,63594,63595,63596,63597,63598,63599,63600,63601,63602,63609,63933,63934,63935,63936,63937,63954,63955,63956,63957,63971,63972,63973,64090,64091,64092,64093,64221,64223,64224,64225,64226,64231,64232,64233,64234,64235,64238,64239,64240,64241,64242,64243,64244,64245,64248,64249,64250,64251,64252,64253,64260,64261,64262,64263,64264,64265,64266,64267,64268,64269,64272,64273,64274,64275,64276,64277,64278,64279,64280,64281,64282,64283,64284,64285,64286,64287,64288,64289,64290,64291,64292,64293,64294,64295,64296,64303,64304,64307,64308,64312,64313,64314,64315,64316,64317,64318,64319,64320,64335,64336,64337,64338,64339,64343,64344,64345,64346,64348,64353,64355,64360,64361,64362,64363,64365,64366,64478,64479,64480,64486,64487,64488,64489,64490,64491,64492,64493,64496,64497,64498,64499,64500,64501,64504,64505,64506,64507,64512,64526,64527,64535,64536,64537,64538,64539,64540,64541,64542,64543,64544,64545,64546,64547,65091,65092,65093,65094,65099,65100,65101,65102,65103,65104,65105,65109,65110,65111,65112,65115,65116,65117,65118,65119,65120,65121,65122,65123,65124,65125,65126,65127,65128,65129,65130,65131,65132,65133,65158,65159,65160,65161,65162,65163,65164,65165,65319,65320,65321,65325,65326,65368,65369,65370,65371,65372,65373,65374,65375,65376,65377,65378,65379,65380,65393,65394,65395,65396,65397,65398,65401,65402,65403,65404,65408,65409,65744,65745,65746,65747,65748,65765,65766,65767,65768,65782,65783,65784,65901,65902,65903,65904,66032,66034,66035,66036,66037,66042,66043,66044,66045,66046,66049,66050,66051,66052,66053,66054,66055,66056,66059,66060,66061,66062,66063,66064,66071,66072,66073,66074,66075,66076,66077,66078,66079,66080,66083,66084,66085,66086,66087,66088,66089,66090,66091,66092,66093,66094,66095,66096,66097,66098,66099,66100,66101,66102,66103,66104,66105,66106,66107,66114,66115,66118,66119,66123,66124,66125,66126,66127,66128,66129,66130,66131,66146,66147,66148,66149,66150,66154,66155,66156,66157,66159,66164,66166,66171,66172,66173,66174,66176,66177,66289,66290,66291,66297,66298,66299,66300,66301,66302,66303,66304,66307,66308,66309,66310,66311,66312,66315,66316,66317,66318,66323,66337,66338,66346,66347,66348,66349,66350,66351,66352,66353,66354,66355,66356,66357,66358,66902,66903,66904,66905,66910,66911,66912,66913,66914,66915,66916,66920,66921,66922,66923,66926,66927,66928,66929,66930,66931,66932,66933,66934,66935,66936,66937,66938,66939,66940,66941,66942,66943,66944,66969,66970,66971,66972,66973,66974,66975,66976,67130,67131,67132,67136,67137,67179,67180,67181,67182,67183,67184,67185,67186,67187,67188,67189,67190,67191,67204,67205,67206,67207,67208,67209,67212,67213,67214,67215,67219,67220,};
	cout << "trunk: " << (sizeof(faceID) / sizeof(DWORD)) << endl;
	for (int i = 0; i < (int) sizeof(faceID) / sizeof(DWORD); ++i) {
		face_id_set.insert(faceID[i]);
	}
	__defined_area_dic[AREA_TRUNK] = defArea;
} while (0);
do {	// 왼쪽 팔
	kArea::DEFINED_AREA_DIC::iterator	it;
	it = __defined_area_dic.find(AREA_LEFT_ARM);
	if (it != __defined_area_dic.end())
		__defined_area_dic.erase(it);

	kArea::DEFINED_AREA	defArea;
	defArea.value = .9f;	// 원하는 색깔이...
	std::set<DWORD>&	face_id_set = defArea.face_id_set;
	DWORD	faceID[] = {55794,55795,55804,55805,55813,55814,55817,55818,55819,55820,55892,55893,55894,55895,55896,55897,55898,55899,55979,55980,55981,55982,56011,56012,56015,56016,56019,56020,56023,56024,56025,56026,56027,56028,56029,56030,56221,56222,56234,56235,56264,56265,56266,56267,56268,56269,56270,56271,56283,56288,56289,56290,56291,56292,56301,56302,56303,56304,56340,56494,56495,56504,56505,56518,56546,56547,56548,56549,56570,56571,56572,56573,56584,56585,56586,56587,56702,56717,56718,56732,56733,56774,56775,56922,56923,56926,56927,56928,56929,56930,56931,56932,56933,56934,56935,56936,56937,56938,56939,56940,56941,56942,56943,56944,56945,56946,56947,56948,56949,56950,56951,56952,56953,56954,56955,56956,56957,56958,56959,56960,56961,56962,56963,56964,56965,56966,56967,56968,56969,56970,56971,56972,56973,56974,56975,56976,56977,56978,56979,56980,56981,56982,56983,56984,56985,56986,56987,56988,56989,56990,56991,56992,56993,56994,56995,56996,56997,56998,56999,57000,57001,57002,57003,57004,57005,57006,57007,57008,57009,57010,57011,57012,57013,57014,57015,57016,57017,57018,57019,57020,57021,57022,57023,57024,57025,57026,57027,57028,57029,57030,57031,57032,57033,57034,57035,57036,57037,57038,57039,57040,57041,57042,57043,57044,57045,57046,57047,57048,57049,57050,57051,57052,57053,57054,57055,57056,57057,57058,57059,57060,57061,57062,57063,57064,57065,57066,57067,57068,57069,57070,57071,57072,57073,57074,57075,57076,57077,57078,57079,57080,57081,57082,57083,57084,57085,57086,57087,57088,57089,57090,57091,57092,57093,57094,57095,57096,57097,57098,57099,57100,57101,57102,57103,57104,57105,57106,57107,57108,57109,57110,57111,57112,57113,57114,57115,57116,57117,57118,57119,57120,57121,57122,57123,57124,57125,57126,57127,57128,57129,57130,57131,57132,57133,57134,57135,57136,57137,57138,57139,57140,57141,57142,57143,57144,57145,57146,57147,57148,57149,57150,57151,57152,57153,57154,57155,57156,57157,57158,57159,57160,57161,57162,57163,57164,57165,57166,57167,57168,57169,57170,57171,57172,57173,57174,57175,57176,57177,57178,57179,57180,57181,57182,57183,57184,57185,57186,57187,57188,57189,57190,57191,57192,57193,57194,57195,57196,57197,57198,57199,57200,57201,57202,57203,57204,57205,57206,57207,57208,57209,57210,57211,57212,57213,57214,57215,57216,57217,57218,57219,57220,57221,57222,57223,57224,57225,57226,57227,57228,57229,57230,57231,57232,57233,57234,57235,57236,57237,57238,57239,57240,57241,57242,57243,57244,57245,57246,57247,57248,57249,57250,57251,57252,57253,57254,57255,57256,57257,57258,57259,57260,57261,57262,57263,57264,57265,57266,57267,57268,57269,57270,57271,57272,57273,57274,57275,57276,57277,57278,57279,57280,57281,57282,57283,57284,57285,57286,57287,57288,57289,57290,57291,57292,57293,57294,57295,57296,57297,57298,57299,57300,57301,57302,57303,57304,57305,57306,57307,57308,57309,57310,57311,57312,57313,57314,57315,57316,57317,57318,57319,57320,57321,57322,57323,57324,57325,57326,57327,57328,57329,57330,57331,57332,57333,57334,57335,57336,57337,57338,57339,57340,57341,57342,57343,57344,57345,57346,57347,57348,57349,57350,57351,57352,57353,57354,57355,57356,57357,57358,57359,57360,57361,57362,57363,57364,79818,79819,79820,79821,79822,79823,79824,79825,79826,79827,79828,79829,79830,79831,79832,79833,79834,79835,79836,79837,79838,79839,79840,79841,79842,79843,79844,79845,79846,79847,79848,79849,79850,79851,79852,79853,79854,79855,79856,79857,79858,79859,79860,79861,79862,79863,79864,79865,79866,79867,79868,79869,79870,79871,79872,79873,79874,79875,79876,79877,79878,79879,79880,79881,79882,79883,79884,79885,79886,79887,79888,79889,79890,79891,79892,79893,79894,79895,79896,79897,79898,79899,79900,79901,79902,79903,79904,79905,79906,79907,79908,79909,79910,79911,79912,79913,79914,79915,79916,79917,79918,79919,79920,79921,79922,79923,79924,79925,79926,79927,79928,79929,79930,79931,79932,79933,79934,79935,79936,79937,79938,79939,79940,79941,79942,79943,79944,79945,79946,79947,79948,79949,79950,79951,79952,79953,79954,79955,79956,79957,79958,79959,79960,79961,79962,79963,79964,79965,79966,79967,79968,79969,79970,79971,79972,79973,79974,79975,79976,79977,79978,79979,79980,79981,79982,79983,79984,79985,79986,79987,79988,79989,79990,79991,79992,79993,79994,79995,79996,79997,79998,79999,80000,80001,80002,80003,80004,80005,80006,80007,80008,80009,80010,80011,80012,80013,80014,80015,80016,80017,80018,80019,80020,80021,80022,80023,80024,80025,80026,80027,80028,80029,80030,80031,80032,80033,80034,80035,80036,80037,80038,80039,80040,80041,80042,80043,80044,80045,80046,80047,80048,80049,80050,80051,80052,80053,80054,80055,80056,80057,80058,80059,80060,80061,80062,80063,80064,80065,80066,80067,80068,80069,80070,80071,80072,80073,80074,80075,80076,80077,80078,80079,80080,80081,80082,80083,80084,80085,80086,80087,80088,80089,80090,80091,80092,80093,80094,80095,80096,80097,80098,80099,80100,80101,80102,80103,80104,80105,80106,80107,80108,80109,80110,80111,80112,80113,80114,80115,80116,80117,80118,80119,80120,80121,80122,80123,80124,80125,80126,80127,80128,80129,80130,80131,80132,80133,80134,80135,80136,80137,80138,80139,80140,80141,80142,80143,80144,80145,80146,80147,80148,80149,80150,80151,80152,80153,80154,80155,80156,80157,80158,80159,80160,80161,80162,80163,80164,80165,80166,80167,80168,80169,80170,80171,80172,80173,80174,80175,80176,80177,80178,80179,80180,80181,80182,80183,80184,80185,80186,80187,80188,80189,80190,80191,80192,80193,80194,80195,80196,80197,80198,80199,80200,80201,80202,80203,80204,80205,80206,80207,80208,80209,80210,80211,80212,80213,80214,80215,80216,80217,80218,80219,80220,80221,80222,80223,80224,80225,80226,80227,80228,80229,80230,80231,80232,80233,80234,80235,80236,80237,80238,80239,80240,80241,80242,80243,80244,80245,80246,80247,80248,80249,80250,80251,80252,80253,80254,80255,80256,80257,80258,80259,80260,80261,80262,80263,80264,80265,80266,80267,80268,80269,80270,80271,80272,80273,80274,80275,80276,80277,80278,80279,80280,80281,80282,80283,80284,80285,80286,80287,80288,80289,80290,80291,80292,80293,80294,80295,80296,80297,80298,80299,80300,80301,80302,80303,80304,80305,80306,80307,80308,80309,80310,80311,80312,80313,80314,80315,80316,80317,80318,80319,80320,80321,80322,80323,80324,80325,80326,80327,80328,80329,80330,80331,80332,80333,80334,80335,80336,80337,80338,80339,80340,80341,80342,80343,80344,80345,80346,80347,80348,80349,80350,80351,80352,80353,80354,80355,80356,80357,80358,80359,80360,80361,80362,80363,80364,80365,80366,80367,80368,80369,80370,80371,80372,80373,80374,80375,80376,80377,80378,80379,80380,80381,80382,80383,80384,80385,80386,80387,80388,80389,80390,80391,80392,80393,80394,80395,80396,80397,80398,80399,80400,80401,80402,80403,80404,80405,80406,80407,80408,80409,80410,80411,80412,80413,80414,80415,80416,80417,80418,80419,80420,80421,80422,80423,80424,80425,80426,80427,80428,80429,80430,80431,80432,80433,80434,80435,80436,80437,80438,80439,80440,80441,80442,80443,80444,80445,80446,80447,80448,80449,80450,80451,80452,80453,80454,80455,80456,80457,80458,80459,80460,80461,80462,80463,80464,80465,80466,80467,80468,80469,80470,80471,80472,80473,80474,80475,80476,80477,80478,80479,80480,80481,80482,80483,80484,80485,80486,80487,80488,80489,80490,80491,80492,80493,80494,80495,80496,80497,80498,80499,80500,80501,80502,80503,80504,80505,80506,80507,80508,80509,80510,80511,80512,80513,80514,80515,80516,80517,80518,80519,80520,80521,80522,80523,80524,80525,80526,80527,80528,80529,80530,80531,80532,80533,80534,80535,80536,80537,80538,80539,80540,80541,80542,80543,80544,80545,80546,80547,80548,80549,80550,80551,80552,80553,80554,80555,80556,80557,80558,80559,80560,80561,80562,80563,80564,80565,80566,80567,80568,80569,80570,80571,80572,80573,80574,80575,80576,80577,80578,80579,80580,80581,80582,80583,80584,80585,80586,80587,80588,80589,80590,80591,80592,80593,80594,80595,80596,80597,80598,80599,80600,80601,80602,80603,80604,80605,80606,80607,80608,80609,80610,80611,80612,80613,80614,80615,80616,80617,80618,80619,80620,80621,80622,80623,80624,80625,80626,80627,80628,80629,80630,80631,80632,80633,80634,80635,80636,80637,80638,80639,80640,80641,80642,80643,80644,80645,80646,80647,80648,80649,80650,80651,80652,80653,80654,80655,80656,80657,80658,80659,80660,80661,80662,80663,80664,80665,80666,80667,80668,80669,80670,80671,80672,80673,80674,80675,80676,80677,80678,80679,80680,80681,80682,80683,80684,80685,80686,80687,80688,80689,80690,80691,80692,80693,80694,80695,80696,80697,80698,80699,80700,80701,80702,80703,80704,80705,80706,80707,80708,80709,80710,80711,80712,80713,80714,80715,80716,80717,80718,80719,80720,80721,80722,80723,80724,80725,80726,80727,80728,80729,80730,80731,80732,80733,80734,80735,80736,80737,80738,80739,80740,80741,80742,80743,80744,80745,80746,80747,80748,80749,80750,80751,80752,80753,80754,80755,80756,80757,80758,80759,80760,80761,80762,80763,80764,80765,80766,80767,80768,80769,80770,80771,80772,80773,80774,80775,80776,80777,80778,80779,80780,80781,80782,80783,80784,80785,80786,80787,80788,80789,80790,80791,80792,80793,80794,80795,80796,80797,80798,80799,80800,80801,80802,80803,80804,80805,80806,80807,80808,80809,80810,80811,80812,80813,80814,80815,80816,80817,80818,80819,80820,80821,80822,80823,80824,80825,80826,80827,80828,80829,80830,80831,80832,80833,80834,80835,80836,80837,80838,80839,80840,80841,80842,80843,80844,80845,80846,80847,80848,80849,80850,80851,80852,80853,80854,80855,80856,80857,80858,80859,80860,80861,80862,80863,80864,80865,80866,80867,80868,80869,80870,80871,80872,80873,80874,80875,80876,80877,80878,80879,80880,80881,80882,80883,80884,80885,80886,80887,80888,80889,80890,80891,80892,80893,80894,80895,80896,80897,80898,80899,80900,80901,80902,80903,80904,80905,80906,80907,80908,80909,80910,80911,80912,80913,80914,80915,80916,80917,80918,80919,80920,80921,80922,80923,80924,80925,80926,80927,80928,80929,80930,80931,80932,80933,80934,80935,80936,80937,80938,80939,80940,80941,80942,80943,80944,80945,80946,80947,80948,80949,80950,80951,80952,80953,80954,80955,80956,80957,80958,80959,80960,80961,80962,80963,80964,80965,80966,80967,80968,80969,80970,80971,80972,80973,80974,80975,80976,80977,80978,80979,80980,80981,80982,80983,80984,80985,80986,80987,80988,80989,80990,80991,80992,80993,80994,80995,80996,80997,80998,80999,81000,81001,81002,81003,81004,81005,81006,81007,81008,81009,81010,81011,81012,81013,81014,81015,81016,81017,81018,81019,81020,81021,81022,81023,81024,81025,81026,81027,81028,81029,81030,81031,81032,81033,81034,81035,81036,81037,81038,81039,81040,81041,81042,81043,81044,81045,81046,81047,81048,81049,81050,81051,81052,81053,81054,81055,81056,81057,81058,81059,81060,81061,81062,81063,81064,81065,81066,81067,81068,81069,81070,81071,81072,81073,81074,81075,81076,81077,81078,81079,81080,81081,81082,81083,81084,81085,81086,81087,81088,81089,81090,81091,81092,81093,81094,81095,81096,81097,81098,81099,81100,81101,81102,81103,81104,81105,81106,81107,81108,81109,81110,81111,81112,81113,81114,81115,81116,81117,81118,81119,81120,81121,81122,81123,81124,81125,81126,81127,81128,81129,81130,81131,81132,81133,81134,81135,81136,81137,81138,81139,81140,81141,81142,81143,81144,81145,81146,81147,81148,81149,81150,81151,81152,81153,81154,81155,81156,81157,81158,81159,81160,81161,81162,81163,81164,81165,81166,81167,81168,81169,81170,81171,81172,81173,81174,81175,81176,81177,81178,81179,81180,81181,81182,81183,81184,81185,81186,81187,81188,81189,81190,81191,81192,81193,81194,81195,81196,81197,81198,81199,81200,81201,81202,81203,81204,81205,81206,81207,81208,81209,81210,81211,81212,81213,81214,81215,81216,81217,81218,81219,81220,81221,81222,81223,81224,81225,81226,81227,81228,81229,81230,81231,81232,81233,81234,81235,81236,81237,81238,81239,81240,81241,81242,81243,81244,81245,81246,81247,81248,81249,81250,81251,81252,81253,81254,81255,81256,81257,81258,81259,81260,81261,81262,81263,81264,81265,81266,81267,81268,81269,81270,81271,81272,81273,81274,81275,81276,81277,81278,81279,81280,81281,81282,81283,81284,81285,81286,81287,81288,81289,81290,81291,81292,81293,81294,81295,81296,81297,81298,81299,81300,81301,81302,81303,81304,81305,81306,81307,81308,81309,81310,81311,81312,81313,81314,81315,81316,81317,81318,81319,81320,81321,81322,81323,81324,81325,81326,81327,81328,81329,81330,81331,81332,81333,81334,81335,81336,81337,81338,81339,81340,81341,81342,81343,81344,81345,81346,81347,81348,81349,81350,81351,81352,81353,81354,81355,81356,81357,81358,81359,81360,81361,81362,81363,81364,81365,81366,81367,81368,81369,81370,81371,81372,81373,81374,81375,81376,81377,81378,81379,81380,81381,81382,81383,81384,81385,81386,81387,81388,81389,81390,81391,81392,81393,81394,81395,81396,81397,81398,81399,81400,81401,81402,81403,81404,81405,81406,81407,81408,81409,81410,81411,81412,81413,81414,81415,81416,81417,81418,81419,81420,81421,81422,81423,81424,81425,81426,81427,81428,81429,81430,81431,81432,81433,81434,81435,81436,81437,81438,81439,81440,81441,81442,81443,81444,81445,81446,81447,81448,81449,81450,81451,81452,81453,81454,81455,81456,81457,81458,81459,81460,81461,81462,81463,81464,81465,81466,81467,81468,81469,81470,81471,81472,81473,81474,81475,81476,81477,81478,81479,81480,81481,81482,81483,81484,81485,81486,81487,81488,81489,81490,81491,81492,81493,81494,81495,81496,81497,81498,81499,81500,81501,81502,81503,81504,81505,81506,81507,81508,81509,81510,81511,81512,81513,81514,81515,81516,81517,81518,81519,81520,81521,81522,81523,81524,81525,81526,81527,81528,81529,81530,81531,81532,81533,81534,81535,81536,81537,81538,81539,81540,81541,81542,81543,81544,81545,81546,81547,81548,81549,81550,81551,81552,81553,81560,81561,81562,81563,81564,81565,81566,81567,81568,81569,81570,81571,81572,81573,81574,81575,81576,81577,81578,81579,81580,81581,81582,81583,81584,81585,81586,81587,81588,81589,81590,81591,81594,81595,81596,81597,81598,81599,81600,81601,81602,81603,81608,81609,81610,81611,81612,81613,81614,81615,81616,81617,81622,81623,81624,81625,81626,81627,81628,81629,81630,81631,81636,81637,81638,81639,81640,81641,81642,81643,81644,81645,81646,81647,81665,81666,81667,81668,81669,81670,81671,81672,81673,81674,81675,81676,81677,81678,81679,81680,81681,81682,81683,81684,81685,81686,81687,81688,81689,81690,81691,81692,81693,81694,81695,81696,81697,81698,81699,81700,81701,81702,81703,81704,81705,81706,81707,81708,81709,81710,81711,81712,81713,81714,81715,81716,81717,81718,81719,81720,81721,81722,81723,81724,81725,81726,81727,81728,81729,81730,81731,81732,81733,81734,81737,81738,81739,81740,81741,81742,81743,81744,81745,81746,81747,81748,81749,81750,81751,81752,81765,81766,81767,81768,81788,81809,81810,81811,81812,81813,81814,81827,81828,81829,81830,81831,81832,81833,81834,81835,81836,81837,81838,81839,81840,81841,81842,81843,81844,81845,81846,81847,81848,81849,81850,81851,81852,81853,81854,81855,81856,81857,81858,81859,81860,81861,81862,81863,81864,81865,81866,81867,81868,81869,81870,81871,81872,81873,81874,81875,81876,81877,81878,81879,81880,81881,81882,81883,81884,81885,81886,81887,81888,81889,81890,81891,81892,81893,81894,81895,81896,81897,81898,81899,81900,81901,81902,81903,81904,81905,81906,81995,81996,81997,81998,81999,82000,82001,82002,82003,82004,82005,82006,82007,82008,82009,82010,82011,82012,82013,82014,82015,82016,82017,82018,82019,82020,82021,82022,82023,82024,82025,82026,82027,82028,82029,82030,82031,82032,82033,82034,82035,82036,82037,82038,82039,82040,82041,82042,82043,82044,82045,82046,82047,82048,82049,82050,82051,82052,82053,82054,82055,82056,82057,82058,82059,82060,82061,82062,82063,82064,82065,82066,82067,82068,82069,82070,82071,82072,82073,82074,82075,82076,82077,82078,82079,82080,82081,82082,82083,82084,82085,82086,82087,82088,82089,82090,82091,82092,82093,82094,82095,82096,82097,82098,82099,82100,82101,82102,82103,82104,82105,82106,82107,82108,82109,82110,82111,82112,82113,82114,82115,82116,82117,82118,82119,82120,82121,82122,82123,82124,82125,82126,82127,82128,82129,82132,82133,82134,82135,82136,82137,82138,82139,82140,82141,82142,82143,82144,82145,82146,82147,82150,82151,82152,82153,82154,82155,82156,82167,82168,82179,82180,82181,82182,82203,82204,82205,82206,82207,82208,82209,82210,82211,82212,82213,82231,82232,82233,82234,82235,82236,82237,82238,82239,82240,82241,82242,82243,82244,82245,82246,82247,82248,82249,82250,82251,82252,82253,82254,82255,82256,82257,82258,82259,82260,82261,82262,82263,82264,82265,82266,82267,82268,82269,82270,82271,82272,82273,82274,82275,82276,82356,};
	cout << "leftArm: " << (sizeof(faceID) / sizeof(DWORD)) << endl;
	for (int i = 0; i < (int) sizeof(faceID) / sizeof(DWORD); ++i) {
		face_id_set.insert(faceID[i]);
	}
	__defined_area_dic[AREA_LEFT_ARM] = defArea;
} while (0);
do {	// 오른쪽 팔
	kArea::DEFINED_AREA_DIC::iterator	it;
	it = __defined_area_dic.find(AREA_RIGHT_ARM);
	if (it != __defined_area_dic.end())
		__defined_area_dic.erase(it);

	kArea::DEFINED_AREA	defArea;
	defArea.value = .9f;	// 원하는 색깔이...
	std::set<DWORD>&	face_id_set = defArea.face_id_set;
	DWORD	faceID[] = {58566,58567,58576,58577,58585,58586,58587,58588,58589,58590,58591,58592,58664,58665,58666,58667,58668,58669,58670,58671,58680,58681,58682,58683,58695,58696,58743,58744,58745,58746,58751,58752,58753,58754,58783,58784,58787,58788,58791,58792,58795,58796,58797,58798,58799,58800,58801,58802,58827,58828,58834,58835,58887,58888,58889,58890,58896,58897,58910,58911,58986,58987,58993,58994,59004,59005,59006,59007,59008,59009,59025,59026,59027,59028,59029,59030,59031,59032,59033,59036,59037,59038,59039,59040,59041,59042,59043,59055,59060,59061,59062,59063,59064,59071,59072,59073,59074,59075,59076,59112,59266,59267,59276,59277,59288,59289,59290,59297,59318,59319,59320,59321,59342,59343,59344,59345,59356,59357,59358,59359,59474,59487,59489,59490,59502,59503,59504,59505,59546,59547,59692,59693,59694,59695,59698,59699,59700,59701,59702,59703,59704,59705,59706,59707,59708,59709,59710,59711,59712,59713,59714,59715,59716,59717,59718,59719,59720,59721,59722,59723,59724,59725,59726,59727,59728,59729,59730,59731,59732,59733,59734,59735,59736,59737,59738,59739,59740,59741,59742,59743,59744,59745,59746,59747,59748,59749,59750,59751,59752,59753,59754,59755,59756,59757,59758,59759,59760,59761,59762,59763,59764,59765,59766,59767,59768,59769,59770,59771,59772,59773,59774,59775,59776,59777,59778,59779,59780,59781,59782,59783,59784,59785,59786,59787,59788,59789,59790,59791,59792,59793,59794,59795,59796,59797,59798,59799,59800,59801,59802,59803,59804,59805,59806,59807,59808,59809,59810,59811,59812,59813,59814,59815,59816,59817,59818,59819,59820,59821,59822,59823,59824,59825,59826,59827,59828,59829,59830,59831,59832,59833,59834,59835,59836,59837,59838,59839,59840,59841,59842,59843,59844,59845,59846,59847,59848,59849,59850,59851,59852,59853,59854,59855,59856,59857,59858,59859,59860,59861,59862,59863,59864,59865,59866,59867,59868,59869,59870,59871,59872,59873,59874,59875,59876,59877,59878,59879,59880,59881,59882,59883,59884,59885,59886,59887,59888,59889,59890,59891,59892,59893,59894,59895,59896,59897,59898,59899,59900,59901,59902,59903,59904,59905,59906,59907,59908,59909,59910,59911,59912,59913,59914,59915,59916,59917,59918,59919,59920,59921,59922,59923,59924,59925,59926,59927,59928,59929,59930,59931,59932,59933,59934,59935,59936,59937,59938,59939,59940,59941,59942,59943,59944,59945,59946,59947,59948,59949,59950,59951,59952,59953,59954,59955,59956,59957,59958,59959,59960,59961,59962,59963,59964,59965,59966,59967,59968,59969,59970,59971,59972,59973,59974,59975,59976,59977,59978,59979,59980,59981,59982,59983,59984,59985,59986,59987,59988,59989,59990,59991,59992,59993,59994,59995,59996,59997,59998,59999,60000,60001,60002,60003,60004,60005,60006,60007,60008,60009,60010,60011,60012,60013,60014,60015,60016,60017,60018,60019,60020,60021,60022,60023,60024,60025,60026,60027,60028,60029,60030,60031,60032,60033,60034,60035,60036,60037,60038,60039,60040,60041,60042,60043,60044,60045,60046,60047,60048,60049,60050,60051,60052,60053,60054,60055,60056,60057,60058,60059,60060,60061,60062,60063,60064,60065,60066,60067,60068,60069,60070,60071,60072,60073,60074,60075,60076,60077,60078,60079,60080,60081,60082,60083,60084,60085,60086,60087,60088,60089,60090,60091,60092,60093,60094,60095,60096,60097,60098,60099,60100,60101,60102,60103,60104,60105,60106,60107,60108,60109,60110,60111,60112,60113,60114,60115,60116,60117,60118,60119,60120,60121,60122,60123,60124,60125,60126,60127,60128,60129,60130,60131,60132,60133,60134,60135,60136,82357,82358,82359,82360,82361,82362,82363,82364,82365,82366,82367,82368,82369,82370,82371,82372,82373,82374,82375,82376,82377,82378,82379,82380,82381,82382,82383,82384,82385,82386,82387,82388,82389,82390,82391,82392,82393,82394,82395,82396,82397,82398,82399,82400,82401,82402,82403,82404,82405,82406,82407,82408,82409,82410,82411,82412,82413,82414,82415,82416,82417,82418,82419,82420,82421,82422,82423,82424,82425,82426,82427,82428,82429,82430,82431,82432,82433,82434,82435,82436,82437,82438,82439,82440,82441,82442,82443,82444,82445,82446,82447,82448,82449,82450,82451,82452,82453,82454,82455,82456,82457,82458,82459,82460,82461,82462,82463,82464,82465,82466,82467,82468,82469,82470,82471,82472,82473,82474,82475,82476,82477,82478,82479,82480,82481,82482,82483,82484,82485,82486,82487,82488,82489,82490,82491,82492,82493,82494,82495,82496,82497,82498,82499,82500,82501,82502,82503,82504,82505,82506,82507,82508,82509,82510,82511,82512,82513,82514,82515,82516,82517,82518,82519,82520,82521,82522,82523,82524,82525,82526,82527,82528,82529,82530,82531,82532,82533,82534,82535,82536,82537,82538,82539,82540,82541,82542,82543,82544,82545,82546,82547,82548,82549,82550,82551,82552,82553,82554,82555,82556,82557,82558,82559,82560,82561,82562,82563,82564,82565,82566,82567,82568,82569,82570,82571,82572,82573,82574,82575,82576,82577,82578,82579,82580,82581,82582,82583,82584,82585,82586,82587,82588,82589,82590,82591,82592,82593,82594,82595,82596,82597,82598,82599,82600,82601,82602,82603,82604,82605,82606,82607,82608,82609,82610,82611,82612,82613,82614,82615,82616,82617,82618,82619,82620,82621,82622,82623,82624,82625,82626,82627,82628,82629,82630,82631,82632,82633,82634,82635,82636,82637,82638,82639,82640,82641,82642,82643,82644,82645,82646,82647,82648,82649,82650,82651,82652,82653,82654,82655,82656,82657,82658,82659,82660,82661,82662,82663,82664,82665,82666,82667,82668,82669,82670,82671,82672,82673,82674,82675,82676,82677,82678,82679,82680,82681,82682,82683,82684,82685,82686,82687,82688,82689,82690,82691,82692,82693,82694,82695,82696,82697,82698,82699,82700,82701,82702,82703,82704,82705,82706,82707,82708,82709,82710,82711,82712,82713,82714,82715,82716,82717,82718,82719,82720,82721,82722,82723,82724,82725,82726,82727,82728,82729,82730,82731,82732,82733,82734,82735,82736,82737,82738,82739,82740,82741,82742,82743,82744,82745,82746,82747,82748,82749,82750,82751,82752,82753,82754,82755,82756,82757,82758,82759,82760,82761,82762,82763,82764,82765,82766,82767,82768,82769,82770,82771,82772,82773,82774,82775,82776,82777,82778,82779,82780,82781,82782,82783,82784,82785,82786,82787,82788,82789,82790,82791,82792,82793,82794,82795,82796,82797,82798,82799,82800,82801,82802,82803,82804,82805,82806,82807,82808,82809,82810,82811,82812,82813,82814,82815,82816,82817,82818,82819,82820,82821,82822,82823,82824,82825,82826,82827,82828,82829,82830,82831,82832,82833,82834,82835,82836,82837,82838,82839,82840,82841,82842,82843,82844,82845,82846,82847,82848,82849,82850,82851,82852,82853,82854,82855,82856,82857,82858,82859,82860,82861,82862,82863,82864,82865,82866,82867,82868,82869,82870,82871,82872,82873,82874,82875,82876,82877,82878,82879,82880,82881,82882,82883,82884,82885,82886,82887,82888,82889,82890,82891,82892,82893,82894,82895,82896,82897,82898,82899,82900,82901,82902,82903,82904,82905,82906,82907,82908,82909,82910,82911,82912,82913,82914,82915,82916,82917,82918,82919,82920,82921,82922,82923,82924,82925,82926,82927,82928,82929,82930,82931,82932,82933,82934,82935,82936,82937,82938,82939,82940,82941,82942,82943,82944,82945,82946,82947,82948,82949,82950,82951,82952,82953,82954,82955,82956,82957,82958,82959,82960,82961,82962,82963,82964,82965,82966,82967,82968,82969,82970,82971,82972,82973,82974,82975,82976,82977,82978,82979,82980,82981,82982,82983,82984,82985,82986,82987,82988,82989,82990,82991,82992,82993,82994,82995,82996,82997,82998,82999,83000,83001,83002,83003,83004,83005,83006,83007,83008,83009,83010,83011,83012,83013,83014,83015,83016,83017,83018,83019,83020,83021,83022,83023,83024,83025,83026,83027,83028,83029,83030,83031,83032,83033,83034,83035,83036,83037,83038,83039,83040,83041,83042,83043,83044,83045,83046,83047,83048,83049,83050,83051,83052,83053,83054,83055,83056,83057,83058,83059,83060,83061,83062,83063,83064,83065,83066,83067,83068,83069,83070,83071,83072,83073,83074,83075,83076,83077,83078,83079,83080,83081,83082,83083,83084,83085,83086,83087,83088,83089,83090,83091,83092,83093,83094,83095,83096,83097,83098,83099,83100,83101,83102,83103,83104,83105,83106,83107,83108,83109,83110,83111,83112,83113,83114,83115,83116,83117,83118,83119,83120,83121,83122,83123,83124,83125,83126,83127,83128,83129,83130,83131,83132,83133,83134,83135,83136,83137,83138,83139,83140,83141,83142,83143,83144,83145,83146,83147,83148,83149,83150,83151,83152,83153,83154,83155,83156,83157,83158,83159,83160,83161,83162,83163,83164,83165,83166,83167,83168,83169,83170,83171,83172,83173,83174,83175,83176,83177,83178,83179,83180,83181,83182,83183,83184,83185,83186,83187,83188,83189,83190,83191,83192,83193,83194,83195,83196,83197,83198,83199,83200,83201,83202,83203,83204,83205,83206,83207,83208,83209,83210,83211,83212,83213,83214,83215,83216,83217,83218,83219,83220,83221,83222,83223,83224,83225,83226,83227,83228,83229,83230,83231,83232,83233,83234,83235,83236,83237,83238,83239,83240,83241,83242,83243,83244,83245,83246,83247,83248,83249,83250,83251,83252,83253,83254,83255,83256,83257,83258,83259,83260,83261,83262,83263,83264,83265,83266,83267,83268,83269,83270,83271,83272,83273,83274,83275,83276,83277,83278,83279,83280,83281,83282,83283,83284,83285,83286,83287,83288,83289,83290,83291,83292,83293,83294,83295,83296,83297,83298,83299,83300,83301,83302,83303,83304,83305,83306,83307,83308,83309,83310,83311,83312,83313,83314,83315,83316,83317,83318,83319,83320,83321,83322,83323,83324,83325,83326,83327,83328,83329,83330,83331,83332,83333,83334,83335,83336,83337,83338,83339,83340,83341,83342,83343,83344,83345,83346,83347,83348,83349,83350,83351,83352,83353,83354,83355,83356,83357,83358,83359,83360,83361,83362,83363,83364,83365,83366,83367,83368,83369,83370,83371,83372,83373,83374,83375,83376,83377,83378,83379,83380,83381,83382,83383,83384,83385,83386,83387,83388,83389,83390,83391,83392,83393,83394,83395,83396,83397,83398,83399,83400,83401,83402,83403,83404,83405,83406,83407,83408,83409,83410,83411,83412,83413,83414,83415,83416,83417,83418,83419,83420,83421,83422,83423,83424,83425,83426,83427,83428,83429,83430,83431,83432,83433,83434,83435,83436,83437,83438,83439,83440,83441,83442,83443,83444,83445,83446,83447,83448,83449,83450,83451,83452,83453,83454,83455,83456,83457,83458,83459,83460,83461,83462,83463,83464,83465,83466,83467,83468,83469,83470,83471,83472,83473,83474,83475,83476,83477,83478,83479,83480,83481,83482,83483,83484,83485,83486,83487,83488,83489,83490,83491,83492,83493,83494,83495,83496,83497,83498,83499,83500,83501,83502,83503,83504,83505,83506,83507,83508,83509,83510,83511,83512,83513,83514,83515,83516,83517,83518,83519,83520,83521,83522,83523,83524,83525,83526,83527,83528,83529,83530,83531,83532,83533,83534,83535,83536,83537,83538,83539,83540,83541,83542,83543,83544,83545,83546,83547,83548,83549,83550,83551,83552,83553,83554,83555,83556,83557,83558,83559,83560,83561,83562,83563,83564,83565,83566,83567,83568,83569,83570,83571,83572,83573,83574,83575,83576,83577,83578,83579,83580,83581,83582,83583,83584,83585,83586,83587,83588,83589,83590,83591,83592,83593,83594,83595,83596,83597,83598,83599,83600,83601,83602,83603,83604,83605,83606,83607,83608,83609,83610,83611,83612,83613,83614,83615,83616,83617,83618,83619,83620,83621,83622,83623,83624,83625,83626,83627,83628,83629,83630,83631,83632,83633,83634,83635,83636,83637,83638,83639,83640,83641,83642,83643,83644,83645,83646,83647,83648,83649,83650,83651,83652,83653,83654,83655,83656,83657,83658,83659,83660,83661,83662,83663,83664,83665,83666,83667,83668,83669,83670,83671,83672,83673,83674,83675,83676,83677,83678,83679,83680,83681,83682,83683,83684,83685,83686,83687,83688,83689,83690,83691,83692,83693,83694,83695,83696,83697,83698,83699,83700,83701,83702,83703,83704,83705,83706,83707,83708,83709,83710,83711,83712,83713,83714,83715,83716,83717,83718,83719,83720,83721,83722,83723,83724,83725,83726,83727,83728,83729,83730,83731,83732,83733,83734,83735,83736,83737,83738,83739,83740,83741,83742,83743,83744,83745,83746,83747,83748,83749,83750,83751,83752,83753,83754,83755,83756,83757,83758,83759,83760,83761,83762,83763,83764,83765,83766,83767,83768,83769,83770,83771,83772,83773,83774,83775,83776,83777,83778,83779,83780,83781,83782,83783,83784,83785,83786,83787,83788,83789,83790,83791,83792,83793,83794,83795,83796,83797,83798,83799,83800,83801,83802,83803,83804,83805,83806,83807,83808,83809,83810,83811,83812,83813,83814,83815,83816,83817,83818,83819,83820,83821,83822,83823,83824,83825,83826,83827,83828,83829,83830,83831,83832,83833,83834,83835,83836,83837,83838,83839,83840,83841,83842,83843,83844,83845,83846,83847,83848,83849,83850,83851,83852,83853,83854,83855,83856,83857,83858,83859,83860,83861,83862,83863,83864,83865,83866,83867,83868,83869,83870,83871,83872,83873,83874,83875,83876,83877,83878,83879,83880,83881,83882,83883,83884,83885,83886,83887,83888,83889,83890,83891,83892,83893,83894,83895,83896,83897,83898,83899,83900,83901,83902,83903,83904,83905,83906,83907,83908,83909,83910,83911,83912,83913,83914,83915,83916,83917,83918,83919,83920,83921,83922,83923,83924,83925,83926,83927,83928,83929,83930,83931,83932,83933,83934,83935,83936,83937,83938,83939,83940,83941,83942,83943,83944,83945,83946,83947,83948,83949,83950,83951,83952,83953,83954,83955,83956,83957,83958,83959,83960,83961,83962,83963,83964,83965,83966,83967,83968,83969,83970,83971,83972,83973,83974,83975,83976,83977,83978,83979,83980,83981,83982,83983,83984,83985,83986,83987,83988,83989,83990,83991,83992,83993,83994,83995,83996,83997,83998,83999,84000,84001,84002,84003,84004,84005,84006,84007,84008,84009,84010,84011,84012,84013,84014,84015,84016,84017,84018,84019,84020,84021,84022,84023,84024,84025,84026,84027,84028,84029,84030,84031,84032,84033,84034,84035,84036,84037,84038,84039,84040,84041,84042,84043,84044,84045,84046,84047,84048,84049,84050,84051,84052,84053,84054,84055,84056,84057,84058,84059,84060,84061,84062,84063,84064,84065,84066,84067,84068,84069,84070,84071,84072,84073,84074,84075,84076,84077,84078,84079,84080,84081,84082,84083,84084,84085,84086,84087,84088,84089,84090,84091,84092,84093,84094,84095,84096,84097,84098,84099,84100,84101,84102,84103,84104,84105,84106,84107,84108,84109,84110,84111,84112,84113,84114,84115,84116,84117,84118,84119,84120,84121,84122,84123,84124,84125,84126,84127,84128,84129,84130,84131,84132,84133,84134,84135,84136,84137,84138,84139,84140,84141,84142,84143,84144,84145,84146,84147,84148,84149,84150,84151,84152,84153,84154,84155,84156,84157,84158,84159,84160,84161,84162,84163,84164,84165,84166,84167,84168,84169,84170,84171,84172,84173,84174,84175,84176,84177,84178,84179,84180,84181,84182,84183,84184,84185,84186,84187,84188,84189,84190,84191,84192,84193,84194,84195,84196,84197,84198,84199,84200,84201,84202,84203,84204,84205,84206,84207,84208,84209,84210,84211,84212,84213,84214,84215,84216,84217,84218,84219,84220,84221,84222,84223,84224,84225,84226,84227,84228,84229,84230,84231,84232,84233,84234,84235,84236,84237,84238,84239,84240,84241,84242,84243,84244,84245,84246,84247,84248,84249,84250,84251,84252,84253,84254,84255,84256,84257,84258,84259,84260,84261,84262,84263,84264,84265,84266,84267,84268,84269,84270,84271,84272,84273,84274,84275,84276,84277,84278,84279,84280,84281,84282,84283,84284,84285,84286,84287,84288,84289,84290,84291,84292,84293,84294,84295,84296,84297,84298,84299,84300,84301,84302,84303,84304,84305,84306,84307,84308,84309,84310,84311,84312,84313,84314,84315,84316,84317,84318,84319,84320,84321,84322,84323,84324,84325,84326,84327,84328,84329,84330,84331,84332,84333,84334,84335,84336,84337,84338,84339,84340,84341,84342,84343,84344,84345,84346,84347,84348,84349,84350,84351,84352,84353,84354,84355,84356,84357,84358,84359,84360,84361,84362,84363,84364,84365,84366,84367,84368,84369,84370,84371,84372,84373,84374,84375,84376,84377,84378,84379,84380,84381,84382,84383,84384,84385,84386,84387,84388,84389,84390,84391,84392,84393,84394,84395,84396,84397,84398,84399,84400,84401,84402,84403,84404,84405,84406,84407,84408,84409,84410,84411,84412,84413,84414,84415,84416,84417,84418,84419,84420,84421,84422,84423,84424,84425,84426,84427,84428,84429,84430,84431,84432,84433,84434,84435,84436,84437,84438,84439,84440,84441,84442,84443,84444,84445,84499,84500,84501,84502,84503,84504,84505,84506,84516,84517,84518,84519,84521,84522,84523,84524,84526,84527,84528,84532,84533,84534,84535,84536,84537,84538,84539,84540,84541,84542,84543,84544,84545,84546,84547,84548,84549,84550,84551,84552,84553,84554,84555,84556,84557,84558,84559,84560,84561,84562,84563,84564,84565,84566,84567,84568,84569,84570,84571,84572,84573,84574,84575,84576,84577,84578,84579,84580,84581,84582,84583,84584,84585,84586,84587,84588,84589,84590,84591,84592,84593,84594,84595,84596,84597,84598,84599,84600,84601,84602,84603,84604,84605,84606,84607,84608,84609,84610,84611,84612,84613,84614,84615,84616,84617,84618,84619,84620,84621,84622,84623,84624,84625,84626,84627,84628,84629,84630,84631,84632,84633,84634,84635,84636,84637,84638,84639,84640,84641,84642,84643,84644,84645,84646,84647,84648,84649,84650,84651,84652,84653,84654,84655,84656,84657,84658,84659,84660,84661,84662,84663,84664,84665,84666,84667,84668,84669,84670,84671,84672,84673,84674,84675,84676,84677,84678,84679,84680,84681,84682,84683,84684,84685,84686,84687,84688,84689,84690,84691,84692,84693,84694,84695,84696,84697,84700,84701,84702,84703,84704,84705,84706,84707,84708,84709,84710,84711,84712,84713,84718,84719,84720,84721,84722,84723,84724,84725,84726,84727,84728,84729,84734,84735,84736,84737,84738,84739,84740,84741,84742,84743,84744,84745,84746,84747,84748,84749,84750,84751,84752,84753,84754,84755,84756,84757,84758,84763,84764,84765,84766,84767,84768,84769,84770,84771,84772,84773,84774,84775,84776,84777,84778,84779,84780,84781,84782,84783,84784,84785,84786,84787,84788,84789,84790,84791,84792,84793,84794,84795,84796,84797,84798,84799,84800,84801,84802,84803,84804,84805,84806,84807,84808,84809,84810,84811,84812,84813,84814,84815,84816,84821,84822,84829,84830,84835,84836,84839,84840,84843,84844,84846,84847,84848,84853,84855,84856,84858,84859,84864,84865,84868,84869,84870,84871,84874,84875,84876,84877,84878,84881,84882,84883,84884,84895,};
	cout << "rightArm: " << (sizeof(faceID) / sizeof(DWORD)) << endl;
	for (int i = 0; i < (int) sizeof(faceID) / sizeof(DWORD); ++i) {
		face_id_set.insert(faceID[i]);
	}
	__defined_area_dic[AREA_RIGHT_ARM] = defArea;
} while (0);
do {	// 왼쪽 다리
	kArea::DEFINED_AREA_DIC::iterator	it;
	it = __defined_area_dic.find(AREA_LEFT_LEG);
	if (it != __defined_area_dic.end())
		__defined_area_dic.erase(it);

	kArea::DEFINED_AREA	defArea;
	defArea.value = .9f;	// 원하는 색깔이...
	std::set<DWORD>&	face_id_set = defArea.face_id_set;
	DWORD	faceID[] = {62360,62361,62386,62387,62388,62433,62434,62442,62443,62444,62445,62446,62447,62448,62449,62450,62451,62452,62544,62545,62546,62547,62548,62549,62550,62551,62554,62555,62556,62557,62558,62559,62560,62561,62562,62563,62564,62565,62566,62567,62568,62569,62570,62571,62572,62573,62574,62575,62576,62577,62578,62579,62580,62581,62582,62583,62584,62587,62588,62589,62590,62599,62600,62601,62602,62603,62604,62605,62606,62607,62608,62609,62612,62613,62614,62615,62616,62617,62618,62619,62622,62623,62624,62625,62626,62627,62628,62629,62630,62631,62632,62633,62634,62635,62636,62637,62638,62639,62640,62641,62642,62643,62644,62645,62646,62647,62648,63084,63085,63110,63111,63112,63157,63158,63166,63167,63168,63169,63170,63171,63172,63173,63174,63175,63176,63268,63269,63270,63271,63272,63273,63274,63275,63278,63279,63280,63281,63282,63283,63284,63285,63286,63287,63288,63289,63290,63291,63292,63293,63294,63295,63296,63297,63298,63299,63300,63301,63302,63303,63304,63305,63306,63307,63308,63311,63312,63313,63314,63323,63324,63325,63326,63327,63328,63329,63330,63331,63332,63333,63336,63337,63338,63339,63340,63341,63342,63343,63346,63347,63348,63349,63350,63351,63352,63353,63354,63355,63356,63357,63358,63359,63360,63361,63362,63363,63364,63365,63366,63367,63368,63369,63370,63371,63372,63612,63613,63614,63615,63616,63617,63618,63619,63620,63621,63622,63623,63624,63625,63626,63627,63628,63629,63630,63631,63632,63633,63634,63635,63636,63637,63638,63639,63640,63641,63642,63643,63644,63645,63646,63647,63648,63649,63650,63651,63652,63653,63654,63655,63656,63657,63658,63659,63660,63661,63662,63663,63664,63665,63666,63667,63668,63669,63670,63671,63672,63673,63674,63675,63676,63677,63678,63679,63680,63681,63682,63683,63686,63687,63729,63734,63735,63738,63739,63749,63750,63751,63756,63757,63758,63759,63760,63761,63762,63763,63764,63765,63766,63767,63768,63769,63770,63771,63772,63773,63774,63775,63776,63777,63778,63779,63784,63785,63786,63787,63790,63792,63793,63794,63795,63796,63797,63798,63799,63800,63801,63802,63803,63804,63805,63806,63807,63808,63809,63810,63811,63812,63813,63814,63815,63816,63817,63818,63819,63820,63821,63822,63823,63824,63825,63826,63827,63828,63829,63830,63831,63832,63833,63834,63835,63836,63837,63838,63839,63840,63841,63842,63843,63844,63845,63846,63847,63848,63849,63850,63851,63852,63853,63854,63855,63856,63857,63858,63859,63860,63861,63862,63863,63864,63865,63866,63867,63868,63869,63870,63873,63874,63875,63876,63877,63878,63879,63880,63881,63882,63883,63884,63885,63886,63890,63891,63892,63893,63894,63895,63898,63899,63900,63901,63904,63905,63917,63918,63919,63920,63921,63922,63923,63924,63925,63958,63959,63961,63962,63965,63966,63967,63968,63975,63976,63977,63978,63979,63980,63981,63982,63983,63984,63985,63986,63987,63988,63989,63990,63997,63998,63999,64000,64001,64002,64003,64004,64005,64006,64007,64008,64009,64010,64011,64012,64036,64037,64038,64039,64040,64041,64042,64043,64044,64045,64046,64065,64066,64070,64071,64072,64073,64074,64075,64102,64103,64104,64105,64106,64107,64108,64109,64116,64117,64118,64119,64120,64121,64130,64131,64132,64133,64135,64140,64141,64144,64145,64161,64162,64171,64181,64182,64183,64184,64185,64186,64187,64188,64193,64194,64195,64196,64197,64198,64199,64200,64201,64202,64203,64204,64205,64206,64207,64208,64209,64210,64211,64212,64213,64214,64215,64216,64217,64218,64367,64368,64369,64370,64371,64372,64373,64374,64375,64376,64377,64378,64379,64380,64381,64382,64383,64384,64385,64386,64387,64388,64389,64390,64391,64392,64393,64394,64395,64396,64399,64401,64402,64403,64404,64406,64407,64408,64409,64410,64411,64417,64426,64427,64428,64429,64430,64431,64434,64435,64436,64437,64438,64439,64440,64441,64442,64443,64444,64445,64447,64448,64449,64450,64451,64452,64453,64454,64455,64456,64457,64458,64459,64460,64461,64462,64463,64464,64465,64466,64467,64468,64469,64470,64471,64472,64473,64474,64528,64529,64530,64531,64548,64549,64550,64551,64552,64553,64554,64555,64556,64557,64558,64559,64560,64561,64562,64563,64564,64565,64566,64567,64568,64569,64575,64576,64577,64578,64579,64580,64581,64582,64583,64584,64585,64586,64587,64588,64589,64590,64591,64592,64593,64594,64595,64596,64597,64598,64599,64600,64601,64602,64603,64604,64605,64606,64607,64608,64609,64610,64611,64612,64613,64614,64615,64616,64617,64618,64619,64620,64621,64622,64623,64624,64625,64626,64627,64628,64629,64630,64631,64660,64661,64662,64663,64664,64665,64666,64667,64668,64669,64670,64671,64672,64673,64674,64675,64676,64678,64679,64680,64681,64682,64683,64684,64685,64686,64687,64688,64689,64690,64699,64700,64701,64702,64703,64704,64705,64706,64707,64708,64709,64710,64711,64712,64713,64714,64715,64716,64717,64718,64721,64723,64724,64725,64726,64729,64743,64744,64745,64746,64747,64748,64749,64750,64751,64752,64753,64754,64755,64756,64757,64758,64759,64760,64761,64762,64763,64764,64765,64766,64767,64768,64769,64770,64771,64772,64773,64774,64775,64776,64777,64778,64779,64780,64781,64782,64783,64784,64785,64786,64787,64788,64789,64790,64791,64792,64793,64794,64795,64796,64797,64798,64799,64800,64801,64802,64803,64804,64805,64806,64807,64808,64809,64810,64812,64813,64814,64815,64816,64817,64818,64819,64820,64821,64822,64823,64824,64825,64826,64827,64828,64829,64830,64831,64832,64833,64834,64835,64836,64837,64838,64839,64840,64841,64842,64843,64844,64845,64846,64847,64848,64849,64850,64851,64852,64853,64854,64856,64857,64858,64863,64864,64871,64872,64873,64874,64875,64876,64881,64882,64883,64884,64885,64886,64887,64888,64889,64890,64891,64892,64893,64908,64909,64910,64911,64916,64917,64918,64919,64920,64921,64922,64923,64924,64925,64926,64945,64946,64953,64954,64957,64958,64959,64960,64961,64962,65150,65151,65183,65184,65185,65186,65187,65188,65193,65194,65195,65196,65197,65198,65199,65200,65201,65202,65203,65226,65227,65228,65229,65230,65231,65234,65235,65236,65237,65238,65245,65246,65247,65248,65249,65250,65269,65270,65275,65276,65279,65280,65283,65284,65285,65286,65287,65288,65289,65290,65291,65292,65299,65300,65304,65324,65382,65385,65386,65497,65498,65540,65545,65546,65728,65729,65730,65731,65732,65733,65736,65769,65770,65772,65773,65776,65777,65786,65787,65788,65789,65790,65791,65792,65793,65814,65815,65816,65817,65818,65819,65820,65821,65822,65823,65847,65848,65849,65850,65851,65852,65853,65854,65855,65856,65876,65877,65881,65882,65883,65884,65885,65886,66255,66256,66269,66270,66373,66374,66375,66376,66637,66638,66639,66640,66641,66704,66735,67193,67196,67197,68800,68801,68802,68803,68804,68805,68806,68807,68808,68809,68810,68811,68812,68813,68814,68815,68816,68817,68818,68819,68820,68821,68822,68823,68824,68825,68826,68827,68828,68829,68830,68831,68832,68833,68834,68835,68836,68837,68838,68839,68840,68841,68842,68843,68844,68845,68846,68847,68848,68849,68850,68851,68852,68853,68854,68855,68856,68857,68858,68859,68860,68861,68862,68863,68864,68865,68866,68867,68868,68869,68870,68871,68872,68873,68874,68875,68876,68877,68878,68879,68880,68881,68882,68883,68884,68885,68886,68887,68888,68889,68890,68891,68892,68893,68894,68895,68896,68897,68898,68899,68900,68901,68902,68903,68904,68905,68906,68907,68908,68909,68910,68911,68912,68913,68914,68915,68916,68917,68918,68919,68920,68921,68922,68923,68924,68925,68926,68927,68928,68929,68930,68931,68932,68933,68934,68935,68936,68937,68938,68939,68940,68941,68942,68943,68944,68945,68946,68947,68948,68949,68950,68951,68952,68953,68954,68955,68956,68957,68958,68959,68960,68961,68962,68963,68964,68965,68966,68967,68968,68969,68970,68971,68972,68973,68974,68975,68976,68977,68978,68979,68980,68981,68982,68983,68984,68985,68986,68987,68988,68989,68990,68991,68992,68993,68994,68995,68996,68997,68998,68999,69000,69001,69002,69003,69004,69005,69006,69007,69008,69009,69010,69011,69012,69013,69014,69015,69016,69017,69018,69019,69020,69021,69022,69023,69024,69025,69026,69027,69028,69029,69030,69031,69032,69033,69034,69035,69036,69037,69038,69039,69040,69041,69042,69043,69044,69045,69046,69047,69048,69049,69050,69051,69052,69053,69054,69055,69056,69057,69058,69059,69060,69061,69062,69063,69064,69065,69066,69067,69068,69069,69070,69071,69072,69073,69074,69075,69076,69077,69078,69079,69080,69081,69082,69083,69084,69085,69086,69087,69088,69089,69090,69091,69092,69093,69094,69095,69096,69097,69098,69099,69100,69101,69102,69103,69104,69105,69106,69107,69108,69109,69110,69111,69112,69113,69114,69115,69116,69117,69118,69119,69120,69121,69122,69123,69124,69125,69126,69127,69128,69129,69130,69131,69132,69133,69134,69135,69136,69137,69138,69139,69140,69141,69142,69143,69144,69145,69146,69147,69148,69149,69150,69151,69152,69153,69154,69155,69156,69157,69158,69159,69160,69161,69162,69163,69164,69165,69166,69167,69168,69169,69170,69171,69172,69173,69174,69175,69176,69177,69178,69179,69180,69181,69182,69183,69184,69185,69186,69187,69188,69189,69190,69191,69192,69193,69194,69195,69196,69197,69198,69199,69200,69201,69202,69203,69204,69205,69206,69207,69208,69209,69210,69211,69212,69213,69214,69215,69216,69217,69218,69219,69220,69221,69222,69223,69224,69225,69226,69227,69228,69229,69230,69231,69232,69233,69234,69235,69236,69237,69238,69239,69240,69241,69242,69243,69244,69245,69246,69247,69248,69249,69250,69251,69252,69253,69254,69255,69256,69257,69258,69259,69260,69261,69262,69263,69264,69265,69266,69267,69268,69269,69270,69271,69272,69273,69274,69275,69276,69277,69278,69279,69280,69281,69282,69283,69284,69285,69286,69287,69288,69289,69290,69291,69292,69293,69294,69295,69296,69297,69298,69299,69300,69301,69302,69303,69304,69305,69306,69307,69308,69309,69310,69311,69312,69313,69314,69315,69316,69317,69318,69319,69320,69321,69322,69323,69324,69325,69326,69327,69328,69329,69330,69331,69332,69333,69334,69335,69336,69337,69338,69339,69340,69341,69342,69343,69344,69345,69346,69347,69348,69349,69350,69351,69352,69353,69354,69355,69356,69357,69358,69359,69360,69361,69362,69363,69364,69365,69366,69367,69368,69369,69370,69371,69372,69373,69374,69375,69376,69377,69378,69379,69380,69381,69382,69383,69384,69385,69386,69387,69388,69389,69390,69391,69392,69393,69394,69395,69396,69397,69398,69399,69400,69401,69402,69403,69404,69405,69406,69407,69408,69409,69410,69411,69412,69413,69414,69415,69416,69417,69418,69419,69420,69421,69422,69423,69424,69425,69426,69427,69428,69429,69430,69431,69432,69433,69434,69435,69436,69437,69438,69439,69440,69441,69442,69443,69444,69445,69446,69447,69448,69449,69450,69451,69452,69453,69454,69455,69456,69457,69458,69459,69460,69461,69462,69463,69464,69465,69466,69467,69468,69469,69470,69471,69472,69473,69474,69475,69476,69477,69478,69479,69480,69481,69482,69483,69484,69485,69486,69487,69488,69489,69490,69491,69492,69493,69494,69495,69496,69497,69498,69499,69500,69501,69502,69503,69504,69505,69506,69507,69508,69509,69510,69511,69512,69513,69514,69515,69516,69517,69518,69519,69520,69521,69522,69523,69524,69525,69526,69527,69528,69529,69530,69531,69532,69533,69534,69535,69536,69537,69538,69539,69540,69541,69542,69543,69544,69545,69546,69547,69548,69549,69550,69551,69552,69553,69554,69555,69556,69557,69558,69559,69560,69561,69562,69563,69564,69565,69566,69567,69568,69569,69570,69571,69572,69573,69574,69575,69576,69577,69578,69579,69580,69581,69582,69583,69584,69585,69586,69587,69588,69589,69590,69591,69592,69593,69594,69595,69596,69597,69598,69599,69600,69601,69602,69603,69604,69605,69606,69607,69608,69609,69610,69611,69612,69613,69614,69615,69616,69617,69618,69619,69620,69621,69622,69623,69624,69625,69626,69627,69628,69629,69630,69631,69632,69633,69634,69635,69636,69637,69638,69639,69640,69641,69642,69643,69644,69645,69646,69647,69648,69649,69650,69651,69652,69653,69654,69655,69656,69657,69658,69659,69660,69661,69662,69663,69664,69665,69666,69667,69668,69669,69670,69671,69672,69673,69674,69675,69676,69677,69678,69679,69680,69681,69682,69683,69684,69685,69686,69687,69688,69689,69690,69691,69692,69693,69694,69695,69696,69697,69698,69699,69700,69701,69702,69703,69704,69705,69706,69707,69708,69709,69710,69711,69712,69713,69714,69715,69716,69717,69718,69719,69720,69721,69722,69723,69724,69725,69726,69727,69728,69729,69730,69731,69732,69733,69734,69735,69736,69737,69738,69739,69740,69741,69742,69743,69744,69745,69746,69747,69748,69749,69750,69751,69752,69753,69754,69755,69756,69757,69758,69759,69760,69761,69762,69763,69764,69765,69766,69767,69768,69769,69770,69771,69772,69773,69774,69775,69776,69777,69778,69779,69780,69781,69782,69783,69784,69785,69786,69787,69788,69789,69790,69791,69792,69793,69794,69795,69796,69797,69798,69799,69800,69801,69802,69803,69804,69805,69806,69807,69808,69809,69810,69811,69812,69813,69814,69815,69816,69817,69818,69819,69820,69821,69822,69823,69824,69825,69826,69827,69828,69829,69830,69831,69832,69833,69834,69835,69836,69837,69838,69839,69840,69841,69842,69843,69844,69845,69846,69847,69848,69849,69850,69851,69852,69853,69854,69855,69856,69857,69858,69859,69860,69861,69862,69863,69864,69865,69866,69867,69868,69869,69870,69871,69872,69873,69874,69875,69876,69877,69878,69879,69880,69881,69882,69883,69884,69885,69886,69887,69888,69889,69890,69891,69892,69893,69894,69895,69896,69897,69898,69899,69900,69901,69902,69903,69904,69905,69906,69907,69908,69909,69910,69911,69912,69913,69914,69915,69916,69917,69918,69919,69920,69921,69922,69923,69924,69925,69926,69927,69928,69929,69930,69931,69932,69933,69934,69935,69936,69937,69938,69939,69940,69941,69942,69943,69944,69945,69946,69947,69948,69949,69950,69951,69952,69953,69954,69955,69956,69957,69958,69959,69960,69961,69962,69963,69964,69965,69966,69967,69968,69969,69970,69971,69972,69973,69974,69975,69976,69977,69978,69979,69980,69981,69982,69983,69984,69985,69986,69987,69988,69989,69990,69991,69992,69993,69994,69995,69996,69997,69998,69999,70000,70001,70002,70003,70004,70005,70006,70007,70008,70009,70010,70011,70012,70013,70014,70015,70016,70017,70018,70019,70020,70021,70022,70023,70024,70025,70026,70027,70028,70029,70030,70031,70032,70033,70034,70035,70036,70037,70038,70039,70040,70041,70042,70043,70044,70045,70046,70047,70048,70049,70050,70051,70052,70053,70054,70055,70056,70057,70058,70059,70060,70061,70062,70063,70064,70065,70066,70067,70068,70069,70070,70071,70072,70073,70074,70075,70076,70077,70078,70079,70080,70081,70082,70083,70084,70085,70086,70087,70088,70089,70090,70091,70092,70093,70094,70095,70096,70097,70098,70099,70100,70101,70102,70103,70104,70105,70106,70107,70108,70109,70110,70111,70112,70113,70114,70115,70116,70117,70118,70119,70120,70121,70122,70123,70124,70125,70126,70127,70128,70129,70130,70131,70132,70133,70134,70135,70136,70137,70138,70139,70140,70141,70142,70143,70144,70145,70146,70147,70148,70149,70150,70151,70152,70153,70154,70155,70156,70157,70158,70159,70160,70161,70162,70163,70164,70165,70166,70167,70168,70169,70170,70171,70172,70173,70174,70175,70176,70177,70178,70179,70180,70181,70182,70183,70184,70185,70186,70187,70188,70189,70190,70191,70192,70193,70194,70195,70196,70197,70198,70199,70200,70201,70202,70203,70204,70205,70206,70207,70208,70209,70210,70211,70212,70213,70214,70215,70216,70217,70218,70219,70220,70221,70222,70223,70224,70225,70226,70227,70228,70229,70230,70231,70232,70233,70234,70235,70236,70237,70238,70239,70240,70241,70242,70243,70244,70245,70246,70247,70248,70249,70250,70251,70252,70253,70254,70255,70256,70257,70258,70259,70260,70261,70262,70263,70264,70265,70266,70267,70268,70269,70270,70271,70272,70273,70274,70275,70276,70277,70278,70279,70280,70281,70282,70283,70284,70285,70286,70287,70288,70289,70290,70291,70292,70293,70294,70295,70296,70297,70298,70299,70300,70301,70302,70303,70304,70305,70306,70307,70308,70309,70310,70311,70312,70313,70314,70315,70316,70317,70318,70319,70320,70321,70322,70323,70324,70325,70326,70327,70328,70329,70330,70331,70332,70333,70334,70335,70336,70337,70338,70339,70340,70341,70342,70343,70344,70345,70346,70347,70348,70349,70350,70351,70352,70353,70354,70355,70356,70357,70358,70359,70360,70361,70362,70363,70364,70365,70366,70367,70368,70369,70370,70371,70372,70373,70374,70375,70376,70377,70378,70379,70380,70381,70382,70383,70384,70385,70504,70505,70506,70507,70508,70509,70510,70511,70512,70513,70514,70515,70516,70517,70518,70519,70520,70521,70522,70523,70524,70525,70526,70527,70528,70529,70530,70531,70532,70553,70555,70556,70557,70558,70559,70560,70561,70562,70563,70564,70565,70566,70567,70568,70606,70608,70609,70610,70611,70612,70613,70614,70615,70616,70617,70618,70657,70658,70659,70660,70661,70662,70663,70664,70665,70666,70667,70668,70669,70670,70671,70672,70673,70674,70675,70676,70677,70678,70679,70680,70681,70682,70683,70684,70685,70686,70687,70688,70689,70690,70691,70692,70693,70694,70695,70696,70697,70698,70699,70700,70701,70702,70703,70704,70705,70706,70707,70708,70709,70710,70711,70712,70713,70714,70715,70716,70717,70718,70719,70720,70721,70722,70723,70724,70725,70726,70727,70728,70729,70730,70731,70732,70733,70734,70735,70736,70737,70738,70739,70740,70741,70742,70743,70744,70745,70746,70747,70748,70749,70750,70751,70752,70753,70754,70755,70756,70757,70758,70759,70760,70761,70762,70763,70764,70765,70766,70767,70768,70769,70770,70771,70772,70773,70774,70775,70776,70777,70778,70779,70780,70781,70782,70783,70784,70785,70786,70787,70788,70789,70790,70791,70792,70793,70794,70795,70796,70797,70798,70799,70800,70801,70802,70803,70804,70805,70806,70807,70808,70809,70810,70811,70812,70813,70814,70815,70816,70817,70818,70819,70820,70821,70822,70823,70824,70825,70826,70827,70828,70829,70830,70831,70832,70833,70834,70835,70836,70837,70838,70839,70840,70841,70842,70843,70844,70845,70846,70847,70848,70849,70850,70851,70852,70853,70854,70855,70856,70857,70858,70859,70860,70861,70862,70863,70864,70865,70866,70867,70868,70869,70870,70871,70872,70873,70874,70875,70876,70877,70878,70879,70880,70881,70882,70883,70884,70885,70886,70887,70888,70889,70890,70891,70892,70893,70894,70895,70896,70897,70898,70899,70900,70901,70902,70903,70904,70905,70906,70907,70908,70909,70910,70911,70912,70913,70914,70915,70916,70917,70918,70919,70920,70921,70922,70923,70924,70925,70926,70927,70928,70929,70930,70931,70932,70933,70934,70935,70936,70937,70938,70939,70940,70941,70942,70943,70944,70945,70946,70947,70948,70949,70950,70951,70952,70953,70954,70955,70956,70957,70958,70959,70960,70961,70962,70963,70964,70965,70966,70967,70968,70969,70970,70971,70972,70973,70974,70975,70976,70977,70978,70979,70980,70981,70982,70983,70984,70985,70986,70987,70988,70989,70990,70991,70992,70993,70994,70995,70996,70997,70998,70999,71000,71001,71002,71003,71004,71005,71006,71007,71008,71009,71010,71011,71012,71013,71014,71015,71016,71017,71018,71019,71020,71021,71022,71023,71024,71025,71026,71027,71028,71029,71030,71031,71032,71033,71034,71035,71036,71037,71038,71039,71040,71041,71042,71043,71044,71045,71046,71047,71048,71049,71050,71051,71052,71053,71054,71055,71056,71057,71058,71059,71060,71061,71062,71063,71064,71065,71066,71067,71068,71069,71070,71071,71072,71073,71074,71075,71076,71077,71078,71079,71080,71081,71082,71083,71084,71085,71086,71087,71088,71089,71090,71091,71092,71093,71094,71095,71096,71097,71098,71099,71100,71101,71102,71103,71104,71105,71106,71107,71108,71109,71110,71111,71112,71113,71114,71115,71116,71117,71118,71119,71120,71121,71122,71123,71124,71125,71126,71127,71128,71129,71130,71131,71132,71133,71134,71135,71136,71137,71138,71139,71140,71141,71142,71143,71144,71145,71146,71147,71148,71149,71150,71151,71152,71153,71154,71155,71156,71157,71158,71159,71160,71161,71162,71163,71164,71165,71166,71167,71168,71169,71170,71171,71172,71173,71174,71175,71176,71177,71178,71179,71180,71181,71182,71183,71184,71185,71186,71187,71188,71189,71190,71191,71192,71193,71194,71195,71196,71197,71198,71199,71200,71201,71202,71203,71204,71205,71206,71207,71208,71209,71210,71211,71212,71213,71214,71215,71216,71217,71218,71219,71220,71221,71222,71223,71224,71225,71226,71227,71228,71229,71230,71231,71232,71233,71234,71235,71236,71237,71238,71239,71240,71241,71242,71243,71244,71245,71246,71247,71248,71249,71250,71251,71252,71253,71254,71255,71256,71257,71258,71259,71260,71261,71262,71263,71264,71265,71266,71267,71268,71269,71270,71271,71272,71273,71274,71275,71276,71277,71278,71279,71280,71281,71282,71283,71284,71285,71286,71287,71288,71289,71290,71291,71292,71293,71294,71295,71296,71297,71298,71299,71300,71301,71302,71303,71304,71305,71306,71307,71308,71309,71310,71311,71312,71313,71314,71315,71316,71317,71318,71319,71320,71321,71322,71323,71324,71325,71326,71327,71328,71329,71330,71331,71332,71333,71334,71335,71336,71337,71338,71339,71340,71341,71342,71343,71344,71345,71346,71347,71348,71349,71350,71351,71352,71353,71354,71355,71356,71357,71360,71361,71383,71423,71424,71425,71426,71427,71428,71429,71430,71431,71432,71433,71434,71435,71436,71437,71438,71439,71440,71441,71442,71443,71444,71445,71446,71447,71448,71449,71450,71451,71452,71453,71454,71455,71456,71457,71458,71459,71460,71461,71462,71463,71464,71465,71466,71467,71468,71469,71470,71471,71472,71473,71474,71475,71476,71477,71478,71479,71480,71481,71482,71483,71484,71485,71486,71487,71488,71489,71490,71491,71492,71493,71494,71495,71496,71497,71498,71499,71500,71501,71502,71503,71504,71505,71506,71507,71508,71509,71510,71511,71512,71513,71514,71515,71516,71517,71518,71519,71520,71521,71522,71523,71524,71525,71526,71527,71528,71529,71530,71531,71532,71533,71534,71535,71536,71537,71538,71539,71540,71541,71542,71543,71544,71545,71546,71547,71548,71549,71550,71551,71552,71553,71554,71555,71556,71557,71558,71559,71560,71561,71562,71563,71564,71565,71566,71567,71568,71569,71570,71571,71572,71573,71574,71575,71576,71577,71578,71579,71580,71581,71582,71583,71584,71585,71586,71587,71588,71589,71590,71591,71592,71593,71594,71595,71596,71597,71598,71599,71600,71601,71602,71603,71604,71605,71606,71607,71608,71609,71610,71611,71612,71613,71614,71615,71616,71617,71618,71619,71620,71621,71622,71623,71624,71625,71626,71627,71628,71629,71630,71631,71632,71633,71634,71635,71636,71637,71638,71639,71640,71641,71642,71643,71644,71645,71646,71647,71648,71649,71650,71651,71652,71653,71654,71655,71656,71657,71658,71659,71660,71661,71662,71663,71664,71665,71666,71667,71668,71669,71670,71671,71672,71673,71674,71675,71676,71677,71678,71679,71680,71681,71682,71683,71684,71685,71686,71687,71688,71689,71690,71691,71692,71693,71694,71695,71696,71697,71698,71699,71700,71701,71702,71703,71704,71705,71706,71707,71708,71709,71710,71711,71712,71713,71714,71715,71716,71717,71718,71719,71720,71721,71722,71723,71724,71725,71726,71727,71728,71729,71730,71731,71732,71733,71734,71735,71736,71737,71738,71739,71740,71741,71742,71743,71744,71745,71746,71747,71748,71749,71750,71751,71752,71753,71754,71755,71756,71757,71758,71759,71760,71761,71768,71769,71770,71771,71772,71773,71774,71775,71776,71777,71778,71779,71780,71781,71782,71783,71784,71785,71804,71805,71806,71807,71811,71812,71813,71814,71815,71816,71817,71818,71819,71822,71823,71824,71825,71826,71827,71828,71829,71831,71832,71833,71834,71835,71836,71837,71838,71839,71840,71841,71842,71843,71844,71845,71846,71847,71848,71849,71850,71851,71852,};
	cout << "leftLeg: " << (sizeof(faceID) / sizeof(DWORD)) << endl;
	for (int i = 0; i < (int) sizeof(faceID) / sizeof(DWORD); ++i) {
		face_id_set.insert(faceID[i]);
	}
	__defined_area_dic[AREA_LEFT_LEG] = defArea;
} while (0);
do {	// 오른쪽 다리
	kArea::DEFINED_AREA_DIC::iterator	it;
	it = __defined_area_dic.find(AREA_RIGHT_LEG);
	if (it != __defined_area_dic.end())
		__defined_area_dic.erase(it);

	kArea::DEFINED_AREA	defArea;
	defArea.value = .9f;	// 원하는 색깔이...
	std::set<DWORD>&	face_id_set = defArea.face_id_set;
	DWORD	faceID[] = {62360,62361,62386,62387,62388,62433,62434,62442,62443,62444,62445,62446,62447,62448,62449,62450,62451,62452,62544,62545,62546,62547,62548,62549,62550,62551,62554,62555,62556,62557,62558,62559,62560,62561,62562,62563,62564,62565,62566,62567,62568,62569,62570,62571,62572,62573,62574,62575,62576,62577,62578,62579,62580,62581,62582,62583,62584,62587,62588,62589,62590,62599,62600,62601,62602,62603,62604,62605,62606,62607,62608,62609,62612,62613,62614,62615,62616,62617,62618,62619,62622,62623,62624,62625,62626,62627,62628,62629,62630,62631,62632,62633,62634,62635,62636,62637,62638,62639,62640,62641,62642,62643,62644,62645,62646,62647,62648,63084,63085,63110,63111,63112,63158,63169,63170,63171,63172,63173,63174,63175,63176,63268,63269,63270,63271,63272,63273,63274,63275,63278,63279,63280,63281,63282,63283,63284,63285,63286,63287,63288,63289,63290,63291,63292,63293,63295,63296,63297,63298,63299,63300,63301,63302,63303,63304,63305,63306,63311,63312,63313,63314,63323,63324,63325,63326,63327,63328,63329,63330,63331,63332,63333,63336,63337,63347,63349,63350,63351,63352,63353,63354,63355,63356,63357,63358,63359,63360,63361,63362,63363,63364,63365,63366,63367,63368,63919,63920,63921,63922,63925,63958,63959,63961,63962,63975,63976,63978,64007,64008,64009,64010,64011,64012,64043,64044,64045,64065,64066,64070,64071,64072,64073,64074,64075,64562,64563,65423,65424,65425,65426,65427,65428,65429,65430,65431,65432,65433,65434,65435,65436,65437,65438,65439,65440,65441,65442,65443,65444,65445,65446,65447,65448,65449,65450,65451,65452,65453,65454,65455,65456,65457,65458,65459,65460,65461,65462,65463,65464,65465,65466,65467,65468,65469,65470,65471,65472,65473,65474,65475,65476,65477,65478,65479,65480,65481,65482,65483,65484,65485,65486,65487,65488,65489,65490,65491,65492,65493,65494,65497,65498,65540,65545,65546,65549,65550,65560,65561,65562,65567,65568,65569,65570,65571,65572,65573,65574,65575,65576,65577,65578,65579,65580,65581,65582,65583,65584,65585,65586,65587,65588,65589,65590,65595,65596,65597,65598,65601,65603,65604,65605,65606,65607,65608,65609,65610,65611,65612,65613,65614,65615,65616,65617,65618,65619,65620,65621,65622,65623,65624,65625,65626,65627,65628,65629,65630,65631,65632,65633,65634,65635,65636,65637,65638,65639,65640,65641,65642,65643,65644,65645,65646,65647,65648,65649,65650,65651,65652,65653,65654,65655,65656,65657,65658,65659,65660,65661,65662,65663,65664,65665,65666,65667,65668,65669,65670,65671,65672,65673,65674,65675,65676,65677,65678,65679,65680,65681,65684,65685,65686,65687,65688,65689,65690,65691,65692,65693,65694,65695,65696,65697,65701,65702,65703,65704,65705,65706,65709,65710,65711,65712,65715,65716,65728,65729,65730,65731,65732,65733,65734,65735,65736,65769,65770,65772,65773,65776,65777,65778,65779,65786,65787,65788,65789,65790,65791,65792,65793,65794,65795,65796,65797,65798,65799,65800,65801,65808,65809,65810,65811,65812,65813,65814,65815,65816,65817,65818,65819,65820,65821,65822,65823,65847,65848,65849,65850,65851,65852,65853,65854,65855,65856,65857,65876,65877,65881,65882,65883,65884,65885,65886,65913,65914,65915,65916,65917,65918,65919,65920,65927,65928,65929,65930,65931,65932,65941,65942,65943,65944,65946,65951,65952,65955,65956,65972,65973,65982,65992,65993,65994,65995,65996,65997,65998,65999,66004,66005,66006,66007,66008,66009,66010,66011,66012,66013,66014,66015,66016,66017,66018,66019,66020,66021,66022,66023,66024,66025,66026,66027,66028,66029,66178,66179,66180,66181,66182,66183,66184,66185,66186,66187,66188,66189,66190,66191,66192,66193,66194,66195,66196,66197,66198,66199,66200,66201,66202,66203,66204,66205,66206,66207,66210,66212,66213,66214,66215,66217,66218,66219,66220,66221,66222,66228,66237,66238,66239,66240,66241,66242,66245,66246,66247,66248,66249,66250,66251,66252,66253,66254,66255,66256,66258,66259,66260,66261,66262,66263,66264,66265,66266,66267,66268,66269,66270,66271,66272,66273,66274,66275,66276,66277,66278,66279,66280,66281,66282,66283,66284,66285,66339,66340,66341,66342,66359,66360,66361,66362,66363,66364,66365,66366,66367,66368,66369,66370,66371,66372,66373,66374,66375,66376,66377,66378,66379,66380,66386,66387,66388,66389,66390,66391,66392,66393,66394,66395,66396,66397,66398,66399,66400,66401,66402,66403,66404,66405,66406,66407,66408,66409,66410,66411,66412,66413,66414,66415,66416,66417,66418,66419,66420,66421,66422,66423,66424,66425,66426,66427,66428,66429,66430,66431,66432,66433,66434,66435,66436,66437,66438,66439,66440,66441,66442,66471,66472,66473,66474,66475,66476,66477,66478,66479,66480,66481,66482,66483,66484,66485,66486,66487,66489,66490,66491,66492,66493,66494,66495,66496,66497,66498,66499,66500,66501,66510,66511,66512,66513,66514,66515,66516,66517,66518,66519,66520,66521,66522,66523,66524,66525,66526,66527,66528,66529,66532,66534,66535,66536,66537,66540,66554,66555,66556,66557,66558,66559,66560,66561,66562,66563,66564,66565,66566,66567,66568,66569,66570,66571,66572,66573,66574,66575,66576,66577,66578,66579,66580,66581,66582,66583,66584,66585,66586,66587,66588,66589,66590,66591,66592,66593,66594,66595,66596,66597,66598,66599,66600,66601,66602,66603,66604,66605,66606,66607,66608,66609,66610,66611,66612,66613,66614,66615,66616,66617,66618,66619,66620,66621,66623,66624,66625,66626,66627,66628,66629,66630,66631,66632,66633,66634,66635,66636,66637,66638,66639,66640,66641,66642,66643,66644,66645,66646,66647,66648,66649,66650,66651,66652,66653,66654,66655,66656,66657,66658,66659,66660,66661,66662,66663,66664,66665,66667,66668,66669,66674,66675,66682,66683,66684,66685,66686,66687,66692,66693,66694,66695,66696,66697,66698,66699,66700,66701,66702,66703,66704,66719,66720,66721,66722,66727,66728,66729,66730,66731,66732,66733,66734,66735,66736,66737,66756,66757,66764,66765,66768,66769,66770,66771,66772,66773,66961,66962,66994,66995,66996,66997,66998,66999,67004,67005,67006,67007,67008,67009,67010,67011,67012,67013,67014,67037,67038,67039,67040,67041,67042,67045,67046,67047,67048,67049,67056,67057,67058,67059,67060,67061,67080,67081,67086,67087,67090,67091,67094,67095,67096,67097,67098,67099,67100,67101,67102,67103,67110,67111,67115,67135,67193,67196,67197,67234,67235,67236,67237,67238,67239,67240,67241,67242,67243,67244,67245,67246,67247,67248,67249,67250,67251,67252,67253,67254,67255,67256,67257,67258,67259,67260,67261,67262,67263,67264,67265,67266,67267,67268,67269,67270,67271,67272,67273,67274,67275,67276,67277,67278,67279,67280,67281,67282,67283,67284,67285,67286,67287,67288,67289,67290,67291,67292,67293,67294,67295,67296,67297,67298,67299,67300,67301,67302,67303,67304,67305,67306,67307,67308,67309,67310,67311,67312,67313,67314,67315,67316,67317,67318,67319,67320,67321,67322,67323,67324,67325,67326,67327,67328,67329,67330,67331,67332,67333,67334,67335,67336,67337,67338,67339,67340,67341,67342,67343,67344,67345,67346,67347,67348,67349,67350,67351,67352,67353,67354,67355,67356,67357,67358,67359,67360,67361,67362,67363,67364,67365,67366,67367,67368,67369,67370,67371,67372,67373,67374,67375,67376,67377,67378,67379,67380,67381,67382,67383,67384,67385,67386,67387,67388,67389,67390,67391,67392,67393,67394,67395,67396,67397,67398,67399,67400,67401,67402,67403,67404,67405,67406,67407,67408,67409,67410,67411,67412,67413,67414,67415,67416,67417,67418,67419,67420,67421,67422,67423,67424,67425,67426,67427,67428,67429,67430,67431,67432,67433,67434,67435,67436,67437,67438,67439,67440,67441,67442,67443,67444,67445,67446,67447,67448,67449,67450,67451,67452,67453,67454,67455,67456,67457,67458,67459,67460,67461,67462,67463,67464,67465,67466,67467,67468,67469,67470,67471,67472,67473,67474,67475,67476,67477,67478,67479,67480,67481,67482,67483,67484,67485,67486,67487,67488,67489,67490,67491,67492,67493,67494,67495,67496,67497,67498,67499,67500,67501,67502,67503,67504,67505,67506,67507,67508,67509,67510,67511,67512,67513,67514,67515,67516,67517,67518,67519,67520,67521,67522,67523,67524,67525,67526,67527,67528,67529,67530,67531,67532,67533,67534,67535,67536,67537,67538,67539,67540,67541,67542,67543,67544,67545,67546,67547,67548,67549,67550,67551,67552,67553,67554,67555,67556,67557,67558,67559,67560,67561,67562,67563,67564,67565,67566,67567,67568,67569,67570,67571,67572,67573,67574,67575,67576,67577,67578,67579,67580,67581,67582,67583,67584,67585,67586,67587,67588,67589,67590,67591,67592,67593,67594,67595,67596,67597,67598,67599,67600,67601,67602,67603,67604,67605,67606,67607,67608,67609,67610,67611,67612,67613,67614,67615,67616,67617,67618,67619,67620,67621,67622,67623,67624,67625,67626,67627,67628,67629,67630,67631,67632,67633,67634,67635,67636,67637,67638,67639,67640,67641,67642,67643,67644,67645,67646,67647,67648,67649,67650,67651,67652,67653,67654,67655,67656,67657,67658,67659,67660,67661,67662,67663,67664,67665,67666,67667,67668,67669,67670,67671,67672,67673,67674,67675,67676,67677,67678,67679,67680,67681,67682,67683,67684,67685,67686,67687,67688,67689,67690,67691,67692,67693,67694,67695,67696,67697,67698,67699,67700,67701,67702,67703,67704,67705,67706,67707,67708,67709,67710,67711,67712,67713,67714,67715,67716,67717,67718,67719,67720,67721,67722,67723,67724,67725,67726,67727,67728,67729,67730,67731,67732,67733,67734,67735,67736,67737,67738,67739,67740,67741,67742,67743,67744,67745,67746,67747,67748,67749,67750,67751,67752,67753,67754,67755,67756,67757,67758,67759,67760,67761,67762,67763,67764,67765,67766,67767,67768,67769,67770,67771,67772,67773,67774,67775,67776,67777,67778,67779,67780,67781,67782,67783,67784,67785,67786,67787,67788,67789,67790,67791,67792,67793,67794,67795,67796,67797,67798,67799,67800,67801,67802,67803,67804,67805,67806,67807,67808,67809,67810,67811,67812,67813,67814,67815,67816,67817,67818,67819,67820,67821,67822,67823,67824,67825,67826,67827,67828,67829,67830,67831,67832,67833,67834,67835,67836,67837,67838,67839,67840,67841,67842,67843,67844,67845,67846,67847,67848,67849,67850,67851,67852,67853,67854,67855,67856,67857,67858,67859,67860,67861,67862,67863,67864,67865,67866,67867,67868,67869,67870,67871,67872,67873,67874,67875,67876,67877,67878,67879,67880,67881,67882,67883,67884,67885,67886,67887,67888,67889,67890,67891,67892,67893,67894,67895,67896,67897,67898,67899,67900,67901,67902,67903,67904,67905,67906,67907,67908,67909,67910,67911,67912,67913,67914,67915,67916,67917,67918,67919,67920,67921,67922,67923,67924,67925,67926,67927,67928,67929,67930,67931,67932,67933,67934,67935,67936,67937,67938,67939,67940,67941,67942,67943,67944,67945,67946,67947,67948,67949,67950,67951,67952,67953,67954,67955,67956,67957,67958,67959,67960,67961,67962,67963,67964,67965,67966,67967,67968,67969,67970,67971,67972,67973,67974,67975,67976,67977,67978,67979,67980,67981,67982,67983,67984,67985,67986,67987,67988,67989,67990,67991,67992,67993,67994,67995,67996,67997,67998,67999,68000,68001,68002,68003,68004,68005,68006,68007,68008,68009,68010,68011,68012,68013,68014,68015,68016,68017,68018,68019,68020,68021,68022,68023,68024,68025,68026,68027,68028,68029,68030,68031,68032,68033,68034,68035,68036,68037,68038,68039,68040,68041,68042,68043,68044,68045,68046,68047,68048,68049,68050,68051,68052,68053,68054,68055,68056,68057,68058,68059,68060,68061,68062,68063,68064,68065,68066,68067,68068,68069,68070,68071,68072,68073,68074,68075,68076,68077,68078,68079,68080,68081,68082,68083,68084,68085,68086,68087,68088,68089,68090,68091,68092,68093,68094,68095,68096,68097,68098,68099,68100,68101,68102,68103,68104,68105,68106,68107,68108,68109,68110,68111,68112,68113,68114,68115,68116,68117,68118,68119,68120,68121,68122,68123,68124,68125,68126,68127,68128,68129,68130,68131,68132,68133,68134,68135,68136,68137,68138,68139,68140,68141,68142,68143,68144,68145,68146,68147,68148,68149,68150,68151,68152,68153,68154,68155,68156,68157,68158,68159,68160,68161,68162,68163,68164,68165,68166,68167,68168,68169,68170,68171,68172,68173,68174,68175,68176,68177,68178,68179,68180,68181,68182,68183,68184,68185,68186,68187,68188,68189,68190,68191,68192,68193,68194,68195,68196,68197,68198,68199,68200,68201,68202,68203,68204,68205,68206,68207,68208,68209,68210,68211,68212,68213,68214,68215,68216,68217,68218,68219,68220,68221,68222,68223,68224,68225,68226,68227,68228,68229,68230,68231,68232,68233,68234,68235,68236,68237,68238,68239,68240,68241,68242,68243,68244,68245,68246,68247,68248,68249,68250,68251,68252,68253,68254,68255,68256,68257,68258,68259,68260,68261,68262,68263,68264,68265,68266,68267,68268,68269,68270,68271,68272,68273,68274,68275,68276,68277,68278,68279,68280,68281,68282,68283,68284,68285,68286,68287,68288,68289,68290,68291,68292,68293,68294,68295,68296,68297,68298,68299,68300,68301,68302,68303,68304,68305,68306,68307,68308,68309,68310,68311,68312,68313,68314,68315,68316,68317,68318,68319,68320,68321,68322,68323,68324,68325,68326,68327,68328,68329,68330,68331,68332,68333,68334,68335,68336,68337,68338,68339,68340,68341,68342,68343,68344,68345,68346,68347,68348,68349,68350,68351,68352,68353,68354,68355,68356,68357,68358,68359,68360,68361,68362,68363,68364,68365,68366,68367,68368,68369,68370,68371,68372,68373,68374,68375,68376,68377,68378,68379,68380,68381,68382,68383,68384,68385,68386,68387,68388,68389,68390,68391,68392,68393,68394,68395,68396,68397,68398,68399,68400,68401,68402,68403,68404,68405,68406,68407,68408,68409,68410,68411,68412,68413,68414,68415,68416,68417,68418,68419,68420,68421,68422,68423,68424,68425,68426,68427,68428,68429,68430,68431,68432,68433,68434,68435,68436,68437,68438,68439,68440,68441,68442,68443,68444,68445,68446,68447,68448,68449,68450,68451,68452,68453,68454,68455,68456,68457,68458,68459,68460,68461,68462,68463,68464,68465,68466,68467,68468,68469,68470,68471,68472,68473,68474,68475,68476,68477,68478,68479,68480,68481,68482,68483,68484,68485,68486,68487,68488,68489,68490,68491,68492,68493,68494,68495,68496,68497,68498,68499,68500,68501,68502,68503,68504,68505,68506,68507,68508,68509,68510,68511,68512,68513,68514,68515,68516,68517,68518,68519,68520,68521,68522,68523,68524,68525,68526,68527,68528,68529,68530,68531,68532,68533,68534,68535,68536,68537,68538,68539,68540,68541,68542,68543,68544,68545,68546,68547,68548,68549,68550,68551,68552,68553,68554,68555,68556,68557,68558,68559,68560,68561,68562,68563,68564,68565,68566,68567,68568,68569,68570,68571,68572,68573,68574,68575,68576,68577,68578,68579,68580,68581,68582,68583,68584,68585,68586,68587,68588,68589,68590,68591,68592,68593,68594,68595,68596,68597,68598,68599,68600,68601,68602,68603,68604,68605,68606,68607,68608,68609,68610,68611,68612,68613,68614,68615,68616,68617,68618,68619,68620,68621,68622,68623,68624,68625,68626,68627,68628,68629,68630,68631,68632,68633,68634,68635,68636,68637,68638,68639,68640,68641,68642,68643,68644,68645,68646,68647,68648,68649,68650,68651,68652,68653,68654,68655,68656,68657,68658,68659,68660,68661,68662,68663,68664,68665,68666,68667,68668,68669,68670,68671,68672,68673,68674,68675,68676,68677,68678,68679,68680,68681,68682,68683,68684,68685,68686,68687,68688,68689,68690,68691,68692,68693,68694,68695,68696,68697,68698,68699,68700,68701,68702,68703,68704,68705,68706,68707,68708,68709,68710,68711,68712,68713,68714,68715,68716,68717,68718,68719,68720,68721,68722,68723,68724,68725,68726,68727,68728,68729,68730,68731,68732,68733,68734,68735,68736,68737,68738,68739,68740,68741,68742,68743,68744,68745,68746,68747,68748,68749,68750,68751,68752,68753,68754,68755,68756,68757,68758,68759,68760,68761,68762,68763,68764,68765,68766,68767,68768,68769,68770,68771,68772,68773,68774,68775,68776,68777,68778,68779,68780,68781,68782,68783,68784,68785,68786,68787,68788,68789,68790,68791,68792,68793,68794,68795,68796,68797,68798,68799,71856,71857,71858,71859,71860,71861,71862,71863,71869,71870,71871,71872,71991,71992,71993,71994,71998,71999,72000,72001,72002,72003,72004,72005,72006,72008,72009,72010,72011,72012,72013,72014,72015,72017,72042,72043,72044,72045,72046,72047,72048,72049,72050,72051,72052,72053,72054,72055,72144,72145,72146,72147,72148,72149,72150,72151,72152,72153,72154,72155,72156,72157,72158,72159,72160,72161,72162,72163,72164,72165,72166,72167,72168,72169,72170,72171,72172,72173,72174,72175,72176,72177,72178,72179,72180,72181,72182,72183,72184,72185,72186,72187,72188,72189,72190,72191,72192,72193,72194,72195,72196,72197,72198,72199,72200,72201,72202,72203,72204,72205,72206,72207,72208,72209,72210,72211,72212,72213,72214,72215,72216,72217,72218,72219,72220,72221,72222,72223,72224,72225,72226,72227,72228,72229,72230,72231,72232,72233,72234,72235,72236,72237,72238,72239,72240,72241,72242,72243,72244,72245,72246,72247,72248,72249,72250,72251,72252,72253,72254,72255,72256,72257,72258,72259,72260,72261,72262,72263,72264,72265,72266,72267,72268,72269,72270,72271,72272,72273,72274,72275,72276,72277,72278,72279,72280,72281,72282,72283,72284,72285,72286,72287,72288,72289,72290,72291,72292,72293,72294,72295,72296,72297,72298,72299,72300,72301,72302,72303,72304,72305,72306,72307,72308,72309,72310,72311,72312,72313,72314,72315,72316,72317,72318,72319,72320,72321,72322,72323,72324,72325,72326,72327,72328,72329,72330,72331,72332,72333,72334,72335,72336,72337,72338,72339,72340,72341,72342,72343,72344,72345,72346,72347,72348,72349,72350,72351,72352,72353,72354,72355,72356,72357,72358,72359,72360,72361,72362,72363,72364,72365,72366,72367,72368,72369,72370,72371,72372,72373,72374,72375,72376,72377,72378,72379,72380,72381,72382,72383,72384,72385,72386,72387,72388,72389,72390,72391,72392,72393,72394,72395,72396,72397,72398,72399,72400,72401,72402,72403,72404,72405,72406,72407,72408,72409,72410,72411,72412,72413,72414,72415,72416,72417,72418,72419,72420,72421,72422,72423,72424,72425,72426,72427,72428,72429,72430,72431,72432,72433,72434,72435,72436,72437,72438,72439,72440,72441,72442,72443,72444,72445,72446,72447,72448,72449,72450,72451,72452,72453,72454,72455,72456,72457,72458,72459,72460,72461,72462,72463,72464,72465,72466,72467,72468,72469,72470,72471,72472,72473,72474,72475,72476,72477,72478,72479,72480,72481,72482,72483,72484,72485,72486,72487,72488,72489,72490,72491,72492,72493,72494,72495,72496,72497,72498,72499,72500,72501,72502,72503,72504,72505,72506,72507,72508,72509,72510,72511,72512,72513,72514,72515,72516,72517,72518,72519,72520,72521,72522,72523,72524,72525,72526,72527,72528,72529,72530,72531,72532,72533,72534,72535,72536,72537,72538,72539,72540,72541,72542,72543,72544,72545,72546,72547,72548,72549,72550,72551,72552,72553,72554,72555,72556,72557,72558,72559,72560,72561,72562,72563,72564,72565,72566,72567,72568,72569,72570,72571,72572,72573,72574,72575,72576,72577,72578,72579,72580,72581,72582,72583,72584,72585,72586,72587,72588,72589,72590,72591,72592,72593,72594,72595,72596,72597,72598,72599,72600,72601,72602,72603,72604,72605,72606,72607,72608,72609,72610,72611,72612,72613,72614,72615,72616,72617,72618,72619,72620,72621,72622,72623,72624,72625,72626,72627,72628,72629,72630,72631,72632,72633,72634,72635,72636,72637,72638,72639,72640,72641,72642,72643,72644,72645,72646,72647,72648,72649,72650,72651,72652,72653,72654,72655,72656,72657,72658,72659,72660,72661,72662,72663,72664,72665,72666,72667,72668,72669,72670,72671,72672,72673,72674,72675,72676,72677,72678,72679,72680,72681,72682,72683,72684,72685,72686,72687,72688,72689,72690,72691,72692,72693,72694,72695,72696,72697,72698,72699,72700,72701,72702,72703,72704,72705,72706,72707,72708,72709,72710,72711,72712,72713,72714,72715,72716,72717,72718,72719,72720,72721,72722,72723,72724,72725,72726,72727,72728,72729,72730,72731,72732,72733,72734,72735,72736,72737,72738,72739,72740,72741,72742,72743,72744,72745,72746,72747,72748,72749,72750,72751,72752,72753,72754,72755,72756,72757,72758,72759,72760,72761,72762,72763,72764,72765,72766,72767,72768,72769,72770,72771,72772,72773,72774,72775,72776,72777,72778,72779,72780,72781,72782,72783,72784,72785,72786,72787,72788,72789,72790,72791,72792,72793,72794,72795,72796,72797,72798,72799,72800,72801,72802,72803,72804,72805,72806,72807,72808,72809,72810,72811,72812,72813,72814,72815,72816,72817,72818,72819,72820,72821,72822,72823,72824,72825,72826,72827,72828,72829,72830,72831,72832,72833,72834,72835,72836,72837,72838,72839,72840,72841,72842,72843,72844,72847,72848,72870,72911,72912,72913,72914,72915,72916,72917,72918,72919,72920,72921,72922,72923,72924,72925,72926,72927,72928,72929,72930,72931,72932,72933,72934,72935,72936,72937,72938,72939,72940,72941,72942,72943,72944,72945,72946,72947,72948,72949,72950,72951,72952,72953,72954,72955,72956,72957,72958,72959,72960,72961,72962,72963,72964,72965,72966,72967,72968,72969,72970,72971,72972,72973,72974,72975,72976,72977,72978,72979,72980,72981,72982,72983,72984,72985,72986,72987,72988,72989,72990,72991,72992,72993,72994,72995,72996,72997,72998,72999,73000,73001,73002,73003,73004,73005,73006,73007,73008,73009,73010,73011,73012,73013,73014,73015,73016,73017,73018,73019,73020,73021,73022,73023,73024,73025,73026,73027,73028,73029,73030,73031,73032,73033,73034,73035,73036,73037,73038,73039,73040,73041,73042,73043,73044,73045,73046,73047,73048,73049,73050,73051,73052,73053,73054,73055,73056,73057,73058,73059,73060,73061,73062,73063,73064,73065,73066,73067,73068,73069,73070,73071,73072,73073,73074,73075,73076,73077,73078,73079,73080,73081,73082,73083,73084,73085,73086,73087,73088,73089,73090,73091,73092,73093,73094,73095,73096,73097,73098,73099,73100,73101,73102,73103,73104,73105,73106,73107,73108,73109,73110,73111,73112,73113,73114,73115,73116,73117,73118,73119,73120,73121,73122,73123,73124,73125,73126,73127,73128,73129,73130,73131,73132,73133,73134,73135,73136,73137,73138,73139,73140,73141,73142,73143,73144,73145,73146,73147,73148,73149,73150,73151,73152,73153,73154,73155,73156,73157,73158,73159,73160,73161,73162,73163,73164,73165,73166,73167,73168,73169,73170,73171,73172,73173,73174,73175,73176,73177,73178,73179,73180,73181,73182,73183,73184,73185,73186,73187,73188,73189,73190,73191,73192,73193,73194,73195,73196,73197,73198,73199,73200,73201,73202,73203,73204,73205,73206,73207,73208,73209,73210,73211,73212,73213,73214,73215,73216,73217,73218,73219,73220,73221,73222,73223,73224,73225,73226,73227,73228,73229,73230,73231,73232,73233,73234,73235,73236,73237,73238,73239,73240,73241,73242,73243,73244,73245,73246,73255,73256,73257,73258,73259,73260,73261,73262,73263,73264,73265,73266,73267,73268,73269,73270,73271,73272,73291,73292,73298,73299,73300,73301,73302,73303,73304,73305,73306,73309,73310,73311,73312,73313,73314,73315,73316,73319,73320,73321,73322,73323,73324,73325,73326,73327,73328,73329,73330,73331,73332,73333,73334,73335,73336,73337,73338,73339,};
	cout << "rightLeg: " << (sizeof(faceID) / sizeof(DWORD)) << endl;
	for (int i = 0; i < (int) sizeof(faceID) / sizeof(DWORD); ++i) {
		face_id_set.insert(faceID[i]);
	}
	__defined_area_dic[AREA_RIGHT_LEG] = defArea;
} while (0);
do {	// 목
	kArea::DEFINED_AREA_DIC::iterator	it;
	it = __defined_area_dic.find(AREA_NECK);
	if (it != __defined_area_dic.end())
		__defined_area_dic.erase(it);

	kArea::DEFINED_AREA	defArea;
	defArea.value = .9f;	// 원하는 색깔이...
	std::set<DWORD>&	face_id_set = defArea.face_id_set;
	DWORD	faceID[] = {51668,51676,51678,51679,51690,51691,51692,51693,52331,52332,52333,52334,52335,52336,52337,52338,52339,52340,52341,52342,52343,52344,52345,52349,52350,52351,52352,52354,52355,52356,52357,52358,52359,52360,52361,52362,52363,52364,52365,52366,52367,52368,52369,52370,52371,52372,52373,52374,52375,52376,52377,52378,52379,52380,52381,52445,52446,52584,52617,52618,52987,52988,53129,53130,53145,53146,53157,53158,53161,53169,53171,53172,53183,53184,53185,53186,53824,53825,53826,53827,53828,53829,53830,53831,53832,53833,53834,53835,53836,53837,53838,53842,53843,53844,53845,53847,53848,53849,53850,53851,53852,53853,53854,53855,53856,53857,53858,53859,53860,53861,53862,53863,53864,53865,53866,53867,53868,53869,53870,53871,53872,53873,53874,53938,53939,54077,54110,54111,54480,54481,54622,54623,54638,54639,54650,54651,114132,114133,114134,114135,114136,114137,114138,114139,114140,114141,114142,114143,114144,114145,114146,114147,114148,114149,114150,114151,114152,114153,114154,114155,114156,114157,114158,114159,114160,114161,114162,114163,114164,114165,114166,114167,114168,114169,114170,114171,114172,114173,114174,114175,114176,114177,114178,114179,114180,114181,114182,114183,114184,114185,114186,114187,114188,114189,114190,114191,114192,114193,114194,114195,114196,114197,114198,114199,114200,114201,114279,114280,114281,114282,114283,114284,114320,114321,114322,114323,114324,114325,114326,114327,114328,114329,114330,114331,114332,114333,114334,114335,114336,114337,114338,114339,114340,114341,114342,114343,114344,114345,114346,114347,114348,114349,114350,114351,114352,114353,114354,114355,114356,114357,114358,114359,114360,114361,114362,114363,114364,114365,114366,114367,114368,114369,114370,114371,114372,114373,114374,114375,114376,114377,114378,114379,114380,114381,114382,114383,114384,114385,114386,114387,114388,114389,114390,114391,114392,114393,114394,114395,114396,114397,114398,114399,114400,114401,114402,114403,114404,114405,114406,114407,114408,114409,114410,114411,114412,114413,114414,114415,114416,114417,114418,114419,114420,114421,114422,114423,114424,114425,114426,114427,114428,114429,114430,114431,114432,114433,114434,114435,114436,114437,114438,114439,114440,114441,114442,114443,114444,114445,114446,114447,114448,114450,114451,114452,114454,114455,114458,114459,114460,114461,114462,114463,114464,114465,114466,114467,114468,114469,114470,114471,114472,114473,114474,114475,114479,114480,114481,114482,114483,114484,114485,114486,114487,114490,114491,114492,114493,114500,114501,114504,114505,114506,114507,114508,114509,114510,114511,114512,114513,114514,114515,114516,114517,114518,114519,114520,114521,114522,114523,114526,114527,114528,114529,114534,114535,114536,114537,114538,114539,114540,114548,114549,114550,114551,114556,114557,114558,114559,114560,114561,114562,114563,114566,114567,114578,114579,114580,114581,114582,114583,114584,114585,114586,114587,114588,114589,114596,114597,114598,114599,114600,114601,114602,114603,114604,114605,114606,114607,114608,114609,114610,114611,114612,114613,114618,114619,114620,114621,114622,114623,114624,114625,114626,114627,114651,114652,114653,114654,114655,114656,114660,114661,114662,114663,114664,114665,114672,114674,114675,114676,114677,114683,114684,114685,114686,114687,114688,114689,114690,114729,114730,114731,114732,114733,114734,114735,114736,114737,114738,114739,114740,114741,114742,114743,114744,114745,114746,114747,114748,114749,114750,114751,114752,114753,114754,114755,114756,114757,114758,114759,114760,114761,114762,114763,114764,114765,114766,114767,114768,114769,114770,114771,114772,114773,114774,114775,114776,114777,114778,114779,114780,114781,114782,114783,114784,114785,114786,114787,114788,114789,114790,114791,114792,114793,114794,114795,114796,114797,114798,114876,114877,114878,114879,114880,114881,114917,114918,114919,114920,114921,114922,114923,114924,114925,114926,114927,114928,114929,114930,114931,114932,114933,114934,114935,114936,114937,114938,114939,114940,114941,114942,114943,114944,114945,114946,114947,114948,114949,114950,114951,114952,114953,114954,114955,114956,114957,114958,114959,114960,114961,114962,114963,114964,114965,114966,114967,114968,114969,114970,114971,114972,114973,114974,114975,114976,114977,114978,114979,114980,114981,114982,114983,114984,114985,114986,114987,114988,114989,114990,114991,114992,114993,114994,114995,114996,114997,114998,114999,115000,115001,115002,115003,115004,115005,115006,115007,115008,115009,115010,115011,115012,115013,115014,115015,115016,115017,115018,115019,115020,115021,115022,115023,115024,115025,115026,115027,115028,115029,115030,115031,115032,115033,115034,115035,115036,115037,115038,115039,115040,115041,115042,115043,115044,115045,115047,115048,115049,115051,115052,115055,115056,115057,115058,115059,115060,115061,115062,115063,115064,115065,115066,115067,115068,115069,115070,115071,115072,115076,115077,115078,115079,115080,115081,115082,115083,115084,115087,115088,115089,115090,115097,115098,115101,115102,115103,115104,115105,115106,115107,115108,115109,115110,115111,115112,115113,115114,115115,115116,115117,115118,115119,115120,115123,115124,115125,115126,115131,115132,115133,115134,115135,115136,115137,115145,115146,115147,115148,115153,115154,115155,115156,115157,115158,115159,115160,115163,115164,115175,115176,115177,115178,115179,115180,115181,115182,115183,115184,115185,115186,115193,115194,115195,115196,115197,115198,115199,115200,115201,115202,115203,115204,115205,115206,115207,115208,115209,115210,115215,115216,115217,115218,115219,115220,115221,115222,115223,115224,115248,115249,115250,115251,115252,115253,115257,115258,115259,115260,115261,115262,115269,115271,115272,115273,115274,115280,115281,115282,115283,115284,115285,115286,115287,};
	cout << "neck: " << (sizeof(faceID) / sizeof(DWORD)) << endl;
	for (int i = 0; i < (int) sizeof(faceID) / sizeof(DWORD); ++i) {
		face_id_set.insert(faceID[i]);
	}
	__defined_area_dic[AREA_NECK] = defArea;
} while (0);
do {	// 가슴
	kArea::DEFINED_AREA_DIC::iterator	it;
	it = __defined_area_dic.find(AREA_CHEST);
	if (it != __defined_area_dic.end())
		__defined_area_dic.erase(it);

	kArea::DEFINED_AREA	defArea;
	defArea.value = .9f;	// 원하는 색깔이...
	std::set<DWORD>&	face_id_set = defArea.face_id_set;
	DWORD	faceID[] = {51744,51747,51748,51749,51750,51751,51752,51753,51754,51755,51756,51757,51758,51767,51768,51769,51770,51771,51772,51792,51793,51794,51795,51796,51797,51798,51799,51800,51801,51802,51803,51804,51805,51806,51811,51812,51813,51814,51980,51981,51982,51983,51984,51985,51986,51987,51988,51990,51991,51992,51993,52002,52004,52005,52006,52007,52008,52009,52010,52011,52022,52023,52024,52025,52026,52027,52031,52033,52034,52035,52036,52055,52056,52057,52058,52059,52060,52077,52078,52079,52080,52081,52082,52083,52084,52085,52086,52119,52120,52121,52122,52123,52124,52125,52126,52127,52128,52129,52130,52131,52132,52133,52134,52135,52136,52137,52138,52139,52140,52143,52144,52145,52146,52147,52148,52149,52150,52151,52152,52153,52154,52163,52164,52165,52166,52167,52168,52169,52170,52171,52172,52176,52177,52178,52179,52180,52181,52182,52183,52184,52185,52186,52187,52188,52189,52190,52191,52192,52193,52194,52195,52252,53058,53059,53060,53061,53062,53063,53064,53065,53066,53067,53068,53069,53070,53071,53072,53073,53074,53075,53076,53077,53078,53079,53080,53081,53098,53100,53101,53102,53103,53104,53105,53108,53109,53110,53111,53112,53113,53114,53115,53116,53117,53118,53119,53120,53121,53122,53123,53237,53240,53241,53242,53243,53244,53245,53246,53247,53248,53249,53250,53251,53260,53261,53262,53263,53264,53265,53285,53286,53287,53288,53289,53290,53291,53292,53293,53294,53295,53296,53297,53298,53299,53304,53305,53306,53307,53461,53463,53464,53465,53466,53467,53468,53469,53470,53471,53472,53473,53474,53475,53476,53477,53478,53479,53480,53483,53485,53486,53487,53488,53489,53490,53491,53492,53493,53494,53495,53496,53497,53498,53499,53500,53501,53502,53503,53504,53509,53510,53512,53513,53514,53515,53516,53517,53518,53519,53520,53521,53522,53523,53524,53525,53526,53527,53528,53529,53538,53539,53540,53541,53542,53543,53544,53545,53546,53547,53548,53549,53550,53551,53552,53553,53560,53562,53563,53564,53565,53566,53567,53568,53569,53570,53571,53572,53573,53574,53575,53576,53577,53578,53579,53612,53613,53614,53615,53616,53617,53618,53619,53620,53621,53622,53623,53624,53625,53626,53627,53628,53629,53630,53631,53632,53633,53636,53637,53638,53639,53640,53641,53642,53643,53644,53645,53646,53647,53658,53659,53660,53661,53662,53663,53664,53665,53669,53670,53671,53672,53673,53674,53675,53676,53677,53678,53679,53680,53681,53682,53683,53684,53685,53686,53687,53688,53745,53812,53813,53814,53815,54164,54178,54214,54221,54222,54247,54248,54260,54261,54272,54360,54361,54362,54363,54541,54542,54543,54544,54545,54546,54547,54548,54549,54550,54551,54552,54553,54554,54555,54556,54557,54558,54559,54560,54561,54562,54563,54564,54565,54566,54567,54568,54569,54570,54571,54572,54573,54574,54583,54584,54585,54586,54587,54588,54589,54590,54591,54592,54593,54594,54595,54596,54597,54598,54601,54602,54603,54604,54605,54606,54607,54608,54609,54610,54611,54612,54613,54614,54615,54616,54654,54655,54656,54657,54658,54659,54660,54661,54662,54663,54664,54665,54666,54667,54668,54669,54670,54671,54672,54673,54674,54675,54676,54677,54678,54679,54680,54681,54682,54683,54684,54685,54686,54687,54688,54689,54690,54691,54692,54693,54694,54695,54696,54697,54698,54699,54700,54701,54702,54703,54704,54705,54706,54707,54708,54709,54710,54711,54712,54713,54714,54715,54716,54717,54718,54719,54720,54721,54722,54723,54724,54725,54726,54727,54728,54729,54730,54731,54732,54733,54734,54735,54736,54737,54738,54739,54740,54741,54742,54743,54744,54745,54746,54747,54748,54749,54750,54751,54752,54753,54754,54755,54756,54757,54758,54759,54760,54761,54762,54763,54764,54765,54766,54767,54768,54769,54770,54771,54772,54773,54774,54775,54776,54777,54778,54779,54780,54781,54782,54783,54784,54785,54786,54787,54788,54789,54790,54791,54792,54793,54794,54795,54796,54797,54798,54799,54800,54801,54802,54803,54804,54805,54806,54807,54808,54809,54810,54811,54812,54813,54814,54815,54816,54817,54818,54819,54820,54821,54822,54823,54824,54825,54826,54827,54828,54829,54830,54831,54832,54833,54834,54835,54836,54837,54838,54839,54840,54841,54842,54843,54844,54845,54846,54847,54848,54849,54850,54851,54852,54853,54854,54855,54856,54857,54858,54859,54860,54861,54862,54863,54864,54865,54866,54867,54868,54869,54870,54871,54872,54873,54874,54875,54876,54877,54878,54879,54880,54881,54882,54883,54884,54885,54886,54887,54888,54889,54890,54891,54892,54893,54894,54895,54896,54897,54898,54899,54900,54901,54902,54903,54904,54905,54906,54907,54908,54909,54910,54911,54912,54913,54914,54915,54916,54917,54918,54919,54920,54921,54922,54923,54924,54925,54926,54927,54928,54929,54930,54931,54932,54933,54934,54935,54936,54937,54938,54939,54940,54941,54942,54943,54944,54945,54946,55101,55102,55103,55104,55105,55106,55107,55108,55109,55110,55111,55112,55113,55114,55115,55116,55117,55118,55119,55120,55121,55122,55123,55124,55125,55126,55127,55128,55129,55130,55131,55132,55133,55134,55135,55136,55137,55138,55139,55140,55141,55142,55143,55144,55145,55146,55147,55148,55149,55150,55151,55152,55153,55154,55155,55156,55157,55158,55159,55160,55161,55162,55163,55164,55165,55166,55167,55168,55169,55170,55171,55172,55173,55174,55175,55176,55177,55178,55179,55180,55181,55182,55183,55184,55185,55186,55187,55188,55189,55190,55191,55192,55193,55194,55195,55196,55197,55198,55199,55200,55201,55202,55203,55204,55205,55206,55207,55208,55209,55210,55211,55212,55213,55214,55215,55216,55217,55218,55219,55220,55221,55222,55223,55224,55225,55226,55227,55228,55229,55230,55231,55232,55233,55234,55235,55236,55237,55238,55239,55240,55241,55242,55243,55244,55245,55246,55247,55248,55249,55250,55251,55262,55264,55265,55266,55267,55268,55269,55270,55271,55272,55273,55274,55275,55276,55277,55278,55279,55280,55281,55282,55283,55284,55285,55286,55287,55288,55289,55290,55291,55292,55293,55294,55295,55296,55297,55298,55299,55300,55301,55302,55303,55304,55305,55306,55307,55308,55309,55310,55311,55312,55313,55314,55315,55316,55317,55318,55319,55320,55321,55322,55323,55324,55325,55340,55341,55342,55343,55344,55345,55346,55347,55357,55358,55359,55360,55361,55362,55363,55364,55384,55385,55386,55387,55388,55389,55390,55391,55396,55397,55398,55399,55400,55401,55402,55403,55408,55409,55410,55411,55412,55413,55414,55415,55420,55421,55422,55423,55424,55425,55426,55427,55432,55433,55434,55439,55440,55441,55442,55443,55444,55445,55446,55453,55454,55455,55456,55457,55458,55465,55466,55467,55468,55469,55470,55477,55478,55479,55480,55481,55482,55483,55484,55485,55486,55487,55488,55489,56341,56342,56343,56344,56345,56346,56347,56348,56349,56350,56351,56352,56353,56354,56856,56857,56858,56859,56861,57370,57371,57372,57373,57374,57375,57376,57377,57378,57379,57380,57381,57382,57383,57384,57385,57386,57387,57388,57389,57390,57391,57392,57393,57394,57395,57396,57397,57398,57399,57400,57401,57402,57403,57404,57405,57406,57407,57408,57409,57410,57411,57412,57413,57414,57415,57416,57417,57418,57419,57420,57421,57422,57423,57424,57425,57426,57427,57428,57429,57430,57431,57432,57433,57434,57435,57436,57437,57438,57439,57440,57441,57442,57443,57444,57445,57446,57447,57448,57449,57450,57451,57452,57453,57454,57455,57456,57457,57458,57459,57460,57461,57462,57463,57464,57465,57466,57467,57468,57469,57470,57471,57472,57473,57474,57475,57476,57477,57478,57479,57480,57481,57482,57483,57484,57485,57486,57487,57488,57489,57490,57491,57492,57493,57494,57495,57496,57497,57498,57499,57500,57501,57502,57503,57504,57505,57506,57507,57508,57509,57510,57511,57512,57513,57514,57515,57516,57517,57518,57519,57520,57521,57522,57523,57524,57525,57526,57527,57528,57529,57530,57531,57532,57533,57534,57535,57536,57537,57538,57539,57540,57541,57542,57543,57544,57545,57546,57547,57548,57549,57550,57551,57552,57553,57554,57555,57556,57557,57558,57559,57560,57561,57562,57563,57564,57565,57566,57567,57568,57569,57570,57571,57572,57573,57574,57575,57576,57577,57578,57579,57580,57581,57582,57583,57584,57585,57586,57587,57588,57589,57590,57591,57592,57593,57594,57595,57596,57597,57598,57599,57600,57601,57602,57603,57604,57605,57606,57607,57608,57609,57610,57611,57612,57613,57614,57615,57616,57617,57618,57619,57620,57621,57622,57623,57624,57625,57626,57627,57628,57629,57630,57631,57632,57633,57634,57635,57636,57637,57638,57639,57640,57641,57642,57643,57644,57645,57646,57647,57648,57649,57650,57651,57652,57653,57654,57655,57656,57657,57658,57659,57660,57661,57662,57663,57664,57665,57666,57667,57668,57669,57670,57671,57672,57673,57674,57675,57676,57677,57678,57679,57680,57681,57682,57683,57684,57685,57686,57687,57688,57689,57690,57691,57692,57693,57694,57695,57696,57697,57698,57699,57700,57701,57702,57703,57704,57705,57706,57707,57708,57709,57710,57711,57712,57713,57714,57715,57716,57717,57718,57873,57874,57875,57876,57877,57878,57879,57880,57881,57882,57883,57884,57885,57886,57887,57888,57889,57890,57891,57892,57893,57894,57895,57896,57897,57898,57899,57900,57901,57902,57903,57904,57905,57906,57907,57908,57909,57910,57911,57912,57913,57914,57915,57916,57917,57918,57919,57920,57921,57922,57923,57924,57925,57926,57927,57928,57929,57930,57931,57932,57933,57934,57935,57936,57937,57938,57939,57940,57941,57942,57943,57944,57945,57946,57947,57948,57949,57950,57951,57952,57953,57954,57955,57956,57957,57958,57959,57960,57961,57962,57963,57964,57965,57966,57967,57968,57969,57970,57971,57972,57973,57974,57975,57976,57977,57978,57979,57980,57981,57982,57983,57984,57985,57986,57987,57988,57989,57990,57991,57992,57993,57994,57995,57996,57997,57998,57999,58000,58001,58002,58003,58004,58005,58006,58007,58008,58009,58010,58011,58012,58013,58014,58015,58016,58017,58018,58019,58020,58021,58022,58023,58032,58034,58035,58036,58037,58038,58039,58040,58041,58042,58043,58044,58045,58046,58047,58048,58049,58050,58051,58052,58053,58054,58055,58056,58057,58058,58059,58060,58061,58062,58063,58064,58065,58066,58067,58068,58069,58070,58071,58072,58073,58074,58075,58076,58077,58078,58079,58080,58081,58082,58083,58084,58085,58086,58087,58088,58089,58090,58091,58092,58093,58094,58095,58096,58097,58112,58113,58114,58115,58116,58117,58118,58119,58129,58130,58131,58132,58133,58134,58135,58136,58156,58157,58158,58159,58160,58161,58162,58163,58168,58169,58170,58171,58172,58173,58174,58175,58178,58180,58181,58182,58183,58184,58185,58186,58187,58190,58191,58192,58193,58194,58195,58196,58197,58198,58199,58200,58201,58204,58205,58206,58209,58210,58211,58212,58213,58214,58215,58216,58217,58218,58223,58224,58225,58226,58227,58228,58229,58230,58235,58236,58237,58238,58239,58240,58241,58242,58247,58248,58249,58250,58251,58252,58253,58254,58255,58256,58257,58258,58259,58260,58261,58262,58263,58368,58369,59113,59114,59115,59116,59117,59118,59119,59120,59121,59122,59123,59124,59125,59126,59628,59629,59630,59631,59632,59633,59635,60142,60143,60144,60145,60146,60147,60148,60149,60150,60151,60152,60153,60154,60155,60156,60157,60158,60159,60160,60161,60162,60163,60164,60165,60166,60167,60168,60169,60170,60171,60172,60173,60174,60175,60176,60177,60178,60179,60180,60181,60182,60183,60184,60185,60186,60187,60188,60189,60190,60191,60192,60193,60194,60195,60196,60197,60198,60199,60200,60201,60202,60203,60204,60205,60206,60207,60208,60209,60210,60211,60212,60213,60214,60215,60216,60217,60218,60219,60220,60221,60222,60223,60224,60225,60226,60227,60228,60229,60230,60231,60232,60233,60234,60235,60236,60237,60238,60239,60240,60241,60242,60243,60244,60245,60246,60247,60248,60249,60250,60251,60252,60253,60254,60255,60256,60257,60258,60259,60260,60261,60262,60263,60264,60265,60266,60267,60268,60269,60270,60271,60272,60273,60274,60275,60276,60277,60278,60279,60280,60281,60282,60283,60284,60285,60286,60287,60288,60289,60290,60291,60292,60293,60294,60295,60296,60297,60298,60299,60300,60301,61266,61267,61356,};
	cout << "chest: " << (sizeof(faceID) / sizeof(DWORD)) << endl;
	for (int i = 0; i < (int) sizeof(faceID) / sizeof(DWORD); ++i) {
		face_id_set.insert(faceID[i]);
	}
	__defined_area_dic[AREA_CHEST] = defArea;
} while (0);
do {	// 복부
	kArea::DEFINED_AREA_DIC::iterator	it;
	it = __defined_area_dic.find(AREA_ABDOMINAL);
	if (it != __defined_area_dic.end())
		__defined_area_dic.erase(it);

	kArea::DEFINED_AREA	defArea;
	defArea.value = .9f;	// 원하는 색깔이...
	std::set<DWORD>&	face_id_set = defArea.face_id_set;
	DWORD	faceID[] = {60561,60562,60563,60564,60575,60576,60577,60578,60579,60580,60581,60587,60588,60589,60590,60591,60592,60593,60594,60597,60598,60610,60611,60633,60634,60660,60684,60685,60690,60691,60696,60697,60700,60701,60736,60737,60738,60739,60740,60741,60742,61114,61115,61116,61117,61492,61493,61494,61495,61506,61507,61508,61509,61510,61511,61512,61518,61519,61520,61521,61522,61523,61524,61525,61528,61529,61564,61565,61615,61616,61621,61622,61627,61628,61631,61632,61668,61670,61671,62045,62046,62047,62048,62198,62199,62200,62201,62202,62203,62204,62205,62206,62207,62208,62209,62212,62214,62215,62216,62217,62218,62219,62220,62223,62225,62226,62227,62228,62229,62230,62231,62232,62233,62234,62235,62236,62237,62238,62239,62240,62241,62242,62243,62244,62245,62246,62247,62248,62249,62250,62251,62252,62253,62254,62255,62256,62257,62258,62259,62260,62261,62262,62263,62264,62265,62274,62275,62276,62277,62278,62279,62280,62281,62282,62283,62284,62285,62286,62287,62288,62289,62290,62291,62292,62293,62294,62295,62296,62297,62298,62299,62300,62301,62302,62303,62304,62305,62306,62307,62308,62309,62310,62311,62312,62313,62314,62315,62316,62317,62318,62319,62320,62321,62322,62323,62324,62325,62326,62327,62328,62329,62330,62331,62332,62333,62334,62335,62336,62337,62338,62339,62340,62351,62352,62403,62404,62425,62426,62427,62428,62429,62430,62435,62436,62437,62438,62439,62440,62488,62489,62490,62491,62492,62493,62494,62495,62496,62506,62521,62522,62523,62524,62525,62526,62527,62528,62529,62530,62531,62532,62533,62534,62535,62536,62537,62538,62649,62650,62651,62652,62696,62697,62698,62699,62700,62701,62702,62704,62705,62706,62707,62708,62709,62710,62711,62712,62713,62715,62716,62717,62718,62719,62720,62721,62722,62723,62724,62725,62726,62727,62728,62729,62730,62731,62732,62733,62734,62735,62736,62737,62784,62785,62786,62787,62788,62789,62834,62835,62836,62837,62838,62847,62848,62851,62852,62853,62854,62922,62923,62924,62925,62926,62927,62928,62929,62930,62931,62932,62933,62938,62940,62941,62942,62943,62944,62951,62954,62955,62956,62957,62958,62959,62960,62961,62962,62963,62964,62965,62966,62967,62968,62969,62970,62971,62972,62973,62974,62975,62976,62977,62978,62979,62980,62981,62982,62983,62984,62985,62986,62987,62988,62989,62998,62999,63000,63001,63002,63003,63004,63005,63006,63007,63008,63009,63010,63011,63012,63013,63014,63015,63016,63017,63019,63020,63021,63022,63023,63024,63025,63026,63027,63028,63029,63030,63031,63032,63033,63034,63035,63036,63037,63038,63039,63040,63041,63042,63043,63044,63045,63046,63047,63048,63049,63050,63051,63052,63053,63054,63055,63056,63057,63058,63059,63060,63061,63062,63063,63064,63075,63076,63127,63128,63149,63150,63151,63152,63153,63154,63159,63160,63161,63162,63163,63164,63212,63213,63214,63215,63216,63217,63218,63219,63220,63224,63229,63230,63245,63246,63247,63248,63249,63250,63251,63252,63253,63254,63255,63256,63257,63258,63259,63260,63261,63262,63373,63374,63375,63376,63420,63421,63422,63423,63424,63425,63426,63428,63429,63430,63431,63432,63433,63434,63435,63439,63440,63441,63442,63443,63444,63445,63446,63447,63448,63449,63450,63451,63452,63453,63454,63455,63456,63457,63458,63459,63460,63461,63508,63509,63510,63511,63512,63513,63558,63559,63560,63561,63562,63575,63576,63577,63578,63701,63702,63703,63906,63927,63928,63931,63932,63933,63934,63935,63936,63937,63938,63939,63940,63941,63942,63943,63954,63955,63956,63957,63969,63970,63971,63972,63973,63974,64025,64026,64027,64028,64029,64030,64031,64032,64033,64034,64035,64151,64155,64174,64177,64178,64180,64321,64322,64323,64324,64325,64326,64327,64328,64329,64330,64331,64332,64333,64334,64335,64336,64337,64338,64339,64340,64341,64343,64344,64345,64346,64347,64348,64349,64350,64351,64352,64353,64354,64355,64356,64357,64358,64359,64360,64361,64362,64363,64364,64365,64366,64486,64487,64488,64489,64490,64491,64492,64493,64545,64570,64571,64572,64573,64574,65512,65513,65514,65717,65738,65739,65742,65743,65744,65745,65746,65747,65748,65749,65750,65751,65752,65753,65754,65765,65766,65767,65768,65780,65781,65782,65783,65784,65785,65836,65837,65838,65839,65840,65841,65842,65843,65844,65845,65846,65962,65966,65985,65988,65989,65991,66132,66133,66134,66135,66136,66137,66138,66139,66140,66141,66142,66143,66144,66145,66146,66147,66148,66150,66154,66155,66156,66157,66158,66159,66160,66161,66162,66163,66164,66165,66166,66167,66168,66169,66170,66171,66172,66173,66174,66175,66176,66177,66297,66298,66299,66300,66301,66302,66303,66304,66356,66381,66382,66383,66384,66385,};
	cout << "abdominal: " << (sizeof(faceID) / sizeof(DWORD)) << endl;
	for (int i = 0; i < (int) sizeof(faceID) / sizeof(DWORD); ++i) {
		face_id_set.insert(faceID[i]);
	}
	__defined_area_dic[AREA_ABDOMINAL] = defArea;
} while (0);
do {	// 엉덩이
	kArea::DEFINED_AREA_DIC::iterator	it;
	it = __defined_area_dic.find(AREA_HIP);
	if (it != __defined_area_dic.end())
		__defined_area_dic.erase(it);

	kArea::DEFINED_AREA	defArea;
	defArea.value = .9f;	// 원하는 색깔이...
	std::set<DWORD>&	face_id_set = defArea.face_id_set;
	DWORD	faceID[] = {62341,62342,62343,62344,62345,62346,62347,62348,62349,62350,62364,62365,62366,62367,62368,62369,62370,62371,62372,62373,62379,62380,62405,62406,62407,62408,62409,62410,62411,62412,62413,62414,62415,62416,62417,62418,62419,62420,62421,62422,62423,62424,62431,62432,62441,62457,62458,62459,62460,62462,62463,62467,62468,62469,62470,62471,62472,62473,62474,62475,62476,62477,62478,62479,62480,62481,62482,62483,62484,62485,62486,62487,62497,62498,62515,62516,62517,62518,62519,62520,62539,62540,62541,62653,62654,62655,62656,62657,62658,62659,62660,62661,62662,62663,62664,62665,62666,62667,62668,62669,62670,62671,62672,62673,62674,62675,62676,62677,62678,62679,62680,62681,62682,62683,62684,62685,62686,62687,62688,62693,62694,62695,62742,62743,62773,62774,62775,62776,62777,62778,62779,62790,62791,62792,62793,62800,62801,62802,62803,62804,62805,62806,62807,62808,62809,62810,62811,62812,62813,62814,62815,62816,62817,62818,62819,62820,62821,62822,62823,62824,62825,62826,62827,62828,62829,62830,62831,62832,62833,62840,62855,62856,62857,62858,62859,62860,62861,62862,62863,62864,62865,62866,62867,62868,62869,62870,62871,62872,62873,62874,62875,62876,62877,62878,62879,62880,62881,62882,62883,62884,63065,63066,63067,63068,63069,63070,63071,63072,63073,63074,63088,63089,63090,63091,63092,63093,63094,63095,63096,63097,63103,63104,63129,63130,63131,63132,63133,63134,63135,63136,63137,63138,63139,63140,63141,63142,63143,63144,63145,63146,63147,63148,63155,63156,63165,63181,63182,63183,63184,63187,63191,63192,63196,63197,63198,63199,63200,63201,63202,63203,63204,63205,63206,63207,63208,63209,63210,63211,63221,63222,63239,63240,63241,63242,63243,63244,63263,63377,63378,63379,63380,63381,63382,63383,63384,63385,63386,63387,63388,63389,63390,63391,63392,63393,63394,63395,63396,63397,63398,63399,63400,63401,63402,63403,63404,63405,63406,63407,63408,63409,63410,63411,63412,63497,63498,63499,63514,63515,63516,63517,63518,63519,63524,63525,63526,63527,63528,63529,63530,63531,63532,63533,63534,63535,63536,63537,63538,63539,63540,63541,63542,63543,63544,63545,63546,63547,63548,63549,63550,63551,63554,63555,63564,63579,63580,63583,63584,63585,63586,63587,63588,63589,63590,63591,63592,63593,63594,63595,63596,63597,63598,63599,63600,63601,63602,63603,63604,63605,63606,63607,63608,63813,63814,63815,63816,63817,63818,63819,63820,63857,63858,63859,63860,63861,63862,63863,63864,63867,63868,64047,64048,64049,64050,64051,64052,64053,64054,64055,64056,64057,64058,64059,64060,64061,64062,64063,64064,64067,64068,64069,64076,64077,64078,64079,64080,64081,64082,64083,64084,64085,64086,64087,64088,64089,64094,64095,64096,64097,64100,64101,64102,64103,64104,64105,64106,64107,64108,64109,64110,64111,64112,64113,64114,64115,64116,64117,64118,64119,64120,64121,64122,64123,64124,64125,64126,64127,64128,64129,64130,64131,64132,64133,64134,64135,64136,64137,64138,64139,64140,64141,64142,64143,64144,64145,64146,64147,64181,64182,64183,64184,64185,64186,64187,64188,64189,64190,64191,64192,64193,64194,64195,64196,64197,64198,64199,64200,64201,64202,64203,64204,64207,64208,64209,64210,64211,64212,64213,64214,64215,64216,64217,64218,64227,64228,64229,64230,64231,64232,64233,64234,64235,64236,64237,64238,64239,64240,64241,64242,64243,64244,64245,64272,64273,64274,64275,64276,64277,64278,64279,64280,64281,64282,64283,64284,64285,64286,64287,64288,64289,64290,64291,64292,64293,64294,64295,64296,64297,64298,64299,64300,64301,64302,64303,64304,64305,64306,64307,64308,64309,64310,64311,64312,64313,64314,64315,64316,64317,64318,64319,64320,64412,64413,64414,64415,64416,64417,64418,64419,64420,64421,64422,64423,64424,64425,64426,64427,64428,64429,64430,64431,64432,64433,64436,64438,64439,64440,64441,64442,64443,64444,64445,64446,64447,64450,64452,64475,64476,64477,64478,64479,64480,64481,64482,64483,64484,64485,64508,64509,64510,64511,64554,64555,64556,64557,64558,64559,64560,64561,64632,64633,64634,64635,64636,64637,64638,64639,64640,64641,64642,64643,64644,64645,64646,64647,64648,64649,64650,64651,64652,64653,64654,64655,64678,64679,64680,64683,64684,64689,64690,64743,64744,64745,64746,64747,64748,64749,64754,64755,64757,64758,64759,64764,64765,64766,64767,64768,64773,64774,64775,64776,64777,64778,64779,64780,64781,64782,64783,64784,64785,64786,64787,64788,64789,64790,64791,64792,64793,64794,64814,64815,64816,64817,64818,64819,64820,64821,64822,64823,64824,64825,64832,64833,64834,64835,64836,64837,64838,64839,64840,64841,64842,64843,64863,64864,64865,64866,64869,64870,64871,64872,64873,64874,64875,64876,64877,64878,64879,64880,64881,64882,64883,64884,64885,64886,64887,64888,64896,64897,64900,64901,64906,64907,64908,64909,64910,64911,64912,64913,64914,64915,64916,64917,64918,64919,64920,64921,64922,64923,64925,64926,64929,64930,64931,64932,64933,64934,64935,64936,64939,64940,64943,64944,64945,64946,64947,64948,64949,64950,64951,64952,64953,64954,64957,64958,64959,64960,64961,64962,64972,64973,64974,64975,64976,64977,64980,64981,64986,64987,64988,64989,64990,64991,64992,64993,64994,64995,64996,64997,64998,65009,65010,65015,65016,65017,65018,65019,65020,65021,65022,65023,65024,65025,65026,65027,65028,65029,65030,65031,65032,65033,65034,65035,65036,65037,65038,65039,65040,65041,65042,65043,65044,65049,65050,65051,65052,65055,65056,65057,65058,65059,65060,65061,65062,65063,65064,65065,65066,65067,65068,65069,65070,65071,65072,65073,65074,65075,65076,65077,65078,65079,65080,65081,65082,65083,65084,65085,65086,65087,65088,65089,65090,65091,65092,65093,65094,65095,65096,65097,65098,65106,65107,65108,65109,65110,65111,65112,65113,65114,65115,65116,65117,65118,65119,65120,65123,65124,65125,65126,65127,65128,65160,65161,65162,65163,65166,65167,65168,65169,65171,65172,65173,65174,65175,65176,65177,65178,65179,65180,65183,65184,65185,65186,65187,65188,65189,65190,65191,65192,65193,65194,65195,65196,65197,65198,65199,65200,65201,65202,65203,65204,65205,65206,65207,65208,65209,65210,65211,65212,65213,65214,65215,65216,65217,65218,65219,65220,65221,65222,65223,65224,65225,65226,65227,65228,65229,65230,65231,65232,65233,65234,65235,65236,65237,65238,65239,65240,65241,65242,65243,65244,65245,65246,65247,65248,65249,65250,65251,65252,65253,65254,65255,65256,65257,65258,65259,65260,65261,65262,65263,65264,65265,65266,65267,65268,65269,65270,65271,65272,65273,65274,65275,65276,65277,65278,65279,65280,65281,65282,65283,65284,65285,65286,65287,65288,65289,65290,65291,65292,65293,65294,65295,65296,65297,65298,65299,65300,65301,65302,65303,65304,65305,65306,65307,65308,65309,65310,65311,65312,65313,65314,65315,65316,65317,65318,65319,65320,65321,65322,65323,65324,65325,65326,65327,65328,65329,65330,65331,65332,65333,65334,65335,65336,65337,65338,65339,65340,65341,65342,65343,65344,65345,65346,65347,65348,65349,65350,65351,65352,65353,65354,65355,65356,65357,65358,65359,65360,65361,65362,65363,65364,65365,65366,65367,65368,65369,65370,65371,65372,65373,65374,65375,65376,65377,65378,65379,65380,65382,65383,65384,65385,65386,65391,65392,65393,65394,65395,65396,65397,65398,65399,65400,65401,65402,65403,65404,65405,65406,65407,65408,65409,65410,65411,65412,65413,65414,65415,65416,65417,65418,65419,65420,65421,65422,65624,65625,65626,65627,65628,65629,65630,65631,65668,65669,65670,65671,65672,65673,65674,65675,65676,65677,65678,65679,65858,65859,65860,65861,65862,65863,65864,65865,65866,65867,65868,65869,65870,65871,65872,65873,65874,65875,65878,65879,65880,65887,65888,65889,65890,65891,65892,65893,65894,65895,65896,65897,65898,65899,65900,65905,65906,65907,65908,65911,65912,65913,65914,65915,65916,65917,65918,65919,65920,65921,65922,65923,65924,65925,65926,65927,65928,65929,65930,65931,65932,65933,65934,65935,65936,65937,65938,65939,65940,65941,65942,65943,65944,65945,65946,65947,65948,65949,65950,65951,65952,65953,65954,65955,65956,65957,65958,65992,65993,65994,65995,65996,65997,65998,65999,66000,66001,66002,66003,66004,66005,66006,66007,66008,66009,66010,66011,66012,66013,66014,66015,66018,66019,66020,66021,66022,66023,66024,66025,66026,66027,66028,66029,66038,66039,66040,66041,66042,66043,66044,66045,66046,66047,66048,66049,66050,66051,66052,66053,66054,66056,66083,66084,66085,66086,66087,66088,66089,66090,66091,66092,66093,66094,66095,66096,66097,66098,66099,66100,66101,66102,66103,66104,66105,66106,66107,66108,66109,66110,66111,66112,66113,66114,66115,66116,66117,66118,66119,66120,66121,66122,66123,66124,66125,66126,66127,66128,66129,66130,66131,66223,66224,66225,66226,66227,66228,66229,66230,66231,66232,66233,66234,66235,66236,66237,66238,66239,66240,66241,66242,66243,66244,66247,66249,66250,66251,66252,66253,66254,66255,66256,66257,66258,66261,66263,66286,66287,66288,66289,66290,66291,66292,66293,66294,66295,66296,66319,66320,66321,66322,66365,66366,66367,66368,66369,66370,66371,66372,66443,66444,66445,66446,66447,66448,66449,66450,66451,66452,66453,66454,66455,66456,66457,66458,66459,66460,66461,66462,66463,66464,66465,66466,66489,66490,66491,66494,66495,66500,66501,66554,66555,66556,66557,66558,66559,66560,66565,66566,66569,66570,66575,66576,66577,66578,66579,66584,66585,66586,66587,66588,66589,66590,66591,66592,66593,66594,66595,66596,66597,66598,66599,66600,66601,66602,66603,66604,66605,66625,66626,66627,66628,66629,66630,66631,66632,66633,66634,66635,66636,66643,66644,66645,66646,66647,66648,66649,66650,66651,66652,66653,66654,66674,66675,66676,66677,66682,66683,66684,66685,66686,66687,66688,66689,66690,66691,66692,66693,66694,66695,66696,66697,66698,66699,66711,66712,66717,66718,66719,66720,66721,66722,66723,66724,66725,66726,66727,66728,66729,66730,66731,66732,66733,66734,66736,66737,66742,66743,66744,66745,66746,66747,66754,66755,66756,66757,66758,66759,66760,66761,66762,66763,66764,66765,66768,66769,66770,66771,66772,66773,66783,66784,66785,66786,66787,66788,66791,66792,66797,66798,66799,66800,66801,66802,66803,66804,66805,66806,66807,66808,66809,66820,66821,66826,66827,66828,66829,66830,66831,66832,66833,66834,66835,66836,66837,66838,66839,66840,66841,66842,66843,66844,66845,66846,66847,66848,66849,66850,66851,66852,66853,66854,66855,66860,66861,66862,66863,66866,66867,66868,66869,66870,66871,66872,66873,66874,66875,66876,66877,66878,66879,66880,66881,66882,66883,66884,66885,66886,66887,66888,66889,66890,66891,66892,66893,66894,66895,66896,66897,66898,66899,66900,66901,66902,66903,66904,66905,66906,66907,66908,66909,66917,66918,66919,66920,66921,66922,66923,66924,66925,66926,66927,66928,66929,66930,66931,66934,66935,66936,66937,66938,66939,66979,66980,66983,66984,66985,66986,66987,66988,66989,66990,66991,66998,66999,67001,67002,67003,67005,67008,67009,67010,67011,67012,67013,67014,67015,67016,67017,67018,67019,67020,67021,67022,67023,67024,67025,67026,67027,67028,67029,67030,67031,67032,67033,67034,67035,67036,67037,67038,67039,67040,67041,67042,67043,67044,67045,67046,67047,67048,67049,67050,67051,67052,67053,67054,67055,67056,67057,67058,67059,67060,67061,67062,67063,67064,67065,67066,67067,67068,67069,67070,67071,67072,67073,67074,67075,67076,67077,67078,67079,67080,67081,67082,67083,67084,67085,67086,67087,67088,67089,67090,67091,67092,67093,67094,67095,67096,67097,67098,67099,67100,67101,67102,67103,67104,67105,67106,67107,67108,67109,67110,67111,67112,67113,67114,67115,67116,67117,67118,67119,67120,67121,67122,67123,67124,67125,67126,67127,67128,67129,67130,67131,67132,67133,67134,67136,67137,67138,67139,67140,67141,67142,67143,67144,67145,67146,67147,67148,67149,67150,67151,67152,67153,67154,67155,67156,67157,67158,67159,67160,67161,67162,67163,67164,67165,67166,67167,67168,67169,67170,67171,67172,67173,67174,67175,67176,67177,67178,67179,67180,67181,67182,67183,67184,67185,67186,67187,67188,67189,67190,67191,67193,67194,67195,67196,67197,67202,67203,67204,67205,67206,67207,67208,67209,67210,67211,67212,67213,67214,67215,67216,67217,67218,67219,67220,67221,67222,67223,67224,67225,67226,67227,67228,67229,67230,67231,67232,67233,};
	cout << "hip: " << (sizeof(faceID) / sizeof(DWORD)) << endl;
	for (int i = 0; i < (int) sizeof(faceID) / sizeof(DWORD); ++i) {
		face_id_set.insert(faceID[i]);
	}
	__defined_area_dic[AREA_HIP] = defArea;
} while (0);
do {	// 왼쪽 팔뚝(상완)
	kArea::DEFINED_AREA_DIC::iterator	it;
	it = __defined_area_dic.find(AREA_LEFT_FORE_ARM);
	if (it != __defined_area_dic.end())
		__defined_area_dic.erase(it);

	kArea::DEFINED_AREA	defArea;
	defArea.value = .9f;	// 원하는 색깔이...
	std::set<DWORD>&	face_id_set = defArea.face_id_set;
	DWORD	faceID[] = {55794,55795,55804,55805,55813,55814,55815,55816,55817,55818,55819,55820,55892,55893,55894,55895,55896,55897,55898,55899,55908,55909,55910,55911,55923,55924,55971,55972,55973,55974,55979,55980,55981,55982,56011,56012,56015,56016,56019,56020,56023,56024,56025,56026,56027,56028,56029,56030,56055,56056,56062,56063,56115,56116,56117,56118,56124,56125,56138,56139,56214,56215,56221,56222,56232,56233,56234,56235,56236,56237,56253,56254,56255,56256,56257,56258,56259,56260,56261,56264,56265,56266,56267,56268,56269,56270,56271,56283,56288,56289,56290,56291,56292,56299,56300,56301,56302,56303,56304,56340,56494,56495,56504,56505,56516,56517,56518,56525,56546,56547,56548,56549,56570,56571,56572,56573,56584,56585,56586,56587,56702,56715,56717,56718,56730,56731,56732,56733,56774,56775,56920,56921,56922,56923,56926,56927,56928,56929,56930,56931,56932,56933,56934,56935,56936,56937,56938,56939,56940,56941,56942,56943,56944,56945,56946,56947,56948,56949,56950,56951,56952,56953,56954,56955,56956,56957,56958,56959,56960,56961,56962,56963,56964,56965,56966,56967,56968,56969,56970,56971,56972,56973,56974,56975,56976,56977,56978,56979,56980,56981,56982,56983,56984,56985,56986,56987,56988,56989,56990,56991,56992,56993,56994,56995,56996,56997,56998,56999,57000,57001,57002,57003,57004,57005,57006,57007,57008,57009,57010,57011,57012,57013,57014,57015,57016,57017,57018,57019,57020,57021,57022,57023,57024,57025,57026,57027,57028,57029,57030,57031,57032,57033,57034,57035,57036,57037,57038,57039,57040,57041,57042,57043,57044,57045,57046,57047,57048,57049,57050,57051,57052,57053,57054,57055,57056,57057,57058,57059,57060,57061,57062,57063,57064,57065,57066,57067,57068,57069,57070,57071,57072,57073,57074,57075,57076,57077,57078,57079,57080,57081,57082,57083,57084,57085,57086,57087,57088,57089,57090,57091,57092,57093,57094,57095,57096,57097,57098,57099,57100,57101,57102,57103,57104,57105,57106,57107,57108,57109,57110,57111,57112,57113,57114,57115,57116,57117,57118,57119,57120,57121,57122,57123,57124,57125,57126,57127,57128,57129,57130,57131,57132,57133,57134,57135,57136,57137,57138,57139,57140,57141,57142,57143,57144,57145,57146,57147,57148,57149,57150,57151,57152,57153,57154,57155,57156,57157,57158,57159,57160,57161,57162,57163,57164,57165,57166,57167,57168,57169,57170,57171,57172,57173,57174,57175,57176,57177,57178,57179,57180,57181,57182,57183,57184,57185,57186,57187,57188,57189,57190,57191,57192,57193,57194,57195,57196,57197,57198,57199,57200,57201,57202,57203,57204,57205,57206,57207,57208,57209,57210,57211,57212,57213,57214,57215,57216,57217,57218,57219,57220,57221,57222,57223,57224,57225,57226,57227,57228,57229,57230,57231,57232,57233,57234,57235,57236,57237,57238,57239,57240,57241,57242,57243,57244,57245,57246,57247,57248,57249,57250,57251,57252,57253,57254,57255,57256,57257,57258,57259,57260,57261,57262,57263,57264,57265,57266,57267,57268,57269,57270,57271,57272,57273,57274,57275,57276,57277,57278,57279,57280,57281,57282,57283,57284,57285,57286,57287,57288,57289,57290,57291,57292,57293,57294,57295,57296,57297,57298,57299,57300,57301,57302,57303,57304,57305,57306,57307,57308,57309,57310,57311,57312,57313,57314,57315,57316,57317,57318,57319,57320,57321,57322,57323,57324,57325,57326,57327,57328,57329,57330,57331,57332,57333,57334,57335,57336,57337,57338,57339,57340,57341,57342,57343,57344,57345,57346,57347,57348,57349,57350,57351,57352,57353,57354,57355,57356,57357,57358,57359,57360,57361,57362,57363,57364,79818,79819,79820,79821,79822,79823,79824,79825,79826,79827,79828,79829,79830,79831,79832,79833,79834,79835,79836,79837,79838,79839,79840,79841,79842,79843,79989,79990,79991,79992,79993,79994,79995,79996,80004,80005,80008,80009,80014,80015,80022,80023,80077,80078,80079,80080,80081,80082,80083,80084,80085,80086,80087,80088,80089,80090,80091,80092,80093,80094,80095,80096,80097,80098,80099,80100,80101,80102,80103,80104,80105,80106,80107,80108,80109,80110,80111,80112,80113,80114,80115,80116,80117,80118,80119,80120,80121,80122,80123,80124,80125,80126,80127,80128,80129,80130,80131,80132,80133,80134,80135,80136,80137,80138,80139,80140,80141,80142,80143,80144,80145,80146,80147,80148,80149,80150,80151,80152,80153,80154,80155,80156,80157,80158,80159,80160,80161,80162,80163,80164,80165,80166,80167,80168,80169,80170,80171,80172,80173,80174,80175,80176,80177,80178,80179,80180,80181,80182,80183,80184,80185,80186,80187,80188,80189,80190,80191,80192,80193,80194,80195,80196,80197,80198,80199,80200,80201,80202,80203,80204,80205,80206,80207,80208,80209,80210,80211,80212,80213,80214,80215,80216,80217,80218,80219,80220,80221,80222,80223,80224,80225,80226,80227,80228,80229,80230,80231,80232,80233,80253,80254,80255,80256,80257,80258,80259,80260,80261,80262,80263,80264,80265,80266,80267,80268,80269,80270,80271,80272,80273,80274,80275,80276,80277,80278,80279,80280,80281,80282,80283,80284,80285,80286,80287,80288,80289,80290,80291,80292,80293,80294,80295,80296,80297,80298,80299,80300,80301,80302,80303,80304,80305,80306,80307,80308,80309,80310,80311,80312,80313,80314,80315,80316,80317,80318,80319,80320,80321,80322,80327,80328,80331,80332,80333,80334,80343,80344,80345,80346,80351,80352,80353,80354,80355,80356,80357,80358,80359,80360,80361,80362,80365,80366,80369,80370,80373,80374,80377,80378,80381,80382,80391,80392,80393,80394,80395,80396,80397,80398,80399,80400,80401,80402,80403,80404,80405,80406,80407,80408,80409,80410,80411,80412,80413,80414,80415,80416,80417,80418,80419,80420,80421,80422,80423,80424,80425,80426,80427,80428,80429,80430,80431,80432,80433,80434,80435,80436,80437,80438,80439,80440,80441,80442,80443,80444,80445,80446,80447,80448,80449,80450,80451,80452,80453,80454,80455,80456,80457,80458,80459,80460,80461,80462,80463,80464,80465,80466,80467,80468,80469,80470,80471,80472,80473,80474,80475,80476,80477,80478,80479,80480,80481,80482,80483,80484,80485,80486,80487,80488,80489,80490,80491,80492,80493,80494,80495,80496,80497,80498,80499,80500,80501,80502,80503,80504,80505,80506,80507,80508,80509,80510,80511,80512,80513,80514,80515,80516,80517,80518,80519,80520,80521,80522,80523,80524,80525,80526,80527,80528,80529,80530,80531,80532,80533,80534,80535,80536,80537,80538,80539,80540,80541,80542,80543,80544,80545,80546,80547,80548,80549,80550,80551,80552,80553,80554,80555,80556,80557,80558,80559,80560,80561,80562,80563,80564,80565,80566,80567,80568,80569,80570,80571,80572,80573,80574,80575,80576,80577,80578,80579,80580,80581,80582,80583,80584,80585,80586,80587,80588,80589,80590,80591,80592,80593,80594,80595,80596,80597,80598,80599,80600,80601,80602,80603,80604,80605,80606,80607,80608,80609,80610,80611,80612,80613,80614,80615,80616,80617,80618,80619,80620,80621,80622,80623,80624,80625,80626,80627,80628,80629,80630,80631,80632,80633,80634,80635,80636,80637,80638,80639,80640,80641,80642,80643,80644,80654,80655,80656,80657,80658,80659,80660,80661,80662,80663,80664,80665,80678,80679,80680,80681,80682,80683,80684,80685,80686,80687,80688,80689,80690,80691,80692,80693,80694,80695,80696,80703,80704,80705,80706,80707,80708,80709,80710,80711,80712,80713,80714,80715,80716,80717,80718,80778,80779,80780,80781,80782,80783,80784,80785,80786,80787,80788,80789,80790,80791,80792,80793,80794,80795,80796,80797,80798,80799,80800,80801,80802,80803,80804,80805,80806,80807,80808,80809,80810,80811,80812,80813,80814,80815,80816,80817,80818,80819,80822,80823,80824,80825,80826,80827,80828,80829,80830,80831,80832,80833,80834,80835,80836,80837,80838,80839,80840,80841,80842,80843,80844,80845,80846,80847,80848,80849,80850,80851,80852,80853,80854,80855,80856,80857,80858,80859,80860,80861,80862,80863,80864,80865,80866,80867,80868,80869,80870,80871,80872,80873,80874,80875,80876,80877,80878,80879,80880,80881,80882,80883,80884,80885,80886,80887,80888,80889,80890,80891,80892,80893,80894,80895,80896,80897,80898,80899,80900,80901,80902,80903,80904,80905,80906,80907,80908,80909,80910,80911,80912,80913,80914,80915,80916,80917,80918,80919,80920,80921,80922,80923,80924,80925,80926,80927,80928,80929,80930,80931,80932,80933,80934,80935,80936,80937,80938,80939,80940,80941,80942,80943,80944,80945,80946,80947,80948,80949,80950,80951,80952,80953,80954,80955,80956,80957,80958,80959,80960,80961,80962,80963,80964,80965,80966,80967,80968,80969,80970,80971,80972,80973,80974,80975,80976,80977,80978,80979,80980,80981,80982,80983,80984,80985,80986,80987,80988,80989,80990,80991,80992,80993,80994,80995,80996,80997,81090,81091,81092,81093,81094,81095,81096,81097,81098,81099,81100,81101,81102,81103,};
	cout << "leftForearm: " << (sizeof(faceID) / sizeof(DWORD)) << endl;
	for (int i = 0; i < (int) sizeof(faceID) / sizeof(DWORD); ++i) {
		face_id_set.insert(faceID[i]);
	}
	__defined_area_dic[AREA_LEFT_FORE_ARM] = defArea;
} while (0);
do {	// 오른쪽 팔뚝(상완)
	kArea::DEFINED_AREA_DIC::iterator	it;
	it = __defined_area_dic.find(AREA_RIGHT_FORE_ARM);
	if (it != __defined_area_dic.end())
		__defined_area_dic.erase(it);

	kArea::DEFINED_AREA	defArea;
	defArea.value = .9f;	// 원하는 색깔이...
	std::set<DWORD>&	face_id_set = defArea.face_id_set;
	DWORD	faceID[] = {58566,58567,58576,58577,58585,58586,58589,58590,58591,58592,58664,58665,58666,58667,58668,58669,58670,58671,58751,58752,58753,58754,58783,58784,58787,58788,58791,58792,58795,58796,58797,58798,58799,58800,58801,58802,58993,58994,59006,59007,59036,59037,59038,59039,59040,59041,59042,59043,59055,59060,59061,59062,59063,59064,59073,59074,59075,59076,59112,59266,59267,59276,59277,59290,59318,59319,59320,59321,59342,59343,59344,59345,59356,59357,59358,59359,59474,59489,59490,59504,59505,59546,59547,59694,59695,59698,59699,59700,59701,59702,59703,59704,59705,59706,59707,59708,59709,59710,59711,59712,59713,59714,59715,59716,59717,59718,59719,59720,59721,59722,59723,59724,59725,59726,59727,59728,59729,59730,59731,59732,59733,59734,59735,59736,59737,59738,59739,59740,59741,59742,59743,59744,59745,59746,59747,59748,59749,59750,59751,59752,59753,59754,59755,59756,59757,59758,59759,59760,59761,59762,59763,59764,59765,59766,59767,59768,59769,59770,59771,59772,59773,59774,59775,59776,59777,59778,59779,59780,59781,59782,59783,59784,59785,59786,59787,59788,59789,59790,59791,59792,59793,59794,59795,59796,59797,59798,59799,59800,59801,59802,59803,59804,59805,59806,59807,59808,59809,59810,59811,59812,59813,59814,59815,59816,59817,59818,59819,59820,59821,59822,59823,59824,59825,59826,59827,59828,59829,59830,59831,59832,59833,59834,59835,59836,59837,59838,59839,59840,59841,59842,59843,59844,59845,59846,59847,59848,59849,59850,59851,59852,59853,59854,59855,59856,59857,59858,59859,59860,59861,59862,59863,59864,59865,59866,59867,59868,59869,59870,59871,59872,59873,59874,59875,59876,59877,59878,59879,59880,59881,59882,59883,59884,59885,59886,59887,59888,59889,59890,59891,59892,59893,59894,59895,59896,59897,59898,59899,59900,59901,59902,59903,59904,59905,59906,59907,59908,59909,59910,59911,59912,59913,59914,59915,59916,59917,59918,59919,59920,59921,59922,59923,59924,59925,59926,59927,59928,59929,59930,59931,59932,59933,59934,59935,59936,59937,59938,59939,59940,59941,59942,59943,59944,59945,59946,59947,59948,59949,59950,59951,59952,59953,59954,59955,59956,59957,59958,59959,59960,59961,59962,59963,59964,59965,59966,59967,59968,59969,59970,59971,59972,59973,59974,59975,59976,59977,59978,59979,59980,59981,59982,59983,59984,59985,59986,59987,59988,59989,59990,59991,59992,59993,59994,59995,59996,59997,59998,59999,60000,60001,60002,60003,60004,60005,60006,60007,60008,60009,60010,60011,60012,60013,60014,60015,60016,60017,60018,60019,60020,60021,60022,60023,60024,60025,60026,60027,60028,60029,60030,60031,60032,60033,60034,60035,60036,60037,60038,60039,60040,60041,60042,60043,60044,60045,60046,60047,60048,60049,60050,60051,60052,60053,60054,60055,60056,60057,60058,60059,60060,60061,60062,60063,60064,60065,60066,60067,60068,60069,60070,60071,60072,60073,60074,60075,60076,60077,60078,60079,60080,60081,60082,60083,60084,60085,60086,60087,60088,60089,60090,60091,60092,60093,60094,60095,60096,60097,60098,60099,60100,60101,60102,60103,60104,60105,60106,60107,60108,60109,60110,60111,60112,60113,60114,60115,60116,60117,60118,60119,60120,60121,60122,60123,60124,60125,60126,60127,60128,60129,60130,60131,60132,60133,60134,60135,60136,82357,82358,82359,82360,82361,82362,82363,82364,82365,82366,82367,82368,82369,82370,82371,82372,82373,82374,82375,82376,82377,82378,82379,82380,82381,82382,82400,82401,82412,82413,82461,82462,82463,82464,82479,82480,82481,82482,82483,82484,82485,82486,82487,82488,82489,82490,82491,82492,82493,82494,82495,82520,82521,82522,82523,82524,82525,82526,82527,82533,82534,82535,82536,82537,82538,82541,82542,82543,82544,82549,82550,82551,82552,82555,82556,82557,82558,82559,82560,82586,82588,82589,82592,82594,82595,82596,82597,82606,82607,82608,82609,82610,82611,82612,82613,82614,82615,82616,82617,82618,82619,82620,82621,82622,82623,82624,82625,82626,82627,82628,82629,82630,82631,82632,82633,82634,82635,82636,82637,82638,82639,82640,82641,82642,82643,82644,82645,82646,82647,82648,82649,82650,82651,82652,82653,82654,82655,82656,82657,82658,82659,82660,82661,82662,82663,82664,82665,82666,82667,82668,82669,82670,82671,82672,82673,82674,82675,82676,82677,82678,82679,82680,82681,82682,82683,82684,82685,82686,82687,82688,82689,82690,82691,82692,82693,82694,82695,82696,82697,82698,82699,82700,82701,82702,82703,82704,82705,82706,82707,82708,82709,82710,82711,82712,82713,82714,82715,82716,82717,82718,82719,82720,82721,82722,82723,82724,82725,82726,82727,82728,82729,82730,82731,82732,82733,82734,82735,82736,82737,82738,82739,82740,82741,82742,82743,82744,82745,82746,82747,82748,82749,82750,82751,82752,82753,82754,82755,82756,82757,82758,82759,82760,82761,82762,82763,82764,82765,82766,82767,82768,82769,82770,82771,82772,82773,82774,82775,82776,82777,82778,82779,82780,82781,82782,82783,82784,82785,82786,82787,82788,82789,82790,82791,82792,82793,82794,82795,82796,82797,82798,82799,82800,82801,82802,82803,82804,82805,82806,82807,82808,82809,82810,82811,82812,82813,82814,82815,82816,82817,82818,82819,82820,82821,82822,82823,82824,82825,82826,82827,82828,82829,82830,82831,82832,82833,82834,82835,82836,82837,82838,82839,82840,82841,82842,82843,82844,82845,82846,82847,82848,82849,82850,82851,82852,82853,82854,82855,82856,82858,82859,82860,82861,82866,82867,82868,82869,82872,82873,82874,82875,82876,82877,82878,82879,82880,82881,82882,82883,82884,82885,82886,82887,82888,82889,82890,82891,82892,82893,82894,82895,82896,82897,82898,82899,82900,82901,82902,82903,82904,82905,82906,82907,82908,82909,82910,82911,82912,82913,82914,82915,82916,82917,82918,82919,82920,82921,82922,82923,82924,82925,82926,82927,82928,82929,82930,82931,82932,82933,82934,82935,82936,82937,82938,82939,82940,82941,82942,82943,82944,82945,82946,82947,82948,82949,82950,82951,82952,82953,82954,82955,82956,82957,82958,82959,82960,82961,82962,82963,82964,82965,82966,82967,82968,82969,82970,82971,82972,82973,82974,82975,82976,82977,82978,82979,82980,82981,82982,82983,82984,82985,82986,82987,82988,82989,82990,82991,82992,82993,82994,82995,82996,82997,82998,82999,83000,83001,83002,83003,83004,83005,83006,83007,83008,83009,83010,83011,83012,83013,83014,83015,83016,83017,83018,83019,83020,83021,83022,83023,83024,83025,83026,83027,83028,83029,83030,83031,83032,83033,83034,83035,83036,83037,83038,83039,83040,83041,83042,83043,83044,83045,83046,83047,83048,83049,83050,83051,83052,83053,83054,83055,83056,83057,83058,83059,83060,83061,83062,83063,83064,83065,83066,83067,83068,83069,83070,83071,83072,83073,83074,83075,83076,83077,83078,83079,83080,83081,83082,83083,83084,83085,83086,83087,83088,83089,83090,83091,83092,83093,83094,83095,83096,83097,83098,83099,83100,83101,83102,83103,83104,83105,83106,83107,83108,83109,83110,83111,83112,83113,83114,83115,83116,83117,83118,83119,83120,83121,83122,83123,83124,83125,83126,83127,83128,83129,83130,83131,83132,83133,83134,83135,83136,83137,83138,83139,83140,83141,83142,83143,83144,83145,83146,83147,83148,83149,83150,83151,83152,83153,83154,83155,83156,83157,83158,83159,83160,83161,83162,83163,83164,83165,83169,83170,83171,83172,83173,83174,83175,83176,83177,83178,83179,83180,83181,83182,83183,83184,83189,83190,83191,83192,83193,83194,83195,83196,83197,83198,83199,83200,83201,83202,83203,83204,83205,83206,83207,83208,83209,83210,83211,83214,83215,83216,83217,83218,83219,83220,83221,83222,83223,83224,83225,83226,83227,83228,83229,83230,83231,83232,83239,83240,83247,83248,83249,83250,83251,83252,83253,83254,83255,83256,83257,83258,83259,83260,83261,83262,83263,83264,83265,83266,83267,83268,83269,83270,83271,83272,83273,83274,83275,83276,83277,83278,83279,83280,83281,83282,83283,83284,83285,83286,83287,83288,83289,83290,83291,83292,83293,83294,83295,83296,83297,83298,83299,83300,83301,83302,83303,83304,83305,83306,83307,83308,83309,83310,83311,83312,83313,83314,83315,83316,83317,83318,83319,83320,83321,83322,83323,83324,83325,83326,83327,83328,83329,83330,83331,83332,83333,83334,83335,83336,83337,83338,83339,83340,83341,83342,83343,83344,83345,83346,83347,83348,83349,83350,83351,83352,83353,83354,83355,83356,83357,83358,83359,83360,83361,83362,83363,83364,83365,83366,83367,83368,83369,83370,83371,83372,83373,83374,83375,83376,83377,83378,83379,83380,83381,83382,83383,83384,83385,83386,83387,83388,83389,83390,83391,83392,83393,83394,83395,83396,83397,83398,83399,83400,83401,83402,83403,83404,83405,83406,83407,83408,83409,83410,83411,83412,83413,83414,83415,83416,83417,83418,83419,83420,83421,83422,83423,83424,83425,83426,83427,83428,83429,83430,83431,83432,83433,83434,83435,83436,83437,83438,83439,83440,83441,83442,83443,83444,83445,83446,83447,83448,83449,83450,83451,83452,83453,83454,83455,83456,83457,83458,83459,83460,83461,83462,83463,83464,83465,83466,83467,83468,83469,83470,83471,83472,83473,83474,83475,83476,83477,83478,83479,83480,83481,83482,83483,83484,83485,83486,83487,83488,83489,83490,83491,83492,83493,83494,83495,83496,83497,83498,83499,83500,83501,83502,83503,83504,83505,83506,83507,83508,83599,83600,83601,83602,83603,83604,83605,83606,83607,83608,83609,83610,83611,83612,83644,83645,83646,83647,83648,83649,83650,83651,83652,83653,83654,83655,83656,83657,83658,83659,83660,83661,83662,83663,83664,83665,83666,83667,83668,83669,83670,83671,};
	cout << "rightForearm: " << (sizeof(faceID) / sizeof(DWORD)) << endl;
	for (int i = 0; i < (int) sizeof(faceID) / sizeof(DWORD); ++i) {
		face_id_set.insert(faceID[i]);
	}
	__defined_area_dic[AREA_RIGHT_FORE_ARM] = defArea;
} while (0);
do {	// 왼쪽 허벅지
	kArea::DEFINED_AREA_DIC::iterator	it;
	it = __defined_area_dic.find(AREA_LEFT_THIGH);
	if (it != __defined_area_dic.end())
		__defined_area_dic.erase(it);

	kArea::DEFINED_AREA	defArea;
	defArea.value = .9f;	// 원하는 색깔이...
	std::set<DWORD>&	face_id_set = defArea.face_id_set;
	DWORD	faceID[] = {62360,62361,62386,62387,62388,62434,62445,62446,62447,62448,62449,62450,62451,62452,62544,62545,62546,62547,62548,62549,62550,62551,62554,62555,62556,62557,62558,62559,62560,62561,62562,62563,62564,62565,62566,62567,62568,62569,62571,62572,62573,62574,62575,62576,62577,62578,62579,62580,62581,62582,62587,62588,62589,62590,62599,62600,62601,62602,62603,62604,62605,62606,62607,62608,62609,62612,62613,62623,62625,62626,62627,62628,62629,62630,62631,62632,62633,62634,62635,62636,62637,62638,62639,62640,62641,62642,62643,62644,63084,63085,63110,63111,63112,63157,63158,63166,63167,63168,63169,63170,63171,63172,63173,63174,63175,63176,63268,63269,63270,63271,63272,63273,63274,63275,63278,63279,63280,63281,63282,63283,63284,63285,63286,63287,63288,63289,63290,63291,63292,63293,63294,63295,63296,63297,63298,63299,63300,63301,63302,63303,63304,63305,63306,63307,63308,63311,63312,63313,63314,63323,63324,63325,63326,63327,63328,63329,63330,63331,63332,63333,63336,63337,63338,63339,63340,63341,63342,63343,63346,63347,63348,63349,63350,63351,63352,63353,63354,63355,63356,63357,63358,63359,63360,63361,63362,63363,63364,63365,63366,63367,63368,63369,63370,63371,63372,63612,63613,63614,63615,63616,63617,63618,63619,63620,63621,63622,63623,63624,63625,63626,63627,63628,63629,63630,63631,63632,63633,63634,63635,63636,63637,63638,63639,63640,63641,63642,63643,63644,63645,63646,63647,63648,63649,63650,63651,63652,63653,63654,63655,63656,63657,63658,63659,63660,63661,63662,63663,63664,63665,63666,63667,63668,63669,63670,63671,63672,63673,63674,63675,63676,63677,63678,63679,63680,63681,63682,63683,63686,63687,63729,63734,63735,63738,63739,63749,63750,63751,63756,63757,63758,63759,63760,63761,63762,63763,63764,63765,63766,63767,63768,63769,63770,63771,63772,63773,63774,63775,63776,63777,63778,63779,63784,63785,63786,63787,63790,63792,63793,63794,63795,63796,63797,63798,63799,63800,63801,63802,63803,63804,63805,63806,63807,63808,63809,63810,63811,63812,63813,63814,63815,63816,63817,63818,63819,63820,63821,63822,63823,63824,63825,63826,63827,63828,63829,63830,63831,63832,63833,63834,63835,63836,63837,63838,63839,63840,63841,63842,63843,63844,63845,63846,63847,63848,63849,63850,63851,63852,63853,63854,63855,63856,63857,63858,63859,63860,63861,63862,63863,63864,63865,63866,63867,63868,63869,63870,63873,63874,63875,63876,63877,63878,63879,63880,63881,63882,63883,63884,63885,63886,63890,63891,63892,63893,63894,63895,63898,63899,63900,63901,63904,63905,63917,63918,63919,63920,63921,63922,63923,63924,63925,63958,63959,63961,63962,63965,63966,63967,63968,63975,63976,63977,63978,63979,63980,63981,63982,63983,63984,63985,63986,63987,63988,63989,63990,63997,63998,63999,64000,64001,64002,64003,64004,64005,64006,64007,64008,64009,64010,64011,64012,64036,64037,64038,64039,64040,64041,64042,64043,64044,64045,64046,64065,64066,64070,64071,64072,64073,64074,64075,64102,64103,64104,64105,64106,64107,64108,64109,64116,64117,64118,64119,64120,64121,64130,64131,64132,64133,64135,64140,64141,64144,64145,64161,64162,64171,64181,64182,64183,64184,64185,64186,64187,64188,64193,64194,64195,64196,64197,64198,64199,64200,64201,64202,64203,64204,64205,64206,64207,64208,64209,64210,64211,64212,64213,64214,64215,64216,64217,64218,64367,64368,64369,64370,64371,64372,64373,64374,64375,64376,64377,64378,64379,64380,64381,64382,64383,64384,64385,64386,64387,64388,64389,64390,64391,64392,64393,64394,64395,64396,64399,64401,64402,64403,64404,64406,64407,64408,64409,64410,64411,64417,64426,64427,64428,64429,64430,64431,64434,64435,64436,64437,64438,64439,64440,64441,64442,64443,64444,64445,64447,64448,64449,64450,64451,64452,64453,64454,64455,64456,64457,64458,64459,64460,64461,64462,64463,64464,64465,64466,64467,64468,64469,64470,64471,64472,64473,64474,64528,64529,64530,64531,64548,64549,64550,64551,64552,64553,64554,64555,64556,64557,64558,64559,64560,64561,64562,64563,64564,64565,64566,64567,64568,64569,64575,64576,64577,64578,64579,64580,64581,64582,64583,64584,64585,64586,64587,64588,64589,64590,64591,64592,64593,64594,64595,64596,64597,64598,64599,64600,64601,64602,64603,64604,64605,64606,64607,64608,64609,64610,64611,64612,64613,64614,64615,64616,64617,64618,64619,64620,64621,64622,64623,64624,64625,64626,64627,64628,64629,64630,64631,64660,64661,64662,64663,64664,64665,64666,64667,64668,64669,64670,64671,64672,64673,64674,64675,64676,64678,64679,64680,64681,64682,64683,64684,64685,64686,64687,64688,64689,64690,64699,64700,64701,64702,64703,64704,64705,64706,64707,64708,64709,64710,64711,64712,64713,64714,64715,64716,64717,64718,64721,64723,64724,64725,64726,64729,64743,64744,64745,64746,64747,64748,64749,64750,64751,64752,64753,64754,64755,64756,64757,64758,64759,64760,64761,64762,64763,64764,64765,64766,64767,64768,64769,64770,64771,64772,64773,64774,64775,64776,64777,64778,64779,64780,64781,64782,64783,64784,64785,64786,64787,64788,64789,64790,64791,64792,64793,64794,64795,64796,64797,64798,64799,64800,64801,64802,64803,64804,64805,64806,64807,64808,64809,64810,64812,64813,64814,64815,64816,64817,64818,64819,64820,64821,64822,64823,64824,64825,64826,64827,64828,64829,64830,64831,64832,64833,64834,64835,64836,64837,64838,64839,64840,64841,64842,64843,64844,64845,64846,64847,64848,64849,64850,64851,64852,64853,64854,64856,64857,64858,64863,64864,64871,64872,64873,64874,64875,64876,64881,64882,64883,64884,64885,64886,64887,64888,64889,64890,64891,64892,64893,64908,64909,64910,64911,64916,64917,64918,64919,64920,64921,64922,64923,64924,64925,64926,64945,64946,64953,64954,64957,64958,64959,64960,64961,64962,65150,65151,65183,65184,65185,65186,65187,65188,65193,65194,65195,65196,65197,65198,65199,65200,65201,65202,65203,65226,65227,65228,65229,65230,65231,65234,65235,65236,65237,65238,65245,65246,65247,65248,65249,65250,65269,65270,65275,65276,65279,65280,65283,65284,65285,65286,65287,65288,65289,65290,65291,65292,65299,65300,65304,65324,65382,65385,65386,65730,65731,65732,65733,65736,65769,65770,65772,65773,65786,65787,65818,65820,65821,65822,65823,65854,65855,65856,65876,65877,65881,65882,65883,65884,65885,65886,66373,66374,69064,69065,69066,69067,69068,69069,69070,69071,69072,69073,69074,69075,69076,69077,69078,69079,69080,69081,69082,69083,69084,69085,69086,69087,69088,69089,69090,69091,69092,69093,69094,69095,69096,69097,69098,69099,69100,69101,69102,69103,69104,69105,69106,69107,69108,69109,69110,69111,69112,69113,69114,69115,69116,69117,69118,69119,69120,69121,69122,69123,69124,69125,69126,69127,69128,69129,69130,69131,69132,69133,69134,69135,69136,69137,69138,69139,69140,69141,69142,69143,69144,69145,69146,69147,69148,69149,69150,69151,69152,69153,69154,69155,69156,69157,69158,69159,69160,69161,69162,69163,69164,69165,69166,69417,69418,69424,69425,69426,69427,69428,69429,69430,69431,69432,69433,69434,69435,69436,69437,69438,69439,69440,69441,69442,69443,69444,69445,69446,69447,69448,69449,69450,69451,69464,69465,69466,69467,69468,69469,69470,69471,69472,69473,69474,69475,69476,69477,69478,69491,69492,69493,69494,69495,69496,69497,69498,69499,69500,69501,69502,69503,69504,69505,69506,69507,69508,69515,69516,69524,69525,69526,69527,69528,69529,69530,69531,69544,69545,69546,69547,69556,69557,69558,69559,69576,69577,69578,69579,69580,69581,69582,69583,69584,69585,69586,69589,69590,69591,69592,69609,69610,69611,69612,69613,69614,69615,69616,69617,69618,69619,69620,69622,69629,69630,69631,69632,69653,69654,69655,69656,69657,69658,69659,69660,69661,69662,69663,69664,69665,69666,69667,69668,69669,69670,69671,69672,69673,69674,69675,69676,69677,69678,69679,69680,69681,69682,69683,69684,69685,69686,69687,69688,69689,69690,69691,69696,69697,69698,69699,69700,69701,69702,69705,69707,69709,69710,69711,69712,69713,69714,69716,69737,69738,69739,69740,69741,69742,69744,69770,69771,69772,69773,69774,69775,69776,69777,69778,69779,69784,69785,69786,69787,69788,69789,69790,69791,69792,69793,69794,69795,69796,69797,69798,69799,69800,69801,69802,69803,69804,69805,69806,69807,69808,69809,69810,69811,69812,69813,69814,69815,69816,69817,69818,69819,69820,69821,69822,69823,69824,69825,69826,69827,69828,69829,69830,69831,69832,69833,69834,69835,69836,69837,69838,69839,69842,69846,69847,69848,69849,69859,69860,69861,69862,69873,69874,69875,69876,69877,69878,69879,69880,69881,69882,69883,69884,69885,69886,69887,69888,69889,69890,69891,69892,69893,69894,69895,69896,69897,69898,69899,69902,69903,69904,69905,69916,69917,69918,69919,69920,69921,69922,69923,69924,69925,69926,69927,69928,69929,69930,69931,69932,69933,69934,69935,69936,69937,69938,69939,69940,69941,69942,69943,69944,69945,69946,69947,69948,69949,69950,69951,69952,69953,69954,69955,69956,69957,69958,69959,69960,69961,69962,69963,69964,69965,69966,69967,69968,69969,69970,69971,69972,69973,69974,69975,69976,69977,69978,69979,69980,69981,69982,69983,69984,69985,69986,69987,69988,69989,69991,70028,70029,70030,70031,70032,70033,70034,70035,70036,70037,70038,70039,70040,70041,70042,70043,70044,70045,70046,70047,70048,70049,70050,70051,70052,70053,70054,70055,70056,70057,70058,70059,70060,70061,70062,70063,70064,70065,70066,70067,70068,70069,70070,70071,70072,70073,70074,70075,70076,70077,70078,70079,70080,70081,70082,70083,70084,70085,70086,70087,70088,70089,70090,70091,70092,70093,70094,70095,70096,70097,70098,70099,70100,70101,70102,70103,70104,70105,70106,70107,70108,70109,70110,70111,70112,70113,70114,70115,70116,70117,70118,70119,70120,70121,70122,70123,70124,70125,70126,70127,70128,70129,70130,70131,70132,70133,70134,70135,70136,70137,70138,70139,70140,70141,70142,70143,70144,70145,70146,70147,70148,70149,70150,70151,70152,70153,70154,70155,70156,70157,70158,70159,70160,70161,70162,70163,70164,70165,70166,70167,70168,70169,70170,70171,70172,70173,70174,70175,70176,70177,70178,70179,70180,70181,70182,70183,70184,70185,70186,70187,70188,70189,70190,70191,70192,70193,70194,70195,70196,70197,70198,70199,70200,70201,70202,70203,70204,70205,70206,70207,70208,70209,70210,70211,70212,70213,70214,70215,70216,70217,70218,70219,70220,70233,70234,70235,70236,70237,70238,70240,70241,70242,70243,70297,70298,70299,70300,70301,70302,70303,70304,70305,70306,70307,70308,70309,70310,70311,70312,70313,70314,70315,70316,70317,70318,70319,70320,70321,70322,70323,70324,70325,70326,70327,70328,70329,70330,70331,70332,70333,70334,70335,70336,70337,70338,70339,70340,70341,70342,70343,70344,70345,70346,70347,70348,70349,70350,70351,70352,70353,70354,70355,70356,70357,70358,70359,70360,70361,70362,70363,70364,70365,};
	cout << "leftThigh: " << (sizeof(faceID) / sizeof(DWORD)) << endl;
	for (int i = 0; i < (int) sizeof(faceID) / sizeof(DWORD); ++i) {
		face_id_set.insert(faceID[i]);
	}
	__defined_area_dic[AREA_LEFT_THIGH] = defArea;
} while (0);
do {	// 오른쪽 허벅지
	kArea::DEFINED_AREA_DIC::iterator	it;
	it = __defined_area_dic.find(AREA_RIGHT_THIGH);
	if (it != __defined_area_dic.end())
		__defined_area_dic.erase(it);

	kArea::DEFINED_AREA	defArea;
	defArea.value = .9f;	// 원하는 색깔이...
	std::set<DWORD>&	face_id_set = defArea.face_id_set;
	DWORD	faceID[] = {65423,65424,65425,65426,65427,65428,65429,65430,65431,65432,65433,65434,65435,65436,65437,65438,65439,65440,65441,65442,65443,65444,65445,65446,65447,65448,65449,65450,65451,65452,65453,65454,65455,65456,65457,65458,65459,65460,65461,65462,65463,65464,65465,65466,65467,65468,65469,65470,65471,65472,65473,65474,65475,65476,65477,65478,65479,65480,65481,65482,65483,65484,65485,65486,65487,65488,65489,65490,65491,65492,65493,65494,65549,65550,65560,65562,65567,65568,65569,65570,65571,65572,65573,65574,65575,65576,65577,65578,65579,65580,65581,65582,65583,65584,65585,65586,65587,65588,65589,65590,65595,65596,65597,65598,65603,65604,65605,65606,65607,65608,65609,65610,65611,65612,65613,65614,65615,65616,65617,65618,65619,65620,65621,65622,65623,65624,65625,65626,65627,65628,65629,65630,65631,65632,65633,65634,65635,65636,65637,65638,65639,65640,65641,65642,65643,65644,65645,65646,65647,65648,65649,65650,65651,65652,65653,65654,65655,65656,65657,65658,65659,65660,65661,65662,65663,65664,65665,65666,65667,65668,65669,65670,65671,65672,65673,65674,65675,65676,65677,65678,65679,65680,65681,65684,65685,65686,65687,65688,65689,65690,65691,65692,65693,65694,65695,65696,65697,65701,65702,65703,65704,65705,65706,65709,65710,65711,65712,65715,65716,65788,65790,65791,65792,65793,65847,65848,65849,65850,65851,65852,65853,65854,65855,65857,65881,65882,65913,65914,65915,65916,65917,65918,65927,65928,65929,65930,65941,65942,65944,65951,65952,65972,65973,65992,65993,65994,65995,65996,65997,65999,66004,66005,66006,66007,66008,66009,66010,66011,66012,66013,66014,66015,66016,66017,66018,66019,66020,66021,66022,66023,66024,66025,66026,66027,66028,66029,66178,66179,66180,66181,66182,66183,66184,66185,66188,66189,66190,66191,66192,66193,66194,66195,66196,66197,66198,66199,66200,66203,66204,66205,66206,66207,66212,66214,66217,66219,66220,66221,66222,66237,66238,66239,66240,66245,66246,66247,66248,66249,66250,66251,66252,66259,66260,66261,66262,66263,66264,66265,66266,66267,66268,66269,66270,66271,66272,66273,66274,66275,66276,66277,66278,66279,66280,66281,66282,66283,66284,66285,66341,66342,66359,66360,66361,66362,66363,66364,66365,66366,66367,66368,66369,66370,66371,66372,66375,66376,66377,66379,66380,66386,66387,66388,66389,66390,66391,66392,66393,66394,66395,66396,66397,66398,66399,66400,66401,66402,66403,66404,66405,66406,66407,66408,66409,66410,66411,66412,66413,66414,66415,66416,66417,66418,66419,66420,66421,66422,66423,66424,66425,66426,66427,66428,66429,66430,66431,66432,66433,66434,66435,66436,66437,66438,66439,66440,66441,66442,66471,66472,66473,66474,66475,66476,66477,66478,66479,66480,66481,66482,66483,66484,66485,66486,66487,66489,66490,66491,66492,66493,66494,66495,66496,66497,66498,66499,66500,66501,66510,66511,66512,66513,66514,66515,66516,66517,66518,66519,66520,66521,66522,66524,66525,66526,66527,66528,66529,66534,66535,66536,66537,66540,66554,66555,66556,66557,66558,66559,66560,66561,66562,66563,66564,66565,66566,66567,66568,66569,66570,66571,66572,66573,66574,66575,66576,66577,66578,66579,66580,66581,66582,66583,66584,66585,66586,66587,66588,66589,66590,66591,66592,66593,66594,66595,66596,66597,66598,66599,66600,66601,66602,66603,66604,66605,66606,66607,66608,66609,66610,66611,66612,66613,66614,66615,66616,66617,66618,66619,66620,66621,66623,66624,66625,66626,66627,66628,66629,66630,66631,66632,66633,66634,66635,66636,66637,66638,66639,66640,66641,66642,66643,66644,66645,66646,66647,66648,66649,66650,66651,66652,66653,66654,66655,66656,66657,66658,66659,66660,66664,66665,66669,66682,66683,66684,66685,66692,66693,66694,66695,66696,66697,66698,66699,66700,66701,66702,66703,66704,66719,66720,66727,66728,66729,66730,66731,66732,66733,66734,66735,66736,66737,66994,66995,66996,66997,67004,67005,67006,67007,67008,67009,67012,67013,67014,67037,67038,67045,67046,67047,67048,67049,67056,67057,67090,67091,67094,67095,67096,67097,67098,67099,67100,67101,67111,67498,67499,67500,67501,67502,67503,67504,67505,67506,67507,67508,67509,67510,67511,67512,67513,67514,67515,67516,67517,67518,67519,67520,67521,67522,67523,67524,67525,67526,67527,67528,67529,67530,67531,67532,67533,67534,67535,67536,67537,67538,67539,67540,67541,67542,67543,67544,67545,67546,67547,67548,67549,67550,67551,67552,67553,67554,67555,67556,67557,67558,67559,67560,67561,67562,67563,67564,67565,67566,67567,67568,67569,67570,67571,67572,67573,67574,67575,67576,67577,67578,67579,67580,67581,67582,67583,67584,67585,67586,67587,67588,67589,67590,67591,67592,67593,67594,67595,67596,67597,67598,67599,67600,67757,67758,67851,67852,67853,67854,67858,67859,67860,67861,67862,67863,67864,67865,67866,67867,67868,67869,67870,67871,67872,67873,67874,67875,67876,67877,67878,67879,67880,67881,67882,67883,67884,67885,67886,67887,67888,67889,67894,67895,67896,67897,67898,67899,67900,67901,67902,67903,67904,67905,67906,67907,67908,67909,67910,67911,67912,67913,67914,67915,67916,67917,67918,67919,67920,67921,67922,67923,67924,67925,67926,67927,67928,67929,67930,67931,67932,67933,67934,67935,67936,67937,67938,67939,67940,67941,67942,67943,67944,67945,67946,67949,67950,67954,67955,67956,67957,67958,67959,67960,67961,67962,67963,67964,67965,67966,67967,67969,67976,67977,67978,67979,67980,67981,67982,67983,67990,67991,67992,67993,67994,67995,68008,68009,68010,68011,68012,68013,68014,68015,68016,68017,68018,68019,68020,68021,68022,68023,68024,68025,68026,68027,68028,68029,68030,68032,68037,68038,68039,68040,68041,68042,68043,68044,68045,68046,68047,68048,68049,68050,68051,68052,68053,68054,68055,68056,68057,68058,68059,68060,68063,68064,68065,68066,68067,68068,68069,68070,68071,68072,68075,68076,68077,68078,68079,68084,68085,68086,68087,68088,68089,68090,68091,68092,68093,68094,68095,68096,68097,68098,68099,68100,68101,68102,68103,68104,68105,68106,68107,68108,68109,68110,68111,68112,68113,68114,68115,68116,68117,68118,68119,68120,68121,68122,68123,68124,68125,68126,68127,68128,68129,68130,68131,68132,68133,68134,68135,68136,68137,68138,68139,68140,68141,68142,68143,68144,68145,68146,68147,68148,68149,68150,68151,68152,68159,68160,68161,68162,68171,68172,68173,68174,68175,68176,68177,68178,68179,68180,68181,68182,68183,68184,68185,68186,68187,68188,68189,68190,68192,68193,68198,68204,68205,68206,68207,68208,68209,68210,68211,68212,68213,68214,68215,68216,68218,68219,68220,68221,68222,68223,68224,68225,68226,68227,68228,68229,68230,68231,68232,68233,68234,68235,68236,68237,68238,68239,68240,68241,68242,68243,68244,68245,68246,68247,68248,68249,68250,68251,68252,68253,68254,68255,68256,68257,68258,68259,68260,68261,68262,68263,68264,68265,68266,68267,68268,68269,68270,68271,68272,68273,68274,68275,68276,68277,68278,68279,68280,68281,68282,68283,68291,68292,68293,68294,68295,68296,68297,68298,68306,68307,68308,68309,68310,68311,68312,68313,68314,68315,68316,68317,68318,68319,68320,68321,68322,68323,68324,68325,68326,68327,68328,68329,68330,68331,68332,68333,68334,68335,68336,68337,68338,68339,68344,68345,68346,68347,68348,68349,68350,68351,68352,68353,68354,68355,68356,68357,68358,68359,68360,68361,68362,68363,68364,68365,68366,68367,68368,68369,68370,68371,68372,68373,68374,68375,68376,68377,68378,68379,68380,68381,68382,68383,68384,68385,68386,68387,68388,68389,68390,68391,68392,68393,68394,68395,68396,68397,68398,68399,68400,68401,68402,68403,68404,68405,68406,68407,68408,68409,68410,68411,68412,68413,68414,68415,68416,68417,68418,68419,68420,68421,68422,68423,68424,68425,68426,68427,68428,68429,68430,68431,68432,68433,68434,68435,68436,68437,68438,68439,68444,68445,68446,68447,68448,68449,68450,68451,68452,68453,68458,68459,68460,68461,68462,68463,68464,68465,68466,68467,68468,68469,68470,68471,68472,68473,68474,68475,68476,68477,68478,68479,68480,68481,68482,68483,68484,68485,68486,68487,68488,68489,68490,68491,68492,68493,68494,68495,68496,68497,68498,68499,68500,68501,68502,68503,68504,68505,68506,68507,68508,68509,68510,68511,68512,68513,68514,68515,68516,68517,68518,68519,68520,68521,68522,68523,68524,68525,68526,68527,68528,68529,68530,68531,68532,68533,68534,68535,68536,68537,68538,68539,68540,68541,68542,68543,68544,68545,68546,68547,68548,68549,68550,68551,68552,68553,68554,68555,68556,68557,68558,68559,68560,68561,68562,68563,68564,68565,68566,68567,68568,68569,68570,68571,68572,68573,68574,68575,68576,68577,68578,68579,68580,68581,68582,68583,68584,68585,68586,68587,68588,68589,68590,68591,68592,68593,68594,68595,68596,68597,68598,68599,68600,68601,68602,68603,68604,68605,68606,68607,68608,68609,68610,68611,68612,68613,68614,68615,68616,68617,68618,68619,68620,68621,68622,68623,68624,68625,68626,68627,68628,68629,68630,68631,68632,68633,68634,68635,68636,68637,68638,68639,68640,68641,68642,68643,68644,68645,68646,68647,68648,68649,68650,68651,68652,68653,68654,68655,68656,68667,68668,68669,68670,68671,68672,68674,68675,68676,68677,68710,68711,68712,68713,68731,68732,68733,68734,68735,68736,68737,68738,68739,68740,68741,68742,68743,68744,68745,68746,68747,68748,68749,68750,68751,68752,68753,68754,68755,68756,68757,68758,68759,68760,68761,68762,68763,68764,68765,68766,68767,68768,68769,68770,68771,68772,68773,68774,68775,68776,68777,68778,68779,68780,68781,68782,68783,68784,68785,68786,68787,68788,68789,68790,68791,68792,68793,68794,68795,68796,68797,68798,68799,};
	cout << "rightThigh: " << (sizeof(faceID) / sizeof(DWORD)) << endl;
	for (int i = 0; i < (int) sizeof(faceID) / sizeof(DWORD); ++i) {
		face_id_set.insert(faceID[i]);
	}
	__defined_area_dic[AREA_RIGHT_THIGH] = defArea;
} while (0);
do {	// 왼쪽 장단지
	kArea::DEFINED_AREA_DIC::iterator	it;
	it = __defined_area_dic.find(AREA_LEFT_CALF);
	if (it != __defined_area_dic.end())
		__defined_area_dic.erase(it);

	kArea::DEFINED_AREA	defArea;
	defArea.value = .9f;	// 원하는 색깔이...
	std::set<DWORD>&	face_id_set = defArea.face_id_set;
	DWORD	faceID[] = {68833,68944,68945,68946,68947,68948,68949,68950,68951,68952,68953,68954,68955,68962,68963,69002,69003,69018,69019,69042,69044,69045,69046,69047,69048,69049,69050,69051,69052,69053,69054,69055,69056,69057,69058,69059,69060,69061,69062,69063,70006,70007,70008,70009,70229,70230,70231,70232,70239,70295,70296,70371,70372,70373,70374,70375,70376,70382,70383,70384,70385,70504,70511,70512,70514,70517,70518,70657,70658,70661,70662,70663,70664,70665,70666,70667,70668,70669,70670,70671,70672,70673,70674,70675,70676,70677,70678,70679,70680,70681,70682,70683,70684,70685,70686,70687,70688,70689,70690,70691,70692,70693,70694,70695,70696,70697,70698,70699,70700,70701,70702,70703,70704,70705,70706,70707,70708,70709,70710,70711,70712,70713,70714,70741,70742,70743,70744,70745,70746,70747,70748,70749,70750,70751,70752,70753,70756,70757,70758,70759,70761,70804,70805,70806,70807,70808,70809,70810,70811,70812,70813,70814,70815,70816,70817,70818,70819,70820,70821,70822,70823,70824,70825,70826,70827,70828,70829,70830,70831,70832,70833,70834,70835,70836,70837,70838,70839,70840,70841,70842,70843,70844,70845,70846,70847,70848,70849,70850,70851,70852,70853,70854,70855,70856,70857,70858,70859,70860,70861,70862,70863,70864,70865,70866,70867,70868,70869,70870,70871,70872,70873,70874,70875,70876,70877,70878,70879,70880,70881,70882,70883,70884,70885,70886,70887,70888,70889,70890,70891,70892,70893,70894,70895,70896,70897,70898,70899,70900,70901,70902,70903,70928,70929,70938,70939,70940,70948,70949,70965,70966,70967,70968,70969,70970,70971,70972,70973,70974,70975,70976,70977,70978,70979,70980,70981,70982,70983,70984,70985,70986,70987,70988,70989,70990,70991,70992,70993,70994,70995,70996,70997,70998,70999,71000,71001,71002,71003,71004,71005,71006,71007,71008,71009,71010,71011,71012,71013,71014,71015,71016,71017,71018,71019,71020,71021,71022,71023,71024,71025,71026,71027,71028,71029,71030,71031,71032,71033,71034,71035,71036,71037,71038,71039,71040,71041,71042,71043,71044,71045,71046,71047,71048,71049,71050,71051,71052,71053,71054,71055,71056,71057,71058,71059,71060,71061,71062,71063,71064,71065,71066,71067,71068,71069,71070,71071,71072,71073,71074,71075,71076,71077,71078,71079,71080,71081,71082,71083,71084,71085,71086,71087,71088,71089,71090,71091,71092,71093,71094,71095,71096,71097,71098,71099,71100,71101,71102,71103,71104,71105,71106,71107,71108,71109,71110,71111,71112,71113,71114,71115,71116,71117,71118,71119,71120,71121,71122,71123,71124,71125,71126,71127,71128,71129,71130,71131,71132,71133,71134,71135,71136,71137,71138,71139,71140,71141,71142,71143,71144,71145,71146,71147,71148,71149,71150,71151,71152,71153,71154,71155,71156,71157,71158,71159,71160,71161,71162,71163,71164,71165,71166,71167,71168,71169,71170,71171,71172,71173,71174,71175,71176,71177,71178,71179,71180,71181,71182,71183,71184,71185,71186,71187,71188,71189,71190,71191,71192,71193,71194,71195,71196,71197,71198,71199,71200,71201,71202,71203,71204,71205,71206,71207,71208,71209,71210,71211,71212,71213,71214,71215,71216,71217,71218,71219,71220,71221,71222,71223,71224,71225,71226,71227,71228,71229,71230,71231,71232,71233,71234,71235,71236,71237,71238,71239,71240,71241,71242,71243,71244,71245,71246,71247,71248,71355,71356,71357,71425,71426,71427,71428,71429,71430,71431,71432,71433,71434,71435,71436,71437,71438,71439,71440,71441,71442,71443,71444,71445,71446,71447,71448,71449,71450,71451,71452,71453,71454,71455,71456,71457,71458,71459,71460,71463,71464,71467,71468,71471,71472,71475,71476,71477,71479,71480,71481,71482,71483,71484,71485,71486,71487,71488,71489,71490,71491,71492,71493,71494,71495,71496,71497,71498,71499,71500,71501,71502,71503,71504,71505,71506,71507,71508,71509,71510,71511,71512,71513,71514,71515,71516,71517,71518,71519,71520,71521,71522,71523,71524,71525,71526,71527,71528,71529,71530,71531,71532,71533,71534,71535,71536,71537,71538,71539,71540,71541,71542,71543,71544,71545,71546,71547,71548,71549,71550,71551,71552,71553,71554,71555,71556,71557,71558,71559,71560,71561,71562,71563,71564,71565,71566,71567,71568,71569,71570,71571,71572,71573,71574,71575,71576,71577,71578,71579,71580,71581,71582,71583,71584,71585,71586,71587,71588,71589,71590,71591,71592,71593,71594,71595,71596,71597,71598,71599,71600,71601,71602,71603,71604,71605,71606,71607,71608,71609,71610,71611,71612,71613,71614,71615,71616,71617,71618,71619,71620,71621,71622,71623,71624,71625,71626,71627,71628,71629,71630,71631,71632,71633,71634,71635,71636,71637,71638,71639,71640,71641,71642,71643,71644,71645,71646,71647,71648,71649,71650,71651,71652,71653,71654,71655,71656,71657,71658,71659,71660,71661,71662,71663,71664,71665,71666,71667,71668,71669,71670,71671,71672,71673,71674,71675,71676,71677,71678,71679,71680,71685,71686,71687,71688,71689,71690,71691,71692,71693,71694,71695,71696,71697,71698,71699,71700,71701,71702,71703,71704,71705,71706,71707,71708,71709,71710,71711,71712,71713,71714,71715,71716,71717,71718,71719,71720,71721,71722,71723,71724,71725,71726,71727,71728,71729,71730,71731,71732,71733,71734,71735,71736,71737,71738,71739,71740,71741,71742,71743,71744,71745,71746,71747,71748,71749,71750,71751,71752,71753,71754,71755,71756,71757,71768,71769,71770,71771,71772,71773,71774,71775,71776,71777,71778,71779,71780,71781,71782,71783,71784,71785,71811,71812,71813,71814,71815,71816,71817,71818,71819,71822,71823,71824,71825,71826,71827,71832,71833,71834,71835,71836,71837,71838,71839,71840,71841,71842,71843,71844,71845,71846,71847,71848,71849,71850,};
	cout << "leftCalf: " << (sizeof(faceID) / sizeof(DWORD)) << endl;
	for (int i = 0; i < (int) sizeof(faceID) / sizeof(DWORD); ++i) {
		face_id_set.insert(faceID[i]);
	}
	__defined_area_dic[AREA_LEFT_CALF] = defArea;
} while (0);
do {	// 오른쪽 장단지
	kArea::DEFINED_AREA_DIC::iterator	it;
	it = __defined_area_dic.find(AREA_RIGHT_CALF);
	if (it != __defined_area_dic.end())
		__defined_area_dic.erase(it);

	kArea::DEFINED_AREA	defArea;
	defArea.value = .9f;	// 원하는 색깔이...
	std::set<DWORD>&	face_id_set = defArea.face_id_set;
	DWORD	faceID[] = {67378,67379,67380,67381,67382,67383,67396,67397,67481,67482,67483,67485,67496,68673,68729,68730,71858,71859,71860,71861,71862,71863,71869,71870,71871,71872,71991,71998,71999,72001,72004,72005,72148,72149,72150,72151,72152,72153,72154,72155,72156,72157,72158,72159,72161,72162,72163,72164,72165,72166,72167,72168,72169,72170,72171,72172,72173,72174,72175,72176,72177,72178,72179,72180,72181,72182,72183,72184,72185,72186,72187,72188,72189,72190,72191,72192,72193,72194,72195,72196,72197,72198,72199,72200,72201,72228,72229,72230,72231,72232,72233,72234,72235,72236,72237,72238,72239,72291,72292,72293,72294,72295,72296,72297,72298,72299,72300,72301,72302,72303,72304,72305,72306,72307,72308,72309,72310,72311,72312,72313,72314,72315,72316,72317,72318,72319,72320,72321,72322,72323,72324,72325,72326,72327,72328,72329,72330,72331,72332,72333,72334,72335,72336,72337,72338,72339,72340,72341,72342,72343,72344,72345,72346,72347,72348,72349,72350,72351,72352,72353,72354,72355,72356,72357,72358,72359,72360,72361,72362,72363,72364,72365,72366,72367,72368,72369,72370,72371,72372,72373,72374,72375,72376,72377,72378,72379,72380,72381,72382,72383,72384,72385,72386,72387,72452,72453,72454,72455,72456,72457,72458,72459,72460,72461,72462,72463,72464,72465,72466,72467,72468,72469,72470,72471,72472,72473,72474,72475,72476,72477,72478,72479,72480,72481,72482,72483,72484,72485,72486,72487,72488,72489,72490,72491,72492,72493,72494,72495,72496,72497,72498,72499,72500,72501,72502,72503,72504,72505,72506,72507,72508,72509,72510,72511,72512,72513,72514,72515,72516,72517,72518,72519,72520,72521,72522,72523,72524,72525,72526,72527,72528,72529,72530,72531,72532,72533,72534,72535,72536,72537,72538,72539,72540,72541,72542,72543,72544,72545,72546,72547,72548,72549,72550,72551,72552,72553,72554,72555,72556,72557,72558,72559,72560,72561,72562,72563,72564,72565,72566,72567,72568,72569,72570,72571,72572,72573,72574,72575,72576,72577,72578,72579,72580,72581,72582,72583,72584,72585,72586,72587,72588,72589,72590,72591,72592,72593,72594,72595,72596,72597,72598,72599,72600,72601,72602,72603,72604,72605,72606,72607,72608,72609,72610,72611,72612,72613,72614,72615,72616,72617,72618,72619,72620,72621,72622,72623,72624,72625,72626,72627,72628,72629,72630,72631,72632,72633,72634,72635,72636,72637,72638,72639,72640,72641,72642,72643,72644,72645,72646,72647,72648,72649,72650,72651,72652,72653,72654,72655,72656,72657,72658,72659,72660,72661,72662,72663,72664,72665,72666,72667,72668,72669,72670,72671,72672,72673,72674,72675,72676,72677,72678,72679,72680,72681,72682,72683,72684,72685,72686,72687,72688,72689,72690,72691,72692,72693,72694,72695,72696,72697,72698,72699,72700,72701,72702,72703,72704,72705,72706,72707,72708,72709,72710,72711,72712,72713,72714,72715,72716,72717,72718,72719,72720,72721,72722,72723,72724,72725,72726,72727,72728,72729,72730,72731,72732,72733,72734,72735,72842,72843,72844,72912,72913,72914,72915,72916,72917,72918,72919,72920,72921,72922,72923,72924,72925,72926,72927,72928,72929,72930,72931,72932,72933,72934,72935,72936,72937,72938,72939,72940,72941,72942,72943,72944,72945,72946,72947,72950,72951,72954,72955,72958,72959,72962,72963,72964,72966,72967,72968,72969,72970,72971,72972,72973,72974,72975,72976,72977,72978,72979,72980,72981,72982,72983,72984,72985,72986,72987,72988,72989,72990,72991,72992,72993,72994,72995,72996,72997,72998,72999,73000,73001,73002,73003,73004,73005,73006,73007,73008,73009,73010,73011,73012,73013,73014,73015,73016,73017,73018,73019,73020,73021,73022,73023,73024,73025,73026,73027,73028,73029,73030,73031,73032,73033,73034,73035,73036,73037,73038,73039,73040,73041,73042,73043,73044,73045,73046,73047,73048,73049,73050,73051,73052,73053,73054,73055,73056,73057,73058,73059,73060,73061,73062,73063,73064,73065,73066,73067,73068,73069,73070,73071,73072,73073,73074,73075,73076,73077,73078,73079,73080,73081,73082,73083,73084,73085,73086,73087,73088,73089,73090,73091,73092,73093,73094,73095,73096,73097,73098,73099,73100,73101,73102,73103,73104,73105,73106,73107,73108,73109,73110,73111,73112,73113,73114,73115,73116,73117,73118,73119,73120,73121,73122,73123,73124,73125,73126,73127,73128,73129,73130,73131,73132,73133,73134,73135,73136,73137,73138,73139,73140,73141,73142,73143,73144,73145,73146,73147,73148,73149,73150,73151,73152,73153,73154,73155,73156,73157,73158,73159,73160,73161,73162,73163,73164,73165,73166,73167,73172,73173,73174,73175,73176,73177,73178,73179,73180,73181,73182,73183,73184,73185,73186,73187,73188,73189,73190,73191,73192,73193,73194,73195,73196,73197,73198,73199,73200,73201,73202,73203,73204,73205,73206,73207,73208,73209,73210,73211,73212,73213,73214,73215,73216,73217,73218,73219,73220,73221,73222,73223,73224,73225,73226,73227,73228,73229,73230,73231,73232,73233,73234,73235,73236,73237,73238,73239,73240,73241,73242,73243,73244,73255,73256,73257,73258,73259,73260,73261,73262,73263,73264,73265,73266,73267,73268,73269,73270,73271,73272,73298,73299,73300,73301,73302,73303,73304,73305,73306,73309,73310,73311,73312,73313,73314,73319,73320,73321,73322,73323,73324,73325,73326,73327,73328,73329,73330,73331,73332,73333,73334,73335,73336,73337,};
	cout << "rightCalf: " << (sizeof(faceID) / sizeof(DWORD)) << endl;
	for (int i = 0; i < (int) sizeof(faceID) / sizeof(DWORD); ++i) {
		face_id_set.insert(faceID[i]);
	}
	__defined_area_dic[AREA_RIGHT_CALF] = defArea;
} while (0);
#endif	// !defined
#endif	// 특정 부위

#if 1	// 시뮬레이션; TO DO LIST: kSimulation::kSimulation 에서 설정하면 왜 잃어버리지???
#if 1	// defined
{
	const MODEL_COLOR_DEF_LIST&	ETRI_modelColorDefList = modelColorDefList[index_to_standard];
	cout << "# of def. = " << ETRI_modelColorDefList.n << endl;
	if (ETRI_modelColorDefList.n) {
		if (ETRI_modelColorDefList.n > N_MODEL_POINT_TYPE * N_BREAST_IMPLANT_TYPE)
			cerr << "# of definitions for Color Isometric Line = " << ETRI_modelColorDefList.n << ": too manay" << endl;
	}
	for (int k = 0; k < N_MODEL_POINT_TYPE * N_BREAST_IMPLANT_TYPE && k < ETRI_modelColorDefList.n; ++k) {
		const MODEL_COLOR_DEF&	modelColorDef = ETRI_modelColorDefList.ptr[k];
		cout << "MODEL_COLOR_DEF[" << k << "]: # = " << modelColorDef.n << endl;

		TEMPLATE_COLOR&	templateColor = BREAST_IMPLANT_TEMPLATE::ColorArray[k / N_BREAST_IMPLANT_TYPE][k % N_BREAST_IMPLANT_TYPE];
		templateColor.resize(0);
		for (int i = 0; i < modelColorDef.n; ++i) {
			templateColor.push_back(modelColorDef.ptr[i]);
		}
	}
}
#else	// defined/!defined
#if 1	// kState::printColor 를 설정하여 얻은 값 (SHIFT + '8')
#pragma warning(disable: 4305)
#if 1	// 왼쪽 가슴 위
static MODEL_COLOR	LEFT_UP_A[] = {
        {53615, 2,      0.500000f, 0.000000f, 0.000000f},
        {54814, 2,      0.500000f, 0.000000f, 0.000000f},
        {54854, 2,      0.500000f, 0.000000f, 0.000000f},
        {54856, 2,      0.500000f, 0.000000f, 0.000000f},
        {54858, 2,      0.500000f, 0.000000f, 0.000000f},
        {54860, 2,      0.500000f, 0.000000f, 0.000000f},
        {54862, 2,      0.500000f, 0.000000f, 0.000000f},
        {54894, 0,      0.500000f, 0.000000f, 0.000000f},
        {54894, 1,      0.500000f, 0.000000f, 0.000000f},
        {54895, 1,      1.000000f, 0.000000f, 0.000000f},
        {54896, 2,      1.000000f, 0.000000f, 0.000000f},
        {54898, 2,      1.000000f, 0.000000f, 0.000000f},
        {54900, 2,      1.000000f, 0.000000f, 0.000000f},
        {54902, 2,      1.000000f, 0.000000f, 0.000000f},
        {54904, 2,      0.500000f, 0.000000f, 0.000000f},
        {54907, 1,      0.500000f, 0.000000f, 0.000000f},
        {55192, 2,      0.500000f, 0.000000f, 0.000000f},
        {55194, 2,      1.000000f, 0.000000f, 0.000000f},
        {55194, 1,      0.500000f, 0.000000f, 0.000000f},
        {55196, 2,      1.000000f, 0.500000f, 0.000000f},
        {55196, 1,      1.000000f, 0.000000f, 0.000000f},
        {55198, 0,      1.000000f, 1.000000f, 0.000000f},
        {55198, 2,      1.000000f, 1.000000f, 0.000000f},
        {55198, 1,      1.000000f, 0.500000f, 0.000000f},
        {55199, 2,      1.000000f, 0.500000f, 0.000000f},
        {55200, 2,      1.000000f, 0.500000f, 0.000000f},
        {55203, 0,      1.000000f, 0.500000f, 0.000000f},
        {55203, 2,      1.000000f, 0.000000f, 0.000000f},
        {55205, 2,      0.500000f, 0.000000f, 0.000000f},
        {55211, 0,      1.000000f, 0.500000f, 0.000000f},
        {55211, 2,      1.000000f, 0.000000f, 0.000000f},
        {55213, 2,      0.500000f, 0.000000f, 0.000000f},
        {55219, 0,      1.000000f, 0.500000f, 0.000000f},
        {55219, 2,      1.000000f, 0.000000f, 0.000000f},
        {55221, 2,      0.500000f, 0.000000f, 0.000000f},
        {55227, 0,      1.000000f, 0.000000f, 0.000000f},
        {55235, 0,      0.500000f, 0.000000f, 0.000000f},
        {55398, 2,      0.500000f, 0.000000f, 0.000000f},
        {55400, 2,      0.500000f, 0.000000f, 0.000000f},
        {55402, 2,      0.500000f, 0.000000f, 0.000000f},
        {55408, 2,      0.500000f, 0.000000f, 0.000000f},
        {55410, 2,      1.000000f, 0.000000f, 0.000000f},
        {55412, 2,      1.000000f, 0.000000f, 0.000000f},
        {55414, 2,      1.000000f, 0.000000f, 0.000000f},
        {55418, 2,      0.500000f, 0.000000f, 0.000000f},
        {55420, 2,      1.000000f, 0.000000f, 0.000000f},
        {55422, 2,      1.000000f, 0.500000f, 0.000000f},
        {55424, 2,      1.000000f, 0.500000f, 0.000000f},
        {55426, 2,      1.000000f, 0.500000f, 0.000000f},
        {55428, 2,      0.500000f, 0.000000f, 0.000000f},
        {55432, 0,      1.000000f, 0.000000f, 0.000000f},
        {55435, 2,      0.500000f, 0.000000f, 0.000000f},
        {55437, 2,      1.000000f, 0.000000f, 0.000000f},
        {55439, 2,      1.000000f, 0.500000f, 0.000000f},
        {55441, 2,      1.000000f, 1.000000f, 0.000000f},
        {55443, 2,      1.000000f, 1.000000f, 0.000000f},
        {55445, 2,      1.000000f, 1.000000f, 0.000000f},
        {55447, 0,      0.500000f, 0.000000f, 0.000000f},
        {55447, 2,      1.000000f, 0.000000f, 0.000000f},
        {55449, 2,      1.000000f, 0.500000f, 0.000000f},
        {55451, 2,      1.000000f, 1.000000f, 0.000000f},
        {55453, 2,      1.000000f, 1.000000f, 0.500000f},
        {55455, 2,      1.000000f, 1.000000f, 0.500000f},
        {55457, 2,      1.000000f, 1.000000f, 0.000000f},
        {55459, 0,      1.000000f, 0.000000f, 0.000000f},
        {55459, 2,      1.000000f, 0.500000f, 0.000000f},
        {55461, 2,      1.000000f, 1.000000f, 0.000000f},
        {55463, 2,      1.000000f, 1.000000f, 0.500000f},
        {55465, 2,      1.000000f, 1.000000f, 1.000000f},
        {55467, 2,      1.000000f, 1.000000f, 0.500000f},
        {55469, 2,      1.000000f, 1.000000f, 0.000000f},
        {55471, 0,      1.000000f, 0.000000f, 0.000000f},
        {55471, 2,      1.000000f, 0.500000f, 0.000000f},
        {55473, 2,      1.000000f, 1.000000f, 0.000000f},
        {55475, 2,      1.000000f, 1.000000f, 0.500000f},
        {55477, 2,      1.000000f, 1.000000f, 0.500000f},
        {55483, 1,      1.000000f, 1.000000f, 0.000000f},
        {55484, 1,      1.000000f, 1.000000f, 0.000000f},
        {55486, 1,      1.000000f, 1.000000f, 0.000000f},
        {55488, 1,      1.000000f, 0.500000f, 0.000000f},
        {55490, 1,      1.000000f, 0.000000f, 0.000000f},
        {55586, 2,      0.500000f, 0.000000f, 0.000000f},
        {55588, 2,      0.500000f, 0.000000f, 0.000000f},
        {55590, 2,      0.500000f, 0.000000f, 0.000000f},
        {55592, 2,      0.500000f, 0.000000f, 0.000000f},
        {57402, 2,      0.500000f, 0.000000f, 0.000000f},
        {57406, 2,      1.000000f, 0.000000f, 0.000000f},
        {57410, 2,      1.000000f, 0.000000f, 0.000000f},
        {57414, 2,      1.000000f, 0.000000f, 0.000000f},
        {57418, 2,      1.000000f, 0.500000f, 0.000000f},
        {57422, 2,      1.000000f, 0.500000f, 0.000000f},
        {57426, 2,      1.000000f, 0.500000f, 0.000000f},
        {57430, 2,      1.000000f, 0.500000f, 0.000000f},
        {57434, 2,      1.000000f, 0.000000f, 0.000000f},
        {57438, 2,      0.500000f, 0.000000f, 0.000000f},
};
static MODEL_COLOR	LEFT_UP_B[] = {
        {53615, 2,      1.000000f, 0.000000f, 0.000000f},
        {54814, 2,      0.500000f, 0.000000f, 0.000000f},
        {54852, 2,      0.500000f, 0.000000f, 0.000000f},
        {54854, 2,      1.000000f, 0.000000f, 0.000000f},
        {54856, 2,      1.000000f, 0.000000f, 0.000000f},
        {54858, 2,      1.000000f, 0.000000f, 0.000000f},
        {54860, 2,      1.000000f, 0.000000f, 0.000000f},
        {54862, 2,      1.000000f, 0.000000f, 0.000000f},
        {54894, 0,      0.500000f, 0.000000f, 0.000000f},
        {54894, 1,      0.500000f, 0.000000f, 0.000000f},
        {54895, 1,      1.000000f, 0.000000f, 0.000000f},
        {54896, 2,      1.000000f, 0.000000f, 0.000000f},
        {54898, 2,      1.000000f, 0.000000f, 0.000000f},
        {54900, 2,      1.000000f, 0.000000f, 0.000000f},
        {54902, 2,      1.000000f, 0.000000f, 0.000000f},
        {54904, 2,      0.500000f, 0.000000f, 0.000000f},
        {54907, 1,      1.000000f, 0.000000f, 0.000000f},
        {54909, 1,      1.000000f, 0.000000f, 0.000000f},
        {55192, 2,      0.500000f, 0.000000f, 0.000000f},
        {55194, 2,      1.000000f, 0.000000f, 0.000000f},
        {55194, 1,      0.500000f, 0.000000f, 0.000000f},
        {55196, 2,      1.000000f, 0.500000f, 0.000000f},
        {55196, 1,      1.000000f, 0.000000f, 0.000000f},
        {55198, 0,      1.000000f, 1.000000f, 0.000000f},
        {55198, 2,      1.000000f, 1.000000f, 0.000000f},
        {55198, 1,      1.000000f, 0.500000f, 0.000000f},
        {55199, 2,      1.000000f, 0.500000f, 0.000000f},
        {55200, 2,      1.000000f, 0.500000f, 0.000000f},
        {55203, 0,      1.000000f, 0.500000f, 0.000000f},
        {55203, 2,      1.000000f, 0.000000f, 0.000000f},
        {55205, 2,      0.500000f, 0.000000f, 0.000000f},
        {55211, 0,      1.000000f, 0.500000f, 0.000000f},
        {55211, 2,      1.000000f, 0.000000f, 0.000000f},
        {55213, 2,      0.500000f, 0.000000f, 0.000000f},
        {55219, 0,      1.000000f, 0.500000f, 0.000000f},
        {55219, 2,      1.000000f, 0.000000f, 0.000000f},
        {55221, 2,      0.500000f, 0.000000f, 0.000000f},
        {55227, 0,      1.000000f, 0.000000f, 0.000000f},
        {55235, 0,      1.000000f, 0.000000f, 0.000000f},
        {55361, 2,      1.000000f, 0.000000f, 0.000000f},
        {55363, 2,      1.000000f, 0.000000f, 0.000000f},
        {55386, 2,      0.500000f, 0.000000f, 0.000000f},
        {55396, 2,      1.000000f, 0.000000f, 0.000000f},
        {55398, 2,      1.000000f, 0.000000f, 0.000000f},
        {55400, 2,      1.000000f, 0.000000f, 0.000000f},
        {55402, 2,      1.000000f, 0.000000f, 0.000000f},
        {55406, 2,      1.000000f, 0.000000f, 0.000000f},
        {55408, 2,      1.000000f, 0.000000f, 0.000000f},
        {55410, 2,      1.000000f, 0.500000f, 0.000000f},
        {55412, 2,      1.000000f, 1.000000f, 0.000000f},
        {55414, 2,      1.000000f, 1.000000f, 0.000000f},
        {55416, 2,      1.000000f, 0.000000f, 0.000000f},
        {55418, 2,      1.000000f, 0.000000f, 0.000000f},
        {55420, 2,      1.000000f, 0.500000f, 0.000000f},
        {55422, 2,      1.000000f, 1.000000f, 0.000000f},
        {55424, 2,      1.000000f, 1.000000f, 0.000000f},
        {55426, 2,      1.000000f, 1.000000f, 0.000000f},
        {55428, 0,      1.000000f, 0.000000f, 0.000000f},
        {55428, 2,      1.000000f, 0.000000f, 0.000000f},
        {55432, 0,      1.000000f, 1.000000f, 0.000000f},
        {55435, 0,      1.000000f, 0.000000f, 0.000000f},
        {55435, 2,      1.000000f, 0.000000f, 0.000000f},
        {55437, 2,      1.000000f, 1.000000f, 0.000000f},
        {55439, 2,      1.000000f, 1.000000f, 0.000000f},
        {55441, 2,      1.000000f, 1.000000f, 0.500000f},
        {55443, 2,      1.000000f, 1.000000f, 0.500000f},
        {55445, 2,      1.000000f, 1.000000f, 0.000000f},
        {55447, 0,      1.000000f, 0.000000f, 0.000000f},
        {55447, 2,      1.000000f, 0.500000f, 0.000000f},
        {55449, 2,      1.000000f, 1.000000f, 0.000000f},
        {55451, 2,      1.000000f, 1.000000f, 0.500000f},
        {55453, 2,      1.000000f, 1.000000f, 1.000000f},
        {55455, 2,      1.000000f, 1.000000f, 1.000000f},
        {55457, 2,      1.000000f, 1.000000f, 0.000000f},
        {55459, 0,      1.000000f, 0.000000f, 0.000000f},
        {55459, 2,      1.000000f, 1.000000f, 0.000000f},
        {55461, 2,      1.000000f, 1.000000f, 0.000000f},
        {55463, 2,      1.000000f, 1.000000f, 1.000000f},
        {55465, 2,      1.000000f, 1.000000f, 1.000000f},
        {55467, 2,      1.000000f, 1.000000f, 0.500000f},
        {55469, 2,      1.000000f, 1.000000f, 0.000000f},
        {55471, 0,      1.000000f, 0.000000f, 0.000000f},
        {55471, 2,      1.000000f, 1.000000f, 0.000000f},
        {55473, 2,      1.000000f, 1.000000f, 0.000000f},
        {55475, 2,      1.000000f, 1.000000f, 0.500000f},
        {55477, 2,      1.000000f, 1.000000f, 0.500000f},
        {55483, 1,      1.000000f, 1.000000f, 0.000000f},
        {55484, 1,      1.000000f, 1.000000f, 0.000000f},
        {55486, 1,      1.000000f, 1.000000f, 0.000000f},
        {55488, 1,      1.000000f, 1.000000f, 0.000000f},
        {55490, 1,      1.000000f, 0.000000f, 0.000000f},
        {55584, 2,      0.500000f, 0.000000f, 0.000000f},
        {55586, 2,      1.000000f, 0.000000f, 0.000000f},
        {55588, 2,      1.000000f, 0.000000f, 0.000000f},
        {55590, 2,      1.000000f, 0.000000f, 0.000000f},
        {55592, 2,      1.000000f, 0.000000f, 0.000000f},
        {57402, 2,      0.500000f, 0.000000f, 0.000000f},
        {57406, 2,      1.000000f, 0.000000f, 0.000000f},
        {57410, 2,      1.000000f, 0.000000f, 0.000000f},
        {57414, 2,      1.000000f, 0.000000f, 0.000000f},
        {57418, 2,      1.000000f, 0.500000f, 0.000000f},
        {57422, 2,      1.000000f, 1.000000f, 0.000000f},
        {57426, 2,      1.000000f, 1.000000f, 0.000000f},
        {57430, 2,      1.000000f, 1.000000f, 0.000000f},
        {57434, 2,      1.000000f, 0.500000f, 0.000000f},
        {57438, 2,      1.000000f, 0.000000f, 0.000000f},
        {57442, 2,      1.000000f, 0.000000f, 0.000000f},
};
static MODEL_COLOR	LEFT_UP_C[] = {
        {53615, 2,      1.000000f, 0.000000f, 0.000000f},
        {54814, 2,      1.000000f, 0.000000f, 0.000000f},
        {54814, 1,      1.000000f, 0.000000f, 0.000000f},
        {54817, 2,      0.500000f, 0.000000f, 0.000000f},
        {54852, 2,      0.500000f, 0.000000f, 0.000000f},
        {54854, 2,      1.000000f, 0.000000f, 0.000000f},
        {54856, 2,      1.000000f, 0.000000f, 0.000000f},
        {54858, 2,      1.000000f, 0.000000f, 0.000000f},
        {54860, 2,      1.000000f, 0.000000f, 0.000000f},
        {54862, 2,      1.000000f, 0.000000f, 0.000000f},
        {54894, 0,      1.000000f, 0.000000f, 0.000000f},
        {54894, 1,      1.000000f, 0.000000f, 0.000000f},
        {54895, 1,      1.000000f, 0.000000f, 0.000000f},
        {54896, 2,      1.000000f, 0.000000f, 0.000000f},
        {54898, 2,      1.000000f, 0.000000f, 0.000000f},
        {54900, 2,      1.000000f, 0.000000f, 0.000000f},
        {54902, 2,      1.000000f, 0.000000f, 0.000000f},
        {54904, 2,      1.000000f, 0.000000f, 0.000000f},
        {54907, 1,      1.000000f, 0.000000f, 0.000000f},
        {54909, 1,      1.000000f, 0.000000f, 0.000000f},
        {54943, 1,      1.000000f, 0.000000f, 0.000000f},
        {55190, 2,      0.500000f, 0.000000f, 0.000000f},
        {55192, 2,      1.000000f, 0.000000f, 0.000000f},
        {55192, 1,      1.000000f, 0.000000f, 0.000000f},
        {55194, 2,      1.000000f, 0.500000f, 0.000000f},
        {55194, 1,      1.000000f, 0.000000f, 0.000000f},
        {55196, 2,      1.000000f, 1.000000f, 0.000000f},
        {55196, 1,      1.000000f, 1.000000f, 0.000000f},
        {55198, 0,      1.000000f, 1.000000f, 0.500000f},
        {55198, 2,      1.000000f, 1.000000f, 0.500000f},
        {55198, 1,      1.000000f, 1.000000f, 0.000000f},
        {55199, 2,      1.000000f, 1.000000f, 0.000000f},
        {55200, 2,      1.000000f, 1.000000f, 0.000000f},
        {55203, 0,      1.000000f, 1.000000f, 0.000000f},
        {55203, 2,      1.000000f, 1.000000f, 0.000000f},
        {55205, 2,      1.000000f, 0.000000f, 0.000000f},
        {55207, 2,      1.000000f, 0.000000f, 0.000000f},
        {55211, 0,      1.000000f, 1.000000f, 0.000000f},
        {55211, 2,      1.000000f, 0.500000f, 0.000000f},
        {55213, 2,      1.000000f, 0.000000f, 0.000000f},
        {55215, 2,      0.500000f, 0.000000f, 0.000000f},
        {55219, 0,      1.000000f, 1.000000f, 0.000000f},
        {55219, 2,      1.000000f, 0.000000f, 0.000000f},
        {55221, 2,      1.000000f, 0.000000f, 0.000000f},
        {55227, 0,      1.000000f, 0.000000f, 0.000000f},
        {55235, 0,      1.000000f, 0.000000f, 0.000000f},
        {55361, 2,      1.000000f, 0.000000f, 0.000000f},
        {55363, 2,      1.000000f, 0.000000f, 0.000000f},
        {55386, 2,      0.500000f, 0.000000f, 0.000000f},
        {55396, 2,      1.000000f, 0.000000f, 0.000000f},
        {55398, 2,      1.000000f, 0.000000f, 0.000000f},
        {55400, 2,      1.000000f, 0.000000f, 0.000000f},
        {55402, 2,      1.000000f, 0.000000f, 0.000000f},
        {55406, 2,      1.000000f, 0.000000f, 0.000000f},
        {55408, 2,      1.000000f, 0.000000f, 0.000000f},
        {55410, 2,      1.000000f, 0.500000f, 0.000000f},
        {55412, 2,      1.000000f, 1.000000f, 0.000000f},
        {55414, 2,      1.000000f, 1.000000f, 0.000000f},
        {55416, 2,      1.000000f, 0.000000f, 0.000000f},
        {55418, 2,      1.000000f, 0.000000f, 0.000000f},
        {55420, 2,      1.000000f, 0.500000f, 0.000000f},
        {55422, 2,      1.000000f, 1.000000f, 0.000000f},
        {55424, 2,      1.000000f, 1.000000f, 0.000000f},
        {55426, 2,      1.000000f, 1.000000f, 0.000000f},
        {55428, 0,      1.000000f, 0.000000f, 0.000000f},
        {55428, 2,      1.000000f, 0.000000f, 0.000000f},
        {55432, 0,      1.000000f, 1.000000f, 0.000000f},
        {55435, 0,      1.000000f, 0.000000f, 0.000000f},
        {55435, 2,      1.000000f, 0.000000f, 0.000000f},
        {55437, 2,      1.000000f, 1.000000f, 0.000000f},
        {55439, 2,      1.000000f, 1.000000f, 0.000000f},
        {55441, 2,      1.000000f, 1.000000f, 0.500000f},
        {55443, 2,      1.000000f, 1.000000f, 0.500000f},
        {55445, 2,      1.000000f, 1.000000f, 0.000000f},
        {55447, 0,      1.000000f, 0.000000f, 0.000000f},
        {55447, 2,      1.000000f, 0.500000f, 0.000000f},
        {55449, 2,      1.000000f, 1.000000f, 0.000000f},
        {55451, 2,      1.000000f, 1.000000f, 0.500000f},
        {55453, 2,      1.000000f, 1.000000f, 1.000000f},
        {55455, 2,      1.000000f, 1.000000f, 1.000000f},
        {55457, 2,      1.000000f, 1.000000f, 0.000000f},
        {55459, 0,      1.000000f, 0.000000f, 0.000000f},
        {55459, 2,      1.000000f, 1.000000f, 0.000000f},
        {55461, 2,      1.000000f, 1.000000f, 0.000000f},
        {55463, 2,      1.000000f, 1.000000f, 1.000000f},
        {55465, 2,      1.000000f, 1.000000f, 1.000000f},
        {55467, 2,      1.000000f, 1.000000f, 1.000000f},
        {55469, 2,      1.000000f, 1.000000f, 0.000000f},
        {55471, 0,      1.000000f, 0.000000f, 0.000000f},
        {55471, 2,      1.000000f, 1.000000f, 0.000000f},
        {55473, 2,      1.000000f, 1.000000f, 0.000000f},
        {55475, 2,      1.000000f, 1.000000f, 1.000000f},
        {55477, 2,      1.000000f, 1.000000f, 1.000000f},
        {55483, 1,      1.000000f, 1.000000f, 0.500000f},
        {55484, 1,      1.000000f, 1.000000f, 0.500000f},
        {55486, 1,      1.000000f, 1.000000f, 0.000000f},
        {55488, 1,      1.000000f, 1.000000f, 0.000000f},
        {55490, 1,      1.000000f, 0.000000f, 0.000000f},
        {55584, 2,      0.500000f, 0.000000f, 0.000000f},
        {55586, 2,      1.000000f, 0.000000f, 0.000000f},
        {55588, 2,      1.000000f, 0.000000f, 0.000000f},
        {55590, 2,      1.000000f, 0.000000f, 0.000000f},
        {55592, 2,      1.000000f, 0.000000f, 0.000000f},
        {57398, 2,      1.000000f, 0.000000f, 0.000000f},
        {57402, 2,      1.000000f, 0.000000f, 0.000000f},
        {57406, 2,      1.000000f, 0.500000f, 0.000000f},
        {57410, 2,      1.000000f, 1.000000f, 0.000000f},
        {57414, 2,      1.000000f, 1.000000f, 0.000000f},
        {57418, 2,      1.000000f, 1.000000f, 0.000000f},
        {57422, 2,      1.000000f, 1.000000f, 0.000000f},
        {57426, 2,      1.000000f, 1.000000f, 0.000000f},
        {57430, 2,      1.000000f, 1.000000f, 0.000000f},
        {57434, 2,      1.000000f, 0.500000f, 0.000000f},
        {57438, 2,      1.000000f, 0.000000f, 0.000000f},
        {57442, 2,      1.000000f, 0.000000f, 0.000000f},
};
#endif
#if 1	// 오른쪽 가슴 위
static MODEL_COLOR	RIGHT_UP_A[] = {
        {57501, 2,      0.500000f, 0.000000f, 0.000000f},
        {57503, 2,      0.500000f, 0.000000f, 0.000000f},
        {57504, 0,      0.500000f, 0.000000f, 0.000000f},
        {57547, 0,      0.500000f, 0.000000f, 0.000000f},
        {57547, 1,      0.500000f, 0.000000f, 0.000000f},
        {57549, 2,      0.500000f, 0.000000f, 0.000000f},
        {57638, 0,      1.000000f, 0.000000f, 0.000000f},
        {57638, 2,      1.000000f, 0.000000f, 0.000000f},
        {57641, 2,      0.500000f, 0.000000f, 0.000000f},
        {57680, 2,      0.500000f, 0.000000f, 0.000000f},
        {57682, 2,      1.000000f, 0.000000f, 0.000000f},
        {57684, 2,      1.000000f, 0.000000f, 0.000000f},
        {57686, 2,      1.000000f, 0.000000f, 0.000000f},
        {57718, 2,      0.500000f, 0.000000f, 0.000000f},
        {57718, 1,      1.000000f, 0.000000f, 0.000000f},
        {57966, 2,      0.500000f, 0.000000f, 0.000000f},
        {57968, 0,      0.500000f, 0.000000f, 0.000000f},
        {57968, 2,      1.000000f, 0.000000f, 0.000000f},
        {57970, 0,      1.000000f, 0.500000f, 0.000000f},
        {57970, 2,      1.000000f, 0.500000f, 0.000000f},
        {57970, 1,      1.000000f, 1.000000f, 0.000000f},
        {57971, 2,      1.000000f, 0.500000f, 0.000000f},
        {57972, 2,      1.000000f, 0.000000f, 0.000000f},
        {57975, 2,      0.500000f, 0.000000f, 0.000000f},
        {57975, 1,      1.000000f, 0.000000f, 0.000000f},
        {57983, 2,      0.500000f, 0.000000f, 0.000000f},
        {57983, 1,      1.000000f, 0.000000f, 0.000000f},
        {57991, 2,      0.500000f, 0.000000f, 0.000000f},
        {57991, 1,      1.000000f, 0.000000f, 0.000000f},
        {57999, 1,      0.500000f, 0.000000f, 0.000000f},
        {58133, 2,      0.500000f, 0.000000f, 0.000000f},
        {58135, 2,      0.500000f, 0.000000f, 0.000000f},
        {58170, 2,      0.500000f, 0.000000f, 0.000000f},
        {58172, 2,      1.000000f, 0.000000f, 0.000000f},
        {58174, 2,      1.000000f, 0.000000f, 0.000000f},
        {58180, 2,      0.500000f, 0.000000f, 0.000000f},
        {58182, 2,      1.000000f, 0.000000f, 0.000000f},
        {58184, 2,      1.000000f, 0.500000f, 0.000000f},
        {58186, 2,      1.000000f, 0.500000f, 0.000000f},
        {58190, 2,      0.500000f, 0.000000f, 0.000000f},
        {58192, 2,      1.000000f, 0.000000f, 0.000000f},
        {58194, 2,      1.000000f, 0.500000f, 0.000000f},
        {58196, 2,      1.000000f, 1.000000f, 0.000000f},
        {58198, 2,      1.000000f, 1.000000f, 0.000000f},
        {58200, 2,      0.500000f, 0.000000f, 0.000000f},
        {58204, 1,      1.000000f, 0.000000f, 0.000000f},
        {58207, 2,      0.500000f, 0.000000f, 0.000000f},
        {58209, 2,      1.000000f, 0.000000f, 0.000000f},
        {58211, 2,      1.000000f, 0.500000f, 0.000000f},
        {58213, 2,      1.000000f, 1.000000f, 0.000000f},
        {58215, 2,      1.000000f, 1.000000f, 0.500000f},
        {58217, 2,      1.000000f, 1.000000f, 0.500000f},
        {58219, 2,      1.000000f, 0.000000f, 0.000000f},
        {58219, 1,      0.500000f, 0.000000f, 0.000000f},
        {58221, 2,      1.000000f, 0.500000f, 0.000000f},
        {58223, 2,      1.000000f, 1.000000f, 0.000000f},
        {58225, 2,      1.000000f, 1.000000f, 0.500000f},
        {58227, 2,      1.000000f, 1.000000f, 1.000000f},
        {58229, 2,      1.000000f, 1.000000f, 0.500000f},
        {58231, 2,      1.000000f, 0.000000f, 0.000000f},
        {58231, 1,      0.500000f, 0.000000f, 0.000000f},
        {58233, 2,      1.000000f, 0.500000f, 0.000000f},
        {58235, 2,      1.000000f, 1.000000f, 0.000000f},
        {58237, 2,      1.000000f, 1.000000f, 0.500000f},
        {58239, 2,      1.000000f, 1.000000f, 0.500000f},
        {58241, 2,      1.000000f, 1.000000f, 0.000000f},
        {58243, 2,      1.000000f, 0.000000f, 0.000000f},
        {58243, 1,      0.500000f, 0.000000f, 0.000000f},
        {58245, 2,      1.000000f, 0.500000f, 0.000000f},
        {58247, 2,      1.000000f, 1.000000f, 0.000000f},
        {58249, 2,      1.000000f, 1.000000f, 0.000000f},
        {58255, 1,      1.000000f, 0.500000f, 0.000000f},
        {58256, 0,      1.000000f, 0.500000f, 0.000000f},
        {58258, 0,      1.000000f, 0.500000f, 0.000000f},
        {58260, 0,      1.000000f, 0.000000f, 0.000000f},
        {58262, 0,      0.500000f, 0.000000f, 0.000000f},
        {60170, 2,      0.500000f, 0.000000f, 0.000000f},
        {60174, 0,      0.500000f, 0.000000f, 0.000000f},
        {60174, 2,      1.000000f, 0.000000f, 0.000000f},
        {60178, 0,      1.000000f, 0.000000f, 0.000000f},
        {60178, 2,      1.000000f, 0.500000f, 0.000000f},
        {60182, 0,      1.000000f, 0.500000f, 0.000000f},
        {60182, 2,      1.000000f, 1.000000f, 0.000000f},
        {60186, 0,      1.000000f, 0.500000f, 0.000000f},
        {60186, 2,      1.000000f, 1.000000f, 0.000000f},
        {60190, 0,      1.000000f, 0.500000f, 0.000000f},
        {60190, 2,      1.000000f, 1.000000f, 0.000000f},
        {60194, 0,      1.000000f, 0.500000f, 0.000000f},
        {60194, 2,      1.000000f, 0.500000f, 0.000000f},
        {60198, 2,      1.000000f, 0.000000f, 0.000000f},
        {60202, 2,      1.000000f, 0.000000f, 0.000000f},
        {60206, 0,      0.500000f, 0.000000f, 0.000000f},
        {60206, 2,      0.500000f, 0.000000f, 0.000000f},
};
static MODEL_COLOR	RIGHT_UP_B[] = {
        {57501, 2,      1.000000f, 0.000000f, 0.000000f},
        {57503, 2,      1.000000f, 0.000000f, 0.000000f},
        {57504, 0,      1.000000f, 0.000000f, 0.000000f},
        {57547, 0,      1.000000f, 0.000000f, 0.000000f},
        {57547, 1,      1.000000f, 0.000000f, 0.000000f},
        {57549, 2,      0.500000f, 0.000000f, 0.000000f},
        {57638, 0,      1.000000f, 0.000000f, 0.000000f},
        {57638, 2,      1.000000f, 0.000000f, 0.000000f},
        {57641, 2,      1.000000f, 0.000000f, 0.000000f},
        {57643, 2,      0.500000f, 0.000000f, 0.000000f},
        {57680, 2,      0.500000f, 0.000000f, 0.000000f},
        {57682, 2,      1.000000f, 0.000000f, 0.000000f},
        {57684, 2,      1.000000f, 0.000000f, 0.000000f},
        {57686, 2,      1.000000f, 0.000000f, 0.000000f},
        {57718, 2,      1.000000f, 0.000000f, 0.000000f},
        {57718, 1,      1.000000f, 0.500000f, 0.000000f},
        {57964, 2,      1.000000f, 0.000000f, 0.000000f},
        {57966, 0,      1.000000f, 0.000000f, 0.000000f},
        {57966, 2,      1.000000f, 0.000000f, 0.000000f},
        {57968, 0,      1.000000f, 0.000000f, 0.000000f},
        {57968, 2,      1.000000f, 1.000000f, 0.000000f},
        {57970, 0,      1.000000f, 1.000000f, 0.000000f},
        {57970, 2,      1.000000f, 1.000000f, 0.000000f},
        {57970, 1,      1.000000f, 1.000000f, 0.500000f},
        {57971, 2,      1.000000f, 1.000000f, 0.000000f},
        {57972, 2,      1.000000f, 1.000000f, 0.000000f},
        {57975, 2,      1.000000f, 0.000000f, 0.000000f},
        {57975, 1,      1.000000f, 1.000000f, 0.000000f},
        {57977, 2,      1.000000f, 0.000000f, 0.000000f},
        {57983, 2,      1.000000f, 0.000000f, 0.000000f},
        {57983, 1,      1.000000f, 0.500000f, 0.000000f},
        {57985, 2,      0.500000f, 0.000000f, 0.000000f},
        {57991, 2,      1.000000f, 0.000000f, 0.000000f},
        {57991, 1,      1.000000f, 0.000000f, 0.000000f},
        {57999, 1,      1.000000f, 0.000000f, 0.000000f},
        {58133, 2,      0.500000f, 0.000000f, 0.000000f},
        {58135, 2,      0.500000f, 0.000000f, 0.000000f},
        {58170, 2,      0.500000f, 0.000000f, 0.000000f},
        {58172, 2,      1.000000f, 0.000000f, 0.000000f},
        {58174, 2,      1.000000f, 0.000000f, 0.000000f},
        {58180, 2,      0.500000f, 0.000000f, 0.000000f},
        {58182, 2,      1.000000f, 0.000000f, 0.000000f},
        {58184, 2,      1.000000f, 0.500000f, 0.000000f},
        {58186, 2,      1.000000f, 0.500000f, 0.000000f},
        {58190, 2,      0.500000f, 0.000000f, 0.000000f},
        {58192, 2,      1.000000f, 0.000000f, 0.000000f},
        {58194, 2,      1.000000f, 0.500000f, 0.000000f},
        {58196, 2,      1.000000f, 1.000000f, 0.000000f},
        {58198, 2,      1.000000f, 1.000000f, 0.000000f},
        {58200, 2,      0.500000f, 0.000000f, 0.000000f},
        {58204, 1,      1.000000f, 0.000000f, 0.000000f},
        {58207, 2,      0.500000f, 0.000000f, 0.000000f},
        {58209, 2,      1.000000f, 0.000000f, 0.000000f},
        {58211, 2,      1.000000f, 0.500000f, 0.000000f},
        {58213, 2,      1.000000f, 1.000000f, 0.000000f},
        {58215, 2,      1.000000f, 1.000000f, 0.500000f},
        {58217, 2,      1.000000f, 1.000000f, 0.500000f},
        {58219, 2,      1.000000f, 0.000000f, 0.000000f},
        {58219, 1,      0.500000f, 0.000000f, 0.000000f},
        {58221, 2,      1.000000f, 0.500000f, 0.000000f},
        {58223, 2,      1.000000f, 1.000000f, 0.000000f},
        {58225, 2,      1.000000f, 1.000000f, 0.500000f},
        {58227, 2,      1.000000f, 1.000000f, 1.000000f},
        {58229, 2,      1.000000f, 1.000000f, 1.000000f},
        {58231, 2,      1.000000f, 0.000000f, 0.000000f},
        {58231, 1,      1.000000f, 0.000000f, 0.000000f},
        {58233, 2,      1.000000f, 1.000000f, 0.000000f},
        {58235, 2,      1.000000f, 1.000000f, 0.000000f},
        {58237, 2,      1.000000f, 1.000000f, 1.000000f},
        {58239, 2,      1.000000f, 1.000000f, 1.000000f},
        {58241, 2,      1.000000f, 1.000000f, 0.500000f},
        {58243, 2,      1.000000f, 0.000000f, 0.000000f},
        {58243, 1,      1.000000f, 0.000000f, 0.000000f},
        {58245, 2,      1.000000f, 1.000000f, 0.000000f},
        {58247, 2,      1.000000f, 1.000000f, 0.000000f},
        {58249, 2,      1.000000f, 1.000000f, 0.500000f},
        {58255, 1,      1.000000f, 1.000000f, 0.000000f},
        {58256, 0,      1.000000f, 1.000000f, 0.000000f},
        {58258, 0,      1.000000f, 1.000000f, 0.000000f},
        {58260, 0,      1.000000f, 0.000000f, 0.000000f},
        {58262, 0,      1.000000f, 0.000000f, 0.000000f},
        {60170, 2,      0.500000f, 0.000000f, 0.000000f},
        {60174, 0,      0.500000f, 0.000000f, 0.000000f},
        {60174, 2,      1.000000f, 0.000000f, 0.000000f},
        {60178, 0,      1.000000f, 0.000000f, 0.000000f},
        {60178, 2,      1.000000f, 0.500000f, 0.000000f},
        {60182, 0,      1.000000f, 0.500000f, 0.000000f},
        {60182, 2,      1.000000f, 1.000000f, 0.000000f},
        {60186, 0,      1.000000f, 1.000000f, 0.000000f},
        {60186, 2,      1.000000f, 1.000000f, 0.000000f},
        {60190, 0,      1.000000f, 1.000000f, 0.000000f},
        {60190, 2,      1.000000f, 1.000000f, 0.000000f},
        {60194, 0,      1.000000f, 1.000000f, 0.000000f},
        {60194, 2,      1.000000f, 1.000000f, 0.000000f},
        {60198, 2,      1.000000f, 1.000000f, 0.000000f},
        {60202, 2,      1.000000f, 0.500000f, 0.000000f},
        {60206, 0,      1.000000f, 0.000000f, 0.000000f},
        {60206, 2,      1.000000f, 0.000000f, 0.000000f},
        {60210, 0,      1.000000f, 0.000000f, 0.000000f},
        {60210, 2,      1.000000f, 0.000000f, 0.000000f},
};
static MODEL_COLOR	RIGHT_UP_C[] = {
        {57501, 2,      1.000000f, 0.000000f, 0.000000f},
        {57503, 2,      1.000000f, 0.000000f, 0.000000f},
        {57504, 0,      1.000000f, 0.000000f, 0.000000f},
        {57545, 1,      0.500000f, 0.000000f, 0.000000f},
        {57547, 0,      1.000000f, 0.000000f, 0.000000f},
        {57547, 1,      1.000000f, 0.000000f, 0.000000f},
        {57549, 2,      1.000000f, 0.000000f, 0.000000f},
        {57638, 0,      1.000000f, 0.000000f, 0.000000f},
        {57638, 2,      1.000000f, 0.000000f, 0.000000f},
        {57641, 2,      1.000000f, 0.000000f, 0.000000f},
        {57643, 2,      0.500000f, 0.000000f, 0.000000f},
        {57678, 2,      1.000000f, 0.000000f, 0.000000f},
        {57680, 2,      1.000000f, 0.000000f, 0.000000f},
        {57682, 2,      1.000000f, 0.000000f, 0.000000f},
        {57684, 2,      1.000000f, 0.000000f, 0.000000f},
        {57686, 2,      1.000000f, 0.000000f, 0.000000f},
        {57718, 2,      1.000000f, 0.000000f, 0.000000f},
        {57718, 1,      1.000000f, 0.500000f, 0.000000f},
        {57964, 2,      1.000000f, 0.000000f, 0.000000f},
        {57966, 0,      1.000000f, 0.000000f, 0.000000f},
        {57966, 2,      1.000000f, 0.000000f, 0.000000f},
        {57968, 0,      1.000000f, 0.000000f, 0.000000f},
        {57968, 2,      1.000000f, 1.000000f, 0.000000f},
        {57970, 0,      1.000000f, 1.000000f, 0.000000f},
        {57970, 2,      1.000000f, 1.000000f, 0.000000f},
        {57970, 1,      1.000000f, 1.000000f, 0.500000f},
        {57971, 2,      1.000000f, 1.000000f, 0.000000f},
        {57972, 2,      1.000000f, 1.000000f, 0.000000f},
        {57975, 2,      1.000000f, 0.000000f, 0.000000f},
        {57975, 1,      1.000000f, 1.000000f, 0.000000f},
        {57977, 2,      1.000000f, 0.000000f, 0.000000f},
        {57983, 2,      1.000000f, 0.000000f, 0.000000f},
        {57983, 1,      1.000000f, 0.500000f, 0.000000f},
        {57985, 2,      0.500000f, 0.000000f, 0.000000f},
        {57991, 2,      1.000000f, 0.000000f, 0.000000f},
        {57991, 1,      1.000000f, 0.000000f, 0.000000f},
        {57999, 1,      1.000000f, 0.000000f, 0.000000f},
        {58116, 2,      0.500000f, 0.000000f, 0.000000f},
        {58118, 2,      1.000000f, 0.000000f, 0.000000f},
        {58131, 2,      0.500000f, 0.000000f, 0.000000f},
        {58133, 2,      1.000000f, 0.000000f, 0.000000f},
        {58135, 2,      1.000000f, 0.000000f, 0.000000f},
        {58158, 2,      1.000000f, 0.000000f, 0.000000f},
        {58168, 2,      1.000000f, 0.000000f, 0.000000f},
        {58170, 2,      1.000000f, 0.000000f, 0.000000f},
        {58172, 2,      1.000000f, 0.500000f, 0.000000f},
        {58174, 2,      1.000000f, 1.000000f, 0.000000f},
        {58178, 2,      1.000000f, 0.000000f, 0.000000f},
        {58180, 2,      1.000000f, 0.000000f, 0.000000f},
        {58182, 2,      1.000000f, 1.000000f, 0.000000f},
        {58184, 2,      1.000000f, 1.000000f, 0.000000f},
        {58186, 2,      1.000000f, 1.000000f, 0.000000f},
        {58188, 2,      1.000000f, 0.000000f, 0.000000f},
        {58190, 2,      1.000000f, 0.000000f, 0.000000f},
        {58192, 2,      1.000000f, 1.000000f, 0.000000f},
        {58194, 2,      1.000000f, 1.000000f, 0.000000f},
        {58196, 2,      1.000000f, 1.000000f, 0.500000f},
        {58198, 2,      1.000000f, 1.000000f, 0.500000f},
        {58200, 2,      1.000000f, 0.000000f, 0.000000f},
        {58200, 1,      1.000000f, 0.000000f, 0.000000f},
        {58204, 1,      1.000000f, 1.000000f, 0.000000f},
        {58207, 2,      1.000000f, 0.000000f, 0.000000f},
        {58207, 1,      0.500000f, 0.000000f, 0.000000f},
        {58209, 2,      1.000000f, 0.500000f, 0.000000f},
        {58211, 2,      1.000000f, 1.000000f, 0.000000f},
        {58213, 2,      1.000000f, 1.000000f, 0.500000f},
        {58215, 2,      1.000000f, 1.000000f, 1.000000f},
        {58217, 2,      1.000000f, 1.000000f, 1.000000f},
        {58219, 2,      1.000000f, 0.000000f, 0.000000f},
        {58219, 1,      1.000000f, 0.000000f, 0.000000f},
        {58221, 2,      1.000000f, 1.000000f, 0.000000f},
        {58223, 2,      1.000000f, 1.000000f, 0.000000f},
        {58225, 2,      1.000000f, 1.000000f, 1.000000f},
        {58227, 2,      1.000000f, 1.000000f, 1.000000f},
        {58229, 2,      1.000000f, 1.000000f, 1.000000f},
        {58231, 2,      1.000000f, 0.000000f, 0.000000f},
        {58231, 1,      1.000000f, 0.000000f, 0.000000f},
        {58233, 2,      1.000000f, 1.000000f, 0.000000f},
        {58235, 2,      1.000000f, 1.000000f, 0.000000f},
        {58237, 2,      1.000000f, 1.000000f, 1.000000f},
        {58239, 2,      1.000000f, 1.000000f, 1.000000f},
        {58241, 2,      1.000000f, 1.000000f, 0.500000f},
        {58243, 2,      1.000000f, 0.000000f, 0.000000f},
        {58243, 1,      1.000000f, 0.000000f, 0.000000f},
        {58245, 2,      1.000000f, 1.000000f, 0.000000f},
        {58247, 2,      1.000000f, 1.000000f, 0.000000f},
        {58249, 2,      1.000000f, 1.000000f, 0.500000f},
        {58255, 1,      1.000000f, 1.000000f, 0.000000f},
        {58256, 0,      1.000000f, 1.000000f, 0.000000f},
        {58258, 0,      1.000000f, 1.000000f, 0.000000f},
        {58260, 0,      1.000000f, 0.000000f, 0.000000f},
        {58262, 0,      1.000000f, 0.000000f, 0.000000f},
        {60166, 2,      1.000000f, 0.000000f, 0.000000f},
        {60170, 0,      1.000000f, 0.000000f, 0.000000f},
        {60170, 2,      1.000000f, 0.000000f, 0.000000f},
        {60174, 0,      1.000000f, 0.000000f, 0.000000f},
        {60174, 2,      1.000000f, 1.000000f, 0.000000f},
        {60178, 0,      1.000000f, 0.500000f, 0.000000f},
        {60178, 2,      1.000000f, 1.000000f, 0.000000f},
        {60182, 0,      1.000000f, 1.000000f, 0.000000f},
        {60182, 2,      1.000000f, 1.000000f, 0.000000f},
        {60186, 0,      1.000000f, 1.000000f, 0.000000f},
        {60186, 2,      1.000000f, 1.000000f, 0.000000f},
        {60190, 0,      1.000000f, 1.000000f, 0.000000f},
        {60190, 2,      1.000000f, 1.000000f, 0.000000f},
        {60194, 0,      1.000000f, 1.000000f, 0.000000f},
        {60194, 2,      1.000000f, 1.000000f, 0.000000f},
        {60198, 2,      1.000000f, 1.000000f, 0.000000f},
        {60202, 2,      1.000000f, 0.500000f, 0.000000f},
        {60206, 0,      1.000000f, 0.000000f, 0.000000f},
        {60206, 2,      1.000000f, 0.000000f, 0.000000f},
        {60210, 0,      1.000000f, 0.000000f, 0.000000f},
        {60210, 2,      1.000000f, 0.000000f, 0.000000f},
};
#endif
#pragma warning(default: 4305)
#endif

#if 1	// BREAST_IMPLANT_TEMPLATE::ColorArray 를 설정
	TEMPLATE_COLOR	templateColor;
//	int	index = 0;

	// LEFT_UP_A
	templateColor.resize(0);
//	(void) fprintf(stdout, "LEFT_UP_A[%d]\n", sizeof(LEFT_UP_A) / sizeof(MODEL_COLOR));
	for (int i = 0; i < sizeof(LEFT_UP_A) / sizeof(MODEL_COLOR); ++i) {
		templateColor.push_back(LEFT_UP_A[i]);
//		BREAST_IMPLANT_TEMPLATE::ColorArray[index].push_back(LEFT_UP_A[i]);
	}
//	BREAST_IMPLANT_TEMPLATE::ColorArray[index++] = templateColor;
	BREAST_IMPLANT_TEMPLATE::ColorArray[MODEL_POINT_LEFT_UP][BREAST_IMPLANT_TYPE_A] = templateColor;
//	(void) fprintf(stdout, "# = %d\n", BREAST_IMPLANT_TEMPLATE::ColorArray[index - 1].size());

	// LEFT_UP_B
	templateColor.resize(0);
//	(void) fprintf(stdout, "LEFT_UP_B[%d]\n", sizeof(LEFT_UP_B) / sizeof(MODEL_COLOR));
	for (int i = 0; i < sizeof(LEFT_UP_B) / sizeof(MODEL_COLOR); ++i) {
		templateColor.push_back(LEFT_UP_B[i]);
//		BREAST_IMPLANT_TEMPLATE::ColorArray[index].push_back(LEFT_UP_B[i]);
	}
//	BREAST_IMPLANT_TEMPLATE::ColorArray[index++] = templateColor;
	BREAST_IMPLANT_TEMPLATE::ColorArray[MODEL_POINT_LEFT_UP][BREAST_IMPLANT_TYPE_B] = templateColor;
//	(void) fprintf(stdout, "# = %d\n", BREAST_IMPLANT_TEMPLATE::ColorArray[index - 1].size());

	// LEFT_UP_C
	templateColor.resize(0);
//	(void) fprintf(stdout, "LEFT_UP_C[%d]\n", sizeof(LEFT_UP_C) / sizeof(MODEL_COLOR));
	for (int i = 0; i < sizeof(LEFT_UP_C) / sizeof(MODEL_COLOR); ++i) {
		templateColor.push_back(LEFT_UP_C[i]);
//		BREAST_IMPLANT_TEMPLATE::ColorArray[index].push_back(LEFT_UP_C[i]);
	}
//	BREAST_IMPLANT_TEMPLATE::ColorArray[index++] = templateColor;
	BREAST_IMPLANT_TEMPLATE::ColorArray[MODEL_POINT_LEFT_UP][BREAST_IMPLANT_TYPE_C] = templateColor;
//	(void) fprintf(stdout, "# = %d\n", BREAST_IMPLANT_TEMPLATE::ColorArray[index - 1].size());

	// RIGHT_UP_A
	templateColor.resize(0);
//	(void) fprintf(stdout, "RIGHT_UP_A[%d]\n", sizeof(RIGHT_UP_A) / sizeof(MODEL_COLOR));
	for (int i = 0; i < sizeof(RIGHT_UP_A) / sizeof(MODEL_COLOR); ++i) {
		templateColor.push_back(RIGHT_UP_A[i]);
//		BREAST_IMPLANT_TEMPLATE::ColorArray[index].push_back(RIGHT_UP_A[i]);
	}
//	BREAST_IMPLANT_TEMPLATE::ColorArray[index++] = templateColor;
	BREAST_IMPLANT_TEMPLATE::ColorArray[MODEL_POINT_RIGHT_UP][BREAST_IMPLANT_TYPE_A] = templateColor;
//	(void) fprintf(stdout, "# = %d\n", BREAST_IMPLANT_TEMPLATE::ColorArray[index - 1].size());

	// RIGHT_UP_B
	templateColor.resize(0);
//	(void) fprintf(stdout, "RIGHT_UP_B[%d]\n", sizeof(RIGHT_UP_B) / sizeof(MODEL_COLOR));
	for (int i = 0; i < sizeof(RIGHT_UP_B) / sizeof(MODEL_COLOR); ++i) {
		templateColor.push_back(RIGHT_UP_B[i]);
//		BREAST_IMPLANT_TEMPLATE::ColorArray[index].push_back(RIGHT_UP_B[i]);
	}
//	BREAST_IMPLANT_TEMPLATE::ColorArray[index++] = templateColor;
	BREAST_IMPLANT_TEMPLATE::ColorArray[MODEL_POINT_RIGHT_UP][BREAST_IMPLANT_TYPE_B] = templateColor;
//	(void) fprintf(stdout, "# = %d\n", BREAST_IMPLANT_TEMPLATE::ColorArray[index - 1].size());

	// RIGHT_UP_C
	templateColor.resize(0);
//	(void) fprintf(stdout, "RIGHT_UP_C[%d]\n", sizeof(RIGHT_UP_C) / sizeof(MODEL_COLOR));
	for (int i = 0; i < sizeof(RIGHT_UP_C) / sizeof(MODEL_COLOR); ++i) {
		templateColor.push_back(RIGHT_UP_C[i]);
//		BREAST_IMPLANT_TEMPLATE::ColorArray[index].push_back(RIGHT_UP_C[i]);
	}
//	BREAST_IMPLANT_TEMPLATE::ColorArray[index++] = templateColor;
	BREAST_IMPLANT_TEMPLATE::ColorArray[MODEL_POINT_RIGHT_UP][BREAST_IMPLANT_TYPE_C] = templateColor;
//	(void) fprintf(stdout, "# = %d\n", BREAST_IMPLANT_TEMPLATE::ColorArray[index - 1].size());
#endif
#endif	// !defined
#endif	// 시뮬레이션

#if 0	// 체성분
	float*	values = __bodyElement.getValuesRef();
	// 파일또는 DB에서 측정값을 읽어 해당 값을 채운다.
#endif
}
#endif	// STANDARD_MODEL
#endif	// 2D 와의 통신을 위해 필요한 정의나 함수의 구현
	(void) fprintf(stdout, "...Done (%f)\n", __timer.GetElapsedTime());

#if 1	// 전체적으로 부드럽게
#if	defined(STANDARD_MODEL) && defined(MODEL_TYPE) && MODEL_TYPE == REAL_MODEL
	cout << "MakeVertexSmooth..." << endl;
	MakeVertexSmooth((1 + __statePtr->area2_control.SmoothIncLevel) * __statePtr->brush.flag.SmoothLevel);
	// kMesh::D3DVERTEX::p_ 를 새로 바뀐 kMesh::D3DVERTEX::p 로 설정
	kMesh::D3DXVERTEX*	pV;
	__vertexBufferPtr->Lock(0, 0, (void**) &pV, 0);
	for (DWORD prID = 0; prID < __numPointReprs; ++prID) {
		D3DXVERTEX&	v = pV[prID];
		v.p_ = v.p;
		__pointReprPtr[prID].flag.force = true;	// __whole_body_simulation_finalize 에서 모든 Vertex 를 변화하도록...
	}
	__vertexBufferPtr->Unlock();
	(void) __whole_body_simulation_finalize();
#endif
#endif	// 전체적으로 부드럽게

#if 1	// 체형변화 시뮬레이션
	for (int i = 0; i < (int) ((kState::VOLUME_MAX - kState::VOLUME_MIN) / (kState::VOLUME_STEP) + 1); ++i)
		__scale_cache_list.push_back(SCALE_CACHE());
#if 0	// Simulation Cache
	// 시간이 많이 걸리는 dV = -1 인 경우를 미리 계산한다.
{
	float	dV = -1.f;
	float	scale;
	if (simulateWholeBody1(false, dV, scale) < 0)
		cerr << "Simulation Cache: oops" << endl;
	else
		cout << "Simulation Cache: ok" << endl;
}
#endif	// Simulation Cache
#endif	// 체형변화 시뮬레이션

#if 1	// Timer
	(void) fprintf(stdout, "Total Elapsed Time in kMesh::setAll: %f\n", (float) (__timer.GetTime() - fTime0));
#endif
	return 0;
}
#endif

void
CLAMP2(
	D3DXVECTOR3&	c,
	FLOAT		lowerBound,
	FLOAT		upperBound
)
{
	FLOAT	fExcess = 0.f;
	if (c.z < lowerBound) {
		fExcess = lowerBound - c.z;
		c.z = lowerBound;
	}
	c.y -= fExcess;
	fExcess = 0.f;
	if (c.y < lowerBound) {
		fExcess = lowerBound - c.y;
		c.y = lowerBound;
	}
	c.x -= fExcess;
	if (c.x < lowerBound)
		c.x = lowerBound;

	fExcess = 0.f;
	if (c.x > upperBound) {
		fExcess = c.x - upperBound;
		c.x = upperBound;
	}
	c.y += fExcess;
	fExcess = 0.f;
	if (c.y > upperBound) {
		fExcess = c.y - upperBound;
		c.y = upperBound;
	}
	c.z += fExcess;
	if (c.z > upperBound)
		c.z = upperBound;
}

D3DVERTEXELEMENT9	kMesh::Element[N_ELEMENT + 1] = {
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
	{
		0,
		2 * sizeof(D3DXVECTOR3),
		D3DDECLTYPE_FLOAT2,
		D3DDECLMETHOD_DEFAULT,
		D3DDECLUSAGE_TEXCOORD,
		0
	},
	{	// c
		0,
		3 * sizeof(D3DXVECTOR3),
		D3DDECLTYPE_FLOAT3,
		D3DDECLMETHOD_DEFAULT,
		D3DDECLUSAGE_COLOR,
		0
	},
	// 임시 저장소
	{	// c_
		0,
		4 * sizeof(D3DXVECTOR3),
		D3DDECLTYPE_FLOAT3,
		D3DDECLMETHOD_DEFAULT,
		D3DDECLUSAGE_COLOR,
		1	// UsageIndex = 1
	},
	// 등고선 색깔
	{	// XY, YZ, ZX
		0,
		5 * sizeof(D3DXVECTOR3),
		D3DDECLTYPE_FLOAT3,
		D3DDECLMETHOD_DEFAULT,
		D3DDECLUSAGE_COLOR,
		2	// UsageIndex = 2
	},

	// nTmp
	{
		0,
		6 * sizeof(D3DXVECTOR3),
		D3DDECLTYPE_FLOAT3,
		D3DDECLMETHOD_DEFAULT,
		D3DDECLUSAGE_NORMAL,
		2	// UsageIndex = 2 (1은 나중에...)
	},

	// p_
	{
		0,
		7 * sizeof(D3DXVECTOR3),
		D3DDECLTYPE_FLOAT3,
		D3DDECLMETHOD_DEFAULT,
		D3DDECLUSAGE_POSITION,
		1	// UsageIndex = 1
	},
	// pTmp
	{
		0,
		8 * sizeof(D3DXVECTOR3),
		D3DDECLTYPE_FLOAT3,
		D3DDECLMETHOD_DEFAULT,
		D3DDECLUSAGE_POSITION,
		2	// UsageIndex = 2
	},

	// pQueue[3]
	{
		0,
		9 * sizeof(D3DXVECTOR3),
		D3DDECLTYPE_FLOAT3,
		D3DDECLMETHOD_DEFAULT,
		D3DDECLUSAGE_POSITION,
		3
	},
	{
		0,
		10 * sizeof(D3DXVECTOR3),
		D3DDECLTYPE_FLOAT3,
		D3DDECLMETHOD_DEFAULT,
		D3DDECLUSAGE_POSITION,
		4
	},
	{
		0,
		11 * sizeof(D3DXVECTOR3),
		D3DDECLTYPE_FLOAT3,
		D3DDECLMETHOD_DEFAULT,
		D3DDECLUSAGE_POSITION,
		5
	},
	// cQueue
	{
		0,
		12 * sizeof(D3DXVECTOR3),
		D3DDECLTYPE_FLOAT3,
		D3DDECLMETHOD_DEFAULT,
		D3DDECLUSAGE_COLOR,
		3
	},
	{
		0,
		13 * sizeof(D3DXVECTOR3),
		D3DDECLTYPE_FLOAT3,
		D3DDECLMETHOD_DEFAULT,
		D3DDECLUSAGE_COLOR,
		4
	},
	{
		0,
		14 * sizeof(D3DXVECTOR3),
		D3DDECLTYPE_FLOAT3,
		D3DDECLMETHOD_DEFAULT,
		D3DDECLUSAGE_COLOR,
		5
	},

	D3DDECL_END()
};

void
kMesh::__init()
{
#ifdef STANDARD_MODEL
	__index_to_standard = -1;
#endif	// STANDARD_MODEL
	__hWnd = NULL;

#if 1	// 색깔템플릿
	__indexTemplateColor = 0;
	__setTemplateColorDone = false;
#endif	// 색깔템플릿
#if 1	// ColorBand
	__colorBandVertexBufferPtr = 0;
	__indicatorLinePtr = 0;
#endif	// ColorBand
#ifdef __USE_A_OFFSET__
	pointer_to__a_vVertexID			= 0;
	pointer_to__a_vVertexID_a_offset	= 0;
	pointer_to__a_vEdgeID			= 0;
	pointer_to__a_vEdgeID_a_offset		= 0;
	pointer_to__a_vFaceIDLocalIndex		= 0;
	pointer_to__a_vFaceIDLocalIndex_a_offset	= 0;
	pointer_to__a_faceID			= 0;
	pointer_to__a_faceID_a_offset		= 0;
#endif	// __USE_A_OFFSET__
	__statePtr = 0;
	__devicePtr = NULL;

	__meshPtr0 = 0;
#if 1	// SetPolygon 에서 설정
	__adjacencyPtr = 0;
	__vertexID2pointReprIDMap = 0;
	__pointReprPtr = NULL;
	__edgePtr = NULL;
	__facePtr = NULL;
#endif
	__meshPtr = NULL;
	__effectPtr = NULL;
	__vertexBufferPtr = NULL;
	__vertexDeclarationPtr = NULL;

	__vertexBufferPtr = NULL;
	__edgeIndexBufferPtr = NULL;
	__faceIndexBufferPtr = NULL;

	__isometricDeclarationPtr = 0;
	__x_isometricVertexBufferPtr = 0;
	__y_isometricVertexBufferPtr = 0;
	__z_isometricVertexBufferPtr = 0;
	__x_isometricLines = 0;
	__y_isometricLines = 0;
	__z_isometricLines = 0;

	m__iHead = -1;
	m__iTail = -1;
	m__iCurrent = -1;
	m__uSize = 0;

	__fVolumeChange = 0.f;

#if 1	// String
	__segmentVertexBuffer = NULL;
	__numSegments = 0;
	__segmentIndexBuffer = NULL;
#if 1	// 2D단면
	__2dProjectNumPoints = 0;
	__2dProjectVertexBufferPtr = 0;
	__2dInterpolatedProjectNumPoints = 0;
	__2dInterpolatedProjectVertexBufferPtr = 0;
	__2dFillProjectNumPoints = 0;
	__2dFillProjectVertexBufferPtr = 0;
#endif
#endif

	__OnePointDeclarationPtr = 0;

	__intersectionVertexBufferPtr = 0;

	__objRHLoaded = false;
	__overlap = false;
#if 1	// manipulator
	__manipulatorType = UNDEF_MANIPULATOR;
	__manipulatorPtr = 0;
	__SegmentVertexDeclarationPtr = 0;
#endif	// manipulator
}

bool
kMesh::__effectInit(
	const LPCTSTR			effectSourceFilePtr,
#if 2	// Texture
	const LPDIRECT3DBASETEXTURE9	texturePtr
#endif
)
{
	bool	bOk = true;

do {
	HRESULT	hResult = D3D_OK;
	assert(__devicePtr);
	if (!__devicePtr) {
		bOk = false;
		break;
	}

	hResult = __devicePtr->CreateVertexDeclaration(kMesh::Element, &__vertexDeclarationPtr);
	assert(hResult == D3D_OK);
	hResult = __devicePtr->CreateVertexDeclaration(kMesh::__SegmentVertexElement, &__SegmentVertexDeclarationPtr);
	assert(hResult == D3D_OK);

	DWORD	Flags = 0;
	LPD3DXBUFFER	pCompilationErrors = NULL;
	hResult = D3DXCreateEffectFromFile(
		__devicePtr,		// LPDIRECT3DDEVICE9	devicePtr
		effectSourceFilePtr,	// LPCWSTR	effectSourceFilePtr
		NULL,			// const D3DXMACRO* pDefines
		NULL,			// LPD3DXINCLUDE pInclude
		Flags,			// DWORD Flags
		NULL,			// LPD3DXEFFECTPOOL pPool
		&__effectPtr,		// LPD3DXEFFECT* ppEffect
		&pCompilationErrors	// LPD3DXBUFFER* ppCompilationErrors
	);
	assert(hResult == D3D_OK);
	if (pCompilationErrors) {
		bOk = false;

//		LPCSTR	pErrorMessage = (LPCSTR) pCompilationErrors->GetBufferPointer();
		(void) pCompilationErrors->Release();	// ULONG
		pCompilationErrors = NULL;

		break;
	}
#if 2	// Texture
{
	HRESULT	hr = S_OK;
	// Set effect variables as needed
	D3DXCOLOR	colorMtrl(1.0f, 1.0f, 1.0f, 1.0f);
//	D3DXVECTOR3	vLightDir(0.1f, -1.0f, 0.1f);
	D3DXVECTOR3	vLightDir(0, -1, -1);	// 앞 위
//	D3DXVECTOR3	vLightDir(1, -1, 1);	// 등 좌측
//	D3DXVECTOR3	vLightDir(-1, -1, 1);	// 등 우측
	D3DXVec3Normalize(&vLightDir, &vLightDir);
//	D3DXCOLOR	vLightDiffuse(1, 1, 1, 1);
//	D3DXCOLOR	vLightDiffuse(1, 0, 0, 1);
	D3DXCOLOR	vLightDiffuse(169.f / 255, 169.f / 255, 169.f / 255, 1);	// dark grey

#if 1
	V(__effectPtr->SetValue("g_MaterialAmbientColor", &__statePtr->g__colorMtrl, sizeof(D3DXCOLOR)));
	V(__effectPtr->SetValue("g_MaterialDiffuseColor", &__statePtr->g__colorMtrl, sizeof(D3DXCOLOR)));
#else
	V(__effectPtr->SetValue("g_MaterialAmbientColor", &colorMtrl, sizeof(D3DXCOLOR)));
	V(__effectPtr->SetValue("g_MaterialDiffuseColor", &colorMtrl, sizeof(D3DXCOLOR)));
#endif

	V(__effectPtr->SetValue("g_LightDir", &vLightDir, sizeof(D3DXVECTOR3)));
	V(__effectPtr->SetValue("g_LightDiffuse", &vLightDiffuse, sizeof(D3DXVECTOR4)));

	V(__effectPtr->SetTexture("g_MeshTexture", texturePtr));
}
#endif
} while (0);

	return bOk;
}

DWORD	kMesh::MAX_SEGMENT	= 10000;

void
kMesh::__tapeMeasureInit()
{
#if 1	// String
	HRESULT	hResult;
	hResult = __devicePtr->CreateVertexBuffer(
		kMesh::MAX_SEGMENT * sizeof(SEGMENT_VERTEX),
		D3DUSAGE_WRITEONLY,
		0,
		D3DPOOL_MANAGED,
		&__segmentVertexBuffer,
		NULL
	);
	assert(D3D_OK == hResult);
	hResult = __devicePtr->CreateIndexBuffer(
		kMesh::MAX_SEGMENT * sizeof(CEdge::LineIndex),
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX32,
		D3DPOOL_MANAGED,
		&__segmentIndexBuffer,
		NULL
	);
	assert(D3D_OK == hResult);
#endif
}

unsigned	kMesh::MAX_SLICE	= 100;

#if 0	// 마지막 주문; 5. 측정선 선명하게...
	// kMesh::__shift_amount_along_normal 로 대체
FLOAT	kMesh::ShiftFactor	= .02f;
#endif
void
kMesh::__isometricInit()
{
	HRESULT	hResult;
	hResult = __devicePtr->CreateVertexDeclaration(__OnePointElement, &__isometricDeclarationPtr);

	hResult = __devicePtr->CreateVertexBuffer(
		(2 * MAX_SLICE * MAX_SEGMENT) * sizeof(D3DXVECTOR3),	// 한 라인당 2개의 포인트가 필요하다.
		D3DUSAGE_WRITEONLY,
		0,
		D3DPOOL_MANAGED,
		&__x_isometricVertexBufferPtr,
		NULL
	);
	assert(D3D_OK == hResult);
	hResult = __devicePtr->CreateVertexBuffer(
		(2 * MAX_SLICE * MAX_SEGMENT) * sizeof(D3DXVECTOR3),	// 한 라인당 2개의 포인트가 필요하다.
		D3DUSAGE_WRITEONLY,
		0,
		D3DPOOL_MANAGED,
		&__y_isometricVertexBufferPtr,
		NULL
	);
	assert(D3D_OK == hResult);
	hResult = __devicePtr->CreateVertexBuffer(
		(2 * MAX_SLICE * MAX_SEGMENT) * sizeof(D3DXVECTOR3),	// 한 라인당 2개의 포인트가 필요하다.
		D3DUSAGE_WRITEONLY,
		0,
		D3DPOOL_MANAGED,
		&__z_isometricVertexBufferPtr,
		NULL
	);
	assert(D3D_OK == hResult);
}

kMesh::~kMesh()
{
#if 1	// DestroyAll
	DestroyAll();
#else
	__Destroy();
#endif
}
void	kMesh::__Destroy()
{
#if 1	// ColorBand
	SAFE_RELEASE(__colorBandVertexBufferPtr);
	SAFE_RELEASE(__indicatorLinePtr);
#endif	// ColorBand
#if 1	// manipulator
	if (__manipulatorPtr) {
		__manipulatorPtr->reset();
		__manipulatorPtr = 0;
	}
	if (__SegmentVertexDeclarationPtr) {
		__SegmentVertexDeclarationPtr->Release();
		__SegmentVertexDeclarationPtr = NULL;
	}
#endif	// manipulator

#if 1	// SetPolygon 에서 설정
	if (__adjacencyPtr) {
		delete[] __adjacencyPtr;
		__adjacencyPtr = 0;
	}
	if (__vertexID2pointReprIDMap) {
		delete[] __vertexID2pointReprIDMap;
		__vertexID2pointReprIDMap = 0;
	}
	if (__pointReprPtr) {
		delete[] __pointReprPtr;
		__pointReprPtr = NULL;
	}
	if (__edgePtr) {
		delete[] __edgePtr;
		__edgePtr = NULL;
	}
	if (__facePtr) {
		delete[] __facePtr;
		__facePtr = NULL;
	}
#endif
	if (__meshPtr) {
		__meshPtr->Release();
		__meshPtr = NULL;
	}
	if (__effectPtr) {
#ifdef _DEBUG
		ULONG	refCount = __effectPtr->Release();
		assert(refCount == 0);
#else
		__effectPtr->Release();
#endif
		__effectPtr = NULL;
	}
	if (__vertexBufferPtr) {
#ifdef _DEBUG
		ULONG	refCount = __vertexBufferPtr->Release();
		assert(refCount == 0);
#else
		__vertexBufferPtr->Release();
#endif
		__vertexBufferPtr = NULL;
	}
	if (__edgeIndexBufferPtr) {
#ifdef _DEBUG
		ULONG	refCount = __edgeIndexBufferPtr->Release();
		assert(refCount == 0);
#else
		__edgeIndexBufferPtr->Release();
#endif
		__edgeIndexBufferPtr = NULL;
	}
	if (__faceIndexBufferPtr) {
#ifdef _DEBUG
		ULONG	refCount = __faceIndexBufferPtr->Release();
		assert(refCount == 0);
#else
		__faceIndexBufferPtr->Release();
#endif
		__faceIndexBufferPtr = NULL;
	}
	if (__vertexDeclarationPtr) {
#ifdef _DEBUG
		ULONG	refCount = __vertexDeclarationPtr->Release();
		assert(refCount == 0);
#else
		__vertexDeclarationPtr->Release();
#endif
		__vertexDeclarationPtr = NULL;
	}

#if 1	// isometric
	if (__isometricDeclarationPtr) {
		__isometricDeclarationPtr->Release();
		__isometricDeclarationPtr = 0;
	}
	if (__x_isometricVertexBufferPtr) {
		__x_isometricVertexBufferPtr->Release();
		__x_isometricVertexBufferPtr = 0;
	}
	if (__y_isometricVertexBufferPtr) {
		__y_isometricVertexBufferPtr->Release();
		__y_isometricVertexBufferPtr = 0;
	}
	if (__z_isometricVertexBufferPtr) {
		__z_isometricVertexBufferPtr->Release();
		__z_isometricVertexBufferPtr = 0;
	}
#endif

	if (this->__segmentVertexBuffer) {
#ifdef _DEBUG
		ULONG	refCount = this->__segmentVertexBuffer->Release();
		assert(refCount == 0);
#else
		this->__segmentVertexBuffer->Release();
#endif
		this->__segmentVertexBuffer = NULL;
	}
	if (this->__segmentIndexBuffer) {
#ifdef _DEBUG
		ULONG	refCount = this->__segmentIndexBuffer->Release();
		assert(refCount == 0);
#else
		this->__segmentIndexBuffer->Release();
#endif
		this->__segmentIndexBuffer = NULL;
	}
#if 1	// 2D단면
	SAFE_RELEASE(__2dProjectVertexBufferPtr);
	SAFE_RELEASE(__2dInterpolatedProjectVertexBufferPtr);
	SAFE_RELEASE(__2dFillProjectVertexBufferPtr);
#endif

	if (__OnePointDeclarationPtr) {
		__OnePointDeclarationPtr->Release();
		__OnePointDeclarationPtr = 0;
	}

	if (__intersectionVertexBufferPtr) {
		__intersectionVertexBufferPtr->Release();
		__intersectionVertexBufferPtr = 0;
	}

	for (MEASURE_PTR_HASH::iterator it = __measure_ptr_dic.begin(); it != __measure_ptr_dic.end(); ++it) {
		MEASURE*&	measure_ptr = it->second;	// SHOULD be reference!
		if (measure_ptr) {
			delete measure_ptr;
			measure_ptr = 0;
		}
	}
}

/*
 * 1. __vertexIDMappedPtr 는 주어진 Vertex ID 가 어느 Point Repr. ID 에 해당하는 지를 말해준다.
 * 이 정보는 ConvertAdjacencyToPointReps 로부터 얻어지므로 필요하지않다.
 * 2. "Vertex 의 갯수"는 일반적으로 "3 * Face 의 갯수"와 같지않다;
 * Face 를 구성하는 Vertex 의 갯수는, 주변의 Face 를 고려하지않으면, 3이다.
 * 하지만 많은 경우, 서로 다른 Face 가 Vertex 를 공유하기때문이다.
 * 여기서 이런 것들은 가려내는 일이 여기서 하는 일이다.
 * 3. GenerateAdjacency 에서 Epsilon 은,
 * 이 값보다 적은 거리에 있는 두 Vertex 를 하나의 Vertex (Point Repr.)로 간주할 때 사용되는 값이다.
 * 정확하려면 FLT_EPSILON(= 1.192092896e-07F)으로 설정하는 것이나 그렇게 하면 너무 엄격하므로
 * 좀 더 적은 값을 사용한다. 그렇다고 지금처럼 0.0001F 와 같은 절대값을 쓰기보다는
 * Vertex 들로 구성된 Mesh 의 전체적인 크기를 고려한 상대적인 값을 쓰는 게 더 낫다. (TO DO LIST)
 * 4. CPointRepr, CEdge 의 갯수는 최대 3 * dwNumFaces 이나 실제로는 그렇지 않다.
 * 더우기 CPointRepr 의 ID 의 경우 Point Repr. ID 를 사용하는 데 이는 연속적이지않다.
 * CEdge 의 ID 는 그나마 다행이다. 설정자체를 보면(CPolygon.cpp; FindEdge) 연속적으로 할당하기 때문이다.
 * 많은 참고하는 ID 의 경우 Point Repr. ID 를 사용하므로 이 또한 앞서 말한 불연속적이라는 문제가 있다.
 * 더 이상 원래의 Vertex ID 는 고려하지 않는다.
 *
 *	CPointRepr::id		// Point Repr. ID
 *	CPointRepr::vVertexID	// NOT USED
 *	CPointRepr::vEdgeID	// Ok
 *	CPointRepr::vFaceID	// Ok
 *
 *	CEdge::id		// Ok
 *	CEdge::prStartID	// Point Repr. ID
 *	CEdge::prEndID		// Point Repr. ID
 *	CEdge::faceID[2]	// Ok
 *
 *	CFace::id		// Ok
 *	CFace::vertexID0	// NOT USED
 *	CFace::vertexID1	// NOT USED
 *	CFace::vertexID2	// NOT USED
 *	CFace::prID0		// Point Repr. ID
 *	CFace::prID1		// Point Repr. ID
 *	CFace::prID2		// Point Repr. ID
 *	CFace::edgeID01		// Ok
 *	CFace::edgeID12		// Ok
 *	CFace::edgeID20		// Ok
 *	CFace::ajID01		// Ok
 *	CFace::ajID12		// Ok
 *	CFace::ajID20		// Ok
 *
 * ID 에 관한 거라면 CPointRepr 를 정리하고 거기에 맞춰, CEdge::prStartID, CEdge::prEndID 와
 * CFace::prID0, CFace::prID1, CFace::prID2 를 정리하면 된다.
 * CPointRepr 의 ID 를 정리하는 것은 새로운 Vertex ID 를 만드는 것이다.
 * 따라서 CEdge 의 prStartID, prEndID 를 거기에 맞추는 것은 CEdge::LineIndex 를 만드는 것이고,
 * CFace 의 prID0, prID1, prID2 의 경우 CFace::FaceIndex 를 만드는 것이다.
 * 즉 Index Buffer 의 Index 만드는 것과 같은 일이다.
 * 이렇게 되면 kMesh::m__pPointReprIDMapped 가 더 이상 필요하지않다!
 */
int
kMesh::SetPolygon(const LPD3DXMESH meshPtr)
{
	if (__adjacencyPtr) {
		delete[] __adjacencyPtr;
		__adjacencyPtr = 0;
	}
	if (__vertexID2pointReprIDMap) {
		delete[] __vertexID2pointReprIDMap;
		__vertexID2pointReprIDMap = 0;
	}
	if (__facePtr) {
		delete[] __facePtr;
		__facePtr = 0;
	}
	if (__edgePtr) {
		delete[] __edgePtr;
		__edgePtr = 0;
	}
	if (__pointReprPtr) {
		delete[] __pointReprPtr;
		__pointReprPtr = 0;
	}

	DWORD	dwNumFaces = meshPtr->GetNumFaces();
//	DWORD	numVertices = meshPtr->GetNumVertices();
	CPointRepr*	pPointRepr = new CPointRepr[3 * dwNumFaces];	// Unique 한 Vertex 의 갯수는 최대 3 * dwNumFaces 이다.
	CEdge*	pEdge = new CEdge[3 * dwNumFaces];		// Unique 한 Edge 의 갯수는 최대 3 * dwNumFaces 이다.
	__facePtr = new CFace[dwNumFaces];			// Unique 한 Face 의 갯수는 정확히 dwNumFaces 이다.

	__adjacencyPtr = new DWORD[3 * dwNumFaces];
	__vertexID2pointReprIDMap = new DWORD[3 * dwNumFaces];		// 최대 3 * dwNumFaces 이다.
	FillMemory(__vertexID2pointReprIDMap, 3 * dwNumFaces * sizeof(DWORD), 0xff);	// (DWORD) -1 로 초기화
	meshPtr->GenerateAdjacency(1.0e-6f, __adjacencyPtr);	// D3D_OK
	meshPtr->ConvertAdjacencyToPointReps(__adjacencyPtr, __vertexID2pointReprIDMap);	// D3D_OK

	LPDIRECT3DVERTEXBUFFER9	pVB;
	LPDIRECT3DINDEXBUFFER9	pIB;
	meshPtr->GetVertexBuffer(&pVB);	// D3D_OK
	meshPtr->GetIndexBuffer(&pIB);	// D3D_OK

	bool		b32BitIndex = meshPtr->GetOptions() & D3DXMESH_32BIT;
	BYTE*		pIndices;
	CPointRepr::VERTEX*	pVertices;
	pIB->Lock(0, 0, (void**) &pIndices, 0);	// D3D_OK
	pVB->Lock(0, 0, (void**) &pVertices, 0);	// D3D_OK

#ifndef __USE_A_OFFSET__
	DWORD	nextEdgeID = (DWORD) 0;
#endif	// !__USE_A_OFFSET__
	for (DWORD faceID = 0; faceID < dwNumFaces; ++faceID) {
		// CFace 정보 설정
		CFace&	face = __facePtr[faceID];
		face.id = faceID;					// CFace::id
		face.ajID01 = __adjacencyPtr[3 * faceID + 0];		// CFace::ajID01
		face.ajID12 = __adjacencyPtr[3 * faceID + 1];		// CFace::ajID12
		face.ajID20 = __adjacencyPtr[3 * faceID + 2];		// CFace::ajID20

		// CPointRepr 정보 설정
		DWORD	prID0 = (DWORD) -1, prID1 = (DWORD) -1, prID2 = (DWORD) -1;
		// 이 Face 를 구성하는 세 개의 Vertex ID 는...
		for (int i = 0; i < 3; ++i) {
			DWORD	vertexID;
			if (b32BitIndex)
				vertexID = ((DWORD*) pIndices)[3 * faceID + i];
			else
				vertexID = (DWORD) (((WORD*) pIndices)[3 * faceID + i]);
			// 이것에 해당하는 Point Representative 는...
			DWORD	prID = __vertexID2pointReprIDMap[vertexID];
			CPointRepr&	pointRepr = pPointRepr[prID];
			if (pointRepr.id == (DWORD) -1) {
				pointRepr.id = prID;			// CPointRepr::id
#if 0	// SetPolygon 을 호출한 후에 CPointRepr::vVertexID 와 meshPtr 의 Vertex Buffer 를 이용하여 설정
#if 1	// tiny.x; Up 방향 = z 축, RHS (정면 = y축)
				CPointRepr::VERTEX	v = pVertices[vertexID];

				// Up 방향 = z축; x축에 대해서 90도 회전
#if 1	// kMeshLoader::meshFromOBJ (woman.obj)
				v.position *= __fScale;
#else	// tiny.x
				float	f = v.p.y;
				v.p.y = v.p.z;
				v.p.z = -f;
#endif

				pointRepr.v = v;
#else
				pointRepr.v = pVertices[vertexID];	// CPointRepr::v
									// Bulk Copy
#endif
#endif
			}
#ifndef __USE_A_OFFSET__
#if 1
			FACE_ID_LOCAL_INDEX	faceIDLocalIndex;
			faceIDLocalIndex.Set(faceID, i);
			pointRepr.vFaceIdLocalIndex.push_back(faceIDLocalIndex);
#else
			pointRepr.vFaceIdLocalIndex.push_back(FACE_ID_LOCAL_INDEX(faceID, i));
#endif
#if 1
			// Vertex 가 서로 다른 Triangle 에서 사용되므로
			// 같은 vertexID 가 들어갈 수 있다;
			// std::vector 대신에 std::set 을 사용해야 한다.
			pointRepr.vVertexID.insert(vertexID);
#else
			pointRepr.vVertexID.push_back(vertexID);	// CPointRepr::vVertexID
#endif
#endif

			if (i == 0) {
#if 0
				face.vertexID0 = vertexID;		// CFace::vertexID0
#endif
				face.prID0 = prID;			// CFace::prID0
				prID0 = prID;
			} else if (i == 1) {
#if 0
				face.vertexID1 = vertexID;		// CFace::vertexID1
#endif
				face.prID1 = prID;			// CFace::prID1
				prID1 = prID;
			} else {	// i = 2
#if 0
				face.vertexID2 = vertexID;		// CFace::vertexID2
#endif
				face.prID2 = prID;			// CFace::prID2
				prID2 = prID;
			}
///			DWORD&	prIDMapped = __vertexIDMappedPtr[vertexID];
///			if (prIDMapped != (DWORD) -1) {
///				assert(prIDMapped == prID);
///			} else
///				prIDMapped = prID;
		}

		// CEdge 정보 설정
		// (prID0, prID1) 는 어떤 Edge 에 속하는가?
		// (prID1, prID2), (prID2, prID0) 은?
		for (int i = 0; i < 3; ++i) {
			DWORD	prStartID, prEndID;
			EDGE*	pEdgeID;
			if (i == 0) {
				prStartID = prID0;
				prEndID = prID1;
				pEdgeID = &face.edgeID01;		// CFace::edgeID01
			} else if (i == 1) {
				prStartID = prID1;
				prEndID = prID2;
				pEdgeID = &face.edgeID12;		// CFace::edgeID12
			} else {	// i = 2
				prStartID = prID2;
				prEndID = prID0;
				pEdgeID = &face.edgeID20;		// CFace::edgeID20
			}

// FindEdge 의 실행시간은 O(n^2) 에 비례한다; n = 500,000 이라면...
//	주어진 (prStartID, prEndID)에 해당하는 Edge 가 이미 등록되었는 지 확인하는 과정에서 시간이 많이 걸린다.
//	등록하는 과정을 좀 더 들여다보면 실행시간을 O(n) 으로 줄일 수 있다;
//	(prStartID, prEndID)가 만드는 새로운 Edge 를 만들 때마다
//	각 Vertex 의 vEdgeID 에 이 Edge 를 등록한다;
//	정확히 말하면 Edge 의 방향까지 말해주는 EDGE 구조를 등록한다:
//	prEndID 에 해당하는 Vertex 에서는 방향(bDir)을 반대로 한다.
//
//	(prStartID, prEndID) 에 해당하는 Edge 가 이미 등록되었는 지 다음과 같이 확인한다;
//	prStartID 를 기준으로 했지만 prEndID 를 기준으로 해도 마찬가지 결과를 준다.
//	prStartID 에 등록된 EDGE 중에서 bDir 이 false 인 Edge 를 찾는다. (왜? 이유는 아래에서...)
//	이 중에서 (prEndID -> prStartID) 로 등록된 EDGE 가 있는 지 확인하면 된다.
//
//	이러한 Edge 를 찾을 수 없다면 (prStartID, prEndID) 는 새로운 Edge 를 구성한다.
#if 1
			int	index = -1;
#ifndef __USE_A_OFFSET__
{
	std::vector<EDGE>&	vEdgeID = pPointRepr[prStartID].vEdgeID;
	index = -1;
	for (int n = 0; n < (int) vEdgeID.size(); ++n) {
		EDGE&	edge = vEdgeID.at(n);
		if (edge.bDir) {	// bDir = true 일 수는 없다!
					// (prStartID, prEndID) 에 해당하는 Edge 가
					// 있는 지를 확인하는 게 이 루틴의 목적이다.
					// bDir = true 라면 prStartID 가 시작이 되는 Edge 란 말인데
					// 이러한 Edge 가 끝점으로 prEndID 를 가질 수가 없다.
					// 왜냐하면 그랬다면 같은 (prStartID, prEndID) 를 다시 테스트한다는 말이고,
					// 이는 정상적이라면 그럴 수 없기 때문이다;
					// 서로 다른 face 에 대해서 loop 을 도는데, 서로 다른 face 인데
					// 같은 (prStartID, prEndID) 가 나올 수 없다.
			continue;
		}

		DWORD	id = edge.id;
		CEdge&	e = pEdge[id];
		if (e.prEndID == prStartID && e.prStartID == prEndID) {
			index = id;
			break;
		}
	}
	if (index == -1) {
		index = nextEdgeID++;
	}
}
#endif
#else
			int	index = FindEdge(prStartID, prEndID, pEdge, 3 * dwNumFaces);
#endif
			assert(index != -1);
			CEdge&	edge = pEdge[index];
			if (edge.id == (DWORD) -1) {	// 새로운 Edge 를 구성한다.
				edge.id = (DWORD) index;		// CEdge::id
				edge.prStartID = prStartID;		// CEdge::prStartID
				edge.prEndID = prEndID;			// CEdge::prEndID
#if 1	// FACE_ID_ARRAY
#ifndef __USE_A_OFFSET__
				edge.faceID.push_back(faceID);
#endif
#else
				edge.faceID[0] = faceID;		// CEdge::faceID[0]
				edge.faceID[1] = (DWORD) -1;		// CEdge::faceID[1]
#endif
#if 1
				EDGE	E1 = {index, true};
#ifndef __USE_A_OFFSET__
				EDGE	E2 = {index, false};
#endif	// !__USE_A_OFFSET__
				*pEdgeID = E1;
#else
				*pEdgeID = EDGE(index, true);
#endif

#ifndef __USE_A_OFFSET__
#if 1
				pPointRepr[prStartID].vEdgeID.push_back(E1);	// CPointRepr::vEdgeID
				pPointRepr[prEndID].vEdgeID.push_back(E2);	// CPointRepr::vEdgeID
#else
				pPointRepr[prStartID].vEdgeID.push_back(EDGE(index, true));	// CPointRepr::vEdgeID
				pPointRepr[prEndID].vEdgeID.push_back(EDGE(index, false));	// CPointRepr::vEdgeID
#endif
#endif
			} else {	// 기존의 Edge 와 같다. 단, 방향은 반대!
#if 1	// FACE_ID_ARRAY
#ifndef __USE_A_OFFSET__
				edge.faceID.push_back(faceID);
#endif
#else
				assert(edge.faceID[0] != (DWORD) -1);
				assert(edge.faceID[1] == (DWORD) -1);
				if (edge.faceID[1] != (DWORD) -1) {
					int	i = 0;
				}
				edge.faceID[1] = faceID;		// CEdge::faceID[1]
#endif
#if 1
				EDGE	E1 = {index, false};
				*pEdgeID = E1;
#else
				*pEdgeID = EDGE(index, false);
#endif
			}
		}
	}
#if 0
#ifdef _DEBUG
{
	struct USED_INDEX {
		DWORD	startIndex;	// 이 index 부터 시작해서
		DWORD	nIndex;		// n 개의 index 가 사용되었다.
	};
	// 총 dwNumVertices 개 중에서 진짜로 사용된 것은?
	// 그리고 그것들이 연속적으로 사용되었을까?
	std::vector<struct USED_INDEX>	used_index;
	DWORD	startIndex = (DWORD) -1;
	DWORD	nIndex = 0;
	for (DWORD i = 0; i < dwNumVertices; ++i) {
		DWORD&	prID = __pointReprPtr[i].id;
		if (prID == (DWORD) -1) {
			if (startIndex != (DWORD) -1) {
				struct USED_INDEX	ui;
				ui.startIndex = startIndex;
				ui.nIndex = nIndex;
				used_index.push_back(ui);

				startIndex = (DWORD) -1;
				nIndex = 0;
			}
			continue;
		}

		if (startIndex == (DWORD) -1)
			startIndex = i;
		++nIndex;
	}
	if (startIndex != (DWORD) -1) {
		struct USED_INDEX	ui;
		ui.startIndex = startIndex;
		ui.nIndex = nIndex;
		used_index.push_back(ui);

		startIndex = (DWORD) -1;
		nIndex = 0;
	}
	size_t	n = used_index.size();
	size_t	w = 0;
	for (size_t i = 0; i < used_index.size(); ++i) {
		struct USED_INDEX	ui = used_index.at(i);
		startIndex = ui.startIndex;
		nIndex = ui.nIndex;
		w += nIndex;
		int	iBreakPoint = 0;
	}
	int	iBreakPoint = 0;
}
#endif	// _DEBUG
#endif

	pVB->Unlock();	// D3D_OK
	pIB->Unlock();	// D3D_OK
	if (pVB) {
#ifdef _DEBUG
		ULONG	refCount = pVB->Release();
		assert(refCount == 1);
#else
		pVB->Release();
#endif
		pVB = NULL;
	}
	if (pIB) {
#ifdef _DEBUG
		ULONG	refCount = pIB->Release();
		assert(refCount == 1);
#else
		pIB->Release();
#endif
		pIB = NULL;
	}


	// 몇 개의 Uniqe 한 Point Repr. ID 가 있는가?
	DWORD	nPointRepr = 0;
	// Point Repr. ID 가 새로운 Vertex ID 로 맵핑된다.
	DWORD*	pPointReprIDRemap = new DWORD[3 * dwNumFaces];
do {
	// __vertexID2pointReprIDMap 의 값은 연속적이지 않다!
	//	0 0 3 3 4 4...	; 1, 2 가 없을 수도 있다.
	// 이를 연속적인 숫자로 바꾼다.
	//	0 0 1 1 2 2	; 즉 3->1 로 4->2 로 바뀐다.
	DWORD*	pointReprRemapPtr = 0;
do {
	pointReprRemapPtr = new DWORD[3 * dwNumFaces];
	FillMemory(pointReprRemapPtr, 3 * dwNumFaces * sizeof(DWORD), 0xff);
	for (DWORD i = 0; i < 3 * dwNumFaces; ++i) {
		DWORD	prID = pPointRepr[i].id;
		pPointReprIDRemap[i] = prID == (DWORD) -1 ? (DWORD) -1 : nPointRepr++;
		if (prID != (DWORD) -1)
			pointReprRemapPtr[prID] = pPointReprIDRemap[i];
	}
	__numPointReprs = nPointRepr;
	__pointReprPtr = new CPointRepr[nPointRepr];

	for (DWORD i = 0; i < 3 * dwNumFaces; ++i) {
		DWORD&	prID = pPointRepr[i].id;
		if (prID == (DWORD) -1)
			continue;

		DWORD	newVertexID = pPointReprIDRemap[prID];
		__pointReprPtr[newVertexID] = pPointRepr[i];
		__pointReprPtr[newVertexID].id = newVertexID;
//		CPointRepr&	pointReprTo = __pointReprPtr[newVertexID];
//		CPointRepr&	pointReprFrom = pPointRepr[i];
//		pointReprTo.id = newVertexID;
//		pointReprTo.v = pointReprFrom.v;
	}
	for (DWORD i = 0; i < 3 * dwNumFaces; ++i) {
#if 1
		DWORD&	prID = __vertexID2pointReprIDMap[i];
		if (prID != (DWORD) -1)
			prID = pointReprRemapPtr[prID];
#endif
	}
} while (0);
	if (pointReprRemapPtr) {
		delete[] pointReprRemapPtr;
		pointReprRemapPtr = 0;
	}
} while (0);

	// CFace::prID0, prID1, prID2 을 새로운 Vertex ID 로 바꾼다: CFace::FaceIndex
	__numFaces = dwNumFaces;
	for (DWORD i = 0; i < dwNumFaces; ++i) {
		CFace&	face = __facePtr[i];

		DWORD&	prID0 = face.prID0;
		assert(prID0 != (DWORD) -1 && prID0 < 3 * dwNumFaces);
		prID0 = pPointReprIDRemap[prID0];

		DWORD&	prID1 = face.prID1;
		assert(prID1 != (DWORD) -1 && prID1 < 3 * dwNumFaces);
		prID1 = pPointReprIDRemap[prID1];

		DWORD&	prID2 = face.prID2;
		assert(prID2 != (DWORD) -1 && prID2 < 3 * dwNumFaces);
		prID2 = pPointReprIDRemap[prID2];
	}

	// 몇 개의 Unique 한 Edge 가 있는가?
	unsigned	nEdge = 0;
	for (DWORD i = 0; i < 3 * dwNumFaces; ++i) {
		if (pEdge[i].id != (DWORD) -1)
			++nEdge;
	}
	__numEdges = nEdge;
	__edgePtr = new CEdge[nEdge];
	for (DWORD i = 0; i < nEdge; ++i) {
		__edgePtr[i] = pEdge[i];
	}

	// CEdge::prStartID, CEdge::prEndID 를 새로운 Vertex ID 로 바꾼다: CEdge::LineIndex
	for (DWORD i = 0; i < nEdge; ++i) {
		CEdge&	edge = __edgePtr[i];


		DWORD&	prStartID = edge.prStartID;
		assert(prStartID != (DWORD) -1 && prStartID < 3 * dwNumFaces);
		prStartID = pPointReprIDRemap[prStartID];

		DWORD&	prEndID = edge.prEndID;
		assert(prEndID != (DWORD) -1 && prEndID < 3 * dwNumFaces);
		prEndID = pPointReprIDRemap[prEndID];
	}
#if 0	// FACE_ID_ARRAY
	// 양쪽에 두 개의 삼각형을 갖지 않은 Edge 목록을 출력; 메쉬 디버깅용
if (1) {
	FILE*	filePtr = 0;
#if 1	// Console
	filePtr = stdout;
#else
	char	filename[] = "C:/pick";
#if defined(_MSC_VER) && (_MSC_VER >= 1400)	// Visual Studio 2005 or above
	errno_t	err = fopen_s(&filePtr, filename, "w");
	assert(err == 0);
	if (err != 0) {
		return -1;
	}
#else
	filePtr = fopen(filename, "w");
#endif
#endif
	(void) fprintf(filePtr, "Edge ID\tprStartID\tprEndID\tFace ID...\n");
	for (DWORD n = 0; n < nEdge; ++n) {
		CEdge&	edge = __edgePtr[n];
		std::vector<DWORD>&	faceIDArray = edge.faceID;
		if (edge.id != (DWORD) -1) {
			if (faceIDArray.size() == 1) {
#if 0	// group
				const kMeshLoader::FID2GID_HASH&	__fid2gidMap = __meshLoader.fid2gidMap();
				kMeshLoader::FID2GID_HASH::const_iterator		fid2gid_it;
				fid2gid_it = __fid2gidMap.find(faceIDArray[0]);
				if (fid2gid_it != __fid2gidMap.end()) {
					const kMeshLoader::GID_NAME_HASH&	__gidMap = __meshLoader.gidMap();
					kMeshLoader::GID_NAME_HASH::const_iterator	gid_it;
					gid_it = __gidMap.find(fid2gid_it->second);
					if (gid_it != __gidMap.end()) {
						(void) fprintf(filePtr, "%d\t%d\t%d\t%d (|%s|)\n", edge.id, edge.prStartID, edge.prEndID, faceIDArray[0], gid_it->second.c_str());
					} else {
						(void) fprintf(filePtr, "%d\t%d\t%d\t%d (no gid_it)\n", edge.id, edge.prStartID, edge.prEndID, faceIDArray[0]);
					}
				} else {
					(void) fprintf(filePtr, "%d\t%d\t%d\t%d (no fid2gid_it)\n", edge.id, edge.prStartID, edge.prEndID, faceIDArray[0]);
				}
#else
				(void) fprintf(filePtr, "%d\t%d\t%d\t%d\n", edge.id, edge.prStartID, edge.prEndID, faceIDArray[0]);
#endif
			} else if (faceIDArray.size() > 2) {
				(void) fprintf(filePtr, "%d\t%d\t%d", edge.id, edge.prStartID, edge.prEndID);
				for (size_t k = 0; k < faceIDArray.size(); ++k) {
					(void) fprintf(filePtr, "\t%d", faceIDArray.at(k));
//					CFace&	face = __facePtr[faceIDArray.at(k)];
//					(void) fprintf(filePtr, " (%d, %d, %d)", face.prID0, face.prID1, face.prID2);
				}
				(void) fprintf(filePtr, "\n");
			}
		}
	}
#if 1	// Console
	(void) fflush(stdout);
#else
	if (filePtr) {
		fclose(filePtr);
		filePtr = 0;
	}
#endif
}
#endif

	if (pPointReprIDRemap) {
		delete[] pPointReprIDRemap;
		pPointReprIDRemap = NULL;
	}

	if (pEdge) {
		delete[] pEdge;
		pEdge = NULL;
	}

	if (pPointRepr) {
		delete[] pPointRepr;
		pPointRepr = NULL;
	}

	return 0;
}

int	kMesh::GetFaceIDLocalIndex(DWORD prID, /* [OUT] */ FACE_ID_LOCAL_INDEX& faceIDLocalIndex) const
{
	if (prID >= __numPointReprs) {
		(void) fprintf(stderr, "ERROR: kMesh::GetFaceIDLocalIndex: prID = %d: Out of range\n", prID);
		return -1;
	}

	// 여러 개가 있을 수 있으나 이를 사용해 좌표를 얻는 데 목적이 있으므로
	// 같은 Point Repr. ID 를 주었으므로 어떤 것을 사용해도 상관없다.
#ifdef __USE_A_OFFSET__
	int	n = pointer_to__a_vFaceIDLocalIndex_a_offset[prID + 1] - pointer_to__a_vFaceIDLocalIndex_a_offset[prID];
	assert(n);
	if (n == 0) {
		(void) fprintf(stderr, "FATAL: kMesh::GetFaceIDLocalIndex: prID = %d: No FACE_ID_LOCAL_INDEX\n", prID);
		return -1;
	}
	faceIDLocalIndex = pointer_to__a_vFaceIDLocalIndex[pointer_to__a_vFaceIDLocalIndex_a_offset[prID]];
#else
	assert(__pointReprPtr[prID].vFaceIdLocalIndex.size());
	if (__pointReprPtr[prID].vFaceIdLocalIndex.size() == 0) {
		(void) fprintf(stderr, "FATAL: kMesh::GetFaceIDLocalIndex: prID = %d: No FACE_ID_LOCAL_INDEX\n", prID);
		return -1;
	}
	faceIDLocalIndex = __pointReprPtr[prID].vFaceIdLocalIndex.at(0);
#endif
	return 0;
}

DWORD	kMesh::GetPrID(const FACE_ID_LOCAL_INDEX& faceIDLocalIndex) const
{
	DWORD	ID = faceIDLocalIndex.ID;
	if (ID >= __numFaces) {
		(void) fprintf(stderr, "ERROR: kMesh::GetPrID: Face ID = %d: Out of range\n", ID);
		return (DWORD) -1;
	}
	return __facePtr[ID].prID[faceIDLocalIndex.local_index];
}


int
kMesh::SetMesh()
{
	HRESULT	hResult;
#if 0
{
D3DVERTEXELEMENT9	element[2] = {
	{
		0,
		0 * sizeof(D3DXVECTOR3),
		D3DDECLTYPE_FLOAT3,
		D3DDECLMETHOD_DEFAULT,
		D3DDECLUSAGE_POSITION,
		0
	},
#if 0
	{
		0,
		1 * sizeof(D3DXVECTOR3),
		D3DDECLTYPE_FLOAT3,
		D3DDECLMETHOD_DEFAULT,
		D3DDECLUSAGE_NORMAL,
		0
	},
	{
		0,
		2 * sizeof(D3DXVECTOR3),
		D3DDECLTYPE_FLOAT3,
		D3DDECLMETHOD_DEFAULT,
		D3DDECLUSAGE_COLOR,
		0
	},
#endif

	D3DDECL_END()
};
	LPD3DXMESH	qMesh = NULL;
	assert(D3D_OK == meshPtr->CloneMesh(
		meshPtr->GetOptions(),
		element,
		__devicePtr,
		&qMesh
	));
	DWORD	dwFormat = D3DXF_FILEFORMAT_TEXT;	// D3DXF_FILEFORMAT_BINARY
	assert(D3D_OK == D3DXSaveMeshToX(
		TEXT("xxx.x"),
		qMesh,
		NULL,
		NULL,
		NULL,
		0,
		dwFormat
	));
	if (qMesh) {
		qMesh->Release();
		qMesh = NULL;
	}
}
#endif
#if 0
	DWORD	dwNumVertices = meshPtr->GetNumVertices();
	DWORD	dwNumFaces = meshPtr->GetNumFaces();
#endif

#if 0
	// Face 와 Edge 를 위한 Vertex Buffer 를 만든다.
	// Q. 몇 개의 Point Representative 가 있는가?
	unsigned	nPointRepr = 0;
	for (DWORD i = 0; i < dwNumVertices; ++i) {
		if (__pointReprPtr[i].id != (DWORD) -1)
			++nPointRepr;
	}
	// A. 총 nPointRepr 개가 있다.
#if 0
#if 0
nPointRepr = 8;
#else
#if 0
nPointRepr = 8 + 1;
#else
nPointRepr = 8 + 3;
#endif
#endif
#endif
	__numPointReprs = nPointRepr;
#else
	unsigned	nPointRepr = __numPointReprs;
#endif
	__devicePtr->CreateVertexBuffer(
		nPointRepr * sizeof(kMesh::D3DXVERTEX),
		D3DUSAGE_WRITEONLY,
		0,
		D3DPOOL_MANAGED,
		&__vertexBufferPtr,
		NULL
	);
	kMesh::D3DXVERTEX*	pVertices;
	__vertexBufferPtr->Lock(0, 0, (void**) &pVertices, 0);
#if 1
	for (DWORD iVertex = 0; iVertex < nPointRepr; ++iVertex) {	// newVertexID
		CPointRepr&	pointRepr = __pointReprPtr[iVertex];
		pVertices[iVertex].p = pointRepr.v.position;
		pVertices[iVertex].p_ = pointRepr.v.position;
		pVertices[iVertex].n = pointRepr.v.normal;
		pVertices[iVertex].tu = pointRepr.v.texcoord.x;
		pVertices[iVertex].tv = pointRepr.v.texcoord.y;
		pVertices[iVertex].tx = -1.f;
#if 1
		pVertices[iVertex].c.x = __statePtr->g__colorMtrl.r;
		pVertices[iVertex].c.y = __statePtr->g__colorMtrl.g;
		pVertices[iVertex].c.z = __statePtr->g__colorMtrl.b;
#else
//		pVertices[iVertex].c = D3DXVECTOR3(0.f, 0.f, (FLOAT) 139 / 255.);	// dark blue
		pVertices[iVertex].c = D3DXVECTOR3(169.f / 255, 169.f / 255, 169.f / 255);	// dark grey
#endif
	}
#else
	// Point Representative ID 가 Vertex Buffer 의 몇 번째에 mapping 되었는 지를 기억한다.
	// 왜냐하면 CEdge 나 CFace 에 있는 정보는 모두 Point Representative ID 를 기준으로 하고 있기때문이다.
	DWORD	iPointRepr = 0;
	for (DWORD i = 0; i < dwNumVertices; ++i) {
		CPointRepr&	pointRepr = __pointReprPtr[i];
		if (pointRepr.id == (DWORD) -1)
			continue;

		pVertices[iPointRepr].p = pointRepr.v.p;
		pVertices[iPointRepr].p_ = pointRepr.v.p;
		pVertices[iPointRepr].n = pointRepr.v.n;
		pVertices[iPointRepr].tu = 0.f;
		pVertices[iPointRepr].tv = 0.f;
		pVertices[iPointRepr].tx = -1.f;
		pVertices[iPointRepr].c = D3DXVECTOR3(0.f, 0.f, 0.f);
		++iPointRepr;
	}
#if 0
pVertices[0].p = D3DXVECTOR3(0.f, 0.f, 1.f);
pVertices[1].p = D3DXVECTOR3(1.f, 0.f, 1.f);
pVertices[2].p = D3DXVECTOR3(1.f, 1.f, 1.f);
pVertices[3].p = D3DXVECTOR3(0.f, 1.f, 1.f);
pVertices[4].p = D3DXVECTOR3(0.f, 0.f, 0.f);
pVertices[5].p = D3DXVECTOR3(1.f, 0.f, 0.f);
pVertices[6].p = D3DXVECTOR3(1.f, 1.f, 0.f);
pVertices[7].p = D3DXVECTOR3(0.f, 1.f, 0.f);
#if 0
#else
#if 0
pVertices[8].p = D3DXVECTOR3(0.3333333f, 0.3333333f, 0.f);
#else
pVertices[8].p = D3DXVECTOR3(.5f, 0.f, 0.f);
pVertices[9].p = D3DXVECTOR3(.5f, .5f, 0.f);
pVertices[10].p = D3DXVECTOR3(0.f, .5f, 0.f);
#endif
#endif
#endif
#endif
	__vertexBufferPtr->Unlock();

#if 0
	// Q. 총 몇 개의 Edge 가 있는가?
	unsigned	nEdge = 0;
	for (DWORD i = 0; i < 3 * dwNumFaces; ++i) {
		if (__edgePtr[i].id != (DWORD) -1)
			++nEdge;
	}
	// A. 총 nEdge 개의 Edge 가 있다.
#if 0
#if 0
nEdge = 18;
#else
#if 0
nEdge = 18 + 3;
#else
nEdge = 18 + 6;
#endif
#endif
#endif
	__numEdges = nEdge;
#else
	unsigned	nEdge = __numEdges;
#endif
	__devicePtr->CreateIndexBuffer(
		nEdge * sizeof(CEdge::LineIndex),
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX32,
		D3DPOOL_MANAGED,
		&__edgeIndexBufferPtr,
		NULL
	);
	CEdge::LineIndex*	pLineIndices;
	__edgeIndexBufferPtr->Lock(0, 0, (void**) &pLineIndices, 0);
#if 1
	for (DWORD iEdge = 0; iEdge < nEdge; ++iEdge) {
		CEdge&	edge = __edgePtr[iEdge];
		assert(iEdge == edge.id);

		DWORD	iStart = edge.prStartID;
		iStart *= N_ELEMENT;
		DWORD	iEnd = edge.prEndID;
		iEnd *= N_ELEMENT;
		pLineIndices[iEdge].iStart = iStart;
		pLineIndices[iEdge].iEnd = iEnd;
	}
#else
	DWORD	iEdge = 0;
	for (DWORD i = 0; i < 3 * dwNumFaces; ++i) {
		CEdge&	edge = __edgePtr[i];
		if (edge.id == (DWORD) -1)
			continue;

		DWORD	iStart = edge.prStartID;
		iStart = this->m__pPointReprIDMapped[iStart];
		assert(iStart != (DWORD) -1);
		iStart *= N_ELEMENT;
		DWORD	iEnd = edge.prEndID;
		iEnd = this->m__pPointReprIDMapped[iEnd];
		assert(iEnd != (DWORD) -1);
		iEnd *= N_ELEMENT;
		pLineIndices[iEdge].iStart = iStart;
		pLineIndices[iEdge].iEnd = iEnd;
		++iEdge;
	}
#if 0
pLineIndices[0].iStart = N_ELEMENT * 0;
pLineIndices[0].iEnd = N_ELEMENT * 1;
pLineIndices[1].iStart = N_ELEMENT * 1;
pLineIndices[1].iEnd = N_ELEMENT * 2;
pLineIndices[2].iStart = N_ELEMENT * 2;
pLineIndices[2].iEnd = N_ELEMENT * 3;
pLineIndices[3].iStart = N_ELEMENT * 3;
pLineIndices[3].iEnd = N_ELEMENT * 0;
pLineIndices[4].iStart = N_ELEMENT * 1;
pLineIndices[4].iEnd = N_ELEMENT * 3;

pLineIndices[5].iStart = N_ELEMENT * 2;
pLineIndices[5].iEnd = N_ELEMENT * 6;
pLineIndices[6].iStart = N_ELEMENT * 6;
pLineIndices[6].iEnd = N_ELEMENT * 3;
pLineIndices[7].iStart = N_ELEMENT * 3;
pLineIndices[7].iEnd = N_ELEMENT * 7;
pLineIndices[8].iStart = N_ELEMENT * 7;
pLineIndices[8].iEnd = N_ELEMENT * 0;
pLineIndices[9].iStart = N_ELEMENT * 0;
pLineIndices[9].iEnd = N_ELEMENT * 4;
pLineIndices[10].iStart = N_ELEMENT * 0;
pLineIndices[10].iEnd = N_ELEMENT * 5;
pLineIndices[11].iStart = N_ELEMENT * 1;
pLineIndices[11].iEnd = N_ELEMENT * 5;
pLineIndices[12].iStart = N_ELEMENT * 5;
pLineIndices[12].iEnd = N_ELEMENT * 2;

pLineIndices[13].iStart = N_ELEMENT * 6;
pLineIndices[13].iEnd = N_ELEMENT * 7;
pLineIndices[14].iStart = N_ELEMENT * 5;
pLineIndices[14].iEnd = N_ELEMENT * 6;
#if 0
pLineIndices[15].iStart = N_ELEMENT * 7;
pLineIndices[15].iEnd = N_ELEMENT * 4;
pLineIndices[16].iStart = N_ELEMENT * 4;
pLineIndices[16].iEnd = N_ELEMENT * 5;
pLineIndices[17].iStart = N_ELEMENT * 5;
pLineIndices[17].iEnd = N_ELEMENT * 7;
#else
#if 0
pLineIndices[15].iStart = N_ELEMENT * 7;
pLineIndices[15].iEnd = N_ELEMENT * 4;
pLineIndices[16].iStart = N_ELEMENT * 4;
pLineIndices[16].iEnd = N_ELEMENT * 5;
pLineIndices[17].iStart = N_ELEMENT * 5;
pLineIndices[17].iEnd = N_ELEMENT * 7;

pLineIndices[18].iStart = N_ELEMENT * 5;
pLineIndices[18].iEnd = N_ELEMENT * 8;
pLineIndices[19].iStart = N_ELEMENT * 7;
pLineIndices[19].iEnd = N_ELEMENT * 8;
pLineIndices[20].iStart = N_ELEMENT * 4;
pLineIndices[20].iEnd = N_ELEMENT * 8;
#else
pLineIndices[15].iStart = N_ELEMENT * 7;
pLineIndices[15].iEnd = N_ELEMENT * 10;
pLineIndices[16].iStart = N_ELEMENT * 10;
pLineIndices[16].iEnd = N_ELEMENT * 4;
pLineIndices[17].iStart = N_ELEMENT * 7;
pLineIndices[17].iEnd = N_ELEMENT * 9;
pLineIndices[18].iStart = N_ELEMENT * 9;
pLineIndices[18].iEnd = N_ELEMENT * 5;
pLineIndices[19].iStart = N_ELEMENT * 5;
pLineIndices[19].iEnd = N_ELEMENT * 8;
pLineIndices[20].iStart = N_ELEMENT * 8;
pLineIndices[20].iEnd = N_ELEMENT * 4;

pLineIndices[21].iStart = N_ELEMENT * 10;
pLineIndices[21].iEnd = N_ELEMENT * 9;
pLineIndices[22].iStart = N_ELEMENT * 9;
pLineIndices[22].iEnd = N_ELEMENT * 8;
pLineIndices[23].iStart = N_ELEMENT * 8;
pLineIndices[23].iEnd = N_ELEMENT * 10;
#endif
#endif
#endif
#endif
	__edgeIndexBufferPtr->Unlock();

#if 0
#if 0
dwNumFaces = 12;
#else
#if 0
dwNumFaces = 12 + (3 - 1);
#else
dwNumFaces = 12 + (4 - 1);
#endif
#endif
#endif
#if 1
	unsigned nFace = __numFaces;
#else
	__numFaces = dwNumFaces;
#endif
	__devicePtr->CreateIndexBuffer(
		nFace * sizeof(CFace::FaceIndex),
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX32,
		D3DPOOL_MANAGED,
		&__faceIndexBufferPtr,
		NULL
	);
	CFace::FaceIndex*	pFaceIndices;
	__faceIndexBufferPtr->Lock(0, 0, (void**) &pFaceIndices, 0);
	for (DWORD iFace = 0; iFace < nFace; ++iFace) {
		CFace&	face = __facePtr[iFace];
		assert(iFace == face.id);
		DWORD	i0 = face.prID0;
		i0 *= N_ELEMENT;
		DWORD	i1 = face.prID1;
		i1 *= N_ELEMENT;
		DWORD	i2 = face.prID2;
		i2 *= N_ELEMENT;
		pFaceIndices[iFace].i0 = i0;
		pFaceIndices[iFace].i1 = i1;
		pFaceIndices[iFace].i2 = i2;
	}
#if 0
pFaceIndices[0].i0 = N_ELEMENT * 0;
pFaceIndices[0].i1 = N_ELEMENT * 1;
pFaceIndices[0].i2 = N_ELEMENT * 3;

pFaceIndices[1].i0 = N_ELEMENT * 1;
pFaceIndices[1].i1 = N_ELEMENT * 2;
pFaceIndices[1].i2 = N_ELEMENT * 3;

pFaceIndices[2].i0 = N_ELEMENT * 2;
pFaceIndices[2].i1 = N_ELEMENT * 6;
pFaceIndices[2].i2 = N_ELEMENT * 3;

pFaceIndices[3].i0 = N_ELEMENT * 6;
pFaceIndices[3].i1 = N_ELEMENT * 7;
pFaceIndices[3].i2 = N_ELEMENT * 3;

pFaceIndices[4].i0 = N_ELEMENT * 3;
pFaceIndices[4].i1 = N_ELEMENT * 7;
pFaceIndices[4].i2 = N_ELEMENT * 0;

pFaceIndices[5].i0 = N_ELEMENT * 7;
pFaceIndices[5].i1 = N_ELEMENT * 4;
pFaceIndices[5].i2 = N_ELEMENT * 0;

pFaceIndices[6].i0 = N_ELEMENT * 4;
pFaceIndices[6].i1 = N_ELEMENT * 5;
pFaceIndices[6].i2 = N_ELEMENT * 0;

pFaceIndices[7].i0 = N_ELEMENT * 0;
pFaceIndices[7].i1 = N_ELEMENT * 5;
pFaceIndices[7].i2 = N_ELEMENT * 1;

pFaceIndices[8].i0 = N_ELEMENT * 1;
pFaceIndices[8].i1 = N_ELEMENT * 5;
pFaceIndices[8].i2 = N_ELEMENT * 2;

pFaceIndices[9].i0 = N_ELEMENT * 2;
pFaceIndices[9].i1 = N_ELEMENT * 5;
pFaceIndices[9].i2 = N_ELEMENT * 6;

pFaceIndices[10].i0 = N_ELEMENT * 6;
pFaceIndices[10].i1 = N_ELEMENT * 5;
pFaceIndices[10].i2 = N_ELEMENT * 7;

#if 0
pFaceIndices[11].i0 = N_ELEMENT * 7;
pFaceIndices[11].i1 = N_ELEMENT * 5;
pFaceIndices[11].i2 = N_ELEMENT * 4;
#else
#if 0
pFaceIndices[11].i0 = N_ELEMENT * 4;
pFaceIndices[11].i1 = N_ELEMENT * 7;
pFaceIndices[11].i2 = N_ELEMENT * 8;
pFaceIndices[12].i0 = N_ELEMENT * 7;
pFaceIndices[12].i1 = N_ELEMENT * 5;
pFaceIndices[12].i2 = N_ELEMENT * 8;
pFaceIndices[13].i0 = N_ELEMENT * 8;
pFaceIndices[13].i1 = N_ELEMENT * 5;
pFaceIndices[13].i2 = N_ELEMENT * 4;
#else
pFaceIndices[11].i0 = N_ELEMENT * 7;
pFaceIndices[11].i1 = N_ELEMENT * 9;
pFaceIndices[11].i2 = N_ELEMENT * 10;
pFaceIndices[12].i0 = N_ELEMENT * 9;
pFaceIndices[12].i1 = N_ELEMENT * 5;
pFaceIndices[12].i2 = N_ELEMENT * 8;
pFaceIndices[13].i0 = N_ELEMENT * 9;
pFaceIndices[13].i1 = N_ELEMENT * 8;
pFaceIndices[13].i2 = N_ELEMENT * 10;
pFaceIndices[14].i0 = N_ELEMENT * 10;
pFaceIndices[14].i1 = N_ELEMENT * 8;
pFaceIndices[14].i2 = N_ELEMENT * 4;
#endif
#endif
#endif
	__faceIndexBufferPtr->Unlock();

	// 각 Vertex 에서의 Normal 값을 새로 결정해야 한다;
	// 원래 Mesh 정보로부터 얻어지는 Vertex 에서는 Normal 은,
	// 그 Vertex 가 속하는 Face 의 Normal 값이다.
	// 하지만 그러한 Vertex 를 하나의 Point Repr. 으로 바꾸었으므로
	// 주어진 Vertex 의 Normal 은 새로 계산되어야 한다:
	//	Vertex 주변 Face 의 Normal 에 대한 평균
#if 1
	DWORD	dwOptions = D3DPOOL_MANAGED;//D3DXMESH_SYSTEMMEM | D3DXMESH_32BIT,
D3DVERTEXELEMENT9	element[3 + 1] = {
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
	{
		0,
		2 * sizeof(D3DXVECTOR3),
		D3DDECLTYPE_FLOAT2,
		D3DDECLMETHOD_DEFAULT,
		D3DDECLUSAGE_TEXCOORD,
		0
	},

	D3DDECL_END()
};
	hResult = D3DXCreateMesh(
		nFace,
		nPointRepr,
		dwOptions,
		element,
		__devicePtr,
		&__meshPtr
	);
	assert(hResult == D3D_OK);

	// Fill it up with vertices
	FLOAT*	pVB = NULL;
	hResult = __meshPtr->LockVertexBuffer(0, (void**) &pVB);
	assert(D3D_OK == hResult);
{
	kMesh::D3DXVERTEX*	pVertices;
	__vertexBufferPtr->Lock(0, 0, (void**) &pVertices, 0);
//	CopyMemory(pVB, pVertices, nPointRepr * sizeof(kMesh::D3DXVERTEX));
	for (DWORD i = 0; i < nPointRepr; ++i) {
#if 0
		pVB[i] = pVertices[i].p;
#else
		pVB[8 * i + 0] = pVertices[i].p.x;
		pVB[8 * i + 1] = pVertices[i].p.y;
		pVB[8 * i + 2] = pVertices[i].p.z;
		pVB[8 * i + 3] = pVertices[i].n.x;
		pVB[8 * i + 4] = pVertices[i].n.y;
		pVB[8 * i + 5] = pVertices[i].n.z;
		pVB[8 * i + 6] = pVertices[i].tu;
		pVB[8 * i + 7] = pVertices[i].tv;
#endif
	}
	__vertexBufferPtr->Unlock();
}
	hResult = __meshPtr->UnlockVertexBuffer();
	assert(D3D_OK == hResult);

	// 렌더링과는 무관하다;
	// 누가 이걸 참조한다면 모를까...
	// Fill it up with indices
	CFace::FaceIndex*	pIB = NULL;
	hResult = __meshPtr->LockIndexBuffer(0, (void**) &pIB);
	assert(D3D_OK == hResult);
{
	CFace::FaceIndex*	pFaceIndices;
	__faceIndexBufferPtr->Lock(0, 0, (void**) &pFaceIndices, 0);
//	CopyMemory(pIB, pFaceIndices, dwNumFaces * sizeof(CFace::FaceIndex));
	for (DWORD i = 0; i < nFace; ++i) {
		pIB[i].i0 = pFaceIndices[i].i0 / N_ELEMENT;	// DrawIndexedPrimitive 에서 사용하는 Index Buffer 에,
								// Vertex 의 크기(N_ELEMENT * sizeof(D3DXVECTOR3))를
								// 고려하여, 실제 Position 이 들어간 index 를 쓰는 것과
								// 달리 순차적인 인덱스가 들어가야 한다!
		pIB[i].i1 = pFaceIndices[i].i1 / N_ELEMENT;
		pIB[i].i2 = pFaceIndices[i].i2 / N_ELEMENT;
	}
	__faceIndexBufferPtr->Unlock();
}
	hResult = __meshPtr->UnlockIndexBuffer();
	assert(D3D_OK == hResult);

#if 0
{
	DWORD	dw = __meshPtr->GetNumBytesPerVertex();
	dw = __meshPtr->GetNumFaces();
	dw = __meshPtr->GetNumVertices();
	dw = __meshPtr->GetOptions();
	LPDIRECT3DVERTEXBUFFER9		pVB = NULL;
	assert(D3D_OK == __meshPtr->GetVertexBuffer(&pVB));
	D3DXVECTOR3*	pV = NULL;
	pVB->Lock(0, 0, (void**) &pV, 0);
	pVB->Unlock();
	LPDIRECT3DINDEXBUFFER9		pIB = NULL;
	assert(D3D_OK == __meshPtr->GetIndexBuffer(&pIB));
	DWORD*	pI = NULL;
	pIB->Lock(0, 0, (void**) &pI, 0);
	pIB->Unlock();

	D3DVERTEXELEMENT9	Declaration[MAX_FVF_DECL_SIZE];
	assert(D3D_OK == __meshPtr->GetDeclaration(Declaration));

	DWORD*	pAdjacency = NULL;
	HRESULT	hr = D3D_OK;
	hr = __meshPtr->GenerateAdjacency(0.0001f, pAdjacency);

	int	iBreakPoint = 1;
}
#endif

#if 0
{
	DWORD*	pAdjacency = new DWORD[3 * dwNumFaces];
	for (DWORD i = 0; i < dwNumFaces; ++i) {
		pAdjacency[3 * i + 0] = __facePtr[i].ajID01;
		pAdjacency[3 * i + 1] = __facePtr[i].ajID12;
		pAdjacency[3 * i + 2] = __facePtr[i].ajID20;
	}
	LPD3DXBUFFER	pErrorsAndWarnings = NULL;
	HRESULT	hr = D3DXValidMesh(
		__meshPtr,
		pAdjacency,
		&pErrorsAndWarnings
	);
	LPCSTR	pErrorAndWarningsMessage = NULL;
	if (pErrorsAndWarnings) {
		pErrorAndWarningsMessage = (LPCSTR) pErrorsAndWarnings->GetBufferPointer();
	}
	if (pAdjacency) {
		delete[] pAdjacency;
		pAdjacency = NULL;
	}
}
#endif

//	DWORD	dwFormat = D3DXF_FILEFORMAT_TEXT;	// D3DXF_FILEFORMAT_BINARY
			// D3DXF_FILEFORMAT_COMPRESSED
			//	Compressed file.
			//	With this flag, you must also specify the binary format or the text format.
{
#if 0
	DWORD*	pdwAttributes;
	assert(D3D_OK == __meshPtr->LockAttributeBuffer(0, &pdwAttributes));
	for (DWORD i = 0; i < dwNumFaces; ++i)
		pdwAttributes[i] = 0;
	assert(D3D_OK == __meshPtr->UnlockAttributeBuffer());
	// Fill subsets
	D3DXATTRIBUTERANGE*	pAttributes = new D3DXATTRIBUTERANGE[1];
	for (DWORD i = 0; i < 1; ++i) {
		pAttributes[i].AttribId = 0;
		pAttributes[i].FaceStart = 0;
		pAttributes[i].FaceCount = dwNumFaces;
		pAttributes[i].VertexStart = 0;
		pAttributes[i].VertexCount = dwNumVertices;
	}
//	assert(D3D_OK == __meshPtr->SetAttributeTable(pAttributes, 1));
//	HRESULT	hr = __meshPtr->SetAttributeTable(pAttributes, 1);
#endif
	DWORD*	pdwAdjacency = new DWORD[3 * nFace];
	for (DWORD i = 0; i < nFace; ++i) {
		pdwAdjacency[3 * i + 0] = __facePtr[i].ajID01;
		pdwAdjacency[3 * i + 1] = __facePtr[i].ajID12;
		pdwAdjacency[3 * i + 2] = __facePtr[i].ajID20;
	}

	(void) fprintf(stdout, "\tCalling D3DXComputeNormals in kMesh::SetMesh... (%f)\n", __timer.GetElapsedTime());
	hResult = D3DXComputeNormals(__meshPtr, pdwAdjacency);
	(void) fprintf(stdout, "\t...Done (%f)\n", __timer.GetElapsedTime());
	if (pdwAdjacency) {
		delete[] pdwAdjacency;
		pdwAdjacency = NULL;
	}
	assert(D3D_OK == hResult);
	// Vertex Buffer 에 있는 normal 을 수정한다!
	FLOAT*	pVB = NULL;
	hResult = __meshPtr->LockVertexBuffer(0, (void**) &pVB);
	assert(D3D_OK == hResult);
{
	kMesh::D3DXVERTEX*	pVertices;
	__vertexBufferPtr->Lock(0, 0, (void**) &pVertices, 0);
	for (DWORD i = 0; i < nPointRepr; ++i) {
		pVertices[i].n.x = pVB[8 * i + 3];
		pVertices[i].n.y = pVB[8 * i + 4];
		pVertices[i].n.z = pVB[8 * i + 5];
		__pointReprPtr[i].v.normal = pVertices[i].n;
	}
#if 1	// 각 Face 의 노말을 저장
	for (int faceID = 0; faceID < (int) __numFaces; ++faceID) {
		CFace&	face = __facePtr[faceID];
		D3DXVECTOR3&	p0 = pVertices[face.prID0].p;
		D3DXVECTOR3&	p1 = pVertices[face.prID1].p;
		D3DXVECTOR3&	p2 = pVertices[face.prID2].p;
		D3DXVECTOR3	v01 = p1 - p0;
//		D3DXVec3Normalize(&v01, &v01);
		D3DXVECTOR3	v02 = p2 - p0;
//		D3DXVec3Normalize(&v02, &v02);
		D3DXVECTOR3&	normal = face.normal;
		D3DXVec3Cross(&normal, &v01, &v02);
		D3DXVec3Normalize(&normal, &normal);
	}
#endif
	__vertexBufferPtr->Unlock();
}
	hResult = __meshPtr->UnlockVertexBuffer();
	assert(D3D_OK == hResult);

#if 0	// 만든 메쉬를 .x 파일로 만든다. (확인용)
	DWORD	dwFormat = D3DXF_FILEFORMAT_TEXT;	// D3DXF_FILEFORMAT_BINARY
	hResult = D3DXSaveMeshToX(
		TEXT("xxx.x"),
		__meshPtr,
		NULL,//pAdjacency,
		NULL,
		NULL,
		0,
		dwFormat
	);
	assert(D3D_OK == hResult);
#endif

#if 0
	if (pAdjacency) {
		delete[] pAdjacency;
		pAdjacency = NULL;
	}
	if (pAttributes) {
		delete[] pAttributes;
		pAttributes = NULL;
	}
#endif
}
#if 0
{
	if (__meshPtr) {
		__meshPtr->Release();
		__meshPtr = NULL;
	}
	LPD3DXBUFFER	pAdjacency = NULL;
	LPD3DXBUFFER	pMaterials = NULL;
	LPD3DXBUFFER	pEffectInstances = NULL;
	DWORD		dwNumMaterials = 0;
	hResult = D3DXLoadMeshFromX(
		TEXT("xxx.x"),
		dwOptions,
		__devicePtr,
		&pAdjacency,
		&pMaterials,
		&pEffectInstances,
		&dwNumMaterials,
		&__meshPtr
	);
	DWORD*	pdwAdjacency = (DWORD*) pAdjacency->GetBufferPointer();
	assert(D3D_OK == hResult);
}
#endif
#if 0
	LPD3DXMESH	meshPtr = NULL;
	LPD3DXBUFFER	pAdjacency = NULL;
	LPD3DXBUFFER	pMaterials = NULL;
	LPD3DXBUFFER	pEffectInstances = NULL;
	DWORD		dwNumMaterials = 0;
	assert(D3D_OK == D3DXLoadMeshFromX(
		TEXT("xxx.x"),
		dwOptions,
		__devicePtr,
		&pAdjacency,
		&pMaterials,
		&pEffectInstances,
		&dwNumMaterials,
		&meshPtr
	));
#if 1
	DWORD*	pdwAdjacency = (DWORD*) pAdjacency->GetBufferPointer();
	for (DWORD i = 0; i < dwNumFaces; ++i) {
		assert(pdwAdjacency[3 * i + 0] == __facePtr[i].ajID01);
		assert(pdwAdjacency[3 * i + 1] == __facePtr[i].ajID12);
		assert(pdwAdjacency[3 * i + 2] == __facePtr[i].ajID20);
	}
#else
	DWORD*	pAdjacency = new DWORD[3 * dwNumFaces];
	for (DWORD i = 0; i < dwNumFaces; ++i) {
		pAdjacency[3 * i + 0] = __facePtr[i].ajID01;
		pAdjacency[3 * i + 1] = __facePtr[i].ajID12;
		pAdjacency[3 * i + 2] = __facePtr[i].ajID20;
	}
#endif
	D3DXMATERIAL*	p = (D3DXMATERIAL*) pMaterials->GetBufferPointer();
	if (pAdjacency) {
		pAdjacency->Release();
		pAdjacency = NULL;
	}
	if (pMaterials) {
		pMaterials->Release();
		pMaterials = NULL;
	}
	if (pEffectInstances) {
		pEffectInstances->Release();
		pEffectInstances = NULL;
	}
#if 0
{
	DWORD*	pdwAdjacency = new DWORD[3 * dwNumFaces];
	for (DWORD i = 0; i < dwNumFaces; ++i) {
		pdwAdjacency[3 * i + 0] = __facePtr[i].ajID01;
		pdwAdjacency[3 * i + 1] = __facePtr[i].ajID12;
		pdwAdjacency[3 * i + 2] = __facePtr[i].ajID20;
	}

	FLOAT*	pfVertexSignal = NULL;
	assert(SUCCEEDED(meshPtr->LockVertexBuffer(D3DLOCK_NOOVERWRITE | D3DLOCK_NOSYSLOCK, (void**) &pfVertexSignal)));

	LPD3DXBUFFER	pIMTBuffer;
	WORD	Offset = 12;	// NORMAL 정보를 이용해서 IMT 를 구한다.
				// Vertex Buffer 에서 NORMAL 정보는 12 바이트 오프셋에 위치한다.
	DWORD	dwWrappingOption = 0L;
	assert(SUCCEEDED(D3DXComputeIMTFromPerVertexSignal(
		meshPtr,

		pfVertexSignal + Offset / 4,	// 이 위치에서부터 시작한다.
		3,				// 총 세 개의 FLOAT 를 이용한다.
		meshPtr->GetNumBytesPerVertex(),	// 이 단위로 다음 값을 얻을 위치로 이동한다.

		dwWrappingOption,
		NULL,	// LPD3DXUVATLASCB	pStatusCallback
		NULL,	// LPVOID		pUserContext
		&pIMTBuffer
	)));
	assert(SUCCEEDED(meshPtr->UnlockVertexBuffer()));

	LPD3DXMESH	pMeshResult = NULL;
	DWORD	dwQualityOption = D3DXUVATLAS_DEFAULT;
	LPD3DXBUFFER	pFacePartitioning = NULL, pVertexRemapArray = NULL;
	LPD3DXBUFFER	pPartitionResultAdjacency = NULL;
	FLOAT	fMaxStretchOut;
	UINT	uNumChartsOut;
#if 1	// Create
	assert(SUCCEEDED(D3DXUVAtlasCreate(
		meshPtr,		// LPD3DXMESH meshPtr,
		0,			// UINT dwMaxChartNumber,
		1.f / 6,		// FLOAT fMaxStretch,
512,	// UINT uWidth,
512,	// UINT uHeight,
2.f,	// FLOAT fGutter
		0,			// DWORD dwTextureIndex,
		pdwAdjacency,		// CONST DWORD *pdwAdjacency,
		NULL,			// CONST DWORD *pdwFalseEdges,
		(FLOAT*) pIMTBuffer->GetBufferPointer(),	// CONST FLOAT *pfIMTArray,

		NULL,			// LPD3DXUVATLASCB pCallback,
		0.f,			// FLOAT fCallbackFrequency,
		NULL,			// LPVOID pUserContent,

		dwQualityOption,	// DWORD dwOptions,
		&pMeshResult,		// LPD3DXMESH *ppMeshOut,
		// 버그: 설명서에는 LPD3DXBUFFER pFacePartitioning 로 되어있다.
		&pFacePartitioning,	// LPD3DXBUFFER *ppFacePartitioning,
		&pVertexRemapArray,	// LPD3DXBUFFER *ppVertexRemapArray,
//		&pPartitionResultAdjacency,	// LPD3DXBUFFER *ppPartitionResultAdjacency,
		&fMaxStretchOut,	// FLOAT *pfMaxStretchOut,
		&uNumChartsOut		// UINT *puNumChartsOut
	)));
	{
		FILE*	f = fopen("a", "w");
		DWORD*	p = (DWORD*) pVertexRemapArray->GetBufferPointer();
		for (DWORD i = 0; i < pMeshResult->GetNumVertices(); ++i) {
			fprintf(f, "i = %u -> %u\n", i, p[i]);
		}
		fclose(f);
	}
#else	// Partition
	assert(SUCCEEDED(D3DXUVAtlasPartition(
		meshPtr,		// LPD3DXMESH meshPtr,
		0,			// UINT dwMaxChartNumber,
		1.f / 6,		// FLOAT fMaxStretch,
		0,			// DWORD dwTextureIndex,
		pdwAdjacency,		// CONST DWORD *pdwAdjacency,
		NULL,			// CONST DWORD *pdwFalseEdges,
		(FLOAT*) pIMTBuffer->GetBufferPointer(),	// CONST FLOAT *pfIMTArray,

		NULL,			// LPD3DXUVATLASCB pCallback,
		0.f,			// FLOAT fCallbackFrequency,
		NULL,			// LPVOID pUserContent,

		dwQualityOption,	// DWORD dwOptions,
		&pMeshResult,		// LPD3DXMESH *ppMeshOut,
		// 버그: 설명서에는 LPD3DXBUFFER pFacePartitioning 로 되어있다.
		&pFacePartitioning,	// LPD3DXBUFFER *ppFacePartitioning,
		&pVertexRemapArray,	// LPD3DXBUFFER *ppVertexRemapArray,
		&pPartitionResultAdjacency,	// LPD3DXBUFFER *ppPartitionResultAdjacency,
		&fMaxStretchOut,	// FLOAT *pfMaxStretchOut,
		&uNumChartsOut		// UINT *puNumChartsOut
	)));
#endif
{
#define COLOR_COUNT	8
	D3DXMATERIAL	GeneratedMaterials[COLOR_COUNT];

	FLOAT	ColorList[COLOR_COUNT][3] = {
		{1.0f, 0.5f, 0.5f},
		{0.5f, 1.0f, 0.5f},
		{1.0f, 1.0f, 0.5f},
		{0.5f, 1.0f, 1.0f},
		{1.0f, 0.5f,0.75f},
		{0.0f, 0.5f, 0.75f},
		{0.5f, 0.5f, 0.75f},
		{0.5f, 0.5f, 1.0f},
	};
{
	for (UINT i = 0; i < COLOR_COUNT; ++i) {
		GeneratedMaterials[i].MatD3D.Ambient.a = 0;
		GeneratedMaterials[i].MatD3D.Ambient.r = ColorList[i][0];
		GeneratedMaterials[i].MatD3D.Ambient.g = ColorList[i][1];
		GeneratedMaterials[i].MatD3D.Ambient.b = ColorList[i][2];

		GeneratedMaterials[i].MatD3D.Diffuse = GeneratedMaterials[i].MatD3D.Ambient;

		GeneratedMaterials[i].MatD3D.Power = 0;

		GeneratedMaterials[i].MatD3D.Emissive.a = 0;
		GeneratedMaterials[i].MatD3D.Emissive.r = 0;
		GeneratedMaterials[i].MatD3D.Emissive.g = 0;
		GeneratedMaterials[i].MatD3D.Emissive.b = 0;

		GeneratedMaterials[i].MatD3D.Specular.a = 0;
		GeneratedMaterials[i].MatD3D.Specular.r = 0.5;
		GeneratedMaterials[i].MatD3D.Specular.g = 0.5;
		GeneratedMaterials[i].MatD3D.Specular.b = 0.5;

		GeneratedMaterials[i].pTextureFilename = NULL;
	}

	DWORD*	pdwAttributeOut = NULL;
	assert(SUCCEEDED(pMeshResult->LockAttributeBuffer(D3DLOCK_NOSYSLOCK, &pdwAttributeOut)));
	DWORD*	pdwChartMapping = (DWORD*) pFacePartitioning->GetBufferPointer();
	for (DWORD i = 0; i < pMeshResult->GetNumFaces(); ++i) {
		pdwAttributeOut[i] = pdwChartMapping[i] % COLOR_COUNT;
	}
	pdwAttributeOut = NULL;
	assert(SUCCEEDED(pMeshResult->UnlockAttributeBuffer()));
}
	assert(D3D_OK == D3DXSaveMeshToX(
		TEXT("xxxUVAtlas.x"),
		pMeshResult,
		NULL,
		GeneratedMaterials,
		NULL,
		COLOR_COUNT,
		dwFormat
	));
}

	if (pdwAdjacency) {
		delete[] pdwAdjacency;
		pdwAdjacency = NULL;
	}
	if (pIMTBuffer) {
		pIMTBuffer->Release();
		pIMTBuffer = NULL;
	}
	if (pFacePartitioning) {
		pFacePartitioning->Release();
		pFacePartitioning = NULL;
	}
	if (pVertexRemapArray) {
		pVertexRemapArray->Release();
		pVertexRemapArray = NULL;
	}
	if (pPartitionResultAdjacency) {
		pPartitionResultAdjacency->Release();
		pPartitionResultAdjacency = NULL;
	}
	if (pMeshResult) {
		pMeshResult->Release();
		pMeshResult = NULL;
	}
}
#endif
	if (meshPtr) {
		meshPtr->Release();
		meshPtr = NULL;
	}
#endif
#endif
	return 0;
}


int	kMesh::ShowVertex(
	const LPDIRECT3DVERTEXBUFFER9	vertexBufferPtr,
	const LPDIRECT3DVERTEXDECLARATION9	vertexDeclarationPtr,
	UINT				stride,
	DWORD				nPointRepr,
	FLOAT				pointSize,
	D3DXVECTOR4			pointColor,
	UINT		StartVertex,
	float	zDelta
) const
{
	HRESULT	hResult;
	__effectPtr->SetFloat("g__zDelta", zDelta);

	__effectPtr->SetInt("g__FillMode", D3DFILL_POINT);
	__effectPtr->SetMatrix("g_mWorldViewProjection", &__worldViewProjectionMatrix);
	__effectPtr->SetMatrix("g_mWorld", &__worldMatrix);
	__effectPtr->SetFloat("pointSize", pointSize);
	__effectPtr->SetVector("pointColor", &pointColor);
	D3DXHANDLE	hTechnique = __effectPtr->GetTechnique(0);
	assert(hTechnique);
	hResult = __effectPtr->SetTechnique(hTechnique);
	assert(S_OK == hResult);
	UINT	NumberOfPasses;
	hResult = __effectPtr->Begin(&NumberOfPasses, 0);
	assert(D3D_OK == hResult);
	for (UINT iPass = 0; iPass < NumberOfPasses; ++iPass) {
		hResult = __effectPtr->BeginPass(iPass);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetVertexDeclaration(vertexDeclarationPtr);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetStreamSource(0, vertexBufferPtr, 0, stride);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->DrawPrimitive(D3DPT_POINTLIST, StartVertex, nPointRepr);
		assert(D3D_OK == hResult);
		hResult = __effectPtr->EndPass();
		assert(S_OK == hResult);
	}
	hResult = __effectPtr->End();
	assert(S_OK == hResult);

	__effectPtr->SetFloat("g__zDelta", 0.f);
	return 0;
}

int	kMesh::ShowEdge(
	const LPDIRECT3DINDEXBUFFER9	edgeIndexBufferPtr,
	const D3DXVECTOR4&		color,
	DWORD				nEdge
) const
{
	HRESULT	hResult;

	__effectPtr->SetInt("g__FillMode", D3DFILL_WIREFRAME);
	__effectPtr->SetMatrix("g_mWorldViewProjection", &__worldViewProjectionMatrix);
	__effectPtr->SetMatrix("g_mWorld", &__worldMatrix);
	__effectPtr->SetVector("pointColor", &color);
	D3DXHANDLE	hTechnique = __effectPtr->GetTechnique(0);
	assert(hTechnique);
	hResult = __effectPtr->SetTechnique(hTechnique);
	assert(S_OK == hResult);
	UINT	NumberOfPasses;
	assert(D3D_OK == hResult);
	hResult = __effectPtr->Begin(&NumberOfPasses, 0);
	for (UINT iPass = 0; iPass < NumberOfPasses; ++iPass) {
		hResult = __effectPtr->BeginPass(iPass);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetVertexDeclaration(__vertexDeclarationPtr);
		assert(D3D_OK == hResult);
		// 인덱스 버퍼를 만들 때, sizeof(D3DXVECTOR3) 기준
		hResult = __devicePtr->SetStreamSource(0, __vertexBufferPtr, 0, sizeof(D3DXVECTOR3));
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetIndices(edgeIndexBufferPtr);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->DrawIndexedPrimitive(D3DPT_LINELIST, 0, 0, N_ELEMENT * __numPointReprs, 0, nEdge);
		assert(D3D_OK == hResult);
		hResult = __effectPtr->EndPass();
		assert(S_OK == hResult);
	}
	hResult = __effectPtr->End();
	assert(D3D_OK == hResult);

	return 0;
}

#if 1	// ColorBand
inline static void	SetXYInProjectionSpace(int i, int k, float x_offset, float unit_width, float&x, float y_offset, float unit_height, float&y)
{
	x = (-1.f + x_offset) + i * unit_width;
	y = (1 - y_offset) - k * unit_height;
}

// (-1, 1)	(0, 0)
// (1, -1)	(W - 1, H - 1)
// (x, y)	((x + 1) / 2 * W - 1, (1 - y) / 2 * H - 1)
inline static void	ProjectionSpaceToScreenSpace(float x0, UINT W, float& x, float y0, UINT H, float& y)
{
	x = ceilf((x0 + 1) / 2 * W) - 1.f;
	y = ceilf((1 - y0) / 2 * H) - 1.f;
}
void	kMesh::ShowColorBand() const
{
	const kColorBand&	colorBand = *((kColorBand*) __statePtr);
	// 유형
	COLOR_BAND_TYPE_T	type	= colorBand.typeRef();
	if (type == COLOR_BAND_UNDEF)
		return;

	// dimension
	const kColorBand::Dimension&	dimension = colorBand.dimensionRef();
	float	x_offset	= dimension.x_offset;
	float	width		= dimension.width;
	float	unit_width = type == COLOR_BAND_TOP_TO_BOTTOM || type == COLOR_BAND_BOTTOM_TO_TOP ? width : width / 3.f;
	float	y_offset	= dimension.y_offset;
	float	height		= dimension.height;
	float	unit_height = type == COLOR_BAND_LEFT_TO_RIGHT || type == COLOR_BAND_RIGHT_TO_LEFT ? height : height / 3.f;
	// indicator
	const kColorBand::Indicator*	indicators = colorBand.indicators();

	HRESULT	hr;
#if 1	// indicator
	const D3DSURFACE_DESC*	pBackBufferSurfaceDesc = DXUTGetD3D9BackBufferSurfaceDesc();
	const UINT&	W = pBackBufferSurfaceDesc->Width;
	const UINT&	H = pBackBufferSurfaceDesc->Height;
	D3DXVECTOR2	__indicatorTwoPoints[2];	// indicator 라인용 두 점
#endif	// indicator
#if 1	// IKC
	struct IKC
	{
		int	i;
		int	k;
		int	c;
	};
#endif	// IKC

if (type == COLOR_BAND_TOP_TO_BOTTOM) {
/*
#if 1	// IKC
	|
	|
	V (k)
#endif	// IKC
	+--+ +	(0, 0, 0)	(0)
	| / /|
	|/ / |
	+ +--+	(1, 0, 0)	(1)
	+--+ +
	| / /|
	|/ / |
	+ +--+	(1, 1, 0)	(2)
	+--+ +
	| / /|
	|/ / |
	+ +--+	(1, 1, 1)	(3)
#if 1	// IKC
	k + c = 3 (일정)
#endif	// IKC
 */
//	float	x_offset = .1f;
//	float	y_offset = .4f;
//	float	width = .1f;
//	float	height = .2f;
#if 1	// fill buffer
{
	IKC	ikc0[] = {{0, 0, 0}, {1, 0, 0}, {0, 1, 1}};
	IKC	ikc1[] = {{1, 0, 0}, {1, 1, 1}, {0, 1, 1}};

	D3DXVERTEX*	colorBandVertexArray;
	__colorBandVertexBufferPtr->Lock(0, 0, (void**) &colorBandVertexArray, 0);
	int	index = 0;
	for (int k = 0; k < 3; ++k) {
		// ikc0
		for (int i = 0; i < 3; ++i) {
			D3DXVERTEX&	v = colorBandVertexArray[index++];
			const IKC&	ikc = ikc0[i];

			SetXYInProjectionSpace(ikc.i, k + ikc.k, x_offset, unit_width, v.p.x, y_offset, unit_height, v.p.y);
			int	c = ikc.c + k;
			v.c = c == 3 ? D3DXVECTOR3(1, 1, 1) : (c == 2 ? D3DXVECTOR3(1, 1, 0) : (c == 1 ? D3DXVECTOR3(1, 0, 0) : D3DXVECTOR3(0, 0, 0)));
		}
		// ikc1
		for (int i = 0; i < 3; ++i) {
			D3DXVERTEX&	v = colorBandVertexArray[index++];
			const IKC&	ikc = ikc1[i];

			SetXYInProjectionSpace(ikc.i, k + ikc.k, x_offset, unit_width, v.p.x, y_offset, unit_height, v.p.y);
			int	c = ikc.c + k;
			v.c = c == 3 ? D3DXVECTOR3(1, 1, 1) : (c == 2 ? D3DXVECTOR3(1, 1, 0) : (c == 1 ? D3DXVECTOR3(1, 0, 0) : D3DXVECTOR3(0, 0, 0)));
		}
	}

	__colorBandVertexBufferPtr->Unlock();
}
#endif	// fill buffer

#if 1	// 렌더링
{
	// kEnum.h 에 있는 SHADING_TYPE 와
	// mesh.fx 에 있는 g__texture 가 같은 상수를 사용하도록 맞춰야 한다!
	HRESULT	hr;
	hr = __effectPtr->SetInt("g__texture", SHADING_NONE);
	assert(hr == D3D_OK);
//	__effectPtr->SetInt("g__overlap", __overlap);
	__effectPtr->SetInt("g__FillMode", D3DFILL_SOLID);
//	__effectPtr->SetInt("g__FillMode", D3DFILL_POINT);
//	__effectPtr->SetVector("pointColor", &D3DXVECTOR4(1, 1, 0, 1));
	D3DXMATRIX	identity;
	D3DXMatrixIdentity(&identity);
	__effectPtr->SetMatrix("g_mWorldViewProjection", &identity);
//	__effectPtr->SetMatrix("g_mWorld", &__worldMatrix);
	D3DXHANDLE	hTechnique = __effectPtr->GetTechnique(0);
	assert(hTechnique);
	hr = __effectPtr->SetTechnique(hTechnique);
	assert(S_OK == hr);
	UINT	NumberOfPasses;
	hr = __effectPtr->Begin(&NumberOfPasses, 0);
	assert(D3D_OK == hr);
	for (UINT iPass = 0; iPass < NumberOfPasses; ++iPass) {
		hr = __effectPtr->BeginPass(iPass);
		assert(D3D_OK == hr);
		hr = __devicePtr->SetVertexDeclaration(__vertexDeclarationPtr);
		assert(D3D_OK == hr);
		hr = __devicePtr->SetStreamSource(0, __colorBandVertexBufferPtr, 0, sizeof(D3DXVERTEX));
		assert(D3D_OK == hr);
		hr = __devicePtr->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 6);
		assert(D3D_OK == hr);
		hr = __effectPtr->EndPass();
		assert(S_OK == hr);
	}
	hr = __effectPtr->End();
	assert(D3D_OK == hr);
}
#endif	// 렌더링

#if 1	// indicator
	for (int n = 0; n < NUM_INDICATORS; ++n) {
		const kColorBand::Indicator&	indicator = indicators[n];
		float	f0 = indicator.f0;
		float	scale = indicator.scale;

		if (f0 < 0.f || f0 > 1.f)
			continue;

#if 1	// fill buffer
		D3DXVECTOR2&	v0 = __indicatorTwoPoints[0];
		SetXYInProjectionSpace(0, 0, x_offset, unit_width, v0.x, y_offset, unit_height, v0.y);
		v0.x -= scale * unit_width;
		v0.y -= 3 * unit_height * f0;
		ProjectionSpaceToScreenSpace(v0.x, W, v0.x, v0.y, H, v0.y);

		D3DXVECTOR2&	v1 = __indicatorTwoPoints[1];
		SetXYInProjectionSpace(1, 0, x_offset, unit_width, v1.x, y_offset, unit_height, v1.y);
		v1.x += scale * unit_width;
		v1.y -= 3 * unit_height * f0;
		ProjectionSpaceToScreenSpace(v1.x, W, v1.x, v1.y, H, v1.y);
#endif	// fill buffer

#if 1	// 렌더링
		hr = __indicatorLinePtr->SetWidth(indicator.width);
		hr = __indicatorLinePtr->Begin();
		hr = __indicatorLinePtr->Draw(__indicatorTwoPoints, 2, indicator.color);
		hr = __indicatorLinePtr->End();
#endif	// 렌더링
	}
#endif	//indicator
}	// COLOR_BAND_TOP_TO_BOTTOM

if (type == COLOR_BAND_BOTTOM_TO_TOP) {
/*
#if 1	// IKC
	|
	|
	V (k)
#endif	// IKC
	+--+ +	(1, 1, 1)	(c = 3)
	| / /|
	|/ / |
	+ +--+	(1, 1, 0)	(c = 2)
	+--+ +
	| / /|
	|/ / |
	+ +--+	(1, 0, 0)	(c = 1)
	+--+ +
	| / /|
	|/ / |
	+ +--+	(0, 0, 0)	(c = 0)
#if 1	// IKC
	k + c = 3 (일정)
#endif	// IKC
 */
//	float	x_offset = .3f;
//	float	y_offset = .4f;
//	float	width = .1f;
//	float	height = .2f;
#if 1	// fill buffer
{
	IKC	ikc0[] = {{0, 0, 3}, {1, 0, 3}, {0, 1, 2}};
	IKC	ikc1[] = {{1, 0, 3}, {1, 1, 2}, {0, 1, 2}};

	D3DXVERTEX*	colorBandVertexArray;
	__colorBandVertexBufferPtr->Lock(0, 0, (void**) &colorBandVertexArray, 0);
	int	index = 0;
	for (int k = 0; k < 3; ++k) {
		// ikc0
		for (int i = 0; i < 3; ++i) {
			D3DXVERTEX&	v = colorBandVertexArray[index++];
			const IKC&	ikc = ikc0[i];

			SetXYInProjectionSpace(ikc.i, k + ikc.k, x_offset, unit_width, v.p.x, y_offset, unit_height, v.p.y);
			int	c = ikc.c - k;
			v.c = c == 3 ? D3DXVECTOR3(1, 1, 1) : (c == 2 ? D3DXVECTOR3(1, 1, 0) : (c == 1 ? D3DXVECTOR3(1, 0, 0) : D3DXVECTOR3(0, 0, 0)));
		}
		// ikc1
		for (int i = 0; i < 3; ++i) {
			D3DXVERTEX&	v = colorBandVertexArray[index++];
			const IKC&	ikc = ikc1[i];

			SetXYInProjectionSpace(ikc.i, k + ikc.k, x_offset, unit_width, v.p.x, y_offset, unit_height, v.p.y);
			int	c = ikc.c - k;
			v.c = c == 3 ? D3DXVECTOR3(1, 1, 1) : (c == 2 ? D3DXVECTOR3(1, 1, 0) : (c == 1 ? D3DXVECTOR3(1, 0, 0) : D3DXVECTOR3(0, 0, 0)));
		}
	}

	__colorBandVertexBufferPtr->Unlock();
}
#endif	// fill buffer

#if 1	// 렌더링
{
	// kEnum.h 에 있는 SHADING_TYPE 와
	// mesh.fx 에 있는 g__texture 가 같은 상수를 사용하도록 맞춰야 한다!
	HRESULT	hr;
	hr = __effectPtr->SetInt("g__texture", SHADING_NONE);
	assert(hr == D3D_OK);
//	__effectPtr->SetInt("g__overlap", __overlap);
	__effectPtr->SetInt("g__FillMode", D3DFILL_SOLID);
//	__effectPtr->SetInt("g__FillMode", D3DFILL_POINT);
//	__effectPtr->SetVector("pointColor", &D3DXVECTOR4(1, 1, 0, 1));
	D3DXMATRIX	identity;
	D3DXMatrixIdentity(&identity);
	__effectPtr->SetMatrix("g_mWorldViewProjection", &identity);
//	__effectPtr->SetMatrix("g_mWorld", &__worldMatrix);
	D3DXHANDLE	hTechnique = __effectPtr->GetTechnique(0);
	assert(hTechnique);
	hr = __effectPtr->SetTechnique(hTechnique);
	assert(S_OK == hr);
	UINT	NumberOfPasses;
	hr = __effectPtr->Begin(&NumberOfPasses, 0);
	assert(D3D_OK == hr);
	for (UINT iPass = 0; iPass < NumberOfPasses; ++iPass) {
		hr = __effectPtr->BeginPass(iPass);
		assert(D3D_OK == hr);
		hr = __devicePtr->SetVertexDeclaration(__vertexDeclarationPtr);
		assert(D3D_OK == hr);
		hr = __devicePtr->SetStreamSource(0, __colorBandVertexBufferPtr, 0, sizeof(D3DXVERTEX));
		assert(D3D_OK == hr);
		hr = __devicePtr->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 6);
		assert(D3D_OK == hr);
		hr = __effectPtr->EndPass();
		assert(S_OK == hr);
	}
	hr = __effectPtr->End();
	assert(D3D_OK == hr);
}
#endif	// 렌더링

#if 1	//indicator
	for (int n = 0; n < NUM_INDICATORS; ++n) {
		const kColorBand::Indicator&	indicator = indicators[n];
		float	f0 = indicator.f0;
		float	scale = indicator.scale;

		if (f0 < 0.f || f0 > 1.f)
			continue;

#if 1	// fill buffer
		D3DXVECTOR2&	v0 = __indicatorTwoPoints[0];
		SetXYInProjectionSpace(0, 0, x_offset, unit_width, v0.x, y_offset, unit_height, v0.y);
		v0.x -= scale * unit_width;
		v0.y -= 3 * unit_height * (1 - f0);
		ProjectionSpaceToScreenSpace(v0.x, W, v0.x, v0.y, H, v0.y);

		D3DXVECTOR2&	v1 = __indicatorTwoPoints[1];
		SetXYInProjectionSpace(1, 0, x_offset, unit_width, v1.x, y_offset, unit_height, v1.y);
		v1.x += scale * unit_width;
		v1.y -= 3 * unit_height * (1 - f0);
		ProjectionSpaceToScreenSpace(v1.x, W, v1.x, v1.y, H, v1.y);
#endif	// fill buffer

#if 1	// 렌더링
		hr = __indicatorLinePtr->SetWidth(indicator.width);
		hr = __indicatorLinePtr->Begin();
		hr = __indicatorLinePtr->Draw(__indicatorTwoPoints, 2, indicator.color);
		hr = __indicatorLinePtr->End();
#endif	// 렌더링
	}
#endif	//indicator
}	// COLOR_BAND_BOTTOM_TO_TOP

if (type == COLOR_BAND_LEFT_TO_RIGHT) {
/*
#if 1	// IKC
	---> (i)
#endif	// IKC
	+--+ + +--+ + +--+ +
	| / /| | / /| | / /|
	|/ / | |/ / | |/ / |
	+ +--+ + +--+ + +--+

	0      1      1    1
	0      0      1    1
	0      0      0    1
#if 1	// IKC
	(0)    (1)    (2)  (3)

	i + c = 3 (일정)
#endif
 */
//	float	x_offset = .1f;
//	float	y_offset = 1.4f;
//	float	width = .2f;
//	float	height = .1f;
#if 1	// fill buffer
{
	IKC	ikc0[3] = {{0, 0, 0}, {1, 0, 1}, {0, 1, 0}};
	IKC	ikc1[3] = {{1, 0, 1}, {1, 1, 1}, {0, 1, 0}};

	D3DXVERTEX*	colorBandVertexArray;
	__colorBandVertexBufferPtr->Lock(0, 0, (void**) &colorBandVertexArray, 0);
	int	index = 0;
	for (int i = 0; i < 3; ++i) {
		// ikc0
		for (int k = 0; k < 3; ++k) {
			D3DXVERTEX&	v = colorBandVertexArray[index++];
			const IKC&	ikc = ikc0[k];

			SetXYInProjectionSpace(i + ikc.i, ikc.k, x_offset, unit_width, v.p.x, y_offset, unit_height, v.p.y);
			int	c = ikc.c + i;
			v.c = c == 3 ? D3DXVECTOR3(1, 1, 1) : (c == 2 ? D3DXVECTOR3(1, 1, 0) : (c == 1 ? D3DXVECTOR3(1, 0, 0) : D3DXVECTOR3(0, 0, 0)));
		}
		// ikc1
		for (int k = 0; k < 3; ++k) {
			D3DXVERTEX&	v = colorBandVertexArray[index++];
			const IKC&	ikc = ikc1[k];

			SetXYInProjectionSpace(i + ikc.i, ikc.k, x_offset, unit_width, v.p.x, y_offset, unit_height, v.p.y);
			int	c = ikc.c + i;
			v.c = c == 3 ? D3DXVECTOR3(1, 1, 1) : (c == 2 ? D3DXVECTOR3(1, 1, 0) : (c == 1 ? D3DXVECTOR3(1, 0, 0) : D3DXVECTOR3(0, 0, 0)));
		}
	}
	__colorBandVertexBufferPtr->Unlock();
}
#endif	// fill buffer

#if 1	// 렌더링
{
	// kEnum.h 에 있는 SHADING_TYPE 와
	// mesh.fx 에 있는 g__texture 가 같은 상수를 사용하도록 맞춰야 한다!
	HRESULT	hr;
	hr = __effectPtr->SetInt("g__texture", SHADING_NONE);
	assert(hr == D3D_OK);
//	__effectPtr->SetInt("g__overlap", __overlap);
	__effectPtr->SetInt("g__FillMode", D3DFILL_SOLID);
//	__effectPtr->SetInt("g__FillMode", D3DFILL_POINT);
//	__effectPtr->SetVector("pointColor", &D3DXVECTOR4(1, 1, 0, 1));
	D3DXMATRIX	identity;
	D3DXMatrixIdentity(&identity);
	__effectPtr->SetMatrix("g_mWorldViewProjection", &identity);
//	__effectPtr->SetMatrix("g_mWorld", &__worldMatrix);
	D3DXHANDLE	hTechnique = __effectPtr->GetTechnique(0);
	assert(hTechnique);
	hr = __effectPtr->SetTechnique(hTechnique);
	assert(S_OK == hr);
	UINT	NumberOfPasses;
	hr = __effectPtr->Begin(&NumberOfPasses, 0);
	assert(D3D_OK == hr);
	for (UINT iPass = 0; iPass < NumberOfPasses; ++iPass) {
		hr = __effectPtr->BeginPass(iPass);
		assert(D3D_OK == hr);
		hr = __devicePtr->SetVertexDeclaration(__vertexDeclarationPtr);
		assert(D3D_OK == hr);
		hr = __devicePtr->SetStreamSource(0, __colorBandVertexBufferPtr, 0, sizeof(D3DXVERTEX));
		assert(D3D_OK == hr);
		hr = __devicePtr->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 6);
		assert(D3D_OK == hr);
		hr = __effectPtr->EndPass();
		assert(S_OK == hr);
	}
	hr = __effectPtr->End();
	assert(D3D_OK == hr);
}
#endif	// 렌더링

#if 1	//indicator
	for (int n = 0; n < NUM_INDICATORS; ++n) {
		const kColorBand::Indicator&	indicator = indicators[n];
		float	f0 = indicator.f0;
		float	scale = indicator.scale;

		if (f0 < 0.f || f0 > 1.f)
			continue;

#if 1	// fill buffer
		D3DXVECTOR2&	v0 = __indicatorTwoPoints[0];
		SetXYInProjectionSpace(0, 0, x_offset, unit_width, v0.x, y_offset, unit_height, v0.y);
		v0.x += 3 * unit_width * f0;
		v0.y += scale * unit_height;
		ProjectionSpaceToScreenSpace(v0.x, W, v0.x, v0.y, H, v0.y);

		D3DXVECTOR2&	v1 = __indicatorTwoPoints[1];
		SetXYInProjectionSpace(0, 1, x_offset, unit_width, v1.x, y_offset, unit_height, v1.y);
		v1.x += 3 * unit_width * f0;
		v1.y -= scale * unit_height;
		ProjectionSpaceToScreenSpace(v1.x, W, v1.x, v1.y, H, v1.y);
#endif	// fill buffer

#if 1	// 렌더링
		hr = __indicatorLinePtr->SetWidth(indicator.width);
		hr = __indicatorLinePtr->Begin();
		hr = __indicatorLinePtr->Draw(__indicatorTwoPoints, 2, indicator.color);
		hr = __indicatorLinePtr->End();
#endif	// 렌더링
	}
#endif	//indicator
}	// COLOR_BAND_LEFT_TO_RIGHT

if (type == COLOR_BAND_RIGHT_TO_LEFT) {
/*
#if 1	// IKC
	---> (i)
#endif	// IKC
	+--+ + +--+ + +--+ +
	| / /| | / /| | / /|
	|/ / | |/ / | |/ / |
	+ +--+ + +--+ + +--+

	1      1      1    0
	1      1      0    0
	1      0      0    0
#if 1	// IKC
	---> (c)
	(3)    (2)    (1)  (0)

	i + c = 3 (일정)
#endif	// IKC
 */
//	float	x_offset = .1f;
//	float	y_offset = 1.6f;
//	float	width = .2f;
//	float	height = .1f;
#if 1	// fill buffer
{
	IKC	ikc0[3] = {{0, 0, 3}, {1, 0, 2}, {0, 1, 3}};
	IKC	ikc1[3] = {{1, 0, 2}, {1, 1, 2}, {0, 1, 3}};

	D3DXVERTEX*	colorBandVertexArray;
	__colorBandVertexBufferPtr->Lock(0, 0, (void**) &colorBandVertexArray, 0);
	int	index = 0;
	for (int i = 0; i < 3; ++i) {
		// ikc0
		for (int k = 0; k < 3; ++k) {
			D3DXVERTEX&	v = colorBandVertexArray[index++];
			const IKC&	ikc = ikc0[k];

			SetXYInProjectionSpace(i + ikc.i, ikc.k, x_offset, unit_width, v.p.x, y_offset, unit_height, v.p.y);
			int	c = ikc.c - i;
			v.c = c == 3 ? D3DXVECTOR3(1, 1, 1) : (c == 2 ? D3DXVECTOR3(1, 1, 0) : (c == 1 ? D3DXVECTOR3(1, 0, 0) : D3DXVECTOR3(0, 0, 0)));
		}
		// ikc1
		for (int k = 0; k < 3; ++k) {
			D3DXVERTEX&	v = colorBandVertexArray[index++];
			const IKC&	ikc = ikc1[k];

			SetXYInProjectionSpace(i + ikc.i, ikc.k, x_offset, unit_width, v.p.x, y_offset, unit_height, v.p.y);
			int	c = ikc.c - i;
			v.c = c == 3 ? D3DXVECTOR3(1, 1, 1) : (c == 2 ? D3DXVECTOR3(1, 1, 0) : (c == 1 ? D3DXVECTOR3(1, 0, 0) : D3DXVECTOR3(0, 0, 0)));
		}
	}
	__colorBandVertexBufferPtr->Unlock();
}
#endif	// fill buffer

#if 1	// 렌더링
{
	// kEnum.h 에 있는 SHADING_TYPE 와
	// mesh.fx 에 있는 g__texture 가 같은 상수를 사용하도록 맞춰야 한다!
	HRESULT	hr;
	hr = __effectPtr->SetInt("g__texture", SHADING_NONE);
	assert(hr == D3D_OK);
//	__effectPtr->SetInt("g__overlap", __overlap);
	__effectPtr->SetInt("g__FillMode", D3DFILL_SOLID);
//	__effectPtr->SetInt("g__FillMode", D3DFILL_POINT);
//	__effectPtr->SetVector("pointColor", &D3DXVECTOR4(1, 1, 0, 1));
	D3DXMATRIX	identity;
	D3DXMatrixIdentity(&identity);
	__effectPtr->SetMatrix("g_mWorldViewProjection", &identity);
//	__effectPtr->SetMatrix("g_mWorld", &__worldMatrix);
	D3DXHANDLE	hTechnique = __effectPtr->GetTechnique(0);
	assert(hTechnique);
	hr = __effectPtr->SetTechnique(hTechnique);
	assert(S_OK == hr);
	UINT	NumberOfPasses;
	hr = __effectPtr->Begin(&NumberOfPasses, 0);
	assert(D3D_OK == hr);
	for (UINT iPass = 0; iPass < NumberOfPasses; ++iPass) {
		hr = __effectPtr->BeginPass(iPass);
		assert(D3D_OK == hr);
		hr = __devicePtr->SetVertexDeclaration(__vertexDeclarationPtr);
		assert(D3D_OK == hr);
		hr = __devicePtr->SetStreamSource(0, __colorBandVertexBufferPtr, 0, sizeof(D3DXVERTEX));
		assert(D3D_OK == hr);
		hr = __devicePtr->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 6);
		assert(D3D_OK == hr);
		hr = __effectPtr->EndPass();
		assert(S_OK == hr);
	}
	hr = __effectPtr->End();
	assert(D3D_OK == hr);
}
#endif	// 렌더링

#if 1	//indicator
	for (int n = 0; n < NUM_INDICATORS; ++n) {
		const kColorBand::Indicator&	indicator = indicators[n];
		float	f0 = indicator.f0;
		float	scale = indicator.scale;

		if (f0 < 0.f || f0 > 1.f)
			continue;

#if 1	// fill buffer
		D3DXVECTOR2&	v0 = __indicatorTwoPoints[0];
		SetXYInProjectionSpace(0, 0, x_offset, unit_width, v0.x, y_offset, unit_height, v0.y);
		v0.x += 3 * unit_width * (1 - f0);
		v0.y += scale * unit_height;
		ProjectionSpaceToScreenSpace(v0.x, W, v0.x, v0.y, H, v0.y);

		D3DXVECTOR2&	v1 = __indicatorTwoPoints[1];
		SetXYInProjectionSpace(0, 1, x_offset, unit_width, v1.x, y_offset, unit_height, v1.y);
		v1.x += 3 * unit_width * (1 - f0);
		v1.y -= scale * unit_height;
		ProjectionSpaceToScreenSpace(v1.x, W, v1.x, v1.y, H, v1.y);
#endif	// fill buffer

#if 1	// 렌더링
		hr = __indicatorLinePtr->SetWidth(indicator.width);
		hr = __indicatorLinePtr->Begin();
		hr = __indicatorLinePtr->Draw(__indicatorTwoPoints, 2, indicator.color);
		hr = __indicatorLinePtr->End();
#endif	// 렌더링
	}
#endif	//indicator
}	// COLOR_BAND_RIGHT_TO_LEFT
}
#endif	// ColorBand

int
kMesh::ShowFace(
	const LPDIRECT3DINDEXBUFFER9	faceIndexBufferPtr,
	DWORD				nFace,
	SHADING_TYPE	shadingType,
	int		fillMode,
	D3DXVECTOR4	color
) const
{
	HRESULT	hResult;

	// Set effect variables as needed
#if 0
//	D3DXCOLOR	colorMtrl(1.0f, 1.0f, 1.0f, 1.0f);
	D3DXCOLOR	colorMtrl(1, 1, 1, 1);
	D3DXVECTOR3	vLightDir(.1f, -1.f, .1f);
	D3DXCOLOR	vLightDiffuse(1, 1, 1, 1);
	__effectPtr->SetValue("g_MaterialAmbientColor", &colorMtrl, sizeof(D3DXCOLOR));
	__effectPtr->SetValue("g_MaterialDiffuseColor", &colorMtrl, sizeof(D3DXCOLOR));
	__effectPtr->SetValue("g_LightDir", &vLightDir, sizeof(D3DXVECTOR3));
	__effectPtr->SetValue("g_LightDiffuse", &vLightDiffuse, sizeof(D3DXVECTOR4));
#endif

	// kEnum.h 에 있는 SHADING_TYPE 와
	// mesh.fx 에 있는 g__texture 가 같은 상수를 사용하도록 맞춰야 한다!
	__effectPtr->SetInt("g__texture", shadingType);
	__effectPtr->SetInt("g__overlap", __overlap);
	__effectPtr->SetInt("g__FillMode", fillMode);
	__effectPtr->SetVector("pointColor", &color);
	__effectPtr->SetMatrix("g_mWorldViewProjection", &__worldViewProjectionMatrix);
	__effectPtr->SetMatrix("g_mWorld", &__worldMatrix);
	D3DXHANDLE	hTechnique = __effectPtr->GetTechnique(0);
	assert(hTechnique);
	hResult = __effectPtr->SetTechnique(hTechnique);
	assert(S_OK == hResult);
	UINT	NumberOfPasses;
	hResult = __effectPtr->Begin(&NumberOfPasses, 0);
	assert(D3D_OK == hResult);
	for (UINT iPass = 0; iPass < NumberOfPasses; ++iPass) {
		hResult = __effectPtr->BeginPass(iPass);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetVertexDeclaration(__vertexDeclarationPtr);
		assert(D3D_OK == hResult);
		// 인덱스 버퍼를 만들 때, sizeof(D3DXVECTOR3) 기준
		hResult = __devicePtr->SetStreamSource(0, __vertexBufferPtr, 0, sizeof(D3DXVECTOR3));
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetIndices(faceIndexBufferPtr);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, N_ELEMENT * __numPointReprs, 0, nFace);
		assert(D3D_OK == hResult);
		hResult = __effectPtr->EndPass();
		assert(S_OK == hResult);
	}
	hResult = __effectPtr->End();
	assert(D3D_OK == hResult);

	return 0;
}
int
kMesh::ShowSubsetFace() const
{
	const LPD3DXMESH	meshSubsetPtr = __meshLoader.meshSubsetPtr();
	if (!meshSubsetPtr) {
		(void) fprintf(stderr, "ERROR: kMeshLoader::__meshSubsetPtr is nil\n");
		return -1;
	}

	HRESULT	hResult;
	D3DXCOLOR	colorMtrl_saved;
	__effectPtr->GetValue("g_MaterialAmbientColor", &colorMtrl_saved, sizeof(D3DXCOLOR));

	DWORD	numSubsets;
	meshSubsetPtr->GetAttributeTable(NULL, &numSubsets);
	D3DXATTRIBUTERANGE*	attributeTable = 0;
do {
	attributeTable = new D3DXATTRIBUTERANGE[numSubsets];
	meshSubsetPtr->GetAttributeTable(attributeTable, &numSubsets);
#define COLOR_COUNT	8
	FLOAT	ColorList[COLOR_COUNT][4] = {
		{1.0f, 0.5f, 0.5f, 1.f},
		{0.5f, 1.0f, 0.5f, 1.f},
		{1.0f, 1.0f, 0.5f, 1.f},
		{0.5f, 1.0f, 1.0f, 1.f},
		{1.0f, 0.5f,0.75f, 1.f},
		{0.0f, 0.5f, 0.75f, 1.f},
		{0.5f, 0.5f, 0.75f, 1.f},
		{0.5f, 0.5f, 1.0f, 1.f},
	};
for (DWORD iSubset = 0; iSubset < numSubsets; ++iSubset) {
	LPDIRECT3DBASETEXTURE9	texturePtrSaved = 0;
	LPDIRECT3DBASETEXTURE9	texturePtr = 0;
	if (__model.getShadingRef() == SHADING_TEXTURE) {
		const kMeshLoader::Material&	material = *__meshLoader.GetMaterials().GetAt(attributeTable[iSubset].AttribId);
		texturePtr = material.texturePtr;
	}

	// Set effect variables as needed
	__effectPtr->SetInt("g__texture", __model.getShadingRef());
	if (texturePtr) {
		__effectPtr->SetTexture("g_MeshTexture", texturePtr);
	} else {	// 텍스쳐를 사용할 수 없거나 사용하지 않는다면 각 부위(Subset) 별로 다른 색을 사용한다.
		__effectPtr->SetInt("g__texture", SHADING_NONE);

		D3DXCOLOR	colorMtrl;
		colorMtrl.r = ColorList[iSubset % COLOR_COUNT][0];
		colorMtrl.g = ColorList[iSubset % COLOR_COUNT][1];
		colorMtrl.b = ColorList[iSubset % COLOR_COUNT][2];
		colorMtrl.a = ColorList[iSubset % COLOR_COUNT][3];
#if 0
//	D3DXCOLOR	colorMtrl(1.0f, 1.0f, 1.0f, 1.0f);
	D3DXCOLOR	colorMtrl(1, 1, 1, 1);
	D3DXVECTOR3	vLightDir(.1f, -1.f, .1f);
	D3DXCOLOR	vLightDiffuse(1, 1, 1, 1);
	__effectPtr->SetValue("g_MaterialAmbientColor", &colorMtrl, sizeof(D3DXCOLOR));
	__effectPtr->SetValue("g_MaterialDiffuseColor", &colorMtrl, sizeof(D3DXCOLOR));
	__effectPtr->SetValue("g_LightDir", &vLightDir, sizeof(D3DXVECTOR3));
	__effectPtr->SetValue("g_LightDiffuse", &vLightDiffuse, sizeof(D3DXVECTOR4));
#endif
		__effectPtr->SetValue("g_MaterialAmbientColor", &colorMtrl, sizeof(D3DXCOLOR));
		__effectPtr->SetValue("g_MaterialDiffuseColor", &colorMtrl, sizeof(D3DXCOLOR));
	}
	__effectPtr->SetInt("g__FillMode", D3DFILL_SOLID);
	__effectPtr->SetMatrix("g_mWorldViewProjection", &__worldViewProjectionMatrix);
	__effectPtr->SetMatrix("g_mWorld", &__worldMatrix);

	D3DXHANDLE	hTechnique = __effectPtr->GetTechnique(0);
	assert(hTechnique);
	hResult = __effectPtr->SetTechnique(hTechnique);
	assert(S_OK == hResult);
	UINT	NumberOfPasses;
	hResult = __effectPtr->Begin(&NumberOfPasses, 0);
	assert(D3D_OK == hResult);
	for (UINT iPass = 0; iPass < NumberOfPasses; ++iPass) {
		hResult = __effectPtr->BeginPass(iPass);
		assert(D3D_OK == hResult);

		meshSubsetPtr->DrawSubset(attributeTable[iSubset].AttribId);

		hResult = __effectPtr->EndPass();
		assert(S_OK == hResult);
	}
	hResult = __effectPtr->End();
	assert(D3D_OK == hResult);

	texturePtr = 0;
	if (texturePtrSaved) {
		__effectPtr->SetTexture("g_MeshTexture", texturePtrSaved);
		texturePtrSaved = 0;
	}
}
#undef COLOR_COUNT
} while (0);
	if (attributeTable) {
		delete[] attributeTable;
		attributeTable = 0;
	}
	__effectPtr->SetValue("g_MaterialAmbientColor", &colorMtrl_saved, sizeof(D3DXCOLOR));

	return 0;
}

void	kMesh::__calculateVolumeChange(
	kMesh::D3DXVERTEX*	pV,
	DWORD*	pPointReprFlag
)
{
	DWORD*	pFace = new DWORD[__numFaces];
	FillMemory(pFace, __numFaces * sizeof(DWORD), 0);

	for (DWORD i = 0; i < __numPointReprs; ++i) {
		if (pPointReprFlag && pPointReprFlag[i] == 0)
			continue;

#ifdef __USE_A_OFFSET__
		for (int k = pointer_to__a_vFaceIDLocalIndex_a_offset[i]; k < pointer_to__a_vFaceIDLocalIndex_a_offset[i + 1]; ++k) {
			const FACE_ID_LOCAL_INDEX&	faceIDLocalIndex = pointer_to__a_vFaceIDLocalIndex[k];
			pFace[faceIDLocalIndex.ID] = 1;
		}
#else
		for (size_t k = 0; k < __pointReprPtr[i].vFaceIdLocalIndex.size(); ++k) {
			pFace[__pointReprPtr[i].vFaceIdLocalIndex.at(k).ID] = 1;
		}
#endif
	}

	FLOAT	fVolume_ = 0.f;	// p_ 를 사용해 계산된 Volume
	FLOAT	fVolume = 0.f;	// p 를 사용해 계산된 Volume
	for (DWORD i = 0; i < __numFaces; ++i) {
		if (pFace[i] == 0)
			continue;

		CFace&	face = __facePtr[i];
#if 1
		DWORD	index0 = face.prID0;
		DWORD	index1 = face.prID1;
		DWORD	index2 = face.prID2;
#else
		DWORD	index0 = this->m__pPointReprIDMapped[face.prID0];
		DWORD	index1 = this->m__pPointReprIDMapped[face.prID1];
		DWORD	index2 = this->m__pPointReprIDMapped[face.prID2];
#endif
		D3DXVECTOR3	cross_;
		fVolume_ += D3DXVec3Dot(&pV[index0].p_, D3DXVec3Cross(&cross_, &pV[index1].p_, &pV[index2].p_));
		D3DXVECTOR3	cross;
		fVolume += D3DXVec3Dot(&pV[index0].p, D3DXVec3Cross(&cross, &pV[index1].p, &pV[index2].p));
	}
	this->__fVolumeChange = (fVolume - fVolume_) / 6.f;

	if (pFace) {
		delete[] pFace;
		pFace = NULL;
	}
}

void
kMesh::CalculateTotalVolume(
	bool	use_p_,
	float*	vol_ptr
)
{
	kMesh::D3DXVERTEX*	pV;
	__vertexBufferPtr->Lock(0, 0, (void**) &pV, 0);

	double	fTotalVolume = 0;
	for (DWORD i = 0; i < __numFaces; ++i) {
		CFace&	face = __facePtr[i];
#if 1
		DWORD	index0 = face.prID0;
		DWORD	index1 = face.prID1;
		DWORD	index2 = face.prID2;
#else
		DWORD	index0 = this->m__pPointReprIDMapped[face.prID0];
		DWORD	index1 = this->m__pPointReprIDMapped[face.prID1];
		DWORD	index2 = this->m__pPointReprIDMapped[face.prID2];
#endif
		D3DXVECTOR3	cross_;
		if (use_p_)
			fTotalVolume += D3DXVec3Dot(&pV[index0].p_, D3DXVec3Cross(&cross_, &pV[index1].p_, &pV[index2].p_));
		else
			fTotalVolume += D3DXVec3Dot(&pV[index0].p, D3DXVec3Cross(&cross_, &pV[index1].p, &pV[index2].p));
	}
	if (vol_ptr)
		*vol_ptr = (float) fTotalVolume / 6.f;
	else
		m__fTotalVolume0 = (float) fTotalVolume / 6.f;

	__vertexBufferPtr->Unlock();
}


#if 0
void
kMesh::Tessellation(
	DWORD	nTessellation,
	DWORD*	pTessellationFlag
)
{
	if (!nTessellation)
		return;

	//pTessellationFlag = new DWORD[g__pMesh->GetNFace()];
	// 몇 개의 새로운 (Vertex, Edge, Face)가 필요한 지 알아본다;
	// 추가되는 Face 의 갯수는 Tessellated 되는 Face 의 갯수의 "정확히" 세배이나
	// 나머지의 갯수는 Tessellated 되는 Face 와 인접하는 Face 가
	// 이미 Tessellated 되었는 지에 따라 달라진다.
	DWORD	nAddPointRepr = 0;
	DWORD	nAddEdge = 0;
	DWORD	nAddFace = 0;
	// 새로운 (Vertex, Edge, Face)의 각 (Vertex Buffer, Line Index Buffer, Face Index Buffer) 에 대한 인덱스를 미리 얻는다;
	// offsetXxxArray 는 Tessellated 되는 Face 가 기존의 Buffer 에서 얼마나 떨어져있는 지를 말해주고,
	// indexXxxArray 는 주어진 Face 의 내에서 각 Xxx 가 몇 번째에 위치하는 지를 말해준다:
	// iFace 번째의 Face 가 Tessellated 된다면,
	// 추가되는 j 번째 Vertex 의 기존 Buffer 로부터의 인덱스는
	//	offsetPointReprArray[3 * iFace] + indexPointReprArray[j]
	// 가 된다.
	// -1 은 해당 Face 가 Tessellated 되지 않거나 (offsetXxxArray)
	// 해당 Xxx 가 새롭게 추가되는 Xxx 가 아님을 뜻한다. (indexXxxArray)
	DWORD*	offsetPointReprArray = new DWORD[__numFaces];
	memset(offsetPointReprArray, -1, __numFaces * sizeof(DWORD));
	DWORD*	offsetEdgeArray = new DWORD[__numFaces];
	memset(offsetEdgeArray, -1, __numFaces * sizeof(DWORD));
	for (DWORD iFace = 0; iFace < __numFaces; ++iFace) {
		if (pTessellationFlag[iFace] != 1)
			continue;

		int	indexPointRepr = 0;
		int	indexEdge = 0;
		CFace&	face = __facePtr[iFace];
		for (int i = 0; i < 3; ++i) {
			if (face.__ajID[i] == (DWORD) -1) {
				++indexPointRepr;
				++nAddPointRepr;
				++indexEdge;
				++nAddEdge;
			}
		}
		offsetPointReprArray[iFace] = indexPointRepr;
		offsetEdgeArray[iFace] = indexEdge;

		nAddEdge += 3;	// 내부에 만들어지는 삼각형의 세 변은 항상 새로운 Edge 이다.
		nAddFace += 3;
	}

	CPointRepr*	pPointRepr = new CPointRepr[__numPointReprs + nAddPointRepr];
	CEdge*		pEdge = new CEdge[__numEdges + nAddEdge];
	CFace*		pFace = new CFace[__numFaces + nAddFace];
	// 먼저 기존의 (CPointRepr, CEdge, CFace) 를 복사한다.
	for (DWORD i = 0; i < __numPointReprs; ++i)
		pPointRepr[i] = __pointReprPtr[i];
	for (DWORD i = 0; i < __numEdges; ++i)
		pEdge[i] = __edgePtr[i];
	for (DWORD i = 0; i < __numFaces; ++i)
		pFace[i] = __facePtr[i];

	// 새로운 Vertex Buffer 를 만든다.
	LPDIRECT3DVERTEXBUFFER9	pVertexBuffer = NULL;
	__devicePtr->CreateVertexBuffer(
		(__numPointReprs + nAddPointRepr) * sizeof(kMesh::D3DXVERTEX),
		D3DUSAGE_WRITEONLY,
		0,
		D3DPOOL_MANAGED,
		&pVertexBuffer,
		NULL
	);
	// 기존 Vertex Buffer 의 내용을 복사한다.
	kMesh::D3DXVERTEX*	__pVertices;
	__vertexBufferPtr->Lock(0, 0, (void**) &__pVertices, 0);
	kMesh::D3DXVERTEX*	pVertices;
	pVertexBuffer->Lock(0, 0, (void**) &pVertices, 0);
	memcpy(pVertices, __pVertices, __numPointReprs * sizeof(kMesh::D3DXVERTEX));

	// 새로운 Line Index Buffer 를 만든다.
	LPDIRECT3DINDEXBUFFER9		pLineIndexBuffer = NULL;
	__devicePtr->CreateIndexBuffer(
		(__numEdges + nAddEdge) * sizeof(CEdge::LineIndex),
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX32,
		D3DPOOL_MANAGED,
		&pLineIndexBuffer,
		NULL
	);
	// 기존 Line Index Buffer 내용을 복사한다.
	CEdge::LineIndex*	__pLineIndices;
	__edgeIndexBufferPtr->Lock(0, 0, (void**) &__pLineIndices, 0);
	CEdge::LineIndex*	pLineIndices;
	pLineIndexBuffer->Lock(0, 0, (void**) &pLineIndices, 0);
	memcpy(pLineIndices, __pLineIndices, __numEdges * sizeof(CEdge::LineIndex));
/*
	for (DWORD iEdge = 0; iEdge < nEdge; ++iEdge) {
		CEdge&	edge = __edgePtr[iEdge];

		DWORD	iStart = edge.prStartID;
		iStart *= N_ELEMENT;
		DWORD	iEnd = edge.prEndID;
		iEnd *= N_ELEMENT;
		pLineIndices[iEdge].iStart = iStart;
		pLineIndices[iEdge].iEnd = iEnd;
	}
 */

	// 새로운 Face Index Buffer 를 만든다.
	LPDIRECT3DINDEXBUFFER9		pFaceIndexBuffer = NULL;
	__devicePtr->CreateIndexBuffer(
		(__numFaces + nAddFace) * sizeof(CFace::FaceIndex),
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX32,
		D3DPOOL_MANAGED,
		&pFaceIndexBuffer,
		NULL
	);
	// 기존 Line Index Buffer 의 내용을 복사한다.
	CFace::FaceIndex*	__pFaceIndices;
	__faceIndexBufferPtr->Lock(0, 0, (void**) &__pFaceIndices, 0);
	CFace::FaceIndex*	pFaceIndices;
	pFaceIndexBuffer->Lock(0, 0, (void**) &pFaceIndices, 0);
	memcpy(pFaceIndices, __pFaceIndices, __numFaces * sizeof(CFace::FaceIndex));
/*
	for (DWORD iFace = 0; iFace < nFace; ++iFace) {
		CFace&	face = __facePtr[iFace];
		DWORD	i0 = face.prID0;
		i0 *= N_ELEMENT;
		DWORD	i1 = face.prID1;
		i1 *= N_ELEMENT;
		DWORD	i2 = face.prID2;
		i2 *= N_ELEMENT;
		pFaceIndices[iFace].i0 = i0;
		pFaceIndices[iFace].i1 = i1;
		pFaceIndices[iFace].i2 = i2;
	}
 */

	// 각 Buffer 를 채우고,
	// 이것에 맞춰 (CPointRepr, CEdge, CFace) 를 수정한다;
	// 모든 것은 Vertex Buffer 를 채우는 것에서부터 시작한다.
	// ID 에 대한 정보는 기존의 (CPointRepr, CEdge, CFace) 와
	// offsetXxxArray, indexXxxArray 를 통해 수정하거나 새로 만든다.
	DWORD	nPointRepr = __numPointReprs;
	DWORD	nEdge = __numEdges;
	DWORD	nFace = __numFaces;
	for (DWORD iFace = 0; iFace < __numFaces; ++iFace) {
		if (pTessellationFlag[iFace] != 1)
			continue;

//		int	indexPointRepr = 0;
//		int	indexEdge = 0;
		CFace&	face = __facePtr[iFace];
		DWORD	index0__ = face.prID0;
		DWORD	index1__ = face.prID1;
		DWORD	index2__ = face.prID2;
		EDGE	edgeID01 = face.edgeID01;
		EDGE	edgeID12 = face.edgeID12;
		EDGE	edgeID20 = face.edgeID20;
//		EDGE	__edgeID01 = face.__edgeID01;
//		EDGE	__edgeID12 = face.__edgeID12;
//		EDGE	__edgeID20 = face.__edgeID20;
		DWORD	ajID01 = face.ajID01;
//		DWORD	ajID12 = face.ajID12;
//		DWORD	ajID20 = face.ajID20;
		DWORD	__ajID01 = face.__ajID01;
		DWORD	__ajID12 = face.__ajID12;
		DWORD	__ajID20 = face.__ajID20;

		// 각 변의 중심에 해당하는 Vertex 에 대한 인텍스;
		// 새로운 Vertex 를 Vertex Buffer 에 추가하고 그 인덱스를 사용한다.
		// 만약 이미 중심에 해당하는 Vertex 가 있다면 그 인덱스를 사용한다.
		DWORD	index0, index1, index2;
		// 현재 iFace 번째 Face 의 Edge01 즉 0 번째 Edge 는,
		// 이웃하는 ajID01 번째 Face 의 몇 번째 Edge 인가?
		// 여기서 하는 Tessellation 방법에 따르면,
		// ajID01 번째 Face 의 이미 Tessellated 되어있다고 하다라도 마찬가지 결과를 준다.
		// 즉 __Edge01 와 이웃하는 __ajID01 번째 Face 를 보더라도
		// 같은 번째 Edge 이다.
		CFace&	ajFace0 = pFace[ajID01];
		int	iAjEdge0 = ajFace0.ajID01 == iFace ? 0 : (ajFace0.ajID12 == iFace ? 1 : (ajFace0.ajID20 == iFace ? 2 : -1));
#ifdef _DEBUG
		assert(iAjEdge0 != -1);
#else
		if (iAjEdge0 == -1)
			return;
#endif
		if (__ajID01 != (DWORD) -1) {
			// edgeID01
			CEdge&	edge1 = pEdge[edgeID01.id];
			index0 = edgeID01.bDir ? edge1.prEndID : edge1.prStartID;
			assert(index0 != index0__);
#if 0
			// 1. 기존의 edge1 과 edge2 의 CEdge::faceID 중에서 iFace 에 해당하는 것을 변경
			CEdge&	edge2 = pEdge[__edgeID01.id];
			DWORD	*pFaceID1, *pFaceID2;
			if (edge1.faceID[0] == iFace) {
				// edge1.faceID[0]/edge2.faceID[0] 을 변경
				pFaceID1 = &edge1.faceID[0];
				pFaceID2 = &edge2.faceID[0];
			} else {
				// edge1.faceID[1]/edge2.faceID[1] 을 변경
				pFaceID1 = &edge1.faceID[1];
				pFaceID2 = &edge2.faceID[1];
			}
			assert(*pFaceID1 == *pFaceID2);
//			*pFaceID1 = ;
//			*pFaceID2 = ;
			// 2. edge1/edge2 를 공유하는 두 Face 에서 해당 EDGE, ajID 를 변경;
			// EDGE::bComplete 를 true 로 변경
			// 이웃하는 두 Face 의 몇 번째 Edge 에 해당하는가?
			// (iFace 번째 Face 의 0번째 Edge 에 해당한다.)
			// Tessellation 의 과정을 보면 어느 Face 에서나 같은 번째의 Edge 에 해당한다.
			// (아래 참고)
			EDGE	*pAjEdge1, *pAjEdge2;
			DWORD	*pAjID1, *pAjID2;
			CFace&	__ajFace0 = pFace[__ajID01];
			if (ajFace0.ajID01 == iFace) {
				// 0번째
				assert(__ajFace0.ajID01 == iFace);
				pAjEdge1 = &ajFace0.edgeID01;
				pAjEdge2 = &__ajFace0.edgeID01;
				pAjID1 = &ajFace0.ajID01;
				pAjID2 = &__ajFace0.ajID01;
			} else if (ajFace0.ajID12 == iFace) {
				// 1번째
				assert(__ajFace0.ajID01 == iFace);
				pAjEdge1 = &ajFace0.edgeID12;
				pAjEdge2 = &__ajFace0.edgeID12;
				pAjID1 = &ajFace0.ajID12;
				pAjID2 = &__ajFace0.ajID12;
			} else {
				assert(ajFace0.ajID20 == iFace);
				// 2번째
				assert(__ajFace0.ajID20 == iFace);
				pAjEdge1 = &ajFace0.edgeID20;
				pAjEdge2 = &__ajFace0.edgeID20;
				pAjID1 = &ajFace0.ajID20;
				pAjID2 = &__ajFace0.ajID20;
			}
			pAjEdge1->bComplete = true;
			pAjEdge2->bComplete = true;
//			*pAjID1 = *pFaceID1;
//			*pAjID2 = *pFaceID2;
#endif
		} else {
			index0 = nPointRepr;

			CPointRepr&	pointRepr = pPointRepr[nPointRepr];
			pointRepr.id = nPointRepr;
			pointRepr.v.p = .5f * (pVertices[index0__].p + pVertices[index1__].p);
			D3DXVECTOR3	n = .5f * (pVertices[index0__].n + pVertices[index1__].n);
			D3DXVec3Normalize(&pointRepr.v.n, &n);
			// vEdgeID, vFaceID 는 아래에서...

			// kMesh::__vertexBufferPtr
			kMesh::D3DXVERTEX&	v = pVertices[nPointRepr];
			v.p = pointRepr.v.p;
			v.p_ = pointRepr.v.p;
			v.n = pointRepr.v.n;
			v.tu = 0.f;
			v.tv = 0.f;
			v.tx = 0.f;	// Tessellation
#if 1
			D3DXVECTOR3&	cColor1 = pVertices[index0__].c;
			D3DXVECTOR3&	cColor2 = pVertices[index1__].c;
			FLOAT	f1 = cColor1.x + cColor1.y + cColor1.z;
			FLOAT	f2 = cColor2.x + cColor2.y + cColor2.z;
			v.c = D3DXVECTOR3(.5f * (f1 + f2), 0.f, 0.f);
#else
//			v.c = .5f * (pVertices[index0__].c + pVertices[index1__].c);
#endif
			CLAMP2(v.c);

			++nPointRepr;

			CEdge&	edge = pEdge[nEdge];
			edge.id = nEdge;

//			++nEdge;
		}
		if (__ajID12 != (DWORD) -1) {
			// edgeID12
			CEdge&	edge = pEdge[edgeID12.id];
			index1 = edgeID12.bDir ? edge.prEndID : edge.prStartID;
			assert(index1 != index1__);
		} else {
			index1 = nPointRepr;

			CPointRepr&	pointRepr = pPointRepr[nPointRepr];
			pointRepr.id = nPointRepr;
			pointRepr.v.p = .5f * (pVertices[index1__].p + pVertices[index2__].p);
			D3DXVECTOR3	n = .5f * (pVertices[index1__].n + pVertices[index2__].n);
			D3DXVec3Normalize(&pointRepr.v.n, &n);
			// vEdgeID, vFaceID 는 아래에서...

			// kMesh::__vertexBufferPtr
			kMesh::D3DXVERTEX&	v = pVertices[nPointRepr];
			v.p = pointRepr.v.p;
			v.p_ = pointRepr.v.p;
			v.n = pointRepr.v.n;
			v.tu = 0.f;
			v.tv = 0.f;
			v.tx = 0.f;	// Tessellation
#if 1
			D3DXVECTOR3&	cColor1 = pVertices[index1__].c;
			D3DXVECTOR3&	cColor2 = pVertices[index2__].c;
			FLOAT	f1 = cColor1.x + cColor1.y + cColor1.z;
			FLOAT	f2 = cColor2.x + cColor2.y + cColor2.z;
			v.c = D3DXVECTOR3(.5f * (f1 + f2), 0.f, 0.f);
#else
			v.c = .5f * (pVertices[index1__].c + pVertices[index2__].c);
#endif
			CLAMP2(v.c);

			++nPointRepr;

//			++nEdge;
		}
		if (__ajID20 != (DWORD) -1) {
			// edgeID20
			CEdge&	edge = pEdge[edgeID20.id];
			index2 = edgeID20.bDir ? edge.prEndID : edge.prStartID;
			assert(index2 != index2__);
		} else {
			index2 = nPointRepr;

			CPointRepr&	pointRepr = pPointRepr[nPointRepr];
			pointRepr.id = nPointRepr;
			pointRepr.v.p = .5f * (pVertices[index2__].p + pVertices[index0__].p);
			D3DXVECTOR3	n = .5f * (pVertices[index2__].n + pVertices[index0__].n);
			D3DXVec3Normalize(&pointRepr.v.n, &n);
			// vEdgeID, vFaceID 는 아래에서...

			// kMesh::__vertexBufferPtr
			kMesh::D3DXVERTEX&	v = pVertices[nPointRepr];
			v.p = pointRepr.v.p;
			v.p_ = pointRepr.v.p;
			v.n = pointRepr.v.n;
			v.tu = 0.f;
			v.tv = 0.f;
			v.tx = 0.f;	// Tessellation
#if 1
			D3DXVECTOR3&	cColor1 = pVertices[index2__].c;
			D3DXVECTOR3&	cColor2 = pVertices[index0__].c;
			FLOAT	f1 = cColor1.x + cColor1.y + cColor1.z;
			FLOAT	f2 = cColor2.x + cColor2.y + cColor2.z;
			v.c = D3DXVECTOR3(.5f * (f1 + f2), 0.f, 0.f);
#else
			v.c = .5f * (pVertices[index2__].c + pVertices[index0__].c);
#endif
			CLAMP2(v.c);

			++nPointRepr;

//			++nEdge;
		}


		// CPointRepr
		// CPointRepr::vEdgeID
		// CPointRepr::vFaceID

		// CEdge

		// CFace


		// Vertex Buffer 의 해당하는 인덱스로 바꾼다.
		index0__ *= N_ELEMENT;
		index1__ *= N_ELEMENT;
		index2__ *= N_ELEMENT;
		index0 *= N_ELEMENT;
		index1 *= N_ELEMENT;
		index2 *= N_ELEMENT;
		// pLineIndices
		if (face.__ajID01 == (DWORD) -1) {
			DWORD	indexEdge = face.edgeID01.id;
			pLineIndices[indexEdge] = CEdge::LineIndex(index0__, index0);
			pLineIndices[nEdge++] = CEdge::LineIndex(index0, index1__);
		}
		if (face.__ajID12 == (DWORD) -1) {
			DWORD	indexEdge = face.edgeID01.id;
			pLineIndices[indexEdge] = CEdge::LineIndex(index1__, index1);
			pLineIndices[nEdge++] = CEdge::LineIndex(index1, index2__);
		}
		if (face.__ajID20 == (DWORD) -1) {
			DWORD	indexEdge = face.edgeID01.id;
			pLineIndices[indexEdge] = CEdge::LineIndex(index2__, index2);
			pLineIndices[nEdge++] = CEdge::LineIndex(index2, index0__);
		}
		pLineIndices[nEdge++] = CEdge::LineIndex(index0, index1);
		pLineIndices[nEdge++] = CEdge::LineIndex(index1, index2);
		pLineIndices[nEdge++] = CEdge::LineIndex(index2, index0);

		// pFaceIndices
{
	CFace&	face = pFace[iFace];
	face.prID0 = index0;
	face.prID1 = index1;
	face.prID2 = index2;
	CFace&	face0 = pFace[nFace + 0];
	face0.prID0 = index0__;
	face0.prID1 = index0;
	face0.prID2 = index2;
	CFace&	face1 = pFace[nFace + 1];
	face1.prID0 = index0;
	face1.prID1 = index1__;
	face1.prID2 = index1;
	CFace&	face2 = pFace[nFace + 2];
	face2.prID0 = index2;
	face2.prID1 = index1;
	face2.prID2 = index2__;
}
		pFaceIndices[iFace] = CFace::FaceIndex(index0, index1, index2);
		pFaceIndices[nFace++] = CFace::FaceIndex(index0__, index0, index2);
		pFaceIndices[nFace++] = CFace::FaceIndex(index0, index1__, index1);
		pFaceIndices[nFace++] = CFace::FaceIndex(index2, index1, index2__);
	}

	if (offsetPointReprArray) {
		delete[] offsetPointReprArray;
		offsetPointReprArray = NULL;
	}
	if (offsetEdgeArray) {
		delete[] offsetEdgeArray;
		offsetEdgeArray = NULL;
	}

	pFaceIndexBuffer->Unlock();
	__faceIndexBufferPtr->Unlock();
#ifdef _DEBUG
{
	ULONG	refCount = __faceIndexBufferPtr->Release();
	assert(refCount == 0);
}
#else
	__faceIndexBufferPtr->Release();
#endif
	__faceIndexBufferPtr = pFaceIndexBuffer;

	pLineIndexBuffer->Unlock();
	__edgeIndexBufferPtr->Unlock();
#ifdef _DEBUG
{
	ULONG	refCount = __edgeIndexBufferPtr->Release();
	assert(refCount == 0);
}
#else
	__edgeIndexBufferPtr->Release();
#endif
	__edgeIndexBufferPtr = pLineIndexBuffer;

	pVertexBuffer->Unlock();
	__vertexBufferPtr->Unlock();
#ifdef _DEBUG
{
	ULONG	refCount = __vertexBufferPtr->Release();
	assert(refCount == 0);
}
#else
	__vertexBufferPtr->Release();
#endif
	__vertexBufferPtr = pVertexBuffer;

	assert(nPointRepr == __numPointReprs + nAddPointRepr);
	__numPointReprs += nAddPointRepr;
	delete[] __pointReprPtr;
	__pointReprPtr = pPointRepr;
	assert(nEdge == __numEdges + nAddEdge);
	__numEdges += nAddEdge;
	delete[] __edgePtr;
	__edgePtr = pEdge;
	assert(nFace == __numFaces + nAddFace);
	__numFaces += nAddFace;
	delete[] __facePtr;
	__facePtr = pFace;
}
#endif


int
kMesh::ShowString(
	const LPDIRECT3DVERTEXBUFFER9		segmentVertexBuffer,
	const LPDIRECT3DINDEXBUFFER9		segmentIndexBuffer,
	DWORD	numSegments,
	const D3DXVECTOR4&	color,
	UINT	StartIndex
) const
{
	if (!numSegments)
		return 0;

	HRESULT	hResult;

	__effectPtr->SetInt("g__FillMode", D3DFILL_WIREFRAME);
	__effectPtr->SetMatrix("g_mWorldViewProjection", &__worldViewProjectionMatrix);
	__effectPtr->SetMatrix("g_mWorld", &__worldMatrix);
	__effectPtr->SetVector("pointColor", &color);
	D3DXHANDLE	hTechnique = __effectPtr->GetTechnique(0);
	assert(hTechnique);
	hResult = __effectPtr->SetTechnique(hTechnique);
	assert(S_OK == hResult);
	UINT	NumberOfPasses;
	assert(D3D_OK == hResult);
	hResult = __effectPtr->Begin(&NumberOfPasses, 0);
	for (UINT iPass = 0; iPass < NumberOfPasses; ++iPass) {
		hResult = __effectPtr->BeginPass(iPass);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetVertexDeclaration(__SegmentVertexDeclarationPtr);
		assert(D3D_OK == hResult);
#if 1	// SEGMENT_VERTEX
		hResult = __devicePtr->SetStreamSource(0, segmentVertexBuffer, 0, sizeof(SEGMENT_VERTEX));
#else
		hResult = __devicePtr->SetStreamSource(0, segmentVertexBuffer, 0, sizeof(D3DXVECTOR3));
#endif
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetIndices(segmentIndexBuffer);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->DrawIndexedPrimitive(D3DPT_LINELIST, 0, 0, numSegments, StartIndex, numSegments);
		assert(D3D_OK == hResult);
		hResult = __effectPtr->EndPass();
		assert(S_OK == hResult);
	}
	hResult = __effectPtr->End();
	assert(D3D_OK == hResult);

	return 0;
}


int	kMesh::setIsometricLines(
	ISOMETRIC_PLANE_T	plane,
	const D3DXVECTOR3&	nDir,
	int			nSlice,
	LPDIRECT3DVERTEXBUFFER9	isometricVertexBufferPtr,
	unsigned&		isometricLines
)
{
	D3DXVECTOR3	nDirNormalized;
	D3DXVec3Normalize(&nDirNormalized, &nDir);

	// 각 Vertex 에서 nDir 방향에 수직인 성분을 구한다.
	kMesh::D3DXVERTEX*	pV;
	__vertexBufferPtr->Lock(0, 0, (void**) &pV, 0);
	float	fDirMin = FLT_MAX;
	float	fDirMax = FLT_MIN;
	for (DWORD i = 0; i < __numPointReprs; ++i) {
//		DWORD	prID = __pointReprPtr[i].id;
		kMesh::D3DXVERTEX&	v = pV[i];
		const D3DXVECTOR3&	P = v.p;

		// Q 의 nDir 방향 성분 = 원점 O 에서 Q 에 이르는 벡터와 nDir 벡터의 내적
		D3DXVECTOR3	w = P - D3DXVECTOR3(0, 0, 0);
		float&	fDir = plane == ISOMETRIC_XY ? v.XY : (plane == ISOMETRIC_YZ ? v.YZ : v.ZX);
		fDir = D3DXVec3Dot(&w, &nDirNormalized);
		if (fDir > fDirMax)
			fDirMax = fDir;
		if (fDir < fDirMin)
			fDirMin = fDir;
	}
	for (DWORD i = 0; i < __numPointReprs; ++i) {
		kMesh::D3DXVERTEX&	v = pV[i];
		float&	fDirNormalized = plane == ISOMETRIC_XY ? v.XY : (plane == ISOMETRIC_YZ ? v.YZ : v.ZX);
		fDirNormalized -= fDirMin;
		fDirNormalized /= (fDirMax - fDirMin);
#if 0
		fDirNormalized *= 3;	// (R, G, B) 로 분배
		v.c.x = fDirNormalized;
		v.c.y = 0;
		v.c.z = 0;
		CLAMP2(v.c);
#endif
	}

	// 각 Face 에서 같은 값(fDirNormalized)을 가지는 점들을 잇는 선분을 만든다;
	// fDirNormalized값(0<= fDirNormalized <= 3)을 nSlice 등분한다.
#if 0	// (R, G, B) 로 분배
	float	fDirNormalized_delta = 3.f / nSlice;
#else
	float	fDirNormalized_delta = 1.f / nSlice;
#endif
	isometricLines = 0;
	D3DXVECTOR3*	pDst = NULL;
	isometricVertexBufferPtr->Lock(0, 0, (void**) &pDst, 0);
	for (DWORD iFace = 0; iFace < __numFaces; ++iFace) {
		CFace&	face = __facePtr[iFace];
		// pV 에 대한 인덱스
		DWORD	index0 = face.prID0;
		DWORD	index1 = face.prID1;
		DWORD	index2 = face.prID2;

		// fDirNormalized 를 정렬한다; 어느 변을 따라 선분을 그을 지를 결정하기 위하여...
		float	fDirNormalized_min, fDirNormalized_max;
		DWORD	index_min, index_max;
		float	fDirNormalized0;
		fDirNormalized0 = plane == ISOMETRIC_XY ? pV[index0].XY : (plane == ISOMETRIC_YZ ? pV[index0].YZ : pV[index0].ZX);pV[index0].tx;
		fDirNormalized_min = fDirNormalized0;
		index_min = index0;
		fDirNormalized_max = fDirNormalized0;
		index_max = index0;
		float	fDirNormalized1;
		fDirNormalized1 = plane == ISOMETRIC_XY ? pV[index1].XY : (plane == ISOMETRIC_YZ ? pV[index1].YZ : pV[index1].ZX);
		if (fDirNormalized1 > fDirNormalized_max) {
			index_max = index1;
			fDirNormalized_max = fDirNormalized1;
		}
		if (fDirNormalized1 < fDirNormalized_min) {
			index_min = index1;
			fDirNormalized_min = fDirNormalized1;
		}
		float	fDirNormalized2;
		fDirNormalized2 = plane == ISOMETRIC_XY ? pV[index2].XY : (plane == ISOMETRIC_YZ ? pV[index2].YZ : pV[index2].ZX);
		if (fDirNormalized2 > fDirNormalized_max) {
			index_max = index2;
			fDirNormalized_max = fDirNormalized2;
		}
		if (fDirNormalized2 < fDirNormalized_min) {
			index_min = index2;
			fDirNormalized_min = fDirNormalized2;
		}
//		assert(index_min != index_max);
		if (index_min == index_max) {
			// 삼각형이 등고선을 만드는 단면과 평행하다.
#ifdef _DEBUG_
			(void) fprintf(stderr, "index_min = index_max: face ID = %d, %f(%d: [%f, %f, %f]), %f(%d: [%f, %f, %f]), %f(%d: [%f, %f, %f])\n", iFace, fDirNormalized0, index0, pV[index0].p.x, pV[index0].p.y, pV[index0].p.z, fDirNormalized1, index1, pV[index1].p.x, pV[index1].p.y, pV[index1].p.z, fDirNormalized2, index2, pV[index2].p.x, pV[index2].p.y, pV[index2].p.z);
#endif
			int	iStart = (int) ceil(fDirNormalized_min / fDirNormalized_delta);
			int	iEnd = (int) floor(fDirNormalized_max / fDirNormalized_delta);
			// 삼각형이 단면과 일치하면 세 변 모두 등고선의 일부가 된다.
			if (iStart != iEnd) {
				// 일치하지 않는다.
				continue;
			}
			// 일치한다.
			D3DXVECTOR3	Pa, Pb, Na, Na1, Nb, Nb1;

			Pa = pV[index0].p, Pb = pV[index1].p;
			Na = pV[index0].n, Nb = pV[index1].n;
			Na1 = Na - D3DXVec3Dot(&Na, &nDir) * Na;	// Na 중에서 nDir 에 수직인 성분
			D3DXVec3Normalize(&Na1, &Na1);
			pDst[2 * isometricLines + 0] = Pa + __shift_amount_along_normal /* __shiftAmount */ * Na1;
			Nb1 = Nb - D3DXVec3Dot(&Nb, &nDir) * Nb;	// Nb 중에서 nDir 에 수직인 성분
			D3DXVec3Normalize(&Nb1, &Nb1);
			pDst[2 * isometricLines + 1] = Pb + __shift_amount_along_normal /* __shiftAmount */ * Nb1;
			++isometricLines;

			Pa = pV[index1].p, Pb = pV[index2].p;
			Na = pV[index1].n, Nb = pV[index2].n;
			Na1 = Na - D3DXVec3Dot(&Na, &nDir) * Na;	// Na 중에서 nDir 에 수직인 성분
			D3DXVec3Normalize(&Na1, &Na1);
			pDst[2 * isometricLines + 0] = Pa + __shift_amount_along_normal /* __shiftAmount */ * Na1;
			Nb1 = Nb - D3DXVec3Dot(&Nb, &nDir) * Nb;	// Nb 중에서 nDir 에 수직인 성분
			D3DXVec3Normalize(&Nb1, &Nb1);
			pDst[2 * isometricLines + 1] = Pb + __shift_amount_along_normal /* __shiftAmount */ * Nb1;
			++isometricLines;

			Pa = pV[index2].p, Pb = pV[index0].p;
			Na = pV[index2].n, Nb = pV[index0].n;
			Na1 = Na - D3DXVec3Dot(&Na, &nDir) * Na;	// Na 중에서 nDir 에 수직인 성분
			D3DXVec3Normalize(&Na1, &Na1);
			pDst[2 * isometricLines + 0] = Pa + __shift_amount_along_normal /* __shiftAmount */ * Na1;
			Nb1 = Nb - D3DXVec3Dot(&Nb, &nDir) * Nb;	// Nb 중에서 nDir 에 수직인 성분
			D3DXVec3Normalize(&Nb1, &Nb1);
			pDst[2 * isometricLines + 1] = Pb + __shift_amount_along_normal /* __shiftAmount */ * Nb1;
			++isometricLines;

			continue;
		}
		DWORD	index_mid;
		float	fDirNormalized_mid;
		if (index_min == index0) {
			if (index_max == index1) {
				index_mid = index2;
				fDirNormalized_mid = fDirNormalized2;
			} else {
				index_mid = index1;
				fDirNormalized_mid = fDirNormalized1;
			}
		} else if (index_min == index1) {
			if (index_max == index2) {
				index_mid = index0;
				fDirNormalized_mid = fDirNormalized0;
			} else {
				index_mid = index2;
				fDirNormalized_mid = fDirNormalized2;
			}
		} else {	// index_min = index2
			if (index_max == index0) {
				index_mid = index1;
				fDirNormalized_mid = fDirNormalized1;
			} else {
				index_mid = index0;
				fDirNormalized_mid = fDirNormalized0;
			}
		}

		// index_min 번째 Vertex 에서 출발해서 index_max 번째 Vertex 를 향하는 성분을 따라가면,
		// fDirNormalized_delta 단위로 선을 긋는다.
		// 몇 개의 선을 그을 지를 결정한다.
		int	iStart = (int) ceil(fDirNormalized_min / fDirNormalized_delta);
		int	iEnd = (int) floor(fDirNormalized_max / fDirNormalized_delta);
		float	fDirNormalized_a = fDirNormalized_min;	// 선분의 다른 끝에 해당하는 fDirNormalized 값을 알아내기 위해
		float	fDirNormalized_b = fDirNormalized_mid;
		int	index_a = index_min;
		int	index_b = index_mid;
		bool	next_edge = false;
		for (int iStep = iStart; iStep <= iEnd; ++iStep) {
			float	fDirNormalized_next = iStep * fDirNormalized_delta;
			// x = a * (1 - t) + b * t 에 해당하는 매개변수 t?
			float	t_a = (fDirNormalized_next - fDirNormalized_min) / (fDirNormalized_max - fDirNormalized_min);
			assert(t_a >= 0 || t_a <= 1);
			D3DXVECTOR3	Pa = pV[index_min].p * (1 - t_a) + pV[index_max].p * t_a;
			D3DXVECTOR3	Na = pV[index_min].n * (1 - t_a) + pV[index_max].n * t_a;

			// 선분의 다른 끝점을 알아낸다.
			if (fDirNormalized_next >= fDirNormalized_mid) {
				next_edge = true;
				fDirNormalized_a = fDirNormalized_mid;
				index_a = index_mid;
				fDirNormalized_b = fDirNormalized_max;
				index_b = index_max;
			}
#if 1
			float	t_b;
			if (fDirNormalized_b == fDirNormalized_a) {
				// index_a 번째 꼭지점과 index_b 번째 꼭지점에서의 값이 같다;
				// 두 꼭지점을 지나는 선분이 등고선과 평행하다.
				// 따라서 fDirNormalized_next 가 fDirNormalized_a 와 같다면
				// 두 꼭지점을 지나는 선분 자체가 등고선의 일부가 된다;
				// 그렇지 않다면 두 꼭지점 사이를 지나는 선분 어느 곳에서도 등고선이 지나지 않는다.
				if (fDirNormalized_next == fDirNormalized_a) {
#ifdef _DEBUG_
					(void) fprintf(stderr, "fDirNormalized_a == fDirNormalized_b && fDirNormalized_a == fDirNormalized_b: %f(%d: [%f, %f, %f]), %f(%d: [%f, %f, %f]), %f(%d: [%f, %f, %f])\n", fDirNormalized0, index0, pV[index0].p.x, pV[index0].p.y, pV[index0].p.z, fDirNormalized1, index1, pV[index1].p.x, pV[index1].p.y, pV[index1].p.z, fDirNormalized2, index2, pV[index2].p.x, pV[index2].p.y, pV[index2].p.z);
#endif
					// 등고선의 일부인 선분의 시작점은 index_a 번째 꼭지점이고(t_a = 0),
					// 끝점은 index_b 번째 꼭지점(t_b = 1 = 1 - t_a)이다.
					t_b = 1 - t_a;
				} else
					continue;
			} else {
				t_b = (fDirNormalized_next - fDirNormalized_a) / (fDirNormalized_b - fDirNormalized_a);
			}
#else
			float	t_b = (fDirNormalized_next - fDirNormalized_a) / (fDirNormalized_b - fDirNormalized_a);
#endif
			assert(t_b >= 0 || t_b <= 1);
			D3DXVECTOR3	Pb = pV[index_a].p * (1 - t_b) + pV[index_b].p * t_b;
			D3DXVECTOR3	Nb = pV[index_a].n * (1 - t_b) + pV[index_b].n * t_b;

			// Pa, Pb 를 잇는 선분을 만든다.
#if 1
			// nDir 에 수직인 방향으로 약간씩 띄워 렌더링했을 때 잘 보이게 한다.
			D3DXVECTOR3	Na1 = Na - D3DXVec3Dot(&Na, &nDir) * Na;	// Na 중에서 nDir 에 수직인 성분
			D3DXVec3Normalize(&Na1, &Na1);
			pDst[2 * isometricLines + 0] = Pa + __shift_amount_along_normal /* __shiftAmount */ * Na1;
			D3DXVECTOR3	Nb1 = Nb - D3DXVec3Dot(&Nb, &nDir) * Nb;	// Nb 중에서 nDir 에 수직인 성분
			D3DXVec3Normalize(&Nb1, &Nb1);
			pDst[2 * isometricLines + 1] = Pb + __shift_amount_along_normal /* __shiftAmount */ * Nb1;
#else
			pDst[2 * isometricLines + 0] = Pa;
			pDst[2 * isometricLines + 1] = Pb;
#endif
			++isometricLines;
		}
	}
	isometricVertexBufferPtr->Unlock();

	__vertexBufferPtr->Unlock();

	return 0;
}


int
kMesh::ShowIsometric(
	const LPDIRECT3DVERTEXBUFFER9	isometricVertexBufferPtr,
	unsigned			isometricLines,
	const D3DXVECTOR4&	lineColor,

	UINT	Stride,
	const D3DXMATRIX*	m_ptr
) const
{
	HRESULT	hResult;

	__effectPtr->SetInt("g__FillMode", D3DFILL_WIREFRAME);
	if (m_ptr)
		__effectPtr->SetMatrix("g_mWorldViewProjection", m_ptr);
	else
		__effectPtr->SetMatrix("g_mWorldViewProjection", &__worldViewProjectionMatrix);
	__effectPtr->SetMatrix("g_mWorld", &__worldMatrix);
	__effectPtr->SetVector("pointColor", &lineColor);
	D3DXHANDLE	hTechnique = __effectPtr->GetTechnique(0);
	assert(hTechnique);
	hResult = __effectPtr->SetTechnique(hTechnique);
	assert(S_OK == hResult);
	UINT	NumberOfPasses;
	assert(D3D_OK == hResult);
	hResult = __effectPtr->Begin(&NumberOfPasses, 0);
	for (UINT iPass = 0; iPass < NumberOfPasses; ++iPass) {
		hResult = __effectPtr->BeginPass(iPass);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetVertexDeclaration(__isometricDeclarationPtr);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetStreamSource(0, isometricVertexBufferPtr, 0, Stride);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->DrawPrimitive(D3DPT_LINELIST, 0, isometricLines);
		assert(D3D_OK == hResult);
		hResult = __effectPtr->EndPass();
		assert(S_OK == hResult);
	}
	hResult = __effectPtr->End();
	assert(D3D_OK == hResult);

	return 0;
}

int	kMesh::DrawAxis(const D3DXVECTOR3& upDir, const D3DXVECTOR3& rightDir) const
{
	D3DVIEWPORT9	viewport;
	__devicePtr->GetViewport(&viewport);
	D3DXVECTOR3	o = D3DXVECTOR3(50, (FLOAT) viewport.Height - 50, 0.5);
	D3DXVECTOR3	O;	// Object 공간에서의 좌표
	D3DXVec3Unproject(&O, &o, &viewport, &__projectionMatrix, &__viewMatrix, &__worldMatrix);
	// Screen 공간에서 30픽셀 차이는 오브젝트 공간에서는 얼마?
	D3DXVECTOR3	o30 = D3DXVECTOR3(50, (FLOAT) viewport.Height - 20, 0.5);
	D3DXVECTOR3	O30;	// Object 공간에서의 좌표
	D3DXVec3Unproject(&O30, &o30, &viewport, &__projectionMatrix, &__viewMatrix, &__worldMatrix);
	FLOAT	axisLength = D3DXVec3Length(&D3DXVECTOR3(O - O30));

	HRESULT	hResult;

	LPDIRECT3DVERTEXBUFFER9		axisVertexBufferPtr = 0;
do {
	hResult = __devicePtr->CreateVertexBuffer(
		2 * sizeof(D3DXVECTOR3),
		D3DUSAGE_WRITEONLY,
		0,
		D3DPOOL_MANAGED,
		&axisVertexBufferPtr,
		NULL
	);

	// upDir-axis
{
	D3DXVECTOR3*	twoPoints = NULL;
	hResult = axisVertexBufferPtr->Lock(0, 0, (void**) &twoPoints, 0);
	twoPoints[0] = O;
	twoPoints[1] = O + axisLength * upDir;
	axisVertexBufferPtr->Unlock();
	__effectPtr->SetInt("g__FillMode", D3DFILL_WIREFRAME);
	__effectPtr->SetMatrix("g_mWorldViewProjection", &__worldViewProjectionMatrix);
	__effectPtr->SetMatrix("g_mWorld", &__worldMatrix);
	D3DXVECTOR4	pointColor(1.f, 0.f, 1.f, 1.f);
	__effectPtr->SetVector("pointColor", &pointColor);
	D3DXHANDLE	hTechnique = __effectPtr->GetTechnique(0);
	assert(hTechnique);
	hResult = __effectPtr->SetTechnique(hTechnique);
	assert(S_OK == hResult);
	UINT	NumberOfPasses;
	assert(D3D_OK == hResult);
	hResult = __effectPtr->Begin(&NumberOfPasses, 0);
	for (UINT iPass = 0; iPass < NumberOfPasses; ++iPass) {
		hResult = __effectPtr->BeginPass(iPass);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetVertexDeclaration(__isometricDeclarationPtr);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetStreamSource(0, axisVertexBufferPtr, 0, sizeof(D3DXVECTOR3));
		assert(D3D_OK == hResult);
		hResult = __devicePtr->DrawPrimitive(D3DPT_LINELIST, 0, 1);
		assert(D3D_OK == hResult);
		hResult = __effectPtr->EndPass();
		assert(S_OK == hResult);
	}
	hResult = __effectPtr->End();
	assert(D3D_OK == hResult);
}
	// rightDir-axis
{
	D3DXVECTOR3*	twoPoints = NULL;
	hResult = axisVertexBufferPtr->Lock(0, 0, (void**) &twoPoints, 0);
	twoPoints[0] = O;
	twoPoints[1] = O + axisLength * rightDir;
	axisVertexBufferPtr->Unlock();
	__effectPtr->SetInt("g__FillMode", D3DFILL_WIREFRAME);
	__effectPtr->SetMatrix("g_mWorldViewProjection", &__worldViewProjectionMatrix);
	__effectPtr->SetMatrix("g_mWorld", &__worldMatrix);
	D3DXVECTOR4	pointColor(0.f, 1.f, 1.f, 1.f);
	__effectPtr->SetVector("pointColor", &pointColor);
	D3DXHANDLE	hTechnique = __effectPtr->GetTechnique(0);
	assert(hTechnique);
	hResult = __effectPtr->SetTechnique(hTechnique);
	assert(S_OK == hResult);
	UINT	NumberOfPasses;
	assert(D3D_OK == hResult);
	hResult = __effectPtr->Begin(&NumberOfPasses, 0);
	for (UINT iPass = 0; iPass < NumberOfPasses; ++iPass) {
		hResult = __effectPtr->BeginPass(iPass);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetVertexDeclaration(__isometricDeclarationPtr);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetStreamSource(0, axisVertexBufferPtr, 0, sizeof(D3DXVECTOR3));
		assert(D3D_OK == hResult);
		hResult = __devicePtr->DrawPrimitive(D3DPT_LINELIST, 0, 1);
		assert(D3D_OK == hResult);
		hResult = __effectPtr->EndPass();
		assert(S_OK == hResult);
	}
	hResult = __effectPtr->End();
	assert(D3D_OK == hResult);
}
	// x-axis
{
	D3DXVECTOR3*	twoPoints = NULL;
	hResult = axisVertexBufferPtr->Lock(0, 0, (void**) &twoPoints, 0);
	twoPoints[0] = O;
	twoPoints[1] = O + axisLength * D3DXVECTOR3(1, 0, 0);
	axisVertexBufferPtr->Unlock();
	__effectPtr->SetInt("g__FillMode", D3DFILL_WIREFRAME);
	__effectPtr->SetMatrix("g_mWorldViewProjection", &__worldViewProjectionMatrix);
	__effectPtr->SetMatrix("g_mWorld", &__worldMatrix);
	D3DXVECTOR4	pointColor(1.f, 0.f, 0.f, 1.f);
	__effectPtr->SetVector("pointColor", &pointColor);
	D3DXHANDLE	hTechnique = __effectPtr->GetTechnique(0);
	assert(hTechnique);
	hResult = __effectPtr->SetTechnique(hTechnique);
	assert(S_OK == hResult);
	UINT	NumberOfPasses;
	assert(D3D_OK == hResult);
	hResult = __effectPtr->Begin(&NumberOfPasses, 0);
	for (UINT iPass = 0; iPass < NumberOfPasses; ++iPass) {
		hResult = __effectPtr->BeginPass(iPass);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetVertexDeclaration(__isometricDeclarationPtr);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetStreamSource(0, axisVertexBufferPtr, 0, sizeof(D3DXVECTOR3));
		assert(D3D_OK == hResult);
		hResult = __devicePtr->DrawPrimitive(D3DPT_LINELIST, 0, 1);
		assert(D3D_OK == hResult);
		hResult = __effectPtr->EndPass();
		assert(S_OK == hResult);
	}
	hResult = __effectPtr->End();
	assert(D3D_OK == hResult);
}
	// y-axis
{
	D3DXVECTOR3*	twoPoints = NULL;
	hResult = axisVertexBufferPtr->Lock(0, 0, (void**) &twoPoints, 0);
	twoPoints[0] = O;
	twoPoints[1] = O + axisLength * D3DXVECTOR3(0, 1, 0);
	axisVertexBufferPtr->Unlock();
	__effectPtr->SetInt("g__FillMode", D3DFILL_WIREFRAME);
	__effectPtr->SetMatrix("g_mWorldViewProjection", &__worldViewProjectionMatrix);
	__effectPtr->SetMatrix("g_mWorld", &__worldMatrix);
	D3DXVECTOR4	pointColor(0.f, 1.f, 0.f, 1.f);
	__effectPtr->SetVector("pointColor", &pointColor);
	D3DXHANDLE	hTechnique = __effectPtr->GetTechnique(0);
	assert(hTechnique);
	hResult = __effectPtr->SetTechnique(hTechnique);
	assert(S_OK == hResult);
	UINT	NumberOfPasses;
	assert(D3D_OK == hResult);
	hResult = __effectPtr->Begin(&NumberOfPasses, 0);
	for (UINT iPass = 0; iPass < NumberOfPasses; ++iPass) {
		hResult = __effectPtr->BeginPass(iPass);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetVertexDeclaration(__isometricDeclarationPtr);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetStreamSource(0, axisVertexBufferPtr, 0, sizeof(D3DXVECTOR3));
		assert(D3D_OK == hResult);
		hResult = __devicePtr->DrawPrimitive(D3DPT_LINELIST, 0, 1);
		assert(D3D_OK == hResult);
		hResult = __effectPtr->EndPass();
		assert(S_OK == hResult);
	}
	hResult = __effectPtr->End();
	assert(D3D_OK == hResult);
}
	// z-axis
{
	D3DXVECTOR3*	twoPoints = NULL;
	hResult = axisVertexBufferPtr->Lock(0, 0, (void**) &twoPoints, 0);
	twoPoints[0] = O;
	twoPoints[1] = O + axisLength * D3DXVECTOR3(0, 0, 1);
	axisVertexBufferPtr->Unlock();
	__effectPtr->SetInt("g__FillMode", D3DFILL_WIREFRAME);
	__effectPtr->SetMatrix("g_mWorldViewProjection", &__worldViewProjectionMatrix);
	__effectPtr->SetMatrix("g_mWorld", &__worldMatrix);
	D3DXVECTOR4	pointColor(0.f, 0.f, 1.f, 1.f);
	__effectPtr->SetVector("pointColor", &pointColor);
	D3DXHANDLE	hTechnique = __effectPtr->GetTechnique(0);
	assert(hTechnique);
	hResult = __effectPtr->SetTechnique(hTechnique);
	assert(S_OK == hResult);
	UINT	NumberOfPasses;
	assert(D3D_OK == hResult);
	hResult = __effectPtr->Begin(&NumberOfPasses, 0);
	for (UINT iPass = 0; iPass < NumberOfPasses; ++iPass) {
		hResult = __effectPtr->BeginPass(iPass);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetVertexDeclaration(__isometricDeclarationPtr);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetStreamSource(0, axisVertexBufferPtr, 0, sizeof(D3DXVECTOR3));
		assert(D3D_OK == hResult);
		hResult = __devicePtr->DrawPrimitive(D3DPT_LINELIST, 0, 1);
		assert(D3D_OK == hResult);
		hResult = __effectPtr->EndPass();
		assert(S_OK == hResult);
	}
	hResult = __effectPtr->End();
	assert(D3D_OK == hResult);
}
} while (0);
	if (axisVertexBufferPtr) {
		axisVertexBufferPtr->Release();
		axisVertexBufferPtr = 0;
	}

	return 0;
}
int	kMesh::DrawAxis0() const
{
	D3DXVECTOR3	O = D3DXVECTOR3(0, 0, 0);
	float	axisLength = .1f * __radius;

	HRESULT	hResult;

	LPDIRECT3DVERTEXBUFFER9		axisVertexBufferPtr = 0;
do {
	hResult = __devicePtr->CreateVertexBuffer(
		2 * sizeof(D3DXVECTOR3),
		D3DUSAGE_WRITEONLY,
		0,
		D3DPOOL_MANAGED,
		&axisVertexBufferPtr,
		NULL
	);

	// x-axis
{
	D3DXVECTOR3*	twoPoints = NULL;
	hResult = axisVertexBufferPtr->Lock(0, 0, (void**) &twoPoints, 0);
	twoPoints[0] = O;
	twoPoints[1] = O + axisLength * D3DXVECTOR3(1, 0, 0);
	axisVertexBufferPtr->Unlock();
	__effectPtr->SetInt("g__FillMode", D3DFILL_WIREFRAME);
	__effectPtr->SetMatrix("g_mWorldViewProjection", &__worldViewProjectionMatrix);
	__effectPtr->SetMatrix("g_mWorld", &__worldMatrix);
	D3DXVECTOR4	pointColor(1.f, 0.f, 0.f, 1.f);
	__effectPtr->SetVector("pointColor", &pointColor);
	D3DXHANDLE	hTechnique = __effectPtr->GetTechnique(0);
	assert(hTechnique);
	hResult = __effectPtr->SetTechnique(hTechnique);
	assert(S_OK == hResult);
	UINT	NumberOfPasses;
	assert(D3D_OK == hResult);
	hResult = __effectPtr->Begin(&NumberOfPasses, 0);
	for (UINT iPass = 0; iPass < NumberOfPasses; ++iPass) {
		hResult = __effectPtr->BeginPass(iPass);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetVertexDeclaration(__isometricDeclarationPtr);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetStreamSource(0, axisVertexBufferPtr, 0, sizeof(D3DXVECTOR3));
		assert(D3D_OK == hResult);
		hResult = __devicePtr->DrawPrimitive(D3DPT_LINELIST, 0, 1);
		assert(D3D_OK == hResult);
		hResult = __effectPtr->EndPass();
		assert(S_OK == hResult);
	}
	hResult = __effectPtr->End();
	assert(D3D_OK == hResult);
}
	// y-axis
{
	D3DXVECTOR3*	twoPoints = NULL;
	hResult = axisVertexBufferPtr->Lock(0, 0, (void**) &twoPoints, 0);
	twoPoints[0] = O;
	twoPoints[1] = O + axisLength * D3DXVECTOR3(0, 1, 0);
	axisVertexBufferPtr->Unlock();
	__effectPtr->SetInt("g__FillMode", D3DFILL_WIREFRAME);
	__effectPtr->SetMatrix("g_mWorldViewProjection", &__worldViewProjectionMatrix);
	__effectPtr->SetMatrix("g_mWorld", &__worldMatrix);
	D3DXVECTOR4	pointColor(0.f, 1.f, 0.f, 1.f);
	__effectPtr->SetVector("pointColor", &pointColor);
	D3DXHANDLE	hTechnique = __effectPtr->GetTechnique(0);
	assert(hTechnique);
	hResult = __effectPtr->SetTechnique(hTechnique);
	assert(S_OK == hResult);
	UINT	NumberOfPasses;
	assert(D3D_OK == hResult);
	hResult = __effectPtr->Begin(&NumberOfPasses, 0);
	for (UINT iPass = 0; iPass < NumberOfPasses; ++iPass) {
		hResult = __effectPtr->BeginPass(iPass);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetVertexDeclaration(__isometricDeclarationPtr);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetStreamSource(0, axisVertexBufferPtr, 0, sizeof(D3DXVECTOR3));
		assert(D3D_OK == hResult);
		hResult = __devicePtr->DrawPrimitive(D3DPT_LINELIST, 0, 1);
		assert(D3D_OK == hResult);
		hResult = __effectPtr->EndPass();
		assert(S_OK == hResult);
	}
	hResult = __effectPtr->End();
	assert(D3D_OK == hResult);
}
	// z-axis
{
	D3DXVECTOR3*	twoPoints = NULL;
	hResult = axisVertexBufferPtr->Lock(0, 0, (void**) &twoPoints, 0);
	twoPoints[0] = O;
	twoPoints[1] = O + axisLength * D3DXVECTOR3(0, 0, 1);
	axisVertexBufferPtr->Unlock();
	__effectPtr->SetInt("g__FillMode", D3DFILL_WIREFRAME);
	__effectPtr->SetMatrix("g_mWorldViewProjection", &__worldViewProjectionMatrix);
	__effectPtr->SetMatrix("g_mWorld", &__worldMatrix);
	D3DXVECTOR4	pointColor(0.f, 0.f, 1.f, 1.f);
	__effectPtr->SetVector("pointColor", &pointColor);
	D3DXHANDLE	hTechnique = __effectPtr->GetTechnique(0);
	assert(hTechnique);
	hResult = __effectPtr->SetTechnique(hTechnique);
	assert(S_OK == hResult);
	UINT	NumberOfPasses;
	assert(D3D_OK == hResult);
	hResult = __effectPtr->Begin(&NumberOfPasses, 0);
	for (UINT iPass = 0; iPass < NumberOfPasses; ++iPass) {
		hResult = __effectPtr->BeginPass(iPass);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetVertexDeclaration(__isometricDeclarationPtr);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetStreamSource(0, axisVertexBufferPtr, 0, sizeof(D3DXVECTOR3));
		assert(D3D_OK == hResult);
		hResult = __devicePtr->DrawPrimitive(D3DPT_LINELIST, 0, 1);
		assert(D3D_OK == hResult);
		hResult = __effectPtr->EndPass();
		assert(S_OK == hResult);
	}
	hResult = __effectPtr->End();
	assert(D3D_OK == hResult);
}
} while (0);
	if (axisVertexBufferPtr) {
		axisVertexBufferPtr->Release();
		axisVertexBufferPtr = 0;
	}

	return 0;
}
int	kMesh::DrawPivot() const
{
	D3DXVECTOR3	O = .5f * (__left_lower_near + __right_upper_far);
	FLOAT	axisLength = .1f * __radius;

	HRESULT	hResult;

#if 1	// 점으로 표시
	LPDIRECT3DVERTEXBUFFER9	pivotVertexBufferPtr = NULL;
	__devicePtr->CreateVertexBuffer(
		1 * sizeof(D3DXVECTOR3),
		D3DUSAGE_WRITEONLY,
		0,
		D3DPOOL_MANAGED,
		&pivotVertexBufferPtr,
		NULL
	);
	D3DXVECTOR3*	pivotVertexPtr;
	pivotVertexBufferPtr->Lock(0, 0, (void**) &pivotVertexPtr, 0);
do {
	pivotVertexPtr[0] = .5f * (__left_lower_near + __right_upper_far);
	__effectPtr->SetInt("g__FillMode", D3DFILL_POINT);
	__effectPtr->SetMatrix("g_mWorldViewProjection", &__worldViewProjectionMatrix);
	__effectPtr->SetMatrix("g_mWorld", &__worldMatrix);
	FLOAT		pointSize = 5.f;
	D3DXVECTOR4	pointColor(1, 1, 0, 1);
	__effectPtr->SetFloat("pointSize", pointSize);
	__effectPtr->SetVector("pointColor", &pointColor);
	D3DXHANDLE	hTechnique = __effectPtr->GetTechnique(0);
	assert(hTechnique);
	hResult = __effectPtr->SetTechnique(hTechnique);
	assert(S_OK == hResult);
	UINT	NumberOfPasses;
	hResult = __effectPtr->Begin(&NumberOfPasses, 0);
	assert(D3D_OK == hResult);
	for (UINT iPass = 0; iPass < NumberOfPasses; ++iPass) {
		hResult = __effectPtr->BeginPass(iPass);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetVertexDeclaration(__OnePointDeclarationPtr);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetStreamSource(0, pivotVertexBufferPtr, 0, sizeof(D3DXVECTOR3));
		assert(D3D_OK == hResult);
		hResult = __devicePtr->DrawPrimitive(D3DPT_POINTLIST, 0, 1);
		assert(D3D_OK == hResult);
		hResult = __effectPtr->EndPass();
		assert(S_OK == hResult);
	}
	hResult = __effectPtr->End();
	assert(S_OK == hResult);
} while (0);
	pivotVertexBufferPtr->Unlock();
	if (pivotVertexBufferPtr) {
		pivotVertexBufferPtr->Release();
		pivotVertexBufferPtr = 0;
	}
#endif

	LPDIRECT3DVERTEXBUFFER9		axisVertexBufferPtr = 0;
do {
	hResult = __devicePtr->CreateVertexBuffer(
		2 * sizeof(D3DXVECTOR3),
		D3DUSAGE_WRITEONLY,
		0,
		D3DPOOL_MANAGED,
		&axisVertexBufferPtr,
		NULL
	);

	// x-axis
{
	D3DXVECTOR3*	twoPoints = NULL;
	hResult = axisVertexBufferPtr->Lock(0, 0, (void**) &twoPoints, 0);
	twoPoints[0] = O;
	twoPoints[1] = O + axisLength * D3DXVECTOR3(1, 0, 0);
	axisVertexBufferPtr->Unlock();
	__effectPtr->SetInt("g__FillMode", D3DFILL_WIREFRAME);
	__effectPtr->SetMatrix("g_mWorldViewProjection", &__worldViewProjectionMatrix);
	__effectPtr->SetMatrix("g_mWorld", &__worldMatrix);
	D3DXVECTOR4	pointColor(1.f, 0.f, 0.f, 1.f);
	__effectPtr->SetVector("pointColor", &pointColor);
	D3DXHANDLE	hTechnique = __effectPtr->GetTechnique(0);
	assert(hTechnique);
	hResult = __effectPtr->SetTechnique(hTechnique);
	assert(S_OK == hResult);
	UINT	NumberOfPasses;
	assert(D3D_OK == hResult);
	hResult = __effectPtr->Begin(&NumberOfPasses, 0);
	for (UINT iPass = 0; iPass < NumberOfPasses; ++iPass) {
		hResult = __effectPtr->BeginPass(iPass);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetVertexDeclaration(__isometricDeclarationPtr);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetStreamSource(0, axisVertexBufferPtr, 0, sizeof(D3DXVECTOR3));
		assert(D3D_OK == hResult);
		hResult = __devicePtr->DrawPrimitive(D3DPT_LINELIST, 0, 1);
		assert(D3D_OK == hResult);
		hResult = __effectPtr->EndPass();
		assert(S_OK == hResult);
	}
	hResult = __effectPtr->End();
	assert(D3D_OK == hResult);
}
	// y-axis
{
	D3DXVECTOR3*	twoPoints = NULL;
	hResult = axisVertexBufferPtr->Lock(0, 0, (void**) &twoPoints, 0);
	twoPoints[0] = O;
	twoPoints[1] = O + axisLength * D3DXVECTOR3(0, 1, 0);
	axisVertexBufferPtr->Unlock();
	__effectPtr->SetInt("g__FillMode", D3DFILL_WIREFRAME);
	__effectPtr->SetMatrix("g_mWorldViewProjection", &__worldViewProjectionMatrix);
	__effectPtr->SetMatrix("g_mWorld", &__worldMatrix);
	D3DXVECTOR4	pointColor(0.f, 1.f, 0.f, 1.f);
	__effectPtr->SetVector("pointColor", &pointColor);
	D3DXHANDLE	hTechnique = __effectPtr->GetTechnique(0);
	assert(hTechnique);
	hResult = __effectPtr->SetTechnique(hTechnique);
	assert(S_OK == hResult);
	UINT	NumberOfPasses;
	assert(D3D_OK == hResult);
	hResult = __effectPtr->Begin(&NumberOfPasses, 0);
	for (UINT iPass = 0; iPass < NumberOfPasses; ++iPass) {
		hResult = __effectPtr->BeginPass(iPass);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetVertexDeclaration(__isometricDeclarationPtr);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetStreamSource(0, axisVertexBufferPtr, 0, sizeof(D3DXVECTOR3));
		assert(D3D_OK == hResult);
		hResult = __devicePtr->DrawPrimitive(D3DPT_LINELIST, 0, 1);
		assert(D3D_OK == hResult);
		hResult = __effectPtr->EndPass();
		assert(S_OK == hResult);
	}
	hResult = __effectPtr->End();
	assert(D3D_OK == hResult);
}
	// z-axis
{
	D3DXVECTOR3*	twoPoints = NULL;
	hResult = axisVertexBufferPtr->Lock(0, 0, (void**) &twoPoints, 0);
	twoPoints[0] = O;
	twoPoints[1] = O + axisLength * D3DXVECTOR3(0, 0, 1);
	axisVertexBufferPtr->Unlock();
	__effectPtr->SetInt("g__FillMode", D3DFILL_WIREFRAME);
	__effectPtr->SetMatrix("g_mWorldViewProjection", &__worldViewProjectionMatrix);
	__effectPtr->SetMatrix("g_mWorld", &__worldMatrix);
	D3DXVECTOR4	pointColor(0.f, 0.f, 1.f, 1.f);
	__effectPtr->SetVector("pointColor", &pointColor);
	D3DXHANDLE	hTechnique = __effectPtr->GetTechnique(0);
	assert(hTechnique);
	hResult = __effectPtr->SetTechnique(hTechnique);
	assert(S_OK == hResult);
	UINT	NumberOfPasses;
	assert(D3D_OK == hResult);
	hResult = __effectPtr->Begin(&NumberOfPasses, 0);
	for (UINT iPass = 0; iPass < NumberOfPasses; ++iPass) {
		hResult = __effectPtr->BeginPass(iPass);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetVertexDeclaration(__isometricDeclarationPtr);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetStreamSource(0, axisVertexBufferPtr, 0, sizeof(D3DXVECTOR3));
		assert(D3D_OK == hResult);
		hResult = __devicePtr->DrawPrimitive(D3DPT_LINELIST, 0, 1);
		assert(D3D_OK == hResult);
		hResult = __effectPtr->EndPass();
		assert(S_OK == hResult);
	}
	hResult = __effectPtr->End();
	assert(D3D_OK == hResult);
}
} while (0);
	if (axisVertexBufferPtr) {
		axisVertexBufferPtr->Release();
		axisVertexBufferPtr = 0;
	}

	return 0;
}

int	kMesh::findNormalAmountByVolumeChange(
	float&	fNormalAmount,		// [OUT]
	float	fNormalAmountGuess,	// 항상 양수이다.
	float	volumeChange,		// +:= 늘어난다, -:= 줄어든다
	unsigned&	max_iter,
	float	volumeChangeTolerance
) const
{
	volumeChangeTolerance = fabsf(volumeChange) * 0.00001f;
	int	iReturnVal = 0;
	if (volumeChange == 0.f || max_iter == 0) {	// 어쩌라구???
		return 0;
	}

	// numPointReprs 개의 Vertex 중에서
	kMesh::D3DXVERTEX*	vertexArray;
	__vertexBufferPtr->Lock(0, 0, (void**) &vertexArray, 0);
	DWORD*	pointReprArray = 0;	// 움직이는 Vertex 목록
	DWORD*	faceArray = 0;	// Vertex 가 움직임으로써 모양이 변하는 Face 목록;
				// TO DO LIST: 비트면 충분한데...
do {
	pointReprArray = new DWORD[__numPointReprs];
	faceArray = new DWORD[__numFaces];
	// 0(False)으로 초기화한다.
	FillMemory(pointReprArray, __numPointReprs * sizeof(DWORD), 0);
	FillMemory(faceArray, __numFaces * sizeof(DWORD), 0);

	// 움직이는 Vertex 로부터 모양이 변하는 Face 의 목록을 얻느다.
	int	numVertexChange = 0;	// 움직이는 Vertex 의 갯수
	for (int i = 0; i < (int) __numPointReprs; ++i) {
		const CPointRepr&	pointRepr = __pointReprPtr[i];
		DWORD	prID = pointRepr.id;
		D3DXVERTEX&	vertexInfo = vertexArray[prID];
		vertexInfo.pTmp = vertexInfo.p;			// 나중 계산을 위해 임시로 복사해둔다.
		// 색깔 변화를 통해 움직이는 Vertex 인지를 판단한다.
		D3DXVECTOR3&	cColor = vertexInfo.c;
		if (cColor.x == 0.f && cColor.y == 0.f && cColor.z == 0.f) {	// 색깔이 없다면
			continue;	// 건너뛴다.
		}

		pointReprArray[i] = 1;
		++numVertexChange;
		// 이 Vertex 에 연결된 Face 의 목록을 구한다.
#ifdef __USE_A_OFFSET__
		for (int k = pointer_to__a_vFaceIDLocalIndex_a_offset[i]; k < pointer_to__a_vFaceIDLocalIndex_a_offset[i + 1]; ++k) {
			const FACE_ID_LOCAL_INDEX&	faceIDLocalIndex = pointer_to__a_vFaceIDLocalIndex[k];
			faceArray[faceIDLocalIndex.ID] = 1;
		}
#else
		for (size_t k = 0; k < __pointReprPtr[i].vFaceIdLocalIndex.size(); ++k) {
			faceArray[pointRepr.vFaceIdLocalIndex.at(k).ID] = 1;
		}
#endif
	}
	if (numVertexChange == 0) {	// 움직일 Vertex 가 없으므로 원하는 Volume 변화를 얻을 수 없다.
		iReturnVal = -1;
		break;
	}

	// D3DXVERTEX::p 를 이용하여, 모양이 변할 Face 들의 초기 Volume 을 얻는다;
	// 값 자체는 의미가 없다. 변화했을 때의 Volume 과의 차이가 중요하다.
	FLOAT	fVolume0 = 0.f;
	for (int i = 0; i < (int) __numFaces; ++i) {
		if (faceArray[i] == 0) {	// 모양이 변하지 않는 Face는...
			continue;		// 건너뛴다.
		}

		CFace&	face = __facePtr[i];
		DWORD	index0 = face.prID0;
		DWORD	index1 = face.prID1;
		DWORD	index2 = face.prID2;
		D3DXVECTOR3	cross_;
		fVolume0 += D3DXVec3Dot(&vertexArray[index0].p, D3DXVec3Cross(&cross_, &vertexArray[index1].p, &vertexArray[index2].p));
	}
	fVolume0 /= 6.f;

	// 움직일 Vertex 들을 fNormalAmount 에 해당하는 만큼,
	// 색깔에 따라 상대적으로 움직여서 변화된 Volume 을 얻는다.
	// 원하는 Volume 변화가 일어날 때까지 fNormalAmount 를 조절한다.
	// 가장 간단한 Bisection 방식을 사용한다:
	//	1) fNormalAmountLeft(원하는 Volume 변화에 못 미치는 위치) 와
	//	fNormalAmountRight(원하는 Volume 변화를 넘어서는 위치)의 중간 위치(fNormalAmountMid)에서 시작한다.
	//	2) 그 위치에서의 Volume 변화가 원하는 값보다 크다면
	//	fNormalAmountLeft = fNormalAmountMid 가 되고,
	//	적다면 fNormalAmountRight = fNormalAmountMid 가 된다.
	//	3) 원하는 Volume 변화가 될 때까지 위 과정을 반복한다.

	// fNormalAmountLeft 와 fNormalAmountRight 를 결정한다:
	//	1) volumeChange > 0 인 경우,
	//	fNormalAmountLeft = 0, fNormalAmountRight = fNormalAmountGuess 에서 시작해서
	//	fNormalAmountRight 에서의 Volume 변화와 원하는 Volume 변화의 차이의 부호가 변화할 때까지
	//	fNormalAmountRight 값을 2배씩 증가시킨다;
	//	증가시킬 때 fNormalAmountRight 가 fNormalAmountLeft 가 된다.
	//	2) volumeChange < 0 인 경우,
	//	fNormalAmountLeft = -fNormalAmountGuess, fNormalAmountRight = 0 에서 시작해서
	//	fNormalAmountLeft 값을 2배씩 증가시킨다;
	//	증가시킬 때 fNormalAmountLeft 가 fNormalAmountRight 가 된다.
	// TO DO LIST: Volume 을 계산하는 식의 특성상 fNormalAmountRight(volumeChange > 0),
	// fNormalAmountLeft(volumeChange < 0) 를 증가시킨다고 해서
	// 반드시 원하는 방향으로 Volume 변화가 일어나는 것은 아니다;
	// 예를 들어, fNormalAmountRight 가 너무 크면 어느 순간에 Volume 변화가 음수가 될 수도 있다.
	// fNormalAmountGuess 가 너무 크지 않고(오브젝트의 크기: __a 의 0.01 정도?),
	// Volume 변화(volumeChange)가 너무 크지 않다면 그런대로 적용할 수 있을 것이다.
	FLOAT	fNormalAmountLeft;
	FLOAT	fNormalAmountRight;
	if (volumeChange < 0) {
		fNormalAmountLeft = -fNormalAmountGuess;
		fNormalAmountRight = 0.f;
	} else {	// volumeChange > 0
		fNormalAmountLeft = 0.f;
		fNormalAmountRight = fNormalAmountGuess;
	}
	assert(volumeChange != 0.f);
	bool	found = false;
	unsigned	iter = max_iter;
	do {
		FLOAT	fNormalAmountBoundary = volumeChange > 0 ? fNormalAmountRight : fNormalAmountLeft;
		if (iter == 0) {
			found = false;
			break;
		}

		// fNormalAmount 에 해당하는 양만큼, 각 Vertex 의 색깔에 따라 상대적으로,
		// 각 Vertex 를 움직인다.
		for (DWORD i = 0; i < __numPointReprs; ++i) {
			if (pointReprArray[i] == 0) {	// 움직이지 않는 Vertex 라면
				continue;		// 건너뛴다.
			}

			const CPointRepr&	pointRepr = __pointReprPtr[i];
			D3DXVERTEX&	vertexInfo = vertexArray[pointRepr.id];
			const D3DXVECTOR3&	cColor = vertexInfo.c;
			FLOAT	f = (cColor.x + cColor.y + cColor.z) / 3.f;	// 상대적인 양
			D3DXVECTOR3	normalScale = vertexInfo.n;
			normalScale *= f * fNormalAmountBoundary;
			vertexInfo.pTmp = vertexInfo.p + normalScale;
		}

		// Vertex 가 움직였을 때 변화된 Volume 을 계산한다.
		FLOAT	fVolumeTmp = 0.f;
		for (DWORD i = 0; i < __numFaces; ++i) {
			if (faceArray[i] == 0) {	// 모양이 변하지 않는 Face는...
				continue;		// 건너뛴다.
			}

			CFace&	face = __facePtr[i];
			DWORD	index0 = face.prID0;
			DWORD	index1 = face.prID1;
			DWORD	index2 = face.prID2;
			D3DXVECTOR3	cross;
			fVolumeTmp += D3DXVec3Dot(&vertexArray[index0].pTmp, D3DXVec3Cross(&cross, &vertexArray[index1].pTmp, &vertexArray[index2].pTmp));
		}
		fVolumeTmp /= 6.f;
		FLOAT	fVolumeChange = fVolumeTmp - fVolume0;
		--iter;
#if 1
		if (volumeChange > 0) {
			if (fVolumeChange > volumeChange) {	// 더 많이 늘어났으므로
				found = true;			// fNormalAmountLeft, fNormalAmountRight 사이에서
								// 원하는 volumeChange 가 있을 것이다.
				break;
			}
		} else {
			if (fVolumeChange > 0.f || fVolumeChange < volumeChange) {	// 더 많이 줄어들었으므로
				found = true;			// fNormalAmountLeft, fNormalAmountRight 사이에서
								// 원하는 volumeChange 가 있을 것이다.
				break;
			}
		}
#else
		if (volumeChange > 0) {
			if (fVolumeChange < 0.f || fVolumeChange > volumeChange) {	// 더 많이 늘어났으므로
				found = true;			// fNormalAmountLeft, fNormalAmountRight 사이에서
								// 원하는 volumeChange 가 있을 것이다.
				break;
			}
		} else {
			if (fVolumeChange > 0.f || fVolumeChange < volumeChange) {	// 더 많이 줄어들었으므로
				found = true;			// fNormalAmountLeft, fNormalAmountRight 사이에서
								// 원하는 volumeChange 가 있을 것이다.
				break;
			}
		}
#endif

		if (volumeChange > 0) {
			fNormalAmountLeft = fNormalAmountRight;
			fNormalAmountRight *= 2.f;
		} else {
			fNormalAmountRight = fNormalAmountLeft;
			fNormalAmountLeft *= 2.f;
		}
		// 다시 반복한다.
	} while (1);
	if (!found) {	// Volume 변화의 부호가 바뀌는 위치를 찾지 못했다.
		iReturnVal = -2;
		break;
	}

	// Bisection 을 적용하여 원하는 Volume 변화가 일어나는 fNormalAmountMid 를 찾는다;
	// fNormalAmountLeft 와 fNormalAmountRight 사이에서 원하는 Volume 변화가 있을 것이다.
	// TO DO LIST: fNormalAmountLeft 와 fNormalAmountRight 사이가 너무 크다면(fNormalAmountGuess 값이 커서)
	// 이 사이에서 Volume 변화가 연속적이라는 가정이다.
	do {
		FLOAT	fNormalAmountMid = .5f * (fNormalAmountLeft + fNormalAmountRight);
		if (max_iter == 0) {
			found = false;
			break;
		}

		// fNormalAmount 에 해당하는 양만큼, 각 Vertex 의 색깔에 따라 상대적으로,
		// 각 Vertex 를 움직인다.
		for (DWORD i = 0; i < __numPointReprs; ++i) {
			if (pointReprArray[i] == 0) {	// 움직이지 않는 Vertex 라면
				continue;		// 건너뛴다.
			}

			const CPointRepr&	pointRepr = __pointReprPtr[i];
			D3DXVERTEX&	vertexInfo = vertexArray[pointRepr.id];
			const D3DXVECTOR3&	cColor = vertexInfo.c;
			FLOAT	f = (cColor.x + cColor.y + cColor.z) / 3.f;	// 상대적인 양
			D3DXVECTOR3	normalScale = vertexInfo.n;
			normalScale *= f * fNormalAmountMid;
			vertexInfo.pTmp = vertexInfo.p + normalScale;
		}

		// Vertex 가 움직였을 때 변화된 Volume 을 계산한다.
		FLOAT	fVolumeTmp = 0.f;
		for (DWORD i = 0; i < __numFaces; ++i) {
			if (faceArray[i] == 0) {	// 모양이 변하지 않는 Face는...
				continue;		// 건너뛴다.
			}

			CFace&	face = __facePtr[i];
			DWORD	index0 = face.prID0;
			DWORD	index1 = face.prID1;
			DWORD	index2 = face.prID2;
			D3DXVECTOR3	cross;
			fVolumeTmp += D3DXVec3Dot(&vertexArray[index0].pTmp, D3DXVec3Cross(&cross, &vertexArray[index1].pTmp, &vertexArray[index2].pTmp));
		}
		fVolumeTmp /= 6.f;
		FLOAT	fVolumeChange = fVolumeTmp - fVolume0;
		--max_iter;
		if (fabsf(fVolumeChange - volumeChange) < volumeChangeTolerance) {
			fNormalAmount = fNormalAmountMid;
			found = true;
			break;
		}

#if 1
		if (fVolumeChange > volumeChange) {	// 너무 많이 늘어났다.
			fNormalAmountRight = fNormalAmountMid;
		} else {	// 너무 적게 늘어났다.
			fNormalAmountLeft = fNormalAmountMid;
		}
#else
		if (volumeChange > 0) {
			// fVolumeChange < 0 라면 너무너무 많이 늘어났다.
			if (fVolumeChange < 0.f || fVolumeChange > volumeChange) {	// 너무 많이 늘어났다.
				fNormalAmountRight = fNormalAmountMid;
			} else {	// 너무 적게 늘어났다.
				fNormalAmountLeft = fNormalAmountMid;
			}
		} else {
			// fVolumeChange > 0 라면 너무너무 많이 줄어들었다.
			if (fVolumeChange > 0.f || fVolumeChange < volumeChange) {	// 너무 많이 줄어들었다.
				fNormalAmountLeft = fNormalAmountMid;
			} else {	// 너무 적게 늘어났다.
				fNormalAmountRight = fNormalAmountMid;
			}
		}
#endif
		// 다시 반복한다.
	} while (1);
	if (!found) {	// 원하는 Volume 변화가 일어나는 위치를 찾지 못했다.
		iReturnVal = -3;
		break;
	}

	// 찾았다!
	iReturnVal = 1;
	break;
} while (0);
	if (pointReprArray) {
		delete[] pointReprArray;
		pointReprArray = 0;
	}
	if (faceArray) {
		delete[] faceArray;
		faceArray = 0;
	}
	__vertexBufferPtr->Unlock();

	return iReturnVal;
}

int	kMesh::ShowShape(float volumeChange) const
{
	// 주어진 Volume 변화를 가져오려면 각 Vertex 를 "최대" 얼마나 움직여야 하나?
	float	fNormalAmount;	// 실제로는 각 Vertex 의 색깔에 따라 상대적으로 움직인다.
	unsigned	max_iter = 100;
	int	iReturnVal = findNormalAmountByVolumeChange(
		fNormalAmount,
		0.01f * __a,
		volumeChange,
		max_iter,
		0.001f * volumeChange
	);
	if (iReturnVal != 1)
		return -1;

	HRESULT	hResult;

	kMesh::D3DXVERTEX*	vertexArray;
	__vertexBufferPtr->Lock(0, 0, (void**) &vertexArray, 0);
	DWORD*	pointReprArray = 0;	// 움직이는 Vertex 목록
	DWORD*	faceArray = 0;	// Vertex 가 움직임으로써 모양이 변하는 Face 목록;
				// TO DO LIST: 비트면 충분한데...
do {
	pointReprArray = new DWORD[__numPointReprs];
	faceArray = new DWORD[__numFaces];
	// 0(False)으로 초기화한다.
	FillMemory(pointReprArray, __numPointReprs * sizeof(DWORD), 0);
	FillMemory(faceArray, __numFaces * sizeof(DWORD), 0);

	// 움직이는 Vertex 로부터 모양이 변하는 Face 의 목록을 얻느다.
	int	numVertexChange = 0;	// 움직이는 Vertex 의 갯수
	for (int i = 0; i < (int) __numPointReprs; ++i) {
		const CPointRepr&	pointRepr = __pointReprPtr[i];
		DWORD	prID = pointRepr.id;
		D3DXVERTEX&	vertexInfo = vertexArray[prID];
		vertexInfo.pTmp = vertexInfo.p;			// 나중 계산을 위해 임시로 복사해둔다.
		// 색깔 변화를 통해 움직이는 Vertex 인지를 판단한다.
		D3DXVECTOR3&	cColor = vertexInfo.c;
		if (cColor.x == 0.f && cColor.y == 0.f && cColor.z == 0.f) {	// 색깔이 없다면
			continue;	// 건너뛴다.
		}

		pointReprArray[i] = 1;
		++numVertexChange;
		// 이 Vertex 에 연결된 Face 의 목록을 구한다.
#ifdef __USE_A_OFFSET__
		for (int k = pointer_to__a_vFaceIDLocalIndex_a_offset[i]; k < pointer_to__a_vFaceIDLocalIndex_a_offset[i + 1]; ++k) {
			const FACE_ID_LOCAL_INDEX&	faceIDLocalIndex = pointer_to__a_vFaceIDLocalIndex[k];
			faceArray[faceIDLocalIndex.ID] = 1;
		}
#else
		for (size_t k = 0; k < __pointReprPtr[i].vFaceIdLocalIndex.size(); ++k) {
			faceArray[pointRepr.vFaceIdLocalIndex.at(k).ID] = 1;
		}
#endif
	}
	int	numFaceChange = 0;	// 모양이 변하는(적어도 하나의 Vertex 가 움직여서) Face 의 갯수
	for (int i = 0; i < (int) __numFaces; ++i) {
		if (faceArray[i])
			++numFaceChange;
	}
	if (numVertexChange == 0) {	// 움직일 Vertex 가 없으므로 원하는 Volume 변화를 얻을 수 없다.
		iReturnVal = -1;
		break;
	}

#if 1	// 점으로 표시
	LPDIRECT3DVERTEXBUFFER9	shapeVertexBufferPtr = NULL;
	__devicePtr->CreateVertexBuffer(
		numVertexChange * sizeof(D3DXVECTOR3),
		D3DUSAGE_WRITEONLY,
		0,
		D3DPOOL_MANAGED,
		&shapeVertexBufferPtr,
		NULL
	);
	D3DXVECTOR3*	shapeVertexPtr;
	shapeVertexBufferPtr->Lock(0, 0, (void**) &shapeVertexPtr, 0);
do {
	int	n = 0;
	for (DWORD i = 0; i < __numPointReprs; ++i) {
		if (pointReprArray[i] == 0) {	// 움직이지 않는 Vertex 라면
			continue;		// 건너뛴다.
		}

		const CPointRepr&	pointRepr = __pointReprPtr[i];
		D3DXVERTEX&	vertexInfo = vertexArray[pointRepr.id];
		const D3DXVECTOR3&	cColor = vertexInfo.c;
		FLOAT	f = (cColor.x + cColor.y + cColor.z) / 3.f;	// 상대적인 양
		D3DXVECTOR3	normalScale = vertexInfo.n;
		normalScale *= f * fNormalAmount;
		vertexInfo.pTmp = vertexInfo.p + normalScale;
		shapeVertexPtr[n++] = vertexInfo.pTmp;
	}

	__effectPtr->SetInt("g__FillMode", D3DFILL_POINT);
	__effectPtr->SetMatrix("g_mWorldViewProjection", &__worldViewProjectionMatrix);
	__effectPtr->SetMatrix("g_mWorld", &__worldMatrix);
	FLOAT				pointSize = 3.f;
	D3DXVECTOR4			pointColor(1, 1, 0, 1);
	__effectPtr->SetFloat("pointSize", pointSize);
	__effectPtr->SetVector("pointColor", &pointColor);
	D3DXHANDLE	hTechnique = __effectPtr->GetTechnique(0);
	assert(hTechnique);
	hResult = __effectPtr->SetTechnique(hTechnique);
	assert(S_OK == hResult);
	UINT	NumberOfPasses;
	hResult = __effectPtr->Begin(&NumberOfPasses, 0);
	assert(D3D_OK == hResult);
	for (UINT iPass = 0; iPass < NumberOfPasses; ++iPass) {
		hResult = __effectPtr->BeginPass(iPass);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetVertexDeclaration(__OnePointDeclarationPtr);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetStreamSource(0, shapeVertexBufferPtr, 0, sizeof(D3DXVECTOR3));
		assert(D3D_OK == hResult);
		hResult = __devicePtr->DrawPrimitive(D3DPT_POINTLIST, 0, numVertexChange);
		assert(D3D_OK == hResult);
		hResult = __effectPtr->EndPass();
		assert(S_OK == hResult);
	}
	hResult = __effectPtr->End();
	assert(S_OK == hResult);
} while (0);
	shapeVertexBufferPtr->Unlock();
	if (shapeVertexBufferPtr) {
		shapeVertexBufferPtr->Release();
		shapeVertexBufferPtr = 0;
	}
#endif

	// 선으로 표시
#if 1
	LPDIRECT3DVERTEXBUFFER9	shapeLineBufferPtr = NULL;
	__devicePtr->CreateVertexBuffer(
		2 * 3 * numFaceChange * sizeof(D3DXVECTOR3),	// 선을 표시하기 위해서는 2개의 점이 필요하다.
		D3DUSAGE_WRITEONLY,
		0,
		D3DPOOL_MANAGED,
		&shapeLineBufferPtr,
		NULL
	);
	D3DXVECTOR3*	shapeLinePtr;
	shapeLineBufferPtr->Lock(0, 0, (void**) &shapeLinePtr, 0);
do {
	int	numLines = 0;
	for (int i = 0; i < (int) __numFaces; ++i) {
		if (faceArray[i] == 0)
			continue;

		CFace&	face = __facePtr[i];
		// 하나의 Face 당 세 개의 Edge
		D3DXVERTEX&	v0 = vertexArray[face.prID0];
		D3DXVERTEX&	v1 = vertexArray[face.prID1];
		D3DXVERTEX&	v2 = vertexArray[face.prID2];
		// prID0, prID1
		shapeLinePtr[2 * numLines + 0] = v0.pTmp;
		shapeLinePtr[2 * numLines + 1] = v1.pTmp;
		++numLines;
		// prID1, prID2
		shapeLinePtr[2 * numLines + 0] = v1.pTmp;
		shapeLinePtr[2 * numLines + 1] = v2.pTmp;
		++numLines;
		// prID2, prID0
		shapeLinePtr[2 * numLines + 0] = v2.pTmp;
		shapeLinePtr[2 * numLines + 1] = v0.pTmp;
		++numLines;
	}

	__effectPtr->SetInt("g__FillMode", D3DFILL_WIREFRAME);
	__effectPtr->SetMatrix("g_mWorldViewProjection", &__worldViewProjectionMatrix);
	__effectPtr->SetMatrix("g_mWorld", &__worldMatrix);
	D3DXVECTOR4	lineColor(0.f, 1.f, 1.f, 1.f);
	__effectPtr->SetVector("pointColor", &lineColor);
	D3DXHANDLE	hTechnique = __effectPtr->GetTechnique(0);
	assert(hTechnique);
	hResult = __effectPtr->SetTechnique(hTechnique);
	assert(S_OK == hResult);
	UINT	NumberOfPasses;
	assert(D3D_OK == hResult);
	hResult = __effectPtr->Begin(&NumberOfPasses, 0);
	for (UINT iPass = 0; iPass < NumberOfPasses; ++iPass) {
		hResult = __effectPtr->BeginPass(iPass);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetVertexDeclaration(__OnePointDeclarationPtr);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetStreamSource(0, shapeLineBufferPtr, 0, sizeof(D3DXVECTOR3));
		assert(D3D_OK == hResult);
		hResult = __devicePtr->DrawPrimitive(D3DPT_LINELIST, 0, numLines);
		assert(D3D_OK == hResult);
		hResult = __effectPtr->EndPass();
		assert(S_OK == hResult);
	}
	hResult = __effectPtr->End();
	assert(D3D_OK == hResult);
} while (0);
	if (shapeLineBufferPtr) {
		shapeLineBufferPtr->Release();
		shapeLineBufferPtr = 0;
	}
#endif

} while (0);
	if (faceArray) {
		delete[] faceArray;
		faceArray = 0;
	}
	if (pointReprArray) {
		delete[] pointReprArray;
		pointReprArray = 0;
	}
	__vertexBufferPtr->Unlock();

	return 0;
}


int	kMesh::ShowBoundingBox() const
{
	LPDIRECT3DVERTEXBUFFER9		lineVertexBuffer = 0;
	LPDIRECT3DINDEXBUFFER9		lineIndexBuffer = 0;
do {
	HRESULT hResult = __devicePtr->CreateVertexBuffer(
		8 * sizeof(D3DXVECTOR3),	// 직육면체(Bounding Box)에는 8개의 꼭지점(Vertex)가 있다.
		D3DUSAGE_WRITEONLY,
		0,
		D3DPOOL_MANAGED,
		&lineVertexBuffer,
		NULL
	);
	assert(hResult == D3D_OK);
	hResult = __devicePtr->CreateIndexBuffer(
		2 * 12 * sizeof(DWORD),	// 직육면체(Bounding Box)는 12개의 모서리가 있고, 모서리당 2개의 Vertex
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX32,
		D3DPOOL_MANAGED,
		&lineIndexBuffer,
		NULL
	);
	assert(D3D_OK == hResult);

	D3DXVECTOR3*	linePointPtr = NULL;
	hResult = lineVertexBuffer->Lock(0, 0, (void**) &linePointPtr, 0);
	assert(D3D_OK == hResult);
	assert(linePointPtr);
	float	left__ = __left_lower_near.x;
	float	lower__ = __left_lower_near.y;
	float	near__ = __left_lower_near.z;
	float	right__ = __right_upper_far.x;
	float	upper__ = __right_upper_far.y;
	float	far__ = __right_upper_far.z;
	// 밑면 4개
	linePointPtr[0].x = left__;
	linePointPtr[0].y = lower__;
	linePointPtr[0].z = near__;
	linePointPtr[1].x = right__;
	linePointPtr[1].y = lower__;
	linePointPtr[1].z = near__;
	linePointPtr[2].x = right__;
	linePointPtr[2].y = lower__;
	linePointPtr[2].z = far__;
	linePointPtr[3].x = left__;
	linePointPtr[3].y = lower__;
	linePointPtr[3].z = far__;
	// 윗면 4개: y값만 lower__ 에서 upper__ 로 바뀜
	linePointPtr[4 + 0].x = left__;
	linePointPtr[4 + 0].y = upper__;
	linePointPtr[4 + 0].z = near__;
	linePointPtr[4 + 1].x = right__;
	linePointPtr[4 + 1].y = upper__;
	linePointPtr[4 + 1].z = near__;
	linePointPtr[4 + 2].x = right__;
	linePointPtr[4 + 2].y = upper__;
	linePointPtr[4 + 2].z = far__;
	linePointPtr[4 + 3].x = left__;
	linePointPtr[4 + 3].y = upper__;
	linePointPtr[4 + 3].z = far__;
	hResult = lineVertexBuffer->Unlock();
	assert(D3D_OK == hResult);

	DWORD*			lineIndicesPtr;
	lineIndexBuffer->Lock(0, 0, (void**) &lineIndicesPtr, 0);
	// 밑면 4개
	lineIndicesPtr[2 * 0 + 0] = 0;
	lineIndicesPtr[2 * 0 + 1] = 1;
	lineIndicesPtr[2 * 1 + 0] = 1;
	lineIndicesPtr[2 * 1 + 1] = 2;
	lineIndicesPtr[2 * 2 + 0] = 2;
	lineIndicesPtr[2 * 2 + 1] = 3;
	lineIndicesPtr[2 * 3 + 0] = 3;
	lineIndicesPtr[2 * 3 + 1] = 0;
	// 윗면 4개
	lineIndicesPtr[2 * 4 + 0] = 4 + 0;
	lineIndicesPtr[2 * 4 + 1] = 4 + 1;
	lineIndicesPtr[2 * 5 + 0] = 4 + 1;
	lineIndicesPtr[2 * 5 + 1] = 4 + 2;
	lineIndicesPtr[2 * 6 + 0] = 4 + 2;
	lineIndicesPtr[2 * 6 + 1] = 4 + 3;
	lineIndicesPtr[2 * 7 + 0] = 4 + 3;
	lineIndicesPtr[2 * 7 + 1] = 4 + 0;
	// 옆면 4개
	lineIndicesPtr[2 * 8 + 0] = 0;
	lineIndicesPtr[2 * 8 + 1] = 4 + 0;
	lineIndicesPtr[2 * 9 + 0] = 1;
	lineIndicesPtr[2 * 9 + 1] = 4 + 1;
	lineIndicesPtr[2 * 10 + 0] = 2;
	lineIndicesPtr[2 * 10 + 1] = 4 + 2;
	lineIndicesPtr[2 * 11 + 0] = 3;
	lineIndicesPtr[2 * 11 + 1] = 4 + 3;
	lineIndexBuffer->Unlock();

	hResult = __effectPtr->SetInt("g__FillMode", D3DFILL_WIREFRAME);
	assert(D3D_OK == hResult);
	hResult = __effectPtr->SetVector("pointColor", &D3DXVECTOR4(1, 0, 1, 1));
	D3DXHANDLE	hTechnique = __effectPtr->GetTechnique(0);
	assert(hTechnique);
	hResult = __effectPtr->SetTechnique(hTechnique);
	assert(S_OK == hResult);
	UINT	NumberOfPasses;
	hResult = __effectPtr->Begin(&NumberOfPasses, 0);
	assert(D3D_OK == hResult);
	for (UINT iPass = 0; iPass < NumberOfPasses; ++iPass) {
		hResult = __effectPtr->BeginPass(iPass);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetVertexDeclaration(__OnePointDeclarationPtr);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetStreamSource(0, lineVertexBuffer, 0, sizeof(D3DXVECTOR3));
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetIndices(lineIndexBuffer);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->DrawIndexedPrimitive(D3DPT_LINELIST, 0, 0, 8, 0, 12);
		assert(D3D_OK == hResult);
		hResult = __effectPtr->EndPass();
		assert(S_OK == hResult);
	}
	hResult = __effectPtr->End();
	assert(S_OK == hResult);
} while (0);
	if (lineVertexBuffer) {
		lineVertexBuffer->Release();
		lineVertexBuffer = NULL;
	}
	if (lineIndexBuffer) {
		lineIndexBuffer->Release();
		lineIndexBuffer = 0;
	}

	return 0;
}

int	kMesh::ShowBoundingSphere() const
{
	LPDIRECT3DVERTEXBUFFER9		lineVertexBuffer = 0;
	LPDIRECT3DINDEXBUFFER9		lineIndexBuffer = 0;
do {
	UINT	nStep = 5;	// 360 의 약수이어야 한다!
	UINT	nPoints = 360 / nStep;
	HRESULT hResult = __devicePtr->CreateVertexBuffer(
		nPoints * sizeof(D3DXVECTOR3),
		D3DUSAGE_WRITEONLY,
		0,
		D3DPOOL_MANAGED,
		&lineVertexBuffer,
		NULL
	);
	assert(hResult == D3D_OK);
	hResult = __devicePtr->CreateIndexBuffer(
		2 * nPoints * sizeof(DWORD),
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX32,
		D3DPOOL_MANAGED,
		&lineIndexBuffer,
		NULL
	);
	assert(D3D_OK == hResult);

	float	length = __right_upper_far.z - __left_lower_near.z;
	float	ell;
	if (length < (ell = __right_upper_far.y - __left_lower_near.y))
		length = ell;
	if (length < (ell = __right_upper_far.x - __left_lower_near.x))
		length = ell;
	float	radius = .5f * length;
	// xy 평면
{
	D3DXVECTOR3*	linePointPtr = NULL;
	hResult = lineVertexBuffer->Lock(0, 0, (void**) &linePointPtr, 0);
	DWORD*		lineIndicesPtr = 0;
	hResult = lineIndexBuffer->Lock(0, 0, (void**) &lineIndicesPtr, 0);
	assert(D3D_OK == hResult);
	int	index = 0;
	for (int n = 0; n < (int) nPoints; ++n, ++index) {
		float	angle = nStep * n * D3DX_PI / 180.f;
		FLOAT	x = __pivotCenter.x + radius * cosf(angle);
		FLOAT	y = __pivotCenter.y + radius * sinf(angle);
		linePointPtr[index] = D3DXVECTOR3(x, y, __pivotCenter.z);
		lineIndicesPtr[2 * index + 0] = index;
		lineIndicesPtr[2 * index + 1] = index == (int) nPoints - 1 ? 0 : index + 1;
	}
	hResult = lineVertexBuffer->Unlock();
	assert(D3D_OK == hResult);
	lineIndexBuffer->Unlock();

	hResult = __effectPtr->SetInt("g__FillMode", D3DFILL_WIREFRAME);
	assert(D3D_OK == hResult);
	hResult = __effectPtr->SetVector("pointColor", &D3DXVECTOR4(0, 0, 1, 1));
	D3DXHANDLE	hTechnique = __effectPtr->GetTechnique(0);
	assert(hTechnique);
	hResult = __effectPtr->SetTechnique(hTechnique);
	assert(S_OK == hResult);
	UINT	NumberOfPasses;
	hResult = __effectPtr->Begin(&NumberOfPasses, 0);
	assert(D3D_OK == hResult);
	for (UINT iPass = 0; iPass < NumberOfPasses; ++iPass) {
		hResult = __effectPtr->BeginPass(iPass);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetVertexDeclaration(__OnePointDeclarationPtr);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetStreamSource(0, lineVertexBuffer, 0, sizeof(D3DXVECTOR3));
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetIndices(lineIndexBuffer);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->DrawIndexedPrimitive(D3DPT_LINELIST, 0, 0, nPoints, 0, nPoints);
		assert(D3D_OK == hResult);
		hResult = __effectPtr->EndPass();
		assert(S_OK == hResult);
	}
	hResult = __effectPtr->End();
	assert(S_OK == hResult);
}
	// yz 평면
{
	D3DXVECTOR3*	linePointPtr = NULL;
	hResult = lineVertexBuffer->Lock(0, 0, (void**) &linePointPtr, 0);
	DWORD*		lineIndicesPtr = 0;
	hResult = lineIndexBuffer->Lock(0, 0, (void**) &lineIndicesPtr, 0);
	assert(D3D_OK == hResult);
	int	index = 0;
	for (int n = 0; n < (int) nPoints; ++n, ++index) {
		float	angle = nStep * n * D3DX_PI / 180.f;
		FLOAT	y = __pivotCenter.y + radius * cosf(angle);
		FLOAT	z = __pivotCenter.z + radius * sinf(angle);
		linePointPtr[index] = D3DXVECTOR3(__pivotCenter.x, y, z);
		lineIndicesPtr[2 * index + 0] = index;
		lineIndicesPtr[2 * index + 1] = index == (int) nPoints - 1 ? 0 : index + 1;
	}
	hResult = lineVertexBuffer->Unlock();
	assert(D3D_OK == hResult);
	lineIndexBuffer->Unlock();

	hResult = __effectPtr->SetInt("g__FillMode", D3DFILL_WIREFRAME);
	assert(D3D_OK == hResult);
	hResult = __effectPtr->SetVector("pointColor", &D3DXVECTOR4(1, 0, 0, 1));
	D3DXHANDLE	hTechnique = __effectPtr->GetTechnique(0);
	assert(hTechnique);
	hResult = __effectPtr->SetTechnique(hTechnique);
	assert(S_OK == hResult);
	UINT	NumberOfPasses;
	hResult = __effectPtr->Begin(&NumberOfPasses, 0);
	assert(D3D_OK == hResult);
	for (UINT iPass = 0; iPass < NumberOfPasses; ++iPass) {
		hResult = __effectPtr->BeginPass(iPass);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetVertexDeclaration(__OnePointDeclarationPtr);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetStreamSource(0, lineVertexBuffer, 0, sizeof(D3DXVECTOR3));
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetIndices(lineIndexBuffer);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->DrawIndexedPrimitive(D3DPT_LINELIST, 0, 0, nPoints, 0, nPoints);
		assert(D3D_OK == hResult);
		hResult = __effectPtr->EndPass();
		assert(S_OK == hResult);
	}
	hResult = __effectPtr->End();
	assert(S_OK == hResult);
}
	// zx 평면
{
	D3DXVECTOR3*	linePointPtr = NULL;
	hResult = lineVertexBuffer->Lock(0, 0, (void**) &linePointPtr, 0);
	DWORD*		lineIndicesPtr = 0;
	hResult = lineIndexBuffer->Lock(0, 0, (void**) &lineIndicesPtr, 0);
	assert(D3D_OK == hResult);
	int	index = 0;
	for (int n = 0; n < (int) nPoints; ++n, ++index) {
		float	angle = nStep * n * D3DX_PI / 180.f;
		FLOAT	z = __pivotCenter.z + radius * cosf(angle);
		FLOAT	x = __pivotCenter.x + radius * sinf(angle);
		linePointPtr[index] = D3DXVECTOR3(x, __pivotCenter.y, z);
		lineIndicesPtr[2 * index + 0] = index;
		lineIndicesPtr[2 * index + 1] = index == (int) nPoints - 1 ? 0 : index + 1;
	}
	hResult = lineVertexBuffer->Unlock();
	assert(D3D_OK == hResult);
	lineIndexBuffer->Unlock();

	hResult = __effectPtr->SetInt("g__FillMode", D3DFILL_WIREFRAME);
	assert(D3D_OK == hResult);
	hResult = __effectPtr->SetVector("pointColor", &D3DXVECTOR4(0, 1, 0, 1));
	D3DXHANDLE	hTechnique = __effectPtr->GetTechnique(0);
	assert(hTechnique);
	hResult = __effectPtr->SetTechnique(hTechnique);
	assert(S_OK == hResult);
	UINT	NumberOfPasses;
	hResult = __effectPtr->Begin(&NumberOfPasses, 0);
	assert(D3D_OK == hResult);
	for (UINT iPass = 0; iPass < NumberOfPasses; ++iPass) {
		hResult = __effectPtr->BeginPass(iPass);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetVertexDeclaration(__OnePointDeclarationPtr);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetStreamSource(0, lineVertexBuffer, 0, sizeof(D3DXVECTOR3));
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetIndices(lineIndexBuffer);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->DrawIndexedPrimitive(D3DPT_LINELIST, 0, 0, nPoints, 0, nPoints);
		assert(D3D_OK == hResult);
		hResult = __effectPtr->EndPass();
		assert(S_OK == hResult);
	}
	hResult = __effectPtr->End();
	assert(S_OK == hResult);
}
} while (0);
	if (lineVertexBuffer) {
		lineVertexBuffer->Release();
		lineVertexBuffer = NULL;
	}
	if (lineIndexBuffer) {
		lineIndexBuffer->Release();
		lineIndexBuffer = 0;
	}

	return 0;
}

#if 0	// group
void	kMesh::ShowGroup(
	const kID&	gid,
	int	showType	// 0x01:= Vertex, 0x02:= Edge, 0x04:= Face
) const
{
	const kMeshLoader::GROUP_INFO_HASH&	__groupMap = __meshLoader.groupMap();
	if (!__groupMap.size()) {
		(void) fprintf(stderr, "kMesh::ShowGroup: No group\n");
		return;
	}
	if (gid.isNull()) {
		(void) fprintf(stderr, "kMesh::ShowGroup: gid is null\n");
		return;
	}
	(void) fprintf(stdout, "kMesh::ShowGroup: gid = %d\n", gid.id());

	// gid 에 해당하는 fid 의 목록은...
	// gid 가 유효한가?
	const kMeshLoader::GID_NAME_HASH&	__gidMap = __meshLoader.gidMap();
	kMeshLoader::GID_NAME_HASH::const_iterator	gid_it = __gidMap.find(gid);
	if (gid_it == __gidMap.end()) {
		(void) fprintf(stderr, "kMesh::ShowGroup: No group for gid = %d\n", gid.id());
		return;
	}
	(void) fprintf(stdout, "kMesh::ShowGroup: |%s| for gid = %d\n", gid_it->second.c_str(), gid.id());
	// fid 목록은 얻는다.
	kMeshLoader::GROUP_INFO_HASH::const_iterator	group_it = __groupMap.find(gid_it->second);
	if (group_it == __groupMap.end()) {
		(void) fprintf(stderr, "FATAL: kMesh::ShowGroup: No fid list (gid = %d, |%s|)\n", gid.id(), gid_it->second.c_str());
		return;
	}
	const struct kMeshLoader::GROUP_INFO&	groupInfo = group_it->second;
	(void) fprintf(stdout, "kMesh::ShowGroup: |%s| (%d)\n", gid_it->second.c_str(), gid.id());
#if 0	// Debug
	for (int i = 0; i < (int) groupInfo.__fidList.size(); ++i)
		fprintf(stdout, "\t%d\n", groupInfo.__fidList.at(i).id());
#endif
	unsigned	nFace = groupInfo.__fidList.size();
	(void) fprintf(stdout, "kMesh::ShowGroup: # of face(s) = %d\n", nFace);

	if (showType & 0x01) {	// Vertex
		(void) fprintf(stdout, "kMesh::ShowGroup: w/ Vertex\n");
		LPDIRECT3DVERTEXBUFFER9		vertexBufferPtr = 0;
		DWORD*		pointReprIDArray = 0;	// 선택된 Face 에 속하는 Vertex ID 배열; 값이 1 이면 해당 Vertex 가 속한다.
	do {
		pointReprIDArray = new DWORD[__numPointReprs];
		FillMemory(pointReprIDArray, __numPointReprs * sizeof(DWORD), 0);
		for (int n = 0; n < (int) nFace; ++n) {
			DWORD	faceID = groupInfo.__fidList.at(n).id();
			CFace&	face = __facePtr[faceID];
			pointReprIDArray[face.prID0] = 1;
			pointReprIDArray[face.prID1] = 1;
			pointReprIDArray[face.prID2] = 1;
		}
		unsigned	nPointRepr = 0;		// 선택된 Face 에 속하는 Vertex ID 의 갯수
		for (int n = 0; n < (int) __numPointReprs; ++n) {
			if (!pointReprIDArray[n])
				continue;
			++nPointRepr;
		}

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
		D3DXVERTEX*	vertexPtr;
		vertexBufferPtr->Lock(0, 0, (void**) &vertexPtr, 0);
		int	index = 0;
		for (int n = 0; n < (int) __numPointReprs; ++n) {
			if (!pointReprIDArray[n])
				continue;

			const CPointRepr&	pointRepr = __pointReprPtr[n];
			vertexPtr[index].p = pointRepr.v.p;
			vertexPtr[index].p_ = pointRepr.v.p;
			vertexPtr[index].n = pointRepr.v.n;
			vertexPtr[index].tu = 0.f;
			vertexPtr[index].tv = 0.f;
			vertexPtr[index].tx = -1.f;
			vertexPtr[index].c = D3DXVECTOR3(169.f / 255, 169.f / 255, 169.f / 255);	// dark grey
			++index;
		}
		vertexBufferPtr->Unlock();

		ShowVertex(vertexBufferPtr, __vertexDeclarationPtr, sizeof(kMesh::D3DXVERTEX), nPointRepr, 3.f, D3DXVECTOR4(0, 1, 1, 1));
	} while (0);
		if (pointReprIDArray) {
			delete[] pointReprIDArray;
			pointReprIDArray = 0;
		}
		if (vertexBufferPtr) {
			vertexBufferPtr->Release();
			vertexBufferPtr = 0;
		}
	}
	if (showType & 0x02) {	// Edge
		(void) fprintf(stdout, "kMesh::ShowGroup: w/ Edge\n");
		LPDIRECT3DINDEXBUFFER9		edgeIndexBufferPtr = 0;
		DWORD*		edgeIDArray = 0;	// 선택된 Face 에 속하는 Edge ID 배열; 1이면 해당 Edge 가 속한다.
	do {
		edgeIDArray = new DWORD[__numEdges];
		FillMemory(edgeIDArray, __numEdges * sizeof(DWORD), 0);
		for (int n = 0; n < (int) nFace; ++n) {
			DWORD	faceID = groupInfo.__fidList.at(n).id();
			CFace&	face = __facePtr[faceID];
			edgeIDArray[face.edgeID01.id] = 1;
			edgeIDArray[face.edgeID12.id] = 1;
			edgeIDArray[face.edgeID20.id] = 1;
		}
		unsigned	nEdge = 0;		// 선택된 Face 에 속하는 Edge ID 의 갯수
		for (int n = 0; n < (int) __numEdges; ++n) {
			if (!edgeIDArray[n])
				continue;
			++nEdge;
		}
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
		int	index = 0;
		for (int n = 0; n < (int) __numEdges; ++n) {
			if (!edgeIDArray[n])
				continue;

			const CEdge&	edge = __edgePtr[n];
			DWORD	iStart = edge.prStartID;
			iStart *= N_ELEMENT;
			DWORD	iEnd = edge.prEndID;
			iEnd *= N_ELEMENT;
			edgeIndexPtr[index].iStart = iStart;
			edgeIndexPtr[index].iEnd = iEnd;
			++index;
		}
		edgeIndexBufferPtr->Unlock();

		ShowEdge(edgeIndexBufferPtr, D3DXVECTOR4(0, 1, 1, 1), nEdge);
	} while (0);
		if (edgeIndexBufferPtr) {
			edgeIndexBufferPtr->Release();
			edgeIndexBufferPtr = 0;
		}
		if (edgeIDArray) {
			delete[] edgeIDArray;
			edgeIDArray = 0;
		}
	}
	if (showType & 0x04) {	// Face
		(void) fprintf(stdout, "kMesh::ShowGroup: w/ Face\n");
		LPDIRECT3DINDEXBUFFER9		faceIndexBufferPtr = 0;
	do {
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
		for (int n = 0; n < (int) nFace; ++n) {
			DWORD	faceID = groupInfo.__fidList.at(n).id();
			const CFace&	face = __facePtr[faceID];
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
		ShowFace(faceIndexBufferPtr, nFace);
		__devicePtr->SetRenderState(D3DRS_CULLMODE, cullMode);
	} while (0);
		if (faceIndexBufferPtr) {
			faceIndexBufferPtr->Release();
			faceIndexBufferPtr = 0;
		}
	}
}
#endif

void	kMesh::follow(
	const kMesh&	mesh0,
	bool	bDir
)
{
	const std::set<DWORD>	selectedPrIDSet = __select.selectedPrIDSet();
	if (!selectedPrIDSet.size())	// 선택된 점이 없다.
		return;

	kMesh::D3DXVERTEX*	vertexArray;
	__vertexBufferPtr->Lock(0, 0, (void**) &vertexArray, 0);
	const LPDIRECT3DVERTEXBUFFER9	vertexBufferPtr0 = mesh0.GetVertexBufferPointer();
	kMesh::D3DXVERTEX*	vertexArray0;
	vertexBufferPtr0->Lock(0, 0, (void**) &vertexArray0, 0);
	const LPDIRECT3DVERTEXBUFFER9	selectVertexBufferPtr = __select.GetVertexBufferPointer();
	D3DXVECTOR3*	selectArray = NULL;
	selectVertexBufferPtr->Lock(0, 0, (void**) &selectArray, 0);
	int	n = 0;
	for (std::set<DWORD>::const_iterator it = selectedPrIDSet.begin(); it != selectedPrIDSet.end(); ++it) {
		DWORD	prID = *it;
		FACE_ID_LOCAL_INDEX	faceIDLocalIndex;
		int	rval = GetFaceIDLocalIndex(prID, faceIDLocalIndex);
		assert(rval == 0);
		if (rval == 0) {
			DWORD	prID0 = mesh0.GetPrID(faceIDLocalIndex);
			assert(prID0 != (DWORD) -1);
			if (prID0 != (DWORD) -1) {
				// Q. mesh -> mesh0 로 향하는 벡터는?
				// A. 초기위치인 kMesh::D3DXVERTEX::p_ 에 의해서 결정된다.
				D3DXVECTOR3	nDir = D3DXVECTOR3(vertexArray0[prID0].p_ - vertexArray[prID].p_);
				float	length = D3DXVec3Length(&nDir);	// 최대 length 만큼 움직인다.
				float	step = length / 10.f;		// 많아야 10 단계를 거쳐 움직인다;
									// TO DO LIST: 다른 방식에 따라 움직여야 한다.
				D3DXVec3Normalize(&nDir, &nDir);
				if (bDir == false)
					nDir = -nDir;
				D3DXVECTOR3&	p = vertexArray[prID].p;
				p += step * nDir;
				selectArray[n++] = p;
			}
		}
	}
	selectVertexBufferPtr->Unlock();
	vertexBufferPtr0->Unlock();
	__vertexBufferPtr->Unlock();
}

void	kMesh::standard_set_normal()
{
	if (!__meshPtr0) {
		(void) fprintf(stderr, "No standard mesh\n");
		return;
	}

	// 변화할 영역은?
	std::vector<DWORD>	selectedPrIDArray;
{
	const STANDARD_SIMULATION&	standard = __simulation.getStandardRef();
	const AREA_TYPE_T&	areaType = standard.area;
	D3DXVERTEX*	vertexArray;
	__vertexBufferPtr->Lock(0, 0, (void**) &vertexArray, 0);
//	int	index = 0;
	for (kArea::DEFINED_AREA_DIC::const_iterator area_it = __defined_area_dic.begin(); area_it != __defined_area_dic.end(); ++area_it) {
		if (areaType != area_it->first)
			continue;
		const kArea::DEFINED_AREA&	defArea = area_it->second;
		const std::set<DWORD>&	face_id_set = defArea.face_id_set;
		std::set<DWORD>::const_iterator	it;
		for (std::set<DWORD>::const_iterator it = face_id_set.begin(); it != face_id_set.end(); ++it) {
			DWORD	faceID = *it;
			if (faceID >= __numFaces) {
				(void) fprintf(stderr, "FATAL: kMesh::standard_set_normal: Face ID = %d: Out of range: Ignored\n", faceID);
				continue;
			}
			const CFace&	face = __facePtr[faceID];
			for (int i = 0; i < 3; ++i) {
				selectedPrIDArray.push_back(face.prID[i]);
			}
		}
	}
	__vertexBufferPtr->Unlock();
}

	if (!selectedPrIDArray.size()) {	// 선택된 점이 없다.
		(void) fprintf(stderr, "No selected points\n");
		return;
	}

	kMesh::D3DXVERTEX*	vertexArray;
	__vertexBufferPtr->Lock(0, 0, (void**) &vertexArray, 0);
	const LPDIRECT3DVERTEXBUFFER9	vertexBufferPtr0 = __meshPtr0->GetVertexBufferPointer();
	kMesh::D3DXVERTEX*	vertexArray0;
	vertexBufferPtr0->Lock(0, 0, (void**) &vertexArray0, 0);
//	const LPDIRECT3DVERTEXBUFFER9	selectVertexBufferPtr = __select.GetVertexBufferPointer();
	for (int n = 0; n < (int) selectedPrIDArray.size(); ++n) {
		DWORD	prID = selectedPrIDArray.at(n);
		FACE_ID_LOCAL_INDEX	faceIDLocalIndex;
		int	rval = GetFaceIDLocalIndex(prID, faceIDLocalIndex);
		assert(rval == 0);
		if (rval == 0) {
			DWORD	prID0 = __meshPtr0->GetPrID(faceIDLocalIndex);
			assert(prID0 != (DWORD) -1);
			if (prID0 != (DWORD) -1) {
				D3DXVECTOR3	nDir = D3DXVECTOR3(vertexArray0[prID0].p - vertexArray[prID].p);
				// normal 을 새로 설정한다.
				D3DXVec3Normalize(&vertexArray[prID].n, &nDir);
			}
		}
	}

	vertexBufferPtr0->Unlock();
	__vertexBufferPtr->Unlock();
}
void	kMesh::__standard_total_volume_change(
	float&	totalVolumeChange
)
{
	if (!__meshPtr0) {
		(void) fprintf(stderr, "No standard mesh\n");
		return;
	}

	DWORD*	pPointReprFlag = 0;
do {
	pPointReprFlag = new DWORD[__numPointReprs];
	FillMemory(pPointReprFlag, __numPointReprs * sizeof(DWORD), 0);

	// 변화할 영역은?
	std::vector<DWORD>	selectedPrIDArray;
{
	const STANDARD_SIMULATION&	standard = __simulation.getStandardRef();
	const AREA_TYPE_T&	areaType = standard.area;
	D3DXVERTEX*	vertexArray;
	__vertexBufferPtr->Lock(0, 0, (void**) &vertexArray, 0);
//	int	index = 0;
	for (kArea::DEFINED_AREA_DIC::const_iterator area_it = __defined_area_dic.begin(); area_it != __defined_area_dic.end(); ++area_it) {
		if (!(areaType & area_it->first))
			continue;
		const kArea::DEFINED_AREA&	defArea = area_it->second;
		const std::set<DWORD>&	face_id_set = defArea.face_id_set;
		std::set<DWORD>::const_iterator	it;
		for (std::set<DWORD>::const_iterator it = face_id_set.begin(); it != face_id_set.end(); ++it) {
			DWORD	faceID = *it;
			if (faceID >= __numFaces) {
				(void) fprintf(stderr, "FATAL: kMesh::__standard_total_volume_change: Face ID = %d: Out of range: Ignored\n", faceID);
				continue;
			}
			const CFace&	face = __facePtr[faceID];
			for (int i = 0; i < 3; ++i) {
				DWORD	prID = face.prID[i];
				pPointReprFlag[prID] = 1;
				selectedPrIDArray.push_back(prID);
				vertexArray[prID].pTmp = vertexArray[prID].p;	// 임시로 저장
			}
		}
	}
	__vertexBufferPtr->Unlock();
}

	if (!selectedPrIDArray.size()) {	// 선택된 점이 없다.
		(void) fprintf(stderr, "No selected points\n");
		return;
	}

	kMesh::D3DXVERTEX*	vertexArray;
	__vertexBufferPtr->Lock(0, 0, (void**) &vertexArray, 0);
	const LPDIRECT3DVERTEXBUFFER9	vertexBufferPtr0 = __meshPtr0->GetVertexBufferPointer();
	kMesh::D3DXVERTEX*	vertexArray0;
	vertexBufferPtr0->Lock(0, 0, (void**) &vertexArray0, 0);
//	const LPDIRECT3DVERTEXBUFFER9	selectVertexBufferPtr = __select.GetVertexBufferPointer();
	for (int n = 0; n < (int) selectedPrIDArray.size(); ++n) {
		DWORD	prID = selectedPrIDArray.at(n);
		FACE_ID_LOCAL_INDEX	faceIDLocalIndex;
		int	rval = GetFaceIDLocalIndex(prID, faceIDLocalIndex);
		assert(rval == 0);
		if (rval == 0) {
			DWORD	prID0 = __meshPtr0->GetPrID(faceIDLocalIndex);
			assert(prID0 != (DWORD) -1);
			if (prID0 != (DWORD) -1) {
				vertexArray[prID].p = vertexArray0[prID0].p;	// 가고자 하는 최종 위치로 설정한다.
			}
		}
	}
	float	fVolumeChangeSaved = __fVolumeChange;	// PUSH
	__calculateVolumeChange(vertexArray, pPointReprFlag);
	totalVolumeChange = __fVolumeChange;
	__fVolumeChange = fVolumeChangeSaved;		// POP

	// 원래대로 되돌린다.
	for (int n = 0; n < (int) selectedPrIDArray.size(); ++n) {
		DWORD	prID = selectedPrIDArray.at(n);
		vertexArray[prID].p = vertexArray[prID].pTmp;
	}

	vertexBufferPtr0->Unlock();
	__vertexBufferPtr->Unlock();
} while (0);
	if (pPointReprFlag) {
		delete[] pPointReprFlag;
		pPointReprFlag = 0;
	}
}

static inline void
CLAMP(
	D3DXVECTOR3&	c,
	FLOAT		lowerBound = 0.f,
	FLOAT		upperBound = 1.f
)
{
	if (c.x < lowerBound)
		c.x = lowerBound;
	if (c.y < lowerBound)
		c.y = lowerBound;
	if (c.z < lowerBound)
		c.z = lowerBound;

	if (c.x > upperBound)
		c.x = upperBound;
	if (c.y > upperBound)
		c.y = upperBound;
	if (c.z > upperBound)
		c.z = upperBound;
}
void	kMesh::__Set__next_prID(
	DWORD	prID,
	std::vector<DWORD>&	next_prID,
	const DWORD*	pPointReprFlag
)
{
#ifdef __USE_A_OFFSET__
	for (int k = pointer_to__a_vEdgeID_a_offset[prID]; k < pointer_to__a_vEdgeID_a_offset[prID + 1]; ++k) {
		const EDGE	e = pointer_to__a_vEdgeID[k];
		const CEdge&	edge = __edgePtr[e.id];
		DWORD	id = e.bDir ? edge.prEndID : edge.prStartID;
		if (pPointReprFlag[id] == 0) {
			next_prID.push_back(id);
		}
	}
#else
	const CPointRepr&	pointRepr = __pointReprPtr[prID];
	for (size_t i = 0; i < pointRepr.vEdgeID.size(); ++i) {
		EDGE	e = pointRepr.vEdgeID.at(i);
		const CEdge&	edge = __edgePtr[e.id];
		DWORD	id = e.bDir ? edge.prEndID : edge.prStartID;
		if (pPointReprFlag[id] == 0) {
			next_prID.push_back(id);
		}
	}
#endif
}
void	kMesh::setColorIsometric(
	DWORD	prID,
	bool	shiftKeyPressed,
	unsigned	nChannel,
	unsigned	nStep
)
{
	DWORD*	pPointReprFlag = 0;
	kMesh::D3DXVERTEX*	pV;
	__vertexBufferPtr->Lock(0, 0, (void**) &pV, 0);
do {
	pPointReprFlag = new DWORD[__numPointReprs];
	FillMemory(pPointReprFlag, __numPointReprs * sizeof(DWORD), 0);

	std::vector<DWORD>	next_prID;
	next_prID.push_back(prID);
	if (!shiftKeyPressed) {
		for (DWORD i = 0; i < __numPointReprs; ++i)
			pV[i].c = D3DXVECTOR3(0.f, 0.f, 0.f);
	}
	for (unsigned iDepth = 0; iDepth < nChannel * nStep; ++iDepth) {
		std::vector<DWORD>	current_prID(next_prID);
		next_prID.resize(0);
		for (size_t i = 0; i < current_prID.size(); ++i) {
			DWORD	index = current_prID.at(i);
			D3DXVECTOR3	color = D3DXVECTOR3(0.f, 0.f, 0.f);
			assert(nStep > 0);
			switch (nChannel) {
			case 1:
				color = D3DXVECTOR3(1.f - (1.f / (FLOAT) nStep) * (iDepth - 0 * nStep), 0.f, 0.f);
				break;
			case 2:
				if (iDepth < nStep) {
					color = D3DXVECTOR3(1.f, 1.f - (1.f / (FLOAT) nStep) * (iDepth - 0 * nStep), 0.f);
				} else {	// iDepth < 2 * nStep
					color = D3DXVECTOR3(1.f - (1.f / (FLOAT) nStep) * (iDepth - 1 * nStep), 0.f, 0.f);
				}
				break;
			case 3:
				if (iDepth < nStep) {
					color = D3DXVECTOR3(1.f, 1.f, 1.f - (1.f / (FLOAT) nStep) * (iDepth - 0 * nStep));
				} else if (iDepth < 2 * nStep) {
					color = D3DXVECTOR3(1.f, 1.f - (1.f / (FLOAT) nStep) * (iDepth - 1 * nStep), 0.f);
				} else {	// iDepth < 3 * nStep
					color = D3DXVECTOR3(1.f - (1.f / (FLOAT) nStep) * (iDepth - 2 * nStep), 0.f, 0.f);
				}
				break;
			default:
				assert(false);
			}
			if (shiftKeyPressed) {
				pV[index].c += color;
				CLAMP(pV[index].c);
			} else
				pV[index].c = color;

			pPointReprFlag[current_prID.at(i)] = 1;
		}
		for (size_t i = 0; i < current_prID.size(); ++i) {
			__Set__next_prID(current_prID.at(i), next_prID, pPointReprFlag);
		}
	}
} while (0);
	__vertexBufferPtr->Unlock();
	if (pPointReprFlag) {
		delete[] pPointReprFlag;
		pPointReprFlag = 0;
	}
}


void	kMesh::ChangeMesh(
#if 1
	float	fNormalAmount,
	unsigned	nChannel
#else
	bool	plus,
	bool	shiftKeyPressed,
	unsigned	nChannel,
	unsigned	nStep
#endif
)
{
	HRESULT	hResult;
	LPDIRECT3DVERTEXBUFFER9	pVB_n = __showNormal.GetVertexBufferPointer();
	kShowNormal::Vertex*	pV_n;
	pVB_n->Lock(0, 0, (void**) &pV_n, 0);

	kMesh::D3DXVERTEX*	pV;
	__vertexBufferPtr->Lock(0, 0, (void**) &pV, 0);

	FLOAT*	pVB_ = NULL;
	hResult = __meshPtr->LockVertexBuffer(0, (void**) &pVB_);
	assert(D3D_OK == hResult);

	DWORD*	pPointReprFlag = new DWORD[__numPointReprs];
	FillMemory(pPointReprFlag, __numPointReprs * sizeof(DWORD), 0);

	for (DWORD i = 0; i < __numPointReprs; ++i) {
		DWORD	prID = __pointReprPtr[i].id;

		DWORD	index = prID;
		D3DXVECTOR3&	cColor = pV[index].c;
		if (cColor.x == 0.f && cColor.y == 0.f && cColor.z == 0.f)
			continue;

		pPointReprFlag[prID] = 1;
		// index 번째의 (새로운) Vertex 의 위치를 Normal 방향으로 cColor 만큼 변화시킨다.
		// 모든 변화는 Point Repr. ID 를 기준으로 하고,
		// cColor 의 설정도 Point Repr. ID 의 위치를 기준으로 하므로
		// 해당 Point Repr. ID 의 v.p 값도 바꾸어야한다.
/*
		if (iDepth < 5)
			color = D3DXVECTOR3(1.f, 1.f - .2f * iDepth, 0.f);
		else if (iDepth < 10)
			color = D3DXVECTOR3(1.f - .2f * (iDepth - 5), 0.f, 0.f);
		if (g__Shift) {
			pV[index].c += color;
			CLAMP(pV[index].c);
		} else
			pV[index].c = color;
*/
		FLOAT	f = (cColor.x + cColor.y + cColor.z) / (FLOAT) nChannel;
#if 1
		D3DXVECTOR3	normalScale = pV[index].n;
		normalScale *= f * fNormalAmount;
		pV_n[2 * index + 0].Position += normalScale;
		pV_n[2 * index + 1].Position += normalScale;
		pV[index].p += normalScale;
		pVB_[8 * index + 0] += normalScale.x;
		pVB_[8 * index + 1] += normalScale.y;
		pVB_[8 * index + 2] += normalScale.z;
		__pointReprPtr[i].v.position += normalScale;
#else
		// a 를 1% 씩 또는 10%(SHIFT 키를 눌렀을 때) 씩 변화시킨다.
		f *= (shiftKeyPressed ? 0.1f : 0.01f) * __a;
		// pV[index].n 은 D3DXComputeNormals 후의 normal 값이다!
		D3DXVECTOR3	normalScale = pV[index].n;
		normalScale *= f;
		if (plus) {
			pV_n[2 * index + 0].Position += normalScale;
			pV_n[2 * index + 1].Position += normalScale;
			pV[index].p += normalScale;
			pVB_[8 * index + 0] += normalScale.x;
			pVB_[8 * index + 1] += normalScale.y;
			pVB_[8 * index + 2] += normalScale.z;
			__pointReprPtr[i].v.p += normalScale;
		} else {
			pV_n[2 * index + 0].Position -= normalScale;
			pV_n[2 * index + 1].Position -= normalScale;
			pV[index].p -= normalScale;
			pVB_[8 * index + 0] -= normalScale.x;
			pVB_[8 * index + 1] -= normalScale.y;
			pVB_[8 * index + 2] -= normalScale.z;
			__pointReprPtr[i].v.p -= normalScale;
		}
#endif
	}

	__calculateVolumeChange(pV, pPointReprFlag);
	SAFE_DELETE_ARRAY(pPointReprFlag);

	hResult = __meshPtr->UnlockVertexBuffer();
	assert(D3D_OK == hResult);
	__vertexBufferPtr->Unlock();
	pVB_n->Unlock();
}

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
void	kMesh::brushInit()
{
	if (
		__simulation.getExecTypeRef() != SIMULATION_EXEC_UNDEF &&	// 시뮬레이션이 진행 중인 경우
		__simulation.getTypeRef() == SIMULATION_TYPE_MANUAL &&
		__simulation.getManualRef().type == MANUAL_DIRECTLY
	) {
		D3DXVERTEX*	vertexPtr;
		__vertexBufferPtr->Lock(0, 0, (void**) &vertexPtr, 0);
		for (DWORD i = 0; i < __numPointReprs; ++i) {
			D3DXVERTEX&	v = vertexPtr[i];
			v.c = D3DXVECTOR3(0, 0, 0);
		}
		__vertexBufferPtr->Unlock();
	}
}
#if 1
int	collectFacesCallback(
	const kMesh*	meshPtr,
	const CFace&	face,
	const void*	voidPtr,
	
	DWORD*		donePrIDArray
)
{
	const kMesh::COLLECT_CALLBACK_ARGUMENT&	arg = *((kMesh::COLLECT_CALLBACK_ARGUMENT*) voidPtr);
	const CPointRepr*	pointReprArray = meshPtr->GetPointReprPointer();
#if 1	// Face 의 노말
	const D3DXVECTOR3&	normal = face.normal;
#else
	D3DXVECTOR3	normal(0, 0, 0);
	for (int i = 0; i < 3; ++i) {
		const CPointRepr&	pointRepr = pointReprArray[face.prID[i]];
		normal += pointRepr.v.normal;
	}
	D3DXVec3Normalize(&normal, &normal);
#endif
	if (D3DXVec3Dot(&normal, &arg.nDir) < 0)	// 반대방향
		return 0;
#if 1
#else
	float	r2 = arg.brush_radius * arg.brush_radius;
//	cout << r2 << endl;
#endif
	int	n = 0;	// 브러쉬 안에 들어오는 Vertex 갯수
	const D3DVIEWPORT9&	viewport = arg.viewport;
	float	brx = arg.brush_radius / (.5f * viewport.Width);
	float	bry = arg.brush_radius / (.5f * viewport.Height);
	for (int i = 0; i < 3; ++i) {
		DWORD	prID = face.prID[i];
		D3DXVECTOR3	Q = pointReprArray[prID].v.position;
#if 1
		D3DXVECTOR3	p;
		D3DXVec3TransformCoord(&p, &Q, &arg.m);
		float	z = viewport.MinZ + p.z * (viewport.MaxZ - viewport.MinZ);
		if (z < 0.f || z > 1.f) {
			cout << z << endl;		
			continue;
		}
#else
		D3DXVECTOR3	q;
		D3DXVec3Project(&q, &Q, &arg.viewport, &arg.projectionMatrix, &arg.viewMatrix, &arg.worldMatrix);
		if (q.z < 0 || q.z > 1.f)
			continue;
#endif

#if 1
		D3DXVECTOR2	A((p.x - arg._p0.x) / brx, (p.y - arg._p0.y) / bry);
//		cout << "br: [" << arg.brx << ", " << arg.bry << "]" << endl;
		FLOAT	d2 = D3DXVec2LengthSq(&A);
//		cout << d2 << endl;
		if (d2 < 1.f) {
			donePrIDArray[prID] = 1;
			++n;
		}
#else
		D3DXVECTOR2	A(q.x - arg.P02.x, q.y - arg.P02.y);
		FLOAT	d2 = D3DXVec2LengthSq(&A);
		if (d2 < r2) {
			donePrIDArray[prID] = 1;
			++n;
		}
#endif
	}
	return n ? 1 : 0;
}
void
kMesh::CollectFaces(
	const kMesh*	meshPtr,	// [IN]; 메쉬의 Vertex, Face 들의 정보을 참고해서...
	const DWORD*	faceIDArray0,	// [IN]; 이 Face 들 중에서
					//	이 값이 NULL 이면 메쉬에 있는 모든 Face 들을 조사한다.
	const COLLECT_CALLBACK_POINTER	callback_ptr,
	const void*			voidPtr,

	DWORD		startFaceID,	// [IN]; 여기서 시작해서...

	std::vector<DWORD>&	faceIDArray,	// [OUT]; Face 들의 목록
	std::vector<DWORD>&	pointReprIDArray
)
{
	DWORD*	doneFaceIDArray = 0;
	DWORD*	activeFaceIDArray = 0;
	DWORD*	next_activeFaceIDArray = 0;
	DWORD*	activePrIDArray = 0;
	do {
		doneFaceIDArray = new DWORD[meshPtr->GetNumFaces()];
		FillMemory(doneFaceIDArray, meshPtr->GetNumFaces() * sizeof(DWORD), 0);
		activeFaceIDArray = new DWORD[meshPtr->GetNumFaces()];
		FillMemory(activeFaceIDArray, meshPtr->GetNumFaces() * sizeof(DWORD), 0);
		next_activeFaceIDArray = new DWORD[meshPtr->GetNumFaces()];
		activePrIDArray = new DWORD[meshPtr->GetNumPointReprs()];
		FillMemory(activePrIDArray, meshPtr->GetNumPointReprs() * sizeof(DWORD), 0);

		activeFaceIDArray[startFaceID] = 1;
		DWORD	n_active_face_id = 0;
		do {
			FillMemory(next_activeFaceIDArray, meshPtr->GetNumFaces() * sizeof(DWORD), 0);
			for (int n = 0; n < (int) meshPtr->GetNumFaces(); ++n) {
				if (!activeFaceIDArray[n])
					continue;

				__CollectFaces(
					meshPtr,
					faceIDArray0,
					callback_ptr, voidPtr,

					n,

					// 유효한 점들을 얻는 데 사용된 Face 들의 목록이지
					// 유효한 점들에 인접한 Face 들의 목록은 아니다;
//					faceIDArray,

					next_activeFaceIDArray,
					doneFaceIDArray,
					activePrIDArray
				);
			}

			n_active_face_id = 0;
			for (int n = 0; n < (int) meshPtr->GetNumFaces(); ++n) {
				activeFaceIDArray[n] = 0;
				if (next_activeFaceIDArray[n]) {
					activeFaceIDArray[n] = 1;
					++n_active_face_id;
				}
			}
		} while (n_active_face_id);
	} while (0);
	SAFE_DELETE_ARRAY(activeFaceIDArray);
	SAFE_DELETE_ARRAY(next_activeFaceIDArray);
	// faceIDArray, pointReprIDArray 결정
	FillMemory(doneFaceIDArray, meshPtr->GetNumFaces() * sizeof(DWORD), 0);
	for (int prID = 0; prID < (int) meshPtr->GetNumPointReprs(); ++prID) {
		if (activePrIDArray[prID]) {
			pointReprIDArray.push_back(prID);

#ifdef __USE_A_OFFSET__
			for (int k = meshPtr->pointer_to__a_vFaceIDLocalIndex_a_offset[prID]; k < meshPtr->pointer_to__a_vFaceIDLocalIndex_a_offset[prID + 1]; ++k) {
				const FACE_ID_LOCAL_INDEX&	faceIDLocalIndex = meshPtr->pointer_to__a_vFaceIDLocalIndex[k];
#else
			for (size_t k = 0; k < meshPtr->GetPointReprPointer()[prID].vFaceIdLocalIndex.size(); ++k) {
				const FACE_ID_LOCAL_INDEX&	faceIDLocalIndex = meshPtr->GetPointReprPointer()[prID].vFaceIdLocalIndex.at(k);
#endif
				doneFaceIDArray[faceIDLocalIndex.ID] = 1;
#ifdef __USE_A_OFFSET__
			}
#else
			}
#endif
			}
	}
	for (int faceID = 0; faceID < (int) meshPtr->GetNumFaces(); ++faceID) {
		if (doneFaceIDArray[faceID])
			faceIDArray.push_back(faceID);
	}
	SAFE_DELETE_ARRAY(doneFaceIDArray);
	SAFE_DELETE_ARRAY(activePrIDArray);
}

void
kMesh::__CollectFaces(
	const kMesh*	meshPtr,	// [IN]; 메쉬의 Vertex, Face 들의 정보을 참고해서
	const DWORD*	faceIDArray0,	// [IN]; 이 Face 들 중에서
					//	이 값이 NULL 이면 메쉬에 있는 모든 Face 들을 조사한다.
	const COLLECT_CALLBACK_POINTER	callback_ptr,
	const void*			voidPtr,

	DWORD		faceID,	// [IN]; 아래쪽에 있는 이 Face 에서 시작해서

//	std::vector<DWORD>&	faceIDArray,	// [OUT]; Face 들의 목록

	DWORD*	next_activeFaceIDArray,	// [OUT]; 아래쪽에 있는 다음 Face 들의 목록을 구한다.
	DWORD*	doneFaceIDArray,	// [IN/OUT]; Face 들의 목록을 갱신한다.
	DWORD*	donePrIDArray		// [IN/OUT]; Face 들의 목록을 갱신한다.
)
{
	const CFace*		faceArray = meshPtr->GetFacePointer();

	doneFaceIDArray[faceID] = 1;
	const CFace&	face = faceArray[faceID];
	// 1. 이 삼각형을 조사할 필요가 있는 지 알아본다.
	if (faceIDArray0 && faceIDArray0[faceID] == 0)	// 조사할 목록에 조차 있지 않다.
		return;
	// ......
	if (callback_ptr(meshPtr, face, voidPtr, donePrIDArray) <= 0)
		return;
//	faceIDArray.push_back(faceID);
	// 2. 인접한 삼각형들의 목록을 만든다.
	for (int i = 0; i < 3; ++i) {
		DWORD	id = meshPtr->adjacencyPtr()[3 * faceID + i];
		if (id == (DWORD) -1)
			continue;
		if (doneFaceIDArray[id] == 0)
			next_activeFaceIDArray[id] = 1;
	}
}
#endif
#if 1	// flatten
void
kMesh::CollectFacesBelowIron(
	const kMesh*	meshPtr,	// [IN]; 메쉬의 Vertex, Face 들의 정보을 참고해서...
	const DWORD*	faceIDArray0,	// [IN]; 이 Face 들 중에서
					//	이 값이 NULL 이면 메쉬에 있는 모든 Face 들을 조사한다.
	const COLLECT_CALLBACK_POINTER	callback_ptr,
	const void*			voidPtr,
	std::vector<DWORD>&	faceIDArray,	// [OUT]; Face 들의 목록
	std::vector<DWORD>&	pointReprIDArray
)
{

	DWORD*	doneFaceIDArray = 0;
	DWORD*	donePrIDArray = 0;
do {
	doneFaceIDArray = new DWORD[meshPtr->GetNumFaces()];
	FillMemory(doneFaceIDArray, meshPtr->GetNumFaces() * sizeof(DWORD), 0);
	donePrIDArray = new DWORD[meshPtr->GetNumPointReprs()];
	FillMemory(donePrIDArray, meshPtr->GetNumPointReprs() * sizeof(DWORD), 0);

	for (int faceID = 0; faceID < (int) meshPtr->GetNumFaces(); ++faceID) {
		const CFace&	face = meshPtr->GetFacePointer()[faceID];
		if (faceIDArray0 && faceIDArray0[faceID] == 0)	// 조사할 목록에 조차 있지 않다.
			return;
		callback_ptr(meshPtr, face, voidPtr, donePrIDArray);
	}
} while (0);
	// faceIDArray, pointReprIDArray 결정
	for (int prID = 0; prID < (int) meshPtr->GetNumPointReprs(); ++prID) {
		if (donePrIDArray[prID]) {
			pointReprIDArray.push_back(prID);

#ifdef __USE_A_OFFSET__
			for (int k = meshPtr->pointer_to__a_vFaceIDLocalIndex_a_offset[prID]; k < meshPtr->pointer_to__a_vFaceIDLocalIndex_a_offset[prID + 1]; ++k) {
				const FACE_ID_LOCAL_INDEX&	faceIDLocalIndex = meshPtr->pointer_to__a_vFaceIDLocalIndex[k];
#else
			for (size_t k = 0; k < meshPtr->GetPointReprPointer()[prID].vFaceIdLocalIndex.size(); ++k) {
				const FACE_ID_LOCAL_INDEX&	faceIDLocalIndex = meshPtr->GetPointReprPointer()[prID].vFaceIdLocalIndex.at(k);
#endif
				doneFaceIDArray[faceIDLocalIndex.ID] = 1;
#ifdef __USE_A_OFFSET__
			}
#else
			}
#endif
			}
	}
	for (int faceID = 0; faceID < (int) meshPtr->GetNumFaces(); ++faceID) {
		if (doneFaceIDArray[faceID])
			faceIDArray.push_back(faceID);
	}
	SAFE_DELETE_ARRAY(doneFaceIDArray);
	SAFE_DELETE_ARRAY(donePrIDArray);
}
#endif	// flatten
void	kMesh::brush()
{
	POINT	ptCursor;
	GetCursorPos(&ptCursor);
	ScreenToClient(DXUTGetHWND(), &ptCursor);
	kState&	state = *__statePtr;
	FLOAT	brushRadius = state.brush.Radius;
	bool	shiftKeyPressed = state.g__Shift || state.brush.flag.Sub;
	UINT&	g__NSelectedPoints = state.g__NSelectedPoints;	// [OUT]
	DWORD&	g__prID = state.g__prID;			// [OUT]
	DWORD&	g__lastPrID = state.g__lastPrID;		// [OUT]

	HRESULT	hResult;
	const INTERSECTION*	intersection_ptr = __statePtr->intersection_ptr;
if (intersection_ptr) {
	D3DXVECTOR3	P0(0, 0, 0);
	D3DXVECTOR3	nDir(0, 0, 0);
	getPoint(*intersection_ptr, P0);
	getNormal(*intersection_ptr, nDir);
	D3DXVec3Normalize(&nDir, &nDir);

	COLLECT_CALLBACK_ARGUMENT	arg;
	arg.P0 = P0 + (shiftKeyPressed ? 1000.f : -1000.f) * nDir;	// 1000.f: TO DO LIST
	arg.nDir = nDir;
	arg.brush_radius = brushRadius;

	D3DVIEWPORT9&	viewport = arg.viewport;
	__devicePtr->GetViewport(&viewport);
#if 1
#else
	arg.projectionMatrix = __projectionMatrix;
	arg.viewMatrix = __viewMatrix;
	arg.worldMatrix = __worldMatrix;
	arg.P02.x = (float) ptCursor.x;
	arg.P02.y = (float) ptCursor.y;
#endif
	D3DXMATRIX	m1;
	D3DXMatrixMultiply(&m1, &__worldMatrix, &__viewMatrix);
	D3DXMatrixMultiply(&arg.m, &m1, &__projectionMatrix);
	// (0, W - 1) -> (-1, 1)
	// (0, H - 1) -> (1, -1)
	arg._p0.x = 2.f * (float) ptCursor.x / (arg.viewport.Width - 1) - 1.f;
	arg._p0.y = 1.f - 2.f * (float) ptCursor.y / (arg.viewport.Height - 1);
//	cout << "screen space: [" << ptCursor.x << ", " << ptCursor.y << "]" << endl;
//	cout << "projection space: [" << arg._p0.x << ", " << arg._p0.y << "]" << endl;
//	arg.brx = brushRadius / (.5f * viewport.Width);
//	arg.bry = brushRadius / (.5f * viewport.Height);
//	cout << "brushRadius: " << brushRadius << endl;
//	arg.aspect_ratio = (float) viewport.Width / viewport.Height;

	std::vector<DWORD>	faceIDArray;
	std::vector<DWORD>	pointReprIDArray;
	switch (__statePtr->brush.Type) {
	case kBrush::BRUSH_STANDARD:
		kMesh::CollectFaces(
			this, NULL,
			collectFacesCallback, &arg,
			intersection_ptr->dwFace,
			faceIDArray,
			pointReprIDArray
		);
		break;
	case kBrush::BRUSH_FLATTEN:
		arg.nDir = -__statePtr->brush.RayDirection;
		CollectFacesBelowIron(
			this, NULL,
			collectFacesCallback, &arg,
			faceIDArray,
			pointReprIDArray
		);
		break;
	default:
		cerr << "kMesh::brush: Unknown Brush Type = " << __statePtr->brush.Type << endl;
		return;
	}

if (__statePtr->brush.flag.Apply) {
	LPDIRECT3DVERTEXBUFFER9	pVB_n = __showNormal.GetVertexBufferPointer();
	kShowNormal::Vertex*	pV_n;
	pVB_n->Lock(0, 0, (void**) &pV_n, 0);

	kMesh::D3DXVERTEX*	pV;
	__vertexBufferPtr->Lock(0, 0, (void**) &pV, 0);

	FLOAT*	pVB_ = NULL;
	hResult = __meshPtr->LockVertexBuffer(0, (void**) &pVB_);
	assert(D3D_OK == hResult);
	CPointRepr::VERTEX*	vertexArray;	// 텍스쳐 메쉬 정보를 담은 Vertex Buffer
	__meshLoader.meshSubsetPtr()->LockVertexBuffer(0, (void**) &vertexArray);

#if 1	// ZBrush Standard Brush
if (__statePtr->brush.flag.Sculpture || __statePtr->g__Alt) {
	if (__statePtr->brush.Type == kBrush::BRUSH_FLATTEN) {
		// COLLECT_CALLBACK_ARGUMENT 의 P0 을 결정;
		// 각 점들에서,
		// 첫 번째 점을 지나고 COLLECT_CALLBACK_ARGUMENT::nDir 를 법선벡터로 하는 평면에 내린,
		// 길이의 평균만큼 첫번째 점을 움직이면 그 점이 COLLECT_CALLBACK_ARGUMENT::P0 가 된다.
		//	P 에서 (P0, nDir) 에 의해 결정되는 평면에 이르는 수직 (부호있는) 거리
		//	= P0 에서 P 에 이르는 벡터의 nDir 방향 성분
		if (pointReprIDArray.size()) {
			D3DXVECTOR3&	P0 = arg.P0;
			D3DXVECTOR3&	nDir = arg.nDir;
			float	signed_h = 0.f;
			P0 = pV[pointReprIDArray.at(0)].p;
			for (int i = 1; i < (int) pointReprIDArray.size(); ++i) {
				D3DXVECTOR3&	P = pV[pointReprIDArray.at(i)].p;
				D3DXVECTOR3	P0P = P - P0;
				signed_h += D3DXVec3Dot(&nDir, &P0P);
			}
			signed_h /= (float) pointReprIDArray.size();
			P0 += signed_h * nDir;
		}
	}
	for (int i = 0; i < (int) pointReprIDArray.size(); ++i) {
		DWORD	prID = pointReprIDArray.at(i);
		D3DXVECTOR3&	p = pV[prID].p;
		if (__statePtr->brush.Type == kBrush::BRUSH_STANDARD) {
			D3DXVECTOR3	ell = p - arg.P0;
			// nDir 에 수직인 성분과 평행한 성분
			D3DXVECTOR3	ell_longitudinal = D3DXVec3Dot(&ell, &nDir) * nDir;
			D3DXVECTOR3	ell_transversal = ell - ell_longitudinal;
			float	s = kBone::smoothstep(0.f, 1.f, 200.f, 0.f, D3DXVec3Length(&ell_transversal));
					// 200.f: TO DO LIST
			p = arg.P0 + ((1.f + s * __statePtr->brush.Intensity * __a) * ell_longitudinal + ell_transversal);
					// .00001f: TO DO LIST
		} else if (__statePtr->brush.Type == kBrush::BRUSH_FLATTEN) {
			// COLLECT_CALLBACK_ARGUMENT::{P0,nDir} 에 의해 결정되는 평면에 내린 수선의 발
			//	1) P 에서 (P0, nDir) 에 의해 결정되는 평면에 이르는 수직 (부호있는) 거리
			//	= P0 에서 P 에 이르는 벡터의 nDir 방향 성분(P0P.nDir)
			//	2) 수선의 발
			//	P0 + (P0P - (P0P.nDir) nDir)
			D3DXVECTOR3&	P0 = arg.P0;
			D3DXVECTOR3&	nDir = arg.nDir;
			D3DXVECTOR3	P0P = p - P0;
			p = P0 + (P0P - D3DXVec3Dot(&P0P, &nDir) * nDir);
		}
//		cout << s << endl;
#if 1
		// index 번째의 (새로운) Vertex 의 위치를 Normal 방향으로 cColor 만큼 변화시킨다.
		// 모든 변화는 Point Repr. ID 를 기준으로 하고,
		// cColor 의 설정도 Point Repr. ID 의 위치를 기준으로 하므로
		// 해당 Point Repr. ID 의 v.p 값도 바꾸어야한다.
/*
		if (iDepth < 5)
			color = D3DXVECTOR3(1.f, 1.f - .2f * iDepth, 0.f);
		else if (iDepth < 10)
			color = D3DXVECTOR3(1.f - .2f * (iDepth - 5), 0.f, 0.f);
		if (g__Shift) {
			pV[index].c += color;
			CLAMP(pV[index].c);
		} else
			pV[index].c = color;
*/
#if 1
		pV_n[2 * prID + 0].Position = p;
		pV_n[2 * prID + 1].Position = p;
		pVB_[8 * prID + 0] = p.x;
		pVB_[8 * prID + 1] = p.y;
		pVB_[8 * prID + 2] = p.z;
		__pointReprPtr[prID].v.position = p;
#if 1	// 텍스쳐 메쉬 변형
#ifdef __USE_A_OFFSET__
		for (int k = pointer_to__a_vVertexID_a_offset[prID]; k < pointer_to__a_vVertexID_a_offset[prID + 1]; ++k)
			vertexArray[pointer_to__a_vVertexID[k]].position = p;
#else
		const std::set<DWORD>&	vVertexID = pointReprArray[prID].vVertexID;
		for (std::set<DWORD>::const_iterator it = vVertexID.begin(); it != vVertexID.end(); ++it)
			vertexArray[*it].position = p;
#endif
#endif	// 텍스쳐 메쉬 변형
#endif
#endif
	}
}
#endif	// ZBrush Standard Brush

#if 1	// Vertex 위치 변경; ZBrush Smooth Brush
if (__statePtr->brush.flag.Smooth || __statePtr->g__Control) {
	for (int i = 0; i < (int) pointReprIDArray.size(); ++i) {
		DWORD	prID = pointReprIDArray.at(i);

		D3DXVERTEX&	v = pV[prID];
		v.pTmp = v.p;
	}
	for (int i = 0; i < (int) pointReprIDArray.size(); ++i) {
		DWORD	prID = pointReprIDArray.at(i);

		D3DXVERTEX&	v = pV[prID];
/*
 * 각 Edge 를 구성하는 두 Vertex 의 중점이 각 Vertex 의 새로운 위치에 영향을 미친다;
 * Vertex 의 새로운 위치들의 centroid 와 원래 위치의 중점의 실제로 새로운 위치가 된다.
 */
		// 주어진 Vertex 를 한 끝으로 하는 모든 Edge 에서 Edge 의 중점을 구한다.
		// 이 점들의 centroid 와 원래 점의 중점이 새로운 위치가 된다.
		D3DXVECTOR3	centroid(0, 0, 0);
		unsigned	numPoints = 0;
#ifdef __USE_A_OFFSET__
		numPoints = pointer_to__a_vEdgeID_a_offset[prID + 1] - pointer_to__a_vEdgeID_a_offset[prID];
		for (int k = pointer_to__a_vEdgeID_a_offset[prID]; k < pointer_to__a_vEdgeID_a_offset[prID + 1]; ++k) {
			const EDGE	e = pointer_to__a_vEdgeID[k];
			const CEdge&	edge = __edgePtr[e.id];
			centroid += .5f * (pV[edge.prStartID].p + pV[edge.prEndID].p);
		}
#else
		const CPointRepr&	pointRepr = __pointReprPtr[prID];
		numPoints = pointRepr.vEdgeID.size();
		for (size_t i = 0; i < pointRepr.vEdgeID.size(); ++i) {
			EDGE	e = pointRepr.vEdgeID.at(i);
			const CEdge&	edge = __edgePtr[e.id];
			centroid += .5f * (pV[edge.prStartID].p + pV[edge.prEndID].p);
		}
#endif
//		cout << "numPoints = " << numPoints << endl;
		centroid /= (float) numPoints;
		v.pTmp = .5f * (v.p + centroid);
	}
	for (int i = 0; i < (int) pointReprIDArray.size(); ++i) {
		DWORD	prID = pointReprIDArray.at(i);

		D3DXVERTEX&	v = pV[prID];
		v.p = v.pTmp;

		D3DXVECTOR3&	p = v.p;
		pV_n[2 * prID + 0].Position = p;
		pV_n[2 * prID + 1].Position = p;
		pVB_[8 * prID + 0] = p.x;
		pVB_[8 * prID + 1] = p.y;
		pVB_[8 * prID + 2] = p.z;
		__pointReprPtr[prID].v.position = p;
#if 1	// 텍스쳐 메쉬 변형
#ifdef __USE_A_OFFSET__
		for (int k = pointer_to__a_vVertexID_a_offset[prID]; k < pointer_to__a_vVertexID_a_offset[prID + 1]; ++k)
			vertexArray[pointer_to__a_vVertexID[k]].position = p;
#else
		const std::set<DWORD>&	vVertexID = pointReprArray[prID].vVertexID;
		for (std::set<DWORD>::const_iterator it = vVertexID.begin(); it != vVertexID.end(); ++it)
			vertexArray[*it].position = p;
#endif
#endif	// 텍스쳐 메쉬 변형
	}
}
#endif	// Vertex 위치 변경; ZBrush Smooth Brush

#if 1
if ((__statePtr->brush.flag.Smooth || __statePtr->g__Control) || (__statePtr->brush.flag.Sculpture || __statePtr->g__Alt)) {
	// 각 Vertex 의 노말을 다시 계산;
	// Vertex 를 공유하는 Face 들의 노말의 평균이 해당 Vertex 의 노말
	// 1. 각 Face 의 노말을 저장할 임시 공간
#if 1	// 계산된 Face 노말을 사용
	for (int i = 0; i < (int) faceIDArray.size(); ++i) {
		DWORD	faceID = faceIDArray.at(i);
		CFace&	face = __facePtr[faceID];
		D3DXVECTOR3&	p0 = pV[face.prID0].p;
		D3DXVECTOR3&	p1 = pV[face.prID1].p;
		D3DXVECTOR3&	p2 = pV[face.prID2].p;
		D3DXVECTOR3	v01 = p1 - p0;
//		D3DXVec3Normalize(&v01, &v01);
		D3DXVECTOR3	v02 = p2 - p0;
//		D3DXVec3Normalize(&v02, &v02);
		D3DXVECTOR3&	normal = face.normal;
		D3DXVec3Cross(&normal, &v01, &v02);
		D3DXVec3Normalize(&normal, &normal);
	}
#else
	D3DXVECTOR3*	faceNormalArray = new D3DXVECTOR3[faceIDArray.size()];
	DWORD*	faceIndex = new DWORD[__numFaces];
	FillMemory(faceIndex, __numFaces * sizeof(DWORD), 0xff);	// (DWORD) -1 로 초기화;
	// (DWORD) -1 가 아닌 경우, faceNormalArray 에 대한 인덱스;
	// faceID 에 해당하는 Face 의 새로운 노말 정보는 faceNormalArray[faceIndex[faceID]] 에 들어있다.
	int	index = 0;
	for (int i = 0; i < (int) faceIDArray.size(); ++i, ++index) {
		DWORD	faceID = faceIDArray.at(i);
		faceIndex[faceID] = index;

		const CFace&	face = __facePtr[faceID];
		D3DXVECTOR3&	p0 = pV[face.prID0].p;
		D3DXVECTOR3&	p1 = pV[face.prID1].p;
		D3DXVECTOR3&	p2 = pV[face.prID2].p;
		D3DXVECTOR3	v01 = p1 - p0;
//		D3DXVec3Normalize(&v01, &v01);
		D3DXVECTOR3	v02 = p2 - p0;
//		D3DXVec3Normalize(&v02, &v02);
		D3DXVECTOR3&	normal = faceNormalArray[index];
		D3DXVec3Cross(&normal, &v01, &v02);
		D3DXVec3Normalize(&normal, &normal);
	}
#endif
	// 2. 각 Vertex 의 노말을 공유하는 Face 들의 노말로부터 계산
	for (int i = 0; i < (int) pointReprIDArray.size(); ++i) {
		DWORD	prID = pointReprIDArray.at(i);
		D3DXVECTOR3&	normal = pV[prID].n;
		unsigned	numFaces = 0;
#ifdef __USE_A_OFFSET__
		numFaces = pointer_to__a_vFaceIDLocalIndex_a_offset[prID + 1] - pointer_to__a_vFaceIDLocalIndex_a_offset[prID];
#else
		numFaces = __pointReprPtr[prID].vFaceIdLocalIndex.size();
#endif
#ifdef __USE_A_OFFSET__
		for (int k = pointer_to__a_vFaceIDLocalIndex_a_offset[prID]; k < pointer_to__a_vFaceIDLocalIndex_a_offset[prID + 1]; ++k) {
			const FACE_ID_LOCAL_INDEX&	faceIDLocalIndex = pointer_to__a_vFaceIDLocalIndex[k];
#else
		for (size_t k = 0; k < __pointReprPtr[prID].vFaceIdLocalIndex.size(); ++k) {
			const FACE_ID_LOCAL_INDEX&	faceIDLocalIndex = __pointReprPtr[prID].vFaceIdLocalIndex.at(k);
#endif
#if 1	// Face 노말
			normal += __facePtr[faceIDLocalIndex.ID].normal;
#else
			int	index = faceIndex[faceIDLocalIndex.ID];
			if (index == (DWORD) -1) {
				cerr << "face ID = " << faceIDLocalIndex.ID << endl;
				continue;
			}
//			assert(index != (DWORD) -1);
			normal += faceNormalArray[index];
#endif
#ifdef __USE_A_OFFSET__
		}
#else
		}
#endif
		normal /= (float) numFaces;
		D3DXVec3Normalize(&normal, &normal);
		// __meshPtr 에 사용되는 노말도 업데이트
	/*
		pVertices[i].n.x = pVB[8 * i + 3];
		pVertices[i].n.y = pVB[8 * i + 4];
		pVertices[i].n.z = pVB[8 * i + 5];
	*/
		pVB_[8 * prID + 3] = normal.x;
		pVB_[8 * prID + 4] = normal.y;
		pVB_[8 * prID + 5] = normal.z;
	}
#if 1	// Face 노말
#else
	SAFE_DELETE_ARRAY(faceIndex);
	SAFE_DELETE_ARRAY(faceNormalArray);
#endif
}
#endif

	__meshLoader.meshSubsetPtr()->UnlockVertexBuffer();
	hResult = __meshPtr->UnlockVertexBuffer();
	assert(D3D_OK == hResult);
	__vertexBufferPtr->Unlock();
	pVB_n->Unlock();

#if 1	// 마지막 주문; 10. 성형 후 등고선 재설정
	setIsometricLines(ISOMETRIC_XY, D3DXVECTOR3(0, 0, 1), MAX_SLICE / 2, __z_isometricVertexBufferPtr, __z_isometricLines);
	setIsometricLines(ISOMETRIC_YZ, D3DXVECTOR3(1, 0, 0), MAX_SLICE / 2, __x_isometricVertexBufferPtr, __x_isometricLines);
	setIsometricLines(ISOMETRIC_ZX, D3DXVECTOR3(0, 1, 0), MAX_SLICE / 2, __y_isometricVertexBufferPtr, __y_isometricLines);
#endif	// 마지막 주문; 10. 성형 후 등고선 재설정
}
	if (__statePtr->brush.flag.Show) {
		__statePtr->debugging_FaceID_array = faceIDArray;
		__statePtr->debugging_prReprID_array = pointReprIDArray;
	}

	return;
}

	D3DXVECTOR3*	pSelectV = NULL;
	LPDIRECT3DVERTEXBUFFER9	pSelectVB = NULL;
	hResult = __select.ResetVertexBuffer(kSelect::NUMBER_OF_POINTS__UNIT, &pSelectVB);
	assert(D3D_OK == hResult);
	assert(pSelectVB);
	hResult = pSelectVB->Lock(0, 0, (void**) &pSelectV, 0);
	assert(D3D_OK == hResult);
	assert(pSelectV);

	UINT	k = 0;
	FLOAT	zNearest = FLT_MAX;
	D3DVIEWPORT9	viewport;
	__devicePtr->GetViewport(&viewport);
#if 1
	kMesh::D3DXVERTEX*	pV;
	__vertexBufferPtr->Lock(0, 0, (void**) &pV, 0);

	D3DXVECTOR3	o = D3DXVECTOR3((FLOAT) ptCursor.x, (FLOAT) ptCursor.y, 0.f);
	D3DXVECTOR3	O;	// Object 공간에서의 좌표
	D3DXVec3Unproject(&O, &o, &viewport, &__projectionMatrix, &__viewMatrix, &__worldMatrix);
#endif
#if 1
	DWORD*	pFaceFlag = new DWORD[__numFaces];
	memset(pFaceFlag, 0, __numFaces * sizeof(DWORD));
	BRUSHPOINT*	pBrushPointFlag = new BRUSHPOINT[__numPointReprs];
	memset(pBrushPointFlag, 0, __numPointReprs * sizeof(BRUSHPOINT));
//#pragma omp parallel for
	for (int iFace = 0; iFace < (int) __numFaces; ++iFace) {
		const CFace&	face = __facePtr[iFace];
		// 1. 삼각형의 normal 이 카메라를 보고 있는가?
		// 2. 삼각형을 이루는 각 꼭지점을 이전에 다룬 적이 있는가?
		// 없다면 원 안에 들어오는 꼭지점이 있는가?
		// 3. 카메라에서 꼭지점에 이르는 반직선이
		// 충돌하는 삼각형이 있는가?
		// (1., 2., 3. 모두를 만족하는 꼭지점에 대해서,
		// 그러한 삼각형들만을 고려해서...)

		// 1. 삼각형의 normal 이 카메라를 보고 있는가?
		D3DXVECTOR3	_0 = __pointReprPtr[face.prID0].v.position;
		D3DXVECTOR3	_1 = __pointReprPtr[face.prID1].v.position;
		D3DXVECTOR3	_2 = __pointReprPtr[face.prID2].v.position;
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
			const D3DXVECTOR3&	Q = __pointReprPtr[prID].v.position;
			D3DXVECTOR3	q;
			D3DXVec3Project(&q, &Q, &viewport, &__projectionMatrix, &__viewMatrix, &__worldMatrix);
			if (q.z < 0 || q.z > 1.f)
				continue;

			D3DXVECTOR2	A((q.x - ptCursor.x) / brushRadius, (q.y - ptCursor.y) / brushRadius);// * aspectRatio);
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
//#pragma omp parallel for
	for (int iFace = 0; iFace < (int) __numFaces; ++iFace) {
		// 1. 고려할 삼각형을 이루는 꼭지점 중에서
		// 2. 원 안에 있는 꼭지점 중에서
		// 다른 고려할 삼각형과 충돌하는 지 알아보자.
		if (pFaceFlag[iFace] == 0)
			continue;
		// 1. 고려할 삼각형안에 있다.
		for (int i = 0; i < 3; ++i) {
			DWORD	prID = __facePtr[iFace].prID[i];
			if (pBrushPointFlag[prID].flag != 2)
				continue;

			// 충돌 테스트까지 끝날 것이다.
			pBrushPointFlag[prID].flag = 3;
			// 2. 원 안에 있다.
			bool	bCollide = false;
			const CPointRepr&	pointRepr = __pointReprPtr[prID];
			const D3DXVECTOR3&	Q = pointRepr.v.position;
			for (int kFace = 0; kFace < (int) __numFaces; ++kFace) {
				if (kFace == iFace)	// 자기 자신은 제외
					continue;
				if (pFaceFlag[kFace] == 0)	// 고려하지않을 삼각형은 제외
					continue;

				FLOAT	fBary1, fBary2;
				FLOAT	fDist;
				const CFace&	face = __facePtr[kFace];
				D3DXVECTOR3	r = Q - O;
				D3DXVECTOR3	rn;	// normalized r
				D3DXVec3Normalize(&rn, &r);
				if (IntersectTriangle(
					O, rn,

					__pointReprPtr[face.prID0].v.position,
					__pointReprPtr[face.prID1].v.position,
					__pointReprPtr[face.prID2].v.position,

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
			if (
				__simulation.getExecTypeRef() != SIMULATION_EXEC_UNDEF &&	// 시뮬레이션이 진행 중인 경우
				__simulation.getTypeRef() == SIMULATION_TYPE_MANUAL &&
				__simulation.getManualRef().type == MANUAL_DIRECTLY
			) {
				// "임의조작 > 직접" 성형인 경우에는 무조건 색칠한다;
				// 늘리고 줄일지는 아래에서 결정한다.
				pV[prID].c += (1.f - brushPoint.d2) * D3DXVECTOR3(0.5f, 0.f, 0.f);
			} else {
#if 1	// smoothstep
				if (shiftKeyPressed)
					pV[prID].c -= kBone::smoothstep(0.f, 1.f, 1.f, 0.f, sqrtf(brushPoint.d2)) * D3DXVECTOR3(0.f, 0.f, .5f);
				else
					pV[prID].c += kBone::smoothstep(0.f, 1.f, 1.f, 0.f, sqrtf(brushPoint.d2)) * D3DXVECTOR3(0.5f, 0.f, 0.f);
#else	// smoothstep/!smoothstep
				if (shiftKeyPressed)
					pV[prID].c -= (1.f - brushPoint.d2) * D3DXVECTOR3(0.f, 0.f, .5f);
				else
					pV[prID].c += (1.f - brushPoint.d2) * D3DXVECTOR3(0.5f, 0.f, 0.f);
#endif	// !smoothstep
			}
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
	for (DWORD i = 0; i < meshRef.GetNumPointReprs(); ++i) {
		DWORD	prID = __pointReprPtr[i].id;

		const D3DXVECTOR3&	Q = __pointReprPtr[i].v.p;
		D3DXVECTOR3	q;
		D3DXVec3Project(&q, &Q, &viewport, &mProj, &mView, &mWorld);
		if (q.z < 0 || q.z > 1.f)
			continue;

		D3DXVECTOR2	A((q.x - ptCursor.x) / state.brush.Radius, (q.y - ptCursor.y) / state.brush.Radius * aspectRatio);
		FLOAT	d2 = D3DXVec2LengthSq(&A);
		if (d2 > 1.f)
			continue;

#if 1
		// O 에서 시작에서 OQ 방향으로 나가는 ray 가 충돌하는 삼각형이 있는가?
		D3DXVECTOR3	R = Q - O;
		if (D3DXVec3Dot(&R, &__pointReprPtr[i].v.n) > 0.f)
			continue;
		BOOL	bHit;
		DWORD	faceID;
		D3DXIntersect(meshRef.GetMeshPointer(), &O, &R, &bHit, &faceID, NULL, NULL, NULL, NULL, NULL);
		if (bHit) {
			const CFace&	face = (meshRef.GetFacePointer())[faceID];
			if (face.prID0 != prID && prID != face.prID1 && prID != face.prID2)
				continue;
		}
#endif

#if 1
		DWORD	index = prID;
#else
		DWORD	index = (meshRef.GetPointReprIDMappedPointer())[prID];
#endif
		if (state.g__Shift)
			pV[index].c -= (1.f - d2) * D3DXVECTOR3(0.f, 0.f, .5f);
		else
			pV[index].c += (1.f - d2) * D3DXVECTOR3(0.5f, 0.f, 0.f);
		CLAMP2(pV[index].c);

		if (k >= kSelect::NUMBER_OF_POINTS__UNIT)
			continue;
		if (q.z < zNearest) {
			state.g__prID = prID;
			zNearest = q.z;
		}
		pSelectV[k++] = Q;
	}
#endif
	__vertexBufferPtr->Unlock();
	hResult = pSelectVB->Unlock();
	assert(D3D_OK == hResult);
	g__NSelectedPoints = k;
//	k = 0;
	if (k == 0) {
		g__prID = (DWORD) -1;
		g__lastPrID = (DWORD) -1;
	}
	__select.SetNumberOfPoints(k);

	// 임의조작(SIMULATION_TYPE_MANUAL)의 경우,
	// 색칠하며 직접 조작(MANUAL_DIRECTLY)하는 경우에는 여기서 부피를 변화시킨다.
	if (
		k &&	// 선택된 점이 있는 경우에 한해서;
			// 그렇지 않으면 __fVolumeChange 가 0으로 초기화된다.
		__simulation.getExecTypeRef() != SIMULATION_EXEC_UNDEF &&	// 시뮬레이션이 진행 중인 경우
		__simulation.getTypeRef() == SIMULATION_TYPE_MANUAL &&
		__simulation.getManualRef().type == MANUAL_DIRECTLY
	) {
		float	normal_amount_step = __simulation.getNormalAmountStepRef();
		// TO DO LIST: 여기서 변화된 부분에 대해서만 부피변화를 주어야 한다.
		ChangeMesh(!__statePtr->g__Shift ? normal_amount_step : -normal_amount_step, 3);
//		SIMULATION_CC_TYPE	ccType = SIMULATION_CC_TYPE_VOLUME_CHANGE;
//		인 경우, kMesh::__fVolumeChange 와 시뮬레이션 전에 저장된 값 kSimulation::__fVolumeChange0 의 차이로
//		kSimulation::__volume_change_cc 를 설정한다.
(void) fprintf(stdout, "f = %f, delta(V) = %f, delta(V0) = %f\n", normal_amount_step, __fVolumeChange, __simulation.getVolumeChange0Ref());
		__simulation.getVolumeChangeCcRef() = __fVolumeChange - __simulation.getVolumeChange0Ref();
	}

//	ReleaseCapture();
}
bool	kMesh::IntersectTriangle(
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

#if 1	// kSelect
#else	// kSelect
void	kMesh::selection(
	int	g__iMouseX,
	int	g__iMouseY,
	bool	g__bDrag,
	bool	shiftKeyPressed,
	int	g__iDragBeginX,
	int	g__iDragBeginY,

	UINT&	g__NSelectedPoints,	// [OUT]
	DWORD&	g__prID			// [OUT]
)
{
	HRESULT	hResult;

	D3DXVECTOR3*	pSelectV = NULL;
	LPDIRECT3DVERTEXBUFFER9	pSelectVB = NULL;
	hResult = __select.ResetVertexBuffer(kSelect::NUMBER_OF_POINTS__UNIT, &pSelectVB);
	assert(D3D_OK == hResult);
	assert(pSelectVB);
	hResult = pSelectVB->Lock(0, 0, (void**) &pSelectV, 0);
	assert(D3D_OK == hResult);
	assert(pSelectV);
	std::set<DWORD>&	selectedPrIDSet = __select.selectedPrIDSet();
	if (!shiftKeyPressed)
		selectedPrIDSet.clear();

	FLOAT	zNearest = FLT_MAX;
	D3DVIEWPORT9	viewport;
	__devicePtr->GetViewport(&viewport);
	for (DWORD i = 0; i < __numPointReprs; ++i) {
		DWORD	prID = __pointReprPtr[i].id;
		const D3DXVECTOR3&	Q = __pointReprPtr[i].v.position;
#if 1
		D3DXVECTOR3	q;
		D3DXVec3Project(&q, &Q, &viewport, &__projectionMatrix, &__viewMatrix, &__worldMatrix);
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

		if (selectedPrIDSet.size() >= kSelect::NUMBER_OF_POINTS__UNIT)
			continue;
		if (q.z < zNearest) {
			g__prID = prID;
			zNearest = q.z;
		}
//		pSelectV[k++] = Q;
		if (!shiftKeyPressed)
			selectedPrIDSet.insert(prID);
		else {
			std::set<DWORD>::iterator	it = selectedPrIDSet.find(prID);
			if (it != selectedPrIDSet.end())
				selectedPrIDSet.erase(it);
			else
				selectedPrIDSet.insert(prID);
		}
	}
	hResult = pSelectVB->Unlock();
	assert(D3D_OK == hResult);
	UINT	k = 0;
	for (std::set<DWORD>::const_iterator it = selectedPrIDSet.begin(); it != selectedPrIDSet.end(); ++it) {
		pSelectV[k++] = __pointReprPtr[*it].v.position;
	}
//	cout << "k = " << k << endl;
	g__NSelectedPoints = k;
	if (k == 0) {
		g__prID = (DWORD) -1;
	}
	__select.SetNumberOfPoints(k);
}
#endif	// !kSelect

// 스크린 좌표계의 좌표 p2 가 마우스가 위치한 곳에 있는 지...
bool	kMesh::isPointPicked(const D3DXVECTOR3& p2, const POINT& ptCursor, int delta)
{
	const D3DXVECTOR3&	q = p2;
	if (q.z < 0 || q.z > 1.f)
		return false;

//	FLOAT	zNearest = FLT_MAX;
	int	iMouseX = ptCursor.x;
	int	iMouseY = ptCursor.y;

	int	high, low;
#if 1
	float	f = floor(q.x);
	int	qx = (int) f;
#else
	FLOAT	f = ceilf(q.x);
	int	qx = q.x < f + .5f ? (int) f : (int) (f + 1.f);
#endif
	high = iMouseX + delta;
	low = iMouseX - delta;
	if (qx > high || qx < low)	// [low, high]:= high - low + 1 = 2 * delta + 1
		return false;

#if 1
	f = ceil(q.y);
	int	qy = (int) f;
#else
	f = ceilf(q.y);
	int	qy = q.y < f + .5f ? (int) f : (int) (f + 1.f);
#endif
	high = iMouseY + delta;
	low = iMouseY - delta;
	if (qy > high || qy < low)
		return false;

	return true;
}

HRESULT	kMesh::Pick(
	const LPD3DXMESH	meshPtr,

	bool	g_bUseD3DXIntersect,
	bool	g_bAllHits,
	const D3DXVECTOR3&	vPickRayOrig,
	const D3DXVECTOR3&	vPickRayDir,

	int&		nearestIntersectionIndex,
	DWORD&		dwNumIntersections,
	INTERSECTION*	intersectionArray
)
{
	nearestIntersectionIndex = -1;
	dwNumIntersections = 0;

	HRESULT	hr;
	LPDIRECT3DVERTEXBUFFER9	pVB;
	LPDIRECT3DINDEXBUFFER9	pIB;

	meshPtr->GetVertexBuffer(&pVB);
	meshPtr->GetIndexBuffer(&pIB);
	D3DVERTEX*	pVertices;
#if 1
	bool	b32BitIndex = meshPtr->GetOptions() & D3DXMESH_32BIT;
	BYTE*	pIndices;
#else
	WORD*	pIndices;
#endif
        pIB->Lock(0, 0, (void**) &pIndices, 0);
        pVB->Lock(0, 0, (void**) &pVertices, 0);

#if 1	// 임의의 위치에서의 길이 측정
	FLOAT	nearestDistance = FLT_MAX;
#endif
	if (g_bUseD3DXIntersect) {
		// When calling D3DXIntersect, one can get just the closest intersection and not
		// need to work with a D3DXBUFFER.  Or, to get all intersections between the ray and
		// the mesh, one can use a D3DXBUFFER to receive all intersections.  We show both
		// methods.
		if (!g_bAllHits) {
			// Collect only the closest intersection
			BOOL	bHit;
			DWORD	dwFace;
			FLOAT	fBary1, fBary2, fDist;
			D3DXIntersect(meshPtr, &vPickRayOrig, &vPickRayDir, &bHit, &dwFace, &fBary1, &fBary2, &fDist, NULL, NULL);
			if (bHit) {
				dwNumIntersections = 1;
				intersectionArray[0].dwFace = dwFace;
				intersectionArray[0].fBary1 = fBary1;
				intersectionArray[0].fBary2 = fBary2;
				intersectionArray[0].fDist = fDist;
#if 1	// 임의의 위치에서의 길이 측정
				nearestIntersectionIndex = 0;
#endif
			} else {
				dwNumIntersections = 0;
			}
		} else {
			// Collect all intersections
			BOOL	bHit;
			LPD3DXBUFFER	pBuffer = NULL;
			D3DXINTERSECTINFO*	pIntersectInfoArray;
			if (FAILED(hr = D3DXIntersect(meshPtr, &vPickRayOrig, &vPickRayDir, &bHit, NULL, NULL, NULL, NULL, &pBuffer, &dwNumIntersections))) {
				pVB->Unlock();
				pIB->Unlock();
				SAFE_RELEASE(pVB);
				SAFE_RELEASE(pIB);
				return hr;
			}
			if (dwNumIntersections > 0) {
				pIntersectInfoArray = (D3DXINTERSECTINFO*) pBuffer->GetBufferPointer();
				if (dwNumIntersections > MAX_INTERSECTIONS)
					dwNumIntersections = MAX_INTERSECTIONS;
				for (DWORD iIntersection = 0; iIntersection < dwNumIntersections; ++iIntersection) {
					intersectionArray[iIntersection].dwFace = pIntersectInfoArray[iIntersection].FaceIndex;
					intersectionArray[iIntersection].fBary1 = pIntersectInfoArray[iIntersection].U;
					intersectionArray[iIntersection].fBary2 = pIntersectInfoArray[iIntersection].V;
					intersectionArray[iIntersection].fDist = pIntersectInfoArray[iIntersection].Dist;
#if 1	// 임의의 위치에서의 길이 측정
					if (pIntersectInfoArray[iIntersection].Dist < nearestDistance) {
						nearestDistance = pIntersectInfoArray[iIntersection].Dist;
						nearestIntersectionIndex = iIntersection;
					}
#endif
			    }
			}
			SAFE_RELEASE(pBuffer);
		}
	} else {	// Not using D3DX
		FLOAT	fBary1, fBary2;
		FLOAT	fDist;
		for (DWORD i = 0; i < meshPtr->GetNumFaces(); ++i) {
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
#else
			D3DXVECTOR3	v0 = pVertices[pIndices[3 * i + 0]].p;
			D3DXVECTOR3	v1 = pVertices[pIndices[3 * i + 1]].p;
			D3DXVECTOR3	v2 = pVertices[pIndices[3 * i + 2]].p;
#endif

			// Check if the Pick ray passes through this point
			if (!IntersectTriangle(vPickRayOrig, vPickRayDir, v0, v1, v2, &fDist, &fBary1, &fBary2))
				continue;

			if (g_bAllHits || dwNumIntersections == 0 || fDist < intersectionArray[0].fDist) {
				if (!g_bAllHits)
					dwNumIntersections = 0;
				intersectionArray[dwNumIntersections].dwFace = i;
				intersectionArray[dwNumIntersections].fBary1 = fBary1;
				intersectionArray[dwNumIntersections].fBary2 = fBary2;
				intersectionArray[dwNumIntersections].fDist = fDist;
#if 1	// 임의의 위치에서의 길이 측정
				if (fDist < nearestDistance) {
					nearestDistance = fDist;
					nearestIntersectionIndex = dwNumIntersections;
				}
#endif
				++dwNumIntersections;
				if (dwNumIntersections == MAX_INTERSECTIONS)
					break;
			}
		}
	}

	pVB->Unlock();
	pIB->Unlock();
	SAFE_RELEASE(pVB);
	SAFE_RELEASE(pIB);

	return S_OK;
}

void	kMesh::SetSegmentBuffer(
	LPDIRECT3DVERTEXBUFFER9&	segmentVertexBuffer,
	LPDIRECT3DINDEXBUFFER9&		segmentIndexBuffer,
#if 1	// SEGMENT_VERTEX
	const CGrowableArray<SEGMENT_VERTEX>&	pointArray,
#else
	const std::vector<D3DXVECTOR3>&	pointArray,
#endif
	bool	closed
)
{
	// Vertex Buffer 설정
#if 1	// SEGMENT_VERTEX
	size_t	numPoints = pointArray.GetSize();
#else
	size_t	numPoints = pointArray.size();
#endif
	if (numPoints < 2)
		return;

	if (segmentVertexBuffer) {
		segmentVertexBuffer->Release();
		segmentVertexBuffer = 0;
	}
	__devicePtr->CreateVertexBuffer(
#if 1	// SEGMENT_VERTEX
		(UINT) numPoints * sizeof(SEGMENT_VERTEX),
#else
		numPoints * sizeof(D3DXVECTOR3),
#endif
		D3DUSAGE_WRITEONLY,
		0,
		D3DPOOL_MANAGED,
		&segmentVertexBuffer,
		NULL
	);
#if 1	// SEGMENT_VERTEX
	SEGMENT_VERTEX*	vertexArrayPtr;
#else
	D3DXVECTOR3*	vertexArrayPtr;
#endif
	segmentVertexBuffer->Lock(0, 0, (void**) &vertexArrayPtr, 0);
#if 1	// SEGMENT_VERTEX
	CopyMemory(vertexArrayPtr, pointArray.GetData(), numPoints * sizeof(SEGMENT_VERTEX));
#else
	for (size_t n = 0; n < numPoints; ++n)
		vertexArrayPtr[n] = pointArray.at(n);
#endif
	segmentVertexBuffer->Unlock();


	// Index Buffer 설정
	if (segmentIndexBuffer) {
		segmentIndexBuffer->Release();
		segmentIndexBuffer = 0;
	}
#if 1	// SEGMENT_VERTEX
	int	last_index = pointArray.GetSize() - 1;
#else
	int	last_index = pointArray.size() - 1;
#endif
	__devicePtr->CreateIndexBuffer(
		(closed ? last_index + 1 : last_index) * sizeof(CEdge::LineIndex),
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX32,
		D3DPOOL_MANAGED,
		&segmentIndexBuffer,
		NULL
	);
	CEdge::LineIndex*	indexArrayPtr;
	segmentIndexBuffer->Lock(0, 0, (void**) &indexArrayPtr, 0);
	for (int n = 0; n < last_index; ++n) {
		indexArrayPtr[n].iStart = n;
		indexArrayPtr[n].iEnd = n + 1;
	}
	if (closed) {
		indexArrayPtr[last_index].iStart = last_index;
		indexArrayPtr[last_index].iEnd = 0;
	}
	segmentIndexBuffer->Unlock();
}

void	kMesh::ShowSegment() const
{
	// 곡면을 따라 표현되는 둘레
	if (__segmentMethod == TAPE_MEASURE_METHOD_TWO_POINT) {	// 임의의 두 점 사이의 거리
#if 1	// SEGMENT_VERTEX
		if (__tapeMeasure.twoPointArray().GetSize())
			ShowString(
				__tapeMeasure.twoPointVertexBufferPtr(), __tapeMeasure.twoPointIndexBufferPtr(),
				__tapeMeasure.twoPointArray().GetSize() - 1, D3DXVECTOR4(0, 1, 1, 1)
			);
#else
		if (__tapeMeasure.twoPointArray().size())
			ShowString(
				__tapeMeasure.twoPointVertexBufferPtr(), __tapeMeasure.twoPointIndexBufferPtr(),
				__tapeMeasure.twoPointArray().size() - 1, D3DXVECTOR4(0, 1, 1, 1)
			);
#endif
		else	// 임의의 두 점 사이를 잇는 선을 찾지 못했을 때;
			// 찾으려고 시도한 선을 보여준다.
			ShowString(__segmentVertexBuffer, __segmentIndexBuffer, __numSegments, D3DXVECTOR4(1, 1, 0, 1));
	} else
		ShowString(__segmentVertexBuffer, __segmentIndexBuffer, __numSegments, D3DXVECTOR4(1, 0, 1, .5f));

	// 줄 자를 사용한 둘레
#if 1	// SEGMENT_VERTEX
	if (__statePtr->g__mode == TAPE_MEASURE_MODE_WRAP && __tapeMeasure.convexPointArray().GetSize()) {
		const CGrowableArray<SEGMENT_VERTEX>&	convexPointArray = __tapeMeasure.convexPointArray();
#else
	if (__statePtr->g__mode == TAPE_MEASURE_MODE_WRAP && __tapeMeasure.convexPointArray().size()) {
		const std::vector<D3DXVECTOR3>&	convexPointArray = __tapeMeasure.convexPointArray();
#endif
		ShowVertex(
			__tapeMeasure.convexVertexBufferPtr(),
			OnePointDeclarationPtr(),
#if 1	// SEGMENT_VERTEX
			sizeof(SEGMENT_VERTEX),	// stride
			__statePtr->g__show_all_convex_points ? convexPointArray.GetSize() : 1 + __statePtr->g__next_convex_index,
#else
			sizeof(D3DXVECTOR3),	// stride
			__statePtr->g__show_all_convex_points ? convexPointArray.size() : 1 + __statePtr->g__next_convex_index,
#endif
			5.f,
			D3DXVECTOR4(1, 0, 0, 1)
		);
		ShowString(
			__tapeMeasure.convexVertexBufferPtr(),
			__tapeMeasure.convexIndexBufferPtr(),
#if 1	// SEGMENT_VERTEX
			__segmentMethod == TAPE_MEASURE_METHOD_TWO_POINT ? convexPointArray.GetSize() - 1 : convexPointArray.GetSize(),
#else
			__segmentMethod == TAPE_MEASURE_METHOD_TWO_POINT ? convexPointArray.size() - 1 : convexPointArray.size(),
#endif
			D3DXVECTOR4(0, 1, 0, 1)
		);
#if 1	// SEGMENT_VERTEX
	}
#else
	}
#endif
}

#if 1	// 2D 와의 통신에 사용되는 함수의 구현
int	kMesh::getValue(
	TYPE_XXX	type,
	int		subType,
	void*		dataPtr
)
{
	switch (type) {
#if 1	// manipulator
	case TYPE__MANIPULATOR:
		if ((MANIPULATOR__SUBTYPE) subType == MANIPULATOR__TYPE)
			*((MANIPULATOR_TYPE_T*) dataPtr) = __manipulatorType;
		else {
			if (__manipulatorPtr)
				return __manipulatorPtr->getValue(subType, dataPtr);
		}
		break;
#endif
	case TYPE__TAPE_MEASURE:
#if 1
		if ((TAPE_MEASURE__SUBTYPE) subType == TAPE_MEASURE__VALUE) {
			TAPE_MEASURE_TYPE_VALUE&	typeValue = *((TAPE_MEASURE_TYPE_VALUE*) dataPtr);
			return setMeasure(typeValue.type, &typeValue.value);
		} else
			return __tapeMeasure.getValue(subType, dataPtr);
#else
		return setMeasure((TAPE_MEASURE__TYPE) subType, (float*) dataPtr);
#endif
	case TYPE__WEIGHT_MEASURE:
		return getWeight((WEIGHT_MEASURE__TYPE) subType, (float*) dataPtr);
	case TYPE__AREA:
		return __area.getValue(subType, dataPtr);
	case TYPE__ISOMETRIC:
		return __isometric.getValue(subType, dataPtr);
	case TYPE__MODEL:
		return __model.getValue(subType, dataPtr);
	case TYPE__SIMULATION:
		if ((SIMULATION__SUBTYPE) subType == SIMULATION__STANDARD) {
			// 여기서 가능한 총 변화량을 계산한다.
			__standard_total_volume_change(__simulation.getStandardRef().volume);
		}
		return __simulation.getValue(subType, dataPtr);
	case TYPE__BODY_ELEMENT:
		return __bodyElement.getValue(subType, dataPtr);
		break;
	default:
		(void) fprintf(stderr, "TYPE_XXX:= %d: Not implemented yet\n", type);
		break;
	}
	return -1;
}

int	kMesh::setValue(
	TYPE_XXX	type,
	int		subType,
	const void*	dataPtr
)
{
	switch (type) {
#if 1	// manipulator
	case TYPE__MANIPULATOR:
		switch ((MANIPULATOR__SUBTYPE) subType) {
		case MANIPULATOR__TYPE:
			// line manipulator 에서 곡선에 변화를 주었다면 더 이상 다른 manipulator 는 사용할 수 없다.
			if (
				UNDEF_MANIPULATOR != *((MANIPULATOR_TYPE_T*) dataPtr) &&
				LINE_MANIPULATOR != *((MANIPULATOR_TYPE_T*) dataPtr) &&
				__manipulatorPtr &&
				__manipulatorType == LINE_MANIPULATOR &&
				__line_manipulator.isReleased()
			) {
				(void) fprintf(stderr, "line manipulator set: line changed: no other manipulator\n");
				return -1;
			}

			if (__manipulatorPtr) {
				__manipulatorPtr->reset();
				__manipulatorPtr = 0;
			}
			__manipulatorType = *((MANIPULATOR_TYPE_T*) dataPtr);
			switch (__manipulatorType) {
			case NORMAL_MANIPULATOR:
				__manipulatorPtr = &__normal_manipulator;
				break;
			case POINT_MANIPULATOR:
				__manipulatorPtr = &__point_manipulator;
				break;
			case LINE_MANIPULATOR:
				__manipulatorPtr = &__line_manipulator;
				break;
			case UNDEF_MANIPULATOR:
				break;
			default:
				(void) fprintf(stderr, "kMesh::setValue(TYPE__MANIPULATOR, MANIPULATOR__TYPE): Unknown manipulator type = %d\n", __manipulatorType);
				return -1;
				break;
			}
			return 0;
			break;
		case MANIPULATOR__SET:
			if (!__manipulatorPtr) {
				(void) fprintf(stderr, "kMesh::setValue(TYPE__MANIPULATOR, MANIPULATOR__SET): manipulator not set: call setValue(TYPE__MANIPULATOR, MANIPULATOR__TYPE) first\n");
				return -1;
			}
			// alpha blending enabled
			DXUTGetD3D9Device()->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
			switch (__manipulatorType) {
			case NORMAL_MANIPULATOR:
				__setNormalManipulator(*((int*) dataPtr));
				break;
			case POINT_MANIPULATOR:
				__setPointManipulator(*((int*) dataPtr));
				break;
			case LINE_MANIPULATOR:
				__setLineManipulator(*((int*) dataPtr));
				break;
			case UNDEF_MANIPULATOR:
				break;
			default:
				(void) fprintf(stderr, "kMesh::setValue(TYPE__MANIPULATOR, MANIPULATOR__SET): Unknown manipulator type = %d\n", __manipulatorType);
				return -1;
				break;
			}
			return 0;
			break;
		default:
			if (__manipulatorPtr)
				return __manipulatorPtr->setValue(subType, dataPtr);
			break;
		}
		break;
#endif
	case TYPE__TAPE_MEASURE:
		return __tapeMeasure.setValue(subType, dataPtr);
		break;
	case TYPE__AREA:
		return __area.setValue(subType, dataPtr);
		break;
	case TYPE__ISOMETRIC:
	{
		if ((ISOMETRIC__SUBTYPE) subType == ISOMETRIC__TYPE) {
			if (__isometric.setValue(ISOMETRIC__TYPE, dataPtr) < 0)
				return -1;
			ISOMETRIC_TYPE_T	type = *((ISOMETRIC_TYPE_T*) dataPtr);
#if 1	// 색깔 등고선을 따로 처리
			if (__isometric.getIsColorSavedRef()) {	// 색깔 등고선 모드 -> ...
				// 색깔을 원래대로...
				D3DXVERTEX*	vertexPtr;
				__vertexBufferPtr->Lock(0, 0, (void**) &vertexPtr, 0);
				for (DWORD i = 0; i < __numPointReprs; ++i) {
					D3DXVERTEX&	v = vertexPtr[i];
					v.c = v.c_;
				}
				__vertexBufferPtr->Unlock();

				__isometric.getIsColorSavedRef() = false;

				// 쉐이딩 모드 복원
				__model.getShadingRef() = __isometric.getSavedShadingTypeRef();
			}
			if (type == ISOMETRIC_COLOR) {	// ... -> 색깔 등고선 모드
				// 색깔을 보존
				D3DXVERTEX*	vertexPtr;
				__vertexBufferPtr->Lock(0, 0, (void**) &vertexPtr, 0);
				for (DWORD i = 0; i < __numPointReprs; ++i) {
					D3DXVERTEX&	v = vertexPtr[i];
					v.c_ = v.c;
				}
				__vertexBufferPtr->Unlock();

				__isometric.getIsColorSavedRef() = true;

				// 쉐이딩 모드 변경
				__isometric.getSavedShadingTypeRef() = __model.getShadingRef();
				__model.getShadingRef() = SHADING_NONE;
			}
#else
			if (type & ISOMETRIC_SAVE_COLOR) {
				// 색깔을 보존
				D3DXVERTEX*	vertexPtr;
				__vertexBufferPtr->Lock(0, 0, (void**) &vertexPtr, 0);
				for (DWORD i = 0; i < __numPointReprs; ++i) {
					D3DXVERTEX&	v = vertexPtr[i];
					v.c_ = v.c;
				}
				__vertexBufferPtr->Unlock();
			} else if (type & ISOMETRIC_RESTORE_COLOR) {
				// 색깔을 원래대로...
				D3DXVERTEX*	vertexPtr;
				__vertexBufferPtr->Lock(0, 0, (void**) &vertexPtr, 0);
				for (DWORD i = 0; i < __numPointReprs; ++i) {
					D3DXVERTEX&	v = vertexPtr[i];
					v.c = v.c_;
				}
				__vertexBufferPtr->Unlock();
			}
#endif
		}
		return __isometric.setValue(subType, dataPtr);
	}
	case TYPE__MODEL:
		return __model.setValue(subType, dataPtr);
	case TYPE__SIMULATION:
		if ((SIMULATION__SUBTYPE) subType != SIMULATION__EXEC) {
			// kMesh::__fVolumeChange 와 시뮬레이션 전에 저장된 값 kSimulation::__fVolumeChange0 의 차이로
			// kSimulation::__volume_change_cc 를 설정한다.
			if ((SIMULATION__SUBTYPE) subType == SIMULATION__METHOD)
				__simulation.getVolumeChange0Ref() = __fVolumeChange;

			// 임의조작의 경우, 여기서 색깔값을 저장한다.
			if (
				(SIMULATION__SUBTYPE) subType == SIMULATION__METHOD &&
				*((SIMULATION_TYPE*) dataPtr) == SIMULATION_TYPE_MANUAL
			) {
				// 색깔을 보존
				D3DXVERTEX*	vertexPtr;
				__vertexBufferPtr->Lock(0, 0, (void**) &vertexPtr, 0);
				for (DWORD i = 0; i < __numPointReprs; ++i) {
					D3DXVERTEX&	v = vertexPtr[i];
					v.c_ = v.c;	// 원래 색깔을 보존
					v.c = D3DXVECTOR3(0, 0, 0);	// 검게 칠한다(초기값)
				}
				__vertexBufferPtr->Unlock();

				// 쉐이딩모드를 없앤다.
//				__model.getShadingRef() = SHADING_NONE;

				// 브러쉬모드로 들어간다.
				__statePtr->brush.flag.On = true;
			}
			return __simulation.setValue(subType, dataPtr);
		}
		// SIMULATION__EXEC
		// 여기서 직접 실행한다.
		return __execSimulation(*((SIMULATION_EXEC_TYPE*) dataPtr));
		break;
	case TYPE__BODY_ELEMENT:
		return __bodyElement.setValue(subType, dataPtr);
		break;
	default:
		(void) fprintf(stderr, "TYPE_XXX:= %d: Not implemented yet\n", type);
		break;
	}
	return -1;
}
#endif

#if 1	// 길이 측정
int	kMesh::setMeasure(TAPE_MEASURE_TYPE_T type, float* lengthPtr)
{
	MEASURE_PTR_HASH::const_iterator	it;
	it = __measure_ptr_dic.find(type);
	if (it == __measure_ptr_dic.end()) {
		(void) fprintf(stderr, "TAPE_MEASURE: Type = %d: Not implemented yet\n", type);
		return -1;
	}

	const MEASURE&	measure = *it->second;
	const std::vector<MEASURE_ITEM*>&	measure_item_ptr_list = measure.measure_item_ptr_list;
	size_t	nDef = measure_item_ptr_list.size();
	if (nDef == 0) {
		(void) fprintf(stderr, "TAPE_MEASURE: Type = %d: No element\n", type);
		return -2;
	}
#if 1	// kMeasure
	float&	length = ((MEASURE*) &measure)->length;
	length = 0.f;
	REF_POINT	refPoint;
	for (int n = 0; n < (int) nDef; ++n) {
		float	len;
		if (n == 0) {
			refPoint.use_inPoint = false;
		} else {
			refPoint.use_inPoint = false;//true;
			refPoint.inPoint = refPoint.outPoint;
		}
		refPoint.set_outPoint = true;
		setMeasureItem(*measure_item_ptr_list.at(n), &len, &refPoint);
		length += len;
	}
#else	// kMeasure/!kMeasure
	if (nDef == 1) {	// 순수한 둘레 측정; TO DO LIST: 나중에는 여러 개의 단면에 의한 둘레도 측정한다.
		setMeasureItem(*measure_item_ptr_list.at(0), lengthPtr);

		return 0;
	}
#if 1
	// nDef 는 2의 배수이어야 한다;
	// 2 * k 번째와 2 * k + 1 번째가 k 번째 segment 가 된다. (k = 0, 1, ..., nDef / 2 - 1)
	assert(nDef % 2 == 0);
	// 단면이 있는 경우(MEASURE_ITEM::nDir != D3DXVECTOR3(0, 0, 0))에 둘레를 설정
	for (int i = 0; i < (int) nDef; ++i) {
		MEASURE_ITEM&	m_item = *measure_item_ptr_list.at(i);
		if (m_item.invalid())	// MEASURE_ITEM_DEF
			continue;
		(void) fprintf(stdout,
			"\t{\n\t\t{%d, %ff, %ff},\n\t\tD3DXVECTOR3(%ff, %ff, %ff)\n\t},\n"
			,
			m_item.pointDef.ID, m_item.pointDef.baryU, m_item.pointDef.baryV,
			m_item.nDir.x, m_item.nDir.y, m_item.nDir.z
		);
		if (m_item.isPoint())
			continue;

		setMeasureItem(m_item);
	}
#endif
#if 1
	// 각 단계를 잇는 두 점을 설정;
	D3DXVECTOR3*	vertexArray;
	int&	numSegments = ((MEASURE*) &measure)->numSegments;
	measure.lineVertexBuffer->Lock(0, 0, (void**) &vertexArray, 0);
	float&	length = ((MEASURE*) &measure)->length;
	numSegments = 0;
	length = 0.f;
	// nDef 는 2의 배수
	for (int k = 0; k < (int) nDef / 2; ++k) {	// nDef / 2 개의 segment 가 생긴다.
		D3DXVECTOR3&	P = vertexArray[2 * k + 0];
		D3DXVECTOR3&	V1 = vertexArray[2 * k + 1];

		const MEASURE_ITEM&	start_m_item = *measure_item_ptr_list.at(2 * k + 0);
		bool	startIsPoint = start_m_item.isPoint();
		D3DXVECTOR3	startPoint;
		getPoint(start_m_item.pointDef, startPoint);
		const MEASURE_ITEM&	end_m_item = *measure_item_ptr_list.at(2 * k + 1);
		if (end_m_item.invalid()) {	// MEASURE_ITEM_DEF
			const MEASURE_ITEM&	m_item = start_m_item;
			assert(m_item.__ptr);
			length += m_item.length;
			// 길이를 잰 두 점을 표시할 수 있다면 한다
			switch (m_item.__type) {
			case MEASURE_TYPE__TWO_POINT_DISTANCE:
			{
				const MEASURE_ITEM_PTR_DEF__TWO_POINT_DISTANCE	ptrDef = *((MEASURE_ITEM_PTR_DEF__TWO_POINT_DISTANCE*) m_item.__ptr);
				getPoint(ptrDef.__twoPointDef.point0Def, P);
				getPoint(ptrDef.__twoPointDef.point1Def, V1);
			}
				++numSegments;
				break;
			case MEASURE_TYPE__POINT_PLANE_DISTANCE:
				(void) fprintf(stderr, "ERROR: kMesh::setMeasure: MEASURE_TYPE__POINT_PLANE_DISTANCE: Not implemented yet\n");
				break;
			case MEASURE_TYPE__TWO_PLANE_DISTANCE:
				(void) fprintf(stderr, "ERROR: kMesh::setMeasure: MEASURE_TYPE__POINT_PLANE_DISTANCE: Not implemented yet\n");
				break;
			case MEASURE_TYPE__GIRTH:
				break;
			case MEASURE_TYPE__TWO_POINT_GIRTH:
			{
				const TWO_POINT_PLANE_DEF&	_2pDef = ((MEASURE_ITEM_PTR_DEF__TWO_POINT_GIRTH*) m_item.__ptr)->__2pDef;
				getPoint(_2pDef.point0Def, P);
				getPoint(_2pDef.point1Def, V1);
			}
				++numSegments;
				break;
			default:
				(void) fprintf(stderr, "ERROR: kMesh::setMeasure: Unknown MEASURE_ITEM_DEF::__type: %d\n", m_item.__type);
				break;
			}

			continue;
		}
		bool	endIsPoint = end_m_item.isPoint();
		D3DXVECTOR3	endPoint;
		getPoint(end_m_item.pointDef, endPoint);
		// 3 종류가 있다:
		//	점 <-> 점
		//	단면 <-> 점
		//	단면 <-> 단면
		if (startIsPoint && endIsPoint) {	// 점 -> 점
			P = startPoint;
			V1 = endPoint;
			length += D3DXVec3Length(&D3DXVECTOR3(P - V1));
		} else if (!startIsPoint && !endIsPoint) {	// 단면 -> 단면
			// 아래의 점 -> 단면 의 결과를 이용하면...
			// 각 단면의 벡터가 평행이라고 가정한다.
			if (D3DXVec3Dot(&start_m_item.nDir, &end_m_item.nDir) != 1.f) {
				(void) fprintf(stderr, "D3DXVec3Dot(&start_m_item.nDir, &end_m_item.nDir) = %f (!= 1.f)\n", D3DXVec3Dot(&start_m_item.nDir, &end_m_item.nDir));
			}
//			assert(D3DXVec3Dot(&start_m_item.nDir, &end_m_item.nDir) == 1.f);
			const D3DXVECTOR3&	nDir = start_m_item.nDir;
			P = endPoint;
			const D3DXVECTOR3&	Q = startPoint;
			FLOAT	t = D3DXVec3Dot(&nDir, &D3DXVECTOR3(P - Q));
			length += fabsf(t);
			V1 = P - t * nDir;
//			V1 = P - D3DXVec3Dot(&nDir, &D3DXVECTOR3(P - Q)) * nDir;
		} else {	// 점 -> 단면
			// Q. 한 점 P 에서, 법선 벡터가 N 이고 평면 위의 한 점이 Q 인 평면 사이의 거리는?
			// A. P 를 지나고 N 에 평행한 벡터가 만드는 임의의 위치를 매개변수 t 로 쓰면
			// P + t * N 이다; 두 점 사이의 거리는 |t * N| 이다.
			// 이 점과 Q 를 잇는 벡터가 N 과 수직이어야 한다:
			//	((Q - P) + t * N) . N = 0
			//	t = (Q - P) . N / -N . N
			// N 이 단위벡터라면
			//	t = N . (P - Q)
			// 따라서 다른 한 점은 P - (N . (P - Q)) * N 이다.
			D3DXVECTOR3	nDir;
			D3DXVECTOR3	Q;
			if (startIsPoint) {	// 단면 -> 점
				nDir = end_m_item.nDir;
				P = startPoint;
				Q = endPoint;
			} else {	// 점 -> 단면
				nDir = start_m_item.nDir;
				P = endPoint;
				Q = startPoint;
			}
			(void) fprintf(stdout, "length(nDir) = %f\n", D3DXVec3Length(&nDir));
			D3DXVec3Normalize(&nDir, &nDir);
			(void) fprintf(stdout, "length(nDir) = %f\n", D3DXVec3Length(&nDir));
//			assert(D3DXVec3Length(&nDir) == 1.f);
			FLOAT	t = D3DXVec3Dot(&nDir, &D3DXVECTOR3(P - Q));
			length += fabsf(t);
			V1 = P - t * nDir;
		}
		++numSegments;
	}
	measure.lineVertexBuffer->Unlock();
#endif
#endif	// !kMeasure
	if (lengthPtr) {
		*lengthPtr = measure.length;
	}

	return 0;
}

int	kMesh::ResetTapeMeasure()
{
#if 1
	__statePtr->__nearestIntersectionIndex = -1;
	__statePtr->__nextIntersectionIndex = 0;
	__statePtr->__numIntersection = 0;
	__numSegments = 0;
#if 1	// manipulator
	MANIPULATOR_TYPE_T	type = UNDEF_MANIPULATOR;
	setValue(TYPE__MANIPULATOR, MANIPULATOR__TYPE, &type);
	__statePtr->manipulator_mode = false;
	__statePtr->__showSegment = true;
	// alpha blending disabled
	DXUTGetD3D9Device()->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
#endif
#if 1	// 2D단면
	SAFE_RELEASE(__2dProjectVertexBufferPtr);
	SAFE_RELEASE(__2dInterpolatedProjectVertexBufferPtr);
	SAFE_RELEASE(__2dFillProjectVertexBufferPtr);
	__2dProjectNumPoints = __2dInterpolatedProjectNumPoints = __2dFillProjectNumPoints = 0;
#endif

	__tapeMeasure.length() = -1.f;
#if 1	// SEGMENT_VERTEX
	__tapeMeasure.twoPointArray().RemoveAll();
	__tapeMeasure.convexPointArray().RemoveAll();
#else
	__tapeMeasure.twoPointArray().resize(0);
	__tapeMeasure.convexPointArray().resize(0);
#endif
#else
	MEASURE_PTR_HASH::const_iterator	it;
	for (MEASURE_PTR_HASH::const_iterator it = __measure_ptr_dic.begin(); it != __measure_ptr_dic.end(); ++it) {
		const MEASURE&	measure = *it->second;
		const std::vector<MEASURE_ITEM*>&	measure_item_ptr_list = measure.measure_item_ptr_list;
		size_t	nDef = measure_item_ptr_list.size();
		for (int i = 0; i < (int) nDef; ++i) {
			MEASURE_ITEM&	m_item = *measure_item_ptr_list.at(i);
			if (m_item.invalid())	// MEASURE_ITEM_DEF
				continue;
			if (m_item.isPoint())
				continue;

			m_item.allSet = false;
//			((kMesh*) this)->setMeasureItem(m_item);
		}
	}
#endif

	return 0;
}

void	kMesh::setMeasureItem(MEASURE_ITEM& m_item, float* lengthPtr, REF_POINT* refPointPtr)
{
	if (m_item.allSet) {	// 이미 결정되었다면 통과
		if (lengthPtr)
			*lengthPtr = m_item.length;
		return;
	}
	m_item.allSet = true;

#if 1	// kMeasure
	assert(m_item.__ptr);
	float&	length = m_item.length;
	bool&	closed = m_item.closed;
	CGrowableArray<SEGMENT_VERTEX>	pointArray;
	switch (m_item.__type) {
	case MEASURE_TYPE__TWO_POINT_DISTANCE:
		((MEASURE_ITEM_PTR_DEF__TWO_POINT_DISTANCE*) m_item.__ptr)->calculate(*this, closed, length, pointArray, refPointPtr);
		break;
	case MEASURE_TYPE__POINT_PLANE_DISTANCE:
		((MEASURE_ITEM_PTR_DEF__POINT_PLANE_DISTANCE*) m_item.__ptr)->calculate(*this, closed, length, pointArray, refPointPtr);
		break;
	case MEASURE_TYPE__TWO_PLANE_DISTANCE:
		((MEASURE_ITEM_PTR_DEF__TWO_PLANE_DISTANCE*) m_item.__ptr)->calculate(*this, closed, length, pointArray, refPointPtr);
		break;
	case MEASURE_TYPE__GIRTH:
		((MEASURE_ITEM_PTR_DEF__GIRTH*) m_item.__ptr)->calculate(*this, closed, length, pointArray, refPointPtr);
		break;
	case MEASURE_TYPE__TWO_POINT_GIRTH:
		((MEASURE_ITEM_PTR_DEF__TWO_POINT_GIRTH*) m_item.__ptr)->calculate(*this, closed, length, pointArray, refPointPtr);
		break;
	default:
		(void) fprintf(stderr, "ERROR: kMesh::setMeasureItem: MEASURE_ITEM_DEF::__type: %d: Unknown\n", m_item.__type);
		break;
	}
	cout << "bool: " << (closed ? "True" : "False") << ", float: " << length << ", # = " << pointArray.GetSize() << endl;
	int&	numSegments = m_item.numSegments;
	numSegments = pointArray.GetSize();
	if (!closed)
		--numSegments;
#if 1	// manipulator; LINE MANIPULATOR
	m_item.isReleased() = false;
#endif	// manipulator; LINE MANIPULATOR
	__tapeMeasure.length() = -1.f;	// 임의계측에서 참고할 수 있도록 설정
	SetSegmentBuffer(m_item.segmentVertexBuffer, m_item.segmentIndexBuffer, pointArray, closed);
	if (lengthPtr)
		*lengthPtr = length;
	return;
#else	// kMeasure/!kMeasure
	if (m_item.__ptr) {
		switch (m_item.__type) {
		case MEASURE_TYPE__TWO_POINT_DISTANCE:
		case MEASURE_TYPE__POINT_PLANE_DISTANCE:
		case MEASURE_TYPE__TWO_PLANE_DISTANCE:
			(void) fprintf(stdout, "WARNING: kMesh::setMeasureItem: MEASURE_ITEM_DEF::__type: %d: Not implemented yet\n", m_item.__type);
			break;
		case MEASURE_TYPE__GIRTH:
		case MEASURE_TYPE__TWO_POINT_GIRTH:
		{
			TAPE_MEASURE_MODE_T	mode;
			START_DEF_PTR	startDefPtr;
			if (m_item.__type == MEASURE_TYPE__GIRTH) {
				startDefPtr.type = START_DEF_PTR_TYPE__GIRTH;
				MEASURE_ITEM_PTR_DEF__GIRTH&	def = *((MEASURE_ITEM_PTR_DEF__GIRTH*) m_item.__ptr);
				mode = def.__mode;
				startDefPtr.plane_def_ptr = &def.__planeDef;
			} else if (m_item.__type == MEASURE_TYPE__TWO_POINT_GIRTH) {
				startDefPtr.type = START_DEF_PTR_TYPE__TWO_POINT;
				MEASURE_ITEM_PTR_DEF__TWO_POINT_GIRTH&	def = *((MEASURE_ITEM_PTR_DEF__TWO_POINT_GIRTH*) m_item.__ptr);
				mode = def.__mode;
				startDefPtr.two_point_plane_def_ptr = &def.__2pDef;
			} else
				assert(0);

#if 1	// SEGMENT_VERTEX
			CGrowableArray<SEGMENT_VERTEX>	segmentPointArray;
			CGrowableArray<SEGMENT_VERTEX>	twoPointArray;
			CGrowableArray<SEGMENT_VERTEX>	convexPointArray;
#else
			std::vector<D3DXVECTOR3>	segmentPointArray;
			std::vector<D3DXVECTOR3>	twoPointArray;
			std::vector<D3DXVECTOR3>	convexPointArray;
#endif
			float&	length = m_item.length;
			bool&	closed = m_item.closed;
			int&	numSegments = m_item.numSegments;
			numSegments = kTapeMeasure::doMeasure_s(
				this,

				mode,
				startDefPtr,

				// [OUT]
				NULL,
				segmentPointArray, length, closed,
				twoPointArray, convexPointArray
			);
#if 1	// manipulator; LINE MANIPULATOR
			m_item.isReleased() = false;
#endif	// manipulator; LINE MANIPULATOR
			__tapeMeasure.length() = -1.f;	// 임의계측에서 참고할 수 있도록 설정
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
			SetSegmentBuffer(
				m_item.segmentVertexBuffer, m_item.segmentIndexBuffer,
				mode == TAPE_MEASURE_MODE_CONTOUR ?
				(startDefPtr.type == START_DEF_PTR_TYPE__GIRTH ?
					segmentPointArray :
					twoPointArray
				) :
					convexPointArray,
				closed
			);
			if (lengthPtr) {
				*lengthPtr = length;
			}
		}
			break;
		default:
			(void) fprintf(stderr, "ERROR: kMesh::setMeasureItem: MEASURE_ITEM_DEF::__type: %d: Unknown\n", m_item.__type);
			break;
		}
		return;
	}

	// 법선벡터
	D3DXVECTOR3	nDir;
#if 1
	nDir = m_item.nDir;
#endif

	// 둘레 측정에 필요한 준비를 한다: EdgeID, t
	FLOAT	t;
	DWORD	edgeID = kTapeMeasure::findEdge(
		this,
		nDir,

		m_item.pointDef.ID, m_item.pointDef.baryU, m_item.pointDef.baryV,

		t,
		true
	);
	assert(edgeID != (DWORD) -1);
	if (edgeID == (DWORD) -1)	// 둘레를 측정할 수 없다.
		return;

	// 초기설정을 한 후,
	__tapeMeasure.setBasePoint(edgeID, t, -1, m_item.pointDef.ID);
	// 둘레를 측정한다.
	bool	closed;
#if 1	// SEGMENT_VERTEX
	CGrowableArray<SEGMENT_VERTEX>	segmentPointArray;
#else
	std::vector<D3DXVECTOR3>	segmentPointArray;
#endif
	int&	numSegments = m_item.numSegments;
	numSegments = __tapeMeasure.doMeasure(
		nDir,
		segmentPointArray,
		m_item.length, closed
	);
	(void) fprintf(stdout,
		"# of segments = %d, length = %f, closed? %s; # of segment point(s) = %d\n"
		,
#if 1	// SEGMENT_VERTEX
		numSegments, m_item.length, closed ? "True" : "False", segmentPointArray.GetSize()
#else
		numSegments, m_item.length, closed ? "True" : "False", segmentPointArray.size()
#endif
	);
	SetSegmentBuffer(m_item.segmentVertexBuffer, m_item.segmentIndexBuffer, segmentPointArray, closed);
	if (lengthPtr) {
		*lengthPtr = m_item.length;
	}
#endif	// !kMeasure
}

int	kMesh::Render__TAPE_MEASURE() const
{
	MEASURE_PTR_HASH::const_iterator	it;
	int	valType = __tapeMeasure.getValRef();
	for (MEASURE_PTR_HASH::const_iterator it = __measure_ptr_dic.begin(); it != __measure_ptr_dic.end(); ++it) {
		if (!(valType & it->first))
			continue;

		const MEASURE&	measure = *it->second;
		const std::vector<MEASURE_ITEM*>&	measure_item_ptr_list = measure.measure_item_ptr_list;
		size_t	nDef = measure_item_ptr_list.size();
//		(void) fprintf(stdout, "# of def. = %d\n", nDef);
#if 0
{
	D3DXVECTOR2	a(10, 10);
	D3DXVECTOR2	b(100, 100);
	D3DXVECTOR2	p[2];
	p[0] = a;
	p[1] = b;

	p[0] = measure.tag.arrow_start_point;
	const MEASURE_ITEM&	m_item = *measure_item_ptr_list.at(0);
	D3DVIEWPORT9	viewport;
	D3DXVECTOR3	P1;
	D3DXVECTOR3	P0 = m_item.__ptr->arrow_end_point;
	DXUTGetD3D9Device()->GetViewport(&viewport);
	D3DXVec3Project(&P1, &P0, &viewport, &__projectionMatrix, &__viewMatrix, &__worldMatrix);
	p[1].x = P1.x; p[1].y = P1.y;	// overwrite p[1]
	LPD3DXLINE	linePtr = 0;
	D3DXCreateLine(DXUTGetD3D9Device(), &linePtr);
	linePtr->Begin();
	linePtr->Draw(p, 2, measure.tag.color);
	linePtr->End();
	SAFE_RELEASE(linePtr);
}
#endif
		for (int i = 0; i < (int) nDef; ++i) {
			const MEASURE_ITEM&	m_item = *measure_item_ptr_list.at(i);
#if 1	// kMeasure
#else	// kMeasure/!kMeasure
			if (m_item.invalid())
				continue;
			if (m_item.isPoint())
				continue;
#endif	// !kMeasure

			((kMesh*) this)->setMeasureItem(*((MEASURE_ITEM*) &m_item));
			D3DXVECTOR4	color;
			//#define D3DCOLOR_ARGB(a,r,g,b) ((D3DCOLOR)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))
			DWORD	dwColor = (DWORD) measure.tag.color;
			color.z = ((DWORD) (dwColor & 0xff)) / 255.f; dwColor >>= 8;
			color.y = ((DWORD) (dwColor & 0xff)) / 255.f; dwColor >>= 8;
			color.x = ((DWORD) (dwColor & 0xff)) / 255.f; dwColor >>= 8;
			color.w = ((DWORD) (dwColor & 0xff)) / 255.f; dwColor >>= 8;
			ShowString(m_item.segmentVertexBuffer, m_item.segmentIndexBuffer, m_item.numSegments, nDef == 1 ? color : (i == 0 ? D3DXVECTOR4(0, 0, 1, 1) : (i == 1 ? D3DXVECTOR4(1, 0, 0, 1) : D3DXVECTOR4(0, 1, 0, 1))));
		}
#if 1	// kMeasure
#else	// kMeasure/!kMeasure
		if (nDef > 1) {
			ShowIsometric(measure.lineVertexBuffer, measure.numSegments, D3DXVECTOR4(0, 1, 1, 1));
			ShowVertex(measure.lineVertexBuffer, __OnePointDeclarationPtr, sizeof(D3DXVECTOR3), 2 * measure.numSegments, 5.f);
		}
#endif	// !kMeasure
	}

	return 0;
}
#endif

#if 1	// 무게 측정
int	kMesh::getWeight(WEIGHT_MEASURE__TYPE weightMeasureType, float* weightPtr)
{
	int	iEnum = (int) weightMeasureType;
	if (iEnum < 0 || iEnum >= N_WEIGHT_MEASURE) {
		(void) fprintf(stderr, "WEIGHT_MEASURE: type:= %d: Not implemented yet\n", weightMeasureType);
		return -1;
	}
	*weightPtr = __weightValues[iEnum];
	return 0;
}
#endif

#if 1	// 모눈
void	kMesh::DrawGrid() const
{
#if 1	// TYPE
	if (((kGrid*) __statePtr)->getTypeRef() != GRID_UNDEF) {
		__select.DrawGrid(*((kGrid*) __statePtr), __worldMatrix, __viewMatrix, __projectionMatrix);
	}
#else	// TYPE/!TYPE
	if (__statePtr->__z != GRID_UNDEF) {
		__select.DrawGrid(*((kGrid*) __statePtr), __worldMatrix, __viewMatrix, __projectionMatrix);
	}
#endif	// !TYPE
}
#endif

#if 1	// 등고선
int	kIsometric::getValue(int subType, void* dataPtr)
{
	if (!dataPtr) {
		(void) fprintf(stderr, "Fatal: kIsometric::getValue: dataPtr is nil\n");
		return -1;
	}

	switch (subType) {
	case ISOMETRIC__TYPE:
		*((ISOMETRIC_TYPE_T*) dataPtr) = __type;
		return 0;
		break;
	case ISOMETRIC__PLANE:
		*((ISOMETRIC_PLANE_T*) dataPtr) = __plane;
		return 0;
		break;
	default:
		(void) fprintf(stderr, "Error: kIsometric::getValue: ISOMETRIC__SUBTYPE = %d: Unknown subType\n", subType);
	}

	return -1;
}

int	kIsometric::setValue(int subType, const void* dataPtr)
{
	if (!dataPtr) {
		(void) fprintf(stderr, "Fatal: kIsometric::setValue: dataPtr is nil\n");
		return -1;
	}

	switch (subType) {
	case ISOMETRIC__TYPE:
		__type = *((ISOMETRIC_TYPE_T*) dataPtr);
#if 0	// 제형욱씨의 요청
		if (__type != ISOMETRIC_UNDEF && __type != ISOMETRIC_LINE && __type != ISOMETRIC_COLOR) {
			__type = ISOMETRIC_UNDEF;
			return -1;
		}
#endif
		return 0;
		break;
	case ISOMETRIC__PLANE:
		__plane = *((ISOMETRIC_PLANE_T*) dataPtr);
		return 0;
		break;
	default:
		(void) fprintf(stderr, "Error: kIsometric::setValue: ISOMETRIC__SUBTYPE = %d: Unknown subType\n", subType);
	}

	return -1;
}
void	kMesh::ShowIsometric() const
{
	const ISOMETRIC_TYPE_T	type = __isometric.__type;
	const ISOMETRIC_PLANE_T	plane = __isometric.__plane;
	if (type == ISOMETRIC_LINE) {
		if (plane & ISOMETRIC_XY)
			ShowIsometric(
				__z_isometricVertexBufferPtr, __z_isometricLines,
				D3DXVECTOR4(0, 1, 1, 1)		// 등고선 색깔 (모두 공통); TO DO LIST
			);
		if (plane & ISOMETRIC_YZ)
			ShowIsometric(
				__x_isometricVertexBufferPtr, __x_isometricLines,
				D3DXVECTOR4(0, 1, 1, 1)		// 등고선 색깔 (모두 공통); TO DO LIST
			);
		if (plane & ISOMETRIC_ZX)
			ShowIsometric(
				__y_isometricVertexBufferPtr, __y_isometricLines,
				D3DXVECTOR4(0, 1, 1, 1)		// 등고선 색깔 (모두 공통); TO DO LIST
			);
	} else if (type == ISOMETRIC_COLOR) {
		kMesh::D3DXVERTEX*	vertexPtr;
		__vertexBufferPtr->Lock(0, 0, (void**) &vertexPtr, 0);
		for (DWORD i = 0; i < __numPointReprs; ++i) {
			kMesh::D3DXVERTEX&	v = vertexPtr[i];
			float	fXYZ = plane == ISOMETRIC_XY ? v.XY : (plane == ISOMETRIC_YZ ? v.YZ : v.ZX);
			fXYZ *= 3;	// (R, G, B) 로 분배
			v.c.x = fXYZ;
			v.c.y = 0;
			v.c.z = 0;
			CLAMP2(v.c);
		}
		__vertexBufferPtr->Unlock();
	}
}
#endif

#if 1	// 모델
int	kModel::getValue(int subType, void* dataPtr)
{
	if (!dataPtr) {
		(void) fprintf(stderr, "Fatal: kModel::getValue: dataPtr is nil\n");
		return -1;
	}

	switch (subType) {
	case MODEL__OBJECT:
		*((OBJECT_TYPE*) dataPtr) = __object;
		return 0;
		break;
	case MODEL__SHADING:
		*((SHADING_TYPE*) dataPtr) = __shading;
		return 0;
		break;
	default:
		(void) fprintf(stderr, "Error: kModel::getValue: MODEL__SUBTYPE = %d: Unknown subType\n", subType);
	}

	return -1;
}

int	kModel::setValue(int subType, const void* dataPtr)
{
	if (!dataPtr) {
		(void) fprintf(stderr, "Fatal: kModel::setValue: dataPtr is nil\n");
		return -1;
	}

	switch (subType) {
	case MODEL__OBJECT:
		__object = *((OBJECT_TYPE*) dataPtr);
		return 0;
		break;
	case MODEL__SHADING:
		__shading = *((SHADING_TYPE*) dataPtr);
		return 0;
		break;
	default:
		(void) fprintf(stderr, "Error: kModel::setValue: MODEL__SUBTYPE = %d: Unknown subType\n", subType);
	}

	return -1;
}

void	kMesh::ShowModel() const
{
	const OBJECT_TYPE&	object = __model.getObjectRef();
	if (object & OBJECT_VERTEX) {
		ShowVertex(
			__vertexBufferPtr, __vertexDeclarationPtr, sizeof(D3DXVERTEX), __numPointReprs
		);
	}
	if (object & OBJECT_EDGE) {
		ShowEdge(
			__edgeIndexBufferPtr, D3DXVECTOR4(0, 0, 1, 1), __numEdges
		);
	}
	if (object & OBJECT_FACE) {
		if (__objRHLoaded && __model.getShadingRef() == SHADING_TEXTURE) {
			ShowSubsetFace();
		} else {
#if 0
			DWORD	cullMode;
			__devicePtr->GetRenderState(D3DRS_CULLMODE, &cullMode);
			__devicePtr->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
			ShowFace(__faceIndexBufferPtr, __numFaces, __model.getShadingRef());
			__devicePtr->SetRenderState(D3DRS_CULLMODE, cullMode);
#else
			ShowFace(
				__faceIndexBufferPtr, __numFaces,
				__model.getShadingRef()
			);
#endif
		}
	}
	if (object & OBJECT_NORMAL) {
		FLOAT	fScale = .5f * __a;
		D3DXVECTOR4	cColor(0.f, 1.f, 0.f, 1.f);
		__showNormal.ShowNormals(
			fScale, D3DXVECTOR4(0, 1, 0, 1),
			__worldMatrix, __viewMatrix, __projectionMatrix
		);
	}
	if (object & OBJECT_VOXEL) {
		RenderVoxel();
	}
}
#endif

#if 1	// 시뮬레이션
#if 1	// 색깔템플릿
void	kMesh::IncIndexTemplateColor()
{
	++__indexTemplateColor;
	if (__indexTemplateColor >= N_MODEL_POINT_TYPE * N_BREAST_IMPLANT_TYPE)
		__indexTemplateColor = 0;

	// 새로 색깔을 칠하도록 한다.
	__setTemplateColorDone = false;
}
void	kMesh::DecIndexTemplateColor()
{
	--__indexTemplateColor;
	if (__indexTemplateColor < 0)
		__indexTemplateColor = N_MODEL_POINT_TYPE * N_BREAST_IMPLANT_TYPE - 1;

	// 새로 색깔을 칠하도록 한다.
	__setTemplateColorDone = false;
}
void	kMesh::ShowTemplateColor()
{
	if (__indexTemplateColor < 0 || __indexTemplateColor >= N_MODEL_POINT_TYPE * N_BREAST_IMPLANT_TYPE) {
		(void) fprintf(stderr, "indexTemplateColor = %d: OUT_OT_RANGE\n", __indexTemplateColor);
		return;
	}

	if (__setTemplateColorDone)	// 이미 색깔이 반영되었다.
		return;

	kMesh::D3DXVERTEX*	vertexPtr;
	__vertexBufferPtr->Lock(0, 0, (void**) &vertexPtr, 0);
	for (DWORD i = 0; i < __numPointReprs; ++i) {
		D3DXVERTEX&	v = vertexPtr[i];
		v.c = D3DXVECTOR3(0, 0, 0);
	}
	const TEMPLATE_COLOR&	templateColor = BREAST_IMPLANT_TEMPLATE::ColorArray[__indexTemplateColor / N_BREAST_IMPLANT_TYPE][__indexTemplateColor % N_BREAST_IMPLANT_TYPE];
	(void) fprintf(stdout, "BREAST_IMPLANT_TEMPLATE::ColorArray[%d][%d]: # = %d\n", __indexTemplateColor / N_BREAST_IMPLANT_TYPE, __indexTemplateColor % N_BREAST_IMPLANT_TYPE, templateColor.size());
	for (int n = 0; n < (int) templateColor.size(); ++n) {
		const MODEL_COLOR&	modelColor = templateColor.at(n);
		FACE_ID_LOCAL_INDEX	faceIDLocalIndex;
		faceIDLocalIndex.ID = modelColor.faceID;
		faceIDLocalIndex.local_index = modelColor.localIndex;
		DWORD	prID = GetPrID(faceIDLocalIndex);
		if (prID == (DWORD) -1)
			continue;
		D3DXVERTEX&	v = vertexPtr[prID];
		v.c.x = modelColor.fColorRed;
		v.c.y = modelColor.fColorGreen;
		v.c.z = modelColor.fColorBlue;
	}
	__vertexBufferPtr->Unlock();

	// 색깔이 반영되었음을 표시한다.
	__setTemplateColorDone = true;
}
#endif	// 색깔템플릿

// SIMULATION__SUBTYPE = SIMULATION__EXEC
int	kMesh::__execSimulation(SIMULATION_EXEC_TYPE execType)
{
	// 직접 칠하면서(MANUAL_DIRECTLY) 임의조작(SIMULATION_TYPE_MANUAL)인 경우에는
	// SIMULATION_EXEC_UNDEF 을 제외하고 아무일도 하지 않는다;
	// kMesh::brush 에서 한다.
	if (
		__simulation.getTypeRef() == SIMULATION_TYPE_MANUAL &&
		__simulation.getManualRef().type == MANUAL_DIRECTLY
	) {
		if (execType == SIMULATION_EXEC_UNDEF) {
			// 색깔을 원래대로...
			D3DXVERTEX*	vertexPtr;
			__vertexBufferPtr->Lock(0, 0, (void**) &vertexPtr, 0);
			for (DWORD i = 0; i < __numPointReprs; ++i) {
				D3DXVERTEX&	v = vertexPtr[i];
				v.c = v.c_;
			}
			__vertexBufferPtr->Unlock();

			// 시뮬레이션중이 아닌 상태로 되돌린다.
			__statePtr->brush.flag.On = false;
			__simulation.getIsReadyRef() = false;
			__simulation.getExecTypeRef() = SIMULATION_EXEC_UNDEF;
		}
		return 0;
	}

	// 시뮬레이션을 하기 전에, 보존하지 않았다면, 색깔을 보존한다;
	// 시뮬레이션을 시작할 색깔을 결정한다. (SIMULATION_TYPE_MANUAL 이 아닌 경우)
	if (
		// 임의조작의 경우,
		//SIMULATION__SUBTYPE = SIMULATION__METHOD 에서 저장한다.
		__simulation.getTypeRef() != SIMULATION_TYPE_MANUAL &&
		__simulation.getIsReadyRef() == false
	) {	// 아직 색깔을 보존하지 않았다.
		if (__simulation.getExecTypeRef() != SIMULATION_EXEC_UNDEF) {
			(void) fprintf(stderr, "Warning: 시뮬레이션을 시작했으나 중지하지 않고 다시 시작했다; 색깔 보존 단계는 건너뛴다\n");
		} else {
			// 색깔을 보존
			D3DXVERTEX*	vertexPtr;
			__vertexBufferPtr->Lock(0, 0, (void**) &vertexPtr, 0);
			for (DWORD i = 0; i < __numPointReprs; ++i) {
				D3DXVERTEX&	v = vertexPtr[i];
				v.c_ = v.c;
			}
			__vertexBufferPtr->Unlock();
		}

		// 시뮬레이션을 할 색깔을 설정한다;
		if (__simulation.getTypeRef() == SIMULATION_TYPE_MANUAL) {
			D3DXVERTEX*	vertexPtr;
			__vertexBufferPtr->Lock(0, 0, (void**) &vertexPtr, 0);
			for (DWORD i = 0; i < __numPointReprs; ++i) {
				D3DXVERTEX&	v = vertexPtr[i];
				v.c = D3DXVECTOR3(0, 0, 0);
			}
			__vertexBufferPtr->Unlock();

			__simulation.getExecTypeRef() = execType;
			__simulation.getIsReadyRef() = true;	// 색깔을 보존했다고 설정한다.
			__statePtr->brush.flag.On = true;

			return 0;
		} else if (__simulation.getTypeRef() == SIMULATION_TYPE_TEMPLATE) {
			// BREAST_IMPLANT_TEMPLATE::implantType,
			// BREAST_IMPLANT_TEMPLATE::positionType 을 첨자로 사용하여,
			// BREAST_IMPLANT_TEMPLATE::ColorArray 로부터 색깔을 얻는다.
			const BREAST_IMPLANT_TEMPLATE&	implantTemplate = __simulation.getTemplateRef();
			int	indexTemplateColor = N_BREAST_IMPLANT_TYPE * implantTemplate.positionType + implantTemplate.implantType;
			if (indexTemplateColor < 0 || indexTemplateColor >= N_MODEL_POINT_TYPE * N_BREAST_IMPLANT_TYPE) {
				(void) fprintf(stderr, "indexTemplateColor = %d: OUT_OT_RANGE\n", indexTemplateColor);
				return -1;
			}

			D3DXVERTEX*	vertexPtr;
			__vertexBufferPtr->Lock(0, 0, (void**) &vertexPtr, 0);
			for (DWORD i = 0; i < __numPointReprs; ++i) {
				D3DXVERTEX&	v = vertexPtr[i];
				v.c = D3DXVECTOR3(0, 0, 0);
			}
			const TEMPLATE_COLOR&	templateColor = BREAST_IMPLANT_TEMPLATE::ColorArray[indexTemplateColor / N_BREAST_IMPLANT_TYPE][indexTemplateColor % N_BREAST_IMPLANT_TYPE];
			(void) fprintf(stdout, "BREAST_IMPLANT_TEMPLATE::ColorArray[%d][%d]: # = %d\n", indexTemplateColor / N_BREAST_IMPLANT_TYPE, indexTemplateColor % N_BREAST_IMPLANT_TYPE, templateColor.size());
			for (int n = 0; n < (int) templateColor.size(); ++n) {
				const MODEL_COLOR&	modelColor = templateColor.at(n);
				FACE_ID_LOCAL_INDEX	faceIDLocalIndex;
				faceIDLocalIndex.ID = modelColor.faceID;
				faceIDLocalIndex.local_index = modelColor.localIndex;
				DWORD	prID = GetPrID(faceIDLocalIndex);
				if (prID == (DWORD) -1)
					continue;
				D3DXVERTEX&	v = vertexPtr[prID];
				v.c.x = modelColor.fColorRed;
				v.c.y = modelColor.fColorGreen;
				v.c.z = modelColor.fColorBlue;
			}
			__vertexBufferPtr->Unlock();
		} else if (__simulation.getTypeRef() == SIMULATION_TYPE_STANDARD) {
			// 해당 영역을 단색(흰색)으로 칠한다.
			const STANDARD_SIMULATION&	standard = __simulation.getStandardRef();
			const AREA_TYPE_T&	areaType = standard.area;
			D3DXVERTEX*	vertexArray;
			__vertexBufferPtr->Lock(0, 0, (void**) &vertexArray, 0);
			for (int n = 0; n < (int) __numPointReprs; ++n)
				vertexArray[n].c = D3DXVECTOR3(0, 0, 0);
			for (kArea::DEFINED_AREA_DIC::const_iterator area_it = __defined_area_dic.begin(); area_it != __defined_area_dic.end(); ++area_it) {
				if (!(areaType & area_it->first))
					continue;
				const kArea::DEFINED_AREA&	defArea = area_it->second;

				// 해당 영역에 표시
				const std::set<DWORD>&	face_id_set = defArea.face_id_set;
				std::set<DWORD>::const_iterator	it;
				for (std::set<DWORD>::const_iterator it = face_id_set.begin(); it != face_id_set.end(); ++it) {
					DWORD	faceID = *it;
					if (faceID >= __numFaces) {
						(void) fprintf(stderr, "FATAL: kMesh::__standard_total_volume_change: Face ID = %d: Out of range: Ignored\n", faceID);
						continue;
					}
					const CFace&	face = __facePtr[faceID];
					for (int i = 0; i < 3; ++i)
						vertexArray[face.prID[i]].c = D3DXVECTOR3(1, 1, 1);
				}
			}
			__vertexBufferPtr->Unlock();
		} else {
			(void) fprintf(stderr, "Warning: Simulation Type = %d: Invalid for saving color\n", __simulation.getTypeRef());
			return -1;
		}

		__simulation.getIsReadyRef() = true;	// 색깔을 보존했다고 설정한다.
	}
	__simulation.getExecTypeRef() = execType;

	// __simulation 을 내용을 바탕으로, execType 에 따라 시뮬레이션을 진행한다;
	// TO DO LIST: 어떻게 __simulation 에 필요한 정보가 설정되었는 지 알 수 있나?
	if (execType == SIMULATION_EXEC_UNDEF) {
		// 색깔을 원래대로...
		D3DXVERTEX*	vertexPtr;
		__vertexBufferPtr->Lock(0, 0, (void**) &vertexPtr, 0);
		for (DWORD i = 0; i < __numPointReprs; ++i) {
			D3DXVERTEX&	v = vertexPtr[i];
			v.c = v.c_;
		}
		__vertexBufferPtr->Unlock();

		// 시뮬레이션중이 아닌 상태로 되돌린다.
		__statePtr->brush.flag.On = false;
		__simulation.getIsReadyRef() = false;
		__simulation.getExecTypeRef() = SIMULATION_EXEC_UNDEF;

		return 0;
	}

	if (
#if 1
		(
			__simulation.getCcTypeRef() == SIMULATION_CC_TYPE_FINAL &&
			execType == SIMULATION_EXEC_DONE
		) ||
		(
			__simulation.getCcTypeRef() == SIMULATION_CC_TYPE_VOLUME_CHANGE &&
			__simulation.getStepTypeRef() == SIMULATION_STEP_TYPE_CC &&
			(execType == SIMULATION_EXEC_PLUS || execType == SIMULATION_EXEC_MINUS)
		)
#else
		execType == SIMULATION_EXEC_DONE ||
		execType == SIMULATION_EXEC_PLUS_STEP ||
		execType == SIMULATION_EXEC_MINUS_STEP
#endif
	) {
		float	volumeChange = 0.f;
		float	fNormalAmountGuess = .01f * __a;	// fNormalAmount 의 초기 추정값
		if (execType == SIMULATION_EXEC_DONE) {
			// 주어진 색깔을 사용하여 주어진 cc 만큼 변화를 준다.
			volumeChange = __simulation.getFinalCcRef();
		} else if (execType == SIMULATION_EXEC_PLUS) {
			volumeChange = __simulation.getCcStepRef();	// +:= 늘어난다
		} else if (execType == SIMULATION_EXEC_MINUS) {
			volumeChange = -__simulation.getCcStepRef();	// -:= 줄어든다
		}

		// 표준체형으로의 변환인 경우, findNormalAmountByVolumeChange 을 호출하기 전에
		// n 값을 새로 설정해야 한다.
		// 표준체형으로의 변환인 경우, n 값을 원래대로 되돌린다.
		if (__simulation.getTypeRef() == SIMULATION_TYPE_STANDARD) {
			// 먼저 n 을 저장한다.
			D3DXVERTEX*	vertexPtr;
			__vertexBufferPtr->Lock(0, 0, (void**) &vertexPtr, 0);
			for (DWORD i = 0; i < __numPointReprs; ++i) {
				D3DXVERTEX&	v = vertexPtr[i];
				v.nTmp = v.n;
			}
			__vertexBufferPtr->Unlock();
		}

		// 주어진 Volume 변화를 가져오려면 각 Vertex 를 "최대" 얼마나 움직여야 하나?
		float	fNormalAmount;	// 실제로는 각 Vertex 의 색깔에 따라 상대적으로 움직인다.
		unsigned	max_iter = 100;	// 최대 100까지 시도한다.
		int	iReturnVal = findNormalAmountByVolumeChange(
			fNormalAmount,
			fNormalAmountGuess,
			volumeChange,
			max_iter,
			0.001f * volumeChange
		);

		// 표준체형으로의 변환인 경우, n 값을 원래대로 되돌린다.
		if (__simulation.getTypeRef() == SIMULATION_TYPE_STANDARD) {
			D3DXVERTEX*	vertexPtr;
			__vertexBufferPtr->Lock(0, 0, (void**) &vertexPtr, 0);
			for (DWORD i = 0; i < __numPointReprs; ++i) {
				D3DXVERTEX&	v = vertexPtr[i];
				v.n = v.nTmp;
			}
			__vertexBufferPtr->Unlock();
		}

		if (iReturnVal != 1)
			return -1;

		// 실제로 Vertex 를 움직인다.
		ChangeMesh(fNormalAmount, 3);
//		SIMULATION_CC_TYPE	ccType = SIMULATION_CC_TYPE_VOLUME_CHANGE;
//		인 경우, kMesh::__fVolumeChange 와 시뮬레이션 전에 저장된 값 kSimulation::__fVolumeChange0 의 차이로
//		kSimulation::__volume_change_cc 를 설정한다.
		__simulation.getVolumeChangeCcRef() = __fVolumeChange - __simulation.getVolumeChange0Ref();
	} else if (
#if 1
		__simulation.getCcTypeRef() == SIMULATION_CC_TYPE_VOLUME_CHANGE &&
		__simulation.getStepTypeRef() == SIMULATION_STEP_TYPE_RATIO &&
		(execType == SIMULATION_EXEC_PLUS || execType == SIMULATION_EXEC_MINUS)
#else
		execType == SIMULATION_EXEC_PLUS ||
		execType == SIMULATION_EXEC_MINUS
#endif
	) {
		float	normal_amount_step = __simulation.getNormalAmountStepRef();
		ChangeMesh(execType == SIMULATION_EXEC_PLUS ? normal_amount_step : -normal_amount_step, 3);
//		SIMULATION_CC_TYPE	ccType = SIMULATION_CC_TYPE_VOLUME_CHANGE;
//		인 경우, kMesh::__fVolumeChange 와 시뮬레이션 전에 저장된 값 kSimulation::__fVolumeChange0 의 차이로
//		kSimulation::__volume_change_cc 를 설정한다.
		__simulation.getVolumeChangeCcRef() = __fVolumeChange - __simulation.getVolumeChange0Ref();
	} else {
		(void) fprintf(stderr, "kMesh::__execSimulation: execType = %d: Invalid\n", execType);
		return -1;
	}

	return 0;
}
#endif

#if 1	// 조작툴(manipulator)
#if 1	// normal manipulator
void	kMesh::SetNormalManipulator(const INTERSECTION& intersection, const D3DXVECTOR3& nDir)
{
	// kNormalManipulator::P(화살표 시작점), kNormalManipulator::N 설정(화살표 방향)
	DWORD	faceID = intersection.dwFace;
	if (faceID >= __numFaces || !__facePtr) {
		(void) fprintf(stderr, "FATAL: faceID = %d (__facePtr = 0x%p: __numFaces = %d)\n", faceID, __facePtr, __facePtr ? __numFaces : -1);
		return;
	}
	const CFace&	face = __facePtr[faceID];
	D3DXVECTOR3	V[3];
	for (int i = 0; i < 3; ++i) {
		if (face.prID[i] >= __numPointReprs || !__pointReprPtr) {
			(void) fprintf(stderr, "FATAL: prID = %d (__pointReprPtr = 0x%p: __numPointReprs = %d)\n", face.prID[i], __pointReprPtr, __pointReprPtr ? __numPointReprs : -1);
			return;
		}
		V[i] = __pointReprPtr[face.prID[i]].v.position;
	}
	// V1 + U * (V2 - V1) + V * (V3 - V1)

	__manipulatorType = NORMAL_MANIPULATOR;
	__manipulatorPtr = &__normal_manipulator;
	D3DXVECTOR3	P0 = V[0] + intersection.fBary1 * (V[1] - V[0]) + intersection.fBary2 * (V[2] - V[0]);
	// nDir 을 평면의 법선벡터로, xDir 을 V1 에서 V2 에 이르는 벡터로 설정
	D3DXVECTOR3	V12 = V[1] - V[0];
	D3DXVECTOR3	V13 = V[2] - V[0];
	D3DXVECTOR3	zDir;	// zDir = V12 x V13
	float	radius = .1f * __radius;
	__normal_manipulator.set(radius, MANIPULATOR_NORMAL_ANGLE_ROTATION_BOTH_THETA_AND_PHI, P0, *D3DXVec3Cross(&zDir, &V12, &V13), V12);
}

int	kMesh::__setNormalManipulator(int n)
{
	// ArcBall 반경
	float	radius = .1f * __radius;	// 오브젝트공간(kObject) 에서의 ArcBall 반경
#if 0
	D3DVIEWPORT9	viewport;
	DXUTGetD3D9Device()->GetViewport(&viewport);

	D3DVIEWPORT9	viewport;
	__devicePtr->GetViewport(&viewport);
	D3DXVECTOR3	o = D3DXVECTOR3(50, (FLOAT) viewport.Height - 50, 0.5);
	D3DXVECTOR3	O;	// Object 공간에서의 좌표
	D3DXVec3Unproject(&O, &o, &viewport, &__projectionMatrix, &__viewMatrix, &__worldMatrix);
	// Screen 공간에서 30픽셀 차이는 오브젝트 공간에서는 얼마?
	D3DXVECTOR3	o30 = D3DXVECTOR3(50, (FLOAT) viewport.Height - 20, 0.5);
	D3DXVECTOR3	O30;	// Object 공간에서의 좌표
	D3DXVec3Unproject(&O30, &o30, &viewport, &__projectionMatrix, &__viewMatrix, &__worldMatrix);
	FLOAT	axisLength = D3DXVec3Length(&D3DXVECTOR3(O - O30));
#endif

	if (n < 0) {
		(void) fprintf(stderr, "ERROR: kMesh::__setNormalManipulator: n = %d: Invalid\n", n);
		return -1;
	}
	if (n == 0) {	// 임의 둘레측정
		if (!__statePtr->g__bString && !__statePtr->g__incompleteString) {
			(void) fprintf(stderr, "ERROR: kMesh::__setNormalManipulator: n = 0: No Measure Mode\n");
			return -1;
		}
		// 둘레측정을 한 적이 있는가?
		if (__statePtr->g__numSegments == 0) {
			(void) fprintf(stderr, "WARNING: kMesh::__setNormalManipulator: n = 0: kState::g__numSegments = 0: No Measure Done\n");
			return -1;
		}
		if (__statePtr->g__numSegments == 1) {
			(void) fprintf(stderr, "WARNING: kMesh::__setNormalManipulator: n = 0: kState::g__numSegments = 1: No Need of Manipulator\n");
			return -1;
		}

		static START_DEF_PTR	startDefPtr;
		// P0, nDir, xDir 결정
		D3DXVECTOR3	P0;
		D3DXVECTOR3	nDir;
		D3DXVECTOR3	xDir;
		if (__statePtr->g__bString) {	// 둘레측정
			startDefPtr.type = START_DEF_PTR_TYPE__GIRTH;
			PLANE_DEF&	planeDef = __tapeMeasure.startDef().plane_def;
			getPoint(planeDef.pointDef, P0);
			nDir = planeDef.normal;
			SEGMENT_VERTEX*	pointPtr;
			__segmentVertexBuffer->Lock(0, 0, (void**) &pointPtr, 0);
			xDir = pointPtr[1].P - pointPtr[0].P;
			__segmentVertexBuffer->Unlock();
			startDefPtr.plane_def_ptr = &planeDef;
		} else {	// 두 점 사이의 거리
			startDefPtr.type = START_DEF_PTR_TYPE__TWO_POINT;
			TWO_POINT_PLANE_DEF&	_2pDef = __tapeMeasure.startDef().two_point_plane_def;
			D3DXVECTOR3	P1;
			D3DXVECTOR3	P2;
			getPoint(_2pDef.point0Def, P1);
			getPoint(_2pDef.point1Def, P2);
			P0 = .5f * (P1 + P2);
			nDir = _2pDef.normal;
			xDir = P2 - P1;
			startDefPtr.two_point_plane_def_ptr = &_2pDef;
		}
		__normal_manipulator.set(
			radius,

			__statePtr->g__bString ?
				MANIPULATOR_NORMAL_ANGLE_ROTATION_BOTH_THETA_AND_PHI
			:
				MANIPULATOR_NORMAL_ANGLE_ROTATION_THETA_ONLY,

			P0,
			nDir,
			xDir,

			normal_manipulator_done_callback_manual_measure,
				this,
				0,
				&startDefPtr
		);

		return 0;
	}

	// 이미 정의된 둘레측정
	// 1. 화면 상에 표시되어 있어야 한다.
	int	val = __tapeMeasure.getValRef();
	if (!(val & n)) {
		(void) fprintf(stderr, "ERROR: kMesh::__setNormalManipulator: n = %d: Not Shown\n", n);
		return -1;
	}

#if 1	// kMesh::setMeasure 와 비슷
	// 2. 해당하는 정의가 있어야 한다.
	TAPE_MEASURE_TYPE_T	type = (TAPE_MEASURE_TYPE_T) n;
	MEASURE_PTR_HASH::const_iterator	it;
	it = __measure_ptr_dic.find(type);
	if (it == __measure_ptr_dic.end()) {
		(void) fprintf(stderr, "ERROR: kMesh::__setNormalManipulator: Type = %d: Not implemented yet\n", type);
		return -1;
	}

	// 3. 전체든 아니면 일부든 둘레측정에 해당해야 한다.
	// 그것도 하나의 둘레측정에 의해서 결정되어야 한다;
	// TO DO LIST: 여러 개의 둘레측정에 의해서 결정되면 각 둘레측정을 변경할 수도 있어야 한다.
	const MEASURE&	measure = *it->second;
	const std::vector<MEASURE_ITEM*>&	measure_item_ptr_list = measure.measure_item_ptr_list;
	size_t	nDef = measure_item_ptr_list.size();
	if (nDef == 0) {
		(void) fprintf(stderr, "FATAL: kMesh::__setNormalManipulator: Type = %d: No element\n", type);
		return -1;
	}
	if (nDef > 1) {
		(void) fprintf(stderr, "WARNING: kMesh::__setNormalManipulator: Type = %d: Two elements or more: Not implemented yet\n", type);
		return -1;
	}
#endif	// kMesh::setMeasure 와 비슷

#if 1	// kMesh::setMeasureItem 과 비슷
	MEASURE_ITEM&	m_item = *measure_item_ptr_list.at(0);
	if (!m_item.__ptr) {
		(void) fprintf(stderr, "ERROR: kMesh::__setNormalManipulator: Type = %d: m_item.__ptr is nil: Obsolete Definition\n", type);
		return -1;
	}
	if (m_item.__type != MEASURE_TYPE__GIRTH && m_item.__type != MEASURE_TYPE__TWO_POINT_GIRTH) {
		(void) fprintf(stderr, "ERROR: kMesh::__setNormalManipulator: Type = %d: GIRTH Only\n", type);
		return -1;
	}
	if (m_item.numSegments == 1) {
		(void) fprintf(stderr, "WARNING: kMesh::__setNormalManipulator: Type = %d: numSegments = 1: No Need of Manipulator\n", type);
		return -1;
	}
#endif	// kMesh::setMeasureItem 과 비슷

	// manipulator 설정
	// P0, nDir, xDir 결정
	D3DXVECTOR3	P0;
	D3DXVECTOR3	nDir;
	D3DXVECTOR3	xDir;
	if (m_item.__type == MEASURE_TYPE__GIRTH) {
		MEASURE_ITEM_PTR_DEF__GIRTH&	def = *((MEASURE_ITEM_PTR_DEF__GIRTH*) m_item.__ptr);
		PLANE_DEF&	planeDef = def.__planeDef;

		getPoint(planeDef.pointDef, P0);
		nDir = planeDef.normal;
		SEGMENT_VERTEX*	pointPtr;
		m_item.segmentVertexBuffer->Lock(0, 0, (void**) &pointPtr, 0);
		xDir = pointPtr[1].P - pointPtr[0].P;
		m_item.segmentVertexBuffer->Unlock();
	} else {
		MEASURE_ITEM_PTR_DEF__TWO_POINT_GIRTH&	def = *((MEASURE_ITEM_PTR_DEF__TWO_POINT_GIRTH*) m_item.__ptr);
		TWO_POINT_PLANE_DEF&	_2pDef = def.__2pDef;

		D3DXVECTOR3	P1;
		D3DXVECTOR3	P2;
		getPoint(_2pDef.point0Def, P1);
		getPoint(_2pDef.point1Def, P2);
		P0 = .5f * (P1 + P2);
		nDir = _2pDef.normal;
		xDir = P2 - P1;
	}
	__normal_manipulator.set(
		radius,

		m_item.__type == MEASURE_TYPE__GIRTH ?
			MANIPULATOR_NORMAL_ANGLE_ROTATION_BOTH_THETA_AND_PHI
		:
			MANIPULATOR_NORMAL_ANGLE_ROTATION_THETA_ONLY,

		P0,
		nDir,
		xDir,

		normal_manipulator_done_callback_defined_measure,
			this,
			n,
			0
	);
	return 0;
}
#endif

#if 1	// point manipulator
int	kMesh::__setPointManipulator(int n)
{
	if (n < 0) {
		(void) fprintf(stderr, "ERROR: kMesh::__setPointManipulator: n = %d: Invalid\n", n);
		return -1;
	}
	if (n == 0) {	// 임의 둘레측정
		if (!__statePtr->g__bString && !__statePtr->g__incompleteString) {
			(void) fprintf(stderr, "ERROR: kMesh::__setPointManipulator: n = 0: No Measure Mode\n");
			return -1;
		}
		// 둘레측정을 한 적이 있는가?
		if (__statePtr->g__numSegments == 0) {
			(void) fprintf(stderr, "WARNING: kMesh::__setPointManipulator: n = 0: kState::g__numSegments = 0: No Measure Done\n");
			return -1;
		}

		static START_DEF_PTR	startDefPtr;
		if (__statePtr->g__bString) {	// 둘레측정
			startDefPtr.type = START_DEF_PTR_TYPE__GIRTH;
			startDefPtr.plane_def_ptr = &__tapeMeasure.startDef().plane_def;
		} else {	// 두 점 사이의 거리
			startDefPtr.type = START_DEF_PTR_TYPE__TWO_POINT;
			startDefPtr.two_point_plane_def_ptr = &__tapeMeasure.startDef().two_point_plane_def;
		}
		__point_manipulator.set(this, 0, &startDefPtr, point_manipulator_done_callback_manual_measure);

		return 0;
	}

	// 이미 정의된 둘레측정
	// 1. 화면 상에 표시되어 있어야 한다.
	int	val = __tapeMeasure.getValRef();
	if (!(val & n)) {
		(void) fprintf(stderr, "ERROR: kMesh::__setPointManipulator: n = %d: Not Shown\n", n);
		return -1;
	}

#if 1	// kMesh::setMeasure 와 비슷
	// 2. 해당하는 정의가 있어야 한다.
	TAPE_MEASURE_TYPE_T	type = (TAPE_MEASURE_TYPE_T) n;
	MEASURE_PTR_HASH::const_iterator	it;
	it = __measure_ptr_dic.find(type);
	if (it == __measure_ptr_dic.end()) {
		(void) fprintf(stderr, "ERROR: kMesh::__setPointManipulator: Type = %d: Not implemented yet\n", type);
		return -1;
	}

	// 3. 전체든 아니면 일부든 둘레측정에 해당해야 한다.
	// 그것도 하나의 둘레측정에 의해서 결정되어야 한다;
	// TO DO LIST: 여러 개의 둘레측정에 의해서 결정되면 각 둘레측정을 변경할 수도 있어야 한다.
	const MEASURE&	measure = *it->second;
	const std::vector<MEASURE_ITEM*>&	measure_item_ptr_list = measure.measure_item_ptr_list;
	size_t	nDef = measure_item_ptr_list.size();
	if (nDef == 0) {
		(void) fprintf(stderr, "FATAL: kMesh::__setPointManipulator: Type = %d: No element\n", type);
		return -1;
	}
	if (nDef > 1) {
		(void) fprintf(stderr, "WARNING: kMesh::__setPointManipulator: Type = %d: Two elements or more: Not implemented yet\n", type);
		return -1;
	}
#endif	// kMesh::setMeasure 와 비슷

#if 1	// kMesh::setMeasureItem 과 비슷
	MEASURE_ITEM&	m_item = *measure_item_ptr_list.at(0);
	if (!m_item.__ptr) {
		(void) fprintf(stderr, "ERROR: kMesh::__setPointManipulator: Type = %d: m_item.__ptr is nil: Obsolete Definition\n", type);
		return -1;
	}
	if (m_item.__type != MEASURE_TYPE__GIRTH && m_item.__type != MEASURE_TYPE__TWO_POINT_GIRTH) {
		(void) fprintf(stderr, "ERROR: kMesh::__setPointManipulator: Type = %d: GIRTH Only\n", type);
		return -1;
	}
	if (m_item.numSegments == 1) {
		(void) fprintf(stderr, "WARNING: kMesh::__setPointManipulator: Type = %d: numSegments = 1: No Need of Manipulator\n", type);
		return -1;
	}
#endif	// kMesh::setMeasureItem 과 비슷

	// manipulator 설정
	static START_DEF_PTR	startDefPtr;
	if (m_item.__type == MEASURE_TYPE__GIRTH) {
		startDefPtr.type = START_DEF_PTR_TYPE__GIRTH;
		MEASURE_ITEM_PTR_DEF__GIRTH&	def = *((MEASURE_ITEM_PTR_DEF__GIRTH*) m_item.__ptr);
		startDefPtr.plane_def_ptr = &def.__planeDef;
	} else if (m_item.__type == MEASURE_TYPE__TWO_POINT_GIRTH) {
		MEASURE_ITEM_PTR_DEF__TWO_POINT_GIRTH&	def = *((MEASURE_ITEM_PTR_DEF__TWO_POINT_GIRTH*) m_item.__ptr);
		startDefPtr.type = START_DEF_PTR_TYPE__TWO_POINT;
		startDefPtr.two_point_plane_def_ptr = &def.__2pDef;
	} else
		assert(0);
	__point_manipulator.set(this, n, &startDefPtr, point_manipulator_done_callback_defined_measure);
	return 0;
}

void	kMesh::SetPointManipulator(const INTERSECTION& intersection)
{
	__manipulatorType = POINT_MANIPULATOR;
	__manipulatorPtr = &__point_manipulator;

	static START_DEF_PTR	startDefPtr;
	startDefPtr.type = START_DEF_PTR_TYPE__GIRTH;
	PLANE_DEF&	planeDef = __tapeMeasure.startDef().plane_def;
	planeDef.pointDef = intersection;
	startDefPtr.plane_def_ptr = &planeDef;
	__point_manipulator.set(this, 0, &startDefPtr);
}
#endif	// point manipulator

#if 1	// line manipulator
int	kMesh::__setLineManipulator(int n)
{
	if (n < 0) {
		(void) fprintf(stderr, "ERROR: kMesh::__setLineManipulator: n = %d: Invalid\n", n);
		return -1;
	}
	if (n == 0) {	// 임의 둘레측정
		if (!__statePtr->g__bString && !__statePtr->g__incompleteString) {
			(void) fprintf(stderr, "ERROR: kMesh::__setLineManipulator: n = 0: No Measure Mode\n");
			return -1;
		}
		// 둘레측정을 한 적이 있는가?
		if (__statePtr->g__numSegments == 0) {
			(void) fprintf(stderr, "WARNING: kMesh::__setLineManipulator: n = 0: kState::g__numSegments = 0: No Measure Done\n");
			return -1;
		}

		LPDIRECT3DVERTEXBUFFER9	segmentVertexBufferPtr = 0;
		unsigned	numSegments = __statePtr->g__numSegments;
		bool	closed = __statePtr->g__stringClosed;
		if (__statePtr->g__bString) {	// 둘레측정
			if (__statePtr->g__mode == TAPE_MEASURE_MODE_WRAP) {	// 줄자
				segmentVertexBufferPtr = __tapeMeasure.convexVertexBufferPtr();
				numSegments = __tapeMeasure.convexPointArray().GetSize();
				if (!closed)
					--numSegments;
			} else {
				segmentVertexBufferPtr = __segmentVertexBuffer;
			}
		} else {	// 두 점 사이의 거리
			if (__statePtr->g__mode == TAPE_MEASURE_MODE_WRAP) {	// 줄자
				segmentVertexBufferPtr = __tapeMeasure.convexVertexBufferPtr();
				numSegments = __tapeMeasure.convexPointArray().GetSize() - 1;
			} else {
				segmentVertexBufferPtr = __tapeMeasure.twoPointVertexBufferPtr();
			}
		}
		assert(segmentVertexBufferPtr);
		SEGMENT_VERTEX*	vertexPtr;
		segmentVertexBufferPtr->Lock(0, 0, (void**) &vertexPtr, 0);
		__line_manipulator.set(this, 0, __statePtr->g__mode, __segmentMethod, closed, closed ? numSegments : numSegments + 1, vertexPtr, 0);
		segmentVertexBufferPtr->Unlock();

		return 0;
	}

	// 이미 정의된 둘레측정
	// 1. 화면 상에 표시되어 있어야 한다.
	int	val = __tapeMeasure.getValRef();
	if (!(val & n)) {
		(void) fprintf(stderr, "ERROR: kMesh::__setLineManipulator: n = %d: Not Shown\n", n);
		return -1;
	}

#if 1	// kMesh::setMeasure 와 비슷
	// 2. 해당하는 정의가 있어야 한다.
	TAPE_MEASURE_TYPE_T	type = (TAPE_MEASURE_TYPE_T) n;
	MEASURE_PTR_HASH::const_iterator	it;
	it = __measure_ptr_dic.find(type);
	if (it == __measure_ptr_dic.end()) {
		(void) fprintf(stderr, "ERROR: kMesh::__setLineManipulator: Type = %d: Not implemented yet\n", type);
		return -1;
	}

	// 3. 전체든 아니면 일부든 둘레측정에 해당해야 한다.
	// 그것도 하나의 둘레측정에 의해서 결정되어야 한다;
	// TO DO LIST: 여러 개의 둘레측정에 의해서 결정되면 각 둘레측정을 변경할 수도 있어야 한다.
	const MEASURE&	measure = *it->second;
	const std::vector<MEASURE_ITEM*>&	measure_item_ptr_list = measure.measure_item_ptr_list;
	size_t	nDef = measure_item_ptr_list.size();
	if (nDef == 0) {
		(void) fprintf(stderr, "FATAL: kMesh::__setLineManipulator: Type = %d: No element\n", type);
		return -1;
	}
	if (nDef > 1) {
		(void) fprintf(stderr, "WARNING: kMesh::__setLineManipulator: Type = %d: Two elements or more: Not implemented yet\n", type);
		return -1;
	}
#endif	// kMesh::setMeasure 와 비슷

#if 1	// kMesh::setMeasureItem 과 비슷
	MEASURE_ITEM&	m_item = *measure_item_ptr_list.at(0);
	if (!m_item.__ptr) {
		(void) fprintf(stderr, "ERROR: kMesh::__setLineManipulator: Type = %d: m_item.__ptr is nil: Obsolete Definition\n", type);
		return -1;
	}
	if (m_item.__type != MEASURE_TYPE__GIRTH && m_item.__type != MEASURE_TYPE__TWO_POINT_GIRTH) {
		(void) fprintf(stderr, "ERROR: kMesh::__setLineManipulator: Type = %d: GIRTH Only\n", type);
		return -1;
	}
	if (m_item.numSegments == 1) {
		(void) fprintf(stderr, "WARNING: kMesh::__setLineManipulator: Type = %d: numSegments = 1: No Need of Manipulator\n", type);
		return -1;
	}
#endif	// kMesh::setMeasureItem 과 비슷

	// manipulator 설정
	TAPE_MEASURE_MODE_T	mode;
	if (m_item.__type == MEASURE_TYPE__GIRTH) {
		MEASURE_ITEM_PTR_DEF__GIRTH&	def = *((MEASURE_ITEM_PTR_DEF__GIRTH*) m_item.__ptr);
		mode = def.__mode;
	} else {
		MEASURE_ITEM_PTR_DEF__TWO_POINT_GIRTH&	def = *((MEASURE_ITEM_PTR_DEF__TWO_POINT_GIRTH*) m_item.__ptr);
		mode = def.__mode;
	}
	SEGMENT_VERTEX*	vertexPtr;
	m_item.segmentVertexBuffer->Lock(0, 0, (void**) &vertexPtr, 0);
	__line_manipulator.set(this, n, mode, __segmentMethod, m_item.closed, m_item.closed ? m_item.numSegments : m_item.numSegments + 1, vertexPtr, &m_item);
	m_item.segmentVertexBuffer->Unlock();

	return 0;
}
#endif

void	kMesh::RenderManipulator() const
{
	if (!__manipulatorPtr)
		return;

#if 1	// line manipulator
	if (__manipulatorType == LINE_MANIPULATOR) {
		if (!__line_manipulator.isSet())
			return;

		ShowString(__line_manipulator.segmentVertexBufferPtr(), __line_manipulator.segmentIndexBufferPtr(), __line_manipulator.numSegmentsInSegmentBuffer(), D3DXVECTOR4(0, 1, 1, 1));

#if 0
		ShowVertex(
			__line_manipulator.segmentVertexBufferPtr(),
			OnePointDeclarationPtr(),
			sizeof(SEGMENT_VERTEX),	// stride
			__line_manipulator.numPoints(),
			5.f,
			D3DXVECTOR4(1, 1, 0, 1)
		);
#endif

		int	indexPicked = __line_manipulator.indexPicked();
		if (indexPicked != -1) {
			ShowString(__line_manipulator.segmentVertexBufferPtr(), __line_manipulator.segmentIndexBufferPtr(), 1, D3DXVECTOR4(1, 0, 0, 1), 2 * indexPicked);
		
			LPDIRECT3DVERTEXBUFFER9		pointVertexBuffer = 0;
		do {
			HRESULT hResult = __devicePtr->CreateVertexBuffer(
				2 * sizeof(D3DXVECTOR3),
				D3DUSAGE_WRITEONLY,
				0,
				D3DPOOL_MANAGED,
				&pointVertexBuffer,
				NULL
			);
#ifdef _DEBUG
			assert(hResult == D3D_OK);
#else
			if (hResult != D3D_OK)
				throw "kMesh::RenderManipulator: CreateVertexBuffer: Failure";
#endif

			D3DXVECTOR3*	pointPtr = NULL;
			pointVertexBuffer->Lock(0, 0, (void**) &pointPtr, 0);
			pointPtr[0] = __line_manipulator.pickedPoint();
			if (__line_manipulator.isPointDefSet())
				getPoint(__line_manipulator.planeDef().pointDef, pointPtr[1]);
			pointVertexBuffer->Unlock();

			// 렌더링
			ShowVertex(
				pointVertexBuffer, __OnePointDeclarationPtr, sizeof(D3DXVECTOR3),
				1, 9.f, D3DXVECTOR4(0, 0, 1, 1),
				0,
				-1.e-0f	// zDelta
			);
			if (__line_manipulator.isPointDefSet())
				ShowVertex(
					pointVertexBuffer, __OnePointDeclarationPtr, sizeof(D3DXVECTOR3),
					1, 9.f, __line_manipulator.isOutOfRange() ? D3DXVECTOR4(1, 0, 0, 1) : D3DXVECTOR4(0, 1, 0, 1),
					1,
					-1.e-0f	// zDelta
				);
		} while (0);
			SAFE_RELEASE(pointVertexBuffer);
		}

		return;
	}
#endif	// line manipulator

#if 1	// point manipulator
	if (__manipulatorType == POINT_MANIPULATOR) {
		if (!__point_manipulator.isSet())
			return;

		LPDIRECT3DVERTEXBUFFER9		pointManipulatorVertexBuffer = 0;
		do {
			const START_DEF_PTR&	startDefPtr = __point_manipulator.startDefPtr();

			// 최대 세 점을 담을 Vertex Buffer 설정;
			// startDefPtr.type = START_DEF_PTR_TYPE__GIRTH 라면 두 점, START_DEF_PTR_TYPE__TWO_POINT 라면 세 점
			HRESULT hResult = __devicePtr->CreateVertexBuffer(
				(startDefPtr.type == START_DEF_PTR_TYPE__GIRTH ? 2 : 3) * sizeof(D3DXVECTOR3),
				D3DUSAGE_WRITEONLY,
				0,
				D3DPOOL_MANAGED,
				&pointManipulatorVertexBuffer,
				NULL
			);
#ifdef _DEBUG
			assert(hResult == D3D_OK);
#else
			if (hResult != D3D_OK)
				throw "kMesh::RenderManipulator: CreateVertexBuffer: Failure";
#endif

			// 설정
			D3DXVECTOR3*	pointManipulatorPointPtr = NULL;
			pointManipulatorVertexBuffer->Lock(0, 0, (void**) &pointManipulatorPointPtr, 0);
			if (startDefPtr.type == START_DEF_PTR_TYPE__GIRTH) {
				getPoint(startDefPtr.plane_def_ptr->pointDef, pointManipulatorPointPtr[0]);
			} else if (startDefPtr.type == START_DEF_PTR_TYPE__TWO_POINT) {
				getPoint(startDefPtr.two_point_plane_def_ptr->point0Def, pointManipulatorPointPtr[0]);
				getPoint(startDefPtr.two_point_plane_def_ptr->point1Def, pointManipulatorPointPtr[1]);
			} else
				assert(0);
			if (__point_manipulator.indexPicked() != -1) {
				getPoint(__point_manipulator.pointDef(), pointManipulatorPointPtr[startDefPtr.type == START_DEF_PTR_TYPE__GIRTH ? 1 : 2]);
			}
			pointManipulatorVertexBuffer->Unlock();

			// 렌더링
			ShowVertex(
				pointManipulatorVertexBuffer, __OnePointDeclarationPtr, sizeof(D3DXVECTOR3),
				startDefPtr.type == START_DEF_PTR_TYPE__GIRTH ? 1 : 2, 9.f, D3DXVECTOR4(1, 1, 1, 1),
				0,
				-1.e-0f	// zDelta
			);
			if (__point_manipulator.indexPicked() != -1) {
				ShowVertex(
					pointManipulatorVertexBuffer, __OnePointDeclarationPtr, sizeof(D3DXVECTOR3),
					1, 9.f, D3DXVECTOR4(1, 0, 1, 1),
					startDefPtr.type == START_DEF_PTR_TYPE__GIRTH ? 1 : 2,
					-1.e-0f	// zDelta
				);
			}
		} while (0);
		SAFE_RELEASE(pointManipulatorVertexBuffer);

		return;
	}
#endif	// point manipulator

	// NORMAL_MANIPULATOR
	if (!__normal_manipulator.isSet())
		return;

	D3DXVECTOR3	nDir;
	__normal_manipulator.GetNew_nDir(nDir);
	D3DXVECTOR3	xDir;
	__normal_manipulator.GetNew_xDir(xDir);
	D3DXVECTOR3	yDir;
	__normal_manipulator.GetNew_yDir(yDir);
	float	radius = .2f * __radius;


	LPDIRECT3DVERTEXBUFFER9		manipulatorVertexBuffer = 0;
	LPDIRECT3DINDEXBUFFER9		manipulatorIndexBuffer = 0;
do {
	// 네 점(중심점, 각 축의 끝점)을 담을 Vertex Buffer 설정
	HRESULT hResult = __devicePtr->CreateVertexBuffer(
		4 * sizeof(D3DXVECTOR3),
		D3DUSAGE_WRITEONLY,
		0,
		D3DPOOL_MANAGED,
		&manipulatorVertexBuffer,
		NULL
	);
	assert(hResult == D3D_OK);
	// 두 점을 채운다.
	D3DXVECTOR3*	manipulatorPointPtr = NULL;
	manipulatorVertexBuffer->Lock(0, 0, (void**) &manipulatorPointPtr, 0);
	manipulatorPointPtr[0] = __normal_manipulator.P0();
	manipulatorPointPtr[1] = __normal_manipulator.P0() + radius * xDir;
	manipulatorPointPtr[2] = __normal_manipulator.P0() + radius * yDir;
	manipulatorPointPtr[3] = __normal_manipulator.P0() + radius * nDir;
	manipulatorVertexBuffer->Unlock();
	// 각 축을 말하는 세 선을 담을 Index Buffer
	hResult = __devicePtr->CreateIndexBuffer(
		2 * 3 * sizeof(DWORD),
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX32,
		D3DPOOL_MANAGED,
		&manipulatorIndexBuffer,
		NULL
	);
	assert(D3D_OK == hResult);
	DWORD*		manipulatorLineIndicesPtr = 0;
	hResult = manipulatorIndexBuffer->Lock(0, 0, (void**) &manipulatorLineIndicesPtr, 0);
	// x축
	manipulatorLineIndicesPtr[0] = 0;
	manipulatorLineIndicesPtr[1] = 1;
	// y축
	manipulatorLineIndicesPtr[2] = 0;
	manipulatorLineIndicesPtr[3] = 2;
	// z축
	manipulatorLineIndicesPtr[4] = 0;
	manipulatorLineIndicesPtr[5] = 3;
	assert(D3D_OK == hResult);
	manipulatorIndexBuffer->Unlock();


	// 1. 시작점과 끝점을 서로 다른 크기와 색으로 표시; 시작점(5), 끝점(10)
	// 시작점; 크기(5), 색깔(흰색)
#if 1
{
	__effectPtr->SetInt("g__FillMode", D3DFILL_POINT);
	__effectPtr->SetMatrix("g_mWorldViewProjection", &__worldViewProjectionMatrix);
	__effectPtr->SetMatrix("g_mWorld", &__worldMatrix);
	__effectPtr->SetFloat("pointSize", 5.f);
	__effectPtr->SetVector("pointColor", &D3DXVECTOR4(1, 1, 1, 1));
	D3DXHANDLE	hTechnique = __effectPtr->GetTechnique(0);
	assert(hTechnique);
	hResult = __effectPtr->SetTechnique(hTechnique);
	assert(S_OK == hResult);
	UINT	NumberOfPasses;
	hResult = __effectPtr->Begin(&NumberOfPasses, 0);
	assert(D3D_OK == hResult);
	for (UINT iPass = 0; iPass < NumberOfPasses; ++iPass) {
		hResult = __effectPtr->BeginPass(iPass);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetVertexDeclaration(__OnePointDeclarationPtr);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetStreamSource(0, manipulatorVertexBuffer, 0, sizeof(D3DXVECTOR3));
		assert(D3D_OK == hResult);
		hResult = __devicePtr->DrawPrimitive(D3DPT_POINTLIST, 0, 1);
		assert(D3D_OK == hResult);
		hResult = __effectPtr->EndPass();
		assert(S_OK == hResult);
	}
	hResult = __effectPtr->End();
	assert(S_OK == hResult);
}
{
	__effectPtr->SetInt("g__FillMode", D3DFILL_POINT);
	__effectPtr->SetMatrix("g_mWorldViewProjection", &__worldViewProjectionMatrix);
	__effectPtr->SetMatrix("g_mWorld", &__worldMatrix);
	__effectPtr->SetFloat("pointSize", 10.f);
	__effectPtr->SetVector("pointColor", &D3DXVECTOR4(1, 0, 0, 1));
	D3DXHANDLE	hTechnique = __effectPtr->GetTechnique(0);
	assert(hTechnique);
	hResult = __effectPtr->SetTechnique(hTechnique);
	assert(S_OK == hResult);
	UINT	NumberOfPasses;
	hResult = __effectPtr->Begin(&NumberOfPasses, 0);
	assert(D3D_OK == hResult);
	for (UINT iPass = 0; iPass < NumberOfPasses; ++iPass) {
		hResult = __effectPtr->BeginPass(iPass);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetVertexDeclaration(__OnePointDeclarationPtr);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetStreamSource(0, manipulatorVertexBuffer, 0, sizeof(D3DXVECTOR3));
		assert(D3D_OK == hResult);
		hResult = __devicePtr->DrawPrimitive(D3DPT_POINTLIST, 1, 1);
		assert(D3D_OK == hResult);
		hResult = __effectPtr->EndPass();
		assert(S_OK == hResult);
	}
	hResult = __effectPtr->End();
	assert(S_OK == hResult);
}
{
	__effectPtr->SetInt("g__FillMode", D3DFILL_POINT);
	__effectPtr->SetMatrix("g_mWorldViewProjection", &__worldViewProjectionMatrix);
	__effectPtr->SetMatrix("g_mWorld", &__worldMatrix);
	__effectPtr->SetFloat("pointSize", 10.f);
	__effectPtr->SetVector("pointColor", &D3DXVECTOR4(0, 1, 0, 1));
	D3DXHANDLE	hTechnique = __effectPtr->GetTechnique(0);
	assert(hTechnique);
	hResult = __effectPtr->SetTechnique(hTechnique);
	assert(S_OK == hResult);
	UINT	NumberOfPasses;
	hResult = __effectPtr->Begin(&NumberOfPasses, 0);
	assert(D3D_OK == hResult);
	for (UINT iPass = 0; iPass < NumberOfPasses; ++iPass) {
		hResult = __effectPtr->BeginPass(iPass);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetVertexDeclaration(__OnePointDeclarationPtr);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetStreamSource(0, manipulatorVertexBuffer, 0, sizeof(D3DXVECTOR3));
		assert(D3D_OK == hResult);
		hResult = __devicePtr->DrawPrimitive(D3DPT_POINTLIST, 2, 1);
		assert(D3D_OK == hResult);
		hResult = __effectPtr->EndPass();
		assert(S_OK == hResult);
	}
	hResult = __effectPtr->End();
	assert(S_OK == hResult);
}
{
	__effectPtr->SetInt("g__FillMode", D3DFILL_POINT);
	__effectPtr->SetMatrix("g_mWorldViewProjection", &__worldViewProjectionMatrix);
	__effectPtr->SetMatrix("g_mWorld", &__worldMatrix);
	__effectPtr->SetFloat("pointSize", 10.f);
	__effectPtr->SetVector("pointColor", &D3DXVECTOR4(0, 0, 1, 1));
	D3DXHANDLE	hTechnique = __effectPtr->GetTechnique(0);
	assert(hTechnique);
	hResult = __effectPtr->SetTechnique(hTechnique);
	assert(S_OK == hResult);
	UINT	NumberOfPasses;
	hResult = __effectPtr->Begin(&NumberOfPasses, 0);
	assert(D3D_OK == hResult);
	for (UINT iPass = 0; iPass < NumberOfPasses; ++iPass) {
		hResult = __effectPtr->BeginPass(iPass);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetVertexDeclaration(__OnePointDeclarationPtr);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetStreamSource(0, manipulatorVertexBuffer, 0, sizeof(D3DXVECTOR3));
		assert(D3D_OK == hResult);
		hResult = __devicePtr->DrawPrimitive(D3DPT_POINTLIST, 3, 1);
		assert(D3D_OK == hResult);
		hResult = __effectPtr->EndPass();
		assert(S_OK == hResult);
	}
	hResult = __effectPtr->End();
	assert(S_OK == hResult);
}
#endif

	// 2. 시작점에서 끝점까지 가는 선 표시; 색깔(녹색)
#if 1
{
	hResult = __effectPtr->SetInt("g__FillMode", D3DFILL_WIREFRAME);
	assert(D3D_OK == hResult);
	hResult = __effectPtr->SetVector("pointColor", &D3DXVECTOR4(1, 0, 0, 1));
	D3DXHANDLE	hTechnique = __effectPtr->GetTechnique(0);
	assert(hTechnique);
	hResult = __effectPtr->SetTechnique(hTechnique);
	assert(S_OK == hResult);
	UINT	NumberOfPasses;
	hResult = __effectPtr->Begin(&NumberOfPasses, 0);
	assert(D3D_OK == hResult);
	for (UINT iPass = 0; iPass < NumberOfPasses; ++iPass) {
		hResult = __effectPtr->BeginPass(iPass);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetVertexDeclaration(__OnePointDeclarationPtr);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetStreamSource(0, manipulatorVertexBuffer, 0, sizeof(D3DXVECTOR3));
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetIndices(manipulatorIndexBuffer);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->DrawIndexedPrimitive(D3DPT_LINELIST, 0, 0, 4, 0, 1);
		assert(D3D_OK == hResult);
		hResult = __effectPtr->EndPass();
		assert(S_OK == hResult);
	}
	hResult = __effectPtr->End();
	assert(S_OK == hResult);
}
{
	hResult = __effectPtr->SetInt("g__FillMode", D3DFILL_WIREFRAME);
	assert(D3D_OK == hResult);
	hResult = __effectPtr->SetVector("pointColor", &D3DXVECTOR4(0, 1, 0, 1));
	D3DXHANDLE	hTechnique = __effectPtr->GetTechnique(0);
	assert(hTechnique);
	hResult = __effectPtr->SetTechnique(hTechnique);
	assert(S_OK == hResult);
	UINT	NumberOfPasses;
	hResult = __effectPtr->Begin(&NumberOfPasses, 0);
	assert(D3D_OK == hResult);
	for (UINT iPass = 0; iPass < NumberOfPasses; ++iPass) {
		hResult = __effectPtr->BeginPass(iPass);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetVertexDeclaration(__OnePointDeclarationPtr);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetStreamSource(0, manipulatorVertexBuffer, 0, sizeof(D3DXVECTOR3));
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetIndices(manipulatorIndexBuffer);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->DrawIndexedPrimitive(D3DPT_LINELIST, 0, 0, 4, 2, 1);
		assert(D3D_OK == hResult);
		hResult = __effectPtr->EndPass();
		assert(S_OK == hResult);
	}
	hResult = __effectPtr->End();
	assert(S_OK == hResult);
}
{
	hResult = __effectPtr->SetInt("g__FillMode", D3DFILL_WIREFRAME);
	assert(D3D_OK == hResult);
	hResult = __effectPtr->SetVector("pointColor", &D3DXVECTOR4(0, 0, 1, 1));
	D3DXHANDLE	hTechnique = __effectPtr->GetTechnique(0);
	assert(hTechnique);
	hResult = __effectPtr->SetTechnique(hTechnique);
	assert(S_OK == hResult);
	UINT	NumberOfPasses;
	hResult = __effectPtr->Begin(&NumberOfPasses, 0);
	assert(D3D_OK == hResult);
	for (UINT iPass = 0; iPass < NumberOfPasses; ++iPass) {
		hResult = __effectPtr->BeginPass(iPass);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetVertexDeclaration(__OnePointDeclarationPtr);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetStreamSource(0, manipulatorVertexBuffer, 0, sizeof(D3DXVECTOR3));
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetIndices(manipulatorIndexBuffer);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->DrawIndexedPrimitive(D3DPT_LINELIST, 0, 0, 4, 4, 1);
		assert(D3D_OK == hResult);
		hResult = __effectPtr->EndPass();
		assert(S_OK == hResult);
	}
	hResult = __effectPtr->End();
	assert(S_OK == hResult);
}
#endif
} while (0);
	if (manipulatorVertexBuffer) {
		manipulatorVertexBuffer->Release();
		manipulatorVertexBuffer = 0;
	}
	if (manipulatorIndexBuffer) {
		manipulatorIndexBuffer->Release();
		manipulatorIndexBuffer = 0;
	}

	// 3. manipulator 를 감싸는 세 원
#if 1
	LPDIRECT3DVERTEXBUFFER9		lineVertexBuffer = 0;
	LPDIRECT3DINDEXBUFFER9		lineIndexBuffer = 0;
	int	nStep = 5;	// 360 / 4 즉 90 의 약수이어야 한다!
	int	nPoints = 360 / nStep;
do {
	HRESULT hResult = __devicePtr->CreateVertexBuffer(
		nPoints * sizeof(D3DXVECTOR3),
		D3DUSAGE_WRITEONLY,
		0,
		D3DPOOL_MANAGED,
		&lineVertexBuffer,
		NULL
	);
	assert(hResult == D3D_OK);
	hResult = __devicePtr->CreateIndexBuffer(
		2 * nPoints * sizeof(DWORD),
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX32,
		D3DPOOL_MANAGED,
		&lineIndexBuffer,
		NULL
	);
	assert(D3D_OK == hResult);

{
	D3DXVECTOR3*	linePointPtr = NULL;
	hResult = lineVertexBuffer->Lock(0, 0, (void**) &linePointPtr, 0);
	DWORD*		lineIndicesPtr = 0;
	hResult = lineIndexBuffer->Lock(0, 0, (void**) &lineIndicesPtr, 0);
	assert(D3D_OK == hResult);
	int	index = 0;
	D3DXVECTOR3	nDir0 = nDir;
	D3DXVECTOR3	v0 = radius * xDir;
	for (int n = 0; n < nPoints; ++n, ++index) {
		float	angle = nStep * n * D3DX_PI / 180.f;
		// nDir 축에 대해 xDir 벡터를 angle 만큼 회전시킨 벡터를 얻는다.
		D3DXMATRIX	m;	// 회전에 해당하는 행렬
		D3DXMatrixRotationAxis(&m, &nDir0, angle);
		D3DXVECTOR3	v;
		D3DXVec3TransformCoord(&v, &v0, &m);
		linePointPtr[index] = __normal_manipulator.P0() + v;
		lineIndicesPtr[2 * index + 0] = index;
		lineIndicesPtr[2 * index + 1] = index == nPoints - 1 ? 0 : index + 1;
	}
	hResult = lineVertexBuffer->Unlock();
	assert(D3D_OK == hResult);
	lineIndexBuffer->Unlock();

	hResult = __effectPtr->SetInt("g__FillMode", D3DFILL_WIREFRAME);
	assert(D3D_OK == hResult);
	hResult = __effectPtr->SetVector("pointColor", &D3DXVECTOR4(0, 0, 1, 1));
	D3DXHANDLE	hTechnique = __effectPtr->GetTechnique(0);
	assert(hTechnique);
	hResult = __effectPtr->SetTechnique(hTechnique);
	assert(S_OK == hResult);
	UINT	NumberOfPasses;
	hResult = __effectPtr->Begin(&NumberOfPasses, 0);
	assert(D3D_OK == hResult);
	for (UINT iPass = 0; iPass < NumberOfPasses; ++iPass) {
		hResult = __effectPtr->BeginPass(iPass);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetVertexDeclaration(__OnePointDeclarationPtr);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetStreamSource(0, lineVertexBuffer, 0, sizeof(D3DXVECTOR3));
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetIndices(lineIndexBuffer);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->DrawIndexedPrimitive(D3DPT_LINELIST, 0, 0, nPoints, 0, nPoints);
		assert(D3D_OK == hResult);
		hResult = __effectPtr->EndPass();
		assert(S_OK == hResult);
	}
	hResult = __effectPtr->End();
	assert(S_OK == hResult);
}
{
	D3DXVECTOR3*	linePointPtr = NULL;
	hResult = lineVertexBuffer->Lock(0, 0, (void**) &linePointPtr, 0);
	DWORD*		lineIndicesPtr = 0;
	hResult = lineIndexBuffer->Lock(0, 0, (void**) &lineIndicesPtr, 0);
	assert(D3D_OK == hResult);
	int	index = 0;
	D3DXVECTOR3	nDir0 = xDir;
	D3DXVECTOR3	v0 = radius * yDir;
	for (int n = 0; n <= nPoints / 2; ++n, ++index) {
		float	angle = nStep * n * D3DX_PI / 180.f;
		// nDir0 축에 대해 v0 벡터를 angle 만큼 회전시킨 벡터를 얻는다.
		D3DXMATRIX	m;	// 회전에 해당하는 행렬
		D3DXMatrixRotationAxis(&m, &nDir0, angle);
		D3DXVECTOR3	v;
		D3DXVec3TransformCoord(&v, &v0, &m);
		linePointPtr[index] = __normal_manipulator.P0() + v;
		lineIndicesPtr[2 * index + 0] = index;
		lineIndicesPtr[2 * index + 1] = index + 1;
	}
	hResult = lineVertexBuffer->Unlock();
	assert(D3D_OK == hResult);
	lineIndexBuffer->Unlock();

	hResult = __effectPtr->SetInt("g__FillMode", D3DFILL_WIREFRAME);
	assert(D3D_OK == hResult);
	hResult = __effectPtr->SetVector("pointColor", &D3DXVECTOR4(1, 0, 0, 1));
	D3DXHANDLE	hTechnique = __effectPtr->GetTechnique(0);
	assert(hTechnique);
	hResult = __effectPtr->SetTechnique(hTechnique);
	assert(S_OK == hResult);
	UINT	NumberOfPasses;
	hResult = __effectPtr->Begin(&NumberOfPasses, 0);
	assert(D3D_OK == hResult);
	for (UINT iPass = 0; iPass < NumberOfPasses; ++iPass) {
		hResult = __effectPtr->BeginPass(iPass);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetVertexDeclaration(__OnePointDeclarationPtr);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetStreamSource(0, lineVertexBuffer, 0, sizeof(D3DXVECTOR3));
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetIndices(lineIndexBuffer);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->DrawIndexedPrimitive(D3DPT_LINELIST, 0, 0, nPoints, 0, index - 1);
		assert(D3D_OK == hResult);
		hResult = __effectPtr->EndPass();
		assert(S_OK == hResult);
	}
	hResult = __effectPtr->End();
	assert(S_OK == hResult);
}
{
	D3DXVECTOR3*	linePointPtr = NULL;
	hResult = lineVertexBuffer->Lock(0, 0, (void**) &linePointPtr, 0);
	DWORD*		lineIndicesPtr = 0;
	hResult = lineIndexBuffer->Lock(0, 0, (void**) &lineIndicesPtr, 0);
	assert(D3D_OK == hResult);
	int	index = 0;
	D3DXVECTOR3	nDir0 = yDir;
	D3DXVECTOR3	v0 = radius * nDir;
	for (int n = - nPoints / 4; n <= nPoints / 4; ++n, ++index) {
		float	angle = nStep * n * D3DX_PI / 180.f;
		// nDir0 축에 대해 v0 벡터를 angle 만큼 회전시킨 벡터를 얻는다.
		D3DXMATRIX	m;	// 회전에 해당하는 행렬
		D3DXMatrixRotationAxis(&m, &nDir0, angle);
		D3DXVECTOR3	v;
		D3DXVec3TransformCoord(&v, &v0, &m);
		linePointPtr[index] = __normal_manipulator.P0() + v;
		lineIndicesPtr[2 * index + 0] = index;
		lineIndicesPtr[2 * index + 1] = index + 1;
	}
	hResult = lineVertexBuffer->Unlock();
	assert(D3D_OK == hResult);
	lineIndexBuffer->Unlock();

	hResult = __effectPtr->SetInt("g__FillMode", D3DFILL_WIREFRAME);
	assert(D3D_OK == hResult);
	hResult = __effectPtr->SetVector("pointColor", &D3DXVECTOR4(0, 1, 0, 1));
	D3DXHANDLE	hTechnique = __effectPtr->GetTechnique(0);
	assert(hTechnique);
	hResult = __effectPtr->SetTechnique(hTechnique);
	assert(S_OK == hResult);
	UINT	NumberOfPasses;
	hResult = __effectPtr->Begin(&NumberOfPasses, 0);
	assert(D3D_OK == hResult);
	for (UINT iPass = 0; iPass < NumberOfPasses; ++iPass) {
		hResult = __effectPtr->BeginPass(iPass);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetVertexDeclaration(__OnePointDeclarationPtr);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetStreamSource(0, lineVertexBuffer, 0, sizeof(D3DXVECTOR3));
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetIndices(lineIndexBuffer);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->DrawIndexedPrimitive(D3DPT_LINELIST, 0, 0, nPoints, 0, index - 1);
		assert(D3D_OK == hResult);
		hResult = __effectPtr->EndPass();
		assert(S_OK == hResult);
	}
	hResult = __effectPtr->End();
	assert(S_OK == hResult);
}
} while (0);
	if (lineVertexBuffer) {
		lineVertexBuffer->Release();
		lineVertexBuffer = 0;
	}
	if (lineIndexBuffer) {
		lineIndexBuffer->Release();
		lineIndexBuffer = 0;
	}
#endif
}

int	kMesh::ManipulatorPick(const POINT& ptCursor)
{
	D3DVIEWPORT9	viewport;
	__devicePtr->GetViewport(&viewport);

	// manipulator 의 시작점 또는 끝점과 만나는 지 확인한다.
	D3DXVECTOR3	P1 = __normal_manipulator.P0();
	D3DXVECTOR3	P2 = __normal_manipulator.P0() + (.1f * __radius) * __normal_manipulator.nDir();
	D3DXVECTOR3	p[2];
	D3DXVec3Project(&p[0], &P1, &viewport, &__projectionMatrix, &__viewMatrix, &__worldMatrix);
	D3DXVec3Project(&p[1], &P2, &viewport, &__projectionMatrix, &__viewMatrix, &__worldMatrix);

//	UINT	k = 0;
	FLOAT	zNearest = FLT_MAX;
	int	index = -1;
	int	iMouseX = ptCursor.x;
	int	iMouseY = ptCursor.y;
	for (int i = 0; i < 2; ++i) {
#if 1
		const D3DXVECTOR3&	q = p[i];
		if (q.z < 0 || q.z > 1.f)
			continue;
		FLOAT	f = ceilf(q.x);
		int	qx = q.x < f + .5f ? (int) f : (int) (f + 1.f);
		int	high, low;
		high = iMouseX + 2;
		low = iMouseX - 2;
		if (qx > high || qx < low)
			continue;
		f = ceilf(q.y);
		int	qy = q.y < f + .5f ? (int) f : (int) (f + 1.f);
		high = iMouseY + 2;
		low = iMouseY - 2;
		if (qy > high || qy < low)
			continue;
#endif

		if (q.z < zNearest) {
			index = i;
			zNearest = q.z;
		}
	}
	if (index != -1) {
		(void) fprintf(stdout, "ManipulatorPick: index = %d\n", index);
	}

	return index;
}
#endif

void	kMesh::ShowDefinedArea()
{
	if (!__statePtr->g__showDefinedArea) {
		if (!__statePtr->g__showDefinedBodyElement)
			__overlap = false;

		// 칠해진 부분이 있다면 원래대로 되돌린다.
		__area.getTypeValRef() = 0;
		__area.setColor(*this);

		return;
	}
	__overlap = true;

	__area.setColor(*this);
}

#if 1	// 체성분
void	kMesh::ShowBodyElement()
{
	if (!__statePtr->g__showDefinedBodyElement) {
		if (!__statePtr->g__showDefinedArea)
			__overlap = false;

		// 칠해진 부분이 있다면 원래대로 되돌린다.
		kArea&	area = __bodyElement.getAreaRef();
		area.getTypeValRef() = 0;
		area.setColor(*this);

		// 기존에 보여주도록 한 부분을 모두 지운다.
		__bodyElement.getVsValueRef() = 0;

		return;
	}
	__overlap = true;

	// __bodyElement::__vsValue 을 참고해서 렌더링
	const float*	values = __bodyElement.getValuesRef();
	kArea&	area = __bodyElement.getAreaRef();

	int	vsValue = __bodyElement.getVsValueRef();
	area.getTypeValRef() = 0;	// 모든 영역을 클리어한 후, 설정된 영역만 표시한다.
	AREA_TYPE_VISIBLE	typeVisible;
	AREA_TYPE_T&	type = typeVisible.type;
	bool&	visible = typeVisible.visible;
//	bool	visible;	// kBodyElement::__area 의 해당 비트도 설정해야 한다!
	// 1. 측정량과 적정량을 읽어 그 값에 해당 부위의 색깔을 설정한다; ratio = 측정값 / 적정값
	// TO DO LIST: 한 부위에 동시에 두 가지 이상의 값을 표시하면 안된다.
	// 현재는 체지방량 < 체수분량 < 근육량
	// 체지방량
	if (vsValue & BODY_ELEMENT_VS_FAT_RIGHT_ARM) {
		AREA_TYPE_COLOR	areaTypeColor;
		areaTypeColor.type = AREA_RIGHT_ARM;
		areaTypeColor.fColor = values[BODY_ELEMENT_RAW_RIGHT_ARM_FAT] / values[BODY_ELEMENT_RAW_IDEAL_RIGHT_ARM_FAT];
		areaTypeColor.fColor *= .5f;
		area.setValue(AREA__COLOR, &areaTypeColor);
		visible = true;
		type = AREA_RIGHT_ARM;
		area.setValue(AREA__TYPE, &typeVisible);
	}
	if (vsValue & BODY_ELEMENT_VS_FAT_LEFT_ARM) {
		AREA_TYPE_COLOR	areaTypeColor;
		areaTypeColor.type = AREA_LEFT_ARM;
		areaTypeColor.fColor = values[BODY_ELEMENT_RAW_LEFT_ARM_FAT] / values[BODY_ELEMENT_RAW_IDEAL_LEFT_ARM_FAT];
		areaTypeColor.fColor *= .5f;
		area.setValue(AREA__COLOR, &areaTypeColor);
		visible = true;
		type = AREA_LEFT_ARM;
		area.setValue(AREA__TYPE, &typeVisible);
	}
	if (vsValue & BODY_ELEMENT_VS_FAT_RIGHT_LEG) {
		AREA_TYPE_COLOR	areaTypeColor;
		areaTypeColor.type = AREA_RIGHT_LEG;
		areaTypeColor.fColor = values[BODY_ELEMENT_RAW_RIGHT_LEG_FAT] / values[BODY_ELEMENT_RAW_IDEAL_RIGHT_LEG_FAT];
		areaTypeColor.fColor *= .5f;
		area.setValue(AREA__COLOR, &areaTypeColor);
		visible = true;
		type = AREA_RIGHT_LEG;
		area.setValue(AREA__TYPE, &typeVisible);
	}
	if (vsValue & BODY_ELEMENT_VS_FAT_LEFT_LEG) {
		AREA_TYPE_COLOR	areaTypeColor;
		areaTypeColor.type = AREA_LEFT_LEG;
		areaTypeColor.fColor = values[BODY_ELEMENT_RAW_LEFT_LEG_FAT] / values[BODY_ELEMENT_RAW_IDEAL_LEFT_LEG_FAT];
		areaTypeColor.fColor *= .5f;
		area.setValue(AREA__COLOR, &areaTypeColor);
		visible = true;
		type = AREA_LEFT_LEG;
		area.setValue(AREA__TYPE, &typeVisible);
	}
	if (vsValue & BODY_ELEMENT_VS_FAT_TRUNK) {
		AREA_TYPE_COLOR	areaTypeColor;
		areaTypeColor.type = AREA_TRUNK;
		areaTypeColor.fColor = values[BODY_ELEMENT_RAW_TRUNK_FAT] / values[BODY_ELEMENT_RAW_IDEAL_TRUNK_FAT];
		areaTypeColor.fColor *= .5f;
		area.setValue(AREA__COLOR, &areaTypeColor);
		visible = true;
		type = AREA_TRUNK;
		area.setValue(AREA__TYPE, &typeVisible);
	}
	// 체수분량
	if (vsValue & BODY_ELEMENT_VS_BODY_FLUID_RIGHT_ARM) {
		AREA_TYPE_COLOR	areaTypeColor;
		areaTypeColor.type = AREA_RIGHT_ARM;
		areaTypeColor.fColor = values[BODY_ELEMENT_RAW_RIGHT_ARM_BODY_FLUID] / values[BODY_ELEMENT_RAW_IDEAL_RIGHT_ARM_BODY_FLUID];
		area.setValue(AREA__COLOR, &areaTypeColor);
		visible = true;
		type = AREA_RIGHT_ARM;
		area.setValue(AREA__TYPE, &typeVisible);
	}
	if (vsValue & BODY_ELEMENT_VS_BODY_FLUID_LEFT_ARM) {
		AREA_TYPE_COLOR	areaTypeColor;
		areaTypeColor.type = AREA_LEFT_ARM;
		areaTypeColor.fColor = values[BODY_ELEMENT_RAW_LEFT_ARM_BODY_FLUID] / values[BODY_ELEMENT_RAW_IDEAL_LEFT_ARM_BODY_FLUID];
		areaTypeColor.fColor *= .5f;
		area.setValue(AREA__COLOR, &areaTypeColor);
		visible = true;
		type = AREA_LEFT_ARM;
		area.setValue(AREA__TYPE, &typeVisible);
	}
	if (vsValue & BODY_ELEMENT_VS_BODY_FLUID_RIGHT_LEG) {
		AREA_TYPE_COLOR	areaTypeColor;
		areaTypeColor.type = AREA_RIGHT_LEG;
		areaTypeColor.fColor = values[BODY_ELEMENT_RAW_RIGHT_LEG_BODY_FLUID] / values[BODY_ELEMENT_RAW_IDEAL_RIGHT_LEG_BODY_FLUID];
		areaTypeColor.fColor *= .5f;
		area.setValue(AREA__COLOR, &areaTypeColor);
		visible = true;
		type = AREA_RIGHT_LEG;
		area.setValue(AREA__TYPE, &typeVisible);
	}
	if (vsValue & BODY_ELEMENT_VS_BODY_FLUID_LEFT_LEG) {
		AREA_TYPE_COLOR	areaTypeColor;
		areaTypeColor.type = AREA_LEFT_LEG;
		areaTypeColor.fColor = values[BODY_ELEMENT_RAW_LEFT_LEG_BODY_FLUID] / values[BODY_ELEMENT_RAW_IDEAL_LEFT_LEG_BODY_FLUID];
		areaTypeColor.fColor *= .5f;
		area.setValue(AREA__COLOR, &areaTypeColor);
		visible = true;
		type = AREA_LEFT_LEG;
		area.setValue(AREA__TYPE, &typeVisible);
	}
	if (vsValue & BODY_ELEMENT_VS_BODY_FLUID_TRUNK) {
		AREA_TYPE_COLOR	areaTypeColor;
		areaTypeColor.type = AREA_TRUNK;
		areaTypeColor.fColor = values[BODY_ELEMENT_RAW_TRUNK_BODY_FLUID] / values[BODY_ELEMENT_RAW_IDEAL_TRUNK_BODY_FLUID];
		areaTypeColor.fColor *= .5f;
		area.setValue(AREA__COLOR, &areaTypeColor);
		visible = true;
		type = AREA_TRUNK;
		area.setValue(AREA__TYPE, &typeVisible);
	}
	// 근육량
	if (vsValue & BODY_ELEMENT_VS_SOFT_LEAN_MASS_RIGHT_ARM) {
		AREA_TYPE_COLOR	areaTypeColor;
		areaTypeColor.type = AREA_RIGHT_ARM;
		areaTypeColor.fColor = values[BODY_ELEMENT_RAW_RIGHT_ARM_SOFT_LEAN_MASS] / values[BODY_ELEMENT_RAW_IDEAL_RIGHT_ARM_SOFT_LEAN_MASS];
		areaTypeColor.fColor *= .5f;
		area.setValue(AREA__COLOR, &areaTypeColor);
		visible = true;
		type = AREA_RIGHT_ARM;
		area.setValue(AREA__TYPE, &typeVisible);
	}
	if (vsValue & BODY_ELEMENT_VS_SOFT_LEAN_MASS_LEFT_ARM) {
		AREA_TYPE_COLOR	areaTypeColor;
		areaTypeColor.type = AREA_LEFT_ARM;
		areaTypeColor.fColor = values[BODY_ELEMENT_RAW_LEFT_ARM_SOFT_LEAN_MASS] / values[BODY_ELEMENT_RAW_IDEAL_LEFT_ARM_SOFT_LEAN_MASS];
		areaTypeColor.fColor *= .5f;
		area.setValue(AREA__COLOR, &areaTypeColor);
		visible = true;
		type = AREA_LEFT_ARM;
		area.setValue(AREA__TYPE, &typeVisible);
	}
	if (vsValue & BODY_ELEMENT_VS_SOFT_LEAN_MASS_RIGHT_LEG) {
		AREA_TYPE_COLOR	areaTypeColor;
		areaTypeColor.type = AREA_RIGHT_LEG;
		areaTypeColor.fColor = values[BODY_ELEMENT_RAW_RIGHT_LEG_SOFT_LEAN_MASS] / values[BODY_ELEMENT_RAW_IDEAL_RIGHT_LEG_SOFT_LEAN_MASS];
		areaTypeColor.fColor *= .5f;
		area.setValue(AREA__COLOR, &areaTypeColor);
		visible = true;
		type = AREA_RIGHT_LEG;
		area.setValue(AREA__TYPE, &typeVisible);
	}
	if (vsValue & BODY_ELEMENT_VS_SOFT_LEAN_MASS_LEFT_LEG) {
		AREA_TYPE_COLOR	areaTypeColor;
		areaTypeColor.type = AREA_LEFT_LEG;
		areaTypeColor.fColor = values[BODY_ELEMENT_RAW_LEFT_LEG_SOFT_LEAN_MASS] / values[BODY_ELEMENT_RAW_IDEAL_LEFT_LEG_SOFT_LEAN_MASS];
		areaTypeColor.fColor *= .5f;
		area.setValue(AREA__COLOR, &areaTypeColor);
		visible = true;
		type = AREA_LEFT_LEG;
		area.setValue(AREA__TYPE, &typeVisible);
	}
	if (vsValue & BODY_ELEMENT_VS_SOFT_LEAN_MASS_TRUNK) {
		AREA_TYPE_COLOR	areaTypeColor;
		areaTypeColor.type = AREA_TRUNK;
		areaTypeColor.fColor = values[BODY_ELEMENT_RAW_TRUNK_SOFT_LEAN_MASS] / values[BODY_ELEMENT_RAW_IDEAL_TRUNK_SOFT_LEAN_MASS];
		areaTypeColor.fColor *= .5f;
		area.setValue(AREA__COLOR, &areaTypeColor);
		visible = true;
		type = AREA_TRUNK;
		area.setValue(AREA__TYPE, &typeVisible);
	}

	// 2. 메쉬에 직접 칠한다.
	area.setColor(*this);
}
#endif

#if 1	// 뼈
int	kMesh::getPoint(DWORD faceID, float baryU, float baryV, D3DXVECTOR3& P, D3DXVECTOR3* P0Ptr) const
{
	if (faceID >= __numFaces || !__facePtr)
		return -1;
	const CFace&	face = __facePtr[faceID];
	D3DXVECTOR3	V[3];
	D3DXVECTOR3	V0[3];
	for (int i = 0; i < 3; ++i) {
		if (face.prID[i] >= __numPointReprs || !__pointReprPtr)
			return -1;
		V[i] = __pointReprPtr[face.prID[i]].v.position;
		if (P0Ptr)
			V0[i] = __pointReprPtr[face.prID[i]].v.position0;
	}
	// V1 + baryU * (V2 - V1) + baryV * (V3 - V1)
	P = V[0] + baryU * (V[1] - V[0]) + baryV * (V[2] - V[0]);
	if (P0Ptr)
		*P0Ptr = V0[0] + baryU * (V0[1] - V0[0]) + baryV * (V0[2] - V0[0]);
	return 0;
}
int	kMesh::getPoint(const POINT_DEF& point_def, D3DXVECTOR3& P, D3DXVECTOR3* P0Ptr) const
{
	return getPoint(point_def.ID, point_def.baryU, point_def.baryV, P, P0Ptr);
}
int	kMesh::getPoint(const INTERSECTION& intersection, D3DXVECTOR3& P, D3DXVECTOR3* P0Ptr) const
{
	return getPoint(intersection.dwFace, intersection.fBary1, intersection.fBary2, P, P0Ptr);
}
#else	// 뼈/!뼈
int	kMesh::getPoint(DWORD faceID, float baryU, float baryV, D3DXVECTOR3& P) const
{
	if (faceID >= __numFaces || !__facePtr)
		return -1;
	const CFace&	face = __facePtr[faceID];
	D3DXVECTOR3	V[3];
	for (int i = 0; i < 3; ++i) {
		if (face.prID[i] >= __numPointReprs || !__pointReprPtr)
			return -1;
		V[i] = __pointReprPtr[face.prID[i]].v.position;
	}
	// V1 + baryU * (V2 - V1) + baryV * (V3 - V1)
	P = V[0] + baryU * (V[1] - V[0]) + baryV * (V[2] - V[0]);
	return 0;
}
int	kMesh::getPoint(const POINT_DEF& point_def, D3DXVECTOR3& P) const
{
	return getPoint(point_def.ID, point_def.baryU, point_def.baryV, P);
}
int	kMesh::getPoint(const INTERSECTION& intersection, D3DXVECTOR3& P) const
{
	return getPoint(intersection.dwFace, intersection.fBary1, intersection.fBary2, P);
}
#endif	// !뼈
int	kMesh::getNormal(DWORD faceID, float baryU, float baryV, D3DXVECTOR3& N) const
{
	if (faceID >= __numFaces || !__facePtr)
		return -1;
	const CFace&	face = __facePtr[faceID];
	D3DXVECTOR3	V[3];
	for (int i = 0; i < 3; ++i) {
		if (face.prID[i] >= __numPointReprs || !__pointReprPtr)
			return -1;
		V[i] = __pointReprPtr[face.prID[i]].v.normal;
	}
	// V1 + baryU * (V2 - V1) + baryV * (V3 - V1)
	N = V[0] + baryU * (V[1] - V[0]) + baryV * (V[2] - V[0]);
	D3DXVec3Normalize(&N, &N);
	return 0;
}
int	kMesh::getNormal(const POINT_DEF& point_def, D3DXVECTOR3& N) const
{
	return getNormal(point_def.ID, point_def.baryU, point_def.baryV, N);
}
int	kMesh::getNormal(const INTERSECTION& intersection, D3DXVECTOR3& N) const
{
	return getNormal(intersection.dwFace, intersection.fBary1, intersection.fBary2, N);
}

#if 1	// 큐 관리
void	kMesh::history(
	void
)
{
	HRESULT	hResult;

	// 세 개의 버퍼를 접근해서 한다;
	//	노말 정보를 담은 버퍼(kShowNormal::__vertexBufferPtr)
	//	렌더링 정보를 담은 버퍼(kMesh::__vertexBufferPtr)
	//	메쉬 정보를 담음 버퍼(kMesh::__meshPtr)
	LPDIRECT3DVERTEXBUFFER9	pVB_n = __showNormal.GetVertexBufferPointer();
	kShowNormal::Vertex*	pV_n;
	pVB_n->Lock(0, 0, (void**) &pV_n, 0);
	kMesh::D3DXVERTEX*	pV;
	__vertexBufferPtr->Lock(0, 0, (void**) &pV, 0);
	FLOAT*	pVB_ = NULL;
	hResult = __meshPtr->LockVertexBuffer(0, (void**) &pVB_);
	assert(D3D_OK == hResult);
	CPointRepr::VERTEX*	vertexArray;	// 텍스쳐 메쉬 정보를 담은 Vertex Buffer
	__meshLoader.meshSubsetPtr()->LockVertexBuffer(0, (void**) &vertexArray);

	int		iCurrent = GetCurrent();
	int		iHead = GetHead();
	int		iTail = GetTail();
	unsigned	uSize = GetQueueSize();

	if (__statePtr->g__Control) {	// Remember
		// iTail 에 기억한다;
		// iTail 이 iHead 와 같다면 iHead 를 1만큼 증가시킨다.
		// iCurrent 는 그대로 유지된다;
		// 설정되지 않았다면 0으로 설정한다.
		if (uSize == 0) {
			// 원형큐가 비었다;
			// 1. iHead = iTail = 0
			// 2. uSize = 1
			iHead = iTail = iCurrent = 0;
			uSize = 1;
			m__iHead = 0;
			m__iTail = 0;
			m__iCurrent = 0;
			m__uSize = 1;
		} else if (uSize == LAST_POSITION__QUEUE_SIZE) {
			// 원형큐가 가득찼다;
			// 1. iHead 자리에 iTail 를 위치시키고
			// 2. 그 자리에 쓴다.
			// 3. iHead 을 하나 증가시킨다.
			iTail = iHead;
			iHead = (iHead + 1) % LAST_POSITION__QUEUE_SIZE;
			m__iHead = iHead;
			m__iTail = iTail;
		} else {
			// 원형큐에 자리가 있다;
			// 1. iTail 을 하나 증가시키고
			// 2. 그 자리에 쓴다.
			iTail = (iTail + 1) % LAST_POSITION__QUEUE_SIZE;
			++uSize;
			m__iTail = iTail;
			m__uSize = uSize;
		}

		m__fVolumeChangeQueue[iTail] = __fVolumeChange;
		for (DWORD iPointRepr = 0; iPointRepr < __numPointReprs; ++iPointRepr) {
			pV[iPointRepr].pQueue[iTail] = pV[iPointRepr].p;
			pV[iPointRepr].cQueue[iTail] = pV[iPointRepr].c;
		}
	} else {
		bool	bMove = false;
		if (__statePtr->g__Shift) {
			// iCurrent 를 뒤로 1만큼 이동시킨다.
			if (iCurrent == -1) {
				bMove = false;
			} else {
				bMove = true;
				if (iCurrent)
					--iCurrent;
				else
					iCurrent = LAST_POSITION__QUEUE_SIZE - 1;
				if (
					(iHead > iTail && (iCurrent < iHead && iCurrent > iTail)) ||
					(iHead <= iTail && (iCurrent < iHead || iCurrent > iTail))
				)
					iCurrent = iTail;
			}
		} else {
			// iCurrent 를 앞으로 1만큼 이동시킨다.
			if (iCurrent == -1) {
				bMove = false;
			} else {
				bMove = true;
				iCurrent = (iCurrent + 1) % LAST_POSITION__QUEUE_SIZE;
				if (
					(iHead > iTail && (iCurrent > iTail && iCurrent < iHead)) ||
					(iHead <= iTail && (iCurrent > iTail || iCurrent < iHead))
				)
					iCurrent = iHead;
			}
		}
		if (bMove) {
			m__iCurrent = iCurrent;
			DWORD*	pPointReprFlag = new DWORD[__numPointReprs];
			FillMemory(pPointReprFlag, __numPointReprs * sizeof(DWORD), 0);

			for (DWORD i = 0; i < __numPointReprs; ++i) {
				DWORD	prID = __pointReprPtr[i].id;
				if (prID == (DWORD) -1)
					continue;

				DWORD	index = i;
				D3DXVECTOR3&	p = pV[index].pQueue[iCurrent];
				pV_n[2 * index + 0].Position = p;
				pV_n[2 * index + 1].Position = p;
				pV[index].p = p;
				pV[index].c = pV[index].cQueue[iCurrent];
				pVB_[8 * index + 0] = p.x;
				pVB_[8 * index + 1] = p.y;
				pVB_[8 * index + 2] = p.z;
#if 1	// 텍스쳐 메쉬 변형
#ifdef __USE_A_OFFSET__
		for (int k = pointer_to__a_vVertexID_a_offset[prID]; k < pointer_to__a_vVertexID_a_offset[prID + 1]; ++k)
			vertexArray[pointer_to__a_vVertexID[k]].position = p;
#else
		const std::set<DWORD>&	vVertexID = __pointReprPtr[prID].vVertexID;
		for (std::set<DWORD>::const_iterator it = vVertexID.begin(); it != vVertexID.end(); ++it)
			vertexArray[*it].position = p;
#endif
#endif	// 텍스쳐 메쉬 변형
				D3DXVECTOR3&	cColor = pV[index].c;
				if (cColor.x != 0.f || cColor.y != 0.f || cColor.z != 0.f)
					pPointReprFlag[prID] = 1;
				__pointReprPtr[i].v.position = p;
			}

#if 1
			__fVolumeChange = m__fVolumeChangeQueue[iCurrent];
#else
			// 원래 위치 p_ 와 비교하여 Volume 의 변화를 계산한다.
			__calculateVolumeChange(pV, pPointReprFlag);
#endif

			SAFE_DELETE_ARRAY(pPointReprFlag);

#if 1	// Vertex 의 새로운 노말 결정
{
	// 각 Vertex 의 노말을 다시 계산;
	// Vertex 를 공유하는 Face 들의 노말의 평균이 해당 Vertex 의 노말
	// 1. 각 Face 의 노말을 저장할 임시 공간
	D3DXVECTOR3*	faceNormalArray = new D3DXVECTOR3[__numFaces];
	for (int n = 0; n < (int) __numFaces; ++n) {	// n:= face ID
		const CFace&	face = __facePtr[n];
		D3DXVECTOR3&	p0 = pV[face.prID0].p;
		D3DXVECTOR3&	p1 = pV[face.prID1].p;
		D3DXVECTOR3&	p2 = pV[face.prID2].p;
		D3DXVECTOR3	v01 = p1 - p0;
//		D3DXVec3Normalize(&v01, &v01);
		D3DXVECTOR3	v02 = p2 - p0;
//		D3DXVec3Normalize(&v02, &v02);
		D3DXVECTOR3&	normal = faceNormalArray[n];
		D3DXVec3Cross(&normal, &v01, &v02);
		D3DXVec3Normalize(&normal, &normal);
	}
	// 2. 각 Vertex 의 노말을 공유하는 Face 들의 노말로부터 계산
	for (int prID = 0; prID < (int) __numPointReprs; ++prID) {	// n:= point repr. ID
		D3DXVECTOR3&	normal = pV[prID].n;
		unsigned	numFaces = 0;
#ifdef __USE_A_OFFSET__
		numFaces = pointer_to__a_vFaceIDLocalIndex_a_offset[prID + 1] - pointer_to__a_vFaceIDLocalIndex_a_offset[prID];
#else
		numFaces = __pointReprPtr[prID].vFaceIdLocalIndex.size();
#endif
#ifdef __USE_A_OFFSET__
		for (int k = pointer_to__a_vFaceIDLocalIndex_a_offset[prID]; k < pointer_to__a_vFaceIDLocalIndex_a_offset[prID + 1]; ++k) {
			const FACE_ID_LOCAL_INDEX&	faceIDLocalIndex = pointer_to__a_vFaceIDLocalIndex[k];
#else
		for (size_t k = 0; k < __pointReprPtr[prID].vFaceIdLocalIndex.size(); ++k) {
			const FACE_ID_LOCAL_INDEX&	faceIDLocalIndex = __pointReprPtr[prID].vFaceIdLocalIndex.at(k);
#endif
			normal += faceNormalArray[faceIDLocalIndex.ID];
#ifdef __USE_A_OFFSET__
		}
#else
		}
#endif
		normal /= (float) numFaces;
		D3DXVec3Normalize(&normal, &normal);
		// __meshPtr 에 사용되는 노말도 업데이트
	/*
		pVertices[i].n.x = pVB[8 * i + 3];
		pVertices[i].n.y = pVB[8 * i + 4];
		pVertices[i].n.z = pVB[8 * i + 5];
	*/
		pVB_[8 * prID + 3] = normal.x;
		pVB_[8 * prID + 4] = normal.y;
		pVB_[8 * prID + 5] = normal.z;
	}
	SAFE_DELETE_ARRAY(faceNormalArray);
}
#endif	// Vertex 의 새로운 노말 결정
		}
	}


	__meshLoader.meshSubsetPtr()->UnlockVertexBuffer();
	hResult = __meshPtr->UnlockVertexBuffer();
	assert(D3D_OK == hResult);
	__vertexBufferPtr->Unlock();
	pVB_n->Unlock();
}
#endif

#if 1	// manipulator_callback
	// kNormalManipulator 에서 Done 이벤트가 발생했을 때,
	// 새로운 nDir 을 사용하여 길이를 다시 재도록 하는 함수
// 임의로 하는 둘레측정에서 사용되는 콜백함수; Pick.cpp 의 OnFrameMove 에 있는 함수
void	normal_manipulator_done_callback_manual_measure(kMesh* pointer_to_kMesh, int /* nType */, START_DEF_PTR* pointer_to_START_DEF_PTR, void* valuePtr)
{
	// 의미있는 포인터로 설정
	if (!pointer_to_kMesh) {
		(void) fprintf(stderr, "FATAL: normal_manipulator_done_callback_manual_measure: pointer_to_kMesh is nil\n");
		return;
	}
	kMesh*	meshPtr = pointer_to_kMesh;
	kState*	statePtr = meshPtr->statePtr();
	if (!statePtr) {
		(void) fprintf(stderr, "FATAL: normal_manipulator_done_callback_manual_measure: kMesh::__statePtr is nil\n");
		return;
	}
	if (!pointer_to_START_DEF_PTR) {
		(void) fprintf(stderr, "FATAL: normal_manipulator_done_callback_manual_measure: pointer_to_START_DEF_PTR is nil\n");
		return;
	}
	START_DEF_PTR&	startDefPtr = *((START_DEF_PTR*) pointer_to_START_DEF_PTR);

	// overwrite
	assert(startDefPtr.type == START_DEF_PTR_TYPE__GIRTH || startDefPtr.type == START_DEF_PTR_TYPE__TWO_POINT);
	D3DXVECTOR3&	nDir = startDefPtr.type == START_DEF_PTR_TYPE__GIRTH ? startDefPtr.plane_def_ptr->normal : startDefPtr.two_point_plane_def_ptr->normal;
	nDir = *((D3DXVECTOR3*) valuePtr);

	// 모두 Reference
	FLOAT&				length			= statePtr->g__stringLength;
	bool&				closed			= statePtr->g__stringClosed;
	int&				numSegments_in_kState	= statePtr->g__numSegments;
	kTapeMeasure&			tapeMeasure		= meshPtr->tapeMeasure();
	DWORD&				numSegments_in_kMesh	= meshPtr->numSegments();
	TAPE_MEASURE_METHOD_T&		segmentMethod		= meshPtr->segmentMethod();
	LPDIRECT3DVERTEXBUFFER9&	segmentVertexBuffer	= meshPtr->GetSegmentVertexBuffer();
	LPDIRECT3DINDEXBUFFER9&		segmentIndexBuffer	= meshPtr->GetSegmentIndexBuffer();

	// 둘레측정
#if 1	// SEGMENT_VERTEX
	CGrowableArray<SEGMENT_VERTEX>	segmentPointArray;
	CGrowableArray<SEGMENT_VERTEX>	twoPointArray;
	CGrowableArray<SEGMENT_VERTEX>	convexPointArray;
#else
	std::vector<D3DXVECTOR3>	segmentPointArray;
	std::vector<D3DXVECTOR3>	twoPointArray;
	std::vector<D3DXVECTOR3>	convexPointArray;
#endif
	numSegments_in_kState = kTapeMeasure::doMeasure_s(
		meshPtr,

		statePtr->g__mode,
		startDefPtr,

		// [OUT]
		NULL,
		segmentPointArray, length, closed,
		twoPointArray, convexPointArray
	);
#if 1	// manipulator; LINE MANIPULATOR
	meshPtr->tapeMeasure().isReleased() = false;
#endif	// manipulator; LINE MANIPULATOR
	meshPtr->tapeMeasure().length() = length;	// 임의계측에서 참고할 수 있도록 설정
#ifdef _DEBUG
	(void) fprintf(stdout,
		"# of segments = %d, length = %f, closed? %s; "
		"# of segment point(s) = %d, # of two point(s) = %d, # of convex point(s) = %d\n"
		,
		numSegments_in_kState, length, closed ? "True" : "False",
#if 1	// SEGMENT_VERTEX
		segmentPointArray.GetSize(), twoPointArray.GetSize(), convexPointArray.GetSize()
#else
		segmentPointArray.size(), twoPointArray.size(), convexPointArray.size()
#endif
	);
#endif
	tapeMeasure.twoPointArray() = twoPointArray;
	tapeMeasure.SetTwoPointBuffer();
	tapeMeasure.convexPointArray() = convexPointArray;
	statePtr->g__next_convex_index = 0;
	if (startDefPtr.type == START_DEF_PTR_TYPE__GIRTH) {
		tapeMeasure.SetConvexBuffer(closed);
		segmentMethod = TAPE_MEASURE_METHOD_GIRTH;
	} else if (startDefPtr.type == START_DEF_PTR_TYPE__TWO_POINT) {
		tapeMeasure.SetConvexBuffer(false);
		segmentMethod = TAPE_MEASURE_METHOD_TWO_POINT;
	} else
		assert(0);
	numSegments_in_kMesh = numSegments_in_kState;
	if (numSegments_in_kMesh > 0) {
#if 1	// SEGMENT_VERTEX
		numSegments_in_kMesh = segmentPointArray.GetSize();
#else
		numSegments_in_kMesh = segmentPointArray.size();
#endif
		if (!closed)
			--numSegments_in_kMesh;
		meshPtr->SetSegmentBuffer(segmentVertexBuffer, segmentIndexBuffer, segmentPointArray, closed);
	}
	(void) fprintf(stdout, "numSegments_in_kMesh = %d, numSegments_in_kState = %d\n", numSegments_in_kMesh, numSegments_in_kState);
#if 1	// 2D단면
	meshPtr->set2dProjectVertexBuffer();
#endif
}

// 이미 정의된 둘레측정에 사용되는 콜백함수; kMesh::setMeasure 의 변형
void	normal_manipulator_done_callback_defined_measure(kMesh* pointer_to_kMesh, int nType, START_DEF_PTR* /* pointer_to_START_DEF_PTR */, void* valuePtr)
{
	// 의미있는 포인터로 설정
	if (!pointer_to_kMesh) {
		(void) fprintf(stderr, "FATAL: normal_manipulator_done_callback_defined_measure: pointer_to_kMesh is nil\n");
		return;
	}
	kMesh*	meshPtr = (kMesh*) pointer_to_kMesh;
	TAPE_MEASURE_TYPE_T	type = (TAPE_MEASURE_TYPE_T) nType;

	const MEASURE_PTR_HASH&	measure_ptr_dic = meshPtr->measure_ptr_dic();

#if 1	// kMesh::setMeasure 와 비슷
	MEASURE_PTR_HASH::const_iterator	it;
	it = measure_ptr_dic.find(type);
	if (it == measure_ptr_dic.end()) {
		(void) fprintf(stderr, "FATAL: normal_manipulator_done_callback_defined_measure: Type = %d: Not implemented yet\n", type);
		return;
	}

	const MEASURE&	measure = *it->second;
	const std::vector<MEASURE_ITEM*>&	measure_item_ptr_list = measure.measure_item_ptr_list;
	size_t	nDef = measure_item_ptr_list.size();
	if (nDef == 0) {
		(void) fprintf(stderr, "FATAL: normal_manipulator_done_callback_defined_measure: Type = %d: No element\n", type);
		return;
	}
	if (nDef > 1) {
		(void) fprintf(stderr, "WARNING: normal_manipulator_done_callback_defined_measure: Type = %d: Two elements or more: Not implemented yet\n", type);
	}
#endif	// kMesh::setMeasure 와 비슷

#if 1	// kMesh::setMeasureItem 과 비슷
	MEASURE_ITEM&	m_item = *measure_item_ptr_list.at(0);
	if (!m_item.__ptr) {
		(void) fprintf(stderr, "ERROR: normal_manipulator_done_callback_defined_measure: Type = %d: m_item.__ptr is nil: Obsolete Definition\n", type);
		return;
	}
	if (m_item.__type != MEASURE_TYPE__GIRTH && m_item.__type != MEASURE_TYPE__TWO_POINT_GIRTH) {
		(void) fprintf(stderr, "ERROR: normal_manipulator_done_callback_defined_measure: Type = %d: GIRTH Only\n", type);
		return;
	}
#endif	// kMesh::setMeasureItem 과 비슷

#if 1	// normal_manipulator_done_callback_manual_measure 와 비슷
	const D3DXVECTOR3&	nDir = *((D3DXVECTOR3*) valuePtr);
	TAPE_MEASURE_MODE_T	mode = TAPE_MEASURE_MODE_CONTOUR;
	START_DEF_PTR	startDefPtr;
	if (m_item.__type == MEASURE_TYPE__GIRTH) {
		startDefPtr.type = START_DEF_PTR_TYPE__GIRTH;
		MEASURE_ITEM_PTR_DEF__GIRTH&	def = *((MEASURE_ITEM_PTR_DEF__GIRTH*) m_item.__ptr);
		mode = def.__mode;
		startDefPtr.plane_def_ptr = &def.__planeDef;

		// overwrite
		def.__planeDef.normal = nDir;
	} else if (m_item.__type == MEASURE_TYPE__TWO_POINT_GIRTH) {
		startDefPtr.type = START_DEF_PTR_TYPE__TWO_POINT;
		MEASURE_ITEM_PTR_DEF__TWO_POINT_GIRTH&	def = *((MEASURE_ITEM_PTR_DEF__TWO_POINT_GIRTH*) m_item.__ptr);
		mode = def.__mode;
		startDefPtr.two_point_plane_def_ptr = &def.__2pDef;

		// overwrite
		def.__2pDef.normal = nDir;
	} else
		assert(0);

#if 1	// SEGMENT_VERTEX
	CGrowableArray<SEGMENT_VERTEX>	segmentPointArray;
	CGrowableArray<SEGMENT_VERTEX>	twoPointArray;
	CGrowableArray<SEGMENT_VERTEX>	convexPointArray;
#else
	std::vector<D3DXVECTOR3>	segmentPointArray;
	std::vector<D3DXVECTOR3>	twoPointArray;
	std::vector<D3DXVECTOR3>	convexPointArray;
#endif
	float&	length = m_item.length;
	bool&	closed = m_item.closed;
	int&	numSegments = m_item.numSegments;
	numSegments = kTapeMeasure::doMeasure_s(
		meshPtr,

		mode,
		startDefPtr,

		// [OUT]
		NULL,
		segmentPointArray, length, closed,
		twoPointArray, convexPointArray
	);
#if 1	// manipulator; LINE MANIPULATOR
	meshPtr->tapeMeasure().isReleased() = false;
#endif	// manipulator; LINE MANIPULATOR
	meshPtr->tapeMeasure().length() = -1.f;	// 임의계측에서 참고할 수 있도록 설정
#ifdef _DEBUG
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
#endif
	meshPtr->SetSegmentBuffer(
		m_item.segmentVertexBuffer,
		m_item.segmentIndexBuffer,
		mode == TAPE_MEASURE_MODE_CONTOUR ?
		(startDefPtr.type == START_DEF_PTR_TYPE__GIRTH ?
			segmentPointArray :
			twoPointArray
		) :
			convexPointArray,
		closed
	);
#endif	// normal_manipulator_done_callback_manual_measure 와 비슷
}

#if 1	// point manipulator_callback
// normal_manipulator_done_callback_manual_measure 함수에서 normal 대신에 pointDef 를 변경
void	point_manipulator_done_callback_manual_measure(kMesh* pointer_to_kMesh, int /* nType */, START_DEF_PTR* pointer_to_START_DEF_PTR, void* valuePtr)
{
	// 의미있는 포인터로 설정
	if (!pointer_to_kMesh) {
		(void) fprintf(stderr, "FATAL: normal_manipulator_done_callback_manual_measure: pointer_to_kMesh is nil\n");
		return;
	}
	kMesh*	meshPtr = pointer_to_kMesh;
	kState*	statePtr = meshPtr->statePtr();
	if (!statePtr) {
		(void) fprintf(stderr, "FATAL: normal_manipulator_done_callback_manual_measure: kMesh::__statePtr is nil\n");
		return;
	}
	if (!pointer_to_START_DEF_PTR) {
		(void) fprintf(stderr, "FATAL: normal_manipulator_done_callback_manual_measure: pointer_to_START_DEF_PTR is nil\n");
		return;
	}
	START_DEF_PTR&	startDefPtr = *((START_DEF_PTR*) pointer_to_START_DEF_PTR);

	// overwrite
	assert(startDefPtr.type == START_DEF_PTR_TYPE__GIRTH || startDefPtr.type == START_DEF_PTR_TYPE__TWO_POINT);
	const kPointManipulator::INDEX_POINT_DEF_PTR&	index_pointDefPtr = *((kPointManipulator::INDEX_POINT_DEF_PTR*) valuePtr);
	POINT_DEF&	pointDef = startDefPtr.type == START_DEF_PTR_TYPE__GIRTH ? startDefPtr.plane_def_ptr->pointDef : (index_pointDefPtr.index == 0 ? startDefPtr.two_point_plane_def_ptr->point0Def : startDefPtr.two_point_plane_def_ptr->point1Def);
	pointDef = *index_pointDefPtr.pointDefPtr;
	if (startDefPtr.type == START_DEF_PTR_TYPE__TWO_POINT)
		startDefPtr.two_point_plane_def_ptr->normal = D3DXVECTOR3(0, 0, 0);

	// 모두 Reference
	FLOAT&				length			= statePtr->g__stringLength;
	bool&				closed			= statePtr->g__stringClosed;
	int&				numSegments_in_kState	= statePtr->g__numSegments;
	kTapeMeasure&			tapeMeasure		= meshPtr->tapeMeasure();
	DWORD&				numSegments_in_kMesh	= meshPtr->numSegments();
	TAPE_MEASURE_METHOD_T&		segmentMethod		= meshPtr->segmentMethod();
	LPDIRECT3DVERTEXBUFFER9&	segmentVertexBuffer	= meshPtr->GetSegmentVertexBuffer();
	LPDIRECT3DINDEXBUFFER9&		segmentIndexBuffer	= meshPtr->GetSegmentIndexBuffer();

	// 둘레측정
#if 1	// SEGMENT_VERTEX
	CGrowableArray<SEGMENT_VERTEX>	segmentPointArray;
	CGrowableArray<SEGMENT_VERTEX>	twoPointArray;
	CGrowableArray<SEGMENT_VERTEX>	convexPointArray;
#else
	std::vector<D3DXVECTOR3>	segmentPointArray;
	std::vector<D3DXVECTOR3>	twoPointArray;
	std::vector<D3DXVECTOR3>	convexPointArray;
#endif
	numSegments_in_kState = kTapeMeasure::doMeasure_s(
		meshPtr,

		statePtr->g__mode,
		startDefPtr,

		// [OUT]
		NULL,
		segmentPointArray, length, closed,
		twoPointArray, convexPointArray
	);
#if 1	// manipulator; LINE MANIPULATOR
	meshPtr->tapeMeasure().isReleased() = false;
#endif	// manipulator; LINE MANIPULATOR
	meshPtr->tapeMeasure().length() = length;	// 임의계측에서 참고할 수 있도록 설정
#ifdef _DEBUG
	(void) fprintf(stdout,
		"# of segments = %d, length = %f, closed? %s; "
		"# of segment point(s) = %d, # of two point(s) = %d, # of convex point(s) = %d\n"
		,
		numSegments_in_kState, length, closed ? "True" : "False",
#if 1	// SEGMENT_VERTEX
		segmentPointArray.GetSize(), twoPointArray.GetSize(), convexPointArray.GetSize()
#else
		segmentPointArray.size(), twoPointArray.size(), convexPointArray.size()
#endif
	);
#endif
	tapeMeasure.twoPointArray() = twoPointArray;
	tapeMeasure.SetTwoPointBuffer();
	tapeMeasure.convexPointArray() = convexPointArray;
	statePtr->g__next_convex_index = 0;
	if (startDefPtr.type == START_DEF_PTR_TYPE__GIRTH) {
		tapeMeasure.SetConvexBuffer(closed);
		segmentMethod = TAPE_MEASURE_METHOD_GIRTH;
	} else if (startDefPtr.type == START_DEF_PTR_TYPE__TWO_POINT) {
		tapeMeasure.SetConvexBuffer(false);
		segmentMethod = TAPE_MEASURE_METHOD_TWO_POINT;
	} else
		assert(0);
	numSegments_in_kMesh = numSegments_in_kState;
	if (numSegments_in_kMesh > 0) {
#if 1
		numSegments_in_kMesh = segmentPointArray.GetSize();
#else
		numSegments_in_kMesh = segmentPointArray.size();
#endif
		if (!closed)
			--numSegments_in_kMesh;
		meshPtr->SetSegmentBuffer(segmentVertexBuffer, segmentIndexBuffer, segmentPointArray, closed);
	}
#if 1	// 2D단면
	meshPtr->set2dProjectVertexBuffer();
#endif
}

// normal_manipulator_done_callback_defined_measure 에서 normal 대신에 pointDef 를 변경;
// START_DEF_PTR_TYPE__GIRTH 의 경우, normal 을 0 으로 초기화
void	point_manipulator_done_callback_defined_measure(kMesh* pointer_to_kMesh, int nType, START_DEF_PTR* /* pointer_to_START_DEF_PTR */, void* valuePtr)
{
	// 의미있는 포인터로 설정
	if (!pointer_to_kMesh) {
		(void) fprintf(stderr, "FATAL: normal_manipulator_done_callback_defined_measure: pointer_to_kMesh is nil\n");
		return;
	}
	kMesh*	meshPtr = (kMesh*) pointer_to_kMesh;
	TAPE_MEASURE_TYPE_T	type = (TAPE_MEASURE_TYPE_T) nType;

	const MEASURE_PTR_HASH&	measure_ptr_dic = meshPtr->measure_ptr_dic();

#if 1	// kMesh::setMeasure 와 비슷
	MEASURE_PTR_HASH::const_iterator	it;
	it = measure_ptr_dic.find(type);
	if (it == measure_ptr_dic.end()) {
		(void) fprintf(stderr, "FATAL: normal_manipulator_done_callback_defined_measure: Type = %d: Not implemented yet\n", type);
		return;
	}

	const MEASURE&	measure = *it->second;
	const std::vector<MEASURE_ITEM*>&	measure_item_ptr_list = measure.measure_item_ptr_list;
	size_t	nDef = measure_item_ptr_list.size();
	if (nDef == 0) {
		(void) fprintf(stderr, "FATAL: normal_manipulator_done_callback_defined_measure: Type = %d: No element\n", type);
		return;
	}
	if (nDef > 1) {
		(void) fprintf(stderr, "WARNING: normal_manipulator_done_callback_defined_measure: Type = %d: Two elements or more: Not implemented yet\n", type);
	}
#endif	// kMesh::setMeasure 와 비슷

#if 1	// kMesh::setMeasureItem 과 비슷
	MEASURE_ITEM&	m_item = *measure_item_ptr_list.at(0);
	if (!m_item.__ptr) {
		(void) fprintf(stderr, "ERROR: normal_manipulator_done_callback_defined_measure: Type = %d: m_item.__ptr is nil: Obsolete Definition\n", type);
		return;
	}
	if (m_item.__type != MEASURE_TYPE__GIRTH && m_item.__type != MEASURE_TYPE__TWO_POINT_GIRTH) {
		(void) fprintf(stderr, "ERROR: normal_manipulator_done_callback_defined_measure: Type = %d: GIRTH Only\n", type);
		return;
	}
#endif	// kMesh::setMeasureItem 과 비슷

#if 1	// normal_manipulator_done_callback_manual_measure 와 비슷
	const kPointManipulator::INDEX_POINT_DEF_PTR	index_pointDefPtr = *((kPointManipulator::INDEX_POINT_DEF_PTR*) valuePtr);
	TAPE_MEASURE_MODE_T	mode;
	START_DEF_PTR	startDefPtr;
	if (m_item.__type == MEASURE_TYPE__GIRTH) {
		startDefPtr.type = START_DEF_PTR_TYPE__GIRTH;
		MEASURE_ITEM_PTR_DEF__GIRTH&	def = *((MEASURE_ITEM_PTR_DEF__GIRTH*) m_item.__ptr);
		mode = def.__mode;
		startDefPtr.plane_def_ptr = &def.__planeDef;

		// overwrite
		def.__planeDef.pointDef = *index_pointDefPtr.pointDefPtr;
	} else {
		startDefPtr.type = START_DEF_PTR_TYPE__TWO_POINT;
		MEASURE_ITEM_PTR_DEF__TWO_POINT_GIRTH&	def = *((MEASURE_ITEM_PTR_DEF__TWO_POINT_GIRTH*) m_item.__ptr);
		mode = def.__mode;
		startDefPtr.two_point_plane_def_ptr = &def.__2pDef;

		// overwrite
		POINT_DEF&	pointDef = index_pointDefPtr.index == 0 ? def.__2pDef.point0Def : def.__2pDef.point1Def;
		pointDef = *index_pointDefPtr.pointDefPtr;
		def.__2pDef.normal = D3DXVECTOR3(0, 0, 0);
	}

#if 1	// SEGMENT_VERTEX
	CGrowableArray<SEGMENT_VERTEX>	segmentPointArray;
	CGrowableArray<SEGMENT_VERTEX>	twoPointArray;
	CGrowableArray<SEGMENT_VERTEX>	convexPointArray;
#else
	std::vector<D3DXVECTOR3>	segmentPointArray;
	std::vector<D3DXVECTOR3>	twoPointArray;
	std::vector<D3DXVECTOR3>	convexPointArray;
#endif
	float&	length = m_item.length;
	bool&	closed = m_item.closed;
	int&	numSegments = m_item.numSegments;
	numSegments = kTapeMeasure::doMeasure_s(
		meshPtr,

		mode,
		startDefPtr,

		// [OUT]
		NULL,
		segmentPointArray, length, closed,
		twoPointArray, convexPointArray
	);
#if 1	// manipulator; LINE MANIPULATOR
	meshPtr->tapeMeasure().isReleased() = false;
#endif	// manipulator; LINE MANIPULATOR
	meshPtr->tapeMeasure().length() = -1.f;	// 임의계측에서 참고할 수 있도록 설정
#ifdef _DEBUG
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
#endif
	meshPtr->SetSegmentBuffer(
		m_item.segmentVertexBuffer, m_item.segmentIndexBuffer,
		mode == TAPE_MEASURE_MODE_CONTOUR ?
		(startDefPtr.type == START_DEF_PTR_TYPE__GIRTH ?
			segmentPointArray :
			twoPointArray
		) :
			convexPointArray,
		closed
	);
#endif	// normal_manipulator_done_callback_manual_measure 와 비슷
}
#endif	// point manipulator_callback
#endif

#if 1	// ShowLowerFace
D3DVERTEXELEMENT9	kMesh::LowerFaceElement[5] = {
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
	{
		0,
		2 * sizeof(D3DXVECTOR3),
		D3DDECLTYPE_FLOAT2,
		D3DDECLMETHOD_DEFAULT,
		D3DDECLUSAGE_TEXCOORD,
		0
	},
	{
		0,
		3 * sizeof(D3DXVECTOR3),
		D3DDECLTYPE_FLOAT3,
		D3DDECLMETHOD_DEFAULT,
		D3DDECLUSAGE_COLOR,
		0
	},
	D3DDECL_END()
};

void	kMesh::ShowLowerFace(
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
		const CFace&	face = __facePtr[faceIDArray.at(n)];
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
	__devicePtr->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);	// 내부도 보게 한다.
{
	ShowFace(faceIndexBufferPtr, nFace, SHADING_TEXTURE);//, D3DFILL_POINT, D3DXVECTOR4(0, 1, 1, 1));
}
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

void	kMesh::ShowLower2Face(
	const std::vector<D3DXVECTOR3>&	faceIDArray,
	const D3DXMATRIX&	mWorld,
	const D3DXMATRIX&	mWorldViewProjection
) const
{
	if (!faceIDArray.size())
		return;
	// faceIDArray 를 이용하여...
	LPDIRECT3DVERTEXDECLARATION9	faceDeclarationPtr = 0;
	LPDIRECT3DVERTEXBUFFER9		faceVertexBufferPtr = 0;
__try {
	HRESULT	hResult;

	hResult = __devicePtr->CreateVertexDeclaration(Element, &faceDeclarationPtr);
	hResult = __devicePtr->CreateVertexBuffer(
		(UINT) faceIDArray.size() * sizeof(D3DXVECTOR3),
		D3DUSAGE_WRITEONLY,
		0,
		D3DPOOL_MANAGED,
		&faceVertexBufferPtr,
		NULL
	);
	D3DXVECTOR3*	vertexArray;
	faceVertexBufferPtr->Lock(0, 0, (void**) &vertexArray, 0);
	for (int i = 0; i < (int) faceIDArray.size(); ++i) {
		vertexArray[i] = faceIDArray.at(i);
	}
	faceVertexBufferPtr->Unlock();

	DWORD	cullMode;
	__devicePtr->GetRenderState(D3DRS_CULLMODE, &cullMode);
	__devicePtr->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
{
	__effectPtr->SetInt("g__texture", SHADING_GREY);
#if 0
	__effectPtr->SetInt("g__FillMode", D3DFILL_SOLID);
#else	// 단색
	__effectPtr->SetInt("g__FillMode", D3DFILL_POINT);
	__effectPtr->SetVector("pointColor", &D3DXVECTOR4(1, 0, 1, 1));
#endif
	__effectPtr->SetMatrix("g_mWorldViewProjection", &mWorldViewProjection);
	__effectPtr->SetMatrix("g_mWorld", &mWorld);
	D3DXHANDLE	hTechnique = __effectPtr->GetTechnique(0);
	assert(hTechnique);
	hResult = __effectPtr->SetTechnique(hTechnique);
	assert(S_OK == hResult);
	UINT	NumberOfPasses;
	hResult = __effectPtr->Begin(&NumberOfPasses, 0);
	assert(D3D_OK == hResult);
	for (UINT iPass = 0; iPass < NumberOfPasses; ++iPass) {
		hResult = __effectPtr->BeginPass(iPass);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetVertexDeclaration(faceDeclarationPtr);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetStreamSource(0, faceVertexBufferPtr, 0, sizeof(D3DXVECTOR3));
		assert(D3D_OK == hResult);
		hResult = __devicePtr->DrawPrimitive(D3DPT_TRIANGLELIST, 0, (UINT) faceIDArray.size() / 3);
		assert(D3D_OK == hResult);
		hResult = __effectPtr->EndPass();
		assert(S_OK == hResult);
	}
	hResult = __effectPtr->End();
	assert(D3D_OK == hResult);
	__devicePtr->SetRenderState(D3DRS_CULLMODE, cullMode);
}
} __finally {
	if (faceDeclarationPtr) {
		faceDeclarationPtr->Release();
		faceDeclarationPtr = 0;
	}
	if (faceVertexBufferPtr) {
		faceVertexBufferPtr->Release();
		faceVertexBufferPtr = 0;
	}
}
}

void	kMesh::ShowLower2Face(
	const std::vector<LOWER_FACE>&	lowerFaceArray,
	const D3DXMATRIX&	mWorld,
	const D3DXMATRIX&	mWorldViewProjection
) const
{
	if (!lowerFaceArray.size())
		return;
	// lowerFaceArray 를 이용하여...
	LPDIRECT3DVERTEXDECLARATION9	lowerFaceDeclarationPtr = 0;
	LPDIRECT3DVERTEXBUFFER9		lowerFaceVertexBufferPtr = 0;
__try {
	HRESULT	hResult;

	hResult = __devicePtr->CreateVertexDeclaration(LowerFaceElement, &lowerFaceDeclarationPtr);
	hResult = __devicePtr->CreateVertexBuffer(
		(UINT) lowerFaceArray.size() * sizeof(LOWER_FACE),
		D3DUSAGE_WRITEONLY,
		0,
		D3DPOOL_MANAGED,
		&lowerFaceVertexBufferPtr,
		NULL
	);
	LOWER_FACE*	vertexArray;
	lowerFaceVertexBufferPtr->Lock(0, 0, (void**) &vertexArray, 0);
	for (int i = 0; i < (int) lowerFaceArray.size(); ++i) {
		vertexArray[i] = lowerFaceArray.at(i);
	}
	lowerFaceVertexBufferPtr->Unlock();

	DWORD	cullMode;
	__devicePtr->GetRenderState(D3DRS_CULLMODE, &cullMode);
	__devicePtr->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
{
	__effectPtr->SetInt("g__texture", SHADING_GREY);
#if 0
	__effectPtr->SetInt("g__FillMode", D3DFILL_SOLID);
#else	// 단색
	__effectPtr->SetInt("g__FillMode", D3DFILL_POINT);
	__effectPtr->SetVector("pointColor", &D3DXVECTOR4(1, 0, 1, 1));
#endif
	__effectPtr->SetMatrix("g_mWorldViewProjection", &mWorldViewProjection);
	__effectPtr->SetMatrix("g_mWorld", &mWorld);
	D3DXHANDLE	hTechnique = __effectPtr->GetTechnique(0);
	assert(hTechnique);
	hResult = __effectPtr->SetTechnique(hTechnique);
	assert(S_OK == hResult);
	UINT	NumberOfPasses;
	hResult = __effectPtr->Begin(&NumberOfPasses, 0);
	assert(D3D_OK == hResult);
	for (UINT iPass = 0; iPass < NumberOfPasses; ++iPass) {
		hResult = __effectPtr->BeginPass(iPass);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetVertexDeclaration(lowerFaceDeclarationPtr);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetStreamSource(0, lowerFaceVertexBufferPtr, 0, sizeof(LOWER_FACE));
		assert(D3D_OK == hResult);
		hResult = __devicePtr->DrawPrimitive(D3DPT_TRIANGLELIST, 0, (UINT) lowerFaceArray.size() / 3);
		assert(D3D_OK == hResult);
		hResult = __effectPtr->EndPass();
		assert(S_OK == hResult);
	}
	hResult = __effectPtr->End();
	assert(D3D_OK == hResult);
	__devicePtr->SetRenderState(D3DRS_CULLMODE, cullMode);
}
} __finally {
	if (lowerFaceDeclarationPtr) {
		lowerFaceDeclarationPtr->Release();
		lowerFaceDeclarationPtr = 0;
	}
	if (lowerFaceVertexBufferPtr) {
		lowerFaceVertexBufferPtr->Release();
		lowerFaceVertexBufferPtr = 0;
	}
}
}
#endif	// ShowLowerFace

#if 1	// 2D단면
// 임의계측(kTapeMeasure::doMeasure_s) 후 바로 이 함수를 호출해서 2D단면에 필요한 Vertex Buffer 설정
void	kMesh::set2dProjectVertexBuffer()
{
	D3DXVECTOR3*	pointPtr = 0;
do {
	bool	closed = __statePtr->g__stringClosed;
	int	numSegments;
	int	numPoints;
{
	LPDIRECT3DVERTEXBUFFER9		segmentVertexBufferPtr;
	if (__tapeMeasure.getModeRef() == TAPE_MEASURE_MODE_CONTOUR) {	// 단면
//		(void) fprintf(stdout, "TAPE_MEASURE_MODE_CONTOUR\n");
		if (__tapeMeasure.getMethodRef() == TAPE_MEASURE_METHOD_GIRTH) {	// 둘레
//			(void) fprintf(stdout, "TAPE_MEASURE_METHOD_GIRTH\n");
			segmentVertexBufferPtr = __segmentVertexBuffer;
			numSegments = __numSegments;
			numPoints = closed ? __numSegments : __numSegments + 1;
		} else {
//			(void) fprintf(stdout, "TAPE_MEASURE_METHOD_TWO_POINT\n");
			segmentVertexBufferPtr = __tapeMeasure.twoPointVertexBufferPtr();
			numPoints = __tapeMeasure.twoPointArray().GetSize();
			numSegments = closed ? numPoints : numPoints - 1;
		}
	} else {
//		(void) fprintf(stdout, "TAPE_MEASURE_MODE_WRAP\n");
		numPoints = __tapeMeasure.convexPointArray().GetSize();
		numSegments = closed ? numPoints : numPoints - 1;
		segmentVertexBufferPtr = __tapeMeasure.convexVertexBufferPtr();
	}
	if (numPoints < 2)
		return;

	// 표시할 점들은...
	pointPtr = new D3DXVECTOR3[numPoints];
	SEGMENT_VERTEX*	vertexArray;
	segmentVertexBufferPtr->Lock(0, 0, (void**) &vertexArray, 0);
	for (int n = 0; n < numPoints; ++n)
		pointPtr[n] = vertexArray[n].P;
	segmentVertexBufferPtr->Unlock();
	(void) fprintf(stdout, "numPoints = %d, numSegments = %d\n", numPoints, numSegments);
}

	// 각 점들을, 각 점들의 중심을 원점으로 하는, 절단면의 수직인 2차원 평면상의 점들로 변환한다;
	// 각 점들의 x, y축에 대한 방향코사인과 중심에서 각 점까지의 거리 ell 만 알면 된다.
	D3DXVECTOR3	center(0, 0, 0);	// 중심

	D3DXVECTOR3	nDir;	// 절단면을 결정하는 법선벡터
	D3DXVECTOR3	xDir;	// 기준축
	const START_DEF&	startDef = __tapeMeasure.startDef();
	if (startDef.type == START_DEF_TYPE__GIRTH) {
		nDir = startDef.plane_def.normal;

		// z축(0, 0, 1)을 nDir 로 바꾸는 회전이동에 대해
		// 기준축(1, 0, 0)이 어떻게 바뀌는 지 알아본다.
		D3DXVECTOR3	zDir0(0, 0, 1);
		float		fDot = D3DXVec3Dot(&zDir0, &nDir);	// 두 벡터사이의 각도
		D3DXVECTOR3	axis;	// zDir0 을 nDir 로 옮길 때 사용하는 회전축
		D3DXVec3Cross(&axis, &zDir0, &nDir);
		D3DXMATRIX	m;	// 이 회전에 해당하는 행렬
		D3DXMatrixRotationAxis(&m, &axis, acosf(fDot));
		D3DXVECTOR3	xDir0(1, 0, 0);
		D3DXVec3TransformCoord(&xDir, &xDir0, &m);
		// 원래는 D3DXVec3TransformNormal 을 사용해야 하나,
		// 회전만 있는 경우에는 결과가 같다.
		// D3DXVec3TransformNormal 를 호출하면 주어진 행렬의 역행렬을 구한 후
		// 다시 Transpose 를 계산하므로 시간이 더 걸린다.

		for (int n = 0; n < numPoints; ++n)
			center += pointPtr[n];
		center /= (float) numPoints;
	} else if (startDef.type == START_DEF_TYPE__TWO_POINT) {
		const TWO_POINT_PLANE_DEF&	_2p_planeDef = startDef.two_point_plane_def;

		nDir = _2p_planeDef.normal;

		D3DXVECTOR3	p0, p1;
		getPoint(_2p_planeDef.point0Def, p0);
		getPoint(_2p_planeDef.point1Def, p1);
		xDir = p1 - p0;
		D3DXVec3Normalize(&xDir, &xDir);

		center = .5f * (p0 + p1);
	} else {
		assert(0);
	}
	D3DXVECTOR3	yDir;
	D3DXVec3Cross(&yDir, &xDir, &nDir);	// 2D단면을 볼 때는 오른손 좌표계;
						// 화면에서 나오는 방향이 z축이라고 했을 때,
						// 오른쪽이 x축, 위쪽이 y축
#if 0
	(void) fprintf(stdout,
		"xDir: [%f, %f, %f], yDir: [%f, %f, %f], nDir: [%f, %f, %f]\n"
		,
		xDir.x, xDir.y, xDir.z,
		yDir.x, yDir.y, yDir.z,
		nDir.x, nDir.y, nDir.z
	);
#endif

	HRESULT	hResult;
	// 두 Vertex Buffer(__2dProjectVertexBufferPtr, __2dInterpolatedProjectVertexBufferPtr)를 설정
	SAFE_RELEASE(__2dProjectVertexBufferPtr);
	hResult = __devicePtr->CreateVertexBuffer(
		numPoints * sizeof(D3DXVECTOR3),	// 한 세그먼트당 10개의 점으로 채운다.
		D3DUSAGE_WRITEONLY,
		0,
		D3DPOOL_MANAGED,
		&__2dProjectVertexBufferPtr,
		NULL
	);
	assert(D3D_OK == hResult);
	SAFE_RELEASE(__2dInterpolatedProjectVertexBufferPtr);
	const int	NUM_INTERPOLATED = 50;	// 각 점들 사이를 NUM_INTERPOLATED 개의 점들로 채운다.
	hResult = __devicePtr->CreateVertexBuffer(
		NUM_INTERPOLATED * numSegments * sizeof(D3DXVECTOR3),	// 한 세그먼트당 10개의 점으로 채운다.
		D3DUSAGE_WRITEONLY,
		0,
		D3DPOOL_MANAGED,
		&__2dInterpolatedProjectVertexBufferPtr,
		NULL
	);
	assert(D3D_OK == hResult);
	SAFE_RELEASE(__2dFillProjectVertexBufferPtr);
	const int	NUM_FILL = 100;	// START_DEF_PTR_TYPE__TWO_POINT 의 경우 양 끝점 사이를 NUM_FILL 개의 점들로 채운다.
	hResult = __devicePtr->CreateVertexBuffer(
		NUM_FILL * sizeof(D3DXVECTOR3),
		D3DUSAGE_WRITEONLY,
		0,
		D3DPOOL_MANAGED,
		&__2dFillProjectVertexBufferPtr,
		NULL
	);
	assert(D3D_OK == hResult);

	D3DXVECTOR3*	contourPointPtr = 0;
	__2dProjectVertexBufferPtr->Lock(0, 0, (void**) &contourPointPtr, 0);
	D3DXVECTOR3*	interpolatedPointPtr = 0;
	__2dInterpolatedProjectVertexBufferPtr->Lock(0, 0, (void**) &interpolatedPointPtr, 0);
	D3DXVECTOR3*	fillPointPtr = 0;
	__2dFillProjectVertexBufferPtr->Lock(0, 0, (void**) &fillPointPtr, 0);

	// 점들을 구성하는 박스의 크기 결정
	float	x_max = -FLT_MAX;
	float	x_min = FLT_MAX;
	float	y_max = -FLT_MAX;
	float	y_min = FLT_MAX;
	for (int n = 0; n < numPoints; ++n) {
		D3DXVECTOR3	r = pointPtr[n] - center;
		D3DXVECTOR3	r0; D3DXVec3Normalize(&r0, &r);
		float	ell = D3DXVec3Length(&r);
		float	cosine1 = D3DXVec3Dot(&r0, &xDir);
		float	cosine2 = D3DXVec3Dot(&r0, &yDir);
		D3DXVECTOR3&	v = contourPointPtr[n];
		v.x = ell * cosine1;
		v.y = ell * cosine2;
		if (v.x > x_max)
			x_max = v.x;
		if (v.x < x_min)
			x_min = v.x;
		if (v.y > y_max)
			y_max = v.y;
		if (v.y < y_min)
			y_min = v.y;
		v.z = 0;
	}
	const float	box_a = .4f;
	const float	offset = .05f;
	// Upper Left: (-1, 1)
	// Lower Right: (1, -1)
	float	scale;	// W x H 박스가 한 변의 길이가 box_a 인 정사각형 박스 안에 들어가도록
	// center 가 박스의 중앙이 아니다.
	if (x_max - x_min > y_max - y_min) {	// W > H; W 가 box_a 에 들어가도록
		scale = box_a / (x_max - x_min);
	} else {	// H > W; H 가 box_a 에 들어가도록
		scale = box_a / (y_max - y_min);
	}
	const float	box_center_x = (1.f - box_a / 2) - offset;
	const float	box_center_y = -(1.f - box_a / 2) + offset;
	(void) fprintf(stdout, "W = %f ([%f, %f]), H = %f ([%f, %f])\n", x_max - x_min, x_min, x_max, y_max - y_min, y_min, y_max);
	D3DVIEWPORT9	viewport;
	DXUTGetD3D9Device()->GetViewport(&viewport);
	float	aspect_ratio = (FLOAT) viewport.Width / viewport.Height;
	float	shift_center_x = .5f * (x_min + x_max);
	float	shift_center_y = .5f * (y_min + y_max);
	for (int n = 0; n < numPoints; ++n) {
		D3DXVECTOR3&	v = contourPointPtr[n];
		v.x -= shift_center_x; v.x *= scale / aspect_ratio; v.x += box_center_x;
		v.y -= shift_center_y; v.y *= scale; v.y += box_center_y;
	}
	__2dProjectNumPoints = numPoints;

	// Catmull-Rom interpolation 에 의해 각 세그먼트를 NUM_INTERPOLATED 개의 점들로 채운다.
	// Catmull-Rom Interpolation 은 주어진 네 점(P1, P2, P3, P4)를 이용하여
	// P2, P3 사이에서 부드러운 곡선을 만든다;
	// P1(P4) 가 없을 때는 P2(P3) 를 사용
	for (int n = 0; n < numSegments; ++n) {
		// n 번째 segment 즉 n 번째 점과 (n + 1)번째 점 사이를 부드러운 점들로 채운다;
		D3DXVECTOR3*	P0Ptr;
		D3DXVECTOR3*	P1Ptr;
		D3DXVECTOR3*	P2Ptr;
		D3DXVECTOR3*	P3Ptr;

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

		// P1, P2 사이를 NUM_INTERPOLATED 개의 점들로 채운다; (1 + NUM_INTERPOLATED) 개의 세그먼트를 만든다.
		for (int m = 0; m < NUM_INTERPOLATED; ++m) {
			FLOAT	s = 1.f / (1 + NUM_INTERPOLATED) * (1 + m);
			D3DXVec3CatmullRom(&interpolatedPointPtr[NUM_INTERPOLATED * n + m], P0Ptr, P1Ptr, P2Ptr, P3Ptr, s);
		}
	}
	__2dInterpolatedProjectNumPoints = NUM_INTERPOLATED * numSegments;

	if (__tapeMeasure.getMethodRef() == TAPE_MEASURE_METHOD_TWO_POINT) {
		__2dFillProjectNumPoints = NUM_FILL;

		D3DXVECTOR3	p0, p1;
		p0 = contourPointPtr[0];
		p1 = contourPointPtr[numPoints - 1];
		for (int n = 0; n < NUM_FILL; ++n) {
			FLOAT	s = 1.f / (1 + NUM_FILL) * (1 + n);
			D3DXVec3CatmullRom(&fillPointPtr[n], &p0, &p0, &p1, &p1, s);
		}
	} else
		__2dFillProjectNumPoints = 0;

	__2dFillProjectVertexBufferPtr->Unlock();
	__2dInterpolatedProjectVertexBufferPtr->Unlock();
	__2dProjectVertexBufferPtr->Unlock();
} while (0);
	if (pointPtr) {
		delete[] pointPtr;
		pointPtr = 0;
	}
}
#endif	// 2D단면

#if 1	// Save/Restore Color
void	kMesh::SaveColor()
{
	D3DXVERTEX*	vertexPtr;
	__vertexBufferPtr->Lock(0, 0, (void**) &vertexPtr, 0);
	for (DWORD i = 0; i < __numPointReprs; ++i) {
		D3DXVERTEX&	v = vertexPtr[i];
		v.c_ = v.c;
	}
	__vertexBufferPtr->Unlock();
}
void	kMesh::RestoreColor()
{
	D3DXVERTEX*	vertexPtr;
	__vertexBufferPtr->Lock(0, 0, (void**) &vertexPtr, 0);
	for (DWORD i = 0; i < __numPointReprs; ++i) {
		D3DXVERTEX&	v = vertexPtr[i];
		v.c = v.c_;
	}
	__vertexBufferPtr->Unlock();
}
#endif	// Save/Restor Color

#if 1	// CollectFaces
// PLANE_DEF 에 결정되는 평면의 아래쪽에 있는 Face 들의 목록을 얻는다;
// 아래의 __CollectLowerFaces 함수를 반복적으로 호출한다:
//	planeDef 로부터 startFaceID 를 결정한후,
//	nextFaceIDArray 에 있는 Face 의 갯수가 0이 될 때까지
//	__CollectLowerFaces 함수를 반복적으로 호출한다.
void
kMesh::CollectLowerFaces(
	const DWORD*		faceIDArray0,		// [IN]; 이 Face 들 중에서
							//	이 값이 NULL 이면 메쉬에 있는 모든 Face 들을 조사한다.
	const PLANE_DEF&	planeDef,		// [IN]; 이 평면에 대해 아래쪽에 있는
	std::vector<DWORD>&	lowerFaceIDArray,	// [OUT]; Face 들의 목록

	const kMesh*		meshPtr			// [IN]; 메쉬의 Vertex, Face 들의 정보을 참고해서...
)
{
	DWORD*	faceIDArray = 0;
	do {
		faceIDArray = new DWORD[meshPtr->GetNumFaces()];
		FillMemory(faceIDArray, meshPtr->GetNumFaces() * sizeof(DWORD), 0);

		CollectLowerFaces(faceIDArray0, planeDef, faceIDArray, meshPtr);

		lowerFaceIDArray.resize(0);
		for (int n = 0; n < (int) meshPtr->GetNumFaces(); ++n) {
			if (faceIDArray[n])
				lowerFaceIDArray.push_back(n);
		}
	} while (0);
	if (faceIDArray) {delete[] faceIDArray; faceIDArray = 0;}
}
void
kMesh::CollectLowerFaces(
	const DWORD*		faceIDArray0,		// [IN]; 이 Face 들 중에서
							//	이 값이 NULL 이면 메쉬에 있는 모든 Face 들을 조사한다.
	const PLANE_DEF&	planeDef,		// [IN]; 이 평면에 대해 아래쪽에 있는
	DWORD*			lowerFaceIDArray,	// [OUT]; Face 들의 목록

	const kMesh*		meshPtr			// [IN]; 메쉬의 Vertex, Face 들의 정보을 참고해서...
)
{
	D3DXVECTOR3	P;				// 평면위의 한 점
	if (meshPtr->getPoint(planeDef.pointDef, P) < 0) {
		cerr << "kMesh::CollectLowerFaces: kMesh::getPoint(): Failure" << endl;
		return;
	}
	const D3DXVECTOR3&	nDir = planeDef.normal;	// 평면을 결정하는 법선벡터

	const CPointRepr*	pointReprArray = meshPtr->GetPointReprPointer();
	const CFace*		faceArray = meshPtr->GetFacePointer();

	// 단면에 의해 잘린 면이 이 점을 통과하는 면만 있는 것이 아니다; TO DO LIST
	// 1. 시작할 꼭지점을 정한다: startVertexID
	DWORD	startFaceID = planeDef.pointDef.ID;
	const CFace&	startFace = faceArray[startFaceID];
	DWORD	startVertexID = (DWORD) -1;
	for (int k = 0; k < 3; ++k) {
		const CPointRepr&	pointRepr = pointReprArray[startFace.prID[k]];
		if (D3DXVec3Dot(&nDir, &D3DXVECTOR3(pointRepr.v.position - P)) < 0) {	// nDir 반대방향(아래쪽)
			startVertexID = startFace.prID[k];
			break;
		}
	}
	assert(startVertexID != -1);
//	cout << "kMesh::CollectLowerFaces: startFaceID = " << startFaceID << endl;
//	cout << "kMesh::CollectLowerFaces: startVertexID = " << startVertexID << endl;

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

		std::vector<DWORD>	nextFaceIDArray;
		nextFaceIDArray.push_back(startFaceID);
		while (nextFaceIDArray.size()) {
			std::vector<DWORD>	nextFaceIDArray_tmp;
			for (int n = 0; n < (int) nextFaceIDArray.size(); ++n) {
				kMesh::__CollectLowerFaces(
					faceIDArray0,
					P, nDir,
					lowerFaceIDArray,
					meshPtr,
					nextFaceIDArray.at(n),
					nextFaceIDArray_tmp,
					donePrIDArray,
					doneFaceIDArray
				);
			}
			nextFaceIDArray = nextFaceIDArray_tmp;
		}
	} while (0);
	if (doneFaceIDArray) {delete[] doneFaceIDArray; doneFaceIDArray = 0;}
	if (donePrIDArray) {delete[] donePrIDArray; donePrIDArray = 0;}
}

void
kMesh::__CollectLowerFaces(
	const DWORD*		faceIDArray0,		// [IN]; 이 Face 들 중에서
							//	이 값이 NULL 이면 메쉬에 있는 모든 Face 들을 조사한다.
	const D3DXVECTOR3&	P,			// [IN]; (P, nDir) 에 의해 결정되는 평면에 대해 아래쪽에 있는
	const D3DXVECTOR3&	nDir,
	DWORD*			lowerFaceIDArray,	// [OUT]; Face 들의 목록

	const kMesh*		meshPtr,		// [IN]; 메쉬의 Vertex, Face 들의 정보을 참고해서
	DWORD	startFaceID,				// [IN]; 아래쪽에 있는 이 Face 에서 시작해서
	std::vector<DWORD>&	nextFaceIDArray,	// [OUT]; 아래쪽에 있는 다음 Face 들의 목록을 구한다.
	DWORD*			donePrIDArray,		// [IN/OUT]; 이미 테스트를 끝낸 Vertex 들의 목록과
	DWORD*			doneFaceIDArray		// [IN/OUT]; Face 들의 목록을 갱신한다.
)
{
//	cout << "__CollectLowerFaces: startFaceID = " << startFaceID << endl;
	const CPointRepr*	pointReprArray = meshPtr->GetPointReprPointer();
	const CFace*		faceArray = meshPtr->GetFacePointer();

#ifdef __USE_A_OFFSET__
//	const DWORD*	pointer_to__a_vVertexID				= meshPtr->pointer_to__a_vVertexID;
//	const int*	pointer_to__a_vVertexID_a_offset		= meshPtr->pointer_to__a_vVertexID_a_offset;
//	const EDGE*	pointer_to__a_vEdgeID				= meshPtr->pointer_to__a_vEdgeID;
//	const int*	pointer_to__a_vEdgeID_a_offset			= meshPtr->pointer_to__a_vEdgeID_a_offset;
	const FACE_ID_LOCAL_INDEX*	pointer_to__a_vFaceIDLocalIndex	= meshPtr->pointer_to__a_vFaceIDLocalIndex;
	const int*	pointer_to__a_vFaceIDLocalIndex_a_offset	= meshPtr->pointer_to__a_vFaceIDLocalIndex_a_offset;
//	const DWORD*	pointer_to__a_faceID				= meshPtr->pointer_to__a_faceID;
//	const int*	pointer_to__a_faceID_a_offset			= meshPtr->pointer_to__a_faceID_a_offset;
#endif	// __USE_A_OFFSET__
	const CFace&	startFace = faceArray[startFaceID];
	for (int i = 0; i < 3; ++i) {
		DWORD	prID = startFace.prID[i];
		if (donePrIDArray[prID]) {
//			cout << "prID = " << prID << ": Pass" << endl;
			continue;
		}
//		cout << "prID = " << prID << ": ..." << endl;

		// Vertex 를 공유하는 면들을 조사한다.
		// 이 Vertex 는 조사가 끝났음을 표시한다.
		donePrIDArray[prID] = 1;
		const CPointRepr&	pointRepr = pointReprArray[prID];
		// 이 Vertex 가 위쪽에 있는 거라면 더 이상 조사할 필요가 없다.
		if (D3DXVec3Dot(&nDir, &D3DXVECTOR3(pointRepr.v.position - P)) > 0)	// nDir 방향
			continue;
#ifdef __USE_A_OFFSET__
		for (int n = pointer_to__a_vFaceIDLocalIndex_a_offset[prID]; n < pointer_to__a_vFaceIDLocalIndex_a_offset[prID + 1]; ++n) {
			const FACE_ID_LOCAL_INDEX&	faceIDLocalIndex = pointer_to__a_vFaceIDLocalIndex[n];
#else
		for (int n = 0; n < (int) pointRepr.vFaceIdLocalIndex.size(); ++n) {
			const FACE_ID_LOCAL_INDEX&	faceIDLocalIndex = pointRepr.vFaceIdLocalIndex.at(n);
#endif
			DWORD	faceID = faceIDLocalIndex.ID;
			// 조사할 목록에 들어있지 않으면 통과
			if (faceIDArray0 && faceIDArray0[faceID] == 0) {
//				cout << "faceID = " << faceID << ": Not in a list: Pass" << endl;
				continue;
			}
			// 이미 조사가 끝났다면 통과
			if (doneFaceIDArray[faceID]) {
//				cout << "faceID = " << faceID << ": In a done list: Pass" << endl;
				continue;
			}
//			cout << "faceID = " << faceID << ": ..." << endl;

			// 이 Face 를 조사한다.
			const CFace&	face = faceArray[faceID];
			bool	isLower = true;
			for (int k = 0; k < 3; ++k) {
				const CPointRepr&	pointRepr = pointReprArray[face.prID[k]];
				if (D3DXVec3Dot(&nDir, &D3DXVECTOR3(pointRepr.v.position - P)) > 0) {	// nDir 방향에 있다.
					isLower = false;
					break;
				}
			}
//			cout << "isLower? " << (isLower ? "True" : "False") << endl;
			if (isLower) {
				lowerFaceIDArray[faceID] = 1;
				nextFaceIDArray.push_back(faceID);
			}

			// 이 Face 는 조사가 끝났음을 표시하고,
			doneFaceIDArray[faceID] = 1;
#ifdef __USE_A_OFFSET__
		}
#else	// __USE_A_OFFSET__/!__USE_A_OFFSET__
		}
#endif	// !__USE_A_OFFSET__
	}
}

void
kMesh::CollectBetweenFaces(
	const PLANE_DEF&	planeDef,		// [IN]; 이 평면과
	const POINT_DEF&	pointDef,		// [IN]; 이 점에 첫 번째 평면의 법선벡터에 의해서 결정되는 평면사이에 있는
	std::vector<DWORD>&	betweenFaceIDArray,	// [OUT]; Face 들의 목록

	const kMesh*		meshPtr			// [IN]; 메쉬의 Vertex, Face 들의 정보을 참고해서...
)
{
	DWORD*	faceIDArray = 0;
	do {
		faceIDArray = new DWORD[meshPtr->GetNumFaces()];
		FillMemory(faceIDArray, meshPtr->GetNumFaces() * sizeof(DWORD), 0);

		CollectBetweenFaces(planeDef, pointDef, faceIDArray, meshPtr);

		betweenFaceIDArray.resize(0);
		for (int n = 0; n < (int) meshPtr->GetNumFaces(); ++n) {
			if (faceIDArray[n])
				betweenFaceIDArray.push_back(n);
		}
	} while (0);
	if (faceIDArray) {delete[] faceIDArray; faceIDArray = 0;}
}
void
kMesh::CollectBetweenFaces(
	const PLANE_DEF&	planeDef,		// [IN]; 이 평면과
	const POINT_DEF&	pointDef,		// [IN]; 이 점에 첫 번째 평면의 법선벡터에 의해서 결정되는 평면사이에 있는
	DWORD*			betweenFaceIDArray,	// [OUT]; Face 들의 목록

	const kMesh*		meshPtr			// [IN]; 메쉬의 Vertex, Face 들의 정보을 참고해서...
)
{
	D3DXVECTOR3	P0;
	meshPtr->getPoint(pointDef, P0);

	D3DXVECTOR3	P1;
	meshPtr->getPoint(planeDef.pointDef, P1);
	const D3DXVECTOR3&	nDir = planeDef.normal;

	// P0 가 아래쪽에 있도록 normal 를 설정한다;
	// P 에서 P0 에 이르는 벡터와 nDir 가 같은 방향이면 normal 은 -nDir,
	// 그렇지 않다면 nDir 이다.
	D3DXVECTOR3	v = P0 - P1;
	D3DXVec3Normalize(&v, &v);

	DWORD*	lowerFaceIDArray = 0;
	DWORD*	lowerFaceIDArray0 = 0;
	do {
		lowerFaceIDArray = new DWORD[meshPtr->GetNumFaces()];
		FillMemory(lowerFaceIDArray, meshPtr->GetNumFaces() * sizeof(DWORD), 0);
		PLANE_DEF	planeDef1;
		planeDef1.pointDef = planeDef.pointDef;
		planeDef1.normal = D3DXVec3Dot(&v, &nDir) > 0 ? -nDir : nDir;
		kMesh::CollectLowerFaces(NULL, planeDef1, lowerFaceIDArray, meshPtr);

		lowerFaceIDArray0 = new DWORD[meshPtr->GetNumFaces()];
		FillMemory(lowerFaceIDArray0, meshPtr->GetNumFaces() * sizeof(DWORD), 0);
		PLANE_DEF	planeDef0;
		planeDef0.pointDef = pointDef;
		planeDef0.normal = D3DXVec3Dot(&v, &nDir) > 0 ? nDir : -nDir;
		kMesh::CollectLowerFaces(lowerFaceIDArray, planeDef0, lowerFaceIDArray0, meshPtr);

		CopyMemory(betweenFaceIDArray, lowerFaceIDArray0, meshPtr->GetNumFaces() * sizeof(DWORD));
	} while (0);
	if (lowerFaceIDArray) {delete[] lowerFaceIDArray; lowerFaceIDArray = 0;}
	if (lowerFaceIDArray0) {delete[] lowerFaceIDArray0; lowerFaceIDArray0 = 0;}
}

void
kMesh::CollectBetweenFaces(
	const PLANE_DEF&	planeDef0,		// [IN]; 이 평면과
	const PLANE_DEF&	planeDef1,		// [IN]; 이 평면 사이에 있는
	std::vector<DWORD>&	betweenFaceIDArray,	// [OUT]; Face 들의 목록

	const kMesh*		meshPtr			// [IN]; 메쉬의 Vertex, Face 들의 정보을 참고해서...
)
{
	DWORD*	faceIDArray = 0;
	do {
		faceIDArray = new DWORD[meshPtr->GetNumFaces()];
		FillMemory(faceIDArray, meshPtr->GetNumFaces() * sizeof(DWORD), 0);

		CollectBetweenFaces(planeDef0, planeDef1, faceIDArray, meshPtr);

		betweenFaceIDArray.resize(0);
		for (int n = 0; n < (int) meshPtr->GetNumFaces(); ++n) {
			if (faceIDArray[n])
				betweenFaceIDArray.push_back(n);
		}
	} while (0);
	if (faceIDArray) {delete[] faceIDArray; faceIDArray = 0;}
}
void
kMesh::CollectBetweenFaces(
	const PLANE_DEF&	planeDef0,		// [IN]; 이 평면과
	const PLANE_DEF&	planeDef1,		// [IN]; 이 평면 사이에 있는
	DWORD*			betweenFaceIDArray,	// [OUT]; Face 들의 목록

	const kMesh*		meshPtr			// [IN]; 메쉬의 Vertex, Face 들의 정보을 참고해서...
)
{
	D3DXVECTOR3	P0;
	meshPtr->getPoint(planeDef0.pointDef, P0);

	D3DXVECTOR3	P1;
	meshPtr->getPoint(planeDef1.pointDef, P1);

	D3DXVECTOR3	v = P1 - P0;
	D3DXVec3Normalize(&v, &v);
	D3DXVECTOR3	nDir;

	DWORD*	lowerFaceIDArray0 = 0;
	DWORD*	lowerFaceIDArray1 = 0;
	do {
		lowerFaceIDArray0 = new DWORD[meshPtr->GetNumFaces()];
		FillMemory(lowerFaceIDArray0, meshPtr->GetNumFaces() * sizeof(DWORD), 0);
		PLANE_DEF	p0;
		p0.pointDef = planeDef0.pointDef;
		nDir = planeDef0.normal;
		p0.normal = D3DXVec3Dot(&v, &nDir) > 0 ? -nDir : nDir;
		kMesh::CollectLowerFaces(NULL, p0, lowerFaceIDArray0, meshPtr);

		lowerFaceIDArray1 = new DWORD[meshPtr->GetNumFaces()];
		FillMemory(lowerFaceIDArray1, meshPtr->GetNumFaces() * sizeof(DWORD), 0);
		PLANE_DEF	p1;
		p1.pointDef = planeDef1.pointDef;
		nDir = planeDef1.normal;
		p1.normal = D3DXVec3Dot(&v, &nDir) > 0 ? nDir : -nDir;
		kMesh::CollectLowerFaces(lowerFaceIDArray0, p1, lowerFaceIDArray1, meshPtr);

		CopyMemory(betweenFaceIDArray, lowerFaceIDArray1, meshPtr->GetNumFaces() * sizeof(DWORD));
	} while (0);
	if (lowerFaceIDArray0) {delete[] lowerFaceIDArray0; lowerFaceIDArray0 = 0;}
	if (lowerFaceIDArray1) {delete[] lowerFaceIDArray1; lowerFaceIDArray1 = 0;}
}
#endif	// CollectFaces

#if 1	// 모든 계측부위 표시
void	kMesh::SetAllMeasureDirty(const int* valPtr)
{
	// 1. 먼저 설정한다; 아래의 CTRL + '2'
	int	val = 0;
	if (valPtr)
		val = *valPtr;
	else {
		// 모든 부위를 표시한다.
		int	iBit = 1;
		for (int i = 0; i < NUM_TAPE_MEASURE_TYPE; ++i, iBit <<= 1)
			val |= iBit;
	}

	// 모든 정의된 부위를 계측해서 값을 설정한다.
	int	iBit = 1;
	for (int i = 0; i < NUM_TAPE_MEASURE_TYPE; ++i, iBit <<= 1) {
		if (!(val & iBit))
			continue;

		TAPE_MEASURE_TYPE_T	type = (TAPE_MEASURE_TYPE_T) iBit;

		MEASURE_PTR_HASH::iterator	it;
		it = __measure_ptr_dic.find(type);
		if (it == __measure_ptr_dic.end()) {
			(void) fprintf(stderr, "TAPE_MEASURE: Type = %d: Not implemented yet\n", type);
			continue;
		}

		MEASURE&	measure = *it->second;
		std::vector<MEASURE_ITEM*>&	measure_item_ptr_list = measure.measure_item_ptr_list;
		size_t	nDef = measure_item_ptr_list.size();
		for (int n = 0; n < (int) nDef; ++n) {
			MEASURE_ITEM&	m_item = *measure_item_ptr_list.at(n);
			m_item.allSet = false;		// 처음부터 다시...
			m_item.__ptr->SetDirty();	// 길이도 다시 계산하도록...
		}
	}
}
void	kMesh::SetAllMeasure(const int* valPtr)
{
	// 1. 먼저 설정한다; 아래의 CTRL + '2'
	int	val = 0;
	if (valPtr)
		val = *valPtr;
	else {
		// 모든 부위를 표시한다.
		int	iBit = 1;
		for (int i = 0; i < NUM_TAPE_MEASURE_TYPE; ++i, iBit <<= 1)
			val |= iBit;
	}
{
	// 모든 정의된 부위를 계측해서 값을 설정한다.
	int	iBit = 1;
	for (int i = 0; i < NUM_TAPE_MEASURE_TYPE; ++i, iBit <<= 1) {
		if (!(val & iBit))
			continue;

		TAPE_MEASURE_TYPE_VALUE	typeValue;
		TAPE_MEASURE_TYPE_T&	type = typeValue.type;
		float&	length = typeValue.value;

		// 해당 부위의 길이를 얻는다.
		type = (TAPE_MEASURE_TYPE_T) iBit;
		getValue(TYPE__TAPE_MEASURE, TAPE_MEASURE__VALUE, &typeValue);
		(void) fprintf(stdout, "TAPE_MEASURE_TYPE_T = %d: length = %f\n", type, length);
	}
}
	// 2. 모든 부위를 표시한다. (toggle)
{
	int	iBit = 1;
	for (int i = 0; i < NUM_TAPE_MEASURE_TYPE; ++i, iBit <<= 1) {
		if (!(val & iBit))
			continue;

		TAPE_MEASURE_TYPE_VISIBLE	typeVisible;
		TAPE_MEASURE_TYPE_T&	type = typeVisible.type;
		bool&	visible = typeVisible.visible;

		type = (TAPE_MEASURE_TYPE_T) iBit;
#if 1
		visible = true;		// set to True
		setValue(TYPE__TAPE_MEASURE, TAPE_MEASURE__TYPE, &typeVisible);
#else
		getValue(TYPE__TAPE_MEASURE, TAPE_MEASURE__TYPE, &typeVisible);
//		(void) fprintf(stdout, "TAPE_MEASURE_TYPE_T = %d: Visible: %s\n", type, visible ? "True" : "False");
		visible = !visible;	// toggle
		setValue(TYPE__TAPE_MEASURE, TAPE_MEASURE__TYPE, &typeVisible);
#endif
	}
}
}

void	kMesh::DisplayAllMeasure(CDXUTTextHelper& txtHelper) const
{
	MEASURE_PTR_HASH::const_iterator	it;
	int	valType = __tapeMeasure.getValRef();
	const MEASURE_PTR_HASH&	measure_ptr_dic = __measure_ptr_dic;
//	D3DXMATRIX	__worldMatrix = *g_Camera.GetWorldMatrix();
//	D3DXMATRIX	__viewMatrix = *g_Camera.GetViewMatrix();
//	D3DXMATRIX	__projectionMatrix = *g_Camera.GetProjMatrix();
	for (MEASURE_PTR_HASH::const_iterator it = measure_ptr_dic.begin(); it != measure_ptr_dic.end(); ++it) {
		if (!(valType & it->first))
			continue;

		const MEASURE&	measure = *it->second;
		const std::vector<MEASURE_ITEM*>&	measure_item_ptr_list = measure.measure_item_ptr_list;
//		size_t	nDef = measure_item_ptr_list.size();
//		(void) fprintf(stdout, "# of def. = %d\n", nDef);
{
	D3DXVECTOR2	p[2];
	p[0] = measure.tag.arrow_start_point;
	const MEASURE_ITEM&	m_item = *measure_item_ptr_list.at(0);
	D3DVIEWPORT9	viewport;
	D3DXVECTOR3	P1;
	D3DXVECTOR3	P0 = m_item.__ptr->arrow_end_point;
	DXUTGetD3D9Device()->GetViewport(&viewport);
	D3DXVec3Project(&P1, &P0, &viewport, &__projectionMatrix, &__viewMatrix, &__worldMatrix);
	p[1].x = P1.x; p[1].y = P1.y;	// overwrite p[1]
	LPD3DXLINE	linePtr = 0;
	D3DXCreateLine(DXUTGetD3D9Device(), &linePtr);
	linePtr->Begin();
	linePtr->Draw(p, 2, measure.tag.color);
	linePtr->End();
	SAFE_RELEASE(linePtr);

	// 위치
	txtHelper.SetInsertionPos((int) (.5f * (p[0].x + p[1].x)), (int) (.5f * (p[0].y + p[1].y)));
//	txtHelper.SetInsertionPos((int) (p[1].x), (int) (p[1].y));

	// 문자열
	WCHAR		strTag[MAX_PATH];
	TAPE_MEASURE_TYPE_VALUE	typeValue;
	TAPE_MEASURE_TYPE_T&	type = typeValue.type;
	float&	length = typeValue.value;
	type = it->first;
	((kMesh*) this)->getValue(TYPE__TAPE_MEASURE, TAPE_MEASURE__VALUE, &typeValue);
	swprintf_s(strTag, MAX_PATH, L"%s (%.1f)", measure.tag.name, length / 10.f);	// cm
	txtHelper.DrawTextLine(strTag);
}
	}
}
#endif	// 모든 계측부위 표시

/*
	DWORD	LThigh[1591] = {	// varName:= |LThigh| (1591)
		63612,63613,63614,63615,63616,
		63617,63618,63619,63620,63621,
		63622,63623,63624,63625,63626,
		63627,63628,63629,63630,63631,
		63632,63633,63634,63635,63636,
		63637,63638,63639,63640,63641,
		63642,63643,63644,63645,63646,
		63647,63648,63649,63650,63651,
		63652,63653,63654,63655,63656,
		63657,63658,63659,63660,63661,
		63662,63663,63664,63665,63666,
		63667,63668,63669,63670,63671,
		63672,63673,63674,63675,63676,
		63677,63678,63679,63680,63681,
		63682,63683,63738,63739,63740,
		63741,63742,63743,63744,63745,
		63746,63747,63748,63749,63750,
		63751,63752,63753,63754,63755,
		63756,63757,63758,63759,63760,
		63761,63762,63763,63764,63765,
		63766,63767,63768,63769,63770,
		63771,63772,63773,63774,63775,
		63776,63777,63778,63779,63780,
		63781,63782,63783,63784,63785,
		63786,63787,63788,63789,63790,
		63791,63792,63793,63794,63795,
		63796,63797,63798,63799,63800,
		63801,63802,63803,63804,63805,
		63806,63807,63808,63809,63810,
		63811,63812,63813,63814,63815,
		63816,63817,63818,63819,63820,
		63821,63822,63823,63824,63825,
		63826,63827,63828,63829,63830,
		63831,63832,63833,63834,63835,
		63836,63837,63838,63839,63840,
		63841,63842,63843,63844,63845,
		63846,63847,63848,63849,63850,
		63851,63852,63853,63854,63855,
		63856,63857,63858,63859,63860,
		63861,63862,63863,63864,63865,
		63866,63867,63868,63869,63870,
		63871,63872,63873,63874,63875,
		63876,63877,63878,63879,63880,
		63881,63882,63883,63884,63885,
		63886,63887,63888,63889,63890,
		63891,63892,63893,63894,63895,
		63896,63897,63898,63899,63900,
		63901,63902,63903,63904,63905,
		64094,64095,64096,64097,64098,
		64099,64100,64101,64102,64103,
		64104,64105,64106,64107,64116,
		64117,64118,64119,64120,64121,
		64122,64123,64130,64131,64132,
		64133,64134,64135,64136,64137,
		64138,64140,64141,64142,64143,
		64144,64145,64150,64152,64153,
		64156,64158,64159,64160,64161,
		64162,64165,64166,64167,64168,
		64169,64170,64171,64172,64173,
		64174,64175,64176,64177,64178,
		64179,64180,64181,64182,64183,
		64184,64185,64186,64193,64194,
		64195,64196,64201,64202,64203,
		64204,64207,64208,64209,64210,
		64211,64212,64213,64214,64215,
		64216,64217,64218,64219,64220,
		64221,64222,64223,64224,64227,
		64228,64246,64247,64252,64253,
		64254,64255,64256,64257,64258,
		64259,64260,64261,64268,64269,
		64270,64271,64309,64310,64327,
		64328,64329,64330,64333,64334,
		64338,64339,64340,64341,64342,
		64345,64347,64348,64349,64350,
		64351,64352,64354,64355,64356,
		64357,64358,64359,64364,64369,
		64370,64379,64380,64381,64382,
		64383,64384,64385,64386,64387,
		64388,64393,64394,64395,64396,
		64408,64410,64411,64460,64463,
		64464,64465,64466,64469,64470,
		64471,64472,64473,64474,64494,
		64495,64496,64497,64498,64499,
		64501,64502,64503,64504,64505,
		64506,64508,64509,64510,64511,
		64512,64513,64514,64515,64516,
		64517,64518,64519,64520,64521,
		64522,64523,64524,64525,64526,
		64527,64529,64530,64531,64548,
		64549,64550,64552,64553,64554,
		64555,64556,64557,64558,64559,
		64560,64561,64572,64573,64574,
		64575,64576,64577,64578,64579,
		64580,64581,64582,64583,64584,
		64585,64586,64587,64588,64589,
		64590,64591,64592,64593,64594,
		64595,64596,64597,64598,64599,
		64600,64601,64602,64603,64604,
		64605,64606,64607,64608,64609,
		64610,64611,64612,64613,64614,
		64615,64616,64617,64618,64619,
		64620,64621,64622,64623,64624,
		64625,64626,64627,64628,64629,
		64630,64631,64660,64661,64662,
		64663,64664,64665,64666,64667,
		64668,64669,64670,64671,64672,
		64673,64674,64675,64676,64677,
		64678,64679,64680,64681,64682,
		64683,64684,64685,64686,64687,
		64688,64689,64690,64699,64700,
		64701,64702,64703,64704,64705,
		64706,64707,64708,64709,64710,
		64711,64712,64713,64714,64715,
		64716,64717,64718,64719,64720,
		64721,64722,64723,64724,64725,
		64726,64727,64728,64729,64730,
		64731,64732,64733,64734,64735,
		64736,64737,64738,64739,64740,
		64741,64742,64743,64744,64745,
		64746,64747,64748,64749,64752,
		64753,64754,64755,64756,64757,
		64758,64759,64760,64761,64762,
		64763,64764,64765,64766,64767,
		64768,64769,64770,64771,64772,
		64773,64774,64775,64776,64777,
		64778,64779,64780,64781,64782,
		64783,64784,64785,64786,64787,
		64788,64789,64790,64791,64792,
		64793,64794,64807,64808,64809,
		64810,64811,64812,64813,64814,
		64815,64816,64817,64818,64819,
		64820,64821,64822,64823,64824,
		64825,64832,64833,64834,64835,
		64836,64837,64838,64839,64840,
		64841,64842,64843,64850,64851,
		64852,64853,64854,64855,64856,
		64857,64858,64859,64860,64861,
		64862,64863,64864,64865,64866,
		64867,64868,64869,64870,64871,
		64872,64873,64874,64875,64876,
		64877,64878,64879,64880,64881,
		64882,64883,64884,64886,64894,
		64895,64896,64897,64898,64899,
		64900,64901,64902,64903,64904,
		64905,64906,64907,64909,64910,
		64911,64912,64913,64914,64915,
		64927,64928,64929,64930,64931,
		64932,64933,64934,64935,64936,
		64937,64938,64939,64940,64941,
		64942,64943,64944,64948,64949,
		64950,64951,64952,64955,64956,
		64963,64964,64965,64966,64967,
		64968,64969,64970,64971,64972,
		64973,64974,64975,64976,64977,
		64978,64979,64982,64983,64984,
		64985,64988,64989,64991,64992,
		64999,65000,65001,65002,65003,
		65004,65005,65006,65007,65008,
		65009,65010,65011,65012,65013,
		65014,65015,65016,65030,65031,
		65032,65045,65046,65047,65048,
		65053,65054,65094,65099,65100,
		65101,65102,65103,65104,65105,
		65134,65135,65136,65137,65138,
		65139,65140,65141,65142,65143,
		65144,65145,65146,65147,65148,
		65149,65150,65151,65152,65153,
		65154,65155,65156,65157,65162,
		65163,65166,65167,65170,65171,
		65172,65173,65174,65175,65176,
		65177,65178,65179,65180,65181,
		65182,65183,65184,65185,65186,
		65187,65188,65189,65190,65191,
		65192,65193,65194,65195,65196,
		65197,65198,65199,65200,65201,
		65202,65203,65204,65205,65206,
		65207,65208,65209,65210,65211,
		65212,65213,65214,65215,65216,
		65217,65218,65219,65220,65221,
		65222,65223,65224,65225,65226,
		65227,65228,65229,65230,65231,
		65232,65233,65234,65235,65236,
		65237,65238,65239,65240,65241,
		65242,65243,65244,65245,65246,
		65247,65248,65249,65250,65251,
		65252,65253,65254,65255,65256,
		65257,65258,65264,65265,65266,
		65267,65268,65269,65270,65271,
		65272,65273,65274,65275,65276,
		65277,65278,65279,65280,65283,
		65284,65285,65286,65287,65288,
		65289,65290,65291,65292,65299,
		65300,65301,65302,65303,65304,
		65305,65306,65307,65308,65309,
		65310,65311,65312,65313,65315,
		65316,65324,65327,65330,65331,
		69064,69065,69066,69067,69068,
		69069,69070,69071,69072,69073,
		69074,69075,69076,69077,69078,
		69079,69080,69081,69082,69083,
		69084,69085,69086,69087,69088,
		69089,69090,69091,69092,69093,
		69094,69095,69096,69097,69098,
		69099,69100,69101,69102,69103,
		69104,69105,69106,69107,69108,
		69109,69110,69111,69112,69113,
		69114,69115,69116,69117,69118,
		69119,69120,69121,69122,69123,
		69124,69125,69126,69127,69128,
		69129,69130,69131,69132,69133,
		69134,69135,69136,69137,69138,
		69139,69140,69141,69142,69143,
		69144,69145,69146,69147,69148,
		69149,69150,69151,69152,69153,
		69154,69155,69156,69157,69158,
		69159,69160,69161,69162,69163,
		69164,69165,69166,69417,69418,
		69424,69425,69426,69427,69428,
		69429,69430,69431,69432,69433,
		69434,69435,69436,69437,69438,
		69439,69440,69441,69442,69443,
		69444,69445,69446,69447,69448,
		69449,69450,69451,69466,69467,
		69468,69469,69470,69471,69472,
		69473,69474,69475,69476,69493,
		69495,69496,69497,69498,69499,
		69500,69501,69502,69515,69516,
		69526,69527,69528,69529,69589,
		69590,69613,69614,69615,69616,
		69618,69653,69654,69655,69656,
		69657,69658,69659,69660,69661,
		69662,69663,69664,69665,69666,
		69667,69668,69669,69670,69673,
		69674,69675,69676,69677,69678,
		69679,69680,69681,69682,69683,
		69684,69685,69686,69687,69688,
		69689,69690,69691,69696,69697,
		69698,69699,69700,69702,69711,
		69712,69713,69714,69774,69775,
		69778,69779,69784,69785,69786,
		69787,69788,69789,69790,69791,
		69792,69793,69794,69795,69796,
		69797,69798,69799,69800,69801,
		69802,69803,69804,69805,69806,
		69807,69808,69809,69810,69811,
		69812,69813,69814,69815,69816,
		69817,69818,69819,69820,69821,
		69822,69823,69824,69825,69826,
		69827,69828,69829,69830,69831,
		69832,69833,69834,69835,69836,
		69837,69838,69839,69848,69849,
		69859,69860,69873,69874,69875,
		69876,69877,69878,69879,69880,
		69881,69882,69883,69886,69887,
		69888,69889,69890,69891,69893,
		69894,69897,69898,69899,69904,
		69916,69917,69918,69919,69920,
		69921,69922,69923,69924,69925,
		69926,69927,69928,69929,69930,
		69931,69932,69933,69934,69935,
		69936,69937,69938,69939,69940,
		69941,69942,69943,69944,69945,
		69946,69947,69948,69949,69950,
		69951,69952,69953,69954,69955,
		69956,69957,69958,69959,69960,
		69961,69962,69963,69964,69965,
		69966,69967,69968,69969,69970,
		69971,69972,69973,69974,69975,
		69976,69977,69978,69979,69980,
		69981,69982,69983,69984,69985,
		69986,70044,70045,70054,70055,
		70056,70057,70058,70059,70060,
		70061,70062,70063,70064,70065,
		70066,70067,70068,70069,70070,
		70071,70072,70073,70074,70075,
		70076,70077,70078,70079,70080,
		70081,70082,70083,70084,70085,
		70086,70087,70088,70089,70090,
		70091,70092,70093,70094,70095,
		70096,70097,70098,70099,70100,
		70101,70102,70103,70104,70105,
		70106,70107,70108,70109,70110,
		70111,70112,70113,70114,70115,
		70116,70117,70118,70119,70120,
		70121,70122,70123,70124,70125,
		70126,70127,70128,70129,70130,
		70131,70132,70133,70134,70135,
		70136,70137,70138,70139,70140,
		70141,70142,70143,70144,70145,
		70146,70147,70148,70149,70150,
		70151,70152,70153,70154,70155,
		70156,70157,70158,70159,70160,
		70161,70162,70163,70164,70165,
		70166,70167,70168,70169,70170,
		70171,70172,70173,70174,70175,
		70176,70177,70178,70179,70180,
		70181,70182,70183,70184,70185,
		70186,70187,70188,70189,70190,
		70191,70192,70193,70194,70197,
		70198,70199,70200,70201,70202,
		70203,70204,70205,70206,70207,
		70208,70209,70210,70211,70212,
		70213,70214,70215,70216,70217,
		70218,70233,70234,70235,70236,
		70237,70238,70297,70298,70299,
		70300,70301,70302,70303,70304,
		70305,70306,70307,70308,70309,
		70310,70311,70312,70313,70314,
		70315,70316,70317,70318,70319,
		70320,70321,70322,70323,70324,
		70325,70326,70327,70328,70329,
		70330,70331,70332,70333,70334,
		70335,70336,70337,70338,70339,
		70340,70341,70342,70343,70344,
		70345,70346,70347,70348,70349,
		70350,70351,70352,70353,70354,
		70355,70356,70357,70358,70359,
		70360,70361,70362,70363,70364,
		70365,
	};	// varName: |LThigh| (1591)

	DWORD	LCalf[1109] = {	// varName:= |LCalf| (1109)
		68821,68822,68823,68832,68833,
		68834,68835,68836,68837,68838,
		68839,68840,68841,68842,68843,
		68844,68845,68846,68847,68848,
		68861,68862,68863,68864,68865,
		68874,68875,68881,68882,68883,
		68901,68902,68905,68906,68920,
		68921,68925,68944,68945,68946,
		68947,68948,68949,68950,68951,
		68952,68953,68954,68955,68956,
		68957,68958,68959,68960,68961,
		68962,68963,68964,68965,68966,
		68967,68971,68972,68973,68974,
		68975,68976,68977,68978,68979,
		68980,68981,68982,68990,68991,
		68992,68993,68994,68995,68996,
		68997,68998,68999,69000,69001,
		69002,69003,69004,69005,69006,
		69007,69008,69009,69010,69011,
		69012,69013,69014,69015,69018,
		69019,69024,69025,69026,69027,
		69028,69029,69030,69031,69032,
		69033,69034,69035,69036,69037,
		69038,69039,69040,69041,69042,
		69043,69044,69045,69046,69047,
		69048,69049,69050,69051,69052,
		69053,69054,69055,69056,69057,
		69058,69059,69060,69061,69062,
		69063,70006,70007,70008,70009,
		70223,70224,70225,70226,70227,
		70228,70229,70230,70231,70232,
		70239,70295,70296,70367,70368,
		70369,70370,70371,70372,70373,
		70374,70375,70376,70382,70383,
		70384,70385,70504,70505,70511,
		70512,70513,70514,70515,70516,
		70517,70518,70521,70522,70523,
		70524,70525,70526,70553,70555,
		70556,70557,70558,70559,70560,
		70561,70562,70563,70564,70565,
		70566,70567,70568,70657,70658,
		70661,70662,70663,70664,70665,
		70666,70667,70668,70669,70670,
		70671,70672,70673,70674,70675,
		70676,70677,70678,70679,70680,
		70681,70682,70683,70684,70685,
		70686,70687,70688,70689,70690,
		70691,70692,70693,70694,70695,
		70696,70697,70698,70699,70700,
		70701,70702,70703,70704,70705,
		70706,70707,70708,70709,70710,
		70711,70712,70713,70714,70715,
		70716,70717,70718,70719,70741,
		70742,70743,70744,70745,70746,
		70747,70748,70749,70750,70751,
		70752,70753,70754,70755,70756,
		70757,70758,70759,70760,70761,
		70762,70763,70764,70765,70766,
		70767,70768,70769,70770,70771,
		70772,70773,70774,70775,70776,
		70777,70778,70780,70793,70803,
		70804,70805,70806,70807,70808,
		70809,70810,70811,70812,70813,
		70814,70815,70816,70817,70818,
		70819,70820,70821,70822,70823,
		70824,70825,70826,70827,70828,
		70829,70830,70831,70832,70833,
		70834,70835,70836,70837,70838,
		70839,70840,70841,70842,70843,
		70844,70845,70846,70847,70848,
		70849,70850,70851,70852,70853,
		70854,70855,70856,70857,70858,
		70859,70860,70861,70862,70863,
		70864,70865,70866,70867,70868,
		70869,70870,70871,70872,70873,
		70874,70875,70876,70877,70878,
		70879,70880,70881,70882,70883,
		70884,70885,70886,70887,70888,
		70889,70890,70891,70892,70893,
		70894,70895,70896,70897,70898,
		70899,70900,70901,70902,70903,
		70904,70905,70906,70907,70926,
		70927,70928,70929,70936,70937,
		70938,70939,70940,70941,70942,
		70948,70949,70950,70951,70957,
		70958,70965,70966,70967,70968,
		70969,70970,70971,70972,70973,
		70974,70975,70976,70977,70978,
		70979,70980,70981,70982,70983,
		70984,70985,70986,70987,70988,
		70989,70990,70991,70992,70993,
		70994,70995,70996,70997,70998,
		70999,71000,71001,71002,71003,
		71004,71005,71006,71007,71008,
		71009,71010,71011,71012,71013,
		71014,71015,71016,71017,71018,
		71019,71020,71021,71022,71023,
		71024,71025,71026,71027,71028,
		71029,71030,71031,71032,71033,
		71034,71035,71036,71037,71038,
		71039,71040,71041,71042,71043,
		71044,71045,71046,71047,71048,
		71049,71050,71051,71052,71053,
		71054,71055,71056,71057,71058,
		71059,71060,71061,71062,71063,
		71064,71065,71066,71067,71068,
		71069,71070,71071,71072,71073,
		71074,71075,71076,71077,71078,
		71079,71080,71081,71082,71083,
		71084,71085,71086,71087,71088,
		71089,71090,71091,71092,71093,
		71094,71095,71096,71097,71098,
		71099,71100,71101,71102,71103,
		71104,71105,71106,71107,71108,
		71109,71110,71111,71112,71113,
		71114,71115,71116,71117,71118,
		71119,71120,71121,71122,71123,
		71124,71125,71126,71127,71128,
		71129,71130,71131,71132,71133,
		71134,71135,71136,71137,71138,
		71139,71140,71141,71142,71143,
		71144,71145,71146,71147,71148,
		71149,71150,71151,71152,71153,
		71154,71155,71156,71157,71158,
		71159,71160,71161,71162,71163,
		71164,71165,71166,71167,71168,
		71169,71170,71171,71172,71173,
		71174,71175,71176,71177,71178,
		71179,71180,71181,71182,71183,
		71184,71185,71186,71187,71188,
		71189,71190,71191,71192,71193,
		71194,71195,71196,71197,71198,
		71199,71200,71201,71202,71203,
		71204,71205,71206,71207,71208,
		71209,71210,71211,71212,71213,
		71214,71215,71216,71217,71218,
		71219,71220,71221,71222,71223,
		71224,71225,71226,71227,71228,
		71229,71230,71231,71232,71233,
		71234,71235,71236,71237,71238,
		71239,71240,71241,71242,71243,
		71244,71245,71246,71247,71248,
		71355,71356,71357,71360,71361,
		71383,71423,71424,71425,71426,
		71427,71428,71429,71430,71431,
		71432,71433,71434,71435,71436,
		71437,71438,71439,71440,71441,
		71442,71443,71444,71445,71446,
		71447,71448,71449,71450,71451,
		71452,71453,71454,71455,71456,
		71457,71458,71459,71460,71461,
		71462,71463,71464,71465,71466,
		71467,71468,71469,71470,71471,
		71472,71473,71474,71475,71476,
		71477,71478,71479,71480,71481,
		71482,71483,71484,71485,71486,
		71487,71488,71489,71490,71491,
		71492,71493,71494,71495,71496,
		71497,71498,71499,71500,71501,
		71502,71503,71504,71505,71506,
		71507,71508,71509,71510,71511,
		71512,71513,71514,71515,71516,
		71517,71518,71519,71520,71521,
		71522,71523,71524,71525,71526,
		71527,71528,71529,71530,71531,
		71532,71533,71534,71535,71536,
		71537,71538,71539,71540,71541,
		71542,71543,71544,71545,71546,
		71547,71548,71549,71550,71551,
		71552,71553,71554,71555,71556,
		71557,71558,71559,71560,71561,
		71562,71563,71564,71565,71566,
		71567,71568,71569,71570,71571,
		71572,71573,71574,71575,71576,
		71577,71578,71579,71580,71581,
		71582,71583,71584,71585,71586,
		71587,71588,71589,71590,71591,
		71592,71593,71594,71595,71596,
		71597,71598,71599,71600,71601,
		71602,71603,71604,71605,71606,
		71607,71608,71609,71610,71611,
		71612,71613,71614,71615,71616,
		71617,71618,71619,71620,71621,
		71622,71623,71624,71625,71626,
		71627,71628,71629,71630,71631,
		71632,71633,71634,71635,71636,
		71637,71638,71639,71640,71641,
		71642,71643,71644,71645,71646,
		71647,71648,71649,71650,71651,
		71652,71653,71654,71655,71656,
		71657,71658,71659,71660,71661,
		71662,71663,71664,71665,71666,
		71667,71668,71669,71670,71671,
		71672,71673,71674,71675,71676,
		71677,71678,71679,71680,71681,
		71682,71685,71686,71687,71688,
		71689,71690,71691,71692,71693,
		71694,71695,71696,71697,71698,
		71699,71700,71701,71702,71703,
		71704,71705,71706,71707,71708,
		71709,71710,71711,71712,71713,
		71714,71715,71716,71717,71718,
		71719,71720,71721,71722,71723,
		71724,71725,71726,71727,71728,
		71729,71730,71731,71732,71733,
		71734,71735,71736,71737,71738,
		71739,71740,71741,71742,71743,
		71744,71745,71746,71747,71748,
		71749,71750,71751,71752,71753,
		71754,71755,71756,71757,71758,
		71759,71768,71769,71770,71771,
		71772,71773,71774,71775,71776,
		71777,71778,71779,71780,71781,
		71782,71783,71784,71785,71804,
		71805,71811,71812,71813,71814,
		71815,71816,71817,71818,71819,
		71822,71823,71824,71825,71826,
		71827,71828,71829,71832,71833,
		71834,71835,71836,71837,71838,
		71839,71840,71841,71842,71843,
		71844,71845,71846,71847,71848,
		71849,71850,71851,71852,
	};	// varName: |LCalf| (1109)
 */
void	kMesh::elongate()
{
	float	fScale = 2.f;
#if 1	// kDisplacementMap
	cout << "**************** kDisplacementMap ****************" << endl;
	cout << "**************************************************" << endl;
//	const WCHAR	bmpFile[] = L"D:/home/icentrik/GDI+/gdiplus-1.0.1/debug/victoria-body.bmp";
	const WCHAR	bmpFile[] = L"D:/home/icentrik/My Documents/Visual Studio Projects/Pick/Media/Scanner/여자모델_Vic090609/tex/victoria-depth-left-thigh.bmp";
	kDisplacementMap	dismap(bmpFile);
do {
	if (dismap.GetLastStatus() != Gdiplus::Ok) {
		cerr << "kDisplacementMap::kDisplacementMap(|" << bmpFile << "|): Failure: " << dismap.GetLastStatus() << endl;
		break;
	}
#if 0	// GDI+ 테스트
	int	width = dismap.Width();
	int	height = dismap.Height();
	cout << "H(" << height << ") x W(" << width << ")" << endl;
	Gdiplus::Color	pixelColor;
//	bitmap->GetPixel(21, 38, &pixelColor);
//	bitmap->GetPixel(43, 79, &pixelColor);
//	bitmap->GetPixel(97, 176, &pixelColor);
	dismap.GetPixel(45, 176, &pixelColor);
	if (dismap.GetLastStatus() != Gdiplus::Ok) {
		cerr << "Bitmap::GetPixel(" << 45 << ", " << 176 << "): Failure: " << dismap.GetLastStatus() << endl;
		break;
	}
	(void) fprintf(stdout,
		"0x%08x: R: 0x%02x, G: 0x%02x, B: 0x%02x, A: 0x%02x\n"
		,
		pixelColor.GetValue(),
		pixelColor.GetRed(), pixelColor.GetGreen(), pixelColor.GetBlue(), pixelColor.GetAlpha()
	);
	float	u = .2f, v = .2f;
	float	val;
	Gdiplus::Color	color_at_floored_uv;
	Gdiplus::Status	status = dismap.GetPixelAtUV(u, v, val, kDisplacementMap::CHANNEL_RED, &color_at_floored_uv);
	if (status != Gdiplus::Ok) {
		cerr << "kDisplacementMap::GetPixel(): Failure" << endl;
	}
	(void) fprintf(stdout, "At (u = %f, v = %f): val = %f\n", u, v, val);
	(void) fprintf(stdout,
		"0x%08x: R: 0x%02x, G: 0x%02x, B: 0x%02x, A: 0x%02x\n"
		,
		pixelColor.GetValue(),
		pixelColor.GetRed(), pixelColor.GetGreen(), pixelColor.GetBlue(), pixelColor.GetAlpha()
	);
#endif	// 테스트
} while (0);
	cout << "***********************************************" << endl;
#endif	// kDisplacementMap
#ifdef STANDARD_MODEL
#if 1	// 뼈의 길이 변경
/*
POINT_DEF: (63763, 0.19955, 0.350599)
nDir: [0, 1, 0]

POINT_DEF: (69918, 0.652526, 0.254092)
nDir: [0, 1, 0]
 */
	PLANE_DEF	planeDef0 = {	// 허벅지 위쪽
		{63763, 0.19955f, 0.350599f},
		D3DXVECTOR3(0, 1, 0)
	};
	PLANE_DEF	planeDef1 = {	// 허벅지 아래쪽
		{69918, 0.652526f, 0.254092f},
		D3DXVECTOR3(0, 1, 0)
	};
#if 1	// kBone
#if 0	// 다리를 길어 보이게 하기
	PLANE_DEF	legPlaneDef = {
		{64169, 0.824042f, 0.0886678f},
		D3DXVECTOR3(0, -1, 0)
	};
	kBone	longLeg;
	longLeg.set(*this, legPlaneDef, NULL, 1.2f, false, NULL);
	longLeg.apply(*this);

	PLANE_DEF	armPlaneDef = {
		{56702, 0.0836451f, 0.461388f},
		D3DXVECTOR3(0.999626f, -0.0234326f, 0.0140953f)
	};
	kBone	longArm;
	longArm.set(*this, armPlaneDef, NULL, 1.2f, false, NULL);
	longArm.apply(*this);
#else	// 다리를 길어 보이게 하기/테스트
#if 1	// 실제
#if 0
	PLANE_DEF	planeDef2 = {	// 정강이 위쪽
#if 0
		{71579, 0.63355f, 0.352928f},
		D3DXVECTOR3(-0.801211f, 0.577214f, 0.157755f)

#else
		{70939, 0.159472f, 0.829287f},
//		{69918, 0.652526f, 0.254092f},
		D3DXVECTOR3(0, 1, 0)
#endif
	};
#endif
	kBone::VERTEX_HASH::const_iterator	cit_hash;
	kBone	boneBetween;
	D3DXVECTOR3	P0(0, 0, 0);
/*
A: [75.9515, 984.014, -63.8398]		// 왼쪽 바깥쪽
B: [75.9515, 639.343, -63.8398]		// 안쪽 중간지점
C: [75.9515, 294.673, -63.8398]		// 오른쪽 바깥쪽
 */
	P0 = D3DXVECTOR3(75.9515f, 984.014f, -63.8398f);
//	P0 = D3DXVECTOR3(75.9515f, 639.343f, -63.8398f);
//	P0 = D3DXVECTOR3(75.9515f, 294.673f, -63.8398f);
	boneBetween.set(*this, planeDef0, &planeDef1.pointDef, fScale, false, &P0);
	const kBone::VERTEX_HASH&	insideBetween = boneBetween.inside();
	const kBone::VERTEX_HASH&	outsideBetween = boneBetween.outside();
	cout << "# of ..." << insideBetween.size() << endl;
	cout << "# of ..." << outsideBetween.size() << endl;
	__statePtr->lower_FaceID_array = boneBetween.insideFaceIDArray();
	__statePtr->lower_FaceID_array = boneBetween.outsideFaceIDArray();
	std::vector<D3DXVECTOR3>	v;	// principal component analysis; pricipal axis (뼈 방향)
	for (cit_hash = insideBetween.begin(); cit_hash != insideBetween.end(); ++cit_hash) {
		__statePtr->debugging_prReprID_array.push_back((*cit_hash).first);
		v.push_back((*cit_hash).second);
	}
#if 1	// PCA
	D3DXVECTOR3&	centroid = boneBetween.centroid();
	D3DXVECTOR3&	nDir = boneBetween.nDir();
	kLinearLeastSquare::linear_least_square_line_3(v, centroid, nDir);
{
	__statePtr->points.RemoveAll();
	__statePtr->points.Add(centroid);
	D3DXVECTOR3	P = centroid + 100 * nDir;	// 100?
	__statePtr->points.Add(P);
}
#endif	// PCA
#if 0	// 뼈의 횡방향으로...
{
	P0 = centroid;
	planeDef0.normal = nDir;
	D3DXVECTOR3	P1;
	getPoint(planeDef0.pointDef, P1);
	D3DXVECTOR3	P0;	// P0:= centroid 에서 planeDef 에 내린 수선의 발
	P0 = centroid - D3DXVec3Dot(&D3DXVECTOR3(centroid - P1), &nDir) * nDir;
	boneBetween.set2(*this, planeDef0, &planeDef1.pointDef, fScale, true, &P0, &dismap);
	const kBone::VERTEX_HASH&	insideBetween = boneBetween.inside();
	const kBone::VERTEX_HASH&	outsideBetween = boneBetween.outside();
	cout << "# of ..." << insideBetween.size() << endl;
	cout << "# of ..." << outsideBetween.size() << endl;
	__statePtr->lower_FaceID_array = boneBetween.insideFaceIDArray();
	__statePtr->lower_FaceID_array = boneBetween.outsideFaceIDArray();
	for (cit_hash = insideBetween.begin(); cit_hash != insideBetween.end(); ++cit_hash) {
		__statePtr->debugging_prReprID_array.push_back((*cit_hash).first);
	}
	__statePtr->debugging_prReprID_array.resize(0);
}
	boneBetween.apply(*this);
#else	// 노말방향으로...
#if 1
#if 0	// move to kMesh::simulateWholeBody
	bool	use_p_False = false;	// V(p)
	CalculateTotalVolume(use_p_False);
	float	vol = m__fTotalVolume0;
	boneBetween.apply_all(*this, __statePtr->scale_alpha);
	CalculateTotalVolume(use_p_False);
	float	dV = m__fTotalVolume0 - vol;
	cout << "dV = " << dV / 1000000.f << " ml" << endl;	// (1mm)^3 = 10^-3 cm^3 = 10^-3 mL = 10-6 L
	SetAllMeasureDirty();
	__statePtr->debugging_prReprID_array.resize(0);
#endif
#else
{
	planeDef0.normal = nDir;
	D3DXVECTOR3	P1;
	getPoint(planeDef0.pointDef, P1);
	D3DXVECTOR3	P0;	// P0:= centroid 에서 planeDef 에 내린 수선의 발
	P0 = centroid - D3DXVec3Dot(&D3DXVECTOR3(centroid - P1), &nDir) * nDir;
	nDir = -nDir;	// TO DO LIST
#if 1	// 뼈의 길이 설정; TO DO LIST
	D3DXVECTOR3	P2;
	getPoint(planeDef1.pointDef, P2);
	D3DXVECTOR3	ell_INSIDE = D3DXVec3Dot(&D3DXVECTOR3(P2 - P0), &nDir) * nDir;
	boneBetween.length() = D3DXVec3Length(&ell_INSIDE) * fScale;
#endif	// 뼈의 길이
	boneBetween.apply2(*this, P0, fScale);
	__statePtr->debugging_prReprID_array.resize(0);
}
#endif
#endif
#if 0
	D3DXVECTOR3	P0_star = boneBetween.P0_star();

	kBone	boneLower;
	PLANE_DEF	lowerPlaneDef;
	lowerPlaneDef.pointDef = planeDef2.pointDef;
	lowerPlaneDef.normal = -planeDef2.normal;
	boneLower.set(*this, lowerPlaneDef, NULL, 2.f, false, &P0_star);
	const kBone::VERTEX_HASH&	insideLower = boneLower.inside();
	const kBone::VERTEX_HASH&	outsideLower = boneLower.outside();
	cout << "# of ..." << insideLower.size() << endl;
	cout << "# of ..." << outsideLower.size() << endl;
	__statePtr->lower_FaceID_array = boneLower.insideFaceIDArray();
	for (int n = 0; n < (int) boneBetween.insideFaceIDArray().size(); ++n)
		__statePtr->lower_FaceID_array.push_back(boneBetween.insideFaceIDArray().at(n));
//	__statePtr->lower_FaceID_array = boneLower.outsideFaceIDArray();
	std::set<DWORD>	excludeSet;
	for (cit_hash = insideLower.begin(); cit_hash != insideLower.end(); ++cit_hash) {
		DWORD	prID = (*cit_hash).first;
		kBone::VERTEX_HASH::const_iterator	it = insideBetween.find(prID);
		if (it != insideBetween.end())
			excludeSet.insert(prID);
	}
	cout << "# of DWORD in excludeSet = " << excludeSet.size() << endl;
	std::set<DWORD>::const_iterator	cit_set;
	for (cit_set = excludeSet.begin(); cit_set != excludeSet.end(); ++cit_set)
		cout << *cit_set << endl;
	boneLower.apply(*this, &excludeSet);
#endif
#else	// 실제/예제
	kBone::VERTEX_HASH::const_iterator	cit_hash;
	kBone	boneBetween;
	boneBetween.set(*this, planeDef0, &planeDef1.pointDef, 2.f, true);
	const kBone::VERTEX_HASH&	insideBetween = boneBetween.inside();
	const kBone::VERTEX_HASH&	outsideBetween = boneBetween.outside();
	cout << "# of ..." << insideBetween.size() << endl;
	cout << "# of ..." << outsideBetween.size() << endl;
#if 1
	__statePtr->lower_FaceID_array = boneBetween.insideFaceIDArray();
	__statePtr->lower_FaceID_array = boneBetween.outsideFaceIDArray();
#else
	DWORD*	lowerFaceIDArray = 0;
	lowerFaceIDArray = new DWORD[__numFaces];
	FillMemory(lowerFaceIDArray, __numFaces * sizeof(DWORD), 0);
	for (cit_hash = insideBetween.begin(); cit_hash != insideBetween.end(); ++cit_hash) {
		DWORD	prID = (*cit_hash).first;
#ifdef __USE_A_OFFSET__
		for (int k = pointer_to__a_vFaceIDLocalIndex_a_offset[prID]; k < pointer_to__a_vFaceIDLocalIndex_a_offset[prID + 1]; ++k) {
			const FACE_ID_LOCAL_INDEX&	faceIDLocalIndex = pointer_to__a_vFaceIDLocalIndex[k];
#else
		for (size_t k = 0; k < __pointReprPtr[prID].vFaceIdLocalIndex.size(); ++k) {
			const FACE_ID_LOCAL_INDEX&	faceIDLocalIndex = __pointReprPtr[prID].vFaceIdLocalIndex.at(k);
#endif
			DWORD	faceID = faceIDLocalIndex.ID;
			if (lowerFaceIDArray[faceID]) {	// 이미 등록되어있다.
				continue;
			}
			bool	allInside = true;
			const CFace&	face = __facePtr[faceID];
			for (int i = 0; i < 3; ++i) {
				kBone::VERTEX_HASH::const_iterator	it = insideBetween.find(face.prID[i]);
				if (it == insideBetween.end()) {
					allInside = false;
					break;
				}
			}
			if (allInside)
				lowerFaceIDArray[faceID] = 1;
#ifdef __USE_A_OFFSET__
		}
#else
		}
#endif
//		__statePtr->debugging_prReprID_array.push_back((*cit_hash).first);
	}
	for (int n = 0; n < (int) __numFaces; ++n) {
		if (lowerFaceIDArray[n])
			__statePtr->lower_FaceID_array.push_back(n);
	}
	if (lowerFaceIDArray) {
		delete[] lowerFaceIDArray;
		lowerFaceIDArray = 0;
	}
#endif
	for (cit_hash = insideBetween.begin(); cit_hash != insideBetween.end(); ++cit_hash)
		__statePtr->debugging_prReprID_array.push_back((*cit_hash).first);
	boneBetween.apply(*this);

	kBone	boneLower;
	PLANE_DEF	lowerPlaneDef;
	lowerPlaneDef.pointDef = planeDef1.pointDef;
	lowerPlaneDef.normal = -planeDef1.normal;
	boneLower.set(*this, lowerPlaneDef, NULL, 2.f, true);
	const kBone::VERTEX_HASH&	insideLower = boneLower.inside();
	const kBone::VERTEX_HASH&	outsideLower = boneLower.outside();
	cout << "# of ..." << insideLower.size() << endl;
	cout << "# of ..." << outsideLower.size() << endl;
	__statePtr->lower_FaceID_array = boneLower.insideFaceIDArray();
//	__statePtr->lower_FaceID_array = boneLower.outsideFaceIDArray();
	std::set<DWORD>	excludeSet;
	for (cit_hash = insideLower.begin(); cit_hash != insideLower.end(); ++cit_hash) {
		DWORD	prID = (*cit_hash).first;
		kBone::VERTEX_HASH::const_iterator	it = insideBetween.find(prID);
		if (it != insideBetween.end())
			excludeSet.insert(prID);
	}
	cout << "# of DWORD in excludeSet = " << excludeSet.size() << endl;
	std::set<DWORD>::const_iterator	cit_set;
	for (cit_set = excludeSet.begin(); cit_set != excludeSet.end(); ++cit_set)
		cout << *cit_set << endl;
	boneLower.apply(*this, &excludeSet);
#endif	// 예제
#endif	// 테스트
#else	// kBone/!kBone
	std::vector<DWORD>	betweenFaceIDArray;	// 허벅지 위쪽과 아래쪽 사이에 있는 면들의 목록
#if 1	// PLANE_DEF
	kMesh::CollectBetweenFaces(planeDef0, planeDef1, betweenFaceIDArray, this);
#else	// PLANE_DEF/POINT_DEF
	POINT_DEF	pointDef = planeDef1.pointDef;
	kMesh::CollectBetweenFaces(planeDef0, pointDef, betweenFaceIDArray, this);
#endif	// POINT_DEF

	std::vector<DWORD>	lowerFaceIDArray;	// 허벅지 아래에 있는 면들의 목록;
							// 허벅지 아래쪽을 정의하는 PLANE_DEF 의 normal 이
							// 허벅지 위쪽을 향해야 한다.
	kMesh::CollectLowerFaces(NULL, planeDef1, lowerFaceIDArray, this);

	__statePtr->lower_FaceID_array = lowerFaceIDArray;

	DWORD*	betweenPointReprIDArray = 0;
	DWORD*	lowerPointReprIDArray = 0;
	do {
		betweenPointReprIDArray = new DWORD[__numPointReprs];
		FillMemory(betweenPointReprIDArray, __numPointReprs * sizeof(DWORD), 0);
		for (int n = 0; n < (int) betweenFaceIDArray.size(); ++n) {
			const CFace&	face = __facePtr[betweenFaceIDArray.at(n)];
			for (int k = 0; k < 3; ++k)
				betweenPointReprIDArray[face.prID[k]] = 1;
		}
		lowerPointReprIDArray = new DWORD[__numPointReprs];
		FillMemory(lowerPointReprIDArray, __numPointReprs * sizeof(DWORD), 0);
		for (int n = 0; n < (int) lowerFaceIDArray.size(); ++n) {
			const CFace&	face = __facePtr[lowerFaceIDArray.at(n)];
			for (int k = 0; k < 3; ++k)
				lowerPointReprIDArray[face.prID[k]] = 1;
		}
#if 0
		__statePtr->debugging_prReprID_array.resize(0);
		for (int i = 0; i < (int) __numPointReprs; ++i) {
			if (betweenPointReprIDArray[i])	// lowerPointReprIDArray[i]
				__statePtr->debugging_prReprID_array.push_back(i);
		}
#endif

#if 0	// 뼈 길이 변화
		float	f = 2.f;
{
		CPointRepr::VERTEX*	vertexArray;
		__meshLoader.meshSubsetPtr()->LockVertexBuffer(0, (void**) &vertexArray);
		D3DXVECTOR3	P0;
		getPoint(planeDef0.pointDef, P0);
		D3DXVECTOR3	P1;
#if 1	// PLANE_DEF
		getPoint(planeDef1.pointDef, P1);
#else	// PLANE_DEF/POINT_DEF
		getPoint(pointDef, P1);
#endif	// POINT_DEF
		D3DXVECTOR3	P0P1 = P1 - P0;
		D3DXVECTOR3	nDir = D3DXVec3Dot(&P0P1, &planeDef0.normal) ? planeDef0.normal : -planeDef0.normal;
#if 0	// between
		D3DXVECTOR3	ell_between = D3DXVec3Dot(&P0P1, &nDir) * nDir;
#endif	// between
		for (int n = 0; n < (int) __numPointReprs; ++n) {
#if 0	// between
			if (betweenPointReprIDArray[n]) {
				CPointRepr&	pointRepr = __pointReprPtr[n];
				CPointRepr::VERTEX&	v = pointRepr.v;
				D3DXVECTOR3	ell = v.position - P0;
				D3DXVECTOR3	ell_longitudinal = D3DXVec3Dot(&ell, &nDir) * nDir;
				D3DXVECTOR3	ell_transversal = ell - ell_longitudinal;
				v.position = P0 + (f * ell_longitudinal + ell_transversal);
#ifdef __USE_A_OFFSET__
				for (int k = pointer_to__a_vVertexID_a_offset[n]; k < pointer_to__a_vVertexID_a_offset[n + 1]; ++k)
					vertexArray[pointer_to__a_vVertexID[k]].position = v.position;
#else
				const std::set<DWORD>&	vVertexID = pointRepr.vVertexID;
				for (std::set<DWORD>::const_iterator it = vVertexID.begin(); it != vVertexID.end(); ++it)
					vertexArray[*it].position = v.position;
#endif
			}
			if (lowerPointReprIDArray[n]) {
				CPointRepr&	pointRepr = __pointReprPtr[n];
				CPointRepr::VERTEX&	v = pointRepr.v;
				D3DXVECTOR3	ell = v.position - P0;
				D3DXVECTOR3	ell_longitudinal = D3DXVec3Dot(&ell, &nDir) * nDir;
				D3DXVECTOR3	ell_transversal = ell - ell_longitudinal;
				// ell_longitudinal 은 betweenFaceIDArray 에 있는 부분과
				// lowerFaceIDArray 에 있는 부분으로 나눈다.
				D3DXVECTOR3	ell_lower = ell_longitudinal - ell_between;
				v.position = P0 + (f * ell_between + ell_lower + ell_transversal);
#ifdef __USE_A_OFFSET__
				for (int k = pointer_to__a_vVertexID_a_offset[n]; k < pointer_to__a_vVertexID_a_offset[n + 1]; ++k)
					vertexArray[pointer_to__a_vVertexID[k]].position = v.position;
#else
				const std::set<DWORD>&	vVertexID = pointRepr.vVertexID;
				for (std::set<DWORD>::const_iterator it = vVertexID.begin(); it != vVertexID.end(); ++it)
					vertexArray[*it].position = v.position;
#endif
			}
#else	// between/lower
			if (lowerPointReprIDArray[n]) {
				CPointRepr&	pointRepr = __pointReprPtr[n];
				CPointRepr::VERTEX&	v = pointRepr.v;
				D3DXVECTOR3	ell = v.position - P1;
				D3DXVECTOR3	ell_longitudinal = D3DXVec3Dot(&ell, &nDir) * nDir;
				D3DXVECTOR3	ell_transversal = ell - ell_longitudinal;
				v.position = P1 + (f * ell_longitudinal + ell_transversal);
#ifdef __USE_A_OFFSET__
				for (int k = pointer_to__a_vVertexID_a_offset[n]; k < pointer_to__a_vVertexID_a_offset[n + 1]; ++k)
					vertexArray[pointer_to__a_vVertexID[k]].position = v.position;
#else
				for (std::set<DWORD>::const_iterator it = vVertexID.begin(); it != vVertexID.end(); ++it)
					vertexArray[*it].position = v.position;
#endif
			}
#endif	// lower
		}
		__meshLoader.meshSubsetPtr()->UnlockVertexBuffer();
}
#endif	// 뼈 길이 변화
	} while (0);
	SAFE_DELETE_ARRAY(betweenPointReprIDArray);
	SAFE_DELETE_ARRAY(lowerPointReprIDArray);
#endif	// !kBone
#endif	// 뼈의 길이 변경
#endif	// STANDARD_MODEL
}

/*
 * blender-2.49b/source/blender/src/editmesh_mods.c
 */
#if 0
void vertexsmooth(void)
{
	EditMesh *em = G.editMesh;
	EditVert *eve, *eve_mir = NULL;
	EditEdge *eed;
	float *adror, *adr, fac;
	float fvec[3];
	int teller=0;
	ModifierData *md= G.obedit->modifiers.first;

	if(G.obedit==0) return;

	/* count */
	eve= em->verts.first;
	while(eve) {
		if(eve->f & SELECT) teller++;
		eve= eve->next;
	}
	if(teller==0) return;
	
	adr=adror= (float *)MEM_callocN(3*sizeof(float *)*teller, "vertsmooth");
	eve= em->verts.first;
	while(eve) {
		if(eve->f & SELECT) {
			eve->tmp.p = (void*)adr;
			eve->f1= 0;
			eve->f2= 0;
			adr+= 3;
		}
		eve= eve->next;
	}

	/* if there is a mirror modifier with clipping, flag the verts that
	 * are within tolerance of the plane(s) of reflection 
	 */
	for (; md; md=md->next) {
		if (md->type==eModifierType_Mirror) {
			MirrorModifierData *mmd = (MirrorModifierData*) md;	
		
			if(mmd->flag & MOD_MIR_CLIPPING) {
				for (eve= em->verts.first; eve; eve= eve->next) {
					if(eve->f & SELECT) {

						switch(mmd->axis){
							case 0:
								if (fabs(eve->co[0]) < mmd->tolerance)
									eve->f2 |= 1;
								break;
							case 1:
								if (fabs(eve->co[1]) < mmd->tolerance)
									eve->f2 |= 2;
								break;
							case 2:
								if (fabs(eve->co[2]) < mmd->tolerance)
									eve->f2 |= 4;
								break;
						}
					}
				}
			}
		}
	}
	
	eed= em->edges.first;
/*
 * 각 Edge 를 구성하는 두 Vertex 의 중점이 각 Vertex 의 새로운 위치에 영향을 미친다;
 * Vertex 의 새로운 위치들의 centroid 와 원래 위치의 중점의 실제로 새로운 위치가 된다.
 */
	while(eed) {
		if( (eed->v1->f & SELECT) || (eed->v2->f & SELECT) ) {
			fvec[0]= (eed->v1->co[0]+eed->v2->co[0])/2.0;
			fvec[1]= (eed->v1->co[1]+eed->v2->co[1])/2.0;
			fvec[2]= (eed->v1->co[2]+eed->v2->co[2])/2.0;
			
			if((eed->v1->f & SELECT) && eed->v1->f1<255) {
				eed->v1->f1++;
				VecAddf(eed->v1->tmp.p, eed->v1->tmp.p, fvec);
			}
			if((eed->v2->f & SELECT) && eed->v2->f1<255) {
				eed->v2->f1++;
				VecAddf(eed->v2->tmp.p, eed->v2->tmp.p, fvec);
			}
		}
		eed= eed->next;
	}

	eve= em->verts.first;
	while(eve) {
		if(eve->f & SELECT) {
			if(eve->f1) {
				
				if (G.scene->toolsettings->editbutflag & B_MESH_X_MIRROR) {
					eve_mir= editmesh_get_x_mirror_vert(G.obedit, eve->co);
				}
				
				adr = eve->tmp.p;
				fac= 0.5/(float)eve->f1;
				
				eve->co[0]= 0.5*eve->co[0]+fac*adr[0];
				eve->co[1]= 0.5*eve->co[1]+fac*adr[1];
				eve->co[2]= 0.5*eve->co[2]+fac*adr[2];
				
				
				/* clip if needed by mirror modifier */
				if (eve->f2) {
					if (eve->f2 & 1) {
						eve->co[0]= 0.0f;
					}
					if (eve->f2 & 2) {
						eve->co[1]= 0.0f;
					}
					if (eve->f2 & 4) {
						eve->co[2]= 0.0f;
					}
				}
				
				if (eve_mir) {
					eve_mir->co[0]=-eve->co[0];
					eve_mir->co[1]= eve->co[1];
					eve_mir->co[2]= eve->co[2];
				}
				
			}
			eve->tmp.p= NULL;
		}
		eve= eve->next;
	}
	MEM_freeN(adror);

	recalc_editnormals();

	allqueue(REDRAWVIEW3D, 0);
	DAG_object_flush_update(G.scene, G.obedit, OB_RECALC_DATA);

#ifdef WITH_VERSE
	if(G.editMesh->vnode)
		sync_all_verseverts_with_editverts(G.editMesh->vnode);
#endif
	BIF_undo_push("Vertex Smooth");
}
#endif
void	kMesh::MakeVertexSmooth(UINT niter)
{
	HRESULT	hResult;

//	LPDIRECT3DVERTEXBUFFER9	pVB_n = __showNormal.GetVertexBufferPointer();
//	kShowNormal::Vertex*	pV_n;
//	pVB_n->Lock(0, 0, (void**) &pV_n, 0);

	kMesh::D3DXVERTEX*	pV;
	__vertexBufferPtr->Lock(0, 0, (void**) &pV, 0);

//	FLOAT*	pVB_ = NULL;
//	hResult = __meshPtr->LockVertexBuffer(0, (void**) &pVB_);
//	assert(D3D_OK == hResult);

#if 1	// main
#if 1	// Vertex 위치 변경
for (UINT iter = 0; iter < niter; ++iter) {
	for (int prID = 0; prID < (int) __numPointReprs; ++prID) {
		if (__pointReprPtr[prID].flag.fixed)
			continue;
		D3DXVERTEX&	v = pV[prID];
		v.pTmp = v.p;
	}
	for (int prID = 0; prID < (int) __numPointReprs; ++prID) {
		if (__pointReprPtr[prID].flag.fixed)
			continue;
		D3DXVERTEX&	v = pV[prID];
/*
 * 각 Edge 를 구성하는 두 Vertex 의 중점이 각 Vertex 의 새로운 위치에 영향을 미친다;
 * Vertex 의 새로운 위치들의 centroid 와 원래 위치의 중점의 실제로 새로운 위치가 된다.
 */
		// 주어진 Vertex 를 한 끝으로 하는 모든 Edge 에서 Edge 의 중점을 구한다.
		// 이 점들의 centroid 와 원래 점의 중점이 새로운 위치가 된다.
		D3DXVECTOR3	centroid(0, 0, 0);
		unsigned	numPoints = 0;
#ifdef __USE_A_OFFSET__
		numPoints = pointer_to__a_vEdgeID_a_offset[prID + 1] - pointer_to__a_vEdgeID_a_offset[prID];
		for (int k = pointer_to__a_vEdgeID_a_offset[prID]; k < pointer_to__a_vEdgeID_a_offset[prID + 1]; ++k) {
			const EDGE	e = pointer_to__a_vEdgeID[k];
			const CEdge&	edge = __edgePtr[e.id];
			centroid += .5f * (pV[edge.prStartID].p + pV[edge.prEndID].p);
		}
#else
		const CPointRepr&	pointRepr = __pointReprPtr[prID];
		numPoints = pointRepr.vEdgeID.size();
		for (size_t i = 0; i < pointRepr.vEdgeID.size(); ++i) {
			EDGE	e = pointRepr.vEdgeID.at(i);
			const CEdge&	edge = __edgePtr[e.id];
			centroid += .5f * (pV[edge.prStartID].p + pV[edge.prEndID].p);
		}
#endif
//		cout << "numPoints = " << numPoints << endl;
		centroid /= (float) numPoints;
		v.pTmp = .5f * (v.p + centroid);
	}
	for (int prID = 0; prID < (int) __numPointReprs; ++prID) {
		if (__pointReprPtr[prID].flag.fixed)
			continue;
		D3DXVERTEX&	v = pV[prID];
		v.p = v.pTmp;

#if 0	// 아래에서 한꺼번에...
		D3DXVECTOR3&	p = v.p;
		pV_n[2 * prID + 0].Position = p;
		pV_n[2 * prID + 1].Position = p;
		pVB_[8 * prID + 0] = p.x;
		pVB_[8 * prID + 1] = p.y;
		pVB_[8 * prID + 2] = p.z;
		__pointReprPtr[prID].v.position = p;
#endif	// 아래에서 한꺼번에...
	}
}
#endif	// Vertex 위치 변경
#endif	// main

#if 1	// __vertexBufferPtr 의 p 으로 해당하는 모든 값들 업데이트
	LPDIRECT3DVERTEXBUFFER9	pVB_n = __showNormal.GetVertexBufferPointer();
	kShowNormal::Vertex*	pV_n;
	pVB_n->Lock(0, 0, (void**) &pV_n, 0);

//	kMesh::D3DXVERTEX*	pV;
//	__vertexBufferPtr->Lock(0, 0, (void**) &pV, 0);

	FLOAT*	pVB_ = NULL;
	hResult = __meshPtr->LockVertexBuffer(0, (void**) &pVB_);
	assert(D3D_OK == hResult);

	// Texture 를 표시하는 메쉬는 변경하지 않는다!
	for (int prID = 0; prID < (int) __numPointReprs; ++prID) {
		if (__pointReprPtr[prID].flag.fixed)
			continue;
		D3DXVERTEX&	v = pV[prID];

		D3DXVECTOR3&	p = v.p;
		pV_n[2 * prID + 0].Position = p;
		pV_n[2 * prID + 1].Position = p;
		pVB_[8 * prID + 0] = p.x;
		pVB_[8 * prID + 1] = p.y;
		pVB_[8 * prID + 2] = p.z;
		__pointReprPtr[prID].v.position = p;
	}
#endif	// __vertexBufferPtr 의 p 으로 해당하는 모든 값들 업데이트

#if 1	// Vertex 의 새로운 노말 결정
	// 각 Vertex 의 노말을 다시 계산;
	// Vertex 를 공유하는 Face 들의 노말의 평균이 해당 Vertex 의 노말
	// 1. 각 Face 의 노말을 저장할 임시 공간
	D3DXVECTOR3*	faceNormalArray = new D3DXVECTOR3[__numFaces];
	for (int n = 0; n < (int) __numFaces; ++n) {	// n:= face ID
		const CFace&	face = __facePtr[n];
		D3DXVECTOR3&	p0 = pV[face.prID0].p;
		D3DXVECTOR3&	p1 = pV[face.prID1].p;
		D3DXVECTOR3&	p2 = pV[face.prID2].p;
		D3DXVECTOR3	v01 = p1 - p0;
//		D3DXVec3Normalize(&v01, &v01);
		D3DXVECTOR3	v02 = p2 - p0;
//		D3DXVec3Normalize(&v02, &v02);
		D3DXVECTOR3&	normal = faceNormalArray[n];
		D3DXVec3Cross(&normal, &v01, &v02);
		D3DXVec3Normalize(&normal, &normal);
	}
	// 2. 각 Vertex 의 노말을 공유하는 Face 들의 노말로부터 계산
	for (int prID = 0; prID < (int) __numPointReprs; ++prID) {	// n:= point repr. ID
		D3DXVECTOR3&	normal = pV[prID].n;
		unsigned	numFaces = 0;
#ifdef __USE_A_OFFSET__
		numFaces = pointer_to__a_vFaceIDLocalIndex_a_offset[prID + 1] - pointer_to__a_vFaceIDLocalIndex_a_offset[prID];
#else
		numFaces = __pointReprPtr[prID].vFaceIdLocalIndex.size();
#endif
#ifdef __USE_A_OFFSET__
		for (int k = pointer_to__a_vFaceIDLocalIndex_a_offset[prID]; k < pointer_to__a_vFaceIDLocalIndex_a_offset[prID + 1]; ++k) {
			const FACE_ID_LOCAL_INDEX&	faceIDLocalIndex = pointer_to__a_vFaceIDLocalIndex[k];
#else
		for (size_t k = 0; k < __pointReprPtr[prID].vFaceIdLocalIndex.size(); ++k) {
			const FACE_ID_LOCAL_INDEX&	faceIDLocalIndex = __pointReprPtr[prID].vFaceIdLocalIndex.at(k);
#endif
			normal += faceNormalArray[faceIDLocalIndex.ID];
#ifdef __USE_A_OFFSET__
		}
#else
		}
#endif
		normal /= (float) numFaces;
		D3DXVec3Normalize(&normal, &normal);
		// __meshPtr 에 사용되는 노말도 업데이트
	/*
		pVertices[i].n.x = pVB[8 * i + 3];
		pVertices[i].n.y = pVB[8 * i + 4];
		pVertices[i].n.z = pVB[8 * i + 5];
	*/
		pVB_[8 * prID + 3] = normal.x;
		pVB_[8 * prID + 4] = normal.y;
		pVB_[8 * prID + 5] = normal.z;
	}
	SAFE_DELETE_ARRAY(faceNormalArray);
#endif	// Vertex 의 새로운 노말 결정

	__vertexBufferPtr->Unlock();
	hResult = __meshPtr->UnlockVertexBuffer();
	assert(D3D_OK == hResult);
	pVB_n->Unlock();
}
void	kMesh::PlotPoints(const CGrowableArray<D3DXVECTOR3>& points, float pointSize, const D3DXVECTOR4* pointColor_ptr) const
{
	D3DXVECTOR4	pointColor = pointColor_ptr ? *pointColor_ptr : D3DXVECTOR4(1, 0, 0, 1);
	UINT	numPoints = points.GetSize();
	if (numPoints == 0)
		return;

	HRESULT	hResult;

	LPDIRECT3DVERTEXBUFFER9	pointsVertexBufferPtr = NULL;
	__devicePtr->CreateVertexBuffer(
		numPoints * sizeof(D3DXVECTOR3),
		D3DUSAGE_WRITEONLY,
		0,
		D3DPOOL_MANAGED,
		&pointsVertexBufferPtr,
		NULL
	);
	D3DXVECTOR3*	pointArrayPtr;
	pointsVertexBufferPtr->Lock(0, 0, (void**) &pointArrayPtr, 0);
	CopyMemory(pointArrayPtr, points.GetData(), numPoints * sizeof(D3DXVECTOR3));
	pointsVertexBufferPtr->Unlock();
do {
	__effectPtr->SetInt("g__FillMode", D3DFILL_POINT);
	__effectPtr->SetMatrix("g_mWorldViewProjection", &__worldViewProjectionMatrix);
	__effectPtr->SetMatrix("g_mWorld", &__worldMatrix);
//	FLOAT		pointSize = 5.f;
//	D3DXVECTOR4	pointColor(1, 0, 0, 1);
	__effectPtr->SetFloat("pointSize", pointSize);
	__effectPtr->SetVector("pointColor", &pointColor);
	D3DXHANDLE	hTechnique = __effectPtr->GetTechnique(0);
	assert(hTechnique);
	hResult = __effectPtr->SetTechnique(hTechnique);
	assert(S_OK == hResult);
	UINT	NumberOfPasses;
	hResult = __effectPtr->Begin(&NumberOfPasses, 0);
	assert(D3D_OK == hResult);
	for (UINT iPass = 0; iPass < NumberOfPasses; ++iPass) {
		hResult = __effectPtr->BeginPass(iPass);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetVertexDeclaration(__OnePointDeclarationPtr);
		assert(D3D_OK == hResult);
		hResult = __devicePtr->SetStreamSource(0, pointsVertexBufferPtr, 0, sizeof(D3DXVECTOR3));
		assert(D3D_OK == hResult);
		hResult = __devicePtr->DrawPrimitive(D3DPT_POINTLIST, 0, numPoints);
		assert(D3D_OK == hResult);
		hResult = __effectPtr->EndPass();
		assert(S_OK == hResult);
	}
	hResult = __effectPtr->End();
	assert(S_OK == hResult);
} while (0);
	SAFE_RELEASE(pointsVertexBufferPtr);
}

#if 1	// SHIFT
void	kMesh::SetShift(SHIFT_ALIGN_TYPE align_type)
{
	const D3DSURFACE_DESC*	pBackBufferSurfaceDesc = DXUTGetD3D9BackBufferSurfaceDesc();
	float	H = (float) pBackBufferSurfaceDesc->Height;
	float	W = (float) pBackBufferSurfaceDesc->Width;
	FLOAT	h = __right_upper_far.y - __left_lower_near.y;
	FLOAT	w = __right_upper_far.x - __left_lower_near.x;
	switch (align_type) {
	case SHIFT_ALIGN_LEFT:
		__shift = 0.f;
		break;
	case SHIFT_ALIGN_CENTER:
		__shift = .5f * (W - (H / h) * w);
		break;
	case SHIFT_ALIGN_RIGHT:
		__shift = W - (H / h) * w;
		break;
	default:
		cerr << "kMesh::SetShift(): Unknown shift align type: " << align_type << endl;
		break;
	}
}
void	kMesh::SetShift(float shift_amount)
{
	// 물체의 중심 수직선이 항상 보이는 화면의 중앙 수직선에 오도록 한다.
	const D3DSURFACE_DESC*	pBackBufferSurfaceDesc = DXUTGetD3D9BackBufferSurfaceDesc();
	float	H = (float) pBackBufferSurfaceDesc->Height;
	float	W = (float) pBackBufferSurfaceDesc->Width;
	FLOAT	h = __right_upper_far.y - __left_lower_near.y;
	FLOAT	w = __right_upper_far.x - __left_lower_near.x;
	// clamp to [0, W - 1]
	if (shift_amount < 0.f)
		shift_amount = 0.f;
	if (shift_amount >= W)
		shift_amount = W;
	if ((W - shift_amount) >= (H / h) * w) {	// 남은 폭이 물체를 담을 만큼 충분한가?
		__shift = shift_amount + .5f * ((W - shift_amount) - (H / h) * w);
	} else {
		__shift = shift_amount - ((.5f * (H / h) * w) - .5f * (W - shift_amount));
	}
}
#endif	// SHIFT
#if 1	// Voxel
void	kMesh::RenderVoxel() const
{
	UINT	num_triangles_per_voxel =
		6 /* # of quad. faces per voxel */ *
		2 /* # of triangles per quad. */;
#if 1	// Index Buffer
	UINT	num_vertices_per_voxel = 8;	// # of vertex per voxel
#else	// Index Buffer/!Index Buffer
	UINT	num_vertices_per_voxel = num_triangles_per_voxel * 3 /* # of vertices per triangle */;
#endif	// !Index Buffer
#if 1
	UINT	num_voxels = __meshLoader.GetNumVoxels();
	if (!num_voxels)
		return;
#else
	LPDIRECT3DVERTEXBUFFER9		voxelVertexBufferPtr = 0;
	LPDIRECT3DINDEXBUFFER9		voxelIndexBufferPtr = 0;

#if 0	// kMeshLoader::VOXEL
	struct VOXEL
	{
		float	size;
		float	X, Y, Z;
		UINT	R, G, B;
	};
	VOXEL	voxels[] = {
#if 1
		{2.500000, 423.750000, 883.750000, 113.750000, 150, 104, 101,},
		{2.500000, 436.250000, 891.250000, 146.250000, 131, 83, 78,},

		{2.500000, 41.250000, 1706.250000, -3.750000, 127, 134, 138,},
		{2.500000, 48.750000, 1706.250000, 33.750000, 119, 122, 123,},
#endif
	};
#endif
//	UINT	num_voxels = sizeof(voxels) / sizeof(VOXEL);
	UINT	num_voxels = __meshLoader.GetVoxels().GetSize();
	if (!num_voxels)
		return;
//	cout << "# of voxels = " << num_voxels << endl;
//	cout << "# of voxels0 = " << __meshLoader.GetVoxels().GetSize() << endl;
//	UINT	num_voxels = 1;
	HRESULT	hr = __devicePtr->CreateVertexBuffer(
		num_voxels * num_vertices_per_voxel * sizeof(D3DXVERTEX),
		D3DUSAGE_WRITEONLY,
		0,
//		D3DPOOL_DEFAULT,//D3DPOOL_MANAGED,
		D3DPOOL_MANAGED,
		&voxelVertexBufferPtr,
		NULL
	);
	assert(D3D_OK == hr);
#if 1	// Index Buffer
	__devicePtr->CreateIndexBuffer(
		num_voxels * num_triangles_per_voxel * sizeof(CFace::FaceIndex),
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX32,
		D3DPOOL_MANAGED,
		&voxelIndexBufferPtr,
		NULL
	);
#endif	// Index Buffer

	D3DXVERTEX*	voxelVertexArray;
	voxelVertexBufferPtr->Lock(0, 0, (void**) &voxelVertexArray, 0);
#if 1	// Index Buffer
	CFace::FaceIndex*	pFaceIndices;
	voxelIndexBufferPtr->Lock(0, 0, (void**) &pFaceIndices, 0);
#endif	// Index Buffer
//	int	index = 0;
		//vx 2.500000 423.750000 883.750000 113.750000 150 104 101
for (int index = 0; index < (int) num_voxels; ++index) {
//	const VOXEL&	voxel = voxels[index];
	const kMeshLoader::VOXEL&	voxel = __meshLoader.GetVoxels().GetAt(index);
	const float& voxel_size = voxel.size;
//		float	voxel_size = 2.5f;
/*
 *                  4           5
         	   +-----------+
	          /|          /|
	         / |         / |
	       0+-----------+1 |
	        |  |        |  |
	        |  |7       |  |6
	        |  +--------|--+        y  z
	        | /         | /         | /
	        |/          |/          |/
	       3*-----------+2		*----x

	LHS;
		v0: (X + 0, Y + d, Z + 0)
		v1: (X + d, Y + d, Z + 0)
		v2: (X + d, Y + 0, Z + 0)
		v3: (X + 0, Y + 0, Z + 0)
		v4: (X + 0, Y + d, Z + d)
		v5: (X + d, Y + d, Z + d)
		v6: (X + d, Y + 0, Z + d)
		v7: (X + 0, Y + 0, Z + d)

	front:
	       0+----+1
		|   /|
		|  / |	f: 013
		| /  |	f: 231
		|/   |
	       3+----+2
	back:
	       5+----+4
		|   /|
		|  / |	f: 546
		| /  |	f: 764
		|/   |
	       6+----+7
	right:
	       1+----+5
		|   /|
		|  / |	f: 152
		| /  |	f: 625
		|/   |
	       2+----+6
	left:
	       4+----+0
		|   /|
		|  / |	f: 407
		| /  |	f: 370
		|/   |
	       7+----+3
	top:
	       4+----+5
		|   /|
		|  / |	f: 450
		| /  |	f: 105
		|/   |
	       0+----+1
	bottom:
	       3+----+2
		|   /|
		|  / |	f: 327
		| /  |	f: 672
		|/   |
	       7+----+6
 */
//		UINT	R = 150, G = 104, B = 101;
	const UINT&	R = voxel.R;
	const UINT&	G = voxel.G;
	const UINT&	B = voxel.B;
		D3DXVECTOR3	voxel_color((float) R / 255.f, (float) G / 255.f, (float) B / 255.f);
		for (UINT i = 0; i < num_vertices_per_voxel; ++i) {
#if 1	// Index Buffer
			voxelVertexArray[index * num_vertices_per_voxel + i].c = voxel_color;
#else	// Index Buffer/!Index Buffer
			voxelVertexArray[index * num_triangles_per_voxel + i].c = voxel_color;
#endif	// !Index Buffer
		}
//		float	X = 423.75, Y = 883.75, Z = 113.75;
	const float&	X = voxel.X;
	const float&	Y = voxel.Y;
	const float&	Z = voxel.Z;
	__statePtr->points.RemoveAll();
//		
		float	d = voxel_size;
	// LHS
	D3DXVECTOR3	v0 = D3DXVECTOR3(X + 0, Y + d, Z + 0);
	D3DXVECTOR3	v1 = D3DXVECTOR3(X + d, Y + d, Z + 0);
	D3DXVECTOR3	v2 = D3DXVECTOR3(X + d, Y + 0, Z + 0);
	D3DXVECTOR3	v3 = D3DXVECTOR3(X + 0, Y + 0, Z + 0);
	D3DXVECTOR3	v4 = D3DXVECTOR3(X + 0, Y + d, Z + d);
	D3DXVECTOR3	v5 = D3DXVECTOR3(X + d, Y + d, Z + d);
	D3DXVECTOR3	v6 = D3DXVECTOR3(X + d, Y + 0, Z + d);
	D3DXVECTOR3	v7 = D3DXVECTOR3(X + 0, Y + 0, Z + d);
	__statePtr->points.Add(v0);
	__statePtr->points.Add(v1);
	__statePtr->points.Add(v2);
	__statePtr->points.Add(v3);
	__statePtr->points.Add(v4);
	__statePtr->points.Add(v5);
	__statePtr->points.Add(v6);
	__statePtr->points.Add(v7);
#if 1	// Index Buffer
/*
 * front:
		|  / |	f: 013
		| /  |	f: 231
 * back:
		|  / |	f: 546
		| /  |	f: 764
 * right:
		|  / |	f: 152
		| /  |	f: 625
 * left:
		|  / |	f: 407
		| /  |	f: 370
 * top:
		|  / |	f: 450
		| /  |	f: 105
 * bottom:
		|  / |	f: 327
		| /  |	f: 672
 */
	voxelVertexArray[index * num_vertices_per_voxel + 0].p = v0;
	voxelVertexArray[index * num_vertices_per_voxel + 1].p = v1;
	voxelVertexArray[index * num_vertices_per_voxel + 2].p = v2;
	voxelVertexArray[index * num_vertices_per_voxel + 3].p = v3;
	voxelVertexArray[index * num_vertices_per_voxel + 4].p = v4;
	voxelVertexArray[index * num_vertices_per_voxel + 5].p = v5;
	voxelVertexArray[index * num_vertices_per_voxel + 6].p = v6;
	voxelVertexArray[index * num_vertices_per_voxel + 7].p = v7;
	// 앞면(front; 0[013], 1[231])
	pFaceIndices[index * num_triangles_per_voxel + 0].i0 = index * num_vertices_per_voxel + 0;
	pFaceIndices[index * num_triangles_per_voxel + 0].i1 = index * num_vertices_per_voxel + 1;
	pFaceIndices[index * num_triangles_per_voxel + 0].i2 = index * num_vertices_per_voxel + 3;
	pFaceIndices[index * num_triangles_per_voxel + 1].i0 = index * num_vertices_per_voxel + 2;
	pFaceIndices[index * num_triangles_per_voxel + 1].i1 = index * num_vertices_per_voxel + 3;
	pFaceIndices[index * num_triangles_per_voxel + 1].i2 = index * num_vertices_per_voxel + 1;
	// 뒷면(back; 2[546], 3[764])
	pFaceIndices[index * num_triangles_per_voxel + 2].i0 = index * num_vertices_per_voxel + 5;
	pFaceIndices[index * num_triangles_per_voxel + 2].i1 = index * num_vertices_per_voxel + 4;
	pFaceIndices[index * num_triangles_per_voxel + 2].i2 = index * num_vertices_per_voxel + 6;
	pFaceIndices[index * num_triangles_per_voxel + 3].i0 = index * num_vertices_per_voxel + 7;
	pFaceIndices[index * num_triangles_per_voxel + 3].i1 = index * num_vertices_per_voxel + 6;
	pFaceIndices[index * num_triangles_per_voxel + 3].i2 = index * num_vertices_per_voxel + 4;
	// 우측면(right; 4[152], 5[625])
	pFaceIndices[index * num_triangles_per_voxel + 4].i0 = index * num_vertices_per_voxel + 1;
	pFaceIndices[index * num_triangles_per_voxel + 4].i1 = index * num_vertices_per_voxel + 5;
	pFaceIndices[index * num_triangles_per_voxel + 4].i2 = index * num_vertices_per_voxel + 2;
	pFaceIndices[index * num_triangles_per_voxel + 5].i0 = index * num_vertices_per_voxel + 6;
	pFaceIndices[index * num_triangles_per_voxel + 5].i1 = index * num_vertices_per_voxel + 2;
	pFaceIndices[index * num_triangles_per_voxel + 5].i2 = index * num_vertices_per_voxel + 5;
	// 좌측면(left; 6[407], 7[370])
	pFaceIndices[index * num_triangles_per_voxel + 6].i0 = index * num_vertices_per_voxel + 4;
	pFaceIndices[index * num_triangles_per_voxel + 6].i1 = index * num_vertices_per_voxel + 0;
	pFaceIndices[index * num_triangles_per_voxel + 6].i2 = index * num_vertices_per_voxel + 7;
	pFaceIndices[index * num_triangles_per_voxel + 7].i0 = index * num_vertices_per_voxel + 3;
	pFaceIndices[index * num_triangles_per_voxel + 7].i1 = index * num_vertices_per_voxel + 7;
	pFaceIndices[index * num_triangles_per_voxel + 7].i2 = index * num_vertices_per_voxel + 0;
	// 윗면(top; 8[450], 9[105])
	pFaceIndices[index * num_triangles_per_voxel + 8].i0 = index * num_vertices_per_voxel + 4;
	pFaceIndices[index * num_triangles_per_voxel + 8].i1 = index * num_vertices_per_voxel + 5;
	pFaceIndices[index * num_triangles_per_voxel + 8].i2 = index * num_vertices_per_voxel + 0;
	pFaceIndices[index * num_triangles_per_voxel + 9].i0 = index * num_vertices_per_voxel + 1;
	pFaceIndices[index * num_triangles_per_voxel + 9].i1 = index * num_vertices_per_voxel + 0;
	pFaceIndices[index * num_triangles_per_voxel + 9].i2 = index * num_vertices_per_voxel + 5;
	// 아래면(bottom; 10[327], 11[672])
	pFaceIndices[index * num_triangles_per_voxel + 10].i0 = index * num_vertices_per_voxel + 3;
	pFaceIndices[index * num_triangles_per_voxel + 10].i1 = index * num_vertices_per_voxel + 2;
	pFaceIndices[index * num_triangles_per_voxel + 10].i2 = index * num_vertices_per_voxel + 7;
	pFaceIndices[index * num_triangles_per_voxel + 11].i0 = index * num_vertices_per_voxel + 6;
	pFaceIndices[index * num_triangles_per_voxel + 11].i1 = index * num_vertices_per_voxel + 7;
	pFaceIndices[index * num_triangles_per_voxel + 11].i2 = index * num_vertices_per_voxel + 2;
	for (int k = 0; k < 12; ++k) {
		pFaceIndices[index * num_triangles_per_voxel + k].i0 *= N_ELEMENT;
		pFaceIndices[index * num_triangles_per_voxel + k].i1 *= N_ELEMENT;
		pFaceIndices[index * num_triangles_per_voxel + k].i2 *= N_ELEMENT;
	}
#else	// Index Buffer/!Index Buffer
	// 앞면(front; 0[013], 1[231])
	voxelVertexArray[index * num_triangles_per_voxel + 3 * 0 + 0].p = v0;
	voxelVertexArray[index * num_triangles_per_voxel + 3 * 0 + 1].p = v1;
	voxelVertexArray[index * num_triangles_per_voxel + 3 * 0 + 2].p = v3;
	voxelVertexArray[index * num_triangles_per_voxel + 3 * 1 + 0].p = v2;
	voxelVertexArray[index * num_triangles_per_voxel + 3 * 1 + 1].p = v3;
	voxelVertexArray[index * num_triangles_per_voxel + 3 * 1 + 2].p = v1;
	// 뒷면(back; 2[546], 3[764])
	voxelVertexArray[index * num_triangles_per_voxel + 3 * 2 + 0].p = v5;
	voxelVertexArray[index * num_triangles_per_voxel + 3 * 2 + 1].p = v4;
	voxelVertexArray[index * num_triangles_per_voxel + 3 * 2 + 2].p = v6;
	voxelVertexArray[index * num_triangles_per_voxel + 3 * 3 + 0].p = v7;
	voxelVertexArray[index * num_triangles_per_voxel + 3 * 3 + 1].p = v6;
	voxelVertexArray[index * num_triangles_per_voxel + 3 * 3 + 2].p = v4;
	// 우측면(right; 4[152], 5[625])
	voxelVertexArray[index * num_triangles_per_voxel + 3 * 4 + 0].p = v1;
	voxelVertexArray[index * num_triangles_per_voxel + 3 * 4 + 1].p = v5;
	voxelVertexArray[index * num_triangles_per_voxel + 3 * 4 + 2].p = v2;
	voxelVertexArray[index * num_triangles_per_voxel + 3 * 5 + 0].p = v6;
	voxelVertexArray[index * num_triangles_per_voxel + 3 * 5 + 1].p = v2;
	voxelVertexArray[index * num_triangles_per_voxel + 3 * 5 + 2].p = v5;
	// 좌측면(left; 6[407], 7[370])
	voxelVertexArray[index * num_triangles_per_voxel + 3 * 6 + 0].p = v4;
	voxelVertexArray[index * num_triangles_per_voxel + 3 * 6 + 1].p = v0;
	voxelVertexArray[index * num_triangles_per_voxel + 3 * 6 + 2].p = v7;
	voxelVertexArray[index * num_triangles_per_voxel + 3 * 7 + 0].p = v3;
	voxelVertexArray[index * num_triangles_per_voxel + 3 * 7 + 1].p = v7;
	voxelVertexArray[index * num_triangles_per_voxel + 3 * 7 + 2].p = v0;
	// 윗면(top; 8[450], 9[105])
	voxelVertexArray[index * num_triangles_per_voxel + 3 * 8 + 0].p = v4;
	voxelVertexArray[index * num_triangles_per_voxel + 3 * 8 + 1].p = v5;
	voxelVertexArray[index * num_triangles_per_voxel + 3 * 8 + 2].p = v0;
	voxelVertexArray[index * num_triangles_per_voxel + 3 * 9 + 0].p = v1;
	voxelVertexArray[index * num_triangles_per_voxel + 3 * 9 + 1].p = v0;
	voxelVertexArray[index * num_triangles_per_voxel + 3 * 9 + 2].p = v5;
	// 아래면(bottom; 10[327], 11[672])
	voxelVertexArray[index * num_triangles_per_voxel + 3 * 10 + 0].p = v3;
	voxelVertexArray[index * num_triangles_per_voxel + 3 * 10 + 1].p = v2;
	voxelVertexArray[index * num_triangles_per_voxel + 3 * 10 + 2].p = v7;
	voxelVertexArray[index * num_triangles_per_voxel + 3 * 11 + 0].p = v6;
	voxelVertexArray[index * num_triangles_per_voxel + 3 * 11 + 1].p = v7;
	voxelVertexArray[index * num_triangles_per_voxel + 3 * 11 + 2].p = v2;
#endif
}

#if 1	// Index Buffer
	voxelIndexBufferPtr->Unlock();
#endif	// Index Buffer
	voxelVertexBufferPtr->Unlock();
#endif

#if 1	// 렌더링
{
	// kEnum.h 에 있는 SHADING_TYPE 와
	// mesh.fx 에 있는 g__texture 가 같은 상수를 사용하도록 맞춰야 한다!
	HRESULT	hr;
	SHADING_TYPE	shadingType = __model.getShadingRef();
	// Voxel 렌더링의 경우, SHADING_NONE 이 본래의 색깔을 표시하도록 한다.
	if (shadingType == SHADING_TEXTURE)
		shadingType = SHADING_NONE;
	hr = __effectPtr->SetInt("g__texture", shadingType);
	assert(hr == D3D_OK);
//	__effectPtr->SetInt("g__overlap", __overlap);
	__effectPtr->SetInt("g__FillMode", D3DFILL_SOLID);
//	__effectPtr->SetInt("g__FillMode", D3DFILL_POINT);
//	__effectPtr->SetVector("pointColor", &D3DXVECTOR4(1, 1, 0, 1));
	D3DXMATRIX	identity;
	D3DXMatrixIdentity(&identity);
//	__effectPtr->SetMatrix("g_mWorldViewProjection", &identity);
	__effectPtr->SetMatrix("g_mWorldViewProjection", &__worldViewProjectionMatrix);
	__effectPtr->SetMatrix("g_mWorld", &__worldMatrix);
	D3DXHANDLE	hTechnique = __effectPtr->GetTechnique(0);
	assert(hTechnique);
	hr = __effectPtr->SetTechnique(hTechnique);
	assert(S_OK == hr);
	UINT	NumberOfPasses;
	hr = __effectPtr->Begin(&NumberOfPasses, 0);
	assert(D3D_OK == hr);
	for (UINT iPass = 0; iPass < NumberOfPasses; ++iPass) {
		hr = __effectPtr->BeginPass(iPass);
		assert(D3D_OK == hr);
#if 1	// kMeshLoader
		hr = __devicePtr->SetVertexDeclaration(__meshLoader.voxelVertexDeclarationPtr());
		assert(D3D_OK == hr);
		hr = __devicePtr->SetStreamSource(0, __meshLoader.voxelVertexBufferPtr(), 0, sizeof(kMeshLoader::VOXEL_VERTEX));
		hr = __devicePtr->SetIndices(__meshLoader.voxelIndexBufferPtr());
		assert(D3D_OK == hr);
		hr = __devicePtr->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, num_voxels * num_vertices_per_voxel, 0, num_voxels * num_triangles_per_voxel);
		assert(D3D_OK == hr);
#else	// kMeshLoader/!kMeshLoader
		hr = __devicePtr->SetVertexDeclaration(__vertexDeclarationPtr);
		assert(D3D_OK == hr);
#if 1	// Index Buffer
		// 인덱스 버퍼를 만들 때, sizeof(D3DXVECTOR3) 기준
		hr = __devicePtr->SetStreamSource(0, voxelVertexBufferPtr, 0, sizeof(D3DXVECTOR3));
		hr = __devicePtr->SetIndices(voxelIndexBufferPtr);
		assert(D3D_OK == hr);
		hr = __devicePtr->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, N_ELEMENT * num_voxels * num_vertices_per_voxel, 0, num_voxels * num_triangles_per_voxel);
		assert(D3D_OK == hr);
#else	// Index Buffer/!Index Buffer
		hr = __devicePtr->SetStreamSource(0, voxelVertexBufferPtr, 0, sizeof(D3DXVERTEX));
		assert(D3D_OK == hr);
		hr = __devicePtr->DrawPrimitive(D3DPT_TRIANGLELIST, 0, num_voxels * num_triangles_per_voxel);
		assert(D3D_OK == hr);
#endif	// !Index Buffer
#endif	// !kMeshLoader
		hr = __effectPtr->EndPass();
		assert(S_OK == hr);
	}
	hr = __effectPtr->End();
	assert(D3D_OK == hr);
}
#endif	// 렌더링

#if 1	// kMeshLoader
#else	// kMeshLoader/!kMeshLoader
	SAFE_RELEASE(voxelIndexBufferPtr);
	SAFE_RELEASE(voxelVertexBufferPtr);
#endif	// !kMeshLoader
}
#endif	// Voxel

#if 1	// 체형변화 시뮬레이션
#if 1
int
kMesh::__whole_body_simulation_initialize(float scale)
{
#if 1	// 표준모델과 몸통서브셋 설정
#ifndef STANDARD_MODEL
	cerr << "kMesh::__whole_body_simulation_initialize: No standard model" << endl;
	return -1;
#endif	// !STANDARD_MODEL
#ifndef __KEKO__
	int	index_to_standard = __index_to_standard;
	if (
		index_to_standard != 1 /* ETRI 초기 표준모델 */ &&
		index_to_standard != 6 /* ETRI 091211 표준모델(남: Michael) */ &&
		index_to_standard != 7 /* ETRI 091211 표준모델(여: Victoria) */
	) {
		cerr << "kMesh::__whole_body_simulation_initialize: index_to_standard = 1, 6, 7 only" << endl;
		return -1;
	}
#endif	// !__KEKO__
	int	subset_index;	// 여기서 사용되는 몸의 subset 의 인덱스;
				// 전체(얼굴, 눈, 눈썹, 몸)이 있을 경우 3(ID = 0, 1),
				// 몸만 있는 경우에는 0 (ID = 3, 4, 5, 6, 7)
#ifdef __KEKO__
	subset_index = 0;
#else	// __KEKO__/!__KEKO__
	subset_index = index_to_standard == 1 /* ETRI 초기 표준모델 */ ? 3 : 0;
#endif	// !__KEKO__
#endif	// 표준모델과 몸통서브셋 설정


	// CPointRepr::Area2Flag 플래그 초기화
	for (DWORD prID = 0; prID < __numPointReprs; ++prID) {
		__pointReprPtr[prID].flag.force = false;
		__pointReprPtr[prID].flag.reset();
	}
	// CFace::Flag.active 플래그 초기화
	for (DWORD faceID = 0; faceID < __numFaces; ++faceID)
		__facePtr[faceID].flag.active = false;
	// CFace::Flag::active 플래그 설정; CPointRepr::Area2Flag::subset 플래그 설정
	DWORD*	subsetArray;
	__meshLoader.meshPtr()->LockAttributeBuffer(0, &subsetArray);
	for (DWORD faceID = 0; faceID < __numFaces; ++faceID) {
		const CFace&	face = __facePtr[faceID];
		if (subsetArray[faceID] != (DWORD) subset_index)	// 0: victoria-face
						// 1: victoria-eyebrow
						// 2: victoria-eye
						// 3: victoria-body
			continue;

		__facePtr[faceID].flag.active = true;
		for (int i = 0; i < 3; ++i)
			__pointReprPtr[face.prID[i]].flag.subset = true;
	}
	__meshLoader.meshPtr()->UnlockAttributeBuffer();

	// dismap 으로부터 값을 얻어 CPointRepr::UVAL::fval 에 설정하고,
	// CPointRepr::Area2Flag::move 플래그를 설정
#ifdef __KEKO__
	const WCHAR	bmpFile[] = L"./Resources/keko-dismap.bmp";
#if 1	// 텍스쳐도 변경
	kMeshLoader::Material&	material = *__meshLoader.GetMaterials().GetAt(subset_index);
	SAFE_RELEASE(material.texturePtr);
	D3DXCreateTextureFromFile(DXUTGetD3D9Device(), bmpFile, &material.texturePtr);
#endif
#else	// __KEKO__/!__KEKO__
	WCHAR		bmpFile[MAX_PATH];
	swprintf_s(bmpFile, MAX_PATH, L"Media/Scanner/에트리표준모델-091211/%s/tex/%s-whole-body-displacement.bmp", index_to_standard == 6 ? L"michael" : L"victoria", index_to_standard == 6 ? L"michael" : L"victoria");
#endif	// !__KEKO__
	kDisplacementMap	dismap(bmpFile);
	if (dismap.GetLastStatus() != Gdiplus::Ok) {
		cerr << "kMesh::__whole_body_simulation_initialize: kDisplacementMap::kDisplacementMap(|" << bmpFile << "|): Failure: " << dismap.GetLastStatus() << endl;
		return -1;
	}
	kMesh::D3DXVERTEX*	pV;	// kMesh::__vertexBufferPtr
	__vertexBufferPtr->Lock(0, 0, (void**) &pV, 0);
	for (DWORD prID = 0; prID < __numPointReprs; ++prID) {
		if (__pointReprPtr[prID].flag.fixed)		// 움직이지 말아야 할 Vertex 라면...
			continue;
		if (__pointReprPtr[prID].flag.subset == false)	// 몸통서브셋이 아니라면...
			continue;

		kMesh::D3DXVERTEX&	v = pV[prID];
		float	factor;	// 주어진 Vertex 에 해당하는, dismap 으로부터 얻은 값
		Gdiplus::Status	status = dismap.GetPixelAtUV(v.tu, v.tv, factor, kDisplacementMap::CHANNEL_RED);
		if (status != Gdiplus::Ok) {
			cerr << "kMesh::__whole_body_simulation_initialize: kDisplacementMap::GetPixel(): Failure" << endl;
			continue;
		} else {
			// clamp to [0, 1]
			factor /= dismap.MaxRed() / (float) 255;
			if (factor < 0.f)
				factor = 0.f;
			if (factor > 1.f)
				factor = 1.f;
		}
		const float	amplitude = factor * scale * __a;	// 이 Vertex 가 normal 방향으로 움직일 거리
		if (fabsf(amplitude) < FLT_EPSILON) {
			continue;
		}
		__pointReprPtr[prID].flag.move = true;	// CPointRepr::Area2Flag::move 플래그 설정
		__pointReprPtr[prID].val.fval = factor;	// CPointRepr::UVAL::fval 설정
	}
	__vertexBufferPtr->Unlock();

	return 0;
}
int
kMesh::__whole_body_simulation_do(float scale)
{
	bool	sub = scale < 0;
#ifndef __KEKO__
#define AREA2_HINT
#endif	// !__KEKO__
#ifdef AREA2_HINT
	typedef kBone::VERTEX_HASH	VERTEX_HASH;
	kState&	state = *__statePtr;
#endif	// AREA2_HINT


	kMesh::D3DXVERTEX*	pV;	// kMesh::__vertexBufferPtr
	__vertexBufferPtr->Lock(0, 0, (void**) &pV, 0);

#ifdef AREA2_HINT
	struct Area2
	{
		float	before;
		float	after;
		bool	increase;	// 면적이 증가했는가?

		Area2() : increase(false) {}
	};
	typedef std::map<DWORD, Area2>	AREA2_HASH;
	AREA2_HASH	area2_dic;
	VERTEX_HASH	vertex_dic;	// __pointReprPtr[prID].move 값이 True 인 Vertex 의 위치를 기억한다.
#endif	// AREA2_HINT
#ifdef AREA2_HINT
if (sub) {
//	cout << "**** before ****" << endl;
	for (DWORD faceID = 0; faceID < __numFaces; ++faceID) {
		if (!__facePtr[faceID].flag.active)
			continue;

		CFace&	face = __facePtr[faceID];
		D3DXVECTOR3&	p0 = pV[face.prID0].p;
		D3DXVECTOR3&	p1 = pV[face.prID1].p;
		D3DXVECTOR3&	p2 = pV[face.prID2].p;
		D3DXVECTOR3	v01 = p1 - p0;
		D3DXVECTOR3	v02 = p2 - p0;
		D3DXVECTOR3	Va;
		D3DXVec3Cross(&Va, &v01, &v02);
//		cout << "Area[" << face.id << "] = " << fabsf(D3DXVec3Length(&Va)) << endl;
		Area2&	area2 = area2_dic[faceID];
		area2.before = fabsf(D3DXVec3Length(&Va));
	}
}
#endif	// AREA2_HINT

	// CPointRepr::Area2Flag::move 가 설정된 Vertex 를 CPointRepr::UVAL::fval 만큼
	// normal 방향으로 움직인다.
	for (DWORD prID = 0; prID < __numPointReprs; ++prID) {
		if (!__pointReprPtr[prID].flag.move)	// 움직이지 않는 Vertex 라면...
			continue;

		kMesh::D3DXVERTEX&	v = pV[prID];
		D3DXVECTOR3&	p = v.p;
#ifdef AREA2_HINT
		if (sub)
			vertex_dic[prID] = p;
#endif	// AREA2_HINT
		p += (__pointReprPtr[prID].val.fval /* factor */ * scale * __a) * v.n;
	}

#ifndef __KEKO__
#if 1	// Vertex 위치 변경; ZBrush Smooth Brush
if (!sub && state.area2_control.Smooth == 2) {
for (int iter = 0; iter < (1 + (int) state.area2_control.SmoothIncLevel) * (int) __statePtr->brush.flag.SmoothLevel; ++iter)
{
	for (DWORD prID = 0; prID < __numPointReprs; ++prID) {
		if (__pointReprPtr[prID].flag.move == false)
			continue;

		kMesh::D3DXVERTEX&	v = pV[prID];
		v.pTmp = v.p;
	}
	for (DWORD prID = 0; prID < __numPointReprs; ++prID) {
		if (__pointReprPtr[prID].flag.move == false)
			continue;

		kMesh::D3DXVERTEX&	v = pV[prID];
/*
 * 각 Edge 를 구성하는 두 Vertex 의 중점이 각 Vertex 의 새로운 위치에 영향을 미친다;
 * Vertex 의 새로운 위치들의 centroid 와 원래 위치의 중점의 실제로 새로운 위치가 된다.
 */
		// 주어진 Vertex 를 한 끝으로 하는 모든 Edge 에서 Edge 의 중점을 구한다.
		// 이 점들의 centroid 와 원래 점의 중점이 새로운 위치가 된다.
		D3DXVECTOR3	centroid(0, 0, 0);
		unsigned	numPoints = 0;
#ifdef __USE_A_OFFSET__
		numPoints = pointer_to__a_vEdgeID_a_offset[prID + 1] - pointer_to__a_vEdgeID_a_offset[prID];
		for (int k = pointer_to__a_vEdgeID_a_offset[prID]; k < pointer_to__a_vEdgeID_a_offset[prID + 1]; ++k) {
			const EDGE	e = pointer_to__a_vEdgeID[k];
			const CEdge&	edge = __edgePtr[e.id];
			centroid += .5f * (pV[edge.prStartID].p + pV[edge.prEndID].p);
		}
#else
		const CPointRepr&	pointRepr = __pointReprPtr[prID];
		numPoints = pointRepr.vEdgeID.size();
		for (size_t i = 0; i < pointRepr.vEdgeID.size(); ++i) {
			EDGE	e = pointRepr.vEdgeID.at(i);
			const CEdge&	edge = __edgePtr[e.id];
			centroid += .5f * (pV[edge.prStartID].p + pV[edge.prEndID].p);
		}
#endif
//		cout << "numPoints = " << numPoints << endl;
		centroid /= (float) numPoints;
		v.pTmp = .5f * (v.p + centroid);
	}
	for (DWORD prID = 0; prID < __numPointReprs; ++prID) {
		if (__pointReprPtr[prID].flag.move == false)
			continue;

		D3DXVECTOR3&	p = pV[prID].p;
		p = pV[prID].pTmp;
	}
}
}
#endif	// Vertex 위치 변경; ZBrush Smooth Brush
#endif	// !__KEKO__

#ifdef AREA2_HINT
if (sub) {
//	cout << "**** after ****" << endl;
	for (DWORD faceID = 0; faceID < __numFaces; ++faceID) {
		if (!__facePtr[faceID].flag.active)
			continue;

		CFace&	face = __facePtr[faceID];
		D3DXVECTOR3&	p0 = pV[face.prID0].p;
		D3DXVECTOR3&	p1 = pV[face.prID1].p;
		D3DXVECTOR3&	p2 = pV[face.prID2].p;
		D3DXVECTOR3	v01 = p1 - p0;
		D3DXVECTOR3	v02 = p2 - p0;
		D3DXVECTOR3	Va;
		D3DXVec3Cross(&Va, &v01, &v02);
//		cout << "Area[" << face.id << "] = " << fabsf(D3DXVec3Length(&Va)) << endl;
		Area2&	area2 = area2_dic[faceID];
		area2.after = fabsf(D3DXVec3Length(&Va));
	}
}
#endif	// AREA2_HINT
#ifdef AREA2_HINT
if (sub) {
#if 0	// debugging...
	state.area2_vertex_array.clear();
	cout << "# of area2_vertex_array = " << state.area2_vertex_array.size() << endl;
	AREA2_HASH::const_iterator	cit_hash;
	for (cit_hash = area2_dic.begin(); cit_hash != area2_dic.end(); ++cit_hash) {
		DWORD	faceID = (*cit_hash).first;
		const Area2&	area2 = (*cit_hash).second;
		cout << "Area2[" << faceID << "]: " << area2.before << " -> " << area2.after << endl;
		if (area2.after > area2.before) {
			cout << "hello, world" << endl;
			CFace&	face = faceArray[faceID];
//			state.debugging_FaceID_array.push_back(faceID);
			// 중복되게 prID 가 들어간다. 하지만 보는 데에는 지장이 없다.
			state.area2_vertex_array.push_back(face.prID[0]);
			state.area2_vertex_array.push_back(face.prID[1]);
			state.area2_vertex_array.push_back(face.prID[2]);
		}
	}
	cout << "# of area2_vertex_array = " << state.area2_vertex_array.size() << endl;
#endif	// debugging...
	AREA2_HASH::iterator	it_hash;
	int	num_inc_faces = 0;
	state.area2_face_array.clear();
	for (it_hash = area2_dic.begin(); it_hash != area2_dic.end(); ++it_hash) {
		DWORD	faceID = (*it_hash).first;
		Area2&	area2 = (*it_hash).second;
		float	delta_area_percent = (area2.after - area2.before) / area2.before * 100.f;
		if (delta_area_percent > (float) state.Area2Threshold) {
			state.area2_face_array.push_back(faceID);
			++num_inc_faces;
			area2.increase = true;
			CFace&	face = __facePtr[faceID];
			for (int i = 0; i < 3; ++i) {
				DWORD	prID = face.prID[i];
				if (!__pointReprPtr[prID].flag.fixed)
					__pointReprPtr[prID].flag.inc = true;
			}
		}
	}
//	cout << "# of face area incresed = " << num_inc_faces << endl;
	for (DWORD prID = 0; prID < __numPointReprs; ++prID) {
		if (__pointReprPtr[prID].flag.inc == false)
			continue;

		// 이 점이 움직이면 이 점을 공유하는 주변의 삼각형 중의 적어도 하나의 면적이 증가한다.
		// 몇 개의 삼각형의 면적이 증가하는가?
		unsigned	numFaces = 0;
		unsigned	numFaces_area_increased = 0;
#ifdef __USE_A_OFFSET__
		numFaces = pointer_to__a_vFaceIDLocalIndex_a_offset[prID + 1] - pointer_to__a_vFaceIDLocalIndex_a_offset[prID];
#else
		numFaces = __pointReprPtr[prID].vFaceIdLocalIndex.size();
#endif
#ifdef __USE_A_OFFSET__
		for (int k = pointer_to__a_vFaceIDLocalIndex_a_offset[prID]; k < pointer_to__a_vFaceIDLocalIndex_a_offset[prID + 1]; ++k) {
			const FACE_ID_LOCAL_INDEX&	faceIDLocalIndex = pointer_to__a_vFaceIDLocalIndex[k];
#else
		for (size_t k = 0; k < __pointReprPtr[prID].vFaceIdLocalIndex.size(); ++k) {
			const FACE_ID_LOCAL_INDEX&	faceIDLocalIndex = __pointReprPtr[prID].vFaceIdLocalIndex.at(k);
#endif
			DWORD	faceID = faceIDLocalIndex.ID;
			AREA2_HASH::const_iterator	cit = area2_dic.find(faceID);
			if (cit == area2_dic.end()) {
				// 다른 subset 에 있는 Face
				continue;
			}
			const Area2&	area2 = (*cit).second;
			if (area2.increase)
				++numFaces_area_increased;
#ifdef __USE_A_OFFSET__
		}
#else
		}
#endif
//		cout << "prID = " << prID << ": " << numFaces_area_increased << "/" << numFaces << endl;
		if (numFaces_area_increased == numFaces)	// TO DO LIST; all or nothing?
			__pointReprPtr[prID].flag.all = true;
	}
#if 0	// debugging...
if (state.area2_control.ShowArea2 == 1) {	// inc
	state.area2_vertex_array.clear();
	for (DWORD prID = 0; prID < (DWORD) mesh.GetNumPointReprs(); ++prID) {
#if 1	// FLAG
		if (__pointReprPtr[prID].flag.inc == false)
			continue;
#else	// FLAG/!FLAG
		if (pointReprIDArray[prID] < 4)
			continue;
#endif	// !FLAG

		state.area2_vertex_array.push_back(prID);
	}
}
if (state.area2_control.ShowArea2 == 2) {	// all
	state.area2_vertex_array.clear();
	for (DWORD prID = 0; prID < (DWORD) mesh.GetNumPointReprs(); ++prID) {
#if 1	// FLAG
		if (__pointReprPtr[prID].flag.all == false)
			continue;
#else	// FLAG/!FLAG
		if (pointReprIDArray[prID] < 4)
			continue;
#endif	// !FLAG

		state.area2_vertex_array.push_back(prID);
	}
}
#endif	// debugging...
}
#endif	// AREA2_HINT

#if 1	// 특정 Vertex (Flag::repos 가 True 인) 의 위치를 재설정
#ifdef AREA2_HINT	// 특정 Vertex (Flag::repos 가 True 인) 의 위치를 재설정
if (
	sub &&
	(state.area2_control.ShowArea2 == 1 || state.area2_control.ShowArea2 == 2) &&
	state.area2_control.Repos
) {
	// __pointReprPtr[prID].repos 값이 True 인 Vertex 들을 원래 위치로
	// repos 를 설정...; TO DO LIST
if (state.area2_control.ShowArea2 == 1) {
	for (DWORD prID = 0; prID < __numPointReprs; ++prID) {
		if (__pointReprPtr[prID].flag.fixed)
			continue;
		__pointReprPtr[prID].flag.repos = __pointReprPtr[prID].flag.inc;
	}
} else if (state.area2_control.ShowArea2 == 2) {
	cerr << "ShowArea2 = 2; not implemented yet" << endl;
}
{
#if 1	// Vertex 위치 변경; 노말방향으로...
#if 1	// LOOPing
	int	num_rest = 0;	// LOOPing 후에도 위치가 아직 결정되지 않은 Vertex 의 갯수
	int	loop_counter = 0;	// 문제가 있을 때 무한루프에 빠지지 않도록...
#define MAX_LOOP	10		// 최대 MAX_LOOP 만큼 반복
do {
	++loop_counter;
//	cout << "LOOPing..." << endl;
	num_rest = 0;	// 초기화
#endif	// LOOPing
	for (DWORD prID = 0; prID < __numPointReprs; ++prID) {
		if (__pointReprPtr[prID].flag.repos == false)
			continue;

		kMesh::D3DXVERTEX&	v = pV[prID];

		D3DXVECTOR3&	p = v.p;
		if (__pointReprPtr[prID].flag.done == true)
			continue;
		// 이 Vertex 를 지금 단계에서 바꿀 수 있는 지 알아본다;
		// 이 Vertex 주변의 Vertex 들 중에서
		//	1) Flag::all 이 False 이거나 즉 Flag::inc 가 False 이거나
		//	Flag::inc 가 True 이더라도 어떤 기준에 의해 Flag::all 이 False 이거나
		//	2) Flag::done 이 True 인 Vertex 가 있는 지 알아본다.
		int	num_hint_vertex = 0;	// 이 Vertex 가 움직일 방향을 결정해줄 주변 Vertex 의 갯수
		D3DXVECTOR3	avg_p(0, 0, 0);	// num_hint_vertex 개의 주변 Vertex 들에 의해 결정되는
						// 이 Vertex 이 "평균" 위치
{
#ifdef __USE_A_OFFSET__
		for (int k = pointer_to__a_vEdgeID_a_offset[prID]; k < pointer_to__a_vEdgeID_a_offset[prID + 1]; ++k) {
			const EDGE	e = pointer_to__a_vEdgeID[k];
			const CEdge&	edge = __edgePtr[e.id];
#else
		const CPointRepr&	pointRepr = __pointReprPtr[prID];
		for (size_t i = 0; i < pointRepr.vEdgeID.size(); ++i) {
			EDGE	e = pointRepr.vEdgeID.at(i);
			const CEdge&	edge = __edgePtr[e.id];
#endif
			DWORD	prID_ref = edge.prStartID == prID ? edge.prEndID : edge.prStartID;
			const CPointRepr::Area2Flag&	flag = __pointReprPtr[prID_ref].flag;
			if (flag.repos && !flag.done)
				continue;
//			cout << "prID_ref = " << prID_ref << endl;
			// 이 Vertex 에서 주어진 Vertex 에 이르는 원래 벡터만큼,
			// 이 Vertex 의 새로운 위치에 더해 주어진 Vertex 의 새로운 위치를 결정한다.
			if (vertex_dic.find(prID_ref) == vertex_dic.end()) {
//				cout << "prID_ref = " << prID_ref << "(" << prID << "): No vertex_dic" << endl;
//				cout << ".fixed? " << (__pointReprPtr[prID_ref].flag.fixed ? "True" : "False") << endl;
				continue;
			}
			avg_p += pV[prID_ref].p + (vertex_dic[prID] - vertex_dic[prID_ref]);
			++num_hint_vertex;
#ifdef __USE_A_OFFSET__
		}
#else
		}
#endif
}
//		cout << "num_hint_vertex = " << num_hint_vertex << endl;
		if (num_hint_vertex == 0) {
			++num_rest;
			continue;
		}
		p = avg_p / (float) num_hint_vertex;
		__pointReprPtr[prID].flag.done = true;
	}
#if 1	// LOOPing
} while (num_rest && loop_counter < MAX_LOOP);
#undef MAX_LOOP
#endif	// LOOPing
#endif	// Vertex 위치 변경

#if 1	// Vertex 위치 변경; ZBrush Smooth Brush
if (state.area2_control.Smooth == 1 || state.area2_control.Smooth == 2) {
	// Smoothing 에 사용될 Vertex 는...
	for (DWORD prID = 0; prID < __numPointReprs; ++prID) {
		if (state.area2_control.Smooth == 1) {
			if (state.area2_control.ShowArea2 == 1)
				__pointReprPtr[prID].flag.smooth = __pointReprPtr[prID].flag.inc;
			else if (state.area2_control.ShowArea2 == 1)
				__pointReprPtr[prID].flag.smooth = __pointReprPtr[prID].flag.all;
		} else if (state.area2_control.Smooth == 2)
			__pointReprPtr[prID].flag.smooth = __pointReprPtr[prID].flag.move;
	}
	if (state.area2_control.Smooth == 1 && state.area2_control.SmoothIncLevel) {
		// smooth 가 설정된 Vertex 에 대해 boundary0 설정
		for (DWORD prID = 0; prID < __numPointReprs; ++prID) {
			CPointRepr::Area2Flag&	flag = __pointReprPtr[prID].flag;
			flag.boundary0 = flag.smooth;
		}
		for (unsigned n = 0; n < state.area2_control.SmoothIncLevel; ++n) {
			// boundary0 가 설정된 Vertex 주위의 Vertex 중에서
			// smooth 가 설정되지 않은 Vertex 에 boundary 를 설정한다.
			for (DWORD prID = 0; prID < __numPointReprs; ++prID) {
				CPointRepr::Area2Flag&	flag = __pointReprPtr[prID].flag;
				if (flag.boundary0 == false) {
					flag.boundary = false;
					continue;
				}
{
#ifdef __USE_A_OFFSET__
		for (int k = pointer_to__a_vEdgeID_a_offset[prID]; k < pointer_to__a_vEdgeID_a_offset[prID + 1]; ++k) {
			const EDGE	e = pointer_to__a_vEdgeID[k];
			const CEdge&	edge = __edgePtr[e.id];
#else
		const CPointRepr&	pointRepr = __pointReprPtr[prID];
		for (size_t i = 0; i < pointRepr.vEdgeID.size(); ++i) {
			EDGE	e = pointRepr.vEdgeID.at(i);
			const CEdge&	edge = __edgePtr[e.id];
#endif
			DWORD	prID_ref = edge.prStartID == prID ? edge.prEndID : edge.prStartID;
			CPointRepr::Area2Flag&	flag_ref = __pointReprPtr[prID_ref].flag;
			flag_ref.boundary = !flag_ref.smooth;
#ifdef __USE_A_OFFSET__
		}
#else
		}
#endif
}
			}

			// boundary 가 설정된 Vertex 에 smooth 를 설정한다.
			for (DWORD prID = 0; prID < __numPointReprs; ++prID) {
				CPointRepr::Area2Flag&	flag = __pointReprPtr[prID].flag;
				if (flag.smooth)	// 이미 설정되었다면 건너뛴다.
					continue;
				flag.smooth = flag.boundary;
			}

			// boundary 가 설정된 Vertex 가 boundary0 가 설정된 Vertex 가 된다.
			for (DWORD prID = 0; prID < __numPointReprs; ++prID) {
				CPointRepr::Area2Flag&	flag = __pointReprPtr[prID].flag;
				flag.boundary0 = flag.boundary;
			}
		}
	}
for (int iter = 0; iter < (1 + (int) __statePtr->area2_control.SmoothIncLevel) * (int) __statePtr->brush.flag.SmoothLevel; ++iter)
{
	for (DWORD prID = 0; prID < __numPointReprs; ++prID) {
		if (__pointReprPtr[prID].flag.smooth == false)
			continue;

		kMesh::D3DXVERTEX&	v = pV[prID];
		v.pTmp = v.p;
	}
	for (DWORD prID = 0; prID < __numPointReprs; ++prID) {
		if (__pointReprPtr[prID].flag.smooth == false)
			continue;

		kMesh::D3DXVERTEX&	v = pV[prID];
/*
 * 각 Edge 를 구성하는 두 Vertex 의 중점이 각 Vertex 의 새로운 위치에 영향을 미친다;
 * Vertex 의 새로운 위치들의 centroid 와 원래 위치의 중점의 실제로 새로운 위치가 된다.
 */
		// 주어진 Vertex 를 한 끝으로 하는 모든 Edge 에서 Edge 의 중점을 구한다.
		// 이 점들의 centroid 와 원래 점의 중점이 새로운 위치가 된다.
		D3DXVECTOR3	centroid(0, 0, 0);
		unsigned	numPoints = 0;
#ifdef __USE_A_OFFSET__
		numPoints = pointer_to__a_vEdgeID_a_offset[prID + 1] - pointer_to__a_vEdgeID_a_offset[prID];
		for (int k = pointer_to__a_vEdgeID_a_offset[prID]; k < pointer_to__a_vEdgeID_a_offset[prID + 1]; ++k) {
			const EDGE	e = pointer_to__a_vEdgeID[k];
			const CEdge&	edge = __edgePtr[e.id];
			centroid += .5f * (pV[edge.prStartID].p + pV[edge.prEndID].p);
		}
#else
		const CPointRepr&	pointRepr = __pointReprPtr[prID];
		numPoints = pointRepr.vEdgeID.size();
		for (size_t i = 0; i < pointRepr.vEdgeID.size(); ++i) {
			EDGE	e = pointRepr.vEdgeID.at(i);
			const CEdge&	edge = __edgePtr[e.id];
			centroid += .5f * (pV[edge.prStartID].p + pV[edge.prEndID].p);
		}
#endif
//		cout << "numPoints = " << numPoints << endl;
		centroid /= (float) numPoints;
		v.pTmp = .5f * (v.p + centroid);
	}
	for (DWORD prID = 0; prID < __numPointReprs; ++prID) {
		if (__pointReprPtr[prID].flag.smooth == false)
			continue;

		D3DXVECTOR3&	p = pV[prID].p;
		p = pV[prID].pTmp;
	}
}
}
#endif	// Vertex 위치 변경; ZBrush Smooth Brush
}
}
#endif	// AREA2_HINT
#endif	// 특정 Vertex (Flag::repos 가 True 인) 의 위치를 재설정

	__vertexBufferPtr->Unlock();

#ifndef __KEKO__
#ifdef AREA2_HINT
#undef AREA2_HINT
#endif	// AREA2_HINT
#endif	// !__KEKO__

	return 0;
}
int	kMesh::__whole_body_simulation_finalize()
{
#if 1	// Lock...
	kMesh::D3DXVERTEX*	pV;	// kMesh::__vertexBufferPtr
	__vertexBufferPtr->Lock(0, 0, (void**) &pV, 0);
	CPointRepr::VERTEX*	vertexArray;	// 텍스쳐 메쉬 정보를 담은 Vertex Buffer
	__meshLoader.meshSubsetPtr()->LockVertexBuffer(0, (void**) &vertexArray);
	kShowNormal::Vertex*	pV_n;		// 노말 정보를 담은 Vertex Buffer
	__showNormal.GetVertexBufferPointer()->Lock(0, 0, (void**) &pV_n, 0);
	FLOAT*	pVB_ = NULL;
	__meshPtr->LockVertexBuffer(0, (void**) &pVB_);
#endif	// Lock...

#if 1	// __vertexBufferPtr 의 p 으로 해당하는 모든 값들 업데이트
	for (DWORD prID = 0; prID < (DWORD) __numPointReprs; ++prID) {
		if (
			!__pointReprPtr[prID].flag.force &&	// 무조건 움직이는 Vertex 가 아닌 경우에...
			!__pointReprPtr[prID].flag.move		// 움직이지 않은 Vertex 라면...
		)
			continue;

		kMesh::D3DXVERTEX&	v = pV[prID];
		D3DXVECTOR3&	p = v.p;
#if 1	// 한꺼번에...
		pV_n[2 * prID + 0].Position = p;
		pV_n[2 * prID + 1].Position = p;
		pVB_[8 * prID + 0] = p.x;
		pVB_[8 * prID + 1] = p.y;
		pVB_[8 * prID + 2] = p.z;
		__pointReprPtr[prID].v.position = p;
#if 1	// 텍스쳐 메쉬 변형
#ifdef __USE_A_OFFSET__
		for (int k = pointer_to__a_vVertexID_a_offset[prID]; k < pointer_to__a_vVertexID_a_offset[prID + 1]; ++k)
			vertexArray[pointer_to__a_vVertexID[k]].position = p;
#else
		const std::set<DWORD>&	vVertexID = __pointReprPtr[prID].vVertexID;
		for (std::set<DWORD>::const_iterator it = vVertexID.begin(); it != vVertexID.end(); ++it)
			vertexArray[*it].position = p;
#endif
#endif	// 텍스쳐 메쉬 변형
#endif	// 한꺼번에...
	}
#endif	// __vertexBufferPtr 의 p 으로 해당하는 모든 값들 업데이트

#if 1	// Vertex 의 새로운 노말 결정
{
#if 1	// common
	// 각 Vertex 의 노말을 다시 계산;
	// Vertex 를 공유하는 Face 들의 노말의 평균이 해당 Vertex 의 노말
	// 1. 각 Face 의 노말을 저장할 임시 공간
	for (DWORD faceID = 0; faceID < (DWORD) __numFaces; ++faceID) {
		// 적어도 하나의 Vertex 가 움직였다면 노말을 다시 계산한다.
		CFace&	face = __facePtr[faceID];
		if (
			// 어떤 Vertex 도 무조건 움직이는 Vertex 가 아닌 경우에...
			(
				!__pointReprPtr[face.prID0].flag.force &&
				!__pointReprPtr[face.prID1].flag.force &&
				!__pointReprPtr[face.prID2].flag.force
			) && (
			!face.flag.active ||	// 체형변화에 관련된 Face 가 아니거나
						// 연관되었더라도 어떤 Vertex 도 실제로 움직이지 않았다면
			(
				!__pointReprPtr[face.prID0].flag.move &&
				!__pointReprPtr[face.prID1].flag.move &&
				!__pointReprPtr[face.prID2].flag.move
			)
			)
		)
			continue;

		D3DXVECTOR3&	p0 = pV[face.prID0].p;
		D3DXVECTOR3&	p1 = pV[face.prID1].p;
		D3DXVECTOR3&	p2 = pV[face.prID2].p;
		D3DXVECTOR3	v01 = p1 - p0;
//		D3DXVec3Normalize(&v01, &v01);
		D3DXVECTOR3	v02 = p2 - p0;
//		D3DXVec3Normalize(&v02, &v02);
		D3DXVECTOR3&	normal = face.normal;
		D3DXVec3Cross(&normal, &v01, &v02);
		D3DXVec3Normalize(&normal, &normal);
	}
	// 2. 각 Vertex 의 노말을 공유하는 Face 들의 노말로부터 계산
	for (DWORD prID = 0; prID < (DWORD) __numPointReprs; ++prID) {
#if 0
		if (__pointReprPtr[prID].flag.subset == false)
			continue;
#endif

		D3DXVECTOR3&	normal = pV[prID].n;
		unsigned	numFaces = 0;
#ifdef __USE_A_OFFSET__
		numFaces = pointer_to__a_vFaceIDLocalIndex_a_offset[prID + 1] - pointer_to__a_vFaceIDLocalIndex_a_offset[prID];
#else
		numFaces = __pointReprPtr[prID].vFaceIdLocalIndex.size();
#endif
#ifdef __USE_A_OFFSET__
		for (int k = pointer_to__a_vFaceIDLocalIndex_a_offset[prID]; k < pointer_to__a_vFaceIDLocalIndex_a_offset[prID + 1]; ++k) {
			const FACE_ID_LOCAL_INDEX&	faceIDLocalIndex = pointer_to__a_vFaceIDLocalIndex[k];
#else
		for (size_t k = 0; k < __pointReprPtr[prID].vFaceIdLocalIndex.size(); ++k) {
			const FACE_ID_LOCAL_INDEX&	faceIDLocalIndex = __pointReprPtr[prID].vFaceIdLocalIndex.at(k);
#endif
			normal += __facePtr[faceIDLocalIndex.ID].normal;
#ifdef __USE_A_OFFSET__
		}
#else
		}
#endif
		normal /= (float) numFaces;
		D3DXVec3Normalize(&normal, &normal);
		// __meshPtr 에 사용되는 노말도 업데이트
	/*
		pVertices[i].n.x = pVB[8 * i + 3];
		pVertices[i].n.y = pVB[8 * i + 4];
		pVertices[i].n.z = pVB[8 * i + 5];
	*/
		pVB_[8 * prID + 3] = normal.x;
		pVB_[8 * prID + 4] = normal.y;
		pVB_[8 * prID + 5] = normal.z;
	}
#endif	// common
}
#endif	// Vertex 의 새로운 노말 결정

#if 1	// Unlock...
	__meshLoader.meshSubsetPtr()->UnlockVertexBuffer();
	__meshPtr->UnlockVertexBuffer();
	__showNormal.GetVertexBufferPointer()->Unlock();
	__vertexBufferPtr->Unlock();
#endif	// Unlock...

#if 1	// 마지막 주문; 10. 성형 후 등고선 재설정
	setIsometricLines(ISOMETRIC_XY, D3DXVECTOR3(0, 0, 1), MAX_SLICE / 2, __z_isometricVertexBufferPtr, __z_isometricLines);
	setIsometricLines(ISOMETRIC_YZ, D3DXVECTOR3(1, 0, 0), MAX_SLICE / 2, __x_isometricVertexBufferPtr, __x_isometricLines);
	setIsometricLines(ISOMETRIC_ZX, D3DXVECTOR3(0, 1, 0), MAX_SLICE / 2, __y_isometricVertexBufferPtr, __y_isometricLines);
#endif	// 마지막 주문; 10. 성형 후 등고선 재설정

	return 0;
}
#else
int
kMesh::__simulateWholeBody(float fScale)
{
#if 1
	typedef kBone::VERTEX_HASH	VERTEX_HASH;
	kMesh&	mesh = *this;
//	float	fScale = __statePtr->scale_alpha;
	// imported from kBone::apply_all
{
#ifndef __KEKO__
#define AREA2_HINT
//#ifdef AREA2_HINT
	bool	sub = fScale < 0;
//#endif	// AREA2_HINT
#endif	// !__KEKO__
#ifdef AREA2_HINT
	kState&	state = *mesh.statePtr();
#endif	// AREA2_HINT


#ifndef STANDARD_MODEL
	cerr << "kMesh::simulateWholeBody: No standard model" << endl;
	return -1;
#endif	// !STANDARD_MODEL
#ifndef __KEKO__
	int	index_to_standard = mesh.index_to_standard();
	if (
		index_to_standard != 1 /* ETRI 초기 표준모델 */ &&
		index_to_standard != 6 /* ETRI 091211 표준모델(남: Michael) */ &&
		index_to_standard != 7 /* ETRI 091211 표준모델(여: Victoria) */
	) {
		cerr << "kMesh::simulateWholeBody: index_to_standard = 1, 6, 7 only" << endl;
		return -1;
	}
#endif	// !__KEKO__
	if (!&mesh) {
		cerr << "FATAL: kMesh::simulateWholeBody: mesh is nil" << endl;
		return -1;
	}
	int	subset_index;	// 여기서 사용되는 몸의 subset 의 인덱스;
				// 전체(얼굴, 눈, 눈썹, 몸)이 있을 경우 3(ID = 0, 1),
				// 몸만 있는 경우에는 0 (ID = 3, 4, 5, 6, 7)
#ifdef __KEKO__
	subset_index = 0;
#else	// __KEKO__/!__KEKO__
	subset_index = index_to_standard == 1 /* ETRI 초기 표준모델 */ ? 3 : 0;
#endif	// !__KEKO__

	CPointRepr*	pointReprArray = mesh.GetPointReprPointer();
	CFace*		faceArray = mesh.GetFacePointer();
#ifndef __KEKO__
	CEdge*		edgeArray = mesh.GetEdgePointer();
#endif	// !__KEKO__
#ifdef __USE_A_OFFSET__
//	const DWORD*	pointer_to__a_vVertexID				= mesh.pointer_to__a_vVertexID;
//	const int*	pointer_to__a_vVertexID_a_offset		= mesh.pointer_to__a_vVertexID_a_offset;
	const EDGE*	pointer_to__a_vEdgeID				= mesh.pointer_to__a_vEdgeID;
	const int*	pointer_to__a_vEdgeID_a_offset			= mesh.pointer_to__a_vEdgeID_a_offset;
	const FACE_ID_LOCAL_INDEX*	pointer_to__a_vFaceIDLocalIndex	= mesh.pointer_to__a_vFaceIDLocalIndex;
	const int*	pointer_to__a_vFaceIDLocalIndex_a_offset	= mesh.pointer_to__a_vFaceIDLocalIndex_a_offset;
//	const DWORD*	pointer_to__a_faceID				= __meshPtr->pointer_to__a_faceID;
//	const int*	pointer_to__a_faceID_a_offset			= __meshPtr->pointer_to__a_faceID_a_offset;
#endif	// __USE_A_OFFSET__

#if 1	// FLAG
#else	// FLAG/!FLAG
	DWORD*	pointReprIDArray = 0;
#endif	// !FLAG
#if 1	// FLAG
	for (DWORD prID = 0; prID < __numPointReprs; ++prID)
		__pointReprPtr[prID].flag.reset();
#else	// FLAG/!FLAG
	pointReprIDArray = new DWORD[mesh.GetNumPointReprs()];
	FillMemory(pointReprIDArray, mesh.GetNumPointReprs() * sizeof(DWORD), 0);
#endif	// !FLAG
	// reset to false
	for (DWORD faceID = 0; faceID < __numFaces; ++faceID)
		__facePtr[faceID].flag.active = false;
	// set CFace::Flag::active flag
	DWORD*	subsetArray;
	mesh.meshLoader().meshPtr()->LockAttributeBuffer(0, &subsetArray);
	for (DWORD faceID = 0; faceID < (DWORD) mesh.GetNumFaces(); ++faceID) {
		const CFace&	face = faceArray[faceID];
		if (subsetArray[faceID] != (DWORD) subset_index)	// 0: victoria-face
						// 1: victoria-eyebrow
						// 2: victoria-eye
						// 3: victoria-body
			continue;

		__facePtr[faceID].flag.active = true;
		for (int i = 0; i < 3; ++i)
#if 1	// FLAG
			__pointReprPtr[face.prID[i]].flag.subset = true;
#else	// FLAG/!FLAG
			pointReprIDArray[face.prID[i]] = 1;
#endif	// !FLAG
	}
	mesh.meshLoader().meshPtr()->UnlockAttributeBuffer();

#if 1	// Lock...
//	CPointRepr::VERTEX*	vertexArray;	// 텍스쳐 메쉬 정보를 담은 Vertex Buffer
//	mesh.meshLoader().meshSubsetPtr()->LockVertexBuffer(0, (void**) &vertexArray);
//	DWORD*	subsetArray;
//	mesh.meshLoader().meshPtr()->LockAttributeBuffer(0, &subsetArray);
#if 1	// lock
//	LPDIRECT3DVERTEXBUFFER9	pVB_n = mesh.showNormal().GetVertexBufferPointer();
//	kShowNormal::Vertex*	pV_n;		// 노말 정보를 담은 Vertex Buffer
//	pVB_n->Lock(0, 0, (void**) &pV_n, 0);

	kMesh::D3DXVERTEX*	pV;	// kMesh::__vertexBufferPtr
	mesh.GetVertexBufferPointer()->Lock(0, 0, (void**) &pV, 0);

//	FLOAT*	pVB_ = NULL;
//	hResult = mesh.GetMeshPointer()->LockVertexBuffer(0, (void**) &pVB_);
//	assert(D3D_OK == hResult);
#endif	// lock
#endif	// Lock...

#if 1	// main
#ifdef AREA2_HINT
	struct Area2
	{
		float	before;
		float	after;
		bool	increase;	// 면적이 증가했는가?

		Area2() : increase(false) {}
	};
	typedef std::map<DWORD, Area2>	AREA2_HASH;
	AREA2_HASH	area2_dic;
#if 1	// FLAG
	VERTEX_HASH	vertex_dic;	// __pointReprPtr[prID].move 값이 True 인 Vertex 의 위치를 기억한다.
#else	// FLAG/!FLAG
	VERTEX_HASH	vertex_dic;	// pointReprArray[prID] = 2 인 Vertex 의 위치를 기억한다.
#endif	// !FLAG
#endif	// AREA2_HINT
#ifdef AREA2_HINT
if (sub) {
	cout << "**** before ****" << endl;
	for (DWORD faceID = 0; faceID < (DWORD) mesh.GetNumFaces(); ++faceID) {
		if (!__facePtr[faceID].flag.active)
			continue;

		CFace&	face = faceArray[faceID];
		D3DXVECTOR3&	p0 = pV[face.prID0].p;
		D3DXVECTOR3&	p1 = pV[face.prID1].p;
		D3DXVECTOR3&	p2 = pV[face.prID2].p;
		D3DXVECTOR3	v01 = p1 - p0;
		D3DXVECTOR3	v02 = p2 - p0;
		D3DXVECTOR3	Va;
		D3DXVec3Cross(&Va, &v01, &v02);
//		cout << "Area[" << face.id << "] = " << fabsf(D3DXVec3Length(&Va)) << endl;
		Area2&	area2 = area2_dic[faceID];
		area2.before = fabsf(D3DXVec3Length(&Va));
	}
}
#endif	// AREA2_HINT


{
#if 1	// Vertex 위치 변경; 노말방향으로...
#if 1	// kDisplacementMap
	cout << "**************** kDisplacementMap ****************" << endl;
	cout << "**************************************************" << endl;
//	const WCHAR	bmpFile[] = L"D:/home/icentrik/GDI+/gdiplus-1.0.1/debug/victoria-body.bmp";
#ifdef __KEKO__
	const WCHAR	bmpFile[] = L"./Resources/keko-dismap.bmp";
#if 1	// 텍스쳐도 변경
	kMeshLoader::Material&	material = *mesh.meshLoader().GetMaterials().GetAt(subset_index);
	SAFE_RELEASE(material.texturePtr);
	D3DXCreateTextureFromFile(DXUTGetD3D9Device(), bmpFile, &material.texturePtr);
#endif
#else	// __KEKO__/!__KEKO__
//	const WCHAR	bmpFile[] = L"D:/home/icentrik/My Documents/Visual Studio Projects/Pick/Media/Scanner/여자모델_Vic090609/tex/victoria-whole-body-displacement.bmp";
	WCHAR		bmpFile[MAX_PATH];
	swprintf_s(bmpFile, MAX_PATH, L"Media/Scanner/에트리표준모델-091211/%s/tex/%s-whole-body-displacement.bmp", index_to_standard == 6 ? L"michael" : L"victoria", index_to_standard == 6 ? L"michael" : L"victoria");
#if 0	// 텍스쳐도 변경
	kMeshLoader::Material&	material = *mesh.meshLoader().GetMaterials().GetAt(subset_index);
	SAFE_RELEASE(material.texturePtr);
	D3DXCreateTextureFromFile(DXUTGetD3D9Device(), bmpFile, &material.texturePtr);
#endif
#endif	// !__KEKO__
	kDisplacementMap	dismap(bmpFile);
	if (dismap.GetLastStatus() != Gdiplus::Ok) {
		cerr << "kDisplacementMap::kDisplacementMap(|" << bmpFile << "|): Failure: " << dismap.GetLastStatus() << endl;
	}
	cout << "***********************************************" << endl;
{
	float	amplitude = fScale * mesh.a();
	for (DWORD prID = 0; prID < (DWORD) mesh.GetNumPointReprs(); ++prID) {
		if (pointReprArray[prID].flag.fixed)
			continue;
#if 1	// FLAG
		if (__pointReprPtr[prID].flag.subset == false)
			continue;
#else	// FLAG/!FLAG
		if (pointReprIDArray[prID] == 0)
			continue;
#endif	// !FLAG

		kMesh::D3DXVERTEX&	v = pV[prID];

		D3DXVECTOR3&	p = v.p;
#if 1
{
#if 1	// smoothstep
		float	f_smooth = 1.f;
#if 0
{
		D3DXVECTOR3	ell_inside = v.p - P0;
		D3DXVECTOR3	ell_longitudinal = D3DXVec3Dot(&ell_inside, &__nDir) * __nDir;
		// P0~P2 사이를 세 구간으로 나눈다.
		// P0~P3, P3~P4, P4~P2
		// P0~P3, P4~P2 사이는 scale_transversal 값을
		// 1부터 fScale 까지 변화시킨다. (smoothstep 함수를 사용)
		// P3~P4 사이는 scale_transversal 값으로 fScale 값을 사용한다.
		// P0~P3 구간의 크기와 P4~P2 구간의 크기는 서로 다르나
		// 여기서는 같다고 가정하고 그것도 P0~P2 구간의 10%라고 가정한다.
		// (TO DO LIST)
		const	float	left_range = 0.2f;
		assert(left_range <= .5f);
		const	float	right_range = 1.f - left_range;
		float	length = D3DXVec3Length(&ell_longitudinal);
		if (length < __length * left_range) {		// P0~P3 구간
			f_smooth = smoothstep(0.f, 0.f, __length * left_range, 1.f, length);
		} else if (length > __length * right_range) {	// P4~P2 구간
			f_smooth = smoothstep(__length * right_range, 1.f, __length, 0.f, length);
		} else {				// P3~P4 구간
			f_smooth = 1.f;
		}
}
#endif
#endif	// smoothstep
#if 1	// GetPixel
		float	val;
{
		Gdiplus::Status	status = dismap.GetPixelAtUV(v.tu, v.tv, val, kDisplacementMap::CHANNEL_RED);
		if (status != Gdiplus::Ok) {
			cerr << "kDisplacementMap::GetPixel(): Failure" << endl;
			continue;
		} else {
			val /= dismap.MaxRed() / (float) 255;	// clamp to [0, 1]
		}
}
#endif	// GetPixel
		float	f = f_smooth * val;
#if 0
		if (fScale > 1.f) {
			// f = 1 + factor = 1 + alpha * factor
			float	factor = fScale - 1.f;
			factor *= val;
			f = 1.f + factor;
		} else {
			// f = 1 - factor = 1 - alpha * factor
			float	factor = 1.f - fScale;
			factor *= val;
			f = 1.f - factor;
		}
#endif
		if (f * fabsf(amplitude) < FLT_EPSILON) {
//			cout << "prID = " << prID << ": No move" << endl;
			continue;
		}
#if 1	// FLAG
		assert(__pointReprPtr[prID].flag.subset == true);
		__pointReprPtr[prID].flag.move = true;
#else	// FLAG/!FLAG
		assert(pointReprIDArray[prID] == 1);
		pointReprIDArray[prID] = 2;
#endif	// !FLAG
#ifdef AREA2_HINT
		if (sub)
			vertex_dic[prID] = p;
#endif	// AREA2_HINT
		p += (f * amplitude) * v.n;
}
#endif	// 변형

#if 0	// 아래에서 한꺼번에...
		pV_n[2 * prID + 0].Position = p;
		pV_n[2 * prID + 1].Position = p;
		pVB_[8 * prID + 0] = p.x;
		pVB_[8 * prID + 1] = p.y;
		pVB_[8 * prID + 2] = p.z;
		pointReprArray[prID].v.position = p;
#if 1	// 텍스쳐 메쉬 변형
#ifdef __USE_A_OFFSET__
		for (int k = mesh.pointer_to__a_vVertexID_a_offset[prID]; k < mesh.pointer_to__a_vVertexID_a_offset[prID + 1]; ++k)
			vertexArray[mesh.pointer_to__a_vVertexID[k]].position = p;
#else
		const std::set<DWORD>&	vVertexID = pointReprArray[prID].vVertexID;
		for (std::set<DWORD>::const_iterator it = vVertexID.begin(); it != vVertexID.end(); ++it)
			vertexArray[*it].position = p;
#endif
#endif	// 텍스쳐 메쉬 변형
#endif	// 아래에서 한꺼번에...
	}
}
#endif	// kDisplacementMap
#endif	// Vertex 위치 변경

#ifndef __KEKO__
#if 1	// Vertex 위치 변경; ZBrush Smooth Brush
if (!sub && state.area2_control.Smooth == 2) {
for (int iter = 0; iter < (1 + (int) state.area2_control.SmoothIncLevel) * (int) mesh.statePtr()->brush.flag.SmoothLevel; ++iter)
{
{
	unsigned	NumSmooth = 0;
	for (DWORD prID = 0; prID < (DWORD) mesh.GetNumPointReprs(); ++prID) {
#if 1	// FLAG
		if (__pointReprPtr[prID].flag.move == false)
			continue;
#else	// FLAG/!FLAG
		if (pointReprIDArray[prID] < 2)
			continue;
#endif	// !FLAG

		++NumSmooth;
		kMesh::D3DXVERTEX&	v = pV[prID];
		v.pTmp = v.p;
	}
	cout << "# of smoothed vertices = " << NumSmooth << endl;
	for (DWORD prID = 0; prID < (DWORD) mesh.GetNumPointReprs(); ++prID) {
#if 1	// FLAG
		if (__pointReprPtr[prID].flag.move == false)
			continue;
#else	// FLAG/!FLAG
		if (pointReprIDArray[prID] < 2)
			continue;
#endif	// !FLAG

		kMesh::D3DXVERTEX&	v = pV[prID];
/*
 * 각 Edge 를 구성하는 두 Vertex 의 중점이 각 Vertex 의 새로운 위치에 영향을 미친다;
 * Vertex 의 새로운 위치들의 centroid 와 원래 위치의 중점의 실제로 새로운 위치가 된다.
 */
		// 주어진 Vertex 를 한 끝으로 하는 모든 Edge 에서 Edge 의 중점을 구한다.
		// 이 점들의 centroid 와 원래 점의 중점이 새로운 위치가 된다.
		D3DXVECTOR3	centroid(0, 0, 0);
		unsigned	numPoints = 0;
#ifdef __USE_A_OFFSET__
		numPoints = pointer_to__a_vEdgeID_a_offset[prID + 1] - pointer_to__a_vEdgeID_a_offset[prID];
		for (int k = pointer_to__a_vEdgeID_a_offset[prID]; k < pointer_to__a_vEdgeID_a_offset[prID + 1]; ++k) {
			const EDGE	e = pointer_to__a_vEdgeID[k];
			const CEdge&	edge = edgeArray[e.id];
			centroid += .5f * (pV[edge.prStartID].p + pV[edge.prEndID].p);
		}
#else
		const CPointRepr&	pointRepr = pointReprArray[prID];
		numPoints = pointRepr.vEdgeID.size();
		for (size_t i = 0; i < pointRepr.vEdgeID.size(); ++i) {
			EDGE	e = pointRepr.vEdgeID.at(i);
			const CEdge&	edge = edgeArray[e.id];
			centroid += .5f * (pV[edge.prStartID].p + pV[edge.prEndID].p);
		}
#endif
//		cout << "numPoints = " << numPoints << endl;
		centroid /= (float) numPoints;
		v.pTmp = .5f * (v.p + centroid);
	}
	for (DWORD prID = 0; prID < (DWORD) mesh.GetNumPointReprs(); ++prID) {
#if 1	// FLAG
		if (__pointReprPtr[prID].flag.move == false)
			continue;
#else	// FLAG/!FLAG
		if (pointReprIDArray[prID] < 2)
			continue;
#endif	// !FLAG

		D3DXVECTOR3&	p = pV[prID].p;
		p = pV[prID].pTmp;

#if 0	// 아래에서 한꺼번에...
		pV_n[2 * prID + 0].Position = p;
		pV_n[2 * prID + 1].Position = p;
		pVB_[8 * prID + 0] = p.x;
		pVB_[8 * prID + 1] = p.y;
		pVB_[8 * prID + 2] = p.z;
		pointReprArray[prID].v.position = p;
#if 1	// 텍스쳐 메쉬 변형
#ifdef __USE_A_OFFSET__
		for (int k = mesh.pointer_to__a_vVertexID_a_offset[prID]; k < mesh.pointer_to__a_vVertexID_a_offset[prID + 1]; ++k)
			vertexArray[mesh.pointer_to__a_vVertexID[k]].position = p;
#else
		const std::set<DWORD>&	vVertexID = pointReprArray[prID].vVertexID;
		for (std::set<DWORD>::const_iterator it = vVertexID.begin(); it != vVertexID.end(); ++it)
			vertexArray[*it].position = p;
#endif
#endif	// 텍스쳐 메쉬 변형
#endif	// 아래에서 한꺼번에...
	}
}
}
}
#endif	// Vertex 위치 변경; ZBrush Smooth Brush
#endif	// !__KEKO__


#if 0	// 마지막에 처리
#if 1	// Vertex 의 새로운 노말 결정
{
#if 1	// common
	// 각 Vertex 의 노말을 다시 계산;
	// Vertex 를 공유하는 Face 들의 노말의 평균이 해당 Vertex 의 노말
	// 1. 각 Face 의 노말을 저장할 임시 공간
	for (DWORD faceID = 0; faceID < (DWORD) mesh.GetNumFaces(); ++faceID) {
		if (!__facePtr[faceID].flag.active)
			continue;

		CFace&	face = faceArray[faceID];
		D3DXVECTOR3&	p0 = pV[face.prID0].p;
		D3DXVECTOR3&	p1 = pV[face.prID1].p;
		D3DXVECTOR3&	p2 = pV[face.prID2].p;
		D3DXVECTOR3	v01 = p1 - p0;
//		D3DXVec3Normalize(&v01, &v01);
		D3DXVECTOR3	v02 = p2 - p0;
//		D3DXVec3Normalize(&v02, &v02);
		D3DXVECTOR3&	normal = face.normal;
		D3DXVec3Cross(&normal, &v01, &v02);
		D3DXVec3Normalize(&normal, &normal);
	}
	// 2. 각 Vertex 의 노말을 공유하는 Face 들의 노말로부터 계산
	for (DWORD prID = 0; prID < (DWORD) mesh.GetNumPointReprs(); ++prID) {
#if 1	// FLAG
		if (__pointReprPtr[prID].flag.subset == false)
			continue;
#else	// FLAG/!FLAG
		if (pointReprIDArray[prID] == 0)
			continue;
#endif	// !FLAG

		D3DXVECTOR3&	normal = pV[prID].n;
		unsigned	numFaces = 0;
#ifdef __USE_A_OFFSET__
		numFaces = pointer_to__a_vFaceIDLocalIndex_a_offset[prID + 1] - pointer_to__a_vFaceIDLocalIndex_a_offset[prID];
#else
		numFaces = pointReprArray[prID].vFaceIdLocalIndex.size();
#endif
#ifdef __USE_A_OFFSET__
		for (int k = pointer_to__a_vFaceIDLocalIndex_a_offset[prID]; k < pointer_to__a_vFaceIDLocalIndex_a_offset[prID + 1]; ++k) {
			const FACE_ID_LOCAL_INDEX&	faceIDLocalIndex = pointer_to__a_vFaceIDLocalIndex[k];
#else
		for (size_t k = 0; k < pointReprArray[prID].vFaceIdLocalIndex.size(); ++k) {
			const FACE_ID_LOCAL_INDEX&	faceIDLocalIndex = pointReprArray[prID].vFaceIdLocalIndex.at(k);
#endif
			normal += faceArray[faceIDLocalIndex.ID].normal;
#ifdef __USE_A_OFFSET__
		}
#else
		}
#endif
		normal /= (float) numFaces;
		D3DXVec3Normalize(&normal, &normal);
		// __meshPtr 에 사용되는 노말도 업데이트
	/*
		pVertices[i].n.x = pVB[8 * i + 3];
		pVertices[i].n.y = pVB[8 * i + 4];
		pVertices[i].n.z = pVB[8 * i + 5];
	*/
		pVB_[8 * prID + 3] = normal.x;
		pVB_[8 * prID + 4] = normal.y;
		pVB_[8 * prID + 5] = normal.z;
	}
#endif	// common
}
#endif	// Vertex 의 새로운 노말 결정
#endif	// 마지막에 처리
}


#ifdef AREA2_HINT
if (sub) {
	cout << "**** after ****" << endl;
	for (DWORD faceID = 0; faceID < (DWORD) mesh.GetNumFaces(); ++faceID) {
		if (!__facePtr[faceID].flag.active)
			continue;

		CFace&	face = faceArray[faceID];
		D3DXVECTOR3&	p0 = pV[face.prID0].p;
		D3DXVECTOR3&	p1 = pV[face.prID1].p;
		D3DXVECTOR3&	p2 = pV[face.prID2].p;
		D3DXVECTOR3	v01 = p1 - p0;
		D3DXVECTOR3	v02 = p2 - p0;
		D3DXVECTOR3	Va;
		D3DXVec3Cross(&Va, &v01, &v02);
//		cout << "Area[" << face.id << "] = " << fabsf(D3DXVec3Length(&Va)) << endl;
		Area2&	area2 = area2_dic[faceID];
		area2.after = fabsf(D3DXVec3Length(&Va));
	}
}
#endif	// AREA2_HINT
#ifdef AREA2_HINT
if (sub) {
#if 0	// debugging...
	state.area2_vertex_array.clear();
	cout << "# of area2_vertex_array = " << state.area2_vertex_array.size() << endl;
	AREA2_HASH::const_iterator	cit_hash;
	for (cit_hash = area2_dic.begin(); cit_hash != area2_dic.end(); ++cit_hash) {
		DWORD	faceID = (*cit_hash).first;
		const Area2&	area2 = (*cit_hash).second;
		cout << "Area2[" << faceID << "]: " << area2.before << " -> " << area2.after << endl;
		if (area2.after > area2.before) {
			cout << "hello, world" << endl;
			CFace&	face = faceArray[faceID];
//			state.debugging_FaceID_array.push_back(faceID);
			// 중복되게 prID 가 들어간다. 하지만 보는 데에는 지장이 없다.
			state.area2_vertex_array.push_back(face.prID[0]);
			state.area2_vertex_array.push_back(face.prID[1]);
			state.area2_vertex_array.push_back(face.prID[2]);
		}
	}
	cout << "# of area2_vertex_array = " << state.area2_vertex_array.size() << endl;
#endif	// debugging...
	AREA2_HASH::iterator	it_hash;
	int	num_inc_faces = 0;
	state.area2_face_array.clear();
	for (it_hash = area2_dic.begin(); it_hash != area2_dic.end(); ++it_hash) {
		DWORD	faceID = (*it_hash).first;
		Area2&	area2 = (*it_hash).second;
//		cout << "Area2[" << faceID << "]: " << area2.before << " -> " << area2.after << endl;
#if 1
		float	delta_area_percent = (area2.after - area2.before) / area2.before * 100.f;
		if (delta_area_percent > (float) state.Area2Threshold) {
			state.area2_face_array.push_back(faceID);
			++num_inc_faces;
			area2.increase = true;
			CFace&	face = faceArray[faceID];
			for (int i = 0; i < 3; ++i) {
				DWORD	prID = face.prID[i];
#if 1	// FLAG
				if (!pointReprArray[prID].flag.fixed)
					__pointReprPtr[prID].flag.inc = true;
#else	// FLAG/!FLAG
				switch (pointReprIDArray[prID]) {
				case 3: // 이미 설정했다; face 단위로 처리되므로 같은 prID 가 두 번 설정될 수 있다...
//					cout << "pointReprIDArray[" << prID << "] = " << pointReprIDArray[prID] << " (= 3)" << endl;
					break;
				case 2:	// 움직인 점
					pointReprIDArray[prID] = 3;
					break;
				case 1:	// 움직이지 않는 점
					break;
				default:
					cout << "pointReprIDArray[" << prID << "] = " << pointReprIDArray[prID] << " (< 1!)" << endl;
					break;
				}
#endif	// !FLAG
			}
		}
#else
		if (area2.after > area2.before) {
			area2.increase = true;
			CFace&	face = faceArray[faceID];
			for (int i = 0; i < 3; ++i) {
				DWORD	prID = face.prID[i];
				switch (pointReprIDArray[prID]) {
				case 3: // 이미 설정했다; face 단위로 처리되므로 같은 prID 가 두 번 설정될 수 있다...
//					cout << "pointReprIDArray[" << prID << "] = " << pointReprIDArray[prID] << " (= 3)" << endl;
					break;
				case 2:	// 움직인 점
					pointReprIDArray[prID] = 3;
					break;
				case 1:	// 움직이지 않는 점
					break;
				default:
					cout << "pointReprIDArray[" << prID << "] = " << pointReprIDArray[prID] << " (< 1!)" << endl;
					break;
				}
			}
		}
#endif
	}
//	cout << "# of face area incresed = " << num_inc_faces << endl;
	for (DWORD prID = 0; prID < (DWORD) mesh.GetNumPointReprs(); ++prID) {
#if 1	// FLAG
		if (__pointReprPtr[prID].flag.inc == false)
			continue;
#else	// FLAG/!FLAG
		if (pointReprIDArray[prID] < 3)
			continue;
#endif	// !FLAG

		// 이 점이 움직이면 이 점을 공유하는 주변의 삼각형 중의 적어도 하나의 면적이 증가한다.
		// 몇 개의 삼각형의 면적이 증가하는가?
		unsigned	numFaces = 0;
		unsigned	numFaces_area_increased = 0;
#ifdef __USE_A_OFFSET__
		numFaces = pointer_to__a_vFaceIDLocalIndex_a_offset[prID + 1] - pointer_to__a_vFaceIDLocalIndex_a_offset[prID];
#else
		numFaces = pointReprArray[prID].vFaceIdLocalIndex.size();
#endif
#ifdef __USE_A_OFFSET__
		for (int k = pointer_to__a_vFaceIDLocalIndex_a_offset[prID]; k < pointer_to__a_vFaceIDLocalIndex_a_offset[prID + 1]; ++k) {
			const FACE_ID_LOCAL_INDEX&	faceIDLocalIndex = pointer_to__a_vFaceIDLocalIndex[k];
#else
		for (size_t k = 0; k < pointReprArray[prID].vFaceIdLocalIndex.size(); ++k) {
			const FACE_ID_LOCAL_INDEX&	faceIDLocalIndex = pointReprArray[prID].vFaceIdLocalIndex.at(k);
#endif
			DWORD	faceID = faceIDLocalIndex.ID;
			AREA2_HASH::const_iterator	cit = area2_dic.find(faceID);
			if (cit == area2_dic.end()) {
				// 다른 subset 에 있는 Face
				continue;
			}
			const Area2&	area2 = (*cit).second;
			if (area2.increase)
				++numFaces_area_increased;
#ifdef __USE_A_OFFSET__
		}
#else
		}
#endif
//		cout << "prID = " << prID << ": " << numFaces_area_increased << "/" << numFaces << endl;
		if (numFaces_area_increased == numFaces)	// TO DO LIST; all or nothing?
#if 1	// FLAG
			__pointReprPtr[prID].flag.all = true;
#else	// FLAG/!FLAG
			pointReprIDArray[prID] = 4;
#endif	// !FLAG
	}
#if 0	// debugging...
if (state.area2_control.ShowArea2 == 1) {	// inc
	state.area2_vertex_array.clear();
	for (DWORD prID = 0; prID < (DWORD) mesh.GetNumPointReprs(); ++prID) {
#if 1	// FLAG
		if (__pointReprPtr[prID].flag.inc == false)
			continue;
#else	// FLAG/!FLAG
		if (pointReprIDArray[prID] < 4)
			continue;
#endif	// !FLAG

		state.area2_vertex_array.push_back(prID);
	}
}
if (state.area2_control.ShowArea2 == 2) {	// all
	state.area2_vertex_array.clear();
	for (DWORD prID = 0; prID < (DWORD) mesh.GetNumPointReprs(); ++prID) {
#if 1	// FLAG
		if (__pointReprPtr[prID].flag.all == false)
			continue;
#else	// FLAG/!FLAG
		if (pointReprIDArray[prID] < 4)
			continue;
#endif	// !FLAG

		state.area2_vertex_array.push_back(prID);
	}
}
#endif	// debugging...
}
#endif	// AREA2_HINT

#if 1	// 특정 Vertex (Flag::repos 가 True 인) 의 위치를 재설정
#ifdef AREA2_HINT	// 특정 Vertex (Flag::repos 가 True 인) 의 위치를 재설정
if (
	sub &&
	(state.area2_control.ShowArea2 == 1 || state.area2_control.ShowArea2 == 2) &&
	state.area2_control.Repos
) {
#if 1	// FLAG
	// __pointReprPtr[prID].repos 값이 True 인 Vertex 들을 원래 위치로
	// repos 를 설정...; TO DO LIST
if (state.area2_control.ShowArea2 == 1) {
	for (DWORD prID = 0; prID < (DWORD) mesh.GetNumPointReprs(); ++prID) {
		if (pointReprArray[prID].flag.fixed)
			continue;
		__pointReprPtr[prID].flag.repos = __pointReprPtr[prID].flag.inc;
	}
} else if (state.area2_control.ShowArea2 == 2) {
	cerr << "ShowArea2 = 2; not implemented yet" << endl;
}
#else	// FLAG/!FLAG
	DWORD	id = 4;
	// pointReprIDArray[prID] = id 인 Vertex 들을 원래 위치로
#endif	// !FLAG
{
#if 1	// Vertex 위치 변경; 노말방향으로...
#if 1	// LOOPing
	int	num_rest = 0;	// LOOPing 후에도 위치가 아직 결정되지 않은 Vertex 의 갯수
	int	loop_counter = 0;	// 문제가 있을 때 무한루프에 빠지지 않도록...
#define MAX_LOOP	10		// 최대 MAX_LOOP 만큼 반복
do {
	++loop_counter;
	cout << "LOOPing..." << endl;
	num_rest = 0;	// 초기화
#endif	// LOOPing
	for (DWORD prID = 0; prID < (DWORD) mesh.GetNumPointReprs(); ++prID) {
#if 1	// FLAG
		if (__pointReprPtr[prID].flag.repos == false)
			continue;
#else	// FLAG/!FLAG
		if (pointReprIDArray[prID] != id)
			continue;
#endif	// !FLAG

		kMesh::D3DXVERTEX&	v = pV[prID];

		D3DXVECTOR3&	p = v.p;
#if 1	// LOOPing
//		cout << "prID = " << prID << endl;
		if (__pointReprPtr[prID].flag.done == true)
			continue;
		// 이 Vertex 를 지금 단계에서 바꿀 수 있는 지 알아본다;
		// 이 Vertex 주변의 Vertex 들 중에서
		//	1) Flag::all 이 False 이거나 즉 Flag::inc 가 False 이거나
		//	Flag::inc 가 True 이더라도 어떤 기준에 의해 Flag::all 이 False 이거나
		//	2) Flag::done 이 True 인 Vertex 가 있는 지 알아본다.
		int	num_hint_vertex = 0;	// 이 Vertex 가 움직일 방향을 결정해줄 주변 Vertex 의 갯수
		D3DXVECTOR3	avg_p(0, 0, 0);	// num_hint_vertex 개의 주변 Vertex 들에 의해 결정되는
						// 이 Vertex 이 "평균" 위치
{
#ifdef __USE_A_OFFSET__
		for (int k = pointer_to__a_vEdgeID_a_offset[prID]; k < pointer_to__a_vEdgeID_a_offset[prID + 1]; ++k) {
			const EDGE	e = pointer_to__a_vEdgeID[k];
			const CEdge&	edge = edgeArray[e.id];
#else
		const CPointRepr&	pointRepr = pointReprArray[prID];
		for (size_t i = 0; i < pointRepr.vEdgeID.size(); ++i) {
			EDGE	e = pointRepr.vEdgeID.at(i);
			const CEdge&	edge = edgeArray[e.id];
#endif
			DWORD	prID_ref = edge.prStartID == prID ? edge.prEndID : edge.prStartID;
			const CPointRepr::Area2Flag&	flag = __pointReprPtr[prID_ref].flag;
			if (flag.repos && !flag.done)
				continue;
//			cout << "prID_ref = " << prID_ref << endl;
			// 이 Vertex 에서 주어진 Vertex 에 이르는 원래 벡터만큼,
			// 이 Vertex 의 새로운 위치에 더해 주어진 Vertex 의 새로운 위치를 결정한다.
			if (vertex_dic.find(prID_ref) == vertex_dic.end()) {
				cout << "prID_ref = " << prID_ref << "(" << prID << "): No vertex_dic" << endl;
				cout << ".fixed? " << (pointReprArray[prID_ref].flag.fixed ? "True" : "False") << endl;
				continue;
			}
			avg_p += pV[prID_ref].p + (vertex_dic[prID] - vertex_dic[prID_ref]);
			++num_hint_vertex;
#ifdef __USE_A_OFFSET__
		}
#else
		}
#endif
}
//		cout << "num_hint_vertex = " << num_hint_vertex << endl;
		if (num_hint_vertex == 0) {
			++num_rest;
			continue;
		}
		p = avg_p / (float) num_hint_vertex;
		__pointReprPtr[prID].flag.done = true;
#else	// LOOPing/!LOOPing
//		p = .5f * (vertex_dic[prID] + p);	// 원래 위치로...
		p = vertex_dic[prID];	// 원래 위치로...
#endif	// LOOPing

#if 0	// 아래에서 한꺼번에...
		pV_n[2 * prID + 0].Position = p;
		pV_n[2 * prID + 1].Position = p;
		pVB_[8 * prID + 0] = p.x;
		pVB_[8 * prID + 1] = p.y;
		pVB_[8 * prID + 2] = p.z;
		pointReprArray[prID].v.position = p;
#if 1	// 텍스쳐 메쉬 변형
#ifdef __USE_A_OFFSET__
		for (int k = mesh.pointer_to__a_vVertexID_a_offset[prID]; k < mesh.pointer_to__a_vVertexID_a_offset[prID + 1]; ++k)
			vertexArray[mesh.pointer_to__a_vVertexID[k]].position = p;
#else
		const std::set<DWORD>&	vVertexID = pointReprArray[prID].vVertexID;
		for (std::set<DWORD>::const_iterator it = vVertexID.begin(); it != vVertexID.end(); ++it)
			vertexArray[*it].position = p;
#endif
#endif	// 텍스쳐 메쉬 변형
#endif	// 아래에서 한꺼번에...
	}
#if 1	// LOOPing
} while (num_rest && loop_counter < MAX_LOOP);
#endif	// LOOPing
#endif	// Vertex 위치 변경

#if 1	// Vertex 위치 변경; ZBrush Smooth Brush
if (state.area2_control.Smooth == 1 || state.area2_control.Smooth == 2) {
	// Smoothing 에 사용될 Vertex 는...
	for (DWORD prID = 0; prID < (DWORD) mesh.GetNumPointReprs(); ++prID) {
		if (state.area2_control.Smooth == 1) {
			if (state.area2_control.ShowArea2 == 1)
				__pointReprPtr[prID].flag.smooth = __pointReprPtr[prID].flag.inc;
			else if (state.area2_control.ShowArea2 == 1)
				__pointReprPtr[prID].flag.smooth = __pointReprPtr[prID].flag.all;
		} else if (state.area2_control.Smooth == 2)
			__pointReprPtr[prID].flag.smooth = __pointReprPtr[prID].flag.move;
	}
	if (state.area2_control.Smooth == 1 && state.area2_control.SmoothIncLevel) {
		// smooth 가 설정된 Vertex 에 대해 boundary0 설정
		for (DWORD prID = 0; prID < (DWORD) mesh.GetNumPointReprs(); ++prID) {
			CPointRepr::Area2Flag&	flag = __pointReprPtr[prID].flag;
			flag.boundary0 = flag.smooth;
		}
		for (unsigned n = 0; n < state.area2_control.SmoothIncLevel; ++n) {
			// boundary0 가 설정된 Vertex 주위의 Vertex 중에서
			// smooth 가 설정되지 않은 Vertex 에 boundary 를 설정한다.
			for (DWORD prID = 0; prID < (DWORD) mesh.GetNumPointReprs(); ++prID) {
				CPointRepr::Area2Flag&	flag = __pointReprPtr[prID].flag;
				if (flag.boundary0 == false) {
					flag.boundary = false;
					continue;
				}
{
#ifdef __USE_A_OFFSET__
		for (int k = pointer_to__a_vEdgeID_a_offset[prID]; k < pointer_to__a_vEdgeID_a_offset[prID + 1]; ++k) {
			const EDGE	e = pointer_to__a_vEdgeID[k];
			const CEdge&	edge = edgeArray[e.id];
#else
		const CPointRepr&	pointRepr = pointReprArray[prID];
		for (size_t i = 0; i < pointRepr.vEdgeID.size(); ++i) {
			EDGE	e = pointRepr.vEdgeID.at(i);
			const CEdge&	edge = edgeArray[e.id];
#endif
			DWORD	prID_ref = edge.prStartID == prID ? edge.prEndID : edge.prStartID;
			CPointRepr::Area2Flag&	flag_ref = __pointReprPtr[prID_ref].flag;
			flag_ref.boundary = !flag_ref.smooth;
#ifdef __USE_A_OFFSET__
		}
#else
		}
#endif
}
			}

			// boundary 가 설정된 Vertex 에 smooth 를 설정한다.
			for (DWORD prID = 0; prID < (DWORD) mesh.GetNumPointReprs(); ++prID) {
				CPointRepr::Area2Flag&	flag = __pointReprPtr[prID].flag;
				if (flag.smooth)	// 이미 설정되었다면 건너뛴다.
					continue;
				flag.smooth = flag.boundary;
			}

			// boundary 가 설정된 Vertex 가 boundary0 가 설정된 Vertex 가 된다.
			for (DWORD prID = 0; prID < (DWORD) mesh.GetNumPointReprs(); ++prID) {
				CPointRepr::Area2Flag&	flag = __pointReprPtr[prID].flag;
				flag.boundary0 = flag.boundary;
			}
		}
	}
for (int iter = 0; iter < (1 + (int) state.area2_control.SmoothIncLevel) * (int) mesh.statePtr()->brush.flag.SmoothLevel; ++iter)
{
	unsigned	NumSmooth = 0;
	for (DWORD prID = 0; prID < (DWORD) mesh.GetNumPointReprs(); ++prID) {
#if 1	// FLAG
		if (__pointReprPtr[prID].flag.smooth == false)
			continue;
#else	// FLAG/!FLAG
		if (pointReprIDArray[prID] != id)
			continue;
#endif	// !FLAG

		++NumSmooth;
		kMesh::D3DXVERTEX&	v = pV[prID];
		v.pTmp = v.p;
	}
	cout << "# of smoothed vertices = " << NumSmooth << endl;
	for (DWORD prID = 0; prID < (DWORD) mesh.GetNumPointReprs(); ++prID) {
#if 1	// FLAG
		if (__pointReprPtr[prID].flag.smooth == false)
			continue;
#else	// FLAG/!FLAG
		if (pointReprIDArray[prID] != id)
			continue;
#endif	// !FLAG

		kMesh::D3DXVERTEX&	v = pV[prID];
/*
 * 각 Edge 를 구성하는 두 Vertex 의 중점이 각 Vertex 의 새로운 위치에 영향을 미친다;
 * Vertex 의 새로운 위치들의 centroid 와 원래 위치의 중점의 실제로 새로운 위치가 된다.
 */
		// 주어진 Vertex 를 한 끝으로 하는 모든 Edge 에서 Edge 의 중점을 구한다.
		// 이 점들의 centroid 와 원래 점의 중점이 새로운 위치가 된다.
		D3DXVECTOR3	centroid(0, 0, 0);
		unsigned	numPoints = 0;
#ifdef __USE_A_OFFSET__
		numPoints = pointer_to__a_vEdgeID_a_offset[prID + 1] - pointer_to__a_vEdgeID_a_offset[prID];
		for (int k = pointer_to__a_vEdgeID_a_offset[prID]; k < pointer_to__a_vEdgeID_a_offset[prID + 1]; ++k) {
			const EDGE	e = pointer_to__a_vEdgeID[k];
			const CEdge&	edge = edgeArray[e.id];
			centroid += .5f * (pV[edge.prStartID].p + pV[edge.prEndID].p);
		}
#else
		const CPointRepr&	pointRepr = __pointReprPtr[prID];
		numPoints = pointRepr.vEdgeID.size();
		for (size_t i = 0; i < pointRepr.vEdgeID.size(); ++i) {
			EDGE	e = pointRepr.vEdgeID.at(i);
			const CEdge&	edge = edgeArray[e.id];
			centroid += .5f * (pV[edge.prStartID].p + pV[edge.prEndID].p);
		}
#endif
//		cout << "numPoints = " << numPoints << endl;
		centroid /= (float) numPoints;
		v.pTmp = .5f * (v.p + centroid);
	}
	for (DWORD prID = 0; prID < (DWORD) mesh.GetNumPointReprs(); ++prID) {
#if 1	// FLAG
		if (__pointReprPtr[prID].flag.smooth == false)
			continue;
#else	// FLAG/!FLAG
		if (pointReprIDArray[prID] != id)
			continue;
#endif	// !FLAG

		D3DXVECTOR3&	p = pV[prID].p;
		p = pV[prID].pTmp;

#if 0	// 아래에서 한꺼번에...
		pV_n[2 * prID + 0].Position = p;
		pV_n[2 * prID + 1].Position = p;
		pVB_[8 * prID + 0] = p.x;
		pVB_[8 * prID + 1] = p.y;
		pVB_[8 * prID + 2] = p.z;
		pointReprArray[prID].v.position = p;
#if 1	// 텍스쳐 메쉬 변형
#ifdef __USE_A_OFFSET__
		for (int k = mesh.pointer_to__a_vVertexID_a_offset[prID]; k < mesh.pointer_to__a_vVertexID_a_offset[prID + 1]; ++k)
			vertexArray[mesh.pointer_to__a_vVertexID[k]].position = p;
#else
		const std::set<DWORD>&	vVertexID = pointReprArray[prID].vVertexID;
		for (std::set<DWORD>::const_iterator it = vVertexID.begin(); it != vVertexID.end(); ++it)
			vertexArray[*it].position = p;
#endif
#endif	// 텍스쳐 메쉬 변형
#endif	// 아래에서 한꺼번에...
	}
}
}
#endif	// Vertex 위치 변경; ZBrush Smooth Brush


#if 0	// 마지막에 처리
#if 1	// Vertex 의 새로운 노말 결정
{
#if 1	// common
	// 각 Vertex 의 노말을 다시 계산;
	// Vertex 를 공유하는 Face 들의 노말의 평균이 해당 Vertex 의 노말
	// 1. 각 Face 의 노말을 저장할 임시 공간
	for (DWORD faceID = 0; faceID < (DWORD) mesh.GetNumFaces(); ++faceID) {
		if (!__facePtr[faceID].flag.active)
			continue;

		CFace&	face = faceArray[faceID];
		D3DXVECTOR3&	p0 = pV[face.prID0].p;
		D3DXVECTOR3&	p1 = pV[face.prID1].p;
		D3DXVECTOR3&	p2 = pV[face.prID2].p;
		D3DXVECTOR3	v01 = p1 - p0;
//		D3DXVec3Normalize(&v01, &v01);
		D3DXVECTOR3	v02 = p2 - p0;
//		D3DXVec3Normalize(&v02, &v02);
		D3DXVECTOR3&	normal = face.normal;
		D3DXVec3Cross(&normal, &v01, &v02);
		D3DXVec3Normalize(&normal, &normal);
	}
	// 2. 각 Vertex 의 노말을 공유하는 Face 들의 노말로부터 계산
	for (DWORD prID = 0; prID < (DWORD) mesh.GetNumPointReprs(); ++prID) {
#if 1	// FLAG
		if (__pointReprPtr[prID].flag.subset == false)
			continue;
#else	// FLAG/!FLAG
		if (pointReprIDArray[prID] == 0)
			continue;
#endif	// !FLAG

		D3DXVECTOR3&	normal = pV[prID].n;
		unsigned	numFaces = 0;
#ifdef __USE_A_OFFSET__
		numFaces = pointer_to__a_vFaceIDLocalIndex_a_offset[prID + 1] - pointer_to__a_vFaceIDLocalIndex_a_offset[prID];
#else
		numFaces = pointReprArray[prID].vFaceIdLocalIndex.size();
#endif
#ifdef __USE_A_OFFSET__
		for (int k = pointer_to__a_vFaceIDLocalIndex_a_offset[prID]; k < pointer_to__a_vFaceIDLocalIndex_a_offset[prID + 1]; ++k) {
			const FACE_ID_LOCAL_INDEX&	faceIDLocalIndex = pointer_to__a_vFaceIDLocalIndex[k];
#else
		for (size_t k = 0; k < pointReprArray[prID].vFaceIdLocalIndex.size(); ++k) {
			const FACE_ID_LOCAL_INDEX&	faceIDLocalIndex = pointReprArray[prID].vFaceIdLocalIndex.at(k);
#endif
			normal += faceArray[faceIDLocalIndex.ID].normal;
#ifdef __USE_A_OFFSET__
		}
#else
		}
#endif
		normal /= (float) numFaces;
		D3DXVec3Normalize(&normal, &normal);
		// __meshPtr 에 사용되는 노말도 업데이트
	/*
		pVertices[i].n.x = pVB[8 * i + 3];
		pVertices[i].n.y = pVB[8 * i + 4];
		pVertices[i].n.z = pVB[8 * i + 5];
	*/
		pVB_[8 * prID + 3] = normal.x;
		pVB_[8 * prID + 4] = normal.y;
		pVB_[8 * prID + 5] = normal.z;
	}
#endif	// common
}
#endif	// Vertex 의 새로운 노말 결정
#endif	// 마지막에 처리
}
}
#endif	// AREA2_HINT
#endif	// 특정 Vertex (Flag::repos 가 True 인) 의 위치를 재설정
#endif	// main

	mesh.GetVertexBufferPointer()->Unlock();


#if 1	// FLAG
//	SAFE_DELETE_ARRAY(__pointReprPtr);
	// reset CPointRepr::Area2Flag
	for (DWORD prID = 0; prID < __numPointReprs; ++prID)
		__pointReprPtr[prID].flag.reset();
#else	// FLAG/!FLAG
	SAFE_DELETE_ARRAY(pointReprIDArray);
#endif	// !FLAG
	// reset to false
	for (DWORD faceID = 0; faceID < __numFaces; ++faceID)
		__facePtr[faceID].flag.active = false;
}
#endif

	return 0;
}
#endif
int
kMesh::simulateWholeBody(
	float	scale,		// [0, 1] 사이의 값
	float*	dV_ptr,	// 부피 변화(단위: liter)
	float*	V_ptr	// 변화 후 최종 부피(단위: liter)
)
{
	if (__whole_body_simulation_initialize(scale) < 0) {
		cerr << "kMesh::simulateWholeBody(float scale, ...): __whole_body_simulation_initialize(): Failure" << endl;
		return -1;
	}

#if 1	// main
	bool	use_p_False = false;	// V(p)
	float	vol0;
	CalculateTotalVolume(use_p_False, &vol0);

	if (__whole_body_simulation_do(scale) < 0) {
		cerr << "kMesh::simulateWholeBody(float scale, ...): __whole_body_simulation_do(): Failure" << endl;
		return -1;
	}

	float	vol;
	CalculateTotalVolume(use_p_False, &vol);
	float	dV = vol - vol0;
//	cout << "dV = " << dV / 1000000.f << " ml" << endl;	// (1mm)^3 = 10^-3 cm^3 = 10^-3 mL = 10-6 L
#endif	// main

	if (__whole_body_simulation_finalize() < 0) {
		cerr << "kMesh::simulateWholeBody(float scale, ...): __whole_body_simulation_finalize(): Failure" << endl;
	}

#if 1	// set return values...
	if (dV_ptr)
		*dV_ptr = dV / 1000000.f;	// (1mm)^3 = 10^-3 cm^3 = 10^-3 mL = 10-6 L
	if (V_ptr)
		*V_ptr = vol / 1000000.f;	// (1mm)^3 = 10^-3 cm^3 = 10^-3 mL = 10-6 L
#endif	// set return values...

	// 다시 측정하도록 한다.
	SetAllMeasureDirty();

	return 0;
}
int
kMesh::simulateWholeBody0(
	bool	show,
	float	dV0,	// 초기 부피로부터의 변화량 = vol - vol0
	float&	scale /* OUT */,
	float*	dV_ptr,	// 실제로 변화한 양
	float	dV_percent_error0,
	float*	dV_percent_error_ptr
)
{
	// 반복되는 dV 값
	float	dV_repeat = dV0 > 0 ? -FLT_MAX : FLT_MAX;
	// 반복되는 dV 횟수
	unsigned	num_repeat = 0;

	std::vector<D3DXVECTOR3>	savedVertexArray;
{
	kMesh::D3DXVERTEX*	pV;	// kMesh::__vertexBufferPtr
	__vertexBufferPtr->Lock(0, 0, (void**) &pV, 0);
	for (DWORD prID = 0; prID < __numPointReprs; ++prID) {
		kMesh::D3DXVERTEX&	v = pV[prID];
		savedVertexArray.push_back(v.p);
	}
	__vertexBufferPtr->Unlock();
}
	__timer.GetElapsedTime();		// 각 단계마다 걸린 시간을 알기위해
	(void) fprintf(stdout, "simulating... (%f)\n", __timer.GetElapsedTime());
	bool	use_p_False = true;	// V(p_)
	float	vol0;	// 원래 부피
	cout << "m__fTotalVolume0 = " << m__fTotalVolume0 / 1000000.f << endl;
	CalculateTotalVolume(use_p_False, &vol0);
	cout << "vol0 = " << vol0 / 1000000.f << endl;
#if 0
	float	vol_now;	// 현재 부피
	CalculateTotalVolume(!use_p_False, &vol_now);
	cout << "vol_now = " << vol_now / 1000000.f << endl;
	// dV0 = vol - vol_now
	//     = vol - vol0 + (vol0 - vol_now)
	// vol - vol0 = dV0 - (vol0 - vol_now)
	cout << "dV0 = " << dV0 << endl;
	dV0 -= (vol0 - vol_now) / 1000000.f;
	cout << "dV0 = " << dV0 << endl;
#endif

	float	scale_low = dV0 > 0 ? 0.f : 2 * kState::SCALE_MIN;
	float	scale_high = dV0 > 0 ? 2 * kState::SCALE_MAX : 0.f;

	bool	found = false;
#define NITER_MAX	100
	unsigned	niter = 0;
do {
	++niter;

//	cout << "TRACE: iter = " << niter << ": [" << scale_low << ", " << scale_high << "]" << endl;
	scale = .5f * (scale_low + scale_high);
#if 1	// 주어진 scale 에 해당하는 dV 를 얻는다.
	float	dV;
#if 1	// Simulation Cache
	SIMULATION_CACHE_HASH::const_iterator	cit = __simulationCache.find(*((int*) &scale));
	bool	cached = false;
	if (cit != __simulationCache.end()) {
		cached = true;
		dV = (*cit).second;
		cout << "scale = " << scale << ": cached w/ dV = " << dV << endl;
	} else {
		cached = false;
#endif	// Simulation Cache
	kMesh::D3DXVERTEX*	pV;	// kMesh::__vertexBufferPtr
	__vertexBufferPtr->Lock(0, 0, (void**) &pV, 0);
	for (DWORD prID = 0; prID < __numPointReprs; ++prID) {
		kMesh::D3DXVERTEX&	v = pV[prID];
		v.p = v.p_;
//		D3DXVECTOR3&	p = v.p;
//		p = __pointReprPtr[prID].v.position;
	}
	__vertexBufferPtr->Unlock();
	if (__whole_body_simulation_initialize(scale) < 0) {
		cerr << "kMesh::simulateWholeBody0(float dV0, ...): __whole_body_simulation_initialize(): Failure" << endl;
		return -1;
	}

	if (__whole_body_simulation_do(scale) < 0) {
		cerr << "kMesh::simulateWholeBody0(float dV0, ...): __whole_body_simulation_do(): Failure" << endl;
		return -1;
	}

	bool	use_p_False = false;	// V(p)
	float	vol;	// 시뮬레이션 후 부피
	CalculateTotalVolume(use_p_False, &vol);
	dV = (vol - vol0) / 1000000.f;	// (1mm)^3 = 10^-3 cm^3 = 10^-3 mL = 10-6 L
#if 1	// Simulation Cache
	__simulationCache[*((int*) &scale)] = dV;
	}
#endif	// Simulation Cache
#endif	// 주어진 scale 에 해당하는 dV 를 얻는다.
//	cout << "TRACE: iter = " << niter << ": scale = " << scale << ", dV = " << dV << " (dV0 = " << dV0 << ")" << endl;
//	cout << "TRACE: iter = " << niter << ": scale = " << scale << ", dV = " << dV << " (dV0 = " << dV0 << ")";
	(void) fprintf(stdout,
		"TRACE: iter = %d: %.3e <= scale = %.3e <= %.3e(%s)"
		,
		niter,
		scale_low, scale, scale_high,
		fabsf(scale_high - scale_low) < FLT_EPSILON ? "True" : "False"
//		niter, scale_low, scale_high, scale_high == scale_low ? "True" : "False"
	);
	(void) fprintf(stdout, "\n\tdV = %.3e (dV0 = %.3e)", dV, dV0);

#if 1	// 부호가 바뀐다.
	if (dV * dV0 < 0) {
		if (dV0 > 0) {
			scale_low = scale;
			cout << ": +SIGN!" << endl;
		} else {
			scale_high = scale;
			cout << ": -SIGN!" << endl;
		}
		continue;
	}
#endif	// 부호가 바뀐다.

	float	dV_percent_error = fabs(fabsf(dV) - fabsf(dV0)) / fabsf(dV0) * 100.f;
//	cout << ": eps = " << dV_percent_error << " (eps0 = " << dV_percent_error0 << ")";
	(void) fprintf(stdout, ": eps = %.3e (eps0 = %.3e)", dV_percent_error, dV_percent_error0);
	if (dV_percent_error < dV_percent_error0) {
		cout << ": DONE" << endl;
#if 1	// 종료
		found = true;
		if (dV_ptr)
			*dV_ptr = dV;
		if (dV_percent_error_ptr)
			*dV_percent_error_ptr = dV_percent_error;
#if 1	// Simulation Cache
		if (show && cached) {
			// 초기위치로 바꾼 후,
			kMesh::D3DXVERTEX*	pV;	// kMesh::__vertexBufferPtr
			__vertexBufferPtr->Lock(0, 0, (void**) &pV, 0);
			for (DWORD prID = 0; prID < __numPointReprs; ++prID) {
				kMesh::D3DXVERTEX&	v = pV[prID];
				v.p = v.p_;
		//		D3DXVECTOR3&	p = v.p;
		//		p = __pointReprPtr[prID].v.position;
			}
			__vertexBufferPtr->Unlock();

			float	dV1;
			if (simulateWholeBody(scale, &dV1) < 0) {
				cerr << "kMesh::simulateWholeBody0(float dV0, ...): simulateWholeBody(): Failure" << endl;
				return -1;
			}
			cout << "dV1 = " << dV1 << endl;
		}
#endif	// Simulation Cache
#endif	// 종료
		break;
	}
	if (fabs(dV - dV_repeat) < FLT_EPSILON) {
		++num_repeat;
	} else {
		dV_repeat = dV;
		num_repeat = 1;
	}
	if (num_repeat > 1) {
//		cout << ": dV_repeat = " << dV_repeat << " (# = " << num_repeat << ")";
		(void) fprintf(stdout, ": dV_repeat = %e (# = %d)", dV_repeat, num_repeat);
	}
	if (
		fabs(scale_high - scale_low) < FLT_EPSILON &&
		num_repeat > 3	// TO DO LIST; magic number?
	) {
		cout << ": STOP" << endl;
#if 1	// 종료
		found = true;
		if (dV_ptr)
			*dV_ptr = dV;
		if (dV_percent_error_ptr)
			*dV_percent_error_ptr = dV_percent_error;
#if 1	// Simulation Cache
		if (show && cached) {
			// 초기위치로 바꾼 후,
			kMesh::D3DXVERTEX*	pV;	// kMesh::__vertexBufferPtr
			__vertexBufferPtr->Lock(0, 0, (void**) &pV, 0);
			for (DWORD prID = 0; prID < __numPointReprs; ++prID) {
				kMesh::D3DXVERTEX&	v = pV[prID];
				v.p = v.p_;
		//		D3DXVECTOR3&	p = v.p;
		//		p = __pointReprPtr[prID].v.position;
			}
			__vertexBufferPtr->Unlock();

			float	dV1;
			if (simulateWholeBody(scale, &dV1) < 0) {
				cerr << "kMesh::simulateWholeBody0(float dV0, ...): simulateWholeBody(): Failure" << endl;
				return -1;
			}
			cout << "dV1 = " << dV1 << endl;
		}
#endif	// Simulation Cache
#endif	// 종료
		break;
	}
	if (fabsf(dV) > fabsf(dV0)) {	// 너무 많이 쪘든가 너무 많이 뺐든가
		if (dV0 > 0) {	// 너무 많이 쪘다
			scale_high = scale;
			cout << ": +less";
		} else {	// 너무 많이 뺐다
			scale_low = scale;
			cout << ": -more";
		}
	} else {	// 너무 적게 쪘든가 너무 적게 뺐든가
		if (dV0 > 0) {	// 너무 적게 뺐다
			scale_low = scale;
			cout << ": +more";
		} else {	// 너무 많이 뺐다
			scale_high = scale;
			cout << ": -less";
		}
	}
	cout << endl;
} while (niter < NITER_MAX);
#undef NITER_MAX
	cout << "total iteration = " << niter << ", found = " << (found ? "True" : "False") << ", scale = " << scale << endl;
	int	rval;
	if (!found) {	// restore kMesh::D3DXVERTEX::p
		rval = -1;
	} else {
		rval = 0;
		if (show && (rval = __whole_body_simulation_finalize()) < 0) {
			cerr << "kMesh::simulateWholeBody0(float dV0, ...): __whole_body_simulation_finalize(): Failure" << endl;
		}
	}
	if (rval == -1 || !show) {
		kMesh::D3DXVERTEX*	pV;	// kMesh::__vertexBufferPtr
		__vertexBufferPtr->Lock(0, 0, (void**) &pV, 0);
		for (DWORD prID = 0; prID < __numPointReprs; ++prID) {
			kMesh::D3DXVERTEX&	v = pV[prID];
			v.p = savedVertexArray.at(prID);
		}
		__vertexBufferPtr->Unlock();
	}
	(void) fprintf(stdout, "...done (%f)\n", __timer.GetElapsedTime());

	return 0;
}
int
kMesh::simulateWholeBody1(
	bool	show,
	float	dV0,	// 현재 부피로부터의 변화량 = vol - vol_now
	float&	scale /* OUT */,
	float*	dV_ptr,	// 실제로 변화한 양
	float	dV_percent_error,
	float*	dV_percent_error_ptr
)
{
	bool	use_p_False = true;	// V(p_)
	float	vol0;	// 원래 부피
	cout << "m__fTotalVolume0 = " << m__fTotalVolume0 / 1000000.f << endl;
	CalculateTotalVolume(use_p_False, &vol0);
	cout << "vol0 = " << vol0 / 1000000.f << endl;
	float	vol_now;	// 현재 부피
	CalculateTotalVolume(!use_p_False, &vol_now);
	cout << "vol_now = " << vol_now / 1000000.f << endl;
	// dV0 = vol - vol_now
	//     = vol - vol0 + (vol0 - vol_now)
	// vol - vol0 = dV0 - (vol0 - vol_now)
	cout << "dV0 = " << dV0 << endl;
	dV0 -= (vol0 - vol_now) / 1000000.f;
	cout << "dV0 = " << dV0 << endl;
	return simulateWholeBody0(show, dV0, scale, dV_ptr, dV_percent_error, dV_percent_error_ptr);
}

int	kMesh::simulateWholeBody2(bool show, int index, float& dV, float dV_percent_error0)
{
	if (index < 0 || index >= (int) __scale_cache_list.size()) {
		cerr << "kMesh::simulateWholeBody2: index = " << index << ": OUT OF RANGE(< " << __scale_cache_list.size() << ")" << endl;
		return -1;
	}

	std::vector<D3DXVECTOR3>	savedVertexArray;
{
	kMesh::D3DXVERTEX*	pV;	// kMesh::__vertexBufferPtr
	__vertexBufferPtr->Lock(0, 0, (void**) &pV, 0);
	for (DWORD prID = 0; prID < __numPointReprs; ++prID) {
		kMesh::D3DXVERTEX&	v = pV[prID];
		savedVertexArray.push_back(v.p);
		v.p = v.p_;	// 원래 위치에서...
	}
	__vertexBufferPtr->Unlock();
}
	SCALE_CACHE&	scale_cache = __scale_cache_list.at(index);
	int	rval;
	if (scale_cache.found) {
		rval = 0;
	} else {
		// not found; simulate
		if (kState::VOLUME_MIN + index * kState::VOLUME_STEP == 0) {	// trivial; no volume change
			scale_cache.found = true;
			scale_cache.scale = 0.f;
			dV = 0;
			rval = 0;
		} else {
			float	dV0 = (float) (kState::VOLUME_MIN + index * kState::VOLUME_STEP);
			rval = simulateWholeBody0(
#if 1	// Cache 에 저장
				true,
#else	// Cache 에 저장/!Cache 에저장
				false,
#endif	// !Cache 에 저장
				dV0,
				scale_cache.scale,
				&dV, dV_percent_error0
			);
			if (rval == 0)
				scale_cache.found = true;
		}
#if 1	// Cache 에 저장
		scale_cache.dV = dV;

		scale_cache.vertexArray.clear();
		kMesh::D3DXVERTEX*	pV;	// kMesh::__vertexBufferPtr
		__vertexBufferPtr->Lock(0, 0, (void**) &pV, 0);
		for (DWORD prID = 0; prID < __numPointReprs; ++prID) {
			kMesh::D3DXVERTEX&	v = pV[prID];
			scale_cache.vertexArray.push_back(v.p);
		}
		__vertexBufferPtr->Unlock();
#endif	// Cache 에 저장
	}
	if (show && rval == 0) {
		if (index == -kState::VOLUME_MIN / kState::VOLUME_STEP)	// 초기 위치
			SetAllMeasureDirty();	// 이미 Vertex 의 위치는 시작 위치(p_)로 설정되었으므로,
	       					// 다시 측정하라는 것으로 충분하다.
		else {
#if 1	// Cache 에 저장
			dV = scale_cache.dV;

			kMesh::D3DXVERTEX*	pV;	// kMesh::__vertexBufferPtr
			__vertexBufferPtr->Lock(0, 0, (void**) &pV, 0);
			for (DWORD prID = 0; prID < __numPointReprs; ++prID) {
				kMesh::D3DXVERTEX&	v = pV[prID];
				v.p = scale_cache.vertexArray.at(prID);
				__pointReprPtr[prID].flag.force = true;	// __whole_body_simulation_finalize 에서 모든 Vertex 를 변화하도록...
			}
			__vertexBufferPtr->Unlock();
			rval = __whole_body_simulation_finalize();
			if (rval == 0)
				SetAllMeasureDirty();	// 다시 측정하도록...
#else	// Cache 에 저장/!Cache 에 저장
			rval = simulateWholeBody(scale_cache.scale, &dV);
#endif	// !Cache 에 저장
		}

		if (__hWnd) {
			cout << "sizeof(WPARAM): " << sizeof(WPARAM) << endl;
			cout << "sizeof(LPARAM): " << sizeof(LPARAM) << endl;
			cout << "PostMessage: " << (
				PostMessage(
					__hWnd,
					WM_USER__WHOLE_BODY_SIMULATION_DONE,
					(WPARAM) index,
					(LPARAM) *((int*) &dV)
				) ? "Ok" : "Oops"
			) << endl;
		}
	} else {
		kMesh::D3DXVERTEX*	pV;	// kMesh::__vertexBufferPtr
		__vertexBufferPtr->Lock(0, 0, (void**) &pV, 0);
		for (DWORD prID = 0; prID < __numPointReprs; ++prID) {
			kMesh::D3DXVERTEX&	v = pV[prID];
			v.p = savedVertexArray.at(prID);
		}
		__vertexBufferPtr->Unlock();
	}
	return rval;
}
#endif	// 체형변화 시뮬레이션

#if 1	// 모델스냅샷
int	kMesh::snapshot_save(int index, bool overwrite)
{
	for (int i = __snapshots.size(); i < 1 + index; ++i)
		__snapshots.push_back(SNAPSHOT());

	SNAPSHOT&	snapshot = __snapshots.at(index);
	if (snapshot.saved && !overwrite) {
		cerr << "kMesh::snapshot_save: index = " << index << ": Data exist: Set overwrite to true" << endl;
		return -1;
	}

	snapshot.data.clear();
	kMesh::D3DXVERTEX*	pV;
	__vertexBufferPtr->Lock(0, 0, (void**) &pV, 0);
	for (DWORD prID = 0; prID < __numPointReprs; ++prID) {
		kMesh::D3DXVERTEX&	v = pV[prID];
		snapshot.data.push_back(v.p);
	}
	__vertexBufferPtr->Unlock();
	snapshot.saved = true;

	return 0;
}

int	kMesh::snapshot_load(int index)
{
	if (index < 0 || index >= (int) __snapshots.size()) {
		cerr << "kMesh::snapshot_load: index = " << index << ": OUT OF RANGE(" << __snapshots.size() << ")" << endl;
		return -1;
	}

	SNAPSHOT&	snapshot = __snapshots.at(index);
	if (!snapshot.saved) {
		cerr << "kMesh::snapshot_load: index = " << index << ": No data" << endl;
		return -1;
	}

	if (snapshot.data.size() != __numPointReprs) {
		cerr << "kMesh::snapshot_load: index = " << index << ": Data inconsistent" << endl;
		return -1;
	}

	kMesh::D3DXVERTEX*	pV;
	__vertexBufferPtr->Lock(0, 0, (void**) &pV, 0);
	for (DWORD prID = 0; prID < __numPointReprs; ++prID) {
		kMesh::D3DXVERTEX&	v = pV[prID];
		v.p = snapshot.data.at(prID);
		__pointReprPtr[prID].flag.force = true;	// __whole_body_simulation_finalize 에서 모든 Vertex 를 변화하도록...
	}
	__vertexBufferPtr->Unlock();
	int	rval = __whole_body_simulation_finalize();
	if (rval == 0)
		SetAllMeasureDirty();	// 다시 측정하도록...
	return rval;
}
#endif	// 모델스냅샷

#if 1	// 마지막 주문; 6. 3D 단면
int	kMesh::Set3DCrossSection(bool status)
{
	if (status) {	// On
		if (getValue(TYPE__MODEL, MODEL__OBJECT, &__lastObjectType) < 0)
			return -1;
		if (getValue(TYPE__MODEL, MODEL__SHADING, &__lastShadingType) < 0)
			return -1;
		// 
		SHADING_TYPE	shadingType = SHADING_TEXTURE;
		if (setValue(TYPE__MODEL, MODEL__SHADING, &shadingType) < 0)
			return -1;
		measureDone = false;
	} else {	// Off
		if (setValue(TYPE__MODEL, MODEL__OBJECT, &__lastObjectType) < 0)
			return -1;
		if (setValue(TYPE__MODEL, MODEL__SHADING, &__lastShadingType) < 0)
			return -1;
	}

	return 0;
}
#endif	// 마지막 주문; 6. 3D 단면
