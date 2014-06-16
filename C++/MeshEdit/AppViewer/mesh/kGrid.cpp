#include <mesh/kGrid.h>

const float	GRID_STEP::H_STEP	= .15f;
const float	GRID_STEP::V_STEP	= .15f;
#if 1	// TYPE
#if 1	// 기준이 화면
const float	GRID_CENTER::CENTER_X	= 0.f;
const float	GRID_CENTER::CENTER_Y	= 0.f;
#endif	// 기준이 화면
#endif	// TYPE


kGrid::kGrid() : __type(GRID_UNDEF), __z(GRID_FRONT), __dir(GRID_BOTH)
{
}


int	kGrid::getValue(int subType, void* dataPtr)
{
	if (!dataPtr) {
		(void) fprintf(stderr, "Fatal: kGrid::getValue: dataPtr is nil\n");
		return -1;
	}

	switch (subType) {
#if 1	// TYPE
	case GRID__TYPE:
		*((GRID_TYPE*) dataPtr) = __type;
		break;
#endif	// TYPE
	case GRID__Z_DEPTH:
		*((GRID_Z*) dataPtr) = __z;
		break;
	case GRID__DIRECTION:
		*((GRID_DIR*) dataPtr) = __dir;
		break;
	case GRID__INTERVAL:
		*((GRID_STEP*) dataPtr) = __step;
		break;
	default:
		(void) fprintf(stderr, "Error: kGrid::getValue: GRID__SUBTYPE = %d: Unknown subType\n", subType);
	}

	return -1;
}

int	kGrid::setValue(int subType, const void* dataPtr)
{
	if (!dataPtr) {
		(void) fprintf(stderr, "Fatal: kGrid::setValue: dataPtr is nil\n");
		return -1;
	}

	switch (subType) {
#if 1	// TYPE
	case GRID__TYPE:
		__type = *((GRID_TYPE*) dataPtr);
		break;
#endif	// TYPE
	case GRID__Z_DEPTH:
		__z = *((GRID_Z*) dataPtr);
		break;
	case GRID__DIRECTION:
		__dir = *((GRID_DIR*) dataPtr);
		break;
	case GRID__INTERVAL:
		__step = *((GRID_STEP*) dataPtr);
		break;
	default:
		(void) fprintf(stderr, "Error: kGrid::setValue: GRID__SUBTYPE = %d: Unknown subType\n", subType);
	}

	return -1;
}
