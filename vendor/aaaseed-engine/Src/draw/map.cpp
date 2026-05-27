#include "draw/map.h"

#include "infrastructure/param/param_declare.h"
#include "draw/axe.h"
#include "ui/strsymbo.h"
#include "infrastructure/layer/layers.h"
#include "obj_ui/bdd/util/bdd_uv.h"
#include "draw/stencil.h"
#include "media/video/tex_video.h"
#include "draw/seedcam.h"
#include "obj_ui/bdd/bdd_image/bdd_feedback.h"
#include "gol/gol_color.h"


FACTORY_CREATE_V1( c_map, mapping, Mapping, map );

c_map*	c_map::def	= nullptr;
c_map*	c_map::cur	= nullptr;
c_map*	c_map::ui	= nullptr;

bool			c_map::b_lod_allow_ui;
UVx4 CONST	c_map::quad_uv_01			=	{	{0,1},		{0,0},		{1,1},		{1,0} };
//no sure this is good
UVx4 CONST	c_map::quad_uv_01_flipped	=	{	{0,0},		{1,0},		{0,1},		{1,1} };
UVWx8 CONST	c_map::box_uvw_01			=	{	{0,0,0},	{0,1,0},	{1,1,0},	{1,0,0},
												{0,0,1},	{0,1,1},	{1,1,1},	{1,0,1}
											};

namespace{
	CONSTEXPR	UINT32	TEX_ENV_MODE_NB = 5;

	CONSTEXPR GLint	tex_env_mode_array[ TEX_ENV_MODE_NB ] =
	{
		GL_MODULATE,
		GL_REPLACE,
		GL_ADD,
		GL_BLEND,
		GL_DECAL,
	};
	CONSTEXPR C_PCHAR_C blend_tex_env_mode_str[ TEX_ENV_MODE_NB*2 ] =
	{
		"MODULATE",
		"REPLACE",
		"ADD",
		"BLEND",
		"DECAL",

		"GL_MODULATE",
		"GL_REPLACE",
		"GL_ADD",
		"GL_BLEND",
		"GL_DECAL",
	};

	CONSTEXPR C_PCHAR_C tex_dim_str[4] =
	{
		"NO",
		"1D",
		"2D",
		"3D"
	};

	CONSTEXPR C_PCHAR_C tex_mode_str[c_map::TEX_PROJ_NB] =
	{
		"OBJECT",
		"EYE",
		"SPHERE",
		"REFLECTION",
		"NORMAL",
		"CAMERA",
		"CAMERA_MODELVIEW",
	};

	CONSTEXPR INT32 blend_array[] =
	{
		GL_ZERO,				GL_ONE,
		GL_SRC_COLOR,			GL_ONE_MINUS_SRC_COLOR,			GL_DST_COLOR,			GL_ONE_MINUS_DST_COLOR,
		GL_SRC_ALPHA,			GL_ONE_MINUS_SRC_ALPHA,			GL_DST_ALPHA,			GL_ONE_MINUS_DST_ALPHA,
		GL_CONSTANT_COLOR,		GL_ONE_MINUS_CONSTANT_COLOR,	GL_CONSTANT_ALPHA,		GL_ONE_MINUS_CONSTANT_ALPHA,
		GL_SRC_ALPHA_SATURATE,
		GL_SRC1_COLOR,			GL_ONE_MINUS_SRC1_COLOR,		GL_SRC1_ALPHA,			GL_ONE_MINUS_SRC1_ALPHA
	};
	CONSTEXPR C_PCHAR_C	blend_str[] =
	{
		"ZERO",					"ONE",
		"SRC_COLOR",			"ONE_MINUS_SRC_COLOR",		"DST_COLOR",		"ONE_MINUS_DST_COLOR",
		"SRC_ALPHA",			"ONE_MINUS_SRC_ALPHA",		"DST_ALPHA",		"ONE_MINUS_DST_ALPHA",
		"CONSTANT_COLOR",		"ONE_MINUS_CONSTANT_COLOR",	"CONSTANT_ALPHA",	"ONE_MINUS_CONSTANT_ALPHA",
		"SRC_ALPHA_SATURATE",
		"SRC1_COLOR",			"ONE_MINUS_SRC1_COLOR",		"SRC1_ALPHA",		"ONE_MINUS_SRC1_ALPHA",

		"GL_ZERO",				"GL_ONE",
		"GL_SRC_COLOR",			"GL_ONE_MINUS_SRC_COLOR",		"GL_DST_COLOR",			"GL_ONE_MINUS_DST_COLOR",
		"GL_SRC_ALPHA",			"GL_ONE_MINUS_SRC_ALPHA",		"GL_DST_ALPHA",			"GL_ONE_MINUS_DST_ALPHA",
		"GL_CONSTANT_COLOR",	"GL_ONE_MINUS_CONSTANT_COLOR",	"GL_CONSTANT_ALPHA",	"GL_ONE_MINUS_CONSTANT_ALPHA",
		"GL_SRC_ALPHA_SATURATE",
		"GL_SRC1_COLOR",		"GL_ONE_MINUS_SRC1_COLOR",		"GL_SRC1_ALPHA",		"GL_ONE_MINUS_SRC1_ALPHA"
	};


	CONSTEXPR C_PCHAR_C blend_equation_str[GOL::BLEND_MAX_NB*2] =
	{
		"MIN",
		"MAX",
		"ADD",
		"SUBTRACT",
		"REVERSE_SUBTRACT",
		"MULTIPLY",
		"SCREEN",
		"OVERLAY",
		"DARKEN",
		"LIGHTEN",
		"COLORDODGE",
		"COLORBURN",
		"HARDLIGHT",
		"SOFTLIGHT",
		"DIFFERENCE",
		"EXCLUSION",
		"HSL_HUE",
		"HSL_SATURATION",
		"HSL_COLOR",
		"HSL_LUMINOSITY",

		"GL_MIN",
		"GL_MAX",
		"GL_FUNC_ADD",
		"GL_FUNC_SUBTRACT",
		"GL_FUNC_REVERSE_SUBTRACT",
		"MULTIPLY",
		"SCREEN",
		"OVERLAY",
		"DARKEN",
		"LIGHTEN",
		"COLORDODGE",
		"COLORBURN",
		"HARDLIGHT",
		"SOFTLIGHT",
		"DIFFERENCE",
		"EXCLUSION",
		"HSL_HUE",
		"HSL_SATURATION",
		"HSL_COLOR",
		"HSL_LUMINOSITY"
	};
}

namespace	n_map
{
	GLenum CONSTEXPR wrap_array[5] =
	{
		GL_CLAMP_TO_BORDER,
		GL_CLAMP_TO_EDGE,
		GL_REPEAT,
		GL_MIRRORED_REPEAT,
		GL_MIRROR_CLAMP_TO_EDGE
	};

