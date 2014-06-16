#pragma once
#ifndef __kSIMULATION__
#define __kSIMULATION__


#include <DXUT/Core/DXUT.h>
#include <vector>
#include <mesh/kEnum.h>
#include <mesh/kArea.h>

typedef enum {
	SIMULATION__METHOD,		// 성형 방법을 정할 때;
					// 변수형: SIMULATION_TYPE

	SIMULATION__STANDARD,		// 표준체형으로 변형할 때;
					// 변수형: STANDARD_SIMULATION
	SIMULATION__MANUAL,		// (선택한 성형 부위를) 임의 조작할 때;
					// 변수형: MANUAL_SIMULATION
	SIMULATION__TEMPLATE,		// 가슴 성형의 경우 template 을 사용할 때;
					// 변수형: BREAST_IMPLANT_TEMPLATE

#if 1
	SIMULATION__EXEC_METHOD,	// 실행방법
					// 변수형: SIMULATION_EXEC_METHOD_VALUE
#else
	SIMULATION__CC_TYPE,		// 주어진 cc 만큼 성형할 것인지 아니면 성형 후 결과를 얻을 것인지를 정할 때;
					// 변수형: SIMULATION_CC_TYPE
	SIMULATION__STEP_TYPE,		// 시뮬레이션이 단계적으로 일어날 때, 어떤 단계로 일어날지를 정할 때
					// 변수형: SIMULATION_STEP_TYPE
	SIMULATION__CC_STEP_SIZE,	// 시뮬레이션을 단계적으로 실행할 때 각 단계에서 사용할 cc 의 양을 정할 때;
					// 변수형: float
	SIMULATION__NORMAL_AMOUNT_STEP_SIZE,	// 시뮬레이션을 단계적으로 실행할 때 각 단계에서 사용할 수직길이의 양을 정할 때;
					// 변수형: float
#endif
	SIMULATION__VOLUME_CHANGE,	// 시뮬레이션 전후의 부피 변화량;
#if 1
					// 읽을 때(getValue)에서는 __volume_change_cc 값을 돌려준다.
					//	주) 읽기만 가능
#else
					// 읽을 때(getValue)에서는 __volume_change_cc 값을,
					// 쓸 때(setValue)에서는 __final_cc 값을 사용한다.
#endif

	SIMULATION__EXEC,		// 시뮬레이션을 실행한다;
					// 변수형: SIMULATION_EXEC_TYPE
} SIMULATION__SUBTYPE;
/*
 * 1. 표준체형으로의 변환
 *	SIMULATION_TYPE	method = SIMULATION_TYPE_STANDARD;
 *	setValue(TYPE__SIMULATION, SIMULATION__METHOD, &method);
 *
 *	1) 부위 설정
 *	STANDARD_SIMULATION	standard;	// volume 은 여기서 사용하지 않는다.
 *	standard.area = AREA__ABDOMINAL;	// 복부
 *	setValue(TYPE__SIMULATION, SIMULATION__STANDARD, &standard);
 *	1-1) 표준체형으로 갔을 때 총 변화량
 *	float&	volumeChange = standard.volume;	// 양수:= 살을 쪄야 한다, 음수: 살을 빼야 한다.
 *	getValue(TYPE__SIMULATION, SIMULATION__STANDARD, &standard);
 *
 *******************************************************************************
 *	2) 성형 방법 설정; 어떻게...(by cc or by normal amount)
 *	2-1) 어떤 식으로 할 것인지: SIMULATION__CC_TYPE
 *	// 가) 정해진 양만큼 변화한다.
 *	// 방법 설정
 *	SIMULATION_CC_TYPE	ccType = SIMULATION_CC_TYPE_FINAL;	// 최종 부피
 *	// 정해진 양 설정;
 *	//	표준체형으로 갔을 때의 총 변화량보다 작거나 클 수 없다;
 *	//	앞의 총 변화량의 절대값보다는 작아야 하고, 부호는 총 변화량과 같아야 한다.
 *	float	volumeChange = 10.f;
 *	setValue(TYPE__SIMULATION, SIMULATION__VOLUME_CHANGE, &float);
 *	// 실행
 *	SIMULATION_EXEC_TYPE	execType = SIMULATION_EXEC_DONE;
 *	setValue(TYPE__SIMULATION, SIMULATION__EXEC, &execType);
 *
 *	// 나) 원하는 만큼 변화를 주면서
 *	// 방법 설정
 *	SIMULATION_CC_TYPE	ccType = SIMULATION_CC_TYPE_VOLUME_CHANGE;
 *	// a) 구체적으로 일정한 양만큼씩 아니면 일정한 비율만큼씩
 *	// 일정한 양만큼씩
 *	SIMULATION_STEP_TYPE	stepType = SIMULATION_STEP_TYPE_CC;
 *	setValue(TYPE__SIMULATION, SIMULATION__STEP_TYPE, &stepType);
 *	float	cc_step_size = .1f;
 *	setValue(TYPE__SIMULATION, SIMULATION__CC_STEP_SIZE, &cc_step_size);
 *	// 실행
 *	SIMULATION_EXEC_TYPE	execType = SIMULATION_EXEC_PLUS_STEP;	// SIMULATION_EXEC_MINUS_STEP
 *	setValue(TYPE__SIMULATION, SIMULATION__EXEC, &execType);
 *	// b) 일정한 비율로
 *	SIMULATION_STEP_TYPE	stepType = SIMULATION_STEP_TYPE_RATIO;
 *	setValue(TYPE__SIMULATION, SIMULATION__STEP_TYPE, &stepType);
 *	float	normal_amount_size = .1;
 *	setValue(TYPE__SIMULATION, SIMULATION__NORMAL_AMOUNT_STEP_SIZE, &normal_amount_size);
 *	// 실행
 *	SIMULATION_EXEC_TYPE	execType = SIMULATION_EXEC_PLUS;	// SIMULATION_EXEC_MINUS
 *	setValue(TYPE__SIMULATION, SIMULATION__EXEC, &execType);
 *
 *	// 변화된 부피는?
 *	float	volumeChange;
 *	getValue(TYPE__SIMULATION, SIMULATION__VOLUME_CHANGE, &volumeChange);
 *******************************************************************************
실행방법 테이블
---------------

		 		한번에(가)		조금씩(나)^
						일정 양만큼(a)	일정 비율로(b)
STANDARD			o		o		o
TEMPLATE			o		o		o
MANUAL(BY COLOR ISOMETRIC)*	o		o		o
MANUAL(DIRECTLY)**		N/A		N/A		o

^: +/- 가 가능하다.
*: 실행하기 전에 색깔을 브러쉬로 칠해야 한다.
**: 실행단계(SIMULATION__EXEC)가 필요하지 않다.

 *******************************************************************************
 *
 * 2. 템플릿 사용
 *	SIMULATION_TYPE	method = SIMULATION_TYPE_TEMPLATE;
 *	setValue(TYPE__SIMULATION, SIMULATION__METHOD, &method);
 *
 *	// 적절하게 템플릿을 설정
 *	BREAST_IMPLANT_TEMPLATE	implantTemplate;
 *	implantTemplate.implantType = BREAST_IMPLANT_TYPE_A;
 *	implantTemplate.positionType = MODEL_POINT_LEFT_UP;
 *	setValue(TYPE__SIMULATION, SIMULATION__TEMPLATE, &implantTemplate);
 *
 *	// 어떻게...(by cc or by normal amount)
 *	// 1.의 2) 성형 방법 설정과 동일
 *
 * 3. 임의조작
 *	SIMULATION_TYPE	method = SIMULATION_TYPE_MANUAL;
 *	setValue(TYPE__SIMULATION, SIMULATION__METHOD, &method);
 *
 *	// 부위 설정과 조작 방법 설정: MANUAL_SIMULATION
 *	MANUAL_SIMULATION	manual;
 *	manual.area = AREA__ABDOMINAL;
 *	manual.type = MANUAL_BY_COLOR_ISOMETRIC;
 *	setValue(TYPE__SIMULATION, SIMULATION__MANUAL, &manual);
 *	// (브러쉬 모드로 변환)
 *
 *	// 어떻게... (by cc or by normal amount)
 *	// 1.의 2) 성형 방법 설정과 동일;
 *	// 다만 브러쉬 보드로 들어가는 시점이 실제로는 여기서(실행 단계) 이루어진다.
 *	// 실행단계에 들어가기 전에 MANUAL_BY_COLOR_ISOMETRIC 인 경우 브러쉬로 색깔을 설정해야 한다.
 *	// MANUAL_DIRECTLY 인 경우, 가)의 방법과  나)의 a) 방법은 사용할 수 없다.
 */
