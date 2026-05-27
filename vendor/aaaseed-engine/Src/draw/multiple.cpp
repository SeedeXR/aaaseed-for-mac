#include "draw/multiple.h"
#include "draw/axe.h"
#include "draw/rect.h"
#include "draw/geo/sphere.h"
#include "infrastructure/layer/module.h"
#include "infrastructure/layer/modules.h"
#include "infrastructure/layer/layer_att.h"
#include "infrastructure/layer/layers_att.h"
#include "math/billboard.h"
#include "math/rand.h"
#include "ui/alphabet.h"
#include "image/bind_img_2d.h"
#include "draw/shape.h"
#include "draw/box.h"
#include "draw/color.h"

#include "language/lua/aaalua_wrap.h"


FACTORY_CREATE_V1( c_multiple, multiple, Multiple, multiple );


namespace	n_multiple
{
	static	C_PCHAR_C	str_align[c_multiple::ALIGN_MAX_NB] =
	{
		"World",
		"Normal",
		"Normal_using_z",
		"Normal_with_offset",
		"Billboard",
		"Random",
		"Random_Axe_X",
		"Random_Axe_Y",
		"Random_Axe_Z",
		"Gradient"
	};

	static C_PCHAR_C	str_type[c_multiple::TYPE_MAX_NB] =
	{
		"NUMBER",
		"FACE",
		"FACE_CROSS",
		"FACE_THREE",
		"CUBE",
		"BOX",
		"SPHERE",
		"ELLIPSE",
		"CONE",
		"AXE",
		"LAYERS_PREV",
		"LAYERS_NAME",
		"MODULE_NAME",
		"LAYERS",
		"LAYER",
		"LUA",
	//	"BDD_TRI",
	//	"BDD_PREV",
	};

	static	C_PCHAR_C	str_render[c_multiple::RENDER_MAX_NB] =
	{
		"Regular",
		"Multiple",
		"Both",
	};

	static	C_PCHAR_C	str_tra[c_multiple::TRA_MAX_NB] =
	{
		"Implicit",
		"No",
	};

	CONSTEXPR INT32	BASE_PARAM_NB	= 18;
	CONSTEXPR INT32	IMG_PARAM_NB	= 15;
	CONSTEXPR INT32	OUT_PARAM_NB	= 8;
	CONSTEXPR INT32 TEK_PARAM_NB	= 1;
	CONSTEXPR INT32	GROUP_PARAM_NB	= 4;
	CONSTEXPR INT32	PARAM_NB_MAX	= BASE_PARAM_NB
									+ IMG_PARAM_NB
									+ OUT_PARAM_NB
									+ TEK_PARAM_NB
									+ GROUP_PARAM_NB
									+ c_multiple::PARAMETER_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_SYMBO_MIN_MAX(	render,		c_multiple::RENDER_MULTIPLE,	c_multiple::RENDER_REGULAR,	c_multiple::RENDER_REGULAR,	c_multiple::RENDER_MAX_NB-1,	str_render )
		PARAM_DEF_SYMBO_MIN_MAX(	type,		c_multiple::TYPE_AXE,			c_multiple::TYPE_FACE,		c_multiple::TYPE_NUMBER,	c_multiple::TYPE_MAX_NB-2,		str_type )
		PARAM_DEF_SYMBO_PSTR_ZERO(	alignment,	str_align )
		PARAM_DEF_AXE_Y(			axe			)
		PARAM_DEF_FP32_POS_ONE(		resolution	)
		PARAM_DEF_SYMBO_PSTR_ZERO(	Translation, str_tra )
	
		PARAM_DEF_SCALE_UVAF(		size		)
		PARAM_DEF_REAL_POS_ZERO(	size_jitter	)	
		PARAM_DEF_REAL_POS_ONE(		size_jitter_exponent	)	
		PARAM_DEF_POINT_UVA(		offset		)

		PARAM_DEF_REF(				target_name	)
		PARAM_DEF_INT32(			start,		2, 1,	0, c_module::LAYERS_NB-1. )
		PARAM_DEF_INT32(			stop,		2, 1,	0, c_module::LAYERS_NB-1. )

