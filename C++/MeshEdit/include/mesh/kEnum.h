#pragma once
#ifndef __kENUM__
#define __kENUM__


// TO DO LIST; pure virtual 로 setValue/getValue 를 가진 클래스를 만들어,
// 두 함수를 사용하는 모든 클래스가 상속받게 한다.
class k2DInterfaceBase
{
public:
	virtual ~k2DInterfaceBase() {}

public:
	virtual int	getValue(int nSubType, void* dataPtr) = 0;
	virtual int	setValue(int nSubType, const void* dataPtr) = 0;
};

// Type 정의
typedef enum {
	TYPE__WEIGHT_MEASURE,	// 무게
	TYPE__TAPE_MEASURE,	// 길이
	TYPE__AREA,		// 영역

	TYPE__MODEL,		// 모델의 표현;
				//	1. 오브젝트: 점/선/면/법선벡터 의 표시 여부,
				//	2. 질감
	TYPE__VIEW,		// 뷰

	TYPE__ISOMETRIC,	// 등고선 표시; 선 또는 색깔, 등고선을 만드는 단면
				// kMesh::ISOMETRIC 구조를 사용
	TYPE__GRID,		// 모눈 종이;
				// GRID 구조를 사용

	TYPE__SIMULATION,	// 시뮬레이션

	TYPE__BODY_ELEMENT,	// 체성분
#if 1	// manipulator
	TYPE__MANIPULATOR,	// manipulator
#endif
#if 1	// ColorBand
	TYPE__COLOR_BAND,
#endif	// ColorBand
} TYPE_XXX;


// subType 정의들...
#if 1	// 화면 뷰 페이지
#if 1	// 모델(표준 모델, 측정 모델)마다 다르게 설정할 수 있다.
typedef enum {
	MODEL__OBJECT,	// 모델을 어떻게 표현할 지: 점/선/면/법선벡터
	MODEL__SHADING,	// 질감; OBJECT_TYPE = OBJECT_FACE 인 경우에만 해당
} MODEL__SUBTYPE;

	// MODEL__OBJECT 인 경우에 사용되는 변수형
	typedef enum {
		OBJECT_UNDEF	= 0,	// 아무것도 표현하지 않는다.

		OBJECT_VERTEX	= 1 << 0,	// 점을 표시 (1)
		OBJECT_EDGE	= 1 << 1,	// 선을 표시 (2)
		OBJECT_FACE	= 1 << 2,	// 면을 표시 (4)
		OBJECT_NORMAL	= 1 << 3,	// 각 점에서 표면에 수직방향 표시 (8)
		OBJECT_VOXEL	= 1 << 4,	// 복셀 표시 (16)
	} OBJECT_TYPE;	// OBJECT_NONE 을 제외하고, 동시에 두 가지 이상을 표시할 수 있음; XOR(^) 을 사용

	// MODEL__SHADING 인 경우에 사용되는 변수형;
	// mesh.fx 에 있는 g__texture 변수가 같은 값을 사용하도록 맞춰야 한다!
	typedef enum {
		SHADING_NONE	= 0,	// 단색으로 표시
		SHADING_GREY	= 1,	// Grey 효과 (라이팅만 고려)
		SHADING_TEXTURE	= 2,	// Texture 가 들어간 Color
	} SHADING_TYPE;
#endif

