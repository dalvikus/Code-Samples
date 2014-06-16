#include <mesh/kMeshLoader.h>
#if defined(__READ_FILES__)
#if 1	// kFileCache
#else	// kFileCache/!kFileCache
#ifdef MULTI_STANDARD
#include <mesh/__Indices_M__>			// INDICES__FILENAME
#include <mesh/__VertexBufferIndexByF_M__>	// VERTEXBUFFERINDEXBYF__FILENAME
#include <mesh/__Vertices_M__>			// VERTICES__FILENAME
#include <mesh/__Attributes_M__>		// ATTRIBUTES__FILENAME
#include <mesh/__Textures_M__>			// TEXTURES__FILENAME

#include <mesh/__meshLoader_READ_FILES_M__.h>
#else	// MULTI_STANDARD/!MULTI_STANDARD
#include <mesh/__Indices__>			// INDICES__FILENAME
#include <mesh/__VertexBufferIndexByF__>	// VERTEXBUFFERINDEXBYF__FILENAME
#include <mesh/__Vertices__>			// VERTICES__FILENAME
#include <mesh/__Attributes__>			// ATTRIBUTES__FILENAME
#include <mesh/__Textures__>			// TEXTURES__FILENAME
#endif	// !MULTI_STANDARD
#endif	// !kFileCache
#endif	// __READ_FILES__

D3DVERTEXELEMENT9	kMeshLoader::VERTEX_DECL[3 + 1] = {	// POSITION, NORMAL, TEXCOORD
	{
		0,
		0 * sizeof(D3DXVECTOR3),	// 0
		D3DDECLTYPE_FLOAT3,
		D3DDECLMETHOD_DEFAULT,
		D3DDECLUSAGE_POSITION,
		0
	},
	{
		0,
		1 * sizeof(D3DXVECTOR3),	// 12
		D3DDECLTYPE_FLOAT3,
		D3DDECLMETHOD_DEFAULT,
		D3DDECLUSAGE_NORMAL,
		0
	},
	{
		0,
		2 * sizeof(D3DXVECTOR3),	// 24
		D3DDECLTYPE_FLOAT2,
		D3DDECLMETHOD_DEFAULT,
		D3DDECLUSAGE_TEXCOORD,
		0
	},

	D3DDECL_END()
};


kMeshLoader::kMeshLoader()
{
#if 1	// Voxel
	__voxelVertexDeclarationPtr = 0;
	__voxelVertexBufferPtr = 0;
	__voxelIndexBufferPtr = 0;
#endif	// Voxel
	__meshPtr = 0;
#if 1	// DrawSubset
	__meshSubsetPtr = 0;
#endif
}
kMeshLoader::~kMeshLoader()
{
	Destroy();
}


void	kMeshLoader::__InitMaterial(Material* pMaterial)
{
	ZeroMemory(pMaterial, sizeof(Material));

	pMaterial->vAmbient = D3DXVECTOR3(0.2f, 0.2f, 0.2f);
	pMaterial->vDiffuse = D3DXVECTOR3(0.8f, 0.8f, 0.8f);
	pMaterial->vSpecular = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	pMaterial->nShininess = 0;
	pMaterial->fAlpha = 1.0f;
	pMaterial->bSpecular = false;
	pMaterial->strTexture[0] = L'0';
	pMaterial->texturePtr = NULL;
}

// MeshFromOBJ::CMeshLoader::__AddVertex 로부터...
//
// __VertexCache 배열에서 주어진 hashIndex 번째에 해당하는 값(포인터)은
// hashIndex 에 해당하는 값(VERTEX)들에 대한 Linked List 의 첫번째를 가리킨다.
// MeshFromOBJ 에서 사용하는 예를 보면 hashIndex 는 1부터 시작한다;
// 결국 __VertexCache 의 첫번째는 항상 NULL 이다.
// 아래의 첫번째 if 문에서 자세히 나와있듯이 정확히 하려면 0부터 시작해야 한다.
//
//	주어진 Vertex 정보 VERTEX 가 __Vertices 에 있는 어떤 것들과도 중복되지 않는 지 검사한 후,
//	새로운 VERTEX 라면 __Vertices 에 추가한다;
//	VERTEX 가 들어있는 __Vertices 에 대한 인덱스를 리턴한다.
//
//	중복된 지를 검사할 때 (hashIndex, __VertexCache) 가 사용된다; 일종의 Linked List
//
DWORD	kMeshLoader::__AddVertex(
	const CPointRepr::VERTEX&	vertex,

	UINT	hashIndex,
	CGrowableArray<CacheEntry*>&	__VertexCache,	// [OUT]

	CGrowableArray<CPointRepr::VERTEX>&		__Vertices	// [OUT]
)
{
	// If this vertex doesn't already exist in the Vertices list, create a new entry.
	// Add the index of the vertex to the Indices list.
	bool	bFoundInList = false;
	DWORD	index = 0;

	// Since it's very slow to check every element in the vertex list, a hashtable stores
	// vertex indices according to the vertex position's index as reported by the OBJ file
	if ((UINT) __VertexCache.GetSize() > hashIndex) {	// hashIndex 가 1부터 시작할 때,
							// 처음에 연속적으로 1 이 들어왔을 때,
							// 그리고 실제로 같은 VERTEX 값을 가질 때,
							// 서로 다른 __Vertices 에 들어가게 된다;
							// 물론 삼각형의 경우 이럴 수는 없다.
							// 그럼에도 불구하고 위 식은
							// hashIndex 가 0 부터 시작하는 경우에 해당한다.
		CacheEntry*	pEntry = __VertexCache.GetAt(hashIndex);
		while (pEntry != NULL) {
			CPointRepr::VERTEX*	pCacheVertex = __Vertices.GetData() + pEntry->index;

			// If this vertex is identical to the vertex already in the list, simply
			// point the index buffer to the existing vertex
			if (0 == memcmp(&vertex, pCacheVertex, sizeof(CPointRepr::VERTEX))) {
				bFoundInList = true;
				index = pEntry->index;
				break;
			}

			pEntry = pEntry->pNext;
		}
	}

	// Vertex was not found in the list. Create a new entry, both within the Vertices list
	// and also within the hashtable cache
	if (!bFoundInList) {
		// Add to the Vertices list
		index = __Vertices.GetSize();
		__Vertices.Add(vertex);

		// Add this to the hashtable
		CacheEntry*	pNewEntry = new CacheEntry;
		if (pNewEntry == NULL)
			return (DWORD) -1;

		pNewEntry->index = index;
		pNewEntry->pNext = NULL;

		// Grow the cache if needed
		while ((UINT) __VertexCache.GetSize() <= hashIndex) {
			__VertexCache.Add(NULL);
		}

		// Add to the end of the linked list
		CacheEntry*	pCurEntry = __VertexCache.GetAt(hashIndex);
		if (pCurEntry == NULL) {
			// This is the head element
			__VertexCache.SetAt(hashIndex, pNewEntry);
		} else {
			// Find the tail
			while (pCurEntry->pNext != NULL) {
				pCurEntry = pCurEntry->pNext;
			}

			pCurEntry->pNext = pNewEntry;
		}
	}

	return index;
}

void	kMeshLoader::Destroy()
{
#if 1	// Voxel
	SAFE_RELEASE(__voxelVertexDeclarationPtr);
	SAFE_RELEASE(__voxelVertexBufferPtr);
	SAFE_RELEASE(__voxelIndexBufferPtr);
#endif	// Voxel
	for (int iMaterial = 0; iMaterial < __Materials.GetSize(); ++iMaterial) {
		Material*	pMaterial = __Materials.GetAt(iMaterial);

		// Avoid releasing the same texture twice
		for (int x = iMaterial + 1; x < __Materials.GetSize(); ++x) {
			Material*	pCur = __Materials.GetAt(x);
			if (pCur->texturePtr == pMaterial->texturePtr)
				pCur->texturePtr = NULL;
		}

		SAFE_RELEASE(pMaterial->texturePtr);
		SAFE_DELETE(pMaterial);
	}
	__Materials.RemoveAll();

	SAFE_RELEASE(__meshPtr);
#if 1	// DrawSubset
	SAFE_RELEASE(__meshSubsetPtr);
#endif

#if 0	// group
	__groupMap.clear();
	__gidMap.clear();
	__fid2gidMap.clear();
#endif
}

#if 1	// 마지막 주문; 13. binary .obj
#define PACK	// vArray 의 경우 반복되는 실수(특히 0)를 "pack"한다;
		// 10~20% 크기 줄이는 효과
union U_BINARY_PACK
{
	unsigned char	c[4];
	int		n;
	float		f;
	DWORD		dw;

	U_BINARY_PACK() {for (int i = 0; i < 4; ++i) c[i] = 0;}
	U_BINARY_PACK(int n_) : n(n_) {}
	U_BINARY_PACK(float f_) : f(f_) {}
	U_BINARY_PACK(DWORD dw_) : dw(dw_) {}
};

static int
write_chars(
	FILE*	filePtr,
	const unsigned char	c[4]
#ifdef PACK
	,
	unsigned	n_repeat = 1
#endif	// PACK
)
{
#ifdef PACK
	if (n_repeat == 1)
#endif	// PACK
	return fprintf(filePtr, "%c%c%c%c", c[0], c[1], c[2], c[3]) == EOF ? -1 : 0;
#ifdef PACK
	int	rval = 0;	// 0:= Ok, <0:= Oops
	for (unsigned n = 0; n < n_repeat; ++n) {
		rval = fprintf(filePtr, "%c%c%c%c", c[0], c[1], c[2], c[3]) == EOF ? -1 : 0;
		if (rval < 0)
			break;
	}
	return rval;
#endif	// PACK
}
static int	read_chars(FILE* filePtr, unsigned char c[4])
{
	for (int i = 0; i < 4; ++i) {
#if defined(_MSC_VER) && (_MSC_VER >= 1400)	// Visual Studio 2005 or above
		if (fscanf_s(filePtr, "%c", &c[i]) == EOF)
#else
		if (fscanf(filePtr, "%c", &c[i]) == EOF)
#endif
			return -1;
	}

	return 0;
}
static int	read_int(FILE* filePtr, int& n)
{
	U_BINARY_PACK	un;
	if (read_chars(filePtr, un.c) < 0)
		return -1;

	n = un.n;
	return 0;
}
static int	read_float(FILE* filePtr, float& f)
{
	U_BINARY_PACK	uf;
	if (read_chars(filePtr, uf.c) < 0)
		return -1;

	f = uf.f;
	return 0;
}
static int	read_DWORD(FILE* filePtr, DWORD& dw)
{
	U_BINARY_PACK	udw;
	if (read_chars(filePtr, udw.c) < 0)
		return -1;

	dw = udw.dw;
	return 0;
}

