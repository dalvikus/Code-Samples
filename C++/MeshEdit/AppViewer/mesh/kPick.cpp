#include <mesh/kPick.h>
#include <iostream>
using std::cout;
using std::cerr;
using std::endl;

#if 1	// 마지막 주문; 1. 회전축 고정
LRESULT	CD3DArcBall2::HandleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// Current mouse position
	int	iMouseX = (short) LOWORD(lParam);
	int	iMouseY = (short) HIWORD(lParam);

	switch (uMsg) {
	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
		SetCapture(hWnd);
		OnBegin(iMouseX, iMouseY);
		return TRUE;

	case WM_LBUTTONUP:
		ReleaseCapture();
		OnEnd();
		return TRUE;

	case WM_CAPTURECHANGED:
		if ((HWND) lParam != hWnd) {
			ReleaseCapture();
			OnEnd();
		}
		return TRUE;

	case WM_RBUTTONDOWN:
	case WM_RBUTTONDBLCLK:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONDBLCLK:
		SetCapture(hWnd);
		// Store off the position of the cursor when the button is pressed
		m_ptLastMouse.x = iMouseX;
		m_ptLastMouse.y = iMouseY;
		return TRUE;

		case WM_RBUTTONUP:
		case WM_MBUTTONUP:
		ReleaseCapture();
		return TRUE;

	case WM_MOUSEMOVE:
		if (MK_LBUTTON & wParam) {
#if 1	// 마지막 주문; 1. 회전축 고정
			OnMove2(iMouseX, iMouseY);	// OVERRIDE
#endif	// 마지막 주문; 1. 회전축 고정
		} else if ((MK_RBUTTON & wParam) || (MK_MBUTTON & wParam)) {
			// Normalize based on size of window and bounding sphere radius
			FLOAT	fDeltaX = (m_ptLastMouse.x - iMouseX) * m_fRadiusTranslation / m_nWidth;
			FLOAT	fDeltaY = (m_ptLastMouse.y - iMouseY) * m_fRadiusTranslation / m_nHeight;

			if (wParam & MK_RBUTTON) {
				D3DXMatrixTranslation(&m_mTranslationDelta, -2 * fDeltaX, 2 * fDeltaY, 0.f);
				D3DXMatrixMultiply(&m_mTranslation, &m_mTranslation, &m_mTranslationDelta);
			} else {	// wParam & MK_MBUTTON
				D3DXMatrixTranslation(&m_mTranslationDelta, 0.f, 0.f, 5 * fDeltaY);
				D3DXMatrixMultiply(&m_mTranslation, &m_mTranslation, &m_mTranslationDelta);
			}

			// Store mouse coordinate
			m_ptLastMouse.x = iMouseX;
			m_ptLastMouse.y = iMouseY;
		}
		return TRUE;
	}

	return FALSE;
}

void	CD3DArcBall2::OnMove2(int nX, int nY)
{
	if (m_bDrag) {
		m_vCurrentPt = ScreenToVector((float) nX, (float) nY);
#if 1	// 마지막 주문; 1. 회전축 고정
		m_qNow = m_qDown * QuatFromBallPoints2(m_vDownPt, m_vCurrentPt);	// OVERRIDE
#endif	// 마지막 주문; 1. 회전축 고정
	}
}

