#pragma once
#ifndef __kVIEW__
#define __kVIEW__


#include <DXUT/Core/DXUT.h>
#include <mesh/kEnum.h>


struct kView : k2DInterfaceBase
{
	VIEW_T		__view;
	ROTATION_T	__rotation;
	bool		__home;	// 카메라을 초기 위치로 이동시키고 뷰도 VIEW_PERSPECTIVE 로 바꾼다.

	kView();

	// 2D 에서 __view, __rotation 를 참조할 때
	// override
	virtual int	getValue(int subType, void* dataPtr);
	virtual int	setValue(int subType, const void* dataPtr);

	// 이 클래스를 접근할 수 있는 곳에서 바로 참조할 때
	const VIEW_T&	getViewRef() const		{return __view;}
	VIEW_T&		getViewRef()			{return __view;}
	const ROTATION_T&	getRotationRef() const	{return __rotation;}
	ROTATION_T&		getRotationRef()	{return __rotation;}
	const bool&	getHomeRef() const		{return __home;}
	bool&		getHomeRef()			{return __home;}
};


#endif	// !__kVIEW__
