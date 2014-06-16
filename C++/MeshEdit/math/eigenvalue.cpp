#include <math/eigenvalue.h>
#include <cmath>

void	kLinearAlgebra::eigen_symmetric(
	const FT	*mat,
	const int	n,
	FT*		eigen_vectors,
	FT*		eigen_values,
	const int	MAX_ITER
)
{
  static const FT EPSILON = (FT)0.00001;

  // number of entries in mat
  int nn = (n*(n+1))/2;

  // copy matrix
  FT *a = new FT[nn];
  int ij;
  for(ij=0; ij<nn; ij++)
    a[ij] = mat[ij];
  // Fortran-porting
  a--;

  // init diagonalization matrix as the unit matrix
  FT *v = new FT[n*n];
  ij = 0;
  int i;
  for(i=0; i<n; i++)
    for(int j=0; j<n; j++)
      if(i==j)
	v[ij++] = 1.0;
      else
	v[ij++] = 0.0;
  // Fortran-porting
  v--;

  // compute weight of the non diagonal terms
  ij = 1;
  FT a_norm = 0.0;
  for(i=1; i<=n; i++)
    for(int j=1; j<=i; j++)
    {
      if( i!=j )
      {
	FT a_ij = a[ij];
	a_norm += a_ij * a_ij;
      }
      ij++;
    }

  if(a_norm != 0.0)
  {
    FT a_normEPS = a_norm * EPSILON;
    FT thr = a_norm;

    // rotations
    int nb_iter = 0;
    while(thr > a_normEPS && nb_iter < MAX_ITER)
    {
      nb_iter++;
      FT thr_nn = thr / nn;

      for(int l=1; l< n; l++)
      {
	for(int m=l+1; m<=n; m++)
	{
	  // compute sinx and cosx
	  int lq = (l*l-l)/2;
	  int mq = (m*m-m)/2;

	  int lm = l + mq;
	  FT a_lm = a[lm];
	  FT a_lm_2 = a_lm * a_lm;

	  if(a_lm_2 < thr_nn)
	    continue;

	  int ll   = l + lq;
	  int mm   = m + mq;
	  FT a_ll = a[ll];
	  FT a_mm = a[mm];

	  FT delta = a_ll - a_mm;

	  FT x;
	  if(delta == 0.0)
	    x = (FT) - D3DX_PI / 4;
	  else
	    x = (FT)(- std::atan( (a_lm+a_lm) / delta ) / 2.0);

	  FT sinx    = std::sin(x);
	  FT cosx    = std::cos(x);
	  FT sinx_2  = sinx * sinx;
	  FT cosx_2  = cosx * cosx;
	  FT sincos  = sinx * cosx;

	  // rotate L and M columns
	  int ilv = n*(l-1);
	  int imv = n*(m-1);

	  int i;
	  for( i=1; i<=n;i++ )
	  {
	    if( (i!=l) && (i!=m) )
	    {
	      int iq = (i*i-i)/2;

	      int im;
	      if( i<m )
		im = i + mq;
	      else
		im = m + iq;
	      FT a_im = a[im];

	      int il;
	      if( i<l )
		il = i + lq;
	      else
		il = l + iq;
	      FT a_il = a[il];

	      a[il] = a_il * cosx - a_im * sinx;
	      a[im] = a_il * sinx + a_im * cosx;
	    }

	    ilv++;
	    imv++;

	    FT v_ilv = v[ilv];
	    FT v_imv = v[imv];

	    v[ilv] = cosx * v_ilv - sinx * v_imv;
	    v[imv] = sinx * v_ilv + cosx * v_imv;
	  }

	  x = a_lm * sincos;
	  x += x;

	  a[ll] =  a_ll * cosx_2 + a_mm * sinx_2 - x;
	  a[mm] =  a_ll * sinx_2 + a_mm * cosx_2 + x;
	  a[lm] =  0.0;

	  thr = std::fabs(thr - a_lm_2);
	}
      }
    }
  }

  // convert indices and copy eigen values
  a++;
  for(i=0; i<n; i++)
  {
    int k = i + (i*(i+1))/2;
    eigen_values[i] = a[k];
  }
  delete [] a;

  // sort eigen values and vectors
  int *index = new int[n];
  for(i=0; i<n; i++)
    index[i] = i;

  for(i=0; i<(n-1); i++)
  {
    FT x = eigen_values[i];
    int k = i;

    for(int j=i+1; j<n; j++)
      if(x < eigen_values[j])
      {
	k = j;
	x = eigen_values[j];
      }

    eigen_values[k] = eigen_values[i];
    eigen_values[i] = x;

    int jj = index[k];
    index[k] = index[i];
    index[i] = jj;
  }


  // save eigen vectors
  v++; // back to C++
  ij = 0;
  for(int k=0; k<n; k++ )
  {
    int ik = index[k]*n;
    for(int i=0; i<n; i++)
      eigen_vectors[ij++] = v[ik++];
  }

  delete [] v;
  delete [] index;
}