D3DXQUATERNION	CD3DArcBall2::QuatFromBallPoints2(const D3DXVECTOR3& vFrom, const D3DXVECTOR3& vTo)
{
	if (__fixed_axis_type == FIXED_AXIS::FREE)	// CD3DArcBall::QuatFromBallPoints 와 동일
		return CD3DArcBall::QuatFromBallPoints(vFrom, vTo);

	D3DXVECTOR3	from, to;
	switch (__fixed_axis_type) {
	case FIXED_AXIS::X:
		from = D3DXVECTOR3(0, vFrom.y, vFrom.z);
//		cout << "vFrom: [" << vFrom.x << ", " << vFrom.y << ", " << vFrom.z << "]" << endl;
//		cout << "from: [" << from.x << ", " << from.y << ", " << from.z << "]" << endl;
		D3DXVec3Normalize(&from, &from);
		to = D3DXVECTOR3(0, vTo.y, vTo.z);
//		cout << "vTo: [" << vTo.x << ", " << vTo.y << ", " << vTo.z << "]" << endl;
//		cout << "to: [" << to.x << ", " << to.y << ", " << to.z << "]" << endl;
		D3DXVec3Normalize(&to, &to);
		break;
	case FIXED_AXIS::Y:
		from = D3DXVECTOR3(vFrom.x, 0, vFrom.z);
//		cout << "vFrom: [" << vFrom.x << ", " << vFrom.y << ", " << vFrom.z << "]" << endl;
//		cout << "from: [" << from.x << ", " << from.y << ", " << from.z << "]" << endl;
		D3DXVec3Normalize(&from, &from);
		to = D3DXVECTOR3(vTo.x, 0, vTo.z);
//		cout << "vTo: [" << vTo.x << ", " << vTo.y << ", " << vTo.z << "]" << endl;
//		cout << "to: [" << to.x << ", " << to.y << ", " << to.z << "]" << endl;
		D3DXVec3Normalize(&to, &to);
		break;
	case FIXED_AXIS::Z:
		from = D3DXVECTOR3(vFrom.x, vFrom.y, 0);
//		cout << "vFrom: [" << vFrom.x << ", " << vFrom.y << ", " << vFrom.z << "]" << endl;
//		cout << "from: [" << from.x << ", " << from.y << ", " << from.z << "]" << endl;
		D3DXVec3Normalize(&from, &from);
		to = D3DXVECTOR3(vTo.x, vTo.y, 0);
//		cout << "vTo: [" << vTo.x << ", " << vTo.y << ", " << vTo.z << "]" << endl;
//		cout << "to: [" << to.x << ", " << to.y << ", " << to.z << "]" << endl;
		D3DXVec3Normalize(&to, &to);
		break;
	default:
		// unreachable
		throw;
	}
	float	fDot = D3DXVec3Dot(&from, &to);
	D3DXVECTOR3 vAxis;
	D3DXVec3Cross(&vAxis, &from, &to);
	D3DXQUATERNION	q;
	D3DXQuaternionRotationAxis(&q, D3DXVec3Cross(&vAxis, &from, &to), acosf(fDot));
//	cout << "vAxis: [" << vAxis.x << ", " << vAxis.y << ", " << vAxis.z << "]" << endl;
	return q;
}
#endif	// 마지막 주문; 1. 회전축 고정

void
CModelViewerCamera2::SetViewParams2(const D3DXVECTOR3& Eye, const D3DXVECTOR3& At, const D3DXVECTOR3& Up)
{
//	CBaseCamera::SetViewParams( pvEyePt, pvLookatPt );
//	void	CBaseCamera::SetViewParams(D3DXVECTOR3* pvEyePt, D3DXVECTOR3* pvLookatPt);
{
	if (NULL == &Eye || NULL == &At || NULL == &Up)
		return;

	m_vDefaultEye = m_vEye = Eye;
	m_vDefaultLookAt = m_vLookAt = At;

	// Calc the view matrix
//	D3DXVECTOR3	vUp( 0,1,0 );
	D3DXMatrixLookAtLH(&m_mView, &Eye, &At, &Up);

	D3DXMATRIX	mInvView;
	D3DXMatrixInverse(&mInvView, NULL, &m_mView);

	// The axis basis vectors and camera position are stored inside the
	// position matrix in the 4 rows of the camera's world matrix.
	// To figure out the yaw/pitch of the camera, we just need the Z basis vector
	D3DXVECTOR3*	pZBasis = (D3DXVECTOR3*) &mInvView._31;

	m_fCameraYawAngle = atan2f(pZBasis->x, pZBasis->z);
	float	fLen = sqrtf(pZBasis->z * pZBasis->z + pZBasis->x * pZBasis->x);
	m_fCameraPitchAngle = -atan2f(pZBasis->y, fLen);
}

//	void	CModelViewerCamera::SetViewParams(D3DXVECTOR3* pvEyePt, D3DXVECTOR3* pvLookatPt);
{
	// Propogate changes to the member arcball
	D3DXQUATERNION	quat;
	D3DXMATRIX	mRotation;
//	D3DXVECTOR3	vUp( 0,1,0 );
	D3DXMatrixLookAtLH(&mRotation, &Eye, &At, &Up);
	D3DXQuaternionRotationMatrix(&quat, &mRotation);
	m_ViewArcBall.SetQuatNow(quat);

	// Set the radius according to the distance
	D3DXVECTOR3	vEyeToPoint;
	D3DXVec3Subtract(&vEyeToPoint, &At, &Eye);
	SetRadius(D3DXVec3Length(&vEyeToPoint));

	// View information changed. FrameMove should be called.
	m_bDragSinceLastUpdate = true;
}
}