	CONSTEXPR C_PCHAR_C	wrap_str[ARRAY_SIZE(wrap_array)*2] =
	{
		"CLAMP_TO_BORDER",
		"CLAMP_TO_EDGE",
		"REPEAT",
		"MIRRORED_REPEAT",
		"MIRROR_CLAMP_TO_EDGE",

		"GL_CLAMP_TO_BORDER",
		"GL_CLAMP_TO_EDGE",
		"GL_REPEAT",
		"GL_MIRRORED_REPEAT",
		"GL_MIRROR_CLAMP_TO_EDGE"
	};

	CONSTEXPR INT32 CONST	CAMERA_METHOD_NB =	3;
	CONSTEXPR C_PCHAR_C	tex_camera[CAMERA_METHOD_NB] =
	{
		"No",
		"BY_NAME",
		"BY_INDEX",
	};

	CONSTEXPR INT32 BASE_PARAM_NB			= 5;
	CONSTEXPR INT32 MAPPING_PARAM_NB		= 4;
	CONSTEXPR INT32 WRAP_PARAM_NB			= 3;
	CONSTEXPR INT32 TEX_PARAM_NB			= 15;
	CONSTEXPR INT32 TEX_MORE_PARAM_NB		= 12;
	CONSTEXPR INT32 HEXA_PARAM_NB			= 7;
	CONSTEXPR INT32 FILTER_PARAM_NB			= 5;
	CONSTEXPR INT32 BLEND_PARAM_NB			= 11;
	CONSTEXPR INT32 BLEND_MORE_PARAM_NB		= 13;
	CONSTEXPR INT32 TEX_ENV_PARAM_NB		= 6;
	//CONSTEXPR INT32 COMBINE_PARAM_NB		= 16;
	CONSTEXPR INT32 TRANSFO_PARAM_NB		= 13;
	CONSTEXPR INT32 GROUP_PARAM_NB			= 9;
			  
	CONSTEXPR INT32 PARAM_NB_MAX	= 	BASE_PARAM_NB
										+	MAPPING_PARAM_NB
										+	HEXA_PARAM_NB
										+	TEX_PARAM_NB
										+	TEX_MORE_PARAM_NB
										+	WRAP_PARAM_NB
										+	FILTER_PARAM_NB
										+	BLEND_PARAM_NB
										+	BLEND_MORE_PARAM_NB
										+	TRANSFO_PARAM_NB
										+	GROUP_PARAM_NB;
//										+	2;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_REF(				name_symbo			)
		PARAM_DEF_SYMBO(			tex_use,			2, 0,	3, tex_dim_str )
		PARAM_DEF_BOOL_ON(			tex_implicit		)
		PARAM_DEF_SYMBO_PSTR_ZERO(	tex_mode,			tex_mode_str )
		PARAM_DEF_AXE_Y(			tex_axe				)

		PARAM_DEF_GROUP_CLOSED( Mapping, MAPPING_PARAM_NB + TEX_PARAM_NB + TEX_MORE_PARAM_NB + HEXA_PARAM_NB + TRANSFO_PARAM_NB + 4 )
			PARAM_DEF_BOOL_OFF(	texture_feedback )
			PARAM_DEF_BOOL_OFF(	uv_src_bdd_use )
			PARAM_DEF_REF(		uv_src_bdd_name_symbo )
			PARAM_DEF_INT32(	uv_src_bdd_dataset_id,	2, 1,	1, PARAM_MAX_INT32 )

			PARAM_DEF_GROUP_CLOSED( TEX, TEX_PARAM_NB )
				PARAM_DEF_BOOL_OFF(		tex_flip_uv			)
				PARAM_DEF_BOOL_OFF(		tex_mirror_u		)
				PARAM_DEF_BOOL_OFF(		tex_mirror_v		)
				PARAM_DEF_BOOL_OFF(		tex_coor_centered	)
				PARAM_DEF_REAL_ZERO(	tex_u_ori			)
				PARAM_DEF_REAL_ZERO(	tex_v_ori			)
				PARAM_DEF_REAL_ZERO(	tex_w_ori			)
				PARAM_DEF_REAL_INF(		tex_u,				2, 1 )
				PARAM_DEF_REAL_INF(		tex_v,				2, 1 )
				PARAM_DEF_REAL_INF(		tex_w,				2, 1 )
				PARAM_DEF_REAL_INF(		tex_factor,			2, 1 )
				PARAM_DEF_BOOL_OFF(		tex_rot				)
				PARAM_DEF_POINT_UVA(	tex_rot				)

			PARAM_DEF_GROUP_CLOSED( TEX More, TEX_MORE_PARAM_NB )
				PARAM_DEF_BOOL_OFF(		tex_by_side			)
				PARAM_DEF_REAL_ZERO(	tex_left			)
				PARAM_DEF_REAL_ONE(		tex_right			)
				PARAM_DEF_REAL_ZERO(	tex_bottom			)
				PARAM_DEF_REAL_ONE(		tex_top				)
				PARAM_DEF_REAL_ZERO(	tex_w_min			)
				PARAM_DEF_REAL_ONE(		tex_w_max			)
				PARAM_DEF_REAL_ZERO(	tex_u_speed			)
				PARAM_DEF_REAL_ZERO(	tex_v_speed			)
				PARAM_DEF_REAL_ZERO(	tex_w_speed			)
				PARAM_DEF_REAL_ONE(		tex_speed_factor	)
				PARAM_DEF_BOOL_OFF(		tex_restart_trig	)

			PARAM_DEF_GROUP_CLOSED( Hexagonal mapping, HEXA_PARAM_NB )
				PARAM_DEF_BOOL_OFF(		tex_hexa )
				PARAM_DEF_REAL_ZERO(	tex_hexa_du			)
				PARAM_DEF_REAL_ZERO(	tex_hexa_dv			)
				PARAM_DEF_REAL_ONE(		tex_offset_2		)
				PARAM_DEF_REAL_ONE(		tex_offset_4		)
				PARAM_DEF_REAL_ONE(		tex_offset_6		)
				PARAM_DEF_REAL_ZERO(	tex_offset_factor	)

//todo this should be clarified eye/target and rot done only when no cam
			PARAM_DEF_GROUP_CLOSED( Transformation, TRANSFO_PARAM_NB )
				PARAM_DEF_SYMBO_PSTR_ZERO(	camera_use, tex_camera )
				PARAM_DEF_REF(				camera_name_symbo )
				PARAM_DEF_INT32(			camera_index, 1,	0, 0,	c_layers::CAMERA_NB-1 )
				PARAM_DEF_BOOL_OFF(			lookat )
				PARAM_DEF_POINT_XYZ(		eye )
				PARAM_DEF_POINT_XYZ(		target )
				PARAM_DEF_POINT_XYZ(		rot )

		PARAM_DEF_GROUP_CLOSED( Wrap, WRAP_PARAM_NB )
			PARAM_DEF_SYMBO_SYNO_PSTR(	tex_wrap_u,			2, 1,	wrap_str )
			PARAM_DEF_SYMBO_SYNO_PSTR(	tex_wrap_v,			2, 1,	wrap_str )
			PARAM_DEF_SYMBO_SYNO_PSTR(	tex_wrap_w,			2, 1,	wrap_str )

