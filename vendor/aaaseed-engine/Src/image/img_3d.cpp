#include "img_3d.h"
#include "draw/tex.h"
#include "infrastructure/obj/obj_server.h"
#include "file/file_io.h"
#include "format_3d/ddsbase.h"
#include "math/v_base.h"
#include "spy.h"
#include "file/aaa_file.h"
#include "img_compo.h"

c_img_3d*	c_img_3d::_lua_cur = nullptr;

namespace
{
	c_obj_server< c_file_io > server_reader(0);
}

void c_img_3d::set_size_format(	INT32 CONST sx, INT32 CONST sy, INT32 CONST sz, aaa::PIXEL_FORMAT CONST format )
{
	_sz				= sz;
	_over_sz		= OVER_ONE_AS_FP32(sz);
	c_img_base::set_size_format( sx,sy, format );
}

c_img_3d::c_img_3d()
	:_file_io	{nullptr}
{
	set_size_format( 0,0,0, aaa::PIXEL_FORMAT::UNKNOWN );
}

c_img_3d* c_img_3d::create( C_PCHAR_C signature )
{
	c_img_3d* img = new c_img_3d;
	if( !img )
		debug_break( "%s() : Can't create Image (Probably no more memory).", signature );
	return img;
}

c_img_3d* c_img_3d::create( C_PCHAR_C signature, INT32 CONST index )
{
	c_img_3d* img = new c_img_3d;
	if( !img )
		debug_break( "%s() : Can't create Image at Bind %d (Probably no more memory).", signature, index );
	return img;
}

c_img_3d::~c_img_3d()
{
	if( c_img_3d::_lua_cur == this )
		c_img_3d::_lua_cur = nullptr;
}
void	c_img_3d::move_to_gpu( C_PCHAR_C signature, INT32 index_to_debug )
{
/*
	if( IS_NULL( data ) && IS_NULL( _compressed_data ) )
	{
		ERR_PRINT_STRING( "%s() BINDING in %3d image %s with NULL data", __FUNCTION__, tex::get_index_3d(), get_filename() );
		return;
	}
*/
	if( !is_changed() && !is_force_reload() )	//todonow no bind_img here
		return;
	
	GLenum gl_format = get_gl_format();
	if( gl_format == GL_INVALID_VALUE )
	{
		ERR_PRINT_STRING( "%s() BINDING in %2d tex3d %s with invalid format", signature, tex3d.get_index(), get_filename() );
		return;
	}
	
	GLenum gl_type = get_gl_type();
	if( gl_type == GL_INVALID_VALUE )
	{
//todo verbose flag
//		DBG_PRINT_STRING( "%s() BINDING in %2d image %s with a gl type AAASeed don't with for now", signature, tex3d.get_index(), get_filename() );
		auto pformat = get_pixel_format();
		DBG_PRINT_STRING( "%s() BINDING in %d image %s", signature, tex2d.get_index(), get_filename() );
		DBG_PRINT_STRING( "\twith format %s : AAASeed don't deal with corresponding gl_type for now", aaa::c_pixel_format::get_name( pformat ) );
		return;
	}

	GLenum	internal_format = GOL::make_internal_format_gl( get_channel_nb(), GOL::get_internal_type( gl_type ) );
	if( internal_format == GL_INVALID_VALUE )
	{
		ERR_PRINT_STRING( "%s() BINDING in %2d tex3d %s with invalid internal format", signature, tex3d.get_index(), get_filename() );
		return;
	}

	{
		std::lock_guard<c_img_3d> guard(*this);
		void* data = get_data();
		if( IS_NULL( data ) && IS_NULL( _compressed_data ) )
		{
			if( !tex3d.is_size_internal_format( get_size_x(), get_size_y(), get_size_z(), internal_format ) )
			{
				TBUF_INC( tbuf::CH_GPU_MOVE, 1., "tex3d_declare_to_gpu" );
				tex3d.image( get_size_x(), get_size_y(), get_size_z(), _channel_nb, gl_format, data, _b_compress_do, gl_type );
				TBUF_DEC( tbuf::CH_GPU_MOVE, 1., "tex3d_declare_to_gpu" );
			}
			//set_gpu_in_sync();
		}
		else
		{	//hack internal format could be a problem now
			if( tex3d.is_size_internal_format( get_size_x(), get_size_y(), get_size_z(), internal_format ) )
			{
				SPY_PUSH_RANGE( "tex3d_move_to_gpu", spy::TEX_LOW );
				TBUF_INC( tbuf::CH_GPU_MOVE, 1., "tex3d_move_to_gpu" );
					tex3d.sub_image(	0,
										0, 0, 0, get_size_x(), get_size_y(), get_size_z(),
										get_channel_nb(), gl_format, gl_type, data,
										tex::is_sub_mipmap_do()
									);
				TBUF_DEC( tbuf::CH_GPU_MOVE, 1., "tex3d_move_to_gpu" );
				SPY_POP_RANGE();
			}
			else
			{
				TBUF_INC( tbuf::CH_GPU_MOVE, 1., "tex3d_move_to_gpu" );
		//		if( _b_compressed_data...
				{
					tex3d.image( get_size_x(), get_size_y(), get_size_z(), _channel_nb, gl_format, data, _b_compress_do, gl_type );
				}
				//DBG_PRINT_STRING( "Done" );
				TBUF_DEC( tbuf::CH_GPU_MOVE, 1., "tex3d_move_to_gpu" );

		//		if( _b_compressed_do...
			}
			//todo should be cleaner and in the img object
		//	glFinish();
			if( is_free_when_on_board() )
			{
				dealloc_data();
				if( _file_io )
				{
					server_reader.store( _file_io );
					_file_io = {nullptr};
					change_data( nullptr, 0, false );
				}
			}

			set_gpu_in_sync();
		}
	}
}

