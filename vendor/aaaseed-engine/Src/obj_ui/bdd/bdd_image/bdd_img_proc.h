
#ifdef AAA_BDD_IMG_PROC_H
#error "BDD_IMG_PROC_H included more than once."
#endif
#define AAA_BDD_IMG_PROC_H 1


#ifndef AAA_BDD_H
#	include "obj_ui/bdd/util/bdd.h"
#endif
#ifndef AAA_FN_3D_H
#	include "obj_ui/fn_3d.h"
#endif

class c_img_2d;

class	c_bdd_img_proc final : public c_bdd
{
	FACTORY_DECLARE(c_bdd_img_proc,c_bdd);
private:
	REAL		factor;

	REAL		rx;
	REAL		ry;
	REAL		ra;

	REAL		sax;
	REAL		sfx;
	REAL		say;
	REAL		sfy;

	INT32		size_pixel[2];

	REAL		size[3];
	REAL		size_ui[4];

	REAL		cx;
	REAL		cy;

	INT32		type;
	bool		_b_alpha_generate;
	bool		_b_flip;
	REAL		move_green;
	REAL		move_blue;
	REAL		move_alpha;

	bool		_b_do_rand;

	bool		_b_pal;
	INT32		pal_bind;
	UINT8		pal[256][4];

	c_fn_3d		fn1;
	c_fn_3d		fn2;
	c_fn_3d*	pt_fn1;
	c_fn_3d*	pt_fn2;
protected:
public:

	virtual	void	param_init_pt();

	virtual	void	draw();
	virtual	void	update();
	virtual	AAA_ERR	load_do_after( o_str CONST & filename_in );
	virtual	AAA_ERR	save_do_after( o_str CONST & filename_in );

	FINLINE	REAL	compute_3fv_low( REAL* vec);
			REAL	compute_3fv( REAL* vec);
			void	compute( c_img_2d* img);
	void	rand();

};

extern	c_bdd_img_proc	*bdd_img_proc_cur;
