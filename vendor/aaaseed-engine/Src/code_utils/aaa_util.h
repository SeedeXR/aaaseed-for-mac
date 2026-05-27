
#ifdef AAA_AAA_UTIL_H
#error "AAA_UTIL_H included more than once."
#endif
#define AAA_AAA_UTIL_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif


CONSTEXPR	INT32	IMOD( INT32 i, INT32 CONST m ) NOEXCEPT
{
	if( m==0 )
		return 0;
	i = i % m;
	return ( i<0 ) ? m+i : i;
}

/*
//bad
FINLINE	INT32	IMOD( INT32 i, INT32 m)
{

	if( i<0 )
		return (i%m) + m;
	else if( i<m )
		return i;
	else
		return i%m;
}
*/

template<class T>	FINLINE CONSTEXPR	bool	IS_NULL(			T CONST p )	NOEXCEPT {	return ( p == nullptr );		}
template<class T>	FINLINE CONSTEXPR	bool	IS_NOT_NULL(		T CONST p )	NOEXCEPT {	return ( p != nullptr );		}

template<class T>	FINLINE CONSTEXPR	T		ABS(				T CONST a )	NOEXCEPT {	return ( a >= T(0) ) ? a : -a;		}
template<class T>	FINLINE CONSTEXPR	DOUBLE	OVER_ONE(			T CONST v ) NOEXCEPT
{
	return (v != T(0)) ? DOUBLE(1)/v : aaa::BIGGEST<DOUBLE>;
}
template<class T>	FINLINE CONSTEXPR	REAL	OVER_ONE_AS_REAL(	T CONST v ) NOEXCEPT
{
	return (v != T(0)) ? REAL(1)/v : aaa::BIGGEST<REAL>;
}
template<class T>	FINLINE CONSTEXPR	FP32	OVER_ONE_AS_FP32(	T CONST v ) NOEXCEPT
{
	return (v != T(0)) ? FP32(1)/v : aaa::BIGGEST<FP32>;
}
template<class T>			CONSTEXPR	T		DIV(				T CONST a, T CONST divider ) NOEXCEPT
{
	return (divider != T(0)) ? T(DOUBLE(a)/divider) : ( a==0. ? T(1) : aaa::BIGGEST<T> );
}

template<class T>	FINLINE CONSTEXPR	void	SWAP( T*& a, T*& b ) NOEXCEPT
{
	T* tmp = a;
	a = b;
	b = tmp;
}
template<class T>	FINLINE CONSTEXPR	void	SWAP( T&  a, T&  b ) NOEXCEPT
{
	T tmp = a;
	a = b;
	b = tmp;
}

// now just for good measure undefine MIN and MAX if they are defined
#ifdef MIN
#	undef MIN
#endif

#ifdef MAX 
#	undef MAX 
#endif

template<typename T>				CONSTEXPR	T		MAX(	T CONST  a,	T CONST  b ) NOEXCEPT	{	return	(a > b) ? a : b;		}
template<typename T1, typename T2>	CONSTEXPR	T1		MAX(	T1 CONST a, T2 CONST b ) NOEXCEPT	{	return	MAX( a, T1(b) );		}
template<typename T>				CONSTEXPR	T		MAX0(	T CONST  a )			 NOEXCEPT	{	return	(a > T(0)) ? a : T(0);	}
template<typename T>				CONSTEXPR	T		MAX1(	T CONST  a )			 NOEXCEPT	{	return	(a > T(1)) ? a : T(1);	}

template<typename T>				CONSTEXPR	T		MIN(	T CONST  a,	T CONST  b ) NOEXCEPT	{	return	(a < b) ? a : b;		}
template<typename T1, typename T2>	CONSTEXPR	T1		MIN(	T1 CONST a,	T2 CONST b ) NOEXCEPT	{	return	MIN( a, T1(b) );		}
template<typename T>				CONSTEXPR	T		MIN0(	T CONST  a )			 NOEXCEPT	{	return	(a < T(0)) ? a : T(0);	}
template<typename T>				CONSTEXPR	T		MIN1(	T CONST  a )			 NOEXCEPT	{	return	(a < T(1)) ? a : T(1);	}

namespace aaa
{
	template<typename T>			CONSTEXPR	T		MIN(	T CONST  a,	T CONST  b,	T CONST c )	NOEXCEPT {	return	(a < b) ? ((a < c) ? a : c) : ((b < c) ? b : c);		}
	template<typename T>			CONSTEXPR	T		MAX(	T CONST  a,	T CONST  b,	T CONST c )	NOEXCEPT {	return	(a > b) ? ((a > c) ? a : c) : ((b > c) ? b : c);		}
};

