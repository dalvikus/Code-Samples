#pragma once
#ifndef __kMANIPULATOR__
#define __kMANIPULATOR__
////////////////////////////////////////////////////////////////////////////////


#include <DXUT/Core/DXUT.h>
#include <DXUT/Optional/DXUTcamera.h>	// CD3DArcBall
#include <mesh/kEnum.h>
#include <mesh/kTapeMeasure.h>

class kManipulator :
	public	// kManipulator 를 통해 이를 상속받은 클래스의 override 된 getValue/setValue 를 호출하므로,
		// k2DInterfaceBase 를 접근할 수 있어야 하므로 public 으로 선언해야 한다.
	k2DInterfaceBase
{
public:
	// doRelease 에서 호출하는 callback 함수
	typedef void	(*DO_RELEASE_CALLBACK_POINTER)(kMesh* pointer_to_kMesh, int area, START_DEF_PTR* pointer_to_START_DEF_PTR, void* valuePtr);

public:
	kManipulator()		{reset();}
	virtual ~kManipulator()	{reset();}

protected:
	bool		__isSet;
	kMesh*		__pointer_to_kMesh;
	int		__area;	// <0: 초기값, 0: USER_DEFINED_MEASURE, >0: PRE_DEFINED_MEASURE
	START_DEF_PTR*	__pointer_to_START_DEF_PTR;
	DO_RELEASE_CALLBACK_POINTER	__releaseCallbackPtr;
public:
	bool	isSet() const	{return __isSet;}
	const START_DEF_PTR&	startDefPtr() const	{return *__pointer_to_START_DEF_PTR;}

public:
	virtual void	reset() {
		__isSet = false;
		__pointer_to_kMesh = 0;
		__area = -1;
		__pointer_to_START_DEF_PTR = 0;
		__releaseCallbackPtr = 0;
	}

public:
	virtual LRESULT	HandleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)	{return TRUE;}
	virtual void	doPress()	{}
	virtual	void	doRelease()	{}
};

#if 0
class CD3DArcBall2 : public CD3DArcBall
{
private:
	D3DXVECTOR3	__rightDir;	// 카메라에서 봤을 때 x축
	D3DXVECTOR3	__upDir;	// 카메라에서 봤을 때 y축
	D3DXVECTOR3	__viewDir;	// 카메라에서 봤을 때 z축
public:
	CD3DArcBall2();
public:
	// override
	// CD3DArcBall::Reset 에서 m_qNow 를 설정하기 위하여...
	virtual void	Reset(const D3DXMATRIX* cameraRotationMatrixPtr = 0);
protected:
	// override
	// 정해진 x축(1, 0, 0), y축(0, 1, 0) 대신에 각각 __rightDir, __upDir 을 사용하여 월드좌표를 구하기 위하여...
	virtual D3DXVECTOR3	ScreenToVector(float fScreenPtX, float fScreenPtY);
};
#endif

// 벱선벡터를 조절하는 툴을 다루는 클래스; ArcBall 을 사용하여 구현
class kNormalManipulator : public kManipulator, public CD3DArcBall
{
private:
	MANIPULATOR_NORMAL_ANGLE_ROTATION_T	__angle_rotation;

	D3DXVECTOR3	__P0;	// manipultor 가 조절하는 법선벡터의 시작점
	D3DXVECTOR3	__nDir;	// manipulator 가 조절하는 법선벡터; __zDir
	// manipulator 을 결정하는 서로 수직인 세 벡터
	D3DXVECTOR3	__xDir;	// 법선 벡터만이 필요하나 ArcBall 을 감싸는 세 원을 그리기위하여 필요;
				// 없다면 임의로 설정해도 좋으나...
	D3DXVECTOR3	__yDir;	// __yDir = __zDir x __xDir 로 결정

	float	__radiusInScreenSpace;	// Arcball 이 놓일 Screen Space 에서의 정사각형 영역을 결정;
					// Object Space 에서의 Arcball 의 반경으로부터 결정되어야 한다.

public:
	float	radiusInScreenSpace() const	{return __radiusInScreenSpace;}
	// override
	virtual void	reset();

