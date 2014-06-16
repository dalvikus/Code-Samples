#pragma once
#ifndef __kAREA__
#define __kAREA__


#include <DXUT/Core/DXUT.h>
#include <mesh/kEnum.h>
#include <set>
#include <map>
#include <vector>


#define NUM_AREA_TYPE	15	// 미리 정해진 영역의 갯수
#if 0
typedef enum {
	AREA__TRUNK		= 1 << 0,	// 몸통
	AREA__LEFT_ARM		= 1 << 1,	// 왼쪽 팔
	AREA__RIGHT_ARM		= 1 << 2,	// 오른쪽 팔
	AREA__LEFT_LEG		= 1 << 3,	// 왼쪽 다리
	AREA__RIGHT_LEG		= 1 << 4,	// 오른쪽 다리

	AREA__NECK		= 1 << 5,	// 목
	AREA__CHEST		= 1 << 6,	// 가슴
	AREA__ABDOMINAL		= 1 << 7,	// 복부
	AREA__HIP		= 1 << 8,	// 엉덩이
	AREA__LEFT_FORE_ARM	= 1 << 9,	// 왼쪽 팔뚝(상완)
	AREA__RIGHT_FORE_ARM	= 1 << 10,	// 오른쪽 팔뚝(상완)
	AREA__LEFT_THIGH	= 1 << 11,	// 왼쪽 허벅지
	AREA__RIGHT_THIGH	= 1 << 12,	// 오른쪽 허벅지
	AREA__LEFT_CALF		= 1 << 13,	// 왼쪽 장단지
	AREA__RIGHT_CALF	= 1 << 14,	// 오른쪽 장단지


	AREA__SET_COLOR		= 1 << 31,	// 특정 부위의 색깔을 지정할 때;
						// 변수형: AREA_TYPE_COLOR
} AREA__TYPE;
#endif	//

typedef enum {
	AREA__TYPE	= 0,	// AREA_TYPE_VISIBLE 에 대한 포인터
	AREA__COLOR,		// AREA_TYPE_COLOR 에 대한 포인터
} AREA__SUBTYPE;

typedef enum {
	AREA_TRUNK		= 1 << 0,	// 몸통
	AREA_LEFT_ARM		= 1 << 1,	// 왼쪽 팔
	AREA_RIGHT_ARM		= 1 << 2,	// 오른쪽 팔
	AREA_LEFT_LEG		= 1 << 3,	// 왼쪽 다리
	AREA_RIGHT_LEG		= 1 << 4,	// 오른쪽 다리

	AREA_NECK		= 1 << 5,	// 목
	AREA_CHEST		= 1 << 6,	// 가슴
	AREA_ABDOMINAL		= 1 << 7,	// 복부
	AREA_HIP		= 1 << 8,	// 엉덩이
	AREA_LEFT_FORE_ARM	= 1 << 9,	// 왼쪽 팔뚝(상완)
	AREA_RIGHT_FORE_ARM	= 1 << 10,	// 오른쪽 팔뚝(상완)
	AREA_LEFT_THIGH		= 1 << 11,	// 왼쪽 허벅지
	AREA_RIGHT_THIGH	= 1 << 12,	// 오른쪽 허벅지
	AREA_LEFT_CALF		= 1 << 13,	// 왼쪽 장단지
	AREA_RIGHT_CALF		= 1 << 14,	// 오른쪽 장단지
} AREA_TYPE_T;
#if 0
friend ostream&	operator<<(ostream& os, const AREA_TYPE_T& type)
{
	return os << (
		type == AREA_TRUNK ? "Trunk" : (
		type == AREA_LEFT_ARM ? "Left Arm" : (
		type == AREA_RIGHT_ARM ? "Right Arm" : (
		type == AREA_LEFT_LEG ? "Left Leg" : (
		type == AREA_RIGHT_LEG ? "Right Leg" : (
		type == AREA_NECK ? "Neck" : (
		type == AREA_CHEST ? "Chest" : (
		type == AREA_ABDOMINAL ? "Abdominal" : (
		type == AREA_HIP ? "Hip" : (
		type == AREA_LEFT_FORE_ARM ? "Left Forearm" : (
		type == AREA_RIGHT_FORE_ARM ? "Right Forearm" : (
		type == AREA_LEFT_THIGH ? "Left Thigh" : (
		type == AREA_RIGHT_THIGH : "Right Thigh" : (
		type == AREA_LEFT_CALF : "Left Calf" : (
		type == AREA_RIGHT_CALF : "Right Calf" : (
		"Unknown"
	))))))))))))))));
}
#endif


struct AREA_TYPE_VISIBLE {
	AREA_TYPE_T	type;
	bool		visible;
};

struct AREA_TYPE_COLOR
{
	AREA_TYPE_T	type;
	float		fColor;
};

struct AREA_FACEID_LIST
{
	int	n;
	DWORD*	ptr;
};
struct AREA_TYPE_DEF
{
	AREA_TYPE_T	type;
	AREA_FACEID_LIST	faceIDList;
};
struct AREA_TYPE_DEF_LIST
{
	int	n;
	AREA_TYPE_DEF*	ptr;
};

class kMesh;
struct kArea : k2DInterfaceBase
{
	struct DEFINED_AREA
	{
		float	value;
		std::set<DWORD>	face_id_set;
	};
	typedef std::map<AREA_TYPE_T, DEFINED_AREA>	DEFINED_AREA_DIC;


	int	__typeVal;
#if 1
	int	__allSetVal;	// __typeVal 에 해당하는 영역이 색칠해졌는 지 말해준다.
#endif
	float	__colorArray[NUM_AREA_TYPE];	// 각 영역에 해당하는 색깔;
						// [0, 1] 안에 있으면 주어진 색깔을,
						// 그렇지 않으면 미리 정해진 색깔을 사용한다.
						// 주어진 AREA__TYPE 에 해당하는 인덱스는
						// kArea::getColorIndex 함수를 이용하여 구한다.

	static int	getColorIndex(AREA_TYPE_T areaType);

#if 1
	D3DXCOLOR	__colorMtrl;	// 모델에 입힐 초기 색깔
	void	SetMaterialColor(const D3DXCOLOR& colorMtrl)	{__colorMtrl = colorMtrl;}
#endif
	kArea();

	// 2D 에서 __typeVal 를 참조할 때
	// override
	virtual int	getValue(int subType, void* dataPtr);
	virtual int	setValue(int subType, const void* dataPtr);

	// 이 클래스를 접근할 수 있는 곳에서 바로 참조할 때
	const int&	getTypeValRef() const	{return __typeVal;}
	int&		getTypeValRef()		{return __typeVal;}
	const float*	getColorArrayRef() const	{return __colorArray;}
	float*		getColorArrayRef()		{return __colorArray;}

	// 렌더링하기위해 __typeVal 와 __colorArray 를 이용하여 Mesh 에 색깔을 칠한다.
	int	setColor(const kMesh& mesh);
};


#endif	// !__kAREA__
