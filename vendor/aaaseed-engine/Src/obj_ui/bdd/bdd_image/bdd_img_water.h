
#ifdef AAA_BDD_IMG_WATER_H
#error "BDD_IMG_WATER_H included more than once."
#endif
#define AAA_BDD_IMG_WATER_H 1


#ifndef AAA_BDD_H
#	include "obj_ui/bdd/util/bdd.h"
#endif
#ifndef AAA_DELTA_T_H
#	include "time/delta_t.h"
#endif
#ifndef AAA_IMG_H
#	include "image/img.h"
#endif

class	c_bdd_img_water final : public c_bdd
{
FACTORY_DECLARE(c_bdd_img_water,c_bdd);
private:
	bool		_b_img_clamped_ui;
	
//	REAL		_wave_speed;
	REAL		_weighting_ui[2];	//todoopt make two separate variables
	c_delta_t	_delta_t;

	REAL*		_data;
	REAL*		_data_prev;
	REAL*		_data_prev_prev;

	INT32		_grid_size_x;
	INT32		_grid_size_y;
	INT32		_grid_size_x_ui;
	INT32		_grid_size_y_ui;

	bool		_b_compute_ui;
	bool		_b_restart_trig_ui;
	REAL		_damping_ui;

	bool		_b_img_update_ui;
	INT32		_img_src_index_ui;
	aaa::COMPO	_s_img_src_compo_ui;
	REAL		_img_influence_ui;

	bool		_b_method_complex_ui;
	REAL		_img_threshold_ui;

	REAL		_maa_factor_ui;

	bool		_b_gradient_ui;
	REAL		_gradient_factor;
	REAL		_value_factor_ui;

//	INT32		sample_nb;

	REAL		_out_min;
	REAL		_out_max;
	REAL		_clamp_min_ui;
	REAL		_clamp_max_ui;

	bool		_b_drop_ui;
	REAL		_drop_by_sec_ui;
	REAL		_drop_strenght_ui;

	bool		_b_img_out_float_ui;
	bool		_b_img_out_with_alpha_ui;

	REAL		_inter_ui;

	REAL		_drop_left;
	REAL		_conv_factor_ui[4];

	void		init_water_simulation_data();
	bool		alloc_water_simulation_data( INT32 size_x, INT32 size_y );
	void		dealloc_water_simulation_data();


protected:
			void	compute();
			void	import_image( c_img_2d* img, REAL* dst );
			void	do_drop( REAL* dst );
			void	move_to_tex( c_img_2d* img, REAL* p );

public:
//	virtual	void	draw();
	virtual	void	update();
	virtual	void	param_init_pt();
			void	init();

};

//extern	c_bdd_img_water*	bdd_img_water_cur;
