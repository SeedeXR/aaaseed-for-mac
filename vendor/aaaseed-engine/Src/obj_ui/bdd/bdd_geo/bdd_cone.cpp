#include "obj_ui/bdd/bdd_geo/bdd_cone.h"
#include "draw/model.h"


FACTORY_CREATE_PROP_V1( c_bdd_cone, bdd_cone, Cone, cone, sub_menu="Geometry"; );

namespace n_bdd_cone
{
	CONSTEXPR INT32 BASE_PARAM_NB		= c_bdd::GEO_PARAM_NB + 6 + c_bdd_uv::PARAM_UV_TURN_NB;
	CONSTEXPR INT32 INCOMPLETE_PARAM_NB = 1;
	CONSTEXPR INT32 GROUP_PARAM_NB		= 1;
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_PARAM_NB
									+	INCOMPLETE_PARAM_NB
									+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		BDD_BASE_PARAMS
		PARAM_DEF_POINT_XYZ(	center				)
		PARAM_DEF_REAL_ZERO(	radius_top			)
		PARAM_DEF_INT32(		nb_u,				32,24,	1,c_bdd_uv::MAX_ELT_NB	)
		PARAM_DEF_INT32(		nb_v,				32,2,	1,c_bdd_uv::MAX_ELT_NB	)
		PARAM_DEF_BDD_UV_TURN()

		PARAM_DEF_GROUP_CLOSED(	Incomplete,			INCOMPLETE_PARAM_NB )
			PARAM_DEF_REAL_INF(	lissajous_ratio,	2, 1	)
			//PARAM_DEF_BOOL_ON( cap_top )
			//PARAM_DEF_BOOL_ON( cap_bottom )
		};
}

void	c_bdd_cone::param_init_pt()
{
	INT32	h = param_init_pt_geo();
	param_set_pt_center(	h );
	param_set_pt(			h, _radius_top_ui );
	param_set_pt_nb(		h );
	param_set_pt_uv_turn(	h );

	++h;
		param_set_pt( h, _lissajous_ratio_ui );
		//	param_set_pt( h, _cap_top_ui );
		//	param_set_pt( h, _cap_bottom_ui );

	err_param_init_pt( h );
}

void c_bdd_cone::init()
{
	param_init_with( n_bdd_cone::param, n_bdd_cone::PARAM_NB_MAX );
}

CONSTRUCTOR_CREATE( c_bdd_cone )
{
	_b_axe_logic_vert = false;
	init();
	_radius_top = -_radius_top_ui;
	_lissajous_ratio = -_lissajous_ratio_ui;
//	cap_top = !cap_top_ui;
//	cap_bottom = !cap_bottom_ui;
}

c_bdd_cone::~c_bdd_cone()
{
}

void	c_bdd_cone::build_geo_validate()
{
	c_bdd_uv::build_geo_validate();
	_radius_top = _radius_top_ui;
	_lissajous_ratio = _lissajous_ratio_ui;
//	cap_top_ui = cap_top;
//	cap_bottom_ui = cap_bottom;
}

bool	c_bdd_cone::build_geo_is_needed()
{	
	return		c_bdd_uv::build_geo_is_needed()
			||	_radius_top != _radius_top_ui
			||	_lissajous_ratio != _lissajous_ratio_ui
//			||	cap_top_ui != cap_top
//			||	cap_bottom_ui != cap_bottom
			;
}