AAA_ERR c_img_3d::alloc_data(						INT32 CONST sx, INT32 CONST sy, INT32 CONST sz, aaa::PIXEL_FORMAT CONST format,	C_PCHAR_C signature )
{
	return c_img_base::alloc_data( compute_data_size( sx, sy, sz, format ), format, signature );
}

AAA_ERR	c_img_3d::init_with_size_no_cpu_mem(		INT32 CONST sx, INT32 CONST sy, INT32 CONST sz, aaa::PIXEL_FORMAT CONST format,	C_PCHAR_C signature )
{
	if( !is_size_format( sx,sy,sz, format ) )
	{
		init_base();
		set_size_format( sx,sy,sz, format );

		//was before init
		//set_compress_do( false );
		dealloc_data();
	}
	set_changed();
	return AAA_OK;
}

void	c_img_3d::init_from_mem(					INT32 CONST sx, INT32 CONST sy, INT32 CONST sz, aaa::PIXEL_FORMAT CONST format, UINT8* CONST data_in, INT32 data_buf_size_in )
{
	init_base();
	set_size_format( sx,sy,sz, format );
	if( data_buf_size_in == 0 )
		data_buf_size_in = sx * sy * sz * get_channel_nb();	//hack depend on format

	//	done after set_size for check_all
//	if( data_buf_size_in )
//		data_buf_size_in = compute_data_size();



	if( data_in )
		change_data( data_in, data_buf_size_in, false );
	else
		check_full( __FUNCTION__ );
}

AAA_ERR	c_img_3d::init_with_size(					INT32 CONST sx, INT32 CONST sy, INT32 CONST sz, aaa::PIXEL_FORMAT CONST format,	C_PCHAR_C signature )
{
	//todo	INT32 will not always be the default size
	if( !is_size_format( sx,sy,sz, format ) )
	{
		init_base();
		set_size_format( sx,sy,sz, format );

		//was before init
		//set_compress_do( false );
	}
	if( ERR( alloc_data( sx,sy,sz, format, signature ) ) )
		return ERR_ANY;

//	set_changed();
	return AAA_OK;
}

c_img_3d* c_img_3d::img_init_with_size( c_img_3d* pt,	INT32 CONST sx, INT32 CONST sy, INT32 CONST sz, aaa::PIXEL_FORMAT CONST format,	C_PCHAR_C signature )
{
	if( !pt )
		pt = new c_img_3d;
	if( pt )
	{	//todo	INT32 will not always be the default size
		if( ERR( pt->init_with_size( sx, sy, sz, format, signature ) ) )
			SAFE_DELETE( pt );
	}
	return pt;
}


