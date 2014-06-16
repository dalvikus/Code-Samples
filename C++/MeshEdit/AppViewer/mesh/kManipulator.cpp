#include <mesh/kManipulator.h>
#include <mesh/kMesh.h>
#include <iostream>
using std::cout;
using std::cerr;
using std::endl;

kNormalManipulator::kNormalManipulator() : CD3DArcBall()
{
	reset();
}

void	kNormalManipulator::reset()
{
	__angle_rotation = MANIPULATOR_NORMAL_ANGLE_ROTATION_BOTH_THETA_AND_PHI;
	__P0 = D3DXVECTOR3(0, 0, 0);
	__nDir = D3DXVECTOR3(0, 0, 1);
	__xDir = D3DXVECTOR3(1, 0, 0);
	__yDir = D3DXVECTOR3(0, 1, 0);
	D3DXMatrixIdentity(&__cameraRotationMatrix);
	D3DXMatrixIdentity(&__cameraRotationMatrixInverse);

	kManipulator::reset();
}

void	kNormalManipulator::set(
	float	radius,

	MANIPULATOR_NORMAL_ANGLE_ROTATION_T	angle_rotation,

	const D3DXVECTOR3&	P0,
	const D3DXVECTOR3&	nDir,
	const D3DXVECTOR3&	xDir,

	DO_RELEASE_CALLBACK_POINTER	releaseCallbackPtr,
	kMesh*		pointer_to_kMesh,
	int		area,
	START_DEF_PTR*	pointer_to_START_DEF_PTR
)
{
	reset();

	// Setup the arcball parameters
	Reset();	// 모든 파라미터를 초기화한다;
//	cout << "radius = " << radius << endl;
	__radiusInScreenSpace = 100.f;	// Arcball 이 놓일 영역을 결정한다;
					// 원래는 Object Space 에서의 Arcball 의 반경으로부터 결정되어야 한다.
			// TO DO LIST:
			//	kMesh::RenderManipulator 에서 Arcball 의 반경을 0.1 * kMesh::__radius 로 설정했다.
			//	이 값의 두 배를 한 변의 길이로 하는 정사각형을 Arcball 영역으로 설정하려 했다.
			//	(kMesh::__setNormalManipulator 에서)
			//	위의 값은 Object Space 에서의 값이다.
			//	영역을 결정할 때는 Screen Space 에서의 값이 필요하다.
			//	Q. 어떻게 변환을 자유스럽게 할 수 있을까?
			//	Hint. kMesh::DrawAxis, kMesh::DrawAxis0
			//
			//	주) 영역의 크기는 Pick.cpp 에서 CD3DArcBall::SetOffset 을 결정할 때 매우 중요하다.
			//	(MsgProc 참고)
	SetWindow(2 * (INT) __radiusInScreenSpace, 2 * (INT) __radiusInScreenSpace, 1.f);

	__angle_rotation = angle_rotation;

	__P0 = P0;
	D3DXVec3Normalize(&__nDir, &nDir);	// __zDir
	D3DXVec3Normalize(&__xDir, &xDir);
	D3DXVec3Cross(&__yDir, &__nDir, &__xDir);	// __yDir = __zDir x __xDir

	__isSet	= true;
	__releaseCallbackPtr = releaseCallbackPtr;
	__pointer_to_kMesh = pointer_to_kMesh;
	__area = area;
	__pointer_to_START_DEF_PTR = pointer_to_START_DEF_PTR;
	if (__area > 0)
		__pointer_to_START_DEF_PTR = 0;
}
#if 0
#if 0
/* Usage
 *
	D3DXMATRIX	m;	// 회전에 해당하는 행렬
	D3DXQUATERNION	q;
	D3DXQuaternionRotationMatrix(&q, D3DXMatrixRotationAxis(&m, &D3DXVECTOR3(0, 1, 0), D3DX_PI));
	CD3DArcBall2::Reset(q);
 */
#endif
CD3DArcBall2::CD3DArcBall2() :
	CD3DArcBall(),
	__rightDir(D3DXVECTOR3(1, 0, 0)),
	__upDir(D3DXVECTOR3(0, 1, 0)),
	__viewDir(D3DXVECTOR3(0, 0, 1))
{
}

void	CD3DArcBall2::Reset(const D3DXMATRIX* cameraRotationMatrixPtr)
{
	CD3DArcBall::Reset();

	__rightDir = D3DXVECTOR3(1, 0, 0);
	__upDir = D3DXVECTOR3(0, 1, 0);
	__viewDir = D3DXVECTOR3(0, 0, 1);
	if (cameraRotationMatrixPtr) {
		D3DXMATRIX	cameraRotationInverseMatrix;
		D3DXMatrixInverse(&cameraRotationInverseMatrix, NULL, cameraRotationMatrixPtr);

		// Transform vectors based on camera's rotation matrix
		D3DXVECTOR3	vLocalRight = D3DXVECTOR3(1, 0, 0);
		D3DXVec3TransformCoord(&__rightDir, &vLocalRight, &cameraRotationInverseMatrix);
		D3DXVECTOR3	vLocalUp = D3DXVECTOR3(0, 1, 0);
		D3DXVec3TransformCoord(&__upDir, &vLocalUp, &cameraRotationInverseMatrix);
		D3DXVECTOR3	vLocalAhead = D3DXVECTOR3(0, 0, 1);
		D3DXVec3TransformCoord(&__viewDir, &vLocalAhead, &cameraRotationInverseMatrix);
	}
}

D3DXVECTOR3	CD3DArcBall2::ScreenToVector(float fScreenPtX, float fScreenPtY)
{
#if 1	// CD3DArcBall::ScreenToVector 로부터...
	// Scale to screen
	FLOAT	x = -(fScreenPtX - m_Offset.x - m_nWidth / 2) / (m_fRadius * m_nWidth / 2);
	FLOAT	y = (fScreenPtY - m_Offset.y - m_nHeight / 2) / (m_fRadius * m_nHeight / 2);

	FLOAT	z = 0.0f;
	FLOAT	mag = x * x + y * y;
	if (mag > 1.0f) {
		FLOAT	scale = 1.0f / sqrtf(mag);
		x *= scale;
		y *= scale;
	} else
		z = sqrtf(1.0f - mag);

	// Return vector
#if 1
	return x * __rightDir + y * __upDir + z * __viewDir;
#else
	return	D3DXVECTOR3(x, y, z);
#endif
#endif
}
#endif

