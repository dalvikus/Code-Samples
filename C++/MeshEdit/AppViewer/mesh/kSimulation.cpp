#include <mesh/kSimulation.h>


const MODEL_POINT	BREAST_IMPLANT_TEMPLATE::PositionArray[N_MODEL_POINT_TYPE] = {
	// MODEL_POINT_LEFT_UP: 왼쪽 가슴 위
	{7149, 0.395219f, 0.157255f},
	// MODEL_POINT_RIGHT_UP: 오른쪽 가슴 위
	{6146, 0.530637f, 0.046719f},
};
//TEMPLATE_COLOR		BREAST_IMPLANT_TEMPLATE::ColorArray[N_BREAST_IMPLANT_TYPE * N_MODEL_POINT_TYPE];
TEMPLATE_COLOR		BREAST_IMPLANT_TEMPLATE::ColorArray[N_MODEL_POINT_TYPE][N_BREAST_IMPLANT_TYPE ];



kSimulation::kSimulation() :
	__type(SIMULATION_TYPE_MANUAL),
	__execType(SIMULATION_EXEC_UNDEF),
	__isReady(false)
{
}


int	kSimulation::getValue(int subType, void* dataPtr)
{
	if (!dataPtr) {
		(void) fprintf(stderr, "Fatal: kSimulation::getValue: dataPtr is nil\n");
		return -1;
	}

	switch (subType) {
	case SIMULATION__METHOD:
		*((SIMULATION_TYPE*) dataPtr) = __type;
		break;

	case SIMULATION__STANDARD:
		*((STANDARD_SIMULATION*) dataPtr) = __standard;
		break;
	case SIMULATION__MANUAL:
		*((MANUAL_SIMULATION*) dataPtr) = __manual;
		break;
	case SIMULATION__TEMPLATE:
		*((BREAST_IMPLANT_TEMPLATE*) dataPtr) = __template;
		break;

#if 1
	case SIMULATION__EXEC_METHOD:
	{
		SIMULATION_EXEC_METHOD_VALUE&	execMethodVal = *((SIMULATION_EXEC_METHOD_VALUE*) dataPtr);
		if (__ccType == SIMULATION_CC_TYPE_FINAL) {
			execMethodVal.type = SIMULATION_EXEC_METHOD_FINAL;
			execMethodVal.val = __final_cc;
		} else {	// SIMULATION_CC_TYPE_VOLUME_CHANGE
			if (__stepType == SIMULATION_STEP_TYPE_CC) {
				execMethodVal.type = SIMULATION_EXEC_METHOD_STEP_CC;
				execMethodVal.val = __cc_step;
			} else {	// SIMULATION_STEP_TYPE_RATIO
				execMethodVal.type = SIMULATION_EXEC_METHOD_STEP_RATIO;
				execMethodVal.val = __normal_amount_step;
			}
		}
	}
		break;
#else
	case SIMULATION__CC_TYPE:
		*((SIMULATION_CC_TYPE*) dataPtr) = __ccType;
		break;
	case SIMULATION__STEP_TYPE:
		*((SIMULATION_STEP_TYPE*) dataPtr) = __stepType;
		break;
	case SIMULATION__CC_STEP_SIZE:
		*((float*) dataPtr) = __cc_step;
		break;
	case SIMULATION__NORMAL_AMOUNT_STEP_SIZE:
		*((float*) dataPtr) = __normal_amount_step;
		break;
#endif
	case SIMULATION__VOLUME_CHANGE:
		*((float*) dataPtr) = __volume_change_cc;
		break;

	case SIMULATION__EXEC:
		*((SIMULATION_EXEC_TYPE*) dataPtr) = __execType;
		break;
	default:
		(void) fprintf(stderr, "Error: kSimulation::getValue: SIMULATION__SUBTYPE = %d: Unknown subType\n", subType);
	}

	return -1;
}

int	kSimulation::setValue(int subType, const void* dataPtr)
{
	if (!dataPtr) {
		(void) fprintf(stderr, "Fatal: kSimulation::setValue: dataPtr is nil\n");
		return -1;
	}

	switch (subType) {
	case SIMULATION__METHOD:
		__type = *((SIMULATION_TYPE*) dataPtr);
		break;

	case SIMULATION__STANDARD:
		__standard = *((STANDARD_SIMULATION*) dataPtr);
		break;
	case SIMULATION__MANUAL:
		__manual = *((MANUAL_SIMULATION*) dataPtr);
		if (__manual.type == MANUAL_DIRECTLY)
			__execType = SIMULATION_EXEC_BY_BRUSH;
		break;
	case SIMULATION__TEMPLATE:
		__template = *((BREAST_IMPLANT_TEMPLATE*) dataPtr);
		break;

#if 1
	case SIMULATION__EXEC_METHOD:
	{
		const SIMULATION_EXEC_METHOD_VALUE&	execMethodVal = *((SIMULATION_EXEC_METHOD_VALUE*) dataPtr);
		if (execMethodVal.type == SIMULATION_EXEC_METHOD_FINAL) {
			__ccType = SIMULATION_CC_TYPE_FINAL;
			__final_cc = execMethodVal.val;
		} else {
			__ccType = SIMULATION_CC_TYPE_VOLUME_CHANGE;
			if (execMethodVal.type == SIMULATION_EXEC_METHOD_STEP_CC) {
				__stepType = SIMULATION_STEP_TYPE_CC;
				__cc_step = execMethodVal.val;
			} else {	// SIMULATION_EXEC_METHOD_STEP_RATIO
				__stepType = SIMULATION_STEP_TYPE_RATIO;
				__normal_amount_step = execMethodVal.val;
			}
		}
	}
		break;
#else
	case SIMULATION__CC_TYPE:
		__ccType = *((SIMULATION_CC_TYPE*) dataPtr);
		break;
	case SIMULATION__STEP_TYPE:
		__stepType = *((SIMULATION_STEP_TYPE*) dataPtr);
		break;
	case SIMULATION__CC_STEP_SIZE:
		__cc_step = *((float*) dataPtr);
		break;
	case SIMULATION__NORMAL_AMOUNT_STEP_SIZE:
		__normal_amount_step = *((float*) dataPtr);
		break;
	case SIMULATION__VOLUME_CHANGE:
		__final_cc = *((float*) dataPtr);
		break;
#endif

	case SIMULATION__EXEC:
		__execType = *((SIMULATION_EXEC_TYPE*) dataPtr);
		break;
	default:
		(void) fprintf(stderr, "Error: kSimulation::setValue: SIMULATION__SUBTYPE = %d: Unknown subType\n", subType);
	}

	return -1;
}
