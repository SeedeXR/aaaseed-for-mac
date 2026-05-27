
#ifdef AAA_BDD_FEEDBACK_H
#error "BDD_FEEDBACK_H included more than once."
#endif
#define AAA_BDD_FEEDBACK_H 1


#ifndef AAA_BDD_H
#	include "obj_ui/bdd/util/bdd.h"
#endif

class	c_bdd_feedback final : public c_bdd
{
	FACTORY_DECLARE(c_bdd_feedback,c_bdd);
private:
	static 	bool		b_allow;
	static	bool		b_grab_needed;
	static	INT32		grab_count;
public:
	static			void	reset_grab();
	static			void	set_grab_needed();
	static			bool	is_grab_ready();
	static	FINLINE	bool	is_allow()			{ return b_allow; }
	static	FINLINE	bool*	get_allow_pt()		{ return &b_allow; }
protected:
	bool	_b_grab_ui;
	INT32	_s_grab_dst;
	INT32	_strobe;
	INT32	_strobe_count;
	INT32	_size_x_p2;
	INT32	_size_y_p2;
	INT32	_size_x;
	INT32	_size_y;
	INT32	_size_x_ui;
	INT32	_size_y_ui;
	REAL	_tex_size_u;
	REAL	_tex_size_v;
	REAL	_grab_pos_x;
	REAL	_grab_pos_y;
	bool	_b_grab_alpha_ui;
	bool	_b_verbose_grab;
	bool	_b_smooth_ui;
//	bool	_b_smooth;
	bool	_b_mipmap_generate_ui;

	bool	_b_grab_to_img_ui;
	INT32	_grab_to_img_size_x;
	INT32	_grab_to_img_size_y;
	bool	_b_grab_to_img_avoid_gpu_transfer_ui;

	bool	_b_draw_ui;
	INT32	_draw_nb[3];
	REAL	_draw_space;
	REAL	_center[3];
	REAL	_rotation[3];
	REAL	_scale_ui[4];
	REAL	_scale[3];

	INT32	_seed;
	REAL	_seed_depth_ui;
//	REAL	seed_depth;
//	bool	b_seed_smooth;
	REAL	_seed_width;
	FP32	_seed_color[5];
public:

	virtual	void	param_init_pt();
			void	init();

	void	grab();
	void	draw_squares( REAL* pos_in, REAL* rot, REAL* size);
	void	draw_low();
	void	draw_seed();

	virtual	void	draw();
	virtual	void	update();

	virtual bool	can_implicit() final override { return true; }
};
