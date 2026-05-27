#include "math/fn1d/fn1d_fbm.h"
#include "math/rand.h"
#include "infrastructure/param/param_declare.h"
#include "infrastructure/param/param_focus.h"
#include <math.h>
#include "aaa_mem.h"


c_fn1d_fm::c_fn1d_fm()
{
	_h = 0.;
	set_sample_nb( 1024 );
	set_h();
	set_min_max( -.5, .5 );
	set_frequency( 1.);
}

void	c_fn1d_fm::set_h( REAL h_in )
{
	if ( h_in != _h )
	{
		_h = h_in;
		_b_to_update = true;
	}
}

/*
//	is the original code
REAL	c_pertub::get(register	REAL f_in)
{
register	REAL	*p_f;
register	INT32	i_floor;

	f_in *= f_value_max;
	f_in = FMOD(f_in,f_value_max);

	i_floor = I_FLOOR(f_in);
	p_f = _samples + i_floor;
	f_in -= i_floor;

	return *p_f*( 1. - f_in) + *(p_f+1)*f_in;
}
*/

namespace	n_fm
{
	CONSTEXPR INT32	BASE_PARAM_NB	= 4;
	CONSTEXPR INT32	GROUP_PARAM_NB	= 0;
	CONSTEXPR INT32	PARAM_NB_MAX	=	BASE_PARAM_NB
									+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_REAL(		H fractal,	98., 0,	0, 1 )
		PARAM_DEF_GAIN(		Gain	)
		PARAM_DEF_BIAS(		Bias )
		PARAM_DEF_INT32(	Seed,		8, 7,	0, PARAM_MAX_UINT32	)
	};
}

//c_param_def	*c_fn1d_fm::set_focus()	//par
//{
////todo
///*
//	n_fm::param[0]._dum = (void *)&_h;
//	n_fm::param[1]._dum = (void *)&_gain;
//	n_fm::param[2]._dum = (void *)&_bias;
//	n_fm::param[3]._dum = (void *)&_seed;
//*/
////todo last param should be name()
////par	focus_list_set( n_fm::param, FM_PARAM_NB_MAX, "Fractal Noise" );
//	return n_fm::param;
//}

//	One dimensional Fractal motion via successive random additions
//	0 < _h < 1,	D = 2 - H fractal; dimension
void	c_fn1d_fm_add::compute()
{
	INT32	p2;
	INT32	ordre;
	INT32	d,dd;
	REAL	*p_f;
	REAL	delta;
	REAL	f_max, f_min;
	c_rand_gauss_slick	*rand;
	REAL	factor;
	INT32	nb;
 
//	we deal only with power of 2
	p2 =  2;
	ordre = 1;
	nb = get_sample_nb();
	while( p2 < nb )
	{
		p2 *= 2;
		++ordre;
	}
	
	
//	we gonna fill an array so we better have it
	_samples = (REAL *)REALLOC_ALIGNED( _samples, sizeof(REAL) * (p2+1) );

	if( _samples )
	{
		ERR_PRINT_STRING( "%s() can't allocate %d samples", __FUNCTION__, p2+1 );
		set_sample_nb(0);
		return;
	}

	set_sample_nb( p2 );
//	we need a random generator
	rand = new c_rand_gauss_slick( _seed, _gain, _bias );

//	some good fractal cooking
	*_samples		= 0.;	
	*(_samples+p2)	= 0.; // we want it to loop 	
	dd = p2;
	d = dd/2;
	factor = (REAL)pow( .5, double(_h ));
	delta = 1.;	//	any value because of the scaling
	for( INT32 l = 1; l <= ordre; ++l )
	{
		// delta = sigma * pow( .5,l*_h ) * sqrt(.5) * sqrt( 1 - pow( 2., 2.*_h-2.) );
		//	became because the constant factor vanish with the scaling
		delta *= factor;
//#if	AAA_DEBUG()
//		DBG_PRINT_STRING( " delta = %f", (DOUBLE)delta);
//#endif
		INT32	i;
		for( i = d; i <= p2 - d; i += dd )
			*(_samples + i) = (*(_samples + i - d ) + *(_samples + i + d )) * REAL(.5);
		for( i = 0; i < p2; i += d )
			*(_samples+i) += delta * rand->get_fp32();
		*(_samples+i) = *_samples;	// we want it to loop 
		dd = d;
		d /= 2;
	}
//	we don't need any more a random generator
	delete rand;

//	lets find the min and max
	f_min = f_max = *_samples;
	p_f = _samples+1;
	for( INT32 l = p2; l > 0; --l )
	{
		if ( *p_f > f_max )
			f_max = *p_f;
		else if ( *p_f < f_min )
			f_min = *p_f;
		++p_f;
	}

//	and scale the whole thing in [min/2.,max/2.]
//	using y = ax + b
//todo the * .5 should be in the param itself
	REAL a,b;
	a = REAL( ( _max - _min ) * .5 / ( f_max - f_min ) );
	b = REAL( _min * .5 - a * f_min );
//#if	AAA_DEBUG()
//		DBG_PRINT_STRING( " PERTUB a = %f,\t b = %f", (DOUBLE)a, (DOUBLE)b);
//#endif

	p_f = _samples;
	for( INT32 l = p2; l >= 0; --l )
	{
		*p_f = *p_f * a + b;
#if	DEBUGALL
		fprintf(stderr, "l : %d -> %f\n", l, (DOUBLE)*p_f);
#endif
		++p_f;
	}

	_b_to_update = false;
}


