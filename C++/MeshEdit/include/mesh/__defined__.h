#include <mesh/__defined__>	// 부위 정의

#if 1	// 둘레/길이 정의
#if 1	// DAZ 표준모델
namespace DAZ {
	namespace Head {
		PLANE_DEF	planeDef = {
			{89438, 0.250809f, 0.288781f},
			D3DXVECTOR3(0, 1, 0)
		};
		MEASURE_DEF__GIRTH	girthDef(TAPE_MEASURE_MODE_CONTOUR, planeDef);

		////
		MEASURE_DEF	defList[1] = {
			{MEASURE_TYPE__GIRTH, &girthDef},
		};
	}

	////
	MEASURE_TYPE_DEF	measureTypeDef[] = {
		{TAPE_MEASURE_HEAD,
			{
				L"머리",
				D3DXVECTOR2(10, 10),
				D3DCOLOR_XRGB(0x00, 0x66, 0xcc),
			},
			{1, Head::defList}
		},
	};
}
#endif	// DAZ 표준모델
#if 1	// 에트리 표준모델
namespace ETRI {
#if 1	// 둘레
	namespace Head {
		PLANE_DEF	planeDef = {
			{15743, 0.221824f, 0.408594f},
			D3DXVECTOR3(0, 1, 0)
		};
		MEASURE_DEF__GIRTH	girthDef(TAPE_MEASURE_MODE_CONTOUR, planeDef);

		////
		MEASURE_DEF	defList[1] = {
			{MEASURE_TYPE__GIRTH, &girthDef},
		};
	}
	namespace Neck {
		PLANE_DEF	planeDef = {
			{115150, 0.242606f, 0.727244f},
			D3DXVECTOR3(0, 1, 0)
		};
		MEASURE_DEF__GIRTH	girthDef(TAPE_MEASURE_MODE_CONTOUR, planeDef);

		////
		MEASURE_DEF	defList[1] = {
			{MEASURE_TYPE__GIRTH, &girthDef},
		};
	}
	namespace Breast {
		PLANE_DEF	planeDef = {
			{57914, 0.406461f, 0.384281f},
			D3DXVECTOR3(0, 1, 0)
		};
		MEASURE_DEF__GIRTH	girthDef(TAPE_MEASURE_MODE_WRAP, planeDef);

		////
		MEASURE_DEF	defList[1] = {
			{MEASURE_TYPE__GIRTH, &girthDef},
		};
	}
	namespace Waist {
		PLANE_DEF	planeDef = {
			{61429, 0.279595f, 0.336564f},
			D3DXVECTOR3(0, 1, 0)
		};
		MEASURE_DEF__GIRTH	girthDef(TAPE_MEASURE_MODE_CONTOUR, planeDef);

		////
		MEASURE_DEF	defList[1] = {
			{MEASURE_TYPE__GIRTH, &girthDef},
		};
	}
	namespace Hip {
		PLANE_DEF	planeDef = {
			{65025, 0.815195f, 0.053784f},
			D3DXVECTOR3(0, 1, 0)
		};
		MEASURE_DEF__GIRTH	girthDef(TAPE_MEASURE_MODE_WRAP, planeDef);

		////
		MEASURE_DEF	defList[1] = {
			{MEASURE_TYPE__GIRTH, &girthDef},
		};
	}
	namespace LeftThigh {
		PLANE_DEF	planeDef = {
			{63898, 0.366757f, 0.103650f},
			D3DXVECTOR3(0, 1, 0)
		};
		MEASURE_DEF__GIRTH	girthDef(TAPE_MEASURE_MODE_CONTOUR, planeDef);

		////
		MEASURE_DEF	defList[1] = {
			{MEASURE_TYPE__GIRTH, &girthDef},
		};
	}
	namespace RightThigh {
		PLANE_DEF	planeDef = {
			{65710, 0.526200f, 0.101572f},
			D3DXVECTOR3(0, 1, 0)
		};
		MEASURE_DEF__GIRTH	girthDef(TAPE_MEASURE_MODE_CONTOUR, planeDef);

		////
		MEASURE_DEF	defList[1] = {
			{MEASURE_TYPE__GIRTH, &girthDef},
		};
	}
	namespace LeftForeArm {
		PLANE_DEF	planeDef = {
			{57248, 0.314191f, 0.006411f},
			D3DXVECTOR3(-0.956910f, 0.279197f, -0.079870f)
		};
		FACEID_LIST	faceID_list = {1407, ETRI::leftForearm};
		MEASURE_DEF__GIRTH	girthDef(TAPE_MEASURE_MODE_CONTOUR, planeDef, &faceID_list);

		////
		MEASURE_DEF	defList[1] = {
			{MEASURE_TYPE__GIRTH, &girthDef},
		};
	}
	namespace RightForeArm {
		PLANE_DEF	planeDef = {
			{60035, 0.317288f, 0.417311f},
			D3DXVECTOR3(-0.971231f, -0.143522f, -0.190048f)
		};
		MEASURE_DEF__GIRTH	girthDef(TAPE_MEASURE_MODE_CONTOUR, planeDef);

		////
		MEASURE_DEF	defList[1] = {
			{MEASURE_TYPE__GIRTH, &girthDef},
		};
	}
#endif	// 둘레
#if 1	// 길이
	namespace Height {
		POINT_PLANE_DEF	pointPlaneDef1 = {
			{20790, 0.025643f, 0.962431f},	// 정수리
			{				// 골반
				{62710, 0.448102f, 0.258171f},
				D3DXVECTOR3(0, 1, 0)
			}
		};
		MEASURE_DEF__POINT_PLANE_DISTANCE	pointPlaneDistanceDef1(pointPlaneDef1);

		TWO_PLANE_DEF	twoPlaneDef = {
			{				// 골반
				{65847, 0.015407f, 0.824872f},
				D3DXVECTOR3(0.086850f, 0.996197f, 0.007047f)
			},
			{				// 오른쪽 무릎
				{72432, 0.102591f, 0.257141f},
				D3DXVECTOR3(0.086850f, 0.996197f, 0.007047f)
			}
		};
		MEASURE_DEF__TWO_PLANE_DISTANCE	twoPlaneDistanceDef(twoPlaneDef);

		POINT_PLANE_DEF	pointPlaneDef2 = {
			{73637, 0.016866f, 0.185059f},	// 오른쪽 발뒤꿈치
			{				// 오른쪽 무릎
				{72432, 0.102591f, 0.257141f},
				D3DXVECTOR3(0.086850f, 0.996197f, 0.007047f)
			},
		};
		MEASURE_DEF__POINT_PLANE_DISTANCE	pointPlaneDistanceDef2(pointPlaneDef2);

		////
		MEASURE_DEF	defList[3] = {
			{MEASURE_TYPE__POINT_PLANE_DISTANCE, &pointPlaneDistanceDef1},
			{MEASURE_TYPE__TWO_PLANE_DISTANCE, &twoPlaneDistanceDef},
			{MEASURE_TYPE__POINT_PLANE_DISTANCE, &pointPlaneDistanceDef2},
		};
	}
	namespace SeatedHeight {
		POINT_PLANE_DEF	pointPlaneDef = {
			{20790, 0.025643f, 0.962431f},	// 정수리
			{				// 골반
				{62710, 0.448102f, 0.258171f},
				D3DXVECTOR3(0, 1, 0)
			}
		};
		MEASURE_DEF__POINT_PLANE_DISTANCE	pointPlaneDistanceDef(pointPlaneDef);

		////
		MEASURE_DEF	defList[1] = {
			{MEASURE_TYPE__POINT_PLANE_DISTANCE, &pointPlaneDistanceDef},
		};
	}
	namespace Shoulder {
		TWO_POINT_DEF	twoPointDef = {
			{56509, 0.018941f, 0.184516f},
			{59286, 0.547807f, 0.072270f}
		};
		MEASURE_DEF__TWO_POINT_GIRTH	twoPointGirth(TAPE_MEASURE_MODE_WRAP, twoPointDef);

		////
		MEASURE_DEF	defList[1] = {
			{MEASURE_TYPE__TWO_POINT_GIRTH, &twoPointGirth},
		};
	}
	namespace LeftLeg {
		TWO_PLANE_DEF	twoPlaneDef = {
			{	// 허벅지 윗부분
				{63780, 0.277089f, 0.326401f},
				D3DXVECTOR3(-0.025932f, 0.984654f, -0.172584f)
			},
			{	// 무릎
				{71314, 0.424358f, 0.008220f},
				D3DXVECTOR3(-0.025932f, 0.984654f, -0.172584f)
			}
		};
		MEASURE_DEF__TWO_PLANE_DISTANCE	twoPlaneDistanceDef(twoPlaneDef);

		POINT_PLANE_DEF	pointPlaneDef = {
			{74634, 0.960823f, 0.020719f},	// 발뒤꿈치
			{	// 무릎
				{71314, 0.424358f, 0.008220f},
				D3DXVECTOR3(-0.025932f, 0.984654f, -0.172584f)
			},
		};
		MEASURE_DEF__POINT_PLANE_DISTANCE	pointPlaneDistanceDef(pointPlaneDef);

		////
		MEASURE_DEF	defList[2] = {
			{MEASURE_TYPE__TWO_PLANE_DISTANCE, &twoPlaneDistanceDef},
			{MEASURE_TYPE__POINT_PLANE_DISTANCE, &pointPlaneDistanceDef},
		};
	}
	namespace RightLeg {
		TWO_PLANE_DEF	twoPlaneDef = {
			{	// 허벅지 윗부분
				{66488, 0.188731f, 0.313384f},
				D3DXVECTOR3(-0.025932f, 0.984654f, -0.172584f)
			},
			{	// 무릎
				{72746, 0.530067f, 0.334535f},
				D3DXVECTOR3(-0.025932f, 0.984654f, -0.172584f)
			}
		};
		MEASURE_DEF__TWO_PLANE_DISTANCE	twoPlaneDistanceDef(twoPlaneDef);

		POINT_PLANE_DEF	pointPlaneDef = {
			{73684, 0.008145f, 0.150210f},	// 발뒤꿈치
			{	// 무릎
				{72746, 0.530067f, 0.334535f},
				D3DXVECTOR3(-0.025932f, 0.984654f, -0.172584f)
			},
		};
		MEASURE_DEF__POINT_PLANE_DISTANCE	pointPlaneDistanceDef(pointPlaneDef);

		////
		MEASURE_DEF	defList[2] = {
			{MEASURE_TYPE__TWO_PLANE_DISTANCE, &twoPlaneDistanceDef},
			{MEASURE_TYPE__POINT_PLANE_DISTANCE, &pointPlaneDistanceDef},
		};
	}
	namespace LeftArm {
		TWO_PLANE_DEF	twoPlaneDef1 = {
			{	// 팔 윗부분
				{81085, 0.073198f, 0.486765f},
				D3DXVECTOR3(-0.998316f, 0.058097f, 0.000468f)
			},
			{	// 팔꿈치
				{56777, 0.059849f, 0.894122f},
				D3DXVECTOR3(-0.998316f, 0.058097f, 0.000468f)
			}
		};
		MEASURE_DEF__TWO_PLANE_DISTANCE	twoPlaneDistanceDef1(twoPlaneDef1);

		TWO_PLANE_DEF	twoPlaneDef2 = {
			{	// 팔꿈치
				{81965, 0.369442f, 0.201236f},
				D3DXVECTOR3(-0.931428f, 0.061501f, 0.358703f)
			},
			{	// 손목
				{81085, 0.073198f, 0.486765f},
				D3DXVECTOR3(-0.931428f, 0.061501f, 0.358703f)
			}
		};
		MEASURE_DEF__TWO_PLANE_DISTANCE	twoPlaneDistanceDef2(twoPlaneDef2);

		////
		MEASURE_DEF	defList[2] = {
			{MEASURE_TYPE__TWO_PLANE_DISTANCE, &twoPlaneDistanceDef1},
			{MEASURE_TYPE__TWO_PLANE_DISTANCE, &twoPlaneDistanceDef2},
		};
	}
	namespace RightArm {
		TWO_PLANE_DEF	twoPlaneDef1 = {
			{	// 팔 위부분
				{58673, 0.103706f, 0.770020f},
				D3DXVECTOR3(-0.970453f, -0.220734f, -0.097510f)
			},
			{	// 팔꿈치
				{82418, 0.251168f, 0.664359f},
				D3DXVECTOR3(-0.970453f, -0.220734f, -0.097510f)
			}
		};
		MEASURE_DEF__TWO_PLANE_DISTANCE	twoPlaneDistanceDef1(twoPlaneDef1);

		TWO_PLANE_DEF	twoPlaneDef2 = {
			{	// 팔꿈치
				{82418, 0.251168f, 0.664359f},
				D3DXVECTOR3(-0.964311f, -0.052404f, -0.259560f)
			},
			{	// 손목
				{84823, 0.257910f, 0.566288f},
				D3DXVECTOR3(-0.964311f, -0.052404f, -0.259560f)
			}
		};
		MEASURE_DEF__TWO_PLANE_DISTANCE	twoPlaneDistanceDef2(twoPlaneDef2);

		////
		MEASURE_DEF	defList[2] = {
			{MEASURE_TYPE__TWO_PLANE_DISTANCE, &twoPlaneDistanceDef1},
			{MEASURE_TYPE__TWO_PLANE_DISTANCE, &twoPlaneDistanceDef2},
		};
	}
	namespace LeftArmInside {
		TWO_POINT_DEF	twoPointDef0 = {
			{56950, 0.556603f, 0.268797f},
			{81040, 0.561017f, 0.294061f}
		};
		MEASURE_DEF__TWO_POINT_GIRTH	twoPointGirthDef0(TAPE_MEASURE_MODE_WRAP, twoPointDef0);

		TWO_POINT_DEF	twoPointDef1 = {
			{82198, 0.325851f, 0.469118f},
			{81040, 0.561017f, 0.294061f}
		};
		MEASURE_DEF__TWO_POINT_GIRTH	twoPointGirthDef1(TAPE_MEASURE_MODE_WRAP, twoPointDef1);

