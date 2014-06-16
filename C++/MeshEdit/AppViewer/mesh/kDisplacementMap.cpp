#include <mesh/kDisplacementMap.h>

kDisplacementMap::kDisplacementMap(const WCHAR* mapFile) : Gdiplus::Bitmap(mapFile)
{
	__width = -1;
	__height = -1;
	__max_red = -1;
	__max_green = -1;
	__max_blue = -1;
	__max_alpha = -1;
	if (Image::GetLastStatus() == Gdiplus::Ok) {
		UINT	width = Image::GetWidth();
		if (Image::GetLastStatus() == Gdiplus::Ok) {
			UINT	heigth = Image::GetHeight();
			if (Image::GetLastStatus() == Gdiplus::Ok) {
				__width = width;
				__height = heigth;
			} else {
				cerr << "kDisplacementMap::kDisplacementMap(): Image::GetHeight(): Failure" << endl;
			}
		} else {
			cerr << "kDisplacementMap::kDisplacementMap(): Image::GetWidth(): Failure" << endl;
		}
		// 최대값을 저장
		for (int h = 0; h < __height; ++h)
			for (int w = 0; w < __width; ++w) {
				Gdiplus::Color	color;
				if (Bitmap::GetPixel(h, w, &color) != Gdiplus::Ok) {
					cerr << "kDisplacementMap::kDisplacementMap(): Bitmap::GetPixel(" << h << ", " << w << "): Failure: Ignored" << endl;
					continue;
				}
				int	red = color.GetRed();
				int	green = color.GetGreen();
				int	blue = color.GetBlue();
				int	alpha = color.GetAlpha();
				if (red > __max_red)
					__max_red = red;
				if (green > __max_green)
					__max_green = green;
				if (blue > __max_blue)
					__max_blue = blue;
				if (alpha > __max_alpha)
					__max_alpha = alpha;
			}
	} else {
		cerr << "kDisplacementMap::kDisplacementMap(): Bitmap::Bitmap(const WCHAR*): Failure: |" << mapFile << "|" << endl;
	}
}
kDisplacementMap::~kDisplacementMap()
{
}


