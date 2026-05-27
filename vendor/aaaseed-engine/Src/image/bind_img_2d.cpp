#include "bind_img_2d.h"
#include "bind_img.h"
#include "draw/tex.h"
#include "asc_line.h"
#include "fbo/fbo.h"
#include "spy.h"

namespace {
	CONSTEXPR INT32 BANK_NB_UI		= 20;
	CONSTEXPR INT32 BANK_SIZE_UI	= 32;
};

UINT32	c_bind_img_2d::bank_nb_ui	{BANK_NB_UI};	// bank 16 and 17 are reserved for fonts, 18 and 19 for objects
													// this nb is set up in preference at start param bank_2d_nb
UINT32	c_bind_img_2d::bank_size_ui	{BANK_SIZE_UI};

c_bind_img_2d* g_bind_img_2d = nullptr;

void	bind_img_2d_init()
{
	g_bind_img_2d = new c_bind_img_2d( "Texture Binds", "image_bind", c_bind_img_2d::bank_nb_ui, c_bind_img_2d::bank_size_ui );
	if( IS_NULL( g_bind_img_2d ) )
		debug_break( __FUNCTION__ );
}
void	bind_img_2d_deinit()
{
	SAFE_DELETE( g_bind_img_2d );
}
														  
FINLINE	INT32	c_bind_img_2d::get_cur_index()	CONST	{ return tex_2d_get_bind_cur();	}
FINLINE	INT32	c_bind_img_2d::get_ui_index()	CONST	{ return tex_2d_get_bind_ui();	}

INT32	c_bind_img_2d::make_tex_index(		INT32 bank,  INT32 bind )
{
	if( bank == 6 )
	{	//we map the previous fbo attachment in this case 
		INT32 bind_mapped = c_fbo::get_attachment_bind_prev( bind );
		if( bind_mapped >= 0 )
			return bind_mapped;
	}
	return bank * _bank_size + bind;
}

namespace {
	//todo move  to fbo
	CONSTEXPR C_PCHAR_C str_tex_remap[] = {
		"Fbo Prev Color 0", "Fbo Prev Color 1", "Fbo Prev Color 2", "Fbo Prev Color 3",
		"Fbo Prev Color 4", "Fbo Prev Color 5", "Fbo Prev Color 6", "Fbo Prev Color 7",
		nullptr,			nullptr,			nullptr,			nullptr,
		nullptr,			nullptr,			nullptr,			nullptr,
		"Fbo Prev Depth",	"Fbo Prev Stencil"
	};
}

CHAR CONST *	c_bind_img_2d::get_tex_index_str(		INT32 bank,  INT32 bind )
{
	if( bank == 6 && INSIDE_MIN_MAX( bind, 0, 17 ) )
		return str_tex_remap[bind];
	return nullptr;
}


void	c_bind_img_2d::init()
{
	alloc();
	tex2d.alloc( _bind_max_nb );

	//opencl	Maa suppress this
	//todo remove solve and in 3d also
	/*
	INT32	unpack_ali	= GOL::get_integer( GL_UNPACK_ALIGNMENT );
	INT32	pack_ali	= GOL::get_integer( GL_PACK_ALIGNMENT );

	GOL::set_pixel_storei( GL_UNPACK_ALIGNMENT, 1 );
	GOOD_PRINT_STRING( "changed GL_UNPACK_ALIGNMENT from %d to %d", unpack_ali, GOL::get_integer( GL_UNPACK_ALIGNMENT ) );
	GOL::set_pixel_storei( GL_PACK_ALIGNMENT, 1 );
	GOOD_PRINT_STRING( "changed GL_PACK_ALIGNMENT from %d to %d", pack_ali, GOL::get_integer( GL_PACK_ALIGNMENT ) );
	*/

	//	that's the default
	//	GOL::set_pixel_storei( GL_UNPACK_SWAP_BYTES, 0 );
	//	GOL::set_pixel_storei( GL_PACK_SWAP_BYTES, 0 );
}


/*	not used anywhere
void makeStripeImage()
{
	for( INT32 j = 0; j < stripeImageWidth; ++j )
	{
		stripeImage[ 3*j     ] = ( j<=4 ) ? 255 : 0;
		stripeImage[ 3*j + 1 ] = ( j >4 ) ? 255 : 0;
		stripeImage[ 3*j + 2 ] = 0;
	}
}
*/

