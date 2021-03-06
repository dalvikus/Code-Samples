#if defined(_MSC_VER) && (_MSC_VER >= 1400)
#else
#include <math.h>
#endif
#include <assert.h>
#include <tentacleShape/tentacleShapeUI.h>
#include <tentacleShape/tentacleShape.h>
#include <tentacleShape/kRamp.h>
#include <maya/MDrawData.h>
#include <maya/MMaterial.h>
#include <maya/MObjectArray.h>
#include <maya/MFnSingleIndexedComponent.h>
#define TENTACLE
#ifdef TENTACLE
#include <tentacleShape/kTentacle.h>
#include <maya/MQuaternion.h>
#include <gl/GLU.h>
#include <maya/MGlobal.h>
#include <maya/MFnCamera.h>
#if 0
static void	DrawCreatePoint(M3dView& view, const MPoint& p);
static void	DrawWakes(M3dView& view, const MMatrix& mInvT, const MPointArray& worldPts, const kTentacle& tentacle);
static void	DrawRampInViewPort(M3dView& view, const kPtrArray<RampTuple>& rampTupleArray);
static void	DrawRampInCameraFront(M3dView& view, const kPtrArray<RampTuple>& rampTupleArray);
#endif
#if 2
#define _INHERIT_kBODY
#ifdef _INHERIT_kBODY
#pragma warning(disable: 4250)
#endif
#include <simulation/kShSimulationMayaTentacle.h>
#ifdef _INHERIT_kBODY
#pragma warning(default: 4250)
#else
#include <simulation/kBodyMayaTentacle.h>
#endif
#endif
#endif
#if 1
#define LEAD_COLOR		18
#define ACTIVE_COLOR		15
#define ACTIVE_AFFECTED_COLOR	8
#define DORMANT_COLOR		4
#define HILITE_COLOR		17
#define DORMANT_VERTEX_COLOR	8
#define ACTIVE_VERTEX_COLOR	16
#if 1
#define DORMANT_VERTEX_COLOR0	4
#define DORMANT_VERTEX_COLOR2	16
#define ACTIVE_VERTEX_COLOR0	18
#define ACTIVE_VERTEX_COLOR2	8
#endif
#define POINT_SIZE	4.0
#endif
tentacleShapeUI::tentacleShapeUI()
{
}
tentacleShapeUI::~tentacleShapeUI()
{
}
void*	tentacleShapeUI::creator()
{
	return new tentacleShapeUI();
}
#if 1
void
tentacleShapeUI::getDrawRequests(
	const MDrawInfo&	info,
	bool	objectAndActiveOnly,
	MDrawRequestQueue&	queue
)
{
#ifdef tentacleShapeUI_DEBUG
	cout << "tentacleShapeUI::getDrawRequests()" << endl;
#endif
#ifndef TENTACLE
#if 1
	int	iSelectBy;
	getSelectBy(iSelectBy);
#ifdef tentacleShapeUI_DEBUG
	cout << "tentacleShapeUI::getDrawRequests(): selectBy:= " << (iSelectBy == baseShape::selectByGuide ? "byGuide" : (iSelectBy == baseShape::selectByVertex ? "byVertex" : "byUnknown")) << endl;
#endif
#endif
#endif
	MDrawData	data;
	tentacleShape*	shapeNode = (tentacleShape*) surfaceShape();
	baseShapeGeometry*	geometryPtr = shapeNode->meshGeom();
	if (NULL == geometryPtr || 0 == geometryPtr->faceCount) {
		cerr << "NO DrawRequest for tentacleShape\n";
		return;
	}
	MDrawRequest	request = info.getPrototype(*this);
	getDrawData(geometryPtr, data);
	request.setDrawData(data);
	M3dView::DisplayStyle  appearance    = info.displayStyle();
	M3dView::DisplayStatus displayStatus = info.displayStatus();
#if 0
	cout << "Display Style: " << (
		appearance == M3dView::kBoundingBox ? "M3dView::kBoundingBox" : (
		appearance == M3dView::kFlatShaded ? "M3dView::kFlatShaded" : (
		appearance == M3dView::kGouraudShaded ? "M3dView::kGouraudShaded" : (
		appearance == M3dView::kWireFrame ? "M3dView::kWireFrame" : (
		appearance == M3dView::kPoints ? "M3dView::kPoints" : (
			"Unknown"
		)))))
	) << endl;
	cout << "Display Status: " << (
		displayStatus == M3dView::kActive ? "M3dView::kActive" : (
		displayStatus == M3dView::kLive ? "M3dView::kLive" : (
		displayStatus == M3dView::kDormant ? "M3dView::kDormant" : (
		displayStatus == M3dView::kInvisible ? "M3dView::kInvisible" : (
		displayStatus == M3dView::kHilite ? "M3dView::kHilite" : (
		displayStatus == M3dView::kTemplate ? "M3dView::kTemplate" : (
		displayStatus == M3dView::kActiveTemplate ? "M3dView::kActiveTemplate" : (
		displayStatus == M3dView::kActiveComponent ? "M3dView::kActiveComponent" : (
		displayStatus == M3dView::kLead ? "M3dView::kLead" : (
		displayStatus == M3dView::kIntermediateObject ? "M3dView::kIntermediateObject" : (
		displayStatus == M3dView::kActiveAffected ? "M3dView::kActiveAffected" : (
		displayStatus == M3dView::kNoStatus ? "M3dView::kNoStatus" : (
			"Unknown"
		))))))))))))
	) << endl;
#endif
	if (!info.objectDisplayStatus(M3dView::kDisplayMeshes))
		return;
	switch (appearance) {
		case M3dView::kWireFrame:
		{
			request.setToken( kDrawWireframe );
			M3dView::ColorTable activeColorTable = M3dView::kActiveColors;
			M3dView::ColorTable dormantColorTable = M3dView::kDormantColors;
			switch (displayStatus) {
			case M3dView::kLead :
				request.setColor(LEAD_COLOR, activeColorTable);
				break;
			case M3dView::kActive :
				request.setColor(ACTIVE_COLOR, activeColorTable);
				break;
			case M3dView::kActiveAffected :
				request.setColor(ACTIVE_AFFECTED_COLOR, activeColorTable);
				break;
			case M3dView::kDormant :
				request.setColor(DORMANT_COLOR, dormantColorTable);
				break;
			case M3dView::kHilite :
				request.setColor(HILITE_COLOR, activeColorTable);
				break;
			default:
				break;
			}
			queue.add(request);
			break;
		}
		case M3dView::kGouraudShaded:
		{
			request.setToken(kDrawSmoothShaded);
			MDagPath	path = info.multiPath();
			M3dView		view = info.view();;
			MMaterial	material = MPxSurfaceShapeUI::material( path );
			if (!material.evaluateMaterial(view, path)) {
				cerr << "Couldnt evaluate\n";
			}
			bool	drawTexture = true;
			if (drawTexture && material.materialIsTextured()) {
				material.evaluateTexture(data);
			}
			request.setMaterial(material);
			bool	materialTransparent = false;
			material.getHasTransparency(materialTransparent);
			if (materialTransparent) {
				request.setIsTransparent(true);
			}
			queue.add(request);
			if (
				displayStatus == M3dView::kActive ||
				displayStatus == M3dView::kLead ||
				displayStatus == M3dView::kHilite
			) {
				MDrawRequest	wireRequest = info.getPrototype(*this);
				wireRequest.setDrawData(data);
				wireRequest.setToken(kDrawWireframeOnShaded);
				wireRequest.setDisplayStyle(M3dView::kWireFrame);
				M3dView::ColorTable activeColorTable = M3dView::kActiveColors;
				switch (displayStatus) {
				case M3dView::kLead :
					wireRequest.setColor(LEAD_COLOR, activeColorTable);
					break;
				case M3dView::kActive :
					wireRequest.setColor(ACTIVE_COLOR, activeColorTable);
					break;
				case M3dView::kHilite :
					wireRequest.setColor(HILITE_COLOR, activeColorTable);
					break;
				default:
					break;
				}
				queue.add(wireRequest);
			}
			break;
		}
		case M3dView::kFlatShaded:
			request.setToken(kDrawFlatShaded);
			queue.add(request);
			break;
		case M3dView::kBoundingBox:
			request.setToken(kDrawBoundingBox);
			queue.add(request);
			break;
		default:
			break;
	}
#if 0
	cout << "objectAndActiveOnly? " << (objectAndActiveOnly ? "True" : "False") << endl;
#endif
	if (!objectAndActiveOnly) {
		if (
			appearance == M3dView::kPoints ||
			displayStatus == M3dView::kHilite
		)
		{
			MDrawRequest	vertexRequest = info.getPrototype(*this);
			vertexRequest.setDrawData(data);
			vertexRequest.setToken(kDrawVertices);
#ifdef TENTACLE
			vertexRequest.setColor(DORMANT_VERTEX_COLOR, M3dView::kActiveColors);
#else
#if 1
			vertexRequest.setColor(
				iSelectBy == baseShape::selectByGuide ? DORMANT_VERTEX_COLOR0 :
				(
					iSelectBy == baseShape::selectByVertex ? DORMANT_VERTEX_COLOR2 :
					(
						DORMANT_VERTEX_COLOR
					)
				),
				M3dView::kActiveColors
			);
#else
			vertexRequest.setColor(DORMANT_VERTEX_COLOR, M3dView::kActiveColors);
#endif
#endif
			queue.add(vertexRequest);
		}
#if 0
		cout << "hasActiveComponents? " << (surfaceShape()->hasActiveComponents() ? "True" : "False") << endl;
#endif
		if (surfaceShape()->hasActiveComponents()) {
#ifdef tentacleShapeUI_DEBUG
			cout << "tentacleShapeUI::getDrawRequests: hasComponent" << endl;
#endif
			MDrawRequest	activeVertexRequest = info.getPrototype(*this);
			activeVertexRequest.setDrawData(data);
			activeVertexRequest.setToken(kDrawVertices);
#ifdef TENTACLE
			activeVertexRequest.setColor(ACTIVE_VERTEX_COLOR, M3dView::kActiveColors);
#else
#if 1
			activeVertexRequest.setColor(
				iSelectBy == baseShape::selectByGuide ? ACTIVE_VERTEX_COLOR0 :
				(
					iSelectBy == baseShape::selectByVertex ? ACTIVE_VERTEX_COLOR2 :
					(
						ACTIVE_VERTEX_COLOR
					)
				),
				M3dView::kActiveColors
			);
#else
			activeVertexRequest.setColor(ACTIVE_VERTEX_COLOR, M3dView::kActiveColors);
#endif
#endif
			MObjectArray	clist = surfaceShape()->activeComponents();
			MObject	vertexComponent = clist[0];
			activeVertexRequest.setComponent(vertexComponent);
			queue.add(activeVertexRequest);
		}
	}
}
void
tentacleShapeUI::draw(
	const MDrawRequest&	request,
	M3dView&	view
) const
{
#ifdef tentacleShapeUI_DEBUG
	cout << "tentacleShapeUI::draw()" << endl;
#endif
#if 3
	MStatus	ms;
	MPlug	plug_tentacleArray(((tentacleShape*) surfaceShape())->thisMObject(), ((tentacleShape*) surfaceShape())->__tentacleArray);
	(void) plug_tentacleArray.evaluateNumElements(&ms);
	const kPtrArray<kTentacle>&	tentacleArray = ((tentacleShape*) surfaceShape())->tentacles();
#else
	kPtrArray<kTentacle>	tentacleArray;
	MStatus	ms = ((tentacleShape*) surfaceShape())->getTentacleArray(tentacleArray);
#endif
#if 1
#if 1
	bool	isDrawTentacleRoot;
	ms = ((tentacleShape*) surfaceShape())->getDrawTentacleRootStatus(isDrawTentacleRoot);
	if (ms && isDrawTentacleRoot) {
		const MMatrix&	m = ((tentacleShape*) surfaceShape())->externalInclusiveMatrix();
		MPoint	p;
		ms = ((tentacleShape*) surfaceShape())->getTentacleRootPosition(p);
		p *= m;
		if (ms)
			DrawCreatePoint(view, p);
	}
#endif
#if 1
	const kPtrArray<RampTuple>*	rampTupleArrayPtr = ((tentacleShape*) surfaceShape())->GetRampTupleArrayPtr();
	if (rampTupleArrayPtr) {
		MPlug	plug(((tentacleShape*) surfaceShape())->thisMObject(), tentacleShape::rampDisplay);
		RampDisplay::Type	displayType;
		ms = plug.getValue(*((int*) &displayType));
		if (displayType == RampDisplay::ViewPort)
			DrawRampInViewPort(view, *rampTupleArrayPtr);
		if (displayType == RampDisplay::CameraFront)
			DrawRampInCameraFront(view, *rampTupleArrayPtr);
	}
#endif
#endif
	int	token = request.token();
	switch (token) {
	case kDrawWireframe :
	case kDrawWireframeOnShaded :
#if 0
		cout << "kDrawWireframe or kDrawWireframeOnShaded" << endl;
#endif
		drawVertices0(tentacleArray, request, view);
#if 2
		if (((tentacleShape*) surfaceShape())->shSimulationMayaPtr())
			drawStrands(request, view);
#endif
		break;
	case kDrawSmoothShaded :
#if 0
		cout << "kDrawSmoothShaded" << endl;
#endif
		drawVertices0(tentacleArray, request, view);
#if 2
		if (((tentacleShape*) surfaceShape())->shSimulationMayaPtr())
			drawStrands(request, view);
#endif
		break;
	case kDrawFlatShaded :
#if 0
		cout << "kDrawFlatShaded: Not implemented" << endl;
#endif
		break;
	case kDrawVertices :
#if 0
		cout << "kDrawVertices" << endl;
#endif
		drawVertices0(tentacleArray, request, view);
#if 2
		if (((tentacleShape*) surfaceShape())->shSimulationMayaPtr())
			drawStrands(request, view);
#endif
		break;
	case kDrawBoundingBox:
		cout << "kDrawBoundingBox: Not implemented" << endl;
		break;
	}
}
bool
tentacleShapeUI::select(
	MSelectInfo&	selectInfo,
	MSelectionList&	selectionList,
	MPointArray&	worldSpaceSelectPts
) const
{
#ifdef tentacleShapeUI_DEBUG
	cout << "tentacleShapeUI::select()" << endl;
#endif
#if 3
	const kPtrArray<kTentacle>&	tentacleArray = ((tentacleShape*) surfaceShape())->tentacles();
#else
	kPtrArray<kTentacle>	tentacleArray;
	MStatus	ms = ((tentacleShape*) surfaceShape())->getTentacleArray(tentacleArray);
#endif
#if 1
#if 2
	bool	selected;
	if (((tentacleShape*) surfaceShape())->shSimulationMayaPtr())
		selected = selectStrands(selectInfo, selectionList, worldSpaceSelectPts);
	else
		selected = selectVertices0(tentacleArray, selectInfo, selectionList, worldSpaceSelectPts);
#else
	bool	selected = selectVertices0(tentacleArray, selectInfo, selectionList, worldSpaceSelectPts);
#endif
#else
	bool	componentSelected = false;
	bool	hilited = false;
	hilited = selectInfo.displayStatus() == M3dView::kHilite;
	if (hilited) {
#if 2
		if (((tentacleShape*) surfaceShape())->shSimulationMayaPtr())
			componentSelected = selectStrands(selectInfo, selectionList, worldSpaceSelectPts);
		else
			componentSelected = selectVertices0(tentacleArray, selectInfo, selectionList, worldSpaceSelectPts);
#else
		componentSelected = selectVertices0(tentacleArray, selectInfo, selectionList, worldSpaceSelectPts);
#endif
		selected = selected || componentSelected;
	}
	if (!selected) {
		tentacleShape* shapeNode = (tentacleShape*) surfaceShape();
		selected = true;
		MSelectionMask	priorityMask(MSelectionMask::kSelectNurbsSurfaces);
		MSelectionList	item;
		item.add(selectInfo.selectPath());
		MPoint	xformedPt;
		if (selectInfo.singleSelection()) {
			MPoint	center = shapeNode->boundingBox().center();
			xformedPt = center;
			xformedPt *= selectInfo.selectPath().inclusiveMatrix();
		}
		selectInfo.addSelection(item, xformedPt, selectionList, worldSpaceSelectPts, priorityMask, false);
	}
#endif
	return selected;
}
#endif
#if 1
void
tentacleShapeUI::drawVertices0(
	const kPtrArray<kTentacle>&	tentacleArray,
	const MDrawRequest&	request,
	M3dView&	view
) const
{
#ifdef tentacleShapeUI_DEBUG
	cout << "tentacleShapeUI::drawVertices0()" << endl;
#endif
	MDrawData data = request.drawData();
#if 0
	MVectorArray*	cpPtr = ((tentacleShape*) surfaceShape())->getControlPoints();
	MVectorArray&	cp = *cpPtr;
	baseShapeGeometry * geometryPtr = (baseShapeGeometry*) data.geometry();
#endif
	view.beginGL();
	bool	lightingWasOn = glIsEnabled(GL_LIGHTING) ? true : false;
	if (lightingWasOn) {
		glDisable(GL_LIGHTING);
	}
	float	lastPointSize;
	glGetFloatv(GL_POINT_SIZE, &lastPointSize);
	glPointSize(POINT_SIZE);
	MObject	comp = request.component();
#if 0
#if 10
	int	numExternalStrands = ((tentacleShape*) surfaceShape())->numExternalStrands();
	if (numExternalStrands >= 0) {
		for (int indexStrand = 0; indexStrand < numExternalStrands; ++indexStrand) {
			glBegin(GL_LINE_STRIP);
			for (int k = 0; k < (int) baseShape::N_POINTS_IN_GUIDE; ++k) {
				MVector	worldPt = cp[baseShape::N_POINTS_IN_GUIDE * indexStrand + k];
				glVertex3f((float) worldPt[0], (float) worldPt[1], (float) worldPt[2]);
			}
			glEnd();
		}
		return;
	}
#endif
#endif
#if 3
	const MMatrix&	m = ((tentacleShape*) surfaceShape())->externalInclusiveMatrix();
	MMatrix	mInvT = m.inverse().transpose();
	MPointArray	worldPts(NUM_TENTACLE_SAMPLE_POINTS);
#endif
#if 1
	bool	hasSkinCluster = ((tentacleShape*) surfaceShape())->skinSmoothBind().hasSkinCluster();
#endif
#if 0
	cout << "comp.isNull? " << (comp.isNull() ? "True" : "False") << endl;
#endif
#if 1
	if (!comp.isNull()) {
#if 1
		MFnSingleIndexedComponent	fnComponent(comp);
#ifdef tentacleShapeUI_DEBUG
		cout << "tentacleShapeUI::drawVertices0(): MFnSingleIndexedComponent::elementCount() returns " << fnComponent.elementCount() << endl;
#endif
		for (int i = 0; i < fnComponent.elementCount(); ++i) {
			int	uIndex = fnComponent.element(i);
			if (uIndex >= (int) tentacleArray.length())
				continue;
			const kTentacle&	tentacle = tentacleArray(uIndex);
#if 1
	if (hasSkinCluster) {
			glBegin(GL_LINE_STRIP);
			for (int k = 0; k < (int) NUM_TENTACLE_SAMPLE_POINTS; ++k) {
				const MFloatVector&	point = tentacle.wPoints[k];
				glVertex3f(point[0], point[1], point[2]);
			}
			glEnd();
			if (((tentacleShape*) surfaceShape())->displayWakes()) {
				MPointArray	strandSamplePoints;
				MPointArray	tentacleSamplePoints;
				MStatus	ms = tentacle.getSamplePoints(strandSamplePoints, tentacleShape::numEllipseSamplePoints, tentacleSamplePoints);
				if (ms)
					DrawWakes(view, strandSamplePoints, tentacleSamplePoints, tentacle);
			}
#if 0
	}  {
#else
	} else {
#endif
#endif
		{
#if 1
			MPoint	basePoint0(((kRoot&) tentacle).position);
#else
			MPoint	basePoint0(tentacle.root);
#endif
			MPoint	point = basePoint0 * m;
			double	len0 = tentacle.longitudinal[0].length();
			for (int k = 0; k < NUM_TENTACLE_SAMPLE_POINTS; ++k) {
				if (k) {
					MVector	offset = tentacle.longitudinal[k - 1];
					offset *= mInvT;
					offset.normalize();
					point += len0 * offset;
				}
				worldPts[k] = point;
			}
		}
#if 1
			if (((tentacleShape*) surfaceShape())->displayWakes()) {
				MPointArray	strandSamplePoints;
				MPointArray	tentacleSamplePoints;
				tentacle.getSamplePoints(m, strandSamplePoints, tentacleShape::numEllipseSamplePoints, tentacleSamplePoints);
				DrawWakes(view, strandSamplePoints, tentacleSamplePoints, tentacle);
			}
#else
			if (((tentacleShape*) surfaceShape())->displayWakes())
				DrawWakes(view, mInvT, worldPts, tentacle);
#endif
			glBegin(GL_LINE_STRIP);
			for (int k = 0; k < (int) NUM_TENTACLE_SAMPLE_POINTS; ++k) {
				const MPoint&	point = worldPts[k];
				glVertex3d(point[0], point[1], point[2]);
			}
			glEnd();
#if 1
	}
#endif
		}
#else
		MFnSingleIndexedComponent	fnComponent(comp);
#ifdef tentacleShapeUI_DEBUG
		cout << "tentacleShapeUI::drawVertices0(): MFnSingleIndexedComponent::elementCount() returns " << fnComponent.elementCount() << endl;
#endif
		for (int i = 0; i < fnComponent.elementCount(); ++i) {
			int	index = fnComponent.element(i);
#if 1
			glBegin(GL_LINE_STRIP);
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
				glVertex3f((float) point[0], (float) point[1], (float) point[2]);
			}
			glEnd();
#if 0
			glBegin(GL_POINTS);
			point = basePoint;
			for (int k = 0; k < (int) baseShape::N_POINTS_IN_GUIDE; ++k) {
				if (k) {
					MVector	offset = cp[baseShape::N_POINTS_IN_GUIDE * index + k];
					double	len = offset.length();
#if 3
					offset = offset.transformAsNormal(m);
#endif
					point += len * offset;
				}
				glVertex3f((float) point[0], (float) point[1], (float) point[2]);
			}
			glEnd();
#endif
#if 2
if (((tentacleShape*) surfaceShape())->displayWakes()) {
			glPointSize(POINT_SIZE + 2);
			glBegin(GL_POINTS);
			{
#if 3
				MPoint	point = geometryPtr->wakes[WAKE_POINTS * index] * m;
				MVector	v(point - basePoint);
				v.normalize();
				point = basePoint + v;
#else
				MPoint&	point = geometryPtr->wakes[WAKE_POINTS * index];
#endif
				glVertex3f((float) point[0], (float) point[1], (float) point[2]);
			}
			glEnd();
			glPointSize(POINT_SIZE - 2 > 0 ? POINT_SIZE - 2 : 1);
			glBegin(GL_POINTS);
			for (int k = 1; k < WAKE_POINTS; ++k) {
#if 3
				MPoint	point = geometryPtr->wakes[WAKE_POINTS * index + k] * m;
				MVector	v(point - basePoint);
				v.normalize();
				point = basePoint + v;
#else
				MPoint&	point = geometryPtr->wakes[WAKE_POINTS * index + k];
#endif
				glVertex3f((float) point[0], (float) point[1], (float) point[2]);
			}
			glEnd();
			glPointSize(POINT_SIZE);
}
#endif
#else
			glBegin( GL_POINTS );
			MPoint vertex = geometryPtr->vertices[ index  ];
			glVertex3f( (float)vertex[0],
						(float)vertex[1],
						(float)vertex[2] );
			glEnd();
#endif
#if 0
			char	annotation[32];
#if 1
			(void) _snprintf_s(annotation, 32, _TRUNCATE, "%d", index);
			view.drawText(annotation, basePoint);
#else
			sprintf(annotation, "%d", index);
			view.drawText( annotation, vertex );
#endif
#endif
		}
#endif
	} else {
#if 1
	for (int uIndex = 0; uIndex < (int) tentacleArray.length(); ++uIndex) {
		const kTentacle&	tentacle = tentacleArray(uIndex);
#if 1
	if (hasSkinCluster) {
		glBegin(GL_LINE_STRIP);
		for (int k = 0; k < (int) NUM_TENTACLE_SAMPLE_POINTS; ++k) {
			const MFloatVector&	point = tentacle.wPoints[k];
			glVertex3f(point[0], point[1], point[2]);
		}
		glEnd();
		if (((tentacleShape*) surfaceShape())->displayWakes()) {
			MPointArray	strandSamplePoints;
			MPointArray	tentacleSamplePoints;
			MStatus	ms = tentacle.getSamplePoints(strandSamplePoints, tentacleShape::numEllipseSamplePoints, tentacleSamplePoints);
			if (ms)
				DrawWakes(view, strandSamplePoints, tentacleSamplePoints, tentacle);
		}
#if 0
	}  {
#else
	} else {
#endif
#endif
	{
#if 1
		MPoint	basePoint0(((kRoot&) tentacle).position);
#else
		MPoint	basePoint0(tentacle.root);
#endif
		MPoint	point = basePoint0 * m;
		double	len0 = tentacle.longitudinal[0].length();
		for (int k = 0; k < NUM_TENTACLE_SAMPLE_POINTS; ++k) {
			if (k) {
				MVector	offset = tentacle.longitudinal[k - 1];
				offset *= mInvT;
				offset.normalize();
				point += len0 * offset;
			}
			worldPts[k] = point;
		}
	}
		glBegin(GL_LINE_STRIP);
		for (int k = 0; k < (int) NUM_TENTACLE_SAMPLE_POINTS; ++k) {
			const MPoint&	point = worldPts[k];
			glVertex3d(point[0], point[1], point[2]);
		}
		glEnd();
#if 1
	}
#endif
	}
#else
#ifdef tentacleShapeUI_DEBUG
		cout << "tentacleShapeUI::drawVertices0(): comp.isNull()" << endl;
#endif
#if 0
		drawHair();
#endif
#if 1
		for (int index = 0; index < (int) geometryPtr->vertices.length(); ++index) {
#if 1
			MPoint	basePoint(cp[baseShape::N_POINTS_IN_GUIDE * index + 0]);
#else
			MPoint	basePoint = geometryPtr->vertices[index];
#endif
#if 3
			basePoint *= m;
#endif
			glBegin(GL_LINE_STRIP);
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
				glVertex3f((float) point[0], (float) point[1], (float) point[2]);
			}
			glEnd();
#if 0
			glBegin(GL_POINTS);
			point = basePoint;
			for (int k = 0; k < (int) baseShape::N_POINTS_IN_GUIDE; ++k) {
				if (k) {
					MVector	offset = cp[baseShape::N_POINTS_IN_GUIDE * index + k];
					double	len = offset.length();
#if 3
					offset = offset.transformAsNormal(m);
#endif
					point += len * offset;
				}
				glVertex3f((float) point[0], (float) point[1], (float) point[2]);
			}
			glEnd();
#endif
		}
