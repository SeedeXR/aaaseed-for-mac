/*  things to do
 * 
 *  deal with channel_split
 *	and data_owner/free/dealloc stuff
 *	pointer return when NULL
 * 
 */

#include "img.h"
#include "img_master.h"
#include "gol/gol.h"
#include "draw/tex.h"
#include "wrapper/open_cv_image.h"
#include "spy.h"


c_img_2d*	c_img_2d::_lua_cur = nullptr;


void	c_img_2d::c_init()
{
	DBG_PRINT_STRING( "%s() Begin", __FUNCTION__ );
	//init_uint8_to_float();
	c_init_send();
	img_libs_init();

	DBG_PRINT_STRING( "%s() Done", __FUNCTION__ );
}

void	c_img_2d::c_deinit()
{
	img_libs_deinit();
	c_deinit_send();
}

c_img_2d::c_img_2d()
{
	set_size_format( 0,0, aaa::PIXEL_FORMAT::UNKNOWN );
}

c_img_2d* c_img_2d::create( C_PCHAR_C signature )
{
	c_img_2d* img = new c_img_2d;
	if( !img )
		debug_break( "%s() : Can't create Image (Probably no more memory).", signature );
	return img;
}

// index is just here for debug
c_img_2d* c_img_2d::create( C_PCHAR_C signature, INT32 CONST index )
{
	c_img_2d* img = new c_img_2d;
	if( !img )
		debug_break( "%s() : Can't create Image at Bind %d (Probably no more memory).", signature, index );
	return img;
}

c_img_2d::~c_img_2d()
{
	if( _lua_cur == this )
		_lua_cur = nullptr;
}

