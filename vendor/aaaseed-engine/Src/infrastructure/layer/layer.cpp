
#include "aaa_def.h"
#include "layer.h"
#include "draw/axe.h"
#include "draw/clip.h"
#include "draw/color.h"
#include "draw/fog.h"
#include "draw/lights.h"
#include "draw/mesh.h"
#include "draw/map.h"
#include "draw/mat.h"
#include "draw/model.h"
#include "draw/picking.h"
#include "draw/render.h"
#include "draw/seedcam.h"
#include "draw/stencil.h"
#include "draw/tex_anim.h"
#include "image/bind_img_2d.h"
#include "draw/texture.h"
#include "infrastructure/layer/layers.h"
#include "infrastructure/layer/layer_att.h"
#include "obj_ui/bdd/util/factory_group_bdd.h"
#include "infrastructure/param/param_focus.h"
#include "time/speed.h"
#include "gol/gol_list.h"
#include "spy.h"
#include "obj_ui/bdd/bdd_point/bdd_mocap.h"
#include "obj_ui/transfo/transfo_trs.h"
#include "obj_ui/transfo/transfo_three.h"
#include "media/video/tex_video.h"
#include "shaders/shading.h"
#include "obj_ui/deformer/def_node.h"


bool	c_layer::b_allow_list = false;
bool	c_layer::b_force_list = false;
bool	c_layer::b_verbose_ui = false;

INT32	s_bbox_force = 0;
bool	gb_bbox_see_on_cur;

void	flip_bbox_see_on_cur()
{
	gb_bbox_see_on_cur = !gb_bbox_see_on_cur;
	SWITCH_PRINT_STATE( "See Bounding Box on cur", gb_bbox_see_on_cur );
}

FACTORY_CREATE_V1( c_layer, layer, Layer, layer );

c_layer*	c_layer::ui		= nullptr;
c_layer*	c_layer::cur	= nullptr;

namespace {
	CONSTEXPR	INT32	LOGIC_OP_NB_MAX = 16;
	CONSTEXPR	C_PCHAR_C	str_logic_op[LOGIC_OP_NB_MAX] =
	{
		"CLEAR",
		"AND",
		"AND_REVERSE",
		"COPY",
		"AND_INVERTED",
		"NOOP",
		"XOR",
		"OR",
		"NOR",
		"EQUIV",
		"INVERT",
		"OR_REVERSE",
		"COPY_INVERTED",
		"OR_INVERTED",
		"NAND",
		"SET",
	};

	CONSTEXPR	INT32	logic_op_array[LOGIC_OP_NB_MAX] =
	{
		GL_CLEAR,
		GL_AND,
		GL_AND_REVERSE,
		GL_COPY,
		GL_AND_INVERTED,
		GL_NOOP,
		GL_XOR,
		GL_OR,
		GL_NOR,
		GL_EQUIV,
		GL_INVERT,
		GL_OR_REVERSE,
		GL_COPY_INVERTED,
		GL_OR_INVERTED,
		GL_NAND,
		GL_SET,
	};
};

enum	BDD_USE_TYPE : INT32
{
	BDD_USE_CURRENT = 0,
	BDD_USE_NONE,
	BDD_USE_UPDATE_AND_DRAW,
	BDD_USE_UPDATE,
	BDD_USE_DRAW,
	BDD_USE_NB
};

namespace {
	CONSTEXPR	C_PCHAR_C	str_bdd_active[BDD_USE_NB] =
	{
		"CURRENT_draw",	//hack implement the rest
		"none",
		"update_and_draw",
		"update",
		"draw"
	};
};

enum	COLOR_MATERIAL_USE_MODE : INT32
{
	COLOR_MATERIAL_USE_EMISSION = 0,
	COLOR_MATERIAL_USE_AMBIENT,
	COLOR_MATERIAL_USE_AMBIENT_AND_DIFFUSE,
	COLOR_MATERIAL_USE_DIFFUSE,
	COLOR_MATERIAL_USE_SPECULAR,
	COLOR_MATERIAL_USE_MAX_NB
};

namespace {
	CONSTEXPR INT32 color_material_mode[COLOR_MATERIAL_USE_MAX_NB] =
	{
		GL_EMISSION,
		GL_AMBIENT,
		GL_AMBIENT_AND_DIFFUSE,
		GL_DIFFUSE,
		GL_SPECULAR
	};
	CONSTEXPR C_PCHAR_C color_material_mode_str[COLOR_MATERIAL_USE_MAX_NB] =
	{
		"Emission",
		"Ambient",
		"Ambient and diffuse",
		"Diffuse",
		"Specular"
	};
	CONSTEXPR	INT32 color_material_face[3] =
	{
		GL_BACK,
		GL_FRONT_AND_BACK,
		GL_FRONT
	};

	INT32	bind_1d_cur = 0;

	INT32	bind_2d_cur = 0;
	INT32	bank_2d_cur = 0;

	INT32	bind_3d_cur = 0;


	CONSTEXPR INT32 BASE_PARAM_NB			=	9;
	CONSTEXPR INT32 TIMING_PARAM_NB			=	3;
	CONSTEXPR INT32 LIGHT_MATERIAL_PARAM_NB	=	6;
	CONSTEXPR INT32 TEXTURE_PARAM_NB		=	12;
	CONSTEXPR INT32 TRANSFO_PARAM_NB		=	8;
	CONSTEXPR INT32 TEK_PARAM_NB			=	9;
	CONSTEXPR INT32 MISC_PARAM_NB			=	5;
	CONSTEXPR INT32 GROUP_PARAM_NB			=	6;
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_PARAM_NB
									+	LIGHT_MATERIAL_PARAM_NB
									+	TEXTURE_PARAM_NB
									+	TRANSFO_PARAM_NB
									+	MISC_PARAM_NB
									+	TEK_PARAM_NB
									+	TIMING_PARAM_NB
									+	GROUP_PARAM_NB;

	c_param_def param[PARAM_NB_MAX] =
	{
		//static pt is get_pt_active()
		PARAM_DEF_BOOL_OFF(			active			)
		PARAM_DEF_SYMBO_MIN_MAX(	use_bdd,		0, 1,	0, BDD_USE_NB-1,	str_bdd_active	)
		// where we attach dynamic object thru member pointer _bdd
		PARAM_DEF_CLASS_BRANCH(		bdd,			0, 8,	nullptr )	// not yet inited : (CHAR**)c_bdd::c_factory_group_bdd )

		PARAM_DEF_BOOL_OFF_STR(		use_model,		gstr::current )
		PARAM_DEF_SYMBO_PSTR(		use_deformer,	0, 1,	gstr::borrow	)
		PARAM_DEF_SYMBO_PSTR(		use_multiple,	0, 1,	gstr::borrow	)
		PARAM_DEF_BOOL_OFF_STR(		use_rendering,			gstr::current	)
		PARAM_DEF_BOOL_OFF_STR(		use_color,				gstr::current	)
		PARAM_DEF_SYMBO_PSTR(		use_shading,	1, 1,	gstr::borrow_once	)

		PARAM_DEF_GROUP( Texture, TEXTURE_PARAM_NB )
			PARAM_DEF_BOOL_OFF_STR(		use_mapping,				gstr::current	)
			PARAM_DEF_SYMBO_PSTR(		use_texturing,	0, 1,		gstr::borrow	)
			PARAM_DEF_SYMBO_PSTR(		use_tex_anim,	0, 1,		gstr::borrow	)
			PARAM_DEF_BIND_1D_CURRENT(	bind_1d			)
			PARAM_DEF_BANK_BIND_1D_OUT(	bind_1d_out		)
			PARAM_DEF_BANK_2D_CURRENT(	bank_2d			)	// careful max is set again in is_obj_first() below
			PARAM_DEF_BIND_2D_CURRENT(	bind_2d			)	// careful max is set again in is_obj_first() below
			PARAM_DEF_INT32(			bind_2d_offset,	1, 0,	PARAM_MIN_INT32, PARAM_MAX_INT32	)
			PARAM_DEF_BANK_BIND_2D_OUT(	bind_2d_out		)
			PARAM_DEF_BIND_3D_CURRENT(	bind_3d			)
			PARAM_DEF_BANK_BIND_3D_OUT(	bind_3d_out		)
			PARAM_DEF_BOOL_OFF(			use_tex_video	)

		PARAM_DEF_GROUP_CLOSED( Lights, LIGHT_MATERIAL_PARAM_NB )
			PARAM_DEF_BOOL_OFF_STR(	use_lights_switch, gstr::current )

			PARAM_DEF_BOOL_OFF(		material_use_color	)
			PARAM_DEF_SYMBO(		material_use_color_face,	0, 1,	2, gstr::front_and_back )
			PARAM_DEF_SYMBO(		material_use_color_dst,		COLOR_MATERIAL_USE_AMBIENT, COLOR_MATERIAL_USE_AMBIENT_AND_DIFFUSE,	COLOR_MATERIAL_USE_MAX_NB-1, color_material_mode_str )
			//module	add a flag for same front and back or SAME value to back
			PARAM_DEF_SYMBO_NEG(	material_front,	8, -1,	-1, MATERIAL_MAX_NB-1,	gstr::current	)
			PARAM_DEF_SYMBO_NEG(	material_back,	8, -1,	-1, MATERIAL_MAX_NB-1,	gstr::current	)

		PARAM_DEF_GROUP_CLOSED( Transfo, TRANSFO_PARAM_NB )	// separate object more generic
			PARAM_DEF_BOOL_ON(			push			)
			PARAM_DEF_BOOL_OFF(			camera_linked	)
			PARAM_DEF_SYMBO_MIN_MAX(	trs_1,				0, 1,	0, 2,	gstr::borrow	)
			PARAM_DEF_SYMBO_MIN_MAX(	transfo_three,		0, 1,	0, 2,	gstr::borrow	)
			PARAM_DEF_INT32(			mocap_channel,		1, 0,	0, c_bdd_mocap::MOCAP_CHANNEL_NB_MAX	)	//todo move this in transfo three ?
			PARAM_DEF_INT32(			mocap_node,			1, 0,	0, 128	)									//todo move this in transfo three ?
			PARAM_DEF_SYMBO_MIN_MAX(	trs_2,				0, 1,	0, 2,	gstr::borrow	)
			PARAM_DEF_BOOL_ON(			pop			)

		PARAM_DEF_GROUP_CLOSED( Misc, MISC_PARAM_NB )
			PARAM_DEF_SYMBO_MIN_MAX(	use_fog,		0, 1,	0, 2,	gstr::borrow	)
			PARAM_DEF_BOOL_OFF(			use_logic_op	)
			PARAM_DEF_SYMBO_MIN_MAX(	logic_op,		0, 3,	0, LOGIC_OP_NB_MAX-1, str_logic_op	)
			PARAM_DEF_SYMBO_MIN_MAX(	clip_plane,		1, 0,	0, 2,	gstr::borrow	)
			PARAM_DEF_SYMBO_MIN_MAX(	use_stencil,	0, 1,	0, 2,	gstr::borrow	)

		PARAM_DEF_GROUP_CLOSED( TEK, TEK_PARAM_NB + 1 + TIMING_PARAM_NB)
			PARAM_DEF_REF(			name_symbo		)
			PARAM_DEF_STR(			comment			)	
			PARAM_DEF_GROUP_CLOSED(	timing, TIMING_PARAM_NB )
				PARAM_DEF_NONE(			timing_layer	)
				PARAM_DEF_NONE(			timing_update	)
				PARAM_DEF_NONE(			timing_draw		)

			PARAM_DEF_BOOL_ON(		shader_uniform_update	)
			PARAM_DEF_BOOL_OFF(		shader_bdd_only			)
			PARAM_DEF_BOOL_ON(		origin_draw		)
			PARAM_DEF_BOOL_OFF(		use_list		)
			PARAM_DEF_UINT32_ZERO(	picking_ref		)

			PARAM_DEF_BOOL_OFF(		debug_break		)
			PARAM_DEF_INT32_LOCKED( layer_id_local	)	//todo add local_id as synonym
	};		//todo	all bdd factory have to be created before, this should be enforced instead of being lucky here

