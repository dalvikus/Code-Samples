#include <mesh/kFileCache.h>
#ifdef __READ_FILES__

#ifndef __KEKO__
////////////////////////////////////////////////////////////////////////////////
#ifdef MULTI_STANDARD
#if 1	// MESH_LOADER_READ_FILES
#include <mesh/__Indices_M__>			// INDICES__FILENAME
#include <mesh/__VertexBufferIndexByF_M__>	// VERTEXBUFFERINDEXBYF__FILENAME
#include <mesh/__Vertices_M__>			// VERTICES__FILENAME
#include <mesh/__Attributes_M__>		// ATTRIBUTES__FILENAME
#include <mesh/__Textures_M__>			// TEXTURES__FILENAME

#include <mesh/__meshLoader_READ_FILES_M__.h>
#endif	// MESH_LOADER_READ_FILES
#if 1	// MESH_READ_FILES
#include <mesh/__adjacencyPtr_M__>		// ADJACENCY_PTR__FILENAME
#include <mesh/__vertexID2pointReprIDMap_M__>	// VERTEXID2POINTREPRIDMAP__FILENAME
#include <mesh/__pointReprPtr_M__>		// POINTREPR_PTR__FILENAME
#include <mesh/__edgePtr_M__>			// EDGE_PTR__FILENAME
#include <mesh/__facePtr_M__>			// FACE_PTR__FILENAME

#include <mesh/__mesh_READ_FILES_M__.h>
#endif	// MESH_READ_FILES
#else	// MULTI_STANDARD/!MULTI_STANDARD
#if 1	// MESH_LOADER_READ_FILES
#include <mesh/__Indices__>			// INDICES__FILENAME
#include <mesh/__VertexBufferIndexByF__>	// VERTEXBUFFERINDEXBYF__FILENAME
#include <mesh/__Vertices__>			// VERTICES__FILENAME
#include <mesh/__Attributes__>			// ATTRIBUTES__FILENAME
#include <mesh/__Textures__>			// TEXTURES__FILENAME
#endif	// MESH_LOADER_READ_FILES
#if 1	// MESH_READ_FILES
#include <mesh/__adjacencyPtr__>		// ADJACENCY_PTR__FILENAME
#include <mesh/__vertexID2pointReprIDMap__>	// VERTEXID2POINTREPRIDMAP__FILENAME
#include <mesh/__pointReprPtr__>		// POINTREPR_PTR__FILENAME
#include <mesh/__edgePtr__>			// EDGE_PTR__FILENAME
#include <mesh/__facePtr__>			// FICE_PTR__FILENAME
#endif	// MESH_READ_FILES
// MESH_READ_FILES
#endif	// !MULTI_STANDARD
////////////////////////////////////////////////////////////////////////////////
#else	// !__KEKO__
#if 1	// MESH_LOADER_READ_FILES
#include <mesh/__Indices_KEKO__>			// INDICES__FILENAME
#include <mesh/__VertexBufferIndexByF_KEKO__>	// VERTEXBUFFERINDEXBYF__FILENAME
#include <mesh/__Vertices_KEKO__>			// VERTICES__FILENAME
#include <mesh/__Attributes_KEKO__>		// ATTRIBUTES__FILENAME
#include <mesh/__Textures_KEKO__>			// TEXTURES__FILENAME

#include <mesh/__meshLoader_READ_FILES_M__.h>
#endif	// MESH_LOADER_READ_FILES
#if 1	// MESH_READ_FILES
#include <mesh/__adjacencyPtr_KEKO__>		// ADJACENCY_PTR__FILENAME
#include <mesh/__vertexID2pointReprIDMap_KEKO__>	// VERTEXID2POINTREPRIDMAP__FILENAME
#include <mesh/__pointReprPtr_KEKO__>		// POINTREPR_PTR__FILENAME
#include <mesh/__edgePtr_KEKO__>			// EDGE_PTR__FILENAME
#include <mesh/__facePtr_KEKO__>			// FACE_PTR__FILENAME

#include <mesh/__mesh_READ_FILES_M__.h>
#endif	// MESH_READ_FILES
#endif	// __KEKO__

const MESH_LOADER_READ_FILES*	kFileCache::__pointer_to_MESH_LOADER_READ_FILES =
	__meshLoader_READ_FILES__;
const unsigned			kFileCache::__numEntries_in_MESH_LOADER_READ_FILES =
	sizeof(__meshLoader_READ_FILES__) / sizeof(MESH_LOADER_READ_FILES);
const MESH_READ_FILES*		kFileCache::__pointer_to_MESH_READ_FILES =
	__mesh_READ_FILES__;
const unsigned			kFileCache::__numEntries_in_MESH_READ_FILES =
	sizeof(__mesh_READ_FILES__) / sizeof(MESH_READ_FILES);


#endif	// __READ_FILES__