void	c_img_2d::move_to_gpu( C_PCHAR_C signature, INT32 index_to_debug )
{
/*
	if( IS_NULL( data ) && IS_NULL( _compressed_data ) )
	{
		//todoq we should process this case to init texture
		//ERR_PRINT_STRING( "%s() BINDING in %2d image %s with NULL data", __FUNCTION__, tex2d::get_index_2d(), get_filename() );
		//ERR_PRINT_STRING( "%s() BINDING image %s with NULL data", __FUNCTION__, get_filename() );
		return;
	}
*/
	if( !is_gpu_move() )
		return;
	if( !is_changed() && !is_force_reload() )	//todonow no bind_img here
		return;

//	if( index_to_debug==187 )
//		DBG_PRINT_STRING( "Debug 187" );
	GLenum	gl_format = get_gl_format();
	if( gl_format == GL_INVALID_VALUE )
	{
		ERR_PRINT_STRING( "%s() BINDING in %6d image %s with invalid format", signature, tex2d.get_index(), get_filename() );
		return;
	}
	
	auto gl_type = get_gl_type();
	if( gl_type == GL_INVALID_VALUE )
	{
//todo verbose flag
		auto pformat = get_pixel_format();
		DBG_PRINT_STRING( "%s() BINDING in %d image %s", signature, tex2d.get_index(), get_filename() );
		DBG_PRINT_STRING( "\twith format %s : AAASeed don't deal with corresponding gl_type for now", aaa::c_pixel_format::get_name( pformat ) );
        return;
	}

	{
		std::lock_guard<c_img_2d> guard(*this);

		void* data = get_data();	//check with lock or directaccess	
		if( IS_NULL(data) && IS_NULL(_compressed_data) )
		{
			GLenum	internal_format = GOL::make_internal_format_gl( get_channel_nb(), GOL::get_internal_type( gl_type ) );
			if( internal_format == GL_INVALID_VALUE )
			{
				ERR_PRINT_STRING( "%s() BINDING in %2d image %s with invalid internal format", signature, tex2d.get_index(), get_filename() );
				return;
			}
			if( !tex2d.is_size_internal_format( get_size_x(), get_size_y(), internal_format ) )
			{
				SPY_PUSH_RANGE( "tex2d_declare_to_gpu", spy::TEX_LOW );
				TBUF_INC( tbuf::CH_GPU_MOVE, 1., "tex2d_declare_to_gpu" );
					tex2d.image( get_size_x(), get_size_y(), get_channel_nb(), gl_format, nullptr, _b_compress_do, get_gl_type() );
				TBUF_DEC( tbuf::CH_GPU_MOVE, 1., "tex2d_declare_to_gpu" );
				SPY_POP_RANGE();
			}
			//set_gpu_in_sync();
		}
		else
		{
			//DBG_PRINT_STRING( "Go" );
			bool b_sub;
			//hack internal format could be a problem now
			if( _b_compressed_data )
				b_sub = false;
			else
			{
				GLenum	internal_format = GOL::make_internal_format_gl( get_channel_nb(), GOL::get_internal_type( gl_type ) );
				if( internal_format == GL_INVALID_VALUE )
				{
					ERR_PRINT_STRING( "%s() BINDING in %2d image %s with invalid internal format", signature, tex2d.get_index(), get_filename() );
					return;
				}
				b_sub = tex2d.is_size_internal_format( get_size_x(), get_size_y(), internal_format );
			}
			if( b_sub )
			{
				SPY_PUSH_RANGE( "tex2d_move_to_gpu sub", spy::TEX_LOW );
				TBUF_INC( tbuf::CH_GPU_MOVE, 1., "tex2d_move_to_gpu" );
					tex2d.sub_image(	0,
										0, 0, get_size_x(), get_size_y(),
										get_channel_nb(), gl_format, get_gl_type(), data,
										tex::is_sub_mipmap_do()
									);
				TBUF_DEC( tbuf::CH_GPU_MOVE, 1., "tex2d_move_to_gpu" );
				SPY_POP_RANGE();
			}
			else
			{
				TBUF_INC( tbuf::CH_GPU_MOVE, 1., "tex2d_move_to_gpu" );
				if( _b_compressed_data )
				{
					SPY_PUSH_RANGE( "tex2d_move_to_gpu compressed", spy::TEX_LOW );
						tex2d.image_compressed( get_size_x(), get_size_y(), _channel_nb, _compressed_format, _compressed_size, _compressed_mipmap_nb, _compressed_data );
					SPY_POP_RANGE();
				}
				else
				{
					SPY_PUSH_RANGE( "tex2d_move_to_gpu image", spy::TEX_LOW );
						tex2d.image( get_size_x(), get_size_y(), get_channel_nb(), gl_format, data, _b_compress_do, get_gl_type() );
					SPY_POP_RANGE();
				}
				TBUF_DEC( tbuf::CH_GPU_MOVE, 1., "tex2d_move_to_gpu" );

				if( _b_compress_do && g_img_master->is_compress_write() )
				{
					// get compressed image from OpenGL
					GLenum compressed_format = tex2d.get_internal_format_compressed();
					UINT64 size = tex2d.get_size_byte();
					if( size > 0 && compressed_format > 0 )
					{
						alloc_data_compressed( size, compressed_format );
						if( _compressed_data )
						{
							tex2d.get_compressed( _compressed_data );
							write_aaatc( get_filename_o_str(), _compressed_data, compressed_format, size );
							dealloc_data_compressed();
						}
					}
				}
				//todo should be cleaner and in the img object
				//todo should it be done in all the case
				if( is_free_when_on_board() )
					dealloc_data();
			}
			set_gpu_in_sync();
		}
	}
}