	PARAM_DEF_MAKE_INDEX( bdd				);
	PARAM_DEF_MAKE_INDEX( bank_2d			);
	PARAM_DEF_MAKE_INDEX( bind_2d			);
	PARAM_DEF_MAKE_INDEX( use_model			);
	PARAM_DEF_MAKE_INDEX( use_deformer		);
	PARAM_DEF_MAKE_INDEX( use_multiple		);
	PARAM_DEF_MAKE_INDEX( use_rendering		);
	PARAM_DEF_MAKE_INDEX( use_color			);
	PARAM_DEF_MAKE_INDEX( use_shading		);
	PARAM_DEF_MAKE_INDEX( texture			);
	PARAM_DEF_MAKE_INDEX( use_mapping		);
	PARAM_DEF_MAKE_INDEX( use_texturing		);
	PARAM_DEF_MAKE_INDEX( use_tex_anim		);
	PARAM_DEF_MAKE_INDEX( use_tex_video		);
	PARAM_DEF_MAKE_INDEX( bind_2d_out		);
	PARAM_DEF_MAKE_INDEX( lights			);
	PARAM_DEF_MAKE_INDEX( use_lights_switch			);
//	PARAM_DEF_MAKE_INDEX( material_use_color		);
	PARAM_DEF_MAKE_INDEX( material_use_color_face	);
	PARAM_DEF_MAKE_INDEX( material_use_color_dst	);
	PARAM_DEF_MAKE_INDEX( material_front			);
	PARAM_DEF_MAKE_INDEX( material_back				);
	PARAM_DEF_MAKE_INDEX( transfo					);
	PARAM_DEF_MAKE_INDEX( trs_1				);	
	PARAM_DEF_MAKE_INDEX( transfo_three		);
	PARAM_DEF_MAKE_INDEX( trs_2				);
	PARAM_DEF_MAKE_INDEX( misc				);
	PARAM_DEF_MAKE_INDEX( use_fog			);
	PARAM_DEF_MAKE_INDEX( clip_plane		);
	PARAM_DEF_MAKE_INDEX( use_stencil		);
	PARAM_DEF_MAKE_INDEX( timing			);
};

CONSTRUCTOR_CREATE( c_layer )	//INT32 value)
	//we need these because of comments added in param_init
	,_mat_front_ui		(	-1		)
	,_mat_back_ui		(	-1		)
	,_b_push_ui			(	false	)
	,_b_pop_ui			(	false	)
	,_b_logic_op_ui		(	false	)
	,_b_need_uv			(	false	)
	,_b_push			(	false	)
	,_b_pop				(	false	)
	,_b_transfo			(	false	)
	,_bind_1d_out		(	0		)
	,_bind_2d_out		(	0		)
	,_bank_2d_ui		(	0		)
	,_bind_3d_out		(	0		)
	,_def_cur			(	nullptr	)
	,_gl_list			(	0		)
	,_bdd				(	nullptr	)
	,_render			(	nullptr	)
	,_multiple			(	nullptr	)
	,_multiple_for_bdd	(	nullptr	)
	,_multiple_prev		(	nullptr	)
	,_texturing			(	nullptr	)
	,_shading			(	nullptr	)
	,_shading_last		(	nullptr	)
	,_b_shading_restore	(	false	)
	,_map				(	nullptr	)
	,_stencil			(	nullptr	)
	,_tex_video			(	nullptr	)
	,_model				(	nullptr	)
	,_model_for_bdd		(	nullptr	)
	,_color				(	nullptr	)
	,_color_for_bdd		(	nullptr	)
	,_lights_switch		(	nullptr	)
	,_tex_anim			(	nullptr	)
	,_tex_anim_last		(	nullptr	)
	,_fog				(	nullptr	)
	,_deformer_ui		(	nullptr	)
	,_clip				(	nullptr	)
//	,_bdd_prev			(	nullptr	)	// done in init
	,_transfo_trs1		(	nullptr	)
	,_transfo_three		(	nullptr	)
	,_transfo_trs2		(	nullptr	)
	,_mocap_direct		(	nullptr	)
	,_speed_layer		(	nullptr	)
	,_speed_draw		(	nullptr	)
	,_speed_update		(	nullptr	)
	,_speed_layer_ui	(	nullptr	)
	,_speed_draw_ui		(	nullptr	)
	,_speed_update_ui	(	nullptr	)

{
	if( is_obj_first() )
	{
		DBG_PRINT_STRING( "c_layer size %d", sizeof(c_layer) );

		//these 4 assignations have to be ready and stable for the rest of the session
		param[PARAM_INDEX_bdd].set_symbolic_str( (C_PCHAR_C*)c_bdd::factory_group );
		param[PARAM_INDEX_bdd].set_max( REAL(c_bdd::factory_group->get_item_nb() - 1) );
		//todo fibnd by name instead of hard coded index
		param[PARAM_INDEX_bank_2d].set_max( REAL(g_bind_img_2d->get_bank_nb()	- 1) );
		param[PARAM_INDEX_bind_2d].set_max( REAL(g_bind_img_2d->get_bank_size()	- 1) );

		c_layer_att::c_init();	//todo call deinit too
		//template this and move theses to private back
		//todonownow
		//todoq	solve this
		//	eventually they end up being used without getting updated first
		c_render::set_def( new c_render );
		c_render::set_cur( c_render::get_def() );
		c_render::set_ui( c_render::get_def() );

		c_multiple::ui	=  c_multiple::cur	=	c_multiple::def		= new c_multiple;

		c_map::set_def( new c_map );
		c_map::set_cur( c_map::get_def() );
		c_map::set_ui( c_map::get_def() );

		c_texturing::ui	=  c_texturing::cur	=	c_texturing::def	= new c_texturing;
		c_shading::ui	=  c_shading::cur	=	c_shading::def		= new c_shading;
		c_stencil::ui	=  c_stencil::cur	=	c_stencil::def		= new c_stencil;
		c_tex_video::ui	=  c_tex_video::cur	=	c_tex_video::def	= new c_tex_video;
		c_tex_anim::ui	=  c_tex_anim::cur	=	c_tex_anim::def		= new c_tex_anim;
		c_def_node::c_init();
		c_model::ui		=  c_model::cur		=	c_model::def		= new c_model;
		c_color::set_cur( c_color::ui = c_color::def = new c_color );
#if	!AAA_WATCHDOG()
		c_bdd_mocap::ui	=  c_bdd_mocap::cur	=	c_bdd_mocap::def	= new c_bdd_mocap;
#endif	//AAA_WATCHDOG
	}
	init();
	param_init_with( param, PARAM_NB_MAX );// layer_param, LAYER_PARAM_NB_MAX);
}

void	c_layer::build_sum_up( o_str& o ) CONST
{
	o.erase();
	if( !is_active() )
		o.add( "OFF " );
	if( _s_bdd_use_ui != BDD_USE_NONE )
	{
		switch( _s_bdd_use_ui  )
		{
		case BDD_USE_UPDATE:			o.add( "u_ " );	break;
		case BDD_USE_DRAW:				o.add( "d_ " );	break;
		case BDD_USE_UPDATE_AND_DRAW:
		default:						break;
		}
		if( _s_bdd_use_ui == BDD_USE_CURRENT )
			o.add( "cur " );
		else if( _s_bdd_use_ui != BDD_USE_NONE )
		{
			o.add( c_bdd::factory_group->get_str_ui(_s_bdd_ui) );
			o.add_space();
		}
	}
	if( _b_tex_video_ui )
		o.add( "VIDEO " );
	if( _b_model_ui )
		o.add( "MOD " );
	switch( _s_deformer_ui )
	{
	case 0:		o.add( "def " );	break;	// none
	case 2:		o.add( "DEF " );	break;	// Owner
	default:	break;
	}
	switch( _s_multiple_ui )
	{
	case 0:		o.add( "mul " );	break;
	case 2:		o.add( "MUL " );	break;
	default:	break;
	}
	if( _b_rendering_ui )
		o.add( "REN " );
	switch( _s_shading_ui )
	{
	case 0:		o.add( "cur " );	break;
	case 2:		o.add( "SHA " );	break;
	case 3:		o.add( "sha " );	break;
	default:	break;
	}
	if( _b_mapping_ui )
		o.add( "MAP " );
	switch( _s_texturing_ui )
	{
	case 0:		o.add( "tex " );	break;
	case 2:		o.add( "TEX " );	break;
	default:	break;
	}
	if( _b_color_ui )
		o.add( "COL " );
	if( _b_lights_switch_ui )
		o.add( "LIGHT " );
}

namespace
{
	o_str sum_up;	//todo problem not thread safe
}

#if 1
void	c_layer::param_init_pt_static()
{
	INT32	h=0;

	param_set_pt(				h, get_pt_active()	);
//	layer_bind_list.get_item(index) );

	param_set_pt(				h, _s_bdd_use_ui	);
	param_set_pt(				h, _s_bdd_ui		);
// MODEL
	param_set_pt(				h, _b_model_ui		);
	param_set_pt(				h, _s_deformer_ui	);
// MULTIPLE
	param_set_pt(				h, _s_multiple_ui	);
// RENDER
	param_set_pt(				h, _b_rendering_ui	);
// COLOR
	param_set_pt(				h, _b_color_ui		);
// SHADING
	param_set_pt(				h, _s_shading_ui	);
// TEXTURE
	++h;
	// MAP
		param_set_pt(				h, _b_mapping_ui				);
	// TEXTURING
		param_set_pt(				h, _s_texturing_ui				);
		param_set_pt(				h, _s_tex_anim_ui				);

		param_set_pt(				h, _bind_1d_ui					);
		param_set_pt(				h, _bind_1d_out					);

		param_set_pt(				h, _bank_2d_ui					);
		param_set_pt(				h, _bind_2d_ui					);
		param_set_pt(				h, _bind_2d_offset_ui			);
		param_set_pt(				h, _bind_2d_out					);

		param_set_pt(				h, _bind_3d_ui					);
		param_set_pt(				h, _bind_3d_out					);

		param_set_pt(				h, _b_tex_video_ui				);
//LIGHTS
	++h;
	// LIGHTS_SWITCH
		param_set_pt(				h, _b_lights_switch_ui			);
		param_set_pt(				h, _b_material_use_color_ui		);
		param_set_pt(				h, _material_use_color_face_ui	);
		param_set_pt(				h, _material_use_color_dst_ui	);
		param_set_pt(				h, _mat_front_ui				);
		param_set_pt(				h, _mat_back_ui					);
// TRANSFO
	++h;
		param_set_pt(				h, _b_push_ui					);
		param_set_pt(				h, _b_camera_linked_ui			);
		param_set_pt(				h, _s_transfo1_ui				);
		param_set_pt(				h, _s_transfo_three_ui			);
		param_set_pt(				h, _mocap_channel_ui			);
		param_set_pt(				h, _mocap_node_ui				);
		param_set_pt(				h, _s_transfo2_ui				);
		param_set_pt(				h, _b_pop_ui					);	
// MISC
	++h;
		param_set_pt(				h, _s_fog_ui					);
		param_set_pt(				h, _b_logic_op_ui				);
		param_set_pt(				h, _s_logic_op_ui				);
		param_set_pt(				h, _s_clip_ui					);
		param_set_pt(				h, _s_stencil_ui				);

// TEK
	++h;
		param_set_pt(				h, get_name_symbo()				);
		param_set_pt(				h, get_comment()				);

		// TIMING
		h += 1 + TIMING_PARAM_NB;

		param_set_pt(				h, _b_shader_uniform_update_ui	);
		param_set_pt(				h, _b_shader_bdd_only_ui		);
		param_set_pt(				h, _b_origin_draw_ui			);
		param_set_pt(				h, _b_use_list_ui				);
		param_set_pt(				h, _picking_ref_ui				);

		param_set_pt(				h, _b_debug_break_ui			);
		param_set_pt(				h, _id_local_ui					);

	err_param_init_pt( h );
}

