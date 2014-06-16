#pragma once

// warning C4238: nonstandard extension used : class rvalue used as lvalue
//	D3DXVec3Length(&D3DXVECTOR(P2 - P1));
#pragma warning(disable: 4238)
// warning C4127: conditional expression is constant
//	do {
//		......
//	} while (0);
#pragma warning(disable: 4127)
// warning C4995: 'gets': name was marked as #pragma deprecated
#pragma warning(disable: 4995)
// warning C4201: nonstandard extension used : nameless struct/union
#pragma warning(disable: 4201)
#include <d3dx9math.h>
#if 1
#include <vector>
#include <set>
#else
#include <assert.h>
#endif
#if 1	// 모델 선택
////////////////////////////////////////////////////////////////////////
// 1. 읽어들일 모델을 선택한다.
#define STANDARD_MODEL_NONE		0	// 제일 간단한 모델; 아래 STANDARD_MODEL 가 정의되지 않았을 때 기본값
// 다음 두 경우는 .OBJ 파일을 사용한다;
// 이 경우 File Cache 를 이용할 수 있다.
//	1) 먼저 표준모델을 사용하여(MODEL_TYPE = STANDARD_MODEL_TEST)
//	__WRITE_FILES__ 를 정의해서 필요한 파일들을 만든 후,
//	2) __READ_FILES__ 을 정의하여 모델을 읽어들일 때 1)에서 만든 파일을 사용한다.
//	이 경우 표준모델 또는 측정모델 어느 모델을 읽어들여도 상관없다.
#define DAZ_STUDIO_STANDARD_MODEL	1	// DAZ Studio 모델
#define ETRI_STANDARD_MODEL		2	// 에트리 제공 모델
#define ETRI_STANDARD_MODEL2		3	// 에트리 제공 모델(여)
#define ETRI_STANDARD_MODEL3		4	// 에트리 제공 모델(남)
#define ETRI_STANDARD_MODEL4		5	// 에트리 제공 모델(남; 서이사)
#define ETRI_STANDARD_MODEL6		6	// 에트리 제공 모델-121109(남: michael)
#define ETRI_STANDARD_MODEL7		7	// 에트리 제공 모델-121109(여: victoria)

// 2009년 12월 21일 현재 3, 6, 7만 사용
#define STANDARD_MODEL	ETRI_STANDARD_MODEL6//ETRI_STANDARD_MODEL3//ETRI_STANDARD_MODEL2//ETRI_STANDARD_MODEL//DAZ_STUDIO_STANDARD_MODEL

// 2. 어떤 모델을 사용할 지를 결정한다.
#if defined(STANDARD_MODEL)// && (STANDARD_MODEL == DAZ_STUDIO_STANDARD_MODEL || STANDARD_MODEL == ETRI_STANDARD_MODEL)
#define STANDARD_MODEL_TEST	0		// 표준 모델 테스트; 아래 MODEL_TYPE 이 정의되지 않았을 때 기본값
#define REAL_MODEL		1		// 측정(실제) 모델
#define BOTH_MODEL		2		// 모델 비교; 표준체형으로의 성형 시뮬레이션 목적

#define MODEL_TYPE	REAL_MODEL//STANDARD_MODEL_TEST//BOTH_MODEL
#endif
////////////////////////////////////////////////////////////////////////
#endif
#if 1	// File Cache; .OBJ 파일을 사용할 때에만 해당된다.
////////////////////////////////////////////////////////////////////////
/*
 * kMeshLoader::Create 에서
 *	__Indices(__Indices__), __Vertices(__Vertices__), __Attributes(__Attributes__), 텍스쳐(__Textures__),
 *	__VertexBufferIndexByF(__VertexBufferIndexByF__)
 * kMesh::setAll 에서
 *	__adjacencyPtr(__adjacencyPtr__), __vertexID2pointReprIDMap(__vertexID2pointReprIDMap__),
 *	__pointReprPtr(__pointReprPtr__), __edgePtr(__edgePtr__), __facePtr(__facePtr__)
 * 을 괄호 안의 해당 파일에 쓴다.
 * 측정 모델의 경우, .OBJ 로부터 v, vt 정보를 읽고 __VertexBufferIndexByF__ 에서 읽은
 * __VertexBufferIndexByF 로부터 __Vertices 를 설정한다;
 * 처음부터 읽을 때와 거의 차이가 없다;
 * __AddVertex 를 호출해서 __Indices 를 만드는 데는 거의 시간이 안 걸린다.
 */

