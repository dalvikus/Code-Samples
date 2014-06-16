#pragma once
#include <maya/MPxGeometryIterator.h>
#include <maya/MPoint.h>
#if 1
#include <maya/MIntArray.h>
class baseShape;
#endif
#include <baseShape/baseShapeGeometry.h>
class baseShapeGeometryIterator : public MPxGeometryIterator
{
public:
#if 1
	baseShapeGeometryIterator(void* shapeNode, MObjectArray& components);
	baseShapeGeometryIterator(void* shapeNode, MObject& components);
#else
	baseShapeGeometryIterator(void* userGeometry, MObjectArray& components);
	baseShapeGeometryIterator(void* userGeometry, MObject& components);
#endif
#if 1
	virtual ~baseShapeGeometryIterator();
#endif
	virtual void	reset();
	virtual MPoint	point() const;
	virtual void	setPoint(const MPoint&) const;
	virtual int	iteratorCount() const;
	virtual bool	hasPoints() const;
#if 1
	virtual bool	isDone() const;
	virtual void	next();
private:
	MIntArray	m__indexArray;
	int		m__index;
	unsigned	m__length;
#endif
private:
#if 1
	baseShape*	baseShapeNode;
	int		m__iSelectBy;
	MVectorArray*	m__cpPtr;
#endif
	baseShapeGeometry*	geometryPtr;
};