		PARAM_DEF_GROUP_CLOSED( Filtering, FILTER_PARAM_NB )
			PARAM_DEF_BOOL_OFF(		tex_force_min_mag		)
			PARAM_DEF_SYMBO(		tex_2d_minification,	5, 0,	5, tex::minmag_mode_str )
			PARAM_DEF_SYMBO(		tex_2d_magnification,	1, 0,	1, tex::minmag_mode_str )
			PARAM_DEF_REAL_ZERO(	lod_min					)
			PARAM_DEF_REAL_INF(		lod_max,				0, 16 )

		PARAM_DEF_GROUP( Blending, BLEND_PARAM_NB + BLEND_MORE_PARAM_NB + 1 )
			PARAM_DEF_SYMBO_SYNO_PSTR_ZERO(	tex_env_mode,			blend_tex_env_mode_str )
			PARAM_DEF_BOOL_ON(				blend					)
			PARAM_DEF_SYMBO_SYNO_PSTR(		blend_equation,			1, 2,	blend_equation_str )
			PARAM_DEF_SYMBO_SYNO_PSTR(		blend_src,				1, 6,	blend_str )
			PARAM_DEF_SYMBO_SYNO_PSTR(		blend_dst,				1, 7,	blend_str )
			PARAM_DEF_BOOL_OFF(				blend_separate			)
			PARAM_DEF_SYMBO_SYNO_PSTR(		blend_alpha_equation,	1, 2,	blend_equation_str )
			PARAM_DEF_SYMBO_SYNO_PSTR(		blend_alpha_src,		1, 6,	blend_str )
			PARAM_DEF_SYMBO_SYNO_PSTR(		blend_alpha_dst,		1, 7,	blend_str )
			PARAM_DEF_SYMBO(				alpha_test,				2, 1,	7, gstr::test_func )
			PARAM_DEF_REAL(					alpha_threshold,		.01,0,	0, 1 )

			PARAM_DEF_GROUP_CLOSED( More, BLEND_MORE_PARAM_NB )
				PARAM_DEF_COLOR_RGBGA(		blend_color				)
				PARAM_DEF_BOOL_OFF(			polygon_smooth			)
				PARAM_DEF_SYMBO_PSTR_ZERO(	polygon_smooth_hint,	gstr::hint )
				PARAM_DEF_BOOL_OFF(			tex_env_do				)
				PARAM_DEF_COLOR_RGBGA(		tex_env					)
//todo removed because of cost in param_more
//		PARAM_DEF_NONE( stencil )
//		PARAM_DEF_NONE( texture_video )
	};
}

void	c_map::param_init_pt()
{
	INT32	h=0;

	param_set_pt(	h, get_name_symbo()	);
	param_set_pt(	h, _s_tex_use_ui	);
	param_set_pt(	h, _b_implicit_ui	);
	param_set_pt(	h, _s_tex_method	);
	param_set_pt(	h, _tex_axe			);

	++h;
		param_set_pt(				h, _b_feedback_ui				);
		param_set_pt(				h, _b_uv_src_bdd_use_ui			);
		param_set_pt_attach_obj(	h, _uv_src_bdd_name_symbo,	_uv_src_bdd		);
		param_set_pt(				h, _uv_src_bdd_dataset_id	);

		++h;
			param_set_pt(	h, _b_flip_uv_ui		);
			param_set_pt_2(	h, _b_mirror_ui			);
			param_set_pt(	h, _b_coor_center_ui	);
			param_set_pt_3(	h, _uvw_ori_ui			);
			param_set_pt_3(	h, _uvw_ui				);
			param_set_pt(	h, _uvw_factor_ui		);
			param_set_pt(	h, _b_rot_base_ui		);
			param_set_pt_3(	h, _rot_base_ui			);

		++h;
			param_set_pt(	h, _b_by_side_ui	);
			param_set_pt_2(	h, _uvw_side[0]		);
			param_set_pt_2(	h, _uvw_side[1]		);
			param_set_pt_2(	h, _uvw_side[2]		);

			param_set_pt_3(	h, _uvw_speed			);
			param_set_pt(	h, _speed_factor		);
			param_set_pt(	h, _b_restart_trig_ui	);

		++h;
			param_set_pt(	h,	_b_tex_hexa_ui		);
			param_set_pt_2(	h,	_tex_hexa_delta_ui	);
			param_set_pt_4(	h,	_tex_hexa_offset_ui	);

		++h;
			param_set_pt(				h, _s_camera_use_ui		);
			param_attach_obj_no_inc(	h,  (c_obj_ui*) _camera );
			param_set_pt(				h, _target_name_symbo		);
			if( c_layer::get_cur() )
			{	//todonow this will not work with module
				c_layers*	ps = (c_layers*) c_layer::get_cur()->get_root();
				if( ps && !ps->is_the_root() )
					param_attach_obj_no_inc( h, (c_obj_ui*) ps->get_camera( _camera_index ) );
			}
			param_set_pt(	h, _camera_index	);
			param_set_pt(	h, _b_lookat_ui		);
			param_set_pt_3(	h, _eye				);
			param_set_pt_3(	h, _target			);
			param_set_pt_3(	h, _rot_other_ui	);

	++h;
		param_set_pt_3(	h, _s_wrap_ui	);
				
	++h;
		param_set_pt(	h, _b_force_min_mag_ui	);
		param_set_pt(	h, _s_minification_ui	);
		param_set_pt(	h, _s_magnification_ui	);
		param_set_pt(	h, _lod_min_ui			);
		param_set_pt(	h, _lod_max_ui			);

	++h;
		param_set_pt(	h, _s_tex_env_mode_ui			);
		param_set_pt(	h, _b_blend_ui					);
		param_set_pt(	h, _s_blend_equation_ui			);
		param_set_pt(	h, _s_blend_src_ui				);
		param_set_pt(	h, _s_blend_dst_ui				);
		param_set_pt(	h, _b_blend_separate_ui			);
		param_set_pt(	h, _s_blend_alpha_equation_ui	);
		param_set_pt(	h, _s_blend_alpha_src_ui		);
		param_set_pt(	h, _s_blend_alpha_dst_ui		);
		param_set_pt(	h, _s_alpha_test_ui				);
		param_set_pt(	h, _alpha_threshold_ui			);

		++h;
			param_set_pt_rgbfa(	h, _blend_color_ui			);
			param_set_pt(		h, _b_polygon_smooth_ui		);
			param_set_pt(		h, _polygon_smooth_hint_ui	);
			param_set_pt(		h, _b_tex_env_use_ui		);
			param_set_pt_rgbfa(	h, _tex_env					);

//	param_attach_obj( h, (c_obj_ui*) c_stencil::ui );
//	param_attach_obj( h, (c_obj_ui*) c_tex_video::ui );

	err_param_init_pt(h);
}

