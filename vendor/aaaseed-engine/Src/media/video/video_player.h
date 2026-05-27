
#ifdef AAA_VIDEO_PLAYER_H
#error "VIDEO_PLAYER_H included more than once."
#endif
#define AAA_VIDEO_PLAYER_H 1


#ifndef AAA_IMAGE_FLUX_H
#	include "image_flux.h"
#endif
#ifndef AAA_DELTA_T_H
#	include "time/delta_t.h"
#endif


namespace aaa
{
	enum class	MOVIE_LIB : INT32
	{
		MS_AVI = 0,	// Microsoft AVI
		MS_DS,		// Microsoft DirectShow
		QT,			// Apple Quicktime
		FFMPEG,		// FFMPEG
		IMGSEQ,		// Image Sequence
		//MF,		// Microsoft Media Foundation (Vista & 7)
		UNKNOWN,	// Unkown
		MAX_NB
	};
	enum class	MOVIE_TYPE : INT32
	{
		AVI = 0,
		RM,
		MPG,
		WMV,
		MOV,
		MKV,
		UNKNOWN,
	};
}

class	c_movie_player : public c_image_flux
{
public:
	static	C_PCHAR_C	type_str[ (INT32)aaa::MOVIE_LIB::MAX_NB ];
	static	CONST	INT32	PREROLL_NB_MAX = 8;
	static	void	c_init();
	static	void	c_deinit();

	static	bool	b_verbose;
	static	void	set_verbose( bool CONST in );
	static	void	flip_verbose();

	static	aaa::MOVIE_LIB		pick_type_from_ext(			C_PCHAR_C ext	);
	static	aaa::MOVIE_LIB		pick_type_from_name(		C_PCHAR_C fname	);
	static	aaa::MOVIE_TYPE		pick_kind_from_name(		C_PCHAR_C fname	);
	static	bool				is_sound_file_from_name(	C_PCHAR_C fname	);

private:
	o_str		_filename;
	o_str		_filename_open_and_valid;

	bool		_b_playing;
	bool		_b_started;
	bool		_b_loop;
	bool		_b_paused;

	bool		_b_audio_use;

protected:

	bool		_b_is_callback;
	bool		_b_callback_set;

//	bool		_b_streaming;	//todo Maa made it symetric but its role is strange
	//	act as "is running" flag but seems to double active and/or open flag
//	bool		_b_stream;

	INT32		_s_field;
//	INT32		_frame_captured_index;
//	INT32		_audio_captured_index;

//	bool		_b_force_rgb;

//	INT32		_frames_dropped;
	REAL		_duration;

	REAL		_fps;

	REAL		_time_video;

	INT32		_preroll_nb;

	c_delta_t	_delta_t;

	REAL		_volume;
	REAL		_pan;
	DOUBLE		_rate;
	DOUBLE		_rate_last;
	DOUBLE		_rate_change_time;

	INT32		_bind;

public:
	FINLINE	bool		is_playing() CONST		{ return _b_playing; }
	FINLINE	void		set_playing( bool b )	{ _b_playing = b; }

	FINLINE	bool		is_started() CONST		{ return _b_started; }
	FINLINE	void		set_started( bool b )	{ _b_started = b; }

	FINLINE	bool		is_loop() CONST			{ return _b_loop; }
	FINLINE	void		set_loop( bool b )		{ _b_loop = b; }

	FINLINE	bool		is_paused()	CONST		{ return _b_paused; }
	FINLINE	void		set_paused( bool b )	{ _b_paused = b; }

	FINLINE	bool		is_audio_use()	CONST	{ return _b_audio_use; }
	FINLINE	void		set_audio_use( bool b )	{ _b_audio_use = b; }

	static	void		MOVPLAY_PRINT_STRING( C_PCHAR_C fmt, ... );

	c_movie_player( c_image_flux_buffer* buf, bool b_callback = false );
	virtual	~c_movie_player();

	virtual	void		init_post_constructor() {};

	FINLINE	void		store_bind( INT32 CONST bind )		{	_bind = bind; }
	FINLINE	INT32		get_bind()	CONST					{	return _bind; }

	FINLINE	void		erase_filename()					{ _filename.erase(); }
	FINLINE	void		set_filename( C_PCHAR_C filename )	{ _filename.set( filename ); }
	FINLINE	C_PCHAR_C	get_filename()	CONST				{ return _filename.get(); }
	FINLINE	bool		is_filename()	CONST				{ return !_filename.is_empty(); }


	virtual	AAA_ERR	open_specific()						=	0;
			bool	open( C_PCHAR_C filename );
			bool	open( C_PCHAR_C filename, bool b_force, bool b_retry );

	virtual	void	close_specific()					=	0;
			void	close();

//	virtual	void	release()							=	0;
			void	reload();
			void	set_filename_full( C_PCHAR_C filename_full );

	virtual	aaa::MOVIE_LIB	get_type()					=	0;

//	virtual	bool	start( bool CONST b_stream_in, INT32 CONST millisec )	{}
	virtual	void	stop()								=	0;
	virtual	void	pause()								=	0;
//	virtual	void	start()								=	0;
	virtual	void	play()								=	0;

	virtual	void	update()								{}

	virtual	void	get_frame_data( INT32 CONST index )		{}
	virtual	INT32	get_frame_index_cur()					{	return 0;				}
	virtual	void	ask_frame()		{};
	FINLINE	bool	is_callback()							{	return _b_is_callback;	}

//			INT32	get_frames_dropped()					{	return _frames_dropped;	}

	FINLINE	REAL	get_duration()							{	return _duration;		}
	FINLINE	REAL	get_fps()								{	return _fps;			}
	FINLINE	void	set_fps( REAL CONST fps )				{	_fps = fps;				}

	//FINLINE	void	set_preroll( INT32 CONST in )		{	_preroll_nb = CLAMP( in, 0, PREROLL_NB_MAX );	}	//crash this don't take in account the fixed allocation
	//FINLINE	INT32	get_preroll()						{	return _preroll_nb;		}

	//todo this not use by win_avi for example
	virtual	void	set_position( REAL CONST in );
	virtual	REAL	get_position();

	FINLINE	REAL	get_volume()							{	return _volume;			}
	virtual	bool	set_volume( REAL CONST volume )			{	_volume  = volume;	return true;	}
	FINLINE	REAL	get_pan()								{	return _pan;			}
	virtual	bool	set_pan( REAL CONST pan )				{	_pan = pan;			return true;	}

	FINLINE	DOUBLE	get_rate()								{	return _rate;			}
	virtual	DOUBLE	set_rate( DOUBLE rate )					=	0;
};