void	kNormalManipulator::SetCameraRotationMatrix(const D3DXMATRIX& cameraRotationMatrix)
{
	if (D3DXMatrixInverse(&__cameraRotationMatrixInverse, NULL, &cameraRotationMatrix))
		__cameraRotationMatrix = cameraRotationMatrix;
	else {	// 역행렬이 존재하지 않을 때
		D3DXMatrixIdentity(&__cameraRotationMatrix);
		D3DXMatrixIdentity(&__cameraRotationMatrixInverse);
	}
}

void	kNormalManipulator::doRelease()
{
	D3DXMATRIX	m = __cameraRotationMatrix * *GetRotationMatrix() * __cameraRotationMatrixInverse;
	if (__angle_rotation == MANIPULATOR_NORMAL_ANGLE_ROTATION_BOTH_THETA_AND_PHI)
		D3DXVec3TransformCoord(&__xDir, &__xDir, &m);
	D3DXVec3TransformCoord(&__nDir, &__nDir, &m);
	D3DXVec3TransformCoord(&__yDir, &__yDir, &m);
	if (__angle_rotation == MANIPULATOR_NORMAL_ANGLE_ROTATION_THETA_ONLY) {	// __xDir 에 수직인 방향으로만 움직인다.
		// __xDir 에 수직인 벡터만 얻는다
		__yDir = __yDir - D3DXVec3Dot(&__yDir, &__xDir) * __xDir;
		D3DXVec3Normalize(&__yDir, &__yDir);
		__nDir = __nDir - D3DXVec3Dot(&__nDir, &__xDir) * __xDir;
		D3DXVec3Normalize(&__nDir, &__nDir);
	}

	// 두 번 호출되더라고 같은 결과가 나오도록...
	Reset();
	D3DXMatrixIdentity(&__cameraRotationMatrix);
	D3DXMatrixIdentity(&__cameraRotationMatrixInverse);

	if (__releaseCallbackPtr)
		__releaseCallbackPtr(__pointer_to_kMesh, __area, __pointer_to_START_DEF_PTR, &__nDir);
}

D3DXVECTOR3*	kNormalManipulator::GetNew_xDir(D3DXVECTOR3& xDir) const
{
	if (__angle_rotation == MANIPULATOR_NORMAL_ANGLE_ROTATION_THETA_ONLY) {
		xDir = __xDir;
		return &xDir;
	}
	CD3DArcBall&	nonConstArcBall = *((CD3DArcBall*) this);
	D3DXMATRIX	m = __cameraRotationMatrix * *nonConstArcBall.GetRotationMatrix() * __cameraRotationMatrixInverse;
	return D3DXVec3TransformCoord(&xDir, &__xDir, &m);
}
D3DXVECTOR3*	kNormalManipulator::GetNew_yDir(D3DXVECTOR3& yDir) const
{
	CD3DArcBall&	nonConstArcBall = *((CD3DArcBall*) this);
	D3DXMATRIX	m = __cameraRotationMatrix * *nonConstArcBall.GetRotationMatrix() * __cameraRotationMatrixInverse;
	D3DXVec3TransformCoord(&yDir, &__yDir, &m);
	if (__angle_rotation == MANIPULATOR_NORMAL_ANGLE_ROTATION_THETA_ONLY) {	// __xDir 에 수직인 방향으로만 움직인다.
		// __xDir 에 수직인 벡터만 얻는다
		yDir = yDir - D3DXVec3Dot(&yDir, &__xDir) * __xDir;
		D3DXVec3Normalize(&yDir, &yDir);
	}
	return &yDir;
}
D3DXVECTOR3*	kNormalManipulator::GetNew_nDir(D3DXVECTOR3& nDir) const
{
	CD3DArcBall&	nonConstArcBall = *((CD3DArcBall*) this);
	D3DXMATRIX	m = __cameraRotationMatrix * *nonConstArcBall.GetRotationMatrix() * __cameraRotationMatrixInverse;
	D3DXVec3TransformCoord(&nDir, &__nDir, &m);
	if (__angle_rotation == MANIPULATOR_NORMAL_ANGLE_ROTATION_THETA_ONLY) {	// __xDir 에 수직인 방향으로만 움직인다.
		// __xDir 에 수직인 벡터만 얻는다
		nDir = nDir - D3DXVec3Dot(&nDir, &__xDir) * __xDir;
		D3DXVec3Normalize(&nDir, &nDir);
	}
	return &nDir;
}

#if 1	// interface
int	kNormalManipulator::getValue(int subType, void* dataPtr)
{
	if (!dataPtr) {
		(void) fprintf(stderr, "Fatal: kNormalManipulator::getValue: dataPtr is nil\n");
		return -1;
	}

	switch (subType) {
	case MANIPULATOR__SET:
		*((int*) dataPtr) = __area;
		break;
	case MANIPULATOR__NORMAL_ANGLE_ROTATION:
		*((MANIPULATOR_NORMAL_ANGLE_ROTATION_T*) dataPtr) = __angle_rotation;
		break;
	default:
		(void) fprintf(stderr, "Error: kNormalManipulator::getValue: MANIPULATOR__SUBTYPE = %d: Unknown subType\n", subType);
	}

	return -1;
}

int	kNormalManipulator::setValue(int subType, const void* dataPtr)
{
	if (!dataPtr) {
		(void) fprintf(stderr, "Fatal: kNormalManipulator::setValue: dataPtr is nil\n");
		return -1;
	}

	switch (subType) {
	case MANIPULATOR__NORMAL_ANGLE_ROTATION:
		__angle_rotation = *((MANIPULATOR_NORMAL_ANGLE_ROTATION_T*) dataPtr);
		break;
	default:
		(void) fprintf(stderr, "Error: kNormalManipulator::setValue: MANIPULATOR__SUBTYPE = %d: Unknown subType\n", subType);
	}

	return -1;
}
#endif	// interface

