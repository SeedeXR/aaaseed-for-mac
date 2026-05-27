
#ifdef AAA_BDD_IMG_ANALYSE_H
#error "BDD_IMG_ANALYSE_H included more than once."
#endif
#define AAA_BDD_IMG_ANALYSE_H 1


#ifndef AAA_BDD_IMG_H
#	include "obj_ui/bdd/bdd_image/bdd_img.h"
#endif

class	c_bdd_img_analyse final : public c_bdd_img_base
{
	FACTORY_DECLARE( c_bdd_img_analyse, c_bdd_img_base );
private:
	INT32			_src_img_index			{0};

	bool			_b_force_compute_ui;	//todo move to bdd_img_base

	REAL			_coverage_raw_out;
	REAL			_coverage_out;
	REAL			_center[2];

	INT32			_min_pixel[2];
	REAL			_min[2];
	INT32			_max_pixel[2];
	REAL			_max[2];
	INT32			_size_pixel[2];
	REAL			_size[2];


	REAL			_anal_begin[2];
	REAL			_anal_size[2];

	REAL			_anal_begin_ui[2];
	REAL			_anal_end_ui[2];
	REAL			_anal_size_ui[2];
	bool			_b_by_size_ui;
	bool			_b_anal_discrete_ui;
	INT32			_anal_step_nb_ui[2];	//	we don't use 0/u for the moment
;
	
	bool			_b_find_max;
	aaa::COMPO		_compo_src_ui;
	REAL			_find_max_refine_range;
	REAL			_find_max_skew_y;
	REAL			_max_pos[2];
	REAL			_max_max;
	REAL			_max_coverage;

	bool			_b_find_rect_ui;
	REAL			_coverage_min;
	REAL			_coverage_max;

	bool			_b_draw_texture;
	bool			_b_draw_frame;
	bool			_b_draw_frame_sub;
	bool			_b_draw_crosshair;
	bool			_b_draw_center;

	REAL			_threshold_min_ui;
	REAL			_threshold_max_ui;

			bool	compute(	c_img_2d* img,	aaa::COMPO src_compo	);
			bool	find_max(	c_img_2d* img,	aaa::COMPO src_compo	);
protected:
public:
	virtual	void	param_init_pt();

	virtual	void	draw();
	virtual	void	update();

	virtual bool	can_implicit() final override { return true; }
};
		