/*
 * GUI 구성
 * --------
 *
 *
 * 시뮬레이션 방법:
 *	RADIO: 표준체형, 템플릿, 임의조작
 * 부위: 표준체형, 임의조작을 선택한 경우
 *	RADIO: 가슴, 배, 허벅지, 엉덩이, 팔뚝
 * 템플릿 설정: 템플릿을 선택한 경우
 * 	보형물 종류: SELECT
 * 	보형물 삽입 위치: SELECT
 * 임의조작을 선택한 경우
 *	방법: RADIO: 직접, 색깔 등고선을 칠한 후
 * 표준체형을 선택한 경우
 *	결과보기: BUTTON
 *	결과값: TEXT
 *
 * 성형 방법:
 *	RADIO: 한번에, 단계적으로
 * "한번에"를 선택한 경우
 *	전체 부피: TEXT
 * "단계적으로"를 선택한 경우
 *	방법: RADIO: 일정양, 일정비율
 *	값(일정양/일정비율): TEXT
 *
 * 실행:
 *	BUTTON(DONE); 한번에
 *	BUTTON(+), BUTTON(-): 단계적으로
 *	결과보기: BUTTON, 결과값: TEXT
 * 주) 임의조작에서 MANUAL_DIRECTLY 인 경우에는 마지막 결과보기만 있음.
 */

	// 시뮬레이션 방법
	typedef enum {
		SIMULATION_TYPE_MANUAL,		// 임의 조작
		SIMULATION_TYPE_TEMPLATE,	// 템플릿 사용
		SIMULATION_TYPE_STANDARD,	// 표준 체형으로 변형
	} SIMULATION_TYPE;

	// 표준 체형으로 변형(SIMULATION_TYPE_STANDARD)에 사용될 구조:
	struct STANDARD_SIMULATION {
		AREA_TYPE_T	area;
		float		volume;	// 표준체형으로 변환했을 때, 부피의 총 변화량;
					// SIMULATION__SUBTYPE = SIMULATION__STANDARD_VOLUME_CHANGE 를 사용하여,
					// 값을 설정하고(setValue),
					// 값을 얻는다(getValue).
	};

	// 임의 조작(SIMULATION_TYPE_MANUAL)에 사용될 구조:
	//	MANUAL_SIMULATION
	typedef enum {
		MANUAL_DIRECTLY,		// 브러쉬로 직접 칠하면서 변형시킨다; cc 는 변화량
		MANUAL_BY_COLOR_ISOMETRIC,	// 먼저 색깔을 칠한 후 주어진 cc 만큼 변형시킨다;
						// SIMULATION_TYPE_STANDARD 의 경우에는 항상 MANUAL_BY_COLOR_ISOMETRIC
	} MANUAL_TYPE;
	struct MANUAL_SIMULATION {
		MANUAL_TYPE	type;
		AREA_TYPE_T	area;
//		float		cc;	// 시뮬레이션 최종 cc/변화된 양
#if 0	// C2620: A union member cannot have a default constructor.
		MANUAL_SIMULATION() : type(MANUAL_BY_COLOR_ISOMETRIC), area(AREA__ALL), cc(0.f) {}
#endif
	};

	// 템플릿을 사용한 조작(SIMULATION_TYPE_TEMPLATE)에 쓰이는 구조:
	//	BREAST_IMPLANT_TEMPLATE