//todo separate needed for coherency and for ui
void	c_layer::param_init_pt()
{
	param_attach_obj_no_inc(	PARAM_INDEX_bdd,				_bdd			);
// MODEL
	param_attach_obj_no_inc(	PARAM_INDEX_use_model,			_model			);
// DEFORMER
	param_attach_obj_no_inc(	PARAM_INDEX_use_deformer,		_deformer_ui	);
// MULTIPLE
	param_attach_obj_no_inc(	PARAM_INDEX_use_multiple,		_multiple		);
// RENDER
	param_attach_obj_no_inc(	PARAM_INDEX_use_rendering,		_render			);
// COLOR
	param_attach_obj_no_inc(	PARAM_INDEX_use_color,			_color			);
// SHADING
	param_attach_obj_no_inc(	PARAM_INDEX_use_shading,		_shading		);

	// MAP
	param_attach_obj_no_inc(	PARAM_INDEX_use_mapping,		_map			);
	// TEXTURING
	param_attach_obj_no_inc(	PARAM_INDEX_use_texturing,		_texturing		);

	param_attach_obj_no_inc(	PARAM_INDEX_use_tex_anim,		_tex_anim != c_tex_anim::def ? _tex_anim : nullptr );

	param_attach_obj_no_inc(	PARAM_INDEX_use_tex_video,		_tex_video		);

	param_attach_obj_no_inc(	PARAM_INDEX_use_lights_switch,	_lights_switch	);

	param_attach_obj_no_inc(	PARAM_INDEX_material_front,		( _mat_front >= 0 && c_materials::get_cur() ) ? c_materials::get_cur()->get_from_index( _mat_front ) : nullptr );
	param_attach_obj_no_inc(	PARAM_INDEX_material_back,		( _mat_back  >= 0 && c_materials::get_cur() ) ? c_materials::get_cur()->get_from_index( _mat_back  ) : nullptr );

// TRANSFO
	param_attach_obj_no_inc(	PARAM_INDEX_trs_1,				_transfo_trs1	);
	param_attach_obj_no_inc(	PARAM_INDEX_transfo_three,		_transfo_three	);
	param_attach_obj_no_inc(	PARAM_INDEX_trs_2,				_transfo_trs2	);
	
// MISC
	param_attach_obj_no_inc(	PARAM_INDEX_use_fog,			_fog			);
	param_attach_obj_no_inc(	PARAM_INDEX_clip_plane,			_clip			);
	param_attach_obj_no_inc(	PARAM_INDEX_use_stencil,		_stencil		);

// TEK
	//todo do should we procees this here ?
	INT32 h = PARAM_INDEX_timing;
	if( _speed_layer )
	{
		++h;
		_speed_layer_ui->update();
		param_attach_obj(	h, _speed_layer_ui	);
		_speed_update_ui->update();
		param_attach_obj(	h, _speed_update_ui	);
		_speed_draw_ui->update();
		param_attach_obj(	h, _speed_draw_ui	);
	}
	else
	{
		param_attach_obj(	h, nullptr );
		param_attach_obj(	h, nullptr );
		param_attach_obj(	h, nullptr );
	}
}

void	c_layer::prepare_for_ui()
{
// MODEL
	param_set_sum_up(		PARAM_INDEX_use_model,		_model,			_b_model_ui		);
// RENDER
	param_set_sum_up(		PARAM_INDEX_use_rendering,	_render,		_b_rendering_ui	);
// SHADING
	param_set_sum_up(		PARAM_INDEX_use_shading,	_shading,		_s_shading_ui>=2	);

// TEXTURE
	if( _b_mapping_ui && _map )
		_map->build_sum_up( sum_up );
	else
		sum_up.erase();
	get_param(				PARAM_INDEX_texture)->set_comment( sum_up );

	// MAP
	get_param(				PARAM_INDEX_use_mapping)->set_comment( sum_up );
	// TEXTURING
	param_set_sum_up(		PARAM_INDEX_use_texturing, _texturing,		_s_texturing_ui==2	);

	get_param(PARAM_INDEX_bind_2d)->set_comment( g_bind_img_2d->get_tex_index_str( _bank_2d_ui, _bind_2d_ui ) );

	c_img_2d*	img = g_bind_img_2d->get( _bind_2d_out );
	get_param(PARAM_INDEX_bind_2d_out)->set_comment(	img ? img->get_filename() : nullptr );


// MATERIAL
	if( _b_lights_switch_ui )
		sum_up.set("Lights Switch");
	else
		sum_up.erase();
	if( _mat_front_ui >= 0 )
	{
		sum_up.add_space();
		sum_up.add( _mat_front_ui );
		if( _mat_back_ui >= 0 )
		{
			sum_up.add_space();
			sum_up.add( _mat_back_ui );
		}
		else
			sum_up.add( " Cur" );
	}
	else
	{
		if( _mat_back_ui >= 0 )
		{
			sum_up.add( " Cur " );
			sum_up.add( _mat_back_ui );
		}
	}
	get_param(PARAM_INDEX_lights)->set_comment( sum_up );

	param_set_sum_up( PARAM_INDEX_use_lights_switch, _lights_switch, _b_lights_switch_ui );

	bool b = !_b_material_use_color_ui;
	get_param(PARAM_INDEX_material_use_color_face )->set_unused( b );
	get_param(PARAM_INDEX_material_use_color_dst )->set_unused( b );

// TRANSFO
	if(_b_push_ui &&  _b_push_ui != _b_pop_ui )
		sum_up.set( "Push ");
	else
		sum_up.erase();
	if( _b_camera_linked_ui )
		sum_up.add( "Cam " );
	if( _s_transfo1_ui==2 )
		sum_up.add( "Tr1 " );
	if( _s_transfo_three_ui==2 )
		sum_up.add( "Three " );

	if( _mocap_node_ui && _mocap_channel_ui )
	{
		sum_up.add( "Mocap(" );
		sum_up.add( _mocap_node_ui );
		sum_up.add_char( ',' );
		sum_up.add( _mocap_channel_ui );
		sum_up.add( ") " );
	}
	if( _s_transfo2_ui==2 )						
		sum_up.add( "Tr2 " );
	if( _b_pop_ui &&  _b_push_ui != _b_pop_ui )
		sum_up.add( "Pop");
	get_param(PARAM_INDEX_transfo)->set_comment( sum_up );
	
// MISC
	if( _b_shader_bdd_only_ui )
		sum_up.set( "Shader_bdd_only ");
	else							
		sum_up.erase();
	switch( _s_fog_ui )
	{
	case 0:	sum_up.add( "fog " );	break;
	case 2:	sum_up.add( "FOG " );	break;
	}
	if( _b_logic_op_ui )
	{
		sum_up.add( str_logic_op[_s_logic_op_ui] );
		sum_up.add_space();
	}
	if( _s_clip_ui==2 )	
		sum_up.add( "Clip " );
	get_param( PARAM_INDEX_misc )->set_comment( sum_up );

// TEK
	INT32 h = PARAM_INDEX_timing;
	if( _speed_layer )
	{
		_speed_layer->build_comment( sum_up, true, false );
		get_param(h)->set_comment( sum_up );
	}
	else
		get_param(h)->clear_comment();
}
#else
void	c_layer::param_init_pt()
{
	INT32	h=0;

	param_set_pt(				h, get_pt_active()	);
//	layer_bind_list.get_item(index) );

	param_set_pt(				h, _s_bdd_use_ui	);
	param_attach_obj_no_inc(	h, _bdd				);
	param_set_pt(				h, _s_bdd_ui		);
// MODEL
	param_attach_sum_up(		h, _model,			_b_model_ui		);
	param_set_pt(				h, _b_model_ui		);
	param_attach_obj_no_inc(	h, _deformer_ui		);
	param_set_pt(				h, _s_deformer_ui	);

// MULTIPLE
	param_attach_sum_up(		h, _multiple		);
	param_set_pt(				h, _s_multiple_ui	);
// RENDER
	param_attach_sum_up(		h, _render,			_b_rendering_ui	);
	param_set_pt(				h, _b_rendering_ui	);
// COLOR
	param_attach_obj_no_inc(	h, _color_ui			);
	param_set_pt(				h, _b_color_ui		);
// SHADING
	param_attach_sum_up(		h, _shading,		_s_shading_ui>=2	);
	param_set_pt(				h, _s_shading_ui	);

// TEXTURE
	if( _map )
		_map->build_sum_up( sum_up );
	else
		sum_up.erase();
	get_param(h)->set_comment( sum_up );
	++h;
	// MAP
		param_attach_sum_up(		h, _map				);
		param_set_pt(				h, _b_mapping_ui	);
	// TEXTURING
		param_attach_sum_up(		h, _texturing,		_s_texturing_ui==2	);
		param_set_pt(				h, _s_texturing_ui	);
		param_attach_obj_no_inc(	h, _tex_anim != c_tex_anim::def ? _tex_anim : nullptr );

		param_set_pt(				h, _s_tex_anim_ui				);
		param_set_pt(				h, _bind_1d_ui					);
//todo done in object first but dangerous
//		param_set_max_no_inc(		h, g_bind_img_2d->get_bank_nb()		- 1	);
		get_param(h)->set_comment( g_bind_img_2d->get_tex_index_str( _bank_2d_ui, _bind_2d_ui ) );
		param_set_pt(				h, _bank_2d_ui					);
//todo done in object first but dangerous
//		param_set_max_no_inc(		h, g_bind_img_2d->get_bank_size()	- 1	);
		c_img_2d*	img = g_bind_img_2d->get( _bind_2d_out );
		get_param(h)->set_comment(	img ? img->get_filename() : nullptr );
		param_set_pt(				h, _bind_2d_ui );
		param_set_pt(				h, _bind_2d_offset_ui			);
		param_set_pt(				h, _bind_2d_out					);

		param_set_pt(				h, _bind_3d_ui					);

		param_attach_obj_no_inc(	h, _tex_video					);
		param_set_pt(				h, _b_tex_video_ui				);

// MATERIAL
	if( _mat_front_ui >= 0 )
	{
		sum_up.set( _mat_front_ui );
		if( _mat_back_ui >= 0 )
		{
			sum_up.add_space();
			sum_up.add( _mat_back );
		}
		else
			sum_up.add( " Cur" );
	}
	else
	{
		if( _mat_back_ui >= 0 )
		{
			sum_up.set( "Cur " );
			sum_up.add( _mat_back );
		}
		else
			sum_up.erase();
	}
	get_param(h)->set_comment( sum_up );
	++h;
	// LIGHTS_SWITCH
		param_attach_sum_up(		h, _lights_switch,			_b_lights_switch_ui );
		param_set_pt(				h, _b_lights_switch_ui			);

		param_set_pt(				h, _b_material_use_color_ui		);
		param_set_pt(				h, _material_use_color_face_ui	);
		param_set_pt(				h, _material_use_color_dst_ui	);
		param_attach_obj_no_inc(	h, ( _mat_front >= 0 )	? c_materials::get_cur()->get_from_index( _mat_front ) : nullptr	);
		param_set_pt(				h, _mat_front_ui				);
		param_attach_obj_no_inc(	h, ( _mat_back >= 0 )	? c_materials::get_cur()->get_from_index( _mat_back  ) : nullptr );
		param_set_pt(				h, _mat_back_ui					);


// TRANSFO
	if(_b_push_ui &&  _b_push_ui != _b_pop_ui )
		sum_up.set( "Push ");
	else
		sum_up.erase();
	if( _b_camera_linked_ui )
		sum_up.add( "Cam " );
	if( _s_transfo1_ui==2 )
		sum_up.add( "Tr1 " );
	if( _s_transfo_three_ui==2 )
		sum_up.add( "Three " );

	if( _mocap_node_ui && _mocap_channel_ui )
	{
		sum_up.add( "Mocap(" );
		sum_up.add( _mocap_node_ui );
		sum_up.add_char( ',' );
		sum_up.add( _mocap_channel_ui );
		sum_up.add( ") " );
	}
	if( _s_transfo2_ui==2 )						
		sum_up.add( "Tr2 " );
	if( _b_pop_ui &&  _b_push_ui != _b_pop_ui )
		sum_up.add( "Pop");

	get_param(h)->set_comment(		sum_up );
	++h;
		param_set_pt(				h, _b_push_ui			);
		param_set_pt(				h, _b_camera_linked_ui	);
		param_attach_obj_no_inc(	h, _transfo_trs1		);
		param_set_pt(				h, _s_transfo1_ui		);
		param_attach_obj_no_inc(	h, _transfo_three		);
		param_set_pt(				h, _s_transfo_three_ui	);
		param_set_pt(				h, _mocap_channel_ui	);
		param_set_pt(				h, _mocap_node_ui		);
		param_attach_obj_no_inc(	h, _transfo_trs2		);
		param_set_pt(				h, _s_transfo2_ui		);
		param_set_pt(				h, _b_pop_ui			);
	
// MISC
	if( _b_shader_bdd_only_ui )
		sum_up.set( "Shader_bdd_only ");
	else							
		sum_up.erase();
	switch( _s_fog_ui )
	{
	case 0:	sum_up.add( "fog " );	break;
	case 2:	sum_up.add( "FOG " );	break;
	}
	if( _b_logic_op_ui )
	{
		sum_up.add( str_logic_op[_s_logic_op_ui] );
		sum_up.add_space();
	}
	if( _s_clip_ui==2 )	
		sum_up.add( "Clip " );
	get_param(h)->set_comment( sum_up );
	++h;
		param_attach_obj_no_inc(	h, _fog					);
		param_set_pt(				h, _s_fog_ui			);

		param_set_pt(				h, _b_logic_op_ui		);
		param_set_pt(				h, _s_logic_op_ui		);

		param_attach_obj_no_inc(	h, _clip				);
		param_set_pt(				h, _s_clip_ui			);

		param_attach_obj_no_inc(	h, _stencil				);
		param_set_pt(				h, _s_stencil_ui		);

// TEK
	++h;
		param_set_pt(				h, get_name_symbo()		);
		param_set_pt(				h, get_comment()		);

		if( _speed_layer )
		{
			_speed_layer->build_comment( sum_up, true, false );
			get_param(h)->set_comment( sum_up );
			++h;
			_speed_layer_ui->update();
			param_attach_obj(	h, _speed_layer_ui		);
			_speed_update_ui->update();
			param_attach_obj(	h, _speed_update_ui		);
			_speed_draw_ui->update();
			param_attach_obj(	h, _speed_draw_ui		);
		}
		else
		{
			get_param(h)->clear_comment();
			++h;
			param_attach_obj(	h, nullptr				);
			param_attach_obj(	h, nullptr				);
			param_attach_obj(	h, nullptr				);
		}

		param_set_pt(				h, _b_shader_uniform_update_ui	);
		param_set_pt(				h, _b_shader_bdd_only_ui		);
		param_set_pt(				h, _b_origin_draw_ui			);
		param_set_pt(				h, _b_use_list_ui				);
		param_set_pt(				h, _picking_ref_ui				);

		param_set_pt(				h, _b_debug_break_ui	);
		param_set_pt(				h, _id_local_ui	);

	err_param_init_pt( h );
}
#endif

