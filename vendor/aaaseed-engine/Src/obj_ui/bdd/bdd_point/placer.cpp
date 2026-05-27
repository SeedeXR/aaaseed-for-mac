#include "placer.h"

#include "infrastructure/obj/obj_ui.h"

#include "math/rand.h"
#include "math/noisturb.h"

#include "image/img.h"
#include "image/bind_img_2d.h"
#include "time/aaa_time.h"

#include "math/v.h"
#include "language/lua/aaalua_wrap.h"

//todo	reinstall the statiALEc particle bug
//ttk	check mem alloc
//ttk		and add error/status functions

c_rand_lin		rand_create_and_place;

////////
////////
C_PCHAR_C	c_placer::str_type[TYPE_NB_MAX] =
{
	"Origin",
	"Random",
	"RED",			"GREEN",		"BLUE",				"ALPHA",			"GREY",

	"TURBULENCE",					"FRACTALSUM",	
	"TURBULENCE_IMPROVED",			"FRACTALSUM_IMPROVED",

	"LUA",

	"FIELD_TURBULENCE",				"FIELD_FRACTALSUM",
	"FIELD_TURBULENCE_IMPROVED",	"FIELD_FRACTALSUM_IMPROVED",
};

void	c_placer::param_set( c_obj_ui* caller, INT32& h )
{
//	caller->param_set_pt( h, _b_compute_trig );
	caller->param_set_pt( h, _s_how_ui );
	caller->param_set_pt( h, _b_sphere );
	++h;
		caller->param_set_pt( h, _bind_ui );
		caller->param_set_pt( h, _min_ui );
		caller->param_set_pt( h, _max_ui );
		caller->param_set_pt( h, _b_use_min_max) ;
		caller->param_set_pt( h, _b_outside_ui );	
		caller->param_set_pt( h, _try_max );
		caller->param_set_pt( h, _b_try_always );
		caller->param_set_pt_3( h, _offset_ui );
		caller->param_set_pt_4( h, _scale_ui );
		caller->param_set_pt( h, _b_clamped_ui );
	++h;
		caller->param_set_pt( h, _turb_harm_nb );
		caller->param_set_pt( h, _turb_speed );

	caller->param_set_pt( h, _lua_fn );
}

c_placer::c_placer()
:_img(nullptr)
{
}

void	c_placer::build_comment( o_str& o )
{
	if( _s_how_ui == TYPE_NO )
	{
		o.erase();
		return;
	}
	o.set( c_placer::str_type[_s_how_ui] );
	if( _b_sphere )
		o.add( " Sphere" );
	if( INSIDE_MIN_MAX( _s_how_ui, TYPE_RED, TYPE_GREY ) )
	{
		o.add_space();
		o.add( _bind_ui );
	}
}
void	c_placer::update()
{
	if( _s_how_ui==TYPE_NO )
		return;

	ASSIGN_MIN_MAX( _min, _max, _min_ui, _max_ui );
	_mask_range_factor = OVER_ONE_AS_REAL( _max_ui - _min_ui );
	sub_v3( _offset, _offset_ui, REAL(.5) );				// because we need to add .5 to u/v/w
	scale_v3( _scale, _scale_ui, _scale_ui[3] );

	if( INSIDE_MIN_MAX( _s_how_ui, TYPE_RED, TYPE_GREY ) )
	{
		_img = g_bind_img_2d->get_ready( _bind_ui );
		//		b_emission_use_mask = _img->is_ok();
		_b_something = _img->is_ok() && _img->find_rect_with_value( _rect, _min, _max, _b_outside_ui,  (aaa::COMPO) ( INT32(aaa::COMPO::RED) + _s_how_ui - TYPE_RED ) );
	}
}

FINLINE	void c_placer::pick_point_in_cube( float* vec ) CONST
{
	//	we want a random point in a cube
	//	so we will get u v w in the [-.5,.5] interval
	vec[0] = rand_create_and_place.get_fp32();
	vec[1] = rand_create_and_place.get_fp32();
	vec[2] = rand_create_and_place.get_fp32();
}