void	c_map::build_sum_up( o_str& o )
{
	switch( _s_tex_use_ui )
	{
	case 0:	o.erase();		break;
	case 1:	o.set( "1D " );	break;
	case 2:	o.set( "2D " );	break;
	case 3:	o.set( "3D " );	break;
	}

	if( _b_implicit_ui )
	{
		if( _b_tex_hexa_ui )
			o.add( "Hexa " );
	}
	else
	{
		o.add( tex_mode_str[_s_tex_method] );
		o.add_space();
	}

	o.add( blend_tex_env_mode_str[_s_tex_env_mode_ui] );
	o.add_space();

	//if( _b_blend_ui )
	//{
	//	o.add( blend_str[_s_blend_equation_ui] );
	//	o.add_space();
	//	if( _b_blend_separate_ui )
	//	{
	//		o.add( blend_str[_s_blend_alpha_equation_ui] );
	//		o.add_space();
	//	}
	//}
}

CONSTRUCTOR_CREATE(c_map)
,_uv_src_bdd(nullptr)
,_camera(nullptr)
,_camera_index(0)
{
	param_init_with( n_map::param, n_map::PARAM_NB_MAX );
}

c_map::~c_map()
{
	c_map* map_cur = get_cur();
	if( map_cur == this )
		set_cur_def();
	if( map_cur == this )
		set_ui_def();
}

void c_map::param_init()
{
	clear_v3( _uvw_phase );	// because of trouble with time before delta_t
//todo deal really with it global and by obj
	_b_verbose = true;
}

bool	c_map::b_need_texture_identity = false;	//	set to true when we need to set it back the next time

void	c_map::reset_texture_matrix()
{
	if( b_need_texture_identity )
	{
		GOL::matrix::set_texture();
			GOL::matrix::load_identity();
		GOL::matrix::set_modelview();
		b_need_texture_identity = false;
	}
}

void	c_map::set_texture_matrix_base()
{
	GOL::matrix::set_texture();

		GOL::matrix::load_identity();
		b_need_texture_identity = _b_rot_base_ui && is_not_null_v3( _rot_base_ui );
		if( b_need_texture_identity )
		{
			FP32 center[3];
			interpolate_v3( center, _uvw_min_max, _uvw_min_max + 3, .5 );
			GOL::matrix::translate3v( center );
			GOL::matrix::rotatev_neg( _rot_base_ui );
			GOL::matrix::translate_negv( center );
		}

	GOL::matrix::set_modelview();
}

//todo go back to this when multiple ?
//unused for now
//void	c_map::update_implicit_full()
//{
//	update_implicit();
//	if( _b_implicit )
//	{
//		if( c_multiple::cur->is_render_multiple() )
//			_b_implicit = c_multiple::cur->can_implicit();
//		else
//		{
//			c_bdd* bdd_cur = c_bdd::get_cur();
//			_b_implicit = bdd_cur ? bdd_cur->can_implicit() : false;
//		}
//	}
//}

void	c_map::update_uvw()
{
//	implicit is not always set as it should
//	it can change with the bdd or multiple and we want to keep it in update and not in draw
//	if( _s_tex_method==PROJ_SPHERE && !is_implicit() )	//	optimization for Sphere Map
//	{
//		return;
//	}

	if( _b_by_side_ui )
	{
		_uvw_ui[0] = _uvw_side[0][1] - _uvw_side[0][0];
		_uvw_ori_ui[0] = -_uvw_side[0][0] ;

		_uvw_ui[1] = _uvw_side[1][1] - _uvw_side[1][0];
		_uvw_ori_ui[1] = -_uvw_side[1][0] ;

		_uvw_ui[2] = _uvw_side[2][1] - _uvw_side[2][0];
		_uvw_ori_ui[2] = -_uvw_side[2][0] ;
	}

	//hack ?
	//	if( g_screen_index == 0)
	{
		if( _b_restart_trig_ui || ( draw::is_render_first_pass() && _delta_t.update() ) )
		{
			clear_v3( _uvw_phase );
			_b_restart_trig_ui = false;
		}
		if( _speed_factor != 0. && (_uvw_speed[0] != 0. || _uvw_speed[1] != 0. || _uvw_speed[2] != 0.) )
			add_scale_v3( _uvw_phase, _uvw_speed, -_speed_factor*_delta_t.get_dt() );
	}

	if( _b_coor_center_ui )
	{
		scale_v3( _uvw, _uvw_ui, _uvw_factor_ui*REAL(.5) );
		sub_v3( _uvw_min_max, _uvw_phase, _uvw_ori_ui );
		add_v3( _uvw_min_max+3, _uvw_min_max, _uvw );
		sub_v3( _uvw_min_max, _uvw );
	}
	else
	{
		scale_v3( _uvw, _uvw_ui, _uvw_factor_ui );
		sub_v3( _uvw_min_max, _uvw_phase, _uvw_ori_ui );
		add_v3( _uvw_min_max+3, _uvw_min_max, _uvw );
	}

	//quad used only when implicit
	_b_quad_uv_need_compute		= true;
	_b_quad_uvw_need_compute	= true;
}