AAA_ERR		c_img_3d::read_pvm(	o_str CONST & filename, bool b_load_data )
{
	UINT32	sx;
	UINT32	sy;
	UINT32	sz;
	INT32	channel_nb	= 1;

	size_t	size = _file_io->get_size_from_cur();


	unsigned int components;
	float scale[3];
	// read and uncompress PVM volume
	unsigned char* volume = readPVMvolume( filename.get(),	&sx, &sy, &sz, &components,
															&scale[0], &scale[1], &scale[2]	);
	if( !volume )
	{
		ERR_PRINT_STRING( "%s() failed using readPVMvolume on %s.", __FUNCTION__, filename.get() );
		return ERR_ANY;
	}
	
	size = sx * sy * sz * components;
	//UINT8*	new_data	= (UINT8*)MALLOC_ALIGNED_SIGNATURE( size, 0, __FUNCTION__ );
	aaa::PIXEL_FORMAT format;
	//todo refine
	if( components == 3 )
		format = aaa::c_pixel_format::make_format_from_channel_type( 3,  aaa::PIXEL_TYPE::UINT_8 );	
	else
		format = aaa::c_pixel_format::make_format_from_channel_type( 1,  components==1 ? aaa::PIXEL_TYPE::UINT_8 : aaa::PIXEL_TYPE::UINT_16 );	
	init_with_size( sx,sy,sz, format, __FUNCTION__ );

	void*	new_data = get_data();
	if( !new_data )
	{
		ERR_PRINT_STRING( "%s() failed Allocating %d bytes for %s.", __FUNCTION__, size, filename );
		FREE( volume );
		return ERR_ANY;
	}
	MEMCPY( new_data, volume, size, __FUNCTION__ );
	FREE( volume );


/*
	//if( size != sx * sy * depth )
	//{
	//	ERR_PRINT_STRING( "%s() size %d don't fit dimensions %d x %d x %d.", __FUNCTION__, size, sx, sy, depth );
	//	return ERR_ANY;
	//}

	//UINT8*	data = _file_io->get_cur();
	aaa::PIXEL_FORMAT format;
	//format = c_pixel_format::make_format_from_channel_type( components, aaa::PIXEL_TYPE::UINT_8 );
	//todo refine 
	format = c_pixel_format::make_format_from_channel_type( 1,  components==1 ? aaa::PIXEL_TYPE::UINT_8 : aaa::PIXEL_TYPE::UINT_16 );
	init_from_mem(	sx,sy, depth, format, volume, size );
	change_data( new_data, size, true );
*/
	return AAA_OK;
}

AAA_ERR		c_img_3d::read_vtk(	o_str CONST & filename, bool b_load_data )
{
	INT32	sx;
	INT32	sy;
	INT32	sz;
	INT32	channel_nb	= 1;

	size_t	size = _file_io->get_size_from_cur();
	C_PCHAR	str;

	str = "STRUCTURED_POINTS";
	if( !_file_io->move_after( str ) )
	{
		ERR_PRINT_STRING( "%s() Can't find %s.", __FUNCTION__, str );
		return ERR_ANY;
	}
	str = "DIMENSIONS";
	if( !_file_io->move_after( str ) )
	{
		ERR_PRINT_STRING( "%s() Can't find %s.", __FUNCTION__, str );
		return ERR_ANY;
	}

	_file_io->scan_int32( sx	);
	_file_io->scan_int32( sy	);
	_file_io->scan_int32( sz	);

	size = sx * sy * sz;
	_file_io->seek_from_end( size );
/*
	}
	else //if( str_is_equal_nocase( ext, "raw" ) )
	{
		sx = 256;
		sy = 256;
		depth = size / ( sx * sy );
	}
*/
	if( size != sx * sy * sz )
	{
		ERR_PRINT_STRING( "%s() size %d don't fit dimensions %d x %d x %d.", __FUNCTION__, size, sx, sy, sz );
		return ERR_ANY;
	}

	//todo64 deal with 64 bits size and propagate
	init_from_mem(	sx,sy,sz, aaa::PIXEL_FORMAT::R_8, _file_io->get_cur(), (INT32)size );

	return AAA_OK;
}

AAA_ERR		c_img_3d::read_dds(	o_str CONST & filename, bool b_load_data )
{
	INT32	sx;
	INT32	sy;
	INT32	sz;

	size_t	size = _file_io->get_size_from_cur() - 128;

	if( size / (256*256) <= 256 )
	{
		sx = 256;
		sy = 256;
	}
	else
	{
		sx = 512;
		sy = 512;
	}
	sz = (INT32) (size / ( sx * sy ));

	if( size != sx * sy * sz )
	{
		ERR_PRINT_STRING( "%s() size %d don't fit dimensions %d x %d x %d.", __FUNCTION__, size, sx, sy, sz );
		return ERR_ANY;
	}

	_file_io->get_data_pt( 128 );
	//todo64 deal with 64 bits size and propagate
	init_from_mem(	sx,sy,sz, aaa::PIXEL_FORMAT::R_8, _file_io->get_cur(), (INT32)size );

	return AAA_OK;
}