template<class T>
CONSTEXPR	void	UPDATE_MIN_MAX( T& min, T& max, T CONST a ) NOEXCEPT
{
	if( a < min )
		min = a;
	else if( max < a )
		max = a;
}
template<class T>
CONSTEXPR	void	UPDATE_MIN_MAX_SAFE( T& min, T& max, T CONST a ) NOEXCEPT
{
	//we do the two tests in case max <= min at init
	if( a < min )
		min = a;
	if( max < a )
		max = a;
}
template<class T>
CONSTEXPR	void	FORCE_MIN_MAX( T& min, T& max ) NOEXCEPT
{
	if( max < min )	
		SWAP( min, max );
}
template<class T>
CONSTEXPR	void	ASSIGN_MIN_MAX( T& min, T& max, T CONST a, T CONST b ) NOEXCEPT
{
	if( a < b )	
	{	
		min = a;
		max = b;
	}
	else
	{
		min = b;
		max = a;
	}
}

//	CLAMP
//
template< class T >				CONSTEXPR	void	CLAMP_REF(		T& a,		T CONST min,	T CONST max		) NOEXCEPT
{
	if( a < min )
		a = min;
	else if( a > max )
		a = max;
}
//careful here casting before test fuck iy (signed / unsigned I guess) 
template< class T, class S >	CONSTEXPR	void	CLAMP_REF(		T& a,		S CONST min,	S CONST max		) NOEXCEPT
{
	if( a < min )
		a = T(min);
	else if( a > max )
		a = T(max);
}
template< class T >
CONSTEXPR	T		CLAMP(			T CONST a,	T CONST min,	T CONST max		) NOEXCEPT
{
	if( a <= min )
		return min;
	if( max <= a )
		return max;
	return a;
}
//careful here casting before test fuck iy (signed / unsigned I guess) 
template< class T, class S >
CONSTEXPR	T		CLAMP(			T CONST a,	S CONST min,	S CONST max		) NOEXCEPT
{
	if( a <= min )
		return static_cast<T>(min);
	if( max <= a )
		return static_cast<T>(max);
	return a;
}
template< class T >	CONSTEXPR	void	CLAMP_REF_01(	T& a		)									NOEXCEPT	{	CLAMP_REF( a, T(0), T(1) );		}
template<class T>	CONSTEXPR	T		CLAMP_01(		T CONST a	)									NOEXCEPT	{	return CLAMP( a, T(0), T(1) );	}

template<class T>	CONSTEXPR	void	ORDER( T &a, T &b )												NOEXCEPT
{
	if( a > b )
		SWAP( a, b );
}

template<class T>	CONSTEXPR	bool	INSIDE_MIN_MAX(			T CONST in, T CONST a, T CONST b )		NOEXCEPT	{	return (a  <= in)		&& (in <=  b);		}
template<class S,class T>
					CONSTEXPR	bool	INSIDE_MIN_MAX(			S CONST in, T CONST a, T CONST b )		NOEXCEPT	{	return (a  <= S(in))	&& (in <= S(b));	}
template<class T>	CONSTEXPR	bool	INSIDE_MIN_MAX_STRICT(	T CONST in, T CONST a, T CONST b )		NOEXCEPT	{	return (a  <  in)		&& (in <   b);		}
template<class T>	CONSTEXPR	bool	OUTSIDE_MIN_MAX(		T CONST in, T CONST a, T CONST b )		NOEXCEPT	{	return (in <   a)		|| (b  <  in);		}
													
template<class T>	CONSTEXPR	bool	INSIDE_01(				T CONST in )							NOEXCEPT	{	return INSIDE_MIN_MAX( in, T(0), T(1) );	}
template<class T>	CONSTEXPR	bool	INSIDE(					T CONST in, T CONST a, T CONST b )		NOEXCEPT	{
																														if( a < b )	return INSIDE_MIN_MAX( in, a, b );
																														else		return INSIDE_MIN_MAX( in, b, a );
																													}
template<class T>	CONSTEXPR	bool	INSIDE_STRICT(			T CONST in, T CONST a, T CONST b )		NOEXCEPT	{
																														if( a < b )	return INSIDE_MIN_MAX_STRICT( in, a, b );
																														else		return INSIDE_MIN_MAX_STRICT( in, b, a );
																													}
