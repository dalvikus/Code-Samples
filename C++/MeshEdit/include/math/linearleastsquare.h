#pragma once
#ifndef __LINEAR_LEAST_SQUARE__
#define __LINEAR_LEAST_SQUARE__


#include <vector>
#include <DXUT/Core/DXUT.h>
#include <math/eigenvalue.h>

class kLinearLeastSquare
{
public:
	static void	set_centroid(const std::vector<D3DXVECTOR3>& v, D3DXVECTOR3& centroid);
	static void	set_symetric_3x3_matrix(const std::vector<D3DXVECTOR3>& v, const D3DXVECTOR3& centroid, kLinearAlgebra::FT* _3x3);
	static void	linear_least_square_line_3(const std::vector<D3DXVECTOR3>& v, D3DXVECTOR3& centroid, D3DXVECTOR3& direction_vector);
	static void	linear_least_square_plane_3(const std::vector<D3DXVECTOR3>& v, D3DXVECTOR3& centroid, D3DXVECTOR3& normal_vector);
};


#endif	// !__LINEAR_LEAST_SQUARE__
