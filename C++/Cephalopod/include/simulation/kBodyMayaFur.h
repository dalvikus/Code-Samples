#pragma once
#ifndef __kBODY_MAYA_FUR_H__
#define __kBODY_MAYA_FUR_H__
#include <afterShave/kBodyMaya.h>
class afterShaveGeometry;
class kBodyMayaFur : public kBodyMaya
{
public:
	kBodyMayaFur() : kBodyMaya() {}
	virtual ~kBodyMayaFur() {}
public:
	void	set__T0_in_kSpace_array(const afterShaveGeometry& geometry);
};
#endif
