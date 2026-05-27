#ifdef AAA_V_BASE_H
#error "V_BASE_H included more than once."
#endif
#define AAA_V_BASE_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif
#ifndef AAA_AAA_MEM_H
#	include "aaa_mem.h"
#endif
#include <concepts>	// for std::same_as, used in the if constexpr(...) dispatch of cpy_v2/v3/v4/v16 below

class	c_vector_lib
{
public:

	enum	VECTOR_LIB_TYPE : INT32
	{
		VECTOR_LIB_NO = 0,
		VECTOR_LIB_XNA,
		VECTOR_LIB_MAX_NB,
	};
	static	C_PCHAR_C	vector_lib_str[ VECTOR_LIB_MAX_NB ];
	static	INT32		s_vector_lib;
};


#if 1
template <typename T>	class c_point_xy	{ public: T x; T y;			};
//	used by AAASeed in ancient pieces of code		  
template <typename T>	class c_point_xyz	{ public: T x; T y; T z;	};
typedef  c_point_xyz<REAL> c_point_xyz_real;
#endif


//	base vectors
CONSTEXPR	FP32	zero_v4fp32			[4] = {	0,	0,	0,	0 };
CONSTEXPR	FP32	one_v4fp32			[4] = {	1,	1,	1,	1 };
											  				  
CONSTEXPR	FP32	unit_x_v4fp32		[4] = {	1,	0,	0,	0 };
CONSTEXPR	FP32	unit_xw_v4fp32		[4] = {	1,	0,	0,	1 };
											  				  
CONSTEXPR	FP32	unit_y_v4fp32		[4] = {	0,	1,	0,	0 };
CONSTEXPR	FP32	unit_yw_v4fp32		[4] = {	0,	1,	0,	1 };
											  				  
CONSTEXPR	FP32	unit_z_v4fp32		[4] = {	0,	0,	1,	0 };
CONSTEXPR	FP32	unit_zw_v4fp32		[4] = {	0,	0,	1,	1 };
											  				  
CONSTEXPR	FP32	unit_x_neg_v4fp32	[4] = {	-1,	0,	0,	0 };
CONSTEXPR	FP32	unit_y_neg_v4fp32	[4] = {	0,	-1,	0,	0 };
CONSTEXPR	FP32	unit_z_neg_v4fp32	[4] = {	0,	0,	-1, 0 };
											  				  
CONSTEXPR	FP32	unit_xy_v4fp32		[4] = {	1,	1,	0,	0 };
CONSTEXPR	FP32	unit_yz_v4fp32		[4] = {	0,	1,	1,	0 };
CONSTEXPR	FP32	unit_xz_v4fp32		[4] = {	1,	0,	1,	0 };



#define	AAA_V_DOIT_WITH_INC()	0


//	TEST
template<class T>
		CONSTEXPR	FINLINE bool	is_not_null_v2(	T CONST * CONST	a	)	{	return  *a != 0.			||	*(a+1) != 0.;	}
template<class T>
		CONSTEXPR	FINLINE bool	is_not_null_v3(	T CONST * CONST	a	)	{	return is_not_null_v2(a)	||	*(a+2) != 0. ;	}
template<class T>
		CONSTEXPR	FINLINE bool	is_not_null_v4(	T CONST * CONST	a	)	{	return is_not_null_v3(a)	||	*(a+3) != 0. ;	}