		////
		MEASURE_DEF	defList[2] = {
			{MEASURE_TYPE__TWO_POINT_GIRTH, &twoPointGirthDef0},
			{MEASURE_TYPE__TWO_POINT_GIRTH, &twoPointGirthDef1},
		};
	}
	namespace LeftArmOutside {
		TWO_POINT_DEF	twoPointDef0 = {
			{56540, 0.439220f, 0.528292f},
			{81116, 0.567838f, 0.280615f}
		};
		MEASURE_DEF__TWO_POINT_GIRTH	twoPointGirthDef0(TAPE_MEASURE_MODE_WRAP, twoPointDef0);

		TWO_POINT_DEF	twoPointDef1 = {
			{81638, 0.585758f, 0.282053f},
			{81116, 0.567838f, 0.280615f}
		};
		MEASURE_DEF__TWO_POINT_GIRTH	twoPointGirthDef1(TAPE_MEASURE_MODE_WRAP, twoPointDef1);

		////
		MEASURE_DEF	defList[2] = {
			{MEASURE_TYPE__TWO_POINT_GIRTH, &twoPointGirthDef0},
			{MEASURE_TYPE__TWO_POINT_GIRTH, &twoPointGirthDef1},
		};
	}
	namespace RightArmInside {
		TWO_POINT_DEF	twoPointDef0 = {
			{59046, 0.097483f, 0.517813f},
			{82458, 0.096169f, 0.430583f}
		};
		MEASURE_DEF__TWO_POINT_GIRTH	twoPointGirthDef0(TAPE_MEASURE_MODE_WRAP, twoPointDef0);

		TWO_POINT_DEF	twoPointDef1 = {
			{84733, 0.087797f, 0.654378f},
			{82458, 0.096169f, 0.430583f}
		};
		MEASURE_DEF__TWO_POINT_GIRTH	twoPointGirthDef1(TAPE_MEASURE_MODE_WRAP, twoPointDef1);

		////
		MEASURE_DEF	defList[2] = {
			{MEASURE_TYPE__TWO_POINT_GIRTH, &twoPointGirthDef0},
			{MEASURE_TYPE__TWO_POINT_GIRTH, &twoPointGirthDef1},
		};
	}
	namespace RightArmOutside {
		TWO_POINT_DEF	twoPointDef0 = {
			{59301, 0.006432f, 0.615452f},
			{83625, 0.448489f, 0.026192f}
		};
		MEASURE_DEF__TWO_POINT_GIRTH	twoPointGirthDef0(TAPE_MEASURE_MODE_WRAP, twoPointDef0);

		TWO_POINT_DEF	twoPointDef1 = {
			{84828, 0.265014f, 0.658732f},
			{83625, 0.448489f, 0.026192f}
		};
		MEASURE_DEF__TWO_POINT_GIRTH	twoPointGirthDef1(TAPE_MEASURE_MODE_WRAP, twoPointDef1);

		////
		MEASURE_DEF	defList[2] = {
			{MEASURE_TYPE__TWO_POINT_GIRTH, &twoPointGirthDef0},
			{MEASURE_TYPE__TWO_POINT_GIRTH, &twoPointGirthDef1},
		};
	}
	namespace LeftLegInside {
		TWO_POINT_DEF	twoPointDef = {
//			{64476, 0.321285f, 0.668669f},
//			{74333, 0.221292f, 0.665281f}
			{64379, 0.260959f, 0.429566f},
			{74151, 0.237487f, 0.348736f}
		};
		MEASURE_DEF__TWO_POINT_GIRTH	twoPointGirth(TAPE_MEASURE_MODE_WRAP, twoPointDef);

		////
		MEASURE_DEF	defList[1] = {
			{MEASURE_TYPE__TWO_POINT_GIRTH, &twoPointGirth},
		};
	}
	namespace LeftLegOutside {
		TWO_POINT_DEF	twoPointDef = {
//			{64261, 0.598616f, 0.322904f},
//			{74678, 0.226481f, 0.352553f}
			{63754, 0.831608f, 0.088129f},
			{74125, 0.783423f, 0.183948f}
		};
		MEASURE_DEF__TWO_POINT_GIRTH	twoPointGirth(TAPE_MEASURE_MODE_WRAP, twoPointDef);

		////
		MEASURE_DEF	defList[1] = {
			{MEASURE_TYPE__TWO_POINT_GIRTH, &twoPointGirth},
		};
	}
	namespace RightLegInside {
		TWO_POINT_DEF	twoPointDef = {
			{66209, 0.259354f, 0.478601f},
			{73868, 0.154951f, 0.400970f}
		};
		MEASURE_DEF__TWO_POINT_GIRTH	twoPointGirth(TAPE_MEASURE_MODE_WRAP, twoPointDef);

		////
		MEASURE_DEF	defList[1] = {
			{MEASURE_TYPE__TWO_POINT_GIRTH, &twoPointGirth},
		};
	}
	namespace RightLegOutside {
		TWO_POINT_DEF	twoPointDef = {
			{73721, 0.485668f, 0.005433f},
			{65561, 0.382790f, 0.059710f}
		};
		MEASURE_DEF__TWO_POINT_GIRTH	twoPointGirth(TAPE_MEASURE_MODE_WRAP, twoPointDef);

		////
		MEASURE_DEF	defList[1] = {
			{MEASURE_TYPE__TWO_POINT_GIRTH, &twoPointGirth},
		};
	}
#endif	// 길이

	////
	MEASURE_TYPE_DEF	measureTypeDef[9 + 15] = {
#if 1	// 둘레
		{TAPE_MEASURE_HEAD,
			{
				L"머리",
				D3DXVECTOR2(373, 27),
				D3DCOLOR_XRGB(0x99, 0x00, 0x00),
			},
			{1, Head::defList}
		},
		{TAPE_MEASURE_NECK,
			{
				L"목",
				D3DXVECTOR2(425, 48),
				D3DCOLOR_XRGB(0x00, 0x66, 0xcc),
			},
			{1, Neck::defList}
		},
		{TAPE_MEASURE_BREAST,
			{
				L"가슴",
				D3DXVECTOR2(169, 104),
				D3DCOLOR_XRGB(0x00, 0x66, 0xcc),
			},
			{1, Breast::defList}
		},
		{TAPE_MEASURE_WAIST,
			{
				L"허리",
				D3DXVECTOR2(174, 139),
				D3DCOLOR_XRGB(0x00, 0x66, 0xcc),
			},
			{1, Waist::defList}
		},
		{TAPE_MEASURE_HIP,
			{
				L"엉덩이",
				D3DXVECTOR2(392, 119),
				D3DCOLOR_XRGB(0x00, 0x66, 0xcc),
			},
			{1, Hip::defList}
		},
		{TAPE_MEASURE_LEFT_THIGH,
			{
				L"왼쪽허벅지",
				D3DXVECTOR2(386, 186),
				D3DCOLOR_XRGB(0x00, 0x66, 0xcc),
			},
			{1, LeftThigh::defList}
		},
		{TAPE_MEASURE_RIGHT_THIGH,
			{
				L"오른쪽허벅지",
				D3DXVECTOR2(237, 186),
				D3DCOLOR_XRGB(0x00, 0x66, 0xcc),
			},
			{1, RightThigh::defList}
		},
		{TAPE_MEASURE_LEFT_FOREARM,
			{
				L"왼쪽상완",
				D3DXVECTOR2(393, 105),
				D3DCOLOR_XRGB(0x00, 0x66, 0xcc),
			},
			{1, LeftForeArm::defList}
		},
		{TAPE_MEASURE_RIGHT_FOREARM,
			{
				L"오른쪽상완",
				D3DXVECTOR2(195, 96),
				D3DCOLOR_XRGB(0x00, 0x66, 0xcc),
			},
			{1, RightForeArm::defList}
		},
#endif	// 둘레
#if 1	// 길이
		{TAPE_MEASURE_HEIGHT,
			{
				L"키",
				D3DXVECTOR2(140, 240),
				D3DCOLOR_XRGB(0x00, 0x66, 0xcc),
			},
			{3, Height::defList}
		},
		{TAPE_MEASURE_SEATED_HEIGHT,
			{
				L"앉은키",
				D3DXVECTOR2(449, 139),
				D3DCOLOR_XRGB(0x00, 0x66, 0xcc),
			},
			{1, SeatedHeight::defList}
		},
		{TAPE_MEASURE_SHOULDER_WIDTH,
			{
				L"어깨넓이",
				D3DXVECTOR2(186, 45),
				D3DCOLOR_XRGB(0x00, 0x66, 0xcc),
			},
			{1, Shoulder::defList}
		},
		{TAPE_MEASURE_LEFT_LEG,
			{
				L"왼다리",
				D3DXVECTOR2(393, 197),
				D3DCOLOR_XRGB(0x00, 0x66, 0xcc),
			},
			{2, LeftLeg::defList}
		},
		{TAPE_MEASURE_RIGHT_LEG,
			{
				L"오른다리",
				D3DXVECTOR2(225, 191),
				D3DCOLOR_XRGB(0x00, 0x66, 0xcc),
			},
			{2, RightLeg::defList}
		},
		{TAPE_MEASURE_LEFT_ARM,
			{
				L"왼팔",
				D3DXVECTOR2(439, 110),
				D3DCOLOR_XRGB(0x00, 0x66, 0xcc),
			},
			{2, LeftArm::defList}
		},
		{TAPE_MEASURE_RIGHT_ARM,
			{
				L"오른팔",
				D3DXVECTOR2(157, 109),
				D3DCOLOR_XRGB(0x00, 0x66, 0xcc),
			},
			{2, RightArm::defList}
		},
		{TAPE_MEASURE_LEFT_ARM_INSIDE,
			{
				L"왼팔안쪽",
				D3DXVECTOR2(476, 96),
				D3DCOLOR_XRGB(0x00, 0x66, 0xcc),
			},
			{2, LeftArmInside::defList}
		},
		{TAPE_MEASURE_LEFT_ARM_OUTSIDE,
			{
				L"왼팔바깥쪽",
				D3DXVECTOR2(149, 99),
				D3DCOLOR_XRGB(0x00, 0x66, 0xcc),
			},
			{2, LeftArmOutside::defList}
		},
		{TAPE_MEASURE_RIGHT_ARM_INSIDE,
			{
				L"오른팔안쪽",
				D3DXVECTOR2(469, 49),
				D3DCOLOR_XRGB(0x00, 0x66, 0xcc),
			},
			{2, RightArmInside::defList}
		},
		{TAPE_MEASURE_RIGHT_ARM_OUTSIDE,
			{
				L"오른팔바깥쪽",
				D3DXVECTOR2(153, 52),
				D3DCOLOR_XRGB(0x00, 0x66, 0xcc),
			},
			{2, RightArmOutside::defList}
		},
		{TAPE_MEASURE_LEFT_LEG_INSIDE,
			{
				L"왼다리안쪽",
				D3DXVECTOR2(390, 303),
//				D3DCOLOR_XRGB(0x00, 0x66, 0xcc),
				D3DCOLOR_XRGB(0x99, 0x00, 0x00),
			},
			{1, LeftLegInside::defList}
		},
		{TAPE_MEASURE_LEFT_LEG_OUTSIDE,
			{
				L"왼다리바깥쪽",
				D3DXVECTOR2(448, 367),
//				D3DCOLOR_XRGB(0x00, 0x66, 0xcc),
				D3DCOLOR_XRGB(0x99, 0x00, 0x00),
			},
			{1, LeftLegOutside::defList}
		},
		{TAPE_MEASURE_RIGHT_LEG_INSIDE,
			{
				L"오른다리안쪽",
				D3DXVECTOR2(213, 303),
				D3DCOLOR_XRGB(0x00, 0x66, 0xcc),
			},
			{1, RightLegInside::defList}
		},
		{TAPE_MEASURE_RIGHT_LEG_OUTSIDE,
			{
				L"오른다리바깥쪽",
				D3DXVECTOR2(164, 350),
				D3DCOLOR_XRGB(0x00, 0x66, 0xcc),
			},
			{1, RightLegOutside::defList}
		},
#endif	// 길이
	};
}
namespace ETRI2 {
#if 1	// 둘레
	namespace Breast {
		PLANE_DEF	planeDef = {
			{3580, 0.0435605f, 0.371356f},
			D3DXVECTOR3(0, 1, 0)
		};
		MEASURE_DEF__GIRTH	girthDef(TAPE_MEASURE_MODE_WRAP, planeDef);

		////
		MEASURE_DEF	defList[1] = {
			{MEASURE_TYPE__GIRTH, &girthDef},
		};
	}
#endif	// 둘레
#if 1	// 길이
	namespace Shoulder {
		TWO_POINT_DEF	twoPointDef = {
			{4020, 0.689240f, 0.039838f},
			{7610, 0.051608f, 0.714304f}
		};
		MEASURE_DEF__TWO_POINT_GIRTH	twoPointGirth(TAPE_MEASURE_MODE_WRAP, twoPointDef);

		////
		MEASURE_DEF	defList[1] = {
			{MEASURE_TYPE__TWO_POINT_GIRTH, &twoPointGirth},
		};
	}
	namespace LeftArmInside {
		TWO_POINT_DEF	twoPointDef0 = {
			{5392, 0.354745f, 0.371442f},
			{21596, 0.387527f, 0.030376f}
		};
		MEASURE_DEF__TWO_POINT_GIRTH	twoPointGirthDef0(TAPE_MEASURE_MODE_WRAP, twoPointDef0);

		TWO_POINT_DEF	twoPointDef1 = {
			{21596, 0.164806f, 0.226548f},
			{23292, 0.233248f, 0.574484f}
		};
		MEASURE_DEF__TWO_POINT_GIRTH	twoPointGirthDef1(TAPE_MEASURE_MODE_WRAP, twoPointDef1);

		////
		MEASURE_DEF	defList[2] = {
			{MEASURE_TYPE__TWO_POINT_GIRTH, &twoPointGirthDef0},
			{MEASURE_TYPE__TWO_POINT_GIRTH, &twoPointGirthDef1},
		};
	}
	namespace LeftLegInside {
		TWO_POINT_DEF	twoPointDef = {
			{12725, 0.463740f, 0.006472f},
			{20027, 0.738350f, 0.159019f}
		};
		MEASURE_DEF__TWO_POINT_GIRTH	twoPointGirth(TAPE_MEASURE_MODE_WRAP, twoPointDef);

