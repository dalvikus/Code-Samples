#include <math/eigenvalue.h>
#include <math/linearleastsquare.h>

/*
 * least square
 * 2D: ax + by + c = 0 (line)
 * E^2 = \sum d_i^2 = \sum (a x_i + b y_i + c)^2 / (a^2 + b^2)
 * dE^2/dc = 0 으로터
 *	c = -a x_0 - b y_0
 *	x_0 = \sum x_i / N, y_0 = \sum y_i / N
 * E^2 = \sum [a (x_i - x_0) + b (y_i - y_0)]^2 / (a^2 + b^2)
 * can be written as
 * E^2 = x^T (A^T A) x / x^T x
 *	x^T = (a b)
 *	A^T A = symmetric 2x2 matrix
 *		= \sum (x_i - x_0)^2		\sum (x_i - x_0)(y_i - y_0)
 *		  \sum (y_i - y_0)(x_i - x_0)	\sum (y_i - y_0)^2
 * E^2 is Rayleigh Quotient, which has minimum(maximum) at the smallest(largest) eigenvalue
 * at the corresponding eigenvector(x)
 *
 *	Gilbert Strang, "Linear Algebra and Its Application", 2nd Ed., p.272
 *
 * 3D: ax + by + cz + d = 0 (plane)
 * all are the same as in 2D;
 *	d = - a x_0 - b y_0 - c z_0
 *	x_0 = \sum x_i / N, y_0 = \sum y_i / N, z_0 = \sum z_i / N
 *	E^2 = x^T (A^T A) x / x^T x
 *	x^T = (a b c)
 *	A^T A = symmetric 3x3 matrix
 *		= \sum (x_i - x_0)^2		\sum (x_i - x_0)(y_i - y_0)	\sum (x_i - x_0)(z_i - z_0)
 *		  \sum (y_i - y_0)(x_i - x_0)	\sum (y_i - y_0)^2		\sum (y_i - y_0)(z_i - z_0)
 *		  \sum (z_i - z_0)(x_i - x_0)	\sum (z_i - z_0)(y_i - y_0)	\sum (z_i - z_0)^2
 *
 * 3D: line
 *	x = x_0 + a t
 *	y = y_0 + b t
 *	z = z_0 + c t
 * with (a b c) unit vector: a^2 + b^2 + c^2 = 1
 * E^2 = \sum d_i^2 = \sum {
 * 		[(x_i - x_0)^2 + (y_i - y_0)^2 + (z_i - z_0)^2] +
 * 		[a(x_i - x_0) + b(y_i - y_0) + c(z_i - z_0)]^2
 * 	}
 * 	= \sum {
 * 		[c(y_i - y_0) - b(z_i - z_0)]^2 +
 * 		[a(z_i - z_0) - c(x_i - x_0)]^2 +
 * 		[b(x_i - x_0) - a(y_i - y_0)]^2
 * 	}
 * differentiating by x_0, y_0, z_0, and setting to zero (stationary points)
 *	\sum {a(x_i - x_0) + b(y_i - y_0) + c(z_i - z_0)}
 *		= \sum (x_i - x_0) / a
 *		= \sum (y_i - y_0) / b
 *		= \sum (z_i - z_0) / c
 *
 * these equations are ok for x_0, y_0, z_0 such that
 * 	x_0 = \sum x_i / N
 * 	y_0 = \sum y_i / N
 * 	z_0 = \sum z_i / N
 *
 * Consider a plane whose normal vector (a b c) and passes centroid C(x_0, y_0, z_0)
 * d_i^2 = D0_i^2 - D_i^2
 *	D_i: distance to plane
 *	D0_i: distance to centroid
 * since \sum D0_i^2 is constant,
 * a problem to minimize \sum d_i^2 is equiavalent to that to maximize \sum D_i^2,
 * which can be solved as "3D: plane" case except largest eigenvalue instead of smallest eigenvalue.
 *
 *	http://mathforum.org/library/drmath/view/69103.html
 */