#if defined(STANDARD_MODEL)// && (STANDARD_MODEL == DAZ_STUDIO_STANDARD_MODEL || STANDARD_MODEL == ETRI_STANDARD_MODEL)
// 파일로부터 인덱스 정보를 읽어서 사용할 때는 CPointRepr, CEdge 가 다른 구조를 가진다;
// CPointRepr::vVertexID 에 있는 설명을 참고한다.
#ifdef __USE_A_OFFSET__	// __a_..., __a_..._a_offset
#undef __USE_A_OFFSET__	// __READ_FILES__ 일 때만 정의된다.
#endif
// 동시에 둘 이상의 표준모델을 읽어들일 수 있게 한다.
#define MULTI_STANDARD
////////////////////////////////////////////////////////////////
// __READ_FILES__ 또는 __WRITE_FILES__ 가 정의된다면 하나만 정의되어야 한다.
////////////////////////////////////////////////////////////////
#ifndef __KEKO__
#define __READ_FILES__
#endif	// !__KEKO__
////////////////////////////////////////////////////////////////
//#define __WRITE_FILES__	// 표준모델을 사용할 때만 정의되어야 한다.
#ifdef __WRITE_FILES__
#ifdef MULTI_STANDARD
#define WRITE_APPEND
#define STANDARD_ID	7	// WRITE_APPEND 가 정의되지 않으면 0 그렇지 않으면 0보다 커야 한다;
				// 0부터 순서대로 만든다:
				//	WRITE_APPEND	STANDARD_ID
				//	X		0
				//	O		1
				//	O		1
				//	......
				//
				// 2009년 9월 10일 현재 두 개의 표준모델이 있다.
				//	0: DAZ_STUDIO_STANDARD_MODEL
				//	1: ETRI_STANDARD_MODEL
				//	2: logo
				//	3: 목,손,발 제거한 에트리 (표준)모델 (남)
				//	4: 목,손,발 제거한 에트리 (표준)모델 (여)
				//	5: 목,손,발 제거한 에트리 (표준)모델 (남; 서이사)
				//	6: 목,손,발 제거한 에트리 (표준)모델-091211 (남; Michael)
				//	7: 목,손,발 제거한 에트리 (표준)모델-091211 (여; Victoria)
#endif	// MULTI_STANDARD

// 파일이름
#ifdef MULTI_STANDARD
#define FILENAME_SURFIX		"_M"
#else	// MULTI_STANDARD/!MULTI_STANDARD
#define FILENAME_SURFIX		""
#endif	// !MULTI_STANDARD
// kMeshLoader
#define INDICES__FILENAME		"__Indices" ## FILENAME_SURFIX ## "__"
#define VERTICES__FILENAME		"__Vertices" ## FILENAME_SURFIX ## "__"
#define VERTEXBUFFERINDEXBYF__FILENAME	"__VertexBufferIndexByF" ## FILENAME_SURFIX ## "__"
#define ATTRIBUTES__FILENAME		"__Attributes" ## FILENAME_SURFIX ## "__"
#define TEXTURES__FILENAME		"__Textures" ## FILENAME_SURFIX ## "__"
// kMesh
#define ADJACENCY_PTR__FILENAME			"__adjacencyPtr" ## FILENAME_SURFIX ## "__"
#define VERTEXID2POINTREPRIDMAP__FILENAME	"__vertexID2pointReprIDMap" ## FILENAME_SURFIX ## "__"
#define POINTREPR_PTR__FILENAME			"__pointReprPtr" ## FILENAME_SURFIX ## "__"
#define EDGE_PTR__FILENAME			"__edgePtr" ## FILENAME_SURFIX ## "__"
#define FACE_PTR__FILENAME			"__facePtr" ## FILENAME_SURFIX ## "__"