// todo never used, use doxygen style here
REAL	c_bind_img_2d::get_value_max_from_rect_xy( INT32 index, INT32 x_begin, INT32 x_end, INT32 y_begin, INT32 y_end, aaa::COMPO what )
{
	c_img_2d*	p_img = get_ready( index );
	if( p_img )
		return p_img->get_value_max_from_rect( x_begin, x_end, y_begin, y_end, what );
	else
		return -1.0f;
}

//todo regroup with next
c_img_2d*	c_bind_img_2d::get_img_ignore_bgr(	INT32 CONST index,
												aaa::PIXEL_FORMAT CONST format, INT32 sx, INT32 sy, bool CONST b_min,
												FP32 CONST * CONST rgba, C_PCHAR_C signature )
{
	GOL::adjust_tex_size_to_valid( sx,sy );

	c_img_2d*	p_img = get( index );	//hack was bind_img_get_ready
								//todonow
	if( IS_NULL( p_img ) )
	{
		p_img = get_ready( index );	//hack this to make sure it's loaded once at least
		if( IS_NULL( p_img ) )	// still
		{
			DBG_PRINT_STRING( "%s() create new image %d", __FUNCTION__, index );
			p_img = c_img_2d::create( __FUNCTION__, index );
			if( IS_NULL( p_img ) )
				return nullptr;
			set( index, p_img );
			goto recreate_no_message;
		}
	}

	if( !p_img->is_format_ignore_bgr( format ) )
		goto recreate;
	if( b_min )
	{
		if( !p_img->is_size( sx,sy ) )
			goto recreate;
	}
	else
	{
		if( p_img->get_size_x() < sx || p_img->get_size_y() < sy )
			goto recreate;
		//in case we switch from npot
		if( !GOL::is_tex_npot()
			&& ( !INT_IS_POW2(p_img->get_size_x()) || !INT_IS_POW2(p_img->get_size_y()) ) 
			)
			goto recreate;
	}
	return p_img;

recreate:
	DBG_PRINT_STRING( "%s() recreate image2d %d : %d x %d with %s", signature, index, sx,sy, aaa::c_pixel_format::get_name( format ) );
recreate_no_message:
	if( NOERR(p_img->init_with_size( sx,sy, format, signature )) )
	{
		p_img->set_cpu_keep( true );
		if( rgba )
			p_img->fill_rgba( rgba );
	//	GOL::pbo_do_bind( -1, true );
		//GOL::pbo_do_bind( -1, img->get_mem_type() != c_img_2d::PBO_APP_READ );
	//	GOL::pbo_do_bind( -1 );	//maa	this line and the had to be put back
	//just a test	img->set_pbo_bind( -1 );
	//	img->move_to_gpu();
	}
	return p_img;
}

//todo this kind of things should be part of a better img_bind struct
c_img_2d*	c_bind_img_2d::get_img( INT32 CONST index,
									aaa::PIXEL_FORMAT CONST format, INT32 sx, INT32 sy, bool CONST b_min,
									FP32 CONST * CONST rgba, C_PCHAR_C signature )
{
	GOL::adjust_tex_size_to_valid( sx,sy );

	c_img_2d*	p_img = get( index );	//hack was bind_img_get_ready
								//todonow
	if( IS_NULL( p_img ) )
	{
		p_img = get_ready( index );	//hack this to make sure it's loaded once at least
		if( IS_NULL( p_img ) )	// still
		{
			DBG_PRINT_STRING( "%s() create new image %d", __FUNCTION__, index );
			p_img = c_img_2d::create( __FUNCTION__, index );
			if( IS_NULL( p_img ) )
				return nullptr;
			set( index, p_img );
			goto recreate_no_message;
		}
	}

	if( !p_img->is_format( format ) )
		goto recreate;
	if( b_min )
	{
		if( !p_img->is_size( sx,sy ) )
			goto recreate;
	}
	else
	{
		if( p_img->get_size_x() < sx || p_img->get_size_y() < sy )
			goto recreate;
		//in case we switch from npot
		if( !GOL::is_tex_npot()
			&& ( !INT_IS_POW2(p_img->get_size_x()) || !INT_IS_POW2(p_img->get_size_y()) ) 
			)
			goto recreate;
	}
	return p_img;

recreate:
	DBG_PRINT_STRING( "%s() recreate image2d %d : %d x %d with %s", signature, index, sx,sy, aaa::c_pixel_format::get_name( format ) );
recreate_no_message:
	if( NOERR(p_img->init_with_size( sx,sy, format, signature )) )
	{
		p_img->set_cpu_keep( true );
		if( rgba )
			p_img->fill_rgba( rgba );

	//	GOL::pbo_do_bind( -1, true );
		//GOL::pbo_do_bind( -1, p_img->get_mem_type() != c_img_2d::PBO_APP_READ );
	//	GOL::pbo_do_bind( -1 );	//maa	this line and the had to be put back
	//just a test	img->set_pbo_bind( -1 );
	//	p_img->move_to_gpu();
	}
	return p_img;
}