#define N_BREAST_IMPLANT_TYPE	3	// BREAST_IMPLANT_TYPE 의 갯수
	typedef enum {
		BREAST_IMPLANT_TYPE_A = 0,	// BREAST_IMPLANT_TEMPLATE::ColorArray 을 접근할 때 사용되는 인덱스
		BREAST_IMPLANT_TYPE_B,
		BREAST_IMPLANT_TYPE_C,
	} BREAST_IMPLANT_TYPE;
#define N_MODEL_POINT_TYPE	2	// MODEL_POINT_TYPE 의 갯수
	typedef enum {
		MODEL_POINT_LEFT_UP = 0,	// BREAST_IMPLANT_TEMPLATE::PositionArray 을 접근하는
						// 인덱스로 사용해야 하므로 0부터 시작한다.
		MODEL_POINT_RIGHT_UP,
	} MODEL_POINT_TYPE;
	struct MODEL_COLOR {
		// FACE_ID_LOCAL_INDEX
		DWORD	faceID;
		int	localIndex;

		float	fColorRed;
		float	fColorGreen;
		float	fColorBlue;
	};
	typedef std::vector<MODEL_COLOR>	TEMPLATE_COLOR;
struct MODEL_COLOR_DEF {
	int	n;
	MODEL_COLOR*	ptr;
};
struct MODEL_COLOR_DEF_LIST {
	int	n;
	MODEL_COLOR_DEF*	ptr;
};
	struct MODEL_POINT
	{
		DWORD	ID;
		float	U;
		float	V;
	};
	struct BREAST_IMPLANT_TEMPLATE
	{
		BREAST_IMPLANT_TYPE	implantType;
		MODEL_POINT_TYPE	positionType;

		static const MODEL_POINT	PositionArray[N_MODEL_POINT_TYPE];
//		static TEMPLATE_COLOR		ColorArray[N_BREAST_IMPLANT_TYPE * N_MODEL_POINT_TYPE];
		static TEMPLATE_COLOR		ColorArray[N_MODEL_POINT_TYPE][N_BREAST_IMPLANT_TYPE];
						//	int	iImplantType;
						//	int	iPositionType;
						// 다음 순서로 색깔을 채운다; kMesh::setAll 의
						// BREAST_IMPLANT_TEMPLATE::ColorArray 를 설정 참고
						//	부위 색깔	(iImplantType, iPositionType; ColorArray 인덱스)
						//	---------	-----------------------------
						//	LEFT_UP_A	(0, 0; 0 = 3 * 0 + 0)
						//	LEFT_UP_B	(1, 0; 1 = 3 * 0 + 1)
						//	LEFT_UP_C	(2, 0; 2 = 3 * 0 + 2)
						//	RIGHT_UP_A	(0, 1; 3 = 3 * 1 + 0)
						//	RIGHT_UP_B	(1, 1; 4 = 3 * 1 + 1)
						//	RIGHT_UP_C	(2, 1; 5 = 3 * 1 + 2)
						// N_BREAST_IMPLANT_TYPE * iPositionType + iImplantType 이 인덱스
	};

	typedef enum {
		SIMULATION_EXEC_METHOD_FINAL,		// 한 번에
		SIMULATION_EXEC_METHOD_STEP_CC,		// 단계적으로 일정량
		SIMULATION_EXEC_METHOD_STEP_RATIO,	// 단계적으로 일정비율
	} SIMULATION_EXEC_METHOD_T;
	struct SIMULATION_EXEC_METHOD_VALUE {
		SIMULATION_EXEC_METHOD_T	type;
		float				val;
	};

	typedef enum {
		SIMULATION_STEP_TYPE_CC,	// 일정 양만큼
		SIMULATION_STEP_TYPE_RATIO,	// 일정 비율로
	} SIMULATION_STEP_TYPE;

	typedef enum {
		SIMULATION_CC_TYPE_FINAL,		// 한번에
		SIMULATION_CC_TYPE_VOLUME_CHANGE,	// 단계적으로
	} SIMULATION_CC_TYPE;

	// 실행 방법
	typedef enum {
		SIMULATION_EXEC_UNDEF,		// 시뮬레이션을 멈춘다.
		SIMULATION_EXEC_BY_BRUSH,	// 임의조작에서 브러쉬로 시뮬레이션한다;
						// Subtype 이 SIMULATION__MANUAL 인 경우에 설정한다.
		SIMULATION_EXEC_DONE,		// 주어진 cc 까지 시뮬레이션을 마무리한다.
#if 1
		// __stepType 을 통해 PLUS 인지 PLUS_STEP 인지 알 수 있다.
#else
		SIMULATION_EXEC_PLUS_STEP,	// 정해진 cc 만큼 시뮬레이션을 통해 선택한 부위를 늘인다.
		SIMULATION_EXEC_MINUS_STEP,	// 정해진 cc 만큼 시뮬레이션을 통해 선택한 부위를 줄인다.
#endif
		SIMULATION_EXEC_PLUS,		// 정해진 수직길이만큼 시뮬레이션을 통해 선택한 부위를 늘인다.
		SIMULATION_EXEC_MINUS,		// 정해진 수직길이만큼 시뮬레이션을 통해 선택한 부위를 줄인다.
	} SIMULATION_EXEC_TYPE;


