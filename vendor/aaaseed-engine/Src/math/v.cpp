#include "math/v.h"

//INTEGRATE
REAL	integrate_1d_real( REAL CONST * p, REAL CONST min, REAL CONST max )
{
	REAL	out;
	INT32	i_min = I_FLOOR( min );
	INT32	i_max = I_FLOOR( max );
	if( i_min == i_max )
		out = *( p+i_min ) * ( max-min );
	else
	{
		p += i_min;
		out = REAL( *p * ( i_min+1-min ) );
//		ERR_PRINT_STRING( "%d\t:\t%g", i_min, *p );
		while( ++i_min < i_max )
		{
			++p;
//			ERR_PRINT_STRING( "%d\t:\t%g", i_min, *p );
			out += *p;
		}
		++p;
//		ERR_PRINT_STRING( "%d\t:\t%g", i_min, *p );
		out += *p * ( max-i_max );
	}
	return out;
}

//DIST
CONSTEXPR	REAL	dist_squared_v3r( REAL CONST * a, REAL CONST * b )
{
#if	AAA_USE_XNA()
	if( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
		return	dist_squared_v3_xna( a, b );
#endif
	{
		REAL	tmp = *a - *b;
		DOUBLE	size = tmp * tmp;
#if AAA_V_DOIT_WITH_INC()
		tmp = *++a - *++b;
		size += tmp * tmp;
		tmp = *++a - *++b;
#else
		tmp = *(a+1) - *(b+1);
		size += tmp * tmp;
		tmp = *(a+2) - *(b+2);
#endif
		return REAL( size + tmp * tmp );
	}
}

#if	!AAA_REAL_IS_DOUBLE()
CONSTEXPR	REAL	dist_squared_v3r( REAL CONST * a, DOUBLE CONST * b )
{
	REAL	tmp = *a - (REAL)*b;
	DOUBLE	size = tmp * tmp;
#if AAA_V_DOIT_WITH_INC()
	tmp = *++a - (REAL)*++b;
	size += tmp * tmp;
	tmp = *++a - (REAL)*++b;
#else
	tmp = *(a+1) - (REAL)*(b+1);
	size += tmp * tmp;
	tmp = *(a+2) - (REAL)*(b+2);
#endif
	return REAL(size + tmp * tmp);
}
#endif
#if	!AAA_REAL_IS_DOUBLE()
CONSTEXPR	DOUBLE	dist_squared_v3( DOUBLE CONST * a, DOUBLE CONST * b )
{
	DOUBLE	tmp = *a - *b;
	DOUBLE	size = tmp * tmp;
#if AAA_V_DOIT_WITH_INC()
	tmp = *++a - *++b;
	size += tmp * tmp;
	tmp = *++a - *++b;
#else
	tmp = *(a+1) - *(b+1);
	size += tmp * tmp;
	tmp = *(a+2) - *(b+2);
#endif
	return size + tmp * tmp;
}
#endif
FINLINE	REAL	dist_v3r( REAL CONST * CONST a, REAL CONST * CONST b )
{
#if	AAA_USE_XNA()
	if( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
		return	dist_v3_xna( a, b );
#endif
	return SQRT(dist_squared_v3r( a, b ));
}

#if	!AAA_REAL_IS_DOUBLE()
FINLINE	REAL	dist_v3r( REAL CONST * CONST a, DOUBLE CONST * CONST b )
{
	return SQRT(dist_squared_v3r( a, b ));
}
FINLINE	DOUBLE	dist_v3r( DOUBLE CONST * CONST a, DOUBLE CONST * CONST b )
{
	return SQRT(dist_squared_v3( a, b ));
}
#endif

CONSTEXPR	REAL	dist_box_v3r( REAL CONST * a, REAL CONST * b )
{
#if	AAA_USE_XNA()
	if( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
		return	dist_box_v3_xna( a, b );
#endif
	{
		REAL tmp = *a - *b;
		REAL size = ABS( tmp );
		tmp = *++a - *++b;
		size += ABS( tmp );
		tmp = *++a - *++b;
		return size + ABS( tmp );
	}
}

CONSTEXPR	REAL	dist_box_v3r( REAL CONST * a, REAL CONST x, REAL CONST y, REAL CONST z )
{
#if	AAA_USE_XNA()
	if( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
		return	dist_box_v3_xna( a, x, y, z );
#endif
	{
		REAL tmp = *a - x;
		REAL size = ABS( tmp );
		tmp = *++a - y;
		size += ABS( tmp );
		tmp = *++a - z;
		return size + ABS( tmp );
	}
}

//CONSTEXPR	DOUBLE	norm_squared_v3( DOUBLE CONST * a )
//{
//	return sum_squared_v3( a[0], a[1], a[2] );
//}

FINLINE	FP32	norm_squared_v3r( FP32 CONST * a )
{
#if	AAA_USE_XNA()
	if( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
		return	norm_squared_v3_xna( a );
#endif
	return sum_squared_v3r( a[0], a[1], a[2] );
}

FINLINE	FP32	norm_v3r( FP32 CONST * a )
{
#if	AAA_USE_XNA()
	if( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
		return	norm_v3_xna( a );
#endif
	return SQRT( norm_squared_v3r(a) );
}

FINLINE	FP32	norm_squared_v2r( FP32 CONST * a )
{
#if	AAA_USE_XNA()
	if( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
		return	norm_squared_v2_xna( a );
#endif
	return sum_squared_v2r( a[0], a[1] );
}

FINLINE	FP32	norm_v2r( FP32 CONST * a )
{
	return SQRT( norm_squared_v2r(a) );
}

CONSTEXPR	REAL	get_dist_squared_if_in_dist_squared_v3r( REAL CONST * a, REAL CONST dist_squared )
{
#if	AAA_USE_XNA()
	if( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
		return	get_dist_squared_if_in_dist_squared_v3_xna( a, dist_squared );
#endif
	{
		REAL dist = *a;
		dist *= dist;
		if( dist < dist_squared )
		{
#if AAA_V_DOIT_WITH_INC()
			REAL tmp = *++a;
#else
			REAL tmp = *(a+1);
#endif
			dist += tmp * tmp;
			if( dist < dist_squared )
			{
#if AAA_V_DOIT_WITH_INC()
				tmp = *++a;
#else
				tmp = *(a+2);
#endif
				dist += tmp * tmp;
				if( dist < dist_squared )
					return dist;
			}
		}
		return REAL_BIG_VALUE;
	}
}

CONSTEXPR	REAL	get_dist_squared_if_in_dist_squared_v3r( REAL CONST * a, REAL CONST * b, REAL CONST dist_squared )
{
#if	AAA_USE_XNA()
	if( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
		return	get_dist_squared_if_in_dist_squared_v3_xna( a, b, dist_squared );
#endif
	{
		REAL	dist = *a - *b;
		dist *= dist;
		if( dist < dist_squared )
		{
#if AAA_V_DOIT_WITH_INC()
			REAL tmp = *++a - *++b;
			dist += tmp * tmp;
			if( dist < dist_squared )
			{
				tmp = *++a - *++b;
#else
			REAL	tmp = *(a+1) - *(b+1);
			dist += tmp * tmp;
			if( dist < dist_squared )
			{
				tmp = *(a+2) - *(b+2);
#endif
				dist += tmp * tmp;
				if( dist < dist_squared )
					return dist;
			}
		}
		return REAL_BIG_VALUE;
	}
}

CONSTEXPR	bool	is_dist_squared_less_v3r( REAL CONST x, REAL CONST y, REAL CONST z, REAL dist_squared )
{
#if	0	//	AAA_USE_XNA()
	if( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
		return	is_dist_squared_less_v3_xna( a, b, dist_squared );
#endif
	{
		dist_squared -=	x*x;
		if( dist_squared < 0. )
			return false;
		dist_squared -= y*y;
		if( dist_squared < 0. )
			return false;
		return dist_squared > z*z;
	}
}

CONSTEXPR	bool	is_dist_squared_less_v3r( REAL CONST * a, REAL CONST * b, REAL dist_squared )
{
#if	AAA_USE_XNA()
	if( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
		return	is_dist_squared_less_v3_xna( a, b, dist_squared );
#endif
	{
		REAL d = *a - *b;
		dist_squared -= d*d;
		if( dist_squared < 0. )
			return false;
#if AAA_V_DOIT_WITH_INC()
		d = *++a - *++b;
#else
		d = *(a+1) - *(b+1);
#endif
		dist_squared -= d*d;
		if( dist_squared < 0. )
			return false;
#if AAA_V_DOIT_WITH_INC()
		d = *++a - *++b;
#else
		d = *(a+2) - *(b+2);
#endif			
		return dist_squared > d*d;
	}
}

CONSTEXPR	bool	is_dist_squared_inside_v3r( REAL CONST* a, REAL CONST* b, REAL dist_min_squared, REAL dist_max_squared )
{
	REAL d2 = *a - *b;
	d2 *= d2;
	if( d2 < dist_max_squared )
	{
#if AAA_V_DOIT_WITH_INC()
		REAL d = *++a-*++b;
#else
		REAL d = *(a+1) - *(b+1);
#endif
		d2 += d*d;
		if( d2 < dist_max_squared )
		{
#if AAA_V_DOIT_WITH_INC()
			d = *++a-*++b;
#else
			d = *(a+2) - *(b+2);
#endif
			d2 += d*d;		
			return d2 < dist_max_squared && dist_min_squared < d2;
		}
	}
	return false;
}

CONSTEXPR	void	add_scale_add_v3r( REAL* dst, REAL CONST * a, REAL CONST * b, REAL CONST factor )
{
	//if( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
	//{
	//	return	add_scale_add_v3_xna( dst, a, b, factor );
	//}
	//else
	{
		*dst		+= ( *a		+ *b		)	*	factor	;
#if AAA_V_DOIT_WITH_INC() 
		*++dst		+= ( *++a	+ *++b		)	*	factor	;
		*++dst		+= ( *++a	+ *++b		)	*	factor	;
#else
		*(dst+1)	+= ( *(a+1)	+ *(b+1)	)	*	factor	;
		*(dst+2)	+= ( *(a+2)	+ *(b+2)	)	*	factor	;
#endif
	}
}


CONSTEXPR	void	add_then_scale_v2r( REAL* dst, REAL CONST * a, REAL CONST * b, REAL CONST factor )
{
	//if( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
	//{
	//	return	add_then_scale_v3_xna( dst, a, b, factor );
	//}
	//else
	{
		*dst		= (	*a		+ *b		)	*	factor	;
#if AAA_V_DOIT_WITH_INC()
		*++dst		= ( *++a	+ *++b		)	*	factor	;
#else
		*(dst+1)	= (	*(a+1)	+ *(b+1)	)	*	factor	;
#endif
	}
}

CONSTEXPR	void	add_then_scale_v3r( REAL* dst, REAL CONST * a, REAL CONST * b, REAL CONST factor )
{
	//if( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
	//{
	//	return	add_then_scale_v3_xna( dst, a, b, factor );
	//}
	//else
	{
		*dst		= (	*a		+ *b		)	*	factor	;
#if AAA_V_DOIT_WITH_INC()
		*++dst		= ( *++a	+ *++b		)	*	factor	;
		*++dst		= ( *++a	+ *++b		)	*	factor	;
#else
		*(dst+1)	= (	*(a+1)	+ *(b+1)	)	*	factor	;
		*(dst+2)	= (	*(a+2)	+ *(b+2)	)	*	factor	;
#endif
	}
}

CONSTEXPR	void	add_mul_v3r( REAL* dst, REAL CONST * a, REAL CONST * b, REAL CONST * c )
{
	//if( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
	//{
	//	return	add_mul_v3_xna( dst, a, b, c );
	//}
	//else
	{
		*dst		= *a		+ *b		* *c	;
#if AAA_V_DOIT_WITH_INC()
		*++dst		= *++a		+ *++b		* *++c	;
		*++dst		= *++a		+ *++b		* *++c	;
#else
		*(dst+1)	= *(a+1)	+ *(b+1)	* *(c+1)	;
		*(dst+2)	= *(a+2)	+ *(b+2)	* *(c+2)	;
#endif
	}
}

CONSTEXPR	void	add_mul_v3r( REAL* dst, REAL CONST * b, REAL CONST * c )
{
	//if( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
	//{
	//	return	add_mul_v3_xna( dst, b, c );
	//}
	//else
	{
		*dst		+= *b		* *c;
#if AAA_V_DOIT_WITH_INC()
		*++dst		+= *++b		* *++c;
		*++dst		+= *++b		* *++c;
#else
		*(dst+1)	+= *(b+1)	* *(c+1);
		*(dst+2)	+= *(b+2)	* *(c+2);
#endif
	}
}

CONSTEXPR	void	add_mul_v2r( REAL* dst, REAL CONST * a, REAL CONST * b, REAL CONST * c )
{
	*dst		= *a		+ *b		* *c	;
#if AAA_V_DOIT_WITH_INC()
	*++dst		= *++a		+ *++b		* *++c	;
#else
	*(dst+1)	= *(a+1)	+ *(b+1)	* *(c+1)	;
#endif
}

CONSTEXPR	void	add_mul_v2r( REAL* dst, REAL CONST * b, REAL CONST * c )
{
	*dst		+= *b		* *c;
#if AAA_V_DOIT_WITH_INC()
	*++dst		+= *++b		* *++c;
#else
	*(dst+1)	+= *(b+1)	* *(c+1);
#endif
}

CONSTEXPR void	offset_v3r( REAL* dst, REAL CONST * src, REAL CONST * a, INT32 nb )
{
	for( ; nb > 0; --nb )
	{
		//todo check if compilateur make it good
		add_v3( dst, src, a );
		dst += 3;
		src += 3;
	}
}

CONSTEXPR void	offset_v3r( REAL* dst, REAL CONST * a, INT32 nb )
{
	for( ; nb > 0; --nb )
	{
		//todo check if compilateur make it good
		add_v3( dst, a );
		dst += 3;
	}
}

//SUB

//	dst = a-b
CONSTEXPR	void	sub_array_v3r( REAL* dst, REAL CONST * a, REAL CONST * b, INT32 nb )
{
	for( ; nb > 0; --nb )
	{
#if AAA_V_DOIT_WITH_INC()
		*dst	= *a   - *b;
		*++dst	= *++a - *++b;
		*++dst	= *++a - *++b;
		++dst;
		++a;
		++b;
#else
		sub_v3( dst, a, b );
		dst += 3;
		a += 3;
		b += 3;
#endif
	}
}

//	dst -= a
CONSTEXPR	void	sub_array_v3r( REAL* dst, REAL CONST * a, INT32 nb )
{
	for( ; nb > 0; --nb )
	{
#if AAA_V_DOIT_WITH_INC()
		*dst	-= *a	;
		*++dst	-= *++a	;
		*++dst	-= *++a	;
		++dst;
		++a;
#else
		sub_v3( dst, a );
		dst += 3;
		a += 3;
#endif
	}
}

//	dst = (a-b)*factor
CONSTEXPR	void	sub_then_scale_v3r( REAL* dst, REAL CONST * a, REAL CONST * b, REAL CONST factor )
{
	//if( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
	//{
	//	return	sub_then_scale_v3_xna( dst, a, b, factor );
	//}
	//else
	{
		dst[0] = (a[0] - b[0]) * factor;
		dst[1] = (a[1] - b[1]) * factor;
		dst[2] = (a[2] - b[2]) * factor;
	}
}

//	a = (a-b)*f;
CONSTEXPR	void	sub_then_scale_v3r( REAL* a, REAL CONST * b, REAL CONST f )
{
	//if( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
	//{
	//	return	sub_then_scale_v3_xna( a, b, f );
	//}
	//else
	{
		a[0] = (a[0] - b[0]) * f;
		a[1] = (a[1] - b[1]) * f;
		a[2] = (a[2] - b[2]) * f;
	}
}

//	dst += (a-b)
CONSTEXPR	void	sub_then_add_to_v3r( REAL* dst, REAL CONST * a, REAL CONST * b )
{
	//if( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
	//{
	//	return	sub_then_add_to_v3_xna( dst, a, b );
	//}
	//else
	{
		dst[0] += a[0] - b[0];
		dst[1] += a[1] - b[1];
		dst[2] += a[2] - b[2];
	}
}

//	dst += (a-b)
CONSTEXPR	void	sub_then_add_to_array_v3r( REAL* dst, REAL CONST * a, REAL CONST * b, INT32 nb )
{
	for( ; nb > 0; --nb )
	{
		sub_then_add_to_v3r( dst, a, b );
		dst += 3;
		a += 3;
		b += 3;
	}
}

//	symetric of b relative to a : dst = a + (a-b) = 2a - b
CONSTEXPR	void	sym_v3r( REAL* dst, REAL CONST * a, REAL CONST * b )
{
	//if( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
	//{
	//	return	sym_v3_xna( dst, a, b );
	//}
	//else
	{
		dst[0] = REAL( 2. * a[0] - b[0] );
		dst[1] = REAL( 2. * a[1] - b[1] );
		dst[2] = REAL( 2. * a[2] - b[2] );
	}
}

//MUL

CONSTEXPR void	mul_const_v3r( REAL* dst, REAL CONST * src, REAL CONST * CONST m, INT32 nb )
{
	for( ; nb > 0; --nb )
	{
		//todo check if compilateur make it good
		mul_v3( dst, src, m );
		dst += 3;
		src += 3;
	}
}
CONSTEXPR void	mul_const_v3r( REAL* dst, REAL CONST * CONST m, INT32 nb )
{
	for( ; nb > 0; --nb )
	{
		//todo check if compilateur make it good
		mul_v3( dst, m );
		dst += 3;
	}
}

//	ADD	THEN MUL
CONSTEXPR	void	add_then_mul_v2r( REAL* dst, REAL CONST * src, REAL CONST * a, REAL CONST * m )
{
	//if( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
	//{
	//	return	???_xna( dst, b, c );
	//}
	//else
	{
		dst[0] = (src[0] + a[0]) * m[0];
		dst[1] = (src[1] + a[1]) * m[1];
	}
}
CONSTEXPR	void	add_then_mul_v2r( REAL* dst, REAL CONST * a, REAL CONST * m )
{
	//if( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
	//{
	//	return	???_xna( dst, b, c );
	//}
	//else
	{
		dst[0] = (dst[0] + a[0]) * m[0];
		dst[1] = (dst[1] + a[1]) * m[1];
	}
}
CONSTEXPR	void	add_then_mul_v3r( REAL* dst, REAL CONST * src, REAL CONST * a, REAL CONST * m )
{
	//if( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
	//{
	//	return	???_xna( dst, b, c );
	//}
	//else
	{
		dst[0] = (src[0] + a[0]) * m[0];
		dst[1] = (src[1] + a[1]) * m[1];
		dst[2] = (src[2] + a[2]) * m[2];
	}
}
CONSTEXPR	void	add_then_mul_v3r( REAL* dst, REAL CONST * a, REAL CONST * m )
{
	//if( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
	//{
	//	return	???_xna( dst, b, c );
	//}
	//else
	{
		dst[0] = (dst[0] + a[0]) * m[0];
		dst[1] = (dst[1] + a[1]) * m[1];
		dst[2] = (dst[2] + a[2]) * m[2];
	}
}

CONSTEXPR	void	add_then_mul_v4r( REAL* dst, REAL CONST * src, REAL CONST * a, REAL CONST * m )
{
	//if( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
	//{
	//	return	???_xna( dst, b, c );
	//}
	//else
	{
		dst[0] = (src[0] + a[0]) * m[0];
		dst[1] = (src[1] + a[1]) * m[1];
		dst[2] = (src[2] + a[2]) * m[2];
		dst[3] = (src[3] + a[3]) * m[3];
	}
}
CONSTEXPR	void	add_then_mul_v4r( REAL* dst, REAL CONST * a, REAL CONST * m )
{
	//if( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
	//{
	//	return	???_xna( dst, b, c );
	//}
	//else
	{
		dst[0] = (dst[0] + a[0]) * m[0];
		dst[1] = (dst[1] + a[1]) * m[1];
		dst[2] = (dst[2] + a[2]) * m[2];
		dst[3] = (dst[3] + a[3]) * m[3];
	}
}

//	SUB	THEN MUL

CONSTEXPR void	add_const_then_mul_const_v3r( REAL* dst, REAL CONST * src, REAL CONST * a, REAL CONST * m, INT32 nb )
{
	for( ; nb > 0; --nb )
	{
		//todo check if compilateur make it good
		add_then_mul_v3r( dst, src, a, m );
		dst += 3;
		src += 3;
	}
}
CONSTEXPR void	add_const_then_mul_const_v3r( REAL* dst, REAL CONST * a, REAL CONST * m, INT32 nb )
{
	for( ; nb > 0; --nb )
	{
		//todo check if compilateur make it good
		add_then_mul_v3r( dst, a, m );
		dst += 3;
	}
}

/*
#if	!AAA_REAL_IS_DOUBLE()
FINLINE	void	cpy_v3r( REAL* dst, DOUBLE CONST * src )
{
	*dst		=	(REAL)	*src;
#if AAA_V_DOIT_WITH_INC()
	*++dst		=	(REAL)	*++src;
	*++dst		=	(REAL)	*++src;
#else
	*(dst+1)	=	(REAL)	*(src+1);
	*(dst+2)	=	(REAL)	*(src+2);
#endif
}
#endif


FINLINE	void	cpy_v2r( REAL* dst, REAL CONST * src )
{
	*dst = *src;
	*++dst = *++src;
	*dst = *src;
	*(dst+1) = *(src+1);
}
*/

CONSTEXPR	void	cpy_from_with_stride_v3r( REAL* dst, REAL CONST * src, INT32 stride, INT32 nb )
{
	stride -= 2;
	for( ; nb > 0 ; --nb )
	{
		*dst	= *src;
		*++dst	= *++src;
		*++dst	= *++src;
		++dst;
		src += stride;
	}
}

CONSTEXPR	void	cpy_to_with_stride_v3r( REAL* dst, INT32 stride, REAL CONST * src,  INT32 nb )
{
	stride -= 2;
	for( ; nb  >0 ; --nb )
	{
		*dst	= *src;
		*++dst	= *++src;
		*++dst	= *++src;
		dst	+= stride;
		++src;
	}
}


//ABS
CONSTEXPR	void	abs_v3r( REAL* dst, REAL CONST * src )
{
#if	AAA_USE_XNA()
	if( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
		return	abs_v3_xna( dst, src );
#endif
	{
		dst[0] = ABS(src[0]);
		dst[1] = ABS(src[1]);
		dst[2] = ABS(src[2]);
	}
}

CONSTEXPR	void	abs_v2r( REAL* dst, REAL CONST * src )
{
	dst[0] = ABS(src[0]);
	dst[1] = ABS(src[1]);
}

CONSTEXPR	void	abs_v3r( REAL* dst )
{
#if	AAA_USE_XNA()
	if( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
		return	abs_v3_xna(dst);
#endif
	{
		dst[0] = ABS(dst[0]);
		dst[1] = ABS(dst[1]);
		dst[2] = ABS(dst[2]);
	}
}

CONSTEXPR	void	abs_v2r( REAL* dst )
{
	dst[0] = ABS(dst[0]);
	dst[1] = ABS(dst[1]);
}

//NORMALIZE
static	FINLINE	void	normalize_v3r_low( REAL *dst, REAL CONST s0, REAL CONST s1, REAL CONST s2, REAL CONST size_squared )
{
	if( size_squared != 0.f )
	{
		REAL f = OVER_ONE_AS_REAL( SQRT(size_squared) );
		set_v3( dst, s0 * f, s1 * f, s2 * f );
	}
	else
		clear_v3( dst );
}

static	FINLINE	void	normalize_scale_v3r_low( REAL* dst, REAL CONST s0, REAL CONST s1, REAL CONST s2, REAL CONST size_squared, REAL scale_factor )
{
	if( size_squared != 0.f )
	{
		scale_factor *= OVER_ONE_AS_REAL( SQRT(size_squared) );
		set_v3( dst, s0 * scale_factor, s1 * scale_factor, s2 * scale_factor );
	}
	else
		clear_v3( dst );
}

FINLINE	void	normalize_v3r( REAL *dst, REAL CONST s0, REAL CONST s1, REAL CONST s2 )
{
	normalize_v3r_low( dst, s0, s1, s2, s0*s0 + s1*s1 + s2*s2 );
}

FINLINE	void	normalize_v3r( REAL *dst )
{
#if	AAA_USE_XNA()
	if( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
		return	normalize_v3_xna( dst );
#endif
	normalize_v3r( dst, *dst, *(dst+1), *(dst+2) );
}

FINLINE	void	normalize_v3r( FP32* dst, REAL CONST * src )
{
#if	AAA_USE_XNA()
	if( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
		return	normalize_v3_xna( dst, src );
#endif
	normalize_v3r( dst, *src, *(src+1), *(src+2) );
}

//todo quickly done deal at some point with AAA_REAL_IS_DOUBLE()
FINLINE	void	normalize_v3fp32( FP32* dst, FP32 CONST * src )
{
#if	AAA_USE_XNA()
	if( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
		return	normalize_v3_xna( dst, src );
#endif
	normalize_v3r( dst, *src, *(src+1), *(src+2) );
}

static	FINLINE	void	normalize_v3d_low( DOUBLE *dst, DOUBLE CONST s0, DOUBLE CONST s1, DOUBLE CONST s2, DOUBLE CONST size_squared )
{
	if( size_squared != 0.f )
	{
		DOUBLE f = OVER_ONE( SQRT(size_squared) );
		set_v3( dst, s0 * f, s1 * f, s2 * f );
	}
	else
		clear_v3( dst );
}

static	FINLINE	void	normalize_scale_v3d_low( DOUBLE* dst, DOUBLE CONST s0, DOUBLE CONST s1, DOUBLE CONST s2, DOUBLE CONST size_squared, DOUBLE scale_factor )
{
	if( size_squared != 0.f )
	{
		scale_factor *= OVER_ONE( SQRT(size_squared) );
		set_v3( dst, s0 * scale_factor, s1 * scale_factor, s2 * scale_factor );
	}
	else
		clear_v3( dst );
}

FINLINE	void	normalize_v3d( DOUBLE *dst, DOUBLE CONST s0, DOUBLE CONST s1, DOUBLE CONST s2 )
{
	normalize_v3d_low( dst, s0, s1, s2, s0*s0 + s1*s1 + s2*s2 );
}

FINLINE	void	normalize_v3d( DOUBLE *dst )
{
#if 0	//	AAA_USE_XNA()
	if( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
		return	normalize_v3_xna( dst );
#endif
	normalize_v3d( dst, *dst, *(dst+1), *(dst+2) );
}

FINLINE	void	normalize_v3d( DOUBLE* dst, DOUBLE CONST * src )
{
#if	0	//	AAA_USE_XNA()
	if( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
		return	normalize_v3_xna( dst, src );
#endif
	normalize_v3d( dst, *src, *(src+1), *(src+2) );

}

FINLINE	void	normalize_scale_v3r( REAL* dst, REAL CONST scale_factor )
{
#if	AAA_USE_XNA()
	if( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
		return	normalize_scale_v3_xna( dst, scale_factor );
#endif
	{
		REAL CONST	s0 = *dst;
		REAL CONST	s1 = *(dst+1);
		REAL CONST	s2 = *(dst+2);
		normalize_scale_v3r_low( dst, s0, s1, s2, s0*s0 + s1*s1 + s2*s2, scale_factor );
	}
}

FINLINE	void	normalize_v2( REAL &a, REAL &b )
{
	REAL size = a*a + b*b;

	if( size != 0 )
	{
		size = OVER_ONE_AS_REAL( SQRT( size ) );
		a *= size;
		b *= size;
	}
	else
	{
		a = 0.;
		b = 0.;
	}
}

//NORMALIZE
FINLINE	void	normalize_v2r( REAL* dst )
{
	REAL CONST	d0 = *dst;
	REAL CONST	d1 = *(dst+1);

	REAL	size = d0*d0 + d1*d1;
	if( size != 0 )
	{
		size = OVER_ONE_AS_REAL( SQRT( size ) );
		set_v2( dst, d0*size, d1*size );
	}
	else
		clear_v2( dst );
}

FINLINE	void	normalize_scale_v2r( REAL* dst, REAL CONST scale_factor )
{
	REAL CONST	d0 = *dst;
	REAL CONST	d1 = *(dst+1);

	REAL	size = d0*d0 + d1*d1;
	if( size != 0 )
	{
		size = scale_factor * OVER_ONE_AS_REAL( SQRT( size ) );
		set_v2( dst, d0*size, d1*size );
	}
	else
		clear_v2( dst );
}

FINLINE	void	limit_v3r( REAL* dst, REAL CONST max )
{
#if	AAA_USE_XNA()
	if( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
		return	limit_v3_xna( dst, max );
#endif
	{
		REAL CONST	d0 = *dst;
		REAL CONST	d1 = *(dst+1);
		REAL CONST	d2 = *(dst+2);

		REAL	size = d0*d0 + d1*d1 + d2*d2;
		if( size > max*max )
		{
			size = max * OVER_ONE_AS_REAL( SQRT( size ) );
			set_v3( dst, d0*size, d1*size, d2*size );
		}
	}
}

//DOT
CONSTEXPR	DOUBLE	dot_v2r( REAL CONST * a, REAL CONST * b )
{
	//if( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
	//{
	//	return	dot_v2_xna( a, b );
	//}
	//else
		return a[0]*b[0] + a[1]*b[1];
}


/*
FINLINE	REAL	cos_v3r( REAL *dst, REAL CONST *a, REAL CONST *b)
{
REAL	tmp;
	tmp =	*a		*	*b	;
	tmp +=	*++a	*	*++b	;
	tmp +=	*++a	*	*++b	;
	return tmp;
}
*/

//CROSS
CONSTEXPR	void	cross_v3r( REAL& x, REAL& y, REAL& z, REAL CONST * a, REAL CONST * b )
{
	//if( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
	//{
	//	return	cross_v3_xna( dst, a, b );
	//}
	//else
	{
		REAL a0 = a[0];
		REAL a1 = a[1];
		REAL a2 = a[2];
		REAL b0 = b[0];
		REAL b1 = b[1];
		REAL b2 = b[2];
		x = a1*b2 - a2*b1;
		y = a2*b0 - a0*b2;
		z = a0*b1 - a1*b0;		
//	can't do it this way it exclude the dst being one of the source
//		x = a[1] * b[2] - a[2] * b[1];
//		y = a[2] * b[0] - a[0] * b[2];
//		z = a[0] * b[1] - a[1] * b[0];
	}
}

CONSTEXPR	void	cross_v3r( REAL* dst, REAL CONST * a, REAL CONST * b )
{
#if	AAA_USE_XNA()
	if( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
		return	cross_v3_xna( dst, a, b );
#endif
	{
		REAL a0 = a[0];
		REAL a1 = a[1];
		REAL a2 = a[2];
		REAL b0 = b[0];
		REAL b1 = b[1];
		REAL b2 = b[2];
		dst[0] = a1*b2 - a2*b1;
		dst[1] = a2*b0 - a0*b2;
		dst[2] = a0*b1 - a1*b0;
//	can't do it this way it exclude the dst being one of the source
//		*dst		= a[1] * b[2] - a[2] * b[1];
//		*(dst+1)	= a[2] * b[0] - a[0] * b[2];
//		*(dst+2)	= a[0] * b[1] - a[1] * b[0];
	}
}

CONSTEXPR	void	cross_x_v3r( REAL& y, REAL& z, REAL CONST * a )
{
	REAL a1 = *(a+1);	// avoid problem when dst is src
	y = *(a+2);
	z = - a1;
}
CONSTEXPR	void	cross_x_v3r( REAL& x, REAL& y, REAL& z, REAL CONST * a )
{
	REAL a1 = *(a+1);	// avoid problem when dst is src
	x = 0;
	y = *(a+2);
	z = - a1;
}
CONSTEXPR	void	cross_x_v3r( REAL* dst, REAL CONST * a )
{
	REAL a1 = a[1];	// avoid problem when dst is src
	dst[0] = 0;
	dst[1] = a[2];
	dst[2] = -a1;

}

//like cross_v3r( dst, src, unit_y )
CONSTEXPR	void	cross_y_v3r( REAL& x, REAL& z, REAL CONST * a )
{
	REAL a0 =a[0];	// avoid problem when dst is src
	x = - a[2];
	z = a0;
}
CONSTEXPR	void	cross_y_v3r( REAL& x, REAL& y, REAL& z, REAL CONST * a )
{
	REAL a0 = a[0];	// avoid problem when dst is src
	x = - a[2];
	y = 0;
	z = a0;
}
CONSTEXPR	void	cross_y_v3r( REAL* dst, REAL CONST * a )
{
	REAL a0 = a[0];	// avoid problem when dst is src
	dst[0] = -a[2];
	dst[1] = 0;
	dst[2] = a0;
}

//like cross_v3r( dst, src, unit_y )
CONSTEXPR	void	cross_z_v3r( REAL& x, REAL& y, REAL& z, REAL CONST * a )
{
	REAL a0 = a[0];	// avoid problem when dst is src
	x = a[1];
	y = -a0;
	z = 0;
}
CONSTEXPR	void	cross_z_v3r( REAL& x, REAL& y, REAL CONST * a )
{
	REAL a0 = a[0];	// avoid problem when dst is src
	x = a[1];
	y = -a0;
}
CONSTEXPR	void	cross_z_v3r( REAL* dst, REAL CONST * a )
{
	REAL a0 = a[0];	// avoid problem when dst is src
	dst[0] = a[1];
	dst[1] = -a0;
	dst[2] = 0;
}

//	calculates a normalized crossproduct to v1, v2
FINLINE	void	cross_normalize_v3r( REAL* dst, REAL CONST * a, REAL CONST * b )
{
	REAL	s0;
	REAL	s1;
	REAL	s2;
	cross_v3r( s0, s1, s2, a, b );
	normalize_v3r_low( dst, s0, s1, s2, s0*s0+s1*s1+s2*s2 );
}

FINLINE	void	cross_normalize_scale_v3r( REAL* dst, REAL CONST * a, REAL CONST * b, REAL CONST scale_factor )
{
#if	AAA_USE_XNA()
	if( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
		return	cross_normalize_scale_v3_xna( dst, a, b, scale_factor );
#endif
	{
		REAL	s0;
		REAL	s1;
		REAL	s2;
		cross_v3r( s0, s1, s2, a, b );
		normalize_scale_v3r_low( dst, s0, s1, s2, s0*s0+s1*s1+s2*s2, scale_factor );
	}
}

//	calculates a normalized crossproduct to v1, v2
FINLINE	void	cross_normalize_x_v3r( REAL* dst, REAL CONST * a )
{
	REAL	y, z;
	cross_x_v3r( y, z, a );
	normalize_v2( y, z );
	*dst		= 0;
	*(dst+1)	= y;
	*(dst+2)	= z;
}
FINLINE	void	cross_normalize_y_v3r( REAL* dst, REAL CONST * a )
{
	REAL	x, z;
	cross_y_v3r( x, z, a );
	normalize_v2( x, z );
	*dst		= x;
	*(dst+1)	= 0;
	*(dst+2)	= z;
}
FINLINE	void	cross_normalize_z_v3r( REAL* dst, REAL CONST * a )
{
	REAL	x, y;
	cross_z_v3r( x, y, a );
	normalize_v2( x, y );
	*dst		= x;
	*(dst+1)	= y;
	*(dst+2)	= 0;
}

//
//	MIN MAX
//
CONSTEXPR	void	min_max_v3r( REAL* min, REAL* max, REAL CONST *p )
{
	UPDATE_MIN_MAX_SAFE(	*min,		*max,		*p		);
	UPDATE_MIN_MAX_SAFE(	*++min,		*++max,		*++p	);	
	UPDATE_MIN_MAX_SAFE(	*++min,		*++max,		*++p	);	
}

			void	min_max_v3r( REAL* min, REAL* max, REAL CONST * point, INT32 CONST nb )
{
	cpy_v3( min, point );
	cpy_v3( max, point );
	for( INT32 i = nb - 1; i > 0; --i )
	{
		point += 3;
		min_max_v3r( min, max, point );
	}
}

//NORMAL
CONSTEXPR	void	uv_to_xyz_v3r( REAL* dst, REAL CONST * src, INT32 CONST axe )
{
	switch ( axe )
	{	
	case 0:	cpy_shift1_v3r( dst, src );	break;
	case 1:	cpy_shift2_v3r( dst, src );	break;
	case 2:	cpy_v3( dst, src );		break;
	}
}

//AXE
CONSTEXPR	void	build_point_v3r( REAL* dst, REAL CONST * o, REAL CONST * a, REAL CONST * b, REAL CONST u, REAL CONST v )
{
#if	AAA_USE_XNA()
	if( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
		return	build_point_v3_xna( dst, o, a, b, u, v );
#endif
	{
		dst[0] = o[0] + a[0] * u + b[0] * v;
		dst[1] = o[1] + a[1] * u + b[1] * v;
		dst[2] = o[2] + a[2] * u + b[2] * v;
	}
}

CONSTEXPR	void	build_vector_v3r( REAL* dst, REAL CONST * a, REAL CONST * b, REAL CONST * c, REAL CONST u, REAL CONST v, REAL CONST w )
{
#if	AAA_USE_XNA()
	if( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
		return	build_point_v3_xna( dst, a, b, c, u, v, w );
#endif
	{
		dst[0] = a[0] * u + b[0] * v + c[0] * w;
		dst[1] = a[1] * u + b[1] * v + c[1] * w;
		dst[2] = a[2] * u + b[2] * v + c[2] * w;
	}
}
CONSTEXPR	void	build_point_v3r( REAL* dst, REAL CONST * o, REAL CONST * a, REAL CONST * b, REAL CONST * c, REAL CONST u, REAL CONST v, REAL CONST w )
{
#if	AAA_USE_XNA()
	if( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
		return	build_point_v3_xna( dst, o, a, b, c, u, v, w );
#endif
	{
		dst[0] = o[0] + a[0] * u + b[0] * v + c[0] * w;
		dst[1] = o[1] + a[1] * u + b[1] * v + c[1] * w;
		dst[2] = o[2] + a[2] * u + b[2] * v + c[2] * w;
	}
}

//
//	BARYCENTER
//
CONSTEXPR	void	center_v2r(	REAL* dst, REAL CONST * a, REAL CONST * b )
{
#if	0	//todo AAA_USE_XNA()
	if( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
	{
	}
	else
#endif
	{
		dst[0] = ( a[0] + b[0] ) * REAL(.5);
		dst[1] = ( a[1] + b[1] ) * REAL(.5);
	}
}

CONSTEXPR	void	center_v3r(	REAL* dst, REAL CONST * a, REAL CONST * b )
{
#if	0	//todo AAA_USE_XNA()
	if( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
	{
	}
	else
#endif
	{
		dst[0] = ( a[0] + b[0] ) * REAL(.5);
		dst[1] = ( a[1] + b[1] ) * REAL(.5);
		dst[2] = ( a[2] + b[2] ) * REAL(.5);
	}
}

CONSTEXPR	void	center_v2r(	REAL* dst, REAL CONST * a, REAL CONST * b, REAL CONST * c )
{
#if	0	//todo AAA_USE_XNA()
	if( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
	{
	}
	else
#endif
	{
		dst[0] = ( a[0] + b[0] + c[0] ) * REAL(1./3.);
		dst[1] = ( a[1] + b[1] + c[1] ) * REAL(1./3.);
	}
}

CONSTEXPR	void	center_v3r(	REAL* dst, REAL CONST * a, REAL CONST * b, REAL CONST * c )
{
#if	0	//todo AAA_USE_XNA()
	if( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
	{
	}
	else
#endif
	{
		dst[0] = ( a[0] + b[0] + c[0] ) * REAL(1./3.);
		dst[1] = ( a[1] + b[1] + c[1] ) * REAL(1./3.);
		dst[2] = ( a[2] + b[2] + c[2] ) * REAL(1./3.);
	}
}

FINLINE		void	compute_barycenter_v3r( REAL* dst, REAL CONST * p, INT32 CONST nb )
{		
	cpy_v3( dst, p );
	if( nb > 1 )
	{
		for( INT32 i = nb-1; i>0; --i )
		{
			p += 3;
			add_v3( dst, p );
		}
		scale_v3( dst, 1./REAL(nb) );
	}
}
//
//	CATMULL
//
#define	CATMULL_BUILD_COEF( t )				\
	REAL t2 = t * t;						\
	REAL t3 = t * t2;						\
	REAL f1 =   - t + 2 * t2	-     t3;	\
	REAL f2 = 2     - 5 * t2	+ 3 * t3;	\
	REAL f3 =     t + 4 * t2	- 3 * t3;	\
	REAL f4 =       -     t2	+     t3;

#define	CATMULL_DERIVATIVE_BUILD_COEF( t )	\
	REAL t2 = t * t;						\
	REAL f1 = - 1 + 4  * t - 3 * t2;		\
	REAL f2 =     - 10 * t + 9 * t2;		\
	REAL f3 =   1 + 8  * t - 9 * t2;		\
	REAL f4 =     - 2  * t + 3 * t2;

CONSTEXPR	void	catmull_rom_3( REAL* dst, REAL CONST * a, REAL CONST * b, REAL CONST * c, REAL CONST * d, FP32 CONST t )
{
#if	AAA_USE_XNA()
	if( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
		return	cvCatmullRom_3_xna( dst, a, b, c, d, t );
#endif
	{
		CATMULL_BUILD_COEF(t)
		dst[0] = ( f1*a[0] + f2*b[0] + f3*c[0] + f4*d[0] ) * REAL(.5);
		dst[1] = ( f1*a[1] + f2*b[1] + f3*c[1] + f4*d[1] ) * REAL(.5);
		dst[2] = ( f1*a[2] + f2*b[2] + f3*c[2] + f4*d[2] ) * REAL(.5);
	}					 
}
CONSTEXPR	void	catmull_rom_derivative_3( REAL* dst, REAL CONST * a, REAL CONST * b, REAL CONST * c, REAL CONST * d, FP32 CONST t )
{
	//todo catmullrom_derivative_xna
	//if( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
	//{
	//	return	cvCatmullRom_3_xna( dst, a, b, c, d, t );
	//}
	{
		CATMULL_DERIVATIVE_BUILD_COEF(t)
		dst[0] = ( f1*a[0] + f2*b[0] + f3*c[0] + f4*d[0] ) * REAL(.5);
		dst[1] = ( f1*a[1] + f2*b[1] + f3*c[1] + f4*d[1] ) * REAL(.5);
		dst[2] = ( f1*a[2] + f2*b[2] + f3*c[2] + f4*d[2] ) * REAL(.5);
	}
}

CONSTEXPR	void	catmull_rom_2( REAL* dst, REAL CONST * a, REAL CONST * b, REAL CONST * c, REAL CONST * d, FP32 CONST t )
{
#if	AAA_USE_XNA()
	if( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
		return	cvCatmullRom_2_xna( dst, a, b, c, d, t );
#endif
	{
		CATMULL_BUILD_COEF(t)
		dst[0] = ( f1*a[0] + f2*b[0] + f3*c[0] + f4*d[0] ) * REAL(.5);
		dst[1] = ( f1*a[1] + f2*b[1] + f3*c[1] + f4*d[1] ) * REAL(.5);
	}					 
}
CONSTEXPR	void	catmull_rom_derivative_2( REAL* dst, REAL CONST * a, REAL CONST * b, REAL CONST * c, REAL CONST * d, FP32 CONST t )
{
	//todo catmullrom_derivative_xna
	//if( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
	//{
	//	return	cvCatmullRom_2r_xna( dst, a, b, c, d, t );
	//}
	//else
	{
		CATMULL_DERIVATIVE_BUILD_COEF(t)
		dst[0] = ( f1*a[0] + f2*b[0] + f3*c[0] + f4*d[0] ) * REAL(.5);
		dst[1] = ( f1*a[1] + f2*b[1] + f3*c[1] + f4*d[1] ) * REAL(.5);
	}
}

// Catmull-Rom Curve calculations
CONSTEXPR	void	catmull_rom_v3( REAL* dst, REAL CONST * p, FP32 CONST t )
{
#if	AAA_USE_XNA()
	if( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
		return	cvCatmullRom_3_xna( dst, p, p+3, p+6, p+9, t );
#endif
		return catmull_rom_3( dst, p, p+3, p+6, p+9, t );
}
CONSTEXPR	void	catmull_rom_derivative_v3( REAL* dst, REAL CONST * p, FP32 CONST t )
{
	return catmull_rom_derivative_3( dst, p, p+3, p+6, p+9, t );
}

CONSTEXPR	void	catmull_rom_v2( REAL* dst, REAL CONST * p, FP32 CONST t )
{
	return catmull_rom_2( dst, p, p+2, p+4, p+6, t );
}
CONSTEXPR	void	catmull_rom_derivative_v2( REAL* dst, REAL CONST * p, FP32 CONST t )
{
	return catmull_rom_derivative_2( dst, p, p+2, p+4, p+6, t );
}

//todo precompute coef and interpolate
CONSTEXPR	REAL	catmull_rom_1r( REAL CONST a, REAL CONST b, REAL CONST c, REAL CONST d, FP32 CONST t )
{
	CATMULL_BUILD_COEF(t)

	return	( f1*a + f2*b + f3*c + f4*d ) * REAL(.5);
}

//
//	BEZIER
//
#define BEZIER_BUILD_COEF(t)	\
	REAL t1 = REAL(1) - t;		\
	REAL t2 = t * t;			\
	REAL t12 = t1 * t1;			\
	REAL f1 = t1 * t12;			\
	REAL f2 = 3 * t * t12;		\
	REAL f3 = 3 * t2 * t1;		\
	REAL f4 = t2 * t;

//CONSTEXPR	void	bezier_derivative_build_coef( REAL& f1, REAL& f2, REAL& f3, REAL& f4, REAL CONST t )	{}

CONSTEXPR	void	bezier_3r( REAL* dst, REAL CONST * a, REAL CONST * b, REAL CONST * c, REAL CONST * d, FP32 CONST t )
{
//#if	AAA_USE_XNA()
//	if( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
//		return	cvCatmullRom_3r_xna( dst, a, b, c, d, t );
//#endif
	BEZIER_BUILD_COEF(t)
	dst[0] = f1*a[0] + f2*b[0] + f3*c[0] + f4*d[0];
	dst[1] = f1*a[1] + f2*b[1] + f3*c[1] + f4*d[1];
	dst[2] = f1*a[2] + f2*b[2] + f3*c[2] + f4*d[2];
}

//FINLINE	void	bezier_derivative_3r( REAL* dst, REAL CONST * a, REAL CONST * b, REAL CONST * c, REAL CONST * d, REAL CONST t )	{}
//FINLINE	void	bezier_derivative_v3r( REAL* dst, REAL CONST * p, REAL CONST t )	{}

CONSTEXPR	void	bezier_2r( REAL* dst, REAL CONST * a, REAL CONST * b, REAL CONST * c, REAL CONST * d, REAL CONST t )
{
	BEZIER_BUILD_COEF(t)
	dst[0] = f1*a[0] + f2*b[0] + f3*c[0] + f4*d[0];
	dst[1] = f1*a[1] + f2*b[1] + f3*c[1] + f4*d[1];
}
CONSTEXPR	void	bezier_v3r( REAL* dst, REAL CONST * p, FP32 CONST t )
{
	bezier_3r( dst, p, p+3, p+6, p+9, t );
}
CONSTEXPR	void	bezier_v2r( REAL* dst, REAL CONST * p, FP32 CONST t )
{
	bezier_2r( dst, p, p+2, p+4, p+6, t );
}
//FINLINE	void	catmull_rom_derivative_v2r( REAL* dst, REAL CONST * p, REAL CONST t ) {}

//todo precompute coef and interpolate
CONSTEXPR	REAL	bezier_1r( REAL CONST a, REAL CONST b, REAL CONST c, REAL CONST d, REAL CONST t )
{
	BEZIER_BUILD_COEF(t)

	return	f1*a + f2*b + f3*c + f4*d;
}

CONSTEXPR	void	normal_of_4_point_v3r( REAL* nor, REAL CONST * a, REAL CONST * b, REAL CONST * c, REAL CONST * d )
{
#if	AAA_USE_XNA()
	if( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
		return	normal_of_4_point_v3_xna( nor, a, b, c, d );
#endif
	{
		REAL	vec_u[3];
		sub_v3( nor, b, a );
		sub_v3( vec_u, d, c );
		cross_v3r( nor, vec_u, nor );
	}
}

CONSTEXPR	void	normal_cano_of_4_point_v3r( REAL* nor, REAL CONST * a, REAL CONST * b, REAL CONST * c, REAL CONST * d )
{
#if	AAA_USE_XNA()
	if( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
		return	normal_cano_of_4_point_v3_xna( nor, a, b, c, d );
#endif
	{
		REAL	vec_u[3];
		sub_v3( nor, b, a );
		sub_v3( vec_u, d, c );
		cross_v3r( nor, vec_u, nor );
		normalize_v3r( nor );
	}
}

//	for point p we compute the normal with the cross product of ba and dc
//		prev	....a....		this is really bad when the len of ap is very different of pb
//		cur		...cpd...			same problem for cp and pd
//		next	....b....			to sum up this is really bad when this is asymetric
//									we could do better but it will be more costly
CONSTEXPR void	compute_normals_v3r( REAL* dst, REAL CONST * prev, REAL CONST * cur, REAL CONST * next, INT32 nb, bool CONST b_close )
{
	REAL CONST * a = prev;
	REAL CONST * b = next;
	REAL CONST * c = cur;
	REAL CONST * d = cur + 3;
	if( b_close )
		c += ( nb - 1 ) * 3;
	REAL* nor = dst;

	normal_of_4_point_v3_xna( nor, a, b, c, d );
	if( !b_close )
		scale_v3( nor, REAL(2.) );
	a += 3;
	b += 3;
	c = cur;
	d += 3;
	nor += 3;

	nb -= 2;
	for( ; nb > 0; --nb )
	{
		normal_of_4_point_v3_xna( nor, a, b, c, d );
		a += 3;
		b += 3;
		c += 3;
		d += 3;
		nor += 3;
	}
	if( b_close )
		d = cur;
	else
		d -= 3;
	normal_of_4_point_v3_xna( nor, a, b, c, d );
	if( !b_close )
		scale_v3( nor, REAL(2.) );
}

CONSTEXPR void	compute_normals_cano_v3r( REAL* dst, REAL CONST * prev, REAL CONST * cur, REAL CONST * next, INT32 nb, bool CONST b_close )
{
	REAL CONST * a = prev;
	REAL CONST * b = next;
	REAL CONST * c = cur;
	REAL CONST * d = cur + 3;
	if( b_close )
		c += ( nb - 1 ) * 3;
	REAL* nor = dst;

	normal_cano_of_4_point_v3_xna( nor, a, b, c, d );
	if( !b_close )
		scale_v3( nor, REAL(2.) );
	a += 3;
	b += 3;
	c = cur;
	d += 3;
	nor += 3;

	nb -= 2;
	for( ; nb > 0; --nb )
	{
		normal_cano_of_4_point_v3_xna( nor, a, b, c, d );
		a += 3;
		b += 3;
		c += 3;
		d += 3;
		nor += 3;
	}
	if( b_close )
		d = cur;
	else
		d -= 3;
	normal_cano_of_4_point_v3_xna( nor, a, b, c, d );
	if( !b_close )
		scale_v3( nor, REAL(2.) );
}
//todo move to inline when ready
//hack	this is a pure hack
//		don't work for normal align with Y
FINLINE void	build_normal_vectors_v3r( REAL CONST * nor, REAL* CONST u, REAL* CONST v )
{
	if( (*nor)!=0 || (*(nor+2))!=0 )
	{	//nor is not parallel to y
#if	AAA_USE_XNA()
		if( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
			return	build_normal_vectors_v3_xna( nor, u, v );
#endif
		cross_normalize_y_v3r( v, nor );
		cross_normalize_v3r( u, v, nor );
	}
	else
	{
		cpy_v3( u, unit_z_v4fp32 );
		cpy_v3( v, unit_x_v4fp32 );
	}
}

FINLINE void	build_normal_vectors_using_z_v3r( REAL CONST * nor, REAL* CONST u, REAL* CONST v )
{
	if( (*nor)!=0 || (*(nor+1))!=0 )
	{	//nor is not parallel to z
#if	AAA_USE_XNA()
		if( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
			return	build_normal_vectors_using_z_v3_xna( nor, u, v );
#endif
		cross_normalize_z_v3r( v, nor );
		cross_normalize_v3r( u, v, nor );
	}
	else
	{
		cpy_v3( u, unit_x_v4fp32 );
		cpy_v3( v, unit_y_v4fp32 );
	}
}