void	c_map::tex_draw()
{
//aqua
#if	AAA_DEBUG()
	if( _s_tex_use == 0 )
		err_print( "we should not pass here :  Maa ????" );
#endif
	GOL::set_texture_dim( _s_tex_use );

//	GOL::set_texture_2D_private();
	if( is_implicit() )
	{
		set_texture_matrix_base();
//todo do it well
/*
		else
		{

			//todo merge with to the previous case
			//if( !is_equal_v2r( _uv, 1. ) )
			{
				//b_need_texture_identity = true;
				GOL::matrix::set_texture();
					GOL::load_identity();
					GOL::scale( _uv[0], _uv[1], 1. );
					GOL::translate( -_uv_ori[0], -_uv_ori[1], 0 );
					//GOL::scale( _uv[0], _uv[1] );
				GOL::matrix::set_modelview();
			}
			//todo deal with tex_unit to use this
			//else
			//	reset_texture_matrix();

		}
*/
		GOL::disable_texture_gen_s();
		GOL::disable_texture_gen_t();
		GOL::disable_texture_gen_r();
		return;
	}

	switch( _s_tex_method )
	{
	case PROJ_SPHERE:
		set_texture_matrix_base();
		GOL::enable_texture_gen_s();
		GOL::enable_texture_gen_t();
		GOL::texgen_i( GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP );
		GOL::texgen_i( GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP );
		// next line trigger an error message
		//	glTexGeni(GL_R, GL_TEXTURE_GEN_MODE,  GL_SPHERE_MAP);
		return;

	case PROJ_REFLECTION:
		set_texture_matrix_base();
		GOL::enable_texture_gen_s();
		GOL::enable_texture_gen_t();
		GOL::texgen_i( GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP );
		GOL::texgen_i( GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP );
		return;

	case PROJ_NORMAL:
		set_texture_matrix_base();
		GOL::enable_texture_gen_s();
		GOL::enable_texture_gen_t();
		GOL::enable_texture_gen_r();
		GOL::texgen_i( GL_S, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP);
		GOL::texgen_i( GL_T, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP);
		GOL::texgen_i( GL_R, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP);
		return;
	//todomatrix document what are these modes

	case PROJ_CAMERA:
	case PROJ_CAMERA_MODELVIEW:
		//todo make it work for the side views
		{
			glm::mat4 mat_view;
			glm::mat4 mat_proj;
			c_seedcam* cam = nullptr;
			if( _s_camera_use_ui )	//&& b_pers_win, this flag was set by draw_universe()
			{
				if( _s_camera_use_ui == 1 )
				{
					if( _target_name_symbo.is_empty() )
						_camera = nullptr;
					else
					{
						if( !_camera || !_camera->is_name_symbo( _target_name_symbo ) || !_camera->get_root() )
							_camera = (c_seedcam *)find_by_class_and_name_symbo( "seedcam", _target_name_symbo );
					}				
					cam = _camera;
				}
				else
					cam = c_layers::get_cur()->get_camera( _camera_index );

				if( cam )
				{
					if( cam == c_seedcam::get_cur() )
					{	//todomatrix pb in 4 views ?
						if( _s_tex_method == PROJ_CAMERA )
							mat_proj = cam->get_matrix_projection();
						mat_view = cam->get_matrix_view();
					}
					else
						cam->compute_matrix_view_projection( &mat_view, ( _s_tex_method == PROJ_CAMERA ) ? &mat_proj : nullptr );
				}
			}

			GOL::enable_texture_gen_s();
			GOL::enable_texture_gen_t();
			GOL::enable_texture_gen_r();
			//unused ?
			//GOL::enable_texture_gen_q();

			GOL::texgen_i( GL_S, GL_TEXTURE_GEN_MODE,  GL_OBJECT_LINEAR );
			GOL::texgen_i( GL_T, GL_TEXTURE_GEN_MODE,  GL_OBJECT_LINEAR );
			GOL::texgen_i( GL_R, GL_TEXTURE_GEN_MODE,  GL_OBJECT_LINEAR );
			//unused ?	
			//GOL::texgen_i( GL_Q, GL_TEXTURE_GEN_MODE,  GL_OBJECT_LINEAR );

			GOL::texgen_fv( GL_S, GL_OBJECT_PLANE, unit_x_v4fp32 );
			GOL::texgen_fv( GL_T, GL_OBJECT_PLANE, unit_y_v4fp32 );
			GOL::texgen_fv( GL_R, GL_OBJECT_PLANE, unit_z_v4fp32 );
			//unused ?
			//GOL::texgen_fv( GL_Q, GL_OBJECT_PLANE, tex_q );

			set_texture_matrix_base();
			GOL::matrix::set_texture();
				//GOL::matrix::load_identity();
				//if( _s_camera_use_ui && cam )

				GOL::matrix::translate( _uvw_ori_ui[0]+REAL(.5),	_uvw_ori_ui[1]+REAL(.5),	_uvw_ori_ui[2]+REAL(.5) );
				GOL::matrix::scale(		_uvw[0]*REAL(.5),			_uvw[1]*REAL(.5),			_uvw[2]*REAL(.5)		);
				if( cam )
				{
					if( _s_tex_method == PROJ_CAMERA )
						GOL::matrix::mul_matrix( &mat_proj );
					GOL::matrix::mul_matrix( &mat_view );
					
					//glTexGendv( GL_S, GL_OBJECT_PLANE, &mat44_invert[0] );
					//glTexGendv( GL_T, GL_OBJECT_PLANE, &mat44_invert[4] );
					//glTexGendv( GL_R, GL_OBJECT_PLANE, &mat44_invert[8] );
					//glTexGendv( GL_Q, GL_OBJECT_PLANE, &mat44_invert[12] );
				}
				else
				{
					if( _b_lookat_ui )
					{
						if( is_not_null_v3( _eye ) || is_not_null_v3( _target ) )
							GOL::matrix::lookat_safe( _eye, _target );
					}
					else
					{
						if( is_not_null_v3( _rot_other_ui ) )
							GOL::matrix::rotatev( _rot_other_ui );
					}
				}
			b_need_texture_identity = true;
			GOL::matrix::set_modelview();
		} //of case PROJ_CAMERA PROJ_CAMERA_MODELVIEW
		return;
	case PROJ_OBJECT:
	case PROJ_EYE:
		{
			set_texture_matrix_base();
			//todo check if we need implicit or not
			//	or adopt other strategy like complete update when rendering

			//todo	wrong test and not useful now
			/*
			if(		is_diff_v2r( uv_last, uv )
				||	is_diff_v2r( uv_ori_last, uv_ori )
				||	b_mirror_last[0] != b_mirror[0]
				||	b_mirror_last[1] != b_mirror[1]
				||	b_flip_uv_last != b_flip_uv
				)
				{
				cpy_v2r( uv_last, uv );
				cpy_v2r( uv_ori_last, uv_ori );
				b_mirror_last[0] = b_mirror[0];
				b_mirror_last[1] = b_mirror[1];
				b_flip_uv_last = b_flip_uv;
				}
			*/

			GLfloat	param_s[] = { 0., 0., 0., 0. };
			GLfloat	param_t[] = { 0., 0., 0., 0. };
			//tex3d
			GLfloat	param_r[] = { 0., 0., 0., 0. };
			INT32	axe_u;
			INT32	axe_v;
			axe_build_index_vert( axe_u, axe_v, _tex_axe );
			if( _b_flip_uv_ui )
				SWAP( axe_u, axe_v );

			if( is_2d() || is_3d() )
			{
				REAL	tu = ( _tex_axe == 0 ) ? -_uvw[0] : _uvw[0];
			//todoopt
				param_s[ axe_u ]	= COS_TURN( _rot_base_ui[0] ) * tu;
				param_s[ _tex_axe ] = SIN_TURN( _rot_base_ui[0] ) * tu;

				param_t[ axe_v ]	= COS_TURN( _rot_base_ui[1] ) * _uvw[1];
				param_t[ _tex_axe ] = SIN_TURN( _rot_base_ui[1] ) * _uvw[1];

	//			param_s[ axe_v ] = SIN_INT( _rot_base_ui[2] );
	//			param_t[ axe_u ] = COS_INT( _rot_base_ui[2] );
				//tex3d
				param_r[ _tex_axe] = _uvw[2];
			}
			else if( is_1d() )
			{
				param_s[ _tex_axe ] = _uvw[0];
			}
			if( _b_flip_uv_ui )
			{
				param_t[ 3 ] = _uvw_phase[0] - _uvw_ori_ui[0] + REAL(.5);
				param_s[ 3 ] = _uvw_phase[1] - _uvw_ori_ui[1] + REAL(.5);
			}	
			else
			{
				param_s[ 3 ] = _uvw_phase[0] - _uvw_ori_ui[0] + REAL(.5);
				param_t[ 3 ] = _uvw_phase[1] - _uvw_ori_ui[1] + REAL(.5);
			}
			//tex3d
			param_r[ 3 ] = _uvw_phase[2] - _uvw_ori_ui[2] + REAL(.5);

			GOL::enable_texture_gen_s();
			GOL::enable_texture_gen_t();
			//tex3d
			GOL::enable_texture_gen_r();

			//todo split mode by coordinate
			switch( _s_tex_method )
			{
			case PROJ_OBJECT:	//	OBJECT
				GOL::texgen_i(	GL_S, GL_TEXTURE_GEN_MODE,  GL_OBJECT_LINEAR );
				GOL::texgen_fv(	GL_S, GL_OBJECT_PLANE, param_s );
				GOL::texgen_i(	GL_T, GL_TEXTURE_GEN_MODE,  GL_OBJECT_LINEAR );
				GOL::texgen_fv(	GL_T, GL_OBJECT_PLANE, param_t );
				//tex3d
				GOL::texgen_i(	GL_R, GL_TEXTURE_GEN_MODE,  GL_OBJECT_LINEAR );	//hack
				GOL::texgen_fv(	GL_R, GL_OBJECT_PLANE, param_r );//hack
				break;
			case PROJ_EYE:	//	EYE
				if( c_seedcam::get_cur() )
				{
					//bug	??? this have to be there
					c_seedcam::get_cur()->before_eye();
						param_s[ axe_u ] = _uvw[0];
						GOL::texgen_i(	GL_S, GL_TEXTURE_GEN_MODE,  GL_EYE_LINEAR );
						GOL::texgen_fv( GL_S, GL_EYE_PLANE, param_s );
						GOL::texgen_i(	GL_T, GL_TEXTURE_GEN_MODE,  GL_EYE_LINEAR );
						GOL::texgen_fv(	GL_T, GL_EYE_PLANE, param_t );
						//tex3d
						GOL::texgen_i(	GL_R, GL_TEXTURE_GEN_MODE,  GL_EYE_LINEAR );
						GOL::texgen_fv( GL_R, GL_EYE_PLANE, param_r );
					c_seedcam::get_cur()->after_eye();
				}
				break;
			}
		}
		return;
	}
}

