#include "infrastructure/layer/layer.h"
#include "layer_att.h"
#include "draw/color.h"
#include "draw/fog.h"
#include "draw/lights.h"
#include "draw/model.h"
#include "draw/render.h"
#include "obj_ui/bdd/bdd_point/bdd_mocap.h"
#include "obj_ui/deformer/def_node.h"
#include "draw/map.h"
#include "draw/texture.h"
#include "shaders/shading.h"
#include "draw/stencil.h"
#include "draw/clip.h"
#include "media/video/tex_video.h"
#include "draw/tex_anim.h"
#include "obj_ui/transfo/transfo_three.h"
#include "obj_ui/transfo/transfo_trs.h"
#include "obj_ui/bdd/bdd_edit/bdd_curve_edit.h"


namespace
{
	CONSTEXPR	INT32	STACK_SIZE = 12;
}

//	stack_index points at the current top of the layer-attribute stack.
//	stack_index == -1 means empty, valid slots are stack[0..STACK_SIZE-1].
//	push pre-increments before writing, so first push lands on stack[0]. See push / pop below.
INT32			c_layer_att::stack_index	= -1;
c_layer_att*	c_layer_att::stack			= nullptr;
c_bdd*			c_layer_att::bdd_up			= nullptr;

void	c_layer_att::c_init()
{
	SAFE_NEW_ARRAY( stack, STACK_SIZE );
}

void	c_layer_att::c_deinit()
{
	SAFE_DELETE_ARRAY( stack );
}

void	c_layer_att::get_cur()
{
	_bdd			= c_bdd::get_cur();
#if	!AAA_WATCHDOG()
	_bdd_mocap		= c_bdd_mocap::cur;
	_bdd_multiple	= c_bdd_multiple::cur;
	_bdd_curve_edit	= c_bdd_curve_edit::cur;
#endif	//AAA_WATCHDOG
	_color			= c_color::get_cur();
	_deformer		= c_def_node::get_cur();
	_fog			= c_fog::cur;
	_layer			= c_layer::get_cur();
	_lights_switch	= c_lights_switch::cur;
	_map			= c_map::get_cur();
	_texturing		= c_texturing::get_cur();
	_shading		= c_shading::get_cur();
	_stencil		= c_stencil::cur;
	_clip			= c_clip::cur; //g_clip_cur;
	_tex_video		= c_tex_video::cur;
	_model			= c_model::cur;
	_render			= c_render::get_cur();
//	multiple_		= c_multiple::cur;
	_tex_anim		= c_tex_anim::cur;
	_transfo_three	= c_transfo_three::cur;
	_transfo1		= c_transfo_trs::transfo1_cur;
	_transfo2		= c_transfo_trs::transfo2_cur;
}

void	c_layer_att::set_cur() CONST
{
	c_bdd			::set_cur(			_bdd			);
#if	!AAA_WATCHDOG()
	c_bdd_mocap		::cur			=	_bdd_mocap;
	c_bdd_multiple	::cur			=	_bdd_multiple;
	c_bdd_curve_edit::cur			=	_bdd_curve_edit;
#endif	//AAA_WATCHDOG
	c_color			::set_cur(			_color			);
	c_def_node		::set_cur(			_deformer		);
	c_fog			::cur			=	_fog;
	c_layer			::set_cur(			_layer			);
	c_lights_switch	::cur			=	_lights_switch;
	c_map			::set_cur(			_map			);
	c_texturing		::set_cur(			_texturing		);
	c_shading		::set_cur(			_shading		);
	c_stencil		::cur			=	_stencil;
	c_clip			::cur			=	_clip;
	c_tex_video		::cur			=	_tex_video;
	c_model			::cur			=	_model;
	c_render		::set_cur(			_render			);
//	c_multiple		::cur			=	multiple_;
	c_tex_anim		::cur			=	_tex_anim;
	c_transfo_three	::cur			=	_transfo_three;
	c_transfo_trs	::transfo1_cur	=	_transfo1;
	c_transfo_trs	::transfo2_cur	=	_transfo2;
}

//	push : always increment stack_index. Save the current state into the slot only if the
//	slot is in range. When the stack overflows we drop the save but keep the counter in sync
//	with the push call ; the matching pop will skip the load and just decrement, so the
//	push / pop pair stays balanced through the overflow region.
void	c_layer_att::push()
{
	++stack_index;
	if( stack_index < STACK_SIZE )
	{
		bdd_up = c_bdd::get_cur();
		stack[stack_index].get_cur();
	}
	else
		ERR_PRINT_STRING( "%s() stack overflow at %d slots, too many nested multiple-with-layer", __FUNCTION__, STACK_SIZE );
}

void	c_layer_att::pop()
{
	if( stack_index < 0 )
	{
#if AAA_DEBUG()
		debug_break( "%s() pop on empty stack, missing push upstream", __FUNCTION__ );
#endif
		return;
	}
	if( stack_index < STACK_SIZE )
		stack[stack_index].set_cur();
	//	else : stack[stack_index] was never written (push overflowed), skip the load.
	--stack_index;
}