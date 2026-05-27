
#ifdef AAA_BDD_TERRAIN_H
#error "BDD_TERRAIN_H included more than once."
#endif
#define AAA_BDD_TERRAIN_H 1


#ifndef AAA_BDD_H
#	include "obj_ui/bdd/util/bdd.h"
#endif

class c_rand;

class	c_bdd_terrain final : public c_bdd 
{
	FACTORY_DECLARE(c_bdd_terrain,c_bdd);
private:
	struct st_pave
	{
		REAL	pos[3];
		REAL	nor[3];
		FP32	col[3];
		REAL	z;
	};

	REAL			_center[3];
	REAL			_center_ui[3];
	REAL			_size_last[3];
	REAL			_size[3];
	struct st_pave*	_data;
	INT32			_i_max;
	INT32			_n_max;

	INT32			_order;
	INT32			_order_ui;
	bool			_b_show_rnd;
	bool			_b_show_rnd_ui;

	bool			_b_draw_color;

	bool			_b_need_build;
	bool			_b_normal_done;
	bool			_b_color_done;

	INT32			_s_rand;
	INT32			s_rand_ui_;

	REAL			_sea_level;
	REAL			_sea_level_ui;

	REAL			_dim_min;
	REAL			_dim_max;
	REAL			_dim_min_ui;
	REAL			_dim_max_ui;

	REAL			_dim_freq;
	REAL			_dim_freq_ui;

	bool			_b_addition;
	bool			_b_addition_ui;

//	bool			b_psyche_;
//	bool			b_psyche_ui_;

	c_rand*			_rand_cur;
	REAL			zmin_;
	REAL			zmax_;

	INT32			_seed_ui;
	INT32			_seed;
	REAL			_progression;
	REAL			_progression_ui;
	INT32			dd_cur_;

	FINLINE	struct	st_pave*	TERRE( INT32 x, INT32 y)
		{
		return	_data+x*_i_max+y;
		}
private:
	void	fill();
	void	make_rnd();
	void	make();
	void	switch_rand();
	void	build();
	void	find_min_max();
	REAL	make_wave( REAL pos[3] );
	void	make_color(int f_z);
	void	make_normal();
	void	draw_curves();
	void	draw_low_intermediate(INT32 f_clear);
	void	draw_low();
public:

			void	init();
			void	alloc(); 
			void	dealloc();


	virtual	void	param_init_pt();

	virtual	void	update();
	virtual	void	draw();

	virtual bool	can_implicit() final override;

	virtual	INT32	get_point_nb() final override;
	virtual	REAL*	get_point_pt( INT32 CONST index ) final override;
};