#if 0
void	c_img_2d::copy_to_gpu( C_PCHAR_C signature, INT32 index_dst )
{
/*
	if( IS_NULL( data ) && IS_NULL( _compressed_data ) )
	{
		//todoq we should process this case to init texture
		//ERR_PRINT_STRING( "%s() BINDING in %2d image %s with NULL data", __FUNCTION__, tex2d::get_index_2d(), get_filename() );
		//ERR_PRINT_STRING( "%s() BINDING image %s with NULL data", __FUNCTION__, get_filename() );
		return;
	}
*/

//todo we make it simple this time
//	if( !is_changed() && !is_force_reload() )	//todonow no bind_img here
//		return;

//	if( index_to_debug==187 )
//		DBG_PRINT_STRING( "Debug 187" );

	GLenum	format =  get_gl_format();
	if( format == GL_INVALID_VALUE )
	{
		ERR_PRINT_STRING( "%s() BINDING in %2d image %s with invalid format", signature, tex2d.get_index(), get_filename() );
		return;
	}
	
	auto gl_type = get_gl_type();
	if( gl_type == GL_INVALID_VALUE )
	{
//todo verbose flag
//				DBG_PRINT_STRING( "%s() BINDING in %2d image %s with a gl type AAASeed don't with for now", signature, tex2d.get_index(), get_filename() );
		return;
	}

	GLenum	internal_format = GOL::make_internal_format_gl( get_channel_nb(), GOL::get_internal_type( gl_type ) );
	if( internal_format == GL_INVALID_VALUE )
	{
		//DBG_PRINT_STRING( "invalid internal format" );
		ERR_PRINT_STRING( "%s() BINDING in %2d image %s with invalid internal format", signature, tex2d.get_index(), get_filename() );
		return;
	}

	{
		std::lock_guard<c_img_2d> guard(*this);

		tex_info* ti = get_info(index_dst);
		void* data = get_data();	//check with lock or directaccess
		if( IS_NULL( data ) && IS_NULL( _compressed_data ) )
		{
			if( !tex2d.is_size_internal_format( get_size_x(), get_size_y(), internal_format ) )
			{
				TBUF_INC( tbuf::CH_GPU_MOVE, 1., "tex2d_declare_to_gpu" );
				tex2d.image( get_size_x(), get_size_y(), _channel_nb, format, data, _b_compressed_do, get_gl_type() );
				TBUF_DEC( tbuf::CH_GPU_MOVE, 1., "tex2d_declare_to_gpu" );
			}
			//set_gpu_in_sync();
		}
		else
		{
			//DBG_PRINT_STRING( "Go" );
			//hack internal format could be a problem now
			if( tex2d.is_size_internal_format( get_size_x(), get_size_y(), internal_format ) )
			{
				SPY_PUSH_RANGE( "tex2d_move_to_gpu", spy::TEX_LOW );
				TBUF_INC( tbuf::CH_GPU_MOVE, 1., "tex2d_move_to_gpu" );
					tex2d.sub_image(	0,
										0,0, get_size_x(),get_size_y(),
										get_channel_nb(), format, get_gl_type(), data,
										tex::is_sub_mipmap_do()
									);
				TBUF_DEC( tbuf::CH_GPU_MOVE, 1., "tex2d_move_to_gpu" );
				SPY_POP_RANGE();
			}
			else
			{
				TBUF_INC( tbuf::CH_GPU_MOVE, 1., "tex2d_move_to_gpu" );
				if( _b_compressed_data )
				{
					tex2d.image_compressed( get_size_x(), get_size_y(), _channel_nb, _compressed_format, _compressed_size, _compressed_mipmap_nb, _compressed_data );
				}
				else
				{
					tex2d.image( get_size_x(), get_size_y(), _channel_nb, format, data, _b_compressed_do, get_gl_type() );
				}
				TBUF_DEC( tbuf::CH_GPU_MOVE, 1., "tex2d_move_to_gpu" );

				if( _b_compressed_do && g_img_master->is_compress_write() )
				{
					// get compressed image from OpenGL
					INT32 compressed_format = tex2d.get_internal_format_compressed();
					UINT64 size = tex2d.get_tex_size();
					if( size > 0 && compressed_format > 0 )
					{
						alloc_data_compressed( size, format );
						if( _compressed_data )
						{
							tex2d.get_compressed( _compressed_data );
							write_aaatc( get_filename_o_str(), _compressed_data, compressed_format, size );
							dealloc_data_compressed();
						}
					}
				}
				//todo should be cleaner and in the img object
				//todo should it be done in all the case
				if( is_free_when_on_board() )
					dealloc_data();
			}
			set_gpu_in_sync();
		}
	}
}
#endif //0


void	c_img_2d::move_from_gpu( C_PCHAR_C signature, INT32 index_to_debug )
{
/*
	if( IS_NULL( data ) && IS_NULL( _compressed_data ) )
	{
		//todoq we should process this case to init texture
		//ERR_PRINT_STRING( "%s() BINDING in %2d image %s with NULL data", __FUNCTION__, tex2d::get_index_2d(), get_filename() );
		//ERR_PRINT_STRING( "%s() BINDING image %s with NULL data", __FUNCTION__, get_filename() );
		return;
	}
*/
//todo
//	if( !is_gpu_move() )
//		return;
//	if( !is_changed() && !is_force_reload() )	//todonow no bind_img here
//		return;

//	if( index_to_debug==187 )
//		DBG_PRINT_STRING( "Debug 187" );

	set_cpu_keep( true );

	GLenum gl_format = get_gl_format();
	if( gl_format == GL_INVALID_VALUE )
	{
		ERR_PRINT_STRING( "%s() BINDING in %6d image %s with invalid format", signature, tex2d.get_index(), get_filename() );
		return;
	}
	
	auto gl_type = get_gl_type();
	if( gl_type == GL_INVALID_VALUE )
	{
//todo verbose flag
		auto pformat = get_pixel_format();
		DBG_PRINT_STRING( "%s() BINDING in %d image %s", signature, tex2d.get_index(), get_filename() );
		DBG_PRINT_STRING( "\twith format %s : AAASeed don't deal with corresponding gl_type for now", aaa::c_pixel_format::get_name( pformat ) );
        return;
	}

	{
		std::lock_guard<c_img_2d> guard(*this);

		init_with_size( get_size_x(),get_size_y(), get_pixel_format(), __FUNCTION__ );
		void* data = get_data();
		if( IS_NULL(data) ) // && IS_NULL(_compressed_data) )
		{
			ERR_PRINT_STRING( "%s() No data", signature );
		}
		else
		{
			GOL::get_tex_image_2d( 0, gl_format, gl_type, data );
			//set_gpu_in_sync();
		}
	}
}

