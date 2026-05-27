#include "aaa_def.h"

#include "rand.h"
#include "infrastructure/param/param_declare.h"
#include <math.h>
#include "err.h"
#include "gol/gol_color.h"
#include "gol/gol_draw.h"

CONST	REAL	c_rand::factor_for_float = REAL(1./aaa::BIGGEST<UINT32>);
//	value used by the lin generator
static	CONST	INT32	LIN_MULT  = 69069;


//////
////	measure the periodicity of the rand generator
//		and check that the optimization run fine
void	c_rand::test()
{
	c_rand_lin			a_rand;
	c_rand_lin_portable	p_rand;
	UINT32				i;
	UINT32				v;

//	goto test_gauss;

//	find period of lin generator
	for( i=100; i>0; --i )
	{
		a_rand.get_uint32();
	}
	v = a_rand.get_uint32();
	i = 1;
	while( v != a_rand.get_uint32() )
	{
		++i;
		if ( (i & 0Xffffff) == 0 )
			DBG_PRINT_STRING("%ld * 16M rand without looping", i >> 24);
	}
	DBG_PRINT_STRING("period of the rand generator is %ld",i);

//	check if the implementation is ok for lin in place of lin_portable
	a_rand.set_seed();
	p_rand.set_seed();
	for( ;i!=0; --i )
	{
		if (  a_rand.get_uint32() != p_rand.get_uint32())
		{
			break;
		}
		if ( (i & 0Xffffff) == 0 )
			DBG_PRINT_STRING("%ld * 16M rand without divergence", i >> 24);
	}
	if( i == 0 )
		DBG_PRINT_STRING("Ok to use c_rand_lin in place of c_rand_lin_portable");
	else
	{
		ERR_PRINT_STRING("!!! divergence between c_rand_lin_portable and c_rand_lin");
		ERR_PRINT_STRING("!!! you should use the portable method on this implementation");
		ERR_PRINT_STRING("!!! and rewrite the gauss generator");
	}
//	test_gauss:
//	find period of gauss generator
	c_rand_gauss	g_rand;
	UINT32			vb;
	for ( i=100; i>0; --i )
	{
		g_rand.get_uint32();
	}
	v = g_rand.get_uint32();
	vb = g_rand.get_uint32();
	i = 1;
	//	we use two values because we can get the same value for two different _seed
	//	in the lin case the value and the _seed are the same
	while( v != g_rand.get_uint32() || vb != g_rand.get_uint32() )
	{
		++i;
		if ( (i & 0Xffffff) == 0 )
			DBG_PRINT_STRING("%ld * 16 M rand gauss without looping",i >> 24);
	}
	DBG_PRINT_STRING("period of the rand gauss generator is %ld",i);
}

//////
////	WHITE NOISE
//			lin_portable is a clean but slow implementation
//			if lin work the same you should use it because it's faster
FINLINE	INT32	LO16( INT32	a)	{ return a & 0xffff; }
FINLINE	INT32	HI16( INT32	a)	{ return (a>>16) & 0xffff; }
static	CONST	INT32	MLO = LO16(LIN_MULT);
static	CONST	INT32	MHI = HI16(LIN_MULT);

UINT32	c_rand_lin_portable::get_uint32()
{
	UINT32	hi,lo;

	lo = _seed+1;
	hi = HI16(lo);
	lo = LO16(lo);

	hi = lo * MHI + hi * MLO;
	lo = lo * MLO;

	hi += HI16(lo);
	_seed = lo = ((LO16(hi)<<16) + LO16(lo)) & aaa::BIGGEST<UINT32>;

	return lo;
}

//bug this is bad
//	it flip from odd to even
//	you can see patterns
UINT32	c_rand_lin::get_uint32()
{
	++_count;
//	_seed = ((_seed+1)*LIN_MULT) & aaa::BIGGEST<UINT32>;

// interesting pattern (simili period 36 or 72 ) 
//	_seed = ((_seed+1)*LIN_MULT) * ((_seed+123)*29*31*37);

	_seed = ( ( _seed + 1 ) * LIN_MULT ) * ( _seed * ( _seed + LIN_MULT ) * 29 * 31 * 37 );

	_seed = _seed & aaa::BIGGEST<UINT32>;

// following ok but loop when reseed itself
//	s = s<<13^s;
//	s = (s*(s*s*15731 + 789221) + 1376312589) & aaa::BIGGEST<UINT32>;

//	pas mal mais lourd
//	srand( _seed );
//	_seed = rand() + (rand()<<15) + (rand()<<30);

	return _seed;
}

UINT32	c_rand_license::get_uint32()
{
// following ok but loop when reseed itself
	_seed = _seed<<13^_seed;
	_seed = ( _seed * ( _seed * _seed * 15731 + 789221 ) + 1376312589) & aaa::BIGGEST<UINT32>;
	return _seed;
}

//////
////	GAUSSIAN DONE WITH ADDITIONS
//
UINT32	c_rand_gauss::get_uint32()
{
	UINT32 s;
	UINT32 t;

	s = ((_seed+1)*LIN_MULT) & aaa::BIGGEST<UINT32>;
	t = s>>2;

	s = ((s+1)*LIN_MULT) & aaa::BIGGEST<UINT32>;
	t += s>>2;

	s = ((s+1)*LIN_MULT) & aaa::BIGGEST<UINT32>;
	t += s>>2;

	_seed = s = ((s+1)*LIN_MULT) & aaa::BIGGEST<UINT32>;
	t += s>>2;
	
	return t;
}