// 쓰기 모드
#define WRITE_MODE	"w"
#if defined(MULTI_STANDARD) && defined(WRITE_APPEND)
#ifdef WRITE_MODE
#undef WRITE_MODE
#endif	// WRITE_MODE
#define WRITE_MODE	"a"
#endif	// MULTI_STANDARD && WRITE_APPEND
#endif	// __WRITE_FILES__
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
#endif

// 다시 한 번 확인한다.
#if 1	// MACRO 확인
////////////////////////////////////////////////////////////////
#if defined(STANDARD_MODEL)// && (STANDARD_MODEL == DAZ_STUDIO_STANDARD_MODEL || STANDARD_MODEL == ETRI_STANDARD_MODEL)
#else
#if defined(__READ_FILES__) || defined(__WRITE_FILES__)
#error "__READ_FILES__, __WRITE_FILES__ only for .OBJ file"
#endif
#endif
#ifdef __READ_FILES__
#ifdef __WRITE_FILES__
#error "Only One: __READ_FILES__ or __WRITE_FILES__"
#endif
#ifndef __USE_A_OFFSET__
#define __USE_A_OFFSET__
#endif
#endif
#ifdef __WRITE_FILES__
#ifdef __READ_FILES__
#error "Only One: __READ_FILES__ or __WRITE_FILES__"
#endif
#if MODEL_TYPE != STANDARD_MODEL_TEST
#error "__WRITE_FILES__ only for STANDARD_MODEL_TEST"
#endif
#ifdef __USE_A_OFFSET__
#undef __USE_A_OFFSET__
#endif
#endif
////////////////////////////////////////////////////////////////
#endif	// MACRO 확인
////////////////////////////////////////////////////////////////////////
#endif	// File Cache
/*
MFnMesh:: getPoints ( MFloatPointArray & vertexArray, MSpace::Space space ) const 
	This method copies the vertex list for this mesh into the given point array.
MFnMesh:: getPolygonVertices ( int polygonId, MIntArray & vertexList ) const 
	This method retrieves the object-relative (mesh-relative/global) vertex indices for the specified polygon. The indices refer to the elements in the array returned by the 'getPoints' method.
MFnMesh:: getPoint ( int vertexId, MPoint & pos, MSpace::Space space ) const 
	Get the position of the specified vertex in this mesh's vertex list.
MFnMesh:: getFaceVertexNormal ( int faceIndex, int vertexIndex, MVector & normal, MSpace::Space space ) const 
	Return a per-vertex-per-face normal for a given face (polygon) and given vertex.
MFnMesh:: getEdgeVertices ( int edgeId, int2 & vertexList ) const 
This method retrieves the object-relative (mesh-relative/global) vertex indices corresponding to the specified edge. The indices can be used to refer to the elements in the array returned by the 'getPoints' method.

MItMeshEdge:: getConnectedFaces ( MIntArray & faces, MStatus * ReturnStatus) const 
	This method returns a list of faces (with 1 or 2 faces) connected to teh current edge.
int MItMeshEdge:: getConnectedEdges ( MIntArray & edges, MStatus * ReturnStatus) const 
	This method returns a list of edges connected to the current edge.

MItMeshPolygon:: getEdges ( MIntArray & edges ) 
	This method gets the indices of the edges contained in the current face.
MItMeshPolygon:: getConnectedFaces ( MIntArray & faces )
	This method gets the indices of the faces connected to the current face.
MItMeshPolygon:: getConnectedEdges ( MIntArray & edges ) 
	This method gets the indices of the edges connected to the vertices of the current face, but DOES not include the edges contained in the current face
MItMeshPolygon:: getConnectedVertices ( MIntArray & vertices) 
	This method gets the object-relative indices of the vertices surrounding the vertices of the current face, but does not include the vertices of the current face

MItMeshVertex:: getConnectedFaces ( MIntArray & faces ) 
	This method gets the indices of the faces connected to the current vertex.
MItMeshVertex:: getConnectedEdges ( MIntArray & edges )
	This method gets the indices of the edges connected to the current edge.
MItMeshVertex:: getConnectedVertices ( MIntArray & vertices)
	This method gets the indices of the vertices surrounding the current vertex
MItMeshVertex:: getOppositeVertex ( int &vertexId, int edgeId )
	This gets the other vertex of the given edge

 */