		////
		MEASURE_DEF	defList[1] = {
			{MEASURE_TYPE__TWO_POINT_GIRTH, &twoPointGirth},
		};
	}
	namespace LeftLegOutside {
		TWO_POINT_DEF	twoPointDef = {
			{13237, 0.117480f, 0.130959f},
			{19735, 0.346621f, 0.541356f}
		};
		MEASURE_DEF__TWO_POINT_GIRTH	twoPointGirth(TAPE_MEASURE_MODE_WRAP, twoPointDef);

		////
		MEASURE_DEF	defList[1] = {
			{MEASURE_TYPE__TWO_POINT_GIRTH, &twoPointGirth},
		};
	}
#endif	// 길이

	////
	MEASURE_TYPE_DEF	measureTypeDef[1 + 4] = {
#if 1	// 둘레
		{TAPE_MEASURE_BREAST,
			{
				L"가슴",
				D3DXVECTOR2(169, 104),
				D3DCOLOR_XRGB(0x00, 0x66, 0xcc),
			},
			{1, Breast::defList}
		},
#endif	// 둘레
#if 1	// 길이
		{TAPE_MEASURE_SHOULDER_WIDTH,
			{
				L"어깨넓이",
				D3DXVECTOR2(186, 45),
				D3DCOLOR_XRGB(0x00, 0x66, 0xcc),
			},
			{1, Shoulder::defList}
		},
		{TAPE_MEASURE_LEFT_ARM_INSIDE,
			{
				L"왼팔안쪽",
				D3DXVECTOR2(476, 96),
				D3DCOLOR_XRGB(0x00, 0x66, 0xcc),
			},
			{2, LeftArmInside::defList}
		},
		{TAPE_MEASURE_LEFT_LEG_INSIDE,
			{
				L"왼다리안쪽",
				D3DXVECTOR2(390, 303),
//				D3DCOLOR_XRGB(0x00, 0x66, 0xcc),
				D3DCOLOR_XRGB(0x99, 0x00, 0x00),
			},
			{1, LeftLegInside::defList}
		},
		{TAPE_MEASURE_LEFT_LEG_OUTSIDE,
			{
				L"왼다리바깥쪽",
				D3DXVECTOR2(448, 367),
//				D3DCOLOR_XRGB(0x00, 0x66, 0xcc),
				D3DCOLOR_XRGB(0x99, 0x00, 0x00),
			},
			{1, LeftLegOutside::defList}
		},
#endif	// 길이
	};
}
namespace ETRI3 {
#if 1	// 둘레
	namespace Breast {
		PLANE_DEF	planeDef = {
			{1803, 0.549906f, 0.282328f},
			D3DXVECTOR3(0, 1, 0)
		};
		MEASURE_DEF__GIRTH	girthDef(TAPE_MEASURE_MODE_WRAP, planeDef);

		////
		MEASURE_DEF	defList[1] = {
			{MEASURE_TYPE__GIRTH, &girthDef},
		};
	}
#endif	// 둘레
#if 1	// 길이
	namespace Shoulder {
		TWO_POINT_DEF	twoPointDef = {
			{6131, 0.423927f, 0.167523f},
			{8400, 0.538317f, 0.291315f}
			
		};
		MEASURE_DEF__TWO_POINT_GIRTH	twoPointGirth(TAPE_MEASURE_MODE_WRAP, twoPointDef);

		////
		MEASURE_DEF	defList[1] = {
			{MEASURE_TYPE__TWO_POINT_GIRTH, &twoPointGirth},
		};
	}
	namespace LeftArmInside {
		TWO_POINT_DEF	twoPointDef0 = {
			{7176, 0.512533f, 0.106665f},
			{12177, 0.876410f, 0.061033f}
			
		};
		MEASURE_DEF__TWO_POINT_GIRTH	twoPointGirthDef0(TAPE_MEASURE_MODE_WRAP, twoPointDef0);

		TWO_POINT_DEF	twoPointDef1 = {
			{12178, 0.153257f, 0.590860f},
			{14137, 0.594415f, 0.034974f}
			
		};
		MEASURE_DEF__TWO_POINT_GIRTH	twoPointGirthDef1(TAPE_MEASURE_MODE_WRAP, twoPointDef1);

		////
		MEASURE_DEF	defList[2] = {
			{MEASURE_TYPE__TWO_POINT_GIRTH, &twoPointGirthDef0},
			{MEASURE_TYPE__TWO_POINT_GIRTH, &twoPointGirthDef1},
		};
	}
	namespace LeftLegInside {
		TWO_POINT_DEF	twoPointDef = {
			{20167, 0.368797f, 0.121441f},
			{25763, 0.018784f, 0.955087f}
			
		};
		MEASURE_DEF__TWO_POINT_GIRTH	twoPointGirth(TAPE_MEASURE_MODE_WRAP, twoPointDef);

		////
		MEASURE_DEF	defList[1] = {
			{MEASURE_TYPE__TWO_POINT_GIRTH, &twoPointGirth},
		};
	}
	namespace LeftLegOutside {
		TWO_POINT_DEF	twoPointDef = {
			{20881, 0.747066f, 0.059687f},
			{26292, 0.130361f, 0.720586f}
			
		};
		MEASURE_DEF__TWO_POINT_GIRTH	twoPointGirth(TAPE_MEASURE_MODE_WRAP, twoPointDef);

		////
		MEASURE_DEF	defList[1] = {
			{MEASURE_TYPE__TWO_POINT_GIRTH, &twoPointGirth},
		};
	}
#endif	// 길이

	////
	MEASURE_TYPE_DEF	measureTypeDef[1 + 4] = {
#if 1	// 둘레
		{TAPE_MEASURE_BREAST,
			{
				L"가슴",
				D3DXVECTOR2(169, 104),
				D3DCOLOR_XRGB(0x00, 0x66, 0xcc),
			},
			{1, Breast::defList}
		},
#endif	// 둘레
#if 1	// 길이
		{TAPE_MEASURE_SHOULDER_WIDTH,
			{
				L"어깨넓이",
				D3DXVECTOR2(186, 45),
				D3DCOLOR_XRGB(0x00, 0x66, 0xcc),
			},
			{1, Shoulder::defList}
		},
		{TAPE_MEASURE_LEFT_ARM_INSIDE,
			{
				L"왼팔안쪽",
				D3DXVECTOR2(476, 96),
				D3DCOLOR_XRGB(0x00, 0x66, 0xcc),
			},
			{2, LeftArmInside::defList}
		},
		{TAPE_MEASURE_LEFT_LEG_INSIDE,
			{
				L"왼다리안쪽",
				D3DXVECTOR2(390, 303),
//				D3DCOLOR_XRGB(0x00, 0x66, 0xcc),
				D3DCOLOR_XRGB(0x99, 0x00, 0x00),
			},
			{1, LeftLegInside::defList}
		},
		{TAPE_MEASURE_LEFT_LEG_OUTSIDE,
			{
				L"왼다리바깥쪽",
				D3DXVECTOR2(448, 367),
//				D3DCOLOR_XRGB(0x00, 0x66, 0xcc),
				D3DCOLOR_XRGB(0x99, 0x00, 0x00),
			},
			{1, LeftLegOutside::defList}
		},
#endif	// 길이
	};
}
namespace ETRI5 {
#if 1	// 둘레
	namespace Breast {
		PLANE_DEF	planeDef = {
			{1019, 0.377114f, 0.416148f},
			D3DXVECTOR3(0, 1, 0)
		};
		MEASURE_DEF__GIRTH	girthDef(TAPE_MEASURE_MODE_WRAP, planeDef);

		////
		MEASURE_DEF	defList[1] = {
			{MEASURE_TYPE__GIRTH, &girthDef},
		};
	}
#endif	// 둘레
#if 1	// 길이
	namespace Shoulder {
		TWO_POINT_DEF	twoPointDef = {
			{5406, 0.524355f, 0.015656f},
			{8089, 0.008179f, 0.534068f}
			
		};
		MEASURE_DEF__TWO_POINT_GIRTH	twoPointGirth(TAPE_MEASURE_MODE_WRAP, twoPointDef);

		////
		MEASURE_DEF	defList[1] = {
			{MEASURE_TYPE__TWO_POINT_GIRTH, &twoPointGirth},
		};
	}
	namespace LeftArmInside {
		TWO_POINT_DEF	twoPointDef0 = {
			{5144, 0.438337f, 0.535790f},
			{11191, 0.229654f, 0.082045f}
		};
		MEASURE_DEF__TWO_POINT_GIRTH	twoPointGirthDef0(TAPE_MEASURE_MODE_WRAP, twoPointDef0);

		TWO_POINT_DEF	twoPointDef1 = {
			{11431, 0.051262f, 0.665528f},
			{37850, 0.038349f, 0.802525f}
		};
		MEASURE_DEF__TWO_POINT_GIRTH	twoPointGirthDef1(TAPE_MEASURE_MODE_WRAP, twoPointDef1);

		////
		MEASURE_DEF	defList[2] = {
			{MEASURE_TYPE__TWO_POINT_GIRTH, &twoPointGirthDef0},
			{MEASURE_TYPE__TWO_POINT_GIRTH, &twoPointGirthDef1},
		};
	}
	namespace LeftLegInside {
		TWO_POINT_DEF	twoPointDef = {
			{18873, 0.264270f, 0.697280f},
			{47688, 0.478971f, 0.403495f}
		};
		MEASURE_DEF__TWO_POINT_GIRTH	twoPointGirth(TAPE_MEASURE_MODE_WRAP, twoPointDef);

		////
		MEASURE_DEF	defList[1] = {
			{MEASURE_TYPE__TWO_POINT_GIRTH, &twoPointGirth},
		};
	}
	namespace LeftLegOutside {
		TWO_POINT_DEF	twoPointDef = {
			{18385, 0.117127f, 0.185522f},
			{52670, 0.209640f, 0.110391f}
		};
		MEASURE_DEF__TWO_POINT_GIRTH	twoPointGirth(TAPE_MEASURE_MODE_WRAP, twoPointDef);

		////
		MEASURE_DEF	defList[1] = {
			{MEASURE_TYPE__TWO_POINT_GIRTH, &twoPointGirth},
		};
	}
#endif	// 길이

	////
	MEASURE_TYPE_DEF	measureTypeDef[1 + 4] = {
#if 1	// 둘레
		{TAPE_MEASURE_BREAST,
			{
				L"가슴",
				D3DXVECTOR2(169, 104),
				D3DCOLOR_XRGB(0x00, 0x66, 0xcc),
			},
			{1, Breast::defList}
		},
#endif	// 둘레
#if 1	// 길이
		{TAPE_MEASURE_SHOULDER_WIDTH,
			{
				L"어깨넓이",
				D3DXVECTOR2(186, 45),
				D3DCOLOR_XRGB(0x00, 0x66, 0xcc),
			},
			{1, Shoulder::defList}
		},
		{TAPE_MEASURE_LEFT_ARM_INSIDE,
			{
				L"왼팔안쪽",
				D3DXVECTOR2(476, 96),
				D3DCOLOR_XRGB(0x00, 0x66, 0xcc),
			},
			{2, LeftArmInside::defList}
		},
		{TAPE_MEASURE_LEFT_LEG_INSIDE,
			{
				L"왼다리안쪽",
				D3DXVECTOR2(390, 303),
//				D3DCOLOR_XRGB(0x00, 0x66, 0xcc),
				D3DCOLOR_XRGB(0x99, 0x00, 0x00),
			},
			{1, LeftLegInside::defList}
		},
		{TAPE_MEASURE_LEFT_LEG_OUTSIDE,
			{
				L"왼다리바깥쪽",
				D3DXVECTOR2(448, 367),
//				D3DCOLOR_XRGB(0x00, 0x66, 0xcc),
				D3DCOLOR_XRGB(0x99, 0x00, 0x00),
			},
			{1, LeftLegOutside::defList}
		},
#endif	// 길이
	};
}
namespace ETRI6 {	// 에트리 표준모델 - 091211 (남; michael)
#if 1	// 둘레
#if 0
	namespace Head {
		PLANE_DEF	planeDef = {
			{15743, 0.221824f, 0.408594f},
			D3DXVECTOR3(0, 1, 0)
		};
		MEASURE_DEF__GIRTH	girthDef(TAPE_MEASURE_MODE_CONTOUR, planeDef);

		////
		MEASURE_DEF	defList[1] = {
			{MEASURE_TYPE__GIRTH, &girthDef},
		};
	}
	namespace Neck {
		PLANE_DEF	planeDef = {
			{115150, 0.242606f, 0.727244f},
			D3DXVECTOR3(0, 1, 0)
		};
		MEASURE_DEF__GIRTH	girthDef(TAPE_MEASURE_MODE_CONTOUR, planeDef);

		////
		MEASURE_DEF	defList[1] = {
			{MEASURE_TYPE__GIRTH, &girthDef},
		};
	}
#endif
	namespace Breast {
		PLANE_DEF	planeDef = {
			{3219, 0.298284f, 0.0732327f},
			D3DXVECTOR3(0, 1, 0)
		};
		MEASURE_DEF__GIRTH	girthDef(TAPE_MEASURE_MODE_WRAP, planeDef);

		////
		MEASURE_DEF	defList[1] = {
			{MEASURE_TYPE__GIRTH, &girthDef},
		};
	}
	namespace Waist {
		PLANE_DEF	planeDef = {
			{17112, 0.328969f, 0.470845f},
			D3DXVECTOR3(0, 1, 0)
		};
		MEASURE_DEF__GIRTH	girthDef(TAPE_MEASURE_MODE_CONTOUR, planeDef);

		////
		MEASURE_DEF	defList[1] = {
			{MEASURE_TYPE__GIRTH, &girthDef},
		};
	}
	namespace Hip {
		PLANE_DEF	planeDef = {
			{20390, 0.0215536f, 0.657598f},
			D3DXVECTOR3(0, 1, 0)
		};
		MEASURE_DEF__GIRTH	girthDef(TAPE_MEASURE_MODE_WRAP, planeDef);

		////
		MEASURE_DEF	defList[1] = {
			{MEASURE_TYPE__GIRTH, &girthDef},
		};
	}
	namespace LeftThigh {
		PLANE_DEF	planeDef = {
			{19676, 0.330036f, 0.499293f},
			D3DXVECTOR3(0, 1, 0)
		};
		MEASURE_DEF__GIRTH	girthDef(TAPE_MEASURE_MODE_CONTOUR, planeDef);

