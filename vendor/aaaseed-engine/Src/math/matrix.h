
#ifdef AAA_MATRIX_H
#error "MATRIX_H included more than once."
#endif
#define AAA_MATRIX_H 1


#ifndef	AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif


template <class T>	FINLINE	T	matrix_44_row_dot_v3_1(	T CONST * CONST m, T CONST * CONST src )	{ return src[0]*m[0] + src[1]*m[4] + src[2]*m[8] +        m[12]; }
template <class T>	FINLINE	T	matrix_44_row_dot_v4(	T CONST * CONST m, T CONST * CONST src )	{ return src[0]*m[0] + src[1]*m[4] + src[2]*m[8] + src[3]*m[12]; }

template <class T>	FINLINE	T	matrix_44_mul_x_v3_1(	T CONST * CONST m, T CONST * CONST src )	{ return matrix_44_row_dot_v3_1( m,   src);	}
template <class T>	FINLINE	T	matrix_44_mul_y_v3_1(	T CONST * CONST m, T CONST * CONST src )	{ return matrix_44_row_dot_v3_1( m+1, src);	}
template <class T>	FINLINE	T	matrix_44_mul_z_v3_1(	T CONST * CONST m, T CONST * CONST src )	{ return matrix_44_row_dot_v3_1( m+2, src);	}
template <class T>	FINLINE	T	matrix_44_mul_w_v3_1(	T CONST * CONST m, T CONST * CONST src )	{ return matrix_44_row_dot_v3_1( m+3, src);	}

template <class T>	FINLINE	T	matrix_44_mul_x_v4(		T CONST * CONST m, T CONST * CONST src )	{ return matrix_44_row_dot_v4(	 m,	  src); }
template <class T>	FINLINE	T	matrix_44_mul_y_v4(		T CONST * CONST m, T CONST * CONST src )	{ return matrix_44_row_dot_v4(	 m+1, src); }
template <class T>	FINLINE	T	matrix_44_mul_z_v4(		T CONST * CONST m, T CONST * CONST src )	{ return matrix_44_row_dot_v4(	 m+2, src); }
template <class T>	FINLINE	T	matrix_44_mul_w_v4(		T CONST * CONST m, T CONST * CONST src )	{ return matrix_44_row_dot_v4(	 m+3, src); }



template <class T>
FINLINE	void	matrix_43_mul_v3( T CONST * CONST m, T* CONST vec )
{
	T z     = matrix_44_mul_z_v3_1( m, vec );
	T y     = matrix_44_mul_y_v3_1( m, vec );
	vec[0]	= matrix_44_mul_x_v3_1( m, vec );
	vec[1]	= y;
	vec[2]	= z;
}

template <class T>
FINLINE	void	matrix_43_mul_v3( T CONST * CONST m, T* CONST dst, T CONST * CONST src )
{
	dst[0]	= matrix_44_mul_x_v3_1( m, src );
	dst[1]	= matrix_44_mul_y_v3_1( m, src );
	dst[2]	= matrix_44_mul_z_v3_1( m, src );
}

//template <class T>
//FINLINE	void	matrix_44_mul_v4( T CONST * CONST m, T* CONST dst, T CONST * CONST src )
//{
//	dst[0]	= matrix_44_mul_x_v4( m, src );
//	dst[1]	= matrix_44_mul_y_v4( m, src );
//	dst[2]	= matrix_44_mul_z_v4( m, src );
//	dst[3]	= matrix_44_mul_w_v4( m, src );
//}

template <class T>
FINLINE	void	matrix_44_mul_v3_1( T CONST * CONST m, T* CONST dst, T CONST * CONST src )
{
	dst[0]	= matrix_44_mul_x_v3_1( m, src );
	dst[1]	= matrix_44_mul_y_v3_1( m, src );
	dst[2]	= matrix_44_mul_z_v3_1( m, src );
	dst[3]	= matrix_44_mul_w_v3_1( m, src );
}

template <class T>
FINLINE	void	matrix_44_mul_v3( T CONST * CONST m, T * CONST vec )
{
	T v[4];
	matrix_44_mul_v3_1( m, v, vec );
	scale_v3( vec, v, T(1.)/v[3] );
}

// yes I know but ... (Maa)
#define DOT( i, j )		dst[i+4*j] = a[i+4*0]*b[0+4*j] + a[i+4*1]*b[1+4*j] + a[i+4*2]*b[2+4*j] + a[i+4*3]*b[3+4*j]
template <class T>
FINLINE	void	matrix_44_mul( T* dst, T CONST * CONST a, T CONST * CONST b )
{
   DOT(0,0);	DOT(0,1);	DOT(0,2);	DOT(0,3);
   DOT(1,0);	DOT(1,1);	DOT(1,2);	DOT(1,3);
   DOT(2,0);	DOT(2,1);	DOT(2,2);	DOT(2,3);
   DOT(3,0);	DOT(3,1);	DOT(3,2);	DOT(3,3);
}

