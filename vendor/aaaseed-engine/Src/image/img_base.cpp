#include "img_base.h"
#include "img_utils.h"
#include "img_master.h"
#include "spy.h"


bool	c_img_base::gb_force_reload_ui			= false;	//inited by param
bool	c_img_base::gb_free_when_on_board_ui	= false;
bool	c_img_base::gb_verbose_image_missing_ui	= true;

// pitch is the size in byte to go from a pixel at position x in one line to the pixel at the same position in the next line
INT32	c_img_base::compute_byte_pitch( INT32 CONST sx, INT32 CONST bits_per_pixel, INT32 CONST alignment )
{
	//UINT32 CONST	bit_depth = 8;
	//UINT32 CONST	bits_per_pixels = bit_depth * channel_nb;
	//UINT32		pitch = sx * bits_per_pixels / 8;

	//UINT32 CONST	a = 4;
	//if( sx == 2560 || sx == 1920 )
	//{
	//	debug_break( "sx == 2560" );
	//}
	UINT32	pitch = sx * bits_per_pixel / 8;
	if( alignment > 0 )
		pitch = ( pitch + alignment - 1 ) & ~(alignment - 1);
	//else
	//	debug_break( "alignment == 0" );

	return pitch;
	//switch( channel_nb )
	//{
	//case 1:
	//case 3:	return (pitch & 0x3) ? (pitch&0xfffffffc) + 4 : pitch;
	//case 2:	return (pitch & 0x1) ? pitch + 1 : pitch;
	//default:	return pitch;
	//}
}
INT32	c_img_base::compute_byte_pitch( INT32 CONST sx, aaa::PIXEL_FORMAT CONST pixel_format, INT32 CONST alignment )
{
	return compute_byte_pitch( sx, aaa::c_pixel_format::get_bits_per_pixel( pixel_format ), alignment );
}
UINT64	c_img_base::compute_data_size( INT32 CONST sx, INT32 CONST sy, aaa::PIXEL_FORMAT CONST pixel_format )
{
	return compute_byte_pitch( sx, aaa::c_pixel_format::get_bits_per_pixel( pixel_format ) ) * sy;
}

UINT64	c_img_base::compute_data_size(	INT32 CONST sx, INT32 CONST sy, INT32 CONST sz, aaa::PIXEL_FORMAT CONST pixel_format )
{
	return compute_data_size( sx, sy, pixel_format ) * sz ;
}

void	c_img_base::print_err_unimplemented( C_PCHAR_C fn_name, C_PCHAR_C str_format_asked ) CONST
{
	if( g_img_master->is_unimplemented_verbose() )
	{
		debug_break_if(	g_img_master->is_unimplemented_break(),	"%s() No %s image data, wrong format (see below) or check image_free_when_on_board eventually", fn_name, str_format_asked );
		print_info();
	}
}

void	c_img_base::print_err_unsupported_channel_nb( C_PCHAR_C fn_name ) CONST
{
	if( g_img_master->is_unimplemented_verbose() )
	{
		debug_break_if(	g_img_master->is_unimplemented_break(),	"%s() not implemented with this channel nb %d", fn_name, _channel_nb );
		print_info();
	}
}

void	c_img_base::print_err_unsupported_format( C_PCHAR_C fn_name ) CONST
{
	if( g_img_master->is_unimplemented_verbose() )
	{
		auto format_name = aaa::c_pixel_format::get_name( _pixel_format );
		debug_break_if(	g_img_master->is_unimplemented_break(),	"%s() not implemented with this format %s", fn_name, format_name );
		print_info();
	}
}

void	c_img_base::print_err_unsupported_type( C_PCHAR_C fn_name ) CONST
{
	if( g_img_master->is_unimplemented_verbose() )
	{
		auto type_name = aaa::c_pixel_format::get_pixel_type_name( aaa::c_pixel_format::get_pixel_type(_pixel_format) );
		debug_break_if(	g_img_master->is_unimplemented_break(),	"%s() not implemented with this type %s", fn_name, type_name );
		print_info();
	}
}

void	c_img_base::init_base()
{
//	set_changed();
	reset_time_modification();
	set_cpu_keep( false );
	set_gpu_move( true );
	set_compress_do( false );
	set_empty();
}