class kSimulation : k2DInterfaceBase
{
private:
	SIMULATION_TYPE	__type;	// 성형 방법:
				//	임의 조작(SIMULATION_TYPE_MANUAL)
				//	템플릿 사용(SIMULATION_TYPE_TEMPLATE)
				//	표준 체형으로 변형(SIMULATION_TYPE_STANDARD)
	union {
		STANDARD_SIMULATION	__standard;
		MANUAL_SIMULATION	__manual;	// 임의 조작에 의한 특정 부위 성형:
							//	SIMULATION_TYPE_MANUAL, SIMULATION_TYPE_STANDARD
		BREAST_IMPLANT_TEMPLATE	__template;	// 템플릿을 사용한 가슴 성형:
							//	SIMULATION_TYPE_TEMPLATE
	};

	SIMULATION_CC_TYPE	__ccType;
	union {
		float	__final_cc;		// 시뮬레이션 최종 cc; __ccType = SIMULATION_CC_TYPE_FINAL
		float	__volume_change_cc;	// 시뮬레이션 전후의 변화량; __ccType = SIMULATION_CC_TYPE_VOLUME_CHANGE
	};

	SIMULATION_STEP_TYPE	__stepType;
	union {
		float	__cc_step;		// 시뮬레이션을 단계별로 보여줄 때 각 단계에 해당하는 cc
		float	__normal_amount_step;	// 시뮬레이션을 단계별로 보여줄 때 각 단계에 해당하는 수직길이
	};
	float	__fVolumeChange0;	// 시뮬레이션 방법을 설정하기 전의 kMesh::m__fVolumeChange 값;
					// 시뮬레이션을 통해 메쉬를 변형하면 kMesh::m__fVolumeChange 값이 바뀐다.
					// 실제도 이번 시뮬레이션에서 바뀐 양은 두 값의 차이이다:
					//	kMesh::m__fVolumeChange - __fVolumeChange0

