#include <mesh/kColorBand.h>

kColorBand::kColorBand() : __type(COLOR_BAND_UNDEF)
{
}

kColorBand::~kColorBand()
{
}


int	kColorBand::getValue(int subType, void* dataPtr)
{
	if (!dataPtr) {
		(void) fprintf(stderr, "Fatal: kColorBand::getValue: dataPtr is nil\n");
		return -1;
	}

	switch (subType) {
	case COLOR_BAND__TYPE:
		*((COLOR_BAND_TYPE_T*) dataPtr) = __type;
		return 0;
		break;
	case COLOR_BAND__DIMENSION:
		*((Dimension*) dataPtr) = __dimension;
		return 0;
	case COLOR_BAND__INDICATOR:
	{
		const IndexIndicator&	indexIndicator = *((IndexIndicator*) dataPtr);
		if (indexIndicator.index < 0 || indexIndicator.index >= NUM_INDICATORS) {
			(void) fprintf(stderr, "kColorBand::setValue(COLOR_BAND__INDICATOR): index = %d: Out of range\n", indexIndicator.index);
			return -1;
		}
		*((Indicator*) &indexIndicator) = __indicators[indexIndicator.index];
	}
		return 0;
	default:
		(void) fprintf(stderr, "Error: kColorBand::getValue: AREA__TYPE = %d: Unknown Type\n", subType);
		break;
	}
	return -1;
}

int	kColorBand::setValue(int subType, const void* dataPtr)
{
	if (!dataPtr) {
		(void) fprintf(stderr, "Fatal: kColorBand::getValue: dataPtr is nil\n");
		return -1;
	}

	switch (subType) {
	case COLOR_BAND__TYPE:
		__type = *((COLOR_BAND_TYPE_T*) dataPtr);
		return 0;
		break;
	case COLOR_BAND__DIMENSION:
		__dimension = *((Dimension*) dataPtr);
		return 0;
		break;
	case COLOR_BAND__INDICATOR:
	{
		const IndexIndicator&	indexIndicator = *((IndexIndicator*) dataPtr);
		if (indexIndicator.index < 0 || indexIndicator.index >= NUM_INDICATORS) {
			(void) fprintf(stderr, "kColorBand::setValue(COLOR_BAND__INDICATOR): index = %d: Out of range\n", indexIndicator.index);
			return -1;
		}
		__indicators[indexIndicator.index] = *((Indicator*) &indexIndicator);
	}
		return 0;
		break;
	default:
		(void) fprintf(stderr, "Error: kColorBand::setValue: AREA__TYPE = %d: Unknown Type\n", subType);
		break;
	}
	return -1;
}
