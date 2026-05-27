#include "obj_ui/fn_3d.h"
#include "ui/strsymbo.h"
#include "math/noisturb.h"
#include "math/gainbias.h"
#include "math/v_base.h"
#include "infrastructure/param/param_declare.h"


REAL	norm_sphere_1d( REAL *vec)		{	return ABS(vec[0]);	}
REAL	norm_sphere_2d( REAL *vec)		{	return SQRT( vec[0]*vec[0] + vec[1]*vec[1]); }	//* SQRT(.5);	}	//todon	optimize
REAL	norm_sphere_3d( REAL *vec)		{	return SQRT( vec[0]*vec[0] + vec[1]*vec[1] + vec[2]*vec[2]); }// * SQRT(.33333); }

//REAL	norm_cube_1d( REAL *vec)		{	return ABS(vec[0]);	}
REAL	norm_cube_2d( REAL *vec)		{	return MAX( ABS(vec[0]), ABS(vec[1]) );	}
REAL	norm_cube_3d( REAL *vec)		{	return aaa::MAX(  ABS(vec[0]), ABS(vec[1]), ABS(vec[2]) );	}

//REAL	norm_diamond_1d( REAL *vec)		{	return ABS(vec[0]) ;	}
REAL	norm_diamond_2d( REAL *vec)		{	return ( ABS(vec[0]) + ABS(vec[1]) ) ;}	// * .5 ;	}
REAL	norm_diamond_3d( REAL *vec)		{	return ( ABS(vec[0]) + ABS(vec[1]) + ABS(vec[2]) ) ;}// * (1./3.) ;	}



static	C_PCHAR_C	str_type[9] =
{
	"Fractal sum",
	"Turbulence",
	"Ramp",
	"Grid",
	"Checker",
	"Ramp",
	"Norm",
	"Unused ?",
	"Huh"
};

static	C_PCHAR_C	str_norm[3] =
{
	"norm_sphere",
	"norm_cube",
	"norm_diamond",
};

static	C_PCHAR_C	str_curve[6] =
{
	"raw",
	"inverse",
	"cos",
	"sin",
	"modulo",
	"modulo_inverse",
};

FACTORY_CREATE_V1( c_fn_3d, fn_3d, Mix function 3D, fn3d );

namespace	n_fn_3d
{
	CONSTEXPR INT32 BASE_PARAM_NB	= 37;
	CONSTEXPR INT32 GROUP_PARAM_NB	= 0;
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_PARAM_NB
									+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_SYMBO( type, 1, 0, 8, str_type )
		PARAM_DEF_SYMBO_MIN_MAX( dim, 1, 2, 1, 3, gstr::dim )
		PARAM_DEF_AXE_Z( axe )
		PARAM_DEF_REAL_ONE( freq_factor )
		PARAM_DEF_INT32( harm, 2, 1, 1, 16 )
		PARAM_DEF_SYMBO( method, 1, 0, 2, str_norm )

		PARAM_DEF_GROUP( u, 6 )
			PARAM_DEF_REAL_ZERO( u_offset )
			PARAM_DEF_REAL_ONE( u_freq )
			PARAM_DEF_BOOL_OFF( u_abs )
			PARAM_DEF_BOOL_OFF( u_squared )
			PARAM_DEF_REAL_ZERO( u_mod )
			PARAM_DEF_REAL_INF( u_mod_offset, .5, 0. )

		PARAM_DEF_GROUP( v, 6 )
			PARAM_DEF_REAL_ZERO( v_offset )
			PARAM_DEF_REAL_ONE( v_freq )
			PARAM_DEF_BOOL_OFF( v_abs )
			PARAM_DEF_BOOL_OFF( v_squared )
			PARAM_DEF_REAL_ZERO( v_mod )
			PARAM_DEF_REAL_INF( v_mod_offset, .5, 0. )

		PARAM_DEF_GROUP( axe, 6 )
			PARAM_DEF_REAL_ZERO( axe_offset )
			PARAM_DEF_REAL_ONE( axe_freq )
			PARAM_DEF_BOOL_OFF( axe_abs )
			PARAM_DEF_BOOL_OFF( axe_squared )
			PARAM_DEF_REAL_ZERO( axe_mod )
			PARAM_DEF_REAL_INF( axe_mod_offset, .5, 0. )

			PARAM_DEF_GROUP(	out_gain_bias, 3 )
				PARAM_DEF_BOOL_OFF(	do_clamp_gain_bias )
				PARAM_DEF_GAIN(		gain	)
				PARAM_DEF_BIAS(		bias	)