		////
		MEASURE_DEF	defList[1] = {
			{MEASURE_TYPE__GIRTH, &girthDef},
		};
	}
	namespace RightThigh {
		PLANE_DEF	planeDef = {
			{18106, 0.724477f, 0.23433f},
			D3DXVECTOR3(0, 1, 0)
		};
		MEASURE_DEF__GIRTH	girthDef(TAPE_MEASURE_MODE_CONTOUR, planeDef);

		////
		MEASURE_DEF	defList[1] = {
			{MEASURE_TYPE__GIRTH, &girthDef},
		};
	}
	namespace LeftForeArm {
		PLANE_DEF	planeDef = {
			{6977, 0.632041f, 0.0502328f},
			D3DXVECTOR3(-0.956910f, 0.279197f, -0.079870f)	// 사용하지 않음; 아래의 FACEID_LIST 가 사용
		};
		FACEID_LIST	faceID_list = {1389, ETRI6::leftForearm};
		MEASURE_DEF__GIRTH	girthDef(TAPE_MEASURE_MODE_CONTOUR, planeDef, &faceID_list);

		////
		MEASURE_DEF	defList[1] = {
			{MEASURE_TYPE__GIRTH, &girthDef},
		};
	}
	namespace RightForeArm {
		PLANE_DEF	planeDef = {
			{9304, 0.0414435f, 0.273586f},
			D3DXVECTOR3(-0.971231f, -0.143522f, -0.190048f)	// 사용하지 않음; 아래의 FACEID_LIST 가 사용
		};
		FACEID_LIST	faceID_list = {1385, ETRI6::rightForearm};
		MEASURE_DEF__GIRTH	girthDef(TAPE_MEASURE_MODE_CONTOUR, planeDef, &faceID_list);

		////
		MEASURE_DEF	defList[1] = {
			{MEASURE_TYPE__GIRTH, &girthDef},
		};
	}
#endif	// 둘레
#if 1	// 길이
	namespace Height {
#if 1
		TWO_PLANE_DEF	twoPlaneDef = {
			{				// 목젖
				{248, 0.41579f, 0.253372f},
				D3DXVECTOR3(0.086850f, 0.996197f, 0.007047f)	// 사용하지 않음; 아래의 FACEID_LIST 가 사용
			},
			{				// 골반
				{17244, 0.530352f, 0.137067f},
				D3DXVECTOR3(0.086850f, 0.996197f, 0.007047f)	// 사용하지 않음; 아래의 FACEID_LIST 가 사용
			}
		};
		FACEID_LIST	faceID_list = {5393, ETRI6::trunk};
		MEASURE_DEF__TWO_PLANE_DISTANCE	twoPlaneDistanceDef(twoPlaneDef, &faceID_list);

		TWO_PLANE_DEF	twoPlaneDef2 = {
			{				// 골반
				{17244, 0.530352f, 0.137067f},
				D3DXVECTOR3(0.086850f, 0.996197f, 0.007047f)	// 사용하지 않음; 아래의 FACEID_LIST 가 사용
			},
			{				// 무릎
				{26040, 0.127325f, 0.649684f},
				D3DXVECTOR3(0.086850f, 0.996197f, 0.007047f)	// 사용하지 않음; 아래의 FACEID_LIST 가 사용
			}
		};
		FACEID_LIST	faceID_list2 = {985, ETRI6::leftThigh};
		MEASURE_DEF__TWO_PLANE_DISTANCE	twoPlaneDistanceDef2(twoPlaneDef2, &faceID_list2);

		TWO_PLANE_DEF	twoPlaneDef3 = {
			{				// 무릎
				{26040, 0.127325f, 0.649684f},
				D3DXVECTOR3(0.086850f, 0.996197f, 0.007047f)	// 사용하지 않음; 아래의 FACEID_LIST 가 사용
			},
			{				// 발목
				{25677, 0.733951f, 0.0525352f},
				D3DXVECTOR3(0.086850f, 0.996197f, 0.007047f)	// 사용하지 않음; 아래의 FACEID_LIST 가 사용
			}
		};
		FACEID_LIST	faceID_list3 = {656, ETRI6::leftCalf};
		MEASURE_DEF__TWO_PLANE_DISTANCE	twoPlaneDistanceDef3(twoPlaneDef3, &faceID_list3);

		////
		MEASURE_DEF	defList[3] = {
			{MEASURE_TYPE__TWO_PLANE_DISTANCE, &twoPlaneDistanceDef},
			{MEASURE_TYPE__TWO_PLANE_DISTANCE, &twoPlaneDistanceDef2},
			{MEASURE_TYPE__TWO_PLANE_DISTANCE, &twoPlaneDistanceDef3},
		};
#else
		POINT_PLANE_DEF	pointPlaneDef1 = {
			{20790, 0.025643f, 0.962431f},	// 정수리
			{				// 골반
				{62710, 0.448102f, 0.258171f},
				D3DXVECTOR3(0, 1, 0)
			}
		};
		MEASURE_DEF__POINT_PLANE_DISTANCE	pointPlaneDistanceDef1(pointPlaneDef1);

		TWO_PLANE_DEF	twoPlaneDef = {
			{				// 골반
				{65847, 0.015407f, 0.824872f},
				D3DXVECTOR3(0.086850f, 0.996197f, 0.007047f)
			},
			{				// 오른쪽 무릎
				{72432, 0.102591f, 0.257141f},
				D3DXVECTOR3(0.086850f, 0.996197f, 0.007047f)
			}
		};
		MEASURE_DEF__TWO_PLANE_DISTANCE	twoPlaneDistanceDef(twoPlaneDef);

		POINT_PLANE_DEF	pointPlaneDef2 = {
			{73637, 0.016866f, 0.185059f},	// 오른쪽 발뒤꿈치
			{				// 오른쪽 무릎
				{72432, 0.102591f, 0.257141f},
				D3DXVECTOR3(0.086850f, 0.996197f, 0.007047f)
			},
		};
		MEASURE_DEF__POINT_PLANE_DISTANCE	pointPlaneDistanceDef2(pointPlaneDef2);

		////
		MEASURE_DEF	defList[3] = {
			{MEASURE_TYPE__POINT_PLANE_DISTANCE, &pointPlaneDistanceDef1},
			{MEASURE_TYPE__TWO_PLANE_DISTANCE, &twoPlaneDistanceDef},
			{MEASURE_TYPE__POINT_PLANE_DISTANCE, &pointPlaneDistanceDef2},
		};
#endif
	}
	namespace SeatedHeight {
#if 1
		TWO_PLANE_DEF	twoPlaneDef = {
			{				// 목젖
				{248, 0.41579f, 0.253372f},
				D3DXVECTOR3(0.086850f, 0.996197f, 0.007047f)	// 사용하지 않음; 아래의 FACEID_LIST 가 사용
			},
			{				// 골반
				{17244, 0.530352f, 0.137067f},
				D3DXVECTOR3(0.086850f, 0.996197f, 0.007047f)	// 사용하지 않음; 아래의 FACEID_LIST 가 사용
			}
		};
		FACEID_LIST	faceID_list = {5393, ETRI6::trunk};
		MEASURE_DEF__TWO_PLANE_DISTANCE	twoPlaneDistanceDef(twoPlaneDef, &faceID_list);

		////
		MEASURE_DEF	defList[1] = {
			{MEASURE_TYPE__TWO_PLANE_DISTANCE, &twoPlaneDistanceDef},
		};
#else
		POINT_PLANE_DEF	pointPlaneDef = {
			{20790, 0.025643f, 0.962431f},	// 정수리
			{				// 골반
				{62710, 0.448102f, 0.258171f},
				D3DXVECTOR3(0, 1, 0)
			}
		};
		MEASURE_DEF__POINT_PLANE_DISTANCE	pointPlaneDistanceDef(pointPlaneDef);

		////
		MEASURE_DEF	defList[1] = {
			{MEASURE_TYPE__POINT_PLANE_DISTANCE, &pointPlaneDistanceDef},
		};
#endif
	}
	namespace Shoulder {
		TWO_POINT_DEF	twoPointDef = {
			{6423, 0.375037f, 0.309597f},
			{7923, 0.066051f, 0.158260f}
		};
		MEASURE_DEF__TWO_POINT_GIRTH	twoPointGirth(TAPE_MEASURE_MODE_WRAP, twoPointDef);

		////
		MEASURE_DEF	defList[1] = {
			{MEASURE_TYPE__TWO_POINT_GIRTH, &twoPointGirth},
		};
	}
	namespace LeftLeg {
		TWO_PLANE_DEF	twoPlaneDef = {
			{	// 허벅지 윗부분
				{19630, 0.618375f, 0.0171079f},
				D3DXVECTOR3(-0.025932f, 0.984654f, -0.172584f)
			},
			{	// 무릎
				{26059, 0.602148f, 0.147028f},
				D3DXVECTOR3(-0.025932f, 0.984654f, -0.172584f)
			}
		};
#if 1
		FACEID_LIST	faceID_list = {985, ETRI6::leftThigh};
		MEASURE_DEF__TWO_PLANE_DISTANCE	twoPlaneDistanceDef(twoPlaneDef, &faceID_list);
#else
		MEASURE_DEF__TWO_PLANE_DISTANCE	twoPlaneDistanceDef(twoPlaneDef);
#endif

#if 1
		TWO_PLANE_DEF	twoPlaneDef2 = {
			{	// 무릎
				{26059, 0.602148f, 0.147028f},
				D3DXVECTOR3(-0.025932f, 0.984654f, -0.172584f)
			},
			{	// 발목
				{25677, 0.873043f, 0.123484f},
				D3DXVECTOR3(-0.025932f, 0.984654f, -0.172584f)
			}
		};
		FACEID_LIST	faceID_list2 = {656, ETRI6::leftCalf};
		MEASURE_DEF__TWO_PLANE_DISTANCE	twoPlaneDistanceDef2(twoPlaneDef2, &faceID_list2);

		////
		MEASURE_DEF	defList[2] = {
			{MEASURE_TYPE__TWO_PLANE_DISTANCE, &twoPlaneDistanceDef},
			{MEASURE_TYPE__TWO_PLANE_DISTANCE, &twoPlaneDistanceDef2},
		};
#else
		POINT_PLANE_DEF	pointPlaneDef = {
			{74634, 0.960823f, 0.020719f},	// 발뒤꿈치
			{	// 무릎
				{71314, 0.424358f, 0.008220f},
				D3DXVECTOR3(-0.025932f, 0.984654f, -0.172584f)
			},
		};
		MEASURE_DEF__POINT_PLANE_DISTANCE	pointPlaneDistanceDef(pointPlaneDef);

		////
		MEASURE_DEF	defList[2] = {
			{MEASURE_TYPE__TWO_PLANE_DISTANCE, &twoPlaneDistanceDef},
			{MEASURE_TYPE__POINT_PLANE_DISTANCE, &pointPlaneDistanceDef},
		};
#endif
	}
	namespace RightLeg {
		TWO_PLANE_DEF	twoPlaneDef = {
			{	// 허벅지 윗부분
				{18165, 0.499332f, 0.260509f},
				D3DXVECTOR3(-0.025932f, 0.984654f, -0.172584f)	// 사용하지 않음; 아래의 FACEID_LIST 가 사용
			},
			{	// 무릎
				{24730, 0.150194f, 0.249279f},
				D3DXVECTOR3(-0.025932f, 0.984654f, -0.172584f)	// 사용하지 않음; 아래의 FACEID_LIST 가 사용
			}
		};
#if 1
		FACEID_LIST	faceID_list = {1044, ETRI6::rightThigh};
		MEASURE_DEF__TWO_PLANE_DISTANCE	twoPlaneDistanceDef(twoPlaneDef);
#else
		MEASURE_DEF__TWO_PLANE_DISTANCE	twoPlaneDistanceDef(twoPlaneDef);
#endif

#if 1
		TWO_PLANE_DEF	twoPlaneDef2 = {
			{	// 무릎
				{24730, 0.150194f, 0.249279f},
				D3DXVECTOR3(-0.025932f, 0.984654f, -0.172584f)	// 사용하지 않음; 아래의 FACEID_LIST 가 사용
			},
			{	// 발목
				{24470, 0.8025f, 0.169199f},
				D3DXVECTOR3(-0.025932f, 0.984654f, -0.172584f)	// 사용하지 않음; 아래의 FACEID_LIST 가 사용
			}
		};
		FACEID_LIST	faceID_list2 = {629, ETRI6::rightCalf};
		MEASURE_DEF__TWO_PLANE_DISTANCE	twoPlaneDistanceDef2(twoPlaneDef2, &faceID_list2);

		////
		MEASURE_DEF	defList[2] = {
			{MEASURE_TYPE__TWO_PLANE_DISTANCE, &twoPlaneDistanceDef},
			{MEASURE_TYPE__TWO_PLANE_DISTANCE, &twoPlaneDistanceDef2},
		};
#else
		POINT_PLANE_DEF	pointPlaneDef = {
			{73684, 0.008145f, 0.150210f},	// 발뒤꿈치
			{	// 무릎
				{72746, 0.530067f, 0.334535f},
				D3DXVECTOR3(-0.025932f, 0.984654f, -0.172584f)
			},
		};
		MEASURE_DEF__POINT_PLANE_DISTANCE	pointPlaneDistanceDef(pointPlaneDef);

		////
		MEASURE_DEF	defList[2] = {
			{MEASURE_TYPE__TWO_PLANE_DISTANCE, &twoPlaneDistanceDef},
			{MEASURE_TYPE__POINT_PLANE_DISTANCE, &pointPlaneDistanceDef},
		};
#endif
	}
	namespace LeftArm {
		TWO_PLANE_DEF	twoPlaneDef1 = {
			{	// 팔 윗부분
				{6494, 0.766627f, 0.0753414f},
				D3DXVECTOR3(-0.998316f, 0.058097f, 0.000468f)	// 사용하지 않음; 아래의 FACEID_LIST 가 사용
			},
			{	// 팔꿈치
				{12666, 0.686383f, 0.0740625f},
				D3DXVECTOR3(-0.998316f, 0.058097f, 0.000468f)	// 사용하지 않음; 아래의 FACEID_LIST 가 사용
			}
		};
		FACEID_LIST	faceID_list1 = {1389, ETRI6::leftForearm};
		MEASURE_DEF__TWO_PLANE_DISTANCE	twoPlaneDistanceDef1(twoPlaneDef1, &faceID_list1);