		PARAM_DEF_GROUP_CLOSED( Drawing by Number, IMG_PARAM_NB )
			PARAM_DEF_BOOL_OFF(			color_from_xy				)
			PARAM_DEF_INT32_POS_ZERO(	image_color_bind			)
			PARAM_DEF_BOOL_OFF(			image_linear				)
			PARAM_DEF_SCALE_FP32_UV(	image_scale					)
			PARAM_DEF_POINT_FP32_UV(	image_offset				)
			PARAM_DEF_BOOL_OFF(			image_clamped				)
			PARAM_DEF_INT32_POS_ZERO(	image_gradient_bind			)
			PARAM_DEF_REAL_POS_ZERO(	image_gradient_min			)
			PARAM_DEF_REAL_POS_ONE(		image_gradient_max			)
			PARAM_DEF_BOOL_OFF(			image_gradient_normalize	)
			PARAM_DEF_REAL_ZERO(		image_gradient_size_min		)
			PARAM_DEF_REAL_ONE(			image_gradient_size_max		)
			PARAM_DEF_BOOL_OFF(			image_gradient_size_inverse	)

		PARAM_DEF_GROUP_CLOSED( Out, OUT_PARAM_NB )
			PARAM_DEF_INT32_LOCKED(		index	)
			PARAM_DEF_INT32_LOCKED(		index_u	)
			PARAM_DEF_INT32_LOCKED(		index_v	)
			PARAM_DEF_INT32_LOCKED(		index_w	)
			PARAM_DEF_INT32_LOCKED(		nb		)
			PARAM_DEF_INT32_LOCKED(		nb_u	)
			PARAM_DEF_INT32_LOCKED(		nb_v	)
			PARAM_DEF_INT32_LOCKED(		nb_w	)

		PARAM_DEF_GROUP_CLOSED( parameter, c_multiple::PARAMETER_NB )
			PARAM_DEF_16(  parameter, PARAM_DEF_REAL_ZERO )

		PARAM_DEF_GROUP_CLOSED( TEK, TEK_PARAM_NB )
			PARAM_DEF_BOOL_OFF(			use_matrix	)
	};
}


c_multiple*	c_multiple::def	= nullptr;
c_multiple*	c_multiple::cur	= nullptr;
c_multiple*	c_multiple::ui	= nullptr;

bool		c_multiple::b_use_matrix_ui;


void	c_multiple::build_sum_up( o_str& o )
{
	o.erase();
	switch( _s_render_ui  )
	{
	case RENDER_REGULAR:	return;									break;
	case RENDER_BOTH:		o.add( "B " );
	case RENDER_MULTIPLE:	o.add( n_multiple::str_type[_s_type] );	break;	//todo it could display lua type which can be set directly in the UI
	default:	break;
	}
	switch( _s_type )
	{
	case TYPE_LAYERS_NAME:
	case TYPE_MODULE_NAME:
		o.add_char( '(' );
		o.add( _target_name );
		o.add_char( ')' );
	default:
		break;
	}
}

void	c_multiple::param_init_pt()
{
	INT32	h=0;

	param_set_pt(	h, _s_render_ui				);
	param_set_pt(	h, _s_type_ui				);
	param_set_pt(	h, _s_align_ui				);
	param_set_pt(	h, _s_axe_ui				);
	param_set_pt(	h, _resolution_ui			);
	param_set_pt(	h, _s_translation_ui		);
	param_set_pt_4(	h, _size_ui					);
	param_set_pt(	h, _size_jitter_ui			);
	param_set_pt(	h, _size_jitter_exponent_ui	);
	param_set_pt_3(	h, _offset					);
	param_set_pt(	h, _target_name				);
	param_set_pt(	h, _start_ui				);
	param_set_pt(	h, _stop_ui					);

	++h;
		param_set_pt(	h, _b_color_from_xz_ui		);
		param_set_pt(	h, _color_img_bind			);
		param_set_pt(	h, _b_linear_ui				);
		param_set_pt_2(	h, _uv_scale_ui				);
		param_set_pt_2(	h, _uv_offset_ui			);
		param_set_pt(	h, _b_clamped_ui			);
		param_set_pt(	h, _grad_img_bind			);
		param_set_pt(	h, _grad_min				);
		param_set_pt(	h, _grad_max				);
		param_set_pt(	h, _b_grad_normalize_ui		);
		param_set_pt(	h, _grad_size_min_ui		);
		param_set_pt(	h, _grad_size_max_ui		);
		param_set_pt(	h, _b_grad_size_inverse_ui	);

	++h;
		param_set_pt(	h, _index					);
		param_set_pt_3(	h, _index_by_dim			);
		param_set_pt(	h, _nb						);
		param_set_pt_3(	h, _nb_by_dim				);

	++h;
		param_set_pt_n(	h, _parameter, 16 );

	++h;
		param_set_pt(	h, b_use_matrix_ui			);

	err_param_init_pt(h);
}