#if 1	// point manipulator
int	kPointManipulator::getValue(int subType, void* dataPtr)
{
	if (!dataPtr) {
		(void) fprintf(stderr, "Fatal: kPointManipulator::getValue: dataPtr is nil\n");
		return -1;
	}

	switch (subType) {
	case MANIPULATOR__SET:
		*((int*) dataPtr) = __area;
		break;
	default:
		(void) fprintf(stderr, "Error: kPointManipulator::getValue: MANIPULATOR__SUBTYPE = %d: Unknown subType\n", subType);
	}

	return -1;
}

int	kPointManipulator::setValue(int subType, const void* dataPtr)
{
	if (!dataPtr) {
		(void) fprintf(stderr, "Fatal: kPointManipulator::setValue: dataPtr is nil\n");
		return -1;
	}

#pragma warning(disable: 4065)	// warning C4065: switch statement contains 'default' but no 'case' labels
	switch (subType) {
	default:
		(void) fprintf(stderr, "Error: kPointManipulator::setValue: MANIPULATOR__SUBTYPE = %d: Unknown subType\n", subType);
	}
#pragma warning(default: 4065)

	return -1;
}

LRESULT	kPointManipulator::HandleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return TRUE;
}


void	kPointManipulator::doRelease()
{
	if (__indexPicked == -1)
		return;

	START_DEF_PTR&	startDefPtr = *__pointer_to_START_DEF_PTR;
	if (startDefPtr.type == START_DEF_PTR_TYPE__GIRTH)
		startDefPtr.plane_def_ptr->pointDef = __pointDef;
	else if (startDefPtr.type == START_DEF_PTR_TYPE__TWO_POINT) {
		if (__indexPicked == 0)
			startDefPtr.two_point_plane_def_ptr->point0Def = __pointDef;
		else
			startDefPtr.two_point_plane_def_ptr->point1Def = __pointDef;
	} else {
		assert(0);
	}

	if (__releaseCallbackPtr) {
		INDEX_POINT_DEF_PTR	index_pointDefPtr = {__indexPicked, &__pointDef};
		__releaseCallbackPtr(__pointer_to_kMesh, __area, __pointer_to_START_DEF_PTR, &index_pointDefPtr);
	}

	__indexPicked = -1;	// 새로 시작한다.
}

void	kPointManipulator::pick(const CModelViewerCamera& camera)
{
	if (!__isSet)
		return;

	// 현재 마우스의 위치
	POINT	ptCursor;
	GetCursorPos(&ptCursor);
	ScreenToClient(DXUTGetHWND(), &ptCursor);

	const kMesh&	mesh = *__pointer_to_kMesh;
	const START_DEF_PTR&	startDefPtr = *__pointer_to_START_DEF_PTR;
	// 시작점: START_DEF_PTR::type = START_DEF_PTR_TYPE__GIRTH 인 경우 한 점만 조사하고,
	// START_DEF_PTR_TYPE__TWO_POINT 의 경우에는 두 점을 조사한다.
	D3DXVECTOR3	p0;
	if (startDefPtr.type == START_DEF_PTR_TYPE__GIRTH)
		__pointDef = startDefPtr.plane_def_ptr->pointDef;
	else if (startDefPtr.type == START_DEF_PTR_TYPE__TWO_POINT)
		__pointDef = startDefPtr.two_point_plane_def_ptr->point0Def;
	else
		assert(0);
	mesh.getPoint(__pointDef, p0);
	D3DVIEWPORT9	viewport;
	DXUTGetD3D9Device()->GetViewport(&viewport);
	D3DXVECTOR3	p0_2;	// p0 의 스크린 좌표
	D3DXVec3Project(
		&p0_2,
		&p0,
		&viewport,
		camera.GetProjMatrix(),
		camera.GetViewMatrix(),
		camera.GetWorldMatrix()
	);
	if (kMesh::isPointPicked(p0_2, ptCursor, 4)) {	// 4:= kMesh::RenderManipulator 에서 점의 크기가 9
		__indexPicked = 0;
		return;
	}
	if (startDefPtr.type == START_DEF_PTR_TYPE__GIRTH) {	// 더 이상 조사할 점이 없다.
		__indexPicked = -1;
		return;
	}
	assert(startDefPtr.type == START_DEF_PTR_TYPE__TWO_POINT);

	// START_DEF_PTR_TYPE__TWO_POINT 의 경우 두 번째 점을 조사한다.
	__pointDef = startDefPtr.two_point_plane_def_ptr->point1Def;
	mesh.getPoint(__pointDef, p0);
	D3DXVec3Project(
		&p0_2,
		&p0,
		&viewport,
		camera.GetProjMatrix(),
		camera.GetViewMatrix(),
		camera.GetWorldMatrix()
	);
	__indexPicked = kMesh::isPointPicked(p0_2, ptCursor, 4) ? 1 : -1;
	return;
}
#endif	// point manipulator

#if 1	// line manipulator
int	kLineManipulator::getValue(int subType, void* dataPtr)
{
	if (!dataPtr) {
		(void) fprintf(stderr, "Fatal: kLineManipulator::getValue: dataPtr is nil\n");
		return -1;
	}

	switch (subType) {
	case MANIPULATOR__SET:
		*((int*) dataPtr) = __area;
		break;
	default:
		(void) fprintf(stderr, "Error: kLineManipulator::getValue: MANIPULATOR__SUBTYPE = %d: Unknown subType\n", subType);
	}

	return -1;
}

int	kLineManipulator::setValue(int subType, const void* dataPtr)
{
	if (!dataPtr) {
		(void) fprintf(stderr, "Fatal: kLineManipulator::setValue: dataPtr is nil\n");
		return -1;
	}

#pragma warning(disable: 4065)	// warning C4065: switch statement contains 'default' but no 'case' labels
	switch (subType) {
	default:
		(void) fprintf(stderr, "Error: kLineManipulator::setValue: MANIPULATOR__SUBTYPE = %d: Unknown subType\n", subType);
	}
#pragma warning(default: 4065)

	return -1;
}

