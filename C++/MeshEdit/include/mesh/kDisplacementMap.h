#pragma once
#ifndef __kDISPLACEMENT_MAP__
#define __kDISPLACEMENT_MAP__


#include <Windows.h>
#include <GdiPlus.h>
#include <iostream>
	using std::cout;
	using std::cerr;
	using std::endl;
#include <cmath>
#include <cassert>

// GDI+ 사용하려면 반드시 이 클래스의 Instance 를 하나 만들어야 한다; 예를 들어, kState 에서...
class kGdiPlus
{
private:
	Gdiplus::GdiplusStartupInput	__gdiplusStartupInput;
	ULONG_PTR	__gdiplusToken;
public:
	kGdiPlus() {GdiplusStartup(&__gdiplusToken, &__gdiplusStartupInput, NULL);}
	~kGdiPlus()	{Gdiplus::GdiplusShutdown(__gdiplusToken);}
};


class kDisplacementMap : public Gdiplus::Bitmap
{
public:
	typedef enum {
		CHANNEL_RED,
		CHANNEL_GREEN,
		CHANNEL_BLUE,
		CHANNEL_ALPHA
	} CHANNEL_COLOR;
protected:
	int	__width;
	int	__height;
	int	__max_red;
	int	__max_green;
	int	__max_blue;
	int	__max_alpha;
public:
	int	Width() const	{return __width;}
	int	Height() const	{return __height;}
	int	MaxRed() const	{return __max_red;}
	int	MaxGreen() const	{return __max_green;}
	int	MaxBlue() const	{return __max_blue;}
	int	MaxAlpha() const	{return __max_alpha;}
public:
	kDisplacementMap(const WCHAR* mapFile);
	virtual ~kDisplacementMap();

public:
	// (u, v) 에 해당하는 Pixel 의 값은?
	//	Displacement Map 에서 (왼쪽, 아래) 가 (u = 0, v = 0) 에 해당하고,
	//	(오른쪽, 위) 가 (u = 1, v = 1) 에 해당한다.
	//	반면에 Bitmap 에서 (왼쪽, 위) 가 (x = 0, y = 0) 에 해당하고,
	//	(오른쪽, 아래) 가 (x = width - 1, y = height - 1) 에 해당한다.
	//	즉 y 값의 증가방향이 반대이다.
	//
	//	(x, y): Displacement Map 에서의 좌표	x: [0, width), y: [0, height)
	//	(X, Y): Bitmap 에서의 좌표		X: [0, width), Y: [0, height)
	//
	//	라 하면 다음 관계가 성립한다.
	//		X = x
	//		Y = (height - 1) - y	(식1)
	//
#if 0	// kMeshLoader::__LoadGeometryFromOBJ()...
#if 1	// 에트리 모델: Right-handed (Cartesian) Coordinate System; 텍스쳐 사용
// DirectX 에서는 OpenGL 과 달리 텍스쳐의 v 방향이 위에서 아래로 향한다.
		vt.y = 1.f - vt.y;
#endif
#endif
	Gdiplus::Status	GetPixelAtUV(float u, float v, float& val, CHANNEL_COLOR channel = CHANNEL_ALPHA, Gdiplus::Color* color_ptr_at_floored_uv = NULL) const;
};


#endif	// !__kDISPLACEMENT_MAP__