FINLINE	void c_img_base::set_pixel_format( aaa::PIXEL_FORMAT CONST format )
{
	_pixel_format	=	format;
	_pixel_type		=	aaa::c_pixel_format::get_pixel_type( format );
	_channel_nb		=	aaa::c_pixel_format::get_channel_nb( format );
	_byte_per_pixel	=	aaa::c_pixel_format::get_bits_per_pixel( format ) / 8;
	_byte_pitch		=	compute_byte_pitch( _sx, aaa::c_pixel_format::get_bits_per_pixel( format ) );
}
void c_img_base::set_size_format( INT32 CONST sx, INT32 CONST sy, aaa::PIXEL_FORMAT CONST format )
{
	_sx				=	sx;
	_sy				=	sy;
	_over_sx		=	OVER_ONE_AS_FP32(sx);
	_over_sy		=	OVER_ONE_AS_FP32(sy);
	set_pixel_format( format );
}


c_img_base::c_img_base()
	:_data					{ nullptr }
	,_b_data_owner			{ true }
	,_data_size_allocated	{ 0 }
	,_data_size_used		{ 0 }
	,_b_lock				{ false }
	,_b_free_when_on_board	{ false }
	,_b_changed				{ false }
	//,_sx					{ 0 }
	//,_sy					{ 0 }
	//,_over_sx				{ 0 }
	//,_over_sy				{ 0 }
	,_b_reading				{ false }
	,_byte_pitch			{ 0 }
	,_byte_per_pixel		{ 0 }
	,_channel_nb			{ 0 }
	,_pixel_format			{ aaa::PIXEL_FORMAT::UNKNOWN	}
	,_pixel_type			{ aaa::PIXEL_TYPE::UNKNOWN		}

	,_compressed_data		{ nullptr }
	,_b_compressed_data		{ false }
	,_compressed_mipmap_nb	{ 0 }
	,_compressed_format		{ 0 }
	,_compressed_size		{ 0 }
	,_b_compress_do			{ false }
{
	init_base();
	set_size_format( 0,0, aaa::PIXEL_FORMAT::UNKNOWN );
}

c_img_base::~c_img_base()
{
	if( _b_lock )
		unlock();
	if( IS_NOT_NULL(this) )	// != nullptr )
		dealloc_data();
	else
		DBG_PRINT_STRING( "IMG : try to free NULL image" );
}
void	c_img_base::set_data_size_allocated( UINT64 CONST size )
{
	_data_size_allocated = size;
#if	AAA_DEBUG()
	check_full(__FUNCTION__);
#endif
}
void	c_img_base::set_data_size_used( UINT64 CONST size )
{
	_data_size_used = size;
#if	AAA_DEBUG()
	check_full(__FUNCTION__);
#endif
}

void	c_img_base::dealloc_data_compressed()
{
	IF_FREE_ALIGNED_AND_NULL( _compressed_data );
	_compressed_size	= 0;
	_b_compressed_data	= false;
	//todo we need to keep the info (when lua ask for example but we should use the tex info ?
	//_compressed_format	= 0;
}

