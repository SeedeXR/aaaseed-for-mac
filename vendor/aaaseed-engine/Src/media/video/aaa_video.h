
#ifdef AAA_AAA_VIDEO_H
#error "AAA_VIDEO_H included more than once."
#endif
#define AAA_AAA_VIDEO_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif

class c_bind;
extern	c_bind* g_video_bank_def;


//todonow should be dynamic
CONSTEXPR	INT32	MOVIE_MAX_NB = 512;

extern	AAA_ERR		video_init();
extern	void		video_deinit();
extern	void		video_update();

extern	INT32		movie_build_bind_menu();
extern	C_PCHAR_C	video_get_bind_filename(	INT32 index );
extern	void		video_set_bind_name(						o_str CONST & filename_in );
extern	void		video_set_bind_name(		INT32 index,	o_str CONST & filename_in );


class	c_tex_video;
class	c_movie_player;

class	c_movie_avi;
extern	c_movie_avi*		movie_get_avi(		c_tex_video* tex_video, INT32 key );

// DirectShow
class	c_movie_ds;
extern	c_movie_ds*			movie_get_ds(		c_tex_video* tex_video, INT32 key );

// QuickTime
class	c_movie_qt;
extern	c_movie_qt*			movie_get_qt(		c_tex_video* tex_video, INT32 key );

class	c_movie_ffmpeg;
extern	c_movie_ffmpeg*		movie_get_ffmpeg(	c_tex_video* tex_video, INT32 key );

class	c_movie_img_seq;
extern	c_movie_img_seq*	movie_get_img_seq(	c_tex_video* tex_video, INT32 key );

// Media Foundation (not sure we ever had a c_movie_mf))
class	c_movie_mf;
extern	c_movie_mf*		movie_get_mf(		c_tex_video* tex_video );


extern	void				movie_release(		c_tex_video* tex_video, c_movie_player* movie_player );
extern	void				tex_video_release(  c_tex_video* tex_video );
