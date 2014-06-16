#pragma once
#ifndef __LINEAR_ALGEBRA__
#define __LINEAR_ALGEBRA__


#include <DXUT/Core/DXUT.h>
#include <cmath>

class kLinearAlgebra
{
public:
	typedef double	FT;

	static void
	eigen_symmetric(
		const FT	*mat,
		const int	n,
		FT*		eigen_vectors,
		FT*		eigen_values,
		const int	MAX_ITER = 100
	);
	

	static void
	eigen_symmetric_2(
		const FT	*_2x2,	// a
					// b c
		FT	*eigen_vectors,
		FT	*eigen_values
	);
};

#if 0	// usage
#include <cassert>
/*
  // solve for eigenvalues and eigenvectors.
  // eigen values are sorted in descending order,
  // eigen vectors are sorted in accordance.
  FT eigen_values[3];
  FT eigen_vectors[9];
  eigen_symmetric<FT>(covariance,3,eigen_vectors,eigen_values);

 */

#include <iostream>
using std::cout;
using std::endl;

int	main()
{
	const FT mat[6] = {
		(FT) 1,
		(FT) -1, (FT) 2,
		(FT) 0, (FT) -1, (FT) 1
//		(FT) 3,
//		(FT) 0, (FT) 1,
//		(FT) 0, (FT) 0, (FT) 2
	};
	FT	eigen_values[3];
	FT	eigen_vectors[9];
	eigen_symmetric(mat,3,eigen_vectors,eigen_values);
	for (int i = 0; i < 3; ++i) {
		cout << "eigen_values[" << i << "]: " << eigen_values[i] << endl;
		cout << "eigen_vectors[" << i << "]: [" << eigen_vectors[3 * i + 0] << ", " << eigen_vectors[3 * i + 1] << ", " << eigen_vectors[3 * i + 2] << "]" << endl;
	}

{
	const FT	_2x2[3] = {
		(FT) 2.0,
		(FT) 0.0, (FT) 1.0
//		(FT) 2.0,
//		(FT) -1.0, (FT) 2.0
	};
	FT	eigen_values[2];
	FT	eigen_vectors[2 * 2];
	eigen_symmetric_2(_2x2, eigen_vectors, eigen_values);
	for (int i = 0; i < 2; ++i) {
		cout << "eigen_values[" << i << "]: " << eigen_values[i] << endl;
		cout << "eigen_vectors[" << i << "]: [" << eigen_vectors[2 * i + 0] << ", " << eigen_vectors[2 * i + 1] << "]" << endl;
	}
#if 1
{
	FT	eigen_values[2];
	FT	eigen_vectors[2 * 2];
	eigen_symmetric(_2x2, 2, eigen_vectors, eigen_values);
	for (int i = 0; i < 2; ++i) {
		cout << "eigen_values[" << i << "]: " << eigen_values[i] << endl;
		cout << "eigen_vectors[" << i << "]: [" << eigen_vectors[2 * i + 0] << ", " << eigen_vectors[2 * i + 1] << "]" << endl;
	}
}
#endif
}

	return 0;
}
#endif


#endif	// !__LINEAR_ALGEBRA__
