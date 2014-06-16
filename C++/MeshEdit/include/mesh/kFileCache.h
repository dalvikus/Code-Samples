#ifndef __kFILE_CACHE__
#define __kFILE_CACHE__


#include <mesh/kPolygon.h>
#ifdef __READ_FILES__
struct kFileCache
{
	static const MESH_LOADER_READ_FILES*	__pointer_to_MESH_LOADER_READ_FILES;
	static const unsigned			__numEntries_in_MESH_LOADER_READ_FILES;
	static const MESH_READ_FILES*		__pointer_to_MESH_READ_FILES;
	static const unsigned			__numEntries_in_MESH_READ_FILES;
};
#endif	// __READ_FILES__


#endif	// !__kFILE_CACHE__
