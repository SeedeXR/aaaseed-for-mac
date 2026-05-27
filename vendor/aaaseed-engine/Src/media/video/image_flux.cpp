#include "image_flux.h"
#include "media/video/texture_flux_master.h"
#include "time/speed.h"
#include "media/video/tex_video.h"
#include "media/video/vbl.h"
#include "infrastructure/compute_master.h"
#include "image/img_compo.h"
#include "spy.h"


using namespace aaa;

INT32	c_image_flux::image_flux_obj_count = 0;

//SYNC this should disappear soon
void	c_image_flux_buffer::sync_low( INT32 size_x, INT32 size_y, PIXEL_FORMAT	format )
{
	IF_THIS_NULL_RETURN();

	if( size_x <= 0 || size_y <= 0 )
		return;

	if( _b_callback )
		_images_lock->lock();

		if( !_images_free.empty() )
		{
			for( auto const & p_img_with_lock : _images_free )
				p_img_with_lock->init_with_size( size_x,size_y, format, __FUNCTION__ );
		}
		if( !_images_used.empty() )
		{
			for( auto const & p_img_with_lock : _images_used )
				p_img_with_lock->init_with_size( size_x,size_y, format, __FUNCTION__ );
		}

	if( _b_callback )
		_images_lock->unlock();

	set_size_format( size_x,size_y, format );
}

//c_img_with_lock*	c_image_flux_buffer::get_free_image_locked( c_image_flux* image_flux )
c_img_2d*	c_image_flux_buffer::get_free_image_locked()
{
//	if( image_flux->is_frame_callback() )	_images_lock->lock();
	if( _b_callback )
		_images_lock->lock();

		IMG_CONT*	p_cont;
		if( !_images_free.empty() )
			p_cont = &_images_free;
		else
		{
			if( _images_used.empty() )	//it happened in some F12 situation (e.g. chanel )
			{
				ERR_PRINT_STRING( "%s() no free or used image avalaible should not happen", __FUNCTION__ );
				p_cont = nullptr;
			}
			else
				p_cont = &_images_used;
		}

		c_img_2d* pi;
		if( p_cont )
		{
			pi = *(p_cont->begin());
		//	if( image_flux->is_frame_callback() ) 
			if( _b_callback )
			{
				if( !pi->try_lock() )
				{	
					INT32 count = texture_flux_master->inc_image_flux_lock_fail();
					if( texture_flux_master-> is_image_flux_lock_fail_verbose() )
						ERR_PRINT_STRING( "%s() trying to lock an already locked image : happened %d", __FUNCTION__, count );
					//hack trying to avoid AAASeed being lock
					pi = nullptr;
				}
			}
			//todo	move to copy_frame_to_img in case it fail

			if( pi )
			{
				p_cont->pop_front();
				_images_used.push_back( pi );
				inc_image_index();
			}
		}
		else
		{
			pi = nullptr;
		}

	//if( image_flux->is_frame_callback() )	_images_lock->unlock();
	if( _b_callback )
		_images_lock->unlock();
	return pi;
}

//	always called by tex_video for now
//c_img_with_lock*	c_image_flux_buffer::lock_and_get_image_index( c_image_flux* image_flux, INT32 image_index, bool b_use_closest )
c_img_2d*	c_image_flux_buffer::lock_and_get_image_index( INT32 image_index, bool b_use_closest, bool& b_img_locked )
{
	c_img_2d* ret = nullptr;
	//compute_master.dec();	//hack
//	if( image_flux->is_frame_callback() )	_images_lock->lock();
	if( _b_callback )
		_images_lock->lock();
	//BEGIN

		INT32 index = (INT32)_images_used.size() - 1;
		//b_use_closest = true;
		if( b_use_closest )
		{
		}
		else
		{
			// calculate index in image_used queue 
			index -= _image_index_in - image_index;
		}
		if( UINT32(index) >= _images_used.size() || index < 0 )
		{

			if( b_use_closest )
				ERR_PRINT_STRING( "%s() no _images_used", __FUNCTION__ );
			else
				ERR_PRINT_STRING( "%s() no _images_used with index %d", __FUNCTION__, index );
			ret = nullptr;
		}
		else
		{
			ret = _images_used[index];
			if( ret->is_empty() )	//todo refine 	append when size don't match in case of switch
				ret = nullptr;
		}

	//END
	//if( image_flux->is_frame_callback() )
	if( _b_callback )
	{
		//g_compute_master->dec();	//hack 
		if( ret )
		{
			b_img_locked = true;
			ret->lock();
		}
		//g_compute_master->inc();
		_images_lock->unlock();
	}
	else
	{
		b_img_locked = false;
	}
	//compute_master.inc();
	return ret;
}