/*
 *	x_0 = \sum x_i / N
 *	y_0 = \sum y_i / N
 *	z_0 = \sum z_i / N
 */
void	kLinearLeastSquare::set_centroid(const std::vector<D3DXVECTOR3>& v, D3DXVECTOR3& centroid)
{
	centroid = D3DXVECTOR3(0, 0, 0);
	for (int n = 0; n < (int) v.size(); ++n)
		centroid += v.at(n);
	centroid /= (float) v.size();
}
/*
 *	A^T A = symmetric 3x3 matrix
 *		= \sum (x_i - x_0)^2		\sum (x_i - x_0)(y_i - y_0)	\sum (x_i - x_0)(z_i - z_0)
 *		  \sum (y_i - y_0)(x_i - x_0)	\sum (y_i - y_0)^2		\sum (y_i - y_0)(z_i - z_0)
 *		  \sum (z_i - z_0)(x_i - x_0)	\sum (z_i - z_0)(y_i - y_0)	\sum (z_i - z_0)^2
 *
 */
void	kLinearLeastSquare::set_symetric_3x3_matrix(const std::vector<D3DXVECTOR3>& v, const D3DXVECTOR3& centroid, kLinearAlgebra::FT* _3x3)
{
	// 0
	// 1 2
	// 3 4 5
	for (int i = 0; i < 6; ++i)
		_3x3[i] = (kLinearAlgebra::FT) 0;
	// [0]: \sum (x_i - x_0)^2
	// [1]: \sum (y_i - y_0)(x_i - x_0)
	// [2]: \sum (y_i - y_0)^2
	// [3]: \sum (z_i - z_0)(x_i - x_0)
	// [4]: \sum (z_i - z_0)(y_i - y_0)
	// [5]: \sum (z_i - z_0)^2
	for (int n = 0; n < (int) v.size(); ++n) {
		D3DXVECTOR3	w = v.at(n) - centroid;
		_3x3[0] += w.x * w.x;
		_3x3[1] += w.y * w.x;
		_3x3[2] += w.y * w.y;
		_3x3[3] += w.z * w.x;
		_3x3[4] += w.z * w.y;
		_3x3[5] += w.z * w.z;
	}
}
void	kLinearLeastSquare::linear_least_square_line_3(const std::vector<D3DXVECTOR3>& v, D3DXVECTOR3& centroid, D3DXVECTOR3& direction_vector)
{
#if 1	// same as in least_square_plane_3
	set_centroid(v, centroid);
	kLinearAlgebra::FT	_3x3[6];
	set_symetric_3x3_matrix(v, centroid, _3x3);
	kLinearAlgebra::FT	eigenvalues[3];
	kLinearAlgebra::FT	eigenvectors[9];
	kLinearAlgebra::eigen_symmetric(_3x3, 3, eigenvectors, eigenvalues);
#endif	// same as in least_square_plane_3

	// eigenvector corresponding to the largest eigenvalue
	direction_vector[0] = (float) eigenvectors[0];
	direction_vector[1] = (float) eigenvectors[1];
	direction_vector[2] = (float) eigenvectors[2];
}
void	kLinearLeastSquare::linear_least_square_plane_3(const std::vector<D3DXVECTOR3>& v, D3DXVECTOR3& centroid, D3DXVECTOR3& normal_vector)
{
#if 1	// same as in least_square_line_3
	set_centroid(v, centroid);
	kLinearAlgebra::FT	_3x3[6];
	set_symetric_3x3_matrix(v, centroid, _3x3);
	kLinearAlgebra::FT	eigenvalues[3];
	kLinearAlgebra::FT	eigenvectors[9];
	kLinearAlgebra::eigen_symmetric(_3x3, 3, eigenvectors, eigenvalues);
#endif	// same as in least_square_line_3

	// eigenvector corresponding to the smallest eigenvalue
	normal_vector[0] = (float) eigenvectors[6];
	normal_vector[1] = (float) eigenvectors[7];
	normal_vector[2] = (float) eigenvectors[8];
}