	void	set(
		float	radius,		// ArcBall's radius

		MANIPULATOR_NORMAL_ANGLE_ROTATION_T	angle_rotation,

		const D3DXVECTOR3&	P0,
		const D3DXVECTOR3&	nDir,
		const D3DXVECTOR3&	xDir,

		DO_RELEASE_CALLBACK_POINTER	releaseCallbackPtr = 0,
		kMesh*	pointer_to_kMesh = 0,
		int	area = 0,
		START_DEF_PTR*	pointer_to_START_DEF_PTR = 0
	);
	D3DXVECTOR3	P0() const	{return __P0;}
	D3DXVECTOR3	nDir() const	{return __nDir;}

private:
#if 0
	CD3DArcBall	__arcBall;	// ArcBall 의 회전을 알려주는 ArcBall 자체;
					// CD3DArcBall::ScreenToVector 를 보면 카메라 좌표계에서의 회전을 결정한다.
#endif
	D3DXMATRIX	__cameraRotationMatrix;		// 카메라 자체의 월드좌표에 대한 회전
	D3DXMATRIX	__cameraRotationMatrixInverse;
public:
#if 0
	const CD3DArcBall&	arcBall() const	{return __arcBall;}
	CD3DArcBall&		arcBall()	{return __arcBall;}
#endif
	// 카메라 자체의 회전을 나타내는 행렬을 설정한다;
	// 왜냐하면 ArcBall 의 회전은 카메라 좌표계에서 이루어지기 때문이다.
	void		SetCameraRotationMatrix(const D3DXMATRIX& cameraRotationMatrix);
	// ArcBall 을 사용해 회전이 모두 끝나면 세 벡터(__nDir, __xDir, __yDir)를 새로 설정한다.
	// override
	virtual void		doPress()	{}
	virtual void		doRelease();

	D3DXVECTOR3*	GetNew_xDir(D3DXVECTOR3& xDir) const;
	D3DXVECTOR3*	GetNew_yDir(D3DXVECTOR3& yDir) const;
	D3DXVECTOR3*	GetNew_nDir(D3DXVECTOR3& nDir) const;

	kNormalManipulator();
	virtual ~kNormalManipulator() {reset();}

	// override
	virtual LRESULT	HandleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)	{return CD3DArcBall::HandleMessages(hWnd, uMsg, wParam, lParam);}

#if 1	// interface
	// 2D 에서 __view, __rotation 를 참조할 때
	virtual int	getValue(int subType, void* dataPtr);
	virtual int	setValue(int subType, const void* dataPtr);

	const MANIPULATOR_NORMAL_ANGLE_ROTATION_T&	getAngleRotationRef() const	{return __angle_rotation;}
	MANIPULATOR_NORMAL_ANGLE_ROTATION_T&		getAngleRotationRef()		{return __angle_rotation;}
#endif
};

class	kMesh;
class kPointManipulator : public kManipulator
{
public:
	struct INDEX_POINT_DEF_PTR
	{
		int		index;
		POINT_DEF*	pointDefPtr;
	};
private:
	int		__indexPicked;	// 시작점이 선택된 경우,
					// START_DEF_PTR::type = START_TYPE__GIRTH 라면 __indexPicked 는 0,
					// START_TYPE__TWO_POINT 라면 0 또는 1 이 된다.
	POINT_DEF	__pointDef;	// 시작점이 선택된 경우,
					// 마우스를 움직이면서 새롭게 Picking 된 점;
					// 마우스를 떼는 순간 __indexPicked 에 따라 START_DEF_PTR 의 해당하는 점이 이 점으로 바뀐다.
public:
	int	indexPicked() const	{return __indexPicked;}
	const POINT_DEF&	pointDef() const	{return __pointDef;}
	POINT_DEF&		pointDef()		{return __pointDef;}

public:
	void	reset()
	{
		__indexPicked = -1;

		kManipulator::reset();
	}
	kPointManipulator() {reset();}
	virtual ~kPointManipulator() {reset();}

public:
	void	set(kMesh* pointer_to_kMesh, int area, START_DEF_PTR* pointer_to_START_DEF_PTR, DO_RELEASE_CALLBACK_POINTER releaseCallbackPtr = 0)
	{
		__pointer_to_kMesh = pointer_to_kMesh;
		__area = area;
		__pointer_to_START_DEF_PTR = pointer_to_START_DEF_PTR;
		__releaseCallbackPtr = releaseCallbackPtr;
		__isSet = true;
	}