#if 1	// 뷰
typedef enum {
	VIEW__TYPE,		// 어떤 뷰를 사용할 것인지...
				// 데이터 유형: VIEW_MODE
	VIEW__ROTATION,		// 특정 뷰에서는 카메라를 90도 단위로 회전할 수 있다;
				// 데이터 유형: ROTATION_MODE
	VIEW__HOME,		// 카메라 위치를 포함해서 초기 뷰로 돌아간다; VIEW_PERSPECTIVE
} VIEW__SUBTYPE;

	// 사용되는 뷰
	typedef enum {
		VIEW_PERSPECTIVE,
		VIEW_TOP,
		VIEW_BOTTOM,
		// 아래의 뷰에서는 회전할 수 있다.
		VIEW_FRONT,
		VIEW_BACK,
		VIEW_LEFT,
		VIEW_RIGHT,
	} VIEW_TYPE_T;
	// VIEW__TYPE 에서 사용되는 구조
	struct VIEW_T {
		bool	orthographic;	// VIEW_PERSPECTIVE 가 아닌 경우,
					// 도면의 삼각법처럼 진짜 Orthographic View 인지 아닌 지를 말해준다;
					// True:= 진짜, False:= 무늬만
		VIEW_TYPE_T	type;

		VIEW_T() : orthographic(false), type(VIEW_PERSPECTIVE) {}
	};

	// 회전 여부는 렌더링할 때 매 프레임마다 참고한다;
	// 회전할 지를 결정이 되었다면 다음으로는 회전 중 상태로 넘어간다:
	//	ROTATE_NONE -> ROTATE_READY -> ROTATE_ON -> ROTATE_NONE
	typedef enum {
		ROTATE_NONE,	// 멈춘 상태
		ROTATE_READY,	// 회전하라고 명령을 내린 상태;
				// setValue 에서는 항상 이 값을 사용해서 회전하도록 한다.
		ROTATE_ON,	// 현재 회전 중인 상태
	} ROTATE_STATE;
	// VIEW__ROTATION 에 사용되는 구조
	struct ROTATION_T {
		ROTATE_STATE	state;
		bool		clockwise;	// 위에서 봤을 회전방향; True:= 정면에서 봤을 때 오른쪽에서 왼쪽으로

		ROTATION_T() : state(ROTATE_NONE), clockwise(true) {}
	};
#endif
#endif	// 화면 뷰 페이지

#if 1	// 신체계측 페이지
#if 1	// 체중
typedef enum {
	WEIGHT_MEASURE__LEFT,	// 왼쪽 체중
	WEIGHT_MEASURE__RIGHT,	// 오른쪽 체중
	WEIGHT_MEASURE__TOTAL	// 전체 체중
} WEIGHT_MEASURE__TYPE;
#endif