FINLINE	void c_placer::pick_point_in_sphere( float* vec ) CONST
{
	//	we want a random point in a sphere
	//	so we will get u v w in the [-.5,.5] interval
	REAL d;
	do 
	{
		do 
		{
			vec[0] = rand_create_and_place.get_fp32();
			vec[1] = rand_create_and_place.get_fp32();
			d = vec[0]*vec[0] + vec[1]*vec[1];
		}
		while( d > .25 );
		vec[2] = rand_create_and_place.get_fp32();
	}
	while( ( d + vec[2]*vec[2] ) > .25 );
}

//	Always find a random position in a cube or a sphere
FINLINE	void c_placer::build_point( float* vec ) CONST
{	
	if( _b_sphere )
		pick_point_in_sphere( vec );
	else
		pick_point_in_cube( vec );	
}

FINLINE	bool c_placer::is_val_break( FP32 val ) CONST
{
	if( _b_outside_ui )
		return OUTSIDE_MIN_MAX( val, _min, _max );

	if( INSIDE_MIN_MAX( val, _min, _max ) )
	{
		if( !_b_use_min_max )
			return true;
		val = ( val - _min) * _mask_range_factor ;
		if( rand_create_and_place.get_fp32_01() < val )
			return true;
	}
	return false;
}

FINLINE	bool	c_placer::place_no( UINT32 nb, float* dst, UINT32 dst_stride  ) CONST
{	
	for( UINT32 id=nb; id>0; --id )
	{
		clear_v3( dst );
		dst[3] = 1.;
		dst += dst_stride;
	}
	return true;
}

FINLINE	bool	c_placer::place_random( UINT32 nb, float* dst, UINT32 dst_stride  ) CONST
{	
	for( UINT32 id=nb; id>0; --id )
	{
		build_point( dst );
		dst[3] = 1.;
		dst += dst_stride;
	}
	return true;
}

FINLINE	bool	c_placer::place_lua( UINT32 nb, float* dst, UINT32 dst_stride  ) CONST
{	
	for( UINT32 id=nb; id>0; --id )
	{
		g_lua_wrap_cur->do_fn_fn_pass_i( nullptr, nullptr, _lua_fn.get(), id, dst, 4 );
		dst += dst_stride;
	}
	return true;
}

FINLINE	bool	c_placer::place_compo( UINT32 nb, float* dst, UINT32 dst_stride  ) CONST
{	
	bool	retcode = true;	
	//	we use an image to constraint the emission
	//todo we should prepare all that in update

	if( _b_something )
	{
		aaa::COMPO compo = (aaa::COMPO) ( INT32(aaa::COMPO::RED) + _s_how_ui - TYPE_RED );
		for( UINT32 id=nb; id>0; --id )
		{
			INT32	create_try = _try_max;	//	this avoid endless or long loop with near empty mask
			do	//	the grey level will command the probability of emission
			{
				//	first we want a point on a cube or eventually a sphere
				//build_point( u, v, w );
				REAL	d;
				dst[0] = rand_create_and_place.get_fp32();
				REAL	lu	= ( dst[0] - _offset[0] ) * _scale[0];

				if( lu < _rect.left || _rect.right < lu )
					continue;

				dst[1] = rand_create_and_place.get_fp32();
				if( _b_sphere )
				{
					d = dst[0]*dst[0] + dst[1]*dst[1];
					if( d >= .25 )
						continue;
				}
				REAL	lv	= ( dst[1] - _offset[1] ) * _scale[1];
				if( lv < _rect.bottom || _rect.top < lv )
					continue;
					

				FP32 val = _img->get_value_from_uv( lu, lv, _b_clamped_ui, compo );
				if( is_val_break( val ) )
				{	
					dst[2] = rand_create_and_place.get_fp32();
					if( _b_sphere )
					{
						d = dst[2]*dst[2];
						if( d >= .25 )
							continue;
					}
					break;
				}
			}
			while( --create_try > 0 );
			if( create_try <= 0 )
				retcode = _b_try_always;
			dst += dst_stride;
		}
	}
	else
		retcode = false;
	return retcode;
}