AAA_ERR	c_img_2d::alloc_data(							INT32 CONST sx, INT32 CONST sy, aaa::PIXEL_FORMAT format, C_PCHAR_C signature )
{
	return c_img_base::alloc_data( compute_data_size( sx,sy, format ), format, signature );
}

////////
////////
//todo test return code to protect better
AAA_ERR	c_img_2d::init_with_size_no_cpu_mem(			INT32 CONST sx, INT32 CONST sy, aaa::PIXEL_FORMAT CONST format,	C_PCHAR_C signature )
{
	std::lock_guard<c_img_2d> guard(*this);

	if( !is_size_format( sx,sy, format ) )
	{	
		init_base();
		set_size_format( sx,sy, format );
		//was before init
		//set_compress_do( false );
		dealloc_data();
	}
	set_changed();

	return AAA_OK;
}

AAA_ERR	c_img_2d::init_with_size(						INT32 CONST sx, INT32 CONST sy, aaa::PIXEL_FORMAT CONST format,	C_PCHAR_C signature )
{
	//todo	std::lock_guard<c_img_2d> guard(*this);
	if( !is_size_format( sx,sy, format ) )
	{
		init_base();
		set_size_format( sx,sy, format );

		//was before init
		//set_compress_do( false );
	}

	if( ERR( alloc_data( sx,sy, format, signature ) ) )	
		return ERR_ANY;
//	set_changed();
	return AAA_OK;
}

c_img_2d* c_img_2d::img_init_with_size( c_img_2d* p_img,	INT32 CONST sx, INT32 CONST sy, aaa::PIXEL_FORMAT CONST format,	C_PCHAR_C signature )
{
	if( !p_img )
		p_img = c_img_2d::create( __FUNCTION__ );
	if( p_img )
	{	//todo	INT32 will not always be the default size
		if( ERR( p_img->init_with_size( sx,sy, format, signature ) ) )
			SAFE_DELETE( p_img );
	}
	return p_img;
}
c_img_2d*	c_img_2d::img_init_from_img( c_img_2d* p_img, C_PCHAR_C signature )
{
	return c_img_2d::img_init_with_size( nullptr, p_img->get_size_x(),p_img->get_size_y(), p_img->get_pixel_format(), signature );
}



AAA_ERR		c_img_2d::resize( FP32 CONST factor )
{
	if( factor == FP32(1) )
		return AAA_OK;

	auto sx = get_size_x();
	auto sy = get_size_y();
	sx = MAX( 2, I_FLOOR( sx * factor ) );
	sy = MAX( 2, I_FLOOR( sy * factor ) );
	auto const pixel_format = get_pixel_format();

	cv::Mat	data_mat( get_size_y(),get_size_x(), aaa::c_cv::get_cv_type( pixel_format ), get_data_uint8(), get_byte_pitch() );
	cv::Mat	resized;
	//	resize
	cv::resize( data_mat, resized, cv::Size(sx,sy), .0, .0, cv::InterpolationFlags::INTER_LANCZOS4 );

	bool b_rescaled = copy_from_src( resized.data, (INT32)resized.step, pixel_format, pixel_format, resized.cols, resized.rows, false );

	resized.release();
	return b_rescaled ? AAA_OK : ERR_ANY;
}

