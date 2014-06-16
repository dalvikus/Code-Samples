#pragma once
#ifndef __kMAYA_APP_FUR_H__
#define __kMAYA_APP_FUR_H__
#include <afterShave/kMayaApp.h>
#include <afterShave/afterShaveGeometry.h>
class kMayaAppFur : public kMayaApp
{
public:
	kMayaAppFur(
		const char*	argv0,
		const char*	plug_in_shape_nodename,
		const char*	plug_in_name,
		const char*	plug_in_pathname_or,
		const char*	scene_filename_or,
		int		verbose_level = 0,
		MStatus*	status_ptr = NULL
	);
	~kMayaAppFur();
private:
	MVectorArray		__cp;
	afterShaveGeometry	__geometry;
public:
	const MVectorArray&	cp() const			{return __cp;}
	const afterShaveGeometry&	geometry() const	{return __geometry;}
public:
	MStatus	set();
};
#endif