#else
		int vid = 0;
		for ( int i=0; i<geometryPtr->faceCount; i++ )
		{
			glBegin( GL_POINTS );
			for ( int v=0; v<geometryPtr->face_counts[i]; v++ )
			{
				MPoint vertex =
					geometryPtr->vertices[ geometryPtr->face_connects[vid++] ];
				glVertex3f( (float)vertex[0],
							(float)vertex[1],
							(float)vertex[2] );
			}
			glEnd();
		}
#endif
#endif
	}
#else
	if (!comp.isNull()) {
		MFnSingleIndexedComponent	fnComponent(comp);
#ifdef tentacleShapeUI_DEBUG
		cout << "tentacleShapeUI::drawVertices0(): MFnSingleIndexedComponent::elementCount() returns " << fnComponent.elementCount() << endl;
#endif
		for (int i = 0; i < fnComponent.elementCount(); ++i) {
			int	index = fnComponent.element(i);
#if 1
			glBegin(GL_LINE_STRIP);
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
				glVertex3f((float) point[0], (float) point[1], (float) point[2]);
			}
			glEnd();
#if 0
			glBegin(GL_POINTS);
			point = basePoint;
			for (int k = 0; k < (int) baseShape::N_POINTS_IN_GUIDE; ++k) {
				if (k) {
					MVector	offset = cp[baseShape::N_POINTS_IN_GUIDE * index + k];
					double	len = offset.length();
#if 3
					offset = offset.transformAsNormal(m);
#endif
					point += len * offset;
				}
				glVertex3f((float) point[0], (float) point[1], (float) point[2]);
			}
			glEnd();
#endif
#if 2
if (((tentacleShape*) surfaceShape())->displayWakes()) {
			glPointSize(POINT_SIZE + 2);
			glBegin(GL_POINTS);
			{
#if 3
				MPoint	point = geometryPtr->wakes[WAKE_POINTS * index] * m;
				MVector	v(point - basePoint);
				v.normalize();
				point = basePoint + v;
#else
				MPoint&	point = geometryPtr->wakes[WAKE_POINTS * index];
#endif
				glVertex3f((float) point[0], (float) point[1], (float) point[2]);
			}
			glEnd();
			glPointSize(POINT_SIZE - 2 > 0 ? POINT_SIZE - 2 : 1);
			glBegin(GL_POINTS);
			for (int k = 1; k < WAKE_POINTS; ++k) {
#if 3
				MPoint	point = geometryPtr->wakes[WAKE_POINTS * index + k] * m;
				MVector	v(point - basePoint);
				v.normalize();
				point = basePoint + v;
#else
				MPoint&	point = geometryPtr->wakes[WAKE_POINTS * index + k];
#endif
				glVertex3f((float) point[0], (float) point[1], (float) point[2]);
			}
			glEnd();
			glPointSize(POINT_SIZE);
}
#endif
#else
			glBegin( GL_POINTS );
			MPoint vertex = geometryPtr->vertices[ index  ];
			glVertex3f( (float)vertex[0],
						(float)vertex[1],
						(float)vertex[2] );
			glEnd();
#endif
#if 0
			char	annotation[32];
#if 1
			(void) _snprintf_s(annotation, 32, _TRUNCATE, "%d", index);
			view.drawText(annotation, basePoint);
#else
			sprintf(annotation, "%d", index);
			view.drawText( annotation, vertex );
#endif
#endif
		}
	} else {
#ifdef tentacleShapeUI_DEBUG
		cout << "tentacleShapeUI::drawVertices0(): comp.isNull()" << endl;
#endif
#if 0
		drawHair();
#endif
#if 1
		for (int index = 0; index < (int) geometryPtr->vertices.length(); ++index) {
#if 1
			MPoint	basePoint(cp[baseShape::N_POINTS_IN_GUIDE * index + 0]);
#else
			MPoint	basePoint = geometryPtr->vertices[index];
#endif
#if 3
			basePoint *= m;
#endif
			glBegin(GL_LINE_STRIP);
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
				glVertex3f((float) point[0], (float) point[1], (float) point[2]);
			}
			glEnd();
#if 0
			glBegin(GL_POINTS);
			point = basePoint;
			for (int k = 0; k < (int) baseShape::N_POINTS_IN_GUIDE; ++k) {
				if (k) {
					MVector	offset = cp[baseShape::N_POINTS_IN_GUIDE * index + k];
					double	len = offset.length();
#if 3
					offset = offset.transformAsNormal(m);
#endif
					point += len * offset;
				}
				glVertex3f((float) point[0], (float) point[1], (float) point[2]);
			}
			glEnd();
#endif
		}