void	c_layer::clear_draw_list()
{
	if( _gl_list )
	{
		GOL::delete_lists( _gl_list, 1 );
		_gl_list = 0;
	}
}

void	c_layer::init_speed()
{
	_speed_layer	= new c_speed( true, 3, g_speed_master->get_pb_print_layer(),	"total"		, this  );
	_speed_draw		= new c_speed( true, 4, g_speed_master->get_pb_print_layer(),	"draw"		, this  );
	_speed_update	= new c_speed( true, 4, g_speed_master->get_pb_print_layer(),	"update"	, this  );

	obj_get( _speed_layer_ui	);
	obj_get( _speed_draw_ui		);
	obj_get( _speed_update_ui	);

	_speed_layer_ui->set_timing(	_speed_layer	);
	_speed_draw_ui->set_timing(		_speed_draw		);
	_speed_update_ui->set_timing(	_speed_update	);
}

void	c_layer::init()
{
	_b_use_list				= false;
	_bdd					= c_bdd::bdd_empty;
	clear_draw_list();
	_s_bdd				= -1;
	_mat_front				= 0;
	_mat_back				= 0;
	_model_for_bdd			= nullptr;
	_color_for_bdd			= nullptr;
	_multiple_for_bdd		= nullptr;
	_id_local_ui			= 0;		// 0 when not attached at va higher level
	_bdd_prev				= nullptr;
	_b_normal_draw			= false;
	_b_normal_needed		= false;
}

c_layer::~c_layer()
{
	if( get_cur() == this )
		set_cur_null();
	if( get_ui() == this )
		set_ui_null();

	obj_delete( _deformer_ui );
	if( _render    != c_render::def		)
		obj_delete( _render );
	if( _multiple  != c_multiple::def	)
		obj_delete( _multiple );
	if( _map       != c_map::def		)
		obj_delete( _map );
	if( _texturing != c_texturing::def	)
		obj_delete( _texturing );
	if( _shading   != c_shading::def	)
		obj_delete( _shading );
	if( _stencil   != c_stencil::def	)
		obj_delete( _stencil );
	if( _tex_video != c_tex_video::def	)
	{
		if( _tex_video )
		{
#if	AAA_DEBUG()
			// this don't work anymore with c_node_ui added for tec_video
			//DBG_PRINT_STRING( "%s() deleting tex_video in %s/%s", __FUNCTION__, get_root()->get_root()->get_namer()->get_dir().get(), get_my_filename() );
			DBG_PRINT_STRING( "%s() deleting tex_video in %s", __FUNCTION__, get_name_dbg().get(), get_my_filename() );
#endif
			obj_delete( _tex_video );
		}
	}
	if( _color	   != c_color::def		)
		obj_delete( _color );
	obj_delete( _lights_switch );
	if( _tex_anim  != c_tex_anim::def	)
		obj_delete( _tex_anim );
	if( _model     != c_model::def		)
		obj_delete( _model );
	obj_delete( _fog );
	obj_delete( _clip );

	obj_delete( _transfo_three );
	obj_delete( _transfo_trs1 );
	obj_delete( _transfo_trs2 );

	for( auto const & elt : _bdds )
	{
#if	!AAA_WATCHDOG()
		if( elt.second != c_bdd_mocap::def )
#endif
			delete elt.second;
	}
	_bdds.clear();

	obj_delete( _speed_layer_ui		);
	obj_delete( _speed_draw_ui		);
	obj_delete( _speed_update_ui	);
	obj_delete( _speed_layer		);
	obj_delete( _speed_draw			);
	obj_delete( _speed_update		);

	if( is_obj_first() )
	{
		c_layer_att::c_deinit();	//todo call deinit too

		obj_delete( c_render	::def	);
		obj_delete( c_multiple	::def	);
		obj_delete( c_map		::def	);
		obj_delete( c_texturing	::def	);
		obj_delete( c_shading	::def	);
		obj_delete( c_stencil	::def	);
		obj_delete( c_tex_video	::def	);
		obj_delete( c_tex_anim	::def	);
		obj_delete( c_model		::def	);
		obj_delete( c_color		::def	);
#if	!AAA_WATCHDOG()
		obj_delete( c_bdd_mocap::def	);
#endif	//AAA_WATCHDOG
	}
}

bool	c_layer::param_do_action( c_param * CONST par, CONST aaa::param::ACTION action )
{
	if( (action == aaa::param::ACTION::PARAM_SIGN || action == aaa::param::ACTION::PARAM_OPEN) )
	{
		o_str CONST & o = par->get_name();
//		if( o.is_starting_with_nocase( "bind_2d", 7 ) || o.is_equal( "bank_2d" ) )
		if( o.is_equal( "bind_2d_out" ) )
		{
			g_bind_img_2d->get_bind()->set_index_for_next_load_save( _bind_2d_out );
			ask_type_io_load( aaa::file::TYPE_IO_TEXTURE_2D );
			return true;
		}
	}
	return false;
}



void	c_layer::set_bind_1d_ui( INT32 bind_index )
{
	if( bind_index >= 0 )
		bind_1d_cur = _bind_1d_out = bind_index;
	else
		_bind_1d_out = bind_1d_cur;
}

void	c_layer::set_bind_2d_ui( INT32 bind_index )
{
	bind_index -= _bind_2d_offset_ui;
	bind_index = g_bind_img_2d->get_index_valid( bind_index );
	g_bind_img_2d->split_index( bind_index, _bank_2d_ui, _bind_2d_ui );
	_bind_2d_out = g_bind_img_2d->make_tex_index( _bank_2d_ui, _bind_2d_ui ) + _bind_2d_offset_ui;
}

void	c_layer::set_bind_3d_ui( INT32 bind_index )
{
	if( _bind_3d_ui >= 0 )
		bind_3d_cur = _bind_3d_out = bind_index;
	else
		_bind_3d_out = bind_3d_cur;
}

