#include "aaa_def.h"

#include "wave.h"
#include "math/fn1d/fn1d_pp.h"
#include "math/fn1d/fn1d_fbm.h"
#include "ui/flatland.h"
#include "draw/seeddraw.h"
#include "math/gainbias.h"
#include "time/aaa_time.h"
#include "math/noisturb.h"
#include "math/v_base.h"
#include "gol/gol_light.h"
#include "gol/gol_color.h"
#include "gol/gol_draw.h"
#include "gol/gol_tex.h"
#include "gol/gol_matrix.h"
#include "infrastructure/param/param_declare.h"

FACTORY_INSTANCE_V1( c_def_wave, def_wave, Deformer Wave, def );

namespace	n_def_wave
{
	CONSTEXPR INT32 START_PARAM_NB	= 0 + c_deformer::BASE_PARAM_NB;
	CONSTEXPR INT32 BASE_PARAM_NB	= 21;
	CONSTEXPR INT32 MORE_PARAM_NB	= 14;
	CONSTEXPR INT32 GROUP_PARAM_NB	= 5;
	CONSTEXPR INT32 PARAM_NB_MAX	=	START_PARAM_NB
									+	BASE_PARAM_NB
									+	MORE_PARAM_NB
									+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		DEFORMER_BASE_PARAMS

		PARAM_DEF_GROUP( Base, BASE_PARAM_NB + 3 )	
			PARAM_DEF_REAL_INF(		Len,	0., 10	)
			PARAM_DEF_REAL_ONE(		Height	)
			PARAM_DEF_REAL_INF(		Shape,	1., 0.5	)	//	0:plat, 1:pointu, >1boucle
			PARAM_DEF_REAL_ZERO(	Wind	)		//	max 1., >1 frisoti pattout

			PARAM_DEF_REAL_ZERO(	Angle	)
			PARAM_DEF_REAL_ONE(		Period	)
			PARAM_DEF_REAL_ZERO(	Phase	)	//	demandez a cordo

			PARAM_DEF_GROUP( Perturbation, 4 )	
				PARAM_DEF_BOOL_ON(		Pertub_State )
				PARAM_DEF_REAL_ONE(		Pertub_X		)	//	0:pas, 1:xboucle sur la len
				PARAM_DEF_REAL_ZERO(	Pertub_Z		)	//	0:pas, 1.:trop, au minimun
				PARAM_DEF_REAL_ONE(		Pertub_Strenght	)	//	0:pas, 1:max c'est la len
		
			PARAM_DEF_GROUP( Regularity, 4 )	
				PARAM_DEF_BOOL_ON(	Regul_State )
				PARAM_DEF_REAL(		Regul_Max,			1, 5,	1., 200.	)	//	rapport entre la vague la plus longue possible et la plus courte
				PARAM_DEF_REAL_ONE(	Regul_height_link	)	//	0:hauteur constante, 1:hauteur/len constant
				PARAM_DEF_REAL_ONE(	Regul_shape_link	)	// 0:shape variable 1:shape constante

			PARAM_DEF_GROUP( Turbulence, 6 )	
				PARAM_DEF_BOOL_ON(	Turb_State )
				PARAM_DEF_REAL(		turb_strenght,	1., .5,		0, 200.		)
				PARAM_DEF_REAL(		turb_x,			0., 1.,		0., 200.	)
				PARAM_DEF_REAL(		turb_phase,		0., 1.,		0., 200.	)
				PARAM_DEF_REAL(		turb_change,	1., 0.,		0., 200.	)
				PARAM_DEF_INT32(		turb_harm,		2., 1.,		1., 16.		)

		//More
		PARAM_DEF_GROUP( More, MORE_PARAM_NB )	
			PARAM_DEF_INT32(		Pertub_resolution,	4, 10,		4, 16	)	//	resolution en p2 de la la courbe de pertub
			PARAM_DEF_REAL(			Pertub_fractal_dim,	1., 1.02,	1., 2.	)	// 1:plat 2:bruit max
			PARAM_DEF_GAIN(			Pertub_gain	)	//	0:tout gris .5:uniforme reparti 1:contraste maxi
			PARAM_DEF_BIAS(			Pertub_bias	)	//	0:petite valeur 1:grosse valeur
			PARAM_DEF_INT32_INF(	Pertub_seed,		7, 88888	)	//	seed du random

