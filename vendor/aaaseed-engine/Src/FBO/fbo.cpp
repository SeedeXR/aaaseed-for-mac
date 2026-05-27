#include "fbo.h"
#include "draw/tex.h"
#include "ui/strsymbo.h"
#include "render_buffer.h"
#include "image/bind_img.h"
#include "image/bind_img_2d.h"
#include "obj_ui/bdd/bdd_clear_screen.h"
#include "media/video/texture_flux_master.h"
#include "draw/stereo.h"
#include "draw/texture.h"
#include "gol/gol_pbo.h"
#include "infrastructure/viewport.h"
#include "spy.h"
#include "framebufferobject.h"


FACTORY_CREATE_V1( c_fbo, fbo, Frame buffer Object, fbo );

bool		c_fbo::b_allow = true;

c_fbo*		c_fbo::cur		= nullptr;
c_fbo*		c_fbo::prev		= nullptr;


namespace
{
	INT32	attachment_color_nb = c_fbo::COLOR_BUFFER_NB;
	INT32	attachment_bind_prev[ c_fbo::COLOR_BUFFER_NB + 2 ];	// +2 for depth and stencil
	//should be call only once at class creation
	void init_attachment_bind_prev()
	{
		for( UINT32 i = 0; i < c_fbo::COLOR_BUFFER_NB; ++i )
			attachment_bind_prev[i]	= -42;
		attachment_bind_prev[c_fbo::COLOR_BUFFER_NB]	= -42;	//depth
		attachment_bind_prev[c_fbo::COLOR_BUFFER_NB+1]	= -42;	//stencil
	}
};

INT32 c_fbo::get_attachment_bind_prev( INT32 which ) //return negative if not found
{
	if( which<=0 )
		return attachment_bind_prev[0];
	else if( which<COLOR_BUFFER_NB )
		return attachment_bind_prev[ which ]; 
	else if( which==16 ||  which==17 ) //depth or stencil	
		return attachment_bind_prev[ COLOR_BUFFER_NB + which - 16 ];
	return -42;
}

void c_fbo::c_init()
{
	attachment_color_nb = MIN( COLOR_BUFFER_NB, GOL::fbo_color_attachment_nb_max );
	init_attachment_bind_prev();
}

//maa this is a static
void c_fbo::disable()
{	
	c_frame_buffer_object::unbind();
	GOL::check_error_debug( "after c_fbo::disable()" );
}
namespace
{
	// documentation say up to 32, include are 16 ?
	CONSTEXPR GLenum	color_attachment_all[GOL::FBO_COLOR_ATTACHMENT_NB_MAX] =
	{
		GL_COLOR_ATTACHMENT0,		GL_COLOR_ATTACHMENT0+1,		GL_COLOR_ATTACHMENT0+2,		GL_COLOR_ATTACHMENT0+3,
		GL_COLOR_ATTACHMENT0+4,		GL_COLOR_ATTACHMENT0+5,		GL_COLOR_ATTACHMENT0+6,		GL_COLOR_ATTACHMENT0+7,
		GL_COLOR_ATTACHMENT0+8,		GL_COLOR_ATTACHMENT0+9,		GL_COLOR_ATTACHMENT0+10,	GL_COLOR_ATTACHMENT0+11,
		GL_COLOR_ATTACHMENT0+12,	GL_COLOR_ATTACHMENT0+13,	GL_COLOR_ATTACHMENT0+14,	GL_COLOR_ATTACHMENT0+15
	};

	//CONSTEXPR GLenum	color_attachment_NONE[16] =
	//{
	//	GL_NONE,	GL_NONE,	GL_NONE,	GL_NONE,
	//	GL_NONE,	GL_NONE,	GL_NONE,	GL_NONE,
	//	GL_NONE,	GL_NONE,	GL_NONE,	GL_NONE,
	//	GL_NONE,	GL_NONE,	GL_NONE,	GL_NONE
	//};

	typedef aaa::PIXEL_FORMAT PF;
	typedef GOL::INTERNAL_TYPE IT;

	//	DEPTH
	enum DEPTH_FORMAT : INT32											{	DEPTH_FORMAT_16 = 0,		DEPTH_FORMAT_24,		DEPTH_FORMAT_32,		DEPTH_FORMAT_24_STENCIL_8,	DEPTH_FORMAT_32_STENCIL_8,	DEPTH_FORMAT_MAX_NB		};
	CONSTEXPR PF		depth_pixel_format[	DEPTH_FORMAT_MAX_NB]	=	{	PF::DEPTH_16,				PF::DEPTH_32,			PF::DEPTH_32,			PF::DEPTH_24_STENCIL_8,		PF::DEPTH_32_STENCIL_8		};
	CONSTEXPR C_PCHAR_C	str_depth_format[	DEPTH_FORMAT_MAX_NB]	=	{	"16 bits",					"24 bits",				"32 bits",				"24 bits Stencil 8 bits",	"32 bits Stencil 8 bits"	};
	CONSTEXPR IT		depth_internal_type[DEPTH_FORMAT_MAX_NB]	=	{	IT::DEPTH_16,				IT::DEPTH_24,			IT::DEPTH_32,			IT::DEPTH_24_STENCIL_8,		IT::DEPTH_24_STENCIL_8		};

	//	STENCIL
	enum STENCIL_FORMAT : INT32											{	STENCIL_FORMAT_INDEX1 = 0,	STENCIL_FORMAT_INDEX4,	STENCIL_FORMAT_INDEX8,	STENCIL_FORMAT_INDEX16,		STENCIL_FORMAT_MAX_NB	};
	CONSTEXPR C_PCHAR_C	str_stencil_format[STENCIL_FORMAT_MAX_NB]	=	{	"1 bit",					"4 bits",				"8 bits",				"16 bits"					};
	CONSTEXPR INT32		gl_stencil_format[	STENCIL_FORMAT_MAX_NB]	=	{	GL_STENCIL_INDEX1,			GL_STENCIL_INDEX4,		GL_STENCIL_INDEX8,		GL_STENCIL_INDEX16			};
	CONSTEXPR INT32		stencil_bit_nb[		STENCIL_FORMAT_MAX_NB]	=	{	1,							4,						8,						16							};
}

namespace n_fbo
{
	CONSTEXPR C_PCHAR_C	str_minification[3]		=	{	"Def",	"Nearest",	"Linear"	};
	CONSTEXPR C_PCHAR_C	str_magnification[7]	=	{	"Def",	"Nearest",	"Linear",
																"nearest_mipmap_nearest",	"linear_mipmap_nearest",
																"nearest_mipmap_linear",	"linear_mipmap_linear"
													};

