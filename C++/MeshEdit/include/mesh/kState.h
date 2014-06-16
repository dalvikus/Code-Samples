#pragma once
#ifndef __kSTATE__
#define __kSTATE__


#include <DXUT/Core/DXUT.h>
#include <vector>

#include <mesh/kDef.h>
#include <mesh/kEnum.h>
#include <mesh/kPolygon.h>	// kID
#include <mesh/kGrid.h>
#include <mesh/kView.h>
#if 1	// ColorBand
#include <mesh/kColorBand.h>
#endif	// ColorBand
#include <mesh/kBrush.h>
#if 1	// kDisplacementMap
#include <mesh/kDisplacementMap.h>
#endif	// kDisplacementMap


struct kState : kGrid, kView, kColorBand
{
#if 1	// VK_RETURN
	// Return 키를 눌러 변수이름에 해당하는 문자열을 기억한다;
	// 'A'-'Z', 'a'-'z', '0'-'9', '_' (단 숫자는 첫 글자가 될 수 없다.)
	struct ReturnMessage
	{
		char*	varName;
		bool	keyPressed;
		unsigned	maxChars;	// '0' 은 제외
		unsigned	numChars;

		ReturnMessage(unsigned max_chars = 32)
		{
			varName = new char[max_chars + 1];
			*varName = 0;
			maxChars = max_chars;
			keyPressed = false;
			numChars = 0;
		}
		~ReturnMessage()
		{
			if (varName) {
				delete[] varName;
				varName = 0;
			}
		}
	};

#if 1	// 마지막 주문; 9. 카메라 위치 제한
	const static float	MIN_CAMERA_DISTANCE_FACTOR;	// 모델과 카메라 사이의 거리는,
								// 모델의 기본단위길이(kMesh::__a) 에다
								// 이 값을 곱한 값보다 보다 커야 한다.
	const static float	MAX_CAMERA_DISTANCE_FACTOR;	// ...보다 작아야 한다.
#endif	// 마지막 주문; 9. 카메라 위치 제한
#if 1	// 마지막 주문; 1. 회전축 고정
	bool	fixed_axis;
#endif	// 마지막 주문; 1. 회전축 고정
#if 1	// 마지막 주문; 5. 측정선 선명하게...
	const static float	SHIFT_FACTOR_ALONG_NORMAL;	// 선을 그릴 때, 선을 이루는 각 점을 얼마나 띄울 것인가?
#endif	// 마지막 주문; 5. 측정선 선명하게...
#if 1	// 체형변화 시뮬레이션
	const static float	SCALE_MIN;
	const static float	SCALE_MAX;

	const static int	VOLUME_MIN;	// <= 0 && VOLUME_MIN % VOLUME_STEP == 0
	const static int	VOLUME_STEP;	// > 0
	const static int	VOLUME_MAX;	// >= 0 && VOLUME_MAX % VOLUME_STEP == 0

	bool	playback;
	float	playback_fps;	// frame per second; 24:= Film, 30:= TV
	bool	playback_backward;	// True:= 0부터 살이 빠진다, False:= kMesh::VOLUME_MIN 부터 살이 찐다
	int	playback_index;	// 시작 시간: [kMesh::VOLUME_MIN, kMesh::VOLUME_MAX: kMesh::VOLUME_STEP]
#endif	// 체형변화 시뮬레이션
#if 1	// AREA2_HINT
	std::vector<DWORD>	area2_vertex_array;
	std::vector<DWORD>	area2_face_array;
#if 0	// moved to CPointRepr
	struct Area2Flag {
		bool	subset:1;	// 이 Vertex 가 속한 Face 가 움직이는 subset 에 표함되어 있는가?
		bool	move:1;		// 이 Vertex 가 실제로 움직였는가?
		bool	inc:1;		// 면적이 증가한 삼각형에 속한 Vertex 인가?
		bool	all:1;		// 이 Vertex 주위의 모든 삼각형의 면적이 증가했는가? (all or nothing?)