CONSTRUCTOR_CREATE(c_multiple),
	_pfn_align_then_draw		{nullptr},
	_pfn_align_nor_then_draw	{nullptr},
	_index						{0},
	_nb							{1},
	_s_type						{TYPE_FACE},
	_img_color					{nullptr},
	_img_grad					{nullptr},
	_b_align_normal				{false},
	_p_layer_cached				{nullptr},
	_p_layers_cached			{nullptr},
	_p_module_cached			{nullptr},
	_size_jitter				{0.},
	_int_resolution_by_6		{6},
	_int_resolution_by_12		{12}
{
	clear_v3( _index_by_dim );
	clear_v3( _nb_by_dim );

	param_init_with( n_multiple::param, n_multiple::PARAM_NB_MAX ); // multiple_param, MULTIPLE_PARAM_NB_MAX);
}

c_multiple::~c_multiple()
{
	if( cur == this )
		cur = nullptr;
	if( ui == this )
		ui	= nullptr;
}

void	c_multiple::update_low()
{
	_int_resolution_by_6  = I_CEIL( _resolution_ui * 6 );
	_int_resolution_by_12 = I_CEIL( _resolution_ui * 12 );

	axe_build_index_vert( _i_u, _i_v, _s_axe_ui );
	if( _b_color_from_xz_ui )
	{
		_img_color = g_bind_img_2d->get_ready( _color_img_bind );
		if( _img_color )
		{
			_uv_factor[0] = OVER_ONE_AS_REAL(_uv_scale_ui[0] );
			_uv_factor[1] = OVER_ONE_AS_REAL(_uv_scale_ui[1] );
		}
	}
	else
		_img_color = nullptr;

	if( _s_align_ui == ALIGN_GRADIENT )
	{
		_img_grad = g_bind_img_2d->get_ready( _grad_img_bind );
		if( _img_grad )
		{
			REAL zero, one;
			if( _b_grad_size_inverse_ui )
			{
				zero = _grad_size_min_ui;
				one = _grad_size_max_ui;
			}
			else
			{ 
				zero = _grad_size_max_ui;
				one = _grad_size_min_ui;
			}						
			_grad_size_offset = zero;
			_grad_size_factor = one - zero;

			_grad_min_squared = _grad_min * _grad_min;
			_grad_max_squared = _grad_max * _grad_max;

			_uv_factor[0] = OVER_ONE_AS_REAL( _uv_scale_ui[0] );
			_uv_factor[1] = OVER_ONE_AS_REAL( _uv_scale_ui[1] );
		}
		scale_v3( _size, _size_ui, _size_ui[3] );
		_b_align_normal = false;
	}
	else
	{
		_img_grad = nullptr;
		if( _s_align_ui == ALIGN_WORLD )
		{
			_size[_i_u]			= _size_ui[0] * _size_ui[3];
			_size[_i_v]			= _size_ui[1] * _size_ui[3];
			_size[_s_axe_ui]	= _size_ui[2] * _size_ui[3];
			_b_align_normal = false;
		}
		else
		{
			scale_v3( _size, _size_ui, _size_ui[3] );
			_b_align_normal = INSIDE_MIN_MAX( _s_align_ui, ALIGN_NORMAL, ALIGN_NORMAL_OFFSET );
		}
	}
	_size_jitter = _size_jitter_ui;

	if( _start_ui <= _stop_ui )
	{
		_start = _start_ui;
		_stop = _stop_ui;
	}
	else
	{
		_start = _stop_ui;
		_stop = _start_ui;
	}
}

bool		c_multiple::can_implicit()
{
	switch( get_type() )
	{
	case TYPE_NUMBER:
	case TYPE_AXE:
		return	false;
		break;
/*
	case TYPE_TRI:
		{
		c_bdd_tri* tri = c_layer::get_cur()->get_bdd_tri();
		if(tri)
			return	tri->can_implicit();
		else
			return	true;
		}
		break;
	case TYPE_PREV:
		{
		c_bdd* p_bdd = c_layer::get_cur()->get_bdd_prev();
		if(p_bdd)
			return	p_bdd->can_implicit();
		else
			return	true;
		}
		break;
*/
/*
//todoqqq should deal with these non obvious cases ?
		TYPE_LAYERS_PREV,
		TYPE_LAYERS_NAME,
		TYPE_MODULE_NAME,
		TYPE_LAYERS,
		TYPE_LAYER,
		TYPE_LUA
*/
	default:
		return	true;
		break;
	}
}

bool	c_multiple::_b_allow_list = false;