bool	c_placer::place_noise( UINT32 nb, float* dst, UINT32 dst_stride ) CONST
{
	bool	retcode = true;	
	REAL	b_update_w = _scale[2]!=0. ; 
	REAL	vec[3];
	FP32	val;

	vec[2] = 0.;

	for( UINT32 id=nb; id>0; --id )
	{
		INT32	create_try = _try_max;	//	this avoid endless or long loop with near empty mask
		do	//	the grey level will command the probability of emission
		{
			//	first we want a point on a cube or eventually a sphere
			build_point( dst );
			vec[0] = ( dst[0] - _offset[0] ) * _scale[0];
			vec[1] = ( dst[1] - _offset[1] ) * _scale[1];
			vec[2] = ( dst[2] - _offset[2] ) * _scale[2];
			vec[2] += _turb_speed * REAL(aaa::time::get());

			switch( _s_how_ui )
			{
			case TYPE_TURB:				val = turbulence			( vec, _turb_harm_nb );		break;
			case TYPE_TURB_IMPROVED:	val = turbulence_improved	( vec, _turb_harm_nb );		break;
			case TYPE_FRACSUM:			val = fractalsum			( vec, _turb_harm_nb );		break;
			case TYPE_FRACSUM_IMPROVED:	val = fractalsum_improved	( vec, _turb_harm_nb );		break;
			}

			if( is_val_break( val ) )
				break;
		}
		while( --create_try > 0 );
		dst[3] = val;

		if( create_try <= 0 )
			retcode = _b_try_always;
		
		dst += dst_stride;
	}
	return retcode;
}

FINLINE	bool	c_placer::compute_field( UINT32 nb, float* dst, UINT32 dst_stride, float* src, UINT32 src_stride ) CONST
{
	REAL CONST OX = REAL(123.456);
	REAL CONST OY = REAL(54.321);
	REAL CONST OZ = REAL(9.876);
	REAL	vec[3];
	switch( _s_how_ui )
	{
	case TYPE_FIELD_TURB:
		for( UINT32 id=nb; id>0; --id )
		{
			sub_then_mul_v3( vec, src, _offset, _scale );
			src += src_stride;
			dst[0] = turbulence( vec, _turb_harm_nb );
			vec[0] += OX;
			dst[1] = turbulence( vec, _turb_harm_nb );
			vec[1] += OY;
			dst[2] = turbulence( vec, _turb_harm_nb );
			vec[2] += OZ;
			dst[3] = turbulence( vec, _turb_harm_nb );
			dst += dst_stride;
		}
		break;
	case TYPE_FIELD_FRACSUM:
		for( UINT32 id=nb; id>0; --id )
		{
			sub_then_mul_v3( vec, src, _offset, _scale );	
			src += src_stride;
			dst[0] = fractalsum( vec, _turb_harm_nb );
			vec[0] += OX;
			dst[1] = fractalsum( vec, _turb_harm_nb );
			vec[1] += OY;
			dst[2] = fractalsum( vec, _turb_harm_nb );
			vec[2] += OZ;
			dst[3] = fractalsum( vec, _turb_harm_nb );
			dst += dst_stride;
		}
		break;
	case TYPE_FIELD_TURB_IMPROVED:
		for( UINT32 id=nb; id>0; --id )
		{
			sub_then_mul_v3( vec, src, _offset, _scale );	
			src += src_stride;
			dst[0] = turbulence_improved( vec, _turb_harm_nb );
			vec[0] += OX;
			dst[1] = turbulence_improved( vec, _turb_harm_nb );
			vec[1] += OY;
			dst[2] = turbulence_improved( vec, _turb_harm_nb );
			vec[2] += OZ;
			dst[3] = turbulence_improved( vec, _turb_harm_nb );
			dst += dst_stride;
		}
		break;
	case TYPE_FIELD_FRACSUM_IMPROVED:
		for( UINT32 id=nb; id>0; --id )
		{
			sub_then_mul_v3( vec, src, _offset, _scale );	
			src += src_stride;
			dst[0] = fractalsum_improved( vec, _turb_harm_nb );
			vec[0] += OX;
			dst[1] = fractalsum_improved( vec, _turb_harm_nb );
			vec[1] += OY;
			dst[2] = fractalsum_improved( vec, _turb_harm_nb );
			vec[2] += OZ;
			dst[3] = fractalsum_improved( vec, _turb_harm_nb );
			dst += dst_stride;
		}
		break;
	}
	return true;
}

