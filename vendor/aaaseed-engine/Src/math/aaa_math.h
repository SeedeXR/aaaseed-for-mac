
#ifdef AAA_AAA_MATH_H
#error "AAA_MATH_H included more than once."
#endif
#define AAA_AAA_MATH_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif
#ifndef _INC_MATH
#	include <math.h>
#endif

FINLINE	REAL	SQRT( REAL CONST a ) NOEXCEPT
{
#if AAA_REAL_IS_DOUBLE()
	return sqrt( a);
#else
	return sqrtf( a);
#endif
}
FINLINE	REAL	SQRT_R( REAL CONST a ) NOEXCEPT
{
#if AAA_REAL_IS_DOUBLE()
	return sqrt( a);
#else
	return sqrtf( a);
#endif
}

#if !AAA_REAL_IS_DOUBLE()
FINLINE	DOUBLE	SQRT( DOUBLE CONST a ) NOEXCEPT
{
	return sqrt( a);
}
#endif

FINLINE	DOUBLE	LOG( DOUBLE CONST a ) NOEXCEPT
{
	return log( a );
/*#if AAA_REAL_IS_DOUBLE()
	return log( a);
#else
	return logf( a);
#endif
*/
}
FINLINE	DOUBLE	LOG10( DOUBLE CONST a ) NOEXCEPT
{
	return log10( a );
/*#if AAA_REAL_IS_DOUBLE()
	return log10( a);
#else
	return log10f( a);
#endif
*/
}
FINLINE	DOUBLE	EXP( DOUBLE CONST a ) NOEXCEPT
{
	return exp( a );
/*#if AAA_REAL_IS_DOUBLE()
	return exp( a);
#else
	return expf( a);
#endif
*/
}
FINLINE	REAL	POW_R( REAL CONST a, REAL CONST b ) NOEXCEPT
{
#if AAA_REAL_IS_DOUBLE()
	return pow( a, b);
#else
	return powf( a, b);
#endif
}
FINLINE	DOUBLE	POW_D( DOUBLE CONST a, DOUBLE CONST b ) NOEXCEPT
{
	return pow( a, b );
}
FINLINE	DOUBLE	POW( REAL CONST a, REAL CONST b ) NOEXCEPT
{
	return pow( a, b );
/*#if AAA_REAL_IS_DOUBLE()
	return pow( a, b);
#else
	return powf( a, b);
#endif
*/
}

//////
////	Power of 2
//
CONSTEXPR INT32	POW2( INT32 CONST a ) NOEXCEPT	{ return 1 << a; }
CONSTEXPR INT32	POW2_EQUAL_OR_SUP( INT32 CONST a ) NOEXCEPT
{
	//todo check if this opt, use table ?
	INT32	n = a>255 ? 8 : 0;
	INT32	p = POW2( n );
	while( a > p )
		p <<= 1;
	return p;
}

CONSTEXPR bool	INT_IS_POW2( INT32 CONST a ) NOEXCEPT	{ return (a&(~a+1)) == a; }
//extern	INT32	p2_ceil( INT32	in);
//extern	INT32	p2_floor( INT32	in);

////	Trigo
CONSTEXPR DOUBLE	CONST	PI_TIME_2			= 6.28318530717958647692;
#ifndef	PI	//	due to integration with MSA
CONSTEXPR DOUBLE	CONST	PI					= PI_TIME_2 / DOUBLE(2.);
#endif
CONSTEXPR DOUBLE	CONST	PI_BY_2				= PI / DOUBLE(2.);
CONSTEXPR DOUBLE	CONST	PI_TIME_2_OVER_1	= DOUBLE(1.) / PI_TIME_2;
CONSTEXPR DOUBLE	CONST	DEG_OVER_1			= DOUBLE(1.) / DOUBLE(360);

