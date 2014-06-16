#include <mesh/kBodyElement.h>

kBodyElement::kBodyElement() : __vsValue(0)
{
	for (int i = 0; i < NUM_BODY_ELEMENT_RAW_TYPE; ++i)
		__values[i] = 1.f;
}

int	kBodyElement::getValue(int subType, void* dataPtr)
{
	if (!dataPtr) {
		(void) fprintf(stderr, "Fatal: kBodyElement::getValue: dataPtr is nil\n");
		return -1;
	}

	switch (subType) {
	case BODY_ELEMENT__RAW_VALUE:
	{
		RAW_VALUE&	rawValue = *((RAW_VALUE*) dataPtr);
		int	iRawType = (int) rawValue.type;
		if (iRawType < 0 || iRawType >= NUM_BODY_ELEMENT_RAW_TYPE) {
			return -1;
		}
		rawValue.value = __values[iRawType];
	}
		return 0;
		break;
	case BODY_ELEMENT__VS:
	{
		VS_VISIBLE&	vsVisible = *((VS_VISIBLE*) dataPtr);
		vsVisible.visible = __vsValue & vsVisible.type ? true : false;
	}
		return 0;
		break;
	default:
		(void) fprintf(stderr, "Error: kBodyElement::getValue: BODY_ELEMENT__SUBTYPE = %d: Unknown subType\n", subType);
	}

	return -1;
}

int	kBodyElement::setValue(int subType, const void* dataPtr)
{
	if (!dataPtr) {
		(void) fprintf(stderr, "Fatal: kBodyElement::setValue: dataPtr is nil\n");
		return -1;
	}

	switch (subType) {
	case BODY_ELEMENT__RAW_VALUE:
	{
		const RAW_VALUE&	rawValue = *((RAW_VALUE*) dataPtr);
		int	iRawType = (int) rawValue.type;
		if (iRawType < 0 || iRawType >= NUM_BODY_ELEMENT_RAW_TYPE) {
			return -1;
		}
		__values[iRawType] = rawValue.value;
	}
		return 0;
		break;
	case BODY_ELEMENT__VS:
	{
		const VS_VISIBLE&	vsVisible = *((VS_VISIBLE*) dataPtr);
		if (vsVisible.visible)
			__vsValue |= (int) vsVisible.type;
		else
			__vsValue &= ~(int) vsVisible.type;
	}
		return 0;
		break;
	default:
		(void) fprintf(stderr, "Error: kBodyElement::setValue: BODY_ELEMENT__SUBTYPE = %d: Unknown subType\n", subType);
	}

	return -1;
}
