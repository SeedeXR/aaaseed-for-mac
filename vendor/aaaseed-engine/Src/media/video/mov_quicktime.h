
#ifdef AAA_MOV_QUICKTIME_H
#error "MOV_QUICKTIME_H included more than once."
#endif
#define AAA_MOV_QUICKTIME_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif

#if AAA_WIN64()
#	define AAA_USE_QUICKTIME() 0
#else
#	define AAA_USE_QUICKTIME()	1
#endif

#ifndef AAA_VIDEO_PLAYER_H
#	include "video_player.h"
#endif

#if AAA_USE_QUICKTIME()
namespace	mov_quicktime
{
#	include <Quicktime/Movies.h>
}

////-------------------------- helpful for rgba->rgb conversion
//typedef struct{
//	unsigned char r; 
//	unsigned char g; 
//	unsigned char b;
//} pix24;
//


class	c_movie_qt final : public c_movie_player
{
public:
	static	bool				b_dll_loaded;
protected:
	void						createImgMemAndGWorld();

	mov_quicktime::GWorldPtr 	_offscreenGWorld;
	mov_quicktime::Movie		_movie;

	UINT8*						_offscreenGWorldPixels;	// 32 bit: argb (qt k32ARGBPixelFormat)\

	bool						_b_audio_state;
	bool						_b_need_refresh;
	long						_pos_last;
private:
	long						_qt_movie_duration;

	void		update_duration();
	void		set_position_low( REAL pos );

	mov_quicktime::MovieDrawingCompleteUPP	callback_function;
	void		setLoopState( INT32 state );
	void		set_rate_low( DOUBLE rate );

//	unsigned char * 	_pixels;					// 32 bit: rgb

public:
	c_movie_qt( c_image_flux_buffer* buf );
	virtual	~c_movie_qt();

	static	INT32	lib_open_count;
	static	void	lib_open();
	static	void	lib_close();
	virtual	void	init_post_constructor();

	virtual	aaa::MOVIE_LIB	get_type()			{ return aaa::MOVIE_LIB::QT; }

	virtual	AAA_ERR	open_specific();
	virtual	void	close_specific();

	virtual	void	play();
	virtual	void	pause();
	virtual	void	stop();
	virtual	void	update();
//	virtual	void	start();

	virtual	INT32	get_frame_index_cur();

	virtual	bool	is_use_audio()						{ return false; }
	virtual	void	set_position( REAL CONST time );
	virtual	REAL	get_position();
	virtual	bool	set_volume( REAL CONST volume );
	virtual	REAL	get_pan();
	virtual	bool	set_pan( REAL CONST pan );

	virtual	DOUBLE	set_rate( DOUBLE rate );
			void	update_pixels();

//	virtual	INT32	get_bit_depth()						{ return 0; }
};

#endif