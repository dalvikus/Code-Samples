#include <baseShape/arrowLocator.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MDagPath.h>
#include <maya/MFnCamera.h>
#include <maya/MQuaternion.h>
#include <maya/MFnMatrixData.h>
#include <maya/MMatrix.h>
MTypeId arrowLocator::id(0x80007 + 1);
MObject	arrowLocator::point;
MObject	arrowLocator::origin;
MObject	arrowLocator::to;
MObject	arrowLocator::len;
MObject	arrowLocator::dir;
arrowLocator::arrowLocator() {}
arrowLocator::~arrowLocator() {}
MStatus	arrowLocator::compute(const MPlug&, MDataBlock&)
{
	return MS::kUnknownParameter;
}
void	arrowLocator::postConstructor()
{
	MPlug	plug(thisMObject(), to);
	MFnNumericData	to_val;
	MStatus	ms;
	MObject	to_data = to_val.create(MFnNumericData::k3Double, &ms);
	ms = to_val.setData(0., 0., 1.);
	ms = plug.setValue(to_data);
}
#if 0
#endif
void	arrowLocator::draw(M3dView& view, const MDagPath&, M3dView::DisplayStyle style, M3dView::DisplayStatus status)
{
	double	pointSize;
	MPlug	plug(thisMObject(), point);
	plug.getValue(pointSize);
	plug.setAttribute(origin);
	MObject	origin_data;
	plug.getValue(origin_data);
	MFnNumericData	origin_val(origin_data);
	double	x0, y0, z0;
	origin_val.getData(x0, y0, z0);
	MPoint	p0(x0, y0, z0);
	double	r;
	plug.setAttribute(len);
	plug.getValue(r);
	plug.setAttribute(dir);
	MObject	dir_data;
	plug.getValue(dir_data);
	MFnNumericData	dir_val(dir_data);
	double	theta, phi;
	dir_val.getData(theta, phi);
	theta = theta * M_PI / 180;
	phi = phi * M_PI / 180;
	double	x = x0 + r * sin(theta) * cos(phi);
	double	y = y0 + r * sin(theta) * sin(phi);
	double	z = z0 + r * cos(theta);
#if 0
	plug.setAttribute(to);
	MObject	to_data;
	plug.getValue(to_data);
	MFnNumericData	to_val(to_data);
	double	x, y, z;
	to_val.getData(x, y, z);
#endif
	view.beginGL();
	if (style == M3dView::kFlatShaded || style == M3dView::kGouraudShaded) {
		glPushAttrib(GL_CURRENT_BIT);
		if (status == M3dView::kActive) {
			view.setDrawColor(13, M3dView::kActiveColors);
		} else {
			view.setDrawColor(13, M3dView::kDormantColors);
		}
		glPopAttrib();
	}
	float	lastPointSize;
	glGetFloatv(GL_POINT_SIZE, &lastPointSize);
        glPointSize((float) pointSize);
	glBegin(GL_POINTS);
	glVertex3d(x0, y0, z0);
	glEnd();
        glPointSize(lastPointSize);
	glBegin(GL_LINES);
	glVertex3d(x0, y0, z0);
	glVertex3d(x, y, z);
	glEnd();
#if 0
	glBegin(GL_LINES);
	glVertex3d(x0, y0, z0);
	glVertex3d(X, Y, Z);
	glEnd();
#endif
	MDagPath	cameraDagPath;
	view.getCamera(cameraDagPath);
	MFnCamera	fnCamera(cameraDagPath);
	MVector	viewDir = fnCamera.viewDirection(MSpace::kWorld);
	MVector	upDir = fnCamera.upDirection(MSpace::kWorld);
	MVector	rightDir = fnCamera.rightDirection(MSpace::kWorld);
	MPoint	p(x, y, z);
	MVector	v = p0 - p;
	MQuaternion	q1(M_PI / 4, viewDir);
	MVector	v1 = v.rotateBy(q1);
	v1.normalize();
	MPoint	p1 = p + v1;
	MQuaternion	q2(-M_PI / 4, viewDir);
	MVector	v2 = v.rotateBy(q2);
	v2.normalize();
	MPoint	p2 = p + v2;
#if 1
	glBegin(GL_LINE_STRIP);
	glVertex3d(p1.x, p1.y, p1.z);
	glVertex3d(x, y, z);
	glVertex3d(p2.x, p2.y, p2.z);
	glEnd();
#else
	glBegin( GL_LINES );
	glVertex3f(p1.x, p1.y, p1.z);
	glVertex3f(x, y, z);
	glEnd();
	glBegin( GL_LINES );
	glVertex3f(x, y, z);
	glVertex3f(p2.x, p2.y, p2.z);
	glEnd();
#endif
	view.drawText("Wind", p0);
	view.endGL();
}
bool	arrowLocator::isBounded() const
{
	return true;
}
MBoundingBox	arrowLocator::boundingBox() const
{
	double	pointSize;
	MPlug	plug(thisMObject(), point);
	plug.getValue(pointSize);
	plug.setAttribute(origin);
	MObject	origin_data;
	plug.getValue(origin_data);
	MFnNumericData	origin_val(origin_data);
	double	x0, y0, z0;
	origin_val.getData(x0, y0, z0);
	MPoint	p0(x0, y0, z0);
	double	r;
	plug.setAttribute(len);
	plug.getValue(r);
	plug.setAttribute(dir);
	MObject	dir_data;
	plug.getValue(dir_data);
	MFnNumericData	dir_val(dir_data);
	double	theta, phi;
	dir_val.getData(theta, phi);
	theta = theta * M_PI / 180;
	phi = phi * M_PI / 180;
	double	x = x0 + r * sin(theta) * cos(phi);
	double	y = y0 + r * sin(theta) * sin(phi);
	double	z = z0 + r * cos(theta);
	MPoint	p(x, y, z);
	return MBoundingBox(p0, p);
}
void*	arrowLocator::creator()
{
	return new arrowLocator();
}
MStatus	arrowLocator::initialize()
{
	MStatus	ms = MStatus::kSuccess;
	MFnNumericAttribute	pointAttr;
	point = pointAttr.create("Point", "point", MFnNumericData::kDouble, 3);
	addAttribute(point);
	MFnNumericAttribute	originAttr;
	origin = originAttr.create("Origin", "o", MFnNumericData::k3Double, 0);
	addAttribute(origin);
	MFnNumericAttribute	toAttr;
	to = toAttr.create("To", "t", MFnNumericData::k3Double, 1);
	addAttribute(to);
	MFnNumericAttribute	lenAttr;
	len = lenAttr.create("Length", "len", MFnNumericData::kDouble, 2);
	addAttribute(len);
	MFnNumericAttribute	dirAttr;
	dir = dirAttr.create("Direction", "dir", MFnNumericData::k2Double, 90);
	addAttribute(dir);
	return ms;
}