#if AAA_REAL_IS_DOUBLE()
FINLINE	bool	c_placer::compute_field( UINT32 nb, float* dst, UINT32 dst_stride, REAL* src, UINT32 src_stride ) CONST
{
	REAL	vec[3];
	switch( _s_how_ui )
	{
	case TYPE_FIELD_TURB:
		for( UINT32 id=nb; id>0; --id )
		{
			sub_then_mul_v3( vec, src, _offset, _scale );
			src += src_stride;
			dst[0] = turbulence( vec, _turb_harm_nb );
			vec[0] += 123.456;
			dst[1] = turbulence( vec, _turb_harm_nb );
			vec[1] += 54.321;
			dst[2] = turbulence( vec, _turb_harm_nb );
			vec[2] += 9.876;
			dst[3] = turbulence( vec, _turb_harm_nb );
			dst += dst_stride;
		}
		break;
	case TYPE_FIELD_FRACSUM:
		for( UINT32 id=nb; id>0; --id )
		{
			sub_then_mul_v3( vec, src, _offset, _scale );	
			src += src_stride;
			dst[0] = fractalsum( vec, _turb_harm_nb );
			vec[0] += 123.456;
			dst[1] = fractalsum( vec, _turb_harm_nb );
			vec[1] += 54.321;
			dst[2] = fractalsum( vec, _turb_harm_nb );
			vec[2] += 9.876;
			dst[3] = fractalsum( vec, _turb_harm_nb );
			dst += dst_stride;
		}
		break;
	case TYPE_FIELD_TURB_IMPROVED:
		for( UINT32 id=nb; id>0; --id )
		{
			sub_then_mul_v3( vec, src, _offset, _scale );	
			src += src_stride;
			dst[0] = turbulence_improved( vec, _turb_harm_nb );
			vec[0] += 123.456;
			dst[1] = turbulence_improved( vec, _turb_harm_nb );
			vec[1] += 54.321;
			dst[2] = turbulence_improved( vec, _turb_harm_nb );
			vec[2] += 9.876;
			dst[3] = turbulence_improved( vec, _turb_harm_nb );
			dst += dst_stride;
		}
		break;
	case TYPE_FIELD_FRACSUM_IMPROVED:
		for( UINT32 id=nb; id>0; --id )
		{
			sub_then_mul_v3( vec, src, _offset, _scale );	
			src += src_stride;
			dst[0] = fractalsum_improved( vec, _turb_harm_nb );
			vec[0] += 123.456;
			dst[1] = fractalsum_improved( vec, _turb_harm_nb );
			vec[1] += 54.321;
			dst[2] = fractalsum_improved( vec, _turb_harm_nb );
			vec[2] += 9.876;
			dst[3] = fractalsum_improved( vec, _turb_harm_nb );
			dst += dst_stride;
		}
		break;
	}
	return true;
}
#endif

bool	c_placer::place_one( INT32 id, float* dst  ) CONST
{	
	switch( _s_how_ui )
	{
	case TYPE_NO:						return place_no(		1, dst, 0  );
	case TYPE_RND:						return place_random(	1, dst, 0  );
	case TYPE_LUA:						//	Always return 4 numbers by calling lua
										g_lua_wrap_cur->do_fn_fn_pass_i( nullptr, nullptr, _lua_fn.get(), id, dst, 4 );
										return true;
	case TYPE_RED:
	case TYPE_GREEN:
	case TYPE_BLUE:
	case TYPE_ALPHA:
	case TYPE_GREY:						return place_compo(		1, dst, 0  );

	case TYPE_TURB:
	case TYPE_FRACSUM:
	case TYPE_TURB_IMPROVED:
	case TYPE_FRACSUM_IMPROVED:			return place_noise(		1, dst, 0  );

	case TYPE_FIELD_TURB:
	case TYPE_FIELD_FRACSUM:
	case TYPE_FIELD_TURB_IMPROVED:
	case TYPE_FIELD_FRACSUM_IMPROVED:
											{
												REAL src[3];
												cpy_v3( src, dst );
												return compute_field(	1, dst, 0, src, 0  );

											}
	}
	return false;
}