template<class T>	CONSTEXPR	bool	INSIDE_ABS(				T CONST in, T CONST a )					NOEXCEPT	{	return (in <= a) && (-a <= in);	}	//	order to optimize

template<class T>	CONSTEXPR	bool	OUTSIDE_01(				T CONST in )							NOEXCEPT	{	return OUTSIDE_MIN_MAX( in, T(0), T(1) );	}
template<class T>	CONSTEXPR	bool	OUTSIDE(				T CONST in, T CONST a, T CONST b )		NOEXCEPT	{
																														if( a < b )	return OUTSIDE_MIN_MAX( in, a, b );
																														else		return OUTSIDE_MIN_MAX( in, b, a );
																													}
template<class T>	CONSTEXPR	bool	OUTSIDE_ABS(			T CONST in, T CONST a )					NOEXCEPT	{	return (a < in) || (in < -a);	}	//	order to optimize

template<class T>	CONSTEXPR	bool	IS_INDEX_VALID(			T CONST index, T CONST nb )				NOEXCEPT	{	return index >=0 && index<nb;	}

template<class T>	CONSTEXPR	T		interpolate(			T CONST a,	T CONST b,	T CONST t )		NOEXCEPT	{	return a + t * (b - a) ;		}
template<class T,class S>
					CONSTEXPR	T		interpolate(			T CONST a, T CONST b, S CONST t )		NOEXCEPT	{	return a + T( t * (b - a) ) ;	}
template<class T>	CONSTEXPR	FP32	normalize(				T CONST a, T CONST min, T CONST max )	NOEXCEPT
{
	T d = max - min;
	return (d==T(0)) ? aaa::BIGGEST<FP32> : FP32(a - min) / FP32(d);
}

template<class T>
CONSTEXPR T	get_max_of_array( T CONST * p_list, INT32 nb ) NOEXCEPT
{
	T max = *p_list;
	--nb;
	for( ; nb>0; --nb )
	{
		if( *++p_list > max )
			max = *p_list;
	}
	return max;
}

template<class T>
CONSTEXPR T get_min_of_array( T CONST * p_list, INT32 nb ) NOEXCEPT
{
	T min = *p_list;
	--nb;
	for( ; nb>0; --nb )
	{
		if( *++p_list < min )
			min = *p_list;
	}
	return min;
}

template<class T>
CONSTEXPR	void get_min_max_of_array( T& min, T& max, T CONST * p_list, INT32 nb ) NOEXCEPT
{
	min = max = *p_list;
	--nb;
	for( ; nb>0; --nb )
	{
		UPDATE_MIN_MAX_SAFE( min, max, *++p_list );
	}
}

/*
template	<class T>
FINLINE	void	add_to_array( T* p_list, INT32 nb, REAL offset)
{
	--p_list;
	for( ; nb>0 ; --nb )
		*++p_list += offset;
}
*/

//////
////	
//
//#define	I_FLOOR(a)	( (INT32)(a) - ( ((a)<0.0)?1:0 ) )
template<class T>
CONSTEXPR	INT32	I_FLOOR( T CONST f ) NOEXCEPT
{
#if AAA_DEBUG()
	if( f < T(aaa::LOWEST<INT32>) || f > T(aaa::BIGGEST<INT32>) )
		debug_break( "f out of INT32 range -- cast to INT32 is UB" );
#endif
	INT32	i = (INT32)f;
	if( f-i == 0 )
		return i;
	if( f < 0. )
		return i - 1;
	return i;
}
template<class T>
CONSTEXPR	INT32	I_CEIL( T CONST f ) NOEXCEPT
{
#if AAA_DEBUG()
	if( f < T(aaa::LOWEST<INT32>) || f > T(aaa::BIGGEST<INT32>) )
		debug_break( "f out of INT32 range -- cast to INT32 is UB" );
#endif
	INT32	i = (INT32)f;
	if( f-i == 0 )
		return i;
	if( f >= 0. )
		return i + 1;
	return i;
}
template<class T>			CONSTEXPR	REAL	F_FLOOR( T CONST f )							 NOEXCEPT	{	return (REAL) I_FLOOR( f );		}
template<class T>			CONSTEXPR	REAL	F_CEIL( T CONST f )								 NOEXCEPT	{	return (REAL) I_CEIL( f );		}

							CONSTEXPR	DOUBLE	ROUND_FLOOR(	DOUBLE CONST f, DOUBLE CONST d ) NOEXCEPT	{	return F_FLOOR( f / d ) * d;	}