bool	c_multiple::can_use_list()
{
	if( _b_allow_list )
	{
		switch( get_type() )
		{
		case TYPE_FACE:	//list slower on geforce4 Go
		case TYPE_NUMBER:
			return false;
		case TYPE_FACE_CROSS:
		case TYPE_FACE_THREE:
		case TYPE_CUBE:
		case TYPE_BOX:
		case TYPE_SPHERE:
		case TYPE_CIRCLE:
		case TYPE_CONE:
		case TYPE_AXE:
//			case TYPE_TRI:
//			case TYPE_PREV:
			return true;
		case TYPE_LAYERS:
		case TYPE_LAYERS_PREV:
		case TYPE_LAYERS_NAME:
		case TYPE_MODULE_NAME:
		case TYPE_LAYER:
		case TYPE_LUA:
		default:
			return false;
		}
	}
	else
		return false;
}

void		c_multiple::set_color_low_r( REAL CONST * pos )
{
	FP32	col[4];
	_img_color->get_color4r_from_uv_nearest( col,	pos[0]*_uv_factor[0]+_uv_offset_ui[0],
													pos[1]*_uv_factor[1]+_uv_offset_ui[1],
													_b_clamped_ui ); //_b_clamped, (c_img_2d::COMPO)_s_compo );
	mul_v4( col, c_color::get_cur()->get_color_pt() );
	GOL::color4v( col );
}

static	c_rand_lin	rand_rot;
//hack bad because of random generator
FINLINE	void	c_multiple::do_random_rotate()
{
	INT32	index = get_index();
	GOL::matrix::rotate_xyz(
						rand_rot.get_float_with_seed( index ),
						rand_rot.get_float_with_seed( index * 1654564 ),
						rand_rot.get_float_with_seed( index * 26655458 )
					);
}

FINLINE	void	c_multiple::do_random_rotate_axe( INT32 axe )
{
	INT32	index = get_index();
	REAL	angle = rand_rot.get_float_with_seed( index );
	switch( axe )
	{
	case 0:	GOL::matrix::rotate_x( angle );	break;
	case 1:	GOL::matrix::rotate_y( angle );	break;
	case 2:	GOL::matrix::rotate_z( angle );	break;
	}
}

namespace {
CONSTEXPR CHAR hook_name[] = "aaa.bdd.hook_draw_multiple";
}


FINLINE void	c_multiple::basic_draw( TYPE CONST what )
{
	switch( what )
	{
	//todo optimize these 8 canonical shapes with lists or vbo at least
	case TYPE_FACE_THREE:	draw_rect_axe(					1.,1.,	1	);
	case TYPE_FACE_CROSS:	draw_rect_axe(					1.,1,	0	);
	case TYPE_FACE:			draw_rect_axe(					1.,1,	2	);	break;
	case TYPE_CUBE:			
	case TYPE_BOX:			draw_cube_cano();				break;
	case TYPE_CIRCLE:		draw_ellipse(		nullptr,	1.,1., 2,	GL_LINE_LOOP,	_int_resolution_by_12 	);	break;
	case TYPE_CONE:			draw_cone(						one_v4fp32,	_int_resolution_by_12	);	break;
	case TYPE_SPHERE:		draw_sphere(					1.,			_int_resolution_by_12, _int_resolution_by_6	);	break;

	case TYPE_AXE:			n_axe::draw_origin();	break;
	case TYPE_NUMBER:
		{
			INT32	number = _index;
			if( number < 10 )
				GOL::matrix::translate( -.5	, -.5, 0 );
			else if( number < 100 )
				GOL::matrix::translate( -1.	, -.5, 0 );
			else if( number < 1000 )
				GOL::matrix::translate( -1.5, -.5, 0 );
			else
				GOL::matrix::translate( -2.	, -.5, 0 );
			aaa::alphabet::draw_int32( number, 0 );
		}
		break;
	case TYPE_LAYER:
		if( _p_layer_cached )
		{
			c_multiple* mul = c_multiple::cur;
			_p_layer_cached->update_then_draw();
			c_multiple::cur = mul;
		}
		break;
	case TYPE_LAYERS:
	case TYPE_LAYERS_NAME:	
	case TYPE_LAYERS_PREV:	//hackhack to be stabilize and secure
		if( _p_layers_cached )
		{
			c_multiple* mul = c_multiple::cur;
			_p_layers_cached->draw_by_multiple();
			c_multiple::cur = mul;
		}
		g_lua_wrap_master->do_fn_pass_i( nullptr, "aaa.bdd.hook_draw_multiple", get_index() );
		break;
	case TYPE_MODULE_NAME:	
		if( _p_module_cached )
		{
			c_multiple* mul = c_multiple::cur;
			_p_module_cached->draw_by_multiple();
			c_multiple::cur = mul;
		}
		break;
	case TYPE_LUA:
		g_lua_wrap_master->do_fn_pass_i( nullptr, hook_name, get_index() );
		break;
/*	case TYPE_TRI:
		{
			c_bdd_tri* tri = c_layer::get_cur()->get_bdd_tri();
			if( tri && tri != this )
			{
				tri->update();
				tri->draw_before();
				tri->draw();
			}
		}
		break;

	case TYPE_PREV:
		{
			c_bdd* bdd_prev = get_bdd_prev();
			if( bdd_prev == c_bdd::cur )
			{
				//todo detect it better and sooner (should be in the interface
				c_layer::get_cur()->err_print( "Can't use CURRENT with a particle system using previous");
			}
			else if( bdd_prev )
			{
				bdd_prev->draw_before();
				bdd_prev->draw();
			}
		}
		break;
*/
	default:
		ERR_PRINT_STRING( "please Maa debug multiple draw" );
		break;
	}
}