void	kLineManipulator::doRelease()
{
	if (__indexPicked == -1)
		return;

	__isReleased = true;
	__indexPicked = -1;
	__isPointDefSet = false;
	__isOutOfRange = false;

	// 화면 보여주기 클리어
	kMesh&	mesh = *__pointer_to_kMesh;
	mesh.tapeMeasure().twoPointArray().RemoveAll();
	mesh.tapeMeasure().convexPointArray().RemoveAll();
	mesh.numSegments() = 0;

	(void) fprintf(stdout,
		"__leftIndex = %d, __rightIndex = %d, # of points including end points = %d (%d)\n"
		,
		__leftIndex, __rightIndex, __twoPointArray.GetSize(), __convexPointArray.GetSize()
	);
	if (__leftIndex == -1 || __rightIndex == -1)
		return;
#if 1
	const CGrowableArray<SEGMENT_VERTEX>&	newSegmentPointArray = __mode == TAPE_MEASURE_MODE_CONTOUR ? __twoPointArray : __convexPointArray;
	CGrowableArray<SEGMENT_VERTEX>	segmentPointArray;
	if (__closed) {
		// (__rightIndex...__leftIndex)
		for (int index = __rightIndex + 1; index != __leftIndex; ++index) {
			if (index + 1 == __segmentPointArray.GetSize()) {
				index = 0;
				if (__leftIndex == 0)
					break;
			}
			segmentPointArray.Add(__segmentPointArray[index]);
		}
		for (int i = 0; i < newSegmentPointArray.GetSize(); ++i)
			segmentPointArray.Add(newSegmentPointArray[i]);
	} else {
		// [0, __leftIndex)
		for (int i = 0; i < __leftIndex; ++i)
			segmentPointArray.Add(__segmentPointArray[i]);
		for (int i = 0; i < newSegmentPointArray.GetSize(); ++i)
			segmentPointArray.Add(newSegmentPointArray[i]);
		// (__rightIndex, N)
		for (int i = __rightIndex + 1; i < __segmentPointArray.GetSize(); ++i)
			segmentPointArray.Add(__segmentPointArray[i]);
	}
	__numPointsInSegmentBuffer = segmentPointArray.GetSize();
	__pointer_to_kMesh->SetSegmentBuffer(__segmentVertexBufferPtr, __segmentIndexBufferPtr, segmentPointArray, __closed);
	__segmentPointArray = segmentPointArray;

	// 새로운 길이
	float	length = 0.f;
	const SEGMENT_VERTEX*	vertexArray = __segmentPointArray.GetData();
	for (int n = 0; n < (int) __numPointsInSegmentBuffer - 1; ++n) {
		const D3DXVECTOR3&	p1 = vertexArray[n].P;
		const D3DXVECTOR3&	p2 = vertexArray[n + 1].P;
		D3DXVECTOR3	v = p2 - p1;
		length += D3DXVec3Length(&v);
	}
	if (__closed) {
		const D3DXVECTOR3&	p1 = vertexArray[0].P;
		const D3DXVECTOR3&	p2 = vertexArray[__numPointsInSegmentBuffer - 1].P;
		D3DXVECTOR3	v = p2 - p1;
		length += D3DXVec3Length(&v);
	}
	// __area 에 따라 해당 내용을 업데이트; length, Vertex/Index Buffer, # of segments
	// kState::g__numSegments, kState::g__stringLength
	// 렌더링 버퍼
	//	kMesh 에서 kTapeMeasure::doMeasure_s 호출 후의 과정을 따른다.
	kState&	state = *mesh.statePtr();
	unsigned	numSegments = __closed ? __numPointsInSegmentBuffer : __numPointsInSegmentBuffer - 1;
	kTapeMeasure&	tapeMeasure = mesh.tapeMeasure();
	mesh.segmentMethod() = __method;
	if (__area) {
		tapeMeasure.length() = -1.f;
		MEASURE_ITEM&	m_item = *__pointer_to_MEASURE_ITEM;
		m_item.length = length;
		m_item.numSegments = numSegments;
		mesh.SetSegmentBuffer(m_item.segmentVertexBuffer, m_item.segmentIndexBuffer, segmentPointArray, __closed);
	} else {
		tapeMeasure.length() = length;
		state.g__stringLength = length;
		state.g__numSegments = numSegments;
		mesh.numSegments() = numSegments;
		mesh.SetSegmentBuffer(mesh.GetSegmentVertexBuffer(), mesh.GetSegmentIndexBuffer(), segmentPointArray, __closed);
		// 렌더링을 위한 설정; kMesh::ShowSegment
#if 0
		TAPE_MEASURE_METHOD_T&		segmentMethod = meshPtr->segmentMethod();
		tapeMeasure.twoPointArray() = twoPointArray;
		tapeMeasure.SetTwoPointBuffer();
		tapeMeasure.convexPointArray() = convexPointArray;
		statePtr->g__next_convex_index = 0;
		if (startDefPtr.type == START_DEF_PTR_TYPE__GIRTH) {
			tapeMeasure.SetConvexBuffer(closed);
			segmentMethod = TAPE_MEASURE_METHOD_GIRTH;
		} else if (startDefPtr.type == START_DEF_PTR_TYPE__TWO_POINT) {
			tapeMeasure.SetConvexBuffer(false);
			segmentMethod = TAPE_MEASURE_METHOD_TWO_POINT;
		} else
			assert(0);
#endif
	}
#else
	// __segmentVertexBufferPtr 의 내용을 __segmentPointArray 에 복사
	__segmentPointArray.RemoveAll();
	SEGMENT_VERTEX*	vertexArrayPtr;
	__segmentVertexBufferPtr->Lock(0, 0, (void**) &vertexArrayPtr, 0);
	for (int n = 0; n < (int) __numPointsInSegmentBuffer; ++n)
		__segmentPointArray.Add(vertexArrayPtr[n]);
	__segmentVertexBufferPtr->Unlock();
#endif
}