AAA_ERR		c_img_3d::read_raw(	o_str CONST & filename, bool b_load_data )
{
	INT32	sx;
	INT32	sy;
	INT32	sz;

	size_t	size = _file_io->get_size_from_cur();

	if( size / (256*256) <= 256 )
	{
		sx = 256;
		sy = 256;
	}
	else
	{
		sx = 512;
		sy = 512;
	}
	sz = (INT32)( size / ( sx * sy ) );

	if( size != sx * sy * sz )
	{
		ERR_PRINT_STRING( "%s() size %d don't fit dimensions %d x %d x %d.", __FUNCTION__, size, sx, sy, sz );
		return ERR_ANY;
	}

	//todo64 deal with 64 bits size and propagate
	init_from_mem(	sx,sy,sz, aaa::PIXEL_FORMAT::R_8, _file_io->get_cur(), (INT32)size );

	return AAA_OK;
}


//	tex3d.image( sx, sy, depth, channel_nb, GL_RED, (void*)data, false, GL_UNSIGNED_BYTE );

////////
////////
//todo deal with all this flags
AAA_ERR c_img_3d::read_from_existing_file( o_str CONST & filename_in, bool CONST b_load_data )
{
	AAA_ERR		retcode = ERR_ANY;
	o_str		filename( filename_in );
#if AAA_DEBUG()
	if( !c_file::is_exist( filename ) )
	{	//normaly this is tested already by caller
		debug_break( "%s Can't open file : %s", __FUNCTION__, filename.get() );
		return ERR_FILE_NO;
	}
#endif

	_b_compressed_data = false;
	//_b_do_compression = false;
	set_compress_do( false );

//	set_mem_type( REGULAR_ALIGNED );
	//_b_bgr = false;

//	if( check_read_aaatc( filename_in ) )
//	{
//		strcpy( filename, _filename_compressed.get() );
//	}

	//	open file
	if( !_file_io )
	{
		_file_io = server_reader.get();
		if( !_file_io )
		{

			ERR_PRINT_STRING( "%s() Can't allocate a c_file_io.", __FUNCTION__ );
			return ERR_ANY;
		}
	}
	retcode = _file_io->read_file( filename );
	if( ERR(retcode) )
	{
		set_empty();
		server_reader.store( _file_io );
		return ERR_FILE_NO;
	}

	set_filename( filename_in );	//why here ? ( now used see read_tga() )

//	C_PCHAR	ext = fname::get_ext( filename );

//	if( str_is_equal_nocase( ext, "rgb" ) || str_is_equal_nocase( ext, ext_sgi ) )
//	{
	//	_file_type = FILE_TYPE_SGI;
//		retcode = read_sgi();
//	}
//	else
	{
		//_file_type	= FILE_TYPE::UNKNOWN;
		auto ext = filename.get_ext();
//		C_PCHAR	ext	= fname::get_ext( reader->get_filename() );

		if( str_is_equal_nocase( ext, "vtk" ) )
			retcode	= read_vtk( filename_in, b_load_data );	//todo b_async );
		else if( str_is_equal_nocase( ext, "pvm" ) )
			retcode	= read_pvm( filename_in, b_load_data );	//todo b_async );
		else if( str_is_equal_nocase( ext, c_img_utils::ext_dds ) )
			retcode	= read_dds( filename_in, b_load_data );	//todo b_async );
		else
			retcode	= read_raw( filename_in, b_load_data );
	}

//	DBG_HEAP_IS_CORRUPT();
	return retcode;
}


CHAR header1[]	= "# vtk DataFile Version 3.0\n";
CHAR header2[]	= "# vtk file saved by AAASeed\n";
CHAR format[]	= "BINARY\n";	//ASCII
//&lt;Name of File&gt;	//before BINARY
CHAR dataset[] = "DATASET STRUCTURED_POINTS\n";
//DIMENSIONS 512 512 174
//SPACING 1.376 1.376 1.385
//ORIGIN 0.000 0.000 0.000
//POINT_DATA 45613056


