#if defined(_MSC_VER) && (_MSC_VER >= 1400)
#else
#include <math.h>
#endif
#include <brushTool/CBrushContext.h>
#if 1
#include <tentacleShape/kTentacleEnum.h>
#include <maya/MFnVectorArrayData.h>
#endif
#include <maya/MTimerMessage.h>
#include <maya/MCommandResult.h>
#include <maya/MModelMessage.h>
#include <maya/MSelectionList.h>
#include <maya/MGlobal.h>
#include <assert.h>
#include <maya/MPlug.h>
#include <maya/MDagPath.h>
#include <maya/MFnSingleIndexedComponent.h>
#include <maya/MFnDoubleIndexedComponent.h>
#include <baseShape/kUtil.h>
#include <maya/MFnTransform.h>
#include <float.h>
#include <maya/MDataHandle.h>
#if 1
#include <maya/MUserEventMessage.h>
#include <tentacleShape/tentacleShape.h>
#endif
#include <maya/MFnCamera.h>
#if 2
#include <maya/MFnMesh.h>
#endif
#if 1
#include <maya/MPlugArray.h>
#include <maya/MFnMatrixData.h>
#include <maya/MTime.h>
#include <maya/MAnimControl.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MItMeshVertex.h>
#include <maya/MItMeshFaceVertex.h>
#endif
const char	helpString[] = "Click with left button or drag with middle button to select";
#if 0
#endif
float	CBrushContext::PERIOD = 0.05f;
#if 0
#endif
CBrushContext::CBrushContext() : __activeSelectionArray()
{
#if 1
	__brushType = CBrushToolCommand::BT_TENTACLE_CREATE;
#else
	__brushType = CBrushToolCommand::BT_CUT;
#endif
	CBrushToolCommand::BrushType = __brushType;
	setTitleString("Cephalopod Brush: Create");
	__toolCommandPtr = NULL;
	setImage("brushTool.xpm", MPxContext::kImage1);
#if 1
#if 1
#if MAYA_API_VERSION >= 200800
	__accelParams = MFnMesh::autoUniformGridParams();
#else
#if MAYA_API_VERSION == 700
#else
#endif
#endif
#endif
#endif
}
#if 0
#endif
CBrushContext::~CBrushContext()
{
#ifdef move__DEBUG2
	cout << "MPxContext::~MPxContext()" << endl;
#endif
#if 1
#if 1
	MStatus	ms;
#ifdef _DEBUG
	cout << "CBrushContext::~CBrushContext: free cached intersection accelerator..." << endl;
#endif
	if (__mesh.dagPath().isValid()) {
#pragma warning(disable: 4127)
	do {
#ifdef _DEBUG
		cout << __mesh.fullPathName(&ms) << endl;
		if (!ms) {
			cerr << "MDagPath::fullPathName(): Failure: |" << ms << "|" << endl;
			break;
		}
		cout << "static MFnMesh::globalIntersectionAcceleratorsInfo(): |" << MFnMesh::globalIntersectionAcceleratorsInfo() << "|" << endl;
		MString	desc = __mesh.cachedIntersectionAcceleratorInfo(&ms);
		if (!ms) {
			cerr << "MFnMesh::cachedIntersectionAcceleratorInfo(): Failure: |" << ms << "|" << endl;
			break;
		}
		cout << "MFnMesh::cachedIntersectionAcceleratorInfo(MStatus*): |" << desc << "|" << endl;
#endif
		ms = __mesh.freeCachedIntersectionAccelerator();
#ifdef _DEBUG
		cout << "MFnMesh::freeCachedIntersectionAccelerator() returns |" << ms << "|" << endl;
		cout << "static MFnMesh::globalIntersectionAcceleratorsInfo(): |" << MFnMesh::globalIntersectionAcceleratorsInfo() << "|" << endl;
		desc = __mesh.cachedIntersectionAcceleratorInfo(&ms);
		if (!ms) {
			cerr << "MFnMesh::cachedIntersectionAcceleratorInfo(): Failure: |" << ms << "|" << endl;
			break;
		}
		cout << "MFnMesh::cachedIntersectionAcceleratorInfo(MStatus*): |" << desc << "|" << endl;
#endif
	} while (0);
#pragma warning(default: 4127)
	}
#ifdef _DEBUG
	cout << "...done" << endl;
#endif
#endif
#endif
}
#if 0
#endif
void	CBrushContext::toolOnSetup(MEvent&)
{
#ifdef move__DEBUG2
	cout << "CBrushContext::toolOnSetup()" << endl;
#endif
	MStatus	ms;
	__timerCallbackId = MTimerMessage::addTimerCallback(CBrushContext::PERIOD, timerCallback, this, &ms);
	__activeListModifiedCallback(this);
	__activeListModifiedCallbackId = MModelMessage::addCallback(MModelMessage::kActiveListModified, CBrushContext::__activeListModifiedCallback, this, &ms);
#if 1
#if 1
	ms = MUserEventMessage::registerUserEvent(tentacleShape::TentacleArrayUpdated);
	__userEventTentacleArrayUpdatedCallbackId = MUserEventMessage::addUserEventCallback(tentacleShape::TentacleArrayUpdated, CBrushContext::__userEventTentacleArrayUpdatedCallback, this, &ms);
#endif
#endif
	setHelpString(helpString);
}
#if 1
#if 1
static MStatus	FindClosestIntersection(
	MFnMesh&	mesh,
	short	x,
	short	y,
	bool&	hitted,
	MFloatPoint&	hitPoint,
	MMeshIsectAccelParams*	accelParamsPtr,
#if 1
#if 1
	int*		hitVertexList,
#else
	int&		hitTriangleId,
#endif
	float&		hitBary1,
	float&		hitBary2,
#endif
	int&		polyId,
	MPoint&		position,
	MVector&	normal
)
{
	MStatus	ms;
	M3dView	view = M3dView::active3dView(&ms);
	MPoint	raySource;
	MVector	rayVector;
	MPoint&	nearClipPt = raySource;
	MPoint	farClipPt;
	ms = view.viewToWorld(x, y, nearClipPt, farClipPt);
	rayVector = farClipPt - nearClipPt;
	MFloatPoint	raySource_f((float) raySource.x, (float) raySource.y, (float) raySource.z);
	int	id;
#if 1
	int	hitTriangleId;
#else
#endif
	hitted = mesh.closestIntersection(
		raySource_f, rayVector,
		NULL, NULL, NULL,
		MSpace::kWorld,
		(float) rayVector.length(),
		false,
		accelParamsPtr,
#if 1
		hitPoint, NULL, &id, &hitTriangleId, &hitBary1, &hitBary2,
#else
		hitPoint, NULL, &id, NULL, NULL, NULL,
#endif
		1.e-6f,
		&ms
	);
	if (!ms) {
		cerr << "MFnMesh::closestIntersection(...): Failure: |" << ms << "|" << endl;
	}
#if 0
	cout << "hitted? " << (hitted ? "True" : "False") << endl;
	if (hitted) {
		cout << hitPoint << endl;
MString strCommandString = "string $strBall[] = `polySphere -r 0.5`;";
strCommandString += "$strBallName = $strBall[0];";
float x = 0;
float y = 0;
float z = 0;
x = hitPoint[0];
y = hitPoint[1];
z = hitPoint[2];
strCommandString += "setAttr ($strBallName + \".tx\") ";
strCommandString += x;
strCommandString += ";";
strCommandString += "setAttr ($strBallName + \".ty\") ";
strCommandString += y;
strCommandString += ";";
strCommandString += "setAttr ($strBallName + \".tz\") ";
strCommandString += z;
strCommandString += ";";
MGlobal::executeCommand(strCommandString);
	}
#endif
	if (!hitted)
		return ms;
	MMatrix	inv_m = mesh.dagPath().inclusiveMatrixInverse();
#if MAYA_API_VERSION == 700
	MPoint	hitPoint_in_kObject;
	hitPoint_in_kObject.x = (double) hitPoint.x;
	hitPoint_in_kObject.y = (double) hitPoint.y;
	hitPoint_in_kObject.z = (double) hitPoint.z;
	hitPoint_in_kObject.w = (double) hitPoint.w;
#else
	MPoint	hitPoint_in_kObject(hitPoint);
#endif
	hitPoint_in_kObject *= inv_m;
	ms = mesh.getClosestPointAndNormal(hitPoint_in_kObject, position, normal, MSpace::kObject, &polyId);
	if (!ms) {
		cerr << "MFnMesh::getClosestPointAndNormal(): Failure: |" << ms << "|" << endl;
		return ms;
	}
#if 1
	{
		MDagPath	dagPath = mesh.dagPath(&ms);
		MItMeshPolygon	polyIter(dagPath, MObject::kNullObj, &ms);
		cout << ms << endl;
		int	prevIndex;
		ms = polyIter.setIndex(polyId, prevIndex);
		MPointArray	wPoints;
		MIntArray	vertexList;
		ms = polyIter.getTriangle(hitTriangleId, wPoints, vertexList, MSpace::kWorld);
		cout << "hitTriangleId = " << hitTriangleId << ", vertexList: " << vertexList << endl;
		hitVertexList[0] = vertexList[0];
		hitVertexList[1] = vertexList[1];
		hitVertexList[2] = vertexList[2];
#if 0
		MPoint	wPosition;
		MVector	wNormal;
		ms = mesh.getClosestPointAndNormal(hitPoint, wPosition, wNormal, MSpace::kWorld, NULL);
		cout << "hitPoint: " << hitPoint << endl;
		cout << "wPosition: " << wPosition << endl;
		cout << "wNormal: " << wNormal << endl;
#endif
#if 0
{
	MDagPath	dagPath = mesh.dagPath(&ms);
	MItMeshPolygon	polyIter(dagPath, MObject::kNullObj, &ms);
	cout << ms << endl;
	int	prevIndex;
	ms = polyIter.setIndex(polyId, prevIndex);
	MPointArray	wPoints;
	MIntArray	vertexList;
	ms = polyIter.getTriangle(hitTriangleId, wPoints, vertexList, MSpace::kWorld);
	cout << wPoints << endl;
#if 1
{
	MFloatVector	v1(wPoints[0]), v2(wPoints[1]), v3(wPoints[2]);
	cout << "calculated wPosition: " << (hitBary1 * v1 + hitBary2 * v2 + (1 - hitBary1 - hitBary2) * v3) << endl;
}
#endif
#if 1
{
#if 0
	MFloatVector	v12 = v2 - v1, v13 = v3 - v1;
	v12.normalize(); v13.normalize();
	MFloatVector	v = v12 ^ v13;
	v.normalize();
	cout << v << endl;
	MString	s("tilt: ");
	s += v * wNormal;
	s += " (";
	s += acos(v * wNormal) * 180 / M_PI;
	s += ")";
	s += ": isPlanar? ";
	s += polyIter.isPlanar() ? "True" : "False";
	MGlobal::displayWarning(s);
	cout << s << endl;
#endif
#if 1
#if 1
{
	int	numTriangles;
	ms = polyIter.numTriangles(numTriangles);
	MVector	n = MVector::zero;
	for (int k = 0; k < numTriangles; ++k) {
		MPointArray	points;
		MIntArray	vList;
		ms = polyIter.getTriangle(k, points, vList, MSpace::kWorld);
		MVector	v1 = points[1] - points[0]; v1.normalize();
		MVector	v2 = points[2] - points[0]; v1.normalize();
		MVector	v = v1 ^ v2; v.normalize();
		if (k && n * v < 0) {
			cerr << "WARN: normal[" << k << "]: reversed" << endl;
			v = -v;
		}
		n += v;
	}
	n.normalize();
	if (numTriangles && n * wNormal < 0) {
		cerr << "WARN: n: reversed" << endl;
		n = -n;
	}
	cout << "calculated wNormal: " << n << endl;
}
#else
	int	numTriangles;
	ms = polyIter.numTriangles(numTriangles);
	MVector	n = MVector::zero;
	for (int k = 0; k < numTriangles; ++k) {
		MPointArray	points;
		MIntArray	vList;
		ms = polyIter.getTriangle(k, points, vList, MSpace::kWorld);
		MVector	v1 = points[1] - points[0]; v1.normalize();
		MVector	v2 = points[2] - points[0]; v1.normalize();
		MVector	v = v1 ^ v2; v.normalize();
		n += v;
	}
	cout << "calculated wNormal: " << n.normal() << endl;
#endif
#endif
}
#endif
}
#endif
#if 0
	MItMeshFaceVertex	faceVertexIter(dagPath, polyIter.polygon(&ms), &ms);
	for (; !faceVertexIter.isDone(); faceVertexIter.next()) {
		cout << "faceId = " << faceVertexIter.faceId() << ", vertexId = " << faceVertexIter.vertId() << endl;
		MVector	normal;
		ms = faceVertexIter.getNormal(normal, MSpace::kWorld);
		cout << "normal: " << normal << endl;
	}
	cout << ms << endl;
#endif
	}
#endif
#if 0
	cout << "ID is equal? " << (id == polyId ? "True" : "False") << endl;
#endif
	double	kTol = 1.e-5;
#if 0
	cout << "position is equivalent? " << (position.isEquivalent(hitPoint_in_kObject, kTol) ? "True" : "False") << " (d = " << position.distanceTo(hitPoint_in_kObject) << ")" << endl;
#endif
	assert(id == polyId);
	if (id != polyId)
		MGlobal::displayWarning("id != polyId");
	bool	b = position.isEquivalent(hitPoint_in_kObject, kTol);
	assert(b);
	if (!b)
		MGlobal::displayWarning("position.isEquivalent? False");
	return ms;
}
#endif
#endif
MStatus	CBrushContext::doPress(MEvent& event)
{
#ifdef move__DEBUG2
	cout << "CBrushContext::doPress()" << endl;
#endif
	event.getPosition(start_x, start_y);
#ifdef _DEBUG
	cout << "CBrushContext::doPress(): m(" << start_x << ", " << start_y << ")" << endl;
#endif
#pragma warning(disable: 4127)
#if 1
	MDagPath	cameraDagPath;
	M3dView::active3dView().getCamera(cameraDagPath);
	MFnCamera	fnCamera(cameraDagPath);
#if 1
	if (0) {
#else
	if (__brushType == CBrushToolCommand::BT_CUT) {
		InsideArray_Cut		insideArray_Cut__;
		MStatus	ms = setInsideArray_Cut(insideArray_Cut__);
		if (insideArray_Cut__.length()) {
			__toolCommandPtr = (CBrushToolCommand*) newToolCommand();
			__toolCommandPtr->setActiveSelectionArrayPtr(&__activeSelectionArray);
			InsideArray_Cut&	insideArray_Cut = ((CBrushToolCommand_Cut*) __toolCommandPtr)->getInsideArray();
			for (unsigned n = 0; n < insideArray_Cut__.length(); ++n) {
				DataArrayPtr_Cut	p = new DataArray_Cut(insideArray_Cut__(n));
				insideArray_Cut.append(p);
			}
		} else
			__toolCommandPtr = NULL;
	} else if (__brushType == CBrushToolCommand::BT_SCALE) {
		InsideArray_Scale	insideArray_Scale__;
		MStatus	ms = setInsideArray_Scale(insideArray_Scale__);
		if (insideArray_Scale__.length()) {
			__toolCommandPtr = (CBrushToolCommand*) newToolCommand();
			__toolCommandPtr->setActiveSelectionArrayPtr(&__activeSelectionArray);
			InsideArray_Scale&	insideArray_Scale = ((CBrushToolCommand_Scale*) __toolCommandPtr)->getInsideArray();
			for (unsigned n = 0; n < insideArray_Scale__.length(); ++n) {
				DataArrayPtr_Scale	p = new DataArray_Scale(insideArray_Scale__(n));
				insideArray_Scale.append(p);
			}
		} else
			__toolCommandPtr = NULL;
#endif
	} else if (__brushType == CBrushToolCommand::BT_TRANSLATE) {
		InsideArray_Translate	insideArray_Translate__;
		MStatus	ms = setInsideArray_Translate(insideArray_Translate__);
		if (insideArray_Translate__.length()) {
			__toolCommandPtr = (CBrushToolCommand*) newToolCommand();
			__toolCommandPtr->setActiveSelectionArrayPtr(&__activeSelectionArray);
			Arg_Translate&	arg_Translate = ((CBrushToolCommand_Translate*) __toolCommandPtr)->getArg();
			arg_Translate.xDir = fnCamera.rightDirection(MSpace::kWorld);
			arg_Translate.xDir.normalize();
			arg_Translate.yDir = fnCamera.upDirection(MSpace::kWorld);
			arg_Translate.yDir.normalize();
			InsideArray_Translate&	insideArray_Translate = ((CBrushToolCommand_Translate*) __toolCommandPtr)->getInsideArray();
			for (unsigned n = 0; n < insideArray_Translate__.length(); ++n) {
				DataArrayPtr_Translate	p = new DataArray_Translate(insideArray_Translate__(n));
				insideArray_Translate.append(p);
			}
		} else
			__toolCommandPtr = NULL;
	} else if (__brushType == CBrushToolCommand::BT_ROTATE) {
		InsideArray_Rotate	insideArray_Rotate__;
		MStatus	ms = setInsideArray_Rotate(insideArray_Rotate__);
		if (insideArray_Rotate__.length()) {
			__toolCommandPtr = (CBrushToolCommand*) newToolCommand();
			__toolCommandPtr->setActiveSelectionArrayPtr(&__activeSelectionArray);
			Arg_Rotate&		arg_Rotate = ((CBrushToolCommand_Rotate*) __toolCommandPtr)->getArg();
			arg_Rotate.axis = fnCamera.viewDirection(MSpace::kWorld);
			InsideArray_Rotate&	insideArray_Rotate = ((CBrushToolCommand_Rotate*) __toolCommandPtr)->getInsideArray();
			for (unsigned n = 0; n < insideArray_Rotate__.length(); ++n) {
				DataArrayPtr_Rotate	p = new DataArray_Rotate(insideArray_Rotate__(n));
				insideArray_Rotate.append(p);
			}
		} else
			__toolCommandPtr = NULL;
#if 1
#if 0
	} else if (__brushType == CBrushToolCommand::BT_TENTACLE_TWIST) {
		InsideArray_Tentacle_Twist	insideArray_Tentacle_Twist__;
		MStatus	ms = setInsideArray_Tentacle_Twist(insideArray_Tentacle_Twist__);
		if (insideArray_Tentacle_Twist__.length()) {
			__toolCommandPtr = (CBrushToolCommand*) newToolCommand();
			__toolCommandPtr->setActiveSelectionArrayPtr(&__activeSelectionArray);
			InsideArray_Tentacle_Twist&	insideArray_Tentacle_Twist = ((CBrushToolCommand_Tentacle_Twist*) __toolCommandPtr)->getInsideArray();
			for (unsigned n = 0; n < insideArray_Tentacle_Twist__.length(); ++n) {
				DataArrayPtr_Tentacle_Twist	p = new DataArray_Tentacle_Twist(insideArray_Tentacle_Twist__(n));
				insideArray_Tentacle_Twist.append(p);
			}
		} else
			__toolCommandPtr = NULL;
	} else if (__brushType == CBrushToolCommand::BT_TENTACLE_SHAPE) {
		InsideArray_Tentacle_Shape	insideArray_Tentacle_Shape__;
		MStatus	ms = setInsideArray_Tentacle_Shape(insideArray_Tentacle_Shape__);
		if (insideArray_Tentacle_Shape__.length()) {
			__toolCommandPtr = (CBrushToolCommand*) newToolCommand();
			__toolCommandPtr->setActiveSelectionArrayPtr(&__activeSelectionArray);
			InsideArray_Tentacle_Shape&	insideArray_Tentacle_Shape = ((CBrushToolCommand_Tentacle_Shape*) __toolCommandPtr)->getInsideArray();
			for (unsigned n = 0; n < insideArray_Tentacle_Shape__.length(); ++n) {
				DataArrayPtr_Tentacle_Shape	p = new DataArray_Tentacle_Shape(insideArray_Tentacle_Shape__(n));
				insideArray_Tentacle_Shape.append(p);
			}
		} else
			__toolCommandPtr = NULL;
#endif
#if 1
	} else if (
		__brushType == CBrushToolCommand::BT_TENTACLE_CREATE ||
		__brushType == CBrushToolCommand::BT_TENTACLE_MOVE
	) {
#if 1
		__toolCommandPtr = NULL;
		bool	valid = __mesh.dagPath().isValid();
		if (valid && __brushType == CBrushToolCommand::BT_TENTACLE_MOVE) {
			if (__activeSelectionArray.length() == 0) {
				MGlobal::displayWarning("Nothgin selected");
				valid = false;
			}
		}
		if (valid) {
			__toolCommandPtr = (CBrushToolCommand*) newToolCommand();
			CBrushToolCommand_Tentacle_Create&	toolCommand_Tentacle_Create = *((CBrushToolCommand_Tentacle_Create*) __toolCommandPtr);
			Arg_Tentacle_Create&	arg_Tentacle_Create = toolCommand_Tentacle_Create.arg_Tentacle_Create();
#if 1
			bool&	hitted = arg_Tentacle_Create.hitted;
			hitted = false;
		{
			MFloatPoint	hitPoint;
#if 1
#if 1
#else
			int&	hitTriangleId	= arg_Tentacle_Create.hitTriangleId;
#endif
			float&	hitBary1	= arg_Tentacle_Create.hitBary1;
			float&	hitBary2	= arg_Tentacle_Create.hitBary2;
			int*	hitVertexList	= arg_Tentacle_Create.hitVertexList;
#endif
			int	polyId;
			MPoint	position;
			MVector	normal;
			MStatus	ms = FindClosestIntersection(
				__mesh,
				start_x, start_y, hitted, hitPoint,
#if MAYA_API_VERSION >= 200800
				&__accelParams,
#else
#if MAYA_API_VERSION == 700
				NULL,
#else
				NULL,
#endif
#endif
#if 1
#if 1
				hitVertexList, hitBary1, hitBary2,
#else
				hitTriangleId, hitBary1, hitBary2,
#endif
#endif
				polyId, position, normal
			);
#ifdef _DEBUG
#if 0
			cout << "FindClosestIntersection returns |" << ms << "|" << endl;
#endif
#endif
			if (hitted) {
#if 1
				arg_Tentacle_Create.polyId = polyId;
				arg_Tentacle_Create.position = position;
				arg_Tentacle_Create.normal = normal;
#else
				arg_Tentacle_Create.hitPoint = hitPoint;
#endif
			}
#ifdef _DEBUG
#if 0
			cout << "doPress: hitted? " << (hitted ? "True" : "False");
			if (hitted)
				cout << ": " << hitPoint << endl;
			else
				cout << endl;
#endif
#endif
			toolCommand_Tentacle_Create.doPress(__nodeDagPath);
		}
#endif
		}
#else
		InsideArray_Tentacle_Create	insideArray_Tentacle_Create__;
		MStatus	ms = setInsideArray_Tentacle_Create(insideArray_Tentacle_Create__);
		if (insideArray_Tentacle_Create__.length()) {
			__toolCommandPtr = (CBrushToolCommand*) newToolCommand();
			__toolCommandPtr->setActiveSelectionArrayPtr(&__activeSelectionArray);
			Arg_Tentacle_Create&	arg_Tentacle_Create = ((CBrushToolCommand_Tentacle_Create*) __toolCommandPtr)->getArg();
#if 1
			bool&	hitted = arg_Tentacle_Create.hitted;
			hitted = false;
	{
			MFloatPoint	hitPoint;
			int	polyId;
			MPoint	position;
			MVector	normal;
			ms = FindClosestIntersection(
				__mesh,
				start_x, start_y, hitted, hitPoint,
#if MAYA_API_VERSION >= 200800
				&__accelParams,
#else
#if MAYA_API_VERSION == 700
				NULL,
#else
				NULL,
#endif
#endif
				polyId, position, normal
			);
#ifdef _DEBUG
#if 0
			cout << "FindClosestIntersection returns |" << ms << "|" << endl;
#endif
#endif
			if (hitted) {
#if 1
				arg_Tentacle_Create.polyId = polyId;
				arg_Tentacle_Create.position = position;
				arg_Tentacle_Create.normal = normal;
#else
				arg_Tentacle_Create.hitPoint = hitPoint;
#endif
			}
#ifdef _DEBUG
#if 0
			cout << "doPress: hitted? " << (hitted ? "True" : "False");
			if (hitted)
				cout << ": " << hitPoint << endl;
			else
				cout << endl;
#endif
#endif
	}
#endif
			InsideArray_Tentacle_Create&	insideArray_Tentacle_Create = ((CBrushToolCommand_Tentacle_Create*) __toolCommandPtr)->getInsideArray();
			for (unsigned n = 0; n < insideArray_Tentacle_Create__.length(); ++n) {
				DataArrayPtr_Tentacle_Create	p = new DataArray_Tentacle_Create(insideArray_Tentacle_Create__(n));
				insideArray_Tentacle_Create.append(p);
			}
			((CBrushToolCommand_Tentacle_Create*) __toolCommandPtr)->doPress();
			M3dView	view = M3dView::active3dView(&ms);
			ms = view.refresh();
		} else
			__toolCommandPtr = NULL;
#endif
#endif
#endif
	} else {
		__toolCommandPtr = NULL;
	}
#else
	if (insideSelectionArray__.length()) {
		CBrushToolCommand_Cut::typeStr.set((double) insideSelectionArray__(0).guideArray.length());
		__toolCommandPtr = (CBrushToolCommand*) newToolCommand();
		SelectionArray&	insideSelectionArray = ((CBrushToolCommand_Cut*) __toolCommandPtr)->getInsideSelectionArray();
		for (unsigned n = 0; n < insideSelectionArray__.length(); ++n) {
			SelectionPtr	p = new CSelection(insideSelectionArray__(n));
			insideSelectionArray.append(p);
		}
#if 1
	} else
#else
	} else
#endif
		__toolCommandPtr = NULL;
#endif
#pragma warning(default: 4127)
	return MStatus::kSuccess;
}
MStatus	CBrushContext::doDrag(MEvent& event)
{
#ifdef move__DEBUG2
	cout << "CBrushContext::doDrag()" << endl;
#endif
	event.getPosition(last_x, last_y);
#ifdef _DEBUG
	cout << "CBrushContext::doDrag(): m(" << last_x << ", " << last_y << ")" << endl;
#endif
	if (!__toolCommandPtr)
		return MStatus::kSuccess;
#pragma warning(disable: 4127)
#if 1
	if (0) {
#else
	if (__brushType == CBrushToolCommand::BT_SCALE) {
		__toolCommandPtr->undoIt(false);
		double	f = 1 + .1 * (last_y - start_y) / __brushCircle.getRadius();
		Arg_Scale&		arg_Scale = ((CBrushToolCommand_Scale*) __toolCommandPtr)->getArg();
		arg_Scale.factor = f;
		InsideArray_Scale&	insideArray_Scale = ((CBrushToolCommand_Scale*) __toolCommandPtr)->getInsideArray();
		for (unsigned n = 0; n < insideArray_Scale.length(); ++n) {
			DataArray_Scale&	scaleArray = insideArray_Scale(n);
			for (unsigned k = 0; k < scaleArray.dataArray.length(); ++k) {
				Data_Scale&	dataScale = scaleArray.dataArray(k);
				dataScale.segmentLength = dataScale.segmentLength__ * f;
			}
		}
		MStatus	ms = __toolCommandPtr->redoIt(false);
#endif
	} else if (__brushType == CBrushToolCommand::BT_TRANSLATE) {
		__toolCommandPtr->undoIt(false);
		Arg_Translate&		arg_Translate = ((CBrushToolCommand_Translate*) __toolCommandPtr)->getArg();
		arg_Translate.deltaX = (last_x - start_x) * arg_Translate.xDir + (last_y - start_y) * arg_Translate.yDir;
		MStatus	ms = __toolCommandPtr->redoIt(false);
	} else if (__brushType == CBrushToolCommand::BT_ROTATE) {
		__toolCommandPtr->undoIt(false);
		double	byAngleAmount = CBrushToolCommand_Rotate::ROTATE_NORMALIZATION * (start_x - last_x) / __brushCircle.getRadius();
		Arg_Rotate&		arg_Rotate = ((CBrushToolCommand_Rotate*) __toolCommandPtr)->getArg();
		arg_Rotate.byAngleAmount = byAngleAmount;
		MStatus	ms = __toolCommandPtr->redoIt(false);
#if 1
#if 0
	} else if (__brushType == CBrushToolCommand::BT_TENTACLE_TWIST) {
		__toolCommandPtr->undoIt(false);
		double	byAngleAmount = CBrushToolCommand_Tentacle_Twist::ROTATE_NORMALIZATION * (start_x - last_x) / __brushCircle.getRadius();
		Arg_Tentacle_Twist&	arg_Tentacle_Twist = ((CBrushToolCommand_Tentacle_Twist*) __toolCommandPtr)->getArg();
		arg_Tentacle_Twist.byAngleAmount = byAngleAmount;
		MStatus	ms = __toolCommandPtr->redoIt(false);
	} else if (__brushType == CBrushToolCommand::BT_TENTACLE_SHAPE) {
		__toolCommandPtr->undoIt(false);
		Arg_Tentacle_Shape&	arg_Tentacle_Shape = ((CBrushToolCommand_Tentacle_Shape*) __toolCommandPtr)->getArg();
		arg_Tentacle_Shape.factor = 1 + .1 * (last_y - start_y) / __brushCircle.getRadius();
		cout << "arg_Tentacle_Shape.factor = " << arg_Tentacle_Shape.factor << endl;
		MStatus	ms = __toolCommandPtr->redoIt(false);
#endif
#if 1
	} else if (
		__brushType == CBrushToolCommand::BT_TENTACLE_CREATE ||
		__brushType == CBrushToolCommand::BT_TENTACLE_MOVE
	) {
		Arg_Tentacle_Create&	arg_Tentacle_Create = ((CBrushToolCommand_Tentacle_Create*) __toolCommandPtr)->arg_Tentacle_Create();
		if (arg_Tentacle_Create.hitted) {
			bool	hitted;
			MFloatPoint	hitPoint;
#if 1
#if 1
			int	hitVertexList[3];
#else
			int	hitTriangleId;
#endif
			float	hitBary1, hitBary2;
#endif
			int	polyId;
			MPoint	position;
			MVector	normal;
			MStatus	ms = FindClosestIntersection(
				__mesh,
				last_x, last_y, hitted, hitPoint,
#if MAYA_API_VERSION >= 200800
				&__accelParams,
#else
#if MAYA_API_VERSION == 700
				NULL,
#else
				NULL,
#endif
#endif
#if 1
#if 1
				hitVertexList, hitBary1, hitBary2,
#else
				hitTriangleId, hitBary1, hitBary2,
#endif
#endif
				polyId, position, normal
			);
#ifdef _DEBUG
#if 0
			cout << "FindClosestIntersection returns |" << ms << "|" << endl;
#endif
#endif
			if (hitted) {
#if 1
				arg_Tentacle_Create.polyId = polyId;
				arg_Tentacle_Create.position = position;
				arg_Tentacle_Create.normal = normal;
#else
				arg_Tentacle_Create.hitPoint = hitPoint;
#endif
			}
#ifdef _DEBUG
#if 0
			cout << "doDrag: hitted? " << (hitted ? "True" : "False");
			if (hitted)
				cout << ": " << hitPoint << endl;
			else
				cout << endl;
#endif
#endif
			((CBrushToolCommand_Tentacle_Create*) __toolCommandPtr)->doDrag(__nodeDagPath);
			M3dView	view = M3dView::active3dView(&ms);
			ms = view.refresh();
		}
#endif
#endif
#if 1
	}
#else
	}
#endif
#pragma warning(default: 4127)
	return MStatus::kSuccess;
}
MStatus	CBrushContext::doRelease(MEvent& event)
{
#ifdef move__DEBUG2
	cout << "CBrushContext::doRelease()" << endl;
#endif
	event.getPosition( last_x, last_y );
#ifdef move__DEBUG
	cout << "CBrushContext::doRelease(): m(" << last_x << ", " << last_y << ")" << endl;
#endif
#if 0
{
	MStatus	ms;
	MPoint	worldRaySrcPoint;
	MVector	worldRayDirVector;
	M3dView	view = M3dView::active3dView(&ms);
	ms = view.viewToWorld(last_x, last_y, worldRaySrcPoint, worldRayDirVector);
	cout << "(" << worldRaySrcPoint.x << ", " << worldRaySrcPoint.y << ", " << worldRaySrcPoint.z << "; " << worldRaySrcPoint.w << endl;
	cout << "(" << worldRayDirVector.x << ", " << worldRayDirVector.y << ", " << worldRayDirVector.z << ")" << endl;
	MPoint	nearClipPt;
	MPoint	farClipPt;
	ms = view.viewToWorld(last_x, last_y, nearClipPt, farClipPt);
	cout << "(" << nearClipPt.x << ", " << nearClipPt.y << ", " << nearClipPt.z << "; " << nearClipPt.w << endl;
	cout << "(" << farClipPt.x << ", " << farClipPt.y << ", " << farClipPt.z << "; " << farClipPt.w << endl;
#if 0
	MVector	v2(farClipPt);
	MVector	v1(nearClipPt);
	v2 = v2 - v1;
	ms = v2.normalize();
	cout << "(" << v2.x << ", " << v2.y << ", " << v2.z << endl;
#endif
	MDagPath	camera;
	ms = view.getCamera(camera);
	MFnCamera	fnCamera(camera, &ms);
	MSpace::Space	spc = MSpace::kWorld;
	MVector	upDir = fnCamera.upDirection(spc, &ms);
	MVector	rightDir = fnCamera.rightDirection(spc, &ms);
}
#endif
	if (__toolCommandPtr) {
#if 1
#if 1
		if (
			__brushType == CBrushToolCommand::BT_TENTACLE_CREATE ||
			__brushType == CBrushToolCommand::BT_TENTACLE_MOVE
		) {
			cout << "doRelease: hitted? " << (((CBrushToolCommand_Tentacle_Create*) __toolCommandPtr)->arg_Tentacle_Create().hitted ? "True" : "False") << endl;
			((CBrushToolCommand_Tentacle_Create*) __toolCommandPtr)->doRelease(__nodeDagPath);
		}
#endif
#else
		if (__brushType == CBrushToolCommand::BT_CUT) {
			MStatus	ms = __toolCommandPtr->redoIt(false);
		}
#endif
#if 1
		__toolCommandPtr->rebuildActiveSelectionList(false);
#else
		buildActiveSelectionList();
#endif
#ifdef _DEBUG
#endif
		MStatus	ms;
		M3dView	view = M3dView::active3dView(&ms);
		ms = __toolCommandPtr->finalize();
		view.refresh(true);
		__toolCommandPtr = NULL;
	}
	return MStatus::kSuccess;
}
MStatus	CBrushContext::doEnterRegion(MEvent& event)
{
#ifdef move__DEBUG2
	cout << "CBrushContext::doEnterRegion()" << endl;
	cout << "CBrushContext::doEnterRegion(): Thread ID = " << GetCurrentThreadId() << endl;
#endif
	__brushCircle.sync(event);
	return setHelpString(helpString);
}
#if 0
#endif
void	CBrushContext::toolOffCleanup()
{
#ifdef move__DEBUG2
	cout << "CBrushContext::toolOffCleanup()" << endl;
#endif
	MStatus	ms;
	ms = MTimerMessage::removeCallback(__timerCallbackId);
	ms = MModelMessage::removeCallback(__activeListModifiedCallbackId);
#if 1
#if 1
	ms = MUserEventMessage::deregisterUserEvent(tentacleShape::TentacleArrayUpdated);
	ms = MMessage::removeCallback(__userEventTentacleArrayUpdatedCallbackId);
#endif
#endif
	__activeSelectionArray.clear();
}
void	CBrushContext::__activeListModifiedCallback(void* contextPtr)
{
#ifdef move__DEBUG2
	cout << "__activeListModifiedCallback()" << endl;
#endif
	CBrushContext&	context = *((CBrushContext*) contextPtr);
	if (!context.buildActiveSelectionList()) {
		cerr << "ERROR: CBrushContext::buildActiveSelectionList(): Failure" << endl;
		return;
	}
}
#if 1
#if 1
void	CBrushContext::__userEventTentacleArrayUpdatedCallback(void* contextPtr)
{
#ifdef move__DEBUG2
	cout << "__userEventTentacleArrayUpdatedCallback()" << endl;
#endif
	if (!contextPtr)
		return;
	CBrushContext&	context = *((CBrushContext*) contextPtr);
	if (!context.buildActiveSelectionList()) {
		cerr << "ERROR: CBrushContext::buildActiveSelectionList(): Failure" << endl;
		return;
	}
}
#endif
#endif
MStatus	CBrushContext::buildActiveSelectionList()
{
#ifdef _DEBUG
#if 1
	cout << "CBrushContext::buildActiveSelectionList: Brush-type:= |" << (__brushType == CBrushToolCommand::BT_TRANSLATE ? "Translate" : (__brushType == CBrushToolCommand::BT_ROTATE ? "Rotate" : (__brushType == CBrushToolCommand::BT_TENTACLE_CREATE ? "Create" : (__brushType == CBrushToolCommand::BT_TENTACLE_MOVE ? "Move" : ("Unknown"))))) << "|" << endl;
#else
	cout << "CBrushContext::buildActiveSelectionList: Brush-type:= |" << (__brushType == CBrushToolCommand::BT_SCALE ? "Scale" : (__brushType == CBrushToolCommand::BT_CUT ? "Cut" : (__brushType == CBrushToolCommand::BT_TRANSLATE ? "Translate" : (__brushType == CBrushToolCommand::BT_ROTATE ? "Rotate" : "Unknown")))) << "|" << endl;
#endif
#endif
	MStatus	ms;
	__activeSelectionArray.clear();
#pragma	warning(disable: 4127)
do {
#if 0
	MGlobal::MSelectionMode	mode = MGlobal::selectionMode(&ms); assert(ms);
#ifdef _DEBUG
	cout << "MSelectionMode:= " << (mode == MGlobal::kSelectObjectMode ? "MGlobal::kSelectObjectMode" :
		(mode == MGlobal::kSelectComponentMode ? "MGlobal::kSelectComponentMode" :
		(mode == MGlobal::kSelectRootMode ? "MGlobal::kSelectRootMode" :
		(mode == MGlobal::kSelectLeafMode ? "MGlobal::kSelectLeafMode" :
		(mode == MGlobal::kSelectTemplateMode ? "MGlobal::kSelectTemplateMode" : "kSelectUnknownMode"
		))))) << endl;
#endif
	if (mode != MGlobal::kSelectComponentMode) {
#ifdef _DEBUG
		cerr << "WARNING: CBrushContext::buildActiveSelectionList(): Not a MGlobal::kSelectComponentMode: ";
		cerr << "MSelectionMode:= " << (mode == MGlobal::kSelectObjectMode ? "MGlobal::kSelectObjectMode" :
			(mode == MGlobal::kSelectComponentMode ? "MGlobal::kSelectComponentMode" :
			(mode == MGlobal::kSelectRootMode ? "MGlobal::kSelectRootMode" :
			(mode == MGlobal::kSelectLeafMode ? "MGlobal::kSelectLeafMode" :
			(mode == MGlobal::kSelectTemplateMode ? "MGlobal::kSelectTemplateMode" : "kSelectUnknownMode"
			))))) << endl;
#endif
		break;
	}
#endif
	MSelectionMask	mask = MGlobal::componentSelectionMask(&ms);
	if (!mask.intersects(MSelectionMask::kSelectComponentsMask)) {
#ifdef _DEBUG
		cerr << "WARNING: CBrushContext::buildActiveSelectionList(): Not Set MSelectionMask::kSelectComponentMask Bit" << endl;
#endif
		break;
	}
	MSelectionList	incomingList;
	MGlobal::getActiveSelectionList(incomingList);
	MSelectionList	outgoingList;
	M3dView	view = M3dView::active3dView(&ms);
	bool	failed = false;
	for (unsigned iList = 0; iList < incomingList.length(); ++iList) {
		MDagPath	dagPath;
		MObject		component;
		ms = incomingList.getDagPath(iList, dagPath, component);
		MFnDependencyNode	fnDepNode(dagPath.node());
		SelectionPtr	selectionPtr = NULL;
		for (unsigned i = 0; i < __activeSelectionArray.length(); ++i) {
			SelectionPtr	p = __activeSelectionArray[i];
			if (p->dagPath == dagPath) {
				selectionPtr = p;
				break;
			}
		}
		int		selectBy = -1;
		int		numPoints = -1;
		MMatrix*	worldMatrixPtr = NULL;
#if 1
		kSkinSmoothBind	skinSmoothBind;
#endif
		if (selectionPtr) {
			selectBy = selectionPtr->selectBy;
			numPoints = selectionPtr->numPoints;
			worldMatrixPtr = &selectionPtr->worldMatrix;
		} else {
			selectionPtr = new CSelection();
			__activeSelectionArray.append(selectionPtr);
			selectionPtr->dagPath = dagPath;
		{
			MPlug	plug_selectBy = fnDepNode.findPlug("selectBy", false);
			plug_selectBy.getValue(selectBy);
			selectionPtr->selectBy = selectBy;
		}
		{
			MPlug	plug_numPointsInGuide = fnDepNode.findPlug("numPointsInGuide", false);
			plug_numPointsInGuide.getValue(numPoints);
			selectionPtr->numPoints = numPoints;
		}
		{
			MDagPath	dagPathShape;
			ms = kNode::findExternalDagPath(dagPath.node(), dagPathShape);
			if (ms != MStatus::kSuccess && ms != MStatus::kNotFound) {
				cerr << "Fatal: Cannot set world matrix" << endl;
				failed = true;
				break;
			}
			MObject	transform = (ms == MStatus::kSuccess ? dagPathShape : dagPath).transform(&ms); if (!ms) cerr << ms << endl;
			MFnTransform	fnTransform(transform, &ms); if (!ms) cerr << ms << endl;
			MTransformationMatrix	xformMatrix = fnTransform.transformation(&ms); if (!ms) cerr << ms << endl;
			selectionPtr->worldMatrix = xformMatrix.asMatrix();
			cout << selectionPtr->worldMatrix << endl;
#if 1
#if 1
			MPlug	plug_tentacleArray = fnDepNode.findPlug("tentacleArray", true, &ms);
			ms = skinSmoothBind.set(dagPathShape, plug_tentacleArray);
			selectionPtr->hasSkinCluster = skinSmoothBind.hasSkinCluster();
#endif
#endif
		}
			worldMatrixPtr = &selectionPtr->worldMatrix;
		}
		if (
#if 1
			0 &&
#endif
#if 0
			this->m__iSelectBy == apiMesh::selectByVertex &&
#endif
			component.hasFn(MFn::kDoubleIndexedComponent) &&
			component.hasFn(MFn::kSurfaceCVComponent)
		) {
			MStatus	ms;
			MFnDoubleIndexedComponent	fnCVComp(component, &ms);
			int	len = fnCVComp.elementCount(&ms);
			for (int iElem = 0; iElem < len; ++iElem) {
				int	uIndex;
				int	vIndex;
				ms = fnCVComp.getElement(iElem, uIndex, vIndex);
#ifdef move__DEBUG2
				cout << "\t(MFn::kDoubleIndexedComponent | MFn::kSurfaceCVComponent) component[" << iElem << "]: uIndex = " << uIndex << ", vIndex = " << vIndex << endl;
#endif
				assert(uIndex >= 0);
				assert(vIndex >= 0 && vIndex < 8 * sizeof(unsigned));
				if (vIndex < 0 || vIndex >= 8 * sizeof(unsigned))
					MGlobal::displayError("vIndex < 0 || vIndex >= 8 * sizeof(unsigned)");
				SelectedGuidePtr	guidePtr = NULL;
				for (unsigned i = 0; i < selectionPtr->guideArray.length(); ++i) {
					SelectedGuidePtr	p = selectionPtr->guideArray[i];
					if (p->uIndex == uIndex) {
						guidePtr = p;
						break;
					}
				}
				if (!guidePtr) {
					guidePtr = new SelectedGuide;
					selectionPtr->guideArray.append(guidePtr);
					guidePtr->uIndex = uIndex;
					ms = guidePtr->offsets.setLength(selectionPtr->numPoints); assert(ms);
#if 1
					ms = guidePtr->transversals.setLength(selectionPtr->numPoints); assert(ms);
#endif
					guidePtr->vertexBits = (unsigned) -1;
				}
				unsigned&	vertexBits = guidePtr->vertexBits;
				if (vertexBits == (unsigned) -1) {
					vertexBits = 0;
				}
				unsigned	bit = 1 << vIndex;
				assert(!(vertexBits & bit));
				vertexBits |= bit;
			}
		} else if (
#if 0
			this->m__iSelectBy == apiMesh::selectByGuide &&
#endif
			component.hasFn(MFn::kSingleIndexedComponent) &&
			component.hasFn(MFn::kMeshEdgeComponent)
		) {
			MStatus	ms;
			MFnSingleIndexedComponent	fnEdgeComp(component, &ms);
			int	len = fnEdgeComp.elementCount(&ms);
#if 1
			MPlug	plug_tentacleArray = fnDepNode.findPlug("tentacleArray", true, &ms);
#else
			MPlug	plug_cp_array = fnDepNode.findPlug("controlPoints", true, &ms);
#if 1
#if 0
			MPlug	plug_array_transversal = fnDepNode.findPlug("transversal", true, &ms);
#endif
#endif
#endif
			for (int iElem = 0; iElem < len; ++iElem) {
				int	index = fnEdgeComp.element(iElem, &ms);
			{
				SelectedGuidePtr	guidePtr = new SelectedGuide;
				selectionPtr->guideArray.append(guidePtr);
				guidePtr->uIndex = index;
				ms = guidePtr->offsets.setLength(selectionPtr->numPoints); assert(ms);
#if 1
				ms = guidePtr->transversals.setLength(selectionPtr->numPoints); assert(ms);
#endif
				guidePtr->vertexBits = (unsigned) -1;
#if 1
				MPlug	plug_tentacleArray_element = plug_tentacleArray.elementByPhysicalIndex(index, &ms);
				MObject	valData;
				MFnNumericData		numericData;
				MFnVectorArrayData	vectorData;
				MPlug	plug_root = plug_tentacleArray_element.child(TENTACLE_ARRAY_ROOT, &ms);
				ms = plug_root.getValue(valData);
				ms = numericData.setObject(valData);
				MPoint	root;
				ms = numericData.getData(root[0], root[1], root[2]);
				MPlug	plug_longitudinal = plug_tentacleArray_element.child(TENTACLE_ARRAY_LOGITUDINAL, &ms);
				ms = plug_longitudinal.getValue(valData);
				ms = vectorData.setObject(valData);
#if 1
				ms = vectorData.copyTo(guidePtr->offsets);
#else
				guidePtr->offsets = vectorData.array(&ms);
#endif
				ms = guidePtr->offsets.insert(root, 0);
#if 1
				if (selectionPtr->hasSkinCluster) {
					float	segment_length = (float) guidePtr->offsets[1].length();
					MFloatVectorArray	string_wPoints;
					MVector&	root_wNormal = guidePtr->root_wNormal;
					ms = skinSmoothBind.getPoints(index, plug_tentacleArray_element, segment_length, root_wNormal, string_wPoints);
					guidePtr->offsets.clear();
					for (unsigned n = 0; n < string_wPoints.length(); ++n)
						ms = guidePtr->offsets.append(string_wPoints[n]);
				}
#endif
				MPlug	plug_transversal = plug_tentacleArray_element.child(TENTACLE_ARRAY_TRANSVERSAL, &ms);
				ms = plug_transversal.getValue(valData);
				ms = vectorData.setObject(valData);
#if 1
				ms = vectorData.copyTo(guidePtr->transversals);
#else
				guidePtr->transversals = vectorData.array(&ms);
#endif
#else
				for (int k = 0; k < numPoints; ++k) {
					MPlug	plug_element_offset = plug_cp_array.elementByLogicalIndex(index * numPoints + k);
					MObject	data_offset;
					ms = plug_element_offset.getValue(data_offset);
					MFnNumericData	val_offset(data_offset, &ms);
					MVector&	offset = guidePtr->offsets[k];
					ms = val_offset.getData(offset.x, offset.y, offset.z);
#if 1
#if 0
					MPlug	plug_element_transversal = plug_array_transversal.elementByLogicalIndex(index * numPoints + k);
					MObject	data_transversal;
					ms = plug_element_transversal.getValue(data_transversal);
					MFnNumericData	val_transversal(data_transversal, &ms);
					MVector&	transversal = guidePtr->transversals[k];
					ms = val_transversal.getData(transversal.x, transversal.y, transversal.z);
#endif
#endif
				}
#endif
			}
#ifdef move__DEBUG2
				cout << "\t(MFn::kSingleIndexedComponent | MFn::kMeshEdgeComponent) component[" << iElem << "]: index = " << index << endl;
#endif
			}
		}
	}
	if (failed) {
		__activeSelectionArray.clear();
		break;
	}
#if 0
	for (unsigned i = 0; i < __activeSelectionArray.length(); ++i) {
		SelectionPtr	selectionPtr = __activeSelectionArray[i];
		if (selectionPtr->selectBy != 1) {
			continue;
		}
		int	numPoints = selectionPtr->numPoints;
		MFnDependencyNode	fnDepNode(selectionPtr->dagPath.node());
		MPlug	plug_cp_array = fnDepNode.findPlug("controlPoints", true, &ms);
#if 1
#if 0
		MPlug	plug_array_transversal = fnDepNode.findPlug("transversal", true, &ms);
#endif
#endif
		for (unsigned k = 0; k < selectionPtr->guideArray.length(); ++k) {
			SelectedGuidePtr	guidePtr = selectionPtr->guideArray[k];
			int	index = guidePtr->uIndex;
			for (int k = 0; k < numPoints; ++k) {
				MPlug	plug_element_offset = plug_cp_array.elementByLogicalIndex(index * numPoints + k);
				MObject	data_offset;
				ms = plug_element_offset.getValue(data_offset);
				MFnNumericData	val_offset(data_offset, &ms);
				MVector&	offset = guidePtr->offsets[k];
				ms = val_offset.getData(offset.x, offset.y, offset.z);
#if 1
#if 0
				MPlug	plug_element_transversal = plug_array_transversal.elementByLogicalIndex(index * numPoints + k);
				MObject	data_transversal;
				ms = plug_element_transversal.getValue(data_transversal);
				MFnNumericData	val_transversal(data_transversal, &ms);
				MVector&	transversal = guidePtr->transversals[k];
				ms = val_transversal.getData(transversal.x, transversal.y, transversal.z);
#endif
#endif
			}
		}
	}
#endif
} while (0);
#pragma	warning(default: 4127)
#ifdef _DEBUG
#endif
#if 1
#if 1
{
	MDagPath	dagPathShape;
	cout << "dagPathShape is valid? " << (dagPathShape.isValid() ? "True" : "False") << endl;
	__mesh.setObject(dagPathShape);
	__nodeDagPath = dagPathShape;
	if (__activeSelectionArray.length()) {
		const CSelection&	selection = __activeSelectionArray(0);
		ms = kNode::findExternalDagPath(selection.dagPath.node(), dagPathShape);
		ms = __mesh.setObject(dagPathShape);
		__nodeDagPath = selection.dagPath;
	} else {
		MSelectionList	incomingList;
		MGlobal::getActiveSelectionList(incomingList);
		for (unsigned iList = 0; iList < 1 && iList < incomingList.length(); ++iList) {
			MDagPath	dagPath;
			MObject		component;
			ms = incomingList.getDagPath(iList, dagPath, component);
			ms = dagPath.extendToShape();
			MDagPath	tentacleDagPathShape;
			ms = findTentacleShapeNode(dagPath.node(), tentacleDagPathShape);
			if (ms) {
				__mesh.setObject(dagPath);
				__nodeDagPath = tentacleDagPathShape;
			}
		}
	}
	if (!__nodeDagPath.isValid()) {
		MGlobal::displayError("__nodeDagPath: Not valid");
	}
#if 0
#if 1
	if (__mesh.dagPath().isValid()) {
#if 1
		ms = __skinSmoothBind.setSkinCluster(__mesh);
		ms = __skinSmoothBind.setGeometryMatrix();
		ms = __skinSmoothBind.setInverseBindingMatrixArray();
		ms = __skinSmoothBind.setWeightsArray();
		ms = __skinSmoothBind.setJointHierachy();
		ms = __skinSmoothBind.setJointWorldMatrixArray();
		MTime	t;
		for (int i = 0; i < 24; ++i) {
			t.setValue(1 + i);
			cout << "[#" << (1 + i) << "]: t = " << t.value() << endl;
			ms = __skinSmoothBind.setJointWorldMatrixArray(&t);
		}
#else
		ms = kSkinSmoothBind::setSkinCluster(__mesh, __skinSmoothBind.skinCluster());
		ms = kSkinSmoothBind::setGeometryMatrix(__skinSmoothBind.skinCluster(), __skinSmoothBind.geometry_matrix());
		ms = kSkinSmoothBind::setInverseBindingMatrixArray(__skinSmoothBind.skinCluster(), __skinSmoothBind.inverse_binding_matrix_array());
		ms = kSkinSmoothBind::setWeightsArray(__skinSmoothBind.skinCluster(), __skinSmoothBind.weights_array());
		MObjectArray	joints;
		ms = kSkinSmoothBind::setJointHierachy(__skinSmoothBind.skinCluster(), joints);
		cout << ms << endl;
		ms = kSkinSmoothBind::setJointWorldMatrixArray(joints, __skinSmoothBind.joint_world_matrix_array());
	MTime	t;
	for (int i = 0; i < 1; ++i) {
		t.setValue(1 + i);
		cout << "[#" << (1 + i) << "]: t = " << t.value() << endl;
		ms = kSkinSmoothBind::setJointWorldMatrixArray(joints, __skinSmoothBind.joint_world_matrix_array(), &t);
	}
#endif
	}
#else
	__numJoints = 0;
	__inverse_binding_matrix_array.clear();
	__weights_array.clear();
	if (__mesh.dagPath().isValid()) {
		cout << "MFnMesh::dagPath(): |" << __mesh.dagPath(&ms).fullPathName(&ms) << "|" << endl;
		MPlug	inMesh_plus = __mesh.findPlug("inMesh", false, &ms);
		MPlugArray	skinCluster_plug_array;
		bool	bFound = inMesh_plus.connectedTo(skinCluster_plug_array, true, false, &ms);
		cout << "found? " << (bFound ? "True" : "False") << endl;
		if (bFound && skinCluster_plug_array.length()) {
			ms = __skinCluster.setObject(skinCluster_plug_array[0].node(&ms));
	int	indexTestVertex = 0;
do {
#if 1
	MPlug	geometry_matrix_plug = __skinCluster.findPlug("geomMatrix", false, &ms);
	if (!ms) {cerr << ms << endl; break;}
	MObject	valData;
	ms = geometry_matrix_plug.getValue(valData);
	if (!ms) {cerr << ms << endl; break;}
	MFnMatrixData		matrixData;
	ms = matrixData.setObject(valData);
	if (!ms) {cerr << ms << endl; break;}
#if 1
	__geometry_matrix = matrixData.matrix(&ms);
	MMatrix&	geometry_matrix = __geometry_matrix;
#else
	MMatrix	geometry_matrix = matrixData.matrix(&ms);
#endif
	cout << "geometry matrix at the time of binding: " << geometry_matrix << endl;
#endif
#if 1
	MPlug	inverse_binding_matrix_plug_array = __skinCluster.findPlug("bindPreMatrix", false, &ms);
#if 1
#if 0
	MMatrix	inverse_binding_matrix[5];
#endif
#endif
	if (inverse_binding_matrix_plug_array.isArray()) {
		unsigned	numElements = inverse_binding_matrix_plug_array.numElements(&ms);
		cout << "# of joint(s) = " << numElements << endl;
		__numJoints = numElements;
		for (unsigned indexJoint = 0; indexJoint < numElements; ++indexJoint) {
			MPlug	inverse_binding_matrix_plug = inverse_binding_matrix_plug_array.elementByPhysicalIndex(indexJoint, &ms);
			if (!ms) {cerr << ms << endl; break;}
			MObject	valData;
			ms = inverse_binding_matrix_plug.getValue(valData);
			if (!ms) {cerr << ms << endl; break;}
			MFnMatrixData		matrixData;
			ms = matrixData.setObject(valData);
			if (!ms) {cerr << ms << endl; break;}
#if 1
#if 1
			MMatrix*	matrixPtr = new MMatrix;
			*matrixPtr = matrixData.matrix(&ms);
			__inverse_binding_matrix_array.append(matrixPtr);
#else
			inverse_binding_matrix[indexJoint] = matrixData.matrix(&ms);
#endif
#else
			MMatrix	inverse_binding_matrix = matrixData.matrix(&ms);
			if (!ms) {cerr << ms << endl; break;}
			cout << inverse_binding_matrix << endl;
#endif
		}
	}
#if 1
#if 1
	cout << "inverse binding matrix list at the time of binding:" << endl;
	for (unsigned i = 0; i < __numJoints; ++i)
		cout << "B^-1[" << i << "]: " << __inverse_binding_matrix_array(i) << endl;
#else
	for (int i = 0; i < 5; ++i)
		cout << "B^-1[" << i << "]: " << inverse_binding_matrix[i] << endl;
#endif
#endif
#endif
#if 1
	cout << "weights list:" << endl;
#if 1
#if 0
	MDoubleArray	weights_x;
#endif
#endif
	MPlug	weight_list_compound_plug_array = __skinCluster.findPlug("weightList", false, &ms);
	if (weight_list_compound_plug_array.isArray(&ms) && ms) {
		unsigned	numElements = weight_list_compound_plug_array.numElements(&ms);
		cout << "# of vertices = " << numElements << endl;
		for (unsigned indexVertex = 0; indexVertex < numElements; ++indexVertex) {
			MDoubleArray*	weightsPtr = new MDoubleArray;
			__weights_array.append(weightsPtr);
			MDoubleArray&	weights = *weightsPtr;
			MPlug	weights_compound_plug = weight_list_compound_plug_array.elementByPhysicalIndex(indexVertex, &ms);
			if (!ms) {cerr << ms << endl; break;}
			cout << weights_compound_plug.info(&ms) << endl;
			if (weights_compound_plug.isCompound(&ms) && ms) {
				unsigned	numChildren = weights_compound_plug.numChildren(&ms);
				if (!ms) {cerr << ms << endl; break;}
				if (numChildren != 1) {
					cerr << "ONLY one children(.weights) expected" << endl;
					break;
				}
				for (unsigned m = 0; m < numChildren; ++m) {
					MPlug	weight_plug_array = weights_compound_plug.child(m, &ms);
					if (!ms) {cerr << ms << endl; break;}
					if (weight_plug_array.isArray(&ms) && ms) {
						unsigned	numElements = weight_plug_array.numElements(&ms);
						cout << "weights[" << indexVertex << "]:";
						for (unsigned indexJoint = 0; indexJoint < numElements; ++indexJoint) {
							MPlug	weight_plug = weight_plug_array.elementByPhysicalIndex(indexJoint, &ms);
							double	weight;
							ms = weight_plug.getValue(weight);
							cout << " " <<  weight;
							weights.append(weight);
						}
						cout << endl;
					}
				}
			}
		}
	}
#if 1
	cout << "indexTestVertex = " << indexTestVertex << endl;
#if 1
	cout << "weights[" << indexTestVertex << "]: " << __weights_array(indexTestVertex) << endl;
#else
	cout << "weights_x: " << weights_x << endl;
#endif
#endif
#endif
#if 1
	MPlug	matrix_plug_array = __skinCluster.findPlug("matrix", false, &ms);
#if 1
#if 1
	kPtrArray<MMatrix>	joint_world_matrix_array;
#else
	MMatrix	joint_world_matrix[5];
#endif
#endif
	if (matrix_plug_array.isArray()) {
		unsigned	numElements = matrix_plug_array.numElements(&ms);
		cout << "# of joint(s) = " << numElements << endl;
		for (unsigned indexJoint = 0; indexJoint < numElements; ++indexJoint) {
			MPlug	matrix_plug = matrix_plug_array.elementByPhysicalIndex(indexJoint, &ms);
#if 1
{
			if (!ms) {cerr << ms << endl; break;}
			MObject	valData;
			ms = matrix_plug.getValue(valData);
			if (!ms) {cerr << ms << endl; break;}
			MFnMatrixData		matrixData;
			ms = matrixData.setObject(valData);
			if (!ms) {cerr << ms << endl; break;}
			cout << matrixData.matrix(&ms) << endl;
}
#endif
#if 1
{
			MPlugArray	joint_plug_array;
			bool	bFound = matrix_plug.connectedTo(joint_plug_array, true, false, &ms);
			if (bFound && joint_plug_array.length()) {
				MObject	joint = joint_plug_array[0].node(&ms);
#if 0
MObject	joint_parent_plug = joint_plug_array[0].parent(&ms).node(&ms); cout << ms << endl;
MFnDagNode	joint_parent_dagNode(joint_parent_plug, &ms); cout << ms << endl;
MDagPath	joint_parent_dagPath;
ms = joint_parent_dagNode.getPath(joint_parent_dagPath); cout << ms << endl;
cout << "parent: |" << joint_parent_dagPath.fullPathName(&ms) << "|" << endl; cout << ms << endl;
cout << "parent matrix: |" << joint_parent_dagPath.inclusiveMatrix(&ms) << "|" << endl; cout << ms << endl;
#endif
				MFnDagNode	joint_dagNode(joint, &ms);
#if 0
cout << "# of parent = " << joint_dagNode.parentCount(&ms) << endl; if (!ms) {cerr << ms << endl; break;}
MFnDagNode	joint_parent_dagNode = joint_dagNode.parent(0, &ms); if (!ms) {cerr << ms << endl; break;}
MDagPath	joint_parent_dagPath;
ms = joint_parent_dagNode.getPath(joint_parent_dagPath); if (!ms) {cerr << ms << endl; break;}
cout << "parent: |" << joint_parent_dagPath.fullPathName(&ms) << "|" << endl; if (!ms) {cerr << ms << endl; break;}
cout << "parent matrix: |" << joint_parent_dagPath.inclusiveMatrix(&ms) << "|" << endl; if (!ms) {cerr << ms << endl; break;}
#endif
				MDagPath	joint_dagPath;
				ms = joint_dagNode.getPath(joint_dagPath);
#if 1
#if 1
				MMatrix*	joint_world_matrix_ptr = new MMatrix;
				*joint_world_matrix_ptr = joint_dagPath.inclusiveMatrix(&ms);
				joint_world_matrix_array.append(joint_world_matrix_ptr);
#else
				joint_world_matrix[indexJoint] = joint_dagPath.inclusiveMatrix(&ms);
#endif
#endif
			}
}
#endif
		}
	}
#if 1
#if 1
	assert(__numJoints == joint_world_matrix_array.length());
	cout << "joint's world matrix list for given time:" << endl;
	for (unsigned i = 0; i < __numJoints; ++i) {
		cout << "W[" << i << "]: " << joint_world_matrix_array(i) << endl;
	}
#else
	for (int i = 0; i < 5; ++i) {
		cout << "W[" << i << "]: " << joint_world_matrix[i] << endl;
	}
#endif
#endif
#endif
	MMatrix	m;
#if 1
	if (__numJoints) {
		assert(indexTestVertex < (int) __weights_array.length());
		const MDoubleArray&	weights = __weights_array(indexTestVertex);
		m = weights[0] * (__inverse_binding_matrix_array(0) * joint_world_matrix_array(0));
		for (unsigned i = 1; i < __numJoints; ++i) {
			m += weights[i] * (__inverse_binding_matrix_array(i) * joint_world_matrix_array(i));
		}
	}
	cout << "m(\\sum w_i B^-1_i W_i): " << m << endl;
#else
	m = weights_x[0] * (inverse_binding_matrix[0] * joint_world_matrix[0]);
	for (int i = 1; i < 5; ++i) {
		m += weights_x[i] * (inverse_binding_matrix[i] * joint_world_matrix[i]);
	}
#endif
#if 0
	cout << "V': " << V_prime_kWorld << endl;
	MPoint	V_prime_kObject = V_prime_kWorld * __externalInclusiveMatrix.inverse();
	cout << "V'0: " << V_prime_kObject << endl;
	MPoint	V_kObject = V_prime_kObject * m.inverse();
	cout << "V0: " << V_kObject << endl;
	cout << "MSpace::kObject in geometry: " << V_prime_kWorld * __externalInclusiveMatrix.inverse() * geometry_matrix * m.inverse() * geometry_matrix.inverse() << endl;
#endif
} while (0);
		}
	}
#endif
#endif
}
#endif
#endif
	return MStatus::kSuccess;
}
void
CBrushContext::timerCallback(
	float	,
	float	,
	void*	contextPtr
)
{
#ifdef move__DEBUG2
	cout << "CBrushContext::timerCallback()" << endl;
	cout << "CBrushContext::timerCallback(): Thread ID = " << GetCurrentThreadId() << endl;
	cout << "CBrushContext::timerCallback(): elapsedTime = " << elapsedTime << endl;
	cout << "CBrushContext::timerCallback(): lastTime = " << lastTime << endl;
#endif
	if (
		((CBrushContext*) contextPtr)->getBrushType() != CBrushToolCommand::BT_TENTACLE_CREATE &&
		((CBrushContext*) contextPtr)->getBrushType() != CBrushToolCommand::BT_TENTACLE_MOVE
	)
		((CBrushContext*) contextPtr)->__brushCircle.draw();
	return;
}
CBrushToolCommand::BRUSH_TYPE	CBrushContext::getBrushType() const
{
	return __brushType;
}
MStatus	CBrushContext::setBrushType(CBrushToolCommand::BRUSH_TYPE brushType)
{
	switch (brushType) {
#if 0
	case CBrushToolCommand::BT_CUT:
	case CBrushToolCommand::BT_SCALE:
#endif
	case CBrushToolCommand::BT_TRANSLATE:
	case CBrushToolCommand::BT_ROTATE:
#if 1
#if 0
	case CBrushToolCommand::BT_TENTACLE_TWIST:
	case CBrushToolCommand::BT_TENTACLE_SHAPE:
#endif
#if 1
	case CBrushToolCommand::BT_TENTACLE_CREATE:
	case CBrushToolCommand::BT_TENTACLE_MOVE:
#endif
#endif
		break;
	default:
		cerr << "CBrushContext::setBrushType: Error: |" << brushType << "|: Unknown brush type" << endl;
		return MStatus::kFailure;
	}
	if (brushType == __brushType)
		return MStatus::kSuccess;
	__brushType = brushType;
	CBrushToolCommand::BrushType = brushType;
	MString	titleString("Cephalopod Brush: ");
	switch (brushType) {
#if 0
	case CBrushToolCommand::BT_CUT:
		titleString += "Cut";
		break;
	case CBrushToolCommand::BT_SCALE:
		titleString += "Scale";
		break;
#endif
	case CBrushToolCommand::BT_TRANSLATE:
		titleString += "Translate";
		break;
	case CBrushToolCommand::BT_ROTATE:
		titleString += "Rotate";
		break;
#if 1
#if 0
	case CBrushToolCommand::BT_TENTACLE_TWIST:
		titleString += "Tentacle: Twist";
		break;
	case CBrushToolCommand::BT_TENTACLE_SHAPE:
		titleString += "Tentacle: Shape";
		break;
#endif
#if 1
	case CBrushToolCommand::BT_TENTACLE_CREATE:
		titleString += "Create";
		break;
	case CBrushToolCommand::BT_TENTACLE_MOVE:
		titleString += "Move";
		break;
#endif
#endif
	default:
		titleString += "Unknown";
		break;
	}
	setTitleString(titleString);
#if 1
	return MStatus::kSuccess;
#else
	return buildActiveSelectionList();
#endif
}
#if 0
static double	getCutRatio(double x1, double y1, double x2, double y2, double r)
{
	double	dx = x2 - x1;
	double	dy = y2 - y1;
	double	a = dx * dx + dy * dy;
	if (a < DBL_EPSILON)
		return 0;
	double	b = x1 * dx + y1 * dy;
	double	c = x1 * x1 + y1 * y1 - r * r;
	double	D_4 = b * b - a * c;
	if (D_4 < 0)
		return 0;
	D_4 = sqrt(D_4);
	double	t1 = (-b + D_4) / a;
	double	t2 = (-b - D_4) / a;
	double	t0 = t1 < 0 ? t2 : t1;
	if (t0 < 0)
		t0 = 0;
	if (t0 > 1)
		t0 = 1;
	return t0;
}
MStatus	CBrushContext::setInsideArray_Cut(InsideArray_Cut& insideArray_Cut)
{
	MStatus	ms = MStatus::kSuccess;
	M3dView	view = M3dView::active3dView(&ms);
#if 0
	MSelectionList	outgoingList;
#endif
	for (unsigned i = 0; i < __activeSelectionArray.length(); ++i) {
		SelectionPtr	activeSelectionPtr = __activeSelectionArray[i];
#ifdef move__DEBUG2
		cout << "__activeSelectionArray[" << i << "]" << endl;
		cout << "\t" << "selectBy = " << activeSelectionPtr->selectBy << endl;
#endif
		MDagPath&	dagPath = activeSelectionPtr->dagPath;
#ifdef move__DEBUG2
		cout << "\t" << "MDagPath: \"" << dagPath.fullPathName() << "\"" << endl;
		cout << "\t" << "numPoints = " << activeSelectionPtr->numPoints << endl;
		cout << "\t" << "worldMatrix: " << activeSelectionPtr->worldMatrix << endl;
		cout << "\t" << "guideArray: ";
		cout << "# of guideArray = " << activeSelectionPtr->guideArray.length() << endl;
#endif
		DataArrayPtr_Cut	dataArrayPtr_Cut = NULL;
		for (unsigned k = 0; k < insideArray_Cut.length(); ++k) {
			DataArrayPtr_Cut	p = insideArray_Cut[k];
			if (p->dagPath == dagPath) {
				dataArrayPtr_Cut = p;
				break;
			}
		}
		const MMatrix&	worldMatrix = activeSelectionPtr->worldMatrix;
		for (unsigned k = 0; k < activeSelectionPtr->guideArray.length(); ++k) {
#ifdef move__DEBUG2
			cout << "\t" << "guideArray[" << k << "]" << endl;
			cout << "\t\t" << "uIndex = " << activeGuidePtr->uIndex << endl;
			cout << "\t\t" << "vertexBits = 0x" << std::setw(8) << std::setfill('0') << activeVertexBits << endl;
#endif
			SelectedGuidePtr	activeGuidePtr = activeSelectionPtr->guideArray[k];
#if 0
			int&		uIndex = activeGuidePtr->uIndex;
#endif
			unsigned&	activeVertexBits = activeGuidePtr->vertexBits;
			unsigned	bit = 1 << 0;
			unsigned	insideVertexBits = 0;
#if 1
			double	totalCutLengthRatio = 0;
			bool	isLastVertexInside = false;
			short	last_x__ = -1, last_y__ = -1;
			double	segmentLength0 = 0;
			if (activeGuidePtr->offsets.length() > 1)
				segmentLength0 = activeGuidePtr->offsets[1].length();
#endif
			MPoint	basePoint = activeGuidePtr->offsets[0];
			basePoint *= worldMatrix;
			MPoint	worldPt = basePoint;
			for (int n = 0; n < (int) activeGuidePtr->offsets.length(); ++n, bit <<= 1) {
				if (n > 0) {
					const MVector&	offset = activeGuidePtr->offsets[n];
					double		len = offset.length();
					const MVector	normal = offset.transformAsNormal(worldMatrix);
					worldPt += len * normal;
				}
#if 1
				double	r = this->__brushCircle.getRadius();
				short	x = -1, y = -1;
				bool	bNotClipped = view.worldToView(worldPt, x, y, &ms);
				if (!bNotClipped || !ms) {
#if 1
					if (n && isLastVertexInside)
						totalCutLengthRatio += 1;
					isLastVertexInside = false;
#endif
					continue;
				}
				short	x__ = start_x - x;
				short	y__ = start_y - y;
				if (!(activeVertexBits & bit)) {
#if 1
					if (n && isLastVertexInside) {
						totalCutLengthRatio += getCutRatio(last_x__, last_y__, x__, y__, r);
					}
					isLastVertexInside = false;
#endif
					continue;
				}
				bool	isInside = r * r > (x__ * x__ + y__ * y__);
				if (isInside) {
					insideVertexBits |= bit;
#if 0
					if (activeSelectionPtr->selectBy == 0) {
						ms = outgoingList.add(dagPath.fullPathName() + ".e[" + uIndex + "]");
					} else if (activeSelectionPtr->selectBy == 1) {
						ms = outgoingList.add(dagPath.fullPathName() + ".cv[" + uIndex + "][" + n + "]");
					}
#endif
				}
				if (n && (isLastVertexInside || isInside)) {
					if (isLastVertexInside && isInside)
						totalCutLengthRatio += 1;
					else {
						if (isLastVertexInside)
							totalCutLengthRatio += getCutRatio(last_x__, last_y__, x__, y__, r);
						else
							totalCutLengthRatio += getCutRatio(x__, y__, last_x__, last_y__, r);
					}
				}
				isLastVertexInside = isInside;
				last_x__ = x__; last_y__ = y__;
#endif
			}
			if (insideVertexBits) {
				Data_CutPtr	cutPtr = new Data_Cut;
				cutPtr->uIndex = activeGuidePtr->uIndex;
#if 1
				cutPtr->segmentLength__ = segmentLength0;
				cutPtr->segmentLength = segmentLength0 * (1 - totalCutLengthRatio / (activeGuidePtr->offsets.length() - 1));
#endif
				if (!dataArrayPtr_Cut) {
					dataArrayPtr_Cut = new DataArray_Cut();
					insideArray_Cut.append(dataArrayPtr_Cut);
					dataArrayPtr_Cut->dagPath = activeSelectionPtr->dagPath;
					dataArrayPtr_Cut->selectBy = activeSelectionPtr->selectBy;
					dataArrayPtr_Cut->numPoints = activeSelectionPtr->numPoints;
					dataArrayPtr_Cut->worldMatrix = activeSelectionPtr->worldMatrix;
				}
				dataArrayPtr_Cut->dataArray.append(cutPtr);
			}
		}
	}
#if 0
	MGlobal::setActiveSelectionList(outgoingList, MGlobal::kReplaceList);
#endif
	return ms;
}
MStatus	CBrushContext::setInsideArray_Scale(InsideArray_Scale& insideArray_Scale)
{
	MStatus	ms = MStatus::kSuccess;
	M3dView	view = M3dView::active3dView(&ms);
#if 0
	MSelectionList	outgoingList;
#endif
	for (unsigned i = 0; i < __activeSelectionArray.length(); ++i) {
		SelectionPtr	activeSelectionPtr = __activeSelectionArray[i];
#ifdef move__DEBUG2
		cout << "__activeSelectionArray[" << i << "]" << endl;
		cout << "\t" << "selectBy = " << activeSelectionPtr->selectBy << endl;
#endif
		MDagPath&	dagPath = activeSelectionPtr->dagPath;
#ifdef move__DEBUG2
		cout << "\t" << "MDagPath: \"" << dagPath.fullPathName() << "\"" << endl;
		cout << "\t" << "numPoints = " << activeSelectionPtr->numPoints << endl;
		cout << "\t" << "worldMatrix: " << activeSelectionPtr->worldMatrix << endl;
		cout << "\t" << "guideArray: ";
		cout << "# of guideArray = " << activeSelectionPtr->guideArray.length() << endl;
#endif
		DataArrayPtr_Scale	scaleArrayPtr = NULL;
		for (unsigned k = 0; k < insideArray_Scale.length(); ++k) {
			DataArrayPtr_Scale	p = insideArray_Scale[k];
			if (p->dagPath == dagPath) {
				scaleArrayPtr = p;
				break;
			}
		}
		const MMatrix&	worldMatrix = activeSelectionPtr->worldMatrix;
		for (unsigned k = 0; k < activeSelectionPtr->guideArray.length(); ++k) {
#ifdef move__DEBUG2
			cout << "\t" << "guideArray[" << k << "]" << endl;
			cout << "\t\t" << "uIndex = " << activeGuidePtr->uIndex << endl;
			cout << "\t\t" << "vertexBits = 0x" << std::setw(8) << std::setfill('0') << activeVertexBits << endl;
#endif
			SelectedGuidePtr	activeGuidePtr = activeSelectionPtr->guideArray[k];
#if 0
			int&		uIndex = activeGuidePtr->uIndex;
#endif
			unsigned&	activeVertexBits = activeGuidePtr->vertexBits;
			unsigned	bit = 1 << 0;
			unsigned	insideVertexBits = 0;
#if 1
			double	segmentLength0 = 0;
			if (activeGuidePtr->offsets.length() > 1)
				segmentLength0 = activeGuidePtr->offsets[1].length();
#endif
			MPoint	basePoint = activeGuidePtr->offsets[0];
			basePoint *= worldMatrix;
			MPoint	worldPt = basePoint;
			for (int n = 0; n < (int) activeGuidePtr->offsets.length(); ++n, bit <<= 1) {
				if (n > 0) {
					const MVector&	offset = activeGuidePtr->offsets[n];
					double		len = offset.length();
					const MVector	normal = offset.transformAsNormal(worldMatrix);
					worldPt += len * normal;
				}
				if (!(activeVertexBits & bit))
					continue;
				short	x = -1, y = -1;
				bool	bNotClipped = view.worldToView(worldPt, x, y, &ms);
				if (!bNotClipped || !ms)
					continue;
				short	x__ = start_x - x;
				short	y__ = start_y - y;
				double	r = this->__brushCircle.getRadius();
				bool	isInside = r * r > (x__ * x__ + y__ * y__);
				if (isInside) {
					insideVertexBits |= bit;
#if 0
					if (activeSelectionPtr->selectBy == 0) {
						ms = outgoingList.add(dagPath.fullPathName() + ".e[" + uIndex + "]");
					} else if (activeSelectionPtr->selectBy == 1) {
						ms = outgoingList.add(dagPath.fullPathName() + ".cv[" + uIndex + "][" + n + "]");
					}
#endif
				}
				if (insideVertexBits)
					break;
			}
			if (insideVertexBits) {
				Data_ScalePtr	scalePtr = new Data_Scale;
				scalePtr->uIndex = activeGuidePtr->uIndex;
#if 1
				scalePtr->segmentLength__ = segmentLength0;
				scalePtr->segmentLength = segmentLength0;
#endif
				if (!scaleArrayPtr) {
					scaleArrayPtr = new DataArray_Scale;
					insideArray_Scale.append(scaleArrayPtr);
					scaleArrayPtr->dagPath = activeSelectionPtr->dagPath;
					scaleArrayPtr->selectBy = activeSelectionPtr->selectBy;
					scaleArrayPtr->numPoints = activeSelectionPtr->numPoints;
					scaleArrayPtr->worldMatrix = activeSelectionPtr->worldMatrix;
				}
				scaleArrayPtr->dataArray.append(scalePtr);
			}
		}
	}
#if 0
	MGlobal::setActiveSelectionList(outgoingList, MGlobal::kReplaceList);
#endif
	return ms;
}
#endif
MStatus	CBrushContext::setInsideArray_Translate(InsideArray_Translate& insideArray_Translate)
{
	MStatus	ms = MStatus::kSuccess;
	M3dView	view = M3dView::active3dView(&ms);
#if 0
	MSelectionList	outgoingList;
#endif
	double	r = this->__brushCircle.getRadius();
	double	r2 = r * r;
	for (unsigned i = 0; i < __activeSelectionArray.length(); ++i) {
		SelectionPtr	activeSelectionPtr = __activeSelectionArray[i];
#ifdef move__DEBUG2
		cout << "__activeSelectionArray[" << i << "]" << endl;
		cout << "\t" << "selectBy = " << activeSelectionPtr->selectBy << endl;
#endif
		MDagPath&	dagPath = activeSelectionPtr->dagPath;
#ifdef move__DEBUG2
		cout << "\t" << "MDagPath: \"" << dagPath.fullPathName() << "\"" << endl;
		cout << "\t" << "numPoints = " << activeSelectionPtr->numPoints << endl;
		cout << "\t" << "worldMatrix: " << activeSelectionPtr->worldMatrix << endl;
		cout << "\t" << "guideArray: ";
		cout << "# of guideArray = " << activeSelectionPtr->guideArray.length() << endl;
#endif
		DataArrayPtr_Translate	translateArrayPtr = NULL;
		for (unsigned k = 0; k < insideArray_Translate.length(); ++k) {
			DataArrayPtr_Translate	p = insideArray_Translate[k];
			if (p->dagPath == dagPath) {
				translateArrayPtr = p;
				break;
			}
		}
		const MMatrix&	worldMatrix = activeSelectionPtr->worldMatrix;
		for (unsigned k = 0; k < activeSelectionPtr->guideArray.length(); ++k) {
#ifdef move__DEBUG2
			cout << "\t" << "guideArray[" << k << "]" << endl;
			cout << "\t\t" << "uIndex = " << activeGuidePtr->uIndex << endl;
			cout << "\t\t" << "vertexBits = 0x" << std::setw(8) << std::setfill('0') << activeVertexBits << endl;
#endif
			SelectedGuidePtr	activeGuidePtr = activeSelectionPtr->guideArray[k];
			unsigned&	activeVertexBits = activeGuidePtr->vertexBits;
			MDoubleArray	factorArray(activeGuidePtr->offsets.length(), 0);
			unsigned	bit = 1 << 0;
			unsigned	insideVertexBits = 0;
			MPoint	basePoint = activeGuidePtr->offsets[0];
			basePoint *= worldMatrix;
			MPoint	worldPt = basePoint;
			for (int n = 0; n < (int) activeGuidePtr->offsets.length(); ++n, bit <<= 1) {
				double&	factor = factorArray[n];
#if 1
			if (activeSelectionPtr->hasSkinCluster) {
				worldPt = activeGuidePtr->offsets[n];
			} else {
#endif
				if (n > 0) {
					const MVector&	offset = activeGuidePtr->offsets[n];
					double		len = offset.length();
					const MVector	normal = offset.transformAsNormal(worldMatrix);
					worldPt += len * normal;
				}
#if 1
			}
#endif
				if (!(activeVertexBits & bit))
					continue;
				short	x = -1, y = -1;
				bool	bNotClipped = view.worldToView(worldPt, x, y, &ms);
				if (!bNotClipped || !ms)
					continue;
				short	x__ = start_x - x;
				short	y__ = start_y - y;
				bool	isInside = r2 > (x__ * x__ + y__ * y__);
				if (isInside) {
					insideVertexBits |= bit;
#if 0
					if (activeSelectionPtr->selectBy == 0) {
						ms = outgoingList.add(dagPath.fullPathName() + ".e[" + uIndex + "]");
					} else if (activeSelectionPtr->selectBy == 1) {
						ms = outgoingList.add(dagPath.fullPathName() + ".cv[" + uIndex + "][" + n + "]");
					}
#endif
					factor = 1 - sqrtf((float) x__ * x__ + y__ * y__) / r;
				}
			}
			if (insideVertexBits) {
				Data_TranslatePtr	translatePtr = new Data_Translate;
				translatePtr->uIndex = activeGuidePtr->uIndex;
#if 1
				translatePtr->vertexBits = insideVertexBits;
				translatePtr->factorArray = factorArray;
				translatePtr->offsets__ = activeGuidePtr->offsets;
				translatePtr->offsets = activeGuidePtr->offsets;
#if 1
#if 1
				translatePtr->root_wNormal = activeGuidePtr->root_wNormal;
#endif
				translatePtr->transversals__ = activeGuidePtr->transversals;
				translatePtr->transversals = activeGuidePtr->transversals;
#endif
#endif
				if (!translateArrayPtr) {
					translateArrayPtr = new DataArray_Translate;
					insideArray_Translate.append(translateArrayPtr);
					translateArrayPtr->dagPath = activeSelectionPtr->dagPath;
#if 1
					translateArrayPtr->hasSkinCluster = activeSelectionPtr->hasSkinCluster;
#endif
					translateArrayPtr->selectBy = activeSelectionPtr->selectBy;
					translateArrayPtr->numPoints = activeSelectionPtr->numPoints;
					translateArrayPtr->worldMatrix = activeSelectionPtr->worldMatrix;
				}
				translateArrayPtr->dataArray.append(translatePtr);
			}
		}
	}
#if 0
	MGlobal::setActiveSelectionList(outgoingList, MGlobal::kReplaceList);
#endif
	return ms;
}
MStatus	CBrushContext::setInsideArray_Rotate(InsideArray_Rotate& insideArray_Rotate)
{
	MStatus	ms = MStatus::kSuccess;
	M3dView	view = M3dView::active3dView(&ms);
#if 0
	MSelectionList	outgoingList;
#endif
	double	r = this->__brushCircle.getRadius();
	double	r2 = r * r;
	for (unsigned i = 0; i < __activeSelectionArray.length(); ++i) {
		SelectionPtr	activeSelectionPtr = __activeSelectionArray[i];
#ifdef move__DEBUG2
		cout << "__activeSelectionArray[" << i << "]" << endl;
		cout << "\t" << "selectBy = " << activeSelectionPtr->selectBy << endl;
#endif
		MDagPath&	dagPath = activeSelectionPtr->dagPath;
#ifdef move__DEBUG2
		cout << "\t" << "MDagPath: \"" << dagPath.fullPathName() << "\"" << endl;
		cout << "\t" << "numPoints = " << activeSelectionPtr->numPoints << endl;
		cout << "\t" << "worldMatrix: " << activeSelectionPtr->worldMatrix << endl;
		cout << "\t" << "guideArray: ";
		cout << "# of guideArray = " << activeSelectionPtr->guideArray.length() << endl;
#endif
		DataArrayPtr_Rotate	rotateArrayPtr = NULL;
		for (unsigned k = 0; k < insideArray_Rotate.length(); ++k) {
			DataArrayPtr_Rotate	p = insideArray_Rotate[k];
			if (p->dagPath == dagPath) {
				rotateArrayPtr = p;
				break;
			}
		}
		const MMatrix&	worldMatrix = activeSelectionPtr->worldMatrix;
		for (unsigned k = 0; k < activeSelectionPtr->guideArray.length(); ++k) {
#ifdef move__DEBUG2
			cout << "\t" << "guideArray[" << k << "]" << endl;
			cout << "\t\t" << "uIndex = " << activeGuidePtr->uIndex << endl;
			cout << "\t\t" << "vertexBits = 0x" << std::setw(8) << std::setfill('0') << activeVertexBits << endl;
#endif
			SelectedGuidePtr	activeGuidePtr = activeSelectionPtr->guideArray[k];
			unsigned&	activeVertexBits = activeGuidePtr->vertexBits;
			MDoubleArray	factorArray(activeGuidePtr->offsets.length(), 0);
			unsigned	bit = 1 << 0;
			unsigned	insideVertexBits = 0;
			MPoint	basePoint = activeGuidePtr->offsets[0];
			basePoint *= worldMatrix;
			MPoint	worldPt = basePoint;
			for (int n = 0; n < (int) activeGuidePtr->offsets.length(); ++n, bit <<= 1) {
				double&	factor = factorArray[n];
#if 1
			if (activeSelectionPtr->hasSkinCluster) {
				worldPt = activeGuidePtr->offsets[n];
			} else {
#endif
				if (n > 0) {
					const MVector&	offset = activeGuidePtr->offsets[n];
					double		len = offset.length();
					const MVector	normal = offset.transformAsNormal(worldMatrix);
					worldPt += len * normal;
				}
#if 1
			}
#endif
				if (!(activeVertexBits & bit))
					continue;
				short	x = -1, y = -1;
				bool	bNotClipped = view.worldToView(worldPt, x, y, &ms);
				if (!bNotClipped || !ms)
					continue;
				short	x__ = start_x - x;
				short	y__ = start_y - y;
				bool	isInside = r2 > (x__ * x__ + y__ * y__);
				if (isInside) {
					insideVertexBits |= bit;
#if 0
					if (activeSelectionPtr->selectBy == 0) {
						ms = outgoingList.add(dagPath.fullPathName() + ".e[" + uIndex + "]");
					} else if (activeSelectionPtr->selectBy == 1) {
						ms = outgoingList.add(dagPath.fullPathName() + ".cv[" + uIndex + "][" + n + "]");
					}
#endif
					factor = 1 - sqrtf((float) x__ * x__ + y__ * y__) / r;
				}
			}
			if (insideVertexBits) {
				Data_RotatePtr	rotatePtr = new Data_Rotate;
				rotatePtr->uIndex = activeGuidePtr->uIndex;
#if 1
				rotatePtr->vertexBits = insideVertexBits;
				rotatePtr->factorArray = factorArray;
				rotatePtr->offsets__ = activeGuidePtr->offsets;
				rotatePtr->offsets = activeGuidePtr->offsets;
#if 1
#if 1
				rotatePtr->root_wNormal = activeGuidePtr->root_wNormal;
#endif
				rotatePtr->transversals__ = activeGuidePtr->transversals;
				rotatePtr->transversals = activeGuidePtr->transversals;
#endif
#endif
				if (!rotateArrayPtr) {
					rotateArrayPtr = new DataArray_Rotate;
					insideArray_Rotate.append(rotateArrayPtr);
					rotateArrayPtr->dagPath = activeSelectionPtr->dagPath;
#if 1
					rotateArrayPtr->hasSkinCluster = activeSelectionPtr->hasSkinCluster;
#endif
					rotateArrayPtr->selectBy = activeSelectionPtr->selectBy;
					rotateArrayPtr->numPoints = activeSelectionPtr->numPoints;
					rotateArrayPtr->worldMatrix = activeSelectionPtr->worldMatrix;
				}
				rotateArrayPtr->dataArray.append(rotatePtr);
			}
		}
	}
#if 0
	MGlobal::setActiveSelectionList(outgoingList, MGlobal::kReplaceList);
#endif
	return ms;
}
MStatus	CBrushContext::attenuateLength(bool bDir)
{
	MStatus	ms;
	MGlobal::MSelectionMode	mode = MGlobal::selectionMode(&ms); assert(ms);
	if (mode != MGlobal::kSelectComponentMode) {
#ifdef _DEBUG
		cerr << "WARNING: CBrushContext::attenuateLength(): Not a MGlobal::kSelectComponentMode: ";
		cerr << "MSelectionMode:= " << (mode == MGlobal::kSelectObjectMode ? "MGlobal::kSelectObjectMode" :
			(mode == MGlobal::kSelectComponentMode ? "MGlobal::kSelectComponentMode" :
			(mode == MGlobal::kSelectRootMode ? "MGlobal::kSelectRootMode" :
			(mode == MGlobal::kSelectLeafMode ? "MGlobal::kSelectLeafMode" :
			(mode == MGlobal::kSelectTemplateMode ? "MGlobal::kSelectTemplateMode" : "kSelectUnknownMode"
			))))) << endl;
#endif
		return MStatus::kFailure;
	}
	MSelectionMask	mask = MGlobal::componentSelectionMask(&ms);
	if (!mask.intersects(MSelectionMask::kSelectComponentsMask)) {
#ifdef _DEBUG
		cerr << "WARNING: CBrushContext::attenuateLength(): Not Set MSelectionMask::kSelectComponentMask Bit" << endl;
#endif
		return MStatus::kFailure;
	}
	MSelectionList	incomingList;
	MGlobal::getActiveSelectionList(incomingList);
	for (unsigned iList = 0; iList < incomingList.length(); ++iList) {
		MDagPath	dagPath;
		MObject		component;
		ms = incomingList.getDagPath(iList, dagPath, component);
		MFnDependencyNode	fnDepNode(dagPath.node());
#if 1
#else
		MDoubleArray	attenuateLengthFactorArray;
{
		MStatus	ms;
		MDagPath	dagPathShape;
		ms = findExternalDagPath(dagPath.node(), dagPathShape);
		if (ms != MStatus::kSuccess && ms != MStatus::kNotFound) {
			cout << "incomingList[" << iList << "]: |" << dagPath.fullPathName() << "| (" << component.apiTypeStr() << ")" << endl;
			cout << "Error: Cannot set area: Ignored" << endl;
			continue;
		}
		MObject	objMesh = dagPathShape.node(&ms);
		MFnMesh	fnMesh(objMesh, &ms);
		if (!ms) {
			cout << "incomingList[" << iList << "]: |" << dagPath.fullPathName() << "| (" << component.apiTypeStr() << ")" << endl;
			cout << "Error: Cannot set MFnMesh: Ignored" << endl;
			continue;
		}
		MItMeshPolygon	polyIter(objMesh, &ms);
		double	avg_area = 0;
		MDoubleArray	areaArray;
		areaArray.setLength(fnMesh.numPolygons());
		for (; !polyIter.isDone(); polyIter.next()) {
			double	area;
			polyIter.getArea(area);
			avg_area += area;
			areaArray[polyIter.index()] = area;
		}
		avg_area /= fnMesh.numPolygons();
		MItMeshVertex	vertexIter(objMesh, &ms);
		attenuateLengthFactorArray.setLength(vertexIter.count());
		for (; !vertexIter.isDone(); vertexIter.next()) {
			MIntArray	faceArray;
			ms = vertexIter.getConnectedFaces(faceArray);
			double	avg_area_local = 0;
			for (int i = 0; i < (int) faceArray.length(); ++i)
				avg_area_local += areaArray[faceArray[i]];
			avg_area_local /= faceArray.length();
			attenuateLengthFactorArray[vertexIter.index()] = sqrt(avg_area_local);
		}
}
#endif
		MIntArray	indexArray;
{
		if (
			component.hasFn(MFn::kDoubleIndexedComponent) &&
			component.hasFn(MFn::kSurfaceCVComponent)
		) {
			MStatus	ms;
			MFnDoubleIndexedComponent	fnCVComp(component, &ms);
			int	len = fnCVComp.elementCount(&ms);
			for (int iElem = 0; iElem < len; ++iElem) {
				int	uIndex;
				int	vIndex;
				ms = fnCVComp.getElement(iElem, uIndex, vIndex);
				bool	found = false;
				for (int i = 0; i < (int) indexArray.length(); ++i) {
					if (indexArray[i] == uIndex) {
						found = true;
						break;
					}
				}
				if (!found)
					indexArray.append(uIndex);
			}
		} else if (
			component.hasFn(MFn::kSingleIndexedComponent) &&
			component.hasFn(MFn::kMeshEdgeComponent)
		) {
			MStatus	ms;
			MFnSingleIndexedComponent	fnEdgeComp(component, &ms);
			int	len = fnEdgeComp.elementCount(&ms);
			for (int iElem = 0; iElem < len; ++iElem) {
				int	index = fnEdgeComp.element(iElem, &ms);
				bool	found = false;
				for (int i = 0; i < (int) indexArray.length(); ++i) {
					if (indexArray[i] == index) {
						found = true;
						break;
					}
				}
				if (!found)
					indexArray.append(index);
			}
		}
}
#if 1
		MPlug	plug_attenuateLength = fnDepNode.findPlug("attenuateLength", true, &ms);
		for (int n = 0; n < (int) indexArray.length(); ++n) {
			MPlug	plug_child = plug_attenuateLength.elementByLogicalIndex(indexArray[n]);
			ms = plug_child.setValue(bDir);
		}
#else
		MPlug	plug_numPointsInGuide = fnDepNode.findPlug("numPointsInGuide", false);
		int	numPoints = -1;
		plug_numPointsInGuide.getValue(numPoints);
		MPlug	plug_cp_array = fnDepNode.findPlug("controlPoints", true, &ms);
		for (int n = 0; n < (int) indexArray.length(); ++n) {
			int	index = indexArray[n];
			for (int k = 1; k < numPoints; ++k) {
				MPlug	plug_compound = plug_cp_array.elementByLogicalIndex(index * numPoints + k);
				MObject	offset_data;
				ms = plug_compound.getValue(offset_data);
				MFnNumericData	offset_val(offset_data, &ms);
				double	xVal, yVal, zVal;
				ms = offset_val.getData(xVal, yVal, zVal);
				MVector	offset(xVal, yVal, zVal);
				offset = attenuateLengthFactorArray[index] * offset;
				ms = offset_val.setData(offset.x, offset.y, offset.z);
				ms = plug_compound.setValue(offset_data);
			}
		}
#endif
	}
	return MStatus::kSuccess;
}
#if 1
#if 0
MStatus	CBrushContext::setInsideArray_Tentacle_Twist(InsideArray_Tentacle_Twist& insideArray_Tentacle_Twist)
{
	MStatus	ms = MStatus::kSuccess;
	M3dView	view = M3dView::active3dView(&ms);
#if 0
	MSelectionList	outgoingList;
#endif
	double	r = this->__brushCircle.getRadius();
	double	r2 = r * r;
	for (unsigned i = 0; i < __activeSelectionArray.length(); ++i) {
		SelectionPtr	activeSelectionPtr = __activeSelectionArray[i];
#ifdef move__DEBUG2
		cout << "__activeSelectionArray[" << i << "]" << endl;
		cout << "\t" << "selectBy = " << activeSelectionPtr->selectBy << endl;
#endif
		MDagPath&	dagPath = activeSelectionPtr->dagPath;
#ifdef move__DEBUG2
		cout << "\t" << "MDagPath: \"" << dagPath.fullPathName() << "\"" << endl;
		cout << "\t" << "numPoints = " << activeSelectionPtr->numPoints << endl;
		cout << "\t" << "worldMatrix: " << activeSelectionPtr->worldMatrix << endl;
		cout << "\t" << "guideArray: ";
		cout << "# of guideArray = " << activeSelectionPtr->guideArray.length() << endl;
#endif
		DataArrayPtr_Tentacle_Twist	twistArrayPtr = NULL;
		for (unsigned k = 0; k < insideArray_Tentacle_Twist.length(); ++k) {
			DataArrayPtr_Tentacle_Twist	p = insideArray_Tentacle_Twist[k];
			if (p->dagPath == dagPath) {
				twistArrayPtr = p;
				break;
			}
		}
		const MMatrix&	worldMatrix = activeSelectionPtr->worldMatrix;
		for (unsigned k = 0; k < activeSelectionPtr->guideArray.length(); ++k) {
#ifdef move__DEBUG2
			cout << "\t" << "guideArray[" << k << "]" << endl;
			cout << "\t\t" << "uIndex = " << activeGuidePtr->uIndex << endl;
			cout << "\t\t" << "vertexBits = 0x" << std::setw(8) << std::setfill('0') << activeVertexBits << endl;
#endif
			SelectedGuidePtr	activeGuidePtr = activeSelectionPtr->guideArray[k];
			unsigned&	activeVertexBits = activeGuidePtr->vertexBits;
			MDoubleArray	factorArray(activeGuidePtr->offsets.length(), 0);
			unsigned	bit = 1 << 0;
			unsigned	insideVertexBits = 0;
			MPoint	basePoint = activeGuidePtr->offsets[0];
			basePoint *= worldMatrix;
			MPoint	worldPt = basePoint;
			for (int n = 0; n < (int) activeGuidePtr->offsets.length(); ++n, bit <<= 1) {
				double&	factor = factorArray[n];
				if (n > 0) {
					const MVector&	offset = activeGuidePtr->offsets[n];
					double		len = offset.length();
					const MVector	normal = offset.transformAsNormal(worldMatrix);
					worldPt += len * normal;
				}
				if (!(activeVertexBits & bit))
					continue;
				short	x = -1, y = -1;
				bool	bNotClipped = view.worldToView(worldPt, x, y, &ms);
				if (!bNotClipped || !ms)
					continue;
				short	x__ = start_x - x;
				short	y__ = start_y - y;
				bool	isInside = r2 > (x__ * x__ + y__ * y__);
				if (isInside) {
					insideVertexBits |= bit;
#if 0
					if (activeSelectionPtr->selectBy == 0) {
						ms = outgoingList.add(dagPath.fullPathName() + ".e[" + uIndex + "]");
					} else if (activeSelectionPtr->selectBy == 1) {
						ms = outgoingList.add(dagPath.fullPathName() + ".cv[" + uIndex + "][" + n + "]");
					}
#endif
					factor = 1 - sqrtf((float) x__ * x__ + y__ * y__) / r;
				}
			}
			if (insideVertexBits) {
				Data_Tentacle_TwistPtr	twistPtr = new Data_Tentacle_Twist;
				twistPtr->uIndex = activeGuidePtr->uIndex;
#if 1
				twistPtr->vertexBits = insideVertexBits;
				twistPtr->factorArray = factorArray;
				twistPtr->longitudinals = activeGuidePtr->offsets;
				twistPtr->transversals__ = activeGuidePtr->transversals;
				twistPtr->transversals = activeGuidePtr->transversals;
#endif
				if (!twistArrayPtr) {
					twistArrayPtr = new DataArray_Tentacle_Twist;
					insideArray_Tentacle_Twist.append(twistArrayPtr);
					twistArrayPtr->dagPath = activeSelectionPtr->dagPath;
					twistArrayPtr->selectBy = activeSelectionPtr->selectBy;
					twistArrayPtr->numPoints = activeSelectionPtr->numPoints;
					twistArrayPtr->worldMatrix = activeSelectionPtr->worldMatrix;
				}
				twistArrayPtr->dataArray.append(twistPtr);
			}
		}
	}
#if 0
	MGlobal::setActiveSelectionList(outgoingList, MGlobal::kReplaceList);
#endif
	return ms;
}
MStatus	CBrushContext::setInsideArray_Tentacle_Shape(InsideArray_Tentacle_Shape& insideArray_Tentacle_Shape)
{
	MStatus	ms = MStatus::kSuccess;
	M3dView	view = M3dView::active3dView(&ms);
#if 0
	MSelectionList	outgoingList;
#endif
	double	r = this->__brushCircle.getRadius();
	double	r2 = r * r;
	for (unsigned i = 0; i < __activeSelectionArray.length(); ++i) {
		SelectionPtr	activeSelectionPtr = __activeSelectionArray[i];
#ifdef move__DEBUG2
		cout << "__activeSelectionArray[" << i << "]" << endl;
		cout << "\t" << "selectBy = " << activeSelectionPtr->selectBy << endl;
#endif
		MDagPath&	dagPath = activeSelectionPtr->dagPath;
#ifdef move__DEBUG2
		cout << "\t" << "MDagPath: \"" << dagPath.fullPathName() << "\"" << endl;
		cout << "\t" << "numPoints = " << activeSelectionPtr->numPoints << endl;
		cout << "\t" << "worldMatrix: " << activeSelectionPtr->worldMatrix << endl;
		cout << "\t" << "guideArray: ";
		cout << "# of guideArray = " << activeSelectionPtr->guideArray.length() << endl;
#endif
		DataArrayPtr_Tentacle_Shape	shapeArrayPtr = NULL;
		for (unsigned k = 0; k < insideArray_Tentacle_Shape.length(); ++k) {
			DataArrayPtr_Tentacle_Shape	p = insideArray_Tentacle_Shape[k];
			if (p->dagPath == dagPath) {
				shapeArrayPtr = p;
				break;
			}
		}
		const MMatrix&	worldMatrix = activeSelectionPtr->worldMatrix;
		for (unsigned k = 0; k < activeSelectionPtr->guideArray.length(); ++k) {
#ifdef move__DEBUG2
			cout << "\t" << "guideArray[" << k << "]" << endl;
			cout << "\t\t" << "uIndex = " << activeGuidePtr->uIndex << endl;
			cout << "\t\t" << "vertexBits = 0x" << std::setw(8) << std::setfill('0') << activeVertexBits << endl;
#endif
			SelectedGuidePtr	activeGuidePtr = activeSelectionPtr->guideArray[k];
			unsigned&	activeVertexBits = activeGuidePtr->vertexBits;
			MDoubleArray	factorArray(activeGuidePtr->offsets.length(), 0);
			unsigned	bit = 1 << 0;
			unsigned	insideVertexBits = 0;
			MPoint	basePoint = activeGuidePtr->offsets[0];
			basePoint *= worldMatrix;
			MPoint	worldPt = basePoint;
			for (int n = 0; n < (int) activeGuidePtr->offsets.length(); ++n, bit <<= 1) {
				double&	factor = factorArray[n];
				if (n > 0) {
					const MVector&	offset = activeGuidePtr->offsets[n];
					double		len = offset.length();
					const MVector	normal = offset.transformAsNormal(worldMatrix);
					worldPt += len * normal;
				}
				if (!(activeVertexBits & bit))
					continue;
				short	x = -1, y = -1;
				bool	bNotClipped = view.worldToView(worldPt, x, y, &ms);
				if (!bNotClipped || !ms)
					continue;
				short	x__ = start_x - x;
				short	y__ = start_y - y;
				bool	isInside = r2 > (x__ * x__ + y__ * y__);
				if (isInside) {
					insideVertexBits |= bit;
#if 0
					if (activeSelectionPtr->selectBy == 0) {
						ms = outgoingList.add(dagPath.fullPathName() + ".e[" + uIndex + "]");
					} else if (activeSelectionPtr->selectBy == 1) {
						ms = outgoingList.add(dagPath.fullPathName() + ".cv[" + uIndex + "][" + n + "]");
					}
#endif
					factor = 1 - sqrtf((float) x__ * x__ + y__ * y__) / r;
				}
			}
			if (insideVertexBits) {
				Data_Tentacle_ShapePtr	shapePtr = new Data_Tentacle_Shape;
				shapePtr->uIndex = activeGuidePtr->uIndex;
#if 1
				shapePtr->vertexBits = insideVertexBits;
				shapePtr->factorArray = factorArray;
				shapePtr->transversals__ = activeGuidePtr->transversals;
				shapePtr->transversals = activeGuidePtr->transversals;
#endif
				if (!shapeArrayPtr) {
					shapeArrayPtr = new DataArray_Tentacle_Shape;
					insideArray_Tentacle_Shape.append(shapeArrayPtr);
					shapeArrayPtr->dagPath = activeSelectionPtr->dagPath;
					shapeArrayPtr->selectBy = activeSelectionPtr->selectBy;
					shapeArrayPtr->numPoints = activeSelectionPtr->numPoints;
					shapeArrayPtr->worldMatrix = activeSelectionPtr->worldMatrix;
				}
				shapeArrayPtr->dataArray.append(shapePtr);
			}
		}
	}
#if 0
	MGlobal::setActiveSelectionList(outgoingList, MGlobal::kReplaceList);
#endif
	return ms;
}
#endif
#if 0
MStatus	CBrushContext::setInsideArray_Tentacle_Create(InsideArray_Tentacle_Create& insideArray_Tentacle_Create)
{
	MStatus	ms = MStatus::kSuccess;
	M3dView	view = M3dView::active3dView(&ms);
#if 0
	MSelectionList	outgoingList;
#endif
	double	r = this->__brushCircle.getRadius();
	double	r2 = r * r;
	for (unsigned i = 0; i < __activeSelectionArray.length(); ++i) {
		SelectionPtr	activeSelectionPtr = __activeSelectionArray[i];
#ifdef move__DEBUG2
		cout << "__activeSelectionArray[" << i << "]" << endl;
		cout << "\t" << "selectBy = " << activeSelectionPtr->selectBy << endl;
#endif
		MDagPath&	dagPath = activeSelectionPtr->dagPath;
#ifdef move__DEBUG2
		cout << "\t" << "MDagPath: \"" << dagPath.fullPathName() << "\"" << endl;
		cout << "\t" << "numPoints = " << activeSelectionPtr->numPoints << endl;
		cout << "\t" << "worldMatrix: " << activeSelectionPtr->worldMatrix << endl;
		cout << "\t" << "guideArray: ";
		cout << "# of guideArray = " << activeSelectionPtr->guideArray.length() << endl;
#endif
		DataArrayPtr_Tentacle_Create	createArrayPtr = NULL;
		for (unsigned k = 0; k < insideArray_Tentacle_Create.length(); ++k) {
			DataArrayPtr_Tentacle_Create	p = insideArray_Tentacle_Create[k];
			if (p->dagPath == dagPath) {
				createArrayPtr = p;
				break;
			}
		}
		const MMatrix&	worldMatrix = activeSelectionPtr->worldMatrix;
		for (unsigned k = 0; k < activeSelectionPtr->guideArray.length(); ++k) {
#ifdef move__DEBUG2
			cout << "\t" << "guideArray[" << k << "]" << endl;
			cout << "\t\t" << "uIndex = " << activeGuidePtr->uIndex << endl;
			cout << "\t\t" << "vertexBits = 0x" << std::setw(8) << std::setfill('0') << activeVertexBits << endl;
#endif
			SelectedGuidePtr	activeGuidePtr = activeSelectionPtr->guideArray[k];
			unsigned&	activeVertexBits = activeGuidePtr->vertexBits;
			MDoubleArray	factorArray(activeGuidePtr->offsets.length(), 0);
			unsigned	bit = 1 << 0;
			unsigned	insideVertexBits = 0;
			MPoint	basePoint = activeGuidePtr->offsets[0];
			basePoint *= worldMatrix;
			MPoint	worldPt = basePoint;
			for (int n = 0; n < (int) activeGuidePtr->offsets.length(); ++n, bit <<= 1) {
				double&	factor = factorArray[n];
				if (n > 0) {
					const MVector&	offset = activeGuidePtr->offsets[n];
					double		len = offset.length();
					const MVector	normal = offset.transformAsNormal(worldMatrix);
					worldPt += len * normal;
				}
				if (!(activeVertexBits & bit))
					continue;
				short	x = -1, y = -1;
				bool	bNotClipped = view.worldToView(worldPt, x, y, &ms);
				if (!bNotClipped || !ms)
					continue;
				short	x__ = start_x - x;
				short	y__ = start_y - y;
				bool	isInside = r2 > (x__ * x__ + y__ * y__);
				if (isInside) {
					insideVertexBits |= bit;
#if 0
					if (activeSelectionPtr->selectBy == 0) {
						ms = outgoingList.add(dagPath.fullPathName() + ".e[" + uIndex + "]");
					} else if (activeSelectionPtr->selectBy == 1) {
						ms = outgoingList.add(dagPath.fullPathName() + ".cv[" + uIndex + "][" + n + "]");
					}
#endif
					factor = 1 - sqrtf((float) x__ * x__ + y__ * y__) / r;
				}
			}
			if (insideVertexBits) {
				Data_Tentacle_CreatePtr	createPtr = new Data_Tentacle_Create;
#if 0
				createPtr->uIndex = activeGuidePtr->uIndex;
#if 1
				createPtr->vertexBits = insideVertexBits;
				createPtr->factorArray = factorArray;
				createPtr->transversals__ = activeGuidePtr->transversals;
				createPtr->transversals = activeGuidePtr->transversals;
#endif
#endif
				if (!createArrayPtr) {
					createArrayPtr = new DataArray_Tentacle_Create;
					insideArray_Tentacle_Create.append(createArrayPtr);
					createArrayPtr->dagPath = activeSelectionPtr->dagPath;
					createArrayPtr->selectBy = activeSelectionPtr->selectBy;
					createArrayPtr->numPoints = activeSelectionPtr->numPoints;
					createArrayPtr->worldMatrix = activeSelectionPtr->worldMatrix;
				}
				createArrayPtr->dataArray.append(createPtr);
			}
		}
	}
#if 0
	MGlobal::setActiveSelectionList(outgoingList, MGlobal::kReplaceList);
#endif
	return ms;
}
#endif
#endif