		PARAM_DEF_GROUP( output, 5 )
			PARAM_DEF_SYMBO_PSTR_ZERO( output_curve, str_curve )
			PARAM_DEF_REAL_ONE( output_freq )
			PARAM_DEF_REAL_ZERO( output_phase )
			PARAM_DEF_REAL_ZERO( output_min )
			PARAM_DEF_REAL_ONE( output_max )
	};
}


CONSTRUCTOR_CREATE(c_fn_3d)
{
	param_init_with( n_fn_3d::param, n_fn_3d::PARAM_NB_MAX ); // fn_3d_param, FN_3D_PARAM_NB_MAX);

	norm = norm_sphere_3d;
}

EMPTY_DESTRUCTOR(c_fn_3d)

void	c_fn_3d::param_init_pt()
{
	INT32	h=0;

	param_set_pt( h, type);
	param_set_pt( h, dim_ui);
	param_set_pt( h, axe);
	param_set_pt( h, freq_factor);
	param_set_pt( h, harm);
	param_set_pt( h, method_ui);

	for( INT32 i=0; i<3; ++i )
	{
		++h;
		param_set_pt( h, offset_ui[i] );
		param_set_pt( h, freq_ui[i] );
		param_set_pt( h, b_abs[i] );	
		param_set_pt( h, b_squared[i] );
		param_set_pt( h, mod[i] );
		param_set_pt( h, mod_offset_ui[i] );
	}
	
	++h;
	param_set_pt( h, b_output_clamp);
	param_set_pt( h, _gain	);
	param_set_pt( h, _bias	);

	++h;
	param_set_pt( h, _s_output_curve_ui);
	param_set_pt( h, output_freq);
	param_set_pt( h, output_phase);
	param_set_pt( h, output_min);
	param_set_pt( h, output_max);

	err_param_init_pt(h);
}

void	c_fn_3d::update()
{
	_gain_factor = gain_slick_factor( _gain );
	_bias_factor = bias_slick_factor( _bias );

	output_center = output_min;
	output_factor = (output_max - output_min);

	for( INT32 i=0; i<3; ++i )
	{
		freq[i] = freq_ui[i] * freq_factor;
		offset[i] = offset_ui[i];
		mod_offset[i] = mod_offset_ui[i] * mod[i];
	}

	dim  = dim_ui;
	method  = method_ui;

	axe_build_index( axe_u, axe_v, axe);

	if ( dim == 1 )
		norm = norm_sphere_1d;
	else if ( dim == 2 )
	{
		switch( method )
		{
		case 0:	norm = norm_sphere_2d;	break;
		case 1:	norm = norm_cube_2d;	break;
		case 2:	norm = norm_diamond_2d;	break;
		}
	}
	else if ( dim == 3 )
	{
		switch( method )
		{
		case 0:	norm = norm_sphere_3d;	break;
		case 1:	norm = norm_cube_3d;	break;
		case 2:	norm = norm_diamond_3d;	break;
		}
	}

	switch( type)
	{
	case 2:		scaling_factor = REAL(1. / REAL(harm));	break;
	case 3:		scaling_factor = REAL(1. / REAL(harm));	break;
	case 4:		scaling_factor = REAL(.5 / REAL(harm));	break;
	case 5:		scaling_factor = REAL(2. / REAL(harm));	break;
	default:	scaling_factor = 1.;					break;
	}
}