		TWO_PLANE_DEF	twoPlaneDef2 = {
			{	// 팔꿈치
				{12666, 0.686383f, 0.0740625f},
				D3DXVECTOR3(-0.931428f, 0.061501f, 0.358703f)	// 사용하지 않음; 아래의 FACEID_LIST 가 사용
			},
			{	// 손목
				{13509, 0.767639f, 0.0430536f},
				D3DXVECTOR3(-0.931428f, 0.061501f, 0.358703f)	// 사용하지 않음; 아래의 FACEID_LIST 가 사용
			}
		};
		FACEID_LIST	faceID_list2 = {1184, ETRI6::leftLowerArm};
		MEASURE_DEF__TWO_PLANE_DISTANCE	twoPlaneDistanceDef2(twoPlaneDef2, &faceID_list2);

		////
		MEASURE_DEF	defList[2] = {
			{MEASURE_TYPE__TWO_PLANE_DISTANCE, &twoPlaneDistanceDef1},
			{MEASURE_TYPE__TWO_PLANE_DISTANCE, &twoPlaneDistanceDef2},
		};
	}
	namespace RightArm {
		TWO_PLANE_DEF	twoPlaneDef1 = {
			{	// 팔 위부분
				{8751, 0.810709f, 0.108714f},
				D3DXVECTOR3(-0.970453f, -0.220734f, -0.097510f)	// 사용하지 않음; 아래의 FACEID_LIST 가 사용
			},
			{	// 팔꿈치
				{10588, 0.16868f, 0.124777f},
				D3DXVECTOR3(-0.970453f, -0.220734f, -0.097510f)	// 사용하지 않음; 아래의 FACEID_LIST 가 사용
			}
		};
		FACEID_LIST	faceID_list1 = {1385, ETRI6::rightForearm};
		MEASURE_DEF__TWO_PLANE_DISTANCE	twoPlaneDistanceDef1(twoPlaneDef1, &faceID_list1);

		TWO_PLANE_DEF	twoPlaneDef2 = {
			{	// 팔꿈치
				{10588, 0.16868f, 0.124777f},
				D3DXVECTOR3(-0.964311f, -0.052404f, -0.259560f)	// 사용하지 않음; 아래의 FACEID_LIST 가 사용
			},
			{	// 손목
				{11383, 0.567338f, 0.216063f},
				D3DXVECTOR3(-0.964311f, -0.052404f, -0.259560f)	// 사용하지 않음; 아래의 FACEID_LIST 가 사용
			}
		};
		FACEID_LIST	faceID_list2 = {798, ETRI6::rightLowerArm};
		MEASURE_DEF__TWO_PLANE_DISTANCE	twoPlaneDistanceDef2(twoPlaneDef2, &faceID_list2);

		////
		MEASURE_DEF	defList[2] = {
			{MEASURE_TYPE__TWO_PLANE_DISTANCE, &twoPlaneDistanceDef1},
			{MEASURE_TYPE__TWO_PLANE_DISTANCE, &twoPlaneDistanceDef2},
		};
	}
#if 0
	namespace LeftArmInside {
		TWO_POINT_DEF	twoPointDef0 = {
			{56950, 0.556603f, 0.268797f},
			{81040, 0.561017f, 0.294061f}
		};
		MEASURE_DEF__TWO_POINT_GIRTH	twoPointGirthDef0(TAPE_MEASURE_MODE_WRAP, twoPointDef0);

		TWO_POINT_DEF	twoPointDef1 = {
			{82198, 0.325851f, 0.469118f},
			{81040, 0.561017f, 0.294061f}
		};
		MEASURE_DEF__TWO_POINT_GIRTH	twoPointGirthDef1(TAPE_MEASURE_MODE_WRAP, twoPointDef1);

		////
		MEASURE_DEF	defList[2] = {
			{MEASURE_TYPE__TWO_POINT_GIRTH, &twoPointGirthDef0},
			{MEASURE_TYPE__TWO_POINT_GIRTH, &twoPointGirthDef1},
		};
	}
	namespace LeftArmOutside {
		TWO_POINT_DEF	twoPointDef0 = {
			{56540, 0.439220f, 0.528292f},
			{81116, 0.567838f, 0.280615f}
		};
		MEASURE_DEF__TWO_POINT_GIRTH	twoPointGirthDef0(TAPE_MEASURE_MODE_WRAP, twoPointDef0);

		TWO_POINT_DEF	twoPointDef1 = {
			{81638, 0.585758f, 0.282053f},
			{81116, 0.567838f, 0.280615f}
		};
		MEASURE_DEF__TWO_POINT_GIRTH	twoPointGirthDef1(TAPE_MEASURE_MODE_WRAP, twoPointDef1);

		////
		MEASURE_DEF	defList[2] = {
			{MEASURE_TYPE__TWO_POINT_GIRTH, &twoPointGirthDef0},
			{MEASURE_TYPE__TWO_POINT_GIRTH, &twoPointGirthDef1},
		};
	}
	namespace RightArmInside {
		TWO_POINT_DEF	twoPointDef0 = {
			{59046, 0.097483f, 0.517813f},
			{82458, 0.096169f, 0.430583f}
		};
		MEASURE_DEF__TWO_POINT_GIRTH	twoPointGirthDef0(TAPE_MEASURE_MODE_WRAP, twoPointDef0);

		TWO_POINT_DEF	twoPointDef1 = {
			{84733, 0.087797f, 0.654378f},
			{82458, 0.096169f, 0.430583f}
		};
		MEASURE_DEF__TWO_POINT_GIRTH	twoPointGirthDef1(TAPE_MEASURE_MODE_WRAP, twoPointDef1);

		////
		MEASURE_DEF	defList[2] = {
			{MEASURE_TYPE__TWO_POINT_GIRTH, &twoPointGirthDef0},
			{MEASURE_TYPE__TWO_POINT_GIRTH, &twoPointGirthDef1},
		};
	}
	namespace RightArmOutside {
		TWO_POINT_DEF	twoPointDef0 = {
			{59301, 0.006432f, 0.615452f},
			{83625, 0.448489f, 0.026192f}
		};
		MEASURE_DEF__TWO_POINT_GIRTH	twoPointGirthDef0(TAPE_MEASURE_MODE_WRAP, twoPointDef0);

		TWO_POINT_DEF	twoPointDef1 = {
			{84828, 0.265014f, 0.658732f},
			{83625, 0.448489f, 0.026192f}
		};
		MEASURE_DEF__TWO_POINT_GIRTH	twoPointGirthDef1(TAPE_MEASURE_MODE_WRAP, twoPointDef1);

		////
		MEASURE_DEF	defList[2] = {
			{MEASURE_TYPE__TWO_POINT_GIRTH, &twoPointGirthDef0},
			{MEASURE_TYPE__TWO_POINT_GIRTH, &twoPointGirthDef1},
		};
	}
	namespace LeftLegInside {
		TWO_POINT_DEF	twoPointDef = {
//			{64476, 0.321285f, 0.668669f},
//			{74333, 0.221292f, 0.665281f}
			{64379, 0.260959f, 0.429566f},
			{74151, 0.237487f, 0.348736f}
		};
		MEASURE_DEF__TWO_POINT_GIRTH	twoPointGirth(TAPE_MEASURE_MODE_WRAP, twoPointDef);

		////
		MEASURE_DEF	defList[1] = {
			{MEASURE_TYPE__TWO_POINT_GIRTH, &twoPointGirth},
		};
	}
	namespace LeftLegOutside {
		TWO_POINT_DEF	twoPointDef = {
//			{64261, 0.598616f, 0.322904f},
//			{74678, 0.226481f, 0.352553f}
			{63754, 0.831608f, 0.088129f},
			{74125, 0.783423f, 0.183948f}
		};
		MEASURE_DEF__TWO_POINT_GIRTH	twoPointGirth(TAPE_MEASURE_MODE_WRAP, twoPointDef);

		////
		MEASURE_DEF	defList[1] = {
			{MEASURE_TYPE__TWO_POINT_GIRTH, &twoPointGirth},
		};
	}
	namespace RightLegInside {
		TWO_POINT_DEF	twoPointDef = {
			{66209, 0.259354f, 0.478601f},
			{73868, 0.154951f, 0.400970f}
		};
		MEASURE_DEF__TWO_POINT_GIRTH	twoPointGirth(TAPE_MEASURE_MODE_WRAP, twoPointDef);

		////
		MEASURE_DEF	defList[1] = {
			{MEASURE_TYPE__TWO_POINT_GIRTH, &twoPointGirth},
		};
	}
	namespace RightLegOutside {
		TWO_POINT_DEF	twoPointDef = {
			{73721, 0.485668f, 0.005433f},
			{65561, 0.382790f, 0.059710f}
		};
		MEASURE_DEF__TWO_POINT_GIRTH	twoPointGirth(TAPE_MEASURE_MODE_WRAP, twoPointDef);

		////
		MEASURE_DEF	defList[1] = {
			{MEASURE_TYPE__TWO_POINT_GIRTH, &twoPointGirth},
		};
	}
#endif
#endif	// 길이

	////
	MEASURE_TYPE_DEF	measureTypeDef[9 - 2 + 15 - 8] = {
#if 1	// 둘레
#if 0
		{TAPE_MEASURE_HEAD,
			{
				L"머리",
				D3DXVECTOR2(373, 27),
				D3DCOLOR_XRGB(0x99, 0x00, 0x00),
			},
			{1, Head::defList}
		},
		{TAPE_MEASURE_NECK,
			{
				L"목",
				D3DXVECTOR2(425, 48),
				D3DCOLOR_XRGB(0x00, 0x66, 0xcc),
			},
			{1, Neck::defList}
		},
#endif
		{TAPE_MEASURE_BREAST,
			{
				L"가슴",
				D3DXVECTOR2(169, 104),
				D3DCOLOR_XRGB(0x00, 0x66, 0xcc),
			},
			{1, Breast::defList}
		},
		{TAPE_MEASURE_WAIST,
			{
				L"허리",
				D3DXVECTOR2(174, 139),
				D3DCOLOR_XRGB(0x00, 0x66, 0xcc),
			},
			{1, Waist::defList}
		},
		{TAPE_MEASURE_HIP,
			{
				L"엉덩이",
				D3DXVECTOR2(392, 119),
				D3DCOLOR_XRGB(0x00, 0x66, 0xcc),
			},
			{1, Hip::defList}
		},
		{TAPE_MEASURE_LEFT_THIGH,
			{
				L"왼쪽허벅지",
				D3DXVECTOR2(386, 186),
				D3DCOLOR_XRGB(0x00, 0x66, 0xcc),
			},
			{1, LeftThigh::defList}
		},
		{TAPE_MEASURE_RIGHT_THIGH,
			{
				L"오른쪽허벅지",
				D3DXVECTOR2(237, 186),
				D3DCOLOR_XRGB(0x00, 0x66, 0xcc),
			},
			{1, RightThigh::defList}
		},
		{TAPE_MEASURE_LEFT_FOREARM,
			{
				L"왼쪽상완",
				D3DXVECTOR2(393, 105),
				D3DCOLOR_XRGB(0x00, 0x66, 0xcc),
			},
			{1, LeftForeArm::defList}
		},
		{TAPE_MEASURE_RIGHT_FOREARM,
			{
				L"오른쪽상완",
				D3DXVECTOR2(195, 96),
				D3DCOLOR_XRGB(0x00, 0x66, 0xcc),
			},
			{1, RightForeArm::defList}
		},
#endif	// 둘레
#if 1	// 길이
		{TAPE_MEASURE_HEIGHT,
			{
				L"키",
				D3DXVECTOR2(140, 240),
				D3DCOLOR_XRGB(0x00, 0x66, 0xcc),
			},
			{3, Height::defList}
		},
		{TAPE_MEASURE_SEATED_HEIGHT,
			{
				L"앉은키",
				D3DXVECTOR2(449, 139),
				D3DCOLOR_XRGB(0x00, 0x66, 0xcc),
			},
			{1, SeatedHeight::defList}
		},
		{TAPE_MEASURE_SHOULDER_WIDTH,
			{
				L"어깨넓이",
				D3DXVECTOR2(186, 45),
				D3DCOLOR_XRGB(0x00, 0x66, 0xcc),
			},
			{1, Shoulder::defList}
		},
		{TAPE_MEASURE_LEFT_LEG,
			{
				L"왼다리",
				D3DXVECTOR2(393, 197),
				D3DCOLOR_XRGB(0x00, 0x66, 0xcc),
			},
			{2, LeftLeg::defList}
		},
		{TAPE_MEASURE_RIGHT_LEG,
			{
				L"오른다리",
				D3DXVECTOR2(225, 191),
				D3DCOLOR_XRGB(0x00, 0x66, 0xcc),
			},
			{2, RightLeg::defList}
		},
		{TAPE_MEASURE_LEFT_ARM,
			{
				L"왼팔",
				D3DXVECTOR2(439, 110),
				D3DCOLOR_XRGB(0x00, 0x66, 0xcc),
			},
			{2, LeftArm::defList}
		},
		{TAPE_MEASURE_RIGHT_ARM,
			{
				L"오른팔",
				D3DXVECTOR2(157, 109),
				D3DCOLOR_XRGB(0x00, 0x66, 0xcc),
			},
			{2, RightArm::defList}
		},
#if 0
		{TAPE_MEASURE_LEFT_ARM_INSIDE,
			{
				L"왼팔안쪽",
				D3DXVECTOR2(476, 96),
				D3DCOLOR_XRGB(0x00, 0x66, 0xcc),
			},
			{2, LeftArmInside::defList}
		},
		{TAPE_MEASURE_LEFT_ARM_OUTSIDE,
			{
				L"왼팔바깥쪽",
				D3DXVECTOR2(149, 99),
				D3DCOLOR_XRGB(0x00, 0x66, 0xcc),
			},
			{2, LeftArmOutside::defList}
		},
		{TAPE_MEASURE_RIGHT_ARM_INSIDE,
			{
				L"오른팔안쪽",
				D3DXVECTOR2(469, 49),
				D3DCOLOR_XRGB(0x00, 0x66, 0xcc),
			},
			{2, RightArmInside::defList}
		},
		{TAPE_MEASURE_RIGHT_ARM_OUTSIDE,
			{
				L"오른팔바깥쪽",
				D3DXVECTOR2(153, 52),
				D3DCOLOR_XRGB(0x00, 0x66, 0xcc),
			},
			{2, RightArmOutside::defList}
		},
		{TAPE_MEASURE_LEFT_LEG_INSIDE,
			{
				L"왼다리안쪽",
				D3DXVECTOR2(390, 303),
//				D3DCOLOR_XRGB(0x00, 0x66, 0xcc),
				D3DCOLOR_XRGB(0x99, 0x00, 0x00),
			},
			{1, LeftLegInside::defList}
		},
		{TAPE_MEASURE_LEFT_LEG_OUTSIDE,
			{
				L"왼다리바깥쪽",
				D3DXVECTOR2(448, 367),
//				D3DCOLOR_XRGB(0x00, 0x66, 0xcc),
				D3DCOLOR_XRGB(0x99, 0x00, 0x00),
			},
			{1, LeftLegOutside::defList}
		},
		{TAPE_MEASURE_RIGHT_LEG_INSIDE,
			{
				L"오른다리안쪽",
				D3DXVECTOR2(213, 303),
				D3DCOLOR_XRGB(0x00, 0x66, 0xcc),
			},
			{1, RightLegInside::defList}
		},
		{TAPE_MEASURE_RIGHT_LEG_OUTSIDE,
			{
				L"오른다리바깥쪽",
				D3DXVECTOR2(164, 350),
				D3DCOLOR_XRGB(0x00, 0x66, 0xcc),
			},
			{1, RightLegOutside::defList}
		},
#endif
#endif	// 길이
	};
}
namespace ETRI7 {	// 에트리 표준모델 - 091211 (여; victoria)
#if 1	// 둘레
#if 0
	namespace Head {
		PLANE_DEF	planeDef = {
			{15743, 0.221824f, 0.408594f},
			D3DXVECTOR3(0, 1, 0)
		};
		MEASURE_DEF__GIRTH	girthDef(TAPE_MEASURE_MODE_CONTOUR, planeDef);

