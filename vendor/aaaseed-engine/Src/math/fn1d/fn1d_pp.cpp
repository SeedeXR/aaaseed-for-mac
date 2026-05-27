#include "math/fn1d/fn1d_pp.h"
#include "err.h"
#include "math/rand.h"
#include "infrastructure/param/param_declare.h"
#include "aaa_util.h"

#define	PHASE_PERTUB_STEP_NB	16

c_fn1d_phase_pertub::c_fn1d_phase_pertub( INT32 phase_nb_in )
{
	_period_out = 0.;
	_phase_nb = 0;	//should always reset object variable or set_phase_nb can fail
	set_phase_nb( phase_nb_in );
}

void c_fn1d_phase_pertub::set_phase_nb( INT32 phase_nb_in )
{
	if ( phase_nb_in != _phase_nb )
	{
		_phase_nb = phase_nb_in;
		set_frequency( OVER_ONE_AS_REAL( _phase_nb ) );
		set_sample_nb( PHASE_PERTUB_STEP_NB * _phase_nb );
		_b_to_update = true;
	}
}

//////
////	do it
//
void	c_fn1d_phase_pertub::compute()
{
	REAL	a, d, t;
	REAL*	p_value;
	REAL	delta;
	REAL	deviation;
	REAL	f;
	REAL	total;
	c_rand*	rand;

	samples_alloc();

	//	we need a random generator
	rand = new c_rand_gauss_slick( _seed, _gain, _bias );

	//	first we get some random values for the integer values
	deviation = get_deviation()*REAL(2.);
	p_value = _samples;
	total = 0.;
//	DBG_PRINT_STRING( "there is phase_nb_ = %d", _phase_nb );
//	DBG_PRINT_STRING( "there is sample_nb = %d", sample_nb );
	for( INT32 i = _phase_nb; i > 0; --i )
	{
		f = REAL(1.) + deviation * rand->get_fp32();
		total += f;
		*p_value = f;
		p_value += PHASE_PERTUB_STEP_NB;
	}
	*p_value = *_samples;	//	loop clean
	_period_out = total;

	//	we don't need any more the random generator
	delete rand;
	

	//	then in between we interpolate
	//	with a function than give us horizontal tangents
	//	so we have C1 continuity
	delta = REAL(1.) / PHASE_PERTUB_STEP_NB;
	p_value = _samples;
	for( INT32 i = _phase_nb; i > 0; --i )
	{
		d = *p_value;
		a = 2 * ( d - *(p_value + PHASE_PERTUB_STEP_NB ) );
		t = delta;
		for( INT32 j = PHASE_PERTUB_STEP_NB - 1; j > 0; --j )
		{
			*++p_value = a * t * t * ( t - REAL(1.5) ) + d;
			t += delta;
		}
		++p_value;
	}
	_b_to_update = false;
}

namespace n_phase_pertub {
	CONSTEXPR INT32 PP_PARAM_NB_MAX	= 4;
	CONST c_param_def param[PP_PARAM_NB_MAX] =
	{
		PARAM_DEF_INT32(	Phase_Nb,	100, 1,	1, 10000 )
		PARAM_DEF_GAIN(		Gain		)
		PARAM_DEF_BIAS(		Bias		)
		PARAM_DEF_INT32(	Seed,		8, 7,	0, PARAM_MAX_UINT32 )	//todo INT UINT trouble here
	};
};

//clean	should be param_init_pt or nothing
CONST c_param_def*	c_fn1d_phase_pertub::set_focus()	//par
{
//todo
/*
	n_phase_pertub::param[0]._dum = (void *)&_phase_nb;
	n_phase_pertub::param[1]._dum = (void *)&_gain;
	n_phase_pertub::param[2]._dum = (void *)&_bias;
	n_phase_pertub::param[3]._dum = (void *)&_seed;
*/
//todo last param should be name()
//par	focus_list_set( pp_param, PP_PARAM_NB_MAX, "Phase_Pertubation" );
	return n_phase_pertub::param;
}