c_img_2d*	c_bind_img_2d::get_img_cur( aaa::PIXEL_FORMAT CONST format, INT32 CONST sx, INT32 CONST sy, bool CONST b_min,
										FP32 CONST * rgba, C_PCHAR_C signature )
{
	INT32	s_bind = get_cur_index();
	//tex_2d_bind( s_bind );
	return get_img( s_bind, format, sx,sy, b_min, rgba, signature );
}

void	c_bind_img_2d::do_after_a_compute( bool b_force_nearest )
{
	INT32	bind	=	get_cur_index();
	c_img_2d* p_img	=	get( bind );
	p_img->erase_filename();
	p_img->set_changed();
	p_img->set_cpu_keep( true );
	GOL::get_tex_unit_cur()->set_bind_2d(-2);
	//GOL::invalidate_tex_unit();
	//todo have to understand this one. see if we can use tex_2d_bind_no_gpu_move
	if( b_force_nearest && !tex2d.is_force_nearest() )
	{
		tex2d.set_force_nearest( true );
		tex_2d_bind( bind );
		tex2d.set_force_nearest( false );
	}
	else
		tex_2d_bind( bind );
}

//todoqq	perhaps not always valid ?
bool	c_bind_img_2d::is_size_cur( INT32& size_x, INT32& size_y )
{
	c_img_2d*	p_img = get_cur();
	if( p_img )
	{
		size_x = p_img->get_size_x();
		size_y = p_img->get_size_y();
		return true;
	}
	size_x = 0;
	size_y = 0;
	return false;
}

c_img_2d*	c_bind_img_2d::get_with_image_size( INT32 index, INT32 sx, INT32 sy, aaa::PIXEL_FORMAT format )
{
	c_img_2d** img_hd = get_hd( index );
	if( img_hd )
	{
		*img_hd	= c_img_2d::img_init_with_size( *img_hd, sx,sy, format, __FUNCTION__ );
		return *img_hd;
	}
	return nullptr;
}

void c_bind_img_2d::init_img_from_tex( INT32 CONST dst_bind, INT32 CONST src_bind, INT32 sx, INT32 sy, C_PCHAR_C str_verbose )
{
	SPY_PUSH_RANGE( "init_img_from_tex", spy::GOL_LOW );

		//tex2d.bind( src_bind );
		//GOL::make_tex_complete_2d( gl_bind_src );

		GOL::INTERNAL_TYPE	src_type	= tex2d.get_internal_type( src_bind );
		INT32				ch_nb		= tex2d.get_channel_nb( src_bind );
		aaa::PIXEL_FORMAT	pf			= aaa::c_pixel_format::make_format_from_channel_gltype( ch_nb, src_type );

		if( str_verbose )
		{
			VERBOSE_PRINT_STRING( "%s : redefine texture dst %d from src %d", str_verbose, dst_bind, src_bind );
			VERBOSE_PRINT_STRING( "%s : %d x %d %.64s", str_verbose, sx,sy, aaa::c_pixel_format::get_name(pf) );
		}

		// we need to change the img or img will take over tex later
		c_img_2d* img_dst = get_always( dst_bind );
		img_dst->init_with_size_no_cpu_mem( sx,sy, pf, __FUNCTION__ );
		img_dst->set_gpu_move( false );

		// We need to create texture too
		tex2d.bind( dst_bind );
		tex2d.image_level( 0, sx,sy, ch_nb, src_type, nullptr, aaa::c_pixel_format::get_gl_format(pf,false), aaa::c_pixel_format::get_gl_type(pf) );
		//GOL::set_tex_2d_max_level(0);	//if we don't do this we get in troubles later for texture completeness
		// perhaps we should do more later
		//GOL::make_tex_complete_2d( gl_bind_dst);

	SPY_POP_RANGE();
}