void	c_map::reset_quad_uv()
{
#if 1
	if( _b_flip_uv_ui )
		memcpy( _quad_uv, quad_uv_01_flipped,	sizeof( quad_uv_01_flipped ) );
	else
		memcpy( _quad_uv, quad_uv_01,			sizeof( quad_uv_01 ) );
#else
	UVx4 CONST & uv = _quad_uv; 
	//quad used only when implicit
	if( _b_flip_uv )
	{
		uv[2][0] = uv[0][0]	= uv[0][1] = uv[1][1] = 0;
		uv[1][0] = uv[3][0] = uv[2][1] = uv[3][1] = 1;
	}
	else
	{
		uv[0][0] = uv[1][0] = uv[1][1] = uv[3][1] = 0;
		uv[3][0] = uv[2][0] = uv[0][1] = uv[2][1] = 1;
	}
#endif
	_b_quad_uv_need_compute = false;
}

void	c_map::set_quad_uv( FP32* uv )
{ 
	//quad used only when implicit
	if( _b_flip_uv_ui )
	{
		_quad_uv[1][0] = uv[1];
		_quad_uv[1][1] = uv[0];
		_quad_uv[0][0] = uv[3];
		_quad_uv[0][1] = uv[2];
		_quad_uv[3][0] = uv[5];
		_quad_uv[3][1] = uv[4];
		_quad_uv[2][0] = uv[7];
		_quad_uv[2][1] = uv[6];
	}					   
	else				   
	{					   
		_quad_uv[1][0] = uv[0];
		_quad_uv[1][1] = uv[1];
		_quad_uv[0][0] = uv[2];
		_quad_uv[0][1] = uv[3];
		_quad_uv[3][0] = uv[4];
		_quad_uv[3][1] = uv[5];
		_quad_uv[2][0] = uv[6];
		_quad_uv[2][1] = uv[7];
	}
	_b_quad_uv_need_compute = false;
}

//todo
//tex3d
void	c_map::set_quad_uv_from_uv_min_max( FP32* uv_min_max )
{
	//quad used only when implicit
	if( _b_flip_uv_ui )
	{
		_quad_uv[2][0] = _quad_uv[0][0]	= uv_min_max[0];
		_quad_uv[0][1] = _quad_uv[1][1] = uv_min_max[1];
		_quad_uv[1][0] = _quad_uv[3][0] = uv_min_max[2];
		_quad_uv[2][1] = _quad_uv[3][1] = uv_min_max[3];
	}
	else
	{
		_quad_uv[0][0] = _quad_uv[1][0] = uv_min_max[0];
		_quad_uv[1][1] = _quad_uv[3][1] = uv_min_max[1];
		_quad_uv[3][0] = _quad_uv[2][0] = uv_min_max[2];
		_quad_uv[0][1] = _quad_uv[2][1] = uv_min_max[3];
	}
	_b_quad_uv_need_compute = false;
}

void	c_map::set_quad_uv_from_uvw( FP32* uvw_min_max )
{
	//quad used only when implicit
	if( _b_flip_uv_ui )
	{
		_quad_uv[2][0] = _quad_uv[0][0]	= uvw_min_max[0];
		_quad_uv[0][1] = _quad_uv[1][1] = uvw_min_max[1];
		_quad_uv[1][0] = _quad_uv[3][0] = uvw_min_max[3];
		_quad_uv[2][1] = _quad_uv[3][1] = uvw_min_max[4];
	}
	else
	{
		_quad_uv[0][0] = _quad_uv[1][0] = uvw_min_max[0];
		_quad_uv[1][1] = _quad_uv[3][1] = uvw_min_max[1];
		_quad_uv[3][0] = _quad_uv[2][0] = uvw_min_max[3];
		_quad_uv[0][1] = _quad_uv[2][1] = uvw_min_max[4];
	}
	_b_quad_uv_need_compute = false;
}

void	c_map::set_quad_uvw_from_uvw( FP32* uvw_min_max )
{
	//quad used only when implicit
	if( _b_flip_uv_ui )
	{
		_quad_uvw[4][0] = _quad_uvw[7][0] = uvw_min_max[1];
		_quad_uvw[3][1] = _quad_uvw[7][1] = uvw_min_max[0];
		_quad_uvw[0][0] = _quad_uvw[3][0] = uvw_min_max[4];
		_quad_uvw[0][1] = _quad_uvw[4][1] = uvw_min_max[3];
	}
	else
	{
		_quad_uvw[4][0] = _quad_uvw[7][0] = uvw_min_max[0];
		_quad_uvw[3][1] = _quad_uvw[7][1] = uvw_min_max[1];
		_quad_uvw[0][0] = _quad_uvw[3][0] = uvw_min_max[3];
		_quad_uvw[0][1] = _quad_uvw[4][1] = uvw_min_max[4];
	}

	cpy_v2( _quad_uvw[1], _quad_uvw[0] );
	cpy_v2( _quad_uvw[5], _quad_uvw[4] );
	cpy_v2( _quad_uvw[2], _quad_uvw[3] );
	cpy_v2( _quad_uvw[6], _quad_uvw[7] );

	_quad_uvw[1][2] = _quad_uvw[2][2] = _quad_uvw[5][2] = _quad_uvw[6][2] =  uvw_min_max[2];
	_quad_uvw[0][2] = _quad_uvw[3][2] = _quad_uvw[4][2] = _quad_uvw[7][2] =  uvw_min_max[5];

	_b_quad_uv_need_compute = false;
}

