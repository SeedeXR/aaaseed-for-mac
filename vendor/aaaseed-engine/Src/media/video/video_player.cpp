#include "video_player.h"
#include "tex_video.h"
#include "aaa_video.h"

#include "obj_ui/tracker/trackers.h"


//	VERBOSE
#define	VIDPLAY_HEADER  "# VIDEO PLAYER "
void	c_movie_player::MOVPLAY_PRINT_STRING( C_PCHAR_C fmt, ... )
{
	va_list args;
	va_start( args, fmt );
	trackers::PRINT_STRING_VA( VIDPLAY_HEADER, fmt, args );
	va_end( args );
}

bool	c_movie_player::b_verbose = true;

void c_movie_player::set_verbose( bool in )
{
	b_verbose = in;
	SWITCH_PRINT_STATE( "Movie Verbose", b_verbose );
}

void c_movie_player::flip_verbose()
{
	set_verbose( !b_verbose );
}

//
C_PCHAR_C	c_movie_player::type_str[ (INT32)aaa::MOVIE_LIB::MAX_NB ] =
{
	"AVIFile",
	"DirectShow",
	"Quicktime",
	"FFMPEG",
	"File Sequence",
//	"Media Foundation",
	"Unknown"
};

//todoqqq
//	add a video_master object
//		with param to control .mov and .mp4 case below

aaa::MOVIE_LIB	c_movie_player::pick_type_from_ext( C_PCHAR ext )
{
	if( ext )
	{
		if ( str_is_equal_nocase( ext, "mp4" ) )
			return tex_video_master->get_reader_mp4();
		else if ( str_is_equal_nocase( ext, "mpg" ) ||  str_is_equal_nocase( ext, "mpeg" ) )
			return tex_video_master->get_reader_mpg();
		else if ( str_is_equal_nocase( ext, "m4a" ) )
			return tex_video_master->get_reader_mp4();
		else if ( str_is_equal_nocase( ext, "mov" ) )
			return tex_video_master->get_reader_mov();
	//	return aaa::MOVIE_LIB::MS_DS;
	//	return aaa::MOVIE_LIB::QT;
		else if( c_img_utils::get_save_type_from_ext(ext) != c_img_utils::FILE_TYPE::DEFAULT )
			return aaa::MOVIE_LIB::IMGSEQ;
		else if ( str_is_equal_nocase( ext, "rm" ) )
			return aaa::MOVIE_LIB::FFMPEG;
	}
	return aaa::MOVIE_LIB::UNKNOWN;
}

aaa::MOVIE_LIB	c_movie_player::pick_type_from_name( C_PCHAR_C filename )
{
	C_PCHAR	ext	= fname::get_ext( filename );
	return pick_type_from_ext( ext );
}

//|| str_is_equal_nocase( ext, "m2t" )
aaa::MOVIE_TYPE	c_movie_player::pick_kind_from_name( C_PCHAR_C filename )
{
	C_PCHAR	ext	= fname::get_ext( filename );

	if( ext )
	{
		if	(		str_is_equal_nocase( ext, "mpeg" )
				||	str_is_equal_nocase( ext, "mpg" )
				||	str_is_equal_nocase( ext, "m2v" )
			)
			return aaa::MOVIE_TYPE::MPG;
		else if( str_is_equal_nocase( ext, "avi" ) )
			return aaa::MOVIE_TYPE::AVI;
		else if( str_is_equal_nocase( ext, "mkv" ) )
			return aaa::MOVIE_TYPE::MKV;
		else if( str_is_equal_nocase( ext, "wmv" ) || str_is_equal_nocase( ext, "wma" ) || str_is_equal_nocase( ext, "asf" ) )
			return aaa::MOVIE_TYPE::WMV;
	}
	return aaa::MOVIE_TYPE::UNKNOWN;
}