bool	c_img_2d::copy_from_src( CONST void* RESTRICT src, INT32 CONST src_pitch,
										aaa::PIXEL_FORMAT CONST pixel_format_src,
										aaa::PIXEL_FORMAT CONST pixel_format_dst, UINT32 CONST sx, UINT32 CONST sy,
										bool CONST b_flip_vert )
{
	if( IS_NULL( src ) )
	{
		debug_break( "%s() : NULL source pointer", __FUNCTION__ );
		return false;
	}

	// image will be resized if needed in copy_src_to_rgba, grey and rgb
	st_img_conv options( sx,sy );
	options.b_flip_vert = b_flip_vert;

	AAA_ERR retcode = init_with_size( sx,sy, pixel_format_dst, __FUNCTION__ );
	options.src_pixel_format = pixel_format_src;
	if( NOERR(retcode) )
	{
		options.signature = __FUNCTION__;
		retcode = copy_from_src( src, src_pitch, options );
	}
	return retcode;
}


bool	c_img_2d::copy_from_img( c_img_2d* CONST img_src, bool b_flip_vert, bool b_swap_red_blue )
{
	if( IS_NULL( img_src ) )
		return false;

	aaa::PIXEL_FORMAT format_src = img_src->get_pixel_format();
	if( b_swap_red_blue )
		format_src = aaa::c_pixel_format::get_format_red_blue_swapped( format_src );
	return copy_from_src(	img_src->get_data(), img_src->get_byte_pitch(), format_src,
							img_src->get_pixel_format(), img_src->get_size_x(), img_src->get_size_y(), b_flip_vert );
}



////////
////////
//todo	regroup with init() ? and check if we do enough or too much here
//todo this is pretty dangerous because if data_in is released free it will crash
//todo  so check in all the calls and adapt, it is ok if temporary only.
void	c_img_2d::init_from_mem(					INT32 CONST sx, INT32 CONST sy, aaa::PIXEL_FORMAT CONST format, UINT8* CONST data_in, UINT64 data_buf_size_in )
{
	set_size_format( sx,sy, format );
	if( data_buf_size_in == 0 )
		data_buf_size_in = sx * sy * get_channel_nb();	//hack depend on format

	//	done after set_size for check_all
	if( data_buf_size_in )
		data_buf_size_in = compute_data_size();

	init_base();
	if( data_in )
		change_data( data_in, data_buf_size_in, false );
	else
		check_full( __FUNCTION__ );
}

c_img_2d* c_img_2d::img_init_from_mem( c_img_2d* p_img, INT32 CONST sx, INT32 CONST sy, aaa::PIXEL_FORMAT CONST format, UINT8* CONST new_data, UINT64 CONST new_data_buf_size )
{
	if( !p_img )
		p_img = c_img_2d::create( __FUNCTION__ );		// deallocation must be done by caller
	if( p_img )
		p_img->init_from_mem( sx,sy, format, new_data, new_data_buf_size );
	return p_img;
}

//todoq
c_img_2d* c_img_2d::img_init_from_img_cropped(	c_img_2d* p_img, INT32 x, INT32 y, INT32 sx, INT32 sy )
{
//todo do we make a rectangle obj and use it
//todo fix the direction of the y/v axe of image
	INT32 l = MIN(0,x);
	INT32 b = MIN(0,y);

	ERR_PRINT_STRING( "method %s() not implemented", __FUNCTION__ );
//	if( !p_img )
//		p_img = c_img_2d::create( __FUNCTION__ );
	return p_img;
}