			PARAM_DEF_BOOL_OFF(		Regul_smooth )
			PARAM_DEF_INT32(		Regul_wave_nb,		4, 100,		2, 1000	)	// nb de vague sur lesquelles l'algo boucle
			PARAM_DEF_GAIN(			Regul_gain	)		
			PARAM_DEF_BIAS(			Regul_bias	)
			PARAM_DEF_INT32_INF(	Regul_seed,			7, 88888	)

			PARAM_DEF_GAIN(			Turb_gain	)
			PARAM_DEF_BIAS(			Turb_bias	)
			PARAM_DEF_INT32_INF(	Turb_seed,			7, 45646	)

			PARAM_DEF_BOOL_OFF( Propagate )
	};
}

void	c_def_wave::param_init_pt()
{
	INT32	h = SUPER::param_init_pt_start();

	//Base
	++h;
		param_set_pt( h, _param_len );
		param_set_pt( h, _param_height );
		param_set_pt( h, _param_shape );
		param_set_pt( h, _param_wind );

		param_set_pt( h, _param_angle );
		param_set_pt( h, _period );
		param_set_pt( h, _param_phase );

		++h;
			param_set_pt( h, _b_pertub_on );
			param_set_pt( h, _pertub_x );
			param_set_pt( h, _pertub_z );
			param_set_pt( h, _pertub_factor );

		++h;
			param_set_pt( h, _b_regul );
			param_set_pt( h, _regul_max );
			param_set_pt( h, _regul_height );
			param_set_pt( h, _regul_shape );

		++h;
			param_set_pt( h, _b_turb );
			param_set_pt( h, _turb_strenght );
			param_set_pt( h, _turb_x_ui );
			param_set_pt( h, _turb_phase_ui );
			param_set_pt( h, _turb_change_ui );
			param_set_pt( h, _turb_harm );

	//details
	++h;
		param_set_pt( h, _pertub_resolution );
		param_set_pt( h, _pertub_fractal_dim );
		param_set_pt( h, _pertub_gain );
		param_set_pt( h, _pertub_bias );
		param_set_pt( h, _pertub_seed );

		param_set_pt( h, _b_regul_smooth );
		param_set_pt( h, _regul_wave_nb );
		param_set_pt( h, _regul_gain );
		param_set_pt( h, _regul_bias );
		param_set_pt( h, _regul_seed );

		param_set_pt( h, _turb_gain );
		param_set_pt( h, _turb_bias );
		param_set_pt( h, _turb_seed );

		param_set_pt( h, _b_propagate );

	err_param_init_pt(h);
}

CONSTRUCTOR_CREATE(c_def_wave)
,_i_sample_nb(0)
,_f_sample_nb(0.)
,_phase_table(nullptr)
,_ry_table(nullptr)
,_rz_table(nullptr)
,_pertub(nullptr)
,_regul(nullptr)
{
	init_name_with( "Wave" );
	param_init_with( n_def_wave::param, n_def_wave::PARAM_NB_MAX ); // def_wave_param, DEF_WAVE_PARAM_NB_MAX);

	init();
}

void	c_def_wave::init()
{
	clear_v3( _f_turb );
	_len_last = -_param_len;
	_height_last = -_param_height;
	_shape_last = -_param_shape;
	_regul_height_last = -_regul_height;
	_regul_shape_last = -_regul_shape;
	_b_regul_smooth_last = !_b_regul_smooth;

	_pertub = new c_fn1d_fm_add();
	_pertub->update();
 
	_regul = new c_fn1d_phase_pertub( _regul_wave_nb );

//todo strategy of init/update/alloc/dealloc
//try	compute_phase();
}

c_def_wave::~c_def_wave()
{
	obj_delete( _pertub );
	obj_delete( _regul );

	FREE_ALIGNED(_phase_table);
	FREE_ALIGNED(_ry_table);
	FREE_ALIGNED(_rz_table);
}

INT32 CONST	PHASE_STEP_NB = 8;

