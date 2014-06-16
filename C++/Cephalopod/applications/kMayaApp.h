#pragma once
#ifndef __kMAYA_APP_H__
#define __kMAYA_APP_H__
#include <maya/MDagPath.h>
#include <maya/MObjectArray.h>
#include <maya/MString.h>
#if 1
#include <baseShape/kSkinSmoothBind.h>
#endif
class kMayaApp
{
protected:
	MString		__plug_in_name;
	MObject		__node;
	MDagPath	__body;
#if 1
protected:
	kSkinSmoothBind	__skinSmoothBind;
public:
	const kSkinSmoothBind&	skinSmoothBind() const	{return __skinSmoothBind;}
#endif
public:
	const MObject&	node() const	{return __node;}
	MObject&	node()		{return __node;}
	const MDagPath& body() const	{return __body;}
public:
	kMayaApp(
		const char*	argv0,
		const char*	plug_in_shape_nodename,
		const char*	plug_in_name,
		const char*	plug_in_pathname_or,
		const char*	scene_filename_or,
		int		verbose_level = 0,
		MStatus*	status_ptr = NULL
	);
	virtual ~kMayaApp();
protected:
	struct flags
	{
		bool	initialized: 1;
		bool	plugin_loaded: 1;
		flags() : initialized(false), plugin_loaded(false) {}
	};
	flags	__flags;
#if 1
private:
	MObjectArray	__fieldArray;
public:
	const MObjectArray&	fieldArray() const	{return __fieldArray;}
#endif
#if 1
public:
	struct Setting
	{
		int	fps;
		double	minTime;
		double	maxTime;
		double	animationStartTime;
		double	animationEndTime;
		double	by;
	};
	static MStatus	getSetting(Setting& setting, int verbose_level = 0);
#endif
};
#endif