static const unsigned char	MAGIC[4] = {'D', 'N', '2', 'b'};
static int
is_binary(const char* filename)
{
	FILE*	filePtr = 0;
	int	rval = 0;	// 0:= Ok, <0:= Ooops
	char	filename_b[MAX_PATH];
#if defined(_MSC_VER) && (_MSC_VER >= 1400)	// Visual Studio 2005 or above
	sprintf_s(filename_b, MAX_PATH, "%s.b", filename);
#else
	sprintf(filename_b, "%s.b", filename);
#endif
do {
#if defined(_MSC_VER) && (_MSC_VER >= 1400)	// Visual Studio 2005 or above
	if (fopen_s(&filePtr, filename_b, "rb") != 0) {
 		rval = -1;
		break;
	}
#else
	filePtr = fopen(filename_b, "rb");
	if (!filePtr) {
		rval = -1;
		break;
	}
#endif
	unsigned char	magic[4];
	if (read_chars(filePtr, magic) < 0) {
		rval = -1;
		break;
	}
	if (magic[0] != MAGIC[0] || magic[1] != MAGIC[1] || magic[2] != MAGIC[2] || magic[3] != MAGIC[3]) {
		rval = -1;
		break;
	}
	rval = 0;
	break;
} while (0);
	// close
	if (filePtr) {
		fclose(filePtr);
		filePtr = 0;
	}

	return rval;
}
static int
read_binary(
	const char*	filename,
	CGrowableArray<D3DXVECTOR3>&	vArray,		// [OUT]
	CGrowableArray<D3DXVECTOR2>&	vtArray,	// [OUT]
	CGrowableArray<kMeshLoader::VOXEL>&	voxels			// [OUT]
)
{
	FILE*	filePtr = 0;
	int	rval = 0;	// 0:= Ok, <0:= Ooops
	char	filename_b[MAX_PATH];
#if defined(_MSC_VER) && (_MSC_VER >= 1400)	// Visual Studio 2005 or above
	sprintf_s(filename_b, MAX_PATH, "%s.b", filename);
#else
	sprintf(filename_b, "%s.b", filename);
#endif
do {
#if defined(_MSC_VER) && (_MSC_VER >= 1400)	// Visual Studio 2005 or above
	if (fopen_s(&filePtr, filename_b, "rb") != 0) {
		rval = -1;
		break;
	}
#else
	filePtr = fopen(filename_b, "rb");
	if (!filePtr) {
		rval = -1;
		break;
	}
#endif
	unsigned char	magic[4];
	if (read_chars(filePtr, magic) < 0) {
		(void) fprintf(stderr, "magic: read_chars(): Failure\n");
		rval = -1;
		break;
	}
	if (magic[0] != MAGIC[0] || magic[1] != MAGIC[1] || magic[2] != MAGIC[2] || magic[3] != MAGIC[3]) {
		(void) fprintf(stderr, "magic: mismatch\n");
		rval = -1;
		break;
	}
#if 1	// vArray
	int	num_vs;
	if (read_int(filePtr, num_vs) < 0) {
		(void) fprintf(stderr, "num_vs: read_int(): Failure\n");
		rval = -1;
		break;
	}
	(void) fprintf(stdout, "num_vs = %d\n", num_vs);
	rval = 0;
#ifdef PACK
	//	같은 실수(f)가 MIN_REPEAT 이상(n_repeat) 반복되면 다음과 같이 "pack"한다:
	//		FLT_MIN/n_repeat/f
	//	FLT_MIN 은 어떤 값도 가질 수 없는 값이니,
	//	읽을 때 이 값이 나오면 "pack"된 것으로 처리한다.
{
	int	inum_vs = 0;
	int	index = 0;	// D3DXVECTOR3 의 어떤 값을 결정해야 하는 지 말해준다;
				// 0:= x, 1:= y, 2:= z
	float	x, y, z;
	do {
		float	f;
		if (read_float(filePtr, f) < 0) {rval = -1; break;}
		if (f == FLT_MIN) {	// pack
			int	n_repeat;
			if (read_int(filePtr, n_repeat) < 0) {rval = -1; break;}
			// n_repeat > 0
			float	fval;
			if (read_float(filePtr, fval) < 0) {rval = -1; break;}
			// fval 가 n_repeat 번 반복된다.
			do {
				if (index == 0)
					x = fval;
				else if (index == 1)
					y = fval;
				else {	// index = 2
					z = fval;
					D3DXVECTOR3	v3(x, y, z);
					vArray.Add(v3); ++inum_vs;
				}
				index = (index + 1) % 3;
				--n_repeat;
			} while (n_repeat > 0);
		} else {
			if (index == 0)
				x = f;
			else if (index == 1)
				y = f;
			else {	// index = 2
				z = f;
				D3DXVECTOR3	v3(x, y, z);
				vArray.Add(v3); ++inum_vs;
			}
			index = (index + 1) % 3;
		}
	} while (inum_vs < num_vs);
	if (
		inum_vs != num_vs ||	// 너무 많은 D3DXVECTOR3 가 있었거나
		index != 0		// 아직 결정되지 않은 D3DXVECTOR3 가 있다!
	) {
		(void) fprintf(stderr, "index != 0\n");
		rval = -1;
		break;
	}
	(void) fprintf(stdout, "Ok\n");
}
#else	// PACK/!PACK
	for (int i = 0; i < num_vs; ++i) {
		D3DXVECTOR3	v3;
		if (read_float(filePtr, v3.x) < 0) {rval = -1; break;}
		if (read_float(filePtr, v3.y) < 0) {rval = -1; break;}
		if (read_float(filePtr, v3.z) < 0) {rval = -1; break;}
		vArray.Add(v3);
#if 0	// confirm
		D3DXVECTOR3&	v3 = vArray.GetAt(i);
		if (v3.x != x || v3.y != y || v3.z != z)
			(void) fprintf(stderr, "WARN: vArray[%d]: mismatch\n", i);
#endif	// confirm
	}
#endif	// !PACK
	if (rval < 0) {
		(void) fprintf(stderr, "READ vArray: Failure\n");
		break;
	}
#endif	// vArray
#if 1	// vtArray
	int	num_vts;
	if (read_int(filePtr, num_vts) < 0) {
		(void) fprintf(stderr, "num_vts: read_int(): Failure\n");
		rval = -1;
		break;
	}
	(void) fprintf(stdout, "num_vts = %d\n", num_vts);
	rval = 0;
	for (int i = 0; i < num_vts; ++i) {
		D3DXVECTOR2	v2;
		if (read_float(filePtr, v2.x) < 0) {rval = -1; break;}
		if (read_float(filePtr, v2.y) < 0) {rval = -1; break;}
		vtArray.Add(v2);
#if 0	// confirm
		D3DXVECTOR2&	v2 = vtArray.GetAt(i);
		if (v2.x != u || v2.y != v)
			(void) fprintf(stderr, "WARN: vtArray[%d]: mismatch\n", i);
#endif	// confirm
	}
	if (rval < 0) {
		(void) fprintf(stderr, "READ vtArray: Failure\n");
		break;
	}
#endif	// vtArray
#if 1	// voxels
	int	num_vxs;
	if (read_int(filePtr, num_vxs) < 0) {
		(void) fprintf(stderr, "num_vxs: read_int(): Failure\n");
		rval = -1;
		break;
	}
	(void) fprintf(stdout, "num_vxs = %d\n", num_vxs);
	rval = 0;
	for (int i = 0; i < num_vxs; ++i) {
		kMeshLoader::VOXEL	vx;
		if (read_float(filePtr, vx.size) < 0) {rval = -1; break;}
		if (read_float(filePtr, vx.X) < 0) {rval = -1; break;}
		if (read_float(filePtr, vx.Y) < 0) {rval = -1; break;}
		if (read_float(filePtr, vx.Z) < 0) {rval = -1; break;}
		DWORD	dwColor; if (read_DWORD(filePtr, dwColor) < 0) {rval = -1; break;}
		//#define D3DCOLOR_ARGB(a,r,g,b) ((D3DCOLOR)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))
		vx.B = (UINT) (dwColor & 0xff); dwColor >>= 8;
		vx.G = (UINT) (dwColor & 0xff); dwColor >>= 8;
		vx.R = (UINT) (dwColor & 0xff); dwColor >>= 8;
		voxels.Add(vx);
#if 0	// confirm
		kMeshLoader::VOXEL&	vx = voxels.GetAt(i);
		if (
			vx.size != size ||
			vx.X != X || vx.Y != Y || vx.Z != Z ||
			vx.R != R || vx.G != G || vx.B != B
		)
			(void) fprintf(stderr, "WARN: voxels[%d]: mismatch\n", i);
#endif	// confirm
	}
	if (rval < 0) {
		(void) fprintf(stderr, "READ voxels: Failure\n");
		break;
	}
#endif	// voxels
} while (0);
	// close
	if (filePtr) {
		fclose(filePtr);
		filePtr = 0;
	}

	if (rval < 0) {
		vArray.RemoveAll();
		vtArray.RemoveAll();
		voxels.RemoveAll();
	}
	return rval;
}
static int
write_binary(
	const char*	filename,
	const CGrowableArray<D3DXVECTOR3>&	vArray,
	const CGrowableArray<D3DXVECTOR2>&	vtArray,
	const CGrowableArray<kMeshLoader::VOXEL>&	voxels
)
{
	FILE*	filePtr = 0;
	int	rval = 0;	// 0:= Ok, <0:= Ooops
	char	filename_b[MAX_PATH];
#if defined(_MSC_VER) && (_MSC_VER >= 1400)	// Visual Studio 2005 or above
	sprintf_s(filename_b, MAX_PATH, "%s.b", filename);
#else
	sprintf(filename_b, "%s.b", filename);
#endif
do {
#if defined(_MSC_VER) && (_MSC_VER >= 1400)	// Visual Studio 2005 or above
	if (fopen_s(&filePtr, filename_b, "wb") != 0) {
		rval = -1;
		break;
	}
#else
	filePtr = fopen(filename_b, "wb");
	if (!filePtr) {
		rval = -1;
		break;
	}
#endif
#if 1	// MAGIC BYTES
	if (write_chars(filePtr, MAGIC) < 0) {
		(void) fprintf(stderr, "MAGIC: write_chars(): Failure\n");
		rval = -1;
		break;
	}
#endif	// MAGIC BYTES
#if 1	// vArray
	U_BINARY_PACK	vn(vArray.GetSize());
	if (write_chars(filePtr, vn.c) < 0) {
		(void) fprintf(stderr, "num_vs: write_chars(): Failure\n");
		rval = -1;
		break;
	}
#ifdef PACK
	//	같은 실수(f)가 MIN_REPEAT 이상(n_repeat) 반복되면 다음과 같이 "pack"한다:
	//		FLT_MIN/n_repeat/f
	//	FLT_MIN 은 어떤 값도 가질 수 없는 값이니,
	//	읽을 때 이 값이 나오면 "pack"된 것으로 처리한다.
	float	f = FLT_MIN;	// 어떤 의미있는 값도 가질 수 없는 값으로 설정한다!
	unsigned	n_repeat = 0;
#if 0	// confirm
	unsigned	n_count = 0;
#endif
	const unsigned	MIN_REPEAT = 4;	// 적어도 이 숫자만큼 반복돼야 binary 로 "pack" 하는 데 의미가 있다;
#endif	// PACK
	for (int i = 0; i < vArray.GetSize(); ++i) {
		D3DXVECTOR3&	v3 = vArray.GetAt(i);
		U_BINARY_PACK	ux(v3.x);
#ifdef PACK
		if (f == ux.f)
			++n_repeat;
		else {
			if (n_repeat >= MIN_REPEAT) {
				(void) fprintf(stdout, "index = %d, f = %f (# = %d)\n", i, *((float*) &f), n_repeat);
				float	f_min = FLT_MIN;
				if (write_chars(filePtr, (unsigned char*) &f_min) < 0) {rval = -1; break;}
				if (write_chars(filePtr, (unsigned char*) &n_repeat) < 0) {rval = -1; break;}
				if (write_chars(filePtr, (unsigned char*) &f) < 0) {rval = -1; break;}
			} else {
				if (write_chars(filePtr, (unsigned char*) &f, n_repeat) < 0) {rval = -1; break;}
			}
#if 0	// confirm
			n_count += n_repeat;
#endif
			n_repeat = 1;
			f = ux.f;
		}
#else	// PACK/!PACK
		if (write_chars(filePtr, ux.c) < 0) {rval = -1; break;}
#endif	// !PACK
		U_BINARY_PACK	uy(v3.y);
#ifdef PACK
		if (f == uy.f)
			++n_repeat;
		else {
			if (n_repeat >= MIN_REPEAT) {
				(void) fprintf(stdout, "index = %d, f = %f (# = %d)\n", i, *((float*) &f), n_repeat);
				float	f_min = FLT_MIN;
				if (write_chars(filePtr, (unsigned char*) &f_min) < 0) {rval = -1; break;}
				if (write_chars(filePtr, (unsigned char*) &n_repeat) < 0) {rval = -1; break;}
				if (write_chars(filePtr, (unsigned char*) &f) < 0) {rval = -1; break;}
			} else {
				if (write_chars(filePtr, (unsigned char*) &f, n_repeat) < 0) {rval = -1; break;}
			}
#if 0	// confirm
			n_count += n_repeat;
#endif
			n_repeat = 1;
			f = uy.f;
		}
#else	// PACK/!PACK
		if (write_chars(filePtr, uy.c) < 0) {rval = -1; break;}
#endif	// !PACK
		U_BINARY_PACK	uz(v3.z);
#ifdef PACK
		if (f == uz.f)
			++n_repeat;
		else {
			if (n_repeat >= MIN_REPEAT) {
				(void) fprintf(stdout, "index = %d, f = %f (# = %d)\n", i, *((float*) &f), n_repeat);
				float	f_min = FLT_MIN;
				if (write_chars(filePtr, (unsigned char*) &f_min) < 0) {rval = -1; break;}
				if (write_chars(filePtr, (unsigned char*) &n_repeat) < 0) {rval = -1; break;}
				if (write_chars(filePtr, (unsigned char*) &f) < 0) {rval = -1; break;}
			} else {
				if (write_chars(filePtr, (unsigned char*) &f, n_repeat) < 0) {rval = -1; break;}
			}
#if 0	// confirm
			n_count += n_repeat;
#endif
			n_repeat = 1;
			f = uz.f;
		}
#else	// PACK/!PACK
		if (write_chars(filePtr, uz.c) < 0) {rval = -1; break;}
#endif	// !PACK
	}
#ifdef PACK
	if (n_repeat) {
		(void) fprintf(stdout, "final...\n");
		(void) fprintf(stdout, "index = %d, f = %f (# = %d)\n", -1, *((float*) &f), n_repeat);
#if 0	// confirm
		n_count += n_repeat;
#endif
		if (n_repeat >= MIN_REPEAT) {
			float	f_min = FLT_MIN;
			if (write_chars(filePtr, (unsigned char*) &f_min) < 0) {rval = -1; break;}
			if (write_chars(filePtr, (unsigned char*) &n_repeat) < 0) {rval = -1; break;}
			if (write_chars(filePtr, (unsigned char*) &f) < 0) {rval = -1; break;}
		} else {
			if (write_chars(filePtr, (unsigned char*) &f, n_repeat) < 0) {rval = -1; break;}
		}
	}
#if 0	// confirm
	(void) fprintf(stdout, "n_count = %d (%d)\n", n_count, vArray.GetSize() * 3);
#endif
#endif	// PACK
	if (rval < 0) {
		(void) fprintf(stderr, "WRITE vArray: Failure\n");
		break;
	}
#endif	// vArray
#if 1	// vtArray
	U_BINARY_PACK	vtn(vtArray.GetSize());
	if (write_chars(filePtr, vtn.c) < 0) {
		(void) fprintf(stderr, "num_vts: write_chars(): Failure\n");
		rval = -1;
		break;
	}
	for (int i = 0; i < vtArray.GetSize(); ++i) {
		D3DXVECTOR2&	v2 = vtArray.GetAt(i);
		U_BINARY_PACK	uu(v2.x); if (write_chars(filePtr, uu.c) < 0) {rval = -1; break;}
		U_BINARY_PACK	uv(v2.y); if (write_chars(filePtr, uv.c) < 0) {rval = -1; break;}
	}
	if (rval < 0) {
		(void) fprintf(stderr, "WRITE vtArray: Failure\n");
		break;
	}
#endif	// vtArray
#if 1	// voxels
	if (voxels.GetSize()) {
		U_BINARY_PACK	vxn(voxels.GetSize());
		if (write_chars(filePtr, vxn.c) < 0) {
			(void) fprintf(stderr, "num_vxs: write_chars(): Failure\n");
			rval = -1;
			break;
		}
		for (int i = 0; i < voxels.GetSize(); ++i) {
			kMeshLoader::VOXEL&	vx = voxels.GetAt(i);
			U_BINARY_PACK	uS(vx.size); if (write_chars(filePtr, uS.c) < 0) {rval = -1; break;}
			U_BINARY_PACK	uX(vx.X); if (write_chars(filePtr, uX.c) < 0) {rval = -1; break;}
			U_BINARY_PACK	uY(vx.Y); if (write_chars(filePtr, uY.c) < 0) {rval = -1; break;}
			U_BINARY_PACK	uZ(vx.Z); if (write_chars(filePtr, uZ.c) < 0) {rval = -1; break;}
			D3DCOLOR	color = D3DCOLOR_XRGB(vx.R, vx.G, vx.B);
			U_BINARY_PACK	uC(color); if (write_chars(filePtr, uC.c) < 0) {rval = -1; break;}
		}
		if (rval < 0) {
			(void) fprintf(stderr, "WRITE vxArray: Failure\n");
			break;
		}
	}
#endif	// voxels
} while (0);
	// close
	if (filePtr) {
		fclose(filePtr);
		filePtr = 0;
	}

	return rval;
}
#undef PACK
#endif	// 마지막 주문; 13. binary .obj
HRESULT
kMeshLoader::__LoadGeometryFromOBJ(
	bool	v_vt_only,
	const char*	strFileName,
	const WCHAR*	texturePath,

	// [OUT]
	CGrowableArray<D3DXVECTOR3>&	__vArray,
	CGrowableArray<D3DXVECTOR2>&	__vtArray,
	CGrowableArray<F>&		__fArray,

	// [OUT]
	CGrowableArray<CPointRepr::VERTEX>&	__Vertices,
	CGrowableArray<FACE_INDEX>&		__VertexBufferIndexByF,
	CGrowableArray<DWORD>&		__Indices,
	CGrowableArray<DWORD>&		__Attributes
#if 1	// Voxel
	,
	CGrowableArray<VOXEL>&	voxels
#endif	// Voxel
)
{
	HRESULT	hr = S_OK;
	WCHAR	strMaterialFilename[MAX_PATH] = {0};
//#define CHECK_UNIQUE_POSITION
#if CHECK_UNIQUE_POSITION	// 위치 겹치는 게 있는 지...
	CGrowableArray<CPointRepr::VERTEX>	__VerticesPosition;
	CGrowableArray<CacheEntry*>		__VertexCachePosition;
#endif
	CGrowableArray<CacheEntry*>	__VertexCache;	// Hashtable cache for locating duplicate vertices
#if 1	// kMesh::SetMesh 에서 노말은 따로 계산한다.
	CGrowableArray<D3DXVECTOR3>	vnArray;
#endif
#if 1	// __VertexBufferIndexByF
	int	__lastVerticesSize = 0;
	int	__lastIndex = -1;
#endif

#if 1	// 마지막 주문; 13. binary .obj
	int	is_b = is_binary(strFileName);
	(void) fprintf(stdout, "is_binary(|%s|) = %d\n", strFileName, is_b);
	if (v_vt_only && is_b == 0) {
		if (read_binary(strFileName, __vArray, __vtArray, voxels) < 0) {
			(void) fprintf(stderr, "read_binary(|%s|): Failure\n", strFileName);
		}
		(void) fprintf(stdout, "__vArray# = %d, __vtArray# = %d, voxels# = %d\n", __vArray.GetSize(), __vtArray.GetSize(), voxels.GetSize());
		return hr;
	}
#endif	// 마지막 주문; 13. binary .obj

	FILE*	filePtr = 0;
do {
#if defined(_MSC_VER) && (_MSC_VER >= 1400)	// Visual Studio 2005 or above
	errno_t	err = fopen_s(&filePtr, strFileName, "r");
	assert(err == 0);
	if (err != 0) {
		break;
	}
#else
	filePtr = fopen(strFileName, "r");
#endif
	char	strText[MAX_PATH] = {0};	// TO DO LIST: 충분히 큰가?
	char	line[MAX_PATH];			// TO DO LIST: 충분히 큰가?
	int	iErr = 0;	// 0:= no error

#if 0
	// The first subset uses the default material
	Material*	pMaterial = new Material();
	if (pMaterial == NULL)
		return NULL;
	__InitMaterial(pMaterial);
	wcscpy_s(pMaterial->strName, MAX_PATH - 1, L"default");
	__Materials.Add(pMaterial);
#endif

	DWORD	dwCurSubset = 0;
#if 0	// group
	kID	fid;	// Face ID
	kID	gid;	// Group ID
#endif
	while (1) {
		char	command[MAX_PATH];	// TO DO LIST: 모든 명령을 다 처리할 수 있나?
		int	rval;
		if ((rval = fscanf_s(filePtr, "%s", command, MAX_PATH)) == 0 || rval == EOF) {
			(void) fprintf(stdout, "rval = %d\n", rval);
			if (rval != EOF)
				iErr = 1;
			break;
		}

		if (*command == '#') {
			if (!fgets(line, MAX_PATH, filePtr))
				break;
// Material
		} else if (strcmp(command, "mtllib") == 0) {
			if (!fgets(line, MAX_PATH, filePtr)) {
				(void) fprintf(stderr, "mtllib line error\n");
				iErr = 1;
				break;
			}
			if ((rval = sscanf_s(line, "%s", strMaterialFilename, MAX_PATH)) == 0 || rval == EOF) {
				(void) fprintf(stderr, "cannot mtllib\n");
				iErr = 2;
				break;
			}
// Group
		} else if (strcmp(command, "g") == 0) {	// group
			// 주어진 한 줄에서 group 이름을 찾는다.
			if (!fgets(line, MAX_PATH, filePtr)) {
				(void) fprintf(stderr, "group line error\n");
				iErr = 1;
				break;
			}
			if ((rval = sscanf_s(line, "%s", strText, MAX_PATH)) == 0 || rval == EOF) {
				(void) fprintf(stderr, "cannot group\n");
				iErr = 2;
				break;
			}
		} else if (strcmp(command, "s") == 0) {	// smoothing group
			// TO DO LIST: 당분간은 무시한다.
			if (!fgets(line, MAX_PATH, filePtr))
				break;
// v/vt/vn
		} else if (strcmp(command, "v") == 0) {
			// 세 개의 연속된 실수를 찾는다.
			// TO DO LIST: 한 줄에 있지 않은 경우는?
			D3DXVECTOR3	v;
			if ((rval = fscanf_s(filePtr, "%f", &v.x, sizeof(float))) == 0 || rval == EOF) {
				(void) fprintf(stderr, "cannot v.x\n");
				iErr = 2;
				break;
			}
			if ((rval = fscanf_s(filePtr, "%f", &v.y, sizeof(float))) == 0 || rval == EOF) {
				(void) fprintf(stderr, "cannot v.y\n");
				iErr = 2;
				break;
			}
			if ((rval = fscanf_s(filePtr, "%f", &v.z, sizeof(float))) == 0 || rval == EOF) {
				(void) fprintf(stderr, "cannot v.z\n");
				iErr = 2;
				break;
			}
#if 1	// 에트리 모델: Right-handed (Cartesian) Coordinate System
	// 특정 평면에 대하여 대칭; 거울 대칭
#if 1
			if (v.z != 0.f || v.z != -0.f)
				v.z = -v.z;	// xy 평면에 대하여
#else
			v.x = -v.x;	// yz 평면에 대하여
#endif
#endif
			__vArray.Add(v);
		} else if (strcmp(command, "vt") == 0) {
			// 두 개의 연속된 실수를 찾는다.
			// TO DO LIST: 한 줄에 있지 않은 경우는?
			D3DXVECTOR2	vt;
			if ((rval = fscanf_s(filePtr, "%f", &vt.x, sizeof(float))) == 0 || rval == EOF) {
				(void) fprintf(stderr, "cannot vt.u\n");
				iErr = 2;
				break;
			}
			if ((rval = fscanf_s(filePtr, "%f", &vt.y, sizeof(float))) == 0 || rval == EOF) {
				(void) fprintf(stderr, "cannot vt.v\n");
				iErr = 2;
				break;
			}
#if 1	// 에트리 모델: Right-handed (Cartesian) Coordinate System; 텍스쳐 사용
	// DirectX 에서는 OpenGL 과 달리 텍스쳐의 v 방향이 위에서 아래로 향한다.
			vt.y = 1.f - vt.y;
#endif
			__vtArray.Add(vt);
		} else if (strcmp(command, "vn") == 0) {
			// 세 개의 연속된 실수를 찾는다.
			// TO DO LIST: 한 줄에 있지 않은 경우는?
			D3DXVECTOR3	vn;
			if ((rval = fscanf_s(filePtr, "%f", &vn.x, sizeof(float))) == 0 || rval == EOF) {
				(void) fprintf(stderr, "cannot vn.x\n");
				iErr = 2;
				break;
			}
			if ((rval = fscanf_s(filePtr, "%f", &vn.y, sizeof(float))) == 0 || rval == EOF) {
				(void) fprintf(stderr, "cannot vn.y\n");
				iErr = 2;
				break;
			}
			if ((rval = fscanf_s(filePtr, "%f", &vn.z, sizeof(float))) == 0 || rval == EOF) {
				(void) fprintf(stderr, "cannot vn.z\n");
				iErr = 2;
				break;
			}
#if 1	// kMesh::SetMesh 에서 노말은 따로 계산한다.
			// 어차피 kMesh::SetMesh 에서 normal 을 계산하므로 normal 이 없어도 된다.
			continue;
//			vnArray.Add(vn);
#endif
// f
		} else if (strcmp(command, "f") == 0) {
			// #[/#/#] 형식을 읽어들인다.
			struct F	f;
			int	ierr = 0;
			for (int n = 0; n < 3; ++n) {
				FACE_INDEX&	faceIndex = f.faceIndex[n];
				DWORD&	v_index		= faceIndex.v_index; v_index = (DWORD) -1;
				DWORD&	vt_index	= faceIndex.vt_index; vt_index = (DWORD) -1;
#if 0	// kMesh::SetMesh 에서 노말은 따로 계산한다.
				DWORD&	vn_index	= faceIndex.vn_index; vn_index = -1;
#endif
				char	fmt[64];
				if ((rval = fscanf_s(filePtr, "%s", &fmt, 64)) == 0 || rval == EOF) {
					(void) fprintf(stderr, "#[/#[/#]] error\n");
					ierr = 3;
					break;
				}
				// 반드시 v 에 대한, 1부터 시작하는, 인덱스는 있다;
				// OBJ 에서 모든 인덱스는 1부터 시작한다.
				if ((rval = sscanf_s(fmt, "%d", &v_index, sizeof(int))) == 0 || rval == EOF) {
					(void) fprintf(stderr, "cannot v_index\n");
					ierr = 2;
					break;
				}
				--v_index;	// 0부터 시작하는 인덱스
				assert(v_index >= 0 && v_index < (DWORD) __vArray.GetSize());
				// '/' 가 있는 지 알아본다.
				char*	p_vt = strchr(fmt, '/');
				if (p_vt) {
					if ((rval = sscanf_s(p_vt + 1, "%d", &vt_index, sizeof(int))) == 0 || rval == EOF) {
						(void) fprintf(stderr, "cannot vt_index\n");
						ierr = 2;
						break;
					}
					--vt_index;	// 0부터 시작하는 인덱스
					assert(vt_index >= 0 && vt_index < (DWORD) __vtArray.GetSize());
#if 0	// kMesh::SetMesh 에서 노말은 따로 계산한다.
					// 어차피 kMesh::SetMesh 에서 normal 을 계산하므로 normal 이 없어도 된다;
					// 있어도 무시한다.
					char*	p_vn = strchr(p_vt + 1, '/');
					if (p_vn) {
						if ((rval = sscanf_s(p_vn + 1, "%d", &vn_index, sizeof(int))) == 0 || rval == EOF) {
							(void) fprintf(stderr, "cannot vn_index\n");
							ierr = 2;
							break;
						}
						--vn_index;	// 0부터 시작하는 인덱스
						assert(vn_index >= 0 && vn_index < (DWORD) vnArray.GetSize());
					}
#endif
				}
			}
			if (ierr) {
				iErr = 3;
				break;
			}

			if (v_vt_only)
				continue;
{
#if 1
			// 삼각형을 구성하는 세 꼭지점에 대한 정보를 인덱스를 통해 확인한다.
			//	1) 해당하는 인덱스에 대한 값이 있는 지
			//	2) 적어도 v_index 가 서로 다른 지
			//		i) v_index 가 같다면 서로 다른 꼭지점인데 같은 좌표를 가지므로
			//		삼각형을 이룰 수 없다.
			//		ii) v_index 가 다르더라고, __vArray 가 중복되는 값들을 가진다면,
			//		같은 좌표를 말할 수도 있다. 이 부분에 대한 확인은 하지 않는다;
			//		__AddVertex 를 사용하여 확인할 수 있다. (TO DO LIST)
			bool	invalid = false;
			// 1) 해당하는 인덱스에 대한 값이 있는 지
			for (int n = 0; n < 3; ++n) {
				const FACE_INDEX&	faceIndex = f.faceIndex[n];
				DWORD	v_index		= faceIndex.v_index;
				DWORD	vt_index	= faceIndex.vt_index;
#if 0	// kMesh::SetMesh 에서 노말은 따로 계산한다.
				DWORD	vn_index	= faceIndex.vn_index;
#endif
				if (
					(v_index == (DWORD) -1 || v_index >= (DWORD) __vArray.GetSize()) ||
#if 0	// kMesh::SetMesh 에서 노말은 따로 계산한다.
					(vn_index != (DWORD) -1 && vn_index >= (DWORD) vnArray.GetSize()) ||
#endif
					(vt_index != (DWORD) -1 && vt_index >= (DWORD) __vtArray.GetSize())
				) {
					invalid = true;
					break;
				}
			}
			const FACE_INDEX&	f0 = f.faceIndex[0];
			const FACE_INDEX&	f1 = f.faceIndex[1];
			const FACE_INDEX&	f2 = f.faceIndex[2];
			if (invalid) {
				(void) fprintf(stderr,
					"ERROR: Face ID = %d: Has invalid index: %d/%d %d/%d %d/%d: Ignored\n"
					,
					__fArray.GetSize(),
					f0.v_index, f0.vt_index,
					f1.v_index, f1.vt_index,
					f2.v_index, f2.vt_index
				);

				continue;	// 무시한다
			}
			// 2) 적어도 v_index 가 서로 다른 지
			if (
				f0.v_index == f1.v_index ||
				f1.v_index == f2.v_index ||
				f2.v_index == f0.v_index
			) {
				(void) fprintf(stderr,
					"WARNING: Face ID = %d: Has invalid index: At least two v indices same: %d/%d %d/%d %d/%d: Ignored\n"
					,
					__fArray.GetSize(),
					f0.v_index, f0.vt_index,
					f1.v_index, f1.vt_index,
					f2.v_index, f2.vt_index
				);

				continue;	// 무시한다
			}
#endif
}
			for (int n = 0; n < 3; ++n) {
				const FACE_INDEX&	faceIndex = f.faceIndex[n];
				DWORD	v_index		= faceIndex.v_index;
				DWORD	vt_index	= faceIndex.vt_index;
#if 0	// kMesh::SetMesh 에서 노말은 따로 계산한다.
				DWORD	vn_index	= faceIndex.vn_index;
#endif

				// v_index 가 가리키는 값이 __vArray 에서 Unique 한 것은 아니다!
				// TO DO LIST: __vArray 를 Unique 한 Vertex 들의 배열로 만들고,
				// 그 안에서 index 를 찾을 수도 있다;
				// DirectX 의 MeshFromOBJ 예제의 CMeshLoader::__AddVertex 처럼.
				// 하지만 SetPolygon 에서 GenerateAdjacency, ConvertAdjacencyToPointReps 를 이용하여
				// 이 작업을 하므로 굳이 할 필요가 없다.
				//
				// 기본적으로 Normal 과 (u, v) 는 주어진 Face 에 대해서 정의되어 있다.
				// Normal 의 경우, 주어진 Vertex 를 공유하는 Face 의 Normal 을 평균해서 사용하면 되므로
				// 크게 문제될 것은 없다.
				// 그러나 Texture 를 사용하는 Mapping 을 고려하면 얘기는 달라진다;
				// 같은 Vertex 라 하더라도 그것을 사용하는 Face 에 따라 (u, v) 가 달라질 수 있고,
				// 이들로부터 주어진 Vertex 의 값으로 설정할 수 없다.
				// 그렇다고 Face 를 기준으로 Vertex Buffer 를 설정하자니
				// 위치적으로 같은 Vertex 임에도 불구하고 서로 다른 Vertex 인 것처럼 설정된다.
				// 따라서 Texture Mapping 을 위해 따로 Vertex Buffer 를 구성해야 한다:
				//	1) 지금까지 처럼 Mesh 자체의 구조를 위한 Vertex Buffer
				//	2) Texture Mapping 을 통한 렌더링을 위한 Vertex Buffer
				// 그러므로 kMesh::D3DXVERTEX 구조에서 (tu, tv) 는 더 이상 의미가 없다!
				// 몇 개월이 지난 이제야 발견하다니...그래도 늦지는 않았다.
				CPointRepr::VERTEX	vertex;
				ZeroMemory(&vertex, sizeof(CPointRepr::VERTEX));
				vertex.position = __vArray.GetAt(v_index);
#if 0	// kMesh::SetMesh 에서 노말은 따로 계산한다.
				if (vn_index != (DWORD) -1)
					vertex.normal = vnArray.GetAt(vn_index);
#endif
				if (vt_index != (DWORD) -1)
					vertex.texcoord = __vtArray.GetAt(vt_index);

				// __Vertices 가 Vertex Buffer 를 채우므로,
				// index 가 Vertex Buffer 에 대한 인덱스이다.
				// 새로운 vertex 라면 __Vertices 에 순차적으로 추가된다.
				DWORD	index = __AddVertex(vertex, v_index, __VertexCache, __Vertices);	// v_index: 0부터...
#if CHECK_UNIQUE_POSITION	// 위치 겹치는 게 있는 지...
{
	CPointRepr::VERTEX	v_position;
	ZeroMemory(&v_position, sizeof(CPointRepr::VERTEX));
	v_position.position = __vArray.GetAt(v_index);
	__AddVertex(v_position, v_index, __VertexCachePosition, __VerticesPosition);
}
#endif
#if 1	// __VertexBufferIndexByF
				if (__lastVerticesSize != __Vertices.GetSize()) {       // vertex 가 __Vertices 에 새로 추가됐다.
					// __Vertices 에 순차적으로 추가되었는 지 확인한다.
					__lastVerticesSize = __Vertices.GetSize();
					assert((int) index == __lastIndex + 1);
					__lastIndex = index;

					// vertex 에 해당하는 인덱스 정보(F)도 저장한다; __Vertices 와 같은 순서로...
					__VertexBufferIndexByF.Add(faceIndex);
				}
#endif
				__Indices.Add(index);
			}
			__fArray.Add(f);

			__Attributes.Add(dwCurSubset);
#if 0	// group
{
			// 마야에서 사각형 메쉬를 삼각형 메쉬로 자를 때(Triangulate),
			// 같은 group 이 여러 군데 분산된 경우,
			// 각각을 group### 처럼 서로 다른 group 으로 구별한다: 1부터...
			// 따라서 group 이름에서 숫자부분을 떼고 생각한다.
			char	bareGroup[MAX_PATH] = {0};	// TO DO LIST: 충분히 큰가?
			char*	q = bareGroup;
			char*	p = strText;
			while (*p) {
#if 1
				if (*p > '0' && *p <= '9') {
					*q = '\0';
					break;
				}
#endif
				*q++ = *p++;
			}
			++fid;	// 새로운 Face ID

			GROUP_INFO_HASH::iterator	it;
			it = __groupMap.find(bareGroup);
			if (it == __groupMap.end()) {	// 새로운 Group 이다.
				++gid;	// 새로운 Group ID
				__gidMap[gid] = bareGroup;
				__groupMap[bareGroup] = struct GROUP_INFO(gid, fid);
				__fid2gidMap[fid] = gid;
			} else {
				struct GROUP_INFO&	groupInfo = it->second;
				groupInfo.__fidList.push_back(fid);
				__fid2gidMap[fid] = groupInfo.__gid;
			}
}
#endif

#if 1	// Voxel
		} else if (strcmp(command, "vx") == 0) {
			// vx 2.500000 423.750000 883.750000 113.750000 150 104 101
			VOXEL	voxel;
			if ((rval = fscanf_s(filePtr, "%f", &voxel.size, sizeof(float))) == 0 || rval == EOF) {
				(void) fprintf(stderr, "cannot voxel.size\n");
				iErr = 2;
				break;
			}
			if ((rval = fscanf_s(filePtr, "%f", &voxel.X, sizeof(float))) == 0 || rval == EOF) {
				(void) fprintf(stderr, "cannot voxel.X\n");
				iErr = 2;
				break;
			}
			if ((rval = fscanf_s(filePtr, "%f", &voxel.Y, sizeof(float))) == 0 || rval == EOF) {
				(void) fprintf(stderr, "cannot voxel.Y\n");
				iErr = 2;
				break;
			}
			if ((rval = fscanf_s(filePtr, "%f", &voxel.Z, sizeof(float))) == 0 || rval == EOF) {
				(void) fprintf(stderr, "cannot voxel.Z\n");
				iErr = 2;
				break;
			}
			if ((rval = fscanf_s(filePtr, "%u", &voxel.R, sizeof(UINT))) == 0 || rval == EOF) {
				(void) fprintf(stderr, "cannot voxel.R\n");
				iErr = 2;
				break;
			}
			if ((rval = fscanf_s(filePtr, "%u", &voxel.G, sizeof(UINT))) == 0 || rval == EOF) {
				(void) fprintf(stderr, "cannot voxel.G\n");
				iErr = 2;
				break;
			}
			if ((rval = fscanf_s(filePtr, "%u", &voxel.B, sizeof(UINT))) == 0 || rval == EOF) {
				(void) fprintf(stderr, "cannot voxel.B\n");
				iErr = 2;
				break;
			}
#if 1	// 에트리 모델: Right-handed (Cartesian) Coordinate System
	// 특정 평면에 대하여 대칭; 거울 대칭
#if 1
			if (voxel.Z != 0.f && voxel.Z != -0.f)
				voxel.Z = -voxel.Z;
#endif
#endif
			voxels.Add(voxel);
#endif	// Voxel
#if 1	// Material
		} else if (0 == strcmp(command, "usemtl")) {
			// 주어진 한 줄에서 Material 이름을 찾는다.
			if (!fgets(line, MAX_PATH, filePtr)) {
				(void) fprintf(stderr, "material line error\n");
				iErr = 1;
				break;
			}
			if ((rval = sscanf_s(line, "%s", strText, MAX_PATH)) == 0 || rval == EOF) {
				(void) fprintf(stderr, "cannot material\n");
				iErr = 2;
				break;
			}
			// Material
			WCHAR	strName[MAX_PATH] = {0};
			MultiByteToWideChar(CP_ACP, 0, strText, -1, strName, MAX_PATH);
			strName[MAX_PATH - 1] = 0;

			bool bFound = false;
			for (int iMaterial = 0; iMaterial < __Materials.GetSize(); ++iMaterial) {
				Material*	pCurMaterial = __Materials.GetAt(iMaterial);
				if (0 == wcscmp(pCurMaterial->strName, strName)) {
					bFound = true;
					dwCurSubset = iMaterial;
					break;
				}
			}

			if (!bFound) {
				Material*	pMaterial = new Material();
				if (pMaterial == NULL)
					return NULL;

				dwCurSubset = __Materials.GetSize();
#ifdef _DEBUG
				(void) fprintf(stdout, "Subset ID = %d, %s:= |%s|\n", dwCurSubset, texturePath ? "Texture" : "Material", strText);
#endif
				__InitMaterial(pMaterial);
				wcscpy_s(pMaterial->strName, MAX_PATH - 1, strName);
#if 1	// 에트리 모델: Right-handed (Cartesian) Coordinate System; 텍스쳐 사용
/*
에트리 모델의 경우 "usemtl"을 사용하지 않는다.
"EW_0RHS.obj" 와 "EW_1RHS.obj" 를 저장할 때 모든 면에 대해서 기본값인 "initialShadingGroup"을 사용한다.
이를 적절하게 해당 텍스쳐의 이름으로 바꿔서 subset 을 사용할 수 있도록 한다.
	주) 원래는 kMeshLoader::__LoadMaterialFromMTL 을 제대로 사용하여 "map_Kd"로부터 텍스쳐 파일을 얻어야 하나,
	여기서는 위와 같은 편법을 쓴다.

이 부분이 에트리에 요구할 사항이다;
	면의 정보를 쓰기 전에 해당 면이 어떤 Material 인지를 말해주어야 하는 "usemtl"이 없다.
	원래는 .mtl 형식에 맞춰어야 하나 여기서는 텍스쳐 파일만 알려주어도 충분하므로
	다음과 같이 편법을 사용한다.

	현재의
		g HEAD
		f ...
		......
	대신에
		g HEAD
		usemtl victoria-body.bmp
		f ...
		......
 */
				if (texturePath) {
					wcscpy_s(pMaterial->strTexture, MAX_PATH - 1, strName);
				}
#endif

				__Materials.Add(pMaterial);
			}
#endif
		} else {
			(void) fprintf(stderr, "Unknown command: |%s|\n", command);
		}
	}
	if (!iErr && ferror(filePtr)) {
		iErr = 2;
		break;
	}
	if (iErr) {
		break;
	}
} while (0);
#if CHECK_UNIQUE_POSITION	// 위치 겹치는 게 있는 지...
(void) fprintf(stdout, "# = %d\n", __vArray.GetSize());
(void) fprintf(stdout, "# = %d\n", __VertexCachePosition.GetSize());
#endif
	// close
	if (filePtr) {
		fclose(filePtr);
		filePtr = 0;
	}

#if 1	// DeleteCache
	// Iterate through all the elements in the cache and subsequent linked lists
	for (int i = 0; i < __VertexCache.GetSize(); ++i) {
		CacheEntry*	pEntry = __VertexCache.GetAt(i);
		while (pEntry != NULL) {
			CacheEntry*	pNext = pEntry->pNext;
			if (pEntry) {
				delete pEntry;
				pEntry = 0;
			}
			pEntry = pNext;
		}
	}
	__VertexCache.RemoveAll();
#endif

	if (strMaterialFilename[0]) {
		V_RETURN(__LoadMaterialsFromMTL(strMaterialFilename));
	}

#if 1	// 마지막 주문; 13. binary .obj
	if (v_vt_only) {
		if (write_binary(strFileName, __vArray, __vtArray, voxels) < 0) {
			(void) fprintf(stderr, "write_binary(|%s|): Failure\n", strFileName);
		}
	}
#endif	// 마지막 주문; 13. binary .obj
	return hr;
}

