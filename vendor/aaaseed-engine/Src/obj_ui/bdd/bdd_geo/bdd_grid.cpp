#include "obj_ui/bdd/bdd_geo/bdd_grid.h"
#include "obj_ui/deformer/def_node.h"
#include "draw/model.h"



FACTORY_CREATE_PROP_V1( c_bdd_grid, bdd_grid, Grid, grid, sub_menu="Geometry"; );

namespace n_bdd_grid
{
	CONSTEXPR INT32 BASE_PARAM_NB	= 7 + c_bdd_uv::PARAM_UV_TURN_NB + c_bdd::GEO_PARAM_NB;
	CONSTEXPR INT32 PARAM_NB_MAX	= BASE_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		BDD_BASE_PARAMS
		PARAM_DEF_POINT_XYZ(	center)

		PARAM_DEF_INT32(	nb_u,		2,16,	1,c_bdd_uv::MAX_ELT_NB	)
		PARAM_DEF_INT32(	nb_v,		2,16,	1,c_bdd_uv::MAX_ELT_NB	)
		PARAM_DEF_INT32(	nb_axe,		2,1,	1,c_bdd_uv::MAX_ELT_NB	)

		PARAM_DEF_BDD_UV_TURN()

		PARAM_DEF_BOOL_OFF(	draw_field	)
//		PARAM_DEF_BOOL_ON(	allow_along_u	)
	};
}

void	c_bdd_grid::param_init_pt()
{
	INT32 h = param_init_pt_geo();

	param_set_pt_center(	h );
	param_set_pt_nb_axe(	h );
	param_set_pt_uv_turn(	h );

	param_set_pt(	h, _b_field_ui );
//	param_set_pt(	h, _b_allow_along_u_ui );
	
	err_param_init_pt( h );
}

void c_bdd_grid::init()
{
	param_init_with( n_bdd_grid::param, n_bdd_grid::PARAM_NB_MAX );
}

CONSTRUCTOR_CREATE(c_bdd_grid )
{
	_b_axe_logic_vert = true;
	init();
}

c_bdd_grid::~c_bdd_grid()
{
}

void c_bdd_grid::build_grid( REAL z )
{
	REAL normal[3];
	normal[_i_u] = 0.;
	normal[_i_v] = 0.;
	normal[_i_axe] = _size[0] * _size[1] * _size[2] >= 0. ? REAL(1) : REAL(-1);

	REAL size = _i_axe == 0 ? -_size[0] : _size[0];
	REAL u_begin = _center_ui[_i_u];
	REAL du;
	if( _nb_u_ui > 1 )
	{
		du = size * _u_range_ui / REAL( _nb_u_ui - 1 );
		u_begin += size * ( _u_center_ui - _u_range_ui * REAL(.5));
	}
	else
		du = 0.;
	REAL d_turn = _turn_ui / ( _nb_v_ui==1 ? REAL(1) : (_nb_v_ui-1) );
	d_turn *= size * _u_range_ui;


	size = _size[1];
	REAL v = _center_ui[_i_v];
	REAL dv; 
	if( _nb_v_ui > 1 )
	{
		dv = size * _v_range_ui / REAL( _nb_v_ui - 1 );
		v += size * ( _v_center_ui - _v_range_ui * REAL(.5) );
	}
	else
		dv = 0.;


	REAL*	p_point = _point;
	REAL*	p_normal = _normal;
	//todo (2023 November) should we keep do it this way (u as the external loop)? is it the "tube" heritage where having a strip to do a "ring" was better ?
	// then we should be careful with compute normal uv and all the bdd_uv descendants

	for( INT32 iv = _nb_v_ui; iv > 0; --iv )
	{
		REAL u = u_begin;
		for( INT32 iu = _nb_u_ui; iu > 0; --iu )
		{
			p_point[_i_u] = u;
			p_point[_i_v] = v;
			p_point[_i_axe] = z;
			p_point += 3;

			cpy_v3( p_normal, normal );
			p_normal += 3;

			u += du;
		}
		v += dv;
		u_begin += d_turn;
	}

	build_geo_validate();
}

void c_bdd_grid::build_geo()
{
	build_grid( _center_ui[_i_axe] );
}

void	c_bdd_grid::update()
{
	update_uvw();
	if(	build_geo_is_needed() )
		build_grid( _center_ui[_i_axe] );

	if( _nb_axe_ui == 1 )
		do_deform_and_normal();

	if( c_multiple::cur )
		c_multiple::cur->set_nb( _nb_u_geo, _nb_v_geo, _nb_axe_ui );
}