FINLINE	void	c_layer::update_tex_bind()
{
//	1D
	if( _bind_1d_ui >= 0 )
		bind_1d_cur = _bind_1d_out = _bind_1d_ui;
	else
		_bind_1d_out = bind_1d_cur;

//	2D
	if( _bind_2d_ui >= 0 )
		bind_2d_cur = _bind_2d_ui;
	if( _bank_2d_ui >= 0 )
		bank_2d_cur = _bank_2d_ui;
	_bind_2d_out = g_bind_img_2d->make_tex_index( bank_2d_cur, bind_2d_cur ) + _bind_2d_offset_ui;

//	3D
	if( _bind_3d_ui >= 0 )
		bind_3d_cur = _bind_3d_out = _bind_3d_ui;
	else
		_bind_3d_out = bind_3d_cur;
}

void	c_layer::set_ui_with_cur()
{
	c_bdd::set_ui		(	c_bdd::get_cur()		);
	c_render::ui			= c_render::get_cur();
	c_multiple::ui			= c_multiple::cur;
	c_map::set_ui		(	c_map::get_cur()		);
	c_texturing::set_ui	(	c_texturing::get_cur()	);
	c_shading::set_ui	(	c_shading::get_cur()	);
	c_stencil::ui			= c_stencil::cur;
	c_tex_video::ui			= c_tex_video::cur;
	c_color::ui				= c_color::get_cur();
	c_fog::ui				= c_fog::cur;
	c_model::ui				= c_model::cur;
	c_tex_anim::ui			= c_tex_anim::cur;
	c_def_node::set_ui	(	c_def_node::get_cur()	);
	c_lights::set_ui	(	c_lights::get_cur()		);
	c_materials::set_ui	(	c_materials::get_cur()	);

#if	!AAA_WATCHDOG()
	c_bdd_mocap::ui			= c_bdd_mocap::cur;
#endif	//AAA_WATCHDOG
	c_lights_switch::ui		= c_lights_switch::cur;
}

void	c_layer::set_bdd_external( c_bdd* CONST bdd_in )
{
	_s_bdd = 0;
	_s_bdd_ui = 0;
	clear_draw_list();
	_bdd = bdd_in;
}

c_bdd*	c_layer::bdd_get_always( c_factory_base * const pf )
{
	auto it = _bdds.find( pf );
	if( it == _bdds.end() )
	{
		c_obj_ui* obj = nullptr;
		obj_get( obj, pf );
		return _bdds[pf] = (c_bdd*)obj;
	}
	else
		return it->second;
}

FINLINE	void	c_layer::bdd_set_by_index( INT32 CONST index )
{
	_s_bdd = index;
	clear_draw_list();
	if( index > 0 )
	{
		c_factory_base * CONST  factory = c_bdd::c_bdd::factory_group->get_factory( index );
		_bdd = factory ? bdd_get_always( factory ) : c_bdd::bdd_empty ;
		// //hack ? check it works without
		//	tex_2d_adjust();
		//todo	should not be here
		//	param_init_pt();	//call
	}
	else
	{
		_bdd = c_bdd::bdd_empty;
	}
}

void	c_layer::update()
{
	SPY_PUSH_RANGE_OBJ( "layer::update", spy::LAYER );
	if( _speed_update )
		_speed_update->begin();

	GOL::check_error_debug( "systematic check before c_layer::update()" );
	if( b_verbose_ui )
	{
		C_PCHAR_C str = get_my_filename();
		DBG_PRINT_STRING( "update layer %s", (str&&*str) ? str : "no filename" );
	}	

	cur = this;

	update_tex_bind();

	_multiple_prev = nullptr;
	switch( _s_multiple_ui )
	{
	case 0:	//	current
		_multiple_for_bdd = c_multiple::cur;
		break;
	case 2:	//	Owner
		obj_get( _multiple );
		_multiple->update();
		_multiple_for_bdd = c_multiple::cur;
		break;
	case 1:	//	none
		_multiple_prev = c_multiple::cur;
		c_multiple::cur = nullptr;
//		c_multiple::none.update();
		_multiple_for_bdd = nullptr;
		break;
	}
	if( _b_rendering_ui )
	{
		obj_get( _render );
		_render->update();
	}
	if( _b_tex_video_ui )
	{
		obj_get( _tex_video );
		_tex_video->update();
	}
	if( _b_mapping_ui )
	{
		obj_get( _map );
		GOL::set_tex_unit(0);
		_map->update();
	}
	switch( _s_texturing_ui )
	{
//	case 0:	//	CURRENT
//		break;
	case 1:	//	none
		c_texturing::disable();
		break;
	case 2:	//	Owner
		obj_get( _texturing );
		_texturing->update();
		break;
	}

	switch( _s_shading_ui )
	{
	case 0:	//	CURRENT
		break;
	case 1:	//	none
		c_shading::disable();
		break;
	case 2:	//	owner
	case 3:	//	Imce
		obj_get( _shading );
		{
			c_shading* cur = c_shading::get_cur();
			if( _shading != cur )
			{
				if( _s_shading_ui==3 )	//Once
				{
					_b_shading_restore = true;
					_shading_last = cur;
				}
				_shading->update();
				if( _shading->is_render_valid() )
					_shading->bind_render();
				else
					c_shading::disable();
			}
		}
		break;
	}

	if( _b_color_ui )	//todo introduce a base one
	{
		obj_get( _color );
		_color->update();
	}
	if( _b_lights_switch_ui )
	{
		obj_get( _lights_switch );
		_lights_switch->update();
	}
	switch( _s_tex_anim_ui )
	{
	case 0:	//current
		break;
	case 1:	//none
		_tex_anim_last = c_tex_anim::cur;
		c_tex_anim::def->update();
		break;
	case 2:	//owner
		obj_get( _tex_anim );
		_tex_anim->update();
		break;
	}
	switch( _s_fog_ui )
	{
	case 0:
		if( c_fog::cur )
			c_fog::cur->enable();
		break;
	case 1:
		if( c_fog::cur )
			c_fog::cur->disable();
		break;
	case 2:
		obj_get( _fog );
		_fog->update();
		if( c_fog::cur )
			c_fog::cur->enable();
		break;
	}
	//	deformer can use model data so iut should be updated before deformer
	if( _b_model_ui )
	{
		obj_get( _model );
		_model->update();
	}
	_def_cur = nullptr;
	switch( _s_deformer_ui )
	{
	case 1:	// none
		{
			_def_cur = c_def_node::get_cur();
			if( _def_cur->is_deforming() )	//todo optimize the deformer chain and update
				_def_cur->set_deforming( false );
			else
				_def_cur = nullptr;
		}
		break;
	case 2:	// Owner
		obj_get( _deformer_ui );
		_deformer_ui->update();
		break;
	}
	if( GOL::b_stencil_allow )
	{
		switch( _s_stencil_ui )
		{
		case 0:	if( c_stencil::cur )
					c_stencil::cur->update_state();
				break;
		case 1:	GOL::disable_stencil();
				break;
		case 2:	obj_get( _stencil );
				_stencil->update();
				break;
		}
	}
	if( _b_logic_op_ui )
	{
		GOL::enable_logic_op();
		GOL::logic_op( logic_op_array[_s_logic_op_ui] );
	}
	else
		GOL::disable_logic_op();

	//todo refine this 
	if( _s_clip_ui > 0 )
	{
		if( _s_clip_ui == 2 )
		{
			obj_get( _clip );
			_clip->update();
		}
		else
			c_clip::disable();
	}
	_model_for_bdd = c_model::cur;
	_color_for_bdd = c_color::get_cur();
	//todo check order, rethink for multilayer
	//	dependencies should be solved automatically
//	if ( b_model || s_bdd != 1 )
// was

	bool b_bdd_update;
	if( _s_bdd_use_ui == BDD_USE_NONE )
	{
		//to see it in flatland
		if( _s_bdd_ui != _s_bdd )
			bdd_set_by_index( _s_bdd_ui );
		b_bdd_update = false;
	}
	else
	{
		//todo perhaps the update should be done also when mapping change for ex
		if( _s_bdd_use_ui == BDD_USE_CURRENT )
		{
			if( c_layer_att::is_bdd_cur_equal_up()  )	//todo	do better eventually have a multiple stack and parse it ?
				ERR_PRINT_STRING( "CURRENT bdd is doing multiple with itself: skipping update in %s", get_my_filename() );
			b_bdd_update = false;
		}
		else
		{
			if( _s_bdd_ui != _s_bdd )
				bdd_set_by_index( _s_bdd_ui );
			if( _bdd != c_bdd::get_cur() )
			{
				_bdd_prev = c_bdd::get_cur();
				c_bdd::set_cur( _bdd );
			}
			b_bdd_update = ( _s_bdd_use_ui != BDD_USE_DRAW );
		}
	}

	_b_normal_draw		=	c_map::get_cur()->is_normal_needed() || c_render::get_cur()->is_draw_need_normal();
	_b_normal_needed	=	_b_normal_draw || c_render::get_cur()->is_normal_needed();
	if( b_bdd_update )
	{	
		c_bdd* bdd = c_bdd::get_cur();
		//todo	was crashing without test in EO
		if( bdd )
			bdd->update();
	}

	_b_transfo = _b_camera_linked_ui;
	switch( _s_transfo1_ui )
	{
	case 2:
		obj_get( _transfo_trs1 );
		_transfo_trs1->update();
		c_transfo_trs::transfo1_cur = _transfo_trs1;
	case 0:
		_b_transfo |= c_transfo_trs::transfo1_cur->is_todo();
		break;
	}
	switch( _s_transfo_three_ui )
	{
	case 2:
		obj_get( _transfo_three );
		_transfo_three->update();
		c_transfo_three::cur = _transfo_three;
	case 0:
		_b_transfo |= c_transfo_three::cur->is_todo();
		break;
	}
	switch( _s_transfo2_ui )
	{
	case 2:
		obj_get( _transfo_trs2 );
		_transfo_trs2->update();
		c_transfo_trs::transfo2_cur = _transfo_trs2;
	case 0:
		_b_transfo |= c_transfo_trs::transfo2_cur && c_transfo_trs::transfo2_cur->is_todo();
		break;
	}
#if	!AAA_WATCHDOG()
	if( _mocap_node_ui && _mocap_channel_ui )
	{
		_mocap_direct = c_bdd_mocap::get_from_channel(_mocap_channel_ui );
		_b_transfo = true;
	}
	else
		_mocap_direct = nullptr;
#endif	//AAA_WATCHDOG
	if( _b_transfo )
	{
		_b_push = _b_push_ui;
		_b_pop = _b_pop_ui;
	}
	else if( _b_push_ui == _b_pop_ui )
	{
		_b_push = false;
		_b_pop = false;
	}
	else
	{
		_b_push = _b_push_ui;
		_b_pop = _b_pop_ui;
	}

	//check maa added 2011 not sure 
	if( this == c_layer::get_ui() && !c_picking::is_cur() )
		set_ui_with_cur();

	GOL::check_error_debug( "systematic check after c_layer::update()" );

	if( _speed_update )
		_speed_update->end();
	SPY_POP_RANGE2();
}
	
