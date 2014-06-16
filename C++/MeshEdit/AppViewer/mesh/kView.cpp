#include <mesh/kView.h>


kView::kView() : __home(false)
{
}


int	kView::getValue(int subType, void* dataPtr)
{
	if (!dataPtr) {
		(void) fprintf(stderr, "Fatal: kView::getValue: dataPtr is nil\n");
		return -1;
	}

	switch (subType) {
	case VIEW__TYPE:
		*((VIEW_T*) dataPtr) = __view;
		break;
	case VIEW__ROTATION:
		*((ROTATION_T*) dataPtr) = __rotation;
		break;
	case VIEW__HOME:
		*((bool*) dataPtr) = __home;
		break;
	default:
		(void) fprintf(stderr, "Error: kView::getValue: VIEW__SUBTYPE = %d: Unknown subType\n", subType);
	}

	return -1;
}

int	kView::setValue(int subType, const void* dataPtr)
{
	if (!dataPtr) {
		(void) fprintf(stderr, "Fatal: kView::setValue: dataPtr is nil\n");
		return -1;
	}

	switch (subType) {
	case VIEW__TYPE:
		__view = *((VIEW_T*) dataPtr);
		break;
	case VIEW__ROTATION:
		__rotation = *((ROTATION_T*) dataPtr);
		break;
	case VIEW__HOME:
		__home = *((bool*) dataPtr);
		if (__home) {
			__view.type = VIEW_PERSPECTIVE;
			__view.orthographic = false;
			__rotation.state = ROTATE_NONE;
		}
		break;
	default:
		(void) fprintf(stderr, "Error: kView::setValue: VIEW__SUBTYPE = %d: Unknown subType\n", subType);
	}

	return -1;
}
