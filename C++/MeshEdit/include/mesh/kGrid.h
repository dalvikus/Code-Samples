#pragma once
#ifndef __kGRID__
#define __kGRID__


#include <DXUT/Core/DXUT.h>
#include <mesh/kEnum.h>


struct kGrid : k2DInterfaceBase
{
#if 1	// TYPE
	GRID_TYPE	__type;
#endif	// TYPE
	GRID_Z		__z;
	GRID_DIR	__dir;
	GRID_STEP	__step;	// projection space 에서의 단위로;
				// x: [-1, 1], y: [-1, 1]
#if 1	// TYPE
#if 1	// 기준이 화면
	GRID_CENTER	__O;	// projection space 에서의 단위로;
				// x: [-1, 1], y: [-1, 1]
#endif	// 기준이 화면
#endif	// TYPE

	kGrid();

	// 2D 에서 __view, __rotation 를 참조할 때
	// override
	virtual int	getValue(int subType, void* dataPtr);
	virtual int	setValue(int subType, const void* dataPtr);

	// 이 클래스를 접근할 수 있는 곳에서 바로 참조할 때
#if 1	// TYPE
	const GRID_TYPE&	getTypeRef() const	{return __type;}
	GRID_TYPE&		getTypeRef()		{return __type;}
#endif	// TYPE
	const GRID_Z&		getZRef() const		{return __z;}
	GRID_Z&			getZRef()		{return __z;}
	const GRID_DIR&		getDirRef() const	{return __dir;}
	GRID_DIR&		getDirRef()		{return __dir;}
	const GRID_STEP&	getStepRef() const	{return __step;}
	GRID_STEP&		getStepRef()		{return __step;}
#if 1	// TYPE
#if 1	// 기준이 화면
	const GRID_CENTER&	getCenterRef() const	{return __O;}
	GRID_CENTER&		getCenterRef()		{return __O;}
#endif	// 기준이 화면
#endif	// TYPE
};


#endif	// !__kGRID__