FINLINE	DOUBLE	ATAN( REAL CONST a ) NOEXCEPT
{
	return atan( a );
/*#if AAA_REAL_IS_DOUBLE()
	return atan( a);
#else
	return atanf( a);
#endif
*/
}
FINLINE	DOUBLE	ATAN2( REAL CONST x, REAL CONST y ) NOEXCEPT
{
	return atan2( y, x );
/*#if AAA_REAL_IS_DOUBLE()
	return atan2( a, b);
#else
	return atan2f( a, b);
#endif
*/
}
FINLINE	DOUBLE	ATAN2_TURN( REAL CONST x, REAL CONST y ) NOEXCEPT
{
	return ATAN2( x, y ) * PI_TIME_2_OVER_1;
}

class	c_math
{
private:
	CONSTEXPR static	INT32	CONST BITS_NB_TABLE_SIZE	=	1<<16;
	CONSTEXPR static	INT32	CONST SIN_TABLE_SIZE		=	1024 * 256	;
	CONSTEXPR static	INT32	CONST TAN_TABLE_SIZE		=	4096 * 8	;
	CONSTEXPR static	INT32	CONST OVER1_TABLE_SIZE		=	4096 * 4	;

	static	UINT8	bits_nb		[	BITS_NB_TABLE_SIZE	];

	static	REAL	sin_table	[	SIN_TABLE_SIZE		];
	static	REAL	tan_table	[	TAN_TABLE_SIZE		];
	static	REAL	over1_table	[	OVER1_TABLE_SIZE	];

public:
	c_math();
//	~c_math();

	FINLINE	static	DOUBLE	get_frac(		DOUBLE CONST a ) NOEXCEPT
	{
		double i, f;
		f = modf( a, &i );
		return  f>=0 ? f : f+1. ;
	}

	//todoqq should interpolate
				FINLINE	static REAL		sin_table_direct_get(	REAL CONST a )	 NOEXCEPT	{ return sin_table[ INT32(a*SIN_TABLE_SIZE) ];				}
				FINLINE	static REAL		base_fn(				DOUBLE CONST a ) NOEXCEPT	{ return sin_table[ INT32(get_frac(a)*SIN_TABLE_SIZE) ];	}

	//trigo
	CONSTEXPR	FINLINE static DOUBLE	rad_to_deg(		DOUBLE CONST a ) NOEXCEPT	{ return a * (180.0f/PI);			}
	CONSTEXPR	FINLINE static DOUBLE	rad_to_turn(	DOUBLE CONST a ) NOEXCEPT	{ return a * PI_TIME_2_OVER_1;		}

	CONSTEXPR	FINLINE static DOUBLE	deg_to_rad(		DOUBLE CONST a ) NOEXCEPT	{ return a * (PI/180.0f);			}
	CONSTEXPR	FINLINE static DOUBLE	deg_to_turn(	DOUBLE CONST a ) NOEXCEPT	{ return a * (1./360.0f);			}

	CONSTEXPR	FINLINE static DOUBLE	turn_to_rad(	DOUBLE CONST a ) NOEXCEPT	{ return a * PI_TIME_2;				}
	CONSTEXPR	FINLINE static DOUBLE	turn_to_deg(	DOUBLE CONST a ) NOEXCEPT	{ return a * (360.0f);				}

				FINLINE	static REAL		sin_turn(		DOUBLE CONST a ) NOEXCEPT	{ return base_fn( a		);			}
				FINLINE	static REAL		cos_turn(		DOUBLE CONST a ) NOEXCEPT	{ return sin_turn( a+.25 );			}

				FINLINE	static REAL		sin_rad(		DOUBLE CONST a ) NOEXCEPT	{ return sin_turn( rad_to_turn(a) ); }
				FINLINE	static REAL		cos_rad(		DOUBLE CONST a ) NOEXCEPT	{ return cos_turn( rad_to_turn(a) ); }
				FINLINE	static REAL		sin_deg(		DOUBLE CONST a ) NOEXCEPT	{ return sin_turn( deg_to_turn(a) ); }
				FINLINE	static REAL		cos_deg(		DOUBLE CONST a ) NOEXCEPT	{ return cos_turn( deg_to_turn(a) ); }