namespace {
	c_rand_lin	rand_size;
	REAL size_tmp[3];
}

FINLINE void	c_multiple::draw_one_low( REAL CONST * size, INT32 CONST axe )
{
	if( _size_jitter!=0. )
	{
		REAL f = POW_R( rand_size.get_ufloat_with_seed( get_index() ), _size_jitter_exponent_ui );		
		scale_v3( size_tmp, size, 1. + _size_jitter * (f-.5) );
		size = size_tmp;
	}

	switch( _s_type )
	{
	case TYPE_FACE_THREE:	draw_rect_axe(		size[2],size[0],	IMOD( axe+1, 3 )	);
	case TYPE_FACE_CROSS:	draw_rect_axe(		size[1],size[2],	IMOD( axe+2, 3 )	);
	case TYPE_FACE:			draw_rect_axe(		size[0],size[1],	axe					);	break;
	case TYPE_CUBE:			draw_cube(			size[2]									);	break;
	case TYPE_BOX:			draw_box(			size									);	break;
	case TYPE_CIRCLE:		draw_ellipse(		nullptr, size[0], size[1], axe, GL_LINE_LOOP,	_int_resolution_by_12 	);	break;
	case TYPE_CONE:			draw_cone(			size,											_int_resolution_by_12	);	break;
	default:
		if( !_b_push_done)
			GOL::matrix::push();

		{
			GOL::matrix::rotate_align_on_z( axe );
			GOL::matrix::scale3v( size );

			basic_draw( _s_type );
		}

		if( !_b_push_done )
			GOL::matrix::pop();
		break;
	}
}

FINLINE void	c_multiple::draw_one_at_low( REAL CONST * pos, REAL CONST * size, INT32 axe )
{
	if( _size_jitter!=0. )
	{
		REAL f = POW_R( rand_size.get_ufloat_with_seed( get_index() ), _size_jitter_exponent_ui );		
		scale_v3( size_tmp, size, 1. + _size_jitter * (f-.5) );
		size = size_tmp;
	}

	switch( _s_type )
	{
	case TYPE_FACE_THREE:	draw_rect_at(	size[2], size[0],	pos, IMOD( axe+1, 3 )	);
	case TYPE_FACE_CROSS:	draw_rect_at(	size[1], size[2],	pos, IMOD( axe+2, 3 )	);
	case TYPE_FACE:			draw_rect_at(	size[0], size[1],	pos, axe				);							break;
	case TYPE_CUBE:			draw_cube_at(	size[2],			pos						);							break;
	case TYPE_BOX:			draw_box_at(	size,				pos						);							break;
	case TYPE_CIRCLE:		draw_ellipse(	pos, size[0], size[1], axe, GL_LINE_LOOP,	_int_resolution_by_12 	);	break;
	case TYPE_CONE:			draw_cone_at(	size, pos,									_int_resolution_by_12	);	break;
	default:
		//todo
		//hack	sangblang
		if( is_tra_implicit() )
		{
			if( !_b_push_done )
				GOL::matrix::push();

				GOL::matrix::translate3v( pos );
				GOL::matrix::rotate_align_on_z( axe );
				GOL::matrix::scale3v( size );

				basic_draw( _s_type );

			if( !_b_push_done)
				GOL::matrix::pop();
		}
		else
		{
			basic_draw( _s_type );
		}
		break;
	}
}

