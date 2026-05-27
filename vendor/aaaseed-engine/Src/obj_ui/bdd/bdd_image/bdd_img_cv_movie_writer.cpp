#include "bdd_img_cv_movie_writer.h"
#include "image/bind_img_2d.h" 
#include "time/aaa_time.h"	 


using duration = std::chrono::duration<float>;
using duration_ms = std::chrono::duration<float, std::milli>;

class c_thread_compress : public c_thread
{
public:
	virtual	void	run();
	c_thread_compress() : c_thread( "compress_thread_loop" )	{}
};

void	c_thread_compress::run()
{
	c_bdd_img_cv_mov_writer* l = (c_bdd_img_cv_mov_writer*)get_arg();
	if( !l )
		return;

	set_can_run( true );
	while( is_can_run() )
	{
		l->update_async();
	}

	l->stop();	//todoq symmetrize in the thread by integrating stop in it (look at policies >)
	//	l->dealloc_buffer();

}

C_PCHAR_C	c_bdd_img_cv_mov_writer::codec_str[ INT32(c_bdd_img_cv_mov_writer::CODEC::NB_MAX) ] =
{
	"dialog",
	"Uncompressed",
	"Y420",
	"Xvid",
	"MJPEG",
	"H264",
	"HuffYUV",
	"Flv"
};

//C_PCHAR_C	c_bdd_img_cv_writer::type_str[c_bdd_img_cv_writer::MOVIE_TYPE::NB_MAX] =
//{
//	"avi",
//	"mkv",
//	"mpg",
//	"flv"
//};

FACTORY_CREATE_PROP_V1( c_bdd_img_cv_mov_writer, bdd_img_cv_writer, Image OpenCV Video Writer, bdd_img_cv_writer, sub_menu = "Image"; );

namespace n_bdd_img_cv_movie_writer
{
	CONSTEXPR INT32 BASE_PARAM_NB	= 12 + c_bdd_img::SUPER_PARAM_NB;
	CONSTEXPR INT32 GROUP_PARAM_NB	= 0;
	CONSTEXPR INT32 PARAM_NB	= BASE_PARAM_NB
								+ GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB] =
	{
		BDD_IMG_BASE_PARAMS

		PARAM_DEF_BOOL_OFF(		run									)
		PARAM_DEF_BOOL_OFF(		verbose								)
		PARAM_DEF_FILENAME(		filename,	aaa::file::TYPE_IO_MOVIE_AVI, 0	)
	//	PARAM_DEF_SYMBO(		type,		1, 0,					(INT32)c_bdd_img_cv_mov_writer::MOVIE_TYPE::COUNT - 1,	c_bdd_img_cv_mov_writer::type_str )
		PARAM_DEF_SYMBO_PSTR(	codec,		4, 1,					c_bdd_img_cv_mov_writer::codec_str )
		PARAM_DEF_REAL_POS(		fps,		30., 25.				)
		PARAM_DEF_REAL(			quality,	75., 50., 0., 100.		)
//		PARAM_DEF_BOOL_OFF(		flip_horizontal						)
		PARAM_DEF_BOOL_OFF(		flip_vertical						)
		PARAM_DEF_BOOL_OFF(		flip_bgr							)
		PARAM_DEF_INT32_LOCKED(	frame_count							)
		PARAM_DEF_INT32_LOCKED(	frame_dropped						)
		PARAM_DEF_INT32_LOCKED(	buffer_free_count					)
		PARAM_DEF_INT32_LOCKED(	buffer_used_count					)
	};
}


