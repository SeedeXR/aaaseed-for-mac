
#ifdef AAA_BDD_FLUID_H
#error "BDD_FLUID_H included more than once."
#endif
#define AAA_BDD_FLUID_H 1


#ifndef AAA_BDD_H
#	include "obj_ui/bdd/util/bdd.h"
#endif
#ifndef AAA_DELTA_T_H
#	include "time/delta_t.h"
#endif

#if AAAMSA
	namespace MSA
	{
		class FluidSolver;
		class FluidDrawer;
	};
#else
	class ofxMSAFluidDrawer;
	class ofxMSAFluidSolver;
#endif
class c_img_2d;

class	c_bdd_fluid final : public c_bdd_multiple 
{
	FACTORY_DECLARE( c_bdd_fluid, c_bdd_multiple );
private:
	bool		_b_restart_trig_ui;
	bool		_b_randomize_trig;
	INT32		_img_dst_bind;
	bool		_b_rgb;
	bool		_b_alpha;
	bool		_b_alpha_flip;
	INT32		_s_render;

	INT32		_size_x;
	INT32		_size_y;

	bool		_b_inject_mouse;
	bool		_b_inject_blob;
	bool		_b_inject_color;
	bool		_b_inject_speed;
	REAL		_x_last;
	REAL		_y_last;
	REAL		_inject_size[4];
	REAL		_inject_speed_factor;
	REAL		_inject_color_mouse_freq;
	REAL		_inject_color_blob_freq;
	REAL		_inject_color_factor;
	REAL		_fade_speed;
	bool		_b_wrap_x;
	bool		_b_wrap_y;

	REAL		_time_factor;
	bool		_b_vorticity_confinement;
	INT32		_solver_iteration_nb;
	REAL		_viscosity;
	REAL		_color_diffusion;

	FP32		_rgb[3];	

//	REAL		_speed[2];

	bool		_b_smoke_src;
	bool		_b_img_src;
	INT32		_img_src_bind_ui;
	REAL		_img_threshold;
	FP32		_img_color_factor;
	bool		_b_img_use_mouse_color;
	bool		_b_gradient_src;
	INT32		_gradient_src_bind_ui;
	bool		_b_gradient_linear_ui;
	REAL		_gradient_factor;
	REAL		_justify[2];
	bool		_b_clamped_ui;
	REAL		_scale[3];


	c_delta_t	_delta_t;

#if AAAMSA
	MSA::FluidDrawerGl*	_drawer;
	MSA::FluidSolver*	_solver;
#else
	ofxMSAFluidDrawer*	_drawer;
	ofxMSAFluidSolver*	_solver;
#endif
private:
	void	add_blob( REAL x, REAL y, REAL dx, REAL dy, REAL size_x=1., REAL size_y=1. );
public:
	void	init();
public:
	virtual	void	restart();

	virtual	void	param_init_pt();

	virtual	void	update();
	virtual	void	draw_single();
	virtual	void	draw_multiple();

			void	fill_rgb_from( c_img_2d* src );
			void	fill_speed_from( c_img_2d* src );
/*
	virtual	void	draw_normal_point( REAL len );

	virtual	INT32	get_point_nb() final override;
	virtual	REAL*	get_point_pt( INT32 CONST index ) final override;
*/
};


