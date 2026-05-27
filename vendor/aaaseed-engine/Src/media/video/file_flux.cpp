#include "file_flux.h"
#include "media/video/tex_video.h"
#include "spy.h"


#ifndef AAA_AAA_THREAD_H
#	include "Thread/aaa_thread.h"
#endif

class c_thread_img_seq : public c_thread
{
public:
	virtual	void	run()	{	run_it< c_movie_img_seq, -1 > ();	}
	c_thread_img_seq() : c_thread( "img_seq" )	{}
};


static	CONST	INT32	ASKED_FRAME_UNVALID = -12;

c_movie_img_seq::c_movie_img_seq( c_image_flux_buffer* buf )
	:c_movie_player				(	buf, true	)
	,_frame_index				(	ASKED_FRAME_UNVALID		)
	,_last_frame_index			(	ASKED_FRAME_UNVALID		)
	,_b_back					(	false		)
	,_image						(	nullptr		)
	,_thread_img_seq			(	nullptr		)
	,_first_frame_nb			(	0			)
	,_frames_nb					(	0			)
	,_index_first				(	0			)
	,_index_length				(	0			)
	,_nb_bits_out				(	0			)
	,_preroll_index				(	nullptr		)
	,_preroll_loaded			(	nullptr		)
	,_preroll_moved				(	nullptr		)
{
//	_lock_fileflux = new aaa::MUTEX;
	_thread_img_seq = new c_thread_img_seq;
	//set_swap_red_blue( true );		// from image flux
	set_fps( 25 );
	_preroll_nb		= MAX( PREROLL_NB_MAX, 2 );	//todo crash at 0 or 1
	_preroll_index	= new INT32[ _preroll_nb ];
	_preroll_loaded	= new bool[ _preroll_nb ];
	_preroll_moved	= new bool[ _preroll_nb ];
	
	for ( INT32 i = 0; i < _preroll_nb; i++ )
	{
		_preroll_index[ i ]		= ASKED_FRAME_UNVALID;
		_preroll_loaded[ i ]	= false;
		_preroll_moved[ i ]		= false;
		_preroll_img.push_back( c_img_2d::create(__FUNCTION__) );
	}
	_thread_img_seq->create( this, 0 );
}

c_movie_img_seq::~c_movie_img_seq()
{
	_thread_img_seq->shutdown();
	_thread_img_seq->join();

	for( auto const & pt : _preroll_img )
		delete pt;
	_preroll_img.clear();

	close();
}

aaa::MOVIE_LIB	c_movie_img_seq::get_type()
{
	return aaa::MOVIE_LIB::IMGSEQ;
}

void	c_movie_img_seq::close_specific()
{
}

AAA_ERR	c_movie_img_seq::open_specific() 
{
	bool	b_exist;
	o_str	file_to_check;

	C_PCHAR_C filename = get_filename();
	// file exist, now try to find the last image_, so we now the duration
	// decompose filename into root, index, index length and extension
	fname::serie_split( filename, _root_filename, &_index_first, &_index_length, _ext );

	INT32	index = _index_first - 1;
	do
	{
		++index;
		// generate filename
		fname::serie_make( file_to_check, _root_filename, index, _index_length, _ext );
		// check if filename exist
		b_exist = c_file::is_exist( file_to_check );
	}
	while ( b_exist );

	_frames_nb = index - _index_first;
	_duration = _frames_nb / get_fps();

	// load first image to init pbo, because it makes an error when doing it from the thread
	fname::serie_make( file_to_check, _root_filename, _index_first, _index_length, _ext );
	if( c_file::is_exist( file_to_check ) )
	{
		c_img_2d* image = c_img_2d::create( __FUNCTION__ );
		image = c_img_2d::read( image, file_to_check );
		if( image )
		{
			aaa::PIXEL_FORMAT	format = aaa::PIXEL_FORMAT::UNKNOWN;
			switch( image->get_pixel_format() )
			{
			case aaa::PIXEL_FORMAT::R_8:	format = aaa::PIXEL_FORMAT::R_8;		break;
			case aaa::PIXEL_FORMAT::R_16:	format = aaa::PIXEL_FORMAT::R_16;		break;
			case aaa::PIXEL_FORMAT::RGB_8:	format = aaa::PIXEL_FORMAT::RGB_8;		break;
			case aaa::PIXEL_FORMAT::RGBA_8:	format = aaa::PIXEL_FORMAT::RGBA_8;		break;
			}
		//	set_src_bit_per_pixel( _nb_bits_out = image->get_byte_per_pixel() * 8 );
		//	_b_src_grey = image->get_channel_nb() == 1 ;
			set_flux_size_format( image->get_size_x(), image->get_size_y(), format );	//dummy alloc to make sure the memory type is ok (mainly for pbo)
			delete image;
		}
		else
		{
			set_flux_size_format( 64, 64, aaa::PIXEL_FORMAT::RGBA_8 );
		}
	}
	else
	{
		set_flux_size_format( 32, 32, aaa::PIXEL_FORMAT::R_8 );	//dummy alloc to make sure the memory type is ok (mainly for pbo)
	}
	_thread_img_seq->start();
//		mem::DBG_CHECK_HEAP();
	return AAA_OK;
}

bool	c_movie_img_seq::got_frame_data( INT32 frame_index, INT32 preroll_index )
{
	bool	b_ret = false;
	// make filename using current index
	fname::serie_make( _file_to_check, _root_filename, frame_index, _index_length, _ext );

	//	mem::DBG_CHECK_HEAP();
	// check filename
	bool b_exist = c_file::is_exist( _file_to_check );
	if( b_exist )
	{
		c_img_2d* image = _preroll_img[ preroll_index ];
		if( image )
		{
			// read image from disk
			image = c_img_2d::read( image, _file_to_check );
			b_ret = true;
		}
	}
	//	mem::DBG_CHECK_HEAP();
	return b_ret;
}

