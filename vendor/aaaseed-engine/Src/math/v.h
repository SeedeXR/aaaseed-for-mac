
#ifdef AAA_V_H
#error "V_H included more than once."
#endif
#define AAA_V_H 1


#ifndef AAA_V_BASE_H
#	include "v_base.h"
#endif
#ifndef AAA_AAA_MATH_H
#	include "math/aaa_math.h"
#endif
#ifndef AAA_AAA_UTIL_H
#	include "aaa_util.h"
#endif

#ifndef AAA_USE_XNA
#	define	AAA_USE_XNA()	1
#endif
//	v_xna.h is now included unconditionally : on Windows (AAA_USE_XNA()==1)
//	it brings in the real XNA Math declarations as before ; on Mac / arm64
//	(AAA_USE_XNA()==0) it brings in inline forwarders to the `_v3r` scalar
//	variants for the four `_xna` symbols v.cpp references unconditionally.
//	Both branches gate inside v_xna.h itself.
#ifndef AAA_V_XNA_H
#	include "v_xna.h"
#endif

//	INTERPOLATION
//	dst == src possible
CONSTEXPR	void	interpolate_v1r_list(	REAL* dst,	REAL CONST * a,	REAL CONST * b,	REAL CONST inter,	INT32 nb )
{
	--dst;
	for( ; nb > 0; --nb )
	{
		*++dst = *a + inter * (*b-*a);
		++a;
		++b;  
	}
}
CONSTEXPR	void	interpolate_v3r_list(	REAL* dst,	REAL CONST * a,	REAL CONST * b,	REAL CONST inter,	INT32 nb )
{
	--dst;
	--b;
	for( ; nb > 0; --nb )
	{
		*++dst	=	*a + inter * (*++b-*a);
		++a;
		*++dst	=	*a + inter * (*++b-*a);
		++a; 
		*++dst	=	*a + inter * (*++b-*a);
		++a;
	}
}