#else
		int vid = 0;
		for ( int i=0; i<geometryPtr->faceCount; i++ )
		{
			glBegin( GL_POINTS );
			for ( int v=0; v<geometryPtr->face_counts[i]; v++ )
			{
				MPoint vertex =
					geometryPtr->vertices[ geometryPtr->face_connects[vid++] ];
				glVertex3f( (float)vertex[0],
							(float)vertex[1],
							(float)vertex[2] );
			}
			glEnd();
		}
#endif
	}
#endif
	if (lightingWasOn) {
		glEnable(GL_LIGHTING);
	}
	glPointSize(lastPointSize);
	view.endGL();
}
#if 2
void
tentacleShapeUI::drawStrands(
	const MDrawRequest&	,
	M3dView&	view
) const
{
	const kShSimulationMayaTentacle*	shSimulationMayaPtr = ((tentacleShape*) surfaceShape())->shSimulationMayaPtr();
	if (!shSimulationMayaPtr)
		return;
	const kSimulationCache::kStrandArray&	strandArray = shSimulationMayaPtr->strandArray();
	if (strandArray.length() == 0) {
		return;
	}
	const MVectorArray&	strand_longitudinals = ((tentacleShape*) surfaceShape())->strand_longitudinals();
	unsigned	numStrandSamplePoints = strand_longitudinals.length() / strandArray.length();
	if (numStrandSamplePoints < 2) {
		return;
	}
	view.beginGL();
	bool lightingWasOn = glIsEnabled( GL_LIGHTING ) ? true : false;
	if ( lightingWasOn ) {
		glDisable( GL_LIGHTING );
	}
	float lastPointSize;
	glGetFloatv( GL_POINT_SIZE, &lastPointSize );
	GLfloat	rgba[4];
	glGetFloatv(GL_CURRENT_COLOR, rgba);
	glColor3f(1, 0, 0);
#pragma warning(disable: 4127)
do {
	glPointSize( POINT_SIZE );
#if 1
	for (int indexStrand = 0; indexStrand < (int) strandArray.length(); ++indexStrand) {
		glBegin(GL_LINE_STRIP);
		for (int k = 0; k < (int) numStrandSamplePoints; ++k) {
			const MVector&	worldPt = strand_longitudinals[indexStrand * numStrandSamplePoints + k];
			glVertex3f((float) worldPt[0], (float) worldPt[1], (float) worldPt[2]);
		}
		glEnd();
	}
#else
#if 10
	int	numExternalStrands = ((baseShape*) surfaceShape())->numExternalStrands();
	if (numExternalStrands >= 0) {
		for (int indexStrand = 0; indexStrand < numExternalStrands; ++indexStrand) {
			glBegin(GL_LINE_STRIP);
			for (int k = 0; k < (int) numStrandSamplePoints; ++k) {
				const MVector&	worldPt = strand_longitudinals[indexStrand * numStrandSamplePoints + k];
				glVertex3f((float) worldPt[0], (float) worldPt[1], (float) worldPt[2]);
			}
			glEnd();
		}
		break;
	}
#endif
	const MMatrix&	m = ((baseShape*) surfaceShape())->externalInclusiveMatrix();
	for (int indexStrand = 0; indexStrand < (int) strandArray.length(); ++indexStrand) {
		glBegin(GL_LINE_STRIP);
		MPoint	basePoint(strand_longitudinals[indexStrand * numStrandSamplePoints + 0]);
		basePoint *= m;
		MPoint	point = basePoint;
		for (int k = 0; k < (int) numStrandSamplePoints; ++k) {
			if (k) {
				MVector	offset = strand_longitudinals[indexStrand * numStrandSamplePoints + k];
				double	len = offset.length();
				offset = offset.transformAsNormal(m);
				point += len * offset;
			}
			glVertex3f((float) point[0], (float) point[1], (float) point[2]);
		}
		glEnd();
	}
#endif
} while (0);
#pragma warning(default: 4127)
	if ( lightingWasOn ) {
		glEnable( GL_LIGHTING );
	}
	glPointSize( lastPointSize );
	glColor3fv(rgba);
	view.endGL();
}
#endif
bool
tentacleShapeUI::selectVertices0(
	const kPtrArray<kTentacle>&	tentacleArray,
	MSelectInfo&	selectInfo,
	MSelectionList&	selectionList,
	MPointArray&	worldSpaceSelectPts
) const
{
#ifdef tentacleShapeUI_DEBUG
	cout << "tentacleShapeUI::selectVertices0()" << endl;
#endif
#if 0
	MVectorArray*	cpPtr = ((tentacleShape*) surfaceShape())->getControlPoints();
	MVectorArray&	cp = *cpPtr;
#endif
	bool selected = false;
	M3dView view = selectInfo.view();
	MPoint	xformedPoint;
	MPoint	currentPoint;
	MPoint	selectionPoint;
	double	z, previousZ = 0.0;
 	int	closestPointStrandIndex = -1;
	const MDagPath & path = selectInfo.multiPath();
	MFnSingleIndexedComponent fnComponent;
	MObject	surfaceComponent = fnComponent.create(MFn::kMeshEdgeComponent);
	MMatrix	alignmentMatrix;
	MPoint	singlePoint;
	bool	singleSelection = selectInfo.singleSelection();
	if (singleSelection) {
		alignmentMatrix = selectInfo.getAlignmentMatrix();
	}
#if 0
	tentacleShape* shapeNode = (tentacleShape*) surfaceShape();
	baseShapeGeometry * geometryPtr = shapeNode->meshGeom();
	int	numVertices = geometryPtr->vertices.length();
#endif
#if 3
	const MMatrix&	m = ((tentacleShape*) surfaceShape())->externalInclusiveMatrix();
	MMatrix	mInvT = m.inverse().transpose();
	MPointArray	worldPts(NUM_TENTACLE_SAMPLE_POINTS);
#endif
#if 1
	bool	hasSkinCluster = ((tentacleShape*) surfaceShape())->skinSmoothBind().hasSkinCluster();
#endif
#if 1
	for (int uIndex = 0; uIndex < (int) tentacleArray.length(); ++uIndex) {
		const kTentacle&	tentacle = tentacleArray(uIndex);
#if 1
	if (hasSkinCluster) {
		view.beginSelect();
		glBegin(GL_LINE_STRIP);
		for (int k = 0; k < (int) NUM_TENTACLE_SAMPLE_POINTS; ++k) {
			const MFloatVector&	point = tentacle.wPoints[k];
			glVertex3f(point[0], point[1], point[2]);
		}
		glEnd();
	} else {
#endif
	{
#if 1
		MPoint	basePoint0(((kRoot&) tentacle).position);
#else
		MPoint	basePoint0(tentacle.root);
#endif
		MPoint	point = basePoint0 * m;
		double	len0 = tentacle.longitudinal[0].length();
		for (int k = 0; k < NUM_TENTACLE_SAMPLE_POINTS; ++k) {
			if (k) {
				MVector	offset = tentacle.longitudinal[k - 1];
				offset *= mInvT;
				offset.normalize();
				point += len0 * offset;
			}
			worldPts[k] = point;
		}
	}
		view.beginSelect();
		glBegin(GL_LINE_STRIP);
		for (int k = 0; k < (int) NUM_TENTACLE_SAMPLE_POINTS; ++k) {
			const MPoint&	point = worldPts[k];
			glVertex3d(point[0], point[1], point[2]);
		}
		glEnd();
#if 1
	}
#endif
		if (view.endSelect() > 0) {
			selected = true;
			if (singleSelection) {
				xformedPoint = currentPoint;
				xformedPoint.homogenize();
				xformedPoint*= alignmentMatrix;
				z = xformedPoint.z;
				if (closestPointStrandIndex < 0 || z > previousZ) {
					closestPointStrandIndex = uIndex;
					singlePoint = currentPoint;
					previousZ = z;
				}
			} else {
#if 1
				tentacleShape::lastSelectedPhysicalIndex = uIndex;
#endif
				fnComponent.addElement(uIndex);
			}
		}
	}
#else
	for (int vertexIndex = 0; vertexIndex < numVertices; ++vertexIndex) {
#if 1
#if 1
		MPoint	basePoint(cp[baseShape::N_POINTS_IN_GUIDE * vertexIndex + 0]);
#else
		MPoint	basePoint = geometryPtr->vertices[vertexIndex];
#endif
#if 3
		basePoint *= m;
#endif
#else
		MPoint currentPoint = geometryPtr->vertices[ vertexIndex ];
#endif
		view.beginSelect();
#if 1
			glBegin(GL_LINE_STRIP);
			MPoint	point = basePoint;
			for (int k = 0; k < (int) baseShape::N_POINTS_IN_GUIDE; ++k) {
				if (k) {
					MVector	offset = cp[baseShape::N_POINTS_IN_GUIDE * vertexIndex + k];
					double	len = offset.length();
#if 3
					offset = offset.transformAsNormal(m);
#endif
					point += len * offset;
				}
				glVertex3f((float) point[0], (float) point[1], (float) point[2]);
			}
			glEnd();
#else
		glBegin( GL_POINTS );
		glVertex3f( (float)currentPoint[0],
					(float)currentPoint[1],
					(float)currentPoint[2] );
		glEnd();
#endif
		if ( view.endSelect() > 0 )
		{
			selected = true;
			if ( singleSelection ) {
				xformedPoint = currentPoint;
				xformedPoint.homogenize();
				xformedPoint*= alignmentMatrix;
				z = xformedPoint.z;
				if (closestPointStrandIndex < 0 || z > previousZ) {
					closestPointStrandIndex = vertexIndex;
					singlePoint = currentPoint;
					previousZ = z;
				}
			} else {
				fnComponent.addElement( vertexIndex );
			}
		}
	}
#endif
	if (selected && selectInfo.singleSelection()) {
#if 1
		tentacleShape::lastSelectedPhysicalIndex = closestPointStrandIndex;
#endif
		fnComponent.addElement(closestPointStrandIndex);
		selectionPoint = singlePoint;
		selectionPoint *= path.inclusiveMatrix();
	}
	if (selected) {
		MSelectionList	selectionItem;
		selectionItem.add(path, surfaceComponent);
		MSelectionMask	mask(MSelectionMask::kSelectComponentsMask);
		selectInfo.addSelection(
			selectionItem, selectionPoint,
			selectionList, worldSpaceSelectPts,
			mask, true
		);
	}
	return selected;
}
#if 2
bool
tentacleShapeUI::selectStrands(
	MSelectInfo&	selectInfo,
	MSelectionList&	selectionList,
	MPointArray&	worldSpaceSelectPts
) const
{
#ifdef tentacleShapeUI_DEBUG
	cout << "tentacleShapeUI::selectStrands()" << endl;
#endif
	const kShSimulationMayaTentacle*	shSimulationMayaPtr = ((tentacleShape*) surfaceShape())->shSimulationMayaPtr();
	if (!shSimulationMayaPtr)
		return false;
	const kSimulationCache::kStrandArray&	strandArray = shSimulationMayaPtr->strandArray();
	if (strandArray.length() == 0) {
		return false;
	}
	const MVectorArray&	strand_longitudinals = ((tentacleShape*) surfaceShape())->strand_longitudinals();
	unsigned	numStrandSamplePoints = strand_longitudinals.length() / strandArray.length();
	if (numStrandSamplePoints < 2) {
		return false;
	}
#if 0
	MVectorArray*	cpPtr = ((tentacleShape*) surfaceShape())->getControlPoints();
	MVectorArray&	cp = *cpPtr;
#endif
	bool selected = false;
	M3dView view = selectInfo.view();
	MPoint	xformedPoint;
	MPoint	currentPoint;
	MPoint	selectionPoint;
	double	z, previousZ = 0.0;
 	int	closestPointStrandIndex = -1;
	const MDagPath & path = selectInfo.multiPath();
	MFnSingleIndexedComponent fnComponent;
	MObject	surfaceComponent = fnComponent.create(MFn::kMeshEdgeComponent);
	MMatrix	alignmentMatrix;
	MPoint	singlePoint;
	bool	singleSelection = selectInfo.singleSelection();
	if (singleSelection) {
		alignmentMatrix = selectInfo.getAlignmentMatrix();
	}
#if 1
	for (int indexStrand = 0; indexStrand < (int) strandArray.length(); ++indexStrand) {
		const kSimulationCache::kStrand&	strand = strandArray(indexStrand);
		int	strandId = strand.strandId;
		view.beginSelect();
		glBegin(GL_LINE_STRIP);
		for (int k = 0; k < (int) numStrandSamplePoints; ++k) {
			const MVector&	worldPt = strand_longitudinals[indexStrand * numStrandSamplePoints + k];
			glVertex3f((float) worldPt[0], (float) worldPt[1], (float) worldPt[2]);
		}
		glEnd();
		if (view.endSelect() > 0) {
			selected = true;
			if (singleSelection) {
				xformedPoint = currentPoint;
				xformedPoint.homogenize();
				xformedPoint*= alignmentMatrix;
				z = xformedPoint.z;
				if (closestPointStrandIndex < 0 || z > previousZ) {
					closestPointStrandIndex = strandId;
					singlePoint = currentPoint;
					previousZ = z;
				}
			} else {
#if 1
				tentacleShape::lastSelectedPhysicalIndex = strandId;
#endif
				fnComponent.addElement(strandId);
			}
		}
	}
#endif
	if (selected && selectInfo.singleSelection()) {
#if 1
		tentacleShape::lastSelectedPhysicalIndex = closestPointStrandIndex;
#endif
		fnComponent.addElement(closestPointStrandIndex);
		selectionPoint = singlePoint;
		selectionPoint *= path.inclusiveMatrix();
	}
	if (selected) {
		MSelectionList	selectionItem;
		selectionItem.add(path, surfaceComponent);
		MSelectionMask	mask(MSelectionMask::kSelectComponentsMask);
		selectInfo.addSelection(
			selectionItem, selectionPoint,
			selectionList, worldSpaceSelectPts,
			mask, true
		);
	}
	return selected;
}
#endif
void	tentacleShapeUI::drawBoundingBox(const MDrawRequest& , M3dView& view) const
{
	MPxSurfaceShape *shape = surfaceShape();
	if ( !shape )
		return;
	MBoundingBox box = shape->boundingBox();
	float w = (float) box.width();
	float h = (float) box.height();
	float d = (float) box.depth();
	view.beginGL();
	MPoint minVertex = box.min();
	glBegin( GL_LINE_LOOP );
	MPoint vertex = minVertex;
	glVertex3f( (float)vertex[0],   (float)vertex[1],   (float)vertex[2] );
	glVertex3f( (float)vertex[0]+w, (float)vertex[1],   (float)vertex[2] );
	glVertex3f( (float)vertex[0]+w, (float)vertex[1]+h, (float)vertex[2] );
	glVertex3f( (float)vertex[0],   (float)vertex[1]+h, (float)vertex[2] );
	glVertex3f( (float)vertex[0],   (float)vertex[1],   (float)vertex[2] );
	glEnd();
	MPoint sideFactor(0,0,d);
	MPoint vertex2 = minVertex + sideFactor;
	glBegin( GL_LINE_LOOP );
	glVertex3f( (float)vertex2[0],   (float)vertex2[1],   (float)vertex2[2] );
	glVertex3f( (float)vertex2[0]+w, (float)vertex2[1],   (float)vertex2[2] );
	glVertex3f( (float)vertex2[0]+w, (float)vertex2[1]+h, (float)vertex2[2] );
	glVertex3f( (float)vertex2[0],   (float)vertex2[1]+h, (float)vertex2[2] );
	glVertex3f( (float)vertex2[0],   (float)vertex2[1],   (float)vertex2[2] );
	glEnd();
	glBegin( GL_LINES );
	glVertex3f( (float)vertex2[0],   (float)vertex2[1],   (float)vertex2[2] );
	glVertex3f( (float)vertex[0],   (float)vertex[1],   (float)vertex[2] );
	glVertex3f( (float)vertex2[0]+w,   (float)vertex2[1],   (float)vertex2[2] );
	glVertex3f( (float)vertex[0]+w,   (float)vertex[1],   (float)vertex[2] );
	glVertex3f( (float)vertex2[0]+w,   (float)vertex2[1]+h,   (float)vertex2[2] );
	glVertex3f( (float)vertex[0]+w,   (float)vertex[1]+h,   (float)vertex[2] );
	glVertex3f( (float)vertex2[0],   (float)vertex2[1]+h,   (float)vertex2[2] );
	glVertex3f( (float)vertex[0],   (float)vertex[1]+h,   (float)vertex[2] );
	glEnd();
	view.endGL();
}
#endif
#if 1
#if 1
#if 1
void	tentacleShapeUI::DrawCreatePoint(M3dView& view, const MPoint& p)
{
	MStatus	ms;
	view.beginGL();
#pragma warning(disable: 4127)
do {
	float	lastPointSize;
	glGetFloatv(GL_POINT_SIZE, &lastPointSize);
	GLfloat	lastLineWidth;
	glGetFloatv(GL_LINE_WIDTH, &lastLineWidth);
	GLboolean	rgba_mode;
	glGetBooleanv(GL_RGBA_MODE, &rgba_mode);
	GLfloat	rgba[4];
	glGetFloatv(GL_CURRENT_COLOR, rgba);
do {
	glPointSize(5);
	glColor3f(0, 1, 0);
	glBegin(GL_POINTS);
	glVertex3d(p.x, p.y, p.z);
	glEnd();
} while (0);
	glPointSize(lastPointSize);
	glLineWidth(lastLineWidth);
	glColor3fv(rgba);
} while (0);
#pragma warning(default: 4127)
	view.endGL();
}
#endif
#if 1
#if 1
#if 1
void	tentacleShapeUI::DrawWakes(M3dView& view, const MPointArray& strandSamplePoints, const MPointArray& tentacleSamplePoints, const kTentacle& tentacle)
{
	MStatus	ms;
	unsigned	numStrandSamplePoints = strandSamplePoints.length();
	if (!numStrandSamplePoints) {
		cerr << "FATAL: tentacleShapeUI::getSamplePoints(): # of sample points = 0!" << endl;
		return;
	}
	view.beginGL();
#pragma warning(disable: 4127)
{
	float	lastPointSize;
	glGetFloatv(GL_POINT_SIZE, &lastPointSize);
	GLfloat	lastLineWidth;
	glGetFloatv(GL_LINE_WIDTH, &lastLineWidth);
	GLboolean	rgba_mode;
	glGetBooleanv(GL_RGBA_MODE, &rgba_mode);
	GLfloat	rgba[4];
	glGetFloatv(GL_CURRENT_COLOR, rgba);
	const int	numEllipseSamplePoints = tentacleShape::numEllipseSamplePoints;
	glPointSize(6);
	glColor3f(1, 1, 0);
	glBegin(GL_POINTS);
	{
#if 1
		const MPoint&	p0 = strandSamplePoints[0];
#else
		const MPoint&	p0 = worldPts[0];
#endif
		glVertex3d(p0[0], p0[1], p0[2]);
	}
	glEnd();
#if 1
{
	glLineWidth(1);
	glColor3f(0, 0, 1);
	glBegin(GL_LINE_STRIP);
	for (int k = 0; k < (int) numStrandSamplePoints; ++k) {
#if 1
		const MPoint&	p = strandSamplePoints[k];
#else
		const MPoint&	p = worldPts[k];
#endif
		glVertex3d(p[0], p[1], p[2]);
	}
	glEnd();
}
#endif
#if 1
for (int vIndex = 0; vIndex < (int) numStrandSamplePoints; ++vIndex) {
#if 1
	const MPoint&	point = strandSamplePoints[vIndex];
#else
	const MPoint&	point = worldPts[vIndex];
	MVector	transVector;
	MVector	transVector0 = tentacle.transversal[vIndex];
	transVector = transVector0 * mInvT;
	transVector.normalize();
	int	index = vIndex + 1 == (int) numStrandSamplePoints ? vIndex : vIndex + 1;
	MVector	longiVector0 = tentacle.longitudinal[index - 1];
	MVector	longiVector = longiVector0 * mInvT;
	longiVector.normalize();
	double	a = tentacle.majorAxis[vIndex];
	double	eccentricity = tentacle.eccentricity[vIndex];
	if (eccentricity < 0)
		eccentricity = 0;
	if (eccentricity >= 1)
		eccentricity = 1 - 1e-7;
	double	e2 = eccentricity * eccentricity;
	double	f = a * sqrt(1 - e2);
	double	angle0 = tentacle.rotation[vIndex];
#endif
	if (vIndex != 0) {
		glPointSize(3);
		glColor3f(1, 1, 1);
		glBegin(GL_POINTS);
		{
			glVertex3d(point[0], point[1], point[2]);
		}
		glEnd();
	}
	if (vIndex == 0) {
		glPointSize(6);
		glColor3f(1, 0, 1);
	} else {
		glPointSize(5);
		glColor3f(1, 0, 0);
	}
	glBegin(GL_POINTS);
	{
		const MPoint&	p = tentacleSamplePoints[vIndex * numEllipseSamplePoints];
		glVertex3d(p[0], p[1], p[2]);
	}
	glEnd();
	if (vIndex == 0) {
		glPointSize(5);
		glColor3f(0, 1, 1);
	} else {
		glPointSize(4);
		glColor3f(0, 1, 0);
	}
	glBegin(GL_POINTS);
	{
		for (int n = 1; n < numEllipseSamplePoints; ++n) {
#if 1
			const MPoint&	p = tentacleSamplePoints[vIndex * numEllipseSamplePoints + n];
#else
			double	angle = 2 * M_PI * n / numEllipseSamplePoints;
			MQuaternion	q(angle0 + angle, longiVector);
			double	c = cos(angle);
			double	r = f / sqrt(1 - e2 * c * c);
			MVector	v = transVector.rotateBy(q);
			v *= r;
			MPoint	p = point + v;
#endif
			glVertex3d(p[0], p[1], p[2]);
		}
	}
	glEnd();
}
#endif
#if 1
do {
#if 1
	if (tentacle.cut_frame > 0)
		break;
#else
	if (!tentacle.have_cut)
		break;
#endif
	const kCut&	cut = *((kCut*) &tentacle);
	const int	numSamplePointsInCut = 5 * numEllipseSamplePoints;
	const double	unit_t = (double) 1 / (numStrandSamplePoints - 1);
	int	index = -1;
	for (int k = 0; k < (int) numStrandSamplePoints - 1; ++k) {
		if (k * unit_t <= cut.t && cut.t <= (k + 1) * unit_t) {
			index = k;
			break;
		}
	}
	if (index == -1)
		continue;
	double	local_t = (cut.t - index * unit_t) / unit_t;
#if 1
	MPoint	point;
	MVector	transVector;
	MVector longiVector;
#if 1
	point = strandSamplePoints[index] + local_t * (strandSamplePoints[index + 1] - strandSamplePoints[index]);
{
	MVector	longiVector0 = strandSamplePoints[index + 1] - strandSamplePoints[index];
	ms = longiVector0.normalize();
	MVector	longiVector1 = index + 1 == (int) numStrandSamplePoints ? longiVector0 : (strandSamplePoints[index + 1] - strandSamplePoints[index]);
	ms = longiVector1.normalize();
	longiVector = longiVector0 + local_t * longiVector1;
	longiVector.normalize();
	MVector	transVector0 = MVector(1, 0, 0).rotateBy(MQuaternion(MVector(0, 0, 1), longiVector0)).rotateBy(MQuaternion(tentacle.rotation[index], longiVector0));
	MVector	transVector1 = MVector(1, 0, 0).rotateBy(MQuaternion(MVector(0, 0, 1), longiVector1)).rotateBy(MQuaternion(tentacle.rotation[index + 1], longiVector1));
	transVector = transVector0 + local_t * transVector1;
	transVector.normalize();
}
#else
#if 1
{
	MVector	v = worldPts[index + 1] - worldPts[index];
	point = worldPts[index] + local_t * (worldPts[index + 1] - worldPts[index]);
}
#endif
#if 1
{
	MVector	v0 = tentacle.transversal[index] * mInvT;
	v0.normalize();
	MVector	v1 = tentacle.transversal[index + 1] * mInvT;
	v1.normalize();
	transVector = v0 + local_t * v1;
	transVector.normalize();
}
#endif
{
	MVector	longiVector0 = tentacle.longitudinal[index] + local_t * tentacle.longitudinal[index + 1 == (int) numStrandSamplePoints - 1 ? index : index + 1];
	longiVector = longiVector0 * mInvT;
	longiVector.normalize();
}
#endif
#endif
#if 1
	glPointSize(5);
	glColor3f(1, 0, 1);
	glBegin(GL_POINTS);
	{
		glVertex3d(point[0], point[1], point[2]);
	}
	glEnd();
#endif
#if 1
{
#if 1
	double	a = tentacle.majorAxis[0];
#else
	double	a = tentacle.majorAxis[index] + local_t * (tentacle.majorAxis[index + 1] - tentacle.majorAxis[index]);
#endif
	a *= 2;
#if 1
#else
	double	eccentricity = tentacle.eccentricity[index] + local_t * (tentacle.eccentricity[index + 1] - tentacle.eccentricity[index]);
	if (eccentricity < 0)
		eccentricity = 0;
	if (eccentricity >= 1)
		eccentricity = 1 - 1e-7;
	double	e2 = eccentricity * eccentricity;
	double	f = a * sqrt(1 - e2);
	double	angle0 = tentacle.rotation[index] + local_t * (tentacle.rotation[index + 1] - tentacle.rotation[index]);
#endif
	glPointSize(5);
	glColor3f(1, 1, 0);
	glBegin(GL_POINTS);
	{
#if 1
		MPoint	p = point + a * transVector;
		glVertex3d(p[0], p[1], p[2]);
#else
		MQuaternion	q(angle0, longiVector);
		MVector	v = transVector.rotateBy(q);
		v *= a;
		MPoint	p = point + v;
		glVertex3d(p[0], p[1], p[2]);
#endif
	}
	glEnd();
	glPointSize(4);
	glColor3f(0, 1, 1);
	glBegin(GL_POINTS);
	{
		for (int n = 1; n < numSamplePointsInCut; ++n) {
#if 1
			MQuaternion	q(2 * M_PI * n / numSamplePointsInCut, longiVector);
			MPoint	p = point + a * transVector.rotateBy(q);
			glVertex3d(p[0], p[1], p[2]);
#else
			double	angle = 2 * M_PI * n / numSamplePointsInCut;
			MQuaternion	q(angle0 + angle, longiVector);
			double	c = cos(angle);
			double	r = f / sqrt(1 - e2 * c * c);
			MVector	v = transVector.rotateBy(q);
			v *= r;
			MPoint	p = point + v;
			glVertex3d(p[0], p[1], p[2]);
#endif
		}
	}
	glEnd();
	glLineWidth(3);
	glBegin(GL_LINE_STRIP);
	glVertex3d(point[0], point[1], point[2]);
	MPoint	p = point + a * longiVector;
	glVertex3d(p[0], p[1], p[2]);
	glEnd();
}
#endif
#if 1
#else
#if 1
#if 1
	MVector	zAxis = longiVector;
	MVector	yAxis = zAxis ^ transVector;
	MQuaternion	q_polar(M_PI / 180 * cut.polar, yAxis);
	transVector = transVector.rotateBy(q_polar);
	longiVector = longiVector.rotateBy(q_polar);
	MQuaternion	q_azimuthal(M_PI / 180 * cut.azimuthal, zAxis);
	transVector = transVector.rotateBy(q_azimuthal);
	longiVector = longiVector.rotateBy(q_azimuthal);
#endif
#if 1
{
#if 1
	double	a = tentacle.majorAxis[0];
#else
	double	a = tentacle.majorAxis[index] + local_t * (tentacle.majorAxis[index + 1] - tentacle.majorAxis[index]);
#endif
	a *= 2;
#if 1
#else
	double	eccentricity = tentacle.eccentricity[index] + local_t * (tentacle.eccentricity[index + 1] - tentacle.eccentricity[index]);
	if (eccentricity < 0)
		eccentricity = 0;
	if (eccentricity >= 1)
		eccentricity = 1 - 1e-7;
	double	e2 = eccentricity * eccentricity;
	double	f = a * sqrt(1 - e2);
	double	angle0 = tentacle.rotation[index] + local_t * (tentacle.rotation[index + 1] - tentacle.rotation[index]);
#endif
	glPointSize(POINT_SIZE + 5);
	glColor3f(0, 1, 1);
	glBegin(GL_POINTS);
	{
#if 1
		MPoint	p = point + a * transVector;
		glVertex3d(p[0], p[1], p[2]);
#else
		MQuaternion	q(angle0, longiVector);
		MVector	v = transVector.rotateBy(q);
		v *= a;
		MPoint	p = point + v;
		glVertex3d(p[0], p[1], p[2]);
#endif
	}
	glEnd();
	glPointSize(POINT_SIZE + 1);
	glColor3f(1, 0, 1);
	glBegin(GL_POINTS);
	{
		for (int n = 1; n < numSamplePointsInCut; ++n) {
#if 1
			MQuaternion	q(2 * M_PI * n / numSamplePointsInCut, longiVector);
			MPoint	p = point + a * transVector.rotateBy(q);
			glVertex3d(p[0], p[1], p[2]);
#else
			double	angle = 2 * M_PI * n / numSamplePointsInCut;
			MQuaternion	q(angle0 + angle, longiVector);
			double	c = cos(angle);
			double	r = f / sqrt(1 - e2 * c * c);
			MVector	v = transVector.rotateBy(q);
			v *= r;
			MPoint	p = point + v;
			glVertex3d(p[0], p[1], p[2]);
#endif
		}
	}
	glEnd();
	glLineWidth(3);
	glBegin(GL_LINE_STRIP);
	glVertex3d(point[0], point[1], point[2]);
	MPoint	p = point + a * longiVector;
	glVertex3d(p[0], p[1], p[2]);
	glEnd();
}
#endif
#endif
#endif
} while (0);
#endif
	glPointSize(lastPointSize);
	glLineWidth(lastLineWidth);
	glColor3fv(rgba);
} while (0);
#pragma warning(default: 4127)
	view.endGL();
}
#else
void	tentacleShapeUI::DrawWakes(
	M3dView&	view,
	const MMatrix&	mInvT,
	const MPointArray&	worldPts,
	const kTentacle&	tentacle
)
{
	MStatus	ms;
	unsigned	numStrandSamplePoints = tentacle.transversal.length();
	view.beginGL();
#pragma warning(disable: 4127)
{
	float	lastPointSize;
	glGetFloatv(GL_POINT_SIZE, &lastPointSize);
	GLfloat	lastLineWidth;
	glGetFloatv(GL_LINE_WIDTH, &lastLineWidth);
	GLboolean	rgba_mode;
	glGetBooleanv(GL_RGBA_MODE, &rgba_mode);
	GLfloat	rgba[4];
	glGetFloatv(GL_CURRENT_COLOR, rgba);
	const int	numEllipseSamplePoints = tentacleShape::numEllipseSamplePoints;
#if 1
	MMatrix	worldMatrix = mInvT.transpose().inverse();
	MPointArray	strandSamplePoints;
	MPointArray	tentacleSamplePoints;
	tentacle.getSamplePoints(worldMatrix, strandSamplePoints, numEllipseSamplePoints, tentacleSamplePoints);
#endif
	glPointSize(6);
	glColor3f(1, 1, 0);
	glBegin(GL_POINTS);
	{
#if 1
		const MPoint&	p0 = strandSamplePoints[0];
#else
		const MPoint&	p0 = worldPts[0];
#endif
		glVertex3d(p0[0], p0[1], p0[2]);
	}
	glEnd();
#if 1
{
	glLineWidth(1);
	glColor3f(0, 0, 1);
	glBegin(GL_LINE_STRIP);
	for (int k = 0; k < (int) numStrandSamplePoints; ++k) {
#if 1
		const MPoint&	p = strandSamplePoints[k];
#else
		const MPoint&	p = worldPts[k];
#endif
		glVertex3d(p[0], p[1], p[2]);
	}
	glEnd();
}
#endif
#if 1
for (int vIndex = 0; vIndex < (int) numStrandSamplePoints; ++vIndex) {
#if 1
	const MPoint&	point = strandSamplePoints[vIndex];
#else
	const MPoint&	point = worldPts[vIndex];
	MVector	transVector;
	MVector	transVector0 = tentacle.transversal[vIndex];
	transVector = transVector0 * mInvT;
	transVector.normalize();
	int	index = vIndex + 1 == (int) numStrandSamplePoints ? vIndex : vIndex + 1;
	MVector	longiVector0 = tentacle.longitudinal[index - 1];
	MVector	longiVector = longiVector0 * mInvT;
	longiVector.normalize();
	double	a = tentacle.majorAxis[vIndex];
	double	eccentricity = tentacle.eccentricity[vIndex];
	if (eccentricity < 0)
		eccentricity = 0;
	if (eccentricity >= 1)
		eccentricity = 1 - 1e-7;
	double	e2 = eccentricity * eccentricity;
	double	f = a * sqrt(1 - e2);
	double	angle0 = tentacle.rotation[vIndex];
#endif
	if (vIndex != 0) {
		glPointSize(3);
		glColor3f(1, 1, 1);
		glBegin(GL_POINTS);
		{
			glVertex3d(point[0], point[1], point[2]);
		}
		glEnd();
	}
	if (vIndex == 0) {
		glPointSize(6);
		glColor3f(1, 0, 1);
	} else {
		glPointSize(5);
		glColor3f(1, 0, 0);
	}
	glBegin(GL_POINTS);
	{
#if 1
		MPoint&	p = tentacleSamplePoints[vIndex * numEllipseSamplePoints];
#else
		MQuaternion	q(angle0, longiVector);
		MVector	v = transVector.rotateBy(q);
		v *= a;
		MPoint	p = point + v;
#endif
		glVertex3d(p[0], p[1], p[2]);
	}
	glEnd();
	if (vIndex == 0) {
		glPointSize(5);
		glColor3f(0, 1, 1);
	} else {
		glPointSize(4);
		glColor3f(0, 1, 0);
	}
	glBegin(GL_POINTS);
	{
		for (int n = 1; n < numEllipseSamplePoints; ++n) {
#if 1
			MPoint&	p = tentacleSamplePoints[vIndex * numEllipseSamplePoints + n];
#else
			double	angle = 2 * M_PI * n / numEllipseSamplePoints;
			MQuaternion	q(angle0 + angle, longiVector);
			double	c = cos(angle);
			double	r = f / sqrt(1 - e2 * c * c);
			MVector	v = transVector.rotateBy(q);
			v *= r;
			MPoint	p = point + v;
#endif
			glVertex3d(p[0], p[1], p[2]);
		}
	}
	glEnd();
}
#endif
#if 1
do {
#if 1
	if (tentacle.cut_frame > 0)
		break;
#else
	if (!tentacle.have_cut)
		break;
#endif
	const kCut&	cut = *((kCut*) &tentacle);
	const int	numSamplePointsInCut = 5 * numEllipseSamplePoints;
	const double	unit_t = (double) 1 / (numStrandSamplePoints - 1);
	int	index = -1;
	for (int k = 0; k < (int) numStrandSamplePoints - 1; ++k) {
		if (k * unit_t <= cut.t && cut.t <= (k + 1) * unit_t) {
			index = k;
			break;
		}
	}
	if (index == -1)
		continue;
	double	local_t = (cut.t - index * unit_t) / unit_t;
#if 1
	MPoint	point;
	MVector	transVector;
	MVector longiVector;
#if 1
	point = worldPts[index] + local_t * (strandSamplePoints[index + 1] - strandSamplePoints[index]);
{
	MVector	longiVector0 = strandSamplePoints[index + 1] - strandSamplePoints[index];
	ms = longiVector0.normalize();
	MVector	longiVector1 = index + 1 == (int) numStrandSamplePoints ? longiVector0 : (strandSamplePoints[index + 1] - strandSamplePoints[index]);
	ms = longiVector1.normalize();
	longiVector = longiVector0 + local_t * longiVector1;
	longiVector.normalize();
	MVector	transVector0 = MVector(1, 0, 0).rotateBy(MQuaternion(MVector(0, 0, 1), longiVector0)).rotateBy(MQuaternion(tentacle.rotation[index], longiVector0));
	MVector	transVector1 = MVector(1, 0, 0).rotateBy(MQuaternion(MVector(0, 0, 1), longiVector1)).rotateBy(MQuaternion(tentacle.rotation[index + 1], longiVector1));
	transVector = transVector0 + local_t * transVector1;
	transVector.normalize();
}
#else
#if 1
{
	MVector	v = worldPts[index + 1] - worldPts[index];
	point = worldPts[index] + local_t * (worldPts[index + 1] - worldPts[index]);
}
#endif
#if 1
{
	MVector	v0 = tentacle.transversal[index] * mInvT;
	v0.normalize();
	MVector	v1 = tentacle.transversal[index + 1] * mInvT;
	v1.normalize();
	transVector = v0 + local_t * v1;
	transVector.normalize();
}
#endif
{
	MVector	longiVector0 = tentacle.longitudinal[index] + local_t * tentacle.longitudinal[index + 1 == (int) numStrandSamplePoints - 1 ? index : index + 1];
	longiVector = longiVector0 * mInvT;
	longiVector.normalize();
}
#endif
#endif
#if 1
	glPointSize(5);
	glColor3f(1, 0, 1);
	glBegin(GL_POINTS);
	{
		glVertex3d(point[0], point[1], point[2]);
	}
	glEnd();
#endif
#if 1
{
#if 1
	double	a = tentacle.majorAxis[0];
#else
	double	a = tentacle.majorAxis[index] + local_t * (tentacle.majorAxis[index + 1] - tentacle.majorAxis[index]);
#endif
	a *= 2;
#if 1
#else
	double	eccentricity = tentacle.eccentricity[index] + local_t * (tentacle.eccentricity[index + 1] - tentacle.eccentricity[index]);
	if (eccentricity < 0)
		eccentricity = 0;
	if (eccentricity >= 1)
		eccentricity = 1 - 1e-7;
	double	e2 = eccentricity * eccentricity;
	double	f = a * sqrt(1 - e2);
	double	angle0 = tentacle.rotation[index] + local_t * (tentacle.rotation[index + 1] - tentacle.rotation[index]);
#endif
	glPointSize(5);
	glColor3f(1, 1, 0);
	glBegin(GL_POINTS);
	{
#if 1
		MPoint	p = point + a * transVector;
		glVertex3d(p[0], p[1], p[2]);
#else
		MQuaternion	q(angle0, longiVector);
		MVector	v = transVector.rotateBy(q);
		v *= a;
		MPoint	p = point + v;
		glVertex3d(p[0], p[1], p[2]);
#endif
	}
	glEnd();
	glPointSize(4);
	glColor3f(0, 1, 1);
	glBegin(GL_POINTS);
	{
		for (int n = 1; n < numSamplePointsInCut; ++n) {
#if 1
			MQuaternion	q(2 * M_PI * n / numSamplePointsInCut, longiVector);
			MPoint	p = point + a * transVector.rotateBy(q);
			glVertex3d(p[0], p[1], p[2]);
#else
			double	angle = 2 * M_PI * n / numSamplePointsInCut;
			MQuaternion	q(angle0 + angle, longiVector);
			double	c = cos(angle);
			double	r = f / sqrt(1 - e2 * c * c);
			MVector	v = transVector.rotateBy(q);
			v *= r;
			MPoint	p = point + v;
			glVertex3d(p[0], p[1], p[2]);
#endif
		}
	}
	glEnd();
	glLineWidth(3);
	glBegin(GL_LINE_STRIP);
	glVertex3d(point[0], point[1], point[2]);
	MPoint	p = point + a * longiVector;
	glVertex3d(p[0], p[1], p[2]);
	glEnd();
}
#endif
#if 1
#else
#if 1
#if 1
	MVector	zAxis = longiVector;
	MVector	yAxis = zAxis ^ transVector;
	MQuaternion	q_polar(M_PI / 180 * cut.polar, yAxis);
	transVector = transVector.rotateBy(q_polar);
	longiVector = longiVector.rotateBy(q_polar);
	MQuaternion	q_azimuthal(M_PI / 180 * cut.azimuthal, zAxis);
	transVector = transVector.rotateBy(q_azimuthal);
	longiVector = longiVector.rotateBy(q_azimuthal);
#endif
#if 1
{
#if 1
	double	a = tentacle.majorAxis[0];
#else
	double	a = tentacle.majorAxis[index] + local_t * (tentacle.majorAxis[index + 1] - tentacle.majorAxis[index]);
#endif
	a *= 2;
#if 1
#else
	double	eccentricity = tentacle.eccentricity[index] + local_t * (tentacle.eccentricity[index + 1] - tentacle.eccentricity[index]);
	if (eccentricity < 0)
		eccentricity = 0;
	if (eccentricity >= 1)
		eccentricity = 1 - 1e-7;
	double	e2 = eccentricity * eccentricity;
	double	f = a * sqrt(1 - e2);
	double	angle0 = tentacle.rotation[index] + local_t * (tentacle.rotation[index + 1] - tentacle.rotation[index]);
#endif
	glPointSize(POINT_SIZE + 5);
	glColor3f(0, 1, 1);
	glBegin(GL_POINTS);
	{
#if 1
		MPoint	p = point + a * transVector;
		glVertex3d(p[0], p[1], p[2]);
#else
		MQuaternion	q(angle0, longiVector);
		MVector	v = transVector.rotateBy(q);
		v *= a;
		MPoint	p = point + v;
		glVertex3d(p[0], p[1], p[2]);
#endif
	}
	glEnd();
	glPointSize(POINT_SIZE + 1);
	glColor3f(1, 0, 1);
	glBegin(GL_POINTS);
	{
		for (int n = 1; n < numSamplePointsInCut; ++n) {
#if 1
			MQuaternion	q(2 * M_PI * n / numSamplePointsInCut, longiVector);
			MPoint	p = point + a * transVector.rotateBy(q);
			glVertex3d(p[0], p[1], p[2]);
#else
			double	angle = 2 * M_PI * n / numSamplePointsInCut;
			MQuaternion	q(angle0 + angle, longiVector);
			double	c = cos(angle);
			double	r = f / sqrt(1 - e2 * c * c);
			MVector	v = transVector.rotateBy(q);
			v *= r;
			MPoint	p = point + v;
			glVertex3d(p[0], p[1], p[2]);
#endif
		}
	}
	glEnd();
	glLineWidth(3);
	glBegin(GL_LINE_STRIP);
	glVertex3d(point[0], point[1], point[2]);
	MPoint	p = point + a * longiVector;
	glVertex3d(p[0], p[1], p[2]);
	glEnd();
}
#endif
#endif
#endif
} while (0);
#endif
	glPointSize(lastPointSize);
	glLineWidth(lastLineWidth);
	glColor3fv(rgba);
} while (0);
#pragma warning(default: 4127)
	view.endGL();
}
#endif
#endif
#else
#if 0
#if 1
	view.beginGL();