//COLOR_SCALARS scalars 1
//SCALARS scalars &lt;unsigned_char|unsigned_short&gt;
// bit, unsigned_ char, char, unsigned_ short, short, unsigned_ int, int, unsigned_ long, long, float, ordouble

//SCALARS dataName dataType numComp
//LOOKUP_TABLE tableName (default if not)

AAA_ERR	c_img_3d::write_vtk( o_str CONST & filename )	//, INT32 CONST format )
{
	CHAR str[1024];

	void* data = get_data_uint8();
	
	if( data )
	{
		auto	file = c_file::FOPEN( filename, "wb" );
		if( file )
		{	
			c_file::FWRITE( header1,	sizeof(header1)-1, 1, file );
			c_file::FWRITE( header2,	sizeof(header2)-1, 1, file );
			c_file::FWRITE( format,		sizeof(format) -1, 1, file );
			c_file::FWRITE( dataset,	sizeof(dataset)-1, 1, file );

			sprintf( str, "DIMENSIONS %d %d %d\n", get_size_x(), get_size_y(), get_size_z() );
			c_file::FWRITE( str, strlen(str), 1, file );

			sprintf( str, "SPACING %f %f %f\n", 1., 1., 1. );
			c_file::FWRITE( str, strlen(str), 1, file );

			sprintf( str, "POINT_DATA %d\n", get_size_x() * get_size_y() * get_size_z() );
			c_file::FWRITE( str, strlen(str), 1, file );

			sprintf( str, "COLOR_SCALARS scalars 1\n" );
			c_file::FWRITE( str, strlen(str), 1, file );

			c_file::FWRITE( data, get_data_size_used(), 1, file );

			c_file::FCLOSE( file );
			GOOD_PRINT_STRING( "Img 3d saved %s.", filename.get() );
			return AAA_OK;
		}
		else
		{
			ERR_PRINT_STRING( "Img 3d Can't open file %s for writing.", filename.get() );
		}
	}
	else
	{
		ERR_PRINT_STRING( "Img 3d data for file %s unimplemented for now.", filename.get() );
	}
	return c_img_utils::ERR_CANT_WRITE;
}


AAA_ERR		c_img_3d::write( o_str CONST & filename, c_img_utils::FILE_TYPE_3D image_file_type )
{
	if( !check_data_valid( __FUNCTION__ ) )
	{
		ERR_PRINT_STRING( "IMG : Can't save because no data : %s", filename.get() );
		return c_img_utils::ERR_DATA_NO;
	}

	AAA_ERR		retcode;
	o_str		locname;
	if( filename.is_empty() )
	{
		locname.set( "AAA_NoName" );
		ERR_PRINT_STRING( "%s() try to save image 3d without a filename :", __FUNCTION__ );
		ERR_PRINT_STRING( "\tsaving using \"%s\"", locname.get() );
	}
	else
		locname.set( filename );

	//	get the file type to save to
	auto format = get_pixel_format();

	if( image_file_type == c_img_utils::FILE_TYPE_3D::DEFAULT )
	{
		if( !aaa::c_pixel_format::is_format_i8( format ) )
		{
			goto exit;
		}
		else
		{
			switch( format )
			{
			case aaa::PIXEL_FORMAT::R_8:
				break;
			case aaa::PIXEL_FORMAT::RGB_8:
			case aaa::PIXEL_FORMAT::RGBA_8:
			default:
				ERR_PRINT_STRING( "IMG : %s, Can't save this type of data.", aaa::c_pixel_format::get_name(format) );
				print_info();
				goto exit;
			}
		}
	}

	if( image_file_type == c_img_utils::FILE_TYPE_3D::DEFAULT )
		image_file_type = c_img_utils::FILE_TYPE_3D::VTK;

	{
		C_PCHAR_C	ext = c_img_utils::str_file_type_3d[ static_cast<INT32>(image_file_type) ];
		locname.add_ext( ext );
	}

	//	set the new file name
	set_filename( locname );

	//	open file
//	auto file = c_file::FOPEN( locname.get(), "wb" );
//	if( IS_NULL( file ) )		goto exit;
	switch( image_file_type )
	{
	case c_img_utils::FILE_TYPE_3D::VTK:
		retcode = write_vtk( locname );
		break;
	default:
		ERR_PRINT_STRING( "IMG : Can't save this type of file : %s", locname.get() );
		print_info();
		retcode = c_img_utils::ERR_FILE_TYPE;
		break;
	}

	if( ERR( retcode ) ) // || ferror( file ) )
		goto exit;
	//	close the file
	set_changed();
	//c_file::FCLOSE( file );
	//file = nullptr;
	//	time_modification = c_file::get_mdate( file );
	c_file::push_vfile();
		set_time_modification( c_file::get_mdate( locname ) );
	c_file::pop_vfile();

	return  AAA_OK;
exit:
	//if( IS_NULL( file ) )
	//{
	//	ERR_PRINT_STRING( "IMG write : Can't open %s", locname.get() );
	//}
	//else
	{
		ERR_PRINT_STRING( "%s() : Can't write %s ", __FUNCTION__, locname.get() );
		//c_file::FCLOSE( file );
		//	_file = nullptr;
	}
	return  c_img_utils::ERR_CANT_WRITE;
}