void c_bind_img_2d::copy_tex_to_tex( INT32 CONST src_bind, INT32 CONST dst_bind, bool b_mipmap_generate, bool const b_dst_set_format )
{
	if( !GOL::is_copy_image_sub_data_2d() )
	{
		err_print( "%s() we can't use copy_image_sub_data_2d() : skip copy", __FUNCTION__ );
		return;
	}	
	GLenum	src_format = tex2d.get_internal_format( src_bind );
	if( !src_format ) 
	{
		err_print( "%s() texture source %d have no format : skip copy", __FUNCTION__, src_bind );
		return;
	}

	INT32 src_sx;
	INT32 src_sy;
	tex2d.get_size( src_bind, src_sx,src_sy );

	GLenum dst_format = tex2d.get_internal_format( dst_bind );
	bool b_change = dst_format != src_format;
	if( !b_change )
	{
		INT32 dst_sx;
		INT32 dst_sy;
		tex2d.get_size( dst_bind, dst_sx,dst_sy );
		b_change = dst_sx != src_sx || dst_sy != src_sy;
	}

	GLuint gl_bind_src = tex2d.get_name_gl( src_bind );
	GLuint gl_bind_dst = tex2d.get_name_gl( dst_bind );
	INT32 bind_store = -1;

	if( b_change && b_dst_set_format )
	{
		bind_store = tex2d.get_index();
		init_img_from_tex( dst_bind, src_bind, src_sx, src_sy, "c_bind_img_2d::copy_tex_to_tex()" );
	}
	

	//todo check completeness here
	// 
	//todo copy all level
	//todo we hide params which could be used
	//todo don't execute when under opengl 4.3
	GOL::copy_image_sub_data_2d(	gl_bind_src, 0,
									gl_bind_dst, 0,
									src_sx, src_sy );
	GOL::check_error_debug( "copy_image_sub_data_2d" );

	b_mipmap_generate &= tex2d.is_mipmap_generate();
	//todo 2022 Juillet still needed to be regrouped
	//todo this is not clean it is asymetric: if we call generate_mipmap_texture we don't set texture wraps and filter
	//  2021 Nov this is just a try
	if( b_mipmap_generate )
	{	//2022 July MEU React Dif failed when b_direct_state_access_use is false here
		if( GOL::b_direct_state_access_use )
		{
			GOL::generate_mipmap_texture_direct( gl_bind_dst );
		}
		else
		{
			if( bind_store == -1 )
			{
				bind_store = tex2d.get_index();
				tex2d.bind( dst_bind );
			}
			//todo generate could be here but filtering ? anyhow all this will change with sampler		
			GOL::generate_mipmap_2d_direct();
			//	tex2d.adjust_filtering( true );
			//		INT32 mode = GL_LINEAR;
			//		GOL::set_tex_2d_min_mag( mode, mode );
			//	}
		}
	}

	if( bind_store >= 0 )
		tex2d.bind( bind_store );
}

//todo	all this is dirty
void	c_bind_img_2d::move_to_gpu( INT32 start, INT32 nb )
{
	if( nb == 1 )
	{
		//todo have a specific verbose flag for this
		//GOOD_PRINT_STRING( "bind %d Loading texture on GPU.", start );
		tex_2d_bind( start );
	}
	else
	{
		GOOD_PRINT_STRING( "From bind 2d %d Loading %d textures on GPU :", start, nb );
		GOOD_PRINT_STRING_NO_CR( "" );
		for( INT32 i = start; i < start + nb; ++i )
		{
			PRINT_STRING( "." );
			tex_2d_bind( i );
		}
		PRINT_STRING( "\n" );
	}
}