struct EDGE {
	DWORD	id;
	bool	bDir;

#if 1
	void	Set(DWORD id_ = (DWORD) -1, bool bDir_ = true)
	{
		id = id_;
		bDir = bDir_;
	}
#else
	EDGE(DWORD id_ = (DWORD) -1, bool bDir_ = true) : id(id_), bDir(bDir_) {}
#endif
};


// (Point Repr. ID 에 해당하는 점이 어떤(id) Face 의 몇 번째 Vertex 인지를 알 때 사용된다;
// 여기서 몇 번째는 Face-relative 인덱스로 0, 1, 2 값을 가진다.
//
//	서로 다른 .OBJ 를 사용하면 꼭지점의 Point Repr. ID 는 다를 수 있어도,
//	Face 의 순서가 같다면, FACE_ID_LOCAL_INDEX 는 같다.
//
struct FACE_ID_LOCAL_INDEX
{
	DWORD	ID;
	int	local_index;	// 0, 1, 2

#if 1
	void	Set(DWORD ID_ = (DWORD) -1, int local_index_ = -1)
	{
		ID = ID_;
		local_index = local_index_;
	}
#else
	FACE_ID_LOCAL_INDEX() : ID((DWORD) -1), local_index(-1) {}
	FACE_ID_LOCAL_INDEX(DWORD id, int index) : ID(id), local_index(index) {}
#endif
};
struct CPointRepr {
	// N 개의 Vertex 를 가졌다고해서 N 개의 Point Representative 가 있다는 말은 아니다.
	// 최대 N 개의 Point Representative 를 가질 수 있다.
	// 처음 시작할 때, struct CPointRepr 배열의 크기를 N 으로 잡을 것이다.
	// 그러므로 없는 경우에는 id 값을 (DWORD) -1 로 설정한다.
	DWORD	id;
	CPointRepr() : id((DWORD) -1) {}

#if 1	// 체형변화 시뮬레이션
	struct Area2Flag {
		bool	subset:1;	// 이 Vertex 가 속한 Face 가 움직이는 subset 에 표함되어 있는가?
		bool	move:1;		// 이 Vertex 가 실제로 움직였는가?
		bool	inc:1;		// 면적이 증가한 삼각형에 속한 Vertex 인가?
		bool	all:1;		// 이 Vertex 주위의 모든 삼각형의 면적이 증가했는가? (all or nothing?)