		bool	repos:1;	// 위치를 재설정할 때 이 Vertex 가 사용되는가?
					//	1) inc 만 설정되었더라도 (주위의 삼각형 중에서 어느 하나라도 면적이 늘어났을 때)
					//	2) 또는 다른 조건을 통해... (TO DO LIST)
		bool	done:1;		// 이 Vertex 의 새로운 위치가 결정되었는가?
		bool	smooth:1;	// Smoothing 처리할 때 이 Vertex 가 사용되는가?
					//	1) inc 가 설정된 Vertex 주위로 n(>= 0) 개의 Vertex 까지
					//	TO DO LIST: n > 0 인 경우에...
					//	2) move 가 설정된 Vertex
					//	3) 또는 다른 조건을 통해... (TO DO LIST)
					//
					//
					// 아래의 Area2Control::Smooth 가 1 이고,
					// 아래의 Area2Control::SmoothIncLevel 이 0이 아닌 경우에...
					//
		bool	boundary0:1;	// Area2Flag::smooth 를 결정할 때 사용되는 플래그
		bool	boundary:1;	// 실제로 경계가 되는 Vertex 인가? (임시 플래그)
					//
					// boundary0 이 설정된 Vertex 주변의 Vertex 중에서,
					// smooth 가 설정되지 않은 Vertex 가 boundary 가 된다;
					// boundary 가 설정된 Vertex 가 smooth 될 Vertex 에 추가되고,
					// boundary 가 boundary0 으로 돼 다음 smooth 결정에 사용된다.
					//
					//	1) smooth 가 설정된 Vertex 에 boundary0 설정을 한다;
					//	Area2Control::SmoothIncLevel > 0 경우에 한해
					//
					//	2) boundary0 가 설정된 Vertex 에 대해,
					//	이전 단계에서 smooth 가 설정된 Vertex 를 참고하여,
					//	boundary 가 되는 Vertex 를 결정한다.
					//	3) boundary 가 설정된 Vertex 를,
					//	smooth 가 설정되게 하는 Vertex 에 추가한다.
					//	다음 번을 위해 boundary0 를 갱신한다.
					//
					//	4) Area2Control::SmoothIncLevel 만큼 2)~4)를 반복한다.

		Area2Flag() :
			subset(false),
			move(false),
			inc(false),
			all(false),
			repos(false),
			done(false),
			smooth(false),
			boundary0(false),
			boundary(false)
		{}
	};
#endif
	struct Area2Control
	{
		unsigned	ShowArea2:2;	// 살을 뺐을 때 면적이 증가하는 삼각형 주위의 점들을 표시할 지
						// 0: 표시하지 않음
						// 1: 주위의 삼각형 중에서 하나라도 면적이 증가했을 때 (inc 이 True)
						// 2: 또 다른 조건에 의해... (TO DO LIST)
		bool		Repos;		// 면적이 증가한 삼각형을 가진 Vertex 의 위치를 재설정할 지 여부
		unsigned	Smooth:2;	// 재설정된 Vertex 로 인한 Smoothing 처리를 할 지
						// 0: Smoothing 처리하지 않음
						// 1: 재설정된 Vertex 주위에 있는 Vertex 을 이용해서 (SmoothIncLevel 에 따라)
						// 2: 움직인 모든 Vertex 들을 사용해서 (move 가 True)
						//
						// *: 실제로 몇 단계의 Smoothing 이 일어나는 지는 kBrush::Flag::SmoothLevel 를 사용
		unsigned	SmoothIncLevel:2;	// 재설정된 주위의 몇 단계의 Vertex 를 사용할 지 (Smooth 가 1 일 때만 사용); 0~3 단계