AAA_ERR	c_bind_img_2d::save( INT32 index, C_PCHAR_C filename, c_img_utils::FILE_TYPE type, bool b_verbose )
{
	c_img_2d* img = get_image_data( index );
	if( !img->is_valid() )
	{
		ERR_PRINT_STRING( "no image in bind %d to save as %s", index, filename );
		return ERR_OBJ_NULL;
	}

	C_PCHAR filename_used = IS_NOT_NULL( filename ) ? filename : "snapshot";
	AAA_ERR	retcode = img->write( filename_used, type ); 

	if( NOERR( retcode ) )
		GOOD_PRINT_STRING(	"Saved image of texture %d as %s",			index, filename_used );
	else
		ERR_PRINT_STRING(	"Failed to save image in bind %d as %s",	index, filename_used );
	return retcode;
}


void	bank_2d_move_to_gpu( INT32 index_bank )
{
	g_bind_img_2d->move_to_gpu( g_bind_img_2d->make_tex_index( index_bank, 0 ), g_bind_img_2d->get_bank_size() );
}

void	bank_2d_move_to_gpu_current()
{
	bank_2d_move_to_gpu( tex_2d_get_bind_ui() );
}

void	bank_2d_move_to_gpu_all()	//todoqqq add a start stop in the interface and deal by bank
{
	DBG_PRINT_STRING( "bank_move_to_gpu_all() Started" );
	g_bind_img_2d->move_to_gpu( 0, g_bind_img_2d->get_bind_max_nb() );
	DBG_PRINT_STRING( "bank_move_to_gpu_all() Done" );
}

AAA_ERR	bank_2d_load_at_bind( o_str CONST & filename_in, INT32 index )
{
	AAA_ERR	retcode = ERR_ANY;
	FILE* file = c_file::FOPEN( filename_in, "rt" );
	if( IS_NULL( file ) )
	{
		c_file::print_err( __FUNCTION__, filename_in.get(), "don't exist" );
		retcode = ERR_FILE_NO;
	}
	else
	{
		o_str& img_filename = o_str::push_name();
		o_str& o			= o_str::push_name();
			o.erase();
			o.alloc( AAA_PATH_MAX()*2+1 );
			INT32	nb;
	//		if( b_verbose_param )
	//			VERBOSE_PRINTF( "Parsing %s : ", filename );
			do
			{
				nb = asc_line::get_next_line_no_empty( file, o.get_changeable(), AAA_PATH_MAX()*2 );
				if( nb <= 0 )
				{
					if( nb == EOF )
						retcode = AAA_OK;
				}
				else
				{
					//o.sync();
					g_bind_img_2d->destroy( index );
					//	place a relative path in the filename
					img_filename.set_fname_relative( o.get_changeable() );
					g_bind_img_2d->get_bind()->set_item( index, img_filename );
					index = g_bind_img_2d->get_index_valid( index + 1 );
					//o.erase();
				}
			}
			while( nb >=0 );
		o_str::pop_name();
		o_str::pop_name();
		c_file::FCLOSE( file );
	}
//	img_bind::list_fix->load_do_after( nullptr );
	return retcode;
}

AAA_ERR	bank_2d_load_at_bank(	  o_str CONST & filename_in, INT32 index_bank )	{ return bank_2d_load_at_bind(	   filename_in, g_bind_img_2d->make_tex_index( index_bank, 0 ) );	}
AAA_ERR	bank_2d_load_in_bank_cur( o_str CONST & filename_in )					{ return bank_2d_load_at_bank(	   filename_in, c_layer::get_ui() ? c_layer::get_ui()->get_bank_2d() : 0 );		}
AAA_ERR	bank_2d_load_at_bind_cur( o_str CONST & filename_in )					{ return bank_2d_load_at_bind(	   filename_in, c_layer::get_ui() ? c_layer::get_ui()->get_bind_2d() : 0 );		}
																				  
AAA_ERR	bank_2d_save(			  o_str CONST & filename_in, INT32 index_bank )	{ return g_bind_img_2d->bank_save( filename_in, g_bind_img_2d->make_tex_index( index_bank, 0 ), g_bind_img_2d->get_bank_size() );	}
AAA_ERR	bank_2d_save_cur(		  o_str CONST & filename_in )					{ return bank_2d_save(			   filename_in, c_layer::get_ui() ? c_layer::get_ui()->get_bank_2d() : 0 );		}
AAA_ERR	bank_2d_save_all(		  o_str CONST & filename_in )					{ return g_bind_img_2d->bank_save( filename_in, 0, g_bind_img_2d->get_bind_max_nb() );	}