void	c_bdd_img_cv_mov_writer::param_init_pt()
{
	INT32	h = SUPER::param_init_pt_start_src_resize( false, false );

	//param_set_pt( h, _b_movie_save_ui		);
	param_set_pt( h, _b_movie_run_ui		);
	param_set_pt( h, _b_verbose_ui			);
	param_set_pt( h, _movie_filename_ui		);
//	param_set_pt( h, _movie_type_ui			);
	param_set_pt( h, _movie_codec_ui		);
	param_set_pt( h, _fps_ui				);
	param_set_pt( h, _quality_ui			);
//	param_set_pt( h, _b_flip_h_ui			);
	param_set_pt( h, _b_flip_v_ui			);
	param_set_pt( h, _b_swap_red_blue_ui	);
	param_set_pt( h, _movie_frame_nb		);
	param_set_pt( h, _dropped_frame_nb		);
	param_set_pt( h, _buffer_free_count		);
	param_set_pt( h, _buffer_used_count		);

	err_param_init_pt( h );
}

CONSTRUCTOR_CREATE( c_bdd_img_cv_mov_writer )
	,_thread_compress	{nullptr}
	,_b_movie_run		{false}
	,_b_thread_closing	{false}
	,_movie_frame_nb	{0}
	,_dropped_frame_nb	{0}
	,_b_opened			{false}
//	,_image_nb_to_keep	{}
	,_lock_free			{nullptr}
	,_lock_used			{nullptr}
	,_img				{nullptr}
	,_speed_bdd			{nullptr}
	,_speed_bdd_ui		{nullptr}
	,_buffer_free_count	{0}
	,_buffer_used_count	{0}

{
	alloc_buffer();
	param_init_with( n_bdd_img_cv_movie_writer::param, n_bdd_img_cv_movie_writer::PARAM_NB );
}

c_bdd_img_cv_mov_writer::~c_bdd_img_cv_mov_writer()
{
	_b_movie_run = false;
	close_thread();
	close_video_writer();
	dealloc_buffer();
}

void	c_bdd_img_cv_mov_writer::close()
{
	stop();
}

void	c_bdd_img_cv_mov_writer::close_video_writer()
{
	if( _b_opened && _video_writer.isOpened() )
	{
		_b_opened = false;
		_video_writer.release();
	}
}

void	c_bdd_img_cv_mov_writer::stop()
{
	close_video_writer();
}

void	c_bdd_img_cv_mov_writer::update()
{
	_b_movie_run = _b_movie_run_ui;
	if( _b_movie_run )
	{
		_img = update_part_1();
		if( _img )
			do_process();
	}
}

void	c_bdd_img_cv_mov_writer::draw()
{
}

void	c_bdd_img_cv_mov_writer::alloc_buffer()
{
	for( auto i = 0; i < 4; ++i )
	{
		c_img_2d*	img = c_img_2d::create( __FUNCTION__ );
		_images_free.push_back( img );
	}
	_lock_free = new aaa::MUTEX;
	_lock_used = new aaa::MUTEX;
}

void	c_bdd_img_cv_mov_writer::dealloc_buffer()
{
	//if( !_images_free.empty() )
	{
		for( auto const & img : _images_free )
			delete img;
		_images_free.clear();
	}
	//if( !_images_used.empty() )
	{
		for( auto const & img : _images_used )
			delete img;
		_images_used.clear();
	}
}

