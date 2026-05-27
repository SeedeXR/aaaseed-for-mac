#include "obj_ui/bdd/bdd_geo/bdd_proj_cone.h"
#include "obj_ui/deformer/def_node.h"
#include "draw/map.h"
#include "draw/model.h"
#include "draw/seedcam.h"
#include "gol/gol_draw.h"

FACTORY_CREATE_PROP_V1( c_bdd_proj_cone, bdd_proj_cone, Projection Cone, bdd_proj_cone, sub_menu="Helper"; );

namespace	n_bdd_proj_cone
{
	CONSTEXPR INT32	BASE_PARAM_NB	=	14 + c_bdd::GEO_PARAM_NB;
	CONSTEXPR INT32	PARAM_NB_MAX	=	BASE_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		BDD_BASE_PARAMS

		PARAM_DEF_POINT_FP32_XYZ(	src )
		PARAM_DEF_FP32_ZERO(		src_interpolation )
		PARAM_DEF_BOOL_OFF(			src_draw_cap )

		PARAM_DEF_BOOL_OFF(			dst_in_camera_space )
		PARAM_DEF_POINT_FP32_XYZ(	dst )
		PARAM_DEF_SCALE_FP32_XY(	dst_size )
		PARAM_DEF_FP32_ONE(			dst_interpolation )
		PARAM_DEF_BOOL_OFF(			dst_draw_cap )

		PARAM_DEF_BOOL_OFF(			side_draw )
	}; 
}

void	c_bdd_proj_cone::param_init_pt()
{
	INT32	h = param_init_pt_geo();

	param_set_pt_3( h, _src );
	param_set_pt( h, _src_interpolation );
	param_set_pt( h, _b_src_cap );

	param_set_pt( h, _b_dst_in_camera );
	param_set_pt_3( h, _dst );
	param_set_pt( h, _dst_size_x );
	param_set_pt( h, _dst_size_y );
	param_set_pt( h, _dst_interpolation );
	param_set_pt( h, _b_dst_cap );

	param_set_pt( h, _b_side_draw );

	err_param_init_pt(h);
}

void c_bdd_proj_cone::init()
{
	param_init_with( n_bdd_proj_cone::param, n_bdd_proj_cone::PARAM_NB_MAX ); //  bdd_proj_cone_param, BDD_PROJ_CONE_PARAM_NB_MAX);
}

void c_bdd_proj_cone::alloc()
{
}

void c_bdd_proj_cone::dealloc()
{
}

CONSTRUCTOR_CREATE(c_bdd_proj_cone)
{
	init();
	alloc();
}

c_bdd_proj_cone::~c_bdd_proj_cone()
{
	dealloc();
}

void c_bdd_proj_cone::build()
{
}

void	c_bdd_proj_cone::update()
{
	_axe = c_model::cur->get_axe();
	axe_build_index( _i_u, _i_v, _axe );

	cpy_v3( _point[0], _dst );
	cpy_v3( _point[1], _dst );
	cpy_v3( _point[2], _dst );
	cpy_v3( _point[3], _dst );

	_point[0][_i_u] -= _dst_size_x * REAL(.5);
	_point[1][_i_u] += _dst_size_x * REAL(.5);
	_point[2][_i_u] += _dst_size_x * REAL(.5);
	_point[3][_i_u] -= _dst_size_x * REAL(.5);

	_point[0][_i_v] += _dst_size_y * REAL(.5);
	_point[1][_i_v] += _dst_size_y * REAL(.5);
	_point[2][_i_v] -= _dst_size_y * REAL(.5);
	_point[3][_i_v] -= _dst_size_y * REAL(.5);

	if( _b_dst_in_camera )
	{
		c_seedcam* cam = c_seedcam::get_cur();
		cam->coor_camera_to_world( _point[0] );
		cam->coor_camera_to_world( _point[1] );
		cam->coor_camera_to_world( _point[2] );
		cam->coor_camera_to_world( _point[3] );
	}

	interpolate_v3( _proj[0], _src, _point[0], _src_interpolation );
	interpolate_v3( _proj[1], _src, _point[1], _src_interpolation );
	interpolate_v3( _proj[2], _src, _point[2], _src_interpolation );
	interpolate_v3( _proj[3], _src, _point[3], _src_interpolation );

	interpolate_v3( _targ[0], _src, _point[0], _dst_interpolation );
	interpolate_v3( _targ[1], _src, _point[1], _dst_interpolation );
	interpolate_v3( _targ[2], _src, _point[2], _dst_interpolation );
	interpolate_v3( _targ[3], _src, _point[3], _dst_interpolation );
}