		bool	repos:1;	// 위치를 재설정할 때 이 Vertex 가 사용되는가?
					//	1) inc 만 설정되었더라도 (주위의 삼각형 중에서 어느 하나라도 면적이 늘어났을 때)
					//	2) 또는 다른 조건을 통해... (TO DO LIST)
		bool	done:1;		// 이 Vertex 의 새로운 위치가 결정되었는가?
		bool	smooth:1;	// Smoothing 처리할 때 이 Vertex 가 사용되는가?
					//	1) inc 가 설정된 Vertex 주위로 n(>= 0) 개의 Vertex 까지
					//	TO DO LIST: n > 0 인 경우에...
					//	2) move 가 설정된 Vertex
					//	3) 또는 다른 조건을 통해... (TO DO LIST)
					//
					//
					// 아래의 Area2Control::Smooth 가 1 이고,
					// 아래의 Area2Control::SmoothIncLevel 이 0이 아닌 경우에...
					//
		bool	boundary0:1;	// Area2Flag::smooth 를 결정할 때 사용되는 플래그
		bool	boundary:1;	// 실제로 경계가 되는 Vertex 인가? (임시 플래그)
					//
					// boundary0 이 설정된 Vertex 주변의 Vertex 중에서,
					// smooth 가 설정되지 않은 Vertex 가 boundary 가 된다;
					// boundary 가 설정된 Vertex 가 smooth 될 Vertex 에 추가되고,
					// boundary 가 boundary0 으로 돼 다음 smooth 결정에 사용된다.
					//
					//	1) smooth 가 설정된 Vertex 에 boundary0 설정을 한다;
					//	Area2Control::SmoothIncLevel > 0 경우에 한해
					//
					//	2) boundary0 가 설정된 Vertex 에 대해,
					//	이전 단계에서 smooth 가 설정된 Vertex 를 참고하여,
					//	boundary 가 되는 Vertex 를 결정한다.
					//	3) boundary 가 설정된 Vertex 를,
					//	smooth 가 설정되게 하는 Vertex 에 추가한다.
					//	다음 번을 위해 boundary0 를 갱신한다.
					//
					//	4) Area2Control::SmoothIncLevel 만큼 2)~4)를 반복한다.

		Area2Flag() {reset();}
		void	reset()
		{
			subset = false;
			move = false;
			inc = false;
			all = false;
			repos = false;
			done = false;
			smooth = false;
			boundary0 = false;
			boundary = false;
		}
	};
#endif	// 체형변화 시뮬레이션
#if 1	// Vertex 마다 임시 값을 저장할 장소
	union UVAL {
		int	ival;
		float	fval;
	};
	UVAL	val;
#endif	// Vertex 마다 임시 값을 저장할 장소
#if 1	// Voxel
	struct Flag : Area2Flag
	{
		bool	force:1;	// 이 Vertex 가 무조건 움직이는가?
		bool	fixed:1;	// (목, 손, 발)경계선에 있는 Vertex 인가?
	};
	Flag	flag;
#endif	// Voxel
#if 1
	struct VERTEX {
#if 1
		D3DXVECTOR3	position;
		D3DXVECTOR3	normal;
		D3DXVECTOR2	texcoord;
#else
		D3DXVECTOR3	p;
		D3DXVECTOR3	n;
		FLOAT		u, v;
#endif
	};
#if 1	// 뼈
	struct VERTEX2 : public VERTEX
	{
		D3DXVECTOR3	position0;	// 뼈의 길이를 늘이기 전의 원래 위치;
						// VERTEX::position 은 뼈의 길이변화에 따라 바뀐다.
	};
	VERTEX2	v;
#else	// 뼈/!뼈
	VERTEX	v;
#endif	// !뼈
	// SetPolygon 을 호출한 후에 CPointRepr::vVertexID 와
	// (__fScale, __meshSourcePtr 의 Vertex Buffer) 를 이용하여 설정할 수 있으므로 필요하지 않다;
	// kMesh::__meshPtr 을 만든 후에서 kMesh::__vertexBufferPtr 를 이용한다.
	// 하지만 매번 Vertex Buffer 를 Lock/Unlock 해야하므로 따로 보관한다. (TO DO LIST)
#endif

