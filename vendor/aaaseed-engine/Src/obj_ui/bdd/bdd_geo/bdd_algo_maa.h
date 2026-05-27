
#ifdef AAA_BDD_ALGO_MAA_H
#error "BDD_ALGO_MAA_H included more than once."
#endif
#define AAA_BDD_ALGO_MAA_H 1


#ifndef AAA_BDD_H
#	include "obj_ui/bdd/util/bdd.h"
#endif

class	c_bdd_algo_maa final : public c_bdd_multiple
{
	FACTORY_DECLARE(c_bdd_algo_maa,c_bdd_multiple);
private:
	REAL	_point_nb_as_real;
	INT32	_point_nb;
//	INT32	point_computed_;
	REAL	_step_ui[4];
	REAL	_step[3];
//	REAL	step_last_[3];
	bool	_b_step_random;

	bool	_b_test;

	REAL	_start[3];
//	REAL	start_last_[3];
	REAL	_stop_out[3];
	INT32	_seed;
//	INT32	seed_last_;

	REAL*	_point;
	REAL*	_point_deformed;
	REAL*	_point_to_draw;
	bool*	_b_draw;
	INT32	_point_allocated;
	INT32	_point_allocated_ui;

	//	Box
	INT32	_s_box_type;
	REAL	_box_size_ui[3];
	REAL	_box_size[3];
	REAL	_box_min[3];
	REAL	_box_max[3];
	REAL	_box_origin[3];
	REAL	_box_size_factor;

public:

	virtual	void	draw_single();
	virtual	void	update();

	virtual	INT32	get_point_nb() final override;
	virtual	REAL*	get_points() final override;
	virtual	REAL*	get_point_pt( INT32 CONST index ) final override;

			void	alloc_point();
			void	dealloc_point();
			void	compute_point();

	virtual	void	save_obj_file( FILE* file );

	virtual	void	param_init_pt();
};