	SIMULATION_EXEC_TYPE	__execType;	// 시뮬레이션 실행 방법

private:
	bool	__isReady;	// 시뮬레이션을 할 준비가 되었는 지 말해준다;
				// 즉 색깔을 적절하게 설정했는 지 말해준다.


public:
	kSimulation();


	// 2D와의 연동함수에서...
public:
	// override
	virtual int	getValue(int subType, void* dataPtr);
	virtual int	setValue(int subType, const void* dataPtr);

	// 이 클래스를 접근할 수 있는 곳에서 바로 참조할 때
public:
	const SIMULATION_TYPE&		getTypeRef() const		{return __type;}
	SIMULATION_TYPE&		getTypeRef()			{return __type;}
	const STANDARD_SIMULATION&	getStandardRef() const		{return __standard;}
	STANDARD_SIMULATION&		getStandardRef()		{return __standard;}
	const MANUAL_SIMULATION&	getManualRef() const		{return __manual;}
	MANUAL_SIMULATION&		getManualRef()			{return __manual;}
	const BREAST_IMPLANT_TEMPLATE&	getTemplateRef() const		{return __template;}
	BREAST_IMPLANT_TEMPLATE&	getTemplateRef()		{return __template;}
	const float&			getCcStepRef() const		{return __cc_step;}
	float&				getCcStepRef()			{return __cc_step;}
	const float&			getNormalAmountStepRef() const	{return __normal_amount_step;}
	float&				getNormalAmountStepRef()	{return __normal_amount_step;}
	const SIMULATION_EXEC_TYPE&	getExecTypeRef() const		{return __execType;}
	SIMULATION_EXEC_TYPE&		getExecTypeRef()		{return __execType;}
	const bool&			getIsReadyRef() const		{return __isReady;}
	bool&				getIsReadyRef()			{return __isReady;}
	const float&			getFinalCcRef() const		{return __final_cc;}
	float&				getFinalCcRef()			{return __final_cc;}
	const float&			getVolumeChangeCcRef() const	{return __volume_change_cc;}
	float&				getVolumeChangeCcRef()		{return __volume_change_cc;}
	const SIMULATION_CC_TYPE&	getCcTypeRef() const		{return __ccType;}
	SIMULATION_CC_TYPE&		getCcTypeRef()			{return __ccType;}
	const SIMULATION_STEP_TYPE&	getStepTypeRef() const		{return __stepType;}
	SIMULATION_STEP_TYPE&		getStepTypeRef()		{return __stepType;}
	const float&			getVolumeChange0Ref() const	{return __fVolumeChange0;}
	float&				getVolumeChange0Ref()		{return __fVolumeChange0;}
};


#endif	// !__kSIMULATION__