//////
////	GAUSSIAN WITH GAIN THEN BIAS
//
c_rand_gauss_slick::c_rand_gauss_slick( UINT32 seed_in, REAL gain_in, REAL bias_in)
{
	set_seed( seed_in);
	set_gain( gain_in);
	set_bias( bias_in);
}

REAL	c_rand_gauss_slick::get_fp32_01()
{
	REAL f;

	f = c_rand_lin::get_fp32_01();
	// c_rand_exp
	gain_slick_with_factor( f, _gain_factor );
	bias_slick_with_factor( f, _bias_factor );

	return f;
}

/*
#define	RAND_GAUSS_SLICK_PARAM_NB_MAX	2
c_param_def	rand_gauss_slick_param[RAND_GAUSS_SLICK_PARAM_NB_MAX] =
{
	PARAM_DEF_GAIN(	Gain	)
	PARAM_DEF_BIAS(	Bias	)
};

//clean

c_param_def*	c_rand_gauss_slick::set_focus()
{
	rand_gauss_slick_param[0].pt = (void *)&gain_factor;
	rand_gauss_slick_param[1].pt = (void *)&bias_factor;
	focus_list_set( rand_gauss_slick_param, RAND_GAUSS_SLICK_PARAM_NB_MAX, "Rand" );
	return rand_gauss_slick_param;
}
*/

//////
////	EXPERIMENTAL
//
c_rand_exp::c_rand_exp()
{
	_gain = .5;
	_bias = .5;
	_type = 0;
}

REAL	c_rand_exp::get_fp32_01()
{
	REAL f;

	f = c_rand::get_fp32_01();
	// c_rand_exp

	if ( _type == 0 )
	{
		gain_slick_if_needed( f, _gain );
		bias_slick_if_needed( f, _bias );
	}
	else if ( _type ==  1 )
	{
		gain_perlin( f, _gain );
		bias_perlin( f, _bias );
	}

	return f;
}

CONSTEXPR INT32 RAND_EXP_PARAM_NB_MAX	=	3;
CONST c_param_def param[RAND_EXP_PARAM_NB_MAX] =
{
	PARAM_DEF_GAIN(		Gain	)
	PARAM_DEF_BIAS(		Bias	)
	PARAM_DEF_INT32(	_type,	0, 0,	-0, 1					)	
};

//clean
/*
c_param_def*	c_rand_exp::set_focus()
{
	rand_exp_param[0].pt = (void *)&gain;
	rand_exp_param[1].pt = (void *)&bias;
	rand_exp_param[2].pt = (void *)&_type;
//todo last param should be name()
	focus_list_set( rand_exp_param, RAND_EXP_PARAM_NB_MAX, "Rand" );
	return rand_exp_param;
}
*/

#include "gol/gol.h"
#define	STEP_NB	100
void	c_rand::draw_in_rect(REAL x, REAL y, REAL w, REAL h)
{
	REAL	sx;
	REAL	dx;
	//REAL	f;
	REAL	tmp;
	//REAL	gain = *(REAL *)(rand_exp_param[0].pt);
	//REAL	bias = *(REAL *)(rand_exp_param[1].pt);
	REAL	gain = REAL(.2);
	REAL	bias = REAL(.2);

	sx = x + w;
	REAL df = REAL( 1. / STEP_NB );
	dx = w / STEP_NB;

	GOL::color_red();
	x = sx;
	GOL::begin(GL_LINE_STRIP);
		for( REAL f = 1.; f > .0; f -= df )
		{
			tmp = f;
			gain_slick( tmp, gain );
			GOL::vertex2( x, y + h * tmp );
			x -= dx;
		}
	GOL::end();

	GOL::color_green();
	x = sx;
	GOL::begin(GL_LINE_STRIP);
		for( REAL f = 1.; f > .0; f -= df )
		{
			tmp = f;
			bias_slick( tmp, bias );
			GOL::vertex2( x, y + h * tmp );
			x -= dx;
		}
	GOL::end();

	GOL::color_blue();
	x = sx;
	GOL::begin(GL_LINE_STRIP);
		for( REAL f = 1.; f > .0; f -= df)
		{
			tmp = f;
			gain_slick( tmp, gain);
			bias_slick( tmp, bias);
			GOL::vertex2( x, y + h * tmp );	
			x -= dx;
		}
	GOL::end();

	GOL::color3( .5, 0, 0 );
	x = sx;
	GOL::begin(GL_LINE_STRIP);
		for( REAL f = 1.; f > .0; f -= df )
		{
			tmp = f;
			gain_perlin( tmp, gain );
			GOL::vertex2( x, y + h * tmp );
			x -= dx;
		}
	GOL::end();

	GOL::color3( 0, .5, 0 );
	x = sx;
	GOL::begin(GL_LINE_STRIP);
		for( REAL f = 1.; f > .0; f -= df )
		{
			tmp = f;
			bias_perlin( tmp, bias );
			GOL::vertex2( x, y + h * tmp );
			x -= dx;
		}
	GOL::end();

	GOL::color3( .0, .0, .5 );
	x = sx;
	GOL::begin(GL_LINE_STRIP);
		for( REAL f = 1.; f > .0; f -= df )
		{
			tmp = f;
			gain_perlin( tmp, gain );
			bias_perlin( tmp, bias );
			GOL::vertex2( x, y + h * tmp );	
			x -= dx;
		}
	GOL::end();
}