#if 1	// 길이
#define NUM_TAPE_MEASURE_TYPE	24	// 미리 정해진 영역의 갯수
typedef enum {
#if 0	// TAPE_MEASURE__TYPE
	// 둘레
	TAPE_MEASURE__HEAD		= 1 << 0,	// 머리 둘레
	TAPE_MEASURE__NECK		= 1 << 1,	// 목 둘레
	TAPE_MEASURE__BREAST		= 1 << 2,	// 가슴 둘레
	TAPE_MEASURE__WAIST		= 1 << 3,	// 허리 둘레
	TAPE_MEASURE__HIP		= 1 << 4,	// 엉덩이 둘레
	TAPE_MEASURE__LEFT_THIGH	= 1 << 5,	// 왼쪽 허벅지 둘레
	TAPE_MEASURE__RIGHT_THIGH	= 1 << 6,	// 오른쪽 허벅지 둘레
	TAPE_MEASURE__LEFT_FOREARM	= 1 << 7,	// 왼쪽 팔뚝 둘레
	TAPE_MEASURE__RIGHT_FOREARM	= 1 << 8,	// 오른쪽 팔뚝 둘레

	// 길이
	TAPE_MEASURE__HEIGHT		= 1 << 9,	// 키
	TAPE_MEASURE__SEATED_HEIGHT	= 1 << 10,	// 앉은 키
	TAPE_MEASURE__SHOULDER_WIDTH	= 1 << 11,	// 어깨 넓이
	TAPE_MEASURE__LEFT_LEG		= 1 << 12,	// 왼쪽 다리
	TAPE_MEASURE__RIGHT_LEG		= 1 << 13,	// 오른쪽 다리
	TAPE_MEASURE__LEFT_ARM		= 1 << 14,	// 왼쪽 팔
	TAPE_MEASURE__RIGHT_ARM		= 1 << 15,	// 오른쪽 팔

	TAPE_MEASURE__LEFT_ARM_INSIDE	= 1 << 16,	// 왼쪽 팔 (안쪽)
	TAPE_MEASURE__LEFT_ARM_OUTSIDE	= 1 << 17,	// 왼쪽 팔 (바깥쪽)
	TAPE_MEASURE__RIGHT_ARM_INSIDE	= 1 << 18,	// 오른쪽 팔 (안쪽)
	TAPE_MEASURE__RIGHT_ARM_OUTSIDE	= 1 << 19,	// 오른쪽 팔 (바깥쪽)
	TAPE_MEASURE__LEFT_LEG_INSIDE	= 1 << 20,	// 왼쪽 다리 (안쪽)
	TAPE_MEASURE__LEFT_LEG_OUTSIDE	= 1 << 21,	// 왼쪽 다리 (바깥쪽)
	TAPE_MEASURE__RIGHT_LEG_INSIDE	= 1 << 22,	// 오른쪽 다리 (안쪽)
	TAPE_MEASURE__RIGHT_LEG_OUTSIDE	= 1 << 23,	// 오른쪽 다리 (바깥쪽)
#endif	// TAPE_MEASURE__TYPE


	TAPE_MEASURE__VALUE	= 0,	// TAPE_MEASURE_TYPE_VALUE 에 대한 포인터
					// getValue 만 의미가 있다.
	TAPE_MEASURE__MODE,	// 길이를 재는 모드;
							// 변수형: TAPE_MEASURE_MODE_T
	TAPE_MEASURE__TYPE,	// 특정 부위를 지정할 때
							// 변수형: TAPE_MEASURE_TYPE_VISIBLE
	TAPE_MEASURE__METHOD,	// 둘레를 잴 것인지, 임의의 두 점 사이의 거리를 잴 것인지
							// 변수형: TAPE_MEASURE_METHOD_T
	TAPE_MEASURE__CROSS_SECTION,	// 단면
							// 변수형: TAPE_MEASURE_CROSS_SECTION_T

	TAPE_MEASURE__USER,	// USER_DEFINE_AREA 정보를 얻는다
							// 변수형: kTapeMeasure::USER_DEFINE_AREA;
							// USER_DEFINE_AREA.name 을 설정한 후에...
							// getValue 에서만 사용
	TAPE_MEASURE__NAME,	// 지정된 부위 이름을 정할 때
							// 변수형: TAPE_MEASURE_NAME_T
							// setValue 에서만 사용
	TAPE_MEASURE__NAME_VISIBLE,	// 지정된 부위를 화면에 표시
							// 변수형: TAPE_MEASURE_NAME_VISIBLE
} TAPE_MEASURE__SUBTYPE;

	typedef enum {
		TAPE_MEASURE_CROSS_SECTION_UNDEF = -1,	// 단면을 표시하지 않는다
		TAPE_MEASURE_CROSS_SECTION_2D = 0,
		TAPE_MEASURE_CROSS_SECTION_3D,
	} TAPE_MEASURE_CROSS_SECTION_T;

	typedef enum {
		// 둘레
		TAPE_MEASURE_HEAD		= 1 << 0,	// 머리 둘레
		TAPE_MEASURE_NECK		= 1 << 1,	// 목 둘레
		TAPE_MEASURE_BREAST		= 1 << 2,	// 가슴 둘레
		TAPE_MEASURE_WAIST		= 1 << 3,	// 허리 둘레
		TAPE_MEASURE_HIP		= 1 << 4,	// 엉덩이 둘레
		TAPE_MEASURE_LEFT_THIGH		= 1 << 5,	// 왼쪽 허벅지 둘레
		TAPE_MEASURE_RIGHT_THIGH	= 1 << 6,	// 오른쪽 허벅지 둘레
		TAPE_MEASURE_LEFT_FOREARM	= 1 << 7,	// 왼쪽 팔뚝 둘레
		TAPE_MEASURE_RIGHT_FOREARM	= 1 << 8,	// 오른쪽 팔뚝 둘레

		// 길이
		TAPE_MEASURE_HEIGHT		= 1 << 9,	// 키
		TAPE_MEASURE_SEATED_HEIGHT	= 1 << 10,	// 앉은 키
		TAPE_MEASURE_SHOULDER_WIDTH	= 1 << 11,	// 어깨 넓이
		TAPE_MEASURE_LEFT_LEG		= 1 << 12,	// 왼쪽 다리
		TAPE_MEASURE_RIGHT_LEG		= 1 << 13,	// 오른쪽 다리
		TAPE_MEASURE_LEFT_ARM		= 1 << 14,	// 왼쪽 팔
		TAPE_MEASURE_RIGHT_ARM		= 1 << 15,	// 오른쪽 팔

		TAPE_MEASURE_LEFT_ARM_INSIDE	= 1 << 16,	// 왼쪽 팔 (안쪽)
		TAPE_MEASURE_LEFT_ARM_OUTSIDE	= 1 << 17,	// 왼쪽 팔 (바깥쪽)
		TAPE_MEASURE_RIGHT_ARM_INSIDE	= 1 << 18,	// 오른쪽 팔 (안쪽)
		TAPE_MEASURE_RIGHT_ARM_OUTSIDE	= 1 << 19,	// 오른쪽 팔 (바깥쪽)
		TAPE_MEASURE_LEFT_LEG_INSIDE	= 1 << 20,	// 왼쪽 다리 (안쪽)
		TAPE_MEASURE_LEFT_LEG_OUTSIDE	= 1 << 21,	// 왼쪽 다리 (바깥쪽)
		TAPE_MEASURE_RIGHT_LEG_INSIDE	= 1 << 22,	// 오른쪽 다리 (안쪽)
		TAPE_MEASURE_RIGHT_LEG_OUTSIDE	= 1 << 23,	// 오른쪽 다리 (바깥쪽)
	} TAPE_MEASURE_TYPE_T;

	struct TAPE_MEASURE_TYPE_VALUE {
		TAPE_MEASURE_TYPE_T	type;
		float			value;
	};

	struct TAPE_MEASURE_TYPE_VISIBLE {
		TAPE_MEASURE_TYPE_T	type;
		bool			visible;
	};

	struct TAPE_MEASURE_NAME_T {
		char*	old_name;	// 이전 이름; 처음이라면 NULL 로 설정
		char*	new_name;	// 이름
	};

	struct TAPE_MEASURE_NAME_VISIBLE {
		char*	name;
		bool	visible;
	};

	// 단축키: SHIFT + ALT + F6
	typedef enum {
		TAPE_MEASURE_MODE_CONTOUR	= 0,	// 곡면을 따라
		TAPE_MEASURE_MODE_WRAP,			// 줄자를 사용하여
	} TAPE_MEASURE_MODE_T;

	typedef enum {
		TAPE_MEASURE_METHOD_UNDEF	= -1,
		TAPE_MEASURE_METHOD_TWO_POINT	= 0,	// 임의의 두 점 사이의 거리; 단축키: CTRL + ALT + F6
		TAPE_MEASURE_METHOD_GIRTH		// 둘레의 길이; 단축키: SHIFT + CTRL + F6

	} TAPE_MEASURE_METHOD_T;