//todo the height field is unused by the algo
void	c_def_wave::compute_phase()
{
	REAL	err_margin;

	//	regul hold the phase perturbation for phase_nb
	_regul->update();
	INT32	phase_nb = _regul->get_phase_nb();

	//	now we can build the arrays
	//	we have more resolution in the array so we will do linear interpolation at+b
	REAL tmp = _regul->get_period_out();
	_frequency = OVER_ONE_AS_REAL(tmp);
	//	we want the smallest phase to be covered by at least PHASE_STEP_NB
	_i_sample_nb = INT32( PHASE_STEP_NB * tmp/_regul->get_min() );
	_f_sample_nb = (REAL) (_i_sample_nb);
	err_margin = tmp/_f_sample_nb;
	err_margin /= 4.;
	
#if	AAA_DEBUG()
	DBG_PRINT_STRING( "%d phase with period %f and an array sizeof %d", phase_nb, (FP32)tmp, _i_sample_nb+1);
#endif

	//	allocate the three buffers
	INT32 i = (_i_sample_nb + 1) * sizeof(REAL) ;
	_phase_table = (REAL *) REALLOC_ALIGNED( _phase_table, i );
	_ry_table    = (REAL *) REALLOC_ALIGNED( _ry_table,    i );
	_rz_table    = (REAL *) REALLOC_ALIGNED( _rz_table,    i );

	if( !_phase_table || !_ry_table || !_rz_table )
	{
		BELL();
		err_print( "Can't allocate the phase table for %d samples", _i_sample_nb + 1);
		return;
	}

	REAL	p_prev,p,x;	//	phases
	REAL	f;
	REAL	ry_base =  _param_height * REAL(.5);
	REAL	rz_base = _param_len * _param_shape * REAL(PI_TIME_2_OVER_1);
		//	prepare
	REAL*	p_phase = _phase_table;
	REAL*	p_ry	= _ry_table;
	REAL*	p_rz	= _rz_table;

	x = 0; 
	//	p_prev store the previous value
	p_prev = 0;
	p = 0;

	i = 0;
	f = 0.;
	if( _b_regul_smooth )
/*		{
		while( i < _i_sample_nb)
			{
			REAL	ap,cp,mul;
			//	p accumulate the phase perturbation
			p += _regul->get(f);

			//	interpolation ap*t*t*(t-3/2)+cp*t+f
			//		with t = (x-p_prev)/(p-p_prev)
			//			x = p_prev -> f
			//			x = p -> (f+1)
			cp = p-p_prev;
			mul = 1./cp;
			ap = 2*(cp-1.);

			while( true )
				{
				x = _regul->get_period_out()*i/_f_sample_nb;
				if ( x > (p-err_margin) )
					break;
				tmp = (x-p_prev)*mul;
				tmp = ap*tmp*tmp*(tmp-1.5)+cp*tmp+f;
				*p_phase++ = tmp;

				tmp -= .5;	// we need to be at the top of the wave
				//	we get the relative size over there
				tmp = _regul->get(tmp) - 1.;
				*p_ry++ = ry_base * (tmp*param_regul_height+1.);
				*p_rz++ = rz_base * (tmp*_regul_shape+1.);

				++i;
				}

			p_prev = p;
			f += 1.;
			}
		}
*/
	{
		while( i < _i_sample_nb)
		{
			REAL	ap,bp;	//	phases coefficient
			//	p accumulate the phase perturbation
			p += _regul->get(f);

			//	interpolation ap*x+bp
			//			x = p_prev -> f
			//			x = p -> (f+1)
			ap = OVER_ONE_AS_REAL(p-p_prev);
			bp = f - ap*p_prev;

			while( true )
			{
				x = _regul->get_period_out()*i/_f_sample_nb;
				if ( x > (p-err_margin) )
					break;
				tmp = ap*x + bp;
				*p_phase++ = tmp;
				++i;
			}

			p_prev = p;
			f += 1.;
		}
		//	prepare again
		p_phase = _phase_table;
		x = 0; 
		//	p_prev store the previous value
		p_prev = 0;
		p = 0;

		i = 0;
		f = 0.;
		while( i < _i_sample_nb)
		{
			tmp = *p_phase++;

			tmp -= REAL(.5);	// we need to be at the top of the wave
			//	we get the relative size over there
			tmp = _regul->get(tmp) - REAL(1);
			*p_ry++ = ry_base * ( tmp * _regul_height + REAL(1) );
			*p_rz++ = rz_base * ( tmp * _regul_shape  + REAL(1) );

			++i;
		}
	}
	else
	{
		while( i < _i_sample_nb)
		{
			REAL	ap,bp;	//	phases coefficient
			//	p accumulate the phase perturbation
			p += _regul->get(f);

			//	interpolation ap*x+bp
			//			x = p_prev -> f
			//			x = p -> (f+1)
			ap = OVER_ONE_AS_REAL(p-p_prev);
			bp = f - ap*p_prev;

			while( true )
			{
				x = _regul->get_period_out()*i/_f_sample_nb;
				if ( x > (p-err_margin) )
					break;
				tmp = ap*x + bp;
				*p_phase++ = tmp;

				tmp -= .5;	// we need to be at the top of the wave
				//	we get the relative size over there
				tmp = _regul->get(tmp) - REAL(1);
				*p_ry++ = ry_base * ( tmp * _regul_height + REAL(1) );
				*p_rz++ = rz_base * ( tmp * _regul_shape  + REAL(1) );

				++i;
			}

			p_prev = p;
			f += 1.;
		}
	}
#if	AAA_DEBUG()
	if ( i != _i_sample_nb )
	{
		BELL();
		err_print( "i is %d and should be %d", i, _i_sample_nb);
	}
#endif
	//	should loop nicely for interpolation
	*(_phase_table + _i_sample_nb) = REAL(phase_nb);
	*(_ry_table    + _i_sample_nb) = *_ry_table;
	*(_rz_table    + _i_sample_nb) = *_rz_table;
//	printf( "sample %d value %f : %f, ", _i_sample_nb, _regul->get_period_out(), *(_phase_table + _i_sample_nb));
//	printf( "%f, ", *(_ry_table    + _i_sample_nb) );
//	printf( "%f\n", *(_rz_table    + _i_sample_nb) );

	_len_last = _param_len;
	_height_last = _param_height;
	_shape_last = _param_shape;
	_regul_height_last = _regul_height;
	_regul_shape_last = _regul_shape;
	_b_regul_smooth_last = _b_regul_smooth;
}

