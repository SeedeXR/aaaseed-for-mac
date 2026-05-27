#include "bdd_torus.h"
#include "draw/model.h"


FACTORY_CREATE_PROP_V1( c_bdd_torus, bdd_torus, Torus, torus, sub_menu="Geometry"; );

namespace n_bdd_torus
{
	CONSTEXPR INT32	BASE_PARAM_NB	=  c_bdd::GEO_PARAM_NB + 6 + c_bdd_uv::PARAM_UV_TURN_NB;
	CONSTEXPR INT32	PARAM_NB_MAX	= BASE_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		BDD_BASE_PARAMS
		PARAM_DEF_POINT_XYZ(	center		)
		PARAM_DEF_REAL_ONE(		radius_int	)
		PARAM_DEF_INT32(		nb_u,		32,12,	1,c_bdd_uv::MAX_ELT_NB )
		PARAM_DEF_INT32(		nb_v,		32,12,	1,c_bdd_uv::MAX_ELT_NB )
		PARAM_DEF_BDD_UV_TURN()
	};
}
void	c_bdd_torus::param_init_pt()
{
	INT32 h = param_init_pt_geo();
	param_set_pt_center(	h );
	param_set_pt(			h, _radius_int	);
	param_set_pt_nb(		h );
	param_set_pt_uv_turn(	h );

	err_param_init_pt( h );
}

void c_bdd_torus::init()
{
	param_init_with( n_bdd_torus::param, n_bdd_torus::PARAM_NB_MAX );
}

CONSTRUCTOR_CREATE( c_bdd_torus )
,_radius_int_geo(REAL(4545.549))
{
	_b_axe_logic_vert = false;
	init();
}

c_bdd_torus::~c_bdd_torus()
{
}

void	c_bdd_torus::build_geo_validate()
{
	c_bdd_uv::build_geo_validate();
	_radius_int_geo	= _radius_int;
}

bool	c_bdd_torus::build_geo_is_needed()
{	
	return	c_bdd_uv::build_geo_is_needed()
		||	_radius_int_geo		!=	_radius_int;
}

#define AAA_PRECISE 0	// tried to solve bad stitching (worked but instead add v_max 2025 September Mâa)
void c_bdd_torus::build_geo()
{
#if AAA_PRECISE
#	define GET_SIN_COS GET_SIN_COS_TURN_DOUBLE
	DOUBLE	cu,su;
	DOUBLE	cv,sv;
#else
#	define GET_SIN_COS GET_SIN_COS_TURN
	REAL	cu,su;
	REAL	cv,sv;
#endif
	REAL	u,v;
	REAL	du,dv;
	REAL	u_begin;

	REAL	size[3];
	
	REAL	u_max,v_max;
	REAL	rad_int;
	REAL	rad_int_axe;
	REAL	rad_u;

	REAL*	p_point = _point;
	REAL*	p_normal = _normal;

	build_umin_umax_du( u_begin, u_max, du );
	build_vmin_vmax_dv( v, v_max, dv );

	c_model::cur->get_size_half_v3( size );

	rad_int_axe = size[2];
	rad_int = _radius_int * size[0];
	rad_u = size[0] - rad_int;

	REAL d_turn = _turn_ui / ( _nb_v_ui==1 ? REAL(1) : REAL(_nb_v_ui-1) );
	//d_phase *= _u_range_ui;

	for( INT32 iv = _nb_v_ui; iv > 0; --iv )
	{
		GET_SIN_COS( sv, cv, v );
		REAL tmp1 = cv * rad_int_axe;
		REAL tmp2 = cv * rad_int + rad_u;
		REAL tmp3 = sv * rad_int;
		REAL val_axe = sv * rad_int_axe	+ _center_ui[_i_axe];
		u = u_begin; 
		for( INT32 iu = _nb_u_ui; iu > 0; --iu )
		{
			if( iu == 1 )
				u = u_max;
			GET_SIN_COS( su, cu, u );
	
			p_normal[_i_u]		= cu * tmp1;
			p_normal[_i_v]		= su * tmp1;
			p_normal[_i_axe]	= tmp3;

			p_point[_i_u]		= cu * tmp2	+ _center_ui[_i_u];
			p_point[_i_v]		= su * tmp2 + _center_ui[_i_v];
			p_point[_i_axe]		= val_axe;

			p_point  += 3;
			p_normal += 3;
			u += du;
		}		
		if( iv==2 )	// make sure to stich
			v = v_max;
		else
			v += dv;
		u_begin += d_turn;
		u_max += d_turn;
	}
	build_geo_validate();
}