	CONSTEXPR	INT32 ATTACHMENT_PARAM_NB = 11;
	CONSTEXPR	INT32 TEK_PARAM_NB		= 4;
	CONSTEXPR	INT32 DEPTH_PARAM_NB	= 7;
	CONSTEXPR	INT32 FBO_PARAM_NB		= 10 + PARAM_IMG_SIZE_NB + c_fbo::COLOR_BUFFER_NB * (ATTACHMENT_PARAM_NB+2);
	CONSTEXPR	INT32 GROUP_PARAM_NB	= 3;
	CONSTEXPR	INT32 PARAM_NB_MAX	= 	TEK_PARAM_NB
									+	DEPTH_PARAM_NB
									+	FBO_PARAM_NB
									+	GROUP_PARAM_NB;

#define	PARAM_DEF_COLOR_ATTACHMENT( name, DEF_ACTIVE )\
	PARAM_DEF_GROUP_CLOSED(			name, ATTACHMENT_PARAM_NB+1	)\
		DEF_ACTIVE(					name##_active			)\
		PARAM_DEF_BIND_2D_ALONE(	name##_bind_2d_asked	)\
		PARAM_DEF_BANK_BIND_2D_OUT(	name##_bind_2d_used		)\
		PARAM_DEF_BOOL_OFF(			name##_define_format	)\
		PARAM_DEF_INT32(			name##_channel_nb,		4, 3,		1, 4		) \
		PARAM_DEF_SYMBO(			name##_channel_type,	GOL::INTERNAL_TYPE::UINT_16, GOL::INTERNAL_TYPE::UINT_8,	(INT32)GOL::INTERNAL_TYPE::FLOAT_32,	GOL::internal_type_str	) \
		PARAM_DEF_GROUP_CLOSED(		name##_tek,				5 )\
			PARAM_DEF_BOOL_OFF(			name##_mipmap_generate		)\
			PARAM_DEF_SYMBO_PSTR(		name##_minification,		1,0,		str_minification	)\
			PARAM_DEF_SYMBO_PSTR(		name##_magnification,		6,0,		str_magnification	)\
			PARAM_DEF_BOOL_OFF(			name##_on_cpu				)\
			PARAM_DEF_BOOL_OFF(			name##_on_cpu_use_pbo		)

#define	PARAM_DEF_COLOR_ATTACHMENT_ON( name )	PARAM_DEF_COLOR_ATTACHMENT(	name, PARAM_DEF_BOOL_ON		)
#define	PARAM_DEF_COLOR_ATTACHMENT_OFF( name )	PARAM_DEF_COLOR_ATTACHMENT(	name, PARAM_DEF_BOOL_OFF	)

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_BOOL_OFF(			active				)
		PARAM_DEF_REF(				name_symbo			)
		PARAM_DEF_INT32_LOCKED(		id					)
//		PARAM_DEF_INT32_LOCKED(		id_cur				)
		PARAM_DEF_BOOL_LOCKED(		valid				)
		PARAM_DEF_BOOL_ON(			define_size_format	)
		PARAM_DEF_BOOL_OFF(			size_from_previous	)
		PARAM_DEF_IMG_SIZE_FORMAT(						)		
		PARAM_DEF_BOOL_OFF(			bgr					)
//		PARAM_DEF_BOOL_OFF(			ring_buffer_active	)
//		PARAM_DEF_INT32(				ring_buffer_nb,		2, 1,	1, 1024	)	// 1024 should be enough

		PARAM_DEF_GROUP( Depth, DEPTH_PARAM_NB )
			PARAM_DEF_BOOL_OFF(			depth_use				)
			PARAM_DEF_BOOL_OFF(			depth_as_texture		)
			PARAM_DEF_BIND_2D_ALONE(	depth_bind_2d_asked		)
			PARAM_DEF_BANK_BIND_2D_OUT(	depth_bind_2d_used		)
			PARAM_DEF_SYMBO_PSTR_ONE(	depth_format,			str_depth_format	)
			PARAM_DEF_BOOL_OFF(			depth_on_cpu			)
			PARAM_DEF_BOOL_OFF(			depth_on_cpu_use_pbo	)

		PARAM_DEF_INT32_LOCKED_XY(	size				)

		PARAM_DEF_GROUP_CLOSED( TEK, TEK_PARAM_NB )
			PARAM_DEF_BOOL_LOCKED(	attach				)	// when update provoke an attach, it is set at true until next update
			PARAM_DEF_BOOL_ON(		verbose				)

			// todo depth_use and stencil or not necessary, add a new format to enum, 0 for no buffer
			PARAM_DEF_BOOL_OFF(		stencil_use			)
			PARAM_DEF_SYMBO(		stencil_format,		1, 2,	STENCIL_FORMAT_MAX_NB - 1,	str_stencil_format		)

		PARAM_DEF_NONE( clear )
		
		PARAM_DEF_COLOR_ATTACHMENT_ON(	tex_1			)

		PARAM_DEF_GROUP_CLOSED(	tex_other,		(c_fbo::COLOR_BUFFER_NB-1) * (ATTACHMENT_PARAM_NB+2) )
			PARAM_DEF_2_8( tex, PARAM_DEF_COLOR_ATTACHMENT_OFF )
	};
}

namespace {
	o_str	o_sumup;
}

void c_fbo::build_sumup_one	( o_str& o, INT32 CONST index ) CONST
{
	auto a = &_attach_info[index];
	o.add( a->_channel_nb );
	o.add_char( 'x' );
	o.add( GOL::internal_type_str[ (INT32)a->_channel_type ] );
}

void c_fbo::build_sumup( o_str& o ) CONST
{
	o.set( get_size_x() );
	o.add_char( 'x' );
	o.add( get_size_y() );
	o.add_space();

	for( INT32 i = 0; i < _nb_active_tex; ++i )
	{
		INT32 index = _active_tex_index[ i ];
		auto a = &_attach_info[index];
		if( a->_b_active_ui	)
		{
			o.add( index );
			o.add_char( '-' );
			build_sumup_one( o, index );
			o.add_char( ' ' );
		}
	}
}

void c_fbo::param_init_pt()
{
	INT32	h=0;

	param_set_pt( h, get_pt_active()			);
	param_set_pt( h, get_name_symbo()			);

	param_set_pt( h, _fbo_id_gl_name_used		);
	param_set_pt( h, _b_valid					);		
	param_set_pt( h, _b_define_size_format_ui	);
	param_set_pt( h, _b_size_from_previous_ui	);
	param_set_pt_2( h, _size_ui					);
	param_set_pt( h, _channel_nb_def_ui			);
	param_set_pt( h, _s_channel_type_def_ui		);
	param_set_pt( h, _b_bgr_ui					);
//	param_set_pt( h, _b_ring_buffer_active_ui	);
//	param_set_pt( h, _ring_buffer_nb_ui			);

	++h;
		param_set_pt( h, _b_depth_use_ui			);
		param_set_pt( h, _b_depth_as_texture_ui		);
		param_set_pt( h, _depth_bind_2d_ui			);
		param_set_pt( h, _depth_bind_2d_out			);
		param_set_pt( h, _s_depth_format_ui			);
		param_set_pt( h, _b_depth_on_cpu_ui			);
		param_set_pt( h, _b_depth_on_cpu_use_pbo_ui	);

	param_set_pt_2( h, _size_out	);

	build_sumup( o_sumup );
	get_param(h)->set_comment( o_sumup	);
	++h;
		param_set_pt( h, _b_attach_out			);		
		param_set_pt( h, _b_verbose_ui			);
		param_set_pt( h, _b_stencil_use_ui		);
		param_set_pt( h, _s_stencil_format_ui	);

	param_attach_obj( h, _bdd_clear_screen	);

	for( INT32 i = 0; i < COLOR_BUFFER_NB; ++i )
	{
		if( i == 1 )
			++h;

		auto a = &_attach_info[i];
		if( a->_b_active_ui )
		{
			o_sumup.erase();
			build_sumup_one( o_sumup, i );
			get_param(h)->set_comment( o_sumup	);
		}
		else
			get_param(h)->clear_comment();
		++h;
			param_set_pt( h, a->_b_active_ui		);
			param_set_pt( h, a->_bind_2d_dst_ui		);
			param_set_pt( h, a->_bind_2d_dst_out	);
			param_set_pt( h, a->_b_define_format_ui	);
			param_set_pt( h, a->_channel_nb_ui		);
			param_set_pt( h, a->_channel_type_ui	);
			++h;
//				param_set_pt( h, a->_bind_2d_stride_ui			);
				param_set_pt( h, a->_b_mipmap_generate_ui		);
				param_set_pt( h, a->_s_minification_ui			);
				param_set_pt( h, a->_s_magnification_ui			);
				param_set_pt( h, a->_b_on_cpu_ui				);
				param_set_pt( h, a->_b_on_cpu_use_pbo_ui		);
	}

	err_param_init_pt( h );
}

CONSTRUCTOR_CREATE( c_fbo )
	,_frame_buffer_object		(nullptr)
	,_depth_buffer				(nullptr)
	,_stencil_buffer			(nullptr)
	,_bdd_clear_screen			(nullptr)
	,_attachment_slot			(0)
	,_s_first_color_attachment	(0)
	,_b_define_size_format		(false)
	,_b_attach_out				(false)
{
	init();
	param_init_with( n_fbo::param, n_fbo::PARAM_NB_MAX );
}

void c_fbo::init()
{
	_fbo_id_gl_name_used		= 0;
	_b_bgr_ui					=	false	;

	_b_depth_use				=	false	;
	_b_depth_as_texture			=	false	;
	_depth_bind_2d_out				=	-1		;
	_s_depth_format				=	-1		;

	_b_stencil_use				=	false	;
	_s_stencil_format			=	-1		;
	clear_v2( _size_out );
	clear_v2( _size_asked );
	_nb_active_tex				=	0		;
	_active_tex_index_max		=	0		;
	_s_first_color_attachment	=	0		;
	_b_valid					=	false	;
	_b_ready_to_use				=	false	;

	for( UINT32 i = 0; i < COLOR_BUFFER_NB; ++i )
	{
		auto a = &_attach_info[i];
		a->_pixel_format			=	aaa::PIXEL_FORMAT::UNKNOWN ;
		a->_channel_nb				=	-1;
		a->_channel_type			=	GOL::INTERNAL_TYPE::UNKNOWN;
		a->_b_active				=	false;
		a->_bind_2d_dst_out			=	-1;
		a->_b_mipmap_generate		=	false;
		a->_pbo_bind[ 0 ]			=	-1;
		a->_pbo_bind[ 1 ]			=	-1;
		a->_pbo_buffer_index_cur	=	0;
		a->_tex_size_mb				=	.0;
	}

	_pbo_depth_bind[ 0 ]	= -1;
	_pbo_depth_bind[ 1 ]	= -1;
	_pbo_depth_buffer_cur	= 0;
	
	_buffer_size_depth		= 0.;
	_buffer_size_stencil	= 0.;

	obj_get( _bdd_clear_screen );
}

void	c_fbo::dealloc()
{
	obj_delete( _bdd_clear_screen );
	if( _frame_buffer_object )
	{
		_frame_buffer_object->bind();
		detach_all();
		obj_delete( _frame_buffer_object );
		c_frame_buffer_object::unbind();
	}
	obj_delete( _depth_buffer );
	obj_delete( _stencil_buffer );
}

c_fbo::~c_fbo()
{
	if( cur == this )
		cur = nullptr;
	if( prev == this )
		prev = nullptr;
	dealloc();
}

AAA_ERR c_fbo::load_do_after( o_str CONST & filename )
{
	_bdd_clear_screen->load_from_file_replace_ext( filename );
	return AAA_OK;
}

AAA_ERR c_fbo::save_do_after( o_str CONST & filename )
{
	_bdd_clear_screen->save_to_file_replace_ext( filename );
	return AAA_OK;
}

void c_fbo::detach_all()
{	
	_frame_buffer_object->detach_all();
	if( _depth_buffer != nullptr )
	{
		_frame_buffer_object->detach_render_buffer( _depth_buffer->get_id() );
		GOL::fbo_size_mb -= _buffer_size_depth;
		obj_delete( _depth_buffer );
	}
	if( _stencil_buffer != nullptr )
	{
		_frame_buffer_object->detach_render_buffer( _stencil_buffer->get_id() );
		GOL::fbo_size_mb -= _buffer_size_stencil;
		obj_delete( _stencil_buffer );
	}
}

namespace
{
	template<typename T>
	FINLINE void	ui_cmp( T& o, T ui, bool& b )
	{
		if( o != ui )
			b = true;
	}
}

bool c_fbo::update_check_param()
{
	bool	b_return = false;

	if( _b_size_from_previous_ui && cur )
		cpy_v2( _size_asked, cur->_size_out );
	else
		cpy_v2( _size_asked, _size_ui );

	_size_asked[0] = MIN( _size_asked[0], GOL::viewport_max[0] );
	_size_asked[1] = MIN( _size_asked[1], GOL::viewport_max[1] );

	GOL::adjust_tex_size_to_valid( _size_asked[0], _size_asked[1] );
	

	if( _b_define_size_format_ui )
	{
		ui_cmp(	_size_out[0],			_size_asked[0], b_return );
		ui_cmp(	_size_out[1],			_size_asked[1], b_return );
		ui_cmp( _b_define_size_format,	_b_define_size_format_ui, b_return );
		if( b_return )
			return true;
	}
//	else
//		return true;	//	!_b_define_size_format_ui )	//todomona something wrong when !_b_define_size_format : too much work


	ui_cmp( _b_bgr, _b_bgr_ui, b_return );	//global so strange

	ui_cmp( _b_depth_use, _b_depth_use_ui, b_return	);
	if( _b_depth_use_ui )
	{
		ui_cmp( _b_depth_as_texture,	_b_depth_as_texture_ui, b_return );
		if( _b_depth_as_texture	)
			ui_cmp( _depth_bind_2d_out,	_depth_bind_2d_ui,		b_return );
		ui_cmp( _s_depth_format,		_s_depth_format_ui,		b_return );
	}
	if( b_return )
		return true;

	ui_cmp( _b_stencil_use, _b_stencil_use_ui, b_return );
	if( _b_stencil_use_ui )
		ui_cmp( _s_stencil_format, _s_stencil_format_ui, b_return );

	if( b_return )
		return true;

	aaa::PIXEL_FORMAT pf_def = aaa::c_pixel_format::make_format_from_channel_gltype( _channel_nb_def_ui, _s_channel_type_def_ui, _b_bgr_ui );
	for( INT32 i = 0; i < attachment_color_nb; ++i )
	{
		auto & a = _attach_info[i];
		ui_cmp( a._b_active, a._b_active_ui, b_return );

		// can we optimize ? (Maa 2020)
		//if( !b_return && _b_active[i] )
		if( a._b_active_ui )
		{
			ui_cmp( a._bind_2d_dst_out,		a._bind_2d_dst_ui,			b_return );
			ui_cmp( a._b_mipmap_generate,	a._b_mipmap_generate_ui,	b_return );
			if( _b_define_size_format_ui )
			{
				aaa::PIXEL_FORMAT pixel_format;
				if( a._b_define_format_ui )
					pixel_format = aaa::c_pixel_format::make_format_from_channel_gltype( a._channel_nb_ui, a._channel_type_ui, _b_bgr_ui );
				else
					pixel_format = pf_def;
				INT32 bind = a._bind_2d_dst_ui;
				c_img_2d* img = g_bind_img_2d->get_image_data( bind );
				if( !img || !img->is_size_format( _size_asked[0],_size_asked[1], pixel_format ) )
					b_return = true;
			}
		}
		if( b_return )
			return true;
	}

	return b_return;
}


void c_fbo::attach()
{
	if( !GOL::b_fbo_do )
		return;

	SPY_PUSH_RANGE( "fbo::attach", spy::LAYERS );

	_nb_active_tex				= 0;
	_active_tex_index_max		= -1;
	_s_first_color_attachment	= 0;

	aaa::PIXEL_FORMAT	CONST pixel_format_def		= aaa::c_pixel_format::make_format_from_channel_gltype( _channel_nb_def_ui, _s_channel_type_def_ui, _b_bgr_ui );
	GLenum				CONST internal_format_def	= GOL::make_internal_format_gl( _channel_nb_def_ui, _s_channel_type_def_ui );

	for( INT32 i = 0; i < attachment_color_nb; ++i )
	{
		auto a = &_attach_info[i];
		if( a->_b_active_ui )
		{		
			INT32 bind = a->_bind_2d_dst_ui;
			// generate texture
			tex_2d_bind_no_gpu_move( bind );
			if( _b_define_size_format_ui )
			{
#if	AAA_DEBUG()
				if( _size_asked[0] <= 0 || _size_asked[1] <= 0 )
				{
					debug_break( "in %s() negative or null attachment size asked : %d x %d", __FUNCTION__, _size_asked[0], _size_asked[1] );
				}
				else
#endif
				{
					INT32 channel_nb;
					GOL::INTERNAL_TYPE channel_type;
					aaa::PIXEL_FORMAT pixel_format;
					GLenum	internal_format;
					if( a->_b_define_format_ui )
					{
						channel_nb		= a->_channel_nb_ui;
						channel_type	= a->_channel_type_ui;
						pixel_format	= aaa::c_pixel_format::make_format_from_channel_gltype( channel_nb, channel_type, _b_bgr_ui );
						internal_format = GOL::make_internal_format_gl( channel_nb, channel_type );
					}
					else
					{
						channel_nb		= _channel_nb_def_ui;
						channel_type	= _s_channel_type_def_ui;
						pixel_format	= pixel_format_def;
						internal_format = internal_format_def;
					}
					if( !tex2d.is_size_internal_format( _size_asked[0], _size_asked[1], internal_format ) )
					{
						GLenum last_internal_format;
						if( _b_verbose_ui )
							last_internal_format = GOL::get_2d_internal_format();

						// we need to change the img or an image will take over
						c_img_2d* img = g_bind_img_2d->get_always( bind );
						img->init_with_size_no_cpu_mem( _size_asked[0], _size_asked[1], pixel_format, __FUNCTION__ );
						img->set_gpu_move( false );

						UINT32	nb_byte;
						//todotex
						switch( channel_type )
						{
		//				case GOL::ITF_DEFAULT:
						case GOL::INTERNAL_TYPE::UINT_8:	nb_byte = 1;	break;
						case GOL::INTERNAL_TYPE::UINT_16:	nb_byte = 2;	break;
						case GOL::INTERNAL_TYPE::FLOAT_16:	nb_byte = 2;	break;
						case GOL::INTERNAL_TYPE::FLOAT_32:	nb_byte = 4;	break;
						default:	debug_break( "Unknown format %d", channel_type );
									return;
						}

						GOL::fbo_size_mb -= a->_tex_size_mb;
						a->_tex_size_mb = ( _size_asked[0] * _size_asked[1] * channel_nb * nb_byte ) / DOUBLE(1024 * 1024);
						GOL::fbo_size_mb += a->_tex_size_mb;

						tex2d.image_level( 0, _size_asked[0], _size_asked[1], channel_nb, channel_type ); 

						GOL::check_error_debug( "create texture" );

						if( _b_verbose_ui && GOL::get_2d_internal_format() != last_internal_format )
						{
							verbose_print_string( "In %s() bind %d change fbo internal format", __FUNCTION__, bind );
							VERBOSE_PRINT_STRING( "\t from %.64s", GOL::get_format_str_from_glenum( last_internal_format ) );
							VERBOSE_PRINT_STRING( "\t   to %.64s", GOL::get_2d_internal_format_as_str( ) );
						}

					}
					a->_channel_nb		= channel_nb;
					a->_channel_type	= channel_type;
					a->_pixel_format	= pixel_format;
				}
			}
			else
			//todo we don't store channel_nb, channel_type, pixel_format how is gonna play for tex on cpu for example ?
			{
			}

			//todo the whole cmp update have to be reworked
			//todo change when the byte nb will be by texture
			if( _nb_active_tex == 0 )
			{
				//todo there is something still strange here
				if( !_b_define_size_format_ui )
					tex2d.get_size_cur( _size_out[0], _size_out[0] );
			}

			if( a->_b_mipmap_generate_ui )
			{
				//todo
				GOL::set_tex_2d_max_level();
				//tex2d.set_min_mag( 2, 0 );
			}
			else
			{
				GOL::set_tex_2d_max_level( 0 );
				//we need this for openvr
				//GOL::set_tex_2d_minification( GL_LINEAR );	//todo we should be able to choose GL_NEAREST
			}

			//todo
			// completeness
			tex2d.set_wrap( GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE );
			//tex_2d_bind_no_gpu_move( 0 );

			// Attach texture to framebuffer color buffer
			_frame_buffer_object->attach_texture( GL_TEXTURE_2D, tex2d.get_name_gl( bind ), GL_COLOR_ATTACHMENT0 + i );

			GOL::check_error_debug( "FBO : attach texture" );

			// use for bdd_feedback
			if( _s_first_color_attachment==0 )
				_s_first_color_attachment = GL_COLOR_ATTACHMENT0 + i;
			
			_active_tex_index[ _nb_active_tex ] = i;
			_active_tex_index_max = i;
			++_nb_active_tex;

			a->_bind_2d_dst_out		= a->_bind_2d_dst_ui;
			a->_b_mipmap_generate	= a->_b_mipmap_generate_ui;
		}
		a->_b_active = a->_b_active_ui;
	}	// end color attachments loop

	if( _b_depth_use_ui )
	{
		auto pf_depth = depth_pixel_format[_s_depth_format_ui];
		_buffer_size_depth = (_size_asked[0] * _size_asked[1] * aaa::c_pixel_format::get_byte_per_pixel(pf_depth)) / DOUBLE( 1024 * 1024 );
		_attachment_slot = (_s_depth_format_ui >= DEPTH_FORMAT_24_STENCIL_8) ? GL_DEPTH_STENCIL_ATTACHMENT : GL_DEPTH_ATTACHMENT;
		GLenum internal_format_wanted = aaa::c_pixel_format::get_gl_format_internal(pf_depth);
		if( _b_depth_as_texture_ui )
		{
			INT32 bind = _depth_bind_2d_ui;
			// generate texture
			tex_2d_bind_no_gpu_move( bind );
			if( !tex2d.is_size_internal_format( _size_asked[0], _size_asked[1], internal_format_wanted ) )
			{
				GLenum last_internal_format;
				if( _b_verbose_ui )
					last_internal_format = GOL::get_2d_internal_format();

				// we need to change the img or an image will take over
				c_img_2d* img = g_bind_img_2d->get_always( bind );
				img->init_with_size_no_cpu_mem( _size_asked[0], _size_asked[1], pf_depth, __FUNCTION__ );
				img->set_gpu_move( false );

				//todo what happen if we change on the fly
				//if( !_b_mipmap_generate[i] )
				{	//we need this to keep texture visible
					GOL::set_tex_2d_minification( GL_LINEAR );	//todo we should be able to choose GL_NEAREST0
					GOL::set_tex_2d_magnification( GL_LINEAR );
					GOL::set_tex_2d_max_level( 0 );
				}

				tex2d.image_level(	0,
									_size_asked[0], _size_asked[1], 1,
									depth_internal_type[_s_depth_format_ui],	//GOL::make_internal_depth_format_gl
									nullptr, img->get_gl_format(),  aaa::c_pixel_format::get_gl_type(pf_depth)
									//format and type don't really matter because we transmit no data but can eventually trigger error				
								);
				//todo use this at somme point
				//in this case any future change of the texture implie destroy the id and regenerate one
				//todo this have to be added in tex, but warning it will be unmutable 
				//glTexStorage2D(	GL_TEXTURE_2D,  1,	internal_format_wanted, _size_asked[0], _size_asked[1] );

				GOL::check_error_debug( "create texture" );

				//avoid transfer img to gpu
				img->set_gpu_in_sync();

				if( _b_verbose_ui && GOL::get_2d_internal_format() != last_internal_format )
				{
					verbose_print_string( "In %s() bind %d change fbo internal format", __FUNCTION__, bind );
					VERBOSE_PRINT_STRING( "\t from %.64s", GOL::get_format_str_from_glenum( last_internal_format ) );
					VERBOSE_PRINT_STRING( "\t   to %.64s", GOL::get_2d_internal_format_as_str( ) );
				}
			}

			//if( _b_mipmap_generate[i] )
			//	_b_mipmap_generate_needed = true;

			//tex2d.adjust( false );
			//todo
			tex2d.set_wrap( GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER );
			GOL::set_tex_2d_border_color( unit_x_v4fp32 );

			//tex_2d_bind_no_gpu_move( 0 );

			// Attach texture to framebuffer color buffer
			//got a warning with this one
			_frame_buffer_object->attach_texture( GL_TEXTURE_2D, tex2d.get_name_gl( bind ), _attachment_slot );
			//_frame_buffer_object->framebuffer_texture_nd( attachment_slot, GL_TEXTURE_2D, tex2d.get_name( bind ) );

			_depth_bind_2d_out = _depth_bind_2d_ui;
		}
		else
		{	// version render buffer : old gl way but no access as texture to depth except doing read or copy
			_depth_buffer = new c_render_buffer;
			_depth_buffer->set_storage( internal_format_wanted, _size_asked[0], _size_asked[1] );
			_frame_buffer_object->attach_render_buffer( _depth_buffer->get_id(), _attachment_slot );
		}
		GOL::check_error_debug( "FBO : attach depth" );
		GOL::fbo_size_mb += _buffer_size_depth;

		_b_depth_as_texture = _b_depth_as_texture_ui;
		_s_depth_format = _s_depth_format_ui;
	}

	if( _b_stencil_use_ui )
	{
		_buffer_size_stencil = ( _size_asked[0] * _size_asked[1] * stencil_bit_nb[_s_stencil_format_ui] ) / DOUBLE( 8 * 1024 * 1024 );	// 8 to go from byte to bit
		_stencil_buffer = new c_render_buffer;
		_stencil_buffer->set_storage( gl_stencil_format[_s_stencil_format_ui], _size_asked[0], _size_asked[1] );
		GOL::fbo_size_mb += _buffer_size_stencil;
		_frame_buffer_object->attach_render_buffer( _stencil_buffer->get_id(), GL_STENCIL_ATTACHMENT );
		GOL::check_error_debug( "FBO : attach stencil");

		_s_stencil_format = _s_stencil_format_ui;
	}
	
	if( _nb_active_tex > 0 || _b_depth_use_ui || _b_stencil_use_ui )
	{
		_b_valid = _frame_buffer_object->check_valid();
		if( !_b_valid )
		{
			err_print( "%s() FBO id %d : frame is not complete ", __FUNCTION__, _fbo_id_gl_name_used );
			for( INT32 i=0; i<_nb_active_tex; ++i )
			{
				INT32 tex_index = _active_tex_index[i];
				auto a = &_attach_info[tex_index];
				if( a->_bind_2d_dst_out == _depth_bind_2d_out )
				{
					err_print( " probably because attachment %d bind %d is the same than depth attachment", tex_index+1, _depth_bind_2d_out );
				}
			}
		}
	}
	
	if( _b_valid )
	{
		if( _b_define_size_format_ui )	//todo other case ?
			cpy_v2( _size_out, _size_asked );
		_b_define_size_format = _b_define_size_format_ui;
		_b_depth_use = _b_depth_use_ui;
		_b_stencil_use = _b_stencil_use_ui;
		_b_bgr = _b_bgr_ui;

		if( _b_verbose_ui )
			verbose_print_string( "%s() FBO id %d : we got GL_FRAMEBUFFER_COMPLETE framebuffer ready", __FUNCTION__, _fbo_id_gl_name_used );
		//TODO correct this if we skip color_ATTACHMENT _active_tex_index_max WILL BE a problem

		//_frame_buffer_object->bind();
		//	frame_buffer have the memory of this
		GOL::set_draw_buffers( _active_tex_index_max+1, color_attachment_all );
		//c_frame_buffer_object::disable();

		// to view stencil in shader as texture
		//	https://stackoverflow.com/questions/27535727/opengl-create-a-depth-stencil-texture-for-reading
		_b_attach_out = true;
		//GOL::set_internal_type_def( internal_type_last );
	}
	else
	{
	
	}

	GOL::check_error_debug( "FBO alloc" );

	SPY_POP_RANGE();
}


void c_fbo::leave()
{
	if( !GOL::b_fbo_do )
		return;

	SPY_PUSH_RANGE( "fbo::leave", spy::LAYERS );

		auto bind_store = tex2d.get_index(); 
		bool b_bind_restore = false;

		//todo  2021 Maa added a quick fix to check fbo possibility
		//todo perhaps we should factorize using c_pbo
		bool b_pbo_use = texture_flux_master->is_move_from_gpu_use_pbo() && GOL::b_pbo_can;
		bool b_mipmap_generate = tex2d.is_mipmap_generate();

		bool b_fbo_binded = false;

		for( INT32 i = 0; i < _nb_active_tex; ++i )
		{
			INT32 index = _active_tex_index[ i ];
			auto a = &_attach_info[index];

			INT32 bind = a->_bind_2d_dst_out;
			attachment_bind_prev[i] = bind;
			
		// MIPMAP and MINIFICATION and MAGNIFICATION
			if( GOL::b_direct_state_access_use )
			{
				GLuint CONST gl_bind = tex2d.get_name_gl( bind );
				if( b_mipmap_generate && a->_b_mipmap_generate )
					GOL::generate_mipmap_texture_direct( gl_bind );
				if( a->_s_minification_ui )
					GOL::set_texture_minification( gl_bind, tex::gl_minmag_mode[ a->_s_minification_ui - 1 ] );
				if( a->_s_magnification_ui )
					GOL::set_texture_magnification( gl_bind, tex::gl_minmag_mode[ a->_s_magnification_ui - 1 ] );
			}
			else
			{
				bool b_bind_done = false;
				if( b_mipmap_generate && a->_b_mipmap_generate )
				{
					tex2d.bind( bind );
					b_bind_restore = true;
					b_bind_done = true;
					GOL::generate_mipmap_2d_direct();
				}
				if( a->_s_minification_ui )
				{
					if( !b_bind_done )
					{
						tex2d.bind( bind );
						b_bind_restore = true;
						b_bind_done = true;
					}
					GOL::set_tex_2d_minification( tex::gl_minmag_mode[ a->_s_minification_ui - 1 ] );
				}
				if( a->_s_magnification_ui )
				{
					if( !b_bind_done )
					{
						tex2d.bind( bind );
						b_bind_restore = true;
					}
					GOL::set_tex_2d_magnification( tex::gl_minmag_mode[ a->_s_magnification_ui - 1 ] );
				}
			}

		// TRANSFER to CPU
			if( a->_b_on_cpu_ui )
			{
				c_img_2d*	p_img = g_bind_img_2d->get_with_image_size( bind, _size_out[0],_size_out[1], a->_pixel_format );

				//	tex::push();
				//		tex_2d_bind_no_gpu_move( bind );
				//		p_img->get_tex_image();
				//	tex::pop();

		//todomona make sure we have an async version functionning
				if( p_img )
				{
					TBUF_INC_OBJ( tbuf::CH_GPU_READ, 1., "read_from_gpu", this );
					bool b_done = false;
					if( b_pbo_use && a->_b_on_cpu_use_pbo_ui )
					{
						SPY_PUSH_RANGE( "PBO Read Pixels", spy::IMG );
											
							UINT64 size = p_img->get_data_size_used();	//not sure : the data can be bigger that its actual use, eg, after a realloc (we should have storage and image fns)
							//todo we double buffer here so we need several pbo in contradiction with pref choice pbo_use_several
							a->_pbo_bind[0] = GOL::pbo_alloc( a->_pbo_bind[0], size, false );	
							a->_pbo_bind[1] = GOL::pbo_alloc( a->_pbo_bind[1], size, false );

							if( a->_pbo_bind[0] != -1 && a->_pbo_bind[1] != -1 )
							{
								if( !b_fbo_binded )
								{
									_frame_buffer_object->bind();
									b_fbo_binded = true;
								}
								//todo use glNamedFramebufferReadBuffer
								// PBO readback
								GOL::set_read_buffer( GL_COLOR_ATTACHMENT0 + index );
												
								INT32 buffer_next = ( a->_pbo_buffer_index_cur + 1 ) % 2;

								SPY_PUSH_RANGE( "Fbo Pbo ReadPixels", spy::IMG );		
									// readback of current FBO in the next buffer
									GOL::pbo_do_bind( a->_pbo_bind[ buffer_next ], false );
									//hack? force GL_BGRA since it's more rapid ?  GL_RGB is slow, GL_RGBA is faster but much slower than BG_BGRA
									// previous line was thru long time ago (2020 + now)
									INT32 gl_format = p_img->get_gl_format( texture_flux_master->is_move_from_gpu_bgr() );
									GOL::read_pixels( 0,0, _size_out[0],_size_out[1], gl_format, aaa::c_pixel_format::get_gl_type(a->_pixel_format), nullptr );
			//						GOL::get_tex_image_2d( 0, format, aaa::c_pixel_format::get_gl_type(pixel_format), nullptr );		
									GOL::check_error_debug( "readpixels in fbo transfert cpu" );
								SPY_POP_RANGE();
						
								GOL::pbo_do_bind( a->_pbo_bind[ a->_pbo_buffer_index_cur ], false );
								SPY_PUSH_RANGE( "Fbo Pbo MEMCPY", spy::IMG );
									void* src = (UINT8*) GOL::pbo_do_map( false );
									void* dst = p_img->get_data();					
									MEMCPY( dst, src, size, __FUNCTION__ );
									GOL::pbo_do_unmap( false );
								SPY_POP_RANGE();
								GOL::pbo_do_bind( -1, false );

								a->_pbo_buffer_index_cur = buffer_next;					
								p_img->set_changed();	//side fx trigger uneeded transfert back to GPU but not with set_gpu_move(false) before
								b_done = true;
							}
							else
							{
								debug_break( "could not allocate pbo for color attachment %d", i );
							}
						
						SPY_POP_RANGE();
					}
					// blocking one used when no pbo or pbo failed
					if( !b_done )
					{
						if( !b_fbo_binded )
						{
							_frame_buffer_object->bind();
							b_fbo_binded = true;
						}
						SPY_PUSH_RANGE( "Read Pixels", spy::IMG );
							p_img->read_pixels( 0,0, _size_out[0],_size_out[1], GL_COLOR_ATTACHMENT0 + index );
						SPY_POP_RANGE();
					}
					//hack we should merge the two approaches
					//p_img->set_changed();	//side fx trigger uneeded transfert back to GPU
					p_img->set_gpu_in_sync( bind );	//avoid transfert CPU GPU
					TBUF_DEC_OBJ( tbuf::CH_GPU_READ, 1., "read_from_gpu", this );
				}
	//			GOL::print_errors( "transfer cpu" );
			}
		}

		if( _b_depth_as_texture )
			attachment_bind_prev[	COLOR_BUFFER_NB ] = _depth_bind_2d_out;	//depth
		//attachment_bind_prev[	COLOR_BUFFER_NB ] = _b_depth_as_texture ? _depth_bind_2d : -42;	//depth

#if 1	// to be done
		//todo stencil will require more work
		if( _b_depth_on_cpu_ui )
		{
			//_attachment_slot = (_s_depth_format == DEPTH_FORMAT_24_STENCIL_8) ? GL_DEPTH_STENCIL_ATTACHMENT : GL_DEPTH_ATTACHMENT;
	//		ERR_PRINT_STRING( "%s() not implemented with this texture type %s", __FUNCTION__, aaa::c_pixel_format::get_pixel_type_name(pixel_type) );
			

			//c_img_2d*	p_img = g_bind_img_2d->get( _depth_bind_2d );
			auto pf = depth_pixel_format[_s_depth_format];
			c_img_2d*	p_img = g_bind_img_2d->get_with_image_size( _depth_bind_2d_out, _size_out[0],_size_out[1], pf );

	//todomona make sure we have an async version functionning
			if( p_img )
			{
				TBUF_INC_OBJ( tbuf::CH_GPU_READ, 1., "read_depth_from_gpu", this );
				bool b_done = false;
				if( b_pbo_use && _b_depth_on_cpu_use_pbo_ui )
				{
					SPY_PUSH_RANGE( "PBO Read Depth Pixels", spy::IMG );
											
						UINT64 size = p_img->get_data_size_used();	//not sure : the data can be bigger that its actual use, eg, after a realloc (we should have storage and image fns)
						_pbo_depth_bind[ 0 ] = GOL::pbo_alloc( _pbo_depth_bind[ 0 ], size, false );	
						_pbo_depth_bind[ 1 ] = GOL::pbo_alloc( _pbo_depth_bind[ 1 ], size, false );

						if( _pbo_depth_bind[ 0 ]!=-1 &&  _pbo_depth_bind[ 1 ]!=-1 )
						{
														// PBO readback
							//GOL::set_read_buffer( GL_DEPTH_ATTACHMENT );
								//todo deal with these
								//GL_STENCIL_ATTACHMENT
								//GL_DEPTH_STENCIL_ATTACHMENT: 
																	
							INT32	buffer_next = ( _pbo_depth_buffer_cur + 1 ) % 2;

							SPY_PUSH_RANGE( "Fbo Pbo Depth ReadPixels", spy::IMG );		
								// readback of current FBO in the next buffer
								GOL::pbo_do_bind( _pbo_depth_bind[ buffer_next ], false );
								if( !b_fbo_binded )
								{
									_frame_buffer_object->bind();
									b_fbo_binded = true;
								}
								GOL::read_pixels( 0,0, _size_out[0],_size_out[1], aaa::c_pixel_format::get_gl_format(pf, false), aaa::c_pixel_format::get_gl_type(pf), nullptr );
	//							GOL::get_tex_image_2d( 0,					format, aaa::c_pixel_format::get_gl_type(pixel_format), nullptr );		
								GOL::check_error_debug( "readpixels depth in fbo transfert cpu" );
							SPY_POP_RANGE();
						
							GOL::pbo_do_bind( _pbo_depth_bind[ _pbo_depth_buffer_cur ], false );
							SPY_PUSH_RANGE( "Fbo Pbo Depth MEMCPY", spy::IMG );
								void* src = (UINT8*) GOL::pbo_do_map( false );
								void* dst = p_img->get_data();					
								MEMCPY( dst, src, size, __FUNCTION__ );
								GOL::pbo_do_unmap( false );
							SPY_POP_RANGE();
							GOL::pbo_do_bind( -1, false );

							_pbo_depth_buffer_cur = buffer_next;
							p_img->set_changed();	//side fx trigger uneeded transfert back to GPU but not with set_gpu_move(false) before
							b_done = true;
						}
						else
						{
							debug_break( "non allocated pbo on depth attachment" );
						}
						
					SPY_POP_RANGE();
				}
				// blocking one used when no pbo or pbo failed
				if( !b_done )
				{
					if( !b_fbo_binded )
					{
						_frame_buffer_object->bind();
						b_fbo_binded = true;
					}
					SPY_PUSH_RANGE( "Read Pixels", spy::IMG );
						p_img->read_pixels( 0,0, _size_out[0],_size_out[1], GL_DEPTH_ATTACHMENT );
						//GOL::set_read_buffer( GL_COLOR_ATTACHMENT0 );
						//todo deal with these
						//GL_STENCIL_ATTACHMENT
						//GL_DEPTH_STENCIL_ATTACHMENT: 
					SPY_POP_RANGE();
					//detach_all();	//if we do nothing opengl will say incomplete at next use 2021 Sept Hack by M�a
					//attach();
				}
				//p_img->set_gpu_in_sync( _depth_bind_2d );	//avoid transfert CPU GPU
				TBUF_DEC_OBJ( tbuf::CH_GPU_READ, 1., "read_depth_from_gpu", this );
			}
//			GOL::print_errors( "transfer cpu" );
		}
#endif

		_frame_buffer_object->unbind();

		if( b_bind_restore )
			tex2d.bind( bind_store );

	SPY_POP_RANGE();

//todo fully investigate	
//was there just for a test
//	GOL::set_draw_buffer( GL_BACK );
//	GOL::bind_vao(0);
//	if( _depth_bind_slot )
//		_frame_buffer_object->attach_texture( GL_TEXTURE_2D, 0, _attachment_slot );
//	c_texturing::disable();
//	tex2d.bind( _bind_2d_dst[index] );
//	GOL::set_draw_buffer( GL_NONE );
//	GOL::set_read_buffer( GL_NONE );	//to make sure next call set it at low level

	GOL::check_error_debug( "after c_fbo::leave()" );
}

//void CopyDepthBuffer( GLuint texId, INT32 x, INT32 y, INT32 imageWidth, INT32 imageHeight )
//{
//	GOL::bind_texture_2d( texId );
//
//	glReadBuffer(GL_BACK); // Ensure we are reading from the back buffer.
//	glCopyTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, x,y, imageWidth,imageHeight, 0);
//}

FINLINE	bool	c_fbo::is_attachment_asked() CONST
{
	if( _b_depth_use_ui || _b_stencil_use_ui )
		return true;
	for( INT32 i = 0; i < attachment_color_nb; ++i )
	{
		if( _attach_info[i]._b_active_ui )
			return true;
	}
	return false;
}

void c_fbo::update()
{
	if( !this )
	{
		debug_break( "fbo::update() call with an empty object" );
		return;
	}

	_b_attach_out = false;
	if( !b_allow )
		return;

	if( !is_active() || !is_attachment_asked() )
		_b_ready_to_use = false;
	else
	{
		bool b_attach_needed;
		// create fbo
		if( _frame_buffer_object == nullptr )
		{
			_frame_buffer_object = new c_frame_buffer_object;
			if( !_frame_buffer_object )
			{
				err_print( "%s() FBO : failed to create a c_frame_buffer_object", __FUNCTION__ );
				return;
			}
			_fbo_id_gl_name_used = _frame_buffer_object->get_fbo_id();
			b_attach_needed = true;
		}
		else
			b_attach_needed = false;

		if( update_check_param() ) // do we need to change the fbo
			b_attach_needed = true;

		if( b_attach_needed )
		{
			if( cur )
				cur->leave();
			_frame_buffer_object->bind();
			_b_valid = false;
			detach_all();
			attach();
			//we always leave this unbind
			c_frame_buffer_object::unbind();
		}

		//	GOL::set_read_buffer( GL_NONE );	//to make sure next call set it at low level
		_b_ready_to_use	= _b_valid;
	}
}

void c_fbo::draw()
{
	if( _b_ready_to_use )
	{
		set_cur( this );
		if( _frame_buffer_object->bind_if_valid() )
		{
			GOL::check_error_debug( "c_fbo::update_low()" );
			_fbo_id_gl_name_used = _frame_buffer_object->get_fbo_id();
			viewport_fbo->set_rect_do( 0,0, _size_out[0],_size_out[1] );
		}
	}
	else
	{
		if( !_b_valid )
			err_print( "%s() FBO id %d : not valid can't draw it", __FUNCTION__, _fbo_id_gl_name_used );
	}
}

void	c_fbo::clear()
{
	if( !is_active() )
		return;
	_bdd_clear_screen->update();
	_bdd_clear_screen->draw();
}

void	c_fbo::set_cur( c_fbo* in )
{
//test
	//INT32	val		= 0x7fffffff;
	//val = aaa::BIGGEST<INT32>;
	//REAL test		= REAL(val);

	//UINT32 uval	= 0xffffffff;
	//uval = aaa::BIGGEST<UINT32>;
	//REAL utest	= REAL(uval);

	if( in == cur )
	{
#if AAA_DEBUG()
		if( in )
			DBG_PRINT_STRING( "%s() todo temp message we should just call leave infact (Maa 2022 August)", __FUNCTION__ );
#endif
	}
	else
	{
		if( cur )
			cur->leave();
		set_prev( cur );	//todo push/pop
		cur = in;
		if( !in )
			disable();
	}
}