//ALLOC
//	dangerous we should lock before calling
//todoqq we should lock() individual images before going on with deletion
//todo improve to alloc only different ones
//void	c_image_flux_buffer::dealloc( c_image_flux* image_flux )

void	c_image_flux_buffer::free_container_image_lock( IMG_CONT& container )
{
	if( !container.empty() )
	{
		for( auto const & p_img_with_lock : container )
		{
			p_img_with_lock->lock();		// lock to make sure that we wait until image is ready
			delete p_img_with_lock;			// delete image, no need to unlock destructor will unlock image
		}
		container.clear();
	}
}

void	c_image_flux_buffer::dealloc()
{
	if( _b_callback )
		_images_lock->lock();

		free_container_image_lock( _images_free );
		free_container_image_lock( _images_used );

	if( _b_callback )
		_images_lock->unlock();
}

void	c_image_flux_buffer::alloc( INT32 size_x, INT32 size_y, aaa::PIXEL_FORMAT format, INT32 nb_in )
{
	if( _image_nb_to_keep != nb_in )	// && size_x>0 )	//avoid doing allocation deallocation when the is nothing (eg mp3)
	{
//		_b_callback = image_flux->is_frame_callback();
		dealloc();

		if( _b_callback )
			_images_lock->lock();

			if( _images_free.empty() )
			{
				_image_nb_to_keep = nb_in;
				INT32 i = nb_in;	//todoqqq was + 2 ???
				for( ; i > 0; --i )
				{
					c_img_2d* pi = c_img_2d::create(__FUNCTION__);
					if( pi )
					{
						pi->set_cpu_keep( true );
						_images_free.push_back( pi );
					}
					else
					{
						_image_nb_to_keep = 0;
						break;
					}
				}
			}

		if( _b_callback )
			_images_lock->unlock();

		sync_low( size_x, size_y, format );
	}
}

c_image_flux_buffer::c_image_flux_buffer()
	//:_image_nb_to_keep		(	 0	)
//	,_size_x				(	-1	)
//	,_size_y				(	-1	)
{
	_images_lock = new aaa::MUTEX;
}

c_image_flux_buffer::~c_image_flux_buffer()
{
	dealloc();
	delete _images_lock;
}

void	c_image_flux_buffer::clear_image_index()
{
	_image_index_in = -1;
}

void	c_image_flux_buffer::inc_image_index()
{
	++_image_index_in;
}

/*
//ASYNC
static	void
#ifdef	WIN32
__cdecl
#endif
th_image_flux_loop( void *dummy )
{
//	( (c_image_flux *)dummy )->set_loop();	//todo we never stop it
	thread_end();
}

void	c_image_flux::start_loop()
{
	GOOD_PRINT_STRING( "image_flux thread begin" );
	thread_begin( th_image_flux_loop, 0, (void *)this );
	GOOD_PRINT_STRING( "image_flux thread begin done" );
}
*/

void	c_image_flux::set_tex_video( c_tex_video* CONST p )
{
	if( p && _p_tex_video && p && p != _p_tex_video )
	{
		debug_break( "tex_video already assigned", __FUNCTION__ );
	}
	_p_tex_video = p;
}


