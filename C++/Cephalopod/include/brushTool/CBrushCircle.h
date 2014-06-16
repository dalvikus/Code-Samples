#pragma once
#define USE_SOFTWARE_OVERLAYS
#include <maya/MVectorArray.h>
#include <maya/M3dView.h>
#include <maya/MEvent.h>
class CBrushCircle
{
public:
	CBrushCircle(
		unsigned	colorIndex = CBrushCircle::COLOR_INDEX_DEFAULT,
		double		radius = CBrushCircle::RADIUS_DEFAULT,
		unsigned	nPoints = CBrushCircle::NPOINTS
	);
	~CBrushCircle();
private:
	static GLint	REPEAT_FACTOR;
	static GLint	REPEAT_PATTERN;
	static GLfloat	LINE_WIDTH;
	static GLfloat	POINT_SIZE;
private:
	static unsigned	COLOR_INDEX_DEFAULT;
	unsigned	__colorIndex;
	static double	RADIUS_DEFAULT;
	double		__radius;
	static unsigned	NPOINTS;
	MVectorArray	__points;
public:
	unsigned	getColorIndex() const			{return __colorIndex;}
	void		setColorIndex(unsigned colorIndex)	{__colorIndex = colorIndex;}
	double		getRadius() const			{return __radius;}
	void		setRadius(double radius)		{__radius = radius;}
	const MVectorArray&	getPoints() const		{return __points;}
	MVectorArray&	getPoints()				{return __points;}
private:
	long		__winX, __winY;
	M3dView		__view;
	int		__viewportH;
	int		__viewportW;
	int		__viewX, __viewY;
	bool		__bDrawn;
	long		__lastViewX, __lastViewY;
public:
	int	sync(MEvent& event);
	int	draw();
};