void	c_img_base::alloc_data_compressed( UINT64 CONST size, INT32 CONST format )
{
	dealloc_data_compressed();
	if( size > 0 && format > 0 )
	{
		_compressed_data = (UINT8*) MALLOC_ALIGNED_SIGNATURE( size, 0, "Compressed data" );
		if( IS_NULL( _compressed_data ) )
		{
			ERR_PRINT_STRING( "Can't allocate compressed data" );
		}
		else
		{
			_b_compressed_data = true;
			_compressed_format	= format;
			_compressed_size	= size;
		}
	}
}
////////
////////
void	c_img_base::dealloc_data()
{
	if( get_data_size_allocated()!=0 && _data ) 
	{
		if( _b_data_owner )
		{
			IF_FREE_ALIGNED_AND_NULL( _data );
			//todo we need to keep the info (when lua ask for example but we should use the tex info ?
		}
		else	//	we don't own it so we just forget it
		{		//archi there is a real potential for leaks here
			_data = nullptr;
		}
		set_data_size_used( 0 );
		set_data_size_allocated( 0 );
		_b_data_owner = true;
		set_empty();
		//todo we need to keep the info (when lua ask for example but we should use the tex info ?
		//_pixel_format = PIXEL_FORMAT::UNKNOWN;
	}
//	else
//		DBG_PRINT_STRING( "IMG : no data to free in this image" );
	dealloc_data_compressed();
}
////////
////////
AAA_ERR	c_img_base::alloc_data( UINT64 CONST size_asked, aaa::PIXEL_FORMAT CONST format, C_PCHAR_C signature )
{
	if( !this )
	{
		DBG_PRINT_STRING( "%s() : try to alloc data for NULL image", __FUNCTION__ );
		return c_img_utils::ERR_NULL;
	}
	if( size_asked == 0 )
	{
		DBG_PRINT_STRING( "%s() : can't alloc 0 bytes, didn't even try.", __FUNCTION__ );
		return c_img_utils::ERR_DATA_ALLOC;
	}
	if( !aaa::c_pixel_format::is_supported( format ) )
	{
		DBG_PRINT_STRING( "%s() : don't support %s, didn't even try.", aaa::c_pixel_format::get_name( format ), __FUNCTION__ );
		return c_img_utils::ERR_DATA_ALLOC;
	}
	if( get_data_size_allocated() >= size_asked )	//	if storage already big enough and correct memory
	{
		set_data_size_used( size_asked );
		return AAA_OK;			//		just use it
	}

	if( !_b_data_owner )	//	we don't own it so we just forget it
	{						//archi there is a real potential for leaks here
		set_data_size_used( 0 );
		set_data_size_allocated( 0 );
		set_empty();
		_data			= nullptr;
		_b_data_owner	= true;
	}

	void* data_new;
	if( _data )
		data_new = REALLOC_ALIGNED_SIGNATURE( _data, size_asked, 1024, signature );	//todo check for errors and pbs
	else
		data_new = MALLOC_ALIGNED_SIGNATURE( size_asked, 1024, signature );	//todo check for errors and pbs
		
	if( data_new )
	{
		_data = data_new;
		set_data_size_used( size_asked );
		set_data_size_allocated( size_asked );
		set_changed();
		return	AAA_OK;
	}
	
	DBG_PRINT_STRING( "%s() : can't %s requested memory, please free some", __FUNCTION__, _data ? "realloc" : "alloc" );
	if( _data )
		dealloc_data();
	return c_img_utils::ERR_DATA_ALLOC;
}


bool	c_img_base::check_full( C_PCHAR_C signature ) CONST
{
	UINT64 size = get_data_size_used();
	UINT64 size_computed = compute_data_size();
	if( size != 0 && size < size_computed )
	{
		debug_break( "%s() Houston we got a problem : image data_size %ull smaller than what it is supposed to be %ull", signature, size, size_computed );
		return false;
	}
	return true;
}

void	c_img_base::change_data( UINT8* CONST data_new, UINT64 CONST size_new, bool CONST b_owner_new )
{
//	std::lock_guard<c_img_base> guard(*this);		
	set_changed();
	dealloc_data();
	_data					= data_new;
	_b_data_owner			= b_owner_new;
	_data_size_allocated	= size_new;
	set_data_size_used( size_new );
	clear_empty();
}

////////
////////
INT32	c_img_base::get_checksum()
{
	UINT8* src = get_data_valid_rgb_uint8( __FUNCTION__ );
	if( !src )
	{
		ERR_PRINT_STRING( "%s() no uint8 data to compute checksum,", __FUNCTION__ );
		return 0;
	}

	INT32	checksum = 0;
	if( _pixel_format != aaa::PIXEL_FORMAT::YUY2 )
	{
		// todo need to deal with pitch
		//INT32	i32 = get_pixel_nb_to_process_aligned_x();

		//checksum = 0x3435d464;
		//if ( _channel_nb == 1 )
		//{
		//	UINT8* a = src - 1;
		//	while( i32-- )
		//	{
		//		checksum += *++a;
		//	}
		//}
		//else if ( _channel_nb == 3 )
		//{
		//	UINT8* a = src - 1;
		//	while( i32-- )
		//	{
		//		checksum += *++a;
		//		checksum -= *++a;
		//		checksum ^= *++a;
		//	}
		//}
		//else if ( _channel_nb == 4 )
		//{
		//	INT32* a = ((INT32*)src) - 1;
		//	while( i32-- )
		//	{
		//		checksum += *++a;
		//	}
		//}
		//else
		{
			print_err_unsupported_format( __FUNCTION__ );
		}
	}
	else
	{
		print_err_unsupported_format( __FUNCTION__ );
	}
	return checksum;
}

void	c_img_base::print_info() CONST
{
	GOOD_PRINT_STRING( "\t%ldx%ld, %ld Channels, %ld BytePerPixel.", get_size_x(), get_size_y(), _channel_nb, get_byte_per_pixel() );
}