void	c_bdd_img_cv_mov_writer::do_process()
{
	//UINT32	channel = 1;

	if( _images_free.empty() )
	{
		++_dropped_frame_nb;
		//ERR_PRINT_STRING( "No free image ready in %s()" );
	}
	else
	{
		void * src = _img->get_data();
		if( src )
		{
			auto now = std::chrono::system_clock::now();
			duration elapsed = now - _last_tick;
			if( elapsed.count() >= OVER_ONE( _fps_ui ) )
			{
				// we store the buffer needed in img 
				c_img_2d*	img_free;
				{
					std::lock_guard<aaa::MUTEX> guard(*_lock_free);
					img_free = *( _images_free.begin() );
					_images_free.pop_front();
				}

				// set size format
				INT32 sx,sy;
				_img->get_size_xy( sx,sy );
				//if( !img_free->is_size_format( sx,sy, aaa::PIXEL_FORMAT::RGB_8 ) )
				//	img_free->init_with_size( sx,sy, aaa::PIXEL_FORMAT::RGB_8, __FUNCTION__ );

				// copy from img to img_free
				// HACK : AAASeed doesn't convert to BGR directly, but it can convert to RGB,
				//	so we will trick the source image so the copy does the right conversion,
				//	OpenCV Movie Writer needs 3 channel image for using FFMPEG otherwise it doesn't work
				auto format_src = _img->get_pixel_format();
				if( _b_swap_red_blue_ui )
				{
					aaa::PIXEL_FORMAT format_new = aaa::c_pixel_format::get_format_red_blue_swapped( format_src );
					if( format_new != aaa::PIXEL_FORMAT::UNKNOWN )
						format_src = format_new;
					// don't reaaly know what happen if not format swapped exists
				}
				img_free->copy_from_src( src, _img->get_byte_pitch(), format_src,
												aaa::PIXEL_FORMAT::BGR_8, sx,sy,
												!_b_flip_v_ui );	//for vertical OpenCV is not the same as AAA/OpenGl
				//	img_free->copy_from_img( _img, false, _b_flip_v_ui, !_b_swap_red_blue_ui );
		
				{
					std::lock_guard<aaa::MUTEX> guard(*_lock_used);
					_images_used.push_back( img_free );
				}
				_last_tick = now;
			}
			else
			{
				if( _b_verbose_ui )
					DBG_PRINT_STRING( "%s() rendering faster than fps : Skipping frame", __FUNCTION__ );
			}
		}
		else
		{
			DBG_PRINT_STRING( "%s() No src img", __FUNCTION__ );
		}
	}
	init_thread();
	_buffer_free_count = (UINT32)_images_free.size();
	_buffer_used_count = (UINT32)_images_used.size();

}

AAA_ERR	c_bdd_img_cv_mov_writer::open( c_img_2d* src )
{
	// Setup output video
	//cv::VideoWriter output_cap( argv[2], input_cap.get( CV_CAP_PROP_FOURCC ), input_cap.get( CV_CAP_PROP_FPS ), cv::Size( input_cap.get( CV_CAP_PROP_FRAME_WIDTH ), input_cap.get( CV_CAP_PROP_FRAME_HEIGHT ) ) );

	if ( !src )
	{
		ERR_PRINT_STRING( "No source to open video file" );
		return ERR_ANY;
	}

	INT32 sx,sy;
	src->get_size_xy( sx, sy );

	//C++: bool VideoWriter::open( const string& filename, int fourcc, double fps, Size frameSize, bool isColor = true )


	_name.set( _movie_filename_ui );

	_dropped_frame_nb = 0;
	_movie_frame_nb = 0;

	//switch ( (MOVIE_TYPE)_movie_type_ui )
	//{
	//default:
	//case MOVIE_TYPE::MOVIE_AVI:		name.add_ext( "avi" );		break;
	//case MOVIE_TYPE::MOVIE_MKV:		name.add_ext( "mkv" );		break;
	//case MOVIE_TYPE::MOVIE_FLV:		name.add_ext( "flv" );		break;
	//case MOVIE_TYPE::MOVIE_MPG:		name.add_ext( "mpg" );		break;
	//}

	INT32	codec = -1;				// codec -1 prompt encoder dialog
		
	C_PCHAR ext = "avi";
	switch( (CODEC)_movie_codec_ui )
	{
	case CODEC::RAW:
		codec = cv::VideoWriter::fourcc( 'm', 'r', 'l', 'e' );
		break;
	case CODEC::XVID:
		codec = cv::VideoWriter::fourcc( 'x', 'v', 'i', 'd' );
		ext = "mkv";
		break;
	case CODEC::MJPEG:
		codec = cv::VideoWriter::fourcc( 'M', 'J', 'P', 'G' );
		break;
	case CODEC::H264:
	//	codec = cv::VideoWriter::fourcc( 'X', '2', '6', '4' );	
		codec = cv::VideoWriter::fourcc( 'h', '2', '6', '4' );
		ext = "mp4"; // Use .mp4 for H264
		break;
	case CODEC::FLV:
		codec = cv::VideoWriter::fourcc( 'F', 'S', 'V', '1' );
		ext = "flv"; // Use .flv for FLV
		break;
	case CODEC::HUFFYUV:
		codec = cv::VideoWriter::fourcc( 'H', 'F', 'Y', 'U' );
//		ext = "yuv";
		break;
	case CODEC::I420:
		codec = cv::VideoWriter::fourcc( 'I', '4', '2', '0' );
		break;
	default:
		err_print( "Unsupported codec : %s", codec_str[_movie_codec_ui] );
		return ERR_ANY;
	}

	int backend = cv::CAP_ANY; // Default backend
	// Use FFmpeg backend for H264 and FLV
	//if( _movie_codec_ui == CODEC::H264 || _movie_codec_ui == CODEC::FLV)
	//	backend = cv::CAP_FFMPEG;
	//cv::CAP_ANY: Automatically select a backend.
	//cv::CAP_FFMPEG: Use FFmpeg backend (if available).
	//cv::CAP_OPENCV_MJPEG: Use OpenCV's built-in MJPEG codec.

	_name.add_ext( ext ); // we need the right extension or _video_writer.open fail
	cv::Size size = cv::Size( (int)sx, (int)sy );
	bool b_ret = _video_writer.open( _name.get(), cv::CAP_ANY, codec, _fps_ui, size, true );
	if( !b_ret )
	{
		err_print( "Oops not opening : could be a bad file name (seem to need .avi too) : %s", _name.get() );
	}
	if( _video_writer.isOpened() )
	{
		// Does not seems to have an effect with VFW encoders.
		_video_writer.set( cv::VIDEOWRITER_PROP_QUALITY, _quality_ui );
		_b_opened = true;
		return AAA_OK;
	}

	return ERR_ANY;
}