		////
		MEASURE_DEF	defList[1] = {
			{MEASURE_TYPE__GIRTH, &girthDef},
		};
	}
	namespace Neck {
		PLANE_DEF	planeDef = {
			{115150, 0.242606f, 0.727244f},
			D3DXVECTOR3(0, 1, 0)
		};
		MEASURE_DEF__GIRTH	girthDef(TAPE_MEASURE_MODE_CONTOUR, planeDef);

		////
		MEASURE_DEF	defList[1] = {
			{MEASURE_TYPE__GIRTH, &girthDef},
		};
	}
#endif
	namespace Breast {
		PLANE_DEF	planeDef = {
			{6307, 0.315596f, 0.389145f},
			D3DXVECTOR3(0, 1, 0)
		};
		MEASURE_DEF__GIRTH	girthDef(TAPE_MEASURE_MODE_WRAP, planeDef);

		////
		MEASURE_DEF	defList[1] = {
			{MEASURE_TYPE__GIRTH, &girthDef},
		};
	}
	namespace Waist {
		PLANE_DEF	planeDef = {
			{9965, 0.375689f, 0.083971f},
			D3DXVECTOR3(0, 1, 0)
		};
		MEASURE_DEF__GIRTH	girthDef(TAPE_MEASURE_MODE_CONTOUR, planeDef);

		////
		MEASURE_DEF	defList[1] = {
			{MEASURE_TYPE__GIRTH, &girthDef},
		};
	}
	namespace Hip {
		PLANE_DEF	planeDef = {
			{13543, 0.0419152f, 0.248122f},
			D3DXVECTOR3(0, 1, 0)
		};
		MEASURE_DEF__GIRTH	girthDef(TAPE_MEASURE_MODE_WRAP, planeDef);

		////
		MEASURE_DEF	defList[1] = {
			{MEASURE_TYPE__GIRTH, &girthDef},
		};
	}
	namespace LeftThigh {
		PLANE_DEF	planeDef = {
			{12227, 0.305637f, 0.241448f},
			D3DXVECTOR3(0, 1, 0)
		};
		MEASURE_DEF__GIRTH	girthDef(TAPE_MEASURE_MODE_CONTOUR, planeDef);

		////
		MEASURE_DEF	defList[1] = {
			{MEASURE_TYPE__GIRTH, &girthDef},
		};
	}
	namespace RightThigh {
		PLANE_DEF	planeDef = {
			{14846, 0.00922211f, 0.862489f},
			D3DXVECTOR3(0, 1, 0)
		};
		MEASURE_DEF__GIRTH	girthDef(TAPE_MEASURE_MODE_CONTOUR, planeDef);

		////
		MEASURE_DEF	defList[1] = {
			{MEASURE_TYPE__GIRTH, &girthDef},
		};
	}
	namespace LeftForeArm {
		PLANE_DEF	planeDef = {
			{5378, 0.00963829f, 0.899919f},
			D3DXVECTOR3(-0.989238f, 0.119946f, 0.0837941f)	// 사용하지 않음; 아래의 FACEID_LIST 가 사용
		};
		FACEID_LIST	faceID_list = {1704, ETRI7::leftForearm};
		MEASURE_DEF__GIRTH	girthDef(TAPE_MEASURE_MODE_CONTOUR, planeDef, &faceID_list);

		////
		MEASURE_DEF	defList[1] = {
			{MEASURE_TYPE__GIRTH, &girthDef},
		};
	}
	namespace RightForeArm {
		PLANE_DEF	planeDef = {
			{8153, 0.0273286f, 0.332326f},
			D3DXVECTOR3(-0.982748f, 0.0781478f, -0.167626f)	// 사용하지 않음; 아래의 FACEID_LIST 가 사용
		};
		FACEID_LIST	faceID_list = {1687, ETRI7::rightForearm};
		MEASURE_DEF__GIRTH	girthDef(TAPE_MEASURE_MODE_CONTOUR, planeDef, &faceID_list);

		////
		MEASURE_DEF	defList[1] = {
			{MEASURE_TYPE__GIRTH, &girthDef},
		};
	}
#endif	// 둘레
#if 1	// 길이
	namespace Height {
#if 1
		TWO_PLANE_DEF	twoPlaneDef = {
			{				// 목젖
				{26323, 0.0868376f, 0.0711014f},
				D3DXVECTOR3(0.086850f, 0.996197f, 0.007047f)	// 사용하지 않음; 아래의 FACEID_LIST 가 사용
			},
			{				// 골반
				{11496, 0.382994f, 0.6125f},
				D3DXVECTOR3(0.086850f, 0.996197f, 0.007047f)	// 사용하지 않음; 아래의 FACEID_LIST 가 사용
			}
		};
		FACEID_LIST	faceID_list = {7073, ETRI7::trunk};
		MEASURE_DEF__TWO_PLANE_DISTANCE	twoPlaneDistanceDef(twoPlaneDef, &faceID_list);

		TWO_PLANE_DEF	twoPlaneDef2 = {
			{				// 골반
				{11496, 0.382994f, 0.6125f},
				D3DXVECTOR3(0.086850f, 0.996197f, 0.007047f)	// 사용하지 않음; 아래의 FACEID_LIST 가 사용
			},
			{				// 무릎
				{19585, 0.0600276f, 0.0446881f},
				D3DXVECTOR3(0.086850f, 0.996197f, 0.007047f)	// 사용하지 않음; 아래의 FACEID_LIST 가 사용
			}
		};
		FACEID_LIST	faceID_list2 = {1282, ETRI7::leftThigh};
		MEASURE_DEF__TWO_PLANE_DISTANCE	twoPlaneDistanceDef2(twoPlaneDef2, &faceID_list2);

		TWO_PLANE_DEF	twoPlaneDef3 = {
			{				// 무릎
				{19585, 0.0600276f, 0.0446881f},
				D3DXVECTOR3(0.086850f, 0.996197f, 0.007047f)	// 사용하지 않음; 아래의 FACEID_LIST 가 사용
			},
			{				// 발목
				{18713, 0.200654f, 0.346868f},
				D3DXVECTOR3(0.086850f, 0.996197f, 0.007047f)	// 사용하지 않음; 아래의 FACEID_LIST 가 사용
			}
		};
		FACEID_LIST	faceID_list3 = {740, ETRI7::leftCalf};
		MEASURE_DEF__TWO_PLANE_DISTANCE	twoPlaneDistanceDef3(twoPlaneDef3, &faceID_list3);

		////
		MEASURE_DEF	defList[3] = {
			{MEASURE_TYPE__TWO_PLANE_DISTANCE, &twoPlaneDistanceDef},
			{MEASURE_TYPE__TWO_PLANE_DISTANCE, &twoPlaneDistanceDef2},
			{MEASURE_TYPE__TWO_PLANE_DISTANCE, &twoPlaneDistanceDef3},
		};
#else
		POINT_PLANE_DEF	pointPlaneDef1 = {
			{20790, 0.025643f, 0.962431f},	// 정수리
			{				// 골반
				{62710, 0.448102f, 0.258171f},
				D3DXVECTOR3(0, 1, 0)
			}
		};
		MEASURE_DEF__POINT_PLANE_DISTANCE	pointPlaneDistanceDef1(pointPlaneDef1);

		TWO_PLANE_DEF	twoPlaneDef = {
			{				// 골반
				{65847, 0.015407f, 0.824872f},
				D3DXVECTOR3(0.086850f, 0.996197f, 0.007047f)
			},
			{				// 오른쪽 무릎
				{72432, 0.102591f, 0.257141f},
				D3DXVECTOR3(0.086850f, 0.996197f, 0.007047f)
			}
		};
		MEASURE_DEF__TWO_PLANE_DISTANCE	twoPlaneDistanceDef(twoPlaneDef);

		POINT_PLANE_DEF	pointPlaneDef2 = {
			{73637, 0.016866f, 0.185059f},	// 오른쪽 발뒤꿈치
			{				// 오른쪽 무릎
				{72432, 0.102591f, 0.257141f},
				D3DXVECTOR3(0.086850f, 0.996197f, 0.007047f)
			},
		};
		MEASURE_DEF__POINT_PLANE_DISTANCE	pointPlaneDistanceDef2(pointPlaneDef2);

		////
		MEASURE_DEF	defList[3] = {
			{MEASURE_TYPE__POINT_PLANE_DISTANCE, &pointPlaneDistanceDef1},
			{MEASURE_TYPE__TWO_PLANE_DISTANCE, &twoPlaneDistanceDef},
			{MEASURE_TYPE__POINT_PLANE_DISTANCE, &pointPlaneDistanceDef2},
		};
#endif
	}
	namespace SeatedHeight {
#if 1
		TWO_PLANE_DEF	twoPlaneDef = {
			{				// 목젖
				{26323, 0.0868376f, 0.0711014f},
				D3DXVECTOR3(0.086850f, 0.996197f, 0.007047f)	// 사용하지 않음; 아래의 FACEID_LIST 가 사용
			},
			{				// 골반
				{11496, 0.382994f, 0.6125f},
				D3DXVECTOR3(0.086850f, 0.996197f, 0.007047f)	// 사용하지 않음; 아래의 FACEID_LIST 가 사용
			}
		};
		FACEID_LIST	faceID_list = {7073, ETRI7::trunk};
		MEASURE_DEF__TWO_PLANE_DISTANCE	twoPlaneDistanceDef(twoPlaneDef, &faceID_list);

		////
		MEASURE_DEF	defList[1] = {
			{MEASURE_TYPE__TWO_PLANE_DISTANCE, &twoPlaneDistanceDef},
		};
#else
		POINT_PLANE_DEF	pointPlaneDef = {
			{20790, 0.025643f, 0.962431f},	// 정수리
			{				// 골반
				{62710, 0.448102f, 0.258171f},
				D3DXVECTOR3(0, 1, 0)
			}
		};
		MEASURE_DEF__POINT_PLANE_DISTANCE	pointPlaneDistanceDef(pointPlaneDef);

		////
		MEASURE_DEF	defList[1] = {
			{MEASURE_TYPE__POINT_PLANE_DISTANCE, &pointPlaneDistanceDef},
		};
#endif
	}
	namespace Shoulder {
		TWO_POINT_DEF	twoPointDef = {
			{4805, 0.326227f, 0.203269f},
			{7577, 0.155757f, 0.139314f}
		};
		MEASURE_DEF__TWO_POINT_GIRTH	twoPointGirth(TAPE_MEASURE_MODE_WRAP, twoPointDef);

		////
		MEASURE_DEF	defList[1] = {
			{MEASURE_TYPE__TWO_POINT_GIRTH, &twoPointGirth},
		};
	}
	namespace LeftLeg {
		TWO_PLANE_DEF	twoPlaneDef = {
			{	// 허벅지 윗부분
				{12095, 0.242849f, 0.401147f},
				D3DXVECTOR3(-0.025932f, 0.984654f, -0.172584f)
			},
			{	// 무릎
				{19585, 0.244717f, 0.108785f},
				D3DXVECTOR3(-0.025932f, 0.984654f, -0.172584f)
			}
		};
		FACEID_LIST	faceID_list = {1282, ETRI7::leftThigh};
		MEASURE_DEF__TWO_PLANE_DISTANCE	twoPlaneDistanceDef(twoPlaneDef, &faceID_list);

#if 1
		TWO_PLANE_DEF	twoPlaneDef2 = {
			{	// 무릎
				{19585, 0.244717f, 0.108785f},
				D3DXVECTOR3(-0.025932f, 0.984654f, -0.172584f)
			},
			{	// 발목
				{18710, 0.529306f, 0.1605f},
				D3DXVECTOR3(-0.025932f, 0.984654f, -0.172584f)
			}
		};
		FACEID_LIST	faceID_list2 = {740, ETRI7::leftCalf};
		MEASURE_DEF__TWO_PLANE_DISTANCE	twoPlaneDistanceDef2(twoPlaneDef2, &faceID_list2);

		////
		MEASURE_DEF	defList[2] = {
			{MEASURE_TYPE__TWO_PLANE_DISTANCE, &twoPlaneDistanceDef},
			{MEASURE_TYPE__TWO_PLANE_DISTANCE, &twoPlaneDistanceDef2},
		};
#else
		POINT_PLANE_DEF	pointPlaneDef = {
			{74634, 0.960823f, 0.020719f},	// 발뒤꿈치
			{	// 무릎
				{71314, 0.424358f, 0.008220f},
				D3DXVECTOR3(-0.025932f, 0.984654f, -0.172584f)
			},
		};
		MEASURE_DEF__POINT_PLANE_DISTANCE	pointPlaneDistanceDef(pointPlaneDef);