if (((tentacleShape*) surfaceShape())->displayWakes()) {
	MStatus	ms;
	const MMatrix&	m = ((tentacleShape*) surfaceShape())->externalInclusiveMatrix();
{
	float	lastPointSize;
	glGetFloatv(GL_POINT_SIZE, &lastPointSize);
	GLfloat	lastLineWidth;
	glGetFloatv(GL_LINE_WIDTH, &lastLineWidth);
	GLboolean	rgba_mode;
	glGetBooleanv(GL_RGBA_MODE, &rgba_mode);
	GLfloat	rgba[4];
	glGetFloatv(GL_CURRENT_COLOR, rgba);
	const int	numEllipseSamplePoints = 10;
for (int uIndex = 0; uIndex < (int) tentacleArray.length(); ++uIndex) {
	const kTentacle&	tentacle = tentacleArray(uIndex);
	MPoint	basePoint0(tentacle.root);
	glPointSize(6);
	glColor3f(1, 1, 0);
	glBegin(GL_POINTS);
	{
		MPoint	basePoint = basePoint0 * m;
		glVertex3d(basePoint[0], basePoint[1], basePoint[2]);
	}
	glEnd();
#if 1
{
	glLineWidth(1);
	glColor3f(0, 0, 1);
	glBegin(GL_LINE_STRIP);
	MPoint	basePoint = basePoint0 * m;
	MPoint	point = basePoint;
	for (int k = 0; k < (int) numStrandSamplePoints; ++k) {
		if (k) {
			MVector	offset = tentacle.longitudinal[k - 1];
			double	len = offset.length();
			offset = offset.transformAsNormal(m);
			point += len * offset;
		}
		glVertex3d(point[0], point[1], point[2]);
	}
	glEnd();
}
#endif
#if 1
for (int vIndex = 0; vIndex < (int) numStrandSamplePoints; ++vIndex) {
	MPoint	point = basePoint0;
	point *= m;
{
	int	k = 1;
	while (k <= vIndex) {
		MVector	offset = tentacle.longitudinal[k - 1];
		double	len = offset.length();
		offset = offset.transformAsNormal(m);
		point += len * offset;
		++k;
	}
}
	MVector	transVector;
{
	MVector	transVector0 = tentacle.transversal[vIndex];
	MPoint	point0 = basePoint0;
	int	k = 1;
	while (k <= vIndex) {
		MVector	offset0 = tentacle.longitudinal[k - 1];
		point0 += offset0;
		++k;
	}
	MPoint	p0 = point0 + transVector0;
	MPoint	point1 = point0;
	point1 *= m;
	MPoint	p1 = p0;
	p1 *= m;
	transVector = p1 - point1;
}
	int	index = vIndex + 1 == (int) numStrandSamplePoints ? vIndex : vIndex + 1;
	MVector	longiVector0 = tentacle.longitudinal[index - 1];
	MVector	longiVector = longiVector0.transformAsNormal(m);
	double	a = tentacle.majorAxis[vIndex];
	double	eccentricity = tentacle.eccentricity[vIndex];
	if (eccentricity < 0)
		eccentricity = 0;
	if (eccentricity >= 1)
		eccentricity = 1 - 1e-7;
	double	e2 = eccentricity * eccentricity;
	double	f = a * sqrt(1 - e2);
	double	angle0 = tentacle.rotation[vIndex];
	if (vIndex != 0) {
		glPointSize(3);
		glColor3f(1, 1, 1);
		glBegin(GL_POINTS);
		{
			glVertex3d(point[0], point[1], point[2]);
		}
		glEnd();
	}
	if (vIndex == 0) {
		glPointSize(6);
		glColor3f(1, 0, 1);
	} else {
		glPointSize(5);
		glColor3f(1, 0, 0);
	}
	glBegin(GL_POINTS);
	{
		MQuaternion	q(angle0, longiVector);
		MVector	v = transVector.rotateBy(q);
		v *= a;
		MPoint	p = point + v;
		glVertex3d(p[0], p[1], p[2]);
	}
	glEnd();
	if (vIndex == 0) {
		glPointSize(5);
		glColor3f(0, 1, 1);
	} else {
		glPointSize(4);
		glColor3f(0, 1, 0);
	}
	glBegin(GL_POINTS);
	{
		for (int n = 1; n < numEllipseSamplePoints; ++n) {
			double	angle = 2 * M_PI * n / numEllipseSamplePoints;
			MQuaternion	q(angle0 + angle, longiVector);
			double	c = cos(angle);
			double	r = f / sqrt(1 - e2 * c * c);
			MVector	v = transVector.rotateBy(q);
			v *= r;
			MPoint	p = point + v;
			glVertex3d(p[0], p[1], p[2]);
		}
	}
	glEnd();
}
#endif
#if 1
{
	const kCut&	cut = tentacle.cut;
	const int	numSamplePointsInCut = 5 * numEllipseSamplePoints;
	if (!cut.have)
		continue;
	const double	unit_t = (double) 1 / (numStrandSamplePoints - 1);
	int	index = -1;
	for (int k = 0; k < (int) numStrandSamplePoints - 1; ++k) {
		if (k * unit_t <= cut.t && cut.t <= (k + 1) * unit_t) {
			index = k;
			break;
		}
	}
	if (index == -1)
		continue;
	double	local_t = (cut.t - index * unit_t) / unit_t;
#if 1
	MPoint	point;
	MVector	transVector;
	MVector longiVector;
{
	MPoint	point1_0 = basePoint0;
{
	int	k = 1;
	while (k <= index) {
		MVector	offset0 = tentacle.longitudinal[k - 1];
		point1_0 += offset0;
		++k;
	}
}
	MPoint	point2_0 = basePoint0;
{
	int	k = 1;
	while (k <= index + 1) {
		MVector	offset0 = tentacle.longitudinal[k - 1];
		point2_0 += offset0;
		++k;
	}
}
	MPoint	point1 = point1_0 * m;
	MPoint	point2 = point2_0 * m;
	MVector	v(point2 - point1);
	point = point1 + local_t * v;
}
{
	MVector	transVector1_0 = tentacle.transversal[index];
	MVector	transVector1;
{
	MPoint	point0 = basePoint0;
	int	k = 1;
	while (k <= index) {
		MVector	offset0 = tentacle.longitudinal[k - 1];
		point0 += offset0;
		++k;
	}
	MPoint	p0 = point0 + transVector1_0;
	MPoint	point1 = point0;
	point1 *= m;
	MPoint	p1 = p0;
	p1 *= m;
	transVector1 = p1 - point1;
}
	MVector	transVector2_0 = tentacle.transversal[index + 1];
	MVector	transVector2;
{
	MPoint	point0 = basePoint0;
	int	k = 1;
	while (k <= index + 1) {
		MVector	offset0 = tentacle.longitudinal[k - 1];
		point0 += offset0;
		++k;
	}
	MPoint	p0 = point0 + transVector2_0;
	MPoint	point1 = point0;
	point1 *= m;
	MPoint	p1 = p0;
	p1 *= m;
	transVector2 = p1 - point1;
}
	transVector = transVector1 + local_t * transVector2;
	transVector.normalize();
}
{
	MVector	longiVector0 = tentacle.longitudinal[index] + local_t * tentacle.longitudinal[index + 1 == (int) numStrandSamplePoints - 1 ? index : index + 1];
	longiVector0.normalize();
	longiVector = longiVector0.transformAsNormal(m);
}
#endif
#if 1
	glPointSize(5);
	glColor3f(1, 0, 1);
	glBegin(GL_POINTS);
	{
		glVertex3d(point[0], point[1], point[2]);
	}
	glEnd();
#endif
#if 1
{
#if 1
	double	a = tentacle.majorAxis[0];
#else
	double	a = tentacle.majorAxis[index] + local_t * (tentacle.majorAxis[index + 1] - tentacle.majorAxis[index]);
#endif
	a *= 2;
#if 1
#else
	double	eccentricity = tentacle.eccentricity[index] + local_t * (tentacle.eccentricity[index + 1] - tentacle.eccentricity[index]);
	if (eccentricity < 0)
		eccentricity = 0;
	if (eccentricity >= 1)
		eccentricity = 1 - 1e-7;
	double	e2 = eccentricity * eccentricity;
	double	f = a * sqrt(1 - e2);
	double	angle0 = tentacle.rotation[index] + local_t * (tentacle.rotation[index + 1] - tentacle.rotation[index]);
#endif
	glPointSize(5);
	glColor3f(1, 1, 0);
	glBegin(GL_POINTS);
	{
#if 1
		MPoint	p = point + a * transVector;
		glVertex3d(p[0], p[1], p[2]);
#else
		MQuaternion	q(angle0, longiVector);
		MVector	v = transVector.rotateBy(q);
		v *= a;
		MPoint	p = point + v;
		glVertex3d(p[0], p[1], p[2]);
#endif
	}
	glEnd();
	glPointSize(4);
	glColor3f(0, 1, 1);
	glBegin(GL_POINTS);
	{
		for (int n = 1; n < numSamplePointsInCut; ++n) {
#if 1
			MQuaternion	q(2 * M_PI * n / numSamplePointsInCut, longiVector);
			MPoint	p = point + a * transVector.rotateBy(q);
			glVertex3d(p[0], p[1], p[2]);
#else
			double	angle = 2 * M_PI * n / numSamplePointsInCut;
			MQuaternion	q(angle0 + angle, longiVector);
			double	c = cos(angle);
			double	r = f / sqrt(1 - e2 * c * c);
			MVector	v = transVector.rotateBy(q);
			v *= r;
			MPoint	p = point + v;
			glVertex3d(p[0], p[1], p[2]);
#endif
		}
	}
	glEnd();
	glLineWidth(3);
	glBegin(GL_LINE_STRIP);
	glVertex3d(point[0], point[1], point[2]);
	MPoint	p = point + a * longiVector;
	glVertex3d(p[0], p[1], p[2]);
	glEnd();
}
#endif
#if 1
#if 1
	MVector	zAxis = longiVector;
	MVector	yAxis = zAxis ^ transVector;
	MQuaternion	q_polar(M_PI / 180 * cut.polar, yAxis);
	transVector = transVector.rotateBy(q_polar);
	longiVector = longiVector.rotateBy(q_polar);
	MQuaternion	q_azimuthal(M_PI / 180 * cut.azimuthal, zAxis);
	transVector = transVector.rotateBy(q_azimuthal);
	longiVector = longiVector.rotateBy(q_azimuthal);
#endif
#if 1
{
#if 1
	double	a = tentacle.majorAxis[0];
#else
	double	a = tentacle.majorAxis[index] + local_t * (tentacle.majorAxis[index + 1] - tentacle.majorAxis[index]);
#endif
	a *= 3;
#if 1
#else
	double	eccentricity = tentacle.eccentricity[index] + local_t * (tentacle.eccentricity[index + 1] - tentacle.eccentricity[index]);
	if (eccentricity < 0)
		eccentricity = 0;
	if (eccentricity >= 1)
		eccentricity = 1 - 1e-7;
	double	e2 = eccentricity * eccentricity;
	double	f = a * sqrt(1 - e2);
	double	angle0 = tentacle.rotation[index] + local_t * (tentacle.rotation[index + 1] - tentacle.rotation[index]);
#endif
	glPointSize(POINT_SIZE + 5);
	glColor3f(0, 1, 1);
	glBegin(GL_POINTS);
	{
#if 1
		MPoint	p = point + a * transVector;
		glVertex3d(p[0], p[1], p[2]);
#else
		MQuaternion	q(angle0, longiVector);
		MVector	v = transVector.rotateBy(q);
		v *= a;
		MPoint	p = point + v;
		glVertex3d(p[0], p[1], p[2]);
#endif
	}
	glEnd();
	glPointSize(POINT_SIZE + 1);
	glColor3f(1, 0, 1);
	glBegin(GL_POINTS);
	{
		for (int n = 1; n < numSamplePointsInCut; ++n) {
#if 1
			MQuaternion	q(2 * M_PI * n / numSamplePointsInCut, longiVector);
			MPoint	p = point + a * transVector.rotateBy(q);
			glVertex3d(p[0], p[1], p[2]);
#else
			double	angle = 2 * M_PI * n / numSamplePointsInCut;
			MQuaternion	q(angle0 + angle, longiVector);
			double	c = cos(angle);
			double	r = f / sqrt(1 - e2 * c * c);
			MVector	v = transVector.rotateBy(q);
			v *= r;
			MPoint	p = point + v;
			glVertex3d(p[0], p[1], p[2]);
#endif
		}
	}
	glEnd();
	glLineWidth(3);
	glBegin(GL_LINE_STRIP);
	glVertex3d(point[0], point[1], point[2]);
	MPoint	p = point + a * longiVector;
	glVertex3d(p[0], p[1], p[2]);
	glEnd();
}
#endif
#endif
}
#endif
}
	glPointSize(lastPointSize);
	glLineWidth(lastLineWidth);
	glColor3fv(rgba);
}
}
	view.endGL();
