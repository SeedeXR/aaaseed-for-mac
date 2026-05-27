
#ifdef AAA_TEX_VIDEO_MASTER_H
#error "TEX_VIDEO_MASTER_H included more than once."
#endif
#define AAA_TEX_VIDEO_MASTER_H 1


#ifndef	AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif
#ifndef	AAA_VIDEO_PLAYER_H
#	include "video_player.h"
#endif

class	c_tex_video_master final : public c_obj_active_ui
{
	FACTORY_DECLARE( c_tex_video_master, c_obj_active_ui );
private:
	aaa::PIXEL_FORMAT_FORCE	_s_force_format_ui;
	bool					_b_force_keep_on_cpu_ui;
	bool					_b_swap_red_blue_ui;
	bool					_b_force_format_always_ui;
	bool					_b_audio_allow_ui;
	aaa::MOVIE_LIB			_s_reader_mp4_ui;
	aaa::MOVIE_LIB			_s_reader_mpg_ui;
	aaa::MOVIE_LIB			_s_reader_mov_ui;
	bool					_b_src_aligned_4_ui;
	bool					_b_timing_ui;
	//bool					_b_force_alpha;
	//REAL					_alpha_value_ui;
public:
	FINLINE	bool		is_audio_allow()				CONST	{ return _b_audio_allow_ui; }
	FINLINE	void		set_audio_allow( bool CONST b )			{ _b_audio_allow_ui = b; }

	FINLINE	bool		is_src_aligned_4()				CONST	{ return _b_src_aligned_4_ui; }
	FINLINE	void		set_src_aligned_4( bool CONST b )		{ _b_src_aligned_4_ui = b; }

	//FINLINE	bool	is_force_alpha()				CONST	{	return _b_force_alpha;		}
	//FINLINE	REAL	get_force_alpha()						{	return _alpha_value_ui;		}

	FINLINE	bool		is_force_keep_on_cpu()			CONST	{	return _b_force_keep_on_cpu_ui;	}
	FINLINE	bool		is_swap_red_blue()				CONST	{	return _b_swap_red_blue_ui;		}
	FINLINE	bool		is_timing()						CONST	{	return _b_timing_ui;			}

	FINLINE	aaa::PIXEL_FORMAT_FORCE	get_force_format()	CONST	{	return _s_force_format_ui;		}
	//FINLINE	void				set_force_format( CONST aaa::PIXEL_FORMAT_FORCE format )	{	_s_force_format_ui = format;	}

	FINLINE	aaa::PIXEL_FORMAT_FORCE	compute_format_forced( CONST aaa::PIXEL_FORMAT_FORCE in )
	{
		if( _b_force_format_always_ui )
			return _s_force_format_ui != aaa::PIXEL_FORMAT_FORCE::DEFAULT ? _s_force_format_ui : in;
		return in == aaa::PIXEL_FORMAT_FORCE::DEFAULT ? _s_force_format_ui : in ;
	}

	virtual	void	param_init_pt();
	aaa::MOVIE_LIB	get_reader_mp4();
	aaa::MOVIE_LIB	get_reader_mpg();
	aaa::MOVIE_LIB	get_reader_mov();
};

extern	c_tex_video_master*	tex_video_master;