void	c_def_wave::update()
{
	REAL	k;		//	real wave len

	_pertub->set_min_max( 0, _pertub_factor );
	_pertub->set_sample_nb( 1<<_pertub_resolution ); 
	_pertub->set_fractal_dim( _pertub_fractal_dim ); 
	_pertub->set_gain( _pertub_gain ); 
	_pertub->set_bias( _pertub_bias ); 
	_pertub->set_seed( _pertub_seed ); 
	_pertub->update();

	k	= OVER_ONE_AS_REAL( _param_len );
	_rz	= _param_len * _param_shape * REAL(PI_TIME_2_OVER_1);
	_ry	= _param_height * REAL(.5);
	_wind = _param_wind * REAL(.25);

	_the_cos = COS_TURN( _param_angle );
	_the_cos_time_k = _the_cos * k;
	_the_cos_per_x = _the_cos_time_k * _pertub_x;
	_the_sin = SIN_TURN( _param_angle );
	_the_sin_time_k = _the_sin * k;
	_the_sin_per_x = _the_sin_time_k * _pertub_x;

	_regul->set_center_ratio( 1., _regul_max );
	_regul->set_phase_nb( _regul_wave_nb );
	_regul->set_gain( _regul_gain ); 
	_regul->set_bias( _regul_bias ); 
	_regul->set_seed( _regul_seed ); 
	if( _regul->is_to_update()
			|| _len_last != _param_len
			|| _height_last != _param_height
			|| _shape_last != _param_shape
			|| _regul_height_last != _regul_height
			|| _regul_shape_last != _regul_shape		
			|| _b_regul_smooth_last != _b_regul_smooth	
		)
		compute_phase();

	if( 0.00000001 < _period || _period < -0.00000001 )
		_phase_cur = _param_phase - REAL(aaa::time::get()) / _period;
	else
		_phase_cur = _param_phase;

	_turb_gain_factor = gain_slick_factor(_turb_gain);
	_turb_bias_factor = bias_slick_factor(_turb_bias);

	_turb_x = _turb_x_ui * k;	// in regular space
	_turb_phase = _turb_phase_ui;	//	in phase space
	_turb_change = _turb_change_ui;	//	in phase space	
	
	_f_turb[1] = _phase_cur * _turb_change;
	set_deforming( true );
}