	FINLINE	static void	get_sin_cos( REAL& sin_out, REAL& cos_out, DOUBLE CONST a ) NOEXCEPT
	{
		INT32	index = INT32( get_frac(a) * SIN_TABLE_SIZE );
		sin_out = sin_table[ index ];
		index = (index + (SIN_TABLE_SIZE>>2)) & (SIN_TABLE_SIZE-1);
		cos_out = sin_table[ index ];
	}
				
//todo use a table here too
				FINLINE	static DOUBLE	asin_rad(		DOUBLE CONST a ) NOEXCEPT	{ return asin( a ); }
				FINLINE	static DOUBLE	acos_rad(		DOUBLE CONST a ) NOEXCEPT	{ return acos( a ); }
				
	/*
	FINLINE	REAL	base_fn( DOUBLE CONST a )		CONST { return sin_table[ INT32(a) & (SIN_TABLE_SIZE-1) ]; }

	FINLINE	REAL	sin_rad( DOUBLE CONST a )		CONST { return base_fn( a					* (SIN_TABLE_SIZE*PI_TIME_2_OVER_1) ); }
	FINLINE	REAL	cos_rad( DOUBLE CONST a )		CONST { return base_fn((a+REAL(PI_BY_2)		* (SIN_TABLE_SIZE*PI_TIME_2_OVER_1) ); }
	FINLINE	REAL	sin_deg( DOUBLE CONST a )		CONST { return base_fn( a					* (SIN_TABLE_SIZE*DEG_OVER_1) ); }
	FINLINE	REAL	cos_deg( DOUBLE CONST a )		CONST { return base_fn((a+REAL(90.))		* (SIN_TABLE_SIZE*DEG_OVER_1) ); }
	FINLINE	REAL	sin_turn( DOUBLE CONST a )		CONST { return base_fn( a					* (SIN_TABLE_SIZE) ); }
	FINLINE	REAL	cos_turn( DOUBLE CONST a )		CONST { return base_fn((a+REAL(.25))		* (SIN_TABLE_SIZE) ); }
	//FINLINE	REAL	tan_turn( DOUBLE CONST a )	CONST { return base_fn( a					* (TAN_TABLE_SIZE) ); }//todo
	*/
//inverse
	//todoqq should interpolate
				FINLINE	static	REAL	over1(				REAL CONST a )		NOEXCEPT	{ return over1_table[ ((INT32)((a)*OVER1_TABLE_SIZE)) & (OVER1_TABLE_SIZE-1) ]; }

	CONSTEXPR	FINLINE	static	INT32	count_bits_uint16(	UINT16 CONST value ) NOEXCEPT	{ return bits_nb[0xffff&value]; }
	CONSTEXPR	FINLINE	static	INT32	count_bits_uint32(	UINT32 CONST value ) NOEXCEPT	{ return count_bits_uint16(value) + count_bits_uint16(value>>16); }
	CONSTEXPR	FINLINE	static	INT32	count_bits_uint64(	UINT64 CONST value ) NOEXCEPT	{ return count_bits_uint32(value & 0xffffffff ) + count_bits_uint32(value>>32); }

			static AAA_ERR	test();

			static INT32	convert_hilbert_xy_to_d( INT32 n, INT32 x, INT32 y )			NOEXCEPT;
			static void		convert_hilbert_d_to_xy( INT32 n, INT32 d, INT32& x, INT32& y ) NOEXCEPT;
};


CONSTEXPR	FINLINE DOUBLE	RAD_TO_DEG(			DOUBLE CONST a ) NOEXCEPT	{ return c_math::rad_to_deg(	a ); }
CONSTEXPR	FINLINE DOUBLE	RAD_TO_TURN(		DOUBLE CONST a ) NOEXCEPT	{ return c_math::rad_to_turn(	a ); }

CONSTEXPR	FINLINE DOUBLE	DEG_TO_RAD(			DOUBLE CONST a ) NOEXCEPT	{ return c_math::deg_to_rad(	a ); }
CONSTEXPR	FINLINE DOUBLE	DEG_TO_TURN(		DOUBLE CONST a ) NOEXCEPT	{ return c_math::deg_to_turn(	a ); }