void	c_movie_img_seq::update()
{
	_delta_t.update();
	if( is_valid() )
	{
		if( is_playing() )
		{
			_time_video = REAL( _time_video + _delta_t.get_dt() * _rate );
		}
		_duration = _frames_nb / get_fps();
		if( is_loop() )
		{
			_time_video = FWRAP( _time_video, _duration );
		}
		_frame_index = _index_first + INT32( _time_video * get_fps() );
	}
}

REAL	c_movie_img_seq::get_position()				{	return _time_video;		}
void	c_movie_img_seq::set_position( REAL time )	{	_time_video = time;		}
void	c_movie_img_seq::stop()						{	set_playing( false );	}
//void	c_movie_img_seq::start()					{	set_playing( true );	}
void	c_movie_img_seq::play()						{	set_playing( true );	}
void	c_movie_img_seq::pause()					{	stop();					}
/*
//todo check this is no more use 
void	c_movie_img_seq::restart()
{
	_time_video = _time_offset;
}
*/

void	c_movie_img_seq::load_frame( INT32 frame_index, INT32 i )
{
//	GOOD_PRINT_STRING( "load_frame %d at pos %d", frame_index, i );
	if( got_frame_data( frame_index, i ) )
	{
		_preroll_loaded[ i ] = true;
		_preroll_index[ i ] = frame_index;
	}
}

void	c_movie_img_seq::move_frame( INT32 i )
{
	c_img_2d*	image = _preroll_img[i];
	CONST UINT8*	src = image->get_data_uint8();
	if( src )
	{
		//_src_bit_per_pixel = _nb_bits_out = image->get_byte_per_pixel() * 8;
		//_b_src_grey = image->get_channel_nb()==1 ;
		//init_with_size( image->get_size_x(), image->get_size_y(), 4, __FUNCTION__ );
		got_frame( src, "image sequence" );
		_preroll_moved[ i ] = true;
	}
}

void	c_movie_img_seq::update_async()
{
	if( is_valid() )
	{
		//check index frame
		bool	b_found = false;
		INT32	frame_index = CLAMP( _frame_index, (INT32)_index_first, (INT32)(_frames_nb + _index_first) );
		//GOOD_PRINT_STRING( "Frame asked %d : %d", _frame_asked, frame_index );
		if( _preroll_nb == 0 )
		{
			if( _preroll_index[ 0 ] != frame_index )
			{
				load_frame( frame_index, 0 );
				move_frame( 0 );
			}
		}
		else
		{
			if( frame_index != _last_frame_index )
			{
				if( _last_frame_index > frame_index )
				{
					if( _last_frame_index != _frames_nb + _index_first )
						_b_back = true;
					else
						_b_back = false;
				}
				else
				{
					if( _last_frame_index == _index_first && frame_index == _frames_nb + _index_first )
						_b_back = true;
					else
						_b_back = false;
				}
				_last_frame_index = frame_index;
			}
			for( INT32 i = 0; i < _preroll_nb; ++i )
			{
				//GOOD_PRINT_STRING( "preroll %d :%d", i, _preroll_index[ i ]);
				if( _preroll_index[ i ] == frame_index )
				{
					b_found = true;
					if( !_preroll_loaded[ i ] )
					{
						load_frame( frame_index, i );
						move_frame( i );
					}
					else if( !_preroll_moved[ i ] )
						move_frame( i );
					break;
				}
			}
			if( !b_found )
			{
				for( INT32 i = 0; i < _preroll_nb; ++i )
				{
					//GOOD_PRINT_STRING( "preroll %d :%d", i, _preroll_index[ i ]);
					if( !_preroll_loaded[ i ] )
					{
						load_frame( frame_index, i );
						move_frame( i );
						b_found = true;
						break;
					}
					//else
					//{
					//	_preroll_loaded[ i ] = false;
					//	_preroll_moved[ i ] = false;
					//}
				}
				if( !b_found )
				{
					load_frame( frame_index, 0 );
					move_frame( 0 );
					for( INT32 i = 1; i < _preroll_nb; ++i )
					{
						_preroll_loaded[ i ] = false;
						_preroll_moved[ i ] = false;
					}
				}
				//_lock_fileflux->unlock();
					//reset preroll
			}
				// todoqq  does not work when playing backward
			INT32	delta = 1;
			if( _b_back )
			{
				delta = -1;
			}
			INT32	new_frame_index = WRAP_ID( frame_index + delta, _index_first, _frames_nb + _index_first );
			for( INT32 i = 0; i < _preroll_nb; ++i )
			{
				if( _b_back )
				{
					if( _preroll_index[ i ] != frame_index && ( _preroll_index[ i ] > new_frame_index || _preroll_index[ i ] <= frame_index - _preroll_nb ) )
					{
						_preroll_moved[ i ] = false;
						_preroll_loaded[ i ] = false;
					}
				}
				else
				{
					if( _preroll_index[ i ] != frame_index && ( _preroll_index[ i ] < new_frame_index || _preroll_index[ i ] >= frame_index + _preroll_nb ) )
					{
						_preroll_moved[ i ] = false;
						_preroll_loaded[ i ] = false;
					}
				}
				if( !_preroll_loaded[ i ] )
				{
					load_frame( new_frame_index, i );
					_preroll_moved[ i ] = false;
					new_frame_index = WRAP_ID( new_frame_index + delta, _index_first, _frames_nb + _index_first );
				}
			}
		}
		spy::sleep( 0, "sleep c_movie_img_seq::update_async() valid" );
	}
	else
	{
		spy::sleep( 100, "sleep c_movie_img_seq::update_async() invalid" );
	}
}
