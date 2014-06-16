#if defined(_MSC_VER) && (_MSC_VER >= 1400)
#else
#include <math.h>
#endif
#include <brushTool/CBrushCircle.h>
#include <assert.h>
#include <maya/MGlobal.h>
#include <gl/GLU.h>
unsigned	CBrushCircle::NPOINTS = 100;
unsigned	CBrushCircle::COLOR_INDEX_DEFAULT = 18;
double		CBrushCircle::RADIUS_DEFAULT = 50.;
GLint		CBrushCircle::REPEAT_FACTOR = 1;
GLint		CBrushCircle::REPEAT_PATTERN = (GLint) ((GLushort) 0xffff);
GLfloat		CBrushCircle::LINE_WIDTH = 1;
GLfloat		CBrushCircle::POINT_SIZE = 7.;
CBrushCircle::CBrushCircle(
	unsigned	colorIndex,
	double		radius,
	unsigned	nPoints
) : __colorIndex(colorIndex), __radius(radius), __points(nPoints)
{
	__viewX = -1;
	__viewY = -1;
}
CBrushCircle::~CBrushCircle()
{
}
int	CBrushCircle::draw()
{
	if (this->__viewX == -1 || this->__viewY == -1) {
		return 0;
	}
	assert(this->__viewX != -1 && this->__viewY != -1);
	if (__viewX == -1 || __viewY == -1)
		MGlobal::displayError("__viewX == -1 || __viewY == -1");
	POINT	ptCursor;
	BOOL	b = GetCursorPos(&ptCursor);
	if (!b) {
		assert(b);
	}
	if (ptCursor.x == __winX && ptCursor.y == __winY) {
		return 0;
	}
	__viewX += (short) (ptCursor.x - __winX);
	__viewY += (short) (ptCursor.y - __winY);
	__winX = ptCursor.x;
	__winY = ptCursor.y;
	int	x = this->__viewX;
	int	y = (short) (this->__viewportH - this->__viewY - 1);
	if (x < 0 || x >= this->__viewportW || y < 0 || y >= this->__viewportH) {
		this->__viewX = -1;
		this->__viewY = -1;
#if 1
		this->__view.refresh(true, true);
#else
	if (!context.__bDrawn) {
#ifdef move__DEBUG2
		cout << "marqueeContext::timerCallback(): No Drawing!" << endl;
#endif
		return;
	}
#ifdef move__DEBUG2
	cout << "marqueeContext::timerCallback(): Erase the Drawing..." << endl;
#endif
	bool	lightingWasOn = glIsEnabled(GL_LIGHTING) ? true : false;
	if (lightingWasOn) {
		glDisable(GL_LIGHTING);
	}
	float	lastPointSize;
	glGetFloatv(GL_POINT_SIZE, &lastPointSize);
	glPointSize(CBrushCircle::POINT_SIZE);
{
#ifdef USE_SOFTWARE_OVERLAYS
#if 1
	GLint		repeatFactor;
	glGetIntegerv(GL_LINE_STIPPLE_REPEAT, &repeatFactor);
#if 1
	GLint		repeatPattern;
	glGetIntegerv(GL_LINE_STIPPLE_PATTERN, &repeatPattern);
#else
	GLushort	repeatPattern;
	glGetIntegerv(GL_LINE_STIPPLE_PATTERN, (GLint*) &repeatPattern);
#endif
	GLfloat		lineWidth;
	glGetFloatv(GL_LINE_WIDTH, &lineWidth);
#endif
	GLboolean depthTest[1];
	GLboolean colorLogicOp[1];
	GLboolean lineStipple[1];
	glGetBooleanv (GL_DEPTH_TEST, depthTest);
	glGetBooleanv (GL_COLOR_LOGIC_OP, colorLogicOp);
	glGetBooleanv (GL_LINE_STIPPLE, lineStipple);
#if 1
	glLineStipple(CBrushCircle::REPEAT_FACTOR, (GLushort) CBrushCircle::REPEAT_PATTERN);
	glLineWidth(CBrushCircle::LINE_WIDTH);
#else
	glLineStipple( 1, 0x5555 );
	glLineWidth( 1.0 );
#endif
	glEnable(GL_LINE_STIPPLE);
	glDisable (GL_DEPTH_TEST);
	glEnable(GL_COLOR_LOGIC_OP);
	glLogicOp(GL_INVERT);
	glMatrixMode (GL_MODELVIEW);
	glPushMatrix();
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
    gluOrtho2D(
    			0.0, (GLdouble) viewportW,
    			0.0, (GLdouble) viewportH
    );
    glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
    glTranslatef(0.375, 0.375, 0.0);
	glIndexi (2);
	glBegin(GL_POINTS);
	glVertex2i(context.__lastViewX, context.__lastViewY);
	glEnd();
	SwapBuffers( view.deviceContext() );
	glMatrixMode( GL_MODELVIEW );
	glPopMatrix();
	glDisable(GL_COLOR_LOGIC_OP);
	if (colorLogicOp[0])
		glEnable (GL_COLOR_LOGIC_OP);
	else
		glDisable (GL_COLOR_LOGIC_OP);
	if (depthTest[0])
		glEnable (GL_DEPTH_TEST);
	else
		glDisable (GL_DEPTH_TEST);
	if (lineStipple[0])
		glEnable( GL_LINE_STIPPLE );
	else
		glDisable( GL_LINE_STIPPLE );
#if 1
#if 1
	glLineStipple(repeatFactor, (GLushort) repeatPattern);
#else
	glLineStipple(repeatFactor, repeatPattern);
#endif
	glLineWidth(lineWidth);
#endif
#else
	view.clearOverlayPlane();
	view.endOverlayDrawing();
#endif
}
	if (lightingWasOn) {
		glEnable(GL_LIGHTING);
	}
	glPointSize(lastPointSize);
#endif
		this->__bDrawn = false;
		this->__lastViewX = -1;
		this->__lastViewY = -1;
		return 0;
	}
	MStatus	ms = this->__view.beginGL(); assert(ms);
#if 1
	GLint		repeatFactor;
	glGetIntegerv(GL_LINE_STIPPLE_REPEAT, &repeatFactor);
	GLint		repeatPattern;
	glGetIntegerv(GL_LINE_STIPPLE_PATTERN, &repeatPattern);
	GLfloat		lineWidth;
	glGetFloatv(GL_LINE_WIDTH, &lineWidth);
#endif
{
	glLineStipple(CBrushCircle::REPEAT_FACTOR, (GLushort) CBrushCircle::REPEAT_PATTERN);
	glLineWidth(CBrushCircle::LINE_WIDTH);
	glEnable(GL_LINE_STIPPLE);
	bool	lightingWasOn = glIsEnabled(GL_LIGHTING) ? true : false;
	if (lightingWasOn) {
		glDisable(GL_LIGHTING);
	}
	float	lastPointSize;
	glGetFloatv(GL_POINT_SIZE, &lastPointSize);
#if 1
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0., (GLdouble) this->__viewportW, 0., (GLdouble) this->__viewportH);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.375, 0.375, 0.0);
#if 1
	this->__view.setDrawColor(this->getColorIndex(), M3dView::kActiveColors);