		Area2Control() :
			ShowArea2(1),
			Repos(true),
			Smooth(2),
			SmoothIncLevel(3)
		{}
	};
	Area2Control	area2_control;
	int	Area2Threshold;
#endif	// AREA2_HINT
	bool	DoShift;	// 이 값이 True 면 'j', 'k' 를 사용해 shift 값을 변경할 수 있다.
public:
	bool	SetGridCenter;
	float	scale_alpha;	// kBone::apply* 에 사용되는 fScale
	CGrowableArray<D3DXVECTOR3>	points;
#if 1	// kDisplacementMap
	kGdiPlus	gdiplus;	// singleton
#endif	// kDisplacementMap
	kBrush	brush;
	INTERSECTION*	intersection_ptr;	// brush_do 모드에서 마우스를 클릭하는 시점에
						// 마주치는 Face 가 있는 지를 말해준다.
#if 1	// 모든 계측부위 표시
	bool	displayAll;	// 모든 계측부위 표시
#endif	// 모든 계측부위 표시
	bool	doMeasure;
	ReturnMessage	message;
#endif	// VK_RETURN
#if 1	// ColorBand
	bool	ColorBandControl;
#endif	// ColorBand
	D3DXCOLOR	g__colorMtrl;	// 모델의 기본 색깔
#if 1
#define MAX_INTERSECTIONS 16
	DWORD			__dwNumIntersections;			// Number of faces intersected
	INTERSECTION		__intersectionArray[MAX_INTERSECTIONS];	// Intersection info
#endif

#if 1	// Convex
	TAPE_MEASURE_MODE_T	g__mode;
	bool	g__show_all_convex_points;
	int	g__next_convex_index;	// 줄자 방식으로 둘레를 잴 때,
					// 볼록다각형의 각 꼭지점을 순차적으로 조사하기 위하여...;
					// 'J', 'K' 를 사용하여 각각 이 값을,
					// kTapeMeasure::convexArray 의 크기내에서,
					// 1씩 감소, 증가시킨다:
#endif
#if 3
	// 표준모델을 보여줄 지 말지...
	MODEL_TYPE_T	modelType;
	bool	g__subset;	// subset 에 따라 다르게 표시할 지
	// Picking 을 표준모델에 대해서 할 것인지...
	bool	g__pickStandard;

	bool	g__follow;	// 표준모델의 대응점으로 움직인다;
				// SHIFT 키가 눌려져 있다면 반대방향으로 움직인다.
				// 자동으로 false 로 돌아간다.
	// 둘레의 길이를 잴 때, 단면에 대한 정보를 얻기 위해,
	// 각 면을 지나는 선분의 중점에서 각 Face 에 대한 정보(ID, U, V) 를 출력한다;
	// 이 값이 True 일 때마다 다시 길이를 측정해서 위 정보를 출력한다.
	// 자동으로 false 로 돌아간다.
	bool	g__printBarycentricCoordinates;
	// 이미 정의된 둘레의 길이를 표시한다.
	bool	g__showDefinedMeasure;
	// 선택한 점들을 포함하는 Face 들의 ID 목록을 얻고자 할 때 설정한다.
//	bool	printFaceIDList;
	// 이미 정의된 영역을 표시한다;
	// 자동으로 false 로 돌아간다.
	bool	g__showDefinedArea;
	// 체성분 값을 해당 영역에 색깔로 표시한다; 동시에 g__showDefinedArea 가 true 일 수는 없다.
	bool	g__showDefinedBodyElement;

	// 색칠해진 영역을 출력한다.
//	bool	printColor;
	bool	g__showTemplateColor;
#endif

	DWORD		g__nTessellation;
	unsigned	g__nChannel;
	unsigned	g__nStep;

#if 1	// kState
	UINT		g__NSelectedPoints;
	DWORD		g__lastPrID;
	DWORD		g__prID;
	DWORD		g__index;

#if 1	// Draw String...
#if 1	// Debugging...
	std::vector<DWORD>	debugging_EdgeID_array;
	std::vector<DWORD>	debugging_prReprID_array;
	std::vector<DWORD>	debugging_FaceID_array;
	std::vector<DWORD>	debugging_prReprID_array0;
#endif