FINLINE void	c_map::update_blending()
{
	if( is_blend() )
	{
		if( _s_alpha_test_ui == 1 )
			GOL::disable_alpha_test();
		else
		{
			GOL::enable_alpha_test();
			GOL::set_alpha_func( GOL::test_func_array[_s_alpha_test_ui], get_alpha_threshold() );
		}

		GOL::enable_blend();
		FP32	f = _blend_color_ui[4];
		GOL::blend_color( _blend_color_ui[0]*f, _blend_color_ui[1]*f, _blend_color_ui[2]*f, _blend_color_ui[3] );

		if( _b_polygon_smooth_ui )
		{
			GOL::enable_polygon_smooth();
			//todo move it as a global preference
			GOL::hint( GL_POLYGON_SMOOTH_HINT, _polygon_smooth_hint_ui );

			GOL::set_blend_func( GL_SRC_ALPHA_SATURATE, GL_ONE );
			if( is_blend_separate() )
				GOL::blend_equation(	GOL::BLEND_EQUATION(_s_blend_equation_ui),	GOL::BLEND_EQUATION(_s_blend_alpha_equation_ui)	);
			else
				GOL::blend_equation(	GOL::BLEND_EQUATION(_s_blend_equation_ui) );
		}
		else
		{
			GOL::disable_polygon_smooth();

			if( is_blend_separate() )
			{
				GOL::blend_equation(	GOL::BLEND_EQUATION(_s_blend_equation_ui),	GOL::BLEND_EQUATION(_s_blend_alpha_equation_ui)	);
				GOL::set_blend_func(	blend_array[_s_blend_src_ui],				blend_array[_s_blend_dst_ui],
										blend_array[_s_blend_alpha_src_ui],			blend_array[_s_blend_alpha_dst_ui]		);
			}
			else
			{
				GOL::blend_equation(	GOL::BLEND_EQUATION(_s_blend_equation_ui)	);
				GOL::set_blend_func(	blend_array[_s_blend_src_ui],				blend_array[_s_blend_dst_ui]			);
			}
		}
	}
	else
	{
		GOL::disable_alpha_test();
		GOL::disable_blend();
		GOL::disable_polygon_smooth();
	}
}

FINLINE	void c_map::tex_env_update()
{
	static FP32	color[4];
	scale_v3_cpy_v4( color, _tex_env );
	GOL::texenvv( GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, color );
}

void	c_map::update()
{
	if( GOL::b_tex_allow_ui )
		_s_tex_use = _b_feedback ? 2 : _s_tex_use_ui;
	else
		_s_tex_use = 0;

	//maa april 2020 only update blending was skip for text 123
	//	i try more because most shaders we use don't deal with infos by unit
	//maa	we should go back to it after monaco
	if( GOL::is_tex_unit_index(0) )
	{
		static FP32 border_color[4] = { .5, .5, .5, .5 };
		glTexParameterfv( GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color );
		GOL::set_tex_env_mode( tex_env_mode_array[ _s_tex_env_mode_ui ] );
//maa comment in 2020 April because don't understand why it is here 
//#if	!AAA_WATCHDOG()
		_b_feedback = _b_feedback_ui && c_bdd_feedback::is_allow();
		if( _b_feedback )
		{
			c_bdd_feedback::set_grab_needed();
			_b_feedback = c_bdd_feedback::is_grab_ready();
		}
//#endif
		if( _b_tex_env_use_ui )
			tex_env_update();

//		if( _s_tex_use )	//todo 2022 Mai Maa remove because we want uv if we are implicit so we need this update
		update_uvw();

		update_blending();	//this should be called only for unit 0
		c_map::set_cur( this );

		if( _b_uv_src_bdd_use_ui && !_uv_src_bdd_name_symbo.is_empty() )
		{
			if( !_uv_src_bdd || !_uv_src_bdd->is_name_symbo( _uv_src_bdd_name_symbo ) || !_uv_src_bdd->get_root() )
			{
				_uv_src_bdd = nullptr;
				c_bdd* bdd = find_bdd_by_name_symbo( _uv_src_bdd_name_symbo );
				if( bdd )
				{
					if( bdd->is_inherited_from( "bdd_uv" ) )
						_uv_src_bdd = (c_bdd_uv*) bdd;
					else
						ERR_PRINT_STRING( "%s() found a bdd but this not a bdd_uv", __FUNCTION__ );
				}
			}
		}
		else
			_uv_src_bdd = nullptr;
	}
}

// VERBOSE
void	c_map::print_verbose()
{
	if( _b_verbose )
		SWITCH_PRINT_STATE("Mapping Object have Verbose", _b_verbose );
}

void	c_map::set_verbose( bool b )
{
	_b_verbose = b;
	print_verbose();
}

void	c_map::flip_verbose()
{
	set_verbose( !_b_verbose );
}

// TEXTURE
void	c_map::print_flip_uv()
{
	if( _b_verbose )
		SWITCH_PRINT_STATE("Texture flip_uv", _b_flip_uv_ui );
}

void	c_map::set_flip_uv( bool b_in )
{
	_b_flip_uv_ui = b_in;
	print_flip_uv();
}

void	c_map::flip_flip_uv()
{
	set_flip_uv( !_b_flip_uv_ui );
}

void	c_map::print_mirror_u()
{
	if( _b_verbose )
		SWITCH_PRINT_STATE("Texture mirror_u", _b_mirror_ui[0] );
}

void	c_map::set_mirror_u( bool b_in )
{
	_b_mirror_ui[0] = b_in;
	print_mirror_u();
}

void	c_map::flip_mirror_u()
{
	set_mirror_u( !_b_mirror_ui[0]);
}

void	c_map::print_mirror_v()
{
	if( _b_verbose )
		SWITCH_PRINT_STATE("Texture mirror_v", _b_mirror_ui[1] );
}

void	c_map::set_mirror_v( bool b_in )
{
	_b_mirror_ui[1] = b_in;
	print_mirror_v();
}

void	c_map::flip_mirror_v()
{
	set_mirror_v( !_b_mirror_ui[1]);
}


void	c_map::print_implicit_ui()
{
	if( _b_verbose )
		SWITCH_PRINT_STATE( "Texture implicit", _b_implicit_ui );
}

void	c_map::set_implicit_ui( bool b_in )
{
	_b_implicit_ui = b_in;
	print_implicit_ui();
}