#endif
#endif
#endif
#if 1
#if 1
void	tentacleShapeUI::DrawRampInViewPort(
	M3dView&	view,
	const kPtrArray<RampTuple>&	rampTupleArray
)
{
	MStatus	ms;
	view.beginGL();
#pragma warning(disable: 4127)
{
	float	lastPointSize;
	glGetFloatv(GL_POINT_SIZE, &lastPointSize);
	GLboolean	rgba_mode;
	glGetBooleanv(GL_RGBA_MODE, &rgba_mode);
	GLfloat	rgba[4];
	glGetFloatv(GL_CURRENT_COLOR, rgba);
#if 1
	GLenum	matrix_mode;
	glGetIntegerv(GL_MATRIX_MODE, (GLint*) &matrix_mode);
	glMatrixMode (matrix_mode);
	glPushMatrix();
#endif
do {
	const int	numControlPoints = (int) rampTupleArray.length();
#if 0
	for (int i = 0; i < numControlPoints; ++i) {
		const RampTuple&	rampTuple = rampTupleArray(i);
		cout << "Rationalized_Sorted_RampTuple[" << i << "]:" << endl;
		cout << "\tPosition: " << rampTuple.Position << endl;
		cout << "\tFloatValue: " << rampTuple.FloatValue << endl;
		cout << "\tInterp: " << rampTuple.InterpType << endl;
	}
#endif
#if 1
	MStatus	ms;
	int	H = view.portHeight(&ms);
	int	W = view.portWidth(&ms);
	const float	golden_ratio = (1 + sqrtf(5.f)) / 2;
	int	Lx, Ly;
	int	dL = 10;
	Ly = H / 2;
	Lx = (int) (Ly * golden_ratio);
	if (Lx + 2 * dL > W) {
		Lx = W - 2 * dL;
		Ly = (int) (Lx / golden_ratio);
	}
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, (GLdouble) W, 0.0, (GLdouble) H);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glBegin(GL_LINE_LOOP);
	glVertex2i(dL + 0, dL + 0);
	glVertex2i(dL + Lx * 1, dL + 0);
	glVertex2i(dL + Lx * 1, dL + Ly * 1);
	glVertex2i(dL + 0, dL + Ly * 1);
	glEnd();
#endif
#if 0
	float	xScale = (1 + sqrtf(5.f)) / 2;
	float	yScale = 1.f;
	MDagPath	cameraDagPath;
	view.getCamera(cameraDagPath);
	MFnCamera	fnCamera(cameraDagPath);
	MVector		xDir = fnCamera.rightDirection(MSpace::kWorld, &ms);
	xDir.normalize();
	xDir *= xScale;
	MVector		yDir = fnCamera.upDirection(MSpace::kWorld, &ms);
	yDir.normalize();
	yDir *= yScale;
	MVector		viewDir = fnCamera.viewDirection(MSpace::kWorld, &ms);
	MPoint		eyePoint = fnCamera.eyePoint(MSpace::kWorld, &ms);
	MVector		v0(eyePoint);
	double	dot = v0 * viewDir;
	v0 = v0 - dot * viewDir;
	const float	fScale0 = .1f;
	float	fScale = fScale0;
	do {
		MVector	v00 = (0 - .5) * xDir + (0 - .5) * yDir; v00 += v0;
		short	left_x, lower_y;
		if (!view.worldToView(v00, left_x, lower_y, &ms) || !ms) {
			MGlobal::displayWarning("M3dView::worldToView return False");
			fScale = fScale0;
			break;
		}
		short	right_x, upper_y;
		MVector	v11 = (1 - .5) * xDir + (1 - .5) * yDir; v11 += v0;
		if (!view.worldToView(v11, right_x, upper_y, &ms) || !ms) {
			MGlobal::displayWarning("M3dView::worldToView return False");
			fScale = fScale0;
			break;
		}
		int	w = right_x - left_x;
		int	h = upper_y - lower_y;
		int	H = view.portHeight(&ms);
		int	W = view.portWidth(&ms);
		fScale = (W / 2) / (float) w;
		if (w * fScale / xScale > H) {
			fScale = H / (float) h;
			yScale /= xScale;
			xScale = 1.f;
		}
	} while (0);
	glBegin(GL_LINE_LOOP);
	MVector	v00 = (0 - .5) * xDir + (0 - .5) * yDir; v00 *= fScale; v00 += v0;
	glVertex3d(v00.x, v00.y, v00.z);
	MVector	v10 = (1 - .5) * xDir + (0 - .5) * yDir; v10 *= fScale; v10 += v0;
	glVertex3d(v10.x, v10.y, v10.z);
	MVector	v11 = (1 - .5) * xDir + (1 - .5) * yDir; v11 *= fScale; v11 += v0;
	glVertex3d(v11.x, v11.y, v11.z);
	MVector	v01 = (0 - .5) * xDir + (1 - .5) * yDir; v01 *= fScale; v01 += v0;
	glVertex3d(v01.x, v01.y, v01.z);
	glEnd();
#endif
#if 1
	glPointSize(3);
	glColor3f(1, 0, 1);
	glBegin(GL_POINTS);
	for (int i = 0; i < numControlPoints; ++i) {
		const RampTuple&	rampTuple = rampTupleArray(i);
#if 1
		glVertex2f(dL + Lx * rampTuple.Position, dL + Ly * rampTuple.FloatValue);
#endif
#if 0
		MVector	v = (rampTuple.Position - .5) * xDir + (rampTuple.FloatValue - .5) * yDir;
		v *= fScale;
		v += v0;
		glVertex3d(v.x, v.y, v.z);
#endif
	}
	glEnd();
#endif
#if 1
#if 1
{
	const float	dT0 = .003f;
	glPointSize(2);
	glColor3f(0, 0, 1);
	glBegin(GL_POINTS);
#if 1
	float	x = 0;
	do {
		float	y = -1;
		(void) tentacleShape::GetInterpolatedValue(rampTupleArray, x, y, true);
#if 1
		glVertex2f(dL + Lx * x, dL + Ly * y);
#endif
#if 0
		MVector	v = (x - .5) * xDir + (y - .5) * yDir;
		v *= fScale;
		v += v0;
		glVertex3d(v.x, v.y, v.z);
#endif
		x += dT0;
	} while (x <= 1.f);
#else
	for (int k = 0; k < numControlPoints - 1; ++k) {
		const RampTuple&	left = rampTupleArray(k);
		const RampTuple&	right = rampTupleArray(k + 1);
		const float	x1 = left.Position;
		const float	x2 = right.Position;
		float	y = -1;
		float	x = x1;
		while (x <= x2) {
			const float	y1 = left.FloatValue;
			const float	y2 = right.FloatValue;
			switch (left.InterpType) {
			case INTERP::None:
				y = y1;
				break;
			case INTERP::Linear:
				if (x1 == x2)
					y = y1;
				else
					y = y1 + (x - x1) * (y2 - y1) / (x2 - x1);
				break;
			case INTERP::Smooth:
				if (x1 == x2)
					y = y1;
				else {
					float	s = (x - x1) / (x2 - x1);
					float	dy = y2 - y1;
					y = y1 + s * s * (3 * dy - 2 * dy * s);
				}
				break;
			case INTERP::Spline:
				if (x1 == x2)
					y = y1;
				else {
					assert(k > 0);
					const float	p1 = rampTupleArray(k - 1).FloatValue;
					assert(k + 2 < numControlPoints);
					const float	p4 = rampTupleArray(k + 2).FloatValue;
					const float	p2 = y1;
					const float	p3 = y2;
					float	s = (x - x1) / (x2 - x1);
					float	s2 = s * s;
					float	s3 = s2 * s;
					y = ((-s3 + 2 * s2 - s) * p1 + (3 * s3 - 5 * s2 + 2) * p2 + (-3 * s3 + 4 * s2 + s) * p3 + (s3 - s2) * p4) / 2;
				}
				break;
			default:
				cerr << "Invalid INTERP::Type: " << left.InterpType << endl;
				y = 0;
				break;
			}
#if 1
			glVertex2f(dL + Lx * x, dL + Ly * y);
#endif
#if 0
			MVector	v = (x - .5) * xDir + (y - .5) * yDir;
			v *= fScale;
			v += v0;
			glVertex3d(v.x, v.y, v.z);
#endif
			x += dT0;
		}
	}
#endif
	glEnd();
}
#endif
#if 0
{
	const int	numInterpolatedPoints = 100;
	glPointSize(2);
	glColor3f(0, 1, 0);
	glBegin(GL_POINTS);
	for (int i = 0; i <= numInterpolatedPoints; ++i) {
		float	x = 1.f / numInterpolatedPoints * i;
		if (x < 0.f)
			x = 0.f;
		if (x > 1.f)
			x = 1.f;
		float	y = -1.f;
#if 1
		tentacleShape::GetInterpolatedValue(rampTupleArray, x, y, false);
#else
		for (int k = 0; k < numControlPoints - 1; ++k) {
			const RampTuple&	left = rampTupleArray(k);
			const RampTuple&	right = rampTupleArray(k + 1);
			const float	x1 = left.Position;
			const float	x2 = right.Position;
			if (x1 <= x && x <= x2) {
				const float	y1 = left.FloatValue;
				const float	y2 = right.FloatValue;
				switch (left.InterpType) {
				case INTERP::None:
					y = y1;
					break;
				case INTERP::Linear:
					if (x1 == x2)
						y = y1;
					else
						y = y1 + (x - x1) * (y2 - y1) / (x2 - x1);
					if (y < 0.f)
						y = 0.f;
					if (y > 1.f)
						y = 1.f;
					break;
				case INTERP::Smooth:
					if (x1 == x2)
						y = y1;
					else {
						float	s = (x - x1) / (x2 - x1);
						float	dy = y2 - y1;
						y = y1 + s * s * (3 * dy - 2 * dy * s);
					}
					if (y < 0.f)
						y = 0.f;
					if (y > 1.f)
						y = 1.f;
					break;
				case INTERP::Spline:
					if (x1 == x2)
						y = y1;
					else {
						assert(k > 0);
						const float	p1 = rampTupleArray(k - 1).FloatValue;
						assert(k + 2 < numControlPoints);
						const float	p4 = rampTupleArray(k + 2).FloatValue;
						const float	p2 = y1;
						const float	p3 = y2;
						float	s = (x - x1) / (x2 - x1);
						float	s2 = s * s;
						float	s3 = s2 * s;
y = ((-s3 + 2 * s2 - s) * p1 + (3 * s3 - 5 * s2 + 2) * p2 + (-3 * s3 + 4 * s2 + s) * p3 + (s3 - s2) * p4) / 2;
					}
					if (y < 0.f)
						y = 0.f;
					if (y > 1.f)
						y = 1.f;
					break;
				default:
					cerr << "Invalid INTERP::Type: " << left.InterpType << endl;
					y = 0.f;
					break;
				}
				break;
			}
		}
#endif
#if 1
		glVertex2f(dL + Lx * x, dL + Ly * y);
#endif
#if 0
		MVector	v = (x - .5) * xDir + (y - .5) * yDir;
		v *= fScale;
		v += v0;
		glVertex3d(v.x, v.y, v.z);
#endif
	}
	glEnd();
}
#endif
#endif
} while(0);
#if 1
	glMatrixMode(matrix_mode);
	glPopMatrix();
#endif
	glPointSize(lastPointSize);
	glColor3fv(rgba);
} while (0);
#pragma warning(default: 4127)
	view.endGL();
}
void	tentacleShapeUI::DrawRampInCameraFront(
	M3dView&	view,
	const kPtrArray<RampTuple>&	rampTupleArray
)
{
	MStatus	ms;
	view.beginGL();
#pragma warning(disable: 4127)
{
	float	lastPointSize;
	glGetFloatv(GL_POINT_SIZE, &lastPointSize);
	GLboolean	rgba_mode;
	glGetBooleanv(GL_RGBA_MODE, &rgba_mode);
	GLfloat	rgba[4];
	glGetFloatv(GL_CURRENT_COLOR, rgba);
#if 0
	GLenum	matrix_mode;
	glGetIntegerv(GL_MATRIX_MODE, (GLint*) &matrix_mode);
	glMatrixMode (matrix_mode);
	glPushMatrix();
#endif
do {
	const int	numControlPoints = (int) rampTupleArray.length();
#if 0
	for (int i = 0; i < numControlPoints; ++i) {
		const RampTuple&	rampTuple = rampTupleArray(i);
		cout << "Rationalized_Sorted_RampTuple[" << i << "]:" << endl;
		cout << "\tPosition: " << rampTuple.Position << endl;
		cout << "\tFloatValue: " << rampTuple.FloatValue << endl;
		cout << "\tInterp: " << rampTuple.InterpType << endl;
	}
#endif
#if 0
	MStatus	ms;
	int	H = view.portHeight(&ms);
	int	W = view.portWidth(&ms);
	const float	golden_ratio = (1 + sqrtf(5.f)) / 2;
	int	Lx, Ly;
	int	dL = 10;
	Ly = H / 2;
	Lx = (int) (Ly * golden_ratio);
	if (Lx + 2 * dL > W) {
		Lx = W - 2 * dL;
		Ly = (int) (Lx / golden_ratio);
	}
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, (GLdouble) W, 0.0, (GLdouble) H);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glBegin(GL_LINE_LOOP);
	glVertex2i(dL + 0, dL + 0);
	glVertex2i(dL + Lx * 1, dL + 0);
	glVertex2i(dL + Lx * 1, dL + Ly * 1);
	glVertex2i(dL + 0, dL + Ly * 1);
	glEnd();
#endif
#if 1
	float	xScale = (1 + sqrtf(5.f)) / 2;
	float	yScale = 1.f;
	MDagPath	cameraDagPath;
	view.getCamera(cameraDagPath);
	MFnCamera	fnCamera(cameraDagPath);
	MVector		xDir = fnCamera.rightDirection(MSpace::kWorld, &ms);
	xDir.normalize();
	xDir *= xScale;
	MVector		yDir = fnCamera.upDirection(MSpace::kWorld, &ms);
	yDir.normalize();
	yDir *= yScale;
	MVector		viewDir = fnCamera.viewDirection(MSpace::kWorld, &ms);
	MPoint		eyePoint = fnCamera.eyePoint(MSpace::kWorld, &ms);
	MVector		v0(eyePoint);
	double	dot = v0 * viewDir;
	v0 = v0 - dot * viewDir;
	const float	fScale0 = .1f;
	float	fScale = fScale0;
	do {
		MVector	v00 = (0 - .5) * xDir + (0 - .5) * yDir; v00 += v0;
		short	left_x, lower_y;
		if (!view.worldToView(v00, left_x, lower_y, &ms) || !ms) {
			MGlobal::displayWarning("M3dView::worldToView return False");
			fScale = fScale0;
			break;
		}
		short	right_x, upper_y;
		MVector	v11 = (1 - .5) * xDir + (1 - .5) * yDir; v11 += v0;
		if (!view.worldToView(v11, right_x, upper_y, &ms) || !ms) {
			MGlobal::displayWarning("M3dView::worldToView return False");
			fScale = fScale0;
			break;
		}
		int	w = right_x - left_x;
		int	h = upper_y - lower_y;
		int	H = view.portHeight(&ms);
		int	W = view.portWidth(&ms);
		fScale = (W / 2) / (float) w;
		if (w * fScale / xScale > H) {
			fScale = H / (float) h;
			yScale /= xScale;
			xScale = 1.f;
		}
	} while (0);
	glBegin(GL_LINE_LOOP);
	MVector	v00 = (0 - .5) * xDir + (0 - .5) * yDir; v00 *= fScale; v00 += v0;
	glVertex3d(v00.x, v00.y, v00.z);
	MVector	v10 = (1 - .5) * xDir + (0 - .5) * yDir; v10 *= fScale; v10 += v0;
	glVertex3d(v10.x, v10.y, v10.z);
	MVector	v11 = (1 - .5) * xDir + (1 - .5) * yDir; v11 *= fScale; v11 += v0;
	glVertex3d(v11.x, v11.y, v11.z);
	MVector	v01 = (0 - .5) * xDir + (1 - .5) * yDir; v01 *= fScale; v01 += v0;
	glVertex3d(v01.x, v01.y, v01.z);
	glEnd();
#endif
#if 1
	glPointSize(3);
	glColor3f(1, 0, 1);
	glBegin(GL_POINTS);
	for (int i = 0; i < numControlPoints; ++i) {
		const RampTuple&	rampTuple = rampTupleArray(i);
#if 0
		glVertex2f(dL + Lx * rampTuple.Position, dL + Ly * rampTuple.FloatValue);
#endif
#if 1
		MVector	v = (rampTuple.Position - .5) * xDir + (rampTuple.FloatValue - .5) * yDir;
		v *= fScale;
		v += v0;
		glVertex3d(v.x, v.y, v.z);
#endif
	}
	glEnd();
#endif
#if 1
#if 1
{
	const float	dT0 = .003f;
	glPointSize(2);
	glColor3f(0, 0, 1);
	glBegin(GL_POINTS);
#if 1
	float	x = 0;
	do {
		float	y = -1;
		(void) tentacleShape::GetInterpolatedValue(rampTupleArray, x, y, true);
#if 0
		glVertex2f(dL + Lx * x, dL + Ly * y);
#endif
#if 1
		MVector	v = (x - .5) * xDir + (y - .5) * yDir;
		v *= fScale;
		v += v0;
		glVertex3d(v.x, v.y, v.z);
#endif
		x += dT0;
	} while (x <= 1.f);
#else
	for (int k = 0; k < numControlPoints - 1; ++k) {
		const RampTuple&	left = rampTupleArray(k);
		const RampTuple&	right = rampTupleArray(k + 1);
		const float	x1 = left.Position;
		const float	x2 = right.Position;
		float	y = -1;
		float	x = x1;
		while (x <= x2) {
			const float	y1 = left.FloatValue;
			const float	y2 = right.FloatValue;
			switch (left.InterpType) {
			case INTERP::None:
				y = y1;
				break;
			case INTERP::Linear:
				if (x1 == x2)
					y = y1;
				else
					y = y1 + (x - x1) * (y2 - y1) / (x2 - x1);
				break;
			case INTERP::Smooth:
				if (x1 == x2)
					y = y1;
				else {
					float	s = (x - x1) / (x2 - x1);
					float	dy = y2 - y1;
					y = y1 + s * s * (3 * dy - 2 * dy * s);
				}
				break;
			case INTERP::Spline:
				if (x1 == x2)
					y = y1;
				else {
					assert(k > 0);
					const float	p1 = rampTupleArray(k - 1).FloatValue;
					assert(k + 2 < numControlPoints);
					const float	p4 = rampTupleArray(k + 2).FloatValue;
					const float	p2 = y1;
					const float	p3 = y2;
					float	s = (x - x1) / (x2 - x1);
					float	s2 = s * s;
					float	s3 = s2 * s;
					y = ((-s3 + 2 * s2 - s) * p1 + (3 * s3 - 5 * s2 + 2) * p2 + (-3 * s3 + 4 * s2 + s) * p3 + (s3 - s2) * p4) / 2;
				}
				break;
			default:
				cerr << "Invalid INTERP::Type: " << left.InterpType << endl;
				y = 0;
				break;
			}
#if 0
			glVertex2f(dL + Lx * x, dL + Ly * y);
#endif
#if 1
			MVector	v = (x - .5) * xDir + (y - .5) * yDir;
			v *= fScale;
			v += v0;
			glVertex3d(v.x, v.y, v.z);
#endif
			x += dT0;
		}
	}
#endif
	glEnd();
}
#endif
#if 0
{
	const int	numInterpolatedPoints = 100;
	glPointSize(2);
	glColor3f(0, 1, 0);
	glBegin(GL_POINTS);
	for (int i = 0; i <= numInterpolatedPoints; ++i) {
		float	x = 1.f / numInterpolatedPoints * i;
		if (x < 0.f)
			x = 0.f;
		if (x > 1.f)
			x = 1.f;
		float	y = -1.f;
#if 1
		tentacleShape::GetInterpolatedValue(rampTupleArray, x, y, false);
#else
		for (int k = 0; k < numControlPoints - 1; ++k) {
			const RampTuple&	left = rampTupleArray(k);
			const RampTuple&	right = rampTupleArray(k + 1);
			const float	x1 = left.Position;
			const float	x2 = right.Position;
			if (x1 <= x && x <= x2) {
				const float	y1 = left.FloatValue;
				const float	y2 = right.FloatValue;
				switch (left.InterpType) {
				case INTERP::None:
					y = y1;
					break;
				case INTERP::Linear:
					if (x1 == x2)
						y = y1;
					else
						y = y1 + (x - x1) * (y2 - y1) / (x2 - x1);
					if (y < 0.f)
						y = 0.f;
					if (y > 1.f)
						y = 1.f;
					break;
				case INTERP::Smooth:
					if (x1 == x2)
						y = y1;
					else {
						float	s = (x - x1) / (x2 - x1);
						float	dy = y2 - y1;
						y = y1 + s * s * (3 * dy - 2 * dy * s);
					}
					if (y < 0.f)
						y = 0.f;
					if (y > 1.f)
						y = 1.f;
					break;
				case INTERP::Spline:
					if (x1 == x2)
						y = y1;
					else {
						assert(k > 0);
						const float	p1 = rampTupleArray(k - 1).FloatValue;
						assert(k + 2 < numControlPoints);
						const float	p4 = rampTupleArray(k + 2).FloatValue;
						const float	p2 = y1;
						const float	p3 = y2;
						float	s = (x - x1) / (x2 - x1);
						float	s2 = s * s;
						float	s3 = s2 * s;
y = ((-s3 + 2 * s2 - s) * p1 + (3 * s3 - 5 * s2 + 2) * p2 + (-3 * s3 + 4 * s2 + s) * p3 + (s3 - s2) * p4) / 2;
					}
					if (y < 0.f)
						y = 0.f;
					if (y > 1.f)
						y = 1.f;
					break;
				default:
					cerr << "Invalid INTERP::Type: " << left.InterpType << endl;
					y = 0.f;
					break;
				}
				break;
			}
		}
#endif
#if 0
		glVertex2f(dL + Lx * x, dL + Ly * y);
#endif
#if 1
		MVector	v = (x - .5) * xDir + (y - .5) * yDir;
		v *= fScale;
		v += v0;
		glVertex3d(v.x, v.y, v.z);
#endif
	}
	glEnd();
}
#endif
#endif
} while(0);
#if 0
	glMatrixMode(matrix_mode);
	glPopMatrix();
#endif
	glPointSize(lastPointSize);
	glColor3fv(rgba);
} while (0);
#pragma warning(default: 4127)
	view.endGL();
}
#else
#if 0
#if 1
	view.beginGL();
	const kPtrArray<RampTuple>*	rampTupleArrayPtr = ((tentacleShape*) surfaceShape())->GetRampTupleArrayPtr();