// INTERPOLATE V2
//
//	dst == src possible
template< class T, class S >
CONSTEXPR	FINLINE	void	interpolate_v2( T* CONST dst,	T CONST * a, T CONST * CONST b,	S CONST inter ) NOEXCEPT
{
	T a0 = a[0];
	T a1 = a[1];
	dst[0] = T( a0 + inter * (b[0]-a0) );
	dst[1] = T( a1 + inter * (b[1]-a1) );
}
CONSTEXPR	FINLINE	void	interpolate_v2( FP32* dst, FP32 CONST * CONST a, FP32 CONST * CONST b, FP32 CONST inter ) NOEXCEPT
{
#if AAA_USE_XNA()
	if ( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
		interpolate_v2_xna( dst, a, b, inter );
	else
#endif
		interpolate_v2<FP32,FP32>( dst, a, b,  inter );
}
template< class T, class S >
CONSTEXPR	FINLINE	void	interpolate_v2( T* dst,	T CONST * CONST b,	S CONST inter ) NOEXCEPT
{
	dst[0] += T( inter * (b[0]-dst[0]) );
	dst[1] += T( inter * (b[1]-dst[1]) );
}
CONSTEXPR	FINLINE	void	interpolate_v2( FP32 * CONST dst, FP32 CONST * CONST b, FP32 CONST inter ) NOEXCEPT
{
#if	AAA_USE_XNA()
	if ( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
		interpolate_v2_xna( dst, b, inter );
	else
#endif
		interpolate_v2<FP32,FP32>( dst, b,  inter );
}

// INTERPOLATE V3
//
//	dst == src possible
template< class T, class S, class U >
CONSTEXPR	FINLINE	void	interpolate_v3( T* CONST dst,	S CONST * CONST a,	S CONST * CONST b,	U CONST inter ) NOEXCEPT
{
	U CONST one_minus_inter = U(1. - inter);
	dst[0] = T(	a[0] * one_minus_inter + b[0] * inter );
	dst[1] = T(	a[1] * one_minus_inter + b[1] * inter );
	dst[2] = T(	a[2] * one_minus_inter + b[2] * inter );
}
CONSTEXPR	FINLINE	void	interpolate_v3( FP32* CONST dst, FP32 CONST * CONST a, FP32 CONST * CONST b, FP32 CONST inter ) NOEXCEPT
{
#if AAA_USE_XNA()
	if ( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
		interpolate_v3_xna( dst, a,b, inter );
	else
#endif
		interpolate_v3<FP32,FP32>( dst, a,b, inter );
}
template< class T, class S >
CONSTEXPR	FINLINE	void	interpolate_v3( T* dst,	T CONST * CONST b,	S CONST inter ) NOEXCEPT
{
	dst[0] += T( inter * (b[0]-dst[0]) );
	dst[1] += T( inter * (b[1]-dst[1]) );
	dst[2] += T( inter * (b[2]-dst[2]) );
}
CONSTEXPR	FINLINE	void	interpolate_v3( FP32* CONST dst, FP32 CONST * CONST b, FP32 CONST inter ) NOEXCEPT
{
#if AAA_USE_XNA()
	if( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
		interpolate_v3_xna( dst, b, inter );
	else
#endif
	interpolate_v3<FP32,FP32>( dst, b, inter );
}
FINLINE	void	interpolate_v3( FP32* CONST dst,
								FP32 CONST * CONST a, FP32 CONST * CONST b, FP32 CONST * CONST c, FP32 CONST * CONST d,
								FP32 CONST inter_ab, FP32 CONST inter_ac )
{
#if AAA_USE_XNA()
	if( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
	{
		FP32 tmp[3];
		interpolate_v3_xna( dst, a, b, inter_ab );
		interpolate_v3_xna( tmp, c, d, inter_ab );
		interpolate_v3_xna( dst, tmp, inter_ac );
	}
	else
#endif
	{
		FP32 tmp[3];
		interpolate_v3( dst, a, b, inter_ab );
		interpolate_v3( tmp, c, d, inter_ab );
		interpolate_v3( dst, tmp, inter_ac );
	}
}

// INTERPOLATE V4
//
//	dst == src possible
template< class T, class S >
CONSTEXPR	FINLINE	void	interpolate_v4( T* CONST dst,	T CONST * a,	T CONST * CONST b,	S CONST inter ) NOEXCEPT
{
	S CONST one_minus_inter = S(1. - inter);
	dst[0] = T(	a[0] * one_minus_inter + b[0] * inter );
	dst[1] = T(	a[1] * one_minus_inter + b[1] * inter );
	dst[2] = T(	a[2] * one_minus_inter + b[2] * inter );
	dst[3] = T(	a[3] * one_minus_inter + b[3] * inter );
}
//todo do better in xna
//todo template version
CONSTEXPR	FINLINE	void	interpolate_v4( FP32* CONST dst, FP32 CONST * CONST a, FP32 CONST * CONST b, FP32 CONST inter ) NOEXCEPT
{
#if AAA_USE_XNA()
	if ( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
		interpolate_v4_xna( dst, a, b, inter );
	else
#endif
		interpolate_v4<FP32,FP32>( dst, a, b, inter );
}
template< class T, class S >
CONSTEXPR	FINLINE	void	interpolate_v4( T* dst,	T CONST * CONST b,	S CONST inter ) NOEXCEPT
{
	dst[0] += T( inter * (b[0]-dst[0]) );
	dst[1] += T( inter * (b[1]-dst[1]) );
	dst[2] += T( inter * (b[2]-dst[2]) );
	dst[3] += T( inter * (b[3]-dst[3]) );
}
CONSTEXPR	FINLINE	void	interpolate_v4( FP32* CONST dst, FP32 CONST * CONST b, FP32 CONST inter ) NOEXCEPT
{
#if AAA_USE_XNA()
	if( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
		interpolate_v4_xna( dst, b, inter );
	else
#endif
	interpolate_v4<FP32,FP32>( dst, b, inter );
}

//INTEGRATE
extern			REAL	integrate_1d_real(	REAL CONST * p,		REAL CONST min,		REAL CONST max );

//	DIST
extern	CONSTEXPR	REAL	dist_squared_v3r(	REAL CONST * a,		REAL CONST * b		);
#if	!AAA_REAL_IS_DOUBLE()
extern	CONSTEXPR	REAL	dist_squared_v3r(	REAL CONST * a,		DOUBLE CONST * b	);
extern	CONSTEXPR	DOUBLE	dist_squared_v3(	DOUBLE CONST * a,	DOUBLE CONST * b	);
#endif
extern	FINLINE		REAL	dist_v3r(			REAL CONST * a,		REAL CONST * b		);
#if	!AAA_REAL_IS_DOUBLE()
extern	FINLINE		REAL	dist_v3r(			REAL CONST * a,		DOUBLE CONST * b	);
extern	FINLINE		DOUBLE	dist_v3r(			DOUBLE CONST * a,	DOUBLE CONST * b	);
#endif
extern	CONSTEXPR	REAL	dist_box_v3r(		REAL CONST * a,		REAL CONST * b		);
extern	CONSTEXPR	REAL	dist_box_v3r(		REAL CONST * a,		REAL CONST x,		REAL CONST y, REAL CONST z );

//template< class T > 
//		CONSTEXPR	DOUBLE	sum_squared_v2(		T CONST a,			T CONST b		)	NOEXCEPT				{ return a*a + b*b;	}
		CONSTEXPR	FP32	sum_squared_v2r(	FP32 CONST a,		FP32 CONST b	)	NOEXCEPT				{ return a*a + b*b;	}
																							  
//		FINLINE		DOUBLE	norm_v2(			T CONST a,			T CONST b		)	NOEXCEPT				{ return SQRT( sum_squared_v2(  a, b ) ); }
		FINLINE		FP32	norm_v2r(			FP32 CONST a,		FP32 CONST b	)	NOEXCEPT				{ return SQRT( sum_squared_v2r( a, b ) ); }
//template< class T >																								  
//		CONSTEXPR	DOUBLE	sum_squared_v3(		T CONST a,			T CONST b,		T CONST c )	NOEXCEPT		{ return a*a + b*b + c*c; }
		CONSTEXPR	REAL	sum_squared_v3r(	FP32 CONST a,		FP32 CONST b,	FP32 CONST c )	NOEXCEPT	{ return a*a + b*b + c*c; }
//template< class T > 																							  
//		FINLINE		DOUBLE	norm_v3(			T CONST a,			T CONST b,		T CONST c )	NOEXCEPT		{ return SQRT( sum_squared_v3(  a, b, c ) ); }
		FINLINE		REAL	norm_v3r(			FP32 CONST a,		FP32 CONST b,	FP32 CONST c )	NOEXCEPT	{ return SQRT( sum_squared_v3r( a, b, c ) ); }

//template< class T > 																							  
//		FINLINE		T		norm_squared_v3(	T CONST * a	)	{	return sum_squared_v2r( *a, *(a+1) );	}
extern	FINLINE		FP32	norm_squared_v3r(	FP32 CONST * a		);
extern	FINLINE		FP32	norm_v3r(			FP32 CONST * a		);
extern	FINLINE		FP32	norm_squared_v2r(	FP32 CONST * a		);
extern	FINLINE		FP32	norm_v2r(			FP32 CONST * a		);

//	MISC
extern	CONSTEXPR	REAL	get_dist_squared_if_in_dist_squared_v3r(	REAL CONST * a,								REAL CONST dist_squared );
extern	CONSTEXPR	REAL	get_dist_squared_if_in_dist_squared_v3r(	REAL CONST * a,	REAL CONST * b,				REAL CONST dist_squared );
extern	CONSTEXPR	bool	is_dist_squared_less_v3r(					REAL CONST * a,	REAL CONST * b,				REAL dist_squared );
extern	CONSTEXPR	bool	is_dist_squared_less_v3r(					REAL CONST x,	REAL CONST y, REAL CONST z,	REAL dist_squared );
extern	CONSTEXPR	bool	is_dist_squared_inside_v3r(					REAL CONST * a, REAL CONST * b, REAL dist_min_squared, REAL dist_max_squared );

CONSTEXPR	bool	is_in_dist_squared_v2d(						DOUBLE CONST x, DOUBLE CONST y,				DOUBLE dist_squared ) NOEXCEPT
{
	dist_squared -=	x*x;
	if( dist_squared < 0. )
		return false;
	return dist_squared > y*y;
}

//	SCALE
template<class T>
CONSTEXPR	void	scale_v3_cpy_v4(	T* dst, T CONST * src, T CONST f ) NOEXCEPT
{
	dst[0] = src[0]	* f;
	dst[1] = src[1]	* f;
	dst[2] = src[2]	* f;
	dst[3] = src[3];
}
template<class T>
CONSTEXPR	void	scale_v3_cpy_v4(	T* dst, T CONST * src ) NOEXCEPT
{
	scale_v3_cpy_v4( dst, src, src[4] );
}

//todoq add the version with an axe param
template<class T,class S1,class S2>
CONSTEXPR	void	scale_v4( T* dst, S1 CONST * src, S2 CONST f )
{
	dst[0] = T( src[0]	* f );
	dst[1] = T( src[1]	* f );
	dst[2] = T( src[2]	* f );
	dst[3] = T( src[3]	* f );
}
template<class T>
CONSTEXPR	void	scale_v4( T* dst, T CONST * src, T CONST f )
{
	dst[0] = src[0]	* f;
	dst[1] = src[1]	* f;
	dst[2] = src[2]	* f;
	dst[3] = src[3]	* f;
}
FINLINE	void	scale_v4( FP32* CONST dst, FP32 CONST * src, FP32 CONST f )
{
#if AAA_USE_XNA()
	if( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
		scale_v4_xna( dst, src, f );
	else
#endif
		scale_v4<FP32>( dst, src, f );
}
FINLINE	void	scale_v4( FP32* CONST dst, FP32 CONST f )
{
#if AAA_USE_XNA()
	if( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
		scale_v4_xna( dst, f );
	else
#endif
	{ 
		//dst[0] = FP32( dst[0] * f );
		//dst[1] = FP32( dst[1] * f );
		//dst[2] = FP32( dst[2] * f );
		//dst[3] = FP32( dst[3] * f );
		dst[0] *= f;
		dst[1] *= f;
		dst[2] *= f;
		dst[3] *= f;
	}
}

template<class T,class S1,class S2>
CONSTEXPR	void	scale_v3( T* dst, S1 CONST * src, S2 CONST f )
{
	dst[0] = T( src[0]	* f );
	dst[1] = T( src[1]	* f );
	dst[2] = T( src[2]	* f );
}
template< class T, class S >
CONSTEXPR	void	scale_v3( T* dst, S CONST f )
{
	dst[0] = T( dst[0] * f );
	dst[1] = T( dst[1] * f );
	dst[2] = T( dst[2] * f );
}
FINLINE	void	scale_v3( FP32* CONST dst, FP32 CONST f )
{
#if	AAA_USE_XNA()
	if( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
		scale_v3_xna( dst, f );
	else
#endif
	{ 
		dst[0] = FP32( dst[0] * f );
		dst[1] = FP32( dst[1] * f );
		dst[2] = FP32( dst[2] * f );
	}
}

template<class T, class S>
CONSTEXPR	void	scale_v2( T* dst, T CONST * src, S CONST f )
{
	dst[0] = T( src[0] * f );
	dst[1] = T( src[1] * f );
}
template<class T, class S>
CONSTEXPR	void	scale_v2( T* dst, S CONST f )
{
	dst[0] = T( dst[0] * f );
	dst[1] = T( dst[1] * f );
}
FINLINE	void	scale_v2( FP32* CONST dst, FP32 CONST f )
{
#if	AAA_USE_XNA()
	if( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
		scale_v2_xna( dst, f );
	else
#endif
	{
		dst[0] *= f;
		dst[1] *= f;
	}
}

//	MIX
//	dst == src possible

template<class T>
		CONSTEXPR	void	mix_v4(	T* dst, T CONST * a, T CONST fa, T CONST * b, T CONST fb )
		{
		#if	AAA_USE_XNA()
			if( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
				mix_v4_xna( dst, a, fa, b, fb );
			else
		#endif
			{
				dst[0] = fa*a[0] + fb*b[0];
				dst[1] = fa*a[1] + fb*b[1];
				dst[2] = fa*a[2] + fb*b[2];
				dst[3] = fa*a[3] + fb*b[3];
			}
		}
template<class T>
		CONSTEXPR	void	mix_v4(	T* dst, T CONST fa, T CONST * b,	T CONST fb )
		{
		#if	AAA_USE_XNA()
			if( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
				mix_v4_xna( dst, fa, b, fb );
			else
		#endif
			{
				dst[0] = fa*dst[0] + fb*b[0];
				dst[1] = fa*dst[1] + fb*b[1];
				dst[2] = fa*dst[2] + fb*b[2];
				dst[3] = fa*dst[3] + fb*b[3];
			}
		}
template<class T>
		CONSTEXPR	void	mix_v3(	T* dst, T CONST * a, T CONST fa, T CONST * b, T CONST fb )
		{
		#if	AAA_USE_XNA()
			if( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
				mix_v3_xna( dst, a, fa, b, fb );
			else
		#endif
			{
				dst[0] = fa*a[0] + fb*b[0];
				dst[1] = fa*a[1] + fb*b[1];
				dst[2] = fa*a[2] + fb*b[2];
			}
		}
template<class T>
		CONSTEXPR	void	mix_v3(	T* dst, T CONST fa, T CONST * b,	T CONST fb )
		{
		#if	AAA_USE_XNA()
			if( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
				mix_v3_xna( dst, fa, b, fb );
			else
		#endif
			{
				dst[0] = fa*dst[0] + fb*b[0];
				dst[1] = fa*dst[1] + fb*b[1];
				dst[2] = fa*dst[2] + fb*b[2];
			}
		}
template<class T>
		CONSTEXPR	void	mix_v2(	T* dst, T CONST * a, T CONST fa, T CONST * b, T CONST fb )
		{
		#if	AAA_USE_XNA()
			if( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
				mix_v2_xna( dst, a, fa, b, fb );
			else
		#endif
			{
				dst[0] = fa*a[0] + fb*b[0];
				dst[1] = fa*a[1] + fb*b[1];
			}
		}
template<class T>
		CONSTEXPR	void	mix_v2(	T* dst,	T CONST fa,	T CONST * b,  T CONST fb )
		{
		#if	AAA_USE_XNA()
			if ( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
				mix_v2_xna( dst, fa, b, fb );
			else
		#endif
			{
				dst[0] = fa*dst[0] + fb*b[0];
				dst[1] = fa*dst[1] + fb*b[1];
			}
		}
//	ADD
//todoopt use pre incrementation in other functions
template<class T>
CONSTEXPR	void	add_v4( T* dst, T CONST * a, T CONST * b )
{
#if	AAA_USE_XNA()
	if( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
		add_v4_xna( dst, a, b );
	else
#endif
	{
		dst[0] = a[0] + b[0];
		dst[1] = a[1] + b[1];
		dst[2] = a[2] + b[2];
		dst[3] = a[3] + b[3];
	}
}
template<class T>
CONSTEXPR	void	add_v4( T* dst, T CONST * a )
{
#if	AAA_USE_XNA()
	if( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
		add_v4_xna( dst, a );
	else
#endif
	{
		dst[0] += a[0];
		dst[1] += a[1];
		dst[2] += a[2];
		dst[3] += a[3];
	}
}
template<class T>
CONSTEXPR	void	add_v4( T* dst, T CONST a, T CONST * b )
{
#if	0	//AAA_USE_XNA()
	if ( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
		add_v4_xna( dst, a, b );
	else
#endif
	{
		dst[0] = a + b[0];
		dst[1] = a + b[1];
		dst[2] = a + b[2];
		dst[3] = a + b[3];
	}
}
template<class T>
CONSTEXPR	void	add_v4( T* dst, T CONST * a, T CONST b )
{
	add_v4( dst, b, a );
}
template<class T>
CONSTEXPR	void	add_v4( T* dst, T CONST b )
{
#if	0	//AAA_USE_XNA()
	if ( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
		add_v4_xna( dst, b );
	else
#endif
	{
		dst[0] += b;
		dst[1] += b;
		dst[2] += b;
		dst[3] += b;
	}
}

template<class T>
CONSTEXPR	void	add_v3( T* dst, T CONST * a, T CONST * b )
{
#if	AAA_USE_XNA()
	if( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
		add_v3_xna( dst, a, b );
	else
#endif
	{
		dst[0] = a[0] + b[0];
		dst[1] = a[1] + b[1];
		dst[2] = a[2] + b[2];
	}
}
template<class T, class S>
CONSTEXPR	void	add_v3( T* dst, T CONST * a, S CONST * b )
{
#if 0	//AAA_USE_XNA()
	if ( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
		add_v3_xna( dst, a, b );
	else
#endif
	{
		dst[0] = T(	a[0] + b[0] );
		dst[1] = T(	a[1] + b[1] );
		dst[2] = T(	a[2] + b[2] );
	}
}
template<class T, class S>
CONSTEXPR	void	add_v3( T* dst, S CONST * a, S CONST * b )
{
#if 0	//AAA_USE_XNA()
	if ( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
		add_v3_xna( dst, a, b );
	else
#endif
	{
		dst[0] = T(	a[0] + b[0] );
		dst[1] = T(	a[1] + b[1] );
		dst[2] = T(	a[2] + b[2] );
	}
}
template<class T, class S>
CONSTEXPR	void	add_v3( T* dst, S CONST * a ) NOEXCEPT
{
#if	0 //AAA_USE_XNA()
	if ( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
		add_v3_xna( dst, a );
	else
#endif
	{
		dst[0] += a[0];
		dst[1] += a[1];
		dst[2] += a[2];
	}
}
template<class T>
CONSTEXPR	void	add_v3( T* dst, T CONST a, T CONST * b )
{
#if	0	//AAA_USE_XNA()
	if ( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
		add_v3_xna( dst, a, b );
	else
#endif
	{
		dst[0] = a + b[0];
		dst[1] = a + b[1];
		dst[2] = a + b[2];
	}
}
template<class T>
CONSTEXPR	void	add_v3( T* dst, T CONST b )
{
#if	0	//AAA_USE_XNA()
	if ( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
		add_v3_xna( dst, b );
	else
#endif
	{
		dst[0] += b;
		dst[1] += b;
		dst[2] += b;
	}
}
template<class T,class S>
CONSTEXPR	void	add_v3( T* dst, S CONST x, S CONST y, S CONST z )
{
#if	0	//AAA_USE_XNA()
	if ( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
		add_v3_xna( dst, x,y,z );
	else
#endif
	{
		dst[0] += x;
		dst[1] += y;
		dst[2] += z;
	}
}
template<class T>
CONSTEXPR	void	add_v2( T* dst, T CONST * a, T CONST * b )
{
#if	AAA_USE_XNA()
	if( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
		add_v2_xna( dst, a, b );
	else
#endif
	{
		dst[0] = a[0] + b[0];
		dst[1] = a[1] + b[1];
	}
}
template<class T>
CONSTEXPR	void	add_v2( T* dst, T CONST * a )
{
//#if	AAA_USE_XNA()
//	if( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
//		add_v2_xna( dst, a );
//	else
//#endif
	{
		dst[0] += a[0];
		dst[1] += a[1];
	}
}
template<class T>
CONSTEXPR	void	add_v2( T* dst, T CONST a, T CONST * b )
{
#if	0	//AAA_USE_XNA()
	if ( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
		add_v2_xna( dst, a, b );
	else
#endif
	{
		dst[0] = a + b[0];
		dst[1] = a + b[1];
	}
}
template<class T>
CONSTEXPR	void	add_v2( T* dst, T CONST * a, T CONST b )
{
	add_v2( dst, b, a );
}
template<class T>
CONSTEXPR	void	add_v2( T* dst, T CONST b )
{
#if	0	//AAA_USE_XNA()
	if ( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
		add_v2_xna( dst, b );
	else
#endif
	{
		dst[0] += b;
		dst[1] += b;
	}
}


template< class T, class S >
		CONSTEXPR	void	add_scale_v3( T* dst, T CONST * a, T CONST * b, S CONST factor )
		{
		#if	0 //AAA_USE_XNA()
			if ( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
				add_scale_v3_xna( dst, a, b, factor );
			else
		#endif
			{
				*dst		= T( *a		+ *b		* factor	);
		#if AAA_V_DOIT_WITH_INC()
				*++dst		= T( *++a	+ *++b		* factor	);
				*++dst		= T( *++a	+ *++b		* factor	);
		#else
				*(dst+1)	= T( *(a+1)	+ *(b+1)	* factor	);
				*(dst+2)	= T( *(a+2)	+ *(b+2)	* factor	);
		#endif
			}
		}
template<>
		CONSTEXPR void	add_scale_v3<REAL,REAL>( REAL* dst, REAL CONST * a, REAL CONST * b, REAL CONST factor )
		{
		#if	AAA_USE_XNA()
			if ( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
				add_scale_v3_xna( dst, a, b, factor );
			else
		#endif
			{
				*dst		= REAL( *a		+ *b		* factor	);
		#if AAA_V_DOIT_WITH_INC()
				*++dst		= REAL( *++a	+ *++b		* factor	);
				*++dst		= REAL( *++a	+ *++b		* factor	);
		#else
				*(dst+1)	= REAL( *(a+1)	+ *(b+1)	* factor	);
				*(dst+2)	= REAL( *(a+2)	+ *(b+2)	* factor	);
		#endif
			}
		}
template< class T, class S, class V >
		CONSTEXPR	void	add_scale_v3( T* CONST dst, S CONST * b, V CONST factor )
		{

			*dst		+= T( *b		*	factor	);
		#if AAA_V_DOIT_WITH_INC()
			*++dst		+= T( *++b		*	factor	);
			*++dst		+= T( *++b		*	factor	);
		#else
			*(dst+1)	+= T( *(b+1)	*	factor	);
			*(dst+2)	+= T( *(b+2)	*	factor	);
		#endif
		}
template<>
		CONSTEXPR	void	add_scale_v3<REAL,REAL,REAL>( REAL* CONST dst, REAL CONST * b, REAL CONST factor )
		{
		#if	AAA_USE_XNA()
			if ( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
				add_scale_v3_xna( dst, b, factor );
			else
		#endif
			{
				*dst		+= REAL( *b		*	factor	);
		#if AAA_V_DOIT_WITH_INC()
				*++dst		+= REAL( *++b	*	factor	);
				*++dst		+= REAL( *++b	*	factor	);
		#else
				*(dst+1)	+= REAL( *(b+1)	*	factor	);
				*(dst+2)	+= REAL( *(b+2)	*	factor	);
		#endif
			}
		}

template< class T, class S >
		CONSTEXPR	void	add_scale_v2( T* dst, T CONST * a, T CONST * b, S CONST factor )
		{
			*dst		= *a		+ T( *b		* factor);
		#if AAA_V_DOIT_WITH_INC()
			*++dst		= *++a		+ T( *++b	* factor);
		#else
			*(dst+1)	= *(a+1)	+ T( *(b+1)	* factor);
		#endif
		}
template< class T, class S >
		CONSTEXPR	void	add_scale_v2( T* dst, T CONST * b, S CONST factor )
		{
			*dst		+= *b		* factor;
		#if AAA_V_DOIT_WITH_INC()
			*++dst		+= *++b		* factor;
		#else
			*(dst+1)	+= *(b+1)	* factor;
		#endif
		}

extern	CONSTEXPR	void	add_scale_add_v3r(	REAL* dst,	REAL CONST * a,		REAL CONST * b,	REAL CONST factor		);
extern	CONSTEXPR	void	add_then_scale_v2r( REAL* dst,	REAL CONST * a,		REAL CONST * b,	REAL CONST factor		);
extern	CONSTEXPR	void	add_then_scale_v3r( REAL* dst,	REAL CONST * a,		REAL CONST * b,	REAL CONST factor		);

extern	CONSTEXPR	void	add_mul_v3r(		REAL* dst,	REAL CONST * a,		REAL CONST * b,	REAL CONST * c	);
extern	CONSTEXPR	void	add_mul_v3r(		REAL* dst,						REAL CONST * b,	REAL CONST * c	);
extern	CONSTEXPR	void	add_mul_v2r(		REAL* dst,	REAL CONST * a,		REAL CONST * b,	REAL CONST * c	);
extern	CONSTEXPR	void	add_mul_v2r(		REAL* dst,						REAL CONST * b,	REAL CONST * c	);

extern	CONSTEXPR	void	offset_v3r(			REAL* dst,	REAL CONST * src,	REAL CONST * a,	INT32 nb	);
extern	CONSTEXPR	void	offset_v3r(			REAL* dst,						REAL CONST * a,	INT32 nb	);


//	SUB

template<class T>
CONSTEXPR	void	sub_v4( T* dst, T CONST * a, T CONST * b )
{
#if	AAA_USE_XNA()
	if ( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
		sub_v4_xna( dst, a, b );
	else
#endif
	{
		dst[0] = a[0] - b[0];
		dst[1] = a[1] - b[1];
		dst[2] = a[2] - b[2];
		dst[3] = a[3] - b[3];
	}
}
template<class T>
CONSTEXPR	void	sub_v4( T* dst, T CONST * a )
{
#if	AAA_USE_XNA()
	if ( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
		sub_v4_xna( dst, a );
	else
#endif
	{
		dst[0] -= a[0];
		dst[1] -= a[1];
		dst[2] -= a[2];
		dst[3] -= a[3];
	}
}
template<class T>
CONSTEXPR	void	sub_v4( T* dst, T CONST a, T CONST * b )
{
#if	0	//AAA_USE_XNA()
	if ( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
		sub_v4_xna( dst, a, b );
	else
#endif
	{
		dst[0] = a - b[0];
		dst[1] = a - b[1];
		dst[2] = a - b[2];
		dst[3] = a - b[3];
	}
}
template<class T>
CONSTEXPR	void	sub_v4( T* dst, T CONST * a, T CONST b )
{
#if	0	//AAA_USE_XNA()
	if ( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
		sub_v4_xna( dst, a, b );
	else
#endif
	{
		dst[0] = a[0] - b;
		dst[1] = a[1] - b;
		dst[2] = a[2] - b;
		dst[3] = a[3] - b;
	}
}
template<class T>
CONSTEXPR	void	sub_v4( T* dst, T CONST c )
{
#if	0	//AAA_USE_XNA()
	if ( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
		sub_v4_xna( dst, c );
	else
#endif
	{
		dst[0] -= c;
		dst[1] -= c;
		dst[2] -= c;
		dst[3] -= c;
	}
}

template<class T, class S>
CONSTEXPR	void	sub_v3( T* dst, S CONST * a, S CONST * b )
{
#if	0 //AAA_USE_XNA()
	if ( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
		sub_v3_xna( dst, a, b );
	else
#endif
	{
		dst[0] = T(	a[0] - b[0] );
		dst[1] = T(	a[1] - b[1] );
		dst[2] = T(	a[2] - b[2] );
	}
}
template<class T>
CONSTEXPR	void	sub_v3( T* dst, T CONST * a, T CONST * b )
{
#if	AAA_USE_XNA()
	if ( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
		sub_v3_xna( dst, a, b );
	else
#endif
		sub_v3<T,T>( dst, a, b );
}
template<class T>
CONSTEXPR	void	sub_v3( T* dst, T CONST * a )
{
#if	AAA_USE_XNA()
	if ( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
		sub_v3_xna( dst, a );
	else
#endif
	{
		dst[0] -= a[0];
		dst[1] -= a[1];
		dst[2] -= a[2];
	}
}
template<class T>
CONSTEXPR	void	sub_v3( T* dst, T CONST a, T CONST * b )
{
#if	0	//AAA_USE_XNA()
	if ( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
		sub_v3_xna( dst, a, b );
	else
#endif
	{
		dst[0] = a - b[0];
		dst[1] = a - b[1];
		dst[2] = a - b[2];
	}
}
template<class T>
CONSTEXPR	void	sub_v3( T* dst, T CONST * a, T CONST b )
{
#if	0	//AAA_USE_XNA()
	if ( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
		sub_v3_xna( dst, a, b );
	else
#endif
	{
		dst[0] = a[0] - b;
		dst[1] = a[1] - b;
		dst[2] = a[2] - b;
	}
}
template<class T>
CONSTEXPR	void	sub_v3( T* dst, T CONST c )
{
#if	0	//AAA_USE_XNA()
	if ( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
		sub_v3_xna( dst, c );
	else
#endif
	{
		dst[0] -= c;
		dst[1] -= c;
		dst[2] -= c;
	}
}
template<class T>
CONSTEXPR	void	sub_reverse_v3( T* dst, T CONST * b )
{
	sub_v3( dst, b, dst );
}
template<class T>
CONSTEXPR	void	sub_v2( T* dst, T CONST * a, T CONST * b )
{
#if	0	//AAA_USE_XNA()
	if ( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
		sub_v2_xna( dst, a, b );
	else
#endif
	{
		dst[0] = a[0] - b[0];
		dst[1] = a[1] - b[1];
	}
}
template<class T>
CONSTEXPR	void	sub_v2( T* dst, T CONST * a )
{
#if	0	//AAA_USE_XNA()
	if ( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
		sub_v2_xna( dst, a );
	else
#endif
	{
		dst[0] -= a[0];
		dst[1] -= a[1];
	}
}
template<class T>
CONSTEXPR	void	sub_v2( T* dst, T CONST a, T CONST * b )
{
#if	0	//AAA_USE_XNA()
	if ( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
		sub_v2_xna( dst, a, b );
	else
#endif
	{
		dst[0] = a - b[0];
		dst[1] = a - b[1];
	}
}
template<class T>
CONSTEXPR	void	sub_v2( T* dst, T CONST * a, CONST T b )
{
#if	0	//AAA_USE_XNA()
	if ( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
		sub_v2_xna( dst, a, b );
	else
#endif
	{
		dst[0] = a[0] - b;
		dst[1] = a[1] - b;
	}
}
template<class T>
CONSTEXPR	void	sub_v2( T* dst, CONST T c )
{
#if	0	//AAA_USE_XNA()
	if ( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
		sub_v2_xna( dst, c );
	else
#endif
	{
		dst[0] -= c;
		dst[1] -= c;
	}
}


//	dst = a-b
extern	CONSTEXPR	void	sub_array_v3r(				REAL* dst,	REAL CONST * a, REAL CONST * b,	INT32 nb );
//	dst -= a
extern	CONSTEXPR	void	sub_array_v3r(				REAL* dst,	REAL CONST * a,					INT32 nb );
//	dst = (a-b)*factor
extern	CONSTEXPR	void	sub_then_scale_v3r(			REAL* dst,	REAL CONST * a, REAL CONST * b,	REAL CONST factor );
//	a = (a-b)*f;
extern	CONSTEXPR	void	sub_then_scale_v3r(			REAL* a,	REAL CONST * b,					REAL CONST factor );
//	dst += (a-b)
extern	CONSTEXPR	void	sub_then_add_to_v3r(		REAL* dst,	REAL CONST * a, REAL CONST * b );
//	dst += (a-b)
extern	CONSTEXPR	void	sub_then_add_to_array_v3r(	REAL* dst,	REAL CONST * a, REAL CONST * b,	INT32 nb );

//	symmetry of b relative to a : dst = a + (a-b) = 2a - b
extern	CONSTEXPR	void	sym_v3r(					REAL* dst,	REAL CONST * a, REAL CONST * b );

//	MUL
template<class T>
		CONSTEXPR	void	mul_v2( T* dst, T CONST * a )
		{
			*dst		*= *a		;
		#if AAA_V_DOIT_WITH_INC()
			*++dst		*= *++a		;
		#else
			*(dst+1)	*= *(a+1)	;
		#endif
		}
template<class T, class S>
		CONSTEXPR	void	mul_v2( T* dst, S CONST * a )
		{
			*dst		*= T(*a)		;
		#if AAA_V_DOIT_WITH_INC()
			*++dst		*= T(*++a)		;
		#else
			*(dst+1)	*= T(*(a+1))	;
		#endif
		}
template<class T>
		CONSTEXPR	void	mul_v2( T* dst, T CONST * a, T CONST * b )
		{
			*dst		= *a		* *b	;
		#if AAA_V_DOIT_WITH_INC()
			*++dst		= *++a		* *++b	;
		#else
			*(dst+1)	= *(a+1)	* *(b+1);
		#endif
		}
template<class T>
		CONSTEXPR	void	mul_v3( T* dst, T CONST * a )
		{
		#if	AAA_USE_XNA()
			if ( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
				return	mul_v3_xna( dst, a );
			else
		#endif
			{
				*dst		*= *a		;
		#if AAA_V_DOIT_WITH_INC()
				*++dst		*= *++a		;
				*++dst		*= *++a		;
		#else
				*(dst+1)	*= *(a+1)	;
				*(dst+2)	*= *(a+2)	;
		#endif
			}
		}
template<class T>
		CONSTEXPR	void	mul_v3( T* dst, T CONST * a, T CONST * b )
		{	
		#if	AAA_USE_XNA()
			if ( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
				return	mul_v3_xna( dst, a, b );
			else
		#endif
			{
				*dst		= *a		*	*b		;
		#if AAA_V_DOIT_WITH_INC()
				*++dst		= *++a		*	*++b	;
				*++dst		= *++a		*	*++b	;
		#else
				*(dst+1)	= *(a+1)	*	*(b+1)	;
				*(dst+2)	= *(a+2)	*	*(b+2)	;
		#endif
			}
		}
template<class T,class S1,class S2>
		CONSTEXPR	void	mul_v3( T* dst, S1 CONST * a, S2 CONST * b )
		{
			*dst		=	T(	*a		*	*b		);
		#if AAA_V_DOIT_WITH_INC()
			*++dst		=	T(	*++a	*	*++b	);
			*++dst		=	T(	*++a	*	*++b	);
		#else
			*(dst+1)	=	T(	*(a+1)	*	*(b+1)	);
			*(dst+2)	=	T(	*(a+2)	*	*(b+2)	);
		#endif
		}
template<class T>
		CONSTEXPR	void	mul_v4( T* dst, T CONST * a )
		{
		#if	AAA_USE_XNA()
			if ( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
				return	mul_v4_xna( dst, a );
			else
		#endif
			{
				*dst		*= *a		;
		#if AAA_V_DOIT_WITH_INC()
				*++dst		*= *++a		;
				*++dst		*= *++a		;
				*++dst		*= *++a		;
		#else
				*(dst+1)	*= *(a+1)	;
				*(dst+2)	*= *(a+2)	;
				*(dst+3)	*= *(a+3)	;
		#endif
			}
		}
template<class T>
		CONSTEXPR	void	mul_v4( T* dst, T CONST * a, T CONST * b )
		{
		#if	AAA_USE_XNA()
			if ( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
				return	mul_v4_xna( dst, a, b );
			else
		#endif
			{
				*dst		= *a		* *b	;
		#if AAA_V_DOIT_WITH_INC()
				*++dst		= *++a		* *++b	;
				*++dst		= *++a		* *++b	;
				*++dst		= *++a		* *++b	;
		#else
				*(dst+1)	= *(a+1)	* *(b+1);
				*(dst+2)	= *(a+2)	* *(b+2);
				*(dst+3)	= *(a+3)	* *(b+3);
		#endif
			}
		}
template<class T,class S1,class S2>
		CONSTEXPR	void	mul_v4( T* dst, S1 CONST * a, S2 CONST * b )
		{
			*dst		=	T(	*a		*	*b		);
		#if AAA_V_DOIT_WITH_INC()
			*++dst		=	T(	*++a	*	*++b	);
			*++dst		=	T(	*++a	*	*++b	);
			*++dst		=	T(	*++a	*	*++b	);
		#else
			*(dst+1)	=	T(	*(a+1)	*	*(b+1)	);
			*(dst+2)	=	T(	*(a+2)	*	*(b+2)	);
			*(dst+3)	=	T(	*(a+3)	*	*(b+3)	);
		#endif
		}

template<class T, class S>
		CONSTEXPR	void	mul_add_v3( T* dst, S CONST * a, S CONST * b, S CONST * c )
		{
		#if	AAA_USE_XNA()
			if ( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
				return	mul_add_v3_xna( dst, a, b, c );
			else
		#endif
			{
				*dst		= *a		* *b		+ *c		;
		#if AAA_V_DOIT_WITH_INC()
				*++dst		= *++a		* *++b		+ *++c		;
				*++dst		= *++a		* *++b		+ *++c		;
		#else
				*(dst+1)	= *(a+1)	* *(b+1)	+ *(c+1)	;
				*(dst+2)	= *(a+2)	* *(b+2)	+ *(c+2)	;
		#endif
			}
		}
template<class T, class S>
		CONSTEXPR	void	mul_add_v3_nb( T* dst, S CONST * a, S CONST * b, S CONST * c, INT32 nb )
		{
		#if	AAA_USE_XNA()
			if ( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
			{	//todo this could be optimized in an xna fns
				for( ; nb>0; --nb )
				{
					mul_add_v3_xna( dst, a, b, c );
					a += 3;
					dst += 3;
				}
			}
			else
		#endif
			{
				for( ; nb>0; --nb )
				{
					*dst		= *a		* *b		+ *c		;
		#if AAA_V_DOIT_WITH_INC()
					*++dst		= *++a		* *++b		+ *++c		;
					*++dst		= *++a		* *++b		+ *++c		;
		#else
					*(dst+1)	= *(a+1)	* *(b+1)	+ *(c+1)	;
					*(dst+2)	= *(a+2)	* *(b+2)	+ *(c+2)	;
		#endif
					a += 3;
					dst += 3;
				}
			}
		}
template<class T>
		CONSTEXPR	void	mul_add_v4( T* dst, T CONST * a, T CONST * b, T CONST * c )
		{
		#if	AAA_USE_XNA()
			if ( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
				return	mul_add_v4_xna( dst, a, b, c );
			else
		#endif
			{
				*dst		= *a		* *b		+ *c		;
		#if AAA_V_DOIT_WITH_INC()
				*++dst		= *++a		* *++b		+ *++c		;
				*++dst		= *++a		* *++b		+ *++c		;
				*++dst		= *++a		* *++b		+ *++c		;
		#else
				*(dst+1)	= *(a+1)	* *(b+1)	+ *(c+1)	;
				*(dst+2)	= *(a+2)	* *(b+2)	+ *(c+2)	;
				*(dst+3)	= *(a+3)	* *(b+3)	+ *(c+3)	;
		#endif
			}
		}

template<class T, class S>
		CONSTEXPR	void	mul_add_v3( T* dst, S CONST * b, S CONST * c )
		{
		#if	AAA_USE_XNA()
			if ( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
				return	mul_add_v3_xna( dst, b, c );
			else
		#endif
			{
				*dst	= *dst		* *b		+ *c		;
		#if AAA_V_DOIT_WITH_INC()
				++dst;
				*dst	= *dst		* *++b		+ *++c		;
				++dst;
				*dst	= *dst		* *++b		+ *++c		;
		#else
				++dst;
				*dst	= *dst		* *(b+1)	+ *(c+1)	;
				++dst;
				*dst	= *dst		* *(b+2)	+ *(c+2)	;
		#endif
			}
		}

template<class T, class S>
		CONSTEXPR	void	mul_add_v4( T* dst, S CONST * b, S CONST * c )
		{
		#if	AAA_USE_XNA()
			if ( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
				return	mul_add_v4_xna( dst, b, c );
			else
		#endif
			{
				*dst	= *dst		* *b		+ *c		;
		#if AAA_V_DOIT_WITH_INC()
				++dst;
				*dst	= *dst		* *++b		+ *++c		;
				++dst;
				*dst	= *dst		* *++b		+ *++c		;
				++dst;
				*dst	= *dst		* *++b		+ *++c		;
		#else
				++dst;
				*dst	= *dst		* *(b+1)	+ *(c+1)	;
				++dst;
				*dst	= *dst		* *(b+2)	+ *(c+2)	;
				++dst;
				*dst	= *dst		* *(b+3)	+ *(c+3)	;
		#endif
			}
		}

template< class T >
		CONSTEXPR	void	mul_scale_v2( T* dst, T CONST * a, T CONST * b, REAL CONST f )
		{
			*dst		= *a		* *b		* f	;
		#if AAA_V_DOIT_WITH_INC()
			*++dst		= *++a		* *++b		* f	;
		#else
			*(dst+1)	= *(a+1)	* *(b+1)	* f	;
		#endif
		}
template< class T >
		CONSTEXPR	void	mul_scale_v3( T* dst, T CONST * a, T CONST * b, REAL CONST f )
		{
		#if	AAA_USE_XNA()
			if( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
				return	mul_scale_v3_xna( dst, a, b, f );
			else
		#endif
			{
				*dst		= *a		* *b		* f	;
			#if AAA_V_DOIT_WITH_INC()
				*++dst		= *++a		* *++b		* f	;
				*++dst		= *++a		* *++b		* f	;
			#else
				*(dst+1)	= *(a+1)	* *(b+1)	* f	;
				*(dst+2)	= *(a+2)	* *(b+2)	* f	;
			#endif
			}
		}

extern	CONSTEXPR void	mul_const_v3r(		REAL* dst,										REAL CONST * CONST m,	INT32 nb	);
extern	CONSTEXPR void	mul_const_v3r(		REAL* dst,	REAL CONST * src,					REAL CONST * CONST m,	INT32 nb	);

//	COMBINE
extern	CONSTEXPR	void	add_then_mul_v2r(	REAL* dst,						REAL CONST * a,	REAL CONST * m			);
extern	CONSTEXPR	void	add_then_mul_v2r(	REAL* dst,	REAL CONST * src,	REAL CONST * a,	REAL CONST * m			);
extern	CONSTEXPR	void	add_then_mul_v3r(	REAL* dst,						REAL CONST * a,	REAL CONST * m			);
extern	CONSTEXPR	void	add_then_mul_v3r(	REAL* dst,	REAL CONST * src,	REAL CONST * a,	REAL CONST * m			);
extern	CONSTEXPR	void	add_then_mul_v4r(	REAL* dst,						REAL CONST * a,	REAL CONST * m			);
extern	CONSTEXPR	void	add_then_mul_v4r(	REAL* dst,	REAL CONST * src,	REAL CONST * a,	REAL CONST * m			);
extern	CONSTEXPR			void	add_const_then_mul_const_v3r(
											REAL* dst,						REAL CONST * a,	REAL CONST * m,			INT32 nb	);
extern	CONSTEXPR			void	add_const_then_mul_const_v3r(
											REAL* dst,	REAL CONST * src,	REAL CONST * a,	REAL CONST * m,			INT32 nb	);

template< class T, class S >
		CONSTEXPR void	sub_then_mul_v2(	T* dst,		S CONST* src,		T CONST * a,	T CONST * m	)
		{
		#if	0	//AAA_USE_XNA()
			if ( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
				return	???_xna( dst, b, c );
			else
		#endif
			{
				*dst		= (*src		- *a)		* *m		;
		#if AAA_V_DOIT_WITH_INC()
				*++dst		= (*++src	- *++a)		* *++m		;
		#else
				*(dst+1)	= (*(src+1)	- *(a+1))	* *(m+1)	;
		#endif
			}
		}
template< class T >
		CONSTEXPR void	sub_then_mul_v2( T* dst, T CONST * a, T CONST * m )
		{
		#if	0	//AAA_USE_XNA()
			if ( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
				return	???_xna( dst, b, c );
			else
		#endif
			{
				*dst	= (*dst		- *a)		* *m		;
		#if AAA_V_DOIT_WITH_INC()
				++dst;	*dst	= (*dst		- *++a)		* *++m		;
		#else
				++dst;	*dst	= (*dst		- *(a+1))	* *(m+1)	;
		#endif
			}
		}
template< class T, class S >
		CONSTEXPR	void	sub_then_mul_v3( T* dst, S CONST* src, T CONST * a, T CONST * m )
		{
		#if	0	//AAA_USE_XNA()
			if ( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
				return	???_xna( dst, b, c );
			else
		#endif
			{
				*dst		= (*src		- *a)		* *m		;
		#if AAA_V_DOIT_WITH_INC()
				*++dst		= (*++src	- *++a)		* *++m		;
				*++dst		= (*++src	- *++a)		* *++m		;
		#else
				*(dst+1)	= (*(src+1)	- *(a+1))	* *(m+1)	;
				*(dst+2)	= (*(src+2)	- *(a+2))	* *(m+2)	;
		#endif
			}
		}
template< class T >
		CONSTEXPR	void	sub_then_mul_v3( T* dst, T CONST * a, T CONST * m )
		{
		#if	0	//AAA_USE_XNA()
			if ( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
				return	???_xna( dst, b, c );
			else
		#endif
			{
				*dst	= (*dst		- *a)		* *m		;
		#if AAA_V_DOIT_WITH_INC()
				++dst;	*dst	= (*dst		- *++a)		* *++m		;
				++dst;	*dst	= (*dst		- *++a)		* *++m		;
		#else
				++dst;	*dst	= (*dst		- *(a+1))	* *(m+1)	;
				++dst;	*dst	= (*dst		- *(a+2))	* *(m+2)	;
		#endif
			}
		}
template< class T, class S >
		CONSTEXPR	void	sub_then_mul_v4( T* dst, S CONST* src, T CONST * a, T CONST * m )
		{
		#if	0	//AAA_USE_XNA()
			if ( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
				return	???_xna( dst, b, c );
			else
		#endif
			{
				*dst		= (*src		- *a)		* *m		;
		#if AAA_V_DOIT_WITH_INC()
				*++dst		= (*++src	- *++a)		* *++m		;
				*++dst		= (*++src	- *++a)		* *++m		;
				*++dst		= (*++src	- *++a)		* *++m		;
		#else
				*(dst+1)	= (*(src+1)	- *(a+1))	* *(m+1)	;
				*(dst+2)	= (*(src+2)	- *(a+2))	* *(m+2)	;
				*(dst+3)	= (*(src+3)	- *(a+3))	* *(m+3)	;
		#endif
			}
		}
template< class T >
		CONSTEXPR	void	sub_then_mul_v4( T* dst, T CONST * a, T CONST * m )
		{
		#if	0	//AAA_USE_XNA()
			if ( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
				return	???_xna( dst, b, c );
			else
		#endif
			{
				*dst	= (*dst		- *a)		* *m		;
		#if AAA_V_DOIT_WITH_INC()
				++dst;	*dst	= (*dst		- *++a)		* *++m		;
				++dst;	*dst	= (*dst		- *++a)		* *++m		;
				++dst;	*dst	= (*dst		- *++a)		* *++m		;
		#else
				++dst;	*dst	= (*dst		- *(a+1))	* *(m+1)	;
				++dst;	*dst	= (*dst		- *(a+2))	* *(m+2)	;
				++dst;	*dst	= (*dst		- *(a+3))	* *(m+3)	;
		#endif
			}
		}
	
//	NEG
		//
CONSTEXPR	void	neg_v2r( REAL* dst, REAL CONST * src )
{
	*dst		= -*src;
#if AAA_V_DOIT_WITH_INC()
	*++dst		= -*++src;
#else
	*(dst+1)	= -*(src+1);
#endif
}


template< class T, class S >
	CONSTEXPR	void	neg_v3( T* dst, S CONST * src )
						{
							//if( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
							//{
							//	return	neg_v3_xna( dst, src );
							//}
							//else
							{
								*dst		= -*src;
							#if AAA_V_DOIT_WITH_INC()
								*++dst		= -*++src;
								*++dst		= -*++src;
							#else
								*(dst+1)	= -*(src+1);
								*(dst+2)	= -*(src+2);
							#endif
							}
						}
template< class T >
	CONSTEXPR	void	neg_v3( T* dst )
						{
							//if( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
							//{
							//	return	neg_v3_xna( dst, src );
							//}
							//else
							{
								*dst		= -*dst;
							#if AAA_V_DOIT_WITH_INC()
								++dst;
								*dst		= -*dst;
								++dst;
								*dst		= -*dst;
							#else
								*(dst+1)	= -*(dst+1);
								*(dst+2)	= -*(dst+2);
							#endif
							}
						}

template< class T, class S >
	CONSTEXPR	void	neg_v4( T* dst, S CONST * src )
						{
							//if( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
							//{
							//	return	neg_v4_xna( dst, src );
							//}
							//else
							{
								*dst		= -*src;
							#if AAA_V_DOIT_WITH_INC()
								*++dst		= -*++src;
								*++dst		= -*++src;
								*++dst		= -*++src;
							#else
								*(dst+1)	= -*(src+1);
								*(dst+2)	= -*(src+2);
								*(dst+3)	= -*(src+3);
							#endif
							}
						}
template< class T >
	CONSTEXPR	void	neg_v4( T* dst )
						{
							//if( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
							//{
							//	return	neg_v4_xna( dst, src );
							//}
							//else
							{
								*dst		= -*dst;
							#if AAA_V_DOIT_WITH_INC()
								++dst;
								*dst		= -*dst;
								++dst;
								*dst		= -*dst;
								++dst;
								*dst		= -*dst;
							#else
								*(dst+1)	= -*(dst+1);
								*(dst+2)	= -*(dst+2);
								*(dst+3)	= -*(dst+3);
							#endif
							}
						}


//ABS
extern	CONSTEXPR	void	abs_v3r(					REAL* dst,		REAL CONST * src	);
extern	CONSTEXPR	void	abs_v3r(					REAL* dst						);
extern	CONSTEXPR	void	abs_v2r(					REAL* dst,		REAL CONST * src	);
extern	CONSTEXPR	void	abs_v2r(					REAL* dst						);
//	NORMALIZE
//extern	FINLINE	void	normalize_v3r_low(		REAL *dst, REAL s0, REAL s1, REAL s2, REAL size_squared );
//extern	FINLINE	void	normalize_scale_v3r_low( REAL* dst, REAL s0, REAL s1, REAL s2, REAL size_squared, REAL scale_factor );
extern			  FINLINE	void	normalize_v3r(				REAL* dst,		REAL CONST s0,		REAL CONST s1,		REAL CONST s2	);
extern			  FINLINE	void	normalize_v3r(				REAL* dst		);
extern			  FINLINE	void	normalize_v3r(				REAL* dst,		REAL CONST * src		);
extern			  FINLINE	void	normalize_v3fp32(			FP32* dst,	FP32 CONST * src		);
extern			  FINLINE	void	normalize_v3d(				DOUBLE* dst,	DOUBLE CONST s0,	DOUBLE CONST s1,	DOUBLE CONST s2 );
extern			  FINLINE	void	normalize_v3d(				DOUBLE* dst		);
extern			  FINLINE	void	normalize_v3d(				DOUBLE* dst,	DOUBLE CONST * src );
extern			  FINLINE	void	normalize_scale_v3r(		REAL* dst,		REAL CONST scale_factor		);
extern			  FINLINE	void	normalize_v2(				REAL& a,		REAL& b						);
extern			  FINLINE	void	normalize_v2r(				REAL* dst									);
extern			  FINLINE	void	normalize_scale_v2r(		REAL* dst,		REAL CONST scale_factor		);
extern			  FINLINE	void	limit_v3r(					REAL* dst,		REAL CONST max				);
//	DOT
extern	CONSTEXPR	DOUBLE	dot_v2r(					REAL CONST * a,	REAL CONST * b	);
//extern	CONSTEXPR	DOUBLE	dot_v3r(					REAL CONST * a,	REAL CONST * b	);
#if !AAA_V_DOIT_WITH_INC() || AAA_USE_XNA()
		CONSTEXPR	REAL	dot_v3r( REAL CONST * CONST a, REAL CONST * CONST b )
#else
		CONSTEXPR	REAL	dot_v3r( REAL CONST * a, REAL CONST * b )
#endif
		{
		#if	AAA_USE_XNA()
			if( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
				return	dot_v3_xna( a, b );
		#endif
			{
				REAL tmp =	*a		*	*b		;
		#if AAA_V_DOIT_WITH_INC()
				tmp +=	*++a	*	*++b	;
				tmp +=	*++a	*	*++b	;
		#else
				tmp +=	*(a+1)	*	*(b+1)	;
				tmp +=	*(a+2)	*	*(b+2)	;
		#endif
				return tmp;
			}
		}
//	CROSS
extern	CONSTEXPR	void	cross_v3r(					REAL& x,	REAL& y,	REAL& z,	REAL CONST * a,	REAL CONST * b	);
extern	CONSTEXPR	void	cross_v3r(					REAL* dst,							REAL CONST * a,	REAL CONST * b	);
extern	CONSTEXPR	void	cross_x_v3r(				REAL& y,	REAL& z,				REAL CONST * a	);
extern	CONSTEXPR	void	cross_y_v3r(				REAL& x,	REAL& z,				REAL CONST * a	);
extern	CONSTEXPR	void	cross_z_v3r(				REAL& x,	REAL& y,				REAL CONST * a	);
extern	CONSTEXPR	void	cross_x_v3r(				REAL& x,	REAL& y,	REAL& z,	REAL CONST * a	);
extern	CONSTEXPR	void	cross_y_v3r(				REAL& x,	REAL& y,	REAL& z,	REAL CONST * a	);
extern	CONSTEXPR	void	cross_z_v3r(				REAL& x,	REAL& y,	REAL& z,	REAL CONST * a	);
extern	CONSTEXPR	void	cross_x_v3r(				REAL* dst,							REAL CONST * a	);
extern	CONSTEXPR	void	cross_y_v3r(				REAL* dst,							REAL CONST * a	);
extern	CONSTEXPR	void	cross_z_v3r(				REAL* dst,							REAL CONST * a	);

//	calculates a normalized crossproduct to v1, v2
extern			  FINLINE	void	cross_normalize_v3r(		REAL* dst,		REAL CONST * a, REAL CONST * b );
//	calculates a normalized crossproduct to v1, v2
extern			  FINLINE	void	cross_normalize_x_v3r(		REAL* dst,		REAL CONST * a	);
extern			  FINLINE	void	cross_normalize_y_v3r(		REAL* dst,		REAL CONST * a	);
extern			  FINLINE	void	cross_normalize_z_v3r(		REAL* dst,		REAL CONST * a	);
extern			  FINLINE	void	cross_normalize_scale_v3r(	REAL* dst,		REAL CONST * a,	REAL CONST * b,	REAL CONST scale_factor	);

//
//	MIN MAX
//

template<class T>	
		CONSTEXPR	void	min_v4( T* dst, T CONST * a	) NOEXCEPT
		{
			*dst		=	MIN(	*dst,		*a		);
		#if AAA_V_DOIT_WITH_INC()
			++dst;
			*dst		=	MIN(	*dst,		*++a	);
			++dst;
			*dst		=	MIN(	*dst,		*++a	);
			++dst;
			*dst		=	MIN(	*dst,		*++a	);
		#else
			*(dst+1)	=	MIN(	*(dst+1),	*(a+1)	);
			*(dst+2)	=	MIN(	*(dst+2),	*(a+2)	);
			*(dst+3)	=	MIN(	*(dst+3),	*(a+3)	);
		#endif
		}
template<class T>	
		CONSTEXPR	void	max_v4( T* dst, T CONST * a	) NOEXCEPT	
		{
			*dst		=	MAX(	*dst,		*a		);
		#if AAA_V_DOIT_WITH_INC()
			++dst;
			*dst		=	MAX(	*dst,		*++a	);
			++dst;
			*dst		=	MAX(	*dst,		*++a	);
			++dst;
			*dst		=	MAX(	*dst,		*++a	);
		#else
			*(dst+1)	=	MAX(	*(dst+1),	*(a+1)	);
			*(dst+2)	=	MAX(	*(dst+2),	*(a+2)	);
			*(dst+3)	=	MAX(	*(dst+3),	*(a+3)	);
		#endif
		}
template<class T>	
		CONSTEXPR	void	min_v4( T* dst, T CONST * a, T CONST * b ) NOEXCEPT
		{
			*dst		=	MIN(	*a,			*b		);
		#if AAA_V_DOIT_WITH_INC()
			*++dst		=	MIN(	*++a,		*++b	);
			*++dst		=	MIN(	*++a,		*++b	);
			*++dst		=	MIN(	*++a,		*++b	);
		#else
			*(dst+1)	=	MIN(	*(a+1),		*(b+1)	);
			*(dst+2)	=	MIN(	*(a+2),		*(b+2)	);
			*(dst+3)	=	MIN(	*(a+3),		*(b+3)	);
		#endif
		}
template<class T>	
		CONSTEXPR	void	max_v4( T* dst, T CONST * a, T CONST * b ) NOEXCEPT
		{
			*dst		=	MAX(	*a,			*b		);
		#if AAA_V_DOIT_WITH_INC()
			*++dst		=	MAX(	*++a,		*++b	);
			*++dst		=	MAX(	*++a,		*++b	);
			*++dst		=	MAX(	*++a,		*++b	);
		#else
			*(dst+1)	=	MAX(	*(a+1),		*(b+1)	);
			*(dst+2)	=	MAX(	*(a+2),		*(b+2)	);
			*(dst+3)	=	MAX(	*(a+3),		*(b+3)	);
		#endif
		}

		//
		//	MIN MAX V3
		//
template<class T>	
		CONSTEXPR	void	min_v3( T* dst, T CONST * a	) NOEXCEPT
		{
			*dst		=	MIN(	*dst,		*a		);
		#if AAA_V_DOIT_WITH_INC()
			++dst;
			*dst		=	MIN(	*dst,		*++a	);
			++dst;
			*dst		=	MIN(	*dst,		*++a	);
		#else
			*(dst+1)	=	MIN(	*(dst+1),	*(a+1)	);
			*(dst+2)	=	MIN(	*(dst+2),	*(a+2)	);
		#endif
		}
template<class T>	
		CONSTEXPR	void	max_v3( T* dst, T CONST * a	) NOEXCEPT
		{
			*dst		=	MAX(	*dst,		*a		);
		#if AAA_V_DOIT_WITH_INC()
			++dst;
			*dst		=	MAX(	*dst,		*++a	);
			++dst;
			*dst		=	MAX(	*dst,		*++a	);
		#else
			*(dst+1)	=	MAX(	*(dst+1),	*(a+1)	);
			*(dst+2)	=	MAX(	*(dst+2),	*(a+2)	);
		#endif
		}
template<class T>	
		CONSTEXPR	void	min_v3( T* dst, T CONST * a, T CONST * b ) NOEXCEPT
		{
			*dst		=	MIN(	*a,			*b		);
		#if AAA_V_DOIT_WITH_INC()
			*++dst		=	MIN(	*++a,		*++b	);
			*++dst		=	MIN(	*++a,		*++b	);
		#else
			*(dst+1)	=	MIN(	*(a+1),		*(b+1)	);
			*(dst+2)	=	MIN(	*(a+2),		*(b+2)	);
		#endif
		}
template<class T>	
		CONSTEXPR	void	max_v3( T* dst, T CONST * a, T CONST * b ) NOEXCEPT
		{
			*dst		=	MAX(	*a,			*b		);
		#if AAA_V_DOIT_WITH_INC()
			*++dst		=	MAX(	*++a,		*++b	);
			*++dst		=	MAX(	*++a,		*++b	);
		#else
			*(dst+1)	=	MAX(	*(a+1),		*(b+1)	);
			*(dst+2)	=	MAX(	*(a+2),		*(b+2)	);
		#endif
		}

extern	CONSTEXPR	void	min_max_v3r( REAL* min, REAL* max, REAL CONST * p );
extern				void	min_max_v3r( REAL* min, REAL* max, REAL CONST * point, INT32 CONST nb );


extern	CONSTEXPR	void	build_point_v3r(			REAL* dst,	REAL CONST* o,	REAL CONST* a, REAL CONST* b,					REAL CONST u, REAL CONST v );
extern	CONSTEXPR	void	build_vector_v3r(			REAL* dst,					REAL CONST* a, REAL CONST* b, REAL CONST* c,	REAL CONST u, REAL CONST v, REAL CONST w );
extern	CONSTEXPR	void	build_point_v3r(			REAL* dst,	REAL CONST* o,	REAL CONST* a, REAL CONST* b, REAL CONST* c,	REAL CONST u, REAL CONST v, REAL CONST w );

//	NORMAL
extern	CONSTEXPR	void	compute_normals_v3r(		REAL* dst,	REAL CONST* prev, REAL CONST* cur, REAL CONST* next, INT32 nb, bool CONST b_close );
extern	CONSTEXPR	void	compute_normals_cano_v3r(	REAL* dst,	REAL CONST* prev, REAL CONST* cur, REAL CONST* next, INT32 nb, bool CONST b_close );
extern	CONSTEXPR	void	uv_to_xyz_v3r(				REAL* dst,	REAL CONST* src, INT32 CONST axe );

extern	FINLINE		void	build_normal_vectors_v3r(			REAL CONST* nor, REAL* CONST u, REAL* CONST v );
//maa 2019 this a hack because I can't take a decision
extern	FINLINE		void	build_normal_vectors_using_z_v3r(	REAL CONST* nor, REAL* CONST u, REAL* CONST v );

extern	CONSTEXPR	void	normal_of_4_point_v3r(		REAL* nor,	REAL CONST* a, REAL CONST* b, REAL CONST * c, REAL CONST * d );
extern	CONSTEXPR	void	normal_cano_of_4_point_v3r(	REAL* nor,	REAL CONST* a, REAL CONST* b, REAL CONST * c, REAL CONST * d );

//	BARYCENTER
extern	CONSTEXPR	void	center_v2r(					REAL* dst,	REAL CONST* a, REAL CONST* b );
extern	CONSTEXPR	void	center_v3r(					REAL* dst,	REAL CONST* a, REAL CONST* b );

extern	CONSTEXPR	void	center_v2r(					REAL* dst,	REAL CONST* a, REAL CONST* b, REAL CONST* c );
extern	CONSTEXPR	void	center_v3r(					REAL* dst,	REAL CONST* a, REAL CONST* b, REAL CONST* c );
extern	FINLINE		void	compute_barycenter_v3r(		REAL* dst,	REAL CONST* p, INT32 CONST nb );

// Catmull-Rom Curve calculations
extern	CONSTEXPR	void	catmull_rom_3(				REAL* dst,	REAL CONST* a, REAL CONST* b, REAL CONST* c, REAL CONST* d, FP32 CONST t );
extern	CONSTEXPR	void	catmull_rom_derivative_3(	REAL* dst,	REAL CONST* a, REAL CONST* b, REAL CONST* c, REAL CONST* d, FP32 CONST t );	  
extern	CONSTEXPR	void	catmull_rom_v3(				REAL* dst,	REAL CONST* p, FP32 CONST t );
extern	CONSTEXPR	void	catmull_rom_derivative_v3(	REAL* dst,	REAL CONST* p, FP32 CONST t );
		  
extern	CONSTEXPR	void	catmull_rom_2(				REAL* dst,	REAL CONST* a, REAL CONST* b, REAL CONST* c, REAL CONST* d, FP32 CONST t );
extern	CONSTEXPR	void	catmull_rom_derivative_2(	REAL* dst,	REAL CONST* a, REAL CONST* b, REAL CONST* c, REAL CONST* d, FP32 CONST t );
extern	CONSTEXPR	void	catmull_rom_v2(				REAL* dst,	REAL CONST* p, FP32 CONST t );
extern	CONSTEXPR	void	catmull_rom_derivative_v2(	REAL* dst,	REAL CONST* p, FP32 CONST t );
		  
extern	CONSTEXPR	REAL	catmull_rom_1r(							REAL CONST a,	REAL CONST b, REAL CONST c, REAL CONST d, FP32 CONST t );


//extern	FINLINE	void	bezier_derivative_build_coef( REAL& f1, REAL& f2, REAL& f3, REAL& f4, REAL CONST t )	{}
extern	CONSTEXPR	void	bezier_3r(					REAL* dst,	REAL CONST* a, REAL CONST* b, REAL CONST* c, REAL CONST* d, FP32 CONST t );
extern	CONSTEXPR	void	bezier_2r(					REAL* dst,	REAL CONST* a, REAL CONST* b, REAL CONST* c, REAL CONST* d, REAL CONST t );
//extern	CONSTEXPR	void	bezier_derivative_3r(	REAL* dst,	REAL CONST* a, REAL CONST* b, REAL CONST* c, REAL CONST* d, REAL CONST t )	{}
extern	CONSTEXPR	void	bezier_v3r(					REAL* dst,	REAL CONST* p, FP32 CONST t );

extern	CONSTEXPR	void	bezier_v2r(					REAL* dst,	REAL CONST* p, FP32 CONST t );
extern	CONSTEXPR	REAL	bezier_1r(								REAL CONST a,  REAL CONST b, REAL CONST c, REAL CONST d, REAL CONST t );