void	c_image_flux::set_flux_size_format_direct( INT32 size_x_in, INT32 size_y_in, PIXEL_FORMAT pixel_format )
{
	if( _flux_size_x != size_x_in || _flux_size_y != size_y_in || _pixel_format_used != pixel_format )
	{
		_flux_size_x		= size_x_in;
		_flux_size_y		= size_y_in;
		_size_x				= size_x_in;
		_size_y				= size_y_in;
		_pixel_format_used	= pixel_format;
	}
}

void	c_image_flux::set_flux_size_format( INT32 size_x_in, INT32 size_y_in, PIXEL_FORMAT pixel_format_in )
{	//todo this should be extended/checked
	_pixel_format_default		= c_pixel_format::get_default_format( pixel_format_in );
	PIXEL_FORMAT pixel_format	= c_pixel_format::get_pixel_format_from_force( _s_force_out_pixel_format );
	if( pixel_format == PIXEL_FORMAT::BGRA_8 )
		_pixel_format_default = PIXEL_FORMAT::BGRA_8;

	_src_pixel_format			= pixel_format_in;
	set_flux_size_format_direct( size_x_in, size_y_in, _pixel_format_default );
}

//void	c_image_flux::clear_audio_index()
//{
//	_audio_index_in = -1;
//}

void	c_image_flux::clear_image_index()
{
	_image_flux_buffer->clear_image_index();
}

//void	c_image_flux::clear_stream_index()
//{
//	clear_audio_index();
//	clear_image_index();
//}

c_image_flux::c_image_flux( c_image_flux_buffer* buf, bool b_callback )
	:_b_frame_callback				(	b_callback	)
//	,_p_tex_video					(	nullptr		)
	,_b_use_last_frame				(	false		)
	,_b_valid						(	false		)
	,_b_field_flip_order			(	false		)
	,_size_x						{0}	//	size of image taking crop in account
	,_size_y						{0}
	,_flux_size_x					{0}	//	size of flux
	,_flux_size_y					{0}
	,_b_crop						{false}
	,_b_crop_h						{false}
	,_b_crop_v						{false}
	,_crop_left						{0}
	,_crop_right					{0}
	,_crop_top						{0}
	,_crop_bottom					{0}

	,_b_src_y_inverted				{false}		// Some library will produced inverted images, need to flip them
	,_b_src_bgr						{false}
	,_b_swap_red_blue				{false}
	,_b_do_field_separation			{false}
	,_image_flux_buffer				{nullptr}

	,_b_threshold					{false}
	,_luma_min						{0}
	,_luma_max						{1}
	,_disp_min						{0}
	,_disp_max						{1}

	,_image_flux_obj_index			{-42}
	,_min							{2}
	,_max							{-1}
	,_gain_factor					{-1}
	,_bias_factor					{-1}
	,_speed_fps						{nullptr}
	,_p_buffer_first				{nullptr}
	,_buffer_count					{0}

	,_s_force_src_pixel_format		{ aaa::PIXEL_FORMAT_SRC_FORCE::DEFAULT	}
	,_s_force_out_pixel_format		{ aaa::PIXEL_FORMAT_FORCE::DEFAULT		}
	,_pixel_format_default			{ aaa::PIXEL_FORMAT::RGBA_8				}
	,_pixel_format_used				{ aaa::PIXEL_FORMAT::UNKNOWN			}
{
	if( buf )
	{
		_image_flux_buffer		= buf;
	}
	else
	{
		_image_flux_buffer		= new c_image_flux_buffer;
		_image_flux_buffer->set_owner( this );
	}

	_speed_fps		= new c_speed( false );

//	clear_stream_index();
	clear_image_index();
	_image_flux_obj_index = ++image_flux_obj_count;
}

c_image_flux::~c_image_flux()
{
	if( _image_flux_buffer && _image_flux_buffer->is_owner(this) )
		SAFE_DELETE( _image_flux_buffer );
	SAFE_DELETE( _speed_fps );
}

//void	c_image_flux::inc_audio_index()
//{
//	++_audio_index_in;
////	if(  c_capture::b_verbose )
////		VERBOSE_PRINTF( "capture audio %d", audio_captured_index);
//}