HRESULT	kMeshLoader::__LoadMaterialsFromMTL(LPCTSTR strFileName)
{
	HRESULT	hr = S_OK;

#if 0	// TO DO LIST
	// Set the current directory based on where the mesh was found
	WCHAR wstrOldDir[MAX_PATH] = {0};
	GetCurrentDirectory( MAX_PATH, wstrOldDir );
	SetCurrentDirectory( m_strMediaDir );

	// Find the file
	WCHAR strPath[MAX_PATH];
	char cstrPath[MAX_PATH];
	V_RETURN( DXUTFindDXSDKMediaFileCch( strPath, MAX_PATH, strFileName ) );
	WideCharToMultiByte( CP_ACP, 0, strPath, -1, cstrPath, MAX_PATH, NULL, NULL );

	// File input
	WCHAR strCommand[256] = {0};
	wifstream InFile( cstrPath );
	if( !InFile )
	return DXTRACE_ERR( L"wifstream::open", E_FAIL );

	// Restore the original current directory
	SetCurrentDirectory( wstrOldDir );

	Material* pMaterial = NULL;

	for(; ; )
	{
	InFile >> strCommand;
	if( !InFile )
	break;

	if( 0 == wcscmp( strCommand, L"newmtl" ) )
	{
	// Switching active materials
	WCHAR strName[MAX_PATH] = {0};
	InFile >> strName;

	pMaterial = NULL;
	for( int i = 0; i < __Materials.GetSize(); i++ )
	{
	Material* pCurMaterial = __Materials.GetAt( i );
	if( 0 == wcscmp( pCurMaterial->strName, strName ) )
	{
	pMaterial = pCurMaterial;
	break;
	}
	}
	}

	// The rest of the commands rely on an active material
	if( pMaterial == NULL )
	continue;

	if( 0 == wcscmp( strCommand, L"#" ) )
	{
	// Comment
	}
	else if( 0 == wcscmp( strCommand, L"Ka" ) )
	{
	// Ambient color
	float r, g, b;
	InFile >> r >> g >> b;
	pMaterial->vAmbient = D3DXVECTOR3( r, g, b );
	}
	else if( 0 == wcscmp( strCommand, L"Kd" ) )
	{
	// Diffuse color
	float r, g, b;
	InFile >> r >> g >> b;
	pMaterial->vDiffuse = D3DXVECTOR3( r, g, b );
	}
	else if( 0 == wcscmp( strCommand, L"Ks" ) )
	{
	// Specular color
	float r, g, b;
	InFile >> r >> g >> b;
	pMaterial->vSpecular = D3DXVECTOR3( r, g, b );
	}
	else if( 0 == wcscmp( strCommand, L"d" ) ||
	0 == wcscmp( strCommand, L"Tr" ) )
	{
	// Alpha
	InFile >> pMaterial->fAlpha;
	}
	else if( 0 == wcscmp( strCommand, L"Ns" ) )
	{
	// Shininess
	int nShininess;
	InFile >> nShininess;
	pMaterial->nShininess = nShininess;
	}
	else if( 0 == wcscmp( strCommand, L"illum" ) )
	{
	// Specular on/off
	int illumination;
	InFile >> illumination;
	pMaterial->bSpecular = ( illumination == 2 );
	}
	else if( 0 == wcscmp( strCommand, L"map_Kd" ) )
	{
	// Texture
	InFile >> pMaterial->strTexture;
	}

	else
	{
	// Unimplemented or unrecognized command
	}

	InFile.ignore( 1000, L'\n' );
	}

	InFile.close();

	return S_OK;
#endif

	return hr;
}