FINLINE	void	c_layer::bdd_draw()
{
//clean to revive when move out of seed
//	hmd_move_object();

	c_bdd* bdd = c_bdd::get_cur();

	if( s_bbox_force )
		bdd->draw_bbox();
	else
	{
		if( gb_bbox_see_on_cur && c_layer::get_ui() == this )
		{
			/* GOL::push_attrib( GL_ENABLE_BIT
								| GL_LIGHTING_BIT
								| GL_POLYGON_BIT
								| GL_TEXTURE_BIT
								| GL_LINE_BIT
								| GL_CURRENT_BIT  );
			*/
			GOL::push_att();
				GOL::push_lighting( false );
				GOL::push_texture_dim( 0 );
			
				FP32 tmp = FSAW( FP32(aaa::time::get()) );
				GOL::push_color3( tmp, FP32(1.-tmp), tmp );

				GOL::push_polygon_mode( GL_FRONT_AND_BACK, GL_LINE );
				bdd->draw_bbox();
			GOL::pop_att();
		}
//don't work why ?
//extern	REAL	dummy_float[];
//		glTexEnvf( GL_TEXTURE_FILTER_CONTROL_EXT, GL_TEXTURE_LOD_BIAS_EXT, dummy_float_[0] );
//		glTexEnvf( GL_TEXTURE_FILTER_CONTROL, GL_TEXTURE_LOD_BIAS, dummy_float_[0] );

		if( b_verbose_ui )
		{
			C_PCHAR_C str = bdd->get_my_filename();
			DBG_PRINT_STRING( "\tbdd_draw %s", (str&&*str) ? str : "no filename" );
		}

		bdd->draw_before();		//	used for bdd_tri
		if( b_verbose_ui )
			DBG_PRINT_STRING( "\tdraw_before() done" );
		bdd->draw();
		if( b_verbose_ui )
			DBG_PRINT_STRING( "\tdraw() done" );

		c_render* render = c_render::get_cur();
		if( render->is_top_draw() )
		{
			if( render->is_top() )
			{
				render->begin_top();
				if( render->is_top_line() )
				{
					render->begin_top_line();
					bdd->draw();	//todoopt	pass a pointer to render fn to render obj
					render->end_top_line();
				}
				if( render->is_top_point() )
				{
					render->begin_top_point();
					bdd->draw();
					render->end_top_point();
				}
				render->end_top();
			}
			//	bdd_cur->draw_after();

			if( render->is_top_normal_draw() )
			{
				render->normal_begin();
				{
					REAL len;
					//	POINT
					len = render->get_normal_len_point();
					if( len != 0. )
						bdd->draw_normal_point( len );
					//	FACE
					len = render->get_normal_len_face();
					if( len != 0. )
						bdd->draw_normal_face( len );
				}
				render->normal_end();
			}
		}
		else
		{
			//	bdd_cur->draw_after();
		}
	}

	GOL::check_error_debug( "systematic check after c_layer::bdd_draw()" );
}

void	c_layer::draw()
{
	SPY_PUSH_RANGE_OBJ( "layer::draw", spy::LAYER );
	if( _speed_draw )
		_speed_draw->begin();

	bool	b_draw;
	switch( _s_bdd_use_ui )
	{
	case BDD_USE_CURRENT:
		b_draw = !c_layer_att::is_bdd_cur_equal_up();	//todo	do better eventually have a multiple stack and parse it ?
		if( !b_draw )	// or flag bdd_multiple so they never call them back
			ERR_PRINT_STRING( "CURRENT bdd is doing multiple with itself : skipping draw in %s", get_my_filename() );
		break;
	case BDD_USE_UPDATE_AND_DRAW:
	case BDD_USE_DRAW:
		b_draw = true;
		break;
	case BDD_USE_NONE:
	case BDD_USE_UPDATE:
	default:
		b_draw = false;
		break;
	}
//	if( !b_draw )
//		return;

	if( _b_push )
		c_seedcam::get_cur()->push_matrix();
	if( _b_transfo )
	{
		c_seedcam::get_cur()->before_transfo();
		if( _b_camera_linked_ui )
			GOL::matrix::load_identity();
		if( _s_transfo1_ui != 1 )
			c_transfo_trs::transfo1_cur->do_it();
		if( _mocap_direct )
			_mocap_direct->transfo_do( _mocap_node_ui );
		if( _s_transfo_three_ui != 1 )
			c_transfo_three::cur->do_it();
		if( _s_transfo2_ui != 1 )
			c_transfo_trs::transfo2_cur->do_it();
	}

	//hack	this was done always ( 30 MArch 2002 )
	c_seedcam::get_cur()->after_transfo( _b_transfo );

	//hack, do we bind shaders here ?
	//todo same nearly in texture.cpp should both move to map
	c_map*	map = c_map::get_cur();
	INT32	dim = map->get_tex_use();
	switch( dim )	//todotex
	{
	case 0:
		map->draw_0d();
		break;
	case 1:
		tex_1d_bind( _bind_1d_out );
		map->draw_1d();
		break;
	case 2:
		//todoq		if ( b_force_reload_ui )
		if( map->is_texture_feedback() )
			tex_2d_bind( -1 );
		else if( _s_tex_anim_ui == 1 || !c_tex_anim::cur->is_active() )	//todo check tex_anim_strategie		
		{
			INT32 bind = get_bind_2d();
#if 0
			// useful in punctual debug
			if( bind==137 )
				tex_2d_bind_no_gpu_move( 137 );
#endif
			tex_2d_bind( bind );
		}
		map->draw_2d();
		break;
	case 3:
		tex_3d_bind( get_bind_3d() );
		map->draw_3d();
		break;
	}

	switch( _s_texturing_ui )
	{
	case 0:	//	CURRENT
	//	if( c_texturing* texturing = c_texturing::get_cur() )
	//		texturing->draw_only_active();	//	update and draw already done we try less
		break;
	case 1:	//	none
		c_texturing::disable();
		break;
	case 2:	//	Owner
		_texturing->draw();	//	rest done in update
		break;
	}

	_b_need_uv = GOL::texcoord_update();	//prepare the right fns pointer

	auto materials = c_materials::get_cur();
	if( materials )
	{
		if( _mat_back_ui >= 0 )
		{
			_mat_back  =  _mat_back_ui;
			_mat_front = (_mat_front_ui >= 0 ) ? _mat_front_ui : materials->get_front() ; 	
			materials->set( _mat_front, _mat_back );
		}
		else
		{
			_mat_back  = materials->get_back();
			if( _mat_front_ui >= 0 )
			{
				_mat_front = _mat_front_ui; 	
				materials->set( _mat_front, _mat_back );
			}
			else
				_mat_front = materials->get_front() ;
		}

		//if( c_render::get_cur()->is_light() )
		{
			if( _b_material_use_color_ui )
			{
				GOL::color_material( color_material_face[_material_use_color_face_ui], color_material_mode[_material_use_color_dst_ui] );
				GOL::enable_color_material();
			}
			else
				GOL::disable_color_material();
		}
	}
	else
	{
		GOL::disable_color_material();
		dbg_print( "%() No Materials", __FUNCTION__ );
	}

	//todo do we need to do it ? c_color::cur ?
	if( _b_color_ui )
		_color->draw();

//	GOL::enable(GL_NORMALIZE);

	if( g_app->get_origin_visible() && ( ( g_app->get_origin_visible()==2 && _b_origin_draw_ui ) || this==c_layer::get_ui() ) )
		n_axe::draw_origin( g_app->get_origin_size() );


	if( _b_shader_uniform_update_ui )
	{
		c_shading* shading = c_shading::get_cur();
		if( shading )
		{
			c_shading::update_from_map( 0, map );
		//	GOL::check_error_debug( "systematic check before shading->update_uniforms()" );
			shading->update_uniform_render();
		//	GOL::check_error_debug( "systematic check after shading->update_uniforms()" );
		}
	}
	
	if( b_draw )	// CURRENT or UPDATE_AND_DRAW or DRAW 
	{	
		if( c_picking::is_cur() )
		{
#if	AAA_PICKING()
			if( c_picking::cur_is_by_id() )
				c_picking::cur_set_pick_ref( get_obj_ui_id() );
			else if( _picking_ref_ui )
				c_picking::cur_set_pick_ref( _picking_ref_ui );
#endif
		}

		if( _b_use_list )
		{
			if( _gl_list )
				GOL::call_list( _gl_list );
			else	
			{
				_gl_list = GOL::gen_lists( 1 );
				if( _gl_list )	GOL::new_list( _gl_list, GL_COMPILE_AND_EXECUTE );

					bdd_draw();

				if( _gl_list )	GOL::end_list();
			}
		}
		else
			bdd_draw();
	}

	//todo if we update using _s_shading_ui nce only the first draw will function
	if( _b_shading_restore )
	{
		_b_shading_restore = false;
		if( _shading_last )
		{
			_shading_last->update();
			if( _shading_last->is_render_valid() )
				_shading_last->bind_render();
			else
				c_shading::disable();
		}
		else
			c_shading::disable();
	}
	if( _b_pop )
		c_seedcam::get_cur()->pop_matrix();
	if( _def_cur )
		_def_cur->set_deforming( true );
	if( _s_tex_anim_ui == 1 )
	{
		//avoid null cur but at the price of a test every draw
		// Maa try to make sure _tex_anim_last is never null
		//c_tex_anim::cur = _tex_anim_last;
		
		//auto tex_anim = _tex_anim_last ? _tex_anim_last : c_tex_anim::def;
		//if( c_tex_anim::cur != tex_anim )
		//	tex_anim->update();

		c_tex_anim::cur = _tex_anim_last ? _tex_anim_last : c_tex_anim::def;

	}
	if( _multiple_prev )
		c_multiple::cur = _multiple_prev;

//	if( _b_texturing )
//		c_texturing::disable();

//	if( b_feedback_ && b_allow_feedback )
//		feedback_->draw();
	if( GOL::b_check_error_by_layer )
		GOL::get_error( "check after c_layer::draw()" );

	if( _speed_draw )
		_speed_draw->end();
	SPY_POP_RANGE2();
}