/*
void	c_image_flux_buffer::inc_image_index()
{
//	if( b_src_interlaced_ && b_field_separation_ )
//		image_index_in_ += 2;
//	else
		++_image_index_in;

//	if(  c_capture::b_verbose )
//		VERBOSE_PRINTF( "capture frame %d", frame_captured_index);
}
*/

INT32	c_image_flux::get_image_index() CONST
{
	return _image_flux_buffer ? _image_flux_buffer->get_image_index() : -1;
}


c_img_2d*	c_image_flux::lock_and_get_image_index( INT32 image_index, bool& b_img_locked ) CONST
{
	return _image_flux_buffer->lock_and_get_image_index( image_index, _b_use_last_frame, b_img_locked );
}

void	c_image_flux::got_frame( UINT8 CONST * CONST RESTRICT src, C_PCHAR_C signature, UINT32 CONST src_pitch, bool b_force_alpha, REAL alpha_value )
{
#if AAA_VBL_USE()
	vbl::update();
#endif

	if( IS_NULL( src ) )
	{
		debug_break( "FRANZ we got a null source and we shouldn't" );
		return;
	}
	
	auto * tex_video = get_tex_video();
	if( !tex_video )
	{
		ERR_PRINT_STRING( "%s() image_flux not attached to a tex_video: skipping Frame.", __FUNCTION__ );
		return;
	}
	SPY_PUSH_RANGE( "got_frame", spy::IMG );

	_speed_fps->begin();
	//	count the number of buffer used by the underlining code layer just for info
	if( _buffer_count <= 32 )
	{
		++_buffer_count;
		if( texture_flux_master->is_verbose() )
			GOOD_PRINT_STRING( "image flux %d (%d) image buffer address -> 0x%x", _image_flux_obj_index, _buffer_count, src );
		if( _buffer_count==1 )
			_p_buffer_first = src;
		else if( _p_buffer_first == src )
		{
			if( texture_flux_master->is_verbose() )
				GOOD_PRINT_STRING( "image flux %d image buffer nb is %d", _image_flux_obj_index, _buffer_count-1 );
			_buffer_count = 33;
		}
		else if( _buffer_count == 32 )
		{
			if( texture_flux_master->is_verbose() )
				GOOD_PRINT_STRING( "image flux %d have more image buffer than %d. stop counting.", _image_flux_obj_index, _buffer_count );
		}
	}

	// Pick a destination format we can convert _src_pixel_format into: honour _s_force_out_pixel_format if set,
	// otherwise start from get_pixel_format_used(); fall back to _pixel_format_default when no direct path exists.
	PIXEL_FORMAT pf_picked;
	if( _s_force_out_pixel_format != aaa::PIXEL_FORMAT_FORCE::DEFAULT )
		pf_picked = c_pixel_format::get_pixel_format_from_force( _s_force_out_pixel_format );
	else
		pf_picked =  c_pixel_format::get_default_format( _src_pixel_format );
	if( !c_pixel_format::is_move( pf_picked, _src_pixel_format ) )
		pf_picked = _pixel_format_default;
	set_flux_size_format_direct( get_flux_size_x(), get_flux_size_y(), pf_picked );

	_size_x		= _flux_size_x;
	_size_y		= _flux_size_y;
	_b_crop_h	= false;
	_b_crop_v	= false;

	if( _b_crop )
	{
		INT32	size_x	= _flux_size_x - _crop_left - _crop_right;
		INT32	size_y	= _flux_size_y - _crop_top - _crop_bottom;
		if( size_x > 0 )
		{
			_size_x		= size_x;
			_b_crop_h	= true;
		}
		if( size_y > 0 )
		{
			_size_y		= size_y;
			_b_crop_v	= true;
		}
	}
	if( _b_do_field_separation )
	{
		_size_y /= 2;
	}
//	set_bgr( c_pixel_format::is_bgr( _src_pixel_format ) );

	_image_flux_buffer->set_callback( is_frame_callback() );
	_image_flux_buffer->sync( get_size_x(), get_size_y(), get_pixel_format_used() );

	//DBG_HEAP_IS_CORRUPT();

	TBUF_ADD( tbuf::CH_CAPTURE_CALLBACK, 2.0, "get_lock_a" );
	if( texture_flux_master->is_timing() )
		aaa::time::store( &_time_move_begin );
	st_img_conv options( get_size_x(), get_size_y() );
	options.signature		= signature;
	options.b_force_alpha	= b_force_alpha;
	options.alpha_fp32		= alpha_value;

	if( tex_video->get_image_flux(0) == this )
	{
		c_incrust_process* incrust = get_tex_video()->get_incrust();
		bool b_incrust = incrust->_b_active_ui;
		options.b_lut_active = b_incrust;
		if( incrust->_b_active_ui )
		{
			incrust->update();
			options.lut = incrust->get_lut();
			options.b_force_alpha = false;
		}			
	}
	//todo unify to do it with with the luma lut
	if( _b_threshold )
	{
		options.b_threshold = true;
		options.threshold = _threshold;
	}
	else
		options.b_threshold = false;

	bool b_flip_vert = is_flip_vertical() != _b_src_y_inverted;
	if( c_pixel_format::is_y_inverted( _src_pixel_format ) )
		b_flip_vert = !b_flip_vert;
	options.b_flip_vert = b_flip_vert;

	options.b_swap_red_blue = is_swap_red_blue();
	options.disp_max = _disp_max;
	options.disp_min = _disp_min;
	options.luma_max = _luma_max;
	options.luma_min = _luma_min;

	options.src_pixel_format = _src_pixel_format;

	if( c_img_2d* pa = _image_flux_buffer->get_free_image_locked() )
	{
		if( _b_do_field_separation )
		{
			TBUF_ADD( tbuf::CH_CAPTURE_CALLBACK, 2.5, "get_lock_b" );
			if( c_img_2d* pb = _image_flux_buffer->get_free_image_locked() )
			{
				copy_frame_to_img( pb, src, src_pitch, options, false );
				if( is_frame_callback() )
					pb->unlock();
			}
		}
		TBUF_ADD( tbuf::CH_CAPTURE_CALLBACK, 3.0, "move" );

		//GOOD_PRINT_STRING( "image flux %d before copy_frame_to_img.", _image_flux_obj_index );
		copy_frame_to_img( pa, src, src_pitch, options, true );

		//GOOD_PRINT_STRING( "image flux %d after copy_frame_to_img.", _image_flux_obj_index );
		if( is_frame_callback() )
			pa->unlock();	// in the interlace case the test was added by maa 13 Nov 2006. why it wasn't there ? crash bug ?
	}
	if( texture_flux_master->is_timing() )
	{
		aaa::time::store( &_time_move_end );
		DBG_PRINT_STRING( "Frame conversion for %s (%s) : %d us", signature, _flux_name.get(), aaa::time::get_interval_micro_sec( _time_move_end, _time_move_begin ) );
	}
	_speed_fps->end();

	//DBG_HEAP_IS_CORRUPT();

	TBUF_ADD( tbuf::CH_CAPTURE_CALLBACK, 0., "done" );
	SPY_POP_RANGE();
}


