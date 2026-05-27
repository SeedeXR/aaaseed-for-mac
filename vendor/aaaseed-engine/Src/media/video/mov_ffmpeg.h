
#ifdef AAA_MOV_FFMPEG_H
#error "MOV_FFMPEG_H included more than once."
#endif
#define AAA_MOV_FFMPEG_H 1


#ifndef AAA_WRAP_FFMPEG_H
#	include "lib_wrappers/wrap_ffmpeg.h"
#endif

#if AAA_USE_FFMPEG()

#define AAA_MOV_FFMPEG() AAA_USE_FFMPEG()

#ifndef AAA_VIDEO_PLAYER_H
#	include "video_player.h"
#endif

#if AAA_MOV_FFMPEG()
#ifndef __STDC_CONSTANT_MACROS
#  define __STDC_CONSTANT_MACROS
#endif
#	ifndef AVUTIL_PIXFMT_H
#		include "ffmpeg/libavutil/pixfmt.h"
#	endif
	struct	AVFormatContext;
	struct	AVCodecContext;
	struct	AVCodec;
	struct	AVFrame;
	struct	SwsContext;
	struct	AVPacket;
#endif
#define UINT64_C(val) val##ui64
#ifndef AVCODEC_AVCODEC_H
#	include "ffmpeg/libavcodec/avcodec.h"
#endif
#ifndef AVFORMAT_AVFORMAT_H
#	include "ffmpeg/libavformat/avformat.h"
#endif
#ifndef SWSCALE_SWSCALE_H
#	include "ffmpeg/libswscale/swscale.h"
#endif

class	c_thread_ffmpeg;

class	c_movie_ffmpeg final : public c_movie_player
{
public:
	static	bool		b_dll_loaded;
protected:
	c_thread_ffmpeg*	_thread_ffmpeg;

	bool				_b_update_trig;

	REAL				_p_length;
	INT32				_video_stream;

	REAL				_time_pos;
#if AAA_MOV_FFMPEG()
	AVFormatContext*	_p_Format_Ctx;
	AVCodecContext*		_p_Codec_Ctx;
	AVCodec*			_p_Codec;
	AVFrame*			_p_Frame; 
	AVFrame*			_p_Frame_RGB;
	SwsContext*			_img_convert_ctx;
	AVPacket*			_packet;
	UINT8*				_buffer;
//	AVPixelFormat		_pix_fmt;
	PixelFormat			_pix_fmt;
#endif
	double				_original_pts;
	double				_frame_delay;

	mutable aaa::MUTEX*	_lock_ffmpeg;
	//unused	bool				_b_close_thread;
protected:
#if AAA_MOV_FFMPEG()
			void	init_thread();
			void	close_thread();
			void	start_loop_read();
#endif
public:
	c_movie_ffmpeg( c_image_flux_buffer* buf );
	virtual	~c_movie_ffmpeg();

#if AAA_MOV_FFMPEG()
	static INT32	lib_open_count;
	static	void	lib_open();
	static	void	lib_close();
	static	void	print_av_error( int av_err );
#endif

	virtual	AAA_ERR	open_specific();
	virtual	void	close_specific();
	virtual	void	init_post_constructor();
	virtual	aaa::MOVIE_LIB	get_type();

	virtual	void	play();
	virtual	void	pause();
	virtual	void	stop();
	virtual	void	update();
//	virtual	void	start();

//			void	reload( bool b_rgb32, bool b_audio, bool b_retry )	{};
//			void	loop_read();
//			void	loop_read2();
	virtual	void	update_async();

//			bool	is_use_audio()		{ return false; };
	virtual	void	set_position( REAL CONST time );
	virtual	REAL	get_position();
			REAL	get_volume();
	virtual	bool	set_volume( REAL CONST volume );
			REAL	get_pan()	{};
	virtual	bool	set_pan( REAL CONST pan );

	virtual DOUBLE	set_rate( DOUBLE rate );

//	virtual	INT32	get_bit_depth()	{ return 0; }
};

#endif //#if AAA_USE_FFMPEG()