void	kLineManipulator::reset()
{
	__numPointsInSegmentBuffer = 0;
	__closed = true;
	__pointer_to_MEASURE_ITEM = 0;
	__isReleased = false;
	__indexPicked = -1;
	__isPointDefSet = false;
	__isOutOfRange = false;
	__segmentPointArray.RemoveAll();
	SAFE_RELEASE(__segmentVertexBufferPtr);
	SAFE_RELEASE(__segmentIndexBufferPtr);

	kManipulator::reset();
}

void	kLineManipulator::set(kMesh* pointer_to_kMesh, int area, TAPE_MEASURE_MODE_T mode, TAPE_MEASURE_METHOD_T method, bool closed, unsigned numPoints, const SEGMENT_VERTEX* pointPtr, MEASURE_ITEM* pointer_to_MEASURE_ITEM)
{
	reset();

	assert(pointer_to_kMesh);
	__pointer_to_kMesh = pointer_to_kMesh;
	__area = area;
	__pointer_to_MEASURE_ITEM = pointer_to_MEASURE_ITEM;

	__mode = mode;
	__method = method;
	__closed = closed;
	__numPointsInSegmentBuffer = numPoints;
	for (int n = 0; n < (int) numPoints; ++n)
		__segmentPointArray.Add(pointPtr[n]);
	__pointer_to_kMesh->SetSegmentBuffer(__segmentVertexBufferPtr, __segmentIndexBufferPtr, __segmentPointArray, __closed);

	__isSet = true;
}

void	kLineManipulator::pick(const CModelViewerCamera& camera)
{
	if (!__isSet)
		return;

	// 현재 마우스의 위치
	POINT	ptCursor;
	GetCursorPos(&ptCursor);
	ScreenToClient(DXUTGetHWND(), &ptCursor);

	D3DXVECTOR3*	projectedArray = 0;
	int	numPoints = __segmentPointArray.GetSize();
	int	numSegments = __closed ? numPoints : numPoints - 1;
	(void) printf("numPoints = %d, numSegments = %d\n", numPoints, numSegments);
do {
	projectedArray = new D3DXVECTOR3[numPoints];

//	const kMesh&	mesh = *__pointer_to_kMesh;
	D3DVIEWPORT9	viewport;
	DXUTGetD3D9Device()->GetViewport(&viewport);
	D3DXVec3ProjectArray(
		projectedArray, sizeof(D3DXVECTOR3),
		(D3DXVECTOR3*) __segmentPointArray.GetData(), sizeof(SEGMENT_VERTEX),
		&viewport,
		camera.GetProjMatrix(),
		camera.GetViewMatrix(),
		camera.GetWorldMatrix(),
		numPoints
	);
	__indexPicked = -1;
	__isPointDefSet = false;
	__isOutOfRange = false;
	float	z_max = -1.f;
	float	t_max = -1.f;	// z_max 가 나오도록 하는 점의 t
	for (int i = 0; i < (int) numSegments; ++i) {
		const D3DXVECTOR3&	start = projectedArray[i];
		const D3DXVECTOR3&	end = projectedArray[__closed && i + 1 == numSegments ? 0 : i + 1];
#if 1
		float	t;
		if (fabsf(end.y - start.y) > fabsf(end.x - start.x))	// y 를 기준
			t = ((float) ptCursor.y - start.y) / (end.y - start.y);
		else	// x를 기준
			t = ((float) ptCursor.x - start.x) / (end.x - start.x);
#else
		float	t = ((float) ptCursor.x - start.x) / (end.x - start.x);
#endif
		if (!_finite(t) || t < 0.f || t > 1.f)
			continue;
		D3DXVECTOR3	p2;
		p2.x = start.x + t * (end.x - start.x);	// ptCursor.x
		p2.y = start.y + t * (end.y - start.y);
		p2.z = start.z + t * (end.z - start.z);
//		(void) fprintf(stdout, "t = %f, (%d, %d) <-> (%f, %f)\n", t, ptCursor.x, ptCursor.y, p2.x, p2.y);
		if (kMesh::isPointPicked(p2, ptCursor, 1)) {	// 1:= 점의 범위;
					// 이 값이 크면 선 주위를 대충 골라도 그 선이 선택되나
					// 그렇게 되면 실제 위치의 정확도는 떨어질 수 있다. (TO DO LIST)
//			(void) fprintf(stdout, "(%d, %d): %f (%f)\n", i, __closed && i + 1 == numSegments ? 0 : i + 1, t, p2.z);
			if (p2.z > z_max) {
				t_max = t;
				__indexPicked = i;
			}
		}
	}
//	(void) fprintf(stdout, "__indexPicked = %d\n", __indexPicked);
	if (__indexPicked != -1) {	// __pickedPoint 를 결정
		int	next = __indexPicked + 1;
		if (__closed && next == numSegments)
			next = 0;
		__pickedPoint = __segmentPointArray[__indexPicked].P + t_max * (__segmentPointArray[next].P - __segmentPointArray[__indexPicked].P);
	}
} while (0);
	if (projectedArray) {
		delete[] projectedArray;
		projectedArray = 0;
	}

	return;
}