void	c_bdd_img_cv_mov_writer::update_async()
{
	while( !_images_used.empty() ) // we protect here from the thread freed but still running
	{
		c_img_2d*	img;
		{
			std::lock_guard<aaa::MUTEX> guard(*_lock_used);
			img = *(_images_used.begin());
			_images_used.pop_front();
		}

		if( _b_movie_run ) // we protect here from the thread freed but still running
		{
			aaa::c_cv::img_to_cv_mat( img, _mat_img );

			try
			{
				if( !_b_opened && !_video_writer.isOpened() )
				{
					open( img );
				}
				if( _b_opened && _video_writer.isOpened() )
				{
					_video_writer.write( _mat_img );
					++_movie_frame_nb;
				}
				else
				{
					ERR_PRINT_STRING( "Movie not opened in %s()", __FUNCTION__ );
				}
			}

			catch( cv::Exception& e )
			{
				err_print( "Open CV Exception in %s(): %s", __FUNCTION__, e.what() );
			}	
		}
		
		{
			std::lock_guard<aaa::MUTEX> guard(*_lock_free);
			_images_free.push_back( img );
		}

			//if( _b_thread_closing )
			//	return;	// true;
	}
	
	if( !_b_movie_run )
		close_video_writer();
}

void	c_bdd_img_cv_mov_writer::init_thread()
{
	if( IS_NULL( _thread_compress ) )
	{
		_thread_compress = new c_thread_compress;
		_thread_compress->create( this, 0 );
		_thread_compress->start();
	}
	else
		_thread_compress->start();
}

void	c_bdd_img_cv_mov_writer::close_thread()
{
	if( !_b_thread_closing && IS_NOT_NULL( _thread_compress ) )
	{
		_b_thread_closing = true;
		_thread_compress->shutdown();
		_thread_compress->join();
		SAFE_DELETE( _thread_compress );
		_b_thread_closing = false;
	}

}