#endif
#endif	// 신체계측 페이지

#if 1	// 등고선 페이지
#if 1	// 등고선
// TYPE__ISOMETRIC 의 subType
typedef enum {
	ISOMETRIC__TYPE =0,
	ISOMETRIC__PLANE,
} ISOMETRIC__SUBTYPE;

// 각 subType 에 해당하는 변수형
	// 선으로 표시했을 때,
	// 선에 의한 비교는 3차원 뷰(VIEW__PERSPECTIVE) 보다는 2차원 뷰에 잘 나타난다.
	typedef enum {
#if 1	// 색깔 등고선을 따로 처리
#if 1	// 제형욱씨의 요청
		ISOMETRIC_UNDEF	= 1 << 0,	// 등고선을 표시하지 않을 때 사용
		ISOMETRIC_LINE	= 1 << 1,	// 선
		ISOMETRIC_COLOR	= 1 << 2,	// 색깔
#else
		ISOMETRIC_UNDEF	= -1,
		ISOMETRIC_LINE,
		ISOMETRIC_COLOR,
#endif
#else
		// 숫자(0)이 추가된 것은 원래값임을 뜻한다;
		// 직접 사용하지말고 아래의 숫자(0)이 없는 것을 사용한다.
		ISOMETRIC_UNDEF0	= 1 << 0,	// 등고선을 표시하지 않을 때 사용
		ISOMETRIC_LINE0		= 1 << 1,	// 선
		ISOMETRIC_COLOR0	= 1 << 2,	// 색깔

		// 둘 중의 하나만 설정되어야 한다.
		ISOMETRIC_SAVE_COLOR	= 1 << 3,
		ISOMETRIC_RESTORE_COLOR	= 1 << 4,


		// UNDEF 는 원래 색깔로 되돌리는 것이 기본값이다.
		ISOMETRIC_UNDEF				= ISOMETRIC_UNDEF0 | ISOMETRIC_RESTORE_COLOR,
							// UNDEF 와 같고 추가적으로 원래 색깔로 되돌린다;
							// 원래대로 돌아가는 것이므로 색깔을 따로 저장할 필요가 없다.
							// 즉 ISOMETRIC_UNDEF_SAVE_COLOR 는 없다
		ISOMETRIC_UNDEF_WITHOUT_RESTORE_COLOR	= ISOMETRIC_UNDEF0,

		// LINE 은 원래 색깔을 보존하는 것이 기본값이다.
		ISOMETRIC_LINE				= ISOMETRIC_LINE0 | ISOMETRIC_SAVE_COLOR,
							// 선과 같고 추가적으로 색깔을 보존한다.
		ISOMETRIC_LINE_WITHOUT_SAVE_COLOR	= ISOMETRIC_LINE0,
		ISOMETRIC_LINE_WITH_RESTORE_COLOR	= ISOMETRIC_LINE0 | ISOMETRIC_RESTORE_COLOR,
							// 선과 같고 추가적으로 원래 색깔로 되돌린다.
		// COLOR 는 원래 색깔을 보존하는 것이 기본값이다.
		ISOMETRIC_COLOR				= ISOMETRIC_COLOR0 | ISOMETRIC_SAVE_COLOR,
							// 색깔과 같고 추가적으로 색깔을 보존한다;
							// 색깔을 바꾸는 것으므로 원래 색깔로 되돌릴 수는 없다.
							// 즉 ISOMETRIC_COLOR_RESTORE_COLOR 는 없다.
		ISOMETRIC_COLOR_WITHOUT_SAVE_COLOR	= ISOMETRIC_COLOR0,
#endif
	} ISOMETRIC_TYPE_T;

	// 동시에 둘 이상의 등고선이 표시될 수도 있다.
	typedef enum {
		ISOMETRIC_XY	= 1 << 0,	// z 값의 차이를 등고선으로 표시;
						// 정면에서 봤을 때 좌우 대칭 유무 판단
		ISOMETRIC_YZ	= 1 << 1,	// x 값의 차이를 등고선으로 표시;
						// 뒷면에서 봤을 때 척추의 휜 정도를 판단
		ISOMETRIC_ZX	= 1 << 2,	// y 값의 차이를 등고선으로 표시;
						// 뒷면에서 봤을 때 어깨의 기울어진 정도를 판단
	} ISOMETRIC_PLANE_T;