const float	kLineManipulator::Stretch_Threshold = 1.2f;	// 최대 20% 까지 늘어날 수 있다.
int	kLineManipulator::setPointDef(const POINT_DEF& pointDef)
{
	if (__indexPicked < 0)
		return -1;

	__isOutOfRange = false;
	__isPointDefSet = true;
	__planeDef.pointDef = pointDef;

	assert(__pointer_to_kMesh);
	kMesh&	mesh = *__pointer_to_kMesh;
//	kState&	state = *mesh.statePtr();

	__rightIndex = -1;
	CGrowableArray<SEGMENT_VERTEX>	rightTwoPointArray;
	CGrowableArray<SEGMENT_VERTEX>	rightConvexPointArray;
	(void) fprintf(stdout, "right...\n");
#if 1	// 오른쪽
{
	float	len0 = 0.f;	// 원래 길이; 최대 len0 * Stretch_Threshold 까지 늘어날 수 있다.
	int	index = __indexPicked + 1 == __segmentPointArray.GetSize() ? 0 : __indexPicked + 1;
	// index 가 다음 값이 되면 loop 을 끝내야 한다.
	int	index0 = __closed ?
		(__indexPicked + __segmentPointArray.GetSize() / 2) % __segmentPointArray.GetSize() :	// 전체의 반
		__segmentPointArray.GetSize();	// 오른쪽 끝
	D3DXVECTOR3	lastP = __pickedPoint;	// len0 을 계산한 마지막 지점
	do {
		if (__closed && index + 1 == __segmentPointArray.GetSize())	// __close = True
			index = 0;
		const SEGMENT_VERTEX&	segmentVertex = __segmentPointArray[index];
		len0 += D3DXVec3Length(&D3DXVECTOR3(segmentVertex.P - lastP));
		lastP = segmentVertex.P;
		(void) fprintf(stdout, "index = %d (index0 = %d): ENTER\n", index, index0);

		TWO_POINT_PLANE_DEF2	_2p2Def;
		_2p2Def.point0Def = pointDef;
		_2p2Def.normal = D3DXVECTOR3(0, 0, 0);
		_2p2Def.segmentVertex = segmentVertex;
		START_DEF_PTR	startDefPtr;
		startDefPtr.type = START_DEF_PTR_TYPE__TWO_POINT2;
		startDefPtr.two_point_plane_def2_ptr = &_2p2Def;

#if 1
		// 둘레측정
		CGrowableArray<SEGMENT_VERTEX>	segmentPointArray;
		float	length;
		bool	closed;
		int	numSegments_in_kState = kTapeMeasure::doMeasure_s(
			&mesh,

			__mode,	////
			startDefPtr,

			// [OUT]
			NULL,
			segmentPointArray, length, closed,
			rightTwoPointArray, rightConvexPointArray
		);
		mesh.tapeMeasure().length() = length;	// 임의계측에서 참고할 수 있도록 설정
#ifdef _DEBUG
		(void) fprintf(stdout,
			"# of segments = %d, length = %f, closed? %s; "
			"# of segment point(s) = %d, # of two point(s) = %d, # of convex point(s) = %d\n"
			,
			numSegments_in_kState, length, closed ? "True" : "False",
			segmentPointArray.GetSize(), rightTwoPointArray.GetSize(), rightConvexPointArray.GetSize()
		);
#endif
#else	// 화면에 보여주기
		// 모두 Reference
		FLOAT&				length			= state.g__stringLength;
		bool&				closed			= state.g__stringClosed;
		int&				numSegments_in_kState	= state.g__numSegments;
		kTapeMeasure&			tapeMeasure		= mesh.tapeMeasure();
		DWORD&				numSegments_in_kMesh	= mesh.numSegments();
		TAPE_MEASURE_METHOD_T&		segmentMethod		= mesh.segmentMethod();
		LPDIRECT3DVERTEXBUFFER9&	segmentVertexBuffer	= mesh.GetSegmentVertexBuffer();
		LPDIRECT3DINDEXBUFFER9&		segmentIndexBuffer	= mesh.GetSegmentIndexBuffer();

		// 둘레측정
		CGrowableArray<SEGMENT_VERTEX>	segmentPointArray;
		CGrowableArray<SEGMENT_VERTEX>	twoPointArray;
		CGrowableArray<SEGMENT_VERTEX>	convexPointArray;
		numSegments_in_kState = kTapeMeasure::doMeasure_s(
			&mesh,

			__mode,	////
			startDefPtr,

			// [OUT]
			NULL,
			segmentPointArray, length, closed,
			twoPointArray, convexPointArray
		);
		mesh.tapeMeasure().length() = length;	// 임의계측에서 참고할 수 있도록 설정
#ifdef _DEBUG
		(void) fprintf(stdout,
			"# of segments = %d, length = %f, closed? %s; "
			"# of segment point(s) = %d, # of two point(s) = %d, # of convex point(s) = %d\n"
			,
			numSegments_in_kState, length, closed ? "True" : "False",
			segmentPointArray.GetSize(), twoPointArray.GetSize(), convexPointArray.GetSize()
		);
#endif
		tapeMeasure.twoPointArray() = twoPointArray;
		tapeMeasure.SetTwoPointBuffer();
		tapeMeasure.convexPointArray() = convexPointArray;
		tapeMeasure.SetConvexBuffer(false);
		segmentMethod = TAPE_MEASURE_METHOD_TWO_POINT;
		numSegments_in_kMesh = numSegments_in_kState;
		if (numSegments_in_kMesh > 0) {
			numSegments_in_kMesh = segmentPointArray.GetSize();
			--numSegments_in_kMesh;
			mesh.SetSegmentBuffer(segmentVertexBuffer, segmentIndexBuffer, segmentPointArray, closed);
		}
#endif
		if (numSegments_in_kState < 1) {
			index = index0;	// 아래에서 __isOutOfRange 가 설정되도록...
			break;
		}
		(void) fprintf(stdout, "index = %d (index0 = %d): LEAVE\n", index, index0);
		(void) fprintf(stdout, ">>>> length = %f, len0 = %f, length / len0 = %f (%f)\n", length, len0, length / len0, Stretch_Threshold);
		if (length < len0 * Stretch_Threshold) {
			break;
		}
	} while (++index != index0);
//	__isOutOfRange = index == index0;
	if (index != index0)
		__rightIndex = index;
}
#endif
	__leftIndex = -1;
	CGrowableArray<SEGMENT_VERTEX>	leftTwoPointArray;
	CGrowableArray<SEGMENT_VERTEX>	leftConvexPointArray;
	(void) fprintf(stdout, "left...\n");
#if 1	// 왼쪽
{
	float	len0 = 0.f;	// 원래 길이; 최대 len0 * Stretch_Threshold 까지 늘어날 수 있다.
	int	index = __indexPicked;
	// index 가 다음 값이 되면 loop 을 끝내야 한다.
	int	index0 = __closed ?
		(__indexPicked + __segmentPointArray.GetSize() / 2) % __segmentPointArray.GetSize() :	// 전체의 반
		-1;	// 왼쪽 끝
	D3DXVECTOR3	lastP = __pickedPoint;	// len0 을 계산한 마지막 지점
	do {
		if (__closed && index == -1)	// __close = True
			index = __segmentPointArray.GetSize() - 1;
		const SEGMENT_VERTEX&	segmentVertex = __segmentPointArray[index];
		len0 += D3DXVec3Length(&D3DXVECTOR3(segmentVertex.P - lastP));
		lastP = segmentVertex.P;
		(void) fprintf(stdout, "index = %d (index0 = %d): ENTER\n", index, index0);

		TWO_POINT_PLANE_DEF2	_2p2Def;
		_2p2Def.point0Def = pointDef;
		_2p2Def.normal = D3DXVECTOR3(0, 0, 0);
		_2p2Def.segmentVertex = segmentVertex;
		START_DEF_PTR	startDefPtr;
		startDefPtr.type = START_DEF_PTR_TYPE__TWO_POINT2;
		startDefPtr.two_point_plane_def2_ptr = &_2p2Def;

#if 1
		// 둘레측정
		CGrowableArray<SEGMENT_VERTEX>	segmentPointArray;
		float	length;
		bool	closed;
		int	numSegments_in_kState = kTapeMeasure::doMeasure_s(
			&mesh,

			__mode,	////
			startDefPtr,

			// [OUT]
			NULL,
			segmentPointArray, length, closed,
			leftTwoPointArray, leftConvexPointArray
		);
		mesh.tapeMeasure().length() = length;	// 임의계측에서 참고할 수 있도록 설정
#ifdef _DEBUG
		(void) fprintf(stdout,
			"# of segments = %d, length = %f, closed? %s; "
			"# of segment point(s) = %d, # of two point(s) = %d, # of convex point(s) = %d\n"
			,
			numSegments_in_kState, length, closed ? "True" : "False",
			segmentPointArray.GetSize(), leftTwoPointArray.GetSize(), leftConvexPointArray.GetSize()
		);
#endif
#else	// 화면에 보여주기
		// 모두 Reference
		FLOAT&				length			= state.g__stringLength;
		bool&				closed			= state.g__stringClosed;
		int&				numSegments_in_kState	= state.g__numSegments;
		kTapeMeasure&			tapeMeasure		= mesh.tapeMeasure();
		DWORD&				numSegments_in_kMesh	= mesh.numSegments();
		TAPE_MEASURE_METHOD_T&		segmentMethod		= mesh.segmentMethod();
		LPDIRECT3DVERTEXBUFFER9&	segmentVertexBuffer	= mesh.GetSegmentVertexBuffer();
		LPDIRECT3DINDEXBUFFER9&		segmentIndexBuffer	= mesh.GetSegmentIndexBuffer();

		// 둘레측정
		CGrowableArray<SEGMENT_VERTEX>	segmentPointArray;
		CGrowableArray<SEGMENT_VERTEX>	twoPointArray;
		CGrowableArray<SEGMENT_VERTEX>	convexPointArray;
		numSegments_in_kState = kTapeMeasure::doMeasure_s(
			&mesh,

			__mode,	////
			startDefPtr,

			// [OUT]
			NULL,
			segmentPointArray, length, closed,
			twoPointArray, convexPointArray
		);
		mesh.tapeMeasure().length() = length;	// 임의계측에서 참고할 수 있도록 설정
#ifdef _DEBUG
		(void) fprintf(stdout,
			"# of segments = %d, length = %f, closed? %s; "
			"# of segment point(s) = %d, # of two point(s) = %d, # of convex point(s) = %d\n"
			,
			numSegments_in_kState, length, closed ? "True" : "False",
			segmentPointArray.GetSize(), twoPointArray.GetSize(), convexPointArray.GetSize()
		);
#endif
		tapeMeasure.twoPointArray() = twoPointArray;
		tapeMeasure.SetTwoPointBuffer();
		tapeMeasure.convexPointArray() = convexPointArray;
		tapeMeasure.SetConvexBuffer(false);
		segmentMethod = TAPE_MEASURE_METHOD_TWO_POINT;
		numSegments_in_kMesh = numSegments_in_kState;
		if (numSegments_in_kMesh > 0) {
			numSegments_in_kMesh = segmentPointArray.GetSize();
			--numSegments_in_kMesh;
			mesh.SetSegmentBuffer(segmentVertexBuffer, segmentIndexBuffer, segmentPointArray, closed);
		}
#endif
		if (numSegments_in_kState < 1) {
			index = index0;	// 아래에서 __isOutOfRange 가 설정되도록...
			break;
		}
		(void) fprintf(stdout, "index = %d (index0 = %d): LEAVE\n", index, index0);
		(void) fprintf(stdout, ">>>> length = %f, len0 = %f, length / len0 = %f (%f)\n", length, len0, length / len0, Stretch_Threshold);
		if (length < len0 * Stretch_Threshold) {
			break;
		}
	} while (--index != index0);
//	__isOutOfRange = index == index0;
	if (index != index0)
		__leftIndex = index;
}
#endif
	__isOutOfRange = __rightIndex == -1 || __leftIndex == -1;
	if (__isOutOfRange)
		return -1;
#if 0
	// __segmentVertexBufferPtr 를 변경한다;
	// 1. rightSegmentPointArray 의 마지막 점은 기존의 __rightIndex 번째 점과 같은 점이므로,
	// rightSegmentPointArray 에서 마지막 점을 제거한다;
	// leftSegmentPointArray 에 대해서도 마찬가지이다.
//	leftSegmentPointArray.Remove(leftSegmentPointArray.GetSize() - 1);
//	rightSegmentPointArray.Remove(rightSegmentPointArray.GetSize() - 1);
	// 2. leftSegmentPointArray 의 시작점과 rightSegmentPointArray 의 시작점은 같다.
//	leftSegmentPointArray.Remove(0);
	CGrowableArray<SEGMENT_VERTEX>	segmentPointArray;
	if (__closed) {
		// (__rightIndex...__leftIndex)
		for (int index = __rightIndex + 1; index != __leftIndex; ++index) {
			if (index + 1 == __segmentPointArray.GetSize()) {
				index = 0;
				if (__leftIndex == 0)
					break;
			}
			segmentPointArray.Add(__segmentPointArray[index]);
		}
		// leftSegmentPointArray - 1
		for (int n = 0; n < leftSegmentPointArray.GetSize() - 1; ++n)
			segmentPointArray.Add(leftSegmentPointArray[n]);
		// rightSegmentPointArray
		for (int n = 0; n < rightSegmentPointArray.GetSize(); ++n)
			segmentPointArray.Add(rightSegmentPointArray[n]);
	} else {
		// [0, __leftIndex)
		for (int i = 0; i < __leftIndex; ++i)
			segmentPointArray.Add(__segmentPointArray[i]);
		// leftSegmentPointArray - 1
		for (int n = 0; n < leftSegmentPointArray.GetSize() - 1; ++n)
			segmentPointArray.Add(leftSegmentPointArray[n]);
		// rightSegmentPointArray
		for (int n = 0; n < rightSegmentPointArray.GetSize(); ++n)
			segmentPointArray.Add(rightSegmentPointArray[n]);
		// (__rightIndex, N)
		for (int i = __rightIndex + 1; i < __segmentPointArray.GetSize(); ++i)
			segmentPointArray.Add(__segmentPointArray[i]);
	}
	(void) fprintf(stdout, "__leftIndex = %d, __rightIndex = %d, # of points = %d\n", __leftIndex, __rightIndex, segmentPointArray.GetSize());
	__numPointsInSegmentBuffer = segmentPointArray.GetSize();
	__pointer_to_kMesh->SetSegmentBuffer(__segmentVertexBufferPtr, __segmentIndexBufferPtr, segmentPointArray, __closed);
#else	// 변화한 부분만 모아서 따로 렌더링하기(kMesh::ShowSegment 를 이용);
	// kTapeMeasure::__twoPointArray, kTapeMeasure::__convexPointArray 에 복사
	__twoPointArray.RemoveAll();
	for (int n = leftTwoPointArray.GetSize() - 1; n >= 0; --n)
		__twoPointArray.Add(leftTwoPointArray[n]);
	for (int n = 1; n < rightTwoPointArray.GetSize(); ++n)
		__twoPointArray.Add(rightTwoPointArray[n]);
	__convexPointArray.RemoveAll();
	for (int n = leftConvexPointArray.GetSize() - 1; n >= 0; --n)
		__convexPointArray.Add(leftConvexPointArray[n]);
	for (int n = 1; n < rightConvexPointArray.GetSize(); ++n)
		__convexPointArray.Add(rightConvexPointArray[n]);

	// 렌더링하는 Vertex Buffer 설정
	mesh.tapeMeasure().twoPointArray() = __twoPointArray;
	mesh.tapeMeasure().SetTwoPointBuffer();
	mesh.tapeMeasure().convexPointArray() = __convexPointArray;
	mesh.tapeMeasure().SetConvexBuffer(false);
	mesh.segmentMethod() = TAPE_MEASURE_METHOD_TWO_POINT;
#endif

	return 0;
}
#if 0	// 둘레측정으로 구현
const float	kLineManipulator::RangeConeAngleInDegree = 10.f;
const float	kLineManipulator::TiltAngleInDegree = 10.f;
int	kLineManipulator::setPointDef(const POINT_DEF& pointDef)
{
	// normal 을 설정
	D3DXVECTOR3	p;
	__pointer_to_kMesh->getPoint(pointDef, p);
	D3DXVECTOR3	v = p - __pickedPoint.P;
	D3DXVec3Normalize(&v, &v);
	if (fabsf(D3DXVec3Length(&v) - 1.f) > kTapeMeasure::Normal_Tolerance) {
		__isOutOfRange = true;
		return -1;	// 의미있는 노말을 설정할 수 없다.
	}
	// 이 벡터는, SEGMENT_VERTEX::nDir x SEGMENT_VERTEX::N 이 결정하는 평면에서 많이 벗어나지 않도록 한다.
	D3DXVECTOR3	N;
	D3DXVec3Cross(&N, &__pickedPoint.nDir, &__pickedPoint.N);
	(void) fprintf(stdout, "Tilt angle in degree: %f (%f)\n", fabsf(90 - acosf(D3DXVec3Dot(&N, &v)) * 180 / D3DX_PI), RangeConeAngleInDegree);
	if (fabsf(90 - acosf(D3DXVec3Dot(&N, &v)) * 180 / D3DX_PI) > fabsf(RangeConeAngleInDegree)) {
//		__isOutOfRange = true;
//		return -1;
	}

	D3DXVECTOR3&	normal = __planeDef.normal;
	// SEGMENT_VERTEX::nDir 을 회전시킬 벡터(nAxis)를 설정; nAxis =  v x __pickedPoint.N;
	D3DXVECTOR3	nAxis;
	D3DXVec3Cross(&nAxis, &v, &__pickedPoint.N);
	D3DXVec3Normalize(&nAxis, &nAxis);
	if (fabsf(D3DXVec3Length(&nAxis) - 1.f) > kTapeMeasure::Normal_Tolerance) {
		__isOutOfRange = true;
		return -1;	// 의미있는 노말을 설정할 수 없다.
	}
	normal = __pickedPoint.N;	// 기존의 노말
	// nAxis 에 대해 기존의 노말을 적절한 각도만큼 회전한 벡터를 새로운 노말로 설정;
	float	angleInDegree = D3DXVec3Dot(&__pickedPoint.nDir, &v) ? TiltAngleInDegree : -TiltAngleInDegree;
	kTapeMeasure::setDir(normal, nAxis, D3DX_PI / 180 * angleInDegree);
	(void) fprintf(stdout, "%f\n", D3DXVec3Dot(&__pickedPoint.nDir, &normal));

	// All set
	__isOutOfRange = false;
	__isPointDefSet = true;
	__planeDef.pointDef = pointDef;

	return 0;
}
#endif	// 둘레측정으로 구현
#endif	// line manipulator
