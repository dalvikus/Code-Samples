#pragma once
#ifndef __kBRUSH__
#define __kBRUSH__


struct kBrush
{
	typedef enum {
		BRUSH_NONE,
		BRUSH_STANDARD,
	//	BRUSH_SMOOTH,
		BRUSH_FLATTEN,
	} BRUSH_TYPE;

	struct Flag {
		// hierarchy 에 따라 indent
		bool	On:1;		// 브러쉬 모드인지;
					// 아래의 모든 값들은 이 값이 True 일 때만 해당
			bool	Do:1;		// 빗질을 시작되었는 지(MsgProc와 관련)
			bool	Apply:1;	// 빗질을 적용할 지;
						// Sculpture w/ Sub, Smooth 는 이 값이 True 일 때만 해당
				bool		Sculpture:1;	// Vertex 를 변형할 지;
								// Sub 는 이 값이 True 일 때만 해당
				bool		Sub:1;		// BRUSH_STANDARD 의 경우 들어가게 할 것인지
				bool		Smooth:1;	// 동시에 또는 단독으로 Vertex Smoothing 을 줄 것인지
				unsigned	SmoothLevel:2;	// Smoothing 단계; 0~3 까지 총 4단계
			bool	Show:1;		// 브러쉬 안에 들어오는 Vertex 을 표시할 지

		Flag() {Reset();}
		void Reset()
		{
			// hierarchy 에 따라 indent
			On	= false;	// F12
				Do	= false;
				Apply	= false;	// SHIFT + CTRL + F12
					Sculpture	= false;	// ALT + F12 (Or w/ ALT)
					Sub	= false;		// SHIFT + F12 (Or w/ SHIFT)
					Smooth	= false;		// CTRL + F12 (Or w/ CTRL)
					SmoothLevel = 3;		// 0단계(No Smoothing)
				Show	= false;	// SHIFT + ALT + F12
		}
	};
	Flag	flag;
	BRUSH_TYPE	Type;	// 종류		// SHIFT + CTRL + ALT + F12
	float	Radius;		// 반경
	float	Intensity;	// 빗질의 강도	// [SHIFT +] CTRL + ALT + F10
	D3DXVECTOR3	RayDirection;	// 월드좌표에서 화면 앞에서 브러쉬를 바라보는 방향
	INTERSECTION	intersection;	// 브러쉬의 중심이 물체와 만나는 지점에 대한 정보;
					// Flag::Do 가 True 일 때 유효

	kBrush() {Reset();}
	void	Reset()
	{
		flag.Reset();

		Type	= BRUSH_NONE;
		Radius	= 40.f;
		Intensity	= .0001f;
		RayDirection	= D3DXVECTOR3(0, 0, 0);
	}
};


#endif	// !__kBRUSH__
