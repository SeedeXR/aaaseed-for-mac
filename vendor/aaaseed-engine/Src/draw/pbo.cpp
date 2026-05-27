
#include "pbo.h"
#include "image/img.h"
#include "gol/gol_pbo.h"
#include "tex.h"
#include "media/video/texture_flux_master.h"
#include "image/bind_img_2d.h"
#include "image/bind_img.h"
#include "spy.h"


c_pbo::c_pbo()
	:_index				(	0		)
	,_data_size			(	0		)
	,_bind				(	0		)
//	,_channel_nb		(	0		)
//	,_b_bgr				(	false	)
	,_sx				(	0		)
	,_sy				(	0		)
	,_b_update_needed	(	false	)
{
}

c_pbo::~c_pbo()
{
	dealloc();
}

void c_pbo::dealloc()
{
	if( !_pbo_ids.empty() )
	{
		for( size_t i = 0; i < _pbo_ids.size(); ++i )
		{
			GOL::pbo_dealloc( _pbo_ids[ i ] );
		}
		//delete_buffers( _pbo_ids.size(), &_pbo_ids[ 0 ] );
		_pbo_ids.clear();
	}
}


void c_pbo::allocate( c_img_2d* CONST img, UINT32 CONST bind )
{
	dealloc();
	UINT32	nb_pbo	= texture_flux_master->is_pbo_double_buffer() ? 2 : 1;
	_pbo_ids.resize( nb_pbo );

	_format				= img->get_pixel_format();
	INT32 channel_nb	= img->get_channel_nb();
	img->get_size_xy( _sx, _sy );
	_data_size			= img->get_data_size_used();
	_gl_format			= img->get_gl_format();
	for( UINT32 i = 0; i < nb_pbo; ++i )
	{
		_pbo_ids[ i ]	= GOL::pbo_alloc( -1, _data_size, true );
	}

//	GOL::gen_buffers( nb_pbo, &_pbo_ids[0] );

//	for( size_t i = 0; i < _pbo_ids.size(); ++i )
//	{
//		GOL::bind_buffer( GL_PIXEL_UNPACK_BUFFER, _pbo_ids[i] );
//		GOL::set_buffer_data( GL_PIXEL_UNPACK_BUFFER, _data_size, 0, GL_STREAM_DRAW );
//		GOL::set_buffer_data( GL_PIXEL_UNPACK_BUFFER, _data_size, 0, GL_DYNAMIC_DRAW );
//	}
//	GOL::unbind_buffer( GL_PIXEL_UNPACK_BUFFER );
 //
	g_bind_img_2d->clear_filename_for_index( bind );
	tex_2d_bind_no_gpu_move( bind );

	// PBO data, do alloc create texture then dealloc
	//maa we don't need data to call tex2d_image
//	UINT8*	data	= (UINT8 *)MALLOC_ALIGNED_SIGNATURE( _data_size, 0, __FUNCTION__ );	//todo check for errors and pbs

	//GLenum type = GL_UNSIGNED_BYTE;
	//if( img->is_float() )	type = GL_FLOAT;
	//if( img->is_float() )	type = GL_HALF_FLOAT;
	tex2d.image( _sx, _sy, channel_nb, _gl_format, nullptr, false, img->get_gl_type() );
//	tex2d.sub_image( 0, 0, 0, _sx, _sy, _channel_nb, format, GL_UNSIGNED_BYTE, data, tex::is_sub_do_mipmap() );
//	FREE_ALIGNED_AND_NULL( data );
	//texture = tex;
	_bind = bind;
}
 
//void c_pbo::load_data( CONST UINT8* pixels )
//{
//	if( _pbo_ids.empty() )
//	{
//		ERR_PRINT_STRING( "PBOs not allocated" );
//		return;
//	}
//			_index		= ( _index + 1 ) % _pbo_ids.size();
//	size_t	next_index	= ( _index + 1 ) % _pbo_ids.size();
// 
//	if( _pbo_ids[ next_index ] = -1 )
//	{
//		ERR_PRINT_STRING( "PBO not assigned" );
//		return;	
//	}
//	// bind PBO to update pixel values
//	GOL::pbo_do_bind( _pbo_ids[ next_index ], true );
////	GOL::bind_buffer( GL_PIXEL_UNPACK_BUFFER, _pbo_ids[ next_index ] );
// //
//	// map the buffer object into client's memory
//	// Note that glMapBuffer() causes sync issue.
//	// If GPU is working with this buffer, glMapBuffer() will wait(stall) for GPU to finish its job. To avoid waiting (stall), you can call
//	// first glBufferData() with NULL pointer before glMapBuffer(). If you do that, the previous data in PBO will be discarded and 
//	// GOL::set_buffer_data() returns a new allocated pointer immediately even if GPU is still working with the previous data.
////	GOL::set_buffer_data( GL_PIXEL_UNPACK_BUFFER, _data_size, 0, GL_STREAM_DRAW );
//	GOL::pbo_do_data( _data_size, true );
//	//GOL::set_buffer_data( GL_PIXEL_UNPACK_BUFFER, _data_size, 0, GL_DYNAMIC_DRAW );
//
//	GLubyte*	data_gpu;
//	if( texture_flux_master->is_pbo_buffer_range() )
//		{	data_gpu = (GLubyte*)GOL::pbo_do_map_range( _data_size, true );	}
//	else
//		{	data_gpu = (GLubyte*)GOL::pbo_do_map( true );						}
////	GLubyte*	data_gpu = (GLubyte*)GOL::pbo_do_map( true );
//	//GLubyte*	data_gpu = (GLubyte*)glMapBuffer( GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY );
//	if( data_gpu )
//	{
//		// update data directly on the mapped buffer
//		MEMCPY( data_gpu, pixels, _data_size );
//		GOL::pbo_do_unmap( true );
//		//glUnmapBuffer( GL_PIXEL_UNPACK_BUFFER ); // release pointer to mapping buffer
//	}
// 
//	// it is good idea to release PBOs with ID 0 after use. Once bound with 0, all pixel operations behave normal ways.
//	//GOL::unbind_buffer( GL_PIXEL_UNPACK_BUFFER );
//	GOL::pbo_do_bind( -1, true );
//	_b_update_needed = true;
//}