	void	pick(const CModelViewerCamera& camera);
public:
	// override
	virtual int	getValue(int subType, void* dataPtr);
	virtual int	setValue(int subType, const void* dataPtr);
	virtual LRESULT	HandleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	// override
	virtual void		doPress()	{}
	virtual void		doRelease();
};

/*
 * manipulator 구현 순서
 *	[단계 1] kManipulator 를 상속받는다;
 *	이 때, pure virtual 인 k2DInterfaceBase::getValue 와 k2DInterfaceBase::setValue 함수를 구현한다.
 *
 *	[단계 2] kMesh.h 에 이 클래스의 instance 를 만든다;
 *	kMesh::setValue(TYPE__MANIPULATOR, ...) 에서 subType = MANIPULATOR__TYPE 과 subType = MANIPULATOR__SET 인 경우,
 *	이 instance 를 사용할 수 있도록 한다.
 *	특히 MANIPULATOR__SET 의 경우에 호출하는 __setXxxManipulator 함수를 구현한다;
 *	결국에는 이 클래스의 set 함수를 호출한다.
 *
 *	[단계 3] kMesh::RenderManipulator 에서 이 manipulator 를 렌더링하도록 구현한다.
 *
 *	[단계 4] 마우스 핸들러인 doPress, doDrag, doRelease 함수를 구현한다;
 *	doPress, doDrag 는 Pick.cpp 의 MsgProc 에서 doRelease 는 상속받은 클래스에서.
 *
 *
 *	[단계 5] 이 manipulator 에서 필요한 변수들을 정의하고, 위의 네 단계를 구체적으로 정의한 후
 *	원하는 결과를 얻도록 한다:
 *		doPress 에서는 변수의 설정을 한다.
 *		doDrag 와 kMesh::RenderManipulator 에서는 manipulator 를 현 단계를 보여준다.
 *		doRelease 에서는 설정한 변수를 사용하여 원하는 결과는 얻는다.
 */
class kLineManipulator : public kManipulator
{
	virtual int	getValue(int subType, void* dataPtr);
	virtual int	setValue(int subType, const void* dataPtr);
	virtual void	doRelease();
public:
	kLineManipulator()
	{
		__segmentVertexBufferPtr = 0;
		__segmentIndexBufferPtr = 0;

		reset();
	}
	virtual ~kLineManipulator()	{reset();}

	// override
	virtual void	reset();
/*
 * 수정할 수 있는 선분들을 구성하는 점들은... (kTapeMeasure::doMeasure_s 로부터)
 *
	TAPE_MEASURE_MODE_T/TAPE_MEASURE_METHOD_T	TAPE_MEASURE_METHOD_GIRTH	TAPE_MEASURE_METHOD_TWO_POINT
	-----------------------------------------	-------------------------	-----------------------------
	TAPE_MEASURE_MODE_CONTOUR			__segmentVertexBuffer		__twoPointVertexBufferPtr
	TAPE_MEASURE_MODE_WRAP				__convexVertexBufferPtr		__convexVertexBufferPtr

	주) 정당한 지의 여부는 set 함수를 호출하기 전에 결정
 */

// 변수들...
private:
	MEASURE_ITEM*	__pointer_to_MEASURE_ITEM;
	TAPE_MEASURE_MODE_T	__mode;	// 곡선을 얻는 방법
	TAPE_MEASURE_METHOD_T	__method;