////////
////////
AAA_ERR	c_img_2d::read_from_existing_file( o_str CONST & filename_in, bool CONST b_load_data )
{
	AAA_ERR	retcode = ERR_ANY;
	o_str	filename( filename_in );
	bool	b_compress_do = false;

	o_str	rescale_filename;
	bool	b_need_rescaling = false;
	c_img_utils::FILE_TYPE image_file_type = c_img_utils::FILE_TYPE::DEFAULT;

	// check if file need to be rescaled on load
	if( g_img_master->is_scale_on_load() )
	{
		auto ext = filename.get_ext();
		image_file_type = c_img_utils::get_save_type_from_ext( ext );
		// Check filetype : only rescale TGA, PNG, // EXR, JPG
		switch( image_file_type )
		{
		case c_img_utils::FILE_TYPE::TGA:
		case c_img_utils::FILE_TYPE::PNG:
		case c_img_utils::FILE_TYPE::JPG:
		case c_img_utils::FILE_TYPE::EXR:
			{
				o_str rescale_path;
				rescale_path.set( g_img_master->get_scale_on_load_path() );
				// Only rescaled image if a path has been set
				if( !rescale_path.is_empty() )
				{
					if( !rescale_path.is_ending_with( '/' ) )
						rescale_path.add_slash();
					// the rescaled image will be saved in a subfolder ( rescalePath ) using the same name
					rescale_filename.set_dir_name( filename );
					rescale_filename.add_slash();
					rescale_filename.add( rescale_path );
					auto CONST name = fname::get_fname( filename.get() );
					rescale_filename.add( name );
					// check if file exists
					if( c_file::is_exist( rescale_filename ) )
					{
						// check timestamp between rescaled image and source, if source is more recent then regenerate scaled image
						auto CONST mtime = c_file::get_mdate( filename );
						auto CONST mtime_scaled = c_file::get_mdate( rescale_filename );
						if( mtime > mtime_scaled )
						{
							b_need_rescaling = true;
							DBG_PRINT_STRING( "IMG : Recreate rescaled image %s", filename.get() );
						}
						else
						{
							// load already scaled image
							filename = rescale_filename;
							DBG_PRINT_STRING( "IMG : Loading rescaled image %s", filename.get() );
						}
					}
					else
						b_need_rescaling = true;
				}
			}
			break;
		default:
			DBG_PRINT_STRING( "Format with extension \".%s\" not supported for rescaling on load", ext );
			break;
		}
	}

	//todo caller check will never lead to this case
	if( GOL::is_tex_compressed() && g_img_master->is_compress_read() )
	{
		//todo check we store the new name so do we add it at every iteration or check
		o_str filename_aaatc( filename );
		//check if compressed texture exist
		filename_aaatc.add_ext( c_img_utils::ext_aaatc );
		if( c_file::is_exist( filename_aaatc ) )
			filename.set( filename_aaatc );
		else										
			b_compress_do = true;	// load image, then compress using OpenGL
	}

#if AAA_DEBUG()
	if( !c_file::is_exist( filename ) )
	{	//normaly this is tested already by caller
		debug_break( "%s Can't open file : %s", __FUNCTION__, filename.get() );
		return ERR_FILE_NO;
	}
#endif

//	DBG_HEAP_IS_CORRUPT();
	auto ext = filename.get_ext();

	bool	b_need_to_read;
	// we deal with some special case
	if( ext )
	{
		b_need_to_read = false;
		c_img_utils::FILE_TYPE type = c_img_utils::get_save_type_from_ext( ext );
		switch( type )
		{
		case c_img_utils::FILE_TYPE::TGA:
			if( g_img_master->is_tga_read_custom() )
				retcode = read_tga( filename, b_load_data );
			else
				b_need_to_read = true;
			break;
		case c_img_utils::FILE_TYPE::EXR:
#if AAA_LIB_USE_IMG_OPENEXR()
			retcode = aaa::img::openexr::read( this, filename.get() );
#else
			retcode = ERR_UNIMPLEMENTED_YET;
			DBG_PRINT_STRING( "This version of version of AAASeed don't deal with exr format" );
#endif
			break;
		case c_img_utils::FILE_TYPE::SGI:
		case c_img_utils::FILE_TYPE::YUV:
		case c_img_utils::FILE_TYPE::DDS:
		case c_img_utils::FILE_TYPE::AAATC:
		//	||	str_is_equal_nocase( ext, "astc"	)
			{
				auto file = c_file::FOPEN( filename, "rb" );
				if( !IS_NULL( file ) )
				{
					switch( type )
					{
					case c_img_utils::FILE_TYPE::SGI:	retcode = read_sgi( file, filename );	break;
					case c_img_utils::FILE_TYPE::YUV:	retcode = read_yuv( file );				break;
					case c_img_utils::FILE_TYPE::DDS:	retcode = read_dds( file, filename );	break;
			//		else if( str_is_equal_nocase( ext, "astc" ) )
			//			retcode = read_astc( file, filename );
					case c_img_utils::FILE_TYPE::AAATC:	retcode = read_aaatc( file, filename );	break;
					}
					c_file::FCLOSE( file );
				}
			}
			break;
		default:
			b_need_to_read = true;
			break;
		}
	}
	else
		b_need_to_read = true;

	// or we try to use the libs
	if( b_need_to_read )
	{
#if	AAA_LIB_USE_IMG_GFLSDK() || AAA_LIB_USE_IMG_FREEIMAGE()
		retcode = img_read_with_lib( this, filename, b_load_data );
#else
		DBG_PRINT_STRING( "IMG : Unrecognized extension for : %s", filename );
#endif
	}

	set_compress_do( b_compress_do );
	if( retcode == AAA_OK && b_need_rescaling )
	{
		auto const size_min = g_img_master->scale_on_load_size_min();
		if( size_min <= get_size_x() || size_min <= get_size_y() )
		{
			auto retcode_resized = resize( g_img_master->scale_on_load_factor() );
			if( NOERR( retcode_resized ) )
			{
				if( g_img_master->is_scale_on_load_save() )
				{
					// drop extension, it will added in write() function
					rescale_filename.drop_ext();
					if( NOERR( write( rescale_filename, image_file_type ) ) )
					{
						DBG_PRINT_STRING( "IMG : Rescaled image saved for %s", filename_in.get() );
					}
				}
			}
			else
			{
				ERR_PRINT_STRING( "IMG : Failed to resize image on load %s", filename_in.get() );
			}
		}
	}

//	DBG_HEAP_IS_CORRUPT();
	if( g_img_master->is_memory_check_after_reading() )
		mem::is_all_ok( "c_img_2d::read_from_existing_file()" );
	return retcode;
}


