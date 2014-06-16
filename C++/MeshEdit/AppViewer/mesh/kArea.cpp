#include <mesh/kArea.h>
#include <mesh/kMesh.h>


#if 1
kArea::kArea() : __colorMtrl(1, 1, 1, 1), __typeVal(0), __allSetVal(0)
#else
kArea::kArea() : __typeVal(0)
#endif
{
	for (int i = 0; i < NUM_AREA_TYPE; ++i)
		__colorArray[i] = -1.f;	// 기본값: 미리 정의된 색깔을 사용한다.
}


int	kArea::getValue(int subType, void* dataPtr)
{
	if (!dataPtr) {
		(void) fprintf(stderr, "Fatal: kArea::getValue: dataPtr is nil\n");
		return -1;
	}

	switch (subType) {
	case AREA__TYPE:
	{
		AREA_TYPE_VISIBLE&	typeVisible = *((AREA_TYPE_VISIBLE*) dataPtr);
		typeVisible.visible = __typeVal & typeVisible.type ? true : false;
		return 0;
	}
		break;
	case AREA__COLOR:
	{
		AREA_TYPE_COLOR&	areaTypeColor = *((AREA_TYPE_COLOR*) dataPtr);
		int	colorIndex = getColorIndex(areaTypeColor.type);
		if (colorIndex == -1) {
			(void) fprintf(stderr, "FATAL: kArea::getValue: getColorIndex(%d) returns -1\n", areaTypeColor.type);
			areaTypeColor.fColor = -1.f;
		} else
			areaTypeColor.fColor = __colorArray[colorIndex];
	}
		return 0;
		break;
	default:
		(void) fprintf(stderr, "Error: kArea::getValue: AREA__TYPE = %d: Unknown Type\n", subType);
		break;
	}
	return -1;
}

int	kArea::setValue(int subType, const void* dataPtr)
{
	if (!dataPtr) {
		(void) fprintf(stderr, "Fatal: kArea::getValue: dataPtr is nil\n");
		return -1;
	}

	switch (subType) {
	case AREA__TYPE:
	{
		AREA_TYPE_VISIBLE&	typeVisible = *((AREA_TYPE_VISIBLE*) dataPtr);
		if (typeVisible.visible)
			__typeVal |= typeVisible.type;
		else
			__typeVal &= ~typeVisible.type;
	}
		return 0;
		break;
	case AREA__COLOR:
	{
		AREA_TYPE_COLOR&	areaTypeColor = *((AREA_TYPE_COLOR*) dataPtr);
		int	colorIndex = getColorIndex(areaTypeColor.type);
		if (colorIndex == -1) {
			(void) fprintf(stderr, "FATAL: kArea::getValue: getColorIndex(%d) returns -1\n", areaTypeColor.type);
		} else
			__colorArray[colorIndex] = areaTypeColor.fColor;
	}
		return 0;
		break;
	default:
		(void) fprintf(stderr, "Error: kArea::setValue: AREA__TYPE = %d: Unknown Type\n", subType);
		break;
	}
	return -1;
}

int	kArea::getColorIndex(AREA_TYPE_T areaType)
{
	int	iAreaType = (int) areaType;
	if (iAreaType < 0)
		return -1;

	int	index = 0;
	while (iAreaType >>= 1)
		++index;

	return index >= NUM_AREA_TYPE ? -1 : index;
}