if (rampTupleArrayPtr) {
#define VIEW_PORT
#define CAMERA_FRONT
	const kPtrArray<RampTuple>&	rampTupleArray = *rampTupleArrayPtr;
	MStatus	ms;
	float	lastPointSize;
	glGetFloatv(GL_POINT_SIZE, &lastPointSize);
	GLboolean	rgba_mode;
	glGetBooleanv(GL_RGBA_MODE, &rgba_mode);
	GLfloat	rgba[4];
	glGetFloatv(GL_CURRENT_COLOR, rgba);
#ifdef VIEW_PORT
	GLenum	matrix_mode;
	glGetIntegerv(GL_MATRIX_MODE, (GLint*) &matrix_mode);
	glMatrixMode (matrix_mode);
	glPushMatrix();
#endif
#pragma warning(disable: 4127)
do {
	const int	numControlPoints = (int) rampTupleArray.length();
#if 0
	for (int i = 0; i < numControlPoints; ++i) {
		const RampTuple&	rampTuple = rampTupleArray(i);
		cout << "Rationalized_Sorted_RampTuple[" << i << "]:" << endl;
		cout << "\tPosition: " << rampTuple.Position << endl;
		cout << "\tFloatValue: " << rampTuple.FloatValue << endl;
		cout << "\tInterp: " << rampTuple.InterpType << endl;
	}
#endif
#ifdef VIEW_PORT
	MStatus	ms;
	int	H = view.portHeight(&ms);
	int	W = view.portWidth(&ms);
	const float	golden_ratio = (1 + sqrtf(5.f)) / 2;
	int	Lx, Ly;
	int	dL = 10;
	Ly = H / 2;
	Lx = (int) (Ly * golden_ratio);
	if (Lx + 2 * dL > W) {
		Lx = W - 2 * dL;
		Ly = (int) (Lx / golden_ratio);
	}
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, (GLdouble) W, 0.0, (GLdouble) H);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glBegin(GL_LINE_LOOP);
	glVertex2i(dL + 0, dL + 0);
	glVertex2i(dL + Lx * 1, dL + 0);
	glVertex2i(dL + Lx * 1, dL + Ly * 1);
	glVertex2i(dL + 0, dL + Ly * 1);
	glEnd();
#elif defined(CAMERA_FRONT)
	float	xScale = (1 + sqrtf(5.f)) / 2;
	float	yScale = 1.f;
	MDagPath	cameraDagPath;
	view.getCamera(cameraDagPath);
	MFnCamera	fnCamera(cameraDagPath);
	MVector		xDir = fnCamera.rightDirection(MSpace::kWorld, &ms);
	xDir.normalize();
	xDir *= xScale;
	MVector		yDir = fnCamera.upDirection(MSpace::kWorld, &ms);
	yDir.normalize();
	yDir *= yScale;
	MVector		viewDir = fnCamera.viewDirection(MSpace::kWorld, &ms);
	MPoint		eyePoint = fnCamera.eyePoint(MSpace::kWorld, &ms);
	MVector		v0(eyePoint);
	double	dot = v0 * viewDir;
	v0 = v0 - dot * viewDir;
	const float	fScale0 = .1f;
	float	fScale = fScale0;
	do {
		MVector	v00 = (0 - .5) * xDir + (0 - .5) * yDir; v00 += v0;
		short	left_x, lower_y;
		if (!view.worldToView(v00, left_x, lower_y, &ms) || !ms) {
			MGlobal::displayWarning("M3dView::worldToView return False");
			fScale = fScale0;
			break;
		}
		short	right_x, upper_y;
		MVector	v11 = (1 - .5) * xDir + (1 - .5) * yDir; v11 += v0;
		if (!view.worldToView(v11, right_x, upper_y, &ms) || !ms) {
			MGlobal::displayWarning("M3dView::worldToView return False");
			fScale = fScale0;
			break;
		}
		int	w = right_x - left_x;
		int	h = upper_y - lower_y;
		int	H = view.portHeight(&ms);
		int	W = view.portWidth(&ms);
		fScale = (W / 2) / (float) w;
		if (w * fScale / xScale > H) {
			fScale = H / (float) h;
			yScale /= xScale;
			xScale = 1.f;
		}
	} while (0);
	glBegin(GL_LINE_LOOP);
	MVector	v00 = (0 - .5) * xDir + (0 - .5) * yDir; v00 *= fScale; v00 += v0;
	glVertex3d(v00.x, v00.y, v00.z);
	MVector	v10 = (1 - .5) * xDir + (0 - .5) * yDir; v10 *= fScale; v10 += v0;
	glVertex3d(v10.x, v10.y, v10.z);
	MVector	v11 = (1 - .5) * xDir + (1 - .5) * yDir; v11 *= fScale; v11 += v0;
	glVertex3d(v11.x, v11.y, v11.z);
	MVector	v01 = (0 - .5) * xDir + (1 - .5) * yDir; v01 *= fScale; v01 += v0;
	glVertex3d(v01.x, v01.y, v01.z);
	glEnd();
#endif
#if 1
	glPointSize(3);
	glColor3f(1, 0, 1);
	glBegin(GL_POINTS);
	for (int i = 0; i < numControlPoints; ++i) {
		const RampTuple&	rampTuple = rampTupleArray(i);
#ifdef VIEW_PORT
		glVertex2f(dL + Lx * rampTuple.Position, dL + Ly * rampTuple.FloatValue);
#elif defined(CAMERA_FRONT)
		MVector	v = (rampTuple.Position - .5) * xDir + (rampTuple.FloatValue - .5) * yDir;
		v *= fScale;
		v += v0;
		glVertex3d(v.x, v.y, v.z);
#else
		glVertex2f(rampTuple.Position, rampTuple.FloatValue);
#endif
	}
	glEnd();
#endif
#if 1
#if 1
{
	const float	dT0 = .003f;
	glPointSize(2);
	glColor3f(0, 0, 1);
	glBegin(GL_POINTS);
#if 1
	float	x = 0;
	do {
		float	y = -1;
		assert(tentacleShape::GetInterpolatedValue(rampTupleArray, x, y, false) == 0);
#ifdef VIEW_PORT
		glVertex2f(dL + Lx * x, dL + Ly * y);
#elif defined(CAMERA_FRONT)
		MVector	v = (x - .5) * xDir + (y - .5) * yDir;
		v *= fScale;
		v += v0;
		glVertex3d(v.x, v.y, v.z);
#else
		glVertex2f(x, y);
#endif
		x += dT0;
	} while (x <= 1.f);
#else
	for (int k = 0; k < numControlPoints - 1; ++k) {
		const RampTuple&	left = rampTupleArray(k);
		const RampTuple&	right = rampTupleArray(k + 1);
		const float	x1 = left.Position;
		const float	x2 = right.Position;
		float	y = -1;
		float	x = x1;
		while (x <= x2) {
			const float	y1 = left.FloatValue;
			const float	y2 = right.FloatValue;
			switch (left.InterpType) {
			case INTERP::None:
				y = y1;
				break;
			case INTERP::Linear:
				if (x1 == x2)
					y = y1;
				else
					y = y1 + (x - x1) * (y2 - y1) / (x2 - x1);
				break;
			case INTERP::Smooth:
				if (x1 == x2)
					y = y1;
				else {
					float	s = (x - x1) / (x2 - x1);
					float	dy = y2 - y1;
					y = y1 + s * s * (3 * dy - 2 * dy * s);
				}
				break;
			case INTERP::Spline:
				if (x1 == x2)
					y = y1;
				else {
					assert(k > 0);
					const float	p1 = rampTupleArray(k - 1).FloatValue;
					assert(k + 2 < numControlPoints);
					const float	p4 = rampTupleArray(k + 2).FloatValue;
					const float	p2 = y1;
					const float	p3 = y2;
					float	s = (x - x1) / (x2 - x1);
					float	s2 = s * s;
					float	s3 = s2 * s;
					y = ((-s3 + 2 * s2 - s) * p1 + (3 * s3 - 5 * s2 + 2) * p2 + (-3 * s3 + 4 * s2 + s) * p3 + (s3 - s2) * p4) / 2;
				}
				break;
			default:
				cerr << "Invalid INTERP::Type: " << left.InterpType << endl;
				y = 0;
				break;
			}
#ifdef VIEW_PORT
			glVertex2f(dL + Lx * x, dL + Ly * y);
#elif defined(CAMERA_FRONT)
			MVector	v = (x - .5) * xDir + (y - .5) * yDir;
			v *= fScale;
			v += v0;
			glVertex3d(v.x, v.y, v.z);
#else
			glVertex2f(x, y);
#endif
			x += dT0;
		}
	}
#endif
	glEnd();
}
#endif
#if 0
{
	const int	numInterpolatedPoints = 100;
	glPointSize(2);
	glColor3f(0, 1, 0);
	glBegin(GL_POINTS);
	for (int i = 0; i <= numInterpolatedPoints; ++i) {
		float	x = 1.f / numInterpolatedPoints * i;
		if (x < 0.f)
			x = 0.f;
		if (x > 1.f)
			x = 1.f;
		float	y = -1.f;
#if 1
		tentacleShape::GetInterpolatedValue(rampTupleArray, x, y, false);
#else
		for (int k = 0; k < numControlPoints - 1; ++k) {
			const RampTuple&	left = rampTupleArray(k);
			const RampTuple&	right = rampTupleArray(k + 1);
			const float	x1 = left.Position;
			const float	x2 = right.Position;
			if (x1 <= x && x <= x2) {
				const float	y1 = left.FloatValue;
				const float	y2 = right.FloatValue;
				switch (left.InterpType) {
				case INTERP::None:
					y = y1;
					break;
				case INTERP::Linear:
					if (x1 == x2)
						y = y1;
					else
						y = y1 + (x - x1) * (y2 - y1) / (x2 - x1);
					if (y < 0.f)
						y = 0.f;
					if (y > 1.f)
						y = 1.f;
					break;
				case INTERP::Smooth:
					if (x1 == x2)
						y = y1;
					else {
						float	s = (x - x1) / (x2 - x1);
						float	dy = y2 - y1;
						y = y1 + s * s * (3 * dy - 2 * dy * s);
					}
					if (y < 0.f)
						y = 0.f;
					if (y > 1.f)
						y = 1.f;
					break;
				case INTERP::Spline:
					if (x1 == x2)
						y = y1;
					else {
						assert(k > 0);
						const float	p1 = rampTupleArray(k - 1).FloatValue;
						assert(k + 2 < numControlPoints);
						const float	p4 = rampTupleArray(k + 2).FloatValue;
						const float	p2 = y1;
						const float	p3 = y2;
						float	s = (x - x1) / (x2 - x1);
						float	s2 = s * s;
						float	s3 = s2 * s;
y = ((-s3 + 2 * s2 - s) * p1 + (3 * s3 - 5 * s2 + 2) * p2 + (-3 * s3 + 4 * s2 + s) * p3 + (s3 - s2) * p4) / 2;
					}
					if (y < 0.f)
						y = 0.f;
					if (y > 1.f)
						y = 1.f;
					break;
				default:
					cerr << "Invalid INTERP::Type: " << left.InterpType << endl;
					y = 0.f;
					break;
				}
				break;
			}
		}
#endif
#ifdef VIEW_PORT
		glVertex2f(dL + Lx * x, dL + Ly * y);
#elif defined(CAMERA_FRONT)
		MVector	v = (x - .5) * xDir + (y - .5) * yDir;
		v *= fScale;
		v += v0;
		glVertex3d(v.x, v.y, v.z);
#else
		glVertex2f(x, y);
#endif
	}
	glEnd();
}
#endif
#endif
} while(0);
#pragma warning(default: 4127)
#ifdef VIEW_PORT
	glMatrixMode(matrix_mode);
	glPopMatrix();
#endif
	glPointSize(lastPointSize);
	glColor3fv(rgba);
}
	view.endGL();
#endif
#endif
#endif
#endif
#endif
#endif