	// 이 Point Representative 로 대표되는 Vertex ID 배열에 대한 포인터;
	// ID3DXBaseMesh::GenerateAdjacency 와
	// ID3DXBaseMesh::ConvertAdjacencyToPointReps 그리고
	// Index Buffer 를 사용해 얻어진다.
#ifdef __USE_A_OFFSET__
	// CPointRepr::id 에 따라 순차적으로 vVertexID 를 담은 pointer_to__a_vVertexID 배열(A)과,
	// 주어진 CPointRepr::id 에 대한 목록이 pointer_to__a_vVertexID 배열의 몇 번째에서부터 시작하는 지를 말해주는
	// pointer_to__a_vVertexID_a_offset 배열(OFFSET)을 사용하여 같은 결과를 얻을 수 있다;
	// CPointRepr::id + 1 에 대한 목록의 시작 전까지 얻으면 된다.
	//	[A[OFFSET[CPointRepr::id]], A[OFFSET[CPointRepr::id + 1]])
#else
#if 1	// 삼각형을 돌아가면서 이 값을 설정하는 데,
	// 그러다 보니 같은 Vertex 가 여러 번 들어갈 수 있다.
	// 그래서 vector 대신에 set 컨테이너를 사용한다.
	std::set<DWORD>		vVertexID;
#else
	std::vector<DWORD>	vVertexID;
#endif
#endif
#ifdef __USE_A_OFFSET__
	// A:= pointer_to__a_vEdgeID
	// OFFSET:= pointer_to__a_vEdgeID_a_offset
#else
	// 이 Point Representative 를 한 끝으로하는 Edge 배열에 대한 포인터
	std::vector<EDGE>	vEdgeID;
#endif
#ifdef __USE_A_OFFSET__
	// A:= pointer_to__a_vFaceIDLocalIndex
	// OFFSET:= pointer_to__a_vFaceIDLocalIndex_a_offset
#else
#if 1	// FACE_ID_LOCAL_INDEX
	std::vector<FACE_ID_LOCAL_INDEX>	vFaceIdLocalIndex;
#else
	// 이 Point Representative 를 한 점으로하는 Face 배열에 대한 포인터
	std::vector<DWORD>	vFaceID;
#endif
#endif
};

struct CEdge {
	// N 개의 Face 가 있을 때, 최대 3 * N 개의 Edge 가 있을 수 있다.
	// 없는 경우에는 id 값은 (DWORD) -1 로 설정한다.
	DWORD	id;

	// 이 Edge 를 구성하는 한 끝에 해당하는 Point Representative
	DWORD	prStartID;
	// 나머지 한 끝에 해당하는 Point Representative
	DWORD	prEndID;
	// 1. prStartID 와 prEndID 가 뒤바뀌더라도 같은 Edge 임에는 틀림없다.
	// 하지만 경우에 따라 이 순서가 중요할 수 있으므로,
	// CPointRepr 나 CFace 에서 Edge 를 언급할 때,
	// struct EDGE 를 통해, 이 방향에 대한 정보가 함께 담는다.
	// 2. 주어진 prID 가 어떤 Edge 에 속하는 지를 알아보기위해
	// prStartID 와 prEndID 를 (DWORD) -1 로 설정한다.

	// 이 Edge 를 공유하는 두 Face;
	// 하나만 있을 수 있다.
	// 초기값을 (DWORD) -1 로 설정한다.
#ifdef __USE_A_OFFSET__
	// A:= pointer_to__a_faceID
	// OFFSET:= pointer_to__a_faceID_a_offset
#else
#if 1	// FACE_ID_ARRAY
	std::vector<DWORD>	faceID;
#else
	DWORD	faceID[2];
#endif
#endif

	CEdge(void)
	{
		id = (DWORD) -1;
		prStartID = (DWORD) -1;
		prEndID = (DWORD) -1;
#if 1	// FACE_ID_ARRAY
#else
		faceID[0] = (DWORD) -1;
		faceID[1] = (DWORD) -1;
#endif
	}

	struct LineIndex {
		DWORD	iStart;
		DWORD	iEnd;

		LineIndex(DWORD _0, DWORD _1) : iStart(_0), iEnd(_1) {}
	};
};
#if 0
int	FindEdge(DWORD prStartID, DWORD prEndID, CEdge* pEdge, int NEdge);
#endif