	bool	manipulator_mode;
	bool	in_point_manipulator;
#if 1	// line manipulator
	bool		__showSegment;	// line manipulator 가 조절하는 선분이 보이록
					// 일시적으로 계측 선분을 보이기 않도록 할 때 사용
#endif
	bool		g__bString;
	bool		g__stringMeasure;
	bool		g__incompleteString;

	bool		g__stringMoveO;
	FLOAT		g__stringRotationAngleByAmount;
	bool		g__stringHeading;
	bool		g__stringAttitude;
	FLOAT		g__stringLength;
	D3DXVECTOR3	g__n;
								// 여기서는 임의로 설정하고,
								// (g__stringHeading, g__stringAttitude) 를 이용하여 바꿀 수 있다.
								// 시작할 때 Normalized 된다.
	int		g__numSegments;
	bool		g__stringClosed;
	bool		g__stringError;
	// 마지막 Segment 의 끝점에서 값들...
	//	String 을 그릴 때, g__stringContinueAtLastSegment 가 True 이면,
	//	그 끝점을 시작점으로해서 String 을 그린다;
	//	위의 모든 값들이 아래의 값들로 교체된다.
	bool		g__stringContinueAtLastSegment;
#endif	// Draw String...

	bool		g__plus;
	bool		g__minus;
	bool		g__Alt;
	bool		g__Shift;
	bool		g__Control;
	bool		g__bDrag;
	int		g__iDragBeginX;
	int		g__iDragBeginY;
	int		g__iMouseX;
	int		g__iMouseY;
	bool		g__LBUTTONDOWN;
	bool		g__RBUTTONDOWN;

	float		FOVY;

	bool		g__showBoundingSphere;
	bool		g__showBoundingBox;
#if 0	// group
	kID		g__gid;
	bool		g__showGroup;
#endif
#endif

#if 1
	POINT	g__startPoint;
	POINT	g__endPoint;


	bool	g__showShape;

	bool	g__showLowerFace;
	std::vector<DWORD>		lower_FaceID_array;
	std::vector<D3DXVECTOR3>	lower2_FaceID_array;
	std::vector<LOWER_FACE>		lowerFaceArray;

	bool	g__showContour;