#ifdef USE__PickVar
PickVar::PickVar()
{
	cout << "PickVar::PickVar()" << endl;
	__init();
}

PickVar::~PickVar()
{
	cout << "PickVar::~PickVar()" << endl;
#ifdef USE__pointer_to_kMesh
	cout << "gkMeshPtr = " << gkMeshPtr << endl;
	if (gkMeshPtr) {
		cout << "delete kMesh" << endl;
		delete gkMeshPtr;
		gkMeshPtr = 0;
	}
	cout << "gkMeshPtr0 = " << gkMeshPtr0 << endl;
	if (gkMeshPtr0) {
		cout << "delete kMesh0" << endl;
		delete gkMeshPtr0;
		gkMeshPtr0 = 0;
	}
#endif	// USE__pointer_to_kMesh

	__init();
}

void	PickVar::__init()
{
#ifdef USE__pointer_to_kMesh
	gkMeshPtr = 0;
	gkMeshPtr0 = 0;
#endif	// USE__pointer_to_kMesh
	g_pFont = NULL;
	g_pTextSprite = NULL;
	g_pEffect = NULL;
	g_bShowHelp = true;
	g_bUseD3DXIntersect = true;
	g_bAllHits = true;
}

#ifdef USE__fileOpen
// 표준모델을 측정모델에 연결;
// MULTI_STANDARD 가 정의된 경우 표준모델 인덱스가 일치한 경우에 한해서
int	PickVar::__connect()
{
	// 측정모델/표준모델 모두 로드된 상태에서...
	if (!gkMeshPtr || !gkMeshPtr0) {
		if (!gkMeshPtr)
			cout << "INFO: PickVar::__connect: " << "측정모델 미로드" << endl;
		if (!gkMeshPtr0)
			cout << "INFO: PickVar::__connect: " << "표준모델 미로드" << endl;
		return 0;
	}
	if (setAllArg.connectable != CONNECTABLE_YES)
		return 0;

	cout << "INFO: PickVar::__connect: Connected? " << (gkMeshPtr->meshPtr0() ? "True" : "False") << endl;
#if defined(__READ_FILES__) && defined(MULTI_STANDARD)
	if (setAllArg.index_to_standard != setAllArg0.index_to_standard) {
		// 이 표준모델을 참고하는 측정모델이 아니므로 아무 일도 하지 않는다.
		cerr << "WARNING: PickVar::__connect: 실패: " << "표준모델 불일치: 표준모델[" << setAllArg0.index_to_standard << "], 측정모델[" << setAllArg.index_to_standard << "]" << endl;
//		cout << "gkMeshPtr->meshPtr0(): " << gkMeshPtr->meshPtr0() << endl;
		if (gkMeshPtr->meshPtr0()) {	// 더 이상 맞는 표준모델이 아니므로
			cout << "INFO: PickVar::__connect: already connected: 더 이상 유요한 표준모델이 아니므로 연결을 강제로 끊는다" << endl;
			gkMeshPtr->meshPtr0() = 0;	// 연결을 끊는다.
		}
		return 0;
	}
#endif	// (__READ_FILES__ && MULTI_STANDARD)
	gkMeshPtr->meshPtr0() = gkMeshPtr0;
#if defined(__READ_FILES__) && defined(MULTI_STANDARD)
	cout << "INFO: PickVar::__connect: 성공: " << (setAllArg0.index_to_standard == 0 ? "DAZ" : "에트리") << " 측정모델에 연결" << endl;
#endif	// (__READ_FILES__ && MULTI_STANDARD)
	return 1;
}

