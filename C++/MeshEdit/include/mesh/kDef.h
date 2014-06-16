#pragma once
#ifndef __kDEF__
#define __kDEF__
////////////////////////////////////////////////////////////////////////////////


#include <Windows.h>	// DWORD, FLOAT, ...
#include <d3dx9math.h>	// D3DXVECTOR3


struct LOWER_FACE {
	D3DXVECTOR3	p;
	D3DXVECTOR3	n;
	FLOAT		tu, tv, tx;
	D3DXVECTOR3	c;
};

struct INTERSECTION
{
	DWORD	dwFace;			// mesh face that was intersected
	FLOAT	fBary1, fBary2;		// barycentric coords of intersection
	FLOAT	fDist;			// distance from ray origin to intersection
	FLOAT	tu, tv;			// texture coords of intersection

	INTERSECTION() : dwFace((DWORD) -1), fBary1(0.f), fBary2(0.f) {}
	bool	operator!=(const INTERSECTION& other) const	{return this->dwFace != other.dwFace || this->fBary1 != other.fBary1 || this->fBary2 != other.fBary2 ? true : false;}
	bool	operator==(const INTERSECTION& other) const	{return !(*this != other);}
};

// 길이를 잴 때, 각 세그먼트를 이루는 두 점의 정보를 확장한다;
// D3DXVECTOR3 에서 SEGMENT_VERTEX 로
struct SEGMENT_VERTEX
{
	D3DXVECTOR3	P;	// 기존의 값(위치)
#if 1	// 뼈
	D3DXVECTOR3	P0;	// 기존의 원래 값(위치)
#endif	// 뼈
	D3DXVECTOR3	N;	// P 에서의 노말

#if 1	// SEGMENT_VERTEX::{EdgeID,VertexID,FaceID}
	// TWO_POINT_PLANE_DEF2 에서 사용
	// P 가 어디에서 결정되는가?
	//	선에서(EdgeID = -1, VertexID != -1, FaceID = -1)
	//	변에서(EdgeID != -1, VertexID = -1, FaceID = -1)
	//	면에서(EdgeID = -1, VertexID = -1, FaceID != -1)
	// 세 중의 하나만 -1이 아니어야 한다.
	DWORD	EdgeID;
	DWORD	VertexID;
	DWORD	FaceID;
#endif	// SEGMENT_VERTEX::{EdgeID,VertexID,FaceID}
};

struct POINT_DEF
{
	DWORD	ID;
	float	baryU;
	float	baryV;

	POINT_DEF&	operator=(const INTERSECTION& intersection)
	{
		this->ID = intersection.dwFace;
		this->baryU = intersection.fBary1;
		this->baryV = intersection.fBary2;
		return *this;
	}
};

struct TWO_POINT_DEF
{
	POINT_DEF	point0Def;
	POINT_DEF	point1Def;
};

struct TWO_POINT_PLANE_DEF : TWO_POINT_DEF
{
	// 두 점만 주어지면 단면을 결정하는 법선벡터를 절절하게 결정하나
	// 그 벡터가 원하는 것이 아닐 수 있다.
	// 이런 경우 법선벡터를 직접 설정할 수 있다.
	D3DXVECTOR3	normal;	// normal 이 영벡터면 적절하게 설정

	TWO_POINT_PLANE_DEF() {normal = D3DXVECTOR3(0, 0, 0);}
};

struct TWO_POINT_DEF2
{
	POINT_DEF	point0Def;
	SEGMENT_VERTEX	segmentVertex;
};

struct TWO_POINT_PLANE_DEF2 : TWO_POINT_DEF2
{
	// 두 점만 주어지면 단면을 결정하는 법선벡터를 절절하게 결정하나
	// 그 벡터가 원하는 것이 아닐 수 있다.
	// 이런 경우 법선벡터를 직접 설정할 수 있다.
	D3DXVECTOR3	normal;	// normal 이 영벡터면 적절하게 설정

	TWO_POINT_PLANE_DEF2() {normal = D3DXVECTOR3(0, 0, 0);}
};

struct PLANE_DEF {
	POINT_DEF	pointDef;
	D3DXVECTOR3	normal;
};

struct POINT_PLANE_DEF
{
	POINT_DEF	pointDef;
	PLANE_DEF	planeDef;
};

struct TWO_PLANE_DEF
{
	PLANE_DEF	plane0Def;
	PLANE_DEF	plane1Def;
};


////////////////////////////////////////////////////////////////////////////////
#endif	// !__kDEF__