REAL	c_fn_3d::compute( REAL * v)
{
	REAL	val;
	REAL	tmp;
	REAL	pos[3];
//INT32	i;

	switch( dim_ui )
	{
	case 1:
		pos[0] = (v[axe]   *freq[0]) + offset[0];
		pos[1] = offset[1];
		pos[2] = offset[2];
		break;
	case 2:
		pos[0] = (v[axe_u] *freq[0]) + offset[0];
		pos[1] = (v[axe_v] *freq[1]) + offset[1];
		pos[2] = offset[2];
		break;
	case 3:
		pos[0] = (v[axe]   *freq[0]) + offset[0];
		pos[1] = (v[axe_u] *freq[1]) + offset[1];
		pos[2] = (v[axe_v] *freq[2]) + offset[2];
		break;
	}

//	pos[0] += F_FLOOR( pos[1] / mod[1]) * SIN_INT(dummy_float[0]);


	if( mod[0] != 0.)
		pos[0] = FMOD( pos[0] ) * mod[0] - mod_offset[0];
	if( mod[1] != 0.)
		pos[1] = FMOD( pos[1] ) * mod[1] - mod_offset[1];
	if( mod[2] != 0.)
		pos[2] = FMOD( pos[2] ) * mod[2] - mod_offset[2];


	if( b_abs[0] )
		pos[0] = ABS( pos[0]);
	if( b_abs[1] )
		pos[1] = ABS( pos[1]);
	if( b_abs[2] )
		pos[2] = ABS( pos[2]);

	
	if( b_squared[0] )
		pos[0] *= pos[0];
	if( b_squared[1] )
		pos[1] *= pos[1];
	if( b_squared[2] )
		pos[2] *= pos[2];


	if( type < 2)
	{
		switch( type)
		{	//			f = SIN_INT( vec[1] + rx*f) * .5 + .5;
		case 0:	//	fractalsum
			val = fractalsum( pos, harm );
			break;
		case 1:	//	turbulence
			val = turbulence( pos, harm );
			break;
		}
	}
	else
	{
		val = 0.;
		switch( type)
		{	//			f = SIN_INT( vec[1] + rx*f) * .5 + .5;
		case 2:	//	ramp
			switch( dim )
			{
			case 1:
				for( INT32 i = harm; i>0; --i )
				{
					val += pos[0];
					pos[0] *= 2.;
				}
				break;
			case 2:
				for( INT32 i = harm; i>0; --i )
				{
					val +=	pos[0] * pos[1];
					pos[0] *= 2.;
					pos[1] *= 2.;
				}
				break;
			case 3:
				for( INT32 i = harm; i>0; --i )
				{
					val += pos[0] * pos[1] * pos[2];
					pos[0] *= 2.;
					pos[1] *= 2.;
					pos[2] *= 2.;
				}
				break;
			}
			val = scaling_factor * val;
			break;
		case 3:	// grid
			switch( dim )
			{
			case 1:
				for( INT32 i = harm; i>0; --i )
				{
					val += pos[0];
					pos[0] *= 2.;
				}
				break;
			case 2:
				for( INT32 i = harm; i>0; --i )
				{
					val += MIN( pos[0], pos[1] );
					pos[0] *= 2.;
					pos[1] *= 2.;
				}
				break;
			case 3:
				for( INT32 i = harm; i>0; --i )
				{
					tmp = MIN( pos[0], pos[1] );
					val += MIN( tmp, pos[2] );
					pos[0] *= 2.;
					pos[1] *= 2.;
					pos[2] *= 2.;
				}
				break;
			}
			val = scaling_factor * val + REAL(.5);
			break;
		case 4:	//	checker
			switch( dim )
			{
			case 1:
				for( INT32 i = harm; i>0; --i )
				{
					val += COS_TURN(pos[0]);
					pos[0] *= 2.;
				}
				break;
			case 2:
				for( INT32 i = harm; i>0; --i )
				{
					val += COS_TURN(pos[0]) * COS_TURN(pos[1]);
					pos[0] *= 2.;
					pos[1] *= 2.;
				}
				break;
			case 3:
				for( INT32 i = harm; i>0; --i )
				{
					val += COS_TURN(pos[0]) * COS_TURN(pos[1]) * COS_TURN(pos[2]);
					pos[0] *= 2.;
					pos[1] *= 2.;
					pos[2] *= 2.;
				}
				break;
			}
			val = scaling_factor * val + REAL(.5);
			break;
		case 5:	//	test norm
			switch( dim )
			{
			case 1:
				for( INT32 i = harm; i>0; --i )
				{
					val += (*norm)( pos);
					pos[0] *= 2.;
				}
				break;
			case 2:
				for( INT32 i = harm; i>0; --i )
				{
					val += (*norm)( pos);
					pos[0] *= 2.;
					pos[1] *= 2.;
				}
				break;
			case 3:
				for( INT32 i = harm; i>0; --i )
				{
					val += (*norm)( pos);
					pos[0] *= 2.;
					pos[1] *= 2.;
					pos[2] *= 2.;
				}
				break;
			}
			val = scaling_factor * val;
			break;
		case 7:
			val = pos[0]*pos[0] + pos[1]*pos[1];
			break;
		}
	}

	if ( b_output_clamp )
	{
//		printf( "%2.2f ->", val);
		CLAMP_REF_01( val );
//		printf( "%2.2f\n", val);
	//todon check GAIN and bias ouside [0,1]
		gain_slick_with_factor( val, _gain_factor);
		bias_slick_with_factor( val, _bias_factor);
	}

	switch( _s_output_curve_ui )
	{
	case 1:	val = REAL(1.) - val;												break;
	case 2:	val = REAL( COS_TURN(val*output_freq + output_phase) * -.5 + .5 );	break;
	case 3: val = REAL( COS_TURN(val*output_freq + output_phase) * .5 + .5 );	break;
	case 4:	val = FMOD( val*output_freq + output_phase );						break;
	case 5:	val = REAL( 1. - FMOD( val*output_freq + output_phase ) );			break;
	}

	val = output_center + output_factor * val;
	return val;
}
