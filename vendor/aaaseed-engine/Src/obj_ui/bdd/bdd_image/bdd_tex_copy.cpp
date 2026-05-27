
#include "bdd_tex_copy.h"
#include "gol/gol.h"
#include "image/bind_img.h"
#include "image/bind_img_2d.h"
#include "infrastructure/viewport.h"
#include "fbo/fbo.h"
#include "draw/tex.h"
#include "spy.h"


//todo add clear

FACTORY_CREATE_PROP_V1( c_bdd_tex_copy, bdd_tex_copy, Texture Copy, bdd_tex_copy, sub_menu="Image"; );

namespace
{
	C_PCHAR_C	str_size_grab[2] =
	{
//		"Power of 2",
		"VIEW_WIDTH",
		"VIEW_HEIGHT"
	};
}

namespace n_bdd_tex_copy
{
	CONSTEXPR INT32 BASE_NB_MAX		=	c_bdd::GEO_PARAM_NB + 11;
//	CONSTEXPR INT32 GRAB_NB_MAX	=	0;
	CONSTEXPR INT32 GRAB_NB_MAX		=	11;
	CONSTEXPR INT32 DEBUG_NB_MAX	=	3;
	CONSTEXPR INT32 GROUP_NB_MAX	=	2;
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_NB_MAX
									+	GRAB_NB_MAX
									+	DEBUG_NB_MAX
									+	GROUP_NB_MAX;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		BDD_BASE_PARAMS

		PARAM_DEF_BOOL_ON(		c		olor_attachment					)
		PARAM_DEF_INT32(				color_attachment_index,			1, 0,		0, GOL::FBO_COLOR_ATTACHMENT_NB_MAX-1	)
		PARAM_DEF_BIND_2D_CURRENT_SEL(	src_bind_2d							)
		PARAM_DEF_BANK_BIND_2D_OUT(		src_bind_2d_out						)

		PARAM_DEF_BIND_2D_CURRENT_SEL(	dst_bind_2d							)
		PARAM_DEF_BANK_BIND_2D_OUT(		dst_bind_2d_out						)
//		PARAM_DEF_BOOL_OFF(		strobe_active							)
//		PARAM_DEF_REAL(			strobe_rate,							1, 10,		0, 1000	)

		PARAM_DEF_BOOL_ON(		copy_tex_to_tex )
		PARAM_DEF_BOOL_OFF(		mipmap_generate )
		PARAM_DEF_BOOL_ON(		smooth )

		PARAM_DEF_BOOL_ON(		grab_to_gpu )
		PARAM_DEF_BOOL_OFF(		grab_to_cpu )

		PARAM_DEF_GROUP( Grab, GRAB_NB_MAX )
			PARAM_DEF_SYMBO_NEG(		size_x,			-2, -1,		-2, PARAM_MAX_INT32,			str_size_grab	)
			PARAM_DEF_SYMBO_NEG(		size_y,			-1, -2,		-2, PARAM_MAX_INT32,			str_size_grab	)
			PARAM_DEF_REAL_POS_ONE(		size_factor		)
			PARAM_DEF_INT32_XY(			size_offset,	1, 0,		PARAM_MIN_INT32, PARAM_MAX_INT32 )	
			PARAM_DEF_INT32_LOCKED_XY(	size_used		)

			PARAM_DEF_UV_LOCKED(		tex_size	)
			PARAM_DEF_POINT_XY(			pos			)
//			PARAM_DEF_BOOL_ON(			grab_alpha	)

		PARAM_DEF_GROUP( TEK, DEBUG_NB_MAX )
			PARAM_DEF_BOOL_ON(		dst_format_force	)
			PARAM_DEF_BOOL_OFF(		verbose				)
			PARAM_DEF_BOOL_OFF(		check_gl			)
	};
}


CONSTRUCTOR_CREATE( c_bdd_tex_copy )
	,_src_bind_out(0)
	,_dst_bind_out(0)
{
	param_init_with( n_bdd_tex_copy::param, n_bdd_tex_copy::PARAM_NB_MAX );
	init();
}

void c_bdd_tex_copy::init()
{
//	_strobe_count = 0;
	_tex_size[0] = 0;
	_tex_size[1] = 0;	
}

EMPTY_DESTRUCTOR(c_bdd_tex_copy)