		////
		MEASURE_DEF	defList[2] = {
			{MEASURE_TYPE__TWO_PLANE_DISTANCE, &twoPlaneDistanceDef},
			{MEASURE_TYPE__POINT_PLANE_DISTANCE, &pointPlaneDistanceDef},
		};
#endif
	}
	namespace RightLeg {
		TWO_PLANE_DEF	twoPlaneDef = {
			{	// 허벅지 윗부분
				{13910, 0.624339f, 0.205495f},
				D3DXVECTOR3(-0.025932f, 0.984654f, -0.172584f)
			},
			{	// 무릎
				{20840, 0.396672f, 0.368413f},
				D3DXVECTOR3(-0.025932f, 0.984654f, -0.172584f)
			}
		};
		FACEID_LIST	faceID_list = {1354, ETRI7::rightThigh};
		MEASURE_DEF__TWO_PLANE_DISTANCE	twoPlaneDistanceDef(twoPlaneDef);

#if 1
		TWO_PLANE_DEF	twoPlaneDef2 = {
			{	// 무릎
				{20840, 0.396672f, 0.368413f},
				D3DXVECTOR3(-0.025932f, 0.984654f, -0.172584f)
			},
			{	// 발목
				{20063, 0.307081f, 0.346966f},
				D3DXVECTOR3(-0.025932f, 0.984654f, -0.172584f)
			}
		};
		FACEID_LIST	faceID_list2 = {801, ETRI7::rightCalf};
		MEASURE_DEF__TWO_PLANE_DISTANCE	twoPlaneDistanceDef2(twoPlaneDef2, &faceID_list2);

		////
		MEASURE_DEF	defList[2] = {
			{MEASURE_TYPE__TWO_PLANE_DISTANCE, &twoPlaneDistanceDef},
			{MEASURE_TYPE__TWO_PLANE_DISTANCE, &twoPlaneDistanceDef2},
		};
#else
		POINT_PLANE_DEF	pointPlaneDef = {
			{73684, 0.008145f, 0.150210f},	// 발뒤꿈치
			{	// 무릎
				{72746, 0.530067f, 0.334535f},
				D3DXVECTOR3(-0.025932f, 0.984654f, -0.172584f)
			},
		};
		MEASURE_DEF__POINT_PLANE_DISTANCE	pointPlaneDistanceDef(pointPlaneDef);

		////
		MEASURE_DEF	defList[2] = {
			{MEASURE_TYPE__TWO_PLANE_DISTANCE, &twoPlaneDistanceDef},
			{MEASURE_TYPE__POINT_PLANE_DISTANCE, &pointPlaneDistanceDef},
		};
#endif
	}
	namespace LeftArm {
		TWO_PLANE_DEF	twoPlaneDef1 = {
			{	// 팔 윗부분
				{4138, 0.102006f, 0.822178f},
				D3DXVECTOR3(-0.998316f, 0.058097f, 0.000468f)
			},
			{	// 팔꿈치
				{22637, 0.306695f, 0.122812f},
				D3DXVECTOR3(-0.998316f, 0.058097f, 0.000468f)
			}
		};
		FACEID_LIST	faceID_list1 = {1704, ETRI7::leftForearm};
		MEASURE_DEF__TWO_PLANE_DISTANCE	twoPlaneDistanceDef1(twoPlaneDef1, &faceID_list1);

		TWO_PLANE_DEF	twoPlaneDef2 = {
			{	// 팔꿈치
				{22637, 0.306695f, 0.122812f},
				D3DXVECTOR3(-0.931428f, 0.061501f, 0.358703f)
			},
			{	// 손목
				{23175, 0.587979f, 0.366902f},
				D3DXVECTOR3(-0.931428f, 0.061501f, 0.358703f)
			}
		};
		FACEID_LIST	faceID_list2 = {976, ETRI7::leftLowerArm};
		MEASURE_DEF__TWO_PLANE_DISTANCE	twoPlaneDistanceDef2(twoPlaneDef2, &faceID_list2);

		////
		MEASURE_DEF	defList[2] = {
			{MEASURE_TYPE__TWO_PLANE_DISTANCE, &twoPlaneDistanceDef1},
			{MEASURE_TYPE__TWO_PLANE_DISTANCE, &twoPlaneDistanceDef2},
		};
	}
	namespace RightArm {
		TWO_PLANE_DEF	twoPlaneDef1 = {
			{	// 팔 위부분
				{7667, 0.174006f, 0.814571f},
				D3DXVECTOR3(-0.970453f, -0.220734f, -0.097510f)
			},
			{	// 팔꿈치
				{24552, 0.131059f, 0.0778305f},
				D3DXVECTOR3(-0.970453f, -0.220734f, -0.097510f)
			}
		};
		FACEID_LIST	faceID_list1 = {1687, ETRI7::rightForearm};
		MEASURE_DEF__TWO_PLANE_DISTANCE	twoPlaneDistanceDef1(twoPlaneDef1, &faceID_list1);

		TWO_PLANE_DEF	twoPlaneDef2 = {
			{	// 팔꿈치
				{24552, 0.131059f, 0.0778305f},
				D3DXVECTOR3(-0.964311f, -0.052404f, -0.259560f)
			},
			{	// 손목
				{25514, 0.440672f, 0.3321f},
				D3DXVECTOR3(-0.964311f, -0.052404f, -0.259560f)
			}
		};
		FACEID_LIST	faceID_list2 = {766, ETRI7::rightLowerArm};
		MEASURE_DEF__TWO_PLANE_DISTANCE	twoPlaneDistanceDef2(twoPlaneDef2, &faceID_list2);

		////
		MEASURE_DEF	defList[2] = {
			{MEASURE_TYPE__TWO_PLANE_DISTANCE, &twoPlaneDistanceDef1},
			{MEASURE_TYPE__TWO_PLANE_DISTANCE, &twoPlaneDistanceDef2},
		};
	}
#if 0
	namespace LeftArmInside {
		TWO_POINT_DEF	twoPointDef0 = {
			{56950, 0.556603f, 0.268797f},
			{81040, 0.561017f, 0.294061f}
		};
		MEASURE_DEF__TWO_POINT_GIRTH	twoPointGirthDef0(TAPE_MEASURE_MODE_WRAP, twoPointDef0);

		TWO_POINT_DEF	twoPointDef1 = {
			{82198, 0.325851f, 0.469118f},
			{81040, 0.561017f, 0.294061f}
		};
		MEASURE_DEF__TWO_POINT_GIRTH	twoPointGirthDef1(TAPE_MEASURE_MODE_WRAP, twoPointDef1);

		////
		MEASURE_DEF	defList[2] = {
			{MEASURE_TYPE__TWO_POINT_GIRTH, &twoPointGirthDef0},
			{MEASURE_TYPE__TWO_POINT_GIRTH, &twoPointGirthDef1},
		};
	}
	namespace LeftArmOutside {
		TWO_POINT_DEF	twoPointDef0 = {
			{56540, 0.439220f, 0.528292f},
			{81116, 0.567838f, 0.280615f}
		};
		MEASURE_DEF__TWO_POINT_GIRTH	twoPointGirthDef0(TAPE_MEASURE_MODE_WRAP, twoPointDef0);

		TWO_POINT_DEF	twoPointDef1 = {
			{81638, 0.585758f, 0.282053f},
			{81116, 0.567838f, 0.280615f}
		};
		MEASURE_DEF__TWO_POINT_GIRTH	twoPointGirthDef1(TAPE_MEASURE_MODE_WRAP, twoPointDef1);

		////
		MEASURE_DEF	defList[2] = {
			{MEASURE_TYPE__TWO_POINT_GIRTH, &twoPointGirthDef0},
			{MEASURE_TYPE__TWO_POINT_GIRTH, &twoPointGirthDef1},
		};
	}
	namespace RightArmInside {
		TWO_POINT_DEF	twoPointDef0 = {
			{59046, 0.097483f, 0.517813f},
			{82458, 0.096169f, 0.430583f}
		};
		MEASURE_DEF__TWO_POINT_GIRTH	twoPointGirthDef0(TAPE_MEASURE_MODE_WRAP, twoPointDef0);

		TWO_POINT_DEF	twoPointDef1 = {
			{84733, 0.087797f, 0.654378f},
			{82458, 0.096169f, 0.430583f}
		};
		MEASURE_DEF__TWO_POINT_GIRTH	twoPointGirthDef1(TAPE_MEASURE_MODE_WRAP, twoPointDef1);

		////
		MEASURE_DEF	defList[2] = {
			{MEASURE_TYPE__TWO_POINT_GIRTH, &twoPointGirthDef0},
			{MEASURE_TYPE__TWO_POINT_GIRTH, &twoPointGirthDef1},
		};
	}
	namespace RightArmOutside {
		TWO_POINT_DEF	twoPointDef0 = {
			{59301, 0.006432f, 0.615452f},
			{83625, 0.448489f, 0.026192f}
		};
		MEASURE_DEF__TWO_POINT_GIRTH	twoPointGirthDef0(TAPE_MEASURE_MODE_WRAP, twoPointDef0);

		TWO_POINT_DEF	twoPointDef1 = {
			{84828, 0.265014f, 0.658732f},
			{83625, 0.448489f, 0.026192f}
		};
		MEASURE_DEF__TWO_POINT_GIRTH	twoPointGirthDef1(TAPE_MEASURE_MODE_WRAP, twoPointDef1);

		////
		MEASURE_DEF	defList[2] = {
			{MEASURE_TYPE__TWO_POINT_GIRTH, &twoPointGirthDef0},
			{MEASURE_TYPE__TWO_POINT_GIRTH, &twoPointGirthDef1},
		};
	}
	namespace LeftLegInside {
		TWO_POINT_DEF	twoPointDef = {
//			{64476, 0.321285f, 0.668669f},
//			{74333, 0.221292f, 0.665281f}
			{64379, 0.260959f, 0.429566f},
			{74151, 0.237487f, 0.348736f}
		};
		MEASURE_DEF__TWO_POINT_GIRTH	twoPointGirth(TAPE_MEASURE_MODE_WRAP, twoPointDef);

		////
		MEASURE_DEF	defList[1] = {
			{MEASURE_TYPE__TWO_POINT_GIRTH, &twoPointGirth},
		};
	}
	namespace LeftLegOutside {
		TWO_POINT_DEF	twoPointDef = {
//			{64261, 0.598616f, 0.322904f},
//			{74678, 0.226481f, 0.352553f}
			{63754, 0.831608f, 0.088129f},
			{74125, 0.783423f, 0.183948f}
		};
		MEASURE_DEF__TWO_POINT_GIRTH	twoPointGirth(TAPE_MEASURE_MODE_WRAP, twoPointDef);

		////
		MEASURE_DEF	defList[1] = {
			{MEASURE_TYPE__TWO_POINT_GIRTH, &twoPointGirth},
		};
	}
	namespace RightLegInside {
		TWO_POINT_DEF	twoPointDef = {
			{66209, 0.259354f, 0.478601f},
			{73868, 0.154951f, 0.400970f}
		};
		MEASURE_DEF__TWO_POINT_GIRTH	twoPointGirth(TAPE_MEASURE_MODE_WRAP, twoPointDef);

		////
		MEASURE_DEF	defList[1] = {
			{MEASURE_TYPE__TWO_POINT_GIRTH, &twoPointGirth},
		};
	}
	namespace RightLegOutside {
		TWO_POINT_DEF	twoPointDef = {
			{73721, 0.485668f, 0.005433f},
			{65561, 0.382790f, 0.059710f}
		};
		MEASURE_DEF__TWO_POINT_GIRTH	twoPointGirth(TAPE_MEASURE_MODE_WRAP, twoPointDef);

		////
		MEASURE_DEF	defList[1] = {
			{MEASURE_TYPE__TWO_POINT_GIRTH, &twoPointGirth},
		};
	}
#endif
#endif	// 길이