REAL	c_image_flux::get_fps()	CONST
{
	return _speed_fps->get_fps_last();
}


//	bit_align_p2 is the power of 2 to use (the shift)
static	INT32	compute_src_byte_per_line( INT32 sx, INT32 bit_per_pixel )
{
	INT32	bit_align_p2	= tex_video_master->is_src_aligned_4() ? 5 : 3;
	UINT32	mask			= (1<<bit_align_p2) - 1;
	sx *= bit_per_pixel;
	if( sx & mask )
	{
		sx &= ~mask;
		sx += 1<<bit_align_p2;
	}
	return sx >> 3;
}

bool	c_image_flux::is_swap_red_blue() CONST
{
#if AAA_DEBUG() && false
	bool b_tm_bgr = tex_video_master->is_flip_bgr();
	bool b = b_tm_bgr || _b_bgr_flip;
	if( b )
		DBG_PRINT_STRING( "%s() bgr debug", __FUNCTION__ );
	return b;
#else
	return tex_video_master->is_swap_red_blue() != _b_swap_red_blue;
#endif
}

bool	c_image_flux::copy_frame_to_img( c_img_2d* CONST img_dst, UINT8 CONST * src8, UINT32 src8_pitch, st_img_conv & options, bool b_field_one )
{
	bool b_moved	= false;
//	_b_alpha_done	= false;
	//we start eventually serious processing here
	if( is_frame_callback() )
		g_compute_master->inc();

	//BEGIN
	if( !get_tex_video() )
		ERR_PRINT_STRING( "%s() _p_tex_video is NULL", __FUNCTION__ );
	//	this test should be ok but Maa want more security for houston
	//else if( get_size_x() > img_dst->get_size_x() || get_size_y() > img_dst->get_size_y() )
	else if( get_size_x() != img_dst->get_size_x() || get_size_y() != img_dst->get_size_y() )
	{
//		DBG_PRINT_STRING( "Houston we got a problem : %s() : src %d x %d when dst %d x %d", __FUNCTION__, get_size_x(), get_size_y(), img_dst->get_size_x(), img_dst->get_size_y() );
	}
	else
	{
		UINT8*	RESTRICT dst		= img_dst->get_data_uint8();
		INT32	dst_step			= img_dst->get_byte_pitch();
		INT32	src_bit_per_pixel	= c_pixel_format::get_bits_per_pixel( _src_pixel_format );

		if( src8_pitch == 0 )
			src8_pitch = compute_src_byte_per_line( get_flux_size_x(), src_bit_per_pixel );	//todo probably wrong for I420,NV12...
		options.src_a_pitch	= src8_pitch;

		if( options.b_force_alpha )
		{
			if( img_dst->has_alpha() )
			//todo all alpha format
				options.alpha = aaa::img::c_compo::to_uint8( options.alpha_fp32 );
			else
				options.b_force_alpha = false;
		}
		//todo move to caller
		if( _b_do_field_separation )
		{
			options.b_do_field    = true;
			options.b_field_one   = b_field_one ^ _b_field_flip_order;
		}

		//todo
		if( _b_crop )
		{
			if( _b_crop_v )
				src8 += src8_pitch * _crop_top;
			if( _b_crop_h )
				src8 += _crop_left * src_bit_per_pixel / 8;
		}

		b_moved = img_dst->copy_from_src( src8, src8_pitch, options );
	}

	if( b_moved )
	{
//		if( is_swap_red_blue() != img_dst->is_bgr() )
//		{
//#if AAA_DEBUG()
//			DBG_PRINT_STRING( "%s() bgr debug", __FUNCTION__ );
//#endif
//			img_dst->flip_bgr();
//		}
#if AAA_DEBUG()
	if( _pixel_format_used != img_dst->get_pixel_format() )
	{
		DBG_PRINT_STRING( "_pixel_format_used is %s and img_dst is %s", aaa::c_pixel_format::get_name(_pixel_format_used), aaa::c_pixel_format::get_name(img_dst->get_pixel_format()) );
		///debug_break( "This should not happen. Maa fucked it up." );
	}
	//_pixel_format_used = img_dst->get_pixel_format();
#endif
//	_pixel_format_used = img_dst->get_pixel_format();
//		//img_dst->set_bgr( is_bgr_flip() ? !img_dst->is_bgr() : img_dst->is_bgr() );

		set_changed();
		img_dst->erase_filename();
	}
	else
	{
		img_dst->set_empty();
//		ERR_PRINT_STRING( "%s(): AAASeed did not move this frame", __FUNCTION__ );
	}

	//END
	if( is_frame_callback() )
		g_compute_master->dec();

	return b_moved;
}