	int	__indexPicked;	// 선택된 line segment 의 __segmentPointArray 에 대한 인덱스;
				// 이 값이 음수가 아니면 해당 line segment 가 선택되었음을 뜻한다.
	D3DXVECTOR3	__pickedPoint;	// 선택된 선분의 Picking 된 지점에서의 정보;
					// 선들을 변형할 평면을 결정할 때 사용
	CGrowableArray<SEGMENT_VERTEX>	__segmentPointArray;
	bool		__closed;	// 닫힌 곡선을 만드는지...
	// line manipulator 가 설정될 때,
	//	__mode
	//	__closed
	// 는 결정되고 해제될 때까지 그대로 유지된다.
	// line manipulator 가 설정될 때, 현재 점들에 의해 __segmentPointArray 가 결정되나
	// 마우스를 doRelease 할 때마다 새로운 점들이 생길 수 있으므로
	// 이 값은 바뀔 수 있다.
	// 마우스로 특정 line 을 선택하면
	//	__indexPicked
	//	__pickedPoint
	// 가 결정되고, 마우스를 doRelease 할 때까지 유지된다.
	// 마우스를 움직일 때마다 새로운 점들이 생기면서 __segmentIndexBufferPtr 를 갱신할 수 있고,
	// 이 버퍼에 있는 점들이 만드는 새그먼트의 갯수가 __numPointsInSegmentBuffer 이다;
	// 마우스를 떼는 시점에 __segmentIndexBufferPtr 의 내용이 __segmentPointArray 에 복사된다:
	//	__numPointsInSegmentBuffer
	//	__segmentIndexBufferPtr
	unsigned	__numPointsInSegmentBuffer;	// __segmentVertexBufferPtr 에 담긴 점들이 만드는 세그먼트 갯수
	LPDIRECT3DVERTEXBUFFER9		__segmentVertexBufferPtr;	// __segmentPointArray 로부터 파생된 점들을 담는 버퍼; 렌더링과 동시에 결과물을 담는다.
	LPDIRECT3DINDEXBUFFER9		__segmentIndexBufferPtr;
public:
	unsigned	numSegmentsInSegmentBuffer() const	{return __closed ? __numPointsInSegmentBuffer : __numPointsInSegmentBuffer - 1;}
	int		indexPicked() const	{return __indexPicked;}
	const D3DXVECTOR3&	pickedPoint() const	{return __pickedPoint;}
	const LPDIRECT3DVERTEXBUFFER9	segmentVertexBufferPtr() const	{return __segmentVertexBufferPtr;}
	const LPDIRECT3DINDEXBUFFER9	segmentIndexBufferPtr() const	{return __segmentIndexBufferPtr;}

// 렌더링:
//	1) manipulator 가 set 을 통해 설정되면 __segmentPointArray 를 구성하는 점들을 특정색(흰색)으로 표시;
//	기존의 표시된 선들은 무시하거나 아예 표시하지 않는다.
//	2) 특정 선분이 선택되면 선택된 지점을 특정색(흰색)으로 표시하고
//	그 점을 움직일 때마다 새로운 점을 다른 색으로 표시
public:
	void	set(kMesh* pointer_to_kMesh, int area, TAPE_MEASURE_MODE_T mode, TAPE_MEASURE_METHOD_T method, bool closed, unsigned numPoints, const SEGMENT_VERTEX* pointPtr, MEASURE_ITEM* pointer_to_MEASURE_ITEM);

	void	pick(const CModelViewerCamera& camera);

private:
	bool	__isReleased;	// 원래의 선에 변화가 일어나서 다른 manipulator 로의 전환을 불가능함을 알린다.
	bool		__isPointDefSet;	// __pointDef 이 결정된 경우
	bool		__isOutOfRange;		// setPointDef 에 주어지는 pointDef 가 너무 범위를 벗어났을 때,
						// 새로운 pointDef 로 설정할 수 없어서 이를 렌더링하는 함수에 알려,
						// 기존의 pointDef 에 색깔을 칠해 사용자에게 알릴 때 사용
	PLANE_DEF	__planeDef;	// 시작점이 선택된 경우

public:
	bool	isReleased() const	{return __isReleased;}
	bool	isPointDefSet() const	{return __isPointDefSet;}
	bool	isOutOfRange() const	{return __isOutOfRange;}
	const PLANE_DEF&	planeDef() const	{return __planeDef;}
	PLANE_DEF&	planeDef()	{return __planeDef;}

	// __planeDef 의 pointDef 를 설정함과 동시에
	// __pickedPoint 를 이용하여 __planeDef 의 normal 도 설정;
	// 의미있는 normal 을 설정한 경우 0을 그렇지 않은 경우 -1을 리턴한다.
	static const	float	Stretch_Threshold;
	int	setPointDef(const POINT_DEF& pointDef);

	int	__leftIndex;
	int	__rightIndex;
	CGrowableArray<SEGMENT_VERTEX>	__twoPointArray;
	CGrowableArray<SEGMENT_VERTEX>	__convexPointArray;
#if 0	// 둘레측정으로 구현
public:
	// 새로운 점이, __pickedPoint.nDir x __pickedPoint.N 에 의해 결정되는 평면으로부터 너무 많이 벗어나지 않도록 한다.
	static const float	RangeConeAngleInDegree;
	static const float	TiltAngleInDegree;

#endif
};


////////////////////////////////////////////////////////////////////////////////
#endif	// !__kMANIPULATOR__
