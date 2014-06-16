#include "baseShapeGeometryIterator.h"
#include <maya/MIOStream.h>
#if 1
#include <baseShape/baseShape.h>
#include <maya/MObjectArray.h>
#include <maya/MFnDoubleIndexedComponent.h>
#include <maya/MFnSingleIndexedComponent.h>
#endif
#if 1
baseShapeGeometryIterator::baseShapeGeometryIterator(
	void*	shapeNode,
	MObjectArray&	comps
) : MPxGeometryIterator(shapeNode, comps), baseShapeNode((baseShape*) shapeNode)
{
#ifdef baseShapeGeometryIterator_DEBUG
	cout << "baseShapeGeometryIterator::baseShapeGeometryIterator(MObjectArray&)" << endl;
#endif
	baseShapeNode->getSelectBy(this->m__iSelectBy);
	this->m__cpPtr = baseShapeNode->getControlPoints();
	this->geometryPtr = baseShapeNode->meshGeom();
#if 1
#ifdef baseShapeGeometryIterator_DEBUG
	cout << "baseShapeGeometryIterator::baseShapeGeometryIterator(MObjectArray&)" << endl;
	cout << "baseShapeGeometryIterator::baseShapeGeometryIterator(MObjectArray&): # of component(s) = " << comps.length() << endl;
	cout << "baseShapeGeometryIterator::baseShapeGeometryIterator(MObjectArray&): selectBy:= " << (this->m__iSelectBy == baseShape::selectByGuide ? "byGuide" : (this->m__iSelectBy == baseShape::selectByVertex ? "byVertex" : "byUnknown")) << endl;
#endif
	for (unsigned n = 0; n < comps.length(); ++n) {
		MObject	component = comps[n];
#ifdef baseShapeGeometryIterator_DEBUG
		cout << "baseShapeGeometryIterator::baseShapeGeometryIterator(MObjectArray&): MObjectArray[" << n << "]" << endl;
#endif
		if (this->m__iSelectBy == baseShape::selectByVertex && component.hasFn(MFn::kDoubleIndexedComponent)) {
			MStatus	ms;
			MFnDoubleIndexedComponent	fnVtxComp(component, &ms);
			int	len = fnVtxComp.elementCount(&ms);
			for (int i = 0; i < len; ++i) {
				int	uIndex;
				int	vIndex;
				ms = fnVtxComp.getElement(i, uIndex, vIndex);
				ms = this->m__indexArray.append(baseShape::N_POINTS_IN_GUIDE * uIndex + vIndex);
			}
		} else if (this->m__iSelectBy == baseShape::selectByGuide && component.hasFn(MFn::kSingleIndexedComponent)) {
			MStatus	ms;
			MFnSingleIndexedComponent	fnVtxComp(component, &ms);
			int	len = fnVtxComp.elementCount(&ms);
			for (int i = 0; i < len; ++i) {
				int	index = fnVtxComp.element(i, &ms);
				ms = this->m__indexArray.append(index);
			}
		}
	}
	reset();
	this->m__index = -1;
	this->m__length = 0;
	if (this->m__indexArray.length()) {
		int	nextIndex = this->m__indexArray[this->m__length];
		this->setCurrentPoint(nextIndex);
		this->m__index = nextIndex;
	}
#else
	reset();
#endif
}
baseShapeGeometryIterator::baseShapeGeometryIterator(
	void*	shapeNode,
	MObject&	comps
) : MPxGeometryIterator(shapeNode, comps), baseShapeNode((baseShape*) shapeNode)
{
#ifdef baseShapeGeometryIterator_DEBUG
	cout << "baseShapeGeometryIterator::baseShapeGeometryIterator(MObject&)" << endl;
#endif
	geometryPtr = baseShapeNode->meshGeom();
	reset();
}
#else
baseShapeGeometryIterator::baseShapeGeometryIterator(
	void*	geom,
	MObjectArray&	comps
) : MPxGeometryIterator(geom, comps), geometryPtr((baseShapeGeometry*) geom)
{
#ifdef baseShapeGeometryIterator_DEBUG
	cout << "baseShapeGeometryIterator::baseShapeGeometryIterator(MObjectArray&)" << endl;
#endif
	reset();
}
baseShapeGeometryIterator::baseShapeGeometryIterator(
	void*	geom,
	MObject&	comps
) : MPxGeometryIterator(geom, comps), geometryPtr((baseShapeGeometry*) geom)
{
#ifdef baseShapeGeometryIterator_DEBUG
	cout << "baseShapeGeometryIterator::baseShapeGeometryIterator(MObject&)" << endl;
#endif
	reset();
}
#endif
baseShapeGeometryIterator::~baseShapeGeometryIterator()
{
#ifdef baseShapeGeometryIterator_DEBUG
	cout << "baseShapeGeometryIterator::~baseShapeGeometryIterator()" << endl;
#endif
}
void	baseShapeGeometryIterator::reset()
{
#ifdef baseShapeGeometryIterator_DEBUG
	cout << "baseShapeGeometryIterator::reset()" << endl;
#endif
	MPxGeometryIterator::reset();
	setCurrentPoint(0);
	if (NULL != geometryPtr) {
		int	maxVertex = geometryPtr->vertices.length();
		setMaxPoints(maxVertex);
	}
}
MPoint	baseShapeGeometryIterator::point() const
{
#if 1
#ifdef baseShapeGeometryIterator_DEBUG
	cout << "baseShapeGeometryIterator::point(): ";
	cout << "m__index = " << this->m__index << ", m__length = " << this->m__length << ", m__indexArray.length() = " << this->m__indexArray.length() << endl;
	cout << "baseShapeGeometryIterator::point(): currentPoint = " << currentPoint() << endl;
	cout << "baseShapeGeometryIterator::point(): selectBy:= " << (this->m__iSelectBy == baseShape::selectByGuide ? "byGuide" : (this->m__iSelectBy == baseShape::selectByVertex ? "byVertex" : "byUnknown")) << endl;
#endif
	int	index = this->currentPoint();
	MPoint	pnt;
#if 3
	const MMatrix&	m = baseShapeNode->externalInclusiveMatrix();
#endif
	if (NULL != geometryPtr) {
		if (this->m__iSelectBy == baseShape::selectByGuide) {
			MVectorArray&	cp = *(this->m__cpPtr);
#if 1
			MPoint	basePoint(cp[baseShape::N_POINTS_IN_GUIDE * index + 0]);
#else
			MPoint	basePoint = geometryPtr->vertices[index];
#endif
#if 3
			basePoint *= m;
#endif
			MPoint	point = basePoint;
			for (int k = 0; k < (int) baseShape::N_POINTS_IN_GUIDE; ++k) {
				if (k) {
					MVector	offset = cp[baseShape::N_POINTS_IN_GUIDE * index + k];
					double	len = offset.length();
#if 3
					offset = offset.transformAsNormal(m);
#endif
					point += len * offset;
				}
				pnt += point;
			}
			pnt = pnt / baseShape::N_POINTS_IN_GUIDE;
		} else if (this->m__iSelectBy == baseShape::selectByVertex) {
			int	uIndex = index / baseShape::N_POINTS_IN_GUIDE;
			int	vIndex = index % baseShape::N_POINTS_IN_GUIDE;
#ifdef baseShapeGeometryIterator_DEBUG
			cout << "baseShapeGeometryIterator::point(): (" << uIndex << ", " << vIndex << ")" << endl;
#endif
			MVectorArray&	cp = *(this->m__cpPtr);
#if 1
			MPoint	basePoint(cp[baseShape::N_POINTS_IN_GUIDE * uIndex + 0]);
#else
			MPoint	basePoint = geometryPtr->vertices[uIndex];
#endif
#if 3
			basePoint *= m;
#endif
			pnt = basePoint;
			int	k = 1;
			while (k <= vIndex) {
				MVector	offset = cp[baseShape::N_POINTS_IN_GUIDE * uIndex + k];
				double	len = offset.length();
#if 3
				offset = offset.transformAsNormal(m);
#endif
				pnt += len * offset;
				++k;
			}
cout << "baseShapeGeometryIterator: " << pnt << endl;
		} else {
			pnt = geometryPtr->vertices[index];
		}
	}
#ifdef baseShapeGeometryIterator_DEBUG
	cout << "baseShapeGeometryIterator::point(): (" << pnt.x << ", " << pnt.y << ", " << pnt.z << ")" << endl;
#endif
	return pnt;
#else
#ifdef baseShapeGeometryIterator_DEBUG
	cout << "baseShapeGeometryIterator::point(): index = " << index() << endl;
#endif
	MPoint	pnt;
	if (NULL != geometryPtr) {
		pnt = geometryPtr->vertices[index()];
	}
	return pnt;
#endif
}
void	baseShapeGeometryIterator::setPoint(const MPoint& pnt) const
{
#ifdef baseShapeGeometryIterator_DEBUG
	cout << "baseShapeGeometryIterator::setPoint(): index = " << index() << endl;
#endif
	if (NULL != geometryPtr) {
		geometryPtr->vertices.set(pnt, index());
	}
}
int	baseShapeGeometryIterator::iteratorCount() const
{
#ifdef baseShapeGeometryIterator_DEBUG
	cout << "baseShapeGeometryIterator::iteratorCount(): returns " << geometryPtr->vertices.length() << endl;
#endif
	return geometryPtr->vertices.length();
}
bool	baseShapeGeometryIterator::hasPoints() const
{
#ifdef baseShapeGeometryIterator_DEBUG
	cout << "baseShapeGeometryIterator::hasPoints()" << endl;
#endif
	return true;
}
#if 1
bool	baseShapeGeometryIterator::isDone() const
{
#ifdef baseShapeGeometryIterator_DEBUG
	cout << "baseShapeGeometryIterator::isDone()" << endl;
	cout << "baseShapeGeometryIterator::isDone(): ";
	cout << "m__index = " << this->m__index << ", m__length = " << this->m__length << ", m__indexArray.length() = " << this->m__indexArray.length() << endl;
#endif
	return (unsigned) this->m__length == this->m__indexArray.length() ? true : false;
}
void	baseShapeGeometryIterator::next()
{
#ifdef baseShapeGeometryIterator_DEBUG
	cout << "baseShapeGeometryIterator::next()" << endl;
	cout << "baseShapeGeometryIterator::next(): ";
	cout << "m__index = " << this->m__index << ", m__length = " << this->m__length << ", m__indexArray.length() = " << this->m__indexArray.length() << endl;
#endif
	++this->m__length;
	int	nextIndex = this->m__length == this->m__indexArray.length() ? -1 : this->m__indexArray[this->m__length];
	this->m__index = nextIndex;
	this->setCurrentPoint(nextIndex);
}
#endif