CONSTEXPR	FINLINE DOUBLE	TURN_TO_RAD(		DOUBLE CONST a ) NOEXCEPT	{ return c_math::turn_to_rad(	a ); }
CONSTEXPR	FINLINE DOUBLE	TURN_TO_DEG(		DOUBLE CONST a ) NOEXCEPT	{ return c_math::turn_to_deg(	a ); }

			FINLINE	REAL	SIN_RAD(			DOUBLE CONST a ) NOEXCEPT	{ return c_math::sin_rad(		a ); }
			FINLINE	REAL	COS_RAD(			DOUBLE CONST a ) NOEXCEPT	{ return c_math::cos_rad(		a ); }
			FINLINE	REAL	SIN_DEG(			DOUBLE CONST a ) NOEXCEPT	{ return c_math::sin_deg(		a ); }
			FINLINE	REAL	COS_DEG(			DOUBLE CONST a ) NOEXCEPT	{ return c_math::cos_deg(		a ); }
			FINLINE	REAL	SIN_TURN(			DOUBLE CONST a ) NOEXCEPT	{ return c_math::sin_turn(		a ); }
			FINLINE	REAL	COS_TURN(			DOUBLE CONST a ) NOEXCEPT	{ return c_math::cos_turn(		a ); }
		
			FINLINE	DOUBLE	ASIN_RAD(			DOUBLE CONST a ) NOEXCEPT	{ return				c_math::asin_rad(		a ); }
			FINLINE	DOUBLE	ACOS_RAD(			DOUBLE CONST a ) NOEXCEPT	{ return				c_math::acos_rad(		a ); }
			FINLINE	DOUBLE	ASIN_DEG(			DOUBLE CONST a ) NOEXCEPT	{ return RAD_TO_DEG(	c_math::asin_rad(		a )	); }
			FINLINE	DOUBLE	ACOS_DEG(			DOUBLE CONST a ) NOEXCEPT	{ return RAD_TO_DEG(	c_math::acos_rad(		a )	); }
			FINLINE	DOUBLE	ASIN_TURN(			DOUBLE CONST a ) NOEXCEPT	{ return RAD_TO_TURN(	c_math::asin_rad(		a )	); }
			FINLINE	DOUBLE	ACOS_TURN(			DOUBLE CONST a ) NOEXCEPT	{ return RAD_TO_TURN(	c_math::acos_rad(		a )	); }

//FINLINE	REAL	TAN_TURN( DOUBLE a)	{ return c_math.tan_turn(a); }
			FINLINE	void	GET_SIN_COS_TURN(			REAL& s,	REAL& c,	DOUBLE CONST a ) NOEXCEPT		{	c_math::get_sin_cos( s, c, a ); }
			FINLINE	void	GET_SIN_COS_DEG(			REAL& s,	REAL& c,	DOUBLE CONST a ) NOEXCEPT		{	GET_SIN_COS_TURN( s, c, DEG_TO_TURN(a) ); }
			FINLINE	void	GET_SIN_COS_TURN_DOUBLE(	DOUBLE& s,	DOUBLE& c,	DOUBLE CONST a ) NOEXCEPT
			{
				DOUBLE ph = a * PI_TIME_2;
				c = cos(ph);
				s = sin(ph);
			}

CONSTEXPR	FINLINE	INT32	COUNT_BITS_UINT16(	UINT16 CONST a ) NOEXCEPT	{ return c_math::count_bits_uint16( a ); }
CONSTEXPR	FINLINE	INT32	COUNT_BITS_UINT32(	UINT32 CONST a ) NOEXCEPT	{ return c_math::count_bits_uint32( a ); }
CONSTEXPR	FINLINE	INT32	COUNT_BITS_UINT64(	UINT64 CONST a ) NOEXCEPT	{ return c_math::count_bits_uint64( a ); }

//////
////	Over 1
//
			FINLINE	REAL	OVER1(REAL a)			{	return c_math::over1(a);	}

//#define	IMOD(i,m)	( ((i)%m)+(((i)<0)?m:0) )

//////
////	Misc stuff
//