/* was
REAL	c_def_wave::phase_get( REAL f_in)
{
register	INT32		i_floor;
register	REAL	*p_pha;
//		REAL	*p_hau;

	f_in *= _frequency;
	f_in = FMOD( f_in, _f_sample_nb);
		
	i_floor = (INT32) f_in;
	p_pha = _phase_table + i_floor;
//	p_height = height + (int) i_floor;

	f_in -= i_floor;
	return  (*p_pha)*( 1.-f_in) + (*(p_pha+1))*f_in;
}
*/

//todo	optimize optimize with int or see pertub
/*
REAL	c_def_wave::phase_get( REAL f_in)
{
register	INT32		i;
register	REAL		*p;
	f_in *= _frequency;
	//	get the decimal part
	f_in -= F_FLOOR(f_in);

	// and use it in the array
	f_in *= _i_sample_nb;
	i = (INT32) f_in;	// we know f_in > 0
	f_in -= i;

	//	set ry and rz
	p = _ry_table + i;
	_ry = *p *(1.-f_in) + *(p+1) *f_in;
	p = _rz_table + i;
	_rz = *p *(1.-f_in) + *(p+1) *f_in;

	//	and return the phase
	p = _phase_table + i;
	return   *p *(1.-f_in) + *(p+1) *f_in;
}
*/

REAL	c_def_wave::height_change_get()
{
	REAL f = turbulence( _f_turb, _turb_harm );
	gain_slick_with_factor( f, _turb_gain_factor );
	bias_slick_with_factor( f, _turb_bias_factor );
	return f;
}

//todo	optimize optimize with int or see pertub
REAL	c_def_wave::phase_get( REAL f_in)
{
	INT32		i;
	REAL		*p;

	f_in *= _frequency * _f_sample_nb;
//	f_in *= REAL(PHASE_STEP_NB) / _regul->get_min();
	f_in = FMOD( f_in, _f_sample_nb);

	i = (INT32) f_in;	// we know f_in > 0
	f_in -= i;


	//	get ry
	p = _ry_table + i;
	_ry = *p * (REAL(1)-f_in) + *(p+1) * f_in;

	//	set rz
	p = _rz_table + i;
	_rz = *p * (REAL(1)-f_in) + *(p+1) * f_in;

/*
	//we change the height only on the top half of wave
	tmp = ph - F_FLOOR(ph) -.5;
	tmp = ABS(tmp);
	if (tmp<.25)
		{
		_f_turb[2] = F_FLOOR( ph ) * _turb_phase;
		f1 = height_change_get() * _turb_strenght;
		f1 = 1. - f1; // * (COS_INT(2*tmp)+1.)*.5;
		_ry *= f1;
		}
*/

	//	and return the phase
	p = _phase_table + i;
	return *p * (REAL(1)-f_in) + *(p+1) * f_in;
}


