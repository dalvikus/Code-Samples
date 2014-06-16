//--------------------------------------------------------------------------------------
// File: Pick.cpp
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#include <vector>
#include <iostream>
	using std::cout;
	using std::cerr;
	using std::endl;
#include <DXUT/Core/DXUT.h>
#include <DXUT/Optional/DXUTcamera.h>
#include <DXUT/Optional/DXUTsettingsdlg.h>
#include <DXUT/Optional/SDKmesh.h>
#include <DXUT/Optional/SDKmisc.h>
#include "resource.h"
#include <strsafe.h>

//#define DEBUG_VS	// Uncomment this line to debug vertex shaders
//#define DEBUG_PS	// Uncomment this line to debug pixel shaders

#define CAMERA_DISTANCE 3.5f


//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------
#include <mesh/kPick.h>
#ifdef USE__PickVar
#ifdef USE__pointer_to_PickVar
PickVar*	gPickVarPtr = 0;
#else	// USE__pointer_to_PickVar/!USE__pointer_to_PickVar
PickVar	gPickVar;
#endif	// !USE__pointer_to_PickVar
#else	// USE__PickVar/!USE__PickVar
#include <mesh/kMesh.h>
#include <mesh/kState.h>
kState	state;
#if 1	// 마지막 주문; 1. 회전축 고정
CD3DArcBall2	g_ArcBall;		// Arcball for model control
#else	// 마지막 주문; 1. 회전축 고정/!마지막 주문; 1. 회전축 고정
CD3DArcBall	g_ArcBall;		// Arcball for model control
#endif	// !마지막 주문; 1. 회전축 고정
#ifdef USE__pointer_to_kMesh
kMesh*	gkMeshPtr = 0;
kMesh*	gkMeshPtr0 = 0;
#else	// USE__pointer_to_kMesh/!USE__pointer_to_kMesh
kMesh		gMesh;
kMesh		gMesh0;
#endif	// !USE__pointer_to_kMesh

ID3DXFont*	g_pFont = NULL;		// Font for drawing text
ID3DXSprite*	g_pTextSprite = NULL;	// Sprite for batching draw text calls
ID3DXEffect*	g_pEffect = NULL;	// D3DX effect interface
//CDXUTXFileMesh	g_Mesh;		// The mesh to be rendered

#if 1	// CModelViewerCamera2
CModelViewerCamera2	g_Camera;	// A model viewing camera
#else	// CModelViewerCamera2/CModelViewerCamera
CModelViewerCamera	g_Camera;	// A model viewing camera
#endif	// CModelViewerCamera

bool	g_bShowHelp = true;		// If true, it renders the UI control text
bool	g_bUseD3DXIntersect = true;	// Whether to use D3DXIntersect
bool	g_bAllHits = true;		// Whether to just get the first "hit" or all "hits"
CDXUTDialogResourceManager	g_DialogResourceManager;	// manager for shared resources of dialogs
CD3DSettingsDlg	g_SettingsDlg;	// Device settings dialog
CDXUTDialog	g_HUD;		// dialog for standard controls
CDXUTDialog	g_SampleUI;	// dialog for sample specific controls
#endif	// !USE__PickVar


//--------------------------------------------------------------------------------------
// UI control IDs
//--------------------------------------------------------------------------------------
#define IDC_TOGGLEFULLSCREEN    1
#define IDC_TOGGLEREF           2
#define IDC_CHANGEDEVICE        3
#define IDC_USED3DX             4
#define IDC_ALLHITS             5



//--------------------------------------------------------------------------------------
// Forward declarations
//--------------------------------------------------------------------------------------
#if 1	// w/ comments
//--------------------------------------------------------------------------------------
// Called during device initialization, this code checks the device for some
// minimum set of capabilities, and rejects those that don't pass by returning false.
//--------------------------------------------------------------------------------------
bool	CALLBACK IsDeviceAcceptable(D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, bool bWindowed, void* pUserContext);
//--------------------------------------------------------------------------------------
// As a convenience, DXUT inspects the incoming windows messages for
// keystroke messages and decodes the message parameters to pass relevant keyboard
// messages to the application.  The framework does not remove the underlying keystroke
// messages, which are still passed to the application's MsgProc callback.
//--------------------------------------------------------------------------------------
void	CALLBACK KeyboardProc(UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext);
//--------------------------------------------------------------------------------------
// This callback function is called immediately before a device is created to allow the
// application to modify the device settings. The supplied pDeviceSettings parameter
// contains the settings that the framework has selected for the new device, and the
// application can make any desired changes directly to this structure.  Note however that
// DXUT will not correct invalid device settings so care must be taken
// to return valid device settings, otherwise IDirect3D9::CreateDevice() will fail.
//--------------------------------------------------------------------------------------
bool	CALLBACK ModifyDeviceSettings(DXUTDeviceSettings* pDeviceSettings, void* pUserContext);
//--------------------------------------------------------------------------------------
// Before handling window messages, DXUT passes incoming windows
// messages to the application through this callback function. If the application sets
// *pbNoFurtherProcessing to TRUE, then DXUT will not process this message.
//--------------------------------------------------------------------------------------
LRESULT	CALLBACK MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing, void* pUserContext);
//--------------------------------------------------------------------------------------
// This callback function will be called immediately after the Direct3D device has been
// created, which will happen during application initialization and windowed/full screen
// toggles. This is the best location to create D3DPOOL_MANAGED resources since these
// resources need to be reloaded whenever the device is destroyed. Resources created
// here should be released in the OnDestroyDevice callback.
//--------------------------------------------------------------------------------------
HRESULT	CALLBACK OnCreateDevice(IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext);
//--------------------------------------------------------------------------------------
// This callback function will be called immediately after the Direct3D device has
// been destroyed, which generally happens as a result of application termination or
// windowed/full screen toggles. Resources created in the OnCreateDevice callback
// should be released here, which generally includes all D3DPOOL_MANAGED resources.
//--------------------------------------------------------------------------------------
void	CALLBACK OnDestroyDevice(void* pUserContext);
//--------------------------------------------------------------------------------------
// This callback function will be called once at the beginning of every frame. This is the
// best location for your application to handle updates to the scene, but is not
// intended to contain actual rendering calls, which should instead be placed in the
// OnFrameRender callback.
//--------------------------------------------------------------------------------------
void	CALLBACK OnFrameMove(double fTime, float fElapsedTime, void* pUserContext);
//--------------------------------------------------------------------------------------
// This callback function will be called at the end of every frame to perform all the
// rendering calls for the scene, and it will also be called if the window needs to be
// repainted. After this function has returned, DXUT will call
// IDirect3DDevice9::Present to display the contents of the next buffer in the swap chain
//--------------------------------------------------------------------------------------
void	CALLBACK OnFrameRender(IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext);
//--------------------------------------------------------------------------------------
// Handles the GUI events
//--------------------------------------------------------------------------------------
void	CALLBACK OnGUIEvent(UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext);
//--------------------------------------------------------------------------------------
// This callback function will be called immediately after the Direct3D device has
// entered a lost state and before IDirect3DDevice9::Reset is called. Resources created
// in the OnResetDevice callback should be released here, which generally includes all
// D3DPOOL_DEFAULT resources. See the "Lost Devices" section of the documentation for
// information about lost devices.
//--------------------------------------------------------------------------------------
void	CALLBACK OnLostDevice(void* pUserContext);
//--------------------------------------------------------------------------------------
// This callback function will be called immediately after the Direct3D device has been
// reset, which will happen after a lost device scenario. This is the best location to
// create D3DPOOL_DEFAULT resources since these resources need to be reloaded whenever
// the device is lost. Resources created here should be released in the OnLostDevice
// callback.
//--------------------------------------------------------------------------------------
HRESULT	CALLBACK OnResetDevice(IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext);
// OnResetDevice 중에서, kMesh 로부터 g_Camera, g_ArcBall 을 설정하는 부분만 따로 떼어서
// kMesh 의 Instance 를 만들 때마다 사용한다.
void	SetCameraArcBall();
#else
bool	CALLBACK IsDeviceAcceptable(D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, bool bWindowed, void* pUserContext);
void	CALLBACK KeyboardProc(UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext);
bool	CALLBACK ModifyDeviceSettings(DXUTDeviceSettings* pDeviceSettings, void* pUserContext);
LRESULT	CALLBACK MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing, void* pUserContext);
HRESULT	CALLBACK OnCreateDevice(IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext);
void	CALLBACK OnDestroyDevice(void* pUserContext);
void	CALLBACK OnFrameMove(double fTime, float fElapsedTime, void* pUserContext);
void	CALLBACK OnFrameRender(IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext);
void	CALLBACK OnGUIEvent(UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext);
void	CALLBACK OnLostDevice(void* pUserContext);
HRESULT	CALLBACK OnResetDevice(IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext);
#endif

void	InitApp();
void	RenderText();	// called by OnFrameRender
HRESULT	Pick();		// called by OnFrameRender
#if 0
bool
IntersectTriangle(
	const D3DXVECTOR3&	orig,
	const D3DXVECTOR3&	dir,
	const D3DXVECTOR3&	v0,
	const D3DXVECTOR3&	v1,
	const D3DXVECTOR3&	v2,

	FLOAT*	t,
	FLOAT*	u,
	FLOAT*	v
);
#endif


//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------
INT
WINAPI wWinMain(HINSTANCE, HINSTANCE, LPWSTR, int)
{
#if 1	// into wWinMain
#ifdef USE__pointer_to_PickVar
	gPickVarPtr = new PickVar;
#endif	// USE__pointer_to_PickVar
#endif	// into wWinMain
	cout << "wWinMain" << endl;
    // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	// Set the callback functions. These functions allow DXUT to notify
	// the application about device changes, user input, and windows messages.  The
	// callbacks are optional so you need only set callbacks for events you're interested
	// in. However, if you don't handle the device reset/lost callbacks then the sample
	// framework won't be able to reset your device since the application must first
	// release all device resources before resetting.  Likewise, if you don't handle the
	// device created/destroyed callbacks then DXUT won't be able to
	// recreate your device resources.
	DXUTSetCallbackD3D9DeviceAcceptable(IsDeviceAcceptable);
	DXUTSetCallbackKeyboard(KeyboardProc);
	DXUTSetCallbackDeviceChanging(ModifyDeviceSettings);
	DXUTSetCallbackMsgProc(MsgProc);
	DXUTSetCallbackD3D9DeviceCreated(OnCreateDevice);
	DXUTSetCallbackD3D9DeviceDestroyed(OnDestroyDevice);
	DXUTSetCallbackFrameMove(OnFrameMove);
	DXUTSetCallbackD3D9FrameRender(OnFrameRender);
	DXUTSetCallbackD3D9DeviceLost(OnLostDevice);
	DXUTSetCallbackD3D9DeviceReset(OnResetDevice);

    // Show the cursor and clip it when in full screen
    DXUTSetCursorSettings( true, true );

    InitApp();

    // Initialize DXUT and create the desired Win32 window and Direct3D
    // device for the application. Calling each of these functions is optional, but they
    // allow you to set several options which control the behavior of the framework.
    DXUTInit( true, true ); // Parse the command line and show msgboxes
    DXUTSetHotkeyHandling( true, true, true );  // handle the defaul hotkeys
#ifdef _DEBUG
#ifndef __KEKO__
    DXUTCreateWindow( L"Pick" );
#else	// !__KEKO__/__KEKO__
    DXUTCreateWindow( L"━┓━ (keko)" );
#endif	// __KEKO__
#else	// _DEBUG
	DXUTCreateWindow( L"T1" );
#endif	// !_DEBUG
    DXUTCreateDevice( true, 640, 480 );

    // Pass control to DXUT for handling the message pump and
    // dispatching render calls. DXUT will call your FrameMove
    // and FrameRender callback when there is idle time between handling window messages.
    DXUTMainLoop();

    // Perform any application-level cleanup here. Direct3D device resources are released within the
    // appropriate callback functions and therefore don't require any cleanup code here.

#if 1	// into wWinMain
#ifdef USE__pointer_to_PickVar
	OnLostDevice(0);
	OnDestroyDevice(0);
	SAFE_DELETE(gPickVarPtr);
#endif	// USE__pointer_to_PickVar
#endif	// into wWinMain
    return DXUTGetExitCode();
}


//--------------------------------------------------------------------------------------
// Initialize the app
//--------------------------------------------------------------------------------------
void	InitApp()
{
#ifdef USE__pointer_to_PickVar
	if (!gPickVarPtr)
		return;
	PickVar&	gPickVar = *gPickVarPtr;
#endif	// USE__pointer_to_PickVar
#ifdef USE__PickVar
#ifdef _DEBUG
	bool&	g_bUseD3DXIntersect = gPickVar.g_bUseD3DXIntersect;
	bool&	g_bAllHits = gPickVar.g_bAllHits;
#endif
	CDXUTDialogResourceManager&	g_DialogResourceManager = gPickVar.g_DialogResourceManager;
	CD3DSettingsDlg&	g_SettingsDlg = gPickVar.g_SettingsDlg;
	CDXUTDialog&	g_HUD = gPickVar.g_HUD;
	CDXUTDialog&	g_SampleUI = gPickVar.g_SampleUI;
#endif	// USE__PickVar
	cout << "initApp" << endl;
    // Initialize dialogs
    g_SettingsDlg.Init( &g_DialogResourceManager );
    g_HUD.Init( &g_DialogResourceManager );
    g_SampleUI.Init( &g_DialogResourceManager );

    g_HUD.SetCallback( OnGUIEvent ); int iY = 10;
#ifdef _DEBUG
    g_HUD.AddButton( IDC_TOGGLEFULLSCREEN, L"Toggle full screen", 35, iY, 125, 22 );
    g_HUD.AddButton( IDC_TOGGLEREF, L"Toggle REF (F3)", 35, iY += 24, 125, 22 );
    g_HUD.AddButton( IDC_CHANGEDEVICE, L"Change device (F2)", 35, iY += 24, 125, 22, VK_F2 );
    g_HUD.AddCheckBox( IDC_USED3DX, L"Use D3DXIntersect", 35, iY += 24, 125, 22, g_bUseD3DXIntersect, VK_F4 );
    g_HUD.AddCheckBox( IDC_ALLHITS, L"Show All Hits", 35, iY += 24, 125, 22, g_bAllHits, VK_F5 );
#endif	// _DEBUG

    g_SampleUI.SetCallback( OnGUIEvent ); iY = 10;
}


//--------------------------------------------------------------------------------------
// Called during device initialization, this code checks the device for some
// minimum set of capabilities, and rejects those that don't pass by returning false.
//--------------------------------------------------------------------------------------
bool
CALLBACK IsDeviceAcceptable(
	D3DCAPS9*	pCaps,
	D3DFORMAT	AdapterFormat,
	D3DFORMAT	BackBufferFormat,
	bool	bWindowed,
	void*	pUserContext
)
{
////	cout << "IsDeviceAcceptable" << endl;
    // Skip backbuffer formats that don't support alpha blending
    IDirect3D9* pD3D = DXUTGetD3D9Object();
    if( FAILED( pD3D->CheckDeviceFormat( pCaps->AdapterOrdinal, pCaps->DeviceType,
                    AdapterFormat, D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING,
                    D3DRTYPE_TEXTURE, BackBufferFormat ) ) )
        return false;

    // No fallback defined by this app, so reject any device that
    // doesn't support at least ps2.0
    if( pCaps->PixelShaderVersion < D3DPS_VERSION(2,0) )
        return false;

    return true;
}


//--------------------------------------------------------------------------------------
// This callback function is called immediately before a device is created to allow the
// application to modify the device settings. The supplied pDeviceSettings parameter
// contains the settings that the framework has selected for the new device, and the
// application can make any desired changes directly to this structure.  Note however that
// DXUT will not correct invalid device settings so care must be taken
// to return valid device settings, otherwise IDirect3D9::CreateDevice() will fail.
//--------------------------------------------------------------------------------------
bool	CALLBACK ModifyDeviceSettings(DXUTDeviceSettings* pDeviceSettings, void* pUserContext)
{
	cout << "ModifyDeviceSettings" << endl;
    assert( DXUT_D3D9_DEVICE == pDeviceSettings->ver );

    HRESULT hr;
    IDirect3D9* pD3D = DXUTGetD3D9Object();
    D3DCAPS9 caps;

    V( pD3D->GetDeviceCaps( pDeviceSettings->d3d9.AdapterOrdinal,
                            pDeviceSettings->d3d9.DeviceType,
                            &caps ) );

    // If device doesn't support HW T&L or doesn't support 1.1 vertex shaders in HW
    // then switch to SWVP.
    if( ( caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) == 0 ||
         caps.VertexShaderVersion < D3DVS_VERSION(1,1) )
    {
        pDeviceSettings->d3d9.BehaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
    }

    // Debugging vertex shaders requires either REF or software vertex processing
    // and debugging pixel shaders requires REF.
#ifdef DEBUG_VS
    if( pDeviceSettings->d3d9.DeviceType != D3DDEVTYPE_REF )
    {
        pDeviceSettings->d3d9.BehaviorFlags &= ~D3DCREATE_HARDWARE_VERTEXPROCESSING;
        pDeviceSettings->d3d9.BehaviorFlags &= ~D3DCREATE_PUREDEVICE;
        pDeviceSettings->d3d9.BehaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
    }
#endif
#ifdef DEBUG_PS
    pDeviceSettings->d3d9.DeviceType = D3DDEVTYPE_REF;
#endif

    // For the first device created if its a REF device, optionally display a warning dialog box
    static bool s_bFirstTime = true;
    if( s_bFirstTime )
    {
        s_bFirstTime = false;
        if( pDeviceSettings->d3d9.DeviceType == D3DDEVTYPE_REF )
            DXUTDisplaySwitchingToREFWarning( pDeviceSettings->ver );
    }

    return true;
}


//--------------------------------------------------------------------------------------
// This callback function will be called immediately after the Direct3D device has been
// created, which will happen during application initialization and windowed/full screen
// toggles. This is the best location to create D3DPOOL_MANAGED resources since these
// resources need to be reloaded whenever the device is destroyed. Resources created
// here should be released in the OnDestroyDevice callback.
//--------------------------------------------------------------------------------------
//
HRESULT
CALLBACK OnCreateDevice(
	IDirect3DDevice9*	pd3dDevice,
	const D3DSURFACE_DESC*	pBackBufferSurfaceDesc,
	void*	pUserContext
)
{
#ifdef USE__pointer_to_PickVar
	if (!gPickVarPtr)
		return S_OK;
	PickVar&	gPickVar = *gPickVarPtr;
#endif	// USE__pointer_to_PickVar
#ifdef USE__PickVar
#ifndef USE__fileOpen
	kState&	state = gPickVar.state;
#endif	// !USE__fileOpen
#ifdef USE__pointer_to_kMesh
#ifndef USE__fileOpen
	kMesh*&	gkMeshPtr = gPickVar.gkMeshPtr;
	kMesh*&	gkMeshPtr0 = gPickVar.gkMeshPtr0;
#endif	// !USE__fileOpen
#else	// USE__pointer_to_kMesh/!USE__pointer_to_kMesh
	kMesh&	gMesh = gPickVar.gMesh;
	kMesh&	gMesh0 = gPickVar.gMesh0;
#endif	// !USE__pointer_to_kMesh
	ID3DXFont*&	g_pFont = gPickVar.g_pFont;
	CModelViewerCamera2&	g_Camera = gPickVar.g_Camera;
	CDXUTDialogResourceManager&	g_DialogResourceManager = gPickVar.g_DialogResourceManager;
	CD3DSettingsDlg&	g_SettingsDlg = gPickVar.g_SettingsDlg;
#endif	// USE__PickVar
	cout << "OnCreateDevice" << endl;
	HRESULT	hr;

	V_RETURN(g_DialogResourceManager.OnD3D9CreateDevice(pd3dDevice));
	V_RETURN(g_SettingsDlg.OnD3D9CreateDevice(pd3dDevice));

	// Initialize the font
	V_RETURN(D3DXCreateFont(pd3dDevice, 15, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
		L"Arial", &g_pFont));

#if 0	// Voxel
{
	state.points.RemoveAll();
#if 1	// read...
{
	FILE*	filePtr = 0;
	char	filename[] = "voxel.data";
do {
#if defined(_MSC_VER) && (_MSC_VER >= 1400)	// Visual Studio 2005 or above
	errno_t	err = fopen_s(&filePtr, filename, "r");
	assert(err == 0);
	if (err != 0) {
		break;
	}
#else
	filePtr = fopen(filename, "r");
#endif

	int	n_void = 0;
/*
TYPE_SVL_ONLY
1
0.000000 0.000000 0.000000
0
320 800 320 160 400 160
2.500000 0.000000 0.000000 0.000000
472831
0 91 446 9 194 150 121 19.686541 33
......
 */
/* 홍광진 박사 설명에 따르면...
TYPE_SVL_ONLY			// 현재 파일이 박셀 데이터 저장한 파일이라는 것을 알려줌(에트리에서 정의)
1				// 파일 내의 오브젝트(사람) 수(항상 1)
0.000000 0.000000 0.000000	// 월드 좌표계의 원점 좌표 [캘리브레이션 박스의 중심 위치의 좌표]
0				// 오브젝트의 ID
400 720 400 200 400 200		// 오브젝트가 표현되는 공간 크기[칸 수; 그리드 수 - 1] 및 중심점 정의: X, Y, Z 축 크기, 중심점의 X, Y, Z좌표[그리드 인덱스]
2.500000 0.000000 0.000000 0.000000	// voxel volume의 voxel size(단위: mm)[각 그리드의 교차점에 놓이는 정육면체 한 변의 길이; 단위 복셀의 실제 길이], 박셀 공간 내에서의 월드 좌표계 중심점의 좌표 [캘리브레이션 박스 중심에 대한 상대좌표]
254465				// 박셀 수
0 322 303 149 132 91 73 20.229338 28	// 오브젝트 ID, voxel의 x, y, z좌표, voxel의 R, G, B 값, voxel의 carving test 이전의 maxwell prob값, 해당 voxel의 입력 다시점 영상에 대한 visibility를 가지는 pixel count
 */
	int	n_voxel;
	int	n_count = 0;
	while (1) {
		int	rval;
		char	line[MAX_PATH];			// TO DO LIST: 충분히 큰가?
		if (!fgets(line, MAX_PATH, filePtr))
				break;
//		if ((rval = fscanf_s(filePtr, "%s", command, MAX_PATH)) == 0 || rval == EOF) {
//			break;
//		}
		if (++n_void < 7)
			(void) fprintf(stdout, "|%s|\n", line);
		else if (n_void == 7) {
/*
472831
 */
			if ((rval = sscanf_s(line, "%d", &n_voxel, sizeof(int))) == 0 || rval == EOF) {
				break;
			}
			(void) fprintf(stdout, "# of voxel(s) = %d\n", n_voxel);
		} else {
/*
0 91 446 9 194 150 121 19.686541 33
......
 */
			int	id;
			int	nx, ny, nz;
			int	nr, ng, nb;
			if ((rval = sscanf_s(line,
				"%d %d %d %d %d %d %d",
				&id,
				&nx, &ny, &nz,
				&nr, &ng, &nb
			)) == 0 || rval == EOF) {
				break;
			}
//			(void) fprintf(stdout, "[%d, %d, %d] w/ [%d, %d, %d]\n", nx, ny, nz, nr, ng, nb);
			++n_count;
#if 0
			if (n_count < 10) {
				(void) fprintf(stdout, "%s\n", line);
				(void) fprintf(stdout, "[%d, %d, %d] w/ [%d, %d, %d]\n", nx, ny, nz, nr, ng, nb);
			}
#endif
			float	scale = 2.5f;
			D3DXVECTOR3	P(scale * nx, scale * ny, scale * nz);
			state.points.Add(P);
		}
	}
	assert(n_voxel == n_count);
} while (0);
	if (filePtr) {
		(void) fclose(filePtr);
		filePtr = 0;
	}
}
#endif	// read...
}
#endif	// Voxel
#ifdef __KEKO__
#ifdef USE__pointer_to_kMesh
	cout << "gkMeshPtr = " << gkMeshPtr << endl;
	if (gkMeshPtr) {
		cout << "delete kMesh" << endl;
		delete gkMeshPtr;
		gkMeshPtr = 0;
	}
	cout << "new kMesh" << endl;
	gkMeshPtr = new kMesh;
	kMesh&	gMesh = *gkMeshPtr;
	cout << "gkMeshPtr0 = " << gkMeshPtr0 << endl;
	if (gkMeshPtr0) {
		cout << "delete kMesh0" << endl;
		delete gkMeshPtr0;
		gkMeshPtr0 = 0;
	}
	cout << "new kMesh0" << endl;
	gkMeshPtr0 = new kMesh;
#if	defined(USE__fileOpen) || (defined(__READ_FILES__) && defined(MODEL_TYPE) && MODEL_TYPE == BOTH_MODEL)
	kMesh&	gMesh0 = *gkMeshPtr0;
#endif	// USE__fileOpen || __READ_FILES__
#endif
	gMesh.setAll(
		0,
		&state,
		L"./Resources/tiny.x", pd3dDevice, L"./Resources/mesh.fx",
		1.f,
#ifdef __READ_FILES__
		false,
#endif
		"./Resources/keko-model.obj",
		L"./Resources/",
		DXUTGetHWND()
	);
#else	// __KEKO__/!__KEKO__
#ifdef USE__fileOpen
#if 1	// logo
	if (
		(gPickVar.setAllArg.isSet && gPickVar.setAllArg.connectable != CONNECTABLE_LOGO) ||
		(gPickVar.setAllArg0.isSet && gPickVar.setAllArg0.connectable != CONNECTABLE_LOGO)
	) {
		(void) gPickVar.OnCreateDevice(pd3dDevice);
	} else if (1) {
#ifndef __WRITE_FILES__
		cout << "F8: 로고" << endl;

		// 파라미터 설정
		kMesh::SetAllArg	arg;
#ifdef STANDARD_MODEL
		arg.index_to_standard	= 2;
#endif	// STANDARD_MODEL
		arg.statePtr		= &gPickVar.state;
		arg.xSourceFilePtr	= L"Media\\Scanner\\tiny.x";
		arg.devicePtr		= DXUTGetD3D9Device();	// OnCreateDevice 에서 새로운 값으로 덮어써야 한다.
		arg.effectSourceFilePtr	= L"../include/mesh/mesh.fx";
		arg.fScale 		= 1.f;
#ifdef __READ_FILES__
		arg.stillRead		= true;
#endif	// __READ_FILES__
		arg.objFilePtr		= "Media\\Scanner\\logo.obj";	// 에트리 측정모델(2)
		arg.texturePath		= NULL;
		arg.hWnd		= DXUTGetHWND();

		// 로드
		CONNECTABLE_TYPE_T	connectable = CONNECTABLE_LOGO;
		gPickVar.load(MODEL_TYPE__REAL, arg, connectable);

		// 카메라 설정
		SetCameraArcBall();
#endif	// __WRITE_FILES__
	}
#else	// logo/!logo
	(void) gPickVar.OnCreateDevice(pd3dDevice);
#endif	// !logo
#else	// USE__fileOpen/!USE__fileOpen
#ifdef USE__pointer_to_kMesh
	cout << "gkMeshPtr = " << gkMeshPtr << endl;
	if (gkMeshPtr) {
		cout << "delete kMesh" << endl;
		delete gkMeshPtr;
		gkMeshPtr = 0;
	}
	cout << "new kMesh" << endl;
	gkMeshPtr = new kMesh;
	kMesh&	gMesh = *gkMeshPtr;
	cout << "gkMeshPtr0 = " << gkMeshPtr0 << endl;
	if (gkMeshPtr0) {
		cout << "delete kMesh0" << endl;
		delete gkMeshPtr0;
		gkMeshPtr0 = 0;
	}
	cout << "new kMesh0" << endl;
	gkMeshPtr0 = new kMesh;
#if	defined(USE__fileOpen) || (defined(__READ_FILES__) && defined(MODEL_TYPE) && MODEL_TYPE == BOTH_MODEL)
	kMesh&	gMesh0 = *gkMeshPtr0;
#endif	// USE__fileOpen || __READ_FILES__
#endif
{
	// 모델을 읽어들인다.
#if defined(STANDARD_MODEL)// && (STANDARD_MODEL == DAZ_STUDIO_STANDARD_MODEL || STANDARD_MODEL == ETRI_STANDARD_MODEL)
#if STANDARD_MODEL == ETRI_STANDARD_MODEL	// 에트리 모델
#if defined(MODEL_TYPE) && MODEL_TYPE == BOTH_MODEL	// 모델비교
#pragma message("ETRI: BOTH MODEL")
#pragma message("    REAL MODEL")
	// 측정모델
	gMesh.setAll(
		1,
		&state,
		L"Media\\Scanner\\tiny.x", pd3dDevice, L"../include/mesh/mesh.fx",
		1.f / .8f,
#ifdef __READ_FILES__
		true,
#endif
		"Media\\Scanner\\여자모델_Vic090609\\EW_1RH.obj",
		L"Media\\Scanner\\여자모델_Vic090609\\tex\\",
		DXUTGetHWND()
	);
#if 1
#pragma message("    STANDARD MODEL")
	// 표준모델
	gMesh0.setAll(
		1,
		&state,
		L"Media\\Scanner\\tiny.x", pd3dDevice, L"../include/mesh/mesh.fx",
		1.f,
#ifdef __READ_FILES__
		false,
#endif
		"Media\\Scanner\\여자모델_Vic090609\\EW_0RH.obj",
		L"Media\\Scanner\\여자모델_Vic090609\\tex\\",
		DXUTGetHWND()
	);
	gMesh.meshPtr0() = &gMesh0;
#endif
#elif defined(MODEL_TYPE) && MODEL_TYPE == REAL_MODEL	// 측정 모델
#pragma message("ETRI: REAL MODEL")
	gMesh.setAll(
		1,
		&state,
		L"Media\\Scanner\\tiny.x", pd3dDevice, L"../include/mesh/mesh.fx",
		1.f,
#ifdef __READ_FILES__
		true,
#endif
		"Media\\Scanner\\여자모델_Vic090609\\EW_1RH.obj",
		L"Media\\Scanner\\여자모델_Vic090609\\tex\\",
		DXUTGetHWND()
	);
#else	// 표준모델
#pragma message("ETRI: STANDARD MODEL")
	gMesh.setAll(
		1,
		&state,
		L"Media\\Scanner\\tiny.x", pd3dDevice, L"../include/mesh/mesh.fx",
		1.f,
#ifdef __READ_FILES__
		false,
#endif
#if 0	// 에트리에서 가져온 원본 .OBJ
		"Media\\Scanner\\여자모델_Vic090609\\Victoria_0609_rearrange.obj",
#else			// 마야에서 손본 .OBJ
		"Media\\Scanner\\여자모델_Vic090609\\EW_0RH.obj",
#endif
		L"Media\\Scanner\\여자모델_Vic090609\\tex\\",
		DXUTGetHWND()
	);
#endif
#elif STANDARD_MODEL == ETRI_STANDARD_MODEL2
#if defined(MODEL_TYPE) && MODEL_TYPE == BOTH_MODEL	// 모델비교
#pragma message("ETRI2: BOTH MODEL")
#pragma message("    REAL MODEL")
	// 측정모델
	gMesh.setAll(
		3,
		&state,
		L"Media\\Scanner\\tiny.x", pd3dDevice, L"../include/mesh/mesh.fx",
		1.f,
#ifdef __READ_FILES__
		true,
#endif
//		"Media\\Scanner\\여자모델_Vic090609\\EW_1RH.obj",
//		"Media\\Scanner\\여자모델_Vic090609\\female_04_001.obj",
		"Media\\Scanner\\여자모델_Vic090609\\fem_fat1.obj",
//		"Media\\Scanner\\여자모델_Vic090609\\fem_fat2.obj",
		NULL,//L"Media\\Scanner\\여자모델_Vic090609\\tex\\",
		DXUTGetHWND()
	);
#if 1
#pragma message("    STANDARD MODEL")
	// 표준모델
	gMesh0.setAll(
		3,
		&state,
		L"Media\\Scanner\\tiny.x", pd3dDevice, L"../include/mesh/mesh.fx",
		1.f,
#ifdef __READ_FILES__
		false,
#endif
//		"Media\\Scanner\\여자모델_Vic090609\\EW_0RH.obj",
		"Media\\Scanner\\여자모델_Vic090609\\female_04_001.obj",
//		"Media\\Scanner\\여자모델_Vic090609\\fem_fat1.obj",
//		"Media\\Scanner\\여자모델_Vic090609\\fem_fat2.obj",
		NULL,//L"Media\\Scanner\\여자모델_Vic090609\\tex\\",
		DXUTGetHWND()
	);
	gMesh.meshPtr0() = &gMesh0;
#endif
#elif defined(MODEL_TYPE) && MODEL_TYPE == REAL_MODEL	// 측정 모델
#pragma message("ETRI: REAL MODEL")
	gMesh.setAll(
		3,
		&state,
		L"Media\\Scanner\\tiny.x", pd3dDevice, L"../include/mesh/mesh.fx",
		1.f,
#ifdef __READ_FILES__
		true,
#endif
//		"Media\\Scanner\\여자모델_Vic090609\\EW_1RH.obj",
//		"Media\\Scanner\\여자모델_Vic090609\\female_04_001.obj",
		"Media\\Scanner\\여자모델_Vic090609\\fem_fat1.obj",
//		"Media\\Scanner\\여자모델_Vic090609\\fem_fat2.obj",
		NULL,//L"Media\\Scanner\\여자모델_Vic090609\\tex\\",
		DXUTGetHWND()
	);
#else	// 표준모델
#pragma message("ETRI: STANDARD MODEL")
	gMesh.setAll(
		3,
		&state,
		L"Media\\Scanner\\tiny.x", pd3dDevice, L"../include/mesh/mesh.fx",
		1.f,
#ifdef __READ_FILES__
		false,
#endif
#if 0	// 에트리에서 가져온 원본 .OBJ
		"Media\\Scanner\\여자모델_Vic090609\\Victoria_0609_rearrange.obj",
#else			// 마야에서 손본 .OBJ
//		"Media\\Scanner\\여자모델_Vic090609\\EW_0RH.obj",
		"Media\\Scanner\\여자모델_Vic090609\\female_04_001.obj",
//		"Media\\Scanner\\여자모델_Vic090609\\fem_fat1.obj",
//		"Media\\Scanner\\여자모델_Vic090609\\fem_fat2.obj",
#endif
		NULL,//L"Media\\Scanner\\여자모델_Vic090609\\tex\\",
		DXUTGetHWND()
	);
#endif
#elif STANDARD_MODEL == ETRI_STANDARD_MODEL3
#if defined(MODEL_TYPE) && MODEL_TYPE == BOTH_MODEL	// 모델비교
#pragma message("ETRI3: BOTH MODEL")
#pragma message("    REAL MODEL")
	// 측정모델
	gMesh.setAll(
		4,
		&state,
		L"Media\\Scanner\\tiny.x", pd3dDevice, L"../include/mesh/mesh.fx",
		1.f,
#ifdef __READ_FILES__
		true,
#endif
//		"Media\\Scanner\\여자모델_Vic090609\\EW_1RH.obj",
		"Media\\Scanner\\여자모델_Vic090609\\hong1103.obj",
		NULL,//L"Media\\Scanner\\여자모델_Vic090609\\tex\\",
		DXUTGetHWND()
	);
#if 1
#pragma message("    STANDARD MODEL")
	// 표준모델
	gMesh0.setAll(
		4,
		&state,
		L"Media\\Scanner\\tiny.x", pd3dDevice, L"../include/mesh/mesh.fx",
		1.f,
#ifdef __READ_FILES__
		false,
#endif
//		"Media\\Scanner\\여자모델_Vic090609\\EW_0RH.obj",
		"Media\\Scanner\\여자모델_Vic090609\\male06.obj",
		NULL,//L"Media\\Scanner\\여자모델_Vic090609\\tex\\",
		DXUTGetHWND()
	);
	gMesh.meshPtr0() = &gMesh0;
#endif
#elif defined(MODEL_TYPE) && MODEL_TYPE == REAL_MODEL	// 측정 모델
#pragma message("ETRI: REAL MODEL")
	gMesh.setAll(
		4,
		&state,
		L"Media\\Scanner\\tiny.x", pd3dDevice, L"../include/mesh/mesh.fx",
		1.f,
#ifdef __READ_FILES__
		true,
#endif
//		"Media\\Scanner\\여자모델_Vic090609\\EW_1RH.obj",
		"Media\\Scanner\\여자모델_Vic090609\\hong1103.obj",
		NULL,//L"Media\\Scanner\\여자모델_Vic090609\\tex\\",
		DXUTGetHWND()
	);
#else	// 표준모델
#pragma message("ETRI: STANDARD MODEL")
	gMesh.setAll(
		4,
		&state,
		L"Media\\Scanner\\tiny.x", pd3dDevice, L"../include/mesh/mesh.fx",
		1.f,
#ifdef __READ_FILES__
		false,
#endif
#if 0	// 에트리에서 가져온 원본 .OBJ
		"Media\\Scanner\\여자모델_Vic090609\\Victoria_0609_rearrange.obj",
#else			// 마야에서 손본 .OBJ
//		"Media\\Scanner\\여자모델_Vic090609\\EW_0RH.obj",
		"Media\\Scanner\\여자모델_Vic090609\\male06.obj",
#endif
		NULL,//L"Media\\Scanner\\여자모델_Vic090609\\tex\\",
		DXUTGetHWND()
	);
#endif
#elif STANDARD_MODEL == ETRI_STANDARD_MODEL6	// 에트리표준모델-091211(남: michael)
#if defined(MODEL_TYPE) && MODEL_TYPE == BOTH_MODEL	// 모델비교
#pragma message("ETRI6: Michael: BOTH MODEL")
#pragma message("    REAL MODEL")
	// 측정모델
	gMesh.setAll(
		6,
		&state,
		L"Media\\Scanner\\tiny.x", pd3dDevice, L"../include/mesh/mesh.fx",
		1.f,
#ifdef __READ_FILES__
		true,
#endif
		"Media\\Scanner\\에트리표준모델-091211\\michael\\1207_Male_A.obj",
		L"Media\\Scanner\\에트리표준모델-091211\\michael\\tex\\",
		DXUTGetHWND()
	);
#if 1
#pragma message("    STANDARD MODEL")
	// 표준모델
	gMesh0.setAll(
		6,
		&state,
		L"Media\\Scanner\\tiny.x", pd3dDevice, L"../include/mesh/mesh.fx",
		1.f,
#ifdef __READ_FILES__
		false,
#endif
		"Media\\Scanner\\에트리표준모델-091211\\michael\\michael.obj",
		L"Media\\Scanner\\에트리표준모델-091211\\michael\\tex\\",
		DXUTGetHWND()
	);
	gMesh.meshPtr0() = &gMesh0;
#endif
#elif defined(MODEL_TYPE) && MODEL_TYPE == REAL_MODEL	// 측정 모델
#pragma message("ETRI6: Michael: REAL MODEL")	// A(민이사), B(박노준), C(김이사), D(헬쓰맨)
{
#ifdef __READ_FILES__
	LPCWSTR texture_names[1]={
//		L"1228_Male_A.bmp",
		L"hyuk100113.bmp",
	};
	LPCWSTR_ARRAY	textureArray = {texture_names, 1};
#endif	// __READ_FILES__
	gMesh.setAll(
		6,
		&state,
		L"Media\\Scanner\\tiny.x", pd3dDevice, L"../include/mesh/mesh.fx",
		1.f,
#ifdef __READ_FILES__
		true,
#endif
//		"Media\\Scanner\\에트리표준모델-091211\\michael\\1228_Male_A.obj",
		"Media\\Scanner\\에트리표준모델-091211\\michael\\hyuk100113.obj",
		L"Media\\Scanner\\에트리표준모델-091211\\michael\\tex\\",
		DXUTGetHWND()
#ifdef __READ_FILES__
		,
		&textureArray
#endif	// __READ_FILES__
	);
}
#else	// 표준모델
#pragma message("ETRI6: Michael: STANDARD MODEL")
	gMesh.setAll(
		6,
		&state,
		L"Media\\Scanner\\tiny.x", pd3dDevice, L"../include/mesh/mesh.fx",
		1.f,
#ifdef __READ_FILES__
		false,
#endif
		"Media\\Scanner\\에트리표준모델-091211\\michael\\michael.obj",
		L"Media\\Scanner\\에트리표준모델-091211\\michael\\tex\\",
		DXUTGetHWND()
	);
#endif
#elif STANDARD_MODEL == ETRI_STANDARD_MODEL7	// 에트리표준모델-091211(여: victoria)
#if defined(MODEL_TYPE) && MODEL_TYPE == BOTH_MODEL	// 모델비교
#pragma message("ETRI7: Victoria: BOTH MODEL")
#pragma message("    REAL MODEL")
	// 측정모델
	gMesh.setAll(
		7,
		&state,
		L"Media\\Scanner\\tiny.x", pd3dDevice, L"../include/mesh/mesh.fx",
		1.f,
#ifdef __READ_FILES__
		true,
#endif
		"Media\\Scanner\\에트리표준모델-091211\\victoria\\1207_Female_A.obj",
		L"Media\\Scanner\\에트리표준모델-091211\\victoria\\tex\\",
		DXUTGetHWND()
	);
#if 1
#pragma message("    STANDARD MODEL")
	// 표준모델
	gMesh0.setAll(
		7,
		&state,
		L"Media\\Scanner\\tiny.x", pd3dDevice, L"../include/mesh/mesh.fx",
		1.f,
#ifdef __READ_FILES__
		false,
#endif
		"Media\\Scanner\\에트리표준모델-091211\\victoria\\victoria.obj",
		L"Media\\Scanner\\에트리표준모델-091211\\victoria\\tex\\",
		DXUTGetHWND()
	);
	gMesh.meshPtr0() = &gMesh0;
#endif
#elif defined(MODEL_TYPE) && MODEL_TYPE == REAL_MODEL	// 측정 모델
#pragma message("ETRI7: Victoria: REAL MODEL")	// A(애엄마), B(납짝), C(뚱뚱), E(표준1), F(표준2)
{
#ifdef __READ_FILES__
	LPCWSTR texture_names[1]={
		L"1228_Female_F.bmp",
	};
	LPCWSTR_ARRAY	textureArray = {texture_names, 1};
#endif	// __READ_FILES__
	gMesh.setAll(
		7,
		&state,
		L"Media\\Scanner\\tiny.x", pd3dDevice, L"../include/mesh/mesh.fx",
		1.f,
#ifdef __READ_FILES__
		true,
#endif
		"Media\\Scanner\\에트리표준모델-091211\\victoria\\1228_Female_E.obj",
		L"Media\\Scanner\\에트리표준모델-091211\\victoria\\tex\\",
		DXUTGetHWND()
#ifdef __READ_FILES__
		,
		&textureArray
#endif	// __READ_FILES__
	);
}
#else	// 표준모델
#pragma message("ETRI7: Victoria: STANDARD MODEL")
	gMesh.setAll(
		7,
		&state,
		L"Media\\Scanner\\tiny.x", pd3dDevice, L"../include/mesh/mesh.fx",
		1.f,
#ifdef __READ_FILES__
		false,
#endif
		"Media\\Scanner\\에트리표준모델-091211\\victoria\\victoria.obj",
		L"Media\\Scanner\\에트리표준모델-091211\\victoria\\tex\\",
		DXUTGetHWND()
	);
#endif
#elif STANDARD_MODEL == DAZ_STUDIO_STANDARD_MODEL	// DAZ Studio 모델
#if defined(MODEL_TYPE) && MODEL_TYPE == BOTH_MODEL	// 모델비교
#pragma message("DAZ Studio: BOTH MODEL")
#pragma message("    REAL MODEL")
	// 측정모델
	gMesh.setAll(
		0,
		&state,
		L"Media\\Scanner\\tiny.x", pd3dDevice, L"../include/mesh/mesh.fx",
		10.f / .8f,
#ifdef __READ_FILES__
		true,
#endif
		"Media\\Scanner\\DW_1RH.obj",
		NULL,	// 텍스쳐 사용 안 함
		DXUTGetHWND()
	);
#if 1
#pragma message("    STANDARD MODEL")
	// 표준모델
	gMesh0.setAll(
		0,
		&state,
		L"Media\\Scanner\\tiny.x", pd3dDevice, L"../include/mesh/mesh.fx",
		10.f,
#ifdef __READ_FILES__
		false,
#endif
		"Media\\Scanner\\DW_0RH.obj",
		NULL,	// 텍스쳐 사용 안 함
		DXUTGetHWND()
	);
	gMesh.meshPtr0() = &gMesh0;
#endif
#elif defined(MODEL_TYPE) && MODEL_TYPE == REAL_MODEL	// 측정 모델
#pragma message("DAZ Studio: REAL MODEL")
	gMesh.setAll(
		0,
		&state,
		L"Media\\Scanner\\tiny.x", pd3dDevice, L"../include/mesh/mesh.fx",
		10.f,
#ifdef __READ_FILES__
		true,
#endif
		"Media\\Scanner\\DW_1RH.obj",
		NULL,	// 텍스쳐 사용 안 함
		DXUTGetHWND()
	);
#else	// 모델테스트
#pragma message("DAZ Studio: STANDARD MODEL")
	// 표준모델
	gMesh.setAll(
		0,
		&state,
		L"Media\\Scanner\\tiny.x", pd3dDevice, L"../include/mesh/mesh.fx",
		10.f,
#ifdef __READ_FILES__
		false,
#endif
		"Media\\Scanner\\DW_0RH.obj",	// 표준; EW_0RH.obj 을 마야 2008에서 읽어서 새로 썼을 뿐인데 Vertex 갯수가 다르다. (크기를 변화해서 새로 쓴 DW_1RH.obj 와는 같은 Vertex 갯수)
						// 이름 변경: DW_2RH.obj -> DW_0RH.obj
//		"Media\\Scanner\\DW_0RH.obj",	// 이상하다...
		NULL,	// 텍스쳐 사용 안 함
		DXUTGetHWND()
	);
#endif
#else	// 기타 .obj
#pragma message("STANDARD_MODEL: UNDEF")
	gMesh.setAll(
		2,
		&state,
		L"Media\\Scanner\\tiny.x", pd3dDevice, L"../include/mesh/mesh.fx",
		1.f,
#ifdef __READ_FILES__
		false,
#endif
//		"Media\\Scanner\\logo.obj",
//		"Media\\Scanner\\여자모델_Vic090609\\female_04_001.obj",
		"Media\\Scanner\\여자모델_Vic090609\\male06.obj",
//		"Media\\Scanner\\여자모델_Vic090609\\fem_fat1.obj",
//		"Media\\Scanner\\여자모델_Vic090609\\fem_fat2.obj",
		NULL,
		DXUTGetHWND()
	);
#endif
#else
#pragma message("TINY MODEL")
//	gMesh.setAll(&state, L"Media\\Scanner\\ScannerArm.x", pd3dDevice, L"../include/mesh/mesh.fx", 1.f, NULL);
	gMesh.setAll(
		&state,
		L"Media\\Scanner\\tiny.x", pd3dDevice, L"../include/mesh/mesh.fx",
		10.f, NULL,
		NULL,	// 따로 텍스쳐 사용 안 함
		DXUTGetHWND()
	);
#endif

	// 둘레측정에 사용할 단면을 결정하는 법선벡터의 길이를 1로 만든다.
	D3DXVec3Normalize(&state.g__n, &state.g__n);
}
#endif	// !USE__fileOpen
#endif	// !__KEKO__


	// Define DEBUG_VS and/or DEBUG_PS to debug vertex and/or pixel shaders with the
	// shader debugger. Debugging vertex shaders requires either REF or software vertex
	// processing, and debugging pixel shaders requires REF.  The
	// D3DXSHADER_FORCE_*_SOFTWARE_NOOPT flag improves the debug experience in the
	// shader debugger.  It enables source level debugging, prevents instruction
	// reordering, prevents dead code elimination, and forces the compiler to compile
	// against the next higher available software target, which ensures that the
	// unoptimized shaders do not exceed the shader model limitations.  Setting these
	// flags will cause slower rendering since the shaders will be unoptimized and
	// forced into software.  See the DirectX documentation for more information about
	// using the shader debugger.
#if defined(DEBUG) || defined(_DEBUG) || defined(DEBUG_VS) || defined(DEBUG_PS)
	DWORD	dwShaderFlags = D3DXFX_NOT_CLONEABLE;
#endif

#if defined( DEBUG ) || defined( _DEBUG )
	// Set the D3DXSHADER_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows
	// the shaders to be optimized and to run exactly the way they will run in
	// the release configuration of this program.
	dwShaderFlags |= D3DXSHADER_DEBUG;
#endif

#ifdef DEBUG_VS
	dwShaderFlags |= D3DXSHADER_FORCE_VS_SOFTWARE_NOOPT;
#endif
#ifdef DEBUG_PS
	dwShaderFlags |= D3DXSHADER_FORCE_PS_SOFTWARE_NOOPT;
#endif

	//source blend factor
	DXUTGetD3D9Device()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCCOLOR);	// D3DBLEND_SRCALPHA
	//destination blend factor
	DXUTGetD3D9Device()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR);	// D3DBLEND_INVSRCALPHA

	// Setup the camera's view parameters
	D3DXVECTOR3	vecEye(-CAMERA_DISTANCE, 0.0f, 0.0f);
	D3DXVECTOR3	vecAt(0.0f, 0.0f, 0.0f);
	g_Camera.SetViewParams(&vecEye, &vecAt);

	// Setup the world matrix of the camera
	// Change this to see how Picking works with a translated object
	D3DXMATRIX	mWorld;
	D3DXMatrixTranslation(&mWorld, 0.0f, -1.7f, 0.0f);
	g_Camera.SetWorldMatrix(mWorld);

	return S_OK;
}


//--------------------------------------------------------------------------------------
// This callback function will be called immediately after the Direct3D device has been
// reset, which will happen after a lost device scenario. This is the best location to
// create D3DPOOL_DEFAULT resources since these resources need to be reloaded whenever
// the device is lost. Resources created here should be released in the OnLostDevice
// callback.
//--------------------------------------------------------------------------------------
HRESULT
CALLBACK OnResetDevice(
	IDirect3DDevice9*	pd3dDevice,
	const D3DSURFACE_DESC*	pBackBufferSurfaceDesc,
	void*	pUserContext
)
{
#ifdef USE__pointer_to_PickVar
	if (!gPickVarPtr)
		return S_OK;
	PickVar&	gPickVar = *gPickVarPtr;
#endif	// USE__pointer_to_PickVar
#ifdef USE__PickVar
	ID3DXFont*&	g_pFont = gPickVar.g_pFont;
	ID3DXSprite*&	g_pTextSprite = gPickVar.g_pTextSprite;
	ID3DXEffect*&	g_pEffect = gPickVar.g_pEffect;
	CDXUTDialogResourceManager&	g_DialogResourceManager = gPickVar.g_DialogResourceManager;
	CD3DSettingsDlg&	g_SettingsDlg = gPickVar.g_SettingsDlg;
	CDXUTDialog&	g_HUD = gPickVar.g_HUD;
	CDXUTDialog&	g_SampleUI = gPickVar.g_SampleUI;
#endif	// USE__PickVar
	cout << "OnResetDevice" << endl;
	HRESULT hr;

	g_DialogResourceManager.OnD3D9LostDevice();	// OnLostDevice 전에 두 번 이상 호출될 수 있으므로;
							//	IDirect3DStateBlock9*	m_pStateBlock
							// CDXUTDialogResourceManager::OnD3D9ResetDevice 에서 할당
							// CDXUTDialogResourceManager::OnD3D9LostDevice 에서 해제
	V_RETURN(g_DialogResourceManager.OnD3D9ResetDevice());
	g_SettingsDlg.OnD3D9LostDevice();		// OnLostDevice 전에 두 번 이상 호출될 수 있으므로;
							//	IDirect3DStateBlock9*	m_pStateBlock
							// CD3DSettingsDlg::OnD3D9ResetDevice 에서 할당
							// CD3DSettingsDlg::OnD3D9LostDevice 에서 해제
	V_RETURN(g_SettingsDlg.OnD3D9ResetDevice());

	if (g_pFont)
		V_RETURN(g_pFont->OnResetDevice());
	if (g_pEffect)
		V_RETURN(g_pEffect->OnResetDevice());

	// Create a sprite to help batch calls when drawing many lines of text
	SAFE_RELEASE(g_pTextSprite);			// OnLostDevice 전에 두 번 이상 호출될 수 있으므로;
	V_RETURN(D3DXCreateSprite(pd3dDevice, &g_pTextSprite));

	g_HUD.SetLocation(pBackBufferSurfaceDesc->Width - 170, 0);
	g_HUD.SetSize(170, 170);
	g_SampleUI.SetLocation(pBackBufferSurfaceDesc->Width - 170, pBackBufferSurfaceDesc->Height - 350);
	g_SampleUI.SetSize(170, 300);

	SetCameraArcBall();

	return S_OK;
}
// OnResetDevice 중에서, kMesh 로부터 g_Camera, g_ArcBall 을 설정하는 부분만 따로 떼어서
// kMesh 의 Instance 를 만들 때마다 사용한다.
void	SetCameraArcBall()
{
#ifdef USE__pointer_to_PickVar
	if (!gPickVarPtr)
		return;
	PickVar&	gPickVar = *gPickVarPtr;
#endif	// USE__pointer_to_PickVar
#ifdef USE__PickVar
	kState&	state = gPickVar.state;
	CD3DArcBall&	g_ArcBall = gPickVar.g_ArcBall;
#ifdef USE__pointer_to_kMesh
	kMesh*&	gkMeshPtr = gPickVar.gkMeshPtr;
#else	// USE__pointer_to_kMesh/!USE__pointer_to_kMesh
	kMesh&		gMesh = gPickVar.gMesh;
#endif	// !USE__pointer_to_kMesh
	CModelViewerCamera2&	g_Camera = gPickVar.g_Camera;
#endif	// USE__PickVar
#ifdef USE__pointer_to_kMesh
	if (!gkMeshPtr)
		return;
	kMesh&	gMesh = *gkMeshPtr;
#endif	// USE__pointer_to_kMesh

	gMesh.OnResetDevice();

	const D3DSURFACE_DESC*	pBackBufferSurfaceDesc = DXUTGetD3D9BackBufferSurfaceDesc();
	// Setup the projection matrix
	float	fAspect = (float) pBackBufferSurfaceDesc->Width / (float) pBackBufferSurfaceDesc->Height;
	g_Camera.SetProjParams(state.FOVY, fAspect, gMesh.radius() / 64.0f, gMesh.radius() * 200.0f);

	// Setup the arcball parameters
	g_ArcBall.Reset();	// 모든 파라미터를 초기화한다;
				// 하지만 m_fRadius 와 m_fRadiusTranslation 을 읽어들인 오브젝트에 맞게...
	g_ArcBall.SetWindow(pBackBufferSurfaceDesc->Width, pBackBufferSurfaceDesc->Height, 0.85f);
	cout << "SetCameraArcBall: CD3DArcBall::SetWindow: (" << pBackBufferSurfaceDesc->Width << ", " << pBackBufferSurfaceDesc->Height << ", .85)" << endl;
	g_ArcBall.SetTranslationRadius(gMesh.radius());
	g_Camera.SetWindow(pBackBufferSurfaceDesc->Width, pBackBufferSurfaceDesc->Height);
}


//--------------------------------------------------------------------------------------
// This callback function will be called once at the beginning of every frame. This is the
// best location for your application to handle updates to the scene, but is not
// intended to contain actual rendering calls, which should instead be placed in the
// OnFrameRender callback.
//--------------------------------------------------------------------------------------
#if 1
/*
 * H: 스크린의 높이
 * W: 스크린의 넓이
 * A: 화면의 비율(aspect ratio) ≡ W/H
 * h: 카메라에서 봤을 때 물체의 높이
 * w: 카메라에서 봤을 때 물체의 넓이
 * a: 카메라에서 봤을 때 물체의 비율 ≡ w/h
 *
 * 스크린에 물체가 꽉 차도록 하기 위해서 카메라의 화면 높이, Hc는?
 * 1) h > w (a < 1): 물체의 높이가 넓이보다 클 때 (위로 길쭉한 모양)
 *		┌┐
 *	물체:	││
 *		└┘
 *	i) H > W (A < 1): 스크린의 높이가 넓이보다 클 때 (위로 길쭉한 모양)
 *			┌┐
 *		스크린:	││
 *			└┘
 *		① a < A < 1: 물체가 스크린보다 더 길쭉한 모양일 때
 *			┌───┐
 *			│┌┐  │
 *			│││  │
 *			│││  │
 *			│└┘  │
 *			└───┘
 *			h 를 H 에 맞춘다;
 *			물체가 상대적으로 길기 때문에 w 를 W 에 맞추면 물체의 상하가 스크린을 벗어난다.
 *
 *			∴ Hc = h
 *
 *		② A < a < 1: 스크린이 물체보다 더 길쭉한 모양일 때 (물체가 상대적으로 뚱뚱하다)
 *			┌──┐
 *			│    │
 *			│┌┐│
 *			││││
 *			│└┘│
 *			└──┘
 *			w 를 W 에 맞춘다;
 *			물체가 상대적으로 뚱뚱하므로 h 를 H 에 맞추면 물체의 좌우가 스크린을 벗어난다.
 *
 *			∴ Hc = w/a
 *
 *	ii) H < W (A < 1): 스크린의 넓이가 높이보다 클 때 (옆으로 퍼진 모양)
 *		스크린:	┌─┐
 *			└─┘
 *		h 를 H 에 맞춘다; 물체의 좌우여백이 넉넉하다.
 *		∴ Hc = h
 * 2) h < w (a > 1): 몰체의 넓이가 높이보다 클 때 (옆으로 퍼진 모양)
 *	물체:	┌─┐
 *		└─┘
 *	i) H > W (A < 1): 스크린의 높이가 넓이보다 클 때 (위로 길쭉한 모양)
 *			┌┐
 *		스크린:	││
 *			└┘
 *		w 를 W 에 맞춘다; 물체의 상하여백이 넉넉하다.
 *		∴ Hc = w/a
 *	ii) H < W (A > 1): 스크린의 넓이가 높이보다 클 때 (옆으로 퍼진 모양)
 *		스크린:	┌─┐
 *			└─┘
 *		① a > A > 1: 물체가 스크린보다 더 퍼진 모양일 때
 *			┌─────┐
 *			│          │
 *			│┌───┐│
 *			││      ││
 *			│└───┘│
 *			└─────┘
 *			w 를 W 에 맞춘다;
 *			물체가 상대적으로 뚱뚱하므로 h 를 H 에 맞추면 물체의 좌우가 스크린을 벗어난다.
 *			∴ Hc = w/a
 *		② A > a > 1: 스크린이 물체보다 더 퍼진 모양일 때 (물체가 상대적으로 상하로 길다)
 *			┌──────┐
 *			│┌───┐  │
 *			││      │  │
 *			│└───┘  │
 *			└──────┘
 *			h 를 H 에 맞춘다;
 *			물체가 상대적으로 길기때문에 w 를 W 에 맞추면 물체의 상하가 스크린을 벗어난다.
 *			∴ Hc = h
 */
float	GetEyeDistance(const float h, const float w, float& Hv, float& Wv)
{
#ifdef USE__pointer_to_PickVar
	if (!gPickVarPtr)
		return 0;
	PickVar&	gPickVar = *gPickVarPtr;
#endif	// USE__pointer_to_PickVar
#ifdef USE__PickVar
	kState&	state = gPickVar.state;
//	CD3DArcBall&	g_ArcBall = gPickVar.g_ArcBall;
//	CModelViewerCamera2&	g_Camera = gPickVar.g_Camera;
#endif	// USE__PickVar

	const D3DSURFACE_DESC*	pBackBufferSurfaceDesc = DXUTGetD3D9BackBufferSurfaceDesc();
	const float	H = (float) pBackBufferSurfaceDesc->Height;
	const float	W = (float) pBackBufferSurfaceDesc->Width;
	const float	A = W / H;
	const float	a = w/h;
	if (a < 1) {
		if (A < 1) {
			if (a < A) {	// h -> H
//				cout << "a < A < 1" << endl;
				Hv = h;
			} else {	// w -> W
//				cout << "A <= a < 1" << endl;
				Hv = w / A;
			}
		} else {
//			cout << "a < 1, A >= 1" << endl;
			Hv = h;		// h -> H
		}
	} else {	// a >= 1
		if (A < 1) {
//			cout << "a >= 1, A < 1" << endl;
			Hv = w / A;	// w -> W
		} else {	// A >= 1
			if (a > A) {	// w -> W
//				cout << "a > A >= 1" << endl;
				Hv = w / A;
			} else {	// h -> H
//				cout << "A > a >= 1" << endl;
				Hv = h;
			}
		}
	}
	Wv = A * Hv;
	return .5f * Hv / tanf(.5f * state.FOVY);
}
#endif
#if 0	// 회전을 고려하지 않고 꽉 차보이게;
	// 회전하는 부분은 맞지 않음
void
CALLBACK OnFrameMove(
	double	fTime,
	float	fElapsedTime,
	void*	pUserContext
)
{
#ifdef USE__pointer_to_PickVar
	if (!gPickVarPtr)
		return;
	PickVar&	gPickVar = *gPickVarPtr;
#endif	// USE__pointer_to_PickVar
#ifdef USE__PickVar
	kState&	state = gPickVar.state;
	CD3DArcBall&	g_ArcBall = gPickVar.g_ArcBall;
#ifdef USE__pointer_to_kMesh
	kMesh*&	gkMeshPtr = gPickVar.gkMeshPtr;
	kMesh*&	gkMeshPtr0 = gPickVar.gkMeshPtr0;
#else	// USE__pointer_to_kMesh/!USE__pointer_to_kMesh
	kMesh&	gMesh = gPickVar.gMesh;
	kMesh&	gMesh0 = gPickVar.gMesh0;
#endif	// !USE__pointer_to_kMesh
	CModelViewerCamera2&	g_Camera = gPickVar.g_Camera;
#endif	// USE__PickVar
#ifdef USE__pointer_to_kMesh
	if (!gkMeshPtr)
		return;
	kMesh&	gMesh = *gkMeshPtr;
	kMesh&	gMesh0 = *gkMeshPtr0;
#endif	// USE__pointer_to_kMesh
//	cout << "OnFrameMove" << endl;
#if 1	// Camera
	const D3DXVECTOR3	pivotCenter = gMesh.pivotCenter();

	const D3DXVECTOR3&	left_lower_near = gMesh.left_lower_near();
	const D3DXVECTOR3&	right_upper_far = gMesh.right_upper_far();
	FLOAT	width0 = right_upper_far.x - left_lower_near.x;
	FLOAT	height0 = right_upper_far.y - left_lower_near.y;
	FLOAT	depth0 = right_upper_far.z - left_lower_near.z;
	VIEW_T&	view = state.getViewRef();
	VIEW_TYPE_T&	viewType = view.type;
	const bool&	orthographic = view.orthographic;
if (viewType != VIEW_PERSPECTIVE) {
	// DirectX 의 LHS; 오른쪽이 x축, 위쪽이 y축, 안쪽이 z축
	// 물체의 Up 방향이 y 축을 향해있고, 정면이 -z축이다:
	//		Eye			Up
	//	Top:	(0, distance, 0)	(0, 0, 1)
	//	Bottom:	(0, -distance, 0)	(0, 0, 1)
	//	Front:	(0, 0, -distance)	(0, 1, 0)
	//	Back:	(0, 0, distance)	(0, 1, 0)
	//	Right:	(distance, 0, 0)	(0, 1, 0)
	//	Left:	(-distance, 0, 0)	(0, 1, 0)
	//
	//		h	w	near			camera position
	//	Top:	depth	width	right_upper_far.y	near + distance
	//	Bottom:	depth	width	left_lower_near.y	near - distance
	//	Front:	heigh	width	left_lower_near.z	near - distance
	//	Back:	height	width	right_upper_far.z	near + distance
	//	Right:	height	depth	right_upper_far.x	near + distance
	//	Left:	height	depth	left_lower_near.x	near - distance
	//	*: distance 는 카메라 위치에서 near 까지의 거리(GetEyeDistance 함수의 결과)

	// Orthographic View
	if (viewType == VIEW_TOP || viewType == VIEW_BOTTOM) {
		D3DXMATRIX	matWorld;
		D3DXMatrixIdentity(&matWorld);
		// Pivot 을 원점으로...
		D3DXMatrixTranslation(&matWorld, -pivotCenter.x, -pivotCenter.y, -pivotCenter.z);
		g_Camera.SetWorldMatrix(matWorld);

		float	Hv, Wv;
		float	distance = GetEyeDistance(depth0, width0, Hv, Wv);
		D3DXVECTOR3	Eye(0, 0, 0);
		Eye.y = viewType == VIEW_TOP ?
			(right_upper_far.y - pivotCenter.y) + 1.1f * distance :
			(left_lower_near.y - pivotCenter.y) - 1.1f * distance;
		D3DXVECTOR3	At(0, 0, 0);
		D3DXVECTOR3	Up(0, 0, 1);
		D3DXMATRIX	matView;
//		D3DXMatrixLookAtLH(&matView, &vEye, &vAt, &vUp);
		g_Camera.SetViewParams2(Eye, At, Up);

		if (orthographic) {
			D3DXMATRIX	projectionMatrix;
//			D3DXMatrixOrthoLH(&projectionMatrix, width, depth, 0, 1000);
			D3DXMatrixOrthoLH(
				&projectionMatrix,
				1.05f * Wv, 1.05f * Hv,
				1.1f * distance - .01f * height0,
				1.1f * distance + 1.01f * height0
			);
			g_Camera.SetProjMatrix(projectionMatrix);
		}
	} else if (viewType == VIEW_FRONT || viewType == VIEW_BACK) {
		D3DXMATRIX	matWorld;
		D3DXMatrixIdentity(&matWorld);
		// Pivot 을 원점으로...
		D3DXMatrixTranslation(&matWorld, -pivotCenter.x, -pivotCenter.y, -pivotCenter.z);
		g_Camera.SetWorldMatrix(matWorld);

		float	Hv, Wv;
		float	distance = GetEyeDistance(height0, width0, Hv, Wv);
		D3DXVECTOR3	Eye(0, 0, 0);
		Eye.z = viewType == VIEW_FRONT ?
			(left_lower_near.z - pivotCenter.z) - 1.1f * distance :
			(right_upper_far.z - pivotCenter.z) + 1.1f * distance;
		D3DXVECTOR3	At(0, 0, 0);
		D3DXVECTOR3	Up(0, 1, 0);
		D3DXMATRIX	matView;
//		D3DXMatrixLookAtLH(&matView, &vEye, &vAt, &vUp);
		g_Camera.SetViewParams2(Eye, At, Up);

		if (orthographic) {
			D3DXMATRIX	projectionMatrix;
//			D3DXMatrixOrthoLH(&projectionMatrix, width, height, 0, 1000);
			D3DXMatrixOrthoLH(
				&projectionMatrix,
				1.05f * Wv, 1.05f * Hv,
				1.1f * distance - .01f * depth0,
				1.1f * distance + 1.01f * depth0
			);
			g_Camera.SetProjMatrix(projectionMatrix);
		}
	} else if (viewType == VIEW_LEFT || viewType == VIEW_RIGHT) {
		D3DXMATRIX	matWorld;
		D3DXMatrixIdentity(&matWorld);
		// Pivot 을 원점으로...
		D3DXMatrixTranslation(&matWorld, -pivotCenter.x, -pivotCenter.y, -pivotCenter.z);
		g_Camera.SetWorldMatrix(matWorld);

		float	Hv, Wv;
		float	distance = GetEyeDistance(height0, depth0, Hv, Wv);
		D3DXVECTOR3	Eye(0, 0, 0);
		Eye.x = viewType == VIEW_RIGHT ?
			(right_upper_far.x - pivotCenter.x) + 1.1f * distance :
			(left_lower_near.x - pivotCenter.x) - 1.1f * distance;
		D3DXVECTOR3	At(0, 0, 0);
		D3DXVECTOR3	Up(0, 1, 0);
		D3DXMATRIX	matView;
//		D3DXMatrixLookAtLH(&matView, &vEye, &vAt, &vUp);
		g_Camera.SetViewParams2(Eye, At, Up);

		if (orthographic) {
			D3DXMATRIX	projectionMatrix;
//			D3DXMatrixOrthoLH(&projectionMatrix, depth, height, 0, 1000);
			D3DXMatrixOrthoLH(
				&projectionMatrix,
				1.05f * Wv, 1.05f * Hv,
				1.1f * distance - .01f * width0,
				1.1f * distance + 1.01f * width0
			);
			g_Camera.SetProjMatrix(projectionMatrix);
		}
	}

	ROTATION_T&	rotation = state.getRotationRef();
	ROTATE_STATE&	rotationState = rotation.state;
	if (
		rotationState == ROTATE_READY ||	// 회전하도록 방금 전달받았다
		rotationState == ROTATE_ON	// 회전 중이다.
	) {
	FLOAT	h = right_upper_far.y - left_lower_near.y;
	FLOAT	w = right_upper_far.x - left_lower_near.x;
	float	x_distance0 = left_lower_near.x - .5f * h / tanf(.5f * state.FOVY);
	float	x_distance = 1.2f * x_distance0;
	float	y_distance0 = left_lower_near.y - .5f * h / tanf(.5f * state.FOVY);
	float	y_distance = 1.2f * y_distance0;
	float	z_distance0 = left_lower_near.z - .5f * h / tanf(.5f * state.FOVY);
	float	z_distance = 1.2f * z_distance0;
	FLOAT	width = width0 * 1.1f;
	FLOAT	height = height0 * 1.1f;
	FLOAT	depth = depth0 * 1.1f;

		// 아래 두 값(fTime0, clockwise) 은
		// 회전이 시작할 때 설정된 후,
		// 프레임이 지나면서 회전이 끝날 때까지 사용된다.
		static float	fTime0;
		static bool	clockwise;
		const float	fPeriod = 3.f;
		if (rotationState == ROTATE_READY) {
			clockwise = rotation.clockwise;
			fTime0 = (float) fTime;
			rotationState = ROTATE_ON;
		}

		D3DXVECTOR3	Eye;
		D3DXVECTOR3	At(0, 0, 0);
		if (
			viewType == VIEW_FRONT || viewType == VIEW_BACK ||
			viewType == VIEW_LEFT || viewType == VIEW_RIGHT
		) {
			D3DXVECTOR3	Up(0, 1, 0);
			//	Front:	(0, 0, -distance)	(0, 1, 0)
			//	Back:	(0, 0, distance)	(0, 1, 0)
			//	Right:	(distance, 0, 0)	(0, 1, 0)
			//	Left:	(-distance, 0, 0)	(0, 1, 0)
			Eye.y = 0.f;

			float	t = (float) (fTime - fTime0);
			// clockwise: True
			//	FRONT -> RIGHT -> BACK -> LEFT
			// clockwise: False
			//	FRONT -> LEFT -> BACK -> RIGHT
			if (t > fPeriod / 4.f) {	// 90까지만 돈다
				if (viewType == VIEW_FRONT) {
					Eye.z = 0.f;
					if (clockwise) {	// -> RIGHT
						Eye.x = -x_distance;
						viewType = VIEW_RIGHT;
					} else {		// -> LEFT
						Eye.x = x_distance;
						viewType = VIEW_LEFT;
					}
				} else if (viewType == VIEW_BACK) {
					Eye.z = 0.f;
					if (clockwise) {	// -> LEFT
						Eye.x = x_distance;
						viewType = VIEW_LEFT;
					} else {		// -> RIGHT
						Eye.x = -x_distance;
						viewType = VIEW_RIGHT;
					}
				} else if (viewType == VIEW_LEFT) {
					Eye.x = 0.f;
					if (clockwise) {	// -> FRONT
						Eye.z = z_distance;
						viewType = VIEW_FRONT;
					} else {		// -> BACK
						Eye.z = -z_distance;
						viewType = VIEW_BACK;
					}
				} else {	// RIGHT
					Eye.x = 0.f;
					if (clockwise) {	// -> BACK
						Eye.z = -z_distance;
						viewType = VIEW_BACK;
					} else {		// -> FRONT
						Eye.z = z_distance;
						viewType = VIEW_FRONT;
					}
				}
				rotationState = ROTATE_NONE;
			} else {
				//		Eye			Up
				//	Front:	(0, 0, -distance)	(0, 1, 0)
				//	Back:	(0, 0, distance)	(0, 1, 0)
				//	Right:	(distance, 0, 0)	(0, 1, 0)
				//	Left:	(-distance, 0, 0)	(0, 1, 0)
				//
				// clockwise: True
				//	FRONT -> RIGHT -> BACK -> LEFT
				// clockwise: False
				//	FRONT -> LEFT -> BACK -> RIGHT
				if (viewType == VIEW_FRONT) {
				//	Front:	(0, 0, -distance)	(0, 1, 0)
					if (clockwise) {	// -> RIGHT
				//	Right:	(distance, 0, 0)	(0, 1, 0)
						Eye.x = -x_distance * sinf(2 * D3DX_PI / fPeriod * t);
						Eye.z = z_distance * cosf(2 * D3DX_PI / fPeriod * t);
					} else {		// -> LEFT
				//	Left:	(-distance, 0, 0)	(0, 1, 0)
						Eye.x = x_distance * sinf(2 * D3DX_PI / fPeriod * t);
						Eye.z = z_distance * cosf(2 * D3DX_PI / fPeriod * t);
					}
				} else if (viewType == VIEW_BACK) {
				//	Back:	(0, 0, distance)	(0, 1, 0)
					if (clockwise) {	// -> LEFT
				//	Left:	(-distance, 0, 0)	(0, 1, 0)
						Eye.x = x_distance * sinf(2 * D3DX_PI / fPeriod * t);
						Eye.z = -z_distance * cosf(2 * D3DX_PI / fPeriod * t);
					} else {		// -> RIGHT
				//	Right:	(distance, 0, 0)	(0, 1, 0)
						Eye.x = -x_distance * sinf(2 * D3DX_PI / fPeriod * t);
						Eye.z = -z_distance * cosf(2 * D3DX_PI / fPeriod * t);
					}
				} else if (viewType == VIEW_LEFT) {
				//	Left:	(-distance, 0, 0)	(0, 1, 0)
					if (clockwise) {	// -> FRONT
				//	Front:	(0, 0, -distance)	(0, 1, 0)
						Eye.x = x_distance * cosf(2 * D3DX_PI / fPeriod * t);
						Eye.z = z_distance * sinf(2 * D3DX_PI / fPeriod * t);
					} else {		// -> BACK
				//	Back:	(0, 0, distance)	(0, 1, 0)
						Eye.x = x_distance * cosf(2 * D3DX_PI / fPeriod * t);
						Eye.z = -z_distance * sinf(2 * D3DX_PI / fPeriod * t);
					}
				} else {	// VIEW_RIGHT
				//	Right:	(distance, 0, 0)	(0, 1, 0)
					if (clockwise) {	// -> BACK
				//	Back:	(0, 0, distance)	(0, 1, 0)
						Eye.x = -x_distance * cosf(2 * D3DX_PI / fPeriod * t);
						Eye.z = -z_distance * sinf(2 * D3DX_PI / fPeriod * t);
					} else {		// -> FRONT
				//	Front:	(0, 0, -distance)	(0, 1, 0)
						Eye.x = -x_distance * cosf(2 * D3DX_PI / fPeriod * t);
						Eye.z = z_distance * sinf(2 * D3DX_PI / fPeriod * t);
					}
				}
			}
			// Update the camera's position based on time
			g_Camera.SetViewParams2(Eye, At, Up);

			if (orthographic) {
				D3DXMATRIX	projectionMatrix;
				D3DXMatrixOrthoLH(&projectionMatrix, width > depth ? width : depth, height, 0, 1000);
				g_Camera.SetProjMatrix(projectionMatrix);
			}

		}
	}
} else {
	if (state.getHomeRef()) {
		// OnCreateDevice 에서처럼...
		const D3DSURFACE_DESC*	pBackBufferSurfaceDesc = DXUTGetD3D9BackBufferSurfaceDesc();
		// Setup the projection matrix
		float	fAspect = (float) pBackBufferSurfaceDesc->Width / (float) pBackBufferSurfaceDesc->Height;
		g_Camera.SetProjParams(state.FOVY, fAspect, gMesh.radius() / 64.0f, gMesh.radius() * 200.0f);
		// Setup the arcball parameters
/*
	void	CD3DArcBall::Reset()
	{
		D3DXQuaternionIdentity(&m_qDown);
		D3DXQuaternionIdentity(&m_qNow);
		D3DXMatrixIdentity(&m_mRotation);
		D3DXMatrixIdentity(&m_mTranslation);
		D3DXMatrixIdentity(&m_mTranslationDelta);
		m_bDrag = FALSE;

		m_fRadiusTranslation = 1.0f;
		m_fRadius = 1.0f;
	}
 */
		g_ArcBall.Reset();	// 모든 파라미터를 초기화한다;
					// 하지만 m_fRadius 와 m_fRadiusTranslation 을 읽어들인 오브젝트에 맞게...
		g_ArcBall.SetWindow(pBackBufferSurfaceDesc->Width, pBackBufferSurfaceDesc->Height, 0.85f);
		g_ArcBall.SetTranslationRadius(gMesh.radius());
		state.getHomeRef() = false;
	}
//	IDirect3DDevice9*	pd3dDevice = DXUTGetD3D9Device();

	// Setup world matrix
	D3DXMATRIX	matWorld;
	D3DXMatrixIdentity(&matWorld);
	// Pivot 을 원점으로...
	D3DXMatrixTranslation(&matWorld, -pivotCenter.x, -pivotCenter.y, -pivotCenter.z);
	D3DXMATRIX	m = *g_ArcBall.GetRotationMatrix();

	m = *g_ArcBall.GetTranslationMatrix();
	D3DXMatrixMultiply(&matWorld, &matWorld, g_ArcBall.GetRotationMatrix());
	D3DXMatrixMultiply(&matWorld, &matWorld, g_ArcBall.GetTranslationMatrix());
//	pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);
	g_Camera.SetWorldMatrix(matWorld);

//	D3DXMATRIX	matView;
	// 꽉차게 보이는 카메라의 위치, L 은?
	// 물체의 높이를 h 라고 할 때, tan(fovy/2) = (h/2) / L 로부터
	//	L = (h / 2) / tan(fovy / 2)
	float	Hv, Wv;
	float	distance = GetEyeDistance(height0, width0, Hv, Wv);
	D3DXVECTOR3	vEye(0, 0, (left_lower_near.z - pivotCenter.z) - 1.2f * distance);	// 120%
	D3DXVECTOR3	vAt(0, 0, 0);
	D3DXVECTOR3	vUp(0, 1, 0);
//	D3DXMatrixLookAtLH(&matView, &vEye, &vAt, &vUp);
//	pd3dDevice->SetTransform(D3DTS_VIEW, &matView);
	g_Camera.SetViewParams(&vEye, &vAt);
#ifdef USE__fileOpen
#if 1	// logo
	if (gPickVar.setAllArg.connectable == CONNECTABLE_LOGO) {	// logo
		// Rotate the camera about the y-axis
		D3DXVECTOR3	vFromPt;
		D3DXVECTOR3	vLookatPt = D3DXVECTOR3(0, 0, 0);

		vFromPt.x = -cosf((float) fTime / 3.f) * z_distance;
		vFromPt.y = 0.f;
		vFromPt.z = sinf((float) fTime / 3.f) * z_distance;

		// Update the camera's position based on time
		g_Camera.SetViewParams(&vFromPt, &vLookatPt);
	}
#endif	// logo
#endif	// USE__fileOpen
#if 0
	// Rotate the camera about the y-axis
	D3DXVECTOR3	vFromPt = D3DXVECTOR3(0, 0, 0);
	D3DXVECTOR3	vLookatPt = D3DXVECTOR3(0, 0, 0);

	vFromPt.x = -cosf((float) fTime / 3.f) * CAMERA_DISTANCE;
	vFromPt.y = 1.f;
	vFromPt.z = sinf((float) fTime / 3.f) * CAMERA_DISTANCE;

	// Update the camera's position based on time
	g_Camera.SetViewParams(&vFromPt, &vLookatPt);
#endif
}
#endif

#if 1	// 색깔 등고선, 둘레, 성형
	// 색깔 등고선
	if (!state.g__showDefinedArea && !state.brush.On && state.g__prID != (DWORD) -1 && state.g__prID != state.g__lastPrID) {
		state.g__index = state.g__prID;
		gMesh.setColorIsometric(state.g__prID, state.g__Shift, state.g__nChannel, state.g__nStep);
	}

	// 둘레
#if 1
	if (state.g__bString || state.g__incompleteString) {
		kMesh*	kMeshPtr = &gMesh;
		if (state.g__pickStandard && &gMesh0 && gMesh0.meshLoader().meshPtr())
			kMeshPtr = &gMesh0;
		kMesh&	meshRef = *kMeshPtr;

#pragma warning(disable: 4127)
	do {
		// 같은 위치를 가지고 두 번 계산하지 않도록...;
		//	둘레(state.g__bString): state.g__stringMeasure
		//	두 점 사이의 거리(state.g__incompleteString): state.__numIntersection, state.__lastNumIntersection
		if (state.g__bString) {
			if (!state.g__stringMeasure)
				break;
			else
				state.g__stringMeasure = false;
		}
		if (state.g__incompleteString) {
			unsigned	__numIntersection = state.__numIntersection;
			if (__numIntersection && __numIntersection % 2 != 0)	// 홀수 개의 점이 찍혔다.
				break;
			// 짝수 개의 점이 찍혔다.

			unsigned&	__lastNumIntersection = state.__lastNumIntersection;
			if (__numIntersection == __lastNumIntersection) {	// 한 번 계산했으면 다시 하지 않는다
				if (!state.g__stringMeasure)	// 강제로 다시 측정하라고 하지 않는 한
					break;
				else
					state.g__stringMeasure = false;
			} else {
				__lastNumIntersection = __numIntersection;	// 두 번 계산하지 안도록 한다.
			}
		}

		(void) fprintf(stdout, "doMeasure*\n");
		START_DEF_PTR	startDefPtr;
		START_DEF&	startDef = meshRef.tapeMeasure().startDef();
		D3DXVECTOR3*	normalPtr;
		if (startDef.type == START_DEF_TYPE__GIRTH) {
			startDefPtr.type = START_DEF_PTR_TYPE__GIRTH;
			startDefPtr.plane_def_ptr = &startDef.plane_def;
			normalPtr = &startDef.plane_def.normal;
		} else {	// START_DEF_TYPE__TWO_POINT
			startDefPtr.type = START_DEF_PTR_TYPE__TWO_POINT;
			startDefPtr.two_point_plane_def_ptr = &meshRef.tapeMeasure().startDef().two_point_plane_def;
			normalPtr = &startDef.two_point_plane_def.normal;
		}
		normal_manipulator_done_callback_manual_measure(&meshRef, 0, &startDefPtr, *normalPtr);
	} while (0);
#pragma warning(default: 4127)
	}
#endif

	// 성형
	if (state.g__plus || state.g__minus) {
		float	fNormalAmount = (state.g__Shift ? 0.1f : 0.01f) * gMesh.a();
		if (!state.g__plus)
			fNormalAmount = -fNormalAmount;
		gMesh.ChangeMesh(fNormalAmount, state.g__nChannel);
	}

	if (state.g__prID != state.g__lastPrID)
		state.g__lastPrID = state.g__prID;
#endif
}
#else	// 회전을 고려하지 않고 꽉 차보이게/회전을 고려
void
CALLBACK OnFrameMove(
	double	fTime,
	float	fElapsedTime,
	void*	pUserContext
)
{
#ifdef USE__pointer_to_PickVar
	if (!gPickVarPtr)
		return;
	PickVar&	gPickVar = *gPickVarPtr;
#endif	// USE__pointer_to_PickVar
#ifdef USE__PickVar
	kState&	state = gPickVar.state;
#if 1	// 마지막 주문; 9. 카메라 위치 제한
	CD3DArcBall2&	g_ArcBall = gPickVar.g_ArcBall;
#else	// 마지막 주문; 9. 카메라 위치 제한/!9. 카메라 위치 제한
	CD3DArcBall&	g_ArcBall = gPickVar.g_ArcBall;
#endif	// !마지막 주문; 9. 카메라 위치 제한
#ifdef USE__pointer_to_kMesh
	kMesh*&	gkMeshPtr = gPickVar.gkMeshPtr;
	kMesh*&	gkMeshPtr0 = gPickVar.gkMeshPtr0;
#else	// USE__pointer_to_kMesh/!USE__pointer_to_kMesh
	kMesh&	gMesh = gPickVar.gMesh;
	kMesh&	gMesh0 = gPickVar.gMesh0;
#endif	// !USE__pointer_to_kMesh
	CModelViewerCamera2&	g_Camera = gPickVar.g_Camera;
#endif	// USE__PickVar
#ifdef USE__pointer_to_kMesh
	if (!gkMeshPtr)
		return;
	kMesh&	gMesh = *gkMeshPtr;
	kMesh&	gMesh0 = *gkMeshPtr0;
#endif	// USE__pointer_to_kMesh
//	cout << "OnFrameMove" << endl;
#if 1	// Camera
	const D3DXVECTOR3&	left_lower_near = gMesh.left_lower_near();
	const D3DXVECTOR3&	right_upper_far = gMesh.right_upper_far();
	const D3DXVECTOR3	pivotCenter = gMesh.pivotCenter();
	VIEW_T&	view = state.getViewRef();
	VIEW_TYPE_T&	viewType = view.type;
	const bool&	orthographic = view.orthographic;
	FLOAT	h = right_upper_far.y - left_lower_near.y;
	FLOAT	w = right_upper_far.x - left_lower_near.x;
#if 1	// SHIFT
	float	eye_x_offset = 0.f;
//	D3DVIEWPORT9	viewport;
//	DXUTGetD3D9Device()->GetViewport(&viewport);
//	cout << "Viewport.Height = " << viewport.Height << " (Height = " << pBackBufferSurfaceDesc->Height << "), Viewport.Width = " << viewport.Width << " (Width = " << pBackBufferSurfaceDesc->Width << ")" << endl;
/*
 * 카메라(눈)가 물체(높이가 h, 넓이가 w)의 중앙을 바라보고 있다면(eye_x_offset = 0, eye_y_offset = 0),
 * 물체의 중심이 화면(높이가 H, 넓이가 W)의 중앙에 온다.
	const float	a = w/h;
 * GetEyeDistance 에서 물체가 화면에 차 보이도록 물체의 앞면과 카메라의 거리를 정했으므로,
 * 물체의 좌측 끝(-w/2)이 위치하는 화면 상의 위치는, a(= w/h) > 1 인 경우에는 -W/2 이다.
 * a < 1 경우에는, 화면 상에서 물체의 폭(w)이 차지하는 길이는, H / ? = h / w 에서, (H / h) * w 이다;
 * 화면의 폭(W)과 이 값과의 차이가 물체의 좌우가 화면의 좌우와 떨어진 정도를 말해준다: [W - (H/h)*w] / 2
 * 따라서 물체의 좌측 끝(-w/2)에 해당하는 화면 상의 위치는 -[(H/h)*w]/2 이다;
 * 화면의 좌측 끝을 기준으로 삼으면 ell0 = W/2 - (H/h)*w/2 가 된다.
 */
	const D3DSURFACE_DESC*	pBackBufferSurfaceDesc = DXUTGetD3D9BackBufferSurfaceDesc();
	float	H = (float) pBackBufferSurfaceDesc->Height;
	float	W = (float) pBackBufferSurfaceDesc->Width;
	float	ell;	// 물체의 앞면 좌측선이 화면상의 좌측선을 기준으로 움직일 위치
			//	0:= 물체의 앞면 좌측선을 화면의 좌측에 맞출 때; left align
			//	ell0 = (W - (H / h) * w) / 2:= 물체의 중앙의 화면의 중앙에 위치; center align
			//	W - (H / h) * w:= 물체의 앞면 우측선을 화면의 우측에 맞출 때; right align
			//	(이보다 더 큰 값이면 물체의 우측면이 잘린다)
			// TO DO LIST; ell > W / 2 을 넘어서면
			// 뒷면의 좌측선이 앞면의 좌측선보다 더 좌측으로 나타난다.
			// 따라서 이 경우에는 뒷면의 좌측선을 기준으로 해야한다. 그러나...
	float	ell0 = .5f * (W - (H / h) * w);	// 물체의 앞면 중앙이 화면의 중앙에 위치할 때,
	ell = gMesh.GetShift();
						// 물체의 앞면 좌측선이 화면의 좌측선을 기준으로 움직인 거리
//	ell *= 1.9f;
//	cout << "ell = " << ell << ", ell0 = " << ell0 << endl;
	if (ell < 0.f)	// 물체의 폭이 화면의 폭을 가득 채운 경우
		ell = 0.f;
	if (ell0 < 0.f)	// 물체의 폭이 화면의 폭을 가득 채운 경우
		ell0 = 0.f;
	eye_x_offset = h / H * (ell0 - ell);
#endif	// SHIFT
if (viewType != VIEW_PERSPECTIVE) {
	// y축에 대해서 회전을 생각하므로,
	// 화면에 맞출 높이(Hv)는 항상 y방향의 높이(h)에 의해,
	// 화면에 맞출 넓이(Wv)는 x방향의 넓이(w)와 z방향의 두께(d) 중에서 큰 값에 의해 결정된다.
	FLOAT	d = right_upper_far.z - left_lower_near.z;
	float	Hv, Wv;
	float	distance = GetEyeDistance(h, w > d ? w : d, Hv, Wv);

	// 모든 거리(?_distance)는 pivot 의 위치(물체의 무게중심)에서부터의 거리이다.
	float	x_distance = (left_lower_near.x - pivotCenter.x) - 1.0f * distance;
	float	y_distance = (left_lower_near.y - pivotCenter.y) - 1.0f * distance;
	float	z_distance = (left_lower_near.z - pivotCenter.z) - 1.0f * distance;
#if 0
	FLOAT	width = right_upper_far.x - left_lower_near.x;
	FLOAT	height = right_upper_far.y - left_lower_near.y;
	FLOAT	depth = right_upper_far.z - left_lower_near.z;
	width *= 1.1f;
	height *= 1.1f;
	depth *= 1.1f;
#endif
	// DirectX 의 LHS; 오른쪽이 x축, 위쪽이 y축, 안쪽이 z축
	// 물체의 Up 방향이 y 축을 향해있고, 정면이 -z축이다:
	//		Eye			Up
	//	Top:	(0, distance, 0)	(0, 0, 1)
	//	Bottom:	(0, -distance, 0)	(0, 0, 1)
	//	Front:	(0, 0, -distance)	(0, 1, 0)
	//	Back:	(0, 0, distance)	(0, 1, 0)
	//	Right:	(distance, 0, 0)	(0, 1, 0)
	//	Left:	(-distance, 0, 0)	(0, 1, 0)

	// Orthographic View
	if (viewType == VIEW_TOP || viewType == VIEW_BOTTOM) {
		D3DXMATRIX	matWorld;
		D3DXMatrixIdentity(&matWorld);
		// Pivot 을 원점으로...
		D3DXMatrixTranslation(&matWorld, -pivotCenter.x, -pivotCenter.y, -pivotCenter.z);
		g_Camera.SetWorldMatrix(matWorld);

		D3DXVECTOR3	Eye(0, -y_distance, 0);
		if (viewType == VIEW_BOTTOM)
			Eye.y = -Eye.y;
		D3DXVECTOR3	At(0, 0, 0);
#if 1	// SHIFT
		if (viewType == VIEW_TOP) {
			Eye.x += eye_x_offset;
			At.x += eye_x_offset;
		} else {	// VIEW_BOTTOM
			Eye.x -= eye_x_offset;
			At.x -= eye_x_offset;
		}
#endif	// SHIFT
		D3DXVECTOR3	Up(0, 0, 1);
		D3DXMATRIX	matView;
//		D3DXMatrixLookAtLH(&matView, &vEye, &vAt, &vUp);
		g_Camera.SetViewParams2(Eye, At, Up);

		if (orthographic) {
			D3DXMATRIX	projectionMatrix;
//			D3DXMatrixOrthoLH(&projectionMatrix, width, depth, 0, 1000);
			// 모든 값은 카메라(눈)의 위치를 원점으로 했을 때의 값이다;
			// 특히 z-near, z-far 값을 설정할 때 주의해야 한다.
			D3DXMatrixOrthoLH(
				&projectionMatrix,
				1.05f * Wv, 1.05f * Hv,
				1.1f * distance - .01f * h,	// 1.1f * distance:= 카메라에서 윗면까지의 거리
								// h:= 윗면과 아래면 사이의 거리
								// 0.01: 앞뒤로 1%만큼 더 고려한다.
				1.1f * distance + 1.01f * h
			);
			g_Camera.SetProjMatrix(projectionMatrix);
		}
	} else if (viewType == VIEW_FRONT || viewType == VIEW_BACK) {
#if 0	// At 과 Eye 사이의 일정한 거리; CONSTANT_LENGTH
	if (x_distance > z_distance)
		z_distance = x_distance;
	else
		x_distance = z_distance;
#endif
		D3DXMATRIX	matWorld;
		D3DXMatrixIdentity(&matWorld);
		// Pivot 을 원점으로...
		D3DXMatrixTranslation(&matWorld, -pivotCenter.x, -pivotCenter.y, -pivotCenter.z);
		g_Camera.SetWorldMatrix(matWorld);

		D3DXVECTOR3	Eye(0, 0, z_distance);
		if (viewType == VIEW_BACK)
			Eye.z = -Eye.z;
		D3DXVECTOR3	At(0, 0, 0);
#if 1	// SHIFT
		if (viewType == VIEW_FRONT) {
			Eye.x += eye_x_offset;
			At.x += eye_x_offset;
		} else {	// VIEW_BACK
			Eye.x -= eye_x_offset;
			At.x -= eye_x_offset;
		}
#endif	// SHIFT
		D3DXVECTOR3	Up(0, 1, 0);
		D3DXMATRIX	matView;
//		D3DXMatrixLookAtLH(&matView, &vEye, &vAt, &vUp);
		g_Camera.SetViewParams2(Eye, At, Up);

		if (orthographic) {
			D3DXMATRIX	projectionMatrix;
//			D3DXMatrixOrthoLH(&projectionMatrix, width, height, 0, 1000);
			// 모든 값은 카메라(눈)의 위치를 원점으로 했을 때의 값이다;
			// 특히 z-near, z-far 값을 설정할 때 주의해야 한다.
			D3DXMatrixOrthoLH(
				&projectionMatrix,
				1.05f * Wv, 1.05f * Hv,
				1.1f * distance - .01f * d,
				1.1f * distance + 1.01f * d
			);
			g_Camera.SetProjMatrix(projectionMatrix);
		}
	} else if (viewType == VIEW_LEFT || viewType == VIEW_RIGHT) {
#if 0	// At 과 Eye 사이의 일정한 거리; CONSTANT_LENGTH
	if (x_distance > z_distance)
		z_distance = x_distance;
	else
		x_distance = z_distance;
#endif
		D3DXMATRIX	matWorld;
		D3DXMatrixIdentity(&matWorld);
		// Pivot 을 원점으로...
		D3DXMatrixTranslation(&matWorld, -pivotCenter.x, -pivotCenter.y, -pivotCenter.z);
		g_Camera.SetWorldMatrix(matWorld);

		D3DXVECTOR3	Eye(-x_distance, 0, 0);
		if (viewType == VIEW_LEFT)
			Eye.x = -Eye.x;
		D3DXVECTOR3	At(0, 0, 0);
#if 1	// SHIFT
		if (viewType == VIEW_RIGHT) {
			Eye.z += eye_x_offset;
			At.z += eye_x_offset;
		} else {	// VIEW_LEFT
			Eye.z -= eye_x_offset;
			At.z -= eye_x_offset;
		}
#endif	// SHIFT
		D3DXVECTOR3	Up(0, 1, 0);
		D3DXMATRIX	matView;
//		D3DXMatrixLookAtLH(&matView, &vEye, &vAt, &vUp);
		g_Camera.SetViewParams2(Eye, At, Up);

		if (orthographic) {
			D3DXMATRIX	projectionMatrix;
//			D3DXMatrixOrthoLH(&projectionMatrix, depth, height, 0, 1000);
			// 모든 값은 카메라(눈)의 위치를 원점으로 했을 때의 값이다;
			// 특히 z-near, z-far 값을 설정할 때 주의해야 한다.
			D3DXMatrixOrthoLH(
				&projectionMatrix,
				1.05f * Wv, 1.05f * Hv,
				1.1f * distance - .01f * w,
				1.1f * distance + 1.01f * w
			);
			g_Camera.SetProjMatrix(projectionMatrix);
		}
	}

	ROTATION_T&	rotation = state.getRotationRef();
	ROTATE_STATE&	rotationState = rotation.state;
	if (
		rotationState == ROTATE_READY ||	// 회전하도록 방금 전달받았다
		rotationState == ROTATE_ON	// 회전 중이다.
	) {
		// 아래 두 값(fTime0, clockwise) 은
		// 회전이 시작할 때 설정된 후,
		// 프레임이 지나면서 회전이 끝날 때까지 사용된다.
		static float	fTime0;
		static bool	clockwise;
		const float	fPeriod = 3.f;
		if (rotationState == ROTATE_READY) {
			clockwise = rotation.clockwise;
			fTime0 = (float) fTime;
			rotationState = ROTATE_ON;
		}

#if 1	// SHIFT
/*
 * At(a, 0), Eye(a, b) 위치가 물체가 회전할 때 어떻게 바뀌는 지?
 * 두 좌표축 x, z 가 수직인 y 축(LHS)에 대해서 시계방향 θ 만큼 회전했을 때,
 * At, Eye 의 좌표가 새로운 좌표축 x', z' 으로 어떻게 표시되는가?
 *
 * 회전에 대하여 x, z 와 x', z' 의 관계는 다음과 같다; 방향에 주의
 * C ≡ cosθ, S ≡ sinθ
 *
 *	x' = x C - z S
 *	z' = x S + z C
 *
 * 다시 쓰면
 *
 *	x =  x' C + z' S
 *	z = -x' S + z' C
 *
 * 따라서 새로운 좌표축 x', z' 에 대해 At, Eye 의 좌표는 다음과 같다.
 *
 *	At = a x = a (x' C + z S)
 *		=> a (C, S)
 *	Eye = a x + b z = a (x' C + z' S) + b (-x' S + z' C)
 *		=> a (C, S) + b (-S, C)
 *
 * 1. 아래의 FRONT -> RIGHT 의 경우에 다음과 같은 코드가 이를 말해준다.
 *
	Eye.x = -x_distance * sine;
	Eye.z = z_distance * cosine;
#if 1	// SHIFT
	At.x = eye_x_offset * cosine;
	At.z = eye_x_offset * sine;
	Eye.x += eye_x_offset * cosine;
	Eye.z += eye_x_offset * sine;
#endif	// SHIFT
 *
 * 단, 여기서 b 값이 서로 다른(S의 경우 -x_distance, C의 경우 z_distance) 이유는
 * θ가 0도 일때의 b 값(b1)과 θ가 90도 일때의 b 값(b2)이 서로 다르도록 설정했기 때문이다.
 * 그래야 0도 일때와 90도 일때 물체가 화면에 가득 "차" 보이게된다;
 * 실제로 길이 b가 길이는 √(b1 * b1 * sinθ + b2 * b2 * cosθ) 식에 의해 결정된다.
 * 일정한 길이(= max(b1, b2))보다 시각적으로 더 나은 결과가 나온다.
 *
 * 2. 시계방향으로 회전함에 따라 View 가 다음과 같이 바뀐다:
 *	FRONT(0) -> RIGHT(90) -> BACK(180) -> LEFT(270) -> FRONT(360)
 * 시간에 따른 모든 각도는 [0, 90]에 있으므로 위 식에서 θ에다
 * 적절히 90(RIGHT -> BACK), 180(BACK -> LEFT), 270(LEFT -> FRONT) 을 더한 식을 사용한다.
 * 마찬가지로 시계반대방향으로 돌 때에는 θ대신에 다음 값을 사용한다.
 *	-θ(FRONT -> LEFT), -90 - θ(LEFT -> BACK), -180 - θ(BACK -> RIGHT), -270 - θ(RIGHT -> FRONT)
 * 즉 View가 다음과 같이 바뀐다:
 *	FRONT(-0) -> LEFT(-90) -> BACK(-180) -> RIGHT(-270) -> FRONT(-360)
 */
		D3DXVECTOR3	Eye;
		D3DXVECTOR3	At(0, 0, 0);
		if (
			viewType == VIEW_FRONT || viewType == VIEW_BACK ||
			viewType == VIEW_LEFT || viewType == VIEW_RIGHT
		) {
#if 0	// At 과 Eye 사이의 일정한 거리; CONSTANT_LENGTH
	if (x_distance > z_distance)
		z_distance = x_distance;
	else
		x_distance = z_distance;
#endif
			D3DXVECTOR3	Up(0, 1, 0);
			//	Front:	(0, 0, -distance)	(0, 1, 0)
			//	Back:	(0, 0, distance)	(0, 1, 0)
			//	Right:	(distance, 0, 0)	(0, 1, 0)
			//	Left:	(-distance, 0, 0)	(0, 1, 0)
			Eye.y = 0.f;

			float	t = (float) (fTime - fTime0);
			// clockwise: True
			//	FRONT -> RIGHT -> BACK -> LEFT
			// clockwise: False
			//	FRONT -> LEFT -> BACK -> RIGHT
#if 0
			if (t > fPeriod / 4.f) {	// 90까지만 돈다
				float	cosine = 0.f;
				float	sine = 1.f;
				if (viewType == VIEW_FRONT) {
					Eye.z = 0.f;
					if (clockwise) {	// -> RIGHT
						Eye.x = -x_distance * sine;
						Eye.z = z_distance * cosine;
#if 1	// SHIFT
						At.x = eye_x_offset * cosine;
						At.z = eye_x_offset * sine;
						Eye.x += eye_x_offset * cosine;
						Eye.z += eye_x_offset * sine;
#else	// SHIFT/!SHIFT
						Eye.x = -x_distance;
#endif	// !SHIFT
						viewType = VIEW_RIGHT;
					} else {		// -> LEFT
						Eye.x = x_distance;
						viewType = VIEW_LEFT;
					}
				} else if (viewType == VIEW_BACK) {
					Eye.z = 0.f;
					if (clockwise) {	// -> LEFT
						Eye.x = x_distance;
						viewType = VIEW_LEFT;
					} else {		// -> RIGHT
						Eye.x = -x_distance;
						viewType = VIEW_RIGHT;
					}
				} else if (viewType == VIEW_LEFT) {
					Eye.x = 0.f;
					if (clockwise) {	// -> FRONT
						Eye.z = z_distance;
						viewType = VIEW_FRONT;
					} else {		// -> BACK
						Eye.z = -z_distance;
						viewType = VIEW_BACK;
					}
				} else {	// RIGHT
					Eye.x = 0.f;
					if (clockwise) {	// -> BACK
						Eye.z = -z_distance;
						viewType = VIEW_BACK;
					} else {		// -> FRONT
#if 1	// SHIFT
						Eye.x = -x_distance * cosine;
						Eye.z = z_distance * sine;
#if 1	// SHIFT
						Eye.x += eye_x_offset * sine;
						Eye.z += eye_x_offset * cosine;
						At.x = eye_x_offset * sine;
						At.z = eye_x_offset * cosine;
#endif	// SHIFT
#else	// SHIFT/!SHIFT
						Eye.z = z_distance;
#endif
						viewType = VIEW_FRONT;
					}
				}
				rotationState = ROTATE_NONE;
			} else {
#endif
				float	SINE = sinf(2 * D3DX_PI / fPeriod * t);
				float	COSINE = cosf(2 * D3DX_PI / fPeriod * t);
				bool	stop = false;
				if (t > fPeriod / 4.f) {	// 90까지만 돈다
					SINE = 1.f;
					COSINE = 0.f;
					stop = true;
				}
				float	sine;
				float	cosine;
				//		Eye			Up
				//	Front:	(0, 0, -distance)	(0, 1, 0)
				//	Back:	(0, 0, distance)	(0, 1, 0)
				//	Right:	(distance, 0, 0)	(0, 1, 0)
				//	Left:	(-distance, 0, 0)	(0, 1, 0)
				//
				// clockwise: True
				//	FRONT -> RIGHT -> BACK -> LEFT -> FRONT
				// clockwise: False
				//	FRONT -> LEFT -> BACK -> RIGHT -> FRONT
				if (viewType == VIEW_FRONT) {
				//	Front:	(0, 0, -distance)	(0, 1, 0)
					if (clockwise) {	// -> RIGHT; theta
								//	sine -> sine
								//	cosine -> cosine
				//	Right:	(distance, 0, 0)	(0, 1, 0)
						sine = SINE;
						cosine = COSINE;
						Eye.x = -x_distance * sine;
						Eye.z = z_distance * cosine;
#if 1	// SHIFT
						At.x = eye_x_offset * cosine;
						At.z = eye_x_offset * sine;
						Eye.x += eye_x_offset * cosine;
						Eye.z += eye_x_offset * sine;
#endif	// SHIFT
						if (stop)
							viewType = VIEW_RIGHT;
					} else {		// -> LEFT; -theta
								//	sine -> -sine
								//	cosine -> cosine
				//	Left:	(-distance, 0, 0)	(0, 1, 0)
#if 1
						sine = -SINE;
						cosine = COSINE;
						Eye.x = -x_distance * sine;
						Eye.z = z_distance * cosine;
#if 1	// SHIFT
						At.x = eye_x_offset * cosine;
						At.z = eye_x_offset * sine;
						Eye.x += eye_x_offset * cosine;
						Eye.z += eye_x_offset * sine;
#endif	// SHIFT
#else
						Eye.x = x_distance * sine;
						Eye.z = z_distance * cosine;
#if 1	// SHIFT
						At.x = eye_x_offset * cosine;
						At.z = -eye_x_offset * sine;
						Eye.x += eye_x_offset * cosine;
						Eye.z += -eye_x_offset * sine;
#endif	// SHIFT
#endif
						if (stop)
							viewType = VIEW_LEFT;
					}
				} else if (viewType == VIEW_BACK) {
				//	Back:	(0, 0, distance)	(0, 1, 0)
					if (clockwise) {	// -> LEFT; 180 + theta
								//	sine -> -sine
								//	cosine -> -cosine
				//	Left:	(-distance, 0, 0)	(0, 1, 0)
//						Eye.x = x_distance * sine;
//						Eye.z = -z_distance * cosine;
#if 1
						sine = -SINE;
						cosine = -COSINE;
						Eye.x = -x_distance * sine;
						Eye.z = z_distance * cosine;
#if 1	// SHIFT
						At.x = eye_x_offset * cosine;
						At.z = eye_x_offset * sine;
						Eye.x += eye_x_offset * cosine;
						Eye.z += eye_x_offset * sine;
#endif	// SHIFT
#else
						Eye.x = -x_distance * (-sine);
						Eye.z = z_distance * (-cosine);
#if 1	// SHIFT
						At.x = eye_x_offset * (-cosine);
						At.z = eye_x_offset * (-sine);
						Eye.x += eye_x_offset * (-cosine);
						Eye.z += eye_x_offset * (-sine);
#endif	// SHIFT
#endif
						if (stop)
							viewType = VIEW_LEFT;
					} else {		// -> RIGHT; -180 - theta
								//	sine -> sine
								//	cosine -> -cosine
				//	Right:	(distance, 0, 0)	(0, 1, 0)
#if 1
						sine = SINE;
						cosine = -COSINE;
						Eye.x = -x_distance * sine;
						Eye.z = z_distance * cosine;
#if 1	// SHIFT
						At.x = eye_x_offset * cosine;
						At.z = eye_x_offset * sine;
						Eye.x += eye_x_offset * cosine;
						Eye.z += eye_x_offset * sine;
#endif	// SHIFT
#else
						Eye.x = -x_distance * sine;
						Eye.z = -z_distance * cosine;
#endif
						if (stop)
							viewType = VIEW_RIGHT;
					}
				} else if (viewType == VIEW_LEFT) {
				//	Left:	(-distance, 0, 0)	(0, 1, 0)
					if (clockwise) {	// -> FRONT; 270 + theta
								//	sine -> -cosine
								//	cosine -> sine
				//	Front:	(0, 0, -distance)	(0, 1, 0)
//						Eye.x = x_distance * cosine;
//						Eye.z = z_distance * sine;
#if 1
						sine = -COSINE;
						cosine = SINE;
						Eye.x = -x_distance * sine;
						Eye.z = z_distance * cosine;
#if 1	// SHIFT
						At.x = eye_x_offset * cosine;
						At.z = eye_x_offset * sine;
						Eye.x += eye_x_offset * cosine;
						Eye.z += eye_x_offset * sine;
#endif	// SHIFT
#else
						Eye.x = -x_distance * (-cosine);
						Eye.z = z_distance * (sine);
#if 1	// SHIFT
						At.x = eye_x_offset * (sine);
						At.z = eye_x_offset * (-cosine);
						Eye.x += eye_x_offset * (sine);
						Eye.z += eye_x_offset * (-cosine);
#endif	// SHIFT
#endif
						if (stop)
							viewType = VIEW_FRONT;
					} else {		// -> BACK; - 90 - theta
								//	sine -> -cosine
								//	cosine -> -sine
#if 1
						sine = -COSINE;
						cosine = -SINE;
						Eye.x = -x_distance * sine;
						Eye.z = z_distance * cosine;
#if 1	// SHIFT
						At.x = eye_x_offset * cosine;
						At.z = eye_x_offset * sine;
						Eye.x += eye_x_offset * cosine;
						Eye.z += eye_x_offset * sine;
#endif	// SHIFT
#else
				//	Back:	(0, 0, distance)	(0, 1, 0)
						Eye.x = x_distance * cosine;
						Eye.z = -z_distance * sine;
#endif
						if (stop)
							viewType = VIEW_BACK;
					}
				} else {	// VIEW_RIGHT
				//	Right:	(distance, 0, 0)	(0, 1, 0)
					if (clockwise) {	// -> BACK; 90 + theta
								//	sine -> cosine
								//	cosine -> -sine
				//	Back:	(0, 0, distance)	(0, 1, 0)
//						Eye.x = -x_distance * cosine;
//						Eye.z = -z_distance * sine;
#if 1
						sine = COSINE;
						cosine = -SINE;
						Eye.x = -x_distance * sine;
						Eye.z = z_distance * cosine;
#if 1	// SHIFT
						At.x = eye_x_offset * cosine;
						At.z = eye_x_offset * sine;
						Eye.x += eye_x_offset * cosine;
						Eye.z += eye_x_offset * sine;
#endif	// SHIFT
#else
						Eye.x = -x_distance * (cosine);
						Eye.z = z_distance * (-sine);
#if 1	// SHIFT
						At.x = eye_x_offset * (sine);
						At.z = eye_x_offset * (-sine);
						Eye.x += eye_x_offset * (sine);
						Eye.z += eye_x_offset * (-sine);
#endif	// SHIFT
#endif
						if (stop)
							viewType = VIEW_BACK;
					} else {		// -> FRONT; -270 - theta
								//	sine -> cosine
								//	cosine -> sine
				//	Front:	(0, 0, -distance)	(0, 1, 0)
#if 1
						sine = COSINE;
						cosine = SINE;
						Eye.x = -x_distance * sine;
						Eye.z = z_distance * cosine;
#if 1	// SHIFT
						At.x = eye_x_offset * cosine;
						At.z = eye_x_offset * sine;
						Eye.x += eye_x_offset * cosine;
						Eye.z += eye_x_offset * sine;
#endif	// SHIFT
#else
						Eye.x = -x_distance * cosine;
						Eye.z = z_distance * sine;
#if 1	// SHIFT
						Eye.x += eye_x_offset * sine;
						Eye.z += eye_x_offset * cosine;
						At.x = eye_x_offset * sine;
						At.z = eye_x_offset * cosine;
#endif	// SHIFT
#endif
						if (stop)
							viewType = VIEW_FRONT;
					}
				}
				if (stop)
					rotationState = ROTATE_NONE;
#if 0
			}
#endif
			// Update the camera's position based on time
			g_Camera.SetViewParams2(Eye, At, Up);

			if (orthographic) {
				D3DXMATRIX	projectionMatrix;
//				D3DXMatrixOrthoLH(&projectionMatrix, width > depth ? width : depth, height, 0, 1000);
				float	L = sqrtf(Eye.x * Eye.x + Eye.z * Eye.z);	// 카메라(눈)에서 회전축가지의 거리
				float	D = sqrtf(w * w + d * d);	// D 는 위/아래에서 봤을 때 보이는 면의 대각선 길이
				D3DXMatrixOrthoLH(
					&projectionMatrix,
					1.05f * Wv,
					1.05f * Hv,
					L - .5f * D - .01f * D,
					L + .5f * D + .01f * D
				);
				g_Camera.SetProjMatrix(projectionMatrix);
			}

		}
#else	// SHIFT/!SHIFT
		D3DXVECTOR3	Eye;
		D3DXVECTOR3	At(0, 0, 0);
		if (
			viewType == VIEW_FRONT || viewType == VIEW_BACK ||
			viewType == VIEW_LEFT || viewType == VIEW_RIGHT
		) {
			D3DXVECTOR3	Up(0, 1, 0);
			//	Front:	(0, 0, -distance)	(0, 1, 0)
			//	Back:	(0, 0, distance)	(0, 1, 0)
			//	Right:	(distance, 0, 0)	(0, 1, 0)
			//	Left:	(-distance, 0, 0)	(0, 1, 0)
			Eye.y = 0.f;

			float	t = (float) (fTime - fTime0);
			// clockwise: True
			//	FRONT -> RIGHT -> BACK -> LEFT
			// clockwise: False
			//	FRONT -> LEFT -> BACK -> RIGHT
			if (t > fPeriod / 4.f) {	// 90까지만 돈다
				if (viewType == VIEW_FRONT) {
					Eye.z = 0.f;
					if (clockwise) {	// -> RIGHT
						Eye.x = -x_distance;
						viewType = VIEW_RIGHT;
					} else {		// -> LEFT
						Eye.x = x_distance;
						viewType = VIEW_LEFT;
					}
				} else if (viewType == VIEW_BACK) {
					Eye.z = 0.f;
					if (clockwise) {	// -> LEFT
						Eye.x = x_distance;
						viewType = VIEW_LEFT;
					} else {		// -> RIGHT
						Eye.x = -x_distance;
						viewType = VIEW_RIGHT;
					}
				} else if (viewType == VIEW_LEFT) {
					Eye.x = 0.f;
					if (clockwise) {	// -> FRONT
						Eye.z = z_distance;
						viewType = VIEW_FRONT;
					} else {		// -> BACK
						Eye.z = -z_distance;
						viewType = VIEW_BACK;
					}
				} else {	// RIGHT
					Eye.x = 0.f;
					if (clockwise) {	// -> BACK
						Eye.z = -z_distance;
						viewType = VIEW_BACK;
					} else {		// -> FRONT
						Eye.z = z_distance;
						viewType = VIEW_FRONT;
					}
				}
				rotationState = ROTATE_NONE;
			} else {
				//		Eye			Up
				//	Front:	(0, 0, -distance)	(0, 1, 0)
				//	Back:	(0, 0, distance)	(0, 1, 0)
				//	Right:	(distance, 0, 0)	(0, 1, 0)
				//	Left:	(-distance, 0, 0)	(0, 1, 0)
				//
				// clockwise: True
				//	FRONT -> RIGHT -> BACK -> LEFT
				// clockwise: False
				//	FRONT -> LEFT -> BACK -> RIGHT
				if (viewType == VIEW_FRONT) {
				//	Front:	(0, 0, -distance)	(0, 1, 0)
					if (clockwise) {	// -> RIGHT
				//	Right:	(distance, 0, 0)	(0, 1, 0)
						Eye.x = -x_distance * sinf(2 * D3DX_PI / fPeriod * t);
						Eye.z = z_distance * cosf(2 * D3DX_PI / fPeriod * t);
					} else {		// -> LEFT
				//	Left:	(-distance, 0, 0)	(0, 1, 0)
						Eye.x = x_distance * sinf(2 * D3DX_PI / fPeriod * t);
						Eye.z = z_distance * cosf(2 * D3DX_PI / fPeriod * t);
					}
				} else if (viewType == VIEW_BACK) {
				//	Back:	(0, 0, distance)	(0, 1, 0)
					if (clockwise) {	// -> LEFT
				//	Left:	(-distance, 0, 0)	(0, 1, 0)
						Eye.x = x_distance * sinf(2 * D3DX_PI / fPeriod * t);
						Eye.z = -z_distance * cosf(2 * D3DX_PI / fPeriod * t);
					} else {		// -> RIGHT
				//	Right:	(distance, 0, 0)	(0, 1, 0)
						Eye.x = -x_distance * sinf(2 * D3DX_PI / fPeriod * t);
						Eye.z = -z_distance * cosf(2 * D3DX_PI / fPeriod * t);
					}
				} else if (viewType == VIEW_LEFT) {
				//	Left:	(-distance, 0, 0)	(0, 1, 0)
					if (clockwise) {	// -> FRONT
				//	Front:	(0, 0, -distance)	(0, 1, 0)
						Eye.x = x_distance * cosf(2 * D3DX_PI / fPeriod * t);
						Eye.z = z_distance * sinf(2 * D3DX_PI / fPeriod * t);
					} else {		// -> BACK
				//	Back:	(0, 0, distance)	(0, 1, 0)
						Eye.x = x_distance * cosf(2 * D3DX_PI / fPeriod * t);
						Eye.z = -z_distance * sinf(2 * D3DX_PI / fPeriod * t);
					}
				} else {	// VIEW_RIGHT
				//	Right:	(distance, 0, 0)	(0, 1, 0)
					if (clockwise) {	// -> BACK
				//	Back:	(0, 0, distance)	(0, 1, 0)
						Eye.x = -x_distance * cosf(2 * D3DX_PI / fPeriod * t);
						Eye.z = -z_distance * sinf(2 * D3DX_PI / fPeriod * t);
					} else {		// -> FRONT
				//	Front:	(0, 0, -distance)	(0, 1, 0)
						Eye.x = -x_distance * cosf(2 * D3DX_PI / fPeriod * t);
						Eye.z = z_distance * sinf(2 * D3DX_PI / fPeriod * t);
					}
				}
			}
			// Update the camera's position based on time
#if 1	// SHIFT
			Eye.z += eye_x_offset;
			At.z += eye_x_offset;
#endif	// SHIFT
			g_Camera.SetViewParams2(Eye, At, Up);

			if (orthographic) {
				D3DXMATRIX	projectionMatrix;
//				D3DXMatrixOrthoLH(&projectionMatrix, width > depth ? width : depth, height, 0, 1000);
				float	L = sqrtf(Eye.x * Eye.x + Eye.z * Eye.z);	// 카메라(눈)에서 회전축가지의 거리
				float	D = sqrtf(w * w + d * d);	// D 는 위/아래에서 봤을 때 보이는 면의 대각선 길이
				D3DXMatrixOrthoLH(
					&projectionMatrix,
					1.05f * Wv,
					1.05f * Hv,
					L - .5f * D - .01f * D,
					L + .5f * D + .01f * D
				);
				g_Camera.SetProjMatrix(projectionMatrix);
			}

		}
#endif	// !SHIFT
	}
} else {
	if (state.getHomeRef()) {
		// OnCreateDevice 에서처럼...
		const D3DSURFACE_DESC*	pBackBufferSurfaceDesc = DXUTGetD3D9BackBufferSurfaceDesc();
		// Setup the projection matrix
		float	fAspect = (float) pBackBufferSurfaceDesc->Width / (float) pBackBufferSurfaceDesc->Height;
		g_Camera.SetProjParams(state.FOVY, fAspect, gMesh.radius() / 64.0f, gMesh.radius() * 200.0f);
		// Setup the arcball parameters
/*
	void	CD3DArcBall::Reset()
	{
		D3DXQuaternionIdentity(&m_qDown);
		D3DXQuaternionIdentity(&m_qNow);
		D3DXMatrixIdentity(&m_mRotation);
		D3DXMatrixIdentity(&m_mTranslation);
		D3DXMatrixIdentity(&m_mTranslationDelta);
		m_bDrag = FALSE;

		m_fRadiusTranslation = 1.0f;
		m_fRadius = 1.0f;
	}
 */
		g_ArcBall.Reset();	// 모든 파라미터를 초기화한다;
					// 하지만 m_fRadius 와 m_fRadiusTranslation 을 읽어들인 오브젝트에 맞게...
		g_ArcBall.SetWindow(pBackBufferSurfaceDesc->Width, pBackBufferSurfaceDesc->Height, 0.85f);
		g_ArcBall.SetTranslationRadius(gMesh.radius());
		state.getHomeRef() = false;
	}
//	IDirect3DDevice9*	pd3dDevice = DXUTGetD3D9Device();

	// Setup world matrix
	D3DXMATRIX	matWorld;
	D3DXMatrixIdentity(&matWorld);
	// Pivot 을 원점으로...
	D3DXMatrixTranslation(&matWorld, -pivotCenter.x, -pivotCenter.y, -pivotCenter.z);
	D3DXMATRIX	m = *g_ArcBall.GetRotationMatrix();

	D3DXMatrixMultiply(&matWorld, &matWorld, g_ArcBall.GetRotationMatrix());
#if 1	// 마지막 주문; 9. 카메라 위치 제한
	m = *g_ArcBall.GetTranslationMatrix();
	if (m._43 < gMesh.a() * kState::MIN_CAMERA_DISTANCE_FACTOR)
		m._43 = gMesh.a() * kState::MIN_CAMERA_DISTANCE_FACTOR;
	if (m._43 > gMesh.a() * kState::MAX_CAMERA_DISTANCE_FACTOR)
		m._43 = gMesh.a() * kState::MAX_CAMERA_DISTANCE_FACTOR;
	g_ArcBall.SetTranslationMatrix(m);
#endif	// 마지막 주문; 9. 카메라 위치 제한
	D3DXMatrixMultiply(&matWorld, &matWorld, g_ArcBall.GetTranslationMatrix());
#if 1	// SHIFT2
//	pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);
#if 0
	D3DXMATRIX	m_shift;
	D3DXMatrixTranslation(&m_shift, 1000.f, 0.f, 0.f);
	cout << "m: [" << endl;
	cout << m_shift._11 << ", " << m_shift._12 << ", " << m_shift._13 << ", " << m_shift._14 << endl;
	cout << m_shift._21 << ", " << m_shift._22 << ", " << m_shift._23 << ", " << m_shift._24 << endl;
	cout << m_shift._31 << ", " << m_shift._32 << ", " << m_shift._33 << ", " << m_shift._34 << endl;
	cout << m_shift._41 << ", " << m_shift._42 << ", " << m_shift._43 << ", " << m_shift._44 << endl;
	cout << "]" << endl;
	DXUTGetD3D9Device()->SetTransform(D3DTS_WORLD, &m_shift);
#endif
#endif	// SHIFT2
	g_Camera.SetWorldMatrix(matWorld);

//	D3DXMATRIX	matView;
	// 꽉차게 보이는 카메라의 위치, L 은?
	// 물체의 높이를 h 라고 할 때, tan(fovy/2) = (h/2) / L 로부터
	//	L = (h / 2) / tan(fovy / 2)
//	FLOAT	h = right_upper_far.y - left_lower_near.y;
//	FLOAT	w = right_upper_far.x - left_lower_near.x;
	float	Hv, Wv;
	float	distance = GetEyeDistance(h, w, Hv, Wv);
	distance *= 1.0f;	// 120% (덜 차보이게) -> 100% (꽉 차보이게)
	float	z_distance = (left_lower_near.z - pivotCenter.z) - distance;
	D3DXVECTOR3	vEye(0, 0, z_distance);
	D3DXVECTOR3	vAt(0, 0, 0);
#if 1	// SHIFT
	vEye.x += eye_x_offset;
	vAt.x += eye_x_offset;
#endif	// SHIFT
	D3DXVECTOR3	vUp(0, 1, 0);
//	D3DXMatrixLookAtLH(&matView, &vEye, &vAt, &vUp);
//	pd3dDevice->SetTransform(D3DTS_VIEW, &matView);
	g_Camera.SetViewParams(&vEye, &vAt);
#ifdef USE__fileOpen
#if 1	// logo
	if (gPickVar.setAllArg.connectable == CONNECTABLE_LOGO) {	// logo
		// Rotate the camera about the y-axis
		D3DXVECTOR3	vFromPt;
		D3DXVECTOR3	vLookatPt = D3DXVECTOR3(0, 0, 0);

		vFromPt.x = -cosf((float) fTime / 3.f) * z_distance;
		vFromPt.y = 0.f;
		vFromPt.z = sinf((float) fTime / 3.f) * z_distance;

		// Update the camera's position based on time
		g_Camera.SetViewParams(&vFromPt, &vLookatPt);
	}
#endif	// logo
#endif	// USE__fileOpen
#if 0
	// Rotate the camera about the y-axis
	D3DXVECTOR3	vFromPt = D3DXVECTOR3(0, 0, 0);
	D3DXVECTOR3	vLookatPt = D3DXVECTOR3(0, 0, 0);

	vFromPt.x = -cosf((float) fTime / 3.f) * CAMERA_DISTANCE;
	vFromPt.y = 1.f;
	vFromPt.z = sinf((float) fTime / 3.f) * CAMERA_DISTANCE;

	// Update the camera's position based on time
	g_Camera.SetViewParams(&vFromPt, &vLookatPt);
#endif
}
#endif

#if 1	// 색깔 등고선, 둘레, 성형
	// 색깔 등고선
	if (!state.g__showDefinedArea && !state.brush.flag.On && state.g__prID != (DWORD) -1 && state.g__prID != state.g__lastPrID) {
		state.g__index = state.g__prID;
		gMesh.setColorIsometric(state.g__prID, state.g__Shift, state.g__nChannel, state.g__nStep);
	}

	// 둘레
#if 1
	if (state.g__bString || state.g__incompleteString) {
		kMesh*	kMeshPtr = &gMesh;
		if (state.g__pickStandard && &gMesh0 && gMesh0.meshLoader().meshPtr())
			kMeshPtr = &gMesh0;
		kMesh&	meshRef = *kMeshPtr;

#pragma warning(disable: 4127)
	do {
		// 같은 위치를 가지고 두 번 계산하지 않도록...;
		//	둘레(state.g__bString): state.g__stringMeasure
		//	두 점 사이의 거리(state.g__incompleteString): state.__numIntersection, state.__lastNumIntersection
		if (state.g__bString) {
			if (!state.g__stringMeasure)
				break;
			else
				state.g__stringMeasure = false;
		}
		if (state.g__incompleteString) {
			unsigned	__numIntersection = state.__numIntersection;
			if (__numIntersection && __numIntersection % 2 != 0)	// 홀수 개의 점이 찍혔다.
				break;
			// 짝수 개의 점이 찍혔다.

			unsigned&	__lastNumIntersection = state.__lastNumIntersection;
			if (__numIntersection == __lastNumIntersection) {	// 한 번 계산했으면 다시 하지 않는다
				if (!state.g__stringMeasure)	// 강제로 다시 측정하라고 하지 않는 한
					break;
				else
					state.g__stringMeasure = false;
			} else {
				__lastNumIntersection = __numIntersection;	// 두 번 계산하지 안도록 한다.
			}
		}

		(void) fprintf(stdout, "doMeasure*\n");
		START_DEF_PTR	startDefPtr;
		START_DEF&	startDef = meshRef.tapeMeasure().startDef();
		D3DXVECTOR3*	normalPtr;
		if (startDef.type == START_DEF_TYPE__GIRTH) {
			startDefPtr.type = START_DEF_PTR_TYPE__GIRTH;
			startDefPtr.plane_def_ptr = &startDef.plane_def;
			normalPtr = &startDef.plane_def.normal;
		} else {	// START_DEF_TYPE__TWO_POINT
			startDefPtr.type = START_DEF_PTR_TYPE__TWO_POINT;
			startDefPtr.two_point_plane_def_ptr = &meshRef.tapeMeasure().startDef().two_point_plane_def;
			normalPtr = &startDef.two_point_plane_def.normal;
		}
		normal_manipulator_done_callback_manual_measure(&meshRef, 0, &startDefPtr, *normalPtr);
	} while (0);
#pragma warning(default: 4127)
	}
#endif

	// 성형
	if (state.g__plus || state.g__minus) {
		float	fNormalAmount = (state.g__Shift ? 0.1f : 0.01f) * gMesh.a();
		if (!state.g__plus)
			fNormalAmount = -fNormalAmount;
		gMesh.ChangeMesh(fNormalAmount, state.g__nChannel);
	}

	if (state.g__prID != state.g__lastPrID)
		state.g__lastPrID = state.g__prID;
#endif

#if 1	// 체형변화 시뮬레이션
	if (state.playback) {
		int&	index = state.playback_index;
#if 1	// fps
		const float	fps_inverse = 1.f / state.playback_fps;
		static float	beginTime = -1.f;	// negative means void
		static float	fTime0 = 0.f;
		if (beginTime < 0.f)
			beginTime = (float) fTime;
		if (fTime - fTime0 > fps_inverse) {
#endif	// fps
		float	dV0 = (float) (kState::VOLUME_MIN + index * kState::VOLUME_STEP);
		float	dV;
		if (gMesh.simulateWholeBody2(true, index, dV) < 0) {
			cerr << "kMesh::simulateWholeBody[" << index << "(" << dV0 << ")]: Failure" << endl;
		} else {
			cerr << "kMesh::simulateWholeBody[" << index << "(" << dV0 << ")]: Success: dV = " << dV << endl;
			if (state.playback_backward) {
				--index;
				if (index < 0) {
					index = -kState::VOLUME_MIN / kState::VOLUME_STEP;
//					state.playback = false;
					cout << "elapsed time = " << (fTime - beginTime) << endl;
					beginTime = -1.f;
				}
			} else {
				++index;
				if (kState::VOLUME_MIN + index * kState::VOLUME_STEP > kState::VOLUME_MAX) {
					index = -kState::VOLUME_MIN / kState::VOLUME_STEP;
//					state.playback = false;
					cout << "elapsed time = " << (fTime - beginTime) << endl;
					beginTime = -1.f;
				}
			}
		}
#if 1	// fps
			fTime0 = (float) fTime;
		}
#endif	// fps
	}
#endif	// 체형변화 시뮬레이션
}
#endif	// 회전을 고려


//--------------------------------------------------------------------------------------
// This callback function will be called at the end of every frame to perform all the
// rendering calls for the scene, and it will also be called if the window needs to be
// repainted. After this function has returned, DXUT will call
// IDirect3DDevice9::Present to display the contents of the next buffer in the swap chain
//--------------------------------------------------------------------------------------
void
CALLBACK OnFrameRender(
	IDirect3DDevice9*	pd3dDevice,
	double	fTime,
	float	fElapsedTime,
	void*	pUserContext
)
{
#ifdef USE__pointer_to_PickVar
	if (!gPickVarPtr) {
		HRESULT	hr;
		V(pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 64, 64, 64), 1.0f, 0));
		return;
	}
	PickVar&	gPickVar = *gPickVarPtr;
#endif	// USE__pointer_to_PickVar
#ifdef USE__PickVar
	kState&	state = gPickVar.state;
	CD3DArcBall&	g_ArcBall = gPickVar.g_ArcBall;
#ifdef USE__pointer_to_kMesh
	kMesh*&	gkMeshPtr = gPickVar.gkMeshPtr;
	kMesh*&	gkMeshPtr0 = gPickVar.gkMeshPtr0;
#else	// USE__pointer_to_kMesh/!USE__pointer_to_kMesh
	kMesh&	gMesh = gPickVar.gMesh;
	kMesh&	gMesh0 = gPickVar.gMesh0;
#endif	// !USE__pointer_to_kMesh
	CModelViewerCamera2&	g_Camera = gPickVar.g_Camera;
	CD3DSettingsDlg&	g_SettingsDlg = gPickVar.g_SettingsDlg;
#ifndef __KEKO__
#if 0
	CDXUTDialog&	g_HUD = gPickVar.g_HUD;
	CDXUTDialog&	g_SampleUI = gPickVar.g_SampleUI;
#endif
#endif	// !__KEKO__
#endif	// USE__PickVar
#ifdef USE__pointer_to_kMesh
	if (!gkMeshPtr) {
		HRESULT	hr;
		V(pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 64, 64, 64), 1.0f, 0));
		return;
	}
	kMesh&	gMesh = *gkMeshPtr;
	kMesh&	gMesh0 = *gkMeshPtr0;
#endif	// USE__pointer_to_kMesh
//	cout << "OnFrameRender" << endl;
	kMesh*	kMeshPtr = &gMesh;
	if (state.g__pickStandard && &gMesh0 && gMesh0.meshLoader().meshPtr())
		kMeshPtr = &gMesh0;
	kMesh&	meshRef = *kMeshPtr;

	HRESULT	hr;

	// If the settings dialog is being shown, then
	// render it instead of rendering the app's scene
	if (g_SettingsDlg.IsActive()) {
		g_SettingsDlg.OnRender(fElapsedTime);
		return;
	}

	// Get the projection & view matrix from the camera class
	D3DXMATRIX	mWorld = *g_Camera.GetWorldMatrix();
	D3DXMATRIX	mView = *g_Camera.GetViewMatrix();
	D3DXMATRIX	mProj = *g_Camera.GetProjMatrix();
	D3DXMATRIX	mWorldViewProjection = mWorld * mView * mProj;
	// kMesh 안에서 렌더링하기 위해 필요한 행렬을 설정한다;
	gMesh.setMatrix(mWorld, mView, mProj, *g_ArcBall.GetRotationMatrix());
	if (&gMesh0)
		gMesh0.setMatrix(mWorld, mView, mProj, *g_ArcBall.GetRotationMatrix());

	// Check for Picked triangles
	Pick();

	// Clear the render target and the zbuffer
//	V( pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 45, 50, 170), 1.0f, 0) );
	V(pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 64, 64, 64), 1.0f, 0));

	// Render the scene
	if (FAILED(pd3dDevice->BeginScene()))
		return;

	// See, IrradianceCache.cpp
#if 0
{
	D3DXMATRIX	m_shift;
	D3DXMatrixTranslation(&m_shift, 1000.f, 0.f, 0.f);
	cout << "m: [" << endl;
	cout << m_shift._11 << ", " << m_shift._12 << ", " << m_shift._13 << ", " << m_shift._14 << endl;
	cout << m_shift._21 << ", " << m_shift._22 << ", " << m_shift._23 << ", " << m_shift._24 << endl;
	cout << m_shift._31 << ", " << m_shift._32 << ", " << m_shift._33 << ", " << m_shift._34 << endl;
	cout << m_shift._41 << ", " << m_shift._42 << ", " << m_shift._43 << ", " << m_shift._44 << endl;
	cout << "]" << endl;
	DXUTGetD3D9Device()->SetTransform(D3DTS_WORLD, &m_shift);
}
#endif

#if 1	// 디버깅을 위해 특정 면/선/점을 출력
	gMesh.select().ShowEdge(state.debugging_EdgeID_array, D3DXVECTOR4(1, 0, 0, 1));
	gMesh.select().ShowFace(state.debugging_FaceID_array);
	gMesh.select().ShowVertex(state.debugging_prReprID_array, 7.f, D3DXVECTOR4(0, 0, 0, 1));

	// kState::points 에 있는 점들을 찍는다; 디버깅용
	gMesh.PlotPoints(state.points, 1.f);
#endif
#ifndef __KEKO__
#if 1	// AREA2_HINT
	if (state.area2_control.ShowArea2 == 1 || state.area2_control.ShowArea2 == 2) {
//		gMesh.select().ShowFace(state.area2_face_array);
		gMesh.select().ShowVertex(state.area2_vertex_array, 3.f, D3DXVECTOR4(1, 1, 0, 1));
	}
#endif	// AREA2_HINT
#endif	// !__KEKO__

#if 1	// ColorBand
	gMesh.ShowColorBand();
#endif	// ColorBand

	gMesh.RenderManipulator();

	// 표준모델 따라가기
	if (state.g__follow && &gMesh0 && gMesh0.meshLoader().meshPtr()) {
		gMesh.follow(gMesh0, !state.g__Shift);
		state.g__follow = false;
	}

	// Picking 된 점 표시
	if (
		(state.g__bString || state.g__incompleteString) &&
		state.__nearestIntersectionIndex != -1 &&
		(meshRef.manipulatorPtr() == 0 || meshRef.manipulatorPtr()->isSet() == false)	// manipulator 가 설정되지 않은 경우
	) {
		meshRef.ShowVertex(
			meshRef.intersectionVertexBufferPtr(),
			meshRef.OnePointDeclarationPtr(),
			sizeof(D3DXVECTOR3),	// stride
			state.g__incompleteString && state.__numIntersection > 1 && state.__nextIntersectionIndex == 0 ? 2 : 1,
			5.f,
			D3DXVECTOR4(1, 1, 0, 1)
		);
	}

	// 특정영역을 표시하기 위한 정보 출력
#if 0
	if (state.printFaceIDList) {
		if (meshRef.meshLoader().meshPtr()) {
#if 1	// debugging
		const std::set<DWORD>&	selectedPrIDSet = gMesh.select().selectedPrIDSet();
		for (std::set<DWORD>::const_iterator it = selectedPrIDSet.begin(); it != selectedPrIDSet.end(); ++it) {
			DWORD	prID = *it;
			cout << prID << endl;
		}
#endif	// debugging
			// 표준모델에서 선택된 점들을 포함하는 Face 들의 목록을 출력한다;
			// Face 의 순서는 같으므로 측정모델에서 해당 영역을 선택할 때 사용된다.
			std::vector<DWORD>	selectedFaceIDList;
			meshRef.select().GetSelectedFaceIDList(selectedFaceIDList);
			int	n = selectedFaceIDList.size();
			(void) fprintf(stdout, "Selected Face IDs...\n");
#if 1	// VK_RETURN
			const char*	varName = state.message.varName;
#ifndef __KEKO__
			(void) fprintf(state.filePtr, "// /* ... */	{%s, %d},\n", varName, n);
#endif	// !__KEKO__
			(void) fprintf(stdout, "// /* ... */	{%s, %d},\n", varName, n);
#ifndef __KEKO__
			(void) fprintf(state.filePtr, "\tDWORD	%s[%d] = {\t// varName:= |%s| (%d)\n", varName, n, varName, n);
#endif	// !__KEKO__
			(void) fprintf(stdout, "\tDWORD	%s[%d] = {\t// varName:= |%s| (%d)\n", varName, n, varName, n);
#else	// VK_RETURN/!VK_RETURN
			(void) fprintf(stdout, "{\n");
#endif	// !VK_RETURN
			for (int i = 0; i < n; ++i) {
				if (i % 5 == 0) {
#ifndef __KEKO__
					(void) fprintf(state.filePtr, "%s\t\t", i ? "\n" : "");
#endif	// !__KEKO__
					(void) fprintf(stdout, "%s\t\t", i ? "\n" : "");
				}
#ifndef __KEKO__
				(void) fprintf(state.filePtr, "%d,", selectedFaceIDList.at(i));
#endif	// !__KEKO__
				(void) fprintf(stdout, "%d,", selectedFaceIDList.at(i));
			}
			if (n) {
#ifndef __KEKO__
				(void) fprintf(state.filePtr, "\n");
#endif	// !__KEKO__
				(void) fprintf(stdout, "\n");
			}
#if 1	// VK_RETURN
#ifndef __KEKO__
			(void) fprintf(state.filePtr, "\t};\t// varName: |%s| (%d)\n", varName, n);
#endif	// !__KEKO__
			(void) fprintf(stdout, "\t};\t// varName: |%s| (%d)\n", varName, n);
#else	// VK_RETURN/!VK_RETURN
			(void) fprintf(stdout, "\t};\n");
#endif	// !VK_RETURN
			(void) fprintf(stdout, "# of Selected Face IDs = %d\n", n);
		}

		state.printFaceIDList = false;
	}
#endif
	// 특정영역을 표시
	meshRef.ShowDefinedArea();
	meshRef.ShowBodyElement();

	// 색깔템플릿 표시를 위한 정보 출력
#if 0
	if (state.printColor) {
		(void) fprintf(stdout, "TEMPLATE_COLOR...\n");
		LPDIRECT3DVERTEXBUFFER9	pVB = meshRef.GetVertexBufferPointer();
		kMesh::D3DXVERTEX*	pV;
		pVB->Lock(0, 0, (void**) &pV, 0);
		const CPointRepr*	pPointRepr = meshRef.GetPointReprPointer();
		int	n = 0;
		// n 을 정하기 위해...
		for (DWORD i = 0; i < meshRef.GetNumPointReprs(); ++i) {
			DWORD	prID = pPointRepr[i].id;
			const D3DXVECTOR3&	cColor = pV[prID].c;
			if (cColor.x == 0.f && cColor.y == 0.f && cColor.z == 0.f)
				continue;

			FACE_ID_LOCAL_INDEX	faceIDLocalIndex;
			if (meshRef.GetFaceIDLocalIndex(prID, faceIDLocalIndex) == 0)
				++n;
		}
#if 1	// VK_RETURN
		const char*	varName = state.message.varName;
#ifndef __KEKO__
		(void) fprintf(state.filePtr, "// /* ... */	{%s, %d},\n", varName, n);
#endif	// !__KEKO__
		(void) fprintf(stdout, "// /* ... */	{%s, %d},\n", varName, n);
#ifndef __KEKO__
		(void) fprintf(state.filePtr, "\tMODEL_COLOR	%s[%d] = {\t// varName:= |%s| (%d)\n", varName, n, varName, n);
#endif	// !__KEKO__
		(void) fprintf(stdout, "\tMODEL_COLOR	%s[%d] = {\t// varName:= |%s| (%d)\n", varName, n, varName, n);
#endif	// VK_RETURN
		for (DWORD i = 0; i < meshRef.GetNumPointReprs(); ++i) {
			DWORD	prID = pPointRepr[i].id;
			const D3DXVECTOR3&	cColor = pV[prID].c;
			if (cColor.x == 0.f && cColor.y == 0.f && cColor.z == 0.f)
				continue;

			FACE_ID_LOCAL_INDEX	faceIDLocalIndex;
			if (meshRef.GetFaceIDLocalIndex(prID, faceIDLocalIndex) == 0) {
#ifndef __KEKO__
				(void) fprintf(state.filePtr, "\t\t{%d, %d,\t%ff, %ff, %ff},\n", faceIDLocalIndex.ID, faceIDLocalIndex.local_index, cColor.x, cColor.y, cColor.z);
#endif	// !__KEKO__
				(void) fprintf(stdout, "\t\t{%d, %d,\t%ff, %ff, %ff},\n", faceIDLocalIndex.ID, faceIDLocalIndex.local_index, cColor.x, cColor.y, cColor.z);
			}
		}
#if 1	// VK_RETURN
#ifndef __KEKO__
		(void) fprintf(state.filePtr, "\t};\t// varName: |%s| (%d)\n", varName, n);
#endif	// !__KEKO__
		(void) fprintf(stdout, "\t};\t// varName: |%s| (%d)\n", varName, n);
#endif	// VK_RETURN
		pVB->Unlock();

		state.printColor = false;
	}
#endif
	// 색깔템플릿 표시
	if (state.g__showTemplateColor) {
		meshRef.ShowTemplateColor();
	}

	// 계측부위 표시
	if (state.g__showDefinedMeasure) {
		meshRef.Render__TAPE_MEASURE();
	}

	// 표준모델
	if ((state.modelType & MODEL_TYPE__STANDARD) && &gMesh0 && gMesh0.meshLoader().meshPtr()) {
		// 표준모델의 키를 측정모델에 맞춰 Scaling 하기 전의, 원래 표준모델
//		cout << "표준모델" << endl;
		gMesh0.ShowModel();
	}
	if (state.g__subset) {	// kMeshLoader
#if 0
		DWORD	cullMode;
		pd3dDevice->GetRenderState(D3DRS_CULLMODE, &cullMode);
		pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		meshRef.ShowSubsetFace();
		pd3dDevice->SetRenderState(D3DRS_CULLMODE, cullMode);
#else
		meshRef.ShowSubsetFace();
#endif
	}

	// 측정모델에서 선택된 점에 해당하는, 표준모델의 대응점을 보여준다.
	if ((state.modelType & MODEL_TYPE__STANDARD) && &gMesh0 && gMesh0.meshLoader().meshPtr()) {
		state.debugging_prReprID_array0.resize(0);
		const std::set<DWORD>&	selectedPrIDSet = gMesh.select().selectedPrIDSet();
		for (std::set<DWORD>::const_iterator it = selectedPrIDSet.begin(); it != selectedPrIDSet.end(); ++it) {
			DWORD	prID = *it;
			FACE_ID_LOCAL_INDEX	faceIDLocalIndex;
			int	rval = gMesh.GetFaceIDLocalIndex(prID, faceIDLocalIndex);
			assert(rval == 0);
			if (rval == 0) {
				DWORD	prID0 = gMesh0.GetPrID(faceIDLocalIndex);
				assert(prID0 != (DWORD) -1);
				if (prID0 != (DWORD) -1) {
					state.debugging_prReprID_array0.push_back(prID0);
				}
			}
		}
		gMesh0.select().ShowVertex(state.debugging_prReprID_array0, 5.f, D3DXVECTOR4(1, 1, 0, 1));
	}

#if 0	// group
	// .obj 에서 읽은 그룹별로 보여준다.
	if (state.g__showGroup) {
		gMesh.ShowVertex(gMesh.GetVertexBufferPointer(), gMesh.GetVertexDeclarationPointer(), sizeof(kMesh::D3DXVERTEX), gMesh.GetNumPointReprs());
		int	showType = 0;
		const OBJECT_TYPE&	objectType = gMesh.getModelRef().getObjectRef();
		if (objectType & OBJECT_VERTEX)
			showType ^= 0x01;
		if (objectType & OBJECT_EDGE)
			showType ^= 0x02;
		if (objectType & OBJECT_FACE)
			showType ^= 0x04;
		gMesh.ShowGroup(state.g__gid, showType);
	}
#endif

	// 왼쪽 아래에 세 축을 표시주는 표시
	if (state.g__drawAxis) {
		D3DXMATRIX	mCameraRot;
		D3DXMatrixInverse(&mCameraRot, NULL, g_ArcBall.GetRotationMatrix());

		// Transform vectors based on camera's rotation matrix
		D3DXVECTOR3	vWorldUp, vWorldRight, vWorldAhead;
		D3DXVECTOR3	vLocalUp = D3DXVECTOR3(0, 1, 0);
		D3DXVECTOR3	vLocalRight = D3DXVECTOR3(1, 0, 0);
		D3DXVECTOR3	vLocalAhead = D3DXVECTOR3(0, 0, 1);
		D3DXVec3TransformCoord(&vWorldUp, &vLocalUp, &mCameraRot);
		D3DXVec3TransformCoord(&vWorldRight, &vLocalRight, &mCameraRot);
		D3DXVec3TransformCoord(&vWorldAhead, &vLocalAhead, &mCameraRot);

		gMesh.DrawAxis(vWorldUp, vWorldRight);
	}
	// 모델의 원점과 축을 표시
	if (state.g__drawAxis0)
		gMesh.DrawAxis0();
	// 화면상의 중심점과 축을 표시
	if (state.g__drawPivot)
		gMesh.DrawPivot();

	// 임의의 둘레 표시
	if (state.__showSegment)
		meshRef.ShowSegment();

	// 성형 후 모양 표시
	if (state.g__showShape)
		meshRef.ShowShape(.01f * meshRef.GetTotalVolume());

	// 경계박스 표시
	if (state.g__showBoundingBox)
		meshRef.ShowBoundingBox();
	// 경계구 표시
	if (state.g__showBoundingSphere)
		meshRef.ShowBoundingSphere();

	// 등고선 표시
	meshRef.ShowIsometric();

	// 단면(2D) 표시
	if (
		(state.g__bString || state.g__incompleteString) &&	// 계측 모드
#if 1	// manipulator; LINE MANIPULATOR
		meshRef.tapeMeasure().isReleased() == false &&	// line manipulator 에 의해 선이 변화하지 않은 경우
#endif	// manipulator; LINE MANIPULATOR
		state.g__showContour		// 단면 표시: On
	) {
		meshRef.select().DrawContour(
			meshRef._2dProjectNumPoints(),
			meshRef._2dProjectVertexBufferPtr(),
			3.f,
			D3DXVECTOR4(1, 0, 0, 1),

			meshRef._2dInterpolatedProjectNumPoints(),
			meshRef._2dInterpolatedProjectVertexBufferPtr(),
			1.f,
			D3DXVECTOR4(0, 1, 0, 1),

			meshRef._2dFillProjectNumPoints(),
			meshRef._2dFillProjectVertexBufferPtr(),
			2.f,
			D3DXVECTOR4(0, 0, 1, 1)
		);
	}
	// 측정모델
	if (state.modelType & MODEL_TYPE__REAL) {
		gMesh.ShowModel();
	}

	// 모델 위에 표시되어야 할 것들은 kMesh::ShowModel 다음에 와야 한다!!!

	// 각도 표시
	gMesh.select().ShowAngle(mWorld, mView, mProj, state.g__startPoint, state.g__endPoint, 0.f);

	// 모눈 표시
	gMesh.DrawGrid();

	// 브러쉬원 표시
	if (state.brush.flag.On) {
		HWND	hWnd = GetCapture();
		if (!hWnd)
			SetCapture(DXUTGetHWND());	// HWND
		POINT	ptCursor;
		GetCursorPos(&ptCursor);
		ScreenToClient(DXUTGetHWND(), &ptCursor);
		if (!hWnd)
			ReleaseCapture();
		FLOAT		pointSize = 5.f;
		D3DXVECTOR4	pointColor(1.f, 0.f, 1.f, 1.f);
		gMesh.select().DrawCircle(ptCursor.x, ptCursor.y, state.brush.Radius, pointSize, &pointColor);
	}

	// 단면 표시(3D)
	if (state.g__bString && state.g__showLowerFace) {
		meshRef.ShowLowerFace(state.lower_FaceID_array);
//		meshRef.ShowLower2Face(state.lower2_FaceID_array, mWorld, mWorldViewProjection);
		meshRef.ShowLower2Face(state.lowerFaceArray, mWorld, mWorldViewProjection);
	}

	// 선택사각형(marquee) 표시
	if (!state.brush.flag.On && state.g__Control && state.g__bDrag) {
		meshRef.select().DrawRectangle(state.g__iDragBeginX, state.g__iDragBeginY, state.g__iMouseX, state.g__iMouseY);
	}

	// 선택된 점들 표시
	if (meshRef.select().GetNumberOfPoints()) {
		if (!state.brush.flag.On) {	// 브러쉬가 제대로 작동하는 지 보려면 이 if 문을 없앤다.
			FLOAT		pointSize = 5.f;
			D3DXVECTOR4	pointColor(1.f, 0.f, 1.f, 1.f);
			meshRef.select().ShowPoints(mWorldViewProjection, pointSize, pointColor);
		}
	}


	DXUT_BeginPerfEvent(DXUT_PERFEVENTCOLOR, L"HUD / Stats");	// These events are to help PIX identify what the code is doing
	RenderText();
#ifndef __KEKO__
#if 0
	V(g_HUD.OnRender(fElapsedTime));
	V(g_SampleUI.OnRender(fElapsedTime));
#endif
#endif	// !__KEKO__
	DXUT_EndPerfEvent();

	V(pd3dDevice->EndScene());
}

//--------------------------------------------------------------------------------------
// Checks if mouse point hits geometry the scene.
//--------------------------------------------------------------------------------------
HRESULT
Pick(
	void
)
{
#ifdef USE__pointer_to_PickVar
	if (!gPickVarPtr)
		return S_OK;
	PickVar&	gPickVar = *gPickVarPtr;
#endif	// USE__pointer_to_PickVar
#ifdef USE__PickVar
	kState&	state = gPickVar.state;
#ifdef USE__pointer_to_kMesh
	kMesh*&	gkMeshPtr = gPickVar.gkMeshPtr;
	kMesh*&	gkMeshPtr0 = gPickVar.gkMeshPtr0;
#else	// USE__pointer_to_kMesh/!USE__pointer_to_kMesh
	kMesh&		gMesh = gPickVar.gMesh;
	kMesh&	gMesh0 = gPickVar.gMesh0;
#endif	// !USE__pointer_to_kMesh
	CModelViewerCamera2&	g_Camera = gPickVar.g_Camera;
#endif	// USE__PickVar
#ifdef USE__pointer_to_kMesh
	if (!gkMeshPtr)
		return S_OK;
	kMesh&	gMesh = *gkMeshPtr;
	kMesh&	gMesh0 = *gkMeshPtr0;
#endif	// USE__pointer_to_kMesh
//	cout << "Pick" << endl;
	kMesh*	kMeshPtr = &gMesh;
	if (state.g__pickStandard && &gMesh0 && gMesh0.meshLoader().meshPtr())
		kMeshPtr = &gMesh0;
	kMesh&	meshRef = *kMeshPtr;

	D3DXMATRIX	mWorld = *g_Camera.GetWorldMatrix();
	D3DXMATRIX	mProj = *g_Camera.GetProjMatrix();
	D3DXMATRIX	mView = *g_Camera.GetViewMatrix();
	D3DXMATRIX	mWorldViewProjection = mWorld * mView * mProj;

	state.__dwNumIntersections = 0L;
	if (GetCapture() != DXUTGetHWND())	// 마우스를 클릭하지 않았다면 건너뛴다;
						// 자기 윈도우만 클릭했을 때
		return S_OK;

	if (state.SetGridCenter) {
		POINT	ptCursor;
		GetCursorPos(&ptCursor);
		ScreenToClient(DXUTGetHWND(), &ptCursor);
//		cout << "[" << ptCursor.x << ", " << ptCursor.y << "]" << endl;
		D3DVIEWPORT9	viewport;
		DXUTGetD3D9Device()->GetViewport(&viewport);
		if (ptCursor.x >= (LONG) viewport.Width)
			ptCursor.x = viewport.Width - 1;
		if (ptCursor.x < 0)
			ptCursor.x = 0;
		if (ptCursor.y >= (LONG) viewport.Height)
			ptCursor.y = viewport.Height - 1;
		if (ptCursor.y < 0)
			ptCursor.y = 0;
//		cout << "[" << ptCursor.x << ", " << ptCursor.y << "]" << endl;
		GRID_CENTER&	center = state.getCenterRef();
		center.Ox = (2.f * ptCursor.x / viewport.Width - 1.f);
		center.Oy = -(2.f * ptCursor.y / viewport.Height - 1.f);
		return S_OK;
	}
#if 1	// Pick
#if 1	// Picking 에 사용되는 Ray 생성
	D3DXVECTOR3	vPickRayDir;
	D3DXVECTOR3	vPickRayOrig;

	// Get the Pick ray from the mouse position
//	if (GetCapture())
{
	POINT	ptCursor;
	GetCursorPos(&ptCursor);
	ScreenToClient(DXUTGetHWND(), &ptCursor);
#if 0	// PERSPECTIVE 에만 맞는 식이다.
	const D3DSURFACE_DESC*	pd3dsdBackBuffer = DXUTGetD3D9BackBufferSurfaceDesc();
	const D3DXMATRIX*	pmatProj = g_Camera.GetProjMatrix();

	// Compute the vector of the Pick ray in screen space
	D3DXVECTOR3	v;
#if 1
	v.x = 2.f * ptCursor.x / pd3dsdBackBuffer->Width - 1;
	v.y = -(2.f * ptCursor.y / pd3dsdBackBuffer->Height - 1);
	v.x /= pmatProj->_11;
	v.y /= pmatProj->_22;
	v.z =  1.f;
#else
	v.x =  (((2.0f * ptCursor.x) / pd3dsdBackBuffer->Width) - 1) / pmatProj->_11;
	v.y = -(((2.0f * ptCursor.y) / pd3dsdBackBuffer->Height) - 1) / pmatProj->_22;
	v.z =  1.0f;
#endif

	// Get the inverse view matrix
	const D3DXMATRIX	matView = *g_Camera.GetViewMatrix();
	const D3DXMATRIX	matWorld = *g_Camera.GetWorldMatrix();
	D3DXMATRIX	mWorldView = matWorld * matView;
	D3DXMATRIX	m;
	D3DXMatrixInverse(&m, NULL, &mWorldView);

	// Transform the screen space Pick ray into 3D space
	vPickRayDir.x  = v.x*m._11 + v.y*m._21 + v.z*m._31;
	vPickRayDir.y  = v.x*m._12 + v.y*m._22 + v.z*m._32;
	vPickRayDir.z  = v.x*m._13 + v.y*m._23 + v.z*m._33;
	vPickRayOrig.x = m._41;
	vPickRayOrig.y = m._42;
	vPickRayOrig.z = m._43;
	D3DXVECTOR3	vPickRayDirNormalized;
	D3DXVec3Normalize(&vPickRayDirNormalized, &vPickRayDir);
#else
	D3DVIEWPORT9	viewport;
	DXUTGetD3D9Device()->GetViewport(&viewport);
	D3DXVECTOR3	o1 = D3DXVECTOR3((FLOAT) ptCursor.x, (FLOAT) ptCursor.y, 0.f);
	D3DXVECTOR3	O1;	// Object 공간에서의 좌표
	D3DXVec3Unproject(&O1, &o1, &viewport, &mProj, &mView, &mWorld);
	D3DXVECTOR3	o2 = D3DXVECTOR3((FLOAT) ptCursor.x, (FLOAT) ptCursor.y, 1.f);
	D3DXVECTOR3	O2;	// Object 공간에서의 좌표
	D3DXVec3Unproject(&O2, &o2, &viewport, &mProj, &mView, &mWorld);
	D3DXVECTOR3	v = O2 - O1;
	D3DXVECTOR3	vNormalized;
	D3DXVec3Normalize(&vNormalized, &v);

	vPickRayOrig = O1;
	vPickRayDir = vNormalized;
#endif
}
#endif
	// Get the Picked triangle
	int&		nearestIntersectionIndex = state.__nearestIntersectionIndex;
	DWORD&		dwNumIntersections = state.__dwNumIntersections;
	INTERSECTION*	intersectionArray = state.__intersectionArray;
	meshRef.Pick(
		meshRef.GetMeshPointer(),
		// [IN]
		true, false, vPickRayOrig, vPickRayDir,
		// [OUT]
		nearestIntersectionIndex, dwNumIntersections, intersectionArray
	);
	state.intersection_ptr = nearestIntersectionIndex == -1 ? NULL : &intersectionArray[nearestIntersectionIndex];
#endif	// Pick

#if 1
	// 마우스를 클릭했다.
// 3. 조작툴(manipulator) 모드
	if (state.manipulator_mode) {
		return S_OK;
	}
#endif

// 1. 브러쉬 모드
	// 브러쉬로 칠한다.
	if (state.brush.flag.On) {
		if (!state.brush.flag.Do)
			return S_OK;

		meshRef.brush();

		return S_OK;
	}

// 2. 피킹 모드
#if 0	// Picking 에 사용되는 Ray 생성
	D3DXVECTOR3	vPickRayDir;
	D3DXVECTOR3	vPickRayOrig;

	// Get the Pick ray from the mouse position
//	if (GetCapture())
{
	POINT	ptCursor;
	GetCursorPos(&ptCursor);
	ScreenToClient(DXUTGetHWND(), &ptCursor);
#if 0	// PERSPECTIVE 에만 맞는 식이다.
	const D3DSURFACE_DESC*	pd3dsdBackBuffer = DXUTGetD3D9BackBufferSurfaceDesc();
	const D3DXMATRIX*	pmatProj = g_Camera.GetProjMatrix();

	// Compute the vector of the Pick ray in screen space
	D3DXVECTOR3	v;
#if 1
	v.x = 2.f * ptCursor.x / pd3dsdBackBuffer->Width - 1;
	v.y = -(2.f * ptCursor.y / pd3dsdBackBuffer->Height - 1);
	v.x /= pmatProj->_11;
	v.y /= pmatProj->_22;
	v.z =  1.f;
#else
	v.x =  (((2.0f * ptCursor.x) / pd3dsdBackBuffer->Width) - 1) / pmatProj->_11;
	v.y = -(((2.0f * ptCursor.y) / pd3dsdBackBuffer->Height) - 1) / pmatProj->_22;
	v.z =  1.0f;
#endif

	// Get the inverse view matrix
	const D3DXMATRIX	matView = *g_Camera.GetViewMatrix();
	const D3DXMATRIX	matWorld = *g_Camera.GetWorldMatrix();
	D3DXMATRIX	mWorldView = matWorld * matView;
	D3DXMATRIX	m;
	D3DXMatrixInverse(&m, NULL, &mWorldView);

	// Transform the screen space Pick ray into 3D space
	vPickRayDir.x  = v.x*m._11 + v.y*m._21 + v.z*m._31;
	vPickRayDir.y  = v.x*m._12 + v.y*m._22 + v.z*m._32;
	vPickRayDir.z  = v.x*m._13 + v.y*m._23 + v.z*m._33;
	vPickRayOrig.x = m._41;
	vPickRayOrig.y = m._42;
	vPickRayOrig.z = m._43;
	D3DXVECTOR3	vPickRayDirNormalized;
	D3DXVec3Normalize(&vPickRayDirNormalized, &vPickRayDir);
#else
	D3DVIEWPORT9	viewport;
	DXUTGetD3D9Device()->GetViewport(&viewport);
	D3DXVECTOR3	o1 = D3DXVECTOR3((FLOAT) ptCursor.x, (FLOAT) ptCursor.y, 0.f);
	D3DXVECTOR3	O1;	// Object 공간에서의 좌표
	D3DXVec3Unproject(&O1, &o1, &viewport, &mProj, &mView, &mWorld);
	D3DXVECTOR3	o2 = D3DXVECTOR3((FLOAT) ptCursor.x, (FLOAT) ptCursor.y, 1.f);
	D3DXVECTOR3	O2;	// Object 공간에서의 좌표
	D3DXVec3Unproject(&O2, &o2, &viewport, &mProj, &mView, &mWorld);
	D3DXVECTOR3	v = O2 - O1;
	D3DXVECTOR3	vNormalized;
	D3DXVec3Normalize(&vNormalized, &v);

	vPickRayOrig = O1;
	vPickRayDir = v;
#endif
}
#endif

// 어떤 보조키(SHIFT, CTRL, ALT)도 눌리지 않았을 경우
	// 특정 뷰(FRONT/BACK/RIGHT/LEFT)에서 각도를 표시하기 위한 좌표 획득
	if (!(state.SetGridCenter || state.doMeasure || state.g__Shift || state.g__Control || state.g__Alt)) {
		const VIEW_T&	view = state.getViewRef();
		if (view.type == VIEW_PERSPECTIVE || !view.orthographic)
			return S_OK;

//		ReleaseCapture();
		POINT	ptCursor;
		GetCursorPos(&ptCursor);
//		cout << "hello, world: (" << state.g__startPoint.x << ", " << state.g__endPoint.x << "): (" << ptCursor.x << ", " << ptCursor.y << ")" << endl;
//		cout << "state.g__LBUTTONDOWN: " << (state.g__LBUTTONDOWN ? "True" : "False") << endl;
//		cout << "state.g__RBUTTONDOWN: " << (state.g__RBUTTONDOWN ? "True" : "False") << endl;
		ScreenToClient(DXUTGetHWND(), &ptCursor);
		if (state.g__RBUTTONDOWN) {	// 삭제
			if (state.g__startPoint.x == -1) {	// 삭제할 것이 없다;
				// 무시한다.
			} else {
				if (state.g__endPoint.x == -1) {	// 시작점을 무효로 한다.
					state.g__startPoint.x = -1;
					state.g__startPoint.y = -1;
				} else {			// 끝점을 무효로 한다.
					state.g__endPoint.x = -1;
					state.g__endPoint.y = -1;
				}
			}
		} else {		// 추가
			if (state.g__startPoint.x == -1) {		// 시작점에 추가한다.
//		cout << "ptCursor: (" << ptCursor.x << ", " << ptCursor.y << ")" << endl;
				state.g__startPoint.x = ptCursor.x;
				state.g__startPoint.y = ptCursor.y;
			} else if (state.g__endPoint.x == -1) {	// 끝점에 추가한다.
				if (state.g__startPoint.x != ptCursor.x || state.g__startPoint.y != ptCursor.y) {
//		cout << "ptCursor: (" << ptCursor.x << ", " << ptCursor.y << ")" << endl;
				state.g__endPoint.x = ptCursor.x;
				state.g__endPoint.y = ptCursor.y;
	if (meshRef.__hWnd) {
		float	angle = 180.f / D3DX_PI * atan2f(fabsf((float) state.g__endPoint.y - (float) state.g__startPoint.y), fabsf((float) state.g__endPoint.x - (float) state.g__startPoint.x));
		cout << "sizeof(WPARAM): " << sizeof(WPARAM) << endl;
		cout << "sizeof(LPARAM): " << sizeof(LPARAM) << endl;
		cout << "PostMessage: " << (
			PostMessage(
				meshRef.__hWnd,
				WM_USER__ANGLE,
				(WPARAM) 1,
				(LPARAM) *((int*) &angle)
			) ? "Ok" : "Oops"
		) << endl;
	}
				}
			} else {	// 추가할 곳이 없다.
#if 1
				if (state.g__endPoint.x != ptCursor.x || state.g__endPoint.y != ptCursor.y) {
//		cout << "ptCursor: (" << ptCursor.x << ", " << ptCursor.y << ")" << endl;
				state.g__startPoint.x = ptCursor.x;
				state.g__startPoint.y = ptCursor.y;
				state.g__endPoint.x = -1;
				state.g__endPoint.y = -1;
				}
#else
				cout << "hello, world: No..." << endl;
				// 무시한다.
#endif
			}
		}
//		ReleaseCapture();
//		SetCapture(DXUTGetHWND());
		return S_OK;
	}

// CTRL
	// 점들을 선택
	if (state.g__Control) {
		meshRef.select().selection(meshRef, g_Camera);
//		meshRef.selection(state.g__iMouseX, state.g__iMouseY, state.g__bDrag, state.g__Shift, state.g__iDragBeginX, state.g__iDragBeginY, state.g__NSelectedPoints, state.g__prID);
#if 0
		if (state.g__bString && !state.g__incompleteString && state.g__prID != (DWORD) -1) {
			meshRef.tapeMeasure().setBasePoint((DWORD) -1, 0, state.g__prID, (DWORD) -1);
			state.g__stringMeasure = true;
		}
#endif

		return S_OK;
	}

// SHIFT or ALT...
#if 0
	// Get the Picked triangle
	int&		nearestIntersectionIndex = state.__nearestIntersectionIndex;
	DWORD&		dwNumIntersections = state.__dwNumIntersections;
	INTERSECTION*	intersectionArray = state.__intersectionArray;
	meshRef.Pick(
		meshRef.GetMeshPointer(),
		// [IN]
		true, false, vPickRayOrig, vPickRayDir,
		// [OUT]
		nearestIntersectionIndex, dwNumIntersections, intersectionArray
	);
#endif
	if (nearestIntersectionIndex == -1) {	// no picking
		return S_OK;
	}

	if (state.g__Alt) {
		MANIPULATOR_TYPE_T	type;
		meshRef.getValue(TYPE__MANIPULATOR, MANIPULATOR__TYPE, &type);
		if (type == NORMAL_MANIPULATOR)
			meshRef.SetNormalManipulator(intersectionArray[nearestIntersectionIndex], state.g__n);
		else if (type == POINT_MANIPULATOR)
			meshRef.SetPointManipulator(intersectionArray[nearestIntersectionIndex]);

		nearestIntersectionIndex = -1;
		return S_OK;
	}

	INTERSECTION&	__lastIntersection		= state.__lastIntersection;
	unsigned&	__numIntersection		= state.__numIntersection;
	int&		__nextIntersectionIndex		= state.__nextIntersectionIndex;
	INTERSECTION*	__twoIntersection		= state.__twoIntersection;

	if (__lastIntersection == intersectionArray[nearestIntersectionIndex]) {	// 서로 다른 INTERSECTION...
		return S_OK;
	}
	__lastIntersection = intersectionArray[nearestIntersectionIndex];		// 서로 다른 INTERSECTION...

	// INTERSECTION 을 기억한다.
	__twoIntersection[__nextIntersectionIndex++] = __lastIntersection;
	++__numIntersection;
	if (__nextIntersectionIndex == 2)
		__nextIntersectionIndex = 0;

	// Picking 된 점을 이용하여 필요한 정보를 설정한다.
	D3DXVECTOR3	intersectionPoint;
	meshRef.getPoint(__lastIntersection, intersectionPoint);

	if (state.g__incompleteString) {	// 임의의 두 점 사이의 거리
		// Vertex Buffer 에 쓴다.
		D3DXVECTOR3*	intersectionVertexArray;
		meshRef.intersectionVertexBufferPtr()->Lock(0, 0, (void**) &intersectionVertexArray, 0);
		intersectionVertexArray[__nextIntersectionIndex ? 0 : 1] = intersectionPoint;
		meshRef.intersectionVertexBufferPtr()->Unlock();

		START_DEF&	startDef = meshRef.tapeMeasure().startDef();
		startDef.type = START_DEF_TYPE__TWO_POINT;
		TWO_POINT_PLANE_DEF&	_2pDef = startDef.two_point_plane_def;
		_2pDef.point0Def = __twoIntersection[0];
		_2pDef.point1Def = __twoIntersection[1];
		_2pDef.normal = D3DXVECTOR3(0, 0, 0);
	} else if (state.g__bString) {	// 임의의 위치에서의 길이 측정
#if 1	// 마지막 주문; 6. 3D 단면
		if (state.g__showLowerFace && !meshRef.measureDone) {
			OBJECT_TYPE	objectType = OBJECT_UNDEF;
			meshRef.setValue(TYPE__MODEL, MODEL__OBJECT, &objectType);
			meshRef.measureDone = true;
		}
#endif	// 마지막 주문; 6. 3D 단면
		D3DXVECTOR3*	intersectionVertexArray;
		meshRef.intersectionVertexBufferPtr()->Lock(0, 0, (void**) &intersectionVertexArray, 0);
		intersectionVertexArray[0] = intersectionPoint;
		meshRef.intersectionVertexBufferPtr()->Unlock();

		START_DEF&	startDef = meshRef.tapeMeasure().startDef();
		startDef.type = START_DEF_TYPE__GIRTH;
		PLANE_DEF&	planeDef = startDef.plane_def;
		planeDef.pointDef = __lastIntersection;
		planeDef.normal = state.g__n;

		state.g__stringMeasure = true;
	}

	return S_OK;
}


//--------------------------------------------------------------------------------------
// Render the help and statistics text. This function uses the ID3DXFont interface for
// efficient text rendering.
//--------------------------------------------------------------------------------------
void	RenderText()
{
#ifdef USE__pointer_to_PickVar
	if (!gPickVarPtr)
		return;
	PickVar&	gPickVar = *gPickVarPtr;
#endif	// USE__pointer_to_PickVar
#ifdef USE__PickVar
	kState&	state = gPickVar.state;
#ifdef USE__pointer_to_kMesh
	kMesh*&	gkMeshPtr = gPickVar.gkMeshPtr;
#else	// USE__pointer_to_kMesh/!USE__pointer_to_kMesh
	kMesh&		gMesh = gPickVar.gMesh;
#endif	// !USE__pointer_to_kMesh
	ID3DXFont*&	g_pFont = gPickVar.g_pFont;
	ID3DXSprite*&	g_pTextSprite = gPickVar.g_pTextSprite;
//	CModelViewerCamera2&	g_Camera = gPickVar.g_Camera;
#endif	// USE__PickVar
#ifdef USE__pointer_to_kMesh
	if (!gkMeshPtr)
		return;
	kMesh&	gMesh = *gkMeshPtr;
#endif	// USE__pointer_to_kMesh
//	cout << "RenderText" << endl;
    // The helper object simply helps keep track of text position, and color
    // and then it calls pFont->DrawText( m_pSprite, strMsg, -1, &rc, DT_NOCLIP, m_clr );
    // If NULL is passed in as the sprite object, then it will work however the
    // pFont->DrawText() will not be batched together.  Batching calls will improves performance.
    CDXUTTextHelper txtHelper( g_pFont, g_pTextSprite, 15 );

    // Output statistics
    txtHelper.Begin();
#if 1	// 모든 계측부위 표시
	if (state.displayAll)
		gMesh.DisplayAllMeasure(txtHelper);
#endif	// 모든 계측부위 표시

    txtHelper.SetInsertionPos( 5, 5 );
    txtHelper.SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 0.0f, 1.0f ) );
#ifndef __KEKO__
#if 0
#if 1	// too low fps (why?);
    	// DXUTIsVsyncEnabled returns false: is it a clue?
	txtHelper.DrawTextLine(DXUTGetFrameStats(true));
	txtHelper.DrawTextLine(DXUTGetDeviceStats());
#else
    txtHelper.DrawTextLine( DXUTGetFrameStats( DXUTIsVsyncEnabled() ) );
    txtHelper.DrawTextLine( DXUTGetDeviceStats() );
#endif
#endif
#endif	// !__KEKO__
if (state.g__showText) {

	if (state.__dwNumIntersections < 1) {
		txtHelper.DrawTextLine( L"Use mouse to Pick a polygon" );
	} else {
        WCHAR wstrHitStat[256];

        for(DWORD dwIndex = 0; dwIndex < state.__dwNumIntersections; dwIndex++)
        {
            StringCchPrintf(wstrHitStat, 256,
#if 1
                L"Face=%d, (%f, %f) tu=%3.02f, tv=%3.02f",
#else
                L"Face=%d, tu=%3.02f, tv=%3.02f",
#endif
                state.__intersectionArray[dwIndex].dwFace,
#if 1
                state.__intersectionArray[dwIndex].fBary1,
                state.__intersectionArray[dwIndex].fBary2,
#endif
                state.__intersectionArray[dwIndex].tu,
                state.__intersectionArray[dwIndex].tv );

            txtHelper.DrawTextLine( wstrHitStat );
        }
    }
}
#if 1
{
	WCHAR	keyState[128];
#if 1	// VK_RETURN
	if (state.message.keyPressed) {
		WCHAR	key0[] = L"varName: \"";
		const int	n0 = sizeof(key0) / sizeof(WCHAR) - 1;
		StringCchPrintf(keyState, 128, L"%s", key0);
		int	len = strlen(state.message.varName);
		MultiByteToWideChar(CP_ACP, 0, state.message.varName, -1, keyState + n0, 128);
		if (n0 + len + 1 < 128) {
			keyState[n0 + len] = L'"';
			keyState[n0 + len + 1] = L'\0';
		}
		txtHelper.DrawTextLine(keyState);
	}
#endif	// VK_RETURN
if (state.g__showText) {
	StringCchPrintf(keyState, 128, L"SHIFT: %s", state.g__Shift ? L"On" : L"Off");
	txtHelper.DrawTextLine(keyState);
	StringCchPrintf(keyState, 128, L"CTRL: %s", state.g__Control ? L"On" : L"Off");
	txtHelper.DrawTextLine(keyState);
	StringCchPrintf(keyState, 128, L"DRAG: %s: (%d, %d)...(%d, %d)", state.g__bDrag ? L"On" : L"Off", state.g__iDragBeginX, state.g__iDragBeginY, state.g__iMouseX, state.g__iMouseY);
	txtHelper.DrawTextLine(keyState);
	StringCchPrintf(keyState, 128, L"# of Selected Points: %u", state.g__NSelectedPoints);
	txtHelper.DrawTextLine(keyState);
	StringCchPrintf(keyState, 128, L"g__index = %u", state.g__index);
	txtHelper.DrawTextLine(keyState);
	StringCchPrintf(keyState, 128, L"g__plus = %s", state.g__plus ? L"On" : L"Off");
	txtHelper.DrawTextLine(keyState);
	StringCchPrintf(keyState, 128, L"g__minus = %s", state.g__minus ? L"On" : L"Off");
	txtHelper.DrawTextLine(keyState);
#if 0	// gIntersection
	StringCchPrintf(keyState, 128, L"gNumIntersection = %d", gNumIntersection);
	txtHelper.DrawTextLine(keyState);
	StringCchPrintf(keyState, 128, L"gIntersection[0]: dwFace = %d, U = %f, V = %f", gNumIntersection > 0 ? gIntersection[0].dwFace : -1, gNumIntersection > 0 ? gIntersection[0].fBary1 : -1.f, gNumIntersection > 0 ? gIntersection[0].fBary2 : -1.f);
	txtHelper.DrawTextLine(keyState);
	StringCchPrintf(keyState, 128, L"gIntersection[1]: dwFace = %d, U = %f, V = %f", gNumIntersection > 1 ? gIntersection[1].dwFace : -1, gNumIntersection > 1 ? gIntersection[1].fBary1 : -1.f, gNumIntersection > 1 ? gIntersection[1].fBary2 : -1.f);
	txtHelper.DrawTextLine(keyState);
	StringCchPrintf(keyState, 128, L"gIntersection[2]: dwFace = %d, U = %f, V = %f", gNumIntersection > 2 ? gIntersection[2].dwFace : -1, gNumIntersection > 2 ? gIntersection[2].fBary1 : -1.f, gNumIntersection > 2 ? gIntersection[2].fBary2 : -1.f);
	txtHelper.DrawTextLine(keyState);
#endif

	StringCchPrintf(keyState, 128, L"state.brush.On = %s", state.brush.flag.On ? L"On" : L"Off");
	txtHelper.DrawTextLine(keyState);

	StringCchPrintf(keyState, 128, L"g__L_BUTTON = %s", state.g__LBUTTONDOWN ? L"Pressed" : L"Released");
	txtHelper.DrawTextLine(keyState);
	StringCchPrintf(keyState, 128, L"g__R_BUTTON = %s", state.g__RBUTTONDOWN ? L"Pressed" : L"Released");
	txtHelper.DrawTextLine(keyState);

	StringCchPrintf(keyState, 128, L"g__nTessellation = %d", state.g__nTessellation);
	txtHelper.DrawTextLine(keyState);
}
#ifndef __KEKO__
if (state.g__showText) {
	if (state.g__bString || state.g__incompleteString) {
		StringCchPrintf(keyState, 128, L"Length = %f (Segment = %d); Error: %s, Closed: %s", state.g__stringLength, state.g__numSegments, state.g__stringError ? L"True" : L"False", state.g__stringClosed ? L"True" : L"False");
		txtHelper.DrawTextLine(keyState);
	} else {
		StringCchPrintf(keyState, 128, L"Volume Change = %f (Total Volume = %f)", gMesh.GetVolumeChange(), gMesh.GetTotalVolume());
		txtHelper.DrawTextLine(keyState);
	}
}
#endif	// !__KEKO__
}
#endif

	// 각도 값 표시
	const VIEW_T&	view = state.getViewRef();
	if (view.type != VIEW_PERSPECTIVE && view.orthographic && state.g__startPoint.x != -1 && state.g__endPoint.x != -1) {
		WCHAR	msg[128];
		StringCchPrintf(msg, 128, L"Angle = %f", 180.f / D3DX_PI * atan2f(fabsf((float) state.g__endPoint.y - (float) state.g__startPoint.y), fabsf((float) state.g__endPoint.x - (float) state.g__startPoint.x)));
		txtHelper.DrawTextLine(msg);
	}
	if (state.brush.flag.On) {
		WCHAR	msg[128];
		const kBrush&	brush = state.brush;
		kBrush::Flag	flag = brush.flag;

		StringCchPrintf(msg, 128, L"Intensity = %f ([SHIFT +] CTRL + ATL + F10)", brush.Intensity);
		txtHelper.DrawTextLine(msg);
		StringCchPrintf(msg, 128, L"");
		txtHelper.DrawTextLine(msg);
#if 0
		bool	On:1;		// 브러쉬 모드인지
		bool	Do:1;		// 빗질을 시작되었는 지(MsgProc와 관련)
		bool	Apply:1;	// 빗질을 적용할 지
		bool	Sculpture:1;	// Vertex 를 변형할 지
		bool	Sub:1;		// BRUSH_STANDARD 의 경우 들어가게 할 것인지
		bool	Smooth:1;	// 동시에 또는 단독으로 Vertex Smoothing 을 줄 것인지
		unsigned	SmoothLevel:2;	// Smoothing 단계; 0~3 까지 총 4단계
		bool	Show:1;		// 브러쉬 안에 들어오는 Vertex 을 표시할 지
#endif
		StringCchPrintf(msg, 128, L"Apply? %s (SHIFT + CTRL + F12)", flag.Apply ? L"True" : L"False");
		txtHelper.DrawTextLine(msg);
		StringCchPrintf(msg, 128, L"    Sculpture? %s (ALT + F12)", flag.Sculpture ? L"True" : L"False");
		txtHelper.DrawTextLine(msg);
		StringCchPrintf(msg, 128, L"    Sub? %s (SHIFT + F12)", flag.Sub ? L"True" : L"False");
		txtHelper.DrawTextLine(msg);
		StringCchPrintf(msg, 128, L"    Smooth? %s (CTRL + F12) [Level = %d ([SHIFT + ]CTRL + F10)]", flag.Smooth ? L"True" : L"False", flag.SmoothLevel);
		txtHelper.DrawTextLine(msg);
		StringCchPrintf(msg, 128, L"Show? %s (CTRL + ALT + F12)", flag.Show ? L"True" : L"False");
		txtHelper.DrawTextLine(msg);
	}
#if	1 || defined(__KEKO__)
#ifndef __KEKO__
if (state.g__showText) {
#endif	// !__KEKO__
{
	WCHAR	keyState[128];
	WCHAR	strQueue[1024];
	WCHAR*	p = strQueue;
	size_t	n = 0;
	int		iHead = gMesh.GetHead();
	int		iTail = gMesh.GetTail();
	int		iCurrent = gMesh.GetCurrent();
	unsigned	uSize = gMesh.GetQueueSize();
	for (int i = 0; i < LAST_POSITION__QUEUE_SIZE; ++i) {
		if (i == iTail) {
			n = wsprintf(p, L"T");
			p += n;
		}
		if (uSize == 0)
			n = wsprintf(p, L"0,");
		else {
			if (iTail < iHead) {
				if (i < iHead && i > iTail)
					n = wsprintf(p, L"0,");
				else
					n = wsprintf(p, L"1%s,", i == iCurrent ? L"i" : L"");
			} else {
				if (i < iHead || i > iTail)
					n = wsprintf(p, L"0,");
				else
					n = wsprintf(p, L"1%s,", i == iCurrent ? L"i" : L"");
			}
		}
		p += n;
	}

#if 1
	StringCchPrintf(keyState, 128, L"History [%s]; save: CTRL + F11, load: [SHIFT + ] F11", strQueue);
	txtHelper.DrawTextLine(keyState);
#if 0
	StringCchPrintf(keyState, 128, L"Smooth = %s: CTRL + F12", state.brush.flag.Smooth ? L"On" : L"Off");
	txtHelper.DrawTextLine(keyState);
	StringCchPrintf(keyState, 128, L"Smooth Level = %d: [SHIFT + ] CTRL + F10", state.brush.flag.SmoothLevel);
	txtHelper.DrawTextLine(keyState);
#endif
	StringCchPrintf(keyState, 128, L"Scale = %f: [SHIFT +] ALT + F10", state.scale_alpha);
	txtHelper.DrawTextLine(keyState);
	StringCchPrintf(keyState, 128, L"Apply: [SHIFT + ] ALT + F12");
	txtHelper.DrawTextLine(keyState);
#else
	StringCchPrintf(keyState, 128, L"history(CTRL,F11)(iHead = %d, iTail = %d, iCurrent(SHIFT(o:<-,x:->),F11) = %d: [%s]", iHead, iTail, iCurrent, strQueue);
	txtHelper.DrawTextLine(keyState);
	StringCchPrintf(keyState, 128,
		L"Smooth(CTRL, F12)? %s [Level = %d], Sub(SHIFT, F12)? %s"
		,
		state.brush.flag.Smooth ? L"On" : L"Off",
		1 + state.brush.flag.SmoothLevel,
		state.brush.flag.Sub ? L"On" : L"Off"
	);
	txtHelper.DrawTextLine(keyState);
	StringCchPrintf(keyState, 128, L"scale_alpha(ALT,SHIFT(o:-,x:+),F10) = %f", state.scale_alpha);
	txtHelper.DrawTextLine(keyState);
#endif
}
#ifndef __KEKO__
}
#endif	// !__KEKO__
#endif	// __KEKO__
#ifndef __KEKO__
#if 1	// AREA2_HINT
if (state.brush.flag.On) {
	WCHAR	msg[128];
	StringCchPrintf(msg, 128, L"");
	txtHelper.DrawTextLine(msg);
	txtHelper.DrawTextLine(msg);
	StringCchPrintf(msg, 128, L"Scale = %f: [SHIFT +] ALT + F10", state.scale_alpha);
	txtHelper.DrawTextLine(msg);
	StringCchPrintf(msg, 128, L"");
	txtHelper.DrawTextLine(msg);
	StringCchPrintf(msg, 128,
		L"ShowArea2? %s (CTRL + ALT + '0')"
		,
		state.area2_control.ShowArea2 == 0 ? L"False" : (
			state.area2_control.ShowArea2 == 1 ? L"True(inc)" : (
				state.area2_control.ShowArea2 == 2 ? L"True(all)" : L"UNDEF"
			)
		)
	);
	txtHelper.DrawTextLine(msg);
	StringCchPrintf(msg, 128, L"Repos? %s (CTRL + ALT + SHIFT + '0')", state.area2_control.Repos ? L"True" : L"False");
	txtHelper.DrawTextLine(msg);
	StringCchPrintf(msg, 128,
		L"Smooth? %s (SHIFT + ALT + '0')"
		,
		state.area2_control.Smooth == 0 ? L"False" : (
			state.area2_control.Smooth == 1 ? L"True(inc/all)" : (
				state.area2_control.Smooth == 2 ? L"True(move)" : L"UNDEF"
			)
		)
	);
	txtHelper.DrawTextLine(msg);
	StringCchPrintf(msg, 128, L"SmoothIncLevel = %d ('j', 'k')", state.area2_control.SmoothIncLevel);
	txtHelper.DrawTextLine(msg);
	StringCchPrintf(msg, 128, L"Area2Threshold = %d ([SHIFT + ]CTRL + '0')", state.Area2Threshold);
	txtHelper.DrawTextLine(msg);
}
#endif	// AREA2_HINT
#endif	// !__KEKO__


	txtHelper.End();
}


//--------------------------------------------------------------------------------------
// Before handling window messages, DXUT passes incoming windows
// messages to the application through this callback function. If the application sets
// *pbNoFurtherProcessing to TRUE, then DXUT will not process this message.
//--------------------------------------------------------------------------------------
LRESULT	CALLBACK MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing, void* pUserContext)
{
#ifdef USE__pointer_to_PickVar
	if (!gPickVarPtr)
		return 0;
	PickVar&	gPickVar = *gPickVarPtr;
#endif	// USE__pointer_to_PickVar
#ifdef USE__PickVar
	kState&	state = gPickVar.state;
#if 1	// 마지막 주문; 1. 회전축 고정
	CD3DArcBall2&	g_ArcBall = gPickVar.g_ArcBall;
#else	// 마지막 주문; 1. 회전축 고정/!마지막 주문; 1. 회전축 고정
	CD3DArcBall&	g_ArcBall = gPickVar.g_ArcBall;
#endif	// !마지막 주문; 1. 회전축 고정
#ifdef USE__pointer_to_kMesh
	kMesh*&	gkMeshPtr = gPickVar.gkMeshPtr;
	kMesh*&	gkMeshPtr0 = gPickVar.gkMeshPtr0;
#else	// USE__pointer_to_kMesh/!USE__pointer_to_kMesh
	kMesh&	gMesh = gPickVar.gMesh;
	kMesh&	gMesh0 = gPickVar.gMesh0;
#endif	// !USE__pointer_to_kMesh
	CModelViewerCamera2&	g_Camera = gPickVar.g_Camera;
	CDXUTDialogResourceManager&	g_DialogResourceManager = gPickVar.g_DialogResourceManager;
	CD3DSettingsDlg&	g_SettingsDlg = gPickVar.g_SettingsDlg;
	CDXUTDialog&	g_HUD = gPickVar.g_HUD;
	CDXUTDialog&	g_SampleUI = gPickVar.g_SampleUI;
#endif	// USE__PickVar
#ifdef USE__pointer_to_kMesh
	if (!gkMeshPtr)
		return 0;
	kMesh&	gMesh = *gkMeshPtr;
	kMesh&	gMesh0 = *gkMeshPtr0;
#endif	// USE__pointer_to_kMesh
//	cout << "MsgProc" << endl;
	kMesh*	kMeshPtr = &gMesh;
	if (state.g__pickStandard && &gMesh0 && gMesh0.meshLoader().meshPtr())
		kMeshPtr = &gMesh0;
	kMesh&	meshRef = *kMeshPtr;

	// Always allow dialog resource manager calls to handle global messages
	// so GUI state is updated correctly
	*pbNoFurtherProcessing = g_DialogResourceManager.MsgProc(hWnd, uMsg, wParam, lParam);
	if (*pbNoFurtherProcessing)
		return 0;

	if (g_SettingsDlg.IsActive()) {
		g_SettingsDlg.MsgProc(hWnd, uMsg, wParam, lParam);
		return 0;
	}

	if (uMsg >= WM_USER) {
		cout << "uMsg: " << uMsg << endl;
		cout << "wParam: " << wParam << endl;
		float*	p = (float*) &lParam;
		cout << "lParam: " << *p << endl;
		if (uMsg == WM_USER__ANGLE && wParam == 0) {
			state.g__startPoint.x = -1;
			state.g__startPoint.y = -1;
			state.g__endPoint.x = -1;
			state.g__endPoint.y = -1;
		}
		*pbNoFurtherProcessing = true;
		return 0;
	}
	// Give the dialogs a chance to handle the message first
	if (state.g__Control)
		*pbNoFurtherProcessing = false;
	else
		*pbNoFurtherProcessing = g_HUD.MsgProc(hWnd, uMsg, wParam, lParam);
	if (*pbNoFurtherProcessing)
		return 0;
	*pbNoFurtherProcessing = g_SampleUI.MsgProc(hWnd, uMsg, wParam, lParam);
	if (*pbNoFurtherProcessing)
		return 0;

	// Pass all remaining windows messages to camera so it can respond to user input
	if (state.brush.flag.On) {
		switch (uMsg) {
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
			if (uMsg == WM_LBUTTONDOWN)
				state.g__LBUTTONDOWN = true;
			else
				state.g__RBUTTONDOWN = true;
			state.g__bDrag = true;
			meshRef.brushInit();
#if 1	// Pick
{
#if 1	// Picking 에 사용되는 Ray 생성
D3DXVECTOR3	vPickRayDir;
D3DXVECTOR3	vPickRayOrig;

// Get the Pick ray from the mouse position
//	if (GetCapture())
{
POINT	ptCursor;
GetCursorPos(&ptCursor);
ScreenToClient(DXUTGetHWND(), &ptCursor);
D3DVIEWPORT9	viewport;
DXUTGetD3D9Device()->GetViewport(&viewport);
D3DXVECTOR3	o1 = D3DXVECTOR3((FLOAT) ptCursor.x, (FLOAT) ptCursor.y, 0.f);
D3DXVECTOR3	O1;	// Object 공간에서의 좌표
D3DXVec3Unproject(&O1, &o1, &viewport, g_Camera.GetProjMatrix(), g_Camera.GetViewMatrix(), g_Camera.GetWorldMatrix());
D3DXVECTOR3	o2 = D3DXVECTOR3((FLOAT) ptCursor.x, (FLOAT) ptCursor.y, 1.f);
D3DXVECTOR3	O2;	// Object 공간에서의 좌표
D3DXVec3Unproject(&O2, &o2, &viewport, g_Camera.GetProjMatrix(), g_Camera.GetViewMatrix(), g_Camera.GetWorldMatrix());
D3DXVECTOR3	v = O2 - O1;
D3DXVECTOR3	vNormalized;
D3DXVec3Normalize(&vNormalized, &v);

vPickRayOrig = O1;
vPickRayDir = vNormalized;
}
#endif
// Get the Picked triangle
int&		nearestIntersectionIndex = state.__nearestIntersectionIndex;
DWORD&		dwNumIntersections = state.__dwNumIntersections;
INTERSECTION*	intersectionArray = state.__intersectionArray;
meshRef.Pick(
meshRef.GetMeshPointer(),
// [IN]
true, false, vPickRayOrig, vPickRayDir,
// [OUT]
nearestIntersectionIndex, dwNumIntersections, intersectionArray
);
state.intersection_ptr = nearestIntersectionIndex == -1 ? NULL : &intersectionArray[nearestIntersectionIndex];
state.brush.flag.Do = nearestIntersectionIndex != -1;
if (state.brush.flag.Do) {
	state.brush.RayDirection = vPickRayDir;
}
}
#endif	// Pick
			SetCapture(hWnd);
			break;
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
			if (uMsg == WM_LBUTTONUP)
				state.g__LBUTTONDOWN = false;
			else
				state.g__RBUTTONDOWN = false;
			state.g__bDrag = false;
			meshRef.brushInit();
state.brush.flag.Do = false;
state.brush.RayDirection = D3DXVECTOR3(0, 0, 0);
			ReleaseCapture();
			break;
		}
	}
#if 1
	if (
		!(state.SetGridCenter || state.doMeasure || state.brush.flag.On || state.manipulator_mode) &&
		(state.g__Shift || state.g__Control)
	) {
		int	iMouseX = (int) LOWORD(lParam);
		int	iMouseY = (int) HIWORD(lParam);
		switch (uMsg) {
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
			if (uMsg == WM_LBUTTONDOWN) {
				state.g__LBUTTONDOWN = true;
			} else {
				state.g__RBUTTONDOWN = true;
			}
			state.g__bDrag = true;
			state.g__iDragBeginX = iMouseX;
			state.g__iDragBeginY = iMouseY;
			state.g__iMouseX = iMouseX;
			state.g__iMouseY = iMouseY;
			SetCapture(hWnd);
			break;
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
			if (uMsg == WM_LBUTTONUP)
				state.g__LBUTTONDOWN = false;
			else
				state.g__RBUTTONDOWN = false;
			state.g__bDrag = false;
			ReleaseCapture();
			break;
		case WM_MOUSEMOVE:
			state.g__iMouseX = iMouseX;
			state.g__iMouseY = iMouseY;
		}
		return 0;
	} else {
		if (
			// 특정 모드가 아닐 때(대부분의 경우)
			!(state.SetGridCenter || state.doMeasure || state.brush.flag.On || state.manipulator_mode) ||
			// 특정모드인 경우...
			// 1. 브러쉬 모드가 아니면서 동시에 SHIFT + CONTROL 키를 눌렀을 때
			(!state.brush.flag.On && state.g__Shift && state.g__Control) ||
			// 2. 브러쉬 모드인 경우...
			(
				// 
				state.brush.flag.On && (
#if 1
					!state.brush.flag.Do
#else
					(!state.brush_do && state.g__Shift && state.g__Control) ||
					(state.brush_do && state.intersection_ptr == NULL)
#endif
				)
		   	)
		) {
			const VIEW_T&	view = state.getViewRef();
			if (view.type != VIEW_PERSPECTIVE && view.orthographic) {
				switch (uMsg) {
				case WM_LBUTTONDOWN:
				case WM_RBUTTONDOWN:
					if (uMsg == WM_LBUTTONDOWN)
						state.g__LBUTTONDOWN = true;
					else
						state.g__RBUTTONDOWN = true;
					SetCapture(hWnd);
					break;
				case WM_LBUTTONUP:
				case WM_RBUTTONUP:
					if (uMsg == WM_LBUTTONUP)
						state.g__LBUTTONDOWN = false;
					else
						state.g__RBUTTONDOWN = false;
					ReleaseCapture();
					break;
				}
			}
#if 1	// PERSPECTIVE 뷰에서만 마우스의 이동으로 카메라를 회전시킨다.
			if (view.type == VIEW_PERSPECTIVE) {
				g_ArcBall.HandleMessages(hWnd, uMsg, wParam, lParam);
			}
#else
			g_ArcBall.HandleMessages(hWnd, uMsg, wParam, lParam);
#endif
		} else {
			if (state.SetGridCenter || state.doMeasure) {
				switch (uMsg) {
				case WM_LBUTTONDOWN:
				case WM_RBUTTONDOWN:
					SetCapture(hWnd);
					break;
				case WM_LBUTTONUP:
				case WM_RBUTTONUP:
					ReleaseCapture();
					break;
				}
			}
			if (state.manipulator_mode && meshRef.manipulatorPtr()) {
				POINT	ptCursor;
				ptCursor.x = (int) LOWORD(lParam);
				ptCursor.y = (int) HIWORD(lParam);
				switch (uMsg) {
				case WM_LBUTTONDOWN:
				case WM_RBUTTONDOWN:
					if (uMsg == WM_LBUTTONDOWN)
						state.g__LBUTTONDOWN = true;
					else
						state.g__RBUTTONDOWN = true;

//	meshRef.manipulatorPtr()->doPress();
{
	if (meshRef.manipulatorType() == POINT_MANIPULATOR) {
		kPointManipulator&	point_manipulator = meshRef.point_manipulator();
		point_manipulator.pick(*((CModelViewerCamera*) &g_Camera));
		if (point_manipulator.indexPicked() != -1)
			state.__nearestIntersectionIndex = -1;
	}
	if (meshRef.manipulatorType() == LINE_MANIPULATOR) {	// manipulator 준비
		kLineManipulator&	line_manipulator = meshRef.line_manipulator();
//		(void) fprintf(stdout, "line manipulator: isSet? %s, # of segments = %d\n", line_manipulator.isSet() ? "True" : "False", line_manipulator.numSegmentsInSegmentBuffer());
		line_manipulator.pick(*((CModelViewerCamera*) &g_Camera));
	}
	if (meshRef.manipulatorType() == NORMAL_MANIPULATOR) {	// manipulator 준비
		// ArcBall 의 원점을 화살표의 시작점으로 옮긴다.
		kNormalManipulator&	normal_manipulator = meshRef.normal_manipulator();

		D3DVIEWPORT9	viewport;
		DXUTGetD3D9Device()->GetViewport(&viewport);
		D3DXVECTOR3	O;
		D3DXVECTOR3	P0 = normal_manipulator.P0();
		D3DXVec3Project(
			&O,
			&P0,
			&viewport,
			g_Camera.GetProjMatrix(),
			g_Camera.GetViewMatrix(),
			g_Camera.GetWorldMatrix()
		);
		normal_manipulator.Reset();
/*
 * CD3DArcBall::OnBegin(int nX, int nY) 을 보면...
 *
	void	CD3DArcBall::OnBegin(int nX, int nY)
	{
		// Only enter the drag state if the click falls
		// inside the click rectangle.
		if (
			nX >= m_Offset.x && nX < m_Offset.x + m_nWidth &&
			nY >= m_Offset.y && nY < m_Offset.y + m_nHeight
		) {
			m_bDrag = true;
			m_qDown = m_qNow;
			m_vDownPt = ScreenToVector((float) nX, (float) nY);
		}
	}
 * 
 * (m_Offset.x, m_Offset.y)는 Arcball 이 정의된 사각형의 기준점이 원래 기준점에 비해서 얼마나 벗어났는 지를 말해준다.
 * 여기서 기준점은 (nX, nY)의 원점(0, 0)을 말하는 것으로, 마우스의 이벤트핸들러(CD3DArcBall::HandleMessages)를 보면,
 * (왼쪽, 위)가 된다.
 *
 * kNormalManipulator::set 을 보면 Arcball 은 한 변이 길이가 2 * radius 인 정사각형안에서 정의되었으므로,
 * 정사각형의 중심에서 왼쪽, 위로 radius 만큼 벗어난 곳에 위치한다.
 *
 * 주) CD3DArcBall::ScreenToVector 를 보면 일반적인 Projection Space 와 달리,
 * (왼쪽, 아래)가 (-1, -1), (오른쪽, 위)가 (1, 1)이 아닌,
 * (왼쪽, 아래)가 (1, 1), (오른쪽, 위)가 (-1, -1)이다.
 * 즉 (x, y) -> (-x, -y) 이다.
 * 그렇다고 원래 Projection Space 처럼 (-x, -y)하면 회전방향이 반대가 된다.
 * 다시 CD3DArcBall::QuatFromBallPoints 를 보면 그 이유가 있다.
 * 회전을 설정할 때 Quaternion 을 사용하는 데, D3DXQUATERNION 에 대한 DirectX 설명서를 보면
 * D3DXQUATERNION 을 만드는 회전이, CD3DArcBall::QuatFromBallPoints 에서 만든 회전방향과 반대이다.
 * 그렇기 때문에 일부러 CD3DArcBall::ScreenToVector 에서 (-x, -y)로 했다는 얘기이다.
 * 알기 어렵게 되어있다. 또한 CD3DArcBall::QuatFromBallPoints 에서 설정한 Quaternion 도 제대로
 * (축, 회전각)을 반영하고 있지 않다. 단지 회전한다는 것과 방향만 맞춘 것이다.
 *
 * CD3DArcBall::ScreenToVector 에서 제대로 Projection Space 에 맞게 하고,
 * CD3DArcBall::QuatFromBallPoints 에서 회전방향을 고려하여 축을 반대로 한 후,
 * D3DXQuaternionRotationAxis 를 사용하면 (DirectX 매뉴얼의 D3DXQUATERNION 에 있는 식),
 * 의도한대로 회전한다; DO TO LIST: CD3DArcBall 를 다시 써야 하나...
 */
		normal_manipulator.SetOffset((INT) (O.x - normal_manipulator.radiusInScreenSpace()), (INT) (O.y  - normal_manipulator.radiusInScreenSpace()));
		normal_manipulator.SetCameraRotationMatrix(*g_ArcBall.GetRotationMatrix());
	}
}
					SetCapture(hWnd);
					break;
        			case WM_MOUSEMOVE:
#pragma warning(disable: 4127)
	if (
		(meshRef.manipulatorType() == POINT_MANIPULATOR && meshRef.point_manipulator().indexPicked() != -1) ||
		(meshRef.manipulatorType() == LINE_MANIPULATOR && meshRef.line_manipulator().indexPicked() != -1) ||
		0
	) {
		D3DXVECTOR3	vPickRayDir;
		D3DXVECTOR3	vPickRayOrig;

		POINT	ptCursor;
		GetCursorPos(&ptCursor);
		ScreenToClient(DXUTGetHWND(), &ptCursor);
	{
		D3DVIEWPORT9	viewport;
		DXUTGetD3D9Device()->GetViewport(&viewport);
		D3DXVECTOR3	o1 = D3DXVECTOR3((FLOAT) ptCursor.x, (FLOAT) ptCursor.y, 0.f);
		D3DXVECTOR3	O1;	// Object 공간에서의 좌표
		D3DXVec3Unproject(&O1, &o1, &viewport, g_Camera.GetProjMatrix(), g_Camera.GetViewMatrix(), g_Camera.GetWorldMatrix());
		D3DXVECTOR3	o2 = D3DXVECTOR3((FLOAT) ptCursor.x, (FLOAT) ptCursor.y, 1.f);
		D3DXVECTOR3	O2;	// Object 공간에서의 좌표
		D3DXVec3Unproject(&O2, &o2, &viewport, g_Camera.GetProjMatrix(), g_Camera.GetViewMatrix(), g_Camera.GetWorldMatrix());
		D3DXVECTOR3	v = O2 - O1;
		D3DXVECTOR3	vNormalized;
		D3DXVec3Normalize(&vNormalized, &v);

		vPickRayOrig = O1;
		vPickRayDir = vNormalized;
	}

		int	nearestIntersectionIndex;
		DWORD&		dwNumIntersections = state.__dwNumIntersections;
		INTERSECTION*	intersectionArray = state.__intersectionArray;
		meshRef.Pick(
			meshRef.GetMeshPointer(),
			// [IN]
			true, false, vPickRayOrig, vPickRayDir,
			// [OUT]
			nearestIntersectionIndex, dwNumIntersections, intersectionArray
		);
		if (nearestIntersectionIndex != -1) {	// picking
			if (meshRef.manipulatorType() == POINT_MANIPULATOR)
				meshRef.point_manipulator().pointDef() = intersectionArray[nearestIntersectionIndex];
			else if (meshRef.manipulatorType() == LINE_MANIPULATOR) {
				POINT_DEF	pointDef;
				pointDef = intersectionArray[nearestIntersectionIndex];
				if (meshRef.line_manipulator().setPointDef(pointDef) == 0) {
#if 0	// 둘레측정으로 구현
					START_DEF_PTR	startDefPtr;
					PLANE_DEF&	planeDef = meshRef.line_manipulator().planeDef();
					startDefPtr.type = START_TYPE__GIRTH;
					startDefPtr.plane_def_ptr = &planeDef;
					normal_manipulator_done_callback_manual_measure(&meshRef, 0, &startDefPtr, planeDef.normal);
#endif
				}
			}
		}
	}
#pragma warning(default: 4127)
					break;
				case WM_LBUTTONUP:
				case WM_RBUTTONUP:
					if (uMsg == WM_LBUTTONUP)
						state.g__LBUTTONDOWN = false;
					else
						state.g__RBUTTONDOWN = false;
{	// manipulator 마무리
	meshRef.manipulatorPtr()->doRelease();
}
					ReleaseCapture();
					break;
				}
{	// ArcBall 회전이나...
	meshRef.manipulatorPtr()->HandleMessages(hWnd, uMsg, wParam, lParam);
}
			}
#if 0
			if (state.g__brush) {
				switch (uMsg) {
				case WM_LBUTTONDOWN:
				case WM_RBUTTONDOWN:
					if (uMsg == WM_LBUTTONDOWN)
						state.g__LBUTTONDOWN = true;
					else
						state.g__RBUTTONDOWN = true;
					state.g__bDrag = true;
					meshRef.brushInit();
#if 1	// Pick
{
#if 1	// Picking 에 사용되는 Ray 생성
	D3DXVECTOR3	vPickRayDir;
	D3DXVECTOR3	vPickRayOrig;

	// Get the Pick ray from the mouse position
//	if (GetCapture())
{
	POINT	ptCursor;
	GetCursorPos(&ptCursor);
	ScreenToClient(DXUTGetHWND(), &ptCursor);
	D3DVIEWPORT9	viewport;
	DXUTGetD3D9Device()->GetViewport(&viewport);
	D3DXVECTOR3	o1 = D3DXVECTOR3((FLOAT) ptCursor.x, (FLOAT) ptCursor.y, 0.f);
	D3DXVECTOR3	O1;	// Object 공간에서의 좌표
	D3DXVec3Unproject(&O1, &o1, &viewport, g_Camera.GetProjMatrix(), g_Camera.GetViewMatrix(), g_Camera.GetWorldMatrix());
	D3DXVECTOR3	o2 = D3DXVECTOR3((FLOAT) ptCursor.x, (FLOAT) ptCursor.y, 1.f);
	D3DXVECTOR3	O2;	// Object 공간에서의 좌표
	D3DXVec3Unproject(&O2, &o2, &viewport, g_Camera.GetProjMatrix(), g_Camera.GetViewMatrix(), g_Camera.GetWorldMatrix());
	D3DXVECTOR3	v = O2 - O1;
	D3DXVECTOR3	vNormalized;
	D3DXVec3Normalize(&vNormalized, &v);

	vPickRayOrig = O1;
	vPickRayDir = v;
}
#endif
	// Get the Picked triangle
	int&		nearestIntersectionIndex = state.__nearestIntersectionIndex;
	DWORD&		dwNumIntersections = state.__dwNumIntersections;
	INTERSECTION*	intersectionArray = state.__intersectionArray;
	meshRef.Pick(
		meshRef.GetMeshPointer(),
		// [IN]
		true, false, vPickRayOrig, vPickRayDir,
		// [OUT]
		nearestIntersectionIndex, dwNumIntersections, intersectionArray
	);
	state.intersection_ptr = nearestIntersectionIndex == -1 ? NULL : &intersectionArray[nearestIntersectionIndex];
	state.brush.Do = nearestIntersectionIndex != -1;
}
#endif	// Pick
					SetCapture(hWnd);
					break;
				case WM_LBUTTONUP:
				case WM_RBUTTONUP:
					if (uMsg == WM_LBUTTONUP)
						state.g__LBUTTONDOWN = false;
					else
						state.g__RBUTTONDOWN = false;
					state.g__bDrag = false;
					meshRef.brushInit();
state.brush.Do = false;
					ReleaseCapture();
					break;
				}
			}
#endif
		}
	}
#endif

	return 0;
}


void
toggleBrushMode(
	void
)
{
#ifdef USE__pointer_to_PickVar
	if (!gPickVarPtr)
		return;
	PickVar&	gPickVar = *gPickVarPtr;
#endif	// USE__pointer_to_PickVar
#ifdef USE__PickVar
	kState&	state = gPickVar.state;
#ifdef USE__pointer_to_kMesh
	kMesh*&	gkMeshPtr = gPickVar.gkMeshPtr;
#else	// USE__pointer_to_kMesh/!USE__pointer_to_kMesh
	kMesh&		gMesh = gPickVar.gMesh;
#endif	// !USE__pointer_to_kMesh
#endif	// USE__PickVar
#ifdef USE__pointer_to_kMesh
	if (!gkMeshPtr)
		return;
	kMesh&	gMesh = *gkMeshPtr;
#endif	// USE__pointer_to_kMesh

	state.brush.flag.On = !state.brush.flag.On;
	if (state.brush.flag.On)
		gMesh.SaveColor();
	else
		gMesh.RestoreColor();
	if (state.g__bString || state.g__incompleteString) {
		state.g__bString = false;
		state.g__incompleteString = false;
	}

	if (state.brush.flag.On) {
		// Reset colors
		LPDIRECT3DVERTEXBUFFER9	pVB = gMesh.GetVertexBufferPointer();
		kMesh::D3DXVERTEX*	pV;
		pVB->Lock(0, 0, (void**) &pV, 0);
#if 0
		const CPointRepr*	pPointRepr = gMesh.GetPointReprPointer();
#endif
		for (DWORD i = 0; i < gMesh.GetNumPointReprs(); ++i) {
#if 0
			DWORD	prID = pPointRepr[i].id;
			if (prID == (DWORD) -1)
				continue;
#endif

#if 1
			DWORD	index = i;
#else
			DWORD	index = (gMesh.GetPointReprIDMappedPointer())[prID];
#endif
			pV[index].c = D3DXVECTOR3(0.f, 0.f, 0.f);
		}
		pVB->Unlock();
	}
}

static void	__1KeyPressed()
{
////////////////////////////////////////////////////////////////////////
#ifdef USE__pointer_to_PickVar
	if (!gPickVarPtr)
		return;
	PickVar&	gPickVar = *gPickVarPtr;
#endif	// USE__pointer_to_PickVar
#ifdef USE__PickVar
	kState&	state = gPickVar.state;
#if 1	// 마지막 주문; 1. 회전축 고정
	CD3DArcBall2&	g_ArcBall = gPickVar.g_ArcBall;
#endif	// 마지막 주문; 1. 회전축 고정
#ifdef USE__pointer_to_kMesh
	kMesh*&	gkMeshPtr = gPickVar.gkMeshPtr;
	kMesh*&	gkMeshPtr0 = gPickVar.gkMeshPtr0;
#else	// USE__pointer_to_kMesh/!USE__pointer_to_kMesh
	kMesh&	gMesh = gPickVar.gMesh;
	kMesh&	gMesh0 = gPickVar.gMesh0;
#endif	// !USE__pointer_to_kMesh
	CModelViewerCamera2&	g_Camera = gPickVar.g_Camera;
//	bool&	g_bShowHelp = gPickVar.g_bShowHelp;
#endif	// USE__PickVar
#ifdef USE__pointer_to_kMesh
	kMesh&	gMesh = *gkMeshPtr;
#endif	// USE__pointer_to_kMesh
#ifdef USE__pointer_to_kMesh
	kMesh&	gMesh0 = *gkMeshPtr0;
#endif	// USE__pointer_to_kMesh
//	cout << "KeyboardProc" << endl;
	kMesh*	kMeshPtr = &gMesh;
	if (state.g__pickStandard && &gMesh0 && gMesh0.meshLoader().meshPtr())
		kMeshPtr = &gMesh0;
//	kMesh&	meshRef = *kMeshPtr;
////////////////////////////////////////////////////////////////////////

	if (!state.g__Control) {	// CTRL 키가 눌리지 않은 경우에는 Orthographic View
		if (state.g__Alt)
			g_Camera.saveProjMatrix();
		else
			g_Camera.loadProjMatrix();
		VIEW_T	view;
		view.type = state.g__Shift ? VIEW_BOTTOM : VIEW_TOP;
		view.orthographic = state.g__Alt;
		state.setValue(TYPE__VIEW, VIEW__TYPE, &view);

		return;
	}
	
	// CTRL 키가 눌린 경우...
	//	CTRL + '1', SHIFT + CTRL + '1', SHIFT + CTRL + ALT + '1'
	if (state.g__Shift && !state.g__Alt) {		// SHIFT + CTRL + '1'
		state.g__showBoundingSphere = !state.g__showBoundingSphere;
	} else if (state.g__Shift && state.g__Alt) {	// SHIFT + CTRL + ALT + '1'
		// 미할당
#if 1	// 마지막 주문; 1. 회전축 고정
		state.fixed_axis = !state.fixed_axis;
		if (!state.fixed_axis) {
			CD3DArcBall2::FIXED_AXIS::TYPE	fixed_axis_type = CD3DArcBall2::FIXED_AXIS::FREE;
			// 먼저 초기상태로 돌아간 후...
			bool	reset = true;
			state.setValue(TYPE__VIEW, VIEW__HOME, &reset);
			state.g__n = D3DXVECTOR3(0, 1, 0);
			g_ArcBall.SetFixedAxisType(fixed_axis_type);
		}
#endif	// 마지막 주문; 1. 회전축 고정
	} else {					// CTRL + '1'
		state.g__showBoundingBox = !state.g__showBoundingBox;
	}
}
static void	__2KeyPressed()
{
////////////////////////////////////////////////////////////////////////
#ifdef USE__pointer_to_PickVar
	if (!gPickVarPtr)
		return;
	PickVar&	gPickVar = *gPickVarPtr;
#endif	// USE__pointer_to_PickVar
#ifdef USE__PickVar
	kState&	state = gPickVar.state;
#ifdef USE__pointer_to_kMesh
	kMesh*&	gkMeshPtr = gPickVar.gkMeshPtr;
	kMesh*&	gkMeshPtr0 = gPickVar.gkMeshPtr0;
#else	// USE__pointer_to_kMesh/!USE__pointer_to_kMesh
	kMesh&	gMesh = gPickVar.gMesh;
	kMesh&	gMesh0 = gPickVar.gMesh0;
#endif	// !USE__pointer_to_kMesh
	CModelViewerCamera2&	g_Camera = gPickVar.g_Camera;
//	bool&	g_bShowHelp = gPickVar.g_bShowHelp;
#endif	// USE__PickVar

#ifdef USE__pointer_to_kMesh
	kMesh&	gMesh = *gkMeshPtr;
	kMesh&	gMesh0 = *gkMeshPtr0;
#endif	// USE__pointer_to_kMesh
//	cout << "KeyboardProc" << endl;
	kMesh*	kMeshPtr = &gMesh;
	if (state.g__pickStandard && &gMesh0 && gMesh0.meshLoader().meshPtr())
		kMeshPtr = &gMesh0;
//	kMesh&	meshRef = *kMeshPtr;
////////////////////////////////////////////////////////////////////////

	if (!state.g__Control) {	// CTRL 키가 눌리지 않은 경우에는 Orthographic View
		if (state.g__Alt)
			g_Camera.saveProjMatrix();
		else
			g_Camera.loadProjMatrix();

		VIEW_T	view;
		view.type = state.g__Shift ? VIEW_BACK : VIEW_FRONT;
		view.orthographic = state.g__Alt;
		state.setValue(TYPE__VIEW, VIEW__TYPE, &view);

		return;
	}

	// CTRL 키가 눌린 경우...
	//	CTRL + '2', SHIFT + CTRL + '2', SHIFT + CTRL + ALT + '2'
	if (state.g__Shift && !state.g__Alt) {		// SHIFT + CTRL + '2'
		// 모든 정의된 부위를 표시한다. (toggle)
		// 화면에 나타나게 하려면 state.g__showDefinedMeasure 를 설정해야 한다; CTRL + ALT + '8'

		state.displayAll = !state.displayAll;
		if (state.displayAll) {
			gMesh.tapeMeasure().getValSavedRef() = gMesh.tapeMeasure().getValRef();
//			int	val = TAPE_MEASURE_BREAST | TAPE_MEASURE_HIP;
			gMesh.SetAllMeasure(NULL);//&val);
		} else {
			gMesh.tapeMeasure().getValRef() = gMesh.tapeMeasure().getValSavedRef();
		}
		state.g__showDefinedMeasure = state.displayAll;
	} else if (state.g__Shift && state.g__Alt) {	// SHIFT + CTRL + ALT + '2'
		// 미할당
	} else {					// CTRL + '2'
		// 모든 정의된 부위를 계측해서 값을 설정한다.
		int	iBit = 1;
		for (int i = 0; i < NUM_TAPE_MEASURE_TYPE; ++i, iBit <<= 1) {
			TAPE_MEASURE_TYPE_VALUE	typeValue;
			TAPE_MEASURE_TYPE_T&	type = typeValue.type;
			float&	length = typeValue.value;

			// 해당 부위의 길이를 얻는다.
			type = (TAPE_MEASURE_TYPE_T) iBit;
			gMesh.getValue(TYPE__TAPE_MEASURE, TAPE_MEASURE__VALUE, &typeValue);
			(void) fprintf(stdout, "TAPE_MEASURE_TYPE_T = %d: length = %f\n", type, length);
		}
	}
}
static void	__3KeyPressed()
{
////////////////////////////////////////////////////////////////////////
#ifdef USE__pointer_to_PickVar
	if (!gPickVarPtr)
		return;
	PickVar&	gPickVar = *gPickVarPtr;
#endif	// USE__pointer_to_PickVar
#ifdef USE__PickVar
	kState&	state = gPickVar.state;
#ifdef USE__pointer_to_kMesh
	kMesh*&	gkMeshPtr = gPickVar.gkMeshPtr;
	kMesh*&	gkMeshPtr0 = gPickVar.gkMeshPtr0;
#else	// USE__pointer_to_kMesh/!USE__pointer_to_kMesh
	kMesh&	gMesh = gPickVar.gMesh;
	kMesh&	gMesh0 = gPickVar.gMesh0;
#endif	// !USE__pointer_to_kMesh
	CModelViewerCamera2&	g_Camera = gPickVar.g_Camera;
//	bool&	g_bShowHelp = gPickVar.g_bShowHelp;
#endif	// USE__PickVar
#ifdef USE__pointer_to_kMesh
	kMesh&	gMesh = *gkMeshPtr;
#endif	// USE__pointer_to_kMesh
#ifdef USE__pointer_to_kMesh
	kMesh&	gMesh0 = *gkMeshPtr0;
#endif	// USE__pointer_to_kMesh
//	cout << "KeyboardProc" << endl;
	kMesh*	kMeshPtr = &gMesh;
	if (state.g__pickStandard && &gMesh0 && gMesh0.meshLoader().meshPtr())
		kMeshPtr = &gMesh0;
//	kMesh&	meshRef = *kMeshPtr;
////////////////////////////////////////////////////////////////////////

	if (!state.g__Control) {	// CTRL 키가 눌리지 않은 경우에는 Orthographic View
		if (state.g__Alt)
			g_Camera.saveProjMatrix();
		else
			g_Camera.loadProjMatrix();
		VIEW_T	view;
		view.type = state.g__Shift ? VIEW_LEFT : VIEW_RIGHT;
		view.orthographic = state.g__Alt;
		state.setValue(TYPE__VIEW, VIEW__TYPE, &view);

		return;
	}
	
	// CTRL 키가 눌린 경우...
	// CTRL + '3', SHIFT + CTRL + '3', SHIFT + CTRL + ALT + '3'
	if (state.g__Shift && !state.g__Alt) {		// SHIFT + CTRL + '3'
		// 미할당
	} else if (state.g__Shift && state.g__Alt) {	// SHIFT + CTRL + ALT + '3'
		// 미할당
	} else {					// CTRL + '3'
		// 미할당
	}
}
static void	__JKeyPressed()
{
////////////////////////////////////////////////////////////////////////
#ifdef USE__pointer_to_PickVar
	if (!gPickVarPtr)
		return;
	PickVar&	gPickVar = *gPickVarPtr;
#endif	// USE__pointer_to_PickVar
#ifdef USE__PickVar
	kState&	state = gPickVar.state;
#if 1	// 마지막 주문; 1. 회전축 고정
	CD3DArcBall2&	g_ArcBall = gPickVar.g_ArcBall;
#endif	// 마지막 주문; 1. 회전축 고정
#ifdef USE__pointer_to_kMesh
	kMesh*&	gkMeshPtr = gPickVar.gkMeshPtr;
	kMesh*&	gkMeshPtr0 = gPickVar.gkMeshPtr0;
#else	// USE__pointer_to_kMesh/!USE__pointer_to_kMesh
	kMesh&	gMesh = gPickVar.gMesh;
	kMesh&	gMesh0 = gPickVar.gMesh0;
#endif	// !USE__pointer_to_kMesh
//	CModelViewerCamera2&	g_Camera = gPickVar.g_Camera;
//	bool&	g_bShowHelp = gPickVar.g_bShowHelp;
#endif	// USE__PickVar
#ifdef USE__pointer_to_kMesh
	kMesh&	gMesh = *gkMeshPtr;
#endif	// USE__pointer_to_kMesh
#ifdef USE__pointer_to_kMesh
	kMesh&	gMesh0 = *gkMeshPtr0;
#endif	// USE__pointer_to_kMesh
//	cout << "KeyboardProc" << endl;
	kMesh*	kMeshPtr = &gMesh;
	if (state.g__pickStandard && &gMesh0 && gMesh0.meshLoader().meshPtr())
		kMeshPtr = &gMesh0;
	kMesh&	meshRef = *kMeshPtr;
////////////////////////////////////////////////////////////////////////

#if 1	// ColorBand
if (state.ColorBandControl) {
	if (!state.g__Shift && !state.g__Control && state.g__Alt) {	// ALT + 'J': type 감소
		COLOR_BAND_TYPE_T	type = COLOR_BAND_UNDEF;
		state.getValue(TYPE__COLOR_BAND, COLOR_BAND__TYPE, &type);
		int	iType = (int) type;
		--iType;
		if (iType < (int) COLOR_BAND_UNDEF)	// COLOR_BAND_UNDEF: 가장 작은 COLOR_BAND_TYPE_T
			iType = (int) COLOR_BAND_UNDEF;
		type = (COLOR_BAND_TYPE_T) iType;
		state.setValue(TYPE__COLOR_BAND, COLOR_BAND__TYPE, &type);

		if (type != COLOR_BAND_UNDEF) {	// 표시선 리셋
			kColorBand::IndexIndicator	indexIndicator;
			indexIndicator.index = 0;	// 첫 번째만 가지고 테스트
			indexIndicator.f0 = .7f;
			indexIndicator.width = 2.f;
			indexIndicator.scale = 0.f;
			indexIndicator.color = D3DCOLOR_RGBA(0, 255, 0, 255);
			state.setValue(TYPE__COLOR_BAND, COLOR_BAND__INDICATOR, &indexIndicator);
		}
	}
	if (!state.g__Shift && !state.g__Control && !state.g__Alt) {	// 'J': x_offset 감소
		COLOR_BAND_TYPE_T	type = COLOR_BAND_UNDEF;
		state.getValue(TYPE__COLOR_BAND, COLOR_BAND__TYPE, &type);
		if (type != COLOR_BAND_UNDEF) {
			kColorBand::Dimension	dimension;
			state.getValue(TYPE__COLOR_BAND, COLOR_BAND__DIMENSION, &dimension);
			dimension.x_offset -= .01f;
			if (dimension.x_offset < 0.f)
				dimension.x_offset = 0.f;
			state.setValue(TYPE__COLOR_BAND, COLOR_BAND__DIMENSION, &dimension);
		}
	}
	if (!state.g__Shift && state.g__Control && !state.g__Alt) {	// CTRL + 'J': y_offset 감소
		COLOR_BAND_TYPE_T	type = COLOR_BAND_UNDEF;
		state.getValue(TYPE__COLOR_BAND, COLOR_BAND__TYPE, &type);
		if (type != COLOR_BAND_UNDEF) {
			kColorBand::Dimension	dimension;
			state.getValue(TYPE__COLOR_BAND, COLOR_BAND__DIMENSION, &dimension);
			dimension.y_offset -= .01f;
			if (dimension.y_offset < 0.f)
				dimension.y_offset = 0.f;
			state.setValue(TYPE__COLOR_BAND, COLOR_BAND__DIMENSION, &dimension);
		}
	}
	if (state.g__Shift && !state.g__Control && !state.g__Alt) {	// SHIFT + 'J': width 감소
		COLOR_BAND_TYPE_T	type = COLOR_BAND_UNDEF;
		state.getValue(TYPE__COLOR_BAND, COLOR_BAND__TYPE, &type);
		if (type != COLOR_BAND_UNDEF) {
			kColorBand::Dimension	dimension;
			state.getValue(TYPE__COLOR_BAND, COLOR_BAND__DIMENSION, &dimension);
			dimension.width -= .01f;
			if (dimension.width < 0.f)
				dimension.width = 0.f;
			state.setValue(TYPE__COLOR_BAND, COLOR_BAND__DIMENSION, &dimension);
		}
	}
	if (state.g__Shift && state.g__Control && !state.g__Alt) {	// SHIFT + CTRL + 'J': height 감소
		COLOR_BAND_TYPE_T	type = COLOR_BAND_UNDEF;
		state.getValue(TYPE__COLOR_BAND, COLOR_BAND__TYPE, &type);
		if (type != COLOR_BAND_UNDEF) {
			kColorBand::Dimension	dimension;
			state.getValue(TYPE__COLOR_BAND, COLOR_BAND__DIMENSION, &dimension);
			dimension.height -= .01f;
			if (dimension.height < 0.f)
				dimension.height = 0.f;
			state.setValue(TYPE__COLOR_BAND, COLOR_BAND__DIMENSION, &dimension);
		}
	}
	if (state.g__Shift && !state.g__Control && state.g__Alt) {	// SHIFT + ALT + 'J': indicatorF0 감소
		COLOR_BAND_TYPE_T	type = COLOR_BAND_UNDEF;
		state.getValue(TYPE__COLOR_BAND, COLOR_BAND__TYPE, &type);
		if (type != COLOR_BAND_UNDEF) {
			kColorBand::IndexIndicator	indexIndicator;
			indexIndicator.index = 0;	// 첫 번째만 가지고 테스트
			state.getValue(TYPE__COLOR_BAND, COLOR_BAND__INDICATOR, &indexIndicator);
			indexIndicator.f0 -= .01f;
			if (indexIndicator.f0 < -0.01f)	// -0.01 < 0: 표시선 표시 안 함
				indexIndicator.f0 = -0.01f;
			state.setValue(TYPE__COLOR_BAND, COLOR_BAND__INDICATOR, &indexIndicator);
		}
	}
	if (!state.g__Shift && state.g__Control && state.g__Alt) {	// CTRL + ALT + 'J': indicatorLineScale 감소
		COLOR_BAND_TYPE_T	type = COLOR_BAND_UNDEF;
		state.getValue(TYPE__COLOR_BAND, COLOR_BAND__TYPE, &type);
		if (type != COLOR_BAND_UNDEF) {
			kColorBand::IndexIndicator	indexIndicator;
			indexIndicator.index = 0;	// 첫 번째만 가지고 테스트
			state.getValue(TYPE__COLOR_BAND, COLOR_BAND__INDICATOR, &indexIndicator);
			indexIndicator.scale -= .01f;
			state.setValue(TYPE__COLOR_BAND, COLOR_BAND__INDICATOR, &indexIndicator);
		}
	}
	if (state.g__Shift && state.g__Control && state.g__Alt) {	// SHIFT + CTRL + ALT + 'J': indicatorLineWidth 변화
		COLOR_BAND_TYPE_T	type = COLOR_BAND_UNDEF;
		state.getValue(TYPE__COLOR_BAND, COLOR_BAND__TYPE, &type);
		if (type != COLOR_BAND_UNDEF) {
			static int	giWidth = 2;	// 1, 2, 3
			--giWidth;
			if (giWidth < 1)
				giWidth = 3;

			kColorBand::IndexIndicator	indexIndicator;
			indexIndicator.index = 0;	// 첫 번째만 가지고 테스트
			state.getValue(TYPE__COLOR_BAND, COLOR_BAND__INDICATOR, &indexIndicator);
			indexIndicator.width = giWidth == 1 ? 1.f : (giWidth == 2 ? 2.f : 3.f);
			state.setValue(TYPE__COLOR_BAND, COLOR_BAND__INDICATOR, &indexIndicator);
		}
	}
}
#endif	// ColorBand
#if 0	// Convex
			if (!state.g__show_all_convex_points && meshRef.tapeMeasure().convexPointArray().size()) {
				--state.g__next_convex_index;
				if (state.g__next_convex_index < 0)
					state.g__next_convex_index = meshRef.tapeMeasure().convexPointArray().size() - 1;
			}
#endif
#if 0	// 모델
		{
			OBJECT_TYPE	objectType;
			meshRef.getValue(TYPE__MODEL, MODEL__OBJECT, &objectType);
			if (objectType != OBJECT_UNDEF) {	// 모델의 어느 오브젝트라도 표현한다면
				// 1, 2, 4, 8 의 모든 조합이 돌아가면서 보여준다.
				int	iObjectType = (int) objectType;
				if (iObjectType == 15)
					iObjectType = 1;
				else
					++iObjectType;
				objectType = (OBJECT_TYPE) iObjectType;
				meshRef.setValue(TYPE__MODEL, MODEL__OBJECT, &objectType);
			}
		}
#endif
#if 1	// 길이측정
			if (state.g__showDefinedMeasure) {	// 측정 부위를 바꿔가면서...
				static int	nBit = 0;
				--nBit;
				if (nBit < 0)
					nBit = NUM_TAPE_MEASURE_TYPE - 1;
				TAPE_MEASURE_TYPE_T	type = (TAPE_MEASURE_TYPE_T) (1 << nBit);

				// 해당 부위의 길이를 얻고, 표시를 변경한다.
				TAPE_MEASURE_TYPE_VALUE	typeValue;
				typeValue.type = type;
				float&	length = typeValue.value;
				gMesh.getValue(TYPE__TAPE_MEASURE, TAPE_MEASURE__TYPE, &typeValue);
				(void) fprintf(stdout, "TAPE_MEASURE_TYPE_T = %d: length = %f\n", 1 << nBit, length);

				TAPE_MEASURE_TYPE_VISIBLE	typeVisible;
				typeVisible.type = type;
				gMesh.getValue(TYPE__TAPE_MEASURE, TAPE_MEASURE__TYPE, &typeVisible);
				(void) fprintf(stdout, "TAPE_MEASURE_TYPE_T = %d: Visible: %s\n", type, typeVisible.visible ? "True" : "False");
				typeVisible.visible = !typeVisible.visible;
				gMesh.setValue(TYPE__TAPE_MEASURE, TAPE_MEASURE__TYPE, &typeVisible);
			}
#endif
#if 1	// 영역
			if (state.g__showDefinedArea) {	// 영역을 바꿔가면서...
				static int	nBit = 0;
				--nBit;
				if (nBit < 0)
					nBit = NUM_AREA_TYPE - 1;
				AREA_TYPE_T	type = (AREA_TYPE_T) (1 << nBit);

				// 해당 부위의 표시를 변경한다.
				AREA_TYPE_VISIBLE	typeVisible;
				typeVisible.type = type;
				bool&	visible = typeVisible.visible;
				gMesh.getValue(TYPE__AREA, AREA__TYPE, &typeVisible);
				(void) fprintf(stdout, "AREA_TYPE_T = %d: Visible: %s\n", type, visible ? "True" : "False");
				visible = !visible;
				gMesh.setValue(TYPE__AREA, AREA__TYPE, &typeVisible);
			}
#endif
#if 1	// 체성분
			if (state.g__showDefinedBodyElement) {	// 영역을 바꿔가면서...
				static	int	__vsBit = 0;	// vsBit 값을 저장;

				// 비트를 사용해...
				int	vsBit = __vsBit;	// 읽어서
				--vsBit;			// 바꾼 후
				if (vsBit < 0)
					vsBit = 5 /* NUM_BODY_ELEMENT_VS_TYPE */ - 1;	// 한 체성분만...
				__vsBit = vsBit;		// 다시 저장

				// 해당 부위의 표시를 변경한다.
				VS_VISIBLE	vsVisible;
				vsVisible.type = (BODY_ELEMENT_VS_T) (1 << vsBit);
				gMesh.getValue(TYPE__BODY_ELEMENT, BODY_ELEMENT__VS, &vsVisible);
				bool&	visible = vsVisible.visible;
				(void) fprintf(stdout, "BODY_ELEMENT_VS__TYPE = %d: Visible: %s\n", 1 << vsBit, visible ? "True" : "False");
				visible = !visible;
				gMesh.setValue(TYPE__BODY_ELEMENT, BODY_ELEMENT__VS, &vsVisible);
			}
#endif
#if 1	// 색깔템플릿
			if (state.g__showTemplateColor) {
				meshRef.DecIndexTemplateColor();
			}
#endif	// 색깔템플릿
#if 0	// group
			if (state.g__showGroup)
				--state.g__gid;
#endif
#if 1
		{
			VIEW_T	view;
			const VIEW_TYPE_T&	viewType = view.type;
			state.getValue(TYPE__VIEW, VIEW__TYPE, &view);
#if 0	// 자동으로 해당 뷰로 바꾼 후...
			if (
				viewType == VIEW_FRONT || viewType == VIEW_BACK ||
				viewType == VIEW_LEFT || viewType == VIEW_RIGHT
			) {
			} else {	// 해당 뷰로 바꾼 후...
				VIEW_T	view;
				view.type = VIEW_FRONT;
				state.setValue(TYPE__VIEW, VIEW__TYPE, &view);
			}
			// 회전한다.
			{
				ROTATION_T	rotation;
				ROTATE_STATE&	rotationState = rotation.state;
				state.getValue(TYPE__VIEW, VIEW__ROTATION, &rotation);
				if (rotationState == ROTATE_NONE) {
					rotationState = ROTATE_READY;
					rotation.clockwise = true;
					state.setValue(TYPE__VIEW, VIEW__ROTATION, &rotation);
				}
			}
#else
			if (
				viewType == VIEW_FRONT || viewType == VIEW_BACK ||
				viewType == VIEW_LEFT || viewType == VIEW_RIGHT
			) {
				ROTATION_T	rotation;
				ROTATE_STATE&	rotationState = rotation.state;
				state.getValue(TYPE__VIEW, VIEW__ROTATION, &rotation);
				if (rotationState == ROTATE_NONE) {
					rotationState = ROTATE_READY;
					rotation.clockwise = true;
					state.setValue(TYPE__VIEW, VIEW__ROTATION, &rotation);
				}
			}
#endif
		}
#else
			if (state.g__orthographicView && !state.g__rotating) {
				state.g__jKey = true;
				state.g__kKey = false;
			}
#endif
#if 0	// 모눈
			if (state.__z != GRID_UNDEF) {
#if 1	// 간격
				GRID_STEP	step;
				state.getValue(TYPE__GRID, GRID__INTERVAL, &step);
				step.v_step *= .9f;	// h_step
				state.setValue(TYPE__GRID, GRID__INTERVAL, &step);
#endif
#if 0	// 방향
				GRID_Z	z;
				state.getValue(TYPE__GRID, GRID__Z_DEPTH, &z);
				if (z != GRID_UNDEF) {
					GRID_DIR	dir;
					state.getValue(TYPE__GRID, GRID__DIRECTION, &dir);
					if (dir == GRID_HORIZONTAL)
						dir = GRID_VERTICAL;
					else if (dir == GRID_VERTICAL)
						dir = GRID_BOTH;
					else
						dir = GRID_HORIZONTAL;
					state.setValue(TYPE__GRID, GRID__DIRECTION, &dir);
				}
#endif
			}
#endif
#if 1	// 등고선
#if 1	// XY, YZ, ZX
		{
			ISOMETRIC_TYPE_T	type;
			int	rval = meshRef.getValue(TYPE__ISOMETRIC, ISOMETRIC__TYPE, &type);
#if 1	// 색깔 등고선을 따로 처리
			if (rval == 0 && type != ISOMETRIC_UNDEF) {
				ISOMETRIC_PLANE_T	plane;
				meshRef.getValue(TYPE__ISOMETRIC, ISOMETRIC__PLANE, &plane);
				if (type == ISOMETRIC_LINE) {
					int	iPlane = (int) plane;
					if (iPlane == 7)
						iPlane = 1;
					else
						++iPlane;
					plane = (ISOMETRIC_PLANE_T) iPlane;
				} else if (type == ISOMETRIC_COLOR) {
					if (plane & ISOMETRIC_XY)
						plane = ISOMETRIC_YZ;
					else if (plane & ISOMETRIC_YZ)
						plane = ISOMETRIC_ZX;
					else if (plane & ISOMETRIC_ZX)
						plane = ISOMETRIC_XY;
				}
				meshRef.setValue(TYPE__ISOMETRIC, ISOMETRIC__PLANE, &plane);
			}
#else
			if (!(type & ISOMETRIC_UNDEF0)) {
				ISOMETRIC_PLANE_T	plane;
				meshRef.getValue(TYPE__ISOMETRIC, ISOMETRIC__PLANE, &plane);
				if (type & ISOMETRIC_LINE0) {
					int	iPlane = (int) plane;
					if (iPlane == 7)
						iPlane = 1;
					else
						++iPlane;
					plane = (ISOMETRIC_PLANE_T) iPlane;
				} else if (type & ISOMETRIC_COLOR0) {
					if (plane & ISOMETRIC_XY)
						plane = ISOMETRIC_YZ;
					else if (plane & ISOMETRIC_YZ)
						plane = ISOMETRIC_ZX;
					else if (plane & ISOMETRIC_ZX)
						plane = ISOMETRIC_XY;
				}
				meshRef.setValue(TYPE__ISOMETRIC, ISOMETRIC__PLANE, &plane);
			}
#endif
		}
#endif
#endif
#if 1	// 시뮬레이션
		{
			SIMULATION_EXEC_TYPE	execType = SIMULATION_EXEC_MINUS;
			gMesh.setValue(TYPE__SIMULATION, SIMULATION__EXEC, &execType);

			// 변화된 부피는?
//			SIMULATION_CC_TYPE	ccType = SIMULATION_CC_TYPE_VOLUME_CHANGE;
//			인 경우에만 의미가 있다.
			float	volumeChange;
			gMesh.getValue(TYPE__SIMULATION, SIMULATION__VOLUME_CHANGE, &volumeChange);
			(void) fprintf(stdout, "Volume Change = %f\n", volumeChange);
		}
#endif
#if 1	// SHIFT
	if (state.DoShift) {
		const D3DSURFACE_DESC*	pBackBufferSurfaceDesc = DXUTGetD3D9BackBufferSurfaceDesc();
		float	W = (float) pBackBufferSurfaceDesc->Width;
		gMesh.SetShift(W / 8.f);
	}
#endif	// SHIFT
#if 1	// AREA2_HINT
	--state.area2_control.SmoothIncLevel;
#endif	// AREA2_HINT
#if 1	// 마지막 주문; 1. 회전축 고정
if (state.fixed_axis) {
	CD3DArcBall2::FIXED_AXIS::TYPE	fixed_axis_type = g_ArcBall.GetFixedAxisType();
	switch (fixed_axis_type) {
	case CD3DArcBall2::FIXED_AXIS::FREE:
		fixed_axis_type = CD3DArcBall2::FIXED_AXIS::Z;
		break;
	case CD3DArcBall2::FIXED_AXIS::X:
		fixed_axis_type = CD3DArcBall2::FIXED_AXIS::FREE;
		break;
	case CD3DArcBall2::FIXED_AXIS::Y:
		fixed_axis_type = CD3DArcBall2::FIXED_AXIS::X;
		break;
	case CD3DArcBall2::FIXED_AXIS::Z:
		fixed_axis_type = CD3DArcBall2::FIXED_AXIS::Y;
		break;
	default:
		throw;
	}
	// 먼저 초기상태로 돌아간 후...
	bool	reset = true;
	state.setValue(TYPE__VIEW, VIEW__HOME, &reset);
	state.g__n = D3DXVECTOR3(0, 1, 0);
	g_ArcBall.SetFixedAxisType(fixed_axis_type);
}
#endif	// 마지막 주문; 1. 회전축 고정
}
static void	__KKeyPressed()
{
////////////////////////////////////////////////////////////////////////
#ifdef USE__pointer_to_PickVar
	if (!gPickVarPtr)
		return;
	PickVar&	gPickVar = *gPickVarPtr;
#endif	// USE__pointer_to_PickVar
#ifdef USE__PickVar
	kState&	state = gPickVar.state;
#if 1	// 마지막 주문; 1. 회전축 고정
	CD3DArcBall2&	g_ArcBall = gPickVar.g_ArcBall;
#endif	// 마지막 주문; 1. 회전축 고정
#ifdef USE__pointer_to_kMesh
	kMesh*&	gkMeshPtr = gPickVar.gkMeshPtr;
	kMesh*&	gkMeshPtr0 = gPickVar.gkMeshPtr0;
#else	// USE__pointer_to_kMesh/!USE__pointer_to_kMesh
	kMesh&	gMesh = gPickVar.gMesh;
	kMesh&	gMesh0 = gPickVar.gMesh0;
#endif	// !USE__pointer_to_kMesh
//	CModelViewerCamera2&	g_Camera = gPickVar.g_Camera;
//	bool&	g_bShowHelp = gPickVar.g_bShowHelp;
#endif	// USE__PickVar
#ifdef USE__pointer_to_kMesh
	kMesh&	gMesh = *gkMeshPtr;
#endif	// USE__pointer_to_kMesh
#ifdef USE__pointer_to_kMesh
	kMesh&	gMesh0 = *gkMeshPtr0;
#endif	// USE__pointer_to_kMesh
//	cout << "KeyboardProc" << endl;
	kMesh*	kMeshPtr = &gMesh;
	if (state.g__pickStandard && &gMesh0 && gMesh0.meshLoader().meshPtr())
		kMeshPtr = &gMesh0;
	kMesh&	meshRef = *kMeshPtr;
////////////////////////////////////////////////////////////////////////

#if 1	// ColorBand
if (state.ColorBandControl) {
	if (!state.g__Shift && !state.g__Control && state.g__Alt) {	// ALT + 'K': type 증가
		COLOR_BAND_TYPE_T	type = COLOR_BAND_UNDEF;
		state.getValue(TYPE__COLOR_BAND, COLOR_BAND__TYPE, &type);
		int	iType = (int) type;
		++iType;
		if (iType > (int) COLOR_BAND_RIGHT_TO_LEFT)	// COLOR_BAND_RIGHT_TO_LEFT: 가장 큰 COLOR_BAND_TYPE_T
			iType = (int) COLOR_BAND_RIGHT_TO_LEFT;
		type = (COLOR_BAND_TYPE_T) iType;
		state.setValue(TYPE__COLOR_BAND, COLOR_BAND__TYPE, &type);

		if (type != COLOR_BAND_UNDEF) {	// 표시선 리셋
			kColorBand::IndexIndicator	indexIndicator;
			indexIndicator.index = 0;	// 첫 번째만 가지고 테스트
			indexIndicator.f0 = .7f;
			indexIndicator.width = 2.f;
			indexIndicator.scale = 0.f;
			indexIndicator.color = D3DCOLOR_RGBA(0, 255, 0, 255);
			state.setValue(TYPE__COLOR_BAND, COLOR_BAND__INDICATOR, &indexIndicator);
		}
	}
	if (!state.g__Shift && !state.g__Control && !state.g__Alt) {	// 'K': x_offset 증가
		COLOR_BAND_TYPE_T	type = COLOR_BAND_UNDEF;
		state.getValue(TYPE__COLOR_BAND, COLOR_BAND__TYPE, &type);
		if (type != COLOR_BAND_UNDEF) {
			kColorBand::Dimension	dimension;
			state.getValue(TYPE__COLOR_BAND, COLOR_BAND__DIMENSION, &dimension);
			dimension.x_offset += .01f;
			if (dimension.x_offset > 2.f)
				dimension.x_offset = 2.f;
			state.setValue(TYPE__COLOR_BAND, COLOR_BAND__DIMENSION, &dimension);
		}
	}
	if (!state.g__Shift && state.g__Control && !state.g__Alt) {	// CTRL + 'K': y_offset 증가
		COLOR_BAND_TYPE_T	type = COLOR_BAND_UNDEF;
		state.getValue(TYPE__COLOR_BAND, COLOR_BAND__TYPE, &type);
		if (type != COLOR_BAND_UNDEF) {
			kColorBand::Dimension	dimension;
			state.getValue(TYPE__COLOR_BAND, COLOR_BAND__DIMENSION, &dimension);
			dimension.y_offset += .01f;
			if (dimension.y_offset > 2.f)
				dimension.y_offset = 2.f;
			state.setValue(TYPE__COLOR_BAND, COLOR_BAND__DIMENSION, &dimension);
		}
	}
	if (state.g__Shift && !state.g__Control && !state.g__Alt) {	// SHIFT + 'K': width 증가
		COLOR_BAND_TYPE_T	type = COLOR_BAND_UNDEF;
		state.getValue(TYPE__COLOR_BAND, COLOR_BAND__TYPE, &type);
		if (type != COLOR_BAND_UNDEF) {
			kColorBand::Dimension	dimension;
			state.getValue(TYPE__COLOR_BAND, COLOR_BAND__DIMENSION, &dimension);
			dimension.width += .01f;
			if (dimension.width > 2.f)
				dimension.width = 2.f;
			state.setValue(TYPE__COLOR_BAND, COLOR_BAND__DIMENSION, &dimension);
		}
	}
	if (state.g__Shift && state.g__Control && !state.g__Alt) {	// SHIFT + CTRL + 'K': height 증가
		COLOR_BAND_TYPE_T	type = COLOR_BAND_UNDEF;
		state.getValue(TYPE__COLOR_BAND, COLOR_BAND__TYPE, &type);
		if (type != COLOR_BAND_UNDEF) {
			kColorBand::Dimension	dimension;
			state.getValue(TYPE__COLOR_BAND, COLOR_BAND__DIMENSION, &dimension);
			dimension.height += .01f;
			if (dimension.height > 2.f)
				dimension.height = 2.f;
			state.setValue(TYPE__COLOR_BAND, COLOR_BAND__DIMENSION, &dimension);
		}
	}
	if (state.g__Shift && !state.g__Control && state.g__Alt) {	// SHIFT + ALT + 'J': indicatorF0 증가
		COLOR_BAND_TYPE_T	type = COLOR_BAND_UNDEF;
		state.getValue(TYPE__COLOR_BAND, COLOR_BAND__TYPE, &type);
		if (type != COLOR_BAND_UNDEF) {
			kColorBand::IndexIndicator	indexIndicator;
			indexIndicator.index = 0;	// 첫 번째만 가지고 테스트
			state.getValue(TYPE__COLOR_BAND, COLOR_BAND__INDICATOR, &indexIndicator);
			indexIndicator.f0 += .01f;
			if (indexIndicator.f0 > 1.01f)	// 1.01 < 1: 표시선 표시 안 함
				indexIndicator.f0 = 1.01f;
			state.setValue(TYPE__COLOR_BAND, COLOR_BAND__INDICATOR, &indexIndicator);
		}
	}
	if (!state.g__Shift && state.g__Control && state.g__Alt) {	// CTRL + ALT + 'K': indicatorLineScale 증가
		COLOR_BAND_TYPE_T	type = COLOR_BAND_UNDEF;
		state.getValue(TYPE__COLOR_BAND, COLOR_BAND__TYPE, &type);
		if (type != COLOR_BAND_UNDEF) {
			kColorBand::IndexIndicator	indexIndicator;
			indexIndicator.index = 0;	// 첫 번째만 가지고 테스트
			state.getValue(TYPE__COLOR_BAND, COLOR_BAND__INDICATOR, &indexIndicator);
			indexIndicator.scale += .01f;
			state.setValue(TYPE__COLOR_BAND, COLOR_BAND__INDICATOR, &indexIndicator);
		}
	}
	if (state.g__Shift && state.g__Control && state.g__Alt) {	// SHIFT + CTRL + ALT + 'K': indicatorLineColor 변화
		COLOR_BAND_TYPE_T	type = COLOR_BAND_UNDEF;
		state.getValue(TYPE__COLOR_BAND, COLOR_BAND__TYPE, &type);
		if (type != COLOR_BAND_UNDEF) {
			static int	giColor = 1;	// 0: Red, 1: Green, 2: Blue
			++giColor;
			if (giColor > 2)
				giColor = 0;

			kColorBand::IndexIndicator	indexIndicator;
			indexIndicator.index = 0;	// 첫 번째만 가지고 테스트
			state.getValue(TYPE__COLOR_BAND, COLOR_BAND__INDICATOR, &indexIndicator);
			indexIndicator.color = giColor == 0 ? D3DCOLOR_ARGB(255, 255, 0, 0) : (giColor == 1 ? D3DCOLOR_ARGB(255, 0, 255, 0) : D3DCOLOR_ARGB(255, 0, 0, 255));
			state.setValue(TYPE__COLOR_BAND, COLOR_BAND__INDICATOR, &indexIndicator);
		}
	}
}
#endif	// ColorBand
#if 0	// Convex
			if (!state.g__show_all_convex_points && meshRef.tapeMeasure().convexPointArray().size()) {
				++state.g__next_convex_index;
				if (state.g__next_convex_index >= (int) meshRef.tapeMeasure().convexPointArray().size())
					state.g__next_convex_index = 0;
			}
#endif
#if 0	// 시뮬레이션
		{
			SIMULATION_EXEC_TYPE	execType = SIMULATION_EXEC_PLUS;
			gMesh.setValue(TYPE__SIMULATION, SIMULATION__EXEC, &execType);

//			SIMULATION_CC_TYPE	ccType = SIMULATION_CC_TYPE_VOLUME_CHANGE;
//			인 경우에만 의미가 있다.
			// 변화된 부피는?
			float	volumeChange;
			gMesh.getValue(TYPE__SIMULATION, SIMULATION__VOLUME_CHANGE, &volumeChange);
			(void) fprintf(stdout, "Volume Change = %f\n", volumeChange);
		}
#endif
#if 1	// 등고선
#if 1	// 방법
		{
			ISOMETRIC_TYPE_T	type;
			int	rval = meshRef.getValue(TYPE__ISOMETRIC, ISOMETRIC__TYPE, &type);
			if (rval == 0 && type != ISOMETRIC_UNDEF) {
				if (type == ISOMETRIC_LINE)
					type = ISOMETRIC_COLOR;
				else	// ISOMETRIC_COLOR
					type = ISOMETRIC_LINE;
				meshRef.setValue(TYPE__ISOMETRIC, ISOMETRIC__TYPE, &type);
			}
		}
#endif
#endif
#if 0	// 모눈
			if (state.__z != GRID_UNDEF) {
#if 0	// 위치
				GRID_Z	z;
				state.getValue(TYPE__GRID, GRID__Z_DEPTH, &z);
				if (z != GRID_UNDEF) {
					if (z == GRID_FRONT)
						z = GRID_BACK;
					else if (z == GRID_BACK)
						z = GRID_FRONT;
					state.setValue(TYPE__GRID, GRID__Z_DEPTH, &z);
				}
#endif
#if 1	// 간격
				GRID_STEP	step;
				state.getValue(TYPE__GRID, GRID__INTERVAL, &step);
				step.v_step *= 1.1f;	// h_step
				state.setValue(TYPE__GRID, GRID__INTERVAL, &step);
#endif
			}
#endif
#if 1	// 길이측정
			if (state.g__showDefinedMeasure) {	// 측정 부위를 바꿔가면서...
				static int	nBit = 19;
				++nBit;
				if (nBit == NUM_TAPE_MEASURE_TYPE)
					nBit = 0;
				TAPE_MEASURE_TYPE_T	type = (TAPE_MEASURE_TYPE_T) (1 << nBit);

				// 해당 부위의 길이를 얻고, 표시를 변경한다.
				TAPE_MEASURE_TYPE_VALUE	typeValue;
				typeValue.type = type;
				float&	length = typeValue.value;
				gMesh.getValue(TYPE__TAPE_MEASURE, TAPE_MEASURE__VALUE, &typeValue);
				(void) fprintf(stdout, "TAPE_MEASURE_TYPE_T = %d: length = %f\n", type, length);

				TAPE_MEASURE_TYPE_VISIBLE	typeVisible;
				typeVisible.type = type;
				bool&	visible = typeVisible.visible;
				gMesh.getValue(TYPE__TAPE_MEASURE, TAPE_MEASURE__TYPE, &typeVisible);
				(void) fprintf(stdout, "TAPE_MEASURE_TYPE_T = %d: Visible: %s\n", type, visible ? "True" : "False");
				visible = !visible;
				gMesh.setValue(TYPE__TAPE_MEASURE, TAPE_MEASURE__TYPE, &typeVisible);
			}
#endif
#if 1	// 영역
			if (state.g__showDefinedArea) {		// 영역을 바꿔가면서...
				static int	nBit = -1;
				++nBit;
				if (nBit == NUM_AREA_TYPE)
					nBit = 0;
				AREA_TYPE_T	type = (AREA_TYPE_T) (1 << nBit);

				AREA_TYPE_VISIBLE	typeVisible;
				typeVisible.type = type;
				bool&	visible = typeVisible.visible;
				gMesh.getValue(TYPE__AREA, AREA__TYPE, &typeVisible);
				(void) fprintf(stdout, "AREA_TYPE_T = %d: Visible: %s\n", type, visible ? "True" : "False");
				visible = !visible;
				gMesh.setValue(TYPE__AREA, AREA__TYPE, &typeVisible);
			}
#endif
#if 1	// 체성분
			if (state.g__showDefinedBodyElement) {	// 영역을 바꿔가면서...
				static	int	__vsBit = -1;	// vsBit 값을 저장;

				// 비트를 사용해...
				int	vsBit = __vsBit;	// 읽어서
				++vsBit;			// 바꾼 후
				if (vsBit == 5 /* NUM_BODY_ELEMENT_VS_TYPE */)	// 한 체성분만...
					vsBit = 0;
				__vsBit = vsBit;		// 다시 저장

				// 해당 부위의 표시를 변경한다.
				VS_VISIBLE	vsVisible;
				vsVisible.type = (BODY_ELEMENT_VS_T) (1 << vsBit);
				gMesh.getValue(TYPE__BODY_ELEMENT, BODY_ELEMENT__VS, &vsVisible);
				bool&	visible = vsVisible.visible;
				(void) fprintf(stdout, "BODY_ELEMENT_VS__TYPE = %d: Visible: %s\n", 1 << vsBit, visible ? "True" : "False");
				visible = !visible;
				gMesh.setValue(TYPE__BODY_ELEMENT, BODY_ELEMENT__VS, &vsVisible);
			}
#endif
#if 1	// 색깔템플릿
			if (state.g__showTemplateColor) {
				meshRef.IncIndexTemplateColor();
			}
#endif	// 색깔템플릿
#if 0	// group
			if (state.g__showGroup)
				++state.g__gid;
#endif
#if 1
		{
			VIEW_T	view;
			const VIEW_TYPE_T&	viewType = view.type;
			state.getValue(TYPE__VIEW, VIEW__TYPE, &view);
#if 0	// 자동으로 해당 뷰로 바꾼 후...
			if (
				viewType == VIEW_FRONT || viewType == VIEW_BACK ||
				viewType == VIEW_LEFT || viewType == VIEW_RIGHT
			) {
			} else {
				// 해당 뷰로 바꾼 후...
				VIEW_T	view;
				view.type = VIEW_FRONT;
				state.setValue(TYPE__VIEW, VIEW__TYPE, &view);
			}
			// 회전한다.
			{
				ROTATION_T	rotation;
				ROTATE_STATE&	rotationState = rotation.state;
				state.getValue(TYPE__VIEW, VIEW__ROTATION, &rotation);
				if (rotationState == ROTATE_NONE) {
					rotationState = ROTATE_READY;
					rotation.clockwise = false;
					state.setValue(TYPE__VIEW, VIEW__ROTATION, &rotation);
				}
			}
#else
			if (
				viewType == VIEW_FRONT || viewType == VIEW_BACK ||
				viewType == VIEW_LEFT || viewType == VIEW_RIGHT
			) {
				ROTATION_T	rotation;
				ROTATE_STATE&	rotationState = rotation.state;
				state.getValue(TYPE__VIEW, VIEW__ROTATION, &rotation);
				if (rotationState == ROTATE_NONE) {
					rotationState = ROTATE_READY;
					rotation.clockwise = false;
					state.setValue(TYPE__VIEW, VIEW__ROTATION, &rotation);
				}
			}
#endif
		}
#else
			if (state.g__orthographicView && !state.g__rotating) {
				state.g__jKey = false;
				state.g__kKey = true;
			}
#endif
#if 1	// SHIFT
	if (state.DoShift) {
		const D3DSURFACE_DESC*	pBackBufferSurfaceDesc = DXUTGetD3D9BackBufferSurfaceDesc();
		float	W = (float) pBackBufferSurfaceDesc->Width;
		gMesh.SetShift(W / 4.f * 3.f);
	}
#endif	// SHIFT
#if 1	// AREA2_HINT
	++state.area2_control.SmoothIncLevel;
#endif	// AREA2_HINT

#if 1	// 마지막 주문; 1. 회전축 고정
if (state.fixed_axis) {
	CD3DArcBall2::FIXED_AXIS::TYPE	fixed_axis_type = g_ArcBall.GetFixedAxisType();
	switch (fixed_axis_type) {
	case CD3DArcBall2::FIXED_AXIS::FREE:
		fixed_axis_type = CD3DArcBall2::FIXED_AXIS::X;
		break;
	case CD3DArcBall2::FIXED_AXIS::X:
		fixed_axis_type = CD3DArcBall2::FIXED_AXIS::Y;
		break;
	case CD3DArcBall2::FIXED_AXIS::Y:
		fixed_axis_type = CD3DArcBall2::FIXED_AXIS::Z;
		break;
	case CD3DArcBall2::FIXED_AXIS::Z:
		fixed_axis_type = CD3DArcBall2::FIXED_AXIS::FREE;
		break;
	default:
		throw;
	}
	// 먼저 초기상태로 돌아간 후...
	bool	reset = true;
	state.setValue(TYPE__VIEW, VIEW__HOME, &reset);
	state.g__n = D3DXVECTOR3(0, 1, 0);
	g_ArcBall.SetFixedAxisType(fixed_axis_type);
}
#endif	// 마지막 주문; 1. 회전축 고정
}
static void	__F12KeyPressed()
{
////////////////////////////////////////////////////////////////////////
#ifdef USE__pointer_to_PickVar
	if (!gPickVarPtr)
		return;
	PickVar&	gPickVar = *gPickVarPtr;
#endif	// USE__pointer_to_PickVar
#ifdef USE__PickVar
	kState&	state = gPickVar.state;
#ifdef USE__pointer_to_kMesh
	kMesh*&	gkMeshPtr = gPickVar.gkMeshPtr;
	kMesh*&	gkMeshPtr0 = gPickVar.gkMeshPtr0;
#else	// USE__pointer_to_kMesh/!USE__pointer_to_kMesh
	kMesh&	gMesh = gPickVar.gMesh;
	kMesh&	gMesh0 = gPickVar.gMesh0;
#endif	// !USE__pointer_to_kMesh
//	CModelViewerCamera2&	g_Camera = gPickVar.g_Camera;
//	bool&	g_bShowHelp = gPickVar.g_bShowHelp;
#endif	// USE__PickVar
#ifdef USE__pointer_to_kMesh
	kMesh&	gMesh = *gkMeshPtr;
#endif	// USE__pointer_to_kMesh
#ifdef USE__pointer_to_kMesh
	kMesh&	gMesh0 = *gkMeshPtr0;
#endif	// USE__pointer_to_kMesh
//	cout << "KeyboardProc" << endl;
	kMesh*	kMeshPtr = &gMesh;
	if (state.g__pickStandard && &gMesh0 && gMesh0.meshLoader().meshPtr())
		kMeshPtr = &gMesh0;
	kMesh&	meshRef = *kMeshPtr;
////////////////////////////////////////////////////////////////////////

	kBrush&	brush = state.brush;
	if (state.g__Shift && state.g__Control && state.g__Alt) {	// Shift + Ctrl + Alt
		if (brush.flag.On) {
			brush.Type = brush.Type == kBrush::BRUSH_STANDARD ? kBrush::BRUSH_FLATTEN : kBrush::BRUSH_STANDARD;
		}
#if 1	// 모델스냅샷
		else {
			bool	overwrite = false;
			if (meshRef.snapshot_save(state.area2_control.SmoothIncLevel, overwrite) < 0)
				cerr << "kMesh::snapshot_save(index = " << state.area2_control.SmoothIncLevel << "): Failure" << endl;
			else
				cout << "kMesh::snapshot_save(index = " << state.area2_control.SmoothIncLevel << "): Success" << endl;
		}
#endif	// 모델스냅샷
	} else if (state.g__Shift && state.g__Control && !state.g__Alt) {	// SHIFT + CTRL
		if (brush.flag.On) {
			brush.flag.Apply = !brush.flag.Apply;
		}
	} else if (state.g__Shift && !state.g__Control && !state.g__Alt) {	// SHIFT
//		if (brush.flag.On) {
			brush.flag.Sub = !brush.flag.Sub;
//		}
	} else if (!state.g__Shift && state.g__Control && state.g__Alt) {	// CTRL + ALT
		if (brush.flag.On) {
			brush.flag.Show = !brush.flag.Show;
			if (!brush.flag.Show) {
				state.debugging_FaceID_array.resize(0);
				state.debugging_prReprID_array.resize(0);
			}
		}
#if 1	// 모델스냅샷
		else {
			cout << state.area2_control.SmoothIncLevel << endl;
			if (meshRef.snapshot_load(state.area2_control.SmoothIncLevel) < 0)
				cerr << "kMesh::snapshot_load(index = " << state.area2_control.SmoothIncLevel << "): Failure" << endl;
			else
				cout << "kMesh::snapshot_load(index = " << state.area2_control.SmoothIncLevel << "): Success" << endl;
		}
#endif	// 모델스냅샷
	} else if (!state.g__Shift && state.g__Control && !state.g__Alt) {	// CTRL
//		if (brush.flag.On) {
			brush.flag.Smooth = !brush.flag.Smooth;
//		}
	} else if (!state.g__Control && state.g__Alt) {	// [SHIFT + ]ALT
		if (brush.flag.On) {
			brush.flag.Sculpture = !brush.flag.Sculpture;
		} else {
#if 1
#if 1
#if 1
#if 0
			float	dV;
			if (gMesh.simulateWholeBody2(true, 0, dV) < 0)
				cerr << "Oops!" << endl;
			else
				cout << "Ok!" << endl;
#else
			state.playback = !state.playback;
			if (state.playback) {
				state.playback_index = -kState::VOLUME_MIN / kState::VOLUME_STEP;
				state.playback_backward = state.g__Shift;
			}
#endif
#else
			float	dV0 = 1.f;//11.3249f;//7.790f;
			if (state.g__Shift)
				dV0 = -dV0;
			float	scale;
			float	dV;
			float	dV_dev0 = 1.f;	// 1.0%
			float	dV_dev;
			if (meshRef.simulateWholeBody1(true, dV0, scale, &dV, dV_dev0, &dV_dev) < 0) {
				cerr << "kMesh::simulateWholeBody0(dV0 = " << dV0 << "): Failure" << endl;
			} else {
				cout << "kMesh::simulateWholeBody0(dV0 = " << dV0 << "): Success: dV = " << dV << " (" << dV_dev << "%) w/ scale = " << scale << endl;
			}
#endif
#else
			float	scale = 0.625;//-1.80444e-008f;//state.scale_alpha;
			float	dV;
			float	vol;
			if (state.g__Shift)
				scale = -scale;
			if (meshRef.simulateWholeBody(scale, &dV, &vol) < 0) {
				cerr << "kMesh::simulateWholeBody(" << scale << "): Failure" << endl;
			} else {
				cout << "kMesh::simulateWholeBody(" << scale << "): Success:";
				cout << " dV = " << dV << " (V = " << vol << ")" << endl;
			}
#endif
#else
			meshRef.elongate();
#endif
		}
	} else if (!state.g__Shift && !state.g__Control && !state.g__Alt) {
		toggleBrushMode();
		if (brush.flag.On) {
			brush.Type = kBrush::BRUSH_STANDARD;
		} else {
			brush.Reset();
			state.intersection_ptr = NULL;

			state.debugging_prReprID_array.resize(0);
			state.debugging_FaceID_array.resize(0);
			meshRef.select().SetNumberOfPoints(0);
		}
	}
}
//--------------------------------------------------------------------------------------
// As a convenience, DXUT inspects the incoming windows messages for
// keystroke messages and decodes the message parameters to pass relevant keyboard
// messages to the application.  The framework does not remove the underlying keystroke
// messages, which are still passed to the application's MsgProc callback.
//--------------------------------------------------------------------------------------
void
CALLBACK KeyboardProc(
	UINT	nChar,
	bool	bKeyDown,
	bool	bAltDown,
	void*	pUserContext
)
{
#ifdef USE__pointer_to_PickVar
	if (!gPickVarPtr)
		return;
	PickVar&	gPickVar = *gPickVarPtr;
#endif	// USE__pointer_to_PickVar
#ifdef USE__PickVar
	kState&	state = gPickVar.state;
#ifdef USE__pointer_to_kMesh
	kMesh*&	gkMeshPtr = gPickVar.gkMeshPtr;
	kMesh*&	gkMeshPtr0 = gPickVar.gkMeshPtr0;
#else	// USE__pointer_to_kMesh/!USE__pointer_to_kMesh
	kMesh&	gMesh = gPickVar.gMesh;
	kMesh&	gMesh0 = gPickVar.gMesh0;
#endif	// !USE__pointer_to_kMesh
	CModelViewerCamera2&	g_Camera = gPickVar.g_Camera;
	bool&	g_bShowHelp = gPickVar.g_bShowHelp;
#endif	// USE__PickVar
#ifdef USE__pointer_to_kMesh
	if (!gkMeshPtr) {
#ifdef USE__fileOpen	// F8 에만 사용되어야 한다!
		if (nChar != VK_F8 && nChar != VK_SHIFT && nChar != VK_CONTROL)
			return;
#else	// USE__fileOpen
		return;
#endif	// !USE__fileOpen
	}
	kMesh&	gMesh = *gkMeshPtr;
#endif	// USE__pointer_to_kMesh
#ifdef USE__pointer_to_kMesh
	kMesh&	gMesh0 = *gkMeshPtr0;
#endif	// USE__pointer_to_kMesh
//	cout << "KeyboardProc" << endl;
	kMesh*	kMeshPtr = &gMesh;
	if (state.g__pickStandard && &gMesh0 && gMesh0.meshLoader().meshPtr())
		kMeshPtr = &gMesh0;
	kMesh&	meshRef = *kMeshPtr;

	state.g__Alt = bAltDown;
#if 1	// VK_RETURN
	if (state.message.keyPressed) {
		char*	varName = state.message.varName;
		unsigned&	num_chars = state.message.numChars;
		const unsigned	max_chars = state.message.maxChars;

		// SHIFT 키 처리; 대소문자 구별하기 위해...
//		(void) fprintf(stderr, "VK_RETURN: Key: 0x%02X\n", nChar);
		if (bKeyDown && nChar == VK_SHIFT) {
			state.g__Shift = true;
			return;
		} else if (!bKeyDown && nChar == VK_SHIFT) {
			state.g__Shift = false;
			return;
		}

		if (bKeyDown)	// 키를 떼었을 때 판단한다.
			return;

		if (!isalnum(nChar) && nChar != VK_BACK && nChar != VK_OEM_MINUS && nChar != VK_RETURN) {
			(void) fprintf(stderr, "VK_RETURN: Invalid Key('%c'): 0x%02X\n", nChar, nChar);
			return;
		}
//		(void) fprintf(stdout, "VK_RETURN: SHIFT? %s\n", state.g__Shift ? "On" : "Off");
//		(void) fprintf(stdout, "VK_RETURN: Valid Key('%c') Pressed\n", nChar);

		if (nChar == VK_BACK) {
			if (num_chars == 0)	// do nothing
				return;
			varName[--num_chars] = '\0';
////			(void) fprintf(stdout, "VK_RETURN: varName:= |%s| (%d)\n", varName, num_chars);
			return;
		}

		if (nChar == VK_OEM_MINUS) {
			if (!state.g__Shift) {	// '_' 가 아니다.
				(void) fprintf(stderr, "VK_RETURN: No '_'\n");
				return;
			}
			nChar = '_';
		}
		if (isdigit(nChar)) {
			if (state.g__Shift) {	// 숫자가 아니다.
				(void) fprintf(stderr, "VK_RETURN: No number\n");
				return;
			}
		       	if (num_chars == 0) {
				(void) fprintf(stderr, "VK_RETURN: digit('%c') cannot be first character\n", nChar);
				return;
			}
		}

		if (num_chars == max_chars) {
			(void) fprintf(stderr, "VK_RETURN: Key buffer full\n");
			return;
		}

		if (nChar == VK_RETURN) {	// 결과출력 또는 kState::ReturnKeyPressed 해제
			if (num_chars == 0) {	// 해제
				(void) fprintf(stdout, "VK_RETURN: 해제\n");
				state.message.keyPressed = false;
			} else {	// 출력
				(void) fprintf(stdout, "VK_RETURN: /* ... */	varName:= |%s| (%d)\n", varName, num_chars);
				num_chars = 0;	// 처음부터 다시...;
						// 문자열 자체를 써 먹을 수 있으니 아직 문자열을 ""로 만들지 않는다.
			}
			return;
		}

		if (isalpha(nChar) && !state.g__Shift)
			nChar = 'a' + (nChar - 'A');
		varName[num_chars++] = (char) nChar;
		varName[num_chars] = '\0';	// 문자열 끝 표시
////		(void) fprintf(stdout, "VK_RETURN: varName:= |%s| (%d)\n", varName, num_chars);

		return;
	}
#endif	// VK_RETURN
	if (bKeyDown) {
		switch (nChar) {
#if 1
		case '0':
			if (!state.g__Shift && !state.g__Control && !state.g__Alt) {	// '0' only
#if 1	// kView
				VIEW_T	view;
				view.type = VIEW_PERSPECTIVE;
				view.orthographic = false;
				state.setValue(TYPE__VIEW, VIEW__TYPE, &view);
				g_Camera.loadProjMatrix();
#endif
			}
#if 1	// ColorBand
			if (!state.g__Shift && !state.g__Control && state.g__Alt) {	// ALT + '0': ColorBand
				state.ColorBandControl = !state.ColorBandControl;
			}
#endif	// ColorBand
#if 1	// AREA2_HINT
			if (state.g__Control && state.g__Alt) {
				if (!state.g__Shift) {	// CTRL + ATL + '0'
					++state.area2_control.ShowArea2;
					if (state.area2_control.ShowArea2 == 3)
						state.area2_control.ShowArea2 = 0;
				} else			// CTRL + ALT + SHIFT + '0'
					state.area2_control.Repos = !state.area2_control.Repos;
			}
			if (state.g__Shift && !state.g__Control && !state.g__Alt) {	// SHIFT + '0'
			}
			if (state.g__Shift && !state.g__Control && state.g__Alt) {	// SHIFT + ALT + '0'
				++state.area2_control.Smooth;
				if (state.area2_control.Smooth == 3)
					state.area2_control.Smooth = 0;
			}
			if (state.g__Control && !state.g__Alt) {
				if (state.g__Shift)			// CTRL + SHIFT + '0'
					--state.Area2Threshold;
				else					// CTRL + '0'
					++state.Area2Threshold;
			}
#endif	// AREA2_HINT
			break;
		case '1':
			__1KeyPressed();
			break;
		case '2':
			__2KeyPressed();
			break;
		case '3':
			__3KeyPressed();
			break;
		case '4':
#if 1	// kModel/kGrid
			if (state.g__Shift && !state.g__Control && !state.g__Alt) {
				state.g__showShape = !state.g__showShape;
			}
#else
			if (state.g__Shift && state.g__Control)
				state.g__showVertex = !state.g__showVertex;
			else if (state.g__Shift)
				state.g__showShape = !state.g__showShape;
			else if (state.g__Control)
				state.g__showGrid = !state.g__showGrid;
			else
				state.g__bGridBack = !state.g__bGridBack;
#endif
			break;
		case '5':
#if 1	// kIsometric
			if (state.g__Shift && state.g__Control && !state.g__Alt) {
				state.g__showContour = !state.g__showContour;
			}
#else
			if (state.g__Shift && state.g__Control) {
				state.g__showContour = !state.g__showContour;
			} else if (state.g__Shift)
				state.g__showIsometricLine_x = !state.g__showIsometricLine_x;
			else if (state.g__Control)
				state.g__showIsometricLine_y = !state.g__showIsometricLine_y;
			else
				state.g__showIsometricLine_z = !state.g__showIsometricLine_z;
#endif
			break;
		case '6':
			if (state.g__Shift && state.g__Control && !state.g__Alt) {	// SHIFT + CTRL + '6'
				if (state.g__bString) {
					state.g__showLowerFace = !state.g__showLowerFace;
#if 1	// 마지막 주문; 6. 3D 단면
					meshRef.Set3DCrossSection(state.g__showLowerFace);
#endif	// 마지막 주문; 6. 3D 단면
				}
			}
			if (state.g__Shift && !state.g__Control && !state.g__Alt) {	// SHIFT + '6'
				if (!state.g__showLowerFace) {
					SHADING_TYPE	shadingType = SHADING_TEXTURE;
					meshRef.setValue(TYPE__MODEL, MODEL__SHADING, &shadingType);
				}
			}
#if 1	// 의상
#ifndef __KEKO__
			if (state.g__Shift && !state.g__Control && state.g__Alt) {	// SHIFT + ALT + '6'
	int	index_to_standard = gMesh.index_to_standard();
if (index_to_standard == 6 /* 에트리 표준모델 - 091211 (남: Michael) */ || index_to_standard == 7 /* 에트리 표준모델 - 091211 (여: Victoria) */) {
	int	subset_index;	// 여기서 사용되는 몸의 subset 의 인덱스;
				// 전체(얼굴, 눈, 눈썹, 몸)이 있을 경우 3(ID = 0, 1),
				// 몸만 있는 경우에는 0 (ID = 3, 4, 5, 6, 7)
	subset_index = index_to_standard == 1 /* ETRI 초기 표준모델 */ ? 3 : 0;
	WCHAR		bmpFile[MAX_PATH];
	swprintf_s(bmpFile, MAX_PATH, L"Media/Scanner/에트리표준모델-091211/%s/tex/%s-cloth.bmp", index_to_standard == 6 ? L"michael" : L"victoria", index_to_standard == 6 ? L"michael" : L"victoria");
	kMeshLoader::Material&	material = *gMesh.meshLoader().GetMaterials().GetAt(subset_index);
	SAFE_RELEASE(material.texturePtr);
	D3DXCreateTextureFromFile(DXUTGetD3D9Device(), bmpFile, &material.texturePtr);
}
			}
#endif	// __KEKO__
#endif	// 의상
			if (!state.g__Shift && state.g__Control && !state.g__Alt) {	// CTRL + '6'
				if (!state.g__showLowerFace) {
					SHADING_TYPE	shadingType = SHADING_GREY;
					meshRef.setValue(TYPE__MODEL, MODEL__SHADING, &shadingType);
				}
			}
			if (!state.g__Shift && !state.g__Control && !state.g__Alt) {	// '6'
				if (!state.g__showLowerFace) {
					SHADING_TYPE	shadingType = SHADING_NONE;
					meshRef.setValue(TYPE__MODEL, MODEL__SHADING, &shadingType);
				}
			}
			break;
		case '7':
			if (state.g__Shift && state.g__Control && state.g__Alt) {	// SHIFT + CTRL + ALT
//				state.g__subset = !state.g__subset;
				state.g__showTemplateColor = !state.g__showTemplateColor;
				if (state.g__showTemplateColor) {
					meshRef.SaveColor();
				} else {
					meshRef.RestoreColor();
				}
			}
			if (state.g__Shift && !state.g__Control && state.g__Alt) {	// SHIFT + ALT
				if (&gMesh0 && gMesh0.meshLoader().meshPtr()) {
					int	nModelType = state.modelType;
					nModelType ^= MODEL_TYPE__STANDARD;
					state.modelType = (MODEL_TYPE_T) nModelType;
				}
			}
			if (state.g__Shift && state.g__Control && !state.g__Alt) {	// SHIFT + CTRL
				bool	reset = true;
				state.setValue(TYPE__VIEW, VIEW__HOME, &reset);
				state.g__n = D3DXVECTOR3(0, 1, 0);
			}
			if (!state.g__Shift && state.g__Control && state.g__Alt) {	// CTRL + ALT
				state.g__drawPivot = !state.g__drawPivot;
			}
			if (state.g__Shift && !state.g__Control && !state.g__Alt) {	// SHIFT
				state.g__drawAxis = !state.g__drawAxis;
			}
			if (!state.g__Shift && !state.g__Control && state.g__Alt) {	// ALT
			}
			if (!state.g__Shift && state.g__Control && !state.g__Alt) {	// CTRL
				state.g__drawAxis0 = !state.g__drawAxis0;
			}
			if (!state.g__Shift && !state.g__Control && !state.g__Alt) {	// NONE
				state.g__showText = !state.g__showText;
			}
			break;
		case '8':
			if (state.g__Shift && !state.g__Control && state.g__Alt) {	// SHIFT + ALT + 8
				state.g__pickStandard = !state.g__pickStandard;
			}
			if (state.g__Shift && !state.g__Control && !state.g__Alt) {	// SHIFT + 8
//				state.printColor = true;
{
		(void) fprintf(stdout, "TEMPLATE_COLOR...\n");
		LPDIRECT3DVERTEXBUFFER9	pVB = meshRef.GetVertexBufferPointer();
		kMesh::D3DXVERTEX*	pV;
		pVB->Lock(0, 0, (void**) &pV, 0);
		const CPointRepr*	pPointRepr = meshRef.GetPointReprPointer();
		int	n = 0;
		// n 을 정하기 위해...
		for (DWORD i = 0; i < meshRef.GetNumPointReprs(); ++i) {
			DWORD	prID = pPointRepr[i].id;
			const D3DXVECTOR3&	cColor = pV[prID].c;
			if (cColor.x == 0.f && cColor.y == 0.f && cColor.z == 0.f)
				continue;

			FACE_ID_LOCAL_INDEX	faceIDLocalIndex;
			if (meshRef.GetFaceIDLocalIndex(prID, faceIDLocalIndex) == 0)
				++n;
		}
#if 1	// VK_RETURN
		const char*	varName = state.message.varName;
#ifndef __KEKO__
		(void) fprintf(state.filePtr, "// /* ... */	{%s, %d},\n", varName, n);
#endif	// !__KEKO__
		(void) fprintf(stdout, "// /* ... */	{%s, %d},\n", varName, n);
#ifndef __KEKO__
		(void) fprintf(state.filePtr, "\tMODEL_COLOR	%s[%d] = {\t// varName:= |%s| (%d)\n", varName, n, varName, n);
#endif	// !__KEKO__
		(void) fprintf(stdout, "\tMODEL_COLOR	%s[%d] = {\t// varName:= |%s| (%d)\n", varName, n, varName, n);
#endif	// VK_RETURN
		for (DWORD i = 0; i < meshRef.GetNumPointReprs(); ++i) {
			DWORD	prID = pPointRepr[i].id;
			const D3DXVECTOR3&	cColor = pV[prID].c;
			if (cColor.x == 0.f && cColor.y == 0.f && cColor.z == 0.f)
				continue;

			FACE_ID_LOCAL_INDEX	faceIDLocalIndex;
			if (meshRef.GetFaceIDLocalIndex(prID, faceIDLocalIndex) == 0) {
#ifndef __KEKO__
				(void) fprintf(state.filePtr, "\t\t{%d, %d,\t%ff, %ff, %ff},\n", faceIDLocalIndex.ID, faceIDLocalIndex.local_index, cColor.x, cColor.y, cColor.z);
#endif	// !__KEKO__
				(void) fprintf(stdout, "\t\t{%d, %d,\t%ff, %ff, %ff},\n", faceIDLocalIndex.ID, faceIDLocalIndex.local_index, cColor.x, cColor.y, cColor.z);
			}
		}
#if 1	// VK_RETURN
#ifndef __KEKO__
		(void) fprintf(state.filePtr, "\t};\t// varName: |%s| (%d)\n", varName, n);
#endif	// !__KEKO__
		(void) fprintf(stdout, "\t};\t// varName: |%s| (%d)\n", varName, n);
#endif	// VK_RETURN
		pVB->Unlock();
}
			}
			if (!state.g__Shift && !state.g__Control && state.g__Alt) {	// ALT + 8
				state.g__showDefinedBodyElement = !state.g__showDefinedBodyElement;
				if (state.g__showDefinedBodyElement && state.g__showDefinedArea) {
					state.g__showDefinedArea = false;
				}
#if 1	// ColorBand
				if (state.g__showDefinedBodyElement) {	// 색깔띠 표시
					COLOR_BAND_TYPE_T	type = COLOR_BAND_BOTTOM_TO_TOP;
					state.setValue(TYPE__COLOR_BAND, COLOR_BAND__TYPE, &type);

					// dimension
					kColorBand::Dimension	dimension;
					dimension.x_offset = .1f;
					dimension.width = .1f;
					dimension.y_offset = .4f;
					dimension.height = .9f;
					state.setValue(TYPE__COLOR_BAND, COLOR_BAND__DIMENSION, &dimension);

					// 표시선 리셋
					for (int n = 0; n < NUM_INDICATORS; ++n) {
						kColorBand::IndexIndicator	indexIndicator;
						indexIndicator.index = n;	// 첫 번째만 가지고 테스트
						indexIndicator.f0 = -1.f;
						indexIndicator.width = 2.f;
						indexIndicator.scale = 0.f;
						indexIndicator.color = D3DCOLOR_RGBA(0, 255, 0, 255);
						state.setValue(TYPE__COLOR_BAND, COLOR_BAND__INDICATOR, &indexIndicator);
					}

					kColorBand::IndexIndicator	indexIndicator;
					// 적절량: index = 0 (white)
					indexIndicator.index = 0;
					indexIndicator.f0 = .5f;	// 0.5 를 기준으로...
					indexIndicator.width = 2.f;
					indexIndicator.scale = 0.f;
					indexIndicator.color = D3DCOLOR_RGBA(255, 255, 255, 255);
					state.setValue(TYPE__COLOR_BAND, COLOR_BAND__INDICATOR, &indexIndicator);
					// 10% 내외: index = 1(green), 2(cyan)
					indexIndicator.index = 1;
					indexIndicator.f0 = .5f - .05f;	// -10%
					indexIndicator.width = 1.f;
					indexIndicator.scale = 0.f;
					indexIndicator.color = D3DCOLOR_RGBA(0, 255, 0, 255);
					state.setValue(TYPE__COLOR_BAND, COLOR_BAND__INDICATOR, &indexIndicator);
					indexIndicator.index = 2;
					indexIndicator.f0 = .5f + .05f;	// +10%
					indexIndicator.width = 1.f;
					indexIndicator.scale = 0.f;
					indexIndicator.color = D3DCOLOR_RGBA(0, 255, 255, 255);
					state.setValue(TYPE__COLOR_BAND, COLOR_BAND__INDICATOR, &indexIndicator);
					// 20% 내외: index = 3(blue), 4(pink)
					indexIndicator.index = 3;
					indexIndicator.f0 = .5f - .10f;	// -20%
					indexIndicator.width = 1.f;
					indexIndicator.scale = 0.f;
					indexIndicator.color = D3DCOLOR_RGBA(0, 0, 255, 255);
					state.setValue(TYPE__COLOR_BAND, COLOR_BAND__INDICATOR, &indexIndicator);
					indexIndicator.index = 4;
					indexIndicator.f0 = .5f + .10f;	// +20%
					indexIndicator.width = 1.f;
					indexIndicator.scale = 0.f;
					indexIndicator.color = D3DCOLOR_RGBA(255, 0, 255, 255);
					state.setValue(TYPE__COLOR_BAND, COLOR_BAND__INDICATOR, &indexIndicator);
					// 30% 내외: index = 5(yellow), 6(red)
					indexIndicator.index = 5;
					indexIndicator.f0 = .5f - .15f;	// -30%
					indexIndicator.width = 1.f;
					indexIndicator.scale = 0.f;
					indexIndicator.color = D3DCOLOR_RGBA(255, 255, 0, 255);
					state.setValue(TYPE__COLOR_BAND, COLOR_BAND__INDICATOR, &indexIndicator);
					indexIndicator.index = 6;
					indexIndicator.f0 = .5f + .15f;	// +30%
					indexIndicator.width = 1.f;
					indexIndicator.scale = 0.f;
					indexIndicator.color = D3DCOLOR_RGBA(255, 0, 0, 255);
					state.setValue(TYPE__COLOR_BAND, COLOR_BAND__INDICATOR, &indexIndicator);
				} else {	// 색깔띠 미표시
					COLOR_BAND_TYPE_T	type = COLOR_BAND_UNDEF;
					state.setValue(TYPE__COLOR_BAND, COLOR_BAND__TYPE, &type);

					// 표시선 리셋
					for (int n = 0; n < NUM_INDICATORS; ++n) {
						kColorBand::IndexIndicator	indexIndicator;
						indexIndicator.index = n;	// 첫 번째만 가지고 테스트
						indexIndicator.f0 = -1.f;
						indexIndicator.width = 2.f;
						indexIndicator.scale = 0.f;
						indexIndicator.color = D3DCOLOR_RGBA(0, 255, 0, 255);
						state.setValue(TYPE__COLOR_BAND, COLOR_BAND__INDICATOR, &indexIndicator);
					}
				}
#endif	// ColorBand

			}
			if (state.g__Shift && state.g__Control && state.g__Alt) {	// SHIFT + CTRL + ALT + 8
				state.g__showDefinedArea = !state.g__showDefinedArea;
				if (state.g__showDefinedArea && state.g__showDefinedBodyElement) {
					state.g__showDefinedBodyElement = false;
				}
			}
			if (state.g__Shift && state.g__Control && !state.g__Alt) {	// SHIFT + CTRL + 8
				// 선택한 점들을 포함하는 Face 들의 ID 목록을 얻고자 할 때 설정한다.
//				state.printFaceIDList = true;
#if 1
		if (meshRef.meshLoader().meshPtr()) {
#if 1	// debugging
		const std::set<DWORD>&	selectedPrIDSet = gMesh.select().selectedPrIDSet();
		for (std::set<DWORD>::const_iterator it = selectedPrIDSet.begin(); it != selectedPrIDSet.end(); ++it) {
			DWORD	prID = *it;
			cout << prID << endl;
		}
#endif	// debugging
			// 표준모델에서 선택된 점들을 포함하는 Face 들의 목록을 출력한다;
			// Face 의 순서는 같으므로 측정모델에서 해당 영역을 선택할 때 사용된다.
			std::vector<DWORD>	selectedFaceIDList;
			meshRef.select().GetSelectedFaceIDList(selectedFaceIDList);
			int	n = selectedFaceIDList.size();
			(void) fprintf(stdout, "Selected Face IDs...\n");
#if 1	// VK_RETURN
			const char*	varName = state.message.varName;
#ifndef __KEKO__
			(void) fprintf(state.filePtr, "// /* ... */	{%s, %d},\n", varName, n);
#endif	// !__KEKO__
			(void) fprintf(stdout, "// /* ... */	{%s, %d},\n", varName, n);
#ifndef __KEKO__
			(void) fprintf(state.filePtr, "\tDWORD	%s[%d] = {\t// varName:= |%s| (%d)\n", varName, n, varName, n);
#endif	// !__KEKO__
			(void) fprintf(stdout, "\tDWORD	%s[%d] = {\t// varName:= |%s| (%d)\n", varName, n, varName, n);
#else	// VK_RETURN/!VK_RETURN
			(void) fprintf(stdout, "{\n");
#endif	// !VK_RETURN
			for (int i = 0; i < n; ++i) {
				if (i % 5 == 0) {
#ifndef __KEKO__
					(void) fprintf(state.filePtr, "%s\t\t", i ? "\n" : "");
#endif	// !__KEKO__
					(void) fprintf(stdout, "%s\t\t", i ? "\n" : "");
				}
#ifndef __KEKO__
				(void) fprintf(state.filePtr, "%d,", selectedFaceIDList.at(i));
#endif	// !__KEKO__
				(void) fprintf(stdout, "%d,", selectedFaceIDList.at(i));
			}
			if (n) {
#ifndef __KEKO__
				(void) fprintf(state.filePtr, "\n");
#endif	// !__KEKO__
				(void) fprintf(stdout, "\n");
			}
#if 1	// VK_RETURN
#ifndef __KEKO__
			(void) fprintf(state.filePtr, "\t};\t// varName: |%s| (%d)\n", varName, n);
#endif	// !__KEKO__
			(void) fprintf(stdout, "\t};\t// varName: |%s| (%d)\n", varName, n);
#else	// VK_RETURN/!VK_RETURN
			(void) fprintf(stdout, "\t};\n");
#endif	// !VK_RETURN
			(void) fprintf(stdout, "# of Selected Face IDs = %d\n", n);
		}
#endif
			}
			if (!state.g__Shift && state.g__Control && state.g__Alt) {	// CTRL + ALT + 8
				state.g__showDefinedMeasure = !state.g__showDefinedMeasure;
				// toggle alpha blending
				DXUTGetD3D9Device()->SetRenderState(D3DRS_ALPHABLENDENABLE, state.g__showDefinedMeasure);
			}
			if (!state.g__Shift && state.g__Control && !state.g__Alt) {	// CTRL + 8
//				state.g__printBarycentricCoordinates = true;
#if 1
				const INTERSECTION*	twoIntersection = state.__twoIntersection;
				int	currentIndex = state.__numIntersection ? (state.__nextIntersectionIndex == 0 ? 1 : 0) : -1;
				int	n;
				if (state.g__incompleteString) {
					if (state.__numIntersection && state.__numIntersection % 2 == 0) {
						currentIndex = -1;
						n = 2;
					} else
						n = 0;
				} else {
					n = state.__numIntersection ? 2 : 0;
				}
				(void) fprintf(stdout, "CTRL + 8: n = %d, currentIndex = %d\n", n, currentIndex);
				for (int i = 0; i < n; ++i) {
					if (currentIndex != -1 && i != currentIndex)
						continue;

					(void) fprintf(stdout,
						"\tPOINT_DEF[%d]: (%d, %ff, %ff)\n"
						"\tN[%d]: (%ff, %ff, %ff)\n"
						,
						i,
						twoIntersection[i].dwFace, twoIntersection[i].fBary1, twoIntersection[i].fBary2,
						i, state.g__n.x, state.g__n.y, state.g__n.z
					);
				}
/*
 * MEASURE_ITEM_DEF 에 필요한 정보 출력
	MEASURE_TYPE_T		__type;
	MEASURE_ITEM_PTR_DEF*	__ptr;
 *
typedef enum {
	MEASURE_TYPE__TWO_POINT_DISTANCE	= 0,	// 두 점 사이의 (최단) 거리
	MEASURE_TYPE__POINT_PLANE_DISTANCE,		// 점과 평면사이의 (최단) 거리
	MEASURE_TYPE__TWO_PLANE_DISTANCE,		// 두 평면 사이의 거리
	MEASURE_TYPE__GIRTH,				// 둘레의 길이
	MEASURE_TYPE__TWO_POINT_GIRTH,			// 곡면 상의 두 점 사이의 (둘레)거리
} MEASURE_TYPE_T;
// MEASURE_TYPE__TWO_POINT_DISTANCE
struct MEASURE_ITEM_PTR_DEF__TWO_POINT_DISTANCE : MEASURE_ITEM_PTR_DEF
// MEASURE_TYPE__POINT_PLANE_DISTANCE
struct MEASURE_ITEM_PTR_DEF__POINT_PLANE_DISTANCE : MEASURE_ITEM_PTR_DEF
// MEASURE_TYPE__TWO_PLANE_DISTANCE
struct MEASURE_ITEM_PTR_DEF__TWO_PLANE_DISTANCE : MEASURE_ITEM_PTR_DEF
// MEASURE_TYPE__GIRTH
struct MEASURE_ITEM_PTR_DEF__GIRTH : MEASURE_ITEM_PTR_DEF
// MEASURE_TYPE__TWO_POINT_GIRTH
struct MEASURE_ITEM_PTR_DEF__TWO_POINT_GIRTH : MEASURE_ITEM_PTR_DEF
 *	둘레
		PLANE_DEF
		TAPE_MEASURE_MODE_CONTOUR(머리, 목, 허리, 좌.우허벅지, 좌.우팔뚝)/TAPE_MEASURE_MODE_WRAP(가슴, 엉덩이)
	길이
	TAPE_MEASURE__HEIGHT		= 1 << 9,	// 키
	TAPE_MEASURE__SEATED_HEIGHT	= 1 << 10,	// 앉은 키
	TAPE_MEASURE__SHOULDER_WIDTH	= 1 << 11,	// 어깨 넓이

	TAPE_MEASURE__LEFT_LEG		= 1 << 12,	// 왼쪽 다리
	TAPE_MEASURE__RIGHT_LEG		= 1 << 13,	// 오른쪽 다리
	TAPE_MEASURE__LEFT_ARM		= 1 << 14,	// 왼쪽 팔
	TAPE_MEASURE__RIGHT_ARM		= 1 << 15,	// 오른쪽 팔

	TAPE_MEASURE__LEFT_ARM_INSIDE	= 1 << 16,	// 왼쪽 팔 (안쪽)
	TAPE_MEASURE__LEFT_ARM_OUTSIDE	= 1 << 17,	// 왼쪽 팔 (바깥쪽)
	TAPE_MEASURE__RIGHT_ARM_INSIDE	= 1 << 18,	// 오른쪽 팔 (안쪽)
	TAPE_MEASURE__RIGHT_ARM_OUTSIDE	= 1 << 19,	// 오른쪽 팔 (바깥쪽)
	TAPE_MEASURE__LEFT_LEG_INSIDE	= 1 << 20,	// 왼쪽 다리 (안쪽)
	TAPE_MEASURE__LEFT_LEG_OUTSIDE	= 1 << 21,	// 왼쪽 다리 (바깥쪽)
	TAPE_MEASURE__RIGHT_LEG_INSIDE	= 1 << 22,	// 오른쪽 다리 (안쪽)
	TAPE_MEASURE__RIGHT_LEG_OUTSIDE	= 1 << 23,	// 오른쪽 다리 (바깥쪽)
	n 개의 MEASURE_ITEM_DEF 를 사용;
struct MEASURE_ITEM : LENGTH_DEF, MEASURE_ITEM_DEF
		두 개 이상일 경우에 현재 구조상 MEASURE_ITEM 을 사용하는 예전 방식(LENGTH_DEF: PLANE_DEF 와 동일)에 따라
		2 * n 개의 MEASURE_ITEM 를 사용
		(홀수번째만 MEASURE_ITEM_DEF 를 사용해 정의해주고 짝수번째는 의미없는 MEASURE_ITEM 으로 설정)
	def.__type = MEASURE_TYPE__TWO_POINT_GIRTH;
	def.__ptr = new MEASURE_ITEM_PTR_DEF__TWO_POINT_GIRTH(TAPE_MEASURE_MODE_WRAP, point0Def, point1Def);
 */
#endif
			}
			if (!state.g__Shift && !state.g__Control && !state.g__Alt) {	// 8
				state.g__follow = true;
			}
			break;
		case '9':
			if (state.g__Shift && state.g__Control && state.g__Alt) {	// SHIFT + CTRL + ALT + 9
#if 0	// 임의계측; 기존 이름 삭제 ("가슴"): SHIFT + CTRL + ALT + 9
				// 이름 삭제
				char	name0[] = "가슴";	// 해당 이름
				TAPE_MEASURE_NAME_T	nameType;
				nameType.old_name = name0;
				nameType.new_name = NULL;	// 삭제
				if (0 == gMesh.setValue(TYPE__TAPE_MEASURE, TAPE_MEASURE__NAME, &nameType))
					(void) fprintf(stdout, "이름 삭제: |%s|: 성공\n", name0);
				else
					(void) fprintf(stdout, "이름 삭제: |%s|: 실패\n", name0);
#endif
			} else if (!state.g__Shift && state.g__Control && state.g__Alt) {	// CTRL + ALT + 9
#if 0	// 임의계측; 읽어오기 (실패: "허리"): CTRL + ALT + 9
				// 정의되지 않은 "이름"으로부터; 에러 발생
				kTapeMeasure::USER_DEFINE_AREA	uda;
				char	name[] = "허리";	// 잘못된(지정하지 않은) 이름
				uda.name = name;
				if (gMesh.getValue(TYPE__TAPE_MEASURE, TAPE_MEASURE__USER, &uda) < 0) {
					(void) fprintf(stderr, "ERROR: name:= |%s|: Failure\n", name);
				} else {
					(void) fprintf(stderr,
						"이름: |%s|, 어떻게: %s, 방법: %s, 단면: %s, 길이: %f, 보여주기: %s\n"
						,
						name,
						uda.mode == TAPE_MEASURE_MODE_CONTOUR ? "곡면" : "줄자",
						uda.method == TAPE_MEASURE_METHOD_GIRTH ? "둘레" : "임의의 두 점",
						uda.crossSectionType == TAPE_MEASURE_CROSS_SECTION_UNDEF ? "없음" : (uda.crossSectionType == TAPE_MEASURE_CROSS_SECTION_2D ? "2D" : "3D"),
						uda.length,
						uda.visible ? "True" : "False"
					);
				}
#endif
			} else if (state.g__Shift && !state.g__Control && state.g__Alt) {	// SHIFT + ALT + 9
#if 0	// 임의계측; 읽어오기 (성공: "가슴"): SHIFT + ALT + 9
				// 제대로된 "이름"으로부터; 성공
				kTapeMeasure::USER_DEFINE_AREA	uda;
				char	name[] = "가슴";	// 제대로된 이름
				uda.name = name;
				if (gMesh.getValue(TYPE__TAPE_MEASURE, TAPE_MEASURE__USER, &uda) < 0) {
					(void) fprintf(stderr, "ERROR: name:= |%s|: Failure\n", name);
				} else {
					(void) fprintf(stderr,
						"이름: |%s|, 어떻게: %s, 방법: %s, 단면: %s, 길이: %f, 보여주기: %s\n"
						,
						name,
						uda.mode == TAPE_MEASURE_MODE_CONTOUR ? "곡면" : "줄자",
						uda.method == TAPE_MEASURE_METHOD_GIRTH ? "둘레" : "임의의 두 점",
						uda.crossSectionType == TAPE_MEASURE_CROSS_SECTION_UNDEF ? "없음" : (uda.crossSectionType == TAPE_MEASURE_CROSS_SECTION_2D ? "2D" : "3D"),
						uda.length,
						uda.visible ? "True" : "False"
					);
				}
#endif
			} else if (state.g__Shift && state.g__Control && !state.g__Alt) {	// SHIFT + CTRL + 9
#if 0	// 임의계측; 이름 변경 ("가슴" -> "허리"): SHIFT + CTRL + 9
				// 이름 변경: "가슴" -> "허리"
				char	name0[] = "가슴";	// 원래 이름
				char	name[] = "허리";	// 새로운 이름
				TAPE_MEASURE_NAME_T	nameType;
				nameType.old_name = name0;
				nameType.new_name = name;	// 새로 등록
				if (0 == gMesh.setValue(TYPE__TAPE_MEASURE, TAPE_MEASURE__NAME, &nameType))
					(void) fprintf(stdout, "이름 변경: |%s| -> |%s|: 성공\n", name0, name);
				else
					(void) fprintf(stdout, "이름 변경: |%s| -> |%s|: 실패\n", name0, name);
#endif
			} else if (!state.g__Shift && !state.g__Control && state.g__Alt) {	// ALT + 9
#if 0	// 임의계측; 보여주기 설정 토글 ("허리"): ALT + 9
				// 보여주기 설정 토글
				char	name[] = "허리";	// 새로운 이름
				TAPE_MEASURE_NAME_VISIBLE	nameVisible;
				nameVisible.name = name;
				if (gMesh.getValue(TYPE__TAPE_MEASURE, TAPE_MEASURE__NAME_VISIBLE, &nameVisible) < 0) {
					(void) fprintf(stderr, "ERROR: name:= |%s| NOT found\n", name);
				} else {
					nameVisible.visible = !nameVisible.visible;	// 토글
					if (0 == gMesh.setValue(TYPE__TAPE_MEASURE, TAPE_MEASURE__NAME_VISIBLE, &nameVisible)) {
						(void) fprintf(stdout, "보여주기: %s -> %s: 성공\n", nameVisible.visible ? "False" : "True", nameVisible.visible ? "True" : "False");
					} else {
						(void) fprintf(stderr, "보여주기: %s -> %s: 실패\n", nameVisible.visible ? "False" : "True", nameVisible.visible ? "True" : "False");
					}
				}
#endif
#if 1	// 시뮬레이션; ALT + 9
				SIMULATION_TYPE	type;
				gMesh.getValue(TYPE__SIMULATION, SIMULATION__METHOD, &type);
#if 1		// 한 번(SIMULATION_EXEC_METHOD_FINAL)에 실행하는 경우에만 "실행"버튼을 흉내내고,
		// 나머지의 경우에는 최종 결과를 확인한다.
				SIMULATION_EXEC_METHOD_VALUE	execMethodVal;
				gMesh.getValue(TYPE__SIMULATION, SIMULATION__EXEC_METHOD, &execMethodVal);
				if (execMethodVal.type == SIMULATION_EXEC_METHOD_FINAL) {	// "실행" 버튼 흉내내기
					// 한 번에 처리한다.
					SIMULATION_EXEC_TYPE	execType = SIMULATION_EXEC_DONE;
					gMesh.setValue(TYPE__SIMULATION, SIMULATION__EXEC, &execType);
				} else {	// 변화된 값을 읽는다.
					float	volumeChange;
					gMesh.getValue(TYPE__SIMULATION, SIMULATION__VOLUME_CHANGE, &volumeChange);
					(void) fprintf(stdout, "Volume Change = %f\n", volumeChange);
				}
#else
				MANUAL_SIMULATION	manual;	// type = SIMULATION_TYPE_MANUAL 인 경우에만 의미가 있다
				gMesh.getValue(TYPE__SIMULATION, SIMULATION__MANUAL, &manual);
				if (type == SIMULATION_TYPE_MANUAL && manual.type == MANUAL_DIRECTLY) {
					// 변화된 값을 읽는다.
					// 변화된 부피는?
					float	volumeChange;
					gMesh.getValue(TYPE__SIMULATION, SIMULATION__VOLUME_CHANGE, &volumeChange);
					(void) fprintf(stdout, "Volume Change = %f\n", volumeChange);
				} else {
					// 한 번에 처리한다.
					SIMULATION_EXEC_TYPE	execType = SIMULATION_EXEC_DONE;
					gMesh.setValue(TYPE__SIMULATION, SIMULATION__EXEC, &execType);
				}
#endif
#endif
			} else if (!state.g__Shift && state.g__Control && !state.g__Alt) {	// CTRL + 9
#if 0	// 영역
				bool	visible = true;
				gMesh.setValue(TYPE__AREA, AREA__TRUNK, &visible);
#endif
#if 0	// 모눈
				GRID_Z		z = GRID_UNDEF;
				state.setValue(TYPE__GRID, GRID__Z_DEPTH, &z);
#endif
#if 1	// 등고선
				ISOMETRIC_TYPE_T	type = ISOMETRIC_UNDEF;
				meshRef.setValue(TYPE__ISOMETRIC, ISOMETRIC__TYPE, &type);
#endif
#if 0	// 모델
				OBJECT_TYPE	objectType = OBJECT_UNDEF;
				meshRef.setValue(TYPE__MODEL, MODEL__OBJECT, &objectType);
#endif
#if 0	// 뷰
				VIEW_T	view;
				view.type = VIEW_PERSPECTIVE;
				view.orthographic = false;
				state.setValue(TYPE__VIEW, VIEW__TYPE, &view);
				bool	reset = true;
				state.setValue(TYPE__VIEW, VIEW__HOME, &reset);
#endif
#if 1	// 시뮬레이션; CTRL + 9
				// 시뮬레이션을 멈춘다;
				// SIMULATION_EXEC_DONE 이 아닌 경우에...
				SIMULATION_EXEC_TYPE	execType = SIMULATION_EXEC_UNDEF;
				gMesh.setValue(TYPE__SIMULATION, SIMULATION__EXEC, &execType);
#endif
#if 0	// 임의계측; 보여주기 설정 토글 ("가슴"): CTRL + 9
				// 보여주기 설정 토글
				char	name[] = "가슴";	// 새로운 이름
				TAPE_MEASURE_NAME_VISIBLE	nameVisible;
				nameVisible.name = name;
				if (gMesh.getValue(TYPE__TAPE_MEASURE, TAPE_MEASURE__NAME_VISIBLE, &nameVisible) < 0) {
					(void) fprintf(stderr, "ERROR: name:= |%s| NOT found\n", name);
				} else {
					nameVisible.visible = !nameVisible.visible;	// 토글
					if (0 == gMesh.setValue(TYPE__TAPE_MEASURE, TAPE_MEASURE__NAME_VISIBLE, &nameVisible)) {
						(void) fprintf(stdout, "보여주기: %s -> %s: 성공\n", nameVisible.visible ? "False" : "True", nameVisible.visible ? "True" : "False");
					} else {
						(void) fprintf(stderr, "보여주기: %s -> %s: 실패\n", nameVisible.visible ? "False" : "True", nameVisible.visible ? "True" : "False");
					}
				}
#endif
			} else if (state.g__Shift && !state.g__Control && !state.g__Alt) {	// SHIFT + 9
#if 0	// 임의계측; 새로운 이름 등록 ("가슴"): SHIFT + 9
				char	name[] = "가슴";	// 새로운 이름
				TAPE_MEASURE_NAME_T	nameType;
				nameType.old_name = NULL;
				nameType.new_name = name;	// 새로 등록
				if (0 == gMesh.setValue(TYPE__TAPE_MEASURE, TAPE_MEASURE__NAME, &nameType)) {
					(void) fprintf(stdout, "이름: |%s|: 등록 성공\n", name);
				} else {
					(void) fprintf(stdout, "이름: |%s|: 등록 실패\n", name);
				}
#endif
#if 0	// 뷰
				VIEW_T	view;
				view.type = VIEW_PERSPECTIVE;
				view.orthographic = true;
				state.setValue(TYPE__VIEW, VIEW__TYPE, &view);
#endif
#if 0	// 모델
				OBJECT_TYPE	objectType = OBJECT_FACE;
				meshRef.setValue(TYPE__MODEL, MODEL__OBJECT, &objectType);
				SHADING_TYPE	shadingType = SHADING_GREY;
				meshRef.setValue(TYPE__MODEL, MODEL__SHADING, &shadingType);
#endif
#if 0	// 길이측정
				bool	visible = true;
				gMesh.setValue(TYPE__TAPE_MEASURE, TAPE_MEASURE__HEIGHT, &visible);
#endif
#if 0	// 모눈
				GRID_Z		z = GRID_BACK;
				state.setValue(TYPE__GRID, GRID__Z_DEPTH, &z);
				GRID_DIR	dir = GRID_BOTH;//GRID_VERTICAL;
				state.setValue(TYPE__GRID, GRID__DIRECTION, &dir);
#endif
#if 1	// 등고선
				ISOMETRIC_TYPE_T	type = ISOMETRIC_LINE;
				meshRef.setValue(TYPE__ISOMETRIC, ISOMETRIC__TYPE, &type);
				ISOMETRIC_PLANE_T	plane = ISOMETRIC_XY;
				meshRef.setValue(TYPE__ISOMETRIC, ISOMETRIC__PLANE, &plane);
#endif
#if 1	// 시뮬레이션; SHIFT + 9
			// 1. 시뮬레이션 방법 설정
#if 1	// 임의조작
			{
				SIMULATION_TYPE	simulationType = SIMULATION_TYPE_MANUAL;
				gMesh.setValue(TYPE__SIMULATION, SIMULATION__METHOD, &simulationType);

				MANUAL_SIMULATION	manual;
#if 1
				manual.type = MANUAL_DIRECTLY;
#else
				manual.type = MANUAL_BY_COLOR_ISOMETRIC;
#endif
				manual.area = AREA_CHEST;
				gMesh.setValue(TYPE__SIMULATION, SIMULATION__MANUAL, &manual);
			}
#else
#if 1	// 템플릿사용
			{
				SIMULATION_TYPE	simulationType = SIMULATION_TYPE_TEMPLATE;
				gMesh.setValue(TYPE__SIMULATION, SIMULATION__METHOD, &simulationType);

				BREAST_IMPLANT_TEMPLATE	implantTemplate;
				implantTemplate.implantType = BREAST_IMPLANT_TYPE_B;
				implantTemplate.positionType = MODEL_POINT_RIGHT_UP;
				gMesh.setValue(TYPE__SIMULATION, SIMULATION__TEMPLATE, &implantTemplate);
			}
#else	// 표준체형
			{
				SIMULATION_TYPE	simulationType = SIMULATION_TYPE_STANDARD;
				gMesh.setValue(TYPE__SIMULATION, SIMULATION__METHOD, &simulationType);

				STANDARD_SIMULATION	standard;
				standard.area = (AREA__TYPE) (AREA__RIGHT_ARM | AREA__LEFT_ARM);
				gMesh.setValue(TYPE__SIMULATION, SIMULATION__STANDARD, &standard);

				// 총 변화량은?
				gMesh.getValue(TYPE__SIMULATION, SIMULATION__STANDARD, &standard);
				(void) fprintf(stdout, "total volume change = %f\n", standard.volume);
			}
#endif
#endif
			// 2. 시뮬레이션 실행 벙법
#if 1	// 시뮬레이션이 일어나는 과정
/*
 *******************************************************************************
 *	2) 성형 방법 설정; 어떻게...(by cc or by normal amount)
 *	2-1) 어떤 식으로 할 것인지: SIMULATION__CC_TYPE
 *	// 가) 정해진 양만큼 변화한다.
 *	// 방법 설정
 *	SIMULATION_CC_TYPE	ccType = SIMULATION_CC_TYPE_FINAL;	// 최종 부피
 *	// 정해진 양 설정;
 *	//	표준체형으로 갔을 때의 총 변화량보다 작거나 클 수 없다;
 *	//	앞의 총 변화량의 절대값보다는 작아야 하고, 부호는 총 변화량과 같아야 한다.
 *	float	volumeChange = 10.f;
 *	setValue(TYPE__SIMULATION, SIMULATION__VOLUME_CHANGE, &float);
 *	// 실행
 *	SIMULATION_EXEC_TYPE	execType = SIMULATION_EXEC_DONE;
 *	setValue(TYPE__SIMULATION, SIMULATION__EXEC, &execType);
 *
 *	// 나) 원하는 만큼 변화를 주면서
 *	// 방법 설정
 *	SIMULATION_CC_TYPE	ccType = SIMULATION_CC_TYPE_VOLUME_CHANGE;
 *	// a) 구체적으로 일정한 양만큼씩 아니면 일정한 비율만큼씩
 *	// 일정한 양만큼씩
 *	SIMULATION_STEP_TYPE	stepType = SIMULATION_STEP_TYPE_CC;
 *	setValue(TYPE__SIMULATION, SIMULATION__STEP_TYPE, &stepType);
 *	float	cc_step_size = .1f;
 *	setValue(TYPE__SIMULATION, SIMULATION__CC_STEP_SIZE, &cc_step_size);
 *	// 실행
 *	SIMULATION_EXEC_TYPE	execType = SIMULATION_EXEC_PLUS_STEP;	// SIMULATION_EXEC_MINUS_STEP
 *	setValue(TYPE__SIMULATION, SIMULATION__EXEC, &execType);
 *	// b) 일정한 비율로
 *	SIMULATION_STEP_TYPE	stepType = SIMULATION_STEP_TYPE_RATIO;
 *	setValue(TYPE__SIMULATION, SIMULATION__STEP_TYPE, &stepType);
 *	float	normal_amount_size = .1;
 *	setValue(TYPE__SIMULATION, SIMULATION__NORMAL_AMOUNT_STEP_SIZE, &normal_amount_size);
 *	// 실행
 *	SIMULATION_EXEC_TYPE	execType = SIMULATION_EXEC_PLUS;	// SIMULATION_EXEC_MINUS
 *	setValue(TYPE__SIMULATION, SIMULATION__EXEC, &execType);
 *
 *	// 변화된 부피는?
 *	float	volumeChange;
 *	getValue(TYPE__SIMULATION, SIMULATION__VOLUME_CHANGE, &volumeChange);
 *******************************************************************************
 */
			{
#if 1	// SIMULATION__EXEC_METHOD 를 사용
				SIMULATION_EXEC_METHOD_VALUE	execMethodVal;
#if 0	// 가) 한 번에: SIMULATION_EXEC_METHOD_FINAL
				execMethodVal.type = SIMULATION_EXEC_METHOD_FINAL;
				execMethodVal.val = .5f;
#else	// 나) 단계적으로
#if 0	//   a) 일정한 양만큼: SIMULATION_EXEC_METHOD_STEP_CC
				execMethodVal.type = SIMULATION_EXEC_METHOD_STEP_CC;
				execMethodVal.val = .01f;
#else	//   b) 일정한 비율로: SIMULATION_EXEC_METHOD_STEP_RATIO
				execMethodVal.type = SIMULATION_EXEC_METHOD_STEP_RATIO;
				execMethodVal.val = .01f * gMesh.a();
#endif
#endif
				gMesh.setValue(TYPE__SIMULATION, SIMULATION__EXEC_METHOD, &execMethodVal);
#else
				STANDARD_SIMULATION	standard;
				gMesh.getValue(TYPE__SIMULATION, SIMULATION__STANDARD, &standard);
				(void) fprintf(stdout, "total volume change = %f\n", standard.volume);
#if 1	// 가) 한번에
				SIMULATION_CC_TYPE	ccType = SIMULATION_CC_TYPE_FINAL;
				gMesh.setValue(TYPE__SIMULATION, SIMULATION__CC_TYPE, &ccType);
				// 변화할 값을 정해준다;
				// 표준체형으로의 변환의 경우,
				// 총 변화량의 절대값보다 작어야 하고, 부호는 총변화량과 같아야 한다.
				float	volumeChange = .5f;// * standard.volume;
				// ALT + 9 를 눌러 한번에 변화시킨다.
				gMesh.setValue(TYPE__SIMULATION, SIMULATION__VOLUME_CHANGE, &volumeChange);
#else	// 나) 단계적으로
				SIMULATION_CC_TYPE	ccType = SIMULATION_CC_TYPE_VOLUME_CHANGE;
				gMesh.setValue(TYPE__SIMULATION, SIMULATION__CC_TYPE, &ccType);
#if 1	// a) 일정한 양만큼
				SIMULATION_STEP_TYPE	stepType = SIMULATION_STEP_TYPE_CC;
				gMesh.setValue(TYPE__SIMULATION, SIMULATION__STEP_TYPE, &stepType);
				float	cc_step_size = .01f;//0.1f * standard.volume;
				gMesh.setValue(TYPE__SIMULATION, SIMULATION__CC_STEP_SIZE, &cc_step_size);
#else	// b) 일정한 비율로
				SIMULATION_STEP_TYPE	stepType = SIMULATION_STEP_TYPE_RATIO;
				gMesh.setValue(TYPE__SIMULATION, SIMULATION__STEP_TYPE, &stepType);
				float	normal_amount_size = .01f * gMesh.a();
				gMesh.setValue(TYPE__SIMULATION, SIMULATION__NORMAL_AMOUNT_STEP_SIZE, &normal_amount_size);
#endif
				// 변화된 부피는?
				float	volumeChange;
				// 아직 시뮬레이션을 실행하지 않았으므로 의미없는 값이 나올 것이다.
				gMesh.getValue(TYPE__SIMULATION, SIMULATION__VOLUME_CHANGE, &volumeChange);
				(void) fprintf(stdout, "Volume Change = %f\n", volumeChange);
#endif
#endif
			}
#endif
#endif
			} else if (!state.g__Shift && !state.g__Control && !state.g__Alt) {	// 9
#if 0	// 길이측정
				float	length;
				gMesh.getValue(TYPE__TAPE_MEASURE, TAPE_MEASURE__HEAD, &length);
				(void) fprintf(stdout, "TAPE_MEASURE__HEAD = %f\n", length);
#endif
#if 0	// 영역
				bool	visible;	// NOT USED
				gMesh.getValue(TYPE__AREA, AREA__TRUNK, &visible);
#endif
#if 0	// 모눈
				GRID_Z		z;
				state.getValue(TYPE__GRID, GRID__Z_DEPTH, &z);
				GRID_DIR	dir;
				state.getValue(TYPE__GRID, GRID__DIRECTION, &dir);
#endif
#if 1	// 등고선
				ISOMETRIC_TYPE_T	type;
				meshRef.getValue(TYPE__ISOMETRIC, ISOMETRIC__TYPE, &type);
				ISOMETRIC_PLANE_T	plane;
				meshRef.getValue(TYPE__ISOMETRIC, ISOMETRIC__PLANE, &plane);
#endif
			} else {
				// 사용안함
			}
			break;
		case 'J':
			__JKeyPressed();
			break;
		case 'K':
			__KKeyPressed();
			break;
#endif
		case VK_F7:	// VK_F7: {
#if 1	// manipulator 를 사용
#if 1	// 설정; F7 (임의의 둘레측정), SHIFT + F7(지정된 둘레측정)
			// 임의의 둘레측정
			if (!state.g__Shift && !state.g__Control && !state.g__Alt) {	// F7
				int	n;
				meshRef.getValue(TYPE__MANIPULATOR, MANIPULATOR__SET, &n);
				if (n < 0) {
					n = 0;
					if (meshRef.setValue(TYPE__MANIPULATOR, MANIPULATOR__SET, &n) == 0)
						state.manipulator_mode = true;
				}
			}
			// 지정된 둘레측정
			if (state.g__Shift && !state.g__Control && !state.g__Alt) {	// SHIFT + F7
				int	n;
				gMesh.getValue(TYPE__MANIPULATOR, MANIPULATOR__SET, &n);
				if (n < 0) {
//					n = TAPE_MEASURE_SEATED_HEIGHT;
//					n = TAPE_MEASURE_BREAST;
					n = TAPE_MEASURE_SHOULDER_WIDTH;
					if (0 == gMesh.setValue(TYPE__MANIPULATOR, MANIPULATOR__SET, &n))
						state.manipulator_mode = true;
				}
			}
#endif
#if 1	// 해제
			if (!state.g__Shift && state.g__Control && !state.g__Alt) {	// CTRL + F7
				MANIPULATOR_TYPE_T	type = UNDEF_MANIPULATOR;
				meshRef.setValue(TYPE__MANIPULATOR, MANIPULATOR__TYPE, &type);
				// alpha blending disabled
				DXUTGetD3D9Device()->SetRenderState(D3DRS_ALPHABLENDENABLE, false);

				state.__showSegment = true;
				state.manipulator_mode = false;
			}
#endif
#if 1	// 각도 회전 유형 변경
			if (!state.g__Shift && !state.g__Control && state.g__Alt) {	// ALT + F7
				MANIPULATOR_NORMAL_ANGLE_ROTATION_T	angle_rotation;
				meshRef.getValue(TYPE__MANIPULATOR, MANIPULATOR__NORMAL_ANGLE_ROTATION, &angle_rotation);
				if (angle_rotation == MANIPULATOR_NORMAL_ANGLE_ROTATION_BOTH_THETA_AND_PHI)
					angle_rotation = MANIPULATOR_NORMAL_ANGLE_ROTATION_THETA_ONLY;
				else
					angle_rotation = MANIPULATOR_NORMAL_ANGLE_ROTATION_BOTH_THETA_AND_PHI;
				meshRef.setValue(TYPE__MANIPULATOR, MANIPULATOR__NORMAL_ANGLE_ROTATION, &angle_rotation);
			}
#endif
#if 1	// manipulator 선택
			if (!state.g__Shift && state.g__Control && state.g__Alt) {	// CTRL + ALT + F7
				// POINT_MANIPULATOR -> NORMAL_MANIPULATOR -> LINE_MANIPULATOR -> POINT_MANIPULATOR
				static MANIPULATOR_TYPE_T	new_type = UNDEF_MANIPULATOR;
				if (new_type == UNDEF_MANIPULATOR)
					new_type = POINT_MANIPULATOR;
				else if (new_type == POINT_MANIPULATOR)
					new_type = NORMAL_MANIPULATOR;
#if 0
				else if (new_type == NORMAL_MANIPULATOR)
					new_type = POINT_MANIPULATOR;
#else
				else if (new_type == NORMAL_MANIPULATOR)
					new_type = LINE_MANIPULATOR;
				else if (new_type == LINE_MANIPULATOR)
					new_type = POINT_MANIPULATOR;
#endif
				// 이전 상태는?
				MANIPULATOR_TYPE_T	old_type;
				meshRef.getValue(TYPE__MANIPULATOR, MANIPULATOR__TYPE, &old_type);
				int	old_n = -1;
				if (old_type != UNDEF_MANIPULATOR) {	// manipulator 가 있었다면
					// 다시 만든다. (하지만 유형은 다르게)
					meshRef.getValue(TYPE__MANIPULATOR, MANIPULATOR__SET, &old_n);
				}
				// manipulator 유형을 바꾼 후,
//				state.__showSegment = new_type != LINE_MANIPULATOR;
				if (meshRef.setValue(TYPE__MANIPULATOR, MANIPULATOR__TYPE, &new_type) == 0) {
					(void) fprintf(stdout, "%s\n", new_type == POINT_MANIPULATOR ? "POINT_MANIPULATOR" : (new_type == NORMAL_MANIPULATOR ? "NORMAL_MANIPULATOR" : "LINE_MANIPULATOR"));
					// 이전 상태가 조작툴 모드였다면
					if (old_n != -1) {	// manipulator 가 있었다면
						// 원래대로 되돌린다.
						meshRef.setValue(TYPE__MANIPULATOR, MANIPULATOR__SET, &old_n);
					}
				}
			}
#endif
#if 1	// line manipulator
			if (state.g__Shift && state.g__Control && !state.g__Alt) {	// SHIFT + CTRL + F7
				// line manipulator 가 조절하는 선분을 보기 위해
				// 일시적으로 계측선분을 보이지 않게 한다.
				state.__showSegment = !state.__showSegment;
			}
#endif
#if 1	// manipulator 를 조정하는 화면모드
			if (state.g__Shift && state.g__Control && state.g__Alt) {	// SHIFT + CTRL + ALT + F7
				state.manipulator_mode = !state.manipulator_mode;
			}
#endif
#else
			if (state.g__bString) {
				if (state.g__Control) {
					state.g__stringHeading = true;
					state.g__stringAttitude = false;
				} else {
					state.g__stringHeading = false;
					state.g__stringAttitude = true;
				}
			}
#endif
			break;	// VK_F7: }
		case VK_F8:	// VK_F8: {
#ifdef USE__fileOpen
#ifdef STANDARD_MODEL
#if 0	// logo
			if (!state.g__Shift && !state.g__Control && !state.g__Alt) {	// F8;
											// 에트리[1] 측정모델(2) 설정;
											// 1:= 에트리 표준모델 인덱스
				cout << "F8: 로고" << endl;

				// 파라미터 설정
				kMesh::SetAllArg	arg;
#ifdef STANDARD_MODEL
				arg.index_to_standard	= 5;
#endif	// STANDARD_MODEL
				arg.statePtr		= &state;
				arg.xSourceFilePtr	= L"Media\\Scanner\\tiny.x";
				arg.devicePtr		= DXUTGetD3D9Device();	// OnCreateDevice 에서 새로운 값으로 덮어써야 한다.
				arg.effectSourceFilePtr	= L"../include/mesh/mesh.fx";
				arg.fScale 		= 1413.793f/.525f;//1.f;
#ifdef __READ_FILES__
				arg.stillRead		= true;
#endif	// __READ_FILES__
//				arg.objFilePtr		= "Media\\Scanner\\logo.obj";	// 에트리 측정모델(2)
				arg.objFilePtr		= "Media\\Scanner\\여자모델_Vic090609\\last0.OBJ";	// 에트리 표준모델
				arg.texturePath		= NULL;
				arg.hWnd		= DXUTGetHWND();

				// 로드
				CONNECTABLE_TYPE_T	connectable = CONNECTABLE_YES;
				gPickVar.load(MODEL_TYPE__REAL, arg, connectable);

				// 카메라 설정
				SetCameraArcBall();
			}
#else	// logo/!logo
#if 1	// MakeVertexSmooth
#if 1	// 남
			if (!state.g__Shift && !state.g__Control && !state.g__Alt) {	// F8;
											// 에트리[1] 측정모델(2) 설정;
											// 1:= 에트리 표준모델 인덱스
				cout << "F8: 에트리[1] 측정모델(2) 설정" << endl;

				// 파라미터 설정
				kMesh::SetAllArg	arg;
#ifdef STANDARD_MODEL
				arg.index_to_standard	= 4;	// 1:= 에트리 표준모델 인덱스
#endif	// STANDARD_MODEL
				arg.statePtr		= &state;
				arg.xSourceFilePtr	= L"Media\\Scanner\\tiny.x";
				arg.devicePtr		= DXUTGetD3D9Device();	// OnCreateDevice 에서 새로운 값으로 덮어써야 한다.
				arg.effectSourceFilePtr	= L"../include/mesh/mesh.fx";
				arg.fScale 		= 1.f / 1.2f;
#ifdef __READ_FILES__
				arg.stillRead		= true;
#endif	// __READ_FILES__
				arg.objFilePtr		= "Media\\Scanner\\여자모델_Vic090609\\male06.obj";	// 에트리 측정모델(2)
				arg.texturePath		= NULL;//L"Media\\Scanner\\여자모델_Vic090609\\tex\\";	// 텍스쳐 사용
				arg.hWnd		= DXUTGetHWND();

				// 로드
				gPickVar.load(MODEL_TYPE__REAL, arg);

				// 카메라 설정
				SetCameraArcBall();
			}
#else	// 여
			if (!state.g__Shift && !state.g__Control && !state.g__Alt) {	// F8;
											// 에트리[1] 측정모델(2) 설정;
											// 1:= 에트리 표준모델 인덱스
				cout << "F8: 에트리[1] 측정모델(2) 설정" << endl;

				// 파라미터 설정
				kMesh::SetAllArg	arg;
#ifdef STANDARD_MODEL
				arg.index_to_standard	= 3;	// 1:= 에트리 표준모델 인덱스
#endif	// STANDARD_MODEL
				arg.statePtr		= &state;
				arg.xSourceFilePtr	= L"Media\\Scanner\\tiny.x";
				arg.devicePtr		= DXUTGetD3D9Device();	// OnCreateDevice 에서 새로운 값으로 덮어써야 한다.
				arg.effectSourceFilePtr	= L"../include/mesh/mesh.fx";
				arg.fScale 		= 1.f / 1.2f;
#ifdef __READ_FILES__
				arg.stillRead		= true;
#endif	// __READ_FILES__
				arg.objFilePtr		= "Media\\Scanner\\여자모델_Vic090609\\fem_fat1.obj";	// 에트리 측정모델(2)
				arg.texturePath		= NULL;//L"Media\\Scanner\\여자모델_Vic090609\\tex\\";	// 텍스쳐 사용
				arg.hWnd		= DXUTGetHWND();

				// 로드
				gPickVar.load(MODEL_TYPE__REAL, arg);

				// 카메라 설정
				SetCameraArcBall();
			}
#endif
#else
			if (!state.g__Shift && !state.g__Control && !state.g__Alt) {	// F8;
											// 에트리[1] 측정모델(2) 설정;
											// 1:= 에트리 표준모델 인덱스
				cout << "F8: 에트리[1] 측정모델(2) 설정" << endl;

				// 파라미터 설정
				kMesh::SetAllArg	arg;
#ifdef STANDARD_MODEL
				arg.index_to_standard	= 1;	// 1:= 에트리 표준모델 인덱스
#endif	// STANDARD_MODEL
				arg.statePtr		= &state;
				arg.xSourceFilePtr	= L"Media\\Scanner\\tiny.x";
				arg.devicePtr		= DXUTGetD3D9Device();	// OnCreateDevice 에서 새로운 값으로 덮어써야 한다.
				arg.effectSourceFilePtr	= L"../include/mesh/mesh.fx";
				arg.fScale 		= 1.f / 1.2f;
#ifdef __READ_FILES__
				arg.stillRead		= true;
#endif	// __READ_FILES__
				arg.objFilePtr		= "Media\\Scanner\\여자모델_Vic090609\\EW_2RH.obj";	// 에트리 측정모델(2)
				arg.texturePath		= L"Media\\Scanner\\여자모델_Vic090609\\tex\\";	// 텍스쳐 사용
				arg.hWnd		= DXUTGetHWND();

				// 로드
				gPickVar.load(MODEL_TYPE__REAL, arg);

				// 카메라 설정
				SetCameraArcBall();
			}
#endif
#endif	// !logo
#else	// STANDARD_MODEL/!STANDARD_MODEL
#if 0	// logo
			if (!state.g__Shift && !state.g__Control && !state.g__Alt) {	// F8;
											// 에트리[1] 측정모델(2) 설정;
											// 1:= 에트리 표준모델 인덱스
				cout << "F8: 로고" << endl;

				// 파라미터 설정
				kMesh::SetAllArg	arg;
#ifdef STANDARD_MODEL
				arg.index_to_standard	= -1;
#endif	// STANDARD_MODEL
				arg.statePtr		= &state;
				arg.xSourceFilePtr	= L"Media\\Scanner\\tiny.x";
				arg.devicePtr		= DXUTGetD3D9Device();	// OnCreateDevice 에서 새로운 값으로 덮어써야 한다.
				arg.effectSourceFilePtr	= L"../include/mesh/mesh.fx";
				arg.fScale 		= 1.f;
#ifdef __READ_FILES__
				arg.stillRead		= true;
#endif	// __READ_FILES__
				arg.objFilePtr		= "Media\\Scanner\\logo.obj";	// 에트리 측정모델(2)
				arg.texturePath		= NULL;
				arg.hWnd		= DXUTGetHWND();

				// 로드
				CONNECTABLE_TYPE_T	connectable = CONNECTABLE_LOGO;
				gPickVar.load(MODEL_TYPE__REAL, arg, connectable);

				// 카메라 설정
				SetCameraArcBall();
			}
#else	// logo/!logo
			if (!state.g__Shift && !state.g__Control && !state.g__Alt) {	// F8; 테스트모델 설정
				cout << "F8: 테스트모델 설정" << endl;

				// 파라미터 설정
				kMesh::SetAllArg	arg;
				arg.statePtr		= &state;
				arg.xSourceFilePtr	= L"Media\\Scanner\\tiny.x";
				arg.devicePtr		= DXUTGetD3D9Device();	// OnCreateDevice 에서 새로운 값으로 덮어써야 한다.
				arg.effectSourceFilePtr	= L"../include/mesh/mesh.fx";
				arg.fScale 		= 10.f;
				arg.objFilePtr		= NULL;
				arg.texturePath		= NULL;
				arg.hWnd		= DXUTGetHWND();

				// 로드; 테스트모델은 표준모델 사용하지 않음
				CONNECTABLE_TYPE_T	connectable = CONNECTABLE_NO;
				gPickVar.load(MODEL_TYPE__REAL, arg, connectable);

				// 카메라 설정
				SetCameraArcBall();
			}
#endif	// !logo
#endif	// !STANDARD_MODEL
			if (!state.g__Shift && !state.g__Control && state.g__Alt) {	// ALT + F8;
											// DAZ[0] 측정모델(1) 설정;
											// 0:= DAZ 표준모델 인덱스
				cout << "ALT + F8: DAZ[0] 측정모델(1) 설정" << endl;

				// 파라미터 설정
				kMesh::SetAllArg	arg;
#ifdef STANDARD_MODEL
				arg.index_to_standard	= 0;	// 0:= DAZ 표준모델 인덱스
#endif	// STANDARD_MODEL
				arg.statePtr		= &state;
				arg.xSourceFilePtr	= L"Media\\Scanner\\tiny.x";
				arg.devicePtr		= DXUTGetD3D9Device();	// OnCreateDevice 에서 새로운 값으로 덮어써야 한다.
				arg.effectSourceFilePtr	= L"../include/mesh/mesh.fx";
				arg.fScale 		= 10.f / .8f;
#ifdef __READ_FILES__
				arg.stillRead		= true;
#endif	// __READ_FILES__
				arg.objFilePtr		= "Media\\Scanner\\DW_1RH.obj";	// DAZ 측정모델(1)
				arg.texturePath		= NULL;	// 텍스쳐 사용 안함
				arg.hWnd		= DXUTGetHWND();

				// 로드
				gPickVar.load(MODEL_TYPE__REAL, arg);

				// 카메라 설정
				SetCameraArcBall();
			}
			if (state.g__Shift && !state.g__Control && !state.g__Alt) {	// SHIFT + F8;
											// 에트리[1] 측정모델(1) 설정;
											// 1:= 에트리 표준모델 인덱스
				cout << "SHIFT + F8: 에트리[1] 측정모델(1) 설정" << endl;

				// 파라미터 설정
				kMesh::SetAllArg	arg;
#ifdef STANDARD_MODEL
				arg.index_to_standard	= 1;	// 1:= 에트리 표준모델 인덱스
#endif	// STANDARD_MODEL
				arg.statePtr		= &state;
				arg.xSourceFilePtr	= L"Media\\Scanner\\tiny.x";
				arg.devicePtr		= DXUTGetD3D9Device();	// OnCreateDevice 에서 새로운 값으로 덮어써야 한다.
				arg.effectSourceFilePtr	= L"../include/mesh/mesh.fx";
				arg.fScale 		= 1.f / .8f;
#ifdef __READ_FILES__
				arg.stillRead		= true;
#endif	// __READ_FILES__
				arg.objFilePtr		= "Media\\Scanner\\여자모델_Vic090609\\EW_1RH.obj";	// 에트리 측정모델(1)
				arg.texturePath		= L"Media\\Scanner\\여자모델_Vic090609\\tex\\";	// 텍스쳐 사용
				arg.hWnd		= DXUTGetHWND();

				// 로드
				gPickVar.load(MODEL_TYPE__REAL, arg);

				// 카메라 설정
				SetCameraArcBall();
			}
			if (state.g__Shift && state.g__Control && state.g__Alt) {	// SHIFT + CTRL + ALT + F8;
											// DAZ[0] 표준모델 설정;
											// 0:= DAZ 표준모델 인덱스
				cout << "SHIFT + CTRL + ALT + F8: DAZ[0] 표준모델 설정" << endl;

				// 파라미터 설정
				kMesh::SetAllArg	arg0;
#ifdef STANDARD_MODEL
				arg0.index_to_standard	= 0;	// 0:= 에트리 표준모델 인덱스
#endif	// STANDARD_MODEL
				arg0.statePtr		= &state;
				arg0.xSourceFilePtr	= L"Media\\Scanner\\tiny.x";
				arg0.devicePtr		= DXUTGetD3D9Device();	// OnCreateDevice 에서 새로운 값으로 덮어써야 한다.
				arg0.effectSourceFilePtr	= L"../include/mesh/mesh.fx";
				arg0.fScale 		= 10.f;
#ifdef __READ_FILES__
				arg0.stillRead		= false;
#endif	// __READ_FILES__
				arg0.objFilePtr		= "Media\\Scanner\\DW_0RH.obj";	// DAZ 표준모델
				arg0.texturePath	= NULL;	// 텍스쳐 사용 안함
				arg0.hWnd		= DXUTGetHWND();

				// 로드
				gPickVar.load(MODEL_TYPE__STANDARD, arg0);

				// 카메라 설정은 측정모델 기준
//				SetCameraArcBall();
			}
			if (state.g__Shift && !state.g__Control && state.g__Alt) {	// SHIFT + ALT + F8;
											// 에트리[1] 표준모델 설정;
											// 1:= 에트리 표준모델 인덱스
#if 1
				cout << "SHIFT + ALT + F8: 에트리[1] 표준모델 설정" << endl;

				// 파라미터 설정
				kMesh::SetAllArg	arg0;
#ifdef STANDARD_MODEL
				arg0.index_to_standard	= 5;	// 1:= 에트리 표준모델 인덱스
#endif	// STANDARD_MODEL
				arg0.statePtr		= &state;
				arg0.xSourceFilePtr	= L"Media\\Scanner\\tiny.x";
				arg0.devicePtr		= DXUTGetD3D9Device();	// OnCreateDevice 에서 새로운 값으로 덮어써야 한다.
				arg0.effectSourceFilePtr	= L"../include/mesh/mesh.fx";
				arg0.fScale 		= 1413.793f/.525f;//1.f;
#ifdef __READ_FILES__
				arg0.stillRead		= false;
#endif	// __READ_FILES__
				arg0.objFilePtr		= "Media\\Scanner\\여자모델_Vic090609\\last0.obj";	// 에트리 표준모델
				arg0.texturePath	= NULL;
				arg0.hWnd		= DXUTGetHWND();

				// 로드
				gPickVar.load(MODEL_TYPE__STANDARD, arg0);

				// 카메라 설정은 측정모델 기준
//				SetCameraArcBall();
#else
				cout << "SHIFT + ALT + F8: 에트리[1] 표준모델 설정" << endl;

				// 파라미터 설정
				kMesh::SetAllArg	arg0;
#ifdef STANDARD_MODEL
				arg0.index_to_standard	= 1;	// 1:= 에트리 표준모델 인덱스
#endif	// STANDARD_MODEL
				arg0.statePtr		= &state;
				arg0.xSourceFilePtr	= L"Media\\Scanner\\tiny.x";
				arg0.devicePtr		= DXUTGetD3D9Device();	// OnCreateDevice 에서 새로운 값으로 덮어써야 한다.
				arg0.effectSourceFilePtr	= L"../include/mesh/mesh.fx";
				arg0.fScale 		= 1.f;
#ifdef __READ_FILES__
				arg0.stillRead		= false;
#endif	// __READ_FILES__
				arg0.objFilePtr		= "Media\\Scanner\\여자모델_Vic090609\\EW_0RH.obj";	// 에트리 표준모델
				arg0.texturePath	= L"Media\\Scanner\\여자모델_Vic090609\\tex\\";	// 텍스쳐 사용
				arg0.hWnd		= DXUTGetHWND();

				// 로드
				gPickVar.load(MODEL_TYPE__STANDARD, arg0);

				// 카메라 설정은 측정모델 기준
//				SetCameraArcBall();
#endif
			}
			if (!state.g__Shift && state.g__Control && !state.g__Alt) {	// CTRL + F8;
											// 테스트모델/측정모델 해제
				cout << "CTRL + F8: 테스트모델/측정모델 해제" << endl;

				// 언로드
				gPickVar.unload(MODEL_TYPE__REAL);
			}
			if (!state.g__Shift && state.g__Control && state.g__Alt) {	// CTRL + ALT + F8;
											// 표준모델 해제
				cout << "CTRL + ALT + F8: 표준모델 해제" << endl;

				// 언로드
				gPickVar.unload(MODEL_TYPE__STANDARD);
			}
#endif	// USE__fileOpen
			break;	// VK_F8: }
		case VK_F9:
#if 1
#pragma warning(disable: 4127)
			if (0) {
			} else if (state.g__Shift && state.g__Control && state.g__Alt) {
				meshRef.MakeVertexSmooth();
			} else if (state.g__Shift && state.g__Control && !state.g__Alt) {
			} else if (state.g__Shift && !state.g__Control && state.g__Alt) {
			} else if (state.g__Shift && !state.g__Control && !state.g__Alt) {
			} else if (!state.g__Shift && state.g__Control && state.g__Alt) {
			} else if (!state.g__Shift && state.g__Control && !state.g__Alt) {
				state.g__plus = false;
				state.g__minus = true;
			} else if (!state.g__Shift && !state.g__Control && state.g__Alt) {
			} else if (!state.g__Shift && !state.g__Control && !state.g__Alt) {
				state.g__plus = true;
				state.g__minus = false;
			} else {
			}
#pragma warning(default: 4127)
#else
			if (state.g__Control) {
				state.g__plus = false;
				state.g__minus = true;
			} else {
				state.g__plus = true;
				state.g__minus = false;
			}
#endif
			break;
		case VK_F10:
#if 1
			if (!state.g__Alt && !state.g__Control) {	// F10; kBrush::Radius
				if (state.g__Shift)
					state.brush.Radius *= .95f;	// -5%
				else
					state.brush.Radius *= 1.05f;	// +5%
			}
			if (state.g__Alt && !state.g__Control) {	// ALT + F10; kState::scale_alpha
				if (state.g__Shift)
					state.scale_alpha *= .95f;	// -5%
				else
					state.scale_alpha *= 1.05f;	// +5%
			}
			if (state.g__Alt && state.g__Control) {	// CTRL + F10; kBrush::Intensity
				if (state.g__Shift)
					state.brush.Intensity *= .95f;	// -5%
				else
					state.brush.Intensity *= 1.05f;	// +5%
			}
			if (!state.g__Alt && state.g__Control) {		// CTRL + ALT + F10; kBrush::SmoothLevel
				if (state.g__Shift)
					--state.brush.flag.SmoothLevel;
				else
					++state.brush.flag.SmoothLevel;
				if (state.brush.flag.SmoothLevel < 0)
					state.brush.flag.SmoothLevel = 0;
				if (state.brush.flag.SmoothLevel > 3)
					state.brush.flag.SmoothLevel = 3;
			}
#else
			if (!state.g__Alt) {
				if (state.g__Control) {
					if (state.g__Shift) {
						state.brush.Radius *= 0.9f;
					} else {
						state.brush.Radius *= 0.99f;
					}
				} else {
					if (state.g__Shift) {
						state.brush.Radius *= 1.1f;
					} else {
						state.brush.Radius *= 1.01f;
					}
				}
			} else {
				if (state.g__Control) {
					if (state.g__Shift) {
						state.brush.Intensity *= 0.9f;
					} else {
						state.brush.Intensity *= 0.99f;
					}
				} else {
					if (state.g__Shift) {
						state.brush.Intensity *= 1.1f;
					} else {
						state.brush.Intensity *= 1.01f;
					}
				}
			}
#endif
			break;
		case VK_F11:
			if (state.g__Shift && state.g__Control && state.g__Alt) {
				static D3DSHADEMODE	shading_mode = D3DSHADE_GOURAUD;
				shading_mode = shading_mode == D3DSHADE_FLAT ? D3DSHADE_GOURAUD : D3DSHADE_FLAT;
				(void) DXUTGetD3D9Device()->SetRenderState(D3DRS_SHADEMODE, shading_mode);
				break;
			}
#if 1	// kMesh::history 에서 ALT 키는 사용하지 않으므로,
	// ALT 키를 사용...
			if (!state.g__Shift && state.g__Control && state.g__Alt) {
				state.doMeasure = !state.doMeasure;
				break;
			}
			if (state.g__Shift && !state.g__Control && state.g__Alt) {
				state.SetGridCenter = !state.SetGridCenter;
				break;
			}
			if (!state.g__Shift && !state.g__Control && state.g__Alt) {
				state.DoShift = !state.DoShift;
				break;
			}
#endif
			gMesh.history();
			break;
		case VK_F12:
#if 1
		__F12KeyPressed();
#else
			toggleBrushMode();
			if (state.brush.On) {
				if (state.g__Shift && state.g__Control && state.g__Alt) {
					state.brush_do = true;
					state.brush.Type = kBrush::BRUSH_STANDARD;
				} else if (state.g__Shift && state.g__Control && !state.g__Alt) {
				} else if (state.g__Shift && !state.g__Control && state.g__Alt) {
				} else if (state.g__Shift && !state.g__Control && !state.g__Alt) {
				} else if (!state.g__Shift && state.g__Control && state.g__Alt) {
					state.brush.Type = kBrush::BRUSH_FLATTEN;
				} else if (!state.g__Shift && state.g__Control && !state.g__Alt) {
				} else if (!state.g__Shift && !state.g__Control && state.g__Alt) {
				} else if (!state.g__Shift && !state.g__Control && !state.g__Alt) {
//					meshRef.elongate();
				}
			} else {
				state.brush.Type = kBrush::BRUSH_NONE;
				state.intersection_ptr = NULL;
				state.brush_do = false;
				state.debugging_prReprID_array.resize(0);
				state.debugging_FaceID_array.resize(0);
				meshRef.select().SetNumberOfPoints(0);
			}
#endif
			break;
		case VK_F1:
			g_bShowHelp = !g_bShowHelp;
			break;
		case VK_SHIFT:
			state.g__Shift = true;
			break;
		case VK_CONTROL:
			state.g__Control = true;
			break;
		case VK_F6:	// VK_F6: {
#if 1	// kModel
			if (state.g__Shift && state.g__Control && state.g__Alt) {	// SHIFT + CTRL + ALT + F6
#if 0	// group
				state.g__showGroup = !state.g__showGroup;
#endif
//				state.g__show_all_convex_points = !state.g__show_all_convex_points;
				OBJECT_TYPE	objectType;
				meshRef.getValue(TYPE__MODEL, MODEL__OBJECT, &objectType);
				int	iObjectType = (int) objectType;
				iObjectType ^= OBJECT_VOXEL;
				objectType = (OBJECT_TYPE) iObjectType;
				meshRef.setValue(TYPE__MODEL, MODEL__OBJECT, &objectType);
			}
			if (state.g__Shift && state.g__Control && !state.g__Alt) {	// SHIFT + CTRL + F6
				state.g__bString = !state.g__bString;
				if (state.brush.flag.On)
					state.brush.flag.On = false;
				if (state.g__bString && state.g__incompleteString)
					state.g__incompleteString = false;
				TAPE_MEASURE_METHOD_T	method = state.g__bString ? TAPE_MEASURE_METHOD_GIRTH : TAPE_MEASURE_METHOD_UNDEF;
				meshRef.setValue(TYPE__TAPE_MEASURE, TAPE_MEASURE__METHOD, &method);
#if 1	// 6. 3D 단면
				if (!state.g__bString)
					meshRef.Set3DCrossSection(false);
#endif	// 6. 3D 단면
			}
			if (state.g__Shift && !state.g__Control && state.g__Alt) {	// SHIFT + ALT + F6
				if (state.g__mode == TAPE_MEASURE_MODE_CONTOUR) {
					state.g__mode = TAPE_MEASURE_MODE_WRAP;
					state.g__stringMeasure = true;	// 강제로 다시 측정하도록...
				}
				else {
					state.g__mode = TAPE_MEASURE_MODE_CONTOUR;
					state.g__stringMeasure = true;	// 강제로 다시 측정하도록...
				}
				meshRef.setValue(TYPE__TAPE_MEASURE, TAPE_MEASURE__MODE, &state.g__mode);
			}
			if (!state.g__Shift && state.g__Control && state.g__Alt) {	// CTRL + ALT + F6
				state.g__incompleteString = !state.g__incompleteString;
				if (state.g__incompleteString && state.g__bString) {
					state.g__bString = false;
#if 1	// 6. 3D 단면
					if (state.g__showLowerFace) {
						meshRef.Set3DCrossSection(false);
						state.g__showLowerFace = false;
					}
#endif	// 6. 3D 단면
				}
				TAPE_MEASURE_METHOD_T	method = state.g__incompleteString ? TAPE_MEASURE_METHOD_TWO_POINT : TAPE_MEASURE_METHOD_UNDEF;
				meshRef.setValue(TYPE__TAPE_MEASURE, TAPE_MEASURE__METHOD, &method);
			}
			if (state.g__Shift && !state.g__Control && !state.g__Alt) {	// SHIFT + F6
				OBJECT_TYPE	objectType;
				meshRef.getValue(TYPE__MODEL, MODEL__OBJECT, &objectType);
				int	iObjectType = (int) objectType;
				iObjectType ^= OBJECT_EDGE;
				objectType = (OBJECT_TYPE) iObjectType;
				meshRef.setValue(TYPE__MODEL, MODEL__OBJECT, &objectType);
			}
			if (!state.g__Shift && state.g__Control && !state.g__Alt) {	// CTRL + F6
				OBJECT_TYPE	objectType;
				meshRef.getValue(TYPE__MODEL, MODEL__OBJECT, &objectType);
				int	iObjectType = (int) objectType;
				iObjectType ^= OBJECT_FACE;
				objectType = (OBJECT_TYPE) iObjectType;
				meshRef.setValue(TYPE__MODEL, MODEL__OBJECT, &objectType);
			}
			if (!state.g__Shift && !state.g__Control && state.g__Alt) {	// ALT + F6
				OBJECT_TYPE	objectType;
				meshRef.getValue(TYPE__MODEL, MODEL__OBJECT, &objectType);
				int	iObjectType = (int) objectType;
				iObjectType ^= OBJECT_VERTEX;
				objectType = (OBJECT_TYPE) iObjectType;
				meshRef.setValue(TYPE__MODEL, MODEL__OBJECT, &objectType);
			}
			if (!state.g__Shift && !state.g__Control && !state.g__Alt) {	// F6
				OBJECT_TYPE	objectType;
				meshRef.getValue(TYPE__MODEL, MODEL__OBJECT, &objectType);
				int	iObjectType = (int) objectType;
				iObjectType ^= OBJECT_NORMAL;
				objectType = (OBJECT_TYPE) iObjectType;
				meshRef.setValue(TYPE__MODEL, MODEL__OBJECT, &objectType);
			}
#endif
			break;	// VK_F6: }
		}
	} else {
		switch (nChar) {
#if 0	// manipulator 를 사용
		case VK_F7:
			if (state.g__bString) {
				state.g__stringAttitude = false;
				state.g__stringHeading = false;
			}
			break;
		case VK_F8:
			if (state.g__bString) {
				if (state.g__Control) {
					state.g__stringContinueAtLastSegment = false;
				} else {
					state.g__stringMoveO = false;
				}
			}
			break;
#endif
		case VK_F9:
			state.g__plus = false;
			state.g__minus = false;
			break;
		case VK_SHIFT:
			state.g__Shift = false;
			break;
		case VK_CONTROL:
			state.g__Control = false;
			break;
#if 1	// VK_RETURN
		case VK_RETURN:
			(void) fprintf(stdout, "VK_RETURN: 설정\n");
			state.message.keyPressed = true;
			break;
#endif	// VK_RETURN
		}
	}
}


//--------------------------------------------------------------------------------------
// Handles the GUI events
//--------------------------------------------------------------------------------------
void	CALLBACK OnGUIEvent(UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext)
{
#ifdef USE__pointer_to_PickVar
	if (!gPickVarPtr)
		return;
	PickVar&	gPickVar = *gPickVarPtr;
#endif	// USE__pointer_to_PickVar
#ifdef USE__PickVar
	bool&	g_bUseD3DXIntersect = gPickVar.g_bUseD3DXIntersect;
	bool&	g_bAllHits = gPickVar.g_bAllHits;
	CD3DSettingsDlg&	g_SettingsDlg = gPickVar.g_SettingsDlg;
#endif	// USE__PickVar
	cout << "OnGUIEvent" << endl;
    switch( nControlID )
    {
        case IDC_TOGGLEFULLSCREEN: DXUTToggleFullScreen(); break;
        case IDC_TOGGLEREF:        DXUTToggleREF(); break;
        case IDC_CHANGEDEVICE:     g_SettingsDlg.SetActive( !g_SettingsDlg.IsActive() ); break;
        case IDC_USED3DX:          g_bUseD3DXIntersect = !g_bUseD3DXIntersect; break;
        case IDC_ALLHITS:          g_bAllHits = !g_bAllHits; break;
    }
}


//--------------------------------------------------------------------------------------
// This callback function will be called immediately after the Direct3D device has
// entered a lost state and before IDirect3DDevice9::Reset is called. Resources created
// in the OnResetDevice callback should be released here, which generally includes all
// D3DPOOL_DEFAULT resources. See the "Lost Devices" section of the documentation for
// information about lost devices.
//--------------------------------------------------------------------------------------
void	CALLBACK OnLostDevice(void* pUserContext)
{
#ifdef USE__pointer_to_PickVar
	if (!gPickVarPtr)
		return;
	PickVar&	gPickVar = *gPickVarPtr;
#endif	// USE__pointer_to_PickVar
#ifdef USE__PickVar
#ifdef USE__pointer_to_kMesh
	kMesh*&	gkMeshPtr = gPickVar.gkMeshPtr;
#else	// USE__pointer_to_kMesh/!USE__pointer_to_kMesh
	kMesh&		gMesh = gPickVar.gMesh;
#endif	// !USE__pointer_to_kMesh
	ID3DXFont*&	g_pFont = gPickVar.g_pFont;
	ID3DXSprite*&	g_pTextSprite = gPickVar.g_pTextSprite;
	ID3DXEffect*&	g_pEffect = gPickVar.g_pEffect;
	CDXUTDialogResourceManager&	g_DialogResourceManager = gPickVar.g_DialogResourceManager;
	CD3DSettingsDlg&	g_SettingsDlg = gPickVar.g_SettingsDlg;
#endif	// USE__PickVar
	cout << "OnLostDevice" << endl;
	g_DialogResourceManager.OnD3D9LostDevice();
	g_SettingsDlg.OnD3D9LostDevice();
	if (g_pFont)
		g_pFont->OnLostDevice();
	if (g_pEffect)
		g_pEffect->OnLostDevice();
	SAFE_RELEASE(g_pTextSprite);
#ifdef USE__pointer_to_kMesh
	if (!gkMeshPtr)
		return;
	kMesh&	gMesh = *gkMeshPtr;
#endif	// USE__pointer_to_kMesh
	gMesh.OnLostDevice();
}


//--------------------------------------------------------------------------------------
// This callback function will be called immediately after the Direct3D device has
// been destroyed, which generally happens as a result of application termination or
// windowed/full screen toggles. Resources created in the OnCreateDevice callback
// should be released here, which generally includes all D3DPOOL_MANAGED resources.
//--------------------------------------------------------------------------------------
void	CALLBACK OnDestroyDevice(void* pUserContext)
{
#ifdef USE__pointer_to_PickVar
	if (!gPickVarPtr)
		return;
	PickVar&	gPickVar = *gPickVarPtr;
#endif	// USE__pointer_to_PickVar
#ifdef USE__PickVar
#ifdef USE__pointer_to_kMesh
	kMesh*&	gkMeshPtr = gPickVar.gkMeshPtr;
	kMesh*&	gkMeshPtr0 = gPickVar.gkMeshPtr0;
#else	// USE__pointer_to_kMesh/!USE__pointer_to_kMesh
	kMesh&	gMesh = gPickVar.gMesh;
	kMesh&	gMesh0 = gPickVar.gMesh0;
#endif	// !USE__pointer_to_kMesh
	ID3DXFont*&	g_pFont = gPickVar.g_pFont;
	ID3DXEffect*&	g_pEffect = gPickVar.g_pEffect;
	CDXUTDialogResourceManager&	g_DialogResourceManager = gPickVar.g_DialogResourceManager;
	CD3DSettingsDlg&	g_SettingsDlg = gPickVar.g_SettingsDlg;
#endif	// USE__PickVar
	cout << "OnDestroyDevice" << endl;
	g_DialogResourceManager.OnD3D9DestroyDevice();
	g_SettingsDlg.OnD3D9DestroyDevice();
	SAFE_RELEASE(g_pEffect);
	SAFE_RELEASE(g_pFont);
#ifdef USE__pointer_to_kMesh
	kMesh&	gMesh = *gkMeshPtr;
	if (gkMeshPtr) {
#endif	// USE__pointer_to_kMesh
	gMesh.DestroyAll();
#ifdef USE__pointer_to_kMesh
	}
	cout << "gkMeshPtr = " << gkMeshPtr << endl;
	if (gkMeshPtr) {
		cout << "delete kMesh" << endl;
		delete gkMeshPtr;
		gkMeshPtr = 0;
	}
#endif	// USE__pointer_to_kMesh
#ifdef USE__pointer_to_kMesh
	kMesh&	gMesh0 = *gkMeshPtr0;
	if (gkMeshPtr0) {
#endif	// USE__pointer_to_kMesh
	gMesh0.DestroyAll();
#ifdef USE__pointer_to_kMesh
	}
	cout << "gkMeshPtr0 = " << gkMeshPtr0 << endl;
	if (gkMeshPtr0) {
		cout << "delete kMesh0" << endl;
		delete gkMeshPtr0;
		gkMeshPtr0 = 0;
	}
#endif	// USE__pointer_to_kMesh
}

#ifndef __KEKO__
#if 0	// mfc 에서도...
extern "C"
{
	int	PASCAL wWinMain(HINSTANCE inst, HINSTANCE dumb, LPSTR param, int show);
};
int	main(int argc, char *argv[])
{
	char	buf[256];
	buf[0] = 0;
	for (int i = 1; i < argc; ++i) {
		strcat(buf, argv[i]);
		strcat(buf, " ");
	}

	return wWinMain((HINSTANCE) GetModuleHandle(NULL), NULL, buf, SW_SHOWNORMAL);
}
#else
// 콘솔에 메시지를 표시하기위해...
int main()
{
#if 0
	unsigned	n;
	n = 0x5243ffbc;	// 1,380,188,092L	//0x4fffffff + 0x0243ffbd;
	char*	p = (char*) malloc(n);
	if (p)
		FillMemory(p, n, 0);
	n = 0x3ffaffbc;	// 1,073,414,076L	//0x2fffffff + 0x0ffaffbd;
	char*	q = (char*) malloc(n);
	if (q)
		FillMemory(q, n, 0);
	n = 0x123bffbc;	// 305,921,980L
	char*	r = (char*) malloc(n);
	if (r)
		FillMemory(r, n, 0);
	n = 0x0a44ffbc;	// 172,294,076L
	char*	s = (char*) malloc(n);
	(void) printf("%p %p %p %p\n", p, q, r, s);
	if (s)
		FillMemory(s, n, 0);
	if (!p || !q || !r || !s) {
		if (p) {free(p); p = 0;}
		if (q) {free(q); q = 0;}
		if (r) {free(r); r = 0;}
		if (s) {free(s); s = 0;}
		return 1;
	}
#endif
//	_CrtSetBreakAlloc(498147);
#if 0	// into wWinMain
#ifdef USE__pointer_to_PickVar
	gPickVarPtr = new PickVar;
#endif	// USE__pointer_to_PickVar
#endif	// into wWinMain
	cout << "main: calling wWinMain..." << endl;
	int	rval = wWinMain(NULL, NULL, TEXT("T1..."), 0);
	cout << "main: wWinMain...done" << endl;
#if 0	// into wWinMain
#ifdef USE__pointer_to_PickVar
	OnLostDevice(0);
	OnDestroyDevice(0);
	SAFE_DELETE(gPickVarPtr);
#endif	// USE__pointer_to_PickVar
#endif	// into wWinMain
#if 0
	if (p) {free(p); p = 0;}
	if (q) {free(q); q = 0;}
	if (r) {free(r); r = 0;}
	if (s) {free(s); s = 0;}
#endif
	return rval;
}
#endif
#endif	// !__KEKO__
