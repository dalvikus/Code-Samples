#pragma
#ifndef __kPICK_H__
#define __kPICK_H__
////////////////////////////////////////////////////////////////////////////////


#include <DXUT/Core/DXUT.h>
#include <DXUT/Optional/DXUTcamera.h>
#include <DXUT/Optional/DXUTgui.h>	// CDXUTDialogResourceManager
#include <DXUT/Optional/DXUTsettingsdlg.h>	// CD3DSettingsDlg
#include <mesh/kState.h>
#include <mesh/kMesh.h>

#if 1	// 마지막 주문; 1. 회전축 고정
// CD3DArcBall 의 QuatFromBallPoints 함수에서 특정 축에 대한 회전을 생각하려고...
//	CD3DArcBall::QuatFromBallPoints 함수를 override 하면 되는 데,
//	static 으로 선언되어 있어 이 함수를 호출하는 CD3DArcBall::OnMove 함수를 override 한다.
class CD3DArcBall2 : public CD3DArcBall
{
public:
	struct FIXED_AXIS {
		enum TYPE {
			FREE	= -1,	// default
			X	= 0,
			Y	= 1,
			Z	= 2,
		};
	};
private:
	FIXED_AXIS::TYPE	__fixed_axis_type;
public:
	void	SetFixedAxisType(FIXED_AXIS::TYPE fixed_axis_type)	{__fixed_axis_type = fixed_axis_type;}
	FIXED_AXIS::TYPE	GetFixedAxisType() const	{return __fixed_axis_type;}

public:
	CD3DArcBall2() : __fixed_axis_type(FIXED_AXIS::FREE) {}

	// override
	// CD3DArcBall::QuatFromBallPoints 함수를 override 하면 되는 데,
	// static 으로 선언되어 있어 이 함수를 호출하는 CD3DArcBall::OnMove 함수를 override 한다.
	// 또한 CD3DArcBall::OnMove 도 Pick.cpp 의 MsgProc 에서 직접 호출하는 것이 아니므로,
	// OnMove 를 호출하는 HandleMessages 자체를 override 해야 한다.
private:
	D3DXQUATERNION	QuatFromBallPoints2(const D3DXVECTOR3& vFrom, const D3DXVECTOR3& vTo);
public:
	void	OnMove2(int nX, int nY);
	LRESULT	HandleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

#if 1	// 마지막 주문; 9. 카메라 위치 제한
public:
	void	SetTranslationMatrix(const D3DXMATRIX& m)	{m_mTranslation = m;}
#endif	// 마지막 주문; 9. 카메라 위치 제한
};
#endif	// 마지막 주문; 1. 회전축 고정

// CModelViewerCamera/CBaseCamera 에서 View Matrix 를 설정할 때, Up 방향을 원하는 대로 정할 수 없다.
class CModelViewerCamera2 : public CModelViewerCamera
{
private:
	bool		__saved;
	D3DXMATRIX	__projectionMatrix;
public:
	CModelViewerCamera2() : __saved(false) {}
	virtual ~CModelViewerCamera2() {}

	// 진짜(VIEW_T.orthographic: True) Orthographic View 로 갈 때,
	// 카메라의 Projection Matrix 를 변경한다.
	// 이 때 원래의 Projection Matrix 를 저장했다가
	// Perspective View 로 돌아갈 때 원래의 Projection Matrix 로 복원해야 한다.
	void	loadProjMatrix()	{if (__saved) {m_mProj = __projectionMatrix; __saved = false;}}
	void	saveProjMatrix()	{if (!__saved) {__projectionMatrix = m_mProj; __saved = true;}}
public:
	void	SetViewParams2(const D3DXVECTOR3& Eye, const D3DXVECTOR3& At, const D3DXVECTOR3& Up);
	void	SetProjMatrix(const D3DXMATRIX& projectionMatrix)	{m_mProj = projectionMatrix;}
};

#define USE__PickVar	// PickVar 구조를 사용;
			// 원본과 마찬가지로 필요한 변수들을 main.cpp 에서 전역변수로 선언
			// (아래의 USE__pointer_to_kMesh 가 정의된 것만 제외하고)
#define USE__pointer_to_PickVar	// USE__PickVar 가 정의되었을 때만 유효 (아래 [확인 1])
//#define USE__fileOpen	// 수시로 gMesh, gMesh0 를 바꿀 수 있도록 한다;
			// 1. USE__PickVar 가 정의되지 않으면 의미가 없다.
			//	USE__PickVar 가 정의되지 않으면 의미가 없다. (아래 [확인 2])
			// 2. __READ_FILES__ 이 정의되어 있으면 읽을 때 속도가 빨라진다.
			// 3. 추가적으로 MULTI_STANDARD 가 정의되어 있다면
			// 측정모델/표준모델을 바꿔가며 다양한 모델을 사용할 수 있다.
#define USE__pointer_to_kMesh	// gMesh(gMesh0) 대신에 gMeshPtr(gMeshPtr0)을 정의해서 필요할 때마다
				// 해당하는 kMesh 의 Instance 를 만든다;
				// USE__PickVar 가 정의되지 않고
				// USE__pointer_to_kMesh 까지 정의되지 않으면 원본과 정확히 일치
				//	USE__fileOpen 가 정의된 경우 이 값도 반드시 정의되어야 한다. (아래 [확인 3])