void	c_multiple::draw_one_at_tgn_low( REAL CONST * pos, REAL CONST * size, REAL CONST * u, REAL CONST * v, REAL CONST * nor )
{
	if( _size_jitter!=0. )
	{
		REAL f = POW_R( rand_size.get_ufloat_with_seed( get_index() ), _size_jitter_exponent_ui );		
		scale_v3( size_tmp, size, 1. + _size_jitter * (f-.5) );
		size = size_tmp;
	}

	if( !b_use_matrix_ui )
	{
		bool b_done = true;
		switch( _s_type )
		{
		case TYPE_FACE_THREE:	draw_rect_at_tgn(	size[2], size[0],	pos, nor, u, v	);
		case TYPE_FACE_CROSS:	{	// made to be the same than the matrix method
									REAL nnor[3];
									REAL nu[3];
									neg_v3( nnor, nor );
									neg_v3( nu, u );
									draw_rect_at_tgn(	size[2], size[1],	pos, nnor, v, nu	);
								}
		case TYPE_FACE:			draw_rect_at_tgn(	size[0], size[1],	pos, u, v, nor	);	break;
		case TYPE_CUBE:			draw_cube_at_tgn(	size[2],			pos, u, v, nor	);	break;
		case TYPE_BOX:			draw_box_at_tgn(	size,				pos, u, v, nor	);	break;
		case TYPE_CONE:			draw_cone_at_tgn(	size,				pos, u, v, nor,	_int_resolution_by_12	);	break;
		default:				b_done = false;
		}
		if( b_done )
			return;
	}
	if( !_b_push_done )
		GOL::matrix::push();

		GOL::matrix::build_matrix_pos_size_dir_then_mul( pos, size, u, v, nor );
		set_size_draw( size );	//have sense only when traxs could be used
		basic_draw( _s_type );

	if( !_b_push_done )
		GOL::matrix::pop();
}

//
//	ALIGN
//

FINLINE	void	c_multiple::draw_one_at_tgn_offset_private(	REAL CONST * pos,	REAL CONST * size,	REAL CONST * u,	REAL CONST * v,	REAL CONST * nor )
{
	set_color_r( pos );
#if 1
	REAL p[3];
#else
	//befor 2025 April was done this way and so was changing the pos_draw table
	REAL p = get_pos_draw();
#endif
	//	was
	/*
	switch( get_axe() )
	{
	case 0:	add_scale_v3( p, pos, v,	_offset_axe );	break;
	case 1:	add_scale_v3( p, pos, u,	_offset_axe );	break;
	case 2:	add_scale_v3( p, pos, nor,	_offset_axe );	break;
	}
	*/

	//hack
	//todo the implicit / explicit mechanism have to be decided and then symetrized 
	if( is_tra_implicit() )
	{
		//	now
		//	we should have a full transfo there
		INT32	i_u, i_v, i_axe;
		get_axes_index( i_u, i_v, i_axe );

		add_scale_v3( p, pos, nor,	_offset[i_axe]			);
		add_scale_v3( p, u,			_offset[i_u]*size[i_u]	);
		add_scale_v3( p, v,			_offset[i_v]*size[i_v]	);
	}
	else
		clear_v3(p);
	
	switch( get_axe() )
	{
	case 0:	draw_one_at_tgn_low( p, size, nor, u, v );	break;
	case 1:	draw_one_at_tgn_low( p, size, v, nor, u );	break;
	case 2:	draw_one_at_tgn_low( p, size, u, v, nor );	break;
	}
}

FINLINE	void	c_multiple::draw_one_at_tgn_private(		REAL CONST * pos,	REAL CONST * size,	REAL CONST * u,	REAL CONST * v,	REAL CONST * nor )
{
	set_color_r( pos );
	set_pos_draw( pos );

	if( !is_tra_implicit() )
		pos = (REAL*)zero_v4fp32;

	switch( get_axe() )
	{
	case 0:	draw_one_at_tgn_low( pos, size, nor, u, v );	break;
	case 1:	draw_one_at_tgn_low( pos, size, v, nor, u );	break;
	case 2:	draw_one_at_tgn_low( pos, size, u, v, nor );	break;
	}
}

void	c_multiple::align_world_then_draw( REAL CONST * CONST pos, REAL CONST * CONST size, INT32 CONST axe )
{
	draw_one_at_low( pos, size, axe );
}

void	c_multiple::align_normal_then_draw( REAL CONST * CONST pos, REAL CONST * CONST size, INT32 CONST axe )
{
	//todo deal with this case in bdd_part and better in other
	draw_one_at_low( pos, size, axe );
}

void	c_multiple::align_normal_using_z_then_draw( REAL CONST * CONST pos, REAL CONST * CONST size, INT32 CONST axe )
{
	draw_one_at_low( pos, size, axe );
}