#endif

#if 1	// 모눈 종이
// TYPE__GRID 의 subType
typedef enum {
#if 1	// TYPE
	GRID__TYPE,
#endif	// TYPE
	GRID__Z_DEPTH,
	GRID__DIRECTION,
	GRID__INTERVAL,
} GRID__SUBTYPE;

// 각 subType 에 해당하는 변수형
	// 유형
	typedef enum {
		GRID_UNDEF = -1,
		GRID_NORMAL,
		GRID_CROSS,
	} GRID_TYPE;
	// 표시
	typedef enum {
#if 0
		GRID_UNDEF = -1,	// 모눈 종이를 표시하지 않을 때 사용
#endif
		GRID_FRONT,		// 모눈을 제일 앞에 표시
		GRID_BACK,		// 모눈을 제일 뒤에 표시
	} GRID_Z;

	// 방향
	typedef enum {
		GRID_VERTICAL	= 1 << 0,	// 수직
		GRID_HORIZONTAL	= 1 << 1,	// 수평
		GRID_BOTH	= GRID_VERTICAL | GRID_HORIZONTAL,	// 둘 다 표시 (3 = 1 + 2)
	} GRID_DIR;

	// 간격
	struct GRID_STEP
	{
		float	h_step;
		float	v_step;

		// kGrid.cpp 에서 설정
		static const float	H_STEP;
		static const float	V_STEP;

		GRID_STEP() : h_step(H_STEP), v_step(V_STEP) {}
		GRID_STEP(float x, float y) : h_step(x), v_step(y) {}
	};