void	c_layer::update_draw_shader_bdd()
{
	SPY_PUSH_RANGE_OBJ( "layer::update_draw_shader_bdd", spy::LAYER );
//
//	UPDATE
//
	GOL::check_error_debug( "systematic check before c_layer::update_draw_shader_bdd()" );

	cur = this;

	update_tex_bind();

	//	no bind we assume current
	//	no multiple ?
	//	no rendering
	//	no tex_video
	//	no map
	//	no texturing
	c_shading* shading;
	switch( _s_shading_ui )
	{
	case 0:	//	current
		shading = c_shading::get_cur();
		break;
	case 1:	//	none
		c_shading::disable();
		shading = nullptr;
		break;
	case 2:	//	owner
	case 3:	//	Once
		obj_get( _shading );
		{
			c_shading* cur = c_shading::get_cur();
			if( _shading == cur )
				shading = nullptr;
			else
			{
				if( _s_shading_ui==3 )	//Once
				{
					_b_shading_restore = true;
					_shading_last = cur;
				}
				_shading->update();
				if( _shading->is_render_valid() )
				{
					_shading->bind_render();
					shading = _shading;
				}
				else
				{
					c_shading::disable();
					shading = nullptr;
				}
			}
		}
		break;
	}

	//	no color
	//	no lights switch
	//	no tex_anim
	//	no fog
	//	no model
	//	no deformer
	//	no stencil
	//	no logic_op
	//	no clip
	_model_for_bdd = c_model::cur;
	_color_for_bdd = c_color::get_cur();

	//todo check order, rethink for multilayer
	//	dependencies should be solved automatically
	//	if ( b_model || s_bdd != 1 )
	// was
	bool b_update;
	if( _s_bdd_use_ui == BDD_USE_NONE )
	{
		if( _s_bdd_ui != _s_bdd )
			bdd_set_by_index( _s_bdd_ui );
		b_update = false;
	}
	else
	{
		//todo perhaps the update should be done also when mapping change for ex
		if( _s_bdd_use_ui == BDD_USE_CURRENT )
		{
			if( c_layer_att::is_bdd_cur_equal_up() )	//todo	do better eventually have a multiple stack and parse it ?
				ERR_PRINT_STRING( "CURRENT bdd is doing multiple with itself: skipping update in %s", get_my_filename() );
			b_update = false;
		}
		else
		{
			if( _s_bdd_ui != _s_bdd )
				bdd_set_by_index( _s_bdd_ui ); 
			if( _bdd != c_bdd::get_cur() )
			{
				_bdd_prev = c_bdd::get_cur();
				c_bdd::set_cur( _bdd );
			}
			b_update = ( _s_bdd_use_ui != BDD_USE_DRAW );
		}
	}
//	c_map::get_cur()->update_implicit();	//todotex
	_b_need_uv = GOL::texcoord_update();	//prepare the right fns pointer
	_b_normal_draw		=	c_map::get_cur()->is_normal_needed() || c_render::get_cur()->is_draw_need_normal();
	_b_normal_needed	=	_b_normal_draw || c_render::get_cur()->is_normal_needed();
	if( b_update )
	{	
		//todo	was crashing without test in EO
		if( c_bdd::get_cur() )
			c_bdd::get_cur()->update();
	}

	//	no transfo
	//check maa added 2011 not sure 
	if( this == c_layer::get_ui() && !c_picking::is_cur() )
	{
		set_ui_with_cur();
	}
	GOL::check_error_debug( "systematic check after update in c_layer::update_draw_shader_bdd()()" );

//
//	DRAW
//
	bool	b_draw;
	switch( _s_bdd_use_ui )
	{
	case BDD_USE_CURRENT:
		b_draw = !c_layer_att::is_bdd_cur_equal_up();	//todo	do better eventually have a multiple stack and parse it ?
		if( !b_draw )	// or flag bdd_multiple so they never call them back
			ERR_PRINT_STRING( "CURRENT bdd is doing multiple with itself : skipping draw in %s", get_my_filename() );
		break;
	case BDD_USE_UPDATE_AND_DRAW:
	case BDD_USE_DRAW:
		b_draw = true;
		break;
	case BDD_USE_NONE:
	case BDD_USE_UPDATE:
	default:
		b_draw = false;
		break;
	}
	//	if( !b_draw )
	//		return;

	//	no transfo
	//hack	this was done always ( 30 MArch 2002 )
	//c_seedcam::get_cur()->after_transfo( _b_transfo );

#if 0
//hack, do we bind shaders here ?
	//todo same nearly in texture.cpp should both move to map
	c_map*	map = c_map::cur;
	INT32	dim = map->get_tex_use();

	switch( dim )	//todotex
	{
	case 0:
		GOL::set_texture_0D();
		break;
	case 1:
		tex_1d_bind( _bind_1d );
		map->tex_draw();
		break;
	case 2:
		//todoq		if ( b_force_reload_ui )
		if ( map->is_texture_feedback() )
			tex_2d_bind( -1 );
		else if( _s_tex_anim == 1 || !c_tex_anim::cur->is_active() )	//todo check tex_anim_strategie
		{
			if( _b_do_texture_2d )
				tex_2d_bind( get_bind_2d() );
		}
		map->do_wrap();
		map->tex_draw();
		break;
	case 3:
		tex::bind_3d( get_bind_3d() );
/*
		if ( map->is_implicit() )
		{
			GOL::disable_texture_gen_s();
//			GOL::disable_texture_gen_t();
//			GOL::disable_texture_gen_q();
		}
		else
		{
			GOL::enable_texture_gen_s();
//			GOL::enable_texture_gen_t();
//			GOL::enable_texture_gen_q();
		}
*/
		//todo avoid systematic call
		map->do_wrap_3d();
		map->tex_draw();
		break;
	}

	c_shading::update_from_map( 0, map );
#endif

	//todo check
//	if( c_texturing* texturing = c_texturing::get_cur() )
//		texturing->draw_only_active();	//	update and draw already done we try less

	if( _b_shader_uniform_update_ui )
	{
		if( shading )
		{
			c_shading::update_from_map( 0, c_map::get_cur() );
		//	GOL::check_error_debug( "systematic check before shading->update_uniforms()" );
			shading->update_uniform_render();
		//	GOL::check_error_debug( "systematic check after shading->update_uniforms()" );
		}
	}
	if( b_draw )
	{
		bdd_draw();
	}

	if( _b_shading_restore )
	{
		_b_shading_restore = false;
		if( _shading_last )
		{
			_shading_last->update();
			if( _shading_last->is_render_valid() )
				_shading_last->bind_render();
			else
				c_shading::disable();
		}
		else
			c_shading::disable();
	}

//	if( _b_texturing )
//		c_texturing::disable();

	if( GOL::b_check_error_by_layer )
		GOL::get_error( "check after c_layer::update_draw_shader_bdd()" );

	SPY_POP_RANGE2();
}

void	c_layer::update_then_draw()
{
	if( _speed_layer )
		_speed_layer->begin();
#if AAA_DEBUG()
	if( _b_debug_break_ui )
		debug_break( "layer asked break" );
#endif
	if( _b_shader_bdd_only_ui )
		update_draw_shader_bdd();
	else
	{
		update();
		draw();
	}
	
	if( _speed_layer )
		_speed_layer->end();
}

void	c_layer::bdd_switch_list()
{
	_b_use_list_ui = !_b_use_list_ui;
	SWITCH_PRINT_STATE( "List", _b_use_list_ui );
	clear_draw_list();
}

void	c_layer::bdd_set_focus() CONST
{
	IF_THIS_NULL_RETURN();
	
	if( c_bdd_empty::is_instance( _bdd ) )
		DBG_PRINT_STRING( "Layer Can't focus on empty BDD" );
	else
		_bdd->set_focus();
	
}

extern void	bdd_menu_change( INT32 index_in );

void	c_layer::bdd_menu_update()
{
	bdd_menu_change( _s_bdd_ui );
}


void	c_layer::bdd_switch( INT32 CONST bdd_ind )
{
#if	!AAA_DEMO_LOCKED()
//	menu_set( sub_bdd_old );
// 	menu_change_to_menu_entry( bdd_get_order_from_ref(bdd_index)+1, bdd_get_str_from_ref(bdd_index), MENU_BASE_MODEL+bdd_index);
#endif

	_s_bdd_ui = IMOD( bdd_ind, c_bdd::c_bdd::factory_group->get_item_nb() );

	//	make sure the default bdd is a default bdd
	bdd_menu_update();
}

void	c_layer::bdd_switch( C_PCHAR_C sel )
{
	bdd_switch( c_bdd::factory_group->get_index_from_str( sel ) );
}

void	c_layer::bdd_switch_add( INT32 CONST i )
{
	//if( this )
	bdd_switch( get_bdd_selector()+i );
}

namespace {
	CONST CHAR  trs1_ext[]	= "transfo_trs1";
	CONST CHAR  three_ext[]	= "transfo_three";
	CONST CHAR  trs2_ext[]	= "transfo_trs2";
};

AAA_ERR	c_layer::save_to_file( o_str CONST & filename_in )
{
#if	!AAA_DEMO()
	o_str& filename = o_str::push_name();

		c_dir::change_to_def();
		filename.set_fname_relative_ext_no( filename_in );

		for( auto const & elt : _bdds )
			elt.second->save_to_file_add_ext( filename );

		if_obj_save_add_ext(	_deformer_ui,	filename );
		if_obj_save_add_ext(	_tex_anim,		filename );
		if_obj_save_add_ext(	_render,		filename );
		if_obj_save_add_ext(	_multiple,		filename );
		if_obj_save_add_ext(	_map,			filename );
		if_obj_save_add_ext(	_texturing,		filename );
		if_obj_save_add_ext(	_shading,		filename );
		if_obj_save_add_ext(	_stencil,		filename );
		if_obj_save_add_ext(	_model,			filename );
		if_obj_save_add_ext(	_color,			filename );
		if_obj_save_add_ext(	_lights_switch,	filename );
		if_obj_save_add_ext(	_fog,			filename );
		if_obj_save_add_ext(	_clip,			filename );
		if_obj_save_add_ext(	_tex_video,		filename );

		if_obj_save_add_ext(	_transfo_trs1,	filename, trs1_ext	);
		if_obj_save_add_ext(	_transfo_three,	filename, three_ext );
		if_obj_save_add_ext(	_transfo_trs2,	filename, trs2_ext	);

		c_obj_ui::save_to_file_add_ext(	filename );
#endif
	o_str::pop_name();

	return AAA_OK;
}

bool	c_layer::b_preload_data = false;

void c_layer::load_bdd( c_factory_base * CONST pf, o_str CONST & filename_in )
{
	c_obj_ui* obj = nullptr;
	try_obj_load_add_ext( obj, pf, filename_in );
	if( obj )
		_bdds[pf] = (c_bdd*)obj;
}