void c_bdd_tex_copy::param_init_pt()
{
	INT32	h = param_init_pt_geo();

	param_set_pt(		h, _b_src_color_attachment_ui	);
	param_set_pt(		h, _s_src_color_attachment_ui	);
	param_set_pt(		h, _src_bind_ui					);
	param_set_pt(		h, _src_bind_out				);

	param_set_pt(		h, _dst_bind_ui					);
	param_set_pt(		h, _dst_bind_out				);

//	param_set_pt(		h, _b_strobe_active_ui	);
//	param_set_pt(		h, _strobe_freq_ui		);
	param_set_pt(		h, _b_tex_to_tex		);
	get_param(h)->set_comment( "ON should have no effects" );
	param_set_pt(		h, _b_mipmap_generate_ui );
	param_set_pt(		h, _b_smooth_ui );
//	param_set_pt(		h, _b_grab_alpha_ui	);

	param_set_pt(		h, _b_grab_to_gpu_ui	);
	param_set_pt(		h, _b_grab_to_cpu_ui	);

	++h;	//Grab to tex
		param_set_pt_2(	h, _size_ui			);
		param_set_pt(		h, _size_factor_ui	);
		param_set_pt_2(	h, _size_offset_ui	);
		param_set_pt_2(	h, _size			);
		param_set_pt_2(	h, _tex_size		);
		param_set_pt_2(	h, _grab_pos		);

	++h;
		param_set_pt(		h, _b_dst_format_force_src_ui	);
		param_set_pt(		h, _b_verbose_ui				);
		param_set_pt(		h, _b_check_gl_ui				);

	err_param_init_pt( h );
}

void c_bdd_tex_copy::update()
{
//		c_model::cur->get_size_v3( _size );
	if( _b_src_color_attachment_ui )
	{
		c_fbo* fbo = c_fbo::get_cur();
		if( !fbo )
		{
			err_print( "%s no current fbo : skip grab", __FUNCTION__ );
			_src_bind_out = -1;
		}
		else
			_src_bind_out = fbo->get_color_attachment_bind( _s_src_color_attachment_ui );
	}
	else
		_src_bind_out = g_bind_img_2d->build_index_from_param( _src_bind_ui );

	_dst_bind_out = g_bind_img_2d->build_index_from_param( _dst_bind_ui );

	if( _b_grab_to_gpu_ui )
	{
		c_viewport*	viewport	=	c_viewport::get_cur();

		if( _size_ui[0] == -1 )	
			_size[0] = viewport->get_sx();
		else if( _size_ui[0] == -2 )
			_size[0] = viewport->get_sy();
		else
			_size[0] = _size_ui[0];

		if( _size_ui[1] == -1 )
			_size[1] = viewport->get_sx();
		else if( _size_ui[1] == -2 )
			_size[1] = viewport->get_sy();
		else							
			_size[1] = _size_ui[1];			

		_size[0] = I_FLOOR( _size[0] * _size_factor_ui ) + _size_offset_ui[0];
		_size[1] = I_FLOOR( _size[1] * _size_factor_ui ) + _size_offset_ui[1];

		GOL::adjust_tex_size_to_valid( _size[0], _size[1] );
	}
 }