void	c_def_wave::apply( REAL CONST * src, REAL* dst, INT32 nb )
{
	REAL		z;
	REAL		phase;
	REAL		x;
	REAL		dy;

	for( ; nb>0; --nb )
	{
		//	wave propagate in xz		
		x = *src;
		z = *(src+2);
//todo check and reactivate
/*		if (_b_propagate)
		{
			z -= phase_cur*_param_len;
			*(src+2) = z;
		}
*/
		//	compute the phase
		phase = -x*_the_sin_time_k + z*_the_cos_time_k + _phase_cur; 
		//	build x and phase for the wave train deformation
		x = x*_the_cos_per_x + z*_the_sin_per_x;

		_f_turb[0] = x * _turb_x;
		//	change it with the regul stuff first
		//	and eventually distort the grid in x and z ( the undistorted phase )
		if( _b_regul )
		{
			if( _b_pertub_on )
			{
				//	cas 1
				phase -= _pertub->get_from_2( x, phase, _pertub_z );
				phase = phase_get( phase );
				//	cas 2
				//phase = phase_get( phase ) - _pertub->get_from_2( x, phase, _pertub_z );
				//	cas 3
				//phase = phase_get( phase );
				//phase -= _pertub->get_from_2( x, phase, _pertub_z );
			}
			else
			{
				phase = phase_get( phase );
			}	
		}
		else
			if( _b_pertub_on )
				phase -= _pertub->get_from_2( x, phase, _pertub_z );

		z = COS_TURN( phase );
		if( _b_turb )
		{
			//todo	use lerp() more
			_f_turb[2] = F_FLOOR( phase ) * _turb_phase;
			REAL f1 = height_change_get();
					
			_f_turb[2] += _turb_phase;
			REAL f2 = height_change_get();

			interpolate( f1, f2, phase - F_FLOOR( phase ) );
			f1 = REAL(1) + _turb_strenght * (f1 - REAL(1));
			z = (z-REAL(1)) * f1 + REAL(1);
		}
		//	calculate the cycloid moves
		dy = - _ry * z;	
//dy = _ry;
		z = _rz * SIN_TURN( phase + _wind * z );
//z = 0.;
		//	change original position
		if( _b_add )
		{
			cpy_v3( dst, src );
			*dst++ -= z*_the_sin;
			*dst++ += dy;
			*dst++ += z*_the_cos;
		}
		else
		{
			clear_v3( dst );
			*dst++ = -z*_the_sin;
			*dst++ = dy;
			*dst++ = z*_the_cos;
		}
		src += 3;
	}
}




#include "draw/render.h"
#include "obj_ui/bdd/util/bdd.h"

void c_def_wave::draw_curve()
{
	if( c_flatland::b_draw_curve )
	{	//todo these disables are dangerous
		GOL::set_texture_0D();
		GOL::disable_lighting();
		GOL::matrix::push();
			GOL::matrix::set_ortho_with_secu_margin( 100, 100);
			GOL::color_red();
			draw_phase_in_rect(0, 0, 100, 30);
			draw_phase_break_in_rect(0, 0, 100, 30);
			GOL::color_green();
			draw_interpo(0, 0, 100, 100);
		GOL::matrix::pop();
	}
}

#define	PHASE_NB		30
#define	STEP_BY_PHASE	80

void	c_def_wave::draw_phase_in_rect( REAL x, REAL y, REAL w, REAL h )
{
	REAL	dx;
	REAL	f;
	REAL	df;

	f = _regul_wave_nb + REAL(2); 
	dx = w/(f*STEP_BY_PHASE);
	x += w;
	f -= 1.;
	df = REAL(1./STEP_BY_PHASE);
	GOL::begin(GL_LINE_STRIP);
	for( ; x>0; x -= dx)
	{
		GOL::vertex2( x, y + ( SIN_TURN(phase_get(f)) + REAL(1) ) * h * REAL(.5) );
		f -= df;
	}
	GOL::end();
}

void	c_def_wave::draw_phase_break_in_rect( REAL x, REAL y, REAL w, REAL h )
{
	REAL	dx;
	REAL	f;
	REAL	df;

	f = _regul_wave_nb + REAL(2); 
	dx = w/(f*STEP_BY_PHASE);
	x += w;
	f -= REAL(1);
	df = REAL(1./STEP_BY_PHASE);
	GOL::begin(GL_LINE_STRIP);
	for( ; x>0; x -= dx)
	{
		GOL::vertex2( x, y + FMOD( phase_get(f), REAL(PI_TIME_2) ) * h / REAL(PI_TIME_2) );
		f -= df;
	}
	GOL::end();
}

void	c_def_wave::draw_interpo(REAL x, REAL y, REAL w, REAL h)
{
	REAL	dx;
	REAL	a;
	REAL	c = _param_wind;

	a = REAL(2) * (c - REAL(1));
	dx = REAL(1./STEP_BY_PHASE);
	GOL::begin(GL_LINE_STRIP);
	// todoqqq why pass x as parameters, only used as a counter
	for( x=0.; x<=1.; x += dx )
	{
		GOL::vertex2( x*w, (a*x*x*(x-REAL(1.5))+c*x)*h );
	}
	GOL::end();
}