Gdiplus::Status	kDisplacementMap::GetPixelAtUV(float u, float v, float& val, CHANNEL_COLOR channel, Gdiplus::Color* color_ptr_at_floored_uv) const
{
#if 0	// kMeshLoader::__LoadGeometryFromOBJ()...
#if 1	// 에트리 모델: Right-handed (Cartesian) Coordinate System; 텍스쳐 사용
// DirectX 에서는 OpenGL 과 달리 텍스쳐의 v 방향이 위에서 아래로 향한다.
		vt.y = 1.f - vt.y;
#endif
#endif
	v = 1 - v;

	Gdiplus::Status	status = Gdiplus::Ok;
	val = 0.f;	// 에러가 발생했을 때의 값

	if (__width < 0 || __height < 0) {	// bitmap 파일이 없거나 제대로된 이미지파일이 아니다.
		cerr << "kDisplacementMap::GetPixelAtUV(u = " << u << ", v = " << v << "): Failure: __width = " << __width << ", __height = " << __height << endl;
		status = Gdiplus::InvalidParameter;
		return status;
	}

	if ((u < 0.f || u > 1.f) || (v < 0.f || v > 1.f)) {
		cerr << "kDisplacementMap::GetPixelAtUV(u = " << u << ", v = " << v << "): Failure: OUT OF RANGE" << endl;
		status = Gdiplus::ValueOverflow;
		return status;
	}

	int	x = (INT) std::floor(__width * u);
	if (x >= __width)	// u = 1 or...
		x = __width - 1;
	int	y = (INT) std::floor(__height * v);
	if (y >= __height)	// v = 1 or...
		y = __height - 1;
	assert(x >= 0 && x < __width);
	assert(y >= 0 && y < __height);

	// floor 함수를 사용했듯이 (u, v) 에 해당하는 위치가 정확한 픽셀 위치(x, y)가 아니다.
	// (x, y), (x + 1, y), (x, y + 1), (x + 1, y + 1) 위치에서의 픽셀값을 사용하여
	// (u, v)에서의 픽셀값을 결정한다.
	//
	//	*C4(x,y+1): (u{x},v{y+1})	*C3(x+1,y+1): (u{x+1},v{y+1})
	//			*(u, v)
	//	*C1(x,y): (u{x},v{y})		*C2(x+1,y): (u{x+1},v{y})
	//
	int	xplus1 = x + 1;
	if (xplus1 >= __width)	// clamp to [0, __width)
		xplus1 = x;
	int	yplus1 = y + 1;
	if (yplus1 >= __height)		// clamp to [0, __height)
		yplus1 = y;

	Gdiplus::Color	xycolor;
	Bitmap&	bitmap = *((Bitmap*) this);
	status = bitmap.GetPixel(x, (__height - 1) - y, &xycolor);	// (식1)
	if (status != Gdiplus::Ok) {
		cerr << "kDisplacementMap::GetPixelAtUV(u = " << u << ", v = " << v << "): Failure: Bitmap::GetPixel(X = " << x << ", Y = " << ((__height - 1) - y) << "): Failure" << endl;
		return status;
	}
	if (color_ptr_at_floored_uv)
		*color_ptr_at_floored_uv = xycolor;
#if 0	// verbose
	cout << "(u = " << u << ", v = " << v << "(v0 = " << (1 - v) << ")): (X = " << x << ", Y = " << ((__height - 1) - y) << ")" << endl;
	cout << "\t" << "(" << (int) xycolor.GetR() << ", " << (int) xycolor.GetG() << ", " << (int) xycolor.GetB() << ", " << (int) xycolor.GetA() << ")" << endl;
#endif	// verbose
	Gdiplus::Color	xplus1ycolor;
	status = bitmap.GetPixel(xplus1, (__height - 1) - y, &xplus1ycolor);	// (식1)
	if (status != Gdiplus::Ok) {
		cerr << "kDisplacementMap::GetPixelAtUV(u = " << u << ", v = " << v << "): Failure: Bitmap::GetPixel(X + 1 = " << xplus1 << ", Y = " << ((__height - 1) - y) << "): Failure" << endl;
		return status;
	}
	Gdiplus::Color	xyplus1color;
	status = bitmap.GetPixel(x, (__height - 1) - yplus1, &xyplus1color);	// (식1)
	if (status != Gdiplus::Ok) {
		cerr << "kDisplacementMap::GetPixelAtUV(u = " << u << ", v = " << v << "): Failure: Bitmap::GetPixel(X = " << x << ", Y - 1 = " << ((__height - 1) - yplus1) << "): Failure" << endl;
		return status;
	}
	Gdiplus::Color	xplus1yplus1color;
	status = bitmap.GetPixel(xplus1, (__height - 1) - yplus1, &xplus1yplus1color);	// (식1)
	if (status != Gdiplus::Ok) {
		cerr << "kDisplacementMap::GetPixelAtUV(u = " << u << ", v = " << v << "): Failure: Bitmap::GetPixel(X + 1 = " << xplus1 << ", Y - 1 = " << ((__height - 1) - yplus1) << "): Failure" << endl;
		return status;
	}

	// 네 점에 해당하는 u, v 값과 각 점에서의 색깔을 이용하여,
	// (s, t) 파라미터를 사용한 linear interpolation 을 통해,
	// (u, v) 에서의 색깔을 결정한다.
	float	ux = x / (float) __width;
	float	vy = y / (float) __height;
	float	uxplus1 = xplus1 / (float) __width;
	float	vyplus1 = yplus1 / (float) __height;
	float	s = x == xplus1 ? 1.f : (u - ux) / (uxplus1 - ux);	// uxplus1 - ux = 1 / (float) __width
	// clamp to [0, 1]
	if (s < 0.f) {
		cerr << "kDisplacementMap::GetPixelAtUV(u = " << u << ", v = " << v << "): s(= " << s << ") < 0.f" << endl;
		s = 0.f;
	}
	if (s > 1.f) {
		cerr << "kDisplacementMap::GetPixelAtUV(u = " << u << ", v = " << v << "): s(= " << s << ") > 1.f" << endl;
		s = 1.f;
	}
	float	t = y == yplus1 ? 1.f : (v - vy) / (vyplus1 - vy);	// vyplus1 - vy = 1 / (float) __heigth
	// clamp to [0, 1]
	if (t < 0.f) {
		cerr << "kDisplacementMap::GetPixelAtUV(u = " << u << ", v = " << v << "): t(= " << t << ") < 0.f" << endl;
		t = 0.f;
	}
	if (t > 1.f) {
		cerr << "kDisplacementMap::GetPixelAtUV(u = " << u << ", v = " << v << "): t(= " << t << ") > 1.f" << endl;
		t = 1.f;
	}
	// [순서 1]
	// i) u 방향으로 interpolation
	//
	//	.4 +b	.3
	//	   *x
	//	.1 +a	.2
	//
	// Ca = C1 * s + C2 * (1 - s)
	// Cb = C4 * s + C3 * (1 - s)
	//ii) v 방향으로 interpolation
	//
	//	.4 +b	.3
	//	   *x
	//	.1 +a	.2
	//
	// Cx = Ca * t + Cb * (1 - t)
	//    = (C1 * s + C2 * (1 - s)) * t + (C4 * s + C3 * (1 - s)) * (1 - t)
	//    = C1 * s * t + C2 * (1 - s) * t + C3 * (1 - s) * (1 - t) + C4 * s * (1 - t)
	//
	// [순서 2]
	// i) v 방향으로 interpolation
	//	.4	.3
	//	+c *x	+d
	//	.1	.2
	// Cc = C1 * t + C4 * (1 - t)
	// Cd = C2 * t + C3 * (1 - t)
	//ii) u 방향으로 interpolation
	//	.4	.3
	//	+c *x	+d
	//	.1	.2
	// Cx = Cc * s + Cd * (1 - s)
	//    = (C1 * t + C4 * (1 - t)) * s + (C2 * t + C3 * (1 - t)) * (1 - s)
	//    = C1 * s * t + C2 * (1 - s) * t + C3 * (1 - s) * (1 - t) * C4 * s * (1 - t)
	//
	// 어떤 순서로 하든 결과는 같다:
	//	Cx = C1 * s * t + C2 * (1 - s) * t + C3 * (1 - s) * (1 - t) * C4 * s * (1 - t)

	Gdiplus::Color&	C1 = xycolor;
	Gdiplus::Color&	C2 = xplus1ycolor;
	Gdiplus::Color&	C3 = xplus1yplus1color;
	Gdiplus::Color&	C4 = xyplus1color;
	switch (channel) {
	case CHANNEL_RED:
		val = C1.GetR() * s * t + C2.GetR() * (1 - s) * t + C3.GetR() * (1 - s) * (1 - t) + C4.GetR() * s * (1 - t);
		break;
	case CHANNEL_GREEN:
		val = C1.GetG() * s * t + C2.GetG() * (1 - s) * t + C3.GetG() * (1 - s) * (1 - t) + C4.GetG() * s * (1 - t);
		break;
	case CHANNEL_BLUE:
		val = C1.GetB() * s * t + C2.GetB() * (1 - s) * t + C3.GetB() * (1 - s) * (1 - t) + C4.GetB() * s * (1 - t);
		break;
	case CHANNEL_ALPHA:
		val = C1.GetA() * s * t + C2.GetA() * (1 - s) * t + C3.GetA() * (1 - s) * (1 - t) + C4.GetA() * s * (1 - t);
		break;
	default:
		cerr << "kDisplacementMap::GetPixelAtUV(): CHANNEL_COLOR = " << channel << ": Not Implemented Yet" << endl;
		val = 0.f;
	}
	val /= (float) 255;
	// clamp to [0, 1]
	if (val < 0.f) {
		cerr << "kDisplacementMap::GetPixelAtUV(): val < 0.f" << endl;
		val = 0.f;
	}
	if (val > 1.f) {
		cerr << "kDisplacementMap::GetPixelAtUV(): val > 1.f" << endl;
		val = 1.f;
	}

	return status;
}