void	c_img_2d::set_colum( UINT8* buf, INT32 x, INT32 h )
{
	UINT8* dst_start = get_data_valid_rgb_uint8( __FUNCTION__ );
	if( !dst_start )
		return;
	if( !buf )
		return;

	//todo	other case
	if( get_byte_per_pixel() == 4 )
	{
		UINT32*	src = (UINT32*) buf;
//		INT32	line_offset = get_size_x_aligned();
		INT32	line_offset = get_size_x();
		UINT32*	dst = ((UINT32*)dst_start) + x;
		--src;
		for( INT32 i = h; i > 0; --i )
		{
			*dst = *++src;
			dst += line_offset;
		}
	}
	else
	{
		print_err_unsupported_channel_nb( __FUNCTION__ );
	}
}



void	c_img_2d::draw()
{
//	printf( "!!! img_draw\n" );
	if( !check_data_valid( __FUNCTION__ ) )
		return;

	if( _channel_nb == 3 || _channel_nb == 4 )
	{
		GOL::draw_pixels( get_size_x(),get_size_y(), GL_RGBA, GL_UNSIGNED_BYTE, get_data() );
	}
	else
	{
		DBG_PRINT_STRING( "Drawing img with %d channel is not implemented yet.", _channel_nb );
		print_err_unsupported_channel_nb( __FUNCTION__ );
	}
/*
	GOL::set_mask_color( true,	false,   false,  false );
	GOL::draw_pixels( 	size_x,size_y, GL_RED, GL_UNSIGNED_BYTE, data );
	GOL::set_mask_color( false,	true,   false,  false );
	GOL::draw_pixels( 	size_x,size_y, GL_GREEN, GL_UNSIGNED_BYTE, data + size_x*size_y );
	GOL::set_mask_color( false,	false,   true,  false );
	GOL::draw_pixels( 	size_x,size_y, GL_BLUE, GL_UNSIGNED_BYTE, data + 2*size_x*size_y );
	GOL::set_mask_color( true,	true,   true,  true );
*/
}

//tdo [perhaps add a type arg so we can do in one call what require at least two
void	c_img_2d::read_pixels( INT32 x, INT32 y, INT32 sx, INT32 sy, INT32 which_buffer )
{
	void* dst = get_data();
	if( !dst )
	{
		ERR_PRINT_STRING( "%s() no data to read_pixels,", __FUNCTION__ );
		return;
	}

//todo	color_base->pixel_transfert_update();

	GLenum	gl_format =  get_gl_format();

	tex2d.read_pixels( dst, gl_format, get_gl_type(), x,y, sx,sy, which_buffer );
//todo	color_base->pixel_transfert_reset();
	if( !GOL::have_alpha() )
		fill_alpha_from_rgb();
	set_changed();
}

void	c_img_2d::print_info() CONST
{
	GOOD_PRINT_STRING( "image_bind_2d(%d) : %s", tex2d.get_index(), get_filename() );
	GOOD_PRINT_STRING( "\t%ldx%ld, %ld Channels, %ld BytePerPixel.", get_size_x(),get_size_y(), _channel_nb, get_byte_per_pixel() );
}