template <class T>
FINLINE	void	matrix44_identity( T m[16] )
{
	//MEMCPY( m, Identity, 16*sizeof(T) );
	m[0+4*0] = 1; m[0+4*1] = 0; m[0+4*2] = 0; m[0+4*3] = 0;
	m[1+4*0] = 0; m[1+4*1] = 1; m[1+4*2] = 0; m[1+4*3] = 0;
	m[2+4*0] = 0; m[2+4*1] = 0; m[2+4*2] = 1; m[2+4*3] = 0;
	m[3+4*0] = 0; m[3+4*1] = 0; m[3+4*2] = 0; m[3+4*3] = 1;
}


/*
 * Compute the inverse of a 4x4 matrix.
 *
 * From an algorithm by V. Strassen, 1969, _Numerishe Mathematik_, vol. 13,
 * pp. 354-356.
 * 60 multiplies, 24 additions, 10 subtractions, 8 negations, 2 divisions,
 * 48 assignments, _0_ branches
 *
 * This implementation by Scott McCaskill
 */

enum
{
	M00 = 0, M01 = 4, M02 = 8, M03 = 12,
	M10 = 1, M11 = 5, M12 = 9, M13 = 13,
	M20 = 2, M21 = 6, M22 = 10,M23 = 14,
	M30 = 3, M31 = 7, M32 = 11,M33 = 15
};

template <class T>
FINLINE	void	matrix_44_add( T *dst, T CONST * a )
{
	*dst += *a; 	
	*++dst += *++a;
	*++dst += *++a;
	*++dst += *++a;
	*++dst += *++a;
	*++dst += *++a;
	*++dst += *++a;
	*++dst += *++a;
	*++dst += *++a;
	*++dst += *++a;
	*++dst += *++a;
	*++dst += *++a;
	*++dst += *++a;
	*++dst += *++a;
	*++dst += *++a;
	*++dst += *++a;
}

template <class T>
void	matrix_44_invert_general( T * out, T CONST * m )
{
typedef	T		Mat22[2][2];
		Mat22	r1, r2, r3, r4, r5, r6, r7;
CONST	T*		A = m;
		T*		C = out;
		T		one_over_det;

	//	A is the 4x4 source matrix (to be inverted ).
	//	C is the 4x4 destination matrix
	//	a11 is the 2x2 matrix in the upper left quadrant of A
	//	a12 is the 2x2 matrix in the upper right quadrant of A
	//	a21 is the 2x2 matrix in the lower left quadrant of A
	//	a22 is the 2x2 matrix in the lower right quadrant of A
	//	similarly, cXX are the 2x2 quadrants of the destination matrix

	//	R1 = inverse( a11 )
	one_over_det = 1.0f / ( ( A[M00] * A[M11] ) - ( A[M10] * A[M01] ) );
	r1[0][0] = one_over_det * A[M11];
	r1[0][1] = one_over_det * -A[M01];
	r1[1][0] = one_over_det * -A[M10];
	r1[1][1] = one_over_det * A[M00];

	//	R2 = a21 x R1
	r2[0][0] = A[M20] * r1[0][0] + A[M21] * r1[1][0];
	r2[0][1] = A[M20] * r1[0][1] + A[M21] * r1[1][1];
	r2[1][0] = A[M30] * r1[0][0] + A[M31] * r1[1][0];
	r2[1][1] = A[M30] * r1[0][1] + A[M31] * r1[1][1];

	//	R3 = R1 x a12
	r3[0][0] = r1[0][0] * A[M02] + r1[0][1] * A[M12];
	r3[0][1] = r1[0][0] * A[M03] + r1[0][1] * A[M13];
	r3[1][0] = r1[1][0] * A[M02] + r1[1][1] * A[M12];
	r3[1][1] = r1[1][0] * A[M03] + r1[1][1] * A[M13];

	//	R4 = a21 x R3
	r4[0][0] = A[M20] * r3[0][0] + A[M21] * r3[1][0];
	r4[0][1] = A[M20] * r3[0][1] + A[M21] * r3[1][1];
	r4[1][0] = A[M30] * r3[0][0] + A[M31] * r3[1][0];
	r4[1][1] = A[M30] * r3[0][1] + A[M31] * r3[1][1];

	//	R5 = R4 - a22
	r5[0][0] = r4[0][0] - A[M22];
	r5[0][1] = r4[0][1] - A[M23];
	r5[1][0] = r4[1][0] - A[M32];
	r5[1][1] = r4[1][1] - A[M33];

	//	R6 = inverse( R5 )
	one_over_det = 1.0f / ( ( r5[0][0] * r5[1][1] ) - ( r5[1][0] * r5[0][1] ) );
	r6[0][0] = one_over_det * r5[1][1];
	r6[0][1] = one_over_det * -r5[0][1];
	r6[1][0] = one_over_det * -r5[1][0];
	r6[1][1] = one_over_det * r5[0][0];

	//	c12 = R3 x R6
	C[M02] = r3[0][0] * r6[0][0] + r3[0][1] * r6[1][0];
	C[M03] = r3[0][0] * r6[0][1] + r3[0][1] * r6[1][1];
	C[M12] = r3[1][0] * r6[0][0] + r3[1][1] * r6[1][0];
	C[M13] = r3[1][0] * r6[0][1] + r3[1][1] * r6[1][1];

	//	c21 = R6 x R2
	C[M20] = r6[0][0] * r2[0][0] + r6[0][1] * r2[1][0];
	C[M21] = r6[0][0] * r2[0][1] + r6[0][1] * r2[1][1];
	C[M30] = r6[1][0] * r2[0][0] + r6[1][1] * r2[1][0];
	C[M31] = r6[1][0] * r2[0][1] + r6[1][1] * r2[1][1];

	//	R7 = R3 x c21
	r7[0][0] = r3[0][0] * C[M20] + r3[0][1] * C[M30];
	r7[0][1] = r3[0][0] * C[M21] + r3[0][1] * C[M31];
	r7[1][0] = r3[1][0] * C[M20] + r3[1][1] * C[M30];
	r7[1][1] = r3[1][0] * C[M21] + r3[1][1] * C[M31];

	//	c11 = R1 - R7
	C[M00] = r1[0][0] - r7[0][0];
	C[M01] = r1[0][1] - r7[0][1];
	C[M10] = r1[1][0] - r7[1][0];
	C[M11] = r1[1][1] - r7[1][1];

	//	c22 = -R6
	C[M22] = -r6[0][0];
	C[M23] = -r6[0][1];
	C[M32] = -r6[1][0];
	C[M33] = -r6[1][1];
}

