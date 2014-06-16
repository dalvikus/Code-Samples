#pragma once
#ifndef __kBODY_ELEMENT__
#define __kBODY_ELEMENT__

#include <DXUT/Core/DXUT.h>
#include <mesh/kEnum.h>
#include <mesh/kArea.h>

typedef enum {
	BODY_ELEMENT__RAW_VALUE = 0,	// BoCA.dll 로부터 읽어들인 값을 얻을 때;
					// 변수형: RAW_VALUE
	BODY_ELEMENT__VS,		// BoCA.dll 로부터 읽어들인 값을 사용해서 표준량(또는 적정량)과 비교한 수치;
					// 변수형: VS_VISIBLE
} BODY_ELEMENT__SUBTYPE;

	// 이 중에서 몇 개의 항목은 표준량과 측적량을 사용하여 비율을 정의할 수 있고,
	// 그 값이 허용범위 안에 있으면 측정값은 여전히 표준량이라고 생각된다:
	//	1) 체수분율
	//		정의 = 체수분량([2]) / 체중([0] + [1])
	//		허용범위 = 50% ~ 70%
	//	2) 세포외액비([9])
	//		정의 = 세포외액량([7])/체수분량([2])
	//		허용범위 = 30% ~ 35%
	//	3) 체지방률
	//		정의 = 체지방량([1]) / 체중([0] + [1])
	//		허용범위 = 10% ~ 20% (남), 18% ~ 28% (여)