template<class T>			CONSTEXPR	REAL	ROUND_FLOOR(	T CONST f,		T CONST d )		 NOEXCEPT	{	return REAL( ROUND_FLOOR( DOUBLE(f), DOUBLE(d) ) );	}
template<class T>			CONSTEXPR	REAL	ROUND_CEIL(		T CONST f,		T CONST d )		 NOEXCEPT	{	return F_CEIL( f / d ) * d;		}
template<class T>			CONSTEXPR	REAL	CEIL(			T CONST f )						 NOEXCEPT	{	return (REAL) ceil( f );		}

template<class T>
CONSTEXPR	T		FMOD( T val, T CONST mod ) NOEXCEPT
{
	T CONST ratio = val / mod;
#if AAA_DEBUG()
	if( ratio < T(aaa::LOWEST<INT32>) || ratio > T(aaa::BIGGEST<INT32>) )
		debug_break( "val/mod out of INT32 range -- cast to INT32 is UB" );
#endif
	INT32	n = INT32( ratio );
	val -= T( n * mod );
	if( val < 0 )
		val += T( mod );
	return val;
}
template<class T>
CONSTEXPR	T		FMOD( T val ) NOEXCEPT
{
#if AAA_DEBUG()
	if( val < T(aaa::LOWEST<INT32>) || val > T(aaa::BIGGEST<INT32>) )
		debug_break( "val out of INT32 range -- cast to INT32 is UB" );
#endif
	INT32 n = INT32( val );
	val -= T(n);
	if( val < 0 )
		val += T(1);
	return val;
}
template<class T>
CONSTEXPR	T		FSAW( T val ) NOEXCEPT
{
	val = FMOD( val, T(2) );
	if( val > 1. )
		val = 2 - val;
	return val;
}

CONSTEXPR	INT32	I_ROUND_CEIL( INT32 CONST f, INT32 CONST d ) NOEXCEPT
{ 
	if( d == 0 )
		return f;
	INT32 remainder = ABS(f) % d;
	if( remainder == 0 )
		return f;
	if( f < 0 )
		return -(ABS(f) - remainder);
	return f + d - remainder;
}

template<class T, class U>
CONSTEXPR	T		fwrap_low_private(	T CONST val, U CONST min, U CONST max )	NOEXCEPT	{	return FMOD( val-min, T(max-min)) + min;	}
template<class T, class U>
CONSTEXPR	T		FWRAP(				T CONST val, U CONST min, U CONST max ) NOEXCEPT
{
	if( max > min )
		return fwrap_low_private( val, min, max );
	if( max == min )
		return min;
	return fwrap_low_private( val, max, min );	//todo should we flip ?
}
template<class T>
CONSTEXPR	T		FWRAP(		T CONST val, T CONST max )						NOEXCEPT	{	return FMOD( val, max );	}
template<class T>
CONSTEXPR	T		FWRAP(		T CONST val )									NOEXCEPT	{	return FMOD( val );	}
template<class T>
CONSTEXPR	T		WRAP_01(	T CONST val ) NOEXCEPT
{
	if( INSIDE_MIN_MAX( val, T(0), T(1) ) )
		return val;
	return FMOD( val );
}
CONSTEXPR	INT32	WRAP_ID( INT32 CONST val, INT32 CONST min, INT32 CONST max ) NOEXCEPT
{
	if( INSIDE_MIN_MAX( val, min, max ) )
		return val;
	INT32 tmp = max-min+1;
	return tmp ? FMOD( val-min, tmp ) + min : min;
}
CONSTEXPR	INT32	WRAP_ID(	INT32 CONST val, INT32 CONST max ) NOEXCEPT	{	return WRAP_ID( val, 1, max );			}
CONSTEXPR	INT32	WRAP_INDEX( INT32 CONST val, INT32 CONST max ) NOEXCEPT	{	return WRAP_ID( val, 0, max );			}

template<class T, class U>
CONSTEXPR	T		bounce_low_private( T val, U CONST min, U CONST max ) NOEXCEPT
{
	U range = max - min;
	val -= min;
	val = FMOD( val, T(range*2) );
	if( val > range )
		val = range*2 - val;
	return val + min;
}

template<class T, class U>
CONSTEXPR	T		BOUNCE( T CONST val, U CONST min, U CONST max ) NOEXCEPT
{
	if( max > min )
		return bounce_low_private( val, min, max );
	if( max == min )
		return min;
	return bounce_low_private( val, max, min );	//todo should we flip ?
}