void c_bdd_proj_cone::draw()
{
//	bool	use_normal = is_normal_draw();
	if( c_map::get_cur()->is_implicit() )
	{
		FP32 CONST * uv = c_map::get_cur()->get_quad_uv();
		if( _b_dst_cap )
		{
			GOL::begin( GL_TRIANGLE_STRIP );
				GOL::texcoord2v( uv     );
				GOL::vertex3v( _targ[0] );
				GOL::texcoord2v( uv + 2 );
				GOL::vertex3v( _targ[3] );
				GOL::texcoord2v( uv + 4 );
				GOL::vertex3v( _targ[1] );
				GOL::texcoord2v( uv + 6 );
				GOL::vertex3v( _targ[2] );
			GOL::end();
		}
		if( _b_side_draw )
		{
			GOL::begin( GL_TRIANGLE_STRIP );
				GOL::texcoord2v( uv     );
				GOL::vertex3v( _proj[0] );
				GOL::vertex3v( _targ[0] );

				GOL::texcoord2v( uv + 2 );
				GOL::vertex3v( _proj[3] );
				GOL::vertex3v( _targ[3] );

				GOL::texcoord2v( uv + 6 );
				GOL::vertex3v( _proj[2] );
				GOL::vertex3v( _targ[2] );

				GOL::texcoord2v( uv + 4 );
				GOL::vertex3v( _proj[1] );
				GOL::vertex3v( _targ[1] );

				GOL::texcoord2v( uv     );
				GOL::vertex3v( _proj[0] );
				GOL::vertex3v( _targ[0] );
			GOL::end();
		}
		if( _b_src_cap )
		{
			GOL::begin( GL_TRIANGLE_STRIP );
				GOL::texcoord2v( uv     );
				GOL::vertex3v( _proj[0] );
				GOL::texcoord2v( uv + 2 );
				GOL::vertex3v( _proj[3] );
				GOL::texcoord2v( uv + 4 );
				GOL::vertex3v( _proj[1] );
				GOL::texcoord2v( uv + 6 );
				GOL::vertex3v( _proj[2] );
			GOL::end();
		}
	}
	else
	{
		if( _b_dst_cap )
		{
			GOL::begin( GL_TRIANGLE_STRIP );
				GOL::vertex3v( _targ[0] );
				GOL::vertex3v( _targ[3] );
				GOL::vertex3v( _targ[1] );
				GOL::vertex3v( _targ[2] );
			GOL::end();
		}
		if( _b_side_draw )
		{
			GOL::begin( GL_TRIANGLE_STRIP );
				GOL::vertex3v( _proj[0] );
				GOL::vertex3v( _targ[0] );

				GOL::vertex3v( _proj[3] );
				GOL::vertex3v( _targ[3] );

				GOL::vertex3v( _proj[2] );
				GOL::vertex3v( _targ[2] );

				GOL::vertex3v( _proj[1] );
				GOL::vertex3v( _targ[1] );

				GOL::vertex3v( _proj[0] );
				GOL::vertex3v( _targ[0] );
			GOL::end();
		}
		if( _b_src_cap )
		{
			GOL::begin( GL_TRIANGLE_STRIP );
				GOL::vertex3v( _proj[0] );
				GOL::vertex3v( _proj[3] );
				GOL::vertex3v( _proj[1] );
				GOL::vertex3v( _proj[2] );
			GOL::end();
		}
	}
}