	bool	g__showText;
	bool	g__drawAxis;
	bool	g__drawAxis0;
	bool	g__drawPivot;
#endif

#if 1	// Picking 된 점들을 관리하기 위하여...
	int	__nearestIntersectionIndex;	// 제일 먼저 충돌하는 삼각형에 대한 인덱스;
	// static 으로 처리하면 다시 OnCreateDevice 를 호출했을 때 초기값으로 설정할 수 없다.
	unsigned	__lastNumIntersection;	// 최근의 __numIntersection 값; 중복해서 계산하지 않도록
	INTERSECTION	__lastIntersection;	// 최근의 INTERSECTION; 서로 다른 INTERSECTION 에 대해서만 처리하도록
	unsigned	__numIntersection;
	int		__nextIntersectionIndex;
	INTERSECTION	__twoIntersection[2];
#if 0
public:
	unsigned&	lastNumIntersection()		{return __lastNumIntersection;}
	const unsigned	numIntersection() const		{return __numIntersection;}
	const int	nextIntersectionIndex() const	{return __nextIntersectionIndex;}
	const INTERSECTION*	twoIntersection() const	{return __twoIntersection;}
#endif
#endif

#ifndef __KEKO__
#if 1	// defined
	FILE*	filePtr;	// "defined"
#endif	// defined
#endif	// !__KEKO__
	virtual ~kState()
	{
#ifndef __KEKO__
		if (filePtr) {
			(void) fclose(filePtr);
			filePtr = 0;
		}
#endif	// !__KEKO__
	}
#if 1
	kState() :
#if 1	// 마지막 주문; 1. 회전축 고정
		fixed_axis(false),
#endif	// 마지막 주문; 1. 회전축 고정
#if 1	// 체형변화 시뮬레이션
		playback(false),
		playback_fps(5.f),	// 이 값이 클수록 playback 속도가 빠르다; 최소값 = 1
		playback_backward(true),
		playback_index(0),
#endif	// 체형변화 시뮬레이션
#if 1	// AREA2_HINT
		Area2Threshold(100),
#endif	// AREA2_HINT
		DoShift(false),
		SetGridCenter(false),
#ifdef __KEKO__
		scale_alpha(.4f),
#else	// __KEKO__/!__KEKO__
		scale_alpha(1.f),
#endif	// !__KEKO__
#if 1	// 모든 계측부위 표시
		displayAll(false),
#endif	// 모든 계측부위 표시
		doMeasure(false),
#if 1	// ColorBand
		ColorBandControl(false),
#endif	// ColorBand
//		g__colorMtrl(1, 1, 1, 1),
		g__colorMtrl(169.f / 255, 169.f / 255, 169.f / 255, 1),	// dark grey
#if 1	// Convex
		g__mode(TAPE_MEASURE_MODE_CONTOUR),
		g__show_all_convex_points(true),
		g__next_convex_index(-1),
#endif
		modelType(MODEL_TYPE__REAL),
		g__subset(false),
		g__pickStandard(false),
		g__follow(false),
		g__printBarycentricCoordinates(false),
		g__showDefinedMeasure(false),
//		printFaceIDList(false),
		g__showDefinedArea(false),
		g__showDefinedBodyElement(false),
//		printColor(false),
		g__showTemplateColor(false),
		g__nTessellation(0),
		g__nChannel(3),
		g__nStep(2),
		g__NSelectedPoints(0),
		g__lastPrID((DWORD) -1),
		g__prID((DWORD) -1),
		g__index((DWORD) -1),
		manipulator_mode(false),
		in_point_manipulator(false),
		__showSegment(true),
		g__bString(false),
		g__incompleteString(false),
		g__stringMeasure(false),
		g__stringMoveO(false),
		g__stringRotationAngleByAmount((FLOAT) (1. / 180. * D3DX_PI)),
		g__stringHeading(false),
		g__stringAttitude(false),
		g__stringLength(0.f),
		g__n(D3DXVECTOR3(0.f, 1.f, 0.f)),
		g__numSegments(0),
		g__stringClosed(false),
		g__stringError(false),
		g__stringContinueAtLastSegment(false),
		g__plus(false),
		g__minus(false),
		g__Alt(false),
		g__Shift(false),
		g__Control(false),
		g__bDrag(false),
		g__LBUTTONDOWN(false),
		g__RBUTTONDOWN(false),
		FOVY(D3DX_PI / 4),
		g__showBoundingSphere(false),
		g__showBoundingBox(false),
#if 0	// group
		g__showGroup(false),
#endif
		g__showShape(false),
		g__showLowerFace(false),
		g__showContour(false),
		g__showText(false),
		g__drawAxis(true),
		g__drawAxis0(false),
		g__drawPivot(false)
	{
		g__startPoint.x = -1;
		g__startPoint.x = -1;
		g__endPoint.x = -1;
		g__endPoint.y = -1;
#if 1	// Picking 된 점들을 관리하기 위하여...
		__nearestIntersectionIndex = -1;
		__lastNumIntersection = 0;
		__lastIntersection.dwFace = (DWORD) -1;
		__numIntersection = 0;
		__nextIntersectionIndex = 0;
#endif
#ifndef __KEKO__
#if 1	// defined
		filePtr = 0;
#if defined(_MSC_VER) && (_MSC_VER >= 1400)	// Visual Studio 2005 or above
		errno_t	err = fopen_s(&filePtr, "defined", "w");
#ifdef _DEBUG
		assert(err == 0);
#else
		if (err)
			throw "kState::kState: fopen_s";
#endif
#else
		filePtr = fopen("defined", "a");
#endif
#endif	// defined
#endif	// !__KEKO__
	}
#endif

#if 1	// 2D 와의 통신을 위한 함수
	int	getValue(TYPE_XXX type, int subType, void* dataPtr);
	int	setValue(TYPE_XXX type, int subType, const void* dataPtr);
#endif
};


#endif	// __kSTATE__