	////
	MEASURE_TYPE_DEF	measureTypeDef[9 - 2 + 15 - 8] = {	// HEAD, NECK
#if 1	// 둘레
#if 0
		{TAPE_MEASURE_HEAD,
			{
				L"머리",
				D3DXVECTOR2(373, 27),
				D3DCOLOR_XRGB(0x99, 0x00, 0x00),
			},
			{1, Head::defList}
		},
		{TAPE_MEASURE_NECK,
			{
				L"목",
				D3DXVECTOR2(425, 48),
				D3DCOLOR_XRGB(0x00, 0x66, 0xcc),
			},
			{1, Neck::defList}
		},
#endif
		{TAPE_MEASURE_BREAST,
			{
				L"가슴",
				D3DXVECTOR2(169, 104),
				D3DCOLOR_XRGB(0x00, 0x66, 0xcc),
			},
			{1, Breast::defList}
		},
		{TAPE_MEASURE_WAIST,
			{
				L"허리",
				D3DXVECTOR2(174, 139),
				D3DCOLOR_XRGB(0x00, 0x66, 0xcc),
			},
			{1, Waist::defList}
		},
		{TAPE_MEASURE_HIP,
			{
				L"엉덩이",
				D3DXVECTOR2(392, 119),
				D3DCOLOR_XRGB(0x00, 0x66, 0xcc),
			},
			{1, Hip::defList}
		},
		{TAPE_MEASURE_LEFT_THIGH,
			{
				L"왼쪽허벅지",
				D3DXVECTOR2(386, 186),
				D3DCOLOR_XRGB(0x00, 0x66, 0xcc),
			},
			{1, LeftThigh::defList}
		},
		{TAPE_MEASURE_RIGHT_THIGH,
			{
				L"오른쪽허벅지",
				D3DXVECTOR2(237, 186),
				D3DCOLOR_XRGB(0x00, 0x66, 0xcc),
			},
			{1, RightThigh::defList}
		},
		{TAPE_MEASURE_LEFT_FOREARM,
			{
				L"왼쪽상완",
				D3DXVECTOR2(393, 105),
				D3DCOLOR_XRGB(0x00, 0x66, 0xcc),
			},
			{1, LeftForeArm::defList}
		},
		{TAPE_MEASURE_RIGHT_FOREARM,
			{
				L"오른쪽상완",
				D3DXVECTOR2(195, 96),
				D3DCOLOR_XRGB(0x00, 0x66, 0xcc),
			},
			{1, RightForeArm::defList}
		},
#endif	// 둘레
#if 1	// 길이
		{TAPE_MEASURE_HEIGHT,
			{
				L"키",
				D3DXVECTOR2(140, 240),
				D3DCOLOR_XRGB(0x00, 0x66, 0xcc),
			},
			{3, Height::defList}
		},
		{TAPE_MEASURE_SEATED_HEIGHT,
			{
				L"앉은키",
				D3DXVECTOR2(449, 139),
				D3DCOLOR_XRGB(0x00, 0x66, 0xcc),
			},
			{1, SeatedHeight::defList}
		},
		{TAPE_MEASURE_SHOULDER_WIDTH,
			{
				L"어깨넓이",
				D3DXVECTOR2(186, 45),
				D3DCOLOR_XRGB(0x00, 0x66, 0xcc),
			},
			{1, Shoulder::defList}
		},
		{TAPE_MEASURE_LEFT_LEG,
			{
				L"왼다리",
				D3DXVECTOR2(393, 197),
				D3DCOLOR_XRGB(0x00, 0x66, 0xcc),
			},
			{2, LeftLeg::defList}
		},
		{TAPE_MEASURE_RIGHT_LEG,
			{
				L"오른다리",
				D3DXVECTOR2(225, 191),
				D3DCOLOR_XRGB(0x00, 0x66, 0xcc),
			},
			{2, RightLeg::defList}
		},
		{TAPE_MEASURE_LEFT_ARM,
			{
				L"왼팔",
				D3DXVECTOR2(439, 110),
				D3DCOLOR_XRGB(0x00, 0x66, 0xcc),
			},
			{2, LeftArm::defList}
		},
		{TAPE_MEASURE_RIGHT_ARM,
			{
				L"오른팔",
				D3DXVECTOR2(157, 109),
				D3DCOLOR_XRGB(0x00, 0x66, 0xcc),
			},
			{2, RightArm::defList}
		},
#if 0
		{TAPE_MEASURE_LEFT_ARM_INSIDE,
			{
				L"왼팔안쪽",
				D3DXVECTOR2(476, 96),
				D3DCOLOR_XRGB(0x00, 0x66, 0xcc),
			},
			{2, LeftArmInside::defList}
		},
		{TAPE_MEASURE_LEFT_ARM_OUTSIDE,
			{
				L"왼팔바깥쪽",
				D3DXVECTOR2(149, 99),
				D3DCOLOR_XRGB(0x00, 0x66, 0xcc),
			},
			{2, LeftArmOutside::defList}
		},
		{TAPE_MEASURE_RIGHT_ARM_INSIDE,
			{
				L"오른팔안쪽",
				D3DXVECTOR2(469, 49),
				D3DCOLOR_XRGB(0x00, 0x66, 0xcc),
			},
			{2, RightArmInside::defList}
		},
		{TAPE_MEASURE_RIGHT_ARM_OUTSIDE,
			{
				L"오른팔바깥쪽",
				D3DXVECTOR2(153, 52),
				D3DCOLOR_XRGB(0x00, 0x66, 0xcc),
			},
			{2, RightArmOutside::defList}
		},
		{TAPE_MEASURE_LEFT_LEG_INSIDE,
			{
				L"왼다리안쪽",
				D3DXVECTOR2(390, 303),
//				D3DCOLOR_XRGB(0x00, 0x66, 0xcc),
				D3DCOLOR_XRGB(0x99, 0x00, 0x00),
			},
			{1, LeftLegInside::defList}
		},
		{TAPE_MEASURE_LEFT_LEG_OUTSIDE,
			{
				L"왼다리바깥쪽",
				D3DXVECTOR2(448, 367),
//				D3DCOLOR_XRGB(0x00, 0x66, 0xcc),
				D3DCOLOR_XRGB(0x99, 0x00, 0x00),
			},
			{1, LeftLegOutside::defList}
		},
		{TAPE_MEASURE_RIGHT_LEG_INSIDE,
			{
				L"오른다리안쪽",
				D3DXVECTOR2(213, 303),
				D3DCOLOR_XRGB(0x00, 0x66, 0xcc),
			},
			{1, RightLegInside::defList}
		},
		{TAPE_MEASURE_RIGHT_LEG_OUTSIDE,
			{
				L"오른다리바깥쪽",
				D3DXVECTOR2(164, 350),
				D3DCOLOR_XRGB(0x00, 0x66, 0xcc),
			},
			{1, RightLegOutside::defList}
		},
#endif
#endif	// 길이
	};
}
#endif	// 에트리 표준모델
MEASURE_TYPE_DEF_LIST	measureTypeDefList[] = {
	{1, DAZ::measureTypeDef},
	{9 + 15, ETRI::measureTypeDef},	// 9 + 15 = 24: NUM_TAPE_MEASURE_TYPE
	{0, NULL},
	{1 + 4, ETRI2::measureTypeDef},
	{1 + 4, ETRI3::measureTypeDef},
	{1 + 4, ETRI5::measureTypeDef},
	{9 - 2 + 15 - 8, ETRI6::measureTypeDef},	// HEAD, NECK, {Left,Right}{Arm,Leg}{Inside,OutSide}
	{9 - 2 + 15 - 8, ETRI7::measureTypeDef},	// HEAD, NECK, {Left,Right}{Arm,Leg}{Inside,OutSide}
};
#endif	// 둘레/길이 정의

namespace DAZ {
	AREA_TYPE_DEF	areaTypeDef[1] = {
		{AREA_NECK,		{732, DAZ::neck}},
		// 나머지를 채운다...; Defined.TXT 참고
	};

	MODEL_COLOR_DEF	modelColorDef[] = {
		{100, DAZ::LEFT_UP_A},
		// 나머지를 채운다...; Defined.TXT 참고
	};
}
namespace ETRI {
	AREA_TYPE_DEF	areaTypeDef[15] = {
		{AREA_TRUNK,	{6934, ETRI::trunk}},
		{AREA_LEFT_ARM,		{2733, ETRI::leftArm}},
		{AREA_RIGHT_ARM,	{2986, ETRI::rightArm}},
		{AREA_LEFT_LEG,		{3822, ETRI::leftLeg}},
		{AREA_RIGHT_LEG,	{3728, ETRI::rightLeg}},
		{AREA_NECK,		{842, ETRI::neck}},
		{AREA_CHEST,		{1939, ETRI::chest}},
		{AREA_ABDOMINAL,	{748, ETRI::abdominal}},
		{AREA_HIP,		{1981, ETRI::hip}},
		{AREA_LEFT_FORE_ARM,	{1407, ETRI::leftForearm}},
		{AREA_RIGHT_FORE_ARM,	{1528, ETRI::rightForearm}},
		{AREA_LEFT_THIGH,	{1745, ETRI::leftThigh}},
		{AREA_RIGHT_THIGH,	{1545, ETRI::rightThigh}},
		{AREA_LEFT_CALF,	{907, ETRI::leftCalf}},
		{AREA_RIGHT_CALF,	{853, ETRI::rightCalf}},
	};

	MODEL_COLOR_DEF	modelColorDef[] = {
		// MODEL_POINT_LEFT_UP
		{95, ETRI::LEFT_UP_A},
		{107, ETRI::LEFT_UP_B},
		{115, ETRI::LEFT_UP_C},

		// MODEL_POINT_RIGHT_UP
		{93, ETRI::RIGHT_UP_A},
		{100, ETRI::RIGHT_UP_B},
		{113, ETRI::RIGHT_UP_C},
	};
}
namespace ETRI5 {
#if 1
	AREA_TYPE_DEF	areaTypeDef[2] = {
// /* ... */    {neck, 241},
		{AREA_TRUNK,		{241, ETRI5::neck}},
// /* ... */    {abdominal, 439},
		{AREA_ABDOMINAL,	{439, ETRI5::abdominal}},
	};
#else
	AREA_TYPE_DEF	areaTypeDef[15] = {
		{AREA_TRUNK,	{6934, ETRI::trunk}},
		{AREA_LEFT_ARM,		{2733, ETRI::leftArm}},
		{AREA_RIGHT_ARM,	{2986, ETRI::rightArm}},
		{AREA_LEFT_LEG,		{3822, ETRI::leftLeg}},
		{AREA_RIGHT_LEG,	{3728, ETRI::rightLeg}},
		{AREA_NECK,		{842, ETRI::neck}},
		{AREA_CHEST,		{1939, ETRI::chest}},
		{AREA_ABDOMINAL,	{748, ETRI::abdominal}},
		{AREA_HIP,		{1981, ETRI::hip}},
		{AREA_LEFT_FORE_ARM,	{1407, ETRI::leftForearm}},
		{AREA_RIGHT_FORE_ARM,	{1528, ETRI::rightForearm}},
		{AREA_LEFT_THIGH,	{1745, ETRI::leftThigh}},
		{AREA_RIGHT_THIGH,	{1545, ETRI::rightThigh}},
		{AREA_LEFT_CALF,	{907, ETRI::leftCalf}},
		{AREA_RIGHT_CALF,	{853, ETRI::rightCalf}},
	};
#endif
#if 0
	MODEL_COLOR_DEF	modelColorDef[] = {
		// MODEL_POINT_LEFT_UP
		{95, ETRI::LEFT_UP_A},
		{107, ETRI::LEFT_UP_B},
		{115, ETRI::LEFT_UP_C},

		// MODEL_POINT_RIGHT_UP
		{93, ETRI::RIGHT_UP_A},
		{100, ETRI::RIGHT_UP_B},
		{113, ETRI::RIGHT_UP_C},
	};
#endif
}
namespace ETRI6 {	// 에트리 표준모델 - 091211 (남; michael)
	AREA_TYPE_DEF	areaTypeDef[15 - 1] = {	// NECK missing!!!
// /* ... */    {trunk, 5393},
// /* ... */    {leftArm, 2649},
// /* ... */    {rightArm, 2755},
// /* ... */    {leftLeg, 3154},
// /* ... */    {rightLeg, 3135},
// /* ... */    {chest, 2154},
// /* ... */    {abdominal, 990},
// /* ... */    {hip, 1123},
// /* ... */    {leftForearm, 1389},
// /* ... */    {rightForearm, 1385},
// /* ... */    {leftThigh, 985},
// /* ... */    {rightThigh, 1044},
// /* ... */    {leftCalf, 656},
// /* ... */    {rightCalf, 629},
		{AREA_TRUNK,		{5393, ETRI6::trunk}},
		{AREA_LEFT_ARM,		{2649, ETRI6::leftArm}},
		{AREA_RIGHT_ARM,	{2755, ETRI6::rightArm}},
		{AREA_LEFT_LEG,		{3154, ETRI6::leftLeg}},
		{AREA_RIGHT_LEG,	{3135, ETRI6::rightLeg}},
//		{AREA_NECK,		{842, ETRI6::neck}},
		{AREA_CHEST,		{2154, ETRI6::chest}},
		{AREA_ABDOMINAL,	{990, ETRI6::abdominal}},
		{AREA_HIP,		{1123, ETRI6::hip}},
		{AREA_LEFT_FORE_ARM,	{1389, ETRI6::leftForearm}},
		{AREA_RIGHT_FORE_ARM,	{1385, ETRI6::rightForearm}},
		{AREA_LEFT_THIGH,	{985, ETRI6::leftThigh}},
		{AREA_RIGHT_THIGH,	{1044, ETRI6::rightThigh}},
		{AREA_LEFT_CALF,	{656, ETRI6::leftCalf}},
		{AREA_RIGHT_CALF,	{629, ETRI6::rightCalf}},
	};
}
namespace ETRI7 {	// 에트리 표준모델 - 091211 (여; victoria)
	AREA_TYPE_DEF	areaTypeDef[15 - 1] = {	// NECK missing!!!
// /* ... */    {trunk, 7073},
// /* ... */    {leftArm, 2866},
// /* ... */    {rightArm, 2794},
// /* ... */    {leftLeg, 3300},
// /* ... */    {rightLeg, 3246},
// /* ... */    {chest, 2051},
// /* ... */    {abdominal, 1076},
// /* ... */    {hip, 2154},
// /* ... */	{leftForearm, 1704},
// /* ... */	{rightForearm, 1687},
// /* ... */    {leftThigh, 1282},
// /* ... */    {rightThigh, 1354},
// /* ... */    {leftCalf, 740},
// /* ... */    {rightCalf, 801},
		{AREA_TRUNK,		{7073, ETRI7::trunk}},
		{AREA_LEFT_ARM,		{2866, ETRI7::leftArm}},
		{AREA_RIGHT_ARM,	{2794, ETRI7::rightArm}},
		{AREA_LEFT_LEG,		{3300, ETRI7::leftLeg}},
		{AREA_RIGHT_LEG,	{3246, ETRI7::rightLeg}},
//		{AREA_NECK,		{842, ETRI7::neck}},
		{AREA_CHEST,		{2051, ETRI7::chest}},
		{AREA_ABDOMINAL,	{1076, ETRI7::abdominal}},
		{AREA_HIP,		{2154, ETRI7::hip}},
		{AREA_LEFT_FORE_ARM,	{1704, ETRI7::leftForearm}},
		{AREA_RIGHT_FORE_ARM,	{1687, ETRI7::rightForearm}},
		{AREA_LEFT_THIGH,	{1282, ETRI7::leftThigh}},
		{AREA_RIGHT_THIGH,	{1354, ETRI7::rightThigh}},
		{AREA_LEFT_CALF,	{740, ETRI7::leftCalf}},
		{AREA_RIGHT_CALF,	{801, ETRI7::rightCalf}},
	};
}
AREA_TYPE_DEF_LIST	areaTypeDefList[] = {
	{1, DAZ::areaTypeDef},
	{5 + 10, ETRI::areaTypeDef},
	{0, NULL},
	{0, NULL},
	{0, NULL},
//	{0, NULL},
	{2, ETRI5::areaTypeDef},
	{14, ETRI6::areaTypeDef},
	{14, ETRI7::areaTypeDef},
};
MODEL_COLOR_DEF_LIST	modelColorDefList[] = {
	// DAZ
	{1, DAZ::modelColorDef},
	{6, ETRI::modelColorDef},
	{0, NULL},
	{0, NULL},
	{0, NULL},
	{0, NULL},
	{0, NULL},
	{0, NULL},
};
