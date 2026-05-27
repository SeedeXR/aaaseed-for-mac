
#ifdef AAA_FEEDBACK_H
#error "FEEDBACK_H included more than once."
#endif
#define AAA_FEEDBACK_H 1


#ifndef AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif

/*
class	c_feedback final : public c_obj_active_ui
{
	FACTORY_DECLARE( c_feedback, c_obj_active_ui );
public:
	static	c_feedback*	cur;
	static	c_feedback*	ui;
private:
	BOOL	b_grab_;
	INT32	s_grab_dst_;
	INT32	strobe_;
	INT32	strobe_count_;
	BOOL	b_draw_;
	INT32	draw_nb_x_;
	INT32	draw_nb_y_;
	REAL	draw_space_;
	REAL	draw_depth_ui_;
	REAL	draw_depth_;
	INT32	size_x_p2_;
	INT32	size_y_p2_;
	INT32	size_x_;
	INT32	size_y_;
//	used to optimize the feedback grab
	INT32	size_x_grabed_;
	INT32	size_y_grabed_;
	BOOL	b_alpha_grabed_;
	REAL	grab_pos_x_;
	REAL	grab_pos_y_;
	BOOL	b_grab_alpha_;
	INT32	tex_env_mode_;
	BOOL	b_blur_;
	BOOL	b_smooth_ui_;
	BOOL	b_smooth_;
	bool	b_generate_mipmap_;
	INT32	pass_nb_;
	INT32	blend_src_;
	INT32	blend_dst_;
	REAL	x_;
	REAL	y_;
	REAL	angle_;
	REAL	angle_bis_;
	REAL	scale_ui_[3];
	REAL	scale_[3];
	REAL	scale_factor_;
	REAL	red_;
	REAL	green_;
	REAL	blue_;
	REAL	grey_;
	REAL	alpha_;
	INT32	seed_;
	REAL	seed_depth_ui_;
	REAL	seed_depth_;
	BOOL	b_seed_smooth_;
	REAL	seed_width_;
	REAL	seed_color_[5];

protected:
			void	init();
public:

	virtual	void	update();
	virtual	void	draw();

	virtual	void	param_init_pt();

	virtual	AAA_ERR	load_do_after( CONST CHAR* CONST filename );

			void	grab();
			void	draw_square( REAL rot, REAL rot_bis, REAL size_x, REAL size_y, REAL size_z );

	FINLINE	REAL	get_angle() { return angle_; }	
	FINLINE	REAL	get_scale_factor() { return scale_factor_; }

	FINLINE	void	set_angle( REAL in )	{ angle_ = in; }
	FINLINE	void	set_scale_factor( REAL in )	{ scale_factor_ = in; }
};
*/

#endif	//  __FEEDBACK_H__