void	kLinearAlgebra::eigen_symmetric_2(
	const FT	*_2x2,	// a
				// b c
	FT	*eigen_vectors,
	FT	*eigen_values
)
{
	// for better reading
	FT	a = _2x2[0];
	FT	b = _2x2[1];
	FT	c = _2x2[2];
	assert(a > 0 && a * c - b * b);	// positive definite

	FT	D = c*c - 2 * a * c + 4 * b * b + a * a;

	// degenerate or isotropic case
	if(D == 0.0) {
		// unit eigen values by default
		eigen_values[0] = eigen_values[1] = (FT) 1.0;

		// any vector is eigen vector
		// the 2D canonical frame is output by default
		eigen_vectors[2 * 0 + 0] = (FT) 1.0;
		eigen_vectors[2 * 0 + 1] = (FT) 0.0;
		eigen_vectors[2 * 1 + 0] = (FT) 0.0;
		eigen_vectors[2 * 1 + 1] = (FT) 1.0;
	} else {
		if(b == 0.0) {
			if(a >= c) {
				eigen_vectors[0] = a;
				eigen_vectors[2 * 0 + 0] = (FT) 1.0;
				eigen_vectors[2 * 0 + 1] = (FT) 0.0;

				eigen_vectors[1] = c;
				eigen_vectors[2 * 1 + 0] = (FT) 0.0;
				eigen_vectors[2 * 1 + 1] = (FT) 1.0;
			} else {
				eigen_vectors[0] = c;
				eigen_vectors[2 * 0 + 0] = (FT) 0.0;
				eigen_vectors[2 * 0 + 1] = (FT) 1.0;

				eigen_vectors[1] = a;
				eigen_vectors[2 * 1 + 0] = (FT) 1.0;
				eigen_vectors[2 * 1 + 1] = (FT) 0.0;
			}
		} else {
#if 1
			// lambda1 = [(a + c) + sqrt(D)] / 2
			// lambda2 = [(a + c) - sqrt(D)] / 2
			// a, c 모두 양수이므로(positive definite),
			// lambda1 >= lambda2 이다.
			FT	l1 = (FT) (0.5 * ((a + c) + std::sqrt(D)));
			FT	l2 = (FT) (0.5 * ((a + c) - std::sqrt(D)));

			// all eigen values of a symmetric positive
			// definite matrix must be real and positive
			// we saturate the values if this is not the
			// case for floating point computations.
			l1 = (l1 < (FT) 0.0) ? (FT) 0.0 : l1;
			l2 = (l2 < (FT) 0.0) ? (FT) 0.0 : l2;

			FT	x1 = (FT) 1.0;
			FT	y1 = -(a - l1) / b;
//			FT	x2 = (FT) 1.0;
//			FT	y2 = -(a - l2) / b;
			FT	L1 = std::sqrt(x1 * x1 + y1 * y1);
//			FT	L2 = std::sqrt(x2 * x2 + y2 * y2);
			x1 *= (FT) -1.0;
			y1 *= (FT) -1.0;
			// sort eigen values and vectors in descendent order.
			eigen_values[0] = l1;
			eigen_vectors[2 * 0 + 0] = x1 / L1;
			eigen_vectors[2 * 0 + 1] = y1 / L1;

			eigen_values[1] = l2;
#if 1	// RHS
			eigen_vectors[2 * 1 + 0] = -y1 / L1;
			eigen_vectors[2 * 1 + 1] = x1 / L1;
#else
			eigen_vectors[2 * 1 + 0] = x2 / L2;
			eigen_vectors[2 * 1 + 1] = y2 / L2;
#endif
#else
			FT	l1 = (FT) (0.5 * (-1 * std::sqrt(D) + c + a));
			FT	l2 = (FT) (0.5 * (     std::sqrt(D) + c + a));

			// all eigen values of a symmetric positive
			// definite matrix must be real and positive
			// we saturate the values if this is not the
			// case for floating point computations.
			l1 = (l1 < (FT) 0.0) ? (FT) 0.0 : l1;
			l2 = (l2 < (FT) 0.0) ? (FT) 0.0 : l2;

			// sort eigen values and vectors in descendent order.
			if (l1 >= l2) {
				eigen_values[0] = l1;
				eigen_vectors[2 * 0 + 0] = (FT) 1.0;
				eigen_vectors[2 * 0 + 1] = (FT) (-(std::sqrt(D) - c + a) / (2 * b));

				eigen_values[1] = l2;
				eigen_vectors[2 * 1 + 0] = (FT) 1.0;
				eigen_vectors[2 * 1 + 1] = (FT) ( (std::sqrt(D) + c - a) / (2 * b));
			} else {
				eigen_values[0] = l2;
				eigen_vectors[2 * 0 + 0] = (FT) 1.0;
				eigen_vectors[2 * 0 + 1] = (FT) ( (std::sqrt(D) + c - a) / (2 * b));

				eigen_values[1] = l1;
				eigen_vectors[2 * 1 + 0] = (FT) 1.0;
				eigen_vectors[2 * 1 + 1] = (FT) (-(std::sqrt(D) - c + a) / (2 * b));
			}
#endif
		} // end generic case
	}
}
