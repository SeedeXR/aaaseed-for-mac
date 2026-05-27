
#ifdef AAA_BDD_IMG_SUBSTRATE_H
#error "BDD_IMG_SUBSTRATE_H included more than once."
#endif
#define AAA_BDD_IMG_SUBSTRATE_H 1


#ifndef AAA_BDD_H
#	include "obj_ui/bdd/util/bdd.h"
#endif

class c_img_2d;
class c_crack;

class	c_bdd_img_substrate final : public c_bdd
{
	FACTORY_DECLARE( c_bdd_img_substrate, c_bdd);
public:
private:
	INT32	_size_pixel[2];
	INT32	_size_pixel_ui[2];
	INT32	_bind_last;

	INT32	_size_grid[2];

	bool	_b_texture_size_min;
	bool	_b_restart_trig_ui;
	bool	_b_img_erase_trig;

	c_img_2d*	_img;
	UINT32*	_grid;

	INT32	_crack_seed_nb;
	INT32	_move_nb;
	bool	_b_move;

	typedef  c_crack*	p_crack;

	p_crack*	_hd_crack_a;
	p_crack*	_hd_crack_a_cur;
	p_crack*	_hd_crack_a_max;

	p_crack*	_hd_crack_b;
	p_crack*	_hd_crack_b_cur;
	p_crack*	_hd_crack_b_max;

	p_crack		_crack;

//	REAL	_color_draw[4];

	FP32	_color_back_ui[5];
	FP32	_color_draw_ui[5];

private:
	//todo
	//FINLINE REAL	make_u( INT32 iu, REAL su );
	//FINLINE REAL	make_v( INT32 iv, REAL sv );
			void	alloc_grid();
			void	init_grid();
			void	sync_stupid();
public:
			void	dealloc_crack();
			void	alloc_crack();
			void	init_crack();
			void	swap_crack();

			void	init();
	
	virtual bool	can_implicit() final override;
	virtual	void	draw();
	virtual	void	update();

			void	erase_img(c_img_2d* img);
			void	compute();

	virtual	void	param_init_pt();
};
