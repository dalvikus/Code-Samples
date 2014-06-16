#pragma once
#ifndef __kBONE_H__
#define __kBONE_H__
////////////////////////////////////////////////////////////////////////////////


#include <map>
#include <vector>
#include <set>
#include <mesh/kDef.h>
#if 1	// kDisplacementMap
#include <mesh/kDisplacementMap.h>
#endif	// kDisplacementMap

/*
 * kMesh.cpp 의 "실제" 소스 참고
 *	#if 1	// 뼈의 길이 변경
 *	#if 1	// kBone
 *	#if 1	// 다리를 길어 보이게 하기
 *	......	//	(적용이 쉬운 예)
 *	#else	// 다리를 길어 보이게 하기/테스트
 *	#if 1	// 실제
 *	......	//	(복잡한 예)
 *	#else	// 실제/예제
 *	......	//	(단순한 예)
 *	#endif	// 예제
 *	#endif	// 테스트
 *	#else	// kBone/!kBone
 *	......
 *	#endif	// !kBone
 *	#endif	// 뼈의 길이 변경
 */
class kMesh;
struct kBone : PLANE_DEF, POINT_DEF
{
	typedef std::map<DWORD, D3DXVECTOR3>	VERTEX_HASH;
private:
	VERTEX_HASH	__inside;
	VERTEX_HASH	__outside;
	std::vector<DWORD>	__insideFaceIDArray;
	std::vector<DWORD>	__outsideFaceIDArray;
	D3DXVECTOR3	__P0_star;	// 확대/축소할 때 사용되는 기준점;
					// 횡(橫: transversal)으로 살을 늘이는 경우,
					// 연속된 두 뼈에 붙은 살이 역시 연속적으로 보이게 하기 위해서는
					// 같은 기준점을 사용해야 한다.
public:
	const VERTEX_HASH&		inside() const			{return __inside;}
	const VERTEX_HASH&		outside() const			{return __outside;}
	const std::vector<DWORD>&	insideFaceIDArray() const	{return __insideFaceIDArray;}
	const std::vector<DWORD>&	outsideFaceIDArray() const	{return __outsideFaceIDArray;}
	const D3DXVECTOR3&		P0_star() const			{return __P0_star;}

public:
	kBone();
	~kBone();

public:
	// 정의된 뼈에 붙은 살을 변화시킨다; __inside, __outside
	void	set(
		const kMesh&	mesh,
		const PLANE_DEF&	planeDef,	// 이 평면과
		const POINT_DEF*	pointDefPtr,	// 이 점에 의해서 정의되는 뼈 사이에 있는 점들을
					// (이 값이 NULL 이면 평면에 속하는(법선벡터 방향에 있는) 모든 점들을)
		float	fScale,				// 이 만큼 확대(fScale > 1)/축소(fScale < 1)한다.
		bool	both = true,			// 뼈에 붙은 살을 종(縱: longitudinal; False)으로(받드시),
							// 또는 동시에 횡(橫: transversal; True) 으로 늘인다.
		const D3DXVECTOR3*	__P0Ptr = NULL	// 사용하는 기준점
	);
#if 1	// kDisplacementMap
	void	set2(
		const kMesh&	mesh,
		const PLANE_DEF&	planeDef,	// 이 평면과
		const POINT_DEF*	pointDefPtr,	// 이 점에 의해서 정의되는 뼈 사이에 있는 점들을
					// (이 값이 NULL 이면 평면에 속하는(법선벡터 방향에 있는) 모든 점들을)
		float	fScale,				// 이 만큼 확대(fScale > 1)/축소(fScale < 1)한다.
		bool	both = true,			// 뼈에 붙은 살을 종(縱: longitudinal; False)으로(받드시),
							// 또는 동시에 횡(橫: transversal; True) 으로 늘인다.
		const D3DXVECTOR3*	__P0Ptr = NULL,	// 사용하는 기준점
		const kDisplacementMap*	dismap_ptr = NULL	// 변형할 때 Displacement Map 을 참고
	);
#endif	// kDisplacementMap
	// 살의 변화를 실제로 모델에 적용한다; 먼저 kBone::set 함수를 호출해야 한다.
	// 뼈의 횡방향으로...
	void	apply(
		kMesh&	mesh,
		const std::set<DWORD>*	excludeSetPtr = 0	// 뼈가 연속적으로 있을 경우,
								// 종(縱)과 횡(橫)으로 살을 변화시킬 경우,
								// 두 뼈에 동시에 속하는 점들은 제외한다.
	);
#if 1	// 마지막 주문; 7. 다리 길이 변경
	static void	leg_elongate(kMesh& mesh, const POINT_DEF& pointDef, float scale);
#endif	// 마지막 주문; 7. 다리 길이 변경
#if 1	// kDisplacementMap
	// 노말방향으로...
	void	apply2(
		kMesh&	mesh,
		const D3DXVECTOR3&	P0,
		float	fScale,
		const std::set<DWORD>*	excludeSetPtr = 0	// 뼈가 연속적으로 있을 경우,
								// 종(縱)과 횡(橫)으로 살을 변화시킬 경우,
								// 두 뼈에 동시에 속하는 점들은 제외한다.
	);
#if 0	// moved to kMesh::simulateWholeBody
	void	apply_all(kMesh& mesh, float fScale);
#endif
#endif	// kDisplacementMap

	// x1 에서 y1 값을 가지고, x2 에서 y2 값을 가질 때,
	// x1 과 x2 사이에(inclusive) 있는 점 x 에서의 값은?
	static float	smoothstep(float x1, float y1, float x2, float y2, float x);

private:
	D3DXVECTOR3	__centroid;	// 뼈의 중심
	D3DXVECTOR3	__nDir;		// 뼈의 방향; 시작과 끝은 아직 결정하지 못했다... (TO DO LIST)
	float		__length;	// 뼈의 길이
public:
	D3DXVECTOR3&	centroid()	{return __centroid;}
	D3DXVECTOR3&	nDir()		{return __nDir;}
	float&		length()	{return __length;}
	const D3DXVECTOR3&	centroid() const	{return __centroid;}
	const D3DXVECTOR3&	nDir() const		{return __nDir;}
	const float&		length() const		{return __length;}
};



////////////////////////////////////////////////////////////////////////////////
#endif	// !__kBONE_H__
