
#ifdef AAA_AAASEED_H
#error "AAASEED_H included more than once."
#endif
#define AAA_AAASEED_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif

class c_node_ui;


extern	bool	gb_start_with_lenticular;
extern	bool	gb_restore_execution_state;
extern	bool	gb_log_file_open;
extern	bool	gb_callback_exit_exit;

extern	AAA_ERR	seed_do_main_part_1( INT32 CONST argc, char** argv );
extern	AAA_ERR	seed_do_main_part_2( INT32 CONST argc, char** argv );
extern	AAA_ERR	seed_do_main_part_3();
extern	void	seed_release();

extern	void	set_verbose_callback( bool in );
extern	void	flip_verbose_callback();

extern	bool	gb_verbose_callback;
extern	bool	gb_verbose_callback_display;
extern	bool	gb_verbose_callback_idle;
extern	bool	gb_verbose_callback_mouse;
extern	bool	gb_verbose_callback_mouse_move;


extern	bool	gb_force_screen_change;
extern	bool	gb_force_screen_size;
extern	INT32	g_force_screen_size_x;
extern	INT32	g_force_screen_size_y;
extern	bool	gb_force_screen_bits_per_pixel;
extern	INT32	g_force_screen_bits_per_pixel_nb;
extern	bool	gb_force_screen_frequency;
extern	INT32	g_force_screen_frequency;

extern	void	deal_with_net_events();
extern	void	do_global_idle();

namespace aaa
{
	CONSTEXPR CHAR	AAASEED_WINDOW_NAME[]		=	"AAASeed Plug In...";
	CONSTEXPR WCHAR	AAASEED_WINDOW_NAME_LONG[]	=	L"AAASeed Plug In...";

	extern	void	show_about();
	extern	void	show_license();
}	//namespace aaa

extern	c_node_ui* node_pref;