void	c_img_3d::print_info() CONST
{
	GOOD_PRINT_STRING( "image_bind_3d(%d) : %s", tex3d.get_index(), get_filename() );
	GOOD_PRINT_STRING( "\t%ldx%ldx%ld, %ld Channels, %ld BytePerPixel.", get_size_x(), get_size_y(), get_size_z(), _channel_nb, get_byte_per_pixel() );
}

using namespace aaa;

void	c_img_3d::fill_rgba( FP32 CONST * CONST color )
{
	if( !check_valid(__FUNCTION__) )
		return;

	FP32 col[4];
	aaa::PIXEL_FORMAT	format =_pixel_format;
	if( c_pixel_format::is_bgr(format) )
	{
		col[0] = color[2];
		col[1] = color[1];
		col[2] = color[0];
		col[3] = color[3];
	}
	else
		cpy_v4( col, color );

	INT32 pitch = get_byte_pitch();
		
	switch( get_data_type() )
	{
	case aaa::PIXEL_TYPE::UINT_8:
		{
			UINT8* dst = get_data_uint8();

			switch( format )
			{
			case PIXEL_FORMAT::R_8:			fill_channel_1<UINT8>	( dst,	pitch,	aaa::img::c_compo::to_uint8( col[0] )	);	break;
			case PIXEL_FORMAT::RG_8:		fill_channel_2<UINT8>	( dst,	pitch,	aaa::img::c_compo::to_uint8( col[0] ),
																					aaa::img::c_compo::to_uint8( col[1] )	);	break;
			case PIXEL_FORMAT::RGB_8:
			case PIXEL_FORMAT::BGR_8:		fill_channel_3<UINT8>	( dst,	pitch,	aaa::img::c_compo::to_uint8( col[0] ),
																					aaa::img::c_compo::to_uint8( col[1] ),
																					aaa::img::c_compo::to_uint8( col[2] )	);	break;
			case PIXEL_FORMAT::RGBA_8:
			case PIXEL_FORMAT::BGRA_8:
				{
					//if( texture_flux_master->is_convert_to_rgb_sse3() )	
					//	fill_rgba8_sse3( col )
					//else
					{
						UINT32	u32;
						UINT32	rgba;
		//todo endian could change this 
						u32 = UINT32( aaa::img::c_compo::to_uint8( col[0] ) );
						rgba = u32;
						u32 = UINT32( aaa::img::c_compo::to_uint8( col[1] ) );
						rgba |= u32<<8;
						u32 = UINT32( aaa::img::c_compo::to_uint8( col[2] ) );
						rgba |= u32<<16;
						u32 = UINT32( aaa::img::c_compo::to_uint8( col[3] ) );
						rgba |= u32<<24;

					//	INT32	i32 = get_pixel_nb_to_process_aligned_x() + 1;
					//	UINT32*	a	= ((UINT32*)dst) - 1;
					//	while( --i32 )
					//	{
					//		*++a = rgba;
					//	}
						UINT32	sx		= get_size_x();
						UINT32	pitch	= get_byte_pitch();
						for( INT32 i = get_size_y(); i > 0; --i )
						{
							INT32	i32 = sx;
							UINT32*	a = (UINT32*)dst - 1;
							while( i32-- )
							{
								*++a = rgba;
							}
							dst += pitch;
						}
					}
					set_changed();
				}
				break;
			default:
				print_err_unsupported_format( __FUNCTION__ );
				break;
			}
		}
		break;
	case aaa::PIXEL_TYPE::UINT_16:
		{
			pitch /= 2;
			UINT16* dst = get_data_uint16();

			switch( format )
			{
			case PIXEL_FORMAT::R_16:		fill_channel_1<UINT16>	( dst,	pitch,	aaa::img::c_compo::to_uint16( col[0] )	);	break;
			case PIXEL_FORMAT::RG_16:		fill_channel_2<UINT16>	( dst,	pitch,	aaa::img::c_compo::to_uint16( col[0] ),
																					aaa::img::c_compo::to_uint16( col[1] )	);	break;
			case PIXEL_FORMAT::RGB_16:																						
			case PIXEL_FORMAT::BGR_16:		fill_channel_3<UINT16>	( dst,	pitch,	aaa::img::c_compo::to_uint16( col[0] ),
																					aaa::img::c_compo::to_uint16( col[1] ),
																					aaa::img::c_compo::to_uint16( col[2] )	);	break;
			case PIXEL_FORMAT::RGBA_16:											
			case PIXEL_FORMAT::BGRA_16:		fill_channel_4<UINT16>	( dst,	pitch,	aaa::img::c_compo::to_uint16( col[0] ),
																					aaa::img::c_compo::to_uint16( col[1] ),
																					aaa::img::c_compo::to_uint16( col[2] ),
																					aaa::img::c_compo::to_uint16( col[3] )	);	break;
			default:
				print_err_unsupported_format( __FUNCTION__ );
				break;
			}
		}
		break;
	case aaa::PIXEL_TYPE::FLOAT_16:
		{
			pitch /= 2;
			FP16* dst = get_data_fp16();

			switch( format )
			{
			case PIXEL_FORMAT::DEPTH_16:
			case PIXEL_FORMAT::R_16FP:		fill_channel_1<FP16>	( dst,	pitch,	aaa::img::c_compo::to_fp16( col[0] )	);	break;
			case PIXEL_FORMAT::RG_16FP:		fill_channel_2<FP16>	( dst,	pitch,	aaa::img::c_compo::to_fp16( col[0] ),
																					aaa::img::c_compo::to_fp16( col[1] )	);	break;
			case PIXEL_FORMAT::RGB_16FP:
			case PIXEL_FORMAT::BGR_16FP:	fill_channel_3<FP16>	( dst,	pitch,	aaa::img::c_compo::to_fp16( col[0] ),
																					aaa::img::c_compo::to_fp16( col[1] ),
																					aaa::img::c_compo::to_fp16( col[2] )	);	break;
			case PIXEL_FORMAT::RGBA_16FP:
			case PIXEL_FORMAT::BGRA_16FP:	fill_channel_4<FP16>	( dst,	pitch,	aaa::img::c_compo::to_fp16( col[0] ),
																					aaa::img::c_compo::to_fp16( col[1] ),
																					aaa::img::c_compo::to_fp16( col[2] ),
																					aaa::img::c_compo::to_fp16( col[3] )	);	break;
			default:
				print_err_unsupported_format( __FUNCTION__ );
				break;
			}
		}
		break;
	case aaa::PIXEL_TYPE::FLOAT_32:
		{
			pitch /= 4;
			FP32* dst = get_data_fp32();

			switch( format )
			{
			case PIXEL_FORMAT::DEPTH_32:
			case PIXEL_FORMAT::R_32FP:		fill_channel_1<FP32>	( dst,	pitch,	col[0]	);	break;
			case PIXEL_FORMAT::RG_32FP:		fill_channel_2<FP32>	( dst,	pitch,	col[0],
																					col[1]	);	break;
			case PIXEL_FORMAT::RGB_32FP:												 
			case PIXEL_FORMAT::BGR_32FP:	fill_channel_3<FP32>	( dst,	pitch,	col[0],
																					col[1],
																					col[2]	);	break;
			case PIXEL_FORMAT::RGBA_32FP:											
			case PIXEL_FORMAT::BGRA_32FP:	fill_channel_4<FP32>	( dst,	pitch,	col[0],
																					col[1],
																					col[2],
																					col[3] 	);	break;
			default:
				print_err_unsupported_format( __FUNCTION__ );
				break;
			}
		}
		break;
	default:
		print_err_unsupported_type( __FUNCTION__ );
		break;
	}
}