bool	c_movie_player::is_sound_file_from_name( C_PCHAR_C filename )
{
	C_PCHAR	ext	= fname::get_ext( filename );

	if( ext &&	(	str_is_equal_nocase( ext, "mp3" )
				|| str_is_equal_nocase( ext, "m4a" )
				|| str_is_equal_nocase( ext, "wave" )
				|| str_is_equal_nocase( ext, "flac" )
				)
		)
	{
		return true;
	}
	return false;
}

void	c_movie_player::c_init()	{}
void	c_movie_player::c_deinit()	{}

 bool	c_movie_player::open( C_PCHAR_C filename )
{
	close();
	set_filename_full( filename );
	bool b_good = false;
	if( !filename || *filename==0 )
	{
		ERR_PRINT_STRING( "C++ %s() : Can't open file because no name is specified", __FUNCTION__ );
		goto exit;
	}
	if( !c_file::is_exist( filename ) )
	{
		c_file::print_err( __FUNCTION__, filename, "don't exist" );
		goto exit;
	}
	// force so it will be reallyt set in update
	_volume = -42;
	_pan = -42;
	_rate_last = -42;
	b_good = NOERR(open_specific());

exit:
	if( b_good )
	{
		set_valid( true );
//		set_capturing( true );
		_filename_open_and_valid.set( filename );
	}
	else
		close();
	return b_good;
}

void	c_movie_player::close()
{
	stop();
	close_specific();
	set_valid( false );
//	set_capturing( false );
	_filename_open_and_valid.erase();
	set_started( false );
}

//todo finally removed
void	c_movie_player::stop()
{
	set_playing(false);
}

void	c_movie_player::reload()
{
	close();
	open( get_filename() );
}

void	c_movie_player::set_filename_full( C_PCHAR_C filename_full )
{
	set_flux_name( fname::get_fname( filename_full ) );
	set_filename( filename_full );
}

/*
void	c_movie_player::restart()
{
}
*/

bool	c_movie_player::open( C_PCHAR_C filename, bool b_force, bool b_retry )
{
	IF_THIS_NULL_RETURN_FALSE();
	if( b_force || !is_valid() )
	{
//		C_PCHAR_C filename	= movie_bind_get_name( bind );
//		if( str_is_diff( filename, _filename_open_and_valid.get() ) || get_tex_video()->is_retry() )
		if( str_is_diff( filename, _filename_open_and_valid.get() ) || b_retry )
		{
			//GOOD_PRINT_STRING( "%s last was %s", filename, _filename_open_and_valid.get() );
			if( open( filename ) )
			{
				//if( get_data_uint8() )	//todo this test is strange
				//{
				//}
				//else
				//{
				//	ERR_PRINT_STRING( "video buffers for %s didn't got allocated", filename );
				//	return;
				//}
				return true;
			}
		}
	}
	return is_valid();
}

void	c_movie_player::set_position( REAL in )
{
	debug_break( "%s() not implemented for this derived class", __FUNCTION__ );
}

REAL	c_movie_player::get_position()
{
	debug_break( "%s() not implemented for this derived class", __FUNCTION__ );
	return _time_video;
}


c_movie_player::c_movie_player( c_image_flux_buffer* buf, bool b_callback )
	:c_image_flux		(	buf, b_callback		)
	,_b_callback_set	(	false	)
	,_s_field			(	0		)
//	,_frames_dropped	(	-1		)
	,_time_video		(	.0		)
	,_b_audio_use		(	false	)
	,_b_playing			(	false	)
	,_b_paused			(	false	)
	,_rate				(	1.		)
	,_rate_last			(	0.		)
	,_rate_change_time	(	0.		)
	,_pan				(	.5f		)
	,_volume			(	1.0f	)
	,_duration			(	.0f		)
	,_b_loop			(	false	)
	,_b_started			(	false	)
	,_b_is_callback		(	false	)
//	,_b_force_rgb		(	false	)
	,_preroll_nb		(	0		)
{
}

c_movie_player::~c_movie_player()
{
	stop();
}