// [확인 1]
#ifdef USE__pointer_to_PickVar	// USE__PickVar 가 정의되지 않으면 의미가 없다.
#ifndef USE__PickVar
#pragma message("USE__pointer_to_PickVar defined but USE__PickVar not defined: force to clear USE__pointer_to_PickVar")
#undef USE__pointer_to_PickVar
#endif	// !USE__PickVar
#endif	// USE__pointer_to_PickVar
// [확인 2]
#ifdef USE__fileOpen	// USE__PickVar 가 정의되지 않으면 의미가 없다.
#ifndef USE__PickVar
#pragma message("USE__fileOpen defined but USE__PickVar not defined: force to clear USE__fileOpen")
#undef USE__fileOpen
#endif	// !USE__PickVar
#endif	// USE__fileOpen
// [확인 3]
#ifdef USE__fileOpen	// USE__fileOpen 이 설정되어있다면 반드시 USE__pointer_to_kMesh 을 설정되어 있어야 한다.
#ifndef USE__pointer_to_kMesh
#pragma message("USE__fileOpen defined but USE__pointer_to_kMesh not defined: force to set USE__pointer_to_kMesh")
#define USE__pointer_to_kMesh
#endif	// !USE__pointer_to_kMesh

typedef enum {
	CONNECTABLE_LOGO	= -1,
	CONNECTABLE_NO		= 0,
	CONNECTABLE_YES		= 1,
} CONNECTABLE_TYPE_T;
#endif	// USE__fileOpen
// [확인 4]: 파일캐쉬(__WRITE_FILES__)를 만들 때는 일시적으로 USE__pointer_to_PickVar 와 USE__fileOpen 을 해제한다.
#if 0
#ifdef __WRITE_FILES__
#ifdef USE__pointer_to_PickVar
#pragma message("__WRITE_FILES__ defined, disable USE__pointer_to_PickVar")
#undef USE__pointer_to_PickVar
#endif	// USE__pointer_to_PickVar
#ifdef USE__fileOpen
#pragma message("__WRITE_FILES__ defined, disable USE__fileOpen")
//#undef USE__fileOpen
#endif	// USE__fileOpen
#endif	// __WRITE_FILES__
#endif

#ifdef USE__PickVar
struct PickVar
{
private:
	void	__init();
public:
	PickVar();
	virtual ~PickVar();

#ifdef USE__fileOpen
protected:
	struct SetAllArg : kMesh::SetAllArg
	{
		bool	isSet;	// 이 값이 True 인 경우,
				// OnCreateDevice 에서 kMesh::SetAllArg 를 사용하여 kMesh 의 Instance 를 만든다;
				// kMesh::SetAllArg::devicePtr 은 OnCreateDevice 에서 새로운 값으로 덮어써야 한다.
		CONNECTABLE_TYPE_T	connectable;	// 표준모델이 있는 경우 연결할 수 있는 지...;
					// 테스트모델을/로고를 제외하고 대부분의 경우 CONNECTABLE_YES;
					// (load 함수에서 기본값은 CONNECTABLE_YES)

		SetAllArg() : isSet(false), connectable(CONNECTABLE_YES) {}
	};

protected:
	// 표준모델을 측정모델에 연결;
	// MULTI_STANDARD 가 정의된 경우 표준모델 인덱스가 일치한 경우에 한해서
	int	__connect();
	// 표준모델을 측정모델에서 분리
	int	__disconnect();
public:
	// Pick.cpp 의 OnCreateDevice 에서 호출;
	// 이전에 설정된 측정모델/표준모델이 있다면
	// 새로운 IDirect3DDevice9 를 사용하여 새로 로드
	int	OnCreateDevice(IDirect3DDevice9* devicePtr);

	int	load(MODEL_TYPE_T modelType, const kMesh::SetAllArg& arg, CONNECTABLE_TYPE_T connectable = CONNECTABLE_YES);	// 모델을 로드
	int	unload(MODEL_TYPE_T modelType);				// 모델을 언로드
#endif	// USE__fileOpen
public:
	kState	state;
#if 1	// 마지막 주문; 1. 회전축 고정
	CD3DArcBall2	g_ArcBall;		// Arcball for model control
#else	// 마지막 주문; 1. 회전축 고정/!마지막 주문; 1. 회전축 고정
	CD3DArcBall	g_ArcBall;		// Arcball for model control
#endif	// !마지막 주문; 1. 회전축 고정
#ifdef USE__pointer_to_kMesh
#ifdef USE__fileOpen
	SetAllArg	setAllArg;
#endif	// USE__fileOpen
	kMesh*	gkMeshPtr;
#ifdef USE__fileOpen
	SetAllArg	setAllArg0;
#endif	// USE__fileOpen
	kMesh*	gkMeshPtr0;
#else	// USE__pointer_to_kMesh/!USE__pointer_to_kMesh
	kMesh		gMesh;
	kMesh		gMesh0;
#endif	// !USE__pointer_to_kMesh

	ID3DXFont*	g_pFont;		// Font for drawing text
	ID3DXSprite*	g_pTextSprite;		// Sprite for batching draw text calls
	ID3DXEffect*	g_pEffect;		// D3DX effect interface
//	CDXUTXFileMesh	g_Mesh;			// The mesh to be rendered

	CModelViewerCamera2	g_Camera;	// A model viewing camera

	bool		g_bShowHelp;		// If true, it renders the UI control text
	bool		g_bUseD3DXIntersect;	// Whether to use D3DXIntersect
	bool		g_bAllHits;		// Whether to just get the first "hit" or all "hits"
	CDXUTDialogResourceManager	g_DialogResourceManager;	// manager for shared resources of dialogs
	CD3DSettingsDlg	g_SettingsDlg;		// Device settings dialog
	CDXUTDialog	g_HUD;			// dialog for standard controls
	CDXUTDialog	g_SampleUI;		// dialog for sample specific controls
};
#endif	// USE__PickVar


////////////////////////////////////////////////////////////////////////////////
#endif	// !__kPICK_H__