#else
	glIndexi(2);
#endif
	glPointSize(CBrushCircle::POINT_SIZE);
#if 1
	this->__view.refresh(true, true);
#else
#ifdef USE_SOFTWARE_OVERLAYS
	GLboolean depthTest[1];
	GLboolean colorLogicOp[1];
	GLboolean lineStipple[1];
	glGetBooleanv (GL_DEPTH_TEST, depthTest);
	glGetBooleanv (GL_COLOR_LOGIC_OP, colorLogicOp);
	glGetBooleanv (GL_LINE_STIPPLE, lineStipple);
	glDisable (GL_DEPTH_TEST);
	glEnable(GL_COLOR_LOGIC_OP);
	glLogicOp(GL_XOR);
	if (this->__bDrawn) {
#if 1
		glBegin(GL_LINE_LOOP);
		const MVectorArray&	points = this->getPoints();
		for (unsigned i = 0; i < CBrushCircle::NPOINTS; ++i) {
			glVertex2f((GLfloat) points[i].x, (GLfloat) points[i].y);
		}
		glEnd();
#else
		glBegin(GL_POINTS);
		glVertex2i(context.__lastViewX, context.__lastViewY);
		glEnd();
#endif
	}
#else
	view.clearOverlayPlane();
#endif
#endif
#if 1
	MVectorArray&	points = this->getPoints();
	glBegin(GL_LINE_LOOP);
	double&	r = this->__radius;
	double	f = 2 * M_PI / CBrushCircle::NPOINTS;
	for (unsigned i = 0; i < CBrushCircle::NPOINTS; ++i) {
		double	angle = f * i;
		double&	px = points[i].x;
		double&	py = points[i].y;
		px = x + r * cos(angle);
		py = y + r * sin(angle);
		glVertex2f((GLfloat) px, (GLfloat) py);
	}
	glEnd();
