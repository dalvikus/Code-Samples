#include <mesh/kState.h>

#if 1	// 2D 와의 통신에 사용되는 함수의 구현
int	kState::getValue(
	TYPE_XXX	type,
	int		subType,
	void*		dataPtr
)
{
	switch (type) {
	case TYPE__GRID:
		return ((kGrid*) this)->getValue(subType, dataPtr);
	case TYPE__VIEW:
		return ((kView*) this)->getValue(subType, dataPtr);
#if 1	// ColorBand
	case TYPE__COLOR_BAND:
		return ((kColorBand*) this)->getValue(subType, dataPtr);
#endif	// ColorBand
	default:
		(void) fprintf(stderr, "Error: kState::setValue: TYPE_XXX:= %d: Not implemented yet\n", type);
		break;
	}
	return -1;
}

int	kState::setValue(
	TYPE_XXX	type,
	int		subType,
	const void*	dataPtr
)
{
	switch (type) {
	case TYPE__GRID:
		return ((kGrid*) this)->setValue(subType, dataPtr);
	case TYPE__VIEW:
		return ((kView*) this)->setValue(subType, dataPtr);
#if 1	// ColorBand
	case TYPE__COLOR_BAND:
		return ((kColorBand*) this)->setValue(subType, dataPtr);
		break;
#endif	// ColorBand
	default:
		(void) fprintf(stderr, "Error: kState::setValue: TYPE_XXX:= %d: Not implemented yet\n", type);
		break;
	}
	return -1;
}
#endif

#if 1	// 체형변화 시뮬레이션
const float	kState::SCALE_MIN	= -5.f;
const float	kState::SCALE_MAX	= 5.f;

const int	kState::VOLUME_MIN	= -30;	// <= 0 && VOLUME_MIN % VOLUME_STEP == 0
const int	kState::VOLUME_STEP	= 1;	// > 0
const int	kState::VOLUME_MAX	= 10;	// >= 0 && VOLUME_MAX % VOLUME_STEP == 0
#endif	// 체형변화 시뮬레이션
#if 1	// 마지막 주문; 5. 측정선 선명하게...
const float	kState::SHIFT_FACTOR_ALONG_NORMAL	= .05f;	// kMesh::__a 의 몇 배
#endif	// 마지막 주문; 5. 측정선 선명하게...
#if 1	// 마지막 주문; 9. 카메라 위치 제한
const float	kState::MIN_CAMERA_DISTANCE_FACTOR	= -50.f;	// 카메라와 모델 사이의 거리는 이 보다 커야 한다; 줌 제한
									// 작을(<) 수록 확대가 더 많이 된다.
const float	kState::MAX_CAMERA_DISTANCE_FACTOR	= 150.f;	// 카메라와 모델 사이의 거리는 이 보다 커야 한다; 줌 제한
									// 클(>) 수록 축소가 더 많이 된다.
#endif	// 마지막 주문; 9. 카메라 위치 제한