class kID
{
private:
	DWORD	__id;
public:
	DWORD	id() const		{return __id;}

public:
	kID()				{setNull();}
	kID(DWORD id) : __id(id)	{};

public:
	void	setNull()	{__id = (DWORD) -1;}
	bool	isNull() const	{return __id == (DWORD) -1;}

	kID&	operator=(const kID& other)
	{
		if (this != &other) {
			this->__id = other.id();
		}

		return *this;
	}
	kID&	operator=(int id)
	{
		this->__id = (DWORD) id;

		return *this;
	}
#if 0
	kID&	operator++()
	{
		++__id;
		return *this;
	}
#endif
	void	operator++()	{++__id;}
	void	operator--()	{if ((int) __id >= 0) {--__id;}}
};
struct CFace {
	DWORD	id;

#if 1	// 체형변화 시뮬레이션
	struct Flag
	{
		bool	active;
	};
	Flag	flag;
#endif	// 체형변화 시뮬레이션
#if 0
	// Index Buffer 로부터 얻는 Vertex ID
	union {
		struct {
			DWORD	vertexID0;
			DWORD	vertexID1;
			DWORD	vertexID2;
		};
		DWORD	vertexID[3];
	};
#endif

	// Point Representative 로부터 얻는 ID
	union {
		struct {
			DWORD	prID0;
			DWORD	prID1;
			DWORD	prID2;
		};
		DWORD	prID[3];
	};

	// 이 Face 를 구성하는 Edge 들에 대한 ID
	// 주어진 Face 의 Edge 에 대한 정의와 방향이 반대면
	// bDir = false 로 설정한다;
	// Triangle 을 구성하는 Vertex 들에서
	// 0번째 Vertex 에서 1번째 Vertex 로 가는 Edge 가
	// 기존의 정의된 Edge 와 시작과 끝이 값다면 bDir = true 이고,
	// 그렇지 않다면 bDir = false 이다.
	union {
		struct {
			EDGE	edgeID01;
			EDGE	edgeID12;
			EDGE	edgeID20;

//			E1() : edgeID01((DWORD) -1, true), edgeID12((DWORD) -1, true), edgeID20((DWORD) -1, true) {}
		};
		EDGE	edgeID[3];
	};
#if 0
	// Tessellated 되면 기존의 Edge 는 두 개로 나뉜다:
	//	(prStartID, prEndID) => (prStartID, prNewID), (prNewID, prEndID)
	// 단, 방향은 그대로 유지된다.
	union {
		struct {
			EDGE	__edgeID01;
			EDGE	__edgeID12;
			EDGE	__edgeID20;

//			E2() : __edgeID01((DWORD) -1, true), __edgeID12((DWORD) -1, true), __edgeID20((DWORD) -1, true) {}
		};
		EDGE	__edgeID[3];
	};
#endif

	// 인접한 Face 에 대한 ID;
	// 모두 있는 것은 아니다.
	// ID3DXBaseMesh::GenerateAdjacency 를 통해 얻는다.
	// 초기값을 (DWORD) -1 로 설정한다.
	union {
		struct {
			DWORD	ajID01;
			DWORD	ajID12;
			DWORD	ajID20;
		};
		DWORD	ajID[3];
	};
#if 0
	// Tessellated 되면 각 변은 두 부분으로 나뉜다:
	//	ajID01: 첫 번째 부분
	//	__ajID01: 두 번째 부분
	union {
		struct {
			DWORD	__ajID01;
			DWORD	__ajID12;
			DWORD	__ajID20;
		};
		DWORD	__ajID[3];
	};
#endif

	CFace(void)
	{
		prID0 = (DWORD) -1;
		prID1 = (DWORD) -1;
		prID2 = (DWORD) -1;
		edgeID[0].Set();
		edgeID[1].Set();
		edgeID[2].Set();
		ajID01 = (DWORD) -1;
		ajID12 = (DWORD) -1;
		ajID20 = (DWORD) -1;
#if 0
		__ajID01 = (DWORD) -1;
		__ajID12 = (DWORD) -1;
		__ajID20 = (DWORD) -1;
#endif
	}