#define NUM_BODY_ELEMENT_RAW_TYPE	52
	typedef enum {
		BODY_ELEMENT_RAW_EXCEPT_FAT		= 0,	// 제지방량:= 근육량 + 무기질량; 체중에서 지방을 뺀 양
		BODY_ELEMENT_RAW_FAT			= 1,	// 체지방량
		BODY_ELEMENT_RAW_BODY_FLUID		= 2,	// 체수분량:= 세포내액 + 세포외액
		BODY_ELEMENT_RAW_SOFT_LEAN_MASS		= 3,	// 근육량:= 체수분량 + 단백질량
		BODY_ELEMENT_RAW_MINERAL		= 4,	// 무기질량
		BODY_ELEMENT_RAW_PROTEIN		= 5,	// 단백질량
		BODY_ELEMENT_RAW_INTRACELLULAR_FLUID	= 6,	// 세포내액량
		BODY_ELEMENT_RAW_EXTRACELLULAR_FLUID	= 7,	// 세포외액량
		BODY_ELEMENT_RAW_INTRACELLULAR_FLUID_PERCENT	= 8,	// 세포내액비(%)
		BODY_ELEMENT_RAW_EXTRACELLULAR_FLUID_PERCENT	= 9,	// 세포외액비(%):= [7]/[2]

		BODY_ELEMENT_RAW_ABDOMEN_FAT_AREA		= 10,	// 복부지방면적
		BODY_ELEMENT_RAW_ABDOMEN_SUBCUTANEOUS_FAT_AREA	= 11,	// 피하지방면적
		BODY_ELEMENT_RAW_ABDOMEN_VISCERAL_FAT_AREA	= 12,	// 내장지방면적
		BODY_ELEMENT_RAW_ABDOMEN_FAT_RATIO		= 13,	// 복부지방률

		BODY_ELEMENT_RAW_RIGHT_ARM_FAT		= 14,	// 오른팔 체지방량
		BODY_ELEMENT_RAW_LEFT_ARM_FAT		= 15,	// 왼팔 체지방량
		BODY_ELEMENT_RAW_RIGHT_LEG_FAT		= 16,	// 오른다리 체지방량
		BODY_ELEMENT_RAW_LEFT_LEG_FAT		= 17,	// 왼다리 체지방량
		BODY_ELEMENT_RAW_TRUNK_FAT		= 18,	// 몸통 체지방량

		BODY_ELEMENT_RAW_RIGHT_ARM_BODY_FLUID	= 19,	// 오른팔 체수분량
		BODY_ELEMENT_RAW_LEFT_ARM_BODY_FLUID	= 20,	// 왼팔 체수분량
		BODY_ELEMENT_RAW_RIGHT_LEG_BODY_FLUID	= 21,	// 오른다리 체수분량
		BODY_ELEMENT_RAW_LEFT_LEG_BODY_FLUID	= 22,	// 왼다리 체수분량
		BODY_ELEMENT_RAW_TRUNK_BODY_FLUID	= 23,	// 몸통 체수분량

		BODY_ELEMENT_RAW_RIGHT_ARM_SOFT_LEAN_MASS	= 24,	// 오른팔 근육량
		BODY_ELEMENT_RAW_LEFT_ARM_SOFT_LEAN_MASS	= 25,	// 왼팔 근육량
		BODY_ELEMENT_RAW_RIGHT_LEG_SOFT_LEAN_MASS	= 26,	// 오른다리 근육량
		BODY_ELEMENT_RAW_LEFT_LEG_SOFT_LEAN_MASS	= 27,	// 왼다리 근육량
		BODY_ELEMENT_RAW_TRUNK_SOFT_LEAN_MASS		= 28,	// 몸통 근육량


		BODY_ELEMENT_RAW_IDEAL_RIGHT_ARM_FAT		= 29,	// 오른팔 체지방 적정량
		BODY_ELEMENT_RAW_IDEAL_LEFT_ARM_FAT		= 30,	// 왼팔 체지방 적정량
		BODY_ELEMENT_RAW_IDEAL_RIGHT_LEG_FAT		= 31,	// 오른다리 체지방 적정량
		BODY_ELEMENT_RAW_IDEAL_LEFT_LEG_FAT		= 32,	// 왼다리 체지방 적정량
		BODY_ELEMENT_RAW_IDEAL_TRUNK_FAT		= 33,	// 몸통 체지방 적정량

		BODY_ELEMENT_RAW_IDEAL_RIGHT_ARM_SOFT_LEAN_MASS	= 34,	// 오른팔 근육 적정량
		BODY_ELEMENT_RAW_IDEAL_LEFT_ARM_SOFT_LEAN_MASS	= 35,	// 왼팔 근육 적정량
		BODY_ELEMENT_RAW_IDEAL_RIGHT_LEG_SOFT_LEAN_MASS	= 36,	// 오른다리 근육 적정량
		BODY_ELEMENT_RAW_IDEAL_LEFT_LEG_SOFT_LEAN_MASS	= 37,	// 왼다리 근육 적정량
		BODY_ELEMENT_RAW_IDEAL_TRUNK_SOFT_LEAN_MASS	= 38,	// 몸통 근육 적정량

		BODY_ELEMENT_RAW_IDEAL_RIGHT_ARM_BODY_FLUID	= 39,	// 오른팔 체수분 적정량
		BODY_ELEMENT_RAW_IDEAL_LEFT_ARM_BODY_FLUID	= 40,	// 왼팔 체수분 적정량
		BODY_ELEMENT_RAW_IDEAL_RIGHT_LEG_BODY_FLUID	= 41,	// 오른다리 체수분 적정량
		BODY_ELEMENT_RAW_IDEAL_LEFT_LEG_BODY_FLUID	= 42,	// 왼다리 체수분 적정량
		BODY_ELEMENT_RAW_IDEAL_TRUNK_BODY_FLUID		= 43,	// 몸통 체수분 적정량

		BODY_ELEMENT_RAW_STANDARD_BODY_FLUID		= 44,	// 체수분 표준량
		BODY_ELEMENT_RAW_STANDARD_FAT			= 45,	// 체지방 표준량
		BODY_ELEMENT_RAW_STANDARD_EXCEPT_FAT		= 46,	// 제지방 표준량:= 근육 표준량 + 무기질 표준량
		BODY_ELEMENT_RAW_STANDARD_MINERAL		= 47,	// 무기질 표준량
		BODY_ELEMENT_RAW_STANDARD_PROTEIN		= 48,	// 단백질 표준량

		BODY_ELEMENT_RAW_SOFT_LEAN_MASS_CHANGE		= 49,	// 근육 조절량;
									// 근육 표준량 - 측정 근육량
									// ([46] - [47]) - [3]
		BODY_ELEMENT_RAW_FAT_CHANGE			= 50,	// 체지방 조절량; 체지방 표준량 - 측정 체지방량
									// [45] - [1]
		BODY_ELEMENT_RAW_IDEAL_WEIGHT			= 51,	// 적정 체중
	} BODY_ELEMENT_RAW_T;
	struct RAW_VALUE
	{
		BODY_ELEMENT_RAW_T	type;
		float			value;
	};