//todo	better memory scheme
template< class IMG >
static	IMG* c_img_base::read( IMG* image, o_str CONST & filename, bool CONST b_load_data, bool CONST b_free, bool CONST b_force_keep, bool CONST b_premultiply )
{	
	SPY_PUSH_RANGE2( b_load_data ? "img_read" : "img_header", spy::FILE_HIGH, filename );

	bool b_need_delete = false;
	bool b_new_image = false;

	// add to list of filenames thread needs to load
	// start the thread if needed
	//todo clean trailing blank in filename as a general policy
	if( filename.is_empty() )
	{
#if	AAA_DEBUG()
		DBG_PRINT_STRING( "IMG : Can't open file with no filename " );
#endif
		if( image )
			image->set_empty();
		goto exit;
	}

	//	store modification time for this file
	time_t	mtime;
	if( c_file::is_vfile() )
	{
		c_file::push_vfile();
		mtime = c_file::get_mdate( filename );
		c_file::pop_vfile();
	}
	else
		mtime = c_file::get_mdate( filename );

	if( !mtime )	//	it means the file is not there
	{
		if( gb_verbose_image_missing_ui )
			NO_MEDIA_PRINT_STRING( "IMG : Can't find image : %s", filename.get() );		
		b_need_delete = true;
		goto exit;
	}

	if( image )
	{
		//	check if the image is already loaded there
		//maa 2018 I removed  2018 I removed if( !image->is_changed() but I fon't know why it is there
		//if( !image->is_changed() && image->get_time_modification() == mtime )
		if( image->get_filename_o_str().is_equal( filename ) && image->get_time_modification() == mtime )
		{
			if( !b_force_keep || image->get_data_size_used()!=0 )	//related to test in c_bind_img<T>::get_image_data( INT32 CONST index )
			{
#if	AAA_DEBUG()
				o_str	filename_rel;
				filename_rel.set_fname_relative( filename );
				if( strcmp( image->get_filename(), filename_rel.get() ) == 0 )
					DBG_PRINT_STRING( "%s already loaded", filename_rel.get() );
#endif
				goto exit;
			}
		}
	}
	else
	{	//	check if we can get an image
		image = IMG::create( __FUNCTION__ );
		if( image )
			b_new_image = true;
	}

	if( image )
	{
		//todo should be before_read() begin
		image->lock();
		image->set_reading( true );

		image->set_free_when_on_board( b_free && !b_force_keep );

		//todo should be before_read() end

		AAA_ERR	retcode = image->read_from_existing_file( filename, b_load_data );
		if( b_force_keep )
			image->set_cpu_keep( true );

		//todo should be after_read() begin
		if( NOERR( retcode ) )
		{
			if( b_premultiply && image->has_alpha() )
				image->premultiply_alpha();

			//	place a relative path in the filename
			image->set_filename_rel( filename );
			image->set_time_modification( mtime );
			image->set_changed();
		//archi have a verbose ? we want to avoid message when loading on the fly continuously
#if	AAA_DEBUG()
		//	image->print_info();
#endif
			//GOOD_PRINT_STRING( "IMG : Assumed %s format for %s : Done Ok", img_file_type_str[image->file_type], filename.get() );
		}
		else
		{
			image->set_empty();
			ERR_PRINT_STRING( "IMG : Failed to read %s", filename.get() );
		}
		image->set_reading( false );
		image->unlock();
		//todo should be after_read() end
	}

	if( b_new_image )
		b_need_delete |= image->is_empty();

exit:	
	if( b_need_delete )
		SAFE_DELETE( image );

	SPY_POP_RANGE2();
	return image;
}

//todoq deal with force keep ?
template< class IMG >
static IMG* c_img_base::reread( IMG* image )
{
	if( image )
		return read( image, image->get_filename() );
	else
	{
		ERR_PRINT_STRING( "%s() : Can't with non existent image", __FUNCTION__ );
		return nullptr;
	}
}

// generate template fns
#include "img.h"
template c_img_2d* c_img_base::read(   c_img_2d* image, o_str CONST & filename, bool CONST b_load_data, bool CONST b_free, bool CONST b_force_keep, bool CONST b_premultiply );
template c_img_2d* c_img_base::reread( c_img_2d* image );

#include "img_3d.h"
template c_img_3d* c_img_base::read(   c_img_3d* image, o_str CONST & filename, bool CONST b_load_data, bool CONST b_free, bool CONST b_force_keep, bool CONST b_premultiply );
template c_img_3d* c_img_base::reread( c_img_3d* image );