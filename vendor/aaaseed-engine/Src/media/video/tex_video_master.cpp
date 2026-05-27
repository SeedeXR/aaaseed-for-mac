#include "tex_video_master.h"
#include "infrastructure/param/param_declare.h"
#ifndef AAA_CAPTURE_VIDEO_H
#	include "media/video/capture/capture_video.h"
#endif

FACTORY_CREATE_V1( c_tex_video_master, tex_video_master, Texture Video Master, tex_video_master );

namespace n_tex_video_master
{
	CONSTEXPR INT32 BASE_PARAM_NB	= 12;
	CONSTEXPR INT32 GROUP_NB		= 0;

	CONSTEXPR INT32	PARAM_NB_MAX	=	BASE_PARAM_NB
									+	GROUP_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_BOOL_ON(			active					)
		PARAM_DEF_BOOL_OFF(			force_keep_on_cpu		)
		PARAM_DEF_BOOL_OFF(			swap_red_blue			)
		PARAM_DEF_BOOL_ON(			src_aligned_4			)
		PARAM_DEF_BOOL_OFF(			force_format_always		)
		PARAM_DEF_SYMBO_PSTR_ZERO(	force_format,			aaa::c_pixel_format::force_pixel_format_str )
		PARAM_DEF_BOOL_OFF(			verbose_capture			)
		PARAM_DEF_BOOL_ON(			audio_allow				)
		//PARAM_DEF_SYMBO(			reader_mp4,				2,1,	c_vid_play::TYPE_MAX_NB-1, c_vid_play::type_str )
		PARAM_DEF_SYMBO_MIN_MAX(	reader_mp4,				2,1,	1,2,	c_movie_player::type_str )
		PARAM_DEF_SYMBO_MIN_MAX(	reader_mpg,				2,1,	1,4,	c_movie_player::type_str )
		PARAM_DEF_SYMBO_MIN_MAX(	reader_mov,				2,1,	1,2,	c_movie_player::type_str )
	//	{	nullptr,	PARAM_SYMBOLIC,	"reader_mp4",		2,1,	1,2,	nullptr, c_movie_player::type_str	},
	//	{	nullptr,	PARAM_SYMBOLIC,	"reader_mpg",		2,1,	1,4,	nullptr, c_movie_player::type_str	},
		//PARAM_DEF_BOOL_ON(		force_alpha )
		//PARAM_DEF_REAL_ZERO_ONE(	force_alpha_value )
		PARAM_DEF_BOOL_OFF(			timing_print )
	};
}

void	c_tex_video_master::param_init_pt()
{
	INT32	h = 0;

	param_set_pt( h, get_pt_active()			);
	param_set_pt( h, _b_force_keep_on_cpu_ui	);
	param_set_pt( h, _b_swap_red_blue_ui		);
	param_set_pt( h, _b_src_aligned_4_ui		);
	param_set_pt( h, _b_force_format_always_ui	);
	param_set_pt( h, _s_force_format_ui			);
	param_set_pt( h,  c_capture::b_verbose		);
	param_set_pt( h, _b_audio_allow_ui			);
	param_set_pt( h, _s_reader_mp4_ui			);
	param_set_pt( h, _s_reader_mpg_ui			);
	param_set_pt( h, _s_reader_mov_ui			);
	//param_set_pt( h, _b_force_alpha			);
	//param_set_pt( h, _alpha_value_ui			);
	param_set_pt( h, _b_timing_ui				);

	err_param_init_pt( h );
}

CONSTRUCTOR_CREATE( c_tex_video_master )
{
	param_init_with( n_tex_video_master::param, n_tex_video_master::PARAM_NB_MAX );
}

EMPTY_DESTRUCTOR( c_tex_video_master )

aaa::MOVIE_LIB	c_tex_video_master::get_reader_mp4()
{
	return _s_reader_mp4_ui;
}
aaa::MOVIE_LIB	c_tex_video_master::get_reader_mpg()
{
	return _s_reader_mpg_ui;
}
aaa::MOVIE_LIB	c_tex_video_master::get_reader_mov()
{
	return _s_reader_mov_ui;
}
//void	c_tex_video_master::update()	{}

c_tex_video_master*	tex_video_master = nullptr;