#define NUM_BODY_ELEMENT_VS_TYPE	15
	typedef enum {
		BODY_ELEMENT_VS_FAT_RIGHT_ARM			= 1 << 0,
		BODY_ELEMENT_VS_FAT_LEFT_ARM			= 1 << 1,
		BODY_ELEMENT_VS_FAT_RIGHT_LEG			= 1 << 2,
		BODY_ELEMENT_VS_FAT_LEFT_LEG			= 1 << 3,
		BODY_ELEMENT_VS_FAT_TRUNK			= 1 << 4,

		BODY_ELEMENT_VS_BODY_FLUID_RIGHT_ARM		= 1 << 5,
		BODY_ELEMENT_VS_BODY_FLUID_LEFT_ARM		= 1 << 6,
		BODY_ELEMENT_VS_BODY_FLUID_RIGHT_LEG		= 1 << 7,
		BODY_ELEMENT_VS_BODY_FLUID_LEFT_LEG		= 1 << 8,
		BODY_ELEMENT_VS_BODY_FLUID_TRUNK		= 1 << 9,

		BODY_ELEMENT_VS_SOFT_LEAN_MASS_RIGHT_ARM	= 1 << 10,
		BODY_ELEMENT_VS_SOFT_LEAN_MASS_LEFT_ARM		= 1 << 11,
		BODY_ELEMENT_VS_SOFT_LEAN_MASS_RIGHT_LEG	= 1 << 12,
		BODY_ELEMENT_VS_SOFT_LEAN_MASS_LEFT_LEG		= 1 << 13,
		BODY_ELEMENT_VS_SOFT_LEAN_MASS_TRUNK		= 1 << 14,
	} BODY_ELEMENT_VS_T;
	struct VS_VISIBLE
	{
		BODY_ELEMENT_VS_T	type;
		bool			visible;
	};

struct kBodyElement : k2DInterfaceBase
{
	float	__values[NUM_BODY_ELEMENT_RAW_TYPE];
	kArea	__area;
	int	__vsValue;	// 어떤 부위를 색칠할지를 결정한다.

	kBodyElement();
#if 1
	void	SetMaterialColor(const D3DXCOLOR& colorMtrl)	{__area.SetMaterialColor(colorMtrl);}
#endif

	//	1) 체수분율
	//		정의 = 체수분량([2]) / 체중([0] + [1])
	//		허용범위 = 50% ~ 70%
	//	2) 세포외액비([9])
	//		정의 = 세포외액량([7])/체수분량([2])
	//		허용범위 = 30% ~ 35%
	//	3) 체지방률
	//		정의 = 체지방량([1]) / 체중([0] + [1])
	//		허용범위 = 10% ~ 20% (남), 18% ~ 28% (여)
//	float	getBodyFluidRatio() const;

	// 2D 에서 __view, __rotation 를 참조할 때
	// override
	virtual int	getValue(int subType, void* dataPtr);
	virtual int	setValue(int subType, const void* dataPtr);

	// 이 클래스를 접근할 수 있는 곳에서 바로 참조할 때
	const float*	getValuesRef() const	{return __values;}
	float*		getValuesRef()		{return __values;}
	const int&	getVsValueRef() const	{return __vsValue;}
	int&		getVsValueRef()		{return __vsValue;}
	const kArea&	getAreaRef() const	{return __area;}
	kArea&		getAreaRef()		{return __area;}
};
#endif	// !__kBODY_ELEMENT__