void c_pbo::load_data( c_img_2d* CONST img, UINT32 CONST bind )
{
	if( !img->is_size_format( _sx,_sy, _format ) )
		allocate( img, bind );

	if( _pbo_ids.empty() )
	{
		ERR_PRINT_STRING( "PBOs not allocated" );
		return;
	}
	_index = (_index + 1) % _pbo_ids.size();
	size_t	next_index = (_index + 1) % _pbo_ids.size();

	if( _pbo_ids[next_index] == -1 )
	{
		ERR_PRINT_STRING( "PBO not assigned" );
		return;
	}

	//_b_bgr = img->is_bgr();
	// bind PBO to update pixel values
	if( GOL::pbo_do_bind( _pbo_ids[next_index], true ) )
	{
		//	GOL::bind_buffer( GL_PIXEL_UNPACK_BUFFER, _pbo_ids[ next_index ] );
		//
		// map the buffer object into client's memory
		// Note that glMapBuffer() causes sync issue.
		// If GPU is working with this buffer, glMapBuffer() will wait(stall) for GPU to finish its job. To avoid waiting (stall), you can call
		// first GOL::set_buffer_data() with NULL pointer before glMapBuffer(). If you do that, the previous data in PBO will be discarded and 
		// GOL::set_buffer_data() returns a new allocated pointer immediately even if GPU is still working with the previous data.
		//	GOL::set_buffer_data( GL_PIXEL_UNPACK_BUFFER, _data_size, 0, GL_STREAM_DRAW );
		GOL::pbo_do_data( _data_size, true );
		//GOL::set_buffer_data( GL_PIXEL_UNPACK_BUFFER, _data_size, 0, GL_DYNAMIC_DRAW );

		GLubyte*	data_gpu;
		if( texture_flux_master->is_pbo_buffer_range() )
			data_gpu = (GLubyte*)GOL::pbo_do_map_range( _data_size, true );
		else
			data_gpu = (GLubyte*)GOL::pbo_do_map( true );

		//	GLubyte*	data_gpu = (GLubyte*)GOL::pbo_do_map( true );
		//GLubyte*	data_gpu = (GLubyte*)glMapBuffer( GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY );
		if( data_gpu )
		{
			// update data directly on the mapped buffer
			void*	pixels = img->get_data();
			if( IS_NOT_NULL( pixels ) )	
				MEMCPY( data_gpu, pixels, _data_size, __FUNCTION__ );
			GOL::pbo_do_unmap( true );
			//glUnmapBuffer( GL_PIXEL_UNPACK_BUFFER ); // release pointer to mapping buffer
		}

		// it is good idea to release PBOs with ID 0 after use. Once bound with 0, all pixel operations behave normal ways.
		//GOL::unbind_buffer( GL_PIXEL_UNPACK_BUFFER );
		GOL::pbo_do_bind( -1, true );
		_b_update_needed = true;
	}
	else
		debug_break( "%s() could not bind pbo : skip the load", __FUNCTION__ );
}

void c_pbo::move_to_gpu( c_img_2d* CONST img, bool b_mipmap_generate )
{
	if( _b_update_needed )
	{
		if( _pbo_ids[_index] == -1 )
		{
			ERR_PRINT_STRING( "PBO not assigned" );
			return;
		}
		TBUF_INC( tbuf::CH_GPU_MOVE, 1., "pbo_move_to_gpu" );
		if( GOL::pbo_do_bind( _pbo_ids[ _index ], true ) )
		{
			//	GOL::bind_buffer( GL_PIXEL_UNPACK_BUFFER, _pbo_ids[_index] );

			if( _gl_format != GL_INVALID_VALUE )
				tex2d.sub_image( 0, 0,0, _sx,_sy, aaa::c_pixel_format::get_channel_nb(_format), _gl_format, img->get_gl_type(), nullptr, b_mipmap_generate );
			//	tex.tex2d_image( _sx, _sy, _channel_nb, format, nullptr, false );

			// it is good idea to release PBOs with ID 0 after use. Once bound with 0, all pixel operations behave normal ways.
			//GOL::unbind_buffer( GL_PIXEL_UNPACK_BUFFER );
			GOL::pbo_do_bind( -1, true );
			TBUF_DEC( tbuf::CH_GPU_MOVE,10., "pbo_move_to_gpu" );
			_b_update_needed = false;
			img->set_gpu_in_sync();
		}
		else
		{
			debug_break( "%s() could not bind pbo : skip the sub_image", __FUNCTION__ );
		}
	}
}