	struct FaceIndex {
		DWORD	i0;
		DWORD	i1;
		DWORD	i2;

		FaceIndex(DWORD _0, DWORD _1, DWORD _2) : i0(_0), i1(_1), i2(_2) {}
	};

	D3DXVECTOR3	normal;
};


#if 0
// 배열의 연속적인 값을 참조할 때 사용;
//	index 번째 성분에서부터 n 개의 연속적인 값을 참조
struct INDEX_REF
{
	int	index;
	int	n;
};
struct POINT_REPR2
{
	INDEX_REF	VertexRef;		// VertexID 배열에 대한 참조
	INDEX_REF	Edge2Ref;		// EDGE2 배열에 대한 참조
	INDEX_REF	FaceIDLocalIndexRef;	// FACE_ID_LOCAL_INDEX 배열에 대한 참조
};
// POINT_REPR2 배열

struct EDGE2
{
	int	prStartID;
	int	prEndID;
	bool	bDir;

	INDEX_REF	FaceRef;	// FACE2 배열에 대한 참조
};
// EDGE2 배열

struct FACE2
{
	// Face 를 구성하는 세 꼭지점의 ID;
	//	POINT_REPR2 배열에 대한 인덱스
	int	prID0;
	int	prID1;
	int	prID2;

	// Face 를 구성하는 세 변의 ID;
	//	EDGE2 배열에 대한 인덱스
	int	edgeID0;
	int	edgeID1;
	int	edgeID2;

	// Face 와 인접하는 Face 의 ID; 없다면 -1
	//	FACE2 배열에 대한 인덱스
	int	ajID0;
	int	ajID1;
	int	ajID2;
};
// FACE2 배열
#endif

#ifdef __USE_A_OFFSET__
#ifdef MULTI_STANDARD
struct INT_ARRAY {
	const int*	ptr;
	int	n;
};
struct DWORD_ARRAY {
	const DWORD*	ptr;
	int	n;
};
struct EDGE_ARRAY {
	const EDGE*	ptr;
	int	n;
};
struct FACE_ID_LOCAL_INDEX_ARRAY {
	const FACE_ID_LOCAL_INDEX*	ptr;
	int	n;
};
struct LPCWSTR_ARRAY {
	LPCWSTR*	ptr;
	int	n;
};
struct FLOAT_ARRAY {
	const float*	ptr;
	int	n;
};
struct MESH_LOADER_READ_FILES {
	DWORD_ARRAY	__Indices__;
	FLOAT_ARRAY	__Vertices__;
	INT_ARRAY	__VertexBufferIndexByF__;
	INT_ARRAY	__Attributes__;
	LPCWSTR_ARRAY	__Textures__;
};
struct MESH_READ_FILES
{
	DWORD_ARRAY	__adjacencyPtr__;
	INT_ARRAY	__vertexID2pointReprIDMap__;

	int		__numPointReprs__;
	DWORD_ARRAY	__a_vVertexID;
	INT_ARRAY	__a_vVertexID_a_offset;
	EDGE_ARRAY	__a_vEdgeID;
	INT_ARRAY	__a_vEdgeID_a_offset;
	FACE_ID_LOCAL_INDEX_ARRAY	__a_vFaceIDLocalIndex;
	INT_ARRAY			__a_vFaceIDLocalIndex_a_offset;

	int	__numEdges__;
	INT_ARRAY	__edgePtr__;
	DWORD_ARRAY	__a_faceID;
	INT_ARRAY	__a_faceID_a_offset;

	int		__numFaces__;
	INT_ARRAY	__facePtr__;
};
#endif	// MULTI_STANDARD
#endif	// __USE_A_OFFSET__
