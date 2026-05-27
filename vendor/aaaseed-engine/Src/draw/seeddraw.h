
#ifdef AAA_SEEDDRAW_H
#error "SEEDDRAW_H included more than once."
#endif
#define AAA_SEEDDRAW_H 1


#ifndef AAA_AAA_STR_H
#	include "aaa_str.h"
#endif

class	c_obj_ui;
class	c_seedcam;

class	draw
{
public:
	static	bool	b_multiple_draw_allow;
	static	bool	b_clean_focus;
	static	bool	b_clean_render;
	static	bool	b_clean_render_menu;
	static	bool	b_clean_render_from_right;
	static	bool	b_clean_render_from_top;
	static	INT32	clean_render_x_offset;
	static	INT32	clean_render_y_offset;

	static	INT32	frame_index;

private:
	static	bool	b_swap_buffer_ui;
	static	bool	b_swap_buffer;
	static	bool	no_erase_force_no_swap;

public:
	static FINLINE	bool	is_swap_buffer()			{ return b_swap_buffer;				}
	static FINLINE	void	set_swap_buffer(bool b)		{ b_swap_buffer = b;				}

	static	void	flip_clean_render();
private:
	static	bool	b_render_first_pass;	//todo perhaps not the right place still

public:
	enum WHERE : INT32
	{
		WHERE_NONE = 0,
		WHERE_PERSPECTIVE = 1,
		WHERE_ORTHO = 2
	};

	static	void	c_init();
	static	void	c_deinit();

	static	void	render();
	static	WHERE	mouse_is_where( INT32 x, INT32 y );

	static	FINLINE	INT32	get_frame()						{ return frame_index;				}
	static			void	inc_frame();

	static	void	speed_render_swap_end();
	static	bool*	get_swap_buffer_ui_pt()					{ return &b_swap_buffer_ui;			}
	static	bool*	get_no_erase_force_no_swap_pt()			{ return &no_erase_force_no_swap;	}

	static	void	set_stencil( bool in );
	static	bool	is_rendering();
	static	REAL	get_timing_fps_average();
	static	REAL	get_timing_fps_swap_average();

	static	void	speed_print_flip();
	static	void	speed_render_flip();

	static	void	param_add_pt( c_obj_ui* caller, INT32& h );

	static	void	rendering_set_ortho_with_factor( INT32 CONST factor );
	static	void	do_timings();

	static	FINLINE	bool is_render_first_pass()				{ return b_render_first_pass;		}
	static	FINLINE	void set_render_first_pass( bool in )	{ b_render_first_pass = in;			}

	static	INT32	view_axe_cur;
	static	FINLINE	INT32	get_view()						{	return view_axe_cur;			}
	static	void	set_view( INT32 view_axe );

	static	bool	gb_overlap_active;
private:
	static	void	view_update();
	static	void	render_field();
	static	void	render_before();
	static	void	render_central();
	static	void	render_after();
public:

};


extern	void		mouse_convert_xy_pixel_to_render( INT32 CONST px, INT32 CONST py, FP32& rx, FP32& ry );

extern	void		retrace_check_flip();

extern	void		retrace_check_start();
extern	void		test_swap_buffer();

extern	void		reset_erased_screens();
extern	void		draw_stencil();

extern	void		build_snap_filename( o_str& dst );

class	c_img_2d;
extern	c_img_2d*	get_img_frame_buffer(		INT32 which_buffer );
extern	void		save_frame_buffer_to_file(	C_PCHAR filename, INT32 which_buffer, bool b_verbose );
extern	void		print_frame_buffer(			INT32 which_buffer, bool b_verbose );

//extern	c_img_2d*	img_snapshot(									INT32 x, INT32 y, INT32 sx, INT32 sy, INT32 which_buffer );
//extern	void		img_snapshot_and_save(		C_PCHAR filename,	INT32 x, INT32 y, INT32 sx, INT32 sy, INT32 which_buffer, bool b_verbose );
//extern	void		img_snapshot_and_print(							INT32 x, INT32 y, INT32 sx, INT32 sy, INT32 which_buffer, bool b_verbose );

class c_shading;

class	c_draw_ui_guard final
{
private:
	c_shading*	_cur;
public:
	c_draw_ui_guard();
	~c_draw_ui_guard();
	C_NO_CPY_MOVE(c_draw_ui_guard);
};