#else
	glBegin(GL_POINTS);
	glVertex2i(x, y);
	glEnd();
#endif
	SwapBuffers(this->__view.deviceContext());
#if 1
#else
#ifdef USE_SOFTWARE_OVERLAYS
	this->__bDrawn = true;
	this->__lastViewX = x;
	this->__lastViewY = y;
	if (colorLogicOp[0])
		glEnable (GL_COLOR_LOGIC_OP);
	else
		glDisable (GL_COLOR_LOGIC_OP);
	if (depthTest[0])
		glEnable (GL_DEPTH_TEST);
	else
		glDisable (GL_DEPTH_TEST);
	if (lineStipple[0])
		glEnable( GL_LINE_STIPPLE );
	else
		glDisable( GL_LINE_STIPPLE );
#endif
#endif
}
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
#endif
	if (lightingWasOn) {
		glEnable(GL_LIGHTING);
	}
	glPointSize(lastPointSize);
}
#if 1
	glLineStipple(repeatFactor, (GLushort) repeatPattern);
	glLineWidth(lineWidth);
#endif
	ms = this->__view.endGL(); assert(ms);
	return 0;
}
int	CBrushCircle::sync(MEvent& event)
{
	if (this->__viewX != -1) {
		assert(this->__viewY != -1);
		if (this->__bDrawn) {
			assert(this->__lastViewX != -1);
			assert(this->__lastViewY != -1);
			if (__viewX == -1 || __viewY == -1)
				MGlobal::displayError("__viewX == -1 || __viewY == -1");
			this->__view.refresh(true, true);
			this->__bDrawn = false;
			this->__lastViewX = -1;
			this->__lastViewY = -1;
		}
		this->__viewX = -1;
		this->__viewY = -1;
	}
	POINT	ptCursor;
	BOOL	b = GetCursorPos(&ptCursor);
	if (!b) {
		assert(b);
	}
	this->__winX = ptCursor.x;
	this->__winY = ptCursor.y;
#if 1
	short	viewX, viewY;
	MStatus	ms = event.getPosition(viewX, viewY); assert(ms);
	this->__viewX = viewX;
	this->__viewY = viewY;
#else
	MStatus	ms = event.getPosition(this->__viewX, this->__viewY); assert(ms);
#endif
	this->__view = M3dView::active3dView(&ms); assert(ms);
	this->__viewportH = this->__view.portHeight(&ms); assert(ms);
	this->__viewportW = this->__view.portWidth(&ms); assert(ms);
	this->__bDrawn = false;
	this->__lastViewX = -1;
	this->__lastViewY = -1;
	return 0;
}