#if AAA_REAL_IS_DOUBLE()
//avoid these double case
bool	c_placer::place_one( INT32 id, REAL* dst  ) CONST
{	
	float d32[4];
	cpy_v4( d32, dst );
	bool	b;
	switch( _s_how_ui )
	{
	case TYPE_NO:						b = place_no(		1, d32, 0  );
	case TYPE_RND:						b = place_random(	1, d32, 0  );
	case TYPE_LUA:						//	Always return 4 numbers by calling lua
										g_lua_wrap_cur->do_fn_fn_pass_i( nullptr, nullptr, _lua_fn.get(), id, d32, 4 );
										b = true;
	case TYPE_RED:
	case TYPE_GREEN:
	case TYPE_BLUE:
	case TYPE_ALPHA:
	case TYPE_GREY:						b = place_compo(		1, d32, 0  );

	case TYPE_TURB:
	case TYPE_FRACSUM:
	case TYPE_TURB_IMPROVED:
	case TYPE_FRACSUM_IMPROVED:			b = place_noise(		1, d32, 0  );

	case TYPE_FIELD_TURB:
	case TYPE_FIELD_FRACSUM:
	case TYPE_FIELD_TURB_IMPROVED:
	case TYPE_FIELD_FRACSUM_IMPROVED:	b = compute_field(	1, d32, 0, dst, 0  );

	default: b = false;
	}
	cpy_v4( dst, d32 );
	return b;
}
#endif

bool c_placer::place( UINT32 nb, float* dst, UINT32 dst_stride, REAL* src, UINT32 src_stride ) CONST
{
	switch( _s_how_ui )
	{
	case TYPE_NO:						return place_no(		nb, dst, dst_stride );
	case TYPE_RND:						return place_random(	nb, dst, dst_stride );
	case TYPE_LUA:						return place_lua(		nb, dst, dst_stride );

	case TYPE_RED:
	case TYPE_GREEN:
	case TYPE_BLUE:
	case TYPE_ALPHA:
	case TYPE_GREY:						return place_compo(		nb, dst, dst_stride );

	case TYPE_TURB:
	case TYPE_FRACSUM:
	case TYPE_TURB_IMPROVED:
	case TYPE_FRACSUM_IMPROVED:			return place_noise(		nb, dst, dst_stride );

	case TYPE_FIELD_TURB:
	case TYPE_FIELD_FRACSUM:
	case TYPE_FIELD_TURB_IMPROVED:
	case TYPE_FIELD_FRACSUM_IMPROVED:	return compute_field(	nb, dst, dst_stride, src, src_stride  );
	}
	return false;
}

#if AAA_REAL_IS_DOUBLE()
bool c_placer::place( UINT32 nb, float* dst, UINT32 dst_stride, float* src, UINT32 src_stride ) CONST
{
	switch( _s_how_ui )
	{
	case TYPE_NO:						return place_no(		nb, dst, dst_stride );
	case TYPE_RND:						return place_random(	nb, dst, dst_stride );
	case TYPE_LUA:						return place_lua(		nb, dst, dst_stride );

	case TYPE_RED:
	case TYPE_GREEN:
	case TYPE_BLUE:
	case TYPE_ALPHA:
	case TYPE_GREY:						return place_compo(		nb, dst, dst_stride );

	case TYPE_TURB:
	case TYPE_FRACSUM:
	case TYPE_TURB_IMPROVED:
	case TYPE_FRACSUM_IMPROVED:			return place_noise(		nb, dst, dst_stride );

	case TYPE_FIELD_TURB:
	case TYPE_FIELD_FRACSUM:
	case TYPE_FIELD_TURB_IMPROVED:
	case TYPE_FIELD_FRACSUM_IMPROVED:	return compute_field(	nb, dst, dst_stride, src, src_stride  );
	}
	return false;
}
#endif