int	kArea::setColor(const kMesh& mesh)
{
	if (__typeVal == __allSetVal)
		return 0;

	const CFace*	faceList = mesh.GetFacePointer();
	DWORD		numFaces = mesh.GetNumFaces();
#define COLOR_COUNT	8
	FLOAT	ColorList3[COLOR_COUNT][3] = {
		{1.0f, 0.5f, 0.5f},
		{0.5f, 1.0f, 0.5f},
		{1.0f, 1.0f, 0.5f},
		{0.5f, 1.0f, 1.0f},
		{1.0f, 0.5f, .75f},
		{0.0f, 0.5f, .75f},
		{0.5f, 0.5f, .75f},
		{0.5f, 0.5f, 1.0f},
	};

	kMesh::D3DXVERTEX*	vertexArray;
	mesh.GetVertexBufferPointer()->Lock(0, 0, (void**) &vertexArray, 0);
#if 0
	for (int n = 0; n < (int) mesh.GetNumPointReprs(); ++n)
		vertexArray[n].c = D3DXVECTOR3(0, 0, 0);
#endif
	int	index = 0;
	DEFINED_AREA_DIC::const_iterator	area_it;
	(void) fprintf(stdout, "# of definition in Area = %d\n", mesh.defined_area_dic().size());
	for (area_it = mesh.defined_area_dic().begin(); area_it != mesh.defined_area_dic().end(); ++area_it, ++index) {
		AREA_TYPE_T	areaType = area_it->first;
#if 1
		if (!(__typeVal & areaType)) {
			if (!(__allSetVal & areaType))	// 이미 원래 색깔로 돌아왔다.
				continue;
			__allSetVal &= ~areaType;

			const DEFINED_AREA&	defArea = area_it->second;
			// 2. 해당 영역을 원래 색깔로 되돌린다.
			const std::set<DWORD>&	face_id_set = defArea.face_id_set;
			for (std::set<DWORD>::const_iterator it = face_id_set.begin(); it != face_id_set.end(); ++it) {
#if 1	// FACE_ID_LOCAL_INDEX
				DWORD	faceID = *it;
				if (faceID >= numFaces) {
					(void) fprintf(stderr, "FATAL: kArea::setColor: Face ID = %d: Out of range: Ignored\n", faceID);
					continue;
				}
				const CFace&	face = faceList[faceID];
				for (int i = 0; i < 3; ++i) {
					DWORD	prID = face.prID[i];
					vertexArray[prID].c.x = __colorMtrl.r;
					vertexArray[prID].c.y = __colorMtrl.g;
					vertexArray[prID].c.z = __colorMtrl.b;
				}
#else
				DWORD	objIndex = *it;
				assert(objIndex != (DWORD) -1);
				if (objIndex == (DWORD) -1)
					continue;
				DWORD	prID = mesh.getPrID(objIndex);
//				assert(prID != (DWORD) -1);
				if (prID == (DWORD) -1) {
					(void) fprintf(stderr, "No prID for objIndex = %d\n", objIndex);
					continue;
				}
				vertexArray[prID].c.x = __colorMtrl.r;
				vertexArray[prID].c.y = __colorMtrl.g;
				vertexArray[prID].c.z = __colorMtrl.b;
#endif
			}

			continue;
		}
#else
		if (!(__typeVal & areaType))
			continue;
#endif
		if (__allSetVal & areaType)	// 이미 색칠했다
			continue;
		__allSetVal ^= areaType;

		const DEFINED_AREA&	defArea = area_it->second;

		// 1. 색깔을 결정
		int	colorIndex = getColorIndex(areaType);
		if (colorIndex == -1) {
			(void) fprintf(stderr, "FATAL: kArea::setColor: getColorIndex(%d) returns -1\n", areaType);
			continue;
		}

		float	fColor = __colorArray[colorIndex];
		D3DXVECTOR3	color;
		if (fColor < 0.f || fColor > 1.f) {
			color = ColorList3[index % COLOR_COUNT];
		} else {
			color = D3DXVECTOR3(3.f * fColor, 0, 0);
			CLAMP2(color);
		}
//		color = D3DXVECTOR3(1, 0, 0);

		// 2. 해당 영역에 표시
		const std::set<DWORD>&	face_id_set = defArea.face_id_set;
		for (std::set<DWORD>::const_iterator it = face_id_set.begin(); it != face_id_set.end(); ++it) {
#if 1	// FACE_ID_LOCAL_INDEX
			DWORD	faceID = *it;
			if (faceID >= numFaces) {
				(void) fprintf(stderr, "FATAL: kArea::setColor: Face ID = %d: Out of range: Ignored\n", faceID);
				continue;
			}
			const CFace&	face = faceList[faceID];
			for (int i = 0; i < 3; ++i)
				vertexArray[face.prID[i]].c = color;
#else
			DWORD	objIndex = *it;
			assert(objIndex != (DWORD) -1);
			if (objIndex == (DWORD) -1)
				continue;
			DWORD	prID = mesh.getPrID(objIndex);
//			assert(prID != (DWORD) -1);
			if (prID == (DWORD) -1) {
				(void) fprintf(stderr, "No prID for objIndex = %d\n", objIndex);
				continue;
			}
			vertexArray[prID].c = color;
#endif
		}
	}
	mesh.GetVertexBufferPointer()->Unlock();
#undef COLOR_COUNT

	return 0;
}