/*
 * Invert matrix m.  This algorithm contributed by Stephane Rehel
 * <rehel@worldnet.fr>
 */
/* NB. OpenGL Matrices are COLUMN major. */
#define	MAT(m,r,c)	(m)[(c)*4+(r)]

/* Here's some shorthand converting standard (row,column) to index. */
#define	m11	MAT(m,0,0)
#define	m12	MAT(m,0,1)
#define	m13	MAT(m,0,2)
#define	m14	MAT(m,0,3)
#define	m21	MAT(m,1,0)
#define	m22	MAT(m,1,1)
#define	m23	MAT(m,1,2)
#define	m24	MAT(m,1,3)
#define	m31	MAT(m,2,0)
#define	m32	MAT(m,2,1)
#define	m33	MAT(m,2,2)
#define	m34	MAT(m,2,3)
#define	m41	MAT(m,3,0)
#define	m42	MAT(m,3,1)
#define	m43	MAT(m,3,2)
#define	m44	MAT(m,3,3)

template <class T>
void	matrix_44_invert( T * CONST out, T CONST * CONST m )
{
	T det;
	T tmp[16];	// Allow out == in

	if( m41 != 0. || m42 != 0. || m43 != 0. || m44 != 1. )
	{
		matrix_44_invert_general( out, m );
		return;
	}

	//	Inverse = adjoint / det. (See linear algebra texts.)
	tmp[0]= m22 * m33 - m23 * m32;
	tmp[1]= m23 * m31 - m21 * m33;
	tmp[2]= m21 * m32 - m22 * m31;

	//	Compute determinant as early as possible using these cofactors.
	det= m11 * tmp[0] + m12 * tmp[1] + m13 * tmp[2];

	//	Run singularity test.
	if( det == T(0.) )
	{
		//	DBG_PRINT_STRING("invert_matrix: Warning: Singular matrix.");
		matrix44_identity( out );
	}
	else
	{
		T d12, d13, d23, d24, d34, d41;
		T im11, im12, im13, im14;

		det = T(DOUBLE(1.)/(DOUBLE)det);

		//	Compute rest of inverse.
		tmp[0] *= det;
		tmp[1] *= det;
		tmp[2] *= det;
		tmp[3]  = 0.;

		im11= m11 * det;
		im12= m12 * det;
		im13= m13 * det;
		im14= m14 * det;
		tmp[4] = im13 * m32 - im12 * m33;
		tmp[5] = im11 * m33 - im13 * m31;
		tmp[6] = im12 * m31 - im11 * m32;
		tmp[7] = 0.;

		//	Pre-compute 2x2 dets for first two rows when computing
		//	cofactors of last two rows.
		d12 = im11*m22 - m21*im12;
		d13 = im11*m23 - m21*im13;
		d23 = im12*m23 - m22*im13;
		d24 = im12*m24 - m22*im14;
		d34 = im13*m24 - m23*im14;
		d41 = im14*m21 - m24*im11;

		tmp[8] =  d23;
		tmp[9] = -d13;
		tmp[10] = d12;
		tmp[11] = 0.;

		tmp[12] = -(m32 * d34 - m33 * d24 + m34 * d23);
		tmp[13] =  (m31 * d34 + m33 * d41 + m34 * d13);
		tmp[14] = -(m31 * d24 + m32 * d41 + m34 * d12);
		tmp[15] =  1.;
		// use memcpy instead of MEMCPY to help compiler optinmize
		memcpy( out, (CONST T *) tmp, 16 * sizeof(T) );	
	}
}

#undef m11
#undef m12
#undef m13
#undef m14
#undef m21
#undef m22
#undef m23
#undef m24
#undef m31
#undef m32
#undef m33
#undef m34
#undef m41
#undef m42
#undef m43
#undef m44
#undef MAT
