#include "def_bdd_uv.h"
#include "obj_ui/bdd/util/bdd_uv.h"
#include "obj_ui/bdd/bdd_edit/bdd_grid_adjustable.h"

FACTORY_INSTANCE_V1( c_def_bdd_uv, def_bdd_uv, Deformer bdd uv, def );

namespace n_def_bdd_uv
{
	CONSTEXPR INT32 BASE_PARAM_NB	= c_deformer::BASE_PARAM_NB + 6;
	CONSTEXPR INT32 GROUP_NB		= 0;
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_PARAM_NB
									+	GROUP_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		DEFORMER_BASE_PARAMS

		PARAM_DEF_REF(			bdd_name_symbo )

		PARAM_DEF_BOOL_OFF(		reveal_uv  )

		PARAM_DEF_REAL_ZERO(	left )
		PARAM_DEF_REAL_ONE(		right )
		PARAM_DEF_REAL_ZERO(	bottom )
		PARAM_DEF_REAL_ONE(		top )
	};
}

CONSTRUCTOR_CREATE( c_def_bdd_uv )
{
	_bdd_target = nullptr;
	_bdd_target_adjust = nullptr;

	init_name_with( "Interpolate" );
	param_init_with( n_def_bdd_uv::param, n_def_bdd_uv::PARAM_NB_MAX );
}

void c_def_bdd_uv::param_init_pt()
{
	INT32	h = SUPER::param_init_pt_start();

	param_set_pt_attach_obj( h, _target_name_symbo, _bdd_target );

	param_set_pt( h, _b_reveal_uv );

	param_set_pt_2( h, _u );
	param_set_pt_2( h, _v );

	err_param_init_pt(h);
}

c_def_bdd_uv::~c_def_bdd_uv()
{
}

void	c_def_bdd_uv::update()
{
	if( !_target_name_symbo.is_empty() )
	{
		if( !_bdd_target && !_bdd_target->is_name_symbo( _target_name_symbo ) || !_bdd_target->get_root() )
		{
			c_bdd* bdd = find_bdd_by_name_symbo( _target_name_symbo );
			if( bdd )
			{
				//hack
				if( _b_reveal_uv )
				{
					if( c_bdd_grid_adjustable::is_instance(bdd) )
						_bdd_target_adjust = (c_bdd_grid_adjustable*) bdd;
					else
						err_print( "%s() found a bdd but this not a bdd_grid_adjustable", __FUNCTION__ );
				}
				else
					_bdd_target_adjust = nullptr;

				if( bdd->is_inherited_from( "bdd_uv" ) )	//todo do it thru the factory and not the string
					_bdd_target = (c_bdd_uv*) bdd;
				else
					err_print( "%s() found a bdd but this not a bdd_uv", __FUNCTION__ );
			}
		}
	}
	else
		_bdd_target = nullptr;
	set_deforming( _bdd_target && _bdd_target->get_point_nb()!=0 );
}

void	c_def_bdd_uv::apply( REAL CONST * src, REAL* dst, INT32 nb )
{
	REAL	u = _u[0];
	REAL	v = _v[0];

	REAL	du = _u[1] - u;
	REAL	dv = _v[1] - v;

	//u -= .5;
	//v -= .5;

	if( _b_reveal_uv && _bdd_target_adjust )
	{
		REAL vec[3];
		REAL dst_u, dst_v;
		vec[2] = 0.;
		for( ; nb>0; --nb )
		{
			vec[0] = u + du * src[0];
			vec[1] = v + dv * src[1];
			if( _bdd_target_adjust->get_uv_from_pos( dst_u, dst_v, vec ) )
			{
				dst[0] = dst_u;
				dst[1] = dst_v;
				dst[2] = 0;
			}
			src += 3;
			dst += 3;
		}
	}
	else
	{
		for( ; nb>0; --nb )
		{
			_bdd_target->get_point_from_uv( dst, u + du*src[0], v + dv*src[1] );
			src += 3;
			dst += 3;
		}
	}
}