void	c_multiple::align_normal_offset_then_draw( REAL CONST * CONST pos, REAL CONST * CONST size, INT32 CONST axe )
{
	REAL	p[3];
	cpy_v3( p, pos );
	p[_i_u]			+= _offset[0];
	p[_i_v]			+= _offset[1];
	p[_s_axe_ui]	+= _offset[2];
	//todo deal with this case in bdd_part and better in other
	draw_one_at_low( p, size, axe );
}

//todo pass a c_img_2d::COMPO and then propagate to the calling paths
void	c_multiple::align_gradient_then_draw( REAL CONST * CONST pos, REAL CONST * CONST size, INT32 CONST axe )
{
	if( _img_grad )
	{
		REAL	grad[3];
		//clear_v3r( grad );
		_img_grad->get_gradient_from_uv( grad,	pos[0]*_uv_factor[0]+_uv_offset_ui[0],
												pos[1]*_uv_factor[1]+_uv_offset_ui[1],
												_b_clamped_ui, aaa::COMPO::GREY, _b_linear_ui );
		REAL n = norm_squared_v2r( grad );
		if( OUTSIDE( n, _grad_min_squared, _grad_max_squared ) )
			return;

		n = SQRT(n);
		if( _b_grad_normalize_ui )
			scale_v2( grad, 1./n );
		else
		{
			REAL tmp = (n-_grad_min) / (_grad_max-_grad_min);
			scale_v2( grad, (_grad_size_offset + _grad_size_factor*tmp)/n );
		}
		grad[2] = 0;
		REAL	u[3];
		cross_z_v3r( u, grad );
		draw_one_at_tgn_private( pos, size, grad, u, unit_z_v4fp32 );
//		grad[2] = grad[1];
//		grad[1];
//		draw_one_at_nor( pos, size, grad );
	}
	else
		//todo deal with this case in bdd_part and better in other
		draw_one_at_low( pos, size, axe );
}


void	c_multiple::align_billboard_then_draw( REAL CONST * CONST pos, REAL CONST * CONST size, INT32 CONST axe )
{
	GOL::matrix::push();
		_b_push_done = true;
		GOL::matrix::translate3v( pos );
		billboard::do_x( pos );
		draw_one_low( size, axe );
	GOL::matrix::pop();
}

void	c_multiple::align_random_then_draw( REAL CONST * CONST pos, REAL CONST * CONST size, INT32 CONST axe )
{
	GOL::matrix::push();
		_b_push_done = true;
		GOL::matrix::translate3v( pos );
		do_random_rotate();
		draw_one_low( size, axe );
	GOL::matrix::pop();
}

void	c_multiple::align_random_axe_x_then_draw( REAL CONST * CONST pos, REAL CONST * CONST size, INT32 CONST axe )
{
	GOL::matrix::push();
		_b_push_done = true;
		GOL::matrix::translate3v( pos );
		do_random_rotate_axe( 0 );
		draw_one_low( size, axe );
	GOL::matrix::pop();
}

void	c_multiple::align_random_axe_y_then_draw( REAL CONST * CONST pos, REAL CONST * CONST size, INT32 CONST axe )
{
	GOL::matrix::push();
		_b_push_done = true;
		GOL::matrix::translate3v( pos );
		do_random_rotate_axe( 1 );
		draw_one_low( size, axe );
	GOL::matrix::pop();
}

void	c_multiple::align_random_axe_z_then_draw( REAL CONST * CONST pos, REAL CONST * CONST size, INT32 CONST axe )
{
	GOL::matrix::push();
		_b_push_done = true;
		GOL::matrix::translate3v( pos );
		do_random_rotate_axe( 2 );
		draw_one_low( size, axe );
	GOL::matrix::pop();
}

FINLINE	void	c_multiple::pick_fn_for_align_then_draw()
{
	switch( get_align() )
	{
	case ALIGN_WORLD:			_pfn_align_then_draw =		&c_multiple::align_world_then_draw;
								break;
		//todo deal with this case in bdd_part and better in other
	case ALIGN_GRADIENT:		_pfn_align_then_draw =		&c_multiple::align_gradient_then_draw;
								break;
		//todo deal with this case in bdd_part and better in other
	case ALIGN_NORMAL:			_pfn_align_then_draw =		&c_multiple::align_normal_then_draw;
								_pfn_align_nor_then_draw =	&c_multiple::draw_one_at_tgn_private;
								break;
	case ALIGN_NORMAL_USING_Z:	_pfn_align_then_draw =		&c_multiple::align_normal_using_z_then_draw;
								_pfn_align_nor_then_draw =	&c_multiple::draw_one_at_tgn_private;	// perhaps we need a special one here
								break;
	case ALIGN_NORMAL_OFFSET:	_pfn_align_then_draw =		&c_multiple::align_normal_offset_then_draw;
								_pfn_align_nor_then_draw =	&c_multiple::draw_one_at_tgn_offset_private;
								break;
	case ALIGN_BILLBOARD:		_pfn_align_then_draw =		&c_multiple::align_billboard_then_draw;
								break;
	case ALIGN_RANDOM:			_pfn_align_then_draw =		&c_multiple::align_random_then_draw;
								break;
	case ALIGN_RANDOM_AXE_X:	_pfn_align_then_draw =		&c_multiple::align_random_axe_x_then_draw;
								break;
	case ALIGN_RANDOM_AXE_Y:	_pfn_align_then_draw =		&c_multiple::align_random_axe_y_then_draw;
								break;
	case ALIGN_RANDOM_AXE_Z:	_pfn_align_then_draw =		&c_multiple::align_random_axe_z_then_draw;
								break;
	default:					debug_break( "%s() the fn pointer will be NULL", __FUNCTION__ );
								break;
	}
}