template<class T>
		CONSTEXPR	FINLINE bool	is_equal_v2(	T CONST * CONST a, T CONST * CONST b )
		{
		#if	0	//AAA_USE_XNA()
			if ( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
				return	is_equal_v2_xna( a, b );
			else
		#endif
			{
				return( *a == *b
					&&	*(a+1) == *(b+1)
					);
			}
		}
template<class T>
		CONSTEXPR	FINLINE bool	is_equal_v3( T CONST * CONST a, T CONST * CONST b )
		{
		#if 0	//AAA_USE_XNA()
			if ( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
				return	is_equal_v3_xna( a, b );
		#endif
			{
				return( *a == *b
					&&	*(a+1) == *(b+1)
					&&	*(a+2) == *(b+2)
					);
			}
		}
template<class T>
		CONSTEXPR	FINLINE bool	is_equal_v4( T CONST * CONST a, T CONST * CONST b )
		{
		#if	0	//AAA_USE_XNA()
			if ( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
				return	is_equal_v4_xna( a, b );
		#endif
			{
				return( *a == *b
					&&	*(a+1) == *(b+1)
					&&	*(a+2) == *(b+2)
					&&	*(a+3) == *(b+3)
					);
			}
		}

template<class T>
		CONSTEXPR	FINLINE bool	is_diff_v2(		T CONST * CONST a,		T CONST * CONST b		)
							{
								return( *a != *b
									||	*(a+1) != *(b+1)
									);
							}
template<class T>
		CONSTEXPR	FINLINE bool	is_diff_v3(		T CONST * CONST a,		T CONST * CONST b		)
							{
								//if ( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
								//{
								//	return	is_diff_v3_xna( a, b );
								//}
								//else
								{
									return( *a != *b
										||	*(a+1) != *(b+1)
										||	*(a+2) != *(b+2)
										);
								}
							}
template<class T>
		CONSTEXPR	FINLINE bool	is_diff_v4(		T CONST * CONST a,		T CONST * CONST b		)
							{
								//if ( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
								//{
								//	return	is_diff_v4_xna( a, b );
								//}
								//else
								{
									return( *a != *b
										||	*(a+1) != *(b+1)
										||	*(a+2) != *(b+2)
										||	*(a+3) != *(b+3)
										);
								}
							}
template<class T>
		CONSTEXPR	FINLINE bool	is_diff_v5(		T CONST * CONST a,		T CONST * CONST b	)
							{
								return( *a != *b
									||	*(a+1) != *(b+1)
									||	*(a+2) != *(b+2)
									||	*(a+3) != *(b+3)
									||	*(a+4) != *(b+4)
									);
							}

template<class T>
		CONSTEXPR	FINLINE bool	is_equal_v2( T CONST * CONST a, T CONST f )
		{
		#if	0	//AAA_USE_XNA()
			if ( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
				return	is_equal_v2_xna( a, f );
			else
		#endif
				return *a == f && *(a+1) == f ;
		}
template<class T>
		CONSTEXPR	FINLINE bool	is_equal_v3( T CONST * CONST a, T CONST f )
		{
		#if	0	//AAA_USE_XNA()
			if ( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
				return	is_equal_v3_xna( a, f );
		#endif
				return *a == f && *(a+1) == f && *(a+2) == f;

		}
template<class T>
		CONSTEXPR	FINLINE bool	is_equal_v4( T CONST * CONST a, T CONST f )
		{
		#if	0	//AAA_USE_XNA()
			if ( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
				return	is_equal_v4_xna( a, f );
		#endif
				return *a == f && *(a+1) == f && *(a+2) == f && *(a+3) == f;
		}

template<class T>	CONSTEXPR	FINLINE bool	is_diff_v2( T CONST * CONST a, T CONST f )	{	return !is_equal_v2( a, f );	}
template<class T>	CONSTEXPR	FINLINE bool	is_diff_v3( T CONST * CONST a, T CONST f )	{	return !is_equal_v3( a, f );	}
template<class T>	CONSTEXPR	FINLINE bool	is_diff_v4( T CONST * CONST a, T CONST f )	{	return !is_equal_v4( a, f );	}

template<class T>	CONSTEXPR	FINLINE bool	is_all_one_v2(		T CONST * CONST a ) {	return	is_equal_v2( a, T(1) );	}
template<class T>	CONSTEXPR	FINLINE bool	is_all_one_v3(		T CONST * CONST a ) {	return	is_equal_v3( a, T(1) );	}
template<class T>	CONSTEXPR	FINLINE bool	is_all_one_v4(		T CONST * CONST a ) {	return	is_equal_v4( a, T(1) );	}


template<class T>
		CONSTEXPR	bool	is_inside_v2(					T CONST * vec,	T CONST * min,	T CONST * max	)
		{
			return INSIDE( *vec, *min, *max )
				&& INSIDE( *(vec+1), *(min+1), *(max+1) )
				;
		}
template<class T>
		CONSTEXPR	bool	is_inside_v3(					T CONST * vec,	T CONST * min,	T CONST * max	)
		{
			return INSIDE( *vec, *min, *max )
				&& INSIDE( *(vec+1), *(min+1), *(max+1) )
				&& INSIDE( *(vec+2), *(min+2), *(max+2) )
				;
		}
template<class T>
		CONSTEXPR	bool	is_inside_v4(					T CONST * vec,	T CONST * min,	T CONST * max	)
		{
			return INSIDE( *vec, *min, *max )
				&& INSIDE( *(vec+1), *(min+1), *(max+1) )
				&& INSIDE( *(vec+2), *(min+2), *(max+2) )
				&& INSIDE( *(vec+3), *(min+3), *(max+3) )
				;
		}

template<class T>
		CONSTEXPR	bool	is_inside_min_max_v2(			T CONST * vec,	T CONST * min,	T CONST * max	) NOEXCEPT
		{
			return INSIDE_MIN_MAX( *vec, *min, *max )
				&& INSIDE_MIN_MAX( *(vec+1), *(min+1), *(max+1) )
				;
		}
template<class T>
		CONSTEXPR	bool	is_inside_min_max_v3(			T CONST * vec,	T CONST * min,	T CONST * max	) NOEXCEPT
		{
			return INSIDE_MIN_MAX( *vec, *min, *max )
				&& INSIDE_MIN_MAX( *(vec+1), *(min+1), *(max+1) )
				&& INSIDE_MIN_MAX( *(vec+2), *(min+2), *(max+2) )
				;
		}
template<class T>
		CONSTEXPR	bool	is_inside_min_max_v4(			T CONST * vec,	T CONST * min,	T CONST * max	) NOEXCEPT
		{
			return INSIDE_MIN_MAX( *vec, *min, *max )
				&& INSIDE_MIN_MAX( *(vec+1), *(min+1), *(max+1) )
				&& INSIDE_MIN_MAX( *(vec+2), *(min+2), *(max+2) )
				&& INSIDE_MIN_MAX( *(vec+3), *(min+3), *(max+3) )
				;
		}

template<class T>
		CONSTEXPR	bool	is_inside_min_max_strict_v2(	T CONST * vec,	T CONST * min,	T CONST * max	) NOEXCEPT
		{
			return	INSIDE_MIN_MAX_STRICT( *(vec+0), *(min+0), *(max+0) )
				&&	INSIDE_MIN_MAX_STRICT( *(vec+1), *(min+1), *(max+1) )
				;
		}
template<class T>
		CONSTEXPR	bool	is_inside_min_max_strict_v3(	T CONST * vec,	T CONST * min,	T CONST * max	) NOEXCEPT
		{
			return	INSIDE_MIN_MAX_STRICT( *(vec+0), *(min+0), *(max+0) )
				&&	INSIDE_MIN_MAX_STRICT( *(vec+1), *(min+1), *(max+1) )
				&&	INSIDE_MIN_MAX_STRICT( *(vec+2), *(min+2), *(max+2) )
				;
		}
template<class T>
		CONSTEXPR	bool	is_inside_min_max_strict_v4(	T CONST * vec,	T CONST * min,	T CONST * max	) NOEXCEPT
		{
			return	INSIDE_MIN_MAX_STRICT( *(vec+0), *(min+0), *(max+0) )
				&&	INSIDE_MIN_MAX_STRICT( *(vec+1), *(min+1), *(max+1) )
				&&	INSIDE_MIN_MAX_STRICT( *(vec+2), *(min+2), *(max+2) )
				&&	INSIDE_MIN_MAX_STRICT( *(vec+3), *(min+3), *(max+3) )
				;
		}

//	COPY
// Single template that dispatches at compile time:
//   - Same type (T == S): Release uses memcpy as an intrinsic (/O2 /Oi inlines it to a single movq
//     for 8 bytes, movq+movd for 12, movups for 16, etc.), slightly fewer ops than N individual movss.
//     Debug expands inline stores instead, because a real CRT memcpy call per invocation would kill
//     realtime in per-pixel paths.
//   - Cross type (eg, FP16 → FP32): per-element T(...) cast does the conversion.
// Same-type explicit call cpy_v2<FP32, FP32>(p, q) correctly takes the fast path since the dispatch
// is inside one function body, not across overloads.
template< class T, class S >
	FINLINE	void	cpy_v2( T* dst, S CONST * CONST src ) NOEXCEPT
					{
						if constexpr( std::same_as<T, S> )
						{
						#if AAA_DEBUG()
							dst[0] = src[0];
							dst[1] = src[1];
						#else
							memcpy( dst, src, sizeof(T)*2 );
						#endif
						}
						else
						{
							dst[0] = T( src[0] );
							dst[1] = T( src[1] );
						}
					}
// See cpy_v2 for the dispatch rationale.
template< class T, class S >
	FINLINE	void	cpy_v3( T* dst, S CONST * CONST src ) NOEXCEPT
					{
						if constexpr( std::same_as<T, S> )
						{
						#if AAA_DEBUG()
							dst[0] = src[0];
							dst[1] = src[1];
							dst[2] = src[2];
						#else
							memcpy( dst, src, sizeof(T)*3 );
						#endif
						}
						else
						{
							dst[0] = T( src[0] );
							dst[1] = T( src[1] );
							dst[2] = T( src[2] );
						}
					}
template< class T, class S >
	FINLINE void	cpy_swap_v3( T* dst, S CONST * CONST src ) NOEXCEPT
					{
					//#if	AAA_USE_XNA()
					//	if( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
					//		return	cpy_v3_xna( dst, src );
					//#endif
						dst[0]	= T( src[2]	);
						dst[1]	= T( src[1]	);
						dst[2]	= T( src[0]	);
					}

// See cpy_v2 for the dispatch rationale.
template< class T, class S >
	FINLINE	void	cpy_v4( T* dst, S CONST * CONST src ) NOEXCEPT
					{
						if constexpr( std::same_as<T, S> )
						{
						#if AAA_DEBUG()
							dst[0] = src[0];
							dst[1] = src[1];
							dst[2] = src[2];
							dst[3] = src[3];
						#else
							memcpy( dst, src, sizeof(T)*4 );
						#endif
						}
						else
						{
							dst[0] = T( src[0] );
							dst[1] = T( src[1] );
							dst[2] = T( src[2] );
							dst[3] = T( src[3] );
						}
					}

template< class T, class S >
	CONSTEXPR	void	cpy_vn( T* dst, S CONST * src, INT32 CONST nb )
						{
							//if( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
							//{
							//	return	cpy_v4_xna( dst, src );
							//}
							//else
							{
							#if AAA_V_DOIT_WITH_INC()
								for( ; nb>0; --nb )
									*dst++ = *src++;
							#else
								for( INT32 i=0; i<nb; ++i )
									dst[i] = src[i];
							#endif
							}
						}
template< class T >
	CONSTEXPR	void	cpy_vn( T* dst, T CONST * src, INT32 CONST nb )
						{
							//if( c_vector_lib::s_vector_lib == c_vector_lib::VECTOR_LIB_XNA )
							//{
							//	return	cpy_v4_xna( dst, src );
							//}
							//else
							MEMCPY( dst, src, nb*sizeof(T), __FUNCTION__ );
						}
template< class T >
#if	 1
	CONSTEXPR	void	cpy_v3n( T*& dst, T CONST * CONST src, INT32 nb )
						{
							MEMCPY( dst, src, nb*sizeof(T)*3, __FUNCTION__ );
							dst += nb*3;
						}
#else
	CONSTEXPR	void	cpy_v3n( T*& dst, T CONST * src, INT32 nb )
						{
							for( ; nb > 0 ; --nb, dst+=3, src+=3 )
								cpy_v3( dst, src );
						
#endif
template< class T, class S >
	FINLINE bool		check_and_cpy_vn( T* CONST dst, S CONST * CONST src, INT32 CONST nb ) NOEXCEPT
						{
							for( INT32 i=0; i<nb; ++i )
							{
								if( dst[i] != src[i] )
								{	//copy this one and the rest potentially different
									cpy_vn( dst+i, src+i, nb-i );
									return true;
								}
							}
							return false;
						}
// See cpy_v2 for the dispatch rationale.
template< class T, class S >
	FINLINE void	cpy_v16( T* dst, S CONST * CONST src ) NOEXCEPT
					{
						if constexpr( std::same_as<T, S> )
						{
						#if AAA_DEBUG()
							dst[ 0] = src[ 0];	dst[ 1] = src[ 1];	dst[ 2] = src[ 2];	dst[ 3] = src[ 3];
							dst[ 4] = src[ 4];	dst[ 5] = src[ 5];	dst[ 6] = src[ 6];	dst[ 7] = src[ 7];
							dst[ 8] = src[ 8];	dst[ 9] = src[ 9];	dst[10] = src[10];	dst[11] = src[11];
							dst[12] = src[12];	dst[13] = src[13];	dst[14] = src[14];	dst[15] = src[15];
						#else
							memcpy( dst, src, sizeof(T)*16 );
						#endif
						}
						else
						{
							dst[ 0] = T( src[ 0] );	dst[ 1] = T( src[ 1] );	dst[ 2] = T( src[ 2] );	dst[ 3] = T( src[ 3] );
							dst[ 4] = T( src[ 4] );	dst[ 5] = T( src[ 5] );	dst[ 6] = T( src[ 6] );	dst[ 7] = T( src[ 7] );
							dst[ 8] = T( src[ 8] );	dst[ 9] = T( src[ 9] );	dst[10] = T( src[10] );	dst[11] = T( src[11] );
							dst[12] = T( src[12] );	dst[13] = T( src[13] );	dst[14] = T( src[14] );	dst[15] = T( src[15] );
						}
					}

	
//COPY

CONSTEXPR	void	cpy_shift1_v3r( REAL* dst, REAL CONST * src )
{
	*dst		= *(src+2);
#if AAA_V_DOIT_WITH_INC()
	*++dst		= *src;
	*++dst		= *(src+1);
#else
	*(dst+1)	= *src;
	*(dst+2)	= *(src+1);
#endif
}

CONSTEXPR	void	cpy_shift2_v3r( REAL* dst, REAL CONST * src )
{
	*dst		= *(src+1);
#if AAA_V_DOIT_WITH_INC()
	*++dst		= *(src+2);
	*++dst		= *src;
#else
	*(dst+1)	= *(src+2);
	*(dst+2)	= *src;
#endif
}


extern	CONSTEXPR		void	cpy_from_with_stride_v3r(	REAL* dst,	REAL CONST * src,	INT32 stride,		INT32 nb	);
extern	CONSTEXPR		void	cpy_to_with_stride_v3r(		REAL* dst,	INT32 stride,		REAL CONST * src,	INT32 nb	);
extern	FINLINE			REAL*	cpy_alternate_v3(			REAL* dst,	REAL CONST * a,		REAL CONST * b,		INT32 nb	);
extern	FINLINE			REAL*	cpy_alternate_v2(			REAL* dst,	REAL CONST * a,		REAL CONST * b,		INT32 nb	);

//	CLEAR
//
//FINLINE	void	clear_v3r( REAL* dst, CONST size_t nb )		{	MEMCLEAR( dst, nb * sizeof(REAL)*3 );	}
//FINLINE	void	clear_v4r( REAL* dst, CONST size_t nb )		{	MEMCLEAR( dst, nb * sizeof(REAL)*4 );	}

FINLINE	void	clear_vr( REAL* dst,	CONST size_t nb )	{	MEMCLEAR( dst, nb * sizeof(REAL) );	}
FINLINE	void	clear_vf( FP32* dst,	CONST size_t nb )	{	MEMCLEAR( dst, nb * sizeof(FP32) );	}
FINLINE	void	clear_vd( DOUBLE* dst,	CONST size_t nb )	{	MEMCLEAR( dst, nb * sizeof(DOUBLE) );	}

//	SET
//
template< class T, class S >
		CONSTEXPR	void	set_vn(	T* dst, S CONST val, INT32 nb ) NOEXCEPT
						{	//todo opt
							T CONST tmp = T(val);
							--dst;
							for( ; nb > 0; --nb )
								*++dst = tmp;
						}
template< class T, class S >
		CONSTEXPR	void	set_v2( T* dst, S CONST a, S CONST b ) NOEXCEPT
						{
							dst[0] = T(a);
							dst[1] = T(b);
						}
template< class T, class S >
		CONSTEXPR	void	set_v2( T* dst, S CONST a )	NOEXCEPT
						{
							T CONST tmp = T(a);
							dst[0] = tmp;
							dst[1] = tmp;
						}

template< class T, class S1, class S2, class S3 >
		CONSTEXPR	void	set_v3( T* dst, S1 CONST a, S2 CONST b, S3 CONST c ) NOEXCEPT
						{
							dst[0] = T(a);
							dst[1] = T(b);
							dst[2] = T(c);
						}
template< class T, class S >
		CONSTEXPR	void	set_v3( T* dst, S CONST a ) NOEXCEPT
						{
							T CONST tmp = T(a);
							dst[0] = tmp;
							dst[1] = tmp;
							dst[2] = tmp;
						}
template< class T, class S1, class S2, class S3, class S4 >
		CONSTEXPR	void	set_v4( T* dst, S1 CONST a, S2 CONST b, S3 CONST c, S4 CONST d ) NOEXCEPT
						{
							dst[0] = T(a);
							dst[1] = T(b);
							dst[2] = T(c);
							dst[3] = T(d);
						}
template< class T, class S >
		CONSTEXPR	void	set_v4( T* dst, S CONST a ) NOEXCEPT
						{
							T CONST tmp = T(a);
							dst[0] = tmp;
							dst[1] = tmp;
							dst[2] = tmp;
							dst[3] = tmp;
						}
template<class T>
		CONSTEXPR	void	clear_v2(	T* dst ) NOEXCEPT
						{
							set_v2( dst, T(0) );
						}
template<class T>
		CONSTEXPR	void	clear_v3(	T* dst ) NOEXCEPT
						{
							set_v3( dst, T(0) );
						}
template<class T>
		CONSTEXPR	void	clear_v4(	T* dst ) NOEXCEPT
						{
							set_v4( dst, T(0) );
						}

template< class T, class S >
		CONSTEXPR	void	swap_v3( T* a, S* b ) NOEXCEPT
						{
							SWAP( a[0],	b[0] );
							SWAP( a[1],	b[1] );
							SWAP( a[2],	b[2] );
						}

		//	AXE
		CONSTEXPR	void	axe_build_index_vert( INT32 &i_u, INT32 &i_v, INT32 CONST i_axe ) NOEXCEPT
						{
							switch( i_axe )
							{
							case 0:		i_u = 2;	i_v = 1;	break;
							case 1:		i_u = 2;	i_v = 0;	break;
							default:
//#if AAA_DEBUG()
//										ERR_PRINT_STRING( "%s() with i_axe %d: should be 0,1,2]", __FUNCTION__, i_axe );
//#endif
							case 2:		i_u = 0;	i_v = 1;	break;
							
							}
						}

		CONSTEXPR	void	axe_build_index( INT32 &i_u, INT32 &i_v, INT32 CONST i_axe ) NOEXCEPT
						{
							switch( i_axe )
							{
							case 0:		i_u = 1;	i_v = 2;	break;
							case 1:		i_u = 2;	i_v = 0;	break;
							default:
//#if AAA_DEBUG()
//										ERR_PRINT_STRING( "%s() with i_axe %d: should be 0,1,2]", __FUNCTION__, i_axe );
//#endif
							case 2:		i_u = 0;	i_v = 1;	break;
							}
						}