// 표준모델을 측정모델에서 분리
int	PickVar::__disconnect()
{
	if (!gkMeshPtr || !gkMeshPtr0) {
		if (!gkMeshPtr)
			cout << "INFO: PickVar::__disconnect: " << "측정모델 미로드" << endl;
		if (!gkMeshPtr0)
			cout << "INFO: PickVar::__disconnect: " << "표준모델 미로드" << endl;
		return 0;
	}
	if (setAllArg.connectable != CONNECTABLE_YES)
		return 0;

	cout << "INFO: PickVar::__disconnect: Connected? " << (gkMeshPtr->meshPtr0() ? "True" : "False") << endl;
#if defined(__READ_FILES__) && defined(MULTI_STANDARD)
	if (setAllArg.index_to_standard != setAllArg0.index_to_standard) {
		// 이 표준모델을 참고하는 측정모델이 아니므로 아무 일도 하지 않는다.
		cerr << "INFO: PickVar::__disconnect: 실패: " << "표준모델 불일치: 표준모델[" << setAllArg0.index_to_standard << "], 측정모델[" << setAllArg.index_to_standard << "]" << endl;
//		cout << "gkMeshPtr->meshPtr0(): " << gkMeshPtr->meshPtr0() << endl;
		assert(gkMeshPtr->meshPtr0() == 0);
		return 0;
	}
#endif	// (__READ_FILES__ && MULTI_STANDARD)
	gkMeshPtr->meshPtr0() = NULL;
#if defined(__READ_FILES__) && defined(MULTI_STANDARD)
	cout << "INFO: PickVar::__disconnect: 성공: " << (setAllArg0.index_to_standard == 0 ? "DAZ" : "에트리") << " 측정모델에서 분리" << endl;
#endif	// (__READ_FILES__ && MULTI_STANDARD)
	return 1;
}

// Pick.cpp 의 OnCreateDevice 에서 호출;
// 이전에 설정된 측정모델/표준모델이 있다면
// 새로운 IDirect3DDevice9 를 사용하여 새로 로드
int	PickVar::OnCreateDevice(IDirect3DDevice9* devicePtr)
{
	if (setAllArg.isSet) {
		cout << "INFO: PickVar::setAllArg.isSet? True" << endl;
		setAllArg.devicePtr = devicePtr;
		load(MODEL_TYPE__REAL, setAllArg);
	}
	if (setAllArg0.isSet) {
		cout << "INFO: PickVar::setAllArg0.isSet? True" << endl;
		setAllArg0.devicePtr = devicePtr;
		load(MODEL_TYPE__STANDARD, setAllArg0);
	}

	return 0;
}

// 모델을 로드
int	PickVar::load(MODEL_TYPE_T modelType, const kMesh::SetAllArg& arg, CONNECTABLE_TYPE_T connectable)
{
	kMesh*&	kMeshPtrRef = modelType == MODEL_TYPE__STANDARD ? gkMeshPtr0 : gkMeshPtr;
	cout << "INFO: kMeshPtr = " << kMeshPtrRef << endl;
	if (kMeshPtrRef) {
		cout << "INFO: delete kMesh..." << endl;
		delete kMeshPtrRef;
		kMeshPtrRef = 0;
	}
	kMeshPtrRef = new kMesh;
	cout << kMeshPtrRef << endl;

	kMeshPtrRef->setAll(arg);

	PickVar::SetAllArg&	argRef = modelType == MODEL_TYPE__STANDARD ? setAllArg0 : setAllArg;
	argRef.isSet = true;
	argRef.connectable = connectable;
	*((kMesh::SetAllArg*) &argRef) = arg;

	if (connectable == CONNECTABLE_YES)
		__connect();

	return 0;
}

// 모델을 언로드
int	PickVar::unload(MODEL_TYPE_T modelType)
{
	__disconnect();

	kMesh*&	kMeshPtrRef = modelType == MODEL_TYPE__STANDARD ? gkMeshPtr0 : gkMeshPtr;
	cout << "INFO: kMeshPtr = " << kMeshPtrRef << endl;
	if (kMeshPtrRef) {
		cout << "INFO: delete kMesh..." << endl;
		delete kMeshPtrRef;
		kMeshPtrRef = 0;
	}

	PickVar::SetAllArg&	argRef = modelType == MODEL_TYPE__STANDARD ? setAllArg0 : setAllArg;
	argRef.isSet = false;
	argRef.connectable = CONNECTABLE_YES;

	return 0;
}
#endif	// USE__fileOpen
#endif	// USE__PickVar