//	begin end should always be called in pair
//		even if begin() return false it push the atts
//				compatibility reason for now if not the old processes calling begin/end will screw the stack
bool	c_multiple::begin()
{
	bool	retcode = false;

	if( g_lua_wrap_master->exits_function_by_name( hook_name ) )
		_s_type = TYPE_LUA;
	else
		_s_type = _s_type_ui;
	switch( _s_type )
	{
	case TYPE_LAYER:
		_p_layer_cached = c_layers::get_cur()->layer_get_raw_from_index( IMOD( _layers_cur, c_layers::LAYER_NB_MAX ) );
		if( _p_layer_cached && _p_layer_cached!= c_layer::get_cur() && _p_layer_cached->is_active() )
		{
			retcode = true;
			c_layer_att::push();
		}
		else
			_p_layer_cached = nullptr;			
		break;
	case TYPE_LAYERS:
	case TYPE_LAYERS_NAME:	
	case TYPE_LAYERS_PREV:	//hackhack to be stabilize and secure
		switch( _s_type )	//todoopt don't search every time
		{
		case TYPE_LAYERS:
			_p_layers_cached = c_module::get_cur()->layers_get_from_index( _layers_cur );
			break;
		case TYPE_LAYERS_NAME:
			_p_layers_cached = c_module::get_cur()->layers_get_from_name_short( c_multiple::cur->get_target_name().get() );
			if( !_p_layers_cached )
				_p_layers_cached = (c_layers*)c_obj_ui::find_by_class_and_name_symbo( "layers", c_multiple::cur->get_target_name() ); 
			break;
		case TYPE_LAYERS_PREV:
			_p_layers_cached = c_module::get_cur()->layers_get_from_index( c_layers::get_cur()->get_id_local() -1 -1 );	// -1 for id to index -1 for previous 
			break;
		}	
		if( _p_layers_cached && _p_layers_cached != c_layers::get_cur() && _p_layers_cached->is_active() )
		{
			retcode = true;
			c_layers_att::push();
		}
		else
			_p_layers_cached = nullptr;
		retcode = true;
		break;
	case TYPE_MODULE_NAME:	
		_p_module_cached = c_modules::get_cur()->module_get_from_name_short( c_multiple::cur->get_target_name().get() );
		if( _p_module_cached && _p_module_cached != c_module::get_cur() && _p_module_cached->is_active() )
		{
			retcode = true;
			//todo	c_module_att::pop()
		}
		else
			_p_module_cached = nullptr;
		break;
	default:
		retcode = true;
		break;
	}

	pick_fn_for_align_then_draw();
	_imod = _stop - _start + 1;
	set_multiple_layers_cur( _start );
	init_index();
	//make sur we don't draw bbox
	_b_bbox_see_on_cur_store = gb_bbox_see_on_cur;
	gb_bbox_see_on_cur = false;
	//hack hack was commented out ? bug ? order of command ?
	_nb = 1;

	_b_push_done = false;
	//todo		cur = this;
	return retcode;
}

void	c_multiple::end()
{
	switch( _s_type )
	{
	case TYPE_LAYER:
		if( _p_layer_cached )
			c_layer_att::pop();
		break;
	case TYPE_LAYERS:
	case TYPE_LAYERS_NAME:	
	case TYPE_LAYERS_PREV:	//hackhack to be stabilize and secure
		if( _p_layers_cached )
			c_layers_att::pop();
		break;
	case TYPE_MODULE_NAME:
		//if( _p_module_cached )
			//todo	c_module_att::pop()
	default:
		break;
	}
	gb_bbox_see_on_cur = _b_bbox_see_on_cur_store;
}