//todo there is no check 
AAA_ERR	c_layer::layer_load_from_existing_file( o_str CONST & filename_in, bool b_load_all )
{

//	c_file::dir_change_to_def();

	c_obj_ui::load_from_existing_file( filename_in );

	o_str& filename = o_str::push_name();
		filename.set_fname_relative_ext_no( filename_in );

		//todo explain or remove
		//boul
		if( _s_bdd_ui < 0  )
		{
			BOX_WAR( "On layer %d the bdd %d don't exists, update this layer", _id_local_ui, _s_bdd_ui );
			_s_bdd_ui = 0;
			b_load_all = true;
		}
/*
		extern	void	dir_build_list();
		extern	void	dir_build_list(std::vector<o_str>& vec);
		c_file::dir_push_def_from_filename(  filename_in);	//can pass just a dir name
			std::vector<o_str> vec;
			dir_build_list( vec);
		c_file::dir_pop_def();
*/
		
		SPY_PUSH_RANGE_OBJ( "load bdd", spy::LAYER );

			if( b_load_all )
			{
				C_PCHAR_C name = filename.get() + filename.get_len() - 4;
				if( name[0] == 'f' && name[1] == 'x' && name[2] == '_' && name[4] == 0 )
				{
					INT32 index = name[3] - 97;
					c_factory_base ** ppf = c_file_virtual::get_bdd_factories(index);
					o_str& bdd_filename = o_str::push_name( filename );
					while( c_factory_base * pf = *ppf++ )
					{
#if 1
						c_obj_ui* obj = nullptr;
						bdd_filename.replace_ext( pf->get_file_ext().get() );
						c_obj_ui::obj_get_load_from_existing_file( obj, pf, bdd_filename );
						if( obj )
							_bdds[pf] = (c_bdd*)obj;
#else
						load_bdd( pf, filename );
#endif
					}
					o_str::pop_name();
				}
				else
				{
					debug_break( "%s() We hit a case where we should not anyone.", __FUNCTION__ );
					//todo this a really a bad idea because we parse over 100 item
					for( auto const & pf : c_bdd::factory_group->get_factories() )
						load_bdd( pf, filename );
				}
				//for( auto const & pf : c_file_virtual::get_bdd_factories() )
				//	load_bdd( pf, filename );
				//c_factory_base
			}
			else
			{
				c_factory_base*	pf = c_bdd::factory_group->get_factory( _s_bdd_ui );
				load_bdd( pf, filename );
			}
		
		SPY_POP_RANGE2();

		bdd_switch( _s_bdd_ui );
		//we do it always r GaBuZoMeu could be in trouble in certain cases (_s_bdd_use_ui saved as current or none)
		//if( _s_bdd_use_ui >= BDD_USE_UPDATE_AND_DRAW && _s_bdd_ui != _s_bdd )
			bdd_set_by_index( _s_bdd_ui );	
		if( b_preload_data )	//hack like the rest of preload
			_bdd->load_data();

		if( _deformer_ui && _s_deformer_ui == 2
#if AAA_STATE_COMPILE()
				&& c_state_master::is_state_ref()
#endif //AAA_STATE_COMPILE
				)
		{
			_deformer_ui->remove_and_destroy_all();
		}
		try_obj_load_add_ext( _deformer_ui,		filename );
		try_obj_load_add_ext( _tex_anim,		filename );
		try_obj_load_add_ext( _multiple,		filename );
		try_obj_load_add_ext( _render,			filename );
//hack unclear probably a very old format pre 2008 where map and rendering where in the same object ?
		if( _render && _render->get_file_version()== 0 )
		{
			debug_break( "we should not pass here with > 2010 files, render version shopuld be >=1" );
			filename.add_ext( "ren" );
			try_obj_load_with_this_filename( _map, filename );
			filename.drop_ext();
			_b_mapping_ui = _b_rendering_ui;
		}
		else
		{
			try_obj_load_add_ext( _map, filename );
		}
		try_obj_load_add_ext( _texturing,		filename );
		try_obj_load_add_ext( _shading,			filename );
		try_obj_load_add_ext( _stencil,			filename );
		try_obj_load_add_ext( _model,			filename );
		try_obj_load_add_ext( _color,			filename );
		try_obj_load_add_ext( _lights_switch,	filename );
		try_obj_load_add_ext( _fog,				filename );
		try_obj_load_add_ext( _clip,			filename );
		try_obj_load_add_ext( _tex_video,		filename );

		//todo optimize witgh new fns sequence add.drop
		filename.add_ext( trs1_ext );
			try_obj_load_with_this_filename(	_transfo_trs1,	filename );
			if( _transfo_trs1 )
				c_namer::set_file_forced(		_transfo_trs1,	filename );
		filename.drop_ext();
		filename.add_ext( three_ext );
			try_obj_load_with_this_filename(	_transfo_three,	filename );
			if( _transfo_three )
				c_namer::set_file_forced(		_transfo_three,	filename );
		filename.drop_ext();
		filename.add_ext( trs2_ext );
			try_obj_load_with_this_filename(	_transfo_trs2,	filename );
			if( _transfo_trs2 )
				c_namer::set_file_forced(		_transfo_trs2,	filename );
		filename.drop_ext();

		// force existence of the model obj even if as saved with default value and so not loaded
		if( _b_model_ui )
			get_model();
		if( _b_color_ui )
			get_color();
		if( _b_rendering_ui )
			get_render();


		//todo not sure we need that anymore (Maa 2020)
		if( b_preload_data )	//hack
		{
			update_tex_bind();	//need it if we call get_bind_2d()
			tex_2d_bind( get_bind_2d() );
		}

	o_str::pop_name();
	return AAA_OK;
}

AAA_ERR	c_layer::load_from_existing_file( o_str CONST & filename )
{
	return	layer_load_from_existing_file( filename, true );
}

c_render*			c_layer::get_render()					{ return obj_get( _render );		}
c_multiple*			c_layer::get_multiple()					{ return obj_get( _multiple );		}
c_map*				c_layer::get_mapping()					{ return obj_get( _map );			}
c_texturing*		c_layer::get_texturing()				{ return obj_get( _texturing );		}
c_shading*			c_layer::get_shading()					{ return obj_get( _shading );		}
c_stencil*			c_layer::get_stencil()					{ return obj_get( _stencil );		}
c_tex_video*		c_layer::get_tex_video()				{ return obj_get( _tex_video );		}
c_model*			c_layer::get_model()					{ return obj_get( _model );			}
c_transfo_trs*		c_layer::get_transfo_trs1()				{ return obj_get( _transfo_trs1 );	}
c_transfo_trs*		c_layer::get_transfo_trs2()				{ return obj_get( _transfo_trs2 );	}
c_material*			c_layer::get_material_front()	CONST	{ return c_materials::get_ui() ? c_materials::get_ui()->get_from_index( _mat_front ) : nullptr ; }
c_material*			c_layer::get_material_back()	CONST	{ return c_materials::get_ui() ? c_materials::get_ui()->get_from_index( _mat_back ) : nullptr ; }
c_def_node*			c_layer::get_deformer()			CONST	{ return _deformer_ui;				}
c_color*			c_layer::get_color()					{ return obj_get( _color );			}
c_clip*				c_layer::get_clip()						{ return obj_get( _clip );			}


void	c_layer::set_id_local( INT32 CONST id_in )
{
static CHAR str_layer_name[] = "Layer_-";

	_id_local_ui = id_in;

	str_layer_name[6] = 'A' + _id_local_ui - 1;
	set_name( str_layer_name );

	make_param_header_name();
}

void	c_layer::become_ui()
{
	IF_THIS_NULL_RETURN();

	c_obj_ui*	obj = get_root();
	if( obj->is_class<c_layers>() )
	{
		((c_layers*)obj)->layer_ui_set( _id_local_ui - 1 );
	}
}

void	c_layer::info_str_make_bis( CHAR* CONST str ) CONST
{
	sprintf( str, "%s %s",
		is_my_filename() ? get_my_filename() : get_name_str(),
		(_s_bdd>=0)?c_bdd::factory_group->get_str_ui(_s_bdd):"none"
		);
}

void	c_layer::info_str_make( CHAR* CONST str ) CONST
{
	strcpy( str, "                " );
	CHAR*	pt = str;

	if( _s_bdd_use_ui != BDD_USE_NONE )
		*pt = (_s_bdd_use_ui==BDD_USE_CURRENT)?'c':((_s_bdd_use_ui==BDD_USE_UPDATE_AND_DRAW)?'D':((_s_bdd_use_ui==BDD_USE_UPDATE)?'u':'d'));
	++pt;
	if ( _b_model_ui )					{	*pt = 'M';	}
	++pt;
	if ( _s_deformer_ui != 1 )			{	*pt = (_s_deformer_ui==0)?'d':'D';	}
	++pt;
	if ( _s_multiple_ui != 1 )			{	*pt = (_s_multiple_ui==0)?'m':'M';	}
	*++pt = '-';
	++pt;
	if ( _b_rendering_ui )				{	*pt = 'R';	}
	++pt;
	if ( _b_mapping_ui )				{	*pt = 'M';	}
	++pt;
	if ( _s_texturing_ui != 1 )			{	*pt = (_s_texturing_ui==0) ? 't' : 'T' ;	}
	++pt;
	if ( _s_shading_ui != 1 )			{	*pt = (_s_tex_anim_ui==0) ? 's' : 'S' ;		}
	++pt;
	if ( _b_color_ui )					{	*pt = 'C';	}
	++pt;
	if ( _s_tex_anim_ui != 1)			{	*pt = (_s_tex_anim_ui==0) ? 'a' : 'A' ;	}
	++pt;
	if ( _s_transfo1_ui != 1)			{	*pt = (_s_transfo1_ui==0) ? 't' : 'T' ;	}
	++pt;
	if ( _s_transfo_three_ui != 1 )		{	*pt = (_s_transfo_three_ui==0) ? 't' : 'T' ;	}
	++pt;
	if ( _s_transfo2_ui != 1 )			{	*pt = (_s_transfo2_ui==0) ? 't' : 'T' ;	}
	++pt;
	if ( _s_fog_ui != 1 )				{	*pt = (_s_fog_ui==0) ? 'f' : 'F' ;	}
	++pt;
	if ( _b_use_list )					{	*pt = 'O' ;	}
}


static	FINLINE	bool	loc_test( CONST c_factory_base* CONST factory, c_obj_ui* obj )
{
	if( obj && factory == obj->get_factory() )
	{
		obj->set_focus();
		return true;
	}
	return false;
}

//todofocus exact process is unclear and should be commented
//todofocus set_focus should be virtual and call this and others when needed without a self calling call
void	c_layer::update_obj_uis()
{
	c_obj_ui*	oui = focus_param::get_flatland_obj_focus();
	if( oui )
	{
		//todo check we do all the ui (subscribe or ui_list)
		CONST c_factory_base* CONST pfactory	= oui->get_factory();
		C_PCHAR_C pstr							= oui->get_class_name();

//maanow
//	the focus thing should a property of the class ?
		//todocam
		//if ( loc_test( pfactory, c_seedcam::get_ui() ) );
		//else
		if( loc_test( pfactory, c_layer::get_ui() ) )
		{
		}
		else if( str_is_equal( pstr, "bdd", 3 ) )
		{
#if	!AAA_WATCHDOG()
			if( c_bdd::get_ui() && focus_param::get_flatland_obj_focus() != c_bdd_mocap::ui )
#else
			if( c_bdd::get_ui() )
#endif	//AAA_WATCHDOG
			{
				c_bdd::get_ui()->set_focus();
			}
		}
		else if( str_is_equal( pstr, "def", 3 ) )
		{
			c_def_node::get_ui()->set_focus();
		}
		else if( loc_test( pfactory, c_render::get_ui() ) );
		else if( loc_test( pfactory, c_multiple::ui ) );
		else if( loc_test( pfactory, c_map::get_ui() ) );
		else if( loc_test( pfactory, c_texturing::get_ui() ) );
		else if( loc_test( pfactory, c_shading::get_ui() ) );
		else if( loc_test( pfactory, c_lights_switch::ui ) );
		else if( loc_test( pfactory, c_stencil::ui ) );
		else if( loc_test( pfactory, c_tex_anim::ui ) );
		else if( loc_test( pfactory, c_model::ui ) );
		else if( loc_test( pfactory, c_color::ui ) );
		else if( loc_test( pfactory, c_stencil::ui ) );
		else if( loc_test( pfactory, c_tex_video::ui ) );
		else if( loc_test( pfactory, c_fog::ui ) );
		else if( loc_test( pfactory, c_tex_anim::ui ) )
		{
		}
	}
}

//todonow
void	obj_focus_update()
{
	//todo	this not clean and should be solve with the previous
	if( c_layer::get_ui() )
	{
		c_layer::get_ui()->bdd_menu_update();
	}
}