void	c_map::flip_implicit_ui()
{
	set_implicit_ui( !_b_implicit_ui );
}

//	TEX_USE
void	c_map::print_tex_use_ui()
{
	if( _b_verbose )
		SWITCH_PRINT_STRING("Texture", tex_dim_str[_s_tex_use_ui] );
}

void	c_map::set_tex_use_ui( INT32 value_in )
{
	_s_tex_use_ui = IMOD( value_in, 4 );	//hack	we don't loop with 3D ??
	print_tex_use_ui();						//	this could be feature
}

INT32	c_map::inc_tex_use_ui()
{
	set_tex_use_ui( _s_tex_use_ui + 1 );
	return _s_tex_use_ui;
}

INT32	c_map::dec_tex_use_ui()
{
	set_tex_use_ui( _s_tex_use_ui - 1 );
	return _s_tex_use_ui;
}

//	TEX_FEEDBACK
void c_map::set_texture_feedback( bool flag )
{
	_b_feedback_ui = flag;
	SWITCH_PRINT_STATE( "Feedback Texture", _b_feedback_ui );
}

void c_map::flip_texture_feedback()
{
	set_texture_feedback( !_b_feedback_ui );
}


//	TEX_ENV_MODE
//void	c_map::print_tex_env_mode()
//{
//	if( _b_verbose )
//		SWITCH_PRINT_STRING("Texture Environment Mode", blend_tex_env_mode_str[_s_tex_env_mode_ui] );
//}
//
//void	c_map::set_tex_env_mode( INT32 value_in )
//{
//	_s_tex_env_mode_ui = IMOD( value_in, 4 );
//	print_tex_env_mode();
//}
//
//INT32	c_map::inc_tex_env_mode()
//{
//	set_tex_env_mode( _s_tex_env_mode_ui+1 );
//	return _s_tex_env_mode_ui;
//}
//
//INT32	c_map::dec_tex_env_mode()
//{
//	set_tex_env_mode( _s_tex_env_mode_ui-1 );
//	return _s_tex_env_mode_ui;
//}

//	TEX_REF
void	c_map::print_tex_mode()
{
	if( _b_verbose )
		SWITCH_PRINT_STRING( "Texture projection", tex_mode_str[_s_tex_method] );
}

void	c_map::set_tex_mode(INT32 value_in)
{
	_s_tex_method = (TEX_PROJ) IMOD( value_in, TEX_PROJ_NB );
	print_tex_mode();
}

INT32	c_map::inc_tex_mode()
{
	set_tex_mode( _s_tex_method + 1 );
	return _s_tex_method;
}

INT32	c_map::dec_tex_mode()
{
	set_tex_mode( _s_tex_method - 1 );
	return _s_tex_method;
}

//	TEX_AXE
void	c_map::print_tex_axe()
{
	if( _b_verbose )
		SWITCH_PRINT_STRING("Texture projection axe ", gstr::xyz[_tex_axe] );
}

void	c_map::set_tex_axe( INT32 value_in )
{
	_tex_axe = IMOD( value_in, 3 );
	print_tex_axe();
}

INT32	c_map::inc_tex_axe()
{
	set_tex_axe( _tex_axe + 1 );
	return _tex_axe;
}

INT32	c_map::dec_tex_axe()
{
	set_tex_axe( _tex_axe-1 );
	return _tex_axe;
}

void	c_map::set_uv( REAL u, REAL v )
{
	_uvw_ui[0] = u;
	_uvw_ui[1] = v;
}
void	c_map::set_uvw( REAL CONST* val )
{
	cpy_v3( _uvw_ui, val );
}
void	c_map::set_uv_ori( REAL u, REAL v )
{
	_uvw_ori_ui[0] = u;
	_uvw_ori_ui[1] = v;
}
void	c_map::set_uvw_ori( REAL CONST* val )
{
	cpy_v3( _uvw_ori_ui, val );
}

// TEX_WRAP
/*
void	c_map::print_wrap()
{
	if( _b_verbose )
		SWITCH_PRINT_STATE( "tex_wrap", b_wrap );
}

void	c_map::set_wrap( bool b )
{
	b_wrap = b;
	print_wrap();
}

bool	c_map::flip_wrap()
{
	set_wrap(!b_wrap);
	return b_wrap;
}
*/

//todo
//	infact move tex stuff with texture 

void	c_map::print_blend()
{
	if( _b_verbose )
		SWITCH_PRINT_STATE("Blend", _b_blend_ui );
}

void	c_map::set_blend( bool b )
{
	_b_blend_ui = b;
	print_blend();
}

bool	c_map::flip_blend()
{
	set_blend( !_b_blend_ui );
	return _b_blend_ui;
}

void	c_map::do_wrap_1d()
{
	tex1d.set_wrap( n_map::wrap_array[_s_wrap_ui[0]] );
//maa april 2020 done when we move tex to gpu in adjust
//	GOL::enable_anisotropic_filtering_1d();	//todo do we need this here
			
	if( is_lod_allow() )
	{
		glTexParameterf( GL_TEXTURE_1D, GL_TEXTURE_MIN_LOD, _lod_min_ui );
		glTexParameterf( GL_TEXTURE_1D, GL_TEXTURE_MAX_LOD, _lod_max_ui );
	}
	if( _b_force_min_mag_ui )
		tex1d.set_min_mag( _s_minification_ui, _s_magnification_ui );
}
void	c_map::do_wrap_2d()
{
	tex2d.set_wrap( n_map::wrap_array[_s_wrap_ui[0]], n_map::wrap_array[_s_wrap_ui[1]] );
//2021 we forced it to find a bug
//	GOL::set_tex_2d_wrap_s( n_map::wrap_array[_s_wrap_ui[0]] );
//	GOL::set_tex_2d_wrap_t( n_map::wrap_array[_s_wrap_ui[1]] );
//maa april 2020 done when we move tex to gpu in adjust
//	GOL::enable_anisotropic_filtering_2d();	//todo do we need this here

	if( is_lod_allow() )
	{
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_LOD, _lod_min_ui );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAX_LOD, _lod_max_ui );
	}
	if( _b_force_min_mag_ui )
		tex2d.set_min_mag( _s_minification_ui, _s_magnification_ui );
}
void	c_map::do_wrap_3d()
{
	tex3d.set_wrap( n_map::wrap_array[_s_wrap_ui[0]], n_map::wrap_array[_s_wrap_ui[1]], n_map::wrap_array[_s_wrap_ui[2]] );
//maa april 2020 done when we move tex to gpu in adjust
//	GOL::enable_anisotropic_filtering_3d();	//todo do we need this here
			
	if( is_lod_allow() )
	{
		glTexParameterf( GL_TEXTURE_3D, GL_TEXTURE_MIN_LOD, _lod_min_ui );
		glTexParameterf( GL_TEXTURE_3D, GL_TEXTURE_MAX_LOD, _lod_max_ui );
	}
	if( _b_force_min_mag_ui )
		tex3d.set_min_mag( _s_minification_ui, _s_magnification_ui );
}