HRESULT	kMeshLoader::Create(
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
)
{
	HRESULT	hr = S_OK;

	Destroy();

	CGrowableArray<D3DXVECTOR3>	__vArray;
	CGrowableArray<D3DXVECTOR2>	__vtArray;
	CGrowableArray<F>		__fArray;
	CGrowableArray<CPointRepr::VERTEX>	__Vertices;	// Filled and copied to the vertex buffer
	CGrowableArray<FACE_INDEX>		__VertexBufferIndexByF;
	CGrowableArray<DWORD>		__Indices;	// Filled and copied to the index buffer
	CGrowableArray<DWORD>		__Attributes;	// Filled and copied to the attribute buffer
#if 1	// Voxel
	CGrowableArray<VOXEL>	voxels;
#endif	// Voxel
#ifndef __READ_FILES__
	bool	v_vt_only = false;
#if 1	// Voxel
	V_RETURN(__LoadGeometryFromOBJ(
		v_vt_only,
		strFileName, texturePath,

		__vArray, __vtArray, __fArray,

		__Vertices, __VertexBufferIndexByF, __Indices, __Attributes
		,
		voxels
	));
#else	// Voxel/!Voxel
	V_RETURN(__LoadGeometryFromOBJ(
		v_vt_only,
		strFileName, texturePath,

		__vArray, __vtArray, __fArray,

		__Vertices, __VertexBufferIndexByF, __Indices, __Attributes
	));
#endif	// Voxel
#else
	if (strFileName) {
#if defined(__READ_FILES__)
		bool	v_vt_only = true;
#else
		bool	v_vt_only = false;
#endif
#if 1	// Voxel
		V_RETURN(__LoadGeometryFromOBJ(
			v_vt_only,
			strFileName, texturePath,

			__vArray, __vtArray, __fArray,

			__Vertices, __VertexBufferIndexByF, __Indices, __Attributes
			,
			voxels
		));
#else	// Voxel/!Voxel
		V_RETURN(__LoadGeometryFromOBJ(
			v_vt_only,
			strFileName, texturePath,

			__vArray, __vtArray, __fArray,

			__Vertices, __VertexBufferIndexByF, __Indices, __Attributes
		));
#endif
	}
#endif
#if defined(__READ_FILES__)
	int	numElements;	// 배열 자체의 크기로 의미있는 성분의 갯수와는 다르다;
				// for loop counter 에서 실제 갯수를 따진다:
				// 예를 들어 __Vertices__ 의 경우 numElements / 5
#ifdef MULTI_STANDARD
#if 1	// kFileCache
	assert(index_to_standard >= 0 && index_to_standard < (int) kFileCache::__numEntries_in_MESH_LOADER_READ_FILES);
	const MESH_LOADER_READ_FILES&	meshLoader_READ_FILES = kFileCache::__pointer_to_MESH_LOADER_READ_FILES[index_to_standard];
#else	// kFileCache/!kFileCache
	assert(index_to_standard >= 0 && index_to_standard < sizeof(__meshLoader_READ_FILES__) / sizeof(MESH_LOADER_READ_FILES));
	const MESH_LOADER_READ_FILES&	meshLoader_READ_FILES = __meshLoader_READ_FILES__[index_to_standard];
#endif	// !kFileCache
#endif	// MULTI_STANDARD
	(void) fprintf(stdout, "Reading...\n");

	(void) fprintf(stdout, "\tSetting __Indices...");
	////////////////////////////////////////////////////////////////////////
	__Indices.RemoveAll();
#ifdef MULTI_STANDARD
	const DWORD*	__Indices__ = meshLoader_READ_FILES.__Indices__.ptr;
	numElements = meshLoader_READ_FILES.__Indices__.n;
#else	// MULTI_STANDARD/!MULTI_STANDARD
	numElements = sizeof(__Indices__) / sizeof(DWORD);
#endif	// !MULTI_STANDARD
	for (int n = 0; n < numElements; ++n)
		__Indices.Add(__Indices__[n]);
	////////////////////////////////////////////////////////////////////////
	(void) fprintf(stdout, "...Done\n");

	(void) fprintf(stdout, "\tSetting __Vertices...");
	////////////////////////////////////////////////////////////////////////
if (!strFileName) {
	__Vertices.RemoveAll();
	// v.x, v.y, v.z, vt.x, vt.y
#ifdef MULTI_STANDARD
	const float*	__Vertices__ = meshLoader_READ_FILES.__Vertices__.ptr;
	numElements = meshLoader_READ_FILES.__Vertices__.n;
#else	// MULTI_STANDARD/!MULTI_STANDARD
	numElements = sizeof(__Vertices__) / sizeof(float);
#endif	// !MULTI_STANDARD
	for (int n = 0; n < numElements / 5; ++n) {
		CPointRepr::VERTEX	vertex;
		ZeroMemory(&vertex, sizeof(CPointRepr::VERTEX));
		vertex.position.x = __Vertices__[5 * n + 0];
		vertex.position.y = __Vertices__[5 * n + 1];
		vertex.position.z = __Vertices__[5 * n + 2];
		vertex.texcoord.x = __Vertices__[5 * n + 3];
		vertex.texcoord.y = __Vertices__[5 * n + 4];
		__Vertices.Add(vertex);
	}
} else {
	__Vertices.RemoveAll();
	// faceIndex.v_index, faceIndex.vt_index
#ifdef MULTI_STANDARD
	const int*	__VertexBufferIndexByF__ = meshLoader_READ_FILES.__VertexBufferIndexByF__.ptr;
	numElements = meshLoader_READ_FILES.__VertexBufferIndexByF__.n;
#else	// MULTI_STANDARD/!MULTI_STANDARD
	numElements = sizeof(__VertexBufferIndexByF__) / sizeof(int);
#endif	// !MULTI_STANDARD
	for (int n = 0; n < numElements / 2; ++n) {
		CPointRepr::VERTEX	vertex;
		ZeroMemory(&vertex, sizeof(CPointRepr::VERTEX));
		int	v_index = __VertexBufferIndexByF__[2 * n + 0];
		int	vt_index = __VertexBufferIndexByF__[2 * n + 1];
		if (v_index < __vArray.GetSize())
			vertex.position = __vArray.GetAt(v_index);//__VertexBufferIndexByF__[2 * n + 0]);
		else
			(void) fprintf(stderr, "v_index = %d: Out of range (%d)\n", v_index, __vArray.GetSize());
		if (vt_index >= 0 && vt_index < __vtArray.GetSize())
			vertex.texcoord = __vtArray.GetAt(vt_index);//__VertexBufferIndexByF__[2 * n + 1]);
		else {
			if (vt_index >= 0)
				(void) fprintf(stderr, "vt_index = %d: Out of range (%d)\n", vt_index, __vtArray.GetSize());
		}
		__Vertices.Add(vertex);
#if 0	// __Vertices__ 는 측정모델뿐 아니라 표준모델도 사용하므로 이렇게 덮어쓰면 표준모델이 달라진다; rollback
		__Vertices__[5 * n + 0] = vertex.position.x;
		__Vertices__[5 * n + 1] = vertex.position.y;
		__Vertices__[5 * n + 2] = vertex.position.z;
		__Vertices__[5 * n + 3] = vertex.texcoord.x;
		__Vertices__[5 * n + 4] = vertex.texcoord.y;
#endif
	}
}
	////////////////////////////////////////////////////////////////////////
	(void) fprintf(stdout, "...Done\n");

	(void) fprintf(stdout, "\tSetting __Attributes...");
	////////////////////////////////////////////////////////////////////////
	__Attributes.RemoveAll();
#ifdef MULTI_STANDARD
	const int*	__Attributes__ = meshLoader_READ_FILES.__Attributes__.ptr;
	numElements = meshLoader_READ_FILES.__Attributes__.n;
#else	// MULTI_STANDARD/!MULTI_STANDARD
	numElements = sizeof(__Attributes__) / sizeof(int);
#endif	// !MULTI_STANDARD
	for (int n = 0; n < numElements / 2; ++n) {
		for (int i = 0; i < __Attributes__[2 * n + 1]; ++i)
			__Attributes.Add(__Attributes__[2 * n + 0]);
	}
	////////////////////////////////////////////////////////////////////////
	(void) fprintf(stdout, "...Done\n");

	(void) fprintf(stdout, "\tSetting Texture...");
	////////////////////////////////////////////////////////////////////////
	for (int iMaterial = 0; iMaterial < __Materials.GetSize(); ++iMaterial) {
		Material*	pMaterial = __Materials.GetAt(iMaterial);

		// Avoid releasing the same texture twice
		for (int x = iMaterial + 1; x < __Materials.GetSize(); ++x) {
			Material*	pCur = __Materials.GetAt(x);
			if (pCur->texturePtr == pMaterial->texturePtr)
				pCur->texturePtr = NULL;
		}

		SAFE_RELEASE(pMaterial->texturePtr);
		SAFE_DELETE(pMaterial);
	}
	__Materials.RemoveAll();
#ifdef MULTI_STANDARD
	LPCWSTR*	__Textures__ = textureArrayPtr ? textureArrayPtr->ptr : meshLoader_READ_FILES.__Textures__.ptr;
	numElements = textureArrayPtr ? textureArrayPtr->n : meshLoader_READ_FILES.__Textures__.n;
#else	// MULTI_STANDARD/!MULTI_STANDARD
	numElements = sizeof(__Textures__) / sizeof(LPCWSTR);
#endif	// !MULTI_STANDARD
	for (int n = 0; n < numElements; ++n) {
		(void) fwprintf(stdout, L"Texture[%d]: |%s|\n", n, __Textures__[n]);
		Material*	pMaterial = new Material();
		__InitMaterial(pMaterial);
		WCHAR		strName[MAX_PATH];
		swprintf_s(strName, MAX_PATH, L"__Textures__[%d]", n);
		wcscpy_s(pMaterial->strName, MAX_PATH - 1, strName);
		wcscpy_s(pMaterial->strTexture, MAX_PATH - 1, __Textures__[n]);
		__Materials.Add(pMaterial);
	}
	////////////////////////////////////////////////////////////////////////
	(void) fprintf(stdout, "...Done\n");

	(void) fprintf(stdout, "...Done\n");
#endif	// defined(__READ_FILES__) || defined(__READ_OBJ_FILE__)
#ifdef __WRITE_FILES__
	(void) fprintf(stdout, "Writing...\n");
{
#if 0	// .obj 로부터 읽어들인 Face 에 대한 정보를 쓴다:
{
	//	Face ID: (x0, y0, -z0), (x1, y1, -z1), (x2, y2, -z2); RH->LH
	//
	// 서로 다른 .OBJ 더라도 같은 오브젝트를 기술하고
	// 더욱이 Face 의 순서가 같다면 두 .OBJ 가 생성하는 이 파일은 같다.
	(void) fprintf(stdout, "Writing __fArray...\n");
	FILE*	filePtr = 0;
	char	filename[] = "__fArray";
do {
#if defined(_MSC_VER) && (_MSC_VER >= 1400)	// Visual Studio 2005 or above
	errno_t	err = fopen_s(&filePtr, filename, "w");
	assert(err == 0);
	if (err != 0) {
		break;
	}
#else
	filePtr = fopen(filename, "w");
#endif
	(void) fprintf(filePtr, "# Face ID: (x0, y0, -z0), (x1, y1, -z1), (x2, y2, -z2); RH->LH\n");
	for (int n = 0; n < __fArray.GetSize(); ++n) {
		const struct F&		f = __fArray.GetAt(n);
		const D3DXVECTOR3&	v0 = __vArray.GetAt(f.faceIndex[0].v_index);
		const D3DXVECTOR3&	v1 = __vArray.GetAt(f.faceIndex[1].v_index);
		const D3DXVECTOR3&	v2 = __vArray.GetAt(f.faceIndex[2].v_index);
		(void) fprintf(filePtr,
			"%d\t(%f, %f, %f)\t(%f, %f, %f)\t(%f, %f, %f)\n"
			,
			n, v0.x, v0.y, -v0.z, v1.x, v1.y, -v1.z, v2.x, v2.y, -v2.z
		);
	}
} while (0);
	if (filePtr) {
		(void) fclose(filePtr);
		filePtr = 0;
	}
(void) fprintf(stdout, "...Done\n");
}
#endif
#if 1	// __Indices 정보를 파일에 쓴다;
{
	//
	//	같은 .OBJ 파일을 읽어들이는 데 매먼 __LoadGeometryFromOBJ 를 호출할 필요가 있나?
	//
	// Face 순서가 같고 Face 를 구성하는 Vertex 의 좌표가 같다면
	// 서로 다른 .OBJ 라 하더라도 __Indices 정보는 같다:
	//	Victoria_0609_rearrange.obj	(에트리에서 보낸 표준모델)
	//	EW_0RH.obj			(마야에서 읽어들여 다시 출력한 표준모델)
	// 왜?
	// Face 를 구성하는 각 Vertex 의 좌표(텍스쳐 좌표 포함)를 가지고 __Vertices 를 구성하고,
	// 해당하는 __Vertices 에 대한 인덱스로 __Indices 를 구성하므로;
	// 물론 같은 파일을 계속 읽어들이면 당연히 __Indices 정보도 같다.
	FILE*	filePtr = 0;
	char	filename[] = INDICES__FILENAME;
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
	(void) fprintf(filePtr, "static const DWORD __Indices_ID%d__[%d]={", STANDARD_ID, __Indices.GetSize());
	(void) fprintf(stdout, "/* ... */	{__Indices_ID%d__, %d},\n", STANDARD_ID, __Indices.GetSize());
#else	// MULTI_STANDARD/!MULTI_STANDARD
	(void) fprintf(filePtr, "static const DWORD __Indices__[%d]={", __Indices.GetSize());
#endif	// !MULTI_STANDARD
	for (int n = 0; n < __Indices.GetSize(); ++n)
		(void) fprintf(filePtr, "%d,", __Indices.GetAt(n));
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
#if 1	// __Vertices 정보를 파일에 쓴다;
{
	//	Face 의 순서가 같다면, 비록 v, vt 의 순서는 다르다 할지라도,
	//	두 .OBJ 가 생성하는 이 파일은 같다.
	FILE*	filePtr = 0;
	char	filename[] = VERTICES__FILENAME;
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
	(void) fprintf(filePtr, "static const float __Vertices_ID%d__[%d]={", STANDARD_ID, 5 * __Vertices.GetSize());
	(void) fprintf(stdout, "/* ... */	{__Vertices_ID%d__, %d},\n", STANDARD_ID, 5 * __Vertices.GetSize());
#else	// MULTI_STANDARD/!MULTI_STANDARD
	(void) fprintf(filePtr, "static const float __Vertices__[%d]={", 5 * __Vertices.GetSize());
#endif	// !MULTI_STANDARD
	for (int n = 0; n < __Vertices.GetSize(); ++n) {
		const CPointRepr::VERTEX&	vertex = __Vertices.GetAt(n);
		const D3DXVECTOR3&	v = vertex.position;
		const D3DXVECTOR2&	vt = vertex.texcoord;
		(void) fprintf(filePtr, "%ff,%ff,%ff,%ff,%ff,", v.x, v.y, v.z, vt.x, vt.y);
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
#if 1	// __VertexBufferIndexByF 정보를 파일에 쓴다.
{
	//
	// 만약 서로 다른 .OBJ 파일인데, Face 의 순서나 v, vt 의 순서가 같다면...
	//
	// __Vertices 를 구성할 때 사용됐던 v, vt 의 인덱스(v_index, vt_index) 정보인
	// __VertexBufferIndexByF 와 v, vt 배열만 있다면,
	// kMeshLoader::__AddVertex 를 사용하지 않고, __Vertices 를 바로 구성할 수 있다.
	FILE*	filePtr = 0;
	char	filename[] = VERTEXBUFFERINDEXBYF__FILENAME;
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
	(void) fprintf(filePtr, "static const int __VertexBufferIndexByF_ID%d__[%d]={", STANDARD_ID, 2 * __VertexBufferIndexByF.GetSize());
	(void) fprintf(stdout, "/* ... */	{__VertexBufferIndexByF_ID%d__, %d},\n", STANDARD_ID, 2 * __VertexBufferIndexByF.GetSize());
#else	// MULTI_STANDARD/!MULTI_STANDARD
	(void) fprintf(filePtr, "static const int __VertexBufferIndexByF__[%d]={", 2 * __VertexBufferIndexByF.GetSize());
#endif	// !MULTI_STANDARD
	for (int n = 0; n < __VertexBufferIndexByF.GetSize(); ++n) {
		const FACE_INDEX&	faceIndex = __VertexBufferIndexByF.GetAt(n);
		(void) fprintf(filePtr, "%d,%d,", faceIndex.v_index, faceIndex.vt_index);
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
#if 1	// __Attributes 정보를 파일에 쓴다.
{
	FILE*	filePtr = 0;
	char	filename[] = ATTRIBUTES__FILENAME;
	int*	numSubsetPtr = 0;
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
	// __Attributes 의 값은 0 부터 순차적으로 증가한다.
	int	max_subset = 0;
	if (__Attributes.GetSize())
		max_subset = __Attributes.GetAt(__Attributes.GetSize() - 1) + 1;
	numSubsetPtr = new int[max_subset];
	for (int n = 0; n < max_subset; ++n)
		numSubsetPtr[n] = 0;
	for (int i = 0; i < __Attributes.GetSize(); ++i)
		++numSubsetPtr[__Attributes.GetAt(i)];
#ifdef MULTI_STANDARD
	(void) fprintf(filePtr, "static const int __Attributes_ID%d__[%d]={", STANDARD_ID, 2 * max_subset);
	(void) fprintf(stdout, "/* ... */	{__Attributes_ID%d__, %d},\n", STANDARD_ID, 2 * max_subset);
#else	// MULTI_STANDARD/!MULTI_STANDARD
	(void) fprintf(filePtr, "static const int __Attributes__[%d]={", 2 * max_subset);
#endif	// !MULTI_STANDARD
	for (int n = 0; n < max_subset; ++n)
		(void) fprintf(filePtr, "%d,%d,", n, numSubsetPtr[n]);
	(void) fprintf(filePtr, "};\n");
	////////////////////////////////////////////////////////////////////////
	(void) fprintf(stdout, "...Done\n");			// }
} while (0);
	if (numSubsetPtr) {
		delete[] numSubsetPtr;
		numSubsetPtr = 0;
	}
	if (filePtr) {
		(void) fclose(filePtr);
		filePtr = 0;
	}
}
#endif
#if 1	// 텍스쳐 정보를 파일에 쓴다.
{
	FILE*	filePtr = 0;
	char	filename[] = TEXTURES__FILENAME;
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
	(void) fprintf(filePtr, "static LPCWSTR __Textures_ID%d__[%d]={\n", STANDARD_ID, __Materials.GetSize());
	(void) fprintf(stdout, "/* ... */	{__Textures_ID%d__, %d},\n", STANDARD_ID, __Materials.GetSize());
#else	// MULTI_STANDARD/!MULTI_STANDARD
	(void) fprintf(filePtr, "static LPCWSTR __Textures__[%d]={\n", __Materials.GetSize());
#endif	// !MULTI_STANDARD
	for (int n = 0; n < __Materials.GetSize(); ++n)	// "default"는 건너뛴다.
		(void) fwprintf(filePtr, L"\tL\"%s\",\n", __Materials.GetAt(n)->strTexture);
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
#endif	// __WRITE_FILES__

	// 다 읽어들였다;
	// 이제부터 메쉬를 만들자;
	// 다시 한 번 __vArray 에 대한 부분을 적어보자:
	//	v_index 가 가리키는 값이 __vArray 에서 Unique 한 것은 아니다!
	//	TO DO LIST: __vArray 를 Unique 한 Vertex 들의 배열로 만들고,
	//	그 안에서 index 를 찾을 수도 있다;
	//	DirectX 의 MeshFromOBJ 예제의 CMeshLoader::__AddVertex 처럼.
	//	하지만 SetPolygon 에서 GenerateAdjacency, ConvertAdjacencyToPointReps 를 이용하여
	//	이 작업을 하므로 굳이 할 필요가 없다.
	// Vertex Buffer 를 만들 때, 주어진 Vertex 에서 (POSITION, NORMAL, TEXTURE) 를 설정해야 한다.
	// 그런데 __vArray, vnArray, __vtArray 의 갯수가 다르다.
	// 이를 어찌 할 거나...
	// (물론 __vArray 처럼 vnArray 나 __vtArray 는 겹치는 부분이 있긴 하겠지만
	// 그걸 빼고 Unique 한 값들의 갯수가 같다는 보장이 있나???)

#if 1	// Voxel
	__setVoxelBuffer(devicePtr, voxels);
#endif	// Voxel
	HRESULT	hResult = D3D_OK;
#if 1
	DWORD	dwOptions = D3DPOOL_MANAGED | D3DXMESH_32BIT;
	hResult = D3DXCreateMesh(
		__Indices.GetSize() / 3, __Vertices.GetSize(),
		dwOptions,
		VERTEX_DECL,
		devicePtr,
		&__meshPtr
	);
	assert(hResult == D3D_OK);

	// Copy the vertex data
	CPointRepr::VERTEX*	pVertex;
	__meshPtr->LockVertexBuffer(0, (void**) &pVertex);
	memcpy(pVertex, __Vertices.GetData(), __Vertices.GetSize() * sizeof(CPointRepr::VERTEX));
	__meshPtr->UnlockVertexBuffer();
	__Vertices.RemoveAll();

	// Copy the index data
	DWORD*	pIndex;
	__meshPtr->LockIndexBuffer(0, (void**) &pIndex);
#if 1	// 에트리 모델; Right-handed (Cartesian) Coordinate System
	// DirectX 에서는 삼각형을 이루는 꼭지점의 인덱스가 반대이다.
	for (int n = 0; n < __Indices.GetSize() / 3; ++n) {
		pIndex[3 * n + 0] = __Indices.GetAt(3 * n + 0);
		pIndex[3 * n + 1] = __Indices.GetAt(3 * n + 2);
		pIndex[3 * n + 2] = __Indices.GetAt(3 * n + 1);
	}
#else
	memcpy(pIndex, __Indices.GetData(), __Indices.GetSize() * sizeof(DWORD));
#endif
	__meshPtr->UnlockIndexBuffer();
	__Indices.RemoveAll();

	// Copy the attribute data
	DWORD*	pSubset;
	__meshPtr->LockAttributeBuffer(0, &pSubset);
	memcpy(pSubset, __Attributes.GetData(), __Attributes.GetSize() * sizeof(DWORD));
	__meshPtr->UnlockAttributeBuffer();
	__Attributes.RemoveAll();

#if 0
	// Reorder the vertices according to subset and optimize the mesh for this graphics
	// card's vertex cache. When rendering the mesh's triangle list the vertices will
	// cache hit more often so it won't have to re-execute the vertex shader.
	DWORD*	aAdjacency = new DWORD[__meshPtr->GetNumFaces() * 3];
	if (aAdjacency == NULL)
		return NULL;

	__meshPtr->GenerateAdjacency(1e-6f, aAdjacency);
	__meshPtr->OptimizeInplace(D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_VERTEXCACHE, aAdjacency, NULL, NULL, NULL);

	if (aAdjacency) {
		delete[] aAdjacency;
		aAdjacency = 0;
	}
#endif
#endif

	// Save
#if 1
#if 0
	DWORD	dwFormat = D3DXF_FILEFORMAT_TEXT;	// D3DXF_FILEFORMAT_BINARY
#endif
#if 1	// Material
#ifdef _DEBUG
	for (int n = 0; n < __Materials.GetSize(); ++n)
		(void) fwprintf(stdout, L"Material[%d]:= |%s|\n", n, __Materials.GetAt(n)->strName);
#endif
#if 1	// w/ fake (rainbow) colors
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

#if 1
	// Reorder the vertices according to subset and optimize the mesh for this graphics
	// card's vertex cache. When rendering the mesh's triangle list the vertices will
	// cache hit more often so it won't have to re-execute the vertex shader.
	DWORD*	faceRemap = 0;
	DWORD*	aAdjacency = 0;
#if 1	// DrawSubset
#else
	LPD3DXMESH	meshPtr = 0;
#endif
	LPD3DXBUFFER	vertexRemapBufferPtr = 0;
do {
	faceRemap = new DWORD[__meshPtr->GetNumFaces()];
	aAdjacency = new DWORD[__meshPtr->GetNumFaces() * 3];
	if (aAdjacency == NULL)
		break;
	__meshPtr->GenerateAdjacency(1e-6f, aAdjacency);
	__meshPtr->Optimize(
		D3DXMESHOPT_ATTRSORT,
		aAdjacency,
		NULL,
		faceRemap,
		&vertexRemapBufferPtr,	// Optimize 를 사용하는 한,
					// 그리고 __meshPtr 과 __meshSubsetPtr 을 따로 관리하는 한,
					// faceRemap, vertexRemapBufferPtr 는 사용되지 않는다.
					//	- 070809 (EST)
					//	- 김광호
#if 1	// DrawSubset
		&__meshSubsetPtr
#else
		&meshPtr
#endif
	);
	// faceRemap[newFaceIndex] = oldFaceIndex
//	DWORD*	vertexRemapPtr = (DWORD*) vertexRemapBufferPtr->GetBufferPointer();
#if 1	// 에트리 모델: Right-handed (Cartesian) Coordinate System; 텍스쳐 사용
	if (texturePath) {
		WCHAR		texturePathname[MAX_PATH];
		for (int iMaterial = 0; iMaterial < __Materials.GetSize(); ++iMaterial) {
			Material*	pMaterial = __Materials.GetAt(iMaterial);

			(void) fwprintf(stdout, L"Material[%d]: Texture:= |%s|\n", iMaterial, pMaterial->strTexture);
			if (pMaterial->strTexture[0] == L'0') {	// 텍스쳐 파일이 설정되지 않았다
				continue;
			}

			swprintf_s(texturePathname, MAX_PATH, L"%s/%s", texturePath, pMaterial->strTexture);
			HRESULT	hr = D3DXCreateTextureFromFile(devicePtr, texturePathname, &pMaterial->texturePtr);
#ifdef _DEBUG
			assert(hr == D3D_OK);
#else
			if (hr != D3D_OK)
				throw "kMeshLoader::Create: D3DXCreateTextureFromFile: Failure";
#endif
		}
	}
#endif

#if 0	// __meshSubsetPtr 에 설정된 subset 정보를,
	// __meshPtr 에도 설정하겠다는 얘기인데...
	// __meshPtr 에서 설정한 subset 정보를 사용하여,
	// Optimize 를 사용하여 subset 정보가 설정된 (GetAttributeTable 를 통해)
	// __meshSubsetPtr 을 얻었는 데, 도대체 거꾸로 무얼하겠다는 건지???
	//	- 070809 (EST)
	//	- 김광호
#if 1	// DrawSubset
	LPD3DXMESH	meshPtr = __meshSubsetPtr;
#endif
	DWORD	attributeTableSize;
	meshPtr->GetAttributeTable(NULL, &attributeTableSize);
	D3DXATTRIBUTERANGE*	attributeTable = new D3DXATTRIBUTERANGE[attributeTableSize];
do {
	meshPtr->GetAttributeTable(attributeTable, &attributeTableSize);
	DWORD*	pdwAttributeOut = NULL;
	assert(SUCCEEDED(__meshPtr->LockAttributeBuffer(D3DLOCK_NOSYSLOCK, &pdwAttributeOut)));
	if (!pdwAttributeOut) {
		// TO DO LIST: Why???
		(void) fprintf(stderr, "%p\n", pdwAttributeOut);
		break;
	}

	for (DWORD iOptimizedFace = 0; iOptimizedFace < meshPtr->GetNumFaces(); ++iOptimizedFace) {
		// iOptimizedFace 번째 Face 는 몇 번째 Subset 인가?
		int	iSubset = -1;
		for (DWORD dwI = 0; dwI < attributeTableSize; ++dwI) {
			const D3DXATTRIBUTERANGE&	table = attributeTable[dwI];
			if (table.FaceStart <= iOptimizedFace && iOptimizedFace < table.FaceStart + table.FaceCount) {
				iSubset = table.AttribId;
				break;
			}
		}
		if (iSubset == -1) {
			(void) fprintf(stderr, "Face ID = %d: No subset\n", iOptimizedFace);
			continue;
		}
		DWORD	iUnOptimizedFace = faceRemap[iOptimizedFace];
		if (iUnOptimizedFace != iOptimizedFace) {
			int i = 0;
		}
		pdwAttributeOut[iUnOptimizedFace] = iSubset % COLOR_COUNT;	// !!
	}
	assert(SUCCEEDED(__meshPtr->UnlockAttributeBuffer()));
} while (0);
	if (attributeTable) {
		delete[] attributeTable;
		attributeTable = 0;
	}
#endif
} while (0);
	if (vertexRemapBufferPtr) {
		vertexRemapBufferPtr->Release();
		vertexRemapBufferPtr = 0;
	}
	if (faceRemap) {
		delete[] faceRemap;
		faceRemap = 0;
	}
	if (aAdjacency) {
		delete[] aAdjacency;
		aAdjacency = 0;
	}
#if 1	// DrawSubset
#else
	if (meshPtr) {
		meshPtr->Release();
		meshPtr = 0;
	}
#endif
#else	// OptimizeInplace 를 했을 경우
#if 0
	// Reorder the vertices according to subset and optimize the mesh for this graphics
	// card's vertex cache. When rendering the mesh's triangle list the vertices will
	// cache hit more often so it won't have to re-execute the vertex shader.
	DWORD*	aAdjacency = new DWORD[__meshPtr->GetNumFaces() * 3];
	if (aAdjacency == NULL)
		return NULL;

	__meshPtr->GenerateAdjacency(1e-6f, aAdjacency);
	__meshPtr->OptimizeInplace(D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_VERTEXCACHE, aAdjacency, NULL, NULL, NULL);

	if (aAdjacency) {
		delete[] aAdjacency;
		aAdjacency = 0;
	}
#endif
#if 0	// __meshSubsetPtr 에 설정된 subset 정보를,
	// __meshPtr 에도 설정하겠다는 얘기인데...
	// __meshPtr 에서 설정한 subset 정보를 사용하여,
	// Optimize 를 사용하여 subset 정보가 설정된 (GetAttributeTable 를 통해)
	// __meshSubsetPtr 을 얻었는 데, 도대체 거꾸로 무얼하겠다는 건지???
	DWORD	attributeTableSize;
	__meshPtr->GetAttributeTable(NULL, &attributeTableSize);
	D3DXATTRIBUTERANGE*	attributeTable = new D3DXATTRIBUTERANGE[attributeTableSize];
do {
	__meshPtr->GetAttributeTable(attributeTable, &attributeTableSize);
	DWORD*	pdwAttributeOut = NULL;
	assert(SUCCEEDED(__meshPtr->LockAttributeBuffer(D3DLOCK_NOSYSLOCK, &pdwAttributeOut)));
	for (DWORD i = 0; i < __meshPtr->GetNumFaces(); ++i) {
		// i 번째 Face 는 몇 번째 Subset 인가?
		int	iSubset = -1;
		for (DWORD dwI = 0; dwI < attributeTableSize; ++dwI) {
			const D3DXATTRIBUTERANGE&	table = attributeTable[dwI];
			if (table.FaceStart <= i && i < table.FaceStart + table.FaceCount) {
				iSubset = table.AttribId;
				break;
			}
		}
		if (iSubset == -1) {
			(void) fprintf(stderr, "Face ID = %d: No subset\n", i);
			continue;
		}
		pdwAttributeOut[i] = iSubset % COLOR_COUNT;
	}
	assert(SUCCEEDED(__meshPtr->UnlockAttributeBuffer()));
} while (0);
	if (attributeTable) {
		delete[] attributeTable;
		attributeTable = 0;
	}
#endif
#endif
}
#if 0	// .obj 로부터 만든 메쉬를 .x 로 쓴다.
	assert(D3D_OK == D3DXSaveMeshToX(
		TEXT("meshFromOBJ.x"),
		__meshPtr,
		NULL,
		GeneratedMaterials,
		NULL,
		COLOR_COUNT,
		dwFormat
	));
#endif
}
#else	// real colors in file
{
	// Fill out D3DXMATERIAL structures
	UINT	numMaterials = __Materials.GetSize();
	D3DXMATERIAL*	pMaterials = new D3DXMATERIAL[numMaterials];
	char*	pStrTexture = new char[MAX_PATH * numMaterials];
	if (pMaterials != NULL && pStrTexture != NULL) {
		for (UINT i = 0; i < numMaterials; ++i) {
			Material*	pMat = __Materials.GetAt(i);
			if (!pMat)
				continue;

			pMaterials[i].MatD3D.Ambient.r = pMat->vAmbient.x;
			pMaterials[i].MatD3D.Ambient.g = pMat->vAmbient.y;
			pMaterials[i].MatD3D.Ambient.b = pMat->vAmbient.z;
			pMaterials[i].MatD3D.Ambient.a = pMat->fAlpha;

#if 1
			if (i % 3 == 2) {
				pMaterials[i].MatD3D.Diffuse.r = 200;
				pMaterials[i].MatD3D.Diffuse.g = pMat->vDiffuse.y;
				pMaterials[i].MatD3D.Diffuse.b = pMat->vDiffuse.z;
			} else if (i % 3 == 1) {
				pMaterials[i].MatD3D.Diffuse.r = pMat->vDiffuse.x;
				pMaterials[i].MatD3D.Diffuse.g = 200;
				pMaterials[i].MatD3D.Diffuse.b = pMat->vDiffuse.z;
			} else {
				pMaterials[i].MatD3D.Diffuse.r = pMat->vDiffuse.x;
				pMaterials[i].MatD3D.Diffuse.g = pMat->vDiffuse.y;
				pMaterials[i].MatD3D.Diffuse.b = 200;
			}
			pMaterials[i].MatD3D.Diffuse.a = pMat->fAlpha;
#else
			pMaterials[i].MatD3D.Diffuse.r = pMat->vDiffuse.x;
			pMaterials[i].MatD3D.Diffuse.g = pMat->vDiffuse.y;
			pMaterials[i].MatD3D.Diffuse.b = pMat->vDiffuse.z;
			pMaterials[i].MatD3D.Diffuse.a = pMat->fAlpha;
#endif

			pMaterials[i].MatD3D.Specular.r = pMat->vSpecular.x;
			pMaterials[i].MatD3D.Specular.g = pMat->vSpecular.y;
			pMaterials[i].MatD3D.Specular.b = pMat->vSpecular.z;
			pMaterials[i].MatD3D.Specular.a = pMat->fAlpha;

			pMaterials[i].MatD3D.Emissive.r = 0;
			pMaterials[i].MatD3D.Emissive.g = 0;
			pMaterials[i].MatD3D.Emissive.b = 0;
			pMaterials[i].MatD3D.Emissive.a = 0;

			pMaterials[i].MatD3D.Power = (float) pMat->nShininess;
			WideCharToMultiByte(CP_ACP, 0, pMat->strTexture, -1, pStrTexture + i * MAX_PATH, MAX_PATH, NULL, NULL);
			pMaterials[i].pTextureFilename = pStrTexture + i * MAX_PATH;
		}
	}
	hResult = D3DXSaveMeshToX(
		TEXT("meshFromOBJ.x"),
		__meshPtr,
		NULL,//pAdjacency,
		pMaterials,
		NULL,
		numMaterials,
		dwFormat
	);
	assert(D3D_OK == hResult);
	SAFE_DELETE_ARRAY(pMaterials);
	SAFE_DELETE_ARRAY(pStrTexture);
}
#endif
#else
	hResult = D3DXSaveMeshToX(
		TEXT("meshFromOBJ.x"),
		__meshPtr,
		NULL,//pAdjacency,
		NULL,
		NULL,
		0,
		dwFormat
	);
	assert(D3D_OK == hResult);
#endif
#endif
	return hr;
}
#if 1	// Voxel
void	kMeshLoader::__setVoxelBuffer(IDirect3DDevice9* devicePtr, const CGrowableArray<VOXEL>& voxels)
{
	SAFE_RELEASE(__voxelVertexDeclarationPtr);
	SAFE_RELEASE(__voxelVertexBufferPtr);
	SAFE_RELEASE(__voxelIndexBufferPtr);
	__num_voxels = voxels.GetSize();
	if (!__num_voxels)
		return;
//	cout << "# of voxels = " << num_voxels << endl;
//	cout << "# of voxels0 = " << __meshLoader.GetVoxels().GetSize() << endl;
//	UINT	num_voxels = 1;
	UINT	num_triangles_per_voxel =
		6 /* # of quad. faces per voxel */ *
		2 /* # of triangles per quad. */;
#if 1	// Index Buffer
	UINT	num_vertices_per_voxel = 8;	// # of vertex per voxel
#else	// Index Buffer/!Index Buffer
	UINT	num_vertices_per_voxel = num_triangles_per_voxel * 3 /* # of vertices per triangle */;
#endif	// !Index Buffer

	HRESULT	hr;
D3DVERTEXELEMENT9	Element[3 + 1] = {	// VOXEL_VERTEX
	{	// p
		0,
		0 * sizeof(D3DXVECTOR3),
		D3DDECLTYPE_FLOAT3,
		D3DDECLMETHOD_DEFAULT,
		D3DDECLUSAGE_POSITION,
		0
	},
	{	// n
		0,
		1 * sizeof(D3DXVECTOR3),
		D3DDECLTYPE_FLOAT3,
		D3DDECLMETHOD_DEFAULT,
		D3DDECLUSAGE_NORMAL,
		0
	},
	{	// c
		0,
		2 * sizeof(D3DXVECTOR3),
		D3DDECLTYPE_FLOAT3,
		D3DDECLMETHOD_DEFAULT,
		D3DDECLUSAGE_COLOR,
		0
	},

	D3DDECL_END()
};
	hr = devicePtr->CreateVertexDeclaration(Element, &__voxelVertexDeclarationPtr);
	assert(hr == D3D_OK);

	hr = devicePtr->CreateVertexBuffer(
		__num_voxels * num_vertices_per_voxel * sizeof(VOXEL_VERTEX),
		D3DUSAGE_WRITEONLY,
		0,
//		D3DPOOL_DEFAULT,//D3DPOOL_MANAGED,
		D3DPOOL_MANAGED,
		&__voxelVertexBufferPtr,
		NULL
	);
	assert(D3D_OK == hr);
#if 1	// Index Buffer
	devicePtr->CreateIndexBuffer(
		__num_voxels * num_triangles_per_voxel * sizeof(CFace::FaceIndex),
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX32,
		D3DPOOL_MANAGED,
		&__voxelIndexBufferPtr,
		NULL
	);
#endif	// Index Buffer

	VOXEL_VERTEX*	voxelVertexArray;
	__voxelVertexBufferPtr->Lock(0, 0, (void**) &voxelVertexArray, 0);
#if 1	// Index Buffer
	CFace::FaceIndex*	pFaceIndices;
	__voxelIndexBufferPtr->Lock(0, 0, (void**) &pFaceIndices, 0);
#endif	// Index Buffer
//	int	index = 0;
		//vx 2.500000 423.750000 883.750000 113.750000 150 104 101
for (int index = 0; index < (int) __num_voxels; ++index) {
//	const VOXEL&	voxel = voxels[index];
	const kMeshLoader::VOXEL&	voxel = voxels.GetAt(index);
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

		n0: (-1, 1, -1)
		n1: (1, 1, -1)
		n2: (1, -1, -1)
		n3: (-1, -1, -1)
		n4: (-1, 1, 1)
		n5: (1, 1, 1)
		n6: (1, -1, 1)
		n7: (-1, -1, 1)

	front:
	       0+----+1
		|f0 /|
		|  / |	f0: 013
		| /  |	f1: 231
		|/ f1|
	       3+----+2
	back:
	       5+----+4
		|f2 /|
		|  / |	f2: 546
		| /  |	f3: 764
		|/ f3|
	       6+----+7
	right:
	       1+----+5
		|f4 /|
		|  / |	f4: 152
		| /  |	f5: 625
		|/ f5|
	       2+----+6
	left:
	       4+----+0
		|f6 /|
		|  / |	f6: 407
		| /  |	f7: 370
		|/ f7|
	       7+----+3
	top:
	       4+----+5
		|f8 /|
		|  / |	f8: 450
		| /  |	f9: 105
		|/ f9|
	       0+----+1
	bottom:
	       3+----+2
		|fa /|
		|  / |	fa: 327
		| /  |	fb: 672
		|/ fb|
	       7+----+6
 */
//		UINT	R = 150, G = 104, B = 101;
	const UINT&	R = voxel.R;
	const UINT&	G = voxel.G;
	const UINT&	B = voxel.B;
		D3DXVECTOR3	voxel_color((float) R / 255.f, (float) G / 255.f, (float) B / 255.f);
//	voxel_color = D3DXVECTOR3(169.f / 255, 169.f / 255, 169.f / 255);
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
#if 1	// Index Buffer
	voxelVertexArray[index * num_vertices_per_voxel + 0].p = v0;
	voxelVertexArray[index * num_vertices_per_voxel + 1].p = v1;
	voxelVertexArray[index * num_vertices_per_voxel + 2].p = v2;
	voxelVertexArray[index * num_vertices_per_voxel + 3].p = v3;
	voxelVertexArray[index * num_vertices_per_voxel + 4].p = v4;
	voxelVertexArray[index * num_vertices_per_voxel + 5].p = v5;
	voxelVertexArray[index * num_vertices_per_voxel + 6].p = v6;
	voxelVertexArray[index * num_vertices_per_voxel + 7].p = v7;
/*
		n0: (-1, 1, -1)
		n1: (1, 1, -1)
		n2: (1, -1, -1)
		n3: (-1, -1, -1)
		n4: (-1, 1, 1)
		n5: (1, 1, 1)
		n6: (1, -1, 1)
		n7: (-1, -1, 1)
 */
	D3DXVECTOR3	N0(1, 1, 1);
	N0 /= sqrtf(3.f);	// normalize
	voxelVertexArray[index * num_vertices_per_voxel + 0].n = D3DXVECTOR3(-N0.x, N0.y, -N0.z);
	voxelVertexArray[index * num_vertices_per_voxel + 1].n = D3DXVECTOR3(N0.x, N0.y, -N0.z);
	voxelVertexArray[index * num_vertices_per_voxel + 2].n = D3DXVECTOR3(N0.x, -N0.y, -N0.z);
	voxelVertexArray[index * num_vertices_per_voxel + 3].n = D3DXVECTOR3(-N0.x, -N0.y, -N0.z);
	voxelVertexArray[index * num_vertices_per_voxel + 4].n = D3DXVECTOR3(-N0.x, N0.y, N0.z);
	voxelVertexArray[index * num_vertices_per_voxel + 5].n = D3DXVECTOR3(N0.x, N0.y, N0.z);
	voxelVertexArray[index * num_vertices_per_voxel + 6].n = D3DXVECTOR3(N0.x, -N0.y, N0.z);
	voxelVertexArray[index * num_vertices_per_voxel + 7].n = D3DXVECTOR3(-N0.x, -N0.y, N0.z);
/*
 * front:
		|  / |	f0: 013
		| /  |	f1: 231
 * back:
		|  / |	f2: 546
		| /  |	f3: 764
 * right:
		|  / |	f4: 152
		| /  |	f5: 625
 * left:
		|  / |	f6: 407
		| /  |	f7: 370
 * top:
		|  / |	f8: 450
		| /  |	f9: 105
 * bottom:
		|  / |	fa: 327
		| /  |	fb: 672
 */
	// 앞면(front; f0[013], f1[231])
	pFaceIndices[index * num_triangles_per_voxel + 0].i0 = index * num_vertices_per_voxel + 0;
	pFaceIndices[index * num_triangles_per_voxel + 0].i1 = index * num_vertices_per_voxel + 1;
	pFaceIndices[index * num_triangles_per_voxel + 0].i2 = index * num_vertices_per_voxel + 3;
	pFaceIndices[index * num_triangles_per_voxel + 1].i0 = index * num_vertices_per_voxel + 2;
	pFaceIndices[index * num_triangles_per_voxel + 1].i1 = index * num_vertices_per_voxel + 3;
	pFaceIndices[index * num_triangles_per_voxel + 1].i2 = index * num_vertices_per_voxel + 1;
	// 뒷면(back; f2[546], f3[764])
	pFaceIndices[index * num_triangles_per_voxel + 2].i0 = index * num_vertices_per_voxel + 5;
	pFaceIndices[index * num_triangles_per_voxel + 2].i1 = index * num_vertices_per_voxel + 4;
	pFaceIndices[index * num_triangles_per_voxel + 2].i2 = index * num_vertices_per_voxel + 6;
	pFaceIndices[index * num_triangles_per_voxel + 3].i0 = index * num_vertices_per_voxel + 7;
	pFaceIndices[index * num_triangles_per_voxel + 3].i1 = index * num_vertices_per_voxel + 6;
	pFaceIndices[index * num_triangles_per_voxel + 3].i2 = index * num_vertices_per_voxel + 4;
	// 우측면(right; f4[152], f5[625])
	pFaceIndices[index * num_triangles_per_voxel + 4].i0 = index * num_vertices_per_voxel + 1;
	pFaceIndices[index * num_triangles_per_voxel + 4].i1 = index * num_vertices_per_voxel + 5;
	pFaceIndices[index * num_triangles_per_voxel + 4].i2 = index * num_vertices_per_voxel + 2;
	pFaceIndices[index * num_triangles_per_voxel + 5].i0 = index * num_vertices_per_voxel + 6;
	pFaceIndices[index * num_triangles_per_voxel + 5].i1 = index * num_vertices_per_voxel + 2;
	pFaceIndices[index * num_triangles_per_voxel + 5].i2 = index * num_vertices_per_voxel + 5;
	// 좌측면(left; f6[407], f7[370])
	pFaceIndices[index * num_triangles_per_voxel + 6].i0 = index * num_vertices_per_voxel + 4;
	pFaceIndices[index * num_triangles_per_voxel + 6].i1 = index * num_vertices_per_voxel + 0;
	pFaceIndices[index * num_triangles_per_voxel + 6].i2 = index * num_vertices_per_voxel + 7;
	pFaceIndices[index * num_triangles_per_voxel + 7].i0 = index * num_vertices_per_voxel + 3;
	pFaceIndices[index * num_triangles_per_voxel + 7].i1 = index * num_vertices_per_voxel + 7;
	pFaceIndices[index * num_triangles_per_voxel + 7].i2 = index * num_vertices_per_voxel + 0;
	// 윗면(top; f8[450], f9[105])
	pFaceIndices[index * num_triangles_per_voxel + 8].i0 = index * num_vertices_per_voxel + 4;
	pFaceIndices[index * num_triangles_per_voxel + 8].i1 = index * num_vertices_per_voxel + 5;
	pFaceIndices[index * num_triangles_per_voxel + 8].i2 = index * num_vertices_per_voxel + 0;
	pFaceIndices[index * num_triangles_per_voxel + 9].i0 = index * num_vertices_per_voxel + 1;
	pFaceIndices[index * num_triangles_per_voxel + 9].i1 = index * num_vertices_per_voxel + 0;
	pFaceIndices[index * num_triangles_per_voxel + 9].i2 = index * num_vertices_per_voxel + 5;
	// 아래면(bottom; f10[327], f11[672])
	pFaceIndices[index * num_triangles_per_voxel + 10].i0 = index * num_vertices_per_voxel + 3;
	pFaceIndices[index * num_triangles_per_voxel + 10].i1 = index * num_vertices_per_voxel + 2;
	pFaceIndices[index * num_triangles_per_voxel + 10].i2 = index * num_vertices_per_voxel + 7;
	pFaceIndices[index * num_triangles_per_voxel + 11].i0 = index * num_vertices_per_voxel + 6;
	pFaceIndices[index * num_triangles_per_voxel + 11].i1 = index * num_vertices_per_voxel + 7;
	pFaceIndices[index * num_triangles_per_voxel + 11].i2 = index * num_vertices_per_voxel + 2;
#if 0
	for (int k = 0; k < 12; ++k) {
		pFaceIndices[index * num_triangles_per_voxel + k].i0 *= N_ELEMENT;
		pFaceIndices[index * num_triangles_per_voxel + k].i1 *= N_ELEMENT;
		pFaceIndices[index * num_triangles_per_voxel + k].i2 *= N_ELEMENT;
	}
#endif
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
	__voxelIndexBufferPtr->Unlock();
#endif	// Index Buffer
	__voxelVertexBufferPtr->Unlock();
}
#endif	// Voxel
