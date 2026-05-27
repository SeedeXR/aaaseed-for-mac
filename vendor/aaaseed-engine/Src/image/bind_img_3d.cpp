#include "bind_img_3d.h"
#include "bind_img.h"
#include "draw/tex.h"


UINT32	c_bind_img_3d::bank_nb_ui		{4};
UINT32	c_bind_img_3d::bank_size_ui		{32};


c_bind_img_3d* g_bind_img_3d	= nullptr;

void	bind_img_3d_init()
{
	g_bind_img_3d = new c_bind_img_3d( "Texture 3d Binds", "tex3d_bind",  c_bind_img_3d::bank_nb_ui, c_bind_img_3d::bank_size_ui );
	if( IS_NULL( g_bind_img_3d ) )
	{
		debug_break( __FUNCTION__ );
	}
}
void	bind_img_3d_deinit()
{
	SAFE_DELETE( g_bind_img_3d );
} 


FINLINE	INT32	c_bind_img_3d::get_cur_index()	CONST	{ return	tex_3d_get_bind_cur();	}
FINLINE	INT32	c_bind_img_3d::get_ui_index()	CONST	{ return	tex_3d_get_bind_ui();	}

/*! \todo this kind of things should be part of a better img_bind struct */
c_img_3d*	c_bind_img_3d::get_img( INT32 CONST index,
									aaa::PIXEL_FORMAT CONST format, INT32 sx, INT32 sy, INT32 sz, bool b_min,
									FP32 CONST * CONST rgba, C_PCHAR_C signature )
{
	GOL::adjust_tex_size_to_valid( sx, sy, sz );

	c_img_3d*	img = get( index );	//hack was bind_img_get_ready
									//todonow
	if( IS_NULL( img ) )
	{
		img = get_ready( index );	//hack this to make sure it's loaded once at least
		if( IS_NULL( img ) )	// still
		{
			DBG_PRINT_STRING( "%s() create new image %d", __FUNCTION__, index );
			img = c_img_3d::create( __FUNCTION__, index );
			if( IS_NULL( img ) )
				return nullptr;
			set( index, img );
			goto recreate_no_message;
		}
	}

	if( img->get_pixel_format() != format )
		goto recreate;
	if( b_min )
	{
		if( img->get_size_x() != sx || img->get_size_y() != sy || img->get_size_z() != sz )
			goto recreate;
	}
	else
	{
		if( img->get_size_x() < sx || img->get_size_y() < sy || img->get_size_z() < sz )
			goto recreate;
		//in case we switch from npot
		if( !GOL::is_tex_npot()
			&& ( !INT_IS_POW2(img->get_size_x()) || !INT_IS_POW2(img->get_size_y()) || !INT_IS_POW2(img->get_size_z()) ) 
			)
		{
			goto recreate;
		}
	}
	return img;

recreate:
	DBG_PRINT_STRING( "%s() recreate image3d %d : %d x %d x %d with %s", signature, index, sx,sy,sz, aaa::c_pixel_format::get_name( format ) );
recreate_no_message:
	if( NOERR(img->init_with_size( sx,sy,sz, format, signature )) )
	{
		img->set_cpu_keep( true );
		if( rgba )
			img->fill_rgba( rgba );
	//	GOL::pbo_do_bind( -1, true );
		//GOL::pbo_do_bind( -1, img->get_mem_type() != c_img_2d::PBO_APP_READ );
	//	GOL::pbo_do_bind( -1 );	//maa	this line and the had to be put back
	//just a test	img->set_pbo_bind( -1 );
	//	img->move_to_gpu();
	}
	return img;
}

c_img_3d*	c_bind_img_3d::get_img_cur( aaa::PIXEL_FORMAT CONST format, INT32 CONST sx, INT32 CONST sy, INT32 CONST sz, bool CONST b_min, FP32 CONST * CONST rgba, C_PCHAR_C signature )
{
	INT32	s_bind = get_cur_index();
	//tex_3d_bind( s_bind );
	return get_img( s_bind, format, sx, sy, sz, b_min, rgba, signature );
}

void	c_bind_img_3d::do_after_a_compute( bool b_force_nearest )
{
	INT32	bind	=	get_cur_index();
	c_img_3d*	img	=	get( bind );
	img->erase_filename();
	img->set_changed();
	img->set_cpu_keep( true );
	GOL::get_tex_unit_cur()->set_bind_3d(-2);
	//GOL::invalidate_tex_unit();
	if( b_force_nearest && !tex3d.is_force_nearest() )
	{
		tex3d.set_force_nearest( true );
			tex_3d_bind( bind );
		tex3d.set_force_nearest( false );
	}
	else
		tex_3d_bind( bind );
}

void	c_bind_img_3d::init()
{
	alloc();
	tex3d.alloc( _bind_max_nb );
}

//todo	all this is dirty
void	c_bind_img_3d::move_to_gpu( INT32 start, INT32 nb )
{
	if( nb == 1 )
	{
		//todo have a specific verbose flag for this
		//GOOD_PRINT_STRING( "bind %d Loading texture on GPU.", start );
		tex_3d_bind( start );
	}
	else
	{
		GOOD_PRINT_STRING( "From bind 3d %d Loading %d textures on GPU :", start, nb );
		GOOD_PRINT_STRING_NO_CR( "" );
		for( INT32 i = start; i < start + nb; ++i )
		{
			PRINT_STRING( "." );
			tex_3d_bind( i );
		}
		PRINT_STRING( "\n" );
	}
}