void c_bdd_tex_copy::grab_to_tex( INT32 CONST src_bind )
{
	GLenum				src_format = tex2d.get_internal_format( src_bind );
	if( !src_format ) 
	{
		err_print( "%s() no format for attachment %d : skip copy", __FUNCTION__, _s_src_color_attachment_ui );
		return;
	}
	INT32				src_sx;
	INT32				src_sy;
	tex2d.get_size( src_bind, src_sx,src_sy );

//	c_tex_info_2d*	dst_info = tex2d.get_info( _s_grab_dst_ui );
	GLenum				dst_format = tex2d.get_internal_format( _dst_bind_out );
	INT32				dst_sx;
	INT32				dst_sy;
	tex2d.get_size( _dst_bind_out, dst_sx,dst_sy );

	//hack
//			c_color::cur->pixel_transfert_update();

/*		if( b_verbose_grab_ )
			VERBOSE_PRINTF( "BDD_FEEDBACK : grabbed %d x %d / size %d x %d", tex_w_grabed, tex_h_grabed, _size_x, _size_y );
*/

	bool b_sub;
	if( 	dst_sx >= _size[0]
		&&	dst_sy >= _size[1]
		&&	dst_format == src_format
		)
	{
		if( dst_sx != _size[0] || dst_sy != _size[1] )
			b_sub = false;
		else
			b_sub = true;
	}
	else
		b_sub = false;


	//todo	deal with overlap
	c_viewport*	viewport	=	c_viewport::get_cur();
	INT32 grab_x = viewport->get_x() + interpolate( 0, viewport->get_sx()-_size[0], _grab_pos[0]*.5 +.5 );
	INT32 grab_y = viewport->get_y() + interpolate( 0, viewport->get_sy()-_size[1], _grab_pos[1]*.5 +.5 );
		
	SPY_PUSH_RANGE( "bdd_tex_copy::copy_to_tex", spy::GOL_LOW );

		GOL::set_read_buffer( GL_COLOR_ATTACHMENT0 + _s_src_color_attachment_ui );
		if( _b_check_gl_ui )
			GOL::check_error( "set_read_buffer" );

//	GOL::push_attrib( GL_ALL_ATTRIB_BITS );
//	GOL::push_attrib( GL_TEXTURE_BIT );

		tex_2d_bind_no_gpu_move( _dst_bind_out );

		if( b_sub )
		{
			//todoq integrate tex_copy_sub_image_2d in tex
			//todoq deal with swizzle
			GOL::tex_copy_sub_image_2d( 0,
					0,0,
					grab_x, grab_y,
					_size[0], _size[1]
					);
			if( _b_check_gl_ui )
				GOL::check_error( "tex_2d_bind" );
		}
		else
		{
			g_bind_img_2d->init_img_from_tex( _dst_bind_out, src_bind, src_sx, src_sy, "c_bdd_tex_copy::grab_to_tex()" );

			//todoq deal with swizzle
			GOL::tex_copy_image_2d( 0, src_format,	grab_x, grab_y, _size[0], _size[1], 0 );
			if( _b_check_gl_ui )
				GOL::check_error( "tex_copy_image_2d" );
		}

	//hack
	//			c_color::cur->pixel_transfert_reset();
		if( _b_mipmap_generate_ui )
			GOL::generate_mipmap_2d();
		else
		//hack we should extend with mipmap generation
	//	if( b_smooth != b_smooth_ui )	
		{
	//		_b_smooth = _b_smooth_ui;
			INT32 mode = _b_smooth_ui ? GL_LINEAR : GL_NEAREST ;
			GOL::set_tex_2d_min_mag( mode, mode );
		}
		//	if( GOL::is_anisotropic_filtering() )
		//		GOL::enable_anisotropic_filtering_2d();

	SPY_POP_RANGE();

	_tex_size[0] = REAL( _size[0] ) / REAL( src_sx );
	_tex_size[1] = REAL( _size[1] ) / REAL( src_sy );
}

void c_bdd_tex_copy::grab_to_img( INT32 CONST src_bind )
{
	GOL::INTERNAL_TYPE	src_type		= tex2d.get_internal_type( src_bind );
	INT32				ch_nb			= tex2d.get_channel_nb( src_bind );
	aaa::PIXEL_FORMAT	pixel_format	= aaa::c_pixel_format::make_format_from_channel_gltype( ch_nb, src_type );

	//deal with pixel types
	c_img_2d* p_img = g_bind_img_2d->get_with_image_size( _dst_bind_out, _size[0], _size[1], pixel_format );
	if( p_img )
	{
		//p_img->lock();
		
		c_viewport*	viewport	=	c_viewport::get_cur();
		p_img->read_pixels(	viewport->get_x() + ( viewport->get_sx() - _size[0] ) / 2,
							viewport->get_y() + ( viewport->get_sy() - _size[1] ) / 2,
							_size[0], _size[1], GL_COLOR_ATTACHMENT0 + _s_src_color_attachment_ui );

		//p_img->unlock();
		if( !_b_grab_to_gpu_ui )
		{	//	to avoid a later transfert to texture
			p_img->set_gpu_in_sync( _dst_bind_out );
		}
	}
}

namespace {
//	bool	b_force_grab = true;
}

void	c_bdd_tex_copy::draw()
{
//	if( _b_grab_to_tex_ui && ( b_grab_needed || b_force_grab ) )
//	if( _strobe_count > _strobe_ui )
//	{
//		_strobe_count = 0;

	if( _src_bind_out >= 0 )
	{
		if( _b_tex_to_tex )
			g_bind_img_2d->copy_tex_to_tex( _src_bind_out, _dst_bind_out, _b_mipmap_generate_ui, _b_dst_format_force_src_ui );
		else if( _b_grab_to_cpu_ui )
			grab_to_img( _src_bind_out );
		else if( _b_grab_to_gpu_ui )
			grab_to_tex( _src_bind_out );
	}

}
