#pragma once
#ifndef __kCOLOR_BAND__
#define __kCOLOR_BAND__
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
typedef enum {
	COLOR_BAND__TYPE,		// COLOR_BAND_TYPE_T 에 대한 포인터
	// dimension
	COLOR_BAND__DIMENSION,		// kColorBand::Dimension 에 대한 포인터
	// indicator
	COLOR_BAND__INDICATOR,		// kColorBand::IndexIndicator 에 대한 포인터
} COLOR_BAND__SUBTYPE;

typedef enum {
	COLOR_BAND_UNDEF = -1,
	COLOR_BAND_TOP_TO_BOTTOM = 0,	// 세로방향; 아래쪽: 1, 위쪽: 0
	COLOR_BAND_BOTTOM_TO_TOP,	// 세로방향; 아래쪽: 0, 위쪽: 1
	COLOR_BAND_LEFT_TO_RIGHT,	// 가로방향; 왼쪽: 0, 오른쪽: 1
	COLOR_BAND_RIGHT_TO_LEFT	// 가로방향; 왼쪽: 1, 오른쪽: 0
} COLOR_BAND_TYPE_T;

#define NUM_INDICATORS	15	// 3(하한, 상한, 값) * 5(좌우팔, 좌우다리, 몸통)
////////////////////////////////////////////////////////////////////////////////


#include <DXUT/Core/DXUT.h>
#include <mesh/kEnum.h>

class kColorBand : k2DInterfaceBase
{
public:
	struct Dimension
	{
		float	x_offset;	// 색깔띠 시작점(왼쪽, 위)의 왼쪽이 화면의 왼쪽에서 얼마나 벗어낫는가;
					// 화면의 크기는 상하, 좌우 모두 2로 취급된다(Projection Space)
		float	width;		// 색깔띠의 좌우길이;
					// 좌우길이와 COLOR_BAND__X_OFFSET 의 합이 2보다 작아야 한다.
		float	y_offset;	// 색깔띠 시작점의 위쪽이 화면의 위쪽에서 얼마나 벗어낫는가
		float	height;		// 색깔띠의 상하길이;
					// 상하길이와 COLOR_BAND__Y_OFFSET 의 합이 2보다 작아야 한다.
	
		// 기본형: COLOR_BAND_BOTTOM_TO_TOP
		Dimension() :
			x_offset(.1f),
			width(.1f),
			y_offset(.4f),
			height(.6f)
		{}
	};
	struct Indicator
	{
		float	f0;	// 색깔띠에 나타낼 표시선의 위치;
				// [0, 1]사이면 표시되고 그렇지 않으면 표시되지 않는다.
		float	width;	// 표시선 두께
		float	scale;	// 표시선의 길이(length)는 가로방향의 경우 COLOR_BAND__HEIGHT 에서 지정한 값이,
				// 세로방향의 경우 COLOR_BAND__WIDTH 에서 지정한 값이 사용된다(length0);
				// 이 길이를 여기서 지정한 값의 비율만큼 늘어난다:
				//	length = length0;
				//	length *= scale;
				// (음수라면 줄어든다.)
		D3DCOLOR	color;	// 색깔, D3DXCOLOR 에 대한 포인터

		Indicator() :
			f0(-1.f),
			width(2.f),
			scale(0.0f),
			color(D3DCOLOR_RGBA(0, 255, 0, 255))
		{}
	};
	struct IndexIndicator : Indicator
	{
		int	index;
	};
private:
	COLOR_BAND_TYPE_T	__type;
	// dimension
	Dimension	__dimension;
	// indicator
	Indicator	__indicators[NUM_INDICATORS];
public:
	kColorBand();
	virtual ~kColorBand();

public:
	// 2D 에서 __typeVal 를 참조할 때
	// override
	virtual int	getValue(int subType, void* dataPtr);
	virtual int	setValue(int subType, const void* dataPtr);

	// 이 클래스를 접근할 수 있는 곳에서 바로 참조할 때
	const COLOR_BAND_TYPE_T&	typeRef() const	{return __type;}
	COLOR_BAND_TYPE_T&		typeRef()	{return __type;}
	// dimension
	const Dimension&	dimensionRef() const	{return __dimension;}
	Dimension&		dimensionRef()		{return __dimension;}
	// indicator
	const Indicator*	indicators() const	{return __indicators;}
	Indicator*		indicators()		{return __indicators;}
};


////////////////////////////////////////////////////////////////////////////////
#endif	// !__kCOLOR_BAND__