#if 1	// TYPE
#if 1	// 기준이 화면
	struct GRID_CENTER
	{
		float	Ox;
		float	Oy;

		// kGrid.cpp 에서 설정
		static const float	CENTER_X;
		static const float	CENTER_Y;

		GRID_CENTER() : Ox(CENTER_X), Oy(CENTER_Y) {}
		GRID_CENTER(float x, float y) : Ox(x), Oy(y) {}
	};
#endif	// 기준이 화면
#endif	// TYPE
#endif	// 모눈 종이
#endif	// 등고선 페이지

#if 1	// manipulator
typedef enum {
	MANIPULATOR__TYPE,	// manipulator 유형;
				// 데이터 유형: MANIPULATOR_TYPE_T 에 대한 포인터
	MANIPULATOR__SET,	// 설정;
				// 데이터 유형: int 에 포인터
				// 값:
				//	0	임의로 하는, 사용자 정의 둘레측정
				//	!= 0	이미 정의된 둘레측정
	MANIPULATOR__NORMAL_ANGLE_ROTATION,	// 유형;
				// 데이터 유형: MANIPULATOR_NORMAL_ANGLE_ROTATION_T 에 대한 포인터
} MANIPULATOR__SUBTYPE;

	typedef enum {
		UNDEF_MANIPULATOR = -1,
		NORMAL_MANIPULATOR,
		POINT_MANIPULATOR,
		LINE_MANIPULATOR
	} MANIPULATOR_TYPE_T;

	typedef enum {
		MANIPULATOR_NORMAL_ANGLE_ROTATION_BOTH_THETA_AND_PHI = 0,
		MANIPULATOR_NORMAL_ANGLE_ROTATION_THETA_ONLY,	// x축에 대한 회전 즉 노말의 z축에 대한 각도(\theta)만 바꾼다.
	} MANIPULATOR_NORMAL_ANGLE_ROTATION_T;
#endif	// manipulator

typedef enum {
	MODEL_TYPE__STANDARD = 1 << 0,
	MODEL_TYPE__REAL = 1 << 1,
} MODEL_TYPE_T;

typedef enum {
	WM_USER__LENGTH	= WM_USER + 10,
	WM_USER__ANGLE	= WM_USER + 11,
	WM_USER__WHOLE_BODY_SIMULATION_DONE	= WM_USER + 12,
} WM_USER__MSG;

#if 1	// SHIFT
typedef enum {
	SHIFT_ALIGN_LEFT,
	SHIFT_ALIGN_CENTER,
	SHIFT_ALIGN_RIGHT,
} SHIFT_ALIGN_TYPE;
#endif	// SHIFT


#endif	// !__kENUM__