void c_bdd_cone::build_geo()
{
	build_geo_validate();

	REAL	r;
	REAL	cu,su;

	REAL	u_begin;
	REAL	u_max;
	REAL	du;
	build_umin_umax_du( u_begin, u_max, du );

	REAL	size[3];
	c_model::cur->get_size_v3( size );
	scale_v2( size, .5 );

	REAL	v_begin;
	REAL	dv;
	build_v_dv( v_begin, dv );

	//todo make sure the normal are really right
	//	this look ok but that should be wrong


	//we increment this way (opposite to memory) not to compute sinus and cosinus at each iteration
	//todo do other code for the turning version
	REAL v;
	REAL dr = _radius_top - REAL(1);

	REAL r_start = v_begin * dr + ( REAL(1) + _radius_top ) * REAL(.5);
	dr *= dv;

	dv *= size[2];
	v_begin = size[2] * v_begin + _center_ui[_i_axe];

	REAL d_turn = _turn_ui / ( _nb_v_ui==1 ? REAL(1) : (_nb_v_ui-1) );
//	d_turn *= _u_range_ui;

	INT32 stride = _nb_u_ui * 3;

	if( d_turn == 0. )
	{
		REAL u = u_begin;
		REAL normal[3];
		for( INT32 iu = 0; iu < _nb_u_ui; ++iu )
		{
			REAL* p_point  = _point  + iu * 3;
			REAL* p_normal = _normal + iu * 3;

			if( iu == _nb_u_ui-1 )
				u = u_max;

			cu = COS_TURN( u );
			su = SIN_TURN( u * _lissajous_ratio );
		
			normal[0] = cu * size[2];
			normal[1] = su * size[2];
			cu *= size[0];
			su *= size[1];
			normal[2] = (REAL(1) - _radius_top) * norm_v2r( cu , su );
			normalize_v3r( normal );

			v = v_begin;
			r = r_start;
			for( INT32 iv = _nb_v_ui - 1; iv >= 0; --iv )
			{
				p_normal[_i_u]		= normal[0];
				p_point[_i_u]		= r * cu + _center_ui[_i_u];

				p_normal[_i_v]		= normal[1];
				p_point[_i_v]		= r * su + _center_ui[_i_v];

				p_normal[_i_axe]	= normal[2];
				p_point[_i_axe]		= v;

				p_point += stride;
				p_normal += stride;

				v += dv;
				r += dr;
			}
			u += du;
		}
	}
	else
	{
		REAL normal[3];

		REAL* p_point  = _point;
		REAL* p_normal = _normal;

		v = v_begin;
		r = r_start;
		for( INT32 iv = _nb_v_ui - 1; iv >= 0; --iv )
		{
			REAL u = u_begin;
			for( INT32 iu = 0; iu < _nb_u_ui; ++iu )
			{
				if( iu == _nb_u_ui-1 )
					u = u_max;

				cu = COS_TURN( u );
				su = SIN_TURN( u * _lissajous_ratio );
			
				normal[0] = cu * size[2];
				normal[1] = su * size[2];
				cu *= size[0];
				su *= size[1];
				normal[2] = (REAL(1) - _radius_top) * norm_v2r( cu , su );
				normalize_v3r( normal );

				p_normal[_i_u]		= normal[0];
				p_point[_i_u]		= r * cu + _center_ui[_i_u];

				p_normal[_i_v]		= normal[1];
				p_point[_i_v]		= r * su + _center_ui[_i_v];

				p_normal[_i_axe]	= normal[2];
				p_point[_i_axe]		= v;

				p_point += 3;
				p_normal += 3;

				u += du;
			}
			v += dv;
			r += dr;

			u_begin += d_turn;
			u_max += d_turn;
		}
	}

}

//todo we should have compute_point_2d and compute_point_3d for optimisation
void c_bdd_cone::get_point_from_uv( REAL* dst, REAL u, REAL v )
{
	u = _u_center_ui + _u_range_ui * u;
	v = _v_center_ui + _v_range_ui * v;
	REAL	c = COS_TURN( u );
	REAL	s = SIN_TURN( u * _lissajous_ratio );
	REAL	r = interpolate(  REAL(1.), _radius_top, v ) * REAL(.5);

	dst[_i_u]		=	r * c *  _size_geo[0]	+ _center_ui[_i_u];
	dst[_i_v]		=	r * s *  _size_geo[0]	+ _center_ui[_i_v];
	dst[_i_axe]		=	(v-REAL(.5)) * _size_geo[2]	+ _center_ui[_i_